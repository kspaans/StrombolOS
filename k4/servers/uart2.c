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
 * SERIAL UART SERVER PROTOCOL
 * read   "r#" | notifier read a byte from the UART
 * Getc() "g"  | client asks for a byte
 * Putc() "p#" | Xmit the client's byte
 *
 * DOT:
digraph uart1serv {
  // a -> b [label="Sends To"];
  uart2rx -> uart2serv;
  uart2tx -> uart2serv; // Will probably need a courier.
  user_wrappers -> uart2serv;
}
 */
void uart2serv()
{
  int r;
  
  int rxtid;
  int txtid;
  int client_tid;
  char buf[BUFSIZE];
  char input_queue[QUEUESIZE];
  int head = 0, tail = 0;
  int reader_queue[2]; // their TIDs

  reader_queue[0] = 0;
  reader_queue[1] = 0; // XXX FIXME dead code

  RegisterAs("com2");
  rxtid = Create(INTERRUPT, notifier_uart2rx);
  if (rxtid < 0) PANIC;
  txtid = Create(INTERRUPT, notifier_uart2tx);
  if (txtid < 0) PANIC;

  FOREVER {
    r = Receive(&client_tid, buf, BUFSIZE);
    if (r < 0 || r > BUFSIZE) PANIC;

    switch (buf[0]) {
      case 'r':
        if (client_tid != rxtid) {
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
          if (r != 0) {
            DPRINTERR("UART1: reply failed with %d retval to client tid %d\r\n",
                      r, reader_queue[0]);
            PANIC;
          }
        }
        break;
      case 'p':
        //if (Reply(txtid, "putchar plz") != 0) PANIC;
        bwputc(COM2, buf[1]); // will fuck up the ctrlr???
        if (Reply(client_tid, NULL, 0) != 0) PANIC;
        break;
      default:
        PANIC;
    }
  }
}
