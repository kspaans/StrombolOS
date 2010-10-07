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

int is_valid_tid(int tid);

int _kSend(struct td *mytd, int Tid, char *msg, int msglen, char *reply,
           int replylen, struct td *tds)
{
  char *sentdata; // or a message structure?
  int sentlen;

  if (Tid < 0) { // the only impossible TIDs?
    return -1;
  }
  if (!is_valid_tid(Tid)) {
    return -2;
  }

  // COPY DATA
  sentdata = mytd->mq->data;
  sentlen  = mytd->mq->mlen;
  while (msglen--) {
    *msg++ = *sentdata;
  } // LOOKS CORRECT TO ME!!! >.<

  if (!waiting_for_recv(Tid)) {
    mytd->state = SEND_BLOCKED;
  }

  return sentlen;
}
