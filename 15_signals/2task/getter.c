#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void
sigint_handler (int sig)
{
  printf ("\nМУА-ХА-ХА! надо kill pid\n");
}

void
sigusr1_handler (int sig)
{
  printf ("пришел сигнал SIGUSR1!\n");
}

int
main ()
{
  //странно, все-таки, что компилятор не вызывает ошибку, но vs_code ругается
  sigset_t mask;

  sigemptyset (&mask);
  sigaddset (&mask, SIGUSR1);
  if (sigprocmask (SIG_BLOCK, &mask, NULL) == -1)
    {
      perror ("sigprocmask");
      exit (EXIT_FAILURE);
    }

  struct sigaction sa_int = { 0 };
  sa_int.sa_handler = sigint_handler;
  sigemptyset (&sa_int.sa_mask);
  sa_int.sa_flags = 0;
  if (sigaction (SIGINT, &sa_int, NULL) == -1)
    {
      perror ("sigaction SIGINT");
      exit (EXIT_FAILURE);
    }

  struct sigaction sa_usr1 = { 0 };
  sa_usr1.sa_handler = sigusr1_handler;
  sigemptyset (&sa_usr1.sa_mask);
  sa_usr1.sa_flags = 0;
  if (sigaction (SIGUSR1, &sa_usr1, NULL) == -1)
    {
      perror ("sigaction SIGUSR1");
      exit (EXIT_FAILURE);
    }

  printf ("PID: %d\n", getpid ());
  printf ("попробуйте нажать ctrl+c)))))\n");

  while (1)
    {
      pause ();
    }

  return 0;
}
