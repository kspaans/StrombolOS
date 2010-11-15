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
  Send (trid, msg, 1, NULL, 0);

  FOREVER {
    cur = Time ();
    if (cur - last > 50) {
       //DPRINTERR ("timeout cur = %d, last = %d??\n",cur,last);
       Delay (1);
       Send (trid, msg, 1, NULL, 0);
       i = 1;
       last = cur;
    }
    else {
      c = Getc_r(COM1);
      if (c != -1) {
        last = cur;
        // DPRINTOK ("bing c=%x i=%d\n",c,i);
        msg[i] = (char)c;
        i++;
        if (i > 10) {
          i = 1;
          Send (trid, msg, 11, NULL, 0);
          Delay (1);
          Send (trid, msg, 1,  NULL, 0);
        }
      }
    }
  }
}

int trains_switch (char id, char dir) {
  if (dir=='S' || dir == 's') {
    Putc(COM1,33);
    Putc(COM1,id);
  }
  else if (dir=='C' || dir == 'c') {
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

struct sensorevent {
  char group;
  int id;
  int time;
};

struct sensorevent fucksensor (int x, int time) {
  struct sensorevent ans;
  ans.time = time;
       if (x    >= 1 && x    <= 16) { ans.group = 'A'; ans.id = x;    }  
  else if (x-16 >= 1 && x-16 <= 16) { ans.group = 'B'; ans.id = x-16; }
  else if (x-32 >= 1 && x-32 <= 16) { ans.group = 'C'; ans.id = x-32; }
  else if (x-48 >= 1 && x-48 <= 16) { ans.group = 'D'; ans.id = x-48; }
  else if (x-64 >= 1 && x-64 <= 16) { ans.group = 'E'; ans.id = x-64; }
//  if (ans.group == 'C' && ans.id == 3) { Putc (COM1, 0x0f); Putc(COM1, 0x34); }
  return ans;
}


void trains () {
  int tid;
  char cmd[15];
  int speeds[100];
  char sw[32];
  //int head = 0;
  int i;

  struct sensorevent latest;
  latest.group = 0;
  for (i = 0; i < 100; i++) { speeds[i] = 0; }

  int r;
  RegisterAs ("tr");
  Create (USER_HIGH, sensorserver);
  FOREVER {
start:
    r = Receive (&tid, cmd, 15);

    // "special" ones
    switch (cmd[0]) {
      case 'v': // st
        Reply (tid, &sw[unfuckswitch(cmd[1])], 1);
        goto start;
        break;
      case 'd':
        Reply (tid, (char*)(&latest), sizeof (struct sensorevent));
        goto start;
        break;
      default:
        break;
    }

    Reply (tid, NULL, 0);
    switch (cmd[0]) {
      case 'p': // poll sensors
        switch (r) {
          case 1:   /*bwprintf (COM2, "pinging\n");*/Putc (COM1, 133); break;
          case 11: 
           for (i = 0; i < 10; i++) {
             char wut = cmd[i+1];
             int j = 1;
             while (wut) {
               if (wut &128) latest = fucksensor (i*8+j,Time());
               wut <<= 1;
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
      case 'w': // sw
        trains_switch(cmd[1],cmd[2]); 
        sw[unfuckswitch(cmd[1])] = cmd[2];  
        break;
      case 'a': // switch all
        for (i = 0; i < 22; i++) {
          trains_switch(fuckswitch(i), cmd[1]);
          sw[i] = cmd[1];
        }
        break;
    }
  }
}
