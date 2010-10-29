#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"

/*
 * Notifier task for the clock server -- Awaits the "timer 1" event. When the
 * even happens, it pings the clock server with the message 'n', telling it to
 * "tick".
 */
void notifier_clock()
{
  int r, ctid;

  ctid = WhoIs("clck");
  FOREVER {
    //DPRINTOK("Clock Notifier Awaiting Event!\r\n");
    r = AwaitEvent(TIMER1);
    if (r != 0) {
      DPRINTERR("Could not awaitevent(): %d\r\n", r);
      PANIC;
    }
    //DPRINTOK("gogogo\r\n");
    r = Send(ctid, "n", 1, NULL, 0);
    if (r != 0) {
      DPRINTFUNC("notifier_clock()");
      DPRINTERR("Woah, got a bad return value, %d, when sending to ClockSe"
               "rv\r\n", r);
    }
  }
}
