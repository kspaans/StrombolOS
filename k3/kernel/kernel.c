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
   // bwprintf (COM2, "Registers before user mode: ");
   // print_regs();
   // bwprintf (COM2, "\n");
    bwprintf (COM2, "Going to address %d for user %d\n\n", cur->entry, (int)(cur-&tds[0]));
    req = activate(&cur->trap, cur->SPSR, cur->entry);
    req = *((int*)cur->entry-1)&0xFFFF;
    bwprintf (COM2, "got back req=%d", req);
    //bwprintf (COM2, "Registers after user mode: ");
   // print_regs();
    bwprintf (COM2, "\n");
    switch (req) { // eventually should move into exception.c?
      case 0:
        newtid = current_tid++;
        req = _kCreate(&tds[newtid], cur->trap.r0, (void *)cur->trap.r1,
                       cur->tid, newtid, stacks[newtid]);
	DPRINTERR ("BACK FROM _kCREATE\n");
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
    bwprintf (COM2, "handled?\n");
    cur->trap.r0 = req;
    cur = schedule (cur, &tasks);
  }

  //c += a;
 
  DPRINT("Goodbye\r\n");
  return 0;
}
