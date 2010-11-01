#include <bwio.h>   // XXX
#include <ts7200.h>
#include <debug.h>  // XXX
#include "servers.h"
#include "../kernel/switch.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"

typedef unsigned int uint;

/*
 * Notifier task for the serial server -- Awaits the UART1TRANS event
 */
void notifier_uart2tx()
{
  int tid, blah;
  char data = 't';
  unsigned char *uzbits = (unsigned char *)0x01FDCFC0;
  tid = WhoIs("com2");

  UseBits(uzbits, 23);
  Receive(&blah, NULL, 0);
  Reply(blah, NULL, 0);
  UseBits(uzbits, 24);
  FOREVER {
    AwaitEvent(UART2TRANS);
    // Turn off the interrupt for now
    *(uint*)(UART2_BASE+UART_CTLR_OFFSET) &= ~TIEN_MASK;
    UseBits(uzbits, 25);
    *(int*)(UART2_BASE+UART_INTR_OFFSET) |= 0xFFFFFFFF; // clear MSI
    if (*(int*)(UART2_BASE+UART_FLAG_OFFSET) & TXFE_MASK) {
      UseBits(uzbits, 26);
      Send (tid, &data, 1, NULL, 0);
      UseBits(uzbits, 27);
    }
  }
}
