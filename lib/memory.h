/* Memory management routine
   Copyright (C) 1998 Kunihiro Ishiguro

This file is part of GNU Zebra.

GNU Zebra is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Zebra is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Zebra; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
02111-1307, USA.  */

#ifndef _ZEBRA_MEMORY_H
#define _ZEBRA_MEMORY_H

#include <lib/memtypes.h>
#include <string.h>


#define array_size(ar) (sizeof(ar) / sizeof(ar[0]))

/* For pretty printing of memory allocate information. */
struct memory_list
{
  int index;
  const char *format;
};

struct mlist {
  struct memory_list *list;
  const char *name;
};
 
#ifdef MEMORY_LOG
 struct mstat_t
 {
   const char *name;
   long alloc;
   unsigned long t_malloc;
   unsigned long c_malloc;
   unsigned long t_calloc;
   unsigned long c_calloc;
   unsigned long t_realloc;
   unsigned long t_free;
   unsigned long c_strdup;
 };
#else
 struct mstat_t
 {
   char *name;
   long alloc;
 };
#endif /* MEMORY_LOG */

extern struct mlist mlists[];
extern struct mstat_t mstat[MTYPE_MAX];

/* #define MEMORY_LOG */
#ifdef MEMORY_LOG
#define XMALLOC(mtype, size) \
  mtype_zmalloc (__FILE__, __LINE__, (mtype), (size))
#define XCALLOC(mtype, size) \
  mtype_zcalloc (__FILE__, __LINE__, (mtype), (size))
#define XREALLOC(mtype, ptr, size)  \
  mtype_zrealloc (__FILE__, __LINE__, (mtype), (ptr), (size))
#define XFREE(mtype, ptr) \
  do { \
    mtype_zfree (__FILE__, __LINE__, (mtype), (ptr)); \
    ptr = NULL; } \
  while (0)
#define XSTRDUP(mtype, str) \
  mtype_zstrdup (__FILE__, __LINE__, (mtype), (str))
#else
#define XMALLOC(mtype, size)       zmalloc ((mtype), (size))
#define XCALLOC(mtype, size)       zcalloc ((mtype), (size))
#define XREALLOC(mtype, ptr, size) zrealloc ((mtype), (ptr), (size))
#define XFREE(mtype, ptr)          do { \
                                     zfree ((mtype), (ptr)); \
                                     ptr = NULL; } \
                                   while (0)
#define XSTRDUP(mtype, str)        zstrdup ((mtype), (str))
#endif /* MEMORY_LOG */

/* Human friendly string for given byte count */
#define MTYPE_MEMSTR_LEN 20


/* Prototypes of memory function. */
extern void *zmalloc (int type, int size);
extern void *zcalloc (int type, int size);
extern void *zrealloc (int type, void *ptr, int size);
extern void  zfree (int type, void *ptr);
extern char *zstrdup (int type, const char *str);

extern void *mtype_zmalloc (const char *file, int line, int type, int size);

extern void *mtype_zcalloc (const char *file, int line, int type, int size);

extern void *mtype_zrealloc (const char *file, int line, int type, void *ptr,
		             int size);

extern void mtype_zfree (const char *file, int line, int type,
		         void *ptr);

extern char *mtype_zstrdup (const char *file, int line, int type,
		            const char *str);
extern void memory_init (void);
extern void log_memstats_stderr (const char *);

/* return number of allocations outstanding for the type */
extern unsigned long mtype_stats_alloc (int);
extern const char *mtype_memstr (char *, int, unsigned long);


extern struct memory_list memory_list_lib[];
extern struct memory_list memory_list_zebra[];
extern struct memory_list memory_list_bgp[];
extern struct memory_list memory_list_rip[];
extern struct memory_list memory_list_ripng[];
extern struct memory_list memory_list_babel[];
extern struct memory_list memory_list_ospf[];
extern struct memory_list memory_list_ospf6[];
extern struct memory_list memory_list_isis[];
extern struct memory_list memory_list_pim[];
extern struct memory_list memory_list_vtysh[];


#endif /* _ZEBRA_MEMORY_H */
