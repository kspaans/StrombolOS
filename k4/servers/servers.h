#ifndef SERVERS__INCLUDED
#define SERVERS__INCLUDED

void nameserv();
void clckserv();
void uart1serv();
void uart2serv();

void rps_server();

void notifier_clock();
void notifier_uart1rx();
void notifier_uart1tx();
void notifier_uart2rx();
void notifier_uart2tx();

#endif // SERVERS__INCLUDED
