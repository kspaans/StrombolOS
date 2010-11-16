#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "clock_client.h"
#include "../servers/servers.h"
#include "../ktests/tests.h"

void idle () {
  while (1);
}

void idle_shell()
{
  int i;

  i = Create (SYSCALL_HIGH, &nameserv); // MUST be tid 2
//  i = Create (USER_HIGH, &wm);          // MUST be tid 3



  i = Create (SYSCALL_HIGH, &clckserv);
  i = Create(SYSCALL_HIGH, &uart1serv);
  i = Create(SYSCALL_HIGH, &uart2serv);
  i = Create (SYSCALL_HIGH, &trains);
 
  i = Create (USER_HIGH, &wm);
  i = Create (IDLE, idle);
  Exit();
}
