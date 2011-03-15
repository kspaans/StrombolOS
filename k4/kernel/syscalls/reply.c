/*
1 - Check if the TID exists
2 - look at the blocked list for send-blocked tasks, for one with TID
  2A - If such a task does not exist, return -3
3 - Copy the reply into the sender's buffer
4 - set status to READY, but make sure that if caller is same priority as sender, that sender gets to go first
*/

#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "../switch.h"
#include "ksyscall.h"

int _kReply(struct td *mytd, int tid, char *reply, int replylen,
            struct td *tds, int current_tid)
{
  char *replybuf;
  int B, buflen;

  DPRINT(">>> Entered with mytd 0x%x, tid %d, reply(0x%x)\'%s\', replylen %d, t"
         "ds 0x%x, current_tid %d\r\n", mytd, tid, reply, reply, replylen, tds,
         current_tid);

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
  //DPRINTOK("<<<REPLY>>>Using reply queue: 0x%x, msg 0x%x\r\n", &tds[tid].replyq,
  //         tds[tid].replyq.msg);
  replybuf = tds[tid].replyq.msg;
  B = buflen = tds[tid].replyq.msglen;
  // should do it? What about replyer tid?
  int rl = replylen;
  //DPRINTOK("<<<REPLY>>>buflen: 0x%x, replylen 0x%x\r\n", buflen, rl);
  while (buflen-- && rl--) {
    *replybuf++ = *reply++;
  }

  tds[tid].state = READY;
  tds[tid].trap.r0 = replylen;

  if (B < replylen) {
    DPRINT("<<< returning ERROR -4: %d < %d\r\n", B, replylen);
    return -4;
  } 

  DPRINT("<<< returning\r\n");
  return 0;
}
