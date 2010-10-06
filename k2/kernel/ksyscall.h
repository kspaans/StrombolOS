int _kCreate(struct td *newtd, int priority, void (*code)(), int parenttid,
             int newtid, int *stack);
int _kMyTid(struct td *taskd);
int _kMyParentTid(struct td *taskd);
void _kPass(struct td *taskd);
void _kExit(struct td *taskd);
