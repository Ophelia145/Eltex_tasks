#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 5000
#define BROADCAST_IP "255.255.255.255"

int
main (void)
{
  int sock;
  struct sockaddr_in addr;
  int opt = 1;
  char msg[] = "where r u";

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  if (setsockopt (sock, SOL_SOCKET, SO_BROADCAST, &opt, sizeof (opt)) < 0)
    {
      perror ("setsockopt");
      close (sock);
      exit (EXIT_FAILURE);
    }

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons (PORT);
  inet_pton (AF_INET, BROADCAST_IP, &addr.sin_addr);

  while (1)
    {
      ssize_t sent = sendto (sock, msg, strlen (msg), 0,
                             (struct sockaddr *)&addr, sizeof (addr));
      if (sent < 0)
        {
          perror ("sendto");
        }
      else
        {
          printf ("Broadcast sent: %s\n", msg);
        }
      sleep (2);
    }

  close (sock);
  return 0;
}
