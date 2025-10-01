#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SHM_KEY 1234

// posix
#include <fcntl.h>
#include <sys/mman.h>

#define SHM_NAME "/myshm"