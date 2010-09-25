#include <bwio.h>
#include <ts7200.h>

void lols () {
  bwputc (COM2,'U');
  bwputc (COM2,'U');
  bwputc (COM2,'U');
  bwputc (COM2,'U');
  bwputc (COM2,'U');
//  while (1) {}
}

int main () {
  void (*fp)(void) = lols;
  bwsetfifo (COM2, OFF);
  bwputc (COM2, 'F');
  asm ("ldr r1, .lawl\n\t"
       //"ldr r6, KYLE\n\t"
       "mov r8, #8\n\t"
 //      "str r1, [r8]\n\t"
       "str r1, [r8]\n\t"
       "mov r8, #12\n\t"
       "str r1, [r8]\n\t"
       "mov r8, #16\n\t"
       "str r1, [r8]\n\t"
       "mov r8, #20\n\t"
       "str r1, [r8]\n\t"
       "swi 0\n\t"//"mov pc, #8\n\t"
       "ldr pc, [sp], #4\n\t"
       "mov pc, #0x08\n\t"  //      "swi 0\n\t"
       ".lawl:  ldr pc, [sp, #0]\n\t"    
 //      "KYLE: .word 0x00218000\n\t"
);
  return 0;
}
