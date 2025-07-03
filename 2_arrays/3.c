#include "main.h"

void
fill_triangles (int n, int m)
{
  int array[n][m];

  for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < m; j++)
        {
          if (j <= m - (i + 1 + 1))
            array[i][j] = 0;
          else
            {
              array[i][j] = 1;
            }
        }
    }
  for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < m; j++)
        {

          printf ("%d ", array[i][j]);
        }
      printf ("\n");
    }
}