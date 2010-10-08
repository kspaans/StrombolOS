#include "../user/usyscall.h"
#include <bwio.h>
#include <ts7200.h>

void rps_server() {
  int q[50];
  int *cur = q;
  int *next = 0;
  int p1, p2;
  char a1, a2;
  char m[2]; m[1] = 0;
  int tid;
  int nump = 0;
  bwprintf (COM2, "IT'S A MOTHERFUCKING RPS SERVER!!!!!!!!!.\r\n");
  RegisterAs ("RPS");
  bwprintf (COM2, "I AM REGISTERED??? WhoIs (\"RPS\") = %d\r\n",WhoIs ("RPS"));
  while (1) {
   Receive (&tid, m, 1);
   bwprintf (COM2, "dicks");
 
    switch (m[0]) {
      case 'J':
        bwprintf (COM2, "RPS_SERVER: %d joined.\r\n", tid);
        *(next++) = tid;
        nump++;
        break;
      case 'R':
      case 'P':
      case 'S':
        if (p1 == 0 || p2 == 0) { m[0] = 'Q'; Reply (tid, m, 1); }
        if (p1 == tid) { bwprintf (COM2, "RPS_SERVER: Player 1 says %c.\r\n", m[0]); a1 = m[0]; }
        else { bwprintf (COM2, "RPS_SERVER: Player 2 says %c.\r\n", m[0]); a2 = m[0]; }
        break;
      case 'Q':
        bwprintf (COM2, "RPS_SERVER: %d quit.", tid);
        if (tid == p1) p1 = 0;
        else  p2 = 0;
        break;
    }
    if (p1 == p2 && p1 == 0 && nump >= 2) {
      bwprintf (COM2, "\r\n RPS_SERVER: Start game: %d vs. %d.\r\n", p1, p2);
      p1 = cur[0];
      p2 = cur[1];
      nump -= 2;
      a1 = a2 = 0;
      Reply (p1, 0, 0);
      Reply (p2, 0, 0);
    } 
    if (a1 != 0 && a2 != 0) {
      if      (a1 == 'R' && a2 == 'R') { m[0] = 'T'; Reply (p1, m, 1); m[0] = 'T'; Reply (p2, m, 1); }
      else if (a1 == 'R' && a2 == 'P') { m[0] = 'L'; Reply (p1, m, 1); m[0] = 'W'; Reply (p2, m, 1); }
      else if (a1 == 'R' && a2 == 'S') { m[0] = 'W'; Reply (p1, m, 1); m[0] = 'L'; Reply (p2, m, 1); }
      else if (a1 == 'P' && a2 == 'R') { m[0] = 'W'; Reply (p1, m, 1); m[0] = 'L'; Reply (p2, m, 1); }
      else if (a1 == 'P' && a2 == 'P') { m[0] = 'T'; Reply (p1, m, 1); m[0] = 'T'; Reply (p2, m, 1); }
      else if (a1 == 'P' && a2 == 'S') { m[0] = 'L'; Reply (p1, m, 1); m[0] = 'W'; Reply (p2, m, 1); }
      else if (a1 == 'S' && a2 == 'R') { m[0] = 'L'; Reply (p1, m, 1); m[0] = 'W'; Reply (p2, m, 1); }
      else if (a1 == 'S' && a2 == 'P') { m[0] = 'W'; Reply (p1, m, 1); m[0] = 'L'; Reply (p2, m, 1); }
      else if (a1 == 'S' && a2 == 'S') { m[0] = 'T'; Reply (p1, m, 1); m[0] = 'T'; Reply (p2, m, 1); }
      a1 = a2 = 0;
    }
  }
}
