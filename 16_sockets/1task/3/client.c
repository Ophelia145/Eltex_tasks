#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

#define BUFFER_SIZE 100

int
main ()
{
  int client_sock;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  client_sock = socket (AF_INET, SOCK_DGRAM, 0);
  if (client_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons (SERVER_PORT);
  inet_pton (AF_INET, SERVER_IP, &server_addr.sin_addr);

  const char *message = "hello!";
  if (sendto (client_sock, message, strlen (message), 0,
              (struct sockaddr *)&server_addr, sizeof (server_addr))
      < 0)
    {
      perror ("sendto");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  printf ("sent to serv: %s\n", message);

  ssize_t recv_len
      = recvfrom (client_sock, buffer, sizeof (buffer) - 1, 0, NULL, NULL);
  if (recv_len < 0)
    {
      perror ("recvfrom");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  buffer[recv_len] = '\0';
  printf ("reply from serv: %s\n", buffer);

  close (client_sock);
  return 0;
}
