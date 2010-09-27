#include <bwio.h>
#include <ts7200.h>
#include "exception.h"
#include "switch.h"
#include "regdump.h"

#define FOREVER for(;;)
#define NULL (void *)0

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

int activate(struct td *taskd)
{
  bwputstr(COM2, "< activate> upper half...\n");
  bwprintf(COM2, "< activate> with TD %x\n", (int)taskd);
  bwprintf(COM2, "< activate> with TD.tid %x\n", taskd->tid);
  bwprintf(COM2, "< activate> with stack:\n");
  bwprintf(COM2, "< activate>   %x\n", (int)taskd->stack[0]);
  bwprintf(COM2, "< activate>   %x\n", (int)taskd->stack[-1]);
  activate_lower(taskd);
  //change_mode(0x10);
  return 0;
}

void first()
{
  bwputstr(COM2, "Hey, I'm a user!\n");
  swtch(0, NULL);
}

void kinit(struct td *tds, void *s, void (*first)())
{
  bwputstr(COM2, "< init> entering\n");
  install_handler(handle);
  bwputstr(COM2, "< init> installed exception handler\n");
  bwputstr(COM2, "< init> will initialize some space probably...\n");
  tds[0].tid      = 0;
  tds[0].stack    = s;
  tds[0].state    = 0;
  tds[0].priority = 0;
  tds[0].next     = NULL;
  tds[0].retval   = 88;
  tds[0].pc       = first;

  tds[0].stack[0] = (int)tds[0].pc; // setup LR, aka the entry point
  ++(tds[0].stack);
  tds[0].stack[0] = tds[0].retval; // increment MORE>?!
  bwputstr(COM2, "< init> Setup the initial state\n");

  bwprintf(COM2, "< init> Using initial stack: %x\n", (int)s);
  bwputstr(COM2, "< init> leaving\n");
}

void test()
{
  change_mode(0x10);
}

int main () {
  int retval   = 0;
  void (*fp)() = &bluepill;
  int a, b, c;
  struct td tds[10]; // why not?
  int ustack1[1024]; // Probably a bit much?
  int ustack2[1024];

  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");
  //regdump();

  a = 1; b = 2; c = 10;
  kinit(tds, (void *)ustack1, &first);
 
  a *= (c - b);
  bwputstr(COM2, "< kernel> Jump!\n");
  int xx = 42;
  bwprintf (COM2, "xx is %d\n", xx);

  activate(&tds[0]);

  //print_mode();
  //test();
  //print_mode();
  //retval = swtch(88, fp); // Exit from the kernel, and return right after
  //print_mode();
  xx++;
  //retval = swtch (88,fp);
  bwprintf (COM2, "xx is %d\n", xx);

  //regdump();
  bwprintf(COM2, "< kernel> Returned %d from SWI\n", retval);
  //regdump();
  c += a;
  bwprintf(COM2, "< kernel> Context arithmetic is %d\n", c);
 
  return 0;
}
