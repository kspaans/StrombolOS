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
  char msg[1];
  msg[0]  = 'g';
  int tid;

  switch (channel) {
    case COM1: tid = WhoIs ("com1"); break;
    case COM2: tid = WhoIs ("com2"); break;
    default: PANIC;
  }
//  bwprintf (COM2, "getting from tid %d!\n", tid);
  if (Send(tid, msg, 1, &c, 1) != 1){ DPRINTERR ("CONFUSE??\n"); PANIC; }
//  bwprintf (COM2, "success?\n");
  return c;
}

int Getc_r (int ch) {
  char c;
  char msg[1];
  msg[0] = 'i';
  int tid;
  switch (ch) {
    case COM1: tid = WhoIs ("com1"); break;
    case COM2: tid = WhoIs ("com2"); break;
    default: PANIC;
  }
  int ret = Send (tid, msg, 1, &c, 1);
  if (ret>1)  PANIC;
  if (ret==0) return -1;
  else        return c;
}

int Putc(int channel, char ch)
{
  char msg[2];
  msg[0] = 'p';
  msg[1] = ch;
  int tid;
  switch (channel) {
    case COM1: tid = WhoIs ("com1"); break;
    case COM2: tid = WhoIs ("com2"); break;
    default: PANIC;
  }
  if (Send (tid, msg, 2, NULL, 0) != 0) PANIC;
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
