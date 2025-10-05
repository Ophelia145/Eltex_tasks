#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#define __USE_MISC 1
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netpacket/packet.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_C 7
#define BUF_SZ 1024
#define RESP_SZ 256
#define EXIT_CMD "q"
#define PORT_SRV 1414

volatile sig_atomic_t stop_flag = 0;

void
sig_handler (int signo)
{
  stop_flag = 1;
  (void)signo;
}

typedef struct
{
  struct in_addr addr;
  uint16_t port;
  int count;
  int active;
} Client;

Client cl_list[MAX_C];

static int
cmp_client (struct in_addr *a, uint16_t p1, struct in_addr *b, uint16_t p2)
{
  return a->s_addr == b->s_addr && p1 == p2;
}

static int
find_or_add_client (struct in_addr *ip, uint16_t port)
{
  for (int i = 0; i < MAX_C; ++i)
    if (cl_list[i].active
        && cmp_client (&cl_list[i].addr, cl_list[i].port, ip, port))
      return i;

  for (int i = 0; i < MAX_C; ++i)
    if (!cl_list[i].active)
      {
        cl_list[i].addr = *ip;
        cl_list[i].port = port;
        cl_list[i].count = 0;
        cl_list[i].active = 1;
        return i;
      }
  return -1;
}

static void
reset_client (int i)
{
  cl_list[i].active = 0;
  cl_list[i].count = 0;
  cl_list[i].port = 0;
  memset (&cl_list[i].addr, 0, sizeof (cl_list[i].addr));
}

static uint16_t
compute_checksum (void *hdr)
{
  uint32_t sum = 0;
  uint16_t *ptr = hdr;
  for (int i = 0; i < 10; ++i)
    sum += ntohs (ptr[i]);
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return htons (~sum);
}

int
make_response_packet (char *pkt, struct ether_header *eth_in,
                      struct iphdr *ip_in, struct udphdr *udp_in, char *msg,
                      int ctr)
{
  struct ether_header *eth_out = (struct ether_header *)pkt;
  memcpy (eth_out->ether_dhost, eth_in->ether_shost, 6);
  memcpy (eth_out->ether_shost, eth_in->ether_dhost, 6);
  eth_out->ether_type = htons (ETHERTYPE_IP);

  struct iphdr *ip_out = (struct iphdr *)(pkt + sizeof (struct ether_header));
  ip_out->version = 4;
  ip_out->ihl = 5;
  ip_out->tos = 0;
  char rmsg[RESP_SZ];
  int rlen = snprintf (rmsg, sizeof (rmsg), "%s %d", msg, ctr);
  ip_out->tot_len
      = htons (sizeof (struct iphdr) + sizeof (struct udphdr) + rlen);
  ip_out->id = 0;
  ip_out->frag_off = 0;
  ip_out->ttl = 64;
  ip_out->protocol = IPPROTO_UDP;
  ip_out->saddr = ip_in->daddr;
  ip_out->daddr = ip_in->saddr;
  ip_out->check = 0;
  ip_out->check = compute_checksum (ip_out);

  struct udphdr *udp_out = (struct udphdr *)(pkt + sizeof (struct ether_header)
                                             + sizeof (struct iphdr));
  udp_out->source = udp_in->dest;
  udp_out->dest = udp_in->source;
  udp_out->len = htons (sizeof (struct udphdr) + rlen);
  udp_out->check = 0;

  char *payload_out = (char *)(udp_out + 1);
  memcpy (payload_out, rmsg, rlen);

  return sizeof (struct ether_header) + sizeof (struct iphdr)
         + sizeof (struct udphdr) + rlen;
}

int
main ()
{
  struct sigaction sa = { 0 };
  sa.sa_handler = sig_handler;
  sigemptyset (&sa.sa_mask);
  sigaction (SIGINT, &sa, NULL);

  int sock = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (sock == -1)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_ll sll = { 0 };
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex ("eth0");
  sll.sll_protocol = htons (ETH_P_ALL);
  if (bind (sock, (struct sockaddr *)&sll, sizeof (sll)) == -1)
    {
      perror ("bind");
      close (sock);
      exit (EXIT_FAILURE);
    }

  printf ("raw server on eth0 listening udp:%d\n", PORT_SRV);

  char buffer[BUF_SZ];
  struct in_addr srv_ip;
  inet_pton (AF_INET, "10.10.10.20", &srv_ip);

  while (!stop_flag)
    {
      int n = recv (sock, buffer, sizeof (buffer), 0);
      if (n == -1)
        {
          if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
              perror ("recv");
              break;
            }
          continue;
        }

      struct ether_header *eth = (struct ether_header *)buffer;
      struct iphdr *iph
          = (struct iphdr *)(buffer + sizeof (struct ether_header));
      struct udphdr *udph
          = (struct udphdr *)(buffer + sizeof (struct ether_header)
                              + iph->ihl * 4);
      if (n < (int)(sizeof (struct ether_header) + iph->ihl * 4
                    + sizeof (struct udphdr)))
        continue;
      if (ntohs (eth->ether_type) != ETHERTYPE_IP)
        continue;
      if (iph->protocol != IPPROTO_UDP)
        continue;
      if (iph->daddr != srv_ip.s_addr)
        continue;
      if (ntohs (udph->dest) != PORT_SRV)
        continue;

      char *pl = (char *)(udph + 1);
      uint16_t pl_len = ntohs (udph->len) - sizeof (struct udphdr);
      if (pl_len > 0)
        {
          int safe_len = (pl_len < (BUF_SZ - (pl - buffer) - 1))
                             ? pl_len
                             : (BUF_SZ - (pl - buffer) - 1);
          pl[safe_len] = '\0';
        }
      else
        continue;

      struct in_addr cli_ip;
      cli_ip.s_addr = iph->saddr;
      uint16_t cli_port = ntohs (udph->source);
      printf ("received from %s:%d: %s\n", inet_ntoa (cli_ip), cli_port, pl);

      int idx = find_or_add_client (&cli_ip, udph->source);
      if (idx == -1)
        {
          fprintf (stderr, "too many clients, ignore %s:%d\n",
                   inet_ntoa (cli_ip), cli_port);
          continue;
        }

      if (strcmp (pl, EXIT_CMD) == 0)
        {
          reset_client (idx);
          printf ("client reset %s:%d\n", inet_ntoa (cli_ip), cli_port);
          continue;
        }

      cl_list[idx].count++;

      char reply_pkt[BUF_SZ] = { 0 };
      int pkt_len = make_response_packet (reply_pkt, eth, iph, udph, pl,
                                          cl_list[idx].count);

      struct sockaddr_ll dst = { 0 };
      dst.sll_family = AF_PACKET;
      dst.sll_ifindex = if_nametoindex ("eth0");
      dst.sll_halen = ETH_ALEN;
      memcpy (dst.sll_addr, eth->ether_shost, 6);
      dst.sll_protocol = htons (ETH_P_IP);

      if (sendto (sock, reply_pkt, pkt_len, 0, (struct sockaddr *)&dst,
                  sizeof (dst))
          == -1)
        perror ("sendto");
      else
        printf ("sent reply to %s:%d: %s %d\n", inet_ntoa (cli_ip), cli_port,
                pl, cl_list[idx].count);
    }

  close (sock);
  return 0;
}
