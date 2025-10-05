#include "driver.h"
#include <sys/wait.h>

static void
cleanupOnError (driver_t *driver)
{
  perror ("couldn't create process or pipe.\n");
  close (driver->command_fd[PIPE_READ]);
  close (driver->command_fd[PIPE_WRITE]);
  close (driver->response_fd[PIPE_READ]);
  close (driver->response_fd[PIPE_WRITE]);
}

int
createDriver (driver_t drivers[], int *driver_count)
{
  if (*driver_count >= MAX_DRIVERS)
    return ERROR;

  driver_t *driver = &drivers[*driver_count];

  if (pipe (driver->command_fd) == -1 || pipe (driver->response_fd) == -1)
    {
      perror ("pipe");
      return ERROR;
    }

  pid_t pid = fork ();
  if (pid == -1)
    {
      cleanupOnError (driver);
      return ERROR;
    }
  else if (pid == 0)
    {
      close (driver->response_fd[PIPE_READ]);
      close (driver->command_fd[PIPE_WRITE]);
      if (driverProcess (*driver) == ERROR)
        exit (EXIT_FAILURE);
      exit (EXIT_SUCCESS);
    }
  else
    {
      int child_status;
      if (read (driver->response_fd[PIPE_READ], &child_status,
                sizeof (child_status))
          != sizeof (child_status))
        {
          cleanupOnError (driver);
          return ERROR;
        }

      close (driver->response_fd[PIPE_WRITE]);
      close (driver->command_fd[PIPE_READ]);

      driver->pid = pid;
      driver->status = AVAILABLE;
      driver->time_left = 0;

      (*driver_count)++;
      return OK;
    }
}
int
sendTaskToDriver (driver_t drivers[], int driver_count, pid_t pid,
                  int task_timer, driver_t *result)
{
  int index = findDriverByPid (drivers, driver_count, pid);
  if (index == -1)
    return ERROR;

  driver_t *driver = &drivers[index];
  task_t task = { .type = SEND_TASK, .task_timer = task_timer, .pid = pid };

  if (write (driver->command_fd[PIPE_WRITE], &task, sizeof (task))
      != sizeof (task))
    return ERROR;

  driver_response response;
  if (read (driver->response_fd[PIPE_READ], &response, sizeof (response))
      != sizeof (response))
    return ERROR;

  result->response = response;

  if (response.status == BUSY && response.time_left > 0)
    {
      return IMBUSY;
    }

  return OK;
}

int
getDriverStatus (driver_t drivers[], int driver_count, pid_t pid,
                 driver_t *result)
{
  int index = findDriverByPid (drivers, driver_count, pid);
  if (index == -1)
    return ERROR;

  driver_t *driver = &drivers[index];
  task_t task = { .type = GET_STATUS, .task_timer = 0, .pid = 0 };

  if (write (driver->command_fd[PIPE_WRITE], &task, sizeof (task))
      != sizeof (task))
    return ERROR;

  driver_response response;
  if (read (driver->response_fd[PIPE_READ], &response, sizeof (response))
      == -1)
    return ERROR;

  result->response = response;
  return OK;
}

int
getAllDrivers (driver_t drivers[], int driver_count)
{
  if (!drivers)
    return ERROR;

  task_t task = { .type = GET_STATUS, .task_timer = 0, .pid = 0 };

  for (int i = 0; i < driver_count; i++)
    if (write (drivers[i].command_fd[PIPE_WRITE], &task, sizeof (task))
        != sizeof (task))
      return ERROR;

  for (int i = 0; i < driver_count; i++)
    {
      driver_response response;
      if (read (drivers[i].response_fd[PIPE_READ], &response,
                sizeof (response))
          != sizeof (response))
        return ERROR;
      drivers[i].response = response;
    }

  return OK;
}

int
findDriverByPid (driver_t drivers[], int driver_count, pid_t pid)
{
  if (!drivers)
    return ERROR;
  for (int i = 0; i < driver_count; i++)
    if (drivers[i].pid == pid)
      return i;
  return ERROR;
}

void
cleanupDrivers (driver_t drivers[], int *driver_count)
{
  for (int i = 0; i < *driver_count; i++)
    {
      kill (drivers[i].pid, SIGUSR1);
      close (drivers[i].command_fd[PIPE_WRITE]);
      close (drivers[i].response_fd[PIPE_READ]);

      int status;
      wait (&status);
      printf ("Driver %d exited with status %d\n", drivers[i].pid, status);
    }
  *driver_count = 0;
}
