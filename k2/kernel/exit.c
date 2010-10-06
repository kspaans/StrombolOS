/*
 * Called _kExit so as not to interfere with C's built-in _Exit()
 */
#include "switch.h"
#include "ksyscall.h"

void _kExit(struct td *taskd)
{
  taskd->state = DEFUNCT;
  return;
}
