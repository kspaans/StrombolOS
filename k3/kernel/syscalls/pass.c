#include "../switch.h"
#include "ksyscall.h"
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>

void _kPass(struct td *taskd)
{
  DPRINT("Passing: taskd 0x%x, tid %d\r\n", taskd, taskd->tid);
  taskd->state = READY; // does this move it to the end of the priority queue?
  return;
}
