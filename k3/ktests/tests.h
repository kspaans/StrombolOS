#ifndef __TESTS_H__
#define __TESTS_H__

#define PANIC {bwputstr(COM2, "PANIC PANIC PANIC PANIC  PANIC PANIC PANIC PANIC\r\n"); \
  *((int *)0xFFFFFFFF) = 1; }

void srr_tests();
void srr0();
void srr1();
void returny();

#endif/*__TESTS_H__*/
