#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

int
main ()
{
  int s = socket (AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (s == -1)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_in addr
      = { .sin_family = AF_INET, .sin_port = htons (8888) };
  inet_pton (AF_INET, "127.0.0.1", &addr.sin_addr);

  char packet[1024];
  char *str = "hi regular!";
  struct udphdr *udph = (struct udphdr *)packet;
  char *payload = packet + sizeof (struct udphdr);
  int len = sizeof (struct udphdr) + strlen (str) + 1;
  memcpy (payload, "hi regular!", strlen (str) + 1);
  udph->source = htons (7777);
  udph->dest = htons (8888);
  udph->len = htons (len);
  udph->check = 0;

  if (sendto (s, packet, sizeof (struct udphdr) + len, 0,
              (struct sockaddr *)&addr, sizeof (addr))
      == -1)
    {
      perror ("sendto");
      exit (EXIT_FAILURE);
    }

  while (1)
    {
      char buf[1024];

      int bytes_received = recv (s, buf, sizeof (buf), 0);
      if (bytes_received == -1)
        {
          perror ("recvfrom");
          exit (EXIT_FAILURE);
        }

      struct iphdr *iph = (struct iphdr *)buf;
      struct udphdr *udph = (struct udphdr *)(buf + iph->ihl * 4);

      if (iph->protocol != IPPROTO_UDP)
        continue;
      if (ntohs (udph->dest) != 7777)
        continue;
      if (ntohs (udph->source) != 8888)
        continue;

      printf ("payload: %s\n", buf + iph->ihl * 4 + sizeof (struct udphdr));
      break;
    }

  close (s);
}