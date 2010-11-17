
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <ANSI.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "../servers/servers.h"
#include "../servers/track.h"

#define MAX_TRAINS 6
#define HEIGHT 8
#define WIDTH 24
struct pos {
  int x, y;
};

struct pos sensorlocation (int sen) {
  struct pos p;
  switch (sen) {
    case 0: case 1:   p.x = 4; p.y = 1; return p; // A1
    case 2: case 3:   p.x = 7; p.y = 4; return p;// A3
    case 4: case 5:   p.x = 5; p.y = 8; return p;// A5
    case 6: case 7:   p.x = 4; p.y = 7; return p;// A7
    case 8: case 9:   p.x = 3; p.y = 6; return p;// A9
    case 10: case 11: p.x = 4; p.y = 5; return p; // A11
    case 12: case 13: p.x = 3; p.y = 2; return p; // A13
    case 14: case 15: p.x = 4; p.y = 3; return p; // A15
    case 16: case 17: p.x = 14; p.y = 6; return p;// B1
    case 18: case 19: p.x = 13; p.y = 5; return p;// B3
    case 20: case 21: p.x = 14; p.y = 2; return p;// B5
    case 22: case 23: p.x = 1; p.y = 6; return p;// B7
    case 24: case 25: p.x = 1; p.y = 8; return p;// B9
    case 26: case 27: p.x = 1; p.y = 7; return p; // B11
    case 28: case 29: p.x = 15; p.y = 4; return p;// B13
    case 30: case 31: p.x = 7; p.y = 4; return p;// B15
    case 32: case 33: p.x = 15; p.y = 4; return p;// C1
    case 34: case 35: p.x = 19; p.y = 8; return p;// C3
    case 36: case 37: p.x = 9; p.y = 7; return p;// C5
    case 38: case 39: p.x = 11; p.y = 8; return p;// C7
    case 40: case 41: p.x = 9; p.y = 6; return p;// C9
    case 42: case 43: p.x = 9; p.y = 2; return p; // C11
    case 44: case 45: p.x = 8; p.y = 1; return p; // C13
    case 46: case 47: p.x = 14; p.y = 7; return p;// C15
    case 48: case 49: p.x = 15; p.y = 4; return p;// E1
    case 50: case 51: p.x = 17; p.y = 3; return p;// E3
    case 52: case 53: p.x = 18; p.y = 2; return p;// E5
    case 54: case 55: p.x = 21; p.y = 1; return p; // E7
    case 56: case 57: p.x = 22; p.y = 6; return p;// E9
    case 58: case 59: p.x = 21; p.y = 7; return p;// E11
    case 60: case 61: p.x = 18; p.y = 6; return p;// E13
    case 62: case 63: p.x = 13; p.y = 3; return p;// E15
    case 64: case 65: p.x = 15; p.y = 4; return p;// D1
    case 66: case 67: p.x = 16; p.y = 2; return p;// D3
    case 68: case 69: p.x = 22; p.y = 2; return p;// D5
    case 70: case 71: p.x = 23; p.y = 2; return p; // D7
    case 72: case 73: p.x = 23; p.y = 6; return p;// D9
    case 74: case 75: p.x = 16; p.y = 7; return p;// D11
    case 76: case 77: p.x = 16; p.y = 6; return p;// D13
    case 78: case 79: p.x = 17; p.y = 5; return p;// D15
  }
  return p;
}
struct pos switchlocation (int sw) {
  struct pos p;
  switch (sw) {
    case 0: p.x = 4; p.y = 6; return p; // SW1
    case 1: p.x = 5; p.y = 7; return p;// SW2
    case 2: p.x = 6; p.y = 8; return p;// SW3
    case 3: p.x = 4; p.y = 2; return p; // SW4
    case 4: p.x = 17; p.y = 8; return p;// SW5
    case 5: p.x = 12; p.y = 7; return p;// SW6
    case 6: p.x = 18; p.y = 7; return p;// SW7
    case 7: p.x = 23; p.y = 5; return p;// SW8
    case 8: p.x = 23; p.y = 3; return p;// SW9
    case 9: p.x = 17; p.y = 2; return p;// SW10
    case 10:p.x = 7; p.y = 1; return p; // SW11
    case 11:p.x = 5; p.y = 1; return p; // SW12
    case 12:p.x = 13; p.y = 2; return p;// SW13
    case 13:p.x = 7; p.y = 3; return p; // SW14
    case 14:p.x = 7; p.y = 5; return p;// SW15
    case 15:p.x = 13; p.y = 6; return p;// SW16
    case 16:p.x = 17; p.y = 6; return p;// SW17
    case 17:p.x = 13; p.y = 8; return p;// SW18
    case 18:p.x = 14; p.y = 5; return p;// SW99
    case 19:p.x = 16; p.y = 5; return p;// SW9A
    case 20:p.x = 16; p.y = 3; return p;// SW9B
    case 21:p.x = 14; p.y = 3; return p;// SW9C
  }
  return p;
}

char charatlocation (struct pos p, char *trk) {
  return trk[(p.x-1)+(p.y-1)*WIDTH];
}

void putpixel (struct pos p, int colour, char *trk) {
  CURSORPUSH();
  bwprintf (COM2, "[%d;%dH[%dm%c[m", p.y+2, p.x+2, colour, charatlocation(p,trk));
  CURSORPOP();
}

void drawsensors (char *trk) {
  int i;
  for (i=0; i<80; i++) {
    putpixel (sensorlocation(i), 44, trk);
  }
}
void drawswitches (char *trk, int c) {
  int i;
  for (i =0; i<22; i++) {
    putpixel (switchlocation(i),c,trk);
  }
}

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
  CURSORPUSH ();
  CURSORMOVE (1,40);
  SETCOLOUR (BG+BRIGHT+MAGENTA);
  SETCOLOUR (FG + BRIGHT+ WHITE);
  if (s.group != 0) {
    // XXX queef
    //bwprintf (COM2, "Latest sensor: %c%d at time %d:%d.%d     ", s.group,s.id, (s.time/1200), (s.time/20)%60, (s.time/2) %10);
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
  // XXX queef
  //bwprintf (COM2, "Time: %d:%d.%d %c", (ticks/600),(ticks/10)%60, ticks % 10, flair);
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



void drawtrack (char *trk) { 
  CURSORPUSH();
  SETCOLOUR(BG+BRIGHT+BLACK);
  SETCOLOUR(FG+BRIGHT+WHITE);
  CURSORMOVE(2,1);
  int x,y; 
  for (y = 0; y < HEIGHT; y++) { 
    bwprintf (COM2," [K[%d;1H  ", y+3); 
    for (x = 0; x < WIDTH; x++) { 
      bwprintf (COM2,"%c", trk[x+y*WIDTH]); 
    } 
  }
  bwprintf (COM2, "[K\n");
  CLEARTOEND();
  SETCOLOUR(BG+BLACK);
  SETCOLOUR(FG+WHITE);
  CURSORPOP();
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
void drawlegend (int *legend, char *trk, int trid) {
  char packet[2];
  packet[0] = 'P';
  CURSORPUSH();
  CURSORMOVE(3,30);
  SETCOLOUR(BG+BRIGHT+BLACK);
  bwprintf (COM2, "Train ID\t\tPosition");
  int i;
  struct msg in;
  for (i = 0; legend[i] && i < MAX_TRAINS; i++) {
    //get train pos
    packet[1] = (char)legend[i];
    Send (trid, packet,2, (char*)(&in), sizeof (struct msg));
    CURSORMOVE(4+i,30); 
    SETCOLOUR(41+i);
    bwprintf (COM2, "  ");
    SETCOLOUR(BG+BRIGHT+BLACK);
    struct sensorevent s;// = fucksensor((int)c,0);
    switch ((char)in.d1) {
      case 255:
      case 254:
        bwprintf (COM2, "  Train %d\t(lost!)", legend[i]); 
        break; 
       // bwprintf (COM2, "  I AM ERROR."); 
        break;
      default:
        s = fucksensor(in.d1,0);
        bwprintf (COM2, "  Train %d\t(%c%d+%d.%d)", legend[i],s.group, s.id, in.d2/10, in.d2%10);
        break;
    }
  }
  SETCOLOUR (BG+BLACK);
  CURSORPOP();
}
void eval (char *cmd, int trid, int track_tid, char *sw, struct sensorevent s,
           struct measurement mz[][80], char *trk, int *legend) {
  char packet[5];
  char buf[32];
  struct msg m;
  SETCOLOUR (FG+BRIGHT+WHITE);
  if (!strcmp("go", cmd)) {
    packet[0] = 'g';
    Send (trid, packet, 1, NULL, 0);
    bwputstr (COM2, "Train set started.\n");
  }
  else if (!strcmp ("add", token(cmd,0,buf))) {
    int i=0;
    while (legend[i] != 0 && i < MAX_TRAINS) i++;
    if (i == MAX_TRAINS) bwprintf (COM2, "Too many trains!\n");  
    else {
      legend[i] = stoi(token(cmd,1,buf));
      drawlegend(legend, trk, trid);
      bwprintf (COM2, "Adding train %d (%d).\n",stoi(token (cmd,1,buf)),i);
      packet[0] = 'A';
      packet[1] = (char)stoi(token(cmd,1,buf));
      Send (trid, packet, 2, NULL, 0);
    }
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
  else if (!strcmp ("trap", token(cmd,0,buf))) {
    cmd[0] = 'T';
    Send (trid, cmd, 10, NULL, 0);
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
    if (token(cmd,2,buf)[0] == 's' || token(cmd,2,buf)[0] =='S') putpixel (switchlocation(unfuckswitch(stoi(token(cmd,1,buf)))), 47, trk);
    else putpixel (switchlocation(unfuckswitch(stoi(token(cmd,1,buf)))), 100, trk);
    Send (trid, packet, 3, NULL, 0);
    bwprintf (COM2, "Switching switch %d to direction %c.\n", stoi(token(cmd,1,buf)), token(cmd,2,buf)[0]);
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
    if (token(cmd,1,buf)[0] == 's' || token(cmd,1,buf)[0] == 'S') drawswitches(trk,47);
    else drawswitches (trk, 100);
    Send (trid, packet, 2, NULL, 0);
    bwprintf (COM2, "Switching all switches to direction %c.\n", token(cmd,1,buf)[0]);
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
    bwprintf (COM2, "Switch %d is in direction %c.\n", stoi(token(cmd,1,buf)), c);
  }
  else if (!strcmp ("loc", token (cmd,0,buf))) {
    struct sensorevent dispsensor;
    struct msg in;
    packet[0] = 'P';
    packet[1] = (char)stoi(token(cmd,1,buf));
    char c;
    Send (trid, packet,2, (char*)(&in), sizeof (struct msg));
    switch (c) {
      case 255:
        bwprintf (COM2, "Train %d is lost!\n", stoi(token(cmd,1,buf))); break;
      case 254:
        bwprintf (COM2, "Train %d has not been added!\n", stoi(token(cmd,1,buf))); break;
      default:
        dispsensor = fucksensor (stoi(token(cmd,1,buf)),0);
        bwprintf (COM2, "Train %d is at sensor %c%d.\n", stoi(token(cmd,1,buf)), dispsensor.group,dispsensor.id); break;
    }
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
/*void wm () {
  RegisterAs ("wm");
  Pass(); // let ui open.
  int tid_ui = WhoIs ("ui");
  
}*/

void wm () {
/*  bwputstr (COM2, "[2J[1;1H");
  int ii,jj;
  for (ii = 0; ii < 15; ii++) {
    bwprintf (COM2, "[%d;1H[41m                                                            ", ii);
  }
  bwprintf (COM2, "[100m[2;2H  [4;8H  [100;100H");
  Shutdown();
*/
  int done = 0;
  int i=0, j, t;
  char inbuf[32];
  int ch;
  int n = 0;
  int trid, track_tid;
  char sw[32];
  int legend[MAX_TRAINS];
  for (i=0; i<MAX_TRAINS; i++) {
    legend[i] = 0;
  }

  char *trkB = "---------------------,  "
                "---/  |,-------------,\\ "
                "   |  /      \\|/      \\|"
                "   |  |       |        |"
                "   |  \\      /|\\      /|"
                "---\\  |`-------------\'/ "
                "----\\  `-------------\'  "
                "------------\\---/-------";



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
   //tables (sw);
   drawtrack(trkB);
   while (!done) {
    t = Time()/2; 
    if (t%5==0) drawlegend(legend,trkB,trid); 
    Send (trid, &sensorquery, 1, (char*)(&sen), sizeof(struct sensorevent));
    //prettyprintsensor (sen); 
    //examine_sensors(sen, data, measurements);
    //prettyprinttime (t);
    ch = Getc_r(COM2);
    if (ch !=-1) {
     if (ch == CHR_RETURN) { 
       bwputc (COM2,'\n');
       inbuf[n] = 0;
       eval (inbuf, trid, track_tid, sw, sen, measurements, trkB,legend);
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
