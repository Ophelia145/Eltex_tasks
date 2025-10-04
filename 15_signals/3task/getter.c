#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
main ()
{
  sigset_t mask;
  int signum;

  sigemptyset (&mask);
  sigaddset (&mask, SIGUSR1);
  if (sigprocmask (SIG_BLOCK, &mask, NULL) == -1)
    {
      perror ("sigprocmask");
      exit (EXIT_FAILURE);
    }

  printf ("started proc with pid: %d\n", getpid ());

  int count = 0;
  while (1)
    {
      if (sigwait (&mask, &signum) != 0)
        {
          perror ("sigwait");
          continue;
        }
      if (signum == SIGUSR1)
        {
          printf ("received SIGUSR1!\ni=%d\n", count);
        }
      count++;
    }

  return 0;
}