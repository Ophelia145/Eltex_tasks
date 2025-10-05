#include "driver.h"
#include <sys/wait.h>

static int g_continue = 1;

static void
handleSigusr1 (int signum)
{
  (void)signum;
  g_continue = 0;
}

static void
childCleanup (driver_t driver)
{
  close (driver.command_fd[PIPE_READ]);
  close (driver.response_fd[PIPE_WRITE]);
}
static int
taskGetStatus (driver_t *driver_info, time_t task_timer_end)
{
  driver_response response;
  response.status = driver_info->status;

  if (driver_info->status == BUSY)
    {
      time_t now = time (NULL);
      response.time_left = (task_timer_end > now) ? task_timer_end - now : 0;
    }
  else
    {
      response.time_left = 0;
    }

  if (write (driver_info->response_fd[PIPE_WRITE], &response,
             sizeof (response))
      != sizeof (response))
    {
      return ERROR;
    }
  driver_info->response = response;

  return OK;
}

static int
taskSendTask (driver_t *driver_info, driver_status *status,
              time_t *task_timer_end, task_t task)
{
  driver_response response;

  if (*status == AVAILABLE)
    {

      *task_timer_end = time (NULL) + task.task_timer;
      driver_info->status = *status = BUSY;
      driver_info->time_left = task.task_timer;

      response.status = AVAILABLE;
      response.time_left = task.task_timer;
    }
  else
    {
      time_t now = time (NULL);
      response.status = BUSY;
      response.time_left = (*task_timer_end > now) ? *task_timer_end - now : 0;
    }

  if (write (driver_info->response_fd[PIPE_WRITE], &response,
             sizeof (response))
      != sizeof (response))
    return ERROR;

  driver_info->response = response;
  return OK;
}

static int
taskProcess (driver_t *driver_info, driver_status *status,
             time_t *task_timer_end)
{
  task_t task;
  if (read (driver_info->command_fd[PIPE_READ], &task, sizeof (task))
      != sizeof (task))
    return ERROR;

  if (task.type == GET_STATUS)
    return taskGetStatus (driver_info, *task_timer_end);
  else if (task.type == SEND_TASK)
    return taskSendTask (driver_info, status, task_timer_end, task);

  return ERROR;
}

static time_t
timeCalculation (driver_t *driver_info, driver_status *status,
                 const time_t *time_end)
{
  time_t current = time (NULL);
  if (current >= *time_end)
    {
      *status = AVAILABLE;
      driver_info->status = AVAILABLE;
      driver_info->time_left = 0;
      return -1;
    }
  else
    {
      driver_info->time_left = *time_end - current;
      return driver_info->time_left * 1000;
    }
}

int
driverProcess (driver_t driver_info)
{
  driver_status status = AVAILABLE;
  time_t task_timer_end = 0;

  driver_info.pid = getpid ();
  driver_info.status = status;
  driver_info.time_left = 0;

  struct pollfd fds[] = { { driver_info.command_fd[PIPE_READ], POLLIN, 0 } };

  struct sigaction sa = { 0 };
  sa.sa_handler = handleSigusr1;
  sigaction (SIGUSR1, &sa, NULL);

  int start_status = OK;
  if (write (driver_info.response_fd[PIPE_WRITE], &start_status,
             sizeof (start_status))
      != sizeof (start_status))
    {
      perror ("write failed");
    }

  time_t timeout = -1;
  while (g_continue)
    {
      int poll_result = poll (fds, 1, (int)timeout);
      if (poll_result == -1)
        continue;

      if (status == BUSY)
        {
          timeout = timeCalculation (&driver_info, &status, &task_timer_end);
        }
      else
        {
          timeout = -1;
        }

      if (poll_result == 0)
        {
          driver_info.status = status;
          driver_info.time_left = task_timer_end;
          continue;
        }

      if (fds[0].revents & POLLIN)
        taskProcess (&driver_info, &status, &task_timer_end);
    }

  childCleanup (driver_info);
  return OK;
}