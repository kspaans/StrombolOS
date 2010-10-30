#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../kernel/switch.h"
#include "usyscall.h"
#include "user.h"
#include "clock_client.h"
#include "../servers/servers.h"
#include "../ktests/tests.h"

void trains()
{
  char c;
  int i;

  DPRINTOK("Hi, I'm the train controller!\r\n");

  while ((c = bwgetc(COM2)) != 0) {
    switch (c) {
      case 'q':
        goto DONE;
      case 's':
        //Putc(1, 0x9F);
        for (i = 0; i < 31; ++i) {
          bwprintf(COM2, "I read: %c\r\n", Getc(1));
        }
        break;
      default:
        bwputstr(COM2, "WUT\r\n");
        break;
    }
  } DONE:;

  DPRINTOK("Goodbye.\r\n");
  Exit();
}
