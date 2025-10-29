#include "multi.h"

int
main (void)
{
  int sock;
  struct sockaddr_in addr;
  char msg[] = "hi group";

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr (MULTICAST_GROUP);
  addr.sin_port = htons (PORT);

  while (1)
    {
      ssize_t sent = sendto (sock, msg, strlen (msg), 0,
                             (struct sockaddr *)&addr, sizeof (addr));
      if (sent < 0)
        perror ("sendto");
      else
        printf ("multicast sent: %s\n", msg);
      sleep (2);
    }

  close (sock);
  return 0;
}
