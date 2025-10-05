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

#define SRV_PORT 1414
#define CLI_PORT 1313
#define MSG_MAX 256

volatile sig_atomic_t stop_flag = 0;

void
sig_h (int s)
{
  (void)s;
  stop_flag = 1;
}

static uint16_t
chk_ip (void *hdr)
{
  uint32_t sum = 0;
  uint16_t *p = hdr;
  for (int i = 0; i < 10; i++)
    sum += ntohs (p[i]);
  while (sum >> 16)
    sum = (sum & 0xFFFF) + (sum >> 16);
  return htons (~sum);
}

struct snd_args
{
  int sock;
  char *pkt;
  struct sockaddr_ll dst;
};
struct rcv_args
{
  int sock;
  in_addr_t cli;
  in_addr_t srv;
};

int
get_net_info (char *ipbuf, size_t iplen, char *ifbuf, size_t iflen)
{
  struct ifaddrs *ifa, *iflist;
  int f = 0;
  if (getifaddrs (&iflist) == -1)
    {
      perror ("getifaddrs");
      return -1;
    }
  for (ifa = iflist; ifa; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET
          && !(ifa->ifa_flags & IFF_LOOPBACK))
        {
          struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
          if (inet_ntop (AF_INET, &sa->sin_addr, ipbuf, iplen))
            {
              strncpy (ifbuf, ifa->ifa_name, iflen - 1);
              ifbuf[iflen - 1] = '\0';
              f = 1;
              break;
            }
        }
    }
  freeifaddrs (iflist);
  return f ? 0 : -1;
}

int
get_mac (const char *ifn, unsigned char *mac)
{
  struct ifaddrs *ifa, *list;
  int f = 0;
  if (getifaddrs (&list) == -1)
    {
      perror ("getifaddrs");
      return -1;
    }
  for (ifa = list; ifa; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_PACKET
          && strcmp (ifa->ifa_name, ifn) == 0)
        {
          struct sockaddr_ll *s = (struct sockaddr_ll *)ifa->ifa_addr;
          memcpy (mac, s->sll_addr, 6);
          f = 1;
          break;
        }
    }
  freeifaddrs (list);
  return f ? 0 : -1;
}

void *
snd_thr (void *a)
{
  struct snd_args *args = (struct snd_args *)a;
  char buf[MSG_MAX];
  while (!stop_flag)
    {
      printf ("enter text or 'q': ");
      fgets (buf, sizeof (buf), stdin);
      buf[strcspn (buf, "\n")] = 0;
      if (strcmp (buf, "q") == 0)
        stop_flag = 1;
      int pl_len = strlen (buf);
      struct iphdr *ip
          = (struct iphdr *)(args->pkt + sizeof (struct ether_header));
      ip->tot_len
          = htons (sizeof (struct iphdr) + sizeof (struct udphdr) + pl_len);
      ip->check = 0;
      ip->check = chk_ip (ip);
      struct udphdr *udp
          = (struct udphdr *)(args->pkt + sizeof (struct ether_header)
                              + sizeof (struct iphdr));
      memcpy ((char *)(udp + 1), buf, pl_len + 1);
      udp->len = htons (sizeof (struct udphdr) + pl_len);
      int totlen = sizeof (struct ether_header) + sizeof (struct iphdr)
                   + sizeof (struct udphdr) + pl_len;
      if (sendto (args->sock, args->pkt, totlen, 0,
                  (struct sockaddr *)&args->dst, sizeof (args->dst))
          == -1)
        {
          perror ("sendto");
          break;
        }
      printf ("sent: %s\n", buf);
      usleep (500);
    }
  return NULL;
}

void *
rcv_thr (void *a)
{
  struct rcv_args *args = (struct rcv_args *)a;
  int sock = args->sock;
  char buf[MSG_MAX];
  while (!stop_flag)
    {
      int n = recv (sock, buf, sizeof (buf), 0);
      if (n == -1)
        {
          if (errno != EAGAIN && errno != EWOULDBLOCK)
            {
              perror ("recv");
              break;
            }
          continue;
        }
      struct ether_header *eth = (struct ether_header *)buf;
      if (ntohs (eth->ether_type) != ETHERTYPE_IP)
        continue;
      struct iphdr *ip = (struct iphdr *)(buf + sizeof (struct ether_header));
      if (ip->protocol != IPPROTO_UDP)
        continue;
      if (ip->daddr != args->cli || ip->saddr != args->srv)
        continue;
      struct udphdr *udp = (struct udphdr *)(buf + sizeof (struct ether_header)
                                             + ip->ihl * 4);
      if (ntohs (udp->dest) != CLI_PORT || ntohs (udp->source) != SRV_PORT)
        continue;
      char *pl = (char *)(udp + 1);
      pl[ntohs (udp->len) - sizeof (struct udphdr)] = '\0';
      printf ("received: %s\n", pl);
    }
  return NULL;
}

int
main ()
{
  int s = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (s == -1)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }
  struct timeval tv = { 1, 0 };
  if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) < 0)
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  char myip[INET_ADDRSTRLEN] = { 0 }, iface[IFNAMSIZ] = { 0 };
  if (get_net_info (myip, sizeof (myip), iface, sizeof (iface)) != 0)
    {
      fprintf (stderr, "no ip/interface\n");
      close (s);
      exit (EXIT_FAILURE);
    }
  printf ("iface: %s, ip: %s\n", iface, myip);

  unsigned char src_mac[6];
  if (get_mac (iface, src_mac) != 0)
    {
      fprintf (stderr, "no MAC\n");
      close (s);
      exit (EXIT_FAILURE);
    }

  char pkt[1024] = { 0 };
  struct ether_header *eth = (struct ether_header *)pkt;
  unsigned char dst_mac[6] = { 0x00, 0x10, 0x11, 0x12, 0x13, 0x14 };
  char dst_ip[INET_ADDRSTRLEN] = "10.10.10.20";
  memcpy (eth->ether_shost, src_mac, 6);
  memcpy (eth->ether_dhost, dst_mac, 6);
  eth->ether_type = htons (ETHERTYPE_IP);

  struct iphdr *ip = (struct iphdr *)(pkt + sizeof (struct ether_header));
  ip->version = 4;
  ip->ihl = 5;
  ip->tos = 0;
  ip->id = 0;
  ip->frag_off = 0;
  ip->ttl = 64;
  ip->protocol = IPPROTO_UDP;
  ip->check = 0;
  inet_pton (AF_INET, myip, &ip->saddr);
  inet_pton (AF_INET, dst_ip, &ip->daddr);

  struct udphdr *udp = (struct udphdr *)(pkt + sizeof (struct ether_header)
                                         + sizeof (struct iphdr));
  udp->source = htons (CLI_PORT);
  udp->dest = htons (SRV_PORT);
  udp->check = 0;

  struct sockaddr_ll sll = { 0 };
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex (iface);
  sll.sll_halen = ETH_ALEN;
  memcpy (sll.sll_addr, dst_mac, 6);
  sll.sll_protocol = htons (ETH_P_IP);

  struct snd_args sa = { .sock = s, .pkt = pkt, .dst = sll };
  struct rcv_args ra = { .sock = s };
  inet_pton (AF_INET, myip, &ra.cli);
  inet_pton (AF_INET, dst_ip, &ra.srv);

  pthread_t tid_s, tid_r;
  if (pthread_create (&tid_s, NULL, snd_thr, &sa) != 0)
    {
      perror ("pthread sender");
      exit (EXIT_FAILURE);
    }
  if (pthread_create (&tid_r, NULL, rcv_thr, &ra) != 0)
    {
      perror ("pthread receiver");
      pthread_cancel (tid_s);
      exit (EXIT_FAILURE);
    }

  pthread_join (tid_s, NULL);
  pthread_join (tid_r, NULL);
  close (s);
}
