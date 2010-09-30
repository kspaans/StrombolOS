#include "switch.h"
#include "ksyscall.h"

void _Pass(struct td *taskd)
{
  taskd->state = READY;
  return;
}
