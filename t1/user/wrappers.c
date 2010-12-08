#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <lock.h>
#include "lib.h"
#include "usyscall.h"
#include "../kernel/switch.h"
#include "../ktests/tests.h"
#include "../servers/track.h"

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

void LockAcquire (int l) {
  char msg[2];
  msg[0] = 'A';
  msg[1] = (char)l;
  Send (WhoIs("lk"), msg, 2, NULL, 0);
}

void LockRelease (int l) {
  char msg[2];
  msg[0] = 'R';
  msg[1] = (char)l;
  Send (WhoIs("lk"), msg, 2, NULL, 0);
}

int ReserveChunks(int sensor, int distance, int goal)
{
bwputc(COM2, 'b');
  int track_tid;
  int d = 0; // distance reserved so far, in mm
  int r;
  char c;
  struct msg out;
  struct trip next;
  //char sname[4];


  track_tid = WhoIs("trak"); // TODO: speed me up

  out.id = '~';
  out.d1 = sensor;
  out.d2 = goal;
  r = Send(track_tid, (char *)&out, sizeof(struct msg), NULL, 0);
  if (r != 0) PANIC;
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');
bwputc(COM2, 'c');

  out.id = 'r';
  out.d1 = sensor;
  r = Send(track_tid, (char *)&out, sizeof(struct msg), &c, 1);
  if (r == 1) { // could not reserve
    return -1;
  }
//bwputc(COM2, 'd');

  //sens_id_to_name(sensor, sname);
  //LockAcquire(COM2_W_LOCK);
  //bwprintf(COM2, "ReserveChunks: wanting sensor %s for dist %dmm\r\n", sname,
  //distance);
  //LockRelease(COM2_W_LOCK);
  while (d < distance) {
    if (sensor/2 == goal/2) {
      return 1337;
    }
    out.id = 'n';
    out.d1 = sensor;
    out.d2 = goal;
    if (Send(track_tid, (char *)&out,  sizeof(struct msg), (char *)&next,
             sizeof(struct trip)) != sizeof(next)) {
      PANIC;
    }
    if (next.destination == -1) { // running to a dead-end
      return -2;
    }
    if (next.destnode.id == goal/2) { return 1337; }
    out.id = 'r';
    out.d1 = next.destination;
    r = Send(track_tid, (char *)&out, sizeof(struct msg), &c, 1);
    if (r == 1) { // could not reserve
      return -1;
    }
    else if (r != 0) {
      PANIC;
    }
    d += next.distance;
    //sens_id_to_name(next.destination, sname);
    //LockAcquire(COM2_W_LOCK);
    //bwprintf(COM2, "ReserveChunks: reserved sensor %s, dist so far %dmm\r\n", sname,
    //d);
    //LockRelease(COM2_W_LOCK);
    sensor = next.destination;
  }
  return 0;
}

void ReleaseAll()
{
  struct msg m;

  m.id = 'f';
  Send(WhoIs("trak"), (char *)&m, sizeof(struct msg), NULL, 0);
}
