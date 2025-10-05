#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SERVER_PATH "/tmp/unix_stream_socket"
#define BUFFER_SIZE 100

int
main ()
{
  int server_sock, client_sock;
  struct sockaddr_un server_addr;
  char buffer[BUFFER_SIZE];

  server_sock = socket (AF_LOCAL, SOCK_STREAM, 0);
  if (server_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  unlink (SERVER_PATH);

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sun_family = AF_LOCAL;
  strcpy (server_addr.sun_path, SERVER_PATH);

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

  printf ("server is running\n");

  client_sock = accept (server_sock, NULL, NULL);
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

  printf ("answer sent: %s\n", reply);

  close (client_sock);
  close (server_sock);
  unlink (SERVER_PATH);
  return 0;
}
