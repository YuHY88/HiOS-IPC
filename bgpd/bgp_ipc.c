/* protocol library.
   bgp register to itm and route, prepare to recive message from ifm,
   ifm: interface link
   route: interface ip and route
 */

#include "stream.h"
#include "log.h"
#include "thread.h"
#include "memory.h"
#include "errcode.h"
#include "buffer.h"
#include <lib/fifo.h>
#include "lib/mpls_common.h"
#include "lib/memtypes.h"
#include "ifm_common.h"
#include <route/route.h>
#include <lib/linklist.h>
#include <pthread.h>
#include <sys/msg.h>
#include "lib/msg_ipc_n.h"
#include "lib/snmp_common.h"

#include "bgpd/bgpd.h"
#include "bgpd/bgp_ipc.h"
#include "bgpd/bgp_zebra.h"
#include "bgpd/bgp_debug.h"
#include "bgpd/bgp_msg.h"
#include "bgpd/bgp_damp.h"
#include "bgpd/bgp_packet.h"
#include "bgpd/bgp_route.h"

/* Get prefix in ZServ format; family should be filled in on prefix */
static void stream_get_prefix (struct ifm_event *pevent, struct prefix *p)
{
    size_t plen = prefix_blen (p);
    u_char c;
	
    p->prefixlen = 0;
	
    if (plen == 0)
    {
        return;
    }
    memcpy (&p->u.prefix, &(pevent->ipaddr.addr), plen);
	if(pevent->ipaddr.type != INET_FAMILY_IPV6)
	{
    	p->u.prefix4.s_addr = ntohl(p->u.prefix4.s_addr);
	}
    c = pevent->ipaddr.prefixlen;
    p->prefixlen = MIN(plen * 8, c);
}

static int
memconstant(const void *s, int c, size_t n)
{
    const u_char *p = s;
	
    while (n-- > 0)
        if (*p++ != c)
            return 0;
		
    return 1;
}

int bgp_l3vpn_event_register()
{
	return mpls_com_l3vpn_event_register(MODULE_ID_BGP);
}

int bgp_com_label_new(uint16_t vrf_id)
{
	uint32_t *plabel = NULL;
    uint32_t label = 0;
	
	plabel =  mpls_com_get_label(vrf_id, MODULE_ID_BGP);
	if(plabel == NULL)
	{
		return 0;
	}
	label = *plabel;
    mem_share_free_bydata(plabel, MODULE_ID_BGP);
    //if (BGP_DEBUG (events, EVENTS))
	//{
		zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] vrf_id %d label %u ",__func__,__LINE__, vrf_id, label);
	//}
	
	return label;
}

int bgp_com_label_free(uint16_t vrf_id, uint32_t label)
{
    //if (BGP_DEBUG (events, EVENTS))
	//{
		zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] vrf_id %d label %d ",__func__,__LINE__, vrf_id, label);
	//}
	
	if(label == 0)
	{
		return 0;
	}
	
	return mpls_com_label_free(vrf_id, label, MODULE_ID_BGP);
}



static int
bgp_route_msg_send(struct route_entry *route, enum IPC_OPCODE opcode,int num)
{
    //int ret = ipc_send_route(route, num*sizeof(struct route_entry), num, MODULE_ID_ROUTE, MODULE_ID_BGP,IPC_TYPE_ROUTE, 0, opcode);

	int ret = ipc_send_msg_n2(route, sizeof(struct route_entry)*num, num,MODULE_ID_ROUTE,MODULE_ID_BGP,
                              IPC_TYPE_ROUTE, 0, opcode, 0);
	if(ret < 0)
	{
		zlog_err("%s[%d],ret %d opcode %d num %d",__FUNCTION__,__LINE__,ret,opcode,num);
	}

	
    return ret;
}

static int
bgp_route_vrf_msg_send(struct l3vpn_route *l3vpn_route_entry, enum IPC_OPCODE opcode,int num)
{
    //int ret = ipc_send_route(l3vpn_route_entry, num * sizeof(struct l3vpn_route), num, MODULE_ID_MPLS, MODULE_ID_BGP, IPC_TYPE_L3VPN, 0, opcode);

	int ret = ipc_send_msg_n2(l3vpn_route_entry, sizeof(struct l3vpn_route)*num, num, MODULE_ID_MPLS,MODULE_ID_BGP,
                              IPC_TYPE_L3VPN, 0, opcode, 0);
	if(ret < 0)
	{
		zlog_err("%s[%d],ret %d opcode %d num %d",__FUNCTION__,__LINE__,ret,opcode,num);
	}
	
    return ret;
}



int bgp_send_route_timer(void *para)
{    
	int i = 0;
	int ret = 0;
	int item = 0;
	int send_count = 0;
    enum IPC_OPCODE opcode;
    struct fifo *pfifo = NULL;
    int msg_num = IPC_BGP_LEN/sizeof(struct route_entry);
    struct route_entry route[IPC_BGP_LEN/sizeof(struct route_entry)];
    struct routefifo *routefifo_entry[IPC_BGP_LEN/sizeof(struct route_entry)];
	
    for (item = 0; item < msg_num; item ++)
    {
        routefifo_entry[item] = NULL;
    }
	
    bm->routefifo_id = 0;
	 
    while (!FIFO_EMPTY(&bm->routefifo))
    {
        if (send_count >= 1000)
        {
            break;
        }
		
        pfifo = &bm->routefifo;
        routefifo_entry[0] = (struct routefifo *)FIFO_TOP(pfifo);
        opcode = routefifo_entry[0]->opcode;
        pfifo = &(routefifo_entry[0]->fifo);
        memcpy(&route[0], &routefifo_entry[0]->route, sizeof(struct route_entry));
        item = 1;
		
        while (FIFO_HEAD(pfifo) && item < msg_num)
        {
            routefifo_entry[item] = (struct routefifo *)FIFO_TOP(pfifo);
            if (opcode != routefifo_entry[item]->opcode)
            {
                break;
            }
			
            memcpy(&route[item], &routefifo_entry[item]->route, sizeof(struct route_entry));
            pfifo = &(routefifo_entry[item]->fifo);
            item ++;
        }
		
        ret = bgp_route_msg_send(route, opcode, item);
       
        if (ret < 0)
        {
            break;
        }
        else
        {
            send_count += item;
        }
		
        for (i = 0; i < item; i ++)
        {
            FIFO_DEL(routefifo_entry[i]);
            XFREE (MTYPE_ROUTE_FIFO, routefifo_entry[i]);
        }
    }

	//bm->routefifo_id =  high_pre_timer_add("bgp_send_route", LIB_TIMER_TYPE_LOOP, bgp_send_route_timer, NULL, 1000);
	
  //  bm->t_thread_routefifo =
  //      thread_add_timer_msec (bm->master, bgp_send_route_timer, NULL, 1000);
	
    return 0;
}


int bgp_send_route_timer_vrf(void *para)
{    
	int i = 0;
	int ret = 0;
	int item = 0;
	int send_count = 0;
    enum IPC_OPCODE opcode;
    struct fifo *pfifo = NULL;
    int msg_num = IPC_BGP_LEN/sizeof(struct l3vpn_route);
    struct l3vpn_route l3vpn_route[IPC_BGP_LEN/sizeof(struct l3vpn_route)];
    struct routefifo_vrf *routefifo_vrf[IPC_BGP_LEN/sizeof(struct l3vpn_route)];
	
    for (item = 0; item < msg_num; item ++)
    {
        routefifo_vrf[item] = NULL;
    }
	
	bm->routefifo_vrf_id = 0;

    while (!FIFO_EMPTY(&bm->routefifo_vrf))
    {
        if (send_count >= 1000)
        {
            break;
        }
		
        pfifo = &bm->routefifo_vrf;
        routefifo_vrf[0] = (struct routefifo_vrf *)FIFO_TOP(pfifo);
        opcode = routefifo_vrf[0]->opcode;
        pfifo = &(routefifo_vrf[0]->fifo);
        memcpy(&l3vpn_route[0], &routefifo_vrf[0]->l3vpn_route, sizeof(struct l3vpn_route));
        item = 1;

		while (FIFO_HEAD(pfifo) && item < msg_num)
        {
            routefifo_vrf[item] = (struct routefifo_vrf *)FIFO_TOP(pfifo);
            if (opcode != routefifo_vrf[item]->opcode)
            {
                break;
            }
			
            memcpy(&l3vpn_route[item], &routefifo_vrf[item]->l3vpn_route, sizeof(struct l3vpn_route));
            pfifo = &(routefifo_vrf[item]->fifo);
            item ++;
        }
		
        ret = bgp_route_vrf_msg_send(l3vpn_route, opcode, item);
        if (ret < 0)
        {
            break;
        }
        else
        {
            send_count += item;
        }
		
        for (i = 0; i < item; i ++)
        {
            FIFO_DEL(routefifo_vrf[i]);
            XFREE (MTYPE_BGP_VRF_ROUTE_INFO, routefifo_vrf[i]);
        }
    }

     //bm->routefifo_vrf_id  =  high_pre_timer_add("bgp_send_route_vrf", LIB_TIMER_TYPE_LOOP, bgp_send_route_timer_vrf, NULL, 1000);
	
    //bm->t_thread_routefifo_vrf =
    //    thread_add_timer_msec (bm->master, bgp_send_route_timer_vrf, NULL, 1000);
	
    return 0;
}

void bgp_zapi_route (enum IPC_OPCODE opcode, struct prefix *p, struct zapi_route *api)
{
    struct route_entry route;
    struct routefifo *newroutefifoentry;
	
    memset(&route, 0x0, sizeof(struct route_entry));
	
	if(p->family == AF_INET6)
	{
	    route.prefix.type = INET_FAMILY_IPV6;
		IPV6_ADDR_COPY(route.prefix.addr.ipv6, &p->u.prefix6);
		
		if (CHECK_FLAG (api->flags, ZEBRA_FLAG_BLACKHOLE))
	    {
	        route.nhp[0].action = NHP_ACTION_DROP;
	    }
		else
		{
			route.nhp[0].nexthop.type = INET_FAMILY_IPV6;
			IPV6_ADDR_COPY(route.nhp[0].nexthop.addr.ipv6, &api->nexthop_v6);
		}
	}
	else
	{
	    route.prefix.type = INET_FAMILY_IPV4;
	    route.prefix.addr.ipv4 = ntohl(p->u.prefix4.s_addr);
		
	    if (CHECK_FLAG (api->flags, ZEBRA_FLAG_BLACKHOLE))
	    {
	        route.nhp[0].action = NHP_ACTION_DROP;
	    }
		else
		{
			route.nhp[0].nexthop.type = INET_FAMILY_IPV4;
	        route.nhp[0].nexthop.addr.ipv4 = ntohl(api->nexthop.s_addr);
		}
	}
	
    route.prefix.prefixlen = p->prefixlen;
    route.vpn = api->vrf_id;
	route.nhp_num = 1;
	
    route.nhp[0].nhp_type = NHP_TYPE_IP;

    if(CHECK_FLAG(api->flags,ZEBRA_FLAG_IBGP))
    {
    	if(p->family == AF_INET6)
		{
        	route.nhp[0].protocol = ROUTE_PROTO_IBGP6;
		}
		else
		{
        	route.nhp[0].protocol = ROUTE_PROTO_IBGP;
		}
    }
    else
    {
    	if(p->family == AF_INET6)
		{
        	route.nhp[0].protocol = ROUTE_PROTO_EBGP6;
		}
		else
		{
        	route.nhp[0].protocol = ROUTE_PROTO_EBGP;
		}
    }
	
    route.nhp[0].distance = api->distance;
    route.nhp[0].cost = api->metric;
    route.nhp[0].vpn = api->vrf_id;
    route.nhp[0].down_flag = LINK_DOWN;
    route.nhp[0].active = ROUTE_STATUS_INACTIVE;

    newroutefifoentry = XCALLOC (MTYPE_ROUTE_FIFO, sizeof (struct routefifo));
    if(NULL == newroutefifoentry)
    {
        zlog_err("%s [%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return;
    }
	
    newroutefifoentry->opcode = opcode;
    memcpy(&newroutefifoentry->route,&route,sizeof(struct route_entry));
    FIFO_ADD(&bm->routefifo, &newroutefifoentry->fifo);
	
    return;
}

void bgp_zapi_ipv4_route_vrf (enum IPC_OPCODE opcode, struct prefix_ipv4 *p, struct zapi_route *api,int flag)
{
    struct l3vpn_route l3vpn_route_entry;
    struct routefifo_vrf *newroutevrffifo;
	
    memset(&l3vpn_route_entry, 0x0, sizeof(struct l3vpn_route));
	
    l3vpn_route_entry.prefix.type = INET_FAMILY_IPV4;
    l3vpn_route_entry.prefix.addr.ipv4 = ntohl(p->prefix.s_addr);
    l3vpn_route_entry.prefix.prefixlen = p->prefixlen;
    l3vpn_route_entry.vrf_id = api->vrf_id;
	
	l3vpn_route_entry.nhp.nexthop.type = INET_FAMILY_IPV4;
	
    if (!CHECK_FLAG (api->flags, ZEBRA_FLAG_BLACKHOLE))
    {	
		l3vpn_route_entry.nhp.nexthop.addr.ipv4 = ntohl(api->nexthop.s_addr);
    }

	if(CHECK_FLAG(api->flags,ZEBRA_FLAG_IBGP))
    {
        l3vpn_route_entry.nhp.protocol = ROUTE_PROTO_IBGP;
    }
    else
    {
        l3vpn_route_entry.nhp.protocol = ROUTE_PROTO_EBGP;
    }
	
	if(flag)
	{
		l3vpn_route_entry.nhp.import_flag = 1;
	}
	else
	{
		l3vpn_route_entry.nhp.import_flag = 0;
	}

	if(api->ifindex_tnl)
	{
		l3vpn_route_entry.nhp.tunnel_if = api->ifindex_tnl;
	}
	
	l3vpn_route_entry.nhp.distance = api->distance;
	l3vpn_route_entry.nhp.inlabel = api->inlabel;
	l3vpn_route_entry.nhp.outlabel = api->outlabel;
	l3vpn_route_entry.nhp.cost = api->metric;

	newroutevrffifo = XCALLOC (MTYPE_BGP_VRF_ROUTE_INFO, sizeof (struct routefifo_vrf));
    if(NULL == newroutevrffifo)
    {
        zlog_err("%s [%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
        return;
    }
	
    newroutevrffifo->opcode = opcode;
    memcpy(&newroutevrffifo->l3vpn_route,&l3vpn_route_entry,sizeof(struct l3vpn_route));
    FIFO_ADD(&bm->routefifo_vrf, &newroutevrffifo->fifo);
	
    return;
	
}


/* Interface addition from zebra daemon. */
struct interface *
bgp_interface_add_read (int type, struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Read interface name. */
    ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

    /* Lookup/create interface by name. */
    ifp = if_get_by_name_len_vrf (ifname_tmp, strnlen (ifname_tmp, INTERFACE_NAMSIZ), vrf_id);
	
    if(ZEBRA_INTERFACE_ADDRESS_ADD == type)
    {
        bgp_interface_if_set_value (pevent, ifp);
    }

    return ifp;
}

/*
 * Read interface up/down msg (ZEBRA_INTERFACE_UP/ZEBRA_INTERFACE_DOWN)
 * from zebra server.  The format of this message is the same as
 * that sent for ZEBRA_INTERFACE_ADD/ZEBRA_INTERFACE_DELETE (see
 * comments for zebra_interface_add_read), except that no sockaddr_dl
 * is sent at the tail of the message.
 */
struct interface *
bgp_interface_state_read (struct ifm_event *pevent, vrf_id_t vrf_id)
{
    struct interface *ifp;
    char ifname_tmp[INTERFACE_NAMSIZ];

    /* Get interface name. */
    ifm_get_name_by_ifindex(pevent->ifindex, ifname_tmp);

    /* Lookup this by interface index. */
    ifp = if_lookup_by_name_len_vrf (ifname_tmp, strnlen (ifname_tmp, INTERFACE_NAMSIZ), vrf_id);

    /* If such interface does not exist, indicate an error */
    if (NULL == ifp)
    {
        zlog_debug(BGP_DEBUG_TYPE_OTHER,"%s[%d] ", __FUNCTION__, __LINE__);
        return NULL;
    }
	
    bgp_interface_if_set_value (pevent, ifp);

    return ifp;
}

void
bgp_interface_if_set_value (struct ifm_event *pevent,struct interface *ifp)
{
    uchar    pmac[6] = {0};
    uint16_t mtu = 0;
    uint32_t type = 0;
	
    ifp->ifindex = pevent->ifindex;
	
    type = IFM_TYPE_ID_GET (pevent->ifindex );
    if(type == IFNET_TYPE_ETHERNET)
    {
        ifp->ll_type = IFNET_TYPE_GIGABIT_ETHERNET;
        ifp->metric = 1;
        ifp->bandwidth = 100000;
    }
    else if(type == IFNET_TYPE_LOOPBACK)
    {
        ifp->metric = 0;
        ifp->ll_type = IFNET_TYPE_LOOPBACK;
        SET_FLAG(ifp->flags, IFF_LOOPBACK);
        ifp->bandwidth = 0;
    }
	
    if(pevent->up_flag == IFNET_LINKUP)
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
	
    if(ifm_get_mac(pevent->ifindex, MODULE_ID_BGP, pmac) == 0)
    {
        ifp->hw_addr_len = 6;
        ifp->hw_addr[0] = pmac[0];
        ifp->hw_addr[1] = pmac[1];
        ifp->hw_addr[2] = pmac[2];
        ifp->hw_addr[3] = pmac[3];
        ifp->hw_addr[4] = pmac[4];
        ifp->hw_addr[5] = pmac[5];
    }
	
    /* get mtu */
    if(ifm_get_mtu(pevent->ifindex, MODULE_ID_BGP, &mtu) == 0)
    {
        ifp->mtu = mtu;
    }
    else
    {
        ifp->mtu = 9600;
    }
	
    return;
}

struct connected *
bgp_interface_address_read (int type, struct ifm_event *pevent, vrf_id_t vrf_id)
{    
	int plen;
    u_char ifc_flags;
    struct prefix p;
	struct prefix d;
	struct prefix *dp;    
	struct interface *ifp;
    struct connected *ifc;

    memset (&p, 0, sizeof(struct prefix));
    memset (&d, 0, sizeof(struct prefix));

    /* Get interface index. */
    ifp = if_lookup_by_index_vrf (pevent->ifindex, vrf_id);
    if (ifp == NULL)
    {
        ifp = bgp_interface_add (type,pevent,vrf_id);
    }

    if (ifp == NULL)
    {
        zlog_debug (BGP_DEBUG_TYPE_OTHER,"%s[%d] (%s): Can't find interface by ifindex: %d ",__FUNCTION__,__LINE__,
                   (type == ZEBRA_INTERFACE_ADDRESS_ADD? "ADD" : "DELETE"), pevent->ifindex);
        return NULL;
    }

    /* Fetch flag. */
    ifc_flags = 0x00;

	if(pevent->ipaddr.type == INET_FAMILY_IPV6)
	{
    	d.family = p.family = AF_INET6;
	}
	else
	{
    	d.family = p.family = AF_INET;
	}
	
    plen = prefix_blen (&d);

    stream_get_prefix (pevent, &p);
	
    //if(BGP_DEBUG(events, EVENTS))
   	//{
        char buf[IPV6_ADDR_STRLEN] = "";
		
        prefix2str(&p, buf, IPV6_ADDR_STRLEN);
    	zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d] type(%d) p %s ",__FUNCTION__,__LINE__, type, buf);
    //}
	
	/* Fetch destination address. */
    memcpy (&d.u.prefix, &(pevent->ipaddr.addr), plen);

    /* N.B. NULL destination pointers are encoded as all zeroes */
    dp = memconstant(&d.u.prefix,0,plen) ? NULL : &d;

    if (type == ZEBRA_INTERFACE_ADDRESS_ADD)
    {
        /* N.B. NULL destination pointers are encoded as all zeroes */
        ifc = connected_add_by_prefix(ifp, &p, dp);
        if (ifc != NULL)
        {
            ifc->flags = ifc_flags;
			
            if (ifc->destination)
            {
                ifc->destination->prefixlen = ifc->address->prefixlen;
            }
            else if (CHECK_FLAG(ifc->flags, ZEBRA_IFA_PEER))
            {
                /* carp interfaces on OpenBSD with 0.0.0.0/0 as "peer" */
                char buf1[PREFIX_STRLEN] = "";
                zlog_debug(BGP_DEBUG_TYPE_OTHER,"warning: interface %s address %s "
                          "with peer flag set, but no peer address!",
                          ifp->name,
                          prefix2str (ifc->address, buf1, sizeof buf1));
                UNSET_FLAG(ifc->flags, ZEBRA_IFA_PEER);
            }
			
			//if(BGP_DEBUG(events, EVENTS))
			//{        
				
                char buf2[PREFIX_STRLEN] = "";
				prefix2str (&p, buf2, sizeof(buf2));
            	zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d],p %s ",__FUNCTION__,__LINE__,buf2);
			//}
        }
    }
    else
    {
        //assert (type == ZEBRA_INTERFACE_ADDRESS_DELETE);
		if(type != ZEBRA_INTERFACE_ADDRESS_DELETE)
		{
			return NULL;
		}
		
        ifc = connected_delete_by_prefix(ifp, &p);
    }

    return ifc;
}

void
bgp_redistribute_route_register(void)
{
    route_event_register(ROUTE_PROTO_STATIC, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_CONNECT, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_OSPF, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_ISIS, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_RIP, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_IBGP, MODULE_ID_BGP);
    route_event_register(ROUTE_PROTO_EBGP, MODULE_ID_BGP);
}

void
bgp_redistribute_route_unregister(void)
{
    route_event_unregister(ROUTE_PROTO_STATIC, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_CONNECT, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_OSPF, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_ISIS, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_RIP, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_IBGP, MODULE_ID_BGP);
    route_event_unregister(ROUTE_PROTO_EBGP, MODULE_ID_BGP);
}

void
bgp_redistribute_routev6_register(void)
{
    routev6_event_register(ROUTE_PROTO_STATIC, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_CONNECT, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_OSPF6, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_ISIS6, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_RIPNG, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_IBGP6, MODULE_ID_BGP);
    routev6_event_register(ROUTE_PROTO_EBGP6, MODULE_ID_BGP);
}

void
bgp_redistribute_routev6_unregister(void)
{
    routev6_event_unregister(ROUTE_PROTO_STATIC, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_CONNECT, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_OSPF6, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_ISIS6, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_RIPNG, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_IBGP6, MODULE_ID_BGP);
    routev6_event_unregister(ROUTE_PROTO_EBGP6, MODULE_ID_BGP);
}

static void bgp_route_manage( struct ipc_mesg_n *mesg)
{
    int i = 0;
	struct bgp *bgp = NULL;
    struct route_entry *p_route = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
	
    /* process the ipc message */
    pmsghdr = &(mesg->msghdr);
    if(!pmsghdr)
    {
        return;
    }

	bgp = bgp_get_default();
	if(NULL == bgp)
	{
		return;
	}
	
    if(pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        for(i = 0 ; i < pmsghdr->data_num ; i ++)
        {
            p_route = (struct route_entry *)&(mesg->msg_data) + i;
            bgp_route_read_ipv4 (p_route, mesg);
        }
    }
}

/*  ifm register  */
void bgp_ifm_register(void)
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_BGP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_BGP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_BGP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_BGP, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_BGP, IFNET_IFTYPE_L3IF);
}

/* recieve msg from ifm*/
static void bgp_ifm_manage(struct ipc_mesg_n *pMsg)
{
    //char name[INTERFACE_NAMSIZ];
    vrf_id_t vrf_id = VRF_DEFAULT;
	void *pdata = NULL;
    struct ifm_event pevent;
	
    pdata = pMsg->msg_data;
	memset(&pevent,0x0,sizeof(struct ifm_event));
    pevent = *((struct ifm_event*)pdata);

	if(pMsg->msghdr.opcode == IPC_OPCODE_EVENT)
	{
        switch (pMsg->msghdr.msg_subtype)
        {
	       case IFNET_EVENT_IP_ADD:
	       {
	          bgp_interface_address_add(&pevent, vrf_id);
	          break;
	       }
	       case IFNET_EVENT_IP_DELETE:
	       {
	          bgp_interface_address_delete(&pevent, vrf_id);
	          break;
	       }
	       case IFNET_EVENT_UP:
	       {
	          bgp_interface_up (&pevent, vrf_id);
	          break;
	       }
	       case IFNET_EVENT_DOWN:
	       {
	          bgp_interface_down (&pevent, vrf_id);
	          break;
	       }
	       case IFNET_EVENT_IF_DELETE:
	       {
	          bgp_interface_delete(&pevent, vrf_id);
	          break;
	       }
	       default:
	       {
	          break;
	       }
        }
	}
	
    return;
}

void bgp_msg_send_noack(struct ipc_msghdr_n *pReqhdr, uint32_t errcode, unsigned int msg_index)
{
    struct ipc_mesg_n *pMsgRep = NULL;

    pMsgRep = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(uint32_t), MODULE_ID_BGP);
    if(pMsgRep == NULL) 
    {
        return;     //if malloc fail, remote can only wait for a timeout
    }

    memset(pMsgRep, 0, sizeof(struct ipc_msghdr_n));

    pMsgRep->msghdr.data_len    = sizeof(uint32_t);
    pMsgRep->msghdr.opcode      = IPC_OPCODE_NACK;
    pMsgRep->msghdr.msg_index   = msg_index;
    pMsgRep->msghdr.data_num    = 1;
    memcpy(pMsgRep->msg_data, &errcode, sizeof(uint32_t));
    
    if(ipc_send_reply_n1(pReqhdr, pMsgRep, sizeof(struct ipc_msghdr_n) + sizeof(uint32_t)) == -1)
    {
        mem_share_free(pMsgRep, MODULE_ID_BGP);
    }
    
}

static void bgp_snmp_as_get(struct ipc_msghdr_n *pmsghdr)
{
    struct bgp *bgp;
    struct bgp_as_key bgp_as;
	
    memset(&bgp_as,0x0,sizeof(struct bgp_as_key));
	
    bgp = bgp_get_default();
    if(bgp)
    {
        bgp_as.as = bgp->as;
        bgp_as.router_id = bgp->router_id_static;
        bgp_as.distance_ebgp = bgp->distance_ebgp;
        bgp_as.distance_ibgp = bgp->distance_ibgp;
        bgp_as.distance_local = bgp->distance_local;
        bgp_as.keepalive = bgp->default_keepalive;
        bgp_as.holdtime = bgp->default_holdtime;
        bgp_as.retrytime = bgp->default_connect;
        bgp_as.local_pref = bgp->default_local_pref;
        bgp_as.med = bgp->default_med;
		
        if(bgp_flag_check (bgp, BGP_FLAG_ASPATH_IGNORE))
        {
            bgp_as.as_path = 1;
        }
		
        if(bgp_flag_check (bgp, BGP_FLAG_DETERMINISTIC_MED))
        {
            bgp_as.med_deter = 1;
        }
		
        if(bgp_flag_check (bgp, BGP_FLAG_ALWAYS_COMPARE_MED))
        {
            bgp_as.med_diff = 1;
        }
		
        if(CHECK_FLAG (bgp->af_flags[AFI_IP][SAFI_UNICAST], BGP_CONFIG_DAMPENING))
        {
            bgp_as.damped = 1;
            bgp_as.half_life = damp->half_life/60;
            bgp_as.reuse_limit = damp->reuse_limit;
            bgp_as.suppress_value = damp->suppress_value;
            bgp_as.max_suppress_time = damp->max_suppress_time/60;
        }

		//ret = ipc_send_reply_n2(&bgp_as, sizeof(struct bgp_as_key), 1 , pmsghdr->sender_id,
        //                        MODULE_ID_BGP, IPC_TYPE_BGP, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);

		ipc_ack_to_snmp(pmsghdr, &bgp_as, sizeof(struct bgp_as_key), 1);   
    }
    else
    {
        //bgp_msg_send_noack( pmsghdr, ERRNO_NOT_FOUND, pmsghdr->msg_index);
        ipc_noack_to_snmp(pmsghdr);
    }
    return;
}

static void bgp_snmp_redistribute_get(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int i = 0;
	int num = 0;	
    int ret = 0;
	int flag = 0;
	int msg_num = 0;
    int instance = 0;
	int last_flag = 1;
	int type_redis = 0;
    afi_t afi = AFI_IP;
    struct bgp *bgp;
	enum ROUTE_PROTO type;
	struct bgp_redist *redist;
	struct listnode *node, *nnode;
	
    struct bgp_redistribute_key bgp_redistribute_last = *((struct bgp_redistribute_key*)pdata);
	
    bgp = bgp_get_default();
    if(bgp)
    {
        msg_num = IPC_BGP_LEN/sizeof(struct bgp_redistribute_key);
        struct bgp_redistribute_key bgp_redistribute[IPC_BGP_LEN/sizeof(struct bgp_redistribute_key)];
        memset(bgp_redistribute,0x0,msg_num*sizeof(struct bgp_redistribute_key));

		for(i = 0 ; i < 129 ; i ++)
		{
			if(i < bgp_redistribute_last.vrf_id)
				continue;

			if(i == 0  &&  bgp_redistribute_last.vrf_id == 0)
			{
		        for(type = ROUTE_PROTO_INVALID ; type < ROUTE_PROTO_NUM; type ++)
		        {
		            if(type < bgp_redistribute_last.redist_type || bgp_redistribute_last.redist_type == ROUTE_PROTO_STATIC || bgp_redistribute_last.redist_type == ROUTE_PROTO_CONNECT )
		            {
		                continue;
		            }
					
		            if(ROUTE_PROTO_OSPF == type || ROUTE_PROTO_ISIS == type || ROUTE_PROTO_RIP == type)
		            {
		                type_redis = type - ROUTE_PROTO_OSPF;
		                for(instance = 0; instance < MAX_INSTANCE_ID ; instance ++)
		                {
		                    if(type == bgp_redistribute_last.redist_type && instance <= bgp_redistribute_last.redist_instance)
		                    {
		                        continue;
		                    }
							
		                    if(bgp->redist_instance[type_redis][instance].redist_instance_flag)
		                    {
		                        flag = 1;
			                    bgp_redistribute[num].as = bgp->as;
								bgp_redistribute[num].vrf_id = 0;
		                        bgp_redistribute[num].redist_type= type;
		                        bgp_redistribute[num].redist_instance= instance;
		                        if(bgp->redist_instance[type_redis][instance].redist_instance_metric_flag)
		                        {
		                            bgp_redistribute[num].redist_metric_flag= bgp->redist_instance[type_redis][instance].redist_instance_metric_flag;
		                            bgp_redistribute[num].redist_metric= bgp->redist_instance[type_redis][instance].redist_instance_metric;
		                        }
		                        num ++;
		                    }
							
							if(num == msg_num)
							{
								break;
							}
		                }
		            }
		            else
		            {
		                if(bgp->redist[afi][type])
		                {
		                    flag = 1;
		                    bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = 0;
		                    bgp_redistribute[num].redist_type= type;
		                    bgp_redistribute[num].redist_instance= 0;
		                    if(bgp->redist_metric_flag[afi][type])
		                    {
		                        bgp_redistribute[num].redist_metric_flag= bgp->redist_metric_flag[afi][type];
		                        bgp_redistribute[num].redist_metric= bgp->redist_metric[afi][type];
		                    }
		                    num ++;
		                }
						
		            }
					
		            if(num == msg_num)
		            {
		                break;
		            }
		        }
				
				if(num == msg_num)
				{
					break;
				}
			}
			else
			{
				if(i == 0 || bgp_vrf_redist_info[i] == NULL)
				{
					continue;
				}
				
		        for(type = ROUTE_PROTO_INVALID ; type < ROUTE_PROTO_NUM; type ++)
	        	{
		            if((i == bgp_redistribute_last.vrf_id && type < bgp_redistribute_last.redist_type)
						|| (i == bgp_redistribute_last.vrf_id  && bgp_redistribute_last.redist_type == ROUTE_PROTO_STATIC )
						|| (i == bgp_redistribute_last.vrf_id  && bgp_redistribute_last.redist_type == ROUTE_PROTO_CONNECT ))
		            {
		                continue;
		            }
					
		            if(ROUTE_PROTO_OSPF == type)
	            	{
	            	    last_flag = 1;
						for(ALL_LIST_ELEMENTS (bgp_vrf_redist_info[i]->redist_ospf, node, nnode, redist))
						{
							if(i == bgp_redistribute_last.vrf_id && type == bgp_redistribute_last.redist_type && last_flag)
							{
								if(bgp_redistribute_last.redist_instance == redist->instance)
								{
									last_flag = 0;
								}
								
								continue;
							}
							
							flag = 1;
							bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = i;
							bgp_redistribute[num].redist_type = type;
							bgp_redistribute[num].redist_instance= redist->instance;
							if(redist->metric_flag)
							{
								bgp_redistribute[num].redist_metric_flag= redist->metric_flag;
								bgp_redistribute[num].redist_metric= redist->metric;
							}
							num ++;
							
							if(num == msg_num)
							{
								break;
							}
						}
	            	}
					else if( ROUTE_PROTO_ISIS == type)
	            	{
	            	    last_flag = 1;
						for(ALL_LIST_ELEMENTS (bgp_vrf_redist_info[i]->redist_isis, node, nnode, redist))
						{						
							if(i == bgp_redistribute_last.vrf_id && type == bgp_redistribute_last.redist_type && last_flag)
							{
								if(bgp_redistribute_last.redist_instance == redist->instance)
								{
									last_flag = 0;
								}
								
								continue;
							}
							
							flag = 1;
							bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = i;
							bgp_redistribute[num].redist_type= type;
							bgp_redistribute[num].redist_instance= redist->instance;
							if(redist->metric_flag)
							{
								bgp_redistribute[num].redist_metric_flag= redist->metric_flag;
								bgp_redistribute[num].redist_metric= redist->metric;
							}
							num ++;
							
							if(num == msg_num)
							{
								break;
							}
						}
	            	}
					else if(ROUTE_PROTO_RIP == type)
	            	{
						last_flag = 1;
						for(ALL_LIST_ELEMENTS (bgp_vrf_redist_info[i]->redist_rip, node, nnode, redist))
						{						
							if(i == bgp_redistribute_last.vrf_id && type == bgp_redistribute_last.redist_type && last_flag)
							{
								if(bgp_redistribute_last.redist_instance == redist->instance)
								{
									last_flag = 0;
								}
								
								continue;
							}
							
							flag = 1;
							bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = i;
							bgp_redistribute[num].redist_type= type;
							bgp_redistribute[num].redist_instance= redist->instance;
							if(redist->metric_flag)
							{
								bgp_redistribute[num].redist_metric_flag= redist->metric_flag;
								bgp_redistribute[num].redist_metric= redist->metric;
							}
							num ++;
							
							if(num == msg_num)
							{
								break;
							}
						}
	            	}					
					else
					{
		                if(ROUTE_PROTO_STATIC == type && bgp_vrf_redist_info[i]->redist[0])				
		                {
		                    flag = 1;
		                    bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = i;
		                    bgp_redistribute[num].redist_type = type;
		                    bgp_redistribute[num].redist_instance = 0;
		                    if(bgp_vrf_redist_info[i]->redist_metric_flag[0])
		                    {
		                        bgp_redistribute[num].redist_metric_flag= bgp_vrf_redist_info[i]->redist_metric_flag[0];
		                        bgp_redistribute[num].redist_metric= bgp_vrf_redist_info[i]->redist_metric[0];
		                    }					    
		                    num ++;
		                }
						else if(ROUTE_PROTO_CONNECT == type && bgp_vrf_redist_info[i]->redist[1])
						{
                            flag = 1;
		                    bgp_redistribute[num].as = bgp->as;
							bgp_redistribute[num].vrf_id = i;
		                    bgp_redistribute[num].redist_type = type;
		                    bgp_redistribute[num].redist_instance = 0;
 
                            if(bgp_vrf_redist_info[i]->redist_metric_flag[1])
		                    {                           
		                        bgp_redistribute[num].redist_metric_flag= bgp_vrf_redist_info[i]->redist_metric_flag[1];
		                        bgp_redistribute[num].redist_metric= bgp_vrf_redist_info[i]->redist_metric[1];
		                    }
		                    num ++;
		                }
					}
					
					if(num == msg_num)
					{
						break;
					}
	        	}
				
				if(num == msg_num)
				{
					break;
				}
			}
		}
        if(num == msg_num || flag)
        {   
            ipc_ack_to_snmp(pmsghdr, bgp_redistribute, num * sizeof(struct bgp_redistribute_key), num);
 
#if 0			
            ret = ipc_send_reply_bulk(bgp_redistribute, num*sizeof(struct bgp_redistribute_key), num, pmsghdr->sender_id,
                                      MODULE_ID_BGP, IPC_TYPE_BGP, pmsghdr->msg_subtype, pmsghdr->msg_index);
#endif
            if(ret < 0)
            {
                zlog_err("[%s %d] ret %d",__FUNCTION__,__LINE__,ret);
            }
        }
    }
	
    if(!bgp || flag == 0)
    {
        ipc_noack_to_snmp(pmsghdr);
    }
    return;
}

static void bgp_snmp_peer_get(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int i = 0;
    int ret = 0;
	int flag = 0;
	int msg_num = 0;
    afi_t afi = AFI_IP;
    safi_t safi = SAFI_UNICAST;
    struct bgp *bgp;
    struct peer *peer;
    struct listnode *node, *nnode;
	
    struct bgp_peer_key bgp_peer_last = *((struct bgp_peer_key*)pdata);
	
    bgp = bgp_get_default();
    if(bgp)
    {
        msg_num = IPC_BGP_LEN/sizeof(struct bgp_peer_key);
        struct bgp_peer_key bgp_peer[IPC_BGP_LEN/sizeof(struct bgp_peer_key)];
        memset(bgp_peer,0x0,msg_num*sizeof(struct bgp_peer_key));
		
        for(ALL_LIST_ELEMENTS (bgp->peer, node, nnode, peer))
        {
            if(bgp_peer_last.remote_ip.s_addr != 0 && (ntohl(bgp_peer_last.remote_ip.s_addr) >= ntohl(peer->su.sin.sin_addr.s_addr)))
            {
                continue;
            }
			
            flag = 1;
            bgp_peer[i].local_as = peer->local_as;
            bgp_peer[i].remote_ip = peer->su.sin.sin_addr;
            bgp_peer[i].source_ip = peer->su_local.sin.sin_addr;
            bgp_peer[i].remote_as = peer->as;
			
            if(peer->desc)
            {
                memcpy(bgp_peer[i].desc,peer->desc,80);
            }
			
            if(CHECK_FLAG(peer->flags, PEER_FLAG_ENABLE))
            {
                bgp_peer[i].enable = 1;
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_NEXTHOP_SELF))
            {
                bgp_peer[i].nexthop_change_local = 1;
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_REMOVE_PRIVATE_AS))
            {
                bgp_peer[i].exclude_private_as = 1;
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_IBGP_ROUTE))
            {
                bgp_peer[i].send_ibgp = 1;
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_DEFAULT_ORIGINATE))
            {
                bgp_peer[i].send_default = 1;
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_MAX_PREFIX))
            {
                bgp_peer[i].route_limit = 1;
                bgp_peer[i].route_max = peer->pmax[afi][safi];
            }
			
            if(CHECK_FLAG(peer->af_flags[afi][safi], PEER_FLAG_ALLOWAS_IN))
            {
                bgp_peer[i].allow_enable = 1;
                bgp_peer[i].allow_in = peer->allowas_in[afi][safi];
            }
			
            if(peer->sort == BGP_PEER_IBGP || peer->ttl == 1)
            {
                bgp_peer[i].ttl_enable = 0;
            }
            else
            {
                bgp_peer[i].ttl_enable = 1;
            }
			
			bgp_peer[i].ifindex_tnl = peer->ifindex_tnl;
			bgp_peer[i].vrf_id = peer->vrf_id;
			if(peer->afc[AFI_IP][SAFI_MPLS_VPN])
			{
				bgp_peer[i].vpnv4_enable = 1;
			}
			
            bgp_peer[i].ttl = peer->ttl;
            bgp_peer[i].routeadv = peer->routeadv;
            bgp_peer[i].weight = peer->weight;
            bgp_peer[i].change_local_as = peer->change_local_as;
            i ++;
			
            if(i == msg_num)
            {
                break;
            }
        }
        if(i == msg_num || flag)
        {
            ipc_ack_to_snmp(pmsghdr, bgp_peer, i*sizeof(struct bgp_peer_key), i);
#if 0          
            ret = ipc_send_reply_bulk(bgp_peer, i*sizeof(struct bgp_peer_key), i, pmsghdr->sender_id,
                                      MODULE_ID_BGP, IPC_TYPE_BGP, pmsghdr->msg_subtype, pmsghdr->msg_index);
#endif
            if(ret < 0)
            {
                zlog_err("[%s %d] ret %d",__FUNCTION__,__LINE__,ret);
            }
        }
    }
    if(! bgp|| flag == 0)
    {
       ipc_noack_to_snmp(pmsghdr);
    }
    return;
}

static void bgp_snmp_publish_get(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int i = 0;
	int msg_num = 0;
    struct bgp *bgp = NULL;  
	int flag = 0;
	struct bgp_table *table = NULL;
	struct bgp_node *rn = NULL;
	struct bgp_network *bgp_network = NULL;
	struct bgp_node *rn_temp = NULL;
	struct in_addr netmask; 
	
	struct bgp_publish_key bgp_publish_last = *((struct bgp_publish_key*)pdata);

	bgp = bgp_get_default();
    if(bgp)
    {
        msg_num = IPC_MSG_LEN/sizeof(struct bgp_publish_key);
        struct bgp_publish_key bgp_publish[IPC_MSG_LEN/sizeof(struct bgp_publish_key)];
        memset(bgp_publish, 0x0, msg_num*sizeof(struct bgp_publish_key));
		          					   
	    //for(m = 0; m < AFI_IP; m++)
		//{
           //for(n = 0; n < SAFI_UNICAST; n++)
		   //{   
		table = bgp->network[AFI_IP][SAFI_UNICAST]; 
        
		if(bgp_publish_last.p.u.prefix4.s_addr == 0)    /*first get*/
		{
             for (rn = bgp_table_top (table); rn; rn = bgp_route_next (rn))
	         {
                 if ((bgp_network = rn->info) != NULL)
                 {                                                         					            				  
                      flag = 1;
                      bgp_publish[i].local_as = bgp->as;
                      bgp_publish[i].p.u.prefix4 = rn->p.u.prefix4;
					  
					  struct in_addr netmask; 
					  masklen_to_netip (rn->p.prefixlen, &netmask);                    
                      bgp_publish[i].router_mask = netmask; 
                      bgp_publish[i].reserved = 0;
					  i++;
					  if(i == msg_num)
                      {
                         break;
                      }
                                                                                  
                 }   
             }  
		}
        else{
             rn_temp = bgp_node_lookup(table, &(bgp_publish_last.p));

		     if(rn_temp != NULL)
		     {
                 for (rn = rn_temp; rn; rn = bgp_route_next (rn))
	             {
                    if(rn == rn_temp)
				    {
					   continue;
					}
                    if ((bgp_network = rn->info) != NULL)
                    {                                                          					            				  
                         flag = 1;
                         bgp_publish[i].local_as = bgp->as;
                         bgp_publish[i].p.u.prefix4 = rn->p.u.prefix4;
					  					   
					     masklen_to_netip (rn->p.prefixlen, &netmask);                    
                         bgp_publish[i].router_mask = netmask; 

					     i++;
					     if(i == msg_num)
                         {
                            break;
                         }                                                                                   
                    }                       					
                 }
			 }          
		}                 				                                    				   			   		   				      	
		
		if(i == msg_num || flag)
        {   
            ipc_ack_to_snmp(pmsghdr, bgp_publish, i*sizeof(struct bgp_publish_key), i);
           
        }
		
    }
	
	if(! bgp|| flag == 0)
    {  
        ipc_noack_to_snmp(pmsghdr); 
    }
	
    return;
}

static void bgp_snmp_summary_get(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int flag = 0;
	int i = 0;
	int msg_num = 0;
    struct bgp *bgp = NULL;
	struct bgp_table *table = NULL;
	struct bgp_node *rn = NULL;
	struct bgp_node *rn_temp = NULL;
    struct bgp_aggregate *aggregate = NULL;
	struct in_addr netmask;
	
	struct bgp_summary_key bgp_summary_last = *((struct bgp_summary_key*)pdata);

	bgp = bgp_get_default();
    if(bgp)
    {
        msg_num = IPC_MSG_LEN/sizeof(struct bgp_summary_key);
        struct bgp_summary_key bgp_summary[IPC_MSG_LEN/sizeof(struct bgp_summary_key)];
        memset(bgp_summary, 0x0, msg_num*sizeof(struct bgp_summary_key));
		          					   
	    //for(m = 0; m < AFI_IP; m++)
		//{
          // for(n = 0; n < SAFI_UNICAST; n++)
		  // { 
		  
	    table = bgp->aggregate[AFI_IP][SAFI_UNICAST]; 
        if(bgp_summary_last.p.u.prefix4.s_addr == 0)    /*first get*/
		{
            for (rn = bgp_table_top (table); rn; rn = bgp_route_next (rn))
	        {
                if ((aggregate = rn->info) != NULL)
                {                                                                                                                                      
                    flag = 1;
                    bgp_summary[i].local_as = bgp->as;
                    bgp_summary[i].p.u.prefix4 = rn->p.u.prefix4;
					   					 
					masklen_to_netip (rn->p.prefixlen, &netmask);                    
                    bgp_summary[i].router_mask = netmask;                    
              
				    bgp_summary[i].as_set = aggregate->as_set;
					bgp_summary[i].summary_only = aggregate->summary_only;
					i++;
					if(i == msg_num)
                    {
                        break;
                    }                                                                                 
                 }   
            }
        }
        else{
             rn_temp = bgp_node_lookup(table, &(bgp_summary_last.p));

		     if(rn_temp != NULL)
		     {                  			 
                for (rn = rn_temp; rn; rn = bgp_route_next (rn))
	            {
                   if ((aggregate = rn->info) != NULL)
                   {         
                      if(rn == rn_temp)
				      {
					     continue;
					  }
                                                                                                                    
                      flag = 1;
                      bgp_summary[i].local_as = bgp->as;
                      bgp_summary[i].p.u.prefix4 = rn->p.u.prefix4;
					   					 
					  masklen_to_netip (rn->p.prefixlen, &netmask);                    
                      bgp_summary[i].router_mask = netmask;                       
              
				      bgp_summary[i].as_set = aggregate->as_set;
					  bgp_summary[i].summary_only = aggregate->summary_only;
					  i++;
					  if(i == msg_num)
                      {
                         break;
                      }
                                                                                   
                   }   
                }

		     }
		}     
                  				                                    				   			   		   				    		   		     		
		if(i == msg_num || flag)
        {  
            ipc_ack_to_snmp(pmsghdr, bgp_summary, i*sizeof(struct bgp_summary_key), i);
        }
    }
	
	if(! bgp|| flag == 0)
    {
        ipc_noack_to_snmp(pmsghdr);
    }
	
    return;
}

static void bgp_snmp_routes_get(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int i = 0;
	int msg_num = 0;
    struct bgp *bgp = NULL;  
	int flag = 0;
	struct bgp_table *table = NULL;
	struct bgp_node *rn = NULL;
	struct bgp_node *rn_temp = NULL;
	struct bgp_info *ri = NULL;
	struct in_addr netmask; 
	
	struct bgp_route_key bgp_route_last = *((struct bgp_route_key*)pdata);

	bgp = bgp_get_default();
    if(bgp)
    {
        msg_num = IPC_MSG_LEN/sizeof(struct bgp_route_key);
        struct bgp_route_key bgp_route[IPC_MSG_LEN/sizeof(struct bgp_route_key)];
        memset(bgp_route, 0x0, msg_num*sizeof(struct bgp_route_key));
	
	   
		table = bgp->rib[AFI_IP][SAFI_UNICAST]; 
        
        if(bgp_route_last.p.u.prefix4.s_addr == 0)    /*first get*/
		{
             for (rn = bgp_table_top (table); rn; rn = bgp_route_next (rn))
	         {
                 if (rn->info != NULL)
                 {                        
                     for (ri = rn->info; ri; ri = ri->next)
                     {                                                                                                  
                         flag = 1;
                           
                         bgp_route[i].p.u.prefix4 = rn->p.u.prefix4;
					     bgp_route[i].vpn_id = ri->vrf_id;
						   
					     masklen_to_netip (rn->p.prefixlen, &netmask);                    
                         bgp_route[i].router_mask = netmask; 
						 bgp_route[i].next_hop    = ri->attr->nexthop;
						 bgp_route[i].med         = ri->attr->med;
						 bgp_route[i].metric      = ri->attr->extra->weight;
						 bgp_route[i].preference  = ri->attr->local_pref;
                           
						 i++;
						 if(i == msg_num)
                         {
                            break;
                         }
                     }                                                               
                 }   
             }
                  				                                    				   			   		   				
         }		 
         else{
             rn_temp = bgp_node_lookup(table, &(bgp_route_last.p));

		     if(rn_temp != NULL)
		     {
                 for (rn = rn_temp; rn; rn = bgp_route_next (rn))
	             {  
                    if (rn->info != NULL)
                    {   
                       if(rn == rn_temp)
                       {
                          continue;
					   }
                       for (ri = rn->info; ri; ri = ri->next)
                       {                                                                                                  
                          flag = 1;
                           
                          bgp_route[i].p.u.prefix4 = rn->p.u.prefix4;
					      bgp_route[i].vpn_id = ri->vrf_id;
						   
					      masklen_to_netip (rn->p.prefixlen, &netmask);                    
                          bgp_route[i].router_mask = netmask; 
						  bgp_route[i].next_hop    = ri->attr->nexthop;
						  bgp_route[i].med         = ri->attr->med;
						  bgp_route[i].metric      = ri->attr->extra->weight;
						  bgp_route[i].preference  = ri->attr->local_pref;
                           
						  i++;
						  if(i == msg_num)
                          {
                             break;
                          }
                       }                                                               
                    }   
                }            
		     }
		}

        if(i == msg_num || flag)
        {   
            ipc_ack_to_snmp(pmsghdr, bgp_route, i*sizeof(struct bgp_route_key), i);    
        }
	 				
    }
	
	if(! bgp|| flag == 0)
    {
        ipc_noack_to_snmp(pmsghdr);
    }
	
    return;
}

static void bgp_snmp_manage(struct ipc_mesg_n *pMsg)
{
    //if(BGP_DEBUG(events, EVENTS))
    //{
        zlog_debug(BGP_DEBUG_TYPE_EVENTS,"%s[%d], subtype %d ",__FUNCTION__,__LINE__,pMsg->msghdr.msg_subtype);
    //}

	struct ipc_msghdr_n *pmsghdr = NULL;
	pmsghdr = &(pMsg->msghdr);
    if(!pmsghdr)
    {
        return;
    }
	
    switch(pMsg->msghdr.msg_subtype)
    {
	    case BGP_SNMP_GET_AS:
	    {
	        bgp_snmp_as_get(pmsghdr);
	        break;
	    }
	    case BGP_SNMP_GET_REDISTRIBUTE:
	    {
	        bgp_snmp_redistribute_get( pMsg->msg_data, pmsghdr);
	        break;
	    }
	    case BGP_SNMP_GET_PEER:
	    {
	        bgp_snmp_peer_get(pMsg->msg_data, pmsghdr);
	        break;
	    }
		case BGP_SNMP_GET_PUBLISH:
		{
            bgp_snmp_publish_get(pMsg->msg_data, pmsghdr);
			break;
		}
		case BGP_SNMP_GET_SUMMARY:
		{
            bgp_snmp_summary_get(pMsg->msg_data, pmsghdr);
            break;
		}
		case BGP_SNMP_GET_ROUTES:
		{
            bgp_snmp_routes_get(pMsg->msg_data, pmsghdr);
			break;
		}
		
	    default:
	    {
	        break;
	    }
    }
    return;
}

static void bgp_vrf_manage(struct ipc_mesg_n *pMsg)
{
	uint32_t vrf_id = 0;
	char * target = NULL;
	void *pdata = NULL;
	struct l3vpn_entry *l3vpn_info = NULL;

	pdata = pMsg->msg_data;
	switch (pMsg->msghdr.msg_subtype)
	{
		case L3VPN_SUBTYPE_LABEL:
		{
			if(IPC_OPCODE_DELETE == pMsg->msghdr.opcode)
			{
				l3vpn_info = (struct l3vpn_entry *)pdata;
				if(l3vpn_info)
				{
					bgp_vrf_manage_delete(l3vpn_info->l3vpn.vrf_id);
				}
			}
			break;
		}

		case L3VPN_SUBTYPE_RD:
		{			
			l3vpn_info = (struct l3vpn_entry *)pdata;
			if(l3vpn_info)
			{
				bgp_vrf_manage_rd(l3vpn_info);
			}
			break;
		}
		case L3VPN_SUBTYPE_TARGET:
		{	
			vrf_id = pMsg->msghdr.msg_index;			
			bgp_vrf_manage_target_delete(vrf_id);
			break;
		}
		case L3VPN_SUBTYPE_IMPORT:
		{
			vrf_id = pMsg->msghdr.msg_index;			
			target = (char *)pdata;
			if(target)
			{
				switch (pMsg->msghdr.opcode)
				{
					case IPC_OPCODE_ADD:
						bgp_vrf_manage_import_add(vrf_id,target);
						break;
					case IPC_OPCODE_DELETE:
						bgp_vrf_manage_import_delete(vrf_id,target);
						break;
					default:
						break;
				}
			}
			break;
		}
		case L3VPN_SUBTYPE_EXPORT:
		{	
			vrf_id = pMsg->msghdr.msg_index;			
			target = (char *)pdata;
			if(target)
			{
				switch (pMsg->msghdr.opcode)
				{
					case IPC_OPCODE_ADD:
						bgp_vrf_manage_export_add(vrf_id,target);
						break;
					case IPC_OPCODE_DELETE:
						bgp_vrf_manage_export_delete(vrf_id,target);
						break;
					default:
						break;
				}
			}
			break;
		}
		case L3VPN_SUBTYPE_APPLY:
		{			
			l3vpn_info = (struct l3vpn_entry *)pdata;
			if(l3vpn_info)
			{
				bgp_vrf_manage_apply_label(l3vpn_info);
			}
			break;
		}
		default:
		{
			break;
		}
	}
	return;
}

#if 0
static void bgp_common_manage(struct ipc_mesg mesg)
{
    void *pdata = NULL;
    struct ifm_event pevent;
    struct ipc_msghdr *pmsghdr = NULL;
	
    pmsghdr = &(mesg.msghdr);
    pdata = mesg.msg_data;
	
    if(pmsghdr->msg_type == IPC_TYPE_IFM)
    {
        if(pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
			memset(&pevent,0x0,sizeof(struct ifm_event));
            pevent = *((struct ifm_event*)pdata);
            bgp_ifm_manage(pevent, pmsghdr->msg_subtype);
        }
    }
    else if(pmsghdr->msg_type == IPC_TYPE_BGP)
    {
        if(pmsghdr->opcode == IPC_OPCODE_GET)
        {
            bgp_snmp_manage(pdata, pmsghdr);
        }
    }
    else if(pmsghdr->msg_type == IPC_TYPE_L3VPN)
    {
        bgp_vrf_manage(pdata, pmsghdr);
    }
	
    return;
}
#endif

void
bgp_route_msg_rcv_delete()
{
    struct listnode *pnode = NULL;
    struct list *plist = &bm->route_rcv;	
    struct ipc_mesg_n *pmesg_new = NULL;
	
	if(list_isempty(plist))
	{
		return;
	}
	
	while(1)
	{
		pnode = listhead(plist);
		if ((NULL == pnode) || (NULL == pnode->data))
		{
			break;
		}
		
        pmesg_new = pnode->data;
        list_delete_node(plist, pnode);
        XFREE(MTYPE_BGP_ROUTE_RCV_INFO, pmesg_new);
		pmesg_new = NULL;
	}
	
	return;

}

void
bgp_common_msg_rcv_delete()
{
    struct listnode *pnode = NULL;
    struct list *plist = &bm->com_rcv;	
    struct ipc_mesg_n *pmesg_new = NULL;
	
	if(list_isempty(plist))
	{
		return;
	}
	
	while(1)
	{
		pnode = listhead(plist);
		if ((NULL == pnode) || (NULL == pnode->data))
		{
			break;
		}
		
        pmesg_new = pnode->data;
        list_delete_node(plist, pnode);
        XFREE(MTYPE_BGP_COMMON_RCV_INFO, pmesg_new);
		pmesg_new = NULL;
	}
	
	return;

}

#if 0
void bgp_route_msg_rcv(void *arg)
{
	int ret = 0;
	int msg_num = 0;
	struct ipc_mesg mesg;
	struct ipc_mesg *newmesg;
    struct list *plist = &bm->route_rcv;
	
    prctl(PR_SET_NAME, "bgp_recv_route");
	
	while(1)
	{
		memset(&mesg,0,sizeof(struct ipc_mesg));

		if(ipc_route_id < 0)
	    {
			ipc_route_id = ipc_connect(IPC_MSG_ROUTE);
	        if(ipc_route_id < 0)
	        {
	            zlog_err("%s[%d]: ipc connect failed!\n", __func__, __LINE__);
	            return;
	        }
	    }
		
		memset(&mesg, 0, sizeof(struct ipc_mesg));

		ret = msgrcv(ipc_route_id, &mesg, IPC_BGP_LEN+IPC_HEADER_LEN-4, MODULE_ID_BGP, 0);
	    if (ret == -1)
		{
			continue;
		}

		pthread_mutex_lock(&bm->route_rcv_lock);

		newmesg = XCALLOC (MTYPE_BGP_ROUTE_RCV_INFO, sizeof(struct ipc_mesg));
		if (newmesg == NULL)
		{
			zlog_err("%s[%d]: memory xcalloc error!",__func__, __LINE__);
			pthread_mutex_unlock(&bm->route_rcv_lock);
			continue;
		}
		
		memset(newmesg, 0, sizeof(struct ipc_mesg));
		memcpy(newmesg, &mesg, sizeof(struct ipc_mesg));
		listnode_add(plist, newmesg);

		pthread_mutex_unlock(&bm->route_rcv_lock);
		
        if (msg_num++ == 200)
        {
            msg_num = 0;
            usleep(1000);
        }
	}
	return;
}

void bgp_common_msg_rcv(void *arg)
{
	int ret = 0;
	int msg_num = 0;
	struct ipc_mesg mesg;
	struct ipc_mesg *newmesg;
    struct list *plist = &bm->com_rcv;
	
    prctl(PR_SET_NAME, "bgp_recv_common");
	
	while(1)
	{
		memset(&mesg,0,sizeof(struct ipc_mesg));

		if(ipc_common_id < 0)
	    {
			ipc_connect_common();
	        if(ipc_common_id < 0)
	        {
	            zlog_err("%s[%d]: ipc connect failed!\n", __func__, __LINE__);
	            return;
	        }
	    }
		
		memset(&mesg, 0, sizeof(struct ipc_mesg));

		ret = msgrcv(ipc_common_id, &mesg, IPC_BGP_LEN+IPC_HEADER_LEN-4, MODULE_ID_BGP, 0);
	    if (ret == -1)
		{
			continue;
		}

		pthread_mutex_lock(&bm->com_rcv_lock);

		newmesg = XCALLOC (MTYPE_BGP_COMMON_RCV_INFO, sizeof(struct ipc_mesg));
		if (newmesg == NULL)
		{
			zlog_err("%s[%d]: memory xcalloc error!",__func__, __LINE__);
			pthread_mutex_unlock(&bm->com_rcv_lock);
			continue;
		}
		
		memset(newmesg, 0, sizeof(struct ipc_mesg));
		memcpy(newmesg, &mesg, sizeof(struct ipc_mesg));
		listnode_add(plist, newmesg);

		pthread_mutex_unlock(&bm->com_rcv_lock);
		
        if (msg_num++ == 200)
        {
            msg_num = 0;
            usleep(1000);
        }
	}
	return;
}

/*receive route process*/
static int bgp_route_msg_rcv_process()
{	
    int token = 200;
	struct ipc_mesg mesg;
    struct listnode *pnode = NULL;
    struct ipc_mesg *newmesg = NULL;
    struct list *plist = &bm->route_rcv;

	while(token)
	{
		token --;
		
		pthread_mutex_lock(&bm->route_rcv_lock);
		if(list_isempty(plist))
		{
			pthread_mutex_unlock(&bm->route_rcv_lock);
			continue;
		}
		
        pnode = listhead(plist);
        if ((NULL == pnode) || (NULL == pnode->data))
    	{
			pthread_mutex_unlock(&bm->route_rcv_lock);
			continue;
    	}
		
        newmesg = pnode->data;
		memset(&mesg, 0 ,sizeof(struct ipc_mesg));
        memcpy(&mesg, newmesg, sizeof(struct ipc_mesg));
        list_delete_node(plist, pnode);
        XFREE(MTYPE_BGP_ROUTE_RCV_INFO, newmesg);
		newmesg = NULL;
		pthread_mutex_unlock(&bm->route_rcv_lock);

		bgp_route_manage(mesg);
	}

	return 0;
}

static int bgp_common_msg_rcv_process()
{	
    int token = 200;
	struct ipc_mesg mesg;
    struct listnode *pnode = NULL;
    struct ipc_mesg *newmesg = NULL;
    struct list *plist = &bm->com_rcv;

	while(token)
	{
		token --;
		
		pthread_mutex_lock(&bm->com_rcv_lock);
		if(list_isempty(plist))
		{
			pthread_mutex_unlock(&bm->com_rcv_lock);
			continue;
		}
		
        pnode = listhead(plist);
        if ((NULL == pnode) || (NULL == pnode->data))
    	{
			pthread_mutex_unlock(&bm->com_rcv_lock);
			continue;
    	}
		
        newmesg = pnode->data;
		memset(&mesg, 0 ,sizeof(struct ipc_mesg));
        memcpy(&mesg, newmesg, sizeof(struct ipc_mesg));
        list_delete_node(plist, pnode);
        XFREE(MTYPE_BGP_COMMON_RCV_INFO, newmesg);
		newmesg = NULL;
		pthread_mutex_unlock(&bm->com_rcv_lock);

		bgp_common_manage(mesg);
	}

	return 0;
}

int bgp_msg_rcv(struct thread *thread)
{	
    bgp_common_msg_rcv_process();
    bgp_route_msg_rcv_process();
	
    usleep(1000);
	
    thread_add_event (bm->master, bgp_msg_rcv, NULL, 0 );
	
    return ERRNO_SUCCESS;
}
#endif

int bgp_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;
    int revln = 0;

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        switch(pmsg->msghdr.msg_type)
        {    
            case IPC_TYPE_ROUTE:                   
                 bgp_route_manage (pmsg);         
		         break;
			
            case IPC_TYPE_IFM:
                 bgp_ifm_manage(pmsg);
                 break;
			
			case IPC_TYPE_BGP:
				 bgp_snmp_manage(pmsg); 
				 break;
			
		    case IPC_TYPE_L3VPN:
                 bgp_vrf_manage(pmsg);
                 break;
			
			case IPC_TYPE_PACKET:
				 bgp_pkt_rcv(pmsg);
				 break;
			
            default:            
                 break;
        }
    }

    mem_share_free(pmsg, MODULE_ID_BGP);
  
    return retva;
}


/*Send msg to tcp to control the tcp is connect or not.(tcp dport is 179)*/
int bgp_used_rfcnt = 0;
int bgp_sendmsg_to_tcp(bgp_msg_type_t flag,struct peer *peer)
{
	int ret = -1;
	struct peer_entry temp_entry;
	int data_len = sizeof(struct peer_entry);
	
	switch(flag)
	{
		case BGP_ENABLE_MSG:
			if(bgp_used_rfcnt == 1)
				goto drop;
			bgp_used_rfcnt = 1;
			break;
		case BGP_DISABLE_MSG:
			if(bgp_used_rfcnt == 0)
				goto drop;
			bgp_used_rfcnt = 0;
			break;
		case BGP_SET_PASSWD_MSG:
	        memset(&temp_entry, 0, data_len);
	        if(peer->su_local.sin.sin_addr.s_addr)
	        {
	            temp_entry.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
	        }
	        temp_entry.dip = ntohl(peer->su.sin.sin_addr.s_addr);
			memcpy(temp_entry.pass,peer->password,strlen(peer->password));
			
			break;
		case BGP_UNSET_PASSWD_MSG:
			memset(&temp_entry, 0, data_len);
	        if(peer->su_local.sin.sin_addr.s_addr)
	        {
	            temp_entry.sip = ntohl(peer->su_local.sin.sin_addr.s_addr);
	        }
	        temp_entry.dip = ntohl(peer->su.sin.sin_addr.s_addr);
			break;
		default:
			goto drop;
	}

	/*id = ipc_connect(IPC_MSG_FTM);
	msghdr.data_len = data_len;
	msghdr.module_id = MODULE_ID_FTM;
	msghdr.msg_type = IPC_TYPE_BGP;
	msghdr.msg_subtype = flag;
	msghdr.msg_index = 0;
	msghdr.data_num = 1;
	msghdr.opcode = IPC_OPCODE_EVENT;
	msghdr.sender_id = MODULE_ID_BGP;
    ret = ipc_send(id, &msghdr, &temp_entry);*/
	
	ret = ipc_send_msg_n2(&temp_entry,data_len,1,MODULE_ID_FTM,MODULE_ID_BGP,IPC_TYPE_BGP,flag,IPC_OPCODE_EVENT,0);
	if(ret < 0)
	{
		usleep(10000);
	}
	
	return 0;
	
drop:
	return -1;
}

#if 0
void bgp_route_msg_rcv_init()
{
	pthread_t route_rcv;
	
	pthread_mutex_init(&bm->route_rcv_lock, NULL);

    pthread_create(&route_rcv, NULL, (void *)bgp_route_msg_rcv, NULL);

    pthread_detach(route_rcv);
	
	return;
}

void bgp_common_msg_rcv_init()
{
	pthread_t common_rcv;
	
	pthread_mutex_init(&bm->com_rcv_lock, NULL);

    pthread_create(&common_rcv, NULL, (void *)bgp_common_msg_rcv, NULL);

    pthread_detach(common_rcv);
	
	return;
}
#endif

