/* Interface related function for RIP.
 * Copyright (C) 1997, 98 Kunihiro Ishiguro <kunihiro@zebra.org>
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
#include "memory.h"
#include "network.h"
#include "table.h"
#include "log.h"
#include "stream.h"
#include "thread.h"
#include "privs.h"
#include "ifm_common.h"
#include "msg_ipc.h"
#include "ripd/ripd.h"
#include "ripd/rip_debug.h"
#include "ripd/rip_interface.h"

#include "ripngd/ripngd.h"

/* static prototypes */

const struct message ri_version_msg[] =
{
    {RI_RIP_VERSION_1,             "1"},
    {RI_RIP_VERSION_2,             "2"},
    {RI_RIP_VERSION_1_AND_2,     "1 2"},
    {RI_RIP_VERSION_2_BROAD,  "2 broadcast"},
    {RI_RIP_VERSION_2_MULTI,  "2 multicast"},
};
extern struct list *rip_list;

//extern struct zebra_privs_t ripd_privs;

struct rip_summary*
rip_summary_new(void)
{
	struct rip_summary *rip_summary_temp = NULL;
	
	do
	{
    	rip_summary_temp = (struct rip_summary*)XCALLOC(MTYPE_RIP_SUMMARY, sizeof(struct rip_summary));
		if (rip_summary_temp == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(rip_summary_temp == NULL);

	memset(rip_summary_temp, 0, sizeof(struct rip_summary));

	return rip_summary_temp;
}

void
rip_summary_free(void *val)
{
    XFREE(MTYPE_RIP_SUMMARY, val);
}

void
rip_neighbor_free(void *val)
{
    XFREE(MTYPE_PREFIX, val);
}

int
ri_summary_prefix_cmp(struct prefix *p1, struct prefix *p2)
{
    if (htonl(p1->u.prefix4.s_addr) > htonl(p2->u.prefix4.s_addr))
        return 1;
    else if (htonl(p1->u.prefix4.s_addr) == htonl(p2->u.prefix4.s_addr))
        if (p1->prefixlen < p2->prefixlen)
            return 1;

    return -1;
}

/* delete listnode from summary */
int
summary_only_route_delete(struct rip_summary *rs)
{
    struct rip_info *rinfo = NULL;
	struct route_node *rp = rs->tablenode;
    if (rp && rp->info)
    {
		if(!prefix_same(&rs->p, &rp->p))
		{
			zlog_err("%s %d summary point node doesn`t fit to tablenode", __func__, __LINE__);
			return -1;
		}
		
        rinfo = listgetdata(listhead((struct list *)rp->info));

		if(!rinfo)
		{
			zlog_err("list node is NULL");
			return -1;
		}
		
		if(rs->dflag)
		{
			rinfo->summary_count--;
			zlog_debug(RIP_DBG_EVENT, "summary count is %d", rinfo ? rinfo->summary_count : 100);
		}

        if (0 == rinfo->summary_count)
        {			
            UNSET_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS);

            if (!CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO))
            {
                UNSET_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY);

                if (!rinfo->sub_type)
                {
                    RS_TIMER_OFF(rinfo->t_timeout);
                    RS_TIMER_OFF(rinfo->t_garbage_collect);
                    listnode_delete((struct list *)rp->info, rinfo);

                    if (list_isempty((struct list *)rp->info))
                    {
                        list_free((struct list *)rp->info);
                        rp->info = NULL;
                        route_unlock_node(rp);
                    }

                    rip_info_free(rinfo);
					rinfo = NULL;
                }
            }
        }
    }

    return 0;
}

/* Allocate new RIP's interface configuration. */
struct rip_interface *
rip_interface_new(void)
{
    struct rip_interface *ri;

	do
	{
    	ri = (struct rip_interface *)XCALLOC(MTYPE_RIP_INTERFACE, sizeof(struct rip_interface));
		if (ri == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(ri == NULL);

	memset(ri, 0, sizeof(struct rip_interface));

    /* Default authentication type is simple password for Cisco
       compatibility. */
    memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);
    ri->auth_type = RIP_NO_AUTH;
    ri->md5_auth_len = RIP_AUTH_MD5_COMPAT_SIZE;
    ri->md5_auth_id = 0;
    ri->enable_inter_node = 0;
    ri->mode = IFNET_MODE_L3;
    /* Set default split-horizon behavior.  If the interface is Frame
       Relay or SMDS is enabled, the default value for split-horizon is
       off.  But currently Zebra does detect Frame Relay or SMDS
       interface.  So all interface is set to split horizon.  */
    ri->split_horizon = RIP_SPLIT_HORIZON;
    ri->poison_reverse = RIP_NO_POISON_REVERSE;
    ri->summary = list_new();
    ri->summary->cmp = (int (*)(void *, void *)) ri_summary_prefix_cmp;
    ri->summary->del = rip_summary_free;
    ri->neighbor = list_new();
    ri->neighbor->del = rip_neighbor_free;
    ri->passive = 0;

    ri->ripng_enable = 0;
    ri->ipv6_passive = 0;
    ri->ipv6_split_horizon = RIP_SPLIT_HORIZON;
    ri->ipv6_poison_reverse = RIP_NO_POISON_REVERSE;	
    ri->ipv6_metric = RIP_DEFAULT_METRIC_DEFAULT;
	
    return ri;
}

/* Send RIP request packet to specified interface. */
void
rip_request_interface_send(struct interface *ifp, u_char version)
{
//  struct sockaddr_in to;

    if (version == RIPv2 || version == RI_RIP_VERSION_2_MULTI)
    {
        //if (IS_RIP_DEBUG_EVENT)
            zlog_debug(RIP_DBG_EVENT, "multicast  version %d request on %s", RIPv2, ifp->name);
    }
    else
    {
        //if (IS_RIP_DEBUG_EVENT)
            zlog_debug(RIP_DBG_EVENT, "broadticast version %d request on %s", (version == RIPv1) ? RIPv1 : RIPv2, ifp->name);
    }

    rip_request_send(NULL, ifp, version, NULL);
#if 0

    /* RIPv1 and non multicast interface. */
    if (if_is_pointopoint(ifp))
    {
        struct listnode *cnode, *cnnode;
        struct connected *connected;

        if (IS_RIP_DEBUG_EVENT)
            zlog_debug("unicast request to %s", ifp->name);

        for (ALL_LIST_ELEMENTS(ifp->connected, cnode, cnnode, connected))
        {
            if (connected->address->family == AF_INET)
            {
                memset(&to, 0, sizeof(struct sockaddr_in));
                to.sin_port = htons(RIP_PORT_DEFAULT);

                if (connected->destination)
                    /* use specified broadcast or peer destination addr */
                    to.sin_addr = connected->destination->u.prefix4;
                else if (connected->address->prefixlen < IPV4_MAX_PREFIXLEN)
                    /* calculate the appropriate broadcast address */
                    to.sin_addr.s_addr =
                        ipv4_broadcast_addr(connected->address->u.prefix4.s_addr,
                                            connected->address->prefixlen);
                else
                    /* do not know where to send the packet */
                    continue;

                if (IS_RIP_DEBUG_EVENT)
                    zlog_debug("SEND request to %s", inet_ntoa(to.sin_addr));

                rip_request_send(&to, ifp, version, connected);
            }
        }
    }

#endif
}

/* This will be executed when interface goes up. */
void
rip_request_interface(struct interface *ifp)
{
    struct rip_interface *ri;
    struct rip_instance *rip;
    int vsend;

    /* In default ripd doesn't send RIP_REQUEST to the loopback interface. */
    if (if_is_loopback(ifp))
        return;

    /* If interface is down, don't send RIP packet. */
    if (! if_is_operative(ifp))
        return;

    /* Fetch RIP interface information. */
    ri = (struct rip_interface *)ifp->info;

//  ri->ri_send = RI_RIP_UNSPEC;

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
        return;

    /* If there is no version configuration in the interface,
       use rip's version setting. */
    vsend = ((ri->ri_send == RI_RIP_UNSPEC) ?
             rip->version_send : ri->ri_send);
	if(vsend == RIPv1)
	{
    	rip_request_interface_send(ifp, RIPv1);
	}
	else
	{
    	rip_request_interface_send(ifp, RIPv2);
	}
}

/* Request routes at all interfaces. */
void
rip_request_neighbor(struct interface *ifp, struct rip_instance *rip)
{
    struct connected *connected = NULL;
    struct rip_interface *ri = NULL;
    struct listnode *nnode;
    struct prefix *p = NULL;
    struct sockaddr_in to;
    int vsend;

    if (! rip)
        return;

    //if (IS_RIP_DEBUG_EVENT)
        zlog_debug(RIP_DBG_EVENT, "request to the all neighbor");

    /* Send request to all neighbor. */
    /* Check each interface. */
    ri = (struct rip_interface *)ifp->info;

    if (ri->enable_inter_node != rip->id)
        return;

    if (!listcount(ri->neighbor))
        return;

    vsend = ((ri->ri_send == RI_RIP_UNSPEC) ?
             rip->version_send : ri->ri_send);

    for (ALL_LIST_ELEMENTS_RO(ri->neighbor, nnode, p))
    {
        /* Check each neighbor. */
        if ((connected = connected_lookup_address(ifp, p->u.prefix4)) == NULL)
        {
            zlog_debug(RIP_DBG_EVENT, "Neighbor %s doesnt have connected network",
                      inet_ntoa(p->u.prefix4));
            continue;
        }

        if (connected->address->family != AF_INET)
            continue;

        /* Set destination address and port */
        memset(&to, 0, sizeof(struct sockaddr_in));
        to.sin_addr = p->u.prefix4;
        to.sin_port = htons(RIP_PORT_DEFAULT);

        /* RIP version is rip's configuration. */
		if(vsend == RIPv1)
		{
			rip_request_send(&to, ifp, RIPv1, connected);
		}
		else
		{
			rip_request_send(&to, ifp, RIPv2, connected);
		}
    }
}

/* ret 0: do nothing; ret 1: delete route; ret 2: send delete to this interface  */
int
rip_version_change_table_check(struct rip_instance *rip, int pver, int nver, uint32_t ifindex)
{
    struct route_node *rp;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;
    struct listnode *node = NULL;
    struct listnode *rnode = NULL;
	int ret;

	if(pver == nver)
	{
		zlog_err("Error: version equal before and after change");
		return 0;
	}

	if(pver >= 3)
	{
		if(nver < 3)
		{
			ret = 1;
		}
		else
		{
			ret = 2;
		}
	}
	else
	{
		if(nver < 3)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
	}

	if(ret != 1)
	{
		return ret;
	}

    /* If routes exists below this node, generate aggregate routes. */
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((NULL == (list = rp->info)) || !listcount(list))
            continue;

        for (ALL_LIST_ELEMENTS(list, node, rnode, rinfo))
        {
            if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
					&& rinfo->ifindex == ifindex)
            {
				if(ret == 1)
				{
					rip_ecmp_delete(rinfo, rip);
				}
            }
        }
    }

	if(rip->triggered_update_flag == 1)
	{
		rip_event(RIP_TRIGGERED_UPDATE, rip);
	}

	return ret;
}

/* Apply network configuration to all interface. */
void
version_change_all_interface(struct rip_instance *rip, int pver, int nver)
{
    struct interface *ifp;
    struct listnode *node, *nnode;
    struct rip_interface *ri;
	int ret;

    /* Check each interface. */
    for (ALL_LIST_ELEMENTS(iflist, node, nnode, ifp))
    {
        ri = (struct rip_interface *)ifp->info;

        if (ri->enable_inter_node != rip->id)
            continue;
		
		if(ri->ri_send)
		{
			continue;
		}
        /* Send RIP request to the interface. */
		ret = rip_version_change_table_check(rip, pver, nver, ifp->ifindex);
		if(ret == 2)
		{
			zlog_err("Error: instance version can`t return 2\n");;
		}

        /* Send RIP request to the interface. */
		rip_request_interface(ifp);
		rip_request_neighbor(ifp, rip);
        /* Send RIP routes to the interface. */
        rip_update_interface_spec(rip_all_route, ifp, rip, 0);
        rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);
    }
}

/* Is there and address on interface that I could use ? */
int
rip_if_ipv4_address_check(struct interface *ifp)
{
    struct listnode *nn;
    struct connected *connected;
    int count = 0;

    for (ALL_LIST_ELEMENTS_RO(ifp->connected, nn, connected))
    {
        struct prefix *p;

        p = connected->address;

        if (p->family == AF_INET)
            count++;

    }

    return count;
}

/* Does this address belongs to me ? */
int
if_check_address(struct in_addr addr)
{
    struct listnode *node;
    struct interface *ifp;

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        struct listnode *cnode;
        struct connected *connected;

		if(ifp == NULL)
		{
			zlog_err("%-15s[%d]: ifp is NULL!", __FUNCTION__, __LINE__);
			continue;
		}

        for (ALL_LIST_ELEMENTS_RO(ifp->connected, cnode, connected))
        {
            struct prefix_ipv4 *p;

			if(connected == NULL)
			{
				zlog_err("%-15s[%d]: connected is NULL!", __FUNCTION__, __LINE__);
				continue;
			}

            p = (struct prefix_ipv4 *) connected->address;
			if(p == NULL)
			{
				zlog_err("%-15s[%d]: p is NULL!", __FUNCTION__, __LINE__);
				continue;
			}

            if (p->family != AF_INET)
                continue;

            if (IPV4_ADDR_CMP(&p->prefix, &addr) == 0)
                return 1;
        }
    }

    return 0;
}

static int
memconstant(const void *s, int c, size_t n)
{
    const u_char *p = (u_char *)s;

    while (n-- > 0)
        if (*p++ != c)
            return 0;

    return 1;
}

/* Get prefix in ZServ format; family should be filled in on prefix */
void
ifm_ipc_get_prefix(struct ifm_event *pevent, struct prefix *p)
{
    size_t plen = prefix_blen(p);
    u_char c;
    p->prefixlen = 0;

    if (plen == 0)
        return;

    if (pevent->ipaddr.type == INET_FAMILY_IPV6)
    {
        memcpy(&p->u.prefix, &(pevent->ipaddr.addr.ipv6), plen);
    }
    else
    {
        memcpy(&p->u.prefix, &(pevent->ipaddr.addr.ipv4), plen);
        p->u.prefix4.s_addr = ntohl(p->u.prefix4.s_addr);
    }

    c = pevent->ipaddr.prefixlen;
    p->prefixlen = MIN(plen * 8, c);
}

void
rip_interface_if_set_value(struct ifm_event *pevent, struct interface *ifp)
{

    uint16_t mtu = 0;
    uchar  pmac[6];

    /* Read interface's index. */
    ifp->ifindex = pevent->ifindex;
    /* Read interface's value. */


    /* get the mac */
    if ( ifm_get_mac(pevent->ifindex, MODULE_ID_RIP, pmac) == 0)
    {
        ifp->hw_addr_len = 6;

        if (ifp->hw_addr_len)
        {
            ifp->hw_addr[0] = pmac[0];
            ifp->hw_addr[1] = pmac[1];
            ifp->hw_addr[2] = pmac[2];
            ifp->hw_addr[3] = pmac[3];
            ifp->hw_addr[4] = pmac[4];
            ifp->hw_addr[5] = pmac[5];
        }
    }

    /* get mtu */
    if (ifm_get_mtu(pevent->ifindex, MODULE_ID_RIP, &mtu) == 0)
    {
        ifp->mtu = mtu;
        ifp->mtu6 = mtu;
    }

    ifp->ll_type = IFNET_TYPE_ETHERNET;
}

struct interface *
rip_find_interface(struct vty *vty, uint32_t ifindex)
{
    struct interface *ifp = NULL;
    char if_name[NAME_STRING_LEN];
    vrf_id_t vrf_id = VRF_DEFAULT;
    /*get the name of the interface*/
    ifm_get_name_by_ifindex(ifindex , if_name);
    /*according to the interface name look up the struct pointer ifp */
    ifp = if_get_by_name_len_vrf(if_name, strlen(if_name), vrf_id);

    if (ifp == NULL)
    {
    	if(vty)
    	{
        	vty_error_out(vty, "The specified interface does not exist.%s", VTY_NEWLINE);
    	}
        return NULL;
    }
	ifp->ifindex = ifindex;

    return ifp;
}

struct connected *
rip_ifm_interface_address_read(int type, struct ifm_event *pevent, vrf_id_t vrf_id)
{
    ifindex_t ifindex;
    struct interface *ifp;
    struct connected *ifc = NULL;
    struct listnode *node = NULL;
    struct prefix p, d, *dp;
    int plen;

    memset(&p, 0, sizeof(p));
    memset(&d, 0, sizeof(d));

    if (pevent->ipaddr.type == INET_FAMILY_IPV4 && IPV4_NET0(pevent->ipaddr.addr.ipv4))
    {
        return NULL;;
    }	

    /* Get interface index. */
    ifindex = pevent->ifindex;

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
		/* Lookup this by interface index. */
		ifp = rip_find_interface(NULL, pevent->ifindex);
		
		if (!ifp)
		{
			return NULL;
		}
    }
    else
    {
        ifp = if_lookup_by_index(ifindex);
    }

    if (ifp == NULL)
    {
		//if(IS_RIP_DEBUG_EVENT)
		{
			zlog_debug(RIP_DBG_EVENT, "Can't find interface by ifindex: %02x ", ifindex);
		}
        return NULL;
    }

    /* Fetch flag. */

    /* set ifp link*/
    if (pevent->up_flag == IFNET_LINKUP)
    {
        ifp->status = ZEBRA_INTERFACE_ACTIVE;
        SET_FLAG(ifp->flags, IFF_UP);
        SET_FLAG(ifp->flags, IFF_RUNNING);
    }
    else
    {
        ifp->status = ZEBRA_INTERFACE_SUB;
        SET_FLAG(ifp->flags, 0);
    }

    rip_interface_if_set_value(pevent, ifp);

    /* Fetch interface address. */
    if (pevent->ipaddr.type == INET_FAMILY_IPV4)
    {
        d.family = p.family = AF_INET;
    }
    else if (pevent->ipaddr.type == INET_FAMILY_IPV6)
    {
        d.family = p.family = AF_INET6;
    }

    plen = prefix_blen(&d);

    ifm_ipc_get_prefix(pevent, &p);

    /* N.B. NULL destination pointers are encoded as all zeroes */
    dp = memconstant(&d.u.prefix, 0, plen) ? NULL : &d;

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {

        /* In case of same prefix come, replace it with new one. */
        for (ALL_LIST_ELEMENTS_RO(ifp->connected, node, ifc))
        {
            if (IPV4_ADDR_SAME(&ifc->address->u.prefix4, &p.u.prefix4))
            {
                return ifc;
            }
        }

        /* N.B. NULL destination pointers are encoded as all zeroes */
        ifc = connected_add_by_prefix(ifp, &p, dp);

        if (ifc != NULL)
        {
            if (pevent->ipflag == IP_TYPE_SLAVE)
                ifc->flags = ZEBRA_IFA_SECONDARY;
            else if (pevent->ipflag == IP_TYPE_STATIC || pevent->ipflag == IP_TYPE_DHCP || pevent->ipflag == IP_TYPE_UNNUMBERED)
                ifc->flags = 0;

            if (ifc->destination)
            {
                ifc->destination->prefixlen = ifc->address->prefixlen;
            }
            else if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_PEER))
            {

                /* carp interfaces on OpenBSD with 0.0.0.0/0 as "peer" */
                char buf[PREFIX_STRLEN];
				
				//if(IS_RIP_DEBUG_EVENT)
				{
                	zlog_debug(RIP_DBG_EVENT, "warning: interface %s address %s "
                          "with peer flag set, but no peer address!",
                          ifp->name, prefix2str(ifc->address, buf, sizeof buf));
				}
                UNSET_FLAG(ifc->flags, ZEBRA_IFA_PEER);
            }
        }
    }
    else
    {
    	if(type == ZEBRA_INTERFACE_ADDRESS_DELETE)
    	{
//        assert(type == ZEBRA_INTERFACE_ADDRESS_DELETE);

        	ifc = connected_delete_by_prefix(ifp, &p);
    	}
    }

    return ifc;
}


struct interface *
rip_ifm_interface_state_read(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Get interface name. */
    ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

    /* Lookup this by interface index. */
    ifp = if_lookup_by_name_len(ifname_tmp, strnlen(ifname_tmp, INTERFACE_NAMSIZ));

//   /* If such interface does not exist, indicate an error */
//   if (! ifp)
//   {
//      return NULL;
//   }

    return ifp;
}

int
rip_if_down(struct interface *ifp, int flag)
{
    struct route_node *rp;
    struct rip_info *rinfo;
    struct rip_interface *ri = NULL;
    struct list *list = NULL;
    struct rip_summary *rs = NULL;
    struct listnode *listnode = NULL, *nextnode = NULL;
    struct rip_instance *rip;

    ri = (struct rip_interface *)ifp->info;

    if (!ri || !ri->enable_inter_node)
    {
        return -1;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
        return 0;
	if(flag)
	{
		rip->inter_count--;
	}
	
    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS(list, listnode, nextnode, rinfo))
            {
                if (rip_route_rte(rinfo) && (rinfo->ifindex == ifp->ifindex))
                {
                    rip_ecmp_delete(rinfo, rip);
                }
            }
        }
    }

    rip_update_interface_spec(rip_all_route, ifp, rip, RIP_METRIC_INFINITY);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, RIP_METRIC_INFINITY);

    if (flag && ri->summary)
    {
        for (ALL_LIST_ELEMENTS(ri->summary, listnode, nextnode, rs))
        {
            if (rs && !rs->dflag && rs->tablenode && (list = rs->tablenode->info))
            {
                rip->inter_sum_count--;
                summary_only_route_delete(rs);
                rs->tablenode = NULL;
            }
        }
    }

    if (rip->triggered_update_flag)
    {
        rip->triggered_update_flag = 0;
        rip_event(RIP_TRIGGERED_UPDATE, rip);
    }

    return 0;
}

/* Inteface link down message processing. */
int
rip_interface_down(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    struct rip_interface *ri = NULL;

    /* zebra_interface_state_read() updates interface structure in
       iflist. */
    ifp = rip_ifm_interface_state_read(pevent, vrf_id);

    if (ifp == NULL)
        return 0;


    /* set ifp link*/
    ifp->status = ZEBRA_INTERFACE_SUB;
    ifp->flags = 0;
    UNSET_FLAG(ifp->flags, IFF_UP);
    rip_interface_if_set_value(pevent, ifp);

    ri = (struct rip_interface *)ifp->info;
	if(ri == NULL)
	{
		zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
		return 0;
	}
    if (!(ri->enable_inter_node || ri->ripng_enable))
        return 0;

    rip_if_down(ifp, 0);
    ripng_if_down(ifp, 0);
    ifp->flags = 0;

    //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT) , "interface %s index %02x flags %llx metric %d mtu %d is down",
                   ifp->name, ifp->ifindex, (unsigned long long)ifp->flags,
                   ifp->metric, ifp->mtu);
    }
    return 0;
}


/* Update interface status. */
void
rip_enable_apply(struct interface *ifp, struct rip_instance *rip)
{
    /* Check interface. */
    if (! if_is_operative(ifp))
    {
        return;
    }

    /* any interface MUST have an IPv4 address */
    if (! rip_if_ipv4_address_check(ifp))
    {
        /* Might as well clean up the route table as well
         * rip_if_down sets to 0 ri->running, and displays "turn off %s"
         **/
        rip_if_down(ifp, 0);

        //if (IS_RIP_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "no valid ip address under interface %s", ifp->name);
        }

        rip_connect_set(ifp, 0, rip);
        return;
    }

    /* Add interface wake up thread. */
    rip_connect_set(ifp, 1, rip);

    /* Send RIP request to the interface. */
    rip_request_interface(ifp);
    rip_request_neighbor(ifp, rip);

    /* Send RIP routes to the interface. */
    rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);
    rip_event(RIP_TRIGGERED_UPDATE, rip);
}


/* Inteface link up message processing */
int
rip_interface_up(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    struct rip_interface *ri = NULL;
    struct rip_instance *rip;
    struct ripng_instance *ripng;

    ifp = rip_ifm_interface_state_read(pevent, vrf_id);

    /* zebra_interface_state_read() updates interface structure in
       iflist. */
    if (ifp == NULL)
        return 0;

    /* set ifp link*/
    ifp->status = ZEBRA_INTERFACE_ACTIVE;
    SET_FLAG(ifp->flags, IFF_UP);
    SET_FLAG(ifp->flags, IFF_RUNNING);
    rip_interface_if_set_value(pevent, ifp);

    ri = (struct rip_interface *)ifp->info;
	if(ri == NULL)
	{
		zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
		return 0;
	}

    if (ri->enable_inter_node)
    {
        rip = rip_instance_lookup(ri->enable_inter_node);

        if (rip)
        {
            /* Check if this interface is RIP enabled or not.*/
            rip_enable_apply(ifp, rip);
        }
    }

    if (ri->ripng_enable)
    {
        ripng = ripng_instance_lookup(ri->ripng_enable);

        if (ripng)
        {
            ripng_enable_apply(ifp, ripng);
        }
    }

    //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "interface %s index %02x flags %#llx metric %d mtu %d is up",
                   ifp->name, ifp->ifindex, (unsigned long long) ifp->flags,
                   ifp->metric, ifp->mtu);
    }
    /* Apply distribute list to the all interface. */
    rip_distribute_update_interface(ifp);

    return 0;
}

int
rip_interface_delete(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    struct rip_interface *ri = NULL;
    struct listnode *snode, *nsnode;
    struct rip_summary *rs;

    /* Lookup this by interface index. */
    ifp = rip_ifm_interface_state_read(pevent, vrf_id);
    /* If such interface does not exist, indicate an error */

    if (ifp == NULL)
        return 0;

    rip_if_down(ifp, 1);
    ripng_if_down(ifp, 1);
    ifp->flags = 0;

	//if(IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
	{
   	 zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "interface delete %s index %d flags %#llx metric %d mtu %d",
              	ifp->name, ifp->ifindex, (unsigned long long) ifp->flags, ifp->metric, ifp->mtu);
	}
	
    ri = (struct rip_interface *)ifp->info;
	
	if (ri->summary)
	{
		 for (ALL_LIST_ELEMENTS(ri->summary, snode, nsnode, rs))
		 {
			 if(rs)
			 {
				 RIP_TIMER_OFF(rs->t_sup);
				 RIP_TIMER_OFF(rs->t_sdown);
			 }
		 }
	
		 list_delete(ri->summary);
		 ri->summary = NULL;
	}
	
    if (ri->neighbor)
    {
        list_delete(ri->neighbor);
    }
	ri->neighbor = NULL;
    /* To support pseudo interface do not free interface structure.  */
    if_delete(ifp);
//    ifp->ifindex = IFINDEX_INTERNAL;

    return 0;
}

void
rip_interface_clean(const char id)
{
    struct listnode *node, *snode, *nsnode;
    struct interface *ifp;
    struct rip_interface *ri;
    struct rip_summary *rs;

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        ri = (struct rip_interface *)ifp->info;

        if (ri->enable_inter_node != id)
            continue;

        ifp->metric = RIP_DEFAULT_METRIC_DEFAULT;

        ri->enable_inter_node = 0;
        ri->ri_send = RI_RIP_UNSPEC;
        ri->ri_receive = RI_RIP_UNSPEC;
        ri->auth_type = RIP_NO_AUTH;

        if (ri->auth_str)
        {
            memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);
        }

        if (ri->key_chain)
        {
            free(ri->key_chain);
            ri->key_chain = NULL;
        }

        ri->split_horizon = RIP_SPLIT_HORIZON;
        ri->poison_reverse = RIP_NO_POISON_REVERSE;
        ri->list[RIP_FILTER_IN] = NULL;
        ri->list[RIP_FILTER_OUT] = NULL;
        ri->prefix[RIP_FILTER_IN] = NULL;
        ri->prefix[RIP_FILTER_OUT] = NULL;
        ri->recv_badpackets = 0;
        ri->recv_badroutes = 0;
        ri->sent_updates = 0;
        ri->receive_packet = 0;

	   if (ri->summary)
		{
			for (ALL_LIST_ELEMENTS(ri->summary, snode, nsnode, rs))
			{
				if(rs)
				{
					RIP_TIMER_OFF(rs->t_sup);
					RIP_TIMER_OFF(rs->t_sdown);
				}
			}

			list_delete_all_node(ri->summary);
		}


        if (ri->neighbor)
        {
            list_delete_all_node(ri->neighbor);
        }

        ri->passive = 0;

    }
}

void
rip_interface_reset(void)
{
    struct listnode *node, *snode, *nsnode;
    struct interface *ifp;
    struct rip_interface *ri;
    struct rip_summary *rs;

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        ri = (struct rip_interface *)ifp->info;

        ri->enable_inter_node = 0;

        ri->ri_send = RI_RIP_UNSPEC;
        ri->ri_receive = RI_RIP_UNSPEC;

        ri->auth_type = RIP_NO_AUTH;

        if (ri->auth_str)
        {
            memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);
        }

        if (ri->key_chain)
        {
            free(ri->key_chain);
            ri->key_chain = NULL;
        }

        ri->split_horizon = RIP_SPLIT_HORIZON;
        ri->poison_reverse = RIP_NO_POISON_REVERSE;

        ri->list[RIP_FILTER_IN] = NULL;
        ri->list[RIP_FILTER_OUT] = NULL;

        ri->prefix[RIP_FILTER_IN] = NULL;
        ri->prefix[RIP_FILTER_OUT] = NULL;

        ri->recv_badpackets = 0;
        ri->recv_badroutes = 0;
        ri->sent_updates = 0;
        ri->receive_packet = 0;
		
		if (ri->summary)
		 {
			 for (ALL_LIST_ELEMENTS(ri->summary, snode, nsnode, rs))
			 {
				 if(rs && rs->t_sup)
				 {
					 /*thread_call(rs->t_sup);
					 rs->t_sup = NULL;*/
					 RIP_TIMER_OFF(rs->t_sup);
				 }
				 
				 if(rs && rs->t_sdown)
				 {
					 /*thread_call(rs->t_sdown);
					 rs->t_sdown = NULL;*/
					 RIP_TIMER_OFF(rs->t_sdown);
				 }
			 }
		
			 list_delete_all_node(ri->summary);
//			 ri->summary = NULL;
		 }
		
		
		 if (ri->neighbor)
		 {
			 list_delete_all_node(ri->neighbor);
		 }
//		 ri->neighbor = NULL;
        ri->passive = 0;

        ri->ipv6_passive = 0;
        ri->ripng_enable = 0;
        ri->ipv6_split_horizon = RIP_SPLIT_HORIZON;
        ri->ipv6_poison_reverse = RIP_NO_POISON_REVERSE;
    }
}

int
rip_interface_address_add(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct connected *ifc;
    struct rip_interface *ri;
    struct rip_instance *rip;
    struct ripng_instance *ripng;

    ifc = rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, pevent, vrf_id);

    if (ifc == NULL)
        return 0;

	if(ifc->ifp == NULL)
	{
		zlog_err("%-15s[%d]: ifc->ifp is NULL!", __FUNCTION__, __LINE__);
		return 0;
	}

    ri = (struct rip_interface *)ifc->ifp->info;
	if(ri == NULL)
	{
		zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
		return 0;
	}

	if(ifc->address == NULL)
	{
		zlog_err("%-15s[%d]: ifc->address is NULL!", __FUNCTION__, __LINE__);
		return 0;
	}

    if (ifc->address->family == AF_INET)
    {
        //if (IS_RIP_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "connected IP address %s/%d is added",
                       inet_ntoa(ifc->address->u.prefix4), ifc->address->prefixlen);
        }

        if (ri->enable_inter_node)
        {
            rip = rip_instance_lookup(ri->enable_inter_node);

            if (!rip)
                return -1;

            rip_enable_apply(ifc->ifp, rip);
        }

    }

    if ((ifc->address->family == AF_INET6) && (ri->ripng_enable))
    {
        //if (IS_RIPNG_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "connected IPv6 address %s/%d is added",
                       inet6_ntoa(ifc->address->u.prefix6), ifc->address->prefixlen);
        }
        ripng = ripng_instance_lookup(ri->ripng_enable);

        if (ripng)
        {
            ripng_enable_apply(ifc->ifp, ripng);
        }
    }

#ifdef HAVE_SNMP
			rip_ifaddr_add(ifc->ifp, ifc);
#endif /* HAVE_SNMP */
    return 0;
}


int
rip_route_ifaddress_del(struct connected *ifc, struct rip_instance *rip)
{
    struct route_node *rp;
    struct rip_info *rinfo;
    struct list *list = NULL;
    struct listnode *listnode = NULL, *nextnode = NULL;
    struct prefix nexthop;


    for (rp = route_top(rip->table); rp; rp = route_next(rp))
    {
        if ((list = (struct list *)rp->info) != NULL)
        {
            for (ALL_LIST_ELEMENTS(list, listnode, nextnode, rinfo))
            {
                memset(&nexthop, 0, sizeof(struct prefix));
                nexthop.prefixlen = IPV4_MAX_BITLEN;
                nexthop.u.prefix4 = rinfo->from;
                nexthop.family = ifc->address->family;

                if (rinfo->ifindex == ifc->ifp->ifindex && CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_RTE)
                        && prefix_match(CONNECTED_PREFIX(ifc), &nexthop))
                {
                    rip_ecmp_delete(rinfo, rip);
                }
            }
        }
    }
    return 0;
}

void
rip_apply_address_del(struct connected *ifc, struct rip_instance *rip)
{
    struct prefix_ipv4 address;
    struct prefix *p;
    struct rip_info rinfo;

    if (! if_is_up(ifc->ifp))
        return;

    p = ifc->address;

    memset(&address, 0, sizeof(address));
    address.family = p->family;
    address.prefix = p->u.prefix4;
    address.prefixlen = p->prefixlen;
    apply_mask_ipv4(&address);

    rip_route_ifaddress_del(ifc, rip);

    rinfo.distance = ROUTE_METRIC_RIP;
    rinfo.type = ROUTE_PROTO_RIP;
    SET_FLAG(rinfo.sub_type, RIP_ROUTE_INTERFACE);
    rinfo.id = rip->id;
    rinfo.external_id = 0;
    rinfo.ifindex = ifc->ifp->ifindex;
    rinfo.nexthop = ifc->address->u.prefix4;
    rinfo.metric = 0;

    rip_redistribute_delete(rip, &rinfo, &address);

    if (rip->triggered_update_flag)
    {
        rip->triggered_update_flag = 0;
        rip_event(RIP_TRIGGERED_UPDATE, rip);
    }
}

int
rip_interface_address_delete(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct connected *ifc;
    struct rip_interface *ri;
    struct rip_instance *rip;
    struct ripng_instance *ripng = NULL;

    ifc = rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_DELETE, pevent, vrf_id);

    if (ifc)
    {
    	if(ifc->ifp == NULL)
		{
			zlog_err("%-15s[%d]: ifc->ifp is NULL!", __FUNCTION__, __LINE__);
			return 0;
		}
		
        ri = (struct rip_interface *)ifc->ifp->info;
		if(ri == NULL)
		{
			zlog_err("%-15s[%d]: ri is NULL!", __FUNCTION__, __LINE__);
			return 0;
		}

        if ((ifc->address->family == AF_INET) && ri && (ri->enable_inter_node))
        {
            //if (IS_RIP_DEBUG_EVENT)
                zlog_debug(RIP_DBG_EVENT, "connected IP address %s/%d is deleted",
                           inet_ntoa(ifc->address->u.prefix4), ifc->address->prefixlen);

#ifdef HAVE_SNMP
            rip_ifaddr_delete(ifc->ifp, ifc);
#endif /* HAVE_SNMP */


            /* Chech wether this prefix needs to be removed */
            rip = rip_instance_lookup(ri->enable_inter_node);

            if (rip)
            {
                rip_apply_address_del(ifc, rip);
            }

        }

        if ((ifc->address->family == AF_INET6) && ri && ri->ripng_enable)
        {
            //if (IS_RIPNG_DEBUG_EVENT)
                zlog_debug(RIPNG_DBG_EVENT, "connected IPv6 address %s/%d is deleted",
                           inet6_ntoa(ifc->address->u.prefix6), ifc->address->prefixlen);

            ripng = ripng_instance_lookup(ri->ripng_enable);

            if (ripng)
            {
                ripng_apply_address_del(ifc, ripng);
            }

        }

        connected_free(ifc);

    }

    return 0;
}

void
rip_connect_set(struct interface *ifp, int set, struct rip_instance *rip)
{
    struct listnode *node, *nnode;
    struct connected *connected;
    struct prefix_ipv4 address;
    struct rip_info rinfo;

    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, connected))
    {
        struct prefix *p;
        p = connected->address;

        if (p->family != AF_INET)
            continue;

        address.family = AF_INET;
        address.prefix = p->u.prefix4;
        address.prefixlen = p->prefixlen;
        apply_mask_ipv4(&address);

        memset(&rinfo, 0, sizeof(struct rip_info));
        rinfo.distance = rip->distance;
        rinfo.type = ROUTE_PROTO_RIP;
        SET_FLAG(rinfo.sub_type, RIP_ROUTE_INTERFACE);
        rinfo.id = rip->id;
        rinfo.external_id = 0;
        rinfo.ifindex = ifp->ifindex;
        rinfo.nexthop = connected->address->u.prefix4;
        rinfo.metric = 0;

        if (set)
        {
            /* Check once more wether this prefix is within a "network IF_OR_PREF" one */
            rip_redistribute_add(rip, &rinfo, &address);
        }
        else
        {
            rip_redistribute_delete(rip, &rinfo, &address);
        }

        if (rip->triggered_update_flag)
        {
            rip->triggered_update_flag = 0;
            rip_event(RIP_TRIGGERED_UPDATE, rip);
        }
    }
}


/* Add new RIP neighbor to the neighbor tree. */
int
rip_neighbor_add(struct interface *ifp, struct prefix *p)
{
    struct rip_interface *ri = (struct rip_interface *)ifp->info;
    struct listnode *node = NULL;
    struct prefix *lp = NULL;

    if (listcount(ri->neighbor))
    {
        for (node = listhead(ri->neighbor); node; node = listnextnode(node))
        {
            lp = (struct prefix *)listgetdata(node);

            if (prefix_same(lp, p))
            {
                return -1;
            }
        }
    }

	do
	{
    	lp = (struct prefix *)XMALLOC(MTYPE_PREFIX, sizeof(struct prefix));
		if (lp == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(lp == NULL);

	memset(lp, 0, sizeof(struct prefix));
	
    memcpy(lp, p, sizeof(struct prefix));
    listnode_add(ri->neighbor, lp);

    return 0;
}

/* Delete RIP neighbor from the neighbor tree. */
int
rip_neighbor_delete(struct interface *ifp, struct prefix *p)
{
    struct rip_interface *ri = (struct rip_interface *)ifp->info;
    struct listnode *node;
    struct prefix *lp = NULL;

    if (listcount(ri->neighbor))
    {
        /* look for sepcified neighbor and delete it. */
        for (node = listhead(ri->neighbor); node; node = listnextnode(node))
        {
            lp = (struct prefix *)listgetdata(node);

            if (prefix_same(lp, p))
            {
                listnode_delete(ri->neighbor, lp);
                rip_neighbor_free(lp);
				lp = NULL;
                return 0;
            }
        }
    }

    return -1;
}

int
rs_up_to_null(void *t)
{
    struct rip_summary *rs;

    rs = (struct rip_summary *)(t);
    if (!rs)
        return -1;

    rs->t_sup = 0;
    return 0;
}

int rs_down_to_null(void *t)
{
    struct rip_summary *rs;
    struct rip_interface *ri;

    rs = (struct rip_summary *)(t);

    if (NULL == rs)
        return 0;

    rs->t_sdown = 0;
    rs->uflag = 0;

    /* delete listnode from summary */
    if (rs->tablenode)
    {
        summary_only_route_delete(rs);
    }

    /* delete listnode from summary */
    if (rs->dflag)
    {
        ri = rs->ri;
        rs->tablenode = NULL;
        listnode_delete(ri->summary, rs);
        rip_summary_free(rs);
		rs = NULL;
    }

    return 0;
}

struct rip_info *
rip_add_new(const struct prefix *p, struct rip_instance *rip, int flag)
{
    struct route_node *rp = NULL;
    struct list *list = NULL;
    struct rip_info *summary_info = NULL;


    rp = route_node_get(rip->table, p);

    /* Make aggregate address structure. */
    summary_info = rip_info_new();
    summary_info->type = ROUTE_PROTO_NUM + 1;
    SET_FLAG(summary_info->sub_type, RIP_ROUTE_SUMMARY);
    summary_info->id = rip->id;
    summary_info->metric_out = RIP_METRIC_INFINITY;
    summary_info->mmt_count = 0;
    summary_info->ifindex = 0;
    summary_info->ifindex_out = 0xffffffff;
    summary_info->external_id = 0;

    if (flag)
    {
        summary_info->summary_count = 0;
        SET_FLAG(summary_info->summary_flag, RIP_SUMMARY_AUTO);
    }
    else
    {
        summary_info->summary_count++;
        SET_FLAG(summary_info->summary_flag, RIP_SUMMARY_ADRESS);
    }

//  summary_info->distance = rip_distance_apply(summary_info, rip);

    summary_info->distance = rip->distance;
    summary_info->rp = rp;
    //summary_info->t_timeout = NULL;
    //summary_info->t_garbage_collect = NULL;
	summary_info->t_timeout = 0;
    summary_info->t_garbage_collect = 0;

    if (!rp->info)
    {
        rp->info = list_new();
        list = (struct list *)rp->info;
        list->cmp = (int (*)(void *, void *)) rip_ecmp_route_cmp;
    }
	else
	{
        route_unlock_node (rp);
	}

    list = (struct list *)rp->info;
    listnode_add_sort(list, summary_info);

    return summary_info;

}
/* summary set flag and return the min metric and ifindex
paramater:
    sumtype : 1 : summary   2:summary address;
    opflag : 1: add or reset   0:delete
*/
void
rip_summary_subprefix_flagset(struct route_node *rp,
                              int sumtype, int opflag, u_int32_t *ifmt)
{
    struct rip_info *rinfo = NULL;
    struct route_node *node = NULL;
    struct list *list = NULL;
    struct listnode *nnode = NULL;

    u_int32_t metric = RIP_METRIC_INFINITY;
    u_int32_t mmt_count = 0;

	if(rp == NULL)
	{
		zlog_err("%s %d rp is NULL",__func__,__LINE__);
		return ;
	}

    route_lock_node(rp);

    for (node = rp; node; node = route_next(node))
    {
        if (node && (node->info != NULL))
        {
            if (prefix_match(&rp->p, &node->p))
            {

                list = (struct list *)node->info;

                for (ALL_LIST_ELEMENTS_RO(list, nnode, rinfo))
                {
                    if (rinfo->metric >= RIP_METRIC_INFINITY || CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
                        continue;

                    if (1 == sumtype)
                    {
                        if (opflag)
                        {
                            if (rinfo->metric == metric)
                            {
                                mmt_count++;
                            }
                            else if (rinfo->metric < metric)
                            {
                                mmt_count = 1;
                                metric = rinfo->metric;
                            }
                        }
                    }

                    if (2 == sumtype)
                    {
                        if (opflag)
                        {
                            if (rinfo->metric == metric)
                            {
                                mmt_count++;
                            }
                            else if (rinfo->metric < metric)
                            {
                                mmt_count = 1;
                                metric = rinfo->metric;
                            }
                        }
                    }
                }
            }
            else
            {
				route_unlock_node(node);
                break;
            }
        }
    }

    if (ifmt)
    {
        ifmt[0] = metric;
        ifmt[1] = mmt_count;


        //if (IS_RIP_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "get summary metric %d mmt_count %d", metric, mmt_count);
        }
    }
}

static void
rip_route_summary_unset(struct rip_summary *rs, struct rip_instance *rip)
{
    struct route_node *lp = NULL;
    struct rip_info *rinfo = NULL;
    struct list *list = NULL;


    lp = rs->tablenode;

    if (lp && ((list = (struct list *)lp->info) != NULL))
    {
        rinfo = (struct rip_info *)listgetdata(listhead(list));

        if (!CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY) && CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS))
        {
            return;
        }

        RS_TIMER_OFF(rs->t_sup);
        RS_TIMER_OFF(rs->t_sdown);
        RS_TIMER_ON(rs->t_sdown, rs_down_to_null, rip->garbage_time);

//    /* If routes exists below this node, generate aggregate routes. */
//    rip_summary_subprefix_flagset(lp, 2, 0, NULL);
    }

}

static int
rip_route_summary_set(struct rip_summary *rs, struct rip_instance *rip)
{
    struct route_node *lp;
    struct rip_info *lrinfo = NULL;
    struct list *list = NULL;
    u_int32_t ifmt [2];

    rs->uflag = 1;
    lp = rs->tablenode;

    if (lp && ((list = (struct list *)lp->info) != NULL))
    {
        lrinfo = (struct rip_info *)listgetdata(listhead(list));

        rip_summary_subprefix_flagset(lp, 2, 1, ifmt);

        if (lrinfo->metric != ifmt[0])
        {
            SET_FLAG(lrinfo->flags, RIP_RTF_CHANGED);
        }

        lrinfo->metric = ifmt[0];
        lrinfo->metric_out = ifmt[0];
        lrinfo->mmt_count = ifmt[1];
    }

    if (ifmt[1] != 17)
    {

        RS_TIMER_OFF(rs->t_sdown);
        RS_TIMER_OFF(rs->t_sup);
        RS_TIMER_ON(rs->t_sup, rs_up_to_null, rip->garbage_time);
        return 1;
    }

    return 0;
}
/* Lookup the summary node as given prefix. */
struct listnode *
ri_summary_prefix_lookup(struct list *list, struct prefix *p)
{
    struct listnode *node = NULL;
    struct rip_summary *rs = NULL;

    if (!list)
        return NULL;

    for (ALL_LIST_ELEMENTS_RO(list, node, rs))
    {
        if (prefix_same(p, &rs->p))
            return node;
    }

    return NULL;
}

/* Lookup the summary node contained by the prefix. */
static struct listnode *
ri_summary_prefix_contain_lookup(struct list *list, struct prefix *p)
{
    struct listnode *node = NULL;
    struct rip_summary *rs = NULL;

    if (listcount(list) < 2)
        return NULL;

    for (ALL_LIST_ELEMENTS_RO(list, node, rs))
    {
        if (prefix_match(p, &rs->p) && !prefix_same(p, &rs->p) && !rs->dflag)
        {
            return node;
        }
    }

    return NULL;
}

/* Lookup the node which contain the given prefix. */
static struct listnode *
ri_summary_delete_prefix_match_lookup(const struct list *list, const struct listnode *node, const struct prefix *p)
{
    struct listnode *pnode = NULL;
    struct listnode *hnode = NULL;
    struct rip_summary *prs = NULL;

    if (listcount(list) < 2)
        return NULL;

	pnode = listnextnode(node);
    while (pnode)
    {
        prs = NULL;
        prs = (struct rip_summary *)listgetdata(pnode);

        if (prefix_match(&prs->p, p) && !prefix_same(p, &prs->p))
        {
            if (!prs->dflag)
            {
                return pnode;
            }
        }
        else
        {
            break;
        }
		
        pnode = listnextnode(pnode);
    }

    hnode = listhead(list);
	pnode = node->prev;
    while (pnode)
    {
        prs = NULL;
        prs = (struct rip_summary *)listgetdata(pnode);

        if (prefix_match(&prs->p, p) && !prefix_same(p, &prs->p))
        {
            if (!prs->dflag)
            {
                return pnode;
            }
        }
		if(pnode == hnode)
		{
			return NULL;
		}
        pnode = pnode->prev;
    }

    return NULL;
}

static int
add_summary_to_rip_table(struct rip_summary *rs, struct interface *ifp, struct rip_instance *rip)
{
    struct listnode *nnode;
    struct rip_interface *ri = rs->ri;
    struct rip_summary *frs = NULL;
    struct route_node *lp;
    struct list *list = NULL;
    struct rip_info *rinfo = NULL;

    if (!rs->tablenode)
    {
        lp = route_node_lookup(rip->table, &rs->p);

        if (lp && ((list = (struct list *)lp->info) != NULL))
        {
            rinfo = (struct rip_info *)listgetdata(listhead(list));

            if (!CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY))
            {
                rinfo = rip_add_new(&rs->p, rip, 0);
            }
            else
            {
                rinfo->summary_count++;
                SET_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS);
            }
            rs->tablenode = lp;
        }
        else
        {
            rinfo = rip_add_new(&rs->p, rip, 0);
            SET_FLAG(rinfo->flags, RIP_RTF_CHANGED);
            rs->tablenode = rinfo->rp;
        }
		
		if(lp)
            route_unlock_node(lp);
    }
    else
    {
        if ((list = rs->tablenode->info))
        {
            rinfo = listgetdata(listhead(list));
            rinfo->summary_count++;
        }
        else
        {
			//if (IS_RIP_DEBUG_EVENT)
			{
            	zlog_debug(RIP_DBG_EVENT, "summary table node has no entry");
			}
        }
    }

    rip->inter_sum_count++;

    if ((nnode = ri_summary_prefix_contain_lookup(ri->summary, &rs->p)) != NULL)
    {
        return 0;
    }

    if ((nnode = ri_summary_prefix_match_lookup(ri->summary, &rs->p)) != NULL)
    {
        frs = (struct rip_summary *)listgetdata(nnode);
        frs->uflag = 0;
        rip_route_summary_unset(frs, rip);

        //if (IS_RIP_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "interface %s unset summary	%s/%d", ifp->name,
                       inet_ntoa(frs->p.u.prefix4), frs->p.prefixlen);
        }
    }

    rip_route_summary_set(rs, rip);

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "interface %s set summary  %s/%d for output", ifp->name,
                   inet_ntoa(rs->p.u.prefix4), rs->p.prefixlen);
    }

    rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);

    return 0;
}

static int
add_summary_to_interface(struct interface *ifp, struct prefix *p)
{
    struct rip_interface *ri = (struct rip_interface *)ifp->info;
    struct rip_summary *rs = NULL;
    struct rip_instance *rip = NULL;
    struct listnode *node = NULL;

    node = ri_summary_prefix_lookup(ri->summary, p);

    if (node)
    {
        rs = (struct rip_summary *)listgetdata(node);

        if (rs->dflag)
        {
            rs->dflag = 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        rs = rip_summary_new();
        prefix_copy(&rs->p, p);
        rs->dflag = 0;
        rs->ri = ri;
        rs->tablenode = NULL;
        rs->uflag = 0;
        listnode_add_sort(ri->summary, rs);
    }

    if (0 == ri->enable_inter_node)
    {
        return 0;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return 0;
    }

    add_summary_to_rip_table(rs, ifp, rip);
    return 0;

}

static int
delete_summary_without_enable(struct interface *ifp, struct prefix *p)
{
    struct listnode *node;
    struct rip_summary *rs;
    struct rip_interface *ri = ifp->info;

    node = ri_summary_prefix_lookup(ri->summary, p);

    if (!node)
        return -1;

    rs = listgetdata(node);

    if (rs->dflag)
        return -1;

    rs->dflag = 1;

    if (rs->t_sdown)
        return 0;

	RIP_TIMER_OFF(rs->t_sup);

    if (rs->tablenode)
    {
        zlog_err("%s  %d rotue table node still existed without no command", __func__, __LINE__);
        return -1;
    }

    listnode_delete(ri->summary, rs);
    rip_summary_free(rs);
	rs = NULL;
    return 0;

}


static int
delete_summary_from_interface(struct interface *ifp, struct prefix *p, struct rip_instance *rip)
{

    struct listnode *node, *nnode;
    struct rip_summary *rs;
    struct rip_interface *ri = (struct rip_interface *)ifp->info;
//  struct list *list = NULL;
    struct rip_summary *nrs = NULL;
//  struct rip_info *rinfo = NULL;

    node = ri_summary_prefix_lookup(ri->summary, p);

    if (!node)
        return -1;

    rs = (struct rip_summary *)listgetdata(node);

    if (rs->dflag)
        return -1;

    rs->dflag = 1;

    if (rs->t_sdown)
        return 0;

    rip->inter_sum_count--;
//  if(rs->tablenode && (list = rs->tablenode->info))
//  {
//      rinfo = listgetdata(listhead(list));
//      rinfo->summary_count--;
//  }

    if ((nnode = ri_summary_prefix_contain_lookup(ri->summary, p)) != NULL)
    {
        if (rs->tablenode)
        {
            summary_only_route_delete(rs);
            rs->tablenode = NULL;
        }

        listnode_delete(ri->summary, rs);
        rip_summary_free(rs);
		rs = NULL;
        return 0;
    }

    rip_route_summary_unset(rs, rip);

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "interface %s unset summary	%s/%d", ifp->name,
                   inet_ntoa(rs->p.u.prefix4), rs->p.prefixlen);
    }

    if ((nnode = ri_summary_delete_prefix_match_lookup(ri->summary, node, p)) != NULL)
    {
        nrs = (struct rip_summary *)listgetdata(nnode);
        rs->uflag = 0;
        rip_route_summary_set(nrs, rip);

        //if (IS_RIP_DEBUG_EVENT)
        {
            zlog_debug(RIP_DBG_EVENT, "interface %s set summary  %s/%d for output", ifp->name,
                       inet_ntoa(nrs->p.u.prefix4), nrs->p.prefixlen);
        }
    }

    if (rip->summaryflag)
    {
        if (rip->t_summary_unset)
        {
            //thread_cancel(rip->t_summary_unset);
            //rip->t_summary_unset = NULL;
            RIP_TIMER_OFF(rip->t_summary_unset);
        }

		if(0 ==  rip->t_summary_set)
		{
        	//rip->t_summary_set = thread_add_timer(master_rip, rip_summary_on, rip, rip->garbage_time);
			rip->t_summary_set = high_pre_timer_add ((char *)"rip_sum_timer",\
					LIB_TIMER_TYPE_NOLOOP, rip_summary_on, rip, (rip->garbage_time)*1000);
		}
    }

    rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);

    return 0;
}

static void
rip_if_mode_change_delete_route(struct interface *ifp, struct rip_instance *rip,
                                struct ripng_instance *ripng)
{
    struct listnode *nn;
    struct connected *connected;	
    struct ifm_event pevent;
    vrf_id_t vrf_id = VRF_DEFAULT;

    for (ALL_LIST_ELEMENTS_RO(ifp->connected, nn, connected))
    {
        struct prefix *p;

        p = connected->address;

        if (p->family == AF_INET && rip)
        {
            rip_apply_address_del(connected, rip);
			
			pevent.ipaddr.type = INET_FAMILY_IPV4;
			pevent.ipaddr.addr.ipv4 = connected->address->u.prefix4.s_addr;
			pevent.ipaddr.prefixlen = connected->address->prefixlen;
			pevent.ipflag = IP_TYPE_STATIC;
			rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_DELETE, &pevent, vrf_id);
        }
        else if (p->family == AF_INET6 && ripng)
        {
            ripng_apply_address_del(connected, ripng);
			
			pevent.ipaddr.type = INET_FAMILY_IPV6;
			IPV6_ADDR_COPY(&pevent.ipaddr.addr.ipv6, &connected->address->u.prefix6.s6_addr);
			pevent.ipaddr.prefixlen = connected->address->prefixlen;
			pevent.ipflag = IP_TYPE_STATIC;
			rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_DELETE, &pevent, vrf_id);
        }

    }

}

int
rip_interface_change_mode(struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    struct rip_interface *ri;
	struct list *list;
    struct listnode *node, *nnode;
    struct rip_summary *rs;
    struct rip_instance *rip = NULL;
    struct ripng_instance *ripng = NULL;
	struct rip_info *rinfo;

    /* Lookup this by interface index. */
    ifp = rip_find_interface(NULL, pevent->ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;
    ri->mode = pevent->mode;


    if (pevent->mode != IFNET_MODE_L3)
    {

        ifp->metric = 0;

		if (ri->enable_inter_node)
        {
            rip = rip_instance_lookup(ri->enable_inter_node);
			rip->inter_count--;
        }

        if (ri->ripng_enable)
        {
            ripng = ripng_instance_lookup(ri->ripng_enable);
			ripng->ifcount--;
        }

        rip_if_mode_change_delete_route(ifp, rip, ripng);

        ri->enable_inter_node = 0;
        memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);
        ri->auth_type = RIP_NO_AUTH;
        ri->md5_auth_len = RIP_AUTH_MD5_COMPAT_SIZE;
        ri->md5_auth_id = 0;
        /* Set default split-horizon behavior.  If the interface is Frame
           Relay or SMDS is enabled, the default value for split-horizon is
           off.  But currently Zebra does detect Frame Relay or SMDS
           interface.  So all interface is set to split horizon.  */
        ri->split_horizon = RIP_SPLIT_HORIZON;
        ri->poison_reverse = RIP_NO_POISON_REVERSE;
        ri->ri_send = RI_RIP_UNSPEC;
        ri->ri_receive = RI_RIP_UNSPEC;
		ri->passive = 0;

        ri->ripng_enable = 0;
        ri->ipv6_passive = 0;
        ri->ipv6_poison_reverse = RIP_NO_POISON_REVERSE;
        ri->ipv6_split_horizon = RIP_SPLIT_HORIZON;

        if (ri->summary)
        {
            for (ALL_LIST_ELEMENTS(ri->summary, node, nnode, rs))
            {
                if (rs && rs->tablenode)
                {
                	list = (struct list *)rs->tablenode->info;
					rinfo = (struct rip_info *)listgetdata(listhead(list));					
					rinfo->summary_count--;
					
					if (CHECK_FLAG(rinfo->sub_type, RIP_ROUTE_SUMMARY) 
							&& CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_ADRESS)
							&& !CHECK_FLAG(rinfo->summary_flag, RIP_SUMMARY_AUTO))
					{
						if(rinfo->summary_count == 0)
						{							
							RIP_TIMER_OFF(rinfo->t_timeout);
							RIP_TIMER_OFF(rinfo->t_garbage_collect);
							
							/* Unlock route_node. */
							listnode_delete(list, rinfo);
							if (list_isempty(list))
							{
								list_free(list);
								rs->tablenode->info = NULL;
								route_unlock_node(rs->tablenode);
							}
							/* Free RIP routing information. */
							rip_info_free(rinfo);		
							rinfo = NULL;
						}
					}
					
                }

				if(rs)
				{
					RIP_TIMER_OFF(rs->t_sup);
					RIP_TIMER_OFF(rs->t_sdown);
				}
            }

            list_delete_all_node(ri->summary);
        }

        if (ri->neighbor)
        {
            list_delete_all_node(ri->neighbor);
        }
    }

	ifp->vpn = pevent->vpn;
	
    //if (IS_RIP_DEBUG_EVENT || IS_RIPNG_DEBUG_EVENT)
        zlog_debug((RIP_DBG_EVENT|RIPNG_DBG_EVENT), "change interface %s index %02x to mode %d vpn to %d",
                   ifp->name, ifp->ifindex, ri->mode, ifp->vpn);

    return 0;
}

void
rip_inter_if_set_value(uint32_t ifindex, struct interface *ifp, vrf_id_t vrf_id)
{
    int ret = 0;
    struct ifm_l3 l3 ;
    uint8_t pdown_flag = 0;
    struct ifm_event pevent;


    if (ifm_get_link(ifindex, MODULE_ID_RIP, &pdown_flag) == 0)
    {
        if (pdown_flag == IFNET_LINKUP)
        {
            ifp->status = ZEBRA_INTERFACE_ACTIVE;
            SET_FLAG(ifp->flags, IFF_UP);
            SET_FLAG(ifp->flags, IFF_RUNNING);
        }
        else
        {
            ifp->status = ZEBRA_INTERFACE_SUB;
            SET_FLAG(ifp->flags, 0);
        }
    }
    else
    {
        ifp->status = ZEBRA_INTERFACE_SUB;
        SET_FLAG(ifp->flags, 0);
    }


    ret = ifm_get_l3if(ifindex, MODULE_ID_RIP, &l3);
    
	if(ret != 0)
 	{
 		zlog_err("%-15s[%d]: RIP get ifindex %0x l3 time out", __FUNCTION__, __LINE__, ifindex);
 	}

    if (ret == 0 && (l3.ipv4[0].addr || l3.ipv6[0].addr))
    {

        pevent.ifindex = ifindex;

        if (CHECK_FLAG(ifp->flags, IFF_UP))
        {
            pevent.up_flag = 0;
        }
        else
        {
            pevent.up_flag = 1;
        }

        if (l3.ipv4_flag)
        {
            pevent.ipaddr.type = INET_FAMILY_IPV4;
            pevent.ipaddr.addr.ipv4 = l3.ipv4[0].addr;
            pevent.ipaddr.prefixlen = l3.ipv4[0].prefixlen;
            pevent.ipflag = l3.ipv4_flag;
            rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, &pevent, vrf_id);

            if (l3.ipv4[1].addr)
            {
                pevent.ipaddr.addr.ipv4 = l3.ipv4[1].addr;
                pevent.ipaddr.prefixlen = l3.ipv4[1].prefixlen;
                pevent.ipflag = IP_TYPE_SLAVE;
                rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, &pevent, vrf_id);
            }
        }

        if (l3.ipv6_flag)
        {
            pevent.ipaddr.type = INET_FAMILY_IPV6;
            IPV6_ADDR_COPY(&pevent.ipaddr.addr.ipv6, &l3.ipv6[0].addr);
            pevent.ipaddr.prefixlen = l3.ipv6[0].prefixlen;
            pevent.ipflag = l3.ipv6_flag;
            rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, &pevent, vrf_id);

            if(0 != l3.ipv6[1].prefixlen)
            {
				IPV6_ADDR_COPY(&pevent.ipaddr.addr.ipv6, &l3.ipv6[1].addr);
                pevent.ipaddr.prefixlen = l3.ipv6[1].prefixlen;
                pevent.ipflag = IP_TYPE_SLAVE;
                rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, &pevent, vrf_id);
            }
			
            if (l3.ipv6_link_local.addr[0])
            {
				IPV6_ADDR_COPY(&pevent.ipaddr.addr.ipv6, &l3.ipv6_link_local.addr);
                pevent.ipaddr.prefixlen = l3.ipv6_link_local.prefixlen;
                pevent.ipflag = IP_TYPE_SLAVE;
                rip_ifm_interface_address_read(ZEBRA_INTERFACE_ADDRESS_ADD, &pevent, vrf_id);
            }

        }

    }

}

DEFUN(ip_rip_enable,
      ip_rip_enable_cmd,
      "rip enable instance <1-255>",
      RIP_STR
      "Enable RIP on the interface\n"
      "specified an instance\n"
      "Instance number\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct rip_instance *rip;
    struct interface *ifp;
    vrf_id_t vrf_id = VRF_DEFAULT;
    struct rip_interface *ri;
    struct rip_summary *rs = NULL;
    struct listnode *node, *nnode;
    unsigned char id;
    id = (unsigned char)atoi(argv[0]);

    VTY_CHECK_NM_INTERFACE(ifindex);

    /* Lookup/create interface by name. */
    ifp = rip_find_interface(vty, ifindex);
    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

	if (ri == NULL)
    {
        vty_error_out(vty, "The specified ri is NULL!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->enable_inter_node != 0)
    {
        vty_error_out(vty, "The specified interface has been enabled by RIP %d%s", ri->enable_inter_node, VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    /* check rip instance */
    rip = rip_instance_get(NULL, id);

    if (!rip)
    {
        VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    rip->inter_count++;

    ri->enable_inter_node = id;
    rip_inter_if_set_value(ifindex, ifp, vrf_id);

    if (ri->summary)
    {
        for (ALL_LIST_ELEMENTS(ri->summary, node, nnode, rs))
        {
            if (rs && !rs->dflag)
            {
                add_summary_to_rip_table(rs, ifp, rip);
            }
        }
    }

    rip_enable_apply(ifp, rip);
    return CMD_SUCCESS;

}

DEFUN(no_ip_rip_enable,
      no_ip_rip_enable_cmd,
      "no rip enable instance",
      NO_STR
      RIP_STR
      "Enable RIP on the interface\n"
      "specified an instance\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct interface *ifp;
    struct rip_interface *ri;

    VTY_CHECK_NM_INTERFACE(ifindex);

    /*according to the interface name look up the struct pointer ifp */
    ifp = rip_find_interface(vty, ifindex);
    if (ifp == NULL)
    {
        return CMD_WARNING;
    }
	ifp->ifindex = ifindex;

    ri = (struct rip_interface *)ifp->info;

	if (ri == NULL)
    {
        vty_error_out(vty, "The specified ri is NULL!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->enable_inter_node == 0)
    {
        vty_error_out(vty, "RIP already disabled under this interface %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    rip_if_down(ifp, 1);

    //if (IS_RIP_DEBUG_EVENT)
    {
        zlog_debug(RIP_DBG_EVENT, "disable interface %s", ifp->name);
    }

    ri->enable_inter_node = 0;

    return CMD_SUCCESS;
}

/* RIP neighbor configuration set. */
DEFUN(rip_neighbor,
      rip_neighbor_cmd,
      "rip neighbor A.B.C.D ",
      RIP_STR
      "Set a neighbor router\n"
      "IP address <neighbor>, e.g., 35.0.0.1\n")
{
    int ret;
    struct prefix p;
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ret = str2prefix(argv[0], &p);

    if (ret <= 0)
    {
        vty_error_out(vty, " Please specify IP address by A.B.C.D/M%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* check input ip validation */
    ret = inet_valid_host(ntohl(p.u.prefix4.s_addr));

    if (IPV4_NET0(ntohl(p.u.prefix4.s_addr)) || ret == FALSE)
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p.prefixlen = 32;
    p.family = AF_INET;

    ret = rip_ip_address_self_check(&(p.u.prefix4));

    if (ret < 0)
    {
        vty_error_out(vty, "The local address cannot be configured as a peer address.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = rip_neighbor_add(ifp, &p);

    if (ret < 0)
    {
        vty_error_out(vty, "The specified peer already exists. %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

/* RIP neighbor configuration unset. */
DEFUN(no_rip_neighbor,
      no_rip_neighbor_cmd,
      "no rip neighbor A.B.C.D ",
      NO_STR
      RIP_STR
      "Set a neighbor router\n"
      "IP address <neighbor>, e.g., 35.0.0.1\n")
{
    int ret;
    struct prefix p;
    struct interface *ifp;
    struct rip_interface *ri = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ret = str2prefix(argv[0], &p);

    if (ret <= 0)
    {
        vty_error_out(vty, "Please specify IP address by A.B.C.D%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    /* check input ip validation */
    ret = inet_valid_network(ntohl(p.u.prefix4.s_addr));

    if (ret == FALSE)
    {

        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    p.prefixlen = 32;
    p.family = AF_INET;
    rip_neighbor_delete(ifp, &p);

    if (ret < 0)
    {
        vty_error_out(vty, "The specified peer does not exist under this interface.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

#if 0
DEFUN(ip_rip_receive_version,
      ip_rip_receive_version_cmd,
      "ip rip receive version (1|2)",
      IP_STR
      "Routing Information Protocol\n"
      "Advertisement reception\n"
      "Version control\n"
      "RIP version 1\n"
      "RIP version 2\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = vty->index;

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    /* Version 1. */
    if (atoi(argv[0]) == 1)
    {
        ri->ri_receive = RI_RIP_VERSION_1;
        return CMD_SUCCESS;
    }

    if (atoi(argv[0]) == 2)
    {
        ri->ri_receive = RI_RIP_VERSION_2;
        return CMD_SUCCESS;
    }

    return CMD_WARNING;
}

DEFUN(ip_rip_receive_version_1,
      ip_rip_receive_version_1_cmd,
      "ip rip receive version 1 2",
      IP_STR
      "Routing Information Protocol\n"
      "Advertisement reception\n"
      "Version control\n"
      "RIP version 1\n"
      "RIP version 2\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    ifp = (struct interface *)vty->index;
    ri = ifp->info;

    /* Version 1 and 2. */
    ri->ri_receive = RI_RIP_VERSION_1_AND_2;

    return CMD_SUCCESS;
}

DEFUN(ip_rip_receive_version_2,
      ip_rip_receive_version_2_cmd,
      "ip rip receive version 2 1",
      IP_STR
      "Routing Information Protocol\n"
      "Advertisement reception\n"
      "Version control\n"
      "RIP version 2\n"
      "RIP version 1\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    /* Version 1 and 2. */
    ri->ri_receive = RI_RIP_VERSION_1_AND_2;

    return CMD_SUCCESS;
}

DEFUN(no_ip_rip_receive_version,
      no_ip_rip_receive_version_cmd,
      "no ip rip receive version",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Advertisement reception\n"
      "Version control\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    ri->ri_receive = RI_RIP_UNSPEC;
    return CMD_SUCCESS;
}

ALIAS(no_ip_rip_receive_version,
      no_ip_rip_receive_version_num_cmd,
      "no ip rip receive version (1|2)",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Advertisement reception\n"
      "Version control\n"
      "Version 1\n"
      "Version 2\n")
#endif
DEFUN(ip_rip_send_version,
      ip_rip_send_version_1_cmd,
      "rip version (1|2)",
      RIP_STR
      "Set RIP version\n"
      "RIP version 1 on interface\n"
      "RIP version 2 on interface\n")
{
    struct interface *ifp = NULL;
    struct rip_interface *ri = NULL;
    struct rip_instance *rip = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int vsend = 0;
    int priver = 0;
	int ret;

    VTY_CHECK_NM_INTERFACE(ifindex);

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    priver = ri->ri_send;

    /* Version 1 or 2. */
    if (atoi(argv[0]) == 1)
    {
        ri->ri_send = RI_RIP_VERSION_1;
    }

    if (atoi(argv[0]) == 2)
    {
        ri->ri_send = RI_RIP_VERSION_2;
    }

    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    vsend = ((priver == RI_RIP_UNSPEC) ? rip->version_recv: priver);

    if (vsend == RI_RIP_VERSION_2_MULTI)
    {
        vsend = RI_RIP_VERSION_2;
    }

    if (vsend != ri->ri_send)
    {
        /* Send RIP request to the interface. */
		ret = rip_version_change_table_check(rip, priver, ri->ri_send, ifindex);
		if(ret == 2)
		{
			rip_update_interface_spec(rip_all_route, ifp, rip, RIP_METRIC_INFINITY);
			rip_update_neighbor_spec(ifp, rip_all_route, rip, RIP_METRIC_INFINITY);
		}
		rip_request_interface(ifp);
		rip_request_neighbor(ifp, rip);
    }

    return CMD_WARNING;
}

DEFUN(ip_rip_send_version_2,
      ip_rip_send_version_2_cmd,
      "rip version 2 (broadcast|multicast)",
      RIP_STR
      "Set RIP version\n"
      "RIP version 2 on interface\n"
      "RIP version 2 broadcast mode\n"
      "RIP version 2 multicast mode\n")
{
    struct interface *ifp = NULL;
    struct rip_interface *ri = NULL;
    struct rip_instance *rip = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    int vsend = 0;
    int priver = 0;
	int ret;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    priver = ri->ri_send;

    if (argv[0] && !strncmp(argv[0], "b", 1))
    {
        ri->ri_send = RI_RIP_VERSION_2_BROAD;
    }
    else
    {
        ri->ri_send = RI_RIP_VERSION_2_MULTI;
    }

    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    /* Version 2 broadcast or multicast. */
    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    vsend = ((priver == RI_RIP_UNSPEC) ? rip->version_recv: priver);

    if (vsend == RI_RIP_VERSION_2)
    {
        vsend = RI_RIP_VERSION_2_MULTI;
    }

    if (vsend != ri->ri_send)
    {
        /* Send RIP request to the interface. */
		ret = rip_version_change_table_check(rip, priver, ri->ri_send, ifindex);
		if(ret == 2)
		{
			rip_update_interface_spec(rip_all_route, ifp, rip, RIP_METRIC_INFINITY);
			rip_update_neighbor_spec(ifp, rip_all_route, rip, RIP_METRIC_INFINITY);
		}
		rip_request_interface(ifp);
		rip_request_neighbor(ifp, rip);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ip_rip_send_version,
      no_ip_rip_send_version_cmd,
      "no rip version",
      NO_STR
      RIP_STR
      "Set RIP version\n")
{
    struct interface *ifp = NULL;
    struct rip_interface *ri = NULL;
    struct rip_instance *rip = NULL;
    int ret = 0;
    int vsend = 0;
    int priver = 0;
    uint32_t ifindex = (uint32_t)vty->index;


    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ri_send == RI_RIP_UNSPEC)
    {
        return CMD_WARNING;
    }

    priver = ri->ri_send;
    ri->ri_send = RI_RIP_UNSPEC;

    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    vsend = (priver == RI_RIP_VERSION_2_MULTI) ? RI_RIP_VERSION_2 : priver;

    if (vsend != rip->version_recv)
    {
        /* Send RIP request to the interface. */
		ret = rip_version_change_table_check(rip, priver, ri->ri_send, ifindex);
		if(ret == 2)
		{
			rip_update_interface_spec(rip_all_route, ifp, rip, RIP_METRIC_INFINITY);
			rip_update_neighbor_spec(ifp, rip_all_route, rip, RIP_METRIC_INFINITY);
		}
		rip_request_interface(ifp);
		rip_request_neighbor(ifp, rip);
    }

    return CMD_SUCCESS;
}

ALIAS(no_ip_rip_send_version,
      no_ip_rip_send_version_num_cmd,
      "no rip version (1|2)",
      NO_STR
      RIP_STR
      "Specify RIP version\n"
      "RIP version 1 on interface\n"
      "RIP version 2 on interface\n")


DEFUN(rip_default_metric,
      rip_default_metric_cmd,
      "rip metric <0-15>",
      RIP_STR
      "Set metric to inbound and outbound routes\n"
      "The value of metric adding to route")
{
    struct interface *ifp;
    int ret;
    struct rip_instance *rip;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    /* check rip instance */
    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ifp->metric = atoi(argv[0]);
    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);
    if (!rip)
    {
        return CMD_WARNING;
    }

    ret = rip_update_interface_spec(rip_all_route, ifp, rip, 0);
	rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);
    if (ret)
        return CMD_WARNING;

    return CMD_SUCCESS;
}

DEFUN(no_rip_default_metric,
      no_rip_default_metric_cmd,
      "no rip metric",
      NO_STR
      RIP_STR
      "Set metric to inbound and outbound routes\n")
{
    struct interface *ifp;
    int ret;
    struct rip_instance *rip;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    /* check rip instance */
    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ifp->metric = RIP_DEFAULT_METRIC_DEFAULT;


    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    ret = rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);

    if (ret)
        return CMD_WARNING;

    return CMD_SUCCESS;
}

DEFUN(route_summary,
      route_summary_cmd,
      "rip route-summary A.B.C.D/M",
      RIP_STR
      "Configure summary route on this interface\n"
      "summary prefix\n")
{
    int ret;
    struct prefix p, pp;
    struct rip_interface *ri = NULL;
    struct interface *ifp;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ret = str2prefix(argv[0], &p);

    if (!ret)
    {
        vty_error_out(vty, "Malformed prefix.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&pp, &p, sizeof(struct prefix));
    apply_mask(&p);

    ret = inet_valid_network(ntohl(p.u.prefix4.s_addr));

    if (ret == FALSE)
    {
        vty_error_out(vty, "Malformed network.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!prefix_same(&p, &pp))
    {
        vty_error_out(vty, "Used modified summary prefix %s/%d%s", inet_ntoa(p.u.prefix4), p.prefixlen, VTY_NEWLINE);
    }

    ret = add_summary_to_interface(ifp, &p);

    if (ret)
    {
        vty_error_out(vty, "Configuration already existed%s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    return CMD_SUCCESS;
}

DEFUN(no_route_summary,
      no_route_summary_cmd,
      "no rip route-summary A.B.C.D/M",
      NO_STR
      RIP_STR
      "Configure summary route on this interface\n"
      "summary prefix\n")
{
    struct prefix p, pp;
    struct interface *ifp;
    struct rip_interface *ri = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    struct rip_instance *rip = NULL;
    int ret;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    /* Convert string to prefix structure. */
    ret = str2prefix(argv[0], &p);

    if (!ret)
    {
        vty_error_out(vty, "Malformed prefix.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&pp, &p, sizeof(struct prefix));
    apply_mask(&p);

    ret = inet_valid_network(ntohl(p.u.prefix4.s_addr));

    if (ret == FALSE)
    {
        vty_error_out(vty, "Malformed network.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!prefix_same(&p, &pp))
    {
        vty_info_out(vty, "used modified summary prefix %s/%d%s", inet_ntoa(p.u.prefix4), p.prefixlen, VTY_NEWLINE);
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->enable_inter_node)
    {
        rip = rip_instance_lookup(ri->enable_inter_node);

        if (!rip)
        {
            vty_error_out(vty, "can`t find rip instance %d.%s", ri->enable_inter_node, VTY_NEWLINE);
            return CMD_WARNING;
        }

        ret = delete_summary_from_interface(ifp, &p, rip);

        if (ret < 0)
        {
            vty_error_out(vty, "no such configuration%s", VTY_NEWLINE);
            return CMD_SUCCESS;
        }
    }
    else
    {
        delete_summary_without_enable(ifp, &p);
    }

    return CMD_SUCCESS;
}

DEFUN(ip_rip_authentication_mode,
      ip_rip_authentication_mode_cmd,
      "rip auth PASSWORD {md5 <1-255>}",
      RIP_STR
      "Authentication"
      "Authentication password, length <1-16>\n"
      "MD5 authentication mode\n"
      "MD5 authentication key-id\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
// int auth_type;
    struct rip_instance *rip;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (argc != 2)
    {
        vty_error_out(vty, "incorrect argument count%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strlen(argv[0]) > 16)
    {
        vty_error_out(vty, "RIPv2 authentication string must be <= 16%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->key_chain)
    {
        vty_error_out(vty, "key-chain configuration exists%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[1] == NULL)
    {
        ri->auth_type = RIP_AUTH_SIMPLE_PASSWORD;
    }
    else
    {
        ri->auth_type = RIP_AUTH_MD5;
        ri->md5_auth_id = (char)atoi(argv[1]);
    }

    memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);
    strncpy(ri->auth_str, argv[0], RIP_AUTH_MD5_COMPAT_SIZE);


    /* check rip instance */
    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);

    return CMD_SUCCESS;

}
DEFUN(no_ip_rip_authentication_mode,
      no_ip_rip_authentication_mode_cmd,
      "no rip auth",
      NO_STR
      RIP_STR
      "Authentication control\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    struct rip_instance *rip;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->auth_type == RIP_NO_AUTH)
    {
        return CMD_WARNING;
    }

    ri->auth_type = RIP_NO_AUTH;
    ri->md5_auth_len = RIP_AUTH_MD5_COMPAT_SIZE;
    ri->md5_auth_id = 0;
    memset(ri->auth_str, 0, RIP_AUTH_MD5_COMPAT_SIZE + 1);


    /* check rip instance */
    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    rip = rip_instance_lookup(ri->enable_inter_node);

    if (!rip)
    {
        return CMD_WARNING;
    }

    rip_update_interface_spec(rip_all_route, ifp, rip, 0);
    rip_update_neighbor_spec(ifp, rip_all_route, rip, 0);
    return CMD_SUCCESS;
}

#if 0
ALIAS(no_ip_rip_authentication_mode,
      no_ip_rip_authentication_mode_type_cmd,
      "no ip rip authentication mode (md5|text)",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication mode\n"
      "Keyed message digest\n"
      "Clear text authentication\n")

ALIAS(no_ip_rip_authentication_mode,
      no_ip_rip_authentication_mode_type_authlen_cmd,
      "no ip rip authentication mode (md5|text) auth-length (rfc|old-ripd)",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication mode\n"
      "Keyed message digest\n"
      "Clear text authentication\n"
      "MD5 authentication data length\n"
      "RFC compatible\n"
      "Old ripd compatible\n")

DEFUN(ip_rip_authentication_string,
      ip_rip_authentication_string_cmd,
      "ip rip authentication string LINE",
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication string\n"
      "Authentication string\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    if (strlen(argv[0]) > 16)
    {
        vty_out(vty, "%% RIPv2 authentication string must be shorter than 16%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->key_chain)
    {
        vty_out(vty, "%% key-chain configuration exists%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->auth_str)
        free(ri->auth_str);

    ri->auth_str = strdup(argv[0]);

    return CMD_SUCCESS;
}

DEFUN(no_ip_rip_authentication_string,
      no_ip_rip_authentication_string_cmd,
      "no ip rip authentication string",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication string\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    if (ri->auth_str)
        free(ri->auth_str);

    ri->auth_str = NULL;

    return CMD_SUCCESS;
}

ALIAS(no_ip_rip_authentication_string,
      no_ip_rip_authentication_string2_cmd,
      "no ip rip authentication string LINE",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication string\n"
      "Authentication string\n")

DEFUN(ip_rip_authentication_key_chain,
      ip_rip_authentication_key_chain_cmd,
      "ip rip authentication key-chain LINE",
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication key-chain\n"
      "name of key-chain\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->auth_str)
    {
        vty_out(vty, "%% authentication string configuration exists%s",
                VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (ri->key_chain)
        free(ri->key_chain);

    ri->key_chain = strdup(argv[0]);

    return CMD_SUCCESS;
}

DEFUN(no_ip_rip_authentication_key_chain,
      no_ip_rip_authentication_key_chain_cmd,
      "no ip rip authentication key-chain",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication key-chain\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->key_chain)
        free(ri->key_chain);

    ri->key_chain = NULL;

    return CMD_SUCCESS;
}

ALIAS(no_ip_rip_authentication_key_chain,
      no_ip_rip_authentication_key_chain2_cmd,
      "no ip rip authentication key-chain LINE",
      NO_STR
      IP_STR
      "Routing Information Protocol\n"
      "Authentication control\n"
      "Authentication key-chain\n"
      "name of key-chain\n")

/* CHANGED: ip rip split-horizon
   Cisco and Zebra's command is
   ip split-horizon
 */
#endif

DEFUN(ip_rip_split_horizon,
      ip_rip_split_horizon_cmd,
      "rip split-horizon",
      RIP_STR
      "Perform split horizon\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->split_horizon == RIP_SPLIT_HORIZON)
    {
        return CMD_SUCCESS;
    }

    ri->split_horizon = RIP_SPLIT_HORIZON;

    return CMD_SUCCESS;
}

DEFUN(ip_rip_split_horizon_poisoned_reverse,
      ip_rip_split_horizon_poisoned_reverse_cmd,
      "rip poisoned-reverse",
      RIP_STR
      "Perform poisoned-reverse\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
    {
        return CMD_SUCCESS;
    }

    ri->poison_reverse = RIP_SPLIT_HORIZON_POISONED_REVERSE;

    return CMD_SUCCESS;
}

/* CHANGED: no ip rip split-horizon
   Cisco and Zebra's command is
   no ip split-horizon
 */
DEFUN(no_ip_rip_split_horizon,
      no_ip_rip_split_horizon_cmd,
      "no rip split-horizon",
      NO_STR
      RIP_STR
      "Perform split horizon\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->split_horizon == RIP_NO_SPLIT_HORIZON)
    {
        return CMD_SUCCESS;
    }

    ri->split_horizon = RIP_NO_SPLIT_HORIZON;

    return CMD_SUCCESS;
}

DEFUN(no_ip_rip_split_horizon_poisoned_reverse,
      no_ip_rip_split_horizon_poisoned_reverse_cmd,
      "no rip poisoned-reverse",
      NO_STR
      RIP_STR
      "Perform poisoned-reverse\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->poison_reverse == RIP_NO_POISON_REVERSE)
    {
        return CMD_SUCCESS;
    }

    ri->poison_reverse = RIP_NO_POISON_REVERSE;

    return CMD_SUCCESS;
}

DEFUN(rip_passive_interface,
      rip_passive_interface_cmd,
      "rip passive",
      RIP_STR
      "Suppress routing updates on this interface\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;


    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ri->passive = 1;

    //if (IS_RIP_DEBUG_EVENT)
        zlog_debug(RIP_DBG_EVENT, "interface %s: passive = %d", ifp->name, ri->passive);

    return CMD_SUCCESS;
}

DEFUN(no_rip_passive_interface,
      no_rip_passive_interface_cmd,
      "no rip passive",
      NO_STR
      RIP_STR
      "Suppress routing updates on this interface\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ri->passive = 0;

    //if (IS_RIP_DEBUG_EVENT)
        zlog_debug(RIP_DBG_EVENT, "interface %s: passive = %d", ifp->name, ri->passive);

    return CMD_SUCCESS;
}

void
rip_authentication_type(struct rip_interface *ri, char *type)
{
    switch (ri->auth_type)
    {
        case 0:
            strcpy(type, "no authentication");
            break;
        case 1:
            strcpy(type, "data authentication");
            break;
        case 2:
            strcpy(type, "simple password authentication");
            break;
        case 3:
            strcpy(type, "MD5 authentication");
            break;
        default:
            strcpy(type, "wrong type");
            break;
    }
}

void 
show_rip_interface_brief(struct vty *vty, struct interface *ifp, struct rip_instance *rip)
{
	struct rip_interface *ri;
	struct connected *connected;
	struct listnode *cnode, *cnnode;
	char name[INTERFACE_NAMSIZ];
	int vflag, len = 0;


	ri = (struct rip_interface *)ifp->info;
	memset(name, 0, INTERFACE_NAMSIZ);
	get_interface_for_short(ifp->ifindex, name);

	vty_out(vty, "%-13s%-7s", name, (if_is_operative(ifp) ? "up" : "down"));
	
    for (ALL_LIST_ELEMENTS(ifp->connected, cnode, cnnode, connected))
    {
        if (connected->address->family != AF_INET)
            continue;

        /* Show interface information. */
        len += vty_out(vty, "%-s  ", inet_ntoa(connected->address->u.prefix4));
    }

	len = 36 - len;

	if (len)
	{
		vty_out(vty, "%*s", len, " ");
	}

	vflag = 0;
	if (ri->ri_send == RI_RIP_UNSPEC)
	{
		if(rip->version_send == RI_RIP_VERSION_1)
		{
			if(rip->version_recv == RI_RIP_VERSION_1_AND_2)
			{
				vflag = 1;
			}			
        	vty_out(vty, "RIPv1 %s%s", vflag ? "Compatible" : " ", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "RIPv2 Multicast%s", VTY_NEWLINE);
		}
	}
	else
	{
		if(ri->ri_send == RI_RIP_VERSION_1)
		{
			vty_out(vty, "RIPv1 %s", VTY_NEWLINE);
		}
		else if(ri->ri_send == RI_RIP_VERSION_2_BROAD)
		{
			vty_out(vty, "RIPv2 Broadcast%s", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "RIPv2 Multicast%s", VTY_NEWLINE);
		}
	}

}

void
show_ip_rip_interface_sub(struct vty *vty, struct interface *ifp, struct rip_instance *rip)
{
    int is_up, vflag = 0;
    struct connected *connected;
    struct rip_interface *ri = NULL;
    char type[64];
    struct listnode *cnode, *cnnode;
    struct rip_summary *rs;

    memset(type, 0, sizeof(type));
    vty_out(vty, "%s is %s%s", ifp->name,
            ((is_up = if_is_operative(ifp)) ? "up" : "down"), VTY_NEWLINE);
    vty_out(vty, "  ifindex %02x, MTU %u bytes, BW %u Kbit %s%s",
            ifp->ifindex, ifp->mtu, ifp->bandwidth, if_flag_dump(ifp->flags),
            VTY_NEWLINE);

    ri = (struct rip_interface *)ifp->info;

    for (ALL_LIST_ELEMENTS(ifp->connected, cnode, cnnode, connected))
    {

        if (connected->address->family != AF_INET)
            continue;

        /* Show OSPF interface information. */
        vty_out(vty, "  Internet Address:    %s/%d%s",
                inet_ntoa(connected->address->u.prefix4), connected->address->prefixlen, VTY_NEWLINE);

        if (connected->destination)
        {
            const char *dstr;

            if (!listnode_lookup(ri->neighbor, connected->destination))
                dstr = "Peer";
            else
                dstr = "Broadcast";

            vty_out(vty, " %s %s,", dstr, inet_ntoa(connected->destination->u.prefix4));
        }

    }

    /* Is interface RIP enabled? */
    if (!ri->enable_inter_node)
    {
        vty_out(vty, "  RIP not enabled on this interface%s", VTY_NEWLINE);
        return;
    }
    else if (!is_up)
    {
        vty_out(vty, "  RIP is enabled, but this interface is down%s",
                VTY_NEWLINE);
        return;
    }

    vty_out(vty, "  passive interface:   %s%s",
            ri->passive ? "enabled" : "disabled", VTY_NEWLINE);
    vty_out(vty, "  rip metric: %d%s", ifp->metric, VTY_NEWLINE);

	vflag = 0;
	if (ri->ri_send == RI_RIP_UNSPEC)
	{
		if(rip->version_send == RI_RIP_VERSION_1)
		{
			if(rip->version_recv == RI_RIP_VERSION_1_AND_2)
			{
				vflag = 1;
			}			
        	vty_out(vty, "  Protocol: RIPv1 %s%s", vflag ? "Compatible (Non-Standard)" : " ", VTY_NEWLINE);
			vty_out(vty, "  send:%s%s  receive:%s%s", "RIPv1 Packets", VTY_NEWLINE,
				vflag ? "RIPv1 Packets, RIPv2 Multicast and Broadcast Packets" : "RIPv1 Packets", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "  Protocol: RIPv2 Multicast%s", VTY_NEWLINE);
			vty_out(vty, "  send:%s%s  receive:%s%s", "RIPv2 Packets", VTY_NEWLINE,
				"RIPv2 Multicast and Broadcast Packets", VTY_NEWLINE);
		}
	}
	else
	{
		if(ri->ri_send == RI_RIP_VERSION_1)
		{
			vty_out(vty, "  Protocol: RIPv1%s", VTY_NEWLINE);
			vty_out(vty, "  send:%s%s  receive:%s%s", "RIPv1 Packets",VTY_NEWLINE, "RIPv1 Packets", VTY_NEWLINE);
		}
		else if(ri->ri_send == RI_RIP_VERSION_2_BROAD)
		{
			vty_out(vty, "  Protocol: RIPv2 Broadcast%s", VTY_NEWLINE);
			vty_out(vty, "  send:%s%s  receive:%s%s", "RIPv2 Packets", VTY_NEWLINE,
				"RIPv1 Packets, RIPv2 Multicast and Broadcast Packets", VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "  Protocol: RIPv2 Multicast%s", VTY_NEWLINE);
			vty_out(vty, "  send:%s%s  receive:%s%s", "RIPv2 Packets", VTY_NEWLINE,
				"RIPv2 Multicast and Broadcast Packets", VTY_NEWLINE);
		}
	}

    vty_out(vty, "  split horizon:    %s%s",
            (ri->split_horizon == 1) ? "enabled" : "disabled", VTY_NEWLINE);
    vty_out(vty, "  poison reverse:    %s%s",
            (ri->poison_reverse == 2) ? "enabled" : "disabled", VTY_NEWLINE);
    rip_authentication_type(ri, type);
    vty_out(vty, "  authentication type:    %s%s", type, VTY_NEWLINE);

    /* RIP route summary configuration. */
    if (listcount(ri->summary))
        for (ALL_LIST_ELEMENTS_RO(ri->summary, cnode, rs))
        {
            if (!rs->dflag)
                vty_out(vty, " rip route-summary %s/%d%s",
                        inet_ntoa(rs->p.u.prefix4), rs->p.prefixlen, VTY_NEWLINE);
        }

    vty_out(vty, "  received bad packets:    %d%s", ri->recv_badpackets, VTY_NEWLINE);
    vty_out(vty, "  received bad routes:    %d%s", ri->recv_badroutes, VTY_NEWLINE);
    vty_out(vty, "  send updates:    %d%s", ri->sent_updates, VTY_NEWLINE);
    vty_out(vty, "  receive right packets:    %d%s", ri->receive_packet, VTY_NEWLINE);

}


static int
show_rip_interface_get_parameter(struct vty *vty, unsigned char id, const char *ifname_string, 
												const char *ifname_count, const char *detail_flag)
{
	struct interface *ifp = NULL;
	struct rip_instance *rip = NULL;
	uint32_t ifindex = 0;
	char ifname_tmp[INTERFACE_NAMSIZ];
	struct rip_interface *ri = NULL;

	if(!id)
	{
		//if(IS_RIP_DEBUG_EVENT)
		{
			zlog_err("get wrong instacne id: 0 ");
		}
		return CMD_WARNING; 
	}
	rip = rip_instance_lookup(id);
	if (! rip)
	{
		VTY_RIP_INSTANCE_ERR
	}

	if (ifname_string == NULL && ifname_count == NULL)
	{
		vty_error_out(vty, "can`t read the configuration %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	else
	{
		ifindex = ifm_get_ifindex_by_name(ifname_string, ifname_count);

		if (ifindex == 0)
		{
			vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
			return CMD_WARNING;
		}

		/*get the name of the interface*/
		ifm_get_name_by_ifindex(ifindex , ifname_tmp);

		if ((ifp = if_lookup_by_name(ifname_tmp)) == NULL)
		{
			vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		else
		{

			ri = (struct rip_interface *)ifp->info;

			if (!ri)
			{
				return CMD_WARNING;
			}

			if (ri->enable_inter_node == id)
			{
				if(detail_flag && strncmp(detail_flag, "d", 1) == 0)
				{
					show_ip_rip_interface_sub(vty, ifp, rip);
				}
				else
				{
					/* show interface version and keychain */
					vty_out(vty, "Interface    State  IP Address(main slave)              Protocol%s", VTY_NEWLINE);
					show_rip_interface_brief(vty, ifp, rip);
				}
			}
			else
			{
				if (ri->enable_inter_node == 0)
					vty_error_out(vty, "This interface didn`t enable rip protocol %s", VTY_NEWLINE);
				else
					vty_error_out(vty, "This interface belongs to rip instance %d %s", ri->enable_inter_node, VTY_NEWLINE);
			}
		}
	}

	return CMD_WARNING;
}

DEFUN(show_rip_interface,
      show_rip_interface_cmd,
      "show rip <1-255> interface (ethernet|gigabitethernet|xgigabitethernet) USP [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "The port/subport format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n"
      "show all rip information\n")
{	
    unsigned char id;
	
    id = (u_char)atoi(argv[0]);
	show_rip_interface_get_parameter(vty, id, argv[1], argv[2], argv[3]);
    return CMD_SUCCESS;
}

DEFUN (show_rip_interface_vlan,
       show_rip_interface_vlan_cmd,
	   "show rip <1-255> interface vlanif <1-4094> [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "Vlan interface\n"
	   "VLAN interface number\n"
	   "show all rip information\n")
{	
	unsigned char id;
	
	id = (u_char)atoi(argv[0]);
	show_rip_interface_get_parameter(vty, id, "vlanif", argv[1], argv[2]);
	return CMD_SUCCESS;
}


DEFUN (show_rip_interface_trunk,
       show_rip_interface_trunk_cmd,
	   "show rip <1-255> interface trunk TRUNK [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "Trunk interface\n"
	   "The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
	   "show all rip information\n")
{	
	unsigned char id;
	
	id = (u_char)atoi(argv[0]);
	show_rip_interface_get_parameter(vty, id, "trunk", argv[1], argv[2]);
	return CMD_SUCCESS;
}

DEFUN (show_rip_interface_loopback,
       show_rip_interface_loopback_cmd,
	   "show rip <1-255> interface loopback <0-128> [detail]",
	   SHOW_STR
	   RIP_STR
	   "rip instance number\n"
	   CLI_INTERFACE_STR
	   "LoopBack interface\n"
	   "LoopBack interface number\n"
	   "show all rip information\n")
{	
	unsigned char id;
	
	id = (u_char)atoi(argv[0]);
	show_rip_interface_get_parameter(vty, id, "loopback", argv[1], argv[2]);
	return CMD_SUCCESS;
}

DEFUN(show_rip_interface_all,
      show_rip_interface_all_cmd,
      "show rip <1-255> interface [detail]",
      SHOW_STR
      RIP_STR
      "rip instance number\n"
      CLI_INTERFACE_STR)
{
    struct interface *ifp = NULL;
    struct listnode *node = NULL;
    struct rip_instance *rip = NULL;
    unsigned char id;
    struct rip_interface *ri = NULL;
	int pflag = 0;

    id = (u_char)atoi(argv[0]);
    rip = rip_instance_lookup(id);

    if (! rip)
    {
        VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

	if(argv[1] && strncmp(argv[1], "d", 1) == 0)
	{
		pflag = 1;
	}
	else
	{
		vty_out(vty, "Interface    State  IP Address(main slave)              Protocol%s", VTY_NEWLINE);
	}
	
    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        if (!ifp)
            continue;

        ri = (struct rip_interface *)ifp->info;

        if (ri && ri->enable_inter_node == id)
        {
			if(pflag)
			{
				show_ip_rip_interface_sub(vty, ifp, rip);
			}
			else
			{
				show_rip_interface_brief(vty, ifp, rip);
			}
        }
    }

    return CMD_SUCCESS;
}

/* Write rip configuration of each interface. */
int
rip_interface_config_write(struct vty *vty)
{
    struct listnode *node, *snode;
    struct interface *ifp;
    struct rip_interface *ri;
    struct rip_summary *rs;
    struct prefix *p;

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        vty_out(vty, "interface %s%s", ifp->name,
                VTY_NEWLINE);

        if (ifp->desc)
            vty_out(vty, " description %s%s", ifp->desc,
                    VTY_NEWLINE);

        ri = (struct rip_interface *)ifp->info;

        /* Do not display the interface if there is no
         * configuration about it.
         **/

        /* enable under interface */
        if (ri->ripng_enable)
        {
            vty_out(vty, " ripng enable instance %d%s", ri->ripng_enable, VTY_NEWLINE);
        }
        /* interface metric */
        if (ri->ipv6_metric)
        {
            vty_out(vty, " ripng metric %d%s", ri->ipv6_metric, VTY_NEWLINE);
        }
        /* RIP passive interface listing. */
        if (ri->ipv6_passive)
        {
            vty_out(vty, " ripng passive%s", VTY_NEWLINE);
        }
        /* Split horizon. */
        if (ri->ipv6_split_horizon == RIP_NO_SPLIT_HORIZON)
        {
            vty_out(vty, " no ripng split-horizon%s", VTY_NEWLINE);
        }
        /* poison reverse. */
        if (ri->ipv6_poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
        {
            vty_out(vty, " ripng poisoned-reverse%s", VTY_NEWLINE);
        }
#if 0
        if ((!ifp->desc)                                     &&
                (!ifp->metric)                                   &&
                (!ri->enable_inter_node)                         &&
                (ri->split_horizon == RIP_SPLIT_HORIZON)         &&
                (ri->poison_reverse == RIP_NO_POISON_REVERSE)     &&
                (ri->ri_send == RI_RIP_UNSPEC)                   &&
                (ri->ri_receive == RI_RIP_UNSPEC)                &&
                (ri->auth_type != RIP_AUTH_MD5)                  &&
                (ri->auth_type != RIP_AUTH_SIMPLE_PASSWORD)      &&
                (ri->md5_auth_len != RIP_AUTH_MD5_SIZE)          &&
                (!ri->summary)                                   &&
                (!ri->passive)                                   &&
                (!ri->auth_str)                                  &&
                (!ri->key_chain))
            continue;
#endif
        /* enable under interface */
        if (ri->enable_inter_node)
        {
            vty_out(vty, " rip enable instance %d%s", ri->enable_inter_node, VTY_NEWLINE);
        }

        /* RIP passive interface listing. */
        if (ri->passive)
        {
            vty_out(vty, " rip passive%s", VTY_NEWLINE);
        }

        /* interface neighbor configuration. */
        if (ri->neighbor && listcount(ri->neighbor))
        {
            for (ALL_LIST_ELEMENTS_RO(ri->neighbor, snode, p))
            {
                vty_out(vty, " rip neighbor %s%s",
                        inet_ntoa(p->u.prefix4), VTY_NEWLINE);
            }
        }

        /* Split horizon. */
        if (ri->split_horizon == RIP_NO_SPLIT_HORIZON)
        {
            vty_out(vty, " no rip split-horizon%s", VTY_NEWLINE);
        }

        /* poison reverse. */
        if (ri->poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
        {
            vty_out(vty, " rip poisoned-reverse%s", VTY_NEWLINE);
        }


        /* RIP route summary configuration. */
        if (ri->summary && listcount(ri->summary))
        {
            for (ALL_LIST_ELEMENTS_RO(ri->summary, snode, rs))
            {
                if (!rs->dflag)
                    vty_out(vty, " rip route-summary %s/%d%s",
                            inet_ntoa(rs->p.u.prefix4), rs->p.prefixlen, VTY_NEWLINE);
            }
        }
		
        if (ifp->metric)
            vty_out(vty, " rip metric %d%s", ifp->metric, VTY_NEWLINE);


        /* RIP version setting. */
        if (ri->ri_send != RI_RIP_UNSPEC)
            vty_out(vty, " rip version %s%s",
                    lookup(ri_version_msg, ri->ri_send),
                    VTY_NEWLINE);

 //       if (ri->ri_receive != RI_RIP_UNSPEC)
 //           vty_out(vty, " rip version %s%s", lookup(ri_version_msg, ri->ri_receive), VTY_NEWLINE);

        /* RIP authentication. */
        if (ri->auth_type == RIP_AUTH_SIMPLE_PASSWORD)
        {
            vty_out(vty, " rip auth %s%s", ri->auth_str, VTY_NEWLINE);
        }

        if (ri->auth_type == RIP_AUTH_MD5)
        {
            vty_out(vty, " rip auth %s md5 %d%s", ri->auth_str, ri->md5_auth_id, VTY_NEWLINE);
        }

 //       if (ri->key_chain)
//            vty_out(vty, " ip rip authentication key-chain %s%s",
//                    ri->key_chain, VTY_NEWLINE);

        vty_out(vty, "!%s", VTY_NEWLINE);

    }

    return 0;
}


/* How many link local IPv6 address could be used on the interface ? */
int
ripng_if_ipv6_lladdress_check(struct interface *ifp)
{
    struct listnode *nn;
    struct connected *connected;
    int count = 0;

    for (ALL_LIST_ELEMENTS_RO(ifp->connected, nn, connected))
    {
        struct prefix *p;
        p = connected->address;
      if (p && (p->family == AF_INET6) &&
          !IN6_IS_ADDR_LINKLOCAL (&p->u.prefix6))
        {
            count++;
        }
    }

    return count;
}

void
ripng_connect_set(struct interface *ifp, int set, struct ripng_instance *ripng)
{
    struct listnode *node, *nnode;
    struct connected *connected;
    struct prefix_ipv6 address;
    struct ripng_info rinfo;

    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, connected))
    {
        struct prefix *p;
        p = connected->address;

        if (!p || p->family != AF_INET6 || IN6_IS_ADDR_LINKLOCAL(&p->u.prefix6))
            continue;

        address.family = AF_INET6;
        address.prefix = p->u.prefix6;
        address.prefixlen = p->prefixlen;
        IPV6_ADDR_COPY(&address.prefix, &connected->address->u.prefix6);
        apply_mask_ipv6(&address);

        memset(&rinfo, 0, sizeof(struct ripng_info));
        rinfo.id = ripng->id;
        rinfo.external_id = 0;
        rinfo.ifindex = connected->ifp->ifindex;
        rinfo.type = ROUTE_PROTO_RIPNG;
        rinfo.sub_type = RIPNG_ROUTE_INTERFACE;
		rinfo.distance = ripng->distance;

        if (set)
        {
            ripng_redistribute_add(&rinfo, &address, ripng);
        }
        else
        {
            ripng_redistribute_delete(&rinfo, &address, ripng);
        }
    }
}

/* flag : ifp->count change 1 delete; 0, do nothing */
int
ripng_if_down(struct interface *ifp, int flag)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct list *list = NULL;
    struct listnode *listnode = NULL, *nextnode = NULL;
    struct rip_interface *ri = (struct rip_interface *)ifp->info;
    struct ripng_instance *ripng = NULL;

    if (!ri || !ri->ripng_enable)
    {
        return -1;
    }

    ripng = ripng_instance_lookup(ri->ripng_enable);

    if (ripng)
    {
    	if(flag)
    	{
    		ripng->ifcount--;
    	}
        for (rp = route_top(ripng->table); rp; rp = route_next(rp))
        {
            if ((list = (struct list *)rp->info) != NULL)
            {
                for (ALL_LIST_ELEMENTS(list, listnode, nextnode, rinfo))
                {
                    if (rinfo->ifindex == ifp->ifindex)
                    {
                        ripng_ecmp_delete(rinfo, ripng);
                    }
                }
            }
        }
    }

    return 0;
}

int
ripng_route_ifaddress_del(struct connected *ifc, struct ripng_instance *ripng)
{
    struct route_node *rp;
    struct ripng_info *rinfo;
    struct list *list = NULL;
    struct listnode *listnode = NULL, *nextnode = NULL;
    struct prefix nexthop;

    for (rp = route_top(ripng->table); rp; rp = route_next(rp))
        if ((list = (struct list *)rp->info) != NULL)
            for (ALL_LIST_ELEMENTS(list, listnode, nextnode, rinfo))
            {
                memset(&nexthop, 0, sizeof(struct prefix));
                nexthop.prefixlen = IPV6_MAX_BITLEN;
                IPV6_ADDR_COPY(&nexthop.u.prefix6, &rinfo->from);
                nexthop.family = ifc->address->family;

                if (rinfo->ifindex == ifc->ifp->ifindex && CHECK_FLAG(rinfo->sub_type, RIPNG_ROUTE_RTE)
                        && prefix_match(CONNECTED_PREFIX(ifc), &nexthop))
                {
                    ripng_ecmp_delete(rinfo, ripng);
                }
            }

    return 0;
}

void
ripng_apply_address_del(struct connected *ifc, struct ripng_instance *ripng)
{
    struct prefix_ipv6 address;
    struct prefix *p;
    struct ripng_info rinfo;

    if (! if_is_up(ifc->ifp))
        return;

    p = ifc->address;

    memset(&address, 0, sizeof(struct prefix_ipv6));
    address.family = p->family;
    address.prefix = p->u.prefix6;
    address.prefixlen = p->prefixlen;
    IPV6_ADDR_COPY(&address.prefix, &ifc->address->u.prefix6);
    apply_mask_ipv6(&address);

    ripng_route_ifaddress_del(ifc, ripng);


    rinfo.type = ROUTE_PROTO_RIPNG;
    rinfo.sub_type = RIPNG_ROUTE_INTERFACE;
    rinfo.ifindex = ifc->ifp->ifindex;
    rinfo.id = ripng->id;
    IPV6_ADDR_COPY(&rinfo.nexthop, &ifc->address->u.prefix6);
    rinfo.metric = 0;

    ripng_redistribute_delete(&rinfo, &address, ripng);
    ripng_event(RIPNG_TRIGGERED_UPDATE, ripng);
}

/* Check RIPng is enabed on this interface. */
void
ripng_enable_apply(struct interface *ifp, struct ripng_instance *ripng)
{
    struct rip_interface *ri;
    ri = (struct rip_interface *)ifp->info;
	struct connected *ifc;
    struct listnode *node, *nnode;

    /* Check interface. */
    if (! if_is_up(ifp))
        return;

    /* Check interface. */
    if (! if_is_operative(ifp))
    {
        return;
    }

    if (!ri || !ri->ripng_enable || (ri->mode != IFNET_MODE_L3))
    {
        return;
    }

    /* any interface MUST have an IPv4 address */
    if (!ripng_if_ipv6_lladdress_check(ifp))
    {
        /* Might as well clean up the route table as well
         * rip_if_down sets to 0 ri->running, and displays "turn off %s"
         **/
        ripng_if_down(ifp, 0);

        //if (IS_RIPNG_DEBUG_EVENT)
        {
            zlog_debug(RIPNG_DBG_EVENT, "no valid ip address under interface %s", ifp->name);
        }

        ripng_connect_set(ifp, 0, ripng);
        return;
    }

	/* Add interface wake up thread. */
	ripng_connect_set(ifp, 1, ripng);

    /* Send RIP request to the interface. */
	
    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, ifc))
    {
    	if(ifc->address && ifc->address->family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&ifc->address->u.prefix6))
    	{
			ripng_request(ifp);
			
			if (ri->ipv6_passive)
			{
				//if (IS_RIPNG_DEBUG_PACKET)
				{
					zlog_debug(RIPNG_DBG_EVENT, "interface %s is passive", ifp->name);
				}
				break;
			}
			ripng_output_process(ifc, NULL, ripng_all_route, ripng, 0);
    	}
    }

}

void
show_ripng_interface_sub(struct vty *vty, struct interface *ifp)
{
    int is_up;
    struct connected *connected;
    struct rip_interface *ri = NULL;
    struct listnode *cnode, *cnnode;

    vty_out(vty, "%s is %s%s", ifp->name,
            ((is_up = if_is_operative(ifp)) ? "up" : "down"), VTY_NEWLINE);
    vty_out(vty, "  ifindex %02x, MTU %u bytes, BW %u Kbit %s%s",
            ifp->ifindex, ifp->mtu, ifp->bandwidth, if_flag_dump(ifp->flags),
            VTY_NEWLINE);

    ri = (struct rip_interface *)ifp->info;

    for (ALL_LIST_ELEMENTS(ifp->connected, cnode, cnnode, connected))
    {

        if (connected->address->family != AF_INET6)
            continue;

        /* Show OSPF interface information. */
        vty_out(vty, "  Internet Address:	  %s/%d%s",
                inet6_ntoa(connected->address->u.prefix6), connected->address->prefixlen, VTY_NEWLINE);
    }

    /* Is interface RIP enabled? */
    if (!ri->ripng_enable)
    {
        vty_out(vty, "  RIPng not enabled on this interface%s", VTY_NEWLINE);
        return;
    }
    else if (!is_up)
    {
        vty_out(vty, "  RIPng is enabled, but this interface is down%s", VTY_NEWLINE);
        return;
    }
	
    vty_out(vty, "  ripng metric: %d%s", ri->ipv6_metric, VTY_NEWLINE);
    vty_out(vty, "  passive interface:   %s%s",
            ri->ipv6_passive ? "enabled" : "disabled", VTY_NEWLINE);

    vty_out(vty, "  split horizon:	 %s%s",
            (ri->ipv6_split_horizon == 1) ? "enabled" : "disabled", VTY_NEWLINE);
    vty_out(vty, "  poison reverse:	  %s%s",
            (ri->ipv6_poison_reverse == 2) ? "enabled" : "disabled", VTY_NEWLINE);

}

DEFUN(ipv6_ripng_enable,
      ipv6_ripng_enable_cmd,
      "ripng enable instance <1-255>",
      RIPNG_STR
      "Enable RIPng on the interface\n"
      "specified an instance\n"
      "Instance number\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct interface *ifp;
    vrf_id_t vrf_id = VRF_DEFAULT;
    struct rip_interface *ri;
    struct ripng_instance *ripng = NULL;
    unsigned char id;

    id = (unsigned char)atoi(argv[0]);
    VTY_CHECK_NM_INTERFACE(ifindex);

    /* Lookup/create interface by name. */
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ripng_enable != 0)
    {
        vty_error_out(vty, "The specified interface has been enabled by RIPng %d%s", ri->ripng_enable, VTY_NEWLINE);
        return CMD_SUCCESS;
    }

    ripng = ripng_instance_get(vty, id);
	ripng->ifcount++;
    rip_inter_if_set_value(ifindex, ifp, vrf_id);
    ri->ripng_enable = id;
//    ripng_link_local_address_add(ifp);
    ripng_enable_apply(ifp, ripng);
    return CMD_SUCCESS;

}

DEFUN(no_ipv6_ripng_enable,
      no_ipv6_ripng_enable_cmd,
      "no ripng enable instance",
      NO_STR
      RIPNG_STR
      "Enable RIPng on the interface\n"
      "specified an instance\n")
{
    uint32_t ifindex = (uint32_t)vty->index;
    struct interface *ifp;
    struct rip_interface *ri;

    VTY_CHECK_NM_INTERFACE(ifindex);
    /*according to the interface name look up the struct pointer ifp */

    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    if (ifp == NULL)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }


    if (ri->ripng_enable == 0)
    {
        vty_error_out(vty, "RIPng already disabled under this interface %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
	
    //if (IS_RIPNG_DEBUG_EVENT)
    {
        zlog_debug(RIPNG_DBG_EVENT, "disable interface %s", ifp->name);
    }

    ripng_if_down(ifp, 0);

    ri->ripng_enable = 0;
    return CMD_SUCCESS;
}

DEFUN(ipv6_ripng_split_horizon,
      ipv6_ripng_split_horizon_cmd,
      "ripng split-horizon",
      RIPNG_STR
      "Perform split horizon\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ipv6_split_horizon == RIP_SPLIT_HORIZON)
    {
        return CMD_SUCCESS;
    }

    ri->ipv6_split_horizon = RIP_SPLIT_HORIZON;

    return CMD_SUCCESS;
}

DEFUN(ipv6_ripng_split_horizon_poisoned_reverse,
      ipv6_ripng_split_horizon_poisoned_reverse_cmd,
      "ripng poisoned-reverse",
      RIPNG_STR
      "Perform poisoned-reverse\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ipv6_poison_reverse == RIP_SPLIT_HORIZON_POISONED_REVERSE)
    {
        return CMD_SUCCESS;
    }

    ri->ipv6_poison_reverse = RIP_SPLIT_HORIZON_POISONED_REVERSE;

    return CMD_SUCCESS;
}

/* CHANGED: no ip rip split-horizon
   Cisco and Zebra's command is
   no ip split-horizon
 */
DEFUN(no_ipv6_ripng_split_horizon,
      no_ipv6_ripng_split_horizon_cmd,
      "no ripng split-horizon",
      NO_STR
      RIPNG_STR
      "Perform split horizon\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ipv6_split_horizon == RIP_NO_SPLIT_HORIZON)
    {
        return CMD_SUCCESS;
    }

    ri->ipv6_split_horizon = RIP_NO_SPLIT_HORIZON;

    return CMD_SUCCESS;
}

DEFUN(no_ipv6_ripng_split_horizon_poisoned_reverse,
      no_ipv6_ripng_split_horizon_poisoned_reverse_cmd,
      "no ripng poisoned-reverse",
      NO_STR
      RIPNG_STR
      "Perform poisoned-reverse\n")
{
    struct interface *ifp;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    if (ri->ipv6_poison_reverse == RIP_NO_POISON_REVERSE)
    {
        return CMD_SUCCESS;
    }

    ri->ipv6_poison_reverse = RIP_NO_POISON_REVERSE;

    return CMD_SUCCESS;
}

DEFUN(ripng_passive_interface,
      ripng_passive_interface_cmd,
      "ripng passive",
      RIPNG_STR
      "Suppress routing updates on this interface\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;


    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ri->ipv6_passive = 1;

    //if (IS_RIPNG_DEBUG_EVENT)
        zlog_debug(RIPNG_DBG_EVENT, "interface %s: passive = %d", ifp->name, ri->passive);

    return CMD_SUCCESS;
}

DEFUN(no_ripng_passive_interface,
      no_ripng_passive_interface_cmd,
      "no ripng passive",
      NO_STR
      RIPNG_STR
      "Suppress routing updates on this interface\n")
{
    struct interface *ifp;
    struct rip_interface *ri;

    uint32_t ifindex = (uint32_t)vty->index;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ri->ipv6_passive = 0;

    //if (IS_RIPNG_DEBUG_EVENT)
        zlog_debug(RIPNG_DBG_EVENT, "interface %s: passive = %d", ifp->name, ri->passive);

    return CMD_SUCCESS;
}

DEFUN(ripng_default_metric,
      ripng_default_metric_cmd,
      "ripng metric <0-15>",
      RIPNG_STR
      "Set metric to inbound and outbound routes\n"
      "The value of metric adding to route")
{
    struct interface *ifp;
    struct ripng_instance *ripng;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;
	struct connected *ifc;
    struct listnode *node, *nnode;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    
    /* check rip instance */
    ri = (struct rip_interface *)ifp->info;
    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }
	
    ri->ipv6_metric = atoi(argv[0]);

    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    ripng = ripng_instance_lookup(ri->enable_inter_node);

    if (!ripng)
    {
        return CMD_WARNING;
    }

    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, ifc))
    {
    	if(ifc->address && ifc->address->family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&ifc->address->u.prefix6))
    	{
			ripng_output_process(ifc, NULL, ripng_all_route, ripng, 0);
    	}
    }

    return CMD_SUCCESS;
}

DEFUN(no_ripng_default_metric,
      no_ripng_default_metric_cmd,
      "no ripng metric",
      NO_STR
      RIPNG_STR
      "Set metric to inbound and outbound routes\n")
{
    struct interface *ifp;
    struct ripng_instance *ripng;
    struct rip_interface *ri;
    uint32_t ifindex = (uint32_t)vty->index;
	
	struct connected *ifc;
    struct listnode *node, *nnode;

    VTY_CHECK_NM_INTERFACE(ifindex);
    ifp = rip_find_interface(vty, ifindex);

    if (!ifp)
    {
        return CMD_WARNING;
    }

    /* check rip instance */
    ri = (struct rip_interface *)ifp->info;

    if (ri->mode != IFNET_MODE_L3)
    {
        VTY_INTERFACE_MODE_ERR;
        return CMD_SUCCESS;
    }

    ri->ipv6_metric = RIP_DEFAULT_METRIC_DEFAULT;


    if (!ri->enable_inter_node)
    {
        return CMD_SUCCESS;
    }

    ripng = ripng_instance_lookup(ri->enable_inter_node);

    if (!ripng)
    {
        return CMD_WARNING;
    }
	
    for (ALL_LIST_ELEMENTS(ifp->connected, node, nnode, ifc))
    {
    	if(ifc->address && ifc->address->family == AF_INET6 && IN6_IS_ADDR_LINKLOCAL(&ifc->address->u.prefix6))
    	{
			ripng_output_process(ifc, NULL, ripng_all_route, ripng, 0);
    	}
    }
	
    return CMD_SUCCESS;
}

DEFUN(show_ripng_interface,
      show_ripng_interface_cmd,
      "show ripng <1-255> interface (ethernet|gigabitethernet|xgigabitethernet) USP",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      CLI_INTERFACE_STR
      CLI_INTERFACE_ETHERNET_STR
      CLI_INTERFACE_GIGABIT_ETHERNET_STR
      CLI_INTERFACE_XGIGABIT_ETHERNET_STR
      "The port/subport format: <0-7>/<0-31>/<1-255>[.<1-4095>]\n")
{
    struct interface *ifp = NULL;
    struct ripng_instance *ripng = NULL;
    uint32_t ifindex = 0;
    char ifname_tmp[INTERFACE_NAMSIZ];
    unsigned char id;
    struct rip_interface *ri = NULL;

    id = (u_char)atoi(argv[0]);
    ripng = ripng_instance_lookup(id);

    if (! ripng)
    {
        VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    if (argv[1] == NULL && argv[2] == NULL)
    {
        vty_error_out(vty, "can`t read the configuration %s", VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    else
    {
        ifindex = ifm_get_ifindex_by_name((char *)argv[1], (char *)argv[2]);

        if (ifindex == 0)
        {
            vty_error_out(vty, "Command incomplete ,please check out%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        /*get the name of the interface*/
        ifm_get_name_by_ifindex(ifindex , ifname_tmp);

        if ((ifp = if_lookup_by_name(ifname_tmp)) == NULL)
        {
            vty_error_out(vty, "No such interface name%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {

            ri = (struct rip_interface *)ifp->info;

            if (!ri)
            {
                return CMD_SUCCESS;
            }

            if (ri->ripng_enable == id)
            {
                show_ripng_interface_sub(vty, ifp);
            }
            else
            {
                if (ri->ripng_enable == 0)
                    vty_error_out(vty, "This interface didn`t enable ripng protocol %s", VTY_NEWLINE);
                else
                    vty_error_out(vty, "This interface belongs to ripng instance %d %s", ri->ripng_enable, VTY_NEWLINE);
            }
        }
    }

    return CMD_SUCCESS;
}

DEFUN(show_ripng_interface_all,
      show_ripng_interface_all_cmd,
      "show ripng <1-255> interface",
      SHOW_STR
      RIPNG_STR
      "ripng instance number\n"
      CLI_INTERFACE_STR)
{
    struct interface *ifp = NULL;
    struct listnode *node = NULL;
    struct ripng_instance *ripng = NULL;
    unsigned char id;
    struct rip_interface *ri = NULL;

    id = (u_char)atoi(argv[0]);
    ripng = ripng_instance_lookup(id);

    if (! ripng)
    {
        VTY_RIP_INSTANCE_ERR;
        return CMD_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS_RO(iflist, node, ifp))
    {
        if (!ifp)
            continue;

        ri = (struct rip_interface *)ifp->info;

        if (ri && ri->ripng_enable == id)
            show_ripng_interface_sub(vty, ifp);
    }

    return CMD_SUCCESS;
}

/* Called when interface structure allocated. */
int
rip_interface_new_hook(struct interface *ifp)
{
    ifp->info = (struct rip_interface *)rip_interface_new();
    return 0;
}

/* Called when interface structure deleted. */
int
rip_interface_delete_hook(struct interface *ifp)
{
	XFREE(MTYPE_RIP_INTERFACE, ifp->info);
    ifp->info = NULL;
    return 0;
}

void
rip_l3if_init(void)
{
    ifm_l3if_init(rip_interface_config_write);


    /***install cmd in subif interface** */
    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &rip_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &route_summary_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_route_summary_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &rip_neighbor_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);

    /***install cmd in if interface** */

    install_element(PHYSICAL_IF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &rip_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &route_summary_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_route_summary_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &rip_neighbor_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

    install_element(PHYSICAL_IF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);


    install_element(PHYSICAL_IF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);

    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);

    /* trunk interface */
    install_element(TRUNK_IF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &rip_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &route_summary_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_route_summary_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &rip_neighbor_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

    install_element(TRUNK_IF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);


    install_element(TRUNK_IF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);

    /* sub trunk interface */
    install_element(TRUNK_SUBIF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &rip_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &route_summary_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_route_summary_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &rip_neighbor_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

    install_element(TRUNK_SUBIF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);


    install_element(TRUNK_SUBIF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);
		
    /* vlanif interface */
	install_element(VLANIF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &rip_default_metric_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &route_summary_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_route_summary_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &rip_neighbor_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

	install_element(VLANIF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);

	install_element(VLANIF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
	install_element(VLANIF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);

	/* lookback interface */
	install_element(LOOPBACK_IF_NODE, &ip_rip_split_horizon_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ip_rip_split_horizon_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ip_rip_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &rip_passive_interface_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_rip_passive_interface_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ip_rip_authentication_mode_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ip_rip_authentication_mode_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ip_rip_enable_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ip_rip_enable_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &rip_default_metric_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_rip_default_metric_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &route_summary_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_route_summary_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &rip_neighbor_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_rip_neighbor_cmd, CMD_SYNC);

	install_element(LOOPBACK_IF_NODE, &ip_rip_send_version_1_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ip_rip_send_version_2_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ip_rip_send_version_cmd, CMD_SYNC);


	install_element(LOOPBACK_IF_NODE, &ipv6_ripng_enable_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ipv6_ripng_enable_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ipv6_ripng_split_horizon_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ipv6_ripng_split_horizon_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ipv6_ripng_split_horizon_poisoned_reverse_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ripng_passive_interface_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ripng_passive_interface_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &ripng_default_metric_cmd, CMD_SYNC);
	install_element(LOOPBACK_IF_NODE, &no_ripng_default_metric_cmd, CMD_SYNC);


}

/* Allocate and initialize interface vector. */
void
rip_if_init(void)
{
    /* Default initial size of interface vector. */
    if_add_hook(IF_NEW_HOOK, rip_interface_new_hook);
    if_add_hook(IF_DELETE_HOOK, rip_interface_delete_hook);

    install_element(CONFIG_NODE, &show_rip_interface_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_rip_interface_vlan_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_rip_interface_trunk_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_rip_interface_loopback_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_rip_interface_all_cmd, CMD_LOCAL);

    install_element(CONFIG_NODE, &show_ripng_interface_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ripng_interface_all_cmd, CMD_LOCAL);

    /*l3if init */
    rip_l3if_init();

}
