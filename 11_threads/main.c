#include "main.h"
int
main ()
{
  printf ("1st Task\n");
  run_5_threads ();

  printf ("2nd Task (without watcher)\n");
  increment_threads ();

  printf ("\n3rd Task\n");
  market ();
  return 0;
}