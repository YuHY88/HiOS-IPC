/* serverpacket.c
 *
 * Construct and send DHCP server packets
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <time.h>

#include "pool_address.h"
#include "serverpacket.h"
#include "dhcp_server.h"
#include "options.h"

#include "pool.h"
#include "dhcp_if.h"

extern struct ip_pool *p_pool;
extern struct if_dhcp *pif_server;


/* send a packet to giaddr using the kernel ip stack */
static int send_packet_to_relay(struct dhcpMessage *payload)
{
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Forwarding packet to relay");

	return kernel_packet(payload, pif_server->ip_addr, SERVER_PORT,
			payload->giaddr, SERVER_PORT);
}


/* send a packet to a specific arp address and ip address by creating our own ip packet */
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast)
{
	uint8_t *chaddr;
	uint32_t ciaddr;
	struct option124_vlan_ifvalid option124_vlan;
	int flag = DISABLE;/* 是否要发送单播地址 */

	if (force_broadcast)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "broadcasting packet to client (NAK)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} 
	else if (payload->ciaddr)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "unicasting packet to client ciaddr");
		ciaddr = payload->ciaddr;
		chaddr = payload->chaddr;
		flag = ENABLE;
	} 
	else if (ntohs(payload->flags) & BROADCAST_FLAG)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "broadcasting packet to client (requested)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} 
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "broadcasting packet to client yiaddr");
		//ciaddr = payload->yiaddr;
		//chaddr = payload->chaddr;
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
		
	}
	option124_vlan.vlanid = 0;
	option124_vlan.ifvalid = 1;
	if(flag == ENABLE)
	{
		return kernel_packet(payload, pif_server->ip_addr, SERVER_PORT,
			ciaddr, CLIENT_PORT);
	}
	else 
	{ 
		return raw_packet(payload, pif_server->ip_addr, SERVER_PORT,
			ciaddr, CLIENT_PORT, chaddr, pif_server->ifindex, option124_vlan, 0);
	}
}


/* send a dhcp packet, if force broadcast is set, the packet will be broadcast to the client */
static int send_packet(struct dhcpMessage *payload, int force_broadcast)
{
	int ret;

	if(payload->giaddr)
	{
		ret = send_packet_to_relay(payload);
	}
	else
	{ 
		ret = send_packet_to_client(payload, force_broadcast);
	}
	return ret;
}

static void init_packet(struct dhcpMessage *packet, struct dhcpMessage *oldpacket, char type)
{
	init_header(packet, type);
	packet->xid = oldpacket->xid;
	memcpy(packet->chaddr, oldpacket->chaddr, 16);
	packet->flags = oldpacket->flags;
	packet->giaddr = oldpacket->giaddr;
	packet->ciaddr = oldpacket->ciaddr;
	add_simple_option(packet->options, DHCP_SERVER_ID, pif_server->ip_addr);
	add_simple_option(packet->options, DHCP_SUBNET, p_pool->dhcp_pool.mask);
	add_simple_option(packet->options, DHCP_ROUTER, p_pool->dhcp_pool.gateway);
	add_simple_option(packet->options, DHCP_DNS_SERVER, p_pool->dhcp_pool.dns_server);
}


/* add in the bootp options */
#if 0
static void add_bootp_options(struct dhcpMessage *packet)
{
	packet->siaddr = server_config.siaddr;
	if (server_config.sname)
		strncpy(packet->sname, server_config.sname, sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		strncpy(packet->file, server_config.boot_file, sizeof(packet->file) - 1);
}
#endif

/* send a DHCP OFFER to a DHCP DISCOVER */
int sendOffer(struct dhcpMessage *oldpacket, struct dhcpOfferedAddr *offeredaddr)
{
	struct dhcpMessage packet;
	uint32_t  lease_time_align = p_pool->dhcp_pool.leasetime;
	uint8_t  *lease_time;
	//struct option_set *curr;
	struct in_addr inaddr;


	if(!offeredaddr)
	{
		DHCP_LOG_ERROR("offeredaddr is NULL");
		return -1;		
	}

	memset(&packet,0,sizeof(struct dhcpMessage));
	init_packet(&packet, oldpacket, DHCPOFFER);

	packet.yiaddr = offeredaddr->yiaddr;


	if(!packet.yiaddr) 
	{
		DHCP_LOG_ERROR("no IP addresses to give -- OFFER abandoned");
		return -1;
	}

	if((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) 
	{
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);

		if(lease_time_align > p_pool->dhcp_pool.leasetime || lease_time_align < 60)
		{
			lease_time_align = p_pool->dhcp_pool.leasetime;
		}			
	}

	if(!p_pool->dhcp_pool.leasetime || offeredaddr->type == e_type_static)
	{
		lease_time_align = 0x7fffffff; // unlimited			
	}

		
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "add_lease ,lease_time_align %d minutes\n",lease_time_align);

	offeredaddr->expires = time(0) + 7;
	offeredaddr->ifindex = pif_server->ifindex; 						
	offeredaddr->state = e_state_send_offer;
	
	/* ADDME: end of short circuit */

	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

	/*curr = server_config.options;
	while (curr) 
	{
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}*/

	//add_bootp_options(&packet);
	inaddr.s_addr		= packet.yiaddr;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "sending OFFER of %s", inet_ntoa(inaddr));
	return send_packet(&packet, 0);
}

int sendNAK(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;

	init_packet(&packet, oldpacket, DHCPNAK);

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "sending NAK");
	return send_packet(&packet, 1);
}

int sendACK(struct dhcpMessage *oldpacket, uint32_t yiaddr,struct dhcpOfferedAddr *offeredaddr)
{
	struct dhcpMessage packet;
	//struct option_set *curr;
	uint8_t *lease_time;
	uint32_t lease_time_align = p_pool->dhcp_pool.leasetime;
	
	struct in_addr inaddr;

	init_packet(&packet, oldpacket, DHCPACK);
	packet.yiaddr = yiaddr;

	if((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) 
	{
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);

		if(lease_time_align > p_pool->dhcp_pool.leasetime || lease_time_align < 60)
		{
			lease_time_align = p_pool->dhcp_pool.leasetime;
		}		
	}


	if(!p_pool->dhcp_pool.leasetime || offeredaddr->type == e_type_static)
	{
		lease_time_align = 0x7fffffff; // unlimited 		
	}
	
	if(lease_time_align == 0x7fffffff)
	{
		offeredaddr->expires = lease_time_align;
	}
	else
	{
		offeredaddr->expires = time(0) + lease_time_align;
	}
	offeredaddr->state = e_state_send_ack;

	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

	/*curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}*/

	//add_bootp_options(&packet);
	inaddr.s_addr		= packet.yiaddr;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "sending ACK to %s", inet_ntoa(inaddr));

	if(send_packet(&packet, 0) < 0)
	{
		return -1;
	}
	//add_lease(packet.chaddr, packet.yiaddr, lease_time_align);

	return 0;
}

int send_inform(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	//struct option_set *curr;

	init_packet(&packet, oldpacket, DHCPACK);

	/*curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}*/

	//add_bootp_options(&packet);

	return send_packet(&packet, 0);
}
