#include "main.h"

mqd_t server_qd = (mqd_t)-1;
mqd_t client_qd = (mqd_t)-1;
char client_queue_name[128];
char client_name[128];

WINDOW *chat_win;
WINDOW *input_win;
WINDOW *user_win;

pthread_mutex_t win_lock = PTHREAD_MUTEX_INITIALIZER;

void *
receiver_thread (void *arg __attribute__ ((unused)))
{
  message_info msg;
  while (1)
    {
      ssize_t bytes = mq_receive (client_qd, (char *)&msg, sizeof (msg), NULL);
      if (bytes >= 0)
        {
          if (msg.type == MSG_USERLIST)
            {
              pthread_mutex_lock (&win_lock);
              werase (user_win);
              box (user_win, 0, 0);
              mvwprintw (user_win, 0, 2, "Users");

              int line = 1;
              char *saveptr;
              char *token = strtok_r (msg.text, "\n", &saveptr);
              while (token && line < getmaxy (user_win) - 1)
                {
                  mvwprintw (user_win, line++, 1, "%s", token);
                  token = strtok_r (NULL, "\n", &saveptr);
                }

              wrefresh (user_win);
              pthread_mutex_unlock (&win_lock);
            }
          else
            {
              // 👉 обычное сообщение в чат
              pthread_mutex_lock (&win_lock);
              if (strcmp (msg.sender, "SERVER") == 0)
                wattron (chat_win, COLOR_PAIR (1));
              else
                wattron (chat_win, COLOR_PAIR (2));

              wprintw (chat_win, "[%s]: %s\n", msg.sender, msg.text);
              wrefresh (chat_win);

              if (strcmp (msg.sender, "SERVER") == 0)
                wattroff (chat_win, COLOR_PAIR (1));
              else
                wattroff (chat_win, COLOR_PAIR (2));

              pthread_mutex_unlock (&win_lock);
            }
        }
    }
  return NULL;
}

int
main ()
{
  printf ("Enter your name: ");
  if (scanf ("%127s", client_name) != 1)
    {
      fprintf (stderr, "Invalid name\n");
      exit (1);
    }
  getchar ();

  snprintf (client_queue_name, sizeof (client_queue_name), "/client_%s",
            client_name);

  struct mq_attr attr;
  memset (&attr, 0, sizeof (attr));
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof (message_info);

  client_qd = mq_open (client_queue_name, O_CREAT | O_RDONLY, 0666, &attr);
  if (client_qd == -1)
    {
      perror ("mq_open client");
      exit (1);
    }

  ////////отрисовка
  initscr ();
  cbreak ();
  // noecho ();
  start_color ();
  init_pair (1, COLOR_GREEN, COLOR_BLACK);
  init_pair (2, COLOR_CYAN, COLOR_BLACK);

  int height, width;
  getmaxyx (stdscr, height, width);

  chat_win = newwin (height - 3, width - 20, 0, 0);
  input_win = newwin (3, width - 20, height - 3, 0);
  user_win = newwin (height - 3, 20, 0, width - 20);

  keypad (input_win, TRUE);
  scrollok (chat_win, TRUE);
  scrollok (user_win, TRUE);
  box (input_win, 0, 0);
  box (user_win, 0, 0);
  wrefresh (chat_win);
  wrefresh (user_win);
  wrefresh (input_win);
  /////////////////////////////////////
  pthread_t tid;
  pthread_create (&tid, NULL, receiver_thread, NULL);
  pthread_detach (tid);

  server_qd = mq_open (QUEUE, O_WRONLY);
  if (server_qd == -1)
    {
      perror ("server open");
      exit (1);
    }

  message_info join_msg = { 0 };
  join_msg.type = MSG_REG;
  strcpy (join_msg.sender, client_name);
  strcpy (join_msg.queue_name, client_queue_name);
  mq_send (server_qd, (char *)&join_msg, sizeof (join_msg), 0);

  ////////////////////////////////////////////////////

  char text[MAX_SIZE];
  while (1)
    {
      pthread_mutex_lock (&win_lock);
      wclear (input_win);
      box (input_win, 0, 0);
      mvwprintw (input_win, 1, 1, "> ");
      wrefresh (input_win);
      pthread_mutex_unlock (&win_lock);

      wgetnstr (input_win, text, MAX_SIZE - 1);

      if (strcmp (text, "/e") == 0)
        {
          message_info exit_msg = { 0 };
          exit_msg.type = MSG_EXIT;
          strcpy (exit_msg.sender, client_name);
          mq_send (server_qd, (char *)&exit_msg, sizeof (exit_msg), 0);
          break;
        }

      message_info chat_msg = { 0 };
      chat_msg.type = MSG_CHAT;
      strcpy (chat_msg.sender, client_name);
      strcpy (chat_msg.text, text);
      mq_send (server_qd, (char *)&chat_msg, sizeof (chat_msg), 0);
    }

  endwin ();
  mq_close (client_qd);
  mq_unlink (client_queue_name);
  return 0;
}
