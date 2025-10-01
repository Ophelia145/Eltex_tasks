#include "main.h"

int
main ()
{
  int shmid;
  char *shmaddr;

  shmid = shmget (SHM_KEY, SHM_SIZE, 0666);
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

  printf ("sysV client read message: %s\n", shmaddr);

  strcpy (shmaddr, "Hello!");
  printf ("sysV client wrote reply: %s\n", shmaddr);
  if (shmdt (shmaddr) == -1)
    {
      perror ("shmdt");
      exit (1);
    }
  return 0;
}
