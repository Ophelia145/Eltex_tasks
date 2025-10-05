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
#define SERVER_PORT 1414
#define CLIENT_PORT 1313
#define MAX_MESSAGE_LENGTH 256

volatile sig_atomic_t stop_flag = 0;

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

struct sender_args
{
  int sock;
  char *packet;
  struct sockaddr_ll sll;
};

struct receiver_args
{
  int sock;
  in_addr_t client_ip;
  in_addr_t server_ip;
};

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

void *
sender_thread (void *arg)
{
  struct sender_args *args = (struct sender_args *)arg;
  char input[MAX_MESSAGE_LENGTH];

  while (!stop_flag)
    {
      printf ("enter text or 'q' to quit: ");
      fgets (input, MAX_MESSAGE_LENGTH, stdin);
      input[strcspn (input, "\n")] = '\0';
      if (strcmp (input, "q") == 0)
        stop_flag = 1;

      int payload_len = strlen (input);

      struct iphdr *iph
          = (struct iphdr *)(args->packet + sizeof (struct ether_header));
      iph->tot_len = htons (sizeof (struct iphdr) + sizeof (struct udphdr)
                            + payload_len);
      iph->check = 0;
      iph->check = ip_checksum (iph);

      struct udphdr *udph
          = (struct udphdr *)(args->packet + sizeof (struct ether_header)
                              + sizeof (struct iphdr));
      char *payload = (char *)(udph + 1);
      memcpy (payload, input, payload_len + 1);

      udph->len = htons (sizeof (struct udphdr) + payload_len);

      int packet_tot_len = sizeof (struct ether_header) + sizeof (struct iphdr)
                           + sizeof (struct udphdr) + payload_len;

      if (sendto (args->sock, args->packet, packet_tot_len, 0,
                  (struct sockaddr *)&args->sll, sizeof (args->sll))
          == -1)
        {
          perror ("sendto");
          break;
        }
      printf ("sent: %s\n", input);
      usleep (500);
    }

  return NULL;
}

void *
receiver_thread (void *arg)
{
  struct receiver_args *args = (struct receiver_args *)arg;

  int sock = args->sock;
  in_addr_t client_ip = args->client_ip;
  in_addr_t server_ip = args->server_ip;
  char buf[MAX_MESSAGE_LENGTH];

  while (!stop_flag)
    {
      int bytes_received = recv (sock, buf, sizeof (buf), 0);
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
      if (ntohs (eth->ether_type) != ETHERTYPE_IP)
        continue;

      struct iphdr *iph = (struct iphdr *)(buf + sizeof (struct ether_header));
      if (iph->protocol != IPPROTO_UDP)
        continue;
      if (iph->daddr != client_ip)
        continue;
      if (iph->saddr != server_ip)
        continue;

      struct udphdr *udph = (struct udphdr *)((buf + iph->ihl * 4)
                                              + sizeof (struct ether_header));
      if (ntohs (udph->dest) != CLIENT_PORT)
        continue;
      if (ntohs (udph->source) != SERVER_PORT)
        continue;

      char *payload = (char *)(udph + 1);
      uint16_t udp_len = ntohs (udph->len);
      uint16_t payload_len = udp_len - sizeof (struct udphdr);
      payload[payload_len] = '\0';
      printf ("received: %s\n", payload);
    }

  return NULL;
}

int
get_mac_address (const char *ifname, unsigned char *mac)
{
  struct ifaddrs *ifaddr = NULL, *ifa = NULL;
  int found = 0;
  if (getifaddrs (&ifaddr) == -1)
    {
      perror ("getifaddrs");
      return -1;
    }
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
    {
      if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_PACKET
          && strcmp (ifa->ifa_name, ifname) == 0)
        {
          struct sockaddr_ll *s = (struct sockaddr_ll *)ifa->ifa_addr;
          memcpy (mac, s->sll_addr, 6);
          found = 1;
          break;
        }
    }
  freeifaddrs (ifaddr);
  return found ? 0 : -1;
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

  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;
  if (setsockopt (s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof (tv)) < 0)
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  char my_ip[INET_ADDRSTRLEN] = { 0 };
  char iface[IFNAMSIZ] = { 0 };
  if (get_ip_and_iface (my_ip, sizeof (my_ip), iface, sizeof (iface)) != 0)
    {
      fprintf (stderr, "didn't get ip and interface\n");
      close (s);
      exit (EXIT_FAILURE);
    }
  printf ("using interface: %s, ip: %s\n", iface, my_ip);

  unsigned char src_mac[6];
  if (get_mac_address (iface, src_mac) != 0)
    {
      fprintf (stderr, "didn't get MAC address for %s\n", iface);
      close (s);
      exit (EXIT_FAILURE);
    }

  char packet[1024] = { 0 };
  struct ether_header *eth = (struct ether_header *)packet;

  char dest_ip[INET_ADDRSTRLEN] = "10.10.10.20";
  unsigned char dest_mac[6] = { 0x00, 0x10, 0x11, 0x12, 0x13, 0x14 };

  memcpy (eth->ether_shost, src_mac, 6);
  memcpy (eth->ether_dhost, dest_mac, 6);
  eth->ether_type = htons (ETHERTYPE_IP);

  struct iphdr *iph = (struct iphdr *)(packet + sizeof (struct ether_header));
  iph->version = 4;
  iph->ihl = 5;
  iph->tos = 0;
  iph->id = 0;
  iph->frag_off = 0;
  iph->ttl = 64;
  iph->protocol = IPPROTO_UDP;
  iph->check = 0;
  inet_pton (AF_INET, my_ip, &iph->saddr);
  inet_pton (AF_INET, dest_ip, &iph->daddr);

  struct udphdr *udph = (struct udphdr *)(packet + sizeof (struct ether_header)
                                          + sizeof (struct iphdr));
  udph->source = htons (CLIENT_PORT);
  udph->dest = htons (SERVER_PORT);
  udph->check = 0;

  struct sockaddr_ll sll = { 0 };
  sll.sll_family = AF_PACKET;
  sll.sll_ifindex = if_nametoindex (iface);
  sll.sll_halen = ETH_ALEN;
  memcpy (sll.sll_addr, dest_mac, 6);
  sll.sll_protocol = htons (ETH_P_IP);

  struct sender_args tdata = { .sock = s, .packet = packet, .sll = sll };
  struct receiver_args receiver_args;
  receiver_args.sock = s;
  inet_pton (AF_INET, my_ip, &receiver_args.client_ip);
  inet_pton (AF_INET, dest_ip, &receiver_args.server_ip);

  pthread_t sender_tid, receiver_tid;
  if (pthread_create (&sender_tid, NULL, sender_thread, &tdata) != 0)
    {
      perror ("pthread_create sender");
      exit (EXIT_FAILURE);
    }
  if (pthread_create (&receiver_tid, NULL, receiver_thread, &receiver_args)
      != 0)
    {
      perror ("pthread_create receiver");
      pthread_cancel (sender_tid);
      exit (EXIT_FAILURE);
    }

  pthread_join (sender_tid, NULL);
  pthread_join (receiver_tid, NULL);
  close (s);
}