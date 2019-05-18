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
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <net-snmp-config.h>
#include <lib/types.h>
#include <types.h>

#include <net-snmp-includes.h>
#include <net-snmp-agent-includes.h>

#include <snmp_index_operater.h>

#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <l2/l2_if.h>
#include <l2/vlan.h>
#include <l2/qinq.h>
#include <lib/log.h>
#include "snmp_config_table.h"

#include "mib_vlan.h"

/*local temp variable*/
#define BITS_STRING_LEN      512
#define BITS_STRING_LEN_VLAN_LIST      64
static uchar     str_value[BITS_STRING_LEN];
static int       int_value = 0;
static uint32_t  uint_value = 0;

/* table list define */
struct list *hhrVlanDatabaseTable_list  = NULL;
struct list *hhrVlanConfigTable_list  = NULL;
struct list *hhrVlanMappingTable_list  = NULL;
struct list *hhrVlanMapping2LayerTable_list = NULL;

/* variable to store refresh data time of the last time */
static time_t  hhrVlanDatabaseTable_time_old  = 0;
static time_t  hhrVlanConfigTable_time_old  = 0;
static time_t  hhrVlanMappingTable_time_old  = 0;
static time_t  hhrVlanMapping2LayerTable_time_old  = 0;

/* lock of tables */
enum table_lock
{
    UNLOCK,
    LOCK,
};
static enum table_lock hhrVlanDatabaseTable_lock = UNLOCK;
static enum table_lock hhrVlanConfigTable_lock = UNLOCK;
static enum table_lock hhrVlanMappingTable_lock = UNLOCK;
static enum table_lock hhrVlanMapping2LayerTable_lock = UNLOCK;


/*
 * define the structure we're going to ask the agent to register our
 * information at
 */
static oid hhrVlanDatabaseTable_oid[] = {VLAN_HHRVLANCONFIG, 2};
FindVarMethod hhrVlanDatabaseTable_get;
struct variable2 hhrVlanDatabaseTable_variables[] =
{
    {hhrVlanMacLearn,              ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 2}},
    {hhrVlanMacLearnLimit,         ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 3}},
    {hhrVlanStormSupressUnicast,   ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 4}},
    {hhrVlanStormSupressBroadcast, ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 5}},
    {hhrVlanStormSupressMulticast, ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 6}},
    {hhrVlanMacLearnAction,        ASN_INTEGER, RONLY, hhrVlanDatabaseTable_get, 2, {1, 7}}
} ;

static oid hhrIfVlanConfigTable_oid[] = {VLAN_HHRVLANCONFIG, 3};
FindVarMethod hhrIfVlanConfigTable_get;
struct variable2 hhrL2SubIfVlanConfigTable_variables[] =
{
    {hhrVlanConfigIfDescr,    ASN_OCTET_STR, RONLY, hhrIfVlanConfigTable_get, 2, {1, 1}},
    {hhrVlanConfigPVID,       ASN_INTEGER,   RONLY, hhrIfVlanConfigTable_get, 2, {1, 2}},
    {hhrVlanConfigVlanMode,   ASN_INTEGER,   RONLY, hhrIfVlanConfigTable_get, 2, {1, 3}},
    {hhrVlanConfigVlanMember, ASN_OCTET_STR, RONLY, hhrIfVlanConfigTable_get, 2, {1, 4}},
    {hhrVlanConfigunTagVlan,  ASN_OCTET_STR, RONLY, hhrIfVlanConfigTable_get, 2, {1, 5}}
} ;

static oid hhrVlanMappingTable_oid[] = {VLAN_HHRVLANCONFIG, 4};
FindVarMethod hhrVlanMappingTable_get;
struct variable2 hhrVlanMappingTable_variables[] =
{
    {hhrVlanMappingIfDesrc,  ASN_OCTET_STR, RONLY, hhrVlanMappingTable_get, 2, {1, 3}},
    {hhrVlanMappingAction,   ASN_INTEGER,   RONLY, hhrVlanMappingTable_get, 2, {1, 4}},
    {hhrVlanMappingSvlan,    ASN_INTEGER,   RONLY, hhrVlanMappingTable_get, 2, {1, 5}},
    {hhrVlanMappingSvlanCos, ASN_INTEGER,   RONLY, hhrVlanMappingTable_get, 2, {1, 6}},
    {hhrVlanMappingCvlan,    ASN_INTEGER,   RONLY, hhrVlanMappingTable_get, 2, {1, 7}}
} ;

static oid hhrVlanMapping2LayerTable_oid[] = {VLAN_HHRVLANCONFIG, 5};
FindVarMethod hhrVlanMapping2LayerTable_get;
struct variable2 hhrVlanMapping2LayerTable_variables[] =
{
    {HhrVlanMapping2LayerIfDesrc,  ASN_OCTET_STR, RONLY, hhrVlanMapping2LayerTable_get, 2, {1, 4}},
    {HhrVlanMapping2LayerAction,   ASN_INTEGER,   RONLY, hhrVlanMapping2LayerTable_get, 2, {1, 5}},
    {HhrVlanMapping2LayerSvlan,    ASN_INTEGER,   RONLY, hhrVlanMapping2LayerTable_get, 2, {1, 6}},
    {HhrVlanMapping2LayerSvlanCos, ASN_INTEGER,   RONLY, hhrVlanMapping2LayerTable_get, 2, {1, 7}},
    {HhrVlanMapping2LayerCvlan,    ASN_INTEGER,   RONLY, hhrVlanMapping2LayerTable_get, 2, {1, 8}}
} ;

static oid hhrVlanConfigList_oid[] = {VLAN_HHRVLANCONFIG};
FindVarMethod hhrIfVlanListForIpranTable_get;
struct variable1 hhrIfVlanListForIpranTable_variables[] =
{
    {hhrl2VlanListForIpran1,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {10}},
    {hhrl2VlanListForIpran2,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {11}},
    {hhrl2VlanListForIpran3,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {12}},
    {hhrl2VlanListForIpran4,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {13}},
    {hhrl2VlanListForIpran5,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {14}},
    {hhrl2VlanListForIpran6,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {15}},
    {hhrl2VlanListForIpran7,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {16}},
    {hhrl2VlanListForIpran8,  ASN_OCTET_STR, RONLY, hhrIfVlanListForIpranTable_get, 1, {17}}
};

void init_mib_vlan(void)
{
    REGISTER_MIB("hhrVlanDatabaseTable", hhrVlanDatabaseTable_variables, variable2,
                 hhrVlanDatabaseTable_oid) ;
    REGISTER_MIB("hhrIfVlanConfigTable", hhrL2SubIfVlanConfigTable_variables, variable2,
                 hhrIfVlanConfigTable_oid) ;
    REGISTER_MIB("hhrVlanMappingTable", hhrVlanMappingTable_variables, variable2,
                 hhrVlanMappingTable_oid) ;
    REGISTER_MIB("hhrVlanMapping2LayerTable", hhrVlanMapping2LayerTable_variables, variable2,
                 hhrVlanMapping2LayerTable_oid) ;
    REGISTER_MIB("hhrVlanConfigList", hhrIfVlanListForIpranTable_variables,
                 variable1, hhrVlanConfigList_oid);

    snmp_vlan_list_init();
}

struct ipc_mesg_n *snmp_get_vlan_info_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    /*struct ipc_mesg   *pmesg = ipc_send_common_wait_reply1(NULL, 0, 1 ,
                                                           MODULE_ID_L2, module_id, IPC_TYPE_VLAN, VLAN_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct vlan_info *) pmesg->msg_data;
    }

    return NULL;*/

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1, MODULE_ID_L2, module_id, 
		IPC_TYPE_VLAN, VLAN_INFO_MAX, IPC_OPCODE_GET_BULK, 0, 1000);

    if (pMsgRcv != NULL)
    {
        if(pMsgRcv->msg_data == NULL || pMsgRcv->msghdr.data_num == 0)
        {
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return NULL;
        }
        else
        {
            *pdata_num = pMsgRcv->msghdr.data_num;
            return pMsgRcv;
        }
    }
}

/* pdata_num 返回实际的switch_info数量 */
struct ipc_mesg_n *snmp_get_switch_info_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    /*struct ipc_mesg   *pmesg = ipc_send_common_wait_reply1(NULL, 0, 1 ,
                                                           MODULE_ID_L2, module_id, IPC_TYPE_L2IF, L2IF_INFO_MAX, IPC_OPCODE_GET_BULK, ifindex);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct l2if *) pmesg->msg_data;
    }

    return NULL;*/

	struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1, MODULE_ID_L2, module_id, 
		                        IPC_TYPE_L2IF, L2IF_INFO_MAX, IPC_OPCODE_GET_BULK, 0, 1000);
    
    if (pMsgRcv != NULL)
    {
        if(pMsgRcv->msg_data == NULL || pMsgRcv->msghdr.data_num == 0)
        {
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return NULL;
        }
        else
        {
            *pdata_num = pMsgRcv->msghdr.data_num;
            return pMsgRcv;
        }
    }

    return NULL;
}

/* get interface switch_vlan */
struct ipc_mesg_n *snmp_get_switch_vlan(uint32_t ifindex, int module_id)
{
     /*return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_L2, module_id,
                                      IPC_TYPE_L2IF, L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_GET_BULK, ifindex);*/

	return ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1, MODULE_ID_L2, module_id, 
		                IPC_TYPE_L2IF, L2IF_INFO_SWITCH_VLAN, IPC_OPCODE_GET_BULK, 0, 1000);
}

/* get interface switch_vlan tag or untag */
struct ipc_mesg_n  *snmp_get_switch_vlan_tag(uint32_t ifindex, int module_id)
{
#if 0
    return ipc_send_common_wait_reply(&ifindex, sizeof(ifindex), 1, MODULE_ID_L2, module_id,
                                      IPC_TYPE_L2IF, L2IF_INFO_SWITCH_VLAN_TAG, IPC_OPCODE_GET_BULK, 0);
#endif

	return ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1, MODULE_ID_L2, module_id, 
		                IPC_TYPE_L2IF, L2IF_INFO_SWITCH_VLAN_TAG, IPC_OPCODE_GET_BULK, 0, 1000);
}


/* pdata_num 返回实际的qinq数量 */
struct ipc_mesg_n *snmp_qinq_list_get_bulk(struct l2if_vlan_mapping *key_entry , int module_id, int *pdata_num)
{
    /*struct ipc_mesg   *pmesg = ipc_send_common_wait_reply1(key_entry, sizeof(struct l2if_vlan_mapping), 1 ,
                                                           MODULE_ID_L2, module_id, IPC_TYPE_L2IF, L2IF_INFO_QINQ, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct l2if_vlan_mapping *) pmesg->msg_data;
    }

    return NULL;*/

    struct ipc_mesg_n* pMsgRcv = ipc_sync_send_n2(key_entry, sizeof(struct l2if_vlan_mapping), 1, 
                    MODULE_ID_L2, module_id, IPC_TYPE_L2IF, L2IF_INFO_QINQ, IPC_OPCODE_GET_BULK, 0, 1000);

    if (pMsgRcv != NULL)
    {
        if(pMsgRcv->msg_data == NULL || pMsgRcv->msghdr.data_num == 0)
        {
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return NULL;
        }
        else
        {
            *pdata_num = pMsgRcv->msghdr.data_num;
            return pMsgRcv;
        }
    }
}

/* hhrVlanDatabaseTable*/

void hhrVlanDatabaseTable_node_free(struct vlan_info *node)
{
    XFREE(0, node);
}

int hhrVlanDatabaseTable_get_data_from_ipc(u_int32_t index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct vlan_info *pVlanDatabaseEntry = NULL;
    int data_num = 0;
    int ret = 0;

    pMsgRcv = snmp_get_vlan_info_bulk(index, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        //  zlog_debug(SNMP_MTYPE_MIB_MODULE, "%s[%d] snmp_get_vlan_info_bulk data_num=0\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    else
    {
//      zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] snmp_get_vlan_info_bulk data_num=%d\n", __FUNCTION__, __LINE__,data_num);
		pVlanDatabaseEntry = (struct vlan_info *)pMsgRcv->msg_data;
		if(pVlanDatabaseEntry)
		{
            for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
            {
                snmp_vlan_node_add(HHR_VLAN_DATABASE_TABLE, pVlanDatabaseEntry);
                pVlanDatabaseEntry++;
            }
        }

        time(&hhrVlanDatabaseTable_time_old);   //refresh time_old after refresh cache data
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct vlan_info *hhrVlanDatabaseTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct vlan_info *pVlanDatabaseEntry = NULL;
    struct vlan_info *entry_temp = NULL;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_VLAN_DATABASE_TABLE_GET_NEXT_INTERVAL))
    {
        hhrVlanDatabaseTable_lock = UNLOCK;
    }
	else 
	{   /* if get-next doesn't timeout ,then don't update hhrVlanDatabaseTable_list*/
		time(&hhrVlanDatabaseTable_time_old);
	}

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrVlanDatabaseTable_time_old) > HHR_VLAN_DATABASE_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrVlanDatabaseTable_lock))
    {
        if (NULL == hhrVlanDatabaseTable_list)
        {
            hhrVlanDatabaseTable_list = list_new();
            hhrVlanDatabaseTable_list->del = (void (*)(void *))hhrVlanDatabaseTable_node_free;
        }

        if (0 != hhrVlanDatabaseTable_list->count)
        {
            snmp_vlan_node_del_all(HHR_VLAN_DATABASE_TABLE);
        }
    }

    /* then lock */
    hhrVlanDatabaseTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrVlanDatabaseTable_list)
    {
        hhrVlanDatabaseTable_list = list_new();
        hhrVlanDatabaseTable_list->del = (void (*)(void *))hhrVlanDatabaseTable_node_free;
    }

    if (0 == hhrVlanDatabaseTable_list->count)
    {
        index_temp = 0;
        ret = hhrVlanDatabaseTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pVlanDatabaseEntry = (struct vlan_info *)snmp_vlan_node_lookup(exact, HHR_VLAN_DATABASE_TABLE, index);

    if (NULL == pVlanDatabaseEntry) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVlanDatabaseTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct vlan_info *)hhrVlanDatabaseTable_list->tail->data;
            ret = hhrVlanDatabaseTable_get_data_from_ipc(entry_temp->vlanid);

            if (FALSE == ret)
            {
                hhrVlanDatabaseTable_lock = UNLOCK; //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pVlanDatabaseEntry = (struct vlan_info *)snmp_vlan_node_lookup(exact, HHR_VLAN_DATABASE_TABLE, index);

            if (NULL != pVlanDatabaseEntry)
            {
                *index_next = pVlanDatabaseEntry->vlanid;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrVlanDatabaseTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pVlanDatabaseEntry;
            }
        }
    }

    *index_next = pVlanDatabaseEntry->vlanid;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrVlanDatabaseTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pVlanDatabaseEntry;
}

static int hhrVlanListTable_get_data_by_index(u_int16_t v_start, u_int16_t v_end)
{
	struct vlan_info *entry_temp = NULL;
	u_int32_t index_temp;
	static time_t getNext_time_old; //to store the get-next operate time of the last time
	int ret = 0;
	time_t time_now = 0;
	time(&time_now);

	/* get-next timeout */
	if ((abs(time_now - getNext_time_old) > HHR_VLAN_DATABASE_TABLE_GET_NEXT_INTERVAL))
	{
		hhrVlanDatabaseTable_lock = UNLOCK;
	}
	else
	{
		time(&hhrVlanDatabaseTable_time_old);
	}

	/* if timeout && not lock, clear list */
	if ((abs(time_now - hhrVlanDatabaseTable_time_old) > HHR_VLAN_DATABASE_TABLE_REFRESH_TIME) && \
			(UNLOCK == hhrVlanDatabaseTable_lock))
	{
		if (NULL == hhrVlanDatabaseTable_list)
		{
			hhrVlanDatabaseTable_list = list_new();
			hhrVlanDatabaseTable_list->del = (void (*)(void *))hhrVlanDatabaseTable_node_free;
		}

		if (0 != hhrVlanDatabaseTable_list->count)
		{
			snmp_vlan_node_del_all(HHR_VLAN_DATABASE_TABLE);
		}
	}

	/* then lock */
	hhrVlanDatabaseTable_lock = LOCK;

	/* if list empty, get data by index(0,0,...) */
	if (NULL == hhrVlanDatabaseTable_list)
	{
		hhrVlanDatabaseTable_list = list_new();
		hhrVlanDatabaseTable_list->del = (void (*)(void *))hhrVlanDatabaseTable_node_free;
	}

	if (0 == hhrVlanDatabaseTable_list->count)
    {
        index_temp = 0;
        ret = hhrVlanDatabaseTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
        	hhrVlanDatabaseTable_lock = UNLOCK; //search over, unclock
            return 0;
        }
    }

	if (NULL != hhrVlanDatabaseTable_list->tail->data)
	{
        entry_temp = (struct vlan_info *)hhrVlanDatabaseTable_list->tail->data;

		while(v_end > entry_temp->vlanid )
		{
			ret = hhrVlanDatabaseTable_get_data_from_ipc(entry_temp->vlanid);
			if (FALSE == ret || NULL == hhrVlanDatabaseTable_list->tail->data)
			{
				break;
			}
			else 
			{
				entry_temp = (struct vlan_info *)hhrVlanDatabaseTable_list->tail->data;
			}
		}
		
	}

	hhrVlanDatabaseTable_lock = UNLOCK; //search over, unclock
	time(&getNext_time_old);
    return 0;

}


/* hhrIfVlanConfigTable*/

void hhrVlanConfigTable_node_free(struct l2if *node)
{
    XFREE(0, node);
}

int hhrVlanConfigTable_get_data_from_ipc(u_int32_t index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct l2if *pL2ifEntry = NULL;
    int data_num = 0;
    int ret = 0;

    pMsgRcv = snmp_get_switch_info_bulk(index, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        //  zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] snmp_get_switch_info_bulk data_num=0\n", __FUNCTION__, __LINE__);
        return FALSE;
    }
    else
    {
        //  zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] snmp_get_switch_info_bulk data_num=%d\n", __FUNCTION__, __LINE__,data_num);
        pL2ifEntry = (struct l2if *)pMsgRcv->msg_data;
        if(pL2ifEntry)
        {
            for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
            {
                snmp_vlan_node_add(HHR_VLAN_CINFIG_TABLE, pL2ifEntry);
                pL2ifEntry++;
            }
        }
        time(&hhrVlanConfigTable_time_old); //refresh time_old after refresh cache data
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct l2if *hhrVlanConfigTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct l2if *pL2ifEntry = NULL;
    struct l2if *l2if_temp = NULL;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_VLAN_CINFIG_TABLE_GET_NEXT_INTERVAL))
    {
        hhrVlanConfigTable_lock = UNLOCK;
    }
	else 
	{   /* if get-next doesn't timeout ,then don't update hhrVlanConfigTable_list*/
		time(&hhrVlanConfigTable_time_old);
	}

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrVlanConfigTable_time_old) > HHR_VLAN_CINFIG_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrVlanConfigTable_lock))
    {
        if (NULL == hhrVlanConfigTable_list)
        {
            hhrVlanConfigTable_list = list_new();
            hhrVlanConfigTable_list->del = (void (*)(void *))hhrVlanConfigTable_node_free;
        }

        if (0 != hhrVlanConfigTable_list->count)
        {
            snmp_vlan_node_del_all(HHR_VLAN_CINFIG_TABLE);
        }
    }

    /* then lock */
    hhrVlanConfigTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrVlanConfigTable_list)
    {
        hhrVlanConfigTable_list = list_new();
        hhrVlanConfigTable_list->del = (void (*)(void *))hhrVlanConfigTable_node_free;
    }

    if (0 == hhrVlanConfigTable_list->count)
    {
        index_temp = 0;
        ret = hhrVlanConfigTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pL2ifEntry = (struct l2if *)snmp_vlan_node_lookup(exact, HHR_VLAN_CINFIG_TABLE, index);

    if (NULL == pL2ifEntry) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVlanConfigTable_list->tail->data)
            {
                return NULL;
            }

            l2if_temp = (struct l2if *)hhrVlanConfigTable_list->tail->data;
            ret = hhrVlanConfigTable_get_data_from_ipc(l2if_temp->ifindex);

            if (FALSE == ret)
            {
                hhrVlanConfigTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pL2ifEntry = (struct l2if *)snmp_vlan_node_lookup(exact, HHR_VLAN_CINFIG_TABLE, index);

            if (NULL != pL2ifEntry)
            {
                *index_next = pL2ifEntry->ifindex;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrVlanConfigTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pL2ifEntry;
            }
        }
    }

    *index_next = pL2ifEntry->ifindex;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrVlanConfigTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pL2ifEntry;
}


/* hhrVlanMappingTable + hhrVlanMapping2LayerTable*/

void hhrVlanMappingTable_node_free(struct l2if_vlan_mapping *node)
{
    XFREE(0, node);
}

int hhrVlanMappingTable_get_data_from_ipc(struct l2if_vlan_mapping *index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct l2if_vlan_mapping *p_l2if_vlan_mapping = NULL;
    int                       data_num = 0;
    int                       ret = 0;
    int                       flag = 0;
    struct l2if_vlan_mapping  tmp_index;

    memcpy(&tmp_index, index, sizeof(struct l2if_vlan_mapping));

    while (1)
    {
        pMsgRcv = snmp_qinq_list_get_bulk(&tmp_index, MODULE_ID_SNMPD, &data_num);

        if (0 == data_num || NULL == pMsgRcv)
        {
            return FALSE;
        }
        else
        {
			p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)pMsgRcv->msg_data;
            if(p_l2if_vlan_mapping)
            {
                for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
                {
                    memcpy(&tmp_index, p_l2if_vlan_mapping, sizeof(struct l2if_vlan_mapping));

                    if (0 == p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_start)
                    {
                        snmp_vlan_node_add(HHR_VLANMAPPING_TABLE, p_l2if_vlan_mapping);
                        flag = 1;
                    }

                    p_l2if_vlan_mapping++;
                }
            }

			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

            if (1 == flag)  //Returns subinterface
            {
                time(&hhrVlanMappingTable_time_old);    //refresh time_old after refresh cache data
                return TRUE;
            }
        }
    }

    return FALSE;
}

struct l2if_vlan_mapping *hhrVlanMappingTable_get_data_by_index(int exact,
                                                                struct l2if_vlan_mapping  *index, struct l2if_vlan_mapping  *index_next)
{
    struct l2if_vlan_mapping *p_l2if_vlan_mapping = NULL;
    struct l2if_vlan_mapping *pdata_temp = NULL;
    struct l2if_vlan_mapping entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_VLAN_MAPPING_TABLE_GET_NEXT_INTERVAL))
    {
        hhrVlanMappingTable_lock = UNLOCK;
    }
	else 
	{   /* if get-next doesn't timeout ,then don't update hhrVlanMappingTable_list*/
		time(&hhrVlanMappingTable_time_old);
	}

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrVlanMappingTable_time_old) > HHR_VLAN_MAPPING_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrVlanMappingTable_lock))
    {
        if (NULL == hhrVlanMappingTable_list)
        {
            hhrVlanMappingTable_list = list_new();
            hhrVlanMappingTable_list->del = (void (*)(void *))hhrVlanMappingTable_node_free;
        }

        if (0 != hhrVlanMappingTable_list->count)
        {
            snmp_vlan_node_del_all(HHR_VLANMAPPING_TABLE);
        }
    }

    /* then lock */
    hhrVlanMappingTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrVlanMappingTable_list)
    {
        hhrVlanMappingTable_list = list_new();
        hhrVlanMappingTable_list->del = (void (*)(void *))hhrVlanMappingTable_node_free;
    }

    if (0 == hhrVlanMappingTable_list->count)
    {
        memset(&entry_temp, 0, sizeof(struct l2if_vlan_mapping));
        ret = hhrVlanMappingTable_get_data_from_ipc(&entry_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)snmp_vlan_node_lookup(exact, HHR_VLANMAPPING_TABLE, index);

    if (NULL == p_l2if_vlan_mapping) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVlanMappingTable_list->tail->data)
            {
                return NULL;
            }

            pdata_temp = (struct l2if_vlan_mapping *)hhrVlanMappingTable_list->tail->data;
            ret = hhrVlanMappingTable_get_data_from_ipc(pdata_temp);

            if (FALSE == ret)
            {
                hhrVlanMappingTable_lock = UNLOCK;  //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)snmp_vlan_node_lookup(exact, HHR_VLANMAPPING_TABLE, index);

            if (NULL != p_l2if_vlan_mapping)
            {
                index_next->ifindex = p_l2if_vlan_mapping->ifindex;
                index_next->t_vlan_mapping.svlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_start;
                index_next->t_vlan_mapping.svlan.vlan_end = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_end;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrVlanMappingTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return p_l2if_vlan_mapping;
            }
        }
    }

    index_next->ifindex = p_l2if_vlan_mapping->ifindex;
    index_next->t_vlan_mapping.svlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_start;
    index_next->t_vlan_mapping.svlan.vlan_end = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_end;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrVlanMappingTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return p_l2if_vlan_mapping;
}

void hhrVlanMapping2LayerTable_node_free(struct l2if_vlan_mapping *node)
{
    XFREE(0, node);
}

int hhrVlanMapping2LayerTable_get_data_from_ipc(struct l2if_vlan_mapping *index)
{
    struct l2if_vlan_mapping *p_l2if_vlan_mapping = NULL;
    int                       data_num = 0;
    int                       ret = 0;
    int                       flag = 0;
    struct l2if_vlan_mapping  tmp_index;
	struct ipc_mesg_n *pMsgRcv = NULL;

    memcpy(&tmp_index, index, sizeof(struct l2if_vlan_mapping));

    while (1)
    {
        pMsgRcv = snmp_qinq_list_get_bulk(&tmp_index, MODULE_ID_SNMPD, &data_num);

        if (0 == data_num || NULL == pMsgRcv)
        {
            return FALSE;
        }
        else
        {
			p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)pMsgRcv->msg_data;
            if(p_l2if_vlan_mapping)
            {
                for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
                {
                    memcpy(&tmp_index, p_l2if_vlan_mapping, sizeof(struct l2if_vlan_mapping));

                    if (0 != p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_start)
                    {
                        snmp_vlan_node_add(HHR_VLANMAPPING2LAYER_TABLE, p_l2if_vlan_mapping);
                        flag = 1;
                    }

                    p_l2if_vlan_mapping++;
                }
            }
            
			mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

            if (1 == flag)  //Returns subinterface
            {
                time(&hhrVlanMapping2LayerTable_time_old);  //refresh time_old after refresh cache data
                return TRUE;
            }
        }
    }

    return FALSE;
}

struct l2if_vlan_mapping *hhrVlanMapping2LayerTable_get_data_by_index(int exact,
                                                                      struct l2if_vlan_mapping *index, struct l2if_vlan_mapping *index_next)
{
    struct l2if_vlan_mapping *p_l2if_vlan_mapping = NULL;
    struct l2if_vlan_mapping *pdata_temp = NULL;
    struct l2if_vlan_mapping entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_VLAN_MAPPING2LAYER_TABLE_GET_NEXT_INTERVAL))
    {
        hhrVlanMapping2LayerTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrVlanMapping2LayerTable_time_old) > HHR_VLAN_MAPPING2LAYER_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrVlanMapping2LayerTable_lock))
    {
        if (NULL == hhrVlanMapping2LayerTable_list)
        {
            hhrVlanMapping2LayerTable_list = list_new();
            hhrVlanMapping2LayerTable_list->del = (void (*)(void *))hhrVlanMapping2LayerTable_node_free;
        }

        if (0 != hhrVlanMapping2LayerTable_list->count)
        {
            snmp_vlan_node_del_all(HHR_VLANMAPPING2LAYER_TABLE);
        }
    }

    /* then lock */
    hhrVlanMapping2LayerTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrVlanMapping2LayerTable_list)
    {
        hhrVlanMapping2LayerTable_list = list_new();
        hhrVlanMapping2LayerTable_list->del = (void (*)(void *))hhrVlanMapping2LayerTable_node_free;
    }

    if (0 == hhrVlanMapping2LayerTable_list->count)
    {
        memset(&entry_temp, 0, sizeof(struct l2if_vlan_mapping));
        ret = hhrVlanMapping2LayerTable_get_data_from_ipc(&entry_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)snmp_vlan_node_lookup(exact, HHR_VLANMAPPING2LAYER_TABLE, index);

    if (NULL == p_l2if_vlan_mapping) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVlanMapping2LayerTable_list->tail->data)
            {
                return NULL;
            }

            pdata_temp = (struct l2if_vlan_mapping *)hhrVlanMapping2LayerTable_list->tail->data;
            ret = hhrVlanMapping2LayerTable_get_data_from_ipc(pdata_temp);

            if (FALSE == ret)
            {
                hhrVlanMapping2LayerTable_lock = UNLOCK;    //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            p_l2if_vlan_mapping = (struct l2if_vlan_mapping *)snmp_vlan_node_lookup(exact, HHR_VLANMAPPING2LAYER_TABLE, index);

            if (NULL != p_l2if_vlan_mapping)
            {
                index_next->ifindex = p_l2if_vlan_mapping->ifindex;
                index_next->t_vlan_mapping.svlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_start;
                index_next->t_vlan_mapping.cvlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_start;
                index_next->t_vlan_mapping.cvlan.vlan_end = p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_end;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrVlanMapping2LayerTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return p_l2if_vlan_mapping;
            }
        }
    }

    index_next->ifindex = p_l2if_vlan_mapping->ifindex;
    index_next->t_vlan_mapping.svlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.svlan.vlan_start;
    index_next->t_vlan_mapping.cvlan.vlan_start = p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_start;
    index_next->t_vlan_mapping.cvlan.vlan_end = p_l2if_vlan_mapping->t_vlan_mapping.cvlan.vlan_end;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrVlanMapping2LayerTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return p_l2if_vlan_mapping;
}

/******************** list operate functions  ****************/
static void *snmp_vlan_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case HHR_VLAN_DATABASE_TABLE:
            return XCALLOC(0, sizeof(struct vlan_info));
            break;

        case HHR_VLAN_CINFIG_TABLE:
            return XCALLOC(0, sizeof(struct l2if));
            break;

        case HHR_VLANMAPPING_TABLE:
            return XCALLOC(0, sizeof(struct l2if_vlan_mapping));
            break;

        case HHR_VLANMAPPING2LAYER_TABLE:
            return XCALLOC(0, sizeof(struct l2if_vlan_mapping));
            break;

        default:
            break;
    }

    return NULL;
}

static void snmp_vlan_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case HHR_VLAN_DATABASE_TABLE:
            node_add = snmp_vlan_node_xcalloc(HHR_VLAN_DATABASE_TABLE);
            memcpy(node_add, node, sizeof(struct vlan_info));
            listnode_add(hhrVlanDatabaseTable_list, node_add);
            break;

        case HHR_VLAN_CINFIG_TABLE:
            node_add = snmp_vlan_node_xcalloc(HHR_VLAN_CINFIG_TABLE);
            memcpy(node_add, node, sizeof(struct l2if));
            listnode_add(hhrVlanConfigTable_list, node_add);
            break;

        case HHR_VLANMAPPING_TABLE:
            node_add = snmp_vlan_node_xcalloc(HHR_VLANMAPPING_TABLE);
            memcpy(node_add, node, sizeof(struct l2if_vlan_mapping));
            listnode_add(hhrVlanMappingTable_list, node_add);
            break;

        case HHR_VLANMAPPING2LAYER_TABLE:
            node_add = snmp_vlan_node_xcalloc(HHR_VLANMAPPING2LAYER_TABLE);
            memcpy(node_add, node, sizeof(struct l2if_vlan_mapping));
            listnode_add(hhrVlanMapping2LayerTable_list, node_add);
            break ;

        default:
            break;
    }
}

static void *snmp_vlan_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    /* vlan database table */
    struct vlan_info    *data1_find;
    u_int32_t           index1;

    /* vlan config table */
    struct l2if     *data2_find;
    u_int32_t       index2;


    /* VlanMapping table */
    struct l2if_vlan_mapping *data3_find;
    struct l2if_vlan_mapping *index3;

    /* VlanMapping2Layer table */
    struct l2if_vlan_mapping *data4_find;
    struct l2if_vlan_mapping *index4;



    switch (table_num)
    {
        case HHR_VLAN_DATABASE_TABLE:
            index1 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrVlanDatabaseTable_list, node, nnode, data1_find))
            {
                if (0 == index1)
                {
                    return hhrVlanDatabaseTable_list->head->data;
                }

                if (index1 == data1_find->vlanid)
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

        case HHR_VLAN_CINFIG_TABLE:
            index2 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrVlanConfigTable_list, node, nnode, data2_find))
            {
                if (0 == index2)
                {
                    return hhrVlanConfigTable_list->head->data;
                }

                if (index2 == data2_find->ifindex)
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

        case HHR_VLANMAPPING_TABLE:
            index3 = (struct l2if_vlan_mapping *)index_input;

            for (ALL_LIST_ELEMENTS(hhrVlanMappingTable_list, node, nnode, data3_find))
            {
                if (0 == index3->ifindex)
                {
                    return hhrVlanMappingTable_list->head->data;
                }

                if ((index3->ifindex == data3_find->ifindex) && \
                        (index3->t_vlan_mapping.svlan.vlan_start == data3_find->t_vlan_mapping.svlan.vlan_start) && \
                        (index3->t_vlan_mapping.svlan.vlan_end == data3_find->t_vlan_mapping.svlan.vlan_end) && \
                        (index3->t_vlan_mapping.cvlan.vlan_start == data3_find->t_vlan_mapping.cvlan.vlan_start) && \
                        (index3->t_vlan_mapping.cvlan.vlan_end == data3_find->t_vlan_mapping.cvlan.vlan_end))
                {
                    if (1 == exact) //get
                    {
                        return data3_find;
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

        case HHR_VLANMAPPING2LAYER_TABLE:
            index4 = (struct l2if_vlan_mapping *)index_input;

            for (ALL_LIST_ELEMENTS(hhrVlanMapping2LayerTable_list, node, nnode, data4_find))
            {
                if (0 == index4->ifindex)
                {
                    return hhrVlanMapping2LayerTable_list->head->data;
                }

                if ((index4->ifindex == data4_find->ifindex) && \
                        (index4->t_vlan_mapping.svlan.vlan_start == data4_find->t_vlan_mapping.svlan.vlan_start) && \
                        (index4->t_vlan_mapping.svlan.vlan_end == data4_find->t_vlan_mapping.svlan.vlan_end) && \
                        (index4->t_vlan_mapping.cvlan.vlan_start == data4_find->t_vlan_mapping.cvlan.vlan_start) && \
                        (index4->t_vlan_mapping.cvlan.vlan_end == data4_find->t_vlan_mapping.cvlan.vlan_end))
                {
                    if (1 == exact) //get
                    {
                        return data4_find;
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

static void snmp_vlan_node_del_all(int table_num)
{
    switch (table_num)
    {
        case HHR_VLAN_DATABASE_TABLE:
            list_delete_all_node(hhrVlanDatabaseTable_list);
            break;

        case HHR_VLAN_CINFIG_TABLE:
            list_delete_all_node(hhrVlanConfigTable_list);
            break;

        case HHR_VLANMAPPING_TABLE:
            list_delete_all_node(hhrVlanMappingTable_list);
            break;

        case HHR_VLANMAPPING2LAYER_TABLE:
            list_delete_all_node(hhrVlanMapping2LayerTable_list);
            break;

        default:
            break;
    }
}

static void snmp_vlan_list_init(void)
{
    hhrVlanDatabaseTable_list = list_new();
    hhrVlanDatabaseTable_list->del = (void (*)(void *))hhrVlanDatabaseTable_node_free;

    hhrVlanConfigTable_list = list_new();
    hhrVlanConfigTable_list->del = (void (*)(void *))hhrVlanConfigTable_node_free;

    hhrVlanMappingTable_list = list_new();
    hhrVlanMappingTable_list->del = (void (*)(void *))hhrVlanMappingTable_node_free;

    hhrVlanMapping2LayerTable_list = list_new();
    hhrVlanMapping2LayerTable_list->del = (void (*)(void *))hhrVlanMapping2LayerTable_node_free;
}

/************************ get functions *****************************/
int snmp_vlan_base_switch_vlan_get(uint32_t ifindex)
{
    u_char *pdata;

    memset(str_value, 0, sizeof(str_value));
    struct ipc_mesg_n* pMsgRcv = snmp_get_switch_vlan(ifindex, MODULE_ID_SNMPD);

	if(pMsgRcv)
	{
		pdata = (u_char *)pMsgRcv->msg_data;
		if(pdata)
		{
        	memcpy(str_value, pdata, sizeof(pMsgRcv->msghdr.data_len));
		}
		
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
    }

    return 0;
}

int snmp_vlan_base_switch_vlan_tag_get(uint32_t ifindex)
{
    u_char *pdata;

    memset(str_value, 0, sizeof(str_value));
    struct ipc_mesg_n* pMsgRcv = snmp_get_switch_vlan_tag(ifindex, MODULE_ID_SNMPD);

	if(pMsgRcv)
	{
		pdata = (u_char *)pMsgRcv->msg_data;
		if(pdata)
		{
        	memcpy(str_value, pdata, sizeof(pMsgRcv->msghdr.data_len));
		}
		
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
    }
	
    return 0;
}


u_char *
hhrVlanDatabaseTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vlan_info   *pVlanDatabaseEntry = NULL;
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;

//   zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter hhrBaseIfTable_get exact %d\n", __FUNCTION__, __LINE__, exact);
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* lookup */
    pVlanDatabaseEntry = hhrVlanDatabaseTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pVlanDatabaseEntry)
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
        case hhrVlanMacLearn:
            *var_len = sizeof(int);
            int_value = (DISABLE == pVlanDatabaseEntry->mac_learn) ? 2 : 1;
            return (u_char *)&int_value;

        case hhrVlanMacLearnLimit:
            *var_len = sizeof(int);
            int_value = pVlanDatabaseEntry->limit_num;
            return (u_char *)&int_value;

        case hhrVlanStormSupressUnicast:
            *var_len = sizeof(int);
            int_value = (DISABLE == pVlanDatabaseEntry->storm_unicast) ? 2 : 1;
            return (u_char *)&int_value;

        case hhrVlanStormSupressBroadcast:
            *var_len = sizeof(int);
            int_value = (DISABLE == pVlanDatabaseEntry->storm_broadcast) ? 2 : 1;
            return (u_char *)&int_value;

        case hhrVlanStormSupressMulticast:
            *var_len = sizeof(int);
            int_value = (DISABLE == pVlanDatabaseEntry->storm_multicast) ? 2 : 1;
            return (u_char *)&int_value;

        case hhrVlanMacLearnAction:
            *var_len = sizeof(int);
            int_value = (pVlanDatabaseEntry->limit_action) ? 2 : 1;
            return (u_char *)&int_value;

        default:
            return (NULL);
    }
}


u_char *
hhrIfVlanConfigTable_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2if   *pL2ifEntry = NULL;
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;

//    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter hhrBaseIfTable_get exact %d\n", __FUNCTION__, __LINE__, exact);
    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pL2ifEntry = hhrVlanConfigTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pL2ifEntry)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case hhrVlanConfigIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(pL2ifEntry->ifindex, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrVlanConfigVlanMode:
            *var_len = sizeof(int);

            if (pL2ifEntry->switch_info.mode == SWITCH_MODE_ACCESS)
            {
                int_value = 1;
            }
            else if (pL2ifEntry->switch_info.mode == SWITCH_MODE_TRUNK)
            {
                int_value = 2;
            }
            else if (pL2ifEntry->switch_info.mode == SWITCH_MODE_HYBRID)
            {
                int_value = 3;
            }

            return (u_char *)&int_value;

        case hhrVlanConfigPVID:
            *var_len = sizeof(int);
            int_value = pL2ifEntry->switch_info.access_vlan;
            return (u_char *)&int_value;

        case hhrVlanConfigVlanMember:
            snmp_vlan_base_switch_vlan_get(index_next);
            *var_len = BITS_STRING_LEN;
            return str_value;

        case hhrVlanConfigunTagVlan:
            snmp_vlan_base_switch_vlan_tag_get(index_next);
            *var_len = BITS_STRING_LEN;
            return str_value;

        default:
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}

/*vlan范围字符串转为整型，例如str[]="12 to 34",v_start=12,v_end=34*/
int vlan_str_range_to_int_range(char *str, uint16_t *v_start, uint16_t *v_end)
{
    char *pdata = NULL;
    int num1, num2;

    pdata = strstr(str, "to");

    if (pdata == NULL)
    {
        *v_start = *v_end = atoi(str);
    }
    else
    {
        sscanf(str, "%d to %d", &num1, &num2);
        *v_start = num1;
        *v_end = num2;
    }

    return 0;
}

/*vlan范围整型转为字符串，例如v_start=12,v_end=34,str[]="12 to 34"*/
int vlan_int_range_to_str_range(char *str, uint16_t *v_start, uint16_t *v_end)
{
    if (*v_start == *v_end)
    {
        sprintf(str, "%d", *v_start);
    }
    else
    {
        sprintf(str, "%d to %d", *v_start, *v_end);
    }

    return 0;
}

u_char *
hhrVlanMappingTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2if_vlan_mapping *pVlanMappingEntry = NULL;
    struct l2if_vlan_mapping index, index_next;
    char index2[13];
    char index2_next[13];
    int index2_len;
    int ret = 0;
    int temp1, temp2;
    int max_len = sizeof(index2);

//    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter hhrBaseIfTable_get exact %d\n", __FUNCTION__, __LINE__, exact);

    memset(&index, 0, sizeof(struct l2if_vlan_mapping));
    memset(&index_next, 0, sizeof(struct l2if_vlan_mapping));
    index2_len = sizeof(index2);
    /* validate the index */
    ret = ipran_snmp_int_str_index_get(vp, name, length, &index.ifindex, index2, max_len, &index2_len, exact);

    if (ret < 0)
    {
        return NULL;
    }

    vlan_str_range_to_int_range(index2, &index.t_vlan_mapping.svlan.vlan_start, &index.t_vlan_mapping.svlan.vlan_end);

    pVlanMappingEntry = hhrVlanMappingTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pVlanMappingEntry)
    {
        return NULL;
    }

    vlan_int_range_to_str_range(index2_next, &index_next.t_vlan_mapping.svlan.vlan_start, &index_next.t_vlan_mapping.svlan.vlan_end);
    index2_len = strlen(index2_next);

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_str_index_set(vp, name, length, index_next.ifindex, index2_next, index2_len);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case hhrVlanMappingIfDesrc:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(index_next.ifindex, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrVlanMappingAction:
            int_value = 0;
            temp1 = pVlanMappingEntry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMappingEntry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_ADD))
            {
                int_value = 3;
            }
            else if ((temp1 == VLAN_ACTION_ADD) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 2;
            }
            else if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 1;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrVlanMappingSvlan:
            temp1 = pVlanMappingEntry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMappingEntry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 0;
            }
            else
            {
                int_value = pVlanMappingEntry->t_vlan_mapping.svlan_new.vlan_start;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrVlanMappingSvlanCos:
            int_value = (int)pVlanMappingEntry->t_vlan_mapping.svlan_cos;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrVlanMappingCvlan:
            temp1 = pVlanMappingEntry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMappingEntry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_ADD) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 0;
            }
            else if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = pVlanMappingEntry->t_vlan_mapping.svlan_new.vlan_start;
            }
            else
            {
                int_value = pVlanMappingEntry->t_vlan_mapping.cvlan_new.vlan_start;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}


u_char *
hhrVlanMapping2LayerTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2if_vlan_mapping *pVlanMapping2Entry = NULL;
    struct l2if_vlan_mapping index, index_next;
    char index3[13];
    char index3_next[13];
    int index3_len;
    u_int32_t svlanid;
    int ret = 0;
    int temp1, temp2;
    int index_length = 0 ;
//    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter hhrBaseIfTable_get exact %d\n", __FUNCTION__, __LINE__, exact);

    memset(&index, 0, sizeof(struct l2if_vlan_mapping));
    memset(&index_next, 0, sizeof(struct l2if_vlan_mapping));
    index3_len = sizeof(index3);

    /* validate the index */
    ret = ipran_snmp_intx2_str_index_get(vp, name, length, &index.ifindex, &svlanid, index3, index3_len,
                                         &index_length , exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.t_vlan_mapping.svlan.vlan_start = index.t_vlan_mapping.svlan.vlan_end = svlanid;
    vlan_str_range_to_int_range(index3, &index.t_vlan_mapping.cvlan.vlan_start, &index.t_vlan_mapping.cvlan.vlan_end);

    pVlanMapping2Entry = hhrVlanMapping2LayerTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pVlanMapping2Entry)
    {
        return NULL;
    }

    vlan_int_range_to_str_range(index3_next, &index_next.t_vlan_mapping.cvlan.vlan_start, &index_next.t_vlan_mapping.cvlan.vlan_end);
    index3_len = strlen(index3_next);

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_intx2_str_index_set(vp, name, length, index_next.ifindex,
                                       (u_int32_t)index_next.t_vlan_mapping.svlan.vlan_start, index3_next, index3_len);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case HhrVlanMapping2LayerIfDesrc:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(index_next.ifindex, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case HhrVlanMapping2LayerAction:
            int_value = 0;
            temp1 = pVlanMapping2Entry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMapping2Entry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_DELETE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 3;
            }
            else if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 2;
            }
            else if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_TRANSLATE))
            {
                int_value = 1;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case HhrVlanMapping2LayerSvlan:
            temp1 = pVlanMapping2Entry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMapping2Entry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_DELETE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 0;
            }
            else
            {
                int_value = pVlanMapping2Entry->t_vlan_mapping.svlan_new.vlan_start;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case HhrVlanMapping2LayerSvlanCos:
            int_value = (int)pVlanMapping2Entry->t_vlan_mapping.svlan_cos;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case HhrVlanMapping2LayerCvlan:
            temp1 = pVlanMapping2Entry->t_vlan_mapping.svlan_act;
            temp2 = pVlanMapping2Entry->t_vlan_mapping.cvlan_act;

            if ((temp1 == VLAN_ACTION_TRANSLATE) && (temp2 == VLAN_ACTION_NO))
            {
                int_value = 0;
            }
            else
            {
                int_value = pVlanMapping2Entry->t_vlan_mapping.cvlan_new.vlan_start;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}

int snmp_vlan_List_table_get(u_int8_t num, unsigned char *pvlan_buff, int vlan_buff_length)
{
	struct vlan_info	 *pVlanDatabaseEntry = NULL;
	u_char vlan_list_buff[BITS_STRING_LEN_VLAN_LIST];
	uint16_t v_start;
	uint16_t v_end;
	u_int32_t index_next = 0;
	int pos = 0;
	int bit_num = 0;
	int ret = 0;
	int exact_flag = 0;
	struct listnode  *node = NULL;
	struct listnode *nnode = NULL;

	if (NULL == pvlan_buff || vlan_buff_length < BITS_STRING_LEN_VLAN_LIST)
	{
	  return 0;
	}

	v_start = (num - 1) * BITS_STRING_LEN_VLAN_LIST * 8 + 1;
	v_end = num * BITS_STRING_LEN_VLAN_LIST * 8;
	memset(vlan_list_buff, 0, sizeof(vlan_list_buff));

	ret = hhrVlanListTable_get_data_by_index(v_start, v_end);
    for (ALL_LIST_ELEMENTS(hhrVlanDatabaseTable_list, node, nnode, pVlanDatabaseEntry))
    {
		  if(pVlanDatabaseEntry == NULL)
		  {
			  break;
		  }

		  index_next = pVlanDatabaseEntry->vlanid;
		  if ((index_next >= v_start) && (index_next <= v_end))
		  {
			  bit_num = pVlanDatabaseEntry->vlanid - v_start;
			  pos = 0x01FF & (bit_num / 8);
			  vlan_list_buff[pos] = vlan_list_buff[pos] | (0x80 >> (bit_num % 8));
		  }  
    }

	memcpy(pvlan_buff, vlan_list_buff, BITS_STRING_LEN_VLAN_LIST);

	return 1;
}


u_char *
hhrIfVlanListForIpranTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrl2VlanListForIpran1:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(1, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran2:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(2, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran3:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(3, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran4:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(4, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran5:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(5, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran6:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(6, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran7:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(7, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        case hhrl2VlanListForIpran8:
            memset(str_value, 0, STRING_LEN);
            snmp_vlan_List_table_get(8, str_value, STRING_LEN);
            *var_len = BITS_STRING_LEN_VLAN_LIST;
            return str_value;

        default:
            return (NULL);
    }
}


