#include "main.h"

int
search_by_name (double_list *journal)
{
  char name[10];
  printf ("Введите имя для поиска: ");
  fgets (name, sizeof (name), stdin);
  name[strcspn (name, "\n")] = '\0';
  node *temp = journal->head;

  int found = 0;
  while (temp != NULL)
    {
      if (strcmp (temp->person.name, name) == 0)
        {
          printf ("Найден: %s %s, тел: %s\n", temp->person.name,
                  temp->person.second_name, temp->person.tel);
          found = 1;
        }
      temp = temp->next;
    }

  if (!found)
    {
      printf ("Абоненты не найдены\n");
    }
  return 0;
}