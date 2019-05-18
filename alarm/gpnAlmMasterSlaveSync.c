/******************************************************************************
 * Filename: gpnAlmMasterSlaveSync.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2018.8.9  lipf created
 *
******************************************************************************/

#include <unistd.h>
#include <string.h>
#include <time.h>

#include "lib/devm_com.h"
#include "lib/linklist.h"
#include "lib/module_id.h"
#include "lib/msg_ipc.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include <lib/syslog.h>
#include <lib/log.h>

#include "gpnAlmMasterSlaveSync.h"
#include "gpnAlmDataSheet.h"
#include "gpnAlmDataStructApi.h"
#include "gpnAlmSnmpApi.h"
#include "gpnAlmAlmNotify.h"
#include "gpnAlmDebug.h"

static char ipc_buf[IPC_MSG_LEN1];

static struct alarm_device_info gAlmDeviceInfo;		//记录当前设备信息

//static int	gAlmSyncStatus;					//主备告警同步状态，初始/正在同步/同步完成
static int	gAlmSlaveStatus;				//备盘状态，初始/刚上电/常规三种状态

static int gPowerOnSyncDelay;				//备盘刚上电时，同步延时

//备盘刚上电时待同步的当前告警表
static struct list *pCurrAlmSyncTable = NULL;

//备盘刚上电时待同步的历史告警表
static struct list *pHistAlmSyncTable = NULL;

//备盘刚上电时又上报的告警或常规时间上报的告警，只需同步告警新动作(产生/消除)
static struct alarm_list newCurrAlmSyncTable;

//备盘刚上电时又上报的告警或常规时间上报的告警，只需同步告警新动作(产生/消除)
static struct alarm_list newHistAlmSyncTable;

//备盘刚上电时待同步的事件
static struct list *pEvtSyncTable	 = NULL;

//备盘刚上电时又上报的事件或常规时间上报的事件
static struct alarm_list newEvtSyncTable;


extern struct thread_master *alarm_master;


/* 主备同步初始化 */
void alarm_MS_sync_init(void)
{
	memset(&gAlmDeviceInfo, 0, sizeof(struct alarm_device_info));
	gAlmDeviceInfo.devType   = DEV_TYPE_INVALID;
	gAlmDeviceInfo.ms_status = DEVM_HA_INVALID;
	
	//gAlmSyncStatus 	= GPN_ALM_SYNC_INVALID;
	gAlmSlaveStatus = GPN_ALM_SLAVE_POWER_INVALID;

	pCurrAlmSyncTable	= list_new();
	pHistAlmSyncTable	= list_new();
	pEvtSyncTable		= list_new();

	newCurrAlmSyncTable.added_time = 0;
	newHistAlmSyncTable.added_time = 0;
	newEvtSyncTable.added_time = 0;
	
	newCurrAlmSyncTable.pList = list_new();
	newHistAlmSyncTable.pList = list_new();
	newEvtSyncTable.pList = list_new();

	devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_ALARM, 0);
	//devm_event_register(DEV_EVENT_SLOT_ADD, MODULE_ID_ALARM, 0);
	devm_event_register(DEV_EVENT_SLOT_DELETE, MODULE_ID_ALARM, 0);

	//thread_add_timer(alarm_master, alarm_MS_my_status_check, NULL, 30);
	//thread_add_timer(alarm_master, alarm_MS_data_sync_process, NULL, 1);
	high_pre_timer_add((char *)"GpnAlmSyncMyStatusCheckTimer",    LIB_TIMER_TYPE_LOOP, alarm_MS_my_status_check, NULL, 30000);
	high_pre_timer_add((char *)"GpnAlmSyncProcessTimer",    LIB_TIMER_TYPE_LOOP, alarm_MS_data_sync_process, NULL, 1000);
}


static int ipc_send_ha(void *pdata, int data_len, int data_num, int module_id,
			enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	ipc_send_msg_ha_n2(pdata, data_len, data_num, module_id,
		module_id, msg_type, subtype, opcode, 0, 0);

	return 1;
}




/* 定时检查本盘的主备状态 */
int alarm_MS_my_status_check(struct thread *t)
{
	struct devm_unit para_devm_unit;
	int uint = 1;
	int ret = 0;

	enum DEV_TYPE devType = DEV_TYPE_INVALID;

	if(devm_comm_get_dev_type(MODULE_ID_ALARM, &devType) != 0)
    {
        zlog_err("%s[%d]: filem get device type error, device type is %d\n",__FILE__,__LINE__, devType);
		return 0;
    }	

    switch(devType)
    {
        case DEV_TYPE_BOX:  /* 盒式设备，例如 181 系列 */

			gAlmDeviceInfo.devType = DEV_TYPE_BOX;		//盒式设备无主备
			break;

        case DEV_TYPE_SHELF:/* 单框设备，例如 2000 系列有背板的设备 */  

			gAlmDeviceInfo.devType = DEV_TYPE_SHELF;
			ret = devm_comm_get_unit(uint, MODULE_ID_ALARM, &para_devm_unit);

			if(0 != ret)
			{
				return 0;
			}
			else
			{
			 	if(para_devm_unit.slot_main_board == para_devm_unit.myslot)
			 	{
					para_devm_unit.slot_ha = DEVM_HA_MASTER;
				}
				else
				{
					para_devm_unit.slot_ha = DEVM_HA_SLAVE;
				}
			}

			if(gAlmDeviceInfo.ms_status != para_devm_unit.slot_ha)
			{
			 	gAlmDeviceInfo.ms_status = para_devm_unit.slot_ha;
			}
			
			gAlmDeviceInfo.slot_main_board 	= para_devm_unit.slot_main_board;			
			gAlmDeviceInfo.myslot 			= para_devm_unit.myslot;

			if(GPN_ALM_SLAVE_POWER_DONE == gAlmSlaveStatus)
			{				
				gAlmDeviceInfo.slot_slave_board = para_devm_unit.slot_slave_board;
			}
			 
            break;

        case DEV_TYPE_STACK: /* 多框堆叠设备，暂时没有 */
            ret = 1;
            break;
             
        default: break;
    }

	/*zlog_debug(ALARM_DBG_SYNC, "%s[%d] : myslot(%d), mainboard(%d), slaveboard(%d)\n", __func__, __LINE__,
		gAlmDeviceInfo.myslot, gAlmDeviceInfo.slot_main_board, gAlmDeviceInfo.slot_slave_board);*/

	//thread_add_timer(alarm_master, alarm_MS_my_status_check, NULL, 30);
	return 1;
}


/* 获取本盘主备状态 */
enum DEVM_HA_E alarm_MS_my_status_get(void)
{
	return gAlmDeviceInfo.ms_status;
}


uint32_t alarm_MS_slave_board_slot_get(void)
{
	return gAlmDeviceInfo.slot_slave_board;
}


uint32_t alarm_MS_slave_power_done(void)
{
	if(alarm_MS_slave_board_slot_get() && (GPN_ALM_SLAVE_POWER_DONE == gAlmSlaveStatus))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//主备数据同步处理接口，定时处理
int alarm_MS_data_sync_process(struct thread *t)
{
	if(DEVM_HA_MASTER == alarm_MS_my_status_get())	//本盘为主盘
	{
		switch(gAlmSlaveStatus)
		{
			/* 备盘不存在，不进行任何同步 */
			case GPN_ALM_SLAVE_POWER_INVALID:
				alarm_new_curr_alarm_sync_table_del_all();
				alarm_new_hist_alarm_sync_table_del_all();
				alarm_new_event_sync_table_del_all();
				break;

			/* 备盘新上电，将已存在的当前告警、历史告警表都拷贝到 
			   pCurrAlmSyncTable/pHistAlmSyncTable 中，
			   开始拷贝后将 gAlmSyncStatus 修改为正在同步标志，此过程中
			   上报的告警都存放在 newCurrAlmSyncTable          中，前两个表同步完成后，再同步该表，
			   事件同步类似 */
			case GPN_ALM_SLAVE_POWER_NEW:
				gPowerOnSyncDelay--;				
				if(1 == gPowerOnSyncDelay)
				{
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : slave power new, start to sync alarm ...\n", __func__, __LINE__);
					alarm_curr_alarm_table_sync();
					alarm_hist_alarm_table_sync();
					alarm_event_table_sync();
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : slave power done........................\n", __func__, __LINE__);
					gAlmSlaveStatus = GPN_ALM_SLAVE_POWER_DONE;
				}
				break;

			/* 备盘已上电，每出现一个告警，开始缓存，缓存时间到后，进行同步 */
			case GPN_ALM_SLAVE_POWER_DONE:
				//zlog_debug((ALARM_DBG_SYNC, "%s[%d] : slave power done, start to sync alarm ...\n", __func__, __LINE__);
				alarm_new_curr_alarm_table_sync();
				alarm_new_hist_alarm_table_sync();
				alarm_new_event_table_sync();
				break;			

			default:
				break;
		}
	}

	//thread_add_timer(alarm_master, alarm_MS_data_sync_process, NULL, 1);
	return 1;
}



//处理devm的IPC消息
void alarm_handle_devm_msg(struct ipc_mesg_n *pmsg)
{
	struct devm_com *pDevCom = NULL;
	
	if(IPC_OPCODE_EVENT == pmsg->msghdr.opcode)
	{
		pDevCom = (struct devm_com *)(pmsg->msg_data);
		
		if(DEV_EVENT_HA_BOOTSTATE_CHANGE == pmsg->msghdr.msg_subtype)
		{					
			if(gAlmDeviceInfo.myslot == pDevCom->slot)	//本盘主备状态变化
			{
				if((HA_ROLE_MASTER == pDevCom->ha_role) && 
					(DEVM_HA_SLAVE == gAlmDeviceInfo.ms_status))
				{
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : alarm ms master changed from slave to master\n", __func__, __LINE__);
					gAlmDeviceInfo.ms_status = DEVM_HA_MASTER;
					
				}
				else if((HA_ROLE_SLAVE == pDevCom->ha_role) &&
						(DEVM_HA_MASTER == gAlmDeviceInfo.ms_status))
				{
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : alarm ms master changed from master to slave\n", __func__, __LINE__);
					gAlmDeviceInfo.ms_status = DEVM_HA_SLAVE;
				}
				else
				{
					return;
				}

				gAlmDeviceInfo.slot_main_board = pDevCom->main_slot;
				gAlmDeviceInfo.slot_slave_board = pDevCom->slave_slot;
			}
			else
			{
				if(0 == gAlmDeviceInfo.myslot)
				{					
					gAlmDeviceInfo.myslot = pDevCom->slot;
					gAlmDeviceInfo.slot_main_board = pDevCom->main_slot;
					gAlmDeviceInfo.slot_slave_board = pDevCom->slave_slot;

					if(gAlmDeviceInfo.myslot == gAlmDeviceInfo.slot_main_board)
					{
						gAlmDeviceInfo.ms_status = DEVM_HA_MASTER;
						
						if(gAlmDeviceInfo.slot_slave_board)
						{
							gAlmSlaveStatus = GPN_ALM_SLAVE_POWER_NEW;
							gPowerOnSyncDelay = GPN_ALM_SYNC_DELAY;
						}
					}
					else
					{
						gAlmDeviceInfo.ms_status = DEVM_HA_SLAVE;
					}
					zlog_debug(ALARM_DBG_SYNC, "[%s]%d : my slot power on, (%d:%d) (%d:%d)\n", __func__, __LINE__, 
						gAlmDeviceInfo.myslot, gAlmDeviceInfo.ms_status,
						gAlmDeviceInfo.slot_main_board, gAlmDeviceInfo.slot_slave_board);					
				}
				else 
				{
					if(gAlmDeviceInfo.slot_slave_board == pDevCom->slot)
					{
						return;
					}
					
					gAlmDeviceInfo.slot_main_board = pDevCom->main_slot;
					gAlmDeviceInfo.slot_slave_board = pDevCom->slave_slot;

					if(gAlmDeviceInfo.myslot == gAlmDeviceInfo.slot_main_board)
					{
						gAlmDeviceInfo.ms_status = DEVM_HA_MASTER;
						gAlmSlaveStatus = GPN_ALM_SLAVE_POWER_NEW;
						gPowerOnSyncDelay = GPN_ALM_SYNC_DELAY;
					}
					else
					{
						gAlmDeviceInfo.ms_status = DEVM_HA_SLAVE;
					}					
				}
			}
		}
		else if(DEV_EVENT_SLOT_DELETE == pmsg->msghdr.msg_subtype)
		{
			zlog_debug(ALARM_DBG_SYNC, "[%s]%d : board deleted, slot(%d), ms(%d:%d)\n", __func__, __LINE__, pDevCom->slot,
					pDevCom->main_slot, pDevCom->slave_slot);
			if(pDevCom->slot == pDevCom->slave_slot)
			{
				gAlmSlaveStatus = GPN_ALM_SLAVE_POWER_INVALID;
				
				gAlmDeviceInfo.slot_main_board = gAlmDeviceInfo.myslot;
				gAlmDeviceInfo.slot_slave_board = 0;
				gAlmDeviceInfo.ms_status = DEVM_HA_MASTER;
			}
		}
	}
}



//处理ha的IPC消息，即主备同步消息
void alarm_handle_ha_msg(struct ipc_mesg_n *pmsg)
{
	if(IPC_OPCODE_ADD == pmsg->msghdr.opcode)
	{
		if(IPC_TYPE_SYNC_DATA == pmsg->msghdr.msg_type)
		{
			switch(pmsg->msghdr.msg_subtype)
			{
				case GPN_ALM_MSG_CURR_ALARM_SYNC:
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : recv sync data : curr\n", __func__, __LINE__);
					alarm_slave_store_master_curr_alarm(pmsg->msg_data, pmsg->msghdr.data_num);
					break;

				case GPN_ALM_MSG_HIST_ALARM_SYNC:
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : recv sync data : hist\n", __func__, __LINE__);
					alarm_slave_store_master_hist_alarm(pmsg->msg_data, pmsg->msghdr.data_num);
					break;

				case GPN_ALM_MSG_NEW_CURR_ALARM_SYNC:
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : recv sync data : new curr\n", __func__, __LINE__);
					alarm_slave_store_master_new_curr_alarm(pmsg->msg_data, pmsg->msghdr.data_num);
					break;

				case GPN_ALM_MSG_NEW_HIST_ALARM_SYNC:
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : recv sync data : new hist\n", __func__, __LINE__);
					alarm_slave_store_master_new_hist_alarm(pmsg->msg_data, pmsg->msghdr.data_num);
					break;

				case GPN_ALM_MSG_EVENT_SYNC:
					alarm_slave_store_master_event(pmsg->msg_data, pmsg->msghdr.data_num);
					zlog_debug(ALARM_DBG_SYNC, "%s[%d] : recv sync data : event\n", __func__, __LINE__);
					break;

				default:
					break;
			}
		}
	}
}



/* 拷贝需要同步的表到目标表，当前告警表/历史告警表/时间表到对应的同步表 */
int alarm_copy_table_to_sync_table(int table_src)
{
	stDSCurrAlm  currDB_get;
	stDSHistAlm	 histDB_get;
	stDSEvent 	 eventDB_get;

	memset(&currDB_get, 0, sizeof(stDSCurrAlm));
	memset(&histDB_get, 0, sizeof(stDSHistAlm));
	memset(&eventDB_get, 0, sizeof(stDSEvent));
	
	stDSCurrAlm  *pCurrAlmDB = NULL;
	stDSHistAlm  *pHistAlmDB = NULL;
	stDSEvent 	 *pEventDB = NULL;

	uint32_t ret = 0;
	uint32_t index = GPN_ALM_DS_ILLEGAL_INDEX;
	uint32_t index_next = GPN_ALM_DS_ILLEGAL_INDEX;
	
	switch(table_src)
	{
		case GPN_ALM_CURR_ALM_DSHEET:			
			ret = gpnAlmDSApiCurrAlmGetNext(index, &index_next, &currDB_get);
			while(GPN_ALM_SNMP_OK == ret)
			{
				pCurrAlmDB = (stDSCurrAlm *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(stDSCurrAlm));
				if(NULL == pCurrAlmDB)
				{
					zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
					return 0;
				}

				zlog_debug(ALARM_DBG_SYNC, "%s[%d] : copy curr (%08x|%08x|%08x), index(%08x : %08x)\n", __func__, __LINE__,
					currDB_get.index, currDB_get.almType, currDB_get.ifIndex,
					index, index_next);
				
				memcpy(pCurrAlmDB, &currDB_get, sizeof(stDSCurrAlm));
				listnode_add(pCurrAlmSyncTable, pCurrAlmDB);

				index = index_next;
				ret = gpnAlmDSApiCurrAlmGetNext(index, &index_next, &currDB_get);
			}			
			break;

		case GPN_ALM_HIST_ALM_DSHEET:
			ret = gpnAlmDSApiHistAlmGetNext(index, &index_next, &histDB_get);
			while(GPN_ALM_SNMP_OK == ret)
			{
				pHistAlmDB = (stDSHistAlm *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(stDSHistAlm));
				if(NULL == pHistAlmDB)
				{
					zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
					return 0;
				}

				zlog_debug(ALARM_DBG_SYNC, "%s[%d] : copy hist (%08x|%08x|%08x)\n", __func__, __LINE__,
					histDB_get.index, histDB_get.almType, histDB_get.ifIndex);
				
				memcpy(pHistAlmDB, &histDB_get, sizeof(stDSHistAlm));
				listnode_add(pHistAlmSyncTable, pHistAlmDB);

				index = index_next;
				ret = gpnAlmDSApiHistAlmGetNext(index, &index_next, &histDB_get);
			}
			break;

		case GPN_ALM_EVENT_DSHEET:
			ret = gpnAlmDSApiEventGetNext(index, &index_next, &eventDB_get);
			while(GPN_ALM_SNMP_OK == ret)
			{
				pEventDB = (stDSEvent *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(stDSEvent));
				if(NULL == pEventDB)
				{
					zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
					return 0;
				}

				zlog_debug(ALARM_DBG_SYNC, "%s[%d] : copy event (%08x|%08x|%08x)\n", __func__, __LINE__,
					eventDB_get.index, eventDB_get.eventType, eventDB_get.ifIndex);
				
				memcpy(pEventDB, &eventDB_get, sizeof(stDSEvent));
				listnode_add(pEvtSyncTable, pEventDB);

				index = index_next;
				ret = gpnAlmDSApiEventGetNext(index, &index_next, &eventDB_get);
			}
			break;

		default:
			return 0;
	}
	return 1;
}


//增加一个告警项到 newCurrAlmSyncTable 表
int alarm_new_curr_alarm_sync_table_add(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd, int action)
{
	struct alarm_sync_info  *pAlmSyncDB = NULL;
	pAlmSyncDB = (struct alarm_sync_info *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(struct alarm_sync_info));
	if(NULL == pAlmSyncDB)
	{
		zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
		return 0;
	}	
	
	memcpy(&pAlmSyncDB->viewPort, pViewPort, sizeof(objLogicDesc));
	memcpy(&pAlmSyncDB->almValueRecd, pAlmValueRecd, sizeof(stAlmValueRecd));
	pAlmSyncDB->action = action;

	zlog_debug(ALARM_DBG_SYNC, "%s[%d] : new curr alarm add : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
		pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
		pAlmSyncDB->almValueRecd.thisTime);
		
	if(0 == newCurrAlmSyncTable.pList->count)	//该项为第一项，记录时间
	{
		time(&newCurrAlmSyncTable.added_time);
	}

	listnode_add(newCurrAlmSyncTable.pList, pAlmSyncDB);
	
	return 1;
}


//清空 newCurrAlmSyncTable 表
int alarm_new_curr_alarm_sync_table_del_all(void)
{
	struct listnode *pNode  = NULL;
	struct alarm_sync_info  *pAlmSyncDB = NULL;
	
	if(newCurrAlmSyncTable.pList && newCurrAlmSyncTable.pList->count)
	{
		pNode = listhead(newCurrAlmSyncTable.pList);
		while(pNode)
		{
			pAlmSyncDB = (struct alarm_sync_info *)pNode->data;
			if(pAlmSyncDB)
			{
				XFREE(MTYPE_ALARM_ENTRY,pAlmSyncDB);
			}

			list_delete_node(newCurrAlmSyncTable.pList, pNode);

			pNode = listhead(newCurrAlmSyncTable.pList);
		}
	}

	return 1;
}


//增加一个告警项到 newHistAlmSyncTable 表
int alarm_new_hist_alarm_sync_table_add(objLogicDesc *pViewPort, stAlmValueRecd *pAlmValueRecd)
{
	struct alarm_sync_info  *pAlmSyncDB = NULL;
	pAlmSyncDB = (struct alarm_sync_info *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(struct alarm_sync_info));
	if(NULL == pAlmSyncDB)
	{
		zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
		return 0;
	}	
	
	memcpy(&pAlmSyncDB->viewPort, pViewPort, sizeof(objLogicDesc));
	memcpy(&pAlmSyncDB->almValueRecd, pAlmValueRecd, sizeof(stAlmValueRecd));

	zlog_debug(ALARM_DBG_SYNC, "%s[%d]new hist alarm add : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
		pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
		pAlmSyncDB->almValueRecd.thisTime);
		
	if(0 == newHistAlmSyncTable.pList->count)	//该项为第一项，记录时间
	{
		time(&newHistAlmSyncTable.added_time);
	}

	listnode_add(newHistAlmSyncTable.pList, pAlmSyncDB);
	
	return 1;
}


//清空 newHistAlmSyncTable 表
int alarm_new_hist_alarm_sync_table_del_all(void)
{
	struct listnode *pNode  = NULL;
	struct alarm_sync_info  *pAlmSyncDB = NULL;
	
	if(newHistAlmSyncTable.pList && newHistAlmSyncTable.pList->count)
	{
		pNode = listhead(newHistAlmSyncTable.pList);
		while(pNode)
		{
			pAlmSyncDB = (struct alarm_sync_info *)pNode->data;
			if(pAlmSyncDB)
			{
				XFREE(MTYPE_ALARM_ENTRY,pAlmSyncDB);
			}

			list_delete_node(newHistAlmSyncTable.pList, pNode);

			pNode = listhead(newHistAlmSyncTable.pList);
		}
	}

	return 1;
}




//增加一个告警项到 newEvtSyncTable 表
int alarm_new_event_sync_table_add(objLogicDesc *pViewPort, stEvtValueRecd *pEvtValueRecd)
{
	stDSEvent *pEvtSyncDB = NULL;
	pEvtSyncDB = (stDSEvent *)XMALLOC(MTYPE_ALARM_ENTRY, sizeof(stDSEvent));
	if(NULL == pEvtSyncDB)
	{
		zlog_err("%s[%d] : xmalloc error!\n", __func__, __LINE__);
		return 0;
	}

	memset(pEvtSyncDB, 0, sizeof(stDSEvent));
	
	pEvtSyncDB->devIndex = pViewPort->devIndex;
	pEvtSyncDB->ifIndex  = pViewPort->portIndex;
	pEvtSyncDB->ifIndex2 = pViewPort->portIndex3;
	pEvtSyncDB->ifIndex3 = pViewPort->portIndex4;
	pEvtSyncDB->ifIndex4 = pViewPort->portIndex5;
	pEvtSyncDB->ifIndex5 = pViewPort->portIndex6;

	pEvtSyncDB->eventType = pEvtValueRecd->evtSubType;
	pEvtSyncDB->level 	  = pEvtValueRecd->evtRank;
	pEvtSyncDB->thisTime  = pEvtValueRecd->thisTime;
	pEvtSyncDB->detail 	  = pEvtValueRecd->detial;
	
	listnode_add(newEvtSyncTable.pList, pEvtSyncDB);

	if(0 == newEvtSyncTable.pList->count)	//该项为第一项，记录时间
	{
		time(&newEvtSyncTable.added_time);
	}

	return 1;
}


//清空 newEvtSyncTable 表
int alarm_new_event_sync_table_del_all(void)
{
	struct listnode *pNode  = NULL;
	stDSEvent *pEvtSyncDB = NULL;
	
	if(newEvtSyncTable.pList && newEvtSyncTable.pList->count)
	{
		pNode = listhead(newEvtSyncTable.pList);
		while(pNode)
		{
			pEvtSyncDB = (stDSEvent *)pNode->data;
			if(pEvtSyncDB)
			{
				XFREE(MTYPE_ALARM_ENTRY,pEvtSyncDB);
			}

			list_delete_node(newEvtSyncTable.pList, pNode);

			pNode = listhead(newEvtSyncTable.pList);
		}
	}

	return 1;
}



//同步 pCurrAlmSyncTable 表，备盘刚上电时同步 
int alarm_curr_alarm_table_sync(void)
{	
	alarm_copy_table_to_sync_table(GPN_ALM_CURR_ALM_DSHEET);

	struct listnode *pNode  = NULL;
	stDSCurrAlm *pCurrAlmDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSCurrAlm);

	if(0 == pCurrAlmSyncTable->count)
	{
		return 0;
	}
	
	memset(ipc_buf, 0, IPC_MSG_LEN1);
	pNode = listhead(pCurrAlmSyncTable);
	while(pNode)
	{
		pCurrAlmDB = (stDSCurrAlm *)pNode->data;
		if(NULL == pCurrAlmDB)
		{
			return 0;
		}
		
		memcpy(ipc_buf + table_num * sizeof(stDSCurrAlm), pCurrAlmDB, sizeof(stDSCurrAlm));
		table_num++;

		zlog_debug(ALARM_DBG_SYNC, "%s[%d] : curr send : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
			pCurrAlmDB->almType, pCurrAlmDB->ifIndex, pCurrAlmDB->ifIndex2,
			pCurrAlmDB->thisTime);

		if(table_num >= max_table_num)
		{
			ipc_send_ha((void *)ipc_buf, (sizeof(stDSCurrAlm) * table_num), table_num, MODULE_ID_ALARM,
				IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_CURR_ALARM_SYNC, IPC_OPCODE_ADD, 0);
			
			table_num = 0;
		}

		//删除该节点
		XFREE(MTYPE_ALARM_ENTRY, pCurrAlmDB);
		list_delete_node(pCurrAlmSyncTable, pNode);

		pNode = listhead(pCurrAlmSyncTable);
	}

	zlog_debug(ALARM_DBG_SYNC, "%s[%d] : curr table cnt = %d\n", __func__, __LINE__, table_num);

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSCurrAlm) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_CURR_ALARM_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}


//同步 pHistAlmSyncTable 表，备盘刚上电时同步 
int alarm_hist_alarm_table_sync(void)
{
	alarm_copy_table_to_sync_table(GPN_ALM_HIST_ALM_DSHEET);

	struct listnode *pNode  = NULL;
	stDSHistAlm *pHistAlmDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSHistAlm);

	if(0 == pHistAlmSyncTable->count)
	{
		return 0;
	}

	memset(ipc_buf, 0, IPC_MSG_LEN1);

	pNode = listhead(pHistAlmSyncTable);
	while(pNode)
	{
		pHistAlmDB = (stDSHistAlm *)pNode->data;
		if(NULL == pHistAlmDB)
		{
			return 0;
		}
		
		memcpy(ipc_buf + table_num * sizeof(stDSHistAlm), pHistAlmDB, sizeof(stHistAlmDB));
		table_num++;

		if(table_num >= max_table_num)
		{
			ipc_send_ha((void *)ipc_buf, (sizeof(stDSHistAlm) * table_num), table_num, MODULE_ID_ALARM,
				IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_HIST_ALARM_SYNC, IPC_OPCODE_ADD, 0);
			
			table_num = 0;
		}

		//删除该节点
		XFREE(MTYPE_ALARM_ENTRY,pHistAlmDB);
		list_delete_node(pHistAlmSyncTable, pNode);

		pNode = listhead(pHistAlmSyncTable);
	}
	zlog_debug(ALARM_DBG_SYNC, "%s[%d] : hist table cnt = %d\n", __func__, __LINE__, table_num);

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSHistAlm) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_HIST_ALARM_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}


//同步 pEventSyncTable 表，备盘刚上电时同步 
int alarm_event_table_sync(void)
{
	alarm_copy_table_to_sync_table(GPN_ALM_EVENT_DSHEET);

	struct listnode *pNode  = NULL;
	stDSEvent *pEventDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSEvent);

	if(0 == pEvtSyncTable->count)
	{
		return 0;
	}

	memset(ipc_buf, 0, IPC_MSG_LEN1);

	pNode = listhead(pEvtSyncTable);
	while(pNode)
	{
		pEventDB = (stDSEvent *)pNode->data;
		if(NULL == pEventDB)
		{
			return 0;
		}
		
		memcpy(ipc_buf + table_num * sizeof(stDSEvent), pEventDB, sizeof(stEventAlmDB));
		table_num++;

		if(table_num >= max_table_num)
		{
			ipc_send_ha((void *)ipc_buf, (sizeof(stDSEvent) * table_num), table_num, MODULE_ID_ALARM,
				IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_EVENT_SYNC, IPC_OPCODE_ADD, 0);
			
			table_num = 0;
		}

		//删除该节点
		XFREE(MTYPE_ALARM_ENTRY,pEventDB);
		list_delete_node(pEvtSyncTable, pNode);

		pNode = listhead(pEvtSyncTable);
	}
	zlog_debug(ALARM_DBG_SYNC, "%s[%d] : event table cnt = %d\n", __func__, __LINE__, table_num);

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSEvent) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_EVENT_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}




//同步 newCurrAlmSyncTable 表
int alarm_new_curr_alarm_table_sync(void)
{
	time_t time_now = 0;
	struct listnode *pNode  = NULL;
	struct alarm_sync_info  *pAlmSyncDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSCurrAlm);

	if(0 == newCurrAlmSyncTable.pList->count)
	{
		return 0;
	}
	
	time(&time_now);

	memset(ipc_buf, 0, IPC_MSG_LEN1);

	//缓存时间到，且处于常规状态（即上电同步完成）
	if(((time_now - newCurrAlmSyncTable.added_time) > GPN_ALM_SYNC_CACHE_TIME) &&
		(GPN_ALM_SLAVE_POWER_DONE == gAlmSlaveStatus))
	{
		pNode = listhead(newCurrAlmSyncTable.pList);
		while(pNode)
		{
			pAlmSyncDB = (struct alarm_sync_info *)pNode->data;
			if(NULL == pAlmSyncDB)
			{
				return 0;
			}

			zlog_debug(ALARM_DBG_SYNC, "%s[%d] : new alarm send : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
				pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
				pAlmSyncDB->almValueRecd.thisTime);
			
			memcpy(ipc_buf + table_num * sizeof(struct alarm_sync_info), pAlmSyncDB, sizeof(struct alarm_sync_info));
			table_num++;

			if(table_num >= max_table_num)
			{
				ipc_send_ha((void *)ipc_buf, (sizeof(struct alarm_sync_info) * table_num), table_num, MODULE_ID_ALARM,
					IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_NEW_CURR_ALARM_SYNC, IPC_OPCODE_ADD, 0);

				table_num = 0;
			}

			//删除该节点
			XFREE(MTYPE_ALARM_ENTRY,pAlmSyncDB);
			list_delete_node(newCurrAlmSyncTable.pList, pNode);

			pNode = listhead(newCurrAlmSyncTable.pList);
		}		
	}

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSCurrAlm) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_NEW_CURR_ALARM_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}


//同步 newHistAlmSyncTable 表
int alarm_new_hist_alarm_table_sync(void)
{
	time_t time_now = 0;
	struct listnode *pNode  = NULL;
	struct alarm_sync_info  *pAlmSyncDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSHistAlm);

	if(0 == newHistAlmSyncTable.pList->count)
	{
		return 0;
	}
	
	time(&time_now);

	memset(ipc_buf, 0, IPC_MSG_LEN1);

	//缓存时间到，且处于常规状态（即上电同步完成）
	if(((time_now - newHistAlmSyncTable.added_time) > GPN_ALM_SYNC_CACHE_TIME) &&
		(GPN_ALM_SLAVE_POWER_DONE == gAlmSlaveStatus))
	{
		pNode = listhead(newHistAlmSyncTable.pList);
		while(pNode)
		{
			pAlmSyncDB = (struct alarm_sync_info *)pNode->data;
			if(NULL == pAlmSyncDB)
			{
				return 0;
			}

			zlog_debug(ALARM_DBG_SYNC, "%s[%d] : new alarm send : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
				pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
				pAlmSyncDB->almValueRecd.thisTime);
			
			memcpy(ipc_buf + table_num * sizeof(struct alarm_sync_info), pAlmSyncDB, sizeof(struct alarm_sync_info));
			table_num++;

			if(table_num >= max_table_num)
			{
				ipc_send_ha((void *)ipc_buf, (sizeof(struct alarm_sync_info) * table_num), table_num, MODULE_ID_ALARM,
					IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_NEW_HIST_ALARM_SYNC, IPC_OPCODE_ADD, 0);

				table_num = 0;
			}

			//删除该节点
			XFREE(MTYPE_ALARM_ENTRY,pAlmSyncDB);
			list_delete_node(newHistAlmSyncTable.pList, pNode);

			pNode = listhead(newHistAlmSyncTable.pList);
		}		
	}

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSHistAlm) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_NEW_HIST_ALARM_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}


//同步 newEvtSyncTable 表
int alarm_new_event_table_sync(void)
{
	time_t time_now = 0;
	struct listnode *pNode  = NULL;
	stDSEvent *pEventDB = NULL;

	int table_num = 0;
	int max_table_num = IPC_MSG_LEN1/sizeof(stDSEvent);

	if(0 == newEvtSyncTable.pList->count)
	{
		return 0;
	}
	
	time(&time_now);

	memset(ipc_buf, 0, IPC_MSG_LEN1);

	//缓存时间到，且处于常规状态（即上电同步完成）
	if(((time_now - newEvtSyncTable.added_time) > GPN_ALM_SYNC_CACHE_TIME) &&
		(GPN_ALM_SLAVE_POWER_DONE == gAlmSlaveStatus))
	{
		pNode = listhead(newEvtSyncTable.pList);
		while(pNode)
		{
			pEventDB = (stDSEvent *)pNode->data;
			if(NULL == pEventDB)
			{
				return 0;
			}
			
			memcpy(ipc_buf + table_num * sizeof(stDSEvent), pEventDB, sizeof(stEventAlmDB));
			table_num++;

			if(table_num >= max_table_num)
			{
				ipc_send_ha((void *)ipc_buf, (sizeof(stDSEvent) * table_num), table_num, MODULE_ID_ALARM,
					IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_EVENT_SYNC, IPC_OPCODE_ADD, 0);

				table_num = 0;
			}			

			//删除该节点
			XFREE(MTYPE_ALARM_ENTRY,pEventDB);
			list_delete_node(newEvtSyncTable.pList, pNode);

			pNode = listhead(newEvtSyncTable.pList);
		}		
	}

	if(table_num)
	{
		ipc_send_ha((void *)ipc_buf, (sizeof(stDSEvent) * table_num), table_num, MODULE_ID_ALARM,
			IPC_TYPE_SYNC_DATA, GPN_ALM_MSG_EVENT_SYNC, IPC_OPCODE_ADD, 0);
	}

	return 1;
}


//备盘将从主盘接收的当前告警信息同步到告警表，备盘刚上电时的告警同步
int alarm_slave_store_master_curr_alarm(uint8_t *pdata, uint8_t data_num)
{
	stDSCurrAlm *pCurrAlmDB = NULL;
	objLogicDesc viewPort;
	stAlmValueRecd almValueRecd;

	uint8_t cnt = 0;

	while(cnt < data_num)
	{
		pCurrAlmDB = (stDSCurrAlm *)(pdata + sizeof(stDSCurrAlm)*cnt);
		
		if(NULL == pCurrAlmDB)
		{
			return 0;
		}

		zlog_debug(ALARM_DBG_SYNC, "%s[%d] : curr recv : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
			pCurrAlmDB->almType, pCurrAlmDB->ifIndex, pCurrAlmDB->ifIndex2,
			pCurrAlmDB->thisTime);

		memset(&almValueRecd, 0, sizeof(stAlmValueRecd));

		viewPort.devIndex   = pCurrAlmDB->devIndex;
		viewPort.portIndex  = pCurrAlmDB->ifIndex;
		viewPort.portIndex3 = pCurrAlmDB->ifIndex2;
		viewPort.portIndex4 = pCurrAlmDB->ifIndex3;
		viewPort.portIndex5 = pCurrAlmDB->ifIndex4;
		viewPort.portIndex6 = pCurrAlmDB->ifIndex5;

		almValueRecd.almSubType  = pCurrAlmDB->almType;
		almValueRecd.portAlmRank = pCurrAlmDB->level;
		almValueRecd.prodCnt	 = pCurrAlmDB->count;
		almValueRecd.firstTime	 = pCurrAlmDB->firstTime;
		almValueRecd.thisTime	 = pCurrAlmDB->thisTime;

		gpnAlmANtCurrAlmInsertDS(&viewPort, &almValueRecd);
		
		cnt++;
	}	

	return 1;
}


//备盘将从主盘接收的历史告警信息同步到告警表，备盘刚上电时的告警同步
int alarm_slave_store_master_hist_alarm(uint8_t *pdata, uint8_t data_num)
{
	stDSHistAlm *pHistAlmDB = NULL;
	objLogicDesc viewPort;
	stAlmValueRecd almValueRecd;

	uint8_t cnt = 0;

	while(cnt < data_num)
	{		
		pHistAlmDB = (stDSHistAlm *)(pdata + sizeof(stDSHistAlm)*cnt);	
		if(0 == pHistAlmDB)
		{
			return 0;
		}

		memset(&almValueRecd, 0, sizeof(stAlmValueRecd));
		
		viewPort.devIndex   = pHistAlmDB->devIndex;
		viewPort.portIndex  = pHistAlmDB->ifIndex;
		viewPort.portIndex3 = pHistAlmDB->ifIndex2;
		viewPort.portIndex4 = pHistAlmDB->ifIndex3;
		viewPort.portIndex5 = pHistAlmDB->ifIndex4;
		viewPort.portIndex6 = pHistAlmDB->ifIndex5;

		almValueRecd.almSubType  = pHistAlmDB->almType;
		almValueRecd.portAlmRank = pHistAlmDB->level;
		almValueRecd.prodCnt	 = pHistAlmDB->count;
		almValueRecd.firstTime	 = pHistAlmDB->firstTime;
		almValueRecd.thisTime	 = pHistAlmDB->thisTime;
		almValueRecd.disapTime	 = pHistAlmDB->disapTime;
		
		gpnAlmANtHistAlmInsertDS(&viewPort, &almValueRecd);

		cnt++;
	}

	return 1;
}



//备盘将从主盘接收的告警信息同步到告警表，备盘上电完成后的告警同步
int alarm_slave_store_master_new_curr_alarm(uint8_t *pdata, uint8_t data_num)
{
	struct alarm_sync_info *pAlmSyncDB = NULL;
	
	uint8_t cnt = 0;

	while(cnt < data_num)
	{
		pAlmSyncDB = (struct alarm_sync_info *)(pdata + sizeof(struct alarm_sync_info)*cnt);
		if(NULL == pAlmSyncDB)
		{
			return 0;
		}

		zlog_debug(ALARM_DBG_SYNC, "%s[%d] : new alarm recv : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
			pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
			pAlmSyncDB->almValueRecd.thisTime);

		if(GPN_SOCK_MSG_OPT_RISE == pAlmSyncDB->action)
		{
			gpnAlmANtCurrAlmInsertDS(&pAlmSyncDB->viewPort, &pAlmSyncDB->almValueRecd);
		}
		else if(GPN_SOCK_MSG_OPT_CLEAN == pAlmSyncDB->action)
		{
			gpnAlmANtCurrAlmOutDS(&pAlmSyncDB->viewPort, &pAlmSyncDB->almValueRecd);
		}
		else
		{
			zlog_err("%s[%d] : sync error, invalid alarm action\n", __func__, __LINE__);
			return 0;
		}

		cnt++;
	}

	return 1;
}


//备盘将从主盘接收的告警信息同步到告警表，备盘上电完成后的告警同步
int alarm_slave_store_master_new_hist_alarm(uint8_t *pdata, uint8_t data_num)
{
	struct alarm_sync_info *pAlmSyncDB = NULL;
	
	uint8_t cnt = 0;

	while(cnt < data_num)
	{
		pAlmSyncDB = (struct alarm_sync_info *)(pdata + sizeof(struct alarm_sync_info)*cnt);
		if(NULL == pAlmSyncDB)
		{
			return 0;
		}

		zlog_debug(ALARM_DBG_SYNC, "%s[%d] : new alarm recv : index(%08x|%08x|%08x), time(%d)\n", __func__, __LINE__,
			pAlmSyncDB->almValueRecd.almSubType, pAlmSyncDB->viewPort.devIndex, pAlmSyncDB->viewPort.portIndex,
			pAlmSyncDB->almValueRecd.thisTime);

		gpnAlmANtHistAlmInsertDS(&pAlmSyncDB->viewPort, &pAlmSyncDB->almValueRecd);

		cnt++;
	}

	return 1;
}


//备盘将从主盘接收的事件信息通道到事件表
int alarm_slave_store_master_event(uint8_t *pdata, uint8_t data_num)
{
	stDSEvent *pEventDB = NULL;

	objLogicDesc viewPort;
	stEvtValueRecd evtValueRecd;

	uint8_t cnt = 0;

	while(cnt < data_num)
	{		
		pEventDB = (stDSEvent *)(pdata + sizeof(stDSEvent)*cnt);
		if(NULL == pEventDB)
		{
			return 0;
		}

		memset(&evtValueRecd, 0, sizeof(stEvtValueRecd));

		viewPort.devIndex = pEventDB->devIndex;
		viewPort.portIndex = pEventDB->ifIndex;
		viewPort.portIndex3 = pEventDB->ifIndex2;
		viewPort.portIndex4 = pEventDB->ifIndex3;
		viewPort.portIndex5 = pEventDB->ifIndex4;
		viewPort.portIndex6 = pEventDB->ifIndex5;

		evtValueRecd.evtSubType = pEventDB->eventType;
		evtValueRecd.evtRank	= pEventDB->level;
		evtValueRecd.thisTime	= pEventDB->thisTime;
		evtValueRecd.detial		= pEventDB->detail;
	
		gpnAlmANtEvtInsertDS(&viewPort, &evtValueRecd);

		cnt++;
	}
	
	return 1;
}




