/* RIPng daemon
 * Copyright (C) 1998, 1999 Kunihiro Ishiguro
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

#include "prefix.h"
#include "filter.h"
#include "log.h"
#include "thread.h"
#include "memory.h"
#include "if.h"
#include "stream.h"
#include "table.h"
#include "command.h"
#include "sockopt.h"
#include "distribute.h"
#include "plist.h"
#include "routemap.h"
#include "if_rmap.h"
#include "privs.h"
#include "ftm/pkt_eth.h"
#include "ftm/pkt_ip.h"
#include "ftm/pkt_ipv6.h"
#include "ftm/pkt_udp.h"
#include "lib/pkt_buffer.h"
#include <lib/checksum.h>
#include "ripngd.h"
#include "ripng_route.h"
#include "ripng_nexthop.h"

#include "ripd.h"
#include "rip_debug.h"
/* RIPng structure which includes many parameters related to RIPng
   protocol. If ripng couldn't active or ripng doesn't configured,
   ripng->fd must be negative value. */
struct stats ripng_stats;

extern struct zebra_privs_t ripngd_privs;
struct list *ripng_list;


/* RIPng next hop specification. */
struct ripng_nexthop
{
    enum ripng_nexthop_type
    {
        RIPNG_NEXTHOP_UNSPEC,
        RIPNG_NEXTHOP_ADDRESS
    } flag;
    struct in6_addr address;
};

/* Prototypes. */
void ripng_nexthop_rte(struct rte_ipv6 *rte, struct sockaddr_in6 *from, struct ripng_nexthop *nexthop);
void ripng_response_process(struct ripng_packet *packet, int size,
                       struct sockaddr_in6 *from, struct interface *ifp, int hoplimit, struct ripng_instance *ripng);
void ripng_route_process(struct rte_ipv6 *rte, struct sockaddr_in6 *from, struct ripng_nexthop *ripng_nexthop,
                    struct interface *ifp, struct ripng_instance *ripng);

int ripng_triggered_update(void *);

/* Return remain time in second. */
static unsigned long
ripng_thread_timer_remain_second (TIMERID thread, u_int32_t timer_val)
{
	int pass_time = 0;

	high_pre_timer_passtime(thread, &pass_time);

	if(timer_val - pass_time > 0)
		return (timer_val - pass_time);
	else
		return 0;
}


static int
ripng_ecmp_route_cmp(struct ripng_info *ri1, struct ripng_info *ri2)
{

    if (ri1->sub_type > ri2->sub_type)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

static int
ripng_route_rte(struct ripng_info *rinfo)
{
    return ((ROUTE_PROTO_RIPNG == rinfo->type) 
			&& (rinfo->sub_type == RIPNG_ROUTE_RTE
            	|| rinfo->sub_type == RIPNG_ROUTE_INTERFACE));
}

/* Allocate new ripng information. */
struct ripng_info *
ripng_info_new()
{
    struct ripng_info *new;

    new = XCALLOC(MTYPE_RIPNG_ROUTE, sizeof(struct ripng_info));
    return new;
}

/* Free ripng information. */
void
ripng_info_free(struct ripng_info *rinfo)
{
    XFREE(MTYPE_RIPNG_ROUTE, rinfo);
}

struct connected *
connected_lookup_localaddress (struct interface *ifp)
{
  struct listnode *cnode;
  struct connected *c;
  struct connected *match;

  match = NULL;

  for (ALL_LIST_ELEMENTS_RO (ifp->connected, cnode, c))
    {
      if (c->address && (c->address->family == AF_INET6) && (IN6_IS_ADDR_LINKLOCAL (&c->address->u.prefix6)))
      {
			match = c;
      }
    }
  return match;
}

/* Send RIPng packet. */
int
ripng_send_packet(u_char *buf, int size, struct sockaddr_in6 *to, struct connected *ifc)
{
    int ret = 0;
    struct sockaddr_in6 sin;
    struct ipv6_hdr iph;
    struct udphdr udph;
    struct stream *pdata = NULL;
    union pkt_control pcb;
    struct prefix dip;
    struct rip_interface *ri = (struct rip_interface *)ifc->ifp->info;

	if(!ri)
	{
		zlog_err("%s %d ripng send pkt ri is NULL", __func__, __LINE__);
		return 0;
	}
//    assert(ri != NULL);

    //if (IS_RIPNG_DEBUG_PACKET)
    {
        if (to)
        {
            zlog_debug(RIPNG_DBG_PACKET, "send to %s", inet6_ntoa(to->sin6_addr));
        	zlog_debug(RIPNG_DBG_PACKET, "  send interface %s", ifc->ifp->name);
        	zlog_debug(RIPNG_DBG_PACKET, "  send packet size %d", size);
        }
    }

    memset(&sin, 0, sizeof(struct sockaddr_in6));
    sin.sin6_family = AF_INET6;
#ifdef SIN6_LEN
    sin.sin6_len = sizeof(struct sockaddr_in6);
#endif /* SIN6_LEN */
    sin.sin6_flowinfo = htonl(RIPNG_PRIORITY_DEFAULT);

    /* When destination is specified. */
    if (to != NULL)
    {
        sin.sin6_addr = to->sin6_addr;
        sin.sin6_port = to->sin6_port;
    }
    else
    {
        inet_pton(AF_INET6, RIPNG_GROUP, &sin.sin6_addr);
        sin.sin6_port = htons(RIPNG_PORT_DEFAULT);
    }

    dip.family = AF_INET6;
    dip.prefixlen = 128;
    IPV6_ADDR_COPY(&dip.u.prefix6, &sin.sin6_addr.s6_addr);

    memset(&iph, 0, sizeof(struct iphdr));
    memset(&udph, 0, sizeof(struct udphdr));
    memset(&pcb, 0, sizeof(pcb));

    iph.tos1 = 5;
    iph.version = 6;
    iph.data_len = htons(size + UDP_HEADER_SIZE);
    iph.ttl = 255;
    iph.protocol = IP_P_UDP;
    IPV6_ADDR_COPY(&iph.saddr, &ifc->address->u.prefix6);
    IPV6_ADDR_COPY(&iph.daddr, &sin.sin6_addr);

    /***make udp check struct and udp header ***/
    udph.checksum = 0x00;
    uint32_t sum = 0x00;
    unsigned char *data;

    udph.dport = htons(RIPNG_PORT_DEFAULT);
    udph.sport = htons(RIPNG_PORT_DEFAULT);
    udph.len = htons(UDP_HEADER_SIZE + size);

    /*** set packet length  ***/
    if (to)
        pdata = stream_new(sizeof(struct ipv6_hdr) + UDP_HEADER_SIZE + size);
    else
    {
        pdata = stream_new(ETH_HLEN + sizeof(struct ipv6_hdr) + UDP_HEADER_SIZE + size);

        /*** set ip header  ***/
        stream_write(pdata, &iph, sizeof(iph));
    }

    /*** set udp checksum ***/
//  data = (unsigned char *)XMALLOC(MTYPE_RIP_UDP, sizeof(struct ipv6pseudo_hdr)+ UDP_HEADER_SIZE + size);
    data = (unsigned char *)XMALLOC(MTYPE_RIP_UDP, UDP_HEADER_SIZE + size);

    if (data == NULL)
    {
        zlog_err("ERROR:rip send packet error, malloc returns null");
        return -1;
    }

    memcpy(data, (unsigned char *)&udph, UDP_HEADER_SIZE);
    memcpy(data + UDP_HEADER_SIZE, buf, size);

    sum = in6_checksum(ifc->address, &dip, IP_P_UDP, (uint16_t *)data, UDP_HEADER_SIZE + size);
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
        IPV6_ADDR_COPY(&pcb.ipcb.sipv6, &ifc->address->u.prefix6);
        IPV6_ADDR_COPY(&pcb.ipcb.dipv6, &sin.sin6_addr);
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
        pcb.ethcb.dmac[0] = 0x33;
        pcb.ethcb.dmac[1] = 0x33;
        pcb.ethcb.dmac[2] = 0x00;
        pcb.ethcb.dmac[3] = 0x00;
        pcb.ethcb.dmac[4] = 0x00;
        pcb.ethcb.dmac[5] = 0x09;

        pcb.ethcb.ifindex = ifc->ifp->ifindex;
        pcb.ethcb.ethtype = ETH_P_IPV6;
        pcb.ethcb.cos = 3;

//  if (IS_RIP_DEBUG_PACKET)
//  {
//      pkt_dump (STREAM_DATA(pdata), sizeof(iph) + sizeof(udph) + size);
//  }
//  rip_wrong_size_pkt_dump(STREAM_DATA(pdata), sizeof(iph) + sizeof(udph) + size);

        ret = pkt_send(PKT_TYPE_ETH, &pcb, STREAM_DATA(pdata), sizeof(iph) + sizeof(udph) + size);
    }

    stream_free(pdata);
    pdata = NULL;

    if (ret != 0)
    {
        if (to)
        {
            ripng_stats.tx_packets++;
			if (IS_RIPNG_DEBUG_EVENT)
			{
            	zlog_err("RIPng send fail on %s to %s: %s", ifc->ifp->name,
                	     inet6_ntoa(to->sin6_addr), safe_strerror(errno));
			}
        }
        else
        {
            ripng_stats.tx_failed++;

			if (IS_RIPNG_DEBUG_EVENT)
			{
            	zlog_err("RIPng send success on %s: %s", ifc->ifp->name, safe_strerror(errno));
			}
        }
    }

    return ret;
}

/* Dump rip packet */
void
ripng_packet_dump(struct ripng_packet *packet, int size, const char *sndrcv)
{
    caddr_t lim;
    struct rte_ipv6 *rte;
    const char *command_str;

    /* Set command string. */
    if (packet->command == RIPNG_REQUEST)
        command_str = "request";
    else if (packet->command == RIPNG_RESPONSE)
        command_str = "response";
    else
        command_str = "unknown";

    /* Dump packet header. */
    zlog_debug(RIPNG_DBG_PACKET, "%s %s version %d packet size %d",
               sndrcv, command_str, packet->version, size);

    /* Dump each routing table entry. */
    rte = packet->rte;

    for (lim = (caddr_t) packet + size; (caddr_t) rte < lim; rte++)
    {
        if (rte->metric == RIPNG_METRIC_NEXTHOP)
            zlog_debug(RIPNG_DBG_PACKET, "  nexthop %s/%d", inet6_ntoa(rte->addr), rte->prefixlen);
        else
            zlog_debug(RIPNG_DBG_PACKET, "  %s/%d metric %d tag %d",
                       inet6_ntoa(rte->addr), rte->prefixlen,
                       rte->metric, ntohs(rte->tag));
    }
}

/* RIPng next hop address RTE (Route Table Entry). */
void
ripng_nexthop_rte(struct rte_ipv6 *rte,
                  struct sockaddr_in6 *from,
                  struct ripng_nexthop *nexthop)
{
    char buf[INET6_BUFSIZ];

    /* Logging before checking RTE. */
	//if (IS_RIPNG_DEBUG_PACKET)
	{
    	zlog_debug(RIPNG_DBG_PACKET, "RIPng nexthop RTE address %s tag %d prefixlen %d",
        	           inet6_ntoa(rte->addr), ntohs(rte->tag), rte->prefixlen);
	}
    /* RFC2080 2.1.1 Next Hop:
     The route tag and prefix length in the next hop RTE must be
     set to zero on sending and ignored on receiption.  */
    if (ntohs(rte->tag) != 0)
	{
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng nexthop RTE with non zero tag value %d from %s",
            	      ntohs(rte->tag), inet6_ntoa(from->sin6_addr));
		}
	}
    if (rte->prefixlen != 0)
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
       	 	zlog_debug(RIPNG_DBG_PACKET, "RIPng nexthop RTE with non zero prefixlen value %d from %s",
            	      rte->prefixlen, inet6_ntoa(from->sin6_addr));
    	}
	}
    /* Specifying a value of 0:0:0:0:0:0:0:0 in the prefix field of a
     next hop RTE indicates that the next hop address should be the
     originator of the RIPng advertisement.  An address specified as a
     next hop must be a link-local address.  */
    if (IN6_IS_ADDR_UNSPECIFIED(&rte->addr))
    {
        nexthop->flag = RIPNG_NEXTHOP_UNSPEC;
        memset(&nexthop->address, 0, sizeof(struct in6_addr));
        return;
    }

    if (IN6_IS_ADDR_LINKLOCAL(&rte->addr))
    {
        nexthop->flag = RIPNG_NEXTHOP_ADDRESS;
        IPV6_ADDR_COPY(&nexthop->address, &rte->addr);
        return;
    }

    /* The purpose of the next hop RTE is to eliminate packets being
     routed through extra hops in the system.  It is particularly useful
     when RIPng is not being run on all of the routers on a network.
     Note that next hop RTE is "advisory".  That is, if the provided
     information is ignored, a possibly sub-optimal, but absolutely
     valid, route may be taken.  If the received next hop address is not
     a link-local address, it should be treated as 0:0:0:0:0:0:0:0.  */
	//if (IS_RIPNG_DEBUG_PACKET)
	{
		zlog_debug(RIPNG_DBG_PACKET, "RIPng nexthop RTE with non link-local address %s from %s",
	    	  inet6_ntoa(rte->addr), inet_ntop(AF_INET6, &from->sin6_addr, buf, INET6_BUFSIZ));
	}
    nexthop->flag = RIPNG_NEXTHOP_UNSPEC;
    memset(&nexthop->address, 0, sizeof(struct in6_addr));

    return;
}

/* If ifp has same link-local address then return 1. */
int
ripng_lladdr_check(struct interface *ifp, struct in6_addr *addr)
{
    struct listnode *node;
    struct connected *connected;
    struct prefix *p;

    for (ALL_LIST_ELEMENTS_RO(ifp->connected, node, connected))
    {
        p = connected->address;

        if (p && p->family == AF_INET6 &&
                IN6_IS_ADDR_LINKLOCAL(&p->u.prefix6) &&
                IN6_ARE_ADDR_EQUAL(&p->u.prefix6, addr))
            return 1;
    }

    return 0;
}

/* RIPng route garbage collect timer. */
int
ripng_garbage_collect(void *t)
{
    struct ripng_info *rinfo;
    struct route_node *rp;
	struct list *list;

    rinfo = (struct ripng_info *)(t);

    if (!rinfo)
    {
        return -1;
    }

    rinfo->t_garbage_collect = 0;

    /* Off timeout timer. */
    RIPNG_TIMER_OFF(rinfo->t_timeout);

    /* Get route_node pointer. */
    rp = rinfo->rp;

    /* Unlock route_node. */
	list = rp->info;
	if(list)
	{
    	listnode_delete(rp->info, rinfo);
	}

    if (list_isempty((struct list *)rp->info))
    {
        list_free(rp->info);
        rp->info = NULL;
        route_unlock_node(rp);
    }

    /* Free RIPng routing information. */
    ripng_info_free(rinfo);
	rinfo = NULL;
    return 0;
}

/* Add new route to the ECMP list.
 * RETURN: the new entry added in the list, or NULL if it is not the first
 *         entry and ECMP is not allowed.
 */
struct ripng_info *
ripng_ecmp_add(struct ripng_info *rinfo_new, struct ripng_instance *ripng)
{
    struct route_node *rp = rinfo_new->rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
	struct listnode *lnode = NULL;

    if (rp->info == NULL)
    {
        rp->info = list_new();		
        list = (struct list *)rp->info;
        list->cmp = (int (*)(void *, void *))ripng_ecmp_route_cmp;
    }

    list = (struct list *)rp->info;

    /* If ECMP is not allowed and some entry already exists in the list,
     * do nothing. */
//  if (listcount (list))
//    return NULL;

    rinfo = ripng_info_new();
    memcpy(rinfo, rinfo_new, sizeof(struct ripng_info));
    listnode_add_sort(list, rinfo);

    if (ripng_route_rte(rinfo))
    {
        if (rinfo->sub_type == RIPNG_ROUTE_RTE)
        {

            ripng_timeout_update(rinfo, ripng);
        }

        ripng_zebra_ipv6_add(rp);
    }

    ripng_aggregate_increment(rp, rinfo);

    /* Set the route change flag on the first entry. */
	lnode = listhead(list);
	if(!lnode)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
		}
		return NULL;
	}
	rinfo = (struct ripng_info *)listgetdata(lnode);
	if(!rinfo)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
		}
		return NULL;
	}
    SET_FLAG(rinfo->flags, RIPNG_RTF_CHANGED);

    /* Signal the output process to trigger an update. */
	ripng->triggered_flag = 1;
//    ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);

    return rinfo;
}

/* Replace the ECMP list with the new route.
 * RETURN: the new entry added in the list
 */
struct ripng_info *
ripng_ecmp_replace(struct ripng_info *rinfo_new, struct ripng_instance *ripng)
{
    struct route_node *rp = rinfo_new->rp;
    struct list *list = (struct list *)rp->info;
    struct ripng_info *rinfo = NULL, *tmp_rinfo = NULL;
    struct listnode *node = NULL, *nextnode = NULL;

    if (list == NULL || listcount(list) == 0)
        return ripng_ecmp_add(rinfo_new, ripng);

    /* Get the first entry */
	node = listhead(list);
	if(!node)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
		}
		return NULL;
	}
	rinfo = (struct ripng_info *)listgetdata(node);
	if(!rinfo)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
		}
		return NULL;
	}

    /* Learnt route replaced by a local one. Delete it from zebra. */
    if (ripng_route_rte(rinfo) && (CHECK_FLAG(rinfo->flags, RIPNG_RTF_FIB)))
    {
        ripng_zebra_ipv6_delete(rp);
    }

    if (rinfo->metric != RIPNG_METRIC_INFINITY)
        ripng_aggregate_decrement_list(rp, list);

    /* Re-use the first entry, and delete the others. */
    for (ALL_LIST_ELEMENTS(list, node, nextnode, tmp_rinfo))
    {
        if (!tmp_rinfo)
            continue;

        if (tmp_rinfo != rinfo)
        {
            RIPNG_TIMER_OFF(tmp_rinfo->t_timeout);
            RIPNG_TIMER_OFF(tmp_rinfo->t_garbage_collect);
            list_delete_node(list, node);
            ripng_info_free(tmp_rinfo);
			tmp_rinfo = NULL;
        }
    }

    RIPNG_TIMER_OFF(rinfo->t_timeout);
    RIPNG_TIMER_OFF(rinfo->t_garbage_collect);
    memcpy(rinfo, rinfo_new, sizeof(struct ripng_info));

    if (ripng_route_rte(rinfo))
    {
        if (rinfo->sub_type == RIPNG_ROUTE_RTE)
        {
            ripng_timeout_update(rinfo, ripng);
        }

        /* The ADD message implies an update. */
        ripng_zebra_ipv6_add(rp);
    }

    ripng_aggregate_increment(rp, rinfo);

    /* Set the route change flag. */
    SET_FLAG(rinfo->flags, RIPNG_RTF_CHANGED);

    /* Signal the output process to trigger an update. */
	ripng->triggered_flag = 1;
//    ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);

    return rinfo;
}

/* Delete one route from the ECMP list.
 * RETURN:
 *  null - the entry is freed, and other entries exist in the list
 *  the entry - the entry is the last one in the list; its metric is set
 *              to INFINITY, and the garbage collector is started for it
 */
struct ripng_info *
ripng_ecmp_delete(struct ripng_info *rinfo, struct ripng_instance *ripng)
{
    struct route_node *rp = rinfo->rp;
    struct list *list = (struct list *)rp->info;
	struct listnode *lnode;

	if(!list)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d list is NULL", __func__, __LINE__);		
		}
		return NULL;
	}

    RIPNG_TIMER_OFF(rinfo->t_timeout);

    if (rinfo->metric != RIPNG_METRIC_INFINITY)
        ripng_aggregate_decrement(rp, rinfo);

    if (listcount(list) > 1)
    {
        /* Some other ECMP entries still exist. Just delete this entry. */
        if (ripng_route_rte(rinfo) && CHECK_FLAG(rinfo->flags, RIPNG_RTF_FIB))
        {
			ripng_zebra_ipv6_delete(rp);
        }
        RIPNG_TIMER_OFF(rinfo->t_timeout);
        RIPNG_TIMER_OFF(rinfo->t_garbage_collect);
        listnode_delete(list, rinfo);
        ripng_info_free(rinfo);
        rinfo = NULL;

		/* The ADD message implies the update. */
		lnode = listhead(list);
		if(!lnode)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
			}
			return NULL;
		}
		rinfo = (struct ripng_info *)listgetdata(lnode);
		if(!rinfo)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
			}
			return NULL;
		}
		
        if (ripng_route_rte(rinfo))
        {
            ripng_zebra_ipv6_add(rp);
        }
    }
    else
    {
		lnode = listhead(list);
		if(!lnode)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
			}
			return NULL;
		}
		rinfo = (struct ripng_info *)listgetdata(lnode);
		if(!rinfo)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
			}
			return NULL;
		}

        /* This is the only entry left in the list. We must keep it in
         * the list for garbage collection time, with INFINITY metric. */

        rinfo->metric = RIPNG_METRIC_INFINITY;
        RIPNG_TIMER_ON(rinfo->t_garbage_collect,
                       ripng_garbage_collect, ripng->garbage_time);

        if (ripng_route_rte(rinfo) && CHECK_FLAG(rinfo->flags, RIPNG_RTF_FIB))
            ripng_zebra_ipv6_delete(rp);
    }

    /* Set the route change flag on the first entry. */
	lnode = listhead(list);
	if(!lnode)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
		}
		return NULL;
	}
	rinfo = (struct ripng_info *)listgetdata(lnode);
	if(!rinfo)
	{
		//if(IS_RIPNG_DEBUG_EVENT)		
		{			
			zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
		}
		return NULL;
	}
    SET_FLAG(rinfo->flags, RIPNG_RTF_CHANGED);

    /* Signal the output process to trigger an update. */
	ripng->triggered_flag = 1;
//    ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);

    return rinfo;
}

/* Timeout RIPng routes. */
int
ripng_timeout(void *t)
{
    struct ripng_info *rinfo;
    struct ripng_instance *ripng = NULL;

    rinfo = (struct ripng_info *)(t);

    if (!rinfo)
    {
        return -1;
    }

    rinfo->t_timeout = 0;

    ripng = ripng_instance_lookup(rinfo->id);

    if (ripng)
    {
        ripng_ecmp_delete(rinfo, ripng);
    }
    else
    {
		if (IS_RIPNG_DEBUG_EVENT)
		{
        	zlog_err("can`t find ripng instance by ripng rinfo id %d", rinfo->id);
		}
    }

    return 0;
}

void
ripng_timeout_update(struct ripng_info *rinfo, struct ripng_instance *ripng)
{
    if (rinfo->metric != RIPNG_METRIC_INFINITY)
    {
        RIPNG_TIMER_OFF(rinfo->t_timeout);
        RIPNG_TIMER_ON(rinfo->t_timeout, ripng_timeout, ripng->timeout_time);
    }
}

/* Process RIPng route according to RFC2080. */
void
ripng_route_process(struct rte_ipv6 *rte, struct sockaddr_in6 *from,
                    struct ripng_nexthop *ripng_nexthop,
                    struct interface *ifp, struct ripng_instance *ripng)
{
    struct prefix_ipv6 p;
    struct route_node *rp;
    struct ripng_info *rinfo = NULL, newinfo;
    struct in6_addr *nexthop;
    int same = 0;
    struct list *list = NULL;
    struct listnode *node = NULL;
	struct rip_interface *ri = (struct rip_interface *)ifp->info;

    /* Make prefix structure. */
    memset(&p, 0, sizeof(struct prefix_ipv6));
    p.family = AF_INET6;
    /* p.prefix = rte->addr; */
    IPV6_ADDR_COPY(&p.prefix, &rte->addr);
    p.prefixlen = rte->prefixlen;

    /* Make sure mask is applied. */
    /* XXX We have to check the prefix is valid or not before call
       apply_mask_ipv6. */
    apply_mask_ipv6(&p);

    memset(&newinfo, 0, sizeof(newinfo));
    newinfo.type = ROUTE_PROTO_RIPNG;
    newinfo.sub_type = RIPNG_ROUTE_RTE;

    if (ripng_nexthop->flag == RIPNG_NEXTHOP_ADDRESS)
        newinfo.nexthop = ripng_nexthop->address;
    else
        newinfo.nexthop = from->sin6_addr;

    newinfo.from = from->sin6_addr;
    newinfo.ifindex = ifp->ifindex;
    newinfo.tag = ntohs(rte->tag);    /* XXX */

    /* Once the entry has been validated, update the metric by
     * adding the cost of the network on wich the message
     * arrived. If the result is greater than infinity, use infinity
     * (RFC2453 Sec. 3.9.2)
     **/
    rte->metric += ri->ipv6_metric;

    if (rte->metric > RIPNG_METRIC_INFINITY)
        rte->metric = RIPNG_METRIC_INFINITY;

    /* Set nexthop pointer. */
    if (ripng_nexthop->flag == RIPNG_NEXTHOP_ADDRESS)
        nexthop = &ripng_nexthop->address;
    else
        nexthop = &from->sin6_addr;

    /* Lookup RIPng routing table. */
    rp = route_node_get(ripng->table, (struct prefix *) & p);

    newinfo.rp = rp;
    newinfo.nexthop = *nexthop;
    newinfo.metric = rte->metric;
	newinfo.metric_out = newinfo.metric;
    newinfo.tag = ntohs(rte->tag);
    newinfo.id = ripng->id;
	newinfo.distance = ripng->distance;

	/* Check to see whether there is already RIPng route on the table. */
	if ((list = (struct list *)rp->info) != NULL)
	{
	   for (ALL_LIST_ELEMENTS_RO(list, node, rinfo))
	   {
		   /* Need to compare with redistributed entry or local entry */
		   if (!ripng_route_rte(rinfo))
			   break;

		   if(IN6_ARE_ADDR_EQUAL(&rinfo->from, &from->sin6_addr)
                && (rinfo->ifindex == ifp->ifindex))
			   break;

		   if((rte->metric != RIPNG_METRIC_INFINITY) && (rte->metric > rinfo->metric))
		   {
			   route_unlock_node(rp);
			   return;
		   }
		   
		   if(!ripng_route_rte(rinfo))
		   {
			   
			   /* New route has a smaller metric. Replace the ECMP list
				* with the new one in below. */
			   if (rte->metric <= rinfo->metric)
				   break;

			   if (ripng->distance > rinfo->distance)
			   {
				   /* New route has a greater distance. Discard it. */
				   route_unlock_node(rp);
				   return;
			   }
			   else
			   {
				   break;
			   }

			   /* Metrics are same. Keep "rinfo" null and the new route
				* is added in the ECMP list in below. */
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
		if (rinfo->type != ROUTE_PROTO_RIPNG
				&& rinfo->metric != RIPNG_METRIC_INFINITY && rte->metric != RIPNG_METRIC_INFINITY)
		{

			if (rinfo->metric >= newinfo.metric)
			{
				if (rinfo->distance <= newinfo.distance)
				{
					ripng_ecmp_add(&newinfo, ripng);

					/* Update RIP peer. */
						ripng_peer_update(from, ripng, ifp->ifindex);
				}
			}
			else
			{
				if (rinfo->distance > newinfo.distance)
				{
					ripng_ecmp_replace(&newinfo, ripng);

					/* Update RIP peer. */
						ripng_peer_update(from, ripng, ifp->ifindex);
				}
			}

			route_unlock_node(rp);
			return;
		}

	}
		
    if (!rinfo)
    {
        /* Now, check to see whether there is already an explicit route
        for the destination prefix.  If there is no such route, add
        this route to the routing table, unless the metric is
        infinity (there is no point in adding a route which
        unusable). */
        if (rte->metric != RIPNG_METRIC_INFINITY)
        {
            ripng_ecmp_add(&newinfo, ripng);
			/* Update RIP peer. */
			ripng_peer_update(from, ripng, ifp->ifindex);
        }
    }
    else
    {
        /* If there is an existing route, compare the next hop address
        to the address of the router from which the datagram came.
        If this datagram is from the same router as the existing
        route, reinitialize the timeout.  */
        same = (IN6_ARE_ADDR_EQUAL(&rinfo->from, &from->sin6_addr)
                && (rinfo->ifindex == ifp->ifindex));

		if (!same && (RIPNG_METRIC_INFINITY != rte->metric) && (rinfo->metric == rte->metric))
		{
			ripng_ecmp_add(&newinfo, ripng);
		
			/* Update RIP peer. */
				ripng_peer_update(from, ripng, ifp->ifindex);
		}
        /* Next, compare the metrics.  If the datagram is from the same
        router as the existing route, and the new metric is different
        than the old one; or, if the new metric is lower than the old
        one; do the following actions: */
        if ((same && rinfo->metric != rte->metric) ||
                rte->metric < rinfo->metric)
        {
            if (listcount(list) == 1)
            {
                if (newinfo.metric != RIPNG_METRIC_INFINITY)
                    ripng_ecmp_replace(&newinfo, ripng);
                else
                {
                    ripng_ecmp_delete(rinfo, ripng);
                }
            }
            else
            {
                if (newinfo.metric < rinfo->metric)
                    ripng_ecmp_replace(&newinfo, ripng);
                else /* newinfo.metric > rinfo->metric */
                {

                    ripng_ecmp_delete(rinfo, ripng);
                }
            }
        }
        else /* same & no change */
        {
			if (same && (rte->metric != RIPNG_METRIC_INFINITY) && (CHECK_FLAG(rinfo->sub_type, RIPNG_ROUTE_RTE)))
			{
				ripng_timeout_update(rinfo, ripng);
				ripng_peer_update(from, ripng, ifp->ifindex);
			}
	//			  ripng_timeout_update(rinfo, ripng);
        }

        /* Unlock tempolary lock of the route. */
        route_unlock_node(rp);
    }
	
    /* RIPng peer update. */
//    ripng_peer_update(from, ripng, ifp->ifindex);
}

/* Add redistributed route to RIPng table. */
void
ripng_redistribute_add(struct ripng_info *newinfo, struct prefix_ipv6 *p, struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
	struct listnode *node, *nnode;

    /* Redistribute route  */

    if (IN6_IS_ADDR_LINKLOCAL (&p->prefix))
        return;
    if (IN6_IS_ADDR_LOOPBACK(&p->prefix))
        return;

    rp = route_node_get(ripng->table, (struct prefix *) p);
    newinfo->rp = rp;

    if ((list = (struct list *)rp->info) != NULL && listcount(list) != 0)
    {
		node = listhead(list);
		if(!node)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
			}
			route_unlock_node(rp);
			return;
		}
		rinfo = (struct ripng_info *)listgetdata(node);

		if (NULL == rinfo)
		{
			ripng_ecmp_add(newinfo, ripng);
		}
		else
		{
			if (rinfo->metric == RIPNG_METRIC_INFINITY)
			{
				ripng_ecmp_replace(newinfo, ripng);
				route_unlock_node(rp);
				return;
			}

			if ((ROUTE_PROTO_RIPNG == rinfo->type)
					&& CHECK_FLAG(rinfo->sub_type, RIPNG_ROUTE_INTERFACE))
			{
				if((ROUTE_PROTO_RIPNG == newinfo->type)
					&& CHECK_FLAG(newinfo->sub_type, RIPNG_ROUTE_INTERFACE))
				{
					rinfo->metric = 0;
					rinfo->metric_out = 0;
					RIPNG_TIMER_OFF(rinfo->t_timeout);
					RIPNG_TIMER_OFF(rinfo->t_garbage_collect);	
					
					route_unlock_node(rp);
					return;
				}

				if (newinfo->distance <= rinfo->distance 
					|| rinfo->metric == RIP_METRIC_INFINITY)
				{
					ripng_ecmp_add(newinfo, ripng);
				}
				
				route_unlock_node(rp);
				return;
			}

			if ((ROUTE_PROTO_RIPNG == newinfo->type) && newinfo->id == ripng->id
					&& CHECK_FLAG(newinfo->sub_type, RIPNG_ROUTE_INTERFACE))
			{
				if ((ROUTE_PROTO_CONNECT == rinfo->type) 
						&& (RIPNG_ROUTE_REDISTRIBUTE == rinfo->sub_type))
				{
					if (newinfo->distance >= rinfo->distance)
					{
						ripng_ecmp_add(newinfo, ripng);
					}
					route_unlock_node(rp);
					return;
				}
				else
				{
					ripng_ecmp_replace(newinfo, ripng);
					route_unlock_node(rp);
					return;
				}
			}

			if (newinfo->distance < rinfo->distance)
			{
				ripng_ecmp_replace(newinfo, ripng);
			}
			else if (newinfo->distance == rinfo->distance)
			{

				for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
				{
					if (newinfo->type == rinfo->type && newinfo->external_id == rinfo->external_id
							&& IPV6_ADDR_SAME(&newinfo->nexthop, &rinfo->nexthop))
					{
						rinfo->metric = newinfo->metric;
						rinfo->metric_out = newinfo->metric;
						rinfo->external_metric = newinfo->external_metric;
						RIPNG_TIMER_OFF(rinfo->t_garbage_collect);
						route_unlock_node(rp);
						return;
					}
				}

				ripng_ecmp_add(newinfo, ripng);
			}
		}

    }
    else
    {   
    	if(rp->info)
    	{
			route_unlock_node(rp);
		}
    
        rinfo = ripng_ecmp_add(newinfo, ripng);
    }

    //if (IS_RIPNG_DEBUG_EVENT)
    {
        if (newinfo->nexthop.s6_addr)
        {
            zlog_debug(RIPNG_DBG_EVENT, "Redistribute new prefix %s/%d with nexthop %s on the interface %s",
                       inet6_ntoa(p->prefix), p->prefixlen, inet6_ntoa(newinfo->nexthop),
                       ifindex2ifname(newinfo->ifindex));
        }
        else
        {
            zlog_debug(RIPNG_DBG_EVENT, "Redistribute new prefix %s/%d on the interface %s",
                       inet6_ntoa(p->prefix), p->prefixlen,
                       ifindex2ifname(newinfo->ifindex));
        }
    }

}


/* Delete redistributed route from RIPng table. */
void
ripng_redistribute_delete(struct ripng_info *newinfo, struct prefix_ipv6 *p, struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct list *list = NULL;
    struct listnode *node, *nnode;

	if (IN6_IS_ADDR_LINKLOCAL (&p->prefix))
	{
		return;
	}
	if (IN6_IS_ADDR_LOOPBACK (&p->prefix))
	{
		return;
	}

    rp = route_node_lookup(ripng->table, (struct prefix *) p);

    if (rp)
    {

        list = (struct list *)rp->info;
        if (list != NULL && listcount(list) != 0)
        {

            for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
            {
                if (!rinfo)
                    continue;

                if (rinfo->type == newinfo->type
                        && rinfo->external_id == newinfo->external_id
                        && rinfo->ifindex == newinfo->ifindex)
                {
                    /* Perform poisoned reverse. */
                    ripng_ecmp_delete(rinfo, ripng);
					
					//if (IS_RIPNG_DEBUG_EVENT)
					{
						zlog_debug(RIPNG_DBG_EVENT, "Poisone %s/%d on the interface %s with an "
								   "infinity metric [delete]",
								   inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen,
								   ifindex2ifname(newinfo->ifindex));
					}
                }
            }
        }
		
		route_unlock_node(rp);
    }

}

/* Withdraw redistributed route. */
void
ripng_redistribute_withdraw(enum ROUTE_PROTO type, int id, struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct list *list;
    struct listnode *node, *nnode;

    if (!ripng)
        return;

    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS(list, node, nnode, rinfo))
            {
                if (rinfo->type == type  && CHECK_FLAG(rinfo->sub_type, RIPNG_ROUTE_REDISTRIBUTE))
                {
                    if (id && rinfo->external_id != id)
                    {
                        continue;
                    }

                    ripng_ecmp_delete(rinfo, ripng);

                    /* Perform poisoned reverse. */
                    //if (IS_RIPNG_DEBUG_EVENT)
                    {
						zlog_debug(RIPNG_DBG_EVENT, "Poisone %s/%d on the interface %s [withdraw]",
									   inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen,
									   ifindex2ifname(rinfo->ifindex));
                    }

                }
            }
        }
    }

	if(ripng->triggered_flag)
	{
		ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);
		ripng->triggered_flag = 0;
	}
}

/* RIP routing information. */
void
ripng_response_process(struct ripng_packet *packet, int size,
                       struct sockaddr_in6 *from, struct interface *ifp,
                       int hoplimit, struct ripng_instance *ripng)
{
    caddr_t lim;
    struct rte_ipv6 *rte;
    struct ripng_nexthop nexthop;

    /* RFC2080 2.4.2  Response Messages:
     The Response must be ignored if it is not from the RIPng port.  */
    if (ntohs(from->sin6_port) != RIPNG_PORT_DEFAULT)
    {
    
	    //if (IS_RIPNG_DEBUG_PACKET)
	    {
	        zlog_debug(RIPNG_DBG_PACKET, "RIPng packet comes from non RIPng port %d from %s",
	                  ntohs(from->sin6_port), inet6_ntoa(from->sin6_addr));
	    }
        ripng_peer_bad_packet(from, ripng->peer_list);
        ripng_stats.rx_bad_routes++;
        return;
    }

    /* The datagram's IPv6 source address should be checked to see
     whether the datagram is from a valid neighbor; the source of the
     datagram must be a link-local address.  */
    if (! IN6_IS_ADDR_LINKLOCAL(&from->sin6_addr))
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
	        zlog_debug(RIPNG_DBG_PACKET, "RIPng packet comes from non link local address %s",
	                  inet6_ntoa(from->sin6_addr));
		}
        ripng_peer_bad_packet(from, ripng->peer_list);
        ripng_stats.rx_bad_routes++;
        return;
    }

    /* It is also worth checking to see whether the response is from one
     of the router's own addresses.  Interfaces on broadcast networks
     may receive copies of their own multicasts immediately.  If a
     router processes its own output as new input, confusion is likely,
     and such datagrams must be ignored. */
    if (ripng_lladdr_check(ifp, &from->sin6_addr))
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng packet comes from my own link local address %s",
            	      inet6_ntoa(from->sin6_addr));
		}
        ripng_peer_bad_packet(from, ripng->peer_list);
        ripng_stats.rx_bad_routes++;
        return;
    }

    /* As an additional check, periodic advertisements must have their
     hop counts set to 255, and inbound, multicast packets sent from the
     RIPng port (i.e. periodic advertisement or triggered update
     packets) must be examined to ensure that the hop count is 255. */
    if (hoplimit >= 0 && hoplimit != 255)
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng packet comes with non 255 hop count %d from %s",
            	      hoplimit, inet6_ntoa(from->sin6_addr));
		}
        ripng_peer_bad_packet(from, ripng->peer_list);
        ripng_stats.rx_bad_routes++;
        return;
    }

    /* Reset nexthop. */
    memset(&nexthop, 0, sizeof(struct ripng_nexthop));
    nexthop.flag = RIPNG_NEXTHOP_UNSPEC;

    /* Set RTE pointer. */
    rte = packet->rte;

    for (lim = ((caddr_t) packet) + size; (caddr_t) rte < lim; rte++)
    {
        /* First of all, we have to check this RTE is next hop RTE or
           not.  Next hop RTE is completely different with normal RTE so
           we need special treatment. */
        if (rte->metric == RIPNG_METRIC_NEXTHOP)
        {        
	            ripng_nexthop_rte(rte, from, &nexthop);
            continue;
        }

        ripng_stats.rx_routes++;

        /* RTE information validation. */

        /* - is the destination prefix valid (e.g., not a multicast
           prefix and not a link-local address) A link-local address
           should never be present in an RTE. */
        if (IN6_IS_ADDR_MULTICAST(&rte->addr))
        {
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Destination prefix is a multicast address %s/%d [%d]",
                	      inet6_ntoa(rte->addr), rte->prefixlen, rte->metric);
			}
            ripng_peer_bad_route(from, ripng->peer_list);
            ripng_stats.rx_bad_routes++;
            continue;
        }

        if (IN6_IS_ADDR_LINKLOCAL(&rte->addr))
        {
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Destination prefix is a link-local address %s/%d [%d]",
                	      inet6_ntoa(rte->addr), rte->prefixlen, rte->metric);
			}
            ripng_peer_bad_route(from, ripng->peer_list);
            ripng_stats.rx_bad_routes++;
            continue;
        }

        if (IN6_IS_ADDR_LOOPBACK(&rte->addr))
        {
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Destination prefix is a loopback address %s/%d [%d]",
                	      inet6_ntoa(rte->addr), rte->prefixlen, rte->metric);
			}
            ripng_peer_bad_route(from, ripng->peer_list);
            ripng_stats.rx_bad_routes++;
            continue;
        }

        /* - is the prefix length valid (i.e., between 0 and 128,
           inclusive) */
        if (rte->prefixlen > 128)
        {
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Invalid prefix length %s/%d from %s%%%s",
                	      inet6_ntoa(rte->addr), rte->prefixlen,
                    	  inet6_ntoa(from->sin6_addr), ifp->name);
			}
            ripng_peer_bad_route(from, ripng->peer_list);
            ripng_stats.rx_bad_routes++;
            continue;
        }

        /* - is the metric valid (i.e., between 0 and 16, inclusive) */
        if (!(rte->metric <= 16))
        {
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Invalid metric %d from %s%%%s", rte->metric,
                	      inet6_ntoa(from->sin6_addr), ifp->name);
			}
            ripng_peer_bad_route(from, ripng->peer_list);
            ripng_stats.rx_bad_routes++;
            continue;
        }

        /* Vincent: XXX Should we compute the direclty reachable nexthop
         * for our RIPng network ?
         **/

        /* Routing table updates. */
        ripng_route_process(rte, from, &nexthop, ifp, ripng);
    }

	if(ripng->triggered_flag)
	{
		ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);
		ripng->triggered_flag = 0;
	}
}

/* Response to request message. */
void
ripng_request_process(struct ripng_packet *packet, int size,
                      struct sockaddr_in6 *from, struct connected *ifc, struct ripng_instance *ripng)
{
    caddr_t lim;
	struct list *list;
	struct listnode *lnode;
    struct rte_ipv6 *rte;
    struct prefix_ipv6 p;
    struct route_node *rp;
    struct ripng_info *rinfo;

    lim = ((caddr_t) packet) + size;
    rte = packet->rte;

    /* The Request is processed entry by entry.  If there are no
       entries, no response is given. */
    if (lim == (caddr_t) rte)
        return;

    /* There is one special case.  If there is exactly one entry in the
       request, and it has a destination prefix of zero, a prefix length
       of zero, and a metric of infinity (i.e., 16), then this is a
       request to send the entire routing table.  In that case, a call
       is made to the output process to send the routing table to the
       requesting address/port. */
    if (lim == ((caddr_t)(rte + 1)) &&
            IN6_IS_ADDR_UNSPECIFIED(&rte->addr) &&
            rte->prefixlen == 0 &&
            rte->metric == RIPNG_METRIC_INFINITY)
    {
        /* All route with split horizon */
        ripng_output_process(ifc, NULL, ripng_all_route, ripng, 0);
    }
    else
    {
        /* Except for this special case, processing is quite simple.
        Examine the list of RTEs in the Request one by one.  For each
        entry, look up the destination in the router's routing
        database and, if there is a route, put that route's metric in
        the metric field of the RTE.  If there is no explicit route
        to the specified destination, put infinity in the metric
        field.  Once all the entries have been filled in, change the
        command from Request to Response and send the datagram back
        to the requestor. */
        memset(&p, 0, sizeof(struct prefix_ipv6));
        p.family = AF_INET6;

        for (; ((caddr_t) rte) < lim; rte++)
        {
            p.prefix = rte->addr;
            p.prefixlen = rte->prefixlen;
            apply_mask_ipv6(&p);

            rp = route_node_lookup(ripng->table, (struct prefix *) & p);

            if (rp)
            {
				list = (struct list *)rp->info;
				if(!list)
				{
					//if(IS_RIPNG_DEBUG_EVENT)		
					{			
						zlog_debug(RIPNG_DBG_EVENT, "%s %d list is NULL", __func__, __LINE__);		
					}
					route_unlock_node(rp);
					return ;
				}
				lnode = listhead(list);
				if(!lnode)
				{
					//if(IS_RIPNG_DEBUG_EVENT)		
					{			
						zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
					}
					route_unlock_node(rp);
					return ;
				}
				rinfo = (struct ripng_info *)listgetdata(lnode);
				if(!rinfo)
				{
					//if(IS_RIPNG_DEBUG_EVENT)		
					{			
						zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
					}
					route_unlock_node(rp);
					return ;
				}
                rte->metric = rinfo->metric;
                route_unlock_node(rp);
            }
            else
                rte->metric = RIPNG_METRIC_INFINITY;
        }

        packet->command = RIPNG_RESPONSE;

        ripng_send_packet((u_char *) packet, size, NULL, ifc);
    }
}

/* First entry point of reading RIPng packet. */
int
ripng_read(struct sockaddr_in6 from, uint32_t in_ifindex, void *ibuf, uint16_t len)
{
    struct ripng_packet *packet;
    struct interface *ifp;
	struct connected *ifc;
//  int hoplimit = -1;
    struct rip_interface *ri = NULL;
    struct ripng_instance *ripng = NULL;

    ripng_stats.rx_packets++;

    if (len <= 0)
    {
        ripng_stats.rx_errors++;
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng recvfrom failed.");
		}
        return len;
    }

    packet = (struct ripng_packet *) ibuf;

    /* Which interface is this packet comes from. */
    ifp = if_lookup_by_index(in_ifindex);

    /* RIP packet received */
    //if (IS_RIPNG_DEBUG_PACKET)
    {
        zlog_debug(RIPNG_DBG_PACKET, "RIPng packet from %s port %d on %s, packet in_index is %02x",
                   inet6_ntoa(from.sin6_addr), ntohs(from.sin6_port),
                   ifp ? ifp->name : "unknown", in_ifindex);
    }

    /* If this packet come from unknown interface, ignore it. */
    if (ifp == NULL || (ifp->ifindex != in_ifindex))
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "ripng_read: cannot find interface for packet from %s port %d",
            	      inet6_ntoa(from.sin6_addr), ntohs(from.sin6_port));
		}
        ripng_stats.rx_errors++;
        return -1;
    }

    /* Check RIPng process is enabled on this interface. */
    ri = (struct rip_interface *)ifp->info;

    if (!ri->ripng_enable)
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "ripng_read: ripng didn`t enabled under interface %s ", ifp->name);
		}
        ripng_stats.rx_errors++;
        return -1;
    }

    ripng = ripng_instance_lookup(ri->ripng_enable);

    if (!ripng)
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "ripng_read: can`t find ripng instance while interface enabled %d", ri->ripng_enable);
		}
        return -1;
    }
	ifc = connected_lookup_localaddress(ifp);
	if(!ifc)
	{
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "ripng_read: interface %s without linklocal address", ifp->name);
		}
        return -1;

	}

    /* Check RTE boundary.  RTE size (Packet length - RIPng header size
       (4)) must be multiple size of one RTE size (20). */
    if (((len - 4) % 20) != 0)
    {
        ripng_stats.rx_errors++;
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng invalid packet size %d from %s", len,
                  inet6_ntoa(from.sin6_addr));
		}
        ripng_peer_bad_packet(&from, ripng->peer_list);
        return 0;
    }

    /* Does not reponse to the requests on the loopback interfaces */
    if (if_is_loopback(ifp))
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "ripng_read: interface can`t be loopback type");
		}
        ripng_stats.rx_errors++;
        return -1;
    }

    /* Logging before packet checking. */
    if (IS_RIPNG_DEBUG_PACKET)
    {
        ripng_packet_dump(packet, len, "RECV");
    }
    /* Packet version mismatch checking. */
    if (packet->version != ripng->version)
    {
		//if (IS_RIPNG_DEBUG_PACKET)
		{
        	zlog_debug(RIPNG_DBG_PACKET, "RIPng packet version %d doesn't fit to my version %d",
            	      packet->version, ripng->version);
		}
        ripng_peer_bad_packet(&from, ripng->peer_list);
        ripng_stats.rx_errors++;
        return 0;
    }

    /* Process RIPng packet. */
    switch (packet->command)
    {
        case RIPNG_REQUEST:
            /* When passive interface is specified, suppress responses */
            ripng_request_process(packet, len, &from, ifc, ripng);
            break;
        case RIPNG_RESPONSE:
            ripng_response_process(packet, len, &from, ifp, 255, ripng);
            break;
        default:
			//if (IS_RIPNG_DEBUG_PACKET)
			{
            	zlog_debug(RIPNG_DBG_PACKET, "Invalid RIPng command %d", packet->command);
			}
            ripng_peer_bad_packet(&from, ripng->peer_list);
            break;
    }

    return 0;
}

/* Walk down the RIPng routing table then clear changed flag. */
void
ripng_clear_changed_flag(struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;

    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
        if ((list = (struct list *)rp->info) != NULL)
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                UNSET_FLAG(rinfo->flags, RIPNG_RTF_CHANGED);
                /* This flag can be set only on the first entry. */
                break;
            }
}

/* Regular update of RIPng route.  Send all routing formation to RIPng
   enabled interface. */
int
ripng_update_process(int route_type, struct ripng_instance *ripng, u_int32_t metric)
{
    struct listnode *node;
    struct interface *ifp;
    struct rip_interface *ri;
	struct connected *ifc;
    struct listnode *ifnode, *ifnnode;

    /* Supply routes to each interface. */
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        ri = (struct rip_interface *)ifp->info;

        if (if_is_loopback(ifp) || ! if_is_up(ifp))
            continue;

        /* When passive interface is specified, suppress announce to the
           interface. */
        if (!ri->ripng_enable || ri->ipv6_passive || (ri->ripng_enable != ripng->id))
            continue;

		for (ALL_LIST_ELEMENTS(ifp->connected, ifnode, ifnnode, ifc))
		{
			if(ifc->address && ifc->address->family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&ifc->address->u.prefix6))
			{
				ripng_output_process(ifc, NULL, route_type, ripng, metric);
			}
		}
    }

    return 0;
}

/* Regular update of RIPng route.  Send all routing formation to RIPng
   enabled interface. */
int
ripng_update(void *t)
{
    struct ripng_instance *ripng = NULL;

    ripng = (struct ripng_instance *)(t);

    if (!ripng)
    {
        return -1;
    }

    /* Clear update timer thread. */
    ripng->t_update = 0;

    /* Logging update event. */
    //if (IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug(RIPNG_DBG_EVENT, "RIPng update timer expired!");
    }
    /* Supply routes to each interface. */
	ripng_update_process(ripng_all_route, ripng, 0);

    /* Triggered updates may be suppressed if a regular update is due by
       the time the triggered update would be sent. */
	RIPNG_TIMER_OFF(ripng->t_triggered_interval);
	RIPNG_TIMER_OFF(ripng->t_triggered_update);
    ripng->trigger = 0;

    /* Reset flush event. */
    ripng_event(RIPNG_UPDATE_EVENT, ripng);

    return 0;
}

/* Triggered update interval timer. */
int
ripng_triggered_interval(void *t)
{
    struct ripng_instance *ripng = NULL;

    ripng = (struct ripng_instance *)(t);

    if (!ripng)
    {
        return -1;
    }

    ripng->t_triggered_interval = 0;

    if (ripng->trigger)
    {
        ripng->trigger = 0;		
        ripng_triggered_update(t);
    }
	
    return 0;
}

/* Execute triggered update. */
int
ripng_triggered_update(void *t)
{
    struct ripng_instance *ripng = NULL;
    int interval;

    ripng = (struct ripng_instance *)(t);

    if (!ripng)
    {
        return -1;
    }

    ripng->t_triggered_update = 0;

    /* Cancel interval timer. */
	RIPNG_TIMER_OFF(ripng->t_triggered_interval);
    ripng->trigger = 0;

	/* Logging triggered update. */
	//if (IS_RIPNG_DEBUG_EVENT)
	{
		zlog_debug(RIPNG_DBG_EVENT, "RIPng triggered update!");
	}
	/* Split Horizon processing is done when generating triggered
	   updates as well as normal updates (see section 2.6). */
	ripng_update_process(ripng_changed_route, ripng, 0);

    /* Once all of the triggered updates have been generated, the route
       change flags should be cleared. */
    ripng_clear_changed_flag(ripng);

    /* After a triggered update is sent, a timer should be set for a
       random interval between 1 and 5 seconds.  If other changes that
       would trigger updates occur before the timer expires, a single
       update is triggered when the timer expires. */
    interval = (random() % 5) + 1;

    /*ripng->t_triggered_interval = thread_add_timer(master_rip, ripng_triggered_interval, ripng, interval);*/
	ripng->t_triggered_interval = high_pre_timer_add ((char *)"ripng_tri_i_timer",\
							LIB_TIMER_TYPE_NOLOOP, ripng_triggered_interval, ripng, (interval)*1000);

    return 0;
}

/* Write routing table entry to the stream and return next index of
   the routing table entry in the stream. */
int
ripng_write_rte(int num, struct stream *s, struct prefix_ipv6 *p,
                struct in6_addr *nexthop, u_int16_t tag, u_char metric)
{
    /* RIPng packet header. */
    if (num == 0)
    {
        stream_putc(s, RIPNG_RESPONSE);
        stream_putc(s, RIPNG_V1);
        stream_putw(s, 0);
    }

    /* Write routing table entry. */
    if (!nexthop)
        stream_write(s, (u_char *) &p->prefix, sizeof(struct in6_addr));
    else
        stream_write(s, (u_char *) nexthop, sizeof(struct in6_addr));

    stream_putw(s, tag);

    if (p)
        stream_putc(s, p->prefixlen);
    else
        stream_putc(s, 0);

    stream_putc(s, metric);

    return ++num;
}


/* Send RESPONSE message to specified destination. */
void
ripng_output_process(struct connected *ifc, struct sockaddr_in6 *to, int route_type,
                     struct ripng_instance *ripng, u_int32_t metric)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct rip_interface *ri;
    struct ripng_aggregate *aggregate;
    struct prefix_ipv6 *p;
    struct list *list = NULL;
    struct listnode *lnode = NULL;
    struct list *ripng_rte_list = NULL;

    ripng_rte_list = ripng_rte_new();

    //if (IS_RIPNG_DEBUG_EVENT)
    {
        if (to)
        {
            zlog_debug(RIPNG_DBG_EVENT, "RIPng update routes to neighbor %s",
                       inet6_ntoa(to->sin6_addr));
        }
        else
        {
            zlog_debug(RIPNG_DBG_EVENT, "RIPng update routes on interface %s", ifc->ifp->name);
        }
    }

    /* Get RIPng interface. */
    ri = (struct rip_interface *)ifc->ifp->info;

    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
    {
		list = (struct list *)rp->info;
		if(!list)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
			}
			continue ;
		}
		lnode = listhead(list);
		if(!lnode)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
			}
			continue;
		}
		rinfo = (struct ripng_info *)listgetdata(lnode);
		if(!rinfo)
		{
			//if(IS_RIPNG_DEBUG_EVENT)		
			{			
				zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
			}
			continue;
		}
		
        if (!rinfo->suppress)
        {
            /* If no route-map are applied, the RTE will be these following
             * informations.
             */
            p = (struct prefix_ipv6 *) & rp->p;
            rinfo->metric_out = rinfo->metric;
            rinfo->tag_out    = rinfo->tag;
            memset(&rinfo->nexthop_out, 0, sizeof(rinfo->nexthop_out));

            /* In order to avoid some local loops,
             * if the RIPng route has a nexthop via this interface, keep the nexthop,
             * otherwise set it to 0. The nexthop should not be propagated
             * beyond the local broadcast/multicast area in order
             * to avoid an IGP multi-level recursive look-up.
             */
            if (rinfo->ifindex == ifc->ifp->ifindex)
                rinfo->nexthop_out = rinfo->nexthop;

            /* Changed route only output. */
            if (route_type == ripng_changed_route &&
                    (!(rinfo->flags & RIPNG_RTF_CHANGED)))
                continue;

#if 0
            /* When the route-map does not set metric. */
            if (! rinfo->metric_set)
            {
                /* If the route is not connected or localy generated
                   one, use default-metric value */
                if (rinfo->metric != RIPNG_METRIC_INFINITY)
                {
                }
            }
			#endif
			
			rinfo->metric_out = ri->ipv6_metric ? rinfo->metric + ri->ipv6_metric + 1 : rinfo->metric + 1;

            /* Split horizon. */
            if ((ri->ipv6_split_horizon == RIP_SPLIT_HORIZON) && (ri->ipv6_poison_reverse != RIP_SPLIT_HORIZON_POISONED_REVERSE))
            {
                /* We perform split horizon for RIPng routes. */
                int suppress = 0;
                struct ripng_info *tmp_rinfo = NULL;

                for (ALL_LIST_ELEMENTS_RO(list, lnode, tmp_rinfo))
                    if (tmp_rinfo->type == ROUTE_PROTO_RIPNG && tmp_rinfo->sub_type != RIPNG_ROUTE_INTERFACE  
							&& tmp_rinfo->ifindex == ifc->ifp->ifindex)
                    {
                        suppress = 1;
                        break;
                    }

                if (suppress)
                    continue;
            }

            if (rinfo->metric_out > RIPNG_METRIC_INFINITY)
                rinfo->metric_out = RIPNG_METRIC_INFINITY;

            /* Perform split-horizon with poisoned reverse
             * for RIPng routes.
             **/
            if (ri->ipv6_poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
            {
                struct ripng_info *tmp_rinfo = NULL;

                for (ALL_LIST_ELEMENTS_RO(list, lnode, tmp_rinfo))
                {
                    if (!tmp_rinfo)
                        continue;

                    if ((tmp_rinfo->type == ROUTE_PROTO_RIPNG) && tmp_rinfo->sub_type != RIPNG_ROUTE_INTERFACE  
							&& tmp_rinfo->ifindex == ifc->ifp->ifindex)
                    {
                        rinfo->metric_out = RIPNG_METRIC_INFINITY;
                    }
                }
            }

            if (metric)
            {
                rinfo->metric_out = metric;
            }

            /* Add RTE to the list */
            ripng_rte_add(ripng_rte_list, p, rinfo, NULL);
        }

        /* Process the aggregated RTE entry */
        if ((aggregate = rp->aggregate) != NULL &&
                aggregate->count > 0 &&
                aggregate->suppress == 0)
        {
            /* If no route-map are applied, the RTE will be these following
             * informations.
             */
            p = (struct prefix_ipv6 *) & rp->p;
            aggregate->metric_set = 0;
            aggregate->metric_out = aggregate->metric;
            aggregate->tag_out    = aggregate->tag;
            memset(&aggregate->nexthop_out, 0, sizeof(aggregate->nexthop_out));

            /* Changed route only output. */

            /* XXX, vincent, in order to increase time convergence,
             * it should be announced if a child has changed.
             */
            if (route_type == ripng_changed_route)
                continue;

            if (aggregate->metric_out > RIPNG_METRIC_INFINITY)
                aggregate->metric_out = RIPNG_METRIC_INFINITY;

            /* Add RTE to the list */
            ripng_rte_add(ripng_rte_list, p, NULL, aggregate);
        }

    }

    /* Flush the list */
    ripng_rte_send(ripng_rte_list, ifc, to, ripng);

	/* free all send data */
	if(ripng_rte_list)
	{
		list_delete(ripng_rte_list);
	}
	ripng_rte_list = NULL;
}


/* Create new RIPng instance and set it to global variable. */
struct ripng_instance *
ripng_create(const u_char id)
{
    struct ripng_instance *ripng = NULL;

    /* Allocaste RIPng instance. */
    ripng = XCALLOC(MTYPE_RIPNG, sizeof(struct ripng_instance));

    /* Default version and timer values. */
    ripng->id = id;
    ripng->version = RIPNG_V1;
	ripng->distance = ROUTE_METRIC_RIP;
    ripng->update_time = RIPNG_UPDATE_TIMER_DEFAULT;
    ripng->timeout_time = RIPNG_TIMEOUT_TIMER_DEFAULT;
    ripng->garbage_time = RIPNG_GARBAGE_TIMER_DEFAULT;
    ripng->default_metric = RIPNG_DEFAULT_METRIC_DEFAULT;
	ripng->ifcount = 0;
	ripng->triggered_flag = 0;

    /* Initialize RIPng routig table. */
    ripng->table = route_table_init();
    ripng->aggregate = route_table_init();
    ripng->redist_infos = list_new();
    ripng->peer_list = list_new();
    ripng->peer_list->cmp = (int (*)(void *, void *)) ripng_peer_list_cmp;

    memset(&ripng_stats, 0, sizeof(ripng_stats));

    /* Threads. */
    ripng_event(RIPNG_UPDATE_EVENT, ripng);

    return ripng;
}

struct ripng_instance *
ripng_instance_lookup(const u_char id)
{
    struct ripng_instance *ripng = NULL;
    struct listnode *node = NULL;

    for (ALL_LIST_ELEMENTS_RO(ripng_list, node, ripng))
    {
        if (ripng->id && id && (ripng->id == id))
        {
            return ripng;
        }
    }

    return NULL;
}

struct ripng_instance *
ripng_instance_get(struct vty *vty, const u_char id)
{
    struct ripng_instance *ripng = NULL;

    ripng = ripng_instance_lookup(id);

    if (!ripng)
    {
        ripng = ripng_create(id);
        listnode_add(ripng_list, ripng);

        //if (IS_RIPNG_DEBUG_EVENT)
        {
            zlog_debug(RIPNG_DBG_EVENT, "add ripng instance %d", ripng->id);
        }
    }

    if (vty)
    {
        vty->node = RIPNG_NODE;
        vty->index = ripng;
    }

    return ripng;
}

int
ripng_instance_delete(struct vty *vty, const u_char id)
{
    struct ripng_instance *ripng;

    ripng = ripng_instance_lookup(id);

    if (ripng == NULL)
    {
        vty_error_out(vty, "Can't find RIPng instance %d%s", id, VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    //if (IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug(RIPNG_DBG_EVENT, "delete ripng instance %d", ripng->id);
		zlog_debug(RIPNG_DBG_EVENT, "send delete msg to all interface");
    }
    ripng_update_process(rip_all_route, ripng, RIP_METRIC_INFINITY);	
    ripng_clean(ripng);
    listnode_delete(ripng_list, ripng);
    XFREE(MTYPE_RIPNG, ripng);
	ripng = NULL;
    return 0;
}

/* Send RIPng request to the interface. */
int
ripng_request(struct interface *ifp)
{
    struct rte_ipv6 *rte;
    struct ripng_packet ripng_packet;
	struct connected *ifc;
    struct listnode *node, *nnode;

    /* In default ripd doesn't send RIP_REQUEST to the loopback interface. */
    if (if_is_loopback(ifp))
        return 0;

    /* If interface is down, don't send RIP packet. */
    if (! if_is_up(ifp))
        return 0;

    //if (IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug(RIPNG_DBG_EVENT, "RIPng send request to %s", ifp->name);
    }

    memset(&ripng_packet, 0, sizeof(ripng_packet));
    ripng_packet.command = RIPNG_REQUEST;
    ripng_packet.version = RIPNG_V1;
    rte = ripng_packet.rte;
    rte->metric = RIPNG_METRIC_INFINITY;

	
    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, ifc))
    {
    	if(ifc->address && ifc->address->family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&ifc->address->u.prefix6))
    	{
    		ripng_send_packet((u_char *) &ripng_packet, sizeof(ripng_packet),
                      		NULL, ifc);
    	}
    }
    return -1;
}


static int
ripng_update_jitter(int time)
{
    return ((random() % (time + 1)) - (time / 2));
}

void
ripng_event(enum ripng_event event, struct ripng_instance *ripng)
{
    int jitter = 0;

    switch (event)
    {
        case RIPNG_UPDATE_EVENT:
            if (ripng->t_update)
            {
                /*thread_cancel(ripng->t_update);
                ripng->t_update = NULL;*/
                RIPNG_TIMER_OFF(ripng->t_update);
            }

            /* Update timer jitter. */
            jitter = ripng_update_jitter(ripng->update_time);
			if(0 == ripng->t_update)
			{
            	/*ripng->t_update =
                	thread_add_timer(master_rip, ripng_update, ripng, ripng->update_time + jitter);*/
				ripng->t_update = high_pre_timer_add ((char *)"ripng_up_timer",\
							LIB_TIMER_TYPE_NOLOOP, ripng_update, ripng, (ripng->update_time + jitter)*1000);
			}
            break;
        case RIPNG_TRIGGERED_UPDATE:
            if (ripng->t_triggered_interval)
            {
                ripng->trigger = 1;
            }
            else if (! ripng->t_triggered_update)
            {
                /*ripng->t_triggered_update = thread_add_timer(master_rip, ripng_triggered_update, ripng, 1);*/
				ripng->t_triggered_update = high_pre_timer_add ((char *)"ripng_tri_u_timer",\
				 				LIB_TIMER_TYPE_NOLOOP, ripng_triggered_update, ripng, 1000);
            }
            break;
        default:
            break;
    }
}


/* Print out routes update time. */
/*static void
ripng_vty_out_uptime(struct vty *vty, struct ripng_info *rinfo)
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
        vty_out(vty, "%5s", timebuf);
    }
    else if ((thread = rinfo->t_garbage_collect) != NULL)
    {
        clock = thread_timer_remain_second(thread);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, "%5s", timebuf);
    }
}
*/

/* Print out routes update time. */
static void
ripng_vty_out_uptime_new(struct vty *vty, struct ripng_info *rinfo, struct ripng_instance *ripng)
{
    time_t clock;
    struct tm *tm;
#define TIME_BUF 25
    char timebuf [TIME_BUF];
    TIMERID		thread;

    if ((thread = rinfo->t_timeout) != 0)
    {
        clock = ripng_thread_timer_remain_second(thread, ripng->timeout_time);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, "%5s", timebuf);
    }
    else if ((thread = rinfo->t_garbage_collect) != 0)
    {
        clock = ripng_thread_timer_remain_second(thread, ripng->garbage_time);
        tm = gmtime(&clock);
        strftime(timebuf, TIME_BUF, "%M:%S", tm);
        vty_out(vty, "%5s", timebuf);
    }
}



void
ripng_table_distance_change(struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *node = NULL;

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL && listcount(list) != 0)
        {
            for (ALL_LIST_ELEMENTS_RO(list, node, rinfo))
            {
                if (NULL == rinfo)
                    continue;

                if (ripng_route_rte(rinfo) && rinfo->metric != RIP_METRIC_INFINITY)
                {
                    rinfo->distance = ripng->distance;
                    ripng_zebra_ipv6_update(rp);
                }
            }
        }
    }
}

static char *
ripng_route_subtype_print(struct ripng_info *rinfo)
{
    static char str[3];
    memset(str, 0, 3);

    if (rinfo->suppress)
        strcat(str, "S");

    switch (rinfo->sub_type)
    {
        case RIPNG_ROUTE_RTE:
            strcat(str, "n");
            break;
        case RIPNG_ROUTE_REDISTRIBUTE:
            strcat(str, "r");
            break;
        case RIPNG_ROUTE_INTERFACE:
            strcat(str, "i");
            break;
        default:
            strcat(str, "?");
            break;
    }

    return str;
}

void
get_interface_for_short(ifindex_t ifindex, char *shortname)
{
	int n,i = 0;
	const char *names = ifindex2ifname(ifindex);

	n = strlen(names);
	for(i = 0; i < n; i++)
	{
		if(names[i] == ' ')
			break;
	}

	if(i != n)
	{
		switch (names[0])
		{
			case 'e':
				strcat(shortname, "Eth");
				break;
			case 'g':
				strcat(shortname, "GE");
				break;
			case 'x':
				strcat(shortname, "XGE");
				break;
			case 't':
				strcat(shortname, "Tru");
				break;
			case 'l':
				strcat(shortname, "Lo");
				break;
			case 'v':
				strcat(shortname, "Vlan");
				break;
			default:
				strcat(shortname, "?");
				break;
		}
		memcpy(shortname + strlen(shortname), names + i + 1, n - i - 1);
		
	}
	else
	{
		strcat(shortname, "unknown");
	}

}

DEFUN(show_ipv6_ripng,
      show_ipv6_ripng_cmd,
      "show ripng <1-255> route",
      SHOW_STR
      RIPNG_STR
      "instance number\n"
      "Route information\n")
{
    struct ripng_instance *ripng = NULL;
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct ripng_aggregate *aggregate;
    struct prefix_ipv6 *p;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    int len;
    u_char id;
	char name[INTERFACE_NAMSIZ];

    id = (u_char)atoi(argv[0]);
    ripng = ripng_instance_lookup(id);

    if (! ripng)
    {
	    VTY_RIP_INSTANCE_ERR
    }

    /* Header of display. */
    vty_out(vty, "Codes: R - RIPng, C - connected, S - Static, O - OSPF, B - IBGP, I - ISIS%s"
            " 	E - EBGP %s"
            "Sub-codes:%s"
            "      (n) - normal, (s) - static, (d) - default, (r) - redistribute,%s"
            "      (i) - interface, (a/S) - aggregated/Suppressed%s%s"
            "     Network%s      Next Hop                                   Via        Metric  Tag   Time%s",
            VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE,
            VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
    {
        if ((aggregate = rp->aggregate) != NULL)
        {
            p = (struct prefix_ipv6 *) & rp->p;

#ifdef DEBUG
            vty_out(vty, "R(a) %d/%d %s/%d%s",
                          aggregate->count, aggregate->suppress,
                          inet6_ntoa(p->prefix), p->prefixlen, VTY_NEWLINE);
#else
            vty_out(vty, "R(a) %s/%d%s",
                          inet6_ntoa(p->prefix), p->prefixlen, VTY_NEWLINE);
#endif /* DEBUG */
            vty_out(vty, "%*s", 6, " ");
            vty_out(vty, "                                         self          %2d      %d%s", aggregate->metric,
                    aggregate->tag,
                    VTY_NEWLINE);
        }

        if ((list = (struct list *)rp->info) != NULL)
            for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
            {
                p = (struct prefix_ipv6 *) & rp->p;

#ifdef DEBUG
                vty_out(vty, "%c(%s) 0/%d %s/%d %s",
                              rip_zebra_route_char(rinfo->type),
                              ripng_route_subtype_print(rinfo),
                              rinfo->suppress,
                              inet6_ntoa(p->prefix), p->prefixlen, VTY_NEWLINE);
#else
                vty_out(vty, "%c(%s) %s/%d %s",
                              rip_zebra_route_char(rinfo->type),
                              ripng_route_subtype_print(rinfo),
                              inet6_ntoa(p->prefix), p->prefixlen, VTY_NEWLINE);
#endif /* DEBUG */
                vty_out(vty, "%*s", 6, " ");
                len = vty_out(vty, "%s", inet6_ntoa(rinfo->nexthop));

                len = 41 - len;

                if (len > 0)
                    len = vty_out(vty, "%*s", len, " ");

                /* from */
                if ((ROUTE_PROTO_RIPNG == rinfo->type) 
						&& (RIPNG_ROUTE_RTE == rinfo->sub_type))
                {
					memset(name, 0, INTERFACE_NAMSIZ);
					get_interface_for_short(rinfo->ifindex, name);
                    len = vty_out(vty, "%s", name);
                }
                else if (rinfo->metric == RIPNG_METRIC_INFINITY)
                {
                    len = vty_out(vty, "kill");
                }
                else
                {					
                    if (rinfo->external_id)
                    {
                        len = vty_out(vty, "self(%d)", rinfo->external_id);
                    }
                    else
                    {
                        len = vty_out(vty, "self");
                    }

					
                }
				
				len = 14 - len;
				
				if (len > 0)
					vty_out(vty, "%*s", len, " ");

                vty_out(vty, "%2d      %d    ",
                        rinfo->metric, rinfo->tag);

                /* time */
                if ((ROUTE_PROTO_RIPNG == rinfo->type) &&
                        (RIPNG_ROUTE_RTE == rinfo->sub_type))
                {
                    /* RTE from remote RIP routers */
                    //ripng_vty_out_uptime(vty, rinfo);
					ripng_vty_out_uptime_new(vty, rinfo, ripng);
                }
                else if (RIPNG_METRIC_INFINITY == rinfo->metric)
                {
                    /* poisonous reversed routes (gc) */
                    //ripng_vty_out_uptime(vty, rinfo);
					ripng_vty_out_uptime_new(vty, rinfo, ripng);
                }

                vty_out(vty, "%s", VTY_NEWLINE);
            }
    }

    return CMD_SUCCESS;
}

DEFUN(show_ipv6_ripng_status,
      show_ipv6_ripng_status_cmd,
      "show ripng instance <1-255>",
      SHOW_STR
      RIPNG_STR
      "RIPng routing protocol instance\n"
      "instance number <1-255>\n")
{
    struct ripng_instance *ripng = NULL;
    u_char id;

    id = (u_char)atoi(argv[0]);
    ripng = ripng_instance_lookup(id);

    if (! ripng)
    {
    	VTY_RIP_INSTANCE_ERR
    }
    vty_out(vty, "Routing Protocol:\"RIPng\"%s", VTY_NEWLINE);
    vty_out(vty, "  RIPng instance: %d%s", ripng->id, VTY_NEWLINE);

    vty_out(vty, "  Sending updates every %ld seconds with +/-50%%,",
            ripng->update_time);
    /*vty_out(vty, " next due in %lu seconds%s",
            thread_timer_remain_second(ripng->t_update),
            VTY_NEWLINE);*/
	vty_out(vty, " next due in %lu seconds%s",
            ripng_thread_timer_remain_second(ripng->t_update, ripng->update_time),
            VTY_NEWLINE);
    vty_out(vty, "  Timeout after %ld seconds,", ripng->timeout_time);
    vty_out(vty, " garbage collect after %ld seconds%s", ripng->garbage_time,
            VTY_NEWLINE);
	
    vty_out(vty, "  distance: %d%s", ripng->distance, VTY_NEWLINE);
    vty_out(vty, "  maximum number of ECMP: %d%s", NHP_ECMP_NUM, VTY_NEWLINE);
    vty_out(vty, "  Number of interfaces enabled : %d%s", ripng->ifcount, VTY_NEWLINE);

    /* Redistribute information. */
    vty_out(vty, "  default metric is 0%s", VTY_NEWLINE);
    vty_out(vty, "Redistributing:");
    config_write_ripng_redistribute(vty, ripng);
    vty_out(vty, "%s", VTY_NEWLINE);


    return CMD_SUCCESS;
}

DEFUN(router_ripng,
      router_ripng_cmd,
      "ripng instance <1-255>",
      RIPNG_STR
      "instance configure\n"
      "instance number <1-255>\n")
{
    u_char id;
    struct ripng_instance *ripng = NULL;

    id = (u_char)atoi(argv[0]);
    ripng = ripng_instance_get(vty, id);

    if (ripng)
        return CMD_SUCCESS;
    else
        return CMD_WARNING;
}

DEFUN(no_router_ripng,
      no_router_ripng_cmd,
      "no ripng instance <1-255>",
      NO_STR
      RIPNG_STR
      "instance configure\n"
      "instance number <1-255>\n")
{
    u_char id;
    id = (u_char)atoi(argv[0]);

    ripng_instance_delete(vty, id);
    return CMD_SUCCESS;
}

DEFUN(ripng_aggregate_address,
      ripng_aggregate_address_cmd,
      "aggregate-address X:X::X:X/M",
      "Set aggregate RIPng route announcement\n"
      "Aggregate network\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    int ret;
    struct prefix p;
    struct route_node *node;

    if (!ripng)
    {
        VTY_RIP_INSTANCE_ERR
    }

    ret = str2prefix_ipv6(argv[0], (struct prefix_ipv6 *) & p);

    if (ret <= 0)
    {
        vty_error_out(vty, "Malformed address%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Check aggregate alredy exist or not. */
    node = route_node_get(ripng->aggregate, &p);

    if (node->info)
    {
        vty_error_out(vty, "There is already same aggregate route.%s", VTY_NEWLINE);
        route_unlock_node(node);
        return CMD_WARNING;
    }

    node->info = (void *)1;

    ripng_aggregate_add(&p, ripng);

    return CMD_SUCCESS;
}

DEFUN(no_ripng_aggregate_address,
      no_ripng_aggregate_address_cmd,
      "no aggregate-address X:X::X:X/M",
      NO_STR
      "Delete aggregate RIPng route announcement\n"
      "Aggregate network")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    int ret;
    struct prefix p;
    struct route_node *rn;

    if (!ripng)
    {
        VTY_RIP_INSTANCE_ERR
    }

    ret = str2prefix_ipv6(argv[0], (struct prefix_ipv6 *) & p);

    if (ret <= 0)
    {
        vty_error_out(vty, "Malformed address%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    rn = route_node_lookup(ripng->aggregate, &p);

    if (! rn)
    {
        vty_error_out(vty, "Can't find aggregate route.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    route_unlock_node(rn);
    rn->info = NULL;
    route_unlock_node(rn);

    ripng_aggregate_delete(&p, ripng);

    return CMD_SUCCESS;
}

#if 0
DEFUN(ripng_default_metric,
      ripng_default_metric_cmd,
      "default-metric <1-16>",
      "Set a metric of redistribute routes\n"
      "Default metric\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;

    if (ripng)
    {
        ripng->default_metric = atoi(argv[0]);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ripng_default_metric,
      no_ripng_default_metric_cmd,
      "no default-metric",
      NO_STR
      "Set a metric of redistribute routes\n"
      "Default metric\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;

    if (ripng)
    {
        ripng->default_metric = RIPNG_DEFAULT_METRIC_DEFAULT;
    }

    return CMD_SUCCESS;
}

ALIAS(no_ripng_default_metric,
      no_ripng_default_metric_val_cmd,
      "no default-metric <1-16>",
      NO_STR
      "Set a metric of redistribute routes\n"
      "Default metric\n")
#endif

DEFUN(ripng_timers,
      ripng_timers_cmd,
      "timers <0-3600> <0-3600> <0-3600>",
      "Set RIPng timers\n"
      "Routing table update timer value in second. Default is 30.\n"
      "Routing information timeout timer. Default is 180.\n"
      "Garbage collection timer. Default is 120.\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
    unsigned long update;
    unsigned long timeout;
    unsigned long garbage;

    VTY_GET_INTEGER_RANGE("update timer", update, argv[0], 0, 3600);
    VTY_GET_INTEGER_RANGE("timeout timer", timeout, argv[1], 0, 3600);
    VTY_GET_INTEGER_RANGE("garbage timer", garbage, argv[2], 0, 3600);

    if (update >= timeout || update >= garbage)
    {
        vty_error_out(vty, "update time should be small than age time and garbage-collect time%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* Set each timer value. */
    ripng->update_time = update;
    ripng->timeout_time = timeout;
    ripng->garbage_time = garbage;

    /* Reset update timer thread. */
    ripng_event(RIPNG_UPDATE_EVENT, ripng);

    return CMD_SUCCESS;
}

DEFUN(no_ripng_timers,
      no_ripng_timers_cmd,
      "no timers",
      NO_STR
      "Set RIPng timers\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;

    /* Set each timer value to the default. */
    ripng->update_time = RIPNG_UPDATE_TIMER_DEFAULT;
    ripng->timeout_time = RIPNG_TIMEOUT_TIMER_DEFAULT;
    ripng->garbage_time = RIPNG_GARBAGE_TIMER_DEFAULT;

    /* Reset update timer thread. */
    ripng_event(RIPNG_UPDATE_EVENT, ripng);

    return CMD_SUCCESS;
}

DEFUN(ripng_distance,
      ripng_distance_cmd,
      "distance <1-255>",
	  "Set RIPng distance\n"
      "Distance value\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;
	u_char dis = atoi(argv[0]);

    if (ripng == NULL)
    {
        vty_error_out(vty, "Can't find RIP instance %s", VTY_NEWLINE);
        return CMD_ERR_NOTHING_TODO;
    }

	if(ripng->distance == dis)
	{
		return CMD_SUCCESS;
	}

    ripng->distance = dis;
    ripng_table_distance_change(ripng);
    return CMD_SUCCESS;
}

DEFUN(no_ripng_distance,
      no_ripng_distance_cmd,
      "no distance",
      NO_STR
      "Set RIPng distance\n")
{
    struct ripng_instance *ripng = (struct ripng_instance *)vty->index;

    if (ripng == NULL)
    {
		VTY_RIP_INSTANCE_ERR
    }

	if(ripng->distance == ROUTE_METRIC_RIP)
	{
		return CMD_SUCCESS;
	}

    ripng->distance = ROUTE_METRIC_RIP;
    ripng_table_distance_change(ripng);
    return CMD_SUCCESS;
}

DEFUN(show_ip_ripng_statistics,
      show_ipv6_ripng_statistics_cmd,
      "show ripng statistics",
      SHOW_STR
      RIPNG_STR
      "RIPNG statistics information\n")
{
    struct ripng_instance *ripng;
    struct route_node *np;
    struct ripng_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *listnode = NULL;
    struct listnode *ripnglistnode = NULL;
    long i = 0;
    long j = 0;
    long k = 0;

    vty_out(vty, "RIPng Statistical information is %s", VTY_NEWLINE);
    vty_out(vty, " 	packet sent is %lu %s", ripng_stats.tx_packets, VTY_NEWLINE);
    vty_out(vty, " 	packet received is %lu %s", ripng_stats.rx_packets, VTY_NEWLINE);
    vty_out(vty, " 	error packet received is %lu %s", ripng_stats.rx_errors, VTY_NEWLINE);
    vty_out(vty, " 	packet send failed is %lu %s", ripng_stats.tx_failed, VTY_NEWLINE);
    vty_out(vty, " 	routes received is %lu %s", ripng_stats.rx_routes, VTY_NEWLINE);
    vty_out(vty, " 	bad routes received is %lu %s", ripng_stats.rx_bad_routes, VTY_NEWLINE);
    vty_out(vty, " 	routes send is %lu %s", ripng_stats.tx_routes, VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(ripng_list, ripnglistnode, ripng))
    {

        i = 0;
        j = 0;
        k = 0;

        for (np = route_top(ripng->table); np; np = route_next(np))
        {
            if ((list = (struct list *)np->info) != NULL)
                for (ALL_LIST_ELEMENTS_RO(list, listnode, rinfo))
                {
                    i++;

                    if (rinfo->metric != RIP_METRIC_INFINITY)
                    {
                        j++;

                        if ((ROUTE_PROTO_RIPNG == rinfo->type) 
								&& CHECK_FLAG(rinfo->sub_type, RIPNG_ROUTE_RTE))
                            k++;
                    }
                }
        }

        vty_out(vty, "   ------------- ripng instance %d------------ %s", ripng->id, VTY_NEWLINE);

        vty_out(vty, " 	all routes in table is %lu %s", i, VTY_NEWLINE);
        vty_out(vty, " 	active routes in table is %lu %s", j, VTY_NEWLINE);
        vty_out(vty, " 	learned active routes in table is %lu %s", k, VTY_NEWLINE);

    }

    return CMD_SUCCESS;

}

/* RIPng configuration write function. */
int
ripng_config_write(struct vty *vty)
{
    int write = 0;
    struct route_node *rp;
    struct ripng_instance *ripng = NULL;
    struct listnode *node = NULL;

    for (ALL_LIST_ELEMENTS_RO(ripng_list, node, ripng))
    {
        if (ripng)
        {

            write++;
            /* RIPng router. */
            vty_out(vty, "ripng instance %d%s", ripng->id, VTY_NEWLINE);

            /* RIPng timers configuration. */
            if (ripng->update_time != RIPNG_UPDATE_TIMER_DEFAULT ||
                    ripng->timeout_time != RIPNG_TIMEOUT_TIMER_DEFAULT ||
                    ripng->garbage_time != RIPNG_GARBAGE_TIMER_DEFAULT)
            {
                vty_out(vty, " timers %ld %ld %ld%s",
                        ripng->update_time,
                        ripng->timeout_time,
                        ripng->garbage_time,
                        VTY_NEWLINE);
            }

			if(ROUTE_METRIC_RIP != ripng->distance)
			{
            	vty_out(vty, " distance %d%s", ripng->distance, VTY_NEWLINE);
			}

            /* RIPng default metric configuration */
            if (ripng->default_metric != RIPNG_DEFAULT_METRIC_DEFAULT)
                vty_out(vty, " default-metric %d%s",
                        ripng->default_metric, VTY_NEWLINE);

            config_write_ripng_redistribute(vty, ripng);


            /* RIPng aggregate routes. */
            for (rp = route_top(ripng->aggregate); rp; rp = route_next(rp))
                if (rp->info != NULL)
                    vty_out(vty, " aggregate-address %s/%d%s",
                            inet6_ntoa(rp->p.u.prefix6), rp->p.prefixlen, VTY_NEWLINE);

        }
    }

    return write;
}

void
ripng_interface_reset(u_char id)
{
    struct listnode *node;
    struct interface *ifp;
    struct rip_interface *ri;

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        ri = (struct rip_interface *)ifp->info;

        if (ri->ripng_enable && ri->ripng_enable == id)
        {
            ri->ripng_enable = 0;
            ri->ipv6_passive = 0;
            ri->ipv6_poison_reverse = RIP_NO_POISON_REVERSE;
            ri->ipv6_split_horizon = RIP_SPLIT_HORIZON;
			ri->ipv6_metric = RIP_DEFAULT_METRIC_DEFAULT;
        }
    }
}

/* delete all the added ripng routes. */
void
ripng_clean(struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct ripng_aggregate *aggregate;
    struct list *list = NULL;
    struct listnode *lnode = NULL;

    if (ripng)
    {
        /* Clear RIPng routes */
        for (rp = route_top(ripng->table); rp; rp = route_next(rp))
        {
            if ((list = (struct list *)rp->info) != NULL)
            {
				lnode = listhead(list);
				if(!lnode)
				{
					//if(IS_RIPNG_DEBUG_EVENT)		
					{			
						zlog_debug(RIPNG_DBG_EVENT, "%s %d listnode is NULL", __func__, __LINE__);		
					}
					continue;
				}
				rinfo = (struct ripng_info *)listgetdata(lnode);
				if(!rinfo)
				{
					//if(IS_RIPNG_DEBUG_EVENT)		
					{			
						zlog_debug(RIPNG_DBG_EVENT, "%s %d rinfo is NULL", __func__, __LINE__);		
					}
					continue;
				}

                if (ripng_route_rte(rinfo))
                    ripng_zebra_ipv6_delete(rp);

                for (ALL_LIST_ELEMENTS_RO(list, lnode, rinfo))
                {
                    RIPNG_TIMER_OFF(rinfo->t_timeout);
                    RIPNG_TIMER_OFF(rinfo->t_garbage_collect);
                    ripng_info_free(rinfo);
					rinfo = NULL;
                }

                list_delete(list);
                rp->info = NULL;
                route_unlock_node(rp);
            }

            if ((aggregate = rp->aggregate) != NULL)
            {
                ripng_aggregate_free(aggregate);
                rp->aggregate = NULL;
                route_unlock_node(rp);
            }
        }


        if (ripng->table)
        {
            route_table_finish(ripng->table);
            ripng->table = NULL;
        }

        /* Cancel the RIPng timers */
        RIPNG_TIMER_OFF(ripng->t_update);
        RIPNG_TIMER_OFF(ripng->t_triggered_update);
        RIPNG_TIMER_OFF(ripng->t_triggered_interval);
		ripng->triggered_flag = 0;

        /* RIPng aggregated prefixes */
        for (rp = route_top(ripng->aggregate); rp; rp = route_next(rp))
            if (rp->info)
            {
                rp->info = NULL;
                route_unlock_node(rp);
            }

        if (ripng->aggregate)
        {
            route_table_finish(ripng->aggregate);
            ripng->aggregate = NULL;
        }

        ripng_interface_reset(ripng->id);
        rip_redistribute_clean(ripng->redist_infos);

    } /* if (ripng) */

}

void
ripng_clean_all(void)
{
    struct listnode *node, *nnode;
    struct ripng_instance *ripng;

    for (ALL_LIST_ELEMENTS(ripng_list, node, nnode, ripng))
    {
        ripng_clean(ripng);
        listnode_delete(ripng_list, ripng);
        XFREE(MTYPE_RIPNG, ripng);
		ripng = NULL;
    }
}

/* RIP node structure. */
static struct cmd_node ripng_node =
{
    RIPNG_NODE,
    "%s(config-ripng)# ",
    1
};

/* Initialize ripng structure and set commands. */
void
ripng_init()
{
    /* Randomize. */
    srandom(time(NULL));

    /* Install RIPNG_NODE. */
    install_node(&ripng_node, ripng_config_write);

    install_element(CONFIG_NODE, &show_ipv6_ripng_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ripng_status_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ipv6_ripng_statistics_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &router_ripng_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &no_router_ripng_cmd, CMD_SYNC);

    install_default(RIPNG_NODE);
    install_element(RIPNG_NODE, &ripng_aggregate_address_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &no_ripng_aggregate_address_cmd, CMD_SYNC);

//    install_element(RIPNG_NODE, &ripng_default_metric_cmd, CMD_SYNC);
//    install_element(RIPNG_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);
//    install_element(RIPNG_NODE, &no_ripng_default_metric_val_cmd, CMD_SYNC);

    install_element(RIPNG_NODE, &ripng_timers_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &no_ripng_timers_cmd, CMD_SYNC);

    install_element(RIPNG_NODE, &ripng_distance_cmd, CMD_SYNC);
    install_element(RIPNG_NODE, &no_ripng_distance_cmd, CMD_SYNC);
	
	ripng_peer_init();

    ripng_list = list_new();

}
