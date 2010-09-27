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
  bwputstr(COM2, "< bluepill> Welcome to The Matrix...\n");
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
  int a, b, c;
  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");
  //regdump();

  a = 1; b = 2; c = 10;
  kinit();
 
  a *= (c - b);
  bwputstr(COM2, "< kernel> Jump!\n");
  int xx = 42;
  bwprintf (COM2, "xx is %d\n", xx);
  retval = swtch(88, fp); // Exit from the kernel, and return right after
  xx++;
  retval = swtch (88,fp);
  bwprintf (COM2, "xx is %d\n", xx);

  //regdump();
  bwprintf(COM2, "< kernel> Returned %d from SWI\n", retval);
  //regdump();
  c += a;
  bwprintf(COM2, "< kernel> Context arithmetic is %d\n", c);
 
  return 0;
}
