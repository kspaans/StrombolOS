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
  struct delay delaypool[MAXTASKS];
  struct delay *delay_list = NULL, *temp, *iter_node, *iter_prev;;

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
        //bwputstr(COM2, "LALA\r\n");
        if (r != 0) { bwputstr(COM2, "haha\r\n");PANIC; }
        ++ticks;
  //bwprintf (COM2, "[45m[s[1;25HSilly time = %d[K[m[u",ticks);
  //      bwprintf(COM2, "[s[1;100H[2K[15D%d[u", ticks);
        //DPRINTOK("Clock tick!\r\n");
        while (delay_list && ticks > delay_list->time) {
          DPRINT("Finding a delayer to wake... size of list %d\r\n", delaysize);
          /*
          for (temp = delay_list; temp != NULL; temp = temp->next) {
            DPRINTOK("  {time %d, tid %d} ->\r\n", temp->time, temp->tid);
            if (temp == temp->next) {
              bwputstr(COM2, "AHHHHH 1-node linked list loop\r\n");
              PANIC;
            }
          }
          DPRINT("  NULL\r\n");
          */
          r = Reply(delay_list->tid, NULL, 0); // error in here
          if (r != 0) {
            DPRINTERR("WOAH, failed replying to Notifier: r = %d, the del was "
                      "%x\r\n", r, delay_list);
            PANIC;
          }
          delay_list = delay_list->next;
          --delaysize;
          DPRINT("Replied, size of list is now %d\r\n", delaysize);
        }
        //bwputstr(COM2, "LALA3\r\n");
        break;
      case 't':
        r = Reply(tid, (char *)(&ticks), 4); /* XXX is this unsafe? */
        if (r != 0) PANIC;
        break;
      case 'd':
        /* Put the requesting task's info in the delay_list */
        temp = &delaypool[delayindex];
        delayindex = (delayindex + 1) % MAXTASKS;
        ++delaysize;
        DPRINT("Adding delayer %d, size of the list is now %d\r\n", tid, delaysize);
        temp->tid = tid;
        temp->time = ticks + *((int *)(buf + 1));
        /* Perform insertion sort of the new node into the list */
        if (delaysize == 1) {
          DPRINT("easy case, the delay list is empty\r\n");
          delay_list = temp;
          delay_list->next = NULL;
        }
        else {
          DPRINT("hard case, the delay list needs insertion sort!\r\n");
          iter_node = delay_list;
          iter_prev = NULL;
          while (iter_node != NULL) {
            DPRINT("Our time %d vs their time %d, their next %x\r\n",
                   temp->time, iter_node->time, iter_node->next);
            if (iter_node->time > temp->time) {
              temp->next = iter_node;
              if (iter_prev) {
                iter_prev->next = temp;
              }
              else {
                delay_list = temp;
              }
              break;
            }
            else if (iter_node->next == NULL) {
              DPRINT("Special case! Insert at the end of the list\r\n");
              iter_node->next = temp;
              temp->next = NULL; /* special case, but meh this works */
              break;
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
