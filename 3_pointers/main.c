#include "main.h"

int
main ()
{
  int n, n2;
  printf ("\nЗадание 1\n Введите целое положительное числo и число на замену "
          "3его байта.\n");
  if (scanf ("%d %d", &n, &n2) == 2 && n2 <= 255 && n2 >= 0)
    {
      change_3rd_byte (&n, (unsigned char)n2);
      printf ("результат: %d\n", n);
    }

  printf ("\nЗадание 2 запускается командой make run2\n \n");

  printf ("\nЗадание 3\n \n");
  print_array ();

  printf ("\nЗадание 4\n \n");

  char str[200] = "Getting born in the state of Mississippi. Papa was a "
                  "copper and her momma was a hippie.";
  char mini[100] = "Mississippi";
  char *p = strstrmy (str, mini);
  if (p != NULL)
    {
      printf ("\nFirst occurence on %ld position\n", p - str);
      printf ("%s\n", p);
    }
  else
    printf ("\n mini-string wasn't found in the string");

  return 0;
}