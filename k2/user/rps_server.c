void rps_server() {
  int q[50];
  int *cur = 0;
  int *next = 0;
  int p1, p2;
  char a1, a2;
  char m;
  int tid;
  int nump = 0;
  while (1) {
    Recieve (&tid, &m, 1);
    switch (m) {
      case 'J':
        *(next++) = tid;
        nump++;
        break;
      case 'R':
      case 'P':
      case 'S':
        if (p1 == 0 || p2 == 0) { m = 'Q'; reply (tid, m, 1); }
        if (p1 == tid) a1 = m;
        else a2 = m;
        break;
      case 'Q':
        if (tid == p1) p1 = 0;
        else  p2 = 0;
        break;
    }
    if (p1 == p2 && p1 == 0 && nump >= 2) {
      p1 = cur[0];
      p2 = cur[1];
      nump -= 2;
      a1 = a2 = 0;
      Reply (p1, 0, 0);
      Reply (p2, 0, 0);
    } 
    if (a1 != 0 && a2 != 0) {
      if      (a1 == 'R' && a2 == 'R') { m = 'T'; Reply (p1, &m, 1); m = 'T'; Reply (p2, &m, 1); }
      else if (a1 == 'R' && a2 == 'P') { m = 'L'; Reply (p1, &m, 1); m = 'W'; Reply (p2, &m, 1); }
      else if (a1 == 'R' && a2 == 'S') { m = 'W'; Reply (p1, &m, 1); m = 'L'; Reply (p2, &m, 1); }
      else if (a1 == 'P' && a2 == 'R') { m = 'W'; Reply (p1, &m, 1); m = 'L'; Reply (p2, &m, 1); }
      else if (a1 == 'P' && a2 == 'P') { m = 'T'; Reply (p1, &m, 1); m = 'T'; Reply (p2, &m, 1); }
      else if (a1 == 'P' && a2 == 'S') { m = 'L'; Reply (p1, &m, 1); m = 'W'; Reply (p2, &m, 1); }
      else if (a1 == 'S' && a2 == 'R') { m = 'L'; Reply (p1, &m, 1); m = 'W'; Reply (p2, &m, 1); }
      else if (a1 == 'S' && a2 == 'P') { m = 'W'; Reply (p1, &m, 1); m = 'L'; Reply (p2, &m, 1); }
      else if (a1 == 'S' && a2 == 'S') { m = 'T'; Reply (p1, &m, 1); m = 'T'; Reply (p2, &m, 1); }
      a1 = a2 = 0;
    }
  }
}
