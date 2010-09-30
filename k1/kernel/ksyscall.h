int _Create(int priority, void (*code)(), int parenttid);
int _MyTid(struct td *taskd);
int _MyParentTid(struct td *taskd);
void _Pass(struct td *taskd);
void _kExit(struct td *taskd);
