
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <ANSI.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "../servers/servers.h"

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


void prettyprinttime (int ticks) {
  CURSORPUSH();
  CURSORMOVE(1,200);
  CURSORBACK(25);
  SETCOLOUR(BG+MAGENTA);
  SETCOLOUR(FG+WHITE);
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

void eval (char *cmd, int trid) {
  char packet[5];
  char buf[32];
  SETCOLOUR (FG+BRIGHT+WHITE);
  if (!strcmp("go", cmd)) {
    packet[0] = 'g';
    Send (trid, packet, 1, NULL, 0);
    bwputstr (COM2, "Train set started.\n");
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
  else {
    bwprintf (COM2, "Unknown command.\n");
  }
  SETCOLOUR (FG+WHITE);
}

void wm () {
  int sec;  int done = 0;
  int i=0,t;
  char inbuf[32];
  char ch;
  int n = 0;
  int trid;
  RegisterAs ("wm");
  trid = WhoIs ("tr");
  bwputstr(COM2, "[s[2J[;H[45mStrombolOS v0.0.4 (Turbo Samba)."
                 "[K[m[u");
  bwputstr (COM2, "[?25l");
  CURSORMOVE(2,1);
  SETCOLOUR(BG+BRIGHT+BLACK);
  for (i = 0; i < 14; i++ ){
    CLEARTOEND ();
    bwputc (COM2, '\n');
  }
//  ENABLESCROLL(16);
  
  SETCOLOUR(BG+BLACK);
//  CURSORMOVE(16,1);
  bwputstr(COM2,"+--------------------------------------------------+\n"
  "|    ______                 __        ______  ____ |\n"
  "|   / __/ /________  __ _  / /  ___  / / __ \\/ __/ |\n"
  "|  _\\ \\/ __/ __/ _ \\/  ' \\/ _ \\/ _ \\/ / /_/ /\\ \\   |\n"
  "| /___/\\__/_/  \\___/_/_/_/_.__/\\___/_/\\____/___/   |\n"
  "|                             v0.0.4 (Turbo Samba) |\n"
  "+--------------------------------------------------+\n\n\n> "); 
   while (!done) {
    t = Time()/2; 
  //  prettyprinttime (t);
    if (bwinputready(COM2)) {
     ch = bwgetc(COM2);
     if (ch == CHR_RETURN) { 
       bwputc (COM2,'\n');
       inbuf[n] = 0;

       eval (inbuf, trid);
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
         bwputc (COM2, ch);
         inbuf[n++] = ch;
       }
     }
   }
  
  }
  DPRINTERR ("wm closing wut\n");
}
