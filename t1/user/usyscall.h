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
int Reply(int tid, char *reply, int replylen);
int RegisterAs (char *name);
int WhoIs (char *name);
int Delay(int ticks);
int Time();
int AwaitEvent(int event);
void Shutdown();

int Getc(int channel);
int Getc_r(int ch);
int Putc(int channel, char ch);

void LockAcquire(int l);
void LockRelease(int l);

int ReserveChunks(int sensor, int distance);
void ReleaseAll();

#endif // USYSCALL__INCLUDED
