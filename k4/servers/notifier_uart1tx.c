#include <bwio.h>   // XXX
#include <ts7200.h>
#include <debug.h>  // XXX
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"

typedef unsigned int uint;
/*
 * Notifier task for the serial server -- Awaits the UART1TRANS event
 */
void notifier_uart1tx()
{
  int r, tid;
  char data = 't';
  char dummy;
  tid = WhoIs("com1");

  FOREVER {
    AwaitEvent(UART1TRANS);
    *(uint*)(UART1_BASE+UART_INTR_OFFSET) = 0xFFFFFFFF;  // clear interrupt

 //   DPRINTOK ("UART1 TRANSMISSION INTERRUPT.\n");
    
    if ((*(int*)(UART1_BASE+UART_FLAG_OFFSET) & CTS_MASK) && (*(int*)(UART1_BASE+UART_FLAG_OFFSET) & TXFE_MASK)) {
      *(uint*)(VIC2BASE+INTENCL_OFFSET)  = UART1_MASK;
      r = Send (tid, &data,1, &dummy, 1);
      if (r) { *(uint*)(VIC2BASE+INTEN_OFFSET) = UART1_MASK;} // if uart1 server asked us to we turn back on interrupts
    }
  }
}
