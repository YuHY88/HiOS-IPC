
/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_timer.h
*
*  date: 2017.3.28~29
*
*  modify:2017.10~2017.12
*
*  modify:	2018.3.13 modified by liufuying to make mstp module code beautiful
*/
#include "lib/log.h"
#include "lib/thread.h"

#include "mstp.h"
#include "mstp_port.h"
#include "mstp_timer.h"
#include "mstp_sm.h"


extern struct mstp_bridge			*mstp_global_bridge;
extern struct thread_master 		*l2_master;

int  mstp_timer_start(void * num)
{
	struct mstp_port		*mstp_port = NULL, *mstp_port_next = NULL;
	static int 				mstp_ticks = 0;

	if(NULL == mstp_global_bridge)
	{
		return 0 ;
	}

	/*maybe need to deal one time in 100ms*/
	/*now:	one 100ms do one time*/
	/*modify by youcheng 2018/8/16*/
	mstp_state_machine_normal_action(mstp_global_bridge);

	/*change Boolean variable controlled by the system initialization process to
	**every  initialization process control by themselves*/

	/*per second do one time*/
	mstp_ticks++;

	/*(MSTP_TIMER_EXPIRE_COUNT_PER_SECOND*mstp_global_bridge->mstp_tick_timer) == 1S
	if change please change it*/
	if(mstp_ticks == MSTP_TIMER_EXPIRE_COUNT_PER_SECOND) /*5*100ms = 0.5s*/
	{
		mstp_ticks = 0;
		list_for_each_entry_safe(mstp_port, mstp_port_next, &mstp_global_bridge->port_head, port_list)
    	{
			mstp_port->port_variables.tick = MSTP_TRUE;
			mstp_state_machine_normal_action(mstp_global_bridge);
    	}
		
	}

	//thread_add_timer_msec(l2_master, mstp_timer_start, NULL, mstp_global_bridge->mstp_tick_timer);

	MSTP_TIMER_MSEC_ON_LOOP(mstp_global_bridge->mstp_tick_time,mstp_timer_start,NULL,mstp_global_bridge->mstp_tick_timer);
	return 0;
}


