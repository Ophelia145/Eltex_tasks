#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define PORT 8888
#define BUF_SZ 1024

int
main (void)
{
  int s = socket (AF_INET, SOCK_DGRAM, 0);
  if (s == -1)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_in addr = { .sin_family = AF_INET,
                              .sin_port = htons (PORT),
                              .sin_addr.s_addr = INADDR_ANY };

  if (bind (s, (struct sockaddr *)&addr, sizeof (addr)) == -1)
    {
      perror ("bind");
      close (s);
      exit (EXIT_FAILURE);
    }

  srand ((unsigned)time (NULL));
  printf ("UDP server is listening on %d\n", PORT);

  while (1)
    {
      char buf[BUF_SZ];
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof (client_addr);

      ssize_t len = recvfrom (s, buf, sizeof (buf) - 1, 0,
                              (struct sockaddr *)&client_addr, &client_len);
      if (len < 0)
        {
          perror ("recvfrom");
          continue;
        }

      buf[len] = '\0';
      printf ("Received: %s\n", buf);

      if (len > 0)
        {
          int pos = rand () % len;
          buf[pos] += 1;
        }

      printf ("Modified: %s\n", buf);

      if (sendto (s, buf, strlen (buf), 0, (struct sockaddr *)&client_addr,
                  client_len)
          == -1)
        {
          perror ("sendto");
        }
    }

  close (s);
  return 0;
}
