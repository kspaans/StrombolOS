#include "../switch.h"
#include "ksyscall.h"

int _kMyTid(struct td *taskd)
{
  return taskd->tid;
}
