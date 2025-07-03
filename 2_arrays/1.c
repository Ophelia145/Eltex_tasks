#include "main.h"

void
print_square_matrix (int n)
{
  int array[n][n];
  int num = 1;
  for (int i = 1; i <= n; i++)
    {
      for (int j = 1; j <= n; j++)
        {
          array[i][j] = num++;
          printf ("%d ", array[i][j]);
        }
      printf ("\n");
    }
}