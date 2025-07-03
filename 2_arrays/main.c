#include "main.h"

int
main ()
{
  printf ("Задание 1, задайте размер матрицы:\n");
  int n;
  if (scanf ("%d", &n) == 1 && n > 0)
    {
      print_square_matrix (n);
    }

  printf ("Задание 2, задайте размер массива:\n");

  if (scanf ("%d", &n) == 1 && n > 0)
    {
      int array[n];
      printf ("Введите эл-ты массива\n");
      for (int i = 0; i < n; i++)
        {
          scanf ("%d", &array[i]);
        }
      backwards (n, array);
    }

  printf ("Задание 3, задайте размеры матрицы (кол-во строк и столбцов):\n");
  int n2;
  if (scanf ("%d %d", &n, &n2) == 2 && n > 0 && n2 > 0)
    {

      fill_triangles (n, n2);
    }

  printf ("Задание 4, задайте размер матрицы:\n");
  if (scanf ("%d", &n) == 1 && n > 0)
    {
      snail (n);
    }

  return 0;
}