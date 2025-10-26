#include "../client.h"

int
main (void)
{
  int sock;
  struct sockaddr_in srv;
  char buf[BUF_SIZE];

  sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  memset (&srv, 0, sizeof (srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons (SERVER_PORT);

  if (inet_pton (AF_INET, SERVER_IP, &srv.sin_addr) <= 0)
    {
      fprintf (stderr, "inet_pton failed\n");
      close (sock);
      exit (EXIT_FAILURE);
    }

  const char *req = "GET_TIME\n";
  ssize_t sent = sendto (sock, req, strlen (req), 0, (struct sockaddr *)&srv,
                         sizeof (srv));
  if (sent < 0)
    {
      perror ("sendto");
      close (sock);
      exit (EXIT_FAILURE);
    }

  socklen_t len = sizeof (srv);
  ssize_t n = recvfrom (sock, buf, sizeof (buf) - 1, 0,
                        (struct sockaddr *)&srv, &len);
  if (n < 0)
    {
      perror ("recvfrom");
      close (sock);
      exit (EXIT_FAILURE);
    }
  else if (n == 0)
    {
      fprintf (stderr, "server did not respond\n");
    }
  else
    {
      buf[n] = '\0';
      printf ("server said the time is: %s", buf);
    }

  close (sock);
  return 0;
}
