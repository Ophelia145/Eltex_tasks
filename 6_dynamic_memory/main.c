#include "main.h"
void
start (double_list *journal)
{
  char command[10];
  int choice;

  while (1)
    {
      printf ("\n1) Добавить абонента\n2) Удалить абонента\n3) Поиск "
              "абонентов по имени\n4) Вывод всех записей\n5) Выход\n");
      printf (">> ");

      if (fgets (command, sizeof (command), stdin) == NULL)
        break;

      choice = atoi (command);

      switch (choice)
        {
        case 1:
          push_back (journal);
          break;
        case 2:
          delete (journal);
          break;
        case 3:
          search_by_name (journal);
          break;
        case 4:
          print_all_the_records (journal);
          break;
        case 5:
          return;
        default:
          printf ("Некорректная команда! Попробуйте еще.\n");
        }
    }
}
int
main ()
{
  double_list *journal = (double_list *)malloc (sizeof (double_list));
  init (journal);

  start (journal);
  free_list (journal);
  free (journal);
  return 0;
}

void
free_list (double_list *journal)
{
  node *current = journal->head;
  while (current != NULL)
    {
      node *next = current->next;
      free (current);
      current = next;
    }
  journal->head = NULL;
  journal->tail = NULL;
  journal->size = 0;
}