#include "main.h"

#include <signal.h>
#define MAX_CLIENTS 12

client_info clients[MAX_CLIENTS];
int client_count = 0;
mqd_t server_mq = (mqd_t)-1;

void
broadcast (message_info *m)
{
  for (int i = 0; i < client_count; i++)
    {
      mq_send (clients[i].qd, (char *)m, sizeof (message_info), 0);
    } //(char *)m
}

void
cleanup_and_exit (int signo)
{
  mq_unlink (QUEUE);
  (void)signo;
  if (server_mq != (mqd_t)-1)
    {
      mq_close (server_mq);
      mq_unlink (QUEUE);
    }
  for (int i = 0; i < client_count; i++)
    {
      mq_close (clients[i].qd);
    }
  printf ("\nServer shutting down.\n");
  exit (0);
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
  // attr.mq_curmsgs = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof (message_info);

  mqd_t mq = mq_open (QUEUE, O_CREAT | O_RDWR, 0666, &attr);
  if (mq == (mqd_t)-1)
    {
      perror ("open");
      exit (1);
    }
  printf ("Server started, queue: %s\n", QUEUE);

  message_info msg;
  while (1)
    {
      ssize_t bytes = mq_receive (mq, (char *)&msg, sizeof (msg), 0);
      if (bytes >= 0)
        {
          if (msg.type == MSG_CHAT)
            {
              broadcast (&msg);
            }
          else
            {
              message_info notify;
              notify.type = MSG_CHAT;
              strncpy (notify.sender, "SERVER", sizeof (notify.sender) - 1);
              notify.sender[sizeof (notify.sender) - 1] = '\0';

              if (msg.type == MSG_REG)
                {
                  if (client_count < MAX_CLIENTS)
                    {
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
                        {
                          printf (
                              "Client '%s' already registered, ignoring.\n",
                              msg.sender);
                          continue;
                        }
                      strncpy (clients[client_count].name, msg.sender,
                               sizeof (clients[0].name) - 1);
                      clients[client_count].name[sizeof (clients[0].name) - 1]
                          = '\0';
                      clients[client_count].qd = client_qd;
                      client_count++;
                      printf ("Our new chatter is %s\n", msg.sender);

                      snprintf (notify.text, MAX_SIZE, "%s joined us",
                                msg.sender);
                      broadcast (&notify);
                    }
                  else
                    {
                      fprintf (stderr,
                               "max quantity of clients reached, reject %s\n",
                               msg.sender);
                    }
                }
              else if (msg.type == MSG_EXIT)
                {
                  for (int i = 0; i < client_count; i++)
                    {
                      if (strcmp (clients[i].name, msg.sender) == 0)
                        {
                          mq_close (clients[i].qd);

                          for (int j = i; j < client_count - 1; j++)
                            {
                              clients[j] = clients[j + 1];
                            }
                          client_count--;
                          break;
                        }
                    }

                  snprintf (notify.text, MAX_SIZE, "%s left the chat",
                            msg.sender);
                  broadcast (&notify);
                }
              else
                {

                  fprintf (stderr, "unknown message type received\n");
                }
            }
        }
    }
  cleanup_and_exit (0);

  return 0;
}