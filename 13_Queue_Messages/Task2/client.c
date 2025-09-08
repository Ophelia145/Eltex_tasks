#include "main.h"
#include <ncurses.h>
#include <pthread.h>

mqd_t server_qd = (mqd_t)-1;
mqd_t client_qd = (mqd_t)-1;
char client_queue_name[128];
char client_name[120];
void *
receiver_thread (void *arg __attribute__ ((unused)))
{
  message_info msg;
  while (1)
    {
      ssize_t bytes = mq_receive (client_qd, (char *)&msg, sizeof (msg), NULL);
      if (bytes >= 0)
        {
          if (msg.type == MSG_CHAT)
            {
              printf ("[%s]: %s\n", msg.sender, msg.text);
            }
        }
    }
  return NULL;
}
int
main ()
{

  printf ("enter your name: ");
  scanf ("%s", client_name);
  snprintf (client_queue_name, 128, "/client_%s", client_name);

  server_qd = mq_open (QUEUE, O_WRONLY);
  if (server_qd == -1)
    {
      perror ("server open");
      exit (1);
    }

  struct mq_attr attr;
  memset (&attr, 0, sizeof (attr));
  // attr.mq_curmsgs = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof (message_info);
  attr.mq_flags = 0;
  client_qd = mq_open (client_queue_name, O_CREAT | O_RDONLY, 0666, &attr);
  if (client_qd == -1)
    {
      perror ("mq_open client");
      exit (1);
    }
  message_info join_msg;
  join_msg.type = MSG_REG;
  strcpy (join_msg.sender, client_name);
  strcpy (join_msg.queue_name, client_queue_name);
  mq_send (server_qd, (char *)&join_msg, sizeof (join_msg), 0);

  pthread_t tid;
  pthread_create (&tid, NULL, receiver_thread, NULL);

  char text[MAX_SIZE];
  while (1)
    {
      fgets (text, MAX_SIZE, stdin);
      text[strcspn (text, "\n")] = 0;

      if (strcmp (text, "/exit") == 0)
        {
          message_info exit_msg;
          exit_msg.type = MSG_EXIT;
          strcpy (exit_msg.sender, client_name);
          mq_send (server_qd, (char *)&exit_msg, sizeof (exit_msg), 0);
          break;
        }
      message_info chat_msg;
      chat_msg.type = MSG_CHAT;
      strcpy (chat_msg.sender, client_name);
      strcpy (chat_msg.text, text);
      mq_send (server_qd, (char *)&chat_msg, sizeof (chat_msg), 0);
    }

  mq_close (client_qd);
  mq_unlink (client_queue_name);

  return 0;
}