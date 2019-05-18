#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/hash1.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>
#include "l2/l2_if.h"
#include "cfm/cfm_session.h"
#include "aps/elps.h"
#include "aps/elps_pkt.h"
#include "aps/elps_fsm.h"
#include "aps/elps_timer.h"
#include "l2_msg.h"

struct cfm_sess *elps_find_cfm(uint32_t ifindex,uint16_t sess_id)
{
	struct cfm_sess *sess = NULL; 
	sess = cfm_session_lookup_new(sess_id);
	if(sess != 	NULL)
	{
		if(sess->ifindex == ifindex)
		{
			return sess;
		}
	}

	return NULL;
}


int elps_init_session (struct elps_sess *psess)
{
	struct l2if *pif = NULL; 
	struct cfm_sess *sess_master = NULL;
	struct cfm_sess *sess_backup = NULL; 
	int is_change = 0;
	int ret;
	
	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	psess->info.current_status = ELPS_NO_REQUEST;
	psess->info.current_event = APS_ELPS_NR;
	psess->info.active_port = psess->info.master_port;
	if(psess->info.master_cfm_session)
	{
		sess_master = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session);
	}
	if(psess->info.backup_cfm_session)
	{
		sess_backup = elps_find_cfm(psess->info.backup_port,psess->info.backup_cfm_session);
	}
	
	pif = l2if_lookup(psess->info.backup_port);
	if(pif == NULL)
	{
		return ERRNO_FAIL;
	}
	
	if(pif->down_flag == IFNET_LINKDOWN)
	{
		psess->info.current_event = LOCAL_ELPS_SF_P;
		is_change =1;
	}
	else if(sess_backup != NULL)
	{
		if(sess_backup ->state == OAM_STATUS_DOWN)
		{
			psess->info.current_event = LOCAL_ELPS_SF_P;
			is_change =1;
		}	
	}
	
	if(is_change == 0)
	{
		
		pif = l2if_lookup(psess->info.master_port);
		if(pif == NULL)
		{
			return ERRNO_FAIL;
		}
		else if(pif->down_flag == IFNET_LINKDOWN)
		{
			psess->info.current_event = LOCAL_ELPS_SF_W;
			is_change =1;
		} 
		else if (sess_master != NULL)
		{
			if(sess_master ->state == OAM_STATUS_DOWN)
			{
				psess->info.current_event = LOCAL_ELPS_SF_W;
				is_change =1;
			}
		} 
	}

	if(is_change)
	{
		ret = elps_fsm(psess,NULL,psess->info.current_event);
		if(ret)
		{
			ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
			return ret;
		}
	}
	else
	{
		ret =aps_send(psess);
		if(ret)
		{
			ELPS_LOG_DBG("[Func:%s]:aps_send return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
			return ERRNO_FAIL;
		}
		elps_start_msg_timer(psess);
	}

	return ERRNO_SUCCESS;	
}
int elps_local_handle_lop ( struct elps_sess *psess)
{
	int ret;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}	
	psess->info.current_status = ELPS_LOCKOUT_OF_PROTECTION;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = psess->info.master_port;
	if(psess->info.active_port == psess->info.backup_port)
	{
		/*block相应端口*/
		psess->info.switch_action = ELPS_FORWRAD_MASTER;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
    	if(ret != ERRNO_SUCCESS)
    	{
        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        		return ERRNO_IPC;
    	}
		psess->info.active_port = psess->info.master_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	 }
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;
}
int elps_local_handle_fs ( struct elps_sess *psess)
{
	int ret;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = psess->info.master_port;
		
	psess->info.current_status = ELPS_FORCED_SWITCH;
	if(psess->info.active_port == psess->info.master_port)
	{
		/*block相应端口*/
		psess->info.switch_action = ELPS_FORWRAD_BACKUP;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    	if(ret != ERRNO_SUCCESS)
	    	{
	        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        		return ERRNO_IPC;
	    	}
		psess->info.active_port = psess->info.backup_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
	}
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;
}
int elps_local_handle_ms ( struct elps_sess *psess)
{
	int ret;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));	
	
	psess->info.current_status = ELPS_MANUAL_SWITCH;
	if(psess->info.active_port == psess->info.master_port)
	{
		/*block相应端口*/
	    psess->info.switch_action = ELPS_FORWRAD_BACKUP;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    	if(ret != ERRNO_SUCCESS)
	    	{
	        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        		return ERRNO_IPC;
	    	}
		psess->info.active_port = psess->info.backup_port;

		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = psess->info.master_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
	}
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;
}


int elps_local_handle_sf( struct elps_sess *psess)
{
    int ret = ERRNO_FAIL;
	if(psess == NULL)
	{		
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		return ERRNO_FAIL;
	}

	if(psess->info.holdoff == 0)
	{	
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		ret = elps_link_down_handler(psess);	
	}
	else
	{
		if(psess->pholdoff_timer == 0)
		{
		 	elps_start_holdoff_timer(psess);
		}
		else
		{
			ELPS_LOG_DBG("%-15s[Func:%s]:hold off timer is running.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
			return ERRNO_EXISTED;
		}	
	}
	
	return ret;
}

int elps_link_down_handler (struct elps_sess *psess)
{
	struct l2if *pif = NULL; 
	struct cfm_sess *sess_master = NULL;
	struct cfm_sess *sess_backup = NULL;
	int is_change = 0;
	int ret = ERRNO_FAIL;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;	

	ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if(psess == NULL)
	{
		ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		return ERRNO_FAIL;
	}
    if(psess->info.current_status == ELPS_LOCKOUT_OF_PROTECTION)
    {
        return ERRNO_EXISTED;
    }
	if(psess->info.master_cfm_session)
	{
		sess_master = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session);	
	}
	if(psess->info.backup_cfm_session)
	{
		sess_backup = elps_find_cfm(psess->info.backup_port,psess->info.backup_cfm_session); 		
	}
	if(psess->info.active_port == psess->info.backup_port)
	{
		pif = l2if_lookup(psess->info.backup_port);
		if(pif == NULL)
		{
			ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
			return ERRNO_FAIL;
		}
		if(pif->down_flag == IFNET_LINKDOWN)
		{	
			if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
			{
				elps_stop_wtr_timer(psess);
			}
			psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_PROTECTION;
			/*block相应端口*/
			psess->info.switch_action = ELPS_FORWRAD_MASTER;
			ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
		    if(ret != ERRNO_SUCCESS)
		    {
		        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		        return ERRNO_FAIL;
		    }
			psess->info.active_port = psess->info.master_port;
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);

			gPortInfo.iIfindex = psess->info.backup_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			
			is_change = 1;
		}
		else if(sess_backup)
		{			
			ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d sess_backup->state %d",__FILE__,__func__,__LINE__,sess_backup->state);
			if(sess_backup->state == OAM_STATUS_DOWN)
			{
				if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
				{
					elps_stop_wtr_timer(psess);
				}
				psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_PROTECTION;
				/*block相应端口*/
				psess->info.switch_action = ELPS_FORWRAD_MASTER;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
		    	if(ret != ERRNO_SUCCESS)
		    	{
		        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		        		return ERRNO_FAIL;
		    	}
				psess->info.active_port = psess->info.master_port;	
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iIfindex = psess->info.backup_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
				is_change =1 ;
			}
		}
		if(is_change ==0)
		{
            if(psess->info.current_status > ELPS_SIGNAL_FAIL_FOR_WORKING)
            {
                return ERRNO_EXISTED;
            }
			pif = l2if_lookup(psess->info.master_port);
			if(pif == NULL)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				return ERRNO_FAIL;
			}

			if(pif->down_flag == IFNET_LINKDOWN)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
				{
					elps_stop_wtr_timer(psess);
				}
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;	
				psess->info.active_port = psess->info.backup_port;
                is_change =1 ;
			}
			else if(sess_master)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d sess_master->state %d",__FILE__,__func__,__LINE__,sess_master->state);
				if(sess_master->state == OAM_STATUS_DOWN)
				{
					if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
					{
						elps_stop_wtr_timer(psess);
					}
					psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
					psess->info.active_port = psess->info.backup_port;
                    is_change =1 ;
				}
			}
		}	
	}
	else if(psess->info.active_port == psess->info.master_port)
	{
		pif = l2if_lookup(psess->info.backup_port);
		if(pif == NULL)
		{
			ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
			return ERRNO_FAIL;
		}
		
		if(pif->down_flag == IFNET_LINKDOWN)
		{
			ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
			if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
			{
				elps_stop_wtr_timer(psess);
			}
			psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_PROTECTION;
			psess->info.active_port = psess->info.master_port;
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.backup_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			is_change =1;
		}
		else if(sess_backup)
		{
			ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d sess_backup->state %d",__FILE__,__func__,__LINE__,sess_backup->state);
			if(sess_backup->state == OAM_STATUS_DOWN)
			{
				if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
				{
					elps_stop_wtr_timer(psess);
				}
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_PROTECTION;
				psess->info.active_port = psess->info.master_port;
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.backup_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
				is_change =1;
			}
		}
		if(is_change ==0)
		{
            if(psess->info.current_status > ELPS_SIGNAL_FAIL_FOR_WORKING)
            {
                return ERRNO_EXISTED;
            }
			pif = l2if_lookup(psess->info.master_port);
			if(pif == NULL)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				return ERRNO_FAIL;
			}
			if(pif->down_flag == IFNET_LINKDOWN)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
				if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
				{
					elps_stop_wtr_timer(psess);
				}
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
				
				/*block相应端口*/
				psess->info.switch_action = ELPS_FORWRAD_BACKUP;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
			    if(ret != ERRNO_SUCCESS)
			    {
			        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
			        return ERRNO_FAIL;
			    }
				psess->info.active_port = psess->info.backup_port;
                is_change =1 ;
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(sess_master)
			{
				ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d sess_master->state %d",__FILE__,__func__,__LINE__,sess_master->state);
				if(sess_master->state == OAM_STATUS_DOWN)
				{
					if(psess->info.current_status == ELPS_WAIT_TO_RESTORE)
					{
						elps_stop_wtr_timer(psess);
					}
					psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;					
					/*block相应端口*/
					psess->info.switch_action = ELPS_FORWRAD_BACKUP;
					ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
				    if(ret != ERRNO_SUCCESS)
				    {
				        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
				        return ERRNO_FAIL;
				    }
					psess->info.active_port = psess->info.backup_port;
                    is_change =1 ;
					memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = psess->info.master_port;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
				}
			}
		}
	} 
	
    if(is_change)
    {
    	aps_send_burst(psess);
        return ERRNO_SUCCESS;
    }
    else
    {
	    return ERRNO_EXISTED;
    }
}


int elps_local_handle_fsclear ( struct elps_sess *psess)
{
	struct l2if *pif = NULL;
	struct cfm_sess *sess = NULL; 
	int ret;
	int is_change = 0; 

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	
	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	
	if(psess->pholdoff_timer == 0)
	{
		if(psess->info.master_cfm_session)
		{
			sess = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session); 
		}
		pif = l2if_lookup(psess->info.master_port);
		if(pif == NULL)
		{
			return ERRNO_FAIL;
		}
		else if(pif->down_flag == IFNET_LINKDOWN)
		{
			psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
			psess->info.active_port = psess->info.backup_port;	
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
			is_change =1;
		}
		else if(sess != NULL)
		{
			if(sess->state == OAM_STATUS_DOWN)
			{
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
				psess->info.active_port = psess->info.backup_port;	
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
				is_change =1;
			}
		}
		
		
	}
	if(is_change == 0)
	{
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status =ELPS_NO_REQUEST;		
			/*block相应端口*/
			psess->info.switch_action = ELPS_FORWRAD_MASTER;
			ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
		    	if(ret != ERRNO_SUCCESS)
		    	{
		        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		        		return ERRNO_IPC;
		    	}
			psess->info.active_port = psess->info.master_port;
				
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
		else
		{
			psess->info.current_status =ELPS_DO_NOT_REVERT;
			psess->info.active_port = psess->info.backup_port;		
		}
	}
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;
}


int elps_local_handle_msclear ( struct elps_sess *psess)
{
	int ret;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	if(psess->info.failback == FAILBACK_ENABLE)
	{
		psess->info.current_status =ELPS_NO_REQUEST;
		/*block相应端口*/
		psess->info.switch_action = ELPS_FORWRAD_MASTER;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    	if(ret != ERRNO_SUCCESS)
	    	{
	        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        		return ERRNO_IPC;
	    	}
		psess->info.active_port = psess->info.master_port;	
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = psess->info.master_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	}
	else
	{
		psess->info.current_status =ELPS_DO_NOT_REVERT;
	}
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;
}

int elps_local_handle_lopclear ( struct elps_sess *psess)
{
	struct l2if *pif = NULL;
	struct cfm_sess *sess_master = NULL;
	struct cfm_sess *sess_backup = NULL;
	int ret;
	int is_change = 0;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	if(psess->pholdoff_timer == 0)
	{
		if(psess->info.master_cfm_session)
		{
			sess_master = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session);
		}
		if(psess->info.backup_cfm_session)
		{
			sess_backup = elps_find_cfm(psess->info.backup_port,psess->info.backup_cfm_session); 
		}
		pif = l2if_lookup(psess->info.backup_port);
		if(pif == NULL)
		{
			return ERRNO_FAIL;
		}
		else if(pif->down_flag == IFNET_LINKDOWN)
		{
			psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_PROTECTION;
			psess->info.active_port = psess->info.master_port;
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.backup_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			is_change = 1;
			
		}
		else if(sess_backup != NULL)
		{
			if(sess_backup->state == OAM_STATUS_DOWN)
			{
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_PROTECTION;
				psess->info.active_port = psess->info.master_port;
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.backup_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
				is_change = 1;
			}
		}
		
		if(is_change == 0)
		{
				pif = l2if_lookup(psess->info.master_port);
				if(pif == NULL)
				{
					return ERRNO_FAIL;
				}
				else if(pif->down_flag == IFNET_LINKDOWN)
				{
					psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
					/*block相应端口*/
					psess->info.switch_action = ELPS_FORWRAD_BACKUP;
					ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
				    	if(ret != ERRNO_SUCCESS)
				    	{
				        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
				        		return ERRNO_IPC;
				    	}
					psess->info.active_port = psess->info.backup_port;
					memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = psess->info.master_port;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
					
					is_change =1;
					
				}
				else if(sess_master !=NULL)
				{
					if(sess_master ->state == OAM_STATUS_DOWN)
					{
						psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
						/*block相应端口*/
						psess->info.switch_action = ELPS_FORWRAD_BACKUP;
						ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
					    	if(ret != ERRNO_SUCCESS)
					    	{
					        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
					        		return ERRNO_IPC;
					    	}
						psess->info.active_port = psess->info.backup_port;

						memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = psess->info.master_port;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
						is_change =1;
					}
				}
		}
	}
	if(is_change == 0)
	{
		psess->info.current_status=ELPS_NO_REQUEST;
		psess->info.active_port = psess->info.master_port;	
	}
	aps_send_burst(psess);	
	return ERRNO_SUCCESS;

}


int elps_local_handle_rsf ( struct elps_sess *psess)
{
	struct l2if *pif = NULL;
	struct cfm_sess *sess  = NULL;
	int ret;
    int is_change = 0;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
		 
	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	if(psess->info.master_cfm_session)
	{
		sess = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session); 
	}
	if((psess->info.current_event ==LOCAL_ELPS_W_RECOVERS_FROM_SF)&&
		(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_WORKING))
	{
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status = ELPS_WAIT_TO_RESTORE;

			elps_start_wtr_timer(psess);
		}
		else if (psess->info.failback == FAILBACK_DISABLE)
		{
			psess->info.current_status = ELPS_DO_NOT_REVERT;
		}
		
	}
	else if((psess->info.current_event ==LOCAL_ELPS_P_RECOVERS_FROM_SF)&&
		(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION))
	{
		pif = l2if_lookup(psess->info.master_port);
		if(pif == NULL)
		{
			return ERRNO_FAIL;
		}
		else if(pif->down_flag == IFNET_LINKDOWN)
		{
			psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
			/*block相应端口*/
			psess->info.switch_action = ELPS_FORWRAD_BACKUP;
			ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
		    	if(ret != ERRNO_SUCCESS)
		    	{
		        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		        		return ERRNO_IPC;
		    	}
				
			psess->info.active_port = psess->info.backup_port;
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.backup_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);

			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
			
			is_change = 1;
			
		}
		else if(sess != NULL)
		{
			if(sess->state == OAM_STATUS_DOWN)
			{
				psess->info.current_status =ELPS_SIGNAL_FAIL_FOR_WORKING;
				
				/*block相应端口*/
				psess->info.switch_action = ELPS_FORWRAD_BACKUP;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
			    	if(ret != ERRNO_SUCCESS)
			    	{
			        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
			        		return ERRNO_IPC;
			    	}
					
				psess->info.active_port = psess->info.backup_port;
				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.backup_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
				
				is_change = 1;
			}

		}
		
		if(is_change == 0)
		{
			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.backup_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
			
			psess->info.current_status= ELPS_NO_REQUEST;
		}
	}
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}

int elps_local_handle_wtr_timeout(struct elps_sess * psess)
{
	int ret;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	/*block相应端口*/
	psess->info.switch_action = ELPS_FORWRAD_MASTER;
	ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
    	if(ret != ERRNO_SUCCESS)
    	{
        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
        		return ERRNO_IPC;
    	}
	psess->info.active_port = psess->info.master_port;
	psess->info.current_status = ELPS_NO_REQUEST;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
	gPortInfo.iIfindex = psess->info.master_port;
	ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}



int elps_aps_change_to_nrw(struct elps_sess * psess)
{
	int ret;
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	if (psess->info.active_port == psess->info.backup_port)
	{
        ELPS_LOG_DBG("%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
		/*block相应端口*/
		psess->info.switch_action = ELPS_FORWRAD_MASTER;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    if(ret != ERRNO_SUCCESS)
	    {
	        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        return ERRNO_IPC;
	    }
		psess->info.active_port = psess->info.master_port;

		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = psess->info.master_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	}
	else if(psess->info.current_status == ELPS_NO_REQUEST)
	{
		return ERRNO_EXISTED;
	}
	psess->info.current_status= ELPS_NO_REQUEST;
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}

int elps_aps_change_to_nrp(struct elps_sess * psess)
{
	int ret;
	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	if(psess->info.active_port == psess->info.master_port)
	{
		/*block相应端口*/
		psess->info.switch_action = ELPS_FORWRAD_BACKUP;
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    	if(ret != ERRNO_SUCCESS)
	    	{
	        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        		return ERRNO_IPC;
	    	}
		psess->info.active_port = psess->info.backup_port;

		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
		gPortInfo.iIfindex = psess->info.master_port;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
	}
	else if(psess->info.current_status == ELPS_NO_REQUEST)
	{
		return ERRNO_EXISTED;
	}
	psess->info.current_status = ELPS_NO_REQUEST;
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}

int elps_aps_handle_nr(struct elps_sess * psess,struct aps_pkt *aps_pdu)
{
	struct l2if *pif = NULL;
	struct cfm_sess *sess_master = NULL;
	struct cfm_sess *sess_backup = NULL;
	int ret;
	int is_change = 0;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	
	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}
	if(aps_pdu == NULL)
	{
		return ERRNO_FAIL;
	}

	if(psess->info.master_cfm_session)
	{
		sess_master = elps_find_cfm(psess->info.master_port,psess->info.master_cfm_session);
	}
	if(psess->info.backup_cfm_session)
	{
		sess_backup = elps_find_cfm(psess->info.backup_port,psess->info.backup_cfm_session); 
	}
	
	if((aps_pdu->request_signal == 0)&&
		(aps_pdu->bridge_signal == 0))
	{
		if(psess->info.active_port == psess->info.master_port)
		{
			if(psess->pholdoff_timer == 0)
			{
				pif = l2if_lookup(psess->info.backup_port);
				if(pif == NULL)
				{
					return ERRNO_FAIL;
				}
				else if(pif->down_flag == IFNET_LINKDOWN)
				{
					psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_PROTECTION;
					psess->info.active_port = psess->info.master_port;

					memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = psess->info.backup_port;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
					
					is_change =1;
				}
				else if(sess_backup != NULL)
				{
					if(sess_backup->state == OAM_STATUS_DOWN)
					{
						psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_PROTECTION;
						psess->info.active_port = psess->info.master_port;

						memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = psess->info.backup_port;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
						
						is_change =1;
					}
				}
				if(is_change == 0)
				{
					pif = l2if_lookup(psess->info.master_port);
					if(pif == NULL)
					{
						return ERRNO_FAIL;
					}
					else if(pif->down_flag == IFNET_LINKDOWN)
					{
						psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_WORKING;
						psess->info.switch_action = ELPS_FORWRAD_BACKUP;
						/*block相应端口*/
						ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
					    	if(ret != ERRNO_SUCCESS)
					    	{
					        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
					        		return ERRNO_IPC;
					    	}
						psess->info.active_port = psess->info.backup_port;

						memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = psess->info.master_port;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
						
						is_change =1;
					}
					else if(sess_master != NULL)
					{
						if(sess_master->state == OAM_STATUS_DOWN)
						{
							psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_WORKING;
							psess->info.switch_action = ELPS_FORWRAD_BACKUP;
							/*block相应端口*/
							ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
						    	if(ret != ERRNO_SUCCESS)
						    	{
						        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
						        		return ERRNO_IPC;
						    	}
							psess->info.active_port = psess->info.backup_port;

							memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
							gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
							gPortInfo.iIfindex = psess->info.master_port;
							ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
							
							is_change =1;
						}
					}	
				}
			}
			if(is_change == 0)
			{
				psess->info.current_status= ELPS_NO_REQUEST;
				psess->info.active_port = psess->info.master_port;
				return ERRNO_EXISTED;
			}
		}
		else if(psess->info.active_port == psess->info.backup_port)
		{
			if(psess->pholdoff_timer == 0)
			{
				pif = l2if_lookup(psess->info.master_port);
				if(pif == NULL)
				{
					return ERRNO_FAIL;
				}
				else if(pif->down_flag == IFNET_LINKDOWN)
				{
					psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_WORKING;
					psess->info.active_port = psess->info.backup_port;		

					memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = psess->info.master_port;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
					
					is_change =1;
				}
				else if(sess_master!= NULL)
				{
					if(sess_master->state == OAM_STATUS_DOWN)
					{
						psess->info.current_status = ELPS_SIGNAL_FAIL_FOR_WORKING;
						psess->info.active_port = psess->info.backup_port;		

						memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = psess->info.master_port;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
						
						is_change =1;
					}
				}
			}
			if(is_change ==0)
			{
				psess->info.current_status = ELPS_NO_REQUEST;
				
				/*block相应端口*/
				psess->info.switch_action = ELPS_FORWRAD_MASTER;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
			    	if(ret != ERRNO_SUCCESS)
			    	{
			        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
			        		return ERRNO_IPC;
			    	}
				psess->info.active_port = psess->info.master_port;

				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
			}
		}
			
		
	}
	else if ((aps_pdu->request_signal == 1)&&
			(aps_pdu->bridge_signal ==1))
	{
	
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status = ELPS_NO_REQUEST;
			if(psess->info.active_port == psess->info.backup_port)
			{
				/*block相应端口*/
			    psess->info.switch_action = ELPS_FORWRAD_MASTER;
				ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
			    	if(ret != ERRNO_SUCCESS)
			    	{
			        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
			        		return ERRNO_IPC;
			    	}
				psess->info.active_port = psess->info.master_port;

				memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = psess->info.master_port;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
			}
			else
			{
				psess->info.current_status = ELPS_NO_REQUEST;
				return ERRNO_EXISTED;
			}
			
		}
		else if (psess->info.failback == FAILBACK_DISABLE)
		{
			if(psess->info.active_port == psess->info.backup_port)
			{
				psess->info.current_status= ELPS_DO_NOT_REVERT;
			}

			else
			{
				psess->info.current_status = ELPS_NO_REQUEST;
				return ERRNO_EXISTED;
			}
		}
		
	}
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}

int elps_aps_handle_dnr(struct elps_sess *psess)
{
	int ret;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;

	if(psess == NULL)
	{
		return ERRNO_FAIL;
	}

	if(psess->info.failback == FAILBACK_ENABLE)
	{
		
		if(psess->info.active_port == psess->info.master_port)
		{
			/*block相应端口*/
			psess->info.switch_action = ELPS_FORWRAD_BACKUP;
			ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
		    	if(ret != ERRNO_SUCCESS)
		    	{
		        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
		        		return ERRNO_IPC;
		    	}
			psess->info.active_port = psess->info.backup_port;

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}	
		else if(psess->info.current_status == ELPS_NO_REQUEST )
		{
			return ERRNO_EXISTED;
		}
		psess->info.current_status =ELPS_NO_REQUEST;
		
	}
	else if (psess->info.failback == FAILBACK_DISABLE)
	{
		psess->info.current_status =ELPS_DO_NOT_REVERT;
		if(psess->info.active_port == psess->info.master_port)
		{
			/*block相应端口*/
			psess->info.switch_action = ELPS_FORWRAD_BACKUP;
			ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_STG, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    	if(ret != ERRNO_SUCCESS)
	    	{
	        		ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        		return ERRNO_IPC;
	    	}
			psess->info.active_port = psess->info.backup_port;

			memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = psess->info.master_port;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PORT_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}	
	}
	aps_send_burst(psess);
	return ERRNO_SUCCESS;
}

