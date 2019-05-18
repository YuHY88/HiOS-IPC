/*
 *  cli command MIB group implementation - ifm_snmp.c
 *
 */
/* Portions of this file are subject to the following copyright(s).  See
 * the Net-SNMP's COPYING file for more details and other copyrights
 * that may apply:
 */
/*
 * Portions of this file are copyrighted by:
 * Copyright ?2003 Sun Microsystems, Inc. All rights reserved.
 * Use is subject to license terms specified in the COPYING file
 * distributed with the Net-SNMP package.
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <lib/types.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/route_com.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>

#include "../../l2/trunk.h"
#include "../../l2/l2_snmp.h"
#include "snmp_config_table.h"

#include "mib_trunk.h"

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;

#define                   TRUNK_MAX_NAME_LEGTH      NAME_STRING_LEN

struct list *hhrlacpTable_list  = NULL;
struct list *hhrlacpIfMemberTable_list = NULL;

static time_t hhrlacpTable_time_old  = 0;
static time_t hhrlacpIfMemberTable_time_old = 0;

enum table_lock
{
    UNLOCK,
    LOCK,
};

static enum table_lock hhrlacpTable_lock = UNLOCK;
static enum table_lock hhrlacpIfMemberTable_lock = UNLOCK;

#define TRUNK_TABLE_GET_NEXT_INTERVAL               3
#define TRUNK_PORT_TABLE_GET_NEXT_INTERVAL              3

#define TRUNK_TABLE_REFRESH_TIME                    5
#define TRUNK_PORT_TABLE_REFRESH_TIME                   5

struct trunk_key
{
    uint32_t           trunkid;
    uint32_t           ifindex;
};

struct trunk_port_self
{
    uint16_t           trunkid;
    struct trunk_port info;
};

struct trunk_loadbalcance_global_config
{
    uint8_t       global_load_mode;
};
/*
 * Object ID definitions
 */
/*hhrlacpTable*/
static oid hhrlacpTable_oid[] = {HHRTRUNKOID, 6};
FindVarMethod hhrlacpTable_get;
struct variable2 hhrlacpTable_variables[] =
{
    {hhrlacpFailbackMode,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  2}},
    {hhrlacpFailbackWtr,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  3}},
    {hhrlacpLoadBalance,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  4}},
    {hhrlacpEnable,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  5}},
    {hhrlacpInterval,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  6}},
    {hhrlacpPriority,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  7}},
    {hhrlacpWordMode,  ASN_INTEGER, RONLY, hhrlacpTable_get, 2, {1,  8}}
};

/*hhrlacpIfMemberTable*/
static oid hhrlacpIfMemberTable_oid[] = {HHRTRUNKOID, 7};
FindVarMethod hhrlacpIfMemberTable_get;
struct variable2 hhrlacpIfMemberTable_variables[] =
{
    {hhrlacpMemberIfDesc, ASN_OCTET_STR, RONLY, hhrlacpIfMemberTable_get, 2, {1,  3}},
    {hhrlacpMemberIfWorkMode,        ASN_INTEGER, RONLY, hhrlacpIfMemberTable_get, 2, {1,  4}},
    {hhrlacpMemberIfPriority,        ASN_INTEGER, RONLY, hhrlacpIfMemberTable_get, 2, {1,  5}},
    {hhrlacpMemberIfMSState,        ASN_INTEGER, RONLY, hhrlacpIfMemberTable_get, 2, {1,  6}},
};

/*gloableLACPModeTable*/
static oid hhrlacpGlobalTable_oid[] = {HHRTRUNKOID, 30};
FindVarMethod hhrlacpGlobalTable_get;
struct variable1 hhrlacpGlobalTable_variables[] =
{
    {hhrlacpGlobalLoadBalance, ASN_INTEGER, RONLY, hhrlacpGlobalTable_get, 1, {1}},
};

void init_mib_trunk(void)
{
    REGISTER_MIB("hhrlacpTable", hhrlacpTable_variables,
                 variable2, hhrlacpTable_oid);

    REGISTER_MIB("hhrlacpIfMemberTable", hhrlacpIfMemberTable_variables,
                 variable2, hhrlacpIfMemberTable_oid);

    REGISTER_MIB("hhrlacpGlobalTable", hhrlacpGlobalTable_variables,
             variable1, hhrlacpGlobalTable_oid);
}

/* 批量返回 trunkid 后面的 100 个 trunk 接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */
struct ipc_mesg_n *trunk_get_bulk(uint16_t trunkid, int module_id, int *pdata_num)
{
    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(NULL, 0, 1 , MODULE_ID_L2, module_id,
                                                         IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_GET_BULK, trunkid);*/
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_L2, module_id,
                                                         IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_GET_BULK, trunkid,2000);
	if (NULL == pmesg)
	{
		*pdata_num = 0;
		return NULL;
	}

	if (NULL == pmesg->msg_data || 0 == pmesg->msghdr.data_num)
	{
		mem_share_free(pmesg,MODULE_ID_L2);
		*pdata_num = 0;
		return NULL;
	}
    *pdata_num = pmesg->msghdr.data_num;
    return  pmesg;

}

/* 批量返回 ifindex 后面的 100 个 trunk成员 接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */
struct ipc_mesg_n *trunk_port_get_bulk(uint16_t trunkid, int module_id, int *pdata_num)
{
   /* struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(NULL, 0, 1 , MODULE_ID_L2, module_id,
                                                         IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_GET_BULK, trunkid);*/
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_L2, module_id,
                                                         IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_GET_BULK, trunkid,2000);

  	if (NULL == pmesg)
	{
		*pdata_num = 0;
		return NULL;
	}

	if ( NULL == pmesg->msg_data || 0 == pmesg->msghdr.data_num )
	{
		mem_share_free(pmesg,MODULE_ID_L2);
		*pdata_num = 0;
		return NULL;
	}
    *pdata_num = pmesg->msghdr.data_num;
    return  pmesg;

    return NULL;
}


void hhrlacpTable_node_free(struct trunk *node)
{
    XFREE(0, node);
}

void hhrlacpIfMemberTable_node_free(struct trunk_port_self *node)
{
    XFREE(0, node);
}

static void *trunk_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case hhrlacpTable:
            return XCALLOC(0, sizeof(struct trunk));

        case hhrlacpIfMemberTable:
            return XCALLOC(0, sizeof(struct trunk_port_self));

        default:
            break;
    }

    return NULL;
}

static void trunk_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case hhrlacpTable:
            node_add = trunk_node_xcalloc(hhrlacpTable);
            memcpy(node_add, node, sizeof(struct trunk));
            listnode_add(hhrlacpTable_list, node_add);
            break;

        case hhrlacpIfMemberTable:
            node_add = trunk_node_xcalloc(hhrlacpIfMemberTable);
            memcpy(node_add, node, sizeof(struct trunk_port_self));
            listnode_add(hhrlacpIfMemberTable_list, node_add);
            break;

        default:
            break;
    }
}


static void *trunk_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    struct trunk_port_self  *data1_find;
    struct trunk_key    *index1;

    struct trunk    *data2_find;
    u_int32_t       index2;

    switch (table_num)
    {
        case hhrlacpIfMemberTable:
            index1 = (struct trunk_key *)index_input;

            for (ALL_LIST_ELEMENTS(hhrlacpIfMemberTable_list, node, nnode, data1_find))
            {
                if ((0 == index1->trunkid)\
                        && (0 == index1->ifindex))
                {
                    return hhrlacpIfMemberTable_list->head->data;
                }

                if ((data1_find->trunkid == index1->trunkid) && \
                        (data1_find->info.ifindex == index1->ifindex))
                {
                    if (1 == exact) //get
                    {
                        return data1_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            break;

        case hhrlacpTable:
            index2 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrlacpTable_list, node, nnode, data2_find))
            {
                if (0 == index2)
                {
                    return hhrlacpTable_list->head->data;
                }

                if (data2_find->trunkid == index2)
                {
                    if (1 == exact) //get
                    {
                        return data2_find;
                    }
                    else
                    {
                        if (NULL == node->next)
                        {
                            return NULL;
                        }
                        else
                        {
                            return node->next->data;
                        }
                    }
                }
            }

            break;

        default:
            break;
    }

    return NULL;
}


static void trunk_node_del_all(int table_num)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    switch (table_num)
    {
        case hhrlacpTable:
            list_delete_all_node(hhrlacpTable_list);
            break;

        case hhrlacpIfMemberTable:
            list_delete_all_node(hhrlacpIfMemberTable_list);
            break;

        default:
            break;
    }
}


static void l3_list_init(void)
{
    hhrlacpTable_list = list_new();
    hhrlacpTable_list->del = (void (*)(void *))trunk_node_del_all;

    hhrlacpIfMemberTable_list = list_new();
    hhrlacpIfMemberTable_list->del = (void (*)(void *))trunk_node_del_all;
}



int hhrlacpTable_get_data_from_ipc(u_int32_t index)
{
    struct trunk *ptrunkeEntry = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_num = 0;
    int ret = 0;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s' index [%d] count [%d]\n", __FILE__, __LINE__, __func__ , index , data_num);

    pmesg = trunk_get_bulk(index, MODULE_ID_SNMPD, &data_num);
	
    if (0 == data_num)
    {
    	 //mem_share_free(pmesg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
     	ptrunkeEntry = (struct trunk *) pmesg->msg_data;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            trunk_node_add(hhrlacpTable, ptrunkeEntry);
            ptrunkeEntry++;
        }

        time(&hhrlacpTable_time_old);   //refresh time_old after refresh cache data
         mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct trunk *hhrlacpTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct trunk *phhrlacpTableEntry = NULL;
    struct trunk *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > TRUNK_TABLE_GET_NEXT_INTERVAL))
    {
        hhrlacpTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrlacpTable_time_old) > TRUNK_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrlacpTable_lock))
    {
        if (NULL == hhrlacpTable_list)
        {
            hhrlacpTable_list = list_new();
            hhrlacpTable_list->del = (void (*)(void *))hhrlacpTable_node_free;
        }

        if (0 != hhrlacpTable_list->count)
        {
            trunk_node_del_all(hhrlacpTable);
        }
    }

    /* then lock */
    hhrlacpTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrlacpTable_list)
    {
        hhrlacpTable_list = list_new();
        hhrlacpTable_list->del = (void (*)(void *))hhrlacpTable_node_free;
    }

    if (0 == hhrlacpTable_list->count)
    {
        index_temp = 0;
        ret = hhrlacpTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    phhrlacpTableEntry = (struct trunk *)trunk_node_lookup(exact, hhrlacpTable, index);

    if (NULL == phhrlacpTableEntry) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrlacpTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct trunk *)hhrlacpTable_list->tail->data;
            ret = hhrlacpTable_get_data_from_ipc(entry_temp->trunkid);

            if (FALSE == ret)
            {
                hhrlacpTable_lock = UNLOCK; //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            phhrlacpTableEntry = (struct trunk *)trunk_node_lookup(exact, hhrlacpTable, index);

            if (NULL != phhrlacpTableEntry)
            {
                *index_next = phhrlacpTableEntry->trunkid;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrlacpTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return phhrlacpTableEntry;
            }
        }
    }

    *index_next = phhrlacpTableEntry->trunkid;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrlacpTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return phhrlacpTableEntry;
}



u_char *hhrlacpTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct trunk   *phhrlacpTable = NULL;
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;

//    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] enter hhrRIPInstanceTable_get\n", __FUNCTION__, __LINE__);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    phhrlacpTable = hhrlacpTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == phhrlacpTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    switch (vp->magic)
    {
        case hhrlacpFailbackMode:
            int_value = phhrlacpTable->failback;

            if (int_value == 0)
            {
                int_value = 1 ;
            }
            else
            {
                int_value = 2 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpFailbackWtr:
            int_value = phhrlacpTable->wtr;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpLoadBalance:
            int_value = phhrlacpTable->ecmp_mode;

            if (int_value == TRUNK_BALANCE_LABEL)
            {
                int_value = 1 ;
            }
            else if (int_value == TRUNK_BALANCE_DIP)
            {
                int_value = 3 ;
            }
            else if (int_value == TRUNK_BALANCE_SIP)
            {
                int_value = 4 ;
            }
            else if (int_value == TRUNK_BALANCE_SIP_DIP)
            {
                int_value = 2 ;
            }
            else if (int_value == TRUNK_BALANCE_SMAC)
            {
                int_value = 6 ;
            }
            else if (int_value == TRUNK_BALANCE_DMAC)
            {
                int_value = 5 ;
            }
            else if (int_value == TRUNK_BALANCE_SMAC_DMAC)
            {
                int_value = 8 ;
            }
            else if (int_value == TRUNK_BALANCE_SPORT_DMAC)
            {
                int_value = 7 ;
            }
	   else if (int_value == TRUNK_BALANCE_SPORT_DPORT)
            {
                int_value = 9 ;
            }
            else
            {
                int_value = 1 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpEnable:
            int_value = phhrlacpTable->lacp_enable;

            if (int_value == TRUNK_ENABLE)
                //int_value = 2 ;
            {
                int_value = 1 ;
            }
            else if (int_value == TRUNK_DISABLE)
                //int_value = 1 ;
            {
                int_value = 2 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpInterval:
            int_value = phhrlacpTable->lacp_interval;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpPriority:
            int_value = phhrlacpTable->priority;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpWordMode:
            if (TRUNK_MODE_BACKUP == phhrlacpTable->work_mode)
            {
                int_value = 1;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

int hhrlacpIfMemberTable_get_data_from_ipc(struct trunk_key *index)
{
    struct trunk_port *ptrunk_port = NULL;
    struct  ipc_mesg_n *pmesg = NULL;
    int data_num = 0;
    int ret = 0;
    int next_trunk_id = 0;
    struct trunk_port_self tmp_self ;
    pmesg = trunk_port_get_bulk(index->trunkid, MODULE_ID_SNMPD, &data_num);
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s'index->trunkid [%d] count [%d]\n", __FILE__, __LINE__, __func__ , index->trunkid , data_num);

    if (0 == data_num)
    {
    	//mem_share_free(pmesg, MODULE_ID_SNMPD);
        return (FALSE);
    }
    else
    {
		ptrunk_port = (struct trunk_port *) pmesg->msg_data;
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            memcpy(&tmp_self.info , ptrunk_port , sizeof(struct trunk_port)) ;
            tmp_self.trunkid =  index->trunkid ;
            trunk_node_add(hhrlacpIfMemberTable, &tmp_self);
            ptrunk_port++;
        }

        time(&hhrlacpIfMemberTable_time_old);   //refresh time_old after refresh cache data
		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct trunk_port_self *hhrlacpIfMemberTable_get_data_by_index(int exact, struct trunk_key *index,
                                                               struct trunk_key *index_next)
{
    struct trunk_port_self *phhrlacpIfMemberTable = NULL;
    struct trunk_port_self entry_temp;
    struct trunk_key index_temp;
    int next_trunk_id = 0;
    static time_t getNext_time_old; //to store the get-next operate time of the last time

    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > TRUNK_PORT_TABLE_GET_NEXT_INTERVAL))
    {
        hhrlacpIfMemberTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrlacpIfMemberTable_time_old) > TRUNK_PORT_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrlacpIfMemberTable_lock))
    {
        if (NULL == hhrlacpIfMemberTable_list)
        {
            hhrlacpIfMemberTable_list = list_new();
            hhrlacpIfMemberTable_list->del = (void (*)(void *))hhrlacpIfMemberTable_node_free;
        }

        if (0 != hhrlacpIfMemberTable_list->count)
        {
            trunk_node_del_all(hhrlacpIfMemberTable);
        }
    }

    /* then lock */
    hhrlacpIfMemberTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrlacpIfMemberTable_list)
    {
        hhrlacpIfMemberTable_list = list_new();
        hhrlacpIfMemberTable_list->del = (void (*)(void *))hhrlacpIfMemberTable_node_free;
    }

    if (0 == hhrlacpIfMemberTable_list->count)
    {
        if ((0 == index->trunkid && 0 == index->ifindex))
        {
            hhrlacpTable_get_data_by_index(0, &index->trunkid , &next_trunk_id);
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s'index->trunkid  [%d] next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , index->trunkid , next_trunk_id);

            if (0 != next_trunk_id)
            {
                index_temp.trunkid = next_trunk_id ;
            }
            else
            {
                return (NULL);
            }

            ret = hhrlacpIfMemberTable_get_data_from_ipc(&index_temp);

            if (FALSE == ret)
            {
                return (NULL);
            }

            phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s' next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , next_trunk_id);

            if (NULL != phhrlacpIfMemberTable)
            {
                index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
                index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrlacpIfMemberTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return phhrlacpIfMemberTable;
            }

        }
        else
        {
            ret = hhrlacpIfMemberTable_get_data_from_ipc(index);

            if (FALSE == ret)
            {
                return (NULL);
            }

            phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s' next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , next_trunk_id);

            if (NULL != phhrlacpIfMemberTable)
            {
                index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
                index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrlacpIfMemberTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return phhrlacpIfMemberTable;
            }
        }
    }

    phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);

    if (NULL == phhrlacpIfMemberTable)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrlacpIfMemberTable_list->tail->data)
            {
                hhrlacpTable_get_data_by_index(0, &index->trunkid , &next_trunk_id);
                zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s'index->trunkid  [%d] next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , index->trunkid , next_trunk_id);

                if (0 != next_trunk_id)
                {
                    index_temp.trunkid = next_trunk_id ;
                }
                else
                {
                    return (NULL);
                }

                ret = hhrlacpIfMemberTable_get_data_from_ipc(&index_temp);

                if (FALSE == ret)
                {
                    return (NULL);
                }

                phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);
                zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s' next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , next_trunk_id);

                if (NULL != phhrlacpIfMemberTable)
                {
                    index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
                    index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

                    if (1 == exact) //get operate && find node, unlock
                    {
                        hhrlacpIfMemberTable_lock = UNLOCK;
                    }
                    else
                    {
                        time(&getNext_time_old);    //refresh get-next time of this time
                    }

                    return phhrlacpIfMemberTable;
                }
                else
                {
                    return (NULL);
                }

            }

            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s'\n", __FILE__, __LINE__, __func__);
            memcpy(&entry_temp, hhrlacpIfMemberTable_list->tail->data, sizeof(struct trunk_port_self));
            index_temp.trunkid = entry_temp.trunkid ;
            index_temp.ifindex = entry_temp.info.ifindex ;
            hhrlacpTable_get_data_by_index(0, &index->trunkid , &next_trunk_id);
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s'index->trunkid  [%d] next_trunk_id [%d]\n", __FILE__, __LINE__, __func__ , index->trunkid , next_trunk_id);

            if (0 != next_trunk_id)
            {
                index_temp.trunkid = next_trunk_id ;
            }
            else
            {
                return (NULL);
            }

            ret = hhrlacpIfMemberTable_get_data_from_ipc(&index_temp);

            if (FALSE == ret)
            {
                return (NULL);
            }

            phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);

            if (NULL != phhrlacpIfMemberTable)
            {
                //memcpy(index_next, &pOSPFAreaTable->key, sizeof(struct ospf_key));
                index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
                index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrlacpIfMemberTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return phhrlacpIfMemberTable;
            }

            phhrlacpIfMemberTable = (struct trunk_port_self *)trunk_node_lookup(exact, hhrlacpIfMemberTable, index);

            if (NULL != phhrlacpIfMemberTable)
            {
                //memcpy(index_next, &pOSPFAreaTable->key, sizeof(struct ospf_key));
                index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
                index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrlacpIfMemberTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return phhrlacpIfMemberTable;
            }
            else
            {
                return (NULL);
            }

        }
    }

    index_next->trunkid = phhrlacpIfMemberTable->trunkid ;
    index_next->ifindex = phhrlacpIfMemberTable->info.ifindex ;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrlacpIfMemberTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return phhrlacpIfMemberTable;
}

u_char *hhrlacpIfMemberTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct trunk_port_self   *phhrlacpIfMemberTableEntry = NULL;
    struct trunk_key index1 = {0, 0};
    struct trunk_key index1_next = {0, 0};
    int ret = 0;
    struct trunk   *p_trunk = NULL;
    uint32_t next_trunk_id , trunkid = 0;

    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &index1.trunkid, &index1.ifindex , exact);

    if (ret < 0)
    {
        return NULL;
    }

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': trunkid [%d] ifindex [%d]\n", __FILE__, __LINE__, __func__ , index1.trunkid , index1.ifindex);

    phhrlacpIfMemberTableEntry = hhrlacpIfMemberTable_get_data_by_index(exact, &index1, &index1_next);

    if (NULL == phhrlacpIfMemberTableEntry)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, index1_next.trunkid, index1_next.ifindex);
    }

    switch (vp->magic)
    {
        case hhrlacpMemberIfDesc:
            memset(str_value, 0, TRUNK_MAX_NAME_LEGTH);
            ifm_get_name_by_ifindex(phhrlacpIfMemberTableEntry->info.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrlacpMemberIfWorkMode:
            int_value = phhrlacpIfMemberTableEntry->info.passive;

            if (int_value == 1)
            {
                int_value = 2 ;
            }
            else
            {
                int_value = 1 ;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpMemberIfPriority:
            int_value = phhrlacpIfMemberTableEntry->info.priority;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrlacpMemberIfMSState:
            trunkid = index1_next.trunkid;
            p_trunk = hhrlacpTable_get_data_by_index(1, &trunkid , &next_trunk_id);

            if (NULL != p_trunk)
            {
                if (TRUNK_DISABLE == p_trunk->lacp_enable)
                {
                    if (TRUNK_MODE_BACKUP == p_trunk->work_mode)
                    {
                        int_value = (p_trunk->master_if == phhrlacpIfMemberTableEntry->info.ifindex) ?  1 : 2;
                    }
                    else
                    {
                        int_value = 3;
                    }
                }
                else
                {
                    if (TRUNK_MODE_BACKUP == p_trunk->work_mode)
                    {
                        int_value = (phhrlacpIfMemberTableEntry->info.link.sync_state == 1) ?  1 : 2;
                    }
                    else
                    {
                        int_value = 3;
                    }

                }
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;

}

int hhrlacpGlobalTable_get_data(struct trunk_loadbalcance_global_config	*global_load_config)
{
    int data_num = 0;

	#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(NULL, 0, 1 , MODULE_ID_L2, MODULE_ID_SNMPD,
							IPC_TYPE_TRUNK, TRUNK_INFO_GLOBAL_LOADBALANCE, IPC_OPCODE_GET_BULK, 0);
	#endif
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_L2, MODULE_ID_SNMPD,
                                  IPC_TYPE_TRUNK, TRUNK_INFO_GLOBAL_LOADBALANCE, IPC_OPCODE_GET_BULK, 0,2000);

	if (NULL==pmesg)
	{
		return -1;
	}
	
   if (NULL == pmesg->msg_data || 0 == pmesg->msghdr.data_num)
   {
	   mem_share_free(pmesg,MODULE_ID_L2);
	   return -1;
   }

  
	memcpy(global_load_config, pmesg->msg_data, sizeof(struct trunk_loadbalcance_global_config));
	mem_share_free(pmesg, MODULE_ID_SNMPD);
  
   return 0;
}


u_char *hhrlacpGlobalTable_get(struct variable *vp,
								 oid *name,
								 size_t *length,
								 int exact, size_t *var_len, WriteMethod **write_method)
{
	struct trunk_loadbalcance_global_config	 hhrlacpGlobalTable_getTableEntry;
	int data_num = 0;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }
	hhrlacpGlobalTable_get_data(&hhrlacpGlobalTable_getTableEntry);
	
	switch (vp->magic)
	{
		case hhrlacpGlobalLoadBalance:
		 int_value = hhrlacpGlobalTable_getTableEntry.global_load_mode;

		if (int_value == TRUNK_BALANCE_LABEL)
		{
		    int_value = 1 ;
		}
		else if (int_value == TRUNK_BALANCE_DIP)
		{
		    int_value = 3 ;
		}
		else if (int_value == TRUNK_BALANCE_SIP)
		{
		    int_value = 4 ;
		}
		else if (int_value == TRUNK_BALANCE_SIP_DIP)
		{
		    int_value = 2 ;
		}
		else if (int_value == TRUNK_BALANCE_SMAC)
		{
		    int_value = 6 ;
		}
		else if (int_value == TRUNK_BALANCE_DMAC)
		{
		    int_value = 5 ;
		}
		else if (int_value == TRUNK_BALANCE_SMAC_DMAC)
		{
		    int_value = 8 ;
		}
		else if (int_value == TRUNK_BALANCE_SPORT_DMAC)
		{
		    int_value = 7 ;
		}
		else if (int_value == TRUNK_BALANCE_SPORT_DPORT)
		{
		    int_value = 9 ;
		}
		else
		{
		    int_value = 1 ;
		}

		*var_len = sizeof(int);
		return (u_char *)&int_value;
        default:
            return NULL;
	}

    return NULL;
}
