#include "main.h"

int delete (abonent *array)
{
  char tel[10];
  printf ("Введите номер телефона для удаления: ");
  fgets (tel, sizeof (tel), stdin);
  tel[strcspn (tel, "\n")] = '\0';

  int found = 0;
  for (int i = 0; i < out_of; i++)
    {
      if (strcmp (array[i].tel, tel) == 0)
        {
          memset (&array[i], 0, sizeof (abonent));
          printf ("Абонент удален\n");
          found = 1;
          break;
        }
    }

  if (!found)
    {
      printf ("Абонент с таким телефоном не найден\n");
    }
  return 0;
}