#include <bwio.h>
#include <ts7200.h>
#include "exception.h"
#include "switch.h"
#include "regdump.h"

#define FOREVER for(;;)

void fuck () {
  bwputstr (COM2, "swi totally worked. going into infinite loop..\n");
  //regdump();
  while (1) {}
}

void bluepill()
{
  bwputstr(COM2, "Welcome to The Matrix...\n");
  //regdump();
}

void kinit()
{
  bwputstr(COM2, "< init> entering\n");
  install_handler(handle);
  bwputstr(COM2, "< init> installed exception handler\n");
  bwputstr(COM2, "< init> will initialize some space probably...\n");
  bwputstr(COM2, "< init> leaving\n");
}

int main () {
  int retval   = 0;
  void (*fp)() = &bluepill;
  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");
  //regdump();

  kinit();
 
  retval = swtch(88, fp);
  //regdump();
  bwprintf(COM2, "Returned %d from SWI\n", retval);
  //regdump();
 
  return 0;
}
