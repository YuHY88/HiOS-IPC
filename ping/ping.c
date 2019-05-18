#include <zebra.h>
#include <lib/vty.h>
#include <lib/msg_ipc.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/thread.h>
#include <lib/log.h>
#include <lib/timer.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/linklist.h>
#include <lib/vty.h>
#include <lib/memory.h>
#include <lib/checksum.h>
#include <lib/module_id.h>
#include <lib/command.h>

#include "ping.h"
#include "ping6.h"

/**
 * @brief      <+rcv pkt from ipc+>
 * @param[in ] <+thread master+>
 * @param[out] <+none+>
 * @return     <+never break out+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 9:55:37 AM
 * @note       <+pkt about icmp or icmpv6 or udp+>
 */
 #if 0
int icmp_ping_msg_rcv(struct thread *thread)
{
	struct pkt_buffer *pkt = NULL;
    struct ip_control * ipcb = NULL;
    struct icmphdr *icmp_pkt = NULL;
    struct icmpv6_req *icmpv6_pkt = NULL;
    uint32_t datalen = 0;
	int token = 20;

	while (token > 0)
	{
		pkt = pkt_rcv(MODULE_ID_PING);
		if(pkt == NULL)
			goto out;

        ipcb  = (struct ip_control *)(&(pkt->cb)); 
        if(ipcb->protocol == IP_P_ICMP)
        {
            icmp_pkt = (struct icmphdr *)(pkt->data);
            datalen = pkt->data_len;

           if(ICMP_TYPE_ECHOREPLY == icmp_pkt->type)
           {
               icmp_ping_recv_reply(icmp_pkt,ipcb,datalen);
           }
		   else
		   {
				trace_udp_rcv_reply(icmp_pkt, ipcb);
		   }
        }
        else if(ipcb->protocol == IP_P_IPV6_ICMP)
        {
            icmpv6_pkt = (struct icmpv6_req *)(pkt->data);
            datalen = pkt->data_len;
            if(ICMPV6_TYPE_ECHO == icmpv6_pkt->hdr.type)
            {
                icmpv6_ping_recv_request(pkt);
            }
            else if(ICMPV6_TYPE_REPLY == icmpv6_pkt->hdr.type)
            {
                icmpv6_ping_recv_reply(icmpv6_pkt, ipcb, datalen);
            }
        }
		else if(ipcb->protocol == IP_P_UDP)
		{
			PING_ZLOG_DEBUG();
			trace_udp_rcv_localip(pkt->data, ipcb);
		}
		
        token--;
	}

out:
	usleep(1000);
    thread_add_event (ping_master, icmp_ping_msg_rcv, NULL, 0);
    return 0;		
}
#endif

int icmp_ping_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen)
{
	struct pkt_buffer *pkt = NULL;
    struct ip_control * ipcb = NULL;
    struct icmphdr *icmp_pkt = NULL;
    struct icmpv6_req *icmpv6_pkt = NULL;
    uint32_t datalen = 0;
	int revln = 0;
	void *pdata = NULL;

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
	if(revln <= imlen)
	{
		pkt = (struct pkt_buffer *)(pmesg->msg_data);

		/*pkt->next = NULL;
		pkt->prev = NULL;
		pkt->transport_header = NULL;
		pkt->network_header = NULL;
		pkt->link_header = NULL;
		pkt->data = (char *)pkt + pkt->data_offset;*/
		APP_RECV_PKT_FROM_FTM_DATA_SET(pkt);

        ipcb  = (struct ip_control *)(&(pkt->cb)); 
        if(ipcb->protocol == IP_P_ICMP)
        {
            icmp_pkt = (struct icmphdr *)(pkt->data);
            datalen = pkt->data_len;

           if(ICMP_TYPE_ECHOREPLY == icmp_pkt->type)
           {
               icmp_ping_recv_reply(icmp_pkt,ipcb,datalen);
           }
		   else
		   {
				trace_udp_rcv_reply(icmp_pkt, ipcb);
		   }
        }
        else if(ipcb->protocol == IP_P_IPV6_ICMP)
        {
            icmpv6_pkt = (struct icmpv6_req *)(pkt->data);
            datalen = pkt->data_len;
            if(ICMPV6_TYPE_ECHO == icmpv6_pkt->hdr.type)
            {
                icmpv6_ping_recv_request(pkt);
            }
            else if(ICMPV6_TYPE_REPLY == icmpv6_pkt->hdr.type)
            {
                icmpv6_ping_recv_reply(icmpv6_pkt, ipcb, datalen);
            }
        }
		else if(ipcb->protocol == IP_P_UDP)
		{
			PING_ZLOG_DEBUG();
			trace_udp_rcv_localip(pkt->data, ipcb);
		}
		
	}

	mem_share_free(pmesg, MODULE_ID_PING);
    return 0;		
}


/**
 * @brief      <+set pkt control struct for ip layer+>
 * @param[in ] <+control struct and local param info+>
 * @param[out] <+none+>
 * @return     <+description+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 9:55:19 AM
 * @note       <+none+>
 */
void icmp_ping_set_ipcb(struct ip_control *ipcb,struct ping_info *pingInfo)
{    
    memset(ipcb, 0, sizeof(struct ip_control));

	ipcb->sip = pingInfo->srcip.addr.ipv4;	
	ipcb->dip = pingInfo->destip.addr.ipv4;
	ipcb->protocol = IP_P_ICMP;
	ipcb->is_changed = 0x1;
	ipcb->vpn = pingInfo->vpn;	
	ipcb->frag_off = pingInfo->fragflag;
    ipcb->tos = 2;

    if (PING_CMD == pingInfo->type)
    {
        ipcb->ttl = pingInfo->ttl;
    }
    else if (TRACE_CMD == pingInfo->type)
    {
        ipcb->ttl = 1;
    }
	else if (TRACE_UDP_CMD == pingInfo->type)
	{
		ipcb->ttl = 1;
		ipcb->protocol = IP_P_UDP;
	}
    
}


/**
 * @brief      <+set icmp pkt header include payload+>
 * @param[in ] <+icmp pkt pointer and local param+>
 * @param[out] <+none+>
 * @return     <+packet size include from icmphdr to pkt end+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:04:55 AM
 * @note       <+none+>
 */
int icmp_ping_set_icmphdr(struct icmphdr *icmp_pkt,struct ping_info *pingInfo)
{
    uint32_t packetsize = 0;
    uint16_t i = 0;
    uint8_t  tmpval = 0x61;
    static uint16_t id = 0;

    packetsize = ICMP_HEADER_SIZE + pingInfo->size ;
    icmp_pkt->type = ICMP_TYPE_ECHO;
    icmp_pkt->code = 0;        	
    icmp_pkt->checksum = 0;
    icmp_pkt->sequence = htons(1);
    icmp_pkt->id  = htons(++id);

	for(i = 0; i < pingInfo->size ; i++)
	{
		icmp_pkt->icmp_data[i] = tmpval++;
		if(tmpval == 0x78)
			tmpval = 0x61;
	}
    icmp_pkt->checksum = in_checksum((u_int16_t *)icmp_pkt, packetsize);

    pingInfo->data = icmp_pkt;

    return packetsize;
}


/**
 * @brief      <+print ping result to screen and free memory+>
 * @param[in ] <+local control struct+>
 * @param[out] <+ping result+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:09:18 AM
 * @note       <+none+>
 */
static void icmp_ping_stats_vtyout(struct ping_info *pingInfo)
{
    uint32_t sendcnt,recvcnt;

    sendcnt = pingInfo->stats->sendcnt;
    recvcnt = pingInfo->stats->recvcnt;
    
    vty_out(pingInfo->pvty, "---PING statistics----\r\n");   
    vty_out(pingInfo->pvty, "%d packets transmitted, %d received , %d%% lost\r\n",
                                  sendcnt,recvcnt,(sendcnt - recvcnt)*100/sendcnt);
    vtysh_return(pingInfo->pvty,CMD_SUCCESS);
    
    XFREE(MTYPE_PING,pingInfo->stats);
    XFREE(MTYPE_PING,pingInfo->data);
    XFREE(MTYPE_PING,pingInfo);
}


/**
 * @brief      <+print trace result to screen+>
 * @param[in ] <+local control struct+>
 * @param[out] <+trace result+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:12:28 AM
 * @note       <+none+>
 */
static void icmp_trace_vtyout(struct ping_info *pingInfo)
{
    vty_out(pingInfo->pvty,"Trace complete!\r\n");

    vtysh_return(pingInfo->pvty,CMD_SUCCESS);

    zlog_debug(PING_DBG_ALL,"func:%s line:%d pingInfo:%p pingInfo->data:%p pingInfo->stats:%p pingInfo->stats->buffer:%p\n"
        ,__func__,__LINE__,pingInfo,pingInfo->data,pingInfo->stats,pingInfo->stats->buffer);

    XFREE(MTYPE_PING,pingInfo->stats->buffer);
    XFREE(MTYPE_PING,pingInfo->stats);
    XFREE(MTYPE_PING,pingInfo->data);
    XFREE(MTYPE_PING,pingInfo);

}

/**
 * @brief      <+process icmp reply and print result to screen+>
 * @param[in ] <+icmp reply pkt and ip control and pkt len+>
 * @param[out] <+ping result+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:34:26 AM
 * @note       <+none+>
 */
void icmp_ping_recv_reply(struct icmphdr *icmp_reply,struct ip_control *ipcb,uint32_t datalen)
{
    struct ping_info *pingInfo = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
    struct icmphdr   *icmp_req = NULL;
    struct timeval recvsec;
    uint8_t addr[20];
    uint8_t tmp[10];
    uint32_t time;
    uint16_t reply_seq,reply_id;
	uint8_t ping_pkt_flag = 0;
   
    reply_seq = ntohs(icmp_reply->sequence);
    reply_id = ntohs(icmp_reply->id);
	inet_ipv4tostr(ipcb->sip, (char *)addr);
    time_get_time (TIME_CLK_REALTIME, &recvsec);
    
    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',ipcb->sip:0x%x seq:%d ttl:%d\n",
        __FILE__,__LINE__,__func__,ipcb->sip,reply_seq,ipcb->ttl);
    
	for (ALL_LIST_ELEMENTS(preq_list, pnode, pnextnode, pingInfo))
	{
	    icmp_req = (struct icmphdr *)(pingInfo->data);
		if((reply_id == ntohs(icmp_req->id)) && (reply_seq == ntohs(icmp_req->sequence)))
		{  
            zlog_debug(PING_DBG_ALL,"%s, %d icmp_ping_recv_reply! pingInfo:%p seq:%d\n",
                __FUNCTION__, __LINE__,pingInfo,reply_seq);

            if(PING_CMD == pingInfo->type && ICMP_TYPE_TIMEOUT == icmp_reply->type)
            {
                zlog_debug(PING_DBG_ALL,"func:%s line:%d ping recv timeout datagram.\n",__func__,__LINE__);
                break;
            }

            time = timeval_elapsed (recvsec, pingInfo->sendsec);

            if (PING_CMD == pingInfo->type)
            {
            	if(pingInfo->stats->lastrecv != 1)
        		{	
        			pingInfo->stats->recvcnt++;
        			pingInfo->stats->lastrecv = 1;
	                vty_out(pingInfo->pvty,"Reply from %s: bytes=%d Sequence=%d ttl=%d time:%dms\r\n",
	                                        addr,datalen,reply_seq,ipcb->ttl,time/1000);
        		}
            	ping_pkt_flag = 1;
                //icmp_ping_send_next(pingInfo);
            }
            else if (TRACE_CMD == pingInfo->type)
            {
                zlog_debug(PING_DBG_ALL,"%s %d:In func '%s'",__FILE__,__LINE__,__func__);      

                ICMP_TRACE_SPRINT_TTL(icmp_reply,pingInfo,(char *)tmp);
                sprintf((char *)tmp,"%4dms ",time/1000);
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);
                if(inet_valid_ipv4(ipcb->sip))
                    pingInfo->stats->ip = ipcb->sip;
                ICMP_TRACE_SPRINT_IP(icmp_reply,pingInfo,(char *)addr);
                ICMP_TRACE_VTYOUT(icmp_reply,pingInfo);

                icmp_trace_send_next(pingInfo);
            }
			if(ping_pkt_flag == 0)
        	{
        		list_delete_node(preq_list, pnode);
			}
            return ;
		}
	}
    
    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',preq_list->count:%d\n",__FILE__,__LINE__,__func__,preq_list->count);
 
}

/**
 * @brief      <+timer thread for ping,if time out 
 * 				  print timeout result to screen+>
 * @param[in ] <+thread master+>
 * @param[out] <+timeout info+>
 * @return     <+if preq_list's node is empty,
 *				return zero;else never break out+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:43:22 AM
 * @note       <+none+>
 */
int icmp_ping_timer_thread(void *thread)
{
	struct ping_info *pingInfo = NULL;
	struct listnode  *pnode  = NULL;
	struct listnode  *pnextnode = NULL; 
    struct icmphdr   *icmp_req = NULL;
    struct timeval   time;
    uint32_t wait_time;
    uint8_t tmp[10];
	uint8_t ttl = 0;
	uint8_t addr[20];
	struct udphdr *udph = NULL;
	uint8_t ping_pkt_flag = 0;

	for (ALL_LIST_ELEMENTS(preq_list, pnode, pnextnode, pingInfo))
	{  
        time_get_time (TIME_CLK_REALTIME, &time);
        wait_time =  timeval_elapsed (time, pingInfo->sendsec);
		if (pingInfo->type == PING_CMD ||
			pingInfo->type == TRACE_CMD)
		{
			icmp_req = (struct icmphdr *)(pingInfo->data);
			zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p icmp_req->seq:%d,pingInfo->time:%d wait_time:%d\n",__FILE__,__LINE__,__func__,pingInfo,icmp_req->sequence,pingInfo->time,wait_time);
		}
		else if (pingInfo->type == TRACE_UDP_CMD)
		{
			udph = (struct udphdr *)(pingInfo->data);
		}
        
		if (wait_time > (pingInfo->time)*1000*1000)
		{
            if (PING_CMD == pingInfo->type)
            {  
				if( pingInfo->stats->lastrecv == 0 )
        		{
        			if(wait_time > (pingInfo->waittime)*1000*1000)
        			{
        				vty_out(pingInfo->pvty,"ping ip timeout!seq:%d...\r\n",ntohs(icmp_req->sequence));
						list_delete_node(preq_list, pnode);
						icmp_ping_send_next(pingInfo);
						break;
					}
				}
				else
				{
					list_delete_node(preq_list, pnode);
					icmp_ping_send_next(pingInfo);
					break;
				}
				ping_pkt_flag = 1;
            }
            else if (TRACE_CMD == pingInfo->type)
            {
                ICMP_TRACE_SPRINT_TTL(icmp_req,pingInfo,(char *)tmp);
                sprintf((char *)tmp, "%5s", " * ");
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);

                ICMP_TRACE_VTYOUT(icmp_req,pingInfo);
                
                icmp_trace_send_next(pingInfo);
            }
			else if (TRACE_UDP_CMD == pingInfo->type)
			{
				if( pingInfo->stats->lastrecv == 0 )
        		{
        			if(wait_time > (pingInfo->waittime)*1000*1000)
        			{
						ttl = (ntohs(udph->dport) - TRACE_BASE_DPORT)/(pingInfo->count) + 1;
						TRACE_SET_SPRINT_TTL(ttl, pingInfo, tmp);
						sprintf((char *)tmp, "%5s", " * ");
		                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);

				zlog_debug(PING_DBG_ALL,"%s%d:ttl:0x%x sencnt:%d cnt:%d\n",__func__,__LINE__,ttl,pingInfo->stats->sendcnt,pingInfo->count);
						if (pingInfo->stats->sendcnt == pingInfo->count)
						{
							if( pingInfo->stats->ip != 1 )
							{
								TRACE_SET_SPRINT_IP(pingInfo->stats->ip, pingInfo, addr);
							}
							else
							{
								sprintf((char *)tmp, "%s", " tracert ip timeout ");
		                		strcat((char *)(pingInfo->stats->buffer),(char *)tmp);
								vty_out(pingInfo->pvty,"%s\r\n",pingInfo->stats->buffer);
								memset(pingInfo->stats->buffer,0,3 + (pingInfo->count) * 7 + 17);
							}
						}

        				list_delete_node(preq_list, pnode);
						trace_udp_send_next(pingInfo);
						break;
					}
				}
				else
				{
					list_delete_node(preq_list, pnode);
					trace_udp_send_next(pingInfo);
					break;
				}
				ping_pkt_flag = 1;
			/*
			    ttl = (ntohs(udph->dport) - TRACE_BASE_DPORT)/(pingInfo->count) + 1;
				TRACE_SET_SPRINT_TTL(ttl, pingInfo, tmp);
				sprintf((char *)tmp, "%5s", " * ");
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);

				zlog_debug("%s%d:ttl:0x%x sencnt:%d cnt:%d\n",__func__,__LINE__,ttl,pingInfo->stats->sendcnt,pingInfo->count);
				if (pingInfo->stats->sendcnt == pingInfo->count)
				{
					vty_out(pingInfo->pvty,"%s\r\n",pingInfo->stats->buffer);
					memset(pingInfo->stats->buffer,0,3 + (pingInfo->count) * 7 + 17);
				}
				
				trace_udp_send_next(pingInfo);
				*/
			}
			if( ping_pkt_flag != 1 ) 
            	list_delete_node(preq_list, pnode);
            break;
		}
        else
        {
           //vty_out(pingInfo->pvty,"ping ip waiting...\r\n");
        }

	}

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',preq_list->count:%d\n",__FILE__,__LINE__,__func__,preq_list->count);

    if (preq_list->count)
		//thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1); /* Æð1s ¶¨Ê±Æ÷*/
		high_pre_timer_add("PingThreadTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
	else
		ptimer = 0;

	return 0;
    
}


/**
 * @brief      <+send next pkt base prev pkt+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:52:13 AM
 * @note       <+none+>
 */
void icmp_ping_send_next(struct ping_info *pingInfo)
{
    struct icmphdr *icmp_pkt = NULL;
    struct icmphdr *next_pkt = NULL;
    struct ip_control pkt_ctrl;
    uint16_t icmp_seq = 0;
    uint32_t packetsize = ICMP_HEADER_SIZE + pingInfo->size;;
    uint8_t  ret;
    struct vty *pvty = pingInfo->pvty;

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p",__FILE__,__LINE__,__func__,pingInfo->data);

    icmp_pkt = pingInfo->data;
    if(ntohs(icmp_pkt->sequence) >= pingInfo->count
        ||(pvty->sig_int_recv == 1))
    {
        pvty->sig_int_recv = 0;
        icmp_ping_stats_vtyout(pingInfo);
        return ;
    }
    
    icmp_ping_set_ipcb(&pkt_ctrl,pingInfo); 
    next_pkt = icmp_pkt;
    icmp_seq = ntohs(next_pkt->sequence);
    next_pkt->sequence = htons(++icmp_seq);
    next_pkt->checksum = 0;
    next_pkt->checksum = in_checksum((uint16_t *)next_pkt,packetsize);
    
	ret = pkt_send(PKT_TYPE_IP, (union pkt_control *)&pkt_ctrl, next_pkt, packetsize);
	if( 0 != ret )
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ;		
	}
    pingInfo->stats->sendcnt++;
	pingInfo->stats->lastrecv = 0;
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));
    
	listnode_add(preq_list, pingInfo); 
    if( 0 == ptimer )
    {
        //ptimer = thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1);
		ptimer = high_pre_timer_add("pingTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
    }

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p preq_list->count:%d\n",__FILE__,__LINE__,__func__,pingInfo,preq_list->count);

}



/**
 * @brief      <+send icmp request+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+return errno+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 10:55:08 AM
 * @note       <+none+>
 */
int icmp_ping_send_request(struct ping_info *pingInfo)
{
    struct icmphdr *icmp_pkt = NULL;
    struct ip_control pktcontrol;
    int packetsize;	
	int ret;

    icmp_pkt = XMALLOC(MTYPE_PING,sizeof(struct icmphdr) + pingInfo->size);
    if ( NULL == icmp_pkt )
    {
        return ERRNO_MALLOC;
    }

    icmp_ping_set_ipcb(&pktcontrol,pingInfo);   
    packetsize = icmp_ping_set_icmphdr(icmp_pkt,pingInfo);
	ret = pkt_send(PKT_TYPE_IP, (union pkt_control *)&pktcontrol, icmp_pkt, packetsize);
	if( 0 != ret )
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;		
	}

    pingInfo->stats->sendcnt++;    
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));    
	listnode_add(preq_list, pingInfo); 
    if( 0 == ptimer )
    {
        //ptimer = thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1);
		ptimer = high_pre_timer_add("pingTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
    }

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p preq_list->count:%d\n",__FILE__,__LINE__,__func__,pingInfo,preq_list->count);

    return 0;
}


/**
 * @brief      <+trace send next packet+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 1:46:51 PM
 * @note       <+none+>
 */
void icmp_trace_send_next(struct ping_info *pingInfo)
{
    struct icmphdr *icmp_pkt = NULL;
    struct icmphdr *next_pkt = NULL;
    struct ip_control pkt_ctrl;
    struct vty *pvty = pingInfo->pvty;
    uint32_t packetsize = ICMP_HEADER_SIZE + pingInfo->size;
    uint16_t icmp_seq = 0;
    uint8_t  ret;
    static uint8_t ttl = 1;
    
    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p ttl:%d\n",__FILE__,__LINE__,__func__,pingInfo->data,ttl);

    icmp_pkt = pingInfo->data;
    if((ttl >= pingInfo->ttl && ntohs(icmp_pkt->sequence) >= pingInfo->count)
        || ((pingInfo->stats->ip == (uint32_t)(pingInfo->destip.addr.ipv4)) && (ntohs(icmp_pkt->sequence) >= pingInfo->count))
        || (pvty->sig_int_recv == 1))
    {
        ttl = 1;
        pvty->sig_int_recv = 0;
        icmp_trace_vtyout(pingInfo);
        return ;
    }

    next_pkt = icmp_pkt;
    if(ntohs(next_pkt->sequence) == pingInfo->count)
    {
        ttl++;
        next_pkt->sequence = htons(1);
    }
    else
    {   
        icmp_seq = ntohs(next_pkt->sequence);
        next_pkt->sequence = htons(++icmp_seq);
    }
	
    next_pkt->checksum = 0;
    next_pkt->checksum = in_checksum((uint16_t *)next_pkt,packetsize);
    icmp_ping_set_ipcb(&pkt_ctrl,pingInfo);
    pkt_ctrl.ttl = ttl;
	ret = pkt_send(PKT_TYPE_IP, (union pkt_control *)&pkt_ctrl, next_pkt, packetsize);
	if( 0 != ret )
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ;		
	}
	
    pingInfo->stats->sendcnt++;
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));    
	listnode_add(preq_list, pingInfo); 
    if( 0 == ptimer )
    {
        //ptimer = thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1);
		ptimer = high_pre_timer_add("pingTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
    }

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p preq_list->count:%d\n",__FILE__,__LINE__,__func__,pingInfo,preq_list->count);

}


/**
 * @brief      <+trace set udp header and payload+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 1:50:24 PM
 * @note       <+none+>
 */
void trace_set_udphdr(uint8_t *udp_pkt,struct ping_info *pingInfo)
{
	struct udphdr *udph = NULL;
	uint8_t *udp_payload = NULL;
	uint32_t i = 0;
	uint32_t tmpval = 0x61;

	udph = (struct udphdr *)udp_pkt;
	udp_payload = (uint8_t *)((uint8_t *)udph + sizeof(struct udphdr));
	udph->sport = htons(TRACE_SPORT);
	udph->dport = htons(TRACE_BASE_DPORT);
	udph->len = htons(sizeof(struct udphdr) + pingInfo->size);
	udph->checksum = 0;

	for(i = 0; i < pingInfo->size ; i++)
	{
		udp_payload[i] = tmpval++;
		if(tmpval == 0x78)
			tmpval = 0x61;
	}
	
	pingInfo->data = udp_pkt;
}

/**
 * @brief      <+trace send udp packet+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+return errno+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 1:58:06 PM
 * @note       <+none+>
 */
int trace_udp_send(struct ping_info *pingInfo)
{
	uint8_t *udp_pkt = NULL;
	struct ip_control pktctrl;
	uint8_t ret = 0;
	uint32_t pkt_size = 0;

	pkt_size = sizeof(struct udphdr) + pingInfo->size;
	udp_pkt = XCALLOC(MTYPE_PING, pkt_size);
	if (!udp_pkt) return ERRNO_FAIL;

	memset(&pktctrl, 0, sizeof(struct ip_control));
	icmp_ping_set_ipcb(&pktctrl, pingInfo);
	pktctrl.chsum_enable = 1;
	trace_set_udphdr(udp_pkt, pingInfo);

	ret = pkt_send(PKT_TYPE_IP, (union pkt_control *)&pktctrl, udp_pkt, pkt_size);
	if( 0 != ret )
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return ERRNO_FAIL;		
	}

    pingInfo->stats->sendcnt++;
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));   
	listnode_add(preq_list, pingInfo); 
    if( 0 == ptimer )
    {
        //ptimer = thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1);
		ptimer = high_pre_timer_add("pingTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
    }

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p preq_list->count:%d\n",__FILE__,__LINE__,__func__,pingInfo,preq_list->count);

    return 0;	
}

/**
 * @brief      <+trace send next udp packet base prev+>
 * @param[in ] <+local control struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 1:59:59 PM
 * @note       <+none+>
 */
void trace_udp_send_next(struct ping_info *pingInfo)
{
	struct ip_control ipcb;
	struct udphdr *udph = NULL;
	uint8_t ret = 0;
	uint16_t tmp_dport = 0;
	struct vty *pvty = pingInfo->pvty;
	static uint8_t ttl = 1;

	zlog_debug(PING_DBG_ALL,"%s %d ttl:%d pingInfo->ttl:%d sig_int_rcv:%d\n",__func__,__LINE__,ttl,pingInfo->ttl,pvty->sig_int_recv);
	zlog_debug(PING_DBG_ALL,"%s%d:sendcnt:%d cnt:%d\n",__func__,__LINE__,pingInfo->stats->sendcnt,pingInfo->count);
    
	if ((pvty->sig_int_recv == 1) || 
		((ttl >= pingInfo->ttl) && (pingInfo->stats->sendcnt == pingInfo->count))|| 
		((pingInfo->stats->ip == (uint32_t)(pingInfo->destip.addr.ipv4) && (pingInfo->stats->sendcnt == pingInfo->count))))
	{
	    ttl = 1;
		pvty->sig_int_recv = 0;
		return icmp_trace_vtyout(pingInfo);
	}
	memset(&ipcb, 0, sizeof(struct ip_control));
	if ((pingInfo->stats->sendcnt == pingInfo->count))
	{
		ttl ++;
		pingInfo->stats->sendcnt = 0;
	}

	icmp_ping_set_ipcb(&ipcb, pingInfo);
	ipcb.ttl = ttl;
	ipcb.chsum_enable = 1;
	udph = pingInfo->data;
	tmp_dport = ntohs(udph->dport);
	udph->dport = htons(++tmp_dport);
	udph->checksum = 0;
	
	ret = pkt_send(PKT_TYPE_IP, (union pkt_control *)&ipcb, pingInfo->data, ntohs(udph->len));
	if( 0 != ret )
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
		return;		
	}

    pingInfo->stats->sendcnt++;  
	pingInfo->stats->lastrecv = 0;  
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));    
	listnode_add(preq_list, pingInfo); 
    if( 0 == ptimer )
    {
        //ptimer = thread_add_timer(ping_master, icmp_ping_timer_thread, 0, 1);
		ptimer = high_pre_timer_add("pingTimer",LIB_TIMER_TYPE_NOLOOP, icmp_ping_timer_thread, NULL, 1*1000);
    }

    zlog_debug(PING_DBG_ALL,"%s %d:In func '%s',pingInfo:%p preq_list->count:%d\n",__FILE__,__LINE__,__func__,pingInfo,preq_list->count);
	
}

/**
 * @brief      <+process icmp pkt for udp_trace+>
 * @param[in ] <+icmp packet and ip control struct+>
 * @param[out] <+print each hop trace result+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 2:04:09 PM
 * @note       <+none+>
 */
void trace_udp_rcv_reply(struct icmphdr *icmp_pkt,struct ip_control *ipcb)
{
    struct ping_info *pingInfo = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
    struct timeval recvsec;
	uint32_t time = 0;
	uint8_t tmp[10];
    uint8_t addr[20];
	uint8_t ttl = 0;

	struct icmpv4_hdr *ricmph = (struct icmpv4_hdr *)icmp_pkt;
	struct iphdr *riph = (struct iphdr *)((char *)ricmph + sizeof(struct icmpv4_hdr) + 4);
	struct udphdr *rudph = (struct udphdr *)((char *)riph + sizeof(struct iphdr));

	struct udphdr *tudph = NULL;
	uint32_t tdip = 0;
	
	PING_ZLOG_DEBUG();

	time_get_time (TIME_CLK_REALTIME, &recvsec);
	for (ALL_LIST_ELEMENTS(preq_list, pnode, pnextnode, pingInfo))
	{
		tudph = pingInfo->data;
		tdip = pingInfo->destip.addr.ipv4;

		PING_ZLOG_DEBUG("tsport:0x%x rsport:0x%x tdport:0x%x rdport:0x%x rsip:0x%x tdip:%x rdip:%x\n",
						tudph->sport,rudph->sport,tudph->dport,rudph->dport,riph->saddr,tdip,ntohl(riph->daddr));
		
		if ((riph->protocol == IP_P_UDP) && (tdip == ntohl(riph->daddr)) 
			&&(tudph->sport == rudph->sport) && (tudph->dport == rudph->dport))
		{
			PING_ZLOG_DEBUG("trace rcv icmp pkt!udp->dport:%x\n",rudph->dport);

			pingInfo->stats->recvcnt++;
            time = timeval_elapsed (recvsec, pingInfo->sendsec);
            //list_delete_node(preq_list, pnode);

			if (pingInfo->type == TRACE_UDP_CMD)
			{
				pingInfo->stats->lastrecv = 1;
				ttl = (ntohs(rudph->dport) - TRACE_BASE_DPORT)/(pingInfo->count) + 1;
				TRACE_SET_SPRINT_TTL(ttl, pingInfo, tmp);
                sprintf((char *)tmp,"%4dms ",time/1000);
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);
				pingInfo->stats->ip = ipcb->sip;

				PING_ZLOG_DEBUG("sip:0x%x pingInfo->dip:0x%x senctn:%d cnt:%d\n",ipcb->sip,
					  pingInfo->destip.addr.ipv4,pingInfo->stats->sendcnt, pingInfo->count);

				if (pingInfo->stats->sendcnt == pingInfo->count)
				{
					TRACE_SET_SPRINT_IP(ipcb->sip, pingInfo, addr);
				}
				
				//trace_udp_send_next(pingInfo);
			}
			return ;
		}
	}
}


/**
 * @brief      <+process udp trace local intf ip+>
 * @param[in ] <+udp packet and ip control+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 2:12:04 PM
 * @note       <+none+>
 */
void trace_udp_rcv_localip(uchar *udp_pkt, struct ip_control *ipcb)
{
	struct icmphdr *icmph = NULL;
	struct iphdr *iph = NULL;
	struct udphdr *udph = NULL;
	uint8_t *icmp_pkt = NULL;
	
	PING_ZLOG_DEBUG();

	icmp_pkt = XCALLOC(MTYPE_PING, sizeof(struct icmpv4_hdr) + 4 + IP_HEADER_SIZE + sizeof(struct udphdr));
	icmph = (struct icmphdr *)icmp_pkt;
	iph = (struct iphdr *)((uchar *)icmph + sizeof(struct icmpv4_hdr) + 4);
	udph = (struct udphdr *)((uchar *)iph + IP_HEADER_SIZE);

	icmph->type = ICMP_TYPE_UNREACH;
	icmph->code = ICMP_CODE_PORT;

	iph->saddr = htonl(ipcb->sip);
	iph->daddr = htonl(ipcb->dip);
	iph->protocol = IP_P_UDP;

	memcpy(udph, udp_pkt, sizeof(struct udphdr));
	trace_udp_rcv_reply((struct icmphdr *)icmp_pkt, ipcb);

	XFREE(MTYPE_PING, icmp_pkt);
	
}


/**
 * @brief      <+ipv6 str check+>
 * @param[in ] <+ipv6 str+>
 * @param[out] <+none+>
 * @return     <+ERRNO_xxx+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 2:17:25 PM
 * @note       <+none+>
 */
int ndp_ipv6_str_check(const char *src)
{
    int colon = 0;

    if (NULL == src)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    while(*src != '\0')
    {
        if((*src++ == ':') && (++colon > 7))
        {
            return ERRNO_FAIL;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      <+packet register for recv packet+>
 * @param[in ] <+none+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 2:17:25 PM
 * @note       <+none+>
 */
void icmp_ping_pkt_register(void)
{
	union proto_reg proto;
    
    memset(&proto, 0, sizeof(union proto_reg));
	proto.ipreg.protocol = IP_P_ICMP;
	pkt_register(MODULE_ID_PING, PROTO_TYPE_IPV4, &proto);
}

/**
 * @brief      <+ping module init+>
 * @param[in ] <+none+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wednesday, February 28, 2018 2:19:20 PM
 * @note       <+none+>
 */
void icmp_ping_init ( void )
{
	icmp_ping_pkt_register();	
    ptimer = 0;
    preq_list = list_new();
}

