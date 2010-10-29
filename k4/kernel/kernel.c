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

struct accounting_data {
  int interrupts;
  int creates;
  int awaitevents;
  int passes;
  int sends;
  int replies;
  int receives;
  int exits;
  int mytids;
  int myparenttids;
  // and so on, including other things??????
};

int main () {
  int req = 1;
  int i, newtid;
  int current_tid = 1;
  struct td tds[MAXTASKS];
  int stacks[MAXTASKS][STACKSIZE];
  struct taskq tasks;
  struct td *eventq[NUMEVENTS];
  struct accounting_data counters;
  uint irqstatus;

  counters.interrupts   = 0;
  counters.creates      = 0;
  counters.awaitevents  = 0;
  counters.passes       = 0;
  counters.sends        = 0;
  counters.replies      = 0;
  counters.receives     = 0;
  counters.exits        = 0;
  counters.mytids       = 0;
  counters.myparenttids = 0;

  FOREACH(i, MAXTASKS) {
    initbuf(stacks[i], STACKSIZE, 0xDEADBEEF);
  }
  FOREACH(i, NUMEVENTS) {
    eventq[i] = NULL;
  }

  bwsetspeed (COM1,  2400);
  bwsetfifo (COM1, OFF);
  bwsetfifo (COM2, OFF);

  DPRINT("[2J");
  DPRINT("< kernel> Hello, world!\r\n");
  bootstrap (&tds[0], idle_shell, &stacks[0]);

  inittasks (&tasks);
  addtask (&tds[0], &tasks);
  struct td *cur = schedule (&tds[0], &tasks);

  while (cur) {
    //bwprintf (COM2, "Going to execute %d:%x:lr%x\n", cur->tid, cur->entry,
    //          cur->trap.r14);
    req = activate(&cur->trap, cur->SPSR, cur->entry);
    if (req) {
      req = 1234;
      //DPRINTOK ("IRQ: ");
      //bwprintf (COM2, "%d\n", *(int*)(VIC1BASE+IRQSTATUS_OFFSET)); 
      //DPRINTOK ("YAY!!!!!! IRQ!!!!!\n"); 
      //bwprintf (COM2, "VIC1IRQSTATUS = %d\n", *(int*)VIC1BASE);
      //DPRINTOK("Int cleared\r\n");
      //bwprintf (COM2, "VIC1IRQSTATUS = %d\n", *(int*)VIC1BASE);
    }
    else {
      req = *((int*)cur->entry-1)&0xFFFF;
    }
    //bwprintf (COM2, "Req is: %d\n", req);
    switch (req) {
      case 1234:
        irqstatus = *(int*)(VIC1BASE+IRQSTATUS_OFFSET);
        if (irqstatus & UART1RXINTR1_MASK) {
          eventq[UART1RX]->state = READY;
          eventq[UART1RX]->trap.r0 = *(char *)(UART1_BASE + UART_DATA_OFFSET);
          eventq[UART1RX] = (void *)0;
          /*
          DPRINTERR ("FUCK A UART INTERRUPT!!!!!\n");
          bwprintf (COM2, "got %x\n", *(char*)(UART1_BASE+UART_DATA_OFFSET));
          */
        }
        else if (irqstatus & TC1OI_MASK) {  // Timer 1 tick
          /* Handle Timer1 AKA event "0", wakeup the waiter and clear the queue */
          *(int*)(TIMER1_BASE+CLR_OFFSET) = 0; // clear interrupt
          eventq[TIMER1]->state = READY;
          eventq[TIMER1] = (void *)0;
        }
        else {
          DPRINTERR ("UNKNOWN INTERRUPT! HOW DID THIS HAPPEN??\n");
          bwprintf (COM2, "\tIT WAS %x\n\tDYING!!!\n", irqstatus);
          while(1);
        }
        ++counters.interrupts;
        goto doneinterrupt;
        break;
      case 0:
        newtid = current_tid++;
        req = _kCreate(&tds[newtid], cur->trap.r0, (void *)cur->trap.r1,
                       cur->tid, newtid, stacks[newtid]);
        if (req >= 0) {
          addtask(&(tds[newtid]), &tasks);
        }
        ++counters.creates;
        break;
      case 1:
        req = _kMyTid(cur);
        ++counters.mytids;
        break;
      case 2:
        req = _kMyParentTid(cur);
        ++counters.myparenttids;
        break;
      case 3:
        _kPass(cur);
        ++counters.passes;
        break;
      case 4:
        _kExit(cur);
        ++counters.exits;
        break;
      case 5:
        req = _kSend(cur, cur->trap.r0, (char*)cur->trap.r1, cur->trap.r2,
                     (char*)cur->trap.r3, cur->trap.r4, tds, current_tid);
      ++counters.sends;
        break;
      case 6:
        req = _kReceive(cur, (int *)cur->trap.r0, (char *)cur->trap.r1,
                        cur->trap.r2, tds);
        ++counters.receives;
        break;
      case 7:
        req = _kReply(cur, cur->trap.r0, (char *)cur->trap.r1, cur->trap.r2,
                      tds, current_tid);
        ++counters.replies;
        break;
      case 8:
        req = _kAwaitEvent(cur, cur->trap.r0, eventq);
        ++counters.awaitevents;
        break;
      default:
        DPRINTERR ("UNKNOWN SYSCALL.\n");
        while(1);
    }
    cur->trap.r0 = req;
doneinterrupt:
    cur = schedule (cur, &tasks);
  }

  DPRINTOK("+----------------------------------+\r\n");
  DPRINTOK("| INTERRUPT AND SYSCALL STATISTICS |\r\n");
  DPRINTOK("|  Interrupts  %d\t                |\r\n", counters.interrupts);
  DPRINTOK("|  Create      %d\t\t                |\r\n", counters.creates);
  DPRINTOK("|  Exit        %d\t\t                |\r\n", counters.exits);
  DPRINTOK("|  Send        %d                |\r\n", counters.sends);
  DPRINTOK("|  Receive     %d                |\r\n", counters.receives);
  DPRINTOK("|  Reply       %d                |\r\n", counters.replies);
  DPRINTOK("|  AwaitEvent  %d                |\r\n", counters.awaitevents);
  DPRINTOK("|  Pass        %d\t\t                |\r\n", counters.passes);
  DPRINTOK("|  MyTid       %d\t\t                |\r\n", counters.mytids);
  DPRINTOK("|  MyParentTid %d\t\t                |\r\n", counters.myparenttids);
  DPRINTOK("+----------------------------------+\r\n");

 
  DPRINT("Goodbye\r\n");
  return 0;
}
