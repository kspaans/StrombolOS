#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"

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
    DPRINTOK("Notifier_Clock is in business!\r\n");
    /*AwaitEvent(TIMER1_EVENT_ID);*/ // We'll let the notifier handle hardware
                                     // rather than the kernel IRQ handler
    r = Send(ctid, "n", 1, NULL, 0);
    if (r != 0) {
      DPRINTFUNC("notifier_clock()");
      bwprintf(COM2, "Woah, got a bad return value, %d, when sending to ClockSe"
               "rv\r\n", r);
    }
  }
}
