#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

// sysV
typedef struct msgbuf
{
  long msgtyp;
  char mdata[100];
} msgbuf;

// posix

#define MAX_SIZE 1024 // 1024*8байт (long) == 8 кбайт/ 16 кбайт
#include <mqueue.h>
