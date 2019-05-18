/* RIP peer support
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

#include <zebra.h>
#include "log.h"
#include "if.h"
#include "prefix.h"
#include "command.h"
#include "linklist.h"
#include "thread.h"
#include "memory.h"
#include "table.h"
#include "route_com.h"
#include "ifm_common.h"
#include "ripd/ripd.h"
#include "ripd/rip_debug.h"

struct rip_peer * rip_peer_new(void);
int rip_peer_timeout(void *t);
struct rip_peer * rip_peer_get(struct rip_instance *rip, struct in_addr *addr);
char *rip_peer_uptime(struct rip_peer *peer, char *buf, size_t len);

struct rip_peer *
rip_peer_new(void)
{
	struct rip_peer *rip_peer_temp = NULL;
	
	do
	{
    	rip_peer_temp = XCALLOC(MTYPE_RIP_PEER, sizeof(struct rip_peer));
		if (rip_peer_temp == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(rip_peer_temp == NULL);

	memset(rip_peer_temp, 0, sizeof(struct rip_peer));

	return rip_peer_temp;
}

void
rip_peer_free(struct rip_peer *peer)
{
    XFREE(MTYPE_RIP_PEER, peer);
}

struct rip_peer *
rip_peer_lookup(struct rip_instance *rip, struct in_addr *addr)
{
    struct rip_peer *peer;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(rip->peer_list, node, nnode, peer))
    {
        if (IPV4_ADDR_SAME(&peer->addr, addr))
            return peer;
    }

    return NULL;
}

struct rip_peer *
rip_peer_lookup_next(struct rip_instance *rip, struct in_addr *addr)
{
    struct rip_peer *peer;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(rip->peer_list, node, nnode, peer))
    {
        if (htonl(peer->addr.s_addr) > htonl(addr->s_addr))
            return peer;
    }

    return NULL;
}

/* RIP peer is timeout. */
int
rip_peer_timeout(void *t)
{
    struct rip_peer *peer;
    struct rip_instance *rip = NULL;

    peer = (struct rip_peer *)(t);

    if (!peer)
        return 0;
	peer->t_timeout = 0;
    //if (IS_RIP_DEBUG_EVENT)
        zlog_debug(RIP_DBG_EVENT, "delete rip instance %d peer %s ", peer->id, inet_ntoa(peer->addr));

    rip = rip_instance_lookup(peer->id);

    if (!rip)
        return 0;

    listnode_delete(rip->peer_list, peer);
    rip_peer_free(peer);
	peer = NULL;

    return 0;
}

/* Get RIP peer.  At the same time update timeout thread. */
struct rip_peer *
rip_peer_get(struct rip_instance *rip, struct in_addr *addr)
{
    struct rip_peer *peer;

    peer = rip_peer_lookup(rip, addr);

    if (peer)
    {
        if (peer->t_timeout)
        {
            /*thread_cancel(peer->t_timeout);
			peer->t_timeout = NULL;*/
			RIP_TIMER_OFF(peer->t_timeout);
        }
    }
    else
    {
        peer = rip_peer_new();
        peer->addr = *addr;
        listnode_add_sort(rip->peer_list, peer);

        //if (IS_RIP_DEBUG_EVENT)
            zlog_debug(RIP_DBG_EVENT, "add rip peer %s ", inet_ntoa(peer->addr));
    }

    /* Update timeout thread. */
	if(0 == peer->t_timeout)
	{
	    /*peer->t_timeout = thread_add_timer(master_rip, rip_peer_timeout, peer,
                                       RIP_PEER_TIMER_DEFAULT);*/
		peer->t_timeout = high_pre_timer_add ((char *)"rip_peer_timer",\
					LIB_TIMER_TYPE_NOLOOP, rip_peer_timeout, peer, (RIP_PEER_TIMER_DEFAULT)*1000);
	}
    /* Last update time set. */
    time(&peer->uptime);

    return peer;
}

void
rip_peer_update(struct rip_instance *rip, struct sockaddr_in *from, u_char version, ifindex_t ifindex)
{
    struct rip_peer *peer;

    peer = rip_peer_get(rip, &from->sin_addr);
    peer->id = rip->id;
    peer->version = version;
    peer->ifindex = ifindex;

}

/* Display peer uptime. */
char *
rip_peer_uptime(struct rip_peer *peer, char *buf, size_t len)
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

int
rip_peer_list_cmp(struct rip_peer *p1, struct rip_peer *p2)
{
    return htonl(p1->addr.s_addr) > htonl(p2->addr.s_addr);
}

DEFUN(show_rip_neighbor,
      show_rip_neighbor_cmd,
      "show rip <1-255> neighbor [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      "neighbor information\n"
      "detail information\n")
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    struct prefix p;
    struct rip_instance *rip;
    struct rip_peer *peer;
    struct listnode *node, *nnode;
#define RIPNG_UPTIME_LEN 25
    char timebuf[RIPNG_UPTIME_LEN];
    char name[50];
    u_char id;

    id = (u_char)atoi(argv[0]);

    rip = rip_instance_lookup(id);

    if (! rip)
    {
        VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    if ((! rip->table) || (!rip->peer_list))
        return CMD_SUCCESS;

    for (ALL_LIST_ELEMENTS(rip->peer_list, node, nnode, peer))
    {
        if (!peer)
            return CMD_SUCCESS;

        vty_out(vty, "neighbor ip is:%s%s", inet_ntoa(peer->addr), VTY_NEWLINE);
        ifm_get_name_by_ifindex(peer->ifindex, name);
        vty_out(vty, "	connected via interface: %s%s", name, VTY_NEWLINE);
        vty_out(vty, "	time last heared from it is: %s%s", rip_peer_uptime(peer, timebuf, RIPNG_UPTIME_LEN), VTY_NEWLINE);

        if (argv[1])
        {
            vty_out(vty, "	network get from this peer: %s", VTY_NEWLINE);

            for (rp = route_top(rip->table); rp; rp = route_next(rp))
            {
                if ((list = (struct list *)rp->info) != NULL)
                {
                    for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
                    {
                        if (IPV4_ADDR_SAME(&peer->addr, &rinfo->from))
                        {
                            p = rp->p;
                            vty_out(vty, " 	            %s/%d %s", inet_ntoa(p.u.prefix4),
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
rip_peer_init(void)
{
    install_element(CONFIG_NODE, &show_rip_neighbor_cmd, CMD_LOCAL);
}

