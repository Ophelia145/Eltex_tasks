#include "calc.h"

void
start ()
{
  int command = 0, a, b;
  while (1)
    {
      printf ("\nВыберите операцию:\n\n1) Сложение\n\
2) Вычитание\n\
3) Умножение\n\
4) Деление\n\
5) Выход\n\n\n");
      if (scanf ("%d", &command) == 1 && command > 0 && command < 6)
        {
          if (command == 5)
            return;
          printf ("\nВведите 2 операнда.\n");
          if (scanf ("%d %d", &a, &b) != 2)
            {
              while (getchar () != '\n') // очищаем буффер до \n
                ;

              printf ("\nПожалуйста, введите целые числа в следующий раз\n");
              continue;
            }
          switch (command)
            {
            case 1:
              add (a, b);
              break;
            case 2:
              sub (a, b);
              break;
            case 3:
              mul (a, b);
              break;
            case 4:
              divi (a, b);
              break;
            }
        }
    }
}

int
main ()
{
  start ();

  return 0;
}