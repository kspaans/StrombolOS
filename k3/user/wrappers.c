#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "lib.h"
#include "usyscall.h"
#include "../kernel/switch.h"
#include "../ktests/tests.h"

/*
int WhoIs (char *name) {
  int rply;
  char msg[2+strlen(name)];
  msg[0] = 'w';
  strcpy (msg+1, name);
  DPRINT("name '%s', sending message '%s'\r\n", name, msg);
  Send (1, msg, 2+strlen(name), (char*)&rply, 4);
  DPRINT("got reply %d\r\n", rply);
  return rply;
}
*/
int WhoIs (char *name) {
  int tid;
  char msg[6];
  msg[0] = 'w';
  strcpy((msg+1), name);
  Send (1, msg, 6, (char*)&tid, 4);
  return tid; // fix
}

/*
int RegisterAs (char *name) {
  char msg[2+strlen(name)];
  msg[0] = 'r';
  strcpy (msg+1, name);
  DPRINT("name '%s', sending message '%s'\r\n", name, msg);
  Send (1, msg, strlen(name)+2, 0, 0);
  return 0;
}
*/
int RegisterAs (char *name) {
  char msg[6];
  msg[0] = 'r';
  strcpy ((msg+1), name);
//  bwprintf (COM2, "RegisterAs: Gonna send %s.\r\n",msg);
  Send (1, msg, 6, 0, 0);
//  bwprintf (COM2, "RegisterAs: Back.\r\n");
  return 0; // fix
}

int Getc(int channel)
{
  /* To be implemented later
  char c;
  int i;

  i = WhoIs("serial");
  Send(1, );
  */
  return bwgetc(channel);
}

int Delay(int ticks)
{
  int clock_tid;
  int r;
  char msg[5];
  char *ip;

  clock_tid = WhoIs("clck");

  msg[0] = 'd';
  ip = (char *)&ticks;
  msg[1] = *ip++;
  msg[2] = *ip++;
  msg[3] = *ip++;
  msg[4] = *ip++;
  DPRINT("Delayed tid %d with msg \'%x%x%x%x%x\' to server at tid %d\r\n",
         MyTid(), msg[0], msg[1], msg[2], msg[3], msg[4], clock_tid);

  r = Send(clock_tid, msg, 5, NULL, 0);
  if (r < 0) {
    DPRINTERR("Send() from inside Delay() failed with retval %d\r\n", r);
    PANIC;
  }
  return 0;
}

int Time()
{
  int clock_tid;
  int time, r;

  clock_tid = WhoIs("clck");
  r = Send(clock_tid, "t", 1, (char *)&time, 4);
  /* XXX check return value XXX */
  return time;
}
