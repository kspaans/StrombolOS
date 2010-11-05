#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../switch.h"
#include "ksyscall.h"

/*
 * As much hardware specific stuff as possible will happen inside of the
 * notifiers rather than here.
 * IMPLEMENTATION DEPENDENT:
 * RETURN -2 if there is another waiter on that event
 */
int _kAwaitEvent(struct td *mytd, int event, struct td *eventq[])
{
  DPRINT(">>> Entered with mytd 0x%x, event %d\r\n", mytd, event);

  if (event < 0 || event >= NUMEVENTS) {
    return -1;
  }
  if (eventq[event]) {
    DPRINTERR("TD %x is already waiting on event %d\r\n", eventq[event], event);
    return -2;
  }
  /* other errors? */

  /* Queue up the task for waiting on this event */
  eventq[event] = mytd;
  mytd->state = EVENT_BLOCKED;

  DPRINT("<<< returning\r\n");
  return 0;
}
