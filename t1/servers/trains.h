#ifndef __TRAINS_H__
#define __TRAINS_H__

struct sensorevent {
  char group;
  int id;
  int time;
};

/* To indicate that we want to stop, or maybe later perform an action when we
 * encounter this turn_out.
 */
struct turn_out_trap {
  char send_id_byte;
  struct sensorevent sens;
  char action;
};

#endif/*__TRAINS_H__*/
