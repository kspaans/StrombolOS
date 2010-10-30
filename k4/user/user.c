#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "clock_client.h"
#include "../servers/servers.h"
#include "../ktests/tests.h"

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

  Create (SYSCALL_LOW, rps_server);
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 
  Create (USER_HIGH, rps_client); 

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
  char spinner[4];

  spinner[0] = '|';
  spinner[1] = '/';
  spinner[2] = '-';
  spinner[3] = '\\';

  i = Create (SYSCALL_HIGH, &nameserv);
  bwputstr(COM2, " Created nameserver\r\n");
  i = Create (SYSCALL_HIGH, &clckserv);
  bwputstr(COM2, " Created clockserver\r\n");
  i = Create(SYSCALL_HIGH, &uart1serv);
  bwputstr(COM2, " Created UART1server\r\n");
  /* Other servers... */
  bwputstr(COM2, "Please select an option (1:rps, 2:srr_tests, 3:clock, 4:send"
                 "_tests,\r\n"
                 "  5:TRAIN_CONTROLLER, 6:timings): ");
  while (1) {
    c = bwgetc(COM2);//Getc(COM2);
    bwprintf(COM2, "%c\r\n", c);
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
      case '4':
        i = Create(SYSCALL_LOW, &send_tests);
        goto IDLE;
      case '5':
        if (Create(USER_HIGH, &trains) < 0) PANIC; // What priority to use?
        goto IDLE;
      case '6':
        if (Create(USER_HIGH, &timings) < 0) PANIC;
        goto IDLE;
      default:
        bwputstr(COM2, " WUT\r\n");
        break;
    }
  } IDLE:;

  FOREVER {}
}
