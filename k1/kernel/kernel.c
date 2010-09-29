#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "regdump.h"

#define FOREVER for(;;)
#define NULL (void *)0

int activate(struct td *taskd)
{
  int blah;
  bwputstr(COM2, "< activate> upper half...\n");
  bwprintf(COM2, "< activate> with TD.stack pointer %x\n", (int)(&taskd->stack));
  bwprintf(COM2, "< activate> with TD.tid(%x): %x\n", (int)(&taskd->tid), taskd->tid);
  bwprintf(COM2, "< activate> with stack conents:\n");
  bwprintf(COM2, "< activate>  1  %x\n", (int)taskd->stack[1]);
  bwprintf(COM2, "< activate>  0  %x\n", (int)taskd->stack[0]);
  bwprintf(COM2, "< activate> with bottom of stack at %x\n", (int)taskd->stack);
  int i = 0;
  for (i = 14; i >= 0; --i) {
    bwprintf(COM2, "< activate> stack[%d](%x)\t= %x\n", i, (int)(taskd->stack + i), (int)taskd->stack[i]);
  }
  blah = activate_lower(taskd);
  bwprintf(COM2, "TD pointer (%x) and TD.stack (%x)\n", blah, *((int *)blah + 1));
  taskd->stack = (int *)blah;
  return 0;
}

void initbuf (int *p, int n, int val) {
  int *i;
  for (i = p; i<p+n; *(i++) = val);
}

void first()
{
  int i = 4;
  int r;
  while (i--) {
    bwprintf(COM2, "Hey, I'm a user(%d)!\n", i);
    r = swtch(0);
    bwprintf(COM2, "And the kernel gave me %d!\n", r);
  }
  bwputstr (COM2, "CPU Mode: ");
  print_mode();
  bwputstr (COM2, "\n\n");
  int x = 42;
  int b[5];
  initbuf (b, 5, 0xDEADBEEF);
  bwprintf (COM2, "x is %d\n", x);
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
  b[2] -= x;
  b[3]--;
  bwputstr (COM2, "now b[2] = b[2] - x  and b[3] = b[3] - 1...\n");
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
}


void kinit(struct td *tds, int *s, void (*first)())
{
  int i;
  bwputstr(COM2, "< init> entering\n");
  install_handler();
  bwputstr(COM2, "< init> installed exception handler\n");
  bwputstr(COM2, "< init> will initialize some space probably...\n");
  tds[0].tid      = 0xdeadb00b;
  tds[0].stack    = s + 1024; // We are now pointing just below the stack
  tds[0].state    = 0;
  tds[0].priority = 0;
  tds[0].next     = NULL;
  tds[0].retval   = 88;
  tds[0].pc       = first;

  bwprintf(COM2, "< init> Initializing %x through %x of user stack.\n", tds[0].stack - 15, tds[0].stack);

  tds[0].stack -= 15;
  initbuf(tds[0].stack, 15, 0x00FACE00);
  for (i=-1; i<16; i++) {
    bwputr (COM2, tds[0].stack[i]);
    bwputstr (COM2, "\n");
  }
  tds[0].stack[0] = tds[0].retval;
  tds[0].stack[13] = (int)(tds[0].stack + 15);
  tds[0].stack[14] = (int)tds[0].pc;
  bwputstr(COM2, "< init> Setup the initial state\n");

  bwprintf(COM2, "< init> Using initial stack pointer: %x\n", (int)tds[0].stack);
  bwputstr(COM2, "< init> leaving\n");
}

int main () {
  int retval   = 0;
  int a, b, c;
  struct td tds[10]; // why not?
  int ustack1[1024]; // Probably a bit much?
  int ustack2[1024];
  initbuf (ustack1,1024, 0xDEADBEEF);
  initbuf (ustack2,1024, 0xDEADBEEF);

  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");

  //regdump();
  bwputstr (COM2, "ustack1 is ");
  bwputr (COM2,(int)ustack1); 
  bwputstr (COM2, "\n");
  bwprintf(COM2, "ustack1+1023 is %x\n", (int)(ustack1 + 1023));
  bwputstr (COM2, "ustack2 is ");
  bwputr (COM2,(int)ustack2); 
  bwputstr (COM2, "\n");
  bwprintf(COM2, "ustack2+1023 is %x\n", (int)(ustack2 + 1023));
  bwprintf(COM2, "tds is %x\n", (int)tds);

  a = 1; b = 2; c = 10;
  kinit(tds, (void *)ustack1, &first);
 
  a *= (c - b);
  bwputstr(COM2, "< kernel> Jump0! ---- ---- ---- ----\n");
  activate(&tds[0]);
  bwputstr(COM2, "< kernel> Jump1! ---- ---- ---- ----\n");
  activate(&tds[0]);
  bwputstr(COM2, "< kernel> Jump2! ---- ---- ---- ----\n");
  activate(&tds[0]);
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
