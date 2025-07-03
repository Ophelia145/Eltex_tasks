#include "main.h"

void
backwards (int n, int array[n])
{
  for (int i = n - 1; i >= 0; i--)
    {
      printf ("%d ", array[i]);
    }
  printf ("\n");
}