#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL
#include <ANSI.h>

int trains_switch (char id, char dir) {
  if (dir=='S') {
    Putc(COM1,33);
    Putc(COM1,id);
  }
  else if (dir=='C') {
    Putc(COM1, 34);
    Putc(COM1, id);
  }
  else { return 1; }
  Putc (COM1, 32);
  return 0;
}

int fuckswitch (int x) {
  if (x < 18) return x+1;
  else return x+153-18;
}

int unfuckswitch (int x) {
  if (x <19) return x-1;
  else return x-153+18;
}


void trains () {
  int tid;
  char cmd[5];
  int speeds[100];
  char sw[32];
  int i;
  for (i = 0; i < 100; i++) { speeds[i] = 0; }

  RegisterAs ("tr");
  FOREVER {
    Receive (&tid, cmd, 5);

    // "special" ones
    switch (cmd[0]) {
      case 'v': // st
        Reply (tid, &sw[unfuckswitch(cmd[1])], 1);
        break;
      default:
        break;
    }

    Reply (tid, NULL, 0);
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
        speeds[(int)cmd[2]] = cmd[1];
        break;
      case 'r': // reverse
        Putc (COM1, 0);
        Putc (COM1, cmd[1]);
        Delay (100);
        Putc (COM1, 15);
        Putc (COM1, cmd[1]);
        Putc (COM1, speeds[(int)cmd[1]]);
        Putc (COM1, cmd[1]);
        break;
      case 'd': // sw
        trains_switch(cmd[1],cmd[2]); 
        sw[unfuckswitch(cmd[1])] = cmd[2];  
        break;
      case 'a': // switch all
        for (i = 0; i < 32; i++) {
          trains_switch(i, cmd[1]);
          sw[unfuckswitch(i)] = cmd[1];
        }
        break;
    }
  }
}
