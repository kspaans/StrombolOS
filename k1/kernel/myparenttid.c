#include "switch.h"
#include "ksyscall.h"

int _MyParentTid(struct td *taskd)
{
  return taskd->ptid;
}
