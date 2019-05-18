#include <string.h>
#include <sys/queue.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/types.h>
#include <lib/log.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>

#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>

#include "dhcp6.h"
#include "dhcp6c.h"

#include "common.h"
#include "dhcpv6_packet.h"

#include "dhcpv6_msg.h"


extern struct thread_master *dhcpv6_master;
/* 接收 DHCPV6 报文 */
int dhcpv6_pkt_rcv(struct thread *argv)
{
	struct pkt_buffer *pkt = NULL;
	int token = 8;
    /* 每次收 8 个报文 */
    while(token)
	{
	    token--;
		pkt = pkt_rcv(MODULE_ID_DHCPV6);
		if(pkt == NULL) {
			goto out;
		}	
		
		if (check_dhcpv6_packet(pkt))
			continue;
		DHCPV6_LOG_DEBUG("recv packet from ifindex 0x%x", pkt->in_ifindex);
		if (pkt->cb.ipcb.dport == UDP_PORT_DHCPV6_CLIENT
		&& pkt->cb.ipcb.sport == UDP_PORT_DHCPV6_SERVER) {
			dhcpv6_client_fsm(pkt->in_ifindex, pkt, DHCPV6_EVENT_FROM_PKT, 0, 0);
		}
	}
	
out:	
	usleep(1000);
	thread_add_event (dhcpv6_master, dhcpv6_pkt_rcv, NULL, 0); /* thread to receive packet */
	return 0;
}

void dhcpv6_pkt_register(void)
{
	union proto_reg proto;
	struct in6_addr ipv6;

	memset(&proto, 0, sizeof(union proto_reg));

    /* dhcpv6 报文 */
    memset(&proto, 0, sizeof(union proto_reg));
    inet_pton (AF_INET6, DH6ADDR_ALLAGENT, &ipv6);
    IPV6_ADDR_COPY(&proto.ipreg.dipv6, &ipv6);
    proto.ipreg.type = PROTO_TYPE_IPV6;
    proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = UDP_PORT_DHCPV6_SERVER ;	
    proto.ipreg.dipv6_valid = 1;
    pkt_register(MODULE_ID_DHCPV6, PROTO_TYPE_IPV6, &proto);

	memset(&proto, 0, sizeof(union proto_reg));
    inet_pton (AF_INET6, DH6ADDR_ALLSERVER, &ipv6);
    IPV6_ADDR_COPY(&proto.ipreg.dipv6, &ipv6);
    proto.ipreg.type = PROTO_TYPE_IPV6;
    proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = UDP_PORT_DHCPV6_SERVER ;	
    proto.ipreg.dipv6_valid = 1;
    pkt_register(MODULE_ID_DHCPV6, PROTO_TYPE_IPV6, &proto);

	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;	
    proto.ipreg.type = PROTO_TYPE_IPV6;
	proto.ipreg.dport = UDP_PORT_DHCPV6_CLIENT ;	
	pkt_register(MODULE_ID_DHCPV6, PROTO_TYPE_IPV6, &proto);	
	
    memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;	
    proto.ipreg.type = PROTO_TYPE_IPV6;
	proto.ipreg.dport = UDP_PORT_DHCPV6_SERVER ;	
	pkt_register(MODULE_ID_DHCPV6, PROTO_TYPE_IPV6, &proto);

	LOG(LOG_DEBUG,"proto.ipreg.protocol:%d\n",proto.ipreg.protocol);
	dhcpv6_pkt_rcv(NULL);

}

/* 注册要接收的协议报文类型 */
void dhcpv6_event_register(void)
{
	
	ifm_event_register(IFNET_EVENT_IF_DELETE, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_DOWN, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF); /* 注册接口 down 事件*/	
	ifm_event_register(IFNET_EVENT_UP, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF); /* 注册接口 up 事件*/
	ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF);
	ifm_event_register(IFNET_EVENT_MODE_CHANGE, MODULE_ID_DHCPV6, IFNET_IFTYPE_L3IF);
	
    dhcpv6_ipc_rcv(NULL);
}

void dhcpv6_pkt_dump (void *data, int len)
{
    int i;
	u_char* pktdata;
	
	
	zlog_debug("ftm packet info begin: \n");
    for ( i = 0; i < len/16; i++ )
    {
		pktdata = (unsigned char *)data + i*16;
        zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7],
					pktdata[8], pktdata[9],pktdata[10],pktdata[11],pktdata[12],pktdata[13],pktdata[14],pktdata[15]);
    }
	
	if(len%16 >= 8)
	{
		pktdata = (unsigned char *)data + i*16;
        zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x",
					pktdata[0], pktdata[1],pktdata[2],pktdata[3],pktdata[4],pktdata[5],pktdata[6],pktdata[7]);
	}
	
	zlog_debug("ftm packet info end:\n");
    return ;	
}

int dhcpv6_ipc_rcv(struct thread *argv)
{
	static struct ipc_mesg mesg;	
	struct ipc_msghdr *pmsghdr = NULL;	
	void *pdata = NULL;
	struct ifm_event *pevent = NULL;
	uint32_t ifindex;
	int ret;
	//struct inet_prefix ipaddr;  /* 接口 IP 地址 */
	int token = 100;
	while (token > 0)
	{
		if(-1 == ipc_recv_common(&mesg, MODULE_ID_DHCPV6)) {
			goto out;
		}
		token--;
		/* process the ipc message */
		pmsghdr = &(mesg.msghdr);
		pdata = mesg.msg_data;
		DHCPV6_LOG_DEBUG("pmsghdr->msg_type %d, pmsghdr->opcode %d", pmsghdr->msg_type, pmsghdr->opcode);
		if(pmsghdr->msg_type == IPC_TYPE_IFM) 
		{
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				pevent = (struct ifm_event *)pdata;
				ifindex = pevent->ifindex;
				//ipaddr  = pevent->ipaddr;
				if(pmsghdr->msg_subtype == IFNET_EVENT_DOWN)//from ifm
				{
					zlog_debug ( "%s[%d] recv event notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, ifindex );
					//dhcp_if_down(ifindex);
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_UP)
				{
					zlog_debug ( "%s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex );
					//dhcp_if_up(ifindex);			
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_IF_DELETE)
				{
					zlog_debug ( "%s[%d] recv event notify ifindex 0x%0x delete\n", __FUNCTION__, __LINE__, ifindex );
					//dhcp_if_delete(ifindex);
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)//from route
				{
					//dhcp_if_ip_add(ifindex,ipaddr,(pevent->ipflag == IP_TYPE_SLAVE)?1:0);
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
				{
					//dhcp_if_ip_del(ifindex,(pevent->ipflag == IP_TYPE_SLAVE)?1:0);
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
				{			
					zlog_debug ( "%s[%d] recv event notify ifindex 0x%0x mode change %d\n", 
						__FUNCTION__, __LINE__, ifindex, pevent->mode);
					//dhcp_if_mode_change(ifindex,pevent->mode);
				}
				
			}		
		}
		else if(pmsghdr->msg_type == IPC_TYPE_L3IF)
		{
			ifindex = pmsghdr->msg_index;
		
			if(pmsghdr->opcode == IPC_OPCODE_EVENT)
			{
				//ret = 1;
				
				if (pmsghdr->msg_subtype == IFNET_EVENT_IP_ADD)
				{
					ret = dhcpv6_client_enable(ifindex);
					if(!ret)
					{
						dhcpv6_ip_address_set(ifindex , NULL,0,0,0); 			
					}
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_DELETE)
				{
					ret = dhcpv6_client_disable(ifindex);				
				}		
				else if (pmsghdr->msg_subtype == IFNET_EVENT_IP_PD_ADD)
				{
					ret = dhcpv6_client_pd_enable(ifindex);
					if(!ret)
					{
						dhcpv6_ip_address_set(ifindex , NULL,0,1,0); 			
					}
				}
				else if(pmsghdr->msg_subtype == IFNET_EVENT_IP_PD_DELETE)
				{
					ret = dhcpv6_client_pd_disable(ifindex);				
				}	
			}
		}
	}

out:	
    thread_add_event(dhcpv6_master, dhcpv6_ipc_rcv, NULL, 0);
	return 0;
}

