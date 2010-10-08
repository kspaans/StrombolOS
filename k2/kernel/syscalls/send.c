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
  while (msglen--) {
    tds[Tid].messageq[tds[Tid].mq_index].msg = msg;
    tds[Tid].messageq[tds[Tid].mq_index].msglen = msglen;
    tds[Tid].mq_index = (tds[Tid].mq_index + 1) % MQSIZE;
  }

  if (tds[Tid].state != RECEIVE_BLOCKED) {
    mytd->state = SEND_BLOCKED;
    // what to do now?
  }

  return 0; // how to get replylen?
}
