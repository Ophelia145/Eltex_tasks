#include "main.h"

void
clear_input_buffer ()
{
  int c;
  while ((c = getchar ()) != '\n' && c != EOF)
    ;
}

void
push_back (double_list *journal)
{
  abonent new_person = new_abonent ();
  if (journal->size > out_of)
    {
      fprintf (stderr,
               "Список абонентов переполнен. Удалите ненужный для добавления "
               "нового.\n");
      exit (EXIT_FAILURE);
    }

  node *new = (node *)malloc (sizeof (node));
  if (!new)
    {
      perror ("mem alloc fail");
      exit (EXIT_FAILURE);
    }

  new->person = new_person;
  new->next = NULL;
  if (journal->tail)
    {
      journal->tail->next = new;
      new->prev = journal->tail;
      journal->tail = new;
    }
  else
    {
      new->prev = NULL;
      journal->head = journal->tail = new;
    }
  journal->size++;
}

abonent
new_abonent ()
{
  int error = 0;
  abonent new = { 0 };

  printf ("Введите имя: ");
  unsigned long i = 0;
  int c;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (new.name) - 1)
        {
          new.name[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  new.name[i] = '\0';

  if (i >= sizeof (new.name) && (new.name[i] != '\0'))
    {
      printf ("Превышена максимальная длина имени (%zu символов)\n",
              sizeof (new.name) - 1);
      clear_input_buffer ();
    }

  printf ("Введите фамилию: ");
  i = 0;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (new.second_name) - 1)
        {
          new.second_name[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  new.second_name[i] = '\0';

  if (i >= sizeof (new.second_name) && (new.second_name[i] != '\0'))
    {
      printf ("Превышена максимальная длина фамилии (%zu символов)\n",
              sizeof (new.second_name) - 1);
      clear_input_buffer ();
    }

  printf ("Введите телефон: ");
  i = 0;
  while ((c = getchar ()) != '\n' && c != EOF)
    {
      if (i < sizeof (new.tel) - 1)
        {
          new.tel[i++] = c;
        }
      else
        {
          error = 1;
        }
    }
  new.tel[i] = '\0';

  if (i >= sizeof (new.tel) && (new.tel[i] != '\0'))
    {
      printf ("Превышена максимальная длина телефона (%zu символов)\n",
              sizeof (new.tel) - 1);
      clear_input_buffer ();
    }

  if (!error)
    {
      printf ("Новый абонент добавлен.\n");
      ////
    }
  else
    {
      printf ("Некоторые поля были обрезаны из-за превышения длины.\n");
      ////
    }
  return new;
}