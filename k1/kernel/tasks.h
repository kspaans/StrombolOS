#ifndef TASKS__INCLUDED
#define TASKS__INCLUDED
#include "switch.h"

struct taskq {
  struct td *p[NUMPRIO];
  struct td *head[NUMPRIO];
};

struct td *schedule (struct td *cur, struct taskq *tasks);
void inittasks (struct taskq *q);
void addtask (struct td *nt, int p, struct taskq *tasks);
#endif
