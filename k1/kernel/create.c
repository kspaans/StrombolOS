#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "ksyscall.h"
int _kCreate(int priority, void (*code)(), int parenttid)
{
  bwprintf (COM2, "AM CREATE()\n\tFIRST ARG: ");
  bwputr (COM2, priority);
  bwprintf (COM2, ".\n\tSECOND ARG: ");
  bwputr (COM2, (int)code);
  bwprintf (COM2, ".\n\tOK BYE.\n");
  return 0;
  if (priority < 0 || priority > NUMPRIO) {
    return -1;
  }
  //if (next_td == NULL) {
  //  return -2;
  //}
  bwputstr (COM2, "create () not implemented.");
  return 0;
}
