#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "usyscall.h"
#include "clock_client.h"
#include "../kernel/switch.h"
#include "../servers/servers.h"

/*
 * OK, so assignment K3 says that this should be the ``first user task'', close
 * enough! This will start the clock server and four client tasks.
 */
void clock_fut()
{
  /* From the assignment spec: {priority, dlay time, delay count, tid} */
  int settings[4][4] = {{1, 10, 20, -1}, {2, 23, 9, -1}, {3, 33, 6, -1},
                        {4, 71, 3, -1}};
  /* For both sending and receiving */
  int buf[2];
  int i, r;

  /* ... start clock server ... */
  FOREACH(i, 4) {
    r = Create(settings[i][0], clock_client);
    int mytid = MyTid();
    DPRINTOK("--- Parent %d created child %d with priority %d\r\n", mytid, r,
           settings[i][0]);
    settings[i][3] = r;
  }
  FOREACH(i, 4) {
    r = Receive(&settings[i][3], NULL, 0);
    DPRINTOK("--- Parent received from child %d retval %d\r\n", settings[i][3],
           r);
    /*  C H E C K   R E T U R N   V A L U E XXX
     * r and settings[i][3]
     * ... client shouldn't be sending anything important
     */
    buf[0] = settings[i][1];
    buf[1] = settings[i][2];
    r = Reply(settings[i][3], (char *)buf, 8);
    /* check return value ... */
  }
  DPRINTOK("--- Parent %d exiting!\r\n", MyTid());
  //Create(USER_LOW, notifier_clock);
  Exit();
}

/*
 * Delay according to our parent's instructions
 */
void clock_client()
{
  int ptid;
  int i, r;
  int replybuf[2];
  int mytid = MyTid();

  /* get clockserv tid? */
  ptid = MyParentTid();
  DPRINTOK(".%d.%d.%d.Child %d of parent %d created, asking for info...\r\n",
           mytid, mytid, mytid, mytid, ptid);
  r = Send(ptid, NULL, 0, (char *)replybuf, 8);
  /* check return value ... */
  DPRINTOK(".%d.%d.%d.Child %d delay for %d ticks %d times\r\n", mytid,
         mytid, mytid, mytid, replybuf[0], replybuf[1]);
  FOREACH(i, replybuf[1]) {
    DPRINTOK("TID %d loop %d of %d\r\n", mytid, i, replybuf[1]);
    r = Delay(replybuf[0]);
    DPRINT("Child %d returned from delay of %d\r\n", mytid, replybuf[0]);
    /* lol it should be zero */
  }
  DPRINTOK("================= %d Exiting =================\r\n", mytid);
  Exit();
}
