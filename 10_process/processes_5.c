#include "main.h"

void
processes_5 ()
{
  printf ("main pro: pid - %d, ppid - %d\n", getpid (), getppid ());
  pid_t proc1 = fork ();

  if (proc1 < 0)
    {
      perror ("proc1 wasn't created");
      exit (EXIT_FAILURE);
    }

  if (proc1 == 0)
    {
      printf ("proc1: pid = %d, ppid = %d\n", getpid (), getppid ());
      pid_t proc3 = fork ();
      if (proc3 < 0)
        {
          perror ("proc3 wasn't created");
          exit (EXIT_FAILURE);
        }
      else if (proc3 == 0)
        {
          printf ("proc3: pid - %d, ppid - %d\n", getpid (), getppid ());
          exit (EXIT_SUCCESS);
        }

      pid_t proc4 = fork ();
      if (proc4 < 0)
        {
          perror ("proc4 wasn't created");
          exit (EXIT_FAILURE);
        }
      else if (proc4 == 0)
        {
          printf ("proc4: pid - %d, ppid - %d\n", getpid (), getppid ());
          exit (EXIT_SUCCESS);
        }

      waitpid (proc3, NULL, 0);
      waitpid (proc4, NULL, 0);
      exit (EXIT_SUCCESS);
    }

  pid_t proc2 = fork ();
  if (proc2 < 0)
    {
      perror ("proc2 wasn't created");
      exit (EXIT_FAILURE);
    }

  if (proc2 == 0)
    {
      printf ("proc2: pid - %d, ppid - %d\n", getpid (), getppid ());

      pid_t proc5 = fork ();
      if (proc5 < 0)
        {
          perror ("proc5 wasn't created");
          exit (EXIT_FAILURE);
        }
      else if (proc5 == 0)
        {
          printf ("proc5: pid - %d, ppid - %d\n", getpid (), getppid ());
          exit (EXIT_SUCCESS);
        }

      waitpid (proc5, NULL, 0);
      exit (EXIT_SUCCESS);
    }

  waitpid (proc1, NULL, 0);
  waitpid (proc2, NULL, 0);
}