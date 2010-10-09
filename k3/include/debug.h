#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * Special debugging macros, will be saved to the git repo as "off", but
 * define DEBUG to turn them on
 */

#define DEBUG 0

// Nice lines of the form "file.c:## blah blah" -- but remember to include the
//  appropriate format string
#define DPRINT(...) if (DEBUG) {\
  bwprintf(COM2, "%s:%d: ", __FILE__, __LINE__); \
  bwprintf(COM2, __VA_ARGS__); }

// A more verbose debug, in case we REALLY want it
#define VERBOSE_DEBUG_MASK 0x2
#define VDPRINT(...) if (DEBUG & VERBOSE_DEBUG_MASK) {\
  bwprintf(COM2, "%s:%d: ", __FILE__, __LINE__); \
  bwprintf(COM2, __VA_ARGS__); }

#endif/*__DEBUG_H__*/
