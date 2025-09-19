#include "main.h"

#define MAX_CLIENTS 12
#define HISTORY_SIZE 100

client_info clients[MAX_CLIENTS];
int client_count = 0;
mqd_t server_mq = (mqd_t)-1;

message_info history[HISTORY_SIZE];
int history_count = 0;
int history_start = 0;

void
broadcast (message_info *m)
{
  for (int i = 0; i < client_count; i++)
    {
      mq_send (clients[i].qd, (char *)m, sizeof (message_info), 0);
    }
}

void
cleanup_and_exit (int signo)
{
  (void)signo;
  if (server_mq != (mqd_t)-1)
    mq_close (server_mq);
  mq_unlink (QUEUE);
  for (int i = 0; i < client_count; i++)
    mq_close (clients[i].qd);
  printf ("\nserver shutting down.\n");
  exit (0);
}

void
add_to_history (const message_info *msg)
{
  int pos = (history_start + history_count) % HISTORY_SIZE;
  history[pos] = *msg;
  if (history_count < HISTORY_SIZE)
    history_count++;
  else
    history_start = (history_start + 1) % HISTORY_SIZE;
}

void
send_history (mqd_t client_qd)
{
  for (int i = 0; i < history_count; i++)
    {
      int pos = (history_start + i) % HISTORY_SIZE;
      mq_send (client_qd, (char *)&history[pos], sizeof (message_info), 0);
    }
}

int
main ()
{
  mq_unlink (QUEUE);

  struct sigaction sa;
  sa.sa_handler = cleanup_and_exit;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction (SIGINT, &sa, NULL);
  sigaction (SIGTERM, &sa, NULL);

  struct mq_attr attr;
  memset (&attr, 0, sizeof (attr));
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof (message_info);

  server_mq = mq_open (QUEUE, O_CREAT | O_RDWR, 0666, &attr);
  if (server_mq == (mqd_t)-1)
    {
      perror ("open");
      exit (1);
    }
  printf ("server started queue: %s\n", QUEUE);

  message_info msg;
  while (1)
    {
      ssize_t bytes = mq_receive (server_mq, (char *)&msg, sizeof (msg), NULL);
      if (bytes < 0)
        continue;

      if (msg.type == MSG_CHAT)
        {
          broadcast (&msg);
          add_to_history (&msg);
        }
      else
        {
          message_info notify;
          notify.type = MSG_CHAT;
          strncpy (notify.sender, "SERVER", sizeof (notify.sender) - 1);
          notify.sender[sizeof (notify.sender) - 1] = '\0';

          if (msg.type == MSG_REG)
            {
              if (client_count >= MAX_CLIENTS)
                {
                  fprintf (stderr, "max clients reached, reject %s\n",
                           msg.sender);
                  continue;
                }

              mqd_t client_qd = mq_open (msg.queue_name, O_WRONLY);
              if (client_qd == -1)
                {
                  perror ("open new client queue");
                  continue;
                }

              int exists = 0;
              for (int i = 0; i < client_count; i++)
                {
                  if (strcmp (clients[i].name, msg.sender) == 0)
                    {
                      exists = 1;
                      mq_close (client_qd);
                      break;
                    }
                }
              if (exists)
                continue;

              strncpy (clients[client_count].name, msg.sender,
                       sizeof (clients[0].name) - 1);
              clients[client_count].name[sizeof (clients[0].name) - 1] = '\0';
              clients[client_count].qd = client_qd;
              client_count++;

              printf ("new client: %s\n", msg.sender);

              snprintf (notify.text, MAX_SIZE, "%s joined chat", msg.sender);
              broadcast (&notify);
              send_history (client_qd);
            }
          else if (msg.type == MSG_EXIT)
            {
              for (int i = 0; i < client_count; i++)
                {
                  if (strcmp (clients[i].name, msg.sender) == 0)
                    {
                      mq_close (clients[i].qd);
                      for (int j = i; j < client_count - 1; j++)
                        clients[j] = clients[j + 1];
                      client_count--;
                      break;
                    }
                }
              snprintf (notify.text, MAX_SIZE, "%s left our chat", msg.sender);
              broadcast (&notify);
            }
        }
    }

  cleanup_and_exit (0);
  return 0;
}
