/* RIPd and zebra interface.
 * Copyright (C) 1997, 1999 Kunihiro Ishiguro <kunihiro@zebra.org>
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
#include <sys/msg.h>
#include "command.h"
#include "prefix.h"
#include "table.h"
#include "stream.h"
#include "memory.h"
#include "memtypes.h"
#include "routemap.h"
#include "log.h"
#include "vrf.h"
#include "msg_ipc.h"
#include "route/route.h"
#include "ripd/ripd.h"
#include "ripngd/ripngd.h"
#include "ripd/rip_debug.h"
#include "ripd/rip_interface.h"

#include "route_types.h"
#include "route_com.h"
#include "errcode.h"
/* All information about zebra. */

extern struct list *rip_list;

/* Redistribution types */
static struct
{
    enum ROUTE_PROTO type;
    int str_min_len;
    const char *str;
    u_int32_t rcount;
} rip_redist_type[] =
{
    {ROUTE_PROTO_CONNECT, 1, "connected",  0},
    {ROUTE_PROTO_STATIC,  1, "static",     0},
    {ROUTE_PROTO_ISIS,    5, "isis",       0},
    {ROUTE_PROTO_IBGP,    2, "ibgp",       0},
    {ROUTE_PROTO_EBGP,    2, "ebgp",       0},
    {ROUTE_PROTO_OSPF,    5, "ospf",       0},
    {ROUTE_PROTO_RIP,     4, "rip",        0},
	{ROUTE_PROTO_RIPNG,	  4, "ripng", 	   0},
    {ROUTE_PROTO_OSPF6,   5, "ospf6",      0},
	{ROUTE_PROTO_ISIS6,   5, "isis6",	   0},
    {0, 0, NULL, 0}
};

int rip_route_read_ipv4(uint8_t opcode , struct route_entry *p_route);
void rip_zebra_ipv4_send(struct route_node *rp, enum IPC_OPCODE cmd);
int add_rip_fifo_route(struct route_entry *route, enum IPC_OPCODE opcode);
void redis_metric_change_summary_metric_change(struct rip_instance *rip, struct route_node *rp, struct rip_info *rinfo_new);
void rip_table_metric_change(struct redist_msg *rm, struct rip_instance *rip);
void ripng_zebra_ipv6_send(struct route_node *rp, enum IPC_OPCODE cmd);
void ripng_table_metric_change(struct redist_msg *rm, struct ripng_instance *ripng);
struct redist_msg *rip_redist_new(void);

struct redist_msg *rip_redist_new(void)
{
	struct redist_msg *redist_msg_temp = NULL;
	
	do
	{
    	redist_msg_temp = (struct redist_msg*)XCALLOC(MTYPE_RIP_REDIST, sizeof(struct redist_msg));
		if (redist_msg_temp == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(redist_msg_temp == NULL);

	memset(redist_msg_temp, 0, sizeof(struct redist_msg));

	return redist_msg_temp;
}

static void
rip_redist_free(struct redist_msg *rm)
{
    XFREE(MTYPE_RIP_REDIST, rm);
}
/* Zebra route add and delete treatment. */
int
rip_route_read_ipv4(uint8_t opcode , struct route_entry *p_route)
{
    struct prefix_ipv4 p;
    unsigned char plength = 0;
    struct rip_instance *rip;
    struct rip_info rinfo;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;

    memset(&p, 0, sizeof(struct prefix_ipv4));
    p.family = AF_INET;
    plength = p_route->prefix.prefixlen;
    p.prefixlen = MIN(IPV4_MAX_PREFIXLEN, plength);
    p.prefix.s_addr = htonl(p_route->prefix.addr.ipv4);
    memset(&rinfo, 0, sizeof(struct rip_info));

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "            %s/%d", inet_ntoa(p.prefix), p.prefixlen);
    }

    if (IPV4_NET127(p_route->prefix.addr.ipv4))
    {
        return -1;;
    }

    if (IPV4_NET0(p_route->prefix.addr.ipv4))
    {
        return -1;;
    }

    if ((p_route->nhp[0].protocol == ROUTE_PROTO_CONNECT)
            && (p.prefixlen == IPV4_MAX_BITLEN)
            && (!IFM_TYPE_IS_LOOPBCK(p_route->nhp[0].ifindex)))
    {
        zlog_debug(RIP_DBG_EVENT, "host route doesn't redistribute to rip");
        return -1;;
    }

    rinfo.type = p_route->nhp[0].protocol;
    rinfo.distance =  p_route->nhp[0].distance;
    SET_FLAG(rinfo.sub_type, RIP_ROUTE_REDISTRIBUTE);
    rinfo.external_id = p_route->nhp[0].instance;

    if (!p_route->nhp[0].ifindex && 1 == p_route->nhp[0].action)
    {
        rinfo.ifindex = p_route->nhp[0].action;
        rinfo.ifindex_out = p_route->nhp[0].action;
    }
    else
    {
        rinfo.ifindex = p_route->nhp[0].ifindex;
        rinfo.ifindex_out = p_route->nhp[0].ifindex;
    }

    rinfo.nexthop.s_addr = htonl(p_route->nhp[0].nexthop.addr.ipv4);
    rinfo.external_metric = p_route->nhp[0].cost;

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
    {
        if (rip->redist_infos)
        {
            for (ALL_LIST_ELEMENTS_RO(rip->redist_infos, nnode, lrm))
            {
                if (!lrm)
                    continue;

                if (lrm->type == p_route->nhp[0].protocol && lrm->id == p_route->nhp[0].instance)
                {
                    /* Then fetch IPv4 prefixes. */
                    rinfo.id = rip->id;

                    if (opcode == IPC_OPCODE_ADD)
                    {
                        if (lrm->metric == 17)
                        {
                            rinfo.metric = 0;
                            rinfo.metric_out = 0;
                        }
                        else
                        {
                            rinfo.metric = lrm->metric;
                            rinfo.metric_out = lrm->metric;
                        }

                        rip_redistribute_add(rip, &rinfo, &p);
                    }
                    else if (opcode == IPC_OPCODE_DELETE)
                    {
                        rip_redistribute_delete(rip, &rinfo, &p);
                    }
                }
            }
        }
    }

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
    {
        if (rip->triggered_update_flag)
        {
            rip_event(RIP_TRIGGERED_UPDATE, rip);
            rip->triggered_update_flag = 0;
        }
    }

    return 0;

}

int
rip_route_read(struct ipc_msghdr_n *phdr, void *data)
{
    struct route_entry *p_route = NULL;
    int count = 0;

    p_route = (struct route_entry *)data;

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "get info from route, opcode %d, route number %d", phdr->opcode, phdr->data_num);
    }

    for (count = 0; count < phdr->data_num; count++)
    {
        if (p_route->prefix.type == INET_FAMILY_IPV4)
        {
            rip_route_read_ipv4(phdr->opcode , p_route);
        }
        else if (p_route->prefix.type == INET_FAMILY_IPV6)
        {
            ripng_route_read_ipv6(phdr->opcode , p_route);
        }
        else
        {
			//if (IS_RIP_DEBUG_EVENT)
			{
            	zlog_err("Err:%s [%d] route type no V4 or V6", __func__, __LINE__);
			}
            return 0;
        }

        p_route++;
    }

    return 0;
}

int
add_rip_fifo_route(struct route_entry *route, enum IPC_OPCODE opcode)
{
    struct routefifo *newroutefifoentry = NULL;
	
	do
	{
    	newroutefifoentry = XCALLOC(MTYPE_ROUTE_FIFO, sizeof(struct routefifo));
		if (newroutefifoentry == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newroutefifoentry == NULL);

	memset(newroutefifoentry, 0, sizeof(struct routefifo));

    if (newroutefifoentry == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return -1;
    }

    newroutefifoentry->fifo.next = NULL;
    memcpy(&newroutefifoentry->route, route, sizeof(struct route_entry));
    newroutefifoentry->opcode = opcode;
    FIFO_ADD(&rip_m.routefifo, &newroutefifoentry->fifo);

    return 0;
}
/* Send ECMP routes to zebra. */
void
rip_zebra_ipv4_send(struct route_node *rp, enum IPC_OPCODE cmd)
{

    struct list *list = (struct list *)rp->info;
    struct listnode *listnode = NULL;
    struct rip_info *rinfo = NULL;
    int i = 0;
    struct route_entry route;

    /* Reset stream. */
    memset(&route, 0, sizeof(struct route_entry));
    route.prefix.type = INET_FAMILY_IPV4;
    route.prefix.addr.ipv4 = ntohl(rp->p.u.prefix4.s_addr);
    route.prefix.prefixlen = rp->p.prefixlen;
    route.vpn = 0;

    for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
    {
		if (!rip_route_rte(rinfo)
			|| (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY)))
		{
			continue;
		}

        if (i >= NHP_ECMP_NUM)
            break;

        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].protocol = ROUTE_PROTO_RIP;
        route.nhp[i].nexthop.type = INET_FAMILY_IPV4;
        route.nhp[i].nexthop.addr.ipv4 = ntohl(rinfo->nexthop.s_addr);
        route.nhp[i].ifindex = rinfo->ifindex;
        route.nhp[i].instance = rinfo->id;
        route.nhp[i].cost = rinfo->metric;
        route.nhp[i].distance = rinfo->distance;

        route.nhp[i].vpn = 0 ;
        route.nhp[i].down_flag = 1;
        route.nhp[i].active = 0;

        i++;

        if (cmd == IPC_OPCODE_ADD)
        {
            rip_global_route_add++;
            SET_FLAG(rinfo->flags, RIP_RTF_FIB);
        }
        else if (cmd == IPC_OPCODE_DELETE)
        {
            rip_global_route_del++;
            UNSET_FLAG(rinfo->flags, RIP_RTF_FIB);
        }
		
		//if (IS_RIP_DEBUG_EVENT)
		{
			if (cmd == IPC_OPCODE_ADD)
			{
				zlog_debug(RIP_DBG_EVENT, "rip %d add %s/%d to route", rinfo->id ,
						   inet_ntoa(rp->p.u.prefix4), rp->p.prefixlen);
			}
			else if (cmd == IPC_OPCODE_DELETE)
			{
				zlog_debug(RIP_DBG_EVENT, "rip %d delete %s/%d from route", rinfo->id,
						   inet_ntoa(rp->p.u.prefix4), rp->p.prefixlen);
			}
			else
			{
				zlog_debug(RIP_DBG_EVENT, "rip %d update %s/%d to route", rinfo->id,
						   inet_ntoa(rp->p.u.prefix4), rp->p.prefixlen);
			}
		}

    }

    route.nhp_num = i;
    route.nhp[0].action = NHP_ACTION_FORWARD;

    add_rip_fifo_route(&route, cmd);

}

int
rip_send_route_fifo(void *thread)
{
	int i = 0;
	int ret = 0;
	int item = 0;
    int send_or_count = 0;
    struct fifo *pfifo = NULL;
    uint32_t opcodeflag = 0;
 	int msg_num = IPC_RIP_MSG_LEN / sizeof(struct route_entry); /*IPC_MSG_LEN/sizeof(struct route_entry);*/
	struct routefifo *routefifo_entry[msg_num];
    struct route_entry route[msg_num];
    
	
	//rip_m.t_thread_routefifo = NULL;
	rip_m.t_thread_routefifo = 0;

    for (item = 0; item < msg_num; item++)
        routefifo_entry[item] = NULL;

    while (!FIFO_EMPTY(&rip_m.routefifo))
    {
        if (send_or_count >= 2000)
        {
            break;
        }

        pfifo = &rip_m.routefifo;

        routefifo_entry[0] = (struct routefifo *)FIFO_TOP(pfifo);
        opcodeflag = routefifo_entry[0]->opcode;
        pfifo = &(routefifo_entry[0]->fifo);
        memcpy(&route[0], &routefifo_entry[0]->route, sizeof(struct route_entry));
        item = 1;

        while (!FIFO_HEAD(pfifo) && (item < msg_num))
        {
            routefifo_entry[item] = (struct routefifo *)FIFO_TOP(pfifo);

            if (opcodeflag != routefifo_entry[item]->opcode)
                break;

            memcpy(&route[item], &routefifo_entry[item]->route, sizeof(struct route_entry));
            pfifo = &(routefifo_entry[item]->fifo);
            item++;
        }

        /*ret = ipc_send_route(route, sizeof(struct route_entry) * item, item,
                             MODULE_ID_ROUTE, MODULE_ID_RIP, IPC_TYPE_ROUTE, 0, opcodeflag);*/
		ret = ipc_send_msg_n2(route, sizeof(struct route_entry) * item, item,
                             MODULE_ID_ROUTE, MODULE_ID_RIP, IPC_TYPE_ROUTE, 0, opcodeflag, 0);

        if (ret < 0)
        {
            zlog_err("%-15s[%d]:ERROR:%d", __FUNCTION__, __LINE__, ret);
            break;
        }
        else
        {
            send_or_count = send_or_count + item;
        }

        for (i = 0; i < item; i++)
        {
            FIFO_DEL(routefifo_entry[i]);
            XFREE(MTYPE_ROUTE_FIFO, routefifo_entry[i]);
            routefifo_entry[i] = NULL;
        }
    }

    /*rip_m.t_thread_routefifo =
        thread_add_timer_msec(master_rip, rip_send_route_fifo, NULL, 1000);*/
	rip_m.t_thread_routefifo =
		high_pre_timer_add ((char *)"rip_send_route_timer", LIB_TIMER_TYPE_NOLOOP, \
								rip_send_route_fifo, NULL, 1000);

    return 0;
}

/* Add/update ECMP routes to zebra. */
void
rip_zebra_ipv4_update(struct route_node *rp)
{
    rip_zebra_ipv4_send(rp, IPC_OPCODE_UPDATE);
}

/* Add/update ECMP routes to zebra. */
void
rip_zebra_ipv4_add(struct route_node *rp)
{
    rip_zebra_ipv4_send(rp, IPC_OPCODE_ADD);
}

/* Delete ECMP routes from zebra. */
void
rip_zebra_ipv4_delete(struct route_node *rp)
{
    rip_zebra_ipv4_send(rp, IPC_OPCODE_DELETE);
}

void
redis_metric_change_summary_metric_change(struct rip_instance *rip, struct route_node *rp, struct rip_info *rinfo_new)
{
    struct route_node *sp = NULL;
    struct rip_info *rinfo = NULL;
    struct prefix classfull;
    struct list *list = NULL;
    u_int32_t ifmt [2];

    if (CHECK_FLAG(rinfo_new->summary_flag, RIP_SUMMARY_AUTO))
    {
        memcpy(&classfull, &rp->p, sizeof(struct prefix));
    }
    else
    {
        memcpy(&classfull, &rp->p, sizeof(struct prefix));
        rip_apply_classful_mask_ipv4((struct prefix_ipv4 *)&classfull);
    }

    sp = route_node_lookup(rip->table, &classfull);

    if (sp && ((list = (struct list *)sp->info) != NULL))
    {
        rinfo = (struct rip_info *)listgetdata(listhead(list));

        if (rinfo->mmt_count > 1)
        {
            rinfo->mmt_count--;
        }
        else
        {
            rip_summary_subprefix_flagset(sp, 1, 1, ifmt);

            if (rinfo->metric != ifmt[0])
            {
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }

            rinfo->metric = ifmt[0];
            rinfo->metric_out = ifmt[0];
            rinfo->mmt_count = ifmt[1];
        }
    }
	if(sp != NULL)
	{
		route_unlock_node(sp);
	}

    sp = NULL;
    list = NULL;
    rinfo = NULL;
    sp = rip_inter_summary_prefix_match(&rp->p);

    if (!sp || ((list = (struct list *)sp->info) == NULL))
        return;

    rinfo = (struct rip_info *)listgetdata(listhead(list));

    if (rinfo->mmt_count > 1)
    {
        rinfo->mmt_count--;
    }
    else
    {
        rip_summary_subprefix_flagset(sp, 2, 1, ifmt);

        if (rinfo->metric != ifmt[0])
        {
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
        }

        rinfo->metric = ifmt[0];
        rinfo->metric_out = ifmt[0];
        rinfo->mmt_count = ifmt[1];
    }
}

void
rip_table_metric_change(struct redist_msg *rm, struct rip_instance *rip)
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL && listcount(list) != 0)
        {
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                if (rm->id && rinfo->external_id != rm->id)
                {
                    continue;
                }

                if ((rinfo->type == rm->type)
                        && (rinfo->metric != RIP_METRIC_INFINITY))
                {
                    if (17 == rm->metric)
                    {
                        rinfo->metric = 0;
						rinfo->metric_out = 0;
                    }
                    else
                    {
                        rinfo->metric = rm->metric;
						rinfo->metric_out = rm->metric;
                    }

                    SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
                    redis_metric_change_summary_metric_change(rip, rp, rinfo);
                }
            }
        }
    }

    rip_event(RIP_TRIGGERED_UPDATE, rip);
}

void
rip_redistribute_clean(struct list *redist_infos)
{
    struct redist_msg *lrm;
    struct listnode *node, *nnode;
    u_int32 i;

    for (i = 0; rip_redist_type[i].str; i++)
    {
        for (ALL_LIST_ELEMENTS(redist_infos, node, nnode, lrm))
            if (lrm->type == rip_redist_type[i].type)
            {
                listnode_delete(redist_infos, lrm);
                rip_redist_free(lrm);				
				lrm = NULL;
                rip_redist_type[i].rcount--;

                if (rip_redist_type[i].rcount == 0)
                    route_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
            }
    }
}

DEFUN(rip_redistribute_instance_type,
      rip_redistribute_instance_type_cmd,
      "redistribute (ospf|isis|rip) <1-255> {metric <0-15>}",
      REDIST_STR
      "Open Shortest Path First (OSPFv2)\n" \
      "Intermediate System to Intermediate System (IS-IS)\n" \
      "Routing Information Protocol (RIP)\n"     \
      "instance number 1-255\n"                  \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *rm = NULL;
    u_int32 i;
    u_int32_t metric = 17;
    u_int32_t old_metric = 17;
    u_char id = 0;

    if (!rip)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_ERR_NO_MATCH;
    }

    if (argv[1] != NULL)
    {
        id = (u_char)atoi(argv[1]);
    }

    if (argv[2] != NULL)
    {
        metric = atoi(argv[2]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0], rip_redist_type[i].str_min_len) == 0)
        {
            if (rip_redist_type[i].type == ROUTE_PROTO_RIP && id == rip->id)
            {
                vty_error_out(vty, "Can`t redistribtue itself %s", VTY_NEWLINE);
                return CMD_WARNING;
            }

            for (ALL_LIST_ELEMENTS(rip->redist_infos, node, nnode, rm))
                if (rm->type == rip_redist_type[i].type && rm->id == id)
                    break;

            if (rm)
            {
                if (rm->metric != metric)
                {
                    old_metric = rm->metric;
                    rm->metric = metric;
                }
                else
                {
                    if (argv[2])
                    {
                        vty_error_out(vty, "Already existed redistribute %s instance %s metric %s%s",
                                argv[0], argv[1], argv[2], VTY_NEWLINE);
                    }
                    else
                    {
                        vty_error_out(vty, "Already existed redistribute %s instance %s%s",
                                argv[0], argv[1], VTY_NEWLINE);
                    }

                    return CMD_SUCCESS;
                }
            }
            else
            {
                rm = rip_redist_new();
                rm->type = rip_redist_type[i].type;
                rm->id = id;
                rm->metric = metric;
                listnode_add(rip->redist_infos, rm);

                if (rip_redist_type[i].rcount)
                {
                    route_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

                route_event_register(rip_redist_type[i].type, MODULE_ID_RIP);
                rip_redist_type[i].rcount++;
            }

            if (metric != old_metric)
            {
                rip_table_metric_change(rm, rip);
                //  rip_update_process (rip_all_route, rip, 0);
            }

            return CMD_SUCCESS;
        }
    }

    vty_error_out(vty, "Invalid type %s%s", argv[0], VTY_NEWLINE);
    return CMD_WARNING;
}

DEFUN(no_rip_redistribute_instance_type,
      no_rip_redistribute_instance_type_cmd,
      "no redistribute (ospf|isis|rip) <1-255>",
      NO_STR
      REDIST_STR
      "Open Shortest Path First (OSPFv2)\n" \
      "Intermediate System to Intermediate System (IS-IS)\n" \
      "Routing Information Protocol (RIP)\n"      \
      "instance number 1-255\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;
    u_int32 i;
    u_char id = 0;

    if (!rip)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    if (argv[1] != NULL)
    {
        id = (u_char)atoi(argv[1]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0],
                    rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(rip->redist_infos, node, nnode, lrm))
                if (lrm->type == rip_redist_type[i].type && lrm->id == id)
                    break;

            if (lrm)
            {
                listnode_delete(rip->redist_infos, lrm);
                rip_redist_free(lrm);
				lrm = NULL;
                if (rip_redist_type[i].rcount > 1)
                    rip_redist_type[i].rcount--;
                else
                {
                    rip_redist_type[i].rcount = 0;
                    route_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

                rip_redistribute_withdraw(rip_redist_type[i].type, id, rip);
            }
            else
            {
                vty_error_out(vty, "Can`t find redistribute %s instance %s%s",
                        argv[0], argv[1], VTY_NEWLINE);
                return CMD_WARNING;
            }

            return CMD_SUCCESS;
        }
    }

    return CMD_WARNING;
}

DEFUN(rip_redistribute_type,
      rip_redistribute_type_cmd,
      "redistribute (connected|static|ibgp|ebgp) {metric <0-15>}",
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol (IBGP)\n" \
      "Exter Border Gateway Protocol (EBGP)\n" \
      "redistribute metric, range 0-15\n"         \
      "redistribute metric value\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *rm = NULL;
    u_int32 i;
    u_int32_t metric = 17;
    u_int32_t old_metric = 17;

    if (!rip)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    if (argv[1] != NULL)
    {
        metric = atoi(argv[1]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0], rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(rip->redist_infos, node, nnode, rm))
                if (rm->type == rip_redist_type[i].type)
                    break;

            if (rm)
            {

                if (rm->metric != metric)
                {
                    old_metric = rm->metric;
                    rm->metric = metric;
                }
                else
                {
                    if (argv[1])
                    {
                        vty_error_out(vty, "Already existed redistribute %s metric %s%s",
                                argv[0], argv[1], VTY_NEWLINE);
                    }
                    else
                    {
                        vty_error_out(vty, "Already existed redistribute %s%s",
                                argv[0], VTY_NEWLINE);
                    }

                    return CMD_SUCCESS;
                }
            }
            else
            {
                rm = rip_redist_new();
                rm->type = rip_redist_type[i].type;
                rm->id = 0;
                rm->metric = metric;
                listnode_add(rip->redist_infos, rm);

                if (rip_redist_type[i].rcount)
                {
                    route_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

                route_event_register(rip_redist_type[i].type, MODULE_ID_RIP);

                rip_redist_type[i].rcount++;
            }

            if (metric != old_metric)
            {
                rip_table_metric_change(rm, rip);
            }

            return CMD_SUCCESS;
        }
    }

    vty_error_out(vty, "Invalid type %s%s", argv[0], VTY_NEWLINE);
    return CMD_WARNING;
}

DEFUN(no_rip_redistribute_type,
      no_rip_redistribute_type_cmd,
      "no redistribute (connected|static|ibgp|ebgp)",
      NO_STR
      REDIST_STR
      "Connected routes (directly attached subnet or host)\n" \
      "Statically configured routes\n" \
      "Inter Border Gateway Protocol (IBGP)\n" \
      "Exter Border Gateway Protocol (EBGP)\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;
    int i;
    u_char id = 0;

    if (!rip)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0],
                    rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(rip->redist_infos, node, nnode, lrm))
            {
                if (lrm->type == rip_redist_type[i].type && lrm->id == id)
                    break;
            }

            if (lrm)
            {
                rip_redistribute_withdraw(rip_redist_type[i].type, 0, rip);
                listnode_delete(rip->redist_infos, lrm);
                rip_redist_free(lrm);
				lrm = NULL;

                if (rip_redist_type[i].rcount > 1)
                    rip_redist_type[i].rcount--;
                else
                {
                    rip_redist_type[i].rcount = 0;
                    route_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

            }
            else
            {
                vty_error_out(vty, "Can`t find redistribute %s%s",
                        argv[0], VTY_NEWLINE);
                return CMD_WARNING;
            }

            return CMD_SUCCESS;
        }
    }

    return CMD_WARNING;
}

/* Send ECMP routes to zebra. */
void
ripng_zebra_ipv6_send(struct route_node *rp, enum IPC_OPCODE cmd)
{

    struct list *list = (struct list *)rp->info;
    struct listnode *listnode = NULL;
    struct ripng_info *rinfo = NULL;
    int i = 0;
    struct route_entry route;

    /* Reset stream. */
    memset(&route, 0, sizeof(struct route_entry));
    route.prefix.type = INET_FAMILY_IPV6;
    IPV6_ADDR_COPY(&route.prefix.addr.ipv6, &rp->p.u.prefix6.s6_addr);
    route.prefix.prefixlen = rp->p.prefixlen;
    route.vpn = 0;

    for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
    {
        if (rinfo->type != ROUTE_PROTO_RIPNG)
            continue;

        if (i >= NHP_ECMP_NUM)
            break;

        route.nhp[i].nhp_type = NHP_TYPE_IP;
        route.nhp[i].protocol = ROUTE_PROTO_RIPNG;
        route.nhp[i].nexthop.type = INET_FAMILY_IPV6;
        IPV6_ADDR_COPY(&route.nhp[i].nexthop.addr.ipv6, &rinfo->nexthop.s6_addr);
        route.nhp[i].ifindex = rinfo->ifindex;
        route.nhp[i].instance = rinfo->id;
        route.nhp[i].cost = rinfo->metric;
        route.nhp[i].distance = rinfo->distance;
        route.nhp[i].vpn = 0 ;
        route.nhp[i].down_flag = 1;
        route.nhp[i].active = 0;

        i++;

        if (cmd == IPC_OPCODE_ADD)
        {
            SET_FLAG(rinfo->flags, RIPNG_RTF_FIB);
        }
        else if (cmd == IPC_OPCODE_DELETE)
        {
            UNSET_FLAG(rinfo->flags, RIPNG_RTF_FIB);
        }
		
		//if (IS_RIPNG_DEBUG_EVENT)
		{
			if (cmd == IPC_OPCODE_ADD)
			{
				zlog_debug(RIPNG_DBG_EVENT, "ripng %d add %s/%d to route", rinfo->id , 
					inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen);
			}
			else if (cmd == IPC_OPCODE_DELETE)
			{
				zlog_debug(RIPNG_DBG_EVENT, "ripng %d delete %s/%d from route", rinfo->id,
					inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen);
			}
			else
			{
				zlog_debug(RIPNG_DBG_EVENT, "ripng %d update %s/%d  distance %d to route", rinfo->id,
					inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen, rinfo->distance);
			}
		
		}

    }

    route.nhp_num = i;
    route.nhp[0].action = NHP_ACTION_FORWARD;

    add_rip_fifo_route(&route, cmd);

}

/* Add/update ECMP routes to zebra. */
void
ripng_zebra_ipv6_add(struct route_node *rp)
{
    ripng_zebra_ipv6_send(rp, IPC_OPCODE_ADD);
}

/* Delete ECMP routes from zebra. */
void
ripng_zebra_ipv6_delete(struct route_node *rp)
{
    ripng_zebra_ipv6_send(rp, IPC_OPCODE_DELETE);
}

/* Add/update ECMP routes to zebra. */
void
ripng_zebra_ipv6_update(struct route_node *rp)
{
    ripng_zebra_ipv6_send(rp, IPC_OPCODE_UPDATE);
}

/* Zebra route add and delete treatment. */
int
ripng_route_read_ipv6(uint8_t opcode , struct route_entry *p_route)
{
    struct prefix_ipv6 p;
    unsigned char plength = 0;
    struct ripng_info rinfo;
    struct ripng_instance *ripng = NULL;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;

    memset(&p, 0, sizeof(struct prefix_ipv6));
    memset(&rinfo, 0, sizeof(struct ripng_info));

    p.family = AF_INET6;
    plength = p_route->prefix.prefixlen;
    p.prefixlen = MIN(IPV6_MAX_PREFIXLEN, plength);
    IPV6_ADDR_COPY(&p.prefix.s6_addr, &p_route->prefix.addr.ipv6);

    //if (IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug(RIPNG_DBG_EVENT, "            %s/%d", inet6_ntoa(p.prefix), p.prefixlen);
    }

    rinfo.type = p_route->nhp[0].protocol;
    SET_FLAG(rinfo.sub_type, RIPNG_ROUTE_REDISTRIBUTE);
    rinfo.external_id = p_route->nhp[0].instance;
    rinfo.external_metric = p_route->nhp[0].cost;
    rinfo.ifindex = p_route->nhp[0].ifindex;
    IPV6_ADDR_COPY(&rinfo.nexthop.s6_addr, &p_route->nhp[0].nexthop.addr.ipv6);
    rinfo.distance =  p_route->nhp[0].distance;


    for (ALL_LIST_ELEMENTS_RO(ripng_list, node, ripng))
    {
        if (ripng->redist_infos)
        {
            for (ALL_LIST_ELEMENTS_RO(ripng->redist_infos, nnode, lrm))
            {
                if (!lrm)
                    continue;

                if (lrm->type == p_route->nhp[0].protocol && lrm->id == p_route->nhp[0].instance)
                {
                    /* Then fetch IPv4 prefixes. */
                    rinfo.id = ripng->id;

                    if (opcode == IPC_OPCODE_ADD)
                    {
                        if (lrm->metric == 17)
                        {
                            rinfo.metric = 0;
                            rinfo.metric_out = 0;
                        }
                        else
                        {
                            rinfo.metric = lrm->metric;
                            rinfo.metric_out = lrm->metric;
                        }

                        ripng_redistribute_add(&rinfo, &p, ripng);
                    }
                    else if (opcode == IPC_OPCODE_DELETE)
                    {
                        ripng_redistribute_delete(&rinfo, &p, ripng);
                    }
                }
            }
        }
    }

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, ripng))
    {
        ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);
    }

    return 0;

}

void
ripng_table_metric_change(struct redist_msg *rm, struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL && listcount(list) != 0)
        {
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                if(rm->id && rinfo->external_id != rm->id)
                {
                    continue;
                }  
							  
				if ((rinfo->type == rm->type)
                        && (rinfo->metric != RIP_METRIC_INFINITY))
                {
                    if (17 == rm->metric)
                    {
                        rinfo->metric = 0;
						rinfo->metric_out = 0;
                    }
                    else
                    {
                        rinfo->metric = rm->metric;
						rinfo->metric_out = rm->metric;
                    }
                }
            }
        }
    }

    ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);
}

DEFUN(ripng_redistribute_instance_type,
		ripng_redistribute_instance_type_cmd,
		"redistribute (ospf6|isis6|ripng) <1-255> {metric <0-15>}",
		REDIST_STR
		OSPF6_STR 
		"Intermediate System to Intermediate System for ipv6 (IS-IS6)\n" \
		"Routing Information Protocol next generation (RIPng)\n"	 \
		"instance number 1-255\n"				   \
		"redistribute metric, range 0-15\n" 		\
		"redistribute metric value\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *rm = NULL;
    u_int32 i;
    u_int32_t metric = 17;
    u_int32_t old_metric = 17;
    u_char id = 0;

    if (!ripng)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    if (argv[1] != NULL)
    {
        id = (u_char)atoi(argv[1]);
    }

    if (argv[2] != NULL)
    {
        metric = atoi(argv[2]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0], rip_redist_type[i].str_min_len) == 0)
        {
			if (rip_redist_type[i].type == ROUTE_PROTO_RIPNG && id == ripng->id)
			{
				vty_error_out(vty, "Can`t redistribtue itself %s", VTY_NEWLINE);
				return CMD_WARNING;
			}
            for (ALL_LIST_ELEMENTS(ripng->redist_infos, node, nnode, rm))
            {
                if ((rm->type == rip_redist_type[i].type) && (rm->id == id))
                {
                    break;
                }
            }

			if (rm)
			{
			    if (rm->metric == metric)
			    {
			        if (argv[2])
			        {
			            vty_error_out(vty, "Already existed redistribute %s %d metric %d%s",
			                    rip_redist_type[i].str, rm->id, rm->metric, VTY_NEWLINE);
			        }
			        else
			        {
			            vty_error_out(vty, "Already existed redistribute %s%s",
			                    argv[0], VTY_NEWLINE);
			        }

			        return CMD_SUCCESS;
			    }
			    else
			    {
			        old_metric = rm->metric;
			        rm->metric = metric;
			    }
			}
		    else
		    {
		        rm = rip_redist_new();
		        rm->type = rip_redist_type[i].type;
		        rm->id = id;
		        rm->metric = metric;
		        listnode_add(ripng->redist_infos, rm);

		        if (rip_redist_type[i].rcount)
		        {
		            routev6_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
		        }

		        routev6_event_register(rip_redist_type[i].type, MODULE_ID_RIP);

		        rip_redist_type[i].rcount++;
		    }

		    if (metric != old_metric)
		    {
		        ripng_table_metric_change(rm, ripng);
		    }

			return CMD_SUCCESS;
		}
	}

   return CMD_SUCCESS;

}

DEFUN(ripng_redistribute_type,
		ripng_redistribute_type_cmd,
		"redistribute (connected|static|ibgp|ebgp) {metric <0-15>}",
		REDIST_STR
		"Connected routes (directly attached subnet or host)\n" \
		"Statically configured routes\n" \
		"Inter Border Gateway Protocol for ipv6 (IBGP)\n" \
		"Exter Border Gateway Protocol for ipv6 (EBGP)\n" \
		"redistribute metric, range 0-15\n" 		\
		"redistribute metric value\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *rm = NULL;
    u_int32 i;
    u_int32_t metric = 17;
    u_int32_t old_metric = 17;

    if (!ripng)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    if (argv[1] != NULL)
    {
        metric = atoi(argv[1]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0], rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(ripng->redist_infos, node, nnode, rm))
            {
                if (rm->type == rip_redist_type[i].type)
                {
                    break;
                }
            }

			if (rm)
			{
			
				if (rm->metric == metric)
				{
					if (argv[1])
					{
						vty_error_out(vty, "Already existed redistribute %s metric %s%s",
								argv[0], argv[1], VTY_NEWLINE);
					}
					else
					{
						vty_error_out(vty, "Already existed redistribute %s%s",
								argv[0], VTY_NEWLINE);
					}
				
					return CMD_SUCCESS;
				}
				else
				{
					old_metric = rm->metric;
					rm->metric = metric;
				}
			}
			else
			{
				rm = rip_redist_new();
				rm->type = rip_redist_type[i].type;
				rm->id = 0;
				rm->metric = metric;
				listnode_add(ripng->redist_infos, rm);
			
				if (rip_redist_type[i].rcount)
				{
					routev6_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
				}
			
				routev6_event_register(rip_redist_type[i].type, MODULE_ID_RIP);
			
				rip_redist_type[i].rcount++;
			}
			
			if (metric != old_metric)
			{
				ripng_table_metric_change(rm, ripng);
			}

			return CMD_SUCCESS;
        }
    }

    return CMD_SUCCESS;

}

DEFUN(no_ripng_redistribute_instance_type,
		no_ripng_redistribute_instance_type_cmd,
		"no redistribute (ospf6|isis6|ripng) <1-255>",
		NO_STR
		REDIST_STR
		OSPF6_STR 
		"Intermediate System to Intermediate System for ipv6 (IS-IS6)\n" \
		"Routing Information Protocol (RIPng)\n"	  \
		"instance number 1-255\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;
    int i;
    u_char id = 0;

    if (!ripng)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    if (argv[1] != NULL)
    {
        id = (u_char)atoi(argv[1]);
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0],
                    rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(ripng->redist_infos, node, nnode, lrm))
            {
                if (lrm->type == rip_redist_type[i].type && lrm->id == id)
                    break;
            }

            if (lrm)
            {
                ripng_redistribute_withdraw(rip_redist_type[i].type, id, ripng);
                listnode_delete(ripng->redist_infos, lrm);
                rip_redist_free(lrm);
				lrm = NULL;

                if (!rip_redist_type[i].rcount)
                {
                    routev6_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

            }
            else
            {
                vty_error_out(vty, "Can`t find redistribute %s%s",
                        argv[0], VTY_NEWLINE);
                return CMD_WARNING;
            }

            return CMD_SUCCESS;
        }
    }

    return CMD_WARNING;
}


DEFUN(no_ripng_redistribute_type,
		no_ripng_redistribute_type_cmd,
		"no redistribute (connected|static|ibgp|ebgp)",
		NO_STR
		REDIST_STR
		"Connected routes (directly attached subnet or host)\n" \
		"Statically configured routes\n" \
		"Inter Border Gateway Protocol for ipv6 (IBGP)\n" \
		"Exter Border Gateway Protocol for ipv6 (EBGP)\n")

{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    struct listnode *node, *nnode;
    struct redist_msg *lrm = NULL;
    int i;

    if (!ripng)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_WARNING;
    }

    for (i = 0; rip_redist_type[i].str; i++)
    {
        if (strncmp(rip_redist_type[i].str, argv[0],
                    rip_redist_type[i].str_min_len) == 0)
        {
            for (ALL_LIST_ELEMENTS(ripng->redist_infos, node, nnode, lrm))
            {
                if (lrm->type == rip_redist_type[i].type)
                    break;
            }

            if (lrm)
            {
                ripng_redistribute_withdraw(rip_redist_type[i].type, 0, ripng);
                listnode_delete(ripng->redist_infos, lrm);
                rip_redist_free(lrm);
				lrm = NULL;

                if (!rip_redist_type[i].rcount)
                {
                    routev6_event_unregister(rip_redist_type[i].type, MODULE_ID_RIP);
                }

            }
            else
            {
                vty_error_out(vty, "can`t find redistribute %s%s",
                        argv[0], VTY_NEWLINE);
                return CMD_WARNING;
            }

            return CMD_SUCCESS;
        }
    }

    return CMD_WARNING;
}

int
config_write_rip_redistribute(struct vty *vty, struct rip_instance *rip)
{
    struct listnode *node;
    struct redist_msg *rm = NULL;

    for (ALL_LIST_ELEMENTS_RO(rip->redist_infos, node, rm))
    {
        if (!rm)
            continue;

        if (rm->type == ROUTE_PROTO_OSPF || rm->type == ROUTE_PROTO_ISIS || rm->type == ROUTE_PROTO_RIP)
        {
            if (rm->metric != 17)
                vty_out(vty, " redistribute %s %d metric %d%s",
                        rip_route_string(rm->type), rm->id, rm->metric, VTY_NEWLINE);
            else
                vty_out(vty, " redistribute %s %d%s",
                        rip_route_string(rm->type), rm->id, VTY_NEWLINE);
        }
        else
        {
            if (rm->metric != 17)
                vty_out(vty, " redistribute %s metric %d%s",
                        rip_route_string(rm->type), rm->metric, VTY_NEWLINE);
            else
                vty_out(vty, " redistribute %s %s",
                        rip_route_string(rm->type), VTY_NEWLINE);

        }

    }

    return 0;
}

int
config_write_ripng_redistribute(struct vty *vty, struct ripng_instance *ripng)
{
    struct listnode *node;
    struct redist_msg *rm = NULL;

    for (ALL_LIST_ELEMENTS_RO(ripng->redist_infos, node, rm))
    {
        if (!rm)
            continue;

        if (rm->type == ROUTE_PROTO_OSPF6 || rm->type == ROUTE_PROTO_ISIS6 || rm->type == ROUTE_PROTO_RIPNG)
        {
            if (rm->metric != 17)
                vty_out(vty, " redistribute %s %d metric %d%s",
                        rip_route_string(rm->type), rm->id, rm->metric, VTY_NEWLINE);
            else
                vty_out(vty, " redistribute %s %d%s",
                        rip_route_string(rm->type), rm->id, VTY_NEWLINE);
        }
        else
        {
            if (rm->metric != 17)
                vty_out(vty, " redistribute %s metric %d%s",
                        rip_route_string(rm->type), rm->metric, VTY_NEWLINE);
            else
                vty_out(vty, " redistribute %s %s",
                        rip_route_string(rm->type), VTY_NEWLINE);

        }

    }

    return 0;
}

void
rip_zebra_init(void)
{
    /* Install command elements to rip node. */
    install_element(RIP_NODE, &rip_redistribute_instance_type_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_redistribute_instance_type_cmd, CMD_SYNC);
    install_element(RIP_NODE, &rip_redistribute_type_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_redistribute_type_cmd, CMD_SYNC);

}

void
ripng_zebra_init(void)
{
    install_element(RIPNG_NODE, &ripng_redistribute_instance_type_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &no_ripng_redistribute_instance_type_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &ripng_redistribute_type_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &no_ripng_redistribute_type_cmd, CMD_SYNC);
}
