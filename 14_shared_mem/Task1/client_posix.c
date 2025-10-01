#include "main.h"

int
main ()
{
  int fd;
  char *shmaddr;

  fd = shm_open (SHM_NAME, O_RDWR, 0666);
  if (fd == -1)
    {
      perror ("shm_open");
      exit (1);
    }

  shmaddr = mmap (NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shmaddr == MAP_FAILED)
    {
      perror ("mmap");
      exit (1);
    }

  printf ("posix client read message: %s\n", shmaddr);

  strcpy (shmaddr, "Hello!");
  printf ("posix client wrote reply: %s\n", shmaddr);

  munmap (shmaddr, SHM_SIZE);
  close (fd);

  return 0;
}
