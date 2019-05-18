/* 
* icmpv4 and icmpv6 packet forward 
*/

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/checksum.h>
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <route/ndp_cmd.h>

#include <string.h>
#include "ftm_ifm.h"
#include "ftm_fib.h"
#include "ftm_arp.h"
#include "proto_reg.h"
#include "ftm_pkt.h"
#include "pkt_icmp.h"
#include "pkt_ndp.h"


/*icmpv6 报文发送到ping 模块*/
void icmpv6_pkt_send_to_ping(struct pkt_buffer *pkt)
{
	struct ip_proto   proto; 
	int module_id = 0;

	/*find the registered module_id */ 
	memset(&proto, 0,sizeof(struct ip_proto));  
    proto.icmpv6_type_valid = 1;
    proto.type = PROTO_TYPE_IPV6;
    proto.protocol = IP_P_IPV6_ICMP;
    proto.icmpv6_type= ICMPV6_TYPE_ECHO;//此处只为找到module_id,echo/reply不作区分
	module_id = ip_proto_lookup(&proto);
	if(!module_id) 
	{
		zlog_err("%s, %d ip_proto_lookup fail!\n",__FUNCTION__, __LINE__);
	}			
    FTM_NDP_ZLOG_DEBUG("PING moudele_id:0x%x\n",module_id);
	//ftm_pkt_send_to(pkt, module_id);/* send to app */
	ftm_pkt_send_to_n(pkt, module_id);/* send to app */

}



/* 接收 icmpv6 报文，识别报文类型 */
int icmpv6_rcv(struct pkt_buffer *pkt)
{
    struct icmpv6_hdr *icmpv6 = NULL;
    struct fibv6_entry *pfib6 = NULL;
    struct ip_control  *ip6cb = NULL;
	struct ftm_ifm     *pifm =  NULL;
	struct ipv6_addr   ipv6_link_local;

	memset(&ipv6_link_local, 0, sizeof(struct ipv6_addr));
    ip6cb  = (struct ip_control *)(&(pkt->cb));
    icmpv6 = (struct icmpv6_hdr *)(pkt->data);
    if(!icmpv6)
        goto drop;

    switch(icmpv6->type)
    {
        case NDP_NEIGHBOR_SOLICIT:
        case NDP_NEIGHBOR_ADVERTISE:
             FTM_NDP_ZLOG_DEBUG("Rcv ndp pkt!\n");
             return ndp_recv(pkt);    
             break;
        case ICMPV6_TYPE_ECHO:
            FTM_NDP_ZLOG_DEBUG("Rcv ping6 echo pkt!\n");
			if(ipv6_is_linklocal(&(ip6cb->dipv6)))
			{
				pifm = ftm_ifm_lookup(pkt->in_ifindex);
				if (!pifm || !(pifm->pl3if) || !ipv6_is_valid((struct ipv6_addr *)(pifm->pl3if->ipv6_link_local.addr)))
				{
					FTM_NDP_ZLOG_ERR("ifindex:%x pifm or pifm->pl3if or linklocal is NULL!\n",pkt->in_ifindex);
					break;
				}
				memcpy(ipv6_link_local.ipv6, pifm->pl3if->ipv6_link_local.addr, IPV6_ADDR_LEN);
			}
            pfib6 = ftm_fibv6_lookup(&(ip6cb->dipv6), 128, ip6cb->vpn);
            if((pfib6 && (pfib6->nhp.nhp_type == NHP_TYPE_CONNECT)) || 
				IPV6_ADDR_SAME(ip6cb->dipv6.ipv6, ipv6_link_local.ipv6))
            {
                icmpv6_pkt_send_to_ping(pkt);
            }
            else if(ip6cb->ttl == 1)
            {
                FTM_NDP_ZLOG_DEBUG("Icmpv6 timeout!\n");
            }
            break;
        case ICMPV6_TYPE_REPLY:
            FTM_NDP_ZLOG_DEBUG("Rcv ping6 reply pkt!\n");
            icmpv6_pkt_send_to_ping(pkt);
            break;
        default:
            FTM_NDP_ZLOG_DEBUG("Unknown icmpv6 type!\n");
            goto drop;
            break;
    }

	pkt_free(pkt);
    return NET_SUCCESS;
drop:
    zlog_debug("%s %d:Icmpv6 recv failed,drop the packet!\n",__func__,__LINE__);
    pkt_free(pkt);
    return NET_FAILED;
}


/* 接收 icmp 报文，识别报文类型 */
int icmp_rcv(struct pkt_buffer *pkt)
{
	struct fib_entry *pfibinfo;
	struct icmphdr *icmph;	
	struct ip_control * ipcb;
	struct ip_proto proto; 
	int module_id;
	uint32_t ip;
	uint16_t id;
	uint16_t seq;		
	int datalen = 0;
	int ret = 0;
	
	//ftm_pkt_dump(pkt->data, pkt->data_len, PKT_ACTION_RECV, PKT_TYPE_IP);

    icmph = (struct icmphdr *)(pkt->data);
	ipcb  = (struct ip_control *)(&(pkt->cb));

	datalen = pkt->data_len;
	if(datalen < 8)
	{
		zlog_err ("ICMP packets's length is less than 8\n");
		goto drop;
	}
	
	zlog_debug(FTM_DBG_IP,"%s, %d type:0x%x,in_ifindex:0x%x,ttl:0x%x\n",__FUNCTION__, __LINE__,icmph->type,pkt->in_ifindex,ipcb->ttl);
    switch(icmph->type)
    {
        case ICMP_TYPE_ECHO:
		{
			pfibinfo = ftm_fib_lookup(ipcb->dip, 32, ipcb->vpn);
			
            /*如果查找到本机32位路由则回复应答*/
            if((pfibinfo != NULL) && (pfibinfo->nhp.nhp_type == NHP_TYPE_CONNECT))
            {
	            icmph->type = ICMP_TYPE_ECHOREPLY;
				icmph->checksum = 0;           /* icmp的校验和 */
				icmph->checksum = in_checksum((u_int16_t *)icmph, datalen);
				ip = ipcb->sip;
				ipcb->sip = ipcb->dip;
				/* Ping设备本身 */
				if(ip == 0)
				{
					ipcb->dip = ipcb->sip;
				}
				else
				{
					ipcb->dip = ip;
				}
				ipcb->ttl = 255;
				ipcb->is_changed = 1;
                ipcb->frag_off = 0;/*pkt frag default*/
            }
            else if(ipcb->ttl == 1) /*ttl 为1 ，没有查找到本机32位路由回复超时*/
            {
				id = icmph->id;
				seq = icmph->sequence;
				
				/* IP头起始报文作为icmp载荷返回,应答报文增加20字节IP头和8字节ICMP头 */
				ret = pkt_push(pkt, 28);
				if(ret != 0)
				{
					zlog_err("%s, %d \n",__FUNCTION__, __LINE__);
					goto drop;
				}		
				ipcb->ttl = 255;
				icmph = (struct icmphdr *)(pkt->data);				
	            icmph->type = ICMP_TYPE_TIMEOUT;
				icmph->code = 0;
				icmph->id = id;
				icmph->sequence = seq;				
				icmph->checksum = 0;           /* icmp的校验和 */
				icmph->checksum = in_checksum((u_int16_t *)icmph, datalen);

				/* 路由跟踪报文，SIP在IP层获取 */
				ipcb->dip = ipcb->sip;
				ipcb->sip = 0;
				ipcb->is_changed = 1;	
                ipcb->frag_off = 0;/*pkt frag default*/
            }
			else
			{
				goto drop;   /* ttl > 1 的非本机报文丢弃 */
			}
            
            (void)icmp_send_pkt((uint8_t *)icmph, datalen, ipcb);
			
            break;
        }
        case ICMP_TYPE_ECHOREPLY:
        case ICMP_TYPE_TIMEOUT:	
		case ICMP_TYPE_UNREACH:				
        {
            /*drop packet if dip's route is not connect*/
			pfibinfo = ftm_fib_lookup(ipcb->dip, 32, ipcb->vpn);
            if((pfibinfo == NULL) || (pfibinfo->nhp.nhp_type != NHP_TYPE_CONNECT))
                goto drop;

			/*find the registered module_id */
			memset(&proto, 0,sizeof(struct ip_proto));  
			proto.dip = ipcb->dip;
			proto.protocol = ipcb->protocol;
			module_id = ip_proto_lookup(&proto);
			if(!module_id) 
			{
				zlog_err("%s, %d ip_proto_lookup fail!\n",__FUNCTION__, __LINE__);
				goto drop;
			}			
			//ftm_pkt_send_to(pkt, module_id);/* send to app */
			ftm_pkt_send_to_n(pkt, module_id);/* send to app */
			break;
        }
        default: /* include ICMP_TYPE_INFO_REQUEST */
		{
			zlog_debug(FTM_DBG_IP,"%s, %d \n",__FUNCTION__, __LINE__);
			goto drop;
		}			
    }

	pkt_free(pkt);
    return NET_SUCCESS;

drop:	
	zlog_err("icmp_rcv get a error packet,drop it!\n");
	pkt_free(pkt);
	return NET_FAILED;
}


/* 发送 icmp 报文 */
int icmp_send_pkt(uint8_t *icmph, int datalen, struct ip_control *ipcb)
{
	int ret;
	struct pkt_buffer *pkt = NULL;

	if(icmph == NULL || ipcb == NULL)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return 1;
	}
	
	pkt = pkt_alloc(datalen + 100);
	if(!pkt)
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		return 1;
	}	
	
	ret = pkt_push(pkt, datalen);
	if(ret) 
	{
		zlog_err ("%s, %d\n",__FUNCTION__, __LINE__);
		goto drop;
	}

	memcpy((uint8_t *)pkt->data, icmph, datalen);
	memcpy((uint8_t *)&(pkt->cb), (uint8_t *)ipcb, sizeof(struct ip_control));

	return ip_forward(pkt);
	
drop:	
	zlog_err("icmp_send_pkt fail!\n");
	pkt_free(pkt);
	return 1;	
}

int icmp_send(struct pkt_buffer *pkt, uint8_t type, uint8_t code)
{
	struct ip_control *ipctrl = NULL;
	struct iphdr *iph = NULL;
	struct ip_control ipcb;
	uint8_t *buffer = NULL;
	struct icmpv4_hdr *icmph = NULL;
	uint8_t *payload_ip = NULL;
	uint8_t *payload_res = NULL;
	uint32_t len = 0;
	uint32_t reserve = 0;
	struct fib_entry *pfib = NULL;

	zlog_debug(FTM_DBG_IP,"%s[%d]:In function %s, type:%d code:%d\n", __FILE__,__LINE__,__func__, type, code);
	
	memset(&ipcb, 0, sizeof(struct ip_control));
	ipctrl = (struct ip_control *)&(pkt->cb);
	iph = (struct iphdr *)(pkt->network_header);

	if (!iph)
	{
		if (ipctrl->protocol == IP_P_UDP)
		{
			zlog_debug(FTM_DBG_IP,"%s[%d]:In function %s, sip:0x%x dip:0x%x\n", __FILE__,__LINE__,__func__, ipctrl->sip, ipctrl->dip);

			pfib = ftm_fib_lookup(ipctrl->sip, 32, ipctrl->vpn);
			if ((ipctrl->sip == 0) || ((pfib)&&(pfib->nhp.nhp_type == NHP_TYPE_CONNECT)))
			{
				ipctrl->sip = ipctrl->dip;
				pkt->data = pkt->transport_header;
				//ftm_pkt_send_to(pkt, MODULE_ID_PING);
				ftm_pkt_send_to_n(pkt, MODULE_ID_PING);
			}
			
		}
		else
		{
			zlog_debug(FTM_DBG_IP,"%s[%d]:In function %s", __FILE__,__LINE__,__func__);
		}
	
		return ERRNO_SUCCESS;
	}
	
	len = sizeof(struct icmpv4_hdr) + 4 + IP_HEADER_SIZE + ICMP_HEADER_SIZE;
	buffer = XCALLOC(MTYPE_PING, len);
	if (!buffer) return ERRNO_FAIL;

	switch (type)
	{
		case ICMP_TYPE_TIMEOUT:
		case ICMP_TYPE_UNREACH:
			ipcb.dip = ipctrl->sip;

			if (type == ICMP_TYPE_UNREACH)
				ipcb.sip = ipctrl->dip;
			ipcb.protocol = 1;
			ipcb.pkt_type = PKT_TYPE_IP;
			ipcb.is_changed = 1;
			ipcb.ttl = 255;
			ipcb.vpn = ipctrl->vpn;

			payload_res = buffer + sizeof(struct icmpv4_hdr);
			payload_ip = payload_res + 4;
			icmph = (struct icmpv4_hdr *)buffer;
			icmph->type = type;
			icmph->code = code;
			memcpy(payload_res, &reserve, 4);
			memcpy(payload_ip, iph, IP_HEADER_SIZE + ICMP_HEADER_SIZE);
			icmph->checksum = in_checksum((u_int16_t *)icmph, len);

			icmp_send_pkt((uint8_t *)icmph, len, &ipcb);
			break;
		default:
			zlog_err("[%s]%d:Error icmp type!\n",__func__,__LINE__);
			break;
	}

	XFREE(MTYPE_PING, buffer);
	return ERRNO_SUCCESS;
}


