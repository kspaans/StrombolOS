#include "switch.h"
#include "ksyscall.h"

void _kPass(struct td *taskd)
{
  taskd->state = READY;
  return;
}
