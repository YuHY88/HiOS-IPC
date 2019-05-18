/*
 * Copyright (C) 2003 Yasuhiro Ohara
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
 * along with GNU Zebra; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef OSPF6D_H
#define OSPF6D_H

#define OSPF6_DAEMON_VERSION    "0.9.7r"

#include "libospf.h"
#include "thread.h"
//#include "fifo.h"
#include "route_com.h"
#include "lib/hptimer.h"


/* global variables */
extern struct thread_master *master_ospf6;

/* Historical for KAME.  */
#ifndef IPV6_JOIN_GROUP
#ifdef IPV6_ADD_MEMBERSHIP
#define IPV6_JOIN_GROUP IPV6_ADD_MEMBERSHIP
#endif /* IPV6_ADD_MEMBERSHIP. */
#ifdef IPV6_JOIN_MEMBERSHIP
#define IPV6_JOIN_GROUP  IPV6_JOIN_MEMBERSHIP
#endif /* IPV6_JOIN_MEMBERSHIP. */
#endif /* ! IPV6_JOIN_GROUP*/

#ifndef IPV6_LEAVE_GROUP
#ifdef IPV6_DROP_MEMBERSHIP
#define IPV6_LEAVE_GROUP IPV6_DROP_MEMBERSHIP
#endif /* IPV6_DROP_MEMBERSHIP */
#endif /* ! IPV6_LEAVE_GROUP */

#define OSPF6_DEFAULT_PROCESS_ID             1
#define OSPF6_MAX_PROCESS_ID                 255


#define MSG_OK    0
#define MSG_NG    1

/* cast macro: XXX - these *must* die, ick ick. */
#define OSPF6_PROCESS(x) ((struct ospf6 *) (x))
#define OSPF6_AREA(x) ((struct ospf6_area *) (x))
#define OSPF6_INTERFACE(x) ((struct ospf6_interface *) (x))
#define OSPF6_NEIGHBOR(x) ((struct ospf6_neighbor *) (x))

/* operation on timeval structure */
#ifndef timerclear
#define timerclear(a) (a)->tv_sec = (tvp)->tv_usec = 0
#endif /*timerclear*/
#ifndef timersub
#define timersub(a, b, res)                           \
  do {                                                \
    (res)->tv_sec = (a)->tv_sec - (b)->tv_sec;        \
    (res)->tv_usec = (a)->tv_usec - (b)->tv_usec;     \
    if ((res)->tv_usec < 0)                           \
      {                                               \
        (res)->tv_sec--;                              \
        (res)->tv_usec += 1000000;                    \
      }                                               \
  } while (0)
#endif /*timersub*/
#define timerstring(tv, buf, size)                      \
  do {                                                  \
    if ((tv)->tv_sec / 60 / 60 / 24)                    \
      snprintf (buf, size, "%lldd%02lld:%02lld:%02lld", \
                (tv)->tv_sec / 60LL / 60 / 24,          \
                (tv)->tv_sec / 60LL / 60 % 24,          \
                (tv)->tv_sec / 60LL % 60,               \
                (tv)->tv_sec % 60LL);                   \
    else                                                \
      snprintf (buf, size, "%02lld:%02lld:%02lld",      \
                (tv)->tv_sec / 60LL / 60 % 24,          \
                (tv)->tv_sec / 60LL % 60,               \
                (tv)->tv_sec % 60LL);                   \
  } while (0)
#define timerstring_local(tv, buf, size)                  \
  do {                                                    \
    int ret;                                              \
    struct tm *tm;                                        \
    tm = localtime (&(tv)->tv_sec);                       \
    ret = strftime (buf, size, "%Y/%m/%d %H:%M:%S", tm);  \
    if (ret == 0)                                         \
      zlog_warn ("strftime error");                       \
  } while (0)

#define threadtimer_string(now, t, buf, size)                         \
  do {                                                                \
    struct timeval result;                                            \
    if (!t)                                                           \
      snprintf(buf, size, "inactive");                    \
    else {                                                            \
      timersub(&t->u.sands, &now, &result);                           \
      timerstring(&result, buf, size);                                \
    }                                                                 \
} while (0)

#define threadtimer_string_new(now, t, buf, size)                         \
	  do {																  \
		struct timeval result;											  \
		int remain_time;												\
		if (!t) 														  \
		  snprintf(buf, size, "inactive");					  \
		else {															  \
			high_pre_timer_remain(t, &remain_time);						\
			result.tv_sec = remain_time;								\
			result.tv_usec = 0;											\
			timerstring(&result, buf, size);								  \
		}																  \
	} while (0)


/* for commands */
#define OSPF6_AREA_STR      "Area information\n"
#define OSPF6_AREA_ID_STR   "Area ID (as an IPv4 notation)\n"
#define OSPF6_SPF_STR       "Shortest Path First tree information\n"
#define OSPF6_ROUTER_ID_STR "Specify Router-ID\n"
#define OSPF6_LS_ID_STR     "Specify Link State ID\n"

#define VNL VTY_NEWLINE
#define OSPF6_CMD_CHECK_RUNNING() \
  if (ospf6 == NULL) \
    { \
      vty_error_out (vty, "OSPF6 is not running%s", VTY_NEWLINE); \
      return CMD_WARNING; \
    }


#define VTY_CHECK_NM_INTERFACE6(V)                                     \
{                                                                       \
    if(IFM_TYPE_IS_OUTBAND (V))               \
    {                                               \
        vty_error_out(vty,"Outband is not support.%s",VTY_NEWLINE);        \
        return CMD_WARNING;                                                 \
    }                                                                       \
}

#define VTY_INTERFACE_MODE_ERR6                                      \
{                                                                       \
    vty_error_out(vty,"Please select mode l3.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}

#define VTY_OSPF_INTERFACE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"OSPF6 instance is not enabled on this interface.%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}
#define VTY_OSPF6_INSTANCE_ERR                                      \
{                                                                       \
    vty_error_out(vty,"OSPF6 Instance does not exist, please configure the Instance first..%s",VTY_NEWLINE);        \
    return CMD_WARNING;                                                 \
}


#define OSPF6_TIMER_OFF(X)                                                     	\
	do {																		\
	  if (X)																	\
		{																		\
		  high_pre_timer_delete (X);											\
		  (X) = 0;																\
		}																		\
	} while (0)

struct ospf6_statics
{
	u_int32_t recv_all;
	u_int32_t recv_error;
	u_int32_t send_all;
	u_int32_t send_error;
	u_int32_t s_hel;
    u_int32_t s_dd;
    u_int32_t s_lsr;
    u_int32_t s_lsu;
    u_int32_t s_lsa;
    u_int32_t r_hel;
    u_int32_t r_dd;
    u_int32_t r_lsr;
    u_int32_t r_lsu;
    u_int32_t r_lsa;
};



struct ospf6d_master
{
    /* OSPF instance list*/
    struct list *ospf6;

    struct thread_master *master;
    //struct thread *t_thread_routefifo;    /*异步发送route thread*/
	//struct thread *t_read_msg;
	//struct thread *t_read_pkt;
	TIMERID t_thread_routefifo;    /*异步发送route thread*/
    struct fifo routefifo;    /*异步发送route fifo*/
    //struct fifo ifmfifo;

    /* redistribute type flag  */
    int redis_count [ROUTE_PROTO_MAX + 1];
	struct ospf6_statics ospf6_statics;


    struct ospf6_route_table *external6_table;
};


/* Function Prototypes */
extern struct ospf6d_master *om6;
extern struct in6_addr allspfrouters6;
extern struct in6_addr alldrouters6;

extern struct route_node *route_prev(struct route_node *);

extern void ospf6_debug(void);
extern void ospf6_init(void);
extern void ospf6_master_init(void);

#endif /* OSPF6D_H */



