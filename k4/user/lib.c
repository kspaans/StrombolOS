#include "usyscall.h"
#include "lib.h"
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

#if 0
#define PARENT(n) ((n - 1) >> 1)
#define LEFT(n)    (n * 2)
#define RIGHT(n)  ((n * 2) +  1)

void swap(struct heap *h, int a, int b)
{
  int *temp = h->data[a];
  h->data[a] = h->data[b];
  h->data[b] = temp;
}

void heap_push(int *val, struct heap *h)
{
  h->data[h->bottom] = val;
  while (h->bottom > 0 && val < h->data[PARENT(h->bottom)]) {
    swap(h, h->bottom, PARENT(h->bottom));
  }
  h->bottom++;
}

void bubble_down(struct heap *h, int index)
{
  int smallest = index;
  int left  = LEFT(index);
  int right = RIGHT(index);
  if (left <= h->bottom && h->data[left] <= h->data[index]) {
    smallest = left;
  }
  if (right <= h->bottom && h->data[right] <= h->data[left]) {
    smallest = right;
  }
  if (smallest != index) {
    swap(h, index, smallest);
    bubble_down(h, smallest);
  }
}

int *heap_pop(struct heap *h)
{
  int *retval = h->data[0];
  h->data[0] = h->data[h->bottom - 1];
  h->bottom--;
  bubble_down(h, 0);
  return retval;
}
#endif
