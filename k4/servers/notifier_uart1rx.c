#include <bwio.h>   // XXX
#include <ts7200.h>
#include <debug.h>  // XXX
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"

typedef unsigned int uint;
/*
 * Notifier task for the serial server -- Awaits the UART1RX event
 * When it does, it will be given the byte from the kernel, and will turn around
 * and give this byte to the server.
 */
void notifier_uart1rx()
{
  int r, tid;
  char data[2];

  tid = WhoIs("com1");
  data[0] = 'r';
  FOREVER {
    // turn interrupts on here 
    AwaitEvent(UART1RX);
    // turn interrupts off here

    r = *(char*)(UART1_BASE+UART_DATA_OFFSET); 
    if (r < 0) {
      DPRINTERR("Could not awaitevent(): %d\r\n", r);
      PANIC;
    }
    data[1] = r;
    Send (tid, data,2,NULL,0);
  }
}
