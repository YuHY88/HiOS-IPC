/* RIP version 1 and 2.
 * Copyright (C) 2005 6WIND <alain.ritoux@6wind.com>
 * Copyright (C) 1997, 98, 99 Kunihiro Ishiguro <kunihiro@zebra.org>
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
#define QUAGGA_DEFINE_DESC_TABLE

#include <prefix.h>
#include <lib/table.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/stream.h>
#include <lib/routemap.h>
#include <lib/if_rmap.h>
#include <lib/plist.h>
#include <lib/md5.h>
#include <lib/checksum.h>
#include <lib/keychain.h>
#include <lib/privs.h>
#include <lib/module_id.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include "lib/errcode.h"

#include "ripd/ripd.h"
#include "ripd/rip_debug.h"
#include "ripd/rip_interface.h"
#include "ripd/rip_ipc.h"
#include "ripd/rip_msg.h"
#include "ripngd/ripngd.h"

/* privileges global */
//extern struct zebra_privs_t ripd_privs;

/* all fifos, send stream,send to route thread and pkt receive thread */
struct rip_fifo_master rip_m;

/* There is only one rip strucutre. */
struct list *rip_list = NULL;
struct stats rip_stats;

/* RIP route changes. */
long rip_global_route_add = 0;
long rip_global_route_del = 0;

/* RIP queries. */
long rip_global_queries = 0;

/* Prototypes. */
size_t rip_auth_md5_ah_write(struct stream *s, struct rip_interface *ri,
                      struct key *key);
size_t rip_auth_header_write(struct stream *s, struct rip_interface *ri,
                      struct key *key, char *auth_str, int len);
void rip_auth_md5_set(struct stream *s, struct rip_interface *ri, size_t doff,
                 char *auth_str, int authlen);
void rip_if_rmap_update(struct if_rmap *if_rmap);
void rip_routemap_update_redistribute(void);
void rip_routemap_update(const char *notused);
int rip_auth_simple_password(struct rte *rte, struct sockaddr_in *from,
                         struct interface *ifp);
int rip_auth_md5(struct rip_packet *packet, struct sockaddr_in *from,
             int length, struct interface *ifp);
void rip_auth_prepare_str_send(struct rip_interface *ri, struct key *key,
                          char *auth_str, int len);
void rip_auth_simple_write(struct stream *s, char *auth_str, int len);


/* RIP command strings. */
static const struct message rip_msg[] =
{
    {RIP_REQUEST,    "REQUEST"},
    {RIP_RESPONSE,   "RESPONSE"},
    {RIP_TRACEON,    "TRACEON"},
    {RIP_TRACEOFF,   "TRACEOFF"},
    {RIP_POLL,       "POLL"},
    {RIP_POLL_ENTRY, "POLL ENTRY"},
    {0, NULL},
};

/* Return remain time in second. */
static unsigned long
rip_thread_timer_remain_second (TIMERID thread, u_int32_t timer_val)
{
	int pass_time = 0;

	high_pre_timer_passtime(thread, &pass_time);

	if(timer_val - pass_time > 0)
		return (timer_val - pass_time);
	else
		return 0;
}

/* ecmp route range: summary route, interface route, learn route, redistribute route */
int
rip_ecmp_route_cmp(struct rip_info *ri1, struct rip_info *ri2)
{

    if (ri1->sub_type > ri2->sub_type)
        return -1;
    else
        return 1;
}

/* check whether the rotue belongs to rip or not */
int
rip_route_rte(struct rip_info *rinfo)
{
    return (rinfo->type == ROUTE_PROTO_RIP
            && (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
                || CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE)));
}

struct rip_info *
rip_info_new(void)
{
	struct rip_info *rip_info_temp = NULL;

	do
	{
    	rip_info_temp = (struct rip_info *)XCALLOC(MTYPE_RIP_INFO, sizeof(struct rip_info));
		if (rip_info_temp == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(rip_info_temp == NULL);
	
	memset(rip_info_temp, 0, sizeof(struct rip_info));
	
	return rip_info_temp;
}

void
rip_info_free(struct rip_info *rinfo)
{
    XFREE(MTYPE_RIP_INFO, rinfo);
}

/* Timeout RIP routes. */
int
rip_timeout(void *t)

{
    struct rip_info *rinfo;
    struct rip_instance *rip;
    rinfo = (struct rip_info *)(t);

    if (!rinfo)
        return 0;

    rinfo->t_timeout = 0;


    rip = rip_instance_lookup(rinfo->id);

    if (!rip)
        return 0;

    rip_ecmp_delete(rinfo, rip);
    return 0;
}

void
rip_timeout_update(struct rip_info *rinfo, struct rip_instance *rip)
{
    rinfo->rx_packets = rip_stats.rx_packets;
    if (rinfo->metric != RIP_METRIC_INFINITY)
    {
        RIP_TIMER_OFF(rinfo->t_timeout);
        RIP_TIMER_OFF(rinfo->t_garbage_collect);
        RIP_TIMER_ON(rinfo->t_timeout, rip_timeout, rip->timeout_time);
    }
	
}

/* RIP route garbage collect timer. */
int
rip_garbage_collect(void *t)
{
    struct rip_info *rinfo = NULL;
    struct route_node *rp = NULL;

    rinfo = (struct rip_info *)(t);

    if (!rinfo)
        return 0;

    rinfo->t_garbage_collect = 0;

    /* Off timeout timer. */
    RIP_TIMER_OFF(rinfo->t_timeout);

    /* Get route_node pointer. */
    rp = rinfo->rp;

    if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS))
    {
        return 0;
    }

    /* Unlock route_node. */
    listnode_delete((struct list *)rp->info, rinfo);

    if (list_isempty((struct list *)rp->info))
    {
        list_free((struct list *)rp->info);
        rp->info = NULL;
        route_unlock_node(rp);
    }
    /* Free RIP routing information. */
    rip_info_free(rinfo);
	rinfo = NULL;
	
    return 0;
}




/* Check nexthop address validity. */
int
rip_ip_address_self_check(struct in_addr *addr)
{
    struct listnode *node;
    struct listnode *cnode;
    struct interface *ifp;
    struct connected *ifc;
    struct prefix *p;

    /* If nexthop address matches local configured address then it is
       invalid nexthop. */
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        for (ALL_LIST_ELEMENTS_RO(ifp->connected, cnode, ifc))
        {
            p = ifc->address;

            if (p->family == AF_INET
                    && IPV4_ADDR_SAME(&p->u.prefix4, addr))
                return -1;
        }
    }

    return 0;
}


/* Check if the destination address is valid (unicast; not net 0
   or 127) (RFC2453 Section 3.9.2 - Page 26).  But we don't
   check net 0 because we accept default route. */
int
rip_destination_check(struct in_addr addr)
{
    u_int32_t destination;

    /* Convert to host byte order. */
    destination = ntohl(addr.s_addr);

    if (IPV4_NET127(destination))
        return 0;

    /* Net 0 may match to the default route. */
    if (IPV4_NET0(destination) && destination != 0)
        return 0;

    /* Unicast address must belong to class A, B, C. */
    if (IN_CLASSD(destination))
        return 0;

    if (IN_EXPERIMENTAL(destination))
        return 0;

    if (destination == INADDR_BROADCAST)
        return 0;

    return 1;
}

/* Utility function to convert ipv4 prefixes to Classful prefixes */
void
rip_apply_classful_mask_ipv4(struct prefix_ipv4 *p)
{

    u_int32_t destination;

    destination = ntohl(p->prefix.s_addr);

    //if (p->prefixlen == IPV4_MAX_PREFIXLEN);
    /* do nothing for host routes */
    if (IN_CLASSC(destination))
    {
        p->prefixlen = 24;
        apply_mask_ipv4(p);
    }
    else if (IN_CLASSB(destination))
    {
        p->prefixlen = 16;
        apply_mask_ipv4(p);
    }
    else
    {
        p->prefixlen = 8;
        apply_mask_ipv4(p);
    }
}

/* Utility function to convert ipv4 prefixes to Classful prefixes */
int
check_plens_equal(struct prefix *p)
{

    u_int32_t destination = ntohl(p->u.prefix4.s_addr);

    if (IN_CLASSA(destination))
    {
        if (p->prefixlen == 8)
            return 1;

        if (p->prefixlen < 8)
            return 2;
    }
    else if (IN_CLASSB(destination))
    {
        if (p->prefixlen == 16)
            return 1;

        if (p->prefixlen < 16)
            return 2;
    }
    else if (IN_CLASSC(destination))
    {
        if (p->prefixlen == 24)
        {
            return 1;
        }

        if (p->prefixlen < 24)
        {
            return 2;
        }
    }

    return 0;
}


/* find route belongs to atuo summary and doesn`t belong to interface summary, return 1 if found, else return 0
rp: auto summary prefix
sump: interface summary*/
int
rip_auto_summary_check_sub_routes(struct route_node *rp, struct prefix *sump, ifindex_t ifindex)
{
    struct route_node *node = NULL;
    struct list *list = NULL;
    struct rip_info *rinfo = NULL;
    struct listnode *listnode;

	if(rp == NULL)
	{
		zlog_err("%s %d rp is NULL",__func__,__LINE__);
		return -1;
	}

    route_lock_node(rp);
    node = rp;

    while (node)
    {
        if (node->info != NULL && prefix_match(&rp->p, &node->p)
                && (!sump || !prefix_match(sump, &node->p)))
        {
            list = (struct list *)node->info;

            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
				if(CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE) 
						&& rinfo->ifindex == ifindex)
				{
					continue;
				}				
				else if(CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
				{
					continue;
				}
				else
				{
					route_unlock_node(node);
					return 1;
				}
            }
        }

        node = route_next(node);
    }

    return 0;
}

/* find summary route wheather has its sub  route, return 1 if has, else return 0*/
int
rip_summary_check_sub_routes(struct route_node *rp)
{
    struct route_node *node = NULL;
    struct list *list = NULL;
    struct rip_info *rinfo = NULL;
    struct listnode *listnode;
	
	if(rp == NULL)
	{
		zlog_err("%s %d rp is NULL",__func__,__LINE__);
		return -1;
	}

    route_lock_node(rp);
    node = rp;

    while (node)
    {
        if (node->info != NULL && prefix_match(&rp->p, &node->p))
        {
            list = (struct list *)node->info;

            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                if (CHECK_FLAG(rinfo->sub_type, ~RIP_ROUTE_SUMMARY))
                {
                    route_unlock_node(node);
                    return 1;
                }
            }
        }

        node = route_next(node);
    }

    return 0;
}

/* check the summary prefix through all the interface to find the prefix which include this prefix p exist, return routenode in  rip table if existed
p: the route to be checked
*/
struct route_node *
rip_inter_summary_prefix_match(struct prefix *p)
{
    struct interface *ifp;
    struct rip_interface *ri;
    struct rip_summary *rs;
    struct listnode *node, *nnode;

    /* Check each interface. */
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        if (((ri = (struct rip_interface *)ifp->info) != NULL) && ri->summary)
        {
            for (ALL_LIST_ELEMENTS_RO(ri->summary, nnode, rs))
            {
                if ((!rs->dflag) && (rs->uflag) && (prefix_match(&rs->p, p)))
                {
                    return rs->tablenode;
                }
            }
        }
    }

    return NULL;
}

/* Lookup the node which included the given data. */
struct listnode *
ri_summary_prefix_match_lookup(struct list *list, struct prefix *p)
{
    struct listnode *node = NULL;
    struct rip_summary *rs = NULL;

    if (!listcount(list))
        return NULL;

    for (ALL_LIST_ELEMENTS_RO(list, node, rs))
    {
        if ((prefix_match(&rs->p, p)) && (!prefix_same(&rs->p, p)) && (!rs->dflag) && (rs->uflag))
        {
            return node;
        }
    }

    return NULL;
}


/* Lookup the node which has the given data. */
struct listnode *
interface_summary_prefix_same_lookup(struct list *list, struct prefix *p)
{
    struct listnode *node = NULL;
    struct rip_summary *rs = NULL;

    if (!listcount(list))
        return NULL;

    for (ALL_LIST_ELEMENTS_RO(list, node, rs))
    {
        if (prefix_same(&rs->p, p) && !rs->dflag && rs->uflag)
        {
            return node;
        }
    }

    return NULL;
}

/* set rip summary .  flag 1 add, 0 delete, 2 replace */
void
rip_summary_all_check(struct rip_info *rinfo_new, struct route_node *rp, u_int32_t opflag, u_int32_t metric, struct rip_instance *rip)
{
    struct route_node *sp = NULL;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct prefix classfull;
    u_int32_t ifmt [2];
    int ret = 0;

    ifmt[0] = 0;
    ifmt[1] = 0;

    ret = check_plens_equal(&rp->p);

    if (rinfo_new)
    {
        if (ret == 2)
        {
            SET_FLAG(rinfo_new->summary_flag, RIP_SUMMARY_SUPER);
            return;
        }

        memcpy(&classfull, &rp->p, sizeof(struct prefix));

        if (ret == 1)
        {
            SET_FLAG(rinfo_new->summary_flag, RIP_SUMMARY_AUTO);
        }
        else
        {
            SET_FLAG(rinfo_new->summary_flag, RIP_SUMMARY_SUB_AUTO);
            rip_apply_classful_mask_ipv4((struct prefix_ipv4 *)&classfull);
        }
    }
    else
    {
		//if(IS_RIP_DEBUG_EVENT)
		{
        	zlog_err("%s %d rinfo_new is null, opflag %d", __func__, __LINE__, opflag);
		}
    }

    sp = route_node_lookup(rip->table, &classfull);



    if (1 == opflag)
    {
        if (sp && sp->info)
        {
            list = sp->info;
            rinfo = listgetdata(listhead(list));

            if (!CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
            {
                rinfo = rip_add_new(&classfull, rip, 1);
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }

                RIP_TIMER_OFF(rinfo->t_timeout);
                RIP_TIMER_OFF(rinfo->t_garbage_collect);

            if (rinfo->metric_out == rinfo_new->metric)
            {
                rinfo->mmt_count++;
                rinfo->metric = rinfo->metric_out;
            }
            else if (rinfo->metric_out > rinfo_new->metric)
            {
                rinfo->mmt_count = 1;
                rinfo->metric_out = rinfo_new->metric;
                rinfo->metric = rinfo_new->metric;
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }

            rinfo->metric = rinfo->metric_out;
        }
        else
        {
            rinfo = rip_add_new(&classfull, rip, 1);
            rinfo->ifindex = rinfo_new->ifindex;
            rinfo->metric = rinfo_new->metric;
            rinfo->metric_out = rinfo_new->metric;
            rinfo->mmt_count = 1;
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
        }
    }
    else if (0 == opflag)
    {
        if (sp && sp->info)
        {

            list = sp->info;
            rinfo = listgetdata(listhead(list));


            if ((rinfo->metric_out == metric) && (0 != rinfo->mmt_count))
            {
                if (rinfo->mmt_count > 1)
                {
                    rinfo->mmt_count--;
                }
                else if (rinfo->mmt_count == 1)
                {
                    rinfo->metric_out = RIP_METRIC_INFINITY;
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

            if (rinfo->mmt_count == 0)
            {
                rinfo->metric = RIP_METRIC_INFINITY;
                rinfo->metric_out = RIP_METRIC_INFINITY;

                    RIP_TIMER_OFF(rinfo->t_timeout);
                    RIP_TIMER_OFF(rinfo->t_garbage_collect);

                RIP_TIMER_ON(rinfo->t_garbage_collect,
                             rip_garbage_collect, rip->garbage_time);
				
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }			

        }
    }
    else if (2 == opflag)
    {
        if (sp && sp->info)
        {
            list = sp->info;
            rinfo = listgetdata(listhead(list));

			if(rinfo_new->metric == metric)
			{
				zlog_err("replace err, old metric == new metric\n");
			}
//			printf("old metric %d, new metric %d\n", metric, rinfo_new->metric);

			if((rinfo->metric_out == RIP_METRIC_INFINITY) && (rinfo->mmt_count == 0))
			{
				RIP_TIMER_OFF(rinfo->t_timeout);
				RIP_TIMER_OFF(rinfo->t_garbage_collect);
				rinfo->metric_out = rinfo_new->metric;
				rinfo->metric = rinfo_new->metric;
				rinfo->mmt_count++;
				route_unlock_node(sp);
				return;
			}
			
			if(rinfo_new->metric == rinfo->metric_out)
			{
				rinfo->mmt_count++;
			}			
            else if (rinfo_new->metric < rinfo->metric_out )
            {
                rinfo->mmt_count = 1;
                rinfo->metric_out = rinfo_new->metric;
                rinfo->metric = rinfo_new->metric;
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }

            if ((rinfo->metric_out == metric) && (0 != rinfo->mmt_count))
            {
                if (rinfo->mmt_count > 1)
                {
                    rinfo->mmt_count--;
                }
                else if (rinfo->mmt_count == 1)
                {
                    rinfo->metric_out = RIP_METRIC_INFINITY;
					
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

            if (rinfo->mmt_count == 0)
            {
                rinfo->metric = RIP_METRIC_INFINITY;
                rinfo->metric_out = RIP_METRIC_INFINITY;

                    RIP_TIMER_OFF(rinfo->t_timeout);
                    RIP_TIMER_OFF(rinfo->t_garbage_collect);

                RIP_TIMER_ON(rinfo->t_garbage_collect,
                             rip_garbage_collect, rip->garbage_time);
				
                SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            }
			

        }

    }

	if(sp != NULL)
	{
		route_unlock_node(sp);
	}

}

void
rip_summary_count_check(struct rip_info *rinfo_new, struct route_node *rp, u_int32_t opflag, u_int32_t metric, struct rip_instance *rip)
{
    struct route_node *sp = NULL;
    struct rip_info *rinfo = NULL;
    struct list *list;
    u_int32_t ifmt[2];

    /*set rip summary flag accroding to summary routes*/
    sp = rip_inter_summary_prefix_match(&rp->p);

    if (!sp || ((list = sp->info) == NULL))
        return;

    rinfo = listgetdata(listhead(list));

        RIP_TIMER_OFF(rinfo->t_timeout);
        RIP_TIMER_OFF(rinfo->t_garbage_collect);

    if (1 == opflag)
    {
        if (rinfo->metric_out == rinfo_new->metric)
        {
            rinfo->mmt_count++;
            rinfo->metric = rinfo->metric_out;
        }
        else if (rinfo->metric_out > rinfo_new->metric)
        {
            rinfo->mmt_count = 1;
            rinfo->metric_out = rinfo_new->metric;
            rinfo->metric = rinfo->metric_out;
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
        }

        rinfo->metric = rinfo->metric_out;
    }
    else if (0 == opflag)
    {
        if (rinfo->metric_out == metric)
        {
            if (rinfo->mmt_count > 1)
            {
                rinfo->mmt_count--;
            }
            else if (rinfo->mmt_count == 1)
            {
                rinfo->metric_out = RIP_METRIC_INFINITY;
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

        if (rinfo->mmt_count == 0)
        {
            rinfo->metric = RIP_METRIC_INFINITY;
            rinfo->metric_out = RIP_METRIC_INFINITY;

            RIP_TIMER_ON(rinfo->t_garbage_collect,
                         rip_garbage_collect, rip->garbage_time);
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
        }
    }
    else if (2 == opflag)
    {
		if(rinfo_new->metric == metric)
		{
			zlog_err("replace err, old metric == new metric");
		}
		
		if((rinfo->metric_out == RIP_METRIC_INFINITY) && (rinfo->mmt_count == 0))
		{
			RIP_TIMER_OFF(rinfo->t_timeout);
			RIP_TIMER_OFF(rinfo->t_garbage_collect);
			rinfo->metric_out = rinfo_new->metric;
			rinfo->metric = rinfo_new->metric;
			rinfo->mmt_count++;
			return;
		}

		if(rinfo_new->metric == rinfo->metric_out)
		{
			rinfo->mmt_count++;
		}
		else if (rinfo_new->metric < rinfo->metric_out )
		{
			rinfo->mmt_count = 1;
			rinfo->metric_out = rinfo_new->metric;
			rinfo->metric = rinfo_new->metric;
			SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
		}

        if (rinfo->metric_out == metric)
        {
            if (rinfo->mmt_count > 1)
            {
                rinfo->mmt_count--;
            }
            else if (rinfo->mmt_count == 1)
            {
                rinfo->metric_out = RIP_METRIC_INFINITY;
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

        if (rinfo->mmt_count == 0)
        {
            rinfo->metric = RIP_METRIC_INFINITY;
            rinfo->metric_out = RIP_METRIC_INFINITY;

            RIP_TIMER_ON(rinfo->t_garbage_collect,
                         rip_garbage_collect, rip->garbage_time);
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
        }
    }

}

/* Add new route to the ECMP list.
 * RETURN: the new entry added in the list, or NULL if it is not the first
 *         entry and ECMP is not allowed.
 */
struct rip_info *
rip_ecmp_add(struct rip_info *rinfo_new, struct rip_instance *rip)
{
    struct route_node *rp = rinfo_new->rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;

    if (rp->info == NULL)
    {
        rp->info = list_new();
        list = (struct list *)rp->info;
        list->cmp = (int (*)(void *, void *))rip_ecmp_route_cmp;
    }

    list = (struct list *)rp->info;

    if (listcount(list))
    {
        rinfo = (struct rip_info *)listgetdata(listhead(list));

        if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
        {
            if (listcount(list) > 1)
                rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
            else
                rinfo = NULL;
        }

//        if (rinfo && rip_route_rte(rinfo) && CHECK_FLAG(rinfo->flags, RIP_RTF_FIB))
//        {
//            rip_zebra_ipv4_delete(rp);
//        }
    }

    rinfo = rip_info_new();
    memcpy(rinfo, rinfo_new, sizeof(struct rip_info));
    listnode_add_sort(list, rinfo);

    if (rip_route_rte(rinfo))
    {
        if (rinfo->type == ROUTE_PROTO_RIP && (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)))
        {
            rip_timeout_update(rinfo, rip);
			
        }

        rip_zebra_ipv4_add(rp);
    }

    rip_summary_all_check(rinfo, rp, 1, rinfo->metric, rip);

    if (rip->inter_sum_count)
        rip_summary_count_check(rinfo_new, rp, 1, rinfo->metric, rip);

    /* Set the route change flag on the first entry. */
    rinfo = listgetdata(listhead(list));
    SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);


    /* Signal the output process to trigger an update (see section 2.5). */
//  rip_event (RIP_TRIGGERED_UPDATE, rip);
    rip->triggered_update_flag = 1;

    return rinfo;
}

/* Replace the ECMP list with the new route.
 * RETURN: the new entry added in the list
 */
struct rip_info *
rip_ecmp_replace(struct rip_info *rinfo_new, struct rip_instance *rip)
{
    struct route_node *rp = rinfo_new->rp;
    struct list *list = (struct list *)rp->info;
    struct rip_info *rinfo = NULL, *tmp_rinfo = NULL;
    struct listnode *node = NULL, *nextnode = NULL;
    u_int32_t metric = RIP_METRIC_INFINITY+1;

    if (list == NULL || listcount(list) == 0)
        return rip_ecmp_add(rinfo_new, rip);

    /* Get the first entry */
    rinfo = (struct rip_info *)listgetdata(listhead(list));

    if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
    {
        if (listcount(list) > 1)
            rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
        else
            rinfo = NULL;
    }

	if(rinfo)
	{
		metric = rinfo->metric;
	}
		
    /* Learnt route replaced by a local one. Delete it from zebra. */
    if (rinfo && rip_route_rte(rinfo) &&
            (rinfo->metric != RIP_METRIC_INFINITY)
            && CHECK_FLAG(rinfo->flags, RIP_RTF_FIB))
    {
        rip_zebra_ipv4_delete(rp);
    }

    /* Re-use the first entry, and delete the others. */
    for (ALL_LIST_ELEMENTS(list, node, nextnode, tmp_rinfo))
    {
        if (!tmp_rinfo)
            continue;

        if (CHECK_FLAG(tmp_rinfo->sub_type, ~RIP_ROUTE_SUMMARY))
        {
            RIP_TIMER_OFF(tmp_rinfo->t_timeout);
            RIP_TIMER_OFF(tmp_rinfo->t_garbage_collect);
            list_delete_node(list, node);
            rip_info_free(tmp_rinfo);
			tmp_rinfo = NULL;
        }
    }

    rinfo = rip_info_new();
    memcpy(rinfo, rinfo_new, sizeof(struct rip_info));
    listnode_add_sort(list, rinfo);

    if (rip_route_rte(rinfo))
    {
        if (rinfo->type == ROUTE_PROTO_RIP && (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)))
        {
            rip_timeout_update(rinfo, rip);
        }

        rip_zebra_ipv4_add(rp);
    }

	if(metric == RIP_METRIC_INFINITY+1)
	{
		rip_summary_all_check(rinfo, rp, 1, rinfo->metric, rip);
		if (rip->inter_sum_count)
			rip_summary_count_check(rinfo_new, rp, 1, rinfo->metric, rip);
	}
	else
	{
	    rip_summary_all_check(rinfo, rp, 2, metric, rip);
	    if (rip->inter_sum_count)
	        rip_summary_count_check(rinfo_new, rp, 2, metric, rip);
	}
    /* Set the route change flag. */
    rinfo = listgetdata(listhead(list));
    SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);


    /* Signal the output process to trigger an update (see section 2.5). */
//  rip_event (RIP_TRIGGERED_UPDATE, rip);
    rip->triggered_update_flag = 1;

    return rinfo;
}

/* Delete one route from the ECMP list.
 * RETURN:
 *  null - the entry is freed, and other entries exist in the list
 *  the entry - the entry is the last one in the list; its metric is set
 *              to INFINITY, and the garbage collector is started for it
 */
struct rip_info *
rip_ecmp_delete(struct rip_info *rinfo, struct rip_instance *rip)
{
    struct route_node *rp = rinfo->rp;
    struct list *list = (struct list *)rp->info;
    struct rip_info *nrinfo = NULL;
    u_int32_t metric = rinfo->metric;
	char dflag = 0;

    RIP_TIMER_OFF(rinfo->t_timeout);

    if (listcount(list) > 1)
    {
        nrinfo = (struct rip_info *)listgetdata(listhead(list));

        if (CHECK_FLAG(nrinfo->sub_type, RIP_ROUTE_SUMMARY))
        {
            if (listcount(list) > 2)
            {
                /* Some other ECMP entries still exist. Just delete this entry. */
                RIP_TIMER_OFF(rinfo->t_timeout);
                RIP_TIMER_OFF(rinfo->t_garbage_collect);

                if (rip_route_rte(rinfo) && (CHECK_FLAG(rinfo->flags, RIP_RTF_FIB)))
                {
                    rip_zebra_ipv4_delete(rp);
                }

                listnode_delete(list, rinfo);
				dflag = 1;

                if (listcount(list) > 1)
                    nrinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                else
                    nrinfo = NULL;
            }
            else
            {
                rinfo->metric = RIP_METRIC_INFINITY;

                    RIP_TIMER_OFF(rinfo->t_timeout);
//                    RIP_TIMER_OFF(rinfo->t_garbage_collect);

                RIP_TIMER_ON(rinfo->t_garbage_collect, rip_garbage_collect, rip->garbage_time);

                if (rip_route_rte(rinfo) && (CHECK_FLAG(rinfo->flags, RIP_RTF_FIB)))
                {
                    rip_zebra_ipv4_delete(rp);
                }
            }

        }
        else
        {
            rinfo->metric = RIP_METRIC_INFINITY;
            RIP_TIMER_OFF(rinfo->t_timeout);
            RIP_TIMER_OFF(rinfo->t_garbage_collect);

            if (rip_route_rte(rinfo) && (CHECK_FLAG(rinfo->flags, RIP_RTF_FIB)))
            {
                rip_zebra_ipv4_delete(rp);
            }

            listnode_delete(list, rinfo);
			dflag = 1;

            nrinfo = listgetdata(listhead(list));
        }

        if (nrinfo && rip_route_rte(nrinfo))
        {
            /* The ADD message implies the update. */
            rip_zebra_ipv4_add(rp);
        }
    }
    else
    {

        /* This is the only entry left in the list. We must keep it in
         * the list for garbage collection time, with INFINITY metric. */
        rinfo->metric = RIP_METRIC_INFINITY;

            RIP_TIMER_OFF(rinfo->t_timeout);
			/* bug 52551  ifm send interface down all the time after one night run delete interface script*/
 //           RIP_TIMER_OFF(rinfo->t_garbage_collect);

        RIP_TIMER_ON(rinfo->t_garbage_collect, rip_garbage_collect, rip->garbage_time);

        if (rip_route_rte(rinfo) && (CHECK_FLAG(rinfo->flags, RIP_RTF_FIB)))
            rip_zebra_ipv4_delete(rp);
    }

   	rip_summary_all_check(rinfo, rp, 0, metric, rip);	
    if (rip->inter_sum_count)
    {			
		rip_summary_count_check(rinfo, rp, 0, metric, rip);
    }

	/* delete route which need to delete */
	if(dflag)
	{		
		rip_info_free(rinfo);
		rinfo = NULL;
	}
    /* Set the route change flag on the first entry. */
    rinfo = listgetdata(listhead(list));
    SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);

    /* Signal the output process to trigger an update (see section 2.5). */
    rip->triggered_update_flag = 1;

    return rinfo;
}
#if 0
static int
rip_incoming_filter(struct prefix_ipv4 *p, struct rip_interface *ri)
{
    struct distribute *dist;
    struct access_list *alist;
    struct prefix_list *plist;

    /* Input distribute-list filtering. */
    if (ri->list[RIP_FILTER_IN])
    {
        if (access_list_apply(ri->list[RIP_FILTER_IN],
                              (struct prefix *) p) == FILTER_DENY)
        {
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug("%s/%d filtered by distribute in", inet_ntoa(p->prefix), p->prefixlen);
            }
            return -1;
        }
    }

    if (ri->prefix[RIP_FILTER_IN])
    {
        if (prefix_list_apply(ri->prefix[RIP_FILTER_IN],
                              (struct prefix *) p) == PREFIX_DENY)
        {
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug("%s/%d filtered by prefix-list in", inet_ntoa(p->prefix), p->prefixlen);
            }
            return -1;
        }
    }

    /* All interface filter check. */
    dist = distribute_lookup(NULL);

    if (dist)
    {
        if (dist->list[DISTRIBUTE_IN])
        {
            alist = access_list_lookup(AFI_IP, dist->list[DISTRIBUTE_IN]);

            if (alist)
            {
                if (access_list_apply(alist,
                                      (struct prefix *) p) == FILTER_DENY)
                {
                    if (IS_RIP_DEBUG_PACKET)
                    {
                        zlog_debug("%s/%d filtered by distribute in", inet_ntoa(p->prefix), p->prefixlen);
                    }
                    return -1;
                }
            }
        }

        if (dist->prefix[DISTRIBUTE_IN])
        {
            plist = prefix_list_lookup(AFI_IP, dist->prefix[DISTRIBUTE_IN]);

            if (plist)
            {
                if (prefix_list_apply(plist,
                                      (struct prefix *) p) == PREFIX_DENY)
                {
                    if (IS_RIP_DEBUG_PACKET)
                    {
                        zlog_debug("%s/%d filtered by prefix-list in", inet_ntoa(p->prefix), p->prefixlen);
                    }
                    return -1;
                }
            }
        }
    }

    return 0;
}

static int
rip_outgoing_filter(struct prefix_ipv4 *p, struct rip_interface *ri)
{
    struct distribute *dist;
    struct access_list *alist;
    struct prefix_list *plist;

    if (ri->list[RIP_FILTER_OUT])
    {
        if (access_list_apply(ri->list[RIP_FILTER_OUT],
                              (struct prefix *) p) == FILTER_DENY)
        {
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug("%s/%d is filtered by distribute out", inet_ntoa(p->prefix), p->prefixlen);
            }
            return -1;
        }
    }

    if (ri->prefix[RIP_FILTER_OUT])
    {
        if (prefix_list_apply(ri->prefix[RIP_FILTER_OUT],
                              (struct prefix *) p) == PREFIX_DENY)
        {
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug("%s/%d is filtered by prefix-list out", inet_ntoa(p->prefix), p->prefixlen);
            }
            return -1;
        }
    }

    /* All interface filter check. */
    dist = distribute_lookup(NULL);

    if (dist)
    {
        if (dist->list[DISTRIBUTE_OUT])
        {
            alist = access_list_lookup(AFI_IP, dist->list[DISTRIBUTE_OUT]);

            if (alist)
            {
                if (access_list_apply(alist,
                                      (struct prefix *) p) == FILTER_DENY)
                {
                    if (IS_RIP_DEBUG_PACKET)
                    {
                        zlog_debug("%s/%d filtered by distribute out", inet_ntoa(p->prefix), p->prefixlen);
                    }
                    return -1;
                }
            }
        }

        if (dist->prefix[DISTRIBUTE_OUT])
        {
            plist = prefix_list_lookup(AFI_IP, dist->prefix[DISTRIBUTE_OUT]);

            if (plist)
            {
                if (prefix_list_apply(plist,
                                      (struct prefix *) p) == PREFIX_DENY)
                {
                    if (IS_RIP_DEBUG_PACKET)
                    {
                        zlog_debug("%s/%d filtered by prefix-list out", inet_ntoa(p->prefix), p->prefixlen);
                    }
                    return -1;
                }
            }
        }
    }

    return 0;
}
#endif
/* RIP add route to routing table. */
void
rip_rte_process(struct rte *rte, struct sockaddr_in *from,
                struct interface *ifp, unsigned char version, struct rip_instance *rip)
{
//    int ret;
    struct prefix_ipv4 p;
    struct route_node *rp;
    struct rip_info *rinfo = NULL, *frinfo;
    static struct rip_info newinfo_rte;
    struct rip_interface *ri;
    struct in_addr *nexthop;
    struct list *list = NULL;
    struct listnode *node = NULL;
    unsigned char old_dist, new_dist;
    int same = 0;
    int rcount = 0;
    /* Make prefix structure. */
		
    memset(&p, 0, sizeof(struct prefix_ipv4));
    p.family = AF_INET;
    p.prefix = rte->prefix;
    p.prefixlen = ip_masklen(rte->mask);

    /* Make sure mask is applied. */
    apply_mask_ipv4(&p);
	
    /* Apply input filters. */
    ri = (struct rip_interface *)ifp->info;
	if(ri == NULL)
	{
		zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
		return;
	}

#if 0
    ret = rip_incoming_filter(&p, ri);

    if (ret < 0)
    {
        return;
    }
#endif
    memset(&newinfo_rte, 0, sizeof(struct rip_info));
    newinfo_rte.type = ROUTE_PROTO_RIP;
    SET_FLAG(newinfo_rte.sub_type, RIP_ROUTE_RTE);
    newinfo_rte.id = ri->enable_inter_node;
    newinfo_rte.external_id = 0;
    newinfo_rte.nexthop = rte->nexthop;
    newinfo_rte.from = from->sin_addr;
    newinfo_rte.ifindex = ifp->ifindex;
    newinfo_rte.metric = rte->metric;
    newinfo_rte.tag = ntohs(rte->tag);    /* XXX */
    newinfo_rte.metric_out = rte->metric; /* XXX */
    newinfo_rte.rx_packets = rip_stats.rx_packets;
#if 0
    /* Modify entry according to the interface routemap. */
    if (ri->routemap[RIP_FILTER_IN])
    {
        int ret;

        /* The object should be of the type of rip_info */
        ret = route_map_apply(ri->routemap[RIP_FILTER_IN],
                              (struct prefix *) & p, RMAP_RIP, &newinfo);

        if (ret == RMAP_DENYMATCH)
        {
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug("RIP %s/%d is filtered by route-map in", inet_ntoa(p.prefix), p.prefixlen);
            }
            return;
        }

        /* Get back the object */
        rte->nexthop = newinfo.nexthop_out;
        rte->tag = htons(newinfo.tag_out);        /* XXX */
        rte->metric = newinfo.metric_out; /* XXX: the routemap uses the metric_out field */
    }
#endif
    /* Once the entry has been validated, update the metric by
       adding the cost of the network on wich the message
       arrived. If the result is greater than infinity, use infinity
       (RFC2453 Sec. 3.9.2) */
    /* Zebra ripd can handle offset-list in. */
//    ret = rip_offset_list_apply_in(&p, ifp, &rte->metric);

    /* If offset-list does not modify the metric use interface's
       metric. */
// if (!ret)
    rte->metric += ifp->metric;

    if (rte->metric > RIP_METRIC_INFINITY)
        rte->metric = RIP_METRIC_INFINITY;

    /* Set nexthop pointer. */
    if (rte->nexthop.s_addr == 0)
    {

        nexthop = &from->sin_addr;
    }
    else
    {
        nexthop = &rte->nexthop;
    }

    /* Check if nexthop address is myself, then do nothing. */
    if (rip_ip_address_self_check(nexthop) < 0)
    {
        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("Nexthop address %s is myself", inet_ntoa(*nexthop));
        }
        return;
    }

    rp = route_node_get(rip->table, (struct prefix *) & p);
    newinfo_rte.rp = rp;
    memcpy(&newinfo_rte.nexthop, nexthop, sizeof(struct in_addr));
    newinfo_rte.metric = rte->metric;
    newinfo_rte.metric_out = rte->metric;
    newinfo_rte.id = rip->id;
    newinfo_rte.summary_flag = 0;
    newinfo_rte.summary_count = 0;
    newinfo_rte.tag = ntohs(rte->tag);
//  newinfo.distance = rip_distance_apply (&newinfo, rip);
    newinfo_rte.distance = rip->distance;
    new_dist = newinfo_rte.distance ? newinfo_rte.distance : ROUTE_METRIC_RIP;

	/* special case. receive a route whose metric is infinity while has no info in the route table*/
	if(rp && rp->info == NULL && rte->metric == RIP_METRIC_INFINITY)
	{
		route_unlock_node(rp);
		return;
	}
	
    /* Check to see whether there is already RIP route on the table. */
    if ((list = (struct list *)rp->info) != NULL)
    {
        for (ALL_LIST_ELEMENTS_RO(list, node, rinfo))
        {
            if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
            {
                continue;
            }

            if (IPV4_ADDR_SAME(&rinfo->from, &from->sin_addr) &&
                    IPV4_ADDR_SAME(&rinfo->nexthop, nexthop))
            {
                break;
            }

            if ((rte->metric != RIP_METRIC_INFINITY) && (rte->metric > rinfo->metric))
            {
                /* New route has a greater metric. Discard it. */
				route_unlock_node(rp);
                return;
            }

            /* Need to compare with redistributed entry or local entry */
            if (!rip_route_rte(rinfo))
            {
                /* New route has a smaller metric. Replace the ECMP list
                 * with the new one in below. */
                if (rte->metric < rinfo->metric)
                    break;

                /* Metrics are same. We compare the distances. */
                old_dist = rinfo->distance ? \
                           rinfo->distance : ROUTE_METRIC_RIP;

                if (new_dist > old_dist)
                {
                    /* New route has a greater distance. Discard it. */
					route_unlock_node(rp);
                    return;
                }

                if (new_dist < old_dist)
                    /* New route has a smaller distance. Replace the ECMP list
                     * with the new one in below. */
                    break;

                /* Metrics and distances are both same. Keep "rinfo" null and
                 * the new route is added in the ECMP list in below. */
            }

            if (!listnextnode(node))
            {
                break;
            }

        }
    }

    if (rinfo)
    {
        /* Redistributed route check. */
        if (rinfo->type != ROUTE_PROTO_RIP
                && rinfo->metric != RIP_METRIC_INFINITY && rte->metric != RIP_METRIC_INFINITY)
        {

            if (rinfo->metric >= newinfo_rte.metric)
            {
                if (rinfo->distance <= newinfo_rte.distance)
                {
                    rip_ecmp_add(&newinfo_rte, rip);

                    /* Update RIP peer. */
 //                   if ((char)rip->version_recv & version)
                        rip_peer_update(rip, from, version, ifp->ifindex);
                }

            }
            else
            {
                if (rinfo->distance > newinfo_rte.distance)
                {
                    rip_ecmp_replace(&newinfo_rte, rip);

                    /* Update RIP peer. */
//                    if ((char)rip->version_recv & version)
                        rip_peer_update(rip, from, version, ifp->ifindex);
                }
            }
			route_unlock_node(rp);
            return;
        }
    }

    if (rinfo == NULL)
    {
		if(rp->info)
		{
			route_unlock_node(rp);
		}
        /* Now, check to see whether there is already an explicit route
           for the destination prefix.  If there is no such route, add
           this route to the routing table, unless the metric is
           infinity (there is no point in adding a route which
           unusable). */
        if (rte->metric != RIP_METRIC_INFINITY)
        {
            rip_ecmp_add(&newinfo_rte, rip);

            /* Update RIP peer. */
 //           if ((char)rip->version_recv & version)
                rip_peer_update(rip, from, version, ifp->ifindex);
        }
    }
    else
    {
        /* Route is there but we are not sure the route is RIP or not. */

        /* If there is an existing route, compare the next hop address
           to the address of the router from which the datagram came.
           If this datagram is from the same router as the existing
           route, reinitialize the timeout.  */
        same = (IPV4_ADDR_SAME(&rinfo->from, &from->sin_addr)
                && (rinfo->ifindex == ifp->ifindex));

        old_dist = rinfo->distance ? \
                   rinfo->distance : ROUTE_METRIC_RIP;

        if (same && (RIP_METRIC_INFINITY == rte->metric)
                && (rinfo->metric != RIP_METRIC_INFINITY)
                && (rinfo->rx_packets == newinfo_rte.rx_packets))
        {
			route_unlock_node(rp);
            return;
        }

        if (!same && (RIP_METRIC_INFINITY != rte->metric) && (rinfo->metric == rte->metric))
        {
            rip_ecmp_add(&newinfo_rte, rip);

            /* Update RIP peer. */
 //           if ((char)rip->version_recv & version)
                rip_peer_update(rip, from, version, ifp->ifindex);
        }

        /* Next, compare the metrics.  If the datagram is from the same
           router as the existing route, and the new metric is different
           than the old one; or, if the new metric is lower than the old
           one, or if the tag has been changed; or if there is a route
           with a lower administrave distance; or an update of the
           distance on the actual route; do the following actions: */
        if ((same && rinfo->metric != rte->metric)
                || (rte->metric < rinfo->metric)
                || ((same)
                    && (rinfo->metric == rte->metric)
                    && (newinfo_rte.tag != rinfo->tag))
                || (old_dist > new_dist)
                || ((old_dist != new_dist) && same))
        {
            frinfo = (struct rip_info *)listgetdata(listhead(list));
            rcount = listcount(list);

            if (CHECK_FLAG(frinfo->sub_type, RIP_ROUTE_SUMMARY))
            {
                rcount--;
            }

            if (rcount == 1)
            {
                if (newinfo_rte.metric != RIP_METRIC_INFINITY)
                {
					rip_ecmp_replace(&newinfo_rte, rip);

                    /* Update RIP peer. */
 //                   if ((char)rip->version_recv & version)
                        rip_peer_update(rip, from, version, ifp->ifindex);
                }
                else
                {
                    rip_ecmp_delete(rinfo, rip);
                }
            }
            else
            {
                if (newinfo_rte.metric < rinfo->metric)
                {
                    rip_ecmp_replace(&newinfo_rte, rip);

                    /* Update RIP peer. */
//                    if ((char)rip->version_recv & version)
                        rip_peer_update(rip, from, version, ifp->ifindex);
                }
                else if (newinfo_rte.metric > rinfo->metric)
                {
                    rip_ecmp_delete(rinfo, rip);
                }
                else if (new_dist < old_dist)
                {
                    rip_ecmp_replace(&newinfo_rte, rip);

                    /* Update RIP peer. */
 //                   if ((char)rip->version_recv & version)
                        rip_peer_update(rip, from, version, ifp->ifindex);
                }
                else if (new_dist > old_dist)
                {
                    rip_ecmp_delete(rinfo, rip);
                }
                else
                {
                    int update = CHECK_FLAG(rinfo->flags, RIP_RTF_FIB) ? 1 : 0;

                    if(newinfo_rte.metric == RIP_METRIC_INFINITY)
                    {
				    	zlog_err("%s %d, rip auth write parameter wrong", __func__, __LINE__);
						route_unlock_node(rp);
				    	return;		
					}
//                    assert(newinfo.metric != RIP_METRIC_INFINITY);

                    RIP_TIMER_OFF(rinfo->t_timeout);
                    RIP_TIMER_OFF(rinfo->t_garbage_collect);
                    memcpy(rinfo, &newinfo_rte, sizeof(struct rip_info));

                    if (same && (rinfo->type == ROUTE_PROTO_RIP) && (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)))
                    {
                        rip_timeout_update(rinfo, rip);
                    }

                    if (update)
                    {
                        rip_zebra_ipv4_add(rp);
                    }

                    /* - Set the route change flag on the first entry. */
                    rinfo = (struct rip_info *)listgetdata(listhead(list));
                    SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
//                  rip_event (RIP_TRIGGERED_UPDATE, rip);
                    rip->triggered_update_flag = 1;
                }
            }
        }
        else /* same & no change */
        {
            if (same && (rte->metric != RIP_METRIC_INFINITY) && (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)))
            {
                rip_timeout_update(rinfo, rip);

//                if ((char)rip->version_recv & version)
                    rip_peer_update(rip, from, version, ifp->ifindex);
            }
        }
		if(rp->info)
		{
			route_unlock_node(rp);
		}

    }

}

/* Dump RIP packet */
void
rip_packet_dump(struct rip_packet *packet, int size, const char *sndrcv)
{
    caddr_t lim;
    struct rte *rte;
    const char *command_str;
    char pbuf[BUFSIZ], nbuf[BUFSIZ];
    u_char netmask = 0;
    u_char *p;

    /* Set command string. */
    if (packet->command > 0 && packet->command < RIP_COMMAND_MAX)
        command_str = lookup(rip_msg, packet->command);
    else
        command_str = "unknown";

    /* Dump packet header. */
    zlog_debug(RIP_DBG_PACKET, "%s %s version %d packet size %d",
               sndrcv, command_str, packet->version, size);

    /* Dump each routing table entry. */
    rte = packet->rte;

    for (lim = (caddr_t) packet + size; (caddr_t) rte < lim; rte++)
    {
        if (packet->version == RIPv2)
        {
            netmask = ip_masklen(rte->mask);

            if (rte->family == htons(RIP_FAMILY_AUTH))
            {
                if (rte->tag == htons(RIP_AUTH_SIMPLE_PASSWORD))
                {
                    p = (u_char *) & rte->prefix;

                    zlog_debug(RIP_DBG_PACKET, "  family 0x%X type %d auth string: %s",
                               ntohs(rte->family), ntohs(rte->tag), p);
                }
                else if (rte->tag == htons(RIP_AUTH_MD5))
                {
                    struct rip_md5_info *md5;

                    md5 = (struct rip_md5_info *) & packet->rte;

                    zlog_debug(RIP_DBG_PACKET, "  family 0x%X type %d (MD5 authentication)",
                               ntohs(md5->family), ntohs(md5->type));
                    zlog_debug(RIP_DBG_PACKET, "    RIP-2 packet len %d Key ID %d"
                               " Auth Data len %d",
                               ntohs(md5->packet_len), md5->keyid,
                               md5->auth_len);
                    zlog_debug(RIP_DBG_PACKET, "    Sequence Number %ld",
                               (u_long) ntohl(md5->sequence));
                }
                else if (rte->tag == htons(RIP_AUTH_DATA))
                {
                    p = (u_char *) & rte->prefix;

                    zlog_debug(RIP_DBG_PACKET, "  family 0x%X type %d (MD5 data)",
                               ntohs(rte->family), ntohs(rte->tag));
                    zlog_debug(RIP_DBG_PACKET, "    MD5: %02X%02X%02X%02X%02X%02X%02X%02X"
                               "%02X%02X%02X%02X%02X%02X%02X",
                               p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                               p[7], p[9], p[10], p[11], p[12], p[13],
                               p[14], p[15]);
                }
                else
                {
                    zlog_debug(RIP_DBG_PACKET, "  family 0x%X type %d (Unknown auth type)",
                               ntohs(rte->family), ntohs(rte->tag));
                }
            }
            else
                zlog_debug(RIP_DBG_PACKET, "  %s/%d -> %s family %d tag %d metric %ld",
                           inet_ntop(AF_INET, &rte->prefix, pbuf, BUFSIZ),
                           netmask, inet_ntop(AF_INET, &rte->nexthop, nbuf,
                                              BUFSIZ), ntohs(rte->family),
                           ntohs(rte->tag), (u_long) ntohl(rte->metric));
        }
        else
        {
            zlog_debug(RIP_DBG_PACKET, "  %s family %d tag %d metric %ld",
                       inet_ntop(AF_INET, &rte->prefix, pbuf, BUFSIZ),
                       ntohs(rte->family), ntohs(rte->tag),
                       (u_long)ntohl(rte->metric));
        }
    }
}

/*  */
void
rip_wrong_size_pkt_dump(void *data, int len)
{
    int i, taillen;
    u_char* pktdata;
    char string[200];
    char tmp[100];

    if (data == NULL)
    {
        return ;
    }

    for (i = 0; i < len / 16; i++)
    {
        pktdata = (unsigned char *)data + i * 16;
        zlog_err("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
                 pktdata[0], pktdata[1], pktdata[2], pktdata[3], pktdata[4], pktdata[5], pktdata[6], pktdata[7],
                 pktdata[8], pktdata[9], pktdata[10], pktdata[11], pktdata[12], pktdata[13], pktdata[14], pktdata[15]);
    }

    taillen = len % 16;

    if (taillen != 0)
    {
        pktdata = (unsigned char *)data + i * 16;
        memset(string, 0, 200);
        memset(tmp, 0, 100);

        for (i = 0; i < taillen; i++)
        {
            if (i == 8)
                sprintf(tmp, " %02x ", pktdata[i]);
            else if (i == taillen - 1)
                sprintf(tmp, "%02x", pktdata[i]);
            else
                sprintf(tmp, "%02x ", pktdata[i]);

            strcat(string, tmp);
        }

        zlog_err("%s", string);
    }

    return ;
}

/* RIP version 2 authentication. */
int
rip_auth_simple_password(struct rte *rte, struct sockaddr_in *from,
                         struct interface *ifp)
{
    struct rip_interface *ri;
    char *auth_str;

    //if (IS_RIP_DEBUG_PACKET)
    {
        zlog_debug(RIP_DBG_PACKET, "RIPv2 simple password authentication from %s",
                   inet_ntoa(from->sin_addr));
    }
    ri = (struct rip_interface *)ifp->info;

    if (ri->auth_type != RIP_AUTH_SIMPLE_PASSWORD
            || rte->tag != htons(RIP_AUTH_SIMPLE_PASSWORD))
        return 0;

    /* Simple password authentication. */
    if (ri->auth_str)
    {
        auth_str = (char *) & rte->prefix;

        if (strncmp(auth_str, ri->auth_str, 16) == 0)
            return 1;
    }

    if (ri->key_chain)
    {
        struct keychain *keychain;
        struct key *key;

        keychain = keychain_lookup(ri->key_chain);

        if (keychain == NULL)
            return 0;

        key = key_match_for_accept(keychain, (char *) & rte->prefix);

        if (key)
            return 1;
    }

    return 0;
}

/* RIP version 2 authentication with MD5. */
int
rip_auth_md5(struct rip_packet *packet, struct sockaddr_in *from,
             int length, struct interface *ifp)
{
    struct rip_interface *ri;
    struct rip_md5_info *md5;
    struct rip_md5_data *md5data;
    struct keychain *keychain;
    struct key *key;
    MD5_CTX ctx;
    u_char digest[RIP_AUTH_MD5_SIZE];
    u_int16_t packet_len;
    char auth_str[RIP_AUTH_MD5_SIZE];

    //if (IS_RIP_DEBUG_PACKET)
    {
        zlog_debug(RIP_DBG_PACKET, "RIPv2 MD5 authentication from %s", inet_ntoa(from->sin_addr));
    }

    ri = (struct rip_interface *)ifp->info;
    md5 = (struct rip_md5_info *) & packet->rte;

    /* Check auth type. */
    if (ri->auth_type != RIP_AUTH_MD5 || md5->type != htons(RIP_AUTH_MD5))
        return 0;

    /* If the authentication length is less than 16, then it must be wrong for
     * any interpretation of rfc2082. Some implementations also interpret
     * this as RIP_HEADER_SIZE+ RIP_AUTH_MD5_SIZE, aka RIP_AUTH_MD5_COMPAT_SIZE.
     */
    if (!((md5->auth_len == RIP_AUTH_MD5_SIZE)
            || (md5->auth_len == RIP_AUTH_MD5_COMPAT_SIZE)))
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "RIPv2 MD5 authentication, strange authentication "
                       "length field %d", md5->auth_len);
        }
        return 0;
    }

    /* grab and verify check packet length */
    packet_len = ntohs(md5->packet_len);

    if (packet_len > (length - RIP_HEADER_SIZE - RIP_AUTH_MD5_SIZE))
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "RIPv2 MD5 authentication, packet length field %d "
                       "greater than received length %d!",
                       md5->packet_len, length);
        }
        return 0;
    }

    if (md5->keyid != ri->md5_auth_id)
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "RIPv2 MD5 authentication, md5 key id authentication failed. own key id %d "
                       "not equal to received key id %d!",
                       ri->md5_auth_id, md5->keyid);
        }
        return 0;
    }

    /* retrieve authentication data */
    md5data = (struct rip_md5_data *)(((u_char *) packet) + packet_len);

    memset(auth_str, 0, RIP_AUTH_MD5_SIZE);

    if (ri->key_chain)
    {
        keychain = keychain_lookup(ri->key_chain);

        if (keychain == NULL)
            return 0;

        key = key_lookup_for_accept(keychain, md5->keyid);

        if (key == NULL)
            return 0;

        strncpy(auth_str, key->string, RIP_AUTH_MD5_SIZE);
    }
    else if (ri->auth_str)
        strncpy(auth_str, ri->auth_str, RIP_AUTH_MD5_SIZE);


    /* MD5 digest authentication. */
    memset(&ctx, 0, sizeof(ctx));
    MD5Init(&ctx);
    MD5Update(&ctx, packet, packet_len + RIP_HEADER_SIZE);
    MD5Update(&ctx, auth_str, RIP_AUTH_MD5_SIZE);
    MD5Final(digest, &ctx);

    if (memcmp(md5data->digest, digest, RIP_AUTH_MD5_SIZE) == 0)
        return packet_len;
    else
        return 0;
}

/* Pick correct auth string for sends, prepare auth_str buffer for use.
 * (left justified and padded).
 *
 * presumes one of ri or key is valid, and that the auth strings they point
 * to are nul terminated. If neither are present, auth_str will be fully
 * zero padded.
 *
 */
void
rip_auth_prepare_str_send(struct rip_interface *ri, struct key *key,
                          char *auth_str, int len)
{
    if(!ri && !key)
    {
    	zlog_err("%s %d, rip auth send parameter wrong", __func__, __LINE__);
    	return;
    }
//    assert(ri || key);

    memset(auth_str, 0, len);

    if (key && key->string)
        strncpy(auth_str, key->string, len);
    else if (ri->auth_str)
        strncpy(auth_str, ri->auth_str, len);

    return;
}

/* Write RIPv2 simple password authentication information
 *
 * auth_str is presumed to be 2 bytes and correctly prepared
 * (left justified and zero padded).
 */
void
rip_auth_simple_write(struct stream *s, char *auth_str, int len)
{
    if(!s || len != RIP_AUTH_SIMPLE_SIZE)
    {
    	zlog_err("%s %d, rip auth write parameter wrong", __func__, __LINE__);
    	return;		
    }
 //   assert(s && len == RIP_AUTH_SIMPLE_SIZE);

    stream_putw(s, RIP_FAMILY_AUTH);
    stream_putw(s, RIP_AUTH_SIMPLE_PASSWORD);
    stream_put(s, auth_str, RIP_AUTH_SIMPLE_SIZE);

    return;
}

/* write RIPv2 MD5 "authentication header"
 * (uses the auth key data field)
 *
 * Digest offset field is set to 0.
 *
 * returns: offset of the digest offset field, which must be set when
 * length to the auth-data MD5 digest is known.
 */
size_t
rip_auth_md5_ah_write(struct stream *s, struct rip_interface *ri,
                      struct key *key)
{
    size_t doff = 0;

		if(!s || !ri || ri->auth_type != RIP_AUTH_MD5)
		{
			zlog_err("%s %d, rip md5 write parameter wrong", __func__, __LINE__);
    		return 0;		
		}
//    assert(s && ri && ri->auth_type == RIP_AUTH_MD5);

    /* MD5 authentication. */
    stream_putw(s, RIP_FAMILY_AUTH);
    stream_putw(s, RIP_AUTH_MD5);

    /* MD5 AH digest offset field.
     *
     * Set to placeholder value here, to true value when RIP-2 Packet length
     * is known.  Actual value is set in .....().
     */
    doff = stream_get_endp(s);
    stream_putw(s, 0);

    /* Key ID. */
    if (key)
        stream_putc(s, key->index % 256);
    else if (ri->md5_auth_id)
    {
        stream_putc(s, ri->md5_auth_id);
    }
    else
        stream_putc(s, 1);

    /* Auth Data Len.  Set 16 for MD5 authentication data. Older ripds
     * however expect RIP_HEADER_SIZE + RIP_AUTH_MD5_SIZE so we allow for this
     * to be configurable.
     */
    stream_putc(s, ri->md5_auth_len);

    /* Sequence Number (non-decreasing). */
    /* RFC2080: The value used in the sequence number is
       arbitrary, but two suggestions are the time of the
       message's creation or a simple message counter. */
    stream_putl(s, time(NULL));

    /* Reserved field must be zero. */
    stream_putl(s, 0);
    stream_putl(s, 0);

    return doff;
}

/* If authentication is in used, write the appropriate header
 * returns stream offset to which length must later be written
 * or 0 if this is not required
 */
size_t
rip_auth_header_write(struct stream *s, struct rip_interface *ri,
                      struct key *key, char *auth_str, int len)
{
	if(ri->auth_type == RIP_NO_AUTH)
	{
		zlog_err("%s %d, rip auth head write parameter wrong", __func__, __LINE__);
		return 0;		
    }
//		assert(ri->auth_type != RIP_NO_AUTH);

    switch (ri->auth_type)
    {
        case RIP_AUTH_SIMPLE_PASSWORD:
            rip_auth_prepare_str_send(ri, key, auth_str, len);
            rip_auth_simple_write(s, auth_str, len);
            return 0;
        case RIP_AUTH_MD5:
            return rip_auth_md5_ah_write(s, ri, key);
    }

    return 0;
}

/* Write RIPv2 MD5 authentication data trailer */
void
rip_auth_md5_set(struct stream *s, struct rip_interface *ri, size_t doff,
                 char *auth_str, int authlen)
{
    unsigned long len;
    MD5_CTX ctx;
    unsigned char digest[RIP_AUTH_MD5_SIZE];

    /* Make it sure this interface is configured as MD5
       authentication. */
    if((ri->auth_type != RIP_AUTH_MD5) || (authlen != RIP_AUTH_MD5_SIZE))
    {
    	zlog_err("%s %d, rip auth write parameter wrong", __func__, __LINE__);
    	return;		
    }
    if(doff <= 0)
    {
    	zlog_err("%s %d, rip auth write parameter wrong", __func__, __LINE__);
    	return;		
    }
//    assert((ri->auth_type == RIP_AUTH_MD5) && (authlen == RIP_AUTH_MD5_SIZE));
//    assert(doff > 0);

    /* Get packet length. */
    len = stream_get_endp(s);

    /* Check packet length. */
    if (len < (RIP_HEADER_SIZE + RIP_RTE_SIZE))
    {
        zlog_err("rip_auth_md5_set(): packet length %ld is less than minimum length.", len);
        return;
    }

    /* Set the digest offset length in the header */
    stream_putw_at(s, doff, len);

    /* Set authentication data. */
    stream_putw(s, RIP_FAMILY_AUTH);
    stream_putw(s, RIP_AUTH_DATA);

    /* Generate a digest for the RIP packet. */
    memset(&ctx, 0, sizeof(ctx));
    MD5Init(&ctx);
    MD5Update(&ctx, STREAM_DATA(s), stream_get_endp(s));
    MD5Update(&ctx, auth_str, RIP_AUTH_MD5_SIZE);
    MD5Final(digest, &ctx);

    /* Copy the digest to the packet. */
    stream_write(s, digest, RIP_AUTH_MD5_SIZE);
}

/* RIP routing information. */
void
rip_response_process(struct rip_packet *packet, int size,
                     struct sockaddr_in *from, struct connected *ifc, struct rip_instance *rip, struct rip_interface *ri)
{
    caddr_t lim;
    struct rte *rte = NULL;
    struct prefix_ipv4 ifaddr;
    struct prefix_ipv4 ifaddrclass;
    struct prefix nexthop;
    int subnetted;
	struct list *list;

    memset(&ifaddr, 0, sizeof(ifaddr));
	memcpy(&ifaddr, ifc->address, sizeof(struct prefix_ipv4));
	memcpy(&ifaddrclass, &ifaddr, sizeof(struct prefix_ipv4));
	rip_apply_classful_mask_ipv4(&ifaddrclass);

    /* We don't know yet. */
    subnetted = -1;

    /* Set RTE pointer. */
    rte = packet->rte;

    for (lim = (caddr_t) packet + size; (caddr_t) rte < lim; rte++)
    {
        /* RIPv2 authentication check. */
        /* If the Address Family Identifier of the first (and only the
        first) entry in the message is 0xFFFF, then the remainder of
        the entry contains the authentication. */
        /* If the packet gets here it means authentication enabled */
        /* Check is done in rip_read(). So, just skipping it */
        if (packet->version == RIPv2 &&
                rte == packet->rte &&
                rte->family == htons(RIP_FAMILY_AUTH))
            continue;

        rip_stats.rx_routes++;

        if (rte->family != htons(AF_INET))
        {
            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
            /* Address family check.  RIP only supports AF_INET. */
			
			//if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_debug(RIP_DBG_PACKET, "Unsupported family %d from %s.",
                	      ntohs(rte->family), inet_ntoa(from->sin_addr));
			}
            continue;
        }

        /* - is the destination address valid (e.g., unicast; not net 0
           or 127) */
        if (! rip_destination_check(rte->prefix))
        {
            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
			
			//if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_debug(RIP_DBG_PACKET, "Network is net 0 or net 127 or it is not unicast network");
			}
            continue;
        }

        /* Convert metric value to host byte order. */
        rte->metric = ntohl(rte->metric);

        /* - is the metric valid (i.e., between 1 and 16, inclusive) */
        if (!(rte->metric >= 1 && rte->metric <= 16))
        {
            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
			//if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_debug(RIP_DBG_PACKET, "Route's metric is not in the 1-16 range.");
			}
            continue;
        }

        /* RIPv1 does not have nexthop value. */
        if (packet->version == RIPv1 && rte->nexthop.s_addr != 0)
        {
            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
			
			//if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_debug(RIP_DBG_PACKET, "RIPv1 packet with nexthop value %s",
                	      inet_ntoa(rte->nexthop));
			}
            continue;
        }


        /* That is, if the provided information is ignored, a possibly
        sub-optimal, but absolutely valid, route may be taken.  If
        the received Next Hop is not directly reachable, it should be
        treated as 0.0.0.0. */
        if (packet->version == RIPv2 && rte->nexthop.s_addr != 0)
        {

            if (!rip_destination_check(rte->nexthop))
            {
                rip_stats.rx_bad_routes++;
				ri->recv_badroutes++;
				//if(IS_RIP_DEBUG_PACKET)
				{
                	zlog_debug(RIP_DBG_PACKET, "Error: The specified nexthop %s is invalid.", inet_ntoa(rte->nexthop));
				}
                continue;
            }

            nexthop.family = AF_INET;
            nexthop.u.prefix4 = rte->nexthop;
            nexthop.prefixlen = 32;

            if (!prefix_match((struct prefix *)&ifaddrclass, &nexthop))
            {
                rip_stats.rx_bad_routes++;
				ri->recv_badroutes++;
				//if(IS_RIP_DEBUG_PACKET)
				{
                	zlog_debug(RIP_DBG_PACKET, "Error: The specified nexthop %s is not connected.", inet_ntoa(rte->nexthop));
				}
                continue;
            }

            if (! if_lookup_address(rte->nexthop))
            {
                struct route_node *rn;
                struct rip_info *rinfo;

                rn = route_node_match_ipv4(rip->table, &rte->nexthop);

                if (rn)
                {
                	if(!rn->info)
                	{
						route_unlock_node(rn);
						continue;
                	}

					list = (struct list *)rn->info;
                    rinfo = (struct rip_info *)listgetdata(listhead(list));
					if(rinfo == NULL)
					{
						zlog_err("%-15s[%d]: rinfo is NULL!", __FUNCTION__, __LINE__);
						continue;
					}
                    if (rinfo->type == ROUTE_PROTO_RIP
                            && CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE))
                    {
                        //if (IS_RIP_DEBUG_PACKET)
                        {
                            zlog_debug(RIP_DBG_PACKET, "Next hop %s is on RIP network.  Set nexthop to the packet's originator", inet_ntoa(rte->nexthop));
                        }
                        rte->nexthop = rinfo->from;
                    }
                    else
                    {
                        //if (IS_RIP_DEBUG_PACKET)
                        {
                            zlog_debug(RIP_DBG_PACKET, "Next hop %s is not directly reachable. Treat it as 0.0.0.0", inet_ntoa(rte->nexthop));
                        }
                        rte->nexthop.s_addr = 0;
                    }
					
                	route_unlock_node(rn);

                }
                else
                {
                    //if (IS_RIP_DEBUG_PACKET)
                    {
                        zlog_debug(RIP_DBG_PACKET, "Next hop %s is not directly reachable. Treat it as 0.0.0.0", inet_ntoa(rte->nexthop));
                    }
                    rte->nexthop.s_addr = 0;
                }

            }
        }

        /* For RIPv1, there won't be a valid netmask.

        This is a best guess at the masks.  If everyone was using old
        Ciscos before the 'ip subnet zero' option, it would be almost
        right too :-)

        Cisco summarize ripv1 advertisments to the classful boundary
        (/16 for class B's) except when the RIP packet does to inside
        the classful network in question.  */

        if ((packet->version == RIPv1 && rte->prefix.s_addr != 0)
                || (packet->version == RIPv2
                    && (rte->prefix.s_addr != 0 && rte->mask.s_addr == 0)))
        {
            u_int32_t destination;

            if (-1 == subnetted)
            {
                subnetted = 0;

                if (ifaddr.prefixlen > ifaddrclass.prefixlen)
                    subnetted = 1;
            }

            destination = ntohl(rte->prefix.s_addr);

            if (IN_CLASSA(destination))
                masklen2ip(8, &rte->mask);
            else if (IN_CLASSB(destination))
                masklen2ip(16, &rte->mask);
            else if (IN_CLASSC(destination))
                masklen2ip(24, &rte->mask);

            if (1 == subnetted)
            {
                masklen2ip(ifaddrclass.prefixlen, (struct in_addr *) &destination);
            }

            if ((1 == subnetted) && ((rte->prefix.s_addr & destination) ==
                                     ifaddrclass.prefix.s_addr))
            {

                if ((rte->prefix.s_addr & rte->mask.s_addr) != rte->prefix.s_addr)
                {
					masklen2ip(ifaddr.prefixlen, &rte->mask);
					if ((rte->prefix.s_addr & rte->mask.s_addr) != rte->prefix.s_addr)
					{
						masklen2ip(IPV4_MAX_BITLEN, &rte->mask);
					}
                }
#if 0			
                masklen2ip(ifaddr.prefixlen, &rte->mask);

                if ((rte->prefix.s_addr & rte->mask.s_addr) != rte->prefix.s_addr)
                    masklen2ip(32, &rte->mask);
#endif
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "Subnetted route %s", inet_ntoa(rte->prefix));
                }
            }
            else
            {
                if ((rte->prefix.s_addr & rte->mask.s_addr) != rte->prefix.s_addr)
                {
                    rip_stats.rx_bad_routes++;
					ri->recv_badroutes++;
					//if (IS_RIP_DEBUG_PACKET)
					{
						zlog_debug(RIP_DBG_PACKET, "prefix and net mask are not corresponding %s", inet_ntoa(rte->prefix));
					}
                    continue;
                }
            }

            //if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug(RIP_DBG_PACKET, "Resultant route %s", inet_ntoa(rte->prefix));
                zlog_debug(RIP_DBG_PACKET, "Resultant mask %s", inet_ntoa(rte->mask));
            }
        }

        /* In case of RIPv2, if prefix in RTE is not netmask applied one
           ignore the entry.  */
        if ((packet->version == RIPv2)
                && (rte->mask.s_addr != 0)
                && ((rte->prefix.s_addr & rte->mask.s_addr) != rte->prefix.s_addr))
        {
            //if (IS_RIP_DEBUG_PACKET)
            {
            	zlog_debug(RIP_DBG_PACKET, "RIPv2 address %s is not mask /%d applied one",
                	      inet_ntoa(rte->prefix), ip_masklen(rte->mask));
            }
            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
            continue;
        }

        /* Default route's netmask is ignored. */
        if (packet->version == RIPv2
                && (rte->prefix.s_addr == 0)
                && (rte->mask.s_addr != 0))
        {
            //if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug(RIP_DBG_PACKET, "Default route with non-zero netmask.  skip this route");
            }

            rip_stats.rx_bad_routes++;
			ri->recv_badroutes++;
            continue;
        }

        /* Routing table updates. */
        rip_rte_process(rte, from, ifc->ifp, packet->version, rip);

    }

    if (rip->triggered_update_flag)
    {
        rip_event(RIP_TRIGGERED_UPDATE, rip);
    }

    rip->triggered_update_flag = 0;
}

/* RIP packet send to destination address, on interface denoted by
 * by connected argument. NULL to argument denotes destination should be
 * should be RIP multicast group
 */
int
rip_send_packet(u_char * buf, int size, struct sockaddr_in *to,
                struct connected *ifc, u_char version)
{
    int ret = 0;
    struct sockaddr_in sin;
    struct iphdr iph;
    struct pseudo_hdr pudph;
    struct udphdr udph;
    struct stream *pdata = NULL;
    union pkt_control pcb;

	if(NULL == ifc)
	{
		zlog_err("%s %d, rip auth write parameter wrong", __func__, __LINE__);
		return 0;		
    }
//    assert(ifc != NULL);

    if (IS_RIP_DEBUG_PACKET)
    {
#define ADDRESS_SIZE 20
        char dst[ADDRESS_SIZE];
        dst[ADDRESS_SIZE - 1] = '\0';

        if (to)
        {
            strncpy(dst, (char *)inet_ntoa(to->sin_addr), ADDRESS_SIZE - 1);
        }
        else
        {
            sin.sin_addr.s_addr = htonl(INADDR_RIP_GROUP);
            strncpy(dst, (char *)inet_ntoa(sin.sin_addr), ADDRESS_SIZE - 1);
        }

#undef ADDRESS_SIZE
        zlog_debug(RIP_DBG_PACKET, "rip_send_packet %s > %s (%s)",
                   inet_ntoa(ifc->address->u.prefix4),
                   dst, ifc->ifp->name);
    }

//  if ( CHECK_FLAG (ifc->flags, ZEBRA_IFA_SECONDARY) )
//    {
    /*
     * ZEBRA_IFA_SECONDARY is set on linux when an interface is configured
     * with multiple addresses on the same subnet: the first address
     * on the subnet is configured "primary", and all subsequent addresses
     * on that subnet are treated as "secondary" addresses.
     * In order to avoid routing-table bloat on other rip listeners,
     * we do not send out RIP packets with ZEBRA_IFA_SECONDARY source addrs.
     * XXX Since Linux is the only system for which the ZEBRA_IFA_SECONDARY
     * flag is set, we would end up sending a packet for a "secondary"
     * source address on non-linux systems.
     */
//      if (IS_RIP_DEBUG_PACKET)
//        zlog_debug("duplicate dropped");
//      return 0;
//    }

    /* Make destination address. */

    memset(&iph, 0, sizeof(struct iphdr));
    memset(&udph, 0, sizeof(struct udphdr));
    memset(&sin, 0, sizeof(struct sockaddr_in));
    memset(&pcb, 0, sizeof(pcb));
    sin.sin_family = AF_INET;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
    sin.sin_len = sizeof(struct sockaddr_in);
#endif /* HAVE_STRUCT_SOCKADDR_IN_SIN_LEN */

    /* When destination is specified, use it's port and address. */
    if (to)
    {
        sin.sin_port = to->sin_port;
        sin.sin_addr = to->sin_addr;
        udph.dport = to->sin_port;
    }
    else
    {
        sin.sin_port = htons(RIP_PORT_DEFAULT);
        udph.dport =  htons(RIP_PORT_DEFAULT);

        if (version == RIPv2 || version == RI_RIP_VERSION_2_MULTI)
        {
            sin.sin_addr.s_addr = htonl(INADDR_RIP_GROUP);
        }
        else
        {
            sin.sin_addr.s_addr = htonl(INADDR_BROAD_GROUP);
        }
    }

    /*** make ip header ***/

    iph.ihl = 5;
    iph.version = 4;
    iph.tos = 3;
    iph.tot_len = htons(size + IP_HEADER_SIZE + UDP_HEADER_SIZE);
    iph.ttl = 1;
    iph.protocol = IP_P_UDP;
    iph.check = 0;
    iph.saddr = ifc->address->u.prefix4.s_addr;
    iph.daddr = sin.sin_addr.s_addr;
    iph.check = in_checksum((u_int16_t *) & iph, sizeof(iph));

    /***make udp check struct and udp header ***/
    udph.checksum = 0x00;
    pudph.saddr = ifc->address->u.prefix4.s_addr;
    pudph.daddr = sin.sin_addr.s_addr;
    pudph.placeholder = 0x00;
    pudph.protocol = IP_P_UDP;
    pudph.length = htons(UDP_HEADER_SIZE + size);
    unsigned short sum = 0x00;
    unsigned char *data;

    udph.sport = htons(RIP_PORT_DEFAULT);
    udph.len = htons(UDP_HEADER_SIZE + size);

    /*** set packet length  ***/
    if (to)
        pdata = stream_new(IP_HEADER_SIZE + UDP_HEADER_SIZE + size);
    else
    {
        pdata = stream_new(ETH_HLEN + IP_HEADER_SIZE + UDP_HEADER_SIZE + size);

        /*** set ip header  ***/
        stream_write(pdata, &iph, sizeof(iph));
    }

    /*** set udp checksum ***/
	do
	{
    	data = (unsigned char *)XMALLOC(MTYPE_RIP_UDP, sizeof(struct pseudo_hdr) + UDP_HEADER_SIZE + size);
		if (data == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(data == NULL);

	memset(data, 0, sizeof(struct pseudo_hdr) + UDP_HEADER_SIZE + size);

    memcpy(data, (unsigned char *)&pudph, sizeof(struct pseudo_hdr));
    memcpy(data + sizeof(struct pseudo_hdr), (unsigned char *)&udph, UDP_HEADER_SIZE);
    memcpy(data + sizeof(struct pseudo_hdr) + UDP_HEADER_SIZE, buf, size);
    sum = in_checksum((uint16_t *)data, sizeof(struct pseudo_hdr) + UDP_HEADER_SIZE + size);
    udph.checksum = sum;
    XFREE(MTYPE_RIP_UDP, data);
    data = NULL;


    /*** put  udp header in data ***/
    stream_write(pdata, &udph, sizeof(udph));

    /*** put rip header and info in data***/
    stream_write(pdata, buf, size);

    if (to)
    {
        /*PKT_TYPE_IP */
        pcb.ipcb.dip = ntohl(sin.sin_addr.s_addr);
        pcb.ipcb.sip = ntohl(ifc->address->u.prefix4.s_addr);
        pcb.ipcb.is_changed = 1;
        pcb.ipcb.ifindex = ifc->ifp->ifindex;
        pcb.ipcb.vpn = 0;
        pcb.ipcb.ttl = 64;
        pcb.ipcb.protocol = IP_P_UDP;
        pcb.ipcb.tos = 3;
        ret = pkt_send(PKT_TYPE_IP, &pcb, STREAM_DATA(pdata), sizeof(udph) + size);
    }
    else
    {
        if (RIPv2 == version || version == RI_RIP_VERSION_2_MULTI)
        {
            pcb.ethcb.dmac[0] = 0x01;
            pcb.ethcb.dmac[1] = 0x00;
            pcb.ethcb.dmac[2] = 0x5e;
            pcb.ethcb.dmac[3] = 0x00;
            pcb.ethcb.dmac[4] = 0x00;
            pcb.ethcb.dmac[5] = 0x09;
        }
        else
        {
            pcb.ethcb.dmac[0] = 0xff;
            pcb.ethcb.dmac[1] = 0xff;
            pcb.ethcb.dmac[2] = 0xff;
            pcb.ethcb.dmac[3] = 0xff;
            pcb.ethcb.dmac[4] = 0xff;
            pcb.ethcb.dmac[5] = 0xff;
        }

        pcb.ethcb.ifindex = ifc->ifp->ifindex;
        pcb.ethcb.ethtype = ETH_P_IP;
        pcb.ethcb.cos = 3;

        ret = pkt_send(PKT_TYPE_ETH, &pcb, STREAM_DATA(pdata), sizeof(iph) + sizeof(udph) + size);
    }

	zlog_debug(RIP_DBG_PACKET, "%s[%d]:Rip send packet: %s\n", __FUNCTION__, __LINE__,\
							pkt_dump(STREAM_DATA(pdata), sizeof(iph) + sizeof(udph) + size));

    if (ret == 0)
        rip_stats.tx_packets++;

    if (ret == -1)
        rip_stats.tx_failed++;

    stream_free(pdata);
    pdata = NULL;

    //if (IS_RIP_DEBUG_PACKET)
    {
        zlog_debug(RIP_DBG_PACKET, "SEND to  %s.%d", inet_ntoa(sin.sin_addr),
                   ntohs(sin.sin_port));
    }
    if (ret < 0)
    {
        zlog_debug(RIP_DBG_PACKET, "can't send packet : %s", safe_strerror(errno));
    }

    return ret;
}

/* Add redistributed route to RIP table. */
void
rip_redistribute_add(struct rip_instance *rip, struct rip_info *newinfo, struct prefix_ipv4 *p)
{
    int ret;
    struct route_node *rp = NULL;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *node, *nnode;
    /* Redistribute route  */
    ret = rip_destination_check(p->prefix);

    if (! ret)
        return;

    rp = route_node_get(rip->table, (struct prefix *) p);
    newinfo->rp = rp;
    if ((list = (struct list *)rp->info) != NULL)
    {
        rinfo = (struct rip_info *)listgetdata(listhead(list));
        if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
        {
            if (listcount(list) > 1)
                rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
            else
                rinfo = NULL;
        }

        if (NULL == rinfo)
        {
            rip_ecmp_add(newinfo, rip);
        }
        else
        {
            if (rinfo->metric == RIP_METRIC_INFINITY)
            {
                rip_ecmp_replace(newinfo, rip);
				route_unlock_node(rp);
                return;
            }

            if (rinfo->type == ROUTE_PROTO_RIP
                    && CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE))
            {
            	if((newinfo->type == ROUTE_PROTO_RIP)
                    && CHECK_FLAG(newinfo->sub_type, RIP_ROUTE_INTERFACE))
            	{
					rinfo->metric = 0;
					rinfo->metric_out = 0;
					RIP_TIMER_OFF(rinfo->t_timeout);
					RIP_TIMER_OFF(rinfo->t_garbage_collect);	
					route_unlock_node(rp);
					return;
				}

                if (newinfo->distance <= rinfo->distance 
					|| rinfo->metric == RIP_METRIC_INFINITY)
                {
                    rip_ecmp_add(newinfo, rip);
                }				
				route_unlock_node(rp);
                return;
            }

            if (newinfo->type == ROUTE_PROTO_RIP && newinfo->id == rip->id
                    && CHECK_FLAG(newinfo->sub_type, RIP_ROUTE_INTERFACE))
            {
                if (rinfo->type == ROUTE_PROTO_CONNECT && rinfo->sub_type == RIP_ROUTE_REDISTRIBUTE)
                {
                    if (newinfo->distance >= rinfo->distance)
                    {
                        rip_ecmp_add(newinfo, rip);
                    }
					route_unlock_node(rp);
                    return;
                }
                else
                {
                    rip_ecmp_replace(newinfo, rip);
					route_unlock_node(rp);
                    return;
                }
            }

            if (newinfo->distance < rinfo->distance)
            {
                rip_ecmp_replace(newinfo, rip);
            }
            else if (newinfo->distance == rinfo->distance)
            {

                for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
                {
                    if (newinfo->type == rinfo->type && newinfo->external_id == rinfo->external_id
                            && IPV4_ADDR_SAME(&newinfo->nexthop, &rinfo->nexthop))
                    {
//                        if (rinfo->metric == RIP_METRIC_INFINITY)
//                        {
                            rinfo->metric = newinfo->metric;
                            rinfo->metric_out = newinfo->metric;
                            RIP_TIMER_OFF(rinfo->t_garbage_collect);

                            rip_summary_all_check(rinfo, rp, 1, rinfo->metric, rip);

                            if (rip->inter_sum_count)
                                rip_summary_count_check(newinfo, rp, 1, rinfo->metric, rip);
  //                      }

                        rinfo->external_metric = newinfo->external_metric;
  						route_unlock_node(rp);
                        return;
                    }
                }

                rip_ecmp_add(newinfo, rip);
            }
        }
		route_unlock_node(rp);
    }
    else
    {
        rip_ecmp_add(newinfo, rip);
    }

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "Redistribute new prefix %s/%d ", inet_ntoa(p->prefix), p->prefixlen);
        zlog_debug(RIP_DBG_EVENT, " 			with nexthop %s on the interface %s", inet_ntoa(newinfo->nexthop), ifindex2ifname(newinfo->ifindex));
    }

//  rip_event (RIP_TRIGGERED_UPDATE, rip);
}


/* Delete redistributed route from RIP table. */
void
rip_redistribute_delete(struct rip_instance *rip, struct rip_info *newrinfo, struct prefix_ipv4 *p)
{
    int ret;
    struct route_node *rp;
    struct rip_info *rinfo;
    struct list *list = NULL;
    struct listnode *node, *nnode;

    ret = rip_destination_check(p->prefix);

    if (! ret)
        return;

    rp = route_node_lookup(rip->table, (struct prefix *) p);

    if (rp)
    {

        list = (struct list *)rp->info;
        if (list != NULL && listcount(list) != 0)
        {

            for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
            {
                if (!rinfo)
                    continue;

                if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
                {
                    continue;
                }

                if (rinfo->type == newrinfo->type
                        && rinfo->external_id == newrinfo->external_id
                        && rinfo->ifindex == newrinfo->ifindex)
                {
                    /* Perform poisoned reverse. */
                    rip_ecmp_delete(rinfo, rip);

                    //if (IS_RIP_DEBUG_EVENT)
                    {
                        zlog_debug(RIP_DBG_EVENT, "Poisone %s/%d on the interface %s with an "
                                   "infinity metric [delete]",
                                   inet_ntoa(p->prefix), p->prefixlen,
                                   ifindex2ifname(newrinfo->ifindex));
                    }
                }
            }
        }
		
		route_unlock_node(rp);
    }

}

/* Withdraw redistributed route. */
void
rip_redistribute_withdraw(enum ROUTE_PROTO type, int id, struct rip_instance *rip)
{
    struct route_node *rp;
    struct rip_info *rinfo;
    struct list *list;
    struct listnode *node, *nnode;
    struct prefix_ipv4 *p = NULL;

    if (!rip)
        return;

    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
            {
                if (rinfo->type == type  && CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_REDISTRIBUTE))
                {
                    if (id && rinfo->external_id != id)
                    {
                        continue;
                    }

                    rip_ecmp_delete(rinfo, rip);
                    rip->triggered_update_flag = 1;

                    /* Perform poisoned reverse. */
                    //if (IS_RIP_DEBUG_EVENT)
                    {
                        p = (struct prefix_ipv4 *) & rp->p;
                        zlog_debug(RIP_DBG_EVENT, "Poisone %s/%d on the interface %s with an infinity metric [withdraw]",
                                   inet_ntoa(p->prefix), p->prefixlen,
                                   ifindex2ifname(rinfo->ifindex));
                    }

                }
            }
        }
    }

    if (rip->triggered_update_flag)
    {
        rip_event(RIP_TRIGGERED_UPDATE, rip);
        rip->triggered_update_flag = 0;
    }
}

/* Response to request called from rip_read ().*/
void
rip_request_process(struct rip_packet *packet, int size,
                    struct sockaddr_in *from, struct connected *ifc)
{
    caddr_t lim;
    struct rte *rte;
    struct prefix_ipv4 p;
    struct route_node *rp;
    struct rip_info *rinfo;
    struct rip_interface *ri;
    struct rip_instance *rip;
    struct list *list;

    /* Does not reponse to the requests on the loopback interfaces */
    if (if_is_loopback(ifc->ifp))
        return;

    /* Check RIP process is enabled on this interface. */
    ri = (struct rip_interface *)ifc->ifp->info;

    if (! ri->enable_inter_node)
        return;

    /* When passive interface is specified, suppress responses */
    if (ri->passive)
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "interface %s is passive", ifc->ifp->name);
        }
        return;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
        return;

    lim = ((caddr_t) packet) + size;
    rte = packet->rte;

    /* The Request is processed entry by entry.  If there are no
       entries, no response is given. */
    if (lim == (caddr_t) rte)
        return;

    int vsend = ((ri->ri_send == RI_RIP_UNSPEC) ?
                 rip->version_send : ri->ri_send);
	if(RIPv1 != vsend)
	{
		vsend = RIPv2;
	}

    /* There is one special case.  If there is exactly one entry in the
       request, and it has an address family identifier of zero and a
       metric of infinity (i.e., 16), then this is a request to send the
       entire routing table. */
    if (lim == ((caddr_t)(rte + 1)) &&
            ntohs(rte->family) == 0 &&
            ntohl(rte->metric) == RIP_METRIC_INFINITY)
    {
        /* All route with split horizon */
        rip_output_process(ifc, from, rip_all_route, vsend, 0);
    }
    else
    {
        /* Examine the list of RTEs in the Request one by one.  For each
        entry, look up the destination in the router's routing
        database and, if there is a route, put that route's metric in
        the metric field of the RTE.  If there is no explicit route
        to the specified destination, put infinity in the metric
        field.  Once all the entries have been filled in, change the
        command from Request to Response and send the datagram back
        to the requestor. */
        p.family = AF_INET;

        for (; ((caddr_t) rte) < lim; rte++)
        {
            p.prefix = rte->prefix;
            p.prefixlen = ip_masklen(rte->mask);
            apply_mask_ipv4(&p);

            rp = route_node_lookup(rip->table, (struct prefix *) & p);

            if (rp)
            {
                list = (struct list *)rp->info;
                rinfo = (struct rip_info *)listgetdata(listhead(list));

                if (rinfo)
                {
                    if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
                    {
                        if (listcount(list) > 1)
                            rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                    }

                    rte->metric = htonl(rinfo->metric);
                }
				route_unlock_node(rp);
            }
            else
                rte->metric = htonl(RIP_METRIC_INFINITY);
        }

        packet->command = RIP_RESPONSE;

        rip_send_packet((u_char *)packet, size, from, ifc, vsend);
    }

    rip_global_queries++;
}

/* Write routing table entry to the stream and return next index of
   the routing table entry in the stream. */
int
rip_write_rte(int num, struct stream *s, struct prefix_ipv4 *p,
              u_char version, struct rip_info *rinfo, u_int32_t metric, struct connected *ifc)
{
    struct in_addr mask;
    u_int32_t sdmetric;

    if (metric)
    {
        if (ifc->ifp->metric)
            sdmetric = metric + ifc->ifp->metric + 1;
        else
            sdmetric = metric + 1;
    }
    else
    {
        if (ifc->ifp->metric)
            sdmetric = rinfo->metric_out + ifc->ifp->metric + 1;
        else
            sdmetric = rinfo->metric_out + 1;
    }

    if (sdmetric > RIP_METRIC_INFINITY)
        sdmetric = RIP_METRIC_INFINITY;

    /* Write routing table entry. */
    if (version == RIPv1)
    {
        stream_putw(s, AF_INET);
        stream_putw(s, 0);
        stream_put_ipv4(s, p->prefix.s_addr);
        stream_put_ipv4(s, 0);
        stream_put_ipv4(s, 0);
        stream_putl(s, sdmetric);
    }
    else
    {
        masklen2ip(p->prefixlen, &mask);
//    mask.s_addr = htonl(mask.s_addr);
        stream_putw(s, AF_INET);
        stream_putw(s, rinfo->tag_out);
        stream_put_ipv4(s, p->prefix.s_addr);
        stream_put_ipv4(s, mask.s_addr);
        stream_put_ipv4(s, rinfo->nexthop_out.s_addr);
        stream_putl(s, sdmetric);
    }

    return ++num;
}


int
rip_version2_ruoute_send_check(struct rip_instance *rip, struct route_node *rp,
                               struct connected *ifc, u_int32_t *metcount, struct rip_info *rinfo)
{
    struct list *list = NULL;
    struct listnode *snode = NULL;
    struct rip_summary *rs = NULL;
	struct rip_interface *ri = (struct rip_interface *)ifc->ifp->info;

    if (!rp->info)
        return ERRNO_NOT_FOUND;

    list = (struct list *)rp->info;
    rinfo = (struct rip_info *)listgetdata(listhead(list));

    if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS))
    {
        snode = ri_summary_prefix_lookup(ri->summary, &rinfo->rp->p);

        if (snode)
        {
            rs = (struct rip_summary *)listgetdata(snode);

            if (1 == rip_summary_check_sub_routes(rinfo->rp))
            {
                if ((rs->uflag) || (rs->t_sdown))
                {
                    if (rs->t_sdown)
                    {
                        if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO)
                                && rip->summaryflag)
                        {
                            goto next;
                        }
                        else
                        {
                            if (listcount(list) > 1)
                            {
                                rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                                metcount[0] = rinfo->metric;
                                metcount[1] = 1;
                                return 1;
                            }
                            else
                            {
                                metcount[0] = RIP_METRIC_INFINITY;
                                return 1;
                            }
                        }
                    }
                    else
                    {
                        metcount[0] = rinfo->metric;
                        return 1;
                    }
                }
            }
            else
            {
                if (rs->t_sdown && (!CHECK_FLAG(rinfo->summary_flag, ~RIP_SUMMARY_ADRESS)))
                {
                    if (listcount(list) > 1)
                    {
                        rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                        metcount[0] = rinfo->metric;
                        metcount[1] = 1;
                        return 1;
                    }
                    else
                    {
                        metcount[0] = RIP_METRIC_INFINITY;
                        return 1;
                    }
                }
            }
        }
    }

#if 0
    else if ((snode = ri_summary_prefix_match_lookup(ri->summary, &rinfo->rp->p))
             && (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_SUB_ADDR)))
    {
//        snode = ri_summary_prefix_match_lookup(ri->summary,&rinfo->rp->p);
//       if(snode)
//         {
        rs = listgetdata(snode);

        if (rs->t_sup)
        {
            metcount[0] = RIP_METRIC_INFINITY;
            return 1;
        }
        else if (rs->t_sdown)
        {
            metcount[0] = rinfo->metric;
            return 1;
        }
        else
        {
            return -1;
        }

//         }
    }

#endif
    snode = ri_summary_prefix_match_lookup(ri->summary, &rinfo->rp->p);

    if (snode)
    {
        rs = (struct rip_summary *)listgetdata(snode);

        if (rs->t_sup)
        {
            metcount[0] = RIP_METRIC_INFINITY;
            return 1;
        }
        else if (rs->t_sdown)
        {
            goto next;
        }
        else
        {
            return ERRNO_NOT_FOUND;
        }
    }
    else
    {
        goto next;
    }

    return ERRNO_NOT_FOUND;

next:

    if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO))
    {
        if (rip->summaryflag || rip->t_summary_unset)
        {
            if (rip->t_summary_unset)
            {
                if (listcount(list) > 1)
                {
                    rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                    metcount[0] = rinfo->metric;
                    metcount[1] = 1;
                    return 1;
                }
                else
                {
                    metcount[0] = RIP_METRIC_INFINITY;
                    return 1;
                }
            }
            else
            {
                if (listcount(ri->summary)
                        && (snode = ri_summary_prefix_match_lookup(ri->summary, &rinfo->rp->p)))
                {
                    rs = (struct rip_summary *)listgetdata(snode);

                    if (rip_auto_summary_check_sub_routes(rp, &rs->p, ifc->ifp->ifindex))
                    {
                        metcount[0] = rinfo->metric;
                        return 1;
                    }
                    else
                    {
                        if (rs->t_sup)
                        {
                            metcount[0] = RIP_METRIC_INFINITY;
                            return 1;
                        }
                        else if (rs->t_sdown)
                        {
                            metcount[0] = rinfo->metric;
                            return 1;
                        }
                        else
                        {
                            return ERRNO_NOT_FOUND;
                        }

                    }
                }
                else
                {
					if (rip_auto_summary_check_sub_routes(rp, NULL, ifc->ifp->ifindex))
					{
						metcount[0] = rinfo->metric;
						return 1;
					}
					else
					{
						return ERRNO_NOT_FOUND;
					}
                }
            }
        }
        else
        {
            if (listcount(list) > 1)
            {
                rinfo = (struct rip_info *)listgetdata(listnextnode(listhead(list)));
                metcount[0] = rinfo->metric;
                metcount[1] = 1;
                return 1;
            }
        }
    }
    else if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_SUB_AUTO))
    {
        if (rip->summaryflag)
        {
            if (rip->t_summary_set)
            {
                metcount[0] = RIP_METRIC_INFINITY;
                return 1;
            }
            else if (rip->t_summary_unset)
            {
                metcount[0] = rinfo->metric;
                return 1;
            }
            else
            {
                return ERRNO_NOT_FOUND;
            }
        }
        else
        {
            metcount[0] = rinfo->metric;
            return 1;
        }
    }
    else if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_SUPER))
    {
        metcount[0] = rinfo->metric;
        return 1;
    }

    return ERRNO_NOT_FOUND;

}


struct rip_info *
rip_version1_if_class_route_check(struct route_node *rp, struct connected *ifc)
{
    struct route_node *node = NULL;
    struct list *list = NULL;
    struct rip_info *rinfo = NULL;
    struct rip_info *temp_rinfo = NULL;
    struct listnode *lnode;

	if(!rp)
	{
		zlog_err("%s %d rp is NULL",__func__,__LINE__);
		return NULL;
	}

    route_lock_node(rp);
	node = rp;
    while (node)
    {
        if (node->info != NULL)
        {
            if (prefix_match(&rp->p, &node->p))
            {
            	if(node->p.prefixlen == ifc->address->prefixlen)
            	{
					node = route_next(node);
					continue;
				}
				
                list = (struct list *)node->info;

                for (ALL_LIST_ELEMENTS_RO(list, lnode, rinfo))
                {
                	if(CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
							&& (rinfo->ifindex != ifc->ifp->ifindex))
					{
						if(rinfo->metric == RIP_METRIC_INFINITY)
						{
							temp_rinfo = rinfo;
							continue;
						}
						else
						{
	                        route_unlock_node(node);
	                        return rinfo;
						}
                    }
                    else if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_REDISTRIBUTE)
								|| CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE))
                    {
						if(rinfo->metric == RIP_METRIC_INFINITY)
						{
							temp_rinfo = rinfo;
							continue;
						}
						else
						{
	                        route_unlock_node(node);
	                        return rinfo;
						}
                    }
                }
            }
            else
            {
				route_unlock_node(node);
                return temp_rinfo;
            }
        }
		
    	node = route_next(node);
    }

    return temp_rinfo;

}

int
rip_version1_send_route_check(struct route_node *rp, struct connected *ifc)
{
    struct route_node *node = NULL;
    struct list *list = NULL;
    struct rip_info *rinfo = NULL;
    struct listnode *lnode;

	if(!rp)
	{
		zlog_err("%s %d rp is NULL",__func__,__LINE__);
		return 0;
	}

    route_lock_node(rp);
	node = rp;
    while (node)
    {
        if (node->info != NULL)
        {
            if (prefix_match(&rp->p, &node->p))
            {				
                list = (struct list *)node->info;

                for (ALL_LIST_ELEMENTS_RO(list, lnode, rinfo))
                {
                	if(CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
							&& (rinfo->ifindex != ifc->ifp->ifindex))
					{
                        route_unlock_node(node);
                        return 1;
                    }
                    else if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_REDISTRIBUTE)
								|| CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE))
                    {
                        route_unlock_node(node);
                        return 1;
                    }
                }
            }
            else
            {
				route_unlock_node(node);
                return 0;
            }
        }
		
    	node = route_next(node);
    }

    return 0;

}
/* Send update to the ifp or spcified neighbor. */
void
rip_output_process(struct connected *ifc, struct sockaddr_in *to,
                   int route_type, u_char version, u_int32_t metric)
{
    int ret = 0;
    struct stream *s = NULL;
    struct route_node *rp = NULL;
    struct rip_info *rinfo = NULL;
//  struct rip_info *srinfo = NULL;
    struct rip_interface *ri = NULL;
    struct prefix_ipv4 *p = NULL;
    struct prefix ifaddrclass;
    struct key *key = NULL;
    /* this might need to made dynamic if RIP ever supported auth methods
       with larger key string sizes */
    char auth_str[RIP_AUTH_SIMPLE_SIZE];
    size_t doff = 0; /* offset of digest offset field */
    int num = 0;
    int rtemax;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
//  struct listnode *snode = NULL;
    struct rip_instance *rip = NULL;
    u_int32_t smetric = 0;
//  int summary = 0;
    u_int32_t metcount[2];
	struct rip_info *tmp_rinfo = NULL;


    /* Logging output event. */
    //if (IS_RIP_DEBUG_EVENT)
    {
        if (to)
        {
            zlog_debug(RIP_DBG_EVENT, "update routes to neighbor %s use ip %s", inet_ntoa(to->sin_addr), inet_ntoa(ifc->address->u.prefix4));
        }
        else
        {
            zlog_debug(RIP_DBG_EVENT, "update routes on interface %s ifindex %02x ip %s",
                       ifc->ifp->name, ifc->ifp->ifindex, inet_ntoa(ifc->address->u.prefix4));
        }
    }

    /* Get RIP interface. */
    ri = (struct rip_interface *)ifc->ifp->info;
    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "can`t find rip instace %d", ri->enable_inter_node);
        }
        return;
    }

    /* Set output stream. */
    s = rip_m.pktsend_buf;
    /* Reset stream and RTE counter. */
    stream_reset(s);
    rtemax = RIP_MAX_RTE;

    /* If output interface is in simple password authentication mode, we
       need space for authentication data.  */
    if (ri->auth_type == RIP_AUTH_SIMPLE_PASSWORD)
        rtemax -= 1;

    /* If output interface is in MD5 authentication mode, we need space
       for authentication header and data. */
    if (ri->auth_type == RIP_AUTH_MD5)
        rtemax -= 2;

    /* If output interface is in simple password authentication mode
       and string or keychain is specified we need space for auth. data */
    if (ri->auth_type != RIP_NO_AUTH)
    {
        if (ri->key_chain)
        {
            struct keychain *keychain;

            keychain = keychain_lookup(ri->key_chain);

            if (keychain)
                key = key_lookup_for_send(keychain);
        }

        /* to be passed to auth functions later */
        rip_auth_prepare_str_send(ri, key, auth_str, RIP_AUTH_SIMPLE_SIZE);
    }

    if (version == RIPv1)
    {
        memcpy(&ifaddrclass, ifc->address, sizeof(struct prefix));
        rip_apply_classful_mask_ipv4((struct prefix_ipv4 *)&ifaddrclass);
    }

    //if (IS_RIP_DEBUG_PACKET)
    {
        zlog_debug(RIP_DBG_PACKET, "RIP instance %d, summary flag %d, summary count %d", rip->id, rip->summaryflag, rip->inter_sum_count);
    }
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) == NULL || listcount(list) == 0)
            continue;

        /* For RIPv1, if we are subnetted, output subnets in our network    */
        /* that have the same mask as the output "interface". For other     */
        /* networks, only the classfull version is output.                  */
        smetric = 0;
        metcount[0] = RIP_METRIC_INFINITY+1;
        metcount[1] = 0;
        rinfo = (struct rip_info *)listgetdata(listhead(list));

        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "check %s/%d with rinfo->subtype %d summary flag %d list %d count %d metric %d out %d mm %d for output",
                       inet_ntoa(rp->p.u.prefix4), rp->p.prefixlen, rinfo->sub_type, rinfo->summary_flag, listcount(list), rinfo->summary_count,
                       rinfo->metric, rinfo->metric_out, rinfo->mmt_count);
        }

        /* Changed route only output. */
        if (route_type == rip_changed_route &&
                (!(rinfo->flags & RIP_RTF_CHANGED)))
        {
            //if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug(RIP_DBG_PACKET, "changed route only");
            }

            continue;
        }

        if (version == RIPv1)
        {
        	/* ignore classful route for interface if there is no difference prefixlen route with the 
        	interface route contained by this route existed. for example, interface is 10.1.1.1/24, 
        	then the auto summary route is 10.0.0.0/8, if we have a route 10.2.1.0/24, then we send 
        	10.1.1.0/24 and 10.2.1.0/24 instance of 10.0.0.0/8. but if we have another route 10.3.0.0/16,
        	then we wolud send 10.1.1.0/24, 10.2.1.0/24 and 10.0.0.0/8. ps: 10.2.1.0/24 and 10.3.0.0/16
        	cant be learned from the opposite device.*/
        	if(CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_SUPER))
			{
				//if (IS_RIP_DEBUG_PACKET)
				{
					zlog_debug(RIP_DBG_PACKET, "V1: can`t support super route");
				}
				continue;
			}
			else if (prefix_same(&ifaddrclass, &rp->p))
            {
            	tmp_rinfo = rip_version1_if_class_route_check(rp, ifc);
				if(!tmp_rinfo)
				{
					//if (IS_RIP_DEBUG_PACKET)
	                {
	                    zlog_debug(RIP_DBG_PACKET, "V1: interface route");
	                }
	                continue;
				}
				else if(RIP_METRIC_INFINITY == tmp_rinfo->metric)
				{
					smetric = RIP_METRIC_INFINITY;
				}
            }
			else if(CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO)
					&& !rip_version1_send_route_check(rp, ifc))
			{
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "auto summary route without valid sub route can`t be send");
                }
				continue;
			}
            else if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_SUB_AUTO))
            {
            	if(prefix_match(&ifaddrclass, &rp->p))
            	{
            		if((rp->p.prefixlen != ifc->address->prefixlen))
            		{
		                //if (IS_RIP_DEBUG_PACKET)
		                {
		                    zlog_debug(RIP_DBG_PACKET, "sub route with different prefixlen belong to the interface classful route can`t be send");
		                }
						continue;
            		}
            	}
				else
            	{
	                //if (IS_RIP_DEBUG_PACKET)
	                {
	                    zlog_debug(RIP_DBG_PACKET, "sub route not belong to the interface classful route can`t be send");
	                }
					continue;
            	}
            }
            else if (CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS) && !CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO))
            {
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "hand summary route can`t be send");
                }
				continue;
			}
        }
        else
        {
            ret = rip_version2_ruoute_send_check(rip, rp, ifc, metcount, rinfo);

            if (metcount[0] != RIP_METRIC_INFINITY+1)
            {
                smetric = metcount[0];
            }
            else
            {
                if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_err("V2: %s %d get metric -1 < 0", __func__, __LINE__);
                }

                continue;
            }

            if (metcount[1])
                rinfo = listgetdata(listnextnode(listhead(list)));

            if (ret == ERRNO_NOT_FOUND)
            {
                if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_err("V2: %s %d ret < 0", __func__, __LINE__);
                }

                continue;
            }
        }

        p = (struct prefix_ipv4 *) & rp->p;

        /* Apply output filters. */
//  ret = rip_outgoing_filter (p, ri);
//  if (ret < 0)
//    continue;

        /* Split horizon. */
        /* if (split_horizon == rip_split_horizon) */
        if (ri->split_horizon == RIP_SPLIT_HORIZON && (ri->poison_reverse != RIP_SPLIT_HORIZON_POISONED_REVERSE))
        {
            /*
             * We perform split horizon for RIP and connected route.
             * For rip routes, we want to suppress the route if we would
                 * end up sending the route back on the interface that we
                 * learned it from, with a higher metric. For connected routes,
                 * we suppress the route if the prefix is a subset of the
                 * source address that we are going to use for the packet
                 * (in order to handle the case when multiple subnets are
                 * configured on the same interface).
                 */
            int suppress = 0;

            ret = 0;
			if(version == RIPv1)
			{
            	ret = rip_version1_send_route_check(rp, ifc);
			}
            if (version == 2 || ((version == RIPv1) && !ret))
            {
                for (ALL_LIST_ELEMENTS_RO(list, listnode, tmp_rinfo))
                {
                    if (CHECK_FLAG(tmp_rinfo->sub_type, RIP_ROUTE_RTE) 
							&& tmp_rinfo->ifindex == ifc->ifp->ifindex)
                    {
                        suppress = 1;
						//if (IS_RIP_DEBUG_PACKET)
						{
                        	zlog_debug(RIP_DBG_PACKET, "set supress to 1 %s %d", __func__, __LINE__);
						}
                        break;
                    }
					
                    if (CHECK_FLAG(tmp_rinfo->sub_type, RIP_ROUTE_INTERFACE) 
							&& (!prefix_match((struct prefix *)p, ifc->address) && tmp_rinfo->ifindex == ifc->ifp->ifindex))
					{
						suppress = 0;
						//if (IS_RIP_DEBUG_PACKET)
						{
							zlog_debug(RIP_DBG_PACKET, "set supress to 0 %s %d", __func__, __LINE__);
						}
						break;
					}
                }
            }

            if (suppress && (version == 2) && interface_summary_prefix_same_lookup(ri->summary, &rp->p))
            {
                suppress = 0;
				
                //if (IS_RIP_DEBUG_PACKET)
                {
                	zlog_debug(RIP_DBG_PACKET, "set supress to 0 %s %d", __func__, __LINE__);
                }
            }

#if 0
            if (!suppress && rip_route_rte(rinfo) &&
                    prefix_match((struct prefix *)p, ifc->address))
            {
                suppress = 1;
                if (IS_RIP_DEBUG_PACKET)
                {
                	zlog_debug("set supress to 1 %s %d", __func__, __LINE__);
                }
            }
#endif 
            if (suppress)
            {
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "route %s/%d filtered by splithorizon ",
                               inet_ntoa(rp->p.u.prefix4), rp->p.prefixlen);
                }

                continue;
            }
        }

        /* Preparation for route-map. */
//  rinfo->metric_set = 0;
        rinfo->nexthop_out.s_addr = 0;
        rinfo->tag_out = rinfo->tag;

#if 0

        if (CHECK_FLAG(rinfo->sub_type, ~RIP_ROUTE_SUMMARY))
        {
            if (smetric < RIP_METRIC_INFINITY)
            {
                smetric = 16;

                for (ALL_LIST_ELEMENTS_RO(list, snode, srinfo))
                {
                    if (srinfo)
                    {
                        smetric = srinfo->metric < smetric ? srinfo->metric : smetric;
                    }
                }
            }
        }
        else
        {
            if (!summary && smetric < RIP_METRIC_INFINITY)
            {
                smetric = 16;

                for (ALL_LIST_ELEMENTS_RO(list, snode, srinfo))
                {
                    smetric = srinfo->metric < smetric ? srinfo->metric : smetric;
                }
            }
        }

#endif

        /* In order to avoid some local loops,
         * if the RIP route has a nexthop via this interface, keep the nexthop,
         * otherwise set it to 0. The nexthop should not be propagated
         * beyond the local broadcast/multicast area in order
         * to avoid an IGP multi-level recursive look-up.
         * see (4.4)
         */
//        if (rinfo->ifindex == ifc->ifp->ifindex && prefix_match((struct prefix *)p, ifc->address))
//        {
//            rinfo->nexthop_out = rinfo->nexthop;
//        }
		
        /* Interface route-map */
        if (ri->routemap[RIP_FILTER_OUT])
        {
            ret = route_map_apply(ri->routemap[RIP_FILTER_OUT],
                                  (struct prefix *) p, RMAP_RIP,
                                  rinfo);

            if (ret == RMAP_DENYMATCH)
            {
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "RIP %s/%d is filtered by route-map out",
                               inet_ntoa(p->prefix), p->prefixlen);
                }
                continue;
            }
        }

        /* Apply redistribute route map - continue, if deny */
        if (rip->route_map[rinfo->type].name
                && CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE))
        {
            ret = route_map_apply(rip->route_map[rinfo->type].map,
                                  (struct prefix *)p, RMAP_RIP, rinfo);

            if (ret == RMAP_DENYMATCH)
            {
                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "%s/%d is filtered by route-map",
                               inet_ntoa(p->prefix), p->prefixlen);
                }
                continue;
            }
        }

        /* Apply offset-list */
        if (rinfo->metric != RIP_METRIC_INFINITY)
            rip_offset_list_apply_out(p, ifc->ifp, &rinfo->metric_out);

        if (rinfo->metric_out > RIP_METRIC_INFINITY)
            rinfo->metric_out = RIP_METRIC_INFINITY;

        /* Perform split-horizon with poisoned reverse
         * for RIP and connected routes.
         **/
        if (ri->poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
        {
            /*
                * We perform split horizon for RIP and connected route.
                * For rip routes, we want to suppress the route if we would
                 * end up sending the route back on the interface that we
                 * learned it from, with a higher metric. For connected routes,
                 * we suppress the route if the prefix is a subset of the
                 * source address that we are going to use for the packet
                 * (in order to handle the case when multiple subnets are
                 * configured on the same interface).
                 */

            ret = rip_version1_send_route_check(rp, ifc);

            if (version == 2 || ((version == RIPv1) && !ret))
            {
                for (ALL_LIST_ELEMENTS_RO(list, listnode, tmp_rinfo))
                {
                    if (!tmp_rinfo)
                        continue;

                    if (tmp_rinfo->type == ROUTE_PROTO_RIP  &&
                            tmp_rinfo->ifindex == ifc->ifp->ifindex)
                    {
                        smetric = RIP_METRIC_INFINITY;
                    }

                    if (CHECK_FLAG(tmp_rinfo->sub_type, RIP_ROUTE_INTERFACE) &&
                            prefix_match((struct prefix *)p, ifc->address))
                    {
                        smetric = RIP_METRIC_INFINITY;
                    }
                }
            }
        }

        /* Prepare preamble, auth headers, if needs be */
        if (num == 0)
        {
            stream_putc(s, RIP_RESPONSE);

            if (RIPv1 == version)
            {
                stream_putc(s, version);
            }
            else
            {
                stream_putc(s, RIPv2);
            }

            stream_putw(s, 0);

            /* auth header for !v1 && !no_auth */
            if ((ri->auth_type != RIP_NO_AUTH) && (version != RIPv1))
                doff = rip_auth_header_write(s, ri, key, auth_str,
                                             RIP_AUTH_SIMPLE_SIZE);
        }

        /* when delete rip instance or disable an interface, set metric inifinity and advertised it to other*/
        if (metric)
        {
            smetric = RIP_METRIC_INFINITY;
        }

        /* Write RTE to the stream. */
        num = rip_write_rte(num, s, p, version, rinfo, smetric, ifc);

        if (num == rtemax)
        {
            if ((version != RIPv1) && (ri->auth_type == RIP_AUTH_MD5))
            {
                rip_auth_md5_set(s, ri, doff, auth_str, RIP_AUTH_SIMPLE_SIZE);
            }

            ret = rip_send_packet(STREAM_DATA(s), stream_get_endp(s), to, ifc, version);

            if (ret >= 0 && IS_RIP_DEBUG_PACKET)
            {
                rip_packet_dump((struct rip_packet *)STREAM_DATA(s),
                                stream_get_endp(s), "SEND");

            }
			/* Statistics updates. */
			ri->sent_updates++;
            num = 0;
            stream_reset(s);
        }

    }


    /* Flush unwritten RTE. */
    if (num != 0)
    {
        if ((version != RIPv1) && (ri->auth_type == RIP_AUTH_MD5))
        {
            rip_auth_md5_set(s, ri, doff, auth_str, RIP_AUTH_SIMPLE_SIZE);
        }

        ret = rip_send_packet(STREAM_DATA(s), stream_get_endp(s), to, ifc, version);


        if (ret >= 0 && IS_RIP_DEBUG_PACKET)
        {
            rip_packet_dump((struct rip_packet *)STREAM_DATA(s),
                            stream_get_endp(s), "SEND");

        }
		/* Statistics updates. */
		ri->sent_updates++;

        num = 0;
    }

//  stream_free (s);



}

/* Update send to interface. */
int
rip_update_interface_spec(int route_type, struct interface *ifp, struct rip_instance *rip, u_int32_t metric)
{
    struct listnode *ifnode, *ifnnode;
    struct connected *connected;
    struct rip_interface *ri;

    if (if_is_loopback(ifp))
        return 1;

    if (! if_is_operative(ifp))
        return 1;

    /* Fetch RIP interface information. */
    ri = (struct rip_interface *)ifp->info;

    /* When passive interface is specified, suppress announce to the
     interface. */
    if (ri->passive)
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "interface %s is passive", ifp->name);
        }
        return 1;
    }

    if (ri->enable_inter_node != rip->id)
        return 1;

    /*
    * If there is no version configuration in the interface,
    * use rip's version setting.
    */
    int vsend = ((ri->ri_send == RI_RIP_UNSPEC) ?
                 rip->version_send : ri->ri_send);

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "SEND UPDATE to %s ifindex %02x",
                   ifp->name, ifp->ifindex);
    }

    /* send update on each connected network */
    for (ALL_LIST_ELEMENTS(ifp->connected, ifnode, ifnnode, connected))
    {

//      if (CHECK_FLAG(connected->flags, ZEBRA_IFA_SECONDARY))
//      {
//        zlog_debug("slave ip address could not config rip \n");
//        return 1;
//      }

        if (connected->address->family == AF_INET)
        {
            if (vsend == RIPv1)
            {
                //if (IS_RIP_DEBUG_EVENT)
                {
                    zlog_debug(RIP_DBG_EVENT, "RIPv1 announce on %s ", connected->ifp->name);
                }
                rip_output_process(connected, NULL, route_type, RIPv1, metric);
            }

            if (vsend & RIPv2)
            {
                //if (IS_RIP_DEBUG_EVENT)
                {
                    zlog_debug(RIP_DBG_EVENT, "RIPv2 announce on %s ", connected->ifp->name);
                }
                rip_output_process(connected, NULL, route_type, RIPv2, metric);
            }
        }
    }

    return 0;
}

/* Update send to all  neighbor. */
void
rip_update_neighbor_spec(struct interface *ifp, int route_type, struct rip_instance *rip, u_int32_t metric)
{
    struct connected *connected = NULL;
    struct rip_interface *ri = NULL;
    struct listnode *nnode;
    struct prefix *p = NULL;
    struct sockaddr_in to;

    int vsend = 0;

    /* Check interface. */
    if (if_is_loopback(ifp))
        return;

    if (! if_is_operative(ifp))
        return;

    ri = (struct rip_interface *)ifp->info;

    if (ri->enable_inter_node != rip->id)
        return;

    if (!listcount(ri->neighbor))
        return;

    vsend = ((ri->ri_send == RI_RIP_UNSPEC) ? rip->version_send : ri->ri_send);

    for (ALL_LIST_ELEMENTS_RO(ri->neighbor, nnode, p))
    {
        /* Check each neighbor. */
        if ((connected = connected_lookup_address(ifp, p->u.prefix4)) == NULL)
        {
        	//if(IS_RIP_DEBUG_EVENT)
        	{
            	zlog_debug(RIP_DBG_EVENT, "Neighbor %s doesnt have connected network",
                      inet_ntoa(p->u.prefix4));
        	}
			continue;
        }

        if (connected->address->family != AF_INET)
        {
            continue;
        }

        /* Set destination address and port */
        memset(&to, 0, sizeof(struct sockaddr_in));
        to.sin_addr = p->u.prefix4;
        to.sin_port = htons(RIP_PORT_DEFAULT);

        /* RIP version is rip's configuration. */
		if (vsend == RIPv1)
		{
			//if (IS_RIP_DEBUG_EVENT)
			{
				zlog_debug(RIP_DBG_EVENT, "RIPv1 announce to neighbor %s ", inet_ntoa(p->u.prefix4));
			}
			rip_output_process(connected, &to, route_type, RIPv1, metric);
		}
		
		if (vsend & RIPv2)
		{
			//if (IS_RIP_DEBUG_EVENT)
			{
				zlog_debug(RIP_DBG_EVENT, "RIPv1 announce to neighbor %s ", inet_ntoa(p->u.prefix4));
			}
			rip_output_process(connected, &to, route_type, RIPv2, metric);
		}
    }

}
/* Update send to all interface and neighbor. */
void
rip_update_process(int route_type, struct rip_instance *rip, u_int32_t metric)
{
    struct listnode *node;
    struct interface *ifp;
    struct rip_interface *ri;

    /* Send RIP update to each interface. */
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        ri = (struct rip_interface *)ifp->info;
		if (ri == NULL)
		{
			zlog_err("%s(): %d ri is NULL!\n", __FUNCTION__, __LINE__);
			continue;
		}

        if (ri->enable_inter_node != rip->id)
            continue;

        rip_update_interface_spec(route_type, ifp, rip, metric);

        if (listcount(ri->neighbor))
        {
            rip_update_neighbor_spec(ifp, route_type, rip, metric);
        }
    }


}

/* RIP's periodical timer. */
int
rip_update(void *t)
{
    struct rip_instance *rip = (struct rip_instance *)(t);

    if (!rip)
        return 0;

    /* Clear timer pointer. */
    rip->t_update = 0;

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "update timer fire!");
    }
    /* Process update output. */
    rip_update_process(rip_all_route, rip, 0);

    /* Triggered updates may be suppressed if a regular update is due by
       the time the triggered update would be sent. */
    if (rip->t_triggered_interval)
    {
        /*thread_cancel(rip->t_triggered_interval);
        rip->t_triggered_interval = NULL;*/
        RIP_TIMER_OFF(rip->t_triggered_interval);
    }

    rip->trigger = 0;

    /* Register myself. */
    rip_event(RIP_UPDATE_EVENT, rip);

    return 0;
}

/* Walk down the RIP routing table then clear changed flag. */
void
rip_clear_changed_flag(struct rip_instance *rip)
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;

    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                UNSET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
                /* This flag can be set only on the first entry. */
                break;
			}
        }
    }
}

/* Triggered update interval timer. */
int
rip_triggered_interval(void *t)
{
    struct rip_instance *rip = (struct rip_instance *)(t);

    if (!rip)
        return -1;

    rip->t_triggered_interval = 0;

    if (rip->trigger)
    {
        rip->trigger = 0;
        rip_triggered_update(t);
    }

    return 0;
}

/* Execute triggered update. */
int
rip_triggered_update(void *t)
{
    int interval;
    struct rip_instance *rip = (struct rip_instance *)(t);

    if (!rip)
        return 0;

    /* Clear thred pointer. */
    rip->t_triggered_update = 0;

    /* Cancel interval timer. */
    if (rip->t_triggered_interval)
    {
        /*thread_cancel(rip->t_triggered_interval);
        rip->t_triggered_interval = NULL;*/
        RIP_TIMER_OFF(rip->t_triggered_interval);
    }

    rip->trigger = 0;

    /* Logging triggered update. */
    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "triggered update!");
    }
    /* Split Horizon processing is done when generating triggered
       updates as well as normal updates (see section 2.6). */
    rip_update_process(rip_changed_route, rip, 0);

    /* Once all of the triggered updates have been generated, the route
       change flags should be cleared. */
    rip_clear_changed_flag(rip);

    /* After a triggered update is sent, a timer should be set for a
     random interval between 1 and 5 seconds.  If other changes that
     would trigger updates occur before the timer expires, a single
     update is triggered when the timer expires. */
    interval = (random() % 5) + 1;

    /*rip->t_triggered_interval =
        thread_add_timer(master_rip, rip_triggered_interval, rip, interval);*/
	rip->t_triggered_interval = high_pre_timer_add ((char *)"ospf_t_inter_timer", \
				LIB_TIMER_TYPE_NOLOOP, rip_triggered_interval, rip, (interval)*1000);

    return 0;
}

/* Create new RIP instance and set it to global variable. */
struct rip_instance *
rip_create(const u_char id)
{
    struct rip_instance *rip;

	do
	{
    	rip = (struct rip_instance *)XCALLOC(MTYPE_RIP, sizeof(struct rip_instance));
		if (rip == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(rip == NULL);

	memset(rip, 0, sizeof(struct rip_instance));

    /* Set initial value. */
    rip->id = id;
    rip->inter_count = 0;
	rip->summaryflag = 0;
    rip->inter_sum_count = 0;
    rip->version_send = RI_RIP_VERSION_1;
    rip->version_recv = RI_RIP_VERSION_1_AND_2;
    rip->t_version2 = 0;
    rip->update_time = RIP_UPDATE_TIMER_DEFAULT;
    rip->timeout_time = RIP_TIMEOUT_TIMER_DEFAULT;
    rip->garbage_time = RIP_GARBAGE_TIMER_DEFAULT;
    rip->default_metric = RIP_DEFAULT_METRIC_DEFAULT;
    rip->distance = ROUTE_METRIC_RIP;

    /* Initialize RIP routig table. */
    rip->table = route_table_init();
    rip->peer_list = list_new();
    rip->peer_list->cmp = (int (*)(void *, void *)) rip_peer_list_cmp;
	rip->peer_list->del = (void (*) (void *))rip_peer_free;
    rip->redist_infos = list_new();

    memset(&rip_stats, 0, sizeof(rip_stats));

    /* Create read and timer thread. */
//   rip_event (RIP_READ, rip);
    rip_event(RIP_UPDATE_EVENT, rip);

    return rip;
}

/* Sned RIP request to the destination. */
int
rip_request_send(struct sockaddr_in *to, struct interface *ifp,
                 u_char version, struct connected *connected)
{
    struct rte *rte;
    struct rip_packet rip_packet;
    struct listnode *node, *nnode;

    memset(&rip_packet, 0, sizeof(rip_packet));

    rip_packet.command = RIP_REQUEST;

    if (RIPv1 == version)
    {
        rip_packet.version = version;
    }
    else
    {
        rip_packet.version = RIPv2;
    }

    rte = rip_packet.rte;
    rte->metric = htonl(RIP_METRIC_INFINITY);

    if (connected && connected->address->family == AF_INET)
    {
        /*
         * connected is only sent for ripv1 case, or when
         * interface does not support multicast.  Caller loops
         * over each connected address for this case.
         */
        if (rip_send_packet((u_char *) &rip_packet, sizeof(rip_packet),
                            to, connected, version) != sizeof(rip_packet))
            return -1;
        else
            return sizeof(rip_packet);
    }

    /* send request on each connected network */
    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, connected))
    {
        struct prefix_ipv4 *p;


        p = (struct prefix_ipv4 *) connected->address;

        if (p->family != AF_INET)
            continue;

        if (rip_send_packet((u_char *) &rip_packet, sizeof(rip_packet),
                            to, connected, version) != sizeof(rip_packet))
            return -1;
    }

    return sizeof(rip_packet);
}

int
rip_update_jitter(unsigned long time)
{
#define JITTER_BOUND 4
    /* We want to get the jitter to +/- 1/JITTER_BOUND the interval.
       Given that, we cannot let time be less than JITTER_BOUND seconds.
       The RIPv2 RFC says jitter should be small compared to
       update_time.  We consider 1/JITTER_BOUND to be small.
    */

    int jitter_input = time;
    int jitter;

    if (jitter_input < JITTER_BOUND)
        jitter_input = JITTER_BOUND;

    jitter = (((random() % ((jitter_input * 2) + 1)) - jitter_input));

    return jitter / JITTER_BOUND;
}

/* First entry point of RIP packet. */
int
rip_read_pkt(struct sockaddr_in from, uint32_t in_ifindex, void *ibuf, uint16_t len)
{
    int ret = 0;
    int rtenum = 0;

    struct rip_packet *packet = NULL;
    int vrecv;
    struct interface *ifp = NULL;
    struct connected *ifc = NULL;
    struct rip_interface *ri = NULL;
    struct rip_instance *rip = NULL;

    /* Check is this packet comming from myself? */
    if (if_check_address(from.sin_addr))
    {
        //if (IS_RIP_DEBUG_PACKET)
        {
            zlog_debug(RIP_DBG_PACKET, "ignore packet comes from myself");
        }
		return -1;
    }

    /* Which interface is this packet comes from. */
    ifp = if_lookup_address(from.sin_addr);


    /* If this packet come from unknown interface, ignore it. */
    if (ifp == NULL || (ifp->ifindex != in_ifindex))
    {
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("rip_read: cannot find interface for packet from %s port %d",
            	      inet_ntoa(from.sin_addr), ntohs(from.sin_port));
		}
        return -1;
    }
	
    if (!CHECK_FLAG(ifp->flags, IFF_UP))
    {		
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("rip_read: interface %s is down", ifp->name);
		}
        return -1;
    }

    ri = (struct rip_interface *)ifp->info;
	if(ri == NULL)
	{
		zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
		return -1;
	}
    if (!ri->enable_inter_node)
    {
		
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("rip_read: rip didn`t enabled under interface %s for packet from %s "
                  "port %d ",
                  ifp->name, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
		}
        return -1;
    }

    ifc = connected_lookup_address(ifp, from.sin_addr);
	if(ifc == NULL)
	{
		zlog_err("%-15s[%d]: ifc is NULL!", __FUNCTION__, __LINE__);
		return -1;
	}
    if (ifc == NULL)
    {
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("rip_read: cannot find connected address for packet from %s "
            	      "port %d on interface %s",
                	  inet_ntoa(from.sin_addr), ntohs(from.sin_port), ifp->name);
		}
        return -1;
    }

    /* Is RIP running or is this RIP neighbor ?*/
    rip = rip_instance_lookup(ri->enable_inter_node);
    if (!rip)
    {
        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("RIP is not enabled on interface %s.", ifp->name);
        }
        return -1;
    }
	
    /* RIP packet received */
    //if (IS_RIP_DEBUG_PACKET)
    {
        zlog_debug(RIP_DBG_PACKET, "RECV packet from %s port %d on %s, packet in_index is %02x",
                   inet_ntoa(from.sin_addr), ntohs(from.sin_port),
                   ifp ? ifp->name : "unknown", in_ifindex);
    }

	rip_stats.rx_packets++;
    ri->receive_packet++;
    /* The Response must be ignored if it is not from the RIP
       port. (RFC2453 - Sec. 3.9.2)*/
    if (from.sin_port != htons(RIP_PORT_DEFAULT))
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("response doesn't come from RIP port: %d",
            	      from.sin_port);
		}
        return -1;
    }


    /* Packet length check. */
    if (len < RIP_PACKET_MINSIZ)
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;
		
		if(IS_RIP_DEBUG_PACKET)
		{
	        zlog_err("packet size %d is smaller than minimum size %d",
	                  len, RIP_PACKET_MINSIZ);
	        rip_wrong_size_pkt_dump(ibuf, len);
		}
        return len;
    }

    if (len > RIP_PACKET_MAXSIZ)
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;
		
		if(IS_RIP_DEBUG_PACKET)
		{
	        zlog_err("packet size %d is larger than max size %d",
	                  len, RIP_PACKET_MAXSIZ);
	        rip_wrong_size_pkt_dump(ibuf, len);
		}
        return len;
    }

    /* Packet alignment check. */
    if ((len - RIP_PACKET_MINSIZ) % 20)
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;
		
		if(IS_RIP_DEBUG_PACKET)
		{
	        zlog_err("packet size %d is wrong for RIP packet alignment", len);
	        rip_wrong_size_pkt_dump(ibuf, len);
		}
        return len;
    }

    /* Set RTE number. */
    rtenum = ((len - RIP_PACKET_MINSIZ) / 20);

    /* For easy to handle. */

    packet = (struct rip_packet *) ibuf;

    /* RIP version check. */
    if (packet->version == 0)
    {
        rip_stats.rx_errors++;
		if(IS_RIP_DEBUG_PACKET)
		{
        	zlog_err("version 0 with command %d received.", packet->command);
		}
        return -1;
    }

    /* Dump RIP packet. */
    if (IS_RIP_DEBUG_PACKET)
        rip_packet_dump(packet, len, "RECV");

    /* RIP version adjust.  This code should rethink now.  RFC1058 says
       that "Version 1 implementations are to ignore this extra data and
       process only the fields specified in this document.". So RIPv3
       packet should be treated as RIPv1 ignoring must be zero field. */
    if (packet->version > RIPv2)
        packet->version = RIPv2;


    /* RIP Version check. RFC2453, 4.6 and 5.1 */
    vrecv = ((ri->ri_send == RI_RIP_UNSPEC) ?
             rip->version_recv : ri->ri_send);

    if ((packet->version == RIPv1) && !(vrecv & RIPv1))
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;

        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("  packet's v%d doesn't fit to if version spec",
                       packet->version);
        }
        return -1;
    }

    if ((packet->version == RIPv2) && !(vrecv & RIPv2))
    {
        rip_stats.rx_errors++;

        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("  packet's v%d doesn't fit to if version spec",
                       packet->version);
        }
        return -1;
    }

    /* RFC2453 5.2 If the router is not configured to authenticate RIP-2
       messages, then RIP-1 and unauthenticated RIP-2 messages will be
       accepted; authenticated RIP-2 messages shall be discarded.  */
    if ((ri->auth_type == RIP_NO_AUTH)
            && rtenum
            && (packet->version == RIPv2)
            && (packet->rte->family == htons(RIP_FAMILY_AUTH)))
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;

        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("packet RIPv%d is dropped because authentication disabled",
                       packet->version);
        }
        return -1;
    }

    /* RFC:
       If the router is configured to authenticate RIP-2 messages, then
       RIP-1 messages and RIP-2 messages which pass authentication
       testing shall be accepted; unauthenticated and failed
       authentication RIP-2 messages shall be discarded.  For maximum
       security, RIP-1 messages should be ignored when authentication is
       in use (see section 4.1); otherwise, the routing information from
       authenticated messages will be propagated by RIP-1 routers in an
       unauthenticated manner.
    */

    /* We make an exception for RIPv1 REQUEST packets, to which we'll
     * always reply regardless of authentication settings, because:
     *
     * - if there other authorised routers on-link, the REQUESTor can
     *   passively obtain the routing updates anyway
     * - if there are no other authorised routers on-link, RIP can
     *   easily be disabled for the link to prevent giving out information
     *   on state of this routers RIP routing table..
     *
     * I.e. if RIPv1 has any place anymore these days, it's as a very
     * simple way to distribute routing information (e.g. to embedded
     * hosts / appliances) and the ability to give out RIPv1
     * routing-information freely, while still requiring RIPv2
     * authentication for any RESPONSEs might be vaguely useful.
     */
    if ((RIPv1 == packet->version) && (RIP_NO_AUTH != ri->auth_type) 
				&& (RI_RIP_VERSION_2_BROAD == vrecv))
    {
        rip_stats.rx_errors++;
		ri->recv_badpackets++;

        if (IS_RIP_DEBUG_PACKET)
        {
            zlog_err("RIPv1 dropped because authentication enabled");
        }
        return -1;

    }

    if ((RIPv2 == packet->version) && (RIP_NO_AUTH != ri->auth_type))
    {
        const char *auth_desc;

        if (rtenum == 0)
        {
            rip_stats.rx_errors++;
			ri->recv_badpackets++;

            /* There definitely is no authentication in the packet. */
            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_err("RIPv2 authentication failed: no auth RTE in packet");
            }
            return -1;
        }

        /* First RTE must be an Authentication Family RTE */
        if (packet->rte->family != htons(RIP_FAMILY_AUTH))
        {
            rip_stats.rx_errors++;
			ri->recv_badpackets++;

            if (IS_RIP_DEBUG_PACKET)
            {
                zlog_err("RIPv2" " dropped because authentication enabled");
            }
            return -1;
        }

        /* Check RIPv2 authentication. */
        switch (ntohs(packet->rte->tag))
        {
            case RIP_AUTH_SIMPLE_PASSWORD:
                auth_desc = "simple";
                ret = rip_auth_simple_password(packet->rte, &from, ifp);
                break;

            case RIP_AUTH_MD5:
                auth_desc = "MD5";
                ret = rip_auth_md5(packet, &from, len, ifp);
                /* Reset RIP packet length to trim MD5 data. */
                len = ret;
                break;

            default:
                ret = 0;
                auth_desc = "unknown type";

                //if (IS_RIP_DEBUG_PACKET)
                {
                    zlog_debug(RIP_DBG_PACKET, "RIPv2 Unknown authentication type %d",
                               ntohs(packet->rte->tag));
                }
        }

        if (ret)
        {
            //if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug(RIP_DBG_PACKET, "RIPv2 %s authentication success", auth_desc);
            }
        }
        else
        {
            rip_stats.rx_auth_failed++;
			ri->recv_badpackets++;

            //if (IS_RIP_DEBUG_PACKET)
            {
                zlog_debug(RIP_DBG_PACKET, "RIPv2 %s authentication failure", auth_desc);
            }
            return -1;
        }
    }


    /* Process each command. */
    switch (packet->command)
    {
        case RIP_RESPONSE:
            rip_response_process(packet, len, &from, ifc, rip, ri);
            break;
        case RIP_REQUEST:
        case RIP_POLL:
            rip_request_process(packet, len, &from, ifc);
            break;
        case RIP_TRACEON:
        case RIP_TRACEOFF:
			if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_err("Obsolete command %s received", lookup(rip_msg, packet->command));
			}
            break;
        case RIP_POLL_ENTRY:
			if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_err("Obsolete command %s received", lookup(rip_msg, packet->command));
			}
            break;
        default:
			if(IS_RIP_DEBUG_PACKET)
			{
            	zlog_err("Unknown RIP command %d received", packet->command);
			}
            rip_stats.rx_errors++;
			ri->recv_badpackets++;
            break;
    }

    return len;
}

/* separate ipv4 and ipv6 packet according to ip */
int
rip_ipv4_or6_seperate(struct pkt_buffer *pkt)
{
    struct sockaddr_in from4;
    struct sockaddr_in6 from6;
    struct ip_control *p_ipcb = NULL;
    uint32_t ipv4 = 0;

    memset(&from4, 0, sizeof(struct sockaddr_in));
    memset(&from6, 0, sizeof(struct sockaddr_in6));

    p_ipcb = (struct ip_control *) & (pkt->cb);

    if (p_ipcb->sip)
    {
        if (NULL == rip_list)
        {
            return 0;
        }

        ipv4 = htonl(p_ipcb->sip);
        from4.sin_family = AF_INET;
        from4.sin_port = htons(p_ipcb->sport);
        from4.sin_addr = *(struct in_addr *) & ipv4;
        from4.sin_zero[7] = 0;

		rip_read_pkt(from4, pkt->in_ifindex, pkt->data, pkt->data_len);
		
    }
    else if (p_ipcb->sipv6.ipv6)
    {
				return 0;
        if (NULL == ripng_list)
        {
            return 0;
        }

        from6.sin6_family = AF_INET6;
        from6.sin6_port = htons(p_ipcb->sport);
        IPV6_ADDR_COPY(&from6.sin6_addr.s6_addr, &p_ipcb->sipv6);

        ripng_read(from6, pkt->in_ifindex, pkt->data, pkt->data_len);
    }
    else
    {
        return -1;
    }

    return 0;
}

#if 0

int
add_rip_fifo_pkt(struct pkt_buffer *recvpkt)
{
    struct pkteventfifo *newpkt = NULL;

	do
	{
    	newpkt = XCALLOC(MTYPE_IFMEVENT_FIFO, sizeof(struct pkteventfifo));
		if (newpkt == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newpkt == NULL);

	memset(newpkt, 0, sizeof(struct pkteventfifo));

    if (newpkt == NULL)
    {
        zlog_err("%-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return -1;
    }
	newpkt->fifo.next = NULL;
    memcpy(&newpkt->pkt, recvpkt, sizeof(struct pkt_buffer));
	memcpy(&newpkt->pkt_data, recvpkt->data, recvpkt->data_len);
	newpkt->pkt.data = &newpkt->pkt_data;
    FIFO_ADD(&(rip_m.pktfifo), &(newpkt->fifo));
    return 0;
}

void
read_pkt_form_pktfifo(void)
{
    struct pkteventfifo *readpkt = NULL;
    int item = 0;
    int maxcount = 200;
	
    while (!FIFO_EMPTY(&rip_m.pktfifo) && (item++ < maxcount))
    {
        readpkt = (struct pkteventfifo *)FIFO_TOP(&rip_m.pktfifo);
        rip_ipv4_or6_seperate(&(readpkt->pkt));
        FIFO_DEL(readpkt);
        XFREE(MTYPE_IFMEVENT_FIFO, readpkt);
		readpkt = NULL;
    }
}

/* receive pkt form ipc */
int rip_pkt_rcv(struct thread *thread)
{
    struct pkt_buffer *brpkt = NULL;
    int i = 0;
	
	rip_m.pkt_recv = NULL;	
    for (i = 500; i >= 0; i--)
    {
        brpkt = pkt_rcv(MODULE_ID_RIP);

        if (brpkt == NULL)
        {
            break;/* 退出 while 循环 */
        }

		if(brpkt->data == NULL)
		{
			zlog_err("%-15s[%d]: brpkt->data is NULL! data_len = %d", __FUNCTION__, __LINE__, brpkt->data_len);
			continue;
		}
		
		add_rip_fifo_pkt(brpkt);
   		brpkt = NULL;
    }
	
	if (!FIFO_EMPTY(&rip_m.pktfifo))
	{
		read_pkt_form_pktfifo();
	}

    usleep(1000);

	do{
    	rip_m.pkt_recv = thread_add_event_normal(master_rip, rip_pkt_rcv, NULL, 0);  /* thread to receive packet */
		if(!rip_m.pkt_recv)
			zlog_err("%-15s[%d]: can`t add reak pkt event", __func__, __LINE__);
	}
	while(!rip_m.pkt_recv);
    return 0;

}
#endif

/* receive pkt form ipc */
int rip_pkt_rcv_new(struct ipc_mesg_n *pmsg)
{
    struct pkt_buffer *brpkt = NULL;
	
	//rip_m.pkt_recv = NULL;	
    
    brpkt = (struct pkt_buffer *)pmsg->msg_data;
    if (brpkt == NULL)
    {
        return -1;/* 退出 while 循环 */
    }
	APP_RECV_PKT_FROM_FTM_DATA_SET(brpkt);

	if(brpkt->data == NULL)
	{
		zlog_err("%-15s[%d]: brpkt->data is NULL! data_len = %d", __FUNCTION__, __LINE__, brpkt->data_len);
		return -1;
	}
	
	/*add_rip_fifo_pkt(brpkt);
	
	if (!FIFO_EMPTY(&rip_m.pktfifo))
	{
		read_pkt_form_pktfifo();
	}*/

	rip_ipv4_or6_seperate(brpkt);

    return 0;

}


struct rip_instance *
rip_instance_lookup(const u_char id)
{
    struct rip_instance *rip = NULL;
    struct listnode *node = NULL;

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
    {
        if (rip->id && id && (rip->id == id))
        {
            return rip;
        }
    }

    return NULL;
}

struct rip_instance *
rip_instance_get(struct vty *vty, const u_char id)
{
    struct rip_instance *rip = NULL;

    rip = rip_instance_lookup(id);

    if (!rip)
    {
        rip = rip_create(id);
        listnode_add(rip_list, rip);
    }

    if (vty)
    {
        vty->node = RIP_NODE;
        vty->index = rip;
    }

    return rip;
}

int rip_instance_delete(struct vty *vty, const u_char id)
{
    struct rip_instance *rip;

    rip = rip_instance_lookup(id);

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR
        return CMD_SUCCESS;
    }

    rip_update_process(rip_all_route, rip, RIP_METRIC_INFINITY);
    rip_clean(rip);
    listnode_delete(rip_list, rip);
    XFREE(MTYPE_RIP, rip);
	rip = NULL;
    return 0;
}

void
rip_table_distance_change(struct rip_instance *rip)
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *node = NULL;

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL && listcount(list) != 0)
        {
            for (ALL_LIST_ELEMENTS_RO(list, node, rinfo))
            {
                if (!rinfo)
                    continue;

				if ((CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
					|| CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_INTERFACE))
					&& rinfo->metric != RIP_METRIC_INFINITY)
                {
                    rinfo->distance = rip->distance;
                    rip_zebra_ipv4_update(rp);
                }
            }
        }
    }
}
#if 0
static void
version_change_table_check(struct rip_instance *rip, int version)
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct rip_peer *peer = NULL;
    struct list *list = NULL;
    struct listnode *node = NULL;
    struct listnode *rnode = NULL;
    struct listnode *nnode = NULL;
//  u_int32_t metric = RIP_METRIC_INFINITY;

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((NULL == (list = rp->info)) || !listcount(list))
            continue;

        for (ALL_LIST_ELEMENTS(list, node, rnode, rinfo))
        {
            if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE))
            {

                for (ALL_LIST_ELEMENTS_RO(rip->peer_list, nnode, peer))
                {
                    if ((version != peer->version)
                            && (IPV4_ADDR_SAME(&peer->addr.s_addr, &rinfo->from.s_addr)))
                    {

                        rip_ecmp_delete(rinfo, rip);
                    }
                }
            }
        }
    }
}
#endif

int
rip_version2_on(void *t)
{
    struct rip_instance *rip = NULL;

    rip = (struct rip_instance *)(t);

    if (!rip)
        return 0;

    rip->t_version2 = 0;
    return 0;
}

void
rip_event(enum rip_event event, struct rip_instance *rip)
{
    int jitter = 0;

    switch (event)
    {
        case RIP_UPDATE_EVENT:

            if (rip->t_update)
            {
                /*thread_cancel(rip->t_update);
                rip->t_update = NULL;*/
                RIP_TIMER_OFF(rip->t_update);
            }

            jitter = rip_update_jitter(rip->update_time);
			if(0 == rip->t_update)
			{
            	/*rip->t_update =
                	thread_add_timer(master_rip, rip_update, rip, rip->update_time + jitter);*/
				rip->t_update = high_pre_timer_add ((char *)"rip_ud_timer",\
						LIB_TIMER_TYPE_NOLOOP, rip_update, rip, (rip->update_time + jitter)*1000);
			}
            break;
        case RIP_TRIGGERED_UPDATE:

            if (rip->t_triggered_interval)
            {
                rip->trigger = 1;
            }
            else
            {
                if (0 == rip->t_triggered_update)
                {
                    //rip->t_triggered_update = thread_add_timer(master_rip, rip_triggered_update, rip, 1);
					rip->t_triggered_update = high_pre_timer_add ((char *)"rip_t_ud_timer",\
								LIB_TIMER_TYPE_NOLOOP, rip_triggered_update, rip, 1000);
                }
            }

            break;
        default:
            break;
    }
}

DEFUN(router_rip,
      router_rip_cmd,
      "rip instance <1-255>",
      RIP_STR
      "specified an instance\n"
      "Instance number\n")
{
    u_char id;
    struct rip_instance *rip = NULL;

    id = (u_char)atoi(argv[0]);
    rip = rip_instance_get(vty, id);

    if (rip)
        return CMD_SUCCESS;
    else
        return CMD_WARNING;
}

DEFUN(no_router_rip,
      no_router_rip_cmd,
      "no rip instance <1-255>",
      NO_STR
      RIP_STR
      "specified an instance\n"
      "Instance number\n")
{
    u_char id;
    id = (u_char)atoi(argv[0]);

    return rip_instance_delete(vty, id);
}

DEFUN(rip_version,
      rip_version_cmd,
      "version <1-2>",
      "Set RIP version\n"
      "version of RIP instance\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
    int version;
	int pver;

    version = atoi(argv[0]);

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    if (rip->version_recv == version)
        return CMD_SUCCESS;

//    version_change_table_check(rip, version);

    if (version == RI_RIP_VERSION_1 && rip->version_send == RI_RIP_VERSION_2)
    {
    	RIP_TIMER_OFF(rip->t_version2);
    }

    if (version == RI_RIP_VERSION_2 && rip->version_send == RI_RIP_VERSION_1 && 0 == rip->t_version2)
    {
        //rip->t_version2 = thread_add_timer(master_rip, rip_version2_on, rip, rip->garbage_time);
		rip->t_version2 = high_pre_timer_add ((char *)"rip_ver_timer",\
					LIB_TIMER_TYPE_NOLOOP, rip_version2_on, rip, (rip->garbage_time)*1000);
    }
	pver = rip->version_recv;
    rip->version_send = version;
    rip->version_recv = version;

    version_change_all_interface(rip, pver, rip->version_send);

    return CMD_SUCCESS;
}

DEFUN(no_rip_version,
      no_rip_version_cmd,
      "no version",
      NO_STR
      "Set RIP version to default\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;
	int pver;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    if (rip->version_recv != RI_RIP_VERSION_1_AND_2)
    {
    	pver = rip->version_recv;
        /* Set RIP version to the default. */
        rip->version_send = RI_RIP_VERSION_1;
        rip->version_recv = RI_RIP_VERSION_1_AND_2;
        version_change_all_interface(rip, pver, rip->version_recv);
    }

	RIP_TIMER_OFF(rip->t_version2);

    return CMD_SUCCESS;
}

DEFUN(rip_timers,
      rip_timers_cmd,
      "timers update <1-3600> age <1-3600> garbage-collect <1-3600>",
      "Set routing timers\n"
      "Basic routing protocol update timers\n"
      "Routing table update timer value in second. Default is 30.\n"
      "Basic routing protocol age timers\n"
      "Routing table update timer value in second. Default is 180.\n"
      "Basic routing protocol garbage-collect timers\n"
      "Routing table update timer value in second. Default is 120.\n")
{
    unsigned long update;
    unsigned long timeout;
    unsigned long garbage;
    char *endptr = NULL;
    unsigned long RIP_TIMER_MAX = 3600;
    unsigned long RIP_TIMER_MIN = 1;
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    update = strtoul(argv[0], &endptr, 10);

    if (update > RIP_TIMER_MAX || update < RIP_TIMER_MIN || *endptr != '\0')
    {
        vty_error_out(vty, "Update timer value error%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    timeout = strtoul(argv[1], &endptr, 10);

    if (timeout > RIP_TIMER_MAX || timeout < RIP_TIMER_MIN || *endptr != '\0')
    {
        vty_error_out(vty, "Timeout timer value error%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    garbage = strtoul(argv[2], &endptr, 10);

    if (garbage > RIP_TIMER_MAX || garbage < RIP_TIMER_MIN || *endptr != '\0')
    {
        vty_error_out(vty, "Garbage timer value error%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (update >= timeout || update >= garbage)
    {
        vty_error_out(vty, "Update time should be small than age time and garbage-collect time%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Set each timer value. */
    rip->update_time = update;
    rip->timeout_time = timeout;
    rip->garbage_time = garbage;

    /* Reset update timer thread. */
    rip_event(RIP_UPDATE_EVENT, rip);

    return CMD_SUCCESS;
}

DEFUN(no_rip_timers,
      no_rip_timers_cmd,
      "no timers",
      NO_STR
      "Set routing timers\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    /* Set each timer value to the default. */
    rip->update_time = RIP_UPDATE_TIMER_DEFAULT;
    rip->timeout_time = RIP_TIMEOUT_TIMER_DEFAULT;
    rip->garbage_time = RIP_GARBAGE_TIMER_DEFAULT;

    /* Reset update timer thread. */
    rip_event(RIP_UPDATE_EVENT, rip);

    return CMD_SUCCESS;
}

int
rip_summary_on(void *t)
{
    struct rip_instance *rip = NULL;

    rip = (struct rip_instance *)(t);

    if (!rip)
        return 0;

    rip->t_summary_set = 0;
    return 0;
}

int
rip_summary_off(void *t)
{
    struct rip_instance *rip = NULL;

    rip = (struct rip_instance *)(t);

    if (!rip)
        return 0;

    rip->t_summary_unset = 0;
    return 0;
}

DEFUN(rip_distance,
      rip_distance_cmd,
      "distance <1-255>",
      "Set RIP route distance\n"
      "Distance value\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    rip->distance = atoi(argv[0]);
    rip_table_distance_change(rip);
    return CMD_SUCCESS;
}

DEFUN(no_rip_distance,
      no_rip_distance_cmd,
      "no distance",
      NO_STR
      "Set RIP route distance\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_ERR_NOTHING_TODO;
    }

    rip->distance = ROUTE_METRIC_RIP;
    rip_table_distance_change(rip);
    return CMD_SUCCESS;
}

DEFUN(rip_summary_all,
      rip_summary_all_cmd,
      "summary",
      "RIP summary all subnet routes\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_WARNING;
    }

    if (rip->summaryflag)
    {
        vty_error_out(vty, "Summary function was already enabled%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    rip->summaryflag = 1;

    if (rip->t_summary_unset)
    {
        /*thread_cancel(rip->t_summary_unset);
        rip->t_summary_unset = NULL;*/
        RIP_TIMER_OFF(rip->t_summary_unset);
    }

    if (rip->version_send == RI_RIP_VERSION_1)
    {
        return CMD_SUCCESS;
    }

	if(0 ==  rip->t_summary_set)
	{
    	//rip->t_summary_set = thread_add_timer(master_rip, rip_summary_on, rip, rip->garbage_time);
		rip->t_summary_set = high_pre_timer_add ((char *)"rip_sum_timer", \
					LIB_TIMER_TYPE_NOLOOP, rip_summary_on, rip, (rip->garbage_time)*1000);
	}
    rip_update_process(rip_all_route, rip, 0);
    return CMD_SUCCESS;
}

DEFUN(no_rip_summary_all,
      no_rip_summary_all_cmd,
      "no summary",
      NO_STR
      "RIP summary all subnet routes\n")
{
    struct rip_instance *rip = (struct rip_instance *)vty->index;

    if (rip == NULL)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_WARNING;
    }

    if (!rip->summaryflag)
    {
        vty_error_out(vty, "Summary function was  already disabled%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    rip->summaryflag = 0;

    if (0 == rip->t_summary_unset)
    {
        //rip->t_summary_unset = thread_add_timer(master_rip, rip_summary_off, rip, rip->garbage_time);
		rip->t_summary_unset = high_pre_timer_add ((char *)"rip_sum_u_timer",\
				LIB_TIMER_TYPE_NOLOOP, rip_summary_off, rip, (rip->garbage_time)*1000);
    }

    if (rip->version_send == RI_RIP_VERSION_1)
    {
        return CMD_SUCCESS;
    }
	
    rip_update_process(rip_all_route, rip, 0);
    return CMD_SUCCESS;
}


static const char *
rip_route_type_print(int sub_type)
{

    if (CHECK_FLAG(sub_type, RIP_ROUTE_INTERFACE))
    {
        return "i";
    }
    else if (CHECK_FLAG(sub_type, RIP_ROUTE_RTE))
    {
        return "n";
    }
    else if (CHECK_FLAG(sub_type, RIP_ROUTE_REDISTRIBUTE))
    {
        return "r";
    }
    else if (CHECK_FLAG(sub_type, RIP_ROUTE_SUMMARY))
    {
        return "s";
    }
    else
    {
        return "?";
    }
}

struct rip_desc_table
{
    unsigned int type;
    const char *string;
    char chr;
};

#define DESC_ENTRY(T,S,C) [(T)] = { (T), (S), (C) }

const struct rip_desc_table rip_unknown = { 0, "unknown", '?' };
const struct rip_desc_table rip_route_types[] =
{
    DESC_ENTRY(ROUTE_PROTO_CONNECT,    "connected",   'C'),
    DESC_ENTRY(ROUTE_PROTO_STATIC,     "static",  'S'),
    DESC_ENTRY(ROUTE_PROTO_RIP,    "rip", 'R'),
    DESC_ENTRY(ROUTE_PROTO_RIPNG,  "ripng",   'R'),
    DESC_ENTRY(ROUTE_PROTO_OSPF,   "ospf",    'O'),
    DESC_ENTRY(ROUTE_PROTO_OSPF6,  "ospf6",   'O'),
    DESC_ENTRY(ROUTE_PROTO_ISIS,   "isis",    'I'),
	DESC_ENTRY(ROUTE_PROTO_ISIS6,  "isis6",	  'I'),
    DESC_ENTRY(ROUTE_PROTO_IBGP,   "ibgp",    'B'),
    DESC_ENTRY(ROUTE_PROTO_EBGP,   "ebgp",    'E'),
};
#undef DESC_ENTRY

const struct rip_desc_table *
rip_zroute_lookup(u_int zroute)
{
    u_int i;

    if (zroute >= array_size(rip_route_types))
    {
    	//if(IS_RIP_DEBUG_EVENT)
    	{
        	zlog_debug(RIP_DBG_EVENT, "unknown zebra route type: %u", zroute);
    	}
        return &rip_unknown;
    }

    if (zroute == rip_route_types[zroute].type)
        return &rip_route_types[zroute];

    for (i = 0; i < array_size(rip_route_types); i++)
    {
        if (zroute == rip_route_types[i].type)
        {
			//if(IS_RIP_DEBUG_EVENT)
			{
            	zlog_debug(RIP_DBG_EVENT, "internal error: route type table out of order "
                	      "while searching for %u, please notify developers", zroute);
			}
            return &rip_route_types[i];
        }
    }
	
	//if(IS_RIP_DEBUG_EVENT)
	{
	    zlog_debug(RIP_DBG_EVENT, "internal error: cannot find route type %u in table!", zroute);
	}
    return &rip_unknown;
}

const char *
rip_route_string(u_int zroute)
{
    return rip_zroute_lookup(zroute)->string;
}

char
rip_zebra_route_char(u_int zroute)
{
    return rip_zroute_lookup(zroute)->chr;
}

#if 0
/* Print out routes update time. */
void
rip_vty_out_uptime(struct vty *vty, struct rip_info *rinfo)
{
    time_t clock;
    struct tm *tm;
#define TIME_BUF 25
    char timebuf [TIME_BUF];
    struct thread *thread;

    if ((thread = rinfo->t_timeout) != NULL)
    {
        clock = thread_timer_remain_second(thread);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, " %5s", timebuf);
    }
    else if ((thread = rinfo->t_garbage_collect) != NULL)
    {
        clock = thread_timer_remain_second(thread);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, " %5s", timebuf);
    }
}
#endif
/* Print out routes update time. */
void
rip_vty_out_uptime_new(struct vty *vty, struct rip_info *rinfo, struct rip_instance *rip)
{
    time_t clock;
    struct tm *tm;
#define TIME_BUF 25
    char timebuf [TIME_BUF];
    //struct thread *thread;
    TIMERID 		thread;

    if ((thread = rinfo->t_timeout) != 0)
    {
        clock = rip_thread_timer_remain_second(thread, rip->timeout_time);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, " %5s", timebuf);
    }
    else if ((thread = rinfo->t_garbage_collect) != 0)
    {
        clock = rip_thread_timer_remain_second(thread, rip->timeout_time);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, " %5s", timebuf);
    }
}


DEFUN(show_ip_rip,
      show_ip_rip_cmd,
      "show rip <1-255> route",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      "Route information\n")
{
    struct route_node *np;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    struct rip_instance *rip;
    u_char id;
	char name[INTERFACE_NAMSIZ];

    id = (u_char)atoi(argv[0]);
    rip = rip_instance_lookup(id);

    if (! rip)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    vty_out(vty, "Codes: R - RIP, C - connected, S - Static, O - OSPF, B - IBGP, I - ISIS%s"
            " 	E - EBGP %s"
            "Sub-codes: (n) - normal, (r) - redistribute, (i) - interface%s%s"
            "     Network             Next Hop       Metric  Via            Tag Time%s",
            VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    for (np = route_top(rip->table); np; np = route_next(np))
    {

        if ((list = (struct list *)np->info) != NULL)
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                int len;

                if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY) && !CHECK_FLAG(rinfo->sub_type, ~RIP_ROUTE_SUMMARY))
                    continue;

				/* np->lock, For debugging. */
                len = vty_out(vty, "%c(%s) %s/%d",
                              rip_zebra_route_char(rinfo->type),
                              rip_route_type_print(rinfo->sub_type),
                              inet_ntoa(np->p.u.prefix4), np->p.prefixlen);

                len = 25 - len;

                if (len > 0)
                    vty_out(vty, "%*s", len, " ");

                if (rinfo->nexthop.s_addr)
                    vty_out(vty, "%-16s %-5d ", inet_ntoa(rinfo->nexthop),
                            rinfo->metric);
                else
                    vty_out(vty, "0.0.0.0          %-5d ", rinfo->metric);

                /* Route which exist in kernel routing table. */
                if ((rinfo->type == ROUTE_PROTO_RIP) &&
                        CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE))
                {
					memset(name, 0, INTERFACE_NAMSIZ);
					get_interface_for_short(rinfo->ifindex, name);
                    vty_out(vty, "%-15s ", name);
                    vty_out(vty, "%1d ", rinfo->tag);
                    //rip_vty_out_uptime(vty, rinfo);
                    rip_vty_out_uptime_new(vty, rinfo, rip);
                }
                else if (rinfo->metric == RIP_METRIC_INFINITY)
                {
                    vty_out(vty, "kill            ");
                    vty_out(vty, "%1d ", rinfo->tag);
                    //rip_vty_out_uptime(vty, rinfo);
                    rip_vty_out_uptime_new(vty, rinfo, rip);
                }
                else
                {
                    if (rinfo->external_id)
                    {
                        len = vty_out(vty, "self[%d]", rinfo->external_id);
                        len = 16 - len;
                        if (len > 0)
                            vty_out(vty, "%*s", len, " ");
                    }
                    else
                        vty_out(vty, "self            ");

                    vty_out(vty, "%1d", rinfo->tag);
                }

                vty_out(vty, "%s", VTY_NEWLINE);
            }
    }

    return CMD_SUCCESS;
}

/* Vincent: formerly, it was show_ip_protocols_rip: "show ip protocols" */
DEFUN(show_ip_rip_status,
      show_ip_rip_status_cmd,
      "show rip instance <1-255>",
      SHOW_STR
      RIP_STR
      "RIP routing protocol instance\n"
      "rip instance number\n")
{
    struct rip_instance *rip;
    u_char id;

    id = (u_char)atoi(argv[0]);

    rip = rip_instance_lookup(id);

    if (! rip)
    {
		VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    vty_out(vty, "Routing Protocol: \"rip\", instance: %d%s", rip->id, VTY_NEWLINE);
    vty_out(vty, "  RIP version: %s%s", 
		rip->version_send == RI_RIP_VERSION_1 ? (rip->version_recv == RI_RIP_VERSION_1 ? "1" : "1 Compatible") : "2", VTY_NEWLINE);
    vty_out(vty, "  Sending updates every %ld seconds with +/-50%%,", rip->update_time);
    /*vty_out(vty, " next due in %lu seconds%s",
            thread_timer_remain_second(rip->t_update),
            VTY_NEWLINE);*/
	vty_out(vty, " next due in %lu seconds%s",
            rip_thread_timer_remain_second(rip->t_update, rip->update_time),
            VTY_NEWLINE);
    vty_out(vty, "  Timeout after %ld seconds,", rip->timeout_time);
    vty_out(vty, " garbage collect after %ld seconds%s", rip->garbage_time,
            VTY_NEWLINE);
    vty_out(vty, "  maximum number of ECMP: %d%s", NHP_ECMP_NUM, VTY_NEWLINE);

    /* Filtering status show. */
// config_show_distribute(vty);

    /* RIP summary configuration */
    vty_out(vty, "  summary: %s%s", rip->summaryflag ? "enable" : "disable", VTY_NEWLINE);


    /* Default metric and distance information. */
    vty_out(vty, "  rip distance: %d%s", rip->distance, VTY_NEWLINE);
    vty_out(vty, "  enable interface: %d%s", rip->inter_count, VTY_NEWLINE);

    /* Redistribute information. */
    vty_out(vty, "  Default redistribution metric: %d%s", rip->default_metric, VTY_NEWLINE);
    vty_out(vty, "  Redistributing:");
    config_write_rip_redistribute(vty, rip);
    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(show_ip_rip_statistics,
      show_ip_rip_statistics_cmd,
      "show rip statistics",
      SHOW_STR
      RIP_STR
      "RIP statistics information\n")
{
    struct rip_instance *rip;
    struct route_node *np;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    struct listnode *riplistnode = NULL;
//  u_char id;
    long i = 0;
    long j = 0;
    long k = 0;

    vty_out(vty, "RIP Statistical information is %s", VTY_NEWLINE);
    vty_out(vty, " 	packet sent is %lu %s", rip_stats.tx_packets, VTY_NEWLINE);
    vty_out(vty, " 	packet received is %lu %s", rip_stats.rx_packets, VTY_NEWLINE);
    vty_out(vty, " 	error packet received is %lu %s", rip_stats.rx_errors, VTY_NEWLINE);
    vty_out(vty, " 	packet send failed is %lu %s", rip_stats.tx_failed, VTY_NEWLINE);
    vty_out(vty, " 	routes received is %lu %s", rip_stats.rx_routes, VTY_NEWLINE);
    vty_out(vty, " 	bad routes received is %lu %s", rip_stats.rx_bad_routes, VTY_NEWLINE);
    vty_out(vty, " 	received authentication failed is %lu %s", rip_stats.rx_auth_failed, VTY_NEWLINE);
    vty_out(vty, " 	routes send is %lu %s", rip_stats.tx_routes, VTY_NEWLINE);
//  vty_out (vty, "     routes add to route is %lu %s", rip_global_route_add, VTY_NEWLINE);
//  vty_out (vty, "     routes delete from route is %lu %s", rip_global_route_del, VTY_NEWLINE);


    for (ALL_LIST_ELEMENTS_RO(rip_list, riplistnode, rip))
    {

        i = 0;
        j = 0;
        k = 0;

        for (np = route_top(rip->table); np; np = route_next(np))
        {
            if ((list = (struct list *)np->info) != NULL)
                for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
                {
                    if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY) && !CHECK_FLAG(rinfo->sub_type, ~RIP_ROUTE_SUMMARY))
                        continue;

                    i++;

                    if (rinfo->metric != RIP_METRIC_INFINITY)
                    {
                        j++;

                        if ((rinfo->type == ROUTE_PROTO_RIP) &&
                                CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE))
                            k++;
                    }
                }
        }

        vty_out(vty, "   ------------- rip instance %d------------ %s", rip->id, VTY_NEWLINE);

        vty_out(vty, " 	all routes in table is %lu %s", i, VTY_NEWLINE);
        vty_out(vty, " 	active routes in table is %lu %s", j, VTY_NEWLINE);
        vty_out(vty, " 	learned active routes in table is %lu %s", k, VTY_NEWLINE);

    }

    return CMD_SUCCESS;

}

/* RIP configuration write function. */
int
config_write_rip(struct vty *vty)
{
    int write = 0;
    struct rip_instance *rip;
    struct listnode *node;

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
    {
        if (rip)
        {
            /* Router RIP statement. */
            vty_out(vty, "rip instance %d%s", rip->id, VTY_NEWLINE);
            write++;

            /* RIP version statement.  Default is RIP version 2. */
            if (rip->version_send != RI_RIP_VERSION_1
                    || rip->version_recv != RI_RIP_VERSION_1_AND_2)
                vty_out(vty, " version %d%s", rip->version_send,
                        VTY_NEWLINE);

            /* RIP timer configuration. */
            if (rip->update_time != RIP_UPDATE_TIMER_DEFAULT
                    || rip->timeout_time != RIP_TIMEOUT_TIMER_DEFAULT
                    || rip->garbage_time != RIP_GARBAGE_TIMER_DEFAULT)
                vty_out(vty, " timers update %lu age %lu garbage-collect %lu%s",
                        rip->update_time,
                        rip->timeout_time,
                        rip->garbage_time,
                        VTY_NEWLINE);

            /* Redistribute configuration. */
            config_write_rip_redistribute(vty, rip);

            /* RIP offset-list configuration. */
            config_write_rip_offset_list(vty);

            /* RIP enabled network and interface configuration. */

            /* RIP default metric configuration */
            if (rip->default_metric != RIP_DEFAULT_METRIC_DEFAULT)
                vty_out(vty, " ip rip metric %d%s",
                        rip->default_metric, VTY_NEWLINE);

            /* RIP summary configuration */
            if (rip->summaryflag)
                vty_out(vty, " summary %s", VTY_NEWLINE);

            /* Distribute configuration. */
//     write += config_write_distribute (vty);

            /* Distance configuration. */
            if (rip->distance != ROUTE_METRIC_RIP)
                vty_out(vty, " distance %d%s", rip->distance, VTY_NEWLINE);

        }
    }

    return write;
}

/* RIP node structure. */
static struct cmd_node rip_node =
{
    RIP_NODE,
    "%s(config-rip)# ",
    1
};

/* Distribute-list update functions. */
void
rip_distribute_update(struct distribute *dist)
{
    struct interface *ifp;
    struct rip_interface *ri;
    struct access_list *alist;
    struct prefix_list *plist;

    if (! dist->ifname)
        return;

    ifp = if_lookup_by_name(dist->ifname);

    if (ifp == NULL)
        return;

    ri = (struct rip_interface *)ifp->info;

    if (dist->list[DISTRIBUTE_IN])
    {
        alist = access_list_lookup(AFI_IP, dist->list[DISTRIBUTE_IN]);

        if (alist)
            ri->list[RIP_FILTER_IN] = alist;
        else
            ri->list[RIP_FILTER_IN] = NULL;
    }
    else
        ri->list[RIP_FILTER_IN] = NULL;

    if (dist->list[DISTRIBUTE_OUT])
    {
        alist = access_list_lookup(AFI_IP, dist->list[DISTRIBUTE_OUT]);

        if (alist)
            ri->list[RIP_FILTER_OUT] = alist;
        else
            ri->list[RIP_FILTER_OUT] = NULL;
    }
    else
        ri->list[RIP_FILTER_OUT] = NULL;

    if (dist->prefix[DISTRIBUTE_IN])
    {
        plist = prefix_list_lookup(AFI_IP, dist->prefix[DISTRIBUTE_IN]);

        if (plist)
            ri->prefix[RIP_FILTER_IN] = plist;
        else
            ri->prefix[RIP_FILTER_IN] = NULL;
    }
    else
        ri->prefix[RIP_FILTER_IN] = NULL;

    if (dist->prefix[DISTRIBUTE_OUT])
    {
        plist = prefix_list_lookup(AFI_IP, dist->prefix[DISTRIBUTE_OUT]);

        if (plist)
            ri->prefix[RIP_FILTER_OUT] = plist;
        else
            ri->prefix[RIP_FILTER_OUT] = NULL;
    }
    else
        ri->prefix[RIP_FILTER_OUT] = NULL;
}

void
rip_distribute_update_interface(struct interface *ifp)
{
    struct distribute *dist;

    dist = distribute_lookup(ifp->name);

    if (dist)
        rip_distribute_update(dist);
}

/* Update all interface's distribute list. */
/* ARGSUSED */
void
rip_distribute_update_all(struct prefix_list *notused)
{
    struct interface *ifp;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(iflist, node, nnode, ifp))
    {
        rip_distribute_update_interface(ifp);
    }
}
/* ARGSUSED */
void
rip_distribute_update_all_wrapper(struct access_list *notused)
{
    rip_distribute_update_all(NULL);
}

void rip_clean_all()
{
    struct rip_instance *rip;
    struct listnode *node, *nnode;

    if (listcount(rip_list))
    {
        for (ALL_LIST_ELEMENTS(rip_list, node, nnode, rip))
        {
			rip_update_process(rip_all_route, rip, RIP_METRIC_INFINITY);
            rip_clean(rip);
            listnode_delete(rip_list, rip);
            XFREE(MTYPE_RIP, rip);
			rip = NULL;
        }
    }
	
	if(rip_list)
	{
		list_free(rip_list);
		rip_list = NULL;
	}
}
/* Delete all added rip route. */
void
rip_clean(struct rip_instance *rip)
{
    int i;
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *node, *nnode;
    struct rip_peer *peer;
    
    if (rip)
    {
        /* Clear RIP routes */
        for (rp = route_top(rip->table); rp; rp = route_next(rp))
        {
            if ((list = (struct list *)rp->info) != NULL)
            {
                for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
                {
                    if (!rinfo)
                        continue;

                    if (rip_route_rte(rinfo) && CHECK_FLAG(rinfo->flags, RIP_RTF_FIB))
                    {
                        rip_zebra_ipv4_delete(rp);
                    }

                    RIP_TIMER_OFF(rinfo->t_timeout);
                    RIP_TIMER_OFF(rinfo->t_garbage_collect);
                    rip_info_free(rinfo);
					rinfo = NULL;
                }

                list_delete(list);
                rp->info = NULL;				
				route_unlock_node(rp);
            }
        }

        if (rip->table)
        {
            route_table_finish(rip->table);
            rip->table = NULL;
        }

        /* Cancel RIP related timers. */
        RIP_TIMER_OFF(rip->t_summary_set);
        RIP_TIMER_OFF(rip->t_summary_unset);
        RIP_TIMER_OFF(rip->t_version2);
        RIP_TIMER_OFF(rip->t_update);
        RIP_TIMER_OFF(rip->t_triggered_update);
        RIP_TIMER_OFF(rip->t_triggered_interval);

		if(rip->peer_list)
		{
	 		for (ALL_LIST_ELEMENTS(rip->peer_list, node, nnode, peer))
			{
				if(!peer)
					continue;
				RIP_TIMER_OFF(peer->t_timeout);
			}
		    list_delete(rip->peer_list);
			rip->peer_list = NULL;
		}

        /* Redistribute related clear. */
//        if (rip->default_information_route_map)
//            free(rip->default_information_route_map);

        for (i = 0; i < ROUTE_PROTO_NUM; i++)
            if (rip->route_map[i].name)
                free(rip->route_map[i].name);

        rip_redistribute_clean(rip->redist_infos);
		if(rip->redist_infos)
		{
			list_free(rip->redist_infos);
			rip->redist_infos = NULL;
		}
        rip_interface_clean(rip->id);

    }
    rip_offset_clean();
//  rip_distance_reset ();

}

/* Reset all values to the default settings. */
void
rip_reset(void)
{
    /* Reset global counters. */
    rip_global_route_add = 0;
    rip_global_route_del = 0;
    rip_global_queries = 0;

    /* Call ripd related reset functions. */
    rip_debug_reset();
    rip_route_map_reset();

    /* Call library reset functions. */
    //vty_reset();
    access_list_reset();
    prefix_list_reset();

    distribute_list_reset();

    rip_interface_reset();
//  rip_distance_reset ();

}

void
global_rip_m_free(void)
{
    struct routefifo *drfifo = NULL;
	struct ifmeventfifo *dififo = NULL;
	struct pkteventfifo *dpfifo = NULL;

    if (rip_m.t_thread_routefifo)
    {
        //thread_cancel(rip_m.t_thread_routefifo);
        high_pre_timer_delete (rip_m.t_thread_routefifo);        									\
        rip_m.t_thread_routefifo = 0;
    }

    while (!FIFO_EMPTY(&rip_m.routefifo))
    {
        drfifo = (struct routefifo *)FIFO_TOP(&rip_m.routefifo);
        FIFO_DEL(drfifo);
        XFREE(MTYPE_ROUTE_FIFO, drfifo);
		drfifo = NULL;
    }
	
    while (!FIFO_EMPTY(&rip_m.ifmfifo))
    {
        dififo = (struct ifmeventfifo *)FIFO_TOP(&rip_m.ifmfifo);
        FIFO_DEL(dififo);
        XFREE(MTYPE_ROUTE_FIFO, dififo);
		dififo = NULL;
    }
	
    /*if (rip_m.pkt_recv)
    {
        thread_cancel(rip_m.pkt_recv);
		rip_m.pkt_recv = NULL;
    }*/
    /*if (rip_m.ifm_recv)
    {
        thread_cancel(rip_m.ifm_recv);
		rip_m.ifm_recv= NULL;
    }*/
    /*if (rip_m.route_recv)
    {
        thread_cancel(rip_m.route_recv);
		rip_m.route_recv = NULL;
    }*/

    while (!FIFO_EMPTY(&rip_m.pktfifo))
    {
        dpfifo = (struct pkteventfifo *)FIFO_TOP(&rip_m.pktfifo);
        FIFO_DEL(dpfifo);
        XFREE(MTYPE_ROUTE_FIFO, dpfifo);
		dpfifo = NULL;
    }
}

void
rip_if_rmap_update(struct if_rmap *if_rmap)
{
    struct interface *ifp;
    struct rip_interface *ri;
    struct route_map *rmap;

    ifp = if_lookup_by_name(if_rmap->ifname);

    if (ifp == NULL)
        return;

    ri = (struct rip_interface *)ifp->info;

    if (if_rmap->routemap[IF_RMAP_IN])
    {
        rmap = route_map_lookup_by_name(if_rmap->routemap[IF_RMAP_IN]);

        if (rmap)
            ri->routemap[IF_RMAP_IN] = rmap;
        else
            ri->routemap[IF_RMAP_IN] = NULL;
    }
    else
        ri->routemap[RIP_FILTER_IN] = NULL;

    if (if_rmap->routemap[IF_RMAP_OUT])
    {
        rmap = route_map_lookup_by_name(if_rmap->routemap[IF_RMAP_OUT]);

        if (rmap)
            ri->routemap[IF_RMAP_OUT] = rmap;
        else
            ri->routemap[IF_RMAP_OUT] = NULL;
    }
    else
        ri->routemap[RIP_FILTER_OUT] = NULL;
}

void
rip_if_rmap_update_interface(struct interface *ifp)
{
    struct if_rmap *if_rmap;

    if_rmap = if_rmap_lookup(ifp->name);

    if (if_rmap)
        rip_if_rmap_update(if_rmap);
}

void
rip_routemap_update_redistribute(void)
{
    int i;
    struct rip_instance *rip;
    struct listnode *node;

    for (ALL_LIST_ELEMENTS_RO(rip_list, node, rip))
        if (rip)
            for (i = 0; i < ROUTE_PROTO_NUM; i++)
                if (rip->route_map[i].name)
                    rip->route_map[i].map = route_map_lookup_by_name(rip->route_map[i].name);
}

/* ARGSUSED */
void
rip_routemap_update(const char *notused)
{
    struct interface *ifp;
    struct listnode *node, *nnode;

    for (ALL_LIST_ELEMENTS(iflist, node, nnode, ifp))
    {
        rip_if_rmap_update_interface(ifp);
    }

    rip_routemap_update_redistribute();
}

/* Allocate new rip structure and set default value. */
void
rip_init(void)
{
    /* Randomize for triggered update random(). */
    srandom(time(NULL));

    /* Install top nodes. */
    install_node(&rip_node, config_write_rip);

    /* Install rip commands. */
    install_element(CONFIG_NODE, &show_ip_rip_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ip_rip_status_cmd, CMD_LOCAL);


    install_element(CONFIG_NODE, &router_rip_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_router_rip_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &show_ip_rip_statistics_cmd, CMD_LOCAL);

    install_default(RIP_NODE);
    install_element(RIP_NODE, &rip_version_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_version_cmd, CMD_SYNC);
    install_element(RIP_NODE, &rip_timers_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_timers_cmd, CMD_SYNC);
    install_element(RIP_NODE, &rip_distance_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_distance_cmd, CMD_SYNC);
    install_element(RIP_NODE, &rip_summary_all_cmd, CMD_SYNC);
    install_element(RIP_NODE, &no_rip_summary_all_cmd, CMD_SYNC);

    /* Debug related init. */
    rip_debug_init();

    /* SNMP init. */
#ifdef HAVE_SNMP
    rip_snmp_init();
#endif /* HAVE_SNMP */

    /* Access list install. */
    access_list_init();
    access_list_add_hook(rip_distribute_update_all_wrapper);
    access_list_delete_hook(rip_distribute_update_all_wrapper);

    /* Prefix list initialize.*/
    prefix_list_init();
    prefix_list_add_hook(rip_distribute_update_all);
    prefix_list_delete_hook(rip_distribute_update_all);

    /* Distribute list install. */
    distribute_list_init(RIP_NODE);
    distribute_list_add_hook(rip_distribute_update);
    distribute_list_delete_hook(rip_distribute_update);

    /* Route-map */
    rip_route_map_init();
    rip_offset_init();

    route_map_add_hook(rip_routemap_update);
    route_map_delete_hook(rip_routemap_update);

    if_rmap_init(RIP_NODE);
    if_rmap_hook_add(rip_if_rmap_update);
    if_rmap_hook_delete(rip_if_rmap_update);


    rip_list = list_new();

    rip_peer_init();
    rip_zebra_init();
    ripng_init();
    ripng_zebra_init();

    

#if 0
	do{
    	rip_m.pkt_recv = thread_add_event_normal(master_rip, rip_pkt_rcv, NULL, 0);  /* thread to receive packet */
		if(!rip_m.pkt_recv)
			zlog_err("%-15s[%d]: can`t add reak pkt event", __func__, __LINE__);
	}
	while(!rip_m.pkt_recv);
#endif

	/*if(ipc_recv_thread_start((char *)"RipMsgRev", MODULE_ID_RIP, SCHED_OTHER, -1, rip_pkt_msg_rcv, 0) == -1)
    {
        printf("Rip msg receive thread start fail\r\n");
        exit(0);
    }

	ripc_init();
	rip_pkt_register();*/
}
