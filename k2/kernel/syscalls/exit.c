/*
 * Called _kExit so as not to interfere with C's built-in _Exit()
 */
#include "../switch.h"
#include "ksyscall.h"
#include <bwio.h>
#include <ts7200.h>

void _kExit(struct td *taskd)
{
//  bwprintf (COM2, "TID %d EXITING\r\n", taskd->tid);
  taskd->state = DEFUNCT;
  return;
}
