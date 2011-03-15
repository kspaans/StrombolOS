/*
 * P U T  T E S T S  I N  H E R E
 */
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "tests.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "../servers/servers.h"
#include "../kernel/switch.h"

/*
 * Some simple functional tests to try and find edge cases
 * Should return if good, and PANIC otherwise
 * Don't forget to synchronize with the children to make sure everything happens
 * in the correct order.
 */
void srr_tests()
{
  int i, tida, tidb, sender_tid1, sender_tid2;

  DPRINT(">>> Entered\r\n");

  FOREACH(i,10) {
    tida = Create(4, srr0);
    tidb = Create(4, srr1);

    if (Receive(&sender_tid1, NULL, 0) != 0) PANIC;
    if (Receive(&sender_tid2, NULL, 0) != 0) PANIC;
    DPRINTOK("Parent %d Received from both children in order: %d then %d\r\n",
             MyTid(), sender_tid1, sender_tid2);
    if (Reply(sender_tid1, NULL, 0)    != 0) PANIC;
    if (Reply(sender_tid2, NULL, 0)    != 0) PANIC;
    DPRINTOK("LOL1\r\n");

    if (Send(tida, NULL, 0, NULL, 0) != 0) {
      DPRINTERR("Synch with child tid %d failed?\r\n", tida);
      PANIC;
    }
    DPRINTOK("LOL2\r\n");
    if (Send(tidb, NULL, 0, NULL, 0) != 0) {
      DPRINTERR("Synch with child tid %d failed?\r\n", tidb);
      PANIC;
    }
    DPRINTOK("srr_tests: loop %d complete\r\n", i);
  }
  FOREACH(i,10) {
    tida = Create(4, srr1);
    tidb = Create(4, srr0);

    if (Receive(&sender_tid1, NULL, 0) != 0) PANIC;
    if (Receive(&sender_tid2, NULL, 0) != 0) PANIC;
    DPRINTOK("Parent2 %d Received from both children in order: %d then %d\r\n",
             MyTid(), sender_tid1, sender_tid2);
    if (Reply(sender_tid1, NULL, 0)    != 0) PANIC; // Wake sender first
    if (Reply(sender_tid2, NULL, 0)    != 0) PANIC;

    if (Send(tidb, NULL, 0, NULL, 0) != 0) {
      DPRINTERR("Synch with child tid %d failed?\r\n", tida);
      PANIC;
    }
    if (Send(tida, NULL, 0, NULL, 0) != 0) {
      DPRINTERR("Synch with child tid %d failed?\r\n", tidb);
      PANIC;
    }
    DPRINTOK("srr_tests: loop %d complete\r\n", i);
  }

  DPRINT("<<< Exited\r\n");
  Exit();
}

void srr0()
{
  int partner, i;
  char testbuf[1024];
  char replyb[1024];

  FOREACH(i, 1024) {
    testbuf[i] = i % 256;
  }

  /*
   * Register, then synch with parent to make sure that our partner has also
   * registered.
   */
  RegisterAs("srr0");
  if (Send(MyParentTid(), NULL, 0, NULL, 0) != 0) PANIC;
  DPRINTOK("wut\r\n");
  partner = WhoIs("srr1");
  DPRINTOK("SRR0: %d's partner is %d\r\n", MyTid(), partner);

  i = Send(partner, testbuf, 1024, replyb, 1024);
  DPRINT("%d's return value from SEND is %d\r\n", MyTid(), i);
  if (i != 88) {
    DPRINTERR("Send returned %d instead of 88?\r\n");
    PANIC;
  }
  FOREACH(i, 88) {
    if (replyb[i] != ((i % 2) ? 8 : 88)) {
      DPRINTERR("Reply value in buf[%d] contains %d instead of %d?\r\n", i,
                replyb[i], (i % 2) ? 8 : 88);
      PANIC;
    }
  }

  if (Receive(&i, NULL, 0) != 0) {
    DPRINTERR("Child %d's receive failed\r\n", MyTid());
    PANIC;
  }
  if (Reply(i, NULL, 0)    != 0) {
    DPRINTERR("Child %d's reply failed\r\n", MyTid());
    PANIC;
  }
  Exit();
}

void srr1()
{
  int partner, i, ret;
  char testbuf[1024];
  char repl[88];

  /*
   * Register, then synch with parent to make sure that our partner has also
   * registered.
   */
  RegisterAs("srr1");
  if (Send(MyParentTid(), NULL, 0, NULL, 0) != 0) PANIC;
  partner = WhoIs("srr0");
  DPRINTOK("SRR1: %d's partner is %d\r\n", MyTid(), partner);

  ret = Receive(&partner, testbuf, 1024); // error with return value
  DPRINTOK("%d received from %d, value %d\r\n", MyTid(), partner, ret);
  if (ret != 1024) PANIC;
  FOREACH(i, 1024) {
    if (testbuf[i] != i % 256) PANIC;
  }

  FOREACH(i, 88) {
    repl[i] = (i % 2) ? 8 : 88;
  }
  ret = Reply(partner, repl, 88);
  DPRINT("Got %d from Reply()\r\n", ret);
  if (ret != 0) PANIC;

  if (Receive(&i, NULL, 0) != 0) {
    DPRINTERR("Child %d's receive failed\r\n", MyTid());
    PANIC;
  }
  if (Reply(i, NULL, 0)    != 0) {
    DPRINTERR("Child %d's reply failed\r\n", MyTid());
    PANIC;
  }
  Exit();
}

/******************************************************************************/

void returny()
{
  int a,b,c;

  Pass();
  a = 0;
  Pass();
  b = a != 0;
  Pass();
  if (b)
  Pass();
  Pass();
  c = 1;
  Pass();
  Pass();
  Pass();
  Pass();
}

/******************************************************************************/

void send_t_client();
/*
 * Test many concurrent sends. This is the parent task. It will spawn roughly N
 * children, who will all send to it the square of their TIDs. This task will
 * receive and reply in a loop until everything is done. Hopefully this will
 * trip some synchronization or scheduler bugs.
 */
#define NUMSCLIENTS 19
void send_tests()
{
  int sender_tid = -88;
  int children[NUMSCLIENTS];
  int i;
  int sent_message_size;
  char buf[100];

  FOREACH(i, NUMSCLIENTS) {
    children[i] = Create(4, &send_t_client);
  }

  FOREACH(i, NUMSCLIENTS * 50) {
    sent_message_size = Receive(&sender_tid, buf, 100);
    if (sent_message_size != 4) {
      DPRINTERR("Check0\r\n");
      PANIC;
    }
    if (*((int *)(buf)) != sender_tid * sender_tid) {
      DPRINTERR("Check1\r\n");
      PANIC;
    }
    Reply(sender_tid, NULL, 0);
  }
  DPRINTOK("Done!\r\n");

  Exit();
}

void send_t_client()
{
  int ptid = MyParentTid();
  int msg = MyTid();
  int i;

  msg *= msg;

  FOREACH(i, 50) {
    Send(ptid, (char *)(&msg), 4, NULL, 0);
  }

  DPRINTOK("Send_Client done!\r\n");
  Exit();
}
