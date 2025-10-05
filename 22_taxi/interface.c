#include "driver.h"

int
main ()
{
  driver_t drivers[MAX_DRIVERS];
  int driver_count = 0;
  char command[100];
  char args[3][50];
  int arg_count;

  printf ("Taxi system\n");
  printf ("Commands: create_driver (cd),\n send_task <pid> <timer> (st pid "
          "timer),\n get_status (gs pid)"
          "<pid>,\n get_drivers (gd),\n exit (e)\n");
  while (1)
    {
      printf ("> ");
      fflush (stdout);

      if (!fgets (command, sizeof (command), stdin))
        break;
      arg_count = sscanf (command, "%s %s %s", args[0], args[1], args[2]);
      if (arg_count <= 0)
        continue;

      if (strcmp (args[0], "cd") == 0)
        {
          if (createDriver (drivers, &driver_count) == OK)
            printf ("Driver created with PID: %d\n",
                    drivers[driver_count - 1].pid);
          else
            printf ("Failed to create driver or max drivers reached\n");
        }
      else if (strcmp (args[0], "st") == 0 && arg_count == 3)
        {
          pid_t pid = atoi (args[1]);
          time_t timer = atol (args[2]);
          driver_t result;
          int ss
              = sendTaskToDriver (drivers, driver_count, pid, timer, &result);
          if (ss == OK)
            printf ("Task sent to driver %d for %ld seconds\n", pid, timer);
          else if (ss == ERROR)
            printf ("Failed to send task to driver %d\n", pid);
          else
            printf ("\n%d is busy\n", pid);
        }
      else if (strcmp (args[0], "gs") == 0 && arg_count == 2)
        {
          pid_t pid = atoi (args[1]);
          driver_t result;
          if (getDriverStatus (drivers, driver_count, pid, &result) == OK)
            printf ("Driver %d status: %s, time left: %ld\n", pid,
                    result.response.status == BUSY ? "BUSY" : "AVAILABLE",
                    result.response.time_left);
          else
            printf ("Driver %d not found\n", pid);
        }
      else if (strcmp (args[0], "gd") == 0)
        {
          if (getAllDrivers (drivers, driver_count) == OK)
            {
              for (int i = 0; i < driver_count; i++)
                printf ("Driver %d: %s, time left %ld\n", drivers[i].pid,
                        drivers[i].response.status == BUSY ? "BUSY"
                                                           : "AVAILABLE",
                        drivers[i].response.time_left);
            }
        }
      else if (strcmp (args[0], "e") == 0)
        {
          break;
        }
      else
        {
          printf ("wrong args or something that idk\n");
        }
    }

  cleanupDrivers (drivers, &driver_count);

  return 0;
}
