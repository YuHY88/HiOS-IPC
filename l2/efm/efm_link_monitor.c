#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <lib/hash1.h>
#include <lib/log.h>
#include <lib/linklist.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/alarm.h>
#include <lib/command.h>
#include <sys/socket.h>
#include <sys/types.h>



#include "efm/efm_cmd.h"
#include "efm/efm_state.h"
#include "efm/efm_def.h"
#include "efm/efm_link_monitor.h"
#include "efm/efm.h"
#include "efm/efm_pkt.h"
/*efm alarm for Link Event*/
	int
efm_send_link_event_to_alarm(struct efm_if *pefm,int event_location,uint16_t  event_type)  
{
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = pefm->if_index;
	
	if(EFM_EVENT_LOCAL  == event_location)
	{
		switch(event_type)
		{
			case EFM_ERR_SYMBOL_PERIOD_EVENT:               	
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_SYMB_ERR,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_SYMB_ERR, 0);
				break;   
			case  EFM_ERR_FRAME_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_FRE  ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_FRE, 0);
				break; 
			case EFM_ERR_FRAME_PERIOD_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_UNP ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_UNP, 0);
				break; 
			case EFM_ERR_FRAME_SECONDS_SUM_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_FRES  ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_FRES, 0);
				break; 
			default:
				return EFM_FAILURE;
		}
	}
	else if(EFM_EVENT_REMOTE == event_location)
	{

		switch(event_type)
		{
			case EFM_ERR_SYMBOL_PERIOD_EVENT:               	
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_SYMB_ERR ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_SYMB_ERR, 0);
				break;   
			case  EFM_ERR_FRAME_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_FRE ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_FRE, 0);
				break; 
			case EFM_ERR_FRAME_PERIOD_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_UNP,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_UNP, 0);
				break; 
			case EFM_ERR_FRAME_SECONDS_SUM_EVENT:
				
				//ipran_alarm_event_report(IFM_FUN_ETH_TYPE,IFM_SLOT_ID_GET(pefm->if_index),IFM_PORT_ID_GET(pefm->if_index),0,0,GPN_EVT_TYPE_FUN_EFM_REM_FRES ,0);
				ipran_alarm_event_report(&gPortInfo, GPN_EVT_TYPE_FUN_EFM_REM_FRES, 0);
				break; 
			default:
				return EFM_FAILURE;
		}

	}
	return EFM_SUCCESS;
}
/*efm send link monitor data(threshold + window)*/
int
efm_send_link_monitor_data_to_hal(uint32_t index,uint8_t err_type, uint64_t window,uint64_t threshold)
{
	int data_num = 0,data_len = 0;
	uint64_t link_monitor_data[MAX_DATA_LEN] = {0};
	link_monitor_data[data_num++] =  window;
	link_monitor_data[data_num++] =  threshold;
	data_len = 2*sizeof(uint64_t);	

	#if 0
	 return ipc_send_hal ( link_monitor_data, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2,
			IPC_TYPE_EFM, err_type, IPC_OPCODE_LINK_MONITOR, index );
	#endif
	return ipc_send_msg_n2( link_monitor_data, data_len, data_num, MODULE_ID_HAL, MODULE_ID_L2,
			IPC_TYPE_EFM, err_type, IPC_OPCODE_LINK_MONITOR, index );
}
/* Static function to get current sec and usec.  */
	int
efm_system_uptime (struct timeval *tv)
{
	struct sysinfo info;
	static long prev = 0;
	static unsigned long wraparound_count = 0;
	unsigned long uptime =0;
	static long base = 0;
	static long offset = 0;
	long leap = 0;
	long diff = 0;

	/* Get sysinfo.  */
	if (sysinfo (&info) < 0)
    {
		return -1;
    }

	/* Check for wraparound. */
	if (prev > info.uptime)
    {
		wraparound_count++;
    }

	/* System uptime.  */
	uptime = wraparound_count * WRAPAROUND_VALUE + info.uptime;
	prev = info.uptime;      

	/* Get tv_sec and tv_usec.  */
	gettimeofday (tv, NULL);

	/* Deffernce between gettimeofday sec and uptime.  */
	leap = tv->tv_sec - uptime;

	/* Basically we use gettimeofday's return value because it is the
	   only way to get required granularity.  But when diff is very
	   different we adjust the value using base value.  */
	diff = (leap - base) + offset;

	/* When system time go forward than 2 sec.  */
	if (diff > 2 || diff < -2)
    {
		offset -= diff;
    }

	/* Adjust second.  */
	tv->tv_sec += offset;

	return EFM_SUCCESS;
}
/*get current system time*/
	time_t
efm_time_current (time_t *tp)
{
	struct timeval tv;
	int ret;

	/* Get current time i.e. time since reboot. */
	ret = efm_system_uptime (&tv);
	if (ret != EFM_SUCCESS)
    {
		return -1;
    }

	/* When argument is specified copy value.  */
	if (tp)
    {
		*tp = (time_t) tv.tv_sec;
    }

	return tv.tv_sec;
}

/*efm get time for link event occur*/
	uint16_t
efm_link_event_get_ts (struct timeval *last_event_ts)
{
	uint64_t res = 0;
 	uint64_t temp = 0;
	struct timeval now;
	struct timeval diff;

	efm_system_uptime (&now);

	diff = timeval_subtract (now, *last_event_ts);

	EFM_MUL_32_UINT (diff.tv_sec, EFM_CENTISECS_PER_SECS, res);
	temp = diff.tv_usec / EFM_MICROSECS_PER_CENTI_SECS;
	

	res += temp;

	return (uint16_t)(res & EFM_TIME_STAMP_MASK);
}
/*efm set support link monitor 802.3ah Table 57-8*/
	s_int32_t
efm_link_monitor_support_set (struct efm_if *pefm, u_int8_t enable)
{
	if (NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (EFM_TRUE == enable)
    {
		SET_FLAG (pefm->local_info.oam_config, EFM_CFG_LINK_EVENT_SUPPORT);
    }
	else
    {
		UNSET_FLAG (pefm->local_info.oam_config, EFM_CFG_LINK_EVENT_SUPPORT);
    }

	EFM_LOG_DBG("EFM[EVENT]: EFM Link Event Monitoring is %s\n",
			(CHECK_FLAG (pefm->local_info.oam_config, 
				     EFM_CFG_LINK_EVENT_SUPPORT)) ? "Support" : "Unsupport");

	if (pefm->efm_pdu_cnt != 0)
	{
		pefm->valid_pdu_req = EFM_TRUE;
		efm_run_state_machine (pefm);
	}

	return EFM_SUCCESS;
}


/*efm turn on or off link monitor*/
	s_int32_t
efm_link_monitor_on_set (struct efm_if *pefm, u_int8_t enable)
{
	if (NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (enable)
	{
		SET_FLAG (pefm->local_link_event_flags, EFM_LINK_EVENT_ON);
		pefm->efm_link_monitor_t.link_monitor_on = EFM_TRUE;

		#if 0
		if(ipc_send_hal ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pefm->if_index))
		{
                    	zlog_err ( "%s[%d]:leave %s:error:efm send info to hal turn up link monitor timer  failure!\n", __FILE__, __LINE__, __func__ );
			return EFM_FALSE;
		}
		#endif
		if(ipc_send_msg_n2 ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pefm->if_index))
		{
                    	zlog_err ( "%s[%d]:leave %s:error:efm send info to hal turn up link monitor timer  failure!\n", __FILE__, __LINE__, __func__ );
			return EFM_FALSE;
		}
	}
	else
	{
		UNSET_FLAG (pefm->local_link_event_flags, EFM_LINK_EVENT_ON);

		pefm->efm_link_monitor_t.link_monitor_on = EFM_FALSE;
		#if 0
		if(ipc_send_hal ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pefm->if_index ))
		{
                           zlog_err ( "%s[%d]:leave %s:error:efm send info to hal turn off link monitor timer  failure!\n", __FILE__, __LINE__, __func__ );
			return EFM_FALSE;
		}
		#endif
		if(ipc_send_msg_n2 ( NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_L2,
				IPC_TYPE_EFM, pefm->efm_link_monitor_t.link_monitor_on, IPC_OPCODE_LINK_MONITOR, pefm->if_index ))
		{
                           zlog_err ( "%s[%d]:leave %s:error:efm send info to hal turn off link monitor timer  failure!\n", __FILE__, __LINE__, __func__ );
			return EFM_FALSE;
		}
	}

	EFM_LOG_DBG("EFM[EVENT]: EFM Link Monitoring is %s\n",
			(CHECK_FLAG (pefm->local_info.oam_config,
				     EFM_LINK_EVENT_ON)) ? "Enabled" : "Disabled");

	return EFM_SUCCESS;
}

/*efm process local link events*/

	int
efm_process_local_link_events (struct efm_if *pefm, 
		uint64_t errors, uint16_t event_type)
{       
	//uint64_t tot_errors =0;
	enum efm_pdu pdu = EFM_PDU_INVALID;
	uint8_t event = 0;
	efm_bool notify_flag = EFM_TRUE;

	uint8_t oui[3] = {0x01, 0x80, 0xc2};
	uint8_t high_threshold_exceed = EFM_FALSE;
	uint8_t low_threshold_exceed = EFM_FALSE;

	//memset (&tot_errors, 0, sizeof (uint64_t));

	if (NULL == pefm)
    {
		return EFM_SUCCESS;
    }

	memcpy (oui, pefm->mac, 3);

	switch (event_type)
	{
		case EFM_ERR_SYMBOL_PERIOD_EVENT:
			pdu = EFM_ERR_SYMBOL_PERIOD_EVENT_PDU;
			event = EFM_SYM_PERIOD_EVENT_PENDING;

			pefm->local_link_info.err_symbols = errors;
			low_threshold_exceed = EFM_TRUE;
			pefm->local_link_info.err_symbols_total += errors;
			pefm->local_link_info.err_sym_event_total += 1;
			pefm->local_link_info.err_symbol_period_event_ts = 
				efm_link_event_get_ts(&pefm->local_link_info.last_symbol_period_event_ts);
			efm_system_uptime(&pefm->local_link_info.last_symbol_period_event_ts);

			/*event pdu shouldn't be sent to remote EFM client 
			 * if event_notification is disabled*/
			if (pefm->local_link_info.err_symbol_ev_notify_enable == EFM_NOTIFY_OFF)
			{
				notify_flag = EFM_FALSE;
			}
			break;
		case EFM_ERR_FRAME_EVENT:
			pdu = EFM_ERR_FRAME_EVENT_PDU;
			event = EFM_FRAME_EVENT_PENDING;
			pefm->local_link_info.err_frames = (uint32_t)errors;
			low_threshold_exceed = EFM_TRUE;
			pefm->local_link_info.err_frame_event_total += 1;
			pefm->local_link_info.err_frame_total += errors;

			pefm->local_link_info.err_frame_event_ts = 
				efm_link_event_get_ts(&pefm->local_link_info.last_frame_event_ts);

			efm_system_uptime(&pefm->local_link_info.last_frame_event_ts);

			/*evnt pdu shouldn't be sent to remote EFM client
			 *if event_notification is disabled*/
			if(pefm->local_link_info.err_frame_ev_notify_enable == EFM_NOTIFY_OFF)
            {
				notify_flag = EFM_FALSE;
            }
			break;
		case EFM_ERR_FRAME_PERIOD_EVENT:
			pdu = EFM_ERR_FRAME_PERIOD_EVENT_PDU;
			event = EFM_FRAME_PERIOD_EVENT_PENDING;

			pefm->local_link_info.err_frame_period_frames = (uint32_t)errors;
			low_threshold_exceed = EFM_TRUE;
			pefm->local_link_info.err_frame_period_event_total += 1;
			pefm->local_link_info.err_frame_period_error_total += errors;

			pefm->local_link_info.err_frame_period_event_ts = 
				efm_link_event_get_ts(&pefm->local_link_info.last_frame_period_event_ts);
			efm_system_uptime(&pefm->local_link_info.last_frame_period_event_ts);

			/*evnt pdu shouldn't be sent to remote EFM client
			 *if event_notification is disabled*/
			if(pefm->local_link_info.err_frame_period_ev_notify_enable == EFM_NOTIFY_OFF)
            {
				notify_flag = EFM_FALSE;
            }
			break;
		case EFM_ERR_FRAME_SECONDS_SUM_EVENT:
			pdu = EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU;
			event = EFM_FRAME_SEC_SUM_EVENT_PENDING;

			pefm->local_link_info.err_frame_sec_error = (uint16_t)errors;
			low_threshold_exceed = EFM_TRUE;
			pefm->local_link_info.err_frame_sec_event_total += 1;
			pefm->local_link_info.err_frame_sec_error_total += (uint32_t)errors;

			pefm->local_link_info.err_frame_sec_sum_event_ts = 
				efm_link_event_get_ts(&pefm->local_link_info.last_frame_sec_sum_event_ts);
			efm_system_uptime(&pefm->local_link_info.last_frame_sec_sum_event_ts);

			/*evnt pdu shouldn't be sent to remote EFM client
			 *if event_notification is disabled*/
			if(pefm->local_link_info.err_frame_sec_ev_notify_enable == EFM_NOTIFY_OFF)
            {
				notify_flag = EFM_FALSE; 
            }
			break;

		default:
			return EFM_SUCCESS;
	}

	if ((EFM_TRUE == low_threshold_exceed) || (EFM_TRUE == high_threshold_exceed))
    {
		SET_FLAG (pefm->efm_excess_errrors, EFM_OAM_LOCAL_EXCESS_ERRORS);
    }

	if (EFM_TRUE == low_threshold_exceed)
	{
		/*Local Event has been detected. This has to be logged.*/
		efm_link_event_log_add (pefm, &pefm->local_link_info, 
				event_type, EFM_EVENT_LOCAL, oui); 

		//send efm link-event rise to alarm		
		//include EFM_SYMB_ERR, EFM_FRE, EFM_UNP, EFM_FRES
		efm_send_link_event_to_alarm(pefm,EFM_EVENT_LOCAL,event_type);  

		/*check if event notification is enabled before sending PDU*/
		if (notify_flag != EFM_FALSE)
		{ 
			if ( pefm->efm_pdu_cnt != 0 )
			{
				pefm->valid_pdu_req = EFM_TRUE;
				efm_transmit_state_machine (pefm, pdu);
			}
			else
            {
				SET_FLAG (pefm->efm_pending_link_events, event);
            }

		}
	}

	return EFM_SUCCESS;
}


/*Add a event log entry to the list */
	s_int32_t 
efm_link_event_log_add (struct efm_if *pefm, 
		struct efm_link_event_info *event_info,    
		enum efm_link_event_type event_type, 
		enum efm_event_location ev_location, u_int8_t *oui)
{
	time_t uptime;
	//u_int32_t time_sec, time_min, time_hour, time_day;
	struct efm_event_log_entry *ev_log_entry = NULL; 
	struct efm_event_log_entry *ev_entry_temp = NULL;
	struct efm_event_log_entry *eventlog = NULL;
	struct listnode *node = NULL;

	ev_log_entry = (struct efm_event_log_entry *)XCALLOC( MTYPE_EFM_EVLOG_ENTRY, 
			sizeof(struct efm_event_log_entry) );
	if(!ev_log_entry)
	{  
		zlog_err("Could not allocate memory for EFM Event Log Entry\n");
		return EFM_ERR_MEMORY;
	}

	/*Get the current timestamp. This is represented as number 
	  of seconds elapsed since 1970.*/

	uptime = efm_time_current(NULL);

	ev_log_entry->event_log_timestamp= (uint64_t) uptime;
        
	//ev_log_entry->event_log_timestamp = (uint32_t)pefm->rem_link_info.err_frame_event_ts;

	/*Fill up Event Log Entry structure from 
	 * efm_if->local_link_info struct*/

	memcpy(ev_log_entry->event_log_oui, oui, 3);
	ev_log_entry->event_log_type     = (u_int32_t) event_type;
	ev_log_entry->event_log_location = (s_int32_t) ev_location;    

	/*Window and threshold are only for thresold crossing events*/

	if(event_type <= EFM_ERR_FRAME_SECONDS_SUM_EVENT )  
	{      
		if(EFM_ERR_SYMBOL_PERIOD_EVENT == event_type)
		{
			memcpy(&ev_log_entry->event_log_window, &event_info->err_symbol_window, sizeof(uint64_t));
			memcpy(&ev_log_entry->event_log_threshold, &event_info->err_symbol_threshold, sizeof(uint64_t));
			memcpy(&ev_log_entry->event_log_value, &event_info->err_symbols, sizeof(uint64_t)); 
			memcpy(&ev_log_entry->event_log_running_total, &event_info->err_symbols_total, sizeof(uint64_t));
			ev_log_entry->event_log_event_total = event_info->err_sym_event_total;
		}
		else if (EFM_ERR_FRAME_PERIOD_EVENT == event_type)
		{
			ev_log_entry->event_log_window = (uint64_t)event_info->err_frame_period_window;
			ev_log_entry->event_log_threshold = (uint64_t)event_info->err_frame_period_threshold;
			/* error frames is of size 32 bit.*/      
			ev_log_entry->event_log_value = (uint64_t)event_info->err_frame_period_frames; 

			memcpy( &ev_log_entry->event_log_running_total, 
					&event_info->err_frame_period_error_total, sizeof (uint64_t) );

			ev_log_entry->event_log_event_total = event_info->err_frame_period_event_total;
		}
		else if (EFM_ERR_FRAME_EVENT == event_type)
		{
			ev_log_entry->event_log_window = (uint64_t)event_info->err_frame_window;
			ev_log_entry->event_log_threshold = (uint64_t)event_info->err_frame_threshold;
			/* error frames is of size 32 bit.*/
			ev_log_entry->event_log_value = (uint64_t)event_info->err_frames; 
			memcpy( &ev_log_entry->event_log_running_total,
					&event_info->err_frame_total, sizeof (uint64_t) );
			ev_log_entry->event_log_event_total = (uint64_t)event_info->err_frame_event_total;
		}
		else if (EFM_ERR_FRAME_SECONDS_SUM_EVENT == event_type)
		{  
			ev_log_entry->event_log_window = (uint64_t) 
				event_info->err_frame_sec_sum_window;
			ev_log_entry->event_log_threshold = (uint64_t)
				event_info->err_frame_sec_sum_threshold;

			/* error frames is of size 32 bit.*/
			ev_log_entry->event_log_value = (uint64_t)event_info->err_frame_sec_error; 
			ev_log_entry->event_log_running_total = (uint64_t)event_info->err_frame_sec_error_total;
			ev_log_entry->event_log_event_total = 
				event_info->err_frame_sec_event_total;
		}
	} /*end of if(event_type <= EFM_ERR_FRAME_SECONDS_SUM_EVENT )*/
	else if(event_type >= EFM_LINK_FAULT_EVENT && event_type <= EFM_CRITICAL_LINK_EVENT)
	{
		/*Since this event is not threshold crossing event, following parameters
		  should be set to all 1's.*/ 
		memset ( &ev_log_entry->event_log_window, 0xFF, sizeof(uint64_t) );
		memset ( &ev_log_entry->event_log_threshold, 0xFF, sizeof(uint64_t) );
		memset ( &ev_log_entry->event_log_value, 0xFF, sizeof(uint64_t) );
		memset ( &ev_log_entry->event_log_running_total, 0xFF, sizeof(uint64_t) );
		ev_log_entry->event_log_event_total  = 0xFFFFFFFF; 
	}

	/* find the tail of list */	
	EFM_LIST_LOOP (pefm->efm_if_eventlog_list, eventlog, node)
	{
		if(!eventlog)
        {
			break;
        }
	}

	/* one dying gasp log add  not less than 10s */
	if ((eventlog != NULL) && (EFM_DYING_GASP_EVENT == event_type))
	{
		if(eventlog->event_log_type == ev_log_entry->event_log_type)
		{
			if(ev_log_entry->event_log_timestamp - eventlog->event_log_timestamp < 10)
			{
				XFREE (MTYPE_EFM_EVLOG_ENTRY, ev_log_entry);
				return EFM_SUCCESS;
			}
		}
	}

#if 0
	/* one link-dault log add  not less than 10s */
	if ((eventlog != NULL) && (event_type == EFM_LINK_FAULT_EVENT))
	{
		if(eventlog->event_log_type == ev_log_entry->event_log_type)
		{
			if(ev_log_entry->event_log_timestamp - eventlog->event_log_timestamp < 10)
			{
				XFREE (MTYPE_EFM_EVLOG_ENTRY, ev_log_entry);
				return EFM_SUCCESS;
			}
		}
	}
#endif

	/*assign log index */
	/* pefm->num_event_log_entries++;*/   

	pefm->event_log_index++;  
	ev_log_entry->event_log_Index = pefm->event_log_index;

	/*add the event log to the list*/
	if(pefm->num_event_log_entries < pefm->max_event_log_entries)
	{
		/*event log list isn't full. so new entry can be added 
		  without deletion of existing entry. The new entry is  
		  added to tail of list*/  

		listnode_add (pefm->efm_if_eventlog_list, ev_log_entry);    

		pefm->num_event_log_entries++; 
	}
	else
	{  
		/*event list is full. Delete the oldest entry. 
		  This is the one at the head.*/

		ev_entry_temp = listnode_head (pefm->efm_if_eventlog_list);

		/*list_delete_data (efm_oam->efm_if_eventlog_list, (void*) ev_entry_temp);*/
		listnode_delete (pefm->efm_if_eventlog_list, (void*) ev_entry_temp);

		XFREE(MTYPE_EFM_EVLOG_ENTRY, ev_entry_temp);

		/*now the new entry can be added to the tail*/
		listnode_add (pefm->efm_if_eventlog_list, ev_log_entry);
	}

	return EFM_SUCCESS;
}

/*set efm event log size*/
	s_int32_t 
efm_event_log_size_set (struct efm_if *pefm, uint32_t size)
{
	int i = 0, node_count = 0;
	struct listnode *entry_tmp1 = NULL;
	struct listnode *entry_tmp2 = NULL;

	if (NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if (size < pefm->num_event_log_entries)
	{
		pefm->max_event_log_entries =  size;

		node_count = pefm->num_event_log_entries - size;

		if (pefm->efm_if_eventlog_list != NULL)
		{
			entry_tmp1 = pefm->efm_if_eventlog_list->head;

			if ( NULL == entry_tmp1)
            {
				return EFM_FAILURE;
            }

			entry_tmp2 = entry_tmp1->next;
		}

		/*free the node of exceed max_node count*/
		for (i = 0; i < node_count; i++)
		{			
			if (entry_tmp1->prev)
            {
				entry_tmp1->prev->next = entry_tmp1->next;
            }
			else
            {
				pefm->efm_if_eventlog_list->head = entry_tmp1->next;
            }

			if (entry_tmp1->next)
            {
				entry_tmp1->next->prev = entry_tmp1->prev;
            }
			else
            {
				pefm->efm_if_eventlog_list->tail = entry_tmp1->prev;
            }

			pefm->efm_if_eventlog_list->count--;

			XFREE(MTYPE_EFM_EVLOG_ENTRY, entry_tmp1);

			entry_tmp1 = entry_tmp2;

			if (NULL == entry_tmp2)
            {
				return EFM_FAILURE;
            }
			entry_tmp2 = entry_tmp2->next;
		}

		pefm->num_event_log_entries = size;
	}
	else
		pefm->max_event_log_entries =  size;

	EFM_LOG_DBG("EFM[EVENT]: EFM log size is set to %u\n", size);

	return EFM_SUCCESS;
}

/*efm clear event log content*/
	uint32_t
efm_clear_event_log (struct efm_if *pefm) 
{
	struct efm_event_log_entry *eventlog = NULL;
	struct listnode *node = NULL;
	struct listnode *next_node = NULL;

	if (NULL == pefm)
    {
		return EFM_ERR_NOT_ENABLED;
    }

	if(pefm->efm_if_eventlog_list != NULL)
	{
		EFM_LIST_LOOP_DEL (pefm->efm_if_eventlog_list, eventlog, node, next_node)
		{
			if (!eventlog)
            {
				return EFM_SUCCESS;
            }

			listnode_delete (pefm->efm_if_eventlog_list, eventlog);
			pefm->num_event_log_entries--;
		}
	}

	pefm->event_log_index = 0;  

	return EFM_SUCCESS;
}

/*efm process local critical event*/
	int
efm_process_local_event (struct efm_if *pefm, 
		uint16_t local_event, uint8_t enable)
{
		EFM_LOG_DBG("EFM[EVENT]: Executing EFM OAM Critical Link Event Receiver "
					"send info pdu to remote");

	enum efm_link_event_type event_type = EFM_UNKNOWN_EVENT;
	int event_occurred      = 0;
	uint8_t oui[3] = {0x01, 0x80, 0xc2};

	if (NULL == pefm)
    {
		return EFM_SUCCESS;
    }
         EFM_LOG_DBG("EFM[EVENT]: EFM Critica Link Event  is %s\n",
			   local_event == EFM_LINK_FAULT_EVENT
			   ? "Efm Link Fault event occur" : local_event == EFM_CRITICAL_LINK_EVENT?"Efm Critical Event occur":"Efm Dying Gasp occur");
	memcpy (oui, pefm->mac, 3);
        
	switch (local_event)
	{
		case EFM_LINK_FAULT_EVENT:
			if (enable)
			{  

				pefm->efm_local_link_status  = EFM_OAM_LINK_FAULT;
				SET_FLAG (pefm->efm_flags, EFM_LINK_FAULT);
				event_type = EFM_LINK_FAULT_EVENT;
				event_occurred = EFM_TRUE;            
			}
			else
			{
				pefm->efm_local_link_status  = EFM_OAM_LINK_UP;
				UNSET_FLAG (pefm->efm_flags, EFM_LINK_FAULT);
			}
			break;
		case EFM_CRITICAL_LINK_EVENT:
			if (enable)
			{
				pefm->efm_local_critical_event = EFM_TRUE;

				/* set the Critical Event flag which is used to inform to remote 
				 * OAM client, only if Notification is enabled for 
				 * Critical Event*/

				if (pefm->local_link_info.critical_event_enable == EFM_NOTIFY_ON)
				{
					SET_FLAG (pefm->efm_flags, EFM_CRITICAL_EVENT);
				}
				event_type = EFM_CRITICAL_LINK_EVENT;
				event_occurred = EFM_TRUE;
			}
			else
			{
				pefm->efm_local_critical_event = EFM_FALSE;
				UNSET_FLAG (pefm->efm_flags, EFM_CRITICAL_EVENT);
			}
			break;
		case EFM_DYING_GASP_EVENT:
			if (enable)
			{
				pefm->efm_local_dying_gasp = EFM_TRUE;
				/* set the Dying Gasp flag which is used to inform to remote
				 * OAM client, only if Notification is enabled for
				 * Dying Gasp */

				if (pefm->local_link_info.dying_gasp_enable == EFM_NOTIFY_ON)
				{
					SET_FLAG (pefm->efm_flags, EFM_DYING_GASP);
				}
				event_type = EFM_DYING_GASP_EVENT;
				event_occurred = EFM_TRUE;
			}
			else
			{
				pefm->efm_local_dying_gasp = EFM_FALSE;
				UNSET_FLAG (pefm->efm_flags, EFM_DYING_GASP);
			}
			break;
		default:
			break;
	}

	/*If local event has occurred, it has to be logged.*/
	if (event_occurred)
	{ 
		efm_link_event_log_add (pefm, NULL, event_type, EFM_EVENT_LOCAL, oui);
	}



	if (pefm->efm_local_dying_gasp)
    {
		efm_tx (pefm, EFM_INFORMATION_PDU);
    }

	event_occurred = 0;

	efm_run_state_machine (pefm);


	return 0;
}

/*efm receive local link event*/
	void
efm_recv_local_event (uint16_t local_event, uint8_t enable)
{
	struct efm_if *pefm = NULL;
	struct hash_bucket	*pbucket = NULL;
	int cursor =0;
	struct l2if *pif = NULL;

	HASH_BUCKET_LOOP(pbucket, cursor, l2if_table)
	{
		pif = pbucket->data;

		if (pif != NULL)
		{
			pefm = pif->pefm;
			if (pefm != NULL)
			{       
				efm_process_local_event(pefm, local_event, enable);
			}
		}
	}

	return;

}
/*efm receive link monitor event*/
	int 
efm_link_monitor_event_occur(struct ipc_msghdr_n *phdr,uint64_t *pdata)
{     	
	struct l2if *pif = NULL;
	uint64_t efm_link_frame_errors = 0;

	efm_link_frame_errors = *pdata;

	pif =  l2if_lookup( phdr->msg_index);
	if(NULL == pif ||NULL == pif->pefm)
	{
	  zlog_err ( "%s[%d]:leave %s:pif or pefm has NULL\n", __FILE__, __LINE__, __func__ );
	   return EFM_FAILURE;
	}
	 EFM_LOG_DBG( "%s[%d] ############ to check err_frame_sec PDU,efm_link_frame_errors = %llu",
			__FUNCTION__, __LINE__, efm_link_frame_errors);
	efm_process_local_link_events(pif->pefm, efm_link_frame_errors,(uint16_t)phdr->msg_subtype);

        return EFM_SUCCESS;
}





