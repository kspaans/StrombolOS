int _kCreate(int priority, void (*code)(), int parenttid);
int _kMyTid(struct td *taskd);
int _kMyParentTid(struct td *taskd);
void _kPass(struct td *taskd);
void _kkExit(struct td *taskd);
