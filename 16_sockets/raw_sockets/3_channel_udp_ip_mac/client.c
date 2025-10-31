#define _GNU_SOURCE
#include <arpa/inet.h>
#include <errno.h>
#include <net/ethernet.h> // ETH_P_IP
#include <net/if.h>       // struct ifreq
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netpacket/packet.h> // struct sockaddr_ll
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

uint16_t
ip_checksum (void *vdata, size_t length)
{
  char *data = (char *)vdata;

  ((uint16_t *)data)[5] = 0;

  uint32_t sum = 0;
  uint16_t *ptr = (uint16_t *)data;

  for (size_t i = 0; i < length / 2; i++)
    {
      sum += ntohs (ptr[i]);
    }

  //если перенос
  while (sum >> 16)
    {
      sum = (sum & 0xFFFF) + (sum >> 16);
    }

  return htons (~sum);
}

int
main ()
{

  const char *dst_ip_str = "10.10.10.10";
  int dst_port = 8888;
  int src_port = 7777;
  const char *msg = "hi regular!";

  int sockfd = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_ALL));
  if (sockfd == -1)
    {
      perror ("socket(AF_PACKET)");
      return EXIT_FAILURE;
    }

  struct ifreq ifr;
  memset (&ifr, 0, sizeof (ifr));
  strncpy (ifr.ifr_name, "eth0", IFNAMSIZ - 1);

  //индекс  src
  if (ioctl (sockfd, SIOCGIFINDEX, &ifr) == -1)
    {
      perror ("ioctl(SIOCGIFINDEX)");
      close (sockfd);
      return EXIT_FAILURE;
    }
  int ifindex = ifr.ifr_ifindex;

  // MAC интерфейса
  ioctl (sockfd, SIOCGIFHWADDR, &ifr);
  unsigned char src_mac[6];
  memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6);

  unsigned char dst_mac[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x00 };

  // IP интерфейса
  ioctl (sockfd, SIOCGIFADDR, &ifr);
  struct sockaddr_in *sin = (struct sockaddr_in *)&ifr.ifr_addr;
  uint32_t src_ip = sin->sin_addr.s_addr;

  // 14eth + 20ip + 8udp + payload
  unsigned char buffer[1500];
  memset (buffer, 0, sizeof (buffer));

  // ethernet
  struct ethhdr *eth = (struct ethhdr *)buffer;
  memcpy (eth->h_dest, dst_mac, 6);
  memcpy (eth->h_source, src_mac, 6);
  eth->h_proto = htons (ETH_P_IP);

  // ip
  struct iphdr *iph = (struct iphdr *)(buffer + sizeof (struct ethhdr));
  struct udphdr *udph = (struct udphdr *)(buffer + sizeof (struct ethhdr)
                                          + sizeof (struct iphdr));
  unsigned char *payload = buffer + sizeof (struct ethhdr)
                           + sizeof (struct iphdr) + sizeof (struct udphdr);

  size_t payload_len = strlen (msg) + 1;
  memcpy (payload, msg, payload_len);

  iph->ihl = 5;
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len
      = htons (sizeof (struct iphdr) + sizeof (struct udphdr) + payload_len);
  iph->id = htons (0x1234);
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  if (src_ip == 0)
    {

      iph->saddr = inet_addr ("10.10.10.11");
    }
  else
    {
      iph->saddr = src_ip;
    }
  iph->daddr = inet_addr (dst_ip_str);
  iph->check = ip_checksum (iph, sizeof (struct iphdr));

  udph->source = htons (src_port);
  udph->dest = htons (dst_port);
  udph->len = htons (sizeof (struct udphdr) + payload_len);
  udph->check = 0;

  size_t frame_len = sizeof (struct ethhdr) + sizeof (struct iphdr)
                     + sizeof (struct udphdr) + payload_len;

  struct sockaddr_ll sock_addr_ll;
  memset (&sock_addr_ll, 0, sizeof (sock_addr_ll));
  sock_addr_ll.sll_family = AF_PACKET;
  sock_addr_ll.sll_ifindex = ifindex;
  sock_addr_ll.sll_halen = ETH_ALEN;
  memcpy (sock_addr_ll.sll_addr, dst_mac, 6);

  ssize_t sent
      = sendto (sockfd, buffer, frame_len, 0, (struct sockaddr *)&sock_addr_ll,
                sizeof (sock_addr_ll));
  if (sent == -1)
    {
      perror ("sendto(AF_PACKET)");
      close (sockfd);
      return EXIT_FAILURE;
    }
  printf ("Sent Ethernet frame (%zd bytes) on %s -> IP %s UDP %d\n", sent,
          "eth0", dst_ip_str, dst_port);

  while (1)
    {
      unsigned char recvbuf[2048];
      ssize_t r = recv (sockfd, recvbuf, sizeof (recvbuf), 0);
      if (r == -1)
        {
          perror ("recv");
          break;
        }

      if ((size_t)r < sizeof (struct ethhdr) + sizeof (struct iphdr)
                          + sizeof (struct udphdr))
        continue;

      struct ethhdr *reth = (struct ethhdr *)recvbuf;
      if (ntohs (reth->h_proto) != ETH_P_IP)
        continue;

      struct iphdr *riph = (struct iphdr *)(recvbuf + sizeof (struct ethhdr));
      if (riph->protocol != IPPROTO_UDP)
        continue;

      struct udphdr *rudph = (struct udphdr *)(recvbuf + sizeof (struct ethhdr)
                                               + riph->ihl * 4);
      int rsrc = ntohs (rudph->source);
      int rdst = ntohs (rudph->dest);
      if (rdst != src_port)
        continue;
      if (rsrc != dst_port) // от сервера ждем dst port
        continue;

      char *rpayload = (char *)recvbuf + sizeof (struct ethhdr) + riph->ihl * 4
                       + sizeof (struct udphdr);
      printf ("Received reply: %s\n", rpayload);
      break;
    }

  close (sockfd);
  return 0;
}
