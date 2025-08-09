#include "main.h"


void
print_proc_inf ()
{
  pid_t pid1 = fork ();

  if (pid1 == 0)
    {
      printf ("Child's PID: %d, parent's PID: %d\n", getpid (), getppid ());
      exit (EXIT_SUCCESS);
    }
  else if (pid1 > 0)
    {
      int status;
      printf ("Parent's PID: %d, created child with PID: %d\n", getpid (),
              pid1); //форк в процесс родителя возвращает пид ребенка
      waitpid (pid1, &status, 0);
      printf ("Child exited with status %d\n", WEXITSTATUS (status));
    }
  else
    {
      perror ("Ошибка fork()");
      exit (EXIT_FAILURE);
    }
}
