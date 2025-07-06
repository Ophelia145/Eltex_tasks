#include <stdio.h>

int
work_with_file ()
{
  char *str = "String from file";
  char *filename = "output.txt";
  char buffer[256];
  size_t length;
  FILE *fthreadW = fopen (filename, "w");
  if (fthreadW)
    {
      fputs (str, fthreadW);
      fclose (fthreadW);
    }
  else
    {
      perror ("couldn't open file for w");
      return 1;
    }

  FILE *fthreadR = fopen (filename, "r");
  if (fthreadR)
    {
      fseek (fthreadR, 0,
             SEEK_END); //передвинули индикатор на (конец файла + 0)
      length = ftell (fthreadR); //запомнили позицию
      for (long i = length - 1; i >= 0; i--)
        {
          fseek (fthreadR, i, SEEK_SET); // i от начала
          char c = fgetc (fthreadR);
          putchar (c);
        }
      printf ("\n");
      fclose (fthreadR);
    }
  else
    {
      perror ("couldn't open file for r");
      return 1;
    }
  return 0;
}

int
main ()
{
  work_with_file ();
  return 0;
}
