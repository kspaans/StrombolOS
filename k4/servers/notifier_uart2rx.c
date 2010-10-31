#include <bwio.h>   // XXX
#include <ts7200.h>
#include <debug.h>  // XXX
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"

/*
 * Notifier task for the serial server -- Awaits the UART2RX event
 * When it does, it will be given the byte from the kernel, and will turn around
 * and give this byte to the server.
 */
void notifier_uart2rx()
{
  int r, tid;
  char data[2];

  tid = WhoIs("com2");
  data[0] = 'r';
  FOREVER {
    AwaitEvent(UART2RX);
    r = *(char*)(UART2_BASE + UART_DATA_OFFSET); 
    if (r < 0) {
      DPRINTERR("Could not awaitevent(): %d\r\n", r);
      PANIC;
    }
    data[1] = r;
    if (Send(tid, data, 2, NULL, 0) != 0) {
      DPRINTFUNC("notifier_uart2rx()");
      DPRINTERR("Woah, got a bad return value, %d, when sending to SerialServer"
                " tid %d\r\n", r, tid);
      PANIC;
    }
  }
}
