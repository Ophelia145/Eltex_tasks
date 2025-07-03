#include "main.h"

void
snail (int n)
{
  int array[n][n], count = 0;
  int topJ = 0, bottomJ = n - 1, lI = 0, rI = n - 1;
  while (count < n * n)
    {
      for (int j = lI; j <= rI; j++)
        {
          array[topJ][j] = ++count;
        }
      topJ++;
      for (int i = topJ; i <= bottomJ; i++)
        {
          array[i][rI] = ++count;
        }
      rI--;
      for (int j = rI; j >= lI; j--)
        {
          array[bottomJ][j] = ++count;
        }
      bottomJ--;
      for (int i = bottomJ; i >= topJ; i--)
        {
          array[i][lI] = ++count;
        }
      lI++;
    }

  for (int i = 0; i < n; i++)
    {
      for (int j = 0; j < n; j++)
        {
          if (i == 0)
            printf ("%d  ", array[i][j]);

          else
            printf ("%d ", array[i][j]);
        }
      printf ("\n");
    }
}
