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
  int client_sock;
  struct sockaddr_un server_addr;
  char buffer[BUFFER_SIZE];

  client_sock = socket (AF_LOCAL, SOCK_STREAM, 0);
  if (client_sock < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sun_family = AF_LOCAL;
  strcpy (server_addr.sun_path, SERVER_PATH);

  if (connect (client_sock, (struct sockaddr *)&server_addr,
               sizeof (server_addr))
      < 0)
    {
      perror ("connect");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  const char *message = "hello!";
  if (write (client_sock, message, strlen (message)) < 0)
    {
      perror ("write");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  printf ("sent to server: %s\n", message);

  ssize_t recv_len = read (client_sock, buffer, sizeof (buffer) - 1);
  if (recv_len < 0)
    {
      perror ("read");
      close (client_sock);
      exit (EXIT_FAILURE);
    }

  buffer[recv_len] = '\0';
  printf ("reply from server: %s\n", buffer);

  close (client_sock);
  return 0;
}
