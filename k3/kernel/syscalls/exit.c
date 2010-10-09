/*
 * Called _kExit so as not to interfere with C's built-in _Exit()
 */
#include "../switch.h"
#include "ksyscall.h"
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>

void _kExit(struct td *taskd)
{
  DPRINT("Exiting: taskd 0x%x, tid %d\r\n", taskd, taskd->tid);
  taskd->state = DEFUNCT;
  return;
}
