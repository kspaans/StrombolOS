#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "lib.h"
#include "usyscall.h"

int WhoIs (char *name) {
  int rply;
  char msg[2+strlen(name)];
  msg[0] = 'w';
  strcpy (msg+1, name);
  DPRINT("name '%s', sending message '%s'\r\n", name, msg);
  Send (1, msg, 2+strlen(name), (char*)&rply, 4);
  DPRINT("got reply %d\r\n", rply);
  return rply;
}

int RegisterAs (char *name) {
  char msg[2+strlen(name)];
  msg[0] = 'r';
  strcpy (msg+1, name);
  DPRINT("name '%s', sending message '%s'\r\n", name, msg);
  Send (1, msg, strlen(name)+2, 0, 0);
  return 0;
}
