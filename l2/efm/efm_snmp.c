
#include <string.h>
#include <stdlib.h>
#include "thread.h"

#include <lib/hash1.h>
#include <lib/command.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/vty.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/log.h>
#include <lib/linklist.h>
#include <lib/alarm.h>
#include <lib/devm_com.h>
#include <lib/msg_ipc_n.h>
#include <lib/snmp_common.h>

#include <netinet/in.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#include "l2/l2_if.h"
#include "efm/efm.h"
#include "efm/efm_link_monitor.h"
#include "efm/efm_cmd.h"
#include "efm_state.h"
#include "efm/efm_pkt.h"
#include "efm/efm_snmp.h"
/*snmp get efm local information*/
int efm_get_local_info(struct efm_local_info_data * local_info,struct efm_if * pefm)
{
	if(NULL == pefm)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}

	local_info->efm_local_enable = pefm->efm_local_oam_enable;
	
	if(pefm->efm_discovery_state == EFM_DISCOVERY_PASSIVE_WAIT)
	{
		local_info->efm_local_discovery_status = pefm->efm_discovery_state+1;
	}
	else if(pefm->efm_discovery_state == EFM_DISCOVERY_ACTIVE_SEND_LOCAL)
	{
		local_info->efm_local_discovery_status = EFM_DISCOVERY_ACTIVE_SEND_LOCAL+3;
	}
	else if(pefm->efm_discovery_state == EFM_DISCOVERY_SEND_LOCAL_REMOTE ||\
		pefm->efm_discovery_state == EFM_DISCOVERY_SEND_LOCAL_REMOTE_OK||\
		pefm->efm_discovery_state == EFM_DISCOVERY_FAULT_STATE)
	{
		local_info->efm_local_discovery_status = pefm->efm_discovery_state +2;
	}
	else 
	{
		local_info->efm_local_discovery_status =  pefm->efm_discovery_state + 4;
	}
	local_info->efm_local_mode = pefm->efm_local_oam_mode+1;
	local_info->efm_local_pdu_size = pefm->local_info.oam_pdu_config; 	  
	local_info->efm_local_config_revision = pefm->local_info.revision;
	local_info->efm_local_supports = pefm->local_info.oam_config >> 1;
	
	return EFM_SUCCESS;
}
/*snmp get efm peer information*/
int efm_get_peer_info(struct efm_peer_info_data * peer_info,struct efm_if *pefm)
{
	int i = 0;
	if(pefm == NULL)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}

	 if(EFM_DISCOVERY_SEND_ANY != pefm->efm_discovery_state)
	 {
		 zlog_err("%s[%d]:leave %s no get remote info\n",__FILE__,__LINE__,__func__);
				 return EFM_FAILURE;

	 }
		memcpy(peer_info->efm_peer_mac,pefm->rem_info.mac_addr,6);

		memcpy(peer_info->efm_peer_oui,pefm->rem_info.oui,EFM_OUI_LENGTH);

                for(i=0;i<EFM_VSI_LENGTH;i++)
              {   
		peer_info->efm_peer_vendor_info += ((uint32_t)(pefm->rem_info.vend_spec_info[i]))<<(24-8*i);

              }
	 if (pefm->efm_remote_state_valid == EFM_FALSE)
	{
		peer_info->efm_peer_mode = EFM_SNMP_UNKNOWN;
	}
	else
	{
		peer_info->efm_peer_mode = CHECK_FLAG (pefm->rem_info.oam_config, EFM_CFG_OAM_MODE)?
		EFM_SNMP_ACTIVE: EFM_SNMP_PASSIVE;
	}

		peer_info->efm_peer_pdu_size = pefm->rem_info.oam_pdu_config;

		peer_info->efm_peer_config_revision = pefm->rem_info.revision;

		peer_info->efm_peer_supports = (uint8_t)pefm->rem_info.oam_config>>1;
	

		return EFM_SUCCESS;
}   
/*snmp get efm remote loopback information*/
int efm_get_loopback_info(struct efm_loopback_info_data  * loopback_info,struct efm_if *pefm)
{
    if(NULL == pefm)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}	
	if(((pefm->local_info.loopback == EFM_LOCAL_LB) && (pefm->rem_info.loopback == EFM_NONE_LB))||\
			((pefm->rem_info.loopback == EFM_LOCAL_LB)&&(pefm->local_info.loopback == EFM_NONE_LB)))
    {
		loopback_info->efm_loopback_state = SNMP_INITATING_LOOPBACK;
    }

	if((pefm->local_info.loopback == EFM_NONE_LB )&& (pefm->rem_info.loopback == EFM_NONE_LB))
    {
		loopback_info->efm_loopback_state = SNMP_NOLOOPBACK;
    }

	if((pefm->local_info.loopback == EFM_LOCAL_LB) &&( pefm->rem_info.loopback == EFM_REM_LB))
    {
		loopback_info->efm_loopback_state =  SNMP_REMOTE_LOOPBACK;
    }

	if ((pefm->local_info.loopback == EFM_REM_LB)&&(pefm->rem_info.loopback == EFM_LOCAL_LB))
    {
		loopback_info->efm_loopback_state =  SNMP_LOCAL_LOOPBACK;
    }
	//A enable remote_loopback to B  (1) A cancel remte_loopback   (2) B cancel loopback 
	if(((pefm->local_info.loopback == EFM_REM_LB) && (pefm->rem_info.loopback == EFM_NONE_LB))||\
			((pefm->rem_info.loopback == EFM_REM_LB)&&((pefm->rem_info.state & EFM_PAR_MASK) == EFM_PAR_FWD)
			 && ((pefm->rem_info.state << EFM_MUX_BIT_SHIFT) == EFM_MUX_FWD)))
    {
		loopback_info->efm_loopback_state = SNMP_TERMINATING_LOOPBACK;
    }

	if(pefm->efm_local_link_status == EFM_OAM_LINK_FAULT&&pefm->efm_discovery_state == EFM_DISCOVERY_SEND_ANY)
    {
		loopback_info->efm_loopback_state = SNMP_KNOWM;
    }

	
		loopback_info->efm_loopback_ignore_rx = CHECK_FLAG (pefm->local_info.oam_config, EFM_CFG_REM_LB_SUPPORT)?\
							PROCESS_REMOTE_LOOPBACK : IGNORE_REMOTE_LOOPBACK;
	return EFM_SUCCESS;
}
/*snmp get efm send and recv PDU number statistic*/
int efm_get_stats_info(struct efm_stats_info_data * stats_info,struct efm_if *pefm)
{
	if(NULL == pefm)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}	
	stats_info->efm_InformationTx= pefm->tx_count [EFM_INFORMATION_PDU];
	stats_info->efm_InformationRx= pefm->rx_count [EFM_INFORMATION_PDU];
	stats_info->efm_UniqueEventNotificationTx = pefm->tx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] +
		pefm->tx_count [EFM_ERR_FRAME_EVENT_PDU] +
		pefm->tx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] +
		pefm->tx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU];
	stats_info->efm_UniqueEventNotificationRx= pefm->rx_count [EFM_ERR_SYMBOL_PERIOD_EVENT_PDU] +
		pefm->rx_count [EFM_ERR_FRAME_EVENT_PDU] +
		pefm->rx_count [EFM_ERR_FRAME_PERIOD_EVENT_PDU] +
		pefm->rx_count [EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU];
	stats_info->efm_DuplicateEventNotificationTx = 0;
	stats_info->efm_DuplicateEventNotificationRx = 0;
	stats_info->efm_LoopbackControlTx = pefm->tx_count[EFM_LOOPBACK_PDU];
	stats_info->efm_LoopbackControlRx = pefm->rx_count[EFM_LOOPBACK_PDU];
	stats_info->efm_VariableRequestTx =0;		  
	stats_info->efm_VariableRequestRx =0;		   
	stats_info->efm_VariableResponseTx =0;			
	stats_info->efm_VariableResponseRx=0;		   
	stats_info->efm_OrgSpecificTx=0;			  
	stats_info->efm_OrgSpecificRx=0;			 
	stats_info->efm_UnsupportedCodesTx=pefm->tx_count [EFM_PDU_INVALID] ;		   
	stats_info->efm_UnsupportedCodesRx =pefm->rx_count [EFM_PDU_INVALID] ;			
	stats_info->efm_FramesLostDueToOam=0;	

	return EFM_SUCCESS;
}
/*snmp get efm event config information*/
int efm_get_event_config_info(struct efm_event_config_info_data * event_config_info ,struct efm_if  *pefm)
{
	if(NULL == pefm)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}	
	event_config_info->efm_ErrSymPeriodWindowHi  =  (uint32_t)(pefm->local_link_info.err_symbol_window>>32);
	event_config_info->efm_ErrSymPeriodWindowLo = (uint32_t)(pefm->local_link_info.err_symbol_window&0xffffffff);
	event_config_info->efm_ErrSymPeriodThresholdHi = (uint32_t)(pefm->local_link_info.err_symbol_threshold >>	32);
	event_config_info->efm_ErrSymPeriodThresholdLo = (uint32_t)(pefm->local_link_info.err_symbol_threshold&0xffffffff);
	event_config_info->efm_ErrSymPeriodEvNotifEnable = (uint32_t)pefm->local_link_info.err_symbol_ev_notify_enable;
	event_config_info->efm_ErrFramePeriodWindow	= pefm->local_link_info.err_frame_period_window;
	event_config_info->efm_ErrFramePeriodThreshold = pefm->local_link_info.err_frame_period_threshold;
	event_config_info->efm_ErrFramePeriodEvNotifEnable = pefm->local_link_info.err_frame_period_ev_notify_enable;
	event_config_info->efm_ErrFrameWindow = (uint32_t)pefm->local_link_info.err_frame_window;
	event_config_info->efm_ErrFrameThreshold	= pefm->local_link_info.err_frame_threshold;
	event_config_info->efm_ErrFrameEvNotifEnable = pefm->local_link_info.err_frame_ev_notify_enable;
	event_config_info->efm_ErrFrameSecsSummaryWindow = (uint32_t)pefm->local_link_info.err_frame_sec_sum_window;
	event_config_info->efm_ErrFrameSecsSummaryThreshold  = (uint32_t)pefm->local_link_info.err_frame_sec_sum_threshold;
	event_config_info->efm_ErrFrameSecsEvNotifEnable	= pefm->local_link_info.err_frame_sec_ev_notify_enable;
	event_config_info->efm_DyingGaspEnable	= pefm->local_link_info.dying_gasp_enable;
	event_config_info->efm_CriticalEventEnable	= pefm->local_link_info.critical_event_enable;


	return EFM_SUCCESS;
}
/*snmp get efm eventlog information*/
int efm_get_eventlog_info(struct efm_eventlog_info_data * eventlog_info ,struct efm_event_log_entry *eventlog)
{

	if(NULL == eventlog)
	{
		zlog_err("%s[%d]:leave %s:eventlog is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}	
	eventlog_info->efm_EventLogIndex = eventlog->event_log_Index;
	eventlog_info->efm_EventLogTimestamp = eventlog->event_log_timestamp;
	memcpy(eventlog_info->efm_EventLogOui, eventlog->event_log_oui,3);
	if(eventlog->event_log_type == EFM_ERR_FRAME_EVENT)
	{
           	eventlog_info->efm_EventLogType = EFM_ERR_FRAME_EVENT+1;
	}
	else if(eventlog->event_log_type == EFM_ERR_FRAME_PERIOD_EVENT)
	{
        eventlog_info->efm_EventLogType = EFM_ERR_FRAME_PERIOD_EVENT - 1;
	}
	else
	{
		eventlog_info->efm_EventLogType	 =eventlog->event_log_type;
	}
		eventlog_info->efm_EventLogLocation = eventlog->event_log_location;
	
		eventlog_info->efm_EventLogWindowHi =  (uint32_t)(eventlog->event_log_window>>32);
		eventlog_info->efm_EventLogWindowLo = (uint32_t)(eventlog->event_log_window&0xffffffff);
		eventlog_info->efm_EventLogThresholdHi =  (uint32_t)(eventlog->event_log_threshold>>32);
		eventlog_info->efm_EventLogThresholdLo  = (uint32_t)(eventlog->event_log_threshold&0xffffffff);
		eventlog_info->efm_EventLogValue = eventlog->event_log_value;
		eventlog_info->efm_EventLogRunningTotal = eventlog->event_log_running_total;
		eventlog_info->efm_EventLogEventTotal = eventlog->event_log_event_total;
	
	return EFM_SUCCESS;
}
/*snmp get efm base config information*/
int efm_get_base_info(struct efm_info_data * efm_info ,struct efm_if  *pefm)
{
	if(NULL == pefm)
	{
		zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
		return EFM_FAILURE;
	}	
	efm_info->efm_rate = pefm->efm_pdu_time;
	efm_info->efm_timeout = (uint32_t)pefm->efm_link_time;
         efm_info->efm_unidirectionalLinkSupport = CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_UNI_DIR_SUPPORT) ? 1: 0;
	return EFM_SUCCESS;
}
/*snmp get efm remote loopback base config information*/
int efm_get_remloopback_info(struct efm_remloopback_info_data * efm_info,struct efm_if * pefm)
{
	if(NULL == pefm)
		{
			zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
			return EFM_FAILURE;
		}	
	efm_info->efm_remloopback_action = pefm->efm_rem_loopback;
         efm_info->efm_remloopback_timeout = pefm->efm_remote_loopback_timeout;
	efm_info->efm_remloopback_support = CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_REM_LB_SUPPORT)?1:0;
	efm_info->efm_remloopback_macswap = pefm->efm_mac_swap;

	return EFM_SUCCESS;
}
/*snmp get efm event linkmonitor config informaton*/
int efm_get_event_info(struct efm_event_info_data * efm_info,struct efm_if * pefm)
{

                 if(NULL == pefm)
		{
			zlog_err("%s[%d]:leave %s:pefm is NULL\n",__FILE__,__LINE__,__func__);
			return EFM_FAILURE;
		}	
		efm_info->efm_event_linkmonitor_status = CHECK_FLAG (pefm->local_link_event_flags,
				EFM_LINK_EVENT_ON)?1:0;
		efm_info->efm_event_linkmonitor_support = CHECK_FLAG (pefm->local_info.oam_config,
				EFM_CFG_LINK_EVENT_SUPPORT)?1:0;
		efm_info->efm_eventlog_number = pefm->max_event_log_entries;

	return EFM_SUCCESS;
}

int efm_snmp_port_local_info_get(uint32_t ifindex, struct efm_local_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_local_info_snmp efm_local_snmp = {0}; 
	


	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int i = 0;
	

	msg_num = IPC_MSG_LEN/sizeof(struct efm_local_info_snmp);
	msg_num -=3;
	memset(&efm_local_snmp,0,sizeof(struct efm_local_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if (0 == ifindex)   /* ~{3u4N;qH!~} */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}
		
			efm_get_local_info(&efm_local_snmp.efm_local_info,pif->pefm);			
			efm_local_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_local_snmp, sizeof(  struct efm_local_info_snmp ));
			memset(&efm_local_snmp,0,sizeof(struct efm_local_info_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{
		
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)   
		{
			/*
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])  
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                       
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
			*/
			return data_num;
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}


				efm_get_local_info(&efm_local_snmp.efm_local_info,pif->pefm);			
				efm_local_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_local_snmp, sizeof(  struct efm_local_info_snmp ));
				memset(&efm_local_snmp,0,sizeof(struct efm_local_info_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}
int efm_snmp_port_peer_info_get(uint32_t ifindex, struct efm_peer_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_peer_info_snmp efm_peer_snmp = {0};

	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	

	msg_num  = IPC_MSG_LEN/sizeof(struct efm_peer_info_snmp);
	msg_num -=3;
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	memset(&efm_peer_snmp,0,sizeof(struct efm_peer_info_snmp ));
	if (0 == ifindex)   /* ~{3u4N;qH!~} */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}

			efm_get_peer_info(&efm_peer_snmp.efm_peer_info,pif->pefm);
			efm_peer_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_peer_snmp, sizeof(  struct efm_peer_info_snmp ));
			memset(&efm_peer_snmp,0,sizeof(struct efm_peer_info_snmp ));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)    /* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])    /* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                        /* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm)||!pif->pefm->efm_remote_state_valid)
				{
					continue;
				}


				efm_get_peer_info(&efm_peer_snmp.efm_peer_info,pif->pefm);
				efm_peer_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_peer_snmp, sizeof(  struct efm_peer_info_snmp ));
				memset(&efm_peer_snmp,0,sizeof(struct efm_peer_info_snmp ));
				
				pbucket = pnext;
			}
		}
	}

	return data_num;
}

int efm_snmp_port_loopback_info_get(uint32_t ifindex, struct efm_loopback_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_loopback_info_snmp efm_loopback_snmp = {0};

	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;
	
	msg_num =  IPC_MSG_LEN/sizeof(struct efm_loopback_info_snmp);
	msg_num -=3;
	memset(&efm_loopback_snmp,0,sizeof(struct efm_loopback_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if (0 == ifindex)   /* ~{3u4N;qH!~} */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}

			efm_get_loopback_info(&efm_loopback_snmp.efm_loopback_info,pif->pefm);			
			efm_loopback_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++], &efm_loopback_snmp, sizeof(  struct efm_loopback_info_snmp ));
			memset(&efm_loopback_snmp,0,sizeof(struct efm_loopback_info_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)    /* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])    /* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                        /* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}

				efm_get_loopback_info(&efm_loopback_snmp.efm_loopback_info,pif->pefm);
				efm_loopback_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++], &efm_loopback_snmp, sizeof(  struct efm_loopback_info_snmp ));
				memset(&efm_loopback_snmp,0,sizeof(struct efm_loopback_info_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}

int efm_snmp_port_stats_info_get(uint32_t ifindex, struct efm_stats_info_snmp  efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext   = NULL;
	struct l2if   *pif     = NULL;
	struct efm_stats_info_snmp efm_stats_snmp = {0};
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct efm_stats_info_snmp);
	msg_num -=3;
	memset(&efm_stats_snmp,0,sizeof(struct efm_stats_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if (0 == ifindex)   /* ~{3u4N;qH!~} */
	{        
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}
			efm_get_stats_info(&efm_stats_snmp.efm_stats_info,pif->pefm);
			efm_stats_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++],&efm_stats_snmp,sizeof(struct efm_stats_info_snmp));
			memset(&efm_stats_snmp,0,sizeof(struct efm_stats_info_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{      
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)    /* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])    /* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else                                        /* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}
				efm_get_stats_info(&efm_stats_snmp.efm_stats_info,pif->pefm);
				efm_stats_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_stats_snmp,sizeof(struct efm_stats_info_snmp));
				memset(&efm_stats_snmp,0,sizeof(struct efm_stats_info_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}

int efm_snmp_port_event_config_info_get(uint32_t ifindex, struct efm_event_config_info_snmp efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext	= NULL;
	struct l2if   *pif	   = NULL;
	struct efm_event_config_info_snmp efm_event_config_snmp = {0};
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct efm_event_config_info_snmp);
	msg_num -=3;
	memset(&efm_event_config_snmp,0,sizeof(struct efm_event_config_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if (0 == ifindex)	/* ~{3u4N;qH!~} */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}
			efm_get_event_config_info(&efm_event_config_snmp.efm_event_config_info,pif->pefm);
			efm_event_config_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++],&efm_event_config_snmp,sizeof(struct efm_event_config_info_snmp));
			memset(&efm_event_config_snmp,0,sizeof(struct efm_event_config_info_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}
				efm_get_event_config_info(&efm_event_config_snmp.efm_event_config_info,pif->pefm);
				efm_event_config_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_event_config_snmp,sizeof(struct efm_event_config_info_snmp));
				memset(&efm_event_config_snmp,0,sizeof(struct efm_event_config_info_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}

int efm_snmp_port_eventlog_info_get(uint32_t ifindex, uint32_t log_index,struct efm_eventlog_info_snmp efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext = NULL;
	struct l2if   *pif	   = NULL;
	struct efm_eventlog_info_snmp efm_eventlog_snmp = {0};
	struct listnode *node = NULL;
	struct efm_event_log_entry *eventlog = NULL;
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int ifindex_flag = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct efm_event_config_info_snmp);
	msg_num -=3;
	memset(&efm_eventlog_snmp,0,sizeof(struct efm_eventlog_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if ((0 == ifindex)&& (0 == log_index))                   	/* first get data from efm */
	{
	      
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}
			EFM_LIST_LOOP (pif->pefm->efm_if_eventlog_list, eventlog, node)
			{            
				if (!eventlog)
                {
                    continue;
                }
				efm_get_eventlog_info(&efm_eventlog_snmp.efm_eventlog_info,eventlog);
				efm_eventlog_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_eventlog_snmp,sizeof(struct efm_eventlog_info_snmp));
				memset(&efm_eventlog_snmp,0,sizeof(struct efm_eventlog_info_snmp));
				
				if (data_num == msg_num)
				{
					return data_num;
				}
				

			}


		}
	   
	 
	}

	else
	{        
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}


		while (pbucket)
		{

			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				goto loop;
			}
  
				EFM_LIST_LOOP (pif->pefm->efm_if_eventlog_list, eventlog, node)
				{
					if (!eventlog)
					{
						continue;
					}
					
					if((eventlog->event_log_Index > log_index) && !ifindex_flag)
					{
						efm_get_eventlog_info(&efm_eventlog_snmp.efm_eventlog_info,eventlog);
						efm_eventlog_snmp.ifindex = pif->ifindex;
						memcpy(&efm_snmp_mux[data_num++],&efm_eventlog_snmp,sizeof(struct efm_eventlog_info_snmp));
						memset(&efm_eventlog_snmp,0,sizeof(struct efm_eventlog_info_snmp));

						if(data_num == msg_num)
						{
							return data_num;

						}
						
					}
				
			
				if(ifindex_flag)
				{
		
					efm_get_eventlog_info(&efm_eventlog_snmp.efm_eventlog_info,eventlog);
					efm_eventlog_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_eventlog_snmp,sizeof(struct efm_eventlog_info_snmp));
					memset(&efm_eventlog_snmp,0,sizeof(struct efm_eventlog_info_snmp));

					if (data_num == msg_num)
					{

						return data_num;
					}
				}

			}
loop:
			pnext = hios_hash_next_cursor(&l2if_table, pbucket);
			if ((NULL == pnext) || (NULL == pnext->data))
			{
				break;
			}

			pbucket = pnext;
			ifindex_flag = EFM_TRUE;
		}



	

	}
 
	
		
	return data_num;
}

int efm_snmp_port_base_info_get(uint32_t ifindex, struct efm_info_snmp efm_snmp_mux[])
{
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pnext	= NULL;
	struct l2if   *pif	   = NULL;
	struct efm_info_snmp efm_snmp = {0};
	int data_num = 0;
	int msg_num  = 0;	
	int cursor = 0;
	int val = 0;
	int i = 0;

	msg_num  = IPC_MSG_LEN/sizeof(struct efm_info_snmp);
	msg_num -=3;
	memset(&efm_snmp,0,sizeof(struct efm_info_snmp));
	EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
	
	if (0 == ifindex)	/* ~{3u4N;qH!~} */
	{
		HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
		{
			pif = (struct l2if *)pbucket->data;
			if ((NULL == pif) || (NULL == pif->pefm))
			{
				continue;
			}
			efm_get_base_info(&efm_snmp.efm_info,pif->pefm);
			efm_snmp.ifindex = pif->ifindex;
			memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_info_snmp));
			memset(&efm_snmp,0,sizeof(struct efm_info_snmp));

			if (data_num == msg_num)
			{
				return data_num;
			}
		}
	}
	else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
	{
		pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
		if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
		{
			/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
			val %= l2if_table.compute_hash((void *)ifindex);

			if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
			{
				pbucket = l2if_table.buckets[val];
			}
			else										/* ~{51G09~O#M0A41mN*?U~} */
			{
				for (++val; val<HASHTAB_SIZE; ++val)
				{
					if (NULL != l2if_table.buckets[val])
					{
						pbucket = l2if_table.buckets[val];
					}
				}
			}
		}

		if (NULL != pbucket)
		{
			for (i=0; i<msg_num; i++)
			{
				pnext = hios_hash_next_cursor(&l2if_table, pbucket);
				if ((NULL == pnext) || (NULL == pnext->data))
				{
					break;
				}

				pif = pnext->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}
				efm_get_base_info(&efm_snmp.efm_info,pif->pefm);
			         efm_snmp.ifindex = pif->ifindex;
			         memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_info_snmp));
				memset(&efm_snmp,0,sizeof(struct efm_info_snmp));

				pbucket = pnext;
			}
		}
	}

	return data_num;
}

	int efm_snmp_port_remloopback_info_get(uint32_t ifindex, struct efm_remloopback_info_snmp efm_snmp_mux[])
	{
		struct hash_bucket *pbucket = NULL;
		struct hash_bucket *pnext	= NULL;
		struct l2if   *pif	   = NULL;
		struct efm_remloopback_info_snmp efm_snmp = {0};
		int data_num = 0;
		int msg_num  = 0;	
		int cursor = 0;
		int val = 0;
		int i = 0;
	
		msg_num  = IPC_MSG_LEN/sizeof(struct efm_remloopback_info_snmp);
		msg_num -=3;
		memset(&efm_snmp,0,sizeof(struct efm_remloopback_info_snmp));
		EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
		
		if (0 == ifindex)	/* ~{3u4N;qH!~} */
		{
			HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
			{
				pif = (struct l2if *)pbucket->data;
				if ((NULL == pif) || (NULL == pif->pefm))
				{
					continue;
				}
				efm_get_remloopback_info(&efm_snmp.efm_remloopback_info,pif->pefm);
				efm_snmp.ifindex = pif->ifindex;
				memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_remloopback_info_snmp));
				memset(&efm_snmp,0,sizeof(struct efm_remloopback_info_snmp));
	
				if (data_num == msg_num)
				{
					return data_num;
				}
			}
		}
		else /* ~{UR5=~} ifindex ~{:sCf5ZR;8vJ}>]~} */
		{
			pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
			if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
			{
				/* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
				val %= l2if_table.compute_hash((void *)ifindex);
	
				if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
				{
					pbucket = l2if_table.buckets[val];
				}
				else										/* ~{51G09~O#M0A41mN*?U~} */
				{
					for (++val; val<HASHTAB_SIZE; ++val)
					{
						if (NULL != l2if_table.buckets[val])
						{
							pbucket = l2if_table.buckets[val];
						}
					}
				}
			}
	
			if (NULL != pbucket)
			{
				for (i=0; i<msg_num; i++)
				{
					pnext = hios_hash_next_cursor(&l2if_table, pbucket);
					if ((NULL == pnext) || (NULL == pnext->data))
					{
						break;
					}
	
					pif = pnext->data;
					if ((NULL == pif) || (NULL == pif->pefm))
					{
						continue;
					}
					efm_get_remloopback_info(&efm_snmp.efm_remloopback_info,pif->pefm);
				         efm_snmp.ifindex = pif->ifindex;
				         memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_remloopback_info_snmp));
					memset(&efm_snmp,0,sizeof(struct efm_remloopback_info_snmp));
	
					pbucket = pnext;
				}
			}
		}
	
		return data_num;
	}
int efm_snmp_port_event_info_get(uint32_t ifindex, struct efm_event_info_snmp efm_snmp_mux[])
		{
			struct hash_bucket *pbucket = NULL;
			struct hash_bucket *pnext	= NULL;
			struct l2if   *pif	   = NULL;
			struct efm_event_info_snmp efm_snmp = {0};
			int data_num = 0;
			int msg_num  = 0;	
			int cursor = 0;
			int val = 0;
			int i = 0;
		
			msg_num  = IPC_MSG_LEN/sizeof(struct efm_event_info_snmp);
			msg_num -=3;
			memset(&efm_snmp,0,sizeof(struct efm_event_info_snmp));
			EFM_LOG_DBG("[%s][%d]msg_num = %d\n",__FUNCTION__,__LINE__,msg_num);
			
			if (0 == ifindex)	/* ~{3u4N;qH!~} */
			{
				HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
				{
					pif = (struct l2if *)pbucket->data;
					if ((NULL == pif) || (NULL == pif->pefm))
					{
						continue;
					}
					efm_get_event_info(&efm_snmp.efm_event_info,pif->pefm);
					efm_snmp.ifindex = pif->ifindex;
					memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_event_info_snmp));
					memset(&efm_snmp,0,sizeof(struct efm_event_info_snmp));
					if (data_num == msg_num)
					{
						return data_num;
					}
				}
			}
			else /* UR5= ifindex ~{:sCf5ZR;8vJ}>]~} */
			{
				pbucket = hios_hash_find(&l2if_table, (void *)ifindex);
				if (NULL == pbucket)	/* ~{N4UR5=~} ifindex ~{6TS&5DJ}>]~} */
				{
            /* ~{2iURJ'0\75;X5=51G09~O#M05DA41mM72?~} */
					val %= l2if_table.compute_hash((void *)ifindex);
		
					if (NULL != l2if_table.buckets[val])	/* ~{51G09~O#M0A41m2;N*?U~} */
					{
						pbucket = l2if_table.buckets[val];
					}
					else										/* ~{51G09~O#M0A41mN*?U~} */
					{
						for (++val; val<HASHTAB_SIZE; ++val)
						{
							if (NULL != l2if_table.buckets[val])
							{
								pbucket = l2if_table.buckets[val];
							}
						}
					}
				}
		
				if (NULL != pbucket)
				{
					for (i=0; i<msg_num; i++)
					{
						pnext = hios_hash_next_cursor(&l2if_table, pbucket);
						if ((NULL == pnext) || (NULL == pnext->data))
						{
							break;
						}
		
						pif = pnext->data;
						if ((NULL == pif) || (NULL == pif->pefm))
						{
							continue;
						}
						efm_get_event_info(&efm_snmp.efm_event_info,pif->pefm);
						efm_snmp.ifindex = pif->ifindex;
						memcpy(&efm_snmp_mux[data_num++],&efm_snmp,sizeof(struct efm_event_info_snmp));
						memset(&efm_snmp,0,sizeof(struct efm_event_info_snmp));
		
						pbucket = pnext;
					}
				}
			}
		
			return data_num;
		}

/*reply snmpd ,send info to snmp*/
void snmp_msg_rcv_efm(struct ipc_msghdr_n *phdr)
{
	int ret = 0;
	int msg_num = 0;
	uint32_t ifindex = 0;


	ifindex = phdr->msg_index;
	EFM_LOG_DBG("[%s][%d]:msg_ifindex = %d\n",__FUNCTION__,__LINE__,ifindex);
	if(EFM_LOCAL_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_local_info_snmp);
		struct efm_local_info_snmp efm_snmp[msg_num];
		memset(efm_snmp, 0, msg_num*sizeof(struct efm_local_info_snmp));
		ret = efm_snmp_port_local_info_get(ifindex, efm_snmp);	 
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_LOCAL_INFO_LEN);
	}

	else if(EFM_PEER_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_peer_info_snmp);
		struct efm_peer_info_snmp efm_snmp[msg_num];
		memset(efm_snmp, 0, msg_num*sizeof(struct efm_peer_info_snmp));			
		ret = efm_snmp_port_peer_info_get(ifindex, efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_PEER_INFO_LEN);
	}

	else if(EFM_LOOPBACK_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_loopback_info_snmp);
		struct efm_loopback_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_loopback_info_snmp));
		ret = efm_snmp_port_loopback_info_get(ifindex,efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_LOOPBACK_INFO_LEN);		 
	}

	else if(EFM_STATS_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_stats_info_snmp);
		struct efm_stats_info_snmp efm_snmp[msg_num];
		memset(efm_snmp, 0, msg_num*sizeof(struct efm_stats_info_snmp)); 		
		ret = efm_snmp_port_stats_info_get(ifindex, efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_STATS_INFO_LEN);
	}

	else if(EFM_EVENT_CONFIG_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_event_config_info_snmp);
		struct efm_event_config_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_event_config_info_snmp));
		ret = efm_snmp_port_event_config_info_get(ifindex,efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_EVENT_CONFIG_INFO_LEN);
	}

	else if(EFM_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_info_snmp);
		struct efm_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_info_snmp));
		ret = efm_snmp_port_base_info_get(ifindex,efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_INFO_LEN);


	}
	else if(EFM_REMLOOPBACK_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_remloopback_info_snmp);
		struct efm_remloopback_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_remloopback_info_snmp));
		ret = efm_snmp_port_remloopback_info_get(ifindex,efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_REMLOOPBACK_INFO_LEN);
	}
	else if(EFM_EVENT_INFO_LEN == phdr->data_num)
	{
		msg_num = IPC_MSG_LEN/sizeof(struct efm_event_info_snmp);
		struct efm_event_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_event_info_snmp));
		ret = efm_snmp_port_event_info_get(ifindex,efm_snmp);
		efm_send_data_to_snmp(phdr,efm_snmp,ret,ifindex,EFM_EVENT_INFO_LEN);
	}


}
void snmp_msg_rcv_efm_eventlog_info(struct ipc_msghdr_n *phdr,uint32_t log_index)
{

	int ret = 0;
	int msg_num = 0;
	uint32_t ifindex = 0;

         ifindex = phdr->msg_index;
	if(EFM_EVENTLOG_INFO_LEN == phdr->data_num)
	{      
		msg_num = IPC_MSG_LEN/sizeof(struct efm_eventlog_info_snmp);
		struct efm_eventlog_info_snmp efm_snmp[msg_num];
		memset(efm_snmp,0,msg_num*sizeof(struct efm_eventlog_info_snmp));
		ret = efm_snmp_port_eventlog_info_get(ifindex,log_index,efm_snmp);
		efm_send_eventlog_data_to_snmp(phdr,efm_snmp,ret,ifindex);
	}




}

int efm_send_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int ret,uint32_t ifindex,uint8_t num)
{
         int data_len =0;
	
         switch(num)
         {
		case EFM_LOCAL_INFO_LEN:
			data_len = ret*sizeof(struct efm_local_info_snmp);
			break;
		case EFM_PEER_INFO_LEN:
			data_len = ret*sizeof(struct efm_peer_info_snmp);
			break;
		case EFM_LOOPBACK_INFO_LEN:
			data_len = ret*sizeof(struct efm_loopback_info_snmp);
			break;
		case EFM_STATS_INFO_LEN:
			data_len = ret*sizeof(struct efm_stats_info_snmp);
			break;
		case EFM_EVENT_CONFIG_INFO_LEN:
			data_len = ret*sizeof(struct efm_event_config_info_snmp);
			break;		
		case EFM_INFO_LEN:
			data_len = ret*sizeof(struct efm_info_snmp);
			break;
		case EFM_REMLOOPBACK_INFO_LEN:
			data_len = ret*sizeof(struct efm_remloopback_info_snmp);
			break;
		case EFM_EVENT_INFO_LEN:
                 		data_len = ret*sizeof(struct efm_event_info_snmp);
			break;

                default:
            return 0;

	}
	EFM_LOG_DBG("[%s][%d]data_num = %d data_len = %d\n",__FUNCTION__,__LINE__,ret,data_len);
	if (ret > 0)
	{
		ipc_ack_to_snmp(phdr,pdata, data_len,ret);
		
	}
	else
	{

		ipc_noack_to_snmp(phdr);
	}

    return 0;


}
int efm_send_eventlog_data_to_snmp(struct ipc_msghdr_n * phdr,void * pdata,int ret,uint32_t ifindex)
{
	EFM_LOG_DBG("[%s][%d]data_num = %d\n",__FUNCTION__,__LINE__,ret);
	if (ret > 0)
	{
		 ipc_ack_to_snmp(phdr,pdata, ret*sizeof(struct efm_eventlog_info_snmp),ret);
	}
	else
	{
		ipc_noack_to_snmp(phdr);
	}

	return ret;


}






