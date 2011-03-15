/*
 * The Track Server - a warehouse for all track related data.
 * Also acts as the reservation server.
 */
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include <lock.h>
#include <ANSI.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL, FOREACH
#include "track.h"

//#define // some macro for mapping numbers to IDs

struct visitelem {
  struct track_node *next, *source, *prev;
  int dist;
  int swdir;
  int swid;
  int i;
};

struct visitelem findnext (struct visitelem *v) {
  int i;
  int mini;
  int mindist = 100000;

  for (i = 0; i < 80; i ++) {
    if (v[i].next != 0) {
      mindist = (mindist>v[i].dist) ? v[i].dist : mindist;
      mini = (mindist==v[i].dist) ? i : mini;
    }
  }

  struct visitelem ans = v[mini];
  v[mini].next = 0;
  return ans;
}

void insertv (struct visitelem *v, struct track_node *n, int dist, struct track_node *source,int swid, int swdir, struct track_node *prev, int ii) {
  int i;
  for (i = 0; i < 80; i++) {
    if (v[i].next == 0) {
      v[i].next = n;
      v[i].dist = dist;
      v[i].source = source;
      v[i].swid = swid;
      v[i].swdir = swdir;
      v[i].prev = prev;
      v[i].i=ii;
      return;
    }
  }
}

struct track_node *findpath (struct track_node *current, struct track_node *dest,
                             struct track_node **switches) {
  //LockAcquire (COM2_W_LOCK);
  //bwprintf (COM2, "trying to go from %d to %d\n", current->abs_id, dest->abs_id);
  //LockRelease (COM2_W_LOCK);
bwputc(COM2, 'f');
  int trtid = WhoIs("tr");
  struct visitelem v[80];
  int vi = 0;
  int seen[80];
  int i;
  for (i = 0; i < 80; i++) {
    seen[i] = 0;
    v[i].next = 0;
  }
    
  for (i = 0; i < current->num_edges; i++) {
    seen[current->abs_id] = 1;
    v[i].next  = current->edges[i].dest;
    v[i].source = current->edges[i].dest;
    v[i].dist = current->edges[i].dist;
    if (current->edges[i].dest->abs_id > 40) { v[i].swid = current->edges[i].dest->id; v[i].swdir = i; } else v[i].swid = 666;
    vi++;    
  }
bwputc(COM2, 'g');
  while (vi) {
    //LockAcquire(COM2_W_LOCK);
    //bwprintf (COM2, ">> Iteration\n");
    //bwprintf (COM2, "vi = %d\nvisit list = {", vi);
    //for (i = 0; i < 80; i++) { if (v[i].next != 0) bwprintf (COM2, "%d, ", v[i].next->abs_id); }
    //bwprintf (COM2, "}\nseen = ");
    //for (i=0;i<80;i++) { bwprintf (COM2, "%d",seen[i]); }
    //bwprintf (COM2, "\n");
    //LockRelease (COM2_W_LOCK);


    struct visitelem n = findnext (v);
    if (n.next->abs_id == dest->abs_id) { // found our guy
      if (n.swid != 666) {
      LockAcquire(COM2_W_LOCK);
      bwprintf(COM2, "found our guy: swid %d dir %d\r\n", n.swid, n.swdir);
      LockRelease(COM2_W_LOCK);
        char hobomsg[3];
        hobomsg[0] = 'w';
        hobomsg[1] = n.swid;
        hobomsg[2] = n.swdir == 0 ? 'S' :
                     n.swdir == 1 ? 'S' :
                     n.swdir == 2 ? 'C' :
                     'X';
//bwputc(COM2, 'h');
        if (Send(trtid, hobomsg, 3, NULL, 0) != 0) PANIC;
//bwputc(COM2, 'i');

        // UPDATE TRACK SERVER
        //int unfudgedswitch = n.swid < 19 ? n.swid - 1 : (n.swid - 153) + 18;
        //switches[unfudgedswitch]->switch_state = hobomsg[1];
      }
//bwputc(COM2, 'j');

      //LockAcquire (COM2_W_LOCK);
      //char c[4];
      if (n.source->id < 40) {
        //sens_id_to_name(n.source->id*2, c); 
        //bwprintf (COM2, "The answer is apparently %s\n", c);
        //LockRelease (COM2_W_LOCK);
        return n.source;
       }
       else {
         //bwprintf (COM2, "next thing is apparently a switch, %d\n", n.source->id);
         //LockRelease (COM2_W_LOCK);
         return 0; /// ????
       }
      return n.source;
    } 
    vi--;
//bwputc(COM2, 'k');
    seen[n.next->abs_id] = 1;
    //LockAcquire (COM2_W_LOCK);
    //bwprintf (COM2, "Next: %d, visit count: %d\n", n.next->abs_id, vi);
    //LockRelease (COM2_W_LOCK);
    // add their new neighbours to the seen/v 
    for (i = 0; i < n.next->num_edges; i++) {
      if (!seen[n.next->edges[i].dest->abs_id]) {
bwputc(COM2, 'l');
        //LockAcquire (COM2_W_LOCK);
        //bwprintf (COM2, "Adding %d to visit list.\n", n.next->edges[i].dest->abs_id);
        //LockRelease (COM2_W_LOCK);
        vi++;
        seen[n.next->edges[i].dest->abs_id] = 1;
        if (n.swid == 666 && n.next->abs_id > 40) { n.swid = n.prev->id; n.swdir = n.i; }
        insertv (v, n.next->edges[i].dest, n.dist+n.next->edges[i].dist, n.source, n.swid, n.swdir, n.next, i);
      }
      else {
//bwputc(COM2, 'L');
        //LockAcquire (COM2_W_LOCK);
        //bwprintf (COM2, "Already saw %d\n", n.next->edges[i].dest->abs_id);
        ////LockRelease (COM2_W_LOCK);
      }
    }
   // Delay(100);
  }
  bwprintf (COM2, "none found???\n");
  return 0;
}




#define MIN(x, y) x < y ? x : y
#define INFTY 1000000
#if 0
void shortestpath(struct track_node *current, struct track_node *dest)
{
  int to_visit[80];
  int visit_dist[80];
  int visit_idx = 0;
  int curdist   = 0;
  int i, j;

  while () {
    for (i = 0; i < current->num_edges; ++i) {
      to_visit[visit_idx]       = current->edges[i];
      visit_distance[visit_idx] = current->edges[i].dist + curdist;
      ++visit_idx;
    }

    if (visit_idx == 0) {
      // no path!
      return;
    }


    asd;
    opf;
  }
}
#endif

/*
 * Dijkstra's Algorithm
 * All data should be initialized to 1000000 (infinity) when first calling this.
 *
 * DERP DERP need to do breadth-first
 */
void dijkstra(struct track_node *current, struct track_node *dest, int *data,
              int *visited, int *previous, struct path *p)
{
 /* int i;
  struct track_node *neighbour;
  struct track_node *nnode;
  int min = INFTY;
  int next;
  int direction = 8;

  LockAcquire(COM2_W_LOCK);
  bwprintf(COM2, "DIJKSTRA: c id %d abs %d  d id %d abs %d path so far %d\r\n",
           current->id, current->abs_id, dest->id, dest->abs_id, p->count);
  LockRelease(COM2_W_LOCK);
  if (current == dest) return;

  for (i = 0; i < current->num_edges; ++i) { // neighbours
    neighbour = current->edges[i].dest;
    if (visited[neighbour->abs_id]) continue;
    data[neighbour->abs_id] =
      MIN(data[neighbour->abs_id],
          data[current->abs_id] + current->edges[i].dist);
    min = MIN(min, data[neighbour->abs_id]);
    if (min == data[neighbour->abs_id]) {
      direction = i; // ahead, behind, or curved
      next = neighbour->id * 2; // an id, as userland expects
      nnode = neighbour;
      previous[neighbour->abs_id] = current->abs_id;
    }
    LockAcquire(COM2_W_LOCK);
    bwprintf(COM2, "DIJKSTRA-LOOP: i %d neighbour %d data[n] %dmm min %dmm\r\n",
             i, neighbour->abs_id, data[neighbour->abs_id], min);
    LockRelease(COM2_W_LOCK);
  }
  visited[current->abs_id] = 1;

  p->node[p->count].node = next; // kind of dumb/wrong, probably won't end up using this
  p->node[p->count].dir  = direction;
  ++p->count;
  if (p->count >= 20) {
    bwprintf(COM2, "ZAZA overflow of path\r\n");
    PANIC;
  }

  return dijkstra(nnode, dest, data, visited, previous, p);
*/}

/*
 * Takes the current sensor and reports the next expected one given the track
 * and current state of switches. The trick is to know relative directions when
 * approaching switches/sensors, so that we return the correct ID according to
 * that direction. It returns a structure giving both the distance between the
 * source and destination, and a mappable (given map) ID of the destination.
 */
struct trip next_sensor(int current, struct track_node **map)
{
  char name1[4];//, name2[4];
  struct trip t;
  struct track_node *cur = map[current];
  struct track_node *next, *prev;
  int dist = 0;

  sens_id_to_name(current, name1);
    //bwprintf(COM2, "NEXT         %d|%s\r\n", current, name1);
  //sens_id_to_name(cur->id * 2, name1);
  //  bwprintf(COM2, "NEXT  mapped %d|%s  ... edge choices:\r\n", cur->id * 2, name1);
  //sens_id_to_name(cur->edges[AHEAD].dest->id * 2, name1);
  //  bwprintf(COM2, "NEXT s id %d type %d\r\n", cur->edges[AHEAD].dest->id,
  //            cur->edges[AHEAD].dest->type);
  //sens_id_to_name(cur->edges[BEHIND].dest->id * 2, name1);
  //  bwprintf(COM2, "NEXT b id %d type %d\r\n", cur->edges[BEHIND].dest->id,
  //            cur->edges[BEHIND].dest->type);
//  sens_id_to_name(cur->edges[CURVED].dest->id * 2, name1);
//    bwprintf(COM2, "NEXT c       %d|%s\r\n", cur->edges[CURVED].dest->id * 2, name1);
  if (current % 2 == 0) {
    next =  cur->edges[AHEAD].dest;
    //sens_id_to_name(next->id * 2, name1);
    //if (next->type == SENSOR)
    //bwprintf(COM2, "NEXT first n %d|%s\r\n", next->id * 2, name1);
    //else
    //bwprintf(COM2, "NEXT first n(non sens) id %d, type %d\r\n", next->id, next->type);
    dist += cur->edges[AHEAD].dist;
  } else {
    next =  cur->edges[BEHIND].dest;
    //sens_id_to_name(next->id * 2, name1);
    //if (next->type == SENSOR)
    //bwprintf(COM2, "NEXT first n %d|%s\r\n", next->id * 2, name1);
    //else
    //bwprintf(COM2, "NEXT first n(non sens) id %d, type %d\r\n", next->id, next->type);
    dist += cur->edges[BEHIND].dist;
  }

  if (next->type == SWITCH) { // Must decide if we are ahead/behind
    prev = cur;
    while (next->type == SWITCH) {
      //bwprintf(COM2, "NEXT inside while id %d, type %d\r\n", next->id,
      //next->type);
      if (next->edges[AHEAD].dest == prev) {
        //bwprintf(COM2, "NEXT switch behind\r\n");
        dist += next->edges[BEHIND].dist;
        prev =  next;
        next =  next->edges[BEHIND].dest;
      }
      else if (next->edges[BEHIND].dest == prev) {
        if (next->switch_state == 'S' || next->switch_state == 's') {
          //bwprintf(COM2, "NEXT switch ahead\r\n");
          //bwprintf(COM2, "\r\nNEXT went over a switch in state %c\r\n",
          //         next->switch_state);
          dist += next->edges[AHEAD].dist;
          prev =  next;
          next =  next->edges[AHEAD].dest;
        }
        else {
          //bwprintf(COM2, "NEXT switch curved\r\n");
          //bwprintf(COM2, "\r\nNEXT went over a switch in state %c\r\n",
          //         next->switch_state);
          dist += next->edges[CURVED].dist;
          prev =  next;
          next =  next->edges[CURVED].dest;
        }
      }
      else { // Always go to same place when coming at switch in this direction
        //bwprintf(COM2, "NEXT switch behind\r\n");
        dist += next->edges[BEHIND].dist;
        prev =  next;
        next =  next->edges[BEHIND].dest;
      }
    }
    //sens_id_to_name(next->id * 2, name1);
    //bwprintf(COM2, "NEXT outside while  %d|%s\r\n", next->id * 2, name1);
    //bwprintf(COM2, "NEXT outside  prev  %d\r\n", prev->id);
    if (next->edges[AHEAD].dest == prev) {
      //bwprintf(COM2, "NEXT add 0\r\n");
      t.destination = (next->id * 2) + 1;
    }
    else {
      //bwprintf(COM2, "NEXT add 1\r\n");
      t.destination = (next->id * 2);
    }
  }
  else if (next->type == STOP) { // We have nowhere to go "next"
    t.distance = -1;
    t.destination = -1;
  }
  else {
    // To decide which switch direction, see if it points at cur AHEAD or
    // BEHIND
    if (next->edges[AHEAD].dest == cur) {
      //bwprintf(COM2, "NEXT ad 0\r\n");
      t.destination = (next->id * 2) + 1;
    }
    else {
      //bwprintf(COM2, "NEXT ad 1\r\n");
      t.destination = (next->id * 2);
    }
  }

  t.distance = dist;
  t.destnode = *next;
  //bwprintf(COM2, "NEXT ASSERT: next->id %d == map[dest]->id %d\r\n", next->id,
  //         map[t.destination]->id);
  // ASSERT(next->id == map[t.destionation]->id);

  return t;
}

/*
 * Return the neighbours of a sensor (ideal for finding a train that got lost
 * over a switch). There are at most 4 neighbours given our track arrangements.
 * Map must be used before calling this function
 */
void neighbour(struct track_node *cur, struct track_node *next,
               struct neighbours *ne)
{
  //bwputc(COM2, 'N');
  //bwprintf(COM2, "%d", next->id);
  if (next->type == SWITCH) { // Must decide if we are ahead/behind
    //bwputc(COM2, '1');
    if (next->edges[AHEAD].dest == cur) {
      //bwprintf(COM2, "ne6xt%d", next->edges[BEHIND].dest->id);
      neighbour(next, next->edges[BEHIND].dest, ne);
    }
    else if (next->edges[BEHIND].dest == cur) {
      //bwprintf(COM2, "ne7xt%d", next->edges[AHEAD].dest->id);
      neighbour(next, next->edges[AHEAD].dest, ne);
      //bwprintf(COM2, "ne8xt%d", next->edges[CURVED].dest->id);
      neighbour(next, next->edges[CURVED].dest, ne);
    }
    else { // Always go to same place when coming at switch in this direction
      //bwprintf(COM2, "ne9xt%d", next->edges[BEHIND].dest->id);
      neighbour(next, next->edges[BEHIND].dest, ne);
    }
  }
  else if (next->type == STOP) { // We have nowhere to go "next"
    //bwputc(COM2, '2');
    return;
  }
  else {
    //bwputc(COM2, '3');
    // To decide which switch direction, see if it points at cur AHEAD or
    // BEHIND
    if (next->edges[AHEAD].dest == cur) {
      ne->n[ne->count++] = (next->id * 2) + 1;
      //bwputc(COM2, '4');
      if (ne->count > 4) {
        bwprintf(COM2, "waaaah neighbours overflow\r\n");
      }
    }
    else {
      ne->n[ne->count++] = (next->id * 2);
      //bwputc(COM2, '5');
      if (ne->count > 4) {
        bwprintf(COM2, "waaaah neighbours overflow\r\n");
      }
    }
    //bwputc(COM2, '3');
  }

  return;
}

/*
 * Find the edge between sensors A and B (possibly including segments between
 * switches) to report the distance between them.
 */
#if 0
int distance(int a, int b, struct track_node *map[])
{
  int distance;

  next = next_sensor(a, map);

  return
}
#endif /* comment */

/*
 * Simply print the reservations:
 * fit into a 11 x 4 rows table
 * 01234567 1234567 1234567
 * -----------------------
 * SEX #\t SEX #\t SEX #\t SEX #\n
 * SEX #\t SEX #\t SEX #\n
 * SEX #\t SEX #\t SEX #\n
 * ...
 * -----------------------
 *  TODO
 * - I should probably make this print only the reservations of the trains,
 * not the status of each sensor...
 * - Use train numbers, not TIDs
 */
void print_reservations(int *r)
{
  LockAcquire(COM2_W_LOCK);
  CURSORPUSH();
  CURSORMOVE(2,0); // ugh, may need to make this one less column
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A01 %d\tB07 %d\tC13 %d\tE03 %d\r\n", r[0],  r[11], r[22], r[33]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A03 %d\tB09 %d\tC15 %d\tE05 %d\r\n", r[1],  r[12], r[23], r[34]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A05 %d\tB11 %d\tD01 %d\tE07 %d\r\n", r[2],  r[13], r[24], r[35]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A07 %d\tB13 %d\tD03 %d\tE09 %d\r\n", r[3],  r[14], r[25], r[36]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A09 %d\tB15 %d\tD05 %d\tE11 %d\r\n", r[4],  r[15], r[26], r[37]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A11 %d\tC01 %d\tD07 %d\tE13 %d\r\n", r[5],  r[16], r[27], r[38]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A13 %d\tC03 %d\tD09 %d\tE15 %d\r\n", r[6],  r[17], r[28], r[39]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "A15 %d\tC05 %d\tD11 %d\r\n",         r[7],  r[18], r[29]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "B01 %d\tC07 %d\tD13 %d\r\n",         r[8],  r[19], r[30]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "B03 %d\tC09 %d\tD15 %d\r\n",         r[9],  r[20], r[31]);
  bwputstr(COM2, "                               ");
  bwputstr(COM2, "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
  bwprintf(COM2, "B05 %d\tC11 %d\tE01 %d\r\n",         r[10], r[21], r[32]);
  CURSORPOP();
  LockRelease(COM2_W_LOCK);
}

/*
 * PROTOCOL:
 * next:       "n####"  next switch after us?
 * turnout:    "t#[SC]" new turnout state -- or use a struct?
 * dist:       "d####"  distance to next sensor
 * reserve:    "r####"  reserve a section of track
 * release:    "f####"  release this TID's reservations
 * neighbours: "N####"  find all possible neighbours of a sensor
 * Path:       "Pd1|d2" use Dijkstra's Algorithm for d1 to d2
 * Res?:       "R####"  ask for train ##'s reservations
 */
void track()
{
# include "track_data.c"
  struct track_node *sens_num_to_node[] = {
    &aA1, &aA1, &aA3, &aA3, &aA5, &aA5, &aA7, &aA7, &aA9, &aA9, &aA11, &aA11,
    &aA13, &aA13, &aA15, &aA15,
    &aB1, &aB1, &aB3, &aB3, &aB5, &aB5, &aB7, &aB7, &aB9, &aB9, &aB11, &aB11,
    &aB13, &aB13, &aB15, &aB15,
    &aC1, &aC1, &aC3, &aC3, &aC5, &aC5, &aC7, &aC7, &aC9, &aC9, &aC11, &aC11,
    &aC13, &aC13, &aC15, &aC15,
    &aD1, &aD1, &aD3, &aD3, &aD5, &aD5, &aD7, &aD7, &aD9, &aD9, &aD11, &aD11,
    &aD13, &aD13, &aD15, &aD15,
    &aE1, &aE1, &aE3, &aE3, &aE5, &aE5, &aE7, &aE7, &aE9, &aE9, &aE11, &aE11,
    &aE13, &aE13, &aE15, &aE15
  };
  struct track_node *switches[] = {
    &aSW1,  &aSW2,  &aSW3,  &aSW4,  &aSW5,  &aSW6,  &aSW7,  &aSW8,  &aSW9,
    &aSW10, &aSW11, &aSW12, &aSW13, &aSW14, &aSW15, &aSW16, &aSW17, &aSW18,
    &aSW99, &aSW9A, &aSW9B, &aSW9C
  };
  int reservations[40]; // one per physical sensor
  int pdata[80];        // graph node distance values
  int pvisited[80];     // have we visited the graph node?
  int pprevious[80];    // shortest path in reverse direction
  int ureserves[800];     // to tell the user (train server) about their reservations
  int *ures = ureserves;
  struct trip t;
  struct neighbours n;
  struct msg m;
  struct path path;
  int i, r, tid;
  int cnt;
  char c;
  //char sname[4];

  RegisterAs("trak");
  FOREACH(i, 40) reservations[i] = 0;

  FOREVER {
    FOREACH(i, 80) { pdata[i] = INFTY; pvisited[i] = 0; pprevious[i] = -1; }
    path.count = 0;
    FOREACH(i, 20) { path.node[i].dir = 0; path.node[i].node = -1; }

    r = Receive(&tid, (char *)&m, sizeof(struct msg));

    switch (m.id) {
      case 'n':
        t = next_sensor(m.d1, sens_num_to_node);
        r = Reply(tid, (char *)&t, sizeof(struct trip));
        break;
      case 'd':
        t = next_sensor(m.d1, sens_num_to_node);
        r = Reply(tid, (char *)(&t.distance), 4);
      case 't':
        r = Reply(tid, NULL, 0);
        //bwprintf(COM2, "\r\nTurnout %d updated to %c\r\n", m.d1, m.c1);
        i = m.d1;
        if (i >= sizeof(switches)) {
          DPRINTERR("Overflow of switch ID number\r\n");
        }
        else {
          switches[i]->switch_state = m.c1; // lol overflow
        }
        break;
      case 'r':
        //sens_id_to_name(m.d1, sname);
        //LockAcquire(COM2_W_LOCK);
        //bwprintf(COM2, "Task %d asking for %s, current owner: %d\r\n", tid,
        //         sname, reservations[m.d1 / 2]);
        //LockRelease(COM2_W_LOCK);
        // fold sensor IDs of same sensors into one reservation slot, so / 2
        if (reservations[m.d1 / 2] == 0) {
          reservations[m.d1 / 2] = tid;
          r = Reply(tid, NULL, 0); // successfully reserved
        }
        else {
          r = Reply(tid, &c, 1); // couldn't reserve
        }
        //print_reservations(reservations);
        break;
      case 'f':
        FOREACH(i, 72) {
          if (reservations[i] == tid) {
            reservations[i] = 0;
          }
        }
        r = Reply(tid, NULL, 0);
        //print_reservations(reservations);
        break;
      case 'N':
        n.count = 0; n.n[0] = n.n[1] = n.n[2] = n.n[3] = 0;
        if (m.d1 % 2 == 0) {
          neighbour(sens_num_to_node[m.d1],
                    sens_num_to_node[m.d1]->edges[AHEAD].dest, &n);
        } else {
          neighbour(sens_num_to_node[m.d1],
                    sens_num_to_node[m.d1]->edges[BEHIND].dest, &n);
        }
        r = Reply(tid, (char *)&n, sizeof(struct neighbours));
        break;
      case 'P':
        LockAcquire (COM2_W_LOCK);
        bwprintf (COM2, "whaT?\n");
  LockRelease (COM2_W_LOCK);
        //pdata[sens_num_to_node[m.d1]->abs_id] = 0; // set the initial node's dist to 0
        findpath (sens_num_to_node[m.d1], sens_num_to_node[m.d2], switches);
        // now fixup the path
        r = Reply(tid, NULL, 0);
        break;
      case 'R':
        cnt = 0;
        FOREACH(i, 800) { ureserves[i] = -1; }
        FOREACH(i, 72) {
          if (reservations[i] == m.d1) {
            ureserves[(m.d1 * 10) + cnt++] = i;
          }
        }
        r = Reply(tid, (char *)&ures, 4);
        break;
      case '~':
bwputc(COM2, 'e');
        findpath(sens_num_to_node[m.d1], sens_num_to_node[m.d2], switches);
        r = Reply(tid, NULL, 0);
      default:
        PANIC;
    }
  }
}
