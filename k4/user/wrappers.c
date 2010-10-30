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
  //DPRINTOK("WHOIS from client %d, msg %s\r\n", MyTid(), msg);
  Send (1, msg, 6, (char*)&tid, 4);
  //DPRINTOK("WHOIS answer %d to client %d\r\n", tid, MyTid());
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
  char c;
  char buf = 'g';
  int tid1, tid2;

  tid1 = WhoIs("com1");
  //tid2 = WhoIs("com2");
  if (channel) { // COM2
    if (Send(tid1, &buf, 1, &c, 1) != 1) PANIC;
  }
  else {
    //if (Send(tid2, &buf, 1, &c, 1) != 1) PANIC;
  }
  return c;
}

int Putc(int channel, char ch)
{
  return 0;
}

int Delay(int ticks)
{
  int clock_tid;
  int r;
  char msg[5];
  char *ip;

  clock_tid = WhoIs("clck");
  clock_tid = 2;

  msg[0] = 'd';
  ip = (char *)&ticks;
  msg[1] = *ip++;
  msg[2] = *ip++;
  msg[3] = *ip++;
  msg[4] = *ip++;
  DPRINT("Delayed tid %d with msg \'%x%x%x%x%x\' to server at tid %d\r\n",
         MyTid(), msg[0], msg[1], msg[2], msg[3], msg[4], clock_tid);

  //if (t == 5) { bwputstr(COM2, "YAYA\r\n"); }
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
