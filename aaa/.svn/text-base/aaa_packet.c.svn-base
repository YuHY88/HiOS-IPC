/******************************************************************************
 * Filename: aaa_packet.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.9.20  luoyz created
 *
******************************************************************************/

#include <string.h>
#include <unistd.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/sockunion.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_tcp.h>

#include "aaa_packet.h"
#include "aaa_radius.h"
#include "aaa_tac_plus.h"
#include "tacplus.h"
#include "aaa_dot1x.h"


extern struct thread_master *aaa_master;
extern int syslog_debug;

aaa_pkt_debug aaa_pkt_dbg = {0, 0, 0, 0};
static const char   *protostr[] = {"radius", "tacacs+", "unknown"};

void aaa_radius_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(proto));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.sport = RADIUS_AUTH_UDP_PORT;
	pkt_register(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);

	proto.ipreg.sport = RADIUS_ACCT_UDP_PORT;
	pkt_register(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);
}

void aaa_radius_pkt_unregister(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(proto));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.sport = RADIUS_AUTH_UDP_PORT;
	pkt_unregister(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);

	proto.ipreg.sport = RADIUS_ACCT_UDP_PORT;
	pkt_unregister(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);
}

/* register to receive packet through ftm module */
void aaa_tac_plus_pkt_register(void)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(proto));
	proto.ipreg.protocol = IP_P_TCP;
	proto.ipreg.sport = TAC_PLUS_PORT;
	pkt_register(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);
}

void aaa_tac_plus_pkt_unregister(void)
{
	union proto_reg proto;
	memset(&proto, 0, sizeof(proto));
	proto.ipreg.protocol = IP_P_TCP;
	proto.ipreg.sport = TAC_PLUS_PORT;
	pkt_unregister(MODULE_ID_AAA, PROTO_TYPE_IPV4, &proto);
}


/* ieee802.1x收包注册 */
void aaa_dot1x_pkt_register(void)
{
	union proto_reg proto;
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ethreg.dmac_valid = 0;	
	proto.ethreg.ethtype = IEEE802_1X_TYPE;
	pkt_register(MODULE_ID_AAA, PROTO_TYPE_ETH, &proto);
}


/* ieee802.1x收包解注册 */
void aaa_dot1x_pkt_unregister(void)
{
	union proto_reg proto;
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ethreg.dmac_valid = 0;	
	proto.ethreg.ethtype = IEEE802_1X_TYPE;
	pkt_unregister(MODULE_ID_AAA, PROTO_TYPE_ETH, &proto);
}



int aaa_tac_plus_pkt_send(int type, char *buf, uint32_t len, uint16_t src_port)
{
	struct sockaddr_in dst;
	union pkt_control pkt_ctrl;
	int ret;

	uint16_t vpn = 0;

	if(NULL == buf) return AAA_FAIL;

	memset(&dst, 0, sizeof(dst));
	switch(type)
	{
		case TAC_PLUS_AUTHEN:
			tac_plus_authen_server_get (&dst);
			vpn = tac_plus_authen_server_vpn_get();
			zlog_debug(AAA_DBG_TACACS, "%s[%d]->%s : authen port = %d\n", __FILE__, __LINE__, __func__, src_port);
			break;
		case TAC_PLUS_AUTHOR:
			tac_plus_author_server_get (&dst);
			vpn = tac_plus_author_server_vpn_get();
			zlog_debug(AAA_DBG_TACACS, "%s[%d]->%s : author port = %d\n", __FILE__, __LINE__, __func__, src_port);
			break;
		case TAC_PLUS_ACCT:
			tac_plus_acct_server_get (&dst);
			vpn = tac_plus_acct_server_vpn_get();
			zlog_debug(AAA_DBG_TACACS, "%s[%d]->%s : acct port = %d\n", __FILE__, __LINE__, __func__, src_port);
			break;
		default:
			return AAA_ERROR;
	}
		
	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
	pkt_ctrl.ipcb.dip = ntohl(dst.sin_addr.s_addr);
	pkt_ctrl.ipcb.protocol = IP_P_TCP;
	pkt_ctrl.ipcb.dport = ntohs(dst.sin_port);
	pkt_ctrl.ipcb.sport = src_port;
	pkt_ctrl.ipcb.vpn = vpn;
	pkt_ctrl.ipcb.is_changed = 1;
	pkt_ctrl.ipcb.tos = 4; /* aaa 报文的优先级为 4 */

	ret = pkt_send (PKT_TYPE_TCP, &pkt_ctrl, buf, len);
	
	if(ret != NET_SUCCESS)
	{
		zlog_debug(AAA_DBG_TACACS, "%s[%d] call pkt_send failed!", __FUNCTION__, __LINE__);
		return AAA_PKT_SEND_FAIL;
	}

	return AAA_OK;
}


/* Receive udp packets for radius or tcp packets for tacacs+ */
int aaa_handle_ftm_msg(struct ipc_mesg_n *pmsg)
{
	struct pkt_buffer *pkt = (struct pkt_buffer *)pmsg->msg_data;
	uint16_t src_port;
	int pkt_type = PKT_TYPE_INVALID;
	if(pkt != NULL)
	{		
		pkt_type = pkt->cb_type;
		src_port = pkt->cb.ipcb.sport;
		pkt->data = (char *)pkt + pkt->data_offset;
		if(PKT_TYPE_UDP == pkt_type)
		{
			if(RADIUS_AUTH_UDP_PORT == src_port || RADIUS_ACCT_UDP_PORT == src_port)
				aaa_radius_rcv_pkt_handle(pkt);
			else
				zlog_debug(AAA_DBG_RADIUS, "%s[%d] received udp pkt error src port(%u)!",
					__FUNCTION__, __LINE__, src_port);
		}
		else if(PKT_TYPE_TCP == pkt_type)
		{
			if((tac_plus_cfg_authen_server_port_get() == htons(src_port))\
				|| (tac_plus_cfg_author_server_port_get() == htons(src_port))\
				|| (tac_plus_cfg_acct_server_port_get() == htons(src_port)))
			{
				aaa_tac_plus_process_recv_pkt(pkt->data, pkt->data_len);
			}				
			else
			{
				zlog_debug(AAA_DBG_TACACS, "%s[%d] received tcp pkt error src port(%u)!",
					__FUNCTION__, __LINE__, src_port);
			}	
		}
		else if(PKT_TYPE_ETH == pkt_type)
		{
			aaa_dot1x_rcv_pkt_handle (pkt);
		}
		else
			zlog_debug(AAA_DBG_COMMON, "%s[%d] received pkt error, pkt_type(%d)!",
					__FUNCTION__, __LINE__, pkt_type);
	}

	return 0;
}



//buf  : radius pkt
//len  : radius length
//pkt_type : auth/acct pkt
int aaa_radius_pkt_send(char *buf, int len, int pkt_type)
{
	struct sockaddr_in dst;
	union pkt_control pkt_ctrl;
	int ret;

	if(NULL == buf) return AAA_FAIL;

	
	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));

	if(AUTH_PKT == pkt_type)
	{
		radius_auth_server_get(&dst);
		pkt_ctrl.ipcb.dip   = ntohl(dst.sin_addr.s_addr);	
		pkt_ctrl.ipcb.dport = ntohs(dst.sin_port);
		pkt_ctrl.ipcb.sport = ntohs(dst.sin_port);
		pkt_ctrl.ipcb.vpn   = radius_auth_server_vpn_get();
	}
	else if(ACCT_PKT == pkt_type)
	{
		radius_acct_server_get(&dst);
		pkt_ctrl.ipcb.dip   = ntohl(dst.sin_addr.s_addr);	
		pkt_ctrl.ipcb.dport = ntohs(dst.sin_port);
		pkt_ctrl.ipcb.sport = ntohs(dst.sin_port);
		pkt_ctrl.ipcb.vpn   = radius_acct_server_vpn_get();
	}
		
	pkt_ctrl.ipcb.protocol = IP_P_UDP;
	pkt_ctrl.ipcb.ttl = IP_TTL_DEFAULT;
	pkt_ctrl.ipcb.is_changed = 1;
	pkt_ctrl.ipcb.tos = 4;

	aaa_pkt_dump(buf, len, AAA_PKT_SEND, AAA_RADIUS, __FUNCTION__);
	ret = pkt_send(PKT_TYPE_UDP, &pkt_ctrl, buf, len);
	if(ret != NET_SUCCESS)
	{
		zlog_debug(AAA_DBG_RADIUS, "%s[%d] call pkt_send failed!\n", __FUNCTION__, __LINE__);
		return AAA_PKT_SEND_FAIL;
	}

	return AAA_OK;
}


/* 发送eap报文 */
int aaa_eap_pkt_send (char *buf, size_t len, uint32_t ifindex, char *dmac) 
{
	union pkt_control pkt_ctrl;
	int ret;

	memset (&pkt_ctrl, 0, sizeof (union pkt_control));
	pkt_ctrl.ethcb.ethtype = IEEE802_1X_TYPE;	/* 以太层协议类型, mac 后两个字节 */
	pkt_ctrl.ethcb.smac_valid = 0;
	pkt_ctrl.ethcb.is_changed = 1;
	pkt_ctrl.ethcb.ifindex = ifindex;
	pkt_ctrl.ethcb.cos = 0;
	memcpy (pkt_ctrl.ethcb.dmac, dmac, sizeof (pkt_ctrl.ethcb.dmac));
	
	aaa_pkt_dump (buf, len, AAA_PKT_SEND, AAA_EAP, __FUNCTION__);
	ret = pkt_send (PKT_TYPE_ETH, &pkt_ctrl, buf, len);
	if(ret != NET_SUCCESS)
	{
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] call pkt_send failed!\n", __FUNCTION__, __LINE__);
		return AAA_PKT_SEND_FAIL;
	}
	return AAA_OK;
}



void aaa_pkt_dump(const char *buf, uint32_t len, const int opt, const uint8_t proto, const char *caller)
{
	uint32_t i = 0, taillen;
	int    flag = 0;
	char const  *optstr = NULL;
	char const *pktdata;
	char   string[64];
	char   tmp[6];

	struct sockaddr_in dst;
	unsigned int dst_addr = 0;

	radius_auth_server_get(&dst);
	dst_addr = ntohl(dst.sin_addr.s_addr);
	
	if(NULL == buf || NULL == caller)
		return;
	if(len > AAA_PKT_DBG_LEN)
		len = AAA_PKT_DBG_LEN;
	
	if(AAA_PKT_SEND == opt && proto < AAA_PROTO_MAX)
	{
		optstr = "send";
		if((AAA_RADIUS == proto && aaa_pkt_dbg.radius_send)
			|| (AAA_TAC_PLUS == proto && aaa_pkt_dbg.tac_plus_send))
			flag = 1;
	}
	else if(AAA_PKT_RECV == opt && proto < AAA_PROTO_MAX)
	{
		optstr = "recv";
		if((AAA_RADIUS == proto && aaa_pkt_dbg.radius_recv)
			|| (AAA_TAC_PLUS == proto && aaa_pkt_dbg.tac_plus_recv))
			flag = 1;
	}
	else
	{
		zlog_debug(AAA_DBG_COMMON, "%s call %s opt:%d, proto:%d error\n", caller, __FUNCTION__, opt, proto);
		return;
	}
	
	if(0 == flag) return;

	zlog_debug(AAA_DBG_COMMON, "-----aaa %s %s packet begin: ", optstr, protostr[proto]);

	zlog_debug(AAA_DBG_COMMON, "Dst IP   : %d:%d:%d:%d\n", (dst_addr >> 24) & 0xff,\
										   (dst_addr >> 16) & 0xff,\
										   (dst_addr >> 8)  & 0xff,\
										   dst_addr         & 0xff);

	zlog_debug(AAA_DBG_COMMON, "Dst Port : %d\n", ntohs(dst.sin_port));
	
	for(i = 0; (i+16) <= len; i += 16)
    {
		zlog_debug(AAA_DBG_COMMON, "%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
			buf[i], buf[i+1], buf[i+2], buf[i+3], buf[i+4], buf[i+5],
			buf[i+6], buf[i+7], buf[i+8], buf[i+9], buf[i+10], buf[i+11],
			buf[i+12], buf[i+13], buf[i+14], buf[i+15]);
    }
	taillen = len % 16;
	if(taillen != 0)
	{
		pktdata = len > 16 ? (buf + i) : buf;
		memset(string, 0, sizeof(string));
		memset(tmp, 0, sizeof(tmp));
		for(i = 0; i < taillen; i++)
		{
			if(i == 8)
				sprintf(tmp, " %02x ", pktdata[i]);
			else if(i == (taillen -1))
				sprintf(tmp, "%02x", pktdata[i]);
			else
				sprintf(tmp, "%02x ", pktdata[i]);
			strcat(string, tmp);
		}
		zlog_debug(AAA_DBG_COMMON, "%s", string);
	}
	
    zlog_debug(AAA_DBG_COMMON, "aaa packet data end.-----------------------------\n");	
}







