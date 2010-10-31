#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL

#define BUFSIZE 2
#define QUEUESIZE 128

/*
 * SERIAL UART SERVER PROTOCOL // share with uart2.c?
 * DOT:
digraph uart1serv {
  // a -> b [label="Sends To"];
  uart1rx -> uart1serv;
  uart1tx -> uart1serv;
  user_wrappers -> uart1serv;
}
 */
void uart1serv()
{
  int r;
  
  int yo1=1234;
  int rxtid;
  int txtid;
  int client_tid;
  int yo2 = 4321;
  char buf[BUFSIZE];
  /* We can receive a max of 64-bits from the controller at once, so have a
   * buffer of twice that size. The queue is controlled by head and tail, items
   * are removed from the head of the queue and removed from the tail.
   * XXX, ugh this should be moved into a library...
   */
  char input_queue[QUEUESIZE];
  int head = 0, tail = 0;
  int reader_queue[2]; // their TIDs

  reader_queue[0] = 0;
  reader_queue[1] = 0;

  RegisterAs("com1");
  rxtid = Create(INTERRUPT, notifier_uart1rx);
  bwprintf (COM2, "\n\n\n\nONLY GET HERE ONCE.\n\n");
  if (rxtid < 0) PANIC;
  //txtid = Create(INTERRUPT, notifier_uart1tx);
  //if (rxtid < 0) PANIC;

  FOREVER {
    r = Receive(&client_tid, buf, BUFSIZE);
//    DPRINTOK("UART1: received from client tid %d, mesg of size %d, first char"
  //           " %c\r\n", client_tid, r, buf[0]);
    if (r < 0 || r > BUFSIZE) PANIC;

    bwprintf (COM2, "yo1 = %d, yo2 = %d\n", yo1, yo2);
    switch (buf[0]) {
      case 'r':
        if (client_tid != rxtid) {
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          bwprintf (COM2, "client_tid = %d, rxtid = %d\n", client_tid, rxtid);
          DPRINTERR("UART1: WTF notifier didn't send us this RX, %d did\r\n",
                    client_tid);
          PANIC;
        }
        if (Reply(rxtid, NULL, 0) != 0) PANIC;
        /* Too bad if we overrun the circular buffer. It should be big enough */
        input_queue[tail] = buf[1];
        tail = (tail + 1) % QUEUESIZE;

        if (reader_queue[0]) { // someone is waiting
          if (Reply(reader_queue[0], &input_queue[head], 1) != 0) PANIC;
          head = (head + 1) % QUEUESIZE;
        }
        break;
      case 'g':
        if (head == tail) { // circular buffer is empty
          reader_queue[0] = client_tid;
        }
        else {
          r = Reply(client_tid, &input_queue[head], 1);
          head = (head + 1) % QUEUESIZE;
          //r = Reply(reader_queue[0], &input_queue[head++], 1);
          if (r != 0) {
            DPRINTERR("UART1: reply failed with %d retval to client tid %d\r\n",
                      r, reader_queue[0]);
            PANIC;
          }
        }
        break;
      case 'p':
        //if (Reply(txtid, "putchar plz") != 0) PANIC;
        bwputc(COM1, buf[1]); // will fuck up the ctrlr???
        if (Reply(client_tid, NULL, 0) != 0) PANIC;
        break;
      default:
        PANIC;
    }
  }
}
