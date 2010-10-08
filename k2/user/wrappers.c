#include "lib.h"
#include "usyscall.h"

int WhoIs (char *name) {
  int rply;
  char msg[2+strlen(name)];
  msg[0] = 'w';
  strcpy (msg+1, name);
  Send (1, msg, 2+strlen(name), (char*)&rply, 4);
  return rply;
}

int RegisterAs (char *name) {
  char msg[2+strlen(name)];
  msg[0] = 'r';
  strcpy (msg+1, name);
  Send (1, msg, strlen(name)+2, 0, 0);
  return 0;
}
