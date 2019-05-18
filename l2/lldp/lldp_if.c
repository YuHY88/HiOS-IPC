/*
*  Copyright (C) 2016  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: lldp_init.h	
*
*  date: 2016.8.2
*
*  modify:	2018.3.12 modified by liufuying to make LLDP module code beautiful
*
*/

#include "inet_ip.h"
#include "command.h"
#include "l2/l2_if.h"
#include "lib/hash1.h"
#include "lib/msg_ipc.h"
#include "lib/thread.h"
#include "lib/log.h"


#include "lldp.h"
#include "lldp_tx_sm.h"
#include "lldp_if.h"
#include "lldp_init.h"
#include "lldp_msg.h"
#include "lldp_rx_sm.h"
#include "lldp_cli.h"
#include <lib/hptimer.h>

extern lldp_global			lldp_global_msg;
extern struct thread_master *l2_master;
extern struct hash_table 	l2if_table;


void lldp_init(void)
{
	/*init lldp lldp_global_msg*/
	lldp_g_cfg_init();
	lldp_vty_init();

	lldp_pkt_register();
	//lldp_timer_start(NULL);
    high_pre_timer_add("LldpTimer", LIB_TIMER_TYPE_LOOP, lldp_timer_start, NULL, 1000);

	return;
}


//int lldp_timer_start(struct thread *thread)
int lldp_timer_start(void *para)
{
	struct lldp_port		*lldp_port = NULL;
	struct hash_bucket		*pbucket = NULL;
	int						cursor = 0;
	struct l2if				*pif = NULL;
	
	HASH_BUCKET_LOOP( pbucket, cursor, l2if_table )
	{
		pif = pbucket->data;
		if(pif != NULL)
		{
			lldp_port = pif->lldp_port_info;
			
			if(lldp_port != NULL)
			{
				lldp_tx_timers(lldp_port);
				lldp_rx_timers(lldp_port);
			}
		}
	}
	
	//thread_add_timer (l2_master, lldp_timer_start, NULL, lldp_global_msg.timer);

	return 0;
}


