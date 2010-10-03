#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "ksyscall.h"
int _kCreate(int priority, void (*code)(), int parenttid)
{
  if (priority < 0 || priority > NUMPRIO) {
    return -1;
  }
  //if (next_td == NULL) {
  //  return -2;
  //}
  bwputstr (COM2, "create () not implemented.");
  return 0;
}
