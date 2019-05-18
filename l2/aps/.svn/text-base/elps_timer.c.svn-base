#include <lib/types.h>
#include <lib/thread.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/msg_ipc_n.h>

#include "aps/elps.h"
#include "aps/elps_cmd.h"
#include "aps/elps_fsm.h"
#include "aps/elps_timer.h"
#include "aps/elps_pkt.h"
#include "l2_msg.h"


extern struct thread_master *l2_master;
int elps_clear_timer(struct elps_sess *psess)
{
	int ret = 0;

	if(NULL == psess)
	{
		return ret;
	}
	
	if(0 != psess->pkeepalive_timer)
	{
		high_pre_timer_delete(psess->pkeepalive_timer);
		psess->pkeepalive_timer = 0;
	}

	if(0 != psess->pholdoff_timer)
	{
		high_pre_timer_delete(psess->pholdoff_timer);
		psess->pholdoff_timer = 0;
	}

	if(0 != psess->pwtr_timer)
	{
		high_pre_timer_delete(psess->pwtr_timer);
		psess->pwtr_timer = 0;
	}

	if(0 != psess->dfop_timer)
	{
		high_pre_timer_delete(psess->dfop_timer);
		psess->dfop_timer = 0;
	}

	return ret;
}

/* wtr */
int elps_wtr_expiry (void *arg)
{
	s_int32_t ret = 0;
	struct elps_sess *psess = NULL;

	psess = (struct elps_sess*)arg;
	psess->pwtr_timer= 0;
	psess->info.current_event = LOCAL_ELPS_WTR_EXPIRES;
	ret = elps_fsm(psess, NULL,psess->info.current_event);
	if(ret)
	{
		ELPS_LOG_DBG("[Func:%s]:elps_fsm return error %d.--Line:%d", __FUNCTION__, ret ,__LINE__);
	}
	return ret;
}

void elps_start_wtr_timer (struct elps_sess *psess)
{
	uint16_t wtr;

	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->pwtr_timer)
	{
		high_pre_timer_delete(psess->pwtr_timer);
		psess->pwtr_timer  = 0;
	}
	wtr = psess->info.wtr *60;
	psess->pwtr_timer = high_pre_timer_add("ElpsWTRTimer",LIB_TIMER_TYPE_NOLOOP,
								elps_wtr_expiry,(void *)psess,wtr*1000);
}


void elps_stop_wtr_timer (struct elps_sess *psess)
{

	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->pwtr_timer)
	{
		high_pre_timer_delete(psess->pwtr_timer);
		psess->pwtr_timer  = 0;
	}
}

/* holdoff */
int elps_holdoff_expiry (void *arg)
{
	int ret = ERRNO_FAIL;
    struct elps_sess *psess = NULL;

	psess = (struct elps_sess*)arg;
	psess->pholdoff_timer = 0;
	ret = elps_link_down_handler(psess);	
    if(ret == ERRNO_SUCCESS)
	{
		ret = l2_msg_send_hal_wait_ack(psess, sizeof(struct elps_sess), 1, MODULE_ID_HAL, MODULE_ID_L2,
                                   IPC_TYPE_ELPS, OAM_SUBTYPE_ELPS_SESSION, IPC_OPCODE_UPDATE, psess->info.sess_id);
	    if(ret != ERRNO_SUCCESS)
	    {
	        psess->info.status = APS_STATUS_DISABLE;
	        ELPS_LOG_DBG("%-15s[Func:%s]:Send to hal.--Line:%d", __FILE__, __FUNCTION__, __LINE__);
	        return ERRNO_IPC;
	    }
	}

	return ERRNO_SUCCESS;
}

void elps_start_holdoff_timer  (struct elps_sess *psess)
{

	if(psess == NULL)
	{
		return ;
	}

	if(0 != psess->pholdoff_timer)
	{
		high_pre_timer_delete(psess->pholdoff_timer);
	}
	psess->pholdoff_timer = high_pre_timer_add("ElpsHoldOFFTimer",LIB_TIMER_TYPE_NOLOOP,
							elps_holdoff_expiry,(void *)psess,psess->info.holdoff*1000);
}

/* Message interval timer */
int elps_msg_expiry (void *arg)
{
	uint32_t ret = 1;
	struct elps_sess *psess = NULL;

	psess = (struct elps_sess*)arg;
	if(psess->info.status == APS_STATUS_ENABLE )
	{
		ret = aps_send(psess);
	}
	return ret;
}
void elps_start_msg_timer (struct elps_sess *psess)
{

	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->pkeepalive_timer)
	{
		high_pre_timer_delete(psess->pkeepalive_timer);
		psess->pkeepalive_timer = 0;
	}
	psess->pkeepalive_timer = high_pre_timer_add("ElpsKeepAliveTimer",LIB_TIMER_TYPE_LOOP,
								elps_msg_expiry,(void *)psess,psess->info.keepalive*1000);
}

void elps_stop_msg_timer (struct elps_sess *psess)
{
	
	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->pkeepalive_timer)
	{
		high_pre_timer_delete(psess->pkeepalive_timer);
		psess->pkeepalive_timer = 0;
	}
}


//* DFOP no message timer */
int elps_dfop_expiry (void *arg)
{
	uint32_t ret = 1;
	struct elps_sess *psess = NULL;

	psess = (struct elps_sess*)arg;
	psess->dfop_timer= 0;
	
	if(psess->info.current_status == ELPS_SIGNAL_FAIL_FOR_PROTECTION)
	{
		psess->info.dfop_alarm = 0;
	}
	else
	{
		psess->info.dfop_alarm = ELPS_NO_APS_RECV;
	}
	return ret;
}
void elps_start_dfop_timer (struct elps_sess *psess)
{
	uint16_t time_val;
	
	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->dfop_timer)
	{
		high_pre_timer_delete(psess->dfop_timer);
		psess->dfop_timer = 0;
	}
	time_val = (psess->info.keepalive)*4;
	psess->dfop_timer = high_pre_timer_add("ElpsDfopTimer",LIB_TIMER_TYPE_NOLOOP,
								elps_dfop_expiry,(void *)psess,time_val*1000);
}

void elps_stop_dfop_timer (struct elps_sess *psess)
{

	if(psess == NULL)
	{
		return;
	}

	if(0 != psess->dfop_timer)
	{
		high_pre_timer_delete(psess->dfop_timer);
		psess->dfop_timer = 0;
	}
}
