#include "main.h"
#include <stdlib.h>

void
print_array ()
{
  int *a = (int *)malloc (sizeof (int) * 10);
  int *p = a;
  int count = 0;

  for (int i = 0; i < 10; i++)
    {
      *p = (++count);
      printf ("%d ", *p);
      p++;
    }
  printf ("\n");
  free (a);
}