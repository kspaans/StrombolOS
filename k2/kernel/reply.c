/*
1 - Check if the TID exists
2 - look at the blocked list for send-blocked tasks, for one with TID
  2A - If such a task does not exist, return -3
3 - Copy the reply into the sender's buffer
4 - set status to READY, but make sure that if caller is same priority as sender, that sender gets to go first
*/

#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "ksyscall.h"

int _kReply(struct td *mytd, int tid, char *reply, int replylen)
{
  char *sentdata; // or a message structure?
  int sentlen;

  if (tid < 0) {
    return -1;
  }
  if (!find_tid(tid)) {
    return -2;
  }
  if (tds[tid].status != SEND_BLOCKED) {
    return -3;
  }

  // check for space in sender's buffer
  /*   SEND DATA ...
  sentdata = mytd->mq->data;
  sentlen  = mytd->mq->mlen;
  while (msglen--) {
    *msg++ = *sentdata;
  } // LOOKS CORRECT TO ME!!! >.<
  */
  if (overflow) {
    return -4;
  }

  //mytd->status = READY; // if it wasn't already?, it should be

  return 0;
}
