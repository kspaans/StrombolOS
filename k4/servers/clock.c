#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL

struct delay {
  int time;
  int tid;
};

struct heap {
  int size;
  int bottom;
  struct delay **data;
};

#define PARENT(n) ((n - 1) >> 1)
#define LEFT(n)    (n * 2)
#define RIGHT(n)  ((n * 2) +  1)

void swap(struct heap *h, int a, int b)
{
  struct delay *temp = h->data[a];
  h->data[a] = h->data[b];
  h->data[b] = temp;
}

void heap_push(struct delay *val, struct heap *h)
{
  h->data[h->bottom] = val;
  while (h->bottom > 0 && val->time < h->data[PARENT(h->bottom)]->time) {
    swap(h, h->bottom, PARENT(h->bottom));
  }
  h->bottom++;
}

void bubble_down(struct heap *h, int index)
{
  int smallest = index;
  int left  = LEFT(index);
  int right = RIGHT(index);
  if (left <= h->bottom && h->data[left]->time <= h->data[index]->time) {
    smallest = left;
  }
  if (right <= h->bottom && h->data[right]->time <= h->data[left]->time) {
    smallest = right;
  }
  if (smallest != index) {
    swap(h, index, smallest);
    bubble_down(h, smallest);
  }
}

struct delay *heap_pop(struct heap *h)
{
  if (h->bottom == 0) return NULL;
  struct delay *retval = h->data[0];
  h->data[0] = h->data[h->bottom - 1];
  h->bottom--;
  bubble_down(h, 0);
  return retval;
}

#define BUFSIZE 5

/*
 * CLOCK SERVER PROTOCOL
 * Delay()  "d####" | the final four bytes make up number of ticks to delay
 * Time()   "t"     | reply with the number of ticks since startup
 * Notifier "n"     | QUICKLY! ++ticks and reply
 *
 * For Delay(), a task is suspended by not replying to it immediately. It is
 * insertion sorted into a list of Delayed tasked, sorted by their wakeup time
 * (which is easy to calculate with the time given to Delay()). Whenever it
 * hears from the Notifier(), the Clock Server wakes up all tasks accordingly
 * from the front of the list. List nodes are allocated from a pool of size
 * MAXTASKS. It is a circular buffer of structures, so by the time we loop back
 * around to the front, those structures should be "free". XXX This will be a
 * problem if we have one long-waiting task, and lots of short waiting tasks, we
 * will eventually overwrite the original long-delayed task...
 */
void clckserv()
{
  int ticks = 0;
  int r, tid = -1, delayindex = 0;
  char buf[BUFSIZE];
  struct delay delaypool[MAXTASKS];
  struct delay *delayppool[MAXTASKS];
  struct delay *temp;
  struct heap delay_heap;

  delay_heap.bottom = 0;
  delay_heap.size   = MAXTASKS;
  delay_heap.data   = delayppool;

  RegisterAs("clck");
  if (Create(INTERRUPT, &notifier_clock) < 0)   PANIC;

  FOREVER {
    //DPRINTOK("Go clock server!\r\n");
    r = Receive(&tid, buf, BUFSIZE);
    // occasionally this buf has junk in it? Try to keep it terminated?
    DPRINT("received from tid %d, return val %d, mesg: \'%c%c%c%c%c\r\r\n",
           tid, r, buf[0], buf[1], buf[2], buf[3], buf[4]); // truncated mesg,
          // plus return val of 5 when it should be 1?
    if (r < 0 || r > BUFSIZE) PANIC;
    switch (buf[0]) {
      case 'n':
        r = Reply(tid, NULL, 0);
        if (r != 0) { bwputstr(COM2, "haha\r\n");PANIC; }
        ++ticks;
        while (delay_heap.bottom > 0 && ticks > delay_heap.data[0]->time) {
          temp = heap_pop(&delay_heap);
          r = Reply(temp->tid, NULL, 0);
          if (r != 0) {
            PANIC;
          }
        }
        break;
      case 't':
        r = Reply(tid, (char *)(&ticks), 4);
        if (r != 0) PANIC;
        break;
      case 'd':
        /* Put the requesting task's info in the delay_list */
        temp = &delaypool[delayindex];
        delayindex = (delayindex + 1) % MAXTASKS;
        temp->tid = tid;
        temp->time = ticks + *((int *)(buf + 1));
        heap_push(temp, &delay_heap);
        break;
      default:
        bwputstr(COM2, "ERROR: CLOCKSERV(): incorrect message received\r\n");
        PANIC;
    }
  }
}
