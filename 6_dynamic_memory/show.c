#include "main.h"

int
print_all_the_records (double_list *journal)
{
  node *temp = journal->head;
  int i = 0;
  while (temp != NULL)
    {

      printf ("%d: %s %s, тел: %s\n", i + 1, temp->person.name,
              temp->person.second_name, temp->person.tel);
      temp = temp->next;
      i++;
    }

  if (journal->size == 0)
    {
      printf ("Справочник пуст\n");
    }
  else
    {
      printf ("Всего %ld человек\n", journal->size);
    }
  return 0;
}