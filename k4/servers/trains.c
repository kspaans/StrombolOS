#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL
#include <ANSI.h>

void sensorserver () {
  int last = Time ();
  int cur;
  char msg[11]; msg[0] = 'p';
  int i = 1;
  RegisterAs ("sen");
  int trid = WhoIs ("tr"); 
  int c;
  int foo;
  Send (trid, msg, 1, NULL, 0);

  FOREVER {
    cur = Time ();
    if (cur - last > 25) {
      // DPRINTERR ("timeout??\n");
       Delay (25);
       Send (trid, msg, 1, NULL, 0);
       i = 1;
    }
//    DPRINTHUH ("what\n");
    c = Getc_r(COM1);
    if (c != -1) {
      last = cur;
     // DPRINTOK ("bing c=%x i=%d\n",c,i);
      msg[i] = (char)c;
      i++;
      if (i > 10) {
        i = 1;
        Send (trid, msg, 11, NULL, 0);
        Delay (25);
        Send (trid, msg, 1,  NULL, 0);
      }
    }
  }
}

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

void fucksensor (int x, char *s) {
       if (x    >= 1 && x    <= 16) { s[0] = 'A'; s[1] = x;    return; }
  else if (x-16 >= 1 && x-16 <= 16) { s[0] = 'B'; s[1] = x-16; return; }
  else if (x-32 >= 1 && x-32 <= 16) { s[0] = 'C'; s[1] = x-32; return; }
  else if (x-48 >= 1 && x-48 <= 16) { s[0] = 'D'; s[1] = x-48; return; }
  else if (x-64 >= 1 && x-64 <= 16) { s[0] = 'E'; s[1] = x-64; return; }
}

void trains () {
  int tid;
  char cmd[15];
  int speeds[100];
  char sw[32];
  char recentsw [10]; // 5 things
  int head = 0;
  int i;
  for (i = 0; i < 10; i++) { recentsw[i] = 0; }
  for (i = 0; i < 100; i++) { speeds[i] = 0; }
  int r;
  RegisterAs ("tr");
  Create (SYSCALL_LOW, sensorserver);
  FOREVER {
    r = Receive (&tid, cmd, 15);

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
      case 'p': // poll sensors
        switch (r) {
          case 1:   Putc (COM1, 133); break;
          case 11: 
           for (i = 1; i < 11; i++) {
             char sen = cmd[i+1];
             char j = (i%2) ? 1 : 9;
             bwprintf ("got me some data nigz.\n");
             while (sen) {
               if (sen&128) { fucksensor (j+(j/2)*16,&recentsw[head]); bwprintf (COM2, "\n\n%c%d\n\n", recentsw[head],recentsw[head+1]%0xFF);head = (head+1)%5; }
               sen <<= 1;
               j++;
             }
           }
           break;
          default: PANIC;
        }
        break;
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
