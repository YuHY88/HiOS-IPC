#include <lib/zebra.h>
#include <lib/vty.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/thread.h>
#include <lib/timer.h>
#include <lib/module_id.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/checksum.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/command.h>
#include <lib/mpls_common.h>
#include <lib/ether.h>
#include <lib/prefix.h>
#include <ftm/pkt_icmp.h>
#include <ftm/pkt_mpls.h>
#include "mpls.h"
#include "mpls_ping.h"
#include "mpls_main.h"
#include "lspm.h"
#include "pw.h"
#include "ldp/ldp_packet.h"



struct mpls_ping_global gmpls_ping;  /* 全局数据结构*/

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置ping 报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_ping_set_echohdr(struct mpls_echohdr *pecho, enum PING_ECHO_E type)
{
	struct timeval time;
	static uint32_t seqnum = 0;

	time_get_time (TIME_CLK_REALTIME, &time); 
	seqnum++;

    /* 填写 mpls echo 报文内容 */
	pecho->ver = htons(1);
	pecho->flag = 0;
	pecho->msgType = type;
	pecho->replyMode = 2;       /* Reply via an IPv4/IPv6 UDP packet */
	pecho->retCode = 0;
	pecho->retSubcode = 0;
	pecho->sendHandle = seqnum; /*该字段用于request端查找与reply消息匹配的request源。*/
	pecho->seqNum = htonl(seqnum);
	pecho->sendSec.tv_sec = htonl(time.tv_sec - JAN_1970);
	pecho->sendSec.tv_usec = htonl(time.tv_usec/1000);
	pecho->rcvSec.tv_sec = 0;
	pecho->rcvSec.tv_usec = 0;
	
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置ping lsp报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_ping_set_lsp_echo(struct mpls_lsp_ping_echo *pecho, enum PING_ECHO_E type)
{
	mpls_ping_set_echohdr(&pecho->echohdr, type);
	pecho->target_tlv.type = htons(PING_TVL_TRAGET);
	pecho->target_tlv.len  = htons(12);
	pecho->target_tlv.sub_type = htons(1);
	pecho->target_tlv.sub_len = htons(5);
	pecho->target_tlv.ipv4_prefix = 0;
	pecho->target_tlv.prefix_len = 32;
	memset(pecho->target_tlv.resvd, 0, sizeof(pecho->target_tlv.resvd));

	pecho->pad_tlv.type = htons(PING_TVL_PAD);
	pecho->pad_tlv.len = htons(48);
	pecho->pad_tlv.pad_action = 2;
	memset(pecho->pad_tlv.pading, 0, sizeof(pecho->pad_tlv.pading));
	memcpy(pecho->pad_tlv.pading, "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTU", 47);
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置tracert lsp报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_tracert_set_lsp_echo(struct mpls_lsp_tracert_echo *pecho, enum PING_ECHO_E type)
{
	mpls_ping_set_echohdr(&pecho->echohdr, type);
	pecho->target_tlv.type = htons(PING_TVL_TRAGET);
	pecho->target_tlv.len  = htons(12);
	pecho->target_tlv.sub_type = htons(1);
	pecho->target_tlv.sub_len = htons(5);
	pecho->target_tlv.ipv4_prefix = 0;
	pecho->target_tlv.prefix_len = 32;
	memset(pecho->target_tlv.resvd, 0, sizeof(pecho->target_tlv.resvd));

	pecho->downstearm_tlv.type = htons(PING_TVL_DOWNSTREAM);
	pecho->downstearm_tlv.len = htons(20);
	pecho->downstearm_tlv.mtu = htons(1500);
	pecho->downstearm_tlv.addr_type = 1;
	pecho->downstearm_tlv.resvd = 0;
	pecho->downstearm_tlv.ds_ip = 0;
	pecho->downstearm_tlv.ds_inter_addr = 0;
	pecho->downstearm_tlv.muti_path_type = 0;
	pecho->downstearm_tlv.depth_limit = 0;
	pecho->downstearm_tlv.mutipath_len = 0;
	pecho->downstearm_tlv.label1 = 0;
	pecho->downstearm_tlv.label0 = 0;
	pecho->downstearm_tlv.exp = 0;
	pecho->downstearm_tlv.bos = 1;
	pecho->downstearm_tlv.protocal = 0;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置ping pw报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_ping_set_pw_echo(struct mpls_pw_ping_echo *pecho, enum PING_ECHO_E type)
{
	mpls_ping_set_echohdr(&pecho->echohdr, type);
	pecho->target_tlv.type = htons(PING_TVL_TRAGET);
	pecho->target_tlv.len  = htons(20);
	pecho->target_tlv.sub_type = htons(10);
	pecho->target_tlv.sub_len = htons(14);
	pecho->target_tlv.send_addr = 0;
	pecho->target_tlv.remote_addr = 0;
	pecho->target_tlv.pw_id = 0;	
	pecho->target_tlv.pw_type = 0;
	pecho->target_tlv.pad = 0;

	pecho->pad_tlv.type = htons(PING_TVL_PAD);
	pecho->pad_tlv.len = htons(40);
	pecho->pad_tlv.pad_action = 2;
	memset(pecho->pad_tlv.pading, 0, sizeof(pecho->pad_tlv.pading));
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置tracert  pw 报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/

void mpls_tracert_set_pw_echo(struct mpls_pw_tracert_echo *pecho, enum PING_ECHO_E type)
{
	mpls_ping_set_echohdr(&pecho->echohdr, type);
	pecho->target_tlv.type = htons(PING_TVL_TRAGET);
	pecho->target_tlv.len  = htons(20);
	pecho->target_tlv.sub_type = htons(10);
	pecho->target_tlv.sub_len = htons(14);
	pecho->target_tlv.send_addr = 0;
	pecho->target_tlv.remote_addr = 0;
	pecho->target_tlv.pw_id = 0;	
	pecho->target_tlv.pw_type = 0;
	pecho->target_tlv.pad = 0;

	pecho->downstearm_tlv.type = htons(PING_TVL_DOWNSTREAM);
	pecho->downstearm_tlv.len = htons(24);
	pecho->downstearm_tlv.mtu = htons(1500);
	pecho->downstearm_tlv.addr_type = 1;
	pecho->downstearm_tlv.resvd = 0;
	pecho->downstearm_tlv.ds_ip = 0;
	pecho->downstearm_tlv.ds_inter_addr = 0;
	pecho->downstearm_tlv.muti_path_type = 0;
	pecho->downstearm_tlv.depth_limit = 0;
	pecho->downstearm_tlv.mutipath_len = 0;
	pecho->downstearm_tlv.label1 = 0;
	pecho->downstearm_tlv.label11 = 0;
	pecho->downstearm_tlv.exp1 = 0;
	pecho->downstearm_tlv.bos1 = 0;
	pecho->downstearm_tlv.protocal1 = 0;
	
	pecho->downstearm_tlv.label2 = 0;
	pecho->downstearm_tlv.label22 = 0;
	pecho->downstearm_tlv.exp2 = 0;
	pecho->downstearm_tlv.bos2 = 1;
	pecho->downstearm_tlv.protocal2 = 0;

}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置tracert pw gre报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/

void mpls_tracert_set_pw_echo_gre(struct mpls_pw_tracert_echo_gre *pecho, enum PING_ECHO_E type)
{
	mpls_ping_set_echohdr(&pecho->echohdr, type);
	pecho->target_tlv.type = htons(PING_TVL_TRAGET);
	pecho->target_tlv.len  = htons(20);
	pecho->target_tlv.sub_type = htons(10);
	pecho->target_tlv.sub_len = htons(14);
	pecho->target_tlv.send_addr = 0;
	pecho->target_tlv.remote_addr = 0;
	pecho->target_tlv.pw_id = 0;	
	pecho->target_tlv.pw_type = 0;
	pecho->target_tlv.pad = 0;

	pecho->gre_downstream_tlv.type = htons(PING_TVL_DOWNSTREAM);
	pecho->gre_downstream_tlv.len = htons(20);
	pecho->gre_downstream_tlv.mtu = htons(1500);
	pecho->gre_downstream_tlv.addr_type = 1;
	pecho->gre_downstream_tlv.resvd = 0;
	pecho->gre_downstream_tlv.ds_ip = 0;
	pecho->gre_downstream_tlv.ds_inter_addr = 0;
	pecho->gre_downstream_tlv.muti_path_type = 0;
	pecho->gre_downstream_tlv.depth_limit = 0;
	pecho->gre_downstream_tlv.mutipath_len = 0;
	
	pecho->gre_downstream_tlv.label1 = 0;
	pecho->gre_downstream_tlv.label0 = 0;
	pecho->gre_downstream_tlv.exp = 0;
	pecho->gre_downstream_tlv.bos = 1;
	pecho->gre_downstream_tlv.protocal = 0;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置ping 报文的iphdr  结构  
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_ping_set_iphdr(struct ping_ip_h *piph, int data_len)
{
	/* 填写 ip 报文头 */
	piph->ip_h.tot_len	= htons(data_len + IP_HEADER_SIZE + 4);
	piph->ip_h.ihl		= 6;
	piph->ip_h.version 	= 4;
	piph->ip_h.tos		= 0;
	piph->ip_h.ttl		= 1;
    piph->ip_h.id      	= 0;
	piph->ip_h.frag_off	= htons(0x4000);
	piph->ip_h.protocol  = IP_P_UDP;
	piph->ip_h.saddr 	= htonl(gmpls.lsr_id);
	piph->ip_h.daddr 	= htonl(ipv4_get_loopback());
	piph->ip_option     = htonl(0x94040000);
	piph->ip_h.check 	= in_checksum((u_int16_t *)piph, IP_HEADER_SIZE + 4);
}

void mpls_ping_set_iphdr_no_opiont(struct iphdr *piph, int data_len, uint32_t dip)
{
	/* 填写 ip 报文头 */
	piph->tot_len	= htons(data_len + IP_HEADER_SIZE);
	piph->ihl		= 5;
	piph->version 	= 4;
	piph->tos		= 0;
	piph->ttl		= 1;
    piph->id      	= 0;
	piph->frag_off	= htons(0x4000);
	piph->protocol  = IP_P_UDP;
	piph->saddr 	= htonl(gmpls.lsr_id);
	piph->daddr 	= htonl(dip);
	piph->check 	= in_checksum((u_int16_t *)piph, IP_HEADER_SIZE);
}


/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:      注册 mpls ping 报文     
Version:          版本信息
History:          无
Date:
************************************************************/
void mpls_ping_pkt_register(void)
{
	union proto_reg proto;
	
	
	/* 注册 lsp ping request 报文, udp 端口 3503 的 mpls 报文*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.if_type = PKT_INIF_TYPE_LSP;	
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/* 注册 lsp ping request 报文, udp 端口 3503 的 mpls 报文*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.if_type = PKT_INIF_TYPE_LSP;	
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	/* 注册 pw ping request 报文, chtype 0x0021, udp 端口 3503 的 mpls 报文*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = 0x0021;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;	
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto); 

	/*pw ping request 报文, 无控制字, udp 端口 3503 的 mpls 报文.SPE*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;	
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    /* 注册 pw ping request 报文, chtype 0x0021, udp 端口 3503 的 mpls 报文 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = 0x0021;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;	
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);
	
	/*pw ping request 报文, 无控制字, udp 端口 3503 的 mpls 报文 .PE*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REQ;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

    /* 注册 pw ping reply 报文, chtype 0x0021, udp 端口 3053 的 mpls 报文 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = 0x0021;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REPLY;
	proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	
	
    /* 注册 pw ping reply 报文, chtype 0x0021, udp 端口 3053 的 mpls 报文 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = 0x0021;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.protocol = IP_P_UDP;
	proto.mplsreg.dport = UDP_PORT_LSPING_REPLY;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	

	/* 注册 lsp ping request 报文, udp 端口 3503 的 ip 报文*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = UDP_PORT_LSPING_REQ;	
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);
	

	/* 注册 lsp ping reply 报文, udp 目的端口 3053 的 ip 报文 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.dport = UDP_PORT_LSPING_REPLY;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

	/* 注册 lsp ping reply 报文, udp 源端口 3503 的 ip 报文 */
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_UDP;
	proto.ipreg.sport = UDP_PORT_LSPING_REQ;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:      检查ping request 是否超时     
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_ping_timer_thread(void *para)
{
	struct ping_mpls_node *ping_mpls_node = NULL;
	struct listnode  *pnode  = NULL;
	struct listnode  *pnextnode = NULL; 
	struct timeval time;
	struct timeval time_tmp;
	struct timeval sendSec_tmp;
	struct ping_config *pingInfo = NULL;	
	struct lsp_ping_pkt *preq = NULL; 
	struct vty * vty = NULL; 
	uint32_t wait_time = 0;

	time_get_time (TIME_CLK_REALTIME, &time); 

	memset(&sendSec_tmp, 0, sizeof(struct timeval));
	time_tmp.tv_sec = time.tv_sec - JAN_1970;/*将当前时间转换为基于1900年的时间，与发送时间一致*/
	time_tmp.tv_usec = time.tv_usec;
	
	/* 找到超时的lsp request 报文，从链表删除*/
	for (ALL_LIST_ELEMENTS(gmpls_ping.preq_list, pnode, pnextnode, ping_mpls_node))
	{
		preq = &(ping_mpls_node->preq);
		pingInfo = &(ping_mpls_node->pping);
		vty = pingInfo->pvty;
		
		if(vty->sig_int_recv == 1)	
		{
			list_delete_node(gmpls_ping.preq_list, pnode);
			
			gmpls_ping.ptimer = 0;
			XFREE(MTYPE_PING, ping_mpls_node);
	
			vtysh_return(vty, CMD_SUCCESS);
			return ERRNO_SUCCESS;
		}
		
		switch ( pingInfo->type )
		   {
			   case PING_TYPE_LSP:
			   case PING_TYPE_TUNNEL:
				    sendSec_tmp.tv_sec = ntohl(preq->echo.lsp_ping_echo.echohdr.sendSec.tv_sec);
				    sendSec_tmp.tv_usec = ntohl(preq->echo.lsp_ping_echo.echohdr.sendSec.tv_usec)*1000;
				    break;
			   case TRACERT_TYPE_LSP:
			   case TRACERT_TYPE_TUNNEL:
					sendSec_tmp.tv_sec = ntohl(preq->echo.lsp_tracert_echo.echohdr.sendSec.tv_sec);
					sendSec_tmp.tv_usec = ntohl(preq->echo.lsp_tracert_echo.echohdr.sendSec.tv_usec)*1000;
				    break;
			   case PING_TYPE_PW:
					sendSec_tmp.tv_sec = ntohl(preq->echo.pw_ping_echo.echohdr.sendSec.tv_sec);
					sendSec_tmp.tv_usec = ntohl(preq->echo.pw_ping_echo.echohdr.sendSec.tv_usec)*1000;
					break;
 			   case TRACERT_TYPE_PW:
 				    sendSec_tmp.tv_sec = ntohl(preq->echo.pw_tracert_echo.echohdr.sendSec.tv_sec);
 				    sendSec_tmp.tv_usec = ntohl(preq->echo.pw_tracert_echo.echohdr.sendSec.tv_usec)*1000;
 				    break;
			   default:
				    break;
		   }
		
		wait_time =  timeval_elapsed (time_tmp, sendSec_tmp);
		if(wait_time/1000 > pingInfo->timeout)
		{
			list_delete_node(gmpls_ping.preq_list, pnode);
			
			gmpls_ping.ptimer = 0;
		
			/* 回应vty 以及下个tracert处理*/
			if(pingInfo->type == TRACERT_TYPE_LSP || pingInfo->type == TRACERT_TYPE_PW || pingInfo->type == TRACERT_TYPE_TUNNEL)
			{
				pingInfo->count = 1;
				pingInfo->ttl_curr ++;	 /*计算下次tracert的ttl值*/
				mpls_ping_send_request(pingInfo);
				vty_out(vty, "MPLS tracert timeout !\r\n");
				
				if(pingInfo->ttl_curr > pingInfo->ttl)
					vtysh_return(vty, CMD_SUCCESS);
			}
			else
			{
				vty_out(vty, "MPLS ping timeout !\r\n");
				vtysh_return(vty, CMD_SUCCESS);
			}
			
			/* 释放报文内存*/
			XFREE(MTYPE_PING, ping_mpls_node);
	
			return ERRNO_SUCCESS;
		}
		else 
		{
		
			if(pingInfo->type == TRACERT_TYPE_LSP || pingInfo->type == TRACERT_TYPE_PW || pingInfo->type == TRACERT_TYPE_TUNNEL)
			{
				if(pingInfo->count < 4)   /*最多重传两次 */
				{
					mpls_ping_send(pingInfo, preq, 4 + IP_HEADER_SIZE + UDP_HEADER_SIZE + pingInfo->data_len);
					
				}
				/* 回应vty */
				vty_out(vty, "%d	  * 	 *		*\r\n", pingInfo->ttl_curr);
				
			}
			else
			{
				vty_out(vty, "MPLS ping waiting ......!\r\n");
			}
		}
	}

	if(gmpls_ping.preq_list->count)
	{
        MPLS_TIMER_ADD(mpls_ping_timer_thread, NULL, 1);
	}
	else
	{
        MPLS_TIMER_DEL(gmpls_ping.ptimer);
	}

	return ERRNO_FAIL;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       收到 lsp echo request 报文，回应 echo reply 报文     
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_lsp_ping_recv_request(void *pkt_cb, enum PKT_TYPE cb_type, char *data, uint32_t lsp_index)
{
	struct ip_control   ipcb;
	struct ip_control   *ipcb_rcv = NULL;
	struct mpls_control *mplscb_rcv = NULL;
	struct lsp_entry    *lsp_entry = NULL;   
	uint16_t target_tlv_len = 0;
	uint16_t target_tlv_sub_type = 0;
	//uint16_t target_tlv_sub_len = 0;
	uint16_t tlv_type = 0;
	struct mpls_lsp_tracert_reply_echo lsp_tracert_reply_echo;

	struct mpls_echohdr      *mpls_echohdr      = NULL;
	//struct ping_target_tlv   *ldp_target_tlv    = NULL;
	//struct tunnel_target_tlv *tunnel_target_tlv = NULL;
	struct tunnel_pad_tlv    *tunnel_pad_tlv    = NULL;
	struct downstream_tlv    *downstream_tlv    = NULL;
	struct ping_pad_tlv      *ping_pad_tlv      = NULL;

	//uint32_t send_addr = 0;
	//uint32_t remote_addr = 0;

	uint16_t packet_len = 0;
	
	int ret = 0;
	struct timeval time;

	memset(&lsp_tracert_reply_echo, 0, sizeof(struct mpls_lsp_tracert_reply_echo));
 	time_get_time (TIME_CLK_REALTIME, &time);
	
	lsp_entry = mpls_lsp_lookup(lsp_index);
	if(NULL == lsp_entry)
	{
		return ERRNO_FAIL;		
	}
	target_tlv_len = ntohs(*((uint16_t*)&data[34]));
	target_tlv_sub_type = ntohs(*((uint16_t*)&data[36]));
	//target_tlv_sub_len = ntohs(*((uint16_t*)&data[38]));

	packet_len = sizeof(struct mpls_echohdr);			
	mpls_echohdr = (struct mpls_echohdr *)data;
	mpls_echohdr->msgType = PING_ECHO_REPLY;
	mpls_echohdr->retSubcode = 1;
	if(lsp_entry->direction == LSP_DIRECTION_TRANSIT)
	{
		mpls_echohdr->retCode = 8;
	}
	else
	{
		mpls_echohdr->retCode = 3;
	}
	mpls_echohdr->rcvSec.tv_sec = htonl(time.tv_sec - JAN_1970); /* 修改接收时间*/
	mpls_echohdr->rcvSec.tv_usec = htonl(time.tv_usec/1000);

	if(target_tlv_len == 12)/*lsp*/	
	{
		tlv_type = ntohs(*((uint16_t*)&data[48]));
		if(target_tlv_sub_type == 1)//ldp-lsp
		{
			//ldp_target_tlv = (struct ping_target_tlv *)(&data[32]);
			packet_len+= sizeof(struct ping_target_tlv);

			if(tlv_type == PING_TVL_DOWNSTREAM && lsp_entry->direction == LSP_DIRECTION_TRANSIT)
			{
				downstream_tlv = (struct downstream_tlv *)(&data[48]);
				packet_len+= sizeof(struct downstream_tlv);
			}
			else if(tlv_type == PING_TVL_PAD)
			{
				ping_pad_tlv = (struct ping_pad_tlv *)(&data[48]);
				if(ping_pad_tlv->pad_action == PAD_ACT_COPY)
				{
					packet_len+= sizeof(struct ping_pad_tlv);
				}
			}
		}
	}
	else if(target_tlv_len == 24)//rsvp-lsp
	{
		tlv_type = ntohs(*((uint16_t*)&data[60]));
		if(target_tlv_sub_type == 3)//rsvp-lsp
		{
			//tunnel_target_tlv = (struct tunnel_target_tlv *)(&data[32]);

			packet_len+= sizeof(struct tunnel_target_tlv);
			
			if(tlv_type == PING_TVL_DOWNSTREAM && lsp_entry->direction == LSP_DIRECTION_TRANSIT)
			{
				downstream_tlv = (struct downstream_tlv *)(&data[60]);
				packet_len+= sizeof(struct downstream_tlv);
			}
			else if(tlv_type == PING_TVL_PAD)
			{
				tunnel_pad_tlv = (struct tunnel_pad_tlv *)(&data[60]);

				if(tunnel_pad_tlv->pad_action == PAD_ACT_COPY)
				{
					packet_len+= sizeof(struct tunnel_pad_tlv);
				}
			}	
		}
	}

	if(downstream_tlv != NULL)
	{
		downstream_tlv->label0 = htons(lsp_entry->outlabel >> 4);
		downstream_tlv->label1 = lsp_entry->outlabel & 0xf;
		if(lsp_entry->nexthop.addr.ipv4 != 0)
		{
			downstream_tlv->ds_ip = htonl(lsp_entry->nexthop.addr.ipv4);
			downstream_tlv->ds_inter_addr = htonl(lsp_entry->nexthop.addr.ipv4);
		}
	}

	/* 使用ip control 发送udp 报文*/	
	memset(&ipcb, 0, sizeof(struct ip_control));
	ipcb.protocol = IP_P_UDP;
	ipcb.dport = UDP_PORT_LSPING_REPLY;
	ipcb.sport = UDP_PORT_LSPING_REQ;
	ipcb.sip = gmpls.lsr_id;
	if(PKT_TYPE_UDP == cb_type)
	{
		ipcb_rcv = (struct ip_control *)pkt_cb;
		if(ipcb_rcv == NULL)
		{
			return ERRNO_FAIL;		
		}
		ipcb.dip = ipcb_rcv->sip;
		ipcb.dport = ipcb_rcv->sport;
	}
	else
	{
		mplscb_rcv = (struct mpls_control *)pkt_cb;
		if(mplscb_rcv == NULL)
		{
			return ERRNO_FAIL;		
		}
		ipcb.dip = mplscb_rcv->sip;
	}
	ipcb.tos = 5;
	ipcb.ttl = 255;
	ipcb.is_changed = 1;
	
	if(target_tlv_len == 12 || target_tlv_len == 24)
	{
		ret = pkt_send(PKT_TYPE_UDP, (void *)&ipcb, data, packet_len);
	}

	if(ret == NET_FAILED)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;		
	}
	
	gmpls_ping.pkt_send++;
	return ERRNO_SUCCESS;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:      收到 pw echo request 报文，回应 echo reply 报文     
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_pw_ping_recv_request(struct ip_control *ipcb, char *data, uint32_t pwindex)
{
	//struct lsp_ping_pkt  pping ;	
	//struct mpls_control  mplscb ;
	struct ip_control    ip_cb ;
	struct timeval time;
	struct l2vc_entry  *l2vc_entry = NULL;
	struct pw_info *pwinfo = NULL;
	uint16_t target_tlv_len = 0;
	uint16_t tlv_type = 0;
	uint8_t pad_action = 0;
	uint32_t remote_addr = 0;
	struct mpls_echohdr *mpls_echohdr = NULL;
	struct pw_target_tlv *pw_target_tlv = NULL;
	struct downstream_tlv *gre_downstream_tlv = NULL;
	struct pw_downstream_tlv *tp_downstream_tlv = NULL;
	struct pw_pad_tlv        *pw_pad_tlv = NULL;
	uint16_t packet_len = 0;
	int ret = 0;
	struct ping_config   pping;
	struct lsp_ping_pkt_bak  preq;	
	struct udphdr 		*pudph = NULL;
	struct iphdr 	*piph = NULL;
	
	//memset(&pping, 0, sizeof(struct lsp_ping_pkt));
	//memset(&mplscb, 0, sizeof(struct mpls_control));
	memset(&ip_cb, 0, sizeof(struct ip_control));
	memset(&preq, 0, sizeof(struct lsp_ping_pkt_bak));
	
	time_get_time (TIME_CLK_REALTIME, &time);
	pwinfo = pw_lookup(pwindex);
	if(NULL == pwinfo)
	{
		return ERRNO_FAIL;		
	}
	l2vc_entry = pw_get_l2vc(pwinfo);
	if(NULL == l2vc_entry)
	{
		return ERRNO_FAIL;
	}

	mpls_echohdr = (struct mpls_echohdr *)data;
	
	mpls_echohdr->msgType = PING_ECHO_REPLY;
	mpls_echohdr->retSubcode = 1;
	mpls_echohdr->rcvSec.tv_sec = htonl(time.tv_sec - JAN_1970); /* 修改接收时间*/
	mpls_echohdr->rcvSec.tv_usec = htonl(time.tv_usec/1000);

	packet_len = sizeof(struct mpls_echohdr);

	target_tlv_len = ntohs(*((uint16_t*)&data[34]));
	if(target_tlv_len == 20)/*pw*/	
	{
		tlv_type = ntohs(*((uint16_t*)&data[56]));
		if(ntohs(*((uint16_t*)&data[36])) == 10)
		{
			pw_target_tlv = (struct pw_target_tlv *)(&data[32]);

			remote_addr = htonl(pw_target_tlv->remote_addr);
		#if 1
			if(l2vc_entry->pswitch_pw != NULL)
			{
				if(gmpls.lsr_id != ntohl(pw_target_tlv->remote_addr))
				{
					mpls_echohdr->retCode = 8; /*trasit*/
				}
				else
				{
					mpls_echohdr->retCode = 3;
				}
			}
			else
			{
				
				if(gmpls.lsr_id != ntohl(pw_target_tlv->remote_addr))
				{
					zlog_info("%s[%s]:remote addr is not lsr-id.--line:%d.",
						__FILE__, __FUNCTION__, __LINE__);
				//	return ERRNO_FAIL;
				}
				
				mpls_echohdr->retCode = 3;
			}
		#endif
			
			/* 收到华为的ping包后,不能交换target fec tlv的两个地址*/
			#if 0
			pw_target_tlv->send_addr = htonl(remote_addr);
			pw_target_tlv->remote_addr = htonl(send_addr);
			#endif
		}
		
		packet_len+= sizeof(struct pw_target_tlv);
		
		if(tlv_type == PING_TVL_DOWNSTREAM) /*tracert pw*/
		{
			if (pwinfo->mspw_index != 0)      /*trasit pw*/
			{
				if(ntohs(*((uint16_t*)&data[58])) == 24)//double label
				{
					if(mpls_echohdr->retCode == 8)
					{
						tp_downstream_tlv = (struct pw_downstream_tlv *)(&data[56]);
						packet_len+= sizeof(struct pw_downstream_tlv);

						if(l2vc_entry->pswitch_pw != NULL)
						{
							tp_downstream_tlv->label1 = htons(l2vc_entry->pswitch_pw->outlabel >> 4);
							tp_downstream_tlv->label11 = l2vc_entry->pswitch_pw->outlabel & 0xf;
							tp_downstream_tlv->ds_ip  = htonl(l2vc_entry->pswitch_pw->peerip.addr.ipv4);
							tp_downstream_tlv->ds_inter_addr = htonl(l2vc_entry->pswitch_pw->peerip.addr.ipv4);
						}
					}
				}
				else//single label
				{
					if(mpls_echohdr->retCode == 8)
					{
						gre_downstream_tlv = (struct downstream_tlv *)(&data[56]);
						packet_len+= sizeof(struct downstream_tlv);

						if(l2vc_entry->pswitch_pw != NULL)
						{
							gre_downstream_tlv->label1 = htons(l2vc_entry->pswitch_pw->outlabel >> 4);
							gre_downstream_tlv->label0 = l2vc_entry->pswitch_pw->outlabel & 0xf;
							gre_downstream_tlv->ds_ip  = htonl(l2vc_entry->pswitch_pw->peerip.addr.ipv4);
							gre_downstream_tlv->ds_inter_addr = htonl(l2vc_entry->pswitch_pw->peerip.addr.ipv4);
						}
					}
				}
			}
			else/*egress:中兴对与target fec tlv中的remote addr不做判断*/
			{
				
				if(gmpls.lsr_id != remote_addr)
				{
					zlog_err("%s[%s]:ping rcv:remote_addr:%d.--line:%d", 
						__FILE__, __FUNCTION__, ntohl(pw_target_tlv->remote_addr), __LINE__);
				//	return ERRNO_FAIL;	/*不回包*/
				}
				
			}
		}
		else if(tlv_type == PING_TVL_PAD)   /*ping pw*/
		{
			pw_pad_tlv = (struct pw_pad_tlv *)(&data[56]);
			pad_action = pw_pad_tlv->pad_action;
			if(pad_action == PAD_ACT_COPY)
			{
				packet_len+= (htons(pw_pad_tlv->len) + 4);
			}
				
		}
	}
	
	
    /* 修改 IP
	mpls_ping_set_iphdr(&(pping.iph), data_len + UDP_HEADER_SIZE);
	pping.iph.ip_h.daddr = htonl(ipcb->sip);
	pping.iph.ip_h.check = 0;
	pping.iph.ip_h.check = in_checksum((u_int16_t *)&(pping.iph), IP_HEADER_SIZE+4);

	 修改udp 端口
	pping.udph.sport = htons(UDP_PORT_LSPING_REQ);
	pping.udph.dport = htons(ipcb->sport);
	pping.udph.len = htons(UDP_HEADER_SIZE + data_len);
	pping.udph.checksum = 0;
	
	 使用mpls control 		
	memset(&mplscb, 0, sizeof(struct mpls_control));
	mplscb.ifindex = pwindex;
	mplscb.if_type = PKT_INIF_TYPE_PW;
	mplscb.chtype = MPLS_CTLWORD_CHTYPE_IPV4;
	mplscb.exp = 5;
	mplscb.ttl = 255;
	mplscb.is_changed = 1;
	*/
	
/* 使用ip control 发送udp 报文*/	
	memset(&ip_cb, 0, sizeof(struct ip_control));
	ip_cb.protocol = IP_P_UDP;
	ip_cb.sip = gmpls.lsr_id;
	ip_cb.dip = ipcb->sip;
	ip_cb.sport = UDP_PORT_LSPING_REQ;
	ip_cb.dport = ipcb->sport;
	ip_cb.tos = 5;
	ip_cb.ttl = 255;
	ip_cb.is_changed = 1;

	if(tlv_type == PING_TVL_PAD && mpls_echohdr->replyMode == 4)
	{
		pping.type = PING_TYPE_PW;
		pping.pw_index = pwindex;
		pping.data_len = sizeof(struct mpls_pw_ping_echo);		

		//mpls echo replay data
		memcpy(&preq.echo.pw_ping_echo,data,packet_len);

		//ip header
		piph = &(preq.ip_h);
		mpls_ping_set_iphdr_no_opiont(piph, pping.data_len + UDP_HEADER_SIZE, ipcb->sip);
		
		//udp header
		pudph = &(preq.udph);
		pudph->sport = htons(UDP_PORT_LSPING_REQ);
		pudph->dport = htons(ipcb->sport);
		pudph->len = htons(UDP_HEADER_SIZE + pping.data_len);
		pudph->checksum = 0;	

		ret = mpls_ping_send(&pping, &preq, IP_HEADER_SIZE + UDP_HEADER_SIZE + pping.data_len);
		
	}
	else
	{
		ret = pkt_send(PKT_TYPE_UDP, (void *)&ip_cb, data, packet_len);
	}


	if(ret == NET_FAILED)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;		
	}
	
	gmpls_ping.pkt_send++;
	return ERRNO_SUCCESS;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置控制块内容并发送报文
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_ping_send(struct ping_config *pping, void *preq, int len)
{
	struct mpls_control  mplscb;	
	struct raw_control   rawcb;
	struct pw_info *pwinfo = NULL;
	int ret = 0;
	
	if(pping->type == PING_TYPE_PW || pping->type == TRACERT_TYPE_PW)
	{
	    /* 使用mpls control */
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.ifindex = pping->pw_index;
		mplscb.if_type = PKT_INIF_TYPE_PW;
		pwinfo = pw_lookup(pping->pw_index);
		if(NULL == pwinfo)
		{
			return ERRNO_FAIL;
		}
		if(pwinfo->ctrlword_flag == 1)
		{
			mplscb.chtype = MPLS_CTLWORD_CHTYPE_IPV4;	
		}
		else if(pwinfo->ctrlword_flag == 0)
		{
			mplscb.chtype = 0;	
		}
		
		if(pping->type == PING_TYPE_PW)
		{
			mplscb.ttl = 255;		
		}
		else
		{
			mplscb.ttl = pping->ttl_curr;
		}
		mplscb.exp = 5;
		mplscb.is_changed = 1;
		ret = pkt_send(PKT_TYPE_MPLS, (void *)&mplscb, preq, len);		
	}
	else if(pping->type == PING_TYPE_LSP || pping->type == TRACERT_TYPE_LSP)
	{
		/* 使用mpls control */
		memset(&mplscb, 0, sizeof(struct mpls_control));
		mplscb.ifindex = pping->lsp_index;
		mplscb.if_type = PKT_INIF_TYPE_LSP;
		mplscb.chtype = 0;
		if(pping->type == PING_TYPE_LSP)
		{
			mplscb.ttl = 255;		
		}
		else
		{
			mplscb.ttl = pping->ttl_curr;
		}
		mplscb.exp = 0;
		mplscb.is_changed = 1;		
		ret = pkt_send(PKT_TYPE_MPLS, (void *)&mplscb, preq, len);		
	}
	else if(pping->type == PING_TYPE_TUNNEL || pping->type == TRACERT_TYPE_TUNNEL)
	{
		/* 使用raw control */	
		memset(&rawcb, 0, sizeof(struct raw_control));
		rawcb.out_ifindex = pping->ifindex;
		rawcb.priority = 5;	
		if(pping->type == PING_TYPE_TUNNEL)
		{
			rawcb.ttl = 255;		
		}
		else
		{
			rawcb.ttl = pping->ttl_curr;
		}	
		
		ret = pkt_send(PKT_TYPE_RAW, (void *)&rawcb, preq, len);		
	}
	else
		ret = ERRNO_FAIL;
	
	if(ret != ERRNO_FAIL)
	{
		pping->count++;           /*当前报文发送次数*/
		gmpls_ping.pkt_send++;     /*mpls进程总共发送ping包的次数*/
	}
	
	return ret;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       发送request 报文     
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_ping_send_request(struct ping_config *pping)
{
	struct lsp_ping_pkt  *preq = NULL;	
	struct udphdr *pudph = NULL;
	struct pseudo_hdr pudph_pse = {0};
	struct ping_ip_h *piph = NULL;
	struct ping_mpls_node *ping_mpls_node = NULL;
	struct pw_info *pwinfo = NULL;
	int data_len = 0;
	char data[150] = {0};
	int ret = 0;

	if(pping->type == TRACERT_TYPE_LSP || pping->type == TRACERT_TYPE_PW || pping->type == TRACERT_TYPE_TUNNEL)
	{
		if(pping->ttl_curr > pping->ttl)
		{
			return ERRNO_FAIL;					
		}
	}
	

	/* 分配内存*/
	ping_mpls_node = XCALLOC(MTYPE_PING, sizeof(struct ping_mpls_node));
	if(ping_mpls_node == NULL)
	{
		zlog_err("%s[%s]:malloc err.--line:%d", __FILE__, __FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}
	memset(ping_mpls_node, 0, sizeof(struct ping_mpls_node));
	
	preq = &(ping_mpls_node->preq);
	memcpy(&ping_mpls_node->pping, pping, sizeof(struct ping_config));

	if(pping->type == PING_TYPE_PW || pping->type == TRACERT_TYPE_PW)
	{
		pwinfo = pw_lookup(pping->pw_index);
		if(NULL == pwinfo)
		{
			return ERRNO_FAIL;		
		}

	
	    if (pwinfo->ac_type == AC_TYPE_ETH)
	    {
	        if (pwinfo->tag_flag)
	        {
	            pping->pw_type = PW_VC_TYPE_VLAN;
	        }
	        else
	        {
	            pping->pw_type = PW_VC_TYPE_ETH;
	        }
        }
        else if (pwinfo->ac_type == AC_TYPE_TDM)
       {
           pping->pw_type = PW_VC_TYPE_TDM;
       } 
       else
       {
          /* 浜ゆ崲PW VCTYEP */
           pping->pw_type = PW_VC_TYPE_VLAN;
        }
	}
	switch ( pping->type )
	   {
		   case PING_TYPE_LSP:
		   case PING_TYPE_TUNNEL:
				data_len = sizeof(struct mpls_lsp_ping_echo);
				mpls_ping_set_lsp_echo(&preq->echo.lsp_ping_echo, PING_ECHO_REQUEST);
				preq->echo.lsp_ping_echo.target_tlv.ipv4_prefix = htonl(pping->ipv4_prefix);
				preq->echo.lsp_ping_echo.target_tlv.prefix_len = 32;
				preq->echo.lsp_ping_echo.echohdr.sendHandle = htonl((uint32_t)ping_mpls_node->pping.pvty); /* 设置发送的VTY 终端*/
				break;
		   case TRACERT_TYPE_LSP:
		   case TRACERT_TYPE_TUNNEL:
				if(pping->tlv_type == MPLS_PING_TLV_NO_PAD)
				{
					data_len = sizeof(struct mpls_echohdr) + sizeof(struct ping_target_tlv);
				}
				else
				{
			    	data_len = sizeof(struct mpls_lsp_tracert_echo);
				}
			    mpls_tracert_set_lsp_echo(&preq->echo.lsp_tracert_echo, PING_ECHO_REQUEST);				
				preq->echo.lsp_tracert_echo.target_tlv.ipv4_prefix = htonl(pping->ipv4_prefix);
				preq->echo.lsp_tracert_echo.target_tlv.prefix_len = 32;
			//	preq->echo.lsp_tracert_echo.downstearm_tlv.ds_ip = htonl(pping->nexthop);
				preq->echo.lsp_tracert_echo.downstearm_tlv.ds_ip = htonl(pping->ipv4_prefix);
				preq->echo.lsp_tracert_echo.downstearm_tlv.ds_inter_addr = htonl(pping->nexthop);
				preq->echo.lsp_tracert_echo.downstearm_tlv.label1 = htons(pping->lsp_label >> 4);
				preq->echo.lsp_tracert_echo.downstearm_tlv.label0 = pping->lsp_label & 0xf;
				preq->echo.lsp_tracert_echo.echohdr.sendHandle = htonl((uint32_t)ping_mpls_node->pping.pvty); /* 设置发送的VTY 终端*/
			    break;
			case PING_TYPE_PW:
				data_len = sizeof(struct mpls_pw_ping_echo);
				mpls_ping_set_pw_echo(&preq->echo.pw_ping_echo, PING_ECHO_REQUEST);
				preq->echo.pw_ping_echo.target_tlv.send_addr = htonl(gmpls.lsr_id);
				preq->echo.pw_ping_echo.target_tlv.remote_addr = htonl(pping->ipv4_prefix);
				preq->echo.pw_ping_echo.target_tlv.pw_id = htonl(pping->pw_id);
				preq->echo.pw_ping_echo.target_tlv.pw_type = htons(pping->pw_type);
				preq->echo.pw_ping_echo.echohdr.sendHandle = htonl((uint32_t)ping_mpls_node->pping.pvty); /* 设置发送的VTY 终端*/
				preq->echo.pw_ping_echo.echohdr.replyMode  = pping->reply_mode;
				break;
			case TRACERT_TYPE_PW:
				if(pping->gre_pw == 0)
				{
					data_len = sizeof(struct mpls_pw_tracert_echo);
					mpls_tracert_set_pw_echo(&preq->echo.pw_tracert_echo, PING_ECHO_REQUEST);
					preq->echo.pw_tracert_echo.target_tlv.send_addr = htonl(gmpls.lsr_id);
					preq->echo.pw_tracert_echo.target_tlv.remote_addr = htonl(pping->ipv4_prefix);
					preq->echo.pw_tracert_echo.target_tlv.pw_id = htonl(pping->pw_id);
					preq->echo.pw_tracert_echo.target_tlv.pw_type = htons(pping->pw_type);
					preq->echo.pw_tracert_echo.downstearm_tlv.ds_ip = htonl(pping->nexthop);
					preq->echo.pw_tracert_echo.downstearm_tlv.ds_inter_addr = htonl(pping->nexthop);
					preq->echo.pw_tracert_echo.downstearm_tlv.label1 = htons(pping->pw_label >> 4);
					preq->echo.pw_tracert_echo.downstearm_tlv.label11 = pping->pw_label & 0xf;
					preq->echo.pw_tracert_echo.downstearm_tlv.label2 = htons(pping->lsp_label >> 4);
					preq->echo.pw_tracert_echo.downstearm_tlv.label22 = pping->lsp_label & 0xf;
					preq->echo.pw_tracert_echo.echohdr.sendHandle = htonl((uint32_t)ping_mpls_node->pping.pvty); /* 设置发送的VTY 终端*/
				}
				else if(pping->gre_pw == 1)
				{
					data_len = sizeof(struct mpls_pw_tracert_echo_gre);
					mpls_tracert_set_pw_echo_gre(&preq->echo.pw_tracert_gre_echo, PING_ECHO_REQUEST);
					preq->echo.pw_tracert_gre_echo.target_tlv.send_addr = htonl(gmpls.lsr_id);
					preq->echo.pw_tracert_gre_echo.target_tlv.remote_addr = htonl(pping->ipv4_prefix);
					preq->echo.pw_tracert_gre_echo.target_tlv.pw_id = htonl(pping->pw_id);
					preq->echo.pw_tracert_gre_echo.target_tlv.pw_type = htons(pping->pw_type);
					preq->echo.pw_tracert_gre_echo.gre_downstream_tlv.ds_ip = htonl(pping->nexthop);
					preq->echo.pw_tracert_gre_echo.gre_downstream_tlv.ds_inter_addr = htonl(pping->nexthop);
					preq->echo.pw_tracert_gre_echo.gre_downstream_tlv.label1 = htons(pping->pw_label >> 4);
					preq->echo.pw_tracert_gre_echo.gre_downstream_tlv.label0 = pping->pw_label & 0xf;
					preq->echo.pw_tracert_gre_echo.echohdr.sendHandle = htonl((uint32_t)ping_mpls_node->pping.pvty); /* 设置发送的VTY 终端*/
				}
				if(pping->tlv_type == MPLS_PING_TLV_NO_PAD)
				{
					data_len = sizeof(struct mpls_echohdr) + sizeof(struct pw_target_tlv);
				}
				break;
		   default:
			    break;
	   }
	
	ping_mpls_node->pping.data_len = data_len;

	/* 填写 ip 报文头 */
	piph = &(preq->iph);
	mpls_ping_set_iphdr(piph, data_len + UDP_HEADER_SIZE);
	
	/* 填写 udp 报文头 */
	pudph = &(preq->udph);
	pudph->sport = htons(UDP_PORT_LSPING_REPLY);
	pudph->dport = htons(UDP_PORT_LSPING_REQ);
	
	pudph->len = htons(UDP_HEADER_SIZE + data_len);
	pudph->checksum = 0;
	if( pping->type != PING_TYPE_PW &&  pping->type != TRACERT_TYPE_PW )
	{
		pudph_pse.saddr = piph->ip_h.saddr;
		pudph_pse.daddr = piph->ip_h.daddr;
		pudph_pse.placeholder = 0x00;
		pudph_pse.protocol = 17;
		pudph_pse.length = pudph->len;

		
		/*** set udp checksum ***/
		memcpy(data, (unsigned char *)&pudph_pse, sizeof(struct pseudo_hdr));
		memcpy(data + sizeof(struct pseudo_hdr), (unsigned char *)pudph, UDP_HEADER_SIZE);
		memcpy(data + sizeof(struct pseudo_hdr) + UDP_HEADER_SIZE, &(preq->echo), data_len);
		pudph->checksum = in_checksum((uint16_t *)data, sizeof(struct pseudo_hdr)+ UDP_HEADER_SIZE + data_len);
	}

	ret = mpls_ping_send(&ping_mpls_node->pping, preq, 4 + IP_HEADER_SIZE + UDP_HEADER_SIZE + ping_mpls_node->pping.data_len);
	
	if(ret == ERRNO_FAIL)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;		
	}

	gmpls_ping.pkt_send++;

    /* 加到发送链表，起定时器等待应答 */
	listnode_add(gmpls_ping.preq_list, ping_mpls_node);
	if(gmpls_ping.ptimer == 0)
	{
		gmpls_ping.ptimer = MPLS_TIMER_ADD((void *)mpls_ping_timer_thread, NULL, 1);
	}
	
	return ERRNO_SUCCESS;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       设置ping 报文的echo 结构     
Version:          版本信息
History:          无
Date:
************************************************************/

/* 收到reply 报文, 要找到对应的request 报文 */
int mpls_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr *echo, void *data)
{
	struct ping_mpls_node *ping_mpls_node = NULL;
	struct lsp_ping_pkt *preq = NULL;
	struct ping_config *pingInfo = NULL;	
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
	uint16_t tlv_type = 0;
	char addr[20];
	struct timeval time;
	struct timeval time_tmp;
	struct timeval sendSec_tmp;
	struct vty * vty = NULL;
	uint32_t sip = 0;
//  uint8_t ttl = 0;
//	uint32_t ds_ip = 0;
//	uint32_t ds_if = 0;
	uint32_t lsp_label = 0;
	uint32_t pw_label = 0;
	
	if(NULL == ipcb_rcv || NULL == echo || NULL == data)
	{
		zlog_err("%s[%s]:mpls ping recv err.--line:%d", __FILE__, __FUNCTION__, __LINE__);
		return ERRNO_FAIL;
	}

	sip = ipcb_rcv->sip;
	//ttl = ipcb_rcv->ttl;

	inet_ipv4tostr(sip, addr);
	time_get_time (TIME_CLK_REALTIME, &time); 
	time_tmp.tv_sec = time.tv_sec - JAN_1970;/*将当前时间转换为基于1900年的时间，与发送时间一致*/
	time_tmp.tv_usec = time.tv_usec;
	
	/* 找到发送的request 报文，从链表删除*/
	for (ALL_LIST_ELEMENTS(gmpls_ping.preq_list, pnode, pnextnode, ping_mpls_node))
	{
		preq = &(ping_mpls_node->preq);
		pingInfo = &(ping_mpls_node->pping);
		
		vty = pingInfo->pvty;
		
		if(preq->echo.lsp_ping_echo.echohdr.sendHandle == echo->sendHandle)
		{
			sendSec_tmp.tv_sec = ntohl(preq->echo.lsp_ping_echo.echohdr.sendSec.tv_sec);
			sendSec_tmp.tv_usec = ntohl(preq->echo.lsp_ping_echo.echohdr.sendSec.tv_usec)*1000;
			
			list_delete_node(gmpls_ping.preq_list, pnode);
			if(pingInfo->type == TRACERT_TYPE_LSP || pingInfo->type == TRACERT_TYPE_PW || pingInfo->type == TRACERT_TYPE_TUNNEL)
			{		
				if(echo->retCode == 3)
				{
					vty_out(vty, "%-7d%-17s%-18s%ldms!\r\n", pingInfo->ttl_curr,  addr, "egress", timeval_elapsed (time_tmp, sendSec_tmp)/1000);	
					vtysh_return(vty, CMD_SUCCESS);
				}
				else
				{
					if(echo->retCode == 8)//transit
					{
						if(pingInfo->type == TRACERT_TYPE_LSP || pingInfo->type == TRACERT_TYPE_TUNNEL)
						{
							//tlv_type = ntohs(*((uint16_t *)&data[48]));
							tlv_type = ntohs(*((uint16_t *)((uint8_t *)data + 48)));
							if(tlv_type == PING_TVL_DOWNSTREAM) /*tracert lsp*/
							{
								/* downstream mapping ip */
								//pingInfo->nexthop = ntohl(*((uint32_t *)&data[56]));
								pingInfo->nexthop = ntohl(*((uint32_t *)((uint8_t *)data + 56)));
								/* downstream mapping label */
								//lsp_label = ntohs(*((uint16_t *)&data[68]));
								lsp_label = ntohs(*((uint16_t *)((uint8_t *)data + 68)));
								//lsp_label = (lsp_label << 4) + ((*((uint16_t *)&data[70])) >> 4);
								lsp_label = (lsp_label << 4) + ((*((uint16_t *)((uint8_t *)data + 70))) >> 4);
								pingInfo->lsp_label = lsp_label;
							}
						}
						else if(pingInfo->type == TRACERT_TYPE_PW)
						{
							//tlv_type = ntohs(*((uint16_t*)&data[56]));
							tlv_type = ntohs(*((uint16_t*)((uint8_t *)data + 56)));
							if(tlv_type == PING_TVL_DOWNSTREAM)
							{
								/* downstream mapping ip */
								//pingInfo->nexthop = ntohl(*((uint32_t *)&data[64]));
								pingInfo->nexthop = ntohl(*((uint32_t *)((uint8_t *)data + 64)));

								/* downstream mapping label */
								//pw_label = ntohs(*((uint16_t *)&data[76]));
								pw_label = ntohs(*((uint16_t *)((uint8_t *)data + 76)));
								//pw_label = (pw_label << 4) + ((*((uint16_t *)&data[78])) >> 4);
								pw_label = (pw_label << 4) + ((*((uint16_t *)((uint8_t *)data + 78))) >> 4);
								pingInfo->pw_label = pw_label;
								//if(ntohs(*((uint16_t*)&data[58])) == 20)//single
								if(ntohs(*((uint16_t*)((uint8_t *)data + 58)) == 20))//single
								{
									pingInfo->gre_pw = 1;
								}
								else if(ntohs(*((uint16_t*)((uint8_t *)data + 58)) == 20))//double
								{
									pingInfo->gre_pw = 0;
								}
							}
						}
					
						vty_out(vty, "%-7d%-17s%-18s%ldms!\r\n",
							pingInfo->ttl_curr,  addr, "trasit", timeval_elapsed (time_tmp, sendSec_tmp)/1000);
						zlog_info("%s[%s]:return code:%d.--line:%d",
							__FILE__, __FUNCTION__, echo->retCode, __LINE__);
					}
					else
					{
						pingInfo->tlv_type = MPLS_PING_TLV_NO_PAD;
						
						vty_out(vty, "%-7d%-17s%-18s%ldms!\r\n",
							pingInfo->ttl_curr,  addr, "tlv decode error", timeval_elapsed (time_tmp, sendSec_tmp)/1000);
					}
					
					pingInfo->count = 1;
					pingInfo->ttl_curr ++;	 /*计算下次tracert的ttl值*/
					mpls_ping_send_request(pingInfo);
					if(pingInfo->ttl_curr > pingInfo->ttl)
						vtysh_return(vty, CMD_SUCCESS);
					
				}
			}
			else
			{
				/* 回应vty */
				vty_out(vty, "MPLS ping reply from %s: 40 data bytes ttl=%d time:%ldms!\r\n", addr, pingInfo->ttl, timeval_elapsed (time_tmp, sendSec_tmp)/1000);
				vtysh_return(vty, CMD_SUCCESS);
			}
			/* 释放报文内存*/
			XFREE(MTYPE_PING, ping_mpls_node);


			return ERRNO_SUCCESS;
		}
	}
	
	
	return ERRNO_FAIL;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       收到 lsp echo reply 报文 
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_lsp_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr  *echo, void *data)
{
	mpls_ping_recv_reply(ipcb_rcv, echo, data);
	return 0;
}


/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       收到 pw echo reply 报文   
Version:          版本信息
History:          无
Date:
************************************************************/
int mpls_pw_ping_recv_reply(struct ip_control *ipcb_rcv, struct mpls_echohdr  *echo, void *data)
{
	mpls_ping_recv_reply(ipcb_rcv, echo, data);
	return 0;
}


/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       ping处理ftm发过来的消息    
Version:          版本信息
History:          无
Date:
************************************************************/

int mpls_ping_recv(struct pkt_buffer *pkt)
{
	struct mpls_echohdr *echo_rcv;
	uint8_t  msgType = 5;

	gmpls_ping.pkt_recv++;
		
	echo_rcv = (struct mpls_echohdr *)pkt->data; 	

	if(NULL != echo_rcv)
	{
		msgType = echo_rcv->msgType;
	}
	
	if(msgType == PING_ECHO_REPLY)
	{
		zlog_info("%s[%s]:rcv mpls ping reply.--line:%d.",
			__FILE__, __FUNCTION__, __LINE__);
		if(pkt->inif_type == PKT_INIF_TYPE_IF || pkt->inif_type == PKT_INIF_TYPE_LSP)
		{
			return mpls_lsp_ping_recv_reply((void *)&(pkt->cb), echo_rcv, pkt->data);
		}
		else if(pkt->inif_type == PKT_INIF_TYPE_PW)
		{
			return mpls_pw_ping_recv_reply((void *)&(pkt->cb), echo_rcv, pkt->data);
		}
	}
	
	if(msgType == PING_ECHO_REQUEST)
	{
		zlog_info("%s[%s]:rcv mpls ping request.--line:%d.",
			__FILE__, __FUNCTION__, __LINE__);
		if(pkt->inif_type == PKT_INIF_TYPE_LSP)
		{
			return mpls_lsp_ping_recv_request((void *)&(pkt->cb), pkt->cb_type, pkt->data, pkt->in_ifindex);
		}
		else if(pkt->inif_type == PKT_INIF_TYPE_PW)
		{
			return mpls_pw_ping_recv_request((void *)&(pkt->cb) ,pkt->data, pkt->in_ifindex);
		}
	}
	
	gmpls_ping.pkt_error++;
	return ERRNO_FAIL;
}

/************************************************************
Copyright (C), 1988-1999, Huahuan Co., Ltd.
FileName: mpls_ping.c
Author: 王之云      
Description:       mpls ping 模块初始化    
Version:          版本信息
History:          无
Date:
************************************************************/

void mpls_ping_init(void)
{
	memset(&gmpls_ping, 0, sizeof(struct mpls_ping_global));	
    gmpls_ping.preq_list = list_new();	
	mpls_ping_pkt_register();
}


