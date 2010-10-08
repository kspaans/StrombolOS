/*
1 - check validity of TID given...
2 - copy mesg to mq of recv'ing task
3 - if no mesg waiting to recv, BLOCK
4 - regardless of result, return send mesg size
*/

#include <bwio.h>
#include <ts7200.h>
#include "../switch.h"
#include "ksyscall.h"

int _kSend(struct td *mytd, int Tid, char *msg, int msglen, char *reply,
           int replylen, struct td *tds, int current_tid)
{
  if (Tid < 0) { // the only impossible TIDs?
    return -1;
  }
  if (Tid >= current_tid) {
    return -2;
  }

  // Regardless of whether or not Receive() has been called, store the message
  // in the recipient's message queue

  if (tds[Tid].state != RECEIVE_BLOCKED) {
    mytd->state = SEND_BLOCKED;
  } 
  else {
    mytd->state = REPLY_BLOCKED;
    tds[Tid].state = READY;
    
    //copy into replyq.msg, for this edgecase
    char *sendbuf = tds[Tid].replyq.msg;
    int buflen  = tds[Tid].replyq.msglen;
    while (buflen-- && msglen--) {
      *sendbuf++ = *msg++;
    }

  // Tell Reply() where to reply to
  tds[Tid].replyq.msg    = reply;
  tds[Tid].replyq.msglen = replylen;
  //bwprintf (COM2, "going to derefence %d, punk.\r\n\r\n\r\n", mytd
  *((int*)tds[Tid].replyq.tid) = mytd->tid;
    bwprintf (COM2, "kSend: setting %d to READY.\r\n", Tid);
    return 0;
  }

  tds[Tid].messageq[tds[Tid].mq_last].msg = msg;
  tds[Tid].messageq[tds[Tid].mq_last].msglen = msglen;
  tds[Tid].messageq[tds[Tid].mq_last].tid = mytd->tid;
  tds[Tid].mq_last = (tds[Tid].mq_last + 1) % MQSIZE;
  // Tell Reply() where to reply to
  mytd->replyq.msg    = reply;
  mytd->replyq.msglen = replylen;
  mytd->replyq.tid    = mytd->tid; /// Uhh, prolly not necessary????

  return 0; // how to get replylen?
}
