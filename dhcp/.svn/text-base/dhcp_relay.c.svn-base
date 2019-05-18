#include <string.h>
#include <sys/time.h>

#include <lib/types.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/devm_com.h>
#include <lib/ifm_common.h>
#include "packet.h"
#include "dhcp_server.h"
#include "options.h"
#include "dhcp_relay.h"

struct if_dhcp *pif_relay= NULL;
extern struct thread_master *dhcp_master;

/* lifetime of an xid entry in sec. */
#define MAX_LIFETIME   5  //old 2*60


static struct xid_item *xid_add(struct xid_item **xid_list,uint32_t xid, struct sockaddr_in *ip)
{
	struct xid_item *cur;
	struct xid_item *new_xid_item;

	/* Build new node */
	new_xid_item = xmalloc(sizeof(struct xid_item));
	new_xid_item->xid = xid;
	new_xid_item->ip = *ip;
	new_xid_item->timestamp = time(0);	
	new_xid_item->next = NULL;

	//LOG(LOG_DEBUG,"xid add,xid:%08X,pos:%08X",xid,new_xid_item);	//fix warning	

	
	/*printf("aaaaaaaaaaaaa, add static ip  %08X\n",*ip);		
	printf("aaaaaaaaaaaaa, add static mac %02X:%02X:%02X:%02X:%02X:%02X\n",
		mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);*/

	/* If it's the first node to be added... */
	if(*xid_list == NULL)
	{
		*xid_list = new_xid_item;
	}
	else
	{
		cur = *xid_list;
		while(cur->next != NULL)
		{
			cur = cur->next;
		}

		cur->next = new_xid_item;
	}
	return NULL;
}

static void xid_expire(struct xid_item **xid_list)
{
	unsigned current_time = time(0);
	struct xid_item *cur, *next;
	struct xid_item *last = NULL;
	
	cur = *xid_list;

	if(cur == NULL)
	{
		return;	
	}
	//
	do
	{
	
		next = cur->next;		
		
		if((current_time - cur->timestamp) > MAX_LIFETIME)
		{
			if(cur == *xid_list)
			{
				*xid_list = next;				
			}
			else
			{
				if(last)
				{
					last->next = next;
				}
			}
			
			//LOG(LOG_DEBUG,"xid expire,xid:%08X,cur:%08X",cur->xid,cur);		//fix warning
			free(cur);
		}
		else
		{			
			last = cur ;		
		}
		
		cur = next;

	}while(cur!= NULL);
	

	return ;
}

int relay_xid_del_timeout(void *msg)
{
	struct listnode *node;	
	struct if_dhcp *pif= NULL;
	
	//DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	for(node = dhcp_if_list.head ; node; node = node->next)
	{
		pif=listgetdata(node);
		if(pif && pif->dhcp_role == E_DHCP_RELAY && pif->r_ctrl != NULL)
		{
			xid_expire(&pif->r_ctrl->xid_list);			
		}
	}	
	//thread_add_timer(dhcp_master, relay_xid_del_timeout, NULL, 2);
	
	//DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "leave the function of '%s',--the line of %d",__func__,__LINE__);

	return 0;

}

struct xid_item *xid_find(struct xid_item *xid_list,uint32_t xid)
{
	struct xid_item *cur = xid_list;


	while(cur != NULL)
	{
		if(cur->xid == xid)
		{
			break;
		}

		cur = cur->next;
	}

	return cur;
}

static void xid_del(struct xid_item **xid_list, uint32_t xid)
{
	struct xid_item *cur,*next;
	struct xid_item *last = NULL;
	cur = *xid_list;

	if(cur == NULL)
	{
		return;	
	}
	
	do
	{
		next = cur->next;		
		
		if(cur->xid == xid)
		{
			if(cur == *xid_list)
			{
				*xid_list = next;				
			}
			else
			{
				if(last) 
				{
					last->next = next;
				}
			}
			
			//LOG(LOG_DEBUG,"xid del,xid:%08X,cur:%08X",cur->xid,cur);//fix warning		
			free(cur);
		}
		else
		{			
			last = cur ;		
		}
		
		cur  = next;

	}while(cur!= NULL);
	
	return ;
}

static void xid_del_all(struct xid_item **xid_list)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

	struct xid_item *cur,*next;
	
	cur = *xid_list;

	*xid_list=NULL;

	while(cur!=NULL)
	{	
		next = cur->next;
		
		free(cur);

		cur = next ;
	}
	return ;
}

/**
 * get_dhcp_packet_type - gets the message type of a dhcp packet
 * p - pointer to the dhcp packet
 * returns the message type on success, -1 otherwise
 */
static int get_dhcp_packet_type(struct dhcpMessage *p)
{
	uint8_t *op;

	/* it must be either a BOOTREQUEST or a BOOTREPLY */
	if (p->op != BOOTREQUEST && p->op != BOOTREPLY)
	{
		return -1;
	}
	/* get message type option */
	op = get_option(p, DHCP_MESSAGE_TYPE);
	if (op != NULL)
	{
		return op[0];
	}
	return -1;
}

/*static int sendto_ip4(int sock, const void *msg, int msg_len, struct sockaddr_in *to)
{
	int err;

	errno = 0;
	err = sendto(sock, msg, msg_len, 0, (struct sockaddr*) to, sizeof(*to));
	err -= msg_len;
	if (err)
		bb_perror_msg("sendto");
	return err;
}*/

/**
 * pass_to_server() - forwards dhcp packets from client to server
 * p - packet to send
 * client - number of the client
 */
static void pass_to_server(struct dhcpMessage *p, struct sockaddr_in *client_addr, uint32_t sip, uint32_t nexthop)
{
	int type;
    int    ret = -1;
	uchar  pmac[6];
	struct ifm_info pifm_info = {0};
	uint16_t vlanid = 0;
	uint32_t ifindex = 0;	
	                              /*option 82 0x52*//*len*//*sub-option1*//* 2 btybe vlan */  /* 4 btybe ifindex */   /*sub-option2*//* 6 btybe relay mac */
	struct option82_agent dhcp_agent = {DHCP_AGENT, 16, {0x01, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
	/* check packet_type */
	type = get_dhcp_packet_type(p);
	if (type != DHCPDISCOVER && type != DHCPREQUEST
	 && type != DHCPDECLINE && type != DHCPRELEASE
	 && type != DHCPINFORM)
	{
		return;
	}

	/* create new xid entry */
	//xid_add(p->xid, client_addr, client);
	xid_add(&pif_relay->r_ctrl->xid_list,p->xid, client_addr);

	/* forward request to server */
	/* note that we send from fds[0] which is bound to SERVER_PORT (67).
	 * IOW: we send _from_ SERVER_PORT! Although this may look strange,
	 * RFC 1542 not only allows, but prescribes this for BOOTP relays.
	 */
	//sendto_ip4(fds[0], p, packet_len, server_addr);
	//sendto_ip4( p, packet_len, server_addr);
	
	/* add vlan id to option 82 */
	ret = ifm_get_all_info(pif_relay->ifindex, MODULE_ID_DHCP, &pifm_info);
    
	if((ret == 0) && pifm_info.encap.svlan.vlan_start)
	{
		vlanid = htons(pifm_info.encap.svlan.vlan_start);
		memcpy(&dhcp_agent.str[2], &vlanid, 2);
	}
	/* add ifindex to option 82 */
	ifindex = htonl(pif_relay->ifindex);
	memcpy(&dhcp_agent.str[4], &ifindex, 4);
	/* add relay mac to option 82 */

	if(ifm_get_mac(pif_relay->ifindex, MODULE_ID_DHCP,pmac) == 0)
	{
		memcpy(&dhcp_agent.str[10], pmac, 6);
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Info: get interface mac :[%02X:%02X:%02X:%02X:%02X:%02X] \n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	}
	
	add_option_string(p->options, (uint8_t *)&dhcp_agent);
	kernel_packet(p, sip, SERVER_PORT, nexthop, SERVER_PORT);
}

/**
 * pass_to_client() - forwards dhcp packets from server to client
 * p - packet to send
 */
static void pass_to_client(struct dhcpMessage *pkt, uint32_t sip, uint32_t ifindex)
{
	int type;
	struct xid_item *item;
	struct option124_vlan_ifvalid option124_vlan;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

	/* check xid */
	item = xid_find(pif_relay->r_ctrl->xid_list, pkt->xid);
	if(!item) 
	{
		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "return because of item not found");		
		return;
	}

	/* check packet type */
	type = get_dhcp_packet_type(pkt);
	if(type != DHCPOFFER && type != DHCPACK && type != DHCPNAK) 
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "return because of type wrong ,type:%d",type);
		return;
	}

//TODO: also do it if (p->flags & htons(BROADCAST_FLAG)) is set!
	//if (item->ip.sin_addr.s_addr == htonl(INADDR_ANY))
		//item->ip.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	if(item->ip.sin_addr.s_addr && item->ip.sin_addr.s_addr!= pkt->yiaddr)
	{
		kernel_packet(pkt, sip, SERVER_PORT, item->ip.sin_addr.s_addr, item->ip.sin_port);
	}
	else
	{
		option124_vlan.vlanid  = 0;
		option124_vlan.ifvalid = 0;
		raw_packet(pkt, sip, SERVER_PORT, INADDR_BROADCAST,
				item->ip.sin_port, MAC_BCAST_ADDR, ifindex, option124_vlan, 0);
	}

	//if (sendto_ip4(p, packet_len, &item->ip) != 0) {
		//return; /* send error occurred */
	//}

	/* remove xid entry */
	xid_del(&pif_relay->r_ctrl->xid_list, pkt->xid);
}


int dhcp_relay_fsm(struct if_dhcp *pif, struct pkt_buffer *pkt)
{
	struct dhcpMessage *pdhcp_msg = NULL;
	struct sockaddr_in	client_addr;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "Entering the function of '%s'",__func__);
	
	if(!pif || pif->status == DISABLE || pif->mode !=IFNET_MODE_L3)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "leave the function of '%s' because of status or mode", __func__);
	
		return 0;
	}

	pif_relay = pif;

	if(!pif_relay)
	{
		return 0;
	}
	
	if(pkt)
	{
		pdhcp_msg = pkt->data;
	}
	
	if(pif_relay->dhcp_role == E_DHCP_RELAY)
	{			

		if(!pif_relay->r_ctrl)
		{
			return 0;
		}
		if(pif_relay->r_ctrl->nexthop==0)
		{
			return 0;
		}
	}
	else
	{
		if(pif_relay->r_ctrl)
		{
			xid_del_all(&pif_relay->r_ctrl->xid_list);
			XFREE(MTYPE_IF,pif_relay->r_ctrl);
			pif_relay->r_ctrl = NULL;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_RELAY, "free pif->r_ctrl success");			
		}
		
		return 0;
	}

	if(!pdhcp_msg)
	{
		return 0;
	}
	
	if (pkt)
	{
		if(pkt->in_ifindex != pif_relay->ifindex)
		{
			pass_to_client(pdhcp_msg, pif_relay->ip_addr, pif_relay->ifindex);
		}
		else
		{
			pdhcp_msg->giaddr = pif_relay->ip_addr;			

			client_addr.sin_addr.s_addr = pkt->cb.ipcb.sip;
			client_addr.sin_port = pkt->cb.ipcb.sport; 
			pass_to_server(pdhcp_msg, &client_addr,
				pif_relay->ip_addr,
				pif_relay->r_ctrl->nexthop);
		}		
	}
	return 0; /* - not reached */
}
