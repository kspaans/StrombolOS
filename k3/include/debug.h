#ifndef __DEBUG_H__
#define __DEBUG_H__

/*
 * Special debugging macros, will be saved to the git repo as "off", but
 * define DEBUG to turn them on
 */

#define DEBUG 0

// Nice lines of the form "file.c:## blah blah" -- but remember to include the
//  appropriate format string
#if DEBUG == 1
#  define DPRINT(...) { \
    bwprintf(COM2, "%s:%d: ", __FILE__, __LINE__); \
    bwprintf(COM2, __VA_ARGS__); }
#else
#  define DPRINT(...)
#endif

// A more verbose debug, in case we REALLY want it
#if DEBUG == 2
#  define VDPRINT(...) { \
    bwprintf(COM2, "%s:%d: ", __FILE__, __LINE__); \
    bwprintf(COM2, __VA_ARGS__); }
#else
#  define VDPRINT(...)
#endif

#define DPRINTOK(...) { bwprintf (COM2, "[ [32m:)[m ] "); bwprintf (COM2,  __VA_ARGS__); }
#define DPRINTERR(...) { bwprintf (COM2, "[ [31m:([m ] "); bwprintf (COM2,  __VA_ARGS__); }
#define DPRINTFUNC(...){ bwprintf (COM2, "In function [90m"); bwprintf (COM2, __VA_ARGS__); bwprintf (COM2, "[m.\n"); } 
#endif/*__DEBUG_H__*/
