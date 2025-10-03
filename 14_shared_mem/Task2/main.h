#ifndef CHAT_H
#define CHAT_H
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_NAME 21
#define MAX_TEXT 256
#define HISTORY_SIZE 100
#define QUEUE_NAME "/chat_service_queue"
#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_sem"

#define MAX_CLIENTS 10
enum msg_type
{
  USER_CAME = 0,
  NEW_MESSAGE,
  USER_LEFT,
};

typedef struct
{
  enum msg_type type;
  char username[MAX_NAME];
  char text[MAX_TEXT];
  pid_t pid;
} message_t;

typedef struct
{
  message_t history[HISTORY_SIZE];
  char users[MAX_CLIENTS][MAX_NAME];
  int users_count;
  int current_pos;
  int history_count;
} shared_data_t;

#endif