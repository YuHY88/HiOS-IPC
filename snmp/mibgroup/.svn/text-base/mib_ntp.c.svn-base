/*
 *  ntp snmp browser interface - ntp_snmp.c
 *
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"
#include "snmp_config_table.h"

#include "mib_ntp.h"
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#define NTP_OIDPREFIX 1,3,6,1,4,1,9966,5,35,8,12

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

FindVarMethod ntp_configglobal_get;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

struct variable2 ntpConfGblVariables[] =
{
    {ntpMode,               ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {1}},
    {ntpPacket,             ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {2}},
    {ntpSyncInterval,       ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {3}},
    {ntpVersion,            ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {4}},
    {ntpServerOrPeer,       ASN_IPADDRESS, RONLY, ntp_configglobal_get, 1, {5}},
    {ntpClientEnable,       ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {6}},
    {ntpSyncState,          ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {7}},
    {ntpDelayTime,          ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {8}},
    {ntpOffsetTime,         ASN_INTEGER, RONLY, ntp_configglobal_get, 1, {9}},
    {ntpReferenceClockID,   ASN_OCTET_STR, RONLY, ntp_configglobal_get, 1, {10}},
} ;
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

oid  ntpConfGblOid[]   = {NTP_OIDPREFIX};
ntp_globals *ntp_snmp_global_msg = NULL;
static int int_value;
static unsigned char puchr[4] = {0, 0, 0, 0};
static uchar str_value[STRING_LEN] = {'\0'};
static uint32_t           ip_value = 0;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void  init_mib_ntp(void)
{
    REGISTER_MIB("ipran/ntpConfGbl", ntpConfGblVariables, variable2, ntpConfGblOid);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
ntp_globals ntp_glb_cfg_common;

/*return value: lldp local cfg value*/
ntp_globals *ntp_global_cfg_get(int module_id)
{
    struct ipc_mesg_n * psnd_msg = NULL;
    psnd_msg = ipc_sync_send_n2(NULL,1,1,MODULE_ID_NTP,module_id,IPC_TYPE_SNMP,IPC_TYPE_SNMP_NTP_GLOBAL_CFG,IPC_OPCODE_GET,1,2000);
        
    if (psnd_msg == NULL)
    {
        zlog_debug(SNMP_DBG_MIB_GET,"%s[%d] send IPC get info failed! \n", __FUNCTION__, __LINE__);
        return NULL;
    }
    memcpy(&ntp_glb_cfg_common, psnd_msg->msg_data, sizeof(ntp_globals));
    if(psnd_msg)
    {
        mem_share_free(psnd_msg, MODULE_ID_SNMPD);
    }

    return &ntp_glb_cfg_common;
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

u_char *ntp_configglobal_get(struct variable *pVar,
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

    ntp_snmp_global_msg = ntp_global_cfg_get(MODULE_ID_SNMPD);

    if (ntp_snmp_global_msg == NULL)
    {
        return NULL;
    }

    if (iRetVal == 0)
    {
        switch (pVar->magic)
        {
            case ntpMode:
                *iVarlen = sizeof(int);

                if (CLINETMODE == ntp_snmp_global_msg->uiWorkMode)
                {
                    int_value = 2;
                }
                else
                {
                    int_value = 1;
                }

                return ((u_char *)&int_value);

            case ntpPacket:
                *iVarlen = sizeof(int);

                if (strcmp((char *)"broadcast", (char *)ntp_snmp_global_msg->ucPacketFormat) == 0)
                {
                    int_value = 2;
                }
                else
                {
                    int_value = 1;
                }

                return ((u_char *)&int_value);

            case ntpSyncInterval:
                *iVarlen = sizeof(int);
                int_value = ntp_snmp_global_msg->uiSyncInterval;
                return ((u_char *)&int_value);

            case ntpVersion:
                *iVarlen = sizeof(int);
                int_value = ntp_snmp_global_msg->uiVersion;
                return ((u_char *)&int_value);

            case ntpServerOrPeer:
                ip_value =  htonl(ntp_snmp_global_msg->uiServerIP);
                return ((u_char *)&ip_value);

            case ntpClientEnable:
                *iVarlen = sizeof(int);

                if (ntp_snmp_global_msg->uiClientEnable == NTPDISABLE)
                {
                    int_value = 2;
                }
                else
                {
                    int_value = 1;
                }

                return ((u_char *)&int_value);

            case ntpSyncState:
                *iVarlen = sizeof(int);

                if (ntp_snmp_global_msg->uiSyncState)
                {
                    int_value = 2;
                }
                else
                {
                    int_value = 1;
                }

                return ((u_char *)&int_value);

            case ntpDelayTime:
                *iVarlen = sizeof(int);
                int_value = ntp_snmp_global_msg->stTimeDelay.tv_sec * 1000000 + ntp_snmp_global_msg->stTimeDelay.tv_usec;
                return ((u_char *)&int_value);

            case ntpOffsetTime:
                *iVarlen = sizeof(int);
                int_value = ntp_snmp_global_msg->stTimeOffset.tv_sec * 1000000 + ntp_snmp_global_msg->stTimeOffset.tv_usec;
                return ((u_char *)&int_value);

            case ntpReferenceClockID:
                memcpy(str_value, ntp_snmp_global_msg->ucReferID, 16);
                *iVarlen = strlen(str_value);
                return  str_value;

            default :
                return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}



