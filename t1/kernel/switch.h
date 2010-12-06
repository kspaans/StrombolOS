#ifndef SWITCH__INCLUDED
#define SWITCH__INCLUDED

enum STATE {
  ACTIVE,
  READY,
  DEFUNCT,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
  EVENT_BLOCKED,
};

#define NUMPRIO 6
enum PRIORITY {
  INTERRUPT    = 0,
  SYSCALL_HIGH = 1,
  SYSCALL_LOW  = 2,
  USER_HIGH    = 3,
  USER_LOW     = 4,
  IDLE         = 5,
};

#define NUMEVENTS 5
enum EVENT {
  TIMER1  = 0,
  UART1RX = 1,
  UART2RX = 2,
  UART1TRANS = 3,
  UART2TRANS = 4
};

/*
 * Playing fast and loose with memory, a bunch of these represent the message
 * queue of the task -- pointing into the sender's memory
 */
struct mq {
  int msglen;
  char *msg;
  int tid;
};

struct trapframe {
  int r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14;  
};


#define MQSIZE 10 // We can have at most this many tasks send to us at once
struct td {
  int tid;                // Task ID
  int SPSR;               // Saved cpu state.
  void *entry;	  // Where to re-enter the user program.	
  struct trapframe trap;  // Saved registers
  int *stack;             // memory, user's state
  enum STATE state;       // to be impl'd
  enum PRIORITY priority; // Task's priority --  NOT NEEDED?
  struct td *next;        // next td
  int ptid;               // Task's Parent's TID
  struct mq messageq[MQSIZE]; // Pointer to the next unused slot in the MQ
  int mq_next;            // next message in queue to be processed
  int mq_last;            // free slot in the queue
  unsigned int time_active;
  struct mq replyq;
  struct mq fuckq;
};

int swtch(int arg);
void print_mode();
void print_regs();
int activate(struct trapframe *t, int SPSR, void (*entry)());
void install_handler();

#define MAXTASKS 50           // Should do for now.
#define STACKSIZE 327680       // 320KB

#define FOREVER for(;;)
#define FOREACH(i,n) for(i = 0; i < n; ++i)
#define NULL (void *)0

#endif // SWITCH__INCLUDED
