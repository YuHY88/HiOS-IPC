/*
 *  MIB group implementation - mib_stm.c
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
#include "snmpd.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/devm_com.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <ces/stm_if.h>

#include "mib_stm.h"

#include "snmp_config_table.h"

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;


/*
 * Object ID definitions
 */
/*hhrSdhStmBaseTable*/
static struct ipran_snmp_data_cache *hhrSdhStmBaseTable_cache = NULL;
oid hhrSdhStmBaseTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 1};
FindVarMethod hhrSdhStmBaseTable_get;
struct variable2 hhrSdhStmBaseTable_variables[] =
{
    {hhrSdhStmType,       ASN_INTEGER,   RONLY, hhrSdhStmBaseTable_get, 2, {1,  1}},
    {hhrSdhStmAlarm,      ASN_OCTET_STR, RONLY, hhrSdhStmBaseTable_get, 2, {1,  2}},
};

/*hhrSdhStmRSTable*/
static struct ipran_snmp_data_cache *hhrSdhStmRSTable_cache = NULL;
oid hhrSdhStmRSTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 2};
FindVarMethod hhrSdhStmRSTable_get;
struct variable2 hhrSdhStmRSTable_variables[] =
{
    {hhrSdhStmRSJ0TRANSMIT,     ASN_OCTET_STR, RONLY,   hhrSdhStmRSTable_get, 2, {1,  1}},
    {hhrSdhStmRSJ0EXPECTED,     ASN_OCTET_STR, RONLY,   hhrSdhStmRSTable_get, 2, {1,  2}},
    {hhrSdhStmRSJ0RECEIVE,      ASN_OCTET_STR, RONLY,   hhrSdhStmRSTable_get, 2, {1,  3}},
    {hhrSdhStmRSEXCTHRESHOLD,   ASN_INTEGER,   RONLY,   hhrSdhStmRSTable_get, 2, {1,  4}},
    {hhrSdhStmRSDEGTHRESHOLD,   ASN_INTEGER,   RONLY,   hhrSdhStmRSTable_get, 2, {1,  5}},
    {hhrSdhStmRSB1COUNT,        ASN_INTEGER,   RONLY,   hhrSdhStmRSTable_get, 2, {1,  6}},
    {hhrSdhStmRSALARM,          ASN_OCTET_STR, RONLY,   hhrSdhStmRSTable_get, 2, {1,  7}},
};

/*hhrSdhStmMSTable*/
static struct ipran_snmp_data_cache *hhrSdhStmMSTable_cache = NULL;
oid hhrSdhStmMSTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 3};
FindVarMethod hhrSdhStmMSTable_get;
struct variable2 hhrSdhStmMSTable_variables[] =
{
    {hhrSdhStmMSK2RECEIVE,      ASN_INTEGER,   RONLY,   hhrSdhStmMSTable_get, 2, {1,  1}},
    {hhrSdhStmMSEXCTHRESHOLD,   ASN_INTEGER,   RONLY,   hhrSdhStmMSTable_get, 2, {1,  2}},
    {hhrSdhStmMSDEGTHRESHOLD,   ASN_INTEGER,   RONLY,   hhrSdhStmMSTable_get, 2, {1,  3}},
    {hhrSdhStmMSB2COUNT,        ASN_INTEGER,   RONLY,   hhrSdhStmMSTable_get, 2, {1,  4}},
    {hhrSdhStmMSREICOUNT,       ASN_INTEGER,   RONLY,   hhrSdhStmMSTable_get, 2, {1,  5}},
    {hhrSdhStmMSALARM,          ASN_OCTET_STR, RONLY,   hhrSdhStmMSTable_get, 2, {1,  6}},
};

/*hhrSdhStmHPTable*/
static struct ipran_snmp_data_cache *hhrSdhStmHPTable_cache = NULL;
oid hhrSdhStmHPTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 5};
FindVarMethod hhrSdhStmHPTable_get;
struct variable2 hhrSdhStmHPTable_variables[] =
{
    {hhrSdhStmHPVC4INDEX,       ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1,  1}},
    {hhrSdhStmHPJ1TRANSMIT,     ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  2}},
    {hhrSdhStmHPJ1EXPECTED,     ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  3}},
    {hhrSdhStmHPJ1RECEIVE,      ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  4}},
    {hhrSdhStmHPC2TRANSMIT,     ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  5}},
    {hhrSdhStmHPC2RECEIVE,      ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  6}},
    {hhrSdhStmHPG1RECEIVE,      ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1,  7}},
    {hhrSdhStmHPEXCTHRESHOLD,   ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1,  8}},
    {hhrSdhStmHPDEGTHRESHOLD,   ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1,  9}},
    {hhrSdhStmHPB3COUNT,        ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1, 10}},
    {hhrSdhStmHPREICOUNT,       ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1, 11}},
    {hhrSdhStmHPPLUCOUNT,       ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1, 12}},
    {hhrSdhStmHPNGACOUNT,       ASN_INTEGER,     RONLY,   hhrSdhStmHPTable_get, 2, {1, 13}},
    {hhrSdhStmHPALARM,          ASN_OCTET_STR,   RONLY,   hhrSdhStmHPTable_get, 2, {1, 14}},
};

/*hhrSdhStmMSPTable*/
static struct ipran_snmp_data_cache *hhrSdhStmMSPTable_cache = NULL;
oid hhrSdhStmMSPTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 7};
FindVarMethod hhrSdhStmMSPTable_get;
struct variable2 hhrSdhStmMSPTable_variables[] =
{
    {hhrSdhStmMSPGROUPINDEX,            ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  1}},
    {hhrSdhStmMSPGROUPMODE,             ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  2}},
    {hhrSdhStmMSPGROUPWTR,              ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  3}},
    {hhrSdhStmMSPGROUPMASTERIFINDEX,    ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  4}},
    {hhrSdhStmMSPGROUPBACKUPIFINDEX,    ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  5}},
    {hhrSdhStmMSPGROUPSTATUS,           ASN_INTEGER,    RONLY,   hhrSdhStmMSPTable_get, 2, {1,  6}},
};

/*hhrSdhStmSNCPTable*/
static struct ipran_snmp_data_cache *hhrSdhStmSNCPTable_cache = NULL;
oid hhrSdhStmSNCPTable_oid[] = {1, 3, 6, 1, 4, 1, 9966, 5, 39, 9};
FindVarMethod hhrSdhStmSNCPTable_get;
struct variable2 hhrSdhStmSNCPTable_variables[] =
{
    {hhrSdhStmSNCPGROUPNAME,                ASN_OCTET_STR,  RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  1}},
    {hhrSdhStmSNCPGROUPMODE,                ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  2}},
    {hhrSdhStmSNCPGROUPLEVEL,               ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  3}},
    {hhrSdhStmSNCPGROUPSRCIFINDEX,          ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  4}},
    {hhrSdhStmSNCPGROUPSRCVC4,              ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  5}},
    {hhrSdhStmSNCPGROUPSRCVC3,              ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  6}},
    {hhrSdhStmSNCPGROUPSRCVC12,             ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  7}},
    {hhrSdhStmSNCPGROUPSRCBACKUPIFINDEX,    ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  8}},
    {hhrSdhStmSNCPGROUPSRCBACKUPVC4,        ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1,  9}},
    {hhrSdhStmSNCPGROUPSRCBACKUPVC3,        ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 10}},
    {hhrSdhStmSNCPGROUPSRCBACKUPVC12,       ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 11}},
    {hhrSdhStmSNCPGROUPDSTIFINDEX,          ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 12}},
    {hhrSdhStmSNCPGROUPDSTVC4,              ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 13}},
    {hhrSdhStmSNCPGROUPDSTVC3,              ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 14}},
    {hhrSdhStmSNCPGROUPDSTVC12,             ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 15}},
    {hhrSdhStmSNCPGROUPDSTBACKUPIFINDEX,    ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 16}},
    {hhrSdhStmSNCPGROUPDSTBACKUPVC4,        ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 17}},
    {hhrSdhStmSNCPGROUPDSTBACKUPVC3,        ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 18}},
    {hhrSdhStmSNCPGROUPDSTBACKUPVC12,       ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 19}},
    {hhrSdhStmSNCPGROUPWTRMODE,             ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 20}},
    {hhrSdhStmSNCPGROUPWTR,                 ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 21}},
    {hhrSdhStmSNCPGROUPSTATUS,              ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 22}},
    {hhrSdhStmSNCPGROUPNUM,                 ASN_INTEGER,    RONLY,   hhrSdhStmSNCPTable_get, 2, {1, 23}}
};

void init_mib_stm(void)
{
    if (ID_HT157 == gDevTypeID ||
            ID_HT158 == gDevTypeID ||
            ID_HT153 == gDevTypeID ||
            ID_HT201 == gDevTypeID ||
            ID_HT201E == gDevTypeID)
    {
        return;
    }

    REGISTER_MIB("hhrSdhStmBaseTable", hhrSdhStmBaseTable_variables,
                 variable2, hhrSdhStmBaseTable_oid);

    REGISTER_MIB("hhrSdhStmRSTable", hhrSdhStmRSTable_variables,
                 variable2, hhrSdhStmRSTable_oid);

    REGISTER_MIB("hhrSdhStmMSTable", hhrSdhStmMSTable_variables,
                 variable2, hhrSdhStmMSTable_oid);

    REGISTER_MIB("hhrSdhStmHPTable", hhrSdhStmHPTable_variables,
                 variable2, hhrSdhStmHPTable_oid);

    REGISTER_MIB("hhrSdhStmMSPTable", hhrSdhStmMSPTable_variables,
                 variable2, hhrSdhStmMSPTable_oid);

    REGISTER_MIB("hhrSdhStmSNCPTable", hhrSdhStmSNCPTable_variables,
                 variable2, hhrSdhStmSNCPTable_oid);
}

struct ipc_mesg_n *stm_base_get_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_BASE_GET, IPC_OPCODE_GET_BULK, 0);
#endif

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_BASE_GET, IPC_OPCODE_GET_BULK, 0,1000);
    
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

int hhrSdhStmBaseTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct stm_base_info *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_base_info *pdata = NULL;
    int data_len = sizeof(struct stm_base_info);
    int data_num = 0;
    int i;

    pMsgRcv = stm_base_get_bulk(index_input->ifindex, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_base_info *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        return TRUE;
    }
}

struct stm_base_info *hhrSdhStmBaseTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                     int exact,
                                                     const struct stm_base_info  *index_input)
{
    struct listnode            *node;
    struct stm_base_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL || (index_input->ifindex == 0))
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
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


u_char *hhrSdhStmBaseTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact,
                               size_t *var_len,
                               WriteMethod **write_method)
{
    int ret = 0;
    u_int32_t               index = 0;
    u_int32_t               index_next  = 0;
    int                     data_len = sizeof(struct stm_base_info);
    struct stm_base_info    stm_index;
    struct stm_base_info   *stm_info = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrSdhStmBaseTable_cache == NULL)
    {
        hhrSdhStmBaseTable_cache = snmp_cache_init(data_len,
                                                   hhrSdhStmBaseTable_get_data_from_ipc ,
                                                   hhrSdhStmBaseTable_node_lookup);

        if (hhrSdhStmBaseTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    stm_index.ifindex = index;
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmBaseTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, stm_info->ifindex);
    }

    switch (vp->magic)
    {
        case hhrSdhStmType:
            int_value = stm_info->stm_type;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmAlarm:
            str_value[0] = stm_info->stm_alarm;
            *var_len = 1;
            return str_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}

struct ipc_mesg_n *stm_rs_get_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_RS_GET, IPC_OPCODE_GET_BULK, 0);
#endif

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_RS_GET, IPC_OPCODE_GET_BULK, 0,1000);

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

int hhrSdhStmRSTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct stm_rs_info *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_rs_info *pdata = NULL;
    int data_len = sizeof(struct stm_rs_info);
    int data_num = 0;
    int i;

    pMsgRcv = stm_rs_get_bulk(index_input->ifindex, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_rs_info *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        
        return TRUE;
    }
}

struct stm_rs_info *hhrSdhStmRSTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct stm_rs_info  *index_input)
{
    struct listnode            *node;
    struct stm_rs_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL || (index_input->ifindex == 0))
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
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


u_char *hhrSdhStmRSTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact,
                             size_t *var_len,
                             WriteMethod **write_method)
{
    int ret = 0;
    u_int32_t             index = 0;
    u_int32_t             index_next  = 0;
    int                   data_len = sizeof(struct stm_rs_info);
    struct stm_rs_info    stm_index;
    struct stm_rs_info   *stm_info = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrSdhStmRSTable_cache == NULL)
    {
        hhrSdhStmRSTable_cache = snmp_cache_init(data_len,
                                                 hhrSdhStmRSTable_get_data_from_ipc ,
                                                 hhrSdhStmRSTable_node_lookup);

        if (hhrSdhStmRSTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    stm_index.ifindex = index;
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmRSTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, stm_info->ifindex);
    }

    memset(str_value, 0, sizeof(str_value));

    switch (vp->magic)
    {
        case hhrSdhStmRSJ0TRANSMIT:
            memcpy(str_value, stm_info->j0_tx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmRSJ0EXPECTED:
            memcpy(str_value, stm_info->j0_tx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmRSJ0RECEIVE:
            memcpy(str_value, stm_info->j0_rx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmRSEXCTHRESHOLD:
            int_value = stm_info->exc_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmRSDEGTHRESHOLD:
            int_value = stm_info->deg_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmRSB1COUNT:
            int_value = stm_info->b1;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmRSALARM:
            str_value[0] = stm_info->rs_alarm;
            *var_len = 1;
            return str_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}

struct ipc_mesg_n *stm_ms_get_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_MS_GET, IPC_OPCODE_GET_BULK, 0);
#endif

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex, sizeof(ifindex), 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_MS_GET, IPC_OPCODE_GET_BULK, 0,1000);

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

int hhrSdhStmMSTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct stm_ms_info *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_ms_info *pdata = NULL;
    int data_len = sizeof(struct stm_ms_info);
    int data_num = 0;
    int i;

    pMsgRcv = stm_ms_get_bulk(index_input->ifindex, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_ms_info *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
        
        return TRUE;
    }
}

struct stm_ms_info *hhrSdhStmMSTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct stm_ms_info  *index_input)
{
    struct listnode            *node;
    struct stm_ms_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL || (index_input->ifindex == 0))
        {
            return cache->data_list->head->data;
        }

        if (pdata->ifindex == index_input->ifindex)
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

u_char *hhrSdhStmMSTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact,
                             size_t *var_len,
                             WriteMethod **write_method)
{
    int ret = 0;
    u_int32_t             index = 0;
    u_int32_t             index_next  = 0;
    int                   data_len = sizeof(struct stm_ms_info);
    struct stm_ms_info    stm_index;
    struct stm_ms_info   *stm_info = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrSdhStmMSTable_cache == NULL)
    {
        hhrSdhStmMSTable_cache = snmp_cache_init(data_len,
                                                 hhrSdhStmMSTable_get_data_from_ipc ,
                                                 hhrSdhStmMSTable_node_lookup);

        if (hhrSdhStmMSTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    stm_index.ifindex = index;
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmMSTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, stm_info->ifindex);
    }

    memset(str_value, 0, sizeof(str_value));

    switch (vp->magic)
    {
        case hhrSdhStmMSK2RECEIVE:
            str_value[0] = stm_info->k2_rx;
            *var_len = 1;
            return str_value;

        case hhrSdhStmMSEXCTHRESHOLD:
            int_value = stm_info->exc_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSDEGTHRESHOLD:
            int_value = stm_info->deg_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSB2COUNT:
            int_value = stm_info->b2;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSREICOUNT:
            int_value = stm_info->rei_count;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSALARM:
            str_value[0] = stm_info->ms_alarm;
            *var_len = 1;
            return str_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}

struct ipc_mesg_n *stm_hp_get_bulk(struct stm_hp_index *key_index, int module_id, int *pdata_num)
{
    static unsigned int stm_hp_get_bulk_msg_index = 0;

#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(key_index, sizeof(struct stm_hp_index), 1, MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_HP_GET, IPC_OPCODE_GET_BULK, stm_hp_get_bulk_msg_index);
#endif
    
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(key_index, sizeof(struct stm_hp_index), 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_HP_GET, IPC_OPCODE_GET_BULK, 0,1000);
    stm_hp_get_bulk_msg_index++;
    
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

int hhrSdhStmHPTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct stm_hp_info *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_hp_info *pdata = NULL;
    struct stm_hp_index key_index;
    int data_len = sizeof(struct stm_hp_info);
    int data_num = 0;
    int i;

    key_index.ifindex = index_input->ifindex;
    key_index.vc4_id = index_input->vc4_id;
    pMsgRcv = stm_hp_get_bulk(&key_index, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_hp_info *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        return TRUE;
    }
}

struct stm_hp_info *hhrSdhStmHPTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct stm_hp_info  *index_input)
{
    struct listnode            *node;
    struct stm_hp_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if ((index_input->ifindex == 0) && (index_input->vc4_id == 0))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->ifindex == index_input->ifindex) && (pdata->vc4_id == index_input->vc4_id))
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

u_char *hhrSdhStmHPTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact,
                             size_t *var_len,
                             WriteMethod **write_method)
{
    int ret = 0;
    u_int32_t             if_index = 0;
    u_int32_t             vc4_index = 0;
    u_int32_t             index_next  = 0;
    int                   data_len = sizeof(struct stm_hp_info);
    struct stm_hp_info    stm_index;
    struct stm_hp_info   *stm_info = NULL;

    //ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);
    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &vc4_index, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrSdhStmHPTable_cache == NULL)
    {
        hhrSdhStmHPTable_cache = snmp_cache_init(data_len,
                                                 hhrSdhStmHPTable_get_data_from_ipc ,
                                                 hhrSdhStmHPTable_node_lookup);

        if (hhrSdhStmHPTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    stm_index.ifindex = if_index;
    stm_index.vc4_id = vc4_index;
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmHPTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        //ipran_snmp_int_index_set(vp, name, length, stm_info->ifindex);
        ipran_snmp_intx2_index_set(vp, name, length, stm_info->ifindex, stm_info->vc4_id);
    }

    memset(str_value, 0, sizeof(str_value));

    switch (vp->magic)
    {
        case hhrSdhStmHPVC4INDEX:
            int_value = stm_info->vc4_id;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPJ1TRANSMIT:
            memcpy(str_value, stm_info->j1_tx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmHPJ1EXPECTED:
            memcpy(str_value, stm_info->j1_tx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmHPJ1RECEIVE:
            memcpy(str_value, stm_info->j1_rx, STM_OVERHEAD_LEN);
            *var_len = STM_OVERHEAD_LEN;
            return str_value;

        case hhrSdhStmHPC2TRANSMIT:
            str_value[0] = stm_info->c2_tx;
            *var_len = 1;
            return str_value;

        case hhrSdhStmHPC2RECEIVE:
            str_value[0] = stm_info->c2_rx;
            *var_len = 1;
            return str_value;

        case hhrSdhStmHPG1RECEIVE:
            str_value[0] = stm_info->g1_rx;
            *var_len = 1;
            return str_value;

        case hhrSdhStmHPEXCTHRESHOLD:
            int_value = stm_info->exc_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPDEGTHRESHOLD:
            int_value = stm_info->deg_thr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPB3COUNT:
            int_value = stm_info->b3;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPREICOUNT:
            int_value = stm_info->rei_count;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPPLUCOUNT:
            int_value = stm_info->plu_count;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPNGACOUNT:
            int_value = stm_info->nga_count;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmHPALARM:
            str_value[0] = (stm_info->hp_alarm >> 8) & 0xff;
            str_value[1] = stm_info->hp_alarm & 0xff;
            *var_len = 2;
            return str_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}

struct ipc_mesg_n *stm_msp_get_bulk(uint16_t msp_id, int module_id, int *pdata_num)
{
    uint32_t msp_index = msp_id & 0xffff;

    #if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(&msp_index, sizeof(msp_index), 1 , MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_MSP_GET, IPC_OPCODE_GET_BULK, 0);
    #endif

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&msp_index, sizeof(msp_index), 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_MSP_GET, IPC_OPCODE_GET_BULK, 0,1000);  

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

int hhrSdhStmMSPTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                        struct stm_msp_config *index_input)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_msp_config *pdata = NULL;
    int data_len = sizeof(struct stm_msp_config);
    int data_num = 0;
    int i;

    pMsgRcv = stm_msp_get_bulk(index_input->msp_id, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_msp_config *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        return TRUE;
    }
}

struct stm_msp_config *hhrSdhStmMSPTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                     int exact,
                                                     const struct stm_msp_config  *index_input)
{
    struct listnode            *node;
    struct stm_msp_config *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL || (index_input->msp_id == 0))
        {
            return cache->data_list->head->data;
        }

        if (pdata->msp_id == index_input->msp_id)
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

u_char *hhrSdhStmMSPTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact,
                              size_t *var_len,
                              WriteMethod **write_method)
{
    int ret = 0;
    u_int32_t   index = 0;
    u_int32_t   index_next  = 0;
    int         data_len = sizeof(struct stm_msp_config);
    struct stm_msp_config    stm_index;
    struct stm_msp_config   *stm_info = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrSdhStmMSPTable_cache == NULL)
    {
        hhrSdhStmMSPTable_cache = snmp_cache_init(data_len,
                                                  hhrSdhStmMSPTable_get_data_from_ipc ,
                                                  hhrSdhStmMSPTable_node_lookup);

        if (hhrSdhStmMSPTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    stm_index.msp_id = index;
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmMSPTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, stm_info->msp_id);
    }

    memset(str_value, 0, sizeof(str_value));

    switch (vp->magic)
    {
        case hhrSdhStmMSPGROUPINDEX:
            int_value = stm_info->msp_id;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSPGROUPMODE:
            int_value = stm_info->mode ? 2 : 1;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSPGROUPWTR:
            int_value = stm_info->wtr;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSPGROUPMASTERIFINDEX:
            int_value = stm_info->master_if;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSPGROUPBACKUPIFINDEX:
            int_value = stm_info->backup_if;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmMSPGROUPSTATUS:
            if (stm_info->status == 0)
            {
                int_value = 1;
            }
            else if (stm_info->status == 1)
            {
                int_value = 2;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}
struct stm_dxc_config *hhrSdhStmSNCPTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct stm_dxc_config  *index_input)
{
    struct listnode            *node;
    struct stm_dxc_config *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (pdata == NULL || node == NULL)
        {
            return NULL;
        }

        if (index_input == NULL || (index_input->name[0] == NULL))
        {
            return cache->data_list->head->data;
        }

        if (0 == strcmp(pdata->name, index_input->name))
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

struct ipc_mesg_n *stm_sncp_get_bulk(uchar *dxcname, int module_id, int *pdata_num)
{
    int name_len = strlen((void *)dxcname);

    if (name_len > 0)
    {
        name_len += 1;
    }

#if 0
    struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(dxcname, name_len, 1 , MODULE_ID_CES, module_id,
                                                         IPC_TYPE_STM, STM_SNMP_TYPE_SNCP_GET, IPC_OPCODE_GET_BULK, 0);
#endif

    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(dxcname, name_len, 1 , MODULE_ID_CES, module_id,
                                    IPC_TYPE_STM, STM_SNMP_TYPE_SNCP_GET, IPC_OPCODE_GET_BULK, 0,1000); 

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

int hhrSdhStmSNCPTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct stm_dxc_config *dxc)
{
    struct ipc_mesg_n *pMsgRcv = NULL;
    struct stm_dxc_config *pdata = NULL;
    int data_len = sizeof(struct stm_dxc_config);
    int data_num = 0;
    int i;

    pMsgRcv = stm_sncp_get_bulk(dxc->name, MODULE_ID_SNMPD, &data_num);

    if (pMsgRcv == NULL)
    {
        return FALSE;
    }
    else
    {
        pdata = (struct stm_dxc_config *)pMsgRcv->msg_data;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        
        mem_share_free(pMsgRcv, MODULE_ID_SNMPD);

        return TRUE;
    }
}

u_char *hhrSdhStmSNCPTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact,
                               size_t *var_len,
                               WriteMethod **write_method)
{
    int ret = 0;
    int         data_len = sizeof(struct stm_dxc_config);
    struct stm_dxc_config    stm_index;
    u_char      sncp_name[STM_TDM_NAME_LENGTH] = {0};
    struct stm_dxc_config   *stm_info = NULL;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sncp_name, STM_TDM_NAME_LENGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrSdhStmSNCPTable_cache == NULL)
    {
        hhrSdhStmSNCPTable_cache = snmp_cache_init(data_len,
                                                   hhrSdhStmSNCPTable_get_data_from_ipc ,
                                                   hhrSdhStmSNCPTable_node_lookup);

        if (hhrSdhStmSNCPTable_cache == NULL)
        {
            zlog_info("%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return (NULL);
        }
    }

    memset(&stm_index, 0, data_len);
    memcpy(&(stm_index.name), sncp_name, strlen(sncp_name));
    stm_info = snmp_cache_get_data_by_index(hhrSdhStmSNCPTable_cache, exact, &stm_index);

    if (stm_info == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_octstring_index_set(vp, name, length, stm_info->name, strlen(stm_info->name));

    }

    switch (vp->magic)
    {
        case hhrSdhStmSNCPGROUPNAME:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, stm_info->name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrSdhStmSNCPGROUPMODE:
            if (stm_info->mode == STM_NO_PT_MODE)
            {
                int_value = 1;
            }
            else if (stm_info->mode == STM_PT_MODE)
            {
                int_value = 2;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPLEVEL:
            int_value = 3;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCIFINDEX:
            int_value = stm_info->base_if.ifindex;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCVC4:
            int_value = stm_info->base_if.vc12_vc4;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCVC3:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCVC12:
            int_value = (stm_info->base_if.dxc_vc12 + 1);
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCBACKUPIFINDEX:
            int_value = stm_info->backup_if.ifindex;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCBACKUPVC4:
            if (stm_info->mode == STM_NO_PT_MODE)
            {
                int_value = 0;
            }
            else
            {
                int_value = stm_info->backup_if.vc12_vc4;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCBACKUPVC3:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSRCBACKUPVC12:
            if (stm_info->mode == STM_NO_PT_MODE)
            {
                int_value = 0;
            }
            else
            {
                int_value = (stm_info->backup_if.dxc_vc12 + 1);
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTIFINDEX:
            int_value = stm_info->dst_if.ifindex;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTVC4:
            if (!IFM_TYPE_IS_STM(stm_info->dst_if.ifindex))
            {
                int_value = 0;
            }
            else
            {
                int_value = stm_info->dst_if.vc12_vc4;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTVC3:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTVC12:
            if (!IFM_TYPE_IS_STM(stm_info->dst_if.ifindex))
            {
                int_value = 0;
            }
            else
            {
                int_value = (stm_info->dst_if.dxc_vc12 + 1);
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTBACKUPIFINDEX:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTBACKUPVC4:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTBACKUPVC3:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPDSTBACKUPVC12:
            int_value = 0;
            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPWTRMODE:
            if (stm_info->mode == STM_PT_MODE)
            {
                int_value = stm_info->failback;
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPWTR:
            if (stm_info->mode == STM_PT_MODE)
            {
                if (stm_info->failback == STM_PT_RECOVER)
                {
                    int_value = stm_info->wtr;
                }
                else
                {
                    int_value = 0;
                }
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPSTATUS:
            if (stm_info->mode == STM_PT_MODE)
            {
                if (stm_info->status == 0)
                {
                    int_value = 1;
                }
                else if (stm_info->status == 1)
                {
                    int_value = 2;
                }
            }
            else
            {
                int_value = 0;
            }

            *var_len = sizeof(int_value);
            return (u_char *)&int_value;

        case hhrSdhStmSNCPGROUPNUM:
            *var_len = sizeof(int_value);
            int_value = stm_info->num;
            return (u_char *)&int_value;

        default:
            zlog_err("%s:%s[%d] No matched items!\n", __FILE__, __FUNCTION__, __LINE__);
            return NULL;
    }

    return NULL;
}

