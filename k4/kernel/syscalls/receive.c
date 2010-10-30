/*
should be simple!
1 - Check the current TID's message queue for messages
  1.A - If queue is not empty, GOTO step 2
  1.B - If queue is empty, change status to BLOCKED, move task to blocked queue
        (do we have that?) (must be unblocked by send)
2 - Grab first message in message queue
3 - copy contents of message into recv buffer UP TO MSGLEN bytes (lol, aligned fast copy?)
4 - regardless of result, return send mesg size
*/

#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../switch.h"
#include "ksyscall.h"

int _kReceive(struct td *mytd, int *tid, char *msg, int msglen, struct td *tds)
{
  char *sentdata;
  int sentlen;
  int retval;

  DPRINT(">>> Entered with mytd 0x%x, tid(0x%x) %d, msg(0x%x)\'%s\', msglen %d,"
         "tds 0x%x\r\n", mytd, tid, *tid, msg, msg, msglen, tds);
 
  if (mytd->mq_next == mytd->mq_last) {
    DPRINT("||| Blocked!\r\n");
    mytd->state = RECEIVE_BLOCKED;
    mytd->fuckq.msg = msg;
    mytd->fuckq.msglen = msglen;
    mytd->fuckq.tid = (int)tid;
    return -666; // This value gets overwritten by _kSend();
  }

  *tid = mytd->messageq[mytd->mq_next].tid;
  DPRINT("next %d, last %d\r\n", mytd->mq_next, mytd->mq_last);
  sentdata = mytd->messageq[mytd->mq_next].msg;
  sentlen  = mytd->messageq[mytd->mq_next].msglen;
  retval   = mytd->messageq[mytd->mq_next].msglen;
  DPRINT("Taking from message queue 0x%x, len %d\r\n", sentdata, sentlen);
  while (msglen-- && sentlen--) {
    *msg++ = *sentdata++;
  }
  // "Clear" this message struct from the queue
  mytd->mq_next = (mytd->mq_next + 1) % MQSIZE;
  
  // Sender is now REPLY_BLOCKED, waiting for someone to reply
  tds[*tid].state = REPLY_BLOCKED;
  
  DPRINT("<<< returning\r\n");
  return retval;
}
