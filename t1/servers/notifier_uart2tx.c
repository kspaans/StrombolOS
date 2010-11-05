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
void notifier_uart2tx()
{
  int r, tid;
  char data = 't';
  char dummy;
  tid = WhoIs("com2");

  FOREVER {
    AwaitEvent(UART2TRANS);
 //   DPRINTOK ("UART1 TRANSMISSION INTERRUPT.\n");
    
    if ((*(int*)(UART2_BASE+UART_FLAG_OFFSET) & TXFE_MASK) && (*(uint*)(UART2_BASE+UART_INTR_OFFSET) & 0x4 )) {
      *(uint*)(UART2_BASE+UART_INTR_OFFSET) = 0xFFFFFFFF;  // clear interrupt`
      *(uint*)(VIC2BASE+INTENCL_OFFSET)  = UART2_MASK;
      r = Send (tid, &data,1, &dummy, 1);
      if (r) { *(uint*)(VIC2BASE+INTEN_OFFSET) = UART2_MASK;} // if uart2 server asked us to we turn back on interrupts
    }
  }
}
