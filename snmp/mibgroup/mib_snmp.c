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
#include <lib/linklist.h>
#include <lib/log.h>

#include "ipran_snmp_data_cache.h"
#include "snmp_config_table.h"

#include "mib_snmp.h"


static uchar     str_value[STRING_LEN] = {'\0'};
static int       int_value = 0;
static uint32_t  ip_value = 0;
static uint8_t   bits_value = 0;

#define                   SNMP_MAX_NAME_LEGTH      (NAME_STRING_LEN + 1)


/*
 * define the structure we're going to ask the agent to register our
 * information at
 */

FindVarMethod hhrSNMPVersion_get;
struct variable1 hhrSNMPVersion_variables[] =
{
    {hhrSNMPVersion,             ASN_INTEGER, RWRITE,   hhrSNMPVersion_get, 1, {1}},
	{hhrSNMPVpnInstance,         ASN_INTEGER, RWRITE,   hhrSNMPVersion_get, 1, {2}}
} ;

FindVarMethod hhrSNMPServerTable_get;
struct variable2 hhrSNMPServerTable_variables[] =
{
    {hhrSNMPServerIP,             ASN_IPADDRESS, RONLY, hhrSNMPServerTable_get, 2, {1, 1}},
    {hhrSNMPServerUdpPort,        ASN_INTEGER,   RONLY, hhrSNMPServerTable_get, 2, {1, 2}},
    {hhrSNMPServerVPN,            ASN_INTEGER,   RONLY, hhrSNMPServerTable_get, 2, {1, 3}},
    {hhrSNMPServerVersion,        ASN_INTEGER,   RONLY, hhrSNMPServerTable_get, 2, {1, 4}},
    {hhrSNMPServerV1V2cCommunity, ASN_OCTET_STR, RONLY, hhrSNMPServerTable_get, 2, {1, 5}},
    {hhrSNMPServerV3User,         ASN_OCTET_STR, RONLY, hhrSNMPServerTable_get, 2, {1, 6}}
} ;

FindVarMethod hhrSNMPV1V2cTable_get;
struct variable2 hhrSNMPV1V2cTable_variables[] =
{
    {hhrSNMPV1V2cCommunity,       ASN_OCTET_STR, RONLY, hhrSNMPV1V2cTable_get, 2, {1, 1}},
    {hhrSNMPV1V2cCommunityLevel,  ASN_INTEGER,   RONLY, hhrSNMPV1V2cTable_get, 2, {1, 2}}
} ;

FindVarMethod hhrSNMPv3UserTable_get;
struct variable2 hhrSNMPv3UserTable_variables[] =
{
    {hhrSNMPv3User,          ASN_OCTET_STR, RONLY, hhrSNMPv3UserTable_get, 2, {1, 1}},
    {hhrSNMPv3UserAuthType,  ASN_INTEGER,   RONLY, hhrSNMPv3UserTable_get, 2, {1, 2}},
    {hhrSNMPv3UserAuthPwd,   ASN_OCTET_STR, RONLY, hhrSNMPv3UserTable_get, 2, {1, 3}},
    {hhrSNMPv3UserPrivilege, ASN_INTEGER,   RONLY, hhrSNMPv3UserTable_get, 2, {1, 4}},
	{hhrSNMPv3UserPriType,   ASN_INTEGER, 	RONLY, hhrSNMPv3UserTable_get, 2, {1, 5}},
    {hhrSNMPv3UserPriPwd, 	 ASN_OCTET_STR, RONLY, hhrSNMPv3UserTable_get, 2, {1, 6}}
} ;

oid  hhrSNMPVersion_oid[]   = { HHR_SNMP_ROID};
int  hhrSNMPVersion_oid_len = sizeof(hhrSNMPVersion_oid) / sizeof(oid);

oid  hhrSNMPServerTable_oid[]   = { HHR_SNMP_ROID , 10};
int  hhrSNMPServerTable_oid_len = sizeof(hhrSNMPServerTable_oid) / sizeof(oid);

oid  hhrSNMPV1V2cTable_oid[]   = { HHR_SNMP_ROID , 11};
int  hhrSNMPV1V2cTable_oid_len = sizeof(hhrSNMPV1V2cTable_oid) / sizeof(oid);

oid  hhrSNMPv3UserTable_oid[]   = { HHR_SNMP_ROID , 12};
int  hhrSNMPv3UserTable_oid_len = sizeof(hhrSNMPv3UserTable_oid) / sizeof(oid);

static struct ipran_snmp_data_cache *hhrSNMPServerTable_cache = NULL ;
static struct ipran_snmp_data_cache *hhrSNMPV1V2cTable_cache = NULL ;
static struct ipran_snmp_data_cache *hhrSNMPv3UserTable_cache = NULL ;

/*-------------------------------------H3C--------------------------------------*/
oid  h3cSNMPVersion_oid[]   = { H3C_SNMP_ROID , 1};

FindVarMethod h3cSNMPVersion_get;
struct variable1 h3cSNMPVersion_variables[] =
{
    {hhSnmpExtVersion,     ASN_OCTET_STR , RONLY,   h3cSNMPVersion_get, 1, {5}}
} ;

oid  h3cCommunityTable_oid[]   = { H3C_SNMP_ROID , 2 , 1};

FindVarMethod h3cSCommunityTable_get;
struct variable2  h3cCommunityTable_variables[] =
{
    {hhSnmpExtCommunitySecurityLevel_mib, ASN_INTEGER,   RONLY, h3cSCommunityTable_get, 2, {1, 1}},
    {hhSnmpExtCommunitySecurityName_mib,  ASN_OCTET_STR, RONLY, h3cSCommunityTable_get, 2, {1, 2}},
    {hhSnmpExtCommunityName_mib,          ASN_OCTET_STR, RONLY, h3cSCommunityTable_get, 2, {1, 3}},
    {hhSnmpExtCommunityAclNum_mib,        ASN_INTEGER,   RONLY, h3cSCommunityTable_get, 2, {1, 4}},
    {hhSnmpExtCommunityIPv6AclNum_mib,    ASN_INTEGER,   RONLY, h3cSCommunityTable_get, 2, {1, 5}}
} ;
/*-----------------------------------  H3C End-----------------------------------*/


void  init_mib_snmp(void)
{
    REGISTER_MIB("ipran/hhrSNMPVersion", hhrSNMPVersion_variables, variable1,
                 hhrSNMPVersion_oid) ;
    REGISTER_MIB("ipran/hhrSNMPServerTable", hhrSNMPServerTable_variables, variable2,
                 hhrSNMPServerTable_oid) ;
    REGISTER_MIB("ipran/hhrSNMPV1V2cTable", hhrSNMPV1V2cTable_variables, variable2,
                 hhrSNMPV1V2cTable_oid) ;
    REGISTER_MIB("ipran/hhrSNMPv3UserTable", hhrSNMPv3UserTable_variables, variable2,
                 hhrSNMPv3UserTable_oid) ;

    /*-------------------------------------H3C--------------------------------------*/
    REGISTER_MIB("ipran/h3cCommunity", h3cCommunityTable_variables, variable2,
                 h3cCommunityTable_oid) ;

    REGISTER_MIB("ipran/h3cSNMPVersion_get", h3cSNMPVersion_variables, variable1,
                 h3cSNMPVersion_oid) ;
    /*-----------------------------------H3C End------------------------------------*/
}

u_char *hhrSNMPVersion_get(struct variable *vp,
                           oid *pName,  size_t *iOidLen,
                           int  iExact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    unsigned char  *pstr = NULL;

    iRetVal = header_generic(vp, pName, iOidLen,
                             iExact, var_len, write_method);

    if (iRetVal == MATCH_FAILED)
    {
        return (NULL);
    }

    switch (vp->magic)
    {
        case hhrSNMPVersion :
            *var_len = sizeof(int);
            return (u_char *)&snmp_version;

		case hhrSNMPVpnInstance:
			*var_len = sizeof(int);
			int_value = (int)snmp_server_vpn;
            return (u_char *)&int_value;

        default :
            return (NULL);
    }
}


u_char *
hhrSNMPServerTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{

    struct _trap_item index ;
    int ret = 0;
    struct _trap_item *pTRAP_ITEM = NULL;
    uint32_t ipaddr_l = 0;

    *write_method = NULL;

    /* validate the index */
    ret = ipran_snmp_ip_index_get(vp, name, length, &ipaddr_l, exact);

    if (ret < 0)
    {
        return NULL;
    }

    index.ip_addr.addr[0] = ntohl(ipaddr_l);

    pTRAP_ITEM = snmp_trap_lookup_snmp(&index , exact);

    if (NULL == pTRAP_ITEM)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_ip_index_set(vp, name, length, htonl(pTRAP_ITEM->ip_addr.addr[0]));
    }

    switch (vp->magic)
    {
        case hhrSNMPServerIP :
            *var_len = sizeof(uint32_t);
            ip_value = pTRAP_ITEM->ip_addr.addr[0];
            ip_value = htonl(ip_value);
            return (u_char *)(&ip_value);

        case hhrSNMPServerUdpPort :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->port ;
            return (u_char *)(&int_value);

        case hhrSNMPServerVPN :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->vpn ;
            return (u_char *)(&int_value);

        case hhrSNMPServerVersion :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->version ;
            return (u_char *)(&int_value);

        case hhrSNMPServerV1V2cCommunity:
            memset(str_value, 0, STRING_LEN);

            if (V2C == pTRAP_ITEM->version || V1 == pTRAP_ITEM->version)
            {
                snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->name);
            }

            *var_len = strlen(str_value);
            return (str_value);

        case hhrSNMPServerV3User:
            memset(str_value, 0, STRING_LEN);

            if (V3 == pTRAP_ITEM->version)
            {
                snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->name);
            }

            *var_len = strlen(str_value);
            return (str_value);

        default:
            return (NULL) ;
    }
}

u_char *
hhrSNMPV1V2cTable_get(struct variable *vp,
                      oid *name,
                      size_t *length,
                      int exact, size_t *var_len, WriteMethod **write_method)
{
    struct _community_item index ;
    int ret = 0;
    struct _community_item *pTRAP_ITEM = NULL;

    *write_method = NULL;

    /* validate the index */
    ret = ipran_snmp_octstring_index_get(vp, name, length, &index.name, SNMP_MAX_NAME_LEGTH, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pTRAP_ITEM = snmp_community_lookup_snmp(&index , exact);

    if (NULL == pTRAP_ITEM)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_octstring_index_set(vp, name, length, pTRAP_ITEM->name , strlen(pTRAP_ITEM->name));
    }

    switch (vp->magic)
    {
        case hhrSNMPV1V2cCommunity :
            memset(str_value, 0, STRING_LEN);
            snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->name);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrSNMPV1V2cCommunityLevel :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->attribute ;
            return (u_char *)(&int_value);

        default :
            return (NULL);
    }
}


u_char *
hhrSNMPv3UserTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)
{
    struct _user_item index ;
    u_int32_t index_next = 0;
    int ret = 0;
    struct _user_item *pTRAP_ITEM = NULL;

    *write_method = NULL;

    /* validate the index */
    ret = ipran_snmp_octstring_index_get(vp, name, length, &index.name, SNMP_MAX_NAME_LEGTH , exact);

    if (ret < 0)
    {
        return NULL;
    }

    pTRAP_ITEM = snmp_user_lookup_snmp(&index , exact);

    if (NULL == pTRAP_ITEM)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_octstring_index_set(vp, name, length, pTRAP_ITEM->name, strlen(pTRAP_ITEM->name));
    }

    switch (vp->magic)
    {
        case hhrSNMPv3User :
            memset(str_value, 0, STRING_LEN);
            snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->name);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrSNMPv3UserAuthType :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->auth_method ;
            return (u_char *)(&int_value);

        case hhrSNMPv3UserAuthPwd :
            memset(str_value, 0, STRING_LEN);
            snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->auth_secret);
            *var_len = strlen(str_value);
            return (str_value);

        case hhrSNMPv3UserPrivilege :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->permission ;
            return (u_char *)(&int_value);

		case hhrSNMPv3UserPriType :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->encry_method ;
            return (u_char *)(&int_value);

		case hhrSNMPv3UserPriPwd :
            memset(str_value, 0, STRING_LEN);
            snprintf(str_value, SNMP_MAX_NAME_LEGTH, "%s", pTRAP_ITEM->encry_secret);
            *var_len = strlen(str_value);
            return (str_value);

        default :
            return (NULL);
    }

}


/*-------------------------------H3C---------------------------------------------*/

u_char *
h3cSNMPVersion_get(struct variable *vp,
                   oid *name,
                   size_t *length,
                   int exact, size_t *var_len, WriteMethod **write_method)
{
    int    iRetVal = 0, i;
    unsigned char  *pstr = NULL;
    uint8_t temp = 0;
    uint8_t int8_version = 0;
    iRetVal = header_generic(vp, name, length,
                             exact, var_len, write_method);

    if (iRetVal == MATCH_FAILED)
    {
        return (NULL);
    }

    *write_method = NULL;

    switch (vp->magic)
    {
        case hhSnmpExtVersion :
            switch (snmp_version)
            {
                case V1 :
                    bits_value = 0x1;
                    break ;

                case V2C :
                    bits_value = 0x2;
                    break ;

                case V3 :
                    bits_value = 0x4;
                    break ;

                default :
                    bits_value = 0x7;
                    break ;
            }

            EIGHT_BITS_EXCHANGE(temp, bits_value);
            bits_value = temp ;

            *var_len = sizeof(uint8_t);
            return (u_char *)&bits_value;

        default :
            return (NULL);
    }
}

u_char *
h3cSCommunityTable_get(struct variable *vp,
                       oid *name,
                       size_t *length,
                       int exact, size_t *var_len, WriteMethod **write_method)

{
    struct _community_item index ;
    int ret = 0;
    struct _community_item *pTRAP_ITEM = NULL;
    int index2_len;

    *write_method = NULL;
    memset(&index , 0 , sizeof(struct _community_item));
    /* validate the index */
    ret = ipran_snmp_int_str_index_get(vp, name, length,
                                       &index.attribute, &index.name,
                                       COMMUNITY_NAME_MAX_LEN, &index2_len, exact);

    if (ret < 0)
    {
        return NULL;
    }

    pTRAP_ITEM = snmp_community_lookup_snmp(&index , exact);

    if (NULL == pTRAP_ITEM)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        index2_len = strlen(pTRAP_ITEM->name) ;
        ipran_snmp_int_str_index_set(vp, name, length, pTRAP_ITEM->attribute, &(pTRAP_ITEM->name) , index2_len);
    }

    switch (vp->magic)
    {
        case hhSnmpExtCommunityName_mib :
            memset(str_value, 0, STRING_LEN);
            sprintf(str_value , "%s", pTRAP_ITEM->name);
            *var_len = strlen(str_value);
            return (str_value);

        case hhSnmpExtCommunityAclNum_mib :
            *var_len = sizeof(int);
            int_value = pTRAP_ITEM->acl_num ;
            return (u_char *)(&int_value);

        case hhSnmpExtCommunityIPv6AclNum_mib :
            *var_len = sizeof(int);
            int_value = 0 ;
            return (u_char *)(&int_value);

        default :
            return (NULL);
    }
}

/*-------------------------------H3C End------------------------------------------*/
