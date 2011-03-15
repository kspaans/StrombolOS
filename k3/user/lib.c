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
