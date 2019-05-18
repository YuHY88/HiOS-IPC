/*
 * =====================================================================================
 *
 *       Filename:  igmp_pkt.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/22/2017 09:38:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <string.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <ftm/pkt_ip.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

#include "igmp.h"
#include "igmp_pkt.h"

struct igmp_pkt_debug igmp_pkt_dbg = {0, 0};

 /*igmp packet receive register*/
void igmp_pkt_register(void)
{
	union proto_reg proto;

	/*igmp general query packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_GENERAL_QUERY_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v2 leave packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_V2_LEAVE_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v3  report packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_V3_REPORT_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v3  specific group / source packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_register(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);
}

void igmp_pkt_unregister(void)
{
	union proto_reg proto;

	/*igmp general query packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_GENERAL_QUERY_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_unregister(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v2 leave packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_V2_LEAVE_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_unregister(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v3  report packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.dip = IGMP_V3_REPORT_IPADDRESS;
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_unregister(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);

	/*igmp v3  specific group / source packet*/
	memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_IGMP;
	pkt_unregister(MODULE_ID_IPMC, PROTO_TYPE_IPV4, &proto);
}


int igmp_ipmc_pkt_send(uint32_t src_ip, uint32_t dst_ip, uint32_t ifindex, void *data, int data_len)
{
	struct ipc_mesg_n *pSndMsg = NULL;
	uint32_t type = PKT_TYPE_IPMC;
	union pkt_control pkt_ctrl;
	uint8_t *pdata = NULL;
	int ret = ERRNO_FAIL;
	
	if (NULL == data)
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = DISABLE;
	pkt_ctrl.ipcb.protocol = IP_P_IGMP;
	pkt_ctrl.ipcb.ifindex = ifindex;
	pkt_ctrl.ipcb.sip = src_ip;
	pkt_ctrl.ipcb.dip = dst_ip;
	pkt_ctrl.ipcb.tos = 4; 
	pkt_ctrl.ipcb.ttl = 1;
	pkt_ctrl.ipcb.is_changed = 1;

	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_IPMC);
	if(pSndMsg == NULL) 
	{
		zlog_err("%s[%d] : mem share malloc error\n", __FUNCTION__, __LINE__);
		return ERRNO_FAIL;
    }

	memset(pSndMsg, 0, (sizeof(struct ipc_msghdr_n) + sizeof(union pkt_control) + data_len));

    pSndMsg->msghdr.module_id   = MODULE_ID_FTM;
    pSndMsg->msghdr.sender_id   = 0;
    pSndMsg->msghdr.msg_type    = IPC_TYPE_PACKET;
    pSndMsg->msghdr.msg_subtype = type;
    pSndMsg->msghdr.opcode      = 0;
    pSndMsg->msghdr.data_num    = 1;
    pSndMsg->msghdr.data_len    = sizeof(union pkt_control) + data_len;

	pdata = pSndMsg->msg_data;
	memcpy(pdata, &pkt_ctrl, sizeof(union pkt_control));
	memcpy(pdata+sizeof(union pkt_control), data, data_len);
	
	//ret = pkt_send(type, &pkt_ctrl, data, data_len);
	ret = ipc_send_msg_n1(pSndMsg, (sizeof(struct ipc_msghdr_n) + sizeof(union pkt_control) + data_len));
	if(-1 == ret)
	{
		zlog_err("%s, %d pkt_send fail!\n", __FUNCTION__, __LINE__);
		mem_share_free(pSndMsg, MODULE_ID_IPMC);
		return ERRNO_FAIL;		
	}
	
	return ERRNO_SUCCESS;
}


int igmp_pkt_recv(struct pkt_buffer *ppkt)
{
	struct igmpv12_report *report1 = NULL;
	struct igmpv12_report *report2 = NULL;
	struct igmpv3_report *report3 = NULL;
	struct igmpv12_report *leave = NULL;
	struct igmpv3_query *query = NULL;
	struct ip_control *pipcb = NULL;
	struct iphdr *iph = NULL;
	uint8_t max_resp_code;
	uint8_t query_version;
	char *msg = NULL;
	uint16_t msg_len;
	uint8_t msg_type;
	uint8_t ip_hlen;
	
	msg_len = ppkt->data_len;
   	if (msg_len < IGMP_PLEN_MIN) 
	{
		zlog_err("%s, %d igmp packet size=%d shorter than minimum=%d",
	      	__FUNCTION__, __LINE__, msg_len, IGMP_PLEN_MIN);
    	return ERRNO_FAIL;
  	}

	pipcb = (struct ip_control *)&(ppkt->cb);
	pipcb->ifindex = ppkt->in_ifindex;

	if (IP_P_IGMP != pipcb->protocol)
	{
		zlog_err("%s, %d igmp recv pkt err: protocol(%d)!\n",__FUNCTION__, __LINE__, pipcb->protocol);
		return ERRNO_FAIL;
	}
	
	if (1 != pipcb->ttl)
	{
		zlog_err("%s, %d igmp recv pkt err: ttl(%d)!\n", __FUNCTION__, __LINE__, pipcb->ttl);
		return ERRNO_FAIL;
	}

//	iph = (struct iphdr *)ppkt->network_header;
//	if(iph == NULL)
//	{
//		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
//		return ERRNO_FAIL;
//	}
//	
//	ip_hlen = iph->ihl << 2; 
//  	if (ip_hlen < IGMP_HLEN_MIN) 
//	{
//    	zlog_err("%s, %d igmp packet header size=%d shorter than minimum=%d\n",
//	      	__FUNCTION__, __LINE__, ip_hlen, IGMP_HLEN_MIN);
//    	return ERRNO_FAIL;
//  	}
//	
//  	if (ip_hlen > IGMP_HLEN_MAX) 
//	{
//    	zlog_err("%s, %d igmp packet header size=%d greater than maximum=%d\n",
//	      	__FUNCTION__, __LINE__, ip_hlen, IGMP_HLEN_MAX);
//    	return ERRNO_FAIL;
//  	}

	msg = (char *)ppkt->data;
	msg_type = *msg;
	
	switch (msg_type) 
	{
  		case IGMP_QUERY_MEMBER:
    		{
				query = (struct igmpv3_query *)ppkt->data;
      			max_resp_code = query->code;

				  /*
				RFC 3376: 7.1. Query Version Distinctions
				IGMPv1 Query: length = 8 octets AND Max Resp Code field is zero
				IGMPv2 Query: length = 8 octets AND Max Resp Code field is non-zero
				IGMPv3 Query: length >= 12 octets
				  */

      			if (msg_len == 8) 
				{
					query_version = max_resp_code ? IGMP_VERSION2 : IGMP_VERSION1;
      			}
      			else if (msg_len >= 12) 
				{
					query_version = IGMP_VERSION3;
      			}
      			else 
				{
					zlog_err("Unknown IGMP query version");
					return ERRNO_FAIL;
      			}

      			recv_igmp_query(query, query_version, pipcb);

				if (igmp_pkt_dbg.pkt_rsv)
				{
					zlog_debug (IGMP_DBG_RCV, "%s\n", "IGMP Query Receive");
					igmp_packet_dump(query, IGMP_QUERY_MEMBER, query_version);
				}
    		}
			break;
			
  		case IGMP_REPORT_MEMBER_V3:
			report3 = (struct igmpv3_report *)ppkt->data;
    		igmp_v3_report(report3, pipcb);

			if (igmp_pkt_dbg.pkt_rsv)
			{
				zlog_debug (IGMP_DBG_RCV, "%s\n", "IGMP Report Receive");
				igmp_packet_dump(report3, IGMP_REPORT_MEMBER_V3, 0);
			}
			break;

  		case IGMP_REPORT_MEMBER_V2:
			report2 = (struct igmpv12_report *)ppkt->data;
    		igmp_v2_report(report2, pipcb);

			if (igmp_pkt_dbg.pkt_rsv)
			{
				zlog_debug (IGMP_DBG_RCV, "%s\n", "IGMP Report Receive");
				igmp_packet_dump(report2, IGMP_REPORT_MEMBER_V2, 0);
			}
			break;

  		case IGMP_REPORT_MEMBER_V1:
			report1 = (struct igmpv12_report *)ppkt->data;
    		igmp_v1_report(report1, pipcb);

			if (igmp_pkt_dbg.pkt_rsv)
			{
				zlog_debug (IGMP_DBG_RCV, "%s\n", "IGMP Report Receive");
				igmp_packet_dump(report1, IGMP_REPORT_MEMBER_V1, 0);
			}
			break;

  		case IGMP_REPORT_LEAVE_V2:
			leave = (struct igmpv12_report *)ppkt->data;
    		igmp_v2_leave(report2, pipcb);

			if (igmp_pkt_dbg.pkt_rsv)
			{
				zlog_debug (IGMP_DBG_RCV, "%s\n", "IGMP Leave Receive");
				igmp_packet_dump(leave, IGMP_REPORT_LEAVE_V2, 0);
			}
			break;

		default:
			return ERRNO_FAIL;
  	}
	
	return ERRNO_SUCCESS;
}

static void igmp_pkt_query_dump(void *data, uint8_t sub_type)
{
	struct igmpv3_query *query = NULL;
	char ipv4_addr[20];
	int i;

	query = (struct igmpv3_query *)data;
	if (NULL == query)
	{
		return;
	}

	inet_ipv4tostr(query->group, ipv4_addr);

	zlog_debug (IGMP_DBG_RCV, "IGMP Query Version: %d", sub_type);
	
	if (sub_type == IGMP_VERSION1)
	{
		zlog_debug (IGMP_DBG_RCV, " Checksum: 0x%04x", query->chsum);
		zlog_debug (IGMP_DBG_RCV, " Group IpAddress: %s\n", ipv4_addr);
	}
	else if (sub_type == IGMP_VERSION2)
	{
		zlog_debug (IGMP_DBG_RCV, " Checksum: 0x%04x", query->chsum);
		zlog_debug (IGMP_DBG_RCV, " Max Response Code:%d ", query->code);
		zlog_debug (IGMP_DBG_RCV, " Group IpAddress: %s\n", ipv4_addr);
	}
	else if (sub_type == IGMP_VERSION3)
	{
		zlog_debug (IGMP_DBG_RCV, " Checksum: 0x%04x", query->chsum);
		zlog_debug (IGMP_DBG_RCV, " Max Response Code:%d ", query->code);
		zlog_debug (IGMP_DBG_RCV, " Group IPAddress: %s", ipv4_addr);
		zlog_debug (IGMP_DBG_RCV, " Querier's Robustness Variable: %d", query->qrv);
		zlog_debug (IGMP_DBG_RCV, " S flag, suppress Router-side processing: %d", query->sflag);
		zlog_debug (IGMP_DBG_RCV, " Querier's Query Interval Code: %d", query->qqi_code);
		zlog_debug (IGMP_DBG_RCV, " Number of Sources: %d", query->src_num);

		if (query->src_num)
		{
			zlog_debug (IGMP_DBG_RCV, " Source IpAddress:");
			for (i = 0; i < query->src_num; i++)
			{
				inet_ipv4tostr(query->sip[i], ipv4_addr);
				zlog_debug (IGMP_DBG_RCV, " %s", ipv4_addr);
			}
		}
	}
	else
	{
		return;
	}
}

static void igmp_pkt_reportv3_dump(void *data)
{
	struct igmpv3_report *report = NULL;
	struct igmppv3_grec *grec = NULL;
	uint32_t *psrc = NULL;
	char ipv4_addr[20];
	int i, j;

	report = (struct igmpv3_report *)data;
	if (NULL == report)
	{
		return;
	}

	zlog_debug (IGMP_DBG_RCV, "%s", "IGMP Report Version: 3");
	zlog_debug (IGMP_DBG_RCV, " Report Type: %d", report->type);
	zlog_debug (IGMP_DBG_RCV, " Reserve1: %d", report->resv1);
	zlog_debug (IGMP_DBG_RCV, " Checksum: 0x%04x", report->chsum);
	zlog_debug (IGMP_DBG_RCV, " Reserve2: %d", report->resv2);
	zlog_debug (IGMP_DBG_RCV, " Number of Group Records: %d \n", report->grec_num);

	if (report->grec_num)
	{
		zlog_debug (IGMP_DBG_RCV, "%s","    Group Record:");
		for (i = 0; i < report->grec_num; i++)
		{
			grec = report->grec;
			zlog_debug (IGMP_DBG_RCV, " Record Type: %d", grec->type);
			zlog_debug (IGMP_DBG_RCV, " Aux Data Len: %d", grec->aux_data);
			zlog_debug (IGMP_DBG_RCV, " Number of Sources: %d", grec->src_num);
			inet_ipv4tostr(grec->group, ipv4_addr);
			zlog_debug (IGMP_DBG_RCV, " Group IpAddress: %s", ipv4_addr);

			if (grec->src_num)
			{
				psrc = grec->sip;
				for (j = 0; j < grec->src_num; j++)
				{
					inet_ipv4tostr(psrc, ipv4_addr);
					zlog_debug (IGMP_DBG_RCV, " Source IpAddress: %s", ipv4_addr);
				}
			}

			grec = (struct igmppv3_grec *) (psrc + grec->src_num);
		}
	}
}

static void igmp_pkt_reportv12_dump(void *data)
{
	struct igmpv12_report *report = NULL;
	char ipv4_addr[20];

	report = (struct igmpv12_report *)data;
	if (NULL == report)
	{
		return;
	}

	
	zlog_debug (IGMP_DBG_RCV, "%s", "IGMP Report Version: 2");
	zlog_debug (IGMP_DBG_RCV, " Report Type: %d", report->type);
	zlog_debug (IGMP_DBG_RCV, " Max Response Code: %d", report->code);
	zlog_debug (IGMP_DBG_RCV, " Checksum: %d", report->chsum);
	
	inet_ipv4tostr(report->group, ipv4_addr);
	zlog_debug (IGMP_DBG_RCV, " Group IpAddress: %d \n", ipv4_addr);
}

static void igmp_pkt_leave_dump(void *data)
{
	struct igmpv12_report *report = NULL;
	char ipv4_addr[20];

	report = (struct igmpv12_report *)data;
	if (NULL == report)
	{
		return;
	}
	
	zlog_debug (IGMP_DBG_RCV, "%s", "IGMP Leave Packet Version: 2");
	zlog_debug (IGMP_DBG_RCV, " Report Type: %d", report->type);
	zlog_debug (IGMP_DBG_RCV, " Max Response Code: %d", report->code);
	zlog_debug (IGMP_DBG_RCV, " Checksum: %d", report->chsum);
	
	inet_ipv4tostr(report->group, ipv4_addr);
	zlog_debug (IGMP_DBG_RCV, " Group IpAddress: %d \n", ipv4_addr);
}


void igmp_packet_dump(void *data, uint8_t msg_type, uint8_t sub_type)
{
	switch (msg_type)
	{
		case IGMP_QUERY_MEMBER:
			igmp_pkt_query_dump(data, sub_type);
			break;
		case IGMP_REPORT_MEMBER_V3:
			igmp_pkt_reportv3_dump(data);
			break;
		case IGMP_REPORT_MEMBER_V2:
			igmp_pkt_reportv12_dump(data);
			break;
		case IGMP_REPORT_MEMBER_V1:
			igmp_pkt_reportv12_dump(data);
			break;
		case IGMP_REPORT_LEAVE_V2:
			igmp_pkt_leave_dump(data);
			break;
		default:
			break;
	}
}

