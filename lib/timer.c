#include <zebra.h>
#include <sys/time.h>
#include <time.h>
#include "thread.h"
#include "timer.h"


/* Recent absolute time of day */
struct timeval recent_time;
struct timeval last_recent_time;

/* Relative time, since startup */
struct timeval relative_time;
struct timeval relative_time_base;

static unsigned short timers_inited; /* init flag */

#define TIMER_SECOND_MICRO 1000000L  /* 微秒 */


/* 两个时间比较 */
long timeval_cmp (struct timeval a, struct timeval b)
{
    return (a.tv_sec == b.tv_sec
	        ? a.tv_usec - b.tv_usec : a.tv_sec - b.tv_sec);
}


/* 两个时间相减 */
struct timeval timeval_subtract (struct timeval a, struct timeval b)
{
    struct timeval ret;

    ret.tv_usec = a.tv_usec - b.tv_usec;
    ret.tv_sec = a.tv_sec - b.tv_sec;

    return timeval_adjust (ret);
}

unsigned long timeval_elapsed (struct timeval a, struct timeval b)
{
    return (((a.tv_sec - b.tv_sec) * TIMER_SECOND_MICRO)
	        + (a.tv_usec - b.tv_usec));
}


/* Adjust so that tv_usec is in the range [0,TIMER_SECOND_MICRO).
   And change negative values to 0. */
struct timeval timeval_adjust (struct timeval a)
{
    while (a.tv_usec >= TIMER_SECOND_MICRO)
    {
        a.tv_usec -= TIMER_SECOND_MICRO;
        a.tv_sec++;
    }

    while (a.tv_usec < 0)
    {
        a.tv_usec += TIMER_SECOND_MICRO;
        a.tv_sec--;
    }

	/* Change negative timeouts to 0. */
    if (a.tv_sec < 0)        
        a.tv_sec = a.tv_usec = 0; 

    return a;
}



#if !defined(HAVE_CLOCK_MONOTONIC) && !defined(__APPLE__)
static void time_adjust_absolute_time (void)
{
    struct timeval diff;
    if (timeval_cmp (recent_time, last_recent_time) < 0)
    {
        relative_time.tv_sec++;
        relative_time.tv_usec = 0;
    }
    else
    {
        diff = timeval_subtract (recent_time, last_recent_time);
        relative_time.tv_sec += diff.tv_sec;
        relative_time.tv_usec += diff.tv_usec;
        relative_time = timeval_adjust (relative_time);
    }
    
    last_recent_time = recent_time;
}
#endif /* !HAVE_CLOCK_MONOTONIC && !__APPLE__ */


/* gettimeofday wrapper, to keep recent_time updated */
int time_get_absolute_time (struct timeval *tv)
{
  int ret;
  
  assert (tv);
  
    if (!(ret = gettimeofday (&recent_time, NULL)))
    {
      /* init... */
        if (!timers_inited)
        {
            relative_time_base = last_recent_time = recent_time;
            timers_inited = 1;
        }
        /* avoid copy if user passed recent_time pointer.. */
        if (tv != &recent_time)
            *tv = recent_time;
        return 0;
    }
    
    return ret;
}

int time_get_relative_time (struct timeval *tv)
{
    int ret;

#ifdef HAVE_CLOCK_MONOTONIC
  {
    struct timespec tp;
    if (!(ret = clock_gettime (CLOCK_MONOTONIC, &tp)))
      {
        relative_time.tv_sec = tp.tv_sec;
        relative_time.tv_usec = tp.tv_nsec / 1000;
      }
  }
#elif defined(__APPLE__)
  {
    uint64_t ticks;
    uint64_t useconds;
    static mach_timebase_info_data_t timebase_info;

    ticks = mach_absolute_time();
    if (timebase_info.denom == 0)
      mach_timebase_info(&timebase_info);

    useconds = ticks * timebase_info.numer / timebase_info.denom / 1000;
    relative_time.tv_sec = useconds / 1000000;
    relative_time.tv_usec = useconds % 1000000;

    return 0;
  }
#else /* !HAVE_CLOCK_MONOTONIC && !__APPLE__ */
  if (!(ret = time_get_absolute_time (&recent_time)))
      time_adjust_absolute_time();
#endif /* HAVE_CLOCK_MONOTONIC */

  if (tv)
    *tv = relative_time;

  return ret;
}


/* Get absolute time stamp, but in terms of the internal timer
 * Could be wrong, but at least won't go back.
 */
static void time_real_stabilised (struct timeval *tv)
{
    *tv = relative_time_base;
    tv->tv_sec += relative_time.tv_sec;
    tv->tv_usec += relative_time.tv_usec;
    *tv = timeval_adjust (*tv);
}


/* Exported Quagga timestamp function.
 * Modelled on POSIX clock_gettime.
 */
int time_get_time (enum TIME_CLOCK clkid, struct timeval *tv)
{
    switch (clkid)
    {
      case TIME_CLK_REALTIME:             /* 绝对时间*/
        return time_get_absolute_time (tv);
      case TIME_CLK_MONOTONIC:            /* 未校准的相对时间*/
        return time_get_relative_time (tv);
      case TIME_CLK_REALTIME_STABILISED:  /* 校准的相对时间*/
        time_real_stabilised (tv);
        return 0;
      default:
        errno = EINVAL;
        return -1;
    }
	
    return 0;
}


/* 获取校准的相对秒时间 */
time_t time_get_real_time (time_t *t)
{
    struct timeval tv;
	
    time_real_stabilised (&tv);
	
    if (t)
        *t = tv.tv_sec;
	
    return tv.tv_sec;
}


/* 获取未校准的相对时间 */
struct timeval time_get_recent_relative_time (void)
{
    return relative_time;
}


