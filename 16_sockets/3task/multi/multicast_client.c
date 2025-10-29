#include "multi.h"

int
main (void)
{
  int sock;
  struct sockaddr_in addr;
  struct ip_mreqn mreqn;
  char buffer[1024];

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  int reuse = 1;
  if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof (reuse)) < 0)
    {
      perror ("setsockopt(SO_REUSEADDR)");
      close (sock);
      exit (EXIT_FAILURE);
    }

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons (PORT);

  if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
      perror ("bind");
      close (sock);
      exit (EXIT_FAILURE);
    }

  memset (&mreqn, 0, sizeof (mreqn));
  mreqn.imr_multiaddr.s_addr = inet_addr (MULTICAST_GROUP);
  mreqn.imr_address.s_addr = INADDR_ANY;
  mreqn.imr_ifindex = 0;

  if (setsockopt (sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreqn, sizeof (mreqn))
      < 0) //сетевой уровень
    {
      perror ("setsockopt(IP_ADD_MEMBERSHIP)");
      close (sock);
      exit (EXIT_FAILURE);
    }

  printf ("Listening for multicast messages on %s:%d...\n", MULTICAST_GROUP,
          PORT);

  while (1)
    {
      ssize_t len
          = recvfrom (sock, buffer, sizeof (buffer) - 1, 0, NULL, NULL);
      if (len > 0)
        {
          buffer[len] = '\0';
          printf ("Received multicast: %s\n", buffer);
        }
    }

  close (sock);
  return 0;
}