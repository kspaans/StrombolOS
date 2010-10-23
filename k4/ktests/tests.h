#ifndef __TESTS_H__
#define __TESTS_H__

#define PANIC {bwprintf(COM2, "%s:%d: PANIC PANIC PANIC PANIC  PANIC PANIC PANIC PANIC\r\n", __FILE__, __LINE__); \
  *((int *)0xFFFFFFFF) = 1; }

void srr_tests();
void srr0();
void srr1();
void returny();
void send_tests();

#endif/*__TESTS_H__*/
