#include "main.h"

#define COLOR_PINK 12
#define COLOR_DARK_BLUE 13

typedef struct
{
  mqd_t service_mq;
  char name[10];
  pid_t uid;
} sender_args_t;

WINDOW *chat_win, *users_win, *input_win;
int chat_lines = 0;
message_t chat_history[HISTORY_SIZE];
char users[MAX_CLIENTS][20];
int users_count = 0;
pthread_mutex_t ui_mutex = PTHREAD_MUTEX_INITIALIZER;

shared_data_t *shared_memory = NULL;
int last_read_pos = 0;

void
redraw_chat ()
{
  pthread_mutex_lock (&ui_mutex);
  werase (chat_win);
  box (chat_win, 0, 0);
  int start = chat_lines > LINES - 6 ? chat_lines - (LINES - 6) : 0;
  for (int i = start; i < chat_lines; ++i)
    {
      message_t msg = chat_history[i];
      switch (msg.type)
        {
        case USER_CAME:
          mvwprintw (chat_win, i - start + 1, 1, "user %s joined us",
                     msg.username);
          break;
        case USER_LEFT:
          mvwprintw (chat_win, i - start + 1, 1, "user %s left us",
                     msg.username);
          break;
        case NEW_MESSAGE:
          mvwprintw (chat_win, i - start + 1, 1, "%s: %s", msg.username,
                     msg.text);
          break;
        }
    }
  wrefresh (chat_win);
  pthread_mutex_unlock (&ui_mutex);
}

void
redraw_users ()
{
  pthread_mutex_lock (&ui_mutex);
  werase (users_win);
  box (users_win, 0, 0);

  mvwprintw (users_win, 0, 1, "users:");
  for (int i = 0; i < users_count; ++i)
    {
      mvwprintw (users_win, i + 1, 1, "%s", users[i]);
    }
  wrefresh (users_win);
  pthread_mutex_unlock (&ui_mutex);
}

void
add_user (const char *name)
{
  if (!name)
    return;

  if (users_count < MAX_CLIENTS)
    {
      strncpy (users[users_count], name, sizeof (users[users_count]) - 1);
      users[users_count][sizeof (users[users_count]) - 1] = '\0';
      users_count++;
      redraw_users ();
    }
}

void
remove_user (const char *name)
{
  for (int i = 0; i < users_count; ++i)
    {
      if (strcmp (users[i], name) == 0)
        {
          for (int j = i; j < users_count - 1; ++j)
            strcpy (users[j], users[j + 1]);
          users_count--;
          redraw_users ();
          break;
        }
    }
}

void *
receiver_thread (void *arg)
{
  int shm_fd = shm_open (SHM_NAME, O_RDONLY, 0644);
  if (shm_fd == -1)
    {
      perror ("shm_open");
      exit (1);
    }

  shared_data_t *shared_memory
      = mmap (NULL, sizeof (shared_data_t), PROT_READ, MAP_SHARED, shm_fd, 0);
  if (shared_memory == MAP_FAILED)
    {
      perror ("mmap");
      exit (1);
    }

  while (1)
    {
      while (last_read_pos != shared_memory->current_pos)
        {
          message_t msg = shared_memory->history[last_read_pos];

          chat_history[chat_lines] = msg;
          chat_lines++;
          if (chat_lines >= HISTORY_SIZE)
            chat_lines = 0;

          redraw_chat ();

          last_read_pos = (last_read_pos + 1) % HISTORY_SIZE;

          pthread_mutex_lock (&ui_mutex);
          users_count = shared_memory->users_count;
          for (int i = 0; i < users_count; ++i)
            {
              strncpy (users[i], shared_memory->users[i],
                       sizeof (users[i]) - 1);
              users[i][sizeof (users[i]) - 1] = '\0';
            }
          pthread_mutex_unlock (&ui_mutex);

          redraw_users ();
        }
    }
}

void *
sender_thread (void *arg)
{
  int leaving = 0;
  sender_args_t *args = (sender_args_t *)arg;
  message_t msg;
  strncpy (msg.username, args->name, MAX_NAME - 1);
  msg.username[MAX_NAME - 1] = '\0';
  msg.pid = args->uid;

  while (1)
    {
      pthread_mutex_lock (&ui_mutex);
      werase (input_win);
      box (input_win, 0, 0);
      mvwprintw (input_win, 1, 1, ">> ");
      wrefresh (input_win);
      pthread_mutex_unlock (&ui_mutex);
      char text[MAX_TEXT] = { 0 };

      echo ();
      curs_set (1);
      wgetnstr (input_win, text, sizeof (text) - 1);
      curs_set (0);
      noecho ();

      if (strlen (text) == 0)
        continue;

      if (strcmp (text, "/q") == 0)
        {
          msg.type = USER_LEFT;
          leaving = 1;
        }
      else
        {
          msg.type = NEW_MESSAGE;
          strncpy (msg.text, text, MAX_TEXT - 1);
          msg.text[MAX_TEXT - 1] = '\0';
        }
      mq_send (args->service_mq, (char *)&msg, sizeof (message_t), 0);
      if (leaving)
        break;
    }
  return NULL;
}
int
main ()
{
  mqd_t service_mq;

  service_mq = mq_open (QUEUE_NAME, O_WRONLY);
  if (service_mq == (mqd_t)-1)
    {
      perror ("client: mq_open");
      exit (1);
    }

  char name[10];
  printf ("What's your name? ");
  scanf ("%9s", name);
  getchar ();

  pid_t uid = getpid ();
  message_t msg;
  msg.type = USER_CAME;
  strncpy (msg.username, name, MAX_NAME - 1);
  msg.username[MAX_NAME - 1] = '\0';
  msg.pid = uid;
  mq_send (service_mq, (char *)&msg, sizeof (message_t), 0);

  sleep (1);

  initscr ();
  cbreak ();
  noecho ();
  keypad (stdscr, TRUE);
  start_color ();
  use_default_colors ();

  init_color (COLOR_DARK_BLUE, 10, 100, 355);
  init_color (COLOR_PINK, 900, 800, 750);

  init_pair (1, COLOR_PINK, COLOR_DARK_BLUE);

  int chat_height = LINES - 4;
  int users_width = 20;
  chat_win = newwin (chat_height, COLS - users_width, 0, 0);
  users_win = newwin (chat_height, users_width, 0, COLS - users_width);
  input_win = newwin (4, COLS, chat_height, 0);

  wbkgd (chat_win, COLOR_PAIR (1));
  wbkgd (users_win, COLOR_PAIR (1));
  wbkgd (input_win, COLOR_PAIR (1));

  scrollok (chat_win, TRUE);
  keypad (input_win, TRUE);

  box (chat_win, 0, 0);
  box (users_win, 0, 0);
  box (input_win, 0, 0);
  mvwprintw (users_win, 0, 1, "users:");
  mvwprintw (input_win, 1, 1, ">> ");

  wrefresh (chat_win);
  wrefresh (users_win);
  wrefresh (input_win);

  pthread_t recv_tid, send_tid;
  sender_args_t send_args = { .service_mq = service_mq };
  strncpy (send_args.name, name, sizeof (send_args.name));
  send_args.uid = uid;

  pthread_create (&recv_tid, NULL, receiver_thread, NULL);
  pthread_create (&send_tid, NULL, sender_thread, &send_args);

  pthread_join (send_tid, NULL);
  pthread_cancel (recv_tid);

  mq_close (service_mq);

  delwin (chat_win);
  delwin (users_win);
  delwin (input_win);
  endwin ();

  return 0;
}
