/*
*   time define
*/

#ifndef _ZEBRA_LIBTIME_H
#define _ZEBRA_LIBTIME_H


/* 1970.1.1 »’ ±º‰ */
#define TIME_JAN_1970            0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */


/* Clocks type */
enum TIME_CLOCK 
{
    TIME_CLK_REALTIME = 0,	    /* ala gettimeofday() */
    TIME_CLK_MONOTONIC,		    /* monotonic, against an indeterminate base */
    TIME_CLK_REALTIME_STABILISED, /* like realtime, but non-decrementing */
};


struct rusage_t
{
#ifdef HAVE_RUSAGE
  struct rusage cpu;
#endif
  struct timeval real;
};
#define RUSAGE_T        struct rusage_t


/* Global variable containing a recent result from gettimeofday.  This can
   be used instead of calling gettimeofday if a recent value is sufficient.
   This is guaranteed to be refreshed before a thread is called. */
extern struct timeval recent_time;

extern struct timeval timeval_adjust (struct timeval a);
extern unsigned long timeval_elapsed (struct timeval a, struct timeval b);
extern struct timeval timeval_subtract (struct timeval a, struct timeval b);
extern long timeval_cmp (struct timeval a, struct timeval b);

/* replacements for the system gettimeofday(), clock_gettime() and
 * time() functions, providing support for non-decrementing clock on
 * all systems, and fully monotonic on /some/ systems.
 */
extern int time_get_time (enum TIME_CLOCK, struct timeval *);
extern time_t time_get_real_time (time_t *);
extern int time_get_relative_time (struct timeval *tv);
extern int time_get_absolute_time (struct timeval *tv);
/* Similar to recent_time, but a monotonically increasing time value */
extern struct timeval time_get_recent_relative_time (void);

extern struct timeval last_recent_time;
extern struct timeval relative_time;


#endif 

