#include "main.h"

void
clear_input_buffer ()
{
  int c;
  while ((c = getchar ()) != '\n' && c != EOF)
    ;
}

int
add (abonent *array)
{
  int index = -1;
  int error = 0;

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
      printf ("Список абонентов переполнен. Удалите ненужный для добавления "
              "нового.\n");
      return -1;
    }

  printf ("Введите имя: ");
  unsigned long i = 0;
  int c;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (array[index].name) - 1)
        {
          array[index].name[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  array[index].name[i] = '\0';

  if (i >= sizeof (array[index].name) && (array[index].name[i] != '\0'))
    {
      printf ("Превышена максимальная длина имени (%zu символов)\n",
              sizeof (array[index].name) - 1);
      clear_input_buffer ();
    }

  printf ("Введите фамилию: ");
  i = 0;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (array[index].second_name) - 1)
        {
          array[index].second_name[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  array[index].second_name[i] = '\0';

  if (i >= sizeof (array[index].second_name)
      && (array[index].second_name[i] != '\0'))
    {
      printf ("Превышена максимальная длина фамилии (%zu символов)\n",
              sizeof (array[index].second_name) - 1);
      clear_input_buffer ();
    }

  printf ("Введите телефон: ");
  i = 0;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (array[index].tel) - 1)
        {
          array[index].tel[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  array[index].tel[i] = '\0';

  if (i >= sizeof (array[index].tel) && (array[index].tel[i] != '\0'))
    {
      printf ("Превышена максимальная длина телефона (%zu символов)\n",
              sizeof (array[index].tel) - 1);
      clear_input_buffer ();
    }

  if (!error)
    {
      printf ("Новый абонент добавлен.\n");
      return 0;
    }
  else
    {
      printf ("Некоторые поля были обрезаны из-за превышения длины.\n");
      return 1;
    }
}