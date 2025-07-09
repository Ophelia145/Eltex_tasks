#include "main.h"
void
start (abonent *array)
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
          add (array);
          break;
        case 2:
          delete (array);
          break;
        case 3:
          search_by_name (array);
          break;
        case 4:
          print_all_the_records (array);
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
  abonent array[out_of] = { 0 };

  start (array);

  return 0;
}