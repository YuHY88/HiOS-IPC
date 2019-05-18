
#include <string.h>
#include <lib/rmon_common.h>
#include <lib/msg_ipc.h>

#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"



struct ipc_mesg_n *etherStatsTable_bulk(uint32_t stat_id, int module_id, int*pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &stat_id, sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
										IPC_TYPE_SNMP, RMON_etherStatsTable, IPC_OPCODE_GET_BULK,0 );
#endif
        struct ipc_mesg_n *pmsg = ipc_sync_send_n2(&stat_id,sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
                        IPC_TYPE_SNMP, RMON_etherStatsTable, IPC_OPCODE_GET_BULK,0 ,2000);

    if ( pmsg )
    {
        *pdata_num = pmsg->msghdr.data_num;
        return ( struct ipc_mesg_n * ) pmsg;
    }

    return NULL;
}

struct ipc_mesg_n *historyControlTable_bulk(uint32_t his_ctr_id, int module_id, int*pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &his_ctr_id, sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
										IPC_TYPE_SNMP, RMON_historyControlTable, IPC_OPCODE_GET_BULK,0 );
#endif
    struct ipc_mesg_n *pmsg = ipc_sync_send_n2( &his_ctr_id, sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
                                            IPC_TYPE_SNMP, RMON_historyControlTable, IPC_OPCODE_GET_BULK,0 ,2000);

    if ( pmsg )
    {
        *pdata_num = pmsg->msghdr.data_num;
        return ( struct ipc_mesg_n * ) pmsg;
    }

    return NULL;	
}

struct ipc_mesg_n *alarmTable_bulk(uint32_t alarm_id, int module_id, int*pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &alarm_id, sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
                            IPC_TYPE_SNMP, RMON_alarmTable, IPC_OPCODE_GET_BULK, alarm_id );
#endif

    struct ipc_mesg_n *pmsg = ipc_sync_send_n2( &alarm_id, sizeof(uint32_t), 1 , MODULE_ID_RMON, module_id, 
                            IPC_TYPE_SNMP, RMON_alarmTable, IPC_OPCODE_GET_BULK, alarm_id,2000);

    if ( pmsg )
    {
        *pdata_num = pmsg->msghdr.data_num;
        return ( struct ipc_mesg_n * ) pmsg;
    }

    return NULL;
}


struct ipc_mesg_n *eventTable_bulk(uint32_t event_id, int module_id, int*pdata_num)
{
#if 0
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &event_id , sizeof(uint32_t) , 1 , MODULE_ID_RMON, module_id, 
										IPC_TYPE_SNMP, RMON_eventTable, IPC_OPCODE_GET_BULK, event_id );
#endif
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2( &event_id , sizeof(uint32_t) , 1 , MODULE_ID_RMON, module_id, 
                                            IPC_TYPE_SNMP, RMON_eventTable, IPC_OPCODE_GET_BULK, event_id,2000);
    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct ipc_mesg_n * ) pmesg;
    }

    return NULL;
}

struct ipc_mesg_n *etherHistoryTable_bulk(uint32_t his_ctr_id , int module_id, int*pdata_num , uint32_t his_sample_id)
{
#if 0    
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( &his_sample_id, sizeof(uint32_t), 1  , MODULE_ID_RMON, module_id, 
            IPC_TYPE_SNMP, RMON_etherHistoryTable, IPC_OPCODE_GET_BULK, his_ctr_id );
#endif
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2( &his_sample_id, sizeof(uint32_t), 1  , MODULE_ID_RMON, module_id, 
            IPC_TYPE_SNMP, RMON_etherHistoryTable, IPC_OPCODE_GET_BULK, his_ctr_id,2000);

    if ( pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct ipc_mesg_n * ) pmesg;
    }

    return NULL;
}



