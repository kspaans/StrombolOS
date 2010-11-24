#ifndef BOOT__INCLUDED
#define BOOT__INCLUDED

void bootstrap ();

// Load initial count of 0xFFFF, set to 508KHz, free running, and GO!
#define START_TIMER2() { \
  *(uint *)(TIMER2_BASE + LDR_OFFSET)  = 0xFFFF; \
  *(uint *)(TIMER2_BASE + CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK; }

#define READ_TIMER2    *(uint *)(TIMER2_BASE + VAL_OFFSET)
#define DISABLE_TIMER2 *(uint *)(TIMER2_BASE + CRTL_OFFSET) = 0

#define START_TIMER3() { \
  *(uint *)(TIMER3_BASE + LDR_OFFSET)  = 0xFFFFFFFF; \
  *(uint *)(TIMER3_BASE + CRTL_OFFSET) = ENABLE_MASK | CLKSEL_MASK; }

#define READ_TIMER3    *(uint *)(TIMER3_BASE + VAL_OFFSET)
#define DISABLE_TIMER3 *(uint *)(TIMER3_BASE + CRTL_OFFSET) = 0

#endif // BOOT__INCLUDED
