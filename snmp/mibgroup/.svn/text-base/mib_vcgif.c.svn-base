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
#include "snmpd.h"

#include <lib/msg_ipc_n.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/devm_com.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/if.h>
#include <lib/sdh_vcg_common.h>

#include "mib_vcgif.h"

typedef struct
{
    int                         direct;
    t_sdh_vcg_interface         vcgif;
}   t_sdh_vcg_bind_info;


/*local temp variable*/
static uchar            str_value[STRING_LEN] = {'\0'};
static int              int_value = 0;
static uint32_t         uint_value = 0;
static struct counter64 uint64_value;

/*
 * Object ID definitions
 */
#define HHRSDHVCGCOMMON_OID             1,3,6,1,4,1,9966,5,38

/*hhrSdhVcgConfigureTable*/
#define hhrSdhVcgEncapsulateType        1
#define hhrSdhVcgEncapsulateProtocol    2
#define hhrsDhVcgJxTransmit             3
#define hhrSdhVcgJxExcept               4
#define hhrSdhVcgLcasSupport            5
#define hhrSdhVcgLcasEnble              6
#define hhrSdhVcgScrambling             7
#define hhrSdhVcgFcsInsertEnable        8
#define hhrSdhVcgVcgStatus              9
#define hhrSdhVcgVlanSupport           10
#define hhrSdhVcgVlanID                11
#define hhrSdhVcgVlanPri               12
#define hhrSdhVcgVlanTPID              13
#define hhrSdhVcgVlanUpKeep            14
#define hhrSdhVcgVlanDnKeep            15
#define hhrSdhVcgWorkLevel             16
#define hhrSdhVcgSupportLevel          17

static struct ipran_snmp_data_cache *hhrSdhVcgConfigureTable_cache = NULL;
static oid hhrSdhVcgConfigureTable_oid[] = {HHRSDHVCGCOMMON_OID, 1};
FindVarMethod hhrSdhVcgConfigureTable_get;
struct variable2 hhrSdhVcgConfigureTable_variables[] =
{
    {hhrSdhVcgEncapsulateType,     ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  1}},
    {hhrSdhVcgEncapsulateProtocol, ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  2}},
    {hhrsDhVcgJxTransmit,          ASN_OCTET_STR, RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  3}},
    {hhrSdhVcgJxExcept,            ASN_OCTET_STR, RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  4}},
    {hhrSdhVcgLcasSupport,         ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  5}},
    {hhrSdhVcgLcasEnble,           ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  6}},
    {hhrSdhVcgScrambling,          ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  7}},
    {hhrSdhVcgFcsInsertEnable,     ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  8}},
    {hhrSdhVcgVcgStatus,           ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1,  9}},
    {hhrSdhVcgVlanSupport,         ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 10}},
    {hhrSdhVcgVlanID,              ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 11}},
    {hhrSdhVcgVlanPri,             ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 12}},
    {hhrSdhVcgVlanTPID,            ASN_OCTET_STR, RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 13}},
    {hhrSdhVcgVlanUpKeep,          ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 14}},
    {hhrSdhVcgVlanDnKeep,          ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 15}},
    {hhrSdhVcgWorkLevel,           ASN_INTEGER,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 16}},
    {hhrSdhVcgSupportLevel,        ASN_OCTET_STR,   RONLY, hhrSdhVcgConfigureTable_get, 2, {1, 17}}
};

/*hhrSdhVcgBindCfgTabel*/
#define hhrSdhVcgBindLevel              1
#define hhrSdhVcgBindVc4StartNo         2
#define hhrSdhVcgBindLPStartNo          3
#define hhrSdhVcgBindNum                4

static struct ipran_snmp_data_cache *hhrSdhVcgBindCfgTabel_cache = NULL;
static oid hhrSdhVcgBindCfgTabel_oid[] = {HHRSDHVCGCOMMON_OID, 2};
FindVarMethod hhrSdhVcgBindCfgTabel_get;
struct variable2 hhrSdhVcgBindCfgTabel_variables[] =
{
    {hhrSdhVcgBindLevel,           ASN_INTEGER,   RONLY, hhrSdhVcgBindCfgTabel_get, 2, {1, 2}},
    {hhrSdhVcgBindVc4StartNo,      ASN_UNSIGNED,  RONLY, hhrSdhVcgBindCfgTabel_get, 2, {1, 3}},
    {hhrSdhVcgBindLPStartNo,       ASN_UNSIGNED,  RONLY, hhrSdhVcgBindCfgTabel_get, 2, {1, 4}},
    {hhrSdhVcgBindNum,             ASN_UNSIGNED,  RONLY, hhrSdhVcgBindCfgTabel_get, 2, {1, 5}}
};

/*hhrSdhVcgResv*/
#define hhrSdhVcgResv1                  1
#define hhrSdhVcgResv2                  2
#define hhrSdhVcgResv3                  3

static oid hhrSdhVcgResv_oid[] = {HHRSDHVCGCOMMON_OID};
FindVarMethod hhrSdhVcgResv_get;
struct variable1 hhrSdhVcgResv_variables[] =
{
    {hhrSdhVcgResv1,       ASN_INTEGER,   RONLY, hhrSdhVcgResv_get, 1, { 4}},
    {hhrSdhVcgResv2,       ASN_INTEGER,   RONLY, hhrSdhVcgResv_get, 1, { 7}},
    {hhrSdhVcgResv3,       ASN_INTEGER,   RONLY, hhrSdhVcgResv_get, 1, {10}}
};

/**/
#define hhrSdhVcgLevelNumber            1   // hhrSdhVcgOHJxTable
#define hhrSdhVcgReceive                2
#define hhrSdhVcgTIM                    3
#define hhrSdhVcgLcasLevel              4   // hhrSdhVcgLcasStatusTable
#define hhrSdhVcgLcasStatus             5
#define hhrSdhVcgLcasSq                 6
#define hhrSdhVcgOHStatusLevel          7   // hhrSdhVcgOHStatusTable
#define hhrSdhVcgOHStatusC2             8
#define hhrSdhVcgOHStatusG1             9
#define hhrSdhVcgOHStatusF2            10
#define hhrSdhVcgOHStatusK3            11
#define hhrSdhVcgOHStatusF3            12
#define hhrSdhVcgOHStatusN1            13
#define hhrSdhVcgB3OrBipLevel          14   // hhrSdhVcgb3AndBipStatisticsTable
#define hhrSdhVcgB3OrBipTotal          15
#define hhrSdhVcgLpStatusLevel         16   // hhrSdhVcgLpStatusTable
#define hhrSdhVcgLpStatusAlarm         17

static struct ipran_snmp_data_cache *hhrSdhVcgStatus_cache = NULL;
static oid hhrSdhVcgOHJxTable_oid[] = {HHRSDHVCGCOMMON_OID, 3};
FindVarMethod hhrSdhVcgStatus_get;
struct variable2 hhrSdhVcgOHJxTable_variables[] =
{
    {hhrSdhVcgLevelNumber, ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 3}},
    {hhrSdhVcgReceive,     ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 4}},
    {hhrSdhVcgTIM,         ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 5}}
};

static oid hhrSdhVcgLcasStatusTable_oid[] = {HHRSDHVCGCOMMON_OID, 5};
struct variable2 hhrSdhVcgLcasStatusTable_variables[] =
{
    {hhrSdhVcgLcasLevel,   ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 1}},
    {hhrSdhVcgLcasStatus,  ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 2}},
    {hhrSdhVcgLcasSq,      ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 3}}
};

static oid hhrSdhVcgOHStatusTable_oid[] = {HHRSDHVCGCOMMON_OID, 6};
struct variable2 hhrSdhVcgOHStatusTable_variables[] =
{
    {hhrSdhVcgOHStatusLevel, ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 1}},
    {hhrSdhVcgOHStatusC2,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 2}},
    {hhrSdhVcgOHStatusG1,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 3}},
    {hhrSdhVcgOHStatusF2,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 4}},
    {hhrSdhVcgOHStatusK3,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 5}},
    {hhrSdhVcgOHStatusF3,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 6}},
    {hhrSdhVcgOHStatusN1,    ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 7}}
};

static oid hhrSdhVcgb3AndBipStatisticsTable_oid[] = {HHRSDHVCGCOMMON_OID, 8};
struct variable2 hhrSdhVcgb3AndBipStatisticsTable_variables[] =
{
    {hhrSdhVcgB3OrBipLevel, ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 1}},
    {hhrSdhVcgB3OrBipTotal, ASN_COUNTER64, RONLY, hhrSdhVcgStatus_get, 2, {1, 2}},
};

static oid hhrSdhVcgLpStatusTable_oid[] = {HHRSDHVCGCOMMON_OID, 11};
struct variable2 hhrSdhVcgLpStatusTable_variables[] =
{
    {hhrSdhVcgLpStatusLevel, ASN_INTEGER,   RONLY, hhrSdhVcgStatus_get, 2, {1, 1}},
    {hhrSdhVcgLpStatusAlarm, ASN_OCTET_STR, RONLY, hhrSdhVcgStatus_get, 2, {1, 2}}
};


/*hhrSdhVcgStatisticsTable*/
#define hhrSdhVcgRxTotalStatistics      1
#define hhrSdhVcgTxTotalStatistics      2
#define hhrSdhVcgGfpLosTotalStatistics  3

static struct ipran_snmp_data_cache *hhrSdhVcgStatisticsTable_cache = NULL;
static oid hhrSdhVcgStatisticsTable_oid[] = {HHRSDHVCGCOMMON_OID, 9};
FindVarMethod hhrSdhVcgStatisticsTable_get;
struct variable2 hhrSdhVcgStatisticsTable_variables[] =
{
    {hhrSdhVcgRxTotalStatistics,     ASN_COUNTER64, RONLY, hhrSdhVcgStatisticsTable_get, 2, {1, 1}},
    {hhrSdhVcgTxTotalStatistics,     ASN_COUNTER64, RONLY, hhrSdhVcgStatisticsTable_get, 2, {1, 2}},
    {hhrSdhVcgGfpLosTotalStatistics, ASN_COUNTER64, RONLY, hhrSdhVcgStatisticsTable_get, 2, {1, 3}}
};

void init_mib_vcgif(void)
{
    if (gDevTypeID != ID_H9MOLMXE_VX)
    {
        return;
    }

    REGISTER_MIB("hhrSdhVcgConfigureTable", hhrSdhVcgConfigureTable_variables, variable2, hhrSdhVcgConfigureTable_oid);
    REGISTER_MIB("hhrSdhVcgBindCfgTabel", hhrSdhVcgBindCfgTabel_variables, variable2, hhrSdhVcgBindCfgTabel_oid);
    REGISTER_MIB("hhrSdhVcgOHJxTable", hhrSdhVcgOHJxTable_variables, variable2, hhrSdhVcgOHJxTable_oid);
    REGISTER_MIB("hhrSdhVcgResv", hhrSdhVcgResv_variables, variable1, hhrSdhVcgResv_oid);
    REGISTER_MIB("hhrSdhVcgLcasStatusTable", hhrSdhVcgLcasStatusTable_variables, variable2, hhrSdhVcgLcasStatusTable_oid);
    REGISTER_MIB("hhrSdhVcgOHStatusTable", hhrSdhVcgOHStatusTable_variables, variable2, hhrSdhVcgOHStatusTable_oid);
    REGISTER_MIB("hhrSdhVcgb3AndBipStatisticsTable", hhrSdhVcgb3AndBipStatisticsTable_variables, variable2, hhrSdhVcgb3AndBipStatisticsTable_oid);
    REGISTER_MIB("hhrSdhVcgStatisticsTable", hhrSdhVcgStatisticsTable_variables, variable2, hhrSdhVcgStatisticsTable_oid);
    REGISTER_MIB("hhrSdhVcgLpStatusTable", hhrSdhVcgLpStatusTable_variables, variable2, hhrSdhVcgLpStatusTable_oid);
}

/*** hhrSdhVcgConfigureTable start **************************************************/
int hhrSdhVcgConfigureTable_data_get_from_ipc(struct ipran_snmp_data_cache *cache,
                                              t_sdh_vcg_interface *index_input, int exact)
{
    struct ipc_mesg_n *pmesg;
    t_sdh_vcg_interface_getbulk_req_msg req_msg;
    t_sdh_vcg_interface *pdata = NULL;
    int data_len = sizeof(t_sdh_vcg_interface);
    int data_num = 0;
    int i;

    if (NULL == index_input || NULL == cache)
    {
        return FALSE;
    }

    memset(&req_msg, 0, sizeof(t_sdh_vcg_interface_getbulk_req_msg));
    memcpy(&req_msg.msg, index_input, data_len);
    req_msg.req_flag = snmp_req_msg_flag(cache->nullkey, exact);

    pmesg = ipc_sync_send_n2(&req_msg, data_len + 4, 1, MODULE_ID_VCG, MODULE_ID_SNMPD,
                                        IPC_TYPE_VCG, VCG_GET_CONFIG,
                                        IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num = pmesg->msghdr.data_num;
    pdata    = (t_sdh_vcg_interface *)(pmesg->msg_data);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

        for (i = 0; i < data_num; i++)
        {
            snmp_cache_valfilter_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

t_sdh_vcg_interface *hhrSdhVcgConfigureTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                         int exact,
                                                         const t_sdh_vcg_interface *index_input)
{
    struct listnode     *node;
    t_sdh_vcg_interface *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 1 == cache->nullkey)
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

int hhrSdhVcgConfigureTable_cmp(t_sdh_vcg_info *val1, t_sdh_vcg_info *val2)
{
    if (val1->ifindex < val2->ifindex)
    {
        return -1;
    }
    else if (val1->ifindex > val2->ifindex)
    {
        return 1;
    }

    return 0;
}

u_char *hhrSdhVcgConfigureTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    t_sdh_vcg_interface         index;
    t_sdh_vcg_interface        *pdata;
    int                         data_len = sizeof(t_sdh_vcg_interface);
    u_int32_t                   ifindex = 0;
    int                         nullkey = 0;    // 1:null index, -1:error, 0:have index
	u_int8_t                    support_level = 0;	
    *write_method = NULL;

    /* validate the index */
    nullkey = ipran_snmp_int_index_get(vp, name, length, &ifindex, exact);

    if (nullkey < 0)
    {
        return NULL;
    }

    if (NULL == hhrSdhVcgConfigureTable_cache)
    {
        hhrSdhVcgConfigureTable_cache = snmp_cache_init2(data_len,
                                                         hhrSdhVcgConfigureTable_data_get_from_ipc,
                                                         hhrSdhVcgConfigureTable_node_lookup,
                                                         hhrSdhVcgConfigureTable_cmp);

        if (NULL == hhrSdhVcgConfigureTable_cache)
        {
            return NULL;
        }
    }

    snmp_cache_nullkey_set(hhrSdhVcgConfigureTable_cache, nullkey);

    memset(&index, 0, data_len);
    index.ifindex  = ifindex;

    pdata = snmp_cache_data_get(hhrSdhVcgConfigureTable_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrSdhVcgEncapsulateType:
            *var_len = sizeof(int);

            if (SDH_VCG_ENCAP_TYPE_EOS == pdata->config.type)
            {
                int_value = 1;
            }
            else if (SDH_VCG_ENCAP_TYPE_EOE == pdata->config.type)
            {
                int_value = 3;
            }
            else if (SDH_VCG_ENCAP_TYPE_EOP == pdata->config.type)
            {
                int_value = 2;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgEncapsulateProtocol:
            *var_len = sizeof(int);

            if (SDH_VCG_ENCAP_PROTOCOL_HDLC == pdata->config.protocol)
            {
                int_value = 2;
            }
            else if (SDH_VCG_ENCAP_PROTOCOL_GFPF == pdata->config.protocol)
            {
                int_value = 1;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrsDhVcgJxTransmit:
            memset(str_value, 0x00, STRING_LEN);

            if (0x80 & pdata->config.tra_jx[0])
            {
                memcpy(str_value, pdata->config.tra_jx + 1, SDH_OH_JX_LEN - 1);
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrSdhVcgJxExcept:
            memset(str_value, 0x00, STRING_LEN);

            if (0x80 & pdata->config.exp_jx[0])
            {
                memcpy(str_value, pdata->config.exp_jx + 1, SDH_OH_JX_LEN - 1);
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrSdhVcgLcasSupport:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_LCAS & pdata->config.support)
            {
                int_value = 1;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgLcasEnble:
            *var_len = sizeof(int);

            if (1 == pdata->config.lcas_enable)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgScrambling:
            *var_len = sizeof(int);

            if (SDH_VCG_SCRAMBLING_UP == pdata->config.scrambling)
            {
                int_value = 1;
            }
            else if (SDH_VCG_SCRAMBLING_DOWN == pdata->config.scrambling)
            {
                int_value = 2;
            }
            else if (SDH_VCG_SCRAMBLING_BOTH == pdata->config.scrambling)
            {
                int_value = 3;
            }
            else if (SDH_VCG_SCRAMBLING_NONE == pdata->config.scrambling)
            {
                int_value = 4;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgFcsInsertEnable:
            *var_len = sizeof(int);

            if (1 == pdata->config.fcs_insert)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVcgStatus:
            *var_len = sizeof(int);

            if (0 == pdata->config.binding_down.num)
            {
                int_value = 0;
            }
            else
            {
                if (1 == pdata->link_status)
                {
                    int_value = 1;
                }
                else
                {
                    int_value = 2;
                }
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVlanSupport:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                int_value = 1;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVlanID:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                int_value = pdata->config.vid;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVlanPri:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                int_value = pdata->config.cos;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVlanTPID:
            memset(str_value, 0x00, STRING_LEN);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                sprintf(str_value, "0x%04x", pdata->config.tpid);
                *var_len = strlen(str_value);
            }
            else
            {
                *var_len = 0;
            }

            return str_value;

        case hhrSdhVcgVlanUpKeep:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                int_value = (1 == pdata->config.upvlan_ena) ? 0 : 1;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;

        case hhrSdhVcgVlanDnKeep:
            *var_len = sizeof(int);

            if (SDH_VCG_SUPPORT_VLAN & pdata->config.support)
            {
                int_value = (1 == pdata->config.downvlan_ena) ? 0 : 1;
            }
            else
            {
                int_value = 0;
            }

            return (u_char *)&int_value;
         
		case hhrSdhVcgWorkLevel:
			*var_len = sizeof(int);

		    if(pdata->image_level != pdata->config.level)
		    {
               int_value = 3;
			}
			else{
               int_value = pdata->config.level;
			}
		   
			return (u_char *)&int_value; 

		case hhrSdhVcgSupportLevel:   	      
			 memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
		     if(SDH_VCG_SUPPORT_LEVEL_VC4 & pdata->config.support)
		     {
                support_level |= (1 << 7);
			 }

			 if(SDH_VCG_SUPPORT_LEVEL_VC3 & pdata->config.support)
		     {
                support_level |= (1 << 6);
			 }

			 if(SDH_VCG_SUPPORT_LEVEL_VC12 & pdata->config.support)
		     {
                support_level |= (1 << 5);
			 }
		 
             str_value[0] = support_level;
             return str_value;
     
			 
        default:
            return NULL;
    }

    return NULL;
}

/*** hhrSdhVcgBindCfgTabel start **************************************************/
int hhrSdhVcgBindCfgTabel_data_get_from_ipc(struct ipran_snmp_data_cache *cache,
                                            t_sdh_vcg_bind_info *index_input, int exact)
{
    struct ipc_mesg_n *pmesg;
    t_sdh_vcg_interface_getbulk_req_msg req_msg;
    t_sdh_vcg_interface *pdata = NULL;
    t_sdh_vcg_bind_info temp;
    int data_len1 = sizeof(t_sdh_vcg_interface);
    int data_len2 = sizeof(t_sdh_vcg_bind_info);
    int data_num = 0;
    int i;

    if (NULL == index_input || NULL == cache)
    {
        return FALSE;
    }

    data_len1 = sizeof(t_sdh_vcg_interface);

    memset(&req_msg, 0, sizeof(t_sdh_vcg_interface_getbulk_req_msg));
    memcpy(&req_msg.msg, &index_input->vcgif, data_len1);
    req_msg.req_flag = snmp_req_msg_flag(cache->nullkey, exact);

    pmesg = ipc_sync_send_n2(&req_msg, data_len1 + 4, 1, MODULE_ID_VCG, MODULE_ID_SNMPD,
                                        IPC_TYPE_VCG, VCG_GET_CONFIG,
                                        IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num = pmesg->msghdr.data_num;
    pdata    = (t_sdh_vcg_interface *)(pmesg->msg_data);

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

        for (i = 0; i < data_num; i++)
        {
            memset(&temp, 0x00, data_len2);
            memcpy(&temp.vcgif, pdata, data_len1);

            if (0 != pdata->config.binding_up.num)
            {
                temp.direct = 1;
                snmp_cache_valfilter_add(cache, &temp, data_len2);
            }

            if (0 != pdata->config.binding_down.num)
            {
                temp.direct = 2;
                snmp_cache_valfilter_add(cache, &temp, data_len2);
            }

            pdata++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

t_sdh_vcg_bind_info *hhrSdhVcgBindCfgTabel_node_lookup(struct ipran_snmp_data_cache *cache,
                                                       int exact,
                                                       const t_sdh_vcg_bind_info *index_input)
{
    struct listnode     *node;
    t_sdh_vcg_bind_info *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (1 == exact)
        {
            if (pdata->vcgif.ifindex == index_input->vcgif.ifindex && pdata->direct == index_input->direct)
            {
                return pdata;
            }
        }
        else
        {
            if (pdata->vcgif.ifindex > index_input->vcgif.ifindex)
            {
                return pdata;
            }
            else if (pdata->vcgif.ifindex == index_input->vcgif.ifindex && pdata->direct > index_input->direct)
            {
                return pdata;
            }
            else if (pdata->vcgif.ifindex == index_input->vcgif.ifindex && pdata->direct == index_input->direct)
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

int hhrSdhVcgBindCfgTabel_cmp(t_sdh_vcg_bind_info *val1, t_sdh_vcg_bind_info *val2)
{
    if (val1->vcgif.ifindex < val2->vcgif.ifindex)
    {
        return -1;
    }
    else if (val1->vcgif.ifindex > val2->vcgif.ifindex)
    {
        return 1;
    }
    else
    {
        if (val1->direct < val2->direct)
        {
            return -1;
        }
        else if (val1->direct > val2->direct)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return 0;
}

u_char *hhrSdhVcgBindCfgTabel_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    t_sdh_vcg_bind_info         index;
    t_sdh_vcg_bind_info        *pdata;
    int                         data_len = sizeof(t_sdh_vcg_bind_info);
    u_int32_t                   ifindex = 0;
    u_int32_t                   direct = 0;
    int                         nullkey = 0;    // 1:null index, -1:error, 0:have index

    *write_method = NULL;

    /* validate the index */
    nullkey = ipran_snmp_intx2_index_get(vp, name, length, &ifindex, &direct, exact);

    if (nullkey < 0)
    {
        return NULL;
    }

    if (NULL == hhrSdhVcgBindCfgTabel_cache)
    {
        hhrSdhVcgBindCfgTabel_cache = snmp_cache_init2(data_len,
                                                       hhrSdhVcgBindCfgTabel_data_get_from_ipc,
                                                       hhrSdhVcgBindCfgTabel_node_lookup,
                                                       hhrSdhVcgBindCfgTabel_cmp);

        if (NULL == hhrSdhVcgBindCfgTabel_cache)
        {
            return NULL;
        }
    }

    snmp_cache_nullkey_set(hhrSdhVcgBindCfgTabel_cache, nullkey);

    memset(&index, 0, data_len);
    index.vcgif.ifindex = ifindex;
    index.direct  = direct;

    pdata = snmp_cache_data_get(hhrSdhVcgBindCfgTabel_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->vcgif.ifindex, pdata->direct);
    }

    switch (vp->magic)
    {
        case hhrSdhVcgBindLevel:
            *var_len = sizeof(int);
            int_value = pdata->vcgif.config.level;
            return (u_char *)&int_value;

        case hhrSdhVcgBindVc4StartNo:
            *var_len = sizeof(uint32_t);

            if (1 == pdata->direct)
            {
                uint_value = pdata->vcgif.config.binding_up.vc4;
            }
            else
            {
                uint_value = pdata->vcgif.config.binding_down.vc4;
            }

            return (u_char *)&uint_value;

        case hhrSdhVcgBindLPStartNo:
            *var_len = sizeof(uint32_t);

            if (1 == pdata->direct)
            {
                uint_value = pdata->vcgif.config.binding_up.vc3_vc12;
            }
            else
            {
                uint_value = pdata->vcgif.config.binding_down.vc3_vc12;
            }

            return (u_char *)&uint_value;

        case hhrSdhVcgBindNum:
            *var_len = sizeof(uint32_t);

            if (1 == pdata->direct)
            {
                uint_value = pdata->vcgif.config.binding_up.num;
            }
            else
            {
                uint_value = pdata->vcgif.config.binding_down.num;
            }

            return (u_char *)&uint_value;

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrSdhVcgOHJxTable start **************************************************/
int hhrSdhVcgStatus_data_get_from_ipc(struct ipran_snmp_data_cache *cache,
                                      t_sdh_vcg_info *index_input, int exact)
{
    struct ipc_mesg_n *pmesg;
    t_sdh_vcg_info_getbulk_req_msg req_msg;
    t_sdh_vcg_info *pdata = NULL;
    int data_len = sizeof(t_sdh_vcg_info);
    int data_num = 0;
    int i;

    if (NULL == index_input || NULL == cache)
    {
        return FALSE;
    }

    memset(&req_msg, 0, sizeof(t_sdh_vcg_info_getbulk_req_msg));
    memcpy(&req_msg.msg, index_input, data_len);
    req_msg.req_flag = snmp_req_msg_flag(cache->nullkey, exact);

    pmesg = ipc_sync_send_n2(&req_msg, data_len + 4, 1, MODULE_ID_HAL, MODULE_ID_SNMPD,
                                     IPC_TYPE_VCG, VCG_GET_VCINFO,
                                     IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num = pmesg->msghdr.data_num;
    pdata    = (t_sdh_vcg_info *)pmesg->msg_data;

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

        for (i = 0; i < data_num; i++)
        {
            snmp_cache_valfilter_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

t_sdh_vcg_info *hhrSdhVcgStatus_node_lookup(struct ipran_snmp_data_cache *cache,
                                            int exact,
                                            const t_sdh_vcg_info *index_input)
{
    struct listnode *node;
    t_sdh_vcg_info  *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 1 == cache->nullkey)
        {
            return cache->data_list->head->data;
        }

        if (1 == exact)
        {
            if ((pdata->ifindex == index_input->ifindex) \
                    && (pdata->vc4 == index_input->vc4) \
                    && (pdata->vc3_vc12 == index_input->vc3_vc12))
            {
                return pdata;
            }
        }
        else
        {
            if (pdata->ifindex > index_input->ifindex)
            {
                return pdata;
            }
            else if ((pdata->ifindex == index_input->ifindex) \
                     && (pdata->vc4 > index_input->vc4))
            {
                return pdata;
            }
            else if ((pdata->ifindex == index_input->ifindex) \
                     && (pdata->vc4 == index_input->vc4) \
                     && (pdata->vc3_vc12 > index_input->vc3_vc12))
            {
                return pdata;
            }
            else if ((pdata->ifindex == index_input->ifindex) \
                     && (pdata->vc4 == index_input->vc4) \
                     && (pdata->vc3_vc12 == index_input->vc3_vc12))
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

int hhrSdhVcgStatus_cmp(t_sdh_vcg_info *val1, t_sdh_vcg_info *val2)
{
    if (val1->ifindex < val2->ifindex)
    {
        return -1;
    }
    else if (val1->ifindex > val2->ifindex)
    {
        return 1;
    }
    else
    {
        if (val1->vc4 < val2->vc4)
        {
            return -1;
        }
        else if (val1->vc4 > val2->vc4)
        {
            return 1;
        }
        else
        {
            if (val1->vc3_vc12 < val2->vc3_vc12)
            {
                return -1;
            }
            else if (val1->vc3_vc12 > val2->vc3_vc12)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;
}

u_char *hhrSdhVcgStatus_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    t_sdh_vcg_info        index;
    t_sdh_vcg_info       *pdata;
    int                   data_len = sizeof(t_sdh_vcg_info);
    u_int32_t             ifindex = 0;
    u_int32_t             vc4 = 0;
    u_int32_t             vc3_vc12 = 0;
    int                   nullkey = 0;    // 1:null index, -1:error, 0:have index

    *write_method = NULL;

    /* validate the index */
    nullkey = ipran_snmp_intx3_index_get(vp, name, length, &ifindex, &vc4, &vc3_vc12, exact);

    if (nullkey < 0)
    {
        return NULL;
    }

    if (NULL == hhrSdhVcgStatus_cache)
    {
        hhrSdhVcgStatus_cache = snmp_cache_init2(data_len,
                                                 hhrSdhVcgStatus_data_get_from_ipc,
                                                 hhrSdhVcgStatus_node_lookup,
                                                 hhrSdhVcgStatus_cmp);

        if (NULL == hhrSdhVcgStatus_cache)
        {
            return NULL;
        }
    }

    snmp_cache_nullkey_set(hhrSdhVcgStatus_cache, nullkey);

    memset(&index, 0, data_len);
    index.ifindex  = ifindex;
    index.vc4      = vc4;
    index.vc3_vc12 = vc3_vc12;

    pdata = snmp_cache_data_get(hhrSdhVcgStatus_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_intx3_index_set(vp, name, length, pdata->ifindex, pdata->vc4, pdata->vc3_vc12);
    }

    switch (vp->magic)
    {
        case hhrSdhVcgLevelNumber:
            *var_len = sizeof(int);
            int_value = pdata->info.level;
            return (u_char *)&int_value;

        case hhrSdhVcgReceive:
            memset(str_value, 0x00, STRING_LEN);

            if (0x80 & pdata->info.rcv_jx[0])
            {
                memcpy(str_value, pdata->info.rcv_jx + 1, SDH_OH_JX_LEN - 1);
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrSdhVcgTIM:
            *var_len = sizeof(int);
            int_value = (pdata->info.alarm & 0x20) ? 2 : 1;
            return (u_char *)&int_value;

        case hhrSdhVcgLcasLevel:
            *var_len = sizeof(int);
            int_value = pdata->info.level;
            return (u_char *)&int_value;

        case hhrSdhVcgLcasStatus:
            *var_len = sizeof(int);
            int_value = pdata->info.lcas_status.status;
            return (u_char *)&int_value;

        case hhrSdhVcgLcasSq:
            *var_len = sizeof(int);
            int_value = pdata->info.lcas_status.sq;
            return (u_char *)&int_value;

        case hhrSdhVcgOHStatusLevel:
            *var_len = sizeof(int);
            int_value = pdata->info.level;
            return (u_char *)&int_value;

        case hhrSdhVcgOHStatusC2:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.c2;
            return str_value;

        case hhrSdhVcgOHStatusG1:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.g1;
            return str_value;

        case hhrSdhVcgOHStatusF2:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.f2;
            return str_value;

        case hhrSdhVcgOHStatusK3:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.k3;
            return str_value;

        case hhrSdhVcgOHStatusF3:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.f3;
            return str_value;

        case hhrSdhVcgOHStatusN1:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.oh_hp_rx.n1;
            return str_value;

        case hhrSdhVcgB3OrBipLevel:
            *var_len = sizeof(int);
            int_value = pdata->info.level;
            return (u_char *)&int_value;

        case hhrSdhVcgB3OrBipTotal:
            uint64_value.high = (pdata->info.biporb3_total >> 32);
            uint64_value.low  = (pdata->info.biporb3_total);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case hhrSdhVcgLpStatusLevel:
            *var_len = sizeof(int);
            int_value = pdata->info.level;
            return (u_char *)&int_value;

        case hhrSdhVcgLpStatusAlarm:
            memset(str_value, 0x00, STRING_LEN);
            *var_len = 1;
            str_value[0] = pdata->info.alarm;
            return str_value;

        default:
            return NULL;
    }

    return NULL;
}

/*hhrSdhVcgResv*/
u_char *hhrSdhVcgResv_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    *write_method = NULL;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrSdhVcgResv1:
            *var_len = sizeof(int);
            int_value = 0;
            return (u_char *)&int_value;

        case hhrSdhVcgResv2:
            *var_len = sizeof(int);
            int_value = 0;
            return (u_char *)&int_value;

        case hhrSdhVcgResv3:
            *var_len = sizeof(int);
            int_value = 0;
            return (u_char *)&int_value;

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrSdhVcgStatisticsTable start **************************************************/
int hhrSdhVcgStatisticsTable_data_get_from_ipc(struct ipran_snmp_data_cache *cache,
                                               t_sdh_vcg_count *index_input, int exact)
{
    struct ipc_mesg_n *pmesg;
    t_sdh_vcg_count_getbulk_req_msg req_msg;
    t_sdh_vcg_count *pdata = NULL;
    int data_len = sizeof(t_sdh_vcg_count);
    int data_num = 0;
    int i;

    if (NULL == index_input || NULL == cache)
    {
        return FALSE;
    }

    memset(&req_msg, 0, sizeof(t_sdh_vcg_count_getbulk_req_msg));
    memcpy(&req_msg.msg, index_input, data_len);
    req_msg.req_flag = snmp_req_msg_flag(cache->nullkey, exact);

    pmesg = ipc_sync_send_n2(&req_msg, data_len + 4, 1, MODULE_ID_HAL, MODULE_ID_SNMPD,
                                     IPC_TYPE_VCG, VCG_GET_STATISTICS,
                                     IPC_OPCODE_GET_BULK, 0, 2000);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num = pmesg->msghdr.data_num;
    pdata    = (t_sdh_vcg_count *)pmesg->msg_data;

    if (0 == data_num)
    {
        return FALSE;
    }
    else
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

        for (i = 0; i < data_num; i++)
        {
            snmp_cache_valfilter_add(cache, pdata, data_len);
            pdata++;
        }

		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

t_sdh_vcg_count *hhrSdhVcgStatisticsTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                      int exact,
                                                      const t_sdh_vcg_count *index_input)
{
    struct listnode *node;
    t_sdh_vcg_count *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 1 == cache->nullkey)
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

int hhrSdhVcgStatisticsTable_cmp(t_sdh_vcg_count *val1, t_sdh_vcg_count *val2)
{
    if (val1->ifindex < val2->ifindex)
    {
        return -1;
    }
    else if (val1->ifindex > val2->ifindex)
    {
        return 1;
    }

    return 0;
}

u_char *hhrSdhVcgStatisticsTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    t_sdh_vcg_count             index;
    t_sdh_vcg_count            *pdata;
    int                         data_len = sizeof(t_sdh_vcg_count);
    u_int32_t                   ifindex = 0;
    int                         nullkey = 0;    // 1:null index, -1:error, 0:have index

    *write_method = NULL;

    /* validate the index */
    nullkey = ipran_snmp_int_index_get(vp, name, length, &ifindex, exact);

    if (nullkey < 0)
    {
        return NULL;
    }

    if (NULL == hhrSdhVcgStatisticsTable_cache)
    {
        hhrSdhVcgStatisticsTable_cache = snmp_cache_init2(data_len,
                                                          hhrSdhVcgStatisticsTable_data_get_from_ipc,
                                                          hhrSdhVcgStatisticsTable_node_lookup,
                                                          hhrSdhVcgStatisticsTable_cmp);

        if (NULL == hhrSdhVcgStatisticsTable_cache)
        {
            return NULL;
        }
    }

    snmp_cache_nullkey_set(hhrSdhVcgStatisticsTable_cache, nullkey);

    memset(&index, 0, data_len);
    index.ifindex  = ifindex;

    pdata = snmp_cache_data_get(hhrSdhVcgStatisticsTable_cache, exact, &index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrSdhVcgRxTotalStatistics:
            uint64_value.high = (pdata->cnt_total.rx_packet >> 32);
            uint64_value.low  = (pdata->cnt_total.rx_packet);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case hhrSdhVcgTxTotalStatistics:
            uint64_value.high = (pdata->cnt_total.tx_packet >> 32);
            uint64_value.low  = (pdata->cnt_total.tx_packet);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        case hhrSdhVcgGfpLosTotalStatistics:
            uint64_value.high = (pdata->cnt_total.gfp_los >> 32);
            uint64_value.low  = (pdata->cnt_total.gfp_los);
            *var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;

        default:
            return NULL;
    }

    return NULL;
}

