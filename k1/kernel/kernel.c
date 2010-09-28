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
  // update TD.stack here...
  return 0;
}

void initdeadbeef (int *p, int n) {
  int *i;
  for (i = p; i<p+n; *(++i) = 0xDEADBEEF);
}

void first()
{
  int i = 4;
  while (i--) {
    bwprintf(COM2, "Hey, I'm a user(%d)!\n", i);
    swtch(0, NULL);
  }
  bwputstr (COM2, "CPU Mode: ");
  print_mode();
  bwputstr (COM2, "\n\n");
  int x = 42;
  int b[5];
  initdeadbeef (b, 5);
  bwprintf (COM2, "x is %d\n", x);
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
  b[2] -= x;
  b[3]--;
  bwputstr (COM2, "now b[2] = b[2] - x  and b[3] = b[3] - 1...\n");
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
}


void kinit(struct td *tds, void *s, void (*first)())
{
  bwputstr(COM2, "< init> entering\n");
  install_handler(handle);
  bwputstr(COM2, "< init> installed exception handler\n");
  bwputstr(COM2, "< init> will initialize some space probably...\n");
  tds[0].tid      = 0xdeadbeef;
  tds[0].stack    = s;
  tds[0].state    = 0;
  tds[0].priority = 0;
  tds[0].next     = NULL;
  tds[0].retval   = 88;
  tds[0].pc       = first;

  tds[0].stack[0] = (int)tds[0].pc; // setup LR, aka the entry point
  tds[0].stack[0] = tds[0].retval; // increment MORE>?!
  ++(tds[0].stack);
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
  initdeadbeef (ustack1,1024);
  initdeadbeef (ustack2,1024);

  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");

  //regdump();
  bwputstr (COM2, "ustack1[1023] is ");
  bwputr (COM2,ustack1[1023]); 
  bwputstr (COM2, "\n");

  a = 1; b = 2; c = 10;
  kinit(tds, (void *)ustack1, &first);
 
  a *= (c - b);
  bwputstr(COM2, "< kernel> Jump!\n");

  activate(&tds[0]);
  bwputstr(COM2, "< kernel> Jump1!\n");
  activate(&tds[0]);
  bwputstr(COM2, "< kernel> Jump2!\n");
  activate(&tds[0]);

  //print_mode();
  //test();
  //print_mode();
  //retval = swtch(88, fp); // Exit from the kernel, and return right after
  //print_mode();
  //retval = swtch (88,fp);

  //regdump();
  bwprintf(COM2, "< kernel> Returned %d from SWI\n", retval);
  //regdump();
  c += a;
  bwprintf(COM2, "< kernel> Context arithmetic is %d\n", c);
 
  return 0;
}
