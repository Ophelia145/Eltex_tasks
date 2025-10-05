#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/unix_dgram_socket"
#define CLIENT_PATH "/tmp/unix_dgram_client"

#define BUFFER_SIZE 100

int
main ()
{
  int client_sock;
  struct sockaddr_un server_addr, client_addr;
  char buffer[BUFFER_SIZE];

  client_sock = socket (AF_LOCAL, SOCK_DGRAM, 0);
  if (client_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  unlink (CLIENT_PATH);

  memset (&client_addr, 0, sizeof (client_addr));
  client_addr.sun_family = AF_LOCAL;
  strcpy (client_addr.sun_path, CLIENT_PATH);

  if (bind (client_sock, (struct sockaddr *)&client_addr, sizeof (client_addr))
      < 0)
    {
      perror ("bind");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sun_family = AF_LOCAL;
  strcpy (server_addr.sun_path, SERVER_PATH);

  const char *message = "hello!";
  if (sendto (client_sock, message, strlen (message), 0,
              (struct sockaddr *)&server_addr, sizeof (server_addr))
      < 0)
    {
      perror ("sendto");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  printf ("sent to server: %s\n", message);

  ssize_t recv_len
      = recvfrom (client_sock, buffer, sizeof (buffer) - 1, 0, NULL, NULL);
  if (recv_len < 0)
    {
      perror ("recvfrom");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  buffer[recv_len] = '\0';
  printf ("respond from serv: %s\n", buffer);

  close (client_sock);
  unlink (CLIENT_PATH);
  return 0;
}
