#include "../kernel/switch.h"
#include "../user/usyscall.h"

void notifier_uart2tx()
{
  AwaitEvent(UART2TRANS);
}
