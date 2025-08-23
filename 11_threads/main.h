#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void *print_thread_ind (void *arg);
void run_5_threads ();

void *increment (void *arg);
void increment_threads ();

//////////////////////////////// 3rd task:

#define SHOP_COUNT 5
#define CLIENT_COUNT 3

typedef struct shop_t
{
  int index;
  int items;
  pthread_mutex_t mutex;
} shop_t;

typedef struct client_t
{
  int index;
  int needs;
} client_t;

typedef struct market_t
{
  client_t client;
  shop_t *shops;

} market_t;

void market ();
void *client_routine (void *arg);
void *supplier (void *arg);