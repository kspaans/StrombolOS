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
    DPRINT("--- Parent %d created child %d with priority %d\r\n", MyTid(), r,
           settings[i][0]);
    settings[i][3] = r;
  }
  FOREACH(i, 4) {
    r = Receive(&settings[i][3], NULL, 0);
    DPRINT("--- Parent received from child %d retval %d\r\n", settings[i][3],
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
  DPRINT("--- Parent %d exiting!\r\n", MyTid());
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

  /* get clockserv tid? */
  ptid = MyParentTid();
  DPRINT(".%d.%d.%d.Child %d of parent %d created, asking for info...\r\n", MyTid(),
         MyTid(), MyTid(), MyTid(), ptid);
  r = Send(ptid, NULL, 0, (char *)replybuf, 8);
  /* check return value ... */
  DPRINT(".%d.%d.%d.Child %d delay for %d ticks %d times\r\n", MyTid(),
         MyTid(), MyTid(), MyTid(), replybuf[0], replybuf[1]);
  FOREACH(i, replybuf[1]) {
    DPRINT("TID %d loop %d of %d\r\n", MyTid(), i, replybuf[1]);
    //if (MyTid() == 4) DPRINTOK("Child %d returned from delay of %d\r\n", MyTid(), replybuf[0]);
    r = Delay(replybuf[0]);
    DPRINT("Child %d returned from delay of %d\r\n", MyTid(), replybuf[0]);
    /* lol it should be zero */
  }
  DPRINTOK("================= TRALALALA%d ======================\r\n", MyTid());
  Exit();
}
