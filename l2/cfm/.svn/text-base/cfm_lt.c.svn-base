/**
 * @file      : cfm_lt.c
 * @brief     : define of 802.1ag and Y.1731 lt
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:33:34
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */

#include <string.h>
#include <stdlib.h>
#include <lib/hash1.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/thread.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include "lib/msg_ipc.h"
#include <lib/alarm.h>
#include <lib/command.h>
#include <lib/vty.h>

#include "lib/pkt_buffer.h"
#include "lib/log.h"

#include "cfm_session.h"
#include "l2_if.h"

#include "cfm.h"
#include "cfm_lt.h"

#include <lib/zassert.h>
#include "../vlan.h"

//static int cfm_lt_timer_thread(struct thread *thread);
static int cfm_send_ltm(struct cfm_sess *sess);


extern struct thread_master *l2_master;

/**
 * @brief      : ltm报文初始化函数
 * @param[in ] : sess，session指针
 * @param[in ] : dmac，目的mac指针
 * @param[out] : 
 * @return     : 成功返回ltm数据结构指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
static struct cfm_lt *cfm_lt_init(struct cfm_sess *sess,uchar *dmac)
{
	struct cfm_lt 	*lt 	= NULL;
	uint8_t			 mac[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x38};
	struct cfm_ma 	*pma 	= NULL;
	unsigned char 	*p 		= NULL;

	if (NULL == sess || NULL == dmac)
	{
		return NULL; 
	}

	pma = cfm_ma_lookup(sess->ma_index);
	if(!pma)
		return NULL;

	lt = (struct cfm_lt *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_lt));
	if (NULL == lt)
	{
		return NULL;
	}
	
	memset (lt, 0, sizeof(struct cfm_lt));
	
	lt->priority		= pma->priority;
	lt->ttl             = 64;
	lt->ltr_rx_timeout  = 2;
	lt->next_trans_id   = 0;

	memcpy(&lt->target_mac, dmac, 6);

	lt->ltm_pdu.cfm_header.level             = sess->level;
	lt->ltm_pdu.cfm_header.version           = 0;
	lt->ltm_pdu.cfm_header.opcode            = CFM_LINKTRACE_MESSAGE_OPCODE;
	lt->ltm_pdu.cfm_header.flag              = 0x01 << 7;  // HWonly
	lt->ltm_pdu.cfm_header.first_tlv_offset  = 17;

	lt->ltm_pdu.trans_id  = 0;
	lt->ltm_pdu.ttl       = 64;
	memcpy(&lt->ltm_pdu.origin_mac, sess->smac, 6);
	memcpy(&lt->ltm_pdu.target_mac, dmac, 6);

	mac[5] = mac[5] | sess->level;
	memcpy(&lt->ltm_pdu.ethhdr.h_dest,   mac, 6);
	memcpy(&lt->ltm_pdu.ethhdr.h_source, sess->smac, 6);

	lt->ltm_pdu.ethhdr.h_tpid      = htons(0x8100);
	lt->ltm_pdu.ethhdr.h_vlan_tci  = htons((lt->priority << 13) | sess->vlan);
	lt->ltm_pdu.ethhdr.h_ethtype   = htons(0x8902);

	p = lt->ltm_pdu.tlvs;
	
	//tlvs
	*p++ = LTM_Egress_Identifier_Tlv;
	
	*p++ = 0;
	*p++ = 8;	
	
	*p++ = 0;
	*p++ = 0;
	memcpy(p,sess->smac,6);
	p += 6;	

	//tlvs endtlv
	*p = 0;

	lt->remain_len = lt->ltm_pdu.tlvs+199-p;
		
	return lt;
}

/**
 * @brief      : ltm报文发送定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
int cfm_lt_timer_thread(struct thread *thread)
{
    struct cfm_sess *sess = (struct cfm_sess *)(THREAD_ARG(thread));
	struct cfm_lt 	*lt   = NULL;
	struct vty		*vty  = NULL;
	int 			 ret  = 0;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	lt = sess->lt;
	if (NULL == lt)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	vty = lt->vty;

	/* LTR 接收超时 */
	if(lt->ltr_rx_time.tv_sec < lt->ltm_tx_time.tv_sec)
	{
		vty_out(vty, "%sCFM LT timeout ! %s",VTY_NEWLINE,VTY_NEWLINE);
	}
	
	vtysh_return(vty, CMD_SUCCESS);

	/* LT 测试结束 */
	XFREE (MTYPE_TMP, lt);
	sess->lt 		= NULL;
	sess->plb_timer = NULL;
	sess->lt_enable = 0;

	return ret;
}
#endif

static int cfm_lt_timer_out(void *ses)
{
	struct cfm_lt 	*lt   = NULL;
	struct vty		*vty  = NULL;
	int 			 ret  = 0;
	struct cfm_sess * sess = NULL;

	if (NULL == ses)
	{
		return ret; 
	}
	sess = (struct cfm_sess *)ses;
	lt = sess->lt;
	if (NULL == lt)
	{
		return ret; 
	}

	vty = lt->vty;

	/* LTR 接收超时 */
	if(lt->ltr_rx_time.tv_sec < lt->ltm_tx_time.tv_sec)
	{
		vty_out(vty, "%sCFM LT timeout ! %s",VTY_NEWLINE,VTY_NEWLINE);
	}
	
	vtysh_return(vty, CMD_SUCCESS);

	/* LT 测试结束 */
	XFREE (MTYPE_TMP, lt);
	sess->lt 		= NULL;
	sess->plb_timer	= 0;
	sess->lt_enable	= 0;
	
	return ret;
}

/**
 * @brief      : ltm报文发送函数
 * @param[in ] : sess，session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_send_ltm(struct cfm_sess *sess)
{
	struct raw_control rawcb;
	struct timeval	   time;
	struct cfm_lt	  *lt 		  = NULL;
	int 			   ret 		  = 0;
	struct listnode   *p_listnode = NULL;
	void 			  *data 	  = NULL;
	char 			  timerName[20] = "CfmLtTimer";

	lt = sess->lt;
	if (NULL == lt)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	time_get_time (TIME_CLK_REALTIME, &time); 

	memset (&rawcb, 0, sizeof(struct raw_control));

	rawcb.ethtype    = 0x8902;
	rawcb.priority   = lt->priority;
	rawcb.ttl        = 64;

	if (MEP_DIRECT_UP == sess->direct)
	{
		/* Only when for pw */
		if(sess->over_pw)
		{
			rawcb.in_ifindex = sess->ifindex;
		}
		else
		{
			//for switch
			for(ALL_LIST_ELEMENTS_RO(&(vlan_table[sess->vlan]->portlist),p_listnode,data))
			{
				if(((uint32_t)data) != sess->ifindex)
				{				
					rawcb.out_ifindex = (uint32_t)data;
					ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lt->ltm_pdu, sizeof(struct cfm_ltm_pdu) - lt->remain_len);		
					if(ERRNO_SUCCESS != ret)
					{
						return ERRNO_PKT_SEND; 
					}
				}
			}

			goto end;
			
		}
	}
	else if (MEP_DIRECT_DOWN == sess->direct)
	{
		rawcb.out_ifindex = sess->ifindex;
	}
	else
	{
		return ERRNO_CONFIG_INCOMPLETE;
	}

	ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lt->ltm_pdu, sizeof(struct cfm_ltm_pdu) - lt->remain_len);		
	if(ERRNO_SUCCESS != ret)
	{
		return ERRNO_PKT_SEND; 
	}

end:
	
	lt->ltm_tx_time   = time;
	lt->ltm_pdu.trans_id = htonl(lt->ltm_pdu.trans_id++);

	/* 启动LT超时定时器 */	
    //sess->plb_timer 	 = thread_add_timer(l2_master, cfm_lt_timer_thread, sess, lt->ltr_rx_timeout);

	sess->plb_timer  = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, cfm_lt_timer_out, sess, lt->ltr_rx_timeout * 1000);

	zlog_debug(CFM_DBG_LT, "%s:'%s', %s(%lu) add interval(%d)\n", __FILE__, __func__, timerName, sess->plb_timer, lt->ltr_rx_timeout);
	
	return ERRNO_SUCCESS;
}

/**
 * @brief      : ltr报文发送函数
 * @param[in ] : ltr_pdu，ltr报文结构体变量
 * @param[in ] : rawcb，报文发送控制结构
 * @param[in ] : len，报文剩余长度
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int cfm_send_ltr(struct cfm_ltr_pdu ltr_pdu, struct raw_control rawcb,unsigned char len)
{
	int ret = 0;

	zlog_debug(CFM_DBG_LT, "[%s] Reply a lbr pdu.\n\r", __func__);
	ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ltr_pdu, sizeof(struct cfm_ltr_pdu)-len);		

	return ret;
}

/**
 * @brief      : lt发起函数
 * @param[in ] : sess，session指针； 
 * @param[in ] : vty，vtysh指针；
 * @param[in ] : dmac，目的mac指针；
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_lt_start (struct cfm_sess *sess, void *vty,uchar *dmac)
{
	struct cfm_lt *lt  = NULL;
	int 		   ret = 0;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	/* session 使能以后才能进行 LT 测试 */
	if (OAM_STATUS_DISABLE == sess->state)
	{
		return ERRNO_CONFIG_INCOMPLETE;
	}

	/* 因为LB、LT共用一个定时器，不能同时进行测试 */
	if ((1 == sess->lt_enable) || (1 == sess->lb_enable))
	{
		return ERRNO_EXISTED;
	}

	lt = cfm_lt_init(sess,dmac);
	if (NULL == lt)
	{
		return ERRNO_MALLOC;
	}

	sess->lt_enable = 1;
	sess->lt        = lt;
	lt->vty         = vty;

	/* 启动发送 LTM */
	ret = cfm_send_ltm(sess);
	if (ERRNO_SUCCESS != ret)
	{
		XFREE (MTYPE_TMP, lt);
		sess->lt		= NULL;
		sess->plb_timer	= 0;
		sess->lt_enable	= 0;
	}

	return ret;
}

/**
 * @brief      : lt结束函数
 * @param[in ] : sess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_lt_stop(struct cfm_sess *sess)
{
	struct cfm_lt *lt = NULL;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	if  (1 != sess->lt_enable)
	{
		return ERRNO_NOT_FOUND; 
	}

#if 0
	if (NULL != sess->plb_timer)
	{
		THREAD_TIMER_OFF(sess->plb_timer);
		sess->plb_timer = NULL;
	}
#endif

	if (sess->plb_timer)
	{
		high_pre_timer_delete(sess->plb_timer);
		sess->plb_timer = 0;
	}
	
	lt = sess->lt;

	if (NULL != lt)
	{
		XFREE (MTYPE_TMP, lt);
		sess->lt = NULL;
	}

	sess->lt_enable = 0;

	return ERRNO_SUCCESS;
}

/**
 * @brief      : ltm报文接收处理函数
 * @param[in ] : sess，session指针
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_ltm(struct cfm_sess *sess, struct pkt_buffer *pkt)
{
	struct raw_control  rawcb;
	struct eth_control  ethcb;
	struct cfm_ltr_pdu  ltr_pdu;
	char 				name[50];
	struct cfm_ltm_pdu  ltm_pdu;
	struct cfm_ma 	   *pma = NULL;
	unsigned char 	   *p 	= ltr_pdu.tlvs;
	unsigned char 		len = 0;
	int 				ret = 0;
	
	zlog_debug(CFM_DBG_LT, "%s:Entering the function of '%s'",__FILE__,__func__);

	if((NULL == sess) || (NULL == pkt))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	pma = cfm_ma_lookup(sess->ma_index);
	if(!pma)
		return 0;

	memset (&ltm_pdu, 0, sizeof(struct cfm_ltm_pdu));
	/* copy cfm_header & trans_id & ttl */
	/*memcpy (&ltm_pdu.cfm_header, pkt->data, 
						sizeof(struct cfm_ltm_pdu) - sizeof(struct vlan_ethhdr));*/

	memcpy (&ltm_pdu.cfm_header, pkt->data, pkt->data_len);


	if(memcmp(ltm_pdu.target_mac,sess->smac,6))
	{	
		zlog_debug(CFM_DBG_LT, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",ltm_pdu.target_mac[0],
			ltm_pdu.target_mac[1],
			ltm_pdu.target_mac[2],
			ltm_pdu.target_mac[3],
			ltm_pdu.target_mac[4],
			ltm_pdu.target_mac[5]);
		
		zlog_debug(CFM_DBG_LT, "smac:%02X:%02X:%02X:%02X:%02X:%02X\n",sess->smac[0],
			sess->smac[1],
			sess->smac[2],
			sess->smac[3],
			sess->smac[4],
			sess->smac[5]);
	
		zlog_err("%s:session %d receive ltm but not match",__func__,sess->local_mep);
		return ERRNO_UNMATCH;		
	}

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_LT, "physical port is:%s\n",name);

	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_LT, "receive cfm ltm from interface:%s, vpn:%d \n",name,pkt->vpn);

	
	if(sess->direct == MEP_DIRECT_DOWN || (sess->direct == MEP_DIRECT_UP && sess->over_pw))
	{
		if(pkt->in_ifindex != sess->ifindex)
		{
			zlog_err("%s:session %d receive ltm but interface not match",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}
	else
	{
		if(pkt->in_ifindex == sess->ifindex)
		{
			zlog_err("%s:normal mode, session %d receive ltm but interface same",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}

	memset (&ltr_pdu, 0, sizeof(struct cfm_ltr_pdu));

	/* judge the lbm, dmac & level &in_ifindex */


	memset (&rawcb, 0, sizeof(struct raw_control));
	memset (&ethcb, 0, sizeof(struct eth_control));

	memcpy (&ethcb, &pkt->cb.ethcb, sizeof(struct eth_control));

	/* copy cfm_header & trans_id & ttl */
	memcpy (&ltr_pdu.cfm_header, pkt->data,pkt->data_len);

	ltr_pdu.cfm_header.first_tlv_offset = 6;
	ltr_pdu.cfm_header.opcode           = CFM_LINKTRACE_REPLY_OPCODE;
	ltr_pdu.cfm_header.flag 		  	= 0xa0;
	ltr_pdu.action                      = CFM_LTM_RELAY_HIT;
	ltr_pdu.ttl 					   -=1;

	//memcpy(&ltr_pdu.ethhdr.h_dest, &ethcb.smac, 6);
	memcpy(ltr_pdu.ethhdr.h_dest,   ltm_pdu.origin_mac, 6);
	memcpy(ltr_pdu.ethhdr.h_source, sess->smac, 6);

	ltr_pdu.ethhdr.h_tpid      = htons(0x8100);
	ltr_pdu.ethhdr.h_vlan_tci  = htons((pma->priority << 13) | sess->vlan);
	ltr_pdu.ethhdr.h_ethtype   = htons(0x8902);

	rawcb.ethtype    = 0x8902;
	rawcb.priority   = pma->priority;
	rawcb.ttl        = 64;

	//tlvs
	*p++ = LTR_Egress_Identifier_Tlv;
	
	*p++ = 0;
	*p++ = 16;	
	
	*p++ = 0;
	*p++ = 0;
	memcpy(p,ethcb.smac,6);
	p   += 6;
	*p++ = 0;
	*p++ = 0;
	memset(p,0,6);
	p   += 6;

	//tlvs
	*p++ = Reply_Ingress_Tlv;
	
	*p++ = 0;	
	*p++ = 9+strlen(name);
	
	*p++ = 1;
	memcpy(p,sess->smac,6);
	p   += 6;
	*p++ = strlen(name);//sublen
	*p++ = 5; //subtype
	strcpy((char *)p,name);
	p   += strlen(name);
	
	//tlvs
	*p   = 0;

	len  = ltr_pdu.tlvs+199-p;//remain count
		
	if (MEP_DIRECT_UP == sess->direct && sess->over_pw)
	{
		/* Only when for pw, up mep is exit */
		rawcb.in_ifindex = sess->ifindex;
	}
	else
	{
		rawcb.out_ifindex = pkt->in_ifindex;
	}
	
	ret = cfm_send_ltr (ltr_pdu, rawcb,len);

	return ret;
}

/**
 * @brief      : relay action转换函数
 * @param[in ] : relay_action，报文中的action字段
 * @param[out] : str，转换后的字符串
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static int cfm_relay_action_str(uint8_t relay_action,char *str)
{	
	if(str == NULL)
	{
		strcpy(str, "Unknown Relay Action");
		return ERRNO_FAIL;
	}
	
	switch (relay_action)
	{
		case CFM_LTM_RELAY_HIT:
			strcpy(str, "RlyHit");
			break;
		case CFM_LTM_RELAY_FDB:
			strcpy(str, "RlyFDB");
			break;
		case CFM_LTM_RELAY_MPDB:
			strcpy(str, "RlyMDB");
			break;

		default:
			strcpy(str, "Unknown Relay Action");
			break;
	}

	return ERRNO_SUCCESS;
}

/**
 * @brief      : ltr报文接收处理函数
 * @param[in ] : sess，session指针
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_ltr(struct cfm_sess *sess, struct pkt_buffer *pkt) 
{
	char 				buf[18] = {0};
	struct vty  		*vty 	= NULL;
	struct cfm_lt 		*lt 	= NULL;
	struct cfm_ltr_pdu  ltr_rcv_pdu;
	struct timeval		time;
	char 				str[50];
	uint8_t   			hops;

	zlog_debug(CFM_DBG_LT, "%s:Entering the function of '%s'",__FILE__,__func__);

	
	if((NULL == sess) || (NULL == pkt))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	if(memcmp(pkt->cb.ethcb.dmac,sess->smac,6))
	{	
		zlog_err("%s:session %d receive ltr but not match",__func__,sess->local_mep);
		return ERRNO_UNMATCH;		
	}

	ifm_get_name_by_ifindex(pkt->in_ifindex,str);
	zlog_debug(CFM_DBG_LT, "receive cfm ltr from interface:%s, vpn:%d \n",str,pkt->vpn);

	ifm_get_name_by_ifindex(pkt->in_port,str);
	zlog_debug(CFM_DBG_LT, "physical port is:%s\n",str);

	lt = sess->lt;

	/* 未发起LT测试，或者测试已经结束 */
	if(NULL == lt)
	{
		return ERRNO_NOT_FOUND;
	}

	vty = lt->vty;
	if(NULL == vty)
	{
		return ERRNO_NOT_FOUND;
	}

	memset (&ltr_rcv_pdu, 0, sizeof(struct cfm_ltr_pdu));

	/*memcpy (&ltr_rcv_pdu.cfm_header, pkt->data, 
						sizeof(struct cfm_ltr_pdu) - sizeof(struct vlan_ethhdr));*/

	memcpy (&ltr_rcv_pdu.cfm_header, pkt->data,pkt->data_len);

	time_get_time (TIME_CLK_REALTIME, &time); 
	lt->ltr_rx_time = time;


	hops = sess->lt->ttl - ltr_rcv_pdu.ttl;

	/* judge the ltr, dmac & in_ifindex */


	if(hops == 1)
	{
		sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", 
							sess->lt->target_mac[0], sess->lt->target_mac[1],
							sess->lt->target_mac[2], sess->lt->target_mac[3],
							sess->lt->target_mac[4], sess->lt->target_mac[5]);
	
		vty_out(vty, "%sTracing the route to %s over a maximum %d hops:%s", VTY_NEWLINE ,buf, sess->lt->ttl,VTY_NEWLINE);
		vty_out(vty, "-------------------------------------------------------------------------%s",VTY_NEWLINE);
		vty_out(vty, " %-10s %-30s %-12s %s","Hops","Mac","Relay Action",VTY_NEWLINE);
	}

	sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", 
							pkt->cb.ethcb.smac[0], pkt->cb.ethcb.smac[1],
							pkt->cb.ethcb.smac[2], pkt->cb.ethcb.smac[3],
							pkt->cb.ethcb.smac[4], pkt->cb.ethcb.smac[5]);

	cfm_relay_action_str(ltr_rcv_pdu.action,str);

	vty_out(vty, " %-10d %-30s %-12s %s", 	hops , buf,	str,VTY_NEWLINE);
	
	vty_out(vty, "-------------------------------------------------------------------------%s",VTY_NEWLINE);
	
	return ERRNO_SUCCESS;
}

