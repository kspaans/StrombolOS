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

int _kReceive(struct td *mytd, int *tid, char *msg, int msglen)
{
  char *sentdata; // or a message structure?
  int sentlen;

  if (mytd->mq == NULL) {
    mytd->status = RECEIVE_BLOCKED;
    /* ... do the proper scheduling stuff to block us ... call Pass() maybe? */
  }

  sentdata = mytd->mq->data;
  sentlen  = mytd->mq->mlen;
  while (msglen--) {
    *msg++ = *sentdata;
  } // LOOKS CORRECT TO ME!!! >.<
  return sentlen;
}
