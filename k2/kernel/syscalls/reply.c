/*
1 - Check if the TID exists
2 - look at the blocked list for send-blocked tasks, for one with TID
  2A - If such a task does not exist, return -3
3 - Copy the reply into the sender's buffer
4 - set status to READY, but make sure that if caller is same priority as sender, that sender gets to go first
*/

#include <bwio.h>
#include <ts7200.h>
#include "../switch.h"
#include "ksyscall.h"

int _kReply(struct td *mytd, int tid, char *reply, int replylen,
            struct td *tds, int current_tid)
{
  char *replybuf;
  int B, buflen;

  if (tid < 0) {
    return -1;
  }
  if (tid >= current_tid) {
    return -2;
  }
  if (tds[tid].state != REPLY_BLOCKED) {
    return -3;
  }
  // We know the sender is waiting, to reply to them
  replybuf = tds[tid].replyq.msg;
  B = buflen = tds[tid].replyq.msglen;
  // should do it? What about replyer tid?
  int rl = replylen;
  while (buflen-- && rl--) {
    *replybuf++ = *reply++;
  }

//  bwprintf(COM2, "REPLY: Waking up %d.\r\n", tid); 
  tds[tid].state = READY;
  tds[tid].retval = replylen;

  if (B < replylen) {
    return -4;
  } 


  return 0;
}
