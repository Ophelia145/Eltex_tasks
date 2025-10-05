#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#define __USE_MISC 1
#include <ifaddrs.h>
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

#define MAX_CLIENTS 7
#define SIZE_BUF 1024
#define REPLY_SIZE 256
#define QUIT "q"
#define SERVER_PORT 1414

volatile sig_atomic_t stop_flag = 0;

void
handle_sigint (int sig)
{
  (void)sig;
  stop_flag = 1;
}

typedef struct
{
  struct in_addr ip;
  uint16_t port;
  int counter;
  int used;
} client_info;

client_info clients[MAX_CLIENTS];

int
client_equal (struct in_addr *ip1, uint16_t port1, struct in_addr *ip2,
              uint16_t port2)
{
  return ip1->s_addr == ip2->s_addr && port1 == port2;
}

int
get_ind_client (struct in_addr *ip, uint16_t port)
{
  for (int i = 0; i < MAX_CLIENTS; ++i)
    {
      if (clients[i].used
          && client_equal (&clients[i].ip, clients[i].port, ip, port))
        {
          return i;
        }
    }
  for (int i = 0; i < MAX_CLIENTS; ++i)
    {
      if (!clients[i].used)
        {
          clients[i].ip = *ip;
          clients[i].port = port;
          clients[i].counter = 0;
          clients[i].used = 1;
          return i;
        }
    }
  return -1;
}

void
reset_client (int idx)
{
  clients[idx].used = 0;
  clients[idx].counter = 0;
  clients[idx].port = 0;
  memset (&clients[idx].ip, 0, sizeof (clients[idx].ip));
}

uint16_t
ip_checksum (void *header)
{
  uint32_t sum = 0;
  uint16_t *ptr = (uint16_t *)header;

  for (int i = 0; i < 10; i++)
    {
      sum += ntohs (ptr[i]);
    }

  while (sum >> 16)
    {
      sum = (sum & 0xFFFF) + (sum >> 16);
    }

  return htons ((uint16_t)~sum);
}

int
get_ip_and_iface (char *ip_buf, size_t ip_buf_len, char *iface_buf,
                  size_t iface_buf_len)
{
  struct ifaddrs *ifaddr, *ifa;
  int found = 0;
  if (getifaddrs (&ifaddr) == -1)
    {
      perror ("getifaddrs");
      return -1;
    }
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET
          && !(ifa->ifa_flags & IFF_LOOPBACK))
        {
          struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
          if (inet_ntop (AF_INET, &sa->sin_addr, ip_buf, ip_buf_len))
            {
              strncpy (iface_buf, ifa->ifa_name, iface_buf_len - 1);
              iface_buf[iface_buf_len - 1] = '\0';
              found = 1;
              break;
            }
        }
    }
  freeifaddrs (ifaddr);
  return found ? 0 : -1;
}

int
make_answer_pack (char *packet, struct ether_header *received_eth,
                  struct iphdr *received_iph, struct udphdr *received_udph,
                  char *payload, int client_counter)
{
  struct ether_header *eth = (struct ether_header *)packet;

  memcpy (eth->ether_dhost, received_eth->ether_shost, 6);
  memcpy (eth->ether_shost, received_eth->ether_dhost, 6);

  eth->ether_type = htons (ETHERTYPE_IP);

  struct iphdr *iph = (struct iphdr *)(packet + sizeof (struct ether_header));
  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;

  char reply_msg[REPLY_SIZE];
  int reply_msg_len = snprintf (reply_msg, sizeof (reply_msg), "%s %d",
                                payload, client_counter);

  iph->tot_len
      = htons (sizeof (struct iphdr) + sizeof (struct udphdr) + reply_msg_len);
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;

  iph->saddr = received_iph->daddr;
  iph->daddr = received_iph->saddr;

  iph->check = 0;
  iph->check = ip_checksum (iph);

  struct udphdr *udph = (struct udphdr *)(packet + sizeof (struct ether_header)
                                          + sizeof (struct iphdr));

  udph->source = received_udph->dest;
  udph->dest = received_udph->source;

  udph->len = htons (sizeof (struct udphdr) + reply_msg_len);
  udph->check = 0;

  char *reply_payload = (char *)(udph + 1);
  memcpy (reply_payload, reply_msg, reply_msg_len);

  return sizeof (struct ether_header) + sizeof (struct iphdr)
         + sizeof (struct udphdr) + reply_msg_len;
}

int
main ()
{

  struct sigaction sa;
  sa.sa_handler = handle_sigint;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction (SIGINT, &sa, NULL);

  int s = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (s == -1)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_ll sll = { 0 };
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex ("eth0");
  sll.sll_protocol = htons (ETH_P_ALL);

  if (bind (s, (struct sockaddr *)&sll, sizeof (sll)) == -1)
    {
      perror ("bind");
      close (s);
      exit (EXIT_FAILURE);
    }

  printf (
      "raw socket server started on eth0. listening for udp packets on port "
      "1414\n");

  char buf[SIZE_BUF];

  struct in_addr server_ip;
  inet_pton (AF_INET, "10.10.10.20", &server_ip);

  while (!stop_flag)
    {
      int bytes_received = recv (s, buf, sizeof (buf), 0);
      if (bytes_received == -1)
        {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
              continue;
            }
          else
            {
              perror ("recv");
              break;
            }
        }

      struct ether_header *eth = (struct ether_header *)buf;
      struct iphdr *iph = (struct iphdr *)(buf + sizeof (struct ether_header));
      struct udphdr *udph
          = (struct udphdr *)(buf + sizeof (struct ether_header)
                              + iph->ihl * 4);

      if (bytes_received < (int)(sizeof (struct ether_header) + iph->ihl * 4
                                 + sizeof (struct udphdr)))
        continue;
      if (ntohs (eth->ether_type) != ETHERTYPE_IP)
        continue;
      if (iph->protocol != IPPROTO_UDP)
        continue;
      if (iph->daddr != server_ip.s_addr)
        continue;
      if (ntohs (udph->dest) != 1414)
        continue;

      char *payload = (char *)(udph + 1);
      uint16_t udp_len = ntohs (udph->len);
      uint16_t payload_len = udp_len - sizeof (struct udphdr);

      if (bytes_received
          < (int)(sizeof (struct ether_header) + iph->ihl * 4 + udp_len))
        {
          continue;
        }

      if (payload_len > 0)
        {
          int bytes_left_in_buf = SIZE_BUF - (payload - buf) - 1;
          int safe_payload_len = (payload_len < bytes_left_in_buf)
                                     ? payload_len
                                     : bytes_left_in_buf;
          payload[safe_payload_len] = '\0';
        }
      else
        {
          continue;
        }

      struct in_addr client_ip;
      client_ip.s_addr = iph->saddr;
      uint16_t client_port = ntohs (udph->source);

      printf ("received from %s:%d: %s\n", inet_ntoa (client_ip), client_port,
              payload);

      int idx = get_ind_client (&client_ip, udph->source);
      if (idx == -1)
        {
          fprintf (stderr, "enough clients, ignoring message from %s:%d\n",
                   inet_ntoa (client_ip), client_port);
          continue;
        }

      int is_quit = strcmp (payload, QUIT);
      if (is_quit == 0)
        {
          printf ("client counter reset %s:%d\n", inet_ntoa (client_ip),
                  client_port);
          reset_client (idx);
          continue;
        }

      clients[idx].counter++;

      char reply_packet[SIZE_BUF];
      memset (reply_packet, 0, sizeof (reply_packet));
      int packet_len = make_answer_pack (reply_packet, eth, iph, udph, payload,
                                         clients[idx].counter);

      struct sockaddr_ll dest_addr = { 0 };
      dest_addr.sll_family = AF_PACKET;
      dest_addr.sll_ifindex = if_nametoindex ("eth0");
      dest_addr.sll_halen = ETH_ALEN;
      memcpy (dest_addr.sll_addr, eth->ether_shost, 6);
      dest_addr.sll_protocol = htons (ETH_P_IP);

      if (sendto (s, reply_packet, packet_len, 0,
                  (struct sockaddr *)&dest_addr, sizeof (dest_addr))
          == -1)
        {
          perror ("sendto");
        }
      else
        {
          printf ("sent reply to %s:%d: %s %d\n", inet_ntoa (client_ip),
                  client_port, payload, clients[idx].counter);
        }
    }

  close (s);
  return 0;
}