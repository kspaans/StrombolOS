#include "../user/lib.h"
#include "../user/usyscall.h"
#include <bwio.h>
#include <ts7200.h>
struct name {
  char name[5];
  int tid;
};

int strcmp (char *s, char *t) {
  bwprintf (COM2, "Comparing %s to %s\r\n", s,t);
  while ((*s++ == *t++));
  return (t-1==0&&s-1==0) ? 0 : 1;
}

void atoi (char *c, int *res) {
  while ((int)c++!=0) *res = (*res) * 10   + (((int)c-1)-'0');
}

int itoa (int x, char *res) {
  int i = 0;
  for(i=0; x; i++) { *(res++) = (char)(x%10 +'0'); x /= 10; }
  *res = 0;
  return i;
}

int lookup (char *name, struct name *reg,int num) {
  int i;
  for (i = 0; i < num; i++) {
    if (strcmp(name, reg[i].name)) return reg[i].tid; 
  }
  return -1;
}

void nameserv () {
  char msg[6];
  int tid;
  int ans;
  
  struct name reg[100];
  int ind = 0;
  int num = 0;
  bwprintf (COM2, "IT'S A MOTHERFUCKING NAME SERVER!!!!!!!!\n\r");

  while (1) {
    bwprintf (COM2, "Nameserv: waiting for packet.\r\n");
    Receive (&tid, msg, 6);
    bwprintf (COM2, "Nameserv: recieved, from %d,  packet:  %s\r\n", tid,msg);
    switch (msg[0]) {
      case 'w':
        ans = lookup ((msg+1), reg, num);
	bwprintf (COM2, "Nameserv: Going to reply with %d.\r\n", ans);
        Reply(tid, (char*)&ans, 4);
	bwprintf (COM2, "Nameserv: Reply sucessfull.\r\n");
        break;
      case 'r':
        num ++;
        bwprintf (COM2, "NAMES: registering %s to tid %d.\r\n", msg+1,tid);
        strcpy (reg[ind].name, msg + 1);
        reg[ind].tid  = tid;
	Reply (tid, 0, 0);
	bwprintf (COM2, "Nameserv: Reply sucessfull.\r\n");
        break;
      default: 
        Reply (tid, 0, 0);
        break;
    }
  }
}
