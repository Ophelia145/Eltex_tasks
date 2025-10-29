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
  char buffer[1024];

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }
  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr (BROADCAST_IP);
  addr.sin_port = htons (PORT);

  if (bind (sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
      perror ("bind");
      close (sock);
      exit (EXIT_FAILURE);
    }

  printf ("Listening for broadcast packets\n");

  while (1)
    {
      ssize_t len
          = recvfrom (sock, buffer, sizeof (buffer) - 1, 0, NULL, NULL);
      if (len > 0)
        {
          buffer[len] = '\0';
          printf ("Received broadcast: %s\n", buffer);
        }
    }

  close (sock);
  return 0;
}
