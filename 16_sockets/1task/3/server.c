#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 100

int
main ()
{
  int server_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[BUFFER_SIZE];

  server_sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (server_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons (PORT);

  if (bind (server_sock, (struct sockaddr *)&server_addr, sizeof (server_addr))
      < 0)
    {
      perror ("bind");
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  printf ("server is running waiting for message on port %d\n", PORT);

  client_len = sizeof (client_addr);
  ssize_t recv_len = recvfrom (server_sock, buffer, sizeof (buffer) - 1, 0,
                               (struct sockaddr *)&client_addr, &client_len);
  if (recv_len < 0)
    {
      perror ("recvfrom");
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  buffer[recv_len] = '\0';
  printf ("got from client: %s\n", buffer);

  const char *reply = "hi!";
  if (sendto (server_sock, reply, strlen (reply), 0,
              (struct sockaddr *)&client_addr, client_len)
      < 0)
    {
      perror ("sendto");
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  printf ("reply sent: %s\n", reply);

  close (server_sock);
  return 0;
}
