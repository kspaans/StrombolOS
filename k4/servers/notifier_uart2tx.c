#include "../kernel/switch.h"

void notifier_uart2tx()
{
  AwaitEvent(UART2TX);
}
