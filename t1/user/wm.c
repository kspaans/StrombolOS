
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <ANSI.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "../servers/servers.h"
#include "../servers/track.h"

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
}
         
void prettyprinttime (int ticks) {
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
}

void tables(char *sw) {
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
}
void eval (char *cmd, int trid, int track_tid, char *sw, struct sensorevent s,
           struct measurement mz[][80]) {
  char packet[5];
  char buf[32];
  SETCOLOUR (FG+BRIGHT+WHITE);
  if (!strcmp("go", cmd)) {
    packet[0] = 'g';
    Send (trid, packet, 1, NULL, 0);
    bwputstr (COM2, "Train set started.\n");
  }
  else if (!strcmp ("q", cmd)) {
    bwprintf (COM2, "[2J[1;1HQuiting to reboot.\n");
    Shutdown();
  }
  else if (!strcmp ("reboot", cmd)) {
    bwprintf (COM2, "[2J[1;1HRebooting.\n");
    *((uint*)0x80940000) = 0xAAAA;
    while (1); // not super effective
  }
  else if (!strcmp ("help", cmd)) {
    help();
  }
  else if (!strcmp ("stop", cmd)) {
    packet[0] = 's';
    Send (trid, packet, 1, NULL, 0);
    bwputstr (COM2, "Train set stopped.\n");
  }
  else if (!strcmp ("tr", token(cmd,0,buf))) {
    packet[0] = 't';
    packet[1] = (char)stoi(token(cmd,2,buf));
    packet[2] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 3, NULL, 0);
    bwprintf (COM2, "Train %d set to speed %d.\n", stoi(token(cmd,1,buf)), stoi(token(cmd,2,buf)));
  }
  else if (!strcmp ("honk", token(cmd,0,buf))) {
    packet[0] = 't';
    packet[1] = 0x44;
    packet[2] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 3, NULL, 0);
    Delay(4);
    packet[1] = 0x40;
    Send (trid, packet, 3, NULL, 0);
    bwprintf (COM2, "[31mH[32mO[33mN[34mK[35m![37m\n");
  }
  else if (!strcmp ("light", token(cmd,0,buf)) || !strcmp ("lights", token(cmd, 0, buf))) {
    packet[0] = 't';
    if (!strcmp ("on",token (cmd,1,buf))) {
      packet[1] = 0x42;
      bwprintf (COM2, "Lights ");
      SETCOLOUR (BG+YELLOW);
      SETCOLOUR (FG+BRIGHT+BLACK);
      bwprintf (COM2, "ON");
      SETCOLOUR (BG+BLACK);
      SETCOLOUR (FG+WHITE);
      bwprintf (COM2, " for train %d.\n", (char)stoi(token(cmd,2,buf)));
    }
    else {
      packet[1] = 0x40;
      bwprintf (COM2, "Lights ");
      SETCOLOUR (BG+BRIGHT+BLACK);
      SETCOLOUR (FG+BLACK);
      bwprintf (COM2, "OFF");
      SETCOLOUR (BG+BLACK);
      SETCOLOUR (FG+WHITE);
      bwprintf (COM2," for train %d.\n", (char)stoi(token(cmd,2,buf)));
    }
    packet[2] = (char)stoi(token(cmd,2,buf));
    
    Send (trid, packet,3,NULL,0);
  }
  else if (!strcmp ("rv", token(cmd,0,buf))) {
    packet[0] = 'r';
    packet[1] = (char)stoi(token(cmd,1,buf));
    Send (trid, packet, 2, NULL, 0);
    bwprintf (COM2, "Reversing train %d.\n", stoi(token(cmd,1,buf)));
  }
  else if (!strcmp ("sw", token(cmd,0,buf))) {
    packet[0] = 'w';
    packet[1] = (char)(stoi(token(cmd,1,buf))%0xFF);
    packet[2] = token(cmd,2,buf)[0];
    Send (trid, packet, 3, NULL, 0);
    bwprintf (COM2, "Switching switch %d to direction %c.\n", stoi(token(cmd,1,buf)), token(cmd,2,buf)[0]);
    // Notify the TRACKSERVER about the change in state, later we'll also ask it
    // for train state...
    char turnout_update[3];
    turnout_update[0] = 't';
    turnout_update[1] = stoi(token(cmd, 1, buf));
    turnout_update[2] = token(cmd, 2, buf)[0];
    Send(track_tid, turnout_update, 3, NULL, 0);
    sw[unfuckswitch(stoi(token(cmd,1,buf)))] = token(cmd,2,buf)[0];
    tables (sw);
  }
  else if (!strcmp ("swall", token(cmd,0,buf))) {
    packet[0] = 'a';
    packet[1] = token(cmd,1,buf)[0];
    Send (trid, packet, 2, NULL, 0);
    bwprintf (COM2, "Switching all switches to direction %c.\n", token(cmd,1,buf)[0]);
    int ii;
    // Notify the TRACKSERVER about the change in state, later we'll also ask it
    // for train state...
    char turnout_update[3];
    turnout_update[0] = 't';
    turnout_update[2] = token(cmd, 1, buf)[0];
    for (ii = 0; ii < 32; ii++) {
      sw[ii] = token(cmd,1,buf)[0];
      turnout_update[1] = ii;
      Send(track_tid, turnout_update, 3, NULL, 0);
    }
    tables (sw);
  }
  else if (!strcmp ("st", token (cmd,0,buf))) {
    packet[0] = 'v';
    packet[1] = (char)stoi(token(cmd,1,buf));
    char c;
    Send (trid, packet, 2, &c, 1);
    bwprintf (COM2, "Switch %d is in direction %c.\n", stoi(token(cmd,1,buf)), c);
  }
  else if (!strcmp ("wh", token (cmd, 0, buf))) {
    bwprintf (COM2, "Latest sensor: %c%d at time %d:%d.%d\n", s.group,s.id, (s.time/1200), (s.time/20)%60, (s.time/2) %10);
  }
  else if (!strcmp("cal", token(cmd, 0, buf))) {
    int i, j;
    FOREACH(i, 80) {
      FOREACH(j, 80) {
        if (mz[i][j].time != 0) {
          bwprintf(COM2, "Average Time (ticks) [%d][%d]:\t"
                   "%d\r\n", i, j, mz[i][j].time);
        }
      }
    }
  }
  else {
    bwprintf (COM2, "Unknown command.\n");
  }
  SETCOLOUR (FG+WHITE);
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
  int done = 0;
  int i=0, j, t;
  char inbuf[32];
  int ch;
  int n = 0;
  int trid, track_tid;
  char sw[32];
  struct sensorevent data[2];
  struct measurement measurements[80][80];
  for (i = 0; i < 32; i++) sw[i] = '?';
  FOREACH(i, 80) {
    FOREACH(j, 80) {
      measurements[i][j].time = 0;
      measurements[i][j].num  = 0;
    }
  }
  RegisterAs ("wm");


//  while (1) {
//     bwgetc(COM1);
//     bwputc(COM1, 0x61);
 //    bwgetc(COM2);
 //    bwputc(COM1, 0x60);
//     bwputc (COM2, 'f');
//  }
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
//int iii;
/*while (1) {
  iii = Getc_r(COM2);
  if (iii!=-1) 
  bwprintf (COM2, "got %c!!!!!!!!!!!!!!!!!!!!!!\n",iii);
  else bwprintf (COM2,".");
}*/

  ENABLESCROLL(14);
  
  SETCOLOUR(BG+BLACK);
  CURSORMOVE(14,1);
  bwputstr(COM2,"+--------------------------------------------------+\n"
  "|    ______                 __        ______  ____ |\n"
  "|   / __/ /________  __ _  / /  ___  / / __ \\/ __/ |\n"
  "|  _\\ \\/ __/ __/ _ \\/  ' \\/ _ \\/ _ \\/ / /_/ /\\ \\   |\n"
  "| /___/\\__/_/  \\___/_/_/_/_.__/\\___/_/\\____/___/   |\n"
  "|                             v0.0.4 (Turbo Samba) |\n"
  "+--------------------------------------------------+\n\n\n> "); 
   char sensorquery = 'd';
   struct sensorevent sen;
   tables (sw);
   while (!done) {
    t = Time()/2; 
    Send (trid, &sensorquery, 1, (char*)(&sen), sizeof(struct sensorevent));
    prettyprintsensor (sen); 
    examine_sensors(sen, data, measurements);
    prettyprinttime (t);
    ch = Getc_r(COM2);
    if (ch !=-1) {
     if (ch == CHR_RETURN) { 
       bwputc (COM2,'\n');
       inbuf[n] = 0;
       eval (inbuf, trid, track_tid, sw, sen, measurements);
       bwprintf (COM2, "> ");
       n = 0;
     }
     else if (ch == CHR_BACKSPACE) {
       if (n > 0) {
         n--;
         CURSORBACK(1);
         CLEARTOEND();
       }
     }
     else if ( ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ch == ' ') {
       if (n < 32) {
         bwputc (COM2, (char)ch);
         inbuf[n++] = (char)ch;
       }
     }
   }
  
  }
  DPRINTERR ("wm closing wut\n");
}
