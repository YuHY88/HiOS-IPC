/**
 * @file      : cfm_lb.c
 * @brief     : define of 802.1ag and Y.1731 lb
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:32:21
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
#include <lib/log.h>

#include "lib/pkt_buffer.h"
#include "lib/hptimer.h"

#include "cfm_session.h"
#include "l2_if.h"

#include "cfm.h"
#include "cfm_lb.h"

#include <lib/zassert.h>
#include "../vlan.h"

//static int cfm_lb_timer_thread(struct thread *thread);
static int cfm_send_lbm(struct cfm_sess *sess);


extern struct thread_master *l2_master;

/**
 * @brief      : lbm报文初始化函数
 * @param[in ] : sess，session指针
 * @param[in ] : dmac，目的mac指针
 * @param[out] : 
 * @return     : 成功返回lbm数据结构指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

static struct cfm_lb *cfm_lb_init (struct cfm_sess *sess,uchar *dmac)
{
	struct cfm_lb  *lb 	   = NULL;
	uint8_t 		mac[6] = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x30};
	struct cfm_ma  *pma    = NULL;

	if (NULL == sess || NULL == dmac)
	{
		return NULL; 
	}

	pma = cfm_ma_lookup(sess->ma_index);
	if(!pma)
		return NULL;

	lb = (struct cfm_lb *)XMALLOC(MTYPE_TMP, sizeof(struct cfm_lb));
	if (NULL == lb)
	{
		return NULL;
	}

	memset (lb, 0, sizeof(struct cfm_lb));

	lb->lbm_priority 	 = pma->priority;
	lb->lbm_type         = CFM_UCAST_FRAME; 
	lb->lbm_tx_count     = 5;
	lb->lbm_tx_interval  = 2;
	lb->lbr_rx_timeout   = 2;
	lb->lbm_len          = 64;
	lb->next_trans_id    = 0;

	lb->lbr_rx_count     = 0;

	lb->lbm_pdu.trans_id = 0;
	//lb->lbm_pdu.end_tlv  = 0;

	lb->lbm_pdu.cfm_header.level             = sess->level;
	lb->lbm_pdu.cfm_header.version           = 0;
	lb->lbm_pdu.cfm_header.opcode            = CFM_LOOPBACK_MESSAGE_OPCODE;
	lb->lbm_pdu.cfm_header.first_tlv_offset  = 4;

	if (CFM_MCAST_FRAME == lb->lbm_type)
	{
		mac[5] = mac[5] | sess->level;
		memcpy(&lb->lbm_pdu.ethhdr.h_dest,   mac, 6);
	}
	else
	{
		memcpy(&lb->lbm_pdu.ethhdr.h_dest,	 dmac, 6);
	}

	memcpy(&lb->lbm_pdu.ethhdr.h_source, sess->smac, 6);

	lb->lbm_pdu.ethhdr.h_tpid      = htons(0x8100);
	lb->lbm_pdu.ethhdr.h_vlan_tci  = htons((lb->lbm_priority << 13) | sess->vlan);
	lb->lbm_pdu.ethhdr.h_ethtype   = htons(0x8902);

	return lb;
}

/**
 * @brief      : lbm报文发送定时器
 * @param[in ] : thread，定时器指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */
#if 0
int cfm_lb_timer_thread(struct thread *thread)
{
    struct cfm_sess *sess = (struct cfm_sess *)(THREAD_ARG(thread));
	struct cfm_lb 	*lb   = NULL;
	struct vty 		*vty  = NULL;
	int				 ret  = 0;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	lb = sess->lb;
	if (NULL == lb)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	vty = lb->vty;

	/* LBR 接收超时 */
	if(lb->lbr_rx_time.tv_sec < lb->lbm_tx_time.tv_sec)
	{
		vty_out(vty, "%sCFM LB(ID=%d) timeout ! %s", (lb->lbm_tx_count == 4)?VTY_NEWLINE:"",ntohl(lb->lbm_pdu.trans_id) - 1,VTY_NEWLINE);
	}
	
	if (0 == lb->lbm_tx_count)
	{
		/* LB 测试结束 */
		vty_out(vty, "Packet: Sent = %d,  Receive = %d! %s", 
						lb->next_trans_id, lb->lbr_rx_count,VTY_NEWLINE);
		if (0 != lb->lbr_rx_count)
		{
			vty_out(vty, "Frame Delay: min/max/avg = %d/%d/%d ms %s",
						lb->delay_min, lb->delay_max, lb->delay_sum/lb->lbr_rx_count,VTY_NEWLINE);
		}
		
		vtysh_return(vty, CMD_SUCCESS);
		
		XFREE (MTYPE_TMP, lb);
		sess->lb 		= NULL;
		sess->plb_timer = NULL;
		sess->lb_enable = 0;
	}
	else
	{
		/* 继续发送LBM */
		ret = cfm_send_lbm (sess);
		if (ERRNO_SUCCESS != ret)
		{
			vty_out(vty, "CFM LB(ID=%d) send failed ! %s", lb->lbm_pdu.trans_id - 1,VTY_NEWLINE);
			XFREE (MTYPE_TMP, lb);
			sess->lb 		= NULL;
			sess->plb_timer = NULL;
			sess->lb_enable = 0;

			return ERRNO_PKT_SEND; 
		}
	}

	return ERRNO_SUCCESS;
}
#endif

static int cfm_lb_timer_out(  void *ses)
{
	struct cfm_lb 	*lb   = NULL;
	struct vty 		*vty  = NULL;
	int				 ret  = 0;
	struct cfm_sess  *sess = NULL;

	if (NULL == ses)
	{
		return ERRNO_FAIL; 
	}
	
	sess = (struct cfm_sess *)ses;
	lb = sess->lb;
	if (NULL == lb)
	{
		return ERRNO_FAIL; 
	}

	vty = lb->vty;

	/* LBR 接收超时 */
	if(lb->lbr_rx_time.tv_sec < lb->lbm_tx_time.tv_sec)
	{
		vty_out(vty, "%sCFM LB(ID=%d) timeout ! %s", (lb->lbm_tx_count == 4)?VTY_NEWLINE:"",ntohl(lb->lbm_pdu.trans_id) - 1,VTY_NEWLINE);
	}
	
	if (0 == lb->lbm_tx_count)
	{
		/* LB 测试结束 */
		vty_out(vty, "Packet: Sent = %d,  Receive = %d! %s", 
						lb->next_trans_id, lb->lbr_rx_count,VTY_NEWLINE);
		if (0 != lb->lbr_rx_count)
		{
			vty_out(vty, "Frame Delay: min/max/avg = %d/%d/%d ms %s",
						lb->delay_min, lb->delay_max, lb->delay_sum/lb->lbr_rx_count,VTY_NEWLINE);
		}
		
		vtysh_return(vty, CMD_SUCCESS);
	}
	else
	{
		/* 继续发送LBM */
		ret = cfm_send_lbm (sess);
		if (ERRNO_SUCCESS == ret)
		{
			return ERRNO_SUCCESS;
		}
		
		vty_out(vty, "CFM LB(ID=%d) send failed ! %s", lb->lbm_pdu.trans_id - 1,VTY_NEWLINE);
	}

	XFREE (MTYPE_TMP, lb);
	sess->lb		= NULL;
	sess->lb_enable	= 0;
	sess->plb_timer	= 0;

	return ERRNO_SUCCESS;
}

/**
 * @brief      : lbm报文发送函数
 * @param[in ] : sess，session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_send_lbm(struct cfm_sess *sess)
{
	struct raw_control rawcb;
	struct cfm_lb 	  *lb = NULL;
	struct timeval	   time;
	int				   ret = 0;
	struct listnode   *p_listnode = NULL;
	void 			  *data = NULL;
	char 			  timerName[20] = "CfmLbTimer";

	lb = sess->lb;
	if (NULL == lb)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	time_get_time (TIME_CLK_REALTIME, &time); 

	memset (&rawcb, 0, sizeof(struct raw_control));
	rawcb.ethtype    = 0x8902;
	rawcb.priority   = lb->lbm_priority;
	rawcb.ttl        = 255;

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
					ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lb->lbm_pdu, lb->lbm_len);
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

	ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lb->lbm_pdu, lb->lbm_len);		
	zlog_debug(CFM_DBG_LB, "%s:'%s', lb pkt send rv(%d)\n", __FILE__, __func__, ret);
	if(ERRNO_SUCCESS != ret)
	{
		return ERRNO_PKT_SEND; 
	}

end:
	lb->lbm_tx_time		 = time;
	lb->lbm_pdu.trans_id = htonl(ntohl(lb->lbm_pdu.trans_id)+1);
	lb->lbm_tx_count--;
	lb->next_trans_id++;
	
    //sess->plb_timer 	 = thread_add_timer(l2_master, cfm_lb_timer_thread, sess, lb->lbm_tx_interval);
	sess->plb_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, cfm_lb_timer_out, sess, lb->lbm_tx_interval * 1000);

	zlog_debug(CFM_DBG_LB, "%s:'%s', %s(%lu) add interval(%d)\n", __FILE__, __func__, timerName, sess->plb_timer, lb->lbm_tx_interval);

	return ERRNO_SUCCESS;
}

/**
 * @brief      : lbm报文接收处理函数
 * @param[in ] : sess，session指针
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_lbm(struct cfm_sess *sess, struct pkt_buffer *pkt)
{
	struct raw_control rawcb;
	struct eth_control ethcb;
	struct cfm_lb_pdu  lbr_pdu;
	char			   name[50];
	struct cfm_ma 	  *pma 			 = NULL;
	uint8_t 		   dmac_multi[6] = {0x01,0x80,0xc2,0x00,0x00,0x30};
	int	 			   ret 			 = 0;
	

	zlog_debug(CFM_DBG_LB, "%s:Entering the function of '%s'",__FILE__,__func__);

	if((NULL == sess) || (NULL == pkt))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	pma = cfm_ma_lookup(sess->ma_index);
	if(!pma)
		return ERRNO_FAIL;

	dmac_multi[5] += sess->level;

	if(pkt->cb.ethcb.dmac[0]&0x01)
	{
		if(memcmp(pkt->cb.ethcb.dmac,dmac_multi,6))
		{
			zlog_debug(CFM_DBG_LB, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_LB, "multicast dmac:%02X:%02X:%02X:%02X:%02X:%02X\n",dmac_multi[0],
				dmac_multi[1],
				dmac_multi[2],
				dmac_multi[3],
				dmac_multi[4],
				dmac_multi[5]);
		
			zlog_err("%s:session %d receive lbm but not match",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}
	else
	{
		if(memcmp(pkt->cb.ethcb.dmac,sess->smac,6))
		{	
			zlog_debug(CFM_DBG_LB, "target mac:%02X:%02X:%02X:%02X:%02X:%02X\n",pkt->cb.ethcb.dmac[0],
				pkt->cb.ethcb.dmac[1],
				pkt->cb.ethcb.dmac[2],
				pkt->cb.ethcb.dmac[3],
				pkt->cb.ethcb.dmac[4],
				pkt->cb.ethcb.dmac[5]);
			
			zlog_debug(CFM_DBG_LB, "smac:%02X:%02X:%02X:%02X:%02X:%02X\n",sess->smac[0],
				sess->smac[1],
				sess->smac[2],
				sess->smac[3],
				sess->smac[4],
				sess->smac[5]);
		
			zlog_err("%s:session %d receive lbm but not match",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}


	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_LB, "receive cfm lbm from interface:%s, vpn:%d, date_len:%d \n",name,pkt->vpn,pkt->data_len);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_LB, "physical port is:%s\n",name);

	if(sess->direct == MEP_DIRECT_DOWN || (sess->direct == MEP_DIRECT_UP && sess->over_pw))
	{
		if(pkt->in_ifindex != sess->ifindex)
		{
			zlog_err("%s:session %d receive lbm but interface not match",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}
	else
	{
		if(pkt->in_ifindex == sess->ifindex)
		{
			zlog_err("%s:normal mode, session %d receive lbm but interface same",__func__,sess->sess_id);
			return ERRNO_UNMATCH;		
		}
	}


	memset (&lbr_pdu, 0, sizeof(struct cfm_lb_pdu));

	/* judge the lbm, dmac & level &in_ifindex */


	memset (&rawcb, 0, sizeof(struct raw_control));
	memset (&ethcb, 0, sizeof(struct eth_control));

	memcpy (&ethcb, &pkt->cb.ethcb, sizeof(struct eth_control));
	memcpy (&lbr_pdu.cfm_header, pkt->data,pkt->data_len);

	memcpy(&lbr_pdu.ethhdr.h_dest, &ethcb.smac, 6);
	memcpy(&lbr_pdu.ethhdr.h_source, &sess->smac, 6);

	lbr_pdu.ethhdr.h_tpid      = htons(0x8100);
	lbr_pdu.ethhdr.h_vlan_tci  = htons((pma->priority << 13) | sess->vlan);
	lbr_pdu.ethhdr.h_ethtype   = htons(0x8902);

	lbr_pdu.cfm_header.opcode = CFM_LOOPBACK_REPLY_OPCODE;

	rawcb.ethtype    = 0x8902;
	rawcb.priority   = pma->priority;
	rawcb.ttl        = 255;

	if (MEP_DIRECT_UP == sess->direct && sess->over_pw)
	{
		/* Only when for pw, up mep is exit */
		rawcb.in_ifindex = sess->ifindex;
	}
	else
	{
		rawcb.out_ifindex = pkt->in_ifindex;
	}
	
	ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lbr_pdu, pkt->data_len + sizeof(struct vlan_ethhdr));		

	return ret;
}

/**
 * @brief      : lbr报文接收处理函数
 * @param[in ] : sess，session指针
 * @param[in ] : pkt，接收报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_rcv_lbr(struct cfm_sess *sess, struct pkt_buffer *pkt) 
{
	struct vty  		*vty 	= NULL;
	uint32_t 			 delay  = 0;
	struct cfm_lb 		*lb 	= NULL;
	struct cfm_lb_pdu 	 lbr_rcv_pdu;
	struct timeval 		 time;
	char 				 name[50];

	zlog_debug(CFM_DBG_LB, "%s:Entering the function of '%s'",__FILE__,__func__);
	
	if((NULL == sess) || (NULL == pkt))
	{
		return ERRNO_PARAM_ILLEGAL;
	}

	if(memcmp(pkt->cb.ethcb.dmac,sess->smac,6))
	{
		zlog_err("%s:session %d receive lbr but not match",__func__,sess->local_mep);
		return ERRNO_UNMATCH;		
	}

	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_LB, "receive cfm lbr from interface:%s, vpn:%d \n",name,pkt->vpn);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_LB, "physical port is:%s\n",name);

	lb = sess->lb;

	/* 未发起LB测试，或者测试已经结束 */
	if(NULL == lb)
	{
		zlog_err("lb is NULL\n");
	
		return ERRNO_NOT_FOUND;
	}

	vty = lb->vty;
	if(NULL == vty)
	{
		zlog_err("vty is NULL\n");
	
		return ERRNO_NOT_FOUND;
	}

	memset (&lbr_rcv_pdu, 0, sizeof(struct cfm_lb_pdu));

	time_get_time (TIME_CLK_REALTIME, &time); 
	lb->lbr_rx_time = time;

	/* judge the lbr, dmac & in_ifindex */


	memcpy (&lbr_rcv_pdu.cfm_header, pkt->data,pkt->data_len);
	lb->lbr_rx_count++;

	delay 		   = timeval_elapsed (lb->lbr_rx_time, lb->lbm_tx_time);
	delay 		   = delay/1000;

	lb->delay_sum += delay;

	if(0 == lb->delay_min)
	{
		lb->delay_min = delay;
	}
	else if (lb->delay_min > delay)
	{
		lb->delay_min = delay;
	}

	if(lb->delay_max < delay)
	{
		lb->delay_max = delay;
	}

	vty_out(vty, "%sReply from %02x:%02x:%02x:%02x:%02x:%02x, time = %dms %s", 
							(lb->lbr_rx_count == 1)?VTY_NEWLINE:"",
							pkt->cb.ethcb.smac[0], pkt->cb.ethcb.smac[1],
							pkt->cb.ethcb.smac[2], pkt->cb.ethcb.smac[3],
							pkt->cb.ethcb.smac[4], pkt->cb.ethcb.smac[5],
							delay,VTY_NEWLINE);


	return ERRNO_SUCCESS;
}

/**
 * @brief      : lb发起函数
 * @param[in ] : sess，session指针； 
 * @param[in ] : vty，vtysh指针；
 * @param[in ] : dmac，目的mac指针；
 * @param[in ] : size，报文大小；
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_lb_start (struct cfm_sess *sess, void  *vty,uchar *dmac,uint32_t size)
{
	struct cfm_lb  *lb = NULL;
	int 			ret = 0;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	/* session 使能以后才能进行 LB 测试 */
	if (OAM_STATUS_DISABLE == sess->state)
	{
		return ERRNO_CONFIG_INCOMPLETE;
	}

	/* 因为LB、LT共用一个定时器，不能同时进行测试 */
	if ((1 == sess->lt_enable) || (1 == sess->lb_enable))
	{
		return ERRNO_EXISTED;
	}

	lb = cfm_lb_init(sess,dmac);
	if (NULL == lb)
	{
		return ERRNO_MALLOC;
	}

	sess->lb_enable = 1;
	sess->lb        = lb;
	lb->vty         = vty;
	lb->lbm_len 	= size;

	/* 启动发送LBM */
	ret = cfm_send_lbm(sess);
	if (ERRNO_SUCCESS != ret)
	{
		XFREE (MTYPE_TMP, lb);
		sess->lb		= NULL;
		sess->lb_enable	= 0;
		sess->plb_timer	= 0;
	}
	
	return ret;
}

/**
 * @brief      : lb结束函数
 * @param[in ] : sess，session指针； 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 17:53:21
 * @note       : 
 */

int cfm_lb_stop(struct cfm_sess *sess)
{
	struct cfm_lb *lb = NULL;

	if (NULL == sess)
	{
		return ERRNO_PARAM_ILLEGAL; 
	}

	if  (1 != sess->lb_enable)
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
	
	lb = sess->lb;
	if (NULL != lb)
	{
		XFREE (MTYPE_TMP, lb);
		sess->lb = NULL;
	}

	sess->lb_enable = 0;

	return ERRNO_SUCCESS;
}

