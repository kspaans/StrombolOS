#include "usyscall.h"
#include <bwio.h>
#include <ts7200.h>

int strlen (char *s) {
  int i;
  for (i = 0; s++; i++);
  return i;
}

void strcpy (char *d, char *s) {
  while ((*d++ = *s++));
}

int WhoIs (char *name) {
  int tid;
  char msg[6];
  msg[0] = 'w';
  strcpy((msg+1), name);
  bwprintf (COM2, "WhoIs: gonna send %s.\r\n", msg);
  Send (1, msg, 6, (char*)&tid, 4);
  bwprintf (COM2, "WhoIs: Back.\r\n");
  return tid; // fix
}

int RegisterAs (char *name) {
  char msg[6];
  msg[0] = 'r';
  strcpy ((msg+1), name);
  bwprintf (COM2, "RegisterAs: Gonna send %s.\r\n",msg);
  Send (1, msg, 6, 0, 0);
  bwprintf (COM2, "RegisterAs: Back.\r\n");
  return 0; // fix
}
