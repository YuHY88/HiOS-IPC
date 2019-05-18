/* RIPng peer support
 * Copyright (C) 2000 Kunihiro Ishiguro <kunihiro@zebra.org>
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

/* RIPng support added by Vincent Jardin <vincent.jardin@6wind.com>
 * Copyright (C) 2002 6WIND
 */

#include <zebra.h>

#include "if.h"
#include "prefix.h"
#include "command.h"
#include "linklist.h"
#include "thread.h"
#include "memory.h"
#include "lib/ifm_common.h"
#include "lib/table.h"
#include "ripd/ripngd/ripngd.h"
#include "ripd/ripngd/ripng_nexthop.h"
#include "ripd.h"


struct ripng_peer *ripng_peer_new(void);
void ripng_peer_free(struct ripng_peer *peer);
int ripng_peer_timeout(void *t);
struct ripng_peer *ripng_peer_get(struct in6_addr *addr, struct list *peer_list);
char *ripng_peer_uptime(struct ripng_peer *peer, char *buf, size_t len);

/* Linked list of RIPng peer. */

struct ripng_peer *
ripng_peer_new(void)
{
    return XCALLOC(MTYPE_RIPNG_PEER, sizeof(struct ripng_peer));
}

void
ripng_peer_free(struct ripng_peer *peer)
{
    XFREE(MTYPE_RIPNG_PEER, peer);
}

struct ripng_peer *
ripng_peer_lookup(struct in6_addr *addr, struct list *peer_list)
{
    struct ripng_peer *peer;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(peer_list, node, nnode, peer))
    {
        if (IPV6_ADDR_SAME(&peer->addr, addr))
            return peer;
    }

    return NULL;
}

struct ripng_peer *
ripng_peer_lookup_next(struct in6_addr *addr, struct list *peer_list)
{
    struct ripng_peer *peer;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(peer_list, node, nnode, peer))
    {
        if (addr6_cmp(&peer->addr, addr) > 0)
            return peer;
    }

    return NULL;
}

/* RIPng peer is timeout.
 * Garbage collector.
 **/
int
ripng_peer_timeout(void *t)
{
    struct ripng_peer *peer;
    struct ripng_instance *ripng = NULL;
    struct list *peer_list = NULL;

    peer = (struct ripng_peer *)(t);
    ripng = ripng_instance_lookup(peer->id);

    if (!ripng)
    {
        return -1;
    }
	peer->t_timeout = 0;
    peer_list = ripng->peer_list;

    if (!peer_list)
    {
        return -1;
    }

    listnode_delete(peer_list, peer);
    ripng_peer_free(peer);
	peer = NULL;
    return 0;
}

/* Get RIPng peer.  At the same time update timeout thread. */
struct ripng_peer *
ripng_peer_get(struct in6_addr *addr, struct list *peer_list)
{
    struct ripng_peer *peer;

    peer = ripng_peer_lookup(addr, peer_list);

    if (peer)
    {
        if (peer->t_timeout)
        {
            /*thread_cancel(peer->t_timeout);
			peer->t_timeout = NULL;*/
			RIPNG_TIMER_OFF(peer->t_timeout);
        }
    }
    else
    {
        peer = ripng_peer_new();
        peer->addr = *addr; /* XXX */
        listnode_add_sort(peer_list, peer);
    }

    /* Update timeout thread. */
	if(0 == peer->t_timeout)
	{
    	/*peer->t_timeout = thread_add_timer(master_rip, ripng_peer_timeout, peer,
                                       RIPNG_PEER_TIMER_DEFAULT);*/
		peer->t_timeout = high_pre_timer_add ((char *)"ripng_t_out_timer", \
						LIB_TIMER_TYPE_NOLOOP, ripng_peer_timeout, peer, (RIPNG_PEER_TIMER_DEFAULT)*1000);
	}
    /* Last update time set. */
    time(&peer->uptime);

    return peer;
}

void
ripng_peer_update(struct sockaddr_in6 *from, struct ripng_instance *ripng, ifindex_t ifindex)
{
    struct ripng_peer *peer;
    peer = ripng_peer_get(&from->sin6_addr, ripng->peer_list);
    peer->id = ripng->id;
    peer->ifindex = ifindex;
}

void
ripng_peer_bad_route(struct sockaddr_in6 *from, struct list *peer_list)
{
    struct ripng_peer *peer;
    peer = ripng_peer_get(&from->sin6_addr, peer_list);
    peer->recv_badroutes++;
}

void
ripng_peer_bad_packet(struct sockaddr_in6 *from, struct list *peer_list)
{
    struct ripng_peer *peer;
    peer = ripng_peer_get(&from->sin6_addr, peer_list);
    peer->recv_badpackets++;
}

/* Display peer uptime. */
char *
ripng_peer_uptime(struct ripng_peer *peer, char *buf, size_t len)
{
    time_t uptime;
    struct tm *tm;

    /* If there is no connection has been done before print `never'. */
    if (peer->uptime == 0)
    {
        snprintf(buf, len, "never   ");
        return buf;
    }

    /* Get current time. */
    uptime = time(NULL);
    uptime -= peer->uptime;
    tm = gmtime(&uptime);

    /* Making formatted timer strings. */
#define ONE_DAY_SECOND 60*60*24
#define ONE_WEEK_SECOND 60*60*24*7

    if (uptime < ONE_DAY_SECOND)
        snprintf(buf, len, "%02d:%02d:%02d",
                 tm->tm_hour, tm->tm_min, tm->tm_sec);
    else if (uptime < ONE_WEEK_SECOND)
        snprintf(buf, len, "%dd%02dh%02dm",
                 tm->tm_yday, tm->tm_hour, tm->tm_min);
    else
        snprintf(buf, len, "%02dw%dd%02dh",
                 tm->tm_yday / 7, tm->tm_yday - ((tm->tm_yday / 7) * 7), tm->tm_hour);

    return buf;
}


DEFUN(show_ripng_neighbor,
      show_ripng_neighbor_cmd,
      "show ripng <1-255> neighbor [detail]",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      "neighbor information\n"
      "detail information\n")
{
    struct route_node *rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    struct prefix p;
    struct ripng_instance *ripng;
    struct ripng_peer *peer;
    struct listnode *node, *nnode;
#define RIPNG_UPTIME_LEN 25
    char timebuf[RIPNG_UPTIME_LEN];
    char name[50];
    u_char id;

    id = (u_char)atoi(argv[0]);

    ripng = ripng_instance_lookup(id);

    if (! ripng)
    {
        VTY_RIP_INSTANCE_ERR
    }

    if ((! ripng->table) || (!ripng->peer_list))
        return CMD_SUCCESS;

    for (ALL_LIST_ELEMENTS(ripng->peer_list, node, nnode, peer))
    {
        if (!peer)
            return CMD_SUCCESS;

        vty_out(vty, "neighbor ip is:%s%s", inet6_ntoa(peer->addr), VTY_NEWLINE);
        ifm_get_name_by_ifindex(peer->ifindex, name);
        vty_out(vty, "	connected via interface: %s%s", name, VTY_NEWLINE);
        vty_out(vty, "	protocol: RIPNG%s", VTY_NEWLINE);
        vty_out(vty, "	time last heared from it is: %s%s", ripng_peer_uptime(peer, timebuf, RIPNG_UPTIME_LEN), VTY_NEWLINE);

        if (argv[1])
        {
            vty_out(vty, "	network get from this peer: %s", VTY_NEWLINE);

            for (rp = route_top(ripng->table); rp; rp = route_next(rp))
            {
                if ((list = (struct list *)rp->info) != NULL)
                {
                    for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
                    {
                        if (IPV6_ADDR_SAME(&peer->addr, &rinfo->from))
                        {
                            p = rp->p;
                            vty_out(vty, " 	            %s/%d %s", inet6_ntoa(p.u.prefix6),
                                    p.prefixlen, VTY_NEWLINE);
                        }
                    }
                }
            }
        }

    }

    return CMD_SUCCESS;
}

void
ripng_peer_display(struct vty *vty, struct list *peer_list)
{
    struct ripng_peer *peer;
    struct listnode *node, *nnode;
#define RIPNG_UPTIME_LEN 25
    char timebuf[RIPNG_UPTIME_LEN];

    for (ALL_LIST_ELEMENTS(peer_list, node, nnode, peer))
    {
        vty_out(vty, "    %s %s%14s %10d %10d %10d      %s%s", inet6_ntoa(peer->addr),
                VTY_NEWLINE, " ",
                peer->recv_badpackets, peer->recv_badroutes,
                ZEBRA_RIPNG_DISTANCE_DEFAULT,
                ripng_peer_uptime(peer, timebuf, RIPNG_UPTIME_LEN),
                VTY_NEWLINE);
    }
}

int
ripng_peer_list_cmp(struct ripng_peer *p1, struct ripng_peer *p2)
{
    return addr6_cmp(&p1->addr, &p2->addr) > 0;
}

void
ripng_peer_init(void)
{
    install_element(CONFIG_NODE, &show_ripng_neighbor_cmd, CMD_LOCAL);
}

