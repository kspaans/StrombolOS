#include "switch.h"
#include "ksyscall.h"

int _Create(int priority, void (*code)(), int parenttid)
{
  if (priority < 0 || priority > NUMPRIO) {
    return -1;
  }
  //if (next_td == NULL) {
  //  return -2;
  //}

  return 0;
}
