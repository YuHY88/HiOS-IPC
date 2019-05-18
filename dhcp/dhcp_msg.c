#include <stdio.h>
#include <pthread.h>

#include <lib/vty.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include <lib/errcode.h>

#include "packet.h"
#include "dhcp_if.h"
#include "dhcp_server.h"
#include "dhcp_relay.h"
#include "dhcp_msg.h"
#include "dhcp_client.h"
#include "dhcp_server.h"
#include "pool.h"
#include "pool_static_address.h"

extern struct thread_master *dhcp_master;
extern pthread_mutex_t dhcp_lock;

int dhcp_rcv_ifevent(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
    struct ifm_event *pevent = NULL;
    uint32_t ifindex = 0;
	
	struct inet_prefix ipaddr;  /* 接口 IP 地址 */
	/* process the ipc message */
	if(pmsghdr->opcode == IPC_OPCODE_EVENT)
	{
		pevent = (struct ifm_event *)pdata;
		ifindex = pevent->ifindex;
		ipaddr  = pevent->ipaddr;
		if(pmsghdr->msg_subtype == IFNET_EVENT_DOWN)//from ifm
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "recv event notify ifindex 0x%0x down\n", ifindex );
			dhcp_if_down(ifindex);
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_UP)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "recv event notify ifindex 0x%0x up\n", ifindex );
			dhcp_if_up(ifindex);			
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_IF_DELETE)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "recv event notify ifindex 0x%0x delete\n", ifindex );
			pthread_mutex_lock(&dhcp_lock);
			dhcp_if_delete(ifindex);
			pthread_mutex_unlock(&dhcp_lock);
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)//from route
		{
			dhcp_if_ip_add(ifindex,ipaddr,(pevent->ipflag == IP_TYPE_SLAVE)?1:0);
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
		{	
			dhcp_if_ip_del(ifindex,(pevent->ipflag == IP_TYPE_SLAVE)?1:0);
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
		{			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "recv event notify ifindex 0x%0x mode change %d\n", 
				ifindex, pevent->mode);
			dhcp_if_mode_change(ifindex,pevent->mode);
		}
	}/* end if pmsghdr->opcode == IPC_OPCODE_EVENT */		

	return 0;
}

int dhcp_rcv_l3if(struct ipc_mesg_n *pmesg, int imlen)
{
	struct ipc_msghdr_n *pmsghdr = NULL;
	int ret;
	void *pdata = NULL;
	uint32_t ifindex;
	
	pmsghdr = &(pmesg->msghdr);
	ifindex = pmsghdr->msg_index;
	pdata = pmesg->msg_data;
	
	if(pmsghdr->opcode == IPC_OPCODE_EVENT)
	{
		//ret = 1;
		if (pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)
		{
			ret = dhcp_client_enable(ifindex);
			if(!ret)
			{
				dhcp_ip_address_set(ifindex, 0, 0, FALSE, FALSE); 			
			}
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
		{
			ret = dhcp_client_disable(ifindex);				
		}

		/*if(ret)
		{			
			ipc_send_noack(1, pmsghdr->sender_id, MODULE_ID_DHCP, IPC_TYPE_L3IF, pmsghdr->msg_subtype, ifindex);				
		}
		else
		{			
			ipc_send_ack(pmsghdr->sender_id, MODULE_ID_DHCP, IPC_TYPE_L3IF, pmsghdr->msg_subtype, ifindex);					
		}*/
		
	}/* end if pmsghdr->opcode == IPC_OPCODE_EVENT */
	
	else if (pmsghdr->opcode == IPC_OPCODE_GET_BULK)
	{
		if (pmsghdr->msg_subtype == DHCP_SNMP_IPPOOL_GET)
		{
			dhcp_com_ippool_get_bulk(pdata, pmsghdr);
		}
		else if(pmsghdr->msg_subtype == DHCP_SNMP_IPBIND_GET)
		{
			dhcp_com_ipbind_get_bulk(pdata, pmsghdr);
		}
		else if(pmsghdr->msg_subtype == DHCP_SNMP_IFCONFIG_GET)
		{
			dhcp_com_ifconfig_get_bulk(pdata, pmsghdr);
		}
		else if(pmsghdr->msg_subtype == DHCP_SNMP_DYNAMIC_IPBIND_GET)
		{
			dhcp_com_dynamic_ipbind_get_bulk(pdata, pmsghdr);
		}

	}/* end if pmsghdr->opcode == IPC_OPCODE_GET_BULK */
	else if (pmsghdr->opcode == IPC_OPCODE_UPDATE)
	{
		if (pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)
		{
			ret = dhcp_client_save_enable(ifindex);
		}
		else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
		{
			ret = dhcp_client_save_disable(ifindex);				
		}
	}
	return 0;
}

/* 注册要接收的协议报文类型 */
void dhcp_event_register(void)
{
	
	ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF); /* 注册接口 down 事件*/	
	ifm_event_register(IFNET_EVENT_UP, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF); /* 注册接口 up 事件*/
	ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_DHCP, IFNET_IFTYPE_L3IF);
	
//    dhcp_ipc_rcv(NULL);
	
}

/* 注册要接收的协议报文类型 */
void dhcp_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

    /* dhcp 广播报文 */
	proto.ipreg.protocol = IP_P_UDP;	
	proto.ipreg.dport = 67 ;	
	proto.ipreg.dip = 0xffffffff ;	
	pkt_register(MODULE_ID_DHCP, PROTO_TYPE_IPV4, &proto);	

	proto.ipreg.protocol = IP_P_UDP;	
	proto.ipreg.dport = 68 ;	
	proto.ipreg.dip = 0xffffffff ;		
	pkt_register(MODULE_ID_DHCP, PROTO_TYPE_IPV4, &proto);	

    /* dhcp 单播报文 */
	proto.ipreg.dip = 0;
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = 67 ;	
	pkt_register(MODULE_ID_DHCP, PROTO_TYPE_IPV4, &proto);	

	proto.ipreg.protocol = IP_P_UDP;	
	proto.ipreg.dport = 68 ;	
	pkt_register(MODULE_ID_DHCP, PROTO_TYPE_IPV4, &proto);	

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "proto.ipreg.protocol:%d\n",proto.ipreg.protocol);
	//dhcp_pkt_rcv(NULL);
	/*
	if(ipc_recv_thread_start("DhcpIpcMsgRev", MODULE_ID_DHCP, SCHED_OTHER, -1, &dhcp_pkt_rcv, 0) == -1)
	{
		printf(" dhcp ipc msg receive thread start fail\r\n");
		exit(0);
	}	
	*/
}

void dhcp_pkt_dump(void *data, int len)
{
    int i;
	u_char* pktdata;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ftm packet info begin: \n");
    for(i = 0; i < len/16; i++)
    {
		pktdata = (unsigned char *)data + i*16;
        DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
					pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],pktdata[15]);
    }
	
	if(len%16 >= 8)
	{
		pktdata = (unsigned char *)data + i*16;
        DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "%02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7]);
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ftm packet info end:\n");
    return ;	
}

/* 接收 DHCP报文 */
int dhcp_pkt_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    struct pkt_buffer *pkt = NULL;
	struct if_dhcp *pif=NULL;
	struct dhcpMessage *dm = NULL;
	
	pkt = (struct pkt_buffer *)pmsg->msg_data;
	if(pkt == NULL)
		return ERRNO_FAIL;
	APP_RECV_PKT_FROM_FTM_DATA_SET(pkt);
	
	pif = dhcp_if_lookup_by_ip(ntohl(pkt->cb.ipcb.dip), 1);
	if(!pif)
	{
		pif = dhcp_if_lookup(pkt->in_ifindex);
		/* 零配置单播续租，收包ifindex为子接口,通过收vlan查找dhcp链表 */
		if(!pif)
		{
			pif = dhcp_if_lookup(IFM_PARENT_IFINDEX_GET(pkt->in_ifindex));	
		}
	}
	dm = pkt->data;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dhcp_pkt_rcv ,client mac[]: %02X-%02X-%02X-%02X-%02X-%02X \n",
		dm->chaddr[0],dm->chaddr[1],dm->chaddr[2],
		dm->chaddr[3],dm->chaddr[4],dm->chaddr[5]);

	if(!check_packet(dm))
	{
		return ERRNO_FAIL;
	}
	if(pif && pif->dhcp_role != E_DHCP_NOTHING)
	{	
		if(pif->dhcp_role == E_DHCP_CLIENT)
		{	
			dhcp_client_fsm(pif, pkt, DHCP_EVENT_FROM_PKT);
		}
		else if(pif->dhcp_role == E_DHCP_ZERO)
		{	
			dhcp_client_fsm(pif, pkt, DHCP_EVENT_FROM_PKT);
		}
		else if(pif->dhcp_role == E_DHCP_SERVER)
		{
			dhcp_server_fsm(pif,pkt);	
		}
		else if(pif->dhcp_role ==E_DHCP_RELAY)
		{		
			dhcp_relay_fsm(pif,pkt);		
		}
	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dhcp pkt received but droped... ");
	}

	return 0;
}

int dhcp_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{

	int revln = 0;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ipmc recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

	if ( NULL == pmsg || 0 == imlen )
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ipmc recv msg: pmsg is NULL.\r\n");
		return ERRNO_FAIL;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "ipmc recv msg: msg_type %d\r\n", pmsg->msghdr.msg_type);
	revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

	if ( revln <= imlen)
	{
		switch ( pmsg->msghdr.msg_type )
		{
			case IPC_TYPE_PACKET:
				dhcp_pkt_rcv(pmsg, 0);
				break;

			case IPC_TYPE_IFM:
				dhcp_rcv_ifevent(&(pmsg->msghdr), (void*)pmsg->msg_data);
				break;

			case IPC_TYPE_L3IF:
				dhcp_rcv_l3if(pmsg, 0);
				break;

			default:
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dhcp_msg_rcv, receive unk message\r\n");
				break;
		}

	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dhcp recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
	}
	mem_share_free(pmsg, MODULE_ID_DHCP);
	return 0;
}
