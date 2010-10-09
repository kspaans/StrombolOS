/*
 * P U T  T E S T S  I N  H E R E
 */
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "tests.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "../servers/servers.h"
#include "../kernel/switch.h"

/*
 * Some simple functional tests to try and find edge cases
 * Should return if good, and PANIC otherwise
 */
void srr_tests()
{
  int i;

  DPRINT(">>> Entered\r\n");
  Create(1, nameserv);

  FOREACH(i,1) {
    Create(4, srr0);
    Create(4, srr1);
  }
  //FOREACH(i,1) {
  //  Create(4, srr1);
  //  Create(4, srr0);
  //}

  DPRINT("<<< Exited\r\n");
  Exit();
}

void srr0()
{
  int partner, i;
  char testbuf[1024];

  FOREACH(i, 1024) {
    testbuf[i] = i % 256;
  }

  RegisterAs("srr0");
  partner = WhoIs("srr1");
  DPRINT("%d's partner is %d\r\n", MyTid(), partner);

  i = Send(partner, testbuf, 1024, NULL, 0);
  //DPRINT("%d's return value from SEND is %d\r\n", MyTid(), i);
  DPRINT("return value from SEND is %d\r\n", i);
  if (i != 88) PANIC;

  DPRINT("Going to exit now\r\n");
  Exit();
}

void srr1()
{
  int partner, i, ret;
  char testbuf[1024];

  RegisterAs("srr1");
  Pass();
  partner = WhoIs("srr0");
  DPRINT("%d's partner is %d\r\n", MyTid(), partner);

  ret = Receive(&partner, testbuf, 1024);
  DPRINT("%d received from %d, value %d\r\n", MyTid(), partner, ret);
  FOREACH(i, 1024) {
    if (testbuf[i] != i % 256) PANIC;
  }
  Reply(partner, testbuf, 88);

  Exit();
}

/******************************************************************************/

void returny()
{
  int a,b,c;

  Pass();
  a = 0;
  Pass();
  b = a != 0;
  Pass();
  if (b)
  Pass();
  Pass();
  c = 1;
  Pass();
  Pass();
  Pass();
  Pass();
}
