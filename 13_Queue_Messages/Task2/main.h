#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_SIZE 1400

#define HISTORY_SIZE 100
#define QUEUE "/serqueue"

typedef enum
{
  MSG_REG,
  MSG_CHAT,
  MSG_EXIT
} msg_type;

typedef struct message_info
{
  msg_type type;
  char sender[128];
  char text[MAX_SIZE];
  char queue_name[128];

} message_info;

typedef struct client_info
{
  char name[128];
  mqd_t qd; // queue descriptor
} client_info;
