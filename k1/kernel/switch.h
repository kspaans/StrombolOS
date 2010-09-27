struct td {
	int tid;      // Task ID
	int *stack;  // memory, user's state
	int state;    // to be impl'd
	int priority; // Task's priority
	void *next;   // for ready queue
	int retval;   // saved return value
	void *pc;     // entry point?
};

int swtch(int arg, void (*func)());
void change_mode(int mode);
void print_mode();
void activate_lower(struct td *t);
