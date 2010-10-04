#include <bwio.h>
#include <ts7200.h>
#include "switch.h"
#include "tasks.h"

struct td *findtask (struct td *head, struct td *loop) {
  if (head == 0) return 0;

  struct td *i = head->next;
  struct td *fst = i;
  do {
    if (i==0) i = loop;
    if (i->state == READY) 
      return i;
    i = i->next;
  } while (i != fst);
  return 0; // no ready task in this queue
}

void inittasks (struct taskq *q) {
  int i;
  for (i = 0; i < NUMPRIO; i++) 
    q->p[i] = q->head[i] = 0;
}

void addtask (struct td *nt, int p, struct taskq *tasks) {
  nt->next = tasks->head[p];
  tasks->head[p] = nt;
  if (!tasks->p[p]) tasks->p[p] = nt;
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

