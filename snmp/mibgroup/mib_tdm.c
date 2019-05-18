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
#include "ipran_snmp_data_cache.h"
#include "snmp_index_operater.h"
#include "snmpd.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/devm_com.h>
#include <ces/ces_if.h>

#include "mib_tdm.h"
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/log.h>
#include "snmp_config_table.h"

static struct ipran_snmp_data_cache * tdm_local_info_cache = NULL ;

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;

/* table list define */
struct list *hhrCE1Table_list  = NULL;
struct list *hhrCE1SubifTable_list = NULL;

/* variable to store refresh data time of the last time */
static time_t  hhrCE1Table_time_old  = 0;
static time_t  hhrCE1SubifTable_time_old = 0;


/* lock of tables */
enum table_lock
{
    UNLOCK,
    LOCK,
};
static enum table_lock hhrCE1Table_lock = UNLOCK;
static enum table_lock hhrCE1SubifTable_lock = UNLOCK;

static void *tdm_node_xcalloc(int table_num);
static void tdm_node_add(int table_num, void *node);
static void *tdm_node_lookup(int exact, int table_num, void *index_input);
static void tdm_node_del_all(int table_num);
static void tdm_list_init(void);

/*
 * Object ID definitions
 */
/*hhrCE1Table*/
static oid hhrCE1Table_oid[] = {HHRTDMOID, 1};
FindVarMethod hhrCE1Table_get;
struct variable2 hhrCE1Table_variables[] =
{
    {hhrCE1IfDesc,           ASN_OCTET_STR, RONLY, hhrCE1Table_get, 2, {1,  1}},
    {hhrCE1Encapsulate,      ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  2}},
    {hhrCE1FramedMode,       ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  3}},
    {hhrCE1Pcm,              ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  4}},
    {hhrCE1Crc,              ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  5}},
    {hhrCE1JitterBuffer,     ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  6}},
    {hhrCE1Rtp,              ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  7}},
    {hhrCE1loopback,         ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  8}},
    {hhrCE1SendClock,        ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1,  9}},
    {hhrCE1ReceiveClock,     ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 10}},
    {hhrCE1Prbs,             ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 11}},
    {hhrCE1AdminStatus,      ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 12}},
    {hhrCE1StatisticsEnable, ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 13}},
    {hhrCE1TimeSlotStart,    ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 14}},
    {hhrCE1TimeSlotEnd,      ASN_INTEGER,   RONLY, hhrCE1Table_get, 2, {1, 15}}
};

/*hhrCE1SubIfTable*/
static oid hhrCE1SubIfTable_oid[] = {HHRTDMOID, 2};
FindVarMethod hhrCE1SubIfTable_get;
struct variable2 hhrCE1SubIfTable_variables[] =
{
    {hhrCE1SubIfDesc,          ASN_OCTET_STR, RONLY, hhrCE1SubIfTable_get, 2, {1,  1}},
    {hhrCE1SubIfTimeSlotStart, ASN_INTEGER,   RONLY, hhrCE1SubIfTable_get, 2, {1,  2}},
    {hhrCE1SubIfTimeSlotEnd,   ASN_INTEGER,   RONLY, hhrCE1SubIfTable_get, 2, {1,  3}}
};

void init_mib_tdm(void)
{
    if (gDevTypeID == ID_HT157 ||
            gDevTypeID == ID_HT158 ||
            gDevTypeID == ID_HT153 ||
            gDevTypeID == ID_HT201)
    {
        return  ;
    }

    REGISTER_MIB("hhrCE1Table", hhrCE1Table_variables,
                 variable2, hhrCE1Table_oid);
    REGISTER_MIB("hhrCE1SubIfTable", hhrCE1SubIfTable_variables,
                 variable2, hhrCE1SubIfTable_oid);
    tdm_list_init();
}

/* 批量返回 ifindex 后面的 100 个 ces 接口信息，返回值是数组的指针，*pdata_num 返回实际的接口数量 */
struct ipc_mesg_n *ces_if_get_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                             IPC_TYPE_CES, 0, IPC_OPCODE_GET_BULK, 0);
#endif

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                	IPC_TYPE_CES, 0, IPC_OPCODE_GET_BULK, 0,1000);

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

/* hhrCE1Table + hhrCE1SubIfTable*/

void hhrCE1Table_node_free(struct ces_if *node)
{
    XFREE(0, node);
}

int hhrCE1Table_get_data_from_ipc(u_int32_t index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct ces_if *pCesEntry = NULL;
    int data_num = 0;
    int ret = 0;
    int flag = 0;
    u_int32_t  tmp_index = index;

    while (1)
    {
        pMsgRcv = ces_if_get_bulk(tmp_index, MODULE_ID_SNMPD, &data_num);

        if (pMsgRcv == NULL)
        {
            return FALSE;
        }
        else
        {
            pCesEntry = (struct ces_if *)pMsgRcv->msg_data;
            for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
            {
                tmp_index = pCesEntry->ifindex;

                if (!IFM_IS_SUBPORT(pCesEntry->ifindex))
                {
                    tdm_node_add(HHR_CE1_TABLE, pCesEntry);
                    flag = 1;
                }

                pCesEntry++;
            }
            
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

            if (1 == flag)  //Returns subinterface
            {
                time(&hhrCE1Table_time_old);    //refresh time_old after refresh cache data
                return TRUE;
            }
        }
    }

    return FALSE;
}

struct ces_if *hhrCE1Table_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct ces_if *pCesEntry = NULL;
    struct ces_if *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_CE1_TABLE_GET_NEXT_INTERVAL))
    {
        hhrCE1Table_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCE1Table_time_old) > HHR_CE1_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrCE1Table_lock))
    {
        if (NULL == hhrCE1Table_list)
        {
            hhrCE1Table_list = list_new();
            hhrCE1Table_list->del = (void (*)(void *))hhrCE1Table_node_free;
        }

        if (0 != hhrCE1Table_list->count)
        {
            tdm_node_del_all(HHR_CE1_TABLE);
        }
    }

    /* then lock */
    hhrCE1Table_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrCE1Table_list)
    {
        hhrCE1Table_list = list_new();
        hhrCE1Table_list->del = (void (*)(void *))hhrCE1Table_node_free;
    }

    if (0 == hhrCE1Table_list->count)
    {
        index_temp = 0;
        ret = hhrCE1Table_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pCesEntry = (struct ces_if *)tdm_node_lookup(exact, HHR_CE1_TABLE, index);

    if (NULL == pCesEntry)  //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL ==hhrCE1Table_list->tail ||  NULL == hhrCE1Table_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct ces_if *)hhrCE1Table_list->tail->data;
            ret = hhrCE1Table_get_data_from_ipc(entry_temp->ifindex);

            if (FALSE == ret)
            {
                hhrCE1Table_lock = UNLOCK;  //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pCesEntry = (struct ces_if *)tdm_node_lookup(exact, HHR_CE1_TABLE, index);

            if (NULL != pCesEntry)
            {
                *index_next = pCesEntry->ifindex;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCE1Table_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pCesEntry;
            }
        }
    }

    *index_next = pCesEntry->ifindex;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCE1Table_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pCesEntry;
}

void hhrCE1SubIfTable_node_free(struct ces_if *node)
{
    XFREE(0, node);
}

int hhrCE1SubIfTable_get_data_from_ipc(u_int32_t index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct ces_if *pCesSubifEntry = NULL;
    int data_num = 0;
    int ret = 0;
    int flag = 0;
    u_int32_t tmp_index = index;

    while (1)
    {
        pMsgRcv = ces_if_get_bulk(tmp_index, MODULE_ID_SNMPD, &data_num);

        if (pMsgRcv == NULL)
        {
            return FALSE;
        }
        else
        {
            pCesSubifEntry = (struct ces_if *)pMsgRcv->msg_data;
            for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
            {
                tmp_index = pCesSubifEntry->ifindex;

                if (IFM_IS_SUBPORT(pCesSubifEntry->ifindex))
                {
                    tdm_node_add(HHR_CE1_SUBIF_TABLE, pCesSubifEntry);
                    flag = 1;
                }

                pCesSubifEntry++;
            }
            
            mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

            if (1 == flag)  //Returns not subinterface
            {
                time(&hhrCE1SubifTable_time_old);   //refresh time_old after refresh cache data
                return TRUE;
            }
        }
    }

    return FALSE;
}

struct ces_if *hhrCE1SubIfTable_get_data_by_index(int exact, u_int32_t *index, u_int32_t *index_next)
{
    struct ces_if *pCesSubifEntry = NULL;
    struct ces_if *entry_temp;
    u_int32_t index_temp;
    static time_t getNext_time_old; //to store the get-next operate time of the last time
    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if ((abs(time_now - getNext_time_old) > HHR_CE1_SUBIF_TABLE_GET_NEXT_INTERVAL))
    {
        hhrCE1SubifTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrCE1SubifTable_time_old) > HHR_CE1_SUBIF_TABLE_REFRESH_TIME) && \
            (UNLOCK == hhrCE1SubifTable_lock))
    {
        if (NULL == hhrCE1SubifTable_list)
        {
            hhrCE1SubifTable_list = list_new();
            hhrCE1SubifTable_list->del = (void (*)(void *))hhrCE1SubIfTable_node_free;
        }

        if (0 != hhrCE1SubifTable_list->count)
        {
            tdm_node_del_all(HHR_CE1_SUBIF_TABLE);
        }
    }

    /* then lock */
    hhrCE1SubifTable_lock = LOCK;

    /* if list empty, get data by index(0,0,...) */
    if (NULL == hhrCE1SubifTable_list)
    {
        hhrCE1SubifTable_list = list_new();
        hhrCE1SubifTable_list->del = (void (*)(void *))hhrCE1SubIfTable_node_free;
    }

    if (0 == hhrCE1SubifTable_list->count)
    {
        index_temp = 0;
        ret = hhrCE1SubIfTable_get_data_from_ipc(index_temp);

        if (FALSE == ret)
        {
            return NULL;
        }
    }

    pCesSubifEntry = (struct ces_if *)tdm_node_lookup(exact, HHR_CE1_SUBIF_TABLE, index);

    if (NULL == pCesSubifEntry) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrCE1SubifTable_list->tail->data)
            {
                return NULL;
            }

            entry_temp = (struct ces_if *)hhrCE1SubifTable_list->tail->data;
            ret = hhrCE1SubIfTable_get_data_from_ipc(entry_temp->ifindex);

            if (FALSE == ret)
            {
                hhrCE1SubifTable_lock = UNLOCK; //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pCesSubifEntry = (struct ces_if *)tdm_node_lookup(exact, HHR_CE1_SUBIF_TABLE, index);

            if (NULL != pCesSubifEntry)
            {
                *index_next = pCesSubifEntry->ifindex;

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrCE1SubifTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pCesSubifEntry;
            }
        }
    }

    *index_next = pCesSubifEntry->ifindex;

    if (1 == exact) //get operate && find node, unlock
    {
        hhrCE1SubifTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pCesSubifEntry;
}

/******************** list operate functions  ****************/
static void *tdm_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case HHR_CE1_TABLE:
            return XCALLOC(0, sizeof(struct ces_if));
            break;

        case HHR_CE1_SUBIF_TABLE:
            return XCALLOC(0, sizeof(struct ces_if));
            break;

        default:
            break;
    }

    return NULL;
}

static void tdm_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case HHR_CE1_TABLE:
            node_add = tdm_node_xcalloc(HHR_CE1_TABLE);
            memcpy(node_add, node, sizeof(struct ces_if));
            listnode_add(hhrCE1Table_list, node_add);
            break;

        case HHR_CE1_SUBIF_TABLE:
            node_add = tdm_node_xcalloc(HHR_CE1_SUBIF_TABLE);
            memcpy(node_add, node, sizeof(struct ces_if));
            listnode_add(hhrCE1SubifTable_list, node_add);
            break ;

        default:
            break;
    }
}

static void *tdm_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    /* static route table */
    struct ces_if   *data1_find;
    u_int32_t       index1;

    /* rip instance table */
    struct ces_if   *data2_find;
    u_int32_t       index2;


    switch (table_num)
    {
        case HHR_CE1_TABLE:
            index1 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCE1Table_list, node, nnode, data1_find))
            {
                if (0 == index1)
                {
                    return hhrCE1Table_list->head->data;
                }

                if (data1_find->ifindex == index1)
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

        case HHR_CE1_SUBIF_TABLE:
            index2 = *((u_int32_t *)index_input);

            for (ALL_LIST_ELEMENTS(hhrCE1SubifTable_list, node, nnode, data2_find))
            {
                if (0 == index2)
                {
                    return hhrCE1SubifTable_list->head->data;
                }

                if (data2_find->ifindex == index2)
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

    return (NULL);
}

static void tdm_node_del_all(int table_num)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    switch (table_num)
    {
        case HHR_CE1_TABLE:
            list_delete_all_node(hhrCE1Table_list);
            break;

        case HHR_CE1_SUBIF_TABLE:
            list_delete_all_node(hhrCE1SubifTable_list);
            break;

        default:
            break;
    }
}

static void tdm_list_init(void)
{
    hhrCE1Table_list = list_new();
    hhrCE1Table_list->del = (void (*)(void *))hhrCE1Table_node_free;

    hhrCE1SubifTable_list = list_new();
    hhrCE1SubifTable_list->del = (void (*)(void *))hhrCE1SubIfTable_node_free;
}
int tdm_if_local_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct ces_if   *index)
{
	struct ipc_mesg_n *pMsgRcv = NULL;
    struct ces_if * ptdm_info = NULL;
    int data_num = 0;
    int ret = 0;

    pMsgRcv = ces_if_get_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        ptdm_info = (struct ces_if *)pMsgRcv->msg_data;
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, ptdm_info , sizeof(struct ces_if));
            ptdm_info++;
        }
        
		mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        //time(&cache->getNext_time_old);
        return TRUE;
    }
}

struct ces_if *tdm_if_local_info_node_lookup(struct ipran_snmp_data_cache *cache ,
        int exact,
        const struct ces_if  *index_input)
{
    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
    struct listnode  *node, *nnode;
    struct ces_if *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->ifindex == index_input->ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return (NULL);
}

/************************ get functions *****************************/
u_char *hhrCE1Table_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ces_if   *pCesEntry = NULL;
    struct ifm_info pifm = {0};
    int temp;
    int ret = 0;
    struct ces_if index;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] enter hhrCE1Table_get\n", __FUNCTION__, __LINE__);

    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (NULL == tdm_local_info_cache)
    {
        tdm_local_info_cache = snmp_cache_init(sizeof(struct ces_if) ,
                                               tdm_if_local_info_get_data_from_ipc ,
                                               tdm_if_local_info_node_lookup);

        if (NULL == tdm_local_info_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }
    pCesEntry = snmp_cache_get_data_by_index(tdm_local_info_cache , exact, &index);
    if(NULL == pCesEntry)
    {
        return (NULL);

    }
    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pCesEntry->ifindex);
    }

    switch (vp->magic)
    {
        case hhrCE1IfDesc:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(pCesEntry->ifindex, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__,pCesEntry->ifindex);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrCE1Encapsulate:
            int_value = 0;
            temp = pCesEntry->ces.encap;

            if (temp == CES_ENCAP_SATOP)
            {
                int_value = 1;
            }
            else if (temp == CES_ENCAP_CESOPSN)
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1FramedMode:
            int_value = 0;
            temp = pCesEntry->ces.mode;

            if (temp == CES_MODE_UNFRAMED)
            {
                int_value = 1;
            }
            else if (temp == CES_MODE_FRAMED)
            {
                int_value = 2;
            }
            else if (temp == CES_MODE_MULTIFRAMED)
            {
                int_value = 3;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1Pcm:
            int_value = 0;
            temp = pCesEntry->ces.frame;

            if ((CES_FRAME_PCM30 == temp) || (CES_FRAME_PCM30_CRC == temp))
            {
                int_value = 1;
            }
            else if ((CES_FRAME_PCM31 == temp) || (CES_FRAME_PCM31_CRC == temp))
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1Crc:
            int_value = 0;
            temp = pCesEntry->ces.frame;

            if ((CES_FRAME_PCM30_CRC == temp) || (CES_FRAME_PCM31_CRC == temp))
            {
                int_value = 1;
            }
            else if ((CES_FRAME_PCM30 == temp) || (CES_FRAME_PCM31 == temp))
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1JitterBuffer:
            int_value = 0;
            int_value = pCesEntry->ces.jitbuffer;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1Rtp:
            temp = pCesEntry->ces.rtp;
            int_value = (DISABLE == temp) ? 2 : 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1loopback:
            int_value = 0;
            temp = pCesEntry->ces.loopback;

            if (temp == CES_LB_INTERBAL)
            {
                int_value = 1;
            }
            else if (temp == CES_LB_EXTERNAL)
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1SendClock:
            int_value = 0;
            temp = pCesEntry->ces.clock;

            if (temp == CES_CLK_LOOPBACK)
            {
                int_value = 1;
            }
            else if (temp == CES_CLK_LOCAL)
            {
                int_value = 2;
            }
            else if (temp == CES_CLK_ACR)
            {
                int_value = 3;
            }
            else if (temp == CES_CLK_DCR)
            {
                int_value = 4;
            }
            else if (temp == CES_CLK_PHY)
            {
                int_value = 5;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1ReceiveClock:
            int_value = 0;
            temp = pCesEntry->ces.recvclock;

            if (temp == CES_RECEIVE_CLK_LINE)
            {
                int_value = 1;
            }
            else if (temp == CES_RECEIVE_CLK_EXTERNAL)
            {
                int_value = 2;
            }
            else if (temp == CES_RECEIVE_CLK_LOCAL)
            {
                int_value = 3;
            }

            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1Prbs:
            temp = pCesEntry->ces.prbs;
            int_value = (DISABLE == temp) ? 2 : 1;
            *var_len = sizeof(int);
            return (u_char *)&int_value;

        case hhrCE1AdminStatus:
            int_value = 0;
            ret = ifm_get_all_info(pCesEntry->ifindex, MODULE_ID_SNMPD, &pifm);

            if (0 == ret)
            {
                *var_len = sizeof(int);
                int_value = (pifm.shutdown == IFNET_NO_SHUTDOWN) ? 1 : 2; // 1:no shutdown, 2: shutdown
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case hhrCE1StatisticsEnable:
            int_value = 0;
            ret = ifm_get_all_info(pCesEntry->ifindex, MODULE_ID_SNMPD, &pifm);

            if (0 == ret)
            {
                *var_len = sizeof(int);
                int_value = (pifm.statistics == IFNET_STAT_DISABLE) ? 2 : 1; // 1:enable, 2: disable
                return (u_char *)(&int_value);
            }
            else
            {
                return NULL;
            }

        case hhrCE1TimeSlotStart:
            *var_len = sizeof(int);
            int_value = 1;
            return (u_char *)(&int_value);

        case hhrCE1TimeSlotEnd:
            *var_len = sizeof(int);
            int_value = 31;
            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}

u_char *hhrCE1SubIfTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ces_if   *pCesSubifEntry = NULL;
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int i = 0;
    int ret = 0;

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] enter hhrCE1SubIfTable_get\n", __FUNCTION__, __LINE__);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pCesSubifEntry = hhrCE1SubIfTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pCesSubifEntry)
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
        case hhrCE1SubIfDesc:
            memset(str_value, 0, sizeof(str_value));
            ret = ifm_get_name_by_ifindex(index_next, (char *)str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] ifm_get_name_by_ifindex 0x%0x error\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *)str_value);
            return str_value;

        case hhrCE1SubIfTimeSlotStart:
            int_value = 0;
            *var_len = sizeof(int);

            for (i = 1; i <= 31; i++)
            {
                if (pCesSubifEntry->ces.timeslot & (1 << i))
                {
                    int_value = i;
                    break;
                }
            }

            return (u_char *)(&int_value);

        case hhrCE1SubIfTimeSlotEnd:
            int_value = 0;
            *var_len = sizeof(int);

            for (i = 1; i <= 31; i++)
            {
                if (pCesSubifEntry->ces.timeslot & (1 << i))
                {
                    int_value = i;
                }
            }

            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}
