
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"
#include <lib/log.h>

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include "mib_synce.h"
#include "../../clock/clock_msg.h"
//#include "lib/snmp_common.h"
#include "snmp_config_table.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define SYNCE_OIDPREFIX 1,3,6,1,4,1,9966,5,35,8,11

/*local temp variable*/
static uchar str_value[STRING_LEN] = {'\0'};
static int   int_value = 0;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
struct synce_snmp_t *synce_snmp_global = NULL;


FindVarMethod hhr_synce_configglobal_get;
FindVarMethod hhr_synce_clk_if_table_get;
FindVarMethod hhr_synce_clk_pri_table_get;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/


struct variable2 synceConfGblVariables[] =
{
    {synceCfgMode,              ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {1}},
    {synceCfgSSMEnable,         ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {2}},
    {synceCfgSSMInterval,       ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {3}},
    {synceCfgPreemtEnable,      ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {4}},
    {synceCfgFailback,          ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {5}},
    {synceCfgHoldTime,          ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {6}},
    {synceCfgClockSource,       ASN_OCTET_STR,  RONLY, hhr_synce_configglobal_get, 1, {7}},
    {SyncECurrentClockState,    ASN_INTEGER,    RONLY, hhr_synce_configglobal_get, 1, {8}},
    {SyncECurrentFreqOffset,    ASN_OCTET_STR,  RONLY, hhr_synce_configglobal_get, 1, {9}},
} ;



struct variable2 synceClkIfTableVariables[] =
{
    {synceClkIfDescr,   ASN_OCTET_STR,  RONLY,  hhr_synce_clk_if_table_get, 1, {1}},
    {synceClkIfMode,    ASN_INTEGER,    RONLY,  hhr_synce_clk_if_table_get, 1, {2}},
    {synceClkIfSignal,  ASN_INTEGER,    RONLY,  hhr_synce_clk_if_table_get, 1, {3}},
    {synceClkIfQL,      ASN_INTEGER,    RONLY,  hhr_synce_clk_if_table_get, 1, {4}},
    {synceClkIfSA,      ASN_INTEGER,    RONLY,  hhr_synce_clk_if_table_get, 1, {5}},
} ;

struct variable2 synceClkPriVariables[] =
{
    {synceClkSrcDescr,      ASN_OCTET_STR,  RONLY,  hhr_synce_clk_pri_table_get, 1, {1}},
    {synceClkSrcPriority,   ASN_INTEGER,    RONLY,  hhr_synce_clk_pri_table_get, 1, {2}},
    {synceClkSrcValidState, ASN_INTEGER,    RONLY,  hhr_synce_clk_pri_table_get, 1, {3}},
    {synceClkSrcClockState, ASN_INTEGER,    RONLY,  hhr_synce_clk_pri_table_get, 1, {4}},
    {synceClkSrcQL        , ASN_INTEGER,    RONLY,  hhr_synce_clk_pri_table_get, 1, {5}},

} ;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

oid  synceConfGblOid[]   = { SYNCE_OIDPREFIX};

oid  synceClkIfTableOid[]   = { SYNCE_OIDPREFIX , 11 , 1};

oid  synceClkPriOid[]   = { SYNCE_OIDPREFIX , 12 , 1 };


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void  init_mib_synce(void)
{
    REGISTER_MIB("ipran/synceConfGbl", synceConfGblVariables, variable2, synceConfGblOid);

    REGISTER_MIB("ipran/synceClkIfTabel", synceClkIfTableVariables, variable2, synceClkIfTableOid);

    REGISTER_MIB("ipran/synceClkSrcPri", synceClkPriVariables, variable2, synceClkPriOid);

}
struct synce_snmp_t synce_glb_cfg_common;

struct synce_snmp_t *synce_global_cfg_get(int module_id)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_CLOCK,module_id,IPC_TYPE_SYNCE,IPC_TYPE_SNMP_SYNCE_GLOBAL_INFO,IPC_OPCODE_GET,1,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    
    memcpy(&synce_glb_cfg_common, psnd_msg->msg_data, sizeof(struct synce_snmp_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &synce_glb_cfg_common;
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

u_char *hhr_synce_configglobal_get(struct variable *pVar,
                                   oid *pName,  size_t *iOidLen,
                                   int  iExact, size_t *iVarlen, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    unsigned char  *pstr = NULL;

    iRetVal = header_generic(pVar, pName, iOidLen,
                             iExact, iVarlen, write_method);

    if (iRetVal == MATCH_FAILED)
    {
        return (NULL);
    }

    synce_snmp_global = synce_global_cfg_get(MODULE_ID_SNMPD);

    if (synce_snmp_global == NULL)
    {
        return NULL;
    }

    if (iRetVal == 0)
    {
        switch (pVar->magic)
        {
            case synceCfgMode:
                *iVarlen = sizeof(int);
                int_value = synce_snmp_global->mode + 1;
                return ((u_char *)&int_value);

            case synceCfgSSMEnable:
                *iVarlen = sizeof(int);

                if (synce_snmp_global->ssm_enable)
                {
                    int_value = 1;
                }
                else
                {
                    int_value = 2;
                }

                return ((u_char *)&int_value);

            case synceCfgSSMInterval:
                *iVarlen = sizeof(int);
                int_value = synce_snmp_global->ssm_interval;
                return ((u_char *)&int_value);

            case synceCfgPreemtEnable:
                *iVarlen = sizeof(int);

                if (synce_snmp_global->preempt)
                {
                    int_value = 1;
                }
                else
                {
                    int_value = 2;
                }

                return ((u_char *)&int_value);

            case synceCfgFailback :
                *iVarlen = sizeof(int);
                int_value = synce_snmp_global->wtr;
                return ((u_char *)&int_value);

            case synceCfgHoldTime:
                *iVarlen = sizeof(int);
                int_value = synce_snmp_global->hold_time;
                return ((u_char *)&int_value);

            case synceCfgClockSource:
                memset(str_value, 0, sizeof(uchar) * STRING_LEN);
                ifm_get_name_by_ifindex(synce_snmp_global->clk_select, str_value);

                if (strcmp(str_value , "unknown") == 0)
                {
                    strcpy(str_value, "none");
                }

                *iVarlen = strlen((char *) str_value);
                return str_value;

            case SyncECurrentClockState :
                *iVarlen = sizeof(int);
                int_value = synce_snmp_global->cur_clk_stat;
                return ((u_char *)&int_value);
            case SyncECurrentFreqOffset :
                memset(str_value, 0, sizeof(uchar) * STRING_LEN);
                sprintf(str_value,"%d",synce_snmp_global->iFreqOffset);
                *iVarlen = strlen((char *) str_value);
                return str_value;

            default :
                return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}
                                   
uint32_t synce_next_clk_if_ifindex_common;

uint32_t *synce_get_next_clk_if_ifindex(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_CLOCK,module_id,IPC_TYPE_SYNCE,IPC_TYPE_SNMP_SYNCE_CLOCK_IF_NEXT_IFINDEX,IPC_OPCODE_GET,ifindex,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&synce_next_clk_if_ifindex_common, psnd_msg->msg_data, sizeof(uint32_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &synce_next_clk_if_ifindex_common;
}

int hhrSynceClkIf_lookup(int exact, uint32_t index, uint32_t *index_next)
{
    uint32_t *pifindex = NULL;

    if (index_next == NULL)
    {
        return FALSE;
    }

    if (exact)
    {
        *index_next = index;
    }
    else
    {
        //add your lookup function here
        pifindex = synce_get_next_clk_if_ifindex(index, MODULE_ID_SNMPD);

        if (pifindex)
        {
            if (*pifindex == 0)
            {
                return FALSE;
            }

            *index_next = *pifindex;
        }
        else
        {
            //zlog_err ( "%s[%d] ifm_get_next_ifindex erro index 0x%0x\n", __FUNCTION__, __LINE__, index );
            return FALSE;
        }

    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] exact %d index 0x%0x index_next 0x%0x\n", __FUNCTION__, __LINE__, exact, index, *index_next);

    return TRUE;
}
struct synce_snmp_extern_clock_if_t synce_clock_iftable_common;

struct synce_snmp_extern_clock_if_t *synce_clock_iftable_get(uint32_t ifindex)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_CLOCK,MODULE_ID_SNMPD,IPC_TYPE_SYNCE,IPC_TYPE_SNMP_SYNCE_CLOCK_IF_INFO,IPC_OPCODE_GET,ifindex,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&synce_clock_iftable_common, psnd_msg->msg_data, sizeof(struct synce_snmp_extern_clock_if_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &synce_clock_iftable_common;
}

int synce_snmp_ql_value_exchange(uint8_t ql)
{
    int ret = 0;

    switch (ql)
    {
        case 0x0f:
            ret = 1;
            break;

        case 0x02:
            ret = 2;
            break;

        case 0x0b:
            ret = 3;
            break;

        case 0x04:
            ret = 4;
            break;

        case 0x08:
            ret = 5;
            break;

        case 0x00:
            ret = 6;
            break;

        default:
            break;
    }

    return ret;
}

int synce_snmp_sa_value_exchange(int sa_num)
{
    int ret = 0;

    switch (sa_num)
    {
        case 4: //sa4
            ret = 1;
            break;

        case 3:  //sa5
            ret = 2;
            break;

        case 2:  //sa6
            ret = 3;
            break;

        case 1:  //sa7
            ret = 4;
            break;

        case 0:  //sa8
            ret = 5;
            break;

        default:
            break;
    }

    return ret;
}

u_char *hhr_synce_clk_if_table_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;
    struct synce_snmp_extern_clock_if_t *pclk_if = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter hhr_synce_clk_if_table_get exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* lookup */
    ret = hhrSynceClkIf_lookup(exact, index, &index_next);

    if (ret == FALSE)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index 0x%0x index_next 0x%0x vp->magic %d\n", __FUNCTION__, __LINE__, index, index_next, vp->magic);

    pclk_if = synce_clock_iftable_get(index_next);

    if (pclk_if == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "ifm_base_iftable_get index_next 0x%0x\n", index_next);
        return NULL;
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case synceClkIfDescr :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(pclk_if->ifindex, (char *) str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifm_get_name_by_ifindex 0x%0x erro\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] str_value %s *var_len %d\n", __FUNCTION__, __LINE__, str_value, *var_len);
            return str_value;

        case synceClkIfMode :
            *var_len = sizeof(int);
            int_value = pclk_if->mode;

            return (u_char *) &int_value;

        case synceClkIfSignal :
            *var_len = sizeof(int);
            int_value = pclk_if->signal;

            return (u_char *) &int_value;

        case synceClkIfQL :
            *var_len = sizeof(int);
            int_value = synce_snmp_ql_value_exchange(pclk_if->ssm_level);

            return (u_char *) &int_value;

        case synceClkIfSA :
            *var_len = sizeof(int);
            int_value = synce_snmp_sa_value_exchange(pclk_if->sa);

            return (u_char *) &int_value;

        default :
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}
                                   
uint32_t synce_next_clk_src_ifindex_common;

uint32_t *synce_get_next_clk_src_ifindex(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_CLOCK,module_id,IPC_TYPE_SYNCE, \ 
                    IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_NEXT_IFINDEX,IPC_OPCODE_GET,ifindex,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&synce_next_clk_src_ifindex_common, psnd_msg->msg_data, sizeof(uint32_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &synce_next_clk_src_ifindex_common;
}

int hhrSynceClkSrcTable_lookup(int exact, uint32_t index, uint32_t *index_next)
{
    uint32_t *pifindex = NULL;

    if (index_next == NULL)
    {
        return FALSE;
    }

    if (exact)
    {
        *index_next = index;
    }
    else
    {
        //add your lookup function here
        pifindex = synce_get_next_clk_src_ifindex(index, MODULE_ID_SNMPD);

        if (pifindex)
        {
            if (*pifindex == 0)
            {
                return FALSE;
            }

            *index_next = *pifindex;
        }
        else
        {
            //zlog_err ( "%s[%d] ifm_get_next_ifindex erro index 0x%0x\n", __FUNCTION__, __LINE__, index );
            return FALSE;
        }

    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] exact %d index 0x%0x index_next 0x%0x\n", __FUNCTION__, __LINE__, exact, index, *index_next);

    return TRUE;
}
struct synce_snmp_clock_src_t synce_clock_src_table_common;

struct synce_snmp_clock_src_t *synce_clock_src_table_get(uint32_t ifindex)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_CLOCK,MODULE_ID_SNMPD,IPC_TYPE_SYNCE, \ 
                    IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_INFO,IPC_OPCODE_GET,ifindex,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&synce_clock_src_table_common, psnd_msg->msg_data, sizeof(struct synce_snmp_clock_src_t));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &synce_clock_src_table_common;
}


u_char *hhr_synce_clk_pri_table_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    u_int32_t index = 0;
    u_int32_t index_next = 0;
    int ret = 0;
    struct synce_snmp_clock_src_t *pclk_src = NULL;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] enter synce_clk_pri_table_get exact %d\n", __FUNCTION__, __LINE__, exact);

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    /* lookup */
    ret = hhrSynceClkSrcTable_lookup(exact, index, &index_next);

    if (ret == FALSE)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, index_next);
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index 0x%0x index_next 0x%0x vp->magic %d\n", __FUNCTION__, __LINE__, index, index_next, vp->magic);

    pclk_src = synce_clock_src_table_get(index_next);

    if (pclk_src == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "synce_clock_src_table_get failed index: 0x%0x\n", index_next);
        return NULL;
    }

    /*
    * this is where we do the value assignments for the mib results.
    */
    switch (vp->magic)
    {
        case synceClkSrcDescr :
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ret = ifm_get_name_by_ifindex(pclk_src->ifindex, (char *) str_value);

            if (ret < 0)
            {
                zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] ifm_get_name_by_ifindex 0x%0x erro\n", __FUNCTION__, __LINE__, index_next);
                return NULL;
            }

            *var_len = strlen((char *) str_value);
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] str_value %s *var_len %d\n", __FUNCTION__, __LINE__, str_value, *var_len);
            return str_value;

        case synceClkSrcPriority :
            *var_len = sizeof(int);
            int_value = pclk_src->priority;
            return (u_char *) &int_value;

        case synceClkSrcValidState :
            *var_len = sizeof(int);
            int_value = pclk_src->valid_stat;
            return (u_char *) &int_value;

        case synceClkSrcClockState :
            *var_len = sizeof(int);
            int_value = pclk_src->status;
            return (u_char *) &int_value;

        case synceClkSrcQL :
            *var_len = sizeof(int);
            int_value = synce_snmp_ql_value_exchange(pclk_src->quality_level);
            return (u_char *) &int_value;

        default :
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}



