#include "main.h"

int
main ()
{
  key_t key = ftok ("server_sysV", 1);
  int msgid = msgget (key, 0666);
  if (msgid == -1)
    {
      perror ("msgget");
      exit (1);
    }
  msgbuf msg;
  msgrcv (msgid, &msg, sizeof (msg.mdata), 0, 0);
  printf ("Client got %s\n", msg.mdata);
  msg.msgtyp = 2;
  strcpy (msg.mdata, "Hello!");
  msgsnd (msgid, &msg, sizeof (msg.mdata), 0);
  printf ("Client said %s\n", msg.mdata);
  return 0;
}