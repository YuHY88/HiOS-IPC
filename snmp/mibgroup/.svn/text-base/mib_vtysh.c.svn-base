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

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include "mib_vtysh.h"

#include "vtysh/vtysh_snmp.h"
#include "lib/aaa_common.h"

/*local temp variable*/
static uchar    str_value[STRING_LEN] = {'\0'};
static uchar    *str = NULL;
static uchar    mac_value[6] = {0};
static int      int_value = 0;
static uint32_t ip_value = 0;


/* table list define */
struct list *hhrVtyshUserTable_list  = NULL;
struct list *hhrVtyshOnlineUserTable_list  = NULL;

/* variable to store refresh data time of the last time */
static int  hhrVtyshUserTable_time_old  = 0;
static int  hhrVtyshOnlineUserTable_time_old  = 0;

/* lock of tables */
enum table_lock
{
    UNLOCK,
    LOCK,
};
static enum table_lock hhrVtyshUserTable_lock = UNLOCK;
static enum table_lock hhrVtyshOnlineUserTable_lock = UNLOCK;


/*vtysh login cfg*/
static oid hhrVtyshLoginCfg_oid[] = {HHRVTYSH};
FindVarMethod hhrVtyshLoginCfg_get;
struct variable1 hhrVtyshLoginCfg_variables[] =
{
    {hhrVtyshLoginTimeout,          ASN_INTEGER,    RWRITE, hhrVtyshLoginCfg_get, 1, {1}},
    {hhrVtyshLoginAuthType,         ASN_INTEGER,    RWRITE, hhrVtyshLoginCfg_get, 1, {2}}
};


/* vtysh user table */
static oid hhrVtyshUserTable_oid[] = {HHRVTYSH, 10};
FindVarMethod hhrVtyshUserTable_get;
struct variable2 hhrVtyshUserTable_variables[] =
{
    {hhrVtyshPrivilege,         ASN_INTEGER,    RONLY, hhrVtyshUserTable_get, 2, {1, 2}}
};

/* vtysh user online table */
static oid hhrVtyshOnlineUserTable_oid[] = {HHRVTYSH, 11};
FindVarMethod hhrVtyshOnlineUserTable_get;
struct variable2 hhrVtyshOnlineUserTable_variables[] =
{
    {hhrVtyshUserName,          ASN_OCTET_STR,  RONLY, hhrVtyshOnlineUserTable_get, 2, {1, 2}},
    {hhrVtyshUserFrom,          ASN_OCTET_STR,  RONLY, hhrVtyshOnlineUserTable_get, 2, {1, 3}},
    {hhrVtyshUserPrivilege,     ASN_INTEGER,    RONLY, hhrVtyshOnlineUserTable_get, 2, {1, 4}}
};



/******ipc msg request******/
/* vtysh login config */
uint32_t *hhrVtyshLoginConfig_get(int module_id, int subtype)
{
    return ipc_send_common_wait_reply(NULL, 0, 1, module_id, MODULE_ID_SNMPD,
                                      IPC_TYPE_SNMP, subtype, 0, 0);
}



struct vtysh_snmp_user_info *hhrVtyshUserTable_get_bulk(struct vtysh_snmp_user_index *index,
                                                        int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, sizeof(struct vtysh_snmp_user_index), 1,
                                                         module_id, MODULE_ID_SNMPD, IPC_TYPE_SNMP, VTYSH_USER, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg->msg_data;
    }

    return NULL;
}

struct vtysh_snmp_online_user_info *hhrVtyshOnlineUserTable_get_bulk(struct vtysh_snmp_online_user_index *index,
                                                                     int module_id, int *pdata_num)
{
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, sizeof(struct vtysh_snmp_online_user_index), 1,
                                                         module_id, MODULE_ID_SNMPD, IPC_TYPE_SNMP, VTYSH_ONLINE_USER, IPC_OPCODE_GET_BULK, 0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return pmesg->msg_data;
    }

    return NULL;
}



/* list operate */
static void *vtysh_node_xcalloc(int table_num)
{
    switch (table_num)
    {
        case VTYSH_USER_TABLE:
            return XCALLOC(0, sizeof(struct vtysh_snmp_user_info));
            break;

        case VTYSH_ONLINE_USER_TABLE:
            return XCALLOC(0, sizeof(struct vtysh_snmp_online_user_info));
            break;

        default:
            return NULL;
    }

    return NULL;
}

static void vtysh_node_add(int table_num, void *node)
{
    void *node_add = NULL;

    switch (table_num)
    {
        case VTYSH_USER_TABLE:
            node_add = vtysh_node_xcalloc(VTYSH_USER_TABLE);
            memcpy(node_add, node, sizeof(struct vtysh_snmp_user_info));
            listnode_add(hhrVtyshUserTable_list, node_add);
            break;

        case VTYSH_ONLINE_USER_TABLE:
            node_add = vtysh_node_xcalloc(VTYSH_ONLINE_USER_TABLE);
            memcpy(node_add, node, sizeof(struct vtysh_snmp_online_user_info));
            listnode_add(hhrVtyshOnlineUserTable_list, node_add);
            break;

        default:
            break;
    }
}


static void *vtysh_node_lookup(int exact, int table_num, void *index_input)
{
    struct listnode  *node, *nnode;

    /* static vtysh user table */
    struct vtysh_snmp_user_info     *data1_find;
    struct vtysh_snmp_user_index    *index1;

    /* static vtysh user table */
    struct vtysh_snmp_online_user_info  *data2_find;
    struct vtysh_snmp_online_user_index *index2;

    switch (table_num)
    {
        case VTYSH_USER_TABLE:
            index1 = (struct vtysh_snmp_user_index *)index_input;

            for (ALL_LIST_ELEMENTS(hhrVtyshUserTable_list, node, nnode, data1_find))
            {
                zlog_debug(SNMP_DBG_MIB_GET, "index_input(%s) vs (%s)index_found, exact[%d]\n", \
                           index1->username, data1_find->index.username, exact);

                if (0 == strlen(index1->username))
                {
                    return hhrVtyshUserTable_list->head->data;
                }

                if ((strlen(index1->username) == strlen(data1_find->index.username))\
                        && (0 == memcmp(index1->username, data1_find->index.username, strlen(index1->username))))
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

        case VTYSH_ONLINE_USER_TABLE:
            index2 = (struct vtysh_snmp_online_user_index *)index_input;

            for (ALL_LIST_ELEMENTS(hhrVtyshOnlineUserTable_list, node, nnode, data2_find))
            {
                zlog_debug(SNMP_DBG_MIB_GET, "index_input(%d) vs (%d)index_found\n", \
                           index2->usernum, data2_find->index.usernum);

                if (0 == index2->usernum)
                {
                    return hhrVtyshOnlineUserTable_list->head->data;
                }

                if (index2->usernum == data2_find->index.usernum)
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


static void vtysh_node_del_all(int table_num)
{
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' \n", __FILE__, __LINE__, __func__);

    switch (table_num)
    {
        case VTYSH_USER_TABLE:
            list_delete_all_node(hhrVtyshUserTable_list);
            break;

        case VTYSH_ONLINE_USER_TABLE:
            list_delete_all_node(hhrVtyshOnlineUserTable_list);
            break;

        default:
            break;
    }
}




/* vtysh user table */

void hhrVtyshUserTable_node_free(struct vtysh_snmp_user_info *node)
{
    XFREE(0, node);
}

int hhrVtyshUserTable_get_data_from_ipc(struct vtysh_snmp_user_index *index)
{
    struct vtysh_snmp_user_info *vtysh_struct = NULL;
    int data_num = 0;
    int ret = 0;

    vtysh_struct = hhrVtyshUserTable_get_bulk(index, MODULE_ID_VTY, &data_num);

    if (0 == data_num)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> no vtysh user table data return, read over\n", \
                   __FILE__, __LINE__, __func__);
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> success to get vtysh user table data[num = %d]\n", \
                   __FILE__, __LINE__, __func__, data_num);

        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            vtysh_node_add(VTYSH_USER_TABLE, vtysh_struct);
            printf("name[%s]   privilege[%d]\n", vtysh_struct->index.username, vtysh_struct->data.privilege);
            vtysh_struct++;
        }

        time(&hhrVtyshUserTable_time_old);  //refresh time_old after refresh cache data
        return TRUE;
    }
}

struct vtysh_snmp_user_info *hhrVtyshUserTable_get_data_by_index(int exact, struct vtysh_snmp_user_index *index,
                                                                 struct vtysh_snmp_user_index *index_next)
{
    struct vtysh_snmp_user_info *pVtyshUserTable = NULL;
    struct vtysh_snmp_user_index index_temp;
    struct vtysh_snmp_user_info table_temp;

    static int getNext_time_old;    //to store the get-next operate time of the last time

    int ret = 0;
    time_t time_now = 0;
    time(&time_now);

    /* get-next timeout */
    if (abs(time_now - getNext_time_old) > VTYSH_TABLE_GET_NEXT_INTERVAL)
    {
        hhrVtyshUserTable_lock = UNLOCK;
    }

    /* if timeout && not lock, clear list */
    if ((abs(time_now - hhrVtyshUserTable_time_old) > VTYSH_USER_TABLE_REFRESH_TIME)\
            && (UNLOCK == hhrVtyshUserTable_lock))
    {
        if (0 != hhrVtyshUserTable_list->count)
        {
            vtysh_node_del_all(VTYSH_USER_TABLE);
        }
    }

    /* then lock */
    hhrVtyshUserTable_lock = LOCK;

    if (0 == hhrVtyshUserTable_list->count)
    {
        memset(&index_temp, 0, sizeof(struct vtysh_snmp_user_index));
        ret = hhrVtyshUserTable_get_data_from_ipc(&index_temp);

        if (FALSE == ret)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> vtysh table node not exist, read over\n", \
                       __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pVtyshUserTable = (struct vtysh_snmp_user_info *)vtysh_node_lookup(exact, VTYSH_USER_TABLE, index);

    if (NULL == pVtyshUserTable) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVtyshUserTable_list->tail->data)
            {
                return NULL;
            }

            memcpy(&table_temp, hhrVtyshUserTable_list->tail->data, sizeof(struct vtysh_snmp_user_info));
            ret = hhrVtyshUserTable_get_data_from_ipc(&table_temp.index);

            if (FALSE == ret)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> vtysh table read over\n", \
                           __FILE__, __LINE__, __func__);
                hhrVtyshUserTable_lock = UNLOCK;    //search over, unclock
                time(&getNext_time_old);
                return NULL;
            }

            pVtyshUserTable = (struct vtysh_snmp_user_info *)vtysh_node_lookup(exact, VTYSH_USER_TABLE, index);

            if (NULL != pVtyshUserTable)
            {
                memcpy(index_next->username, pVtyshUserTable->index.username, \
                       sizeof(index_next->username));

                if (1 == exact) //get operate && find node, unlock
                {
                    hhrVtyshUserTable_lock = UNLOCK;
                }
                else
                {
                    time(&getNext_time_old);    //refresh get-next time of this time
                }

                return pVtyshUserTable;
            }
        }
    }

    memcpy(index_next->username, pVtyshUserTable->index.username, \
           sizeof(index_next->username));

    if (1 == exact) //get operate && find node, unlock
    {
        hhrVtyshUserTable_lock = UNLOCK;
    }
    else
    {
        time(&getNext_time_old);    //refresh get-next time of this time
    }

    return pVtyshUserTable;
}

/* vtysh online user table */

void hhrVtyshOnlineUserTable_node_free(struct vtysh_snmp_online_user_info *node)
{
    XFREE(0, node);
}

int hhrVtyshOnlineUserTable_get_data_from_ipc(struct vtysh_snmp_online_user_index *index)
{
    struct vtysh_snmp_online_user_info *vtysh_struct = NULL;
    int data_num = 0;
    int ret = 0;

    vtysh_struct = hhrVtyshOnlineUserTable_get_bulk(index, MODULE_ID_VTY, &data_num);

    if (0 == data_num)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> no vtysh user online table data return, read over\n", \
                   __FILE__, __LINE__, __func__);
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> success to get vtysh online user table data[num = %d]\n", \
                   __FILE__, __LINE__, __func__, data_num);

        for (ret = 0; ret < data_num; ret++) //store all data from ipc into list
        {
            vtysh_node_add(VTYSH_ONLINE_USER_TABLE, vtysh_struct);
            vtysh_struct++;
        }

        time(&hhrVtyshOnlineUserTable_time_old);    //refresh time_old after refresh cache data
        return TRUE;
    }
}

struct vtysh_snmp_online_user_info *hhrVtyshOnlineUserTable_get_data_by_index(int exact, struct vtysh_snmp_online_user_index *index,
                                                                              struct vtysh_snmp_online_user_index *index_next)
{
    struct vtysh_snmp_online_user_info *pVtyshOnlineUserTable = NULL;
    struct vtysh_snmp_online_user_index index_temp;
    struct vtysh_snmp_online_user_info table_temp;
    int ret = 0;

    if (0 == index->usernum)
    {
        vtysh_node_del_all(VTYSH_ONLINE_USER_TABLE);
    }


    if (0 == hhrVtyshOnlineUserTable_list->count)
    {
        index_temp.usernum = 0;
        ret = hhrVtyshOnlineUserTable_get_data_from_ipc(&index_temp);

        if (FALSE == ret)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> vtysh online user table node not exist, read over\n", \
                       __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pVtyshOnlineUserTable = (struct vtysh_snmp_online_user_info *)vtysh_node_lookup(exact, VTYSH_ONLINE_USER_TABLE, index);

    if (NULL == pVtyshOnlineUserTable) //not found, renew buf
    {
        // renew date, funcA
        while (1)
        {
            /* get the index of tail node, countinue to get data from ipc */
            if (NULL == hhrVtyshOnlineUserTable_list->tail->data)
            {
                return NULL;
            }

            memcpy(&table_temp, hhrVtyshOnlineUserTable_list->tail->data, sizeof(struct vtysh_snmp_online_user_info));
            ret = hhrVtyshOnlineUserTable_get_data_from_ipc(&table_temp.index);

            if (FALSE == ret)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: in function '%s' -> vtysh online user table read over\n", \
                           __FILE__, __LINE__, __func__);
                //hhrVtyshOnlineUserTable_lock = UNLOCK;     //search over, unclock
                //time(&getNext_time_old);
                return NULL;
            }

            pVtyshOnlineUserTable = (struct vtysh_snmp_online_user_info *)vtysh_node_lookup(exact, VTYSH_ONLINE_USER_TABLE, index);

            if (NULL != pVtyshOnlineUserTable)
            {
                index_next->usernum = pVtyshOnlineUserTable->index.usernum;

                /*
                if(1 == exact)  //get operate && find node, unlock
                    hhrVtyshOnlineUserTable_lock = UNLOCK;
                else
                    time(&getNext_time_old);     //refresh get-next time of this time
                */

                return pVtyshOnlineUserTable;
            }
        }
    }

    index_next->usernum = pVtyshOnlineUserTable->index.usernum;

    /*
    if(1 == exact)  //get operate && find node, unlock
        hhrVtyshOnlineUserTable_lock = UNLOCK;
    else
        time(&getNext_time_old);     //refresh get-next time of this time
    */

    return pVtyshOnlineUserTable;
}

static void vtysh_list_init(void)
{
    hhrVtyshUserTable_list = list_new();
    hhrVtyshUserTable_list->del = (void (*)(void *))hhrVtyshUserTable_node_free;

    hhrVtyshOnlineUserTable_list = list_new();
    hhrVtyshOnlineUserTable_list->del = (void (*)(void *))hhrVtyshOnlineUserTable_node_free;
}


void init_mib_vtysh(void)
{
    REGISTER_MIB("hhrVtyshLoginCfg", hhrVtyshLoginCfg_variables,
                 variable1, hhrVtyshLoginCfg_oid);

    REGISTER_MIB("hhrVtyshUserTable", hhrVtyshUserTable_variables,
                 variable2, hhrVtyshUserTable_oid);

    REGISTER_MIB("hhrVtyshOnlineUserTable", hhrVtyshOnlineUserTable_variables,
                 variable2, hhrVtyshOnlineUserTable_oid);

    vtysh_list_init();
}


u_char *hhrVtyshLoginCfg_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }

    uint32_t *pbuf = NULL;
    int_value = 0;

    switch (vp->magic)
    {
        case hhrVtyshLoginTimeout:
            pbuf = hhrVtyshLoginConfig_get(MODULE_ID_VTY, VTYSH_USER_TIME_OUT);

            if (pbuf)
            {
                int_value = *pbuf;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrVtyshLoginAuthType:
            pbuf = hhrVtyshLoginConfig_get(MODULE_ID_VTY, VTYSH_USER_MODE);

            if (pbuf)
            {
                int_value = *pbuf;

                switch (int_value)
                {
                    case AUTH_LOCAL:
                        int_value = 3;
                        break;

                    case AUTH_RADIUS:
                        int_value = 1;
                        break;

                    case AUTH_TAC_PLUS:
                        int_value = 2;
                        break;

                    default:
                        int_value = AUTH_NONE;
                        break;
                }
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}


u_char *hhrVtyshUserTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vtysh_snmp_user_info *pVtyshUserTable = NULL;
    struct vtysh_snmp_user_index index;
    struct vtysh_snmp_user_index index_next;
    memset(&index,      0, sizeof(struct vtysh_snmp_user_index));
    memset(&index_next, 0, sizeof(struct vtysh_snmp_user_index));

    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_octstring_index_get(vp, name, length, index.username, \
                                         32, \
                                         exact);
    printf("%d : index = %s\n", __LINE__, index.username);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    pVtyshUserTable = hhrVtyshUserTable_get_data_by_index(exact, &index, &index_next);
    printf("%d : index_next = %s\n", __LINE__, index_next.username);

    if (NULL == pVtyshUserTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_octstring_index_set(vp, name, length, index_next.username, \
                                       strlen(index_next.username));
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case hhrVtyshPrivilege:
            int_value = pVtyshUserTable->data.privilege;
            *var_len = sizeof(int_value);
            return ((uchar *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}


u_char *hhrVtyshOnlineUserTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vtysh_snmp_online_user_info *pVtyshOnlineUserTable = NULL;
    struct vtysh_snmp_online_user_index index;
    struct vtysh_snmp_online_user_index index_next;
    memset(&index,      0, sizeof(struct vtysh_snmp_online_user_index));
    memset(&index_next, 0, sizeof(struct vtysh_snmp_online_user_index));

    int ret = 0;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index.usernum, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* get data by index and renew index_next */
    pVtyshOnlineUserTable = hhrVtyshOnlineUserTable_get_data_by_index(exact, &index, &index_next);

    if (NULL == pVtyshOnlineUserTable)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next.usernum);
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case hhrVtyshUserName:
            *var_len = strlen(pVtyshOnlineUserTable->data.username);
            return ((uchar *)pVtyshOnlineUserTable->data.username);

        case hhrVtyshUserFrom:
            *var_len = strlen(pVtyshOnlineUserTable->data.userfrom);
            return ((uchar *)pVtyshOnlineUserTable->data.userfrom);

        case hhrVtyshUserPrivilege:
            int_value = pVtyshOnlineUserTable->data.userprivilege;
            *var_len = sizeof(int_value);
            return ((uchar *)&int_value);

        default:
            return NULL;
    }

    return NULL;
}




