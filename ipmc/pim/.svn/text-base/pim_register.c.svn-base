/*
*   PIM register.c
*   PIM register pdu, register-stop pdu, register-null pdu, timer.
*/

#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/zebra.h>
#include <lib/checksum.h>
#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/ifm_common.h>

#include <ftm/pkt_ip.h>
#include "../ipmc_main.h"
#include "../ipmc_public.h"
#include "../ipmc_if.h"
//#include "../ipmc_msg.h"
#include "pim.h"
#include "pim_pkt.h"
#include "pim_mroute.h"
#include "pim_register.h"
#include "pim_join_prune.h"


/*Could register requires 3 conditions
*  1. I_am_DR(RPF_interface(S));
*  2. DirectlyConnected(S);
*  3. KeepaliveTimer(S,G) is running
*/
/*
bool CouldRegister(S,G) {
         return ( I_am_DR( RPF_interface(S) ) AND
                  KeepaliveTimer(S,G) is running AND
                  DirectlyConnected(S) == TRUE )
      }

*/
	
uint8_t directly_connected_src(uint32_t in_port, uint32_t sip)
{
	struct ifm_l3 l3if;
	int ret = 0;
	
	//l3if = pim_get_if_ip(MODULE_ID_IPMC, in_port);
	ret = ifm_get_l3if(in_port, MODULE_ID_IPMC, &l3if);
	if (ret)
	{
		return PIM_FALSE;
	}

	/*DirectlyConnected(S)*/
	if ((l3if.ipv4[0].prefixlen && ipv4_is_same_subnet(sip, l3if.ipv4[0].addr, l3if.ipv4[0].prefixlen)) ||
		(l3if.ipv4[1].prefixlen && ipv4_is_same_subnet(sip, l3if.ipv4[1].addr, l3if.ipv4[1].prefixlen)))
	{
		return PIM_TRUE;
	}

	return PIM_FALSE;
}

uint8_t CouldRegisterSG(struct pim_mrt_sg *mrt_sg, uint32_t in_port)
{
	struct ipmc_if * pim_if = NULL;
	uint8_t could_register = PIM_FALSE;

	if (!mrt_sg || !(mrt_sg->rp_addr))
	{
		IPMC_LOG_ERROR("mrt_sg == NULL || rp_addr == NULL\n");
		return PIM_FALSE;
	}

	if (!(mrt_sg->flag & PIM_MRT_SG))
	{
		IPMC_LOG_ERROR("mrt_sg->flag = %x\n", mrt_sg->flag);
		return PIM_FALSE;
	}

	/*Group is in PIM-SSM range, don't seng register message.*/
	if (pim_instance_global->pim_conf.ssm_state && IN_PIM_SSM_RANGE(mrt_sg->grp_addr))
	{
		IPMC_LOG_ERROR("mrt_sg(%x, %x) in ssm range\n", mrt_sg->src_addr, mrt_sg->grp_addr);
		return PIM_FALSE;
	}
		

	pim_if = ipmc_if_lookup(in_port);
	if (!pim_if || !pim_if->pim_nbr)
	{
		IPMC_LOG_NOTICE("in_ifindex %x not pim if\n", in_port);
		return PIM_FALSE;
	}

	/*DirectlyConnected(S) && */
	/*I_am_DR(RPF_interface(S)) &&*/
	/*keepalive_timer is running*/
	/*I_am_not_RP*/
	if((mrt_sg->flag & PIM_MRT_LOCAL) && (pim_if->pim_nbr->cur_dr.dr_state & PIM_DR_LOCAL) 
		&& mrt_sg->keepalive_timer && !(ip4_addr_local_main(mrt_sg->rp_addr)))
	{
		could_register = PIM_TRUE;
	}
#if 0
	if ((ipv4_is_same_subnet(mrt_sg->src_addr, l3if->ipv4[0].addr, l3if->ipv4[0].prefixlen) ||
		ipv4_is_same_subnet(mrt_sg->src_addr, l3if->ipv4[1].addr, l3if->ipv4[1].prefixlen)) &&
		(pim_if->pim_nbr->cur_dr.dr_addr == mrt_sg->src_addr) &&
		mrt_sg->keepalive_timer)
		
	{
		could_register = PIM_TRUE;
	}
#endif
	
	return could_register;
}

/* Pseudo code:
	if ( I_am_RP(G) && outer.dst == RP(G) ) {
		sent_Register_Stop == FALSE;
		if ( SPTbit (S,G) || 
			(Switch_To_Spt_Desired(S,G) && (inherited_olist(S,G) == NULL))) {
		send Register-Stop to outer.src
		sent_Register_Stop = TRUE;
		}
		if (SPTbit(S,G) || Switch_To_Spt_Desired(S,G)) {
		if (sent_Register_Stop == TRUE) {
			set Keep_alive_Timer(S,G) to RP_keepalive_period;
		} else {
			set Keep_alive_Timer(S,G) to keepalive_period;
		}
		}
		if ( !SPTbit(S,G) && !pkt.Null_Register_Bit) {
		decapsulate and forward the inner packet to
		inherited_olist(S,G,rpt)
		}
	} else {
	sent Register-Stop(S,G) to outer.src
	}
*/
#if 0
sint32 pim_register_null_process( u_int32_t outer_dip, uint32_t outer_sip, uint8_t outer_ip_tos, 
							uint32_t inner_sip, uint32_t inner_grp, uint32_t is_border, uint32_t is_null)
{
	struct pim_mrt_sg * pim_mrt_sg_t = NULL;

	if (NULL == pim_instance_global)
		return ERRNO_FAIL;
	
	/*if the inner header checksum is non-zero, then check the checksum
	*  bad checksum message++;
	*/
	
	if ((!ipv4_is_valid(inner_sip)) ||
		(!ipv4_is_multicast(inner_grp)) ||
		IN_PIM_SSM_RANGE(inner_grp))
	{
		/*#if 0
		return pim_register_stop_send(outer_dip, outer_sip, outer_ip_tos, 
																inner_sip, inner_grp);
		#else
		return ERRNO_FAIL;
		#endif*/
	}

	/*RP refresh register-stop information at the DR*/
	pim_mrt_sg_t = pim_mrt_sg_get( inner_sip, inner_grp, IPMC_FALSE);
	
	/*check if I am RP for that group, if not send register_stop*/
	if ((pim_mrt_sg_t == NULL) ||
		(!pim_mrt_sg_t->rp_addr) ||
		(!pim_rp_check( inner_grp)) ||
		(outer_dip != pim_mrt_sg_t->rp_addr))
	{
		/*send register-stop(S,G) to outer.src*/
		pim_register_stop_send(outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp);
		pim_instance_global->pim_stat.pimstat_rx_reg_not_rp++;
		return ERRNO_SUCCESS;
	}

	if ((pim_mrt_sg_t->flag & PIM_MRT_SG) &&
		(pim_mrt_sg_t->flag & PIM_MRT_SPT))
	{
		/*send register-stop(S,G) to outer.src*/
		pim_register_stop_send(outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp);

		/*restart keepalivetimer(S,G) to keepalive_period*/
		pim_mrt_sg_t->keepalive_time = PIM_KEEPALIVE_PERIOD_DEFAULT;

		pim_mrt_sg_keepalive_timer_set(pim_mrt_sg_t, PIM_TRUE);
		return ERRNO_SUCCESS;
	}

	return ERRNO_SUCCESS;
}


/*start a register-stop timer to maintain this state
*  timer = ();
	 restart the Register-Suppression timer 
	SET_TIMER(mrtentry->rs_timer, (0.5 * PIM_REGISTER_SUPPRESSION_TIMEOUT)
			+ (RANDOM() % (PIM_REGISTER_SUPPRESSION_TIMEOUT + 1)));
*/
sint32 pim_register_stop_process( uint32_t grp, uint32_t sip, uint8_t grp_mask_len)
{
	struct pim_mrt_sg * pim_mrt_sg_t = NULL;
	struct pim_mrt_sg_grp * mrt_sg_grp = NULL;
	uint32_t index = 0;
	struct hash_bucket *bucket = NULL;

	zlog_debug("%s[%d]: enter function '%s'.\n", __FILE__, __LINE__, __func__);

	if (NULL == pim_instance_global)
	{
		return ERRNO_FAIL;
	}

	if ( (!ipv4_is_multicast(grp)) ||
		(!ipv4_is_valid(sip)) ||
		!sip)
	{
		return ERRNO_FAIL;
	}

	/*lookup route, do register_stop for all grp address
	*  need check for sip==0, not hanging off of (*,G);
	*  need route lookup route by (sip, grp);
	*/
	
	/*(S,G) register-stop*/
	if (sip)
	{
		/*(s,g) register-stop, not create*/
		pim_mrt_sg_t = pim_mrt_sg_get( sip, grp, IPMC_FALSE);
		
		if (NULL == pim_mrt_sg_t)
		{
			/*Don't have (S,G) state, ignore*/
			pim_instance_global->pim_stat.pimstat_unknown_rs++;
			return ERRNO_FAIL;
		}

		if (! (pim_mrt_sg_t->flag & PIM_MRT_SG))
			return ERRNO_SUCCESS;

		switch(pim_mrt_sg_t->reg_state)
		{
			/*is_register_noinfo_state, if Noinfo, ignore*/
			/*ignore register prune state*/
			case PIM_REGISTER_NOINFO_STATE:
			case PIM_REGISTER_PRUNE_STATE:
				return ERRNO_SUCCESS;
			/*register Join state ->register prune state*/
			case PIM_REGISTER_JOIN_STATE:
				return pim_register_stop_join_state_process(pim_mrt_sg_t);
			/*register join-pending state -> register prune state*/
			case PIM_REGISTER_JOIN_PENDING_STATE:
				return pim_register_stop_jp_state_process(pim_mrt_sg_t);
			
			default:
				return ERRNO_SUCCESS;
		}
	}

	/*(*, G) register stop
	* apply to all (S,G) entres for this group that are not in the noinfo
	*/

	mrt_sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), grp);
	if((NULL == mrt_sg_grp) || (!mrt_sg_grp->sg_num))
	{
		/*Don't have (S,G) state, ignore*/
		pim_instance_global->pim_stat.pimstat_unknown_rs++;
		return ERRNO_FAIL;
	}

	for (index = 0; index < mrt_sg_grp->sg_table.hash_size; index++)
	{
		for (bucket = mrt_sg_grp->sg_table.buckets[index]; bucket != NULL; bucket = bucket->next)
		{
			if (!bucket || !(bucket->data))
				continue;

			pim_mrt_sg_t = (struct pim_mrt_sg *)bucket->data;
			
			if (! (pim_mrt_sg_t->flag & PIM_MRT_SG))
				continue;

			switch(pim_mrt_sg_t->reg_state)
			{
				/*is_register_noinfo_state, if Noinfo, ignore*/
				/*ignore register prune state*/
				case PIM_REGISTER_NOINFO_STATE:
				case PIM_REGISTER_PRUNE_STATE:
					break;
				/*register Join state ->register prune state*/
				case PIM_REGISTER_JOIN_STATE:
					pim_register_stop_join_state_process(pim_mrt_sg_t);
					break;
				/*register join-pending state -> register prune state*/
				case PIM_REGISTER_JOIN_PENDING_STATE:
					pim_register_stop_jp_state_process(pim_mrt_sg_t);
					break;
				
				default:
					break;
			}
		}
	}

	return ERRNO_SUCCESS;
}
#endif

/*If the register-stop timer actually expires, 
*  the DR will resume send register message from src to RP
*/
/*	sint32 pim_register_stop_timer_expire(struct thread *thread)*/
sint32 pim_register_stop_timer_expire(void *para)
{
	struct pim_mrt_sg * mrt_sg = NULL;
	
	if (!para)
	{
		IPMC_LOG_ERROR("para invalid\n");
		return ERRNO_SUCCESS;
	}

	/*	mrt_sg = (struct pim_mrt_sg *)THREAD_ARG(thread);*/
	mrt_sg = (struct pim_mrt_sg *)para;
	mrt_sg->rs_timer = 0;

	pim_reg_state_machine(mrt_sg, PIM_REG_RST_EXPIRE);
	
	return ERRNO_SUCCESS;
}

void pim_reg_noinfo(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	switch(event)
	{
		case PIM_REG_COULD_REG:
			mrt_sg->flag |= PIM_MRT_REG;
			/*J state,add reg tunnel*/
			mrt_sg->reg_state = PIM_REG_STATE_J;
			/*数据报文从注册口出去，封装成注册报文*/
			break;
		default:
			break;
	}
}

void pim_reg_join(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	switch(event)
	{
		case PIM_REG_COULD_NOT_REG:
			mrt_sg->flag &= ~PIM_MRT_REG;
			/*NI state,remove reg tunnel*/
			mrt_sg->reg_state = PIM_REG_STATE_NI;
			break;
		case PIM_REG_REG_STOP_RCV:
			/*P state,remove reg tunnel,set reg_stop timerr(0.5~1.5,set 1, so 60s)*/
			mrt_sg->reg_state = PIM_REG_STATE_P;
			if(mrt_sg->rs_timer)
			{
				high_pre_timer_delete(mrt_sg->rs_timer);
				/*thread_cancel(mrt_sg->rs_timer);*/
			}
			/*mrt_sg->rs_timer = thread_add_timer(ipmc_master, 
										pim_register_stop_timer_expire,
										(void *)mrt_sg,
										PIM_REGISTER_SUPPRESSION_TIME_DEFAULT);*/
			mrt_sg->rs_timer = high_pre_timer_add("SG_RSTimer",LIB_TIMER_TYPE_NOLOOP,
						pim_register_stop_timer_expire,(void *)mrt_sg,
						1000 * PIM_REGISTER_SUPPRESSION_TIME_DEFAULT);
			break;
		case PIM_REG_RP_CHANGE:
			/*J state, update reg tunnel*/
			mrt_sg->reg_state = PIM_REG_STATE_J;
			break;
		default:
			break;
	}
}

void pim_reg_join_pending(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	switch(event)
	{
		case PIM_REG_RST_EXPIRE:
			/*J state,add reg tunnel*/
			mrt_sg->reg_state = PIM_REG_STATE_J;
			break;
		case PIM_REG_COULD_NOT_REG:
			mrt_sg->flag &= ~PIM_MRT_REG;
			/*NI state*/
			mrt_sg->reg_state = PIM_REG_STATE_NI;
			break;
		case PIM_REG_REG_STOP_RCV:
			/*P state, set reg_stop timer(0.5~1.5,set 1, so 60s)*/
			mrt_sg->reg_state = PIM_REG_STATE_P;
			if(mrt_sg->rs_timer)
			{
				high_pre_timer_delete(mrt_sg->rs_timer);
				/*thread_cancel(mrt_sg->rs_timer);*/
			}
			/*mrt_sg->rs_timer = thread_add_timer(ipmc_master, 
										pim_register_stop_timer_expire,
										(void *)mrt_sg,
										PIM_REGISTER_SUPPRESSION_TIME_DEFAULT);*/
			mrt_sg->rs_timer = high_pre_timer_add("SG_RSTimer",LIB_TIMER_TYPE_NOLOOP,
							pim_register_stop_timer_expire,(void *)mrt_sg,
							1000 * PIM_REGISTER_SUPPRESSION_TIME_DEFAULT);
			break;
		case PIM_REG_RP_CHANGE:
			/* J state, add reg tunnel ,cancel reg_stop timer*/
			mrt_sg->reg_state = PIM_REG_STATE_J;
			if(mrt_sg->rs_timer)
			{
				high_pre_timer_delete(mrt_sg->rs_timer);
				mrt_sg->rs_timer = 0;
			}
			break;
		default:
			break;
	}
}

void pim_reg_prune(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	switch(event)
	{
		case PIM_REG_RST_EXPIRE:
			/*JP state, set reg_stop timer(5s), send null-reg*/
			mrt_sg->reg_state = PIM_REG_STATE_JP;
			if(mrt_sg->rs_timer)
			{
				high_pre_timer_delete(mrt_sg->rs_timer);
				mrt_sg->rs_timer = 0;
				/*thread_cancel(mrt_sg->rs_timer);*/
			}
			/*mrt_sg->rs_timer = thread_add_timer(ipmc_master, 
										pim_register_stop_timer_expire,
										(void *)mrt_sg,
										PIM_REGISTER_PROBE_TIME_DEFAULT);*/
			mrt_sg->rs_timer = high_pre_timer_add("SGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
							(void *)pim_register_stop_timer_expire,(void *)mrt_sg,
							1000 * PIM_REGISTER_PROBE_TIME_DEFAULT);
			/*ifindex 是远端DR*/
			pim_register_null_send( mrt_sg->upstream_ifindex, mrt_sg->rp_addr, mrt_sg->src_addr, mrt_sg->grp_addr);
			break;
		case PIM_REG_COULD_NOT_REG:
			mrt_sg->flag &= ~PIM_MRT_REG;
			/*NI state*/
			mrt_sg->reg_state = PIM_REG_STATE_NI;
			break;
		case PIM_REG_RP_CHANGE:
			/* J state, add reg tunnel ,cancel reg_stop timer*/
			mrt_sg->reg_state = PIM_REG_STATE_J;
			if(mrt_sg->rs_timer)
			{
				high_pre_timer_delete(mrt_sg->rs_timer);
				mrt_sg->rs_timer = 0;
			}
			break;
		default:
			break;
	}
}

void pim_reg_state_machine(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	if(!mrt_sg)
	{
		return ;
	}
	
    switch(mrt_sg->reg_state)
    {
        case PIM_REG_STATE_NI:
            pim_reg_noinfo(mrt_sg, event);
            break;
        case PIM_REG_STATE_J:
            pim_reg_join(mrt_sg, event);
            break;
        case PIM_REG_STATE_P:
            pim_reg_prune(mrt_sg, event);
            break;
        case PIM_REG_STATE_JP:
            pim_reg_join_pending(mrt_sg, event);
            break;
        default:
            break;
    }   
    return;
}

uint32_t pim_data_pkt_forward(struct pim_mrt_sg *mrt_sg, uint8_t *udp_data, uint32_t data_len)
{
	union pkt_control pkt_ctrl;
	uint32_t i = 0, len = 0, ifindex = 0;
	sint32 ret = ERRNO_FAIL;


	if(!mrt_sg->oif.oif_num || (mrt_sg->flag & PIM_MRT_UNUSED))
	{
		/*oiflist为空，不转发*/
		IPMC_LOG_DEBUG("(%x,%x) oif is NULL or PIM_MRT_UNUSED , flag=%x\n", mrt_sg->src_addr, mrt_sg->grp_addr, mrt_sg->flag);
		return ERRNO_SUCCESS;
	}
	/*软件转发ppkt, 将udp数据报文从指定的出接口组播转发*/
	/*ip hdr */
	for(i = 0; i < IPMC_IF_MAX; i++)
	{
		if(IF_ISSET(i, &(mrt_sg->oif.res_oif)))
		{
			ifindex = ipmc_if_s.ipmc_if_array[i];
			if(ifindex == 0)
			{
				continue;
			}
			/*udp + data*/
			memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
			memcpy(ipmc_send_buf, udp_data, data_len);
			IPMC_LOG_DEBUG("(%x,%x) oif %x,ppkt->data:%s, ppkt->data_len:%d\n", 
				mrt_sg->src_addr, mrt_sg->grp_addr, ifindex, ipmc_send_buf, data_len);

			memset(&pkt_ctrl, 0, sizeof(union pkt_control));
			pkt_ctrl.ipcb.chsum_enable = DISABLE;
			pkt_ctrl.ipcb.protocol = IP_P_UDP;
			pkt_ctrl.ipcb.ifindex = ifindex;
			pkt_ctrl.ipcb.sip = mrt_sg->src_addr;
			pkt_ctrl.ipcb.dip = mrt_sg->grp_addr;
			pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IPMC;
			pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
			pkt_ctrl.ipcb.tos = mrt_sg->rs_tos;	/*udp data's ip hdr priority*/
			pkt_ctrl.ipcb.ttl = 1;
			pkt_ctrl.ipcb.is_changed = 1;
			
			ret = pkt_send(PKT_TYPE_IPMC, &pkt_ctrl, (void *)ipmc_send_buf, data_len);
			if(ret)
			{
				zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
			}
			memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
		}
	}
	return ERRNO_SUCCESS;
}

/*(S,G) Register state: NoInfo (NI), Join (J), Prune (P), Join-Pending (JP)*/
/*if (S,G) entry or (*,G) exist, send message to G-olist, update (S,G) entry
*  if (S,G) entry not exist, 
	1. judge could register, if could register, send register message;
	2. if not, drop.
*/
/*Pseudo code*/
/*
if( iif == RPF_interface(S) AND UpstreamJPState(S,G) == Joined AND
	inherited_olist(S,G) != NULL ) {
		set KeepaliveTimer(S,G) to Keepalive_Period
}

Update_SPTbit(S,G,iif)

oiflist = NULL

if( iif == RPF_interface(S) AND SPTbit(S,G) == TRUE ) {
	oiflist = inherited_olist(S,G)
} else if( iif == RPF_interface(RP(G)) AND SPTbit(S,G) == FALSE ) {
	oiflist = inherited_olist(S,G,rpt)
	CheckSwitchToSpt(S,G)
	void
	     CheckSwitchToSpt(S,G) {
	       if ( ( pim_include(*,G) (-) pim_exclude(S,G)
	              (+) pim_include(S,G) != NULL )
	            AND SwitchToSptDesired(S,G) ) {
	              # Note: Restarting the KAT will result in the SPT switch
	              set KeepaliveTimer(S,G) to Keepalive_Period
	       }
	}
} else {
	# Note: RPF check failed.
	# A transition in an Assert finite state machine may cause an
	# Assert(S,G) or Assert(*,G) message to be sent out interface iif.
	# See Section 4.6 for details.
	if ( SPTbit(S,G) == TRUE AND iif is in inherited_olist(S,G) ) {
		send Assert(S,G) on iif
	} else if ( SPTbit(S,G) == FALSE AND
		iif is in inherited_olist(S,G,rpt) ) {
		send Assert(*,G) on iif
	}
}
oiflist = oiflist (-) iif
forward packet on all interfaces in oiflist
*/
sint32 pim_data_pkt_recv(struct pkt_buffer *ppkt)
{
	struct ip_control *ipcb = NULL;
	uint8_t could_register = PIM_FALSE;
	struct pim_mrt_sg * mrt_sg = NULL;
	struct pim_mrt_wc * mrt_wc = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t iif = ppkt->in_port;
	
	uint8_t is_directly_connected_s = PIM_FALSE;
	uint32_t directly_connected_rpf_ifindex_s = 0;
	uint32_t is_keepalive_timer_restarted = PIM_FALSE;
	struct pim_mrt_wc pim_mrt_wc_t;
	struct pim_mrt_sg * pim_mrt_sg_t = NULL;
	uint8_t is_sptbit_set = PIM_FALSE;
	uint8_t is_wrong_iif = PIM_TRUE;
	struct pim_mrt_oif_list oiflist;

	/*if pif is NULL, need multicast message to all pim_instance oiflist(S,G)?.
	*  if pif is NULL, drop this message, should enable pim to create pim_instance.
	*/
	pif = ipmc_if_lookup(iif);
	if (!pif || !pim_instance_global || pif->pim_mode == 0)
	{
		return ERRNO_FAIL;
	}

	ipcb = (struct ip_control *)&(ppkt->cb);
	if (!ipcb)
	{
		return ERRNO_FAIL;
	}
	/*如果源地址不是与接口同一个网段，不处理该报文
	if(!directly_connected_src(iif, ipcb->sip))
	{
		pim_instance_global->pim_stat.pimstat_not_same_subnet_msg++;
		return ERRNO_FAIL;
	}*/
	if (!ipv4_is_multicast(ipcb->dip))
	{
		pim_instance_global->pim_stat.pimstat_not_mcast_dip_msg++;
		return ERRNO_FAIL;
	}

	/*新创建sg时，keepalive_timer start 210s*/
	mrt_sg = pim_mrt_sg_get( ipcb->sip, ipcb->dip, IPMC_TRUE);
	if(!mrt_sg)
	{
		return ERRNO_FAIL;
	}
	mrt_sg->flag |= PIM_MRT_SG;
	/*if source is derectly connected, keepalive_timer (re)start 210s*/
	/*if directly-connected source and the iif matches, the follows must be deel with here.
	*  Start or restart keepalive-timer(S,G) to keepalive-period.
	*  Register state transition or upstreamJPstate(S,G) transition may happen as a result of restarting keepaliveTimer. 
	*/
	mrt_wc = mrt_sg->wc_mrt;
	if( !(mrt_sg->flag & PIM_MRT_UNUSED) && mrt_sg->flag & PIM_MRT_LOCAL && iif == mrt_sg->upstream_ifindex )
	{
		if(mrt_sg->keepalive_timer)
		{
			high_pre_timer_delete(mrt_sg->keepalive_timer);
			/*thread_cancel(mrt_sg->keepalive_timer);*/
		}
		/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
										pim_mrt_sg_keepalive_timer_expire,
										(void *)mrt_sg, 
										Keepalive_Period_default);*/
		mrt_sg->keepalive_timer = high_pre_timer_add("SGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
							(void *)pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
							1000 * Keepalive_Period_default);
		/*设置spt标志位*/
		Update_SPTbit(mrt_sg, iif);
		if(CouldRegisterSG(mrt_sg, ppkt->in_port))
		{
			/*could reg*/
			pim_reg_state_machine(mrt_sg, PIM_REG_COULD_REG);
		}
		else
		{
			/*could not reg*/
			/*计算出接口,upstreamjpstate transition may happen*/
			pim_mrt_sg_oif_cal(mrt_sg);
		}
	}

	/*
	we check to see whether the packet should be accepted based on
	   TIB state and the interface that the packet arrived on.
	1.
	If the packet should be forwarded using (S,G) state, we then build an
	   outgoing interface list for the packet.	If this list is not empty,
	   then we restart the (S,G) state Keepalive Timer.
	   
	if( iif == RPF_interface(S) AND UpstreamJPState(S,G) == Joined AND
		inherited_olist(S,G) != NULL ) {
			set KeepaliveTimer(S,G) to Keepalive_Period
	2.
	If the packet should be forwarded using (*,*,RP) or (*,G) state, then
	   we just build an outgoing interface list for the packet.  We also
	   check if we should initiate a switch to start receiving this source
	   on a shortest path tree.
	
	3.Process the "Data arrived" event that may trigger an Assert message.*/
	if(iif == mrt_sg->upstream_ifindex && 
		mrt_sg->jp_up_state == PIM_JP_UP_STATE_JOINED &&
		inherited_olist_sg(mrt_sg))
	{
		if(mrt_sg->keepalive_timer)
		{
			high_pre_timer_delete(mrt_sg->keepalive_timer);
			/*thread_cancel(mrt_sg->keepalive_timer);*/
		}
		/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
										pim_mrt_sg_keepalive_timer_expire,
										(void *)mrt_sg, 
									Keepalive_Period_default);*/
		mrt_sg->keepalive_timer = high_pre_timer_add("SGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
							(void *)pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
							1000 * Keepalive_Period_default);
	}
	
	/*设置spt标志位*/
	Update_SPTbit(mrt_sg, iif);

	/*根据spt和iif计算出接口*/
	if(iif == mrt_sg->upstream_ifindex && (mrt_sg->flag & PIM_MRT_SPT))
	{
		/*从spt收到数据报文，从 inherited_olist(S,G)转发流量*/
		pim_mrt_sg_oif_cal(mrt_sg);
	}
	else if(mrt_wc && iif == mrt_wc->upstream_ifindex && !(mrt_sg->flag & PIM_MRT_SPT))
	{
		/*从rpt收到数据报文且rp不在本地,inherited_olist(S,G,RPT)转发流量*/
		pim_mrt_sg_oif_cal(mrt_sg);
		/*检查是否需要进行切换,若满足切换则已经重置keepalive_timer*/
		CheckSwitchToSpt(mrt_sg);
	}
	else
	{
		/*非spt，非rpt，若从某出接口出来，可能需要发送断言*/
		/*不转发数据报文*/
		/*流量到了，向rp发送注册报文，未收到rp发送的sg加枝，出接口为空*/
		
	}
	/*数据报文走软件转发，若需要注册，则封装成注册报文*/
	if(mrt_sg->oif.oif_num)
	{
		/*从出接口转发出去*/
		pim_data_pkt_forward(mrt_sg, ppkt->data, ppkt->data_len);
	}
	else if(mrt_sg->flag & PIM_MRT_REG)
	{
		/*封装注册报文，发送给rp*/
		pim_register_send(mrt_sg->rp_addr, ipcb->tos, mrt_sg->src_addr, mrt_sg->grp_addr, ppkt, 0);
	}
	/*将表项下发到芯片,之后的流量走硬件转发,可能没有出接口*/
	if(!(mrt_sg->flag & PIM_MRT_CACHE) && mrt_sg->oif.oif_num)
	{
		pim_mrt_sg_cache_add(mrt_sg);
	}
	return ERRNO_SUCCESS;
}

/*DR send pim register message
* eth head: DMAC = RP MAC, SRC MAC = DR MAC, Type = ipv4(0x0800), fixed by ftm;
* ip head: dip = rp ip, sip = dr oif ip; protocol = PIM(103), need lookup oif and sip;
* pim head;
* payload: not include inner eth head, only inner ip head;
*/
sint32 pim_register_null_send(uint32_t ifindex, uint32_t dst, uint32_t inner_src, uint32_t inner_dst)
{
	struct ipmc_if *pif = NULL;
	uint32_t dr_addr = 0, len = 0, ret = 0;
	struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_para = NULL;
	struct pim_register_t  reg_flag = {0};
	union pkt_control pkt_ctrl = {0};
	struct pim_iphdr inner_iphdr = {0};

	/*ifindex是与源直连，dst是rp，inner src是源，inner grp是组*/
	pif = ipmc_if_lookup(ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_DEBUG("ifindex %x, rp %x, (%x, %x) ipmc_if is NULL\n",
			ifindex, dst, inner_src, inner_dst);
		return ERRNO_FAIL;
	}
	if(!((pif->pim_nbr->cur_dr.dr_state) & PIM_DR_LOCAL))
	{
		/*与源直连的接口不是dr*/
		IPMC_LOG_DEBUG("(%x, %x) ipmc_if %x is not DR\n",
			inner_src, inner_dst, ifindex);
		return ERRNO_FAIL;
	}
	dr_addr = ipmc_if_main_addr_get(ifindex);
	if(dr_addr == 0)
	{
		IPMC_LOG_DEBUG("(%x, %x) ifindex %x dr_address is not config\n",
			inner_src, inner_dst, ifindex);
		return ERRNO_FAIL;
	}
	/* encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));
	
	pim_para = (uint8_t *)ipmc_send_buf + sizeof(struct pim_hdr);

	/*null_flag : 0x40000000  u32*/
	reg_flag.reg_flags = htonl(0x40000000);
	memcpy(pim_para, &reg_flag, sizeof(struct pim_register_t));
	pim_para += sizeof(struct pim_register_t);
	
	/*ip hdr*/
	memset(&inner_iphdr, 0, sizeof(struct pim_iphdr));
	inner_iphdr.ip_v = 4;
	inner_iphdr.ip_hl = 5;
	inner_iphdr.ip_len = htons(sizeof(struct pim_iphdr));
	inner_iphdr.ip_ttl = 255;
	inner_iphdr.ip_p = IP_P_PIM;
	inner_iphdr.ip_src = htonl(inner_src);
	inner_iphdr.ip_dst = htonl(inner_dst);
	inner_iphdr.ip_sum = htons(in_checksum((uint16_t *)(&inner_iphdr), sizeof (struct pim_iphdr)));
	
	memcpy(pim_para, &inner_iphdr, sizeof(struct pim_iphdr));
	pim_para += sizeof(struct pim_iphdr);
	
	/*pim para length*/
	len = (uint8_t *)pim_para - (uint8_t *)ipmc_send_buf;
	
	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_HELLO;
	pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
	
	/*ip hdr */
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	pkt_ctrl.ipcb.ifindex = pif->ifindex;
	pkt_ctrl.ipcb.sip = ipmc_if_main_addr_get(pif->ifindex);
	pkt_ctrl.ipcb.dip = dst;
	pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IP;
	pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
	pkt_ctrl.ipcb.tos = 0;	/*hello msg not contain ip priority*/
	pkt_ctrl.ipcb.ttl = 1;
	pkt_ctrl.ipcb.is_changed = 1;

	ret = pkt_send(PKT_TYPE_IP, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}
	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
}

sint32 pim_register_send(uint32_t rp_addr, uint8_t rs_tos,  uint32_t inner_sip, uint32_t inner_grp, 
								struct pkt_buffer *ppkt, uint8_t null_reg_flag)
{
    struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_para  = NULL, *udp_data = NULL;
	struct iphdr* in_iphdr = NULL;
	struct pim_udphdr *udph = NULL;
	union pkt_control pkt_ctrl;
	struct ip_control *ipcb = NULL;
	uint32_t len = 0;
	sint32 ret = ERRNO_FAIL;

	/*外层ip hdr信息*/
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	/*oif need route find, nexthop is rp's ipaddr,ifindex not suggest*/
	//pkt_ctrl.ipcb.ifindex = pif->ifindex;
	pkt_ctrl.ipcb.nexthop.addr.ipv4 = rp_addr;
	/*源端dr*/
	pkt_ctrl.ipcb.sip = ipmc_if_main_addr_get(ppkt->in_port);
	pkt_ctrl.ipcb.dip = rp_addr;
	pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IP;
	pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
	pkt_ctrl.ipcb.ttl = 255;
	pkt_ctrl.ipcb.is_changed = 1;
	if (!null_reg_flag)
	{
		/*tos copy from payload*/
		pkt_ctrl.ipcb.tos = rs_tos;	
	}
	else
	{
		/*NULL register message*/
		pkt_ctrl.ipcb.tos = 0;
	}
		

	/* 外层encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));

	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_REGISTER;
	pim_para = (uint8_t *)ipmc_send_buf + sizeof(struct pim_hdr);

	if (null_reg_flag)
	{
		/*encap null reg */
		ipmc_setl(&pim_para, htonl(PIM_REGISTER_NULL_REGISTER_BIT));
		len = sizeof (struct pim_hdr) + sizeof (struct pim_register_t);
		pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
		memcpy(ipmc_send_buf, pimhdr, sizeof(struct pim_hdr));
	}
	else
	{
		ipcb = (struct ip_control *)(&(ppkt->cb));
		/*encap non-null reg*/
		ipmc_setl(&pim_para, htonl(0x0));
		len = sizeof (struct pim_hdr) + sizeof (struct pim_register_t);
		/*重新封装iphdr，udphdr，udp payload*/
		in_iphdr = (struct iphdr *)pim_para;
		memset(in_iphdr, 0, sizeof(struct iphdr));
		/*20 byte*/
		in_iphdr->version = 4;
		in_iphdr->ihl = 20;
		in_iphdr->protocol = IP_P_PIM;
		in_iphdr->saddr = htonl(ipcb->sip);
		in_iphdr->daddr = htonl(ipcb->dip);
		in_iphdr->frag_off = htons(ipcb->frag_off);
		in_iphdr->tos = ipcb->tos;
		in_iphdr->ttl = ipcb->ttl - 1;
		in_iphdr->check = htons(in_checksum((uint16_t *)in_iphdr, sizeof (struct iphdr)));
		
		memcpy(pim_para, (uint8_t *)in_iphdr, sizeof(struct iphdr));
		len += sizeof(struct iphdr);
		pim_para += sizeof(struct iphdr);

		/*encap udp*/
		udph = (struct pim_udphdr *)pim_para;
		udp_data = pim_para + sizeof(struct pim_udphdr);
		memcpy(udp_data, (uint8_t *)(ppkt->data), ppkt->data_len);
		udph->sport = htons(ipcb->sport);
		udph->dport = htons(ipcb->dport);
		udph->len = htons(ppkt->data_len + sizeof(struct pim_udphdr));
		udph->checksum = htons(in_checksum((uint16_t *)udph, sizeof(struct pim_udphdr) + ppkt->data_len));
		memcpy(pim_para, (uint8_t *)udph, sizeof(struct pim_udphdr));
		pim_para += sizeof(struct pim_udphdr) + ppkt->data_len;
		len += sizeof(struct pim_udphdr) + ppkt->data_len;
 
		pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
		memcpy(ipmc_send_buf, pimhdr, sizeof(struct pim_hdr));
	}
	ret = pkt_send(PKT_TYPE_IP, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}

	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
	
	return ret;
}


/* Receiving Register Message at the RP
*   Pseudo code:
*   if ( outer.dst is not one of my addresses) {
		drop the packet;
    }	
*/
sint32 pim_register_recv(uint32_t outer_dip, uint32_t outer_sip, uint8_t outer_ip_tos,
								uint8_t *data, uint16_t data_len)
{
	struct pim_register_t *reg = NULL;
	uint32_t inner_sip, inner_grp = 0;
	uint32_t is_border = 0, is_null = 0;
	uint32_t iif_index = 0, sentRegisterStop = IPMC_FALSE;
	struct pim_iphdr *inner_iphdr = NULL;
	struct pim_mrt_sg *mrt_sg = NULL;
	
	if(!data_len)
	{
		return ERRNO_FAIL;
	}

	/*
	* lookup all interface ip address, and if (if_addr == grp)?.
	* rp is local?
	*/
	if (!(iif_index = ip4_addr_local_main(outer_dip)))
	{
		/*不是发送给当前设备的，直接丢弃*/
		return ERRNO_FAIL;
	}
	if (!pim_instance_global)
	{
		IPMC_LOG_DEBUG("pim_instance_global == NULL\n");
		return ERRNO_FAIL;
	}
	/*配置为rp的接口必须先开启pim，未开启pim肯定不是rp*/
	
	/*register flags: 4 bytes
	*  boder bit + null-register bit + reserverd 30 bits(value 0, not process)
	*/
	reg = (struct pim_register_t *)data;
	if (NULL == reg)
	{
		return ERRNO_FAIL;
	}
	
	is_border = ntohl(reg->reg_flags) & PIM_REGISTER_BORDER_BIT;
	is_null = ntohl(reg->reg_flags) & PIM_REGISTER_NULL_REGISTER_BIT;

	/*register message: payload.
	*  The payload not include eth header, only ip header include.
	*/
	
	inner_iphdr = (struct pim_iphdr *)(data + sizeof (struct pim_register_t));
	inner_sip = ntohl(inner_iphdr->ip_src);
	inner_grp = ntohl(inner_iphdr->ip_dst);
	
	if(pim_rp_check(inner_grp) == outer_dip && outer_dip == pim_instance_global->rp_addr)
	{
		/* send reg_stop msg*/
		mrt_sg = pim_mrt_sg_get(inner_sip, inner_grp, IPMC_TRUE);
		if(mrt_sg == NULL)
		{
			/*表项创建失败，丢弃报文*/
			IPMC_LOG_ERROR("pim_mrt_sg_get(%x, %x) failed\n", inner_sip, inner_grp);
			return ERRNO_FAIL;
		}
		mrt_sg->flag |= PIM_MRT_REG;
		mrt_sg->flag |= PIM_MRT_SG;
		
		//Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
		
		/*
		if ( SPTbit(S,G) OR
		  ( SwitchToSptDesired(S,G) AND
			( inherited_olist(S,G) == NULL ) ) ) {
		   send Register-Stop(S,G) to outer.src
		   sentRegisterStop = TRUE;
		 }
		 if ( SPTbit(S,G) OR SwitchToSptDesired(S,G) ) {
			  if ( sentRegisterStop == TRUE ) {
				   set KeepaliveTimer(S,G) to RP_Keepalive_Period;
			  } else {
				   set KeepaliveTimer(S,G) to Keepalive_Period;
			  }
		 }
		 if( !SPTbit(S,G) AND ! pkt.NullRegisterBit ) {
			  decapsulate and forward the inner packet to
			  inherited_olist(S,G,rpt) # Note (+)
		 }
		*/
		if(mrt_sg->flag & PIM_MRT_SPT || (SwitchToSptDesired(mrt_sg) && mrt_sg->oif.oif_num == 0))
		{
			sentRegisterStop = IPMC_TRUE;
			/*发送register-stop */
			pim_register_stop_send( outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp);
		}
		if(mrt_sg->flag & PIM_MRT_SPT || SwitchToSptDesired(mrt_sg))
		{
			if(sentRegisterStop == IPMC_TRUE)
			{
				/* RP_Keepalive_Period = 3*60 + 5 */
				if(mrt_sg->keepalive_timer)
				{
					high_pre_timer_delete(mrt_sg->keepalive_timer);
				}
				/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
												pim_mrt_sg_keepalive_timer_expire,
												(void *)mrt_sg, 
												PIM_RP_KEEPALIVE_PERIOD_DEFAULT);*/
				mrt_sg->keepalive_timer = high_pre_timer_add("SGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
								(void *)pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
								1000 * PIM_RP_KEEPALIVE_PERIOD_DEFAULT);
			}
			else
			{
				/* Keepalive_Period 210s */
				if(mrt_sg->keepalive_timer)
				{
					high_pre_timer_delete(mrt_sg->keepalive_timer);
					/*thread_cancel(mrt_sg->keepalive_timer);*/
				}
				/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
												pim_mrt_sg_keepalive_timer_expire,
												(void *)mrt_sg, 
												Keepalive_Period_default);*/
				mrt_sg->keepalive_timer = high_pre_timer_add("SGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
								(void *)pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
								1000 * Keepalive_Period_default);
			}
		}
		/*keepalive_timer改变，触发(S,G)加枝*/
		if(JoinDesiredSG(mrt_sg))
		{
			pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
		}
		/*如果存在spt标志位，则直接丢弃非空注册报文,不需要封装，可能丢包*/
		//if(!is_null && !(mrt_sg->flag & PIM_MRT_SPT))
		if(!is_null && !(mrt_sg->flag & PIM_MRT_SPT))
		{
			/*非空注册报文，将内部ip报文走rpt转发*/
			pim_mrt_sg_oif_cal(mrt_sg);
			if(mrt_sg->oif.oif_num)
			{
				data += sizeof(struct pim_register_t) + sizeof(struct iphdr);
				data_len -= (sizeof(struct pim_register_t) + sizeof(struct iphdr)) ;
				pim_data_pkt_forward(mrt_sg, data, data_len);
			}
		}
		
		#if 0
		if (is_null)
		{
			/*recv NULL-Register pdu, rpt forward */
			return pim_register_null_process( outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp, is_border, is_null);
		}
		else
		{
			return pim_register_process( outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp, is_border, is_null);
		}
		#endif
	}
	else
	{
		/*rp 错误*/
		/*发送注册停止*/
		pim_register_stop_send( outer_dip, outer_sip, outer_ip_tos, inner_sip, inner_grp);
	}
	return ERRNO_SUCCESS;
}


/*RP send pim register stop message*/
sint32 pim_register_stop_send(uint32_t outer_dip, uint32_t outer_sip, uint8_t outer_ip_tos,
									uint32_t inner_sip, uint32_t inner_grp)
{
	struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_option_grp, *pim_option_src = NULL;
	union pkt_control pkt_ctrl;
	uint32_t len = 0;
	sint32 ret = ERRNO_FAIL;
	struct encode_ipv4_group egaddr;
	struct encode_ipv4_ucast eusaddr;

	/*ip hdr */
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	/*oif need route find, nexthop is dr's ipaddr*/
	pkt_ctrl.ipcb.sip = outer_dip;
	pkt_ctrl.ipcb.nexthop.addr.ipv4 = outer_sip;
	pkt_ctrl.ipcb.tos = outer_ip_tos;	
	pkt_ctrl.ipcb.ttl = 255;
	pkt_ctrl.ipcb.is_changed = ENABLE;

	/* encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));

	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_REGISTER_STOP;
	len = sizeof (struct pim_hdr) + sizeof (struct pim_register_t) + sizeof (pim_register_stop_t);
	pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);

	/*encap pim option: include grp addr and src addr*/
	egaddr.addr_family = AFI_IP;
	egaddr.encode_type = 0;
	egaddr.reserved = 0;
	egaddr.mask_len = 32;
	egaddr.grp_addr = htonl(inner_grp);
	
	pim_option_grp = (uint8_t *)(ipmc_send_buf + sizeof(struct pim_hdr) + sizeof (struct pim_register_t));
	memcpy(pim_option_grp, &egaddr, sizeof (struct encode_ipv4_group));
	
	eusaddr.addr_family = AFI_IP;
	eusaddr.encode_type = 0;
	eusaddr.unicast_addr = htonl(inner_sip);

	pim_option_src = pim_option_grp + sizeof (struct encode_ipv4_group);
	memcpy(pim_option_src, &eusaddr, sizeof (struct encode_ipv4_source));
	
	ret = pkt_send(PKT_TYPE_IP, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}

	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
	
	return ret;
}


/*rp send ,Only DR receive pim register message*/
sint32 pim_register_stop_recv(uint32_t outer_dip, uint32_t outer_sip,
									uint8_t *data, uint16_t data_len)
{
	struct encode_ipv4_group *egaddr = NULL;
	struct encode_ipv4_ucast *eusaddr = NULL;
	uint32_t dr_ifindex = 0;
	uint32_t grp_addr = 0, src_addr = 0;
	uint8_t mcast_mask = 0;
	struct ipmc_if * pim_if = NULL;
	struct pim_mrt_sg *mrt_sg = NULL;
	
	/*
	* lookup all interface ip address, and if (if_addr == dip)?.
	*/
	if(NULL == pim_instance_global)
	{
		IPMC_LOG_ERROR("pim_instance_global == NULL\n");
		return ERRNO_FAIL;
	}
	if(!(dr_ifindex = ip4_addr_local_main(outer_dip)))
	{
		IPMC_LOG_ERROR("%x is not local address\n", outer_dip);
		return ERRNO_FAIL;
	}
	
	pim_if = ipmc_if_lookup(dr_ifindex);
	if(!pim_if)
	{
		IPMC_LOG_DEBUG("%x is not a address of ipmc_if\n", dr_ifindex);
		return ERRNO_FAIL;
	}
	if(pim_if->pim_nbr == NULL)
	{
		IPMC_LOG_DEBUG("ipmc_if %x is not a pim if\n", dr_ifindex);
		return ERRNO_FAIL;
	}

	if(!(pim_if->pim_nbr->cur_dr.dr_state & PIM_DR_LOCAL))
	{
		IPMC_LOG_DEBUG("ipmc_if %x is not DR\n", dr_ifindex);
		return ERRNO_FAIL;
	}

	pim_instance_global->pim_stat.pimstat_rs_msg_rx++;
	
	if( data == NULL || data_len < (sizeof(struct encode_ipv4_group) + sizeof(struct encode_ipv4_ucast)))
	{
		IPMC_LOG_DEBUG("rp %x, dr %x, data_len == 0 or data == NULL\n", outer_sip, outer_sip);
		pim_instance_global->pim_stat.pimstat_rs_msg_rx_err++;
		return ERRNO_FAIL;
	}
	
	/*PIM option
	*  Group address, Source address, same as multicast source.
	*/
	egaddr = (struct encode_ipv4_group *)data;
	grp_addr = ntohl(egaddr->grp_addr);
	mcast_mask = egaddr->mask_len;

	eusaddr = (struct encode_ipv4_ucast *)(data + sizeof (struct encode_ipv4_group));
	src_addr = ntohl(eusaddr->unicast_addr);
	
	/* 不处理(*,G) reg_stop, only grp_mask == 32 */
	if((!ipv4_is_multicast(grp_addr)) || (!ipv4_is_valid(src_addr)) || !src_addr || mcast_mask != 32)
	{
		IPMC_LOG_ERROR("(%x, %x) , grp_mask %u  invalid\n", src_addr, grp_addr, mcast_mask);
		return ERRNO_FAIL;
	}
	/*获取sg表项*/
	mrt_sg = pim_mrt_sg_get( src_addr, grp_addr, IPMC_FALSE);
	if(mrt_sg == NULL)
	{
		IPMC_LOG_DEBUG("sg(%x, %x) entry not exist\n", src_addr, grp_addr);
	}
	
	/*注册状态机*/
	pim_reg_state_machine(mrt_sg, PIM_REG_REG_STOP_RCV);
	return ERRNO_SUCCESS;
}

