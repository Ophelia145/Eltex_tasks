#include "listening_server.h"

static int
set_nonblocking (int fd)
{
  int flags = fcntl (fd, F_GETFL, 0);
  if (flags == -1)
    return -1;
  if (fcntl (fd, F_SETFL, flags | O_NONBLOCK) == -1)
    return -1;
  return 0;
}

static ssize_t
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

int
main (void)
{
  int tcp_fd = -1, udp_fd = -1, epoll_fd = -1;
  struct sockaddr_in addr;
  int opt = 1;

  tcp_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (tcp_fd < 0)
    {
      perror ("socket tcp");
      exit (EXIT_FAILURE);
    }
  setsockopt (tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
#ifdef SO_REUSEPORT
  setsockopt (tcp_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof (opt));
#endif

  udp_fd = socket (AF_INET, SOCK_DGRAM, 0);
  if (udp_fd < 0)
    {
      perror ("socket udp");
      close (tcp_fd);
      exit (EXIT_FAILURE);
    }
  setsockopt (udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));
#ifdef SO_REUSEPORT
  setsockopt (udp_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof (opt));
#endif

  memset (&addr, 0, sizeof (addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons (PORT);

  if (bind (tcp_fd, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
      perror ("bind tcp");
      close (tcp_fd);
      close (udp_fd);
      exit (EXIT_FAILURE);
    }
  if (listen (tcp_fd, 128) < 0)
    {
      perror ("listen");
      close (tcp_fd);
      close (udp_fd);
      exit (EXIT_FAILURE);
    }

  if (bind (udp_fd, (struct sockaddr *)&addr, sizeof (addr)) < 0)
    {
      perror ("bind udp");
      close (tcp_fd);
      close (udp_fd);
      exit (EXIT_FAILURE);
    }

  set_nonblocking (tcp_fd);
  set_nonblocking (udp_fd);

  epoll_fd = epoll_create1 (0);
  if (epoll_fd < 0)
    {
      perror ("epoll_create1");
      close (tcp_fd);
      close (udp_fd);
      exit (EXIT_FAILURE);
    }

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = tcp_fd;
  if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, tcp_fd, &ev) < 0)
    perror ("epoll_ctl tcp");

  ev.data.fd = udp_fd;
  if (epoll_ctl (epoll_fd, EPOLL_CTL_ADD, udp_fd, &ev) < 0)
    perror ("epoll_ctl udp");

  printf ("Server listening on port %d (TCP + UDP)\n", PORT);

  struct epoll_event events[MAX_EVENTS];
  char buf[BUF_SIZE];

  for (;;)
    {
      int n = epoll_wait (epoll_fd, events, MAX_EVENTS, -1);
      if (n < 0)
        {
          if (errno == EINTR)
            continue;
          perror ("epoll_wait");
          break;
        }

      for (int i = 0; i < n; ++i)
        {
          int fd = events[i].data.fd;

          if (fd == tcp_fd)
            {
              for (;;)
                {
                  struct sockaddr_in cli;
                  socklen_t clilen = sizeof (cli);
                  int conn = accept (tcp_fd, (struct sockaddr *)&cli, &clilen);
                  if (conn < 0)
                    {
                      if (errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                      else
                        {
                          perror ("accept");
                          break;
                        }
                    }
                  set_nonblocking (conn);
                  struct epoll_event ev_conn;
                  ev_conn.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
                  ev_conn.data.fd = conn;
                  epoll_ctl (epoll_fd, EPOLL_CTL_ADD, conn, &ev_conn);
                }
            }
          else if (fd == udp_fd)
            {
              struct sockaddr_in cli;
              socklen_t clilen = sizeof (cli);
              ssize_t r = recvfrom (udp_fd, buf, sizeof (buf) - 1, 0,
                                    (struct sockaddr *)&cli, &clilen);
              if (r > 0)
                {
                  time_t now = time (NULL);
                  struct tm tm_buf;
                  localtime_r (&now, &tm_buf);
                  char timestr[128];
                  size_t tslen = strftime (timestr, sizeof (timestr),
                                           "%Y-%m-%d %H:%M:%S\n", &tm_buf);
                  sendto (udp_fd, timestr, tslen, 0, (struct sockaddr *)&cli,
                          clilen);
                }
            }
          else
            {
              if (events[i].events & (EPOLLHUP | EPOLLRDHUP))
                {
                  epoll_ctl (epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                  close (fd);
                  continue;
                }
              ssize_t r = recv (fd, buf, sizeof (buf) - 1, 0);
              if (r <= 0)
                {
                  epoll_ctl (epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                  close (fd);
                  continue;
                }
              time_t now = time (NULL);
              struct tm tm_buf;
              localtime_r (&now, &tm_buf);
              char timestr[128];
              size_t tslen = strftime (timestr, sizeof (timestr),
                                       "%Y-%m-%d %H:%M:%S\n", &tm_buf);
              if (send_all (fd, timestr, tslen) < 0)
                {
                  epoll_ctl (epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                  close (fd);
                }
              else
                {
                  epoll_ctl (epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                  close (fd);
                }
            }
        }
    }

  close (epoll_fd);
  close (tcp_fd);
  close (udp_fd);
  return 0;
}
