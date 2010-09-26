#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "regdump.h"

void fuck () {
  bwputstr (COM2, "swi totally worked. going into infinite loop..\n");
  //regdump();
  while (1) {}
}

void bluepill()
{
  bwputstr(COM2, "Welcome to The Matrix...\n");
  regdump();
}

int main () {
  int retval   = 0;
  void (*fp)() = &bluepill;
  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2JHello. in main.\n");
  //regdump();

  bwputstr (COM2, "handler installed! gonna swi this shit nigggaaaaa\n");
 
  retval = swtch(2, fp);
  bwprintf(COM2, "Returned %d from SWI\n", retval);
 
  return 0;
}
