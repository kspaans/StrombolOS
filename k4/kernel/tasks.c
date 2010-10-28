#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
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
        i->state == RECEIVE_BLOCKED ||
        i->state == EVENT_BLOCKED)
      ++num_blocked;
    if (i->state == READY)
      return i;
    i = i->next;
  } while (i != fst);

  VDPRINT("%d tasks in blocked state\r\n", num_blocked);

  if (num_blocked == 0) {
    return (void *)-1; // No tasks at all
  }
  else {
    return 0; // no ready task in this queue
  }
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
  int flag_usertasksexist = 0; // Ugh, I don't like flag variables could probably
                               // restructure the scheduler better
  if (cur->state == ACTIVE) cur->state = READY;
  for (i = 0; i < NUMPRIO; i++) {
    nxt = findtask (tasks->p[i], tasks->head[i]);
    //if (nxt == (void *)-1) DPRINTERR("NO TASKS IN PRIORITY QUEUE %d\r\n", i);
    if (nxt != 0 && nxt != (void *)-1) {
      nxt->state = ACTIVE;
      tasks->p[i] = nxt;
      if (i == NUMPRIO - 1 && flag_usertasksexist == 0) {
        DPRINTOK("All tasks have exited.\r\n");
        return 0;
      }
      else {
        //DPRINTERR("Running IDLE task: i = %d, flag = %d\r\n", i,
        //          flag_usertasksexist);
        return nxt;
      }
    }
    if (nxt != (void *)-1 && i > 2) {
      flag_usertasksexist = 1;
    }
  }
  return 0; // NO TASKS LEFT
}
