#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "lib.h"
#include "usyscall.h"

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
  int r, ml = 0, rl = 0;
  char *msg = (void *)0, *rpl = (void *)0;

  clock_tid = WhoIs("clock");
  r = Send(clock_tid, msg, ml, rpl, rl); /* need to figure out "interface" to
                                            clock server */
  return 0;
}
