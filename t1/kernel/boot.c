#include "boot.h"
#include "switch.h"
#include "syscalls/ksyscall.h"
#include <debug.h>
#include <bwio.h>
#include <ts7200.h>

typedef unsigned int uint; 

void enable_timer () {
  *(uint*)(TIMER1_BASE+LDR_OFFSET)  = 100;		       // Trigger interrupt every 0.1s
  *(uint*)(TIMER1_BASE+CRTL_OFFSET) = ENABLE_MASK | MODE_MASK; // Run at 2kHz, periodic mode.
  *(uint*)(TIMER1_BASE+CLR_OFFSET)   = 1;                       // Initially clear the timer. Necessary?
//  DPRINTOK ("Timer 1 enabled.\n");
}

void enable_uart () {
  *(uint*)(UART1_BASE+UART_CTLR_OFFSET) = UARTEN_MASK | MSIEN_MASK | RIEN_MASK;
  *(uint*)(UART2_BASE+UART_CTLR_OFFSET) = UARTEN_MASK | RIEN_MASK | TIEN_MASK ;
}
void enable_interrupts () {
  *(uint*)(VIC1BASE+INTSEL_OFFSET)     &= 0;          // Use IRQ mode
  *(uint*)(VIC2BASE+INTSEL_OFFSET)     &= 0;
  *(uint*)(VIC1BASE+TIMER1CTRL_OFFSET)  = 0x20;       // Enable the timer
  *(uint*)(VIC1BASE+INTEN_OFFSET)       = TC1OI_MASK | UART1RXINTR1_MASK | UART2RXINTR1_MASK; // ... enable the timer? what is difference
//  DPRINTOK ("Interrupts enabled!\n");
}

void bootstrap (struct td *tds, void (*f)(), int *stacks) {
//  bwprintf (COM2, "[s[2J[2;1000r[;H[45mThis is a taskbar."
//      "[K[m[u");
//  bwprintf(COM2, "[?25l[2;1H");
//  bwputstr(COM2, "Welcome to\r\n"
//  "+--------------------------------------------------+\r\n"
//  "|    ______                 __        ______  ____ |\r\n"
//  "|   / __/ /________  __ _  / /  ___  / / __ \\/ __/ |\r\n"
//  "|  _\\ \\/ __/ __/ _ \\/  ' \\/ _ \\/ _ \\/ / /_/ /\\ \\   |\r\n"
//  "| /___/\\__/_/  \\___/_/_/_/_.__/\\___/_/\\____/___/   |\r\n"
//  "|                             v0.0.4 (Turbo Samba) |\r\n"
//  "+--------------------------------------------------+\r\n\r\n");

//  DPRINTFUNC ("bootstrap");
  install_handler ();
//  DPRINTOK ("Interrupt handler installed.\n");
  enable_timer ();
  enable_uart ();
  enable_interrupts ();
  _kCreate(tds, IDLE, f, 0, 0, stacks);
//  DPRINTOK ("First user task created.\n");
//  DPRINTOK ("Booting complete.\n");
}
