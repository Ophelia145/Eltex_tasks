#include "main.h"
#define QUANTITY_THREAD 5
void *
print_thread_ind (void *arg)
{
  int ind = *(int *)arg;
  printf ("thread %d\n", ind);
  return NULL;
}

void
run_5_threads ()
{

  pthread_t threads[QUANTITY_THREAD];
  int index[QUANTITY_THREAD];

  for (int i = 0; i < QUANTITY_THREAD; i++)
    {
      index[i] = i;
      if (pthread_create (&threads[i], NULL, print_thread_ind,
                          (void *)&index[i])
          != 0)
        {
          perror ("creation of thread failed");
          exit (1);
        }
    }
  for (int i = 0; i < QUANTITY_THREAD; i++)
    {
      pthread_join (threads[i], NULL);
    }
}