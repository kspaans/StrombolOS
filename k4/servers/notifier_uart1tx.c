#include "../kernel/switch.h"

void notifier_uart1tx()
{
  AwaitEvent(UART1TX);
}
