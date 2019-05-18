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
#include <time.h>
#include <stdlib.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"
#include "ipran_snmp_data_cache.h"
#include <lib/log.h>

#include <lib/msg_ipc.h>
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
#include <l2/mac_static.h>
#include <l2/l2_if.h>

#include "mib_vlan.h"
#include "mib_l2func.h"

#include "snmp_config_table.h"
/*local temp variable*/
static uchar              str_value[STRING_LEN] = {'\0'};
static int                int_value = 0;
static uint32_t           uint_value = 0;

/* table list define */
struct list *hhrMacAddressTable_list  = NULL;

/* variable to store refresh data time of the last time */
static time_t  hhrMacAddressTable_time_old  = 0;

/* lock of tables */
enum table_lock
{
    UNLOCK,
    LOCK,
};
static enum table_lock hhrMacAddressTable_lock = UNLOCK;

/*
 * Object ID definitions
 */
/*hhrMacConfigTable*/
static oid hhrMacConfig_variables_oid[] = {HHRLAVER2FUNCTION};
FindVarMethod hhrMacConfig_get;
struct variable1 hhrMacConfig_variables[] =
{
    {hhrMacAgeingTime,  ASN_INTEGER,  RONLY, hhrMacConfig_get, 1, {1}},
    {hhrMacMoveEnable,  ASN_INTEGER,  RONLY, hhrMacConfig_get, 1, {2}},
    {hhrMacLearnLimit,  ASN_UNSIGNED, RONLY, hhrMacConfig_get, 1, {3}},
    {hhrMacLearnAction, ASN_INTEGER,  RONLY, hhrMacConfig_get, 1, {4}}
};

/*hhrMacAddressTable*/
static oid hhrMacAddressTable_oid[] = {HHRLAVER2FUNCTION, 11};
FindVarMethod hhrMacAddressTable_get;
struct variable2 hhrMacAddressTable_variables[] =
{
    //{hhrMacVSI,       ASN_INTEGER,   RONLY, hhrMacAddressTable_get, 2, {1, 3}},
    {hhrMaclfDescr,   ASN_OCTET_STR, RONLY, hhrMacAddressTable_get, 2, {1, 4}},
    {hhrMacType,      ASN_INTEGER,   RONLY, hhrMacAddressTable_get, 2, {1, 5}}
};

/*hhrMacIfLimitTable*/
static oid hhrMacIfLimitTable_oid[] = {HHRLAVER2FUNCTION, 12};
FindVarMethod hhrMacIfLimitTable_get;
struct ipran_snmp_data_cache *hhrMacIfLimitTable_cache = NULL;
struct variable2 hhrMacIfLimitTable_variables[] =
{
    {hhrIfMacLimitifDescr, ASN_OCTET_STR, RONLY, hhrMacIfLimitTable_get, 2, {1, 1}},
    {hhrIfMacLearnLimit,   ASN_UNSIGNED,  RONLY, hhrMacIfLimitTable_get, 2, {1, 2}},
    {hhrIfMacLearnAction,  ASN_INTEGER,   RONLY, hhrMacIfLimitTable_get, 2, {1, 3}}
};
    
/*hhrDynamicMacAddressTable*/
static oid hhrDynamicMacAddressTable_oid[] = {HHRLAVER2FUNCTION, 15};
FindVarMethod hhrDynamicMacAddressTable_get;
struct ipran_snmp_data_cache *hhrDynamicMacAddressTable_cache = NULL;
struct variable2 hhrDynamicMacAddressTable_variables[] =
{
    {hhrDynamicMaclfDescr,   ASN_OCTET_STR, RONLY, hhrDynamicMacAddressTable_get, 2, {1, 3}}
};

void init_mib_l2func(void)
{
    REGISTER_MIB("hhrMacConfig", hhrMacConfig_variables, variable1, hhrMacConfig_variables_oid);
    REGISTER_MIB("hhrMacAddressTable", hhrMacAddressTable_variables, variable2, hhrMacAddressTable_oid);
    REGISTER_MIB("hhrMacIfLimitTable", hhrMacIfLimitTable_variables, variable2, hhrMacIfLimitTable_oid);
    REGISTER_MIB("hhrDynamicMacAddressTable", hhrDynamicMacAddressTable_variables, variable2, hhrDynamicMacAddressTable_oid);

    snmp_mac_list_init();
}

/* get mac age time */
struct ipc_mesg_n *snmp_get_mac_config(int module_id)
{
    return ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2, module_id,
                     IPC_TYPE_MAC, MAC_INFO_MAC_CONFIG, IPC_OPCODE_GET, 0, 5000);
}

struct ipc_mesg_n *snmp_static_mac_get_bulk(int module_id, int *pdata_num, struct mac_key *key_entry)
{
    return ipc_sync_send_n2(key_entry, sizeof(struct mac_key), 1 ,
                     MODULE_ID_L2, module_id, IPC_TYPE_MAC, MAC_INFO_MAC, IPC_OPCODE_GET_BULK, 0, 5000);
}

/******************** list operate functions  ****************/
static void *snmp_mac_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case HHR_MAC_ADDRESS_TABLE:
            return XCALLOC(0, sizeof(struct mac_entry));
            break;

        default:
            break;
    }

    return NULL;
}

static void snmp_mac_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case HHR_MAC_ADDRESS_TABLE:
            node_add = snmp_mac_node_xcalloc(HHR_MAC_ADDRESS_TABLE);
            memcpy(node_add, node, sizeof(struct mac_entry));
            listnode_add(hhrMacAddressTable_list, node_add);
            break;

        default:
            break;
    }
}

static void *snmp_mac_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    /* mac address table */
    struct mac_entry    *data_find;
    struct mac_key      *index;

    switch (table_num)
    {
        case HHR_MAC_ADDRESS_TABLE:
            index = (struct mac_key *)index_input;

            for (ALL_LIST_ELEMENTS(hhrMacAddressTable_list, node, nnode, data_find))
            {
                if (0 == index->vlanid)
                {
                    return hhrMacAddressTable_list->head->data;
                }

                if ((index->vlanid == data_find->key.vlanid)\
                        && (0 == memcmp(index->mac, data_find->key.mac, MAC_LEN)))
                {
                    if (1 == exact) //get
                    {
                        return data_find;
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

    return (NULL);
}

static void snmp_mac_node_del_all(int table_num)
{
    switch (table_num)
    {
        case HHR_MAC_ADDRESS_TABLE:
            list_delete_all_node(hhrMacAddressTable_list);
            break;

        default:
            break;
    }
}

static void snmp_mac_list_init(void)
{
    hhrMacAddressTable_list = list_new();
    hhrMacAddressTable_list->del = (void (*)(void *))hhrMacAddressTable_node_free;
}


/* hhrCE1Table + hhrCE1SubIfTable*/

void hhrMacAddressTable_node_free(struct mac_entry *node)
{
    XFREE(0, node);
}

int hhrMacAddressTable_get_data_from_ipc(struct mac_key index)
{
    struct mac_entry *pMacAddressEntry = NULL;
    int data_num = 0;
    int ret = 0;

    //pMacAddressEntry = snmp_static_mac_get_bulk(MODULE_ID_SNMPD, &data_num, &index);
    struct ipc_mesg_n *pMsgRcv = snmp_static_mac_get_bulk(MODULE_ID_SNMPD, &data_num, &index);
    if(pMsgRcv == NULL)
    {
        return FALSE;
    }
    
    data_num = pMsgRcv->msghdr.data_num;
    pMacAddressEntry = (struct mac_entry *) pMsgRcv->msg_data;

    if (data_num == 0 || pMacAddressEntry == NULL)
    {
        //  zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] snmp_static_mac_get_bulk data_num=0\n", __FUNCTION__, __LINE__);
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        //  zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] snmp_static_mac_get_bulk data_num=%d\n", __FUNCTION__, __LINE__,data_num);
        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            snmp_mac_node_add(HHR_MAC_ADDRESS_TABLE, pMacAddressEntry);
            pMacAddressEntry++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        time(&hhrMacAddressTable_time_old); //refresh time_old after refresh cache data
        return TRUE;
    }
}

struct mac_entry *hhrMacAddressTable_get_data_by_index(int exact, struct mac_key *index, struct mac_key *index_next)
{
    struct mac_entry *pMacAddressEntry = NULL;
    struct mac_key key_entry_temp;
    struct mac_entry *mac_entry_temp = NULL;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_MAC_ADDRESS_TABLE_GET_NEXT_INTERVAL))
    {
        hhrMacAddressTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrMacAddressTable_time_old) > HHR_MAC_ADDRESS_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrMacAddressTable_lock))
    {
        if (NULL == hhrMacAddressTable_list)
        {
            hhrMacAddressTable_list = list_new();
            hhrMacAddressTable_list->del = (void (*)(void *))hhrMacAddressTable_node_free;
        }

        if (0 != hhrMacAddressTable_list->count)
        {
            snmp_mac_node_del_all(HHR_MAC_ADDRESS_TABLE);
        }
    }

    /* then lock */
    hhrMacAddressTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrMacAddressTable_list)
    {
        hhrMacAddressTable_list = list_new();
        hhrMacAddressTable_list->del = (void (*)(void *))hhrMacAddressTable_node_free;
    }

    if (0 == hhrMacAddressTable_list->count)
    {
        memset(&key_entry_temp, 0, sizeof(struct mac_key));
        ret = hhrMacAddressTable_get_data_from_ipc(key_entry_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pMacAddressEntry = (struct mac_entry *)snmp_mac_node_lookup(exact, HHR_MAC_ADDRESS_TABLE, index);

    if (NULL == pMacAddressEntry)   //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrMacAddressTable_list->tail->data)
            {
                return NULL;
            }

            mac_entry_temp = (struct mac_entry *)hhrMacAddressTable_list->tail->data;
            ret = hhrMacAddressTable_get_data_from_ipc(mac_entry_temp->key);

            if (FALSE == ret)
            {
                hhrMacAddressTable_lock = UNLOCK;   //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pMacAddressEntry = (struct mac_entry *)snmp_mac_node_lookup(exact, HHR_MAC_ADDRESS_TABLE, index);

            if (NULL != pMacAddressEntry)
            {
                index_next->vlanid = pMacAddressEntry->key.vlanid;
                memcpy(index_next->mac, pMacAddressEntry->key.mac, sizeof(pMacAddressEntry->key.mac));

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrMacAddressTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pMacAddressEntry;
            }
        }
    }

    index_next->vlanid = pMacAddressEntry->key.vlanid;
    memcpy(index_next->mac, pMacAddressEntry->key.mac, sizeof(pMacAddressEntry->key.mac));

    if (1 == exact) //get operate && find node, unlock
    {
        hhrMacAddressTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pMacAddressEntry;
}


/************************ get functions *****************************/
u_char *hhrMacConfig_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mac_configuration *pMacConfigEntry;
    u_int32_t *mac_age_time = 0;
    u_int8_t *mac_move_status = 0;

//   zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] enter hhrMacConfig_get\n", __FUNCTION__, __LINE__);

    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    //pMacConfigEntry = snmp_get_mac_config(MODULE_ID_SNMPD);
    struct ipc_mesg_n *pMsgRcv = snmp_get_mac_config(MODULE_ID_SNMPD);

    if(pMsgRcv == NULL)
    {
        return NULL;
    }
    
    pMacConfigEntry = (struct mac_configuration *) pMsgRcv->msg_data;

    if (pMacConfigEntry == NULL)
    {
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMacAgeingTime:
            int_value = pMacConfigEntry->age_time;
            *var_len = sizeof(int);
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&int_value;

        case hhrMacMoveEnable:
            int_value = (DISABLE == pMacConfigEntry->mac_move) ? 2 : 1;
            *var_len = sizeof(int);
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (u_char *)&int_value;

        case hhrMacLearnLimit:
            uint_value = pMacConfigEntry->limit_num;
            *var_len = sizeof(int);
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (uchar *)&uint_value;

        case hhrMacLearnAction:
            int_value = (pMacConfigEntry->limit_action) ? 2 : 1;
            *var_len = sizeof(int);
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (uchar *)&int_value;

        default:
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
            return (NULL);
    }
}

u_char *hhrMacAddressTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mac_entry   *pMacAddressEntry = NULL;
    struct mac_key index;
    struct mac_key index_next;
    int temp;
    int ret = 0;

    memset(&index, 0, sizeof(struct mac_key));
    memset(&index_next, 0, sizeof(struct mac_key));
    //  zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] enter hhrMacAddressTable_get\n", __FUNCTION__, __LINE__);

    /* validate the index */
    ret = ipran_snmp_mac_int_index_get(vp, name, length, &index.vlanid, index.mac, MAC_LEN, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pMacAddressEntry = hhrMacAddressTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pMacAddressEntry)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_mac_int_index_set(vp, name, length, index_next.vlanid, index_next.mac, MAC_LEN);
    }

    switch (vp->magic)
    {
        /*delete by liub 2019-1-18, no need to get vsi in this table*/
#if 0
        case hhrMacVSI:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)&int_value;
#endif
        case hhrMaclfDescr:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(pMacAddressEntry->outif, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrMacType:
            int_value = 0;
            temp = pMacAddressEntry->status;

            if (temp == MAC_STATUS_STATIC)
            {
                int_value = 1;
            }
            else if (temp == MAC_STATUS_BLACKHOLE)
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}

int hhrMacIfLimitTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache, struct  l2if *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct  l2if *pdata;
    int data_num = 0;
    int i = 0;
    
    pMsgRcv = snmp_get_switch_info_bulk(index_input->ifindex, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct  l2if *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct  l2if));
            pdata++;
        }

        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct l2if *hhrMacIfLimitTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                            int exact,
                                            const struct l2if  *index_input)

{
    struct l2if *pdata;
    struct listnode        *node;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
        {
            if (1 == exact)
            {
                return pdata;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrMacIfLimitTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2if *pdata = NULL;
    struct l2if data_index;
    uint32_t if_index = 0;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrMacIfLimitTable_cache)
    {
        hhrMacIfLimitTable_cache = snmp_cache_init(sizeof(struct l2if),
                                                   hhrMacIfLimitTable_get_data_from_ipc,
                                                   hhrMacIfLimitTable_data_lookup);

        if (NULL == hhrMacIfLimitTable_cache)
        {
            return NULL;
        }
    }

    data_index.ifindex = if_index;
    pdata = snmp_cache_get_data_by_index(hhrMacIfLimitTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrIfMacLimitifDescr:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(pdata->ifindex, (char *)str_value);
            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrIfMacLearnLimit:
            uint_value = pdata->switch_info.limit_num;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case hhrIfMacLearnAction:
            int_value = (pdata->switch_info.limit_action) ? 2 : 1;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default :
            return NULL;
    }
}


struct ipc_mesg_n *snmp_dynamac_mac_get_bulk(struct mac_snmp_index *key_entry, int *pdata_num)
{
    struct ipc_mesg_n   *pmesg = ipc_sync_send_n2(key_entry, sizeof(struct mac_snmp_index), 1 ,
                     MODULE_ID_HAL, MODULE_ID_SNMPD, IPC_TYPE_MAC, MAC_INFO_MAC, IPC_OPCODE_GET_BULK, 0, 5000);

    if (pmesg != NULL)
    {
        if(pmesg->msg_data == NULL || pmesg->msghdr.data_num == 0)
        {
            mem_share_free(pmesg, MODULE_ID_SNMPD);
            return NULL;
        }
        else
        {
            *pdata_num = pmesg->msghdr.data_num;
            return pmesg;
        }
    }

    return NULL;
}

int hhrDynamicMacAddressTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct mac_snmp_entry *index_input)
{
    struct mac_snmp_entry *pdata = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;
    int data_len = sizeof(struct mac_snmp_entry);
    int data_num = 0;
    int i;
    struct mac_snmp_index key_entry;
    struct mac_snmp_entry node_tail;

    memset(&key_entry, 0, sizeof(struct mac_snmp_index));
    key_entry.addr_index = index_input->addr_index;
    key_entry.key.vlanid = index_input->key.vlanid;
    memcpy(key_entry.key.mac, index_input->key.mac, MAC_LEN);
    
    //delete data_list which stored the previous data, avoid fruitless search in lookup function
    if(cache->data_list->count)
    {
        memset(&node_tail, 0, sizeof(struct mac_snmp_entry));
        memcpy(&node_tail, cache->data_list->tail->data, cache->node_size);
    
        list_delete_all_node(cache->data_list);

        snmp_cache_add(cache, &node_tail, cache->node_size);     //leave the tail node for get_bulk index
    }
        
    pMsgRcv = snmp_dynamac_mac_get_bulk(&key_entry, &data_num);
    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct mac_snmp_entry *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        return TRUE;
    }
}

struct mac_snmp_entry *hhrDynamicMacAddressTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                        int exact,
                                                        const struct mac_snmp_entry  *index_input)
{
    struct listnode            *node;
    struct mac_snmp_entry *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL)
        {
            return cache->data_list->head->data;
        }

        if (index_input->key.vlanid == 0)
        {
            return cache->data_list->head->data;
        }

        if (pdata->key.vlanid == index_input->key.vlanid 
            && (!memcmp(pdata->key.mac, index_input->key.mac, MAC_LEN)))
        {
            if (exact == 1)
            {
                return pdata;
            }
            else
            {
                if (node->next == NULL)
                {
                    return NULL;
                }

                return node->next->data;
            }
        }
    }

    return NULL;
}

u_char *hhrDynamicMacAddressTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mac_snmp_entry   *pMacAddressEntry = NULL;
    struct mac_snmp_entry mac_index;
    struct mac_key index;
    int data_len = sizeof(struct mac_key);
    int temp;
    int ret = 0;

    memset(&index, 0, data_len);
    memset(&mac_index, 0, sizeof(struct mac_snmp_entry));

    /* validate the index */
    ret = ipran_snmp_mac_int_index_get(vp, name, length, &index.vlanid, index.mac, MAC_LEN, exact);

    if (ret < 0)
    {
        return NULL;
    }
    

    if (hhrDynamicMacAddressTable_cache == NULL)
    {
        hhrDynamicMacAddressTable_cache = snmp_cache_init(sizeof(struct mac_snmp_entry),
                                                   hhrDynamicMacAddressTable_get_data_from_ipc ,
                                                   hhrDynamicMacAddressTable_node_lookup);

        if (hhrDynamicMacAddressTable_cache == NULL)
        {
            return (NULL);
        }
    }

    mac_index.key.vlanid = index.vlanid;
    memcpy(mac_index.key.mac, index.mac, MAC_LEN);
    pMacAddressEntry = snmp_cache_get_data_by_index(hhrDynamicMacAddressTable_cache, exact, &mac_index);

    if (pMacAddressEntry == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_mac_int_index_set(vp, name, length, 
                            pMacAddressEntry->key.vlanid, 
                            pMacAddressEntry->key.mac, MAC_LEN);
    }

    switch (vp->magic)
    {
        case hhrDynamicMaclfDescr:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(pMacAddressEntry->outif, (char *)str_value);
            if (ret < 0)
            {
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}

