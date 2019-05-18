/*
 *  cli command MIB group implementation - lldp_snmp.c
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
#include "lib/linklist.h"
#include "net-snmp-config.h"
#include <lib/types.h>
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
#include <lib/pkt_buffer.h>
#include <lib/log.h>

#include <lib/ether.h>
#include <lib/ifm_common.h>
#include "mib_lldp.h"

#include "../../l2/l2_snmp.h"
#include "../../l2/lldp/lldp.h"
#include "../../l2/lldp/lldp_api.h"
#include "../../l2/lldp/lldp_tlv.h"

#include "ipran_snmp_data_cache.h"
#include "snmp_config_table.h"

static struct ipran_snmp_data_cache *lldp_port_config_cache = NULL;
static struct ipran_snmp_data_cache *lldp_stats_txrx_port_cache = NULL;
static struct ipran_snmp_data_cache *lldp_loc_port_cache = NULL;
static struct ipran_snmp_data_cache *lldp_loc_port_priv_cache = NULL;

static struct ipran_snmp_data_cache *lldp_rem_cache = NULL;
static struct ipran_snmp_data_cache *lldp_rem_man_addr_cache = NULL;

lldp_global         *lldp_snmp_global_msg = NULL;
lldp_global         lldp_snmp_global_msg_value;
struct lldp_port    *lldp_snmp_port_msg = NULL;
struct lldp_port    lldp_snmp_port_msg_value;
struct lldp_msap    lldp_snmp_port_msap_value;

/*local temp variable*/
static uchar        str_value[STRING_LEN] = {'\0'};
static uchar        mac_value[6] = {0};
static int          int_value = 0;

//static struct ipc_msghdr_n    msghdr;
//static struct ipc_mesg_n      msg;

static struct counter64 uint64_value;

#define LLDP_SYS_CAP_MAX    8


#define LLDP_SNMP_RETURN_STRING(V, L)   \
do                                      \
{                                       \
    *var_len = L;                       \
    memcpy(str_value,V,L);              \
    return str_value;                   \
}while(0)


#define CHECK_FLAG(V,F)      ((V) & (F))
#define LLDP_SNMP_RETURN_INTEGER(V)     \
do                                      \
{                                       \
    *var_len = sizeof(int);             \
    int_value = V;                      \
    return (unsigned char *)&int_value; \
}while(0)

/*
 * oid
 */
oid lldpConfiguration_oid[] = {LLDP_MIB_ROOT, 1};
FindVarMethod lldpConfiguration_get;
struct variable1 lldpConfiguration_variables[] =
{
    {lldpMessageTxInterval,       ASN_INTEGER, RONLY, lldpConfiguration_get, 1, {1}},
    {lldpMessageTxHoldMultiplier, ASN_INTEGER, RONLY, lldpConfiguration_get, 1, {2}},
    {lldpNotificationInterval,    ASN_INTEGER, RONLY, lldpConfiguration_get, 1, {5}}
};

oid lldpPortConfigTable_oid[] = {LLDP_MIB_ROOT, 1, 6};
FindVarMethod lldpPortConfigTable_get;
struct variable2 lldpPortConfigTable_variables[] =
{
    {lldpPortConfigAdminStatus,  ASN_INTEGER, RONLY, lldpPortConfigTable_get, 2, {1, 2}},
    {lldpPortConfigTLVsTxEnable, ASN_INTEGER, RONLY, lldpPortConfigTable_get, 2, {1, 4}}
};

oid lldpStatsTxPort_oid[] = {LLDP_MIB_ROOT, 2, 6};
FindVarMethod lldpStatsTxPort_get;
struct variable2 lldpStatsTxPort_variables[] =
{
    {lldpStatsTxPortFramesTotal, ASN_COUNTER64, RONLY, lldpStatsTxPort_get, 2, {1, 2}}
};

oid lldpStatsRxPort_oid[] = {LLDP_MIB_ROOT, 2, 7};
FindVarMethod lldpStatsRxPort_get;
struct variable2 lldpStatsRxPort_variables[] =
{
    {lldpStatsRxPortTLVsDiscardedTotal,    ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 2}},
    {lldpStatsRxPortFramesErrors,          ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 3}},
    {lldpStatsRxPortFramesTotal,           ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 4}},
    {lldpStatsRxPortFramesDiscardedTotal,  ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 5}},
    {lldpStatsRxPortTLVsUnrecognizedTotal, ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 6}},
    {lldpStatsRxPortAgeoutsTotal,          ASN_COUNTER64, RONLY, lldpStatsRxPort_get, 2, {1, 7}}
};

oid lldpLocalSystemData_oid[] = {LLDP_MIB_ROOT, 3};
FindVarMethod lldpLocalSystemData_get;
struct variable1 lldpLocalSystemData_variables[] =
{
    {lldpLocChassisIdSubtype, ASN_INTEGER,   RONLY, lldpLocalSystemData_get, 1, {1}},
    {lldpLocChassisId,        ASN_OCTET_STR, RONLY, lldpLocalSystemData_get, 1, {2}},
    {lldpLocSysName,          ASN_OCTET_STR, RONLY, lldpLocalSystemData_get, 1, {3}},
    {lldpLocSysDesc,          ASN_OCTET_STR, RONLY, lldpLocalSystemData_get, 1, {4}},
    {lldpLocSysCapSupported,  ASN_INTEGER,   RONLY, lldpLocalSystemData_get, 1, {5}},
    {lldpLocSysCapEnabled,    ASN_INTEGER,   RONLY, lldpLocalSystemData_get, 1, {6}}
};

oid lldpLocPortTable_oid[] = {LLDP_MIB_ROOT, 3, 7};
FindVarMethod lldpLocPortTable_get;
struct variable2 lldpLocPortTable_variables[] =
{
    {lldpLocPortIdSubtype, ASN_INTEGER, RONLY, lldpLocPortTable_get, 2, {1, 2}},
    {lldpLocPortId,        ASN_OCTET_STR, RONLY, lldpLocPortTable_get, 2, {1, 3}},
    {lldpLocPortDesc,      ASN_OCTET_STR, RONLY, lldpLocPortTable_get, 2, {1, 4}}
};

oid lldpLocPortTable_priv_oid[] = {LLDP_MIB_PRIV_ROOT, 1, 1};
FindVarMethod lldpLocPortTable_priv_get;
struct variable2 lldpLocPortTable_priv_variables[] =
{
    {lldpNeighborNum,      ASN_INTEGER, RONLY, lldpLocPortTable_priv_get, 2, {1, 1}} ,
    {lldplldpLocPortDesc,      ASN_OCTET_STR, RONLY, lldpLocPortTable_priv_get, 2, {1, 2}} ,
    {lldplldpLocPhysAddress,      ASN_OCTET_STR, RONLY, lldpLocPortTable_priv_get, 2, {1, 3}},
    {hhrlldpLocPortArpLearning,      ASN_INTEGER, RONLY, lldpLocPortTable_priv_get, 2, {1, 4}},
    {hhrlldpLocPortArpLearningVlan,      ASN_INTEGER, RONLY, lldpLocPortTable_priv_get, 2, {1, 5}},
};

#if 0
oid lldpLocManAddrTable_oid[] = {LLDP_MIB_ROOT, 3, 8};
FindVarMethod lldpLocManAddrTable_get;
struct variable2 lldpLocManAddrTable_variables[] =
{
    {lldpLocManAddrLen,       ASN_INTEGER, RONLY, lldpLocManAddrTable_get, 2, {1, 3}},
    {lldpLocManAddrIfSubtype, ASN_INTEGER, RONLY, lldpLocManAddrTable_get, 2, {1, 4}},
    {lldpLocManAddrIfId,      ASN_INTEGER, RONLY, lldpLocManAddrTable_get, 2, {1, 5}},
    {lldpLocManAddrOID,       ASN_INTEGER, RONLY, lldpLocManAddrTable_get, 2, {1, 6}}
};
#endif

oid lldpRemTable_oid[] = {LLDP_MIB_ROOT, 4, 1};
FindVarMethod lldpRemTable_get;
struct variable2 lldpRemTable_variables[] =
{
    {lldpRemChassisIdSubtype, ASN_INTEGER, RONLY, lldpRemTable_get, 2, {1,  4}},
    {lldpRemChassisId,        ASN_OCTET_STR, RONLY, lldpRemTable_get, 2, {1,  5}},
    {lldpRemPortIdSubtype,    ASN_INTEGER, RONLY, lldpRemTable_get, 2, {1,  6}},
    {lldpRemPortId,           ASN_OCTET_STR, RONLY, lldpRemTable_get, 2, {1,  7}},
    {lldpRemPortDesc,         ASN_OCTET_STR, RONLY, lldpRemTable_get, 2, {1,  8}},
    {lldpRemSysName,          ASN_OCTET_STR, RONLY, lldpRemTable_get, 2, {1,  9}},
    {lldpRemSysDesc,          ASN_OCTET_STR, RONLY, lldpRemTable_get, 2, {1, 10}},
    {lldpRemSysCapSupported,  ASN_INTEGER, RONLY, lldpRemTable_get, 2, {1, 11}},
    {lldpRemSysCapEnabled,    ASN_INTEGER, RONLY, lldpRemTable_get, 2, {1, 12}},
};

oid lldpRemTable_priv_oid[] = {LLDP_MIB_PRIV_ROOT, 2, 1};
FindVarMethod lldpRemTable_priv_get;
struct variable2 lldpRemTable_priv_variables[] =
{
    {lldpSvlanType,           ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 1}},
    {lldpSvlanId,             ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 2}},
    {lldpCvlanType,           ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 3}},
    {lldpCvlanId,             ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 4}},
    {lldpExpiredTime,         ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 5}},
    {lldpMgmtAddrSubType,     ASN_INTEGER, RONLY, lldpRemTable_priv_get, 2, {1, 6}},
    {lldpMgmtAddr,         	  ASN_OCTET_STR, RONLY, lldpRemTable_priv_get, 2, {1, 7}}
};

oid lldpManagementAddressSubType_priv_oid[] = {LLDP_MIB_PRIV_ROOT};
FindVarMethod lldpManagementAddressSubType_priv_get;
struct variable1 lldpManagementAddressSubType_priv_variables[] =
{
    {hhrlldpManagementAddressSubType, ASN_INTEGER, RONLY, lldpManagementAddressSubType_priv_get, 1, {4}},
    {hhrlldpManagementAddress, ASN_OCTET_STR, RONLY, lldpManagementAddressSubType_priv_get, 1, {5}},		
};


oid lldpV2LocManAddrTable_oid[] = {LLDPv2_MIB_ROOT, 3, 8};
FindVarMethod lldpV2LocManAddrTable_get;
struct variable2 lldpV2LocManAddrTable_variables[] =
{
    {lldpV2LocManAddrLen, ASN_INTEGER, RONLY, lldpV2LocManAddrTable_get, 2, {1, 3}},
    {lldpV2LocManAddrIfSubtype, ASN_INTEGER, RONLY, lldpV2LocManAddrTable_get, 2, {1, 4}},
    {lldpV2LocManAddrIfId,      ASN_INTEGER, RONLY, lldpV2LocManAddrTable_get, 2, {1, 5}},
    {lldpV2LocManAddrOID,      ASN_OCTET_STR, RONLY, lldpV2LocManAddrTable_get, 2, {1, 6}}
};


#if 1
oid lldpRemManAddrTable_oid[] = {LLDP_MIB_ROOT, 4, 2};
FindVarMethod lldpRemManAddrTable_get;
struct variable2 lldpRemManAddrTable_variables[] =
{
    {lldpRemManAddrIfSubtype, ASN_INTEGER, RONLY, lldpRemManAddrTable_get, 2, {1, 3}},
    {lldpRemManAddrIfId,      ASN_INTEGER, RONLY, lldpRemManAddrTable_get, 2, {1, 4}},
    {lldpRemManAddrOID,       ASN_OBJECT_ID, RONLY, lldpRemManAddrTable_get, 2, {1, 5}}
};
#endif

void  init_mib_lldp(void)
{

    REGISTER_MIB("lldpConfiguration", lldpConfiguration_variables, variable1,
                 lldpConfiguration_oid);

    REGISTER_MIB("lldpPortConfigTable", lldpPortConfigTable_variables, variable2,
                 lldpPortConfigTable_oid);

    REGISTER_MIB("lldpStatsTxPort", lldpStatsTxPort_variables, variable2,
                 lldpStatsTxPort_oid);

    REGISTER_MIB("lldpStatsRxPort", lldpStatsRxPort_variables, variable2,
                 lldpStatsRxPort_oid);

    REGISTER_MIB("lldpLocalSystemData", lldpLocalSystemData_variables, variable1,
                 lldpLocalSystemData_oid);

    REGISTER_MIB("lldpLocPortTable", lldpLocPortTable_variables, variable2,
                 lldpLocPortTable_oid);

    REGISTER_MIB("lldpLocPortTable_priv", lldpLocPortTable_priv_variables, variable2,
                 lldpLocPortTable_priv_oid);
#if 0
    REGISTER_MIB("lldpLocManAddrTable", lldpLocManAddrTable_variables, variable2,
                 lldpLocManAddrTable_oid);
#endif
    REGISTER_MIB("lldpRemTable", lldpRemTable_variables, variable2,
                 lldpRemTable_oid);

    REGISTER_MIB("priv_lldpRemTable", lldpRemTable_priv_variables, variable2,
                 lldpRemTable_priv_oid);
    REGISTER_MIB("lldpManagementAddressSubType", lldpManagementAddressSubType_priv_variables, variable1,
                 lldpManagementAddressSubType_priv_oid);

    REGISTER_MIB("lldpV2LocManAddrTable", lldpV2LocManAddrTable_variables, variable2,
                 lldpV2LocManAddrTable_oid);
#if 1
    REGISTER_MIB("lldpRemManAddrTable", lldpRemManAddrTable_variables, variable2,
                 lldpRemManAddrTable_oid);
#endif
}

/*return value: lldp local cfg value*/
struct  ipc_mesg_n  *lldp_global_cfg_get(int module_id,struct ipc_mesg_n *pmesg)
{
    /*return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_L2, module_id,
                                      IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_GLOBAL_CFG,
                                      IPC_OPCODE_GET, 0);*/
  
	struct ipc_mesg_n *pmsg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2, module_id,
	                              IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_GLOBAL_CFG,
	                              IPC_OPCODE_GET, 0,2000);

	if (pmsg)
	{	
		memcpy(pmesg,pmsg,sizeof(struct ipc_mesg_n));
		mem_share_free(pmsg, MODULE_ID_SNMPD);
		return NULL;
		//return (struct lldp_global *)pmsg->msg_data;
	}
	else
	{
	 	return NULL;
	}
}

/*return value: lldp local port info value*/
struct lldp_port *lldp_port_info_get(unsigned int ifindex, int module_id,	struct lldp_port *lldp_snmp_port)
{
  /*  return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_L2, module_id,
                                      IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_PORT,
                                      IPC_OPCODE_GET, ifindex);*/

	struct ipc_mesg_n *pmsg =  ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2, module_id,
						 IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_PORT,
						 IPC_OPCODE_GET, ifindex,2000);

	if (pmsg)
	{
		if (pmsg->msg_data != NULL)
		{
			memcpy(lldp_snmp_port,(struct lldp_port*)pmsg->msg_data,sizeof(struct lldp_port));
		}
		mem_share_free(pmsg,MODULE_ID_SNMPD);
		return NULL;
		//return (struct lldp_port *)pmsg->msg_data;
	}
	else
	{
	 	return NULL;
	}

}

/*return value: lldp msap info value*/
struct lldp_msap *lldp_remote_port_info_get(unsigned int ifindex, int module_id,
                                            uint8_t *pdata_num, struct ipc_mesg_n *pmesg)
{
#if 0
    return ipc_send_common_wait_reply(NULL, 0, 1, MODULE_ID_L2, module_id,
                                      IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_PORT_MSAP,
                                      IPC_OPCODE_GET, ifindex);
#else
    *pdata_num = 0;

	struct ipc_mesg_n *pmsg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_L2, module_id,
	                                                    IPC_TYPE_SNMP, IPC_TYPE_SNMP_LLDP_PORT_MSAP,
	                                                    IPC_OPCODE_GET, ifindex,2000);

    if (pmsg)
    {
        *pdata_num = pmsg->msghdr.data_num;
		memcpy(pmesg,pmsg,sizeof(struct ipc_mesg_n));
		mem_share_free(pmsg, MODULE_ID_SNMPD);
		return NULL;
    }
    else
    {
        return NULL;
    }

#endif
}

int lldp_get_next_ifindex(uint32_t ifindex, int module_id, uint32_t *pifindex)
{
    uint8_t if_type = IFNET_TYPE_MAX;
#if 0
    return ipc_send_common_wait_reply(&if_type, sizeof(if_type), 1 , MODULE_ID_IFM, module_id,
                                      IPC_TYPE_IFM, IFNET_INFO_NEXT_IFINDEX, IPC_OPCODE_GET, ifindex);
#else
	struct ipc_mesg_n *pmsg =  ipc_sync_send_n2(&if_type, sizeof(if_type), 1 , MODULE_ID_IFM, module_id,
	                              IPC_TYPE_IFM, IFNET_INFO_NEXT_IFINDEX, IPC_OPCODE_GET, ifindex,2000);

	if (pmsg)
	{
		if (pmsg->msg_data != NULL)
		{
			memcpy(pifindex,(uint32_t *)pmsg->msg_data,sizeof(uint32_t));
		}
		mem_share_free(pmsg,MODULE_ID_SNMPD);
		return 0;
		//return (uint32_t *)pmsg->msg_data;
	}
	else
	{
		  return -1;
	}
#endif
}

int lldp_snmp_if_exact(int exact, uint32_t index, uint32_t *index_next)
{
    uint32_t pifindex = 0;
	int ret = 0;

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
        /*get next ifindex*/
        ret = lldp_get_next_ifindex(index, MODULE_ID_SNMPD, &pifindex);

        if (ret != 0)
        {
            return FALSE;
        }

//      while((!(IFM_TYPE_IS_ETHERNET(*pifindex))) || (IFM_TYPE_IS_OUTBAND(*pifindex)) || (IFM_IS_SUBPORT(*pifindex)))
        while (((!(IFM_TYPE_IS_METHERNET(pifindex))) && (!(IFM_TYPE_IS_TRUNK(pifindex))))
                || (IFM_TYPE_IS_OUTBAND(pifindex)) || (IFM_IS_SUBPORT(pifindex)))
        {
            index = pifindex;
            ret = lldp_get_next_ifindex(index, MODULE_ID_SNMPD, &pifindex);

            if (ret != 0)
            {
                break;
            }
        }

        if (pifindex)
        {
            *index_next = pifindex;
        }
        else
        {
            return FALSE;
        }

    }

    zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] index 0x%0x index_next 0x%0x\n", __FUNCTION__, __LINE__, index, *index_next);

    return TRUE;
}

#if 0
int lldp_get_port_remote_index(struct lldp_port *lldp_local_port,
                               struct lldp_msap **lldp_remote_msap,
                               time_t *time_mark,
                               int *port_num, u_int32_t *remote_index)
{
    int i;
    unsigned int remote_index_next = 0;
    struct lldp_msap *lldp_remote_msap_temp = NULL;
    struct lldp_port    *lldp_snmp_port_msg = NULL;
    unsigned char       buff[8192];

    lldp_snmp_port_msg = lldp_port_info_get(*port_num, MODULE_ID_SNMPD);

    if (NULL == lldp_snmp_port_msg)
    {
        return 0;
    }

    memcpy((unsigned char *)&lldp_snmp_port_msg_value, (unsigned char *)lldp_snmp_port_msg, sizeof(struct lldp_port));

    if (lldp_snmp_port_msg_value.msap_num < *remote_index)
    {
        return 0;
    }

    if (lldp_snmp_port_msg_value.msap_cache != NULL)
    {
        lldp_remote_msap_temp = lldp_remote_port_info_get(*port_num, MODULE_ID_SNMPD);

        if (lldp_remote_msap_temp == NULL)
        {
            return 0;
        }

        memcpy(buff, (unsigned char *)lldp_remote_msap_temp, lldp_snmp_port_msg_value.msap_num * (sizeof(struct lldp_msap)));
    }

    for (i = 1; i <= lldp_snmp_port_msg_value.msap_num; i++)
    {
        if (i > *remote_index)
        {
            remote_index_next = i;
            memset((void *)&lldp_snmp_port_msap_value, 0x00, sizeof(struct lldp_msap));
            memcpy((unsigned char *)&lldp_snmp_port_msap_value, (unsigned char *)lldp_remote_msap_temp, sizeof(struct lldp_msap));
            *lldp_remote_msap = &lldp_snmp_port_msap_value;
            *remote_index = remote_index_next;

            return 1;
        }
        else if (lldp_remote_msap_temp != NULL)
        {
            lldp_remote_msap_temp = (struct lldp_msap *) & buff[(sizeof(struct lldp_msap) * i)];
        }
    }

    *remote_index = remote_index_next;

    return 0;
}


int lldp_get_next_port_num(unsigned int *port_num_next,
                           time_t time_mark, int port_num)
{
    uint32_t *pifindex = NULL;

    /*get next interface*/
    pifindex = lldp_get_next_ifindex(port_num, MODULE_ID_SNMPD);

    if (pifindex == NULL)
    {
        return FALSE;
    }

//  while((!(IFM_TYPE_IS_ETHERNET(*pifindex))) || (IFM_TYPE_IS_OUTBAND(*pifindex)) || (IFM_IS_SUBPORT(*pifindex)))
    while (((!(IFM_TYPE_IS_METHERNET(*pifindex))) && (!(IFM_TYPE_IS_TRUNK(*pifindex))))
            || (IFM_TYPE_IS_OUTBAND(*pifindex)) || (IFM_IS_SUBPORT(*pifindex)))

    {
        port_num = *pifindex;
        pifindex = lldp_get_next_ifindex(port_num, MODULE_ID_SNMPD);

        if (pifindex == NULL)
        {
            break;
        }
    }

    if (pifindex)
    {
        *port_num_next = *pifindex;
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

struct lldp_msap *lldp_remote_port_table_get_next(struct lldp_port *lldp_local_port,
                                                  time_t *time_mark, int *port_num,
                                                  u_int32_t *remote_index)
{
    int                 ret = 0;
    unsigned int        port_num_next = 0;
    struct lldp_msap    *lldp_remote_msap = NULL;
    struct lldp_port    *lldp_local_port_temp = NULL;

    do
    {
        ret = lldp_get_port_remote_index(lldp_local_port, &lldp_remote_msap, time_mark, port_num, remote_index);

        if (!ret)
        {
            do
            {
                lldp_local_port = NULL;
                port_num_next = 0;
                ret = lldp_get_next_port_num(&port_num_next, *time_mark, *port_num);

                if (ret)
                {
                    *port_num = port_num_next;

                    lldp_local_port = lldp_port_info_get(*port_num, MODULE_ID_SNMPD);
                }
                else
                {
                    break;
                }

            }
            while (lldp_local_port == NULL);
        }
        else
        {
            break;
        }
    }
    while (port_num_next > 0);

    if (ret)
    {
        return lldp_remote_msap;
    }
    else
    {
        return NULL;
    }
}
#endif

int lldp_check_array_value(unsigned char *array, unsigned int length)
{
    int i;

    for (i = 0; i < length; i++)
    {
        if (array[i] == 0 || array[i] == '\0')
        {
            continue;
        }

        return 1;
    }

    return 0;
}

#if 0
struct lldp_msap *lldp_remote_port_table_get(time_t *time_mark, int *port_num,
                                             u_int32_t *remote_index, int exact)
{
    int i = 0, j = 0, ret = 0;
    struct lldp_port    *lldp_local_port = NULL;
    struct lldp_msap    *lldp_remote_msap = NULL;
    unsigned int        port_num_next = 0;
    unsigned int        msap_number = 0;
    unsigned char       buff[8192];


    if (exact)
    {
        lldp_local_port = lldp_port_info_get(*port_num, MODULE_ID_SNMPD);

        if (lldp_local_port == NULL)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"lldp_remote_port_table_get lldp_local_port is NULL\n");
            return NULL;
        }

        if ((lldp_local_port->msap_cache != NULL)
                && (lldp_local_port->msap_num >= *remote_index))
        {
            msap_number      = lldp_local_port->msap_num;
            lldp_remote_msap = lldp_remote_port_info_get(*port_num, MODULE_ID_SNMPD);
            memcpy(buff, (unsigned char *)lldp_remote_msap, msap_number * (sizeof(struct lldp_msap)));

            if (lldp_remote_msap == NULL)
            {
                return 0;
            }

            for (i = 1; i <= msap_number ; i++)
            {
                if (i == *remote_index)
                {
                    return lldp_remote_msap;
                }
                else if (lldp_remote_msap != NULL)
                {
                    lldp_remote_msap = (struct lldp_msap *) & buff[(sizeof(struct lldp_msap) * i)];
                }
                else
                {
                    /*error*/
                    return NULL;
                }
            }
        }
        else
        {
            if (lldp_local_port->msap_cache != NULL)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"lldp_local_port->msap_cache == NULL");
            }

            if (lldp_local_port->msap_num >= *remote_index)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"lldp_local_port->msap_num = %d, remote_index = %d", lldp_local_port->msap_num, *remote_index);
            }

            return NULL;
        }
    }
    else
    {
        if (0 == *port_num)
        {
            do
            {
                lldp_local_port = NULL;
                port_num_next = 0;
                ret = lldp_get_next_port_num(&port_num_next, *time_mark, *port_num);

                if (ret)
                {
                    *port_num = port_num_next;

                    lldp_local_port = lldp_port_info_get(*port_num, MODULE_ID_SNMPD);
                }
                else
                {
                    break;
                }

            }
            while (lldp_local_port == NULL);


            lldp_local_port = lldp_port_info_get(*port_num, MODULE_ID_SNMPD);

            if (lldp_local_port == NULL)
            {
                zlog_debug(SNMP_DBG_MIB_GET,"lldp_remote_port_table_get lldp_local_port is NULL\n");
                return NULL;
            }
        }

        lldp_remote_msap = lldp_remote_port_table_get_next(lldp_local_port, time_mark, port_num,
                                                           remote_index);
    }

    return lldp_remote_msap;
}
#endif

struct ipc_mesg_n  *snmp_get_lldp_info_bulk(uint32_t ifindex, int module_id , int *pdata_num, int msg_subtype, int data_num)
{
#if 0
	return ipc_send_common_wait_reply(&if_type, sizeof(if_type), 1 , MODULE_ID_IFM, module_id,
																		IPC_TYPE_IFM, IFNET_INFO_NEXT_IFINDEX, IPC_OPCODE_GET, ifindex);
#else
	struct ipc_mesg_n *pmsg =  NULL;

	pmsg = ipc_sync_send_n2(NULL, 0, data_num, MODULE_ID_L2, module_id,
															IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex, 2000);

	if (pmsg)
	{
		*pdata_num = pmsg->msghdr.data_num;
		return pmsg;
	}

#endif

    return NULL;
}


int lldp_port_config_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct lldp_port_config_snmp  *index)
{
    struct lldp_port_config_snmp *plldp_info = NULL;
    int data_num = 0;
    int ret = 0;

	struct	ipc_mesg_n	*pmesg = NULL;
	
    pmesg = snmp_get_lldp_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num, 
					IPC_TYPE_SNMP_LLDP_PORT_CONFIG, 0);
	if (NULL == pmesg)
	{
		return FALSE;
	}

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

    plldp_info = (struct lldp_port_config_snmp *)pmesg->msg_data;
	if (data_num == 0 || plldp_info == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
		
	for (ret = 0; ret < data_num; ret++)
	{
		snmp_cache_add(cache, plldp_info , sizeof(struct lldp_port_config_snmp));
		plldp_info++;
	}

	mem_share_free(pmesg, MODULE_ID_SNMPD);

	return TRUE;
}

struct lldp_port_config_snmp *lldp_port_config_node_lookup(struct ipran_snmp_data_cache *cache ,
                                        int exact, const struct lldp_port_config_snmp  *index_input)
{
    struct listnode  *node, *nnode;
    struct lldp_port_config_snmp *data_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data_find->ifindex == index_input->ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return NULL;
}


int lldp_stats_txrx_port_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct lldp_stats_txrx_port_snmp  *index)
{
    struct lldp_stats_txrx_port_snmp *plldp_info = NULL;
    int data_num = 0;
    int ret = 0;
	struct	ipc_mesg_n	*pmesg = NULL;
	
    pmesg = snmp_get_lldp_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num, 
					IPC_TYPE_SNMP_LLDP_STATS_TXRX_PORT, 0);
	if (pmesg == NULL)
	{
		return FALSE;
	}

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	plldp_info = (struct lldp_stats_txrx_port_snmp *)pmesg->msg_data;
	if (data_num == 0 || plldp_info == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
		
	for (ret = 0; ret < data_num; ret++)
	{
		snmp_cache_add(cache, plldp_info , sizeof(struct lldp_stats_txrx_port_snmp));
		plldp_info++;
	}

	mem_share_free(pmesg, MODULE_ID_SNMPD);

	return TRUE;
}


struct lldp_stats_txrx_port_snmp *lldp_stats_txrx_port_node_lookup(struct ipran_snmp_data_cache *cache ,
                                        int exact, const struct lldp_stats_txrx_port_snmp  *index_input)
{
    struct listnode  *node, *nnode;
    struct lldp_stats_txrx_port_snmp *data_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data_find->ifindex == index_input->ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return NULL;
}


int lldp_loc_port_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct lldp_loc_port_snmp  *index)
{
    struct lldp_loc_port_snmp *plldp_info = NULL;
    int data_num = 0;
    int ret = 0;
	struct	ipc_mesg_n	*pmesg = NULL;
	
    pmesg = snmp_get_lldp_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num, 
					IPC_TYPE_SNMP_LLDP_LOC_PORT, 0);
	if (pmesg == NULL)
	{
		return FALSE;
	}

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	plldp_info = (struct lldp_loc_port_snmp *)pmesg->msg_data;
	if (data_num == 0 || plldp_info == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
		
	for (ret = 0; ret < data_num; ret++)
	{
	    snmp_cache_add(cache, plldp_info , sizeof(struct lldp_loc_port_snmp));
	    plldp_info++;
	}

	mem_share_free(pmesg, MODULE_ID_SNMPD);

	return TRUE;
}


struct lldp_loc_port_snmp *lldp_loc_port_node_lookup(struct ipran_snmp_data_cache *cache ,
                                        int exact, const struct lldp_loc_port_snmp  *index_input)
{
    struct listnode  *node, *nnode;
    struct lldp_loc_port_snmp *data_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data_find->ifindex == index_input->ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return NULL;
}


int lldp_loc_port_priv_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                        struct lldp_loc_port_priv_snmp  *index)
{
    struct lldp_loc_port_priv_snmp *plldp_info = NULL;
    int data_num = 0;
    int ret = 0;
	struct	ipc_mesg_n	*pmesg = NULL;
	
    pmesg = snmp_get_lldp_info_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num, 
					IPC_TYPE_SNMP_LLDP_LOC_PORT_PRIV, 0);
	if (pmesg == NULL)
	{
		return FALSE;
	}

	zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': data_num [%d]\n", __FILE__, __LINE__, __func__ , data_num);

	plldp_info = (struct lldp_loc_port_priv_snmp *)pmesg->msg_data;
	if (data_num == 0 || plldp_info == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
		
	for (ret = 0; ret < data_num; ret++)
	{
	    snmp_cache_add(cache, plldp_info , sizeof(struct lldp_loc_port_priv_snmp));
	    plldp_info++;
	}

	mem_share_free(pmesg, MODULE_ID_SNMPD);
				
	return TRUE;
}


struct lldp_loc_port_priv_snmp *lldp_loc_port_priv_node_lookup(struct ipran_snmp_data_cache *cache ,
                                        int exact, const struct lldp_loc_port_priv_snmp  *index_input)
{
    struct listnode  *node, *nnode;
    struct lldp_loc_port_priv_snmp *data_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data_find))
    {
        if (NULL == index_input || 0 == index_input->ifindex)
        {
            return cache->data_list->head->data ;
        }

        if (data_find->ifindex == index_input->ifindex)
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
                    return node->next->data ;
                }
            }
        }
    }

    return NULL;
}


struct ipc_mesg_n *snmp_get_lldp_msap_info_bulk(void *data, int data_len, uint32_t ifindex, int module_id , int *pdata_num, int msg_subtype, int data_num)
{ 
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(data, data_len, data_num, MODULE_ID_L2, module_id,
												IPC_TYPE_SNMP, msg_subtype, IPC_OPCODE_GET, ifindex, 2000);
	if (pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return pmesg;
	}

	return NULL;
}


int lldp_msap_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                     struct lldp_msap_snmp  *index)
{
    struct lldp_msap_snmp *plldp_msap = NULL;
    struct  ipc_mesg_n *pmesg = NULL;
    int data_num = 0;
    int i = 0;

	pmesg = snmp_get_lldp_msap_info_bulk((void *) & (index->lldp_msap_info.msap_index), sizeof(uint8_t), \
				 index->ifindex, MODULE_ID_SNMPD, &data_num, IPC_TYPE_SNMP_LLDP_PORT_MSAP, 0);

    zlog_debug(SNMP_DBG_MIB_GET,"%s:'%s': data_num [%d]\n", __FILE__, __func__ , data_num);

	if(pmesg == NULL)
	{
		return FALSE;
	}

	plldp_msap = (struct lldp_msap_snmp *)(pmesg->msg_data);
	if (data_num == 0 || plldp_msap == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
			
	for (i = 0; i < data_num; i++)
	{
		snmp_cache_add(cache, plldp_msap , sizeof(struct lldp_msap_snmp));
		plldp_msap++;
	}
		
	mem_share_free(pmesg, MODULE_ID_SNMPD);
	
	return TRUE;
}


int lldp_msap_man_addr_info_get_data_from_ipc(struct ipran_snmp_data_cache *cache ,
                                     struct lldp_msap_man_addr_snmp  *index)
{
    struct lldp_msap_man_addr_snmp *plldp_msap = NULL;
	struct ipc_mesg_n *pmesg = NULL;
    int data_num = 0;
    int i = 0;

    pmesg = snmp_get_lldp_msap_info_bulk((void *) & (index->lldp_msap_man_addr_info.msap_index), sizeof(uint8_t), \
                                              index->ifindex, MODULE_ID_SNMPD, &data_num, IPC_TYPE_SNMP_LLDP_PORT_MSAP_MAN_ADDR, 0);

    zlog_debug(SNMP_DBG_MIB_GET, "%s:'%s': data_num [%d]\n", __FILE__, __func__ , data_num);

	if(pmesg == NULL)
	{
		return FALSE;
	}

	plldp_msap = (struct lldp_msap_man_addr_snmp *)pmesg->msg_data;
	if (data_num == 0 || plldp_msap == NULL)
	{
		mem_share_free(pmesg, MODULE_ID_SNMPD);
		return FALSE;
	}
		
	for (i = 0; i < data_num; i++)
	{
	    snmp_cache_add(cache, plldp_msap , sizeof(struct lldp_msap_man_addr_snmp));
	    plldp_msap++;
	}

	mem_share_free(pmesg, MODULE_ID_SNMPD);

    return TRUE;
}


struct lldp_msap_snmp *lldp_msap_info_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                  int exact,
                                                  const struct lldp_msap_snmp  *index_input)
{
    struct listnode  *node, *nnode;

    struct lldp_msap_snmp    *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->ifindex)
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->ifindex == index_input->ifindex) &&
                (data1_find->lldp_msap_info.msap_index == index_input->lldp_msap_info.msap_index))
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

    return NULL;
}


u_char *
lldpConfiguration_get(struct variable *vp,
                      oid *name,
                      size_t *length,
                      int exact, size_t *var_len, WriteMethod **write_method)
{
    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }
	struct  ipc_mesg_n  pmesg;
    /*get global config*/
	memset(&pmesg,0,sizeof(struct ipc_mesg_n));
    lldp_global_cfg_get(MODULE_ID_SNMPD,&pmesg);

	lldp_snmp_global_msg = (lldp_global *)(pmesg.msg_data);
    if (lldp_snmp_global_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp global config message NULL!\n", __FUNCTION__, __LINE__);

        return NULL;
    }

    switch (vp->magic)
    {
        case lldpMessageTxInterval:
        {
            *var_len = sizeof(int);
            int_value = lldp_snmp_global_msg->msgTxInterval;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpMessageTxHoldMultiplier:
        {
            *var_len = sizeof(int);
            int_value = lldp_snmp_global_msg->msgTxHold;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpNotificationInterval:
        {
            *var_len = sizeof(int);
            int_value = lldp_snmp_global_msg->trapInterval;

            return (unsigned char *)&int_value;
        }
        break;

        default :
            return NULL;
    }

    return NULL;
}

u_char *
lldpPortConfigTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
	struct lldp_port_config_snmp index;
	int ret = 0;
	struct lldp_port_config_snmp *lldp_port_config = NULL;

	*write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
    if (ret < 0)
    {
        return NULL;
    }

	if (NULL == lldp_port_config_cache)
	{
		lldp_port_config_cache = snmp_cache_init(sizeof(struct lldp_port_config_snmp) ,
																lldp_port_config_info_get_data_from_ipc ,
																lldp_port_config_node_lookup);
	
		if (NULL == lldp_port_config_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}
		
	lldp_port_config = snmp_cache_get_data_by_index (lldp_port_config_cache, exact, &index);
	if (NULL == lldp_port_config)
	{
		return NULL;
	}

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, lldp_port_config->ifindex);
    }

    switch (vp->magic)
    {
        case lldpPortConfigAdminStatus:
        {
            *var_len = sizeof(int);

			if (TRUE == lldp_port_config->adminStatus)
			{
				int_value = 3;
			}
            else
            {
                int_value = 4;
            }

            return (unsigned char *)&int_value;
        }
        break;

        case lldpPortConfigTLVsTxEnable:
        {
            *var_len = sizeof(int);
			int_value = lldp_port_config->tlv_tx_enable;
            return (unsigned char *)&int_value;
        }
        break;

        default :
            return NULL ;
    }

    return NULL;

}

u_char *
lldpStatsTxPort_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
	struct lldp_stats_txrx_port_snmp index;
	int ret = 0;
	struct lldp_stats_txrx_port_snmp *lldp_stats_txrx_port = NULL;
	
	*write_method = NULL;
	
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	if (ret < 0)
	{
		return NULL;
	}
	
	if (NULL == lldp_stats_txrx_port_cache)
	{
		lldp_stats_txrx_port_cache = snmp_cache_init(sizeof(struct lldp_stats_txrx_port_snmp) ,
																lldp_stats_txrx_port_info_get_data_from_ipc ,
																lldp_stats_txrx_port_node_lookup);
	
		if (NULL == lldp_stats_txrx_port_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}
		
	lldp_stats_txrx_port = snmp_cache_get_data_by_index (lldp_stats_txrx_port_cache, exact, &index);
	if (NULL == lldp_stats_txrx_port)
	{
		return NULL;
	}
	
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, lldp_stats_txrx_port->ifindex);
	}

    switch (vp->magic)
    {
        case lldpStatsTxPortFramesTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->tx_statsFramesOutTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->tx_statsFramesOutTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        default :
            return NULL;
    }

    return NULL ;
}

u_char *
lldpStatsRxPort_get(struct variable *vp,
                    oid *name,
                    size_t *length,
                    int exact, size_t *var_len, WriteMethod **write_method)
{
	struct lldp_stats_txrx_port_snmp index;
	int ret = 0;
	struct lldp_stats_txrx_port_snmp *lldp_stats_txrx_port = NULL;
	
	*write_method = NULL;
	
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	if (ret < 0)
	{
		return NULL;
	}
	
	if (NULL == lldp_stats_txrx_port_cache)
	{
		lldp_stats_txrx_port_cache = snmp_cache_init(sizeof(struct lldp_stats_txrx_port_snmp) ,
																lldp_stats_txrx_port_info_get_data_from_ipc ,
																lldp_stats_txrx_port_node_lookup);
	
		if (NULL == lldp_stats_txrx_port_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}
		
	lldp_stats_txrx_port = snmp_cache_get_data_by_index (lldp_stats_txrx_port_cache, exact, &index);
	if (NULL == lldp_stats_txrx_port)
	{
		return NULL;
	}
	
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, lldp_stats_txrx_port->ifindex);
	}

    switch (vp->magic)
    {
        case lldpStatsRxPortTLVsDiscardedTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsTLVsDiscardedTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsTLVsDiscardedTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        case lldpStatsRxPortFramesErrors:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsFramesInErrorsTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsFramesInErrorsTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        case lldpStatsRxPortFramesTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsFramesInTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsFramesInTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        case lldpStatsRxPortFramesDiscardedTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsFramesDiscardedTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsFramesDiscardedTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        case lldpStatsRxPortTLVsUnrecognizedTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsTLVsUnrecognizedTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsTLVsUnrecognizedTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        case lldpStatsRxPortAgeoutsTotal:
        {
			uint64_value.high = (unsigned long)(lldp_stats_txrx_port->rx_statsAgeoutsTotal >> 32);
			uint64_value.low  = (unsigned long)lldp_stats_txrx_port->rx_statsAgeoutsTotal;

			*var_len = sizeof(struct counter64);
            return (u_char *)&uint64_value;
        }
        break;

        default :
            return NULL ;
    }
}

u_char *
lldpLocalSystemData_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

	struct  ipc_mesg_n  pmesg;
    /*get global config*/
	memset(&pmesg,0,sizeof(struct ipc_mesg_n));
    lldp_global_cfg_get(MODULE_ID_SNMPD,&pmesg);

	lldp_snmp_global_msg = (lldp_global *)(pmesg.msg_data);
    if (lldp_snmp_global_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp global config message NULL!\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    switch (vp->magic)
    {
        case lldpLocChassisIdSubtype:
        {
            *var_len = sizeof(int);
            int_value = CHASSIS_ID_MAC_ADDRESS;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocChassisId:
        {
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, &(lldp_snmp_global_msg->source_mac[0]), 6);
            *var_len = 6;

            return str_value;
        }
        break;

        case lldpLocSysName:
        {
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, &lldp_snmp_global_msg->system_name[0], SYSTEM_NAMSIZ);
            *var_len = strlen((char *)str_value);

            return str_value;
        }
        break;

        case lldpLocSysDesc:
        {
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, &lldp_snmp_global_msg->system_desc[0], MAX_SIZE);
            *var_len = strlen((char *)str_value);

            return str_value;
        }
        break;

        case lldpLocSysCapSupported:
        {
            *var_len = sizeof(int);
            int_value = 0x0014;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocSysCapEnabled:
        {
            *var_len = sizeof(int);
            int_value = 0x0014;

            return (unsigned char *)&int_value;
        }
        break;

        default :
            return NULL ;
    }
}

u_char *
lldpLocPortTable_get(struct variable *vp,
                     oid *name,
                     size_t *length,
                     int exact, size_t *var_len, WriteMethod **write_method)
{
	struct lldp_loc_port_snmp index;
	int ret = 0;
	struct lldp_loc_port_snmp *lldp_loc_port = NULL;
	
	*write_method = NULL;
	
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	if (ret < 0)
	{
		return NULL;
	}
	
	if (NULL == lldp_loc_port_cache)
	{
		lldp_loc_port_cache = snmp_cache_init(sizeof(struct lldp_loc_port_snmp) ,
																lldp_loc_port_info_get_data_from_ipc ,
																lldp_loc_port_node_lookup);
	
		if (NULL == lldp_loc_port_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}
		
	lldp_loc_port = snmp_cache_get_data_by_index (lldp_loc_port_cache, exact, &index);
	if (NULL == lldp_loc_port)
	{
		return NULL;
	}
	
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, lldp_loc_port->ifindex);
	}

    switch (vp->magic)
    {
        case lldpLocPortIdSubtype:
        {
            *var_len = sizeof(int);
            int_value = PORT_ID_INTERFACE_NAME;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocPortId:
        {
            memset(str_value, 0, STRING_LEN);

			memcpy(str_value, &lldp_loc_port->if_name[0], INTERFACE_NAMSIZ);
			*var_len = strlen((char *)str_value);

            return str_value;
        }
        break;

        case lldpLocPortDesc:
        {
            memset(str_value, 0, STRING_LEN);

			memcpy(str_value, &lldp_loc_port->alias[0], strlen(lldp_loc_port->alias));
            *var_len = strlen((char *)str_value);

            return str_value;
        }
        break;

        default:
            return NULL;
    }
}

u_char *
lldpLocPortTable_priv_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
	struct lldp_loc_port_priv_snmp index;
	int ret = 0;
	struct lldp_loc_port_priv_snmp *lldp_loc_port_priv = NULL;
	
	*write_method = NULL;
	
	ret = ipran_snmp_int_index_get(vp, name, length, &index.ifindex, exact);
	if (ret < 0)
	{
		return NULL;
	}
	
	if (NULL == lldp_loc_port_priv_cache)
	{
		lldp_loc_port_priv_cache = snmp_cache_init(sizeof(struct lldp_loc_port_priv_snmp) ,
																lldp_loc_port_priv_info_get_data_from_ipc ,
																lldp_loc_port_priv_node_lookup);
	
		if (NULL == lldp_loc_port_priv_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}
		
	lldp_loc_port_priv = snmp_cache_get_data_by_index (lldp_loc_port_priv_cache, exact, &index);
	if (NULL == lldp_loc_port_priv)
	{
		return NULL;
	}
	
	if (!exact)
	{
		ipran_snmp_int_index_set(vp, name, length, lldp_loc_port_priv->ifindex);
	}

    switch (vp->magic)
    {
        case lldpNeighborNum:
        {
            *var_len = sizeof(int);
			int_value = lldp_loc_port_priv->msap_num;
            return (unsigned char *)&int_value;
        }
        break;

        case lldplldpLocPortDesc:
        {
            memset(str_value, 0, STRING_LEN);

			memcpy(str_value, &lldp_loc_port_priv->if_name[0], INTERFACE_NAMSIZ);
			*var_len = strlen((char *)str_value);

            return str_value;
        }

        case lldplldpLocPhysAddress:
        {
            memset(str_value, 0, STRING_LEN);

			memcpy(str_value, &(lldp_loc_port_priv->source_mac[0]), 6);
			*var_len = 6;
            return str_value;
        }

        case hhrlldpLocPortArpLearning:
        {
            *var_len = sizeof(int);
			int_value = lldp_loc_port_priv->arp_nd_learning;
            return (unsigned char *)&int_value;
        }

        case hhrlldpLocPortArpLearningVlan:
        {
            *var_len = sizeof(int);
			int_value = lldp_loc_port_priv->arp_nd_learning_vlan;
            return (unsigned char *)&int_value;
        }

        default:
            return NULL;
    }
}

#if 0

/*need to be change*/
u_char *
lldpLocManAddrTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
    u_int next_index = 0;
    int ret = 0;

    u_int index = 0;
    struct mac_addr addr = {};

    struct mac_addr addr1 = {11, 12, 13, 14, 15, 16};

    ret = ipran_snmp_int_mac_index_get(vp, name, length, &index, &addr, exact);

    if (ret < 0)
    {
        return NULL;
    }

#if 0
    zlog_debug(SNMP_DBG_MIB_GET,"index =[ %d] addr %8x : %8x :%8x :%8x :%8x: %8x\r\n", index,
               addr.addr[0], addr.addr[1], addr.addr[2],
               addr.addr[3], addr.addr[4], addr.addr[5]);
#endif

    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }


#if 0
    lldp_if = lldp_if_lookup_by_mac_addr(&addr, exact);

    if (lldp_if == NULL)
    {
        return NULL;
    }

#endif

    if (!exact)
    {
        ipran_snmp_int_mac_index_set(vp, name, length, 2, &addr1);
    }

    /*get port lldp value*/
    lldp_snmp_port_msg = lldp_port_info_get(index, MODULE_ID_SNMPD);

    if (lldp_snmp_port_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp port[%d] config message NULL!\n", __FUNCTION__, __LINE__, next_index);
        return NULL;
    }


    /*get global config*/
    lldp_snmp_global_msg = lldp_global_cfg_get(MODULE_ID_SNMPD);

    if (lldp_snmp_global_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp global config message NULL!\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    switch (vp->magic)
    {
        case lldpLocManAddrLen:
        {
            *var_len = sizeof(int);

            if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_ALL802)
            {
                int_value = MAC_LEN;

            }
            else if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_IPV4)
            {
                int_value = IPV4_LEN;
            }

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocManAddrIfSubtype:
        {
            *var_len = sizeof(int);
            int_value = IF_NUMBERING_IFINDEX;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocManAddrIfId:
        {
            *var_len = sizeof(int);
            int_value = lldp_snmp_port_msg->if_index;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpLocManAddrOID:
        {
            return NULL;
        }
        break;

        default :
            return NULL ;
    }

}
#endif


u_char *
lldpRemTable_get(struct variable *vp,
                 oid *name,
                 size_t *length,
                 int exact, size_t *var_len, WriteMethod **write_method)
{
    unsigned int        intval = 0;
    unsigned int        time_mark = 0;
    unsigned int        port_num = 0;
    unsigned int        remote_index = 0;
    int                 ret = 0, i = 0;
    //struct lldp_msap    *lldp_port_msap = NULL;
    struct lldp_msap_snmp *lldp_msap = NULL;
    struct lldp_msap_snmp index;

    ret = ipran_snmp_intx3_index_get(vp, name, length, &time_mark,
                                     &port_num, &remote_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ifindex = port_num;
    index.lldp_msap_info.msap_index = remote_index;

//  zlog_debug(SNMP_DBG_MIB_GET,"time_mark [%d] port_num [%d] remote_index[%d]\r\n" , time_mark , port_num , remote_index);
    //get next index------------------
#if 0
    lldp_port_msap = lldp_remote_port_table_get((time_t *) & time_mark, &port_num,
                                                &remote_index, exact);

    if (lldp_port_msap == NULL)
    {
        return NULL;
    }

#else

    if (NULL == lldp_rem_cache)
    {
        lldp_rem_cache = snmp_cache_init(sizeof(struct lldp_msap_snmp) ,
                                         lldp_msap_info_get_data_from_ipc ,
                                         lldp_msap_info_node_lookup);

        if (NULL == lldp_rem_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    lldp_msap = snmp_cache_get_data_by_index(lldp_rem_cache , exact, &index);

    if (NULL == lldp_msap)
    {
        return NULL;
    }

#endif


    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, time_mark,
                                   lldp_msap->ifindex, lldp_msap->lldp_msap_info.msap_index);
    }

    switch (vp->magic)
    {
        case lldpRemChassisIdSubtype:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.chassis_id_subtype;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpRemChassisId:
        {
            memset(str_value, 0, STRING_LEN);

            if (lldp_msap->lldp_msap_info.chassis_id_subtype == CHASSIS_ID_MAC_ADDRESS)
            {
                memcpy(str_value, &(lldp_msap->lldp_msap_info.chassis_id_addr[0]), 6);
                *var_len = 6;
            }
            else if (lldp_msap->lldp_msap_info.chassis_id_subtype == CHASSIS_ID_NETWORK_ADDRESS)
            {
                memcpy(str_value, &(lldp_msap->lldp_msap_info.chassis_id_addr[0]), 4);
                *var_len = 4;
            }

            return str_value;
        }
        break;

        case lldpRemPortIdSubtype:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.port_id_subtype;

            return (unsigned char *)&int_value;

        }
        break;

        case lldpRemPortId:
        {
            memset(str_value, 0, STRING_LEN);

            if (lldp_msap->lldp_msap_info.port_id_subtype == PORT_ID_INTERFACE_NAME)
            {
                memcpy(str_value, &lldp_msap->lldp_msap_info.port_id[0], INTERFACE_NAMSIZ);
                *var_len = strlen((char *)str_value);
            }
            else if (lldp_msap->lldp_msap_info.chassis_id_subtype == PORT_ID_NETWORK_ADDRESS)
            {
                memcpy(str_value, &lldp_msap->lldp_msap_info.port_id[0], 4);
                *var_len = strlen((char *)str_value);
            }

            return str_value;
        }
        break;

        case lldpRemPortDesc:
        {
            memset(str_value, 0, STRING_LEN);

            if (lldp_check_array_value(lldp_msap->lldp_msap_info.port_descr, STRING_LEN))
            {
                memcpy(str_value, &lldp_msap->lldp_msap_info.port_descr[0], 
									strlen(lldp_msap->lldp_msap_info.port_descr));
                *var_len = strlen((char *)str_value);

                return str_value;
            }
            else
            {

                LLDP_SNMP_RETURN_STRING("null", 4);
            }

        }
        break;

        case lldpRemSysName:
        {
            memset(str_value, 0, STRING_LEN);

            if (lldp_check_array_value(lldp_msap->lldp_msap_info.sys_name, MAX_SIZE))
            {
                memcpy(str_value, &lldp_msap->lldp_msap_info.sys_name[0], MAX_SIZE);
                *var_len = strlen((char *)str_value);

                return str_value;
            }
            else
            {
                LLDP_SNMP_RETURN_STRING("null", 4);
            }
        }
        break;

        case lldpRemSysDesc:
        {
            memset(str_value, 0, STRING_LEN);

            if (lldp_check_array_value(lldp_msap->lldp_msap_info.sys_descr, SYSTEM_DESCR_MAX_SIZE))
            {
                memcpy(str_value, &lldp_msap->lldp_msap_info.sys_descr[0], SYSTEM_DESCR_MAX_SIZE);
                *var_len = strlen((char *)str_value);

                return str_value;
            }
            else
            {
                LLDP_SNMP_RETURN_STRING("null", 4);
            }
        }
        break;

        case lldpRemSysCapSupported:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.sys_cap;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpRemSysCapEnabled:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.sys_cap_enabled;

            return (unsigned char *)&int_value;
        }
        break;

        default :
            return NULL;
    }
}

u_char *
lldpRemTable_priv_get(struct variable *vp,
                      oid *name,
                      size_t *length,
                      int exact, size_t *var_len, WriteMethod **write_method)
{
    unsigned int intval = 0;
    u_int32_t time_mark = 0;
    u_int32_t port_num = 0;
    u_int32_t remote_index = 0;
    int ret = 0;
    //struct lldp_msap        *lldp_port_msap = NULL;
    struct lldp_msap_snmp *lldp_msap = NULL;
    struct lldp_msap_snmp index;

    ret = ipran_snmp_intx3_index_get(vp, name, length, &time_mark,
                                     &port_num, &remote_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ifindex = port_num;
    index.lldp_msap_info.msap_index = remote_index;

    /*get remote next index*/
#if 0
    lldp_port_msap = lldp_remote_port_table_get((time_t *) & time_mark, &port_num,
                                                &remote_index, exact);

    if (lldp_port_msap == NULL)
    {
        return NULL;
    }

#else

    if (NULL == lldp_rem_cache)
    {
        lldp_rem_cache = snmp_cache_init(sizeof(struct lldp_msap_snmp) ,
                                         lldp_msap_info_get_data_from_ipc ,
                                         lldp_msap_info_node_lookup);

        if (NULL == lldp_rem_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    lldp_msap = snmp_cache_get_data_by_index(lldp_rem_cache , exact, &index);

    if (NULL == lldp_msap)
    {
        return NULL;
    }

#endif

    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, time_mark,
                                   lldp_msap->ifindex, lldp_msap->lldp_msap_info.msap_index);
    }

    switch (vp->magic)
    {
        case lldpSvlanType:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.svlan_tpid;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpSvlanId:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.svlan;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpCvlanType:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.cvlan_tpid;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpCvlanId:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.cvlan;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpExpiredTime:
        {
            *var_len = sizeof(int);
            int_value = lldp_msap->lldp_msap_info.rx_ttl;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpMgmtAddrSubType:
        {
            *var_len = sizeof(int);

            if (lldp_msap->lldp_msap_info.mgmt_addr_sub_type == 6) /*type:MAC */
            {
                int_value = 2;
            }
            else if (lldp_msap->lldp_msap_info.mgmt_addr_sub_type == 1)
            {
                int_value = 1;
            }

            return (unsigned char *)&int_value;
        }
        break;

        case lldpMgmtAddr:
        {
            if (lldp_msap->lldp_msap_info.mgmt_addr_sub_type == MGMT_ADDR_SUB_ALL802)
            {            
				memcpy(str_value, &(lldp_msap->lldp_msap_info.mgmt_addr[0]), 6);
                *var_len = MAC_LEN;
            }
            else if (lldp_msap->lldp_msap_info.mgmt_addr_sub_type == MGMT_ADDR_SUB_IPV4)
            {            
				memcpy(str_value, &(lldp_msap->lldp_msap_info.mgmt_addr[0]), 4);
                *var_len = IPV4_LEN;
            }
			
            return str_value;
        }
        break;

        default :
            return NULL ;
    }
}



u_char *
lldpManagementAddressSubType_priv_get(struct variable *vp,
                                      oid *name,
                                      size_t *length,
                                      int exact, size_t *var_len, WriteMethod **write_method)

{
    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

   struct  ipc_mesg_n  pmesg;
    /*get global config*/
	memset(&pmesg,0,sizeof(struct ipc_mesg_n));
    lldp_global_cfg_get(MODULE_ID_SNMPD,&pmesg);

	lldp_snmp_global_msg = (lldp_global *)(pmesg.msg_data);
    if (lldp_snmp_global_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp global config message NULL!\n", __FUNCTION__, __LINE__);

        return NULL;
    }

    switch (vp->magic)
    {
        case hhrlldpManagementAddressSubType:
        {
            *var_len = sizeof(int);

            if (lldp_snmp_global_msg->mng_addr_sub == 6) /*type:MAC */
            {
                int_value = 2;
            }
            else if (lldp_snmp_global_msg->mng_addr_sub == 1)
            {
                int_value = 1;
            }

            return (unsigned char *)&int_value;
        }
        break;


		case hhrlldpManagementAddress:
		{
            if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_ALL802)
            {            
				memcpy(str_value, &(lldp_snmp_global_msg->source_mac[0]), 6);
                *var_len = MAC_LEN;
            }
            else if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_IPV4)
            {            
				memcpy(str_value, &(lldp_snmp_global_msg->source_ipaddr[0]), 4);
                *var_len = IPV4_LEN;
            }

            return str_value;
		}	
		break;


        default :
            return NULL;
    }

    return NULL;
}


u_char *
lldpV2LocManAddrTable_get(struct variable *vp,
                          oid *name,
                          size_t *length,
                          int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t index = 0;
    int ret = 0;
    struct mac_addr addr = {};
    uint32_t ip_index = 0;
    uint8_t ip_index_flag = 0;
    uint32_t ip = 0;

    *write_method = NULL;
	struct	ipc_mesg_n	pmesg;
	   /*get global config*/
   memset(&pmesg,0,sizeof(struct ipc_mesg_n));
   lldp_global_cfg_get(MODULE_ID_SNMPD,&pmesg);

	lldp_snmp_global_msg = (lldp_global *)(pmesg.msg_data);
    if (lldp_snmp_global_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] get lldp global config message NULL!\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_ALL802)
    {
        /* validate the index */
        ret = ipran_snmp_int_mac_index_get(vp, name, length, &index,
                                           (uint8_t *)&addr, exact);

    }
    else if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_IPV4)
    {
        ret = ipran_snmp_int_ip_index_get(vp, name, length, &index,
                                          &ip_index, exact);
        ip_index_flag = 1;

        ip = (uint32_t)((lldp_snmp_global_msg->source_ipaddr[0] << 24) +
                        (lldp_snmp_global_msg->source_ipaddr[1] << 16) +
                        (lldp_snmp_global_msg->source_ipaddr[2] << 8) +
                        lldp_snmp_global_msg->source_ipaddr[3]);
    }

    if (ret < 0)
    {
        return NULL;
    }

    if (!exact)
    {
        if (!ip_index_flag)
        {
            if ((index == 6) &&
                    (memcmp(lldp_snmp_global_msg->source_mac, addr.addr, 6) == 0))
            {
                return SNMP_ERR_NOERROR;
            }

            ipran_snmp_int_mac_index_set(vp, name, length,
                                         6, lldp_snmp_global_msg->source_mac);
        }
        else
        {
            if ((index == 1) && (ip == ip_index))
            {
                return SNMP_ERR_NOERROR;
            }

            ipran_snmp_int_ip_index_set(vp, name, length, 1, ip);
        }
    }
    else
    {
        if (!ip_index_flag)
        {
            if (memcmp(lldp_snmp_global_msg->source_mac, addr.addr, 6) != 0)
            {
                return NULL;
            }
        }
        else
        {
            if (ip != ip_index)
            {
                return NULL;
            }
        }
    }

    switch (vp->magic)
    {
        case lldpV2LocManAddrLen:
        {
            *var_len = sizeof(int);

            if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_ALL802)
            {
                int_value = MAC_LEN;
            }
            else if (lldp_snmp_global_msg->mng_addr_sub == MGMT_ADDR_SUB_IPV4)
            {
                int_value = IPV4_LEN;
            }

            return (unsigned char *)&int_value;
        }
        break;

#if 0

        case lldpV2LocManAddrIfSubtype:
        {
            *var_len = sizeof(int);
            int_value = 0;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpV2LocManAddrIfId:
        {
            *var_len = sizeof(int);
            int_value = 0;

            return (unsigned char *)&int_value;
        }
        break;

        case lldpV2LocManAddrOID:
            LLDP_SNMP_RETURN_STRING("null", 4);
            break;
#endif

        default:
            return NULL;
    }

    return NULL;
}


/* Utility function to get a 4 Integer indices + MAC address */
int
lldp_snmp_int4_mac_addr_index_get (struct variable *v, oid *name,
                                   size_t *length, time_t *index1,
                                   uint32_t *index2, uint32_t *index3,
                                   uint32_t *index4, struct mac_addr *addr,
                                   int exact)
{
	int maclen, len;

	if (exact)
	{
		/* Check the length. */
		if (*length - v->namelen != sizeof (struct mac_addr) + 4)
		{
			return -1;
		}
		
		*index1 = name[v->namelen];
      	*index2 = name[v->namelen + 1];
      	*index3 = name[v->namelen + 2];
      	*index4 = name[v->namelen + 3];

      	if ( oid2bytes (name + v->namelen + 4, sizeof (struct mac_addr),
                      addr->addr) != 0 )
      	{
        	return -1;
      	}
		
      	return 0;
	}
  	else
    {
		memset( addr, 0, sizeof(struct mac_addr) );
      	*index1 = 0;
      	*index2 = 0;
      	*index3 = 0;
      	*index4 = 0;

      	len = *length - v->namelen;
		if ( len >= 4)
		{
        	*index1 = name[v->namelen];
        	*index2 = name[v->namelen + 1];
        	*index3 = name[v->namelen + 2];
        	*index4 = name[v->namelen + 3];
		}

		maclen = *length - (v->namelen + 4);
      	if (maclen > sizeof (struct mac_addr))
      	{
        	maclen = sizeof (struct mac_addr);
      	}

		if ( oid2bytes (name + v->namelen + 4, maclen, addr->addr) != 0 )
		{
        	return -1;
		}

		return 0;
	}

	return -1;
}


/* Utility function to set a 4 Integer Indices + MAC address.  */
void
lldp_snmp_int4_mac_addr_index_set (struct variable *v, oid *name,
                                   size_t *length, time_t index1,
                                   uint32_t index2, uint32_t index3,
                                   int32_t index4, struct mac_addr *addr )
{
	oid_copy (name, v->name, v->namelen * sizeof(oid) );

	name[v->namelen]     = index1;
	name[v->namelen + 1] = index2;
	name[v->namelen + 2] = index3;
	name[v->namelen + 3] = index4;

	*length = v->namelen + 4;

	oid_copy_bytes2oid (name + v->namelen + 4, addr->addr,
                      sizeof (struct mac_addr));
	*length += sizeof (struct mac_addr);
}


int
rlldp_if_get_index_rem_ix (struct lldp_msap_man_addr_snmp **rlldp1, 
							struct lldp_msap_man_addr_index *index_input)
{
	int ret = 0;
	struct listnode *node, *nnode;
	struct lldp_msap_man_addr_snmp *data_find = NULL;
	struct lldp_msap_man_addr_snmp *data = NULL;
	int i;
	uint32_t remote_index_next = 0;
	uint8_t local_flag= 0;
	
	for (ALL_LIST_ELEMENTS(lldp_rem_man_addr_cache->data_list, node, nnode, data_find))
	{
		if(index_input->ifindex == data_find->ifindex)
		{
			local_flag = 1;
			data = data_find;
			break;
		}
	}

	if (!local_flag || !data)
	{
		return ret;
	}

	for (i = 0; i < TIME_MARK_INDEX_MAX; i++)
	{
		if (data->lldp_msap_man_addr_info.time_mark[i] == index_input->time_mark)
		{
			if (data->lldp_msap_man_addr_info.msap_index > index_input->remote_index)
			{
				if((!remote_index_next) ||
					(remote_index_next &&
					data->lldp_msap_man_addr_info.msap_index < remote_index_next))
				{
					remote_index_next = data->lldp_msap_man_addr_info.msap_index;
					*rlldp1 = data;
					ret = 1;
				}
			}
		}
	}

	index_input->remote_index = remote_index_next;
	return ret;
}


int
rlldp_if_get_next_port_num (time_t time_mark, int port_num)
{
	int port_num_next = 0;
	int i;
	struct listnode *node, *nnode;
	struct lldp_msap_man_addr_snmp *data_find = NULL;

	for (ALL_LIST_ELEMENTS(lldp_rem_man_addr_cache->data_list, node, nnode, data_find))
	{
		for (i = 0; i < TIME_MARK_INDEX_MAX; i++)
		{
			if (data_find->lldp_msap_man_addr_info.time_mark[i] == time_mark)
			{
				if (data_find->ifindex > port_num)
				{
					if ((!port_num_next) ||
						(data_find->ifindex < port_num_next))
					{
						port_num_next = data_find->ifindex;
					}
				}
			}
		}
	}

	return port_num_next;
}


int
rlldp_if_get_index_port_rem_ix (struct lldp_msap_man_addr_snmp **rlldp_msap,
                                						struct lldp_msap_man_addr_index *index_input)
{
	int ret = 0;
//	int port_num_next = 0;
	uint32_t port_num_next = 0;


	do {
		ret = rlldp_if_get_index_rem_ix (rlldp_msap, index_input);
		if (!ret)
		{
			port_num_next = rlldp_if_get_next_port_num (index_input->time_mark, index_input->ifindex);
			index_input->ifindex = port_num_next;
		}
		else
		{
			break;
		}
	}while(port_num_next > 0);

	if(ret)
	{
    	return 1;
	}
	else
    {
		index_input->ifindex = 0;
      	return 0;
	}
}


time_t
rlldp_if_get_next_time_mark (time_t time_mark)
{
	time_t time_mark_next = 0;
	int i;
	struct listnode *node, *nnode;
	struct lldp_msap_man_addr_snmp *data_find = NULL;

	for (ALL_LIST_ELEMENTS(lldp_rem_man_addr_cache->data_list, node, nnode, data_find))
	{
		for (i = 0; i < TIME_MARK_INDEX_MAX; i++)
		{
			if (data_find->lldp_msap_man_addr_info.time_mark[i] > time_mark)
			{
				if ((!time_mark_next) ||
					(time_mark_next && data_find->lldp_msap_man_addr_info.time_mark[i] < time_mark_next))
				{
					time_mark_next = data_find->lldp_msap_man_addr_info.time_mark[i];
				}
			}
		}
	}

	return time_mark_next;
}


struct lldp_msap_man_addr_snmp *
rlldp_if_rem_table_lookup_next (struct lldp_msap_man_addr_index *index_input)
{
	struct lldp_msap_man_addr_snmp *rlldp_msap = NULL;
	int ret = 0;
	time_t time_mark_next = 0;

	do {
		ret = rlldp_if_get_index_port_rem_ix (&rlldp_msap, index_input);
		if (!ret)
		{
			time_mark_next = rlldp_if_get_next_time_mark (index_input->time_mark);
			index_input->time_mark = time_mark_next;
		}
		else
		{
			break;
		}

	}while(time_mark_next);

	if (ret)
	{
    	return rlldp_msap;
	}
	else
	{
    	return NULL;
	}
}


struct lldp_msap_man_addr_snmp *
rlldp_if_rem_table_lookup(struct ipran_snmp_data_cache *cache,
							int exact, struct lldp_msap_man_addr_index  *index_input)
{
	struct listnode *node, *nnode;
	struct lldp_msap_man_addr_snmp *data_find = NULL;
	int i;
	struct lldp_msap_man_addr_snmp *data = NULL;
	uint8_t local_flag= 0;
	
	if (exact)
    {
		for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data_find))
		{
			if(index_input->ifindex == data_find->ifindex)
			{
				local_flag = 1;
				data = data_find;
				break;
			}
		}

		if (!local_flag || !data)
		{
			return 0;
		}

		for (i = 0; i < TIME_MARK_INDEX_MAX; i++)
		{
			if (data->lldp_msap_man_addr_info.time_mark[i] > index_input->time_mark)
			{
				if (data->lldp_msap_man_addr_info.msap_index == index_input->remote_index)
				{
                  	return data;
				}
			}
		}
    }
	else
    {
		data = rlldp_if_rem_table_lookup_next(index_input);
    }

	return data;
}


#if 1
u_char *
lldpRemManAddrTable_get(struct variable *vp,
                        oid *name,
                        size_t *length,
                        int exact, size_t *var_len, WriteMethod **write_method)
{
	time_t time_mark = 0;
	uint32_t port_num = 0;
	uint32_t remote_index = 0;
	uint32_t man_addr_sub_type = 0;
	struct mac_addr addr;
	int ret = 0;
	struct lldp_msap_man_addr_snmp *lldp_msap_man_addr = NULL;
	struct lldp_msap_man_addr_index index;

	*write_method = NULL;

	/* validate the index */
	ret = ipran_snmp_intx4_mac_index_get (vp, name, length, &time_mark, &port_num, &remote_index, 
																		&man_addr_sub_type, &addr, exact);
	if (ret < 0)
	{
		return NULL;
	}

	index.ifindex = 	port_num;
	index.remote_index = remote_index;
	index.time_mark = time_mark;
	
	if (NULL == lldp_rem_man_addr_cache)
	{
		lldp_rem_man_addr_cache = snmp_cache_init(sizeof(struct lldp_msap_man_addr_snmp) ,
																lldp_msap_man_addr_info_get_data_from_ipc ,
																rlldp_if_rem_table_lookup);
	
		if (NULL == lldp_rem_man_addr_cache)
		{
			zlog_debug(SNMP_DBG_MIB_GET,"%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
			return NULL;
		}
	}

	lldp_msap_man_addr = snmp_cache_get_data_by_index (lldp_rem_man_addr_cache, exact, &index);
	if (NULL == lldp_msap_man_addr)
	{
		return NULL;
	}

	if (!exact)
	{
		lldp_snmp_int4_mac_addr_index_set(vp, name, length, 
												index.time_mark, index.ifindex, index.remote_index,
												lldp_msap_man_addr->lldp_msap_man_addr_info.mgmt_addr_sub_type,
												(struct mac_addr *)lldp_msap_man_addr->lldp_msap_man_addr_info.mgmt_addr);
	}

    switch (vp->magic)
    {
        case lldpRemManAddrIfSubtype:
        {
			*var_len = sizeof(int);
			int_value = lldp_msap_man_addr->lldp_msap_man_addr_info.if_numbering;
			return (unsigned char *)&int_value;
        }

        case lldpRemManAddrIfId:
        {
			*var_len = sizeof(int);
            int_value = lldp_msap_man_addr->lldp_msap_man_addr_info.if_number;
            return (unsigned char *)&int_value;
        }

        case lldpRemManAddrOID:
        {
			int_value = 0;
            *var_len = 1;
            return (unsigned char *)&int_value;
        }

        default :
            return NULL;
    }
}
#endif
