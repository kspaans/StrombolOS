#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL, FOREACH
#include "track.h"


struct msg zeromsg (struct msg*);

int trainid (int trainnum) {
  switch (trainnum) {
    case 12: return 0;
    case 22: return 1;
    case 23: return 2;
    case 32: return 3;
    case 52: return 4;
    default: PANIC; return 0;
  }
}

#define NUM_TRAINS 7
void cali () {
  RegisterAs ("cali");
  struct msg in, out;
  int tid,i; 

  int speeds[NUM_TRAINS][15];
  int atime[NUM_TRAINS][15];
  int dtime[NUM_TRAINS][15];
  int stime[NUM_TRAINS][15]; // in mm

  for (i = 0; i < NUM_TRAINS; i++) {
    speeds[i][0] = 0;
    atime[i][0] = 0;
    dtime[i][0] = 0;
  } 
#include "cali.h"
  while (1) {
    Receive (&tid, (char*)(&in), sizeof(struct msg));
    zeromsg (&out);
    switch (in.id) {
      case 'S': out.d1 = speeds[trainid(in.d1)][in.d2]; break;
      case 'A': out.d1 = atime[trainid(in.d1)][in.d2]; break;
      case 'D': out.d1 = dtime[trainid(in.d1)][in.d2]; break; 
      //case 'T': out.d1 = stoptime[trainid(in.d1)][in.d2]; break;
      default: PANIC;
    }
    Reply (tid, (char*)(&out.d1),sizeof(int)); 
  }
}
