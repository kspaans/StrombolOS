#include "switch.h"
#include "ksyscall.h"

int _MyTid(struct td *taskd)
{
  return taskd->tid;
}
