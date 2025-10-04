#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
  if (argc != 2)
    {
      fprintf (stderr, "usage: %s pid\n", argv[0]);
      exit (EXIT_FAILURE);
    }

  pid_t pid = (pid_t)atoi (argv[1]);

  printf ("sending SIGUSR1 to proc with pid %d\n", pid);

  if (kill (pid, SIGUSR1) == -1)
    {
      perror ("kill");
      exit (EXIT_FAILURE);
    }

  printf ("sig sent\n");

  return 0;
}