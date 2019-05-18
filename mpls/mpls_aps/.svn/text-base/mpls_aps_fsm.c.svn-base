#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/oam_common.h>
#include <lib/command.h>
#include <lib/thread.h>
#include <lib/types.h>
#include <lib/log.h>
#include <lib/alarm.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>


#include <stdlib.h>

#include "pw.h"
#include "tunnel.h"
#include "tunnel_cmd.h"
#include "mpls_main.h"
#include "pw_cmd.h"
#include "mpls_aps/mpls_aps_fsm.h"
#include "lsp_static.h"

extern struct mplsaps_global  gmplsaps;   
extern struct thread_master *mpls_master;

/* wtr */
int mplsaps_wtr_expiry (void *arg)
{
	s_int32_t ret = 0;
    struct aps_sess *psess = NULL;

	OAM_DEBUG();

    psess = (struct aps_sess*)arg;
    psess->pwtr_timer= 0;
	psess->info.current_event = APS_EVENT_LOCAL_WTR;
	
	OAM_DEBUG("psess->info.current_event %d ", psess->info.current_event);	
	
    ret = mplsaps_fsm(psess,NULL, psess->info.current_event);
	if(ret)
	{
		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
	}
    return ret;
}

void mplsaps_start_wtr_timer (struct aps_sess *psess)
{
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return;
	}

	if(0 == psess->pwtr_timer)
	{
		psess->pwtr_timer= COM_THREAD_TIMER_ADD("aps_wtr_timer",LIB_TIMER_TYPE_NOLOOP, mplsaps_wtr_expiry, psess, psess->info.wtr*1000);
	}
}


void mplsaps_stop_wtr_timer (struct aps_sess *psess)
{
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return;
	}

	COM_THREAD_TIMER_OFF(psess->pwtr_timer);
}

/* holdoff */
int mplsaps_holdoff_expiry (void *arg)
{
	int ret = ERRNO_FAIL;
    struct aps_sess *psess = NULL;

	OAM_DEBUG();

	psess = (struct aps_sess*)arg;
	psess->pholdoff_timer = 0;
	
	ret = mplsaps_link_down_handler(psess);	
	if(ret == ERRNO_SUCCESS)
	{
		ret = ipc_send_msg_n2(psess, sizeof(struct aps_sess), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
					IPC_TYPE_MPLSAPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_UPDATE, psess->info.sess_id);
		if(ret != ERRNO_SUCCESS)
		{
			psess->info.status = MPLS_APS_STATUS_DISABLE;

			OAM_ERROR("Send to hal fail.\n");
			return ERRNO_IPC;
		}
	}
	return ERRNO_SUCCESS;
}
void mplsaps_start_holdoff_timer  (struct aps_sess *psess)
{
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ;
	}
	
    if(0 == psess->pholdoff_timer)
	{
		psess->pholdoff_timer = COM_THREAD_TIMER_ADD("mplsaps_holdoff_timer",LIB_TIMER_TYPE_NOLOOP, mplsaps_holdoff_expiry, (void *)psess, psess->info.holdoff*1000);
	}
}

/* Message interval timer */
int mplsaps_msg_expiry (void *arg)
{
	uint32_t ret = 1;
	struct aps_sess *psess = NULL;

	OAM_DEBUG();

	psess = (struct aps_sess*)arg;
	//psess->pkeepalive_timer = 0;
	if(psess->info.status == MPLS_APS_STATUS_ENABLE )
	{
		ret = mplsaps_pkt_send(psess);
		if(ret)
		{
			OAM_ERROR("aps pkt send fail!\n");
		}
	}
	return ret;
}
void mplsaps_start_msg_timer (struct aps_sess *psess)
{
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return;
	}

	if(0 == psess->pkeepalive_timer)
	{
		psess->pkeepalive_timer = COM_THREAD_TIMER_ADD("aps_msg_timer",LIB_TIMER_TYPE_LOOP,mplsaps_msg_expiry,(void *)psess,psess->info.keepalive*1000);
	}
}

void mplsaps_stop_msg_timer (struct aps_sess *psess)
{
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return;
	}

	COM_THREAD_TIMER_OFF(psess->pkeepalive_timer);
}

int mplsaps_updata_backup_status ( struct aps_sess *psess)
{
	int ret = ERRNO_SUCCESS;
	int down_flag = LINK_DOWN;
	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	
	OAM_DEBUG("type %d \n", psess->info.type);
	
	if(psess->info.type == APS_TYPE_PW )
	{
		if(psess->pif == NULL)
		{
			OAM_ERROR("pointer is NULL!\n");
			return ERRNO_FAIL;
		}
		 /* 状态发生变化，切换 */
	   	 if (psess->pif->pw_backup != psess->info.backup)
	     {
			 psess->pif->pw_backup = psess->info.backup;
		     if (( psess->pif->ppw_master != NULL)&&(psess->pif->ppw_slave != NULL))
		     {
	         	psess->pif->ppw_master->pw_backup = psess->info.backup;	
		  		ret = mpls_pw_mplsif_download(psess->pif->ppw_master->pwinfo.pwindex, &psess->pif->pw_backup,
                                        MPLSIF_INFO_PW_BACKUP, OPCODE_UPDATE); /* 涓嬪彂涓?pw 鐨勭储寮?*/
            	if (ERRNO_SUCCESS != ret)
            	{
            	   OAM_DEBUG("IPC send pw status\n");
            	}	
			   	psess->pif->ppw_slave->pw_backup = psess->info.backup;
		    }
			else
		 	{
				OAM_DEBUG("pw_backup != backup\n");
		 		return ERRNO_FAIL;
		 	}
	    }
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if(psess->tunnel_pif)
		{
			psess->tunnel_pif->tunnel.backup_status = psess->info.backup;
			ret = tunnel_if_tunnel_t_download(&psess->tunnel_pif->tunnel, TUNNEL_INFO_BACKUP, OPCODE_UPDATE);
		}
		else
		{
			OAM_DEBUG("tunnel_pif is NULL!\n");
		 	return ERRNO_FAIL;
		}

		if((psess->tunnel_pif->p_mplstp)&&
			(psess->tunnel_pif->p_mplstp->ingress_lsp)&&
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp))
		{
			if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag == LINK_UP)||
				(psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag == LINK_UP))
			{
				down_flag = LINK_UP;
			}
		}
		else
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
		if(psess->tunnel_pif->tunnel.down_flag != down_flag)
		{	
			OAM_DEBUG("tunnel flag %d ", down_flag);
			
			psess->tunnel_pif->tunnel.down_flag = down_flag;
			if (down_flag == LINK_UP)
	        {
	            tunnel_if_up(psess->tunnel_pif);
	        }
	        else
	        {
	           tunnel_if_down(psess->tunnel_pif);
	        }

		   tunnel_if_alarm_process(down_flag, psess->tunnel_pif->tunnel.ifindex);
		}
	}

	return ret;
}

int mplsaps_local_handle_lop ( struct aps_sess *psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	

	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;

	psess->info.current_status = APS_STATE_LO;
	if(psess->info.backup == BACKUP_STATUS_SLAVE)
	{
		psess->info.backup = BACKUP_STATUS_MASTER;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
	}

	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_ERROR("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	mplsaps_send(psess);	
	return ERRNO_SUCCESS;
}
int mplsaps_local_handle_fs ( struct aps_sess *psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	psess->info.current_status = APS_STATE_FS;
	if(psess->info.backup == BACKUP_STATUS_MASTER)
	{
		psess->info.backup= BACKUP_STATUS_SLAVE;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
	}

	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_ERROR("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	 
	mplsaps_send(psess);	
	return ERRNO_SUCCESS;
}
int mplsaps_local_handle_ms ( struct aps_sess *psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	psess->info.current_status = APS_STATE_MS;
	if(psess->info.backup == BACKUP_STATUS_MASTER)
	{
		psess->info.backup= BACKUP_STATUS_SLAVE;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
	}
	
	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_ERROR("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	mplsaps_send(psess);	
	return ERRNO_SUCCESS;
}


int mplsaps_local_handle_sf( struct aps_sess *psess)
{
	int ret = ERRNO_FAIL;

	OAM_DEBUG();
	
	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}

	if(psess->info.holdoff == 0)
	{	
		
		ret = mplsaps_link_down_handler(psess);	
	}
	else
	{
		 if(psess->pholdoff_timer == 0)
		 {
		 	mplsaps_start_holdoff_timer(psess);
		 }
		 else
		{
			 OAM_ERROR("return ERRNO_FAIL!\n");
			return ERRNO_EXISTED;
		}	
	}
	
	return ret;
}

int mplsaps_link_down_handler (struct aps_sess *psess)
{
	int is_change = 0;
	int ret =ERRNO_FAIL;
	uint8_t master_flag = LINK_UP;
	uint8_t backup_flag = LINK_UP;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();
	
	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
    if(psess->info.current_status == APS_STATE_LO)
    {
    	OAM_ERROR("return ERRNO_EXISTED!\n");
        return ERRNO_EXISTED;
    }

	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_ERROR("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
        if((DISABLE == psess->pif->ppw_slave->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_slave->pwinfo.up_flag))
        {
		    backup_flag = LINK_DOWN;
        }
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif == NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_ERROR("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
		    master_flag = LINK_DOWN;
        }
        if((psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->status == DISABLE))
        {
		    backup_flag = LINK_DOWN;
        }
	}

	
	OAM_DEBUG("backup_flag=%d; master_flag=%d ", backup_flag, master_flag);
	
	if(psess->info.backup == BACKUP_STATUS_SLAVE)
	{
		if(backup_flag == LINK_DOWN)
		{	
			if(psess->info.current_status == APS_STATE_WTR)
			{
				mplsaps_stop_wtr_timer(psess);
			}
			psess->info.current_status = APS_STATE_SF_P;
			psess->info.backup= BACKUP_STATUS_MASTER;
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			is_change = 1;
		}
		if(is_change == 0)
		{
            if(psess->info.current_status > APS_STATE_SF_W)
            {
				OAM_ERROR("return ERRNO_FAIL!\n");
                return ERRNO_EXISTED;
            }
			if(master_flag == LINK_DOWN)
			{
				if(psess->info.current_status == APS_STATE_WTR)
				{
					mplsaps_stop_wtr_timer(psess);
				}
				psess->info.current_status = APS_STATE_SF_W;	
				psess->info.backup= BACKUP_STATUS_SLAVE;
				is_change = 1;
			}
		}	
	}
	else if(psess->info.backup == BACKUP_STATUS_MASTER)
	{
		if(backup_flag == LINK_DOWN)
		{
			if(psess->info.current_status == APS_STATE_WTR)
			{
				mplsaps_stop_wtr_timer(psess);
			}
			psess->info.current_status = APS_STATE_SF_P;
			psess->info.backup= BACKUP_STATUS_MASTER;
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			is_change =1;
		}
		if(is_change ==0)
		{
            if(psess->info.current_status > APS_STATE_SF_W)
            {
                return ERRNO_EXISTED;
            }
			if(master_flag == LINK_DOWN)
			{
				if(psess->info.current_status == APS_STATE_WTR)
				{
					mplsaps_stop_wtr_timer(psess);
				}
				psess->info.current_status = APS_STATE_SF_W;
				psess->info.backup= BACKUP_STATUS_SLAVE;
				if(psess->info.type == APS_TYPE_PW )
				{
					gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
				}
				else if(psess->info.type == APS_TYPE_LSP )
				{
					gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
				}
                is_change = 1;
			}
			
		}
	}

	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);	
    if(is_change)
    {
        ret = mplsaps_updata_backup_status(psess);
    	if(ret != ERRNO_SUCCESS)
    	{
    		OAM_ERROR("return ERRNO_FAIL!\n");
    		return ERRNO_FAIL;
    	}
    	mplsaps_send(psess);
        return ERRNO_SUCCESS;
    }
    else
    {
		OAM_ERROR("return ERRNO_EXISTED!\n");
    	return ERRNO_EXISTED;
    }
}


int mplsaps_local_handle_fsclear ( struct aps_sess *psess)
{
	int ret;
	int is_change = 0; 
	uint8_t master_flag = LINK_UP;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
		if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
		    master_flag = LINK_DOWN;
        }
	}
	
	OAM_DEBUG("master_flag=%d.\n", master_flag);
	
	if(psess->pholdoff_timer == 0)
	{
		if(master_flag == LINK_DOWN)
		{
			psess->info.current_status = APS_STATE_SF_W;
			psess->info.backup= BACKUP_STATUS_SLAVE;	
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			is_change = 1;
		}	
	}
	if(is_change == 0)
	{
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status = APS_STATE_NR;		
			psess->info.backup= BACKUP_STATUS_MASTER;
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
			}
		}
		else
		{
			psess->info.current_status = APS_STATE_DNR;
			psess->info.backup= BACKUP_STATUS_SLAVE;	
		}
	}
	
	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);
	
	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	mplsaps_send(psess);	
	return ERRNO_SUCCESS;
}


int mplsaps_local_handle_msclear ( struct aps_sess *psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if(psess->info.failback == FAILBACK_ENABLE)
	{
		psess->info.current_status = APS_STATE_NR;
		psess->info.backup= BACKUP_STATUS_MASTER;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
	}
	else
	{
		psess->info.current_status = APS_STATE_DNR;
		psess->info.backup= BACKUP_STATUS_SLAVE;	
	}

	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);
	
	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	mplsaps_send(psess);	
	return ERRNO_SUCCESS;
}

int mplsaps_local_handle_lopclear ( struct aps_sess *psess)
{
	int ret;
	int is_change = 0;
	uint8_t master_flag = LINK_UP;
	uint8_t backup_flag = LINK_UP;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	if(psess->info.type == APS_TYPE_PW)
	{

		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
        if((DISABLE == psess->pif->ppw_slave->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_slave->pwinfo.up_flag))
        {
		    backup_flag = LINK_DOWN;
        }
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
            master_flag = LINK_DOWN;
        }
        if((psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->status == DISABLE))
        {
            backup_flag = LINK_DOWN;
        }
	}
	
	OAM_DEBUG("backup_flag=%d; master_flag=%d .\n", backup_flag, master_flag);
	
	if(psess->pholdoff_timer == 0)
	{
		if(backup_flag == LINK_DOWN)
		{
			psess->info.current_status = APS_STATE_SF_P;
			psess->info.backup= BACKUP_STATUS_MASTER;	
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
			}
			is_change = 1;
			
		}
		
		if(is_change == 0)
		{
			if(master_flag == LINK_DOWN)
			{
				psess->info.current_status = APS_STATE_SF_W;
				psess->info.backup= BACKUP_STATUS_SLAVE;	
				if(psess->info.type == APS_TYPE_PW )
				{
					gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
				}
				else if(psess->info.type == APS_TYPE_LSP )
				{
					gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
				}
				is_change =1;
				
			}		

		}
	}

	if(is_change == 0)
	{
		psess->info.current_status = APS_STATE_NR;
		psess->info.backup= BACKUP_STATUS_MASTER;	
	}
	
	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);
	
	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	mplsaps_send(psess);	
	return ERRNO_SUCCESS;

}


int mplsaps_local_handle_rsf ( struct aps_sess *psess)
{
	int ret;
    int is_change = 0;
	uint8_t master_flag = LINK_UP;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
		if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
		    master_flag = LINK_DOWN;
        }
	}
	
	OAM_DEBUG("master_flag=%d \n", master_flag);
	
	if((psess->info.current_event == APS_EVENT_LOCAL_W_RECOVER_FROM_SF)&&
		(psess->info.current_status == APS_STATE_SF_W))
	{
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status = APS_STATE_WTR;

			mplsaps_start_wtr_timer(psess);
		}
		else if (psess->info.failback == FAILBACK_DISABLE)
		{
			psess->info.current_status = APS_STATE_DNR;
		}
		
		is_change = 1;
	}
	else if((psess->info.current_event == APS_EVENT_LOCAL_P_RECOVER_FROM_SF)&&
		(psess->info.current_status == APS_STATE_SF_P))
	{
		 if(master_flag == LINK_DOWN)
		 {
			psess->info.current_status = APS_STATE_SF_W;
			psess->info.backup= BACKUP_STATUS_SLAVE;	
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);

				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			is_change = 1;
			
		}		
		if(is_change == 0)
		{
			psess->info.current_status = APS_STATE_NR;
			psess->info.backup= BACKUP_STATUS_MASTER;	
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.backup_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
			}
		}
	}

	OAM_DEBUG("psess->info.current_status %d ", psess->info.current_status);

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}

int mplsaps_local_handle_wtr_timeout(struct aps_sess * psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	psess->info.backup= BACKUP_STATUS_MASTER;	
	psess->info.current_status = APS_STATE_NR;
	
	OAM_DEBUG("psess->info.current_status %d \n", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	if(psess->info.type == APS_TYPE_PW )
	{
		gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.master_index;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	}
	else if(psess->info.type == APS_TYPE_LSP )
	{
		gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
		gPortInfo.iMsgPara1 = psess->info.master_index;
		ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
	}

	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}



int mplsaps_aps_change_to_nrw(struct aps_sess * psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if (psess->info.backup == BACKUP_STATUS_SLAVE)
	{
		psess->info.backup= BACKUP_STATUS_MASTER;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
		}
	}
	else if(psess->info.current_status == APS_STATE_NR)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_EXISTED;
	}
	psess->info.current_status = APS_STATE_NR;

	OAM_DEBUG("psess->info.current_status %d \n", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}

int mplsaps_aps_change_to_nrp(struct aps_sess * psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}	
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	if(psess->info.backup == BACKUP_STATUS_MASTER)
	{
		psess->info.backup= BACKUP_STATUS_SLAVE;
		if(psess->info.type == APS_TYPE_PW )
		{
			gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
		else if(psess->info.type == APS_TYPE_LSP )
		{
			gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
			gPortInfo.iMsgPara1 = psess->info.master_index;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
		}
	}
	else if(psess->info.current_status == APS_STATE_NR)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_EXISTED;
	}
	psess->info.current_status = APS_STATE_NR;

	OAM_DEBUG("psess->info.current_status %d.\n", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}
	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}

int mplsaps_aps_handle_nr(struct aps_sess * psess,struct mplsaps_pkt *aps_pdu)
{
	int ret;
	int is_change = 0;
	uint8_t master_flag = LINK_UP;
	uint8_t backup_flag = LINK_UP;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	if(aps_pdu == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	
	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
        if((DISABLE == psess->pif->ppw_slave->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_slave->pwinfo.up_flag))
        {
		    backup_flag = LINK_DOWN;
        }
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
            master_flag = LINK_DOWN;
        }
        if((psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->status == DISABLE))
        {
            backup_flag = LINK_DOWN;
        }
	}
	
	OAM_DEBUG("backup_flag=%d; master_flag=%d.\n", backup_flag, master_flag);
	
	if((aps_pdu->request_signal == APS_REQUEST_SIGNAL_NULL)&&
		(aps_pdu->bridge_signal == APS_REQUEST_SIGNAL_NULL))
	{
		if(psess->info.backup == BACKUP_STATUS_MASTER)
		{
			if(psess->pholdoff_timer == 0)
			{
				if(backup_flag == LINK_DOWN)
				{
					psess->info.current_status = APS_STATE_SF_P;
					psess->info.backup= BACKUP_STATUS_MASTER;
					if(psess->info.type == APS_TYPE_PW )
					{
						gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
						gPortInfo.iMsgPara1 = psess->info.backup_index;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
					}
					else if(psess->info.type == APS_TYPE_LSP )
					{
						gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
						gPortInfo.iMsgPara1 = psess->info.backup_index;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS_FAIL, GPN_SOCK_MSG_OPT_RISE);
					}
					is_change = 1;
				}
				if(is_change == 0)
				{
					if(master_flag  == LINK_DOWN)
					{
						psess->info.current_status = APS_STATE_SF_W;					
						psess->info.backup= BACKUP_STATUS_SLAVE;
						if(psess->info.type == APS_TYPE_PW )
						{
							gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
							gPortInfo.iMsgPara1 = psess->info.master_index;
							ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
						}
						else if(psess->info.type == APS_TYPE_LSP )
						{
							gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
							gPortInfo.iMsgPara1 = psess->info.master_index;
							ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
						}
						is_change = 1;
					}								
				}
			}
			if(is_change == 0)
			{
				psess->info.current_status = APS_STATE_NR;
				psess->info.backup= BACKUP_STATUS_MASTER;
				
				OAM_DEBUG("return ERRNO_EXISTED!\n");
				return ERRNO_EXISTED;
			}
		
		}
		else if(psess->info.backup == BACKUP_STATUS_SLAVE)
		{
			if(psess->pholdoff_timer == 0)
			{
				if(master_flag == LINK_DOWN)
				{
					psess->info.current_status = APS_STATE_SF_W;
					psess->info.backup= BACKUP_STATUS_SLAVE;		
					if(psess->info.type == APS_TYPE_PW )
					{
						gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
						gPortInfo.iMsgPara1 = psess->info.master_index;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
					}
					else if(psess->info.type == APS_TYPE_LSP )
					{
						gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
						gPortInfo.iMsgPara1 = psess->info.master_index;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
					}
					is_change = 1;
				}
			}
			if(is_change == 0)
			{
				psess->info.current_status = APS_STATE_NR;
				psess->info.backup= BACKUP_STATUS_MASTER;
				if(psess->info.type == APS_TYPE_PW )
				{
					gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if(psess->info.type == APS_TYPE_LSP )
				{
					gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
				}
			}
		}
			
	}
	else if ((aps_pdu->request_signal == APS_REQUEST_SIGNAL_NORMAL)&&
			(aps_pdu->bridge_signal == APS_REQUEST_SIGNAL_NORMAL))
	{
	
		if(psess->info.failback == FAILBACK_ENABLE)
		{
			psess->info.current_status = APS_STATE_NR;
			if(psess->info.backup == BACKUP_STATUS_SLAVE)
			{
				psess->info.backup= BACKUP_STATUS_MASTER;
				if(psess->info.type == APS_TYPE_PW )
				{
					gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
				}
				else if(psess->info.type == APS_TYPE_LSP )
				{
					gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
					gPortInfo.iMsgPara1 = psess->info.master_index;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_CLEAN);
				}
			}
			else
			{
				psess->info.current_status = APS_STATE_NR;
				
				OAM_DEBUG("return ERRNO_EXISTED!\n");
				return ERRNO_EXISTED;
			}
			 
		}
		else if (psess->info.failback == FAILBACK_DISABLE)
		{
			if(psess->info.backup == BACKUP_STATUS_SLAVE)
			{
				psess->info.current_status = APS_STATE_DNR;
			}

			else
			{
				psess->info.current_status = APS_STATE_NR;
				
				OAM_DEBUG("return ERRNO_EXISTED!\n");
				return ERRNO_EXISTED;
			}
		}
		
	}

	OAM_DEBUG("psess->info.current_status %d.\n", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}

int mplsaps_aps_handle_dnr(struct aps_sess *psess)
{
	int ret;
	struct gpnPortInfo gPortInfo;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));

	if(psess->info.failback == FAILBACK_ENABLE)
	{
		
		if(psess->info.backup == BACKUP_STATUS_MASTER)
		{
			psess->info.backup= BACKUP_STATUS_SLAVE;
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
		}	
		else if(psess->info.current_status == APS_STATE_NR )
		{
			OAM_DEBUG("return ERRNO_EXISTED!\n");
			return ERRNO_EXISTED;
		}
		psess->info.current_status = APS_STATE_NR;
		
	}
	else if (psess->info.failback == FAILBACK_DISABLE)
	{
		psess->info.current_status = APS_STATE_DNR;
		if(psess->info.backup == BACKUP_STATUS_MASTER)
		{
			psess->info.backup= BACKUP_STATUS_SLAVE;
			if(psess->info.type == APS_TYPE_PW )
			{
				gPortInfo.iAlarmPort = IFM_PW_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_PW_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
			else if(psess->info.type == APS_TYPE_LSP )
			{
				gPortInfo.iAlarmPort = IFM_LSP_2_TYPE;
				gPortInfo.iMsgPara1 = psess->info.master_index;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_LSP_PS2P, GPN_SOCK_MSG_OPT_RISE);
			}
		}	
	}

	OAM_DEBUG("psess->info.current_status %d.\n", psess->info.current_status);	

	ret = mplsaps_updata_backup_status(psess);
	if(ret != ERRNO_SUCCESS)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	}

	mplsaps_send(psess);
	return ERRNO_SUCCESS;
}


int mplsaps_fsm(struct aps_sess *psess, struct mplsaps_pkt * aps_pdu,uint8_t current_event)
{
	int ret = ERRNO_FAIL;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	if(psess->info.status == MPLS_APS_STATUS_DISABLE)
	{
		OAM_DEBUG("return ERRNO_FAIL!\n");
		return ERRNO_FAIL;
	} 

	OAM_DEBUG("session id = %d .\n", psess->info.sess_id);
	switch(current_event)
	{
		/*远端事件*/
		case APS_EVENT_REMOTE_NR:
			if(psess->info.current_status == APS_STATE_NR)
			{
				ret = mplsaps_aps_handle_nr(psess, aps_pdu);
			}
			break;
		case APS_EVENT_REMOTE_DNR:
			if(psess->info.current_status < APS_STATE_DNR)
			{
				ret = mplsaps_aps_handle_dnr(psess);
			}
			break;
		case APS_EVENT_REMOTE_WTR:
			if(psess->info.current_status < APS_STATE_WTR)
			{
				ret = mplsaps_aps_change_to_nrp(psess);
			}
			break;
		case APS_EVENT_REMOTE_MS:
			if(psess->info.current_status < APS_STATE_MS)
			{
				ret = mplsaps_aps_change_to_nrp(psess);
			}
			break;
		case APS_EVENT_REMOTE_SF_W:
			if(psess->info.current_status < APS_STATE_SF_W)
			{
				ret = mplsaps_aps_change_to_nrp(psess);	
			}
			break;
		case APS_EVENT_REMOTE_FS:
			if(psess->info.current_status < APS_STATE_FS)
			{
				ret = mplsaps_aps_change_to_nrp(psess);
			}
			break;
		case APS_EVENT_REMOTE_SF_P:
			if(psess->info.current_status < APS_STATE_SF_P)
			{
				ret = mplsaps_aps_change_to_nrw(psess);
			}
			break;
		case APS_EVENT_REMOTE_LO:
			if(psess->info.current_status != APS_STATE_LO)
			{
				ret = mplsaps_aps_change_to_nrw(psess);
			}
			break;
		/*本地事件*/
		case APS_EVENT_LOCAL_LO:
			if(psess->info.current_status != APS_STATE_LO)
			{
				ret = mplsaps_local_handle_lop(psess);
			}
			break;
		case APS_EVENT_LOCAL_SF_P:
			if(psess->info.current_status < APS_STATE_SF_P)
			{
				ret = mplsaps_local_handle_sf(psess);
			}
			break;
		case APS_EVENT_LOCAL_FS:
			if(psess->info.current_status < APS_STATE_FS)
			{
				ret = mplsaps_local_handle_fs(psess);
			} 
			break;
		case APS_EVENT_LOCAL_SF_W:
			if(psess->info.current_status < APS_STATE_SF_W)
			{ 
				ret = mplsaps_local_handle_sf(psess);
			}
			break;
		case APS_EVENT_LOCAL_W_RECOVER_FROM_SF:
			if(psess->info.current_status == APS_STATE_SF_W)
			{
				ret = mplsaps_local_handle_rsf(psess);
			}
			break;
		case APS_EVENT_LOCAL_P_RECOVER_FROM_SF:
			if(psess->info.current_status == APS_STATE_SF_P)
			{
				ret = mplsaps_local_handle_rsf(psess);
			}
			break;
		case APS_EVENT_LOCAL_MS:
			if(psess->info.current_status < APS_STATE_MS)
			{
				ret = mplsaps_local_handle_ms(psess);
			}
			break;
		case APS_EVENT_LOCAL_CLEAR:
			if(psess->info.current_status == APS_STATE_FS)
			{
				ret = mplsaps_local_handle_fsclear(psess);
			}
			else if(psess->info.current_status == APS_STATE_MS)
			{
				ret = mplsaps_local_handle_msclear(psess);
			}
			else if(psess->info.current_status ==APS_STATE_LO)
			{
				ret = mplsaps_local_handle_lopclear(psess);
			}
			break;
		case APS_EVENT_LOCAL_WTR:
			if(psess->info.current_status == APS_STATE_WTR)
			{
				ret = mplsaps_local_handle_wtr_timeout(psess);
			}
			break;
		default:
			break;
	}

	if(ret == ERRNO_SUCCESS)
	{
		ret = ipc_send_msg_n2(psess, sizeof(struct aps_sess), 1, MODULE_ID_HAL, MODULE_ID_MPLS,
					IPC_TYPE_MPLSAPS, OAM_SUBTYPE_SESSION, IPC_OPCODE_UPDATE, psess->info.sess_id);
		if(ret != ERRNO_SUCCESS)
		{
			psess->info.status = MPLS_APS_STATUS_DISABLE;
			
			OAM_ERROR("aps Send to hal fail.\n");
			return ERRNO_IPC;
		}
	}
	return ERRNO_SUCCESS;
}


int mplsaps_init_session (struct aps_sess *psess)
{
	int is_change = 0;
	int ret;
	uint8_t master_flag = LINK_UP;
	uint8_t backup_flag = LINK_UP;
	
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	
	
	psess->info.current_status = APS_STATE_NR;
	psess->info.current_event = APS_EVENT_REMOTE_NR;
	psess->info.backup = BACKUP_STATUS_MASTER;
	
	OAM_DEBUG("psess->info.backup %d.\n", psess->info.backup);
	
	if(psess->info.type == APS_TYPE_PW)
	{
		if((psess->pif == NULL)||(psess->pif->ppw_master == NULL)||(psess->pif->ppw_slave == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        
        if((DISABLE == psess->pif->ppw_master->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_master->pwinfo.up_flag))
        {
		    master_flag = LINK_DOWN;
        }
        if((DISABLE == psess->pif->ppw_slave->pwinfo.status)||
            (LINK_DOWN == psess->pif->ppw_slave->pwinfo.up_flag))
        {
		    backup_flag = LINK_DOWN;
        }
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
			(psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
			(psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
		{
			OAM_DEBUG("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
        if((psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->ingress_lsp->status == DISABLE))
        {
            master_flag = LINK_DOWN;
        }
        if((psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag)||
            (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->status == DISABLE))
        {
            backup_flag = LINK_DOWN;
        }
	}

	OAM_DEBUG("backup_flag=%d; master_flag=%d.\n", backup_flag, master_flag);
	
	if(backup_flag == LINK_DOWN)
	{
		psess->info.current_event = APS_EVENT_LOCAL_SF_P;
		is_change = 1;
	}
	
	
	if(is_change == 0)
	{
		if(master_flag == LINK_DOWN)
		{
			psess->info.current_event = APS_EVENT_LOCAL_SF_W;
			is_change = 1;
		} 
	}
	
	if(is_change)
	{
		OAM_DEBUG("psess->info.current_event %d.\n ", psess->info.current_event);	

		ret = mplsaps_fsm(psess, NULL,psess->info.current_event);
		if(ret)
		{
			OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
			return ret;
		}
	}
	else
	{
		ret = mplsaps_pkt_send(psess);
		if(ret)
		{
			OAM_ERROR("return ERRNO_FAIL!\n");
			return ERRNO_FAIL;
		}
		mplsaps_start_msg_timer(psess);
	}

	return ERRNO_SUCCESS;	
}

int mplsaps_state_update(uint16_t sess_id, enum APS_OAM_STATE_E event)
{
	struct aps_sess *psess = NULL;
	int ret ;

	OAM_DEBUG();
	
	 psess = mplsaps_sess_lookup(sess_id); 
	 if(psess == NULL)
	 {
		OAM_ERROR("return ERRNO_NOT_FOUND!\n");
		return ERRNO_NOT_FOUND;
	 }
     if(MPLS_APS_STATUS_DISABLE == psess->info.status)
     {
		OAM_ERROR("return ERRNO_NOT_FOUND!\n");
		return ERRNO_NOT_FOUND;
     }
	 
	 if(event == APS_OAM_STATE_SF_DOWN_W)
	 {
		psess->info.current_event = APS_EVENT_LOCAL_SF_W;
	 }
	 else if(event == APS_OAM_STATE_SF_DOWN_P)
	 {
		psess->info.current_event = APS_EVENT_LOCAL_SF_P;
	 }
	 else if (event == APS_OAM_STATE_SF_UP_W)
	 {
		if(psess->info.current_status == APS_STATE_SF_W)
		{
            if(psess->info.type ==APS_TYPE_PW )
            {
                if((NULL == psess->pif)||(NULL == psess->pif->ppw_master))
                {
					OAM_ERROR("return ERRNO_FAIL!\n");
                    return ERRNO_FAIL;
                }
                if((LINK_UP == psess->pif->ppw_master->pwinfo.up_flag)&&
                    (ENABLE == psess->pif->ppw_master->pwinfo.status))
                {
			        psess->info.current_event = APS_EVENT_LOCAL_W_RECOVER_FROM_SF;
                }
            }
            else if(psess->info.type == APS_TYPE_LSP)
            {
                if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
                    (psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
                    (psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
                {
					OAM_ERROR("return ERRNO_FAIL!\n");
                    return ERRNO_FAIL;
                }
                if( (psess->tunnel_pif->p_mplstp->ingress_lsp->down_flag == LINK_UP)&&
                    (psess->tunnel_pif->p_mplstp->ingress_lsp->status == ENABLE))
                {
                    psess->info.current_event = APS_EVENT_LOCAL_W_RECOVER_FROM_SF;
                }

            }
		}
		else
		{
			OAM_DEBUG("return ERRNO_SUCCESS!\n");
			return ERRNO_SUCCESS;
		}
	 }
	 else if (event == APS_OAM_STATE_SF_UP_P)
	 {
		if(psess->info.current_status == APS_STATE_SF_P)
		{
            if(psess->info.type ==APS_TYPE_PW )
            {
                if((NULL == psess->pif)||(NULL == psess->pif->ppw_slave))
                {
					OAM_DEBUG("return ERRNO_FAIL!\n");
                    return ERRNO_FAIL;
                }
                if((LINK_UP == psess->pif->ppw_slave->pwinfo.up_flag)&&
                    (ENABLE == psess->pif->ppw_slave->pwinfo.status))
                {
                    psess->info.current_event = APS_EVENT_LOCAL_P_RECOVER_FROM_SF;
                }
            }
            else if(psess->info.type == APS_TYPE_LSP)
            {
                if((psess->tunnel_pif ==NULL)||(psess->tunnel_pif->p_mplstp == NULL)||
                    (psess->tunnel_pif->p_mplstp->ingress_lsp == NULL)||
                    (psess->tunnel_pif->p_mplstp->backup_ingress_lsp == NULL))
                {
					OAM_DEBUG("return ERRNO_FAIL!\n");
                    return ERRNO_FAIL;
                }
                if( (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->down_flag == LINK_UP)&&
                    (psess->tunnel_pif->p_mplstp->backup_ingress_lsp->status == ENABLE))
                {
                    psess->info.current_event = APS_EVENT_LOCAL_P_RECOVER_FROM_SF;
                }
            }
		}
		else
		{
			OAM_DEBUG("return ERRNO_SUCCESS!\n");
			return ERRNO_SUCCESS;
		}
	 }
	 else
	 {
		OAM_DEBUG("return ERRNO_SUCCESS!\n");
		return ERRNO_SUCCESS;
	 }
	 
	OAM_DEBUG("psess->info.current_event %d.\n", psess->info.current_event);
	 
	ret = mplsaps_fsm(psess, NULL,psess->info.current_event);
	if(ret)
	{
		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
		return ret;
	}
	return ERRNO_SUCCESS;	
	
}


int mplsaps_pkt_send(struct aps_sess *psess)
{
	union pkt_control pktcontrol;
	struct aps_pkt_lsp aps_pkt_lsp;
	struct mplsaps_pkt  aps_pdu;
	uint32_t label = 13;
	int ret = ERRNO_FAIL ;

	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	memset(&aps_pdu, 0, sizeof(struct mplsaps_pkt));

	aps_pdu.level = 7;
	aps_pdu.opcode = CFM_OPCODE_APS;
	aps_pdu.tlv_offset = 0x04;
	aps_pdu.request_state = psess->info.current_status;
	if(psess->info.backup == BACKUP_STATUS_MASTER )
	{
		aps_pdu.request_signal = 0;
		aps_pdu.bridge_signal = 0;
	}
	else if(psess->info.backup == BACKUP_STATUS_SLAVE)
	{
		aps_pdu.request_signal = 1;
		aps_pdu.bridge_signal = 1;
	}
	aps_pdu.port_type_A = 1;
	aps_pdu.port_type_B = 1;
	aps_pdu.port_type_D = 1;
	if(psess->info.failback == FAILBACK_ENABLE)
	{
		aps_pdu.port_type_R = 1;
	}
	else
	{
		aps_pdu.port_type_R = 0;
	}	
		
	if(psess->info.type == APS_TYPE_PW)
	{
		/* 浣跨敤mpls control */
		memset(&pktcontrol, 0, sizeof(struct mpls_control));
		pktcontrol.mplscb.ifindex = psess->info.backup_index;
		pktcontrol.mplscb.if_type = PKT_INIF_TYPE_PW;
		pktcontrol.mplscb.chtype = 0x8902;
		pktcontrol.mplscb.ttl = 255;
		pktcontrol.mplscb.exp = 5;
		pktcontrol.mplscb.is_changed = 1;
		ret = pkt_send(PKT_TYPE_MPLS, &pktcontrol, &aps_pdu, sizeof(struct mplsaps_pkt));
		
	}
	else if(psess->info.type == APS_TYPE_LSP)
	{
		/* 使用mpls control */
		memset(&pktcontrol, 0, sizeof(struct mpls_control));
		memset(&aps_pkt_lsp, 0, sizeof(struct aps_pkt_lsp));
		pktcontrol.mplscb.ifindex = psess->info.backup_index;
		pktcontrol.mplscb.if_type = PKT_INIF_TYPE_LSP;
		pktcontrol.mplscb.chtype = 0;
		pktcontrol.mplscb.exp = 5;
		pktcontrol.mplscb.ttl = 255;
		pktcontrol.mplscb.is_changed = 1;		
		
		aps_pkt_lsp.label.label1 = htons(label >> 4);  /* get high 16bit */
		aps_pkt_lsp.label.label0 = label & 0xf; 	   /* get low 4bit */
		aps_pkt_lsp.label.exp = 5;
		aps_pkt_lsp.label.bos = 1;
		aps_pkt_lsp.label.ttl = 1;
		aps_pkt_lsp.ctrl_word = htonl(0x10000000 + 0x8902);

		pktcontrol.mplscb.label_num = 1;
		
		memcpy(&aps_pkt_lsp.lb_pkt, &aps_pdu, sizeof(struct mplsaps_pkt));
		ret = pkt_send(PKT_TYPE_MPLS, &pktcontrol, &aps_pkt_lsp, sizeof(struct aps_pkt_lsp));
	}

	if(ret == NET_FAILED)
	{
		OAM_ERROR("aps pkt_send fail.\n");
		return ERRNO_FAIL;
	}
		
	gmplsaps.pkt_send++;
	if(gmplsaps.debug_packet)
	{
		printf("Send APS Packet : %s\n", pkt_dump(&aps_pdu,sizeof(struct mplsaps_pkt)));
	}
	
	return ERRNO_SUCCESS;
	
}

int mplsaps_send (struct aps_sess *psess)
{	
	int i ;
	
	OAM_DEBUG();

	if(psess == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	
	mplsaps_stop_msg_timer(psess); 
	for(i = 0; i<3; i++)
	{ 
		mplsaps_pkt_send(psess);
	}
	psess->pkeepalive_timer = 0;
	
	mplsaps_start_msg_timer(psess);
	return ERRNO_SUCCESS;
}

int mpls_aps_pkt_recv(struct pkt_buffer *ppkt)
{
	struct mplsaps_pkt  pkt_rcv;
	struct aps_sess *psess = NULL;
	uchar *pname = NULL; 
	uchar *lspname = NULL;
	struct static_lsp *plsp = NULL;
	struct l2vc_entry *l2vc_entry = NULL;
	int ret = 0;

	OAM_DEBUG();

	if(NULL == ppkt)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	if(ppkt->data == NULL)
	{
		OAM_ERROR("pointer is NULL!\n");
		return ERRNO_FAIL;
	}
	gmplsaps.pkt_recv++;
	
	if(ppkt->inif_type == PKT_INIF_TYPE_PW)
	{
		pname = pw_get_name(ppkt->in_ifindex);
		if(pname == NULL)
		{
			gmplsaps.pkt_err++;
		
			OAM_ERROR("Pw is not exit!\n");
			return ERRNO_FAIL;
		}
		
	    l2vc_entry = l2vc_lookup(pname);
		if(l2vc_entry == NULL)
		{
			gmplsaps.pkt_err++;
		
			OAM_ERROR("L2vc_entry is not exit!\n");
			return ERRNO_FAIL;
		}
		else
		{
			psess = mplsaps_sess_lookup(l2vc_entry->aps_id);
			if(NULL == psess)
			{
				gmplsaps.pkt_err++;

				OAM_ERROR("Psess is not exist!\n");
				return ERRNO_FAIL;
			}
		}
		
		if(psess->info.backup_index != ppkt->in_ifindex)
		{
			gmplsaps.pkt_err++;
		
			OAM_ERROR("APS frame received not on protection path.\n");
			return ERRNO_FAIL;
		}

	}
	else if(ppkt->inif_type == PKT_INIF_TYPE_LSP)
	{
		lspname = static_lsp_get_name(ppkt->in_ifindex);
		if(lspname == NULL)
		{
			gmplsaps.pkt_err++;
			
			OAM_ERROR("Lsp is not exit!\n");
			return ERRNO_FAIL;
		}
		
	    plsp = static_lsp_lookup(lspname);
		if(plsp == NULL)
		{
			gmplsaps.pkt_err++;

			OAM_ERROR("Plsp is not exit!\n");
			return ERRNO_FAIL;
		}
		else
		{
			psess = mplsaps_sess_lookup(plsp->aps_id);
			if((NULL == psess)||(psess->tunnel_pif == NULL)||(psess->tunnel_pif->p_mplstp == NULL) ||
				(psess->tunnel_pif->p_mplstp->backup_egress_lsp == NULL))
			{
				gmplsaps.pkt_err++;
			
				OAM_ERROR("Psess or backup egress lsp is not exist!\n");
				return ERRNO_FAIL;
			}
		}
		
		OAM_DEBUG("APS frame received not on protection path.ppkt->in_ifindex=%d;psess->tunnel_pif->p_mplstp->backup_egress_lsp->lsp_index=%d.\n",
			ppkt->in_ifindex,psess->tunnel_pif->p_mplstp->backup_egress_lsp->lsp_index);
		
		if(psess->tunnel_pif->p_mplstp->backup_egress_lsp->lsp_index != ppkt->in_ifindex)
		{
			gmplsaps.pkt_err++;

			OAM_ERROR("APS frame received not on protection path\n");
			return ERRNO_FAIL;
		}
	}
	memset (&pkt_rcv, 0, sizeof(struct mplsaps_pkt));
	memcpy(&pkt_rcv, ppkt->data, sizeof(struct mplsaps_pkt));
	if(gmplsaps.debug_packet)
	{
		printf("Recv APS Packet : %s\n", pkt_dump(&pkt_rcv,sizeof(struct mplsaps_pkt)));
	}
	
	/* ABD -- 010x/001x/011x are  incorrect */
    	if ( ((!pkt_rcv.port_type_A)&&( pkt_rcv.port_type_B)&&(!pkt_rcv.port_type_D)) || 
         		((!pkt_rcv.port_type_A) &&( ! pkt_rcv.port_type_B)&& (pkt_rcv.port_type_D)) ||
         		((!pkt_rcv.port_type_A )&& (pkt_rcv.port_type_B)&& (pkt_rcv.port_type_D )) )
	{
		gmplsaps.pkt_err++;

		OAM_ERROR("validate elps aps pdu, Invalid ABDR in PDU\n");
		return ERRNO_FAIL;
	}
	psess->info.current_event = pkt_rcv.request_state;
	
	OAM_DEBUG("psess->info.current_event %d.\n", psess->info.current_event);	
	
	ret = mplsaps_fsm(psess,&pkt_rcv,psess->info.current_event);
	if(ret)
	{
		OAM_ERROR("mplsaps_fsm return error %d.\n", ret);
		return ret;
	}
	return ERRNO_SUCCESS;
	
}

void mplsaps_pkt_register(void)
{
	union proto_reg proto;
	
	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_APS;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	

	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode = CFM_OPCODE_APS;
	//proto.mplsreg.ttl = 1;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);	

	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_7FFA;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_APS;
	proto.mplsreg.inlabel_value = 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	memset(&proto, 0, sizeof(union proto_reg));
	proto.mplsreg.chtype	    = MPLS_CTLWORD_CHTYPE_8902;
	proto.mplsreg.if_type	    = PKT_INIF_TYPE_PW;
	proto.mplsreg.oam_opcode    = CFM_OPCODE_APS;
	proto.mplsreg.inlabel_value = 13;
	pkt_register(MODULE_ID_MPLS, PROTO_TYPE_MPLS, &proto);

	
	return ;
}

