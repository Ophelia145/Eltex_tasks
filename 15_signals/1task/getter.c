#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void
handler (int sig, siginfo_t *siginfo, void *arg)
{
  if (sig == SIGUSR1)
    {
      printf ("got sig SIGUSR1 yay but it doesnt do anything so :)\n");
    }
}

int
main ()
{
  struct sigaction act = { 0 };

  act.sa_flags = 0;
  act.sa_sigaction = &handler;
  if (sigaction (SIGUSR1, &act, NULL) == -1)
    {
      perror ("sigaction");
      exit (EXIT_FAILURE);
    }

  printf ("started proc with pid: %d\n", getpid ());

  while (1)
    {
      pause ();
    }

  return 0;
}