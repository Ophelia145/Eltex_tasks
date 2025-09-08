#include "main.h"

int
main ()
{
  printf ("\n...SysV....\n");
  key_t key = ftok ("server_sysV", 1);
  if (key == -1)
    {
      perror ("ftok");
      exit (1);
    }
  // 0666 : 0-octall(восьмиричная запись)
  int msgid = msgget (key,
                      IPC_CREAT | 0666); //макрос = if объект с key не
                                         //существует - создать else - get id
  if (msgid == -1)
    {
      perror ("msgget");
      exit (1);
    }
  msgbuf msg;
  msg.msgtyp = 1;
  strcpy (msg.mdata, "Hi!");
  if (msgsnd (msgid, &msg, sizeof (msg.mdata), 0) == -1)
    {
      perror ("msgsnd");
      exit (1);
    }

  printf ("Server said %s\n", msg.mdata);

  if (msgrcv (msgid, &msg, sizeof (msg.mdata), 2, 0) == -1)
    {
      perror ("msgrcv");
      exit (1);
    }

  printf ("Server got %s\n", msg.mdata);

  msgctl (msgid, IPC_RMID, NULL);

  return 0;
}