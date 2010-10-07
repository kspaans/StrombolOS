#ifndef USYSCALL__INCLUDED
#define USYSCALL__INCLUDED

int Create(int priority, void (*code)());
int MyTid();
int MyParentTid();
void Pass();
void Exit();
unsigned int Rand ();
void Seed(unsigned int s);

#endif // USYSCALL__INCLUDED
