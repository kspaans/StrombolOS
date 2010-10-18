#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"

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
    /*AwaitEvent(TIMER1_EVENT_ID);*/ // We'll let the notifier handle hardware
                                     // rather than the kernel IRQ handler
    for (r = 0; r < 500000; ++r) {}
    bwputc(COM2, '.');
    /*r = Send(ctid, "n", 1, NULL, 0);
    if (r != 0) {
      DPRINTFUNC("notifier_clock()");
      bwprintf(COM2, "Woah, got a bad return value, %d, when sending to ClockSe"
               "rv\r\n", r);
    }*/
  }
}
