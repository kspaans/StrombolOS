/*
 * Structures for describing the train tracks
 * Yay for mutually-recursive structures
 */

struct track_node;
struct track_edge;

struct track_edge {
  int dist;
  struct track_node *dest;
};

#define EDGES 3
enum edge_type {AHEAD, BEHIND, CURVED};

struct track_node {
  enum { SENSOR, SWITCH, STOP } type;
  int id;     // From the track data file
  int abs_id; // globally unique id
  char switch_state;
  int x;
  int y;
  int num_edges;
  struct track_edge edges[EDGES];
};

/* Like a track_edge, but has id of next node */
struct trip {
  int distance;
  int destination;
  struct track_node destnode;
};

struct path_part {
  int type; // sensor, switch, or dead-end? see enum {} type in track_node
  int node; // id, either sensor ID like user-land expects or switch num
  int dir;  // for switches, what direction they should be in
};

struct path {
  int count;
  struct path_part node[20];
};

struct msg { // for IPC
  char id, c1, c2, c3;
  int d1, d2,d3,d4; 
};

struct neighbours {
  int count;
  int n[4]; // This should never overflow
};
