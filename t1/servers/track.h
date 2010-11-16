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
  int id; // From the track data file
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
