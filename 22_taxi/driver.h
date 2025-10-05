#ifndef DRIVER_MANAGER_H
#define DRIVER_MANAGER_H
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_DRIVERS 20

typedef enum
{
  ERROR = -1,
  OK = 0,
  IMBUSY = 1
} return_status;

typedef enum
{
  PIPE_READ,
  PIPE_WRITE
} pipe_num;

typedef enum
{
  BUSY,
  AVAILABLE
} driver_status;

typedef enum
{
  SEND_TASK,
  GET_STATUS
} task_type;

typedef struct
{
  pid_t pid;
  driver_status status;
  time_t time_left;
} driver_response;

typedef struct
{
  pid_t pid;
  driver_status status;
  time_t time_left;
  int command_fd[2];
  int response_fd[2];
  driver_response response;
} driver_t;

typedef struct
{
  task_type type;
  time_t task_timer;
  pid_t pid;
} task_t;

int createDriver (driver_t drivers[], int *driver_count);
int sendTaskToDriver (driver_t drivers[], int driver_count, pid_t pid,
                      int task_timer, driver_t *result);
int getDriverStatus (driver_t drivers[], int driver_count, pid_t pid,
                     driver_t *result);
int getAllDrivers (driver_t drivers[], int driver_count);
int driverProcess (driver_t driver_info);
int findDriverByPid (driver_t drivers[], int driver_count, pid_t pid);
void cleanupDrivers (driver_t drivers[], int *driver_count);

#endif
