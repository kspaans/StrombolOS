#ifndef USYSCALL__INCLUDED
#define USYSCALL__INCLUDED

int Create(int priority, void (*code)());
int MyTid();
int MyParentTid();
void Pass();
void Exit();
unsigned int Rand ();
void Seed(unsigned int s);
int Send(int Tid, char *msg, int msglen, char *relpy, int replylen);
int Receive(int *tid, char *msg, int msglen);

#endif // USYSCALL__INCLUDED
