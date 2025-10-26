#include "client.h"
int
main (void)
{
  int sock = socket (AF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  struct sockaddr_in srv;
  memset (&srv, 0, sizeof (srv));
  srv.sin_family = AF_INET;
  srv.sin_port = htons (SERVER_PORT);
  if (inet_pton (AF_INET, SERVER_IP, &srv.sin_addr) <= 0)
    {
      fprintf (stderr, "inet_pton failed\n");
      close (sock);
      exit (EXIT_FAILURE);
    }

  if (connect (sock, (struct sockaddr *)&srv, sizeof (srv)) < 0)
    {
      perror ("connect");
      close (sock);
      exit (EXIT_FAILURE);
    }

  const char *req = "GET_TIME\n";
  if (send (sock, req, strlen (req), 0) < 0)
    {
      perror ("send");
      close (sock);
      exit (EXIT_FAILURE);
    }

  char buf[BUF_SIZE];
  ssize_t n = recv (sock, buf, sizeof (buf) - 1, 0);
  if (n < 0)
    {
      perror ("recv");
      close (sock);
      exit (EXIT_FAILURE);
    }
  else if (n == 0)
    {
      fprintf (stderr, "server closed connection\n");
    }
  else
    {
      buf[n] = '\0';
      printf ("server said the time is: %s", buf);
    }

  close (sock);
  return 0;
}
