/* protocol library.
   ospf register to itm and route, prepare to recive message from ifm,
   ifm: interface link
   route: interface ip and route
 */
#include "lib/stream.h"
#include "lib/log.h"
#include "lib/thread.h"
#include "lib/memory.h"
#include "lib/errcode.h"
#include "lib/ifm_common.h"
#include "lib/memtypes.h"
#include "lib/mpls_common.h"
#include "lib/memshare.h"
#include "lib/snmp_common.h"


#include "ospfd/ospf_ipc.h"
#include "ospfd/ospf_packet.h"
#include "ospfd/ospf_zebra.h"
#include "ospfd/ospf_interface.h"
#include "ospfd/ospf_asbr.h"
#include "ospfd/ospf_abr.h"
#include "ospfd/ospf_msg.h"
#include "ospfd/ospf_lsa.h"
#include "ospfd/ospf_lsdb.h"
#include "ospfd/ospf_bfd.h"
#include "ospfd/ospf_opaque.h"
#include "ospfd/ospf_dump.h"
#include "ospfd/ospf_nsm.h"
#include "ospfd/ospf_neighbor.h"


//extern struct ospf_statics ospf_sta;
//extern struct thread_master *master_ospf ;

/*******************************************************************************************

*Function: 	send ipc msg to route: multiple route item
*Parameter:	struct route_entry* route ,enum IPC_OPCODE opcode, unsigned int item
*return: 	ret 

********************************************************************************************/
static int
ospf_send_multiplemessage_route(struct route_entry *route, enum IPC_OPCODE opcode, unsigned int item)
{
    /*int ret = ipc_send_route(route, sizeof(struct route_entry)*item, item, MODULE_ID_ROUTE, MODULE_ID_OSPF,
                             IPC_TYPE_ROUTE, 0, opcode);*/
	int ret = ipc_send_msg_n2(route, sizeof(struct route_entry)*item,  item, MODULE_ID_ROUTE, 
				    			MODULE_ID_OSPF, IPC_TYPE_ROUTE, 0, opcode, 0);
    if(ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:route send ip:%d",__FUNCTION__, __LINE__,route[0].prefix.addr.ipv4);
        return -1;
    }
    return ret;
}

/*******************************************************************************************

*Function: 	send ipc msg to route: multiple route item
*Parameter:	struct route_entry* route ,enum IPC_OPCODE opcode, unsigned int item
*return: 	ret 

********************************************************************************************/
int ospf_asyc_send_route_cmd (void *threadval)
{
    int i = 0;
	int ret = 0;
	int item = 0;   /*  一条IPC msg 承载路由条目个数*/
	int msg_num = IPC_OSPF_LEN/sizeof(struct route_entry);
	int send_or_count = 0;
    struct fifo *pfifo = NULL;
    unsigned int opcodeflag = 0;
	struct routefifo *routefifo_entry[msg_num];
    struct route_entry route[msg_num];
    
	
    om->t_thread_routefifo = 0;
    for (item = 0; item < msg_num; item++)
    {
        routefifo_entry[item] = NULL;
    }
    
    while (!FIFO_EMPTY(&om->routefifo))
    {
        if (send_or_count >= 2000)
        {
            break;
        }
        
        pfifo = &om->routefifo;
        routefifo_entry[0] = (struct routefifo *)FIFO_TOP(pfifo);
        opcodeflag = routefifo_entry[0]->opcode;
        pfifo = &(routefifo_entry[0]->fifo);
        memcpy(&route[0], &routefifo_entry[0]->route, sizeof(struct route_entry));
        item = 1;
		//while (FIFO_HEAD(pfifo) && item < msg_num)
		while ((pfifo->next != (struct fifo *)&om->routefifo) && item < msg_num)
        {
            routefifo_entry[item] = (struct routefifo *)FIFO_TOP(pfifo);
            if (opcodeflag != routefifo_entry[item]->opcode)
            {
                break;
            }
            memcpy(&route[item], &routefifo_entry[item]->route, sizeof(struct route_entry));
            pfifo = &(routefifo_entry[item]->fifo);
            item++;
        }
		
        ret = ospf_send_multiplemessage_route(route, opcodeflag, item);
        if (ret < 0)
        {
            break;
        }
        else
        {
            send_or_count = send_or_count + item;
        }
		
        for (i = 0; i < item; i++)
        {
            FIFO_DEL(routefifo_entry[i]);
            XFREE (MTYPE_ROUTE_FIFO, routefifo_entry[i]);
			routefifo_entry[i] = NULL;
        }
    }
    /*om->t_thread_routefifo =
        thread_add_timer_msec (master_ospf, ospf_asyc_send_route_cmd, NULL, 1000);*/
	om->t_thread_routefifo =
		high_pre_timer_add ((char *)"ospf_asyc_route_timer", LIB_TIMER_TYPE_NOLOOP, ospf_asyc_send_route_cmd, NULL, 1000);
    return 0;
}

static
int ospf_pkt_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
	int msg_block_len = 0;
	
	if ( NULL == pmsg || 0 == imlen )
	{
		zlog_debug(OSPF_DBG_RECV, "%s[%d]: In function '%s': OSPF recv msg: pmsg is NULL.\r\n",\
									 __FILE__, __LINE__, __func__);
		return ERRNO_FAIL;
	}
	
	zlog_debug(OSPF_DBG_RECV, "%s[%d]: Ospf recv msg: pmsg=%p, imlen=%d msg_type:%d msg_subtype:%d\r\n", \
					__FUNCTION__, __LINE__, pmsg, imlen, pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype);

	msg_block_len = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N;
	if(msg_block_len <= imlen)
	{
		switch ( pmsg->msghdr.msg_type )
		{
			case IPC_TYPE_PACKET:
				ospf_pkt_rcv_new(pmsg);
				break;

			case IPC_TYPE_ROUTE:
				ospf_route_manage_new(pmsg);
				break;
			
			default:
				ospf_common_manage_new(pmsg);
				break;
		}
	}
	else
	{
		zlog_debug(OSPF_DBG_RECV, "OSPF recv msg: datalen error, data_len=%d, msgrcv len = %d\n", msg_block_len, imlen);
	}

	if(pmsg)
	{
		mem_share_free(pmsg, MODULE_ID_OSPF);
	}
	
	return ERRNO_SUCCESS;
}


/* ifm register */
static void ospf_ifm_register()
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MTU_CHANGE, MODULE_ID_OSPF, IFNET_IFTYPE_L3IF);
	mpls_com_l3vpn_event_register(MODULE_ID_OSPF);
}


/* redistribute route type. */
void
ospf_init (void)
{
	if(ipc_recv_thread_start((char *)"OspfMsgRev", MODULE_ID_OSPF, SCHED_OTHER, -1, ospf_pkt_msg_rcv, 0) == -1)
    {
        printf("Ospf msg receive thread start fail\r\n");
        exit(0);
    }

	ospf_pkt_register();
    ospf_ifm_register();
}

/*
 * send a register or unregister message to Module ROUTE
 * for the route type (ROUTE_PROTO_STATIC etc.). The Module ROUTE will
 * then set/unset redist[type] in the client handle (a struct zserv) for the
 * sending client
 */
int
ospf_redistribute_send (int command, struct ospf *ospf, int type,
                        vrf_id_t vrf_id)
{
    if(command == ZEBRA_REDISTRIBUTE_ADD)
    {
        if(om->redis_count[type] == 1)
        {
            route_event_register(type, MODULE_ID_OSPF);
        }
        else
        {
            ospf_schedule_asbr_task (ospf);
        }
    }
    else if(command == ZEBRA_REDISTRIBUTE_DELETE)
    {
        if(om->redis_count[type] == 0)
        {
            route_event_unregister(type, MODULE_ID_OSPF);
        }
    }
    return 0;
}

void
ospf_redistribute (int command, struct ospf *ospf, int type, int instance,
                   vrf_id_t vrf_id)
{
    if (command == ZEBRA_REDISTRIBUTE_ADD)
    {
        if (vrf_bitmap_check (ospf->dmetric[type][instance].redist, vrf_id))
        {
            return;
        }
        vrf_bitmap_set (ospf->dmetric[type][instance].redist, vrf_id);
        om->redis_count[type]++;
    }
    else
    {
        if (!vrf_bitmap_check (ospf->dmetric[type][instance].redist, vrf_id))
        {
            return;
        }
        vrf_bitmap_unset (ospf->dmetric[type][instance].redist, vrf_id);
        om->redis_count[type]--;
    }
    ospf_redistribute_send (command, ospf, type, vrf_id);
}


void
ospf_redistribute_default (int command, struct ospf *ospf,
                           vrf_id_t vrf_id)
{
    if (command == ZEBRA_REDISTRIBUTE_DEFAULT_ADD)
    {
        if (vrf_bitmap_check (ospf->default_information, vrf_id))
        {
            return;
        }
        vrf_bitmap_set (ospf->default_information, vrf_id);
    }
    else
    {
        if (!vrf_bitmap_check (ospf->default_information, vrf_id))
        {
            return;
        }
        vrf_bitmap_unset (ospf->default_information, vrf_id);
    }
    //route_message_send (ospf_buf, command, vrf_id);
}


static int
ospf_snmp_iftype_h3c (struct interface *ifp)
{
#define ospf_snmp_iftype_broadcast         1
#define ospf_snmp_iftype_nbma              2
#define ospf_snmp_iftype_pointToPoint      3
#define ospf_snmp_iftype_pointToMultipoint 5
    if (OSPF_IFTYPE_BROADCAST == IF_DEF_PARAMS(ifp)->type)
    {
        return ospf_snmp_iftype_broadcast;
    }
    if (OSPF_IFTYPE_POINTOPOINT == IF_DEF_PARAMS(ifp)->type)
    {
        return ospf_snmp_iftype_pointToPoint;
    }
    if (OSPF_IFTYPE_POINTOMULTIPOINT == IF_DEF_PARAMS(ifp)->type)
    {
        return ospf_snmp_iftype_pointToMultipoint;
    }
    
    return ospf_snmp_iftype_nbma;
}

static int32_t
ospf_snmp_neighbor_state_h3c(u_char nst)
{
    switch (nst)
    {
    case NSM_Attempt:
        return 2;
    case NSM_Init:
        return 3;
    case NSM_TwoWay:
        return 4;
    case NSM_ExStart:
        return 5;
    case NSM_Exchange:
        return 6;
    case NSM_Loading:
        return 7;
    case NSM_Full:
        return 8;
    default:
        return 1; /* down */
    }
}


static int
static_ospf_get_bulk(void *pdata, int data_len, struct statics_ospf sospf_buff[])
{
    u_int32_t ospf_id  = *((u_int32_t *)pdata);
    struct statics_ospf s_ospf;
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
    struct listnode *nnode = NULL;
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf);
    int data_num = 0;
	
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	
    if(ospf_id == 0)/* 1st get */
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            memset(&s_ospf, 0, sizeof(struct statics_ospf));
            s_ospf.ospf_id = ospf->ospf_id;
            s_ospf.router_id.s_addr = ospf->router_id.s_addr;
            s_ospf.vpn = ospf->vpn;
			if(CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
			{
				s_ospf.rfc1583_conf = 1;
			}
			else
			{
				s_ospf.rfc1583_conf = 2;
			}
			if(CHECK_FLAG(ospf->config, OSPF_OPAQUE_CAPABLE))
			{
				s_ospf.opaque_lsa_conf = 1;
			}
			else
			{
				s_ospf.opaque_lsa_conf = 2;
			}
			s_ospf.lsdb_refresh_interval = ospf->refresh_interval;
			s_ospf.ref_bandwidth = ospf->ref_bandwidth;
			s_ospf.manger_distance = ospf->distance_all;
			s_ospf.ase_distance = ospf->distance_external;
			
            memcpy(&sospf_buff[data_num++], &s_ospf, sizeof(struct statics_ospf));
            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            if(ospf->ospf_id > ospf_id)
            {
                memset(&s_ospf, 0, sizeof(struct statics_ospf));
                s_ospf.ospf_id = ospf->ospf_id;
                s_ospf.router_id.s_addr = ospf->router_id.s_addr;
                s_ospf.vpn = ospf->vpn;
				if(CHECK_FLAG(ospf->config, OSPF_RFC1583_COMPATIBLE))
				{
					s_ospf.rfc1583_conf = 1;
				}
				else
				{
					s_ospf.rfc1583_conf = 2;
				}
				if(CHECK_FLAG(ospf->config, OSPF_OPAQUE_CAPABLE))
				{
					s_ospf.opaque_lsa_conf = 1;
				}
				else
				{
					s_ospf.opaque_lsa_conf = 2;
				}
				s_ospf.lsdb_refresh_interval = ospf->refresh_interval;
				s_ospf.ref_bandwidth = ospf->ref_bandwidth;
				s_ospf.manger_distance = ospf->distance_all;
				s_ospf.ase_distance = ospf->distance_external;
				
                memcpy(&sospf_buff[data_num++], &s_ospf, sizeof(struct statics_ospf));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    return data_num;
}

static int
static_ospf_area_get_bulk(void *pdata, int data_len, struct statics_ospf_area sarea_buff[])
{
    struct ospf_key ospf_key = *((struct ospf_key*)pdata);
    struct statics_ospf_area  s_area;
    struct ospf *ospf = NULL;
    struct ospf_area *area = NULL;
    struct listnode *node = NULL,
                    *node1 = NULL,
                    *nnode = NULL;
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_area);
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
    if(ospf_key.ospf_id == 0)   /* 1st get */
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
            {
                memset(&s_area, 0, sizeof(struct statics_ospf_area));
                s_area.ospf_id = ospf->ospf_id;
                s_area.area_id = ntohl(area->area_id.s_addr);
                s_area.area_type = area->external_routing;
                if (NULL != area->area_name)
                {
                    memcpy(s_area.name, area->area_name, strlen(area->area_name));
                }
                s_area.summary = area->no_summary;
                s_area.cost = area->default_cost;
                memcpy(&sarea_buff[data_num++], &s_area, sizeof(struct statics_ospf_area));
                if (data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            if(ospf->ospf_id == ospf_key.ospf_id)
            {
                for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
                {
                    if (ntohl(area->area_id.s_addr) > ospf_key.area_id)
                    {
                        memset(&s_area, 0, sizeof(struct statics_ospf_area));
                        s_area.ospf_id = ospf->ospf_id;
                        s_area.area_id = ntohl(area->area_id.s_addr);
                        s_area.area_type = area->external_routing;
                        if (NULL != area->area_name)
                        {
                            memcpy(s_area.name, area->area_name, strlen(area->area_name));
                        }
                        s_area.summary = area->no_summary;
                        s_area.cost = area->default_cost;
                        memcpy(&sarea_buff[data_num++], &s_area, sizeof(struct statics_ospf_area));
                        if (data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                }
            }
            else if(ospf->ospf_id > ospf_key.ospf_id)
            {
                for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
                {
                    memset(&s_area, 0, sizeof(struct statics_ospf_area));
                    s_area.ospf_id = ospf->ospf_id;
                    s_area.area_id = ntohl(area->area_id.s_addr);
                    s_area.area_type = area->external_routing;
                    if (NULL != area->area_name)
                    {
                        memcpy(s_area.name, area->area_name, strlen(area->area_name));
                    }
                    s_area.summary = area->no_summary;
                    s_area.cost = area->default_cost;
                    memcpy(&sarea_buff[data_num++], &s_area, sizeof(struct statics_ospf_area));
                    if (data_num == msg_num)
                    {
                        return data_num;
                    }
                }
            }
        }
    }
    return data_num;
}
#if 0
static int
static_ospf_network_get_bulk(void *pdata, int data_len, struct statics_ospf_network snetwork_buff[])
{
    struct ospf_key_network ospf_key = *((struct ospf_key_network*)pdata);
    struct statics_ospf_network  s_network;
    struct ospf *ospf = NULL;
    struct route_node *rn;	
    struct route_node *rn1;	
    struct route_node *rn_temp;
    struct listnode *node = NULL,
                    *nnode = NULL,
                    *node1 = NULL;
    struct ospf_area *area = NULL;
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_network);
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	zlog_debug("%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,ospf_key.ospf_id,
	ospf_key.area_id,inet_ntoa(ospf_key.p.u.prefix4),ospf_key.p.prefixlen);
	
    if(ospf_key.ospf_id == 0)   /*  1st get */
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {  
        	for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
        	{
	            for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
	            {
	                if (rn->info)
	                {
	                    struct ospf_network *n = rn->info;
						if(n->area_id.s_addr == area->area_id.s_addr)
						{
		                    memset(&s_network, 0, sizeof(struct statics_ospf_network));
		                    s_network.ospf_id = ospf->ospf_id;
		                    s_network.area_id = ntohl(n->area_id.s_addr);
		                    s_network.p.family = AF_INET;
		                    s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
		                    s_network.p.prefixlen= rn->p.prefixlen;
							zlog_debug("%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
										s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
		                    memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
		                    if (data_num == msg_num)
		                    {
		                        return data_num;
		                    }
						
						}
	                }
	            }
        	}
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            if(ospf_key.ospf_id == ospf->ospf_id)
            {         
            	/* look up rm_temp by key */
				rn_temp = route_node_lookup(ospf->networks, &(ospf_key.p));
				
                for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
                {
                    if (rn->info)
                    {
                        struct ospf_network *n = rn->info;
						
						if(ntohl(n->area_id.s_addr) == ospf_key.area_id)
                        {
							if(rn_temp != NULL)
							{
								for (rn1 = rn_temp; rn1; rn1 = route_next (rn1))
								{
									if(rn1 == rn_temp)
									{
										continue;
									}
									
								    if (rn1->info)
				                    {
				                        struct ospf_network *n1 = rn1->info;
										
										if(ntohl(n1->area_id.s_addr) == ospf_key.area_id)
										{
											memset(&s_network, 0, sizeof(struct statics_ospf_network));
											s_network.ospf_id = ospf->ospf_id;
											s_network.area_id = ntohl(n1->area_id.s_addr);
											s_network.p.family = AF_INET;
											s_network.p.u.prefix4.s_addr = rn1->p.u.prefix4.s_addr;
											s_network.p.prefixlen = rn1->p.prefixlen;
											zlog_debug("%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
												s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
											memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
											if (data_num == msg_num)
											{
												return data_num;
											}
										}
									}
								}
									
							}
                        }                        
						else if(ntohl(n->area_id.s_addr) > ospf_key.area_id)
                        {
                            memset(&s_network, 0, sizeof(struct statics_ospf_network));
                            s_network.ospf_id = ospf->ospf_id;
                            s_network.area_id = ntohl(n->area_id.s_addr);
                            s_network.p.family = AF_INET;
                            s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
                            s_network.p.prefixlen = rn->p.prefixlen;
							zlog_debug("%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);

                            memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
                            if (data_num == msg_num)
                            {
                                return data_num;
                            }
                        }
                    }
                }
            }
            else if(ospf->ospf_id > ospf_key.ospf_id)
            {
                for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
                {
                    if (rn->info)
                    {
                        struct ospf_network *n = rn->info;
                        memset(&s_network, 0, sizeof(struct statics_ospf_network));
                        s_network.ospf_id = ospf->ospf_id;
                        s_network.area_id = ntohl(n->area_id.s_addr);
                        s_network.p.family = AF_INET;
                        s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
                        s_network.p.prefixlen = rn->p.prefixlen;
						zlog_debug("%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
                        memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
                        if (data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                }
            }
        }
    }
    return data_num;
}
#endif

static int
static_ospf_network_get_bulk(void *pdata, int data_len, struct statics_ospf_network snetwork_buff[])
{
    struct ospf_key_network ospf_key = *((struct ospf_key_network*)pdata);
    struct statics_ospf_network  s_network;
    struct ospf *ospf = NULL;
    struct route_node *rn;	
    struct route_node *rn1;	
    struct route_node *rn_temp;
    struct listnode *node = NULL,
                    *nnode = NULL;
    //struct ospf_area *area = NULL;
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_network);
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,ospf_key.ospf_id,
	ospf_key.area_id,inet_ntoa(ospf_key.p.u.prefix4),ospf_key.p.prefixlen);
	
    if(ospf_key.ospf_id == 0)   /*  1st get */
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {  
	        for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
            {
                if (rn->info)
                {
                    struct ospf_network *n = rn->info;
					
                    memset(&s_network, 0, sizeof(struct statics_ospf_network));
                    s_network.ospf_id = ospf->ospf_id;
                    s_network.area_id = ntohl(n->area_id.s_addr);
                    s_network.p.family = AF_INET;
                    s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
                    s_network.p.prefixlen= rn->p.prefixlen;
					zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
                    memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
                    if (data_num == msg_num)
                    {
                        return data_num;
                    }
					
					
                }
            }
        	
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
        {
            if(ospf_key.ospf_id == ospf->ospf_id)
            {         
            	/* look up rn_temp by key */
				rn_temp = route_node_lookup(ospf->networks, &(ospf_key.p));

				if(rn_temp != NULL)
				{
					for (rn1 = rn_temp; rn1; rn1 = route_next (rn1))
					{
						if(rn1 == rn_temp)
						{
							continue;
						}
						
					    if (rn1->info)
	                    {
	                        struct ospf_network *n1 = rn1->info;
							
							memset(&s_network, 0, sizeof(struct statics_ospf_network));
							s_network.ospf_id = ospf->ospf_id;
							s_network.area_id = ntohl(n1->area_id.s_addr);
							s_network.p.family = AF_INET;
							s_network.p.u.prefix4.s_addr = rn1->p.u.prefix4.s_addr;
							s_network.p.prefixlen = rn1->p.prefixlen;
							zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
							memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
							if (data_num == msg_num)
							{
								return data_num;
							}
							
						}
					}
						
				}
            }
            else if(ospf->ospf_id > ospf_key.ospf_id)
            {
                for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
                {
                    if (rn->info)
                    {
                        struct ospf_network *n = rn->info;
                        memset(&s_network, 0, sizeof(struct statics_ospf_network));
                        s_network.ospf_id = ospf->ospf_id;
                        s_network.area_id = ntohl(n->area_id.s_addr);
                        s_network.p.family = AF_INET;
                        s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
                        s_network.p.prefixlen = rn->p.prefixlen;
						zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
                        memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
                        if (data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                }
            }
        }
    }
    return data_num;
}

#define IP_MASK_FORMAT		(1)

static int
statistics_ospf_abr_summary_get_bulk(void *pdata, int data_len, struct statistics_ospf_abr_summary_info abr_sum_buff[])
{
    struct ospf_abr_sum_info_key ospf_key = *((struct ospf_abr_sum_info_key*)pdata);
    struct statistics_ospf_abr_summary_info  s_abr_sum_info;
	struct ospf_area_range *area_range = NULL;
    struct ospf *ospf = NULL;
    struct route_node *rn;	
    struct prefix_ipv4 temp_p;	
    struct route_node *rn_temp;
    struct listnode *node = NULL;
    struct listnode *node1 = NULL;
    struct ospf_area *area = NULL;
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statistics_ospf_abr_summary_info);
    //int msg_num  = 2;//pressure test
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
#if IP_MASK_FORMAT
	zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n",__func__,__LINE__,ospf_key.ospf_id_key,
	ospf_key.area_id_key,inet_ntoa(ospf_key.abr_sum_ip_addr),ip_masklen(ospf_key.abr_sum_ip_mask));
#else
	zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__, ospf_key.ospf_id_key,
	ospf_key.area_id_key, inet_ntoa(ospf_key.p_key.u.prefix4), ospf_key.p_key.prefixlen);
#endif
    if((ospf_key.ospf_id_key == 0) && (ospf_key.area_id_key == 0))   /*  1st get */
    {
        for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
        {  
	        for (ALL_LIST_ELEMENTS_RO(ospf->areas, node1, area))
            {
				for (rn = route_top(area->ranges); rn; rn = route_next(rn))
				{
					if (rn->info)
					{
						area_range = rn->info;
#if IP_MASK_FORMAT
						if(data_num != 0 
							&& (abr_sum_buff[data_num-1].abr_sum_ip_addr.s_addr == rn->p.u.prefix4.s_addr)
							&& (ip_masklen(abr_sum_buff[data_num-1].abr_sum_ip_mask) == rn->p.prefixlen))
#else
						if(data_num != 0 
							&& (abr_sum_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
							&& (abr_sum_buff[data_num-1].p.prefixlen == rn->p.prefixlen))
#endif
						{
							continue;
						}
							
						memset(&s_abr_sum_info, 0, sizeof(struct statistics_ospf_abr_summary_info));
						s_abr_sum_info.ospf_id = ospf->ospf_id;
						s_abr_sum_info.area_id = ntohl(area->area_id.s_addr);
#if IP_MASK_FORMAT
						
						s_abr_sum_info.abr_sum_ip_addr.s_addr = area_range->addr.s_addr;//rn->p.u.prefix4.s_addr;
						masklen_to_netip (area_range->masklen, &(s_abr_sum_info.abr_sum_ip_mask));//rn->p.prefixlen;
#else
						s_abr_sum_info.p.family = AF_INET;
						s_abr_sum_info.p.u.prefix4.s_addr = area_range->addr.s_addr;
						s_abr_sum_info.p.prefixlen = area_range->masklen;
#endif
						if (!CHECK_FLAG(area_range->flags, OSPF_AREA_RANGE_ADVERTISE))
						{
							s_abr_sum_info.abr_summary_notAdvertise = 1;
						}
						else
						{
							s_abr_sum_info.abr_summary_notAdvertise = 2;
						}
						if (area_range->cost_config != OSPF_AREA_RANGE_COST_UNSPEC)
						{
							s_abr_sum_info.abr_summary_cost = area_range->cost_config;
						}
						else
						{
							s_abr_sum_info.abr_summary_cost = 0;
						}
#if IP_MASK_FORMAT
						zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
									s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
									inet_ntoa(s_abr_sum_info.abr_sum_ip_addr),ip_masklen(s_abr_sum_info.abr_sum_ip_mask));
#else
						zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
									s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
									inet_ntoa(s_abr_sum_info.abr_sum_ip_addr), s_abr_sum_info.p.prefixlen);
#endif
	                    memcpy(&abr_sum_buff[data_num++], &s_abr_sum_info, sizeof(struct statistics_ospf_abr_summary_info));
	                    if (data_num == msg_num)
	                    {
	                        return data_num;
	                    }
					}
				}
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
        {
            if(ospf_key.ospf_id_key <= ospf->ospf_id)//找到上一轮最后一次返回的ospf实例号
            {   
            	for (ALL_LIST_ELEMENTS_RO(ospf->areas, node1, area))
            	{
					if(ospf_key.area_id_key == ntohl(area->area_id.s_addr))//找到上一轮最后一次返回的area号
					{
						/* look up rn_temp by key */
#if IP_MASK_FORMAT
						temp_p.family = AF_INET;
					    temp_p.prefix = ospf_key.abr_sum_ip_addr;
					    temp_p.prefixlen = ip_masklen(ospf_key.abr_sum_ip_mask);
						rn_temp = route_node_lookup(area->ranges, (struct prefix*)&(temp_p));//找到上一轮最后一次返回的树节点
#else
						rn_temp = route_node_lookup(area->ranges, &(ospf_key.p_key));//找到上一轮最后一次返回的树节点

#endif
						if(rn_temp != NULL)
						{
							for (rn = rn_temp; rn; rn = route_next(rn))//从上一轮的最后一个树节点开始接着往后遍历
							{
								if(rn == rn_temp)
								{
									continue;
								}
								
								if (rn->info)
								{
									area_range = rn->info;

#if IP_MASK_FORMAT
									if(data_num != 0 
										&& (abr_sum_buff[data_num-1].abr_sum_ip_addr.s_addr == rn->p.u.prefix4.s_addr)
										&& (ip_masklen(abr_sum_buff[data_num-1].abr_sum_ip_mask) == rn->p.prefixlen))
#else
									if(data_num != 0 
										&& (abr_sum_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
										&& (abr_sum_buff[data_num-1].p.prefixlen == rn->p.prefixlen))
#endif
									{
										continue;
									}
										
									memset(&s_abr_sum_info, 0, sizeof(struct statistics_ospf_abr_summary_info));
									s_abr_sum_info.ospf_id = ospf->ospf_id;
									s_abr_sum_info.area_id = ntohl(area->area_id.s_addr);
									
#if IP_MASK_FORMAT
									s_abr_sum_info.abr_sum_ip_addr.s_addr = area_range->addr.s_addr;//rn->p.u.prefix4.s_addr;
									masklen_to_netip (area_range->masklen, &(s_abr_sum_info.abr_sum_ip_mask));//rn->p.prefixlen;
#else
									s_abr_sum_info.p.family = AF_INET;
									s_abr_sum_info.p.u.prefix4.s_addr = area_range->addr.s_addr;
									s_abr_sum_info.p.prefixlen = area_range->masklen;
#endif
									if (!CHECK_FLAG(area_range->flags, OSPF_AREA_RANGE_ADVERTISE))
									{
										s_abr_sum_info.abr_summary_notAdvertise = 1;
									}
									else
									{
										s_abr_sum_info.abr_summary_notAdvertise = 2;
									}
									if (area_range->cost_config != OSPF_AREA_RANGE_COST_UNSPEC)
									{
										s_abr_sum_info.abr_summary_cost = area_range->cost_config;
									}
									else
									{
										s_abr_sum_info.abr_summary_cost = 0;
									}

#if IP_MASK_FORMAT
									zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
												s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
												inet_ntoa(s_abr_sum_info.abr_sum_ip_addr),ip_masklen(s_abr_sum_info.abr_sum_ip_mask));
#else
									zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
												s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
												inet_ntoa(s_abr_sum_info.abr_sum_ip_addr), s_abr_sum_info.p.prefixlen);
#endif
				                    memcpy(&abr_sum_buff[data_num++], &s_abr_sum_info, sizeof(struct statistics_ospf_abr_summary_info));
				                    if (data_num == msg_num)
				                    {
				                        return data_num;
				                    }
								}
							}
						}
					}
					else if(ospf_key.area_id_key < ntohl(area->area_id.s_addr))
					{
						for (rn = route_top(area->ranges); rn; rn = route_next(rn))//新的area节点，从树根开始遍历
						{
							if (rn->info)
							{
								area_range = rn->info;
#if IP_MASK_FORMAT
								if(data_num != 0 
									&& (abr_sum_buff[data_num-1].abr_sum_ip_addr.s_addr == rn->p.u.prefix4.s_addr)
									&& (ip_masklen(abr_sum_buff[data_num-1].abr_sum_ip_mask) == rn->p.prefixlen))
#else
								if(data_num != 0 
									&& (abr_sum_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
									&& (abr_sum_buff[data_num-1].p.prefixlen == rn->p.prefixlen))
#endif
								{
									continue;
								}
									
								memset(&s_abr_sum_info, 0, sizeof(struct statistics_ospf_abr_summary_info));
								s_abr_sum_info.ospf_id = ospf->ospf_id;
								s_abr_sum_info.area_id = ntohl(area->area_id.s_addr);
#if IP_MASK_FORMAT
								s_abr_sum_info.abr_sum_ip_addr.s_addr = area_range->addr.s_addr;//rn->p.u.prefix4.s_addr;
								masklen_to_netip (area_range->masklen, &(s_abr_sum_info.abr_sum_ip_mask));//rn->p.prefixlen;
#else
								s_abr_sum_info.p.family = AF_INET;
								s_abr_sum_info.p.u.prefix4.s_addr = area_range->addr.s_addr;
								s_abr_sum_info.p.prefixlen = area_range->masklen;
#endif
								if (!CHECK_FLAG(area_range->flags, OSPF_AREA_RANGE_ADVERTISE))
								{
									s_abr_sum_info.abr_summary_notAdvertise = 1;
								}
								else
								{
									s_abr_sum_info.abr_summary_notAdvertise = 2;
								}
								if (area_range->cost_config != OSPF_AREA_RANGE_COST_UNSPEC)
								{
									s_abr_sum_info.abr_summary_cost = area_range->cost_config;
								}
								else
								{
									s_abr_sum_info.abr_summary_cost = 0;
								}

#if IP_MASK_FORMAT
								zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
											s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
											inet_ntoa(s_abr_sum_info.abr_sum_ip_addr),ip_masklen(s_abr_sum_info.abr_sum_ip_mask));
#else
								zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ospf_id:%d area:%d target_summary:%s/%d\n", __func__, __LINE__,\
												s_abr_sum_info.ospf_id, s_abr_sum_info.area_id,\
												inet_ntoa(s_abr_sum_info.abr_sum_ip_addr), s_abr_sum_info.p.prefixlen);
#endif								
			                    memcpy(&abr_sum_buff[data_num++], &s_abr_sum_info, sizeof(struct statistics_ospf_abr_summary_info));
			                    if (data_num == msg_num)
			                    {
			                        return data_num;
			                    }
							}
						}
					}
				}
            }	
		}     
	}
    return data_num;
}


static int
statistics_ospf_asbr_summary_get_bulk(void *pdata, int data_len, struct statistics_ospf_asbr_summary_info asbr_sum_buff[])
{
    struct ospf_asbr_sum_info_key ospf_key = *((struct ospf_asbr_sum_info_key*)pdata);
    struct statistics_ospf_asbr_summary_info  s_asbr_sum_info;
	struct ospf_summary *summary = NULL;
    struct ospf *ospf = NULL;
	struct prefix_ipv4 temp_p;
    struct route_node *rn;		
    struct route_node *rn_temp;
    struct listnode *node = NULL;

	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statistics_ospf_asbr_summary_info);
    //int msg_num  = 2;//pressure test
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	
	zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d snmp get ospf_id:%d target_summary:%s/%d\n", __func__, __LINE__,\
									ospf_key.ospf_id_key,\
									inet_ntoa(ospf_key.asbr_sum_ip_addr), ip_masklen(ospf_key.asbr_sum_ip_mask));

	if(ospf_key.ospf_id_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			for (rn = route_top(ospf->summary); rn; rn = route_next(rn))
			{
				if(rn->info)
				{
					summary = rn->info;
					memset(&s_asbr_sum_info, 0, sizeof(struct statistics_ospf_asbr_summary_info));

					s_asbr_sum_info.ospf_id = ospf->ospf_id;
					//s_asbr_sum_info.p.family = AF_INET;
					s_asbr_sum_info.asbr_sum_ip_addr.s_addr = summary->addr.s_addr;//rn->p.u.prefix4.s_addr;
					masklen_to_netip (summary->masklen, &(s_asbr_sum_info.asbr_sum_ip_mask));
					if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
					{
						s_asbr_sum_info.asbr_summary_cost = summary->cost_config;
					}
					else
					{
						s_asbr_sum_info.asbr_summary_cost = 0;
					}

					if (!CHECK_FLAG(summary->flags, OSPF_SUMMARY_ADVERTISE))
					{
						s_asbr_sum_info.asbr_summary_notAdvertise = 1;
					}
					else
					{
						s_asbr_sum_info.asbr_summary_notAdvertise = 2;
					}

					zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d target_summary:%s/%d\n", __func__, __LINE__,\
									s_asbr_sum_info.ospf_id,\
									inet_ntoa(s_asbr_sum_info.asbr_sum_ip_addr), ip_masklen(s_asbr_sum_info.asbr_sum_ip_mask));
						
                    memcpy(&asbr_sum_buff[data_num++], &s_asbr_sum_info, sizeof(struct statistics_ospf_asbr_summary_info));
                    if (data_num == msg_num)
                    {
                        return data_num;
                    }
				}
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			if(ospf_key.ospf_id_key == ospf->ospf_id)//找到上一轮最后一次返回的ospf实例号
            {
				/* look up rn_temp by key */
				temp_p.family = AF_INET;
			    temp_p.prefix = ospf_key.asbr_sum_ip_addr;
			    temp_p.prefixlen = ip_masklen(ospf_key.asbr_sum_ip_mask);
				rn_temp = route_node_lookup(ospf->summary, (struct prefix*)&(temp_p));//找到上一轮最后一次返回的树节点
				if(rn_temp != NULL)
				{
					for (rn = rn_temp; rn; rn = route_next(rn))//从上一轮的最后一个树节点开始接着往后遍历
					{
						if(rn == rn_temp)
						{
							continue;
						}
						
						if (rn->info)
						{
							summary = rn->info;
							memset(&s_asbr_sum_info, 0, sizeof(struct statistics_ospf_asbr_summary_info));

							s_asbr_sum_info.ospf_id = ospf->ospf_id;
							//s_asbr_sum_info.p.family = AF_INET;
							s_asbr_sum_info.asbr_sum_ip_addr.s_addr = summary->addr.s_addr;//rn->p.u.prefix4.s_addr;
							masklen_to_netip (summary->masklen, &(s_asbr_sum_info.asbr_sum_ip_mask));
							if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
							{
								s_asbr_sum_info.asbr_summary_cost = summary->cost_config;
							}
							else
							{
								s_asbr_sum_info.asbr_summary_cost = 0;
							}

							if (!CHECK_FLAG(summary->flags, OSPF_SUMMARY_ADVERTISE))
							{
								s_asbr_sum_info.asbr_summary_notAdvertise = 1;
							}
							else
							{
								s_asbr_sum_info.asbr_summary_notAdvertise = 2;
							}

							zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d target_summary:%s/%d\n", __func__, __LINE__,\
									s_asbr_sum_info.ospf_id,\
									inet_ntoa(s_asbr_sum_info.asbr_sum_ip_addr), ip_masklen(s_asbr_sum_info.asbr_sum_ip_mask));
								
		                    memcpy(&asbr_sum_buff[data_num++], &s_asbr_sum_info, sizeof(struct statistics_ospf_asbr_summary_info));
		                    if (data_num == msg_num)
		                    {
		                        return data_num;
		                    }
						}
					}
				}
			}
			else if(ospf_key.ospf_id_key < ospf->ospf_id)
			{
				for (rn = route_top(ospf->summary); rn; rn = route_next(rn))//新的ospf节点，从树根开始遍历
				{
					if (rn->info)
					{
						summary = rn->info;
						memset(&s_asbr_sum_info, 0, sizeof(struct statistics_ospf_asbr_summary_info));

						s_asbr_sum_info.ospf_id = ospf->ospf_id;
						//s_asbr_sum_info.p.family = AF_INET;
						s_asbr_sum_info.asbr_sum_ip_addr.s_addr = summary->addr.s_addr;//rn->p.u.prefix4.s_addr;
						masklen_to_netip (summary->masklen, &(s_asbr_sum_info.asbr_sum_ip_mask));
						if (summary->cost_config != OSPF_SUMMARY_COST_UNSPEC)
						{
							s_asbr_sum_info.asbr_summary_cost = summary->cost_config;
						}
						else
						{
							s_asbr_sum_info.asbr_summary_cost = 0;
						}

						if (!CHECK_FLAG(summary->flags, OSPF_SUMMARY_ADVERTISE))
						{
							s_asbr_sum_info.asbr_summary_notAdvertise = 1;
						}
						else
						{
							s_asbr_sum_info.asbr_summary_notAdvertise = 2;
						}

						zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d target_summary:%s/%d\n", __func__, __LINE__,\
								s_asbr_sum_info.ospf_id,\
								inet_ntoa(s_asbr_sum_info.asbr_sum_ip_addr), ip_masklen(s_asbr_sum_info.asbr_sum_ip_mask));
							
	                    memcpy(&asbr_sum_buff[data_num++], &s_asbr_sum_info, sizeof(struct statistics_ospf_asbr_summary_info));
	                    if (data_num == msg_num)
	                    {
	                        return data_num;
	                    }
					}
				}
			}
		}
	}
	return data_num;
}

static int
statistics_ospf_redistribute_route_get_bulk(void *pdata, int data_len, struct statistics_ospf_redistribute_route_info redis_route_buff[])
{
    struct ospf_redistribute_route_info_key ospf_key = *((struct ospf_redistribute_route_info_key*)pdata);
    struct statistics_ospf_redistribute_route_info  s_redis_route_info;
    struct ospf *ospf = NULL;
    struct listnode *node = NULL;
	int type = 0;
	u_int32_t instance = 0;

	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statistics_ospf_redistribute_route_info);
	//int msg_num  = 2;//pressure test
	int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	
	zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d snmp get ospf_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
								ospf_key.ospf_id_key,\
								ospf_key.redis_route_type_key, ospf_key.redis_target_instance_id_key);

	if(ospf_key.ospf_id_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			for (type = 0; type < ROUTE_PROTO_MAX; type++)
		    {
		        for (instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
		        {
		            if (type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id)
		            {
		                continue;
		            }

		            if (vrf_bitmap_check(ospf->dmetric[type][instance].redist, VRF_DEFAULT))
		            {
		                if ((type == ROUTE_PROTO_CONNECT && ospf->route_policy_flag == 1)
		                        || type != ROUTE_PROTO_CONNECT)
		                {
							memset(&s_redis_route_info, 0, sizeof(struct statistics_ospf_redistribute_route_info));

							s_redis_route_info.ospf_id = ospf->ospf_id;
							s_redis_route_info.redis_route_type = type;
							s_redis_route_info.redis_target_instance_id = instance;
							if (ospf->dmetric[type][instance].value >= 0)
							{
								s_redis_route_info.redis_route_metric = ospf->dmetric[type][instance].value;
							}
							
							if (ospf->dmetric[type][instance].type == EXTERNAL_METRIC_TYPE_1)
							{
								s_redis_route_info.route_turn2_ospf_tpye = 1;
							}
							else if(ospf->dmetric[type][instance].type == EXTERNAL_METRIC_TYPE_2)
							{
								s_redis_route_info.route_turn2_ospf_tpye = 2;
							}

							zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
										s_redis_route_info.ospf_id,\
										s_redis_route_info.redis_route_type, s_redis_route_info.redis_target_instance_id);
							
		                    memcpy(&redis_route_buff[data_num++], &s_redis_route_info, sizeof(struct statistics_ospf_redistribute_route_info));
		                    if (data_num == msg_num)
		                    {
		                        return data_num;
		                    }
		                }
		            }
		        }
		    }
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			if(ospf->ospf_id >= ospf_key.ospf_id_key)
			{
				for (type = 0; type < ROUTE_PROTO_MAX; type++)
			    {
					if(type >= ospf_key.redis_route_type_key)
					{
						for (instance = 0; instance <= OSPF_MAX_PROCESS_ID; instance++)
				        {
							if(instance > ospf_key.redis_target_instance_id_key)
							{
								if (type == ROUTE_PROTO_OSPF && instance == ospf->ospf_id)
					            {
					                continue;
					            }

				            	if (vrf_bitmap_check(ospf->dmetric[type][instance].redist, VRF_DEFAULT))
					            {
					                if ((type == ROUTE_PROTO_CONNECT && ospf->route_policy_flag == 1)
					                        || type != ROUTE_PROTO_CONNECT)
					                {
										memset(&s_redis_route_info, 0, sizeof(struct statistics_ospf_redistribute_route_info));
										s_redis_route_info.ospf_id = ospf->ospf_id;
										s_redis_route_info.redis_route_type = type;
										s_redis_route_info.redis_target_instance_id = instance;
										if (ospf->dmetric[type][instance].value >= 0)
										{
											s_redis_route_info.redis_route_metric = ospf->dmetric[type][instance].value;
										}
										if (ospf->dmetric[type][instance].type == EXTERNAL_METRIC_TYPE_1)
										{
											s_redis_route_info.route_turn2_ospf_tpye = 1;
										}
										else if(ospf->dmetric[type][instance].type == EXTERNAL_METRIC_TYPE_2)
										{
											s_redis_route_info.route_turn2_ospf_tpye = 2;
										}

										zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d, route_type:%d ,instances:%d\n", __func__, __LINE__,\
													s_redis_route_info.ospf_id,\
													s_redis_route_info.redis_route_type, s_redis_route_info.redis_target_instance_id);
										
					                    memcpy(&redis_route_buff[data_num++], &s_redis_route_info, sizeof(struct statistics_ospf_redistribute_route_info));
					                    if (data_num == msg_num)
					                    {
					                        return data_num;
					                    }
					                }
					            }
							}
				        }
					}
			    }
			}
		}
	}
	return data_num;
}

static int
statistics_ospf_nbma_neighbor_info_get_bulk(void *pdata, int data_len, struct statistics_ospf_nbma_neighbor_info nbma_peer_buff[])
{
    struct ospf_nbma_nbr_info_key ospf_key = *((struct ospf_nbma_nbr_info_key*)pdata);
    struct statistics_ospf_nbma_neighbor_info  s_nbma_nbr_info;
    struct ospf *ospf = NULL;
	struct prefix_ipv4 p;
    struct route_node *rn;		
    struct route_node *rn_temp;
    struct listnode *node = NULL;
    struct ospf_nbr_nbma *nbr_nbma = NULL;

	
    int msg_num  = IPC_MSG_LEN/sizeof(struct statistics_ospf_nbma_neighbor_info);
	//int msg_num  = 2;//pressure test
    int data_num = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }
	
	zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d snmp get ospf_id:%d, neighbor_add:%s\n", __func__, __LINE__,\
 								ospf_key.ospf_id_key,inet_ntoa(ospf_key.nbr_addr_key));

	if(ospf_key.ospf_id_key == 0)   /*  1st get */
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			for (rn = route_top(ospf->nbr_nbma); rn; rn = route_next(rn))
			{
				if ((nbr_nbma = rn->info))
				{
					memset(&s_nbma_nbr_info, 0, sizeof(struct statistics_ospf_nbma_neighbor_info));
					s_nbma_nbr_info.ospf_id = ospf->ospf_id;
					s_nbma_nbr_info.nbr_addr = nbr_nbma->addr;
					s_nbma_nbr_info.nbr_poll_interval = nbr_nbma->v_poll;

					zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d, neighbor_add:%s\n", __func__, __LINE__,\
 								s_nbma_nbr_info.ospf_id,inet_ntoa(s_nbma_nbr_info.nbr_addr));
					
                    memcpy(&nbma_peer_buff[data_num++], &s_nbma_nbr_info, sizeof(struct statistics_ospf_nbma_neighbor_info));
                    if (data_num == msg_num)
                    {
                        return data_num;
                    }
				}
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS_RO (om->ospf, node, ospf))
		{
			if(ospf->ospf_id == ospf_key.ospf_id_key)
			{
				/* look up rn_temp by key */
				p.family = AF_INET;
			    p.prefix = ospf_key.nbr_addr_key;
			    p.prefixlen = IPV4_MAX_BITLEN;
				rn_temp = route_node_lookup(ospf->nbr_nbma, (struct prefix *)&p);//找到上一轮最后一次返回的树节点
				if(rn_temp != NULL)
				{
					for (rn = rn_temp; rn; rn = route_next(rn))//从上一轮的最后一个树节点开始接着往后遍历
					{
						if(rn == rn_temp)
						{
							continue;
						}
						
						if (rn->info)
						{
							nbr_nbma = rn->info;
							memset(&s_nbma_nbr_info, 0, sizeof(struct statistics_ospf_nbma_neighbor_info));

							s_nbma_nbr_info.ospf_id = ospf->ospf_id;
							s_nbma_nbr_info.nbr_addr = nbr_nbma->addr;
							s_nbma_nbr_info.nbr_poll_interval = nbr_nbma->v_poll;

							zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d, neighbor_add:%s\n", __func__, __LINE__,\
		 								s_nbma_nbr_info.ospf_id,inet_ntoa(s_nbma_nbr_info.nbr_addr));
							
		                    memcpy(&nbma_peer_buff[data_num++], &s_nbma_nbr_info, sizeof(struct statistics_ospf_nbma_neighbor_info));
		                    if (data_num == msg_num)
		                    {
		                        return data_num;
		                    }
						}
					}
				}
			}
			else if(ospf->ospf_id > ospf_key.ospf_id_key)
			{
				for (rn = route_top(ospf->nbr_nbma); rn; rn = route_next(rn))
				{
					if ((nbr_nbma = rn->info))
					{
						memset(&s_nbma_nbr_info, 0, sizeof(struct statistics_ospf_nbma_neighbor_info));
						s_nbma_nbr_info.ospf_id = ospf->ospf_id;
						s_nbma_nbr_info.nbr_addr = nbr_nbma->addr;
						s_nbma_nbr_info.nbr_poll_interval = nbr_nbma->v_poll;

						zlog_debug(OSPF_DBG_OTHER, "fun:%s line:%d ack ospf_id:%d, neighbor_add:%s\n", __func__, __LINE__,\
	 								s_nbma_nbr_info.ospf_id,inet_ntoa(s_nbma_nbr_info.nbr_addr));
						
	                    memcpy(&nbma_peer_buff[data_num++], &s_nbma_nbr_info, sizeof(struct statistics_ospf_nbma_neighbor_info));
	                    if (data_num == msg_num)
	                    {
	                        return data_num;
	                    }
					}
				}
			}
		}
	}
	return data_num;
}

static int
static_ospf_if_config_get_bulk(void *pdata, int data_len, struct statics_ospf_if sif_buff[])
{
    u_int32_t ifindex = *((u_int32_t*)pdata);
    struct statics_ospf_if  s_ospf_if;
    struct listnode *n1 = NULL;
	struct listnode *n2 = NULL;
	struct crypt_key *ck = NULL;
    struct interface *ifp = NULL;
    struct ospf_if_params *params = NULL;
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_if);
    int data_num = 0;
	int flag = 0;
	zlog_debug(OSPF_DBG_OTHER, "%s %d ifindex:%d",__func__,__LINE__,ifindex);

    for (ALL_LIST_ELEMENTS_RO (iflist, n1, ifp))
    {
        if (memcmp (ifp->name, "VLINK", 5) == 0)
        {
            continue;
        }
		
		if(ifp->ifindex == 0)
		{
			continue;
		}

		if(ifp->ifindex == ifindex)
		{
			flag = 1;
			continue;
		}	
		
        if(flag == 1 || ifindex == 0)
        {
			zlog_debug(OSPF_DBG_OTHER, "%s %d ifindex:%d %0x",__func__,__LINE__,ifp->ifindex,ifp->ifindex);
            params = IF_DEF_PARAMS (ifp);
            if (OSPF_IF_PARAM_CONFIGURED (params, type) &&
                    params->type != OSPF_IFTYPE_LOOPBACK)
            {
                if (params->type != ospf_default_iftype(ifp))
                {
                    memset(&s_ospf_if, 0, sizeof(struct statics_ospf_if));
                    memcpy(s_ospf_if.if_name, ifp->name, strlen(ifp->name));
                    s_ospf_if.if_type = params->type;
                    s_ospf_if.ifindex = ifp->ifindex;

					if (OSPF_IF_PARAM_CONFIGURED(params, auth_type) &&
                        params->auth_type != OSPF_AUTH_NOTSET)
					{
						s_ospf_if.auth_type = params->auth_type;
						if(params->auth_type == OSPF_AUTH_SIMPLE)
						{
							memcpy(s_ospf_if.auth_password, params->auth_simple, OSPF_AUTH_SIMPLE_SIZE + 1);
						}
						else if(params->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
						{
							for (ALL_LIST_ELEMENTS_RO(params->auth_crypt, n2, ck))
                            {
								s_ospf_if.md5_id = ck->key_id;
								memcpy(s_ospf_if.auth_password, ck->auth_key, OSPF_AUTH_MD5_SIZE + 1);
                            }
						}
						else
						{
							s_ospf_if.auth_type = 3;
						}
					}
					else
					{
						s_ospf_if.auth_type = 3;
					}
					
					s_ospf_if.cost_value = params->output_cost_cmd;
					s_ospf_if.dead_interval = params->v_wait;
					s_ospf_if.hello_interval = params->v_hello;
					s_ospf_if.retransmit_interval = params->retransmit_interval;
					if (OSPF_IF_PARAM_CONFIGURED(params, mtu_ignore) &&
                        params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
					{
						if (params->mtu_ignore == 0)
	                    {
							s_ospf_if.mtu_check = 1;
	                    }
	                    else
	                    {
	                        s_ospf_if.mtu_check = 2;
	                    }
					}
					else
                    {
                        s_ospf_if.mtu_check = 2;
                    }
					s_ospf_if.dr_priority = params->priority;
					
                    memcpy(&sif_buff[data_num++], &s_ospf_if, sizeof(struct statics_ospf_if));
                    if (data_num == msg_num)
                    {
                        return data_num;
                    }
                }
                if (params->type == ospf_default_iftype(ifp))
                {
                    if(OSPF_IF_PARAM_CONFIGURED (params, passive_interface))
                    {
                        memset(&s_ospf_if, 0, sizeof(struct statics_ospf_if));
                        memcpy(s_ospf_if.if_name, ifp->name, strlen(ifp->name));
                        s_ospf_if.if_type = params->type;
                        s_ospf_if.ifindex = ifp->ifindex;
						if (OSPF_IF_PARAM_CONFIGURED(params, auth_type) &&
                        params->auth_type != OSPF_AUTH_NOTSET)
						{
							s_ospf_if.auth_type = params->auth_type;
							if(params->auth_type == OSPF_AUTH_SIMPLE)
							{
								memcpy(s_ospf_if.auth_password, params->auth_simple, OSPF_AUTH_SIMPLE_SIZE + 1);
							}
							else if(params->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
							{
								for (ALL_LIST_ELEMENTS_RO(params->auth_crypt, n2, ck))
	                            {
									s_ospf_if.md5_id = ck->key_id;
									memcpy(s_ospf_if.auth_password, ck->auth_key, OSPF_AUTH_MD5_SIZE + 1);
	                            }
							}
							else
							{
								s_ospf_if.auth_type = 3;
							}
						}
						else
						{
							s_ospf_if.auth_type = 3;
						}
						s_ospf_if.cost_value = params->output_cost_cmd;
						s_ospf_if.dead_interval = params->v_wait;
						s_ospf_if.hello_interval = params->v_hello;
						s_ospf_if.retransmit_interval = params->retransmit_interval;
						if (OSPF_IF_PARAM_CONFIGURED(params, mtu_ignore) &&
	                        params->mtu_ignore != OSPF_MTU_IGNORE_DEFAULT)
						{
							if (params->mtu_ignore == 0)
		                    {
								s_ospf_if.mtu_check = 1;
		                    }
		                    else
		                    {
		                        s_ospf_if.mtu_check = 2;
		                    }
						}
						else
	                    {
	                        s_ospf_if.mtu_check = 2;
	                    }
						s_ospf_if.dr_priority = params->priority;
							
                        memcpy(&sif_buff[data_num++], &s_ospf_if, sizeof(struct statics_ospf_if));
                        if (data_num == msg_num)
                        {
                            return data_num;
                        }
                    }
                }
            }
        }
    }
    
    return data_num;
}

int get_dcn_ne_info_from_tlv(struct ospf_lsa *lsa, struct ospf *ospf, struct ospf_dcn_ne_info *ne_info)
{
	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct lsa_header *lsah = NULL;
	struct opaque_tlv_header *tlvh, *next;
	u_int16_t sum, total;
	struct opaque_tlv_devmac *top_devmac = NULL;
	struct opaque_tlv_devid *top_devid = NULL;
	struct opaque_tlv_manuid *top_vendor = NULL;
	struct opaque_tlv_neid *top_neid = NULL;
	struct opaque_tlv_neipv4 *top_neipv4 = NULL;
	//struct opaque_tlv_neipv6 *top_neipv6 = NULL;
	struct route_entry *target_route = NULL;
	struct inet_prefix  target_prefix;
	int offset = 0;

	//NOTE:Here is the default opaque_lsa_flag == !0x01 is dcn lsa.
	if(!CHECK_FLAG(lsa->opaque_lsa_flag, OPAQUE_LSA_FOR_DCN))
	{
		lsah = (struct lsa_header *) lsa->data;
		u_int32_t lsid = ntohl (lsah->id.s_addr);
		u_char    opaque_type = GET_OPAQUE_TYPE (lsid);

		if(opaque_type == OPAQUE_TYPE_NMS_AUTOREPORT)
		{
			sum = 0;
			total = ntohs (lsah->length) - OSPF_LSA_HEADER_SIZE;

			for (tlvh = OPAQUE_TLV_HDR_TOP (lsah); sum < total;
						tlvh = (next ? next : OPAQUE_TLV_HDR_NEXT (tlvh, offset)))
			{
				next = NULL;
				offset = 0;
				switch (ntohs (tlvh->type))
				{
					case OPAQUE_TLV_TYPE_MANUID:
						top_vendor = (struct opaque_tlv_manuid *) tlvh;
						memset(ne_info->dcn_ne_vendor, 0, sizeof(ne_info->dcn_ne_vendor));
						memcpy(ne_info->dcn_ne_vendor, &(top_vendor->value), ntohs(top_vendor->header.length));

						if((ntohs(tlvh->length)%4) != 0)
						{
							offset = (4 - ntohs(tlvh->length)%4);
						}
						sum += (OPAQUE_TLV_SIZE (tlvh) + offset);
						break;
					case OPAQUE_TLV_TYPE_DEVID:
						top_devid = (struct opaque_tlv_devid *) tlvh;
						memset(ne_info->dcn_ne_device_type, 0, sizeof(ne_info->dcn_ne_device_type));
						memcpy(ne_info->dcn_ne_device_type, &(top_devid->value), ntohs(top_devid->header.length));
						
						if((ntohs(tlvh->length)%4) != 0)
						{
							offset = (4 - ntohs(tlvh->length)%4);
						}
						sum += (OPAQUE_TLV_SIZE (tlvh) + offset);
					  	break;
					case OPAQUE_TLV_TYPE_DEVMAC:
						top_devmac = (struct opaque_tlv_devmac *) tlvh;
						memset(ne_info->mac, 0, sizeof(ne_info->mac));
						memcpy(ne_info->mac, top_devmac->value, ntohs(top_devmac->header.length));

						if((ntohs(tlvh->length)%4) != 0)
						{
							offset = (4 - ntohs(tlvh->length)%4);
						}
						sum += (OPAQUE_TLV_SIZE (tlvh) + offset);
					  	break;
					case OPAQUE_TLV_TYPE_NEID:
						top_neid = (struct opaque_tlv_neid *) tlvh;
						ne_info->dcn_ne_id = top_neid->value.s_addr;
						sum += OPAQUE_TLV_SIZE (tlvh);
					  	break;
					case OPAQUE_TLV_TYPE_NEIPV4:
						top_neipv4 = (struct opaque_tlv_neipv4 *) tlvh;
						ne_info->dcn_ne_ip = top_neipv4->value.s_addr;
						sum += OPAQUE_TLV_SIZE (tlvh);
					  	break;
					case OPAQUE_TLV_TYPE_NEIPV6:	
						sum += OPAQUE_TLV_SIZE (tlvh);
					  	break;
					default:
						sum += OPAQUE_TLV_SIZE (tlvh);
					  	break;
				}
			}

			//get metric from route
			memset(&target_prefix, 0, sizeof(struct inet_prefix));
			target_prefix.type = INET_FAMILY_IPV4;
			target_prefix.addr.ipv4 = ntohl(lsa->data->adv_router.s_addr);
			target_prefix.prefixlen = IPV4_MAX_BITLEN;
			
			target_route = route_com_get_route(&target_prefix, 0, MODULE_ID_OSPF);
			if(target_route != NULL)
			{
				ne_info->dcn_ne_metric = target_route->nhp[0].cost;
                mem_share_free_bydata(target_route, MODULE_ID_OSPF);
			}
			if(IS_DEBUG_OSPF_DCN)
			{
				zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': ne_info->dcn_ne_metric = %d\n", \
									__FILE__, __LINE__, __func__, ne_info->dcn_ne_metric);
			}
		}
		else
		{
			return -1;
		}
	}
	else
	{
		if(IS_DEBUG_OSPF_DCN)
		{
			zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': The lsa is not for dcn.\n",\
									__FILE__, __LINE__, __func__);
		}
		return -1;
	}
	return 0;
}


static int
ospf_dcn_conf_get_bulk(void *pdata, int data_len, struct ospf_dcn_config sdcn_conf_buff[])
{
	//u_int32_t ospf_id = *((u_int32_t *)pdata);
	struct ospf_dcn_config s_ospf_dcn_conf;
	struct ospf *ospf = NULL;
	struct listnode *node = NULL,
	                *nnode = NULL;
	//int msg_num  = IPC_MSG_LEN/sizeof(struct ospf_dcn_config);
	int data_num = 0;

	/*if (listcount (om->ospf) == 0)
    {
        return 0;
    }*/
	
	//if(ospf_id == 0)/* 1st get*/
	//{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{
			if((ospf->ospf_id == OSPF_MAX_PROCESS_ID) && (ospf->dcn_enable))
			{
				memset(&s_ospf_dcn_conf, 0, sizeof(struct ospf_dcn_config));
				s_ospf_dcn_conf.ospf_id = ospf->ospf_id;
				s_ospf_dcn_conf.ospf_dcn_enable = ospf->dcn_enable;
				s_ospf_dcn_conf.ospf_dcn_trap_enable = ospf->dcn_report;
				s_ospf_dcn_conf.ospf_dcn_ne_id = (om->ne_id_static.s_addr == 0) ? ntohl(om->ne_id_dynamic.s_addr) : ntohl(om->ne_id_static.s_addr);
				s_ospf_dcn_conf.ospf_dcn_ne_ip = (om->ne_ip_static.s_addr == 0) ? om->ne_ip_dynamic.s_addr : om->ne_ip_static.s_addr;
				s_ospf_dcn_conf.ospf_dcn_ne_ip_prefixlen = (om->ne_ip_static.s_addr == 0) ? om->ne_ip_dynamic_prefixlen : om->ne_ip_static_prefixlen;
				memcpy(s_ospf_dcn_conf.mac, om->mac, 6);
				memcpy(s_ospf_dcn_conf.dcn_ne_vendor, om->company_name, strlen(om->company_name));
				
				memcpy(&sdcn_conf_buff[data_num++], &s_ospf_dcn_conf, sizeof(struct ospf_dcn_config));
	            return data_num;
			}
		}

		memset(&s_ospf_dcn_conf, 0, sizeof(struct ospf_dcn_config));
		memcpy(&sdcn_conf_buff[data_num++], &s_ospf_dcn_conf, sizeof(struct ospf_dcn_config));
       /* if (data_num == msg_num)
        {
            return data_num;
        }
	}
	else
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{
			if((ospf->ospf_id > ospf_id) && (ospf->dcn_enable))
			{
				memset(&s_ospf_dcn_conf, 0, sizeof(struct ospf_dcn_config));
				s_ospf_dcn_conf.ospf_id = ospf->ospf_id;
				s_ospf_dcn_conf.ospf_dcn_enable = ospf->dcn_enable;
				s_ospf_dcn_conf.ospf_dcn_trap_enable = ospf->dcn_report;
				s_ospf_dcn_conf.ospf_dcn_ne_id = (om->ne_id_static.s_addr == 0) ? ntohl(om->ne_id_dynamic.s_addr) : ntohl(om->ne_id_static.s_addr);
				s_ospf_dcn_conf.ospf_dcn_ne_ip = (om->ne_ip_static.s_addr == 0) ? om->ne_ip_dynamic.s_addr : om->ne_ip_static.s_addr;
				s_ospf_dcn_conf.ospf_dcn_ne_ip_prefixlen = (om->ne_ip_static.s_addr == 0) ? om->ne_ip_dynamic_prefixlen : om->ne_ip_static_prefixlen;
				memcpy(s_ospf_dcn_conf.mac, om->mac, 6);
				memcpy(s_ospf_dcn_conf.dcn_ne_vendor, om->company_name, strlen(om->company_name));
				
				memcpy(&sdcn_conf_buff[data_num++], &s_ospf_dcn_conf, sizeof(struct ospf_dcn_config));
	            if (data_num == msg_num)
	            {
	                return data_num;
	            }
			}
		}
	}*/
	return data_num;
}

static int
ospf_dcn_ne_info_get_bulk(void *pdata, int data_len, struct ospf_dcn_ne_info sdcn_ne_info_buff[])
{
	u_int32_t dcn_ne_id = *((u_int32_t *)pdata);
	struct ospf_dcn_ne_info s_ospf_dcn_ne_info;
	struct ospf *ospf = NULL;
	struct ospf_lsa *lsa;
	struct route_node *rn;
	struct ospf_area *area = NULL;
	struct listnode *node = NULL,
					*node1 = NULL,
	                *nnode = NULL;
	int msg_num  = IPC_MSG_LEN/sizeof(struct ospf_dcn_ne_info);
	int data_num = 0;
	int flag_start = 0;

	if (listcount (om->ospf) == 0)
    {
        return 0;
    }

	if(dcn_ne_id == 0)/* 1st get*/
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{
			if(ospf->ospf_id == OSPF_MAX_PROCESS_ID)
			{
				for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
				{
					if (ospf_lsdb_count_self (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0 ||
                    	(ospf_lsdb_count (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0))
					{
						LSDB_LOOP (OPAQUE_AREA_LSDB(area), rn, lsa)
						{
							memset(&s_ospf_dcn_ne_info, 0, sizeof(struct ospf_dcn_ne_info));
							if(!get_dcn_ne_info_from_tlv(lsa, ospf, &s_ospf_dcn_ne_info))
							{
								memcpy(&sdcn_ne_info_buff[data_num++], &s_ospf_dcn_ne_info, sizeof(struct ospf_dcn_ne_info));
					            if (data_num == msg_num)
					            {
					                return data_num;
					            }
							}
						}
					}
					
				}
			}
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{
			if(ospf->ospf_id == OSPF_MAX_PROCESS_ID)
			{
				for (ALL_LIST_ELEMENTS_RO (ospf->areas, node1, area))
				{
					if (ospf_lsdb_count_self (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0 ||
                    	(ospf_lsdb_count (area->lsdb, OSPF_OPAQUE_AREA_LSA) > 0))
					{
						LSDB_LOOP (OPAQUE_AREA_LSDB(area), rn, lsa)
						{
							/*if(dcn_ne_id == lsa->data->adv_router.s_addr)
							{
								flag_start = 1;
								continue;
							}
							if(flag_start)
							{*/
								memset(&s_ospf_dcn_ne_info, 0, sizeof(struct ospf_dcn_ne_info));
								if(!get_dcn_ne_info_from_tlv(lsa, ospf, &s_ospf_dcn_ne_info))
								{
									if(s_ospf_dcn_ne_info.dcn_ne_id == dcn_ne_id)
									{
										flag_start = 1;
										continue;
									}
									if(flag_start)
									{
										memcpy(&sdcn_ne_info_buff[data_num++], &s_ospf_dcn_ne_info, sizeof(struct ospf_dcn_ne_info));
							            if (data_num == msg_num)
							            {
							                return data_num;
							            }
									}
								}
							//}
						}
					}
				}
			}
		}
	}
	return data_num;
}

static int
h3c_ospf_area_get_bulk(void *pdata, int data_len,
                            struct statics_ospf_area_h3c ospf_area_h3c_buff[],uint32_t msg_index)
{    
	u_int32_t area_id = *((u_int32_t*)pdata);
    struct statics_ospf_area_h3c  h3c_area;
    struct ospf *ospf = NULL;
    struct ospf_area *area = NULL;
    struct listnode *node = NULL;
    int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_area_h3c);
    int data_num = 0;
	//int flag = 0;
    
    if (listcount (om->ospf) == 0)
    {
        return 0;
    }

	ospf = listgetdata ((struct listnode *)listhead (om->ospf));
	if(NULL == ospf)
	{
		return 0;
	}
	
    zlog_debug(OSPF_DBG_OTHER, "%s %d key %d",__func__,__LINE__,msg_index);
	
	for (ALL_LIST_ELEMENTS_RO (ospf->areas, node, area))
    {
		if(msg_index == OSPF_START)
		{
			memset(&h3c_area, 0, sizeof(struct statics_ospf_area_h3c));
	  		h3c_area.area_id = area->area_id.s_addr;
			h3c_area.auth_type = area->auth_type;
			h3c_area.external_routing = area->external_routing;
			h3c_area.spf_calculation = area->spf_calculation;
			h3c_area.abr_count = area->abr_count;
			h3c_area.asbr_count = area->asbr_count;
			h3c_area.lsa_count = ospf_lsdb_count_all(area->lsdb);
			h3c_area.summary = area->no_summary;
			h3c_area.NSSATranslatorRole = area->NSSATranslatorRole;
			h3c_area.NSSATranslatorState = area->NSSATranslatorState;
			h3c_area.NSSATranslatorStabilityInterval = area->NSSATranslatorStabilityInterval;

			zlog_err("%s %d AREA:%d auth_type:%d spf_calculation:%d",__func__,__LINE__,h3c_area.area_id,h3c_area.auth_type,
					area->spf_calculation);

	        memcpy(&ospf_area_h3c_buff[data_num++], &h3c_area, sizeof(struct statics_ospf_area_h3c));
            zlog_err("%s %d AREA:%d auth_type:%d spf_calculation:%d",__func__,__LINE__,ospf_area_h3c_buff[data_num-1].area_id,ospf_area_h3c_buff[data_num-1].auth_type,
					area->spf_calculation);
	        if (data_num == msg_num)
	        {
	            return data_num;
	        }
		}
		else if(area->area_id.s_addr > area_id)
		{
			memset(&h3c_area, 0, sizeof(struct statics_ospf_area_h3c));
	  		h3c_area.area_id = area->area_id.s_addr;
			h3c_area.auth_type = area->auth_type;
			h3c_area.external_routing = area->external_routing;
			h3c_area.spf_calculation = area->spf_calculation;
			h3c_area.abr_count = area->abr_count;
			h3c_area.asbr_count = area->asbr_count;
			h3c_area.lsa_count = ospf_lsdb_count_all(area->lsdb);
			h3c_area.summary = area->no_summary;
			h3c_area.NSSATranslatorRole = area->NSSATranslatorRole;
			h3c_area.NSSATranslatorState = area->NSSATranslatorState;
			h3c_area.NSSATranslatorStabilityInterval = area->NSSATranslatorStabilityInterval;

			zlog_err("%s %d AREA:%d auth_type:%d spf_calculation:%d",__func__,__LINE__,h3c_area.area_id,h3c_area.auth_type,
					area->spf_calculation);

	        memcpy(&ospf_area_h3c_buff[data_num++], &h3c_area, sizeof(struct statics_ospf_area_h3c));
	        if (data_num == msg_num)
	        {
	            return data_num;
	        }
		}

    }

	

	return data_num;

}

static int
h3c_ospf_network_get_bulk(void *pdata, int data_len, struct statics_ospf_network snetwork_buff[])
{
	struct ospf_key_network_h3c ospf_key_h3c = *((struct ospf_key_network_h3c*)pdata);
	struct statics_ospf_network s_network;
	struct ospf *ospf = NULL;
	struct route_node *rn = NULL;	
	struct route_node *rn1 = NULL;	
	struct route_node *rn_temp = NULL;
	struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	
	int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_network);
	int data_num = 0;
	
	if (listcount (om->ospf) == 0)
	{
		return 0;
	}
	
	zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%d\n",__func__,__LINE__,ospf_key_h3c.ospf_id,
		ospf_key_h3c.area_id,ospf_key_h3c.prefix4);
	
	if(ospf_key_h3c.ospf_id == 0)	/*	1st get */
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{  
			for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
			{
				if (rn->info)
				{
					struct ospf_network *n = rn->info;
					
					if(data_num != 0 
							&& (snetwork_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
							&& (snetwork_buff[data_num-1].area_id == n->area_id.s_addr))
					{
						continue;
					}
					
					memset(&s_network, 0, sizeof(struct statics_ospf_network));
					s_network.ospf_id = ospf->ospf_id;
					s_network.area_id = n->area_id.s_addr;
					s_network.p.family = AF_INET;
					s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
					s_network.p.prefixlen= rn->p.prefixlen;
					zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
					memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
					if (data_num == msg_num)
					{
						return data_num;
					}
					
					
				}
			}
			
		}
	}
	else
	{
		for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
		{
			if(ospf_key_h3c.ospf_id == ospf->ospf_id)
			{	
				/* look up rm_temp by key */
				for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
				{
					if (rn->info)
					{
						struct ospf_network *n = rn->info;

						if((ospf_key_h3c.prefix4 == rn->p.u.prefix4.s_addr)
							&& (ospf_key_h3c.area_id == n->area_id.s_addr))
						{
							rn_temp = rn;
							break;
						}				
						
					}
				}
				
				if(rn_temp != NULL)
				{
					for (rn1 = rn_temp; rn1; rn1 = route_next (rn1))
					{
						if(rn1 == rn_temp)
						{
							continue;
						}
						
						if (rn1->info)
						{
							struct ospf_network *n1 = rn1->info;
							
							if(data_num != 0 
								&& (snetwork_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
								&& (snetwork_buff[data_num-1].area_id == n1->area_id.s_addr))
							{
								continue;
							}
							
							memset(&s_network, 0, sizeof(struct statics_ospf_network));
							s_network.ospf_id = ospf->ospf_id;
							s_network.area_id = n1->area_id.s_addr;
							s_network.p.family = AF_INET;
							s_network.p.u.prefix4.s_addr = rn1->p.u.prefix4.s_addr;
							s_network.p.prefixlen = rn1->p.prefixlen;
							zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
							memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
							if (data_num == msg_num)
							{
								return data_num;
							}
							
						}
					}
						
				}
			}
			else if(ospf->ospf_id > ospf_key_h3c.ospf_id)
			{
				for (rn = route_top (ospf->networks); rn; rn = route_next (rn))
				{
					if (rn->info)
					{
						struct ospf_network *n = rn->info;
						
						if(data_num != 0 
								&& (snetwork_buff[data_num-1].p.u.prefix4.s_addr == rn->p.u.prefix4.s_addr)
								&& (snetwork_buff[data_num-1].area_id == n->area_id.s_addr))
						{
							continue;
						}
						
						memset(&s_network, 0, sizeof(struct statics_ospf_network));
						s_network.ospf_id = ospf->ospf_id;
						s_network.area_id = n->area_id.s_addr;
						s_network.p.family = AF_INET;
						s_network.p.u.prefix4.s_addr = rn->p.u.prefix4.s_addr;
						s_network.p.prefixlen = rn->p.prefixlen;
						zlog_debug(OSPF_DBG_OTHER, "%s %d ospf_id:%d area:%d network:%s/%d\n",__func__,__LINE__,s_network.ospf_id,
								s_network.area_id,inet_ntoa(s_network.p.u.prefix4),s_network.p.prefixlen);
						memcpy(&snetwork_buff[data_num++], &s_network, sizeof(struct statics_ospf_network));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}
			}
		}
	}
	return data_num;
}

static int
h3c_ospf_interface_get_bulk(void *pdata, int data_len
                                    , struct statics_ospf_if_h3c interface_h3c_buff[],uint32_t msg_index)
{	 
	struct ospf_key_prefix_h3c key_h3c = *((struct ospf_key_prefix_h3c*)pdata);;
	struct ospf *ospf = NULL;
	struct ospf_interface *oi = NULL;
	struct statics_ospf_if_h3c if_h3c;
	struct listnode *node = NULL,*n2 = NULL;
    struct crypt_key *ck;
	int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_if_h3c);
	int data_num = 0;
	int flag = 0;
    struct ospf_neighbor *nbr;
	
	if (listcount (om->ospf) == 0)
	{
		return 0;
	}

	ospf = listgetdata ((struct listnode *)listhead (om->ospf));
	if(NULL == ospf)
	{
		return 0;
	}
	
    zlog_debug(OSPF_DBG_OTHER, "%s %d key %d, index %d,prefix4 %d",__func__,__LINE__,msg_index,key_h3c.ifindex,key_h3c.prefix4);

	for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
	{
		if((oi->ifp->ifindex == key_h3c.ifindex) && (key_h3c.prefix4 == oi->address->u.prefix4.s_addr))
		{
			flag = 1;
			continue;
		}	
		
		if((msg_index == OSPF_START) || (flag == 1)
                            ||(msg_index == OSPF_START0 && (0 == key_h3c.ifindex &&(0 == key_h3c.prefix4))))
		{
			memset(&if_h3c, 0, sizeof(struct statics_ospf_if_h3c));
			if_h3c.prefix4 = oi->address->u.prefix4.s_addr;
			if_h3c.ifindex = oi->ifp->ifindex;
			if_h3c.area_id = oi->area->area_id.s_addr;
			if_h3c.type = ospf_snmp_iftype_h3c(oi->ifp);
			//if_h3c.IfAdminStat; 
			if_h3c.ospfIfRtrPriority = IF_DEF_PARAMS(oi->ifp)->priority;
        	if_h3c.ospfIfTransitDelay = IF_DEF_PARAMS(oi->ifp)->retransmit_interval;
			if_h3c.ospfIfRetransInterval = IF_DEF_PARAMS(oi->ifp)->transmit_delay;
			if_h3c.ospfIfHelloInterval =  IF_DEF_PARAMS(oi->ifp)->v_hello;
			if_h3c.ospfIfRtrDeadInterval =  IF_DEF_PARAMS(oi->ifp)->v_wait;
			//if_h3c.ospfIfPollInterval =  oi->nbr_nbma->->v_poll;
			if_h3c.ospfIfState = oi->state;
			if_h3c.ospfIfEvents = oi->state_change;
			if_h3c.ospfIfAuthType = IF_DEF_PARAMS(oi->ifp)->auth_type;
			
			if(IF_DEF_PARAMS(oi->ifp)->auth_type == OSPF_AUTH_SIMPLE )
			{
				strncpy((char *)if_h3c.ospfIfAuthKey , (char *) OSPF_IF_PARAM(oi, auth_simple), OSPF_AUTH_SIMPLE_SIZE);
				zlog_debug(OSPF_DBG_OTHER, "%s %d , priority: %s",__func__,__LINE__,if_h3c.ospfIfAuthKey );
			}                
           	else if(IF_DEF_PARAMS(oi->ifp)->auth_type == OSPF_AUTH_CRYPTOGRAPHIC)
           	{
                for (ALL_LIST_ELEMENTS_RO(IF_DEF_PARAMS(oi->ifp)->auth_crypt, n2, ck))
                {
					strncpy((char *)if_h3c.ospfIfAuthKey , (char *) ck->auth_key, OSPF_AUTH_MD5_SIZE);           
					zlog_debug(OSPF_DBG_OTHER, "%s %d , priority: %s",__func__,__LINE__,if_h3c.ospfIfAuthKey );
                }
            }
			/*
			ospfIfBackupDesignatedRouter;
			ospfIfStatus;
			ospfIfMulticastForwarding;
			ospfIfDemand;			
			ospfIfLsaCount;
			ospfIfLsaCksumSum;*/	
			if (DR(oi).s_addr != 0)
            {
                nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &DR(oi));

                if (nbr != NULL)
                {
                    if_h3c.IfDesignatedRouterid = nbr->router_id.s_addr;
                    if_h3c.ospfIfDesignatedRouter = nbr->address.u.prefix4.s_addr;
                }
            }
             
            if (BDR(oi).s_addr != 0)
            {
                nbr = ospf_nbr_lookup_by_addr(oi->nbrs, &BDR(oi));

                if (nbr != NULL)
                {
                    if_h3c.IfBackupDesignatedRouterid = nbr->router_id.s_addr;
                    if_h3c.ospfIfBackupDesignatedRouter = nbr->address.u.prefix4.s_addr;                    
                }
            }            
			
            zlog_debug(OSPF_DBG_OTHER, "%s %d , DR: %d BDR:%d",__func__,__LINE__,if_h3c.ospfIfDesignatedRouter, if_h3c.IfBackupDesignatedRouterid);
            zlog_debug(OSPF_DBG_OTHER, "%s %d , index %d,prefix4 %d AREA:%d",__func__,__LINE__,if_h3c.ifindex,if_h3c.prefix4,if_h3c.area_id);

           	memcpy(&interface_h3c_buff[data_num++], &if_h3c, sizeof(struct statics_ospf_if_h3c));
			if (data_num == msg_num)
			{
				return data_num;
			}
		}
		
	}
	return data_num;

}


static int
h3c_ospf_neighbor_get_bulk(void *pdata, int data_len
                    , struct statics_ospf_neighbor_h3c neighbor_h3c_buff[],uint32_t msg_index)
{	 
	struct ospf_key_prefix_h3c key_h3c = *((struct ospf_key_prefix_h3c*)pdata);;
	struct ospf *ospf = NULL;
	struct ospf_interface *oi = NULL;    
	struct route_node *rn = NULL;
    struct ospf_neighbor *nbr = NULL;
	struct statics_ospf_neighbor_h3c neighbor_h3c;
	struct listnode *node = NULL;
	int msg_num  = IPC_MSG_LEN/sizeof(struct statics_ospf_neighbor_h3c);
	int data_num = 0;
	int flag = 0;
	
	if (listcount (om->ospf) == 0)
	{
		return 0;
	}

	ospf = listgetdata ((struct listnode *)listhead (om->ospf));
	if(NULL == ospf)
	{
		return 0;
	}
	
    zlog_debug(OSPF_DBG_OTHER, "%s %d key %d, index %d,prefix4 %d",__func__,__LINE__,msg_index,key_h3c.ifindex,key_h3c.prefix4);

	for (ALL_LIST_ELEMENTS_RO (ospf->oiflist, node, oi))
	{
        for (rn = route_top(oi->nbrs); rn; rn = route_next(rn))
        {   
			if ((nbr = rn->info))
            {
                /* Do not show myself. */
                if (nbr != oi->nbr_self)
                {
                    /* Down state is not shown. */
                    if (nbr->state != NSM_Down)
                    {
                    	if((oi->ifp->ifindex == key_h3c.ifindex) && (key_h3c.prefix4 == oi->address->u.prefix4.s_addr))
    					{
    						flag = 1;
    						continue;
    					}	
    					
    					if((msg_index == OSPF_START) || (flag == 1)
								||(msg_index == OSPF_START0 && (0 == key_h3c.ifindex &&(0 == key_h3c.prefix4))))
    					{
    						memset(&neighbor_h3c, 0, sizeof(struct statics_ospf_neighbor_h3c));
    						neighbor_h3c.prefix4 = nbr->address.u.prefix4.s_addr;
    						neighbor_h3c.ifindex = nbr->oi->ifp->ifindex;
    						neighbor_h3c.router_id = nbr->router_id.s_addr;
    						neighbor_h3c.options = nbr->options;
    						neighbor_h3c.priority = nbr->priority;
    						neighbor_h3c.state = ospf_snmp_neighbor_state_h3c(nbr->state);
    						neighbor_h3c.state_change = nbr->state_change;
    						//neighbor_h3c.NbrLsRetransQLen ;

    						memcpy(&neighbor_h3c_buff[data_num++], &neighbor_h3c, sizeof(struct statics_ospf_neighbor_h3c));
                            
                            zlog_debug(OSPF_DBG_OTHER, "%s %d , index %d,prefix4 %d AREA:%d state:%d",__func__,__LINE__
                               ,neighbor_h3c.ifindex,neighbor_h3c.prefix4,neighbor_h3c.router_id,neighbor_h3c.state);
    						if (data_num == msg_num)
    						{
    							return data_num;
    						}
    					}
    			    }
                }
    		}
		}
		
	}

	return data_num;

}

void ospf_dcn_ne_info_trap(struct ospf_dcn_ne_info *ne_info, int noline_flag)
{
	if(IS_DEBUG_OSPF_DCN)
	{
		zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	if(noline_flag)
	{
		/*ipc_send_common(ne_info,sizeof(struct ospf_dcn_ne_info),1,\
			MODULE_ID_SNMPD,MODULE_ID_OSPF,IPC_TYPE_SNMP,OSPF_DCN_NE_ONLINE,IPC_OPCODE_UPDATE);*///fix subtype
		ipc_send_msg_n2(ne_info,sizeof(struct ospf_dcn_ne_info),1,\
			MODULE_ID_SNMPD_TRAP,MODULE_ID_OSPF,IPC_TYPE_SNMP,OSPF_DCN_NE_ONLINE,IPC_OPCODE_UPDATE,0);//fix subtype
	}
	else
	{
		/*ipc_send_common(ne_info,sizeof(struct ospf_dcn_ne_info),1,\
			MODULE_ID_SNMPD,MODULE_ID_OSPF,IPC_TYPE_SNMP,OSPF_DCN_NE_OFFLINE,IPC_OPCODE_UPDATE);*///fix subtype
		ipc_send_msg_n2(ne_info,sizeof(struct ospf_dcn_ne_info),1,\
			MODULE_ID_SNMPD_TRAP,MODULE_ID_OSPF,IPC_TYPE_SNMP,OSPF_DCN_NE_OFFLINE,IPC_OPCODE_UPDATE,0);//fix subtype
	}
}

static void ospf_snmp_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
    int ret = 0;
    int msg_len = 0;
	
    zlog_debug(OSPF_DBG_OTHER, "%s %d TYPE %d",__func__,__LINE__,pmsghdr->msg_subtype);

	switch(pmsghdr->msg_subtype)
	{
   		case OSPF_SNMP_INSTANCE_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf);
		        struct statics_ospf sospf_buff[IPC_MSG_LEN/sizeof(struct statics_ospf)];
		        memset(sospf_buff, 0, msg_len*sizeof(struct statics_ospf));
		        ret = static_ospf_get_bulk(pdata, pmsghdr->data_len, sospf_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_n2(sospf_buff, ret*sizeof(struct statics_ospf), ret, pmsghdr->sender_id,
                                      MODULE_ID_OSPF,IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->sequence, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, sospf_buff, ret*sizeof(struct statics_ospf), ret);
		        }		        
				else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
            		ipc_noack_to_snmp(pmsghdr);		        
				}
		    }
			break;
    	case OSPF_SNMP_AREA_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_area);
		        struct statics_ospf_area sarea_buff[IPC_MSG_LEN/sizeof(struct statics_ospf_area)];
		        memset(sarea_buff, 0, msg_len*sizeof(struct statics_ospf_area));
		        ret = static_ospf_area_get_bulk(pdata, pmsghdr->data_len, sarea_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(sarea_buff, ret*sizeof(struct statics_ospf_area), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, sarea_buff, ret*sizeof(struct statics_ospf_area), ret);
        		}
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_noack_to_snmp(pmsghdr);
        		}
		    }
			break;
    	case OSPF_SNMP_NETWORK_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_network);
		        struct statics_ospf_network snetwork_buff[IPC_MSG_LEN/sizeof(struct statics_ospf_network)];
		        memset(snetwork_buff, 0, msg_len*sizeof(struct statics_ospf_network));
		        ret = static_ospf_network_get_bulk(pdata, pmsghdr->data_len, snetwork_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(snetwork_buff, ret*sizeof(struct statics_ospf_network), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, snetwork_buff, ret*sizeof(struct statics_ospf_network), ret);
        		}
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_noack_to_snmp(pmsghdr);
        		}
		    }
			break;
    	case OSPF_SNMP_INTERFACE_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_if);
		        struct statics_ospf_if sif_buff[IPC_MSG_LEN/sizeof(struct statics_ospf_if)];
		        memset(sif_buff, 0, msg_len*sizeof(struct statics_ospf_if));
		        ret = static_ospf_if_config_get_bulk(pdata, pmsghdr->data_len, sif_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(sif_buff, ret*sizeof(struct statics_ospf_if), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, sif_buff, ret*sizeof(struct statics_ospf_if), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_noack_to_snmp(pmsghdr);
		        }
		    }
			break;
		case OSPF_SNMP_DCN_CONF_GET:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct ospf_dcn_config);
				struct ospf_dcn_config sdcn_conf_buff[IPC_MSG_LEN/sizeof(struct ospf_dcn_config)];
				memset(sdcn_conf_buff, 0, msg_len*sizeof(struct ospf_dcn_config));
				ret = ospf_dcn_conf_get_bulk(pdata, pmsghdr->data_len, sdcn_conf_buff);
				if(IS_DEBUG_OSPF_DCN)
				{
					zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': ospf_dcn_conf_get_bulk() return ret = %d\n ", __FILE__, __LINE__, __func__, ret);
				}
				if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(sdcn_conf_buff, ret*sizeof(struct ospf_dcn_config), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, sdcn_conf_buff, ret*sizeof(struct ospf_dcn_config), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_noack_to_snmp(pmsghdr);
		        }
			}
			break;
		case OSPF_SNMP_DCN_NE_INFO_GET:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct ospf_dcn_ne_info);
				struct ospf_dcn_ne_info sdcn_ne_info_buff[IPC_MSG_LEN/sizeof(struct ospf_dcn_ne_info)];
				memset(sdcn_ne_info_buff, 0, msg_len*sizeof(struct ospf_dcn_ne_info));
				ret = ospf_dcn_ne_info_get_bulk(pdata, pmsghdr->data_len, sdcn_ne_info_buff);
				if(IS_DEBUG_OSPF_DCN)
				{
					zlog_debug(OSPF_DBG_DCN, "%s[%d]: In function '%s': ospf_dcn_ne_info_get_bulk() return ret = %d\n",__FILE__, __LINE__, __func__, ret);
				}
				if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(sdcn_ne_info_buff, ret*sizeof(struct ospf_dcn_ne_info), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, sdcn_ne_info_buff, ret*sizeof(struct ospf_dcn_ne_info), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_noack_to_snmp(pmsghdr);
		        }
			}
			break;
		case OSPF_SNMP_AREA_GET_H3C:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_area_h3c);
				struct statics_ospf_area_h3c ospf_area_h3c[IPC_MSG_LEN/sizeof(struct statics_ospf_area_h3c)];
				memset(ospf_area_h3c, 0, msg_len*sizeof(struct statics_ospf_area_h3c));
				ret = h3c_ospf_area_get_bulk(pdata, pmsghdr->data_len, ospf_area_h3c,pmsghdr->msg_index);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(ospf_area_h3c, ret*sizeof(struct statics_ospf_area_h3c), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
		            ipc_ack_to_snmp(pmsghdr, ospf_area_h3c, ret*sizeof(struct statics_ospf_area_h3c), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
				
			}
			break;
		case OSPF_SNMP_NETWORK_GET_H3C:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_network);
		        struct statics_ospf_network snetwork_buff[IPC_MSG_LEN/sizeof(struct statics_ospf_network)];
		        memset(snetwork_buff, 0, msg_len*sizeof(struct statics_ospf_network));
		        ret = h3c_ospf_network_get_bulk(pdata, pmsghdr->data_len, snetwork_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(snetwork_buff, ret*sizeof(struct statics_ospf_network), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, snetwork_buff, ret*sizeof(struct statics_ospf_network), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
			}
			break;
		case OSPF_SNMP_INTERFACE_GET_H3C:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_if_h3c);
				struct statics_ospf_if_h3c interface_h3c[IPC_MSG_LEN/sizeof(struct statics_ospf_if_h3c)];
		        memset(interface_h3c, 0, msg_len*sizeof(struct statics_ospf_if_h3c));
		        ret = h3c_ospf_interface_get_bulk(pdata, pmsghdr->data_len, interface_h3c, pmsghdr->msg_index);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(interface_h3c, ret*sizeof(struct statics_ospf_if_h3c), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, interface_h3c, ret*sizeof(struct statics_ospf_if_h3c), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
			}
			break;
		case OSPF_SNMP_NEIGHBOR_GET_H3C:
			{
				msg_len = IPC_MSG_LEN/sizeof(struct statics_ospf_neighbor_h3c);
				struct statics_ospf_neighbor_h3c neighbor_h3c[IPC_MSG_LEN/sizeof(struct statics_ospf_neighbor_h3c)];
		        memset(neighbor_h3c, 0, msg_len*sizeof(struct statics_ospf_neighbor_h3c));
		        ret = h3c_ospf_neighbor_get_bulk(pdata, pmsghdr->data_len, neighbor_h3c, pmsghdr->msg_index);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(neighbor_h3c, ret*sizeof(struct statics_ospf_neighbor_h3c), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, neighbor_h3c, ret*sizeof(struct statics_ospf_neighbor_h3c), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
			}
			break;
		case OSPF_SNMP_ABR_SUMMARY_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statistics_ospf_abr_summary_info);
		        struct statistics_ospf_abr_summary_info abr_sum_buff[msg_len];
		        memset(abr_sum_buff, 0, msg_len*sizeof(struct statistics_ospf_abr_summary_info));
		        ret = statistics_ospf_abr_summary_get_bulk(pdata, pmsghdr->data_len, abr_sum_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(abr_sum_buff, ret*sizeof(struct statistics_ospf_abr_summary_info), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, abr_sum_buff, ret*sizeof(struct statistics_ospf_abr_summary_info), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
		    }
			break;
		case OSPF_SNMP_ASBR_SUMMARY_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statistics_ospf_asbr_summary_info);
		        struct statistics_ospf_asbr_summary_info asbr_sum_buff[msg_len];
		        memset(asbr_sum_buff, 0, msg_len*sizeof(struct statistics_ospf_asbr_summary_info));
		        ret = statistics_ospf_asbr_summary_get_bulk(pdata, pmsghdr->data_len, asbr_sum_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(asbr_sum_buff, ret*sizeof(struct statistics_ospf_asbr_summary_info), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, asbr_sum_buff, ret*sizeof(struct statistics_ospf_asbr_summary_info), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
		    }
			break;
		case OSPF_SNMP_REDIS_ROUTE_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statistics_ospf_redistribute_route_info);
		        struct statistics_ospf_redistribute_route_info redis_route_buff[msg_len];
		        memset(redis_route_buff, 0, msg_len*sizeof(struct statistics_ospf_redistribute_route_info));
		        ret = statistics_ospf_redistribute_route_get_bulk(pdata, pmsghdr->data_len, redis_route_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(redis_route_buff, ret*sizeof(struct statistics_ospf_redistribute_route_info), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, redis_route_buff, ret*sizeof(struct statistics_ospf_redistribute_route_info), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
		    }
			break;
		case OSPF_SNMP_NBMA_NEIGHBOR_GET:
		    {
		        msg_len = IPC_MSG_LEN/sizeof(struct statistics_ospf_nbma_neighbor_info);
		        struct statistics_ospf_nbma_neighbor_info nbma_peer_buff[msg_len];
		        memset(nbma_peer_buff, 0, msg_len*sizeof(struct statistics_ospf_nbma_neighbor_info));
		        ret = statistics_ospf_nbma_neighbor_info_get_bulk(pdata, pmsghdr->data_len, nbma_peer_buff);
		        if (ret > 0)
		        {
		            /*ret = ipc_send_reply_bulk(nbma_peer_buff, ret*sizeof(struct statistics_ospf_nbma_neighbor_info), ret, pmsghdr->sender_id,
		                                      MODULE_ID_OSPF, IPC_TYPE_OSPF, pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_ack_to_snmp(pmsghdr, nbma_peer_buff, ret*sizeof(struct statistics_ospf_nbma_neighbor_info), ret);
		        }
		        else
		        {
		            /*ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_OSPF, IPC_TYPE_OSPF,
		                                 pmsghdr->msg_subtype, pmsghdr->msg_index);*/
					ipc_noack_to_snmp(pmsghdr);
		        }
		    }
			break;
		default:
			break;
	}
    return ;
}


void ospf_ifm_manage(struct ifm_event pevent, int event)
{
    vrf_id_t vrf_id = VRF_DEFAULT;
    
    switch (event)
    {
    case IFNET_EVENT_IP_ADD:
		if (pevent.ipaddr.type == INET_FAMILY_IPV4)
		{
	        if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
	        {
	        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event IP_ADD ifindex 0x%0x\n", __FUNCTION__, __LINE__, pevent.ifindex);
	        }
	        ospf_interface_address_add(pevent, vrf_id);
		}
		else if(pevent.ipaddr.type == INET_FAMILY_IPV6)
		{
			zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event IPV6_ADD ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        break;
    case IFNET_EVENT_IP_DELETE:		
		if (pevent.ipaddr.type == INET_FAMILY_IPV4)
		{
			if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
	        {
	        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event IP_DEL ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
			}
	        ospf_interface_address_delete(pevent, vrf_id);
		}
		else if(pevent.ipaddr.type == INET_FAMILY_IPV6)
		{
			zlog_debug(OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event IPV6_DEL ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        break;
    case IFNET_EVENT_UP:
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event UP ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        ospf_interface_state_up (pevent, vrf_id);
        break;
    case IFNET_EVENT_DOWN:
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event DOWN ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        ospf_interface_state_down (pevent, vrf_id);
        break;
    case IFNET_EVENT_IF_DELETE:
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event IF_DEL ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        ospf_interface_delete (pevent, vrf_id);
        break;
    case IFNET_EVENT_MODE_CHANGE:
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
			zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event MODE ifindex 0x%0x change\n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        ospf_interface_mode_change (pevent, vrf_id);
        break;
    case IFNET_EVENT_MTU_CHANGE:
		if (IS_DEBUG_OSPF (zebra, ZEBRA_INTERFACE))
        {
        	zlog_debug (OSPF_DBG_ZEBRA_INTERFACE, "%s[%d] recv event MTU ifindex 0x%0x change\n", __FUNCTION__, __LINE__, pevent.ifindex);
		}
        ospf_interface_mtu_change (pevent, vrf_id);
        break;
	case IFNET_EVENT_L3VPN:
		break;
    default:
        break;
    }
    return;
}

static void ospf_bfd_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct bfd_sess *session_info = NULL;

	if(pmsghdr->msg_type == IPC_TYPE_BFD ) //&& pmsghdr->msg_subtype == IPC_OPCODE_EVENT)
	{
		session_info = (struct bfd_sess *)(pdata); 
		if(NULL != session_info)
		{
			if(pmsghdr->msg_subtype == OSPF_SUBTYPE_GET_REPLY)//接收session的相关参数
			{
				ospf_bfd_session_info_load(session_info);
			}
		}
	}

	return;
}
static void ospf_vpn_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	struct l3vpn_entry *l3vpn_info  = NULL;
    struct listnode *node = NULL, *nnode = NULL;
    struct ospf *ospf = NULL;
	
	l3vpn_info = (struct l3vpn_entry *)pdata;

	if(l3vpn_info == NULL || l3vpn_info->l3vpn.vrf_id > 128)
		return;
	
	switch (pmsghdr->opcode)
	{
		case IPC_OPCODE_DELETE:
		{
			for (ALL_LIST_ELEMENTS (om->ospf, node, nnode, ospf))
			{
				if(ospf->vpn == l3vpn_info->l3vpn.vrf_id)
				{
					ospf_finish (ospf);
				}
			}
			break;
		}
		
		default:
			break;
	}

}

static void ospf_bfd_rcv_event(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, "%s[%d]: In function '%s' ", __FILE__, __LINE__, __func__);
	}
	struct bfd_info *base_sess = NULL;

	base_sess = (struct bfd_info *)(pdata);
	if(NULL == base_sess)
	{
		return;
	}

	if(IS_DEBUG_OSPF_BFD)
	{
		zlog_debug(OSPF_DBG_BFD, " Recv event id from BFD is: %d\n", pmsghdr->msg_subtype);
	}
	switch(pmsghdr->msg_subtype)
	{
		case 0://NEIGHBOR_DOWN
		case 1://DETECT_EXPIRE
			ospf_bfd_session_down(base_sess);
			break;
		case 2://ADMIN_DOWN
		
			break;
		case 3://FIRST_PKT bfd会话建立后的首包，通过该包携带的会话信息来填充显示内容
			ospf_bfd_session_status_update(base_sess);
			break;
		default:
			break;
	}

	return;
}

static void u0_device_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	uint16_t ospf_id = OSPF_MAX_PROCESS_ID;
	struct ospf * ospf = NULL;
	struct u0_device_info *p_u0_info = NULL;
	
	if(NULL == pdata)
	{
		OSPF_LOG_DEBUG("pdata == NULL, return!");
		return;
	}
	
	ospf = ospf_lookup_id (ospf_id);
	if(!ospf)
	{
		OSPF_LOG_DEBUG("DCN is not enable, can not handle u0 online!");
		return;
	}
	
	p_u0_info = (struct u0_device_info *)pdata;
	
	switch(pmsghdr->opcode)
	{
		case IPC_OPCODE_ADD://U0 online
			u0_device_add_handle(p_u0_info);
			break;
		case IPC_OPCODE_DELETE://U0 offline
			u0_device_del_handle(p_u0_info);
			break;
		case IPC_OPCODE_UPDATE://U0 update
			u0_device_update_handle(p_u0_info);
			break;
		default:
			break;
	}
	return ;	
}

static void ospf_dcn_msg_handle(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	uint16_t ospf_id = 0;
	int ret = 0;
	struct ospf * ospf = NULL;

	if(NULL == pdata)
	{
		OSPF_LOG_ERROR("pdata == NULL, return!");
		return;
	}
	
	ospf_id = OSPF_MAX_PROCESS_ID;
	
	switch(pmsghdr->msg_subtype)
	{
		case IPC_SUB_TYPE_DCN_STATUS:
		{
			if(pmsghdr->opcode == IPC_OPCODE_GET)
			{
				ospf = ospf_lookup_id (ospf_id);
				if(ospf)
				{
					ret = ipc_send_reply_n2(&(ospf->dcn_enable), sizeof(u_char), 1, pmsghdr->sender_id, MODULE_ID_OSPF,
		                            	IPC_TYPE_OSPF_DCN, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_REPLY);
					
				}
				else
				{
					OSPF_LOG_ERROR("ospf instance 255 is not exist!!!");
					ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, MODULE_ID_OSPF, 
										IPC_TYPE_OSPF_DCN, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_NACK);
					if(ret < 0)
					{
						OSPF_LOG_ERROR("ospf instance 255 is not exist and send noack fail!!!");
					}
				}
			}
			break;
		}
		default:
			break;
	}
	return;
}

#if 0
/*interface information handle*/
void ospf_common_manage(struct ipc_mesg mesg)
{
    struct ifm_event pevent;
    struct ipc_msghdr *pmsghdr = NULL;
    void *pdata = NULL;
    memset(&pevent,0,sizeof(struct ifm_event));
    pmsghdr = &(mesg.msghdr);
    pdata = mesg.msg_data;

	switch(pmsghdr->msg_type)
	{
		case IPC_TYPE_IFM:
		{
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
	        {
	            pevent = *((struct ifm_event*)pdata);
	            ospf_ifm_manage(pevent, pmsghdr->msg_subtype);
	        }
			break;
		}
		case IPC_TYPE_OSPF:
		{
			if(pmsghdr->opcode == IPC_OPCODE_GET_BULK)
	        {
	        	ospf_snmp_manage(pdata, pmsghdr);
	        }
			break;
		}
		case IPC_TYPE_BFD:
		{
			if(pmsghdr->opcode == IPC_OPCODE_UPDATE)
	        {
	            ospf_bfd_manage(pdata, pmsghdr);//接收bfd创建会话后返回的参数
	        }
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				ospf_bfd_rcv_event(pdata, pmsghdr);//处理bfd上报的事件
			}
			break;
		}
		case IPC_TYPE_L3VPN:
		{
			ospf_vpn_manage(pdata,pmsghdr);
			break;
		}
		case IPC_TYPE_EFM:
		{
			u0_device_manage(pdata,pmsghdr);
			break;
		}
		case IPC_TYPE_OSPF_DCN:
		{
	        ospf_dcn_msg_handle(pdata, pmsghdr);
			break;
		}
		default:
		{
			OSPF_LOG_DEBUG("Recv invalid IPC TYPE %d!!!", pmsghdr->msg_type);
			break;
		}
	}
   
    return;
}
#endif

/*interface information handle*/
void ospf_common_manage_new(struct ipc_mesg_n *mesg)
{
    struct ifm_event pevent;
    struct ipc_msghdr_n *pmsghdr = NULL;
    void *pdata = NULL;
    memset(&pevent,0,sizeof(struct ifm_event));
    pmsghdr = &(mesg->msghdr);
    pdata = mesg->msg_data;

	switch(pmsghdr->msg_type)
	{
		case IPC_TYPE_IFM:
		{
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
	        {
	            pevent = *((struct ifm_event*)pdata);
	            ospf_ifm_manage(pevent, pmsghdr->msg_subtype);
	        }
			break;
		}
		case IPC_TYPE_OSPF:
		{
			if(pmsghdr->opcode == IPC_OPCODE_GET_BULK)
	        {
	        	ospf_snmp_manage(pdata, pmsghdr);
	        }
			break;
		}
		case IPC_TYPE_BFD:
		{
			if(pmsghdr->opcode == IPC_OPCODE_UPDATE)
	        {
	            ospf_bfd_manage(pdata, pmsghdr);//接收bfd创建会话后返回的参数
	        }
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				ospf_bfd_rcv_event(pdata, pmsghdr);//处理bfd上报的事件
			}
			break;
		}
		case IPC_TYPE_L3VPN:
		{
			ospf_vpn_manage(pdata,pmsghdr);
			break;
		}
		case IPC_TYPE_EFM:
		{
			u0_device_manage(pdata,pmsghdr);
			break;
		}
		case IPC_TYPE_OSPF_DCN:
		{
	        ospf_dcn_msg_handle(pdata, pmsghdr);
			break;
		}
		default:
		{
			OSPF_LOG_DEBUG("Recv invalid IPC TYPE %d!!!", pmsghdr->msg_type);
			break;
		}
	}
   
    return;
}

#if 0
/* redistribute information handle */
void ospf_route_manage(struct ipc_mesg mesg)
{
    int i;
    struct route_entry *p_route = NULL;
    struct ipc_msghdr *pmsghdr = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;
    /* process the ipc message */
    pmsghdr = &(mesg.msghdr);
	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "pmsghdr->data_num = %d ",pmsghdr->data_num);
	}
	
    if(pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        for(i = 0; i < pmsghdr->data_num; i++)
        {
            p_route = (struct route_entry *)&(mesg.msg_data) + i;
            ospf_route_read_ipv4 (pmsghdr->opcode, p_route,
                                  pmsghdr->data_len, vrf_id);
        }
    }
}
#endif
void ospf_route_manage_new(struct ipc_mesg_n *mesg)
{
    int i;
    struct route_entry *p_route = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
    vrf_id_t vrf_id = VRF_DEFAULT;
    /* process the ipc message */
    pmsghdr = &(mesg->msghdr);
	if (IS_DEBUG_OSPF (zebra, ZEBRA_REDISTRIBUTE))
	{
    	zlog_debug(OSPF_DBG_ZEBRA_REDISTRIBUTE, "pmsghdr->data_num = %d ",pmsghdr->data_num);
	}
	
    if(pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        for(i = 0; i < pmsghdr->data_num; i++)
        {
            p_route = (struct route_entry *)(mesg->msg_data) + i;
            ospf_route_read_ipv4 (pmsghdr->opcode, p_route,
                                  pmsghdr->data_len, vrf_id);
        }
    }
}

#if 0
static void ospf_ldp_manage(struct ipc_mesg mesg)
{
    struct ipc_msghdr *pmsghdr = NULL;
    //vrf_id_t vrf_id = VRF_DEFAULT;    
    void *pdata = NULL;
    struct interface *ifp = NULL;
    struct route_node *rn = NULL;
    pdata = mesg.msg_data;
    /* process the ipc message */
    pmsghdr = &(mesg.msghdr);
    ifindex_t ifindex = *((int*)pdata);

    ifp = if_lookup_by_index (ifindex);
    

        
    if (ifp == NULL)
    {
        zlog_warn("%s[%d]: Cannot find the ifp[%0x].\n",
                  __func__,__LINE__,ifindex);
        return ;
    }

    for (rn = route_top (IF_OIFS (ifp)); rn; rn = route_next (rn))
    {
        struct ospf_interface *oi = rn->info;
        if (!oi)
        {
            continue;
        }
        
        /* 从ip 禁止发送hello 包 */
        if (CHECK_FLAG(oi->connected->flags, ZEBRA_IFA_SECONDARY))
        {
            continue;
        }
        if (pmsghdr->opcode == IPC_OPCODE_ADD)
        {
            ospf_ldp_session_up(oi);
        }
        else if(pmsghdr->opcode == IPC_OPCODE_DELETE)
        {
            ospf_ldp_session_down(oi);
        }
    }
    
    
    
    
}
#endif

#if 0
int ospf_add_fifo_ifm(struct ipc_mesg *mesg)
{
	struct ifmeventfifo *newmesg = NULL;

	do
	{		
		newmesg = XCALLOC (MTYPE_IFMEVENT_FIFO, sizeof(struct ifmeventfifo));	
	
		if (newmesg == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newmesg == NULL);
	if (newmesg == NULL)
	{
		zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		return -1;
	}
	memset(newmesg, 0, sizeof(struct ifmeventfifo));
	memcpy(&newmesg->mesg, mesg, sizeof(struct ipc_mesg));
	FIFO_ADD(&om->ifmfifo, &(newmesg->fifo));
	return 0;
}

void ospf_read_mesg_from_ifmfifo(void)
{
	int item = 0;
	int maxcount = 100;
    struct ifmeventfifo *mesg = NULL;

	while(!FIFO_EMPTY(&om->ifmfifo) && (item++ < maxcount))
	{
		mesg = (struct ifmeventfifo *)FIFO_TOP(&om->ifmfifo);
		if(mesg == NULL)
			break;
		ospf_common_manage(mesg->mesg);			
		FIFO_DEL(mesg);
		XFREE (MTYPE_IFMEVENT_FIFO, mesg);
	}

}

/* recieve interface msg from ifm and route */
int ospf_ifm_msg_rcv(struct ipc_mesg mesg)
{
	int ret = 0;
	void *pdata = NULL;
	struct ipc_msghdr *pmsghdr = NULL;
	
	while(ret != -1)	
	{
		memset(&mesg,0,sizeof(struct ipc_mesg));
		ret = ipc_recv_common(&mesg, MODULE_ID_OSPF);
		if(ret != -1)
		{
			pmsghdr = &(mesg.msghdr);//in order to solve snmp IPC get bulk reply time too long
			pdata = mesg.msg_data;
			if((pmsghdr->msg_type == IPC_TYPE_OSPF) && (pmsghdr->opcode == IPC_OPCODE_GET_BULK))
			{
				ospf_snmp_manage(pdata, pmsghdr);
			}
			else
			{
				ospf_add_fifo_ifm(&mesg);
			}
		}
	}

	if(!FIFO_EMPTY(&om->ifmfifo))
	{
		ospf_read_mesg_from_ifmfifo();
	}

	return 0;
}
#endif

#if 0
void ospf_read_mesg_from_routefifo(void)
{
    struct ifmeventfifo *mesg;
	int item = 0;
	int maxcount = 200;

	while(!FIFO_EMPTY(&om->routefifo_rcv) && (item++ < maxcount))
	{
		mesg = (struct ifmeventfifo *)FIFO_TOP(&om->routefifo_rcv);
		if(mesg == NULL)
			break;
		ospf_route_manage(mesg->mesg);			
		FIFO_DEL(mesg);
		XFREE (MTYPE_ROUTEEVENT_FIFO, mesg);
	}

}


int ospf_add_fifo_route(struct ipc_mesg *mesg)
{
	struct ifmeventfifo *newmesg = NULL;

	do
	{		
		newmesg = XCALLOC (MTYPE_ROUTEEVENT_FIFO, sizeof(struct ifmeventfifo)); 
	
		if (newmesg == NULL)
		{
			zlog_err("%s():%d: XCALLOC failed!", __FUNCTION__, __LINE__);
			sleep(1);
		}
	}while(newmesg == NULL);
	if (newmesg == NULL)
	{
		zlog_err("%-15s[%d]: memory xcalloc error!",__FUNCTION__, __LINE__);
		return -1;
	}
	memset(newmesg, 0, sizeof(struct ifmeventfifo));
	memcpy(&newmesg->mesg, mesg, sizeof(struct ipc_mesg));
	FIFO_ADD(&om->routefifo_rcv, &(newmesg->fifo));
	return 0;
}



/* recieve route msg from route */
int ospf_route_msg_rcv(struct ipc_mesg mesg)
{
	int ret = 0;
	while(ret != -1)	
	{
		memset(&mesg,0,sizeof(struct ipc_mesg));
		ret = ipc_recv_route(&mesg, MODULE_ID_OSPF);
		if(ret != -1)
		{
			ospf_add_fifo_route(&mesg);
		}
	}

	if(!FIFO_EMPTY(&om->routefifo_rcv))
	{
		ospf_read_mesg_from_routefifo();
	}
	return ret ;
#if 0
    int ret = -1;
    /* once receive 300 packet*/
    for(int i = 300; i > 0; i--)
    {
        memset(&mesg,0,sizeof(struct ipc_mesg));
        ret = ipc_recv_route(&mesg, MODULE_ID_OSPF);
        if(ret < 0)
        {
            break;
        }
        ospf_route_manage(mesg);
    }
    return ret;
#endif
}

int ospf_msg_rcv(struct thread *thread)
{
    struct thread *t_read = NULL;
    struct ipc_mesg mesg;
    if (ospf_sta.debug == 1)
    {
        zlog_debug ("%s():%d: ospfd ospf_pkt_rcv() thread!", __FUNCTION__, __LINE__);
    }
    ospf_ifm_msg_rcv(mesg);
    ospf_route_msg_rcv(mesg);
    do
    {
        usleep(1000);/* CPU 10ms */
        //t_read = thread_add_event_normal (master_ospf, ospf_msg_rcv, NULL, 0);
		t_read = thread_add_event (master_ospf, ospf_msg_rcv, NULL, 0);
        if (t_read == NULL)
        {
            zlog_err ("%s():%d: ospfd thread_add_event() add ospf_msg_rcv() failed!", __FUNCTION__, __LINE__);
        }
    }
    while (t_read == NULL);
    return ERRNO_SUCCESS;
}
#endif




