#include <unistd.h>
#include <string.h>
//#include <features.h>
#include <errno.h>
#include <lib/prefix.h>
#include <lib/ether.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include "packet.h"
#include "dhcp_server.h"
#include "options.h"
#include "dhcp.h"
#include "dhcp_client.h"


int untag_socket = -1;
int tag_socket   = -1;
int dhcpvid = -1;

void init_header(struct dhcpMessage *packet, char type)
{
	memset(packet, 0, sizeof(struct dhcpMessage));
	packet->op = BOOTREQUEST;	
	
	switch (type) {
		case DHCPOFFER:
		case DHCPACK:
		case DHCPNAK:
			packet->op = BOOTREPLY;
		default:
			break;
	}
	packet->htype = ETH_10MB;
	packet->hlen = ETH_10MB_LEN;
	packet->cookie = htonl(DHCP_MAGIC);
	packet->options[0] = DHCP_END;
	add_simple_option(packet->options, DHCP_MESSAGE_TYPE, type);
}

/* read a packet from socket fd, return -1 on read error, -2 on packet error */
int check_packet(struct dhcpMessage *packet)
{
	int i;
	const char broken_vendors[][8] = {
		"MSFT 98",
		""
	};
	uint8_t *vendor;


	if (ntohl(packet->cookie) != DHCP_MAGIC) 
	{
		DHCP_LOG_ERROR("received bogus message, ignoring");
		return 0;
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "Received a packet");

	if (packet->op == BOOTREQUEST && (vendor = get_option(packet, DHCP_VENDOR)))
	{
		for (i = 0; broken_vendors[i][0]; i++) 
		{
			if (vendor[OPT_LEN - 2] == (uint8_t) strlen(broken_vendors[i]) &&
			    !strncmp((char *)vendor, broken_vendors[i], vendor[OPT_LEN - 2])) 
			{
		    	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "broken client (%s), forcing broadcast",
		    		broken_vendors[i]);
		    	packet->flags |= htons(BROADCAST_FLAG);
			}
		}
	}
	return 1;
}

uint16_t checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	 *         beginning at location "addr".
	 */
	register int32_t sum = 0;
	uint16_t *source = (uint16_t *) addr;

	while(count > 1)
	{
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if(count > 0)
	{
		/* Make sure that the left-over byte is added correctly both
		 * with little and big endian hosts */
		uint16_t tmp = 0;
		*(uint8_t *) (&tmp) = * (uint8_t *) source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}
	return ~sum;
}

/* Construct a ip/udp header for a packet, and specify the source and dest hardware address */
int raw_packet(struct dhcpMessage *payload, uint32_t source_ip, int source_port,
		   uint32_t dest_ip, int dest_port, uint8_t *dest_arp, int ifindex, struct option124_vlan_ifvalid option124_vlan, int zero)
{
	int result;
	uint16_t tValnNo = 0;
	union pkt_control pktcontrol;
	struct ifm_info   pifm_info = {0};
					  			  /*option 124  0x7C */			/*length 10*/			 /*     4 byte 9966  *//*len 5*//*len 2*//*2byte vlan*//**len 1*//*invalid*/
	struct vendor_124 vendor_id = {DHCP_VI_VERDOR_IDENTIFY,  OPTION_VERDOR_IDENTIFY_LEN, {0x00, 0x00, 0x26, 0xEE, 0x05,  0x02,    0x00, 0x00,    0x01,    0x01}};

    if(option124_vlan.vlanid == VLAN_NONE)
    {
        struct udp_dhcp_packet packet;

		memset(&packet, 0, sizeof(packet));
        packet.ip.protocol = IP_P_UDP;
        packet.ip.saddr = source_ip;
        packet.ip.daddr = dest_ip;
        packet.udp.sport = htons(source_port);
        packet.udp.dport = htons(dest_port);
        packet.udp.len = htons(sizeof(packet.udp) + sizeof(struct dhcpMessage)); /* cheat on the psuedo-header */
        packet.ip.tot_len = packet.udp.len;
        memcpy(&(packet.data), payload, sizeof(struct dhcpMessage));
        /*contradiction*/

		/* get ifindex vlanid */
		result = ifm_get_all_info(ifindex, MODULE_ID_DHCP, &pifm_info);
        
		if((result == 0) && pifm_info.encap.svlan.vlan_start)
		{
			tValnNo = htons(pifm_info.encap.svlan.vlan_start);
		} 
		else
		{
			tValnNo = (uint16_t)htons(dhcpcm.conf->InBandVlanID);
		}
	    memcpy(&vendor_id.str[6], &tValnNo, 2);
		dhcpvid = tValnNo;
		
		if(dhcpcm.conf->MgmtVidFlag == 0)
		{
			vendor_id.str[9] = 1;
		}
		else
		{
			vendor_id.str[9] = 0;
		}
		add_option_string(packet.data.options, (uint8_t *) &vendor_id);
		
		packet.udp.checksum = checksum(&packet, sizeof(struct udp_dhcp_packet));
        packet.ip.tot_len = htons(sizeof(struct udp_dhcp_packet));
        packet.ip.ihl = sizeof(packet.ip) >> 2;
        packet.ip.version = 4;
        packet.ip.ttl = 255;
		packet.ip.tos = 2;
        packet.ip.check = checksum(&(packet.ip), sizeof(packet.ip));

		memset(&(pktcontrol.ethcb), 0, sizeof(struct eth_control));
		pktcontrol.ethcb.ifindex		 = ifindex;
		pktcontrol.ethcb.smac_valid 	 = 0;
		memcpy(pktcontrol.ethcb.dmac,dest_arp,6);
		pktcontrol.ethcb.ethtype = ETH_P_IP;
		pktcontrol.ethcb.cos = 2;
		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "before pkt_send eth\n ");
		result = pkt_send(PKT_TYPE_ETH, &pktcontrol, &packet, sizeof( struct udp_dhcp_packet ));		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "after pkt_send eth\n ");
		
        if (result > 0) 
		{
        	DHCP_LOG_ERROR("pkt_send failed");
        }
    }
    else
    {
		struct udp_dhcp_packet packet;

		tValnNo = (uint16_t)htons(option124_vlan.vlanid);

		memset(&packet, 0, sizeof(packet));
		//packet.tci = ((7 << 13) & 0xe000) | (tValnNo & 0xfff);
		//packet.ether_type = htons(ETH_P_IP);
		packet.ip.protocol = IP_P_UDP;
		packet.ip.saddr = source_ip;
		packet.ip.daddr = dest_ip;
		packet.udp.sport = htons(source_port);
		packet.udp.dport = htons(dest_port);
		// cheat on the psuedo-header 
		packet.udp.len = htons(sizeof(packet.udp) + sizeof(struct dhcpMessage)); 
		packet.ip.tot_len = packet.udp.len;
		memcpy(&(packet.data), payload, sizeof(struct dhcpMessage));
		memcpy(&vendor_id.str[6], &tValnNo, 2);
		dhcpvid = option124_vlan.vlanid;
		vendor_id.str[9] = option124_vlan.ifvalid;
		
		/*we use option124 as the market requirement 2015-9-10*/
		/*use NM server config remove option124 2015-8-5*/
		add_option_string(packet.data.options, (uint8_t *) &vendor_id);
		packet.udp.checksum = checksum(&packet, sizeof(struct udp_dhcp_packet));
		packet.ip.tot_len = htons(sizeof(struct udp_dhcp_packet));
		packet.ip.ihl = sizeof(packet.ip) >> 2;
		packet.ip.version = 4;
		packet.ip.ttl = 255;
		packet.ip.check = checksum(&(packet.ip), sizeof(packet.ip));
		
		memset(&(pktcontrol.ethcb), 0, sizeof(struct eth_control));
		pktcontrol.ethcb.ifindex		 = ifindex;
		memcpy(pktcontrol.ethcb.dmac,dest_arp,6);
		if (zero == 1)
		{
			pktcontrol.ethcb.svlan = option124_vlan.vlanid;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "svlan %d!", pktcontrol.ethcb.svlan);
			pktcontrol.ethcb.smac_valid 	 = 1;
			pktcontrol.ethcb.is_changed 	 = 0;
			pktcontrol.ethcb.cos 	 = 2;
			pktcontrol.ethcb.ethtype = ETH_P_IP;
			result = ifm_get_all_info(ifindex, MODULE_ID_DHCP, &pifm_info);
			if (result == 0) {
				memcpy(pktcontrol.ethcb.smac, pifm_info.mac, 6);
				pktcontrol.ethcb.svlan_tpid = pifm_info.tpid;
			}
		}
		else { 
			pktcontrol.ethcb.ethtype = ETH_P_IP;
			pktcontrol.ethcb.smac_valid 	 = 0;
		}		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "before pkt_send eth\n ");
		result = pkt_send(PKT_TYPE_ETH, &pktcontrol, &packet, sizeof(struct udp_dhcp_packet));		
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "after pkt_send eth\n ");
		
		if (result > 0)
		{
			DHCP_LOG_ERROR("pkt_send failed");
		}
    }

	return result;
}

/* Let the kernel do all the work for packet generation */
int kernel_packet(struct dhcpMessage *payload, uint32_t source_ip, int source_port,
		   uint32_t dest_ip, int dest_port)
{
	int result;	
	union pkt_control pktcontrol;
	struct in_addr addr;
	uint32_t source_ip_tmp, dest_ip_tmp;
	memset(&pktcontrol,0,sizeof(union pkt_control));
	source_ip_tmp = source_ip;
	pktcontrol.ipcb.sip = htonl(source_ip);	
	pktcontrol.ipcb.sport = source_port;
	dest_ip_tmp = dest_ip;
	pktcontrol.ipcb.dip = htonl(dest_ip);
	pktcontrol.ipcb.dport = dest_port;
	pktcontrol.ipcb.vpn = 0;
	pktcontrol.ipcb.protocol = IP_P_UDP;
	pktcontrol.ipcb.ttl = 64;
	pktcontrol.ipcb.tos = 2;
	pktcontrol.ipcb.chsum_enable = 1;
		
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "before pkt_send kernel_packet\n ");
	
	addr.s_addr 	= source_ip_tmp;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "source_ip:%s, source_port:%d\n",inet_ntoa(addr),source_port);	
	addr.s_addr 	= dest_ip_tmp;	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "dest_ip  :%s, dest_port  :%d\n",inet_ntoa(addr),dest_port);
	result = pkt_send(PKT_TYPE_UDP, &pktcontrol, payload, sizeof(struct dhcpMessage));
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_ALL, "after pkt_send kernel_packet\n ");

	return result;
}


