#include "main.h"
int
add (abonent *array)
{
  int index = -1;
  for (int i = 0; i < out_of; i++)
    {
      if (array[i].name[0] == '\0')
        {
          index = i;
          break;
        }
    }
  if (index == -1)
    {
      printf ("Список абнентов переполнен. Удалите ненужный для добавления.\n"
              "нового.");
      return -1;
    }
  printf ("Введите имя: \n");
  fgets (array[index].name, sizeof (array[index].name), stdin);
  array[index].name[strcspn (array[index].name, "\n")] = '\0';

  printf ("Введите фамилию: \n");

  fgets (array[index].second_name, sizeof (array[index].second_name), stdin);
  array[index].second_name[strcspn (array[index].second_name, "\n")] = '\0';

  printf ("Введите телефон: \n");
  fgets (array[index].tel, sizeof (array[index].tel), stdin);
  array[index].tel[strcspn (array[index].tel, "\n")] = '\0';

  printf ("Новый абонент добавлен.\n");
  return 0;
}