#ifndef SERVERS__INCLUDED
#define SERVERS__INCLUDED

void nameserv();
void clckserv();
void uart1serv();
void uart2serv();

void track();
void trains();
void rps_server();
void lock123();

void cali();

void notifier_clock();
void notifier_uart1rx();
void notifier_uart1tx();
void notifier_uart2rx();
void notifier_uart2tx();

int   fuckswitch(int x);
int unfuckswitch(int x);
int strcmp(const char *a, const char *b);

#endif // SERVERS__INCLUDED
