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
    //cur = schedule (cur, tasks);
    req = cur->retval;
DPRINTERR ("going.");
    req = activate(cur, req);
DPRINTERR ("back.");
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
