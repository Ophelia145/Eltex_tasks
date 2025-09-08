#include "main.h"
#define QUEUE_NAME "/queuepos1"
int
main ()
{
  mqd_t mq = mq_open (QUEUE_NAME, O_RDONLY);
  if (mq == (mqd_t)-1)
    {
      perror ("client's mq_open");
      exit (1);
    }

  char buffer[MAX_SIZE];
  unsigned int prio;
  ssize_t bytes = mq_receive (mq, buffer, MAX_SIZE, &prio);
  if (bytes >= 0)
    {
      printf ("Client got %s\n", buffer);
    }
  else
    {
      perror ("mq_receive");
    }
  if (mq_close (mq) == -1)
    {
      perror ("mq_close");
      exit (1);
    }
  mq = mq_open (QUEUE_NAME, O_WRONLY);
  if (mq == (mqd_t)-1)
    {
      perror ("client's mq_open");
      exit (1);
    }

  char reply[] = "Hello!";
  if (mq_send (mq, reply, strlen (reply) + 1, 0) == -1)
    {
      perror ("mq_send");
      exit (1);
    }
  printf ("Client said %s\n", reply);

  if (mq_close (mq) == -1)
    {
      perror ("mq_close");
      exit (1);
    }

  sleep (1);

  return 0;
}