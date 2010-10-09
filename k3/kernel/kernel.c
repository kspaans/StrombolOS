#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "switch.h"
#include "syscalls/ksyscall.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "tasks.h"
#include "../ktests/tests.h"

void initbuf (int *p, int n, int val) {
  int *i;
  for (i = p; i<p+n; *(i++) = val);
}

void kinit(struct td *tds, int *s, void (*first)())
{
  // replace with kCreate????????????????????????
  DPRINT("entering\r\n");
  install_handler();
  DPRINT("installed exception handler\r\n");
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
  DPRINT("initialized the first TD\r\n");

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

  DPRINT("Setup the initial state\r\n");
  DPRINT("Using initial stack pointer: %x\r\n", (int)tds[0].stack);
  DPRINT("leaving\r\n");
}


int main () {
  int req = 1;
  int i, newtid;
  int current_tid = 1;
  struct td tds[MAXTASKS];
  int stacks[MAXTASKS][STACKSIZE];
  struct taskq tasks;

  FOREACH(i, MAXTASKS) {
    initbuf(stacks[i], STACKSIZE, 0xDEADBEEF);
  }

  bwsetfifo (COM2, OFF);

  DPRINT("[2J");
  DPRINT("< kernel> Hello, world!\r\n");

/*
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
  //kinit(tds, stacks[0], &first_user_task);
  kinit(tds, stacks[0], srr_tests);
  /*  Does not yet work, need to rejigger it  */
  //_kCreate(&tds[0], SYSCALL_LOW, &srr_tests, 0, 0, stacks[0]);
 
 // a *= (c - b);
  inittasks (&tasks);
  addtask (&tds[0], &tasks);
  struct td *cur = schedule (&tds[0], &tasks);
  while (cur) {
    //cur = schedule (cur, tasks);
    req = cur->retval;
    req = activate(cur, req);
     
    switch (req) { // eventually should move into exception.c?
      case 0:
        newtid = current_tid++;
        req = _kCreate(&tds[newtid], cur->stack[1], (void *)cur->stack[2],
                       cur->tid, newtid, stacks[newtid]);
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
        req = _kSend(cur, cur->stack[1], (char*)cur->stack[2], cur->stack[3],
                     (char*)cur->stack[4], cur->stack[6], tds, current_tid);
        break;
      case 6:
        req = _kReceive(cur, (int *)cur->stack[1], (char *)cur->stack[2],
                        cur->stack[3], tds);
        break;
      case 7:
        req = _kReply(cur, cur->stack[1], (char *)cur->stack[2], cur->stack[3],
                      tds, current_tid);
        break;
      default:
        req = 0; // ????????????? should probably just kill the proc and print an error?
    }
    cur->retval = req;
    cur = schedule (cur, &tasks);
  }

  //c += a;
 
  DPRINT("Goodbye\r\n");
  return 0;
}
