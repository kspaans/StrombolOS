#include "lib.h"
#include "usyscall.h"
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>

char crazy (int x ) {
  x%=3;
  switch (x) {
  case 0: return 'R';
  case 1: return 'P';
  case 2: return 'S';
  }
  return '\0';
}

void rps_client () {
  int server = WhoIs ("RPS");
  int me = MyTid ();
  char m[2]; m[0] = 'J'; m[1]=0;
  char r,a;
  Send (server, m, 1, 0, 0);
  m[0] = crazy(me%3);
  Send (server, m, 1, &r, 1);
  while (r != 'Q') {
    m[0] = crazy (me);
    switch (r) {
      case 'W':
        bwprintf (COM2, "(%d) I WON :)\r\n", me); break;
      case 'L': 
        bwprintf (COM2, "(%d) I LOST :(\r\n", me); break;
      case 'T':
        bwprintf (COM2, "(%d) I TIED :|\r\n", me); break;
    }
    a=bwgetc(COM2);
    DPRINTERR("%c\n",a);
    m[0] = 'Q';
    if (me %2 ==0) { bwprintf (COM2, "(%d) I GIVE UP >:(\r\n",me); m[0] = 'Q'; Send (server, m, 1, 0, 0); return; }
    m[0] = 'R';
    Send (server, m, 1, &r, 1);
  }
  bwprintf(COM2, "(%d) OTHER DUDE QUIT :'(\r\n", me); 
  Exit();
}
