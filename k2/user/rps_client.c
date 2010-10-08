void rps_client () {
  int server = WhoIs ("RPS");
  int me = MyTid ();
  char m = 'J';
  char r;
  Send (server, &m, 1, 0, 0);
  m = 'R';
  Send (server, &m, 1, r, 1);
  while (r != 'Q') {
    switch (r) {
      case 'W':
        bwprintf (COM2, "(%d) I WON :)\n", me); break;
      case 'L': 
        bwprintf (COM2, "(%d) I LOST :(\n", me); break;
      case 'T':
        bwprintf (COM2, "(%d) I TIED :|\n", me); break;
    }
    m = 'Q';
    if (me %2 ==0) { bwprintf (COM2, "(%d) I GIVE UP >:(\n",me); m = 'Q'; Send (server, &m, 1, 0, 0); return; }
    m = 'R';
    send (server, &m, 1, r, 1);
  }
  bwprintf(COM2, "(%d) OTHER DUDE QUIT :'(\n", me); 
  Exit();
}
