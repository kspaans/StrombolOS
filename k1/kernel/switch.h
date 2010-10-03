#ifndef SWITCH__INCLUDED
#define SWITCH__INCLUDED

enum STATE {
  ACTIVE,
  READY,
  DEFUNCT,
}; /* More can be added in K2 */

/*
 * Why not split what Prof Cowan suggested:
 * INTERRUPT
 * SYSCALL
 * USER
 * IDLE
 */
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

#endif // SWITCH__INCLUDED
