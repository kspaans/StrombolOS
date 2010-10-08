#include "lib.h"
#include "usyscall.h"
#include <bwio.h>
#include <ts7200.h>

void rps_client () {
  int server = WhoIs ("RPS");
  int me = MyTid ();
  char m[2]; m[0] = 'J'; m[1]=0;
  char r;
  Send (server, m, 1, 0, 0);
  m[0] = 'R';
  Send (server, m, 1, &r, 1);
  while (r != 'Q') {
    bwprintf (COM2, "cocks");
    switch (r) {
      case 'W':
        bwprintf (COM2, "(%d) I WON :)\n", me); break;
      case 'L': 
        bwprintf (COM2, "(%d) I LOST :(\n", me); break;
      case 'T':
        bwprintf (COM2, "(%d) I TIED :|\n", me); break;
    }
    m[0] = 'Q';
    if (me %2 ==0) { bwprintf (COM2, "(%d) I GIVE UP >:(\n",me); m[0] = 'Q'; Send (server, m, 1, 0, 0); return; }
    m[0] = 'R';
    Send (server, m, 1, &r, 1);
  }
  bwprintf(COM2, "(%d) OTHER DUDE QUIT :'(\n", me); 
  Exit();
}
