#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL


void trains () {
  int tid;
  char cmd[5];
  RegisterAs ("tr");

  FOREVER {
    Receive (&tid, cmd, 5);
    switch (cmd[0]) {
      case 'g': // go
        Putc (COM1, 0x60);
        break;
      case 's': // stop
        Putc (COM1, 0x61);
        break;
      case 't': // tr 
        Putc (COM1, cmd[1]);
        Putc (COM1, cmd[2]);
        break;
    }
    Reply (tid, NULL, 0);
  }
}
