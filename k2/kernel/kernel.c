#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "syscalls/ksyscall.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "tasks.h"

void initbuf (int *p, int n, int val) {
  int *i;
  for (i = p; i<p+n; *(i++) = val);
}

void kinit(struct td *tds, int *s, void (*first)())
{
	// replace with kCreate????????????????????????
 // bwputstr(COM2, "< init> entering\n");
  install_handler();
 // bwputstr(COM2, "< init> installed exception handler\n");
 // bwputstr(COM2, "< init> will initialize some space probably...\n");
  tds[0].tid      = 0;
  tds[0].stack    = s + 1024; // We are now pointing just below the stack
  tds[0].state    = READY;
  tds[0].priority = SYSCALL_LOW;
  tds[0].next     = NULL;
  tds[0].retval   = 88;
  tds[0].pc       = first;
  tds[0].ptid     = 0;
  tds[0].mq_next  = 0;
  tds[0].mq_last  = 0;

 // bwprintf(COM2, "< init> Initializing %x through %x of user stack.\n", tds[0].stack - 15, tds[0].stack);

  tds[0].stack -= 16;
  initbuf(tds[0].stack, 16, 0x00FACE00);
  /*
  for (i=-1; i<16; i++) {
    bwputr (COM2, tds[0].stack[i]);
    bwputstr (COM2, "\n");
  }
  */
  tds[0].stack[0] = 16;                        // CPSR
  tds[0].stack[1] = tds[0].retval;             // register 0
  tds[0].stack[14] = (int)(tds[0].stack + 16); // register 13 (stack register)
  tds[0].stack[15] = (int)tds[0].pc;           // register 14 (link register)

/*  bwputstr(COM2, "< init> Setup the initial state\n");

  bwprintf(COM2, "< init> Using initial stack pointer: %x\n", (int)tds[0].stack);
  bwputstr(COM2, "< init> leaving\n");
*/}


int main () {
  int req = 1;
  int i, newtid;
  int current_tid = 1;
  struct td tds[MAXTASKS];
  int stacks[MAXTASKS][STACKSIZE];
  struct taskq tasks;

  FOREACH(i, MAXTASKS) {
    //bwprintf(COM2, "kernel init %d\n", i);
    initbuf(stacks[i], STACKSIZE, 0xDEADBEEF);
  }

  bwsetfifo (COM2, OFF);

  /*
   * Initialize special clock bits
   */
  int *clk, buf;

  clk  = (int *)(TIMER3_BASE + LDR_OFFSET);
  *clk = 0xFFFFFFFF;   // Initialize the counter

  clk  = (int *)(TIMER3_BASE + CRTL_OFFSET);
  buf  = *clk;
  buf &= ~MODE_MASK;   // Set mode to off for free running
  buf &= ~CLKSEL_MASK; // Select 2KHz clock
  buf |= ENABLE_MASK;  // Enable the timer
  *clk = buf;


/*
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

*/
  kinit(tds, stacks[0], &first_user_task);
 
 // a *= (c - b);
 // bwputstr(COM2, "< kernel> Jump!\n\n");
  inittasks (&tasks);
  addtask (&tds[0], &tasks);
  struct td *cur = schedule (&tds[0], &tasks);
  while (cur) {
    //cur = schedule (cur, tasks);
    req = cur->retval;
    //bwputstr (COM2, "\tGOING TO USER NOW.\n");
    req = activate(cur, req);
    //bwprintf (COM2, "AM KERNEL.\n\tMODE IS ");
    //print_mode ();
    //bwprintf (COM2, ".\n\tUSER ASKED %d.\n",req);
     
    switch (req) { // eventually should move into exception.c?
      case 0:
	newtid = current_tid++;
        req = _kCreate(&(tds[newtid]),
	               cur->stack[1],        // PRIORITY 
                       (void *)cur->stack[2],        // CODE
                       cur->tid,
		       newtid,               // NEW TID
		       stacks[newtid]);
	if (req >= 0) {
          addtask(&(tds[newtid]), &tasks);
	}
        break;
      case 1:
        req = _kMyTid(cur);
        break;
      case 2:
        req = _kMyParentTid(cur);
        break;
      case 3:
        _kPass(cur);
        break;
      case 4:
        _kExit(cur);
        break;
      case 5:
//	bwprintf(COM2, "KERNEL: user sent to tid %x\r\n", cur->stack[1]);
	req = _kSend(cur, cur->stack[1], (char*)cur->stack[2], cur->stack[3], (char*)cur->stack[4], cur->stack[5], tds, current_tid);
	break;
      case 6:
//	bwprintf(COM2, "KERNEL: user recieve, *tid %x\r\n", *((int *)(cur->stack[1])));
//	bwprintf(COM2, "KERNEL: user recv w/ char%x\r\n", (int)cur->stack[2]);
//	bwprintf(COM2, "KERNEL: user recv w/ len %x\r\n", cur->stack[3]);
	req = _kReceive(cur, (int *)cur->stack[1], (char *)cur->stack[2], cur->stack[3], tds);
//	bwprintf(COM2, "KERNEL: DON'T GO BACK YOU FUCKFACE.\r\n");
	break;
      case 7:
//	bwprintf(COM2, "KERNEL: user reply, tid %x\r\n", cur->stack[1]);
        req = _kReply(cur, cur->stack[1], (char *)cur->stack[2], cur->stack[3], tds, current_tid);
//	bwprintf(COM2, "KERNEL: done replying\r\n");
        break;
      default:
        req = 0; // ????????????? should probably just kill the proc and print an error?
    }
    cur->retval = req;
    cur = schedule (cur, &tasks);
  }

  //bwprintf(COM2, "< kernel> Returned 0x%x from SWI, finally. It should be 0x0.\n", retval);
  //c += a;
  //bwprintf(COM2, "< kernel> Context arithmetic is %d\n", c);
 
  return 0;
}
