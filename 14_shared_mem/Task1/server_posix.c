#include "main.h"

int
main ()
{
  int fd;
  char *shmaddr;

  fd = shm_open (SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (fd == -1)
    {
      perror ("shm_open");
      exit (1);
    }

  ftruncate (fd, SHM_SIZE);

  shmaddr = mmap (NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmaddr == MAP_FAILED)
    {
      perror ("mmap");
      exit (1);
    }

  strcpy (shmaddr, "Hi!");
  printf ("posix server wrote message: %s\n", shmaddr);

  while (strcmp (shmaddr, "Hello!") != 0)
    {
      sleep (1);
    }
  printf ("posix server got reply: %s\n", shmaddr);

  munmap (shmaddr, SHM_SIZE);
  close (fd);
  shm_unlink (SHM_NAME);

  return 0;
}
