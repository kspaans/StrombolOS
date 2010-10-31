#include <bwio.h>   // XXX
#include <ts7200.h>
#include <debug.h>  // XXX
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../ktests/tests.h"

/*
 * Notifier task for the serial server -- Awaits the UART1TRANS event
 */
void notifier_uart1tx()
{
  int r, tid;
  char data = 't';
  tid = WhoIs("com1");

  FOREVER {
    AwaitEvent(UART1TRANS);
//    DPRINTOK ("UART1 TRANSMISSION INTERRUPT.\n");
    *(int*)(UART1_BASE+UART_INTR_OFFSET) |= 0xFFFFFFFF; // clear MSI
    if ( *(int*)(UART1_BASE+UART_FLAG_OFFSET) & TXFE_MASK &&
         *(int*)(UART1_BASE+UART_FLAG_OFFSET) & CTS_MASK) {
      Send (tid, &data,1, NULL, 0);
    }
  }
}
