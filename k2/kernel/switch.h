#ifndef SWITCH__INCLUDED
#define SWITCH__INCLUDED

enum STATE {
  ACTIVE,
  READY,
  DEFUNCT,
  SEND_BLOCKED,
  RECEIVE_BLOCKED,
  REPLY_BLOCKED,
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

struct td {
	int tid;                // Task ID
	int *stack;             // memory, user's state
	enum STATE state;       // to be impl'd
	enum PRIORITY priority; // Task's priority --  NOT NEEDED?
	struct td *next;	// next td
	int retval;             // saved return value
	void *pc;               // entry point?
	int ptid;               // Task's Parent's TID
};

int swtch(int arg);
void print_mode();
int activate(struct td *t, int retval);
void install_handler();

#define MAXTASKS 50    // Should do for now.
#define STACKSIZE 1024 // Probably a bit much?

#define FOREVER for(;;)
#define FOREACH(i,n) for(i = 0; i < n; ++i)
#define NULL (void *)0

#endif // SWITCH__INCLUDED
