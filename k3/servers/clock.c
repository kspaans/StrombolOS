#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL

struct delay {
  int time;
  int tid;
  struct delay *next;
};

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
  int r, tid = -1, delaysize = 0, delayindex = 0;
  char buf[BUFSIZE];
  /* We can't have more than N tasks delaying 
     But this will break pretty heavily if we have a long time waiter and then
     lots of little waiters that gobble up the structures. */
  struct delay delaypool[MAXTASKS];
  struct delay *delay_list, *temp, *iter_node, *iter_prev;;

  RegisterAs("clock");

  FOREVER {
    r = Receive(&tid, buf, BUFSIZE);
    DPRINT("received from tid %d, mesg: \'%c%c%c%c%c\r\r\n", tid, buf[0],
           buf[1], buf[2], buf[3], buf[4]);
    /* Though r should never be -1, the way we wrote things */
    /* XXX TEST THIS XXX */
    if (r < 0 || r > BUFSIZE) PANIC;
    switch (buf[0]) {
      case 'n':
bwprintf(COM2, "CLOCK: request type: \'n\'\r\n");
        r = Reply(tid, NULL, 0);
        if (r != 0) PANIC;
        ++ticks;
        while (delaysize > 0 && ticks > delay_list->time) {
          r = Reply(delay_list->tid, NULL, 0);
          if (r != 0) PANIC;
          delay_list = delay_list->next;
          --delaysize;
        }
        break;
      case 't':
bwprintf(COM2, "CLOCK: request type: \'t\'\r\n");
        r = Reply(tid, NULL, 0);
        /* or replybuf = sprintf("%d", ticks); */
        r = Reply(tid, (char *)(&ticks), 4); /* XXX is this unsafe? */
        if (r != 0) PANIC;
        break;
      case 'd':
        /* Put the requesting task's info in the delay_list */
        temp = &delaypool[delayindex];
        delayindex = (delayindex + 1) % MAXTASKS;
        ++delaysize;
        temp->tid = tid;
        temp->time = ticks + *((int *)(buf + 1));
bwprintf(COM2, "CLOCK: request type: \'d\' delay %d\r\n", temp->time);
        /* Perform insertion sort of the new node into the list */
        if (delaysize == 1) {
          delay_list = temp;
          delay_list->next = NULL;
        }
        else {
          iter_node = delay_list;
          iter_prev = NULL;
          while (iter_node != NULL) {
            if (iter_node->time > temp->time) {
              temp->next = iter_node;
              if (iter_prev) iter_prev->next = delay_list;
              break;
            }
            else if (iter_node->next == NULL) {
              iter_node->next = temp;
              temp->next = NULL; /* special case, but meh this works */
            }
            else {
              iter_prev = iter_node;
              iter_node = iter_node->next;
            }
          }
        }
        break;
      default:
        bwputstr(COM2, "ERROR: CLOCKSERV(): incorrect message received\r\n");
        PANIC;
    }
  }
}
