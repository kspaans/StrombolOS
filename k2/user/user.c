#include <bwio.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"

void second () {
  bwputstr(COM2, "I AM IDLE\n");
  while (1) {}
}

void first()
{
  bwprintf (COM2, "I AM FIRST USER.\n\tMODE IS ");
  print_mode ();
  bwputstr (COM2, ".\n\tCREATE???\n");
  //int z = Create (0xABCDEF01, (void*)0x10FEDCBA);
  int z = Create (3, second);
  int i=0,j=0,k=0;
  while (1) {
    bwprintf (COM2, "I AM FIRST USER.\n\tKERNEL SAID %d\n\tMODE IS ",z);
    print_mode ();
    bwputstr (COM2, ".\n\tPASS??\n");
    i++;
    if (i>10) j++;
    if (j>10) k++;
    i %= 11; j %= 11;
//    bwprintf (COM2, "(i,j,k) = (%d,%d,%d)\n",i,j,k);
    Pass();
    bwprintf (COM2, "I AM FIRST USER.\n\tMODE IS ");
    print_mode ();
    bwputstr (COM2, ".\n\tEXIT????\n");
    Exit();
  }

//  int i = 0xFFFFF;
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
  b[0] = 0xDEADBEEF;
  b[1] = 0xDEADBEEF;
  b[2] = 0xDEADBEEF;
  b[3] = 0xDEADBEEF;
  b[4] = 0xDEADBEEF;
  bwprintf (COM2, "x is %d\n", x);
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
  b[2] -= x;
  b[3]--;
  bwputstr (COM2, "now b[2] = b[2] - x  and b[3] = b[3] - 1...\n");
  i = 5; while (i--) { bwprintf (COM2, "b[%d] = ", i); bwputr(COM2, b[i]); bwputstr (COM2, "\n");}
}

void lol()
{
  int i;
  bwprintf(COM2, "HAHA! I'm a new user task and my tid is '%d'\n", MyTid());
  for(i = 0; i < 50000; ++i);
  Exit();
}

void other_user_task();

void first_user_task()
{
  int tids[4];

  //Create(IDLE, second);

  tids[0] = Create(USER_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[0]);
  tids[1] = Create(USER_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[1]);
  tids[2] = Create(SYSCALL_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[2]);
  tids[3] = Create(SYSCALL_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[3]);

  bwputstr(COM2, "XXX: sending\n");
  Send(0xfacedead, "haha", 5, "wut", 4);
  bwputstr(COM2, "XXX: shouldn't get here...\n");

  bwputstr(COM2, "First: exiting\n");
  Exit();
}

void other_user_task()
{
  bwprintf(COM2, "My TID is %d and my parent's TID is %d.\n",
           MyTid(), MyParentTid());
  Pass();
  bwprintf(COM2, "My TID is %d and my parent's TID is %d.\n",
           MyTid(), MyParentTid());
  Exit();
}
