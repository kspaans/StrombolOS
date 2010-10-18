#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "switch.h"
#include "syscalls/ksyscall.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "tasks.h"
#include "../ktests/tests.h"
#include "boot.h"

typedef unsigned int uint;
void initbuf (int *p, int n, int val) {
  int *i;
  for (i = p; i<p+n; *(i++) = val);
}

void printtrap (struct trapframe *t) {
  bwprintf (COM2, "User trap frame:\n");
  bwprintf (COM2, "\tr0 = %x\n"
                  "\tr1 = %x\n"
                  "\tr2 = %x\n"
                  "\tr3 = %x\n"
                  "\tr4 = %x\n"
                  "\tr5 = %x\n"
                  "\tr6 = %x\n"
                  "\tr7 = %x\n"
                  "\tr8 = %x\n"
                  "\tr9 = %x\n"
                  "\tr10 = %x\n"
                  "\tr11 = %x\n"
                  "\tr12 = %x\n"
                  "\tr13 = %x\n"
                  "\tr14 = %x\n\n",
		t->r0,t->r1,t->r2,t->r3,t->r4,t->r5,t->r6,t->r7,t->r8,t->r9,t->r10,t->r11,t->r12,t->r13,t->r14);
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
  bootstrap (&tds[0], idle_shell, &stacks[0]);

  inittasks (&tasks);
  addtask (&tds[0], &tasks);
  struct td *cur = schedule (&tds[0], &tasks);

  while (cur) {
    bwprintf (COM2, "Going to execute %d\n", cur->tid);
    req = activate(&cur->trap, cur->SPSR, cur->entry);
    req = *((int*)cur->entry-1)&0xFFFF;
    bwprintf (COM2, "Req is: %d\n", req);
    switch (req) {
      case 0:
        newtid = current_tid++;
        req = _kCreate(&tds[newtid], cur->trap.r0, (void *)cur->trap.r1,
                       cur->tid, newtid, stacks[newtid]);
        if (req >= 0) {
          addtask(&(tds[newtid]), &tasks);
        }
        break;
      case 1:
        req = _kMyTid(cur);
	DPRINTERR ("BACK FROM kMYTID\n");
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
        req = _kSend(cur, cur->trap.r0, (char*)cur->trap.r1, cur->trap.r2,
                     (char*)cur->trap.r3, cur->trap.r4, tds, current_tid);
        break;
      case 6:
        req = _kReceive(cur, (int *)cur->trap.r0, (char *)cur->trap.r1,
                        cur->trap.r2, tds);
        break;
      case 7:
        req = _kReply(cur, cur->trap.r0, (char *)cur->trap.r1, cur->trap.r2,
                      tds, current_tid);
        break;
      default:
	DPRINTERR ("UNKNOWN SYSCALL.\n");
	while(1);
    }
    cur->trap.r0 = req;
    cur = schedule (cur, &tasks);
  }

  //c += a;
 
  DPRINT("Goodbye\r\n");
  return 0;
}
