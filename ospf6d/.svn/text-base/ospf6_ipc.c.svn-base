
#include "stream.h"
#include "log.h"
#include "thread.h"
#include "memory.h"
#include "errcode.h"
#include "ifm_common.h"
#include "memtypes.h"
#include "module_id.h"
#include "msg_ipc.h"
#include "vrf.h"
#include "route_com.h"

#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <ftm/pkt_ip.h>
#include "ospf6d/ospf6_message.h"
#include "ospf6d/ospf6_proto.h"
#include "ospf6d/ospf6d.h"
#include "ospf6d/ospf6_zebra.h"
#include "ospf6d/ospf6_ipc.h"
#include "ospf6d/ospf6_debug.h"


extern struct thread_master *master_ospf6 ;

/* send ipc msg to route: multiple route item */
static int
ospf6_send_multiplemessage_route(struct route_entry *route, enum IPC_OPCODE opcode, unsigned int item)
{
    int ret = ipc_send_msg_n2(route, sizeof(struct route_entry) * item, item, MODULE_ID_ROUTE, MODULE_ID_OSPF6,
                             IPC_TYPE_ROUTE, 0, opcode, 0);

    if (ret < 0)
    {
        zlog_err("%-15s[%d]:ERROR:route send ", __FUNCTION__, __LINE__);
        return -1;
    }

    return ret;
}

/* handle route info send*/
static int
ospf6_asyc_send_route_cmd(void *threadval)
{
    int i;
    int ret = 0;
    int item = 0;   /*  一条IPC msg 承载路由条目个数*/
    int send_or_count = 0;
    unsigned int opcodeflag = 0;
    int msg_num = IPC_MSG_LEN / sizeof(struct route_entry);
    struct route_entry route[IPC_MSG_LEN/sizeof(struct route_entry)];
    struct routefifo *routefifo_entry[IPC_MSG_LEN/sizeof(struct route_entry)];
    struct fifo *pfifo = NULL;

    for (item = 0; item < msg_num; item++)
    {
        routefifo_entry[item] = NULL;
    }

    om6->t_thread_routefifo = 0;

    while (!FIFO_EMPTY(&om6->routefifo))
    {
        if (send_or_count >= 2000)
        {
            break;
        }

        pfifo = &om6->routefifo;
        routefifo_entry[0] = (struct routefifo *)FIFO_TOP(pfifo);
        opcodeflag = routefifo_entry[0]->opcode;
        pfifo = &(routefifo_entry[0]->fifo);
        memcpy(&route[0], &routefifo_entry[0]->route, sizeof(struct route_entry));
        item = 1;

        while (!FIFO_EMPTY(pfifo) && item < msg_num)
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

        ret = ospf6_send_multiplemessage_route(route, opcodeflag, item);

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
            XFREE(MTYPE_ROUTE_FIFO, routefifo_entry[i]);
			routefifo_entry[i] = NULL;
        }
    }

    /*om6->t_thread_routefifo =
        thread_add_timer_msec(master_ospf6, ospf6_asyc_send_route_cmd, NULL, 1000);*/

	om6->t_thread_routefifo = 
		high_pre_timer_add ((char *)"ospf6_asyc_route_timer", LIB_TIMER_TYPE_NOLOOP, ospf6_asyc_send_route_cmd, NULL, 1000);
    return 0;
}

int
ospf6_redistribute_send(int command, struct ospf6 *ospf6, int source)
{
    if (command == ZEBRA_REDISTRIBUTE_ADD)
    {
        if (om6->redis_count[source] == 1)
        {
            routev6_event_register(source, MODULE_ID_OSPF6);
        }
    }
    else if (command == ZEBRA_REDISTRIBUTE_DELETE)
    {
        if (om6->redis_count[source] == 0)
        {
            routev6_event_unregister(source, MODULE_ID_OSPF6);
        }
    }

    return 0;
}



/* ifm register */
static void
ospf6_ifm_register()
{
    ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_UP, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
    ifm_event_register(IFNET_EVENT_MTU_CHANGE, MODULE_ID_OSPF6, IFNET_IFTYPE_L3IF);
}


/* register recevie packet */
static void
ospf6_pkt_register(void)
{
    union proto_reg proto;
    struct in6_addr ipv6;

    memset(&proto, 0, sizeof(union proto_reg));
    inet_pton(AF_INET6, ALLSPFROUTERS6, &ipv6);
    IPV6_ADDR_COPY(&proto.ipreg.dipv6, &ipv6);
    proto.ipreg.protocol = IP_P_OSPF;
    proto.ipreg.type = PROTO_TYPE_IPV6;
    proto.ipreg.dipv6_valid = 1;
    pkt_register(MODULE_ID_OSPF6, PROTO_TYPE_IPV6, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    inet_pton(AF_INET6, ALLDROUTERS6, &ipv6);
    IPV6_ADDR_COPY(&proto.ipreg.dipv6, &ipv6);
    proto.ipreg.type = PROTO_TYPE_IPV6;
    proto.ipreg.dipv6_valid = 1;
    proto.ipreg.protocol = IP_P_OSPF;
    pkt_register(MODULE_ID_OSPF6, PROTO_TYPE_IPV6, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_OSPF;
    proto.ipreg.type = PROTO_TYPE_IPV6;
    //proto.ipreg.dipv6_valid = 1;
    pkt_register(MODULE_ID_OSPF6, PROTO_TYPE_IPV6, &proto);
}

static void
ospf6_ifm_manage(struct ifm_event pevent, int event)
{
    vrf_id_t vrf_id = VRF_DEFAULT;

    switch (event)
    {
        case IFNET_EVENT_IP_ADD:
            if (pevent.ipaddr.type == INET_FAMILY_IPV4)
            {
            	if (IS_OSPF6_DEBUG_ZEBRA (RECV))
                	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event IP_ADD ifindex 0x%0x\n", __FUNCTION__, __LINE__, pevent.ifindex);
            }
            else if (pevent.ipaddr.type == INET_FAMILY_IPV6)
            {
            	if (IS_OSPF6_DEBUG_ZEBRA (RECV))
  					zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event IPV6_ADD ifindex 0x%0x\n", __FUNCTION__, __LINE__, pevent.ifindex);
                ospf6_zebra_if_address_update_add(pevent, vrf_id);
            }

            break;
        case IFNET_EVENT_IP_DELETE:
            if (pevent.ipaddr.type == INET_FAMILY_IPV4)
            {
            	if (IS_OSPF6_DEBUG_ZEBRA (RECV))
                	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event IP_DEL ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
            }	
            else if (pevent.ipaddr.type == INET_FAMILY_IPV6)
            {
            	if (IS_OSPF6_DEBUG_ZEBRA (RECV))
					zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event IPV6_DEL ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
				
                ospf6_zebra_if_address_update_delete(pevent, vrf_id);
            }

            break;
        case IFNET_EVENT_UP:
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
            	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event UP ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);
			
            pevent.up_flag = IFNET_LINKUP;
            ospf6_zebra_if_state_update(pevent, vrf_id);
            break;
        case IFNET_EVENT_DOWN:
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
            	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event DOWN ifindex 0x%0x \n", __FUNCTION__, __LINE__, pevent.ifindex);

            pevent.up_flag = IFNET_LINKDOWN;
            ospf6_zebra_if_state_update(pevent, vrf_id);

            break;
        case IFNET_EVENT_IF_DELETE:
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
            	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event IF_DEL ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, pevent.ifindex);
			
            ospf6_zebra_if_del(pevent, vrf_id);

            break;
        case IFNET_EVENT_MODE_CHANGE:
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
            	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event MODE ifindex 0x%0x change\n", __FUNCTION__, __LINE__, pevent.ifindex);
            ospf6_interface_mode_change(pevent, vrf_id);
            break;
        case IFNET_EVENT_MTU_CHANGE:
			if (IS_OSPF6_DEBUG_ZEBRA (RECV))
            	zlog_debug(OSPF6_DBG_ZEBRA_MSG, "%s[%d] recv event MTU ifindex 0x%0x change\n", __FUNCTION__, __LINE__, pevent.ifindex);
            ospf6_interface_mtu_change (pevent, vrf_id);
            break;
        default:
            break;
    }

    return;
}

#if 0
/*interface information handle*/
static void
ospf6_common_manage(struct ipc_mesg mesg)
{
    struct ifm_event pevent;
    void *pdata = NULL;
    struct ipc_msghdr *pmsghdr = NULL;
	
    memset(&pevent, 0, sizeof(struct ifm_event));
    pmsghdr = &(mesg.msghdr);
    pdata = mesg.msg_data;

    if (pmsghdr->msg_type == IPC_TYPE_IFM)
    {
        if (pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
            pevent = *((struct ifm_event*)pdata);
            ospf6_ifm_manage(pevent, pmsghdr->msg_subtype);
        }
    }
    else if (pmsghdr->msg_type == IPC_TYPE_OSPF)/*SNMP GET */
    {
        if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
        {
            //ospf6_snmp_manage(pdata, pmsghdr);
        }
    }

    return;
}
#endif

/*interface information handle*/
static void
ospf6_common_manage_new(struct ipc_mesg_n *mesg)
{
    struct ifm_event pevent;
    void *pdata = NULL;
    struct ipc_msghdr_n *pmsghdr = NULL;
	
    memset(&pevent, 0, sizeof(struct ifm_event));
    pmsghdr = &(mesg->msghdr);
    pdata = mesg->msg_data;

    if (pmsghdr->msg_type == IPC_TYPE_IFM)
    {
        if (pmsghdr->opcode == IPC_OPCODE_EVENT)
        {
            pevent = *((struct ifm_event*)pdata);
            ospf6_ifm_manage(pevent, pmsghdr->msg_subtype);
        }
    }
    else if (pmsghdr->msg_type == IPC_TYPE_OSPF)/*SNMP GET */
    {
        if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
        {
            //ospf6_snmp_manage(pdata, pmsghdr);
        }
    }

    return;
}

#if 0
static int
ospf6_add_fifo_ifm(struct ipc_mesg *mesg)
{
    struct ifmeventfifo *newmesg = NULL;

    newmesg = XCALLOC(MTYPE_IFMEVENT_FIFO, sizeof(struct ifmeventfifo));

    if (newmesg == NULL)
    {
        zlog_err("Can't add fifo to ifm %-15s[%d]: memory xcalloc error!", __FUNCTION__, __LINE__);
        return -1;
    }

    memset(newmesg, 0, sizeof(struct ifmeventfifo));
    memcpy(&newmesg->mesg, mesg, sizeof(struct ipc_mesg));
    FIFO_ADD(&om6->ifmfifo, &(newmesg->fifo));
    return 0;
}


static void
ospf6_read_mesg_form_ifmfifo()
{    
	int item = 0;
    int maxcount = 100;
    struct ifmeventfifo *mesg = NULL;

    while (!FIFO_EMPTY(&om6->ifmfifo) && (item++ < maxcount))
    {
        mesg = (struct ifmeventfifo *)FIFO_TOP(&om6->ifmfifo);
        ospf6_common_manage(mesg->mesg);
        FIFO_DEL(mesg);
        XFREE(MTYPE_IFMEVENT_FIFO, mesg);
    }

}

/* recieve interface msg from ifm and route */
static int
ospf6_ifm_msg_rcv(void)
{
    int ret = 0;
    struct ipc_mesg mesg;

    while (ret != -1)
    {
        memset(&mesg, 0, sizeof(struct ipc_mesg));
        ret = ipc_recv_common(&mesg, MODULE_ID_OSPF6);

        if (ret != -1)
        {
            ospf6_add_fifo_ifm(&mesg);
        }
    }

    if (!FIFO_EMPTY(&om6->ifmfifo))
    {
        ospf6_read_mesg_form_ifmfifo();
    }

    return 0;
}

/* redistribute information handle */
static void
ospf6_route_manage(struct ipc_mesg mesg)
{
    int i;
    vrf_id_t vrf_id = VRF_DEFAULT;
    struct ipc_msghdr *pmsghdr = NULL;
    struct route_entry *p_route = NULL;
	
    /* process the ipc message */
    pmsghdr = &(mesg.msghdr);
    zlog_debug("pmsghdr->data_num = %d ", pmsghdr->data_num);

    if (pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        for (i = 0; i < pmsghdr->data_num; i++)
        {
            p_route = (struct route_entry *) & (mesg.msg_data) + i;
            ospf6_zebra_read_ipv6(pmsghdr->opcode, p_route,
                                  pmsghdr->data_len, vrf_id);
        }
    }

}
#endif

static void
ospf6_route_manage_new(struct ipc_mesg_n *mesg)
{
    int i;
    vrf_id_t vrf_id = VRF_DEFAULT;
    struct ipc_msghdr_n *pmsghdr = NULL;
    struct route_entry *p_route = NULL;
	
    /* process the ipc message */
    pmsghdr = &(mesg->msghdr);
    zlog_debug(OSPF6_DBG_ROUTE, "pmsghdr->data_num = %d ", pmsghdr->data_num);

    if (pmsghdr->msg_type == IPC_TYPE_ROUTE)
    {
        for (i = 0; i < pmsghdr->data_num; i++)
        {
            p_route = (struct route_entry *)(mesg->msg_data) + i;
            ospf6_zebra_read_ipv6(pmsghdr->opcode, p_route,
                                  pmsghdr->data_len, vrf_id);
        }
    }

}

#if 0
/* recieve route msg from route */
static int
ospf6_route_msg_rcv(void)
{
    int ret = -1;
	struct ipc_mesg mesg;

    /* once receive 300 packet*/
    for (int i = 300; i > 0; i--)
    {
        memset(&mesg, 0, sizeof(struct ipc_mesg));
        ret = ipc_recv_route(&mesg, MODULE_ID_OSPF6);

        if (ret < 0)
        {
            break;
        }

        ospf6_route_manage(mesg);
    }

    return ret;
}

static int
ospf6_msg_rcv(struct thread *thread)
{	
	om6->t_read_msg = NULL;	
	
    ospf6_ifm_msg_rcv();
    ospf6_route_msg_rcv();

    do
    {
        usleep(10000);/* CPU 10ms */
        om6->t_read_msg = thread_add_event(master_ospf6, ospf6_msg_rcv, NULL, 0);

        if (om6->t_read_msg == NULL)
        {
            zlog_err("%s():%d: ospfd thread_add_event() add ospf_msg_rcv() failed!", __FUNCTION__, __LINE__);
        }
    }while (om6->t_read_msg == NULL);

    return ERRNO_SUCCESS;
}
#endif

static
int ospf6_pkt_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
	int msg_block_len = 0;
	
	if ( NULL == pmsg || 0 == imlen )
	{
		zlog_debug(OSPF6_DBG_OTHER, "%s[%d]: In function '%s': OSPF recv msg: pmsg is NULL.\r\n",\
									 __FILE__, __LINE__, __func__);
		return ERRNO_FAIL;
	}
	
	zlog_debug(OSPF6_DBG_OTHER, "%s[%d]: Ospf recv msg: pmsg=%p, imlen=%d msg_type:%d msg_subtype:%d\r\n", \
					__FUNCTION__, __LINE__, pmsg, imlen, pmsg->msghdr.msg_type, pmsg->msghdr.msg_subtype);

	msg_block_len = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N;
	if(msg_block_len <= imlen)
	{
		switch ( pmsg->msghdr.msg_type )
		{
			case IPC_TYPE_PACKET:
				ospf6_pkt_rcv_new(pmsg);
				break;

			case IPC_TYPE_ROUTE:
				ospf6_route_manage_new(pmsg);
				break;
			
			default:
				ospf6_common_manage_new(pmsg);
				break;
		}
	}
	else
	{
		zlog_debug(OSPF6_DBG_OTHER, "OSPF recv msg: datalen error, data_len=%d, msgrcv len = %d\n", msg_block_len, imlen);
	}

	if(pmsg)
	{
		mem_share_free(pmsg, MODULE_ID_OSPF6);
	}
	
	return ERRNO_SUCCESS;
}

/* redistribute route type. */
void
ospf6_ipc_init(void)
{
    //thread_add_event(master_ospf6, ospf6_msg_rcv, NULL, 0);
    //thread_add_event(master_ospf6, ospf6_pkt_rcv, NULL, 0);  /* thread to receive packet */
	if(ipc_recv_thread_start((char *)"Ospf6MsgRev", MODULE_ID_OSPF6, SCHED_OTHER, -1, ospf6_pkt_msg_rcv, 0) == -1)
    {
        printf("Ospf6 msg receive thread start fail\r\n");
        exit(0);
    }

	ospf6_pkt_register();
    ospf6_ifm_register();

    /*om6->t_thread_routefifo =
        thread_add_timer_msec(master_ospf6, ospf6_asyc_send_route_cmd, NULL, 1000);*/
	om6->t_thread_routefifo = 
		high_pre_timer_add ((char *)"ospf6_asyc_route_timer", LIB_TIMER_TYPE_NOLOOP, ospf6_asyc_send_route_cmd, NULL, 1000);

}



