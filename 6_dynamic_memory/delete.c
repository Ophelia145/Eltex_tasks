#include "main.h"

int delete (double_list *journal)
{
  char tel[10];
  printf ("Введите номер телефона для удаления: ");
  fgets (tel, sizeof (tel), stdin);
  tel[strcspn (tel, "\n")] = '\0';

  node *temp = journal->head;

  int found = 0;
  while (temp != NULL)
    {
      if (strcmp (temp->person.tel, tel) == 0)
        {
          printf ("Вы выбрали абонента %s %s \n", temp->person.name,
                  temp->person.second_name);
          // memset (&temp->person, 0, sizeof (abonent));
          if (temp->next != NULL && temp->prev != NULL) // middle
            {
              temp->prev->next = temp->next;
              temp->next->prev = temp->prev;
            }
          else if (temp->prev == NULL && temp->next == NULL) // 1 (head==tail)
            {
              journal->head = journal->tail = NULL;
            }
          else if (temp->next == NULL && temp->prev != NULL) // temp=tail (end)
            {

              journal->tail = temp->prev;
              journal->tail->next = NULL;
            }
          else //первый, но есть еще
            {
              journal->head = temp->next;
              journal->head->prev = NULL;
            }
          free (temp);
          journal->size--;
          printf ("Абонент удален");
          found = 1;
          break;
        }
      temp = temp->next;
    }

  if (!found)
    {
      printf ("Абонент с таким телефоном не найден\n");
    }
  return 0;
}