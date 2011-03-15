#ifndef KSYSCALLS__INCLUDED
#define KSYSCALLS__INCLUDED

int _kCreate(struct td *newtd, int priority, void (*code)(), int parenttid,
             int newtid, int *stack);
int _kMyTid(struct td *taskd);
int _kMyParentTid(struct td *taskd);
void _kPass(struct td *taskd);
void _kExit(struct td *taskd);
int _kSend(struct td *mytd, int Tid, char *msg, int msglen, char *reply,
           int replylen, struct td *tds, int current_tid);
int _kReceive(struct td *mytd, int *tid, char *msg, int msglen,
	      struct td *tds);
int _kReply(struct td *mytd, int tid, char *reply, int replylen,
            struct td *tds, int current_tid);
int _kAwaitEvent(struct td *mytd, int tid, struct td *eventq[]);

#endif // KSYSCALLS__INCLUDED
