#if 0
struct calibration {
  int forteen;  /* cm/s */
  int ten;      /* cm/s */
  int eight;    /* cm/s */
  int stopdist; /* cm */
  /* Extras? */
};
enum speed {EIGHT = 0; TEN = 1; FORTEEN = 2; STOPDIST = 3;}

int def[4];
int t12[4];
int t22[4];
int t23[4];
int t32[4];
int t52[4];

def[FORTEEN] = 36;  135
def[TEN]     = 19;  60
def[EIGHT]   = 14;  35

t12[FORTEEN] = 36;
t12[TEN]     = 19;
t12[EIGHT]   = 14;

t22[FORTEEN] = 36;
t22[TEN]     = 19;
t22[EIGHT]   = 14;

t23[FORTEEN] = 36;
t23[TEN]     = 19;
t23[EIGHT]   = 14;

t32[FORTEEN] = 35;  135
t32[TEN]     = 18;  60
t32[EIGHT]   = 12;  35

t52[FORTEEN] = 36;  135
t52[TEN]     = 19;  60
t52[EIGHT]   = 14;  35
#endif

/* All speeds in mm/s */

#define DEFAULT_08 140
#define DEFAULT_10 190
#define DEFAULT_14 360

calibrations[0][0]  = 0;
calibrations[0][8]  = DEFAULT_08;
calibrations[0][10] = DEFAULT_10;
calibrations[0][14] = DEFAULT_14;

calibrations[1][0]  = 0;
calibrations[1][8]  = DEFAULT_08;
calibrations[1][10] = DEFAULT_10;
calibrations[1][14] = DEFAULT_14;

calibrations[2][0]  = 0;
calibrations[2][8]  = DEFAULT_08;
calibrations[2][10] = DEFAULT_10;
calibrations[2][14] = DEFAULT_14;

calibrations[3][0]  = 0;
calibrations[3][8]  = 120;
calibrations[3][10] = 180;
calibrations[3][14] = 350;

calibrations[4][0]  = 0;
calibrations[4][8]  = 140;
calibrations[4][10] = 190;
calibrations[4][14] = 360;

