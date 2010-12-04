
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <ANSI.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "../servers/servers.h"
#include "../servers/track.h"
#include <lock.h>


/*
 __________________
 |__Taskbar________|
 | track  | diag   |
 |        |        | 
 |________|________|
 |                 |
 |                 |
 |                 |
 | > prompt        |  
 -------------------
*/
typedef unsigned int uint;

struct sensorevent {
  char group;
  int id;
  int time;
};

struct measurement {
  int num;
  int time;
};

struct sensorevent fucksensor (int x, int t);

int bwinputready (int channel) {
  int *flags;
  switch (channel) {
  case COM1:
    flags = (int*)(UART1_BASE + UART_FLAG_OFFSET);
    break;
  case COM2:
    flags = (int*)(UART2_BASE + UART_FLAG_OFFSET);
    break;
  default:
    // no
    break;
  }
  return *flags & RXFF_MASK;
}

int min (int a, int b) { return (a>b) ? b : a; }
int max (int a, int b) { return (a>b) ? a : b; }

char *token (char *str, int n,char *buf) {
  int reading = 0, i,j;
  for (i=0; (n||!reading) && str[i] != 0; i++) {
    if (str[i] == ' ' || str[i] == '\t') {
      if (reading) {
        n--;
        reading = 0;
      }
    }
    else reading = 1;
  }
  i--; // went one too far.
  for (j=0; str[i+j] != 0 && str[i+j] != ' ' && str[i+j] != '\t'; j++)
    buf[j] = str[i+j];

  buf[j] = 0;
  return buf;
}

void prettyprintsensor (struct sensorevent s) {
  LockAcquire (COM2_W_LOCK);
  CURSORPUSH ();
  CURSORMOVE (1,40);
  SETCOLOUR (BG+BRIGHT+MAGENTA);
  SETCOLOUR (FG + BRIGHT+ WHITE);
  if (s.group != 0) {
    bwprintf (COM2, "Latest sensor: %c%d at time %d:%d.%d     ", s.group,s.id, (s.time/1200), (s.time/20)%60, (s.time/2) %10);
  }
  SETCOLOUR(BG+BLACK);
  SETCOLOUR (FG+WHITE);
  CURSORPOP ();  
  LockRelease (COM2_W_LOCK);
}
         

void prettyprinttime (int ticks) {
  LockAcquire (COM2_W_LOCK);
  CURSORPUSH();
  CURSORMOVE(1,200);
  CURSORBACK(25);
  SETCOLOUR(BG+BRIGHT+MAGENTA);
  SETCOLOUR(FG+BRIGHT+WHITE);
  DISABLEWRAP();
  char flair;
  switch (((ticks/10)%60)%4) {
    case 0: flair = '-';  break;
    case 1: flair = '\\'; break;
    case 2: flair = '|';  break;
    case 3: flair = '/';  break;
  }
  ENABLEWRAP();
  bwprintf (COM2, "Time: %d:%d.%d %c", (ticks/600),(ticks/10)%60, ticks % 10, flair);
  CLEARTOEND();
  SETCOLOUR(BG+BLACK);
  SETCOLOUR(FG+WHITE);
  CURSORPOP();
  LockRelease (COM2_W_LOCK);
}

/*int strcmp (char *a, char *b) {
  int i = 0;
  for (; a[i]==b[i] && a[i] && b[i]; i++);
  if (a[i] == 0 && b[i] == 0) return 0;
  else                        return i+1;
}
*/
int stoi (char *str) {
  int i, res = 0;
  for (i = 0; str[i] != 0; i++) res = res*10+str[i]-'0';
  return res;
}

void help () { 
  LockAcquire (COM2_W_LOCK);
  bwprintf (COM2, "SYSTEM COMMANDS\n"
                  " help                          Get back to this screen.\n"
                  " q                             Quit to redboot.\n"
                  " reboot                        Reboot.\n\n"

                  "TRAIN COMMANDS\n"
                  " stop                          Emergency stop the train set.\n"
                  " go                            Start train set.\n"
                  " tr <train num> <speed>        Set speed for a train.\n"
                  " rv <train num>                Reverse train.\n"
                  " light on|off <trainum>        Turn the train lights on or off.\n"
                  " honk <train num>              Make the train honk.\n\n"

                  "TRACK COMMANDS\n"
                  " sw <switch num> <switch dir>  Throw switch in direction S(traight)/C(urved)\n"
                  " swall <switch dir>            Switch all switches to direction dir.\n"
                  " wh                            Display last sensor triggered.\n"
                  " st <switch num>               Display the current direction of switch thing something redundant.\n"

                  "\n");
  LockRelease (COM2_W_LOCK);
}




void tables(char *sw) {
  LockAcquire (COM2_W_LOCK);
  CURSORPUSH();
  SETCOLOUR(FG+WHITE);
  SETCOLOUR(BG+MAGENTA);
  int swnum=0,table=0,tablerow=0;

  for (tablerow = 0; tablerow <= 11; tablerow++) {
    CURSORMOVE(2+tablerow,1);
    CLEARTOEND();
  }
  for (swnum = 0; swnum < 22; ) {
    CURSORMOVE(2,table*25+2);
    bwprintf (COM2, "(0lqqqqqqqqwqqqqqqqk");
    CURSORMOVE(3,table*25+2);
    bwprintf (COM2, "x(B SWITCH (0x(B STATE (0x");
    CURSORMOVE(4,table*25+2);
    bwprintf (COM2, "tqqqqqqqqnqqqqqqqu");
    for (tablerow = 0; tablerow <= 7 && swnum < 22; tablerow++) {
      CURSORMOVE(5+tablerow,table*25+2);
      bwprintf (COM2, "x(B %d    ", fuckswitch(swnum));
      if (fuckswitch(swnum) < 10) bwprintf (COM2, "  ");
      else if (fuckswitch(swnum) < 100) bwprintf (COM2, " ");
      bwprintf (COM2, "(0x (B");
      switch (sw[swnum]) {
        case 'S':
          SETCOLOUR (FG+BRIGHT+RED);
          break;
        case 'C':
          SETCOLOUR (FG+BRIGHT+GREEN);
          break;
        case '?':
          SETCOLOUR (FG+BRIGHT+BLACK);
          break;
      }
      bwputc (COM2, sw[swnum]);
      SETCOLOUR (FG+WHITE);
      bwprintf (COM2, "     (0x");
      swnum++;
    }
    CURSORMOVE(5+tablerow,table*25+2);
    bwprintf (COM2, "mqqqqqqqqvqqqqqqqj(B");
    table++;
  }
  CLEARTOEND();
  SETCOLOUR(BG+BLACK);
  CURSORPOP();
  LockRelease (COM2_W_LOCK);
}

void eval (char *cmd, int trid, int track_tid, char *sw, struct sensorevent s,
           struct measurement mz[][80]) {
  char packet[5];
  char buf[32];
  struct msg m;
  LockAcquire (COM2_W_LOCK);
  SETCOLOUR (FG+BRIGHT+WHITE);
  LockRelease (COM2_W_LOCK);
  if (!strcmp("go", cmd)) {
    packet[0] = 'g';
    Send (trid, packet, 1, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwputstr (COM2, "Train set started.\n");
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp("path", token(cmd, 0, buf))) {
    struct path p;
    m.id = 'P';
    m.d1 = stoi(token(cmd, 1, buf));
    m.d2 = stoi(token(cmd, 2, buf));
    Send(track_tid, (char *)&m, sizeof(struct msg), (char *)&p,
         sizeof(struct path));
  }
  else if (!strcmp("neighbourhood", token(cmd, 0, buf))) {
    struct neighbours ne;
    int i;
    m.id = 'N';
    for (i = 0; i < 80; ++i) {
      m.d1 = i;
      Send(track_tid, (char *)&m, sizeof(struct msg), (char *)&ne,
           sizeof(struct neighbours));
      LockAcquire(COM2_W_LOCK);
      bwprintf(COM2, "{Neighbours of %d %d(%d %d %d %d)} ", i, ne.count, ne.n[0],
               ne.n[1], ne.n[2], ne.n[3]);
      LockRelease(COM2_W_LOCK);
    }
    LockAcquire(COM2_W_LOCK);
    bwprintf(COM2, "\r\n");
    LockRelease(COM2_W_LOCK);
  }
  else if (!strcmp ("add", token(cmd,0,buf))) {
//    int i=0;
   // while (legend[i] != 0 && i < MAX_TRAINS) i++;
   // if (i == MAX_TRAINS) bwprintf (COM2, "Too many trains!\n");  
   // else {
    //  legend[i] = stoi(token(cmd,1,buf));
    //  drawlegend(legend, trk, trid, locations);
    //  LockAcquire (COM2_W_LOCK);
      bwprintf (COM2, "Adding train %d.\n",stoi(token (cmd,1,buf)));
   //   LockRelease (COM2_W_LOCK);
      packet[0] = 'A';
      packet[1] = (char)stoi(token(cmd,1,buf));
      Send (trid, packet, 2, NULL, 0);
   // }*/
  }
  else if (!strcmp ("q", cmd)) {
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "[2J[1;1HQuiting to reboot.\n");
    LockRelease (COM2_W_LOCK);
    Shutdown();
  }
  else if (!strcmp ("reboot", cmd)) {
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "[2J[1;1HRebooting.\n");
    LockRelease (COM2_W_LOCK);
    *((uint*)0x80940000) = 0xAAAA;
    while (1); // not super effective
  }
  else if (!strcmp ("help", cmd)) {
    help();
  }
  else if (!strcmp ("stop", cmd)) {
    packet[0] = 's';
    Send (trid, packet, 1, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwputstr (COM2, "Train set stopped.\n");
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("tr", token(cmd,0,buf))) {
    packet[0] = 't';
    packet[1] = (char)stoi(token(cmd,2,buf));
    packet[2] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 3, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Train %d set to speed %d.\n", stoi(token(cmd,1,buf)), stoi(token(cmd,2,buf)));
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("honk", token(cmd,0,buf))) {
    packet[0] = 't';
    packet[1] = 0x44;
    packet[2] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 3, NULL, 0);
    Delay(4);
    packet[1] = 0x40;
    Send (trid, packet, 3, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "[31mH[32mO[33mN[34mK[35m![37m\n");
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("trap", token(cmd,0,buf))) {
    cmd[0] = 'T';
    Send (trid, cmd, 10, NULL, 0);
  }
  else if (!strcmp ("light", token(cmd,0,buf)) || !strcmp ("lights", token(cmd, 0, buf))) {
    packet[0] = 't';
    if (!strcmp ("on",token (cmd,1,buf))) {
      packet[1] = 0x42;
      LockAcquire (COM2_W_LOCK);
      bwprintf (COM2, "Lights ");
      SETCOLOUR (BG+YELLOW);
      SETCOLOUR (FG+BRIGHT+BLACK);
      bwprintf (COM2, "ON");
      SETCOLOUR (BG+BLACK);
      SETCOLOUR (FG+WHITE);
      bwprintf (COM2, " for train %d.\n", (char)stoi(token(cmd,2,buf)));
      LockRelease (COM2_W_LOCK);
    }
    else {
      packet[1] = 0x40;
      LockAcquire (COM2_W_LOCK);
      bwprintf (COM2, "Lights ");
      SETCOLOUR (BG+BRIGHT+BLACK);
      SETCOLOUR (FG+BLACK);
      bwprintf (COM2, "OFF");
      SETCOLOUR (BG+BLACK);
      SETCOLOUR (FG+WHITE);
      bwprintf (COM2," for train %d.\n", (char)stoi(token(cmd,2,buf)));
      LockRelease (COM2_W_LOCK);
    }
    packet[2] = (char)stoi(token(cmd,2,buf));
    
    Send (trid, packet,3,NULL,0);
  }
  else if (!strcmp ("rv", token(cmd,0,buf))) {
    packet[0] = 'r';
    packet[1] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 2, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Reversing train %d.\n", stoi(token(cmd,1,buf)));
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("sw", token(cmd,0,buf))) {
    packet[0] = 'w';
    packet[1] = (char)(stoi(token(cmd,1,buf))%0xFF);
    packet[2] = token(cmd,2,buf)[0];
    Send (trid, packet, 3, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Switching switch %d to direction %c.\n", stoi(token(cmd,1,buf)), token(cmd,2,buf)[0]);
    LockRelease (COM2_W_LOCK);
    // Notify the TRACKSERVER about the change in state, later we'll also ask it
    // for train state...
    m.id = 't';
    m.d1 = stoi(token(cmd, 1, buf)) - 1;
    if (m.d1 > 17) {
      m.d1 -= 134;
    }
    m.c1 = token(cmd, 2, buf)[0];
    Send(track_tid, (char *)&m, sizeof(struct msg), NULL, 0);
    sw[unfuckswitch(stoi(token(cmd,1,buf)))] = token(cmd,2,buf)[0];
    //tables (sw);
  }
  else if (!strcmp ("swall", token(cmd,0,buf))) {
    packet[0] = 'a';
    packet[1] = token(cmd,1,buf)[0];
  /*TODO  if (token(cmd,1,buf)[0] == 's' || token(cmd,1,buf)[0] == 'S') drawswitches(trk,47);
    else drawswitches (trk, 100);
*/    Send (trid, packet, 2, NULL, 0);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Switching all switches to direction %c.\n", token(cmd,1,buf)[0]);
    LockRelease (COM2_W_LOCK);
    // Notify the TRACKSERVER about the change in state, later we'll also ask it
    // for train state...
    int ii;
    m.id = 't';
    m.c1 = token(cmd, 2, buf)[0];
    for (ii = 0; ii < 22; ii++) {
      sw[ii] = token(cmd,1,buf)[0];
      m.d1 = ii;
      Send(track_tid, (char *)&m, sizeof(struct msg), NULL, 0);
    }
    //tables (sw);
  }
  else if (!strcmp ("st", token (cmd,0,buf))) {
    packet[0] = 'v';
    packet[1] = (char)stoi(token(cmd,1,buf));
    char c;
    Send (trid, packet, 2, &c, 1);
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Switch %d is in direction %c.\n", stoi(token(cmd,1,buf)), c);
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("loc", token (cmd,0,buf))) {
    struct sensorevent dispsensor;
    struct msg in;
    packet[0] = 'P';
    packet[1] = (char)stoi(token(cmd,1,buf));
    char c;
    Send (trid, packet,2, (char*)(&in), sizeof (struct msg));
    LockAcquire (COM2_W_LOCK);
    switch (c) {
      case 255:
        bwprintf (COM2, "Train %d is lost!\n", stoi(token(cmd,1,buf))); break;
      case 254:
        bwprintf (COM2, "Train %d has not been added!\n", stoi(token(cmd,1,buf))); break;
      default:
        dispsensor = fucksensor (stoi(token(cmd,1,buf)),0);
        bwprintf (COM2, "Train %d is at sensor %c%d.\n", stoi(token(cmd,1,buf)), dispsensor.group,dispsensor.id); break;
    }
    LockRelease (COM2_W_LOCK);
  }
  else if (!strcmp ("wh", token (cmd, 0, buf))) {
    char c;
    c = 'W';
    Send (trid, &c, 1, NULL, 0);
    Pass();
  }
  else if (!strcmp("cal", token(cmd, 0, buf))) {
    int i, j;
    LockAcquire (COM2_W_LOCK);
    FOREACH(i, 80) {
      FOREACH(j, 80) {
        if (mz[i][j].time != 0) {
          bwprintf(COM2, "Average Time (ticks) [%d][%d]:\t"
                   "%d\r\n", i, j, mz[i][j].time);
        }
      }
    }
    LockRelease (COM2_W_LOCK);
  }
  else {
    LockAcquire (COM2_W_LOCK);
    bwprintf (COM2, "Unknown command.\n");
    LockRelease (COM2_W_LOCK);
  }
  LockAcquire (COM2_W_LOCK);
  SETCOLOUR (FG+WHITE);
  LockRelease (COM2_W_LOCK);
}

#define CURRENT 1
#define LAST    0
void examine_sensors(struct sensorevent s, struct sensorevent data[],
                     struct measurement mz[][80])
{
  int r, last_id, cur_id, time, num;
  char msg[5];
  char *ip;
  int track_tid = WhoIs("trak");
  struct trip next_sens;

  last_id = (data[LAST].group - 'A') * 16 + data[LAST].id;
  cur_id  = (s.group - 'A') * 16 + s.id;
  msg[0] = 'n';
  ip = (char *)&last_id;
  msg[1] = *ip++;
  msg[2] = *ip++;
  msg[3] = *ip++;
  msg[4] = *ip++;

  if (s.time != data[LAST].time && s.id != data[LAST].id && s.group !=
      data[LAST].group && last_id > 0 && cur_id > 0) {
    r = Send(track_tid, msg, 5, (char *)&next_sens, sizeof(struct trip));
    bwprintf(COM2, "Change from sensor %c%d to %c%d (%d--%d) :: predicted next"
        " is: %d(with edges to IDs %d, %d)\r\n",
        data[LAST].group, data[LAST].id, s.group, s.id, last_id, cur_id,
        next_sens.destnode.id, next_sens.destnode.edges[AHEAD],
        next_sens.destnode.edges[BEHIND]);
    time =   mz[last_id][cur_id].time;
    num  = ++mz[last_id][cur_id].num;
    time = ((time * num) + (s.time - data[LAST].time)) / num;
    mz[last_id][cur_id].time = time;
#   if 0
    r = Send(track_tid, msg, 5, (char *)(&dist), 4);
    bwprintf(COM2, ":: Speed between %c%d and %c%d: %d mm/sec (%d / (%d / 20))\r\n",
             data[LAST].group, data[LAST].id, s.group, s.id,
             dist / ((s.time - data[LAST].time) / 20), dist,
             s.time - data[LAST].time);
    //data[CURRENT] = s.time - data[LAST].time;
#   endif /* comment */
  }

  data[LAST] = s;
  return;
}

void wm () {
/*  bwputstr (COM2, "[2J[1;1H");
  int ii,jj;
  for (ii = 0; ii < 15; ii++) {
    bwprintf (COM2, "[%d;1H[41m                                                            ", ii);
  }
  bwprintf (COM2, "[100m[2;2H  [4;8H  [100;100H");
  Shutdown();
*/
  LockAcquire(COM2_R_LOCK);
  LockAcquire(COM2_W_LOCK);
  int done = 0;
  int i=0, j, t;
  char inbuf[32];
  char temp_packet[3];
  int ch;
  int n = 0;
  int trid, track_tid;
  char sw[32];

  struct measurement measurements[80][80];
  for (i = 0; i < 32; i++) sw[i] = '?';
  FOREACH(i, 80) {
    FOREACH(j, 80) {
      measurements[i][j].time = 0;
      measurements[i][j].num  = 0;
    }
  }
  RegisterAs ("wm");

 // RegisterAs ("ui");


  SETCOLOUR (BG+BLACK);
  trid = WhoIs ("tr");
  track_tid = WhoIs("trak");
  CLEAR();
  SETCOLOUR(BG+BRIGHT+MAGENTA);
  SETCOLOUR(FG+BRIGHT+WHITE);
  bwputstr(COM2, "[s[;HStrombolOS v0.0.4 (Turbo Samba)."
                 "[K[m[u");
  bwputstr (COM2, "[?25l");
  CURSORMOVE(2,1);
  ENABLESCROLL(14);
  
  SETCOLOUR(BG+BLACK);
  CURSORMOVE(14,1);
  bwputstr(COM2,"+--------------------------------------------------+\n"
  "|    ______                 __        ______  ____ |\n"
  "|   / __/ /________  __ _  / /  ___  / / __ \\/ __/ |\n"
  "|  _\\ \\/ __/ __/ _ \\/  ' \\/ _ \\/ _ \\/ / /_/ /\\ \\   |\n"
  "| /___/\\__/_/  \\___/_/_/_/_.__/\\___/_/\\____/___/   |\n"
  "|                             v0.0.4 (Turbo Samba) |\n"
  "+--------------------------------------------------+\n\n\n");
  bwputstr(COM2, "Setting switches to curved... ");
  LockRelease (COM2_W_LOCK);
  temp_packet[0] = 'a'; // "swall C"
  temp_packet[1] = 'C';
  Send (trid, temp_packet, 2, NULL, 0);
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "complete.\n");
  LockRelease (COM2_W_LOCK);
  temp_packet[0] = 't'; // tr N 0
  temp_packet[2] = 0;
  temp_packet[1] = 12;
  Send (trid, temp_packet, 3, NULL, 0); // or instead init them when we add them
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "Set train 12 to speed 0.\n");
  LockRelease (COM2_W_LOCK);
  temp_packet[1] = 22;
  Send (trid, temp_packet, 3, NULL, 0);
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "Set train 22 to speed 0.\n");
  LockRelease (COM2_W_LOCK);
  temp_packet[1] = 23;
  Send (trid, temp_packet, 3, NULL, 0);
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "Set train 23 to speed 0.\n");
  LockRelease (COM2_W_LOCK);
  temp_packet[1] = 32;
  Send (trid, temp_packet, 3, NULL, 0);
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "Set train 32 to speed 0.\n");
  LockRelease (COM2_W_LOCK);
  temp_packet[1] = 52;
  Send (trid, temp_packet, 3, NULL, 0);
  LockAcquire (COM2_W_LOCK);
  bwputstr (COM2, "Set train 52 to speed 0.\n");


  bwputstr (COM2, "> ");
//   char sensorquery = 'd';
   struct sensorevent sen;
   //tables (sw);
   LockRelease (COM2_W_LOCK);

   Send (trid, NULL, 0, NULL, 0); // synchro

//   drawtrack(trkB);
   while (!done) {
    t = Time()/2; 
  //  if (t%5==0) drawlegend(legend,trkB,trid,locations); 
  //  Send (trid, &sensorquery, 1, (char*)(&sen), sizeof(struct sensorevent));
  //  prettyprintsensor (sen); 
 //   examine_sensors(sen, data, measurements);
 // prettyprinttime(t);
    ch = Getc_r(COM2);
    if (ch !=-1) {
     if (ch == CHR_RETURN) { 
       LockAcquire (COM2_W_LOCK);
       bwputc (COM2,'\n');
       inbuf[n] = 0;
       LockRelease(COM2_W_LOCK);
       eval (inbuf, trid, track_tid, sw, sen, measurements);
       bwprintf (COM2, "> ");
       n = 0;
       LockRelease (COM2_W_LOCK);
     }
     else if (ch == CHR_BACKSPACE) {
       if (n > 0) {
         n--;
         LockAcquire (COM2_W_LOCK);
         CURSORBACK(1);
         CLEARTOEND();
         LockRelease (COM2_W_LOCK);
       }
     }
     else if ( ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == ' ') {
       if (n < 32) {
         LockAcquire (COM2_W_LOCK);
         bwputc (COM2, (char)ch);
         LockRelease (COM2_W_LOCK);
         inbuf[n++] = (char)ch;
       }
     }
   }
   Pass (); // let other dudes acquire the print lock
  }
  DPRINTERR ("wm closing wut\n");
}
