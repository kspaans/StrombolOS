#include <bwio.h>

void handle()
{
  bwputstr(COM2, "Me handler?\n");
}

void install_handler(void (*fp)())
{
  //bwprintf(COM2, "< init> Using %x as the exception handler\n", (int)fp);
  asm(
    "\tMOV\tr1, #0x28\n"
    "\tSTR\tr0, [r1]\n"
  );
}
