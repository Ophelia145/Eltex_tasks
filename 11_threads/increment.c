#include "main.h"
#define QUANTITY_THREAD 18

pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;
long num = 0;
void *
increment (void *arg)
{
  long tmp;
  for (int i = 0; i < 1000000; i++)

    {
      pthread_mutex_lock (&m1); //именно адрес, а не значение, чтоб корректно
      num++;
      pthread_mutex_unlock (&m1);
    }

  return NULL;
}

void
increment_threads ()
{

  pthread_t threads[QUANTITY_THREAD];
  for (int i = 0; i < QUANTITY_THREAD; i++)
    {
      if (pthread_create (&threads[i], NULL, increment, NULL) != 0)
        {
          perror ("threads creation is bad");
          exit (1);
        }
    }
  for (int i = 0; i < QUANTITY_THREAD; i++)
    {
      pthread_join (threads[i], NULL);
    }

  printf ("num: %ld", num);
}
