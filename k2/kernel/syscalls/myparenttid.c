#include "../switch.h"
#include "ksyscall.h"

int _kMyParentTid(struct td *taskd)
{
  return taskd->ptid;
}
