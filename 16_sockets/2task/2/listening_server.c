#include "listening_server.h"
//!ВЫХОД ИЗ ПРОГРАММЫ: Ctrl+c
int listen_sock;

void
handle_sigint (int sig)
{
  (void)sig;
  printf ("\nСервер завершает работу...\n");
  close (listen_sock);
  exit (0);
}

int client_queue[QUEUE_SIZE];
int queue_front = 0, queue_rear = 0, queue_count = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

void
enqueue_client (int client_sock)
{
  pthread_mutex_lock (&queue_mutex);
  while (queue_count == QUEUE_SIZE)
    {
      pthread_mutex_unlock (&queue_mutex);
      close (client_sock);
      return;
    }
  client_queue[queue_rear] = client_sock;
  queue_rear = (queue_rear + 1) % QUEUE_SIZE;
  queue_count++;
  pthread_cond_signal (&queue_not_empty);
  pthread_mutex_unlock (&queue_mutex);
}

int
dequeue_client ()
{
  pthread_mutex_lock (&queue_mutex);
  while (queue_count == 0)
    pthread_cond_wait (&queue_not_empty, &queue_mutex);
  int client_sock = client_queue[queue_front];
  queue_front = (queue_front + 1) % QUEUE_SIZE;
  queue_count--;
  pthread_mutex_unlock (&queue_mutex);
  return client_sock;
}

void *
handle_client (void *arg)
{
  (void)arg;
  while (1)
    {
      int client_sock = dequeue_client ();
      time_t now = time (NULL);
      char *time_str = ctime (&now);
      send (client_sock, time_str, strlen (time_str), 0);
      close (client_sock);
      printf ("Thread %lu : Обслужен клиент, готов к следующему.\n",
              pthread_self ());
    }
  return NULL;
}

int
main ()
{

  signal (SIGINT, handle_sigint);

  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size = sizeof (client_addr);

  if ((listen_sock = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror ("socket");
      exit (EXIT_FAILURE);
    }

  int opt = 1;
  setsockopt (listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons (PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind (listen_sock, (struct sockaddr *)&server_addr, sizeof (server_addr))
      < 0)
    {
      perror ("bind");
      exit (EXIT_FAILURE);
    }

  if (listen (listen_sock, 5) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }

  printf ("Сервер запущен на порту %d\n", PORT);

  pthread_t threads[MAX_THREADS];
  for (int i = 0; i < MAX_THREADS; i++)
    pthread_create (&threads[i], NULL, handle_client, NULL);

  while (1)
    {
      int client_sock
          = accept (listen_sock, (struct sockaddr *)&client_addr, &addr_size);
      if (client_sock < 0)
        {
          perror ("accept");
          continue;
        }
      printf ("Клиент подключился: %s\n", inet_ntoa (client_addr.sin_addr));
      enqueue_client (client_sock);
    }

  return 0;
}
