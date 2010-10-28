#include "../user/lib.h"
#include "../user/usyscall.h"
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>

struct name {
  char name[5];
  int tid;
};

int strcmp(const char *s1, const char *s2)
{
while (*s1==*s2)
{
if(*s1=='\0')
return(0);
s1++;
s2++;
}
return(*s1-*s2);
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

/* returns the index of the found name */
int lookup (char *name, struct name *reg,int num) {
  int i;
  //DPRINTOK("LOOKUP, looking up num %d name %s\r\n", num, reg);
  for (i = 0; i < num; i++) {
    if (!strcmp(name, reg[i].name)) {
      //bwprintf (COM2, "yup\r\n");
      return i;
    }
    else {
      //bwprintf (COM2, "nope\r\n");
    }
  }
  return -1;
}

void nameserv () {
  char msg[6];
  int tid;
  int ans;
  
  struct name reg[100];
  int num = 0;

  while (1) {
    DPRINT("(()) Nameserver begining of loop...\r\n");
    Receive (&tid, msg, 6);
    DPRINT("received from tid %d, message(0x%x) \'%s\'\r\n", tid, msg, msg);
    switch (msg[0]) {
      case 'w':
        ans = lookup ((msg+1), reg, num);
        //DPRINTOK ("Nameserv:lookup gave %d.\r\n", ans);
        Reply(tid, (char*)(&(reg[ans].tid)), 4);
        //DPRINTOK ("Nameserv: Reply sucessfull.\r\n");
        break;
      case 'r':
        //DPRINTOK("NAMES: registering %s to tid %d.\r\n", msg + 1, tid);
        // Has the name been registered before?
        ans = lookup((msg + 1), reg, num);
        if (ans != -1) {
          reg[ans].tid = tid;
          //DPRINTOK("NAMES: overwriting entry %d with tid %d\r\n", ans, tid);
        }
        else {
          strcpy (reg[num].name, msg + 1);
          reg[num++].tid  = tid;
        }
        Reply (tid, 0, 0);
        //DPRINTOK ("Nameserv: Reply sucessfull.\r\n");
        break;
      default: 
        Reply (tid, 0, 0);
        break;
    }
  }
}
