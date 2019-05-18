#include <lib/types.h>
#include <lib/thread.h>
#include <lib/errcode.h>
#include <lib/log.h>


#include "raps/erps.h"
#include "raps/erps_pkt.h"
#include "raps/erps_timer.h"
#include "raps/erps_fsm.h"
#include "raps/erps_fsm_action.h"




extern struct thread_master *l2_master;

int erps_clear_timer(struct erps_sess *psess)
{
	int ret = 0;

	if(NULL == psess)
	{
		return ret;
	}

	if(0 != psess->holdoff_timer)
	{
		high_pre_timer_delete(psess->holdoff_timer);
		psess->holdoff_timer = 0;
	}

	if(0 != psess->wtr_timer)
	{
		high_pre_timer_delete(psess->wtr_timer);
		psess->wtr_timer = 0;
	}

	if(0 != psess->wtb_timer)
	{
		high_pre_timer_delete(psess->wtb_timer);
		psess->wtb_timer = 0;
	}

	if(0 != psess->guard_timer)
	{
		high_pre_timer_delete(psess->guard_timer);
		psess->guard_timer = 0;
	}

	if(0 != psess->keepalive_timer)
	{
		high_pre_timer_delete(psess->keepalive_timer);
		psess->keepalive_timer = 0;
	}

	return ret;
}
/* wtr */
int erps_wtr_expiry(void *arg)
{
    s_int32_t ret = 0;
    struct erps_sess *psess = NULL;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
    psess = (struct erps_sess *)arg;
    psess->wtr_timer = 0;
    psess->info.current_event = ERPS_EVENT_WTR_EXP;
    ret = erps_fsm(psess, NULL);

    if(ret)
    {
        ERPS_LOG_DBG("[Func:%s]:erps_fsm return error %d.--Line:%d", __FUNCTION__, ret , __LINE__);
    }

    return ret;
}

void erps_start_wtr_timer(struct erps_sess *psess)
{
    uint16_t wtr = 0;

    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

	if(0 != psess->wtr_timer )
	{
		high_pre_timer_delete(psess->wtr_timer);
    	psess->wtr_timer  = 0;
	}
    wtr = psess->info.wtr * 60;

	psess->wtr_timer = high_pre_timer_add("ErpsWTRTimer",LIB_TIMER_TYPE_NOLOOP,
								erps_wtr_expiry,(void *)psess,wtr*1000);
}


void erps_stop_wtr_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
	if(0 != psess->wtr_timer )
	{
		high_pre_timer_delete(psess->wtr_timer);
    	psess->wtr_timer  = 0;
	}
}
/* wtb */
int erps_wtb_expiry(void *arg)
{
    s_int32_t ret = 0;
    struct erps_sess *psess = NULL;


    psess = (struct erps_sess *)arg;

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
    psess->wtb_timer = 0;
    psess->info.current_event = ERPS_EVENT_WTB_EXP;
    ret = erps_fsm(psess, NULL);

    if(ret)
    {
        ERPS_LOG_DBG("[Func:%s]:erps_fsm return error %d.--Line:%d", __FUNCTION__, ret , __LINE__);
    }

    return ret;
}

void erps_start_wtb_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
	if(0 != psess->wtb_timer)
	{
		high_pre_timer_delete(psess->wtb_timer);
   	    psess->wtb_timer = 0;
	}
	psess->wtb_timer = high_pre_timer_add("ErpsWTBTimer",LIB_TIMER_TYPE_NOLOOP,
								erps_wtb_expiry,(void *)psess,psess->info.wtb);
}


void erps_stop_wtb_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

	if(0 != psess->wtb_timer)
	{
		high_pre_timer_delete(psess->wtb_timer);
   	    psess->wtb_timer = 0;
	}
}
/* guardtimer */
int erps_guard_expiry(void *arg)
{
    s_int32_t ret = 0;
    struct erps_sess *psess = NULL;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    psess = (struct erps_sess *)arg;
    psess->guard_timer = 0;

    return ret;
}

void erps_start_guard_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

	if(0 != psess->guard_timer)
	{
		high_pre_timer_delete(psess->guard_timer);
 	    psess->guard_timer = 0;
	}
    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);
	psess->guard_timer = high_pre_timer_add("ErpsGUARDTimer",LIB_TIMER_TYPE_NOLOOP,
								erps_guard_expiry,(void *)psess,psess->info.guardtimer);
}


void erps_stop_guard_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d sess id %d", __FILE__, __func__, __LINE__, psess->info.sess_id);

	if(0 != psess->guard_timer)
	{
		high_pre_timer_delete(psess->guard_timer);
 	    psess->guard_timer = 0;
	}
}

/* holdoff */
int erps_holdoff_expiry(void *arg)
{
    uint32_t ret = 0;
    struct erps_sess *psess = NULL;

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    psess = (struct erps_sess *)arg;
    psess->holdoff_timer = 0;
    erps_link_down_handler(psess);


    return ret;
}

void erps_start_holdoff_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return ;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
	if(0 != psess->holdoff_timer)
	{
		high_pre_timer_delete(psess->holdoff_timer);
 	    psess->holdoff_timer = 0;
	}
	psess->holdoff_timer = high_pre_timer_add("ErpsHoldOFFTimer",LIB_TIMER_TYPE_NOLOOP,
								erps_holdoff_expiry,(void *)psess,psess->info.holdoff);
}

/* Message interval timer */
int erps_msg_expiry(void *arg)
{
    uint32_t ret = 1;
    struct erps_sess *psess = NULL;
	
    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

    psess = (struct erps_sess *)arg;

    if(psess->info.status == SESSION_STATUS_ENABLE)
    {
    	if(psess->info.block_interface != 0 &&
		psess->info.block_interface == psess->info.east_interface)
		{
			psess->r_aps.bpr = 1;
		}
		
        ret = raps_send(psess);
    }

    return ret;
}
void erps_start_msg_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
	if(0 != psess->keepalive_timer)
	{
		high_pre_timer_delete(psess->keepalive_timer);
		psess->keepalive_timer = 0;
	}
    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
	psess->keepalive_timer = high_pre_timer_add("ErpsKeepAliveTimer",LIB_TIMER_TYPE_LOOP,
								erps_msg_expiry,(void *)psess,psess->info.keepalive*1000);
}

void erps_stop_msg_timer(struct erps_sess *psess)
{
    if(psess == NULL)
    {
        ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);
        return;
    }

    ERPS_LOG_DBG("%s: '%s'--the line of %d", __FILE__, __func__, __LINE__);

	if(0 != psess->keepalive_timer)
	{
		high_pre_timer_delete(psess->keepalive_timer);
		psess->keepalive_timer = 0;
	}
}


