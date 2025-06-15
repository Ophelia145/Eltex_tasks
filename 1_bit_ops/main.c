#include "main.h"
int
main ()
{
  printf ("\nЗадание 1 и 2\n Введите положительное и отрицательное числа.\n");
  int a, b, startA, startB;
  if (scanf ("%d %d", &a, &b) == 2)
    {
      if (a != 0)
        {
          startA = find_1st_1_in_num (a);
          print_bin_wth_0_at_the_beginning (a, startA);

          startB = find_1st_1_in_num (b);
          print_bin_wth_0_at_the_beginning (b, startB);
        }
      else
        {
          putchar (0);
          printf ("\n");
        }
    }
  //////////////////////////////////////////
  printf ("\nЗадание 3\n Введите положительное число.\n");
  int с;
  if (scanf ("%d", &с) == 1)
    {
      if (с > 0)
        {

          printf ("%d\n", find_quantity_of_1 (с));
        }
      else if (с == 0)
        {
          printf ("you ought to type positive number, and 0 is neutral \n");
        }
      else
        {

          printf ("you ought to type positive number, not negative \n");
        }
    }

  //////////////////////////////////////////
  printf ("\nЗадание 4\nВведите сначала число над которым будут проводиться "
          "манипуляции, "
          "затем число пользователя:\n");
  int num, user_num;

  if (scanf ("%d %d", &num, &user_num) == 2 && num >= 0 && user_num < 255
      && user_num >= 0)
    {

      printf ("%d\n", change (num, user_num));
    }

  return 0;
}