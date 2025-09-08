#include "main.h"

#define QUEUE_NAME "/queuepos1"

int
main ()
{
  printf ("\n....POSIX...\n");
  struct mq_attr attr = { 0 };
  attr.mq_flags = 0; // ждем
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_SIZE;
  attr.mq_curmsgs = 0;

  mqd_t mq = mq_open (QUEUE_NAME, O_CREAT | O_WRONLY, 0600, &attr);
  if (mq == (mqd_t)-1)
    {
      perror ("mq_open");
      exit (1);
    }
  char msg[] = "Hi!";
  if (mq_send (mq, msg, strlen (msg) + 1, 0) == -1)
    {
      perror ("mq_send");
      exit (1);
    }
  printf ("Server said %s\n", msg);
  if (mq_close (mq) == -1)
    {
      perror ("mq_close");
      exit (1);
    }
  sleep (1);
  mq = mq_open (QUEUE_NAME, O_RDONLY);
  if (mq == (mqd_t)-1)
    {
      perror ("mq_open");
      exit (1);
    }

  char buffer[MAX_SIZE];
  unsigned int prio;
  ssize_t bytes = mq_receive (mq, buffer, MAX_SIZE, &prio);
  if (bytes >= 0)
    {
      printf ("Server got %s\n", buffer);
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
  if (mq_unlink (QUEUE_NAME) == -1)
    {
      perror ("unlink");
      exit (1);
    }

  return 0;
}