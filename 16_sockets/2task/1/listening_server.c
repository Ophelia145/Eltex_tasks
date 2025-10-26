#include "listening_server.h"
#define _GNU_SOURCE
//!ВЫХОД ИЗ ПРОГРАММЫ: Ctrl+c
int listen_sock;

void
handle_sigint (int sig)
{
  (void)sig;
  printf ("\nСервер завершает работу...\n");
  close (listen_sock);
  exit (0);
}

ssize_t
send_all (int fd, const void *buf, size_t len)
{
  size_t total = 0;
  const char *p = buf;
  while (total < len)
    {
      ssize_t n = send (fd, p + total, len - total, MSG_NOSIGNAL);
      if (n < 0)
        {
          if (errno == EINTR)
            continue;
          return -1;
        }
      total += (size_t)n;
    }
  return (ssize_t)total;
}

void *
client_thread (void *arg)
{
  int conn_fd = *(int *)arg;
  free (arg);

  char buf[BUF_SIZE];
  ssize_t n;

  n = recv (conn_fd, buf, sizeof (buf) - 1, 0);
  if (n < 0)
    {
      perror ("recv");
      close (conn_fd);
      return NULL;
    }

  if (n == 0)
    {
      close (conn_fd);
      return NULL;
    }
  buf[n] = '\0';
  time_t now = time (NULL);
  struct tm tm_buf;
  localtime_r (&now, &tm_buf);

  char timestr[128];
  size_t tslen
      = strftime (timestr, sizeof (timestr), "%Y-%m-%d %H:%M:%S\n", &tm_buf);

  if (send_all (conn_fd, timestr, tslen) < 0)
    {
      perror ("send_all");
      close (conn_fd);
      return NULL;
    }

  shutdown (conn_fd, SHUT_WR);
  close (conn_fd);
  return NULL;
}

int
main (void)
{

  signal (SIGINT, handle_sigint);

  int listen_sock = socket (AF_INET, SOCK_STREAM, 0);
  if (listen_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  int opt = 1;
  if (setsockopt (listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt))
      < 0)
    {
      perror ("setsockopt");
      close (listen_sock);
      exit (EXIT_FAILURE);
    }

  struct sockaddr_in addr;
  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons (LISTEN_PORT);

  if (bind (listen_sock, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
      perror ("bind");
      close (listen_sock);
      exit (EXIT_FAILURE);
    }

  if (listen (listen_sock, LEN_QUEUE_REQ) < 0)
    {
      perror ("listen");
      close (listen_sock);
      exit (EXIT_FAILURE);
    }

  printf ("Time server listening on port %d\n", LISTEN_PORT);

  while (1)
    {
      struct sockaddr_in peer;
      socklen_t peerlen = sizeof (peer);
      int *conn_fd_ptr = malloc (sizeof (int));
      if (!conn_fd_ptr)
        {
          fprintf (stderr, "malloc failed\n");
          continue;
        }

      *conn_fd_ptr = accept (listen_sock, (struct sockaddr *)&peer, &peerlen);
      if (*conn_fd_ptr < 0)
        {
          perror ("accept");
          free (conn_fd_ptr);
          continue;
        }

      char peer_ip[INET_ADDRSTRLEN];
      inet_ntop (AF_INET, &peer.sin_addr, peer_ip, sizeof (peer_ip));
      printf ("Accepted connection from %s:%d\n", peer_ip,
              ntohs (peer.sin_port));

      pthread_t tid;
      if (pthread_create (&tid, NULL, client_thread, conn_fd_ptr) != 0)
        {
          perror ("pthread_create");
          close (*conn_fd_ptr);
          free (conn_fd_ptr);
          continue;
        }

      if (pthread_detach (tid) != 0)
        {
          perror ("pthread_detach");
        }
    }

  close (listen_sock);
  return 0;
}
