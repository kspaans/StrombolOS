#include <bwio.h>
#include <ts7200.h>

void fuck () {
  bwputstr (COM2, "swi totally worked. going into infinite loop..\n");
  while (1) {}
}

int main () {
  bwsetfifo (COM2, OFF);
  bwputstr (COM2, "[2JHello. in main.\n");


  asm ("ldr r4, .ADDR\n\t"
       "mov r5, #0x28\n\t"
       "str r4, [r5]\n\t");   // puts 0x218000 into 0x28

  bwputstr (COM2, "handler installed! gonna swi this shit nigggaaaaa\n");
 
  asm ("swi 0\n\t");
 
  bwputstr (COM2, "SHOULD NOT BE HERE.\n");

  asm ("ldmfd sp!, {sl, pc}\n\t"       // return 0;
       ".ADDR: .word 0x00218000\n\t");
 
}



