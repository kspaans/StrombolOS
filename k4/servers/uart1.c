#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL

#define BUFSIZE 5

/*
 * SERIAL UART SERVER PROTOCOL // share with uart2.c?
 * DOT:
digraph uart1serv {
  // a -> b [label="Sends To"];
  uart1rx -> uart1serv;
  uart1tx -> uart1serv;
  // XXX couriers!
  user_wrappers -> uart1serv;
}
 */
void uart1serv()
{
  int r;
  int rxtid, txtid, client_tid;
  char buf[BUFSIZE];

  RegisterAs("com1");
  rxtid =  Create(INTERRUPT, notifier_uart1rx);
  if (rxtid < 0) PANIC;
  //txtid = Create(INTERRUPT, notifier_uart1tx);
  //if (rxtid < 0) PANIC;

  FOREVER {
    r = Receive(&client_tid, buf, BUFSIZE);
    DPRINTOK("UART1: received from client tid %d, mesg of size %d, first char"
             "%c\r\n", client_tid, r, buf[0]);
    if (r < 0 || r > BUFSIZE) PANIC;

    switch (buf[0]) {
      case 'r':
        if (client_tid != rxtid) {
          DPRINTERR("UART1: WTF notifier didn't send us this RX, %d did\r\n",
                    client_tid);
          PANIC;
        }
        if (Reply(rxtid, NULL, 0) != 0) PANIC;
        //enqueue(buf[0]);
        break;
      case 'g':
        /** Probably want a courier or two between the train RX ans us **/
        // So what if no char is ready yet? We want to sleep the client, and
        // somehow wait for next char, while still responding to ther clients
        // Luckily we shouldn't have to worry about a floor of clients getc'ing
        // from us -- in fact we may even want to not really use getc, and
        // warehouse instead
        //
        //if (Reply(client_tid, dequeue(), 1) != 0) PANIC;
        break;
      case 'p':
        //if (Reply(txtid, "putchar plz") != 0) PANIC;
        bwputc(COM1, buf[1]);
        if (Reply(client_tid, NULL, 0) != 0) PANIC;
        break;
      default:
        PANIC;
    }
  }
}
