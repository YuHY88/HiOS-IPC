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
#include "mib_aaa.h"

#include "aaa/aaa_snmp.h"
#include "ipran_snmp_data_cache.h"


/*local temp variable*/
static uchar    str_value[STRING_LEN] = {'\0'};
static uchar    *str = NULL;
static uchar    mac_value[6] = {0};
static int      int_value = 0;
static uint32_t ip_value = 0;


/* table list define */
static struct ipran_snmp_data_cache *hhrAAAUserConfigTable_cache = NULL ;
static struct ipran_snmp_data_cache *hhrAAAUserOnlineTable_cache = NULL ;
static struct ipran_snmp_data_cache *hhrAAAUserLogTable_cache = NULL ;



/*radius cfg*/
static oid hhrAAARadiusCfg_oid[] = {HHRAAA, 1};
FindVarMethod hhrAAARadiusCfg_get;
struct variable1 hhrAAARadiusCfg_variables[] =
{
    {hhrAAARadiusAuthServerIP,          ASN_IPADDRESS,  RWRITE, hhrAAARadiusCfg_get, 1, {1}},
    {hhrAAARadiusAuthServerL3vpn,       ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {2}},
    {hhrAAARadiusAcctServerIP,          ASN_IPADDRESS,  RWRITE, hhrAAARadiusCfg_get, 1, {3}},
    {hhrAAARadiusAcctServerL3vpn,       ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {4}},
    {hhrAAARadiusAcctSwitch,            ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {5}},
    {hhrAAARadiusAcctUpdateInterval,    ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {6}},
    {hhrAAARadiusKey,                   ASN_OCTET_STR,  RWRITE, hhrAAARadiusCfg_get, 1, {7}},
    {hhrAAARadiusRetryTimes,            ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {8}},
    {hhrAAARadiusRetryInterval,         ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {9}},
    {hhrAAARadiusResponseTimeout,       ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {10}},
    {hhrAAARadiusAuthMode,              ASN_INTEGER,    RWRITE, hhrAAARadiusCfg_get, 1, {11}}
};

/*tacacs cfg*/
static oid hhrAAATacacsCfg_oid[] = {HHRAAA, 2};
FindVarMethod hhrAAATacacsCfg_get;
struct variable1 hhrAAATacacsCfg_variables[] =
{
    {hhrAAATacacsAuthenServerIP,        ASN_IPADDRESS,  RWRITE, hhrAAATacacsCfg_get, 1, {1}},
    {hhrAAATacacsAuthenServerL3vpn,     ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {2}},
    {hhrAAATacacsAuthorServerIP,        ASN_IPADDRESS,  RWRITE, hhrAAATacacsCfg_get, 1, {3}},
    {hhrAAATacacsAuthorServerL3vpn,     ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {4}},
    {hhrAAATacacsAuthorSwitch,          ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {5}},
    {hhrAAATacacsAcctServerIP,          ASN_IPADDRESS,  RWRITE, hhrAAATacacsCfg_get, 1, {6}},
    {hhrAAATacacsAcctServerL3vpn,       ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {7}},
    {hhrAAATacacsAcctSwitch,            ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {8}},
    {hhrAAATacacsAcctUpdateInterval,    ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {9}},
    {hhrAAATacacsKey,                   ASN_OCTET_STR,  RWRITE, hhrAAATacacsCfg_get, 1, {10}},
    {hhrAAATacacsResponseTimeout,       ASN_INTEGER,    RWRITE, hhrAAATacacsCfg_get, 1, {11}}
};


/*common cfg*/
static oid hhrAAACommonCfg_oid[] = {HHRAAA, 3};
FindVarMethod hhrAAACommonCfg_get;
struct variable1 hhrAAACommonCfg_variables[] =
{
    {hhrAAAUserMaxIdleTime,             ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {1}},
    {hhrAAAUserMaxLoginNum,             ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {2}},
    {hhrAAAUserDefaultLoginLevel,       ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {3}},
    {hhrAAAAuthorizeFailedOnline,       ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {4}},
    {hhrAAAAccountFailedOnline,         ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {5}},
    {hhrAAAConsoleAdminMode,            ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {6}},
    {hhrAAALoginMode1,                  ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {7}},
    {hhrAAALoginMode2,                  ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {8}},
    {hhrAAALoginMode3,                  ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {9}},
    {hhrAAALoginMode4,                  ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {10}},
    {hhrAAANoneUserLevel,               ASN_INTEGER,    RWRITE, hhrAAACommonCfg_get, 1, {11}},
};



/*enable cfg*/
static oid hhrAAAEnableCfg_oid[] = {HHRAAA, 4};
FindVarMethod hhrAAAEnableCfg_get;
struct variable1 hhrAAAEnableCfg_variables[] =
{
    {hhrAAAEnablePasswordLevel0,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {1}},
    {hhrAAAEnablePasswordLevel1,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {2}},
    {hhrAAAEnablePasswordLevel2,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {3}},
    {hhrAAAEnablePasswordLevel3,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {4}},
    {hhrAAAEnablePasswordLevel4,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {5}},
    {hhrAAAEnablePasswordLevel5,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {6}},
    {hhrAAAEnablePasswordLevel6,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {7}},
    {hhrAAAEnablePasswordLevel7,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {8}},
    {hhrAAAEnablePasswordLevel8,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {9}},
    {hhrAAAEnablePasswordLevel9,        ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {10}},
    {hhrAAAEnablePasswordLevel10,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {11}},
    {hhrAAAEnablePasswordLevel11,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {12}},
    {hhrAAAEnablePasswordLevel12,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {13}},
    {hhrAAAEnablePasswordLevel13,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {14}},
    {hhrAAAEnablePasswordLevel14,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {15}},
    {hhrAAAEnablePasswordLevel15,       ASN_OCTET_STR,  RWRITE, hhrAAAEnableCfg_get, 1, {16}},
};



/* aaa user config table */
static oid hhrAAAUserConfigTable_oid[] = {HHRAAA, 10};
FindVarMethod hhrAAAUserConfigTable_get;
struct variable2 hhrAAAUserConfigTable_variables[] =
{
    {hhrAAAUserConfigUsername,          ASN_OCTET_STR,  RONLY, hhrAAAUserConfigTable_get, 2, {1, 2}},
    {hhrAAAUserConfigPassword,          ASN_OCTET_STR,  RONLY, hhrAAAUserConfigTable_get, 2, {1, 3}},
    {hhrAAAUserConfigLevel,             ASN_INTEGER,    RONLY, hhrAAAUserConfigTable_get, 2, {1, 4}},
    {hhrAAAUserConfigMaxRepeatedNum,    ASN_INTEGER,    RONLY, hhrAAAUserConfigTable_get, 2, {1, 5}}
};


/* aaa user online table */
static oid hhrAAAUserOnlineTable_oid[] = {HHRAAA, 11};
FindVarMethod hhrAAAUserOnlineTable_get;
struct variable2 hhrAAAUserOnlineTable_variables[] =
{
    {hhrAAAUserOnlineUsername,          ASN_OCTET_STR,  RONLY, hhrAAAUserOnlineTable_get, 2, {1, 2}},
    {hhrAAAUserOnlineLoginFrom,         ASN_OCTET_STR,  RONLY, hhrAAAUserOnlineTable_get, 2, {1, 3}},
    {hhrAAAUserOnlineLevelAuthor,       ASN_INTEGER,    RONLY, hhrAAAUserOnlineTable_get, 2, {1, 4}},
    {hhrAAAUserOnlineLevelCurrent,      ASN_INTEGER,    RONLY, hhrAAAUserOnlineTable_get, 2, {1, 5}},
    {hhrAAAUserOnlineLoginMethod,       ASN_INTEGER,    RONLY, hhrAAAUserOnlineTable_get, 2, {1, 6}},
    {hhrAAAUserOnlineTimeLogin,         ASN_INTEGER,    RONLY, hhrAAAUserOnlineTable_get, 2, {1, 7}},
    {hhrAAAUserOnlineTimeOnline,        ASN_INTEGER,    RONLY, hhrAAAUserOnlineTable_get, 2, {1, 8}}
};



/* aaa user log table */
static oid hhrAAAUserLogTable_oid[] = {HHRAAA, 13};
FindVarMethod hhrAAAUserLogTable_get;
struct variable2 hhrAAAUserLogTable_variables[] =
{
    {hhrAAAUserLogUsername,             ASN_OCTET_STR,  RONLY, hhrAAAUserLogTable_get, 2, {1, 2}},
    {hhrAAAUserLogLevel,                ASN_INTEGER,    RONLY, hhrAAAUserLogTable_get, 2, {1, 3}},
    {hhrAAAUserLogLoginMethod,          ASN_INTEGER,    RONLY, hhrAAAUserLogTable_get, 2, {1, 4}},
    {hhrAAAUserLogLoginFrom,            ASN_OCTET_STR,  RONLY, hhrAAAUserLogTable_get, 2, {1, 5}},
    {hhrAAAUserLogLoginResult,          ASN_INTEGER,    RONLY, hhrAAAUserLogTable_get, 2, {1, 6}},
    {hhrAAAUserLogTimeLogin,            ASN_INTEGER,    RONLY, hhrAAAUserLogTable_get, 2, {1, 7}},
    {hhrAAAUserLogTimeExit,             ASN_INTEGER,    RONLY, hhrAAAUserLogTable_get, 2, {1, 8}}
};



/**************************************** H3C ****************************************************************/

/* aaa user log table */
static oid h3c_hhrAAALocalUserTable_oid[] = {H3C_USER_MIB_OID, 1};
FindVarMethod h3c_hhrAAALocalUserTable_get;
struct variable2 h3c_hhrAAALocalUserTable_variables[] =
{
    {h3c_hhrUserName,               ASN_OCTET_STR,  RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 1}},
    {h3c_hhrPassword,               ASN_OCTET_STR,  RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 2}},
    {h3c_hhrAuthMode,               ASN_INTEGER,    RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 3}},
    {h3c_hhrUserLevel,              ASN_INTEGER,    RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 4}},
    {h3c_hhrUserState,              ASN_INTEGER,    RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 5}},
    {h3c_hhrUserOnlineRowStatus,    ASN_INTEGER,    RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 6}},
    {h3c_hhrUserIndex,              ASN_INTEGER,    RONLY, h3c_hhrAAALocalUserTable_get, 2, {1, 7}}
};

/**************************************** END *****************************************************************/


int hhrAAAUserConfigTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            SNMP_USER_CONFIG_INFO *pinfo_input)
{
#if 0
    SNMP_USER_CONFIG_INFO *pinfo = NULL;
    int data_num = 0;
    int ret = 0;
	
    pinfo = (SNMP_USER_CONFIG_INFO *)snmp_table_info_get_bulk(&pinfo_input->index, 
		sizeof(SNMP_USER_CONFIG_INDEX), MODULE_ID_AAA, SNMP_USER_CONFIG_TABLE, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] data_num [%d]\n", __FILE__, __LINE__, __func__,
        pinfo_input->index.user_id, data_num);

    if (0 == data_num || NULL == pinfo)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pinfo , sizeof(SNMP_USER_CONFIG_INFO));
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] [%d]\n", __FILE__, __LINE__, __func__,
				pinfo->index.user_id, ret);
            pinfo++;
        }

        return TRUE;
    }
#endif

	snmp_table_info_get_bulk(cache, (void *)&pinfo_input->index, sizeof(SNMP_USER_CONFIG_INDEX), sizeof(SNMP_USER_CONFIG_INFO),
		MODULE_ID_AAA, SNMP_USER_CONFIG_TABLE);
}

SNMP_USER_CONFIG_INFO *hhrAAAUserConfigTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                         int exact, const SNMP_USER_CONFIG_INDEX *index)
{
    struct listnode  *node, *nnode;

    SNMP_USER_CONFIG_INFO *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index) || (0 == index->user_id))
        {
            return (SNMP_USER_CONFIG_INFO *)cache->data_list->head->data;
        }

        if (pinfo_find->index.user_id == index->user_id)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (SNMP_USER_CONFIG_INFO *)node->next->data;
                }
            }
        }
    }

    return NULL;
}

int hhrAAAUserOnlineTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                            SNMP_USER_ONLINE_INFO *pinfo_input)
{
#if 0
    SNMP_USER_ONLINE_INFO *pinfo = NULL;
    int data_num = 0;
    int ret = 0;
	
    pinfo = (SNMP_USER_ONLINE_INFO *)snmp_table_info_get_bulk(&pinfo_input->index,
		sizeof(SNMP_USER_ONLINE_INDEX), MODULE_ID_AAA, SNMP_USER_ONLINE_TABLE, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] data_num [%d]\n", __FILE__, __LINE__, __func__,
               pinfo_input->index.user_id, data_num);

    if (0 == data_num || NULL == pinfo)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pinfo , sizeof(SNMP_USER_ONLINE_INFO));
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] [%d]\n", __FILE__, __LINE__, __func__,
                       pinfo->index.user_id, ret);
            pinfo++;
        }

        return TRUE;
    }
#endif

	snmp_table_info_get_bulk(cache, (void *)&pinfo_input->index, sizeof(SNMP_USER_ONLINE_INDEX), sizeof(SNMP_USER_ONLINE_INFO),
		MODULE_ID_AAA, SNMP_USER_ONLINE_TABLE);
}

SNMP_USER_ONLINE_INFO *hhrAAAUserOnlineTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                         int exact, const SNMP_USER_ONLINE_INDEX *index_input)
{
    struct listnode  *node, *nnode;

    SNMP_USER_ONLINE_INFO *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index_input) || (0 == index_input->user_id))
        {
            return (SNMP_USER_ONLINE_INFO *)cache->data_list->head->data;
        }

        if (pinfo_find->index.user_id == index_input->user_id)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (SNMP_USER_ONLINE_INFO *)node->next->data;
                }
            }
        }
    }

    return NULL;
}

int hhrAAAUserLogTable_get_data_from_ipc(struct ipran_snmp_data_cache *cache,
                                         SNMP_USER_LOG_INFO *pinfo_input)
{
#if 0
    SNMP_USER_LOG_INFO *pinfo = NULL;
    int data_num = 0;
    int ret = 0;
	
    pinfo = (SNMP_USER_LOG_INFO *)snmp_table_info_get_bulk(&pinfo_input->index, 
		sizeof(SNMP_USER_LOG_INDEX), MODULE_ID_AAA, SNMP_USER_LOG_TABLE, &data_num);

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] data_num [%d]\n", __FILE__, __LINE__, __func__,
               pinfo_input->index.user_id, data_num);

    if (0 == data_num || NULL == pinfo)
    {
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, pinfo , sizeof(SNMP_USER_LOG_INFO));
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': user_id [%d] [%d]\n", __FILE__, __LINE__, __func__,
                       pinfo->index.user_id, ret);
            pinfo++;
        }

        return TRUE;
    }
#endif

	snmp_table_info_get_bulk(cache, (void *)&pinfo_input->index, sizeof(SNMP_USER_LOG_INDEX), sizeof(SNMP_USER_LOG_INFO),
		MODULE_ID_AAA, SNMP_USER_LOG_TABLE);
}

SNMP_USER_LOG_INFO *hhrAAAUserLogTable_node_lookup(struct ipran_snmp_data_cache *cache,
                                                   int exact, const SNMP_USER_LOG_INDEX *index_input)
{
    struct listnode  *node, *nnode;
    SNMP_USER_LOG_INFO *pinfo_find = NULL;

    for (ALL_LIST_ELEMENTS(cache->data_list, node, nnode, pinfo_find))
    {
        if ((NULL == index_input) || (0 == index_input->user_id))
        {
            return (SNMP_USER_LOG_INFO *)cache->data_list->head->data ;
        }

        if (pinfo_find->index.user_id == index_input->user_id)
        {
            if (1 == exact) //get
            {
                return pinfo_find;
            }
            else
            {
                if (NULL == node->next)
                {
                    return NULL;
                }
                else
                {
                    return (SNMP_USER_LOG_INFO *)node->next->data;
                }
            }
        }
    }

    return NULL;
}

void init_mib_aaa(void)
{
    REGISTER_MIB("hhrAAARadiusCfg", hhrAAARadiusCfg_variables,
                 variable1, hhrAAARadiusCfg_oid);

    REGISTER_MIB("hhrAAATacacsCfg", hhrAAATacacsCfg_variables,
                 variable1, hhrAAATacacsCfg_oid);

    REGISTER_MIB("hhrAAACommonCfg", hhrAAACommonCfg_variables,
                 variable1, hhrAAACommonCfg_oid);

    REGISTER_MIB("hhrAAAUserConifgTable", hhrAAAUserConfigTable_variables,
                 variable2, hhrAAAUserConfigTable_oid);

    REGISTER_MIB("hhrAAAUserOnlineTable", hhrAAAUserOnlineTable_variables,
                 variable2, hhrAAAUserOnlineTable_oid);

    REGISTER_MIB("hhrAAAUserLogTable", hhrAAAUserLogTable_variables,
                 variable2, hhrAAAUserLogTable_oid);


    /* H3C */
    REGISTER_MIB("h3c_hhrAAALocalUserTable", h3c_hhrAAALocalUserTable_variables,
                 variable2, h3c_hhrAAALocalUserTable_oid);
}

u_char *hhrAAARadiusCfg_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }
	
    struct aaa_snmp_radius_cfg data;
	memset(&data, 0, sizeof(struct aaa_snmp_radius_cfg));
		
	int ret = snmp_scalar_info_get(&data, sizeof(data), MODULE_ID_AAA, SNMP_AAA_RADIUS_CFG);
	
    switch (vp->magic)
    {
        case hhrAAARadiusAuthServerIP:
            ip_value = 0;
            if (TRUE == ret)
            {
                ip_value = (uint32_t)data.auth_server_ip;
            }

            ip_value = htonl(ip_value);
            *var_len = sizeof(ip_value);
            return ((u_char *)&ip_value);

        case hhrAAARadiusAuthServerL3vpn:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.auth_server_l3vpn;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusAcctServerIP:
            ip_value = 0;

            if (TRUE == ret)
            {
                ip_value = (uint32_t)data.acct_server_ip;
            }

            ip_value = htonl(ip_value);
            *var_len = sizeof(ip_value);
            return ((u_char *)&ip_value);

        case hhrAAARadiusAcctServerL3vpn:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_server_l3vpn;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusAcctSwitch:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_switch;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusAcctUpdateInterval:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_update_interval;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusKey:
			memset(str_value, 0, sizeof(str_value));
            if (TRUE == ret)
            {
                *var_len = strlen(data.key);
				strcpy(str_value, data.key);
                return ((u_char *)str_value);
            }
            else
            {
                return NULL;
            }

        case hhrAAARadiusRetryTimes:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.retry_times;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusRetryInterval:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.retry_interval;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusResponseTimeout:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.rsp_timeout;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAARadiusAuthMode:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.auth_mode;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *hhrAAATacacsCfg_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }
	
    struct aaa_snmp_tacacs_cfg data;
	memset(&data, 0, sizeof(struct aaa_snmp_tacacs_cfg));
		
	int ret = snmp_scalar_info_get(&data, sizeof(data), MODULE_ID_AAA, SNMP_AAA_TAC_CFG);

    switch (vp->magic)
    {
        case hhrAAATacacsAuthenServerIP:
            ip_value = 0;

            if (TRUE == ret)
            {
                ip_value = (uint32_t)data.authen_server_ip;
                ip_value = htonl(ip_value);
            }

            *var_len = sizeof(ip_value);
            return ((u_char *)&ip_value);

        case hhrAAATacacsAuthenServerL3vpn:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.authen_server_l3vpn;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsAuthorServerIP:
            ip_value = 0;

            if (TRUE == ret)
            {
                ip_value = (uint32_t)data.author_server_ip;
                ip_value = htonl(ip_value);
            }

            *var_len = sizeof(ip_value);
            return ((u_char *)&ip_value);

        case hhrAAATacacsAuthorServerL3vpn:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.author_server_l3vpn;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsAuthorSwitch:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.author_switch;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsAcctServerIP:
            ip_value = 0;

            if (TRUE == ret)
            {
                ip_value = data.acct_server_ip;
                ip_value = htonl(ip_value);
            }

            *var_len = sizeof(ip_value);
            return ((u_char *)&ip_value);

        case hhrAAATacacsAcctServerL3vpn:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_server_l3vpn;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsAcctSwitch:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_switch;

                if (DISABLE == int_value)
                {
                    int_value = 2;
                }
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsAcctUpdateInterval:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_update_interval;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAATacacsKey:
            if (TRUE == ret)
            {
                *var_len = strlen(data.key);
				strcpy(str_value, data.key);
                return ((u_char *)str_value);
            }
            else
            {
                return NULL;
            }

        case hhrAAATacacsResponseTimeout:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.rsp_timeout;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *hhrAAACommonCfg_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }
	
    struct aaa_snmp_common_cfg data;
	memset(&data, 0, sizeof(struct aaa_snmp_common_cfg));
		
	int ret = snmp_scalar_info_get(&data, sizeof(data), MODULE_ID_AAA, SNMP_AAA_COMMON_CFG);

    switch (vp->magic)
    {
        case hhrAAALoginMode1:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_mode_first_pri;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAALoginMode2:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_mode_second_pri;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAALoginMode3:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_mode_third_pri;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAALoginMode4:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_mode_forth_pri;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAConsoleAdminMode:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.console_admin_mode;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAAuthorizeFailedOnline:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.author_failed_online;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAAccountFailedOnline:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.acct_failed_online;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAUserMaxIdleTime:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_user_idle_cut;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAUserMaxLoginNum:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_user_max_num;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAAUserDefaultLoginLevel:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.login_user_def_level;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        case hhrAAANoneUserLevel:
            int_value = 0;

            if (TRUE == ret)
            {
                int_value = data.none_user_level;
            }

            *var_len = sizeof(int_value);
            return ((u_char *)&int_value);

        default:
            return NULL;
    }
}

u_char *hhrAAAEnableCfg_get(struct variable *vp,
                            oid *name,
                            size_t *length,
                            int exact, size_t *var_len, WriteMethod **write_method)
{
    if (MATCH_FAILED == header_generic(vp, name, length, exact, var_len, write_method))
    {
        return NULL;
    }
	
	static ENABLE_INFO data;
	memset(&data, 0, sizeof(ENABLE_INFO));
		
	int ret = snmp_scalar_info_get(&data, sizeof(data), MODULE_ID_AAA, SNMP_AAA_ENABLE_CFG);
	
    switch (vp->magic)
    {
        case hhrAAAEnablePasswordLevel0:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[0]);
                return ((u_char *)data.password[0]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel1:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[1]);
                return ((u_char *)data.password[1]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel2:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[2]);
                return ((u_char *)data.password[2]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel3:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[3]);
                return ((u_char *)data.password[3]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel4:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[4]);
                return ((u_char *)data.password[4]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel5:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[5]);
                return ((u_char *)data.password[5]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel6:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[6]);
                return ((u_char *)data.password[6]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel7:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[7]);
                return ((u_char *)data.password[7]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel8:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[8]);
                return ((u_char *)data.password[8]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel9:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[9]);
                return ((u_char *)data.password[9]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel10:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[10]);
                return ((u_char *)data.password[10]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel11:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[11]);
                return ((u_char *)data.password[11]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel12:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[12]);
                return ((u_char *)data.password[12]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel13:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[13]);
                return ((u_char *)data.password[13]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel14:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[14]);
                return ((u_char *)data.password[14]);
            }
            else
            {
                return NULL;
            }

        case hhrAAAEnablePasswordLevel15:
            if (TRUE == ret)
            {
                *var_len = strlen(data.password[15]);
                return ((u_char *)data.password[15]);
            }
            else
            {
                return NULL;
            }

        default:
            return NULL;
    }
}

u_char *hhrAAAUserConfigTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    SNMP_USER_CONFIG_INFO *pinfo = NULL;
    SNMP_USER_CONFIG_INDEX index;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.user_id = index_int ;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrAAAUserConfigTable_cache)
    {
        hhrAAAUserConfigTable_cache = snmp_cache_init(sizeof(SNMP_USER_CONFIG_INFO),
                                                      hhrAAAUserConfigTable_get_data_from_ipc,
                                                      hhrAAAUserConfigTable_node_lookup);

        if (NULL == hhrAAAUserConfigTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (SNMP_USER_CONFIG_INFO *)snmp_cache_get_data_by_index(hhrAAAUserConfigTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.user_id);
    }

    switch (vp->magic)
    {
        case hhrAAAUserConfigUsername:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.username, strlen(pinfo->data.username));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserConfigPassword:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.password, strlen(pinfo->data.password));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserConfigLevel:
            int_value = pinfo->data.level;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserConfigMaxRepeatedNum:
            int_value = pinfo->data.max_repeat_num;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}

u_char *hhrAAAUserOnlineTable_get(struct variable *vp,
                                  oid *name,
                                  size_t *length,
                                  int exact, size_t *var_len, WriteMethod **write_method)
{
    SNMP_USER_ONLINE_INFO *pinfo = NULL;
    SNMP_USER_ONLINE_INDEX index;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.user_id = index_int ;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrAAAUserOnlineTable_cache)
    {
        hhrAAAUserOnlineTable_cache = snmp_cache_init(sizeof(SNMP_USER_ONLINE_INFO),
                                                      hhrAAAUserOnlineTable_get_data_from_ipc,
                                                      hhrAAAUserOnlineTable_node_lookup);

        if (NULL == hhrAAAUserOnlineTable_cache)
        {
            return NULL;
        }
    }

    pinfo = (SNMP_USER_ONLINE_INFO *)snmp_cache_get_data_by_index(hhrAAAUserOnlineTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.user_id);
    }

    switch (vp->magic)
    {
        case hhrAAAUserOnlineUsername:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.username, strlen(pinfo->data.username));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserOnlineLoginFrom:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.login_from, strlen(pinfo->data.login_from));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserOnlineLevelAuthor:
            int_value = pinfo->data.level_author;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserOnlineLevelCurrent:
            int_value = pinfo->data.level_current;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserOnlineLoginMethod:
            int_value = pinfo->data.login_method;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserOnlineTimeLogin:
            int_value = pinfo->data.time_login;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserOnlineTimeOnline:
            int_value = pinfo->data.time_online;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}

u_char *hhrAAAUserLogTable_get(struct variable *vp,
                               oid *name,
                               size_t *length,
                               int exact, size_t *var_len, WriteMethod **write_method)
{
    SNMP_USER_LOG_INFO *pinfo = NULL;
    SNMP_USER_LOG_INDEX index;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);

    index.user_id = index_int ;

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': index [%d]\n", __FILE__, __LINE__, __func__ , index.user_id);

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrAAAUserLogTable_cache)
    {
        hhrAAAUserLogTable_cache = snmp_cache_init(sizeof(SNMP_USER_LOG_INFO),
                                                   hhrAAAUserLogTable_get_data_from_ipc,
                                                   hhrAAAUserLogTable_node_lookup);

        if (NULL == hhrAAAUserLogTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (SNMP_USER_LOG_INFO *)snmp_cache_get_data_by_index(hhrAAAUserLogTable_cache , exact, &index);

    if (NULL == pinfo)
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.user_id);
    }

    switch (vp->magic)
    {
        case hhrAAAUserLogUsername:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.username, strlen(pinfo->data.username));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserLogLevel:
            int_value = pinfo->data.level;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserLogLoginMethod:
            int_value = pinfo->data.login_method;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserLogLoginFrom:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.login_from, strlen(pinfo->data.login_from));
            *var_len = strlen(str_value);
            return str_value;

        case hhrAAAUserLogLoginResult:
            int_value = pinfo->data.login_result;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserLogTimeLogin:
            int_value = pinfo->data.time_start;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case hhrAAAUserLogTimeExit:
            int_value = pinfo->data.time_exit;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}




/************************************************* H3C *****************************************************/


u_char *h3c_hhrAAALocalUserTable_get(struct variable *vp,
                                     oid *name,
                                     size_t *length,
                                     int exact, size_t *var_len, WriteMethod **write_method)
{
    SNMP_USER_CONFIG_INFO *pinfo = NULL;
    SNMP_USER_CONFIG_INDEX index;
    int ret = 0;
    int index_int = 0 ;

    /* validate the index */
    ret = ipran_snmp_int_index_get(vp, name, length, &index_int , exact);
    index.user_id = index_int;

    if (ret < 0)
    {
        return NULL;
    }

    if (NULL == hhrAAAUserConfigTable_cache)
    {
        hhrAAAUserConfigTable_cache = snmp_cache_init(sizeof(SNMP_USER_CONFIG_INFO),
                                                      hhrAAAUserConfigTable_get_data_from_ipc,
                                                      hhrAAAUserConfigTable_node_lookup);

        if (NULL == hhrAAAUserConfigTable_cache)
        {
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]:'%s': \n", __FILE__, __LINE__, __func__);
            return NULL;
        }
    }

    pinfo = (SNMP_USER_CONFIG_INFO *)snmp_cache_get_data_by_index(hhrAAAUserConfigTable_cache, exact, &index);

    if (NULL == pinfo)
    {
        return NULL;
    }

    /* get ready the next index */
    if (!exact)
    {
        ipran_snmp_int_index_set(vp, name, length, pinfo->index.user_id);
    }

    switch (vp->magic)
    {
        case h3c_hhrUserName:
            memset(str_value, 0, STRING_LEN);
            memcpy(str_value, pinfo->data.username, strlen(pinfo->data.username));
            *var_len = strlen(str_value);
            return str_value;

        case h3c_hhrPassword:
            memset(str_value, 0, STRING_LEN);
            //memcpy(str_value, pinfo->data.password, strlen(pinfo->data.password));
            //*var_len = strlen(str_value);
            *var_len = 0;
            return str_value;

        case h3c_hhrAuthMode:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case h3c_hhrUserLevel:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case h3c_hhrUserState:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case h3c_hhrUserOnlineRowStatus:
            int_value = 0;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        case h3c_hhrUserIndex:
            int_value = pinfo->index.user_id;
            *var_len = sizeof(int);
            return (u_char *)(&int_value);

        default:
            return NULL;
    }
}



/************************************************* END *****************************************************/


