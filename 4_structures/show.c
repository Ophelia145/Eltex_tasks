#include "main.h"

int
print_all_the_records (abonent *array)
{
  int count = 0;
  for (int i = 0; i < out_of; i++)
    {
      if (array[i].name[0] != '\0')
        {
          printf ("%d: %s %s, тел: %s\n", i + 1, array[i].name,
                  array[i].second_name, array[i].tel);
          count++;
        }
    }

  if (count == 0)
    {
      printf ("Справочник пуст\n");
    }
  else
    {
      printf ("Всего %d человек\n", count);
    }
  return 0;
}