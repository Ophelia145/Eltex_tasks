#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
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

  //разрешение юзать кастомный IP хэдэр
  int opt = 1;
  if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, &opt, sizeof (opt)) == -1)
    {
      perror ("setsockopt");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons (8888),
  };
  inet_pton (AF_INET, "127.0.0.1", &addr.sin_addr);

  char packet[1024];
  memset (packet, 0, sizeof (packet));

  struct iphdr *iph = (struct iphdr *)packet;
  struct udphdr *udph = (struct udphdr *)(packet + sizeof (struct iphdr));
  char *payload = packet + sizeof (struct iphdr) + sizeof (struct udphdr);
  const char *msg = "hi regular!";

  strcpy (payload, msg);

  int payload_len = strlen (msg) + 1;
  int udp_len = sizeof (struct udphdr) + payload_len;
  int total_len = sizeof (struct iphdr) + udp_len;

  // IP хэдэр
  iph->ihl = 5; //*4=20
  iph->version = 4;
  iph->tos = 0;
  iph->tot_len = htons (total_len);
  iph->id = 0; //ниче не фрагментируем
  iph->frag_off = 0;
  iph->ttl = 255; //почему бы и нет
  iph->protocol = 17;
  iph->check = 0;
  iph->saddr = inet_addr ("127.0.0.1");
  iph->daddr = inet_addr ("127.0.0.1");

  udph->source = htons (7777);
  udph->dest = htons (8888);
  udph->len = htons (udp_len);
  udph->check = 0;

  if (sendto (s, packet, total_len, 0, (struct sockaddr *)&addr, sizeof (addr))
      == -1)
    {
      perror ("sendto");
      exit (EXIT_FAILURE);
    }

  printf ("Packet sent: %s\n", msg);

  while (1)
    {
      char buf[1024];
      int bytes_received = recv (s, buf, sizeof (buf), 0);
      if (bytes_received == -1)
        {
          perror ("recv");
          exit (EXIT_FAILURE);
        }

      struct iphdr *iph_r = (struct iphdr *)buf;
      if (iph_r->protocol != IPPROTO_UDP)
        continue;

      struct udphdr *udph_r = (struct udphdr *)(buf + iph_r->ihl * 4);
      if (ntohs (udph_r->dest) != 7777)
        continue;
      if (ntohs (udph_r->source) != 8888)
        continue;

      char *recv_payload = buf + iph_r->ihl * 4 + sizeof (struct udphdr);
      printf ("server replied: %s\n", recv_payload);
      break;
    }

  close (s);
  return 0;
}
