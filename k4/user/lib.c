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

/* Set bit number bitnum in the array bits, assume the user doesn't overflow the
 * array. Our board is little-endian, but since we're operating at the byte
 * level, I don't think it matters?
 */
void UseBits(unsigned char *bits, int bitnum)
{
  int offset, subbyte;

  offset = bitnum / 8;
  subbyte = bitnum % 8;

  bits[offset] |= 1 << (7 - subbyte);
}

/* Will kind of suck without zero-padding */
void PrintBits(unsigned char *bits, int len)
{
  int i;

  for(i = 0; i < len; ++i) {
    bwprintf(COM2, "%x-", bits[i]);
  }
  bwputstr(COM2, "\r\n");
}
