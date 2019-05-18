/******************************************************************************
 * Filename: gpnAlmMasterSlaveSync.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2018.8.9  lipf created
 *
******************************************************************************/

#ifndef _GPN_ALM_MASTER_SLAVE_SYNC_H_
#define _GPN_ALM_MASTER_SLAVE_SYNC_H_

#include <stdio.h>
#include <time.h>

#include "lib/thread.h"
#include "lib/devm_com.h"
#include "lib/msg_ipc_n.h"
#include "lib/linklist.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"

#include "gpnAlmScan.h"
#include "gpnAlmTimeProc.h"


#define GPN_ALM_BOARD_INVALID			0		//盒式设备，无主备
#define GPN_ALM_BOARD_MASTER			1		//框式设备的主盘
#define GPN_ALM_BOARD_SLAVE				2		//框式设备的备盘

#define GPN_ALM_SLAVE_POWER_INVALID		0		//备盘上电初始值
#define GPN_ALM_SLAVE_POWER_NEW			1		//备盘新上电状态
#define GPN_ALM_SLAVE_POWER_DONE		2		//备盘上电完成，即常规状态

#define GPN_ALM_SYNC_INVALID			0		//告警主备同步初始值
#define GPN_ALM_SYNC_RUNNING			1		//告警主备同步正在进行
#define GPN_ALM_SYNC_DONE				2		//告警主备同步完成


#define GPN_ALM_SYNC_CACHE_TIME			3		//单位second，告警项缓存时间，之后进行同步

#define GPN_ALM_SYNC_DELAY				GPN_ALM_TIM_CLC_SPAN_LIMIT+5	//单位second，备盘刚上电时的告警同步延时

//ipc subtype
#define GPN_ALM_MSG_CURR_ALARM_SYNC		1		//备盘刚上电时当前告警同步消息
#define GPN_ALM_MSG_HIST_ALARM_SYNC		2		//备盘刚上电时历史告警同步消息
#define GPN_ALM_MSG_NEW_CURR_ALARM_SYNC	3		//备盘上电后新告警同步消息
#define GPN_ALM_MSG_NEW_HIST_ALARM_SYNC	4		//备盘上电后新告警同步消息
#define GPN_ALM_MSG_EVENT_SYNC			5		//事件同步消息



struct alarm_list
{
	struct list *pList;

	time_t	added_time;	//本链表中第一项数据的添加时间
};


struct alarm_device_info
{
	uint8_t	devType;
	uint8_t slot_main_board;	       /* mainboard slot */
	uint8_t slot_slave_board;          /* slaveboard slot */	
	uint8_t myslot;                    /* my board slot num */
	enum DEVM_HA_E ms_status;          /* the ha role of myslot is main or slave*/
};


struct alarm_sync_info
{
	objLogicDesc 	viewPort;
	stAlmValueRecd 	almValueRecd;

	int action;	//告警动作，GPN_SOCK_MSG_OPT_RISE/GPN_SOCK_MSG_OPT_CLEAN
};



void alarm_MS_sync_init(void);

enum DEVM_HA_E alarm_MS_my_status_get(void);
uint32_t alarm_MS_slave_board_slot_get(void);
uint32_t alarm_MS_slave_power_done(void);

int alarm_MS_my_status_check(struct thread *t);
int alarm_MS_data_sync_process(struct thread *t);

void alarm_handle_devm_msg(struct ipc_mesg_n *pmsg);
void alarm_handle_ha_msg(struct ipc_mesg_n *pmsg);

int alarm_copy_table_to_sync_table(int table_src);

int alarm_curr_alarm_table_sync(void);
int alarm_hist_alarm_table_sync(void);
int alarm_event_table_sync(void);

int alarm_new_curr_alarm_table_sync(void);
int alarm_new_hist_alarm_table_sync(void);
int alarm_new_event_table_sync(void);

int alarm_new_curr_alarm_sync_table_add(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd, int action);
int alarm_new_hist_alarm_sync_table_add(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd);
int alarm_new_event_sync_table_add(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd);

int alarm_new_curr_alarm_sync_table_del_all(void);
int alarm_new_hist_alarm_sync_table_del_all(void);
int alarm_new_event_sync_table_del_all(void);

int alarm_slave_store_master_curr_alarm(uint8_t *pdata, uint8_t data_num);
int alarm_slave_store_master_hist_alarm(uint8_t *pdata, uint8_t data_num);
int alarm_slave_store_master_new_curr_alarm(uint8_t *pdata, uint8_t data_num);
int alarm_slave_store_master_new_hist_alarm(uint8_t *pdata, uint8_t data_num);
int alarm_slave_store_master_event(uint8_t *pdata, uint8_t data_num);



#endif /* _GPN_ALM_MASTER_SLAVE_SYNC_H_ */

