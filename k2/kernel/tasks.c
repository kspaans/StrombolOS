#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "tasks.h"

struct td *findtask (struct td *head, struct td *loop) {
  int num_blocked = 0;

  if (head == 0) return 0;

  struct td *i = head->next;
  struct td *fst = i;
  do {
    if (i==0) i = loop;
    if (i->state == SEND_BLOCKED ||
        i->state == REPLY_BLOCKED ||
	i->state == RECEIVE_BLOCKED)
      ++num_blocked;
    if (i->state == READY) {
      bwprintf(COM2, "SCHEDULER: %d blocked tasks\r\n", num_blocked);
      return i;
      }
    i = i->next;
  } while (i != fst);
  //bwprintf(COM2, "\r\nSCHEDULER: %d blocked tasks\r\n", num_blocked);
  return 0; // no ready task in this queue
}

void inittasks (struct taskq *q) {
  int i;
  for (i = 0; i < NUMPRIO; i++) 
    q->p[i] = q->head[i] = 0;
}

void addtask (struct td *nt, struct taskq *tasks) {
  nt->next = tasks->head[nt->priority];
  tasks->head[nt->priority] = nt;
  if (!tasks->p[nt->priority]) tasks->p[nt->priority] = nt;
}

struct td *schedule (struct td *cur, struct taskq *tasks) {
  struct td *nxt;
  int i;
  if (cur->state == ACTIVE) cur->state = READY;
  for (i = 0; i < NUMPRIO; i++) {
    nxt = findtask (tasks->p[i], tasks->head[i]);
    if (nxt) {
      nxt->state = ACTIVE;
      tasks->p[i] = nxt;
      return nxt;
    }
  }
  return 0; // NO TASKS LEFT
}

