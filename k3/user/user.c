#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "clock_client.h"
#include "../servers/servers.h"
#include "../ktests/tests.h"

void ipc1 () {
  bwprintf (COM2, "ipc1: I am %d\r\n", MyTid());
  bwprintf (COM2, "ipc1: Registering as LOL\r\n");
  RegisterAs ("LOL"); RegisterAs ("POO");
  bwprintf (COM2, "ipc1: Registration complete.\r\n");
  bwprintf (COM2, "ipc1: WhoIs(\"LOL\") = %d\r\n",WhoIs("LOL"));
  bwprintf (COM2, "ipc1: Exiting.\r\n");
  bwprintf (COM2, "ipc1: WhoIs(\"POO\") = %d\r\n",WhoIs("POO"));
  Exit();
}

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

void fuckit()
{
  int i;
  int tid = 0xB00B;
  char blah[20];
  bwputstr(COM2, "FUCKIT: Going to recieve\r\n");
  i = Receive(&tid, blah, 20);
  bwprintf(COM2, "FUCKIT:recv'd(%d) %s\r\n", i, blah);
  i = Reply(tid, "EAT MY ASS", 11);
  bwprintf(COM2, "FUCKIT: replyed %d. exiting. \r\n", i);
  Exit();
}
void rps_client();
void first_user_task()
{
  //int tids[4];
  //int i;
  //char msg[50];

  //Pass();
  Create (SYSCALL_LOW, rps_server);
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
 // Pass();
  //Create (SYSCALL_LOW, rps_client);
  //Create(IDLE, second);

/*  tids[0] = Create(USER_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[0]);
  tids[1] = Create(USER_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[1]);
  tids[2] = Create(SYSCALL_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[2]);
  tids[3] = Create(SYSCALL_LOW, other_user_task);
  bwprintf(COM2, "Created: %d.\n", tids[3]);
*/
/*  bwprintf (COM2, "FIRST: creating fuckit\r\n");
  i = Create(USER_LOW, fuckit);
  bwprintf(COM2, "FIRST: Sending to %d\r\n", i);
  Send(i, "SUP NIGGA", 10, msg, 50);
  bwputstr(COM2, "FIRST: no longer send blocked...\r\n");
  bwprintf(COM2, "FIRST: response was %s\r\n", msg);
  bwputstr(COM2, "First: exiting\r\n");
*/  Exit();
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

/*
 * The ``First'' user program.
 * It will spawn necessary processes, and then become the idle task.
 * Will need some hacks to be able to change our priority to IDLE.
 */
void idle_shell()
{
  int i, c;

  /* smslant typeface from http://www.network-science.de/ascii/ */
  bwputstr(COM2, "[2JWelcome to\r\n"
  "+--------------------------------------------------+\r\n"
  "|    ______                 __        ______  ____ |\r\n"
  "|   / __/ /________  __ _  / /  ___  / / __ \\/ __/ |\r\n"
  "|  _\\ \\/ __/ __/ _ \\/  ' \\/ _ \\/ _ \\/ / /_/ /\\ \\   |\r\n"
  "| /___/\\__/_/  \\___/_/_/_/_.__/\\___/_/\\____/___/   |\r\n"
  "|                          v0.0.3 (Techno Fitness) |\r\n"
  "+--------------------------------------------------+\r\n\r\n");

  i = Create (SYSCALL_HIGH, nameserv);
  if (i != 1) PANIC;
  bwputstr(COM2, " Created nameserver\r\n");
  i = Create (SYSCALL_HIGH, clckserv);
  if (i != 2) PANIC;
  bwputstr(COM2, " Created clockserver\r\n");
  /* Other servers... */

  bwputstr(COM2, "Please select an option (1:rps, 2:srr_tests, 3:clock): ");
  while (1) {
    c = Getc(COM2);
    if (c < 0) PANIC;
    bwprintf(COM2, "%c", c);
    switch (c) {
      case '1':
        i = Create(SYSCALL_LOW, &first_user_task);
        goto IDLE;
      case '2':
        i = Create(SYSCALL_LOW, &srr_tests);
        goto IDLE;
      case '3':
        i = Create(SYSCALL_LOW, &clock_fut);
        goto IDLE;
      default:
        bwputstr(COM2, " WUT\r\n");
        break;
    }
  } IDLE:;
  bwputstr(COM2, "\r\n");

  /* Set priority to IDLE now */
  //while (1);
  Exit();
}
