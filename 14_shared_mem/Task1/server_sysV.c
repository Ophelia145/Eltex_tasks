#include "main.h"

int
main ()
{
  int shmid;
  char *shmaddr;
  shmid = shmget (SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid < 0)
    {
      perror ("shmget");
      exit (1);
    }

  shmaddr = (char *)shmat (shmid, NULL, 0);
  if (shmaddr == (char *)-1)
    {
      perror ("shmat");
      exit (1);
    }

  strcpy (shmaddr, "Hi!");
  printf ("sysV server wrote message: %s\n", shmaddr);

  while (strcmp (shmaddr, "Hello!") != 0)
    {
      sleep (1);
    }
  printf ("sysV server got reply: %s\n", shmaddr);
  if (shmdt (shmaddr) == -1)
    {
      perror ("shmdt");
    }

  if (shmctl (shmid, IPC_RMID, NULL) == -1)
    {
      perror ("shmctl");
      exit (EXIT_FAILURE);
    }

  return 0;
}
