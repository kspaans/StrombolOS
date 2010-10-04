#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "ksyscall.h"
int _kCreate(struct td *newtd, int priority, void (*code)(), int parenttid,
             int newtid, int *stack)
{
  if (priority < 0 || priority > NUMPRIO) {
    return -1;
  }
  if (newtid == MAXTASKS) { // Oh shit, this should be changed to make destroy() easier...
    return -2;
  }

  newtd->tid       = newtid;
  newtd->stack     = stack + STACKSIZE; // We are now pointing just below the stack
  newtd->state    = READY;
  newtd->priority = priority;
  newtd->next     = NULL;
  newtd->retval   = 0;
  newtd->pc       = code;
  newtd->ptid     = parenttid;

  newtd->stack    -= 16;
  newtd->stack[0]  = 16;                         // CPSR
  newtd->stack[1]  = newtd[0].retval;            // register 0
  newtd->stack[14] = (int)(newtd[0].stack + 16); // register 13 (stack register)
  newtd->stack[15] = (int)newtd[0].pc;           // register 14 (link register)

  return newtid;
}
