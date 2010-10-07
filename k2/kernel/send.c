/*
1 - check validity of TID given...
2 - copy mesg to mq of recv'ing task
3 - if no mesg waiting to recv, BLOCK
4 - regardless of result, return send mesg size
*/

#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "ksyscall.h"

int _kSend(struct td *mytd, int Tid, char *msg, int msglen, char *reply,
           int replylen)
{
  char *sentdata; // or a message structure?
  int sentlen;

  if (tid < 0) {
    return -1;
  }
  if (!find_tid(tid)) {
    return -2;
  }

  // COPY DATA
  sentdata = mytd->mq->data;
  sentlen  = mytd->mq->mlen;
  while (msglen--) {
    *msg++ = *sentdata;
  } // LOOKS CORRECT TO ME!!! >.<

  if (!waiting_for_recv(Tid)) {
    mytd->status = SEND_BLOCKED;
  }

  return sentlen;
}
