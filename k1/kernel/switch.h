struct td {
	int tid;      // Task ID
	int *stack;  // memory, user's state
	int state;    // to be impl'd
	int priority; // Task's priority
	void *next;   // for ready queue
	int retval;   // saved return value
	void *pc;     // entry point?
};

int swtch(int arg);
void print_mode();
int activate_lower(struct td *t);
void install_handler();
