/*
 * The Track Server - a warehouse for all track related data.
 */
#include <bwio.h>
#include <ts7200.h>
#include <debug.h>
#include "servers.h"
#include "../user/usyscall.h"
#include "../user/lib.h"
#include "../ktests/tests.h"  // PANIC
#include "../kernel/switch.h" // FOREVER, NULL, FOREACH
#include "track.h"

//#define // some macro for mapping numbers to IDs

/*
 * Takes the current sensor and reports the next expected one given the track
 * and current state of switches. The trick is to know relative directions when
 * approaching switches/sensors, so that we return the correct ID according to
 * that direction. It returns a structure giving both the distance between the
 * source and destination, and a mappable (given map) ID of the destination.
 */
struct trip next_sensor(int current, struct track_node **map)
{
  char name1[4], name2[4];
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
 * PROTOCOL:
 * next:    "n####"  next switch after us?
 * turnout: "t#[SC]" new turnout state -- or use a struct?
 * dist:    "d####"  distance to next sensor
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
  struct trip t;
  struct msg m;
  int i, r, tid;

  RegisterAs("trak");

  FOREVER {
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
        bwprintf(COM2, "\r\nTurnout %d updated to %c\r\n", m.d1, m.c1);
        i = m.d1;
        if (i >= sizeof(switches)) {
          DPRINTERR("Overflow of switch ID number\r\n");
        }
        else {
          switches[i]->switch_state = m.c1; // lol overflow
        }
        break;
      default:
        PANIC;
    }
  }
}
