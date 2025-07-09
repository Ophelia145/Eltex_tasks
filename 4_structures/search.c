#include "main.h"

int
search_by_name (abonent *array)
{
  char name[10];
  printf ("Введите имя для поиска: ");
  fgets (name, sizeof (name), stdin);
  name[strcspn (name, "\n")] = '\0';

  int found = 0;
  for (int i = 0; i < out_of; i++)
    {
      if (array[i].name[0] != '\0' && strcmp (array[i].name, name) == 0)
        {
          printf ("Найден: %s %s, тел: %s\n", array[i].name,
                  array[i].second_name, array[i].tel);
          found = 1;
        }
    }

  if (!found)
    {
      printf ("Абоненты не найдены\n");
    }
  return 0;
}