
#include <zebra.h>
#include <lib/command.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/errcode.h>
#include <lib/checksum.h>
#include <lib/log.h>

#include "ping6.h"


/**
 * @brief      <+icmpv6 ping set ip control+>
 * @param[in ] <+ip control pointer,ping_info struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
void icmpv6_ping_set_ipcb(struct ip_control *ip6cb,struct ping_info *pingInfo)
{    
    memset(ip6cb, 0, sizeof(struct ip_control));

	IPV6_ADDR_COPY(ip6cb->sipv6.ipv6, pingInfo->srcip.addr.ipv6);	
	IPV6_ADDR_COPY(ip6cb->dipv6.ipv6, pingInfo->destip.addr.ipv6);	
	ip6cb->protocol = IP_P_IPV6_ICMP;
	ip6cb->is_changed = 0x1;
	ip6cb->vpn = pingInfo->vpn;	
    ip6cb->chsum_enable = 1;
    ip6cb->tos = 2;
    ip6cb->is_changed = 1;
    ip6cb->ifindex = pingInfo->ifindex;

    if (PING6_CMD == pingInfo->type)
    {
        ip6cb->ttl = pingInfo->ttl;
    }
    else if (TRACE6_CMD == pingInfo->type)
    {
        ip6cb->ttl = 1;
    }
    
}


/**
 * @brief      <+icmpv6 ping set icmpv6 header+>
 * @param[in ] <+icmpv6_pkt  pointer,ping_info struct+>
 * @param[out] <+none+>
 * @return     <+packet size from icmpv6 header to packet end+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
static int icmpv6_ping_set_icmpv6hdr(struct icmpv6_req *icmpv6_pkt,struct ping_info *pingInfo)
{
    uint32_t packetsize = 0;
    uint8_t  tmpval = 0x61;
    static uint16_t id = 0;
    uint16_t i = 0;

    packetsize = ICMPV6_PING_HDR_SIZE + pingInfo->size ;

    /*icmp 请求报文内容填充*/
    icmpv6_pkt->hdr.type = ICMPV6_TYPE_ECHO;  /* icmpv6的类型 */
    icmpv6_pkt->hdr.code = 0;            	 /* icmpv6的编码 */
    icmpv6_pkt->hdr.checksum = 0;       	 /* icmpv6的校验和 */
    icmpv6_pkt->sequence = htons(1);     /* icmpv6的顺序号 */
    icmpv6_pkt->id  = htons(++id);       /* icmpv6的标志符 */

    /*从小写a 开始到小写z 循环填充负载*/
	for(i = 0; i < pingInfo->size ; i++)
	{
		icmpv6_pkt->data[i] = tmpval++;
		if(tmpval == 0x78)
			tmpval = 0x61;
	}
    icmpv6_pkt->hdr.checksum = in_checksum((u_int16_t *)icmpv6_pkt, packetsize);

    pingInfo->data = icmpv6_pkt;

    return packetsize;
}


/**
 * @brief      <+print ping result on screen and free memory+>
 * @param[in ] <+ping_info struct+>
 * @param[out] <+icmpv6 ping result+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
static void icmpv6_ping_stats_vtyout(struct ping_info *pingInfo)
{
    uint32_t sendcnt,recvcnt;

    sendcnt = pingInfo->stats->sendcnt;
    recvcnt = pingInfo->stats->recvcnt;

    vty_out(pingInfo->pvty, "---PING6 statistics----\r\n");   
    vty_out(pingInfo->pvty, "%d packets transmitted, %d received , %d%% lost\r\n",
                            sendcnt,recvcnt,(sendcnt - recvcnt)*100/sendcnt);

    vtysh_return(pingInfo->pvty,CMD_SUCCESS);
    
    /*释放icmp 请求包、控制信息及统计信息结构体*/ 
    XFREE(MTYPE_PING,pingInfo->stats->buffer);
    XFREE(MTYPE_PING,pingInfo->stats);
    XFREE(MTYPE_PING,pingInfo->data);
    XFREE(MTYPE_PING,pingInfo);

}


/**
 * @brief      <+process icmpv6 request packet+>
 * @param[in ] <+icmpv6 request packet+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
void icmpv6_ping_recv_request(struct pkt_buffer *pkt)
{
    struct ip_control *ip6cb = NULL;
    struct icmpv6_req *icmpv6_echo = NULL;
    struct icmpv6_req *icmpv6_reply = NULL;
    struct ip_control  ip6cb_reply;
    int ret = 0;

    if(!pkt) 
    {
        PING_ZLOG_DEBUG("Err argument!pkt is null!\n");
        return;
    }
    
    PING_ZLOG_DEBUG();
    
    ip6cb = (struct ip_control *)(&(pkt->cb));
    icmpv6_echo = (struct icmpv6_req *)(pkt->data);

    //收到icmpv6请求报文回复应答
    icmpv6_reply = XMALLOC(MTYPE_PING, pkt->data_len);
    memset(&ip6cb_reply, 0, sizeof(struct ip_control));
    memcpy(icmpv6_reply, icmpv6_echo, pkt->data_len);
    icmpv6_reply->hdr.type = ICMPV6_TYPE_REPLY;
    icmpv6_reply->hdr.code = 0;
    IPV6_ADDR_COPY(&(ip6cb_reply.sipv6), &(ip6cb->dipv6));

    //ping设备本身，ftm无法填充源ip
    if(ipv6_is_valid(&(ip6cb->sipv6)))
        IPV6_ADDR_COPY(&(ip6cb_reply.dipv6), &(ip6cb->sipv6));
    else
        IPV6_ADDR_COPY(&(ip6cb_reply.dipv6), &(ip6cb->dipv6));

	//linlocal地址的ping请求从报文入接口发回应答报文
	if(ipv6_is_linklocal((struct ipv6_addr *)(ip6cb->sipv6.ipv6)))
	{
		ip6cb_reply.ifindex = pkt->in_ifindex;
	}

    ip6cb_reply.ttl = 255;
    ip6cb_reply.chsum_enable = 1;
    ip6cb_reply.vpn = ip6cb->vpn;
    ip6cb_reply.protocol = IP_P_IPV6_ICMP;
	ip6cb_reply.is_changed = 1;

    ret = pkt_send(PKT_TYPE_IPV6, (union pkt_control *)(&ip6cb_reply), icmpv6_reply, pkt->data_len);
	if( 0 != ret )
	{
        PING_ZLOG_ERR("pkt_send fail!\n");
		return;		
	}
    
    PING_ZLOG_DEBUG("ret:%d pkt->data_len:%d\n", ret, pkt->data_len);
    
    XFREE(MTYPE_PING, icmpv6_reply);

	
}


/**
 * @brief      <+process icmpv6 reply packet+>
 * @param[in ] <+icmpv6 reply packet、ip control and packet len+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
void icmpv6_ping_recv_reply(struct icmpv6_req *icmpv6_reply,struct ip_control *ip6cb,uint32_t datalen)
{
    struct ping_info *pingInfo = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
    struct icmpv6_req   *icmpv6_req = NULL;
    struct timeval recvsec;
    uint8_t addr[IPV6_ADDR_STRLEN];
    uint32_t time;
    uint16_t reply_seq,reply_id;
   	uint8_t ping_pkt_flag = 0;
	
    reply_seq = ntohs(icmpv6_reply->sequence);
    reply_id = ntohs(icmpv6_reply->id);
    inet_ipv6tostr(&(ip6cb->sipv6), (char *)addr, IPV6_ADDR_STRLEN);
    time_get_time (TIME_CLK_REALTIME, &recvsec);
    
    PING_ZLOG_DEBUG("ip6cb->sipv6:0x%x seq:%d ttl:%d\n",ip6cb->sip,reply_seq,ip6cb->ttl);
    
    /* 收到icmpv6应答报文，从链表删除对应的报文控制信息*/
	for (ALL_LIST_ELEMENTS(preq6_list, pnode, pnextnode, pingInfo))
	{
	    icmpv6_req = (struct icmpv6_req *)(pingInfo->data);
		if((reply_id == ntohs(icmpv6_req->id)) && (reply_seq == ntohs(icmpv6_req->sequence)))
		{  
            PING_ZLOG_DEBUG("icmp_ping_recv_reply! pingInfo:%p seq:%d\n",pingInfo,reply_seq);

            /*ping 收到TIMEOUT 报文不做处理*/
            if(PING6_CMD == pingInfo->type && ICMP_TYPE_TIMEOUT == icmpv6_reply->hdr.type)
            {
                PING_ZLOG_DEBUG("ping6 recv timeout datagram.\n");
                break;
            }

            time = timeval_elapsed (recvsec, pingInfo->sendsec);
            
            //list_delete_node(preq6_list, pnode);

            /*收到上一个报文应答，发送下一个请求包*/
            if (PING6_CMD == pingInfo->type)
            {
            	if( pingInfo->stats->lastrecv != 1 )
        		{
        			pingInfo->stats->lastrecv = 1;
					pingInfo->stats->recvcnt++;
            		vty_out(pingInfo->pvty,"Reply from %s: bytes=%d Sequence=%d ttl=%d time:%dms\r\n",
                                    addr,datalen,reply_seq,ip6cb->ttl,time/1000);
        		}
            	
				ping_pkt_flag = 1;
                //icmpv6_ping_send_next(pingInfo);
            }
            else if (TRACE6_CMD == pingInfo->type)
            {
                #if 0
                /*设置trace 回显ip*/
                ICMP_TRACE_SPRINT_TTL(icmp_reply,pingInfo,(char *)tmp);
                sprintf((char *)tmp,"%4dms ",time/1000);
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);
                if(inet_valid_ipv4(ipcb->sip))
                    pingInfo->stats->ip = ipcb->sip;
                ICMP_TRACE_SPRINT_IP(icmp_reply,pingInfo,(char *)addr);

                /*每跳结束回应vty*/ 
                ICMP_TRACE_VTYOUT(icmp_reply,pingInfo);

                icmp_trace_send_next(pingInfo);
                #endif
            }
            if(ping_pkt_flag == 0)
        	{
				list_delete_node(preq6_list, pnode);
			}
            return ;
		}
	}
    
    PING_ZLOG_DEBUG("preq6_list->count:%d\n", preq6_list->count);
 
}


/* 检查ping request 是否超时*/

/**
 * @brief      <+request packet timer thread+>
 * @param[in ] <+thread master+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
int icmpv6_ping_timer_thread(struct thread *thread)
{
	struct ping_info *pingInfo = NULL;
	struct listnode  *pnode  = NULL;
	struct listnode  *pnextnode = NULL; 
    struct icmpv6_req   *icmpv6_pkt = NULL;
    struct timeval   time;
    uint32_t wait_time;
	uint8_t ping_pkt_flag = 0;
	
    /* 找到超时的icmp request 报文，从链表删除*/
	for (ALL_LIST_ELEMENTS(preq6_list, pnode, pnextnode, pingInfo))
	{  
        time_get_time (TIME_CLK_REALTIME, &time);
        wait_time =  timeval_elapsed (time, pingInfo->sendsec);
        icmpv6_pkt = (struct icmpv6_req *)(pingInfo->data);
        PING_ZLOG_DEBUG("pingInfo:%p icmpv6_pkt->seq:%d,pingInfo->time:%d wait_time:%d\n", 
                              pingInfo, ntohs(icmpv6_pkt->sequence), pingInfo->time, wait_time);
        
		if (wait_time > (pingInfo->time)*1000*1000)
		{
            /*上一个报文超时，发送下一个请求包*/
            if (PING6_CMD == pingInfo->type)
            {     
				if( pingInfo->stats->lastrecv == 0 )
        		{
        			if(wait_time > (pingInfo->waittime)*1000*1000)
        			{
        				vty_out(pingInfo->pvty,"ping ip timeout!seq:%d...\r\n",ntohs(icmpv6_pkt->sequence));
						list_delete_node(preq6_list, pnode);
						icmpv6_ping_send_next(pingInfo);
						break;
					}
				}
				else
				{
					list_delete_node(preq6_list, pnode);
					icmpv6_ping_send_next(pingInfo);
					break;
				}
				ping_pkt_flag = 1;
            }
            else if (TRACE6_CMD == pingInfo->type)
            {
                #if 0
                /*每跳打印ttl 及超时打印星号*/
                ICMP_TRACE_SPRINT_TTL(icmp_req,pingInfo,(char *)tmp);
                sprintf((char *)tmp, "%5s", " * ");
                strcat((char *)(pingInfo->stats->buffer),(char *)tmp);

                /*每跳结束回应vty*/
                ICMP_TRACE_VTYOUT(icmp_req,pingInfo);
                
                icmp_trace_send_next(pingInfo);
                #endif
            }
			if( ping_pkt_flag != 1 ) 
            	list_delete_node(preq6_list, pnode);
            break;
		}
        else
        {
           //vty_out(pingInfo->pvty,"ping ip waiting...\r\n");
        }

	}

    PING_ZLOG_DEBUG("preq6_list->count:%d\n", preq6_list->count);

    if (preq6_list->count)
		//thread_add_timer(ping_master, icmpv6_ping_timer_thread, 0, 1); /* 起1s 定时器*/
		high_pre_timer_add("ping6Timer",LIB_TIMER_TYPE_NOLOOP, icmpv6_ping_timer_thread, NULL, 1*1000);
	else
		p6timer = 0;

	return 0;
    
}



/**
 * @brief      <+icmpv6 ping send next request packet+>
 * @param[in ] <+pingInfo struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
void icmpv6_ping_send_next(struct ping_info *pingInfo)
{
    struct icmpv6_req *icmpv6_pkt = NULL;
    struct icmpv6_req *next_pkt = NULL;
    struct ip_control pkt_ctrl;
    uint16_t icmpv6_seq = 0;
    uint32_t packetsize = ICMP_HEADER_SIZE + pingInfo->size;;
    uint8_t  ret;
    struct vty *pvty = pingInfo->pvty;

    PING_ZLOG_DEBUG("pingInfo:%p", pingInfo->data);

    /*ping ip 结束输出统计信息，释放内存*/
    icmpv6_pkt = pingInfo->data;
    if(ntohs(icmpv6_pkt->sequence) >= pingInfo->count
        ||(pvty->sig_int_recv == 1))
    {
        pvty->sig_int_recv = 0;
        icmpv6_ping_stats_vtyout(pingInfo);
        return ;
    }
    
    /*设置ip 层 控制信息及报文内容*/
    icmpv6_ping_set_ipcb(&pkt_ctrl,pingInfo); 
    next_pkt = icmpv6_pkt;
    icmpv6_seq = ntohs(next_pkt->sequence);
    next_pkt->sequence = htons(++icmpv6_seq);
    next_pkt->hdr.checksum = 0;
    next_pkt->hdr.checksum = in_checksum((uint16_t *)next_pkt,packetsize);
    
    /*发送icmp 请求报文*/
	ret = pkt_send(PKT_TYPE_IPV6, (union pkt_control *)&pkt_ctrl, next_pkt, packetsize);
	if( 0 != ret )
	{
		PING_ZLOG_ERR("Pkt_send fail!\n");
		return ;		
	}
    pingInfo->stats->sendcnt++;
	pingInfo->stats->lastrecv = 0;
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));

    
    /*添加报文控制信息到链表，起定时器超时检测*/
	listnode_add(preq6_list, pingInfo); 
    if( 0 == p6timer )
    {
        //p6timer = thread_add_timer(ping_master, icmpv6_ping_timer_thread, 0, 1);
		p6timer = high_pre_timer_add("ping6Timer",LIB_TIMER_TYPE_NOLOOP, icmpv6_ping_timer_thread, NULL, 1*1000);
    }

    PING_ZLOG_DEBUG("pingInfo:%p preq6_list->count:%d\n", pingInfo, preq6_list->count);
    
}


/**
 * @brief      <+icmpv6 ping send request packet+>
 * @param[in ] <+pingInfo struct+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
int icmpv6_ping_send_request(struct ping_info *pingInfo)
{
    struct icmpv6_req *icmpv6_pkt = NULL;
    struct ip_control pktcontrol;
    int packetsize;	
	int ret;

    icmpv6_pkt = XMALLOC(MTYPE_PING, sizeof(struct icmpv6_req) + pingInfo->size);
    if ( NULL == icmpv6_pkt )
    {
        return ERRNO_MALLOC;
    }

    /*设置ip 层 控制信息*/
    icmpv6_ping_set_ipcb(&pktcontrol,pingInfo);   

    /*填充icmp 报文头及负载*/
    packetsize = icmpv6_ping_set_icmpv6hdr(icmpv6_pkt,pingInfo);

    /*发送icmp 请求报文*/
	ret = pkt_send(PKT_TYPE_IPV6, (union pkt_control *)&pktcontrol, icmpv6_pkt, packetsize);
	if( 0 != ret )
	{
        PING_ZLOG_ERR("pkt_send fail!\n");
		return ERRNO_FAIL;		
	}

    /*发包统计*/
    pingInfo->stats->sendcnt++;
    
    /*获取报文发送时间*/
    time_get_time (TIME_CLK_REALTIME, &(pingInfo->sendsec));
    
    /*添加报文控制信息到链表，起定时器超时检测*/
	listnode_add(preq6_list, pingInfo); 
    if( 0 == p6timer )
    {
        //p6timer = thread_add_timer(ping_master, icmpv6_ping_timer_thread, 0, 1);
		p6timer = high_pre_timer_add("ping6Timer",LIB_TIMER_TYPE_NOLOOP, icmpv6_ping_timer_thread, NULL, 1*1000);
    }

    PING_ZLOG_DEBUG("pingInfo:%p preq6_list->count:%d\n", pingInfo, preq6_list->count);

    return 0;
}




/**
 * @brief      <+register packet to ftm+>
 * @param[in ] <+none+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+none+>
 */
void icmpv6_ping_pkt_register(void)
{
	union proto_reg proto;

    //ping6 请求报文    
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.icmpv6_type_valid = 1;
	proto.ipreg.protocol = IP_P_IPV6_ICMP;
    proto.ipreg.icmpv6_type = ICMPV6_TYPE_ECHO;
    proto.ipreg.type = PROTO_TYPE_IPV6;
	pkt_register(MODULE_ID_PING, PROTO_TYPE_IPV6, &proto);

    //ping6应答报文
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.icmpv6_type_valid = 1;
	proto.ipreg.protocol = IP_P_IPV6_ICMP;
    proto.ipreg.icmpv6_type = ICMPV6_TYPE_REPLY;
    proto.ipreg.type = PROTO_TYPE_IPV6;
	pkt_register(MODULE_ID_PING, PROTO_TYPE_IPV6, &proto);
    
}



/**
 * @brief      <+icmpv6 ping init+>
 * @param[in ] <+none+>
 * @param[out] <+none+>
 * @return     <+none+>
 * @author     jinlei sun
 * @date       Wed Apr 11 14:25:40 CST 2018
 * @note       <+register packet 、init timer and init list+>
 */
void icmpv6_ping_init ( void )
{
	icmpv6_ping_pkt_register();	
    p6timer = 0;
    preq6_list = list_new();
}




