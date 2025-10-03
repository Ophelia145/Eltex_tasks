#include "main.h"

typedef struct user_t
{
  char name[MAX_NAME];
  pid_t pid;
} user_t;

int running = 1;
shared_data_t *shared_memory = NULL;
int shm_fd;

int history_count = 0;

int
find_user (user_t *users, int count, pid_t pid)
{
  for (int i = 0; i < count; ++i)
    if (users[i].pid == pid)
      return i;
  return -1;
}

void
add_to_history (message_t *msg)
{
  if (!msg)
    return;

  memcpy (&shared_memory->history[shared_memory->current_pos], msg,
          sizeof (message_t));
  shared_memory->current_pos = (shared_memory->current_pos + 1) % HISTORY_SIZE;
  if (shared_memory->history_count < HISTORY_SIZE)
    shared_memory->history_count++;
}

void
clean (mqd_t service_mq)
{
  munmap (shared_memory, sizeof (shared_data_t));
  shm_unlink (SHM_NAME);
  mq_close (service_mq);
  mq_unlink (QUEUE_NAME);
}

void *
input_thread (void *arg)
{
  mqd_t service_mq = *(mqd_t *)arg;
  char command[32];
  while (running)
    {
      if (fgets (command, sizeof (command), stdin))
        {
          if (strncmp (command, "/q", 5) == 0)
            {
              printf ("shutting down server...\n");
              running = 0;

              message_t m = {
                .type = NEW_MESSAGE,
                .username = "server",
                .text = "shutdown",
              };
              mq_send (service_mq, (char *)&m, sizeof (message_t), 0);
              break;
            }
        }
    }
  return NULL;
}

int
main ()
{
  printf ("type /q to shut down server.\n");

  shm_fd = shm_open (SHM_NAME, O_CREAT | O_RDWR, 0644);
  if (shm_fd == -1)
    {
      perror ("shm_open");
      exit (1);
    }

  if (ftruncate (shm_fd, sizeof (shared_data_t)) == -1)
    {
      perror ("ftruncate");
      exit (1);
    }

  shared_memory = mmap (NULL, sizeof (shared_data_t), PROT_READ | PROT_WRITE,
                        MAP_SHARED, shm_fd, 0);
  if (shared_memory == MAP_FAILED)
    {
      perror ("mmap");
      exit (1);
    }
  memset (shared_memory, 0, sizeof (shared_data_t));

  mqd_t service_mq;
  user_t users[10];
  int active_users = 0;

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = sizeof (message_t);
  attr.mq_curmsgs = 0;

  service_mq = mq_open (QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
  if (service_mq == (mqd_t)-1)
    {
      perror ("server: mq_open");
      exit (1);
    }

  pthread_t input_tid;
  pthread_create (&input_tid, NULL, input_thread, &service_mq);

  while (running)
    {
      message_t msg;
      if (mq_receive (service_mq, (char *)&msg, sizeof (message_t), NULL)
          == -1)
        {
          if (!running)
            break;
          perror ("server: mq_receive");
          break;
        }
      if (!running)
        break;

      switch (msg.type)
        {
        case USER_CAME:
          {
            if (active_users < MAX_CLIENTS)
              {
                users[active_users].pid = msg.pid;
                strncpy (users[active_users].name, msg.username, MAX_NAME - 1);
                users[active_users].name[MAX_NAME - 1] = '\0';

                strncpy (shared_memory->users[shared_memory->users_count],
                         msg.username, MAX_NAME - 1);
                shared_memory->users[shared_memory->users_count][MAX_NAME - 1]
                    = '\0';
                shared_memory->users_count++;
                active_users++;
                add_to_history (&msg);
                printf ("new user: %s\n", msg.username);
              }
            break;
          }

        case NEW_MESSAGE:
          {
            add_to_history (&msg);
            printf ("new message: %s: %s\n", msg.username, msg.text);
            break;
          }

        case USER_LEFT:
          {
            int idx = find_user (users, active_users, msg.pid);
            if (idx != -1)
              {
                for (int j = idx; j < active_users - 1; ++j)
                  users[j] = users[j + 1];
                active_users--;
                for (int j = idx; j < shared_memory->users_count - 1; ++j)
                  strcpy (shared_memory->users[j],
                          shared_memory->users[j + 1]);
                shared_memory->users_count--;
                add_to_history (&msg);
                printf ("user left: %s\n", msg.username);
              }
            break;
          }
        }
    }

  pthread_join (input_tid, NULL);
  clean (service_mq);
  printf ("server stopped.\n");
  return 0;
}