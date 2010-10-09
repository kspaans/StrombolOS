#ifndef __TESTS_H__
#define __TESTS_H__

#define PANIC *((int *)0xFFFFFFFF) = 1

void srr_tests();
void srr0();
void srr1();
void returny();

#endif/*__TESTS_H__*/
