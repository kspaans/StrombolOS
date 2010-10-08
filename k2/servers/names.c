#include "lib.h"

struct name {
  char name[4];
  int tid;
};

void strcpy (char *d, char *s) {
  while (*d++ = *s++);
}

int strcmp (char *s, char *t) {
  while (s++ == t++);
  return (t--==0&&s--==0) ? 0 : 1;
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

int lookup (char *name, struct name *reg) {
  while (reg++) 
    if (!strcmp(name, reg->name)) return reg->tid;
  return -1;
}

void nameserver () {
  char msg[5];
  int tid;
  int len;
  
  struct name reg[100];
  int ind = 0;
  while (1) {
    if (recieve(&tid, &msg, 5)) {
      switch (msg[0]) {
        case 'w':
          len = itoa (lookup ((msg+1), reg), msg+1);
          reply(&tid, msg+1, len);
          break;
        case 'r':
          strcpy (reg[ind].name, msg + 1);
          reg[ind].tid  = tid;
          reply (&tid, 0, 0);
          break;
        default:
          reply (&tid, 0, 0);
          break;
      }
    }
    else {
      reply (&tid, 0, 0);
    }
  }
}
