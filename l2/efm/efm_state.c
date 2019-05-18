#include <string.h>
#include <stdlib.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/oam_common.h>
#include <lib/types.h>
#include <lib/log.h>
#include <ftm/pkt_eth.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/alarm.h>
#include "efm_def.h"
#include "efm_pkt.h"
#include "efm_state.h"
#if 0
/*efm send PDU timeout*/
	int
efm_pdu_timer_expiry (struct thread *thread)
{
	struct efm_if *pefm = NULL;

    if(NULL == thread || NULL == thread->arg)
    {
		return 0;
    }

	pefm = (struct efm_if *)THREAD_ARG (thread);

    if(NULL == pefm)
    {
		return 0;
    }

	//pefm->efm_tx_timer = NULL;
	EFM_TIMER_OFF(pefm->efm_tx_timer);

	EFM_LOG_DBG("EFM[EVENT]: EFM PDU timer expired\n");

	efm_transmit_state_machine (pefm, EFM_INFORMATION_PDU);

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_SYM_PERIOD_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_SYMBOL_PERIOD_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_SYM_PERIOD_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_PERIOD_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_PERIOD_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_PERIOD_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_SEC_SUM_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_SEC_SUM_EVENT_PENDING);
	}

	return 0;
}
#endif
/*efm discovert timeout*/
	int
efm_pdu_timer_expiry (void * arg)
{
	struct efm_if *pefm = NULL;

	if(NULL == arg)
	{
		return 0;
	}

	pefm = (struct efm_if *)arg;

	if(NULL == pefm)
	{
		return 0;
	}


	EFM_LOG_DBG("EFM[EVENT]: EFM PDU timer expired\n");
	pefm->efm_tx_timer = NULL;

	efm_transmit_state_machine (pefm, EFM_INFORMATION_PDU);

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_SYM_PERIOD_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_SYMBOL_PERIOD_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_SYM_PERIOD_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_PERIOD_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_PERIOD_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_PERIOD_EVENT_PENDING);
	}

	if (CHECK_FLAG (pefm->efm_pending_link_events, EFM_FRAME_SEC_SUM_EVENT_PENDING)
			&& pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_transmit_state_machine (pefm, EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU);
		UNSET_FLAG (pefm->efm_pending_link_events, EFM_FRAME_SEC_SUM_EVENT_PENDING);
	}

	return 0;
}

/*modify by youcheng 2018/8/26*/
/*efm discovert timeout*/
#if 0 
	int
efm_link_timer_expiry (struct thread *thread)
{
	struct efm_if *pefm = NULL;
	high_pre_timer

    if(NULL == thread || NULL == thread->arg)
    {
        return 0;
    }

	pefm = (struct efm_if *)THREAD_ARG (thread);

    if(NULL == pefm)
    {
        return 0;
    }

	EFM_LOG_DBG("EFM[EVENT]: EFM Link timer expired\n");

	//pefm->efm_link_timer = NULL;
	EFM_TIMER_OFF(pefm->efm_link_timer);

	efm_discovery_fault (pefm);
	pefm->efm_remote_state_valid = EFM_FALSE;
	efm_discovery_state_machine (pefm);

	return EFM_SUCCESS;
}
#endif
/*efm discovert timeout*/
	int
efm_link_timer_expiry (void * arg)
{
	struct efm_if *pefm = NULL;

	if(NULL == arg)
	{
		return 0;
	}

	pefm = (struct efm_if *)arg;

    if(NULL == pefm)
    {
        return 0;
    }

	EFM_LOG_DBG("EFM[EVENT]: EFM Link timer expired\n");

	pefm->efm_link_timer = NULL;

	efm_discovery_fault (pefm);
	pefm->efm_remote_state_valid = EFM_FALSE;
	efm_discovery_state_machine (pefm);

	return EFM_SUCCESS;
}


/*802.3ah 57.3.2.1 OAM Discovery*/
/*57.3.2.1.1 FAULT state*/
	int
efm_discovery_fault (struct efm_if *pefm)
{
	uint8_t loopback_act = 0;
	EFM_LOG_DBG("EFM[EVENT]: Executing FAULT state of Discovery state machine\n");

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	if (NULL == pefm)
    {
        return EFM_FAILURE;
    }

    if(EFM_OAM_LINK_FAULT == pefm->efm_local_link_status)
    {
        pefm->local_pdu = EFM_LF_INFO;
    }
	else
    {
        pefm->local_pdu = EFM_RX_INFO;
    }

	pefm->efm_local_stable = EFM_FALSE;

    if(EFM_DISCOVERY_SEND_ANY == pefm->efm_discovery_state)
	{

		//send alarm:EFM_FAIL
		
		//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_RISE);

		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = pefm->if_index;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_RISE);
	}

    pefm->efm_discovery_state = EFM_DISCOVERY_FAULT_STATE;

	UNSET_FLAG (pefm->efm_flags, EFM_LOCAL_STABLE);
	UNSET_FLAG (pefm->efm_flags, EFM_LOCAL_EVALUATING);
	UNSET_FLAG (pefm->efm_flags, EFM_REMOTE_STABLE);
	UNSET_FLAG (pefm->efm_flags, EFM_REMOTE_EVALUATING);

	pefm->efm_remote_state_valid = EFM_FALSE;
	pefm->efm_local_lost_link_timer_done = EFM_FALSE;
	EFM_TIMER_OFF (pefm->efm_link_timer);


	/* If the local DTE or the remote DTE is in loopback mode
	 * then out of loopback mode
	 */
    if((EFM_REM_LB == pefm->local_info.loopback) ||
            (EFM_REM_LB == pefm->rem_info.loopback))
	{
        if(EFM_REM_LB == pefm->local_info.loopback)
		{


			//if local mac-swap so cancel mac-swap    else should cancel local loopback
			if(pefm->efm_mac_swap_flag)
			{				
				loopback_act = EFM_NO_MAC_SWAP;
#if 0
				if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
				{
					zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
					return EFM_FALSE;

				}
#endif
				if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
				{
				       zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
		                         return EFM_FALSE;

				}
				pefm->efm_mac_swap_flag = EFM_FALSE;
			}
			else 
			{
				loopback_act = EFM_DISABLE_LB;
				#if 0
				if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM,   REMOTE_LOOPBACK , IPC_OPCODE_CHANGE, pefm->if_index ))
				{


					zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
					return EFM_FALSE;
				}
				#endif
				if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM,   REMOTE_LOOPBACK , IPC_OPCODE_CHANGE, pefm->if_index ))
				{


					zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel loopback failure!\n", __FILE__, __LINE__, __func__ );
					return EFM_FALSE;
				}
				pefm->efm_loopback_flag = EFM_FALSE;
			}
			//pefm->efm_remoteside_make_localside_loopback_start = EFM_FALSE;

		}

		pefm->efm_rem_loopback = EFM_FALSE;
		pefm->local_info.loopback = EFM_NONE_LB;
		pefm->rem_info.loopback = EFM_NONE_LB;

		pefm->rem_info.state &= 0x00;

		//should set fwd state to hsl, cancel loopback state
		//

		pefm->efm_local_par_action_t = EFM_PAR_FWD;
		pefm->efm_local_mux_action_t = EFM_MUX_FWD;
		pefm->rem_info.loopback = EFM_NONE_LB;
	}

	return EFM_SUCCESS;
}

/*57.3.2.1.2 ACTIV_SEND_LOCAL state*/
	int
efm_discovery_active_send_local (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing ACTIVE_SEND_LOCAL state of Discovery state machine\n");
	if (NULL == pefm)
		return EFM_FAILURE;
         /*0000 1000 local evaluating = 1 local stable = 0 Discovery prodcess has not completed*/
	SET_FLAG (pefm->efm_flags, EFM_LOCAL_EVALUATING);
	UNSET_FLAG (pefm->efm_flags, EFM_LOCAL_STABLE);
	pefm->efm_discovery_state = EFM_DISCOVERY_ACTIVE_SEND_LOCAL;
	pefm->local_pdu = EFM_INFO;

	return EFM_SUCCESS;
}
/*57.3.2..1.3 PASSIVE WAIT state*/
	int
efm_discovery_passive_wait (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing PASSIVE_WAIT state of Discovery "
			"state machine\n");
	if (NULL == pefm)
		return EFM_FAILURE;

	pefm->local_pdu = EFM_RX_INFO;
	pefm->efm_discovery_state = EFM_DISCOVERY_PASSIVE_WAIT;

	return EFM_SUCCESS;
}
/*57.3.2.1.4 SEND LOCAL REMOTE state*/
	int
efm_discovery_send_local_remote (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing SEND_LOCAL_REMOTE state of "
			"Discovery state machine\n");
	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	pefm->local_pdu =  EFM_INFO;
	pefm->efm_discovery_state = EFM_DISCOVERY_SEND_LOCAL_REMOTE;
	pefm->efm_local_stable = EFM_FALSE;

	return EFM_SUCCESS;
}
/*57.3.2.1.5 SEND LOCAL REMOTE OK state*/
	int
efm_discovery_send_local_remote_ok (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing SEND_LOCAL_REMOTE_OK state "
			"of Discovery state machine\n");

	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	pefm->local_pdu =  EFM_INFO;
	pefm->efm_discovery_state = EFM_DISCOVERY_SEND_LOCAL_REMOTE_OK;
	pefm->efm_local_stable = EFM_TRUE;
	/*has recive remote PDU 0001 0000*/
	SET_FLAG (pefm->efm_flags, EFM_LOCAL_STABLE);
	UNSET_FLAG (pefm->efm_flags, EFM_LOCAL_EVALUATING);

	return EFM_SUCCESS;
}
/*57.3.2.1.6 SEND LOCAL ANY state*/
	int
efm_discovery_send_any (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing SEND_ANY state of Discovery "
			"state machine\n");

	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	pefm->local_pdu = EFM_ANY;
	pefm->efm_discovery_state = EFM_DISCOVERY_SEND_ANY;

	return EFM_SUCCESS;
}
/*57.3.2.2 Transmit*/
/*57.3.2.2.1 RESET state*/
	int
efm_transmit_reset (struct efm_if *pefm, enum efm_pdu pdu)
{
	EFM_LOG_DBG("EFM[EVENT/TX]: Executing RESET state of Transmit state machine\n");

	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	/*Start timer*/

	pefm->efm_transmit_state = EFM_TRANSMIT_RESET;

	EFM_TIMER_ON_UNLOOP (pefm->efm_tx_timer, efm_pdu_timer_expiry,
			pefm, pefm->efm_pdu_time);

	pefm->efm_pdu_cnt = pefm->efm_pdu_max;

	efm_transmit_wait_for_tx (pefm);

	return EFM_SUCCESS;
}
/*57.3.2.2.2 WAIT FOR TX state*/
	int
efm_transmit_wait_for_tx (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT/TX]: Executing WAIT_FOR_TX state of Transmit "
			"state machine\n");

	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	pefm->efm_transmit_state = EFM_TRANSMIT_WAIT_FOR_TX;

	return EFM_SUCCESS;
}

	int
efm_transmit_dec_pdu_cnt (struct efm_if *pefm, enum efm_pdu pdu)
{
	EFM_LOG_DBG("EFM[EVENT/TX]: Executing DEC_PDU_CNT state of Transmit "
			"state machine\n");

	if (NULL == pefm)
    {
		return EFM_FAILURE;
    }

	if (pefm->pdu_req == EFM_NORMAL)
	{
		pefm->efm_pdu_cnt = ((pefm->efm_pdu_cnt) - 1);
	}

	pefm->efm_transmit_state = EFM_TRANSMIT_DEC_PDU_CNT;

	efm_transmit_tx_oampdu (pefm, pdu);

	return EFM_SUCCESS;
}

	int
efm_transmit_tx_oampdu (struct efm_if *pefm, enum efm_pdu pdu)
{
	EFM_LOG_DBG("EFM[EVENT/TX]: Executing TX_OAMPDU state of Transmit "
			"state machine\n");

	pefm->efm_transmit_state = EFM_TRANSMIT_TX_OAMPDU;

	efm_tx (pefm, pdu);

	efm_transmit_state_machine (pefm, pdu);

	return EFM_SUCCESS;
}

/*802.3 ah 57.3.2.1 OAM Discovery*/
	void
efm_discovery_state_machine (struct efm_if *pefm )
{
	EFM_LOG_DBG("EFM[EVENT]: Executing EFM OAM Discovery state machine");

	if (NULL == pefm)
    {
		return;
    }

    if((EFM_TRUE == pefm->efm_local_lost_link_timer_done) ||
            (EFM_OAM_LINK_FAULT == pefm->efm_local_link_status))
	{
		efm_discovery_fault (pefm);
		return;
	}

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pefm->if_index;
	switch (pefm->efm_discovery_state)
	{
		case EFM_DISCOVERY_FAULT_STATE:
            if(EFM_MODE_ACTIVE == pefm->efm_local_oam_mode)
			{
				efm_discovery_active_send_local (pefm);
			}
			else
			{

				efm_discovery_passive_wait (pefm);
			}
			//should send efm-fail alarm rise
			
			//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_RISE);
			
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_RISE);
			break;
		case EFM_DISCOVERY_ACTIVE_SEND_LOCAL:
		case EFM_DISCOVERY_PASSIVE_WAIT:
            if(EFM_TRUE ==  pefm->efm_remote_state_valid)
			{      
				efm_discovery_send_local_remote (pefm);
			}
			break;
		case EFM_DISCOVERY_SEND_LOCAL_REMOTE:
            if(EFM_TRUE == pefm->efm_local_satisfied)
			{
				efm_discovery_send_local_remote_ok (pefm);
			}
			break;
		case EFM_DISCOVERY_SEND_LOCAL_REMOTE_OK:
            if(EFM_TRUE == pefm->efm_local_satisfied
                    &&  EFM_TRUE == pefm->efm_remote_stable)
			{
				efm_discovery_send_any (pefm);
				//should send efm-fail alarm clean
				
				//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_CLEAN);
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
			}
            else if(EFM_FALSE == pefm->efm_local_satisfied)
			{
				efm_discovery_send_local_remote (pefm);
			}
			break;
		case EFM_DISCOVERY_SEND_ANY:
            if(EFM_FALSE == pefm->efm_local_satisfied)
			{
				efm_discovery_send_local_remote (pefm);
				//should send efm-fail alarm rise
				
				//ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_RISE);
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
            else if(EFM_FALSE == pefm->efm_remote_stable)
			{
				efm_discovery_send_local_remote_ok (pefm);
				//send efm-fail alarm rise
			    //ipran_alarm_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_ALM_TYPE_FUN_EFM_FAIL,GPN_SOCK_MSG_OPT_RISE);
                    
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_EFM_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			break;
		default:
			break;
	}
}

/*802.3 ah 57.3.2.2 Transmit*/
	void
efm_transmit_state_machine (struct efm_if *pefm, enum efm_pdu pdu)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing EFM OAM Transmit state machine");

	if (NULL == pefm)
    {
		return;
    }

	switch (pefm->efm_transmit_state)
	{
		case EFM_TRANSMIT_RESET:
			efm_transmit_reset (pefm, pdu);
			break;
		case EFM_TRANSMIT_WAIT_FOR_TX:
            if((!pefm->efm_tx_timer)
                    && (EFM_RX_INFO == pefm->local_pdu
						|| pefm->efm_pdu_cnt != pefm->efm_pdu_max))
			{
				efm_transmit_reset (pefm, pdu);
			}
            else if(!pefm->efm_tx_timer
					&& pefm->local_pdu != EFM_RX_INFO
					&& pefm->efm_pdu_cnt == pefm->efm_pdu_max)
			{           
				efm_transmit_tx_oampdu (pefm, pdu);
			}
			else if (pefm->efm_tx_timer
                    && EFM_TRUE == pefm->valid_pdu_req)
			{      
				efm_transmit_dec_pdu_cnt (pefm, pdu);
			}
			break;
		case EFM_TRANSMIT_TX_OAMPDU:
            if(!pefm->efm_tx_timer)
			{
				efm_transmit_reset (pefm, pdu);
			}
			else
			{
				efm_transmit_wait_for_tx (pefm);
			}
			break;
		case EFM_TRANSMIT_DEC_PDU_CNT:
			efm_transmit_tx_oampdu (pefm, pdu);
			break;

		default:
			break;
	}

	return;
}


	void
efm_run_state_machine (struct efm_if *pefm)
{
	EFM_LOG_DBG("EFM[EVENT]: Executing EFM OAM state machine");

	efm_discovery_state_machine(pefm);
	efm_transmit_state_machine (pefm, EFM_INFORMATION_PDU);
}
	
/*efm remote loopback state change*/
	void
efm_process_rem_loopback (struct efm_if *pefm)
{
	
	uint8_t  loopback_act = 0;
    if(NULL == pefm )
    {
		return;
    }

	/* rem_info.loopback set when receive LB */
	if (pefm->rem_info.loopback == EFM_LOCAL_LB)
	{
		/* Section 57.2.11.1 Remote DTE has initiated a remote loopback.
		 * Put the local DTE in looback mode under following condition
		 * 1. The Peer is not passive (Table 57-1).
		 * 2. The Local DTE has not initiated a remote loopback.
		 * 3. The Local DTE has initiated a remote loopback but
		 *    the remove DTE has a lower source MAC address.
		 */
		if (pefm->efm_local_oam_mode == EFM_MODE_ACTIVE
				&& ! CHECK_FLAG (pefm->rem_info.oam_config, EFM_CFG_OAM_MODE))
        {
			return;
        }

		if ((memcmp (pefm->local_info.mac_addr, pefm->rem_info.mac_addr, 6) > 0)
				|| (pefm->local_info.loopback == EFM_NONE_LB))
		{
			if (!CHECK_FLAG(pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT))
			{
				pefm->efm_local_par_action_t = EFM_PAR_FWD;
				pefm->efm_local_mux_action_t = EFM_MUX_FWD;
				pefm->rem_info.loopback = EFM_NONE_LB;
				return;
			}	

			//set lb to hsl, if set fail, return;

			pefm->efm_local_par_action_t = EFM_PAR_LB;
			pefm->efm_local_mux_action_t = EFM_MUX_DISCARD;
			pefm->local_info.loopback = EFM_REM_LB;

			efm_tx (pefm, EFM_INFORMATION_PDU);
		}//local has enable loopback
		else  if ((memcmp (pefm->local_info.mac_addr, pefm->rem_info.mac_addr, 6) < 0)
				&& (pefm->local_info.loopback == EFM_LOCAL_LB))
		{
			pefm->efm_local_par_action_t = EFM_PAR_DISCARD;
			pefm->efm_local_mux_action_t = EFM_MUX_FWD;   
			efm_tx (pefm, EFM_INFORMATION_PDU);
		}
		EFM_LOG_DBG("EFM[EVENT]: Executing EFM OAM Remote Loopback Receiver "
				"state machine");
		if(pefm->local_info.loopback == EFM_REM_LB)
		   {
		          /*first  enable mac swap*/
		         if(pefm->efm_mac_swap&&!pefm->efm_mac_swap_flag)
			{    
				EFM_LOG_DBG("%s[%d]:enable mac swap,so mac swap first",__FUNCTION__,__LINE__);
				/*if loopback has exist and cancel*/

				if(pefm->efm_loopback_flag)
				{
					EFM_LOG_DBG("%s[%d]:efm becaue enable mac swap and loopback has exist,so first cancel loopback",__FUNCTION__,__LINE__);
					loopback_act = EFM_DISABLE_LB;
					#if 0
					if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

						IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
				                 {
                                              
						 zlog_err ( "%s[%d]:leave %s:error:efm send info to loopback failure!\n", __FILE__, __LINE__, __func__ );
						return ;
					}
					#endif
					if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

								IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
					{

						zlog_err ( "%s[%d]:leave %s:error:efm send info to loopback failure!\n", __FILE__, __LINE__, __func__ );
						return ;
					}
					pefm->efm_loopback_flag = EFM_FALSE;
				}
				
				/*set mac swap*/
				loopback_act = EFM_MAC_SWAP;
				#if 0
				if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
				{
                                        
				    zlog_err ( "%s[%d]:leave %s:error:efm send info to hal  mac swap failure!\n", __FILE__, __LINE__, __func__ );
						return ;
				}
				#endif
				if(ipc_send_msg_n2( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
				{

					zlog_err ( "%s[%d]:leave %s:error:efm send info to hal  mac swap failure!\n", __FILE__, __LINE__, __func__ );
					return ;
				}
				pefm->efm_mac_swap_flag = EFM_TRUE;
				
			}
				 /*cancel mac_swap */
			 if(!pefm->efm_mac_swap&&pefm->efm_mac_swap_flag)
			{     
				EFM_LOG_DBG("%s[%d]:efm disenable mac swap,so cancel mac swap",__FUNCTION__,__LINE__);

				loopback_act = EFM_NO_MAC_SWAP;
				#if 0
				if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))

				{
					zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
					  return ;

				}
				#endif
				if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
							IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))

				{
					zlog_err ( "%s[%d]:leave %s:error:efm send info to cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
					return ;

				}
				pefm->efm_mac_swap_flag = EFM_FALSE;	
			}
			 /*if no set mac_swap and no set loopback*/
			if(!pefm->efm_mac_swap_flag&&!pefm->efm_loopback_flag)
			{    
				EFM_LOG_DBG("%s[%d:efm disable mac swap,so only set loopback",__FUNCTION__,__LINE__);
				loopback_act = EFM_LB;
				#if 0
				if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

						IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
				{

					zlog_err ( "%s[%d]:leave %s:error:efm send info to loopback failure!\n", __FILE__, __LINE__, __func__ );
					return ;
				}
				#endif
				if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

							IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
				{

					zlog_err ( "%s[%d]:leave %s:error:efm send info to loopback failure!\n", __FILE__, __LINE__, __func__ );
					return ;
				}
				pefm->efm_loopback_flag = EFM_TRUE;
			  }
		}
	}
	//A enable remote_loopback,A receive B synce infor
	else if (pefm->local_info.loopback == EFM_LOCAL_LB
			&& pefm->rem_info.loopback != EFM_REM_LB)
	{
		/* Section 57.2.11.1 If the local DTE initiated the Remote
		 * loopback and the remote DTE is in Loopback mode updated
		 * Multiplexer state to forward.
		 */
		/*discover B has response loopback A par = dis mux = fwd*/
		if ( ((pefm->rem_info.state & EFM_PAR_MASK) == EFM_PAR_LB)
				&& (CHECK_FLAG (pefm->rem_info.state, EFM_MUX_BIT)))
		{
			//set fwd to hsl, if set fail, return;
			pefm->efm_local_mux_action_t = EFM_MUX_FWD;
			pefm->rem_info.loopback = EFM_REM_LB;
			EFM_TIMER_OFF (pefm->efm_rem_lb_timer);
		}
		/*
		   else if ((pefm->rem_info.state & EFM_PAR_MASK) == EFM_PAR_DISCARD)
		   {
		   pefm->efm_local_par_action_t = EFM_PAR_DISCARD;
		   pefm->efm_local_mux_action_t = EFM_MUX_FWD;
		   pefm->local_info.loopback = EFM_LOCAL_LB;
		   pefm->rem_info.loopback = EFM_REM_LB;
		   pefm->efm_rem_loopback = EFM_TRUE;
		   }
		 
		else 
		{       
			//If remote DTE not enter Loopback mode 
			pefm->efm_local_par_action_t = EFM_PAR_FWD;
			pefm->efm_local_mux_action_t = EFM_MUX_FWD;
			pefm->local_info.loopback = EFM_NONE_LB;
			pefm->rem_info.loopback = EFM_NONE_LB;
			pefm->efm_rem_loopback = EFM_FALSE;
		}
	*/
	}
	/*cancel loopback*/
	else if (pefm->local_info.loopback == EFM_REM_LB
			&& pefm->rem_info.loopback == EFM_NONE_LB)
	{
		/* Section 57.2.11.3 If the local DTE is in Remote loopback
		 * the remote DTE disables remote loopback put both
		 * multiplexer and parser to forward state.
		 */

		//set fwd to hsl, if set fail, return;

		pefm->efm_local_par_action_t = EFM_PAR_FWD;
		pefm->efm_local_mux_action_t = EFM_MUX_FWD;
		pefm->local_info.loopback = EFM_NONE_LB;
		/*cancel    loopback or mac_swap*/
		if(pefm->efm_mac_swap_flag)
		{
			loopback_act = EFM_NO_MAC_SWAP;
			#if 0
			if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
			{
				zlog_err ( "%s[%d]:leave %s:error:efm send info to hal cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
				return ;

			}
			#endif
			if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,
						IPC_TYPE_EFM, MAC_SWAP, IPC_OPCODE_CHANGE, pefm->if_index ))
			{
				zlog_err ( "%s[%d]:leave %s:error:efm send info to hal cancel mac swap failure!\n", __FILE__, __LINE__, __func__ );
						return ;

			}
			pefm->efm_mac_swap_flag = EFM_FALSE;
		}
		else if(pefm->efm_loopback_flag)
		{
			loopback_act = EFM_DISABLE_LB;
			#if 0
			if(ipc_send_hal ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

						IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
			{
				zlog_err ( "%s[%d]:leave %s:error:efm send info to hal cancel loopback  failure!\n", __FILE__, __LINE__, __func__ );
				return ;

			}
			#endif
			if(ipc_send_msg_n2 ( &loopback_act, 1, 1 , MODULE_ID_HAL, MODULE_ID_L2,

					IPC_TYPE_EFM, REMOTE_LOOPBACK, IPC_OPCODE_CHANGE, pefm->if_index ))
			{
                                      zlog_err ( "%s[%d]:leave %s:error:efm send info to hal cancel loopback  failure!\n", __FILE__, __LINE__, __func__ );
		                    return ;

			}
			pefm->efm_loopback_flag = EFM_FALSE;
		}		  


	}
	else if (pefm->rem_info.loopback == EFM_REM_LB)
	{
		/* Section 57.2.11.3 If the local DTE sent a disable
		 * remote loopback and remote DTE has successfully disabled
		 * loopback update multiplexer and parser to forward state.
		 */

		if (((pefm->rem_info.state & EFM_PAR_MASK) == EFM_PAR_FWD)
				&& ((pefm->rem_info.state << EFM_MUX_BIT_SHIFT) == EFM_MUX_FWD))
		{
			//set fwd to hsl, if set fail, return;

			pefm->efm_local_par_action_t = EFM_PAR_FWD;
			pefm->efm_local_mux_action_t = EFM_MUX_FWD;
			pefm->rem_info.loopback = EFM_NONE_LB;
		}
	}
}





