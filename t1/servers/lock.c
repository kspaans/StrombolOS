#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <ANSI.h>
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../user/user.h"
#include "servers.h"
#include "../ktests/tests.h"

struct lock {
  int tid;
  int tidq[16];
  int i;
};

void lock123 () {
  RegisterAs ("lk");
  struct lock db[16];
  int tid;
  int r;
  char msg[2];

  for (r = 0; r < 16; r++) {
    db[r].tid = 0;
    db[r].i   = 0; 
  }

  while (1) {
    r = Receive(&tid, msg, 2);
//    bwprintf (COM2, "msgtype=%c, nm=%d\n",msg[0],(int)msg[1]);
    int lnum = (int)msg[1];
    switch (msg[0]) {
      case 'A': // Acquire
        if (db[lnum].tid == tid ) bwprintf (COM2, "uh, double lock acquire you noob.\n");
        if (db[lnum].tid == 0) { // no one holds this lock, lucky day
          db[lnum].tid = tid;
          Reply (tid, NULL, 0);
        }
        else {
          db[lnum].tidq[(db[lnum].i)++] = tid;
        }
        break;
      case 'R': // Release
        if (db[lnum].i == 0) { // lock queue is now empty
          db[lnum].tid = 0;
        }
        else {
          db[lnum].tid = db[lnum].tidq[--(db[lnum].i)];
          Reply (db[lnum].tid, NULL, 0);
        }
        Reply (tid, NULL, 0);
        break;
      default:
        PANIC;
    }
  }
}
