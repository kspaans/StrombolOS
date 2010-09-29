#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "regdump.h"

#define FOREVER for(;;)
#define NULL (void *)0

void initbuf (int *p, int n, int val) {
  int *i;
  for (i = p; i<p+n; *(i++) = val);
}

void first()
{
  int i = 0xFFFFF;
  int r;
  while (i--) {
    bwprintf(COM2, "Hey, I'm a user(%d)!\n", i);
    r = swtch(i);
    bwprintf(COM2, "And the kernel gave me %d!\n", r);
    for (r = 0; r < 500000; ++r);
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
  /*
  for (i=-1; i<16; i++) {
    bwputr (COM2, tds[0].stack[i]);
    bwputstr (COM2, "\n");
  }
  */
  tds[0].stack[0] = tds[0].retval;
  tds[0].stack[13] = (int)(tds[0].stack + 15);
  tds[0].stack[14] = (int)tds[0].pc;
  bwputstr(COM2, "< init> Setup the initial state\n");

  bwprintf(COM2, "< init> Using initial stack pointer: %x\n", (int)tds[0].stack);
  bwputstr(COM2, "< init> leaving\n");
}

int main () {
  int retval = 1;
  int a, b, c;
  struct td tds[10]; // why not?
  int ustack1[1024]; // Probably a bit much?
  int ustack2[1024];
  initbuf (ustack1,1024, 0xDEADBEEF);
  initbuf (ustack2,1024, 0xDEADBEEF);

  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2J< kernel> Hello, world!\n");

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
  bwputstr(COM2, "< kernel> Jump!\n\n");
  while (retval) {
    retval = activate(&tds[0], (0xF1 + (int)(&retval)) % retval);
    bwprintf(COM2, "< kernel> User called me with %x\n", retval);
  }
  bwprintf(COM2, "< kernel> Returned 0x%x from SWI, finally. It should be 0x0.\n", retval);
  c += a;
  bwprintf(COM2, "< kernel> Context arithmetic is %d\n", c);
 
  return 0;
}
