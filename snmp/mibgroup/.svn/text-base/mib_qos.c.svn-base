
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
#include <lib/msg_ipc_n.h>
#include <lib/qos_common.h>
#include <lib/mpls_common.h>
#include <qos/hqos.h>
#include <qos/qos_car.h>
#include "ipran_snmp_data_cache.h"
#include <qos/qos_policy.h>
#include <qos/qos_if.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include "snmp_config_table.h"
#include "mib_qos.h"


/*local temp variable*/
static uchar        str_value[STRING_LEN] = {'\0'};
static int          int_value_self = 0;
static uint32_t     ip_value = 0;
static uchar        mac_value[6] = {0};
static uint32_t     uint_value = 0;

static struct ipran_snmp_data_cache *hhrQosAclPolicyTable_cahe = NULL;
static struct ipran_snmp_data_cache *hhrQosAclBasicTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosAclAdvanceTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosAclMacTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosCarProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosCarApplyIfTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosPortRateLimitTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosdomain_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosphb_cache  = NULL;
static struct ipran_snmp_data_cache *hhrQosWREDTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrHQosQueueProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrHQosIfApplyTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosPortUntagPriorityTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingApplyTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosPWRateLimitTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrHQosTunnelApplyTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrHQosPWApplyTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingCosInProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingCosOutProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingExpInProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingExpOutProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingTosInProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingTosOutProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingDscpInProfileTable_cache = NULL;
static struct ipran_snmp_data_cache *hhrQosMappingDscpOutProfileTable_cache = NULL;

enum TABLE_TYPE
{
    HHRQOSCARAPPLYIfTABLE = 0,
    HHRQOSPORTRATELIMITTABLE,
    HHRQOSPWRATELIMITTABLE
};

struct qos_if_doubleindex
{
    enum  QOS_DIR direction;
    struct qos_if_snmp qos_car;
};

struct qosmappingInmultindex_domain
{
    int priority_index;
    struct qos_domain pqos_domain;
};

struct qosmappingInmultindex_phb
{
    struct qos_phb tqos_phb;
    int outcolor;
    int queue;
};

struct queue_profiledoubleindex
{
    struct queue_profile pqueue_profile;
    int queuepriority;
};

struct qosmapingdoubleindex
{
    struct qos_entry_snmp pqos_entry_snmp;
    int dir;
};

struct doubleindex_l2vc_entry
{
    struct l2vc_entry fl2vc_entry;
    int dir;
};

/*
 * Object ID definitions
 */
/*hhrQosPortUntagPriorityTable*/
static oid hhrQosPortUntagPriorityTable_oid[] = {HHRQOSOID, 1};
FindVarMethod hhrQosPortUntagPriorityTable_get;
struct variable2 hhrQosPortUntagPriorityTable_variables[] =
{
    {hhrQosPortUntagPriority,  ASN_UNSIGNED, RONLY, hhrQosPortUntagPriorityTable_get, 2, {1,  1}},
    {hhrQosPortUntagIfDescr, ASN_OCTET_STR, RONLY, hhrQosPortUntagPriorityTable_get, 2,  {1,  2}}
};

/*hhrQosMappingCosInProfileTable*/
static oid hhrQosMappingCosInProfileTable_oid[] = {HHRQOSOID, 2, 1};
FindVarMethod hhrQosMappingCosInProfileTable_get;
struct variable2 hhrQosMappingCosInProfileTable_variables[] =
{

    {hhrQosMappingCosInColor, ASN_INTEGER, RONLY, hhrQosMappingCosInProfileTable_get , 2, {1,  3}},
    {hhrQosMappingCosInQueue, ASN_INTEGER, RONLY, hhrQosMappingCosInProfileTable_get,  2, {1,  4}}
};

/*hhrQosMappingCosOutProfileTable*/
static oid hhrQosMappingCosOutProfileTable_oid[] = {HHRQOSOID, 2, 2};
FindVarMethod hhrQosMappingCosOutProfileTable_get;
struct variable2 hhrQosMappingCosOutProfileTable_variables[] =
{
    {hhrQosMappingCosOutCos,  ASN_INTEGER, RONLY, hhrQosMappingCosOutProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingExpInProfileTable*/
static oid hhrQosMappingExpInProfileTable_oid[] = {HHRQOSOID, 2, 3};
FindVarMethod  hhrQosMappingExpInProfileTable_get;
struct variable2 hhrQosMappingExpInProfileTable_variables[] =
{
    {hhrQosMappingExpInColor, ASN_INTEGER, RONLY, hhrQosMappingExpInProfileTable_get, 2, {1, 3}},
    {hhrQosMappingExpInQueue, ASN_INTEGER, RONLY, hhrQosMappingExpInProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingExpOutProfileTable*/
static oid hhrQosMappingExpOutProfileTable_oid[] = {HHRQOSOID, 2, 4};
FindVarMethod hhrQosMappingExpOutProfileTable_get;
struct variable2 hhrQosMappingExpOutProfileTable_variables[] =
{
    {hhrQosMappingExpOutExp, ASN_INTEGER, RONLY, hhrQosMappingExpOutProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingTosInProfileTable*/
static oid hhrQosMappingTosInProfileTable_oid[] = {HHRQOSOID, 2, 5};
FindVarMethod hhrQosMappingTosInProfileTable_get;
struct variable2 hhrQosMappingTosInProfileTable_variables[] =
{
    {hhrQosMappingTosInColor, ASN_INTEGER, RONLY, hhrQosMappingTosInProfileTable_get, 2, {1, 3}},
    {hhrQosMappingTosInQueue, ASN_INTEGER, RONLY, hhrQosMappingTosInProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingTosOutProfileTable*/
static oid hhrQosMappingTosOutProfileTable_oid[] = {HHRQOSOID, 2, 6};
FindVarMethod hhrQosMappingTosOutProfileTable_get;
struct variable2 hhrQosMappingTosOutProfileTable_variables[] =
{
    {hhrQosMappingTosOutTos,  ASN_INTEGER, RONLY, hhrQosMappingTosOutProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingDscpInProfileTable*/
static oid hhrQosMappingDscpInProfileTable_oid[] = {HHRQOSOID, 2, 7};
FindVarMethod hhrQosMappingDscpInProfileTable_get;
struct variable2 hhrQosMappingDscpInProfileTable_variables[] =
{
    {hhrQosMappingDscpInColor, ASN_INTEGER, RONLY, hhrQosMappingDscpInProfileTable_get, 2, {1, 3}},
    {hhrQosMappingDscpInQueue, ASN_INTEGER, RONLY, hhrQosMappingDscpInProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingDscpOutProfileTable*/
static oid hhrQosMappingDscpOutProfileTable_oid[] = {HHRQOSOID, 2, 8};
FindVarMethod hhrQosMappingDscpOutProfileTable_get;
struct variable2 hhrQosMappingDscpOutProfileTable_variables[] =
{
    {hhrQosMappingDscpOutDscp, ASN_INTEGER, RONLY, hhrQosMappingDscpOutProfileTable_get, 2, {1, 4}}
};

/*hhrQosMappingApplyTable*/
static oid hhrQosMappingApplyTable_oid[] = {HHRQOSOID, 4};
FindVarMethod hhrQosMappingApplyTable_get;
struct variable2 hhrQosMappingApplyTable_variables[] =
{
    {hhrQosMappingApplyDomainType, ASN_INTEGER,   RONLY, hhrQosMappingApplyTable_get, 2, {1,  2}},
    {hhrQosMappingApplyProfileId,  ASN_UNSIGNED,  RONLY, hhrQosMappingApplyTable_get, 2, {1,  3}},
    {hhrQosMappingApplyIfDescr,    ASN_OCTET_STR, RONLY, hhrQosMappingApplyTable_get, 2, {1,  4}}
};


/*hhrQosCarProfileTable*/
static oid hhrQosCarProfileTable_oid[] = {HHRQOSOID, 5};
FindVarMethod hhrQosCarProfileTable_get;
struct variable2 hhrQosCarProfileTable_variables[] =
{
    {hhrQosCarCir,               ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1,  2}},
    {hhrQosCarCbs,               ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1,  3}},
    {hhrQosCarPir,               ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1,  4}},
    {hhrQosCarPbs,               ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1,  5}},
    {hhrQosCarColorMode,         ASN_INTEGER,  RONLY, hhrQosCarProfileTable_get, 2, {1,  6}},
    {hhrQosCarGreenAction,       ASN_INTEGER,  RONLY, hhrQosCarProfileTable_get, 2, {1,  7}},
    {hhrQosCarYellowAction,      ASN_INTEGER,  RONLY, hhrQosCarProfileTable_get, 2, {1,  8}},
    {hhrQosCarRedAction,         ASN_INTEGER,  RONLY, hhrQosCarProfileTable_get, 2, {1,  9}},
    {hhrQosCarGreenRemarkCos,    ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 10}},
    {hhrQosCarGreenRemarkDscp,   ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 11}},
    {hhrQosCarGreenQueue,        ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 12}},
    {hhrQosCarYellowRemarkCos,   ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 13}},
    {hhrQosCarYellowRemarkDscp,  ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 14}},
    {hhrQosCarYellowQueue,       ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 15}},
    {hhrQosCarRedRemarkCos,      ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 16}},
    {hhrQosCarRedRemarkDscp,     ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 17}},
    {hhrQosCarRedQueue,          ASN_UNSIGNED, RONLY, hhrQosCarProfileTable_get, 2, {1, 18}}
};

/*hhrQosCarApplyIfTable*/
static oid hhrQosCarApplyIfTable_oid[] = {HHRQOSOID, 6};
FindVarMethod hhrQosCarApplyIfTable_get;
struct variable2 hhrQosCarApplyIfTable_variables[] =
{
    {hhrQosCarApplyIfDescr,           ASN_OCTET_STR, RONLY, hhrQosCarApplyIfTable_get, 2, {1,  3}},
    {hhrQosCarApplyIfCarProfileIndex, ASN_UNSIGNED,  RONLY, hhrQosCarApplyIfTable_get, 2, {1,  4}}
};

/*hhrQosPortRateLimitTable*/
static oid hhrQosPortRateLimitTable_oid[] = {HHRQOSOID, 7};
FindVarMethod hhrQosPortRateLimitTable_get;
struct variable2 hhrQosPortRateLimitTable_variables[] =
{
    {hhrQosPortRateLimitIfDescr,          ASN_OCTET_STR, RONLY, hhrQosPortRateLimitTable_get, 2, {1,  3}},
    {hhrQosPortRateLimitCIR,              ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1,  4}},
    {hhrQosPortRateLimitCBS,              ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1,  5}},
    {hhrQosPortRateLimitPIR,              ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1,  6}},
    {hhrQosPortRateLimitPBS,              ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1,  7}},
    {hhrQosPortRateLimitColorMode,        ASN_INTEGER,   RONLY, hhrQosPortRateLimitTable_get, 2, {1,  8}},
    {hhrQosPortRateLimitGreenAction,      ASN_INTEGER,   RONLY, hhrQosPortRateLimitTable_get, 2, {1,  9}},
    {hhQosPortRateLimitGreenRemarkCos,    ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 10}},
    {hhrQosPortRateLimitGreenRemarkDscp,  ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 11}},
    {hhrQosPortRateLimitGreenQueue,       ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 12}},
    {hhrQosPortRateLimitYellowAction,     ASN_INTEGER,   RONLY, hhrQosPortRateLimitTable_get, 2, {1, 13}},
    {hhrQosPortRateLimitYellowRemarkCos,  ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 14}},
    {hhrQosPortRateLimitYellowRemarkDscp, ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 15}},
    {hhrQosPortRateLimitYellowQueue,      ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 16}},
    {hhrQosPortRateLimitRedAction,        ASN_INTEGER,   RONLY, hhrQosPortRateLimitTable_get, 2, {1, 17}},
    {hhrQosPortRateLimitRedRemarkCos,     ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 18}},
    {hhrQosPortRateLimitRedRemarkDscp,    ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 19}},
    {hhrQosPortRateLimitRedQueue,         ASN_UNSIGNED,  RONLY, hhrQosPortRateLimitTable_get, 2, {1, 20}}
};

/* hhrQosAclBasicTable */
static oid hhrQosAclBasicTable_oid[] = {HHRQOSOID, 8};
FindVarMethod hhrQosAclBasicTable_get;
struct variable2 hhrQosAclBasicTable_variables[] =
{
    {hhrQosAclBasicIpv4SrcOrDest,   ASN_INTEGER,    RONLY, hhrQosAclBasicTable_get, 2, {1,  3}},
    {hhrQosAclBasicIpv4AnyIp,       ASN_INTEGER,    RONLY, hhrQosAclBasicTable_get, 2, {1,  4}},
    {hhrQosAclBasicIpv4Address,     ASN_IPADDRESS,  RONLY, hhrQosAclBasicTable_get, 2, {1,  5}},
    {hhrQosAclBasicIpv4AddressMask, ASN_IPADDRESS,  RONLY, hhrQosAclBasicTable_get, 2, {1,  6}},
    {hhrQosAclBasicIpv4L3vpn,       ASN_UNSIGNED,   RONLY, hhrQosAclBasicTable_get, 2, {1,  7}}
};

/*hhrQosAclAdvanceTable*/
static oid hhrQosAclAdvanceTable_oid[] = {HHRQOSOID, 9};
FindVarMethod hhrQosAclAdvanceTable_get;
struct variable2 hhrQosAclAdvanceTable_variables[] =
{
//  {hhrQosAclAdvanceIpv4Rule,          ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1,  2}},
    {hhrQosAclAdvanceIpv4ProtocolAny,   ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1,  3}},
    {hhrQosAclAdvanceIpv4Protocol,      ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1,  4}},
    {hhrQosAclAdvanceIpv4Sport,         ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1,  5}},
    {hhrQosAclAdvanceIpv4SipAny,        ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1,  6}},
    {hhrQosAclAdvanceIpv4Sip,           ASN_IPADDRESS,  RONLY, hhrQosAclAdvanceTable_get, 2, {1,  7}},
    {hhrQosAclAdvanceIpv4SipMask,       ASN_IPADDRESS,  RONLY, hhrQosAclAdvanceTable_get, 2, {1,  8}},
    {hhrQosAclAdvanceIpv4Dport,         ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1,  9}},
    {hhrQosAclAdvanceIpv4DipAny,        ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 10}},
    {hhrQosAclAdvanceIpv4Dip,           ASN_IPADDRESS,  RONLY, hhrQosAclAdvanceTable_get, 2, {1, 11}},
    {hhrQosAclAdvanceIpv4DipMask,       ASN_IPADDRESS,  RONLY, hhrQosAclAdvanceTable_get, 2, {1, 12}},
    {hhrQosAclAdvanceIpv4L3vpn,         ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1, 13}},
    {hhrQosAclAdvanceIpv4Dscp,          ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1, 14}},
    {hhrQosAclAdvanceIpv4Ttl,           ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1, 15}},
    {hhrQosAclAdvanceIpv4Syn,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 16}},
    {hhrQosAclAdvanceIpv4Fin,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 17}},
    {hhrQosAclAdvanceIpv4Ack,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 18}},
    {hhrQosAclAdvanceIpv4Rst,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 19}},
    {hhrQosAclAdvanceIpv4Psh,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 20}},
    {hhrQosAclAdvanceIpv4Urg,           ASN_INTEGER,    RONLY, hhrQosAclAdvanceTable_get, 2, {1, 21}},
    {hhrQosAclAdvanceIpv4Tos,           ASN_UNSIGNED,   RONLY, hhrQosAclAdvanceTable_get, 2, {1, 22}}
};

/*hhrQosAclMacTable*/
static oid hhrQosAclMacTable_oid[] = {HHRQOSOID, 10};
FindVarMethod hhrQosAclMacTable_get;
struct variable2 hhrQosAclMacTable_variables[] =
{
    //{hhrQosAclMacRule,            ASN_INTEGER,    RONLY, hhrQosAclMacTable_get,  2, {1,   2}},
    {hhrQosAclMacEthTypeAny,    ASN_INTEGER,    RONLY, hhrQosAclMacTable_get,  2, {1,   3}},
    {hhrQosAclMacEthType,       ASN_OCTET_STR,  RONLY, hhrQosAclMacTable_get,  2, {1,   4}},
    {hhrQosAclMacSmac,          ASN_OCTET_STR,  RONLY, hhrQosAclMacTable_get,  2, {1,   5}},
    {hhrQosAclMacDmac,          ASN_OCTET_STR,  RONLY, hhrQosAclMacTable_get,  2, {1,   6}},
    {hhrQosAclMacUntag,         ASN_INTEGER,    RONLY, hhrQosAclMacTable_get,  2, {1,   7}},
    {hhrQosAclMacVlan,          ASN_UNSIGNED,   RONLY, hhrQosAclMacTable_get,  2, {1,   8}},
    {hhrQosAclMacCos,           ASN_UNSIGNED,   RONLY, hhrQosAclMacTable_get,  2, {1,   9}},
    {hhrQosAclMacCvlan,         ASN_UNSIGNED,   RONLY, hhrQosAclMacTable_get,  2, {1,  10}},
    {hhrQosAclMacCvlanCos,      ASN_UNSIGNED,   RONLY, hhrQosAclMacTable_get,  2, {1,  11}},
    {hhrQosAclMacOffset,        ASN_UNSIGNED,   RONLY, hhrQosAclMacTable_get,  2, {1,  12}},
    {hhrQosAclMacOffsetInfo,    ASN_OCTET_STR,  RONLY, hhrQosAclMacTable_get,  2, {1,  13}},
    {hhrQosAclMacSmacConfig,    ASN_INTEGER,    RONLY, hhrQosAclMacTable_get,  2, {1,  14}},
    {hhrQosAclMacDmacConfig,    ASN_INTEGER,    RONLY, hhrQosAclMacTable_get,  2, {1,  15}}
};

/*hhrQosAclPolicyTable*/
static oid hhrQosAclPolicyTable_oid[] = {HHRQOSOID, 11};
FindVarMethod  hhrQosAclPolicyTable_get;
struct variable2  hhrQosAclPolicyTable_variables[] =
{
    {hhrQosAclPolicyFilter,          ASN_INTEGER,   RONLY, hhrQosAclPolicyTable_get, 2, {1,  5}},
    {hhrQosAclPolicyIfDesc,          ASN_OCTET_STR, RONLY, hhrQosAclPolicyTable_get, 2, {1,  6}},
    {hhrQosAclPolicyqosMappingQueue, ASN_UNSIGNED,  RONLY, hhrQosAclPolicyTable_get, 2, {1,  7}},
    {hhrQosAclPolicyCar,             ASN_UNSIGNED,  RONLY, hhrQosAclPolicyTable_get, 2, {1,  8}},
    {hhrQosAclPolicyApplyIfDesc,     ASN_OCTET_STR, RONLY, hhrQosAclPolicyTable_get, 2, {1,  9}}
};
/*hhrQosWREDTable*/
static oid hhrQosWREDTable_oid[] = {HHRQOSOID, 12, 1};
FindVarMethod hhrQosWREDTable_get;
struct variable2 hhrQosWREDTable_variables[] =
{
    {hhrQosWredGreenPacketDropLowThreshold,     ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  2}},
    {hhrQosWredGreenPacketDropHighThreshold,    ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  3}},
    {hhrQosWredGreenPacketMaxDropRatio,         ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  4}},
    {hhrQosWredYellowPacketDropLowThreshold,    ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  5}},
    {hhrQosWredYellowPacketDropHighThreshold,   ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  6}},
    {hhrQosWredYellowPacketMaxDropRatio,        ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  7}},
    {hhrQosWredRedPacketDropLowThreshold,       ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  8}},
    {hhrQosWredRedPacketDropHighThreshold,      ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1,  9}},
    {hhrQosWredRedPacketMaxDropRatio,           ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 10}},

    {hhrQosWredGreenNonTCPPacketDropLowThreshold,           ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 11}},
    {hhrQosWredGreenNonTCPPacketDropHighThreshold,          ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 12}},
    {hhrQosWredGreenNonTCPPacketMaxDropRatio,               ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 13}},
    {hhrQosWredYellowNonTCPPacketDropLowThreshold,          ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 14}},
    {hhrQosWredYellowNonTCPPacketDropHighThreshold,         ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 15}},
    {hhrQosWredYellowNonTCPPacketMaxDropRatio,              ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 16}},
    {hhrQosWredRedNonTCPPacketDropLowThreshold,             ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 17}},
    {hhrQosWredRedNonTCPPacketDropHighThreshold,            ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 18}},
    {hhrQosWredRedNonTCPPacketMaxDropRatio,                 ASN_UNSIGNED, RONLY, hhrQosWREDTable_get, 2, {1, 19}}

};
/*hhrHQosQueueProfileTable*/
static oid hhrHQosQueueProfileTable_oid[] = {HHRQOSOID, 12, 2};
FindVarMethod hhrHQosQueueProfileTable_get;
struct variable2 hhrHQosQueueProfileTable_variables[] =
{
    {hhrHQosQueueWeight,    ASN_UNSIGNED, RONLY, hhrHQosQueueProfileTable_get, 2, {1,  3}},
    {hhrHQosQueueCir,       ASN_UNSIGNED, RONLY, hhrHQosQueueProfileTable_get, 2, {1,  4}},
    {hhrHQosQueueWredId,    ASN_UNSIGNED, RONLY, hhrHQosQueueProfileTable_get, 2, {1,  5}},
    {hhrHQosQueueScheduler, ASN_INTEGER,  RONLY, hhrHQosQueueProfileTable_get, 2, {1,  6}},
    {hhrHQosQueuePir,       ASN_UNSIGNED, RONLY, hhrHQosQueueProfileTable_get, 2, {1,  7}}
};

/*hhrHQosIfApplyTable*/
static oid hhrHQosIfApplyTable_oid[] = {HHRQOSOID, 12, 3};
FindVarMethod  hhrHQosIfApplyTable_get;
struct variable2 hhrHQosIfApplyTable_variables[] =
{
    {hhrHQosIfApplyIfDescr,         ASN_OCTET_STR,  RONLY, hhrHQosIfApplyTable_get, 2, {1,  2}},
    {hhrHQosIfApplyPIR,             ASN_UNSIGNED,   RONLY, hhrHQosIfApplyTable_get, 2, {1,  3}},
    {hhrHQosIfApplyPBS,             ASN_UNSIGNED,   RONLY, hhrHQosIfApplyTable_get, 2, {1,  4}},
    {hhrHQosIfApplyQueueProfileId,  ASN_UNSIGNED,   RONLY, hhrHQosIfApplyTable_get, 2, {1,  5}}
};

/*hhrHQosPWApplyTable*/
static oid hhrHQosPWApplyTable_oid[] = {HHRQOSOID, 12, 4};
FindVarMethod  hhrHQosPWApplyTable_get;
struct variable2 hhrHQosPWApplyTable_variables[] =
{
    {hhrHQosPWApplyCIR,            ASN_UNSIGNED, RONLY, hhrHQosPWApplyTable_get, 2, {1,  2}},
    {hhrHQosPWApplyPIR,            ASN_UNSIGNED, RONLY, hhrHQosPWApplyTable_get, 2, {1,  3}},
    {hhrHQosPWApplyQueueProfileId, ASN_UNSIGNED, RONLY, hhrHQosPWApplyTable_get, 2, {1,  4}}
};

/*hhrHQosTunnelApplyTable*/
static oid hhrHQosTunnelApplyTable_oid[] = {HHRQOSOID, 12, 5};
FindVarMethod hhrHQosTunnelApplyTable_get;
struct variable2 hhrHQosTunnelApplyTable_variables[] =
{
    {hhrHQosTunnelApplyTunnelName,     ASN_OCTET_STR, RONLY, hhrHQosTunnelApplyTable_get, 2, {1, 1}},
    {hhrHQosTunnelApplyCIR,            ASN_UNSIGNED,  RONLY, hhrHQosTunnelApplyTable_get, 2, {1, 2}},
    {hhrHQosTunnelApplyPIR,            ASN_UNSIGNED,  RONLY, hhrHQosTunnelApplyTable_get, 2, {1, 3}},
    {hhrHQosTunnelApplyQueueProfileId, ASN_UNSIGNED,  RONLY, hhrHQosTunnelApplyTable_get, 2, {1, 4}}
};

/*hhrQosPWRateLimitTable*/
static oid hhrQosPWRateLimitTable_oid[] = {HHRQOSOID, 13};
FindVarMethod hhrQosPWRateLimitTable_get;
struct variable2 hhrQosPWRateLimitTable_variables[] =
{
    {hhrQosPWRateLimitCIR, ASN_UNSIGNED, RONLY, hhrQosPWRateLimitTable_get, 2, {1, 3}},
    {hhrQosPWRateLimitPIR, ASN_UNSIGNED, RONLY, hhrQosPWRateLimitTable_get, 2, {1, 4}}
};

/*hhrPortMirrorConfigTable*/
static oid hhrPortMirrorConfigTable_oid[] = {hhrPortMirrorConfigOID, 11};
FindVarMethod hhrPortMirrorConfigTable_get;
struct ipran_snmp_data_cache *hhrPortMirrorConfigTable_cache = NULL;
struct variable2 hhrPortMirrorConfigTable_variables[] =
{
    {hhrMirrorIfDescr,      ASN_OCTET_STR, RONLY, hhrPortMirrorConfigTable_get, 2, {1, 1}},
    {hhrMirrorType,         ASN_UNSIGNED,  RONLY, hhrPortMirrorConfigTable_get, 2, {1, 2}},
    {hhrMirrorToPortDescr,  ASN_OCTET_STR, RONLY, hhrPortMirrorConfigTable_get, 2, {1, 3}},
    {hhrMirrorToGroup,      ASN_UNSIGNED,  RONLY, hhrPortMirrorConfigTable_get, 2, {1, 4}},
    {hhrMirrorDirect,       ASN_UNSIGNED,  RONLY, hhrPortMirrorConfigTable_get, 2, {1, 5}}
};

/*hhrMirrorGroupTable*/
static oid hhrMirrorGroupTable_oid[] = {hhrPortMirrorConfigOID, 12};
FindVarMethod hhrMirrorGroupTable_get;
struct ipran_snmp_data_cache *hhrMirrorGroupTable_cache = NULL;
struct variable2 hhrMirrorGroupTable_variables[] =
{
    {hhrMirrorGroupIfDescr, ASN_OCTET_STR, RONLY, hhrMirrorGroupTable_get, 2, {1, 2}}
};

/*hhrQosAclMixTable*/
static oid hhrQosAclMixTable_oid[] = {HHRQOSOID, 14};
FindVarMethod hhrQosAclMixTable_get;
static struct  ipran_snmp_data_cache *hhrQosAclMixTable_cache = NULL;
struct variable2 hhrQosAclMixTable_variables[] =
{
    {hhrQosAclMixEthType,         ASN_OCTET_STR, RONLY, hhrQosAclMixTable_get, 2, {1,  3}},
    {hhrQosAclMixSmac,            ASN_OCTET_STR, RONLY, hhrQosAclMixTable_get, 2, {1,  4}},
    {hhrQosAclMixDmac,            ASN_OCTET_STR, RONLY, hhrQosAclMixTable_get, 2, {1,  5}},
    {hhrQosAclMixVlan,            ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1,  6}},
    {hhrQosAclMixCos,             ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1,  7}},
    {hhrQosAclMixCvlan,           ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1,  8}},
    {hhrQosAclMixCvlanCos,        ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1,  9}},
    {hhrQosAclMixIpv4Protocol,    ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1, 10}},
    {hhrQosAclMixIpv4SipAny,      ASN_INTEGER,   RONLY, hhrQosAclMixTable_get, 2, {1, 11}},
    {hhrQosAclMixIpv4Sip,         ASN_IPADDRESS, RONLY, hhrQosAclMixTable_get, 2, {1, 12}},
    {hhrQosAclMixIpv4SipMask,     ASN_IPADDRESS, RONLY, hhrQosAclMixTable_get, 2, {1, 13}},
    {hhrQosAclMixIpv4DipAny,      ASN_INTEGER,   RONLY, hhrQosAclMixTable_get, 2, {1, 14}},
    {hhrQosAclMixIpv4Dip,         ASN_IPADDRESS, RONLY, hhrQosAclMixTable_get, 2, {1, 15}},
    {hhrQosAclMixIpv4DipMask,     ASN_IPADDRESS, RONLY, hhrQosAclMixTable_get, 2, {1, 16}},
    {hhrQosAclMixIpv4L3vpn,       ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1, 17}},
    {hhrQosAclMixIpv4Dscp,        ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1, 18}},
    {hhrQosAclMixIpv4Ttl,         ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1, 19}},
    {hhrQosAclMixIpv4Tos,         ASN_UNSIGNED,  RONLY, hhrQosAclMixTable_get, 2, {1, 20}},
    {hhrQosAclMixSmacConfig,      ASN_INTEGER,   RONLY, hhrQosAclMixTable_get, 2, {1, 21}},
    {hhrQosAclMixDmacConfig,      ASN_INTEGER,   RONLY, hhrQosAclMixTable_get, 2, {1, 22}}
};

/*hhrQosPortLRTable*/
static oid hhrQosPortLRTable_oid[] = {HHRQOSOID, 15};
FindVarMethod hhrQosPortLRTable_get;
static struct ipran_snmp_data_cache *hhrQosPortLRTable_cache  = NULL;
struct variable2  hhrQosPortLRTable_variables[] =
{
    {hhrQosPortLRIfDescr,        ASN_OCTET_STR, RONLY, hhrQosPortLRTable_get, 2, {1, 3}},
    {hhrQosPortLRCIR,            ASN_UNSIGNED,  RONLY, hhrQosPortLRTable_get, 2, {1, 4}},
    {hhrQosPortLRCBS,            ASN_UNSIGNED,  RONLY, hhrQosPortLRTable_get, 2, {1, 5}}
};

void init_mib_qos(void)
{
    REGISTER_MIB("hhrQosPortUntagPriorityTable", hhrQosPortUntagPriorityTable_variables,
                 variable2, hhrQosPortUntagPriorityTable_oid);

    REGISTER_MIB("hhrQosMappingCosInProfileTable", hhrQosMappingCosInProfileTable_variables,
                 variable2, hhrQosMappingCosInProfileTable_oid);

    REGISTER_MIB("hhrQosMappingCosOutProfileTable", hhrQosMappingCosOutProfileTable_variables,
                 variable2, hhrQosMappingCosOutProfileTable_oid);

    REGISTER_MIB("hhrQosMappingExpInProfileTable", hhrQosMappingExpInProfileTable_variables,
                 variable2, hhrQosMappingExpInProfileTable_oid);

    REGISTER_MIB("hhrQosMappingExpOutProfileTable", hhrQosMappingExpOutProfileTable_variables,
                 variable2, hhrQosMappingExpOutProfileTable_oid);

    REGISTER_MIB("hhrQosMappingTosInProfileTable", hhrQosMappingTosInProfileTable_variables,
                 variable2, hhrQosMappingTosInProfileTable_oid);

    REGISTER_MIB("hhrQosMappingTosOutProfileTable", hhrQosMappingTosOutProfileTable_variables,
                 variable2, hhrQosMappingTosOutProfileTable_oid);

    REGISTER_MIB("hhrQosMappingDscpInProfileTable", hhrQosMappingDscpInProfileTable_variables,
                 variable2, hhrQosMappingDscpInProfileTable_oid);

    REGISTER_MIB("hhrQosMappingDscpOutProfileTable", hhrQosMappingDscpOutProfileTable_variables,
                 variable2, hhrQosMappingDscpOutProfileTable_oid);

    REGISTER_MIB("hhrQosMappingApplyTable", hhrQosMappingApplyTable_variables,
                 variable2, hhrQosMappingApplyTable_oid);

    REGISTER_MIB("hhrQosCarProfileTable", hhrQosCarProfileTable_variables,
                 variable2, hhrQosCarProfileTable_oid);

    REGISTER_MIB("hhrQosCarApplyIfTable", hhrQosCarApplyIfTable_variables,
                 variable2, hhrQosCarApplyIfTable_oid);

    REGISTER_MIB("hhrQosPortRateLimitTable", hhrQosPortRateLimitTable_variables,
                 variable2, hhrQosPortRateLimitTable_oid);

    REGISTER_MIB("hhrQosAclBasicTable", hhrQosAclBasicTable_variables,
                 variable2, hhrQosAclBasicTable_oid);

    REGISTER_MIB("hhrQosAclAdvanceTable", hhrQosAclAdvanceTable_variables,
                 variable2, hhrQosAclAdvanceTable_oid);

    REGISTER_MIB("hhrQosAclMacTable", hhrQosAclMacTable_variables,
                 variable2, hhrQosAclMacTable_oid);

    REGISTER_MIB("hhrQosAclPolicyTable", hhrQosAclPolicyTable_variables,
                 variable2, hhrQosAclPolicyTable_oid);

    REGISTER_MIB("hhrQosWREDTable", hhrQosWREDTable_variables,
                 variable2, hhrQosWREDTable_oid);

    REGISTER_MIB("hhrHQosQueueProfileTable", hhrHQosQueueProfileTable_variables,
                 variable2, hhrHQosQueueProfileTable_oid);

    REGISTER_MIB("hhrHQosIfApplyTable", hhrHQosIfApplyTable_variables,
                 variable2, hhrHQosIfApplyTable_oid);

    REGISTER_MIB("hhrHQosPWApplyTable", hhrHQosPWApplyTable_variables,
                 variable2, hhrHQosPWApplyTable_oid);

    REGISTER_MIB("hhrHQosTunnelApplyTable", hhrHQosTunnelApplyTable_variables,
                 variable2, hhrHQosTunnelApplyTable_oid);

    REGISTER_MIB("hhrQosPWRateLimitTable", hhrQosPWRateLimitTable_variables,
                 variable2, hhrQosPWRateLimitTable_oid);

    REGISTER_MIB("hhrPortMirrorConfigTable", hhrPortMirrorConfigTable_variables,
                 variable2, hhrPortMirrorConfigTable_oid);

    REGISTER_MIB("hhrMirrorGroupTable", hhrMirrorGroupTable_variables,
                 variable2, hhrMirrorGroupTable_oid);

    REGISTER_MIB("hhrQosAclMixTable", hhrQosAclMixTable_variables,
                 variable2, hhrQosAclMixTable_oid);

    REGISTER_MIB("hhrQosPortLRTable", hhrQosPortLRTable_variables,
                 variable2, hhrQosPortLRTable_oid);
}

int hhrQosAclPolicyTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache , struct qos_policy *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct qos_policy *pqos_policy = NULL;
    int data_num = 0;
    int ret = 0;

    if (NULL == index)
    {
        return FALSE;
    }

    
    pmsg = qos_com_get_policy_bulk(index, MODULE_ID_SNMPD, &data_num);
    if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_policy = (struct qos_policy *)pmsg->msg_data;

    if (0 == data_num || NULL == pqos_policy)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pqos_policy , sizeof(struct qos_policy));

            pqos_policy++;
        }
		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct qos_policy *hhrQosAclPolicyTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                    int exact,
                                                    const struct qos_policy  *index_input)
{
    struct listnode     *node, *nnode;
    struct qos_policy   *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input
                || (0 == index_input->ifindex && 0 == index_input->type && 0 == index_input->acl_num && 0 == index_input->direct))
        {
            return cache->data_list->head->data;
        }

        if ((index_input->ifindex == data1_find->ifindex)
                && (index_input->type == data1_find->type)
                && (index_input->direct == data1_find->direct)
                && (index_input->acl_num == data1_find->acl_num))
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


int hhrQosAcl_ip_rule_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rule_node *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct rule_node *prule_node = NULL;
    int               data_num = 0;
    int               ret = 0;
    int               n_flag = 0;
    uint32_t          tmp_aclnum = index->acl_num;
    uint32_t          tmp_ruleid = index->rule.ip_acl.ruleid;

    while (1)
    {       
        pmsg = qos_com_get_acl_rule_bulk(tmp_aclnum, tmp_ruleid, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}

		prule_node = (struct rule_node *)pmsg->msg_data;
        
        if (0 == data_num || NULL == prule_node)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_aclnum = prule_node->acl_num;
                tmp_ruleid = prule_node->rule.ip_acl.ruleid;

                if (prule_node->type == ACL_TYPE_IP)
                {
                    snmp_cache_add(cache, prule_node, sizeof(struct rule_node));
                    n_flag = 1;
                }

                prule_node++;
            }

            if (1 == n_flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

struct rule_node *hhrQosAcl_ip_rule_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                int exact,
                                                const struct rule_node  *index_input)
{
    struct listnode     *node, *nnode;
    struct rule_node    *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if ((NULL == index_input))
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->acl_num && 0 == index_input->rule.ip_acl.ruleid)
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->acl_num == index_input->acl_num)
                && (data1_find->rule.ip_acl.ruleid == index_input->rule.ip_acl.ruleid))
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

int hhrQosAcl_mac_rule_data_form_ipc(struct ipran_snmp_data_cache *cache, struct rule_node *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct rule_node *prule_node = NULL;
    int               data_num = 0;
    int               ret = 0;
    int               n_flag = 0;
    uint32_t          tmp_acl_num = index->acl_num;
    uint32_t          tmp_ruleid = index->rule.mac_acl.ruleid;

    while (1)
    {
        pmsg = qos_com_get_acl_rule_bulk(tmp_acl_num, tmp_ruleid, MODULE_ID_SNMPD, &data_num);
		if (NULL == pmsg)
		{
			return FALSE;
		}
	
		prule_node = (struct rule_node *)pmsg->msg_data;

        if (0 == data_num || NULL == prule_node)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_acl_num = prule_node->acl_num;
                tmp_ruleid = prule_node->rule.mac_acl.ruleid;

                if (prule_node->type == ACL_TYPE_MAC)
                {
                    snmp_cache_add(cache, prule_node, sizeof(struct rule_node));
                    n_flag = 1;
                }

                prule_node++;
            }

            if (1 == n_flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}


struct rule_node *hhrQosAcl_mac_rule_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct rule_node  *index_input)
{
    struct listnode     *node, *nnode;
    struct rule_node    *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->acl_num && 0 == index_input->rule.mac_acl.ruleid)
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->acl_num == index_input->acl_num)
                && (data1_find->rule.mac_acl.ruleid == index_input->rule.mac_acl.ruleid))
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


int hhrQosAcl_ipex_rule_data_form_ipc(struct ipran_snmp_data_cache *cache , struct rule_node *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct rule_node *prule_node = NULL;
    int               data_num = 0;
    int               ret = 0;
    int               n_flag = 0;
    uint32_t          tmp_acl_num = index->acl_num;
    uint32_t          tmp_ruleid = index->rule.ipex_acl.ruleid;

    while (1)
    {
        pmsg = qos_com_get_acl_rule_bulk(tmp_acl_num, tmp_ruleid, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}

		prule_node = (struct rule_node *)pmsg->msg_data;

        if (0 == data_num || NULL == prule_node)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_acl_num = prule_node->acl_num;
                tmp_ruleid = prule_node->rule.ipex_acl.ruleid;

                if (ACL_TYPE_IP_EXTEND == prule_node->type)
                {
                    snmp_cache_add(cache, prule_node , sizeof(struct rule_node));
                    n_flag = 1;
                }

                prule_node++;
            }

            if (1 == n_flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}


struct rule_node *hhrQosAcl_ipex_rule_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                  int exact,
                                                  const struct rule_node  *index_input)
{
    struct listnode  *node, *nnode;
    struct rule_node *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->acl_num && 0 == index_input->rule.ipex_acl.ruleid)
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->acl_num == index_input->acl_num)
                && (data1_find->rule.ipex_acl.ruleid == index_input->rule.ipex_acl.ruleid))
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


int hhrQosCar_Profile_data_form_ipc(struct ipran_snmp_data_cache *cache , struct car_t *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct car_t *pcar_t = NULL;
    int ret = 0;
    int data_num = 0;

    pmsg = qos_com_get_car_profile_bulk(index->car_id, MODULE_ID_SNMPD, &data_num);
    if (NULL == pmsg)
    {
    	return FALSE;
    }
    
    pcar_t = (struct car_t *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pcar_t)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pcar_t, sizeof(struct car_t));
            pcar_t++;
        }
		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct car_t *hhrQosCarProfile_data_lookup(struct ipran_snmp_data_cache *cache ,
                                           int exact,  const struct car_t  *index_input)
{
    struct listnode  *node, *nnode;
    struct car_t     *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->car_id)
        {
            return cache->data_list->head->data;
        }

        if (index_input->car_id == data1_find->car_id)
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


int hhrQosCar_Rate_Limit_data_form_ipc(struct ipran_snmp_data_cache *cache , struct qos_if_doubleindex *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct qos_if_doubleindex *pqos_if_doubleindex = NULL;
    struct qos_if_snmp        *pqos_if_snmp = NULL;
    int                        ret = 0;
    int                        data_num = 0;
    int                        flag = 0;
    int                        i = 0;
    uint32_t                   tmp_ifindex = index->qos_car.ifindex;
    struct qos_if_doubleindex  data_seg;

    while (1)
    {
        pmsg = qos_com_get_qosif_bulk(tmp_ifindex, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}
 
		pqos_if_snmp = (struct qos_if_snmp *)pmsg->msg_data;

        if (0 == data_num || NULL == pqos_if_snmp)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_ifindex = pqos_if_snmp->ifindex;

                memset(&data_seg, 0, sizeof(struct qos_if_doubleindex));
                memcpy(&(data_seg.qos_car), pqos_if_snmp, sizeof(struct qos_if_snmp));

                for (i = 1; i < 3; i++)
                {
                    if (QOS_CAR_FLAG_CAR == pqos_if_snmp->car_flag[i - 1])
                    {
                        data_seg.direction = i;
                        snmp_cache_add(cache, &data_seg, sizeof(struct qos_if_doubleindex));
                        flag = 1;
                    }
                }

                pqos_if_snmp++;
            }

            if (1 == flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

int hhrQosCarApplyIfTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct qos_if_doubleindex *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct qos_if_doubleindex *pqos_if_doubleindex = NULL;
    struct qos_if_snmp        *pqos_if_snmp = NULL;
    int                        ret = 0;
    int                        data_num = 0;
    int                        flag = 0;
    int                        i = 0;
    uint32_t                   tmp_ifindex = index->qos_car.ifindex;
    struct qos_if_doubleindex  data_seg;

    while (1)
    {
        pmsg = qos_com_get_qosif_bulk(tmp_ifindex, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}

		pqos_if_snmp = (struct qos_if_snmp *)pmsg->msg_data;

        if (0 == data_num || NULL == pqos_if_snmp)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_ifindex = pqos_if_snmp->ifindex;

                memset(&data_seg, 0, sizeof(struct qos_if_doubleindex));
                memcpy(&(data_seg.qos_car), pqos_if_snmp, sizeof(struct qos_if_snmp));

                for (i = 1; i < 3; i++)
                {
                    if (pqos_if_snmp->car_id[i - 1] != 0)
                    {
                        data_seg.direction = i;

                        snmp_cache_add(cache, &data_seg, sizeof(struct qos_if_doubleindex));
                        flag = 1;
                    }
                }

                pqos_if_snmp++;
            }

            if (1 == flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

struct qos_if_doubleindex *hhrQosCarApply_Rate_Limit_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                                 int exact,  const struct qos_if_doubleindex   *index_input)
{
    struct listnode             *node, *nnode;
    struct qos_if_doubleindex   *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->qos_car.ifindex && 0 == index_input->direction)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->qos_car.ifindex == data1_find->qos_car.ifindex)
                && (index_input->direction == data1_find->direction))
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


int hhrQosmappingcos_in_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qosmappingInmultindex_domain *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_domain *qos_domainindex = NULL;
    struct qos_domain *pqos_domain = NULL;
    int n_flag = 0;
    int i = 0;

    
    pmsg = qos_com_get_domain_profile_bulk(index->pqos_domain.id, QOS_TYPE_COS, MODULE_ID_SNMPD, &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_domain = (struct qos_domain *)pmsg->msg_data;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_domain)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        qos_domainindex = malloc(sizeof(struct qosmappingInmultindex_domain));

        if (NULL == qos_domainindex)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            memset(qos_domainindex, 0, sizeof(struct qosmappingInmultindex_domain));
            memcpy(&(qos_domainindex->pqos_domain), pqos_domain, sizeof(struct qos_domain));

            for (i = 0; i < 8; i++)
            {
                qos_domainindex->priority_index = i;
                snmp_cache_add(cache, qos_domainindex, sizeof(struct qosmappingInmultindex_domain));
            }

            pqos_domain++;
        }

        free(qos_domainindex);
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct qosmappingInmultindex_domain *hhrQosmappingcos_in_lookup(struct ipran_snmp_data_cache *cache ,
                                                                int exact,  const struct qosmappingInmultindex_domain  *index_input)
{
    struct listnode  *node, *nnode;
    struct qosmappingInmultindex_domain *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->pqos_domain.id && 0 == index_input->priority_index)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->pqos_domain.id == data1_find->pqos_domain.id)
                && (index_input->priority_index == data1_find->priority_index))
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

int hhrQosmappingcos_out_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qosmappingInmultindex_phb *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *phb_multi = NULL;
    struct qos_phb *pqos_phb = NULL;
    int i = 0;
    int j = 0;
    
    pmsg = qos_com_get_phb_profile_bulk(index->tqos_phb.id, QOS_TYPE_COS, MODULE_ID_SNMPD , &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_phb = (struct qos_phb *)pmsg->msg_data;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_phb)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        phb_multi = malloc(sizeof(struct qosmappingInmultindex_phb));

        if (NULL == phb_multi)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            for (i = 1; i < 4; i++)
            {
                for (j = 1; j < 9; j++)
                {
                    phb_multi->outcolor = i;
                    phb_multi->queue = j;
                    memcpy(&(phb_multi->tqos_phb), pqos_phb, sizeof(struct qos_phb));
                    snmp_cache_add(cache, phb_multi, sizeof(struct qosmappingInmultindex_phb));
                }
            }

            pqos_phb++;
        }

        free(phb_multi);
    }
    
	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}


struct qosmappingInmultindex_phb *hhrQosmappingcos_out_lookup(struct ipran_snmp_data_cache *cache ,
                                                              int exact,  const struct qosmappingInmultindex_phb  *index_input)
{
    struct listnode  *node, *nnode;
    struct qosmappingInmultindex_phb *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->tqos_phb.id && 0 == index_input->queue && 0 == index_input->outcolor)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->tqos_phb.id == data1_find->tqos_phb.id)
                && (index_input->outcolor == data1_find->outcolor)
                && (index_input->queue == data1_find->queue))
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

int hhrQosmappingexp_in_data_form_ipc(struct ipran_snmp_data_cache *cache , struct qosmappingInmultindex_domain *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_domain *qos_domainindex = NULL;
    struct qos_domain *pqos_domain = NULL;
    int n_flag = 0;
    int i = 0;
    
    pmsg = qos_com_get_domain_profile_bulk(index->pqos_domain.id, QOS_TYPE_EXP, MODULE_ID_SNMPD, &data_num);
	if ( NULL == pmsg)
	{
		return FALSE;
	}

	pqos_domain = (struct qos_domain *)pmsg->msg_data;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_domain)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        qos_domainindex = malloc(sizeof(struct qosmappingInmultindex_domain));

        if (NULL == qos_domainindex)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            memset(qos_domainindex, 0, sizeof(struct qosmappingInmultindex_domain));
            memcpy(&(qos_domainindex->pqos_domain), pqos_domain, sizeof(struct qos_domain));

            for (i = 0; i < 8; i++)
            {
                qos_domainindex->priority_index = i;
                snmp_cache_add(cache, qos_domainindex, sizeof(struct qosmappingInmultindex_domain));
            }

            pqos_domain++;
        }

        free(qos_domainindex);
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


int hhrQosmappingexp_out_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qosmappingInmultindex_phb *index)
{
    struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *phb_multi = NULL;
    struct qos_phb *pqos_phb = NULL;
    int i = 0;
    int j = 0;

    
    pmsg = qos_com_get_phb_profile_bulk(index->tqos_phb.id, QOS_TYPE_EXP, MODULE_ID_SNMPD , &data_num);
    if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_phb = (struct qos_phb *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_phb)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        phb_multi = malloc(sizeof(struct qosmappingInmultindex_phb));

        if (NULL == phb_multi)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            for (i = 1; i < 4; i++)
            {
                for (j = 1; j < 9; j++)
                {
                    phb_multi->outcolor = i;
                    phb_multi->queue = j;
                    memcpy(&(phb_multi->tqos_phb), pqos_phb, sizeof(struct qos_phb));
                    snmp_cache_add(cache, phb_multi, sizeof(struct qosmappingInmultindex_phb));
                }
            }

            pqos_phb++;
        }

        free(phb_multi);
    }
	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}


int hhrQosmappingtos_in_data_form_ipc(struct ipran_snmp_data_cache *cache , struct qosmappingInmultindex_domain *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_domain *qos_domainindex = NULL;
    struct qos_domain *pqos_domain = NULL;
    int n_flag = 0;
    int i = 0;
    
    pmsg = qos_com_get_domain_profile_bulk(index->pqos_domain.id, QOS_TYPE_TOS, MODULE_ID_SNMPD, &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}
	
	pqos_domain =  (struct qos_domain *)pmsg->msg_data;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_domain)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        qos_domainindex = malloc(sizeof(struct qosmappingInmultindex_domain));

        if (NULL == qos_domainindex)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            memset(qos_domainindex, 0, sizeof(struct qosmappingInmultindex_domain));
            memcpy(&(qos_domainindex->pqos_domain), pqos_domain, sizeof(struct qos_domain));

            for (i = 0; i < 8; i++)
            {
                qos_domainindex->priority_index = i;
                snmp_cache_add(cache, qos_domainindex, sizeof(struct qosmappingInmultindex_domain));
            }

            pqos_domain++;
        }

        free(qos_domainindex);
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


int hhrQosmappingtos_out_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qosmappingInmultindex_phb *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *phb_multi = NULL;
    struct qos_phb *pqos_phb = NULL;
    int i = 0;
    int j = 0;

    pmsg = qos_com_get_phb_profile_bulk(index->tqos_phb.id, QOS_TYPE_TOS, MODULE_ID_SNMPD , &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_phb = (struct qos_phb *)pmsg->msg_data;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_phb)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        phb_multi = malloc(sizeof(struct qosmappingInmultindex_phb));

        if (NULL == phb_multi)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            for (i = 1; i < 4; i++)
            {
                for (j = 1; j < 9; j++)
                {
                    phb_multi->outcolor = i;
                    phb_multi->queue = j;
                    memcpy(&(phb_multi->tqos_phb), pqos_phb, sizeof(struct qos_phb));
                    snmp_cache_add(cache, phb_multi, sizeof(struct qosmappingInmultindex_phb));
                }
            }

            pqos_phb++;
        }

        free(phb_multi);
    }
	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}


int hhrQosmappingdscp_in_data_form_ipc(struct ipran_snmp_data_cache *cache , struct qosmappingInmultindex_domain *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_domain *qos_domainindex = NULL;
    struct qos_domain *pqos_domain = NULL;
    int n_flag = 0;
    int i = 0;
     
    pmsg = qos_com_get_domain_profile_bulk(index->pqos_domain.id, QOS_TYPE_DSCP, MODULE_ID_SNMPD, &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_domain = (struct qos_domain *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_domain)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        qos_domainindex = malloc(sizeof(struct qosmappingInmultindex_domain));

        if (NULL == qos_domainindex)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            memset(qos_domainindex, 0, sizeof(struct qosmappingInmultindex_domain));
            memcpy(&(qos_domainindex->pqos_domain), pqos_domain, sizeof(struct qos_domain));

            for (i = 0; i < 64; i++)
            {
                qos_domainindex->priority_index = i;
                snmp_cache_add(cache, qos_domainindex, sizeof(struct qosmappingInmultindex_domain));
            }

            pqos_domain++;
        }

        free(qos_domainindex);
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


int hhrQosmappingdscp_out_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qosmappingInmultindex_phb *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *phb_multi = NULL;
    struct qos_phb *pqos_phb = NULL;
    int i = 0;
    int j = 0;

    pmsg = qos_com_get_phb_profile_bulk(index->tqos_phb.id, QOS_TYPE_DSCP, MODULE_ID_SNMPD , &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pqos_phb = (struct qos_phb *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (data_num == 0 || NULL == pqos_phb)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        phb_multi = malloc(sizeof(struct qosmappingInmultindex_phb));

        if (NULL == phb_multi)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            for (i = 1; i < 4; i++)
            {
                for (j = 1; j < 9; j++)
                {
                    phb_multi->outcolor = i;
                    phb_multi->queue = j;
                    memcpy(&(phb_multi->tqos_phb), pqos_phb, sizeof(struct qos_phb));
                    snmp_cache_add(cache, phb_multi, sizeof(struct qosmappingInmultindex_phb));
                }
            }

            pqos_phb++;
        }

        free(phb_multi);
    }
	mem_share_free(pmsg, MODULE_ID_SNMPD);
    return TRUE;
}

int hhrHQosQueueProfileTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct queue_profiledoubleindex *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct queue_profiledoubleindex *pqueue_profiledoubleindex = NULL;
    struct queue_profile *pqueue_profile = NULL;
    pmsg =  qos_com_get_queue_profile_bulk(index->pqueue_profile.id, MODULE_ID_SNMPD, &data_num);
    if (NULL == pmsg)
	{
		return FALSE;
	}

    pqueue_profile = (struct queue_profile *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pqueue_profile)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        pqueue_profiledoubleindex = malloc(sizeof(struct queue_profiledoubleindex));

        if (NULL == pqueue_profiledoubleindex)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }

        for (ret = 0; ret < data_num; ret++)
        {
            for (i = 0; i < 8; i++)
            {
                memset(pqueue_profiledoubleindex, 0, sizeof(struct queue_profiledoubleindex));
                pqueue_profiledoubleindex->queuepriority = i;
                memcpy(&(pqueue_profiledoubleindex->pqueue_profile), pqueue_profile, sizeof(struct queue_profile));
                snmp_cache_add(hhrHQosQueueProfileTable_cache, pqueue_profiledoubleindex, sizeof(struct queue_profiledoubleindex));
            }

            pqueue_profile++;
        }

        free(pqueue_profiledoubleindex);
        mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct queue_profiledoubleindex *hhrHQosQueueProfileTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                      int exact,
                                                                      const struct queue_profiledoubleindex *index_input)
{
    struct listnode  *node, *nnode;
    struct queue_profiledoubleindex *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->pqueue_profile.id && 0 == index_input->queuepriority)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->queuepriority == data1_find->queuepriority)
                && (index_input->pqueue_profile.id == data1_find->pqueue_profile.id))
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


int hhrHQosIfApplyTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qos_if_snmp *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int                 data_num = 0;
    int                 ret = 0;
    int                 flag = 0;
    struct qos_if_snmp *pqos_if_snmp = NULL;
    uint32_t            tmp_ifindex = index->ifindex;

    while (1)
    {
        pmsg = qos_com_get_qosif_bulk(tmp_ifindex, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}

		pqos_if_snmp = (struct qos_if_snmp *)pmsg->msg_data;

        if (0 == data_num || NULL == pqos_if_snmp)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_ifindex = pqos_if_snmp->ifindex;

                if (0 != pqos_if_snmp->hqos.id)
                {
                    snmp_cache_add(hhrHQosIfApplyTable_cache, pqos_if_snmp, sizeof(struct qos_if_snmp));
                    flag = 1;
                }

                pqos_if_snmp++;
            }

            if (1 == flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

struct qos_if_snmp *hhrHQosIfApplyTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                    int exact,
                                                    const struct qos_if_snmp  *index_input)
{
    struct listnode     *node, *nnode;
    struct qos_if_snmp  *data1_find = NULL;

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
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

int hhrQosPortUntagPriorityTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qos_entry_snmp *index)
{
    int data_num = 0;
    int ret = 0;
    struct qos_entry_snmp *pqos_if_snmp = NULL;
	struct qos_entry_snmp *tpqos_if_snmp = NULL;
	
    pqos_if_snmp = ifm_get_qos_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	if (NULL == pqos_if_snmp)
	{
		return FALSE;
	}
	
	tpqos_if_snmp = pqos_if_snmp;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    for (ret = 0; ret < data_num; ret++)
    {
        snmp_cache_add(hhrQosPortUntagPriorityTable_cache, pqos_if_snmp, sizeof(struct qos_entry_snmp));
        pqos_if_snmp ++;
    }
	
	mem_share_free_bydata(tpqos_if_snmp, MODULE_ID_SNMPD);
	
    return TRUE;
}

struct qos_entry_snmp *hhrQosPortUntagPriorityTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                           int exact,
                                                           const struct qos_entry_snmp *index_input)
{
    struct listnode        *node, *nnode;
    struct qos_entry_snmp  *data1_find = NULL;

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
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}


int hhrQosMappingApplyTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qos_entry_snmp *index)
{
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct qosmapingdoubleindex *pqosmappingdoubleindex;
    struct qos_entry_snmp *pqos_if_snmp = NULL;
	 struct qos_entry_snmp *tpqos_if_snmp = NULL;
	 
    pqos_if_snmp = ifm_get_qos_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	if (NULL == pqos_if_snmp)
	{
		return FALSE;
	}
	
	tpqos_if_snmp = pqos_if_snmp;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    pqosmappingdoubleindex = malloc(sizeof(struct qosmapingdoubleindex)) ;

    if (NULL == pqosmappingdoubleindex)
    {
        return FALSE;
    }

    for (ret = 0; ret < data_num; ret++)
    {
        memset(pqosmappingdoubleindex, 0, sizeof(struct qosmapingdoubleindex));

        for (i = 1; i < 3; i++)
        {
            pqosmappingdoubleindex->dir = i;
            memcpy(&(pqosmappingdoubleindex->pqos_entry_snmp), pqos_if_snmp, sizeof(struct qos_entry_snmp));
            snmp_cache_add(hhrQosMappingApplyTable_cache, pqosmappingdoubleindex, sizeof(struct qosmapingdoubleindex));
        }

        pqos_if_snmp ++;
    }

    free(pqosmappingdoubleindex);
	mem_share_free_bydata(tpqos_if_snmp, MODULE_ID_SNMPD);
	
    return TRUE;
}

struct qosmapingdoubleindex *hhrQosMappingApplyTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                            int exact,
                                                            const struct qosmapingdoubleindex   *index_input)
{
    struct listnode              *node, *nnode;
    struct qosmapingdoubleindex  *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->pqos_entry_snmp.ifindex && 0 == index_input->dir)
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->pqos_entry_snmp.ifindex == index_input->pqos_entry_snmp.ifindex)
                && (data1_find->dir == index_input->dir))
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


int hhrQosPWRateLimitTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct doubleindex_l2vc_entry *index)
{
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct doubleindex_l2vc_entry *pdoubleindex_l2vc_entry;
    struct l2vc_entry *pl2vc_entry = NULL;
    struct l2vc_entry *pfree       = NULL;

    pl2vc_entry = mpls_com_get_l2vc_bulk(index->fl2vc_entry.name, MODULE_ID_SNMPD, &data_num);
	if (NULL == pl2vc_entry)
    {
        return FALSE;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    pfree = pl2vc_entry;
    pdoubleindex_l2vc_entry = malloc(sizeof(struct doubleindex_l2vc_entry)) ;

    if (NULL == pdoubleindex_l2vc_entry)
    {
    	mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return FALSE;
    }

    for (ret = 0; ret < data_num; ret++)
    {
        memset(pdoubleindex_l2vc_entry, 0, sizeof(struct doubleindex_l2vc_entry));

        for (i = 1; i < 3; i++)
        {
            pdoubleindex_l2vc_entry->dir = i;
            memcpy(&(pdoubleindex_l2vc_entry->fl2vc_entry), pl2vc_entry, sizeof(struct l2vc_entry));
            snmp_cache_add(hhrQosPWRateLimitTable_cache, pdoubleindex_l2vc_entry, sizeof(struct doubleindex_l2vc_entry));
        }

        pl2vc_entry ++;
    }

    free(pdoubleindex_l2vc_entry);
    mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
	
    return TRUE;
}


struct doubleindex_l2vc_entry *hhrQosPWRateLimitTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                                  int exact,
                                                                  const struct doubleindex_l2vc_entry *index_input)
{
    struct listnode                *node, *nnode;
    struct doubleindex_l2vc_entry  *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == strlen(index_input->fl2vc_entry.name))
        {
            return cache->data_list->head->data;
        }

        if ((data1_find->dir == index_input->dir)
                && (0 == strcmp(data1_find->fl2vc_entry.name, index_input->fl2vc_entry.name)))
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

u_char *hhrQosPortUntagPriorityTable_get(struct variable *vp,
                                         oid *name,
                                         size_t *length,
                                         int exact, size_t *var_len, WriteMethod **write_method)
{
    int if_index = 0;
    struct qos_entry_snmp *pqos_entry_snmp = NULL;
    struct qos_entry_snmp index;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ifindex = if_index;

    if (NULL == hhrQosPortUntagPriorityTable_cache)
    {
        hhrQosPortUntagPriorityTable_cache = snmp_cache_init(sizeof(struct qos_entry_snmp),
                                                             hhrQosPortUntagPriorityTable_data_form_ipc,
                                                             hhrQosPortUntagPriorityTable_lookup);

        if (NULL == hhrQosPortUntagPriorityTable_cache)
        {
            return (NULL);
        }
    }

    pqos_entry_snmp = snmp_cache_get_data_by_index(hhrQosPortUntagPriorityTable_cache, exact, &index);

    if (NULL == pqos_entry_snmp)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pqos_entry_snmp->ifindex);
    }

    switch (vp->magic)
    {
        case hhrQosPortUntagPriority:
            *var_len = sizeof(int);

            if (7 >= pqos_entry_snmp->qos.untag_pri)
            {
                uint_value = pqos_entry_snmp->qos.untag_pri;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortUntagIfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(pqos_entry_snmp->ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingCosInProfileTable_get(struct variable *vp,
                                          oid *name,
                                          size_t *length,
                                          int exact, size_t *var_len, WriteMethod **write_method)
{
    int ret = 0;
    int id_index = 0;
    int incos_index = 0;
    struct qosmappingInmultindex_domain *pqoscos_domain = NULL;
    struct qosmappingInmultindex_domain index;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &id_index, &incos_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqos_domain.id = id_index;
    index.priority_index = incos_index;
    index.pqos_domain.type = QOS_TYPE_COS;

    if (NULL == hhrQosMappingCosInProfileTable_cache)
    {
        hhrQosMappingCosInProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_domain),
                                                               hhrQosmappingcos_in_data_form_ipc,
                                                               hhrQosmappingcos_in_lookup);

        if (NULL == hhrQosMappingCosInProfileTable_cache)
        {
            return (NULL);
        }
    }

    pqoscos_domain = snmp_cache_get_data_by_index(hhrQosMappingCosInProfileTable_cache, exact, &index);

    if (NULL == pqoscos_domain)
    {
        return NULL;
    }

    if ((pqoscos_domain->priority_index < 0) || (pqoscos_domain->priority_index >= 8))
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqoscos_domain->pqos_domain.id, pqoscos_domain->priority_index);
    }

    switch (vp->magic)
    {
        case hhrQosMappingCosInColor:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].color + 1;
            return (u_char *)(&int_value_self);

        case hhrQosMappingCosInQueue:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].queue + 1;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingCosOutProfileTable_get(struct variable *vp,
                                           oid *name,
                                           size_t *length,
                                           int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int outcolor_index = 0;
    int queue = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *tqos_phb_index = NULL;
    struct qosmappingInmultindex_phb qos_phb_index;

    ret = ipran_snmp_intx3_index_get(vp, name, length, &id_index, &outcolor_index, &queue, exact);

    if (ret < 0)
    {
        return 0;
    }

    qos_phb_index.tqos_phb.id = id_index;
    qos_phb_index.outcolor = outcolor_index;
    qos_phb_index.queue = queue;
    qos_phb_index.tqos_phb.type = QOS_TYPE_COS;

    if (NULL == hhrQosMappingCosOutProfileTable_cache)
    {
        hhrQosMappingCosOutProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_phb),
                                                                hhrQosmappingcos_out_data_form_ipc,
                                                                hhrQosmappingcos_out_lookup);

        if (NULL == hhrQosMappingCosOutProfileTable_cache)
        {
            return (NULL);
        }
    }

    tqos_phb_index = snmp_cache_get_data_by_index(hhrQosMappingCosOutProfileTable_cache, exact, &qos_phb_index);

    if (NULL == tqos_phb_index)
    {
        return NULL;
    }

    if (tqos_phb_index->outcolor < 1 || tqos_phb_index->outcolor > 3
            || tqos_phb_index->queue < 1 || tqos_phb_index->queue > 8)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, tqos_phb_index->tqos_phb.id, tqos_phb_index->outcolor, tqos_phb_index->queue);
    }

    switch (vp->magic)
    {
        case hhrQosMappingCosOutCos:
            *var_len = sizeof(int);
            int_value_self = tqos_phb_index->tqos_phb.map[3 * (tqos_phb_index->queue - 1) + (tqos_phb_index->outcolor - 1)].priority;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingExpInProfileTable_get(struct variable *vp,
                                          oid *name,
                                          size_t *length,
                                          int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int incos_index = 0;
    struct qosmappingInmultindex_domain *pqoscos_domain = NULL;
    struct qosmappingInmultindex_domain index;
    int ret = 0;
    int tmp = 0;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &id_index, &incos_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqos_domain.id = id_index;
    index.priority_index = incos_index;
    index.pqos_domain.type = QOS_TYPE_EXP;

    if (NULL == hhrQosMappingExpInProfileTable_cache)
    {
        hhrQosMappingExpInProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_domain),
                                                               hhrQosmappingexp_in_data_form_ipc,
                                                               hhrQosmappingcos_in_lookup);

        if (NULL == hhrQosMappingExpInProfileTable_cache)
        {
            return (NULL);
        }
    }

    pqoscos_domain = snmp_cache_get_data_by_index(hhrQosMappingExpInProfileTable_cache, exact, &index);

    if (NULL == pqoscos_domain)
    {
        return NULL;
    }

    if ((pqoscos_domain->priority_index < 0) || (pqoscos_domain->priority_index >= 8))
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqoscos_domain->pqos_domain.id , pqoscos_domain->priority_index);
    }

    switch (vp->magic)
    {
        case hhrQosMappingExpInColor:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].color + 1;
            return (u_char *)(&int_value_self);

        case hhrQosMappingExpInQueue:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].queue + 1;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingExpOutProfileTable_get(struct variable *vp,
                                           oid *name,
                                           size_t *length,
                                           int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int outcolor_index = 0;
    int queue = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *tqos_phb_index = NULL;
    struct qosmappingInmultindex_phb qos_phb_index;
    ret = ipran_snmp_intx3_index_get(vp, name, length, &id_index, &outcolor_index, &queue, exact);

    if (ret < 0)
    {
        return 0;
    }

    qos_phb_index.tqos_phb.id = id_index;
    qos_phb_index.outcolor = outcolor_index;
    qos_phb_index.queue = queue;
    qos_phb_index.tqos_phb.type = QOS_TYPE_EXP;

    if (NULL == hhrQosMappingExpOutProfileTable_cache)
    {
        hhrQosMappingExpOutProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_phb),
                                                                hhrQosmappingexp_out_data_form_ipc,
                                                                hhrQosmappingcos_out_lookup);

        if (NULL == hhrQosMappingExpOutProfileTable_cache)
        {
            return (NULL);
        }
    }

    tqos_phb_index = snmp_cache_get_data_by_index(hhrQosMappingExpOutProfileTable_cache, exact, &qos_phb_index);

    if (NULL == tqos_phb_index)
    {
        return NULL;
    }

    if (tqos_phb_index->outcolor < 1 || tqos_phb_index->outcolor > 3
            || tqos_phb_index->queue < 1 || tqos_phb_index->queue > 8)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, tqos_phb_index->tqos_phb.id, tqos_phb_index->outcolor, tqos_phb_index->queue);
    }

    switch (vp->magic)
    {
        case  hhrQosMappingExpOutExp:
            *var_len = sizeof(int);
            int_value_self = tqos_phb_index->tqos_phb.map[3 * (tqos_phb_index->queue - 1) + (tqos_phb_index->outcolor - 1)].priority;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingTosInProfileTable_get(struct variable *vp,
                                          oid *name,
                                          size_t *length,
                                          int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int incos_index = 0;
    struct qosmappingInmultindex_domain *pqoscos_domain = NULL;
    struct qosmappingInmultindex_domain index;
    int ret = 0;
    int tmp = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &id_index, &incos_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqos_domain.id = id_index;
    index.priority_index = incos_index;
    index.pqos_domain.type = QOS_TYPE_TOS;

    if (NULL == hhrQosMappingTosInProfileTable_cache)
    {
        hhrQosMappingTosInProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_domain),
                                                               hhrQosmappingtos_in_data_form_ipc,
                                                               hhrQosmappingcos_in_lookup);

        if (NULL == hhrQosMappingTosInProfileTable_cache)
        {
            return (NULL);
        }
    }

    pqoscos_domain = snmp_cache_get_data_by_index(hhrQosMappingTosInProfileTable_cache, exact, &index);

    if (NULL == pqoscos_domain)
    {
        return NULL;
    }

    if ((pqoscos_domain->priority_index < 0) || (pqoscos_domain->priority_index >= 8))
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqoscos_domain->pqos_domain.id, pqoscos_domain->priority_index);
    }

    switch (vp->magic)
    {
        case hhrQosMappingTosInColor:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].color + 1;
            return (u_char *)(&int_value_self);

        case hhrQosMappingTosInQueue:

            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].queue + 1;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingTosOutProfileTable_get(struct variable *vp,
                                           oid *name,
                                           size_t *length,
                                           int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int outcolor_index = 0;
    int queue = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *tqos_phb_index = NULL;
    struct qosmappingInmultindex_phb qos_phb_index;
    ret = ipran_snmp_intx3_index_get(vp, name, length, &id_index, &outcolor_index, &queue, exact);

    if (ret < 0)
    {
        return 0;
    }

    qos_phb_index.tqos_phb.id = id_index;
    qos_phb_index.outcolor = outcolor_index;
    qos_phb_index.queue = queue;
    qos_phb_index.tqos_phb.type = QOS_TYPE_TOS;

    if (NULL == hhrQosMappingTosOutProfileTable_cache)
    {
        hhrQosMappingTosOutProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_phb),
                                                                hhrQosmappingtos_out_data_form_ipc,
                                                                hhrQosmappingcos_out_lookup);

        if (NULL == hhrQosMappingTosOutProfileTable_cache)
        {
            return (NULL);
        }
    }

    tqos_phb_index = snmp_cache_get_data_by_index(hhrQosMappingTosOutProfileTable_cache, exact, &qos_phb_index);

    if (NULL == tqos_phb_index)
    {
        return NULL;
    }

    if (tqos_phb_index->outcolor < 1 || tqos_phb_index->outcolor > 3
            || tqos_phb_index->queue < 1 || tqos_phb_index->queue > 8)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, tqos_phb_index->tqos_phb.id, tqos_phb_index->outcolor, tqos_phb_index->queue);
    }

    switch (vp->magic)
    {
        case hhrQosMappingTosOutTos:
            *var_len = sizeof(int);
            int_value_self = tqos_phb_index->tqos_phb.map[3 * (tqos_phb_index->queue - 1) + (tqos_phb_index->outcolor - 1)].priority;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingDscpInProfileTable_get(struct variable *vp,
                                           oid *name,
                                           size_t *length,
                                           int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int incos_index = 0;
    struct qosmappingInmultindex_domain *pqoscos_domain = NULL;
    struct qosmappingInmultindex_domain index;
    int ret = 0;
    int tmp = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &id_index, &incos_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqos_domain.id = id_index;
    index.priority_index = incos_index;
    index.pqos_domain.type = QOS_TYPE_DSCP;

    if (NULL == hhrQosMappingDscpInProfileTable_cache)
    {
        hhrQosMappingDscpInProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_domain),
                                                                hhrQosmappingdscp_in_data_form_ipc,
                                                                hhrQosmappingcos_in_lookup);

        if (NULL == hhrQosMappingDscpInProfileTable_cache)
        {
            return (NULL);
        }
    }

    pqoscos_domain = snmp_cache_get_data_by_index(hhrQosMappingDscpInProfileTable_cache, exact, &index);

    if (NULL == pqoscos_domain)
    {
        return NULL;
    }

    if ((pqoscos_domain->priority_index < 0) || (pqoscos_domain->priority_index >= 64))
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqoscos_domain->pqos_domain.id, pqoscos_domain->priority_index);
    }

    switch (vp->magic)
    {
        case hhrQosMappingDscpInColor:
            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].color + 1;
            return (u_char *)(&int_value_self);

        case hhrQosMappingDscpInQueue:

            *var_len = sizeof(int);
            int_value_self = pqoscos_domain->pqos_domain.map[pqoscos_domain->priority_index].queue + 1;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

uchar *hhrQosMappingDscpOutProfileTable_get(struct variable *vp,
                                            oid *name,
                                            size_t *length,
                                            int exact, size_t *var_len, WriteMethod **write_method)
{
    int id_index = 0;
    int outcolor_index = 0;
    int queue = 0;
    int ret = 0;
    struct qosmappingInmultindex_phb *tqos_phb_index = NULL;
    struct qosmappingInmultindex_phb qos_phb_index;
    ret = ipran_snmp_intx3_index_get(vp, name, length, &id_index, &outcolor_index, &queue, exact);

    if (ret < 0)
    {
        return 0;
    }

    qos_phb_index.tqos_phb.id = id_index;
    qos_phb_index.outcolor = outcolor_index;
    qos_phb_index.queue = queue;
    qos_phb_index.tqos_phb.type = QOS_TYPE_DSCP;

    if (NULL == hhrQosMappingDscpOutProfileTable_cache)
    {
        hhrQosMappingDscpOutProfileTable_cache = snmp_cache_init(sizeof(struct qosmappingInmultindex_phb),
                                                                 hhrQosmappingdscp_out_data_form_ipc,
                                                                 hhrQosmappingcos_out_lookup);

        if (NULL == hhrQosMappingDscpOutProfileTable_cache)
        {
            return (NULL);
        }
    }

    tqos_phb_index = snmp_cache_get_data_by_index(hhrQosMappingDscpOutProfileTable_cache, exact, &qos_phb_index);

    if (NULL == tqos_phb_index)
    {
        return NULL;
    }

    if (tqos_phb_index->outcolor < 1 || tqos_phb_index->outcolor > 3
            || tqos_phb_index->queue < 1 || tqos_phb_index->queue > 8)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx3_index_set(vp, name, length, tqos_phb_index->tqos_phb.id, tqos_phb_index->outcolor, tqos_phb_index->queue);
    }

    switch (vp->magic)
    {
        case  hhrQosMappingDscpOutDscp:
            *var_len = sizeof(int);
            int_value_self = tqos_phb_index->tqos_phb.map[3 * (tqos_phb_index->queue - 1) + (tqos_phb_index->outcolor - 1)].priority;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}

u_char *hhrQosMappingApplyTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{

    int if_index = 0;
    int dir_index = 0;
    struct qosmapingdoubleindex *doublepqos_entry_snmp = NULL;
    struct qosmapingdoubleindex  index;
    int ret = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &dir_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqos_entry_snmp.ifindex = if_index;
    index.dir = dir_index;

    if (NULL == hhrQosMappingApplyTable_cache)
    {
        hhrQosMappingApplyTable_cache = snmp_cache_init(sizeof(struct qosmapingdoubleindex),
                                                        hhrQosMappingApplyTable_data_form_ipc,
                                                        hhrQosMappingApplyTable_lookup);

        if (NULL == hhrQosMappingApplyTable_cache)
        {
            return (NULL);
        }
    }

    doublepqos_entry_snmp = snmp_cache_get_data_by_index(hhrQosMappingApplyTable_cache, exact, &index);

    if (NULL == doublepqos_entry_snmp)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, doublepqos_entry_snmp->pqos_entry_snmp.ifindex , doublepqos_entry_snmp->dir);
    }

    switch (vp->magic)
    {

        case hhrQosMappingApplyDomainType:
            *var_len = sizeof(int);

            if (1 == doublepqos_entry_snmp->dir)
            {
                int_value_self = doublepqos_entry_snmp->pqos_entry_snmp.qos.domain_type;
            }
            else
            {
                int_value_self = doublepqos_entry_snmp->pqos_entry_snmp.qos.phb_type;
            }

            if (QOS_TYPE_COS == int_value_self)
            {
                int_value_self = 1;
            }
            else if (QOS_TYPE_TOS == int_value_self)
            {
                int_value_self = 3;
            }
            else if (QOS_TYPE_EXP == int_value_self)
            {
                int_value_self = 2;
            }
            else if (QOS_TYPE_DSCP == int_value_self)
            {
                int_value_self = 4;
            }

            return (u_char *)(&int_value_self);

        case hhrQosMappingApplyProfileId:
            *var_len = sizeof(uint32_t);

            if (1 == doublepqos_entry_snmp->dir)
            {
                uint_value = doublepqos_entry_snmp->pqos_entry_snmp.qos.domain_id;
            }
            else
            {
                uint_value = doublepqos_entry_snmp->pqos_entry_snmp.qos.phb_id;
            }

            return (u_char *)(&uint_value);

        case hhrQosMappingApplyIfDescr:
            memset(str_value, 0, STRING_LEN);
            ifm_get_name_by_ifindex(doublepqos_entry_snmp->pqos_entry_snmp.ifindex, str_value);
            *var_len = strlen(str_value);
            return (str_value);

        default:
            return NULL;
    }
}


u_char *hhrQosCarProfileTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct car_t *pcar_t = NULL;
    struct car_t index;
    int ret = 0;
    int carid_index = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &carid_index, exact);
    index.car_id = carid_index;

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrQosCarProfileTable_cache == NULL)
    {
        hhrQosCarProfileTable_cache = snmp_cache_init(sizeof(struct car_t),
                                                      hhrQosCar_Profile_data_form_ipc,
                                                      hhrQosCarProfile_data_lookup);

        if (NULL == hhrQosCarProfileTable_cache)
        {
            return (NULL);
        }
    }

    pcar_t = snmp_cache_get_data_by_index(hhrQosCarProfileTable_cache,  exact, &index);

    if (NULL == pcar_t)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pcar_t->car_id);
    }

    switch (vp->magic)
    {
        case hhrQosCarCir:
            *var_len = sizeof(uint32_t);
            uint_value = pcar_t->cir;
            return (u_char *)(&uint_value);

        case hhrQosCarCbs:
            *var_len = sizeof(uint32_t);
            uint_value = pcar_t->cbs;
            return (u_char *)(&uint_value);

        case hhrQosCarPir:
            *var_len = sizeof(uint32_t);
            uint_value = pcar_t->pir;
            return (u_char *)(&uint_value);

        case hhrQosCarPbs:
            *var_len = sizeof(uint32_t);
            uint_value = pcar_t->pbs;
            return (u_char *)(&uint_value);

        case hhrQosCarColorMode:
            *var_len = sizeof(uint32_t);
            int_value_self = pcar_t->color_blind;
            return (u_char *)(&int_value_self);

        case hhrQosCarGreenAction:
            *var_len = sizeof(uint32_t);
            int_value_self = pcar_t->green_act.drop;
            return (u_char *)(&int_value_self);

        case hhrQosCarYellowAction:
            *var_len = sizeof(uint32_t);
            int_value_self = pcar_t->yellow_act.drop;
            return (u_char *)(&int_value_self);

        case hhrQosCarRedAction:
            *var_len = sizeof(uint32_t);
            int_value_self = pcar_t->red_act.drop;
            return (u_char *)(&int_value_self);

        case hhrQosCarGreenRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pcar_t->green_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->green_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarGreenRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pcar_t->green_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->green_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarGreenQueue:
            *var_len = sizeof(uint32_t);

            if (pcar_t->green_act.new_queue & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->green_act.new_queue & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarYellowRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pcar_t->yellow_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->yellow_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarYellowRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pcar_t->yellow_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->yellow_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarYellowQueue:
            *var_len = sizeof(uint32_t);

            if (pcar_t->yellow_act.new_queue & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->yellow_act.new_queue & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarRedRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pcar_t->red_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->red_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarRedRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pcar_t->red_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->red_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosCarRedQueue:
            *var_len = sizeof(uint32_t);

            if (pcar_t->red_act.new_queue & CAR_ACTION_CONFIGED)
            {
                uint_value = pcar_t->red_act.new_queue & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

u_char *hhrQosCarApplyIfTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    struct qos_if_doubleindex *pqos_if = NULL;
    struct qos_if_doubleindex index;
    int ret = 0;
    int if_index = 0;
    int dir_index = 0;
    int t_index = 0;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &dir_index, exact);

    index.qos_car.ifindex = if_index;

    index.direction = dir_index;

    if (ret < 0)
    {
        return NULL;
    }


    if (hhrQosCarApplyIfTable_cache == NULL)
    {
        hhrQosCarApplyIfTable_cache = snmp_cache_init(sizeof(struct qos_if_doubleindex),
                                                      hhrQosCarApplyIfTable_data_form_ipc,
                                                      hhrQosCarApply_Rate_Limit_data_lookup);

        if (NULL == hhrQosCarApplyIfTable_cache)
        {
            return (NULL);
        }
    }

    pqos_if = snmp_cache_get_data_by_index(hhrQosCarApplyIfTable_cache, exact, &index);


    if (NULL == pqos_if)
    {
        return NULL;
    }

    if (QOS_DIR_INGRESS != pqos_if->direction && QOS_DIR_EGRESS != pqos_if->direction)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqos_if->qos_car.ifindex, pqos_if->direction);
    }

    switch (vp->magic)
    {
        case hhrQosCarApplyIfDescr:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ifm_get_name_by_ifindex(pqos_if->qos_car.ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrQosCarApplyIfCarProfileIndex:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if->qos_car.car_id[pqos_if->direction - 1];
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

u_char *hhrQosPortRateLimitTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{

    struct qos_if_doubleindex *pqos_if = NULL;
    struct qos_if_doubleindex index;
    int ret = 0;
    int if_index = 0;
    int dir_index = 0;
    int t_index = 0;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &dir_index, exact);

    index.qos_car.ifindex = if_index;

    index.direction = dir_index;

    if (ret < 0)
    {
        return NULL;
    }

    if (hhrQosPortRateLimitTable_cache == NULL)
    {
        hhrQosPortRateLimitTable_cache = snmp_cache_init(sizeof(struct qos_if_doubleindex),
                                                         hhrQosCar_Rate_Limit_data_form_ipc,
                                                         hhrQosCarApply_Rate_Limit_data_lookup);

        if (NULL == hhrQosPortRateLimitTable_cache)
        {
            return (NULL);
        }
    }

    pqos_if = snmp_cache_get_data_by_index(hhrQosPortRateLimitTable_cache , exact, &index);

    if (NULL == pqos_if)
    {
        return NULL;
    }

    if (QOS_DIR_INGRESS != pqos_if->direction && QOS_DIR_EGRESS != pqos_if->direction)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqos_if->qos_car.ifindex, pqos_if->direction);
    }

    switch (vp->magic)
    {
        case hhrQosPortRateLimitIfDescr:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ifm_get_name_by_ifindex(pqos_if->qos_car.ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrQosPortRateLimitCIR:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].cir;
            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitCBS:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].cbs;
            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitPIR:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].pir;
            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitPBS:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].pbs;
            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitColorMode:
            *var_len = sizeof(int);
            int_value_self = pqos_if->qos_car.car[pqos_if->direction - 1].color_blind;
            return (u_char *)(&int_value_self);

        case hhrQosPortRateLimitGreenAction:
            *var_len = sizeof(int);
            int_value_self = pqos_if->qos_car.car[pqos_if->direction - 1].green_act.drop;
            return (u_char *)(&int_value_self);

        case hhQosPortRateLimitGreenRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitGreenRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitGreenQueue:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_queue & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].green_act.new_queue & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitYellowAction:
            *var_len = sizeof(int);
            int_value_self = pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.drop;
            return (u_char *)(&int_value_self);

        case hhrQosPortRateLimitYellowRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitYellowRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitYellowQueue:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_queue & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].yellow_act.new_queue & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitRedAction:
            *var_len = sizeof(uint32_t);
            int_value_self = pqos_if->qos_car.car[pqos_if->direction - 1].red_act.drop;
            return (u_char *)(&int_value_self);

        case hhrQosPortRateLimitRedRemarkCos:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_cos & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_cos & CAR_ACTION_COS_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitRedRemarkDscp:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_dscp & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_dscp & CAR_ACTION_DSCP_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosPortRateLimitRedQueue:
            *var_len = sizeof(uint32_t);

            if (pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_queue  & CAR_ACTION_CONFIGED)
            {
                uint_value = pqos_if->qos_car.car[pqos_if->direction - 1].red_act.new_queue  & CAR_ACTION_QUEUE_MASK;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}


uchar *hhrQosAclBasicTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{

    struct  rule_node *prule_node = NULL;
    struct rule_node index;
    int ret = 0;
    int gid_index = 0;
    int rule_index = 0;
    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &gid_index, &rule_index , exact);
    index.acl_num = gid_index;
    index.rule.ip_acl.ruleid = rule_index;
    index.type = ACL_TYPE_IP;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosAclBasicTable_cache)
    {
        hhrQosAclBasicTable_cache = snmp_cache_init(sizeof(struct rule_node),
                                                    hhrQosAcl_ip_rule_data_form_ipc,
                                                    hhrQosAcl_ip_rule_node_lookup);

        if (NULL == hhrQosAclBasicTable_cache)
        {
            return (NULL);
        }
    }


    prule_node = snmp_cache_get_data_by_index(hhrQosAclBasicTable_cache, exact, &index);

    if (NULL == prule_node)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, prule_node->acl_num, prule_node->rule.ip_acl.ruleid);
    }

    switch (vp->magic)
    {
        case hhrQosAclBasicIpv4SrcOrDest:
            *var_len = sizeof(uint32_t);
            int_value_self = prule_node->rule.ip_acl.ip_flag + 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclBasicIpv4AnyIp:
            *var_len = sizeof(uint32_t);

            if (0 == prule_node->rule.ip_acl.ip_masklen)
            {
                int_value_self = 2;
            }
            else
            {
                int_value_self = 1;
            }

            return (u_char *)(&int_value_self);

        case hhrQosAclBasicIpv4Address:
            *var_len = sizeof(uint32_t);

            if (1 == prule_node->rule.ip_acl.ip_mask)
            {
                ip_value = htonl(prule_node->rule.ip_acl.ip);
            }
            else
            {
                ip_value = 0;
            }

            return (u_char *)(&ip_value);

        case hhrQosAclBasicIpv4AddressMask:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(0xffffffff << (32 - prule_node->rule.ip_acl.ip_masklen));
            return (u_char *)(&ip_value);

        case hhrQosAclBasicIpv4L3vpn:
            *var_len = sizeof(uint32_t);

            if (1 == prule_node->rule.ip_acl.vpn_mask)
            {
                uint_value = prule_node->rule.ip_acl.vpnid;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

uchar *hhrQosAclAdvanceTable_get(struct variable *vp,
                                 oid *name,
                                 size_t *length,
                                 int exact, size_t *var_len, WriteMethod **write_method)
{

    struct  rule_node *prule_node = NULL;
    struct rule_node index;
    int ret = 0;
    int gid_index = 0;
    int rule_index = 0;
    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &gid_index, &rule_index , exact);
    index.acl_num = gid_index;
    index.rule.ipex_acl.ruleid = rule_index;
    index.type = ACL_TYPE_IP_EXTEND;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosAclAdvanceTable_cache)
    {
        hhrQosAclAdvanceTable_cache = snmp_cache_init(sizeof(struct rule_node),
                                                      hhrQosAcl_ipex_rule_data_form_ipc,
                                                      hhrQosAcl_ipex_rule_node_lookup);

        if (NULL == hhrQosAclAdvanceTable_cache)
        {
            return (NULL);
        }
    }

    prule_node = snmp_cache_get_data_by_index(hhrQosAclAdvanceTable_cache, exact, &index);

    if (NULL == prule_node)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, prule_node->acl_num, prule_node->rule.ipex_acl.ruleid);
    }

    switch (vp->magic)
    {
        case hhrQosAclAdvanceIpv4ProtocolAny:
            *var_len = sizeof(uint32_t);
            int_value_self = prule_node->rule.ipex_acl.proto_mask ? 1 : 2;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Protocol:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.proto_mask)
            {
                uint_value = prule_node->rule.ipex_acl.protocol;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4Sport:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.sport_mask)
            {
                uint_value = prule_node->rule.ipex_acl.sport;
            }
            else
            {
                uint_value = 0;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4SipAny:
            *var_len = sizeof(uint32_t);

            if (0 == prule_node->rule.ipex_acl.sip_mask)
            {
                int_value_self = 0;
            }
            else if (1 == prule_node->rule.ipex_acl.sip_mask && 0 == prule_node->rule.ipex_acl.sip_masklen)
            {
                int_value_self = 2;
            }
            else
            {
                int_value_self = 1;
            }

            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Sip:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.sip_mask && 0 != prule_node->rule.ipex_acl.sip_masklen)
            {
                ip_value = htonl(prule_node->rule.ipex_acl.sip);
            }
            else
            {
                ip_value = htonl(0);
            }

            return (u_char *)(&ip_value);

        case hhrQosAclAdvanceIpv4SipMask:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(0xffffffff << (32 - prule_node->rule.ipex_acl.sip_masklen));
            return (u_char *)(&ip_value);

        case hhrQosAclAdvanceIpv4Dport:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.dport_mask)
            {
                uint_value = prule_node->rule.ipex_acl.dport;
            }
            else
            {
                uint_value = 0;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4DipAny:
            *var_len = sizeof(int);

            if (0 == prule_node->rule.ipex_acl.dip_mask)
            {
                int_value_self = 0;
            }
            else if (1 == prule_node->rule.ipex_acl.dip_mask && 0 == prule_node->rule.ipex_acl.dip_masklen)
            {
                int_value_self = 2;
            }
            else
            {
                int_value_self = 1;
            }

            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Dip:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.dip_mask && 0 != prule_node->rule.ipex_acl.dip_masklen)
            {
                ip_value = htonl(prule_node->rule.ipex_acl.dip);
            }
            else
            {
                ip_value = htonl(0);
            }

            return (u_char *)(&ip_value);

        case hhrQosAclAdvanceIpv4DipMask:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(0xffffffff << (32 - prule_node->rule.ipex_acl.dip_masklen));
            return (u_char *)(&ip_value);

        case hhrQosAclAdvanceIpv4L3vpn:
            *var_len = sizeof(uint32_t);

            if (1 == prule_node->rule.ipex_acl.vpn_mask)
            {
                uint_value = prule_node->rule.ipex_acl.vpnid;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4Dscp:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.dscp >= 64)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = prule_node->rule.ipex_acl.dscp;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4Ttl:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.ipex_acl.ttl_mask)
            {
                uint_value = prule_node->rule.ipex_acl.ttl;
            }
            else
            {
                uint_value = 0;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclAdvanceIpv4Syn:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x02) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Fin:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x01) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Ack:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x10) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Rst:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x4) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Psh:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x8) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Urg:
            *var_len = sizeof(int);
            int_value_self = ((prule_node->rule.ipex_acl.tcp_type) & 0x20) ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclAdvanceIpv4Tos:
            *var_len = sizeof(int);

            if (prule_node->rule.ipex_acl.tos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = prule_node->rule.ipex_acl.tos;
            }

            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

uchar *hhrQosAclMacTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  rule_node *prule_node = NULL;
    struct rule_node index;
    int ret = 0;
    int gid_index = 0;
    int rule_index = 0;
    /* validate the index */
    ret = ipran_snmp_intx2_index_get(vp, name, length, &gid_index, &rule_index , exact);
    index.acl_num = gid_index;
    index.rule.ipex_acl.ruleid = rule_index;
    index.type = ACL_TYPE_MAC;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosAclMacTable_cache)
    {
        hhrQosAclMacTable_cache = snmp_cache_init(sizeof(struct rule_node),
                                                  hhrQosAcl_mac_rule_data_form_ipc,
                                                  hhrQosAcl_mac_rule_node_lookup);

        if (NULL == hhrQosAclMacTable_cache)
        {
            return (NULL);
        }
    }

    prule_node = snmp_cache_get_data_by_index(hhrQosAclMacTable_cache, exact, &index);

    if (NULL == prule_node)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, prule_node->acl_num, prule_node->rule.mac_acl.ruleid);
    }

    switch (vp->magic)
    {
        case hhrQosAclMacEthTypeAny:
            *var_len = sizeof(int);
            int_value_self = prule_node->rule.mac_acl.ethtype_mask;
            return (u_char *)(&int_value_self);

        case hhrQosAclMacEthType:
            memset(str_value, 0, STRING_LEN);

            if (prule_node->rule.mac_acl.ethtype_mask)
            {
                sprintf((char *) str_value, "0x%04x", prule_node->rule.mac_acl.ethtype);
            }

            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrQosAclMacSmac:
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, prule_node->rule.mac_acl.smac, sizeof(uchar) * 6);
            return mac_value;

        case hhrQosAclMacDmac:
            *var_len = sizeof(uchar) * 6;
            memcpy(mac_value, prule_node->rule.mac_acl.dmac, sizeof(uchar) * 6);
            return mac_value;

        case hhrQosAclMacUntag:
            *var_len = sizeof(int);
            int_value_self = prule_node->rule.mac_acl.untag_mask ? 1 : 2;
            return (u_char *)(&int_value_self);

        case hhrQosAclMacVlan:
            *var_len = sizeof(uint32_t);
            uint_value = prule_node->rule.mac_acl.vlan;
            return (u_char *)(&uint_value);

        case hhrQosAclMacCos:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.mac_acl.cos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = prule_node->rule.mac_acl.cos;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclMacCvlan:
            *var_len = sizeof(uint32_t);
            uint_value = prule_node->rule.mac_acl.cvlan;
            return (u_char *)(&uint_value);

        case hhrQosAclMacCvlanCos:
            *var_len = sizeof(uint32_t);

            if (prule_node->rule.mac_acl.cvlan_cos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = prule_node->rule.mac_acl.cvlan_cos;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclMacOffset:
            *var_len = sizeof(uint32_t);
            uint_value = prule_node->rule.mac_acl.offset_len;
            return (u_char *)(&uint_value);

        case hhrQosAclMacOffsetInfo:
            memset(str_value, 0, STRING_LEN);
            sprintf((char *) str_value, "%d", prule_node->rule.mac_acl.offset_info);
            *var_len = strlen((char *) str_value);
            return str_value;

        case hhrQosAclMacSmacConfig:
            *var_len = sizeof(uint32_t);
            int_value_self = prule_node->rule.mac_acl.smac_mask ? 1 : 2;
            return (u_char *)(&int_value_self);

        case hhrQosAclMacDmacConfig:
            *var_len = sizeof(uint32_t);
            int_value_self = prule_node->rule.mac_acl.dmac_mask ? 1 : 2;
            return (u_char *)(&int_value_self);

        default:
            return NULL;
    }
}


uchar *hhrQosAclPolicyTable_get(struct variable *vp,
                                oid *name,
                                size_t *length,
                                int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  qos_policy *pqos_policy = NULL;
    struct qos_policy index;
    int ret = 0;
    uint32_t if_index = 0;
    uint32_t type_index = 0;
    uint32_t acl_index = 0;
    uint32_t dir_index = 0;
    uint32_t tmp_direct = 0;
    /* validate the index */

    ret = ipran_snmp_intx4_index_get(vp, name, length, &if_index, &type_index, &acl_index, &dir_index, exact);
    index.ifindex = if_index;
    index.type = type_index;

    if (1 == dir_index)
    {
        index.direct = QOS_DIR_EGRESS;
    }
    else if (2 == dir_index)
    {
        index.direct = QOS_DIR_INGRESS;
    }
    else
    {
        index.direct = QOS_DIR_INVALID;
    }

    index.acl_num = acl_index;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosAclPolicyTable_cahe)
    {
        hhrQosAclPolicyTable_cahe = snmp_cache_init(sizeof(struct qos_policy),
                                                    hhrQosAclPolicyTable_get_data_from_ipc,
                                                    hhrQosAclPolicyTable_node_lookup);

        if (NULL == hhrQosAclPolicyTable_cahe)
        {
            return (NULL);
        }
    }

    pqos_policy = snmp_cache_get_data_by_index(hhrQosAclPolicyTable_cahe, exact, &index);

    if (NULL == pqos_policy)
    {
        return NULL;
    }

    if (QOS_DIR_INGRESS != pqos_policy->direct && QOS_DIR_EGRESS != pqos_policy->direct)
    {
        return NULL;
    }

    if (!exact)
    {
        if (QOS_DIR_EGRESS == pqos_policy->direct)
        {
            tmp_direct = 1;
        }
        else
        {
            tmp_direct = 2;
        }

        ipran_snmp_intx4_index_set(vp, name, length, pqos_policy->ifindex, pqos_policy->type, pqos_policy->acl_num, tmp_direct);
    }

    switch (vp->magic)
    {
        case hhrQosAclPolicyFilter:
            *var_len = sizeof(int);
            int_value_self = pqos_policy->action.filter_act ? 2 : 1;
            return (u_char *)(&int_value_self);

        case hhrQosAclPolicyIfDesc:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (0 != pqos_policy->ifindex)
            {
                ifm_get_name_by_ifindex(pqos_policy->ifindex, str_value);
            }

            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrQosAclPolicyqosMappingQueue:
            *var_len = sizeof(uint32_t);

            if (7 >= pqos_policy->action.queue_id)
            {
                uint_value = pqos_policy->action.queue_id;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (u_char *)(&uint_value);

        case hhrQosAclPolicyCar:
            *var_len = sizeof(int);
            uint_value = pqos_policy->action.car_id;
            return (u_char *)(&uint_value);

        case hhrQosAclPolicyApplyIfDesc:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);

            if (POLICY_TYPE_MIRROR == pqos_policy->type)
            {
                if (0 != pqos_policy->action.mirror_if)
                {
                    ifm_get_name_by_ifindex(pqos_policy->action.mirror_if, str_value);
                }
            }

            else if (POLICY_TYPE_REDIRECT == pqos_policy->type)
            {
                if (0 != pqos_policy->action.redirect_if)
                {
                    ifm_get_name_by_ifindex(pqos_policy->action.redirect_if, str_value);
                }
            }

            *var_len = strlen((char *) str_value);
            return (str_value);

        default:
            return NULL;
    }
}

int hhrQosWREDTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct wred_profile *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    struct wred_profile *pwred_profile = NULL;

    pmsg = qos_com_get_wred_profile_bulk(index->id, MODULE_ID_SNMPD, &data_num);
    if (NULL == pmsg)
    {
    	return FALSE;
    }

    pwred_profile = (struct wred_profile *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pwred_profile)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        ret = FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrQosWREDTable_cache, pwred_profile, sizeof(struct wred_profile));
            pwred_profile++;
        }

		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct wred_profile *hhrQosWREDTable_node_lookup(struct ipran_snmp_data_cache *cache ,
                                                 int exact,
                                                 const struct wred_profile  *index_input)
{
    struct listnode     *node, *nnode;
    struct wred_profile *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->id)
        {
            return cache->data_list->head->data;
        }

        if (data1_find->id == index_input->id)
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

uchar *hhrQosWREDTable_get(struct variable *vp,
                           oid *name,
                           size_t *length,
                           int exact, size_t *var_len, WriteMethod **write_method)
{
    struct wred_profile *pwred_profile = NULL;
    struct wred_profile index;
    int ret = 0;
    int wred_id = 0;

    ret = ipran_snmp_int_index_get(vp, name, length, &wred_id, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.id = wred_id;

    if (NULL == hhrQosWREDTable_cache)
    {
        hhrQosWREDTable_cache = snmp_cache_init(sizeof(struct wred_profile),
                                                hhrQosWREDTable_data_form_ipc,
                                                hhrQosWREDTable_node_lookup);

        if (NULL == hhrQosWREDTable_cache)
        {
            return (NULL);
        }
    }

    pwred_profile = snmp_cache_get_data_by_index(hhrQosWREDTable_cache, exact, &index);

    if (NULL == pwred_profile)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pwred_profile->id);
    }

    switch (vp->magic)
    {
        case hhrQosWredGreenPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][0].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredGreenPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][0].drop_high;
            return (u_char *)(&uint_value);

        case hhrQosWredGreenPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][0].drop_ratio;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][0].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][0].drop_high;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][0].drop_ratio;
            return (u_char *)(&uint_value);

        case hhrQosWredRedPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][0].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredRedPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][0].drop_high;;
            return (u_char *)(&uint_value);

        case hhrQosWredRedPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][0].drop_ratio;
            return (u_char *)(&uint_value);

        case  hhrQosWredGreenNonTCPPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][1].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredGreenNonTCPPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][1].drop_high;
            return (u_char *)(&uint_value);

        case hhrQosWredGreenNonTCPPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[0][1].drop_ratio;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowNonTCPPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][1].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowNonTCPPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][1].drop_high;
            return (u_char *)(&uint_value);

        case hhrQosWredYellowNonTCPPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[1][1].drop_ratio;
            return (u_char *)(&uint_value);

        case hhrQosWredRedNonTCPPacketDropLowThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][1].drop_low;
            return (u_char *)(&uint_value);

        case hhrQosWredRedNonTCPPacketDropHighThreshold:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][1].drop_high;;
            return (u_char *)(&uint_value);

        case hhrQosWredRedNonTCPPacketMaxDropRatio:
            *var_len = sizeof(uint32_t);
            uint_value = pwred_profile->wred[2][1].drop_ratio;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}
uchar *hhrHQosQueueProfileTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{
    struct queue_profiledoubleindex *pqueue_profiledoubleindex = NULL;
    struct queue_profiledoubleindex index;
    int profile_idindex = 0;
    int priority_idindex = 0;
    int ret = 0;

    ret = ipran_snmp_intx2_index_get(vp, name, length, &profile_idindex, &priority_idindex, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.pqueue_profile.id = profile_idindex;
    index.queuepriority = priority_idindex;

    if (NULL == hhrHQosQueueProfileTable_cache)
    {
        hhrHQosQueueProfileTable_cache = snmp_cache_init(sizeof(struct queue_profiledoubleindex),
                                                         hhrHQosQueueProfileTable_data_form_ipc,
                                                         hhrHQosQueueProfileTable_node_lookup);

        if (NULL == hhrHQosQueueProfileTable_cache)
        {
            return (NULL);
        }
    }

    pqueue_profiledoubleindex = snmp_cache_get_data_by_index(hhrHQosQueueProfileTable_cache, exact, &index);

    if (NULL == pqueue_profiledoubleindex)
    {
        return NULL;
    }

    if ((pqueue_profiledoubleindex->queuepriority < 0) || (pqueue_profiledoubleindex->queuepriority >= 8))
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pqueue_profiledoubleindex->pqueue_profile.id, pqueue_profiledoubleindex->queuepriority);
    }

    switch (vp->magic)
    {
        case hhrHQosQueueWeight:
            *var_len = sizeof(uint32_t);
            uint_value = pqueue_profiledoubleindex->pqueue_profile.queue[pqueue_profiledoubleindex->queuepriority].weight;
            return (u_char *)(&uint_value);

        case hhrHQosQueueCir:
            *var_len = sizeof(uint32_t);
            uint_value = pqueue_profiledoubleindex->pqueue_profile.queue[pqueue_profiledoubleindex->queuepriority].cir;
            return (u_char *)(&uint_value);

        case hhrHQosQueueWredId:
            *var_len = sizeof(uint32_t);
            uint_value = pqueue_profiledoubleindex->pqueue_profile.queue[pqueue_profiledoubleindex->queuepriority].wred_id;
            return (u_char *)(&uint_value);

        case hhrHQosQueueScheduler:
            *var_len = sizeof(uint32_t);
            int_value_self = pqueue_profiledoubleindex->pqueue_profile.scheduler;

            if (2 == int_value_self)
            {
                int_value_self = 3;
            }
            else if (3 == int_value_self)
            {
                int_value_self = 2;
            }

            return (u_char *)(&int_value_self);

        case hhrHQosQueuePir:
            *var_len = sizeof(uint32_t);
            uint_value = pqueue_profiledoubleindex->pqueue_profile.queue[pqueue_profiledoubleindex->queuepriority].pir;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

uchar *hhrHQosIfApplyTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct qos_if_snmp *pqos_if_snmp = NULL;
    struct qos_if_snmp index;
    int if_index = 0;
    int ret = 0;
    ret  = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ifindex = if_index;

    if (NULL == hhrHQosIfApplyTable_cache)
    {
        hhrHQosIfApplyTable_cache = snmp_cache_init(sizeof(struct qos_if_snmp),
                                                    hhrHQosIfApplyTable_data_form_ipc,
                                                    hhrHQosIfApplyTable_node_lookup);

        if (NULL == hhrHQosIfApplyTable_cache)
        {
            return (NULL);
        }
    }

    pqos_if_snmp = snmp_cache_get_data_by_index(hhrHQosIfApplyTable_cache, exact, &index);

    if (NULL == pqos_if_snmp)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pqos_if_snmp->ifindex);
    }

    switch (vp->magic)
    {
        case hhrHQosIfApplyIfDescr:
            memset(str_value, 0, sizeof(uchar) * STRING_LEN);
            ifm_get_name_by_ifindex(pqos_if_snmp->ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrHQosIfApplyPIR:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if_snmp->hqos.cir;
            return (u_char *)(&uint_value);

        case hhrHQosIfApplyPBS:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if_snmp->hqos.pir;
            return (u_char *)(&uint_value);

        case hhrHQosIfApplyQueueProfileId:
            *var_len = sizeof(uint32_t);
            uint_value = pqos_if_snmp->hqos.hqos_id;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

int hhrHQosPWApplyTable_data_form_ipc(struct ipran_snmp_data_cache *cache , struct hqos_pw *index)
{
    int data_num = 0;
    int ret = 0;
    struct hqos_pw *phqos_pw = NULL;
    struct hqos_pw *pfree = NULL;

    phqos_pw = mpls_com_get_pw_hqos_bulk(index->name, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == phqos_pw)
    {
        return FALSE;
    }
    else
    {
        pfree = phqos_pw;
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrHQosPWApplyTable_cache, phqos_pw, sizeof(struct hqos_pw));
            phqos_pw++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct hqos_pw *hhrHQosPWApplyTable_lookup(struct ipran_snmp_data_cache *cache ,
                                           int exact,
                                           const struct  hqos_pw  *index_input)
{
    struct listnode *node, *nnode;
    struct hqos_pw  *data1_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->name[0])
        {
            return cache->data_list->head->data;
        }

        if (0 == strcmp(data1_find->name, index_input->name))
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

uchar *hhrHQosPWApplyTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    u_char sIndex[NAME_STRING_LEN] = {0};
    u_char sNext_index[NAME_STRING_LEN] = {0};
    struct hqos_pw *pqos_pw = NULL;
    struct hqos_pw index;
    int ret = 0;
    int key_len = 0;

    ret = ipran_snmp_octstring_index_get(vp, name, length, sIndex, NAME_STRING_LEN, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (0x00 == sIndex[0] && 1 == exact)
    {
        return NULL;
    }

    key_len = NAME_STRING_LEN;
    memcpy(&(index.name), sIndex, key_len);

    if (NULL == hhrHQosPWApplyTable_cache)
    {
        hhrHQosPWApplyTable_cache = snmp_cache_init(sizeof(struct hqos_pw),
                                                    hhrHQosPWApplyTable_data_form_ipc,
                                                    hhrHQosPWApplyTable_lookup);

        if (NULL == hhrHQosPWApplyTable_cache)
        {
            return (NULL);
        }
    }

    pqos_pw = (struct hqos_pw *)snmp_cache_get_data_by_index(hhrHQosPWApplyTable_cache, exact, &index);

    if (NULL == pqos_pw)
    {
        return NULL;
    }

    if (!exact)
    {
        snprintf(sNext_index, NAME_STRING_LEN, "%s", pqos_pw->name);
        ipran_snmp_octstring_index_set(vp, name, length, sNext_index, strlen(sNext_index));
    }

    switch (vp->magic)
    {
        case hhrHQosPWApplyCIR:
            *var_len = sizeof(int);
            uint_value = pqos_pw->hqos.cir;
            return (u_char *)(&uint_value);

        case hhrHQosPWApplyPIR:
            *var_len = sizeof(int);
            uint_value = pqos_pw->hqos.pir;
            return (u_char *)(&uint_value);

        case hhrHQosPWApplyQueueProfileId:
            *var_len = sizeof(int);
            uint_value = pqos_pw->hqos.hqos_id;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

int hhrHQosTunnelApplyTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct hqos_tunnel *index)
{
    int data_num = 0;
    int ret = 0;
    struct hqos_tunnel *phqos_tunnel = NULL;
    struct hqos_tunnel *pfree = NULL;

    phqos_tunnel = mpls_com_get_tunnel_hqos_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == phqos_tunnel)
    {
        return FALSE;
    }
    else
    {
        pfree = phqos_tunnel;
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrHQosTunnelApplyTable_cache, phqos_tunnel, sizeof(struct hqos_tunnel));
            phqos_tunnel++;
        }
        mem_share_free_bydata(pfree, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct hqos_tunnel *hhrHQosTunnelApplyTable_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact,
                                                   const struct  hqos_tunnel  *index_input)
{
    struct listnode     *node, *nnode;
    struct hqos_tunnel  *data1_find = NULL;

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
                    return node->next->data;
                }
            }
        }
    }

    return NULL;
}

uchar *hhrHQosTunnelApplyTable_get(struct variable *vp,
                                   oid *name,
                                   size_t *length,
                                   int exact, size_t *var_len, WriteMethod **write_method)
{
    struct hqos_tunnel *pqos_tunnel = NULL;
    struct hqos_tunnel index;
    int if_index = 0;
    int ret = 0;

    ret  = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ifindex = if_index;

    if (NULL == hhrHQosTunnelApplyTable_cache)
    {
        hhrHQosTunnelApplyTable_cache = snmp_cache_init(sizeof(struct hqos_tunnel),
                                                        hhrHQosTunnelApplyTable_data_form_ipc,
                                                        hhrHQosTunnelApplyTable_lookup);

        if (NULL == hhrHQosTunnelApplyTable_cache)
        {
            return (NULL);
        }
    }

    pqos_tunnel = snmp_cache_get_data_by_index(hhrHQosTunnelApplyTable_cache, exact, &index);

    if (NULL == pqos_tunnel)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pqos_tunnel->ifindex);
    }

    switch (vp->magic)
    {
        case hhrHQosTunnelApplyTunnelName:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pqos_tunnel->ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrHQosTunnelApplyCIR:
            *var_len = sizeof(int);
            uint_value = pqos_tunnel->hqos.cir;
            return (u_char *)(&uint_value);

        case hhrHQosTunnelApplyPIR:
            *var_len = sizeof(int);
            uint_value = pqos_tunnel->hqos.pir;
            return (u_char *)(&uint_value);

        case hhrHQosTunnelApplyQueueProfileId:
            *var_len = sizeof(int);
            uint_value = pqos_tunnel->hqos.hqos_id;
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

uchar *hhrQosPWRateLimitTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{

    struct l2vc_entry *pl2vc_entry = NULL;
    struct doubleindex_l2vc_entry *pdoubleindex_l2vc_entry = NULL;
    struct doubleindex_l2vc_entry index;
    u_char SIndex[NAME_STRING_LEN] = {0};
    int dir_index = 0;
    int ret = 0;
    int max_len = sizeof(SIndex);
    int stringlen = sizeof(SIndex);

    ret = ipran_snmp_str_int_index_get(vp, name, length, SIndex, max_len, &stringlen, &dir_index, exact);
    index.dir = dir_index;
    memcpy(&(index.fl2vc_entry.name), SIndex, sizeof(SIndex));

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosPWRateLimitTable_cache)
    {
        hhrQosPWRateLimitTable_cache = snmp_cache_init(sizeof(struct doubleindex_l2vc_entry),
                                                       hhrQosPWRateLimitTable_data_form_ipc,
                                                       hhrQosPWRateLimitTable_node_lookup);

        if (NULL == hhrQosPWRateLimitTable_cache)
        {
            return (NULL);
        }
    }

    pdoubleindex_l2vc_entry = snmp_cache_get_data_by_index(hhrQosPWRateLimitTable_cache, exact, &index);


    if (NULL == pdoubleindex_l2vc_entry)
    {
        return NULL;
    }

    if (1 != pdoubleindex_l2vc_entry->dir && 2 != pdoubleindex_l2vc_entry->dir)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_str_int_index_set(vp, name, length, pdoubleindex_l2vc_entry->fl2vc_entry.name, strlen(pdoubleindex_l2vc_entry->fl2vc_entry.name), pdoubleindex_l2vc_entry->dir);
    }

    switch (vp->magic)
    {
        case hhrQosPWRateLimitCIR:
            *var_len = sizeof(uint32_t);
            uint_value = pdoubleindex_l2vc_entry->fl2vc_entry.car_cir[(pdoubleindex_l2vc_entry->dir) - 1];
            return (u_char *)(&uint_value);

        case hhrQosPWRateLimitPIR:
            *var_len = sizeof(uint32_t);
            uint_value = pdoubleindex_l2vc_entry->fl2vc_entry.car_pir[(pdoubleindex_l2vc_entry->dir) - 1];
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}

int hhrPortMirrorConfigTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qos_if_mirror *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct qos_if_mirror *pdata;
    pmsg = qos_com_get_mirror_port_bulk(index->ifindex, MODULE_ID_SNMPD, &data_num);
	if (NULL == pmsg)
	{
		return FALSE;
	}

	pdata = (struct qos_if_mirror *)pmsg->msg_data;
	
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {

        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrPortMirrorConfigTable_cache, pdata, sizeof(struct qos_if_mirror));
            pdata ++;
        }
		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}


struct qos_if_mirror *hhrPortMirrorConfigTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                      int exact,
                                                      const struct qos_if_mirror   *index_input)
{
    struct listnode  *node;

    struct qos_if_mirror  *data1_find = NULL ;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data1_find))
    {
        if (NULL == data1_find || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return NULL;
        }

        if (0 == index_input->ifindex)
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

    return NULL;
}


uchar *hhrPortMirrorConfigTable_get(struct variable *vp,
                                    oid *name,
                                    size_t *length,
                                    int exact, size_t *var_len, WriteMethod **write_method)
{

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'start hhrPortMirrorConfigTable_get\n", __FILE__, __LINE__, __func__);
    struct qos_if_mirror *pdata;
    struct qos_if_mirror data_index;
    int if_index;
    int ret = 0;
    ret = ipran_snmp_int_index_get(vp, name, length, &if_index, exact);

    if (0 > ret)
    {
        return NULL;
    }

    data_index.ifindex = if_index;

    if (NULL == hhrPortMirrorConfigTable_cache)
    {
        hhrPortMirrorConfigTable_cache = snmp_cache_init(sizeof(struct qos_if_mirror),
                                                         hhrPortMirrorConfigTable_data_form_ipc,
                                                         hhrPortMirrorConfigTable_lookup);

        if (NULL == hhrPortMirrorConfigTable_cache)
        {
            return (NULL);
        }
    }

    pdata = snmp_cache_get_data_by_index(hhrPortMirrorConfigTable_cache, exact, &data_index);

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
        case hhrMirrorIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrMirrorType:

            if (QOS_INFO_MIRROR_TO_PORT == pdata->type)
            {
                uint_value = 2;
            }
            else
            {
                uint_value = 1;
            }

            *var_len = sizeof(int);
            return (u_char *)(&uint_value);

        case hhrMirrorToPortDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->mirror_if, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrMirrorToGroup:
            uint_value = pdata->mirror_group;
            *var_len = sizeof(int);
            return (u_char *)(&uint_value);

        case hhrMirrorDirect:

            if (1 ==  pdata->direct)
            {
                uint_value = 3;
            }
            else if (2 == pdata->direct)
            {
                uint_value = 2;
            }
            else if (3 == pdata->direct)
            {
                uint_value = 1;
            }
            else
            {
                uint_value = 0;
            }

            *var_len = sizeof(int);
            return (u_char *)(&uint_value);

        default:
            return NULL;
    }
}


int hhrMirrorGroupTable_data_form_ipc(struct ipran_snmp_data_cache *cache, struct qos_mirror_snmp *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    int data_num = 0;
    int ret = 0;
    int i = 0;
    struct qos_mirror_snmp *pdata;
    pmsg = qos_com_get_mirror_group_bulk(index->ifindex, index->id, MODULE_ID_SNMPD, &data_num);
    if (NULL == pmsg)
	{
		return FALSE;
	}

	pdata = (struct qos_mirror_snmp *)pmsg->msg_data;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]: '%s'data_num = %d\n", __FILE__, __LINE__, __func__, data_num);

    if (0 == data_num || NULL == pdata)
    {
    	mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(hhrMirrorGroupTable_cache, pdata, sizeof(struct qos_mirror_snmp));
            pdata ++;
        }
		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return TRUE;
    }
}

struct qos_mirror_snmp *hhrMirrorGroupTable_lookup(struct ipran_snmp_data_cache *cache ,
                                                   int exact,
                                                   const struct qos_mirror_snmp   *index_input)
{
    struct listnode       *node;
    struct qos_mirror_snmp   *data1_find = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, data1_find))
    {
        if (NULL == data1_find || NULL == node)
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return NULL;
        }

        if (0 == index_input->ifindex && 0 == index_input->id)
        {
            return cache->data_list->head->data ;
        }

        if (data1_find->id == index_input->id && data1_find->ifindex == index_input->ifindex)
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


uchar *hhrMirrorGroupTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    struct qos_mirror_snmp *pdata;
    uint32_t groupid_index = 0;
    uint32_t if_index = 0;
    struct qos_mirror_snmp data_index;
    int ret = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &groupid_index, exact);

    if (0 > ret)
    {
        return NULL;
    }

    if (NULL == hhrMirrorGroupTable_cache)
    {
        hhrMirrorGroupTable_cache = snmp_cache_init(sizeof(struct qos_mirror_snmp),
                                                    hhrMirrorGroupTable_data_form_ipc,
                                                    hhrMirrorGroupTable_lookup);

        if (NULL == hhrMirrorGroupTable_cache)
        {
            return (NULL);
        }
    }


    data_index.ifindex = if_index;
    data_index.id = groupid_index;
    pdata = snmp_cache_get_data_by_index(hhrMirrorGroupTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->ifindex, pdata->id);
    }

    switch (vp->magic)
    {
        case hhrMirrorGroupIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);
    }

    return NULL;
}

int hhrQosAclMixTable_ipc(struct ipran_snmp_data_cache *cache, struct rule_node *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct rule_node *pdata = NULL;
    int               data_num = 0;
    int               i = 0;
    int               tmp_flag = 0;
    uint32_t          tmp_acl_num = index->acl_num;
    uint32_t          tmp_ruleid = index->rule.mac_acl.ruleid;

    while (1)
    {
        pmsg = qos_com_get_acl_rule_bulk(tmp_acl_num, tmp_ruleid, MODULE_ID_SNMPD, &data_num);
		if (NULL == pmsg)
		{
			return FALSE;
		}
	
		pdata = (struct rule_node *)pmsg->msg_data;

        if (0 == data_num || NULL == pdata)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (i = 0; i < data_num; i++)
            {
                tmp_acl_num = pdata->acl_num;
                tmp_ruleid = pdata->rule.mac_acl.ruleid;

                if (ACL_TYPE_MAC_IP_MIX == pdata->type)
                {
                    snmp_cache_add(cache, pdata, sizeof(struct rule_node));
                    tmp_flag = 1;
                }

                pdata++;
            }

            if (1 == tmp_flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

struct rule_node *hhrQosAclMixTable_lookup(struct ipran_snmp_data_cache *cache ,
                                           int exact,
                                           const struct rule_node *index_input)
{
    struct listnode *node;
    struct rule_node *pdata = NULL;

    for (ALL_LIST_ELEMENTS_RO(cache->data_list, node, pdata))
    {
        if ((NULL == node) || (NULL == pdata))
        {
            return NULL;
        }

        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->acl_num && 0 == index_input->rule.mac_acl.ruleid)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->acl_num == pdata->acl_num) && (index_input->rule.mac_acl.ruleid == pdata->rule.mac_acl.ruleid))
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

uchar *hhrQosAclMixTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct  rule_node *pdata;
    struct  rule_node data_index;
    int ret = 0;
    uint32_t group_index = 0;
    uint32_t rule_index = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &group_index, &rule_index, exact);

    if (ret  <  0)
    {
        return NULL;
    }

    if (NULL == hhrQosAclMixTable_cache)
    {
        hhrQosAclMixTable_cache = snmp_cache_init(sizeof(struct rule_node),
                                                  hhrQosAclMixTable_ipc,
                                                  hhrQosAclMixTable_lookup);

        if (NULL == hhrQosAclMixTable_cache)
        {
            return NULL;
        }

    }

    data_index.acl_num = group_index;
    data_index.rule.mac_acl.ruleid = rule_index;
    pdata = snmp_cache_get_data_by_index(hhrQosAclMixTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->acl_num, pdata->rule.mac_acl.ruleid);
    }

    switch (vp->magic)
    {
        case hhrQosAclMixEthType:
            memset(str_value, 0, sizeof(str_value));

            if (1 == pdata->rule.mac_acl.ethtype_mask)
            {
                sprintf((char *)str_value, "0x%04x", pdata->rule.mac_acl.ethtype);
            }

            *var_len = strlen(str_value);
            return str_value;

        case hhrQosAclMixSmac:
            *var_len = 6;

            if (1 == pdata->rule.mac_acl.smac_mask)
            {
                memcpy(mac_value, pdata->rule.mac_acl.smac, 6);
            }
            else
            {
                memset(mac_value, 0x00, 6);
            }

            return mac_value;

        case hhrQosAclMixDmac:
            *var_len = 6;

            if (1 == pdata->rule.mac_acl.dmac_mask)
            {
                memcpy(mac_value, pdata->rule.mac_acl.dmac, 6);
            }
            else
            {
                memset(mac_value, 0x00, 6);
            }

            return mac_value;

        case hhrQosAclMixVlan:

            if (pdata->rule.mac_acl.vlan >= 4096)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.vlan;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&uint_value);

        case hhrQosAclMixCos:

            if (pdata->rule.mac_acl.cos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.cos;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&uint_value);

        case hhrQosAclMixCvlan:
            *var_len = sizeof(uint32_t);

            if (pdata->rule.mac_acl.cvlan >= 4096)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.cvlan;
            }

            return (uchar *)(&uint_value);

        case hhrQosAclMixCvlanCos:

            if (pdata->rule.mac_acl.cvlan_cos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.cvlan_cos;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&uint_value);

        case hhrQosAclMixIpv4Protocol:

            if (1 == pdata->rule.mac_acl.proto_mask)
            {
                uint_value = pdata->rule.mac_acl.protocol;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&uint_value);

        case hhrQosAclMixIpv4SipAny:
            *var_len = sizeof(int);

            if (0 == pdata->rule.mac_acl.sip_mask)
            {
                int_value_self = 0;
            }
            else if (1 == pdata->rule.mac_acl.sip_mask && 0 == pdata->rule.mac_acl.sip_masklen)
            {
                int_value_self = 2;
            }
            else
            {
                int_value_self = 1;
            }

            return (uchar *)(&int_value_self);

        case hhrQosAclMixIpv4Sip:

            if (1 == pdata->rule.mac_acl.sip_mask && 0 != pdata->rule.mac_acl.sip_masklen)
            {
                ip_value = htonl(pdata->rule.mac_acl.sip);
            }
            else
            {
                ip_value = 0;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&ip_value);

        case hhrQosAclMixIpv4SipMask:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(0xffffffff << (32 - pdata->rule.mac_acl.sip_masklen));
            return (uchar *)(&ip_value);

        case hhrQosAclMixIpv4DipAny:
            *var_len = sizeof(int);

            if (0 == pdata->rule.mac_acl.dip_mask)
            {
                int_value_self = 0;
            }
            else if (1 == pdata->rule.mac_acl.dip_mask && 0 == pdata->rule.mac_acl.dip_masklen)
            {
                int_value_self = 2;
            }
            else
            {
                int_value_self = 1;
            }

            return (uchar *)(&int_value_self);

        case hhrQosAclMixIpv4Dip:

            if (1 == pdata->rule.mac_acl.dip_mask && 0 != pdata->rule.mac_acl.dip_masklen)
            {
                ip_value = htonl(pdata->rule.mac_acl.dip);
            }
            else
            {
                ip_value = 0;
            }

            *var_len = sizeof(uint32_t);
            return (uchar *)(&ip_value);

        case hhrQosAclMixIpv4DipMask:
            *var_len = sizeof(uint32_t);
            ip_value = htonl(0xffffffff << (32 - pdata->rule.mac_acl.dip_masklen));
            return (uchar *)(&ip_value);

        case hhrQosAclMixIpv4L3vpn:
            *var_len = sizeof(uint32_t);

            if (1 == pdata->rule.mac_acl.vpn_mask)
            {
                uint_value = pdata->rule.mac_acl.vpnid;
            }
            else
            {
                uint_value = 0x7FFFFFFF;
            }

            return (uchar *)(&uint_value);

        case hhrQosAclMixIpv4Dscp:
            *var_len = sizeof(uint32_t);

            if (pdata->rule.mac_acl.dscp >= 64)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.dscp;
            }

            return (uchar *)(&uint_value);

        case hhrQosAclMixIpv4Ttl:
            *var_len = sizeof(uint32_t);

            if (1 == pdata->rule.mac_acl.ttl_mask)
            {
                uint_value = pdata->rule.mac_acl.ttl;
            }
            else
            {
                uint_value = 0;
            }

            return (uchar *)(&uint_value);

        case hhrQosAclMixIpv4Tos:
            *var_len = sizeof(uint32_t);

            if (pdata->rule.mac_acl.tos >= 8)
            {
                uint_value = 0x7FFFFFFF;
            }
            else
            {
                uint_value = pdata->rule.mac_acl.tos;
            }

            return (uchar *)(&uint_value);

        case hhrQosAclMixSmacConfig:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->rule.mac_acl.smac_mask ? 1 : 2;
            return (uchar *)(&uint_value);

        case hhrQosAclMixDmacConfig:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->rule.mac_acl.dmac_mask ? 1 : 2;
            return (uchar *)(&uint_value);

        default:
            return NULL;
    }
}
int hhrQosPortLRTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache, struct qos_if_doubleindex *index)
{
	struct ipc_mesg_n *pmsg = NULL;
    struct qos_if_doubleindex *pqos_if_doubleindex = NULL;
    struct qos_if_snmp        *pqos_if_snmp = NULL;
    int                        ret = 0;
    int                        data_num = 0;
    int                        flag = 0;
    int                        i = 0;
    uint32_t                   tmp_ifindex = index->qos_car.ifindex;
    struct qos_if_doubleindex  data_seg;

    while (1)
    {
        pmsg = qos_com_get_qosif_bulk(tmp_ifindex, MODULE_ID_SNMPD, &data_num);
        if (NULL == pmsg)
		{
			return FALSE;
		}

		pqos_if_snmp = (struct qos_if_snmp *)pmsg->msg_data;

        if (0 == data_num || NULL == pqos_if_snmp)
        {
        	mem_share_free(pmsg, MODULE_ID_SNMPD);
            return FALSE;
        }
        else
        {
            for (ret = 0; ret < data_num; ret++)
            {
                tmp_ifindex = pqos_if_snmp->ifindex;

                memset(&data_seg, 0, sizeof(struct qos_if_doubleindex));
                memcpy(&(data_seg.qos_car), pqos_if_snmp, sizeof(struct qos_if_snmp));

                for (i = 1; i < 3; i++)
                {
                    if (QOS_CAR_FLAG_LR == pqos_if_snmp->car_flag[i - 1])
                    {
                        data_seg.direction = i;
                        snmp_cache_add(cache, &data_seg, sizeof(struct qos_if_doubleindex));
                        flag = 1;
                    }
                }

                pqos_if_snmp++;
            }

            if (1 == flag)
            {
            	mem_share_free(pmsg, MODULE_ID_SNMPD);
                return TRUE;
            }
        }
        mem_share_free(pmsg, MODULE_ID_SNMPD);
    }

    return FALSE;
}

struct qos_if_doubleindex *hhrQosPortLRTable_data_lookup(struct ipran_snmp_data_cache *cache ,
                                                         int exact,
                                                         const struct qos_if_doubleindex   *index_input)
{
    struct listnode             *node, *nnode;
    struct qos_if_doubleindex   *data1_find;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, data1_find))
    {
        if (NULL == index_input)
        {
            return cache->data_list->head->data;
        }

        if (0 == index_input->qos_car.ifindex && 0 == index_input->direction)
        {
            return cache->data_list->head->data;
        }

        if ((index_input->qos_car.ifindex == data1_find->qos_car.ifindex) && (index_input->direction == data1_find->direction))
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

uchar *hhrQosPortLRTable_get(struct variable *vp,
                             oid *name,
                             size_t *length,
                             int exact, size_t *var_len, WriteMethod **write_method)
{
    struct qos_if_doubleindex   *pdata = NULL;
    struct qos_if_doubleindex data_index;
    uint32_t if_index = 0;
    int dir_index = 0;
    int ret = 0;
    ret = ipran_snmp_intx2_index_get(vp, name, length, &if_index, &dir_index, exact);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrQosPortLRTable_cache)
    {
        hhrQosPortLRTable_cache = snmp_cache_init(sizeof(struct qos_if_doubleindex),
                                                  hhrQosPortLRTable_get_data_from_ipc,
                                                  hhrQosPortLRTable_data_lookup);

        if (NULL == hhrQosPortLRTable_cache)
        {
            return NULL;
        }
    }

    data_index.qos_car.ifindex = if_index;
    data_index.direction = dir_index;
    pdata = snmp_cache_get_data_by_index(hhrQosPortLRTable_cache, exact, &data_index);

    if (NULL == pdata)
    {
        return NULL;
    }

    if (QOS_DIR_INGRESS != pdata->direction && QOS_DIR_EGRESS != pdata->direction)
    {
        return NULL;
    }

    if (!exact)
    {
        ipran_snmp_intx2_index_set(vp, name, length, pdata->qos_car.ifindex, pdata->direction);
    }

    switch (vp->magic)
    {
        case hhrQosPortLRIfDescr:
            memset(str_value, 0, sizeof(str_value));
            ifm_get_name_by_ifindex(pdata->qos_car.ifindex, str_value);
            *var_len = strlen((char *) str_value);
            return (str_value);

        case hhrQosPortLRCIR:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->qos_car.car[pdata->direction - 1].cir;
            return (uchar *)(&uint_value);

        case hhrQosPortLRCBS:
            *var_len = sizeof(uint32_t);
            uint_value = pdata->qos_car.car[pdata->direction - 1].cbs;
            return (uchar *)(&uint_value);

        default:
            return NULL;
    }
}

