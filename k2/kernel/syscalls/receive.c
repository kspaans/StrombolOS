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
#include "../switch.h"
#include "ksyscall.h"

int _kReceive(struct td *mytd, int *tid, char *msg, int msglen, struct td *tds)
{
  char *sentdata;
  int sentlen;
//  bwputstr(COM2, "RECEIVE 0\r\n");
 
//  bwprintf(COM2, "RECEIVE: tid: %d next: %d last: %d\r\n", mytd->tid, mytd->mq_next, mytd->mq_last);

  if (mytd->mq_next == mytd->mq_last) {
    bwprintf (COM2, "kReceive: BLOCKED\r\n");
    mytd->state = RECEIVE_BLOCKED;
    mytd->replyq.msg = msg;
    mytd->replyq.msglen = msglen;
    mytd->replyq.tid = tid;
    /* ... do the proper scheduling stuff to block us ... call Pass() maybe? */
    return 666;
  }
  *tid = mytd->messageq[mytd->mq_next].tid;
  bwprintf (COM2, "kReceive: next: %d, last: %d\r\n",mytd->mq_next, mytd->mq_last);
//  bwputstr(COM2, "RECEIVE: 1\r\n");
  sentdata = mytd->messageq[mytd->mq_next].msg;
//  bwputstr(COM2, "RECEIVE: 2\r\n");
  sentlen  = mytd->messageq[mytd->mq_next].msglen;
//  bwputstr(COM2, "RECEIVE: 3\r\n");
  while (msglen-- && sentlen--) {
    *msg++ = *sentdata++;
  }
  // "Clear" this message struct from the queue
  mytd->mq_next = (mytd->mq_next + 1) % MQSIZE;
  
  // Sender is now REPLY_BLOCKED, waiting for someone to reply
  tds[*tid].state = REPLY_BLOCKED;
  
 // bwprintf (COM2, "RECEIVE: BITCH\r\n");
  return mytd->messageq[mytd->mq_next].msglen;
}
