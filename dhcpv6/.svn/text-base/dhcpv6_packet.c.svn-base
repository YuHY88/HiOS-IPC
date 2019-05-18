/******************************************************************************
 * Filename: dhcp6_packet.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.23  wumingming created
 *
******************************************************************************/
#include <stdio.h>

#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/queue.h>
#include <lib/pkt_buffer.h>
#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include <lib/errcode.h>

#include "dhcp6.h"
#include "dhcp6c.h"
#include "common.h"
#include "dhcpv6_packet.h"

int check_dhcpv6_packet(struct pkt_buffer *packet)
{
	DHCPV6_LOG_DEBUG("check packect is if dhcpv6!");
	DHCPV6_LOG_DEBUG("sport %d, dport %d!", packet->cb.ipcb.sport, packet->cb.ipcb.dport);
	if ((packet->cb.ipcb.sport == UDP_PORT_DHCPV6_CLIENT
		&& packet->cb.ipcb.dport == UDP_PORT_DHCPV6_SERVER)
		|| (packet->cb.ipcb.dport == UDP_PORT_DHCPV6_CLIENT
		&& packet->cb.ipcb.sport == UDP_PORT_DHCPV6_SERVER)) {
		return 0;
	} else {
		DHCPV6_LOG_DEBUG("The packet is not dhcpv6 packet, sport is %d, dport is %d!", 
			packet->cb.ipcb.sport, packet->cb.ipcb.dport);
		return 1;
	}	
}
uint16_t in_checksum (uint16_t *ptr, int nbytes)
{
	register long sum;
	u_int16_t oddbyte = 0;
	register u_int16_t result;
	u_int8_t *p, *q;

	sum = 0;
	while (nbytes > 1)  
	{
        sum += *ptr++;
        nbytes -= 2;
	}

	if (nbytes == 1) 
	{
        p = (u_int8_t *)&oddbyte;
        q = (u_int8_t *)ptr;
        *p = *q;
        sum += oddbyte;
	}
	
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	result = ~sum;

	return result;
}
int create_ipv6_addr(int ifindex, char *addr)
{
	uchar *pmac = NULL;
	char mac;
	pmac = ifm_get_mac(ifindex, MODULE_ID_DHCPV6);
	if(pmac)
	{
		LOG (LOG_DEBUG, "Info: get interface mac :[%02X:%02X:%02X:%02X:%02X:%02X] \n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	} else {
		LOG (LOG_ERR, "Info: get interface mac is failed \n");
		return 1;
	}
	/* 杞寲涓轰簩杩涘埗绗竷浣嶅弽杞?*/
	mac = pmac[0] ^ (1 << 1);
			/*mac         [0] [1] [2]       [3]  [4] [5]*/				
	sprintf(addr, "fe80::%02x%02x:%02xff:fe%02x:%02x%02x", mac, pmac[1], pmac[2], pmac[3], pmac[4], pmac[5]);
	LOG (LOG_DEBUG, "Info: create ipv6 address is %s \n", addr);
	return 0;
}

/**
 * @brief      : 閺嶈宓?MAC 閸︽澘娼冮悽鐔稿灇闁炬崘鐭鹃張顒€婀撮崷鏉挎絻
 * @param[in ] : pmac  - MAC 閸︽澘娼? * @param[out] : pipv6 - 閻㈢喐鍨氶惃鍕懠鐠侯垱婀伴崷鏉挎勾閸р偓
 * @return     : 閹存劕濮涙潻鏂挎礀 ERRNO_SUCCESS閿涘苯鎯侀崚娆掔箲閸ョ偤鏁婄拠顖滅垳 ERRNO_XXX
 * @author     : Wumm
 * @date       : 2019楠?閺?0閺?14:43:53
 * @note       : u 娴ｅ秵婀紙鏄忔祮
 */
int dhcp_if_get_linklocal(uint32_t ifindex, uint8_t *addr)
{
	uchar *pmac = NULL;

    if (NULL == addr)
    {
		DHCPV6_LOG_ERR("addr is NULL!\n");
        return ERRNO_PARAM_ILLEGAL;
    }
	pmac = ifm_get_mac(ifindex, MODULE_ID_DHCPV6);
	if(pmac)
	{
		DHCPV6_LOG_DEBUG("Info: get interface mac :[%02X:%02X:%02X:%02X:%02X:%02X] \n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	}else{
		DHCPV6_LOG_ERR("Info: get interface mac is failed \n");
		return ERRNO_FAIL;
	}
    addr[0] = 0xfe;                    // 第 0 个字节 fe
    addr[1] = 0x80;                    // 第 1 个字节 80
    memcpy(&addr[8], pmac, 3);         // 8 - 10 字节拷贝 mac 的前 3 个字节
    addr[11] = 0xff;                   // 第 11 字节固定为 ff
	addr[12] = 0xfe;                   // 第 12 字节固定为 fe
    memcpy(&addr[13], &pmac[3], 3);    // 13 - 15 字节拷贝 mac 的后 3 个字节
    return ERRNO_SUCCESS;
}
/* Let the kernel do all the work for packet generation */
int send_multicast_packet(uint8_t *payload, uint32_t ifindex, int len)
{
	DHCPV6_LOG_DEBUG("send multicast packet...");

	union pkt_control pkt_ctrl;
	int ret = -1;
	struct udp_dhcpv6_packet packet;
	struct pseudo_ipv6hdr ppseudo;
	char data[512] = {0};
	char saddr[INET6_ADDRSTRLEN] = {0};
    uint8_t ipv6[16] = "";
	
	/* 灏佽eth灞?*/
	memset(&pkt_ctrl,0,sizeof(pkt_ctrl));
	pkt_ctrl.ethcb.dmac[0] = 0x33;
	pkt_ctrl.ethcb.dmac[1] = 0x33;
	pkt_ctrl.ethcb.dmac[2] = 0x00;
	pkt_ctrl.ethcb.dmac[3] = 0x01;
	pkt_ctrl.ethcb.dmac[4] = 0x00;
	pkt_ctrl.ethcb.dmac[5] = 0x02;
	pkt_ctrl.ethcb.ifindex = ifindex;
	pkt_ctrl.ethcb.ethtype = ETH_P_IPV6;
	pkt_ctrl.ethcb.cos = 2;
	
	/* 灏佽 ipv6澶撮儴 */
	memset(&packet, 0 ,sizeof(struct udp_dhcpv6_packet));
	packet.ip.version = 6;
	packet.ip.tos1 = htons(2);
	packet.ip.data_len = htons(len + UDP_HEADER_SIZE);   //鍑€鑽烽暱搴︼紙payload length锛? udp澶?data闀垮害
	packet.ip.ttl = 64;   
	packet.ip.protocol = IP_P_UDP;
	inet_pton (AF_INET6, DH6ADDR_ALLAGENT, packet.ip.daddr.ipv6);
#if 0
	if (create_ipv6_addr(ifindex, saddr)) {
		LOG(LOG_ERR, "create ipv6 address is failed!");
		return 1;
	}
#else
	if (dhcp_if_get_linklocal(ifindex, ipv6)) {
		DHCPV6_LOG_ERR("get ipv6 address is failed!");
		return 1;
	}
#endif
    memcpy(packet.ip.saddr.ipv6, ipv6, IPV6_ADDR_LEN);
	inet_ntop(AF_INET6, packet.ip.saddr.ipv6, saddr, INET6_ADDRSTRLEN);

	DHCPV6_LOG_DEBUG("daddr %s\n", DH6ADDR_ALLAGENT);

	DHCPV6_LOG_DEBUG("saddr %s\n", saddr);

	/* 灏佽udp */
	packet.udp.sport = htons(UDP_PORT_DHCPV6_CLIENT);
	packet.udp.dport = htons(UDP_PORT_DHCPV6_SERVER);
	packet.udp.len = htons(len + UDP_HEADER_SIZE);//udp闀垮害: udp澶?data闀垮害
	packet.udp.checksum= 0;
	
	/* 鍋忕Щ鍑轰吉棣栭儴, 璁＄畻 checksum */
	memset(&ppseudo, 0, sizeof(struct pseudo_ipv6hdr));
	IPV6_ADDR_COPY(&ppseudo.daddr, &packet.ip.daddr.ipv6);
	IPV6_ADDR_COPY(&ppseudo.saddr, &packet.ip.saddr.ipv6);
	ppseudo.protocol = IP_P_UDP;
	ppseudo.length = packet.udp.len;
	ppseudo.placeholder = 0x00;
	memcpy(data, (unsigned char *)&ppseudo, sizeof(struct pseudo_ipv6hdr));
	memcpy(data + sizeof(struct pseudo_ipv6hdr), (unsigned char *)&packet.udp, UDP_HEADER_SIZE);
	memcpy(data + sizeof(struct pseudo_ipv6hdr) + UDP_HEADER_SIZE, (unsigned char *)payload, len);
	packet.udp.checksum = in_checksum((uint16_t *)data, sizeof(struct pseudo_ipv6hdr)+ UDP_HEADER_SIZE + len);
    memcpy(&packet.data, payload, len);
	DHCPV6_LOG_DEBUG("ip len is %d, udp len is %d", packet.ip.data_len, packet.udp.len);
	DHCPV6_LOG_DEBUG("dhcpv6 send use PKT_TYPE_ETH send multicast packet, from ifindex 0x%x, payload len is %d\n", ifindex, len);
	//DHCPV6_LOG_DEBUG("dhcpv6 msgtype %x, xid %x\n", payload->msgtype, payload->xid);
	//DHCPV6_LOG_DEBUG("dhcpv6 msgtype %x, xid %x\n", packet.message.msgtype, packet.message.xid);

	char *option;
	option = malloc(512);
	option = (char *)payload;
	if(option)
	{
		DHCPV6_LOG_DEBUG("dhcpv6 option: ");
		int i = 0;
		for(i = 0; i < 512/16; i++)
		{
			DHCPV6_LOG_DEBUG("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
						option[0], option[1],option[2],option[3],option[4],option[5],option[6],option[7],
						option[8], option[9],option[10],option[11],option[12],option[13],option[14],option[15]);
		}
	}
/*
	if(payload)
	{
		DHCPV6_LOG_DEBUG("dhcpv6 option: ");
		int i = 0;
		for(i = 0; i < len/16; i++)
		{
			DHCPV6_LOG_DEBUG("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
						payload[0], payload[1],payload[2],payload[3],payload[4],payload[5],payload[6],payload[7],
						payload[8], payload[9],payload[10],payload[11],payload[12],payload[13],payload[14],payload[15]);
		}

		
		DHCPV6_LOG_DEBUG("dhcpv6 packet.option: ");
		for(i = 0; i < len/16; i++)
		{
			DHCPV6_LOG_DEBUG("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
						packet.data[0], packet.data[1],packet.data[2],packet.data[3],packet.data[4],packet.data[5],packet.data[6],packet.data[7],
						packet.data[8], packet.data[9],packet.data[10],packet.data[11],packet.data[12],packet.data[13],packet.data[14],packet.data[15]);
		}
	}
	*/
	ret = pkt_send(PKT_TYPE_ETH, &pkt_ctrl, &packet,sizeof(struct ipv6_hdr) + sizeof(struct udphdr) + len);
	return ret;
}


