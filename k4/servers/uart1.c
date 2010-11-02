#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL

#define BUFSIZE 2
#define QUEUESIZE 128
#define OQUEUESIZE 512

typedef unsigned int uint;
/*
 * SERIAL UART SERVER PROTOCOL
 * read   "r#" | notifier read a byte from the UART
 * Getc() "g"  | client asks for a byte
 * Putc() "p#" | Xmit the client's byte
 *
 * DOT:
digraph uart1serv {
  // a -> b [label="Sends To"];
  uart1rx -> uart1serv;
  uart1tx -> uart1serv;
  user_wrappers -> uart1serv;
}
 */

int transmit (char *q, int head, int tail) {
  int nh = (head+1) % OQUEUESIZE;
  *(uint*)(UART1_BASE+UART_DATA_OFFSET) = q[head];
  return nh;
}

void uart1serv()
{
  int r;
  int cts = 1;
  int rxtid;
  int txtid;
  int client_tid;
  char buf[BUFSIZE];
  char dummy;
  /* We can receive a max of 64-bits from the controller at once, so have a
   * buffer of twice that size. The queue is controlled by head and tail, items
   * are removed from the head of the queue and removed from the tail.
   * XXX, ugh this should be moved into a library...
   */
  char input_queue[QUEUESIZE];
  int head = 0, tail = 0;
  int reader_queue[2]; // their TIDs
  char output_queue[OQUEUESIZE];
  int ohead = 0, otail = 0;

  reader_queue[0] = 0;
  reader_queue[1] = 0;

  RegisterAs("com1");
  DPRINTOK ("Creating uart1 rx notifier.\n");
  rxtid = Create(INTERRUPT, notifier_uart1rx);
  if (rxtid < 0) PANIC;
  DPRINTOK ("Creating uart1 tx notifier.\n");
  txtid = Create(INTERRUPT, notifier_uart1tx);
  if (txtid < 0) PANIC;


  FOREVER {
    r = Receive(&client_tid, buf, BUFSIZE);
//    DPRINTOK("UART1: received from client tid %d, mesg of size %d, first char"
  //           " %c\r\n", client_tid, r, buf[0]);
    if (r < 0 || r > BUFSIZE) PANIC;
    switch (buf[0]) {
      case 'r': // from rxtd
        if (client_tid != rxtid) {
          DPRINTERR("UART1: WTF notifier didn't send us this RX, %d did\r\n",
                   client_tid);
          PANIC;
        }
        if (Reply(client_tid, NULL, 0) != 0) PANIC;
        /* Too bad if we overrun the circular buffer. It should be big enough */
        input_queue[tail] = buf[1];
        tail = (tail + 1) % QUEUESIZE;
        bwprintf (COM2, "got %c\n", buf[1]);
        if (reader_queue[0]) { // someone is waiting
          if (Reply(reader_queue[0], &input_queue[head], 1) != 0) PANIC;
          head = (head + 1) % QUEUESIZE;
        }
        break;
      case 'g': // from getc client
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
      case 't': // from txtd
        if (client_tid != txtid) { DPRINTERR ("non-client sent a tx interrupt message.\n"); PANIC; }
        if (cts >= 1) { DPRINTHUH ("Train controller reset maybe?"); Reply (client_tid, NULL, 0); }
        cts = 1;
        if (ohead != otail) {
          ohead = transmit (output_queue, ohead, otail);
          cts = 0;
          Reply (client_tid, &dummy, 1); // we need interrupts on for the cts stuff
        }
        else {
          Reply (client_tid, NULL, 0); // 0 means don't turn interrupts back on
        }
        break;
      case 'p': // from putc client
        //if (Reply(txtid, "putchar plz") != 0) PANIC;
        output_queue[otail] = buf[1];
        otail = (otail + 1) % OQUEUESIZE;
        if (cts == 1) {
          ohead = transmit (output_queue, ohead, otail); 
          cts = 0; 
          //bwprintf (COM2, "sent a char (putc), now cts %d\n", cts);
        }
        *(uint*)(VIC2BASE+INTEN_OFFSET) = UART1_MASK; 
        Reply (client_tid, NULL, 0); // interrupts on (we need to wait until we know we are really cts) TODO: could cts and txfe happen at the same time?
        break;
      default:
        DPRINTERR ("unknown message to uart1 server.\n");  
    }
  }
}
