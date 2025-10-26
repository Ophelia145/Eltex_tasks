#include "listening_server.h"

int listen_sock;

void
handle_sigint (int sig)
{
  (void)sig;
  printf ("\nСервер завершает работу...\n");
  close (listen_sock);
  exit (0);
}
typedef struct
{
  int sockets[MAX_QUEUE];
  int front, rear, count;
  pthread_mutex_t mutex;
  pthread_cond_t not_empty;
  pthread_cond_t not_full;
} Queue;

Queue client_queue;
int listen_sock;

void
queue_init (Queue *q)
{
  q->front = q->rear = q->count = 0;
  pthread_mutex_init (&q->mutex, NULL);
  pthread_cond_init (&q->not_empty, NULL);
  pthread_cond_init (&q->not_full, NULL);
}

void
enqueue (Queue *q, int client_sock)
{
  pthread_mutex_lock (&q->mutex);

  while (q->count == MAX_QUEUE)
    pthread_cond_wait (&q->not_full, &q->mutex);

  q->sockets[q->rear] = client_sock;
  q->rear = (q->rear + 1) % MAX_QUEUE;
  q->count++;

  pthread_cond_signal (&q->not_empty);
  pthread_mutex_unlock (&q->mutex);
}

int
dequeue (Queue *q)
{
  pthread_mutex_lock (&q->mutex);

  while (q->count == 0)
    pthread_cond_wait (&q->not_empty, &q->mutex);

  int client_sock = q->sockets[q->front];
  q->front = (q->front + 1) % MAX_QUEUE;
  q->count--;

  pthread_cond_signal (&q->not_full);
  pthread_mutex_unlock (&q->mutex);

  return client_sock;
}

void
handle_client (int client_sock)
{
  time_t now = time (NULL);
  char buffer[128];
  snprintf (buffer, sizeof (buffer), "Текущее время: %s", ctime (&now));
  send (client_sock, buffer, strlen (buffer), 0);
  close (client_sock);
}

void *
worker_thread (void *arg)
{
  (void)arg;
  while (1)
    {
      int client_sock = dequeue (&client_queue);
      handle_client (client_sock);
    }
  return NULL;
}

int
main ()
{
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_size = sizeof (client_addr);

  signal (SIGINT, handle_sigint);

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

  if (listen (listen_sock, 10) < 0)
    {
      perror ("listen");
      exit (EXIT_FAILURE);
    }

  printf ("Сервер запущен на порту %d...\n", PORT);

  queue_init (&client_queue);

  pthread_t threads[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_create (&threads[i], NULL, worker_thread, NULL);

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
      enqueue (&client_queue, client_sock);
    }

  close (listen_sock);
  return 0;
}
