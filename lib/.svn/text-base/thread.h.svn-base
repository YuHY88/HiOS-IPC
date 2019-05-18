/* Thread management routine header.
 * Copyright (C) 1998 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  
 */

#ifndef _ZEBRA_THREAD_H
#define _ZEBRA_THREAD_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <lib/pqueue.h>
#ifdef TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif /* TIME_WITH_SYS_TIME */

#include <sys/utsname.h>
#ifdef HAVE_RUSAGE
#include <sys/resource.h>
#endif /* HAVE_RUSAGE */

#include "timer.h"
#include "hptimer.h"


#define GETRUSAGE(X) thread_getrusage(X)


/* Linked list of thread. */
struct thread_list
{
  struct thread *head;
  struct thread *tail;
  int count;
};


struct pqueue;


/* Master of the theads. */
struct thread_master
{
  struct thread_list read;
  struct thread_list write;
  struct pqueue *timer;
  struct thread_list event;
  struct thread_list event_normal;
  struct thread_list ready;
  struct thread_list unuse;
  struct pqueue *background;
  fd_set readfd;
  fd_set writefd;
  fd_set exceptfd;
  unsigned long alloc;
  int (*func)(const char *, struct thread *);
  int (*connect)(void *);
  void (*config)(void *);
};


typedef unsigned char thread_type;


/* Thread itself. */
struct thread
{
  thread_type type;		/* thread type */
  thread_type add_type;		/* thread type */
  struct thread *next;		/* next pointer of the thread */   
  struct thread *prev;		/* previous pointer of the thread */
  struct thread_master *master;	/* pointer to the struct thread_master. */
  int (*func) (struct thread *); /* event function */
  void *arg;			/* event argument */
  union {
    int val;			/* second argument of the event. */
    int fd;			/* file descriptor in case of read/write. */
    struct timeval sands;	/* rest of time sands value. */
  } u;
  int index;			/* used for timers to store position in queue */
  struct timeval real;
  struct cpu_thread_history *hist; /* cache pointer to cpu_history */
  const char *funcname;
  const char *schedfrom;
  int schedfrom_line;
  int (*cmd_excute)(const char *, struct thread *);
   void (*config_finish)(void *);
};


struct cpu_thread_history 
{
  int (*func)(struct thread *);
  unsigned int total_calls;
  struct time_stats
  {
    unsigned long total, max;
  } real;
#ifdef HAVE_RUSAGE
  struct time_stats cpu;
#endif
  thread_type types;
  const char *funcname;
};


/* Thread types. */
#define THREAD_READ           0
#define THREAD_WRITE          1
#define THREAD_TIMER          2
#define THREAD_EVENT          3
#define THREAD_EVENT_NORMAL	  4
#define THREAD_READY          5
#define THREAD_BACKGROUND     6
#define THREAD_UNUSED         7
#define THREAD_EXECUTE        8


/* Thread yield time.  */
#define THREAD_YIELD_TIME_SLOT     10 * 1000L /* 10ms */

/* Macros. */
#define THREAD_ARG(X) ((X)->arg)
#define THREAD_FD(X)  ((X)->u.fd)
#define THREAD_VAL(X) ((X)->u.val)

#define THREAD_READ_ON(master,thread,func,arg,sock) \
  do { \
    if (! thread) \
      thread = thread_add_read (master, func, arg, sock); \
  } while (0)

#define THREAD_WRITE_ON(master,thread,func,arg,sock) \
  do { \
    if (! thread) \
      thread = thread_add_write (master, func, arg, sock); \
  } while (0)

#define THREAD_TIMER_ON(master,thread,func,arg,time) \
  do { \
    if (! thread) \
      thread = thread_add_timer (master, func, arg, time); \
  } while (0)

#define THREAD_TIMER_MSEC_ON(master,thread,func,arg,time) \
  do { \
    if (! thread) \
      thread = thread_add_timer_msec (master, func, arg, time); \
  } while (0)

#define THREAD_OFF(thread) \
  do { \
    if (thread) \
      { \
        thread_cancel (thread); \
        thread = NULL; \
      } \
  } while (0)

#define THREAD_READ_OFF(thread)  THREAD_OFF(thread)
#define THREAD_WRITE_OFF(thread)  THREAD_OFF(thread)
#define THREAD_TIMER_OFF(thread)  THREAD_OFF(thread)

#define debugargdef  const char *funcname, const char *schedfrom, int fromln

#define thread_add_read(m,f,a,v) funcname_thread_add_read(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_add_write(m,f,a,v) funcname_thread_add_write(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_add_timer(m,f,a,v) funcname_thread_add_timer(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_add_timer_msec(m,f,a,v) funcname_thread_add_timer_msec(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_add_event(m,f,a,v) funcname_thread_add_event(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_execute(m,f,a,v) funcname_thread_execute(m,f,a,v,#f,__FILE__,__LINE__)
#define thread_add_event_normal(m,f,a,v) funcname_thread_add_event_normal(m,f,a,v,#f,__FILE__,__LINE__)
#if 0   /* not use normal event suxq 20190324*/
#define thread_add_background(m,f,a,v) funcname_thread_add_background(m,f,a,v,#f,__FILE__,__LINE__)
#endif

/* The 4th arg to thread_add_background is the # of milliseconds to delay. */


/* Prototypes. */
extern struct thread_master *thread_master_create (void);
extern void thread_master_free (struct thread_master *);

extern struct thread *funcname_thread_add_read (struct thread_master *, 
				                int (*)(struct thread *),
				                void *, int, debugargdef);
extern struct thread *funcname_thread_add_write (struct thread_master *,
				                 int (*)(struct thread *),
				                 void *, int, debugargdef);
extern struct thread *funcname_thread_add_timer (struct thread_master *,
				                 int (*)(struct thread *),
				                 void *, long, debugargdef);
extern struct thread *funcname_thread_add_timer_msec (struct thread_master *,
				                      int (*)(struct thread *),
				                      void *, long, debugargdef);
extern struct thread *funcname_thread_add_event (struct thread_master *,
				                 int (*)(struct thread *),
				                 void *, int, debugargdef);
extern struct thread *funcname_thread_add_event_normal (struct thread_master *,
				                 int (*)(struct thread *),
				                 void *, int, debugargdef);
extern struct thread *funcname_thread_add_background (struct thread_master *,
                                               int (*func)(struct thread *),
				               void *arg,
				               long milliseconds_to_delay,
					       debugargdef);
extern struct thread *funcname_thread_execute (struct thread_master *,
                                               int (*)(struct thread *),
                                               void *, int, debugargdef);
extern struct thread *funcname_thread_execute_normal (struct thread_master *,
                                               int (*)(struct thread *),
                                               void *, int, debugargdef);
#undef debugargdef

extern void thread_cancel (struct thread *);
extern unsigned int thread_cancel_event (struct thread_master *, void *);
#if 0   /* close by suxq 20190325*/
extern unsigned int thread_cancel_event_normal (struct thread_master *, void *);
#endif
extern struct thread *thread_fetch (struct thread_master *, struct thread *);
extern void thread_call (struct thread *);
#if 0 /* close by suxq 20190325*/
extern unsigned long thread_timer_remain_second (struct thread *);
extern struct timeval thread_timer_remain(struct thread*);
#endif
extern int thread_timer_cmp(void *a, void *b);
extern int thread_should_yield (struct thread *);

/* Internal libzebra exports */
extern void thread_getrusage (RUSAGE_T *);
extern struct cmd_element show_thread_cpu_cmd;
extern struct cmd_element clear_thread_cpu_cmd;

/* Returns elapsed real (wall clock) time. */
extern unsigned long thread_consumed_time(RUSAGE_T *after, RUSAGE_T *before,
					  unsigned long *cpu_time_elapsed);

/* only for use in logging functions! */
extern struct thread *thread_current;

#endif /* _ZEBRA_THREAD_H */
