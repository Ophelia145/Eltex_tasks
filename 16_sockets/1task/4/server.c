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
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;
  char buffer[BUFFER_SIZE];

  server_sock = socket (AF_INET, SOCK_STREAM, 0);
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

  if (listen (server_sock, 5) < 0)
    {
      perror ("listen");
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  printf ("server is running waiting for message on port %d\n", PORT);

  client_len = sizeof (client_addr);
  client_sock
      = accept (server_sock, (struct sockaddr *)&client_addr, &client_len);
  if (client_sock < 0)
    {
      perror ("accept");
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  ssize_t recv_len = read (client_sock, buffer, sizeof (buffer) - 1);
  if (recv_len < 0)
    {
      perror ("read");
      close (client_sock);
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  buffer[recv_len] = '\0';
  printf ("got from client: %s\n", buffer);

  const char *reply = "hi!";
  if (write (client_sock, reply, strlen (reply)) < 0)
    {
      perror ("write");
      close (client_sock);
      close (server_sock);
      exit (EXIT_FAILURE);
    }

  printf ("reply sent: %s\n", reply);

  close (client_sock);
  close (server_sock);
  return 0;
}
