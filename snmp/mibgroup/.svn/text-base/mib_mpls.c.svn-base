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
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc.h>
#include <lib/mpls_common.h>
#include <lib/linklist.h>
#include <mpls/vpls.h>

#include "mpls/mpls_aps/mpls_aps.h"
#include "ipran_snmp_data_cache.h"

#include "mib_mpls.h"

enum SNMP_MPLS_ENABLE_VAL
{
    SNMP_MPLS_ENABLE = 1,
    SNMP_MPLS_DISABLE
};

#define                   MPLS_STRING_LEN           256
#define                   MPLS_MAX_NAME_LEGTH       NAME_STRING_LEN

/*local temp variable*/
static uchar              str_value[MPLS_STRING_LEN] = {'\0'};
static uchar              mac_value[6] = {0};
static int                int_value = 0;
static uint32_t           ip_value = 0;
static uint32_t           uint_value = 0;
static struct counter64   uint64_value;

struct vsi_if_snmp
{
    uint vsi_id;
    uint vsi_if_id;
};

/*
 * Object ID definitions
 */
/*hhrMplsconfigComm*/
static oid hhrMplsconfigComm_oid[] = {HHRMPLSCONFIG};
FindVarMethod hhrMplsconfigComm_get;
struct variable1 hhrMplsconfigComm_variables[] =
{
    {hhrMplsEnable, ASN_INTEGER,   RONLY, hhrMplsconfigComm_get, 1, {1}},
    {hhrMplsLsrID,  ASN_IPADDRESS, RONLY, hhrMplsconfigComm_get, 1, {2}}
};

/*hhrLspConfigTable*/
static struct ipran_snmp_data_cache *hhrLspConfigTable_cache = NULL;
static oid hhrLspConfigTable_oid[] = {HHRMPLSCONFIG, 10};
FindVarMethod hhrLspConfigTable_get;
struct variable2 hhrLspConfigTable_variables[] =
{
    {hhrLspNodeType,       ASN_INTEGER,   RONLY, hhrLspConfigTable_get, 2, {1,  2}},
    {hhrLspDestIP,         ASN_IPADDRESS, RONLY, hhrLspConfigTable_get, 2, {1,  3}},
    {hhrLspDestMask,       ASN_IPADDRESS, RONLY, hhrLspConfigTable_get, 2, {1,  4}},
    {hhrLspOutIF,          ASN_OCTET_STR, RONLY, hhrLspConfigTable_get, 2, {1,  5}},
    {hhrLspDestMac,        ASN_OCTET_STR, RONLY, hhrLspConfigTable_get, 2, {1,  6}},
    {hhrLspNexthop,        ASN_IPADDRESS, RONLY, hhrLspConfigTable_get, 2, {1,  7}},
    {hhrLspInLabel,        ASN_UNSIGNED,  RONLY, hhrLspConfigTable_get, 2, {1,  8}},
    {hhrLspOutLabel,       ASN_UNSIGNED,  RONLY, hhrLspConfigTable_get, 2, {1,  9}},
    {hhrLspTpOamSessionId, ASN_UNSIGNED,  RONLY, hhrLspConfigTable_get, 2, {1, 10}},
    {hhrLspIndex,          ASN_UNSIGNED,  RONLY, hhrLspConfigTable_get, 2, {1, 11}},
    {hhrLspLinkStatus,     ASN_INTEGER,   RONLY, hhrLspConfigTable_get, 2, {1, 12}},
    {hhrLspAdminStatus,    ASN_INTEGER,   RONLY, hhrLspConfigTable_get, 2, {1, 13}}
};

/*hhrPseudoWireTable*/
static struct ipran_snmp_data_cache *hhrPseudoWireTable_cache = NULL;
static oid hhrPseudoWireTable_oid[] = {HHRMPLSCONFIG, 11};
FindVarMethod hhrPseudoWireTable_get;
struct variable2 hhrPseudoWireTable_variables[] =
{
    {hhrVCid,                 ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1,  2}},
    {hhrPWProtocolType,       ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1,  3}},
    {hhrPWVCType,             ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1,  4}},
    {hhrPWDestination,        ASN_IPADDRESS, RONLY, hhrPseudoWireTable_get, 2, {1,  5}},
    {hhrPWInLabel,            ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1,  6}},
    {hhrPWOutLabel,           ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1,  7}},
    {hhrPWRelatedTunnelName,  ASN_OCTET_STR, RONLY, hhrPseudoWireTable_get, 2, {1,  8}},
    {hhrPWControlWord,        ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1,  9}},
    {hhrPWEncapsulationType,  ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 10}},
    {hhrPWCESFrameNum,        ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 11}},
    {hhrPWTPID,               ASN_OCTET_STR, RONLY, hhrPseudoWireTable_get, 2, {1, 12}},
    {hhrPWVlan,               ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 13}},
    {hhrPWmtu,                ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 14}},
    {hhrRelatedVplsid,        ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 15}},
    {hhrPWInVplsNetLocation,  ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 16}},
    {hhrPWIndex,              ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 17}},
    {hhrPWSwitchPWIndex,      ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 18}},
    {hhrPWEnableMip,          ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 19}},
    {hhrPWTpOamSessionId,     ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 20}},
    {hhrPWStatisticsEnable,   ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 21}},
    {hhrPWTunnelPolicyLdp,    ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 22}},
    {hhrPWExpDomain,          ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 23}},
    {hhrPWPhd,                ASN_UNSIGNED,  RONLY, hhrPseudoWireTable_get, 2, {1, 24}},
    {hhrPWTunnelPolicymplstp, ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 25}},
    {hhrPWTunnelPolicyGre,    ASN_INTEGER,   RONLY, hhrPseudoWireTable_get, 2, {1, 26}},
    {hhrPWLinkStatus,        ASN_INTEGER,    RONLY, hhrPseudoWireTable_get, 2, {1, 27}},
    {hhrPWAdminStatus,       ASN_INTEGER,    RONLY, hhrPseudoWireTable_get, 2, {1, 28}}
};

/*hhrUNIOfVCTable*/
//ipran_snmp_data_cache is hhrPseudoWireTable_cache
static oid hhrUNIOfVCTable_oid[] = {HHRMPLSCONFIG, 12};
FindVarMethod hhrUNIOfVCTable_get;
struct variable2 hhrUNIOfVCTable_variables[] =
{
    {hhrUNIOfCESIfDESC,          ASN_OCTET_STR, RONLY, hhrUNIOfVCTable_get, 2, {1, 1}},
    {hhrUNIRelatedVCId,          ASN_UNSIGNED,  RONLY, hhrUNIOfVCTable_get, 2, {1, 2}},
    {hhrPWProtectionRole,        ASN_INTEGER,   RONLY, hhrUNIOfVCTable_get, 2, {1, 3}},
    {hhrPWProtectionFailback,    ASN_INTEGER,   RONLY, hhrUNIOfVCTable_get, 2, {1, 4}},
    {hhrPWProtectionFailbackWtr, ASN_UNSIGNED,  RONLY, hhrUNIOfVCTable_get, 2, {1, 5}},
    {hhrPWProtectionWorkStatus,  ASN_INTEGER,   RONLY, hhrUNIOfVCTable_get, 2, {1, 6}}
};

/*hhrVplsServiceTable*/
static struct ipran_snmp_data_cache *hhrVplsServiceTable_cache = NULL;
static oid hhrVplsServiceTable_oid[] = {HHRMPLSCONFIG, 13};
FindVarMethod hhrVplsServiceTable_get;
struct variable2 hhrVplsServiceTable_variables[] =
{
    {hhrVplsId,              ASN_UNSIGNED,  RONLY, hhrVplsServiceTable_get, 2, {1, 1}},
    {hhrVplsName,            ASN_OCTET_STR, RONLY, hhrVplsServiceTable_get, 2, {1, 2}},
    {hhrVplsMacLearnEnable,  ASN_INTEGER,   RONLY, hhrVplsServiceTable_get, 2, {1, 3}},
    {hhrVplsMacLearnLimit,   ASN_UNSIGNED,  RONLY, hhrVplsServiceTable_get, 2, {1, 4}},
    {hhrVplsEncapsulateType, ASN_INTEGER,   RONLY, hhrVplsServiceTable_get, 2, {1, 5}},
    {hhrVplsEncapsulateTag,  ASN_INTEGER,   RONLY, hhrVplsServiceTable_get, 2, {1, 6}},
    {hhrVplsEncapsulateTpId, ASN_INTEGER,   RONLY, hhrVplsServiceTable_get, 2, {1, 7}},
    {hhrVplsEncapsulateVlan, ASN_UNSIGNED,  RONLY, hhrVplsServiceTable_get, 2, {1, 8}}
};

/*hhrUNIOfVPLSTable*/
static struct ipran_snmp_data_cache *hhrUNIOfVPLSTable_cache = NULL;
static oid hhrUNIOfVPLSTable_oid[] = {HHRMPLSCONFIG, 15};
FindVarMethod hhrUNIOfVPLSTable_get;
struct variable2 hhrUNIOfVPLSTable_variables[] =
{
    {hhrUNIOfVPLSIfDesc, ASN_OCTET_STR, RONLY, hhrUNIOfVPLSTable_get, 2, {1, 1}}
};

/*hhrTunnelConfigTable*/
static struct ipran_snmp_data_cache *hhrTunnelConfigTable_cache = NULL;
static oid hhrTunnelConfigTable_oid[] = {HHRMPLSCONFIG, 16};
FindVarMethod hhrTunnelConfigTable_get;
struct variable2 hhrTunnelConfigTable_variables[] =
{
    {hhrTunnelName,                   ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1,  1}},
    {hhrTunnelEncapsulateType,        ASN_INTEGER,   RONLY, hhrTunnelConfigTable_get, 2, {1,  2}},
    {hhrTunnelSrcIP,                  ASN_IPADDRESS, RONLY, hhrTunnelConfigTable_get, 2, {1,  3}},
    {hhrTunnelDestIP,                 ASN_IPADDRESS, RONLY, hhrTunnelConfigTable_get, 2, {1,  4}},
    {hhrTunnelRelatedIngressLspName,  ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1,  5}},
    {hhrTunnelRelatedEgressLspName,   ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1,  6}},
    {hhrTunnelLinkStatus,             ASN_INTEGER,   RONLY, hhrTunnelConfigTable_get, 2, {1,  7}},
    {hhrTunneBackupIngressLspName,    ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1,  8}},
    {hhrTunnelBackupEgressLspName,    ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1,  9}},
    {hhrTunneOutIf,                   ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1, 10}},
    {hhrTunnelDestMac,                ASN_OCTET_STR, RONLY, hhrTunnelConfigTable_get, 2, {1, 11}},
    {hhrTunnelNexthop,                ASN_IPADDRESS, RONLY, hhrTunnelConfigTable_get, 2, {1, 12}},
    {hhrTunnelQosRateLimitIngressCIR, ASN_UNSIGNED,  RONLY, hhrTunnelConfigTable_get, 2, {1, 13}},
    {hhrTunnelQosRateLimitIngressPIR, ASN_UNSIGNED,  RONLY, hhrTunnelConfigTable_get, 2, {1, 14}},
    {hhrTunnelQosRateLimitEgressCIR,  ASN_UNSIGNED,  RONLY, hhrTunnelConfigTable_get, 2, {1, 15}},
    {hhrTunnelQosRateLimitEgressPIR,  ASN_UNSIGNED,  RONLY, hhrTunnelConfigTable_get, 2, {1, 16}},
    {hhrTunnelWorkingStatus,          ASN_INTEGER,   RONLY, hhrTunnelConfigTable_get, 2, {1, 17}},
    {hhrTunnelFailback,               ASN_INTEGER,   RONLY, hhrTunnelConfigTable_get, 2, {1, 18}},
    {hhrTunnelFailbackWtr,            ASN_INTEGER,   RONLY, hhrTunnelConfigTable_get, 2, {1, 19}}
};

/*hhrTunnelStatisticsTable*/
static oid hhrTunnelStatisticsTable_oid[] = {HHRMPLSCONFIG, 17};
FindVarMethod hhrTunnelStatisticsTable_get;
struct variable2 hhrTunnelStatisticsTable_variables[] =
{
    {hhrTSInputTotalBytes,    ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  2}},
    {hhrTSInputTotalPackets,  ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  3}},
    {hhrTSInputDropPackets,   ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  4}},
    {hhrTSOutputTotalBytes,   ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  5}},
    {hhrTSOutputTotalPackets, ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  6}},
    {hhrTSOutputDropPackets,  ASN_COUNTER64, RONLY, hhrTunnelStatisticsTable_get, 2, {1,  7}}
};

/*hhrPWStatisticsTable*/
static oid hhrPWStatisticsTable_oid[] = {HHRMPLSCONFIG, 18};
FindVarMethod hhrPWStatisticsTable_get;
struct variable2 hhrPWStatisticsTable_variables[] =
{
    {hhrPSInputTotalBytes,    ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  2}},
    {hhrPSInputTotalPackets,  ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  3}},
    {hhrPSInputDropPackets,   ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  4}},
    {hhrPSOutputTotalBytes,   ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  5}},
    {hhrPSOutputTotalPackets, ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  6}},
    {hhrPSOutputDropPackets,  ASN_COUNTER64, RONLY, hhrPWStatisticsTable_get, 2, {1,  7}}
};

/*hhrldpGlobal*/
static oid hhrldpGlobal_oid[] = {HHRMPLSCONFIG, 19};
FindVarMethod hhrldpGlobal_get;
struct variable1 hhrldpGlobal_variables[] =
{
    {hhrMplsLdpEnable,      ASN_INTEGER,  RONLY, hhrldpGlobal_get, 1, {1}},
    {hhrMplsLdpLabelPolicy, ASN_INTEGER,  RONLY, hhrldpGlobal_get, 1, {2}},
    {hhrMplsLdpAdvertise,   ASN_INTEGER,  RONLY, hhrldpGlobal_get, 1, {3}},
    {hhrMplsLdpKeepalive,   ASN_INTEGER,  RONLY, hhrldpGlobal_get, 1, {4}},
    {hhrMplsLdphello,       ASN_INTEGER,  RONLY, hhrldpGlobal_get, 1, {5}}
};

/*hhrMplsLdpLocalifTable*/
static oid hhrMplsLdpLocalifTable_oid[] = {HHRMPLSCONFIG, 19, 10};
FindVarMethod hhrMplsLdpLocalifTable_get;
static struct ipran_snmp_data_cache *hhrMplsLdpLocalifTable_cache = NULL;
struct variable2 hhrMplsLdpLocalifTable_variables[] =
{
    {hhrMplsLdpLocalIfDesrc,   ASN_OCTET_STR, RONLY, hhrMplsLdpLocalifTable_get, 2, {1, 2}},
    {hhrMplsLdpLocalLdpEnable, ASN_INTEGER,   RONLY, hhrMplsLdpLocalifTable_get, 2, {1, 3}}
};

/*hhrMplsLdpSessionTable*/
static oid hhrMplsLdpSessionTable_oid[] = {HHRMPLSCONFIG, 19, 11};
FindVarMethod hhrMplsLdpSessionTable_get;
static struct ipran_snmp_data_cache *hhrMplsLdpSessionTable_cahce = NULL;
struct variable2 hhrMplsLdpSessionTable_variables[] =
{
    {hhrMplsLdpSessionRole,       ASN_INTEGER,  RONLY, hhrMplsLdpSessionTable_get, 2, {1, 2}},
    {hhrMplsLdpSessionStatus,     ASN_INTEGER,  RONLY, hhrMplsLdpSessionTable_get, 2, {1, 3}},
    {hhrMplsLdpSessionType,       ASN_INTEGER,  RONLY, hhrMplsLdpSessionTable_get, 2, {1, 4}},
    {hhrMplsLdpSessionCreateTime, ASN_UNSIGNED, RONLY, hhrMplsLdpSessionTable_get, 2, {1, 5}}
};

/*hhrMplsLdpLspTable*/
static oid hhrMplsLdpLspTable_oid[] = {HHRMPLSCONFIG, 19, 12};
FindVarMethod hhrMplsLdpLspTable_get;
static struct ipran_snmp_data_cache *hhrMplsLdpLspTable_cache = NULL;
struct variable2 hhrMplsLdpLspTable_variables[] =
{
    {hhrMplsLdpLspDir,          ASN_INTEGER,   RONLY, hhrMplsLdpLspTable_get, 2, {1, 2}},
    {hhrMplsLdpLspFec,          ASN_IPADDRESS, RONLY, hhrMplsLdpLspTable_get, 2, {1, 3}},
    {hhrMplsLdpLspIngressLabel, ASN_UNSIGNED,  RONLY, hhrMplsLdpLspTable_get, 2, {1, 4}},
    {hhrMplsLdpLspEgressLabel,  ASN_UNSIGNED,  RONLY, hhrMplsLdpLspTable_get, 2, {1, 5}},
    {hhrMplsLdpLspNextHop,      ASN_IPADDRESS, RONLY, hhrMplsLdpLspTable_get, 2, {1, 6}}
};

/*hhrMplsLdpLspIPPrefixTable*/
static oid hhrMplsLdpLspIPPrefixTable_oid[] = {HHRMPLSCONFIG, 19, 13};
FindVarMethod hhrMplsLdpLspIPPrefixTable_get;
static struct ipran_snmp_data_cache *hhrMplsLdpLspIPPrefixTable_cache = NULL;
struct variable2 hhrMplsLdpLspIPPrefixTable_variables[] =
{
    {hhrMplsLdpIPPrefixReservedField, ASN_INTEGER, RONLY, hhrMplsLdpLspIPPrefixTable_get, 2, {1, 3}}
};

/*hhrMplsLdpRemotePeerTable*/
static oid hhrMplsLdpRemotePeerTable_oid[] = {HHRMPLSCONFIG, 19, 14};
FindVarMethod hhrMplsLdpRemotePeerTable_get;
static struct ipran_snmp_data_cache   *hhrMplsLdpRemotePeerTable_cache = NULL;
struct variable2 hhrMplsLdpRemotePeerTable_variables[] =
{
    {hhrMplsLdpRemotePeerReservedField, ASN_INTEGER, RONLY, hhrMplsLdpRemotePeerTable_get, 2, {1, 2}}
};

static oid hhrVplsStormSuppresTable_oid[] = {HHRMPLSCONFIG, 20};
FindVarMethod hhrVplsStormSuppresTable_get;
struct variable2 hhrVplsStormSuppresTable_variables[] =
{
    {hhrVplsUnicastEnable,   ASN_INTEGER,  RONLY, hhrVplsStormSuppresTable_get, 2, {1, 1}},
    {hhrVplsUnicastCir,      ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 2}},
    {hhrVplsUnicastCbs,      ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 3}},
    {hhrVplsBroadcastEnable, ASN_INTEGER,  RONLY, hhrVplsStormSuppresTable_get, 2, {1, 4}},
    {hhrVplsBroadcastCir,    ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 5}},
    {hhrVplsBroadcastCbs,    ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 6}},
    {hhrVplsMulticastEnable, ASN_INTEGER,  RONLY, hhrVplsStormSuppresTable_get, 2, {1, 7}},
    {hhrVplsMulticastCir,    ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 8}},
    {hhrVplsMulticastCbs,    ASN_UNSIGNED, RONLY, hhrVplsStormSuppresTable_get, 2, {1, 9}}
};

/*hhrMplsApsTable*/
static struct ipran_snmp_data_cache *hhrMplsApsTable_cache = NULL;
static oid hhrMplsApsTable_oid[] = {HHRMPLSCONFIG, 21};
FindVarMethod hhrMplsApsTable_get;
struct variable2 hhrMplsApsTable_variables[] =
{
    {hhrMplsApsKeepAlive,         ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  2}},
    {hhrMplsApsHoldOff,           ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  3}},
    {hhrMplsApsBackup,            ASN_INTEGER,   RONLY, hhrMplsApsTable_get, 2, {1,  4}},
    {hhrMplsApsBackupFailbackWtr, ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  5}},
    {hhrMplsApsPriority,          ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  6}},
    {hhrMplsApsMasterIndex,       ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  7}},
    {hhrMplsApsMasterName,        ASN_OCTET_STR, RONLY, hhrMplsApsTable_get, 2, {1,  8}},
    {hhrMplsApsBackupIndex,       ASN_UNSIGNED,  RONLY, hhrMplsApsTable_get, 2, {1,  9}},
    {hhrMplsApsBackupName,        ASN_OCTET_STR, RONLY, hhrMplsApsTable_get, 2, {1, 10}},
    {hhrMplsApsEnable,            ASN_INTEGER,   RONLY, hhrMplsApsTable_get, 2, {1, 11}},
    {hhrMplsApsActivePath,        ASN_INTEGER,   RONLY, hhrMplsApsTable_get, 2, {1, 12}},
    {hhrMplsApsCurrentState,      ASN_INTEGER,   RONLY, hhrMplsApsTable_get, 2, {1, 13}},
    {hhrMplsApsType,              ASN_INTEGER,   RONLY, hhrMplsApsTable_get, 2, {1, 14}}
};

/*hhrL3vpnInstanceTable*/
static oid hhrL3vpnInstanceTable_oid[] = {HHRMPLSCONFIG, 22, 1};
FindVarMethod hhrL3vpnInstanceTable_get;
static struct ipran_snmp_data_cache *hhrL3vpnInstanceTable_cache = NULL;

struct variable2 hhrL3vpnInstanceTable_variables[] =
{
    {hhrL3vpnInstanceName,           ASN_OCTET_STR, RONLY, hhrL3vpnInstanceTable_get, 2, {1, 2}},
    {hhrL3vpnApplyLabel,             ASN_INTEGER,   RONLY, hhrL3vpnInstanceTable_get, 2, {1, 3}},
    {hhrL3vpnRD,                     ASN_OCTET_STR, RONLY, hhrL3vpnInstanceTable_get, 2, {1, 4}}
};

/*hhrL3vpnTargetTable*/
static oid hhrL3vpnTargetTable_oid[] = {HHRMPLSCONFIG, 22, 3};
FindVarMethod hhrL3vpnTargetTable_get;
static struct ipran_snmp_data_cache *hhrL3vpnTargetTable_export_cache = NULL;
static struct ipran_snmp_data_cache *hhrL3vpnTargetTable_import_cache = NULL;

struct variable2 hhrL3vpnTargetTable_variables[] =
{
    {hhrL3vpnTargetType,             ASN_UNSIGNED, RONLY, hhrL3vpnTargetTable_get, 2, {1, 3}}
};


/*hhrMplsLabelTable*/
static struct ipran_snmp_data_cache *hhrMplsLabelTable_cache = NULL;

static oid hhrMplsLabelTable_oid[] = {HHRMPLSCONFIG, 30};
FindVarMethod hhrMplsLabelTable_get;
struct variable2 hhrMplsLabelTable_variables[] =
{
    {inLabel,                       ASN_INTEGER,  RONLY, hhrMplsLabelTable_get, 2, {1,  1}},
    {owner,                         ASN_INTEGER,  RONLY, hhrMplsLabelTable_get, 2, {1,  2}},
    {labelState,                    ASN_INTEGER,  RONLY, hhrMplsLabelTable_get, 2, {1,  3}}
};



/*hhrVplsMacBlackWhiteTable*/
static struct ipran_snmp_data_cache *hhrMplsVplsBlackMacTable_getTable_cache = NULL;
static oid hhrMplsVplsBlackMacTable_oid[] = {HHRMPLSCONFIG, 26};
FindVarMethod hhrMplsVplsBlackMacTable_get;
struct variable2 hhrMplsVplsBlackMacTable_variables[] =
{
    {hhrVplsMacBlackWhiteType,            ASN_UNSIGNED,  RONLY, hhrMplsVplsBlackMacTable_get, 2, {1,  1}},
    {hhrVplsMacBlackWhiteMacAddress,      ASN_OCTET_STR, RONLY, hhrMplsVplsBlackMacTable_get, 2, {1,  2}},
    {hhrVplsMacBlackWhiteReservedField,   ASN_UNSIGNED,  RONLY, hhrMplsVplsBlackMacTable_get, 2, {1,  3}}
};

/*hhrVplsEtreeTable*/
static struct ipran_snmp_data_cache *hhrMplsServiceTable_cache = NULL;
static oid hhrMplsServiceTable_oid[] = {HHRMPLSCONFIG, 28};
FindVarMethod hhrMplsServiceTable_get;
struct variable2 hhrMplsServiceTable_variables[] =
{
    {hhrVplsServiceType,            ASN_UNSIGNED,   RONLY, hhrMplsServiceTable_get, 2, {1,  2}},
    {hhrVplsEtreeServiceName,       ASN_OCTET_STR,  RONLY, hhrMplsServiceTable_get, 2, {1,  3}},
    {hhrVplsServiceVpnID,           ASN_UNSIGNED,   RONLY, hhrMplsServiceTable_get, 2, {1,  4}},
    {hhrVplsRootPortName,           ASN_OCTET_STR,  RONLY, hhrMplsServiceTable_get, 2, {1,  5}}
};

/*hhrVplsStaticMacTable*/
static struct ipran_snmp_data_cache *hhrVplsStaticMacTable_getTable_cache = NULL;
static oid hhrVplsStaticMacTable_oid[] = {HHRMPLSCONFIG, 24};
FindVarMethod hhrVplsStaticMacTable_get;
struct variable2 hhrVplsStaticMacTable_variables[] =
{
    {hhrVplsStaticMacAddress,            ASN_OCTET_STR,  RONLY, hhrVplsStaticMacTable_get, 2, {1,  1}},
    {hhrVplsStaticMacBindMode,           ASN_UNSIGNED,   RONLY, hhrVplsStaticMacTable_get, 2, {1,  2}},
    {hhrVplsStaticMacIfDescr,            ASN_OCTET_STR,  RONLY, hhrVplsStaticMacTable_get, 2, {1,  3}},
    {hhrVplsStaticMacPWId,               ASN_UNSIGNED,   RONLY, hhrVplsStaticMacTable_get, 2, {1,  4}}
};

/*hhrDynamicMacAddressTable*/
static oid hhrVplsDynamicMacTable_oid[] = {HHRMPLSCONFIG, 32};
FindVarMethod hhrVplsDynamicMacTable_get;
struct ipran_snmp_data_cache *hhrVplsDynamicMacTable_cache = NULL;
struct variable2 hhrVplsDynamicMacTable_variables[] =
{
    {hhrVplsDynamicMacAddress,            ASN_OCTET_STR,  RONLY, hhrVplsDynamicMacTable_get, 2, {1,  1}},
    {hhrVplsdynamicMacType,               ASN_UNSIGNED,   RONLY, hhrVplsDynamicMacTable_get, 2, {1,  2}},
    {hhrVplsdynamicMacAcIf,            ASN_OCTET_STR,  RONLY, hhrVplsDynamicMacTable_get, 2, {1,  3}},
    {hhrVplsdynamicMacPW,               ASN_OCTET_STR,  RONLY, hhrVplsDynamicMacTable_get, 2, {1,  4}}
};

void init_mib_mpls(void)
{
    REGISTER_MIB("hhrMplsconfigComm", hhrMplsconfigComm_variables,
                 variable1, hhrMplsconfigComm_oid);

    REGISTER_MIB("hhrLspConfigTable", hhrLspConfigTable_variables,
                 variable2, hhrLspConfigTable_oid);

    REGISTER_MIB("hhrVplsServiceTable", hhrVplsServiceTable_variables,
                 variable2, hhrVplsServiceTable_oid);

    REGISTER_MIB("hhrldpGlobal", hhrldpGlobal_variables, variable1, hhrldpGlobal_oid);

    REGISTER_MIB("hhrMplsLdpLocalifTable", hhrMplsLdpLocalifTable_variables,
                 variable2, hhrMplsLdpLocalifTable_oid);

    REGISTER_MIB("hhrMplsLdpSessionTable", hhrMplsLdpSessionTable_variables,
                 variable2, hhrMplsLdpSessionTable_oid);

    REGISTER_MIB("hhrMplsLdpLspTable",  hhrMplsLdpLspTable_variables,
                 variable2, hhrMplsLdpLspTable_oid);

    REGISTER_MIB("hhrMplsLdpLspIPPrefixTable", hhrMplsLdpLspIPPrefixTable_variables,
                 variable2, hhrMplsLdpLspIPPrefixTable_oid);

    REGISTER_MIB("hhrMplsLdpRemotePeerTable", hhrMplsLdpRemotePeerTable_variables,
                 variable2, hhrMplsLdpRemotePeerTable_oid);

    REGISTER_MIB("hhrVplsStormSuppresTable", hhrVplsStormSuppresTable_variables,
                 variable2, hhrVplsStormSuppresTable_oid);

    REGISTER_MIB("hhrPseudoWireTable", hhrPseudoWireTable_variables,
                 variable2, hhrPseudoWireTable_oid);

    REGISTER_MIB("hhrUNIOfVCTable", hhrUNIOfVCTable_variables,
                 variable2, hhrUNIOfVCTable_oid);

    REGISTER_MIB("hhrUNIOfVPLSTable", hhrUNIOfVPLSTable_variables,
                 variable2, hhrUNIOfVPLSTable_oid);

    REGISTER_MIB("hhrTunnelConfigTable", hhrTunnelConfigTable_variables,
                 variable2, hhrTunnelConfigTable_oid);

    REGISTER_MIB("hhrTunnelStatisticsTable", hhrTunnelStatisticsTable_variables,
                 variable2, hhrTunnelStatisticsTable_oid);

    REGISTER_MIB("hhrPWStatisticsTable", hhrPWStatisticsTable_variables,
                 variable2, hhrPWStatisticsTable_oid);

    REGISTER_MIB("hhrMplsApsTable", hhrMplsApsTable_variables,
                 variable2, hhrMplsApsTable_oid);

    REGISTER_MIB("hhrVplsMacBlackWhiteTable", hhrMplsVplsBlackMacTable_variables,
                 variable2, hhrMplsVplsBlackMacTable_oid);

    REGISTER_MIB("hhrVplsEtreeTable", hhrMplsServiceTable_variables,
                 variable2, hhrMplsServiceTable_oid);

    REGISTER_MIB("hhrL3vpnInstanceTable", hhrL3vpnInstanceTable_variables,
                 variable2, hhrL3vpnInstanceTable_oid);

    REGISTER_MIB("hhrL3vpnTargetTable", hhrL3vpnTargetTable_variables,
                 variable2, hhrL3vpnTargetTable_oid);

    REGISTER_MIB("hhrMplsLabelTable", hhrMplsLabelTable_variables,
                 variable2, hhrMplsLabelTable_oid);

    REGISTER_MIB("hhrVplsStaticMacTable", hhrVplsStaticMacTable_variables,
                 variable2, hhrVplsStaticMacTable_oid);

    REGISTER_MIB("hhrVplsDynamicMacTable", hhrVplsDynamicMacTable_variables,
                 variable2, hhrVplsDynamicMacTable_oid);

}

/*** hhrMplsconfigComm start **************************************************/
u_char *hhrMplsconfigComm_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t    *pdata;

    if (header_generic(vp, name, length, exact, var_len, write_method) == MATCH_FAILED)
    {
        return NULL;
    }

    switch (vp->magic)
    {
        case hhrMplsEnable:
            *var_len = sizeof(int);
            int_value = SNMP_MPLS_ENABLE;
            return (u_char *)&int_value;

        case hhrMplsLsrID:
            *var_len = sizeof(uint32_t);
            pdata    = (mpls_com_get_lsr_id(MODULE_ID_SNMPD));
            ip_value = 0;

            if (NULL != pdata)
            {
                ip_value = *pdata;
                ip_value = htonl(ip_value);
            }

            return (u_char *)(&ip_value);

        default:
            return NULL;
    }

    return NULL;
}
/*** hhrMplsconfigComm end ****************************************************/

/*** hhrLspConfigTable start **************************************************/
int hhrLspConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                        struct static_lsp *index_input)
{
    struct static_lsp *pdata = NULL;
    struct static_lsp *pfree = NULL;
    int data_len = sizeof(struct static_lsp);
    int data_num = 0;
    int i;

    pdata = mpls_com_get_slsp_bulk(index_input->name, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct static_lsp *hhrLspConfigTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                 int exact,
                                                 const struct static_lsp *index_input)
{
    struct listnode    *node;
    struct static_lsp  *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->name[0]))
        {
            return cache->data_list->head->data;
        }

        if (strcmp(pdata->name, index_input->name) == 0)
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

u_char *hhrLspConfigTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct static_lsp  *pdata = NULL;
    struct static_lsp   data_index;
    int                 data_len = sizeof(struct static_lsp);
    u_char              sIndex[MPLS_MAX_NAME_LEGTH] = {0};
    u_char              sNext_index[MPLS_MAX_NAME_LEGTH] = {0};
    int                 key_len;
    int                 ret = 0;

    *write_method    = NULL;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, MPLS_MAX_NAME_LEGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        return NULL;
    }

    if (NULL == hhrLspConfigTable_cache)
    {
        hhrLspConfigTable_cache = snmp_cache_init(data_len,
                                                  hhrLspConfigTable_get_data_from_ipc,
                                                  hhrLspConfigTable_node_lookup);

        if (NULL == hhrLspConfigTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    key_len = MPLS_MAX_NAME_LEGTH;
    memset(&data_index, 0, data_len);
    memcpy(data_index.name, sIndex, key_len);

    pdata = snmp_cache_get_data_by_index(hhrLspConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        snprintf(sNext_index, MPLS_MAX_NAME_LEGTH, "%s", pdata->name);
        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index)) ;
    }

    switch (vp->magic)
    {
        case hhrLspNodeType:
            *var_len = sizeof(int);
            int_value = pdata->direction;

            if (0 == int_value)
            {
                int_value = 2;
            }
            else if (1 == int_value)
            {
                int_value = 1;
            }
            else
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrLspDestIP:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->destip.addr.ipv4;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrLspDestMask:
            *var_len = sizeof(uint32_t);

            if (pdata->destip.prefixlen >= 32)
            {
                ip_value = 0xFFFFFFFF;
            }
            else
            {
                ip_value = 0xFFFFFFFF << (32 - pdata->destip.prefixlen);
            }

            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrLspOutIF:
            memset(str_value, 0, MPLS_STRING_LEN);
            ifm_get_name_by_ifindex(pdata->nhp_index, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrLspDestMac:
            *var_len = 6;
            memcpy(&mac_value, pdata->dmac, 6);
            return (mac_value);

        case hhrLspNexthop:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->nexthop.addr.ipv4;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrLspInLabel:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->inlabel;
            return (u_char *)(&uint_value);

        case hhrLspOutLabel:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->outlabel;
            return (u_char *)(&uint_value);

        case hhrLspTpOamSessionId:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->mplsoam_id;
            return (u_char *)(&uint_value);

        case hhrLspIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->lsp_index;
            return (u_char *)(&uint_value);

        case hhrLspLinkStatus:
            *var_len = sizeof(int);

            if (1 == pdata->status)
            {
                int_value = 1;
            }
            else if (0 == pdata->status)
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrLspAdminStatus:
            *var_len = sizeof(int);

            if (1 == pdata->admin_down)
            {
                int_value = 2;
            }
            else if (0 == pdata->admin_down)
            {
                int_value = 1;
            }

            return (u_char *)(&int_value);

        default:
            return (NULL);
    }
}
/*** hhrLspConfigTable end ****************************************************/

/*** hhrPseudoWireTable start *************************************************/
int hhrPseudoWireTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         struct l2vc_entry *index_input)
{
    struct l2vc_entry *pdata = NULL;
    struct l2vc_entry *pfree = NULL;
    int data_len = sizeof(struct l2vc_entry);
    int data_num = 0;
    int i;

    pdata = mpls_com_get_l2vc_bulk(index_input->name, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct l2vc_entry *hhrPseudoWireTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact,
                                                  const struct l2vc_entry *index_input)
{
    struct listnode    *node;
    struct l2vc_entry  *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->name[0]))
        {
            return cache->data_list->head->data;
        }

        if (strcmp(pdata->name, index_input->name) == 0)
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


u_char *hhrPseudoWireTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2vc_entry  *pdata = NULL;
    struct l2vc_entry   data_index;
    int                 data_len = sizeof(struct l2vc_entry);
    u_char              sIndex[MPLS_MAX_NAME_LEGTH] = {0};
    u_char              sNext_index[MPLS_MAX_NAME_LEGTH] = {0};
    int                 key_len;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, MPLS_MAX_NAME_LEGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        return NULL;
    }

    if (NULL == hhrPseudoWireTable_cache)
    {
        hhrPseudoWireTable_cache = snmp_cache_init(data_len,
                                                   hhrPseudoWireTable_get_data_from_ipc,
                                                   hhrPseudoWireTable_node_lookup);

        if (NULL == hhrPseudoWireTable_cache)
        {
            return NULL;
        }
    }

    key_len = MPLS_MAX_NAME_LEGTH;
    memset(&data_index, 0, data_len);
    memcpy(data_index.name, sIndex, key_len);

    pdata = snmp_cache_get_data_by_index(hhrPseudoWireTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        snprintf(sNext_index, MPLS_MAX_NAME_LEGTH, "%s", pdata->name);
        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index)) ;
    }

    switch (vp->magic)
    {
        case hhrVCid:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->pwinfo.vcid;
            return (u_char *)(&uint_value);

        case hhrPWProtocolType:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.protocol;
            return (u_char *)(&int_value);

        case hhrPWVCType:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.ac_type;
            return (u_char *)(&int_value);

        case hhrPWDestination:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->peerip.addr.ipv4;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrPWInLabel:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->inlabel;
            return (u_char *)(&uint_value);

        case hhrPWOutLabel:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->outlabel;
            return (u_char *)(&uint_value);

        case hhrPWRelatedTunnelName:
            memset(str_value, 0, MPLS_STRING_LEN);
            ifm_get_name_by_ifindex(pdata->tunl_index, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrPWControlWord:
            *var_len = sizeof(int);
            int_value = (pdata->pwinfo.ctrlword_flag == 0) ? 2 : 1;   // 1:enable 2:disable
            return (u_char *)(&int_value);

        case hhrPWEncapsulationType:
            *var_len = sizeof(int);

            //int_value = (pdata->pwinfo.tag_flag == 0) ? 1 : 2; // 1:raw 2:tag
            //0: ethernet RAW, 1: ethernet TAG, 2: vlan RAW, 3: vlan TAG
            //mib :
            switch (pdata->pwinfo.tag_flag)
            {
                case 0 :
                    int_value = 1 ;
                    return (u_char *)(&int_value);

                case 1 : //
                    int_value = 2 ;
                    return (u_char *)(&int_value);

                case 2 : //
                    int_value = 3 ;
                    return (u_char *)(&int_value);

                case 3 : //
                    int_value = 4 ;
                    return (u_char *)(&int_value);

                default:
                    int_value = 1 ;
                    return (u_char *)(&int_value);
            }

        case hhrPWCESFrameNum:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.ces.frame_num;
            return (u_char *)(&int_value);

        case hhrPWTPID:
            memset(str_value, 0, MPLS_STRING_LEN);
            sprintf(str_value, "0x%04x\n" , pdata->pwinfo.tpid);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrPWVlan:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.vlan;
            return (u_char *)(&int_value);

        case hhrPWmtu:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.mtu;
            return (u_char *)(&int_value);

        case hhrRelatedVplsid:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.vpls.vsi_id;
            return (u_char *)(&int_value);

        case hhrPWInVplsNetLocation:
            *var_len = sizeof(int);
            int_value = (pdata->pwinfo.vpls.upe == 0) ? 2 : 1; // 1:upe 2:spe
            return (u_char *)(&int_value);

        case hhrPWIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->pwinfo.pwindex;
            return (u_char *)(&uint_value);

        case hhrPWSwitchPWIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->pwinfo.mspw_index;
            return (u_char *)(&uint_value);

        case hhrPWEnableMip:            // EEEEEEEEEEEEEEEEEEEEEEEEEEEE
            *var_len = sizeof(int);
            int_value = 0;

            if (pdata->mplsoam_id == 8192)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrPWTpOamSessionId:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->mplsoam_id;
            return (u_char *)(&uint_value);

        case hhrPWStatisticsEnable:
            *var_len = sizeof(int);
            int_value = (1 == pdata->statis_enable) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrPWTunnelPolicyLdp:
            *var_len = sizeof(int);
            int_value = (0 != pdata->ldp_tunnel) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrPWExpDomain:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->domain_id ;
            return (u_char *)(&uint_value);

        case hhrPWPhd:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->phb_id ;
            return (u_char *)(&uint_value);

        case hhrPWTunnelPolicymplstp :
            *var_len = sizeof(int);
            int_value = (0 != pdata->mplstp_tunnel) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrPWTunnelPolicyGre :
            *var_len = sizeof(int);
            int_value = (0 != pdata->gre_tunnel) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrPWLinkStatus:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.up_flag ? 2 : 1;
            return (u_char *)(&int_value);

        case hhrPWAdminStatus:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.admin_up ? 2 : 1;
            return (u_char *)(&int_value);

        default:
            return (NULL);
    }
}
/*** hhrPseudoWireTable end ***************************************************/

/*** hhrUNIOfVCTable start ****************************************************/
u_char *hhrUNIOfVCTable_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2vc_entry  *pdata = NULL;
    struct l2vc_entry   data_index;
    int                 data_len = sizeof(struct l2vc_entry);
    u_char              sIndex[MPLS_MAX_NAME_LEGTH] = {0};
    u_char              sNext_index[MPLS_MAX_NAME_LEGTH] = {0};
    int                 key_len;
    int                 ret = 0;

    *write_method = NULL;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, MPLS_MAX_NAME_LEGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        return NULL;
    }

    if (NULL == hhrPseudoWireTable_cache)
    {
        hhrPseudoWireTable_cache = snmp_cache_init(data_len,
                                                   hhrPseudoWireTable_get_data_from_ipc,
                                                   hhrPseudoWireTable_node_lookup);

        if (NULL == hhrPseudoWireTable_cache)
        {
            return NULL;
        }
    }

    key_len = MPLS_MAX_NAME_LEGTH;
    memset(&data_index, 0, data_len);
    memcpy(data_index.name, sIndex, key_len);

    pdata = snmp_cache_get_data_by_index(hhrPseudoWireTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        snprintf(sNext_index, MPLS_MAX_NAME_LEGTH, "%s", pdata->name);
        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index)) ;
    }

    switch (vp->magic)
    {
        case hhrUNIOfCESIfDESC:
            memset(str_value, 0, MPLS_STRING_LEN);
            ifm_get_name_by_ifindex(pdata->pwinfo.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrUNIRelatedVCId:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->pwinfo.vcid;
            return (u_char *)(&uint_value);

        case hhrPWProtectionRole:
            *var_len = sizeof(int);
            int_value = pdata->pwinfo.pw_type;
            return (u_char *)(&int_value);

        case hhrPWProtectionFailback:
            *var_len = sizeof(uint32_t);
            int_value = pdata->failback;
            return (u_char *)(&int_value);

        case hhrPWProtectionFailbackWtr:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->wtr;
            return (u_char *)(&uint_value);

        case hhrPWProtectionWorkStatus:
            *var_len = sizeof(int);
            int_value = pdata->pw_backup;
            return (u_char *)(&int_value);

        default:
            return (NULL);
    }
}
/*** hhrUNIOfVCTable end ******************************************************/

/*** hhrVplsServiceTable start ************************************************/
int hhrVplsServiceTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          struct vsi_entry *index_input)
{
    struct vsi_entry *pdata = NULL;
    struct vsi_entry *pfree = NULL;
    int data_len = sizeof(struct vsi_entry);
    int data_num = 0;
    int i;

    pdata = mpls_com_get_vsi_bulk(index_input->vpls.vsi_id, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num )
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct vsi_entry *hhrVplsServiceTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact,
                                                  const struct vsi_entry *index_input)
{
    struct listnode    *node;
    struct vsi_entry   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->vpls.vsi_id))
        {
            return cache->data_list->head->data;
        }

        if (pdata->vpls.vsi_id == index_input->vpls.vsi_id)
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

u_char *hhrVplsServiceTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vsi_entry   *pdata = NULL;
    struct vsi_entry    data_index;
    int                 data_len = sizeof(struct vsi_entry);
    int                 ret = 0;
    int                 index_int = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return (NULL);
    }

    if (NULL == hhrVplsServiceTable_cache)
    {
        hhrVplsServiceTable_cache = snmp_cache_init(data_len,
                                                    hhrVplsServiceTable_get_data_from_ipc,
                                                    hhrVplsServiceTable_node_lookup);

        if (NULL == hhrVplsServiceTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.vpls.vsi_id = index_int;

    pdata = snmp_cache_get_data_by_index(hhrVplsServiceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->vpls.vsi_id);
    }

    switch (vp->magic)
    {
        case hhrVplsId:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.vsi_id;
            return (u_char *)(&uint_value);

        case hhrVplsName:
            memset(str_value, 0, MPLS_MAX_NAME_LEGTH);
            memcpy(str_value, pdata->name, strlen(pdata->name));
            *var_len = strlen(pdata->name);
            return (u_char *)(str_value);

        case hhrVplsMacLearnEnable:
            *var_len = sizeof(int);
            int_value = (pdata->vpls.mac_learn == 0) ? 2 : 1; // 2:enable, 1:disable
            return (u_char *)(&int_value);

        case hhrVplsMacLearnLimit:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.learning_limit;
            return (u_char *)(&uint_value);

        case hhrVplsEncapsulateType:
            *var_len = sizeof(int);
            int_value = pdata->vpls.tag_flag;

            if (int_value == 0)
            {
                int_value = 1;
            }
            else if (int_value == 1)
            {
                int_value = 1;
            }
            else if (int_value == 2)
            {
                int_value = 2;
            }
            else if (int_value == 3)
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrVplsEncapsulateTag:
            *var_len = sizeof(int);
            int_value = pdata->vpls.tag_flag;

            if (int_value == 0)
            {
                int_value = 1 ;
            }
            else if (int_value == 1)
            {
                int_value = 2 ;
            }
            else if (int_value == 2)
            {
                int_value = 1 ;
            }
            else if (int_value == 3)
            {
                int_value = 2 ;
            }

            return (u_char *)(&int_value);

        case hhrVplsEncapsulateTpId:
            *var_len = sizeof(int);
            int_value = 0;

            if (0x8100 == pdata->vpls.tpid)
            {
                int_value = 1;
            }
            else if (0x88a8 == pdata->vpls.tpid)
            {
                int_value = 2;
            }
            else if (0x9100 == pdata->vpls.tpid)
            {
                int_value = 3;
            }

            return (u_char *)(&int_value);

        case hhrVplsEncapsulateVlan:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.vlan;
            return (u_char *)(&uint_value);

        default :
            return (NULL);
    }
}

u_char *hhrldpGlobal_get(struct variable *vp,
                         oid *name,
                         size_t *length,
                         int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ldp_config_global_mib *pdata;
    struct ldp_config_global_mib *pfree = NULL;
    int data_num = 0;

    if (header_generic(vp, name, length, exact, var_len, write_method) ==
            MATCH_FAILED)
    {
        return NULL;
    }

    pdata = mpls_com_get_ldp_global_conf(0, MODULE_ID_SNMPD, &data_num);

    if (NULL == pdata)
    {
        return NULL;
    }
    pfree = pdata;
    switch (vp->magic)
    {
        case hhrMplsLdpEnable:

            if (pdata->gldp_enable)
            {
                int_value = 1;
            }
            else
            {
                int_value = 2;
            }

            *var_len = sizeof(int);
            mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
            return (u_char *)(&int_value);

        case hhrMplsLdpLabelPolicy:
            int_value = pdata->policy;
            *var_len = sizeof(uint32_t);
            mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
            return (u_char *)(&int_value);

        case hhrMplsLdpAdvertise:
            uint_value = pdata->advertise;
            *var_len = sizeof(uint32_t);
            mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
            return (u_char *)(&uint_value);

        case hhrMplsLdpKeepalive:
            uint_value = pdata->keepalive_internal;
            *var_len = sizeof(uint32_t);
            mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
            return (u_char *)(&uint_value);

        case hhrMplsLdphello:
            uint_value = pdata->hello_internal;
            *var_len = sizeof(uint32_t);
            mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

/*hhrMplsLdpLocalifTable*/
int hhrMplsLdpLocalifTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct ldp_localif_mib *index)
{
    struct ldp_localif_mib *pdata = NULL;
    struct ldp_localif_mib *pfree = NULL;

    int data_num = 0;
    int i;

    pdata = mpls_com_get_ldp_localif_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct ldp_localif_mib));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ldp_localif_mib *hhrMplsLdpLocalifTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                           int exact,
                                                           const struct ldp_localif_mib  *index_input)
{
    struct ldp_localif_mib *pdata;
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

u_char *hhrMplsLdpLocalifTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    uint32_t if_index = 0;
    struct ldp_localif_mib *pdata = NULL;
    struct ldp_localif_mib data_index;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &if_index,  exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrMplsLdpLocalifTable_cache == NULL)
    {
        hhrMplsLdpLocalifTable_cache = snmp_cache_init(sizeof(struct ldp_localif_mib),
                                                       hhrMplsLdpLocalifTable_data_form_ipc,
                                                       hhrMplsLdpLocalifTable_node_lookup);

        if (hhrMplsLdpLocalifTable_cache == NULL)
        {
            return (NULL);
        }
    }

    data_index.ifindex = if_index;
    pdata = snmp_cache_get_data_by_index(hhrMplsLdpLocalifTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->ifindex);
    }

    switch (vp->magic)
    {
        case hhrMplsLdpLocalIfDesrc:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ifm_get_name_by_ifindex(pdata->ifindex, str_value);
            *var_len = strlen((char *)str_value);
            return (str_value);

        case hhrMplsLdpLocalLdpEnable:
            int_value = (pdata->ldp_enable) ? 1 : 2;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}

/*hhrMplsLdpSessionTable*/
int hhrMplsLdpSessionTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct ldp_session_mib *index)
{
    struct ldp_session_mib *pdata = NULL;
    struct ldp_session_mib *pfree = NULL;

    int data_num = 0;
    int i;

    pdata = mpls_com_get_ldp_sess_bulk(index->peer_lsrid, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct ldp_session_mib));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}
struct ldp_session_mib *hhrMplsLdpSessionTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                           int exact,
                                                           const struct ldp_session_mib  *index_input)
{
    struct listnode    *node;
    struct ldp_session_mib *pdata = NULL;

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

        if (0 == index_input->peer_lsrid)
        {
            return cache->data_list->head->data;
        }

        if (pdata->peer_lsrid == index_input->peer_lsrid)
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


u_char *hhrMplsLdpSessionTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ldp_session_mib *pdata = NULL;
    struct ldp_session_mib data_index;
    uint32_t index = 0;
    int ret = 0;
    ret = ipran_snmp_ip_index_get(vp, name, length, &index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrMplsLdpSessionTable_cahce == NULL)
    {
        hhrMplsLdpSessionTable_cahce = snmp_cache_init(sizeof(struct ldp_session_mib),
                                                       hhrMplsLdpSessionTable_data_form_ipc,
                                                       hhrMplsLdpSessionTable_node_lookup);

        if (hhrMplsLdpSessionTable_cahce == NULL)
        {
            return (NULL);
        }
    }

    data_index.peer_lsrid = ntohl(index);
    pdata = snmp_cache_get_data_by_index(hhrMplsLdpSessionTable_cahce, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_ip_index_set(vp, name, length, htonl(pdata->peer_lsrid));
    }

    switch (vp->magic)
    {
        case hhrMplsLdpSessionRole:
            int_value = pdata->role;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrMplsLdpSessionStatus:
            int_value = pdata->status;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrMplsLdpSessionType:
            int_value = pdata->type;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrMplsLdpSessionCreateTime:
            uint_value = pdata->creat_time;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

/*hhrMplsLdpLspTable*/
int hhrMplsLdpLspTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct ldp_lsp_mib *index)
{
    struct ldp_lsp_mib *pdata = NULL;
    struct ldp_lsp_mib *pfree = NULL;
    int data_num = 0;
    int i;

    pdata = mpls_com_get_ldp_lsp_bulk(index->lspindex, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct ldp_lsp_mib));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ldp_lsp_mib *hhrMplsLdpLspTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                   int exact,
                                                   const struct ldp_lsp_mib  *index_input)
{
    struct listnode    *node;
    struct ldp_lsp_mib *pdata = NULL;

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

        if (0 == index_input->lspindex)
        {
            return cache->data_list->head->data;
        }

        if (pdata->lspindex == index_input->lspindex)
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


u_char *hhrMplsLdpLspTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ldp_lsp_mib *pdata;
    struct ldp_lsp_mib data_index;
    int ret = 0;
    int data_num = 0;
    uint32_t if_index = 0;

    ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrMplsLdpLspTable_cache == NULL)
    {
        hhrMplsLdpLspTable_cache = snmp_cache_init(sizeof(struct ldp_lsp_mib),
                                                   hhrMplsLdpLspTable_data_form_ipc,
                                                   hhrMplsLdpLspTable_node_lookup);

        if (hhrMplsLdpLspTable_cache == NULL)
        {
            return (NULL);
        }
    }

    data_index.lspindex = if_index;
    pdata = snmp_cache_get_data_by_index(hhrMplsLdpLspTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->lspindex);
    }

    switch (vp->magic)
    {
        case hhrMplsLdpLspDir:
            int_value = pdata->direction;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrMplsLdpLspFec:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(pdata->destip.addr.ipv4);
            return (u_char *)(&ip_value);

        case hhrMplsLdpLspIngressLabel:
            uint_value = pdata->inlabel;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case hhrMplsLdpLspEgressLabel:
            uint_value = pdata->outlabel;
            *var_len = sizeof(uint32_t);
            return (u_char *)(&uint_value);

        case hhrMplsLdpLspNextHop:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(pdata->nexthop.addr.ipv4);
            return (u_char *)(&ip_value);

        default:
            return NULL;
    }

    return NULL;
}

/*hhrMplsLdpLspIPPrefixTable*/
int hhrMplsLdpLspIPPrefixTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct ldp_ipprefix_mib *index)
{
    struct ldp_ipprefix_mib *pdata = NULL;
    struct ldp_ipprefix_mib *pfree = NULL;
    int data_num = 0;
    int i;

    pdata = mpls_com_get_ldp_prefix_bulk(index->prefix.addr.ipv4, index->prefix.prefixlen, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct ldp_ipprefix_mib));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ldp_ipprefix_mib *hhrMplsLdpLspIPPrefixTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,
                                                                const struct ldp_ipprefix_mib *index_input)
{
    struct listnode    *node;
    struct ldp_ipprefix_mib *pdata = NULL;

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

        if (0 == index_input->prefix.addr.ipv4 && 0 == index_input->prefix.prefixlen)
        {
            return cache->data_list->head->data;
        }

        if ((pdata->prefix.addr.ipv4) == index_input->prefix.addr.ipv4 && pdata->prefix.prefixlen == index_input->prefix.prefixlen)
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

u_char *hhrMplsLdpLspIPPrefixTable_get(struct variable *vp,
                                       oid *name,
                                       size_t *length,
                                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ldp_ipprefix_mib *pdata = NULL;
    struct ldp_ipprefix_mib data_index;
    uint32_t ip_index = 0;
    uint32_t prelen_index = 0;
    int ret = 0;

    ret = ipran_snmp_ip_int_index_get(vp, name, length, &ip_index, &prelen_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrMplsLdpLspIPPrefixTable_cache == NULL)
    {
        hhrMplsLdpLspIPPrefixTable_cache = snmp_cache_init(sizeof(struct ldp_ipprefix_mib),
                                                           hhrMplsLdpLspIPPrefixTable_data_form_ipc,
                                                           hhrMplsLdpLspIPPrefixTable_data_lookup);

        if (NULL == hhrMplsLdpLspIPPrefixTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct ldp_ipprefix_mib));
    data_index.prefix.addr.ipv4 = ntohl(ip_index);
    data_index.prefix.prefixlen = prelen_index;
    pdata = snmp_cache_get_data_by_index(hhrMplsLdpLspIPPrefixTable_cache, exact, &data_index);

    if (NULL  == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_ip_int_index_set(vp, name, length, htonl(pdata->prefix.addr.ipv4), pdata->prefix.prefixlen);
    }

    switch (vp->magic)
    {
        case hhrMplsLdpIPPrefixReservedField:
            *var_len = sizeof(uint32_t);
            int_value = 0;
            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}

/*hhrMplsLdpRemotePeerTable*/
int hhrMplsLdpRemotePeerTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct ldp_remoteconf_mib *index)
{
    struct ldp_remoteconf_mib *pdata = NULL;
    struct ldp_remoteconf_mib *pfree = NULL;
    int data_num = 0;
    int i;
    uint32_t remote_peer_index = index->remote_peer;

    pdata = mpls_com_get_ldp_remotconf_bulk(remote_peer_index, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct ldp_remoteconf_mib));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct ldp_remoteconf_mib *hhrMplsLdpRemotePeerTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                                 int exact,
                                                                 const struct ldp_remoteconf_mib  *index_input)
{
    struct listnode    *node;
    struct ldp_remoteconf_mib  *pdata = NULL;

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

        if (0 == index_input->remote_peer)
        {
            return cache->data_list->head->data;
        }

        if (pdata->remote_peer == index_input->remote_peer)
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

u_char *hhrMplsLdpRemotePeerTable_get(struct variable *vp,
                                      oid *name,
                                      size_t *length,
                                      int exact, size_t *var_len, WriteMethod **write_method)
{
    struct ldp_remoteconf_mib *pdata = NULL;
    struct ldp_remoteconf_mib  data_index;
    uint32_t ip_index;
    int ret = 0;
    ret = ipran_snmp_ip_index_get(vp, name, length, &ip_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrMplsLdpRemotePeerTable_cache == NULL)
    {
        hhrMplsLdpRemotePeerTable_cache = snmp_cache_init(sizeof(struct ldp_remoteconf_mib),
                                                          hhrMplsLdpRemotePeerTable_data_form_ipc,
                                                          hhrMplsLdpRemotePeerTable_data_lookup);

        if (NULL == hhrMplsLdpRemotePeerTable_cache)
        {
            return (NULL);
        }
    }

    data_index.remote_peer = ntohl(ip_index);
    pdata = snmp_cache_get_data_by_index(hhrMplsLdpRemotePeerTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_ip_index_set(vp, name, length, htonl(pdata->remote_peer));
    }

    switch (vp->magic)
    {
        case hhrMplsLdpRemotePeerReservedField:
            *var_len = sizeof(uint32_t);
            int_value = 0;
            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}

/*** hhrVplsServiceTable end **************************************************/

/*** hhrVplsStormSuppresTable start *******************************************/
u_char *hhrVplsStormSuppresTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vsi_entry   *pdata = NULL;
    struct vsi_entry    data_index;
    int                 data_len = sizeof(struct vsi_entry);
    int                 ret = 0;
    int                 index_int = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return (NULL);
    }

    if (NULL == hhrVplsServiceTable_cache)
    {
        hhrVplsServiceTable_cache = snmp_cache_init(data_len,
                                                    hhrVplsServiceTable_get_data_from_ipc,
                                                    hhrVplsServiceTable_node_lookup);

        if (NULL == hhrVplsServiceTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.vpls.vsi_id = index_int;

    pdata = snmp_cache_get_data_by_index(hhrVplsServiceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->vpls.vsi_id);
    }

    switch (vp->magic)
    {
        case hhrVplsUnicastEnable:
            *var_len = sizeof(int);
            int_value = (pdata->vpls.uc_cbs == 0) ? 2 : 1; // 2:enable, 1:disable
            return (u_char *)(&int_value);

        case hhrVplsUnicastCir:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.uc_cir;
            return (u_char *)(&uint_value);

        case hhrVplsUnicastCbs:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.uc_cbs;
            return (u_char *)(&uint_value);

        case hhrVplsBroadcastEnable:
            *var_len = sizeof(int);
            int_value = (pdata->vpls.bc_cbs == 0) ? 2 : 1; // 2:enable, 1:disable
            return (u_char *)(&int_value);

        case hhrVplsBroadcastCir:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.bc_cir;
            return (u_char *)(&uint_value);

        case hhrVplsBroadcastCbs:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.bc_cbs;
            return (u_char *)(&uint_value);

        case hhrVplsMulticastEnable:
            *var_len = sizeof(int);
            int_value = (pdata->vpls.mc_cbs == 0) ? 2 : 1; // 2:enable, 1:disable
            return (u_char *)(&int_value);

        case hhrVplsMulticastCir:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.mc_cir;
            return (u_char *)(&uint_value);

        case hhrVplsMulticastCbs:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->vpls.mc_cbs;
            return (u_char *)(&uint_value);

        default :
            return NULL;
    }

    return NULL;
}
/*** hhrVplsStormSuppresTable end *********************************************/

/*** hhrUNIOfVPLSTable start **************************************************/

int hhrUNIOfVPLSTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                        struct vsi_if_snmp *index_input)
{
    char buf[IPC_MSG_LEN];
    struct vsi_entry       *pvsi_data = NULL;
    struct vsi_entry        *pfree = NULL;
    int                     vsi_data_num = 0;
    uint32_t               *pif = NULL;
    uint32_t                *pfree1 = NULL;
    int                     if_data_num = 0;
    struct vsi_if_snmp      vsi_if_data;
    int                     vsi_if_data_len = sizeof(struct vsi_if_snmp);
    int                     i, j;
    int                     ret = 0;
    uint32_t                vsi_index = index_input->vsi_id;
    uint32_t                if_index  = index_input->vsi_if_id;

    while (1)
    {
        pvsi_data = mpls_com_get_vsi_bulk(vsi_index, MODULE_ID_SNMPD, &vsi_data_num);

        if (0 == vsi_data_num || NULL == pvsi_data)
        {
            return FALSE;
        }
        memset(buf, 0 , IPC_MSG_LEN);
        memcpy(buf, pvsi_data, IPC_MSG_LEN);
        pvsi_data = (struct vsi_entry *)buf;
        pfree = pvsi_data;
		
        for (i = 0; i < vsi_data_num; i++)
        {
            vsi_index = pvsi_data->vpls.vsi_id;
            while (1)
            {
                pif = mpls_com_get_vsiif_bulk(vsi_index, if_index, MODULE_ID_SNMPD, &if_data_num);

                if (0 == if_data_num || NULL == pif)
                {
                    if_index = 0;
                    break;
                }
                pfree1 = pif;
                for (j = 0; j < if_data_num; j++)
                {
                    ret = 1;
                    if_index = pif[j];
                    memset(&vsi_if_data, 0, vsi_if_data_len);
                    vsi_if_data.vsi_id    = vsi_index;
                    vsi_if_data.vsi_if_id = pif[j];
                    snmp_cache_add(cache, &vsi_if_data, vsi_if_data_len);
                }
                mem_share_free_bydata(pfree1, MODULE_ID_SNMPD);
            }

            pvsi_data++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        if (1 == ret)
        {
            return TRUE;
        }
    }

    return FALSE;
}

struct vsi_if_snmp *hhrUNIOfVPLSTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact,
                                                  const struct vsi_if_snmp *index_input)
{
    struct listnode    *node;
    struct vsi_if_snmp *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->vsi_id) || (0 == index_input->vsi_if_id))
        {
            return cache->data_list->head->data;
        }

        if (pdata->vsi_id == index_input->vsi_id && pdata->vsi_if_id == index_input->vsi_if_id)
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

u_char *hhrUNIOfVPLSTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vsi_if_snmp *pdata = NULL;
    struct vsi_if_snmp  data_index;
    int                 data_len = sizeof(struct vsi_if_snmp);
    int                 ret = 0;
    int                 index_vsi = 0;
    int                 index_vsi_if = 0;

    *write_method = NULL;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &index_vsi, &index_vsi_if, exact);

    if (ret < 0)
    {
        return (NULL);
    }

    if (NULL == hhrUNIOfVPLSTable_cache)
    {
        hhrUNIOfVPLSTable_cache = snmp_cache_init(data_len,
                                                  hhrUNIOfVPLSTable_get_data_from_ipc,
                                                  hhrUNIOfVPLSTable_node_lookup);

        if (NULL == hhrUNIOfVPLSTable_cache)
        {
            return NULL;
        }
    }

    data_index.vsi_id    = index_vsi;
    data_index.vsi_if_id = index_vsi_if;

    pdata = snmp_cache_get_data_by_index(hhrUNIOfVPLSTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->vsi_id, pdata->vsi_if_id);
    }

    switch (vp->magic)
    {
        case hhrUNIOfVPLSIfDesc:
            memset(str_value, 0, MPLS_MAX_NAME_LEGTH);
            ifm_get_name_by_ifindex(pdata->vsi_if_id, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        default:
            return (NULL);
    }
}
/*** hhrUNIOfVPLSTable end ****************************************************/

/*** hhrTunnelConfigTable start ***********************************************/
int hhrTunnelConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                           struct tunnel_if *index_input)
{
    struct tunnel_if *pdata = NULL;
    struct tunnel_if *pfree = NULL;
    int data_len = sizeof(struct tunnel_if);
    int data_num = 0;
    int i;

    pdata = mpls_com_get_tunnel_bulk(index_input->tunnel.ifindex, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct tunnel_if *hhrTunnelConfigTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact,
                                                   const struct tunnel_if *index_input)
{
    struct listnode    *node;
    struct tunnel_if   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->tunnel.ifindex))
        {
            return cache->data_list->head->data;
        }

        if (pdata->tunnel.ifindex == index_input->tunnel.ifindex)
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

u_char *hhrTunnelConfigTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct tunnel_if   *pdata = NULL;
    struct tunnel_if    data_index;
    int                 data_len = sizeof(struct tunnel_if);
    int                 ret = 0;
    int                 index_int = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return (NULL);
    }

    if (NULL == hhrTunnelConfigTable_cache)
    {
        hhrTunnelConfigTable_cache = snmp_cache_init(data_len,
                                                     hhrTunnelConfigTable_get_data_from_ipc,
                                                     hhrTunnelConfigTable_node_lookup);

        if (NULL == hhrTunnelConfigTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.tunnel.ifindex = index_int;

    pdata = snmp_cache_get_data_by_index(hhrTunnelConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->tunnel.ifindex);
    }

    switch (vp->magic)
    {
        case hhrTunnelName:
            memset(str_value, 0, MPLS_STRING_LEN);
            ifm_get_name_by_ifindex(pdata->tunnel.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunnelEncapsulateType:
            *var_len = sizeof(int);
            int_value = 0;

            if (TUNNEL_PRO_MPLSTP == pdata->tunnel.protocol)
            {
                int_value = 1;
            }
            else if (TUNNEL_PRO_MPLSTE == pdata->tunnel.protocol)
            {
                int_value = 3;
            }
            else if (TUNNEL_PRO_GRE == pdata->tunnel.protocol)
            {
                int_value = 2;
            }

            return (u_char *)(&int_value);

        case hhrTunnelSrcIP:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->tunnel.sip.addr.ipv4;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrTunnelDestIP:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->tunnel.dip.addr.ipv4;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrTunnelRelatedIngressLspName:
            memset(str_value, 0, MPLS_STRING_LEN);
            memcpy(str_value, pdata->inlsp_name, MPLS_MAX_NAME_LEGTH);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunnelRelatedEgressLspName:
            memset(str_value, 0, MPLS_STRING_LEN);
            memcpy(str_value, pdata->elsp_name, MPLS_MAX_NAME_LEGTH);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunnelLinkStatus:
            *var_len = sizeof(int);
            int_value = (pdata->tunnel.down_flag == 0) ? 1 : 2; // 1:up, 2: down
            return (u_char *)(&int_value);

        case hhrTunneBackupIngressLspName:
            memset(str_value, 0, MPLS_STRING_LEN);
            memcpy(str_value, pdata->backup_inlsp_name, MPLS_MAX_NAME_LEGTH);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunnelBackupEgressLspName:
            memset(str_value, 0, MPLS_STRING_LEN);
            memcpy(str_value, pdata->backup_elsp_name, MPLS_MAX_NAME_LEGTH);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunneOutIf:
            memset(str_value, 0, MPLS_STRING_LEN);
            ifm_get_name_by_ifindex(pdata->tunnel.outif, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrTunnelDestMac:
            *var_len = 6;
            memcpy(&mac_value, pdata->tunnel.dmac, 6);
            return (mac_value);

        case hhrTunnelNexthop:
            *var_len = sizeof(uint32_t);
            ip_value = pdata->tunnel.nexthop;
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrTunnelQosRateLimitIngressCIR:
            *var_len = sizeof(uint32_t);
            uint_value  = pdata->tunnel.car_cir[0];
            return (u_char *)&uint_value;

        case hhrTunnelQosRateLimitIngressPIR:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->tunnel.car_pir[0];
            return (uchar *)&uint_value;

        case hhrTunnelQosRateLimitEgressCIR:

            *var_len = sizeof(uint32_t);
            uint_value = pdata->tunnel.car_cir[1];
            return (uchar *)&uint_value;

        case hhrTunnelQosRateLimitEgressPIR:

            *var_len = sizeof(uint32_t);
            uint_value = pdata->tunnel.car_pir[1];
            return (u_char *)&uint_value;

        case hhrTunnelWorkingStatus:
            *var_len = sizeof(int);
            int_value = pdata->tunnel.backup_status;
            return (u_char *)&int_value;

        case hhrTunnelFailback:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->tunnel.failback;
            return (u_char *)&uint_value;

        case hhrTunnelFailbackWtr:
            *var_len = sizeof(int) ;
            int_value = pdata->tunnel.wtr;
            return (u_char *)&int_value;

        default:
            return (NULL);
    }
}
/*** hhrTunnelConfigTable end *************************************************/

/*** hhrTunnelStatisticsTable start *******************************************/
u_char *hhrTunnelStatisticsTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct tunnel_if   *pdata = NULL;
    struct tunnel_if    data_index;
    int                 data_len = sizeof(struct tunnel_if);
    int                 ret = 0;
    int                 index_int = 0;
    struct counter_t   *pcounter_data;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (ret < 0)
    {
        return (NULL);
    }

    if (NULL == hhrTunnelConfigTable_cache)
    {
        hhrTunnelConfigTable_cache = snmp_cache_init(data_len,
                                                     hhrTunnelConfigTable_get_data_from_ipc,
                                                     hhrTunnelConfigTable_node_lookup);

        if (NULL == hhrTunnelConfigTable_cache)
        {
            return NULL;
        }
    }

    memset(&data_index, 0, data_len);
    data_index.tunnel.ifindex = index_int;

    pdata = snmp_cache_get_data_by_index(hhrTunnelConfigTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }
    pcounter_data = mpls_com_get_tunnel_counter(pdata->tunnel.ifindex, MODULE_ID_SNMPD);

    if (NULL == pcounter_data)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->tunnel.ifindex);
    }
    switch (vp->magic)
    {
        case hhrTSInputTotalBytes:
            uint64_value.high = (pcounter_data->rx_bytes >> 32);
            uint64_value.low  = (pcounter_data->rx_bytes);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrTSInputTotalPackets:
            uint64_value.high = (pcounter_data->rx_packets >> 32);
            uint64_value.low  = (pcounter_data->rx_packets);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrTSInputDropPackets:
            uint64_value.high = (pcounter_data->rx_dropped >> 32);
            uint64_value.low  = (pcounter_data->rx_dropped);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrTSOutputTotalBytes:
            uint64_value.high = (pcounter_data->tx_bytes >> 32);
            uint64_value.low  = (pcounter_data->tx_bytes);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrTSOutputTotalPackets:
            uint64_value.high = (pcounter_data->tx_packets >> 32);
            uint64_value.low  = (pcounter_data->tx_packets);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrTSOutputDropPackets:
            uint64_value.high = (pcounter_data->tx_dropped >> 32);
            uint64_value.low  = (pcounter_data->tx_dropped);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        default:
            return (NULL);
    }
}
/*** hhrTunnelStatisticsTable end *********************************************/

/*** hhrPWStatisticsTable start ***********************************************/
u_char *hhrPWStatisticsTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l2vc_entry  *pdata = NULL;
    struct l2vc_entry   data_index;
    int                 data_len = sizeof(struct l2vc_entry);
    u_char              sIndex[MPLS_MAX_NAME_LEGTH] = {0};
    u_char              sNext_index[MPLS_MAX_NAME_LEGTH] = {0};
    int                 key_len;
    int                 ret = 0;
    struct counter_t   *pcounter_data ;

    *write_method = NULL;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, MPLS_MAX_NAME_LEGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        return NULL;
    }

    if (NULL == hhrPseudoWireTable_cache)
    {
        hhrPseudoWireTable_cache = snmp_cache_init(sizeof(struct l2vc_entry),
                                                   hhrPseudoWireTable_get_data_from_ipc,
                                                   hhrPseudoWireTable_node_lookup);

        if (NULL == hhrPseudoWireTable_cache)
        {
            return NULL;
        }
    }

    key_len = MPLS_MAX_NAME_LEGTH;
    memset(&data_index, 0, data_len);
    memcpy(data_index.name, sIndex, key_len);

    pdata = snmp_cache_get_data_by_index(hhrPseudoWireTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }
    pcounter_data = mpls_com_get_l2vc_counter(pdata->name, MODULE_ID_SNMPD);

    if (NULL == pcounter_data)
    {
        return NULL;
    }

    if (0 == exact)     /* get ready the next index */
    {
        snprintf(sNext_index, MPLS_MAX_NAME_LEGTH, "%s", pdata->name);
        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index)) ;
    }
    switch (vp->magic)
    {
        case hhrPSInputTotalBytes:
            uint64_value.high = (pcounter_data->rx_bytes >> 32);
            uint64_value.low  = (pcounter_data->rx_bytes);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrPSInputTotalPackets:
            uint64_value.high = (pcounter_data->rx_packets >> 32);
            uint64_value.low  = (pcounter_data->rx_packets);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrPSInputDropPackets:
            uint64_value.high = (pcounter_data->rx_dropped >> 32);
            uint64_value.low  = (pcounter_data->rx_dropped);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrPSOutputTotalBytes:
            uint64_value.high = (pcounter_data->tx_bytes >> 32);
            uint64_value.low  = (pcounter_data->tx_bytes);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrPSOutputTotalPackets:
            uint64_value.high = (pcounter_data->tx_packets >> 32);
            uint64_value.low  = (pcounter_data->tx_packets);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        case hhrPSOutputDropPackets:
            uint64_value.high = (pcounter_data->tx_dropped >> 32);
            uint64_value.low  = (pcounter_data->tx_dropped);
            *var_len = sizeof(struct counter64);
            mem_share_free_bydata(pcounter_data, MODULE_ID_SNMPD);
            return (u_char *)&uint64_value;

        default:
            return (NULL);
    }
}
/*** hhrPWStatisticsTable end *************************************************/

int hhrMplsApsTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                      struct snmp_aps_sess *index_input)
{
    struct ipc_mesg_n *pmesg = NULL;
    struct snmp_aps_sess *pdata = NULL;
    int data_len = sizeof(struct snmp_aps_sess);
    int data_num = 0;
    int i;

    pmesg = ipc_sync_send_n2(&index_input->info.sess_id, 4, 1,
                                        MODULE_ID_MPLS,
                                        MODULE_ID_SNMPD,
                                        IPC_TYPE_MPLSAPS,
                                        0,
                                        IPC_OPCODE_GET_BULK,
                                        index_input->info.sess_id, 1);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num    = pmesg->msghdr.data_num;
    pdata       = (struct snmp_aps_sess *)pmesg->msg_data;

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }


		mem_share_free(pmesg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct snmp_aps_sess *hhrMplsApsTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                  int exact,
                                                  const struct snmp_aps_sess *index_input)
{
    struct listnode        *node;
    struct snmp_aps_sess   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 0 == index_input->info.sess_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->info.sess_id == index_input->info.sess_id)
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

u_char *hhrMplsApsTable_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    struct snmp_aps_sess   *pdata = NULL;
    struct snmp_aps_sess    data_index;
    int                     data_len = sizeof(struct snmp_aps_sess);
    int                     ret = 0;
    int                     index_int = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrMplsApsTable_cache)
    {
        hhrMplsApsTable_cache = snmp_cache_init(sizeof(struct snmp_aps_sess),
                                                hhrMplsApsTable_get_data_from_ipc,
                                                hhrMplsApsTable_node_lookup);

        if (NULL == hhrMplsApsTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct snmp_aps_sess));
    data_index.info.sess_id = index_int;

    pdata = snmp_cache_get_data_by_index(hhrMplsApsTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->info.sess_id)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->info.sess_id);
    }

    switch (vp->magic)
    {
        case hhrMplsApsKeepAlive:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.keepalive;
            return (u_char *)(&uint_value);

        case hhrMplsApsHoldOff:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.holdoff;
            return (u_char *)(&uint_value);

        case hhrMplsApsBackup:
            *var_len = sizeof(int);
            int_value = (0 != pdata->info.failback) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrMplsApsBackupFailbackWtr:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.wtr;
            return (u_char *)(&uint_value);

        case hhrMplsApsPriority:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.priority;
            return (u_char *)(&uint_value);

        case hhrMplsApsMasterIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.master_index;
            return (u_char *)(&uint_value);

        case hhrMplsApsMasterName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->master_name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrMplsApsBackupIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.backup_index;
            return (u_char *)(&uint_value);

        case hhrMplsApsBackupName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->backup_name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrMplsApsEnable:
            *var_len = sizeof(int);
            int_value = (0 != pdata->info.status) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrMplsApsActivePath:
            *var_len = sizeof(int);
            int_value = pdata->info.backup;
            return (u_char *)(&int_value);

        case hhrMplsApsCurrentState:
            *var_len = sizeof(int);

            switch (pdata->info.current_status)
            {
                case 0:
                    int_value = 1;
                    break;

                case 1:
                    int_value = 2;
                    break;

                case 5:
                    int_value = 3;
                    break;

                case 7:
                    int_value = 4;
                    break;

                case 11:
                    int_value = 5;
                    break;

                case 13:
                    int_value = 6;
                    break;

                case 14:
                    int_value = 7;
                    break;

                case 15:
                    int_value = 8;
                    break;

                default:
                    int_value = 9;
                    break;
            }

            return (u_char *)(&int_value);

        case hhrMplsApsType:
            *var_len = sizeof(int);
            int_value = (APS_TYPE_PW == pdata->info.type) ? 1 : 2;
            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}


int hhrL3vpnInstanceTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct l3vpn_entry *index)
{
    struct l3vpn_entry *pdata = NULL;
    struct l3vpn_entry *pfree = NULL;
    int data_num = 0;
    int i = 0;
    pdata = mpls_com_get_l3vpn_bulk(index->l3vpn.vrf_id, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct l3vpn_entry));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}
struct l3vpn_entry *hhrL3vpnInstanceTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct l3vpn_entry  *index_input)
{
    struct l3vpn_entry *pdata;
    struct listnode        *node;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {

        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if ((NULL == index_input) || (0 == index_input->l3vpn.vrf_id))
        {
            return cache->data_list->head->data;
        }

        if (pdata->l3vpn.vrf_id == index_input->l3vpn.vrf_id)
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

u_char *hhrL3vpnInstanceTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l3vpn_entry *pdata = NULL;
    struct l3vpn_entry data_index;
    unsigned int id_index = 0;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &id_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    data_index.l3vpn.vrf_id = id_index;

    if (0 == hhrL3vpnInstanceTable_cache)
    {
        hhrL3vpnInstanceTable_cache = snmp_cache_init(sizeof(struct l3vpn_entry),
                                                      hhrL3vpnInstanceTable_data_form_ipc,
                                                      hhrL3vpnInstanceTable_node_lookup);

        if (NULL == hhrL3vpnInstanceTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrL3vpnInstanceTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->l3vpn.vrf_id);
    }

    switch (vp->magic)
    {
        case hhrL3vpnInstanceName:
            memset(str_value, 0, NAME_STRING_LEN);
            memcpy(str_value, pdata->name, strlen(pdata->name));
            *var_len = strlen(pdata->name);
            return (uchar *)str_value;

        case hhrL3vpnApplyLabel:
            uint_value = pdata->label_apply;
            *var_len = sizeof(uint32_t);
            return (uchar *)&uint_value;

        case hhrL3vpnRD:
            memset(str_value, 0, NAME_STRING_LEN);
            snprintf(str_value, NAME_STRING_LEN, "%s", pdata->rd);
            *var_len = strlen(str_value);
            return (uchar *)str_value;

        default:
            return NULL;
    }
}

int hhrL3vpnTargetTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct l3vpn_target *index)
{
    struct  l3vpn_target *pdata = NULL;
    struct l3vpn_target *pfree = NULL;
    int data_num = 0;
    int i = 0;

    if (NULL == index)
    {
        return FALSE;
    }

    if (L3VPN_SUBTYPE_INSTANCE == index->target_type || L3VPN_SUBTYPE_EXPORT == index->target_type)
    {
        index->target_type = L3VPN_SUBTYPE_EXPORT;
    }
    else
    {
        index->target_type = L3VPN_SUBTYPE_IMPORT;
    }

    pdata = mpls_com_get_l3vpn_target_bulk(index, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, sizeof(struct  l3vpn_target));
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}
struct l3vpn_target *hhrL3vpnTargetTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                     int exact,
                                                     const struct l3vpn_target  *index_input)
{
    struct l3vpn_target *pdata;
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

        if ((0 == index_input->vrf_id) && (0 == index_input->target[0]) && (0 == index_input->target_type))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->vrf_id == index_input->vrf_id) && (0 == strcmp(pdata->target, index_input->target)) && (pdata->target_type == index_input->target_type))
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

u_char *hhrL3vpnTargetTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct l3vpn_target *pdata;
    struct l3vpn_target data_index;
    unsigned int id_index1 = 0;
    unsigned int id_index2 = 0;
    char index_target[NAME_STRING_LEN] = {'\0'};
    int target_type = 0;
    int ret = 0;
    int max_len = sizeof(index_target);
    int stringlen = sizeof(index_target);
    int i = 0;

    memset(&data_index, 0, sizeof(struct l3vpn_target));
    ret = ipran_snmp_int_str_int_index_get(vp, name, length, &id_index1, index_target, max_len, &max_len, &id_index2, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrL3vpnTargetTable_export_cache)
    {
        hhrL3vpnTargetTable_export_cache = snmp_cache_init(sizeof(struct l3vpn_target),
                                                           hhrL3vpnTargetTable_data_form_ipc,
                                                           hhrL3vpnTargetTable_node_lookup);

        if (NULL == hhrL3vpnTargetTable_export_cache)
        {
            return (NULL);
        }
    }

    data_index.vrf_id = id_index1;

    if (2 == id_index2)
    {
        data_index.target_type = L3VPN_SUBTYPE_IMPORT;
    }
    else if (1 == id_index2)
    {
        data_index.target_type = L3VPN_SUBTYPE_EXPORT;
    }
    else
    {
        data_index.target_type = 0;
    }

    memcpy(&(data_index).target, index_target, NAME_STRING_LEN);

    pdata = snmp_cache_get_data_by_index(hhrL3vpnTargetTable_export_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        if (L3VPN_SUBTYPE_IMPORT == pdata->target_type)
        {
            target_type = 2;
        }
        else
        {
            target_type = 1;
        }

        ipran_snmp_int_str_int_index_set(vp, name, length, pdata->vrf_id, pdata->target, strlen(pdata->target), target_type);
    }

    switch (vp->magic)
    {
        case hhrL3vpnTargetType:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}

#if 0

/*hhrMplsApsTable*/
static struct ipran_snmp_data_cache *hhrMplsVplsProtectTable_cache = NULL;
static oid hhrMplsVplsProtectTable_oid[] = {HHRMPLSCONFIG, 21};
FindVarMethod hhrMplsVplsProtectTable_get;
struct variable2 hhrMplsVplsProtectTable_variables[] =
{
    {hhrMplsApsKeepAlive,         ASN_UNSIGNED,  RONLY, hhrMplsVplsProtectTable_get, 2, {1,  2}},
};

/*** hhrPWStatisticsTable end *************************************************/

int hhrMplsVplsProtectTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                              struct snmp_aps_sess *index_input)
{
    struct ipc_mesg *pmesg = NULL;
    struct snmp_aps_sess *pdata = NULL;
    int data_len = sizeof(struct snmp_aps_sess);
    int data_num = 0;
    int i;

    pmesg = ipc_send_common_wait_reply1(&index_input->info.sess_id, 4, 1,
                                        MODULE_ID_MPLS,
                                        MODULE_ID_SNMPD,
                                        IPC_TYPE_MPLSAPS,
                                        0,
                                        IPC_OPCODE_GET_BULK,
                                        index_input->info.sess_id);

    if (NULL == pmesg)
    {
        return FALSE;
    }

    data_num    = pmesg->msghdr.data_num;
    pdata       = (struct snmp_aps_sess *)pmesg->msg_data;

    if (0 == data_num || NULL == pdata)
    {
        return FALSE;
    }
    else
    {
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }

        return TRUE;
    }
}


struct snmp_aps_sess *hhrMplsVplsProtectTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                          int exact,
                                                          const struct snmp_aps_sess *index_input)
{
    struct listnode        *node;
    struct snmp_aps_sess   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 0 == index_input->info.sess_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->info.sess_id == index_input->info.sess_id)
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

u_char *hhrMplsVplsProtectTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct snmp_aps_sess   *pdata = NULL;
    struct snmp_aps_sess    data_index;
    int                     data_len = sizeof(struct snmp_aps_sess);
    int                     ret = 0;
    int                     index_int = 0;

    *write_method = NULL;

    ret = ipran_snmp_int_index_get(vp, name, length, &index_int, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrMplsVplsProtectTable_cache)
    {
        hhrMplsVplsProtectTable_cache = snmp_cache_init(sizeof(struct snmp_aps_sess),
                                                        hhrMplsServiceTable_get_data_from_ipc,
                                                        hhrMplsServiceTable_node_lookup);

        if (NULL == hhrMplsVplsProtectTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct snmp_aps_sess));
    data_index.info.sess_id = index_int;

    pdata = snmp_cache_get_data_by_index(hhrMplsVplsProtectTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->info.sess_id)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->info.sess_id);
    }

    switch (vp->magic)
    {
        case hhrMplsApsKeepAlive:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.keepalive;
            return (u_char *)(&uint_value);

        case hhrMplsApsHoldOff:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.holdoff;
            return (u_char *)(&uint_value);

        case hhrMplsApsBackup:
            *var_len = sizeof(int);
            int_value = (0 != pdata->info.failback) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrMplsApsBackupFailbackWtr:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.wtr;
            return (u_char *)(&uint_value);

        case hhrMplsApsPriority:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.priority;
            return (u_char *)(&uint_value);

        case hhrMplsApsMasterIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.master_index;
            return (u_char *)(&uint_value);

        case hhrMplsApsMasterName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->master_name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrMplsApsBackupIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->info.backup_index;
            return (u_char *)(&uint_value);

        case hhrMplsApsBackupName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->backup_name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrMplsApsEnable:
            *var_len = sizeof(int);
            int_value = (0 != pdata->info.status) ? 1 : 2;
            return (u_char *)(&int_value);

        case hhrMplsApsActivePath:
            *var_len = sizeof(int);
            int_value = pdata->info.backup;
            return (u_char *)(&int_value);

        case hhrMplsApsCurrentState:
            *var_len = sizeof(int);

            switch (pdata->info.current_status)
            {
                case 0:
                    int_value = 1;
                    break;

                case 1:
                    int_value = 2;
                    break;

                case 5:
                    int_value = 3;
                    break;

                case 7:
                    int_value = 4;
                    break;

                case 11:
                    int_value = 5;
                    break;

                case 13:
                    int_value = 6;
                    break;

                case 14:
                    int_value = 7;
                    break;

                case 15:
                    int_value = 8;
                    break;

                default:
                    int_value = 9;
                    break;
            }

            return (u_char *)(&int_value);

        case hhrMplsApsType:
            *var_len = sizeof(int);
            int_value = (APS_TYPE_PW == pdata->info.type) ? 1 : 2;
            return (u_char *)(&int_value);

        default:
            return NULL;
    }

    return NULL;
}

#endif



/*** hhrPWStatisticsTable end *************************************************/

int hhrMplsServiceTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                          struct mpls_service *index_input)
{
    struct ipc_mesg *pmesg = NULL;
    struct mpls_service *pdata = NULL;
    struct mpls_service *pfree = NULL;
    int data_len = sizeof(struct mpls_service);
    int data_num = 0;
    int i;
    int index_flag = 0;

    if (index_input->service_id != 0)
    {
        index_flag = 1;
    }

    pdata = mpls_com_get_mpls_serivce_bulk(index_input, index_flag,  MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct mpls_service *hhrMplsServiceTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                     int exact,
                                                     const struct mpls_service *index_input)
{
    struct listnode        *node;
    struct mpls_service   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 0 == index_input->service_id)
        {
            return cache->data_list->head->data;
        }

        if (pdata->service_id == index_input->service_id)
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

u_char *hhrMplsServiceTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mpls_service    data_index;
    struct mpls_service   *pdata = NULL;
    int                    ret = 0;

    memset(&data_index, 0, sizeof(struct mpls_service));
    ret = ipran_snmp_int_index_get(vp, name, length, &data_index.service_id, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrMplsServiceTable_cache)
    {
        hhrMplsServiceTable_cache = snmp_cache_init(sizeof(struct mpls_service),
                                                    hhrMplsServiceTable_get_data_from_ipc,
                                                    hhrMplsServiceTable_node_lookup);

        if (NULL == hhrMplsServiceTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrMplsServiceTable_cache, exact, &data_index.service_id);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->service_id)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->service_id);
    }

    switch (vp->magic)
    {
        case hhrVplsServiceType:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->service_type;
            return (u_char *)(&uint_value);

        case hhrVplsEtreeServiceName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->name, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrVplsServiceVpnID:
            *var_len = sizeof(int);
            uint_value = pdata->vpn_id;
            return (u_char *)(&uint_value);

        case hhrVplsRootPortName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->root_port, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        default:
            return NULL;
    }

    return NULL;
}



int hhrMplsVplsBlackMacTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                               struct mpls_mac *index_input)
{
    struct mpls_mac *pdata = NULL;
    struct mpls_mac *pfree = NULL;
    int data_len = sizeof(struct mpls_mac);
    int data_num = 0;
    int i;
    uint8_t *pmac = NULL;

    pdata = mpls_com_get_vpls_black_white_mac_bulk(index_input, data_len, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            pmac = pdata->mac;
            zlog_debug("%s[%d]: In function '%s' vplsid %d mac %02x:%02x:%02x:%02x:%02x:%02x\n", 
                __FILE__, __LINE__, __func__, pdata->vplsid, pmac[0], pmac[1], pmac[2], pmac[3],pmac[4], pmac[5]);
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
            pmac = NULL;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct mpls_mac *hhrMplsVplsBlackMacTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                      int exact,
                                                      const struct mpls_mac *index_input)
{
    struct listnode        *node;
    struct mpls_mac   *pdata = NULL;
    uchar              mac[MAC_LEN];
    memset(mac, 0, MAC_LEN);

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 0 == index_input->type || 0 == index_input->vplsid || 0 == memcmp(index_input->mac, mac, MAC_LEN))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->vplsid == index_input->vplsid) && (pdata->type == index_input->type) && (0 == memcmp(pdata->mac, index_input->mac, MAC_LEN)))
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

u_char *hhrMplsVplsBlackMacTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mpls_mac   *pdata = NULL;
    struct mpls_mac    data_index;
    int                data_len = sizeof(struct mpls_mac);
    int                ret = 0;
    int                out_str_len = 0;
    struct mpls_mac    index_data = {0};
    int                 tmp_type = 0;
    int                 temp_vsid = 0 ;
    uchar               tmp_mac[MAC_LEN];

    *write_method = NULL;
    ret = ipran_snmp_intx2_mac_index_get(vp, name, length, &temp_vsid, &tmp_type, tmp_mac, exact);

    if (0 > ret)
    {
        return NULL;
    }

    index_data.vplsid = temp_vsid ;
    index_data.type = tmp_type;
    memcpy(index_data.mac, tmp_mac, MAC_LEN);

    if (NULL == hhrMplsVplsBlackMacTable_getTable_cache)
    {
        hhrMplsVplsBlackMacTable_getTable_cache = snmp_cache_init(sizeof(struct mpls_mac),
                                                                  hhrMplsVplsBlackMacTable_get_data_from_ipc,
                                                                  hhrMplsVplsBlackMacTable_node_lookup);

        if (NULL == hhrMplsVplsBlackMacTable_getTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct mpls_mac));
    memcpy(&data_index, &index_data, sizeof(struct mpls_mac));
    pdata = snmp_cache_get_data_by_index(hhrMplsVplsBlackMacTable_getTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if ((0 == pdata->mac) || (0 == pdata->vplsid) || (0 == pdata->type))
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_intx2_mac_index_set(vp, name, length, pdata->vplsid, pdata->type, pdata->mac);
    }

    switch (vp->magic)
    {
        case hhrVplsMacBlackWhiteType:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->type;
            return (u_char *)(&uint_value);

        case hhrVplsMacBlackWhiteMacAddress:
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, pdata->mac, sizeof(uchar) * 6);
            return mac_value;

        case hhrVplsMacBlackWhiteReservedField:
            *var_len = sizeof(uint32_t);
            uint_value = 0;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }

    return NULL;
}


/*** hhrMplsLabelTable end *************************************************/

int hhrMplsLabelTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                        struct label_info *index_input)
{
    struct ipc_mesg *pmesg = NULL;
    struct label_info *pdata = NULL;
    struct label_info *pfree = NULL;
    int data_len = sizeof(struct label_info);
    int data_num = 0;
    int i;
    int index_flag = 0;

    if (index_input->label != 0)
    {
        index_flag = 1;
    }

    pdata = mpls_com_get_mpls_label_pool_bulk(index_input, index_flag,  MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct label_info *hhrMplsLabelTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                 int exact,
                                                 const struct label_info *index_input)
{
    struct listnode        *node;
    struct label_info   *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if (NULL == pdata || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input || 0 == index_input->label)
        {
            return cache->data_list->head->data;
        }

        if (pdata->label == index_input->label)
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}
#define MPLS_LABELM_MODULE_ID_UNKNOWN           0
#define MPLS_LABELM_MODULE_ID_STATICLSP         1
#define MPLS_LABELM_MODULE_ID_LDP               2
#define MPLS_LABELM_MODULE_ID_BGP               3
#define MPLS_LABELM_MODULE_ID_L2VPN             4
#define MPLS_LABELM_MODULE_ID_L3VPN             5





u_char *hhrMplsLabelTable_get(struct variable *vp,
                              oid *name,
                              size_t *length,
                              int exact, size_t *var_len, WriteMethod **write_method)
{
    struct label_info    data_index;
    struct label_info   *pdata = NULL;
    int                    ret = 0;

    memset(&data_index, 0, sizeof(struct label_info));
    ret = ipran_snmp_int_index_get(vp, name, length, &data_index.label, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrMplsLabelTable_cache)
    {
        hhrMplsLabelTable_cache = snmp_cache_init(sizeof(struct label_info),
                                                  hhrMplsLabelTable_get_data_from_ipc,
                                                  hhrMplsLabelTable_node_lookup);

        if (NULL == hhrMplsLabelTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrMplsLabelTable_cache, exact, &data_index.label);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (0 == pdata->label)
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pdata->label);
    }

    switch (vp->magic)
    {
        case inLabel:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->label;
            return (u_char *)(&uint_value);

        case owner:
            *var_len = sizeof(uint32_t);

            if (MODULE_ID_LDP == pdata->module_id)
            {
                uint_value = MPLS_LABELM_MODULE_ID_LDP;
            }
            else if (MODULE_ID_SLSP == pdata->module_id)
            {
                uint_value = MPLS_LABELM_MODULE_ID_STATICLSP;
            }
            else if (MODULE_ID_L2VPN == pdata->module_id)
            {
                uint_value = MPLS_LABELM_MODULE_ID_L2VPN;
            }
            else if (MODULE_ID_L3VPN == pdata->module_id)
            {
                uint_value = MPLS_LABELM_MODULE_ID_L3VPN;
            }
            else
            {
                uint_value = MPLS_LABELM_MODULE_ID_UNKNOWN;
            }

            return (u_char *)(&uint_value);

        case labelState:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->state;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }

    return NULL;
}




/*** hhrVplsStaticMacTable begin *************************************************/


int hhrVplsStaticMacTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            struct mpls_smac *index_input)
{
    struct mpls_smac *pdata = NULL;
    struct mpls_smac *pfree = NULL;
    int data_len = sizeof(struct mpls_smac);
    int data_num = 0;
    int i;
    pdata = mpls_com_get_vpls_static_mac_bulk(index_input, data_len, MODULE_ID_SNMPD, &data_num);

    if (0 == data_num)
    {
        mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pfree = pdata;
        for (i = 0; i < data_num; i++)
        {
            snmp_cache_add(cache, pdata, data_len);
            pdata++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct mpls_smac *hhrVplsStaticMacTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                    int exact,
                                                    const struct mpls_smac *index_input)
{
    struct listnode        *node;
    struct mpls_smac   *pdata = NULL;
    uchar               mac[MAC_LEN];
    memset(mac, 0, MAC_LEN);

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

        if (index_input->vplsid == 0 && (0 == memcmp(index_input->mac, mac, MAC_LEN)))
        {
            return cache->data_list->head->data;
        }

        if ((pdata->vplsid == index_input->vplsid) && (0 == memcmp(pdata->mac, index_input->mac, MAC_LEN)))
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
                else
                {
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

u_char *hhrVplsStaticMacTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct mpls_smac   *pdata = NULL;
    struct mpls_smac    data_index;
    int                data_len = sizeof(struct mpls_smac);
    int                ret = 0;
    int                out_str_len = 0;
    struct mpls_smac    index_data = {0};
    int                 temp_vsid = 0 ;
    uchar               tmp_mac[MAC_LEN];

    *write_method = NULL;
    memset(tmp_mac, 0 , MAC_LEN);
    ret = ipran_snmp_int_mac_index_get(vp, name, length, &temp_vsid, tmp_mac, exact);

    if (0 > ret)
    {
        return NULL;
    }

    index_data.vplsid = temp_vsid ;
    memcpy(index_data.mac, tmp_mac, MAC_LEN);

    if (NULL == hhrVplsStaticMacTable_getTable_cache)
    {
        hhrVplsStaticMacTable_getTable_cache = snmp_cache_init(sizeof(struct mpls_smac),
                                                               hhrVplsStaticMacTable_get_data_from_ipc,
                                                               hhrVplsStaticMacTable_node_lookup);

        if (NULL == hhrVplsStaticMacTable_getTable_cache)
        {
            return (NULL);
        }
    }

    memset(&data_index, 0, sizeof(struct mpls_smac));
    memcpy(&data_index, &index_data, sizeof(struct mpls_smac));
    pdata = snmp_cache_get_data_by_index(hhrVplsStaticMacTable_getTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if ((0 == pdata->mac) || (0 == pdata->vplsid) || (0 == pdata->type))
    {
        return NULL;
    }

    if (0 == exact)
    {
        ipran_snmp_int_mac_index_set(vp, name, length, pdata->vplsid, pdata->mac);
    }

    switch (vp->magic)
    {
        case hhrVplsStaticMacAddress:
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, pdata->mac, sizeof(uchar) * 6);
            return mac_value;

        case hhrVplsStaticMacBindMode:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->type;
            return (u_char *)(&uint_value);

        case hhrVplsStaticMacIfDescr:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pdata->ifname, NAME_STRING_LEN);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrVplsStaticMacPWId:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->pwid;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }

    return NULL;
}



struct vpls_dynamic_mac_mib *snmp_vpls_dynamic_mac_get_bulk(struct vpls_dynamic_mac_mib *key_entry, int *pdata_num)
{
    struct ipc_mesg_n   *pmesg = ipc_sync_send_n2(key_entry, sizeof(struct vpls_dynamic_mac_mib), 1,
    MODULE_ID_HAL, MODULE_ID_SNMPD, IPC_TYPE_VSI, VSI_INFO_DYNAMIC_MAC_BULK_GET, IPC_OPCODE_GET_BULK, 0,0);

    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct vpls_dynamic_mac_mib *) pmesg->msg_data;
    }

    return NULL;
}

int hhrVplsDynamicMacTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                       struct vpls_dynamic_mac_mib *index_input)
{
  struct vpls_dynamic_mac_mib *pdata = NULL;
  struct vpls_dynamic_mac_mib *pfree = NULL;
  int data_len = sizeof(struct vpls_dynamic_mac_mib);
  int data_num = 0;
  int i;
  struct vpls_dynamic_mac_mib key_entry;
  struct timeval start_time;
  struct timeval end_time;

  memset(&key_entry, 0, sizeof(struct vpls_dynamic_mac_mib));
  key_entry.data.addr_index = index_input->data.addr_index;
  key_entry.index.vsi_id = index_input->index.vsi_id;
  memcpy(key_entry.index.mac, index_input->index.mac, MAC_LEN);
  
  pdata = (struct vpls_dynamic_mac_mib *)snmp_vpls_dynamic_mac_get_bulk(&key_entry, &data_num);
  if (data_num == 0)
  {
      mem_share_free_bydata(pdata, MODULE_ID_SNMPD);
      return FALSE;
  }
  else
  {
      pfree = pdata;
      for (i = 0; i < data_num; i++)
      {
          snmp_cache_add(cache, pdata, data_len);
          pdata++;
      }
      mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
      return TRUE;
  }
}

struct vpls_dynamic_mac_mib *hhrVplsDynamicMacTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                      int exact,
                                                      const struct vpls_dynamic_mac_mib  *index_input)
{
  struct listnode            *node;
  struct vpls_dynamic_mac_mib *pdata = NULL;

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

      if (index_input->index.vsi_id == 0)
      {
          return cache->data_list->head->data;
      }

      if (pdata->index.vsi_id == index_input->index.vsi_id 
          && (!memcmp(pdata->index.mac, index_input->index.mac, MAC_LEN)))
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

u_char *hhrVplsDynamicMacTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct vpls_dynamic_mac_mib   *pMacAddressEntry = NULL;
    struct vpls_dynamic_mac_mib mac_index;
    int temp;
    int ret = 0;

    memset(&mac_index, 0, sizeof(struct vpls_dynamic_mac_mib));

    /* validate the index */
    ret = ipran_snmp_int_mac_index_get(vp, name, length, &mac_index.index.vsi_id, mac_index.index.mac, exact);

    if (ret < 0)
    {
        return NULL;
    }
 
    if (hhrVplsDynamicMacTable_cache == NULL)
    {
        hhrVplsDynamicMacTable_cache = snmp_cache_init(sizeof(struct vpls_dynamic_mac_mib),
                                                 hhrVplsDynamicMacTable_get_data_from_ipc ,
                                                 hhrVplsDynamicMacTable_node_lookup);

        if (hhrVplsDynamicMacTable_cache == NULL)
        {
            return (NULL);
        }
    }

    pMacAddressEntry = snmp_cache_get_data_by_index(hhrVplsDynamicMacTable_cache, exact, &mac_index);

    if (pMacAddressEntry == NULL)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_mac_index_set(vp, name, length, pMacAddressEntry->index.vsi_id, pMacAddressEntry->index.mac);
    }

    switch (vp->magic)
    {
        case hhrVplsDynamicMacAddress:
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, pMacAddressEntry->index.mac, sizeof(uchar) * 6);
            return mac_value;


        case hhrVplsdynamicMacType:
            *var_len = sizeof(uint32_t);
            uint_value = pMacAddressEntry->data.type;
            return (u_char *)(&uint_value);

        case hhrVplsdynamicMacAcIf:
            memset(str_value, 0, sizeof(str_value));
            memcpy((char *)str_value, pMacAddressEntry->data.ac_interface, NAME_STRING_LEN);
            *var_len = strlen((char *)str_value);
            return str_value;    
            
        case hhrVplsdynamicMacPW:
            memset(str_value, 0, sizeof(str_value));
            memcpy((char *)str_value, pMacAddressEntry->data.pw_name, NAME_STRING_LEN);
            *var_len = strlen((char *)str_value);
            return str_value;

        default:

          return NULL;
        }

    return NULL;
}





