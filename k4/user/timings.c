/*
 * Special timings wrapper around syscalls. It calls SWI itself.
 * Expects the 508KHz timer to be setup and ready.
 */

#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "clock_client.h"
#include "../servers/servers.h"
#include "../ktests/tests.h"

void sender();
void receiver();

void timings()
{
  int start_time = 0xFFFF; // We know initial time to be this.
  int *timer_value_addr = (int *)(TIMER2_BASE + VAL_OFFSET);
  int end_time;
  int i, s, r;
  int child;

  FOREACH(i, 2) {
    *(int *)(TIMER2_BASE + LDR_OFFSET)  = 0xFFFF;      // Load the initial count
    *(int *)(TIMER2_BASE + CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK; // Set to 508KHz, free-running mode and GO!

    //asm("MOV r0, #5\n\tMOV r1, #0\n\tSWI #0\n\t"); // Create();
    asm("SWI #1\n\t"); // MyTid();
    //asm("SWI #2\n\t"); // MyParentTid();
    //asm("SWI #3\n\t"); // Pass();
    //asm("SWI #4\n\t"); // Exit(); can't test this!
    //asm("SWI #5\n\t"); // Send();
    //asm("SWI #6\n\t"); // Receive();
    //asm("SWI #7\n\t"); // Reply();
    //s = Create(USER_LOW, &sender);
    //r = Create(USER_LOW, &receiver);
    //Receive(&child, NULL, 0);
    //Send(s, NULL, 0, NULL, 0);
    //Reply(child, NULL, 0);
    //Receive(&child, NULL, 0);
    //Reply(child, NULL, 0);
    //Receive(&child, NULL, 0);
    //Reply(child, NULL, 0);
    //asm("SWI #8\n\t"); // AwaitEvent(); also hard to test?
    end_time = *timer_value_addr;

    // Disable the clock again
    *(int *)(TIMER2_BASE + CRTL_OFFSET) = 0;

    bwprintf(COM2, "Syscall took %d 508KHz ticks\r\n", start_time - end_time);
  }

  Exit();
}

void sender()
{
  int start_time = 0xFFFF; // We know initial time to be this.
  int *timer_value_addr = (int *)(TIMER2_BASE + VAL_OFFSET);
  int end_time;
  int recipient;
  char buf[4];
  int i;

  Receive(&recipient, NULL, 0);
  Reply(recipient, NULL, 0);
  recipient = WhoIs("zyx");
  //bwprintf(COM2, "Sending to receiver with TID %d\r\n", recipient);

  FOREACH(i, 200) {
  *(int *)(TIMER2_BASE + LDR_OFFSET)  = 0xFFFF;      // Load the initial count
  *(int *)(TIMER2_BASE + CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK; // Set to 508KHz, free-running mode and GO!

  Send(recipient, buf, 4, buf, 4);

  end_time = *timer_value_addr;

  // Disable the clock again
  *(int *)(TIMER2_BASE + CRTL_OFFSET) = 0;

  bwprintf(COM2, "SRR, 4 bytes, took %d 508KHz ticks\r\n", start_time - end_time);
  }

  Send(MyParentTid(), NULL, 0, NULL, 0);
  Exit();
}

void receiver()
{
  int sender;
  char buf[4];
  int i;

  //bwprintf(COM2, "receiver's TID is %d\r\n", MyTid());
  RegisterAs("zyx");
  Send(MyParentTid(), NULL, 0, NULL, 0);

  FOREACH(i, 200) {
  Receive(&sender, buf, 4);
  Reply(sender, buf, 4);
  }

  Send(MyParentTid(), NULL, 0, NULL, 0);
  Exit();
}
