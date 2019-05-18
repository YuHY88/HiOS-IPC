/******************************************************************************
 * Filename: snmp_config_table.c
 *  Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.12.06  lipf created
 *
******************************************************************************/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "snmp/include/net-snmp-config.h"
#include "snmp/include/output_api.h"
#include "snmp/include/config_api.h"
#include "snmp/include/read_config.h"       /* for "internal" definitions */
#include "snmp/include/utilities.h"
#include "snmp/include/mib.h"
#include "snmp/include/parse.h"
#include "snmp/include/snmp_api.h"
#include "snmp/include/callback.h"
#include "snmp/include/snmp_config_table.h"
#include "snmp/agent/system_group.h"


#include <lib/vty.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/ospf_common.h>
#include <lib/msg_ipc.h>
#include <lib/inet_ip.h>
#include <lib/hash1.h>


extern int  write_configs(char *token, char *cptr);


struct list *snmp_community_list = NULL;
struct list *snmp_trap_list = NULL;
struct list *snmp_inform_list = NULL;
struct list *snmp_user_list = NULL;


SNMP_VERSION snmp_version = ALL;
PACKET_STATISTICS snmp_packet_statistics;

int          g_snmp_trap_enable = 1;
int          g_snmp_trap_linkup_down = 1;


//È«¾ÖµÄVPN¡¢DCN-trapÔ´IP¡¢½Ó¿Ú-trapÔ´IP¡¢Ô´IP¶ÔÓ¦½Ó¿Ú
u_int16_t    snmp_server_vpn = 0;
u_int32_t    snmp_trap_dcn_src_ip = 0;
u_int32_t    snmp_trap_if_src_ip = 0;
u_int32_t    snmp_trap_if_src = 0;



/* efm U0 hash table */
static struct hash_table snmp_U0_hash_table;

uint32_t g_snmp_U0_ipv4;



/*************  community list operation  ****************/
static COMMUNITY_ITEM *
snmp_community_new(void)
{
    return (COMMUNITY_ITEM *)XCALLOC(MTYPE_SNMPD_CONFIG, sizeof(COMMUNITY_ITEM));
}

static void
snmp_community_add(COMMUNITY_ITEM *item)
{
    COMMUNITY_ITEM *item_add;
    item_add = snmp_community_new();
    memcpy(item_add, item, sizeof(COMMUNITY_ITEM));
    listnode_add_sort(snmp_community_list, item_add);
}

COMMUNITY_ITEM *
snmp_community_lookup_snmp(COMMUNITY_ITEM *item , int exact)
{
    struct listnode  *node, *nnode;
    COMMUNITY_ITEM   *item_find;

    if (NULL == snmp_community_list || 0 == listcount(snmp_community_list))
    {
        return (NULL);
    }

    if (NULL == item || 0 == strlen(item->name))
    {
        return snmp_community_list->head->data ;
    }

    for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, item_find))
    {
        if (0 == strcmp(item_find->name, item->name))
        {
            if (1 == exact) //get
            {
                return item_find;
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


USER_ITEM *
snmp_user_lookup_snmp(USER_ITEM *item , int exact)
{
    struct listnode  *node, *nnode;
    USER_ITEM    *item_find;
    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]\n", __FUNCTION__, __LINE__);

    if (NULL == snmp_user_list || 0 == listcount(snmp_user_list))
    {
        return (NULL);
    }

    if (NULL == item || 0 == strlen(item->name))
    {
        zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]\n", __FUNCTION__, __LINE__);
        return snmp_user_list->head->data ;
    }

    for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, item_find))
    {
        if (0 == strcmp(item_find->name, item->name))
            if (1 == exact) //get
            {
                return item_find;
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

    return NULL;
}


TRAP_ITEM *
snmp_trap_lookup_snmp(TRAP_ITEM *item , int exact)
{
    struct listnode  *node, *nnode;
    TRAP_ITEM    *item_find;

    if (NULL == snmp_trap_list || 0 == listcount(snmp_trap_list))
    {
        return (NULL);
    }

    if (NULL == item  || 0 == item->ip_addr.addr[0])
    {
        return snmp_trap_list->head->data ;
    }

    zlog_debug(SNMP_DBG_MIB_GET, "%s[%d]  ip %8x\n", __FUNCTION__, __LINE__,  item->ip_addr.addr[0]);

    for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, item_find))
    {
        if ((item_find->ip_addr.addr[0] == item->ip_addr.addr[0]))
            if (1 == exact) //get
            {
                return item_find;
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

    return NULL;
}


static COMMUNITY_ITEM *
snmp_community_lookup(COMMUNITY_ITEM *item)
{
    struct listnode  *node, *nnode;
    COMMUNITY_ITEM   *item_find;

    for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, item_find))
    {
        if (0 == strcmp(item_find->name, item->name))
        {
            return item_find;
        }
    }

    return NULL;
}

void
community_del(COMMUNITY_ITEM *item)
{
    XFREE(MTYPE_SNMPD_CONFIG, item);
}

static int
snmp_community_del(COMMUNITY_ITEM *item)
{
    COMMUNITY_ITEM *item_find;
    item_find = snmp_community_lookup(item);
    listnode_delete(snmp_community_list, item_find);
    snmp_community_list->del(item_find);
    return SNMP_OK;
}


/*************  trap list operation  ****************/
static TRAP_ITEM *
snmp_trap_new(void)
{
    return (TRAP_ITEM *)XCALLOC(MTYPE_SNMPD_CONFIG, sizeof(TRAP_ITEM));
}

static void
snmp_trap_add(TRAP_ITEM *item)
{
    TRAP_ITEM *item_add;
    item_add = snmp_trap_new();
    memcpy(item_add, item, sizeof(TRAP_ITEM));
    listnode_add_sort(snmp_trap_list, item_add);
}

static TRAP_ITEM *
snmp_trap_lookup(TRAP_ITEM *item)
{
    struct listnode  *node, *nnode;
    TRAP_ITEM    *item_find;

    for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, item_find))
    {
        /*if((item_find->ip_addr.addr[0] == item->ip_addr.addr[0])\
            && (item_find->ip_addr.addr[1] == item->ip_addr.addr[1])\
            && (item_find->ip_addr.addr[2] == item->ip_addr.addr[2])\
            && (item_find->ip_addr.addr[3] == item->ip_addr.addr[3])\
            && (item_find->version == item->version)\
            && (item_find->vpn == item->vpn)\
            && (0 == strcmp(item_find->name, item->name)))
            return item_find;*/
        if ((item_find->ip_addr.addr[0] == item->ip_addr.addr[0])\
                && (item_find->ip_addr.addr[1] == item->ip_addr.addr[1])\
                && (item_find->ip_addr.addr[2] == item->ip_addr.addr[2])\
                && (item_find->ip_addr.addr[3] == item->ip_addr.addr[3]))
        {
            return item_find;
        }
    }

    return NULL;
}

void
trap_del(TRAP_ITEM *item)
{
    XFREE(MTYPE_SNMPD_CONFIG, item);
}

static int
snmp_trap_del(TRAP_ITEM *item)
{
    TRAP_ITEM *item_find;
    item_find = snmp_trap_lookup(item);

    if ((item_find->version == item->version)\
            && (item_find->vpn == item->vpn)\
            && (0 == strcmp(item_find->name, item->name)))
    {
        listnode_delete(snmp_trap_list, item_find);
        snmp_trap_list->del(item_find);
        return SNMP_OK;
    }

    return SNMP_ERROR;
}


/*************  inform list operation  ****************/
static INFORM_ITEM *
snmp_inform_new(void)
{
    return (INFORM_ITEM *)XCALLOC(MTYPE_SNMPD_CONFIG, sizeof(INFORM_ITEM));
}

static void
snmp_inform_add(INFORM_ITEM *item)
{
    INFORM_ITEM *inform_item;
    inform_item = snmp_inform_new();
    memcpy(inform_item, item, sizeof(INFORM_ITEM));
    listnode_add_sort(snmp_inform_list, inform_item);
}

static INFORM_ITEM *
snmp_inform_lookup(INFORM_ITEM *item)
{
    struct listnode  *node, *nnode;
    INFORM_ITEM  *item_find;

    for (ALL_LIST_ELEMENTS(snmp_inform_list, node, nnode, item_find))
    {
        /*if((item_find->ip_addr.addr[0] == item->ip_addr.addr[0])\
            && (item_find->ip_addr.addr[1] == item->ip_addr.addr[1])\
            && (item_find->ip_addr.addr[2] == item->ip_addr.addr[2])\
            && (item_find->ip_addr.addr[3] == item->ip_addr.addr[3])\
            && (item_find->vpn == item->vpn)\
            && (0 == strcmp(item_find->name, item->name)))*/

        if ((item_find->ip_addr.addr[0] == item->ip_addr.addr[0])\
                && (item_find->ip_addr.addr[1] == item->ip_addr.addr[1])\
                && (item_find->ip_addr.addr[2] == item->ip_addr.addr[2])\
                && (item_find->ip_addr.addr[3] == item->ip_addr.addr[3]))
        {
            return item_find;
        }
    }

    return NULL;
}

void
inform_del(INFORM_ITEM *item)
{
    XFREE(MTYPE_SNMPD_CONFIG, item);
}

static int
snmp_inform_del(INFORM_ITEM *item)
{
    INFORM_ITEM *item_find;
    item_find = snmp_inform_lookup(item);
    listnode_delete(snmp_inform_list, item_find);
    snmp_inform_list->del(item_find);
    return SNMP_OK;
}


/*************  community list operation  ****************/
static USER_ITEM *
snmp_user_new(void)
{
    return (USER_ITEM *)XCALLOC(MTYPE_SNMPD_CONFIG, sizeof(USER_ITEM));
}

static void
snmp_user_add(USER_ITEM *item)
{
    USER_ITEM *item_add;
    item_add = snmp_user_new();
    memcpy(item_add, item, sizeof(USER_ITEM));
    listnode_add_sort(snmp_user_list, item_add);
}

static USER_ITEM *
snmp_user_lookup(USER_ITEM *item)
{
    struct listnode  *node, *nnode;
    USER_ITEM    *item_find;

    for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, item_find))
    {
        if (0 == strcmp(item_find->name, item->name))
        {
            return item_find;
        }
    }

    return NULL;
}


void
user_del(USER_ITEM *item)
{
    XFREE(MTYPE_SNMPD_CONFIG, item);
}

static int
snmp_user_del(USER_ITEM *item)
{
    USER_ITEM *item_find;
    item_find = snmp_user_lookup(item);
    listnode_delete(snmp_user_list, item_find);
    snmp_user_list->del(item_find);
    return SNMP_OK;
}

/*********** snmp item list init *****************/
void
snmp_item_list_init(void)
{
    snmp_community_list = list_new();
    snmp_community_list->del = (void (*)(void *))community_del;
    snmp_community_list->cmp = (int (*)(void *, void *))snmp_community_item_cmp;

    snmp_trap_list = list_new();
    snmp_trap_list->del = (void (*)(void *))trap_del;
    snmp_trap_list->cmp = (int (*)(void *, void *))snmp_trap_item_cmp;

    snmp_inform_list = list_new();
    snmp_inform_list->del = (void (*)(void *))inform_del;
    snmp_inform_list->cmp = (int (*)(void *, void *))snmp_inform_item_cmp;

    snmp_user_list = list_new();
    snmp_user_list->del = (void (*)(void *))user_del;
    snmp_user_list->cmp = (int (*)(void *, void *))snmp_user_item_cmp;
}

//-----------------  end  --------------------//


/* add public/private for EZview, add by lipf, 2018/3/30 */
static void snmp_add_default_community(void)
{
    COMMUNITY_ITEM item;
    memset(item.name, 0, COMMUNITY_NAME_MAX_LEN + 1);
    item.attribute = RO;

    /* add public for readonly */
    memset(&item, 0, sizeof(COMMUNITY_ITEM));
    strcpy(item.name, "public");
    item.attribute = RO;

    if (NULL == snmp_config_item_get(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_add(SNMP_COMMUNITY_ITEM, &item);
        snmp_community_item_config_new(&item);
    }

    /* add private for read&write */
    memset(&item, 0, sizeof(COMMUNITY_ITEM));
    memset(item.name, 0, COMMUNITY_NAME_MAX_LEN + 1);
    strcpy(item.name, "private");
    item.attribute = RW;

    if (NULL == snmp_config_item_get(SNMP_COMMUNITY_ITEM, &item))
    {
        snmp_config_item_add(SNMP_COMMUNITY_ITEM, &item);
        snmp_community_item_config_new(&item);
    }
}


/*
 * Function: snmp_init_config
 *
 * Purpose: create an attribute-value pair list.
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */

int snmp_init_config(void)
{
    static int ret = 0;

    write_configs("agentXSocket",  "tcp:localhost:705");
    write_configs("agentaddress",  "161");
    write_configs("agentXTimeout", "30");
    write_configs("agentXRetries", "5");
    write_configs("master",        "agentx");
    write_configs("rwcommunity",   "hios");
    //write_configs("trap2sink",     "192.192.2.100:162 trap");

    write_configs("view", "operView included .1");
    write_configs("view", "operView excluded .1.3.6.1.6.3.16.1");
    write_configs("view", "operView excluded .1.3.6.1.6.3.15.1.2");
    write_configs("view", "operView excluded .1.3.6.1.4.9966.6.3.2.10.1");
    write_configs("view", "operView excluded .1.3.6.1.4.9966.6.3.2.10.2");
    write_configs("view", "operView excluded .1.3.6.1.4.9966.5.25.19.1.3");
    write_configs("view", "operView excluded .1.3.6.1.4.9966.5.25.19.1.6");
    write_configs("view", "adminView included .1");

    write_configs("group",      "admingrp");
    write_configs("access",     "admingrp \"\" usm noauth exact adminView adminView adminView");
    write_configs("createUser", "hios");
    write_configs("group",      "admingrp usm hios");

    write_configs("group",      "opergrp");
    write_configs("access",     "opergrp \"\" usm noauth exact operView operView none");

    write_configs("group",      "guestgrp");
    write_configs("access",     "guestgrp \"\" usm noauth exact operView none none");

    /*write_configs("createUser", "hios1 MD5 00000000");
    write_configs("group",      "admingrp usm hios1");
    write_configs("createUser", "hios2 MD5 00000000 DES 00000000");
    write_configs("group",      "admingrp usm hios2");
    write_configs("createUser", "hios3 MD5 00000000");
    write_configs("group",      "admingrp usm hios3");
    write_configs("createUser", "hios4 MD5 00000000 DES 00000000");
    write_configs("group",      "admingrp usm hios4");
    write_configs("trapsink",       "192.192.2.100:162 v1");
    write_configs("trap2sink",      "192.192.2.100:163 v2c");
    write_configs("trapsess", "-v 3 -u hios -e 0x010203040506 -l noAuthNoPriv 192.192.2.100:164");
    write_configs("trapsess", "-v 3 -u hios1 -e 0x010203040506 -a MD5 -A 00000000 -l authNoPriv 192.192.2.100:165");
    write_configs("trapsess", "-v 3 -u hios2 -e 0x010203040506 -a MD5 -A 00000000 -l authNoPriv 192.192.2.100:166");
    write_configs("trapsess", "-v 3 -a MD5 -A 00000000 -l authNoPriv -u hios3 -e 0x010203040506 192.192.2.100:167");
    write_configs("trapsess", "-v 3 -a MD5 -A 00000000 -x AES -X 00000000 -l authPriv -u hios4 -e 0x010203040506 192.192.2.100:168");

    write_configs("informsink",     "192.192.2.100:162 inform");*/

    if (0 == ret)
    {
        snmp_item_list_init();

        /* add public/private for EZview, add by lipf, 2018/3/30 */
        snmp_add_default_community();

        g_snmp_trap_enable = 1;
        g_snmp_trap_linkup_down = 1;
        ret++;
    }

    return SNMP_OK;
}

int snmp_trap_enable_get(void)
{
    return g_snmp_trap_enable;
}

int snmp_trap_link_enable_get(void)
{
    return g_snmp_trap_linkup_down;
}

void snmp_trap_enable_set(int enable)
{
    g_snmp_trap_enable = enable;
}

void snmp_trap_link_enable_set(int enable)
{
    g_snmp_trap_linkup_down = enable;
}

/*
 * Function: snmp_community_item_config_new
 *
 * Purpose: config the new added snmp community item
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_community_item_config_new(COMMUNITY_ITEM *item)
{
    if (RO == item->attribute)
    {
        write_configs("rocommunity", item->name);
    }
    else if (RW == item->attribute)
    {
        write_configs("rwcommunity", item->name);
    }

    return SNMP_OK;
}

/*
 * Function: snmp_community_item_config_all
 *
 * Purpose: config all snmp community items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_community_item_config_all(void)
{
    struct listnode  *node, *nnode;
    COMMUNITY_ITEM   *item;

    for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, item))
    {
        snmp_community_item_config_new(item);
    }

    return SNMP_OK;
}


/*
 * Function: snmp_trap_item_config_new
 *
 * Purpose: config the new added snmp trap item
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_trap_item_config_new(TRAP_ITEM *item)
{
//    char str[80 + TRAP_NAME_MAX_LEN];
    char str[400 + TRAP_NAME_MAX_LEN];


    USER_ITEM user_item;

    if (NULL == item)
    {
        return SNMP_ERROR;
    }

    //memset(str, '\0', (80 + TRAP_NAME_MAX_LEN));
    memset(str, '\0', (400 + TRAP_NAME_MAX_LEN));

    if (IPv4 == item->ip_addr.type)
    {
        uint32_t ipv4 = 0;
        USER_ITEM *p_user_item = NULL;

        ipv4 = item->ip_addr.addr[0];

        if ((V1 == item->version) || (V2C == item->version))
        {
            sprintf(str, "%d.%d.%d.%d:%d %s", ((ipv4 >> 24) & 0xff), \
                    ((ipv4 >> 16) & 0xff), \
                    ((ipv4 >> 8) & 0xff), \
                    (ipv4 & 0xff), \
                    item->port, \
                    item->name);

            if (V1 == item->version)
            {
                write_configs("trapsink", str);
            }
            else if (V2C == item->version)
            {
                write_configs("trap2sink", str);
            }
        }

        else if (V3 == item->version)               //reserved for v3
        {
            memset(user_item.name, '\0', USER_NAME_MAX_LEN);
            strcpy(user_item.name, item->name);
            p_user_item = snmp_config_item_get(SNMP_USER_ITEM, &user_item);

            if (NULL == p_user_item)
            {
                return SNMP_ERROR;
            }

            if (NO_AUTH == p_user_item->auth_method)
            {
                sprintf(str, "-v 3 -l noAuthNoPriv -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \
                        p_user_item->name, \
                        ((ipv4 >> 24) & 0xff), \
                        ((ipv4 >> 16) & 0xff), \
                        ((ipv4 >> 8) & 0xff), \
                        (ipv4 & 0xff), \
                        item->port);
            }
            else if (MD5 == p_user_item->auth_method)
            {
                if (NO_ENCRY == p_user_item->encry_method)
                {
//                    sprintf(str, "-v 3 -l -a MD5 -A %s authNoPriv -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \

                    sprintf(str, "-v 3 -l authNoPriv -a MD5 -A %s  -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \
                            p_user_item->auth_secret, \
                            p_user_item->name, \
                            ((ipv4 >> 24) & 0xff), \
                            ((ipv4 >> 16) & 0xff), \
                            ((ipv4 >> 8) & 0xff), \
                            (ipv4 & 0xff), \
                            item->port);
                }
                else        /* AES or DES -- reserved */
                {
//                    sprintf(str, "-v 3 -l -a MD5 -A %s -x AES -X %s authNoPriv -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \

                    sprintf(str, "-v 3 -l authPriv -a MD5 -A %s -x AES -X %s  -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \
                            p_user_item->auth_secret, \
                            p_user_item->encry_secret, \
                            p_user_item->name, \
                            ((ipv4 >> 24) & 0xff), \
                            ((ipv4 >> 16) & 0xff), \
                            ((ipv4 >> 8) & 0xff), \
                            (ipv4 & 0xff), \
                            item->port);
                }
            }
            else
            {
                if (NO_ENCRY == p_user_item->encry_method)
                {
//                    sprintf(str, "-v 3 -l -a SHA -A %s authNoPriv -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \

                    sprintf(str, "-v 3 -l authNoPriv -a SHA -A %s  -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \
                            p_user_item->auth_secret, \
                            p_user_item->name, \
                            ((ipv4 >> 24) & 0xff), \
                            ((ipv4 >> 16) & 0xff), \
                            ((ipv4 >> 8) & 0xff), \
                            (ipv4 & 0xff), \
                            item->port);
                }
                else        /* AES or DES -- reserved */
                {
//                    sprintf(str, "-v 3 -l -a SHA -A %s -x AES -X %s authNoPriv -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \

                    sprintf(str, "-v 3 -l authPriv -a SHA -A %s -x AES -X %s  -u %s -e 0x010203040506 %d.%d.%d.%d:%d", \
                            p_user_item->auth_secret, \
                            p_user_item->encry_secret, \
                            p_user_item->name, \
                            ((ipv4 >> 24) & 0xff), \
                            ((ipv4 >> 16) & 0xff), \
                            ((ipv4 >> 8) & 0xff), \
                            (ipv4 & 0xff), \
                            item->port);
                }
            }

            write_configs("trapsess", str);
        }
    }
    else if (IPv6 == item->ip_addr.type)
    {
        //ipv6 to be added
        //printf("%s[%d] : error[IPv6 to be added]\n", __FUNCTION__, __LINE__);
        //uint32_t ipv6[4] = {0, 0, 0, 0};
        return SNMP_ERROR;
    }

    return SNMP_OK;
}

/*
 * Function: snmp_trap_item_config_all
 *
 * Purpose: config all snmp trap items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_trap_item_config_all(void)
{
    struct listnode  *node, *nnode;
    TRAP_ITEM    *item;

    for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, item))
    {
        if (V1 == snmp_version)
        {
            if (V1 == item->version)
            {
                snmp_trap_item_config_new(item);
            }
        }
        else if (V2C == snmp_version)
        {
            if (V2C == item->version)
            {
                snmp_trap_item_config_new(item);
            }
        }
        else if (V3 == snmp_version)
        {
            if (V3 == item->version)
            {
                snmp_trap_item_config_new(item);
            }
        }
        else
        {
            snmp_trap_item_config_new(item);
        }
    }

    return SNMP_OK;
}

/*
 * Function: snmp_inform_item_config_new
 *
 * Purpose: config the new added snmp inform item
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_inform_item_config_new(INFORM_ITEM *item)
{
    char str[80 + TRAP_NAME_MAX_LEN];

    if (NULL == item)
    {
        return SNMP_ERROR;
    }

    memset(str, '\0', (80 + TRAP_NAME_MAX_LEN));

    if (IPv4 == item->ip_addr.type)
    {
        uint32_t ipv4 = 0;
        ipv4 = item->ip_addr.addr[0];

        sprintf(str, "%d.%d.%d.%d:%d %s", ((ipv4 >> 24) & 0xff), \
                ((ipv4 >> 16) & 0xff), \
                ((ipv4 >> 8) & 0xff), \
                (ipv4 & 0xff), \
                item->port, \
                item->name);

        write_configs("informsink", str);
    }
    else if (IPv6 == item->ip_addr.type)
    {
        //uint32_t ipv6[4] = {0, 0, 0, 0};
        //ipv6 to be added
        //printf("%s[%d] : error[IPv6 to be added]\n", __FUNCTION__, __LINE__);
        return SNMP_ERROR;
    }

    return SNMP_OK;
}



/*
 * Function: snmp_inform_item_config_all
 *
 * Purpose: config all snmp inform items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_inform_item_config_all(void)
{
    struct listnode  *node, *nnode;
    INFORM_ITEM  *item;

    for (ALL_LIST_ELEMENTS(snmp_inform_list, node, nnode, item))
    {
        snmp_inform_item_config_new(item);
    }

    return SNMP_OK;
}

/*
 * Function: snmp_user_item_config_new
 *
 * Purpose: config the new added snmp user item
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_user_item_config_new(USER_ITEM *item)
{
    char str[30 + TRAP_NAME_MAX_LEN + AUTH_SECRET_MAX_LEN + ENCRY_SECRET_MAX_LEN];

    if (NULL == item)
    {
        return SNMP_ERROR;
    }

    memset(str, '\0', (30 + TRAP_NAME_MAX_LEN + AUTH_SECRET_MAX_LEN + ENCRY_SECRET_MAX_LEN));

    switch (item->auth_method)
    {
        case NO_AUTH:
            sprintf(str, "%s", item->name);
            break;

        case MD5:
            if (NO_ENCRY == item->encry_method)
            {
                sprintf(str, "%s MD5 %s", item->name, item->auth_secret);
            }
            else if (DES == item->encry_method)
            {
                sprintf(str, "%s MD5 %s DES %s", item->name, item->auth_secret, item->encry_secret);
            }
            else
            {
                sprintf(str, "%s MD5 %s AES %s", item->name, item->auth_secret, item->encry_secret);
            }

            break;

        case SHA:
            if (NO_ENCRY == item->encry_method)
            {
                sprintf(str, "%s SHA %s", item->name, item->auth_secret);
            }
            else if (DES == item->encry_method)
            {
                sprintf(str, "%s SHA %s DES %s", item->name, item->auth_secret, item->encry_secret);
            }
            else
            {
                sprintf(str, "%s SHA %s AES %s", item->name, item->auth_secret, item->encry_secret);
            }

            break;

        default:
            break;
    }

    write_configs("createUser", str);

    memset(str, '\0', (30 + TRAP_NAME_MAX_LEN + AUTH_SECRET_MAX_LEN + ENCRY_SECRET_MAX_LEN));

    switch (item->permission)
    {
        case GUEST:
            sprintf(str, "guestgrp usm %s", item->name);
            break;

        case OPERATOR:
            sprintf(str, "opergrp usm %s", item->name);
            break;

        case ADMIN:
            sprintf(str, "admingrp usm %s", item->name);
            break;

        default:
            break;
    }

    write_configs("group", str);
    return SNMP_OK;
}

/*
 * Function: snmp_user_item_config_all
 *
 * Purpose: config all snmp user items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_user_item_config_all(void)
{
    struct listnode  *node, *nnode;
    USER_ITEM    *item;

    for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, item))
    {
        snmp_user_item_config_new(item);
    }

    return SNMP_OK;
}


/*
 * Function: snmp_item_add
 *
 * Purpose: add an snmp config_item.
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 * Remarks: Always appends the new config_item to the end of the list.
 *
 */

int snmp_config_item_add(int item_num, void *item)
{
    switch (item_num)
    {
        case SNMP_COMMUNITY_ITEM:
            snmp_community_add((COMMUNITY_ITEM *)item);
            break;

        case SNMP_TRAP_ITEM:
            snmp_trap_add((TRAP_ITEM *)item);
            break;

        case SNMP_INFORM_ITEM:
            snmp_inform_add((INFORM_ITEM *)item);
            break;

        case SNMP_USER_ITEM:
            snmp_user_add((USER_ITEM *)item);
            break;

        default:
            break;
    }

    return SNMP_OK;
}

/*
 * Function: snmp_item_delete
 *
 * Purpose: delete a snmp item.
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */

int snmp_config_item_del(int item_num, void *item)
{
    switch (item_num)
    {
        case SNMP_COMMUNITY_ITEM:
            if (snmp_community_lookup((COMMUNITY_ITEM *)item))
            {
                if (SNMP_OK == snmp_community_del((COMMUNITY_ITEM *)item))
                {
                    return SNMP_OK;
                }
            }

            break;

        case SNMP_TRAP_ITEM:
            if (snmp_trap_lookup((TRAP_ITEM *)item))
            {
                if (SNMP_OK == snmp_trap_del((TRAP_ITEM *)item))
                {
                    return SNMP_OK;
                }
            }

            break;

        case SNMP_INFORM_ITEM:
            if (snmp_inform_lookup((INFORM_ITEM *)item))
            {
                if (SNMP_OK == snmp_inform_del((INFORM_ITEM *)item))
                {
                    return SNMP_OK;
                }
            }

            break;

        case SNMP_USER_ITEM:
            if (snmp_user_lookup((USER_ITEM *)item))
            {
                if (SNMP_OK == snmp_user_del((USER_ITEM *)item))
                {
                    return SNMP_OK;
                }
            }

            break;

        default:
            break;
    }

    return SNMP_ERROR;
}

/*
 * Function: snmp_config_item_get
 *
 * Purpose: get a snmp config item
 *
 * Returns: success -> *, failure -> NULL.
 *
 */

void *snmp_config_item_get(int item_num, void *item)
{
    switch (item_num)
    {
        case SNMP_COMMUNITY_ITEM:
            return snmp_community_lookup((COMMUNITY_ITEM *)item);

        case SNMP_TRAP_ITEM:
            return snmp_trap_lookup((TRAP_ITEM *)item);

        case SNMP_INFORM_ITEM:
            return snmp_inform_lookup((INFORM_ITEM *)item);

        case SNMP_USER_ITEM:
            return snmp_user_lookup((USER_ITEM *)item);

        default:
            break;
    }

    return NULL;
}


/*
 * Function: snmp_show_init_items
 *
 * Purpose: show snmp init item, same with snmp_init_config()
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_show_init_items(int item_num, struct vty *vty)
{
    switch (item_num)
    {
        case SNMP_COMMUNITY_ITEM:
            vty_out(vty, "community : hios [ro]%s" , VTY_NEWLINE);      //hios[ro]
            break;

        case SNMP_TRAP_ITEM:

            break;

        case SNMP_USER_ITEM:
            vty_out(vty, "user      : hios [admin]%s" , VTY_NEWLINE);       //hios[ro]
            break;

        default:
            break;
    }

    return SNMP_OK;
}


/*
 * Function: snmp_show_items
 *
 * Purpose: show snmp config items
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_show_items(int item_num, struct vty *vty)
{
    unsigned int cnt_community_ro = 0;
    unsigned int cnt_community_rw = 0;

    unsigned int cnt_trap_v1 = 0;
    unsigned int cnt_trap_v2c = 0;
    unsigned int cnt_trap_v3 = 0;

    unsigned int cnt_inform = 0;

    unsigned int cnt_user_guest = 0;
    unsigned int cnt_user_operator = 0;
    unsigned int cnt_user_admin = 0;

    struct listnode *node, *nnode;
    COMMUNITY_ITEM  *community_item;
    TRAP_ITEM       *trap_item;
    INFORM_ITEM     *inform_item;
    USER_ITEM       *user_item;

    //snmp_show_init_item(item_num, vty);

    switch (item_num)
    {
        case SNMP_COMMUNITY_ITEM:
            if (V3 == snmp_version) //not support v3
            {
                return SNMP_OK;
            }

            for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, community_item))
            {
                if (RO == community_item->attribute)
                {
                    cnt_community_ro++;

                    if (1 == cnt_community_ro)
                    {
                        vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_COMMUNITY_CONFIG, VTY_NEWLINE, VTY_NEWLINE);

                        vty_out(vty, "%-16s : %s, acl : %d %s", "community[ro]", community_item->name, community_item->acl_num, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %s, acl : %d %s", " ", community_item->name, community_item->acl_num, VTY_NEWLINE);
                    }
                }
            }

            for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, community_item))
            {
                if (RW == community_item->attribute)
                {
                    cnt_community_rw++;

                    if (1 == cnt_community_rw)
                    {
                        if (0 == cnt_community_ro)
                        {
                            vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_COMMUNITY_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                        }

                        vty_out(vty, "%-16s : %s, acl : %d %s", "community[rw]", community_item->name, community_item->acl_num, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %s, acl : %d %s" , " ", community_item->name, community_item->acl_num, VTY_NEWLINE);
                    }
                }
            }

            break;

        case SNMP_TRAP_ITEM:

            if ((V1 == snmp_version) || (ALL == snmp_version))
            {
                for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, trap_item))
                {
                    if (IPv4 == trap_item->ip_addr.type)
                    {
                        if (V1 == trap_item->version)
                        {
                            cnt_trap_v1++;

                            if (1 == cnt_trap_v1)
                            {
                                vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_V1_TRAP_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                                vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s" , \
                                        "trap[v1]", \
                                        ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                        ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                        ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                        (trap_item->ip_addr.addr[0] & 0xff), \
                                        trap_item->port, \
                                        trap_item->vpn, \
                                        trap_item->name, \
                                        VTY_NEWLINE);
                            }
                            else
                            {
                                vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s", \
                                        " ", \
                                        ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                        ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                        ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                        (trap_item->ip_addr.addr[0] & 0xff), \
                                        trap_item->port, \
                                        trap_item->vpn, \
                                        trap_item->name, \
                                        VTY_NEWLINE);
                            }
                        }
                    }
                    else if (IPv6 == trap_item->ip_addr.type)
                    {
                        //vty_out (vty, "trap : %d.  [rw]%s" , trap_item->name, VTY_NEWLINE);
                        vty_out(vty, "IPv6 to be added %s", VTY_NEWLINE);
                    }
                }
            }

            /*while(NULL != trap_item)
            {

            }*/

            if ((V2C == snmp_version) || (ALL == snmp_version))
            {
                for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, trap_item))
                {
                    if (NULL != trap_item)
                    {
                        if (IPv4 == trap_item->ip_addr.type)
                        {
                            if (V2C == trap_item->version)
                            {
                                cnt_trap_v2c++;

                                if (1 == cnt_trap_v2c)
                                {
                                    if (0 == cnt_trap_v1)
                                    {
                                        vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_V2C_TRAP_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                                    }

                                    vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s" , \
                                            "trap[v2c]", \
                                            ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                            (trap_item->ip_addr.addr[0] & 0xff), \
                                            trap_item->port, \
                                            trap_item->vpn, \
                                            trap_item->name, \
                                            VTY_NEWLINE);
                                }
                                else
                                {
                                    vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s", \
                                            " ", \
                                            ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                            (trap_item->ip_addr.addr[0] & 0xff), \
                                            trap_item->port, \
                                            trap_item->vpn, \
                                            trap_item->name, \
                                            VTY_NEWLINE);
                                }
                            }
                        }
                        else if (IPv6 == trap_item->ip_addr.type)
                        {
                            //vty_out (vty, "trap : %d.  [rw]%s" , trap_item->name, VTY_NEWLINE);
                            vty_out(vty, "IPv6 to be added %s", VTY_NEWLINE);
                        }
                    }
                }
            }

            if ((V3 == snmp_version) || (ALL == snmp_version))
            {
                for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, trap_item))
                {
                    if (NULL != trap_item)
                    {
                        if (IPv4 == trap_item->ip_addr.type)
                        {
                            if (V3 == trap_item->version)
                            {
                                cnt_trap_v3++;

                                if (1 == cnt_trap_v3)
                                {
                                    if ((0 == cnt_trap_v1) && (0 == cnt_trap_v2c))
                                    {
                                        vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_V3_TRAP_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                                    }

                                    vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s" , \
                                            "trap[v3]", \
                                            ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                            (trap_item->ip_addr.addr[0] & 0xff), \
                                            trap_item->port, \
                                            trap_item->vpn, \
                                            trap_item->name, \
                                            VTY_NEWLINE);
                                }
                                else
                                {
                                    vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s", \
                                            " ", \
                                            ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                            ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                            (trap_item->ip_addr.addr[0] & 0xff), \
                                            trap_item->port, \
                                            trap_item->vpn, \
                                            trap_item->name, \
                                            VTY_NEWLINE);
                                }
                            }
                        }
                        else if (IPv6 == trap_item->ip_addr.type)
                        {
                            //vty_out (vty, "trap : %d.  [rw]%s" , trap_item->name, VTY_NEWLINE);
                            vty_out(vty, "IPv6 to be added %s", VTY_NEWLINE);
                        }
                    }
                }
            }

            break;

        case SNMP_INFORM_ITEM:
            if ((V2C != snmp_version) && (ALL != snmp_version))
            {
                return SNMP_OK;
            }

            for (ALL_LIST_ELEMENTS(snmp_inform_list, node, nnode, inform_item))
            {
                if (IPv4 == inform_item->ip_addr.type)
                {
                    cnt_inform++;

                    if (1 == cnt_inform)
                    {
                        vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_INFORM_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                        vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s" , \
                                "inform", \
                                ((inform_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((inform_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((inform_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (inform_item->ip_addr.addr[0] & 0xff), \
                                inform_item->port, \
                                inform_item->vpn, \
                                inform_item->name, \
                                VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %d.%d.%d.%d:%d[%d] %s %s" , \
                                " ", \
                                ((inform_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((inform_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((inform_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (inform_item->ip_addr.addr[0] & 0xff), \
                                inform_item->port, \
                                inform_item->vpn, \
                                inform_item->name, \
                                VTY_NEWLINE);
                    }
                }
                else if (IPv6 == inform_item->ip_addr.type)
                {
                    //vty_out (vty, "trap : %d.  [rw]%s" , trap_item->name, VTY_NEWLINE);
                    vty_out(vty, "IPv6 to be added %s", VTY_NEWLINE);
                }
            }

            break;

        case SNMP_USER_ITEM:
            if ((V3 != snmp_version) && (ALL != snmp_version))
            {
                return SNMP_OK;
            }

            for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, user_item))
            {
                if (ADMIN == user_item->permission)
                {
                    cnt_user_admin++;

                    if (1 == cnt_user_admin)
                    {
                        vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_USER_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                        vty_out(vty, "%-16s : %s %s", "user[admin]", user_item->name, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %s %s" , " ", user_item->name, VTY_NEWLINE);
                    }
                }
            }

            for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, user_item))
            {
                if (OPERATOR == user_item->permission)
                {
                    cnt_user_operator++;

                    if (1 == cnt_user_operator)
                    {
                        if (0 == cnt_user_admin)
                        {
                            vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_USER_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                        }

                        vty_out(vty, "%-16s : %s %s" , "user[operator]", user_item->name, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %s %s" , " ", user_item->name, VTY_NEWLINE);
                    }
                }
            }

            for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, user_item))
            {
                if (GUEST == user_item->permission)
                {
                    cnt_user_guest++;

                    if (1 == cnt_user_guest)
                    {
                        if ((0 == cnt_user_admin) && (0 == cnt_user_operator))
                        {
                            vty_out(vty, "%s%s %s%s", VTY_NEWLINE, STR_SHOW_USER_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
                        }

                        vty_out(vty, "%-16s : %s %s", "user[guest]", user_item->name, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, "%-16s : %s %s" , " ", user_item->name, VTY_NEWLINE);
                    }
                }
            }

            if (cnt_user_admin || cnt_user_operator || cnt_user_guest)
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            break;

        default:
            break;
    }

    return SNMP_OK;
}


/*
 * Function: snmp_write_config_items
 *
 * Purpose: write config items to startup.conf
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */
int snmp_write_config_items(struct vty *vty)
{
    COMMUNITY_ITEM  *community_item = NULL;
    TRAP_ITEM           *trap_item = NULL;
    USER_ITEM           *user_item = NULL;
    struct listnode  *node, *nnode;

    //add user items to startup.conf
    for (ALL_LIST_ELEMENTS(snmp_user_list, node, nnode, user_item))
    {
        switch (user_item->auth_method)
        {
            case NO_AUTH:
                vty_out(vty, " snmp user %s %s", user_item->name, VTY_NEWLINE);
                break;

            case MD5:
                if (NO_ENCRY == user_item->encry_method)
                    vty_out(vty, " snmp user %s auth md5 auth-password %s %s",
                            user_item->name, user_item->auth_secret, VTY_NEWLINE);
                else
                    vty_out(vty, " snmp user %s auth md5 auth-password %s priv des priv-password %s %s",
                            user_item->name, user_item->auth_secret, user_item->encry_secret, VTY_NEWLINE);

                break;

            case SHA:
                if (NO_ENCRY == user_item->encry_method)
                    vty_out(vty, " snmp user %s auth sha auth-password %s %s",
                            user_item->name, user_item->auth_secret, VTY_NEWLINE);
                else
                    vty_out(vty, " snmp user %s auth sha auth-password %s priv des priv-password %s %s",
                            user_item->name, user_item->auth_secret, user_item->encry_secret, VTY_NEWLINE);

                break;

            default:
                break;
        }

        switch (user_item->permission)
        {
            case GUEST:
                vty_out(vty, " snmp user %s privilege guest %s" , user_item->name, VTY_NEWLINE);
                break;

            case OPERATOR:
                vty_out(vty, " snmp user %s privilege operator %s" , user_item->name, VTY_NEWLINE);
                break;

            case ADMIN:
                vty_out(vty, " snmp user %s privilege admin %s" , user_item->name, VTY_NEWLINE);
                break;

            default:
                break;
        }
    }

    /* add by lipf, 2018/3/30
     * if find public,  do nothing; else write "no snmp community public"
     * if find private, do nothing; else write "no snmp community private"
     */
    int public_found = 0;
    int private_found = 0;

    //add community items to startup.conf

    for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, community_item))
    {
        if (NULL != community_item)
        {
            if (0 == strcmp(community_item->name, "public"))    //find public
            {
                public_found = 1;
            }

            if (0 == strcmp(community_item->name, "private"))   //find private
            {
                private_found = 1;
            }


            if (RO == community_item->attribute)
            {
                if (0 == community_item->acl_num)
                {
                    vty_out(vty, " snmp community %s level ro %s" , community_item->name, VTY_NEWLINE);
                }
            }
            else if (RW == community_item->attribute)
            {
                if (0 == community_item->acl_num)
                {
                    vty_out(vty, " snmp community %s level rw %s" , community_item->name, VTY_NEWLINE);
                }
            }
        }
    }

    if (0 == public_found)  //not find public, write "no snmp community public"
    {
        vty_out(vty, " no snmp community public %s" , VTY_NEWLINE);
    }

    if (0 == private_found) //not find private, write "no snmp community private"
    {
        vty_out(vty, " no snmp community private %s" , VTY_NEWLINE);
    }


    //add trap items to startup.conf
    for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, trap_item))
    {
        if (IPv4 == trap_item->ip_addr.type)
        {
            switch (trap_item->version)
            {
                case V1:
                    if (0 == trap_item->vpn)
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d version v1 community %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->name, \
                                VTY_NEWLINE);
                    else
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d vpn %d version v1 community %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->vpn, \
                                trap_item->name, \
                                VTY_NEWLINE);

                    break;

                case V2C:
                    if (0 == trap_item->vpn)
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d version v2c community %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->name, \
                                VTY_NEWLINE);
                    else
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d vpn %d version v2c community %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->vpn, \
                                trap_item->name, \
                                VTY_NEWLINE);

                    break;

                case V3:
                    if (0 == trap_item->vpn)
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d version v3 user %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->name, \
                                VTY_NEWLINE);
                    else
                        vty_out(vty, " snmp server %d.%d.%d.%d udp-port %d vpn %d version v3 user %s %s", \
                                ((trap_item->ip_addr.addr[0] >> 24) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 16) & 0xff), \
                                ((trap_item->ip_addr.addr[0] >> 8) & 0xff), \
                                (trap_item->ip_addr.addr[0] & 0xff), \
                                trap_item->port, \
                                trap_item->vpn, \
                                trap_item->name, \
                                VTY_NEWLINE);

                    break;

                default:
                    break;
            }
        }
        else if (IPv6 == trap_item->ip_addr.type)
        {
            //vty_out (vty, "trap : %d.  [rw]%s" , trap_item->name, VTY_NEWLINE);
            vty_out(vty, " IPv6 to be added %s", VTY_NEWLINE);
        }
    }

    switch (snmp_version)
    {
        case V1:
            vty_out(vty, " snmp version v1 %s" , VTY_NEWLINE);
            break;

        case V2C:
            vty_out(vty, " snmp version v2c %s" , VTY_NEWLINE);
            break;

        case V3:
            vty_out(vty, " snmp version v3 %s" , VTY_NEWLINE);
            break;

        case ALL:
            vty_out(vty, " snmp version all %s" , VTY_NEWLINE);
            break;

        default:
            break;
    }

    if (0 != snmp_server_vpn)
    {
        vty_out(vty, " snmp vpn-instance %d %s" , snmp_server_vpn , VTY_NEWLINE);
    }

	char if_name[100] = {'\0'};
	char ipv4_str[16] = {'\0'};
	
	if (snmp_trap_if_src)
	{
		if(IFM_TYPE_IS_METHERNET(snmp_trap_if_src))
		{
			ifm_get_name_by_ifindex(snmp_trap_if_src, if_name);
			vty_out(vty, " snmp trap source interface %s %s" , if_name, VTY_NEWLINE);
		}
	}
	else if(snmp_trap_if_src_ip)
	{
		uint32_t ip_temp = htonl(snmp_trap_if_src_ip);
		if(inet_ntop(AF_INET, &ip_temp, ipv4_str, sizeof(ipv4_str)) != NULL)
		{
			vty_out(vty, " snmp trap source ip %s %s" , ipv4_str, VTY_NEWLINE);
		}
	}

    return SNMP_OK;
}

/*
 * Function: snmp_show_version
 *
 * Purpose: show snmp version
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */

int snmp_show_version(struct vty *vty)
{
    switch (snmp_version)
    {
        case V1:
            vty_out(vty, "%-16s : %s %s" , "version", "v1", VTY_NEWLINE);
            break;

        case V2C:
            vty_out(vty, "%-16s : %s %s" , "version", "v2c", VTY_NEWLINE);
            break;

        case V3:
            vty_out(vty, "%-16s : %s %s" , "version", "v3", VTY_NEWLINE);
            break;

        case ALL:
            vty_out(vty, "%-16s : %s %s" , "version", "all", VTY_NEWLINE);
            break;

        default:
            break;
    }

    return SNMP_OK;
}


/*
 * Function: snmp_show_all_items
 *
 * Purpose: show snmp config table
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */

int snmp_show_all_items(struct vty *vty)
{
    int trap_enable;
    int trap_linkup_down;

    trap_enable = snmp_trap_enable_get();
    trap_linkup_down = snmp_trap_link_enable_get();

    vty_out(vty, "%s %s%s", STR_SHOW_COMMON_CONFIG, VTY_NEWLINE, VTY_NEWLINE);
    snmp_show_version(vty);

    if (1 == trap_enable)
    {
        vty_out(vty, "%-16s : %s %s", "trap", "enable", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-16s : %s %s", "trap", "disable", VTY_NEWLINE);
    }

    if (1 == trap_linkup_down)
    {
        vty_out(vty, "%-16s: %s %s", "trap link-up/down", "enable", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "%-16s: %s %s", "trap link-up/down", "disable", VTY_NEWLINE);
    }

    snmp_show_items(SNMP_COMMUNITY_ITEM, vty);
    snmp_show_items(SNMP_TRAP_ITEM, vty);
    //snmp_show_items(SNMP_INFORM_ITEM, vty);
    snmp_show_items(SNMP_USER_ITEM, vty);

#if 0	//ä¸ä¿å­˜vpnï¼Œé˜²æ­¢å¤šä½™çš„é…ç½®é”™è¯¯
	if(snmp_server_vpn)
	{
	    vty_out(vty, "%-16s : %u %s" , "vpn", snmp_server_vpn, VTY_NEWLINE);
	}
#endif
	
#if 1
	if(!snmp_trap_if_src)
	{
		vty_out(vty, "%-16s : %s %s" , "trap source port", "---", VTY_NEWLINE);;
	}
	else if(IFM_TYPE_IS_METHERNET(snmp_trap_if_src))
	{
		if(IFM_SUBPORT_ID_GET(snmp_trap_if_src))
		{
			vty_out(vty, "%-16s : %u/%u/%u.%u %s" , "trap source port", IFM_UNIT_ID_GET(snmp_trap_if_src),
				IFM_SLOT_ID_GET(snmp_trap_if_src), IFM_PORT_ID_GET(snmp_trap_if_src), IFM_SUBPORT_ID_GET(snmp_trap_if_src), VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-16s : %u/%u/%u %s" , "trap source port", IFM_UNIT_ID_GET(snmp_trap_if_src),
				IFM_SLOT_ID_GET(snmp_trap_if_src), IFM_PORT_ID_GET(snmp_trap_if_src), VTY_NEWLINE);
		}
	}
	else if(IFM_TYPE_IS_LOOPBCK(snmp_trap_if_src))
	{
		vty_out(vty, "%-16s : %u [loopback] %s" , "trap source port", IFM_SUBPORT_ID_GET(snmp_trap_if_src), VTY_NEWLINE);
	}

	//vty_out(vty, "%-16s : %s %s" , "trap source ip", inet_ntoa(snmp_trap_if_src_ip), VTY_NEWLINE);
	vty_out(vty, "%-16s : %u.%u.%u.%u %s" , "trap source ip", 
		(snmp_trap_if_src_ip >> 24) & 0xff,
		(snmp_trap_if_src_ip >> 16) & 0xff,
		(snmp_trap_if_src_ip >>  8) & 0xff,
		(snmp_trap_if_src_ip >>  0) & 0xff, VTY_NEWLINE);
#endif

    vty_out(vty, "%s %s", STR_SHOW_TAIL, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    
    return SNMP_OK;
}

/*
 * Function: snmp_community_item_cmp
 *
 * Purpose: compare community item value
 *
 * Returns: val1 < val2, return -1
 *          val1 = val2, return 0
 *          val1 > val2, return 1
 *
 */
int snmp_community_item_cmp(COMMUNITY_ITEM *val1, COMMUNITY_ITEM *val2)
{
    assert((NULL != val1) || (NULL != val2));
    return (strcmp(val1->name, val2->name));
}


/*
 * Function: snmp_trap_item_cmp
 *
 * Purpose: compare trap item value
 *
 * Returns: val1 < val2, return -1
 *          val1 = val2, return 0
 *          val1 > val2, return 1
 *
 */
int snmp_trap_item_cmp(TRAP_ITEM *val1, TRAP_ITEM *val2)
{
    assert((NULL != val1) || (NULL != val2));

    if ((NULL == val1) && (NULL != val2))
    {
        return -1;
    }
    else if ((NULL != val1) && (NULL == val2))
    {
        return 1;
    }
    else
    {
        if (val1->version > val2->version)
        {
            return 1;
        }
        else if (val1->version == val2->version)
        {
            if (val1->ip_addr.addr[0] > val2->ip_addr.addr[0])
            {
                return 1;
            }
            else if (val1->ip_addr.addr[0] == val2->ip_addr.addr[0])
            {
                if (val1->port > val2->port)
                {
                    return 1;
                }
                else if (val1->port == val2->port)
                {
                    if (val1->vpn > val2->vpn)
                    {
                        return 1;
                    }
                    else if (val1->vpn == val2->vpn)
                    {
                        return (strcmp(val1->name, val2->name));
                    }

                    if (val1->vpn < val2->vpn)
                    {
                        return -1;
                    }
                }

                if (val1->port < val2->port)
                {
                    return -1;
                }
            }
            else
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
}

/*
 * Function: snmp_inform_item_cmp
 *
 * Purpose: compare inform item value
 *
 * Returns: val1 < val2, return -1
 *          val1 = val2, return 0
 *          val1 > val2, return 1
 *
 */
int snmp_inform_item_cmp(INFORM_ITEM *val1, INFORM_ITEM *val2)
{
    assert((NULL != val1) || (NULL != val2));

    if ((NULL == val1) && (NULL != val2))
    {
        return -1;
    }
    else if ((NULL != val1) && (NULL == val2))
    {
        return 1;
    }
    else
    {
        if (val1->ip_addr.addr[0] > val2->ip_addr.addr[0])
        {
            return 1;
        }
        else if (val1->ip_addr.addr[0] == val2->ip_addr.addr[0])
        {
            if (val1->port > val2->port)
            {
                return 1;
            }
            else if (val1->port == val2->port)
            {
                if (val1->vpn > val2->vpn)
                {
                    return 1;
                }
                else if (val1->vpn == val2->vpn)
                {
                    return (strcmp(val1->name, val2->name));
                }

                if (val1->vpn < val2->vpn)
                {
                    return -1;
                }
            }

            if (val1->port < val2->port)
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }
}


/*
 * Function: snmp_user_item_cmp
 *
 * Purpose: compare user item value
 *
 * Returns: val1 < val2, return -1
 *          val1 = val2, return 0
 *          val1 > val2, return 1
 *
 */
int snmp_user_item_cmp(USER_ITEM *val1, USER_ITEM *val2)
{
    assert((NULL != val1) || (NULL != val2));

    if ((NULL == val1) && (NULL != val2))
    {
        return -1;
    }
    else if ((NULL != val1) && (NULL == val2))
    {
        return 1;
    }
    else
    {
        if (val1->permission < val2->permission)
        {
            return 1;
        }
        else if (val1->permission == val2->permission)
        {
            return (strcmp(val1->name, val2->name));
        }
        else
        {
            return -1;
        }
    }
}


/*
 * Function: snmp_trap_v3_del
 *
 * Purpose: delete a snmp v3 trap through name.
 *
 * Returns: success -> SNMP_OK, failure -> SNMP_ERROR.
 *
 */

int snmp_trap_v3_del(TRAP_ITEM *item)
{
    struct listnode  *node, *nnode;
    TRAP_ITEM    *item_find;

    if (V3 != item->version)
    {
        return SNMP_ERROR;
    }

    for (ALL_LIST_ELEMENTS(snmp_trap_list, node, nnode, item_find))
    {
        if ((0 == strcmp(item_find->name, item->name))
                && (V3 == item_find->version))
        {
            snmp_trap_del(item_find);
        }
    }

    return SNMP_OK;
}


void sys_grp_write(struct vty *vty)
{
    COMMUNITY_ITEM  *community_item = NULL;
    struct listnode  *node, *nnode;

    struct system_group sys_group;
    int trap_enable;
    int trap_linkup_down;

    trap_enable = snmp_trap_enable_get();
    trap_linkup_down = snmp_trap_link_enable_get();

    memcpy(&sys_group, sys_grp_value_get(), sizeof(struct system_group));

    if (0 != strlen(sys_group.sysContact))
    {
        vty_out(vty, "snmp-agent sys-info contact %s%s", sys_group.sysContact, VTY_NEWLINE);
    }

    if (0 != strlen(sys_group.sysName))
    {
        vty_out(vty, "snmp-agent sys-info sysname %s%s", sys_group.sysName, VTY_NEWLINE);
    }

    if (0 != strlen(sys_group.sysLocation))
    {
        vty_out(vty, "snmp-agent sys-info location %s%s", sys_group.sysLocation, VTY_NEWLINE);
    }

    if (trap_enable == 1)
    {
        vty_out(vty, "snmp-agent trap enable%s",  VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "undo snmp-agent trap enable%s",  VTY_NEWLINE);
    }

    if (trap_linkup_down == 1)
    {
        vty_out(vty, "enable snmp trap updown%s",  VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "undo enable snmp trap updown%s",  VTY_NEWLINE);
    }

    //write h3c communities
    for (ALL_LIST_ELEMENTS(snmp_community_list, node, nnode, community_item))
    {
        if (NULL != community_item)
        {
            if (RO == community_item->attribute)
            {
                if (0 != community_item->acl_num)
                {
                    vty_out(vty, "snmp-agent community read %s acl %d%s" , community_item->name, community_item->acl_num , VTY_NEWLINE);
                }
            }
            else if (RW == community_item->attribute)
            {
                if (0 != community_item->acl_num)
                {
                    vty_out(vty, "snmp-agent community write %s acl %d%s" , community_item->name, community_item->acl_num, VTY_NEWLINE);
                }
            }
        }
    }
}



/* Ö¸¶¨trapµÄÔ´½Ó¿Ú£¬²¢Á¢¼´»ñÈ¡½Ó¿ÚIP£¬²¢×¢²á½Ó¿ÚIP±ä»¯ÊÂ¼þ */
int snmp_trap_source_if_cfg(struct vty *vty, uint32_t ifindex)
{
	struct ifm_l3 pifm;
	memset(&pifm, 0, sizeof(struct ifm_l3));

	if(ifm_get_l3if(ifindex, MODULE_ID_SNMPD, &pifm) < 0)
	{
		vty_error_out(vty, "L3 interface not exist!%s", VTY_NEWLINE);
		//snmp_trap_if_src_ip = 0;
		return 0;
	}
	else
	{
		snmp_trap_if_src = ifindex;
		snmp_trap_if_src_ip = pifm.ipv4[0].addr;
	}
	
	/* ×¢²á¶Ë¿ÚIPÐÞ¸ÄÊÂ¼þ */
	ifm_event_register_with_ifindex(IFNET_EVENT_IP_ADD, MODULE_ID_SNMPD_TRAP, ifindex);
	ifm_event_register_with_ifindex(IFNET_EVENT_IP_DELETE, MODULE_ID_SNMPD_TRAP, ifindex);

	return 1;
}

int snmp_no_trap_source_if_cfg(void)
{
	snmp_trap_if_src = 0;
	snmp_trap_if_src_ip = 0;

	/* È¥×¢²á¶Ë¿ÚIPÐÞ¸ÄÊÂ¼þ */
	no_ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_SNMPD_TRAP, 0);
	no_ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_SNMPD_TRAP, 0);

	return 1;
}


/* Ö±½ÓÖ¸¶¨trapµÄÔ´IP£¬²¢Á¢¼´½â³ýÓë¶Ë¿ÚµÄ¶ÔÓ¦¹ØÏµ */
int snmp_trap_source_ip_cfg(struct vty *vty, uint32_t ip)
{
	if(snmp_trap_if_src)
	{
		/* È¥×¢²á¶Ë¿ÚIPÐÞ¸ÄÊÂ¼þ */
		no_ifm_event_register(IFNET_EVENT_IP_ADD, MODULE_ID_SNMPD_TRAP, 0);
		no_ifm_event_register(IFNET_EVENT_IP_DELETE, MODULE_ID_SNMPD_TRAP, 0);
	}
	
	snmp_trap_if_src = 0;
	snmp_trap_if_src_ip = ntohl(ip);

	return 1;
}




int snmp_handle_route_msg(struct ipc_mesg_n *pmsg)
{
	struct ifm_event *pevent = (struct ifm_event *)pmsg->msg_data;
	if(NULL == pevent)
	{
		return 0;
	}
	
	if(IFNET_EVENT_IP_ADD == pmsg->msghdr.msg_subtype)
	{
		snmp_trap_if_src_ip = pevent->ipaddr.addr.ipv4;
	}
	else if(IFNET_EVENT_IP_DELETE == pmsg->msghdr.msg_subtype)
	{
		snmp_trap_if_src_ip = 0;
	}

	zlog_debug(SNMP_DBG_TRAP, "%s[%d] : event(%d:%d), ip (%x)\n", __func__, __LINE__, pmsg->msghdr.msg_subtype, IFNET_EVENT_IP_ADD,
		snmp_trap_if_src_ip);

	return 1;
}



/**************************************************************************************/

/* hash key äº§ç”Ÿæ–¹æ³• */
static unsigned int snmp_U0_compute_hash(void *hash_key)
{
	if(NULL == hash_key)
	{
		zlog_debug (SNMP_DBG_U0, "%s[%d] -> %s : hash_key is NULL!\n",\
			__FILE__, __LINE__, __func__);
		return 0;
	}

    return (unsigned int)hash_key;
}


/* hash key æ¯”è¾ƒæ–¹æ³• */
static int snmp_U0_compare_hash(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = (struct hash_bucket *)item;

	if(NULL == pbucket || NULL == hash_key)
	{
		return SNMP_ERROR;
	}

	if (pbucket->hash_key == hash_key)
    {
        return SNMP_OK;
    }
    return SNMP_ERROR;
}




/* åˆå§‹åŒ–U0 hashè¡¨ */
void snmp_U0_hash_init(void)
{
	hios_hash_init(&snmp_U0_hash_table, 64, snmp_U0_compute_hash, snmp_U0_compare_hash);
}

/* åœ¨U0çš„hashè¡¨ä¸­æ·»åŠ æ–°çš„è¡¨é¡¹ */
static int snmp_U0_hash_add(U0_ITEM *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	U0_ITEM *pinfo_new = (U0_ITEM *)XMALLOC(MTYPE_SNMPD_CONFIG, sizeof(U0_ITEM));
	if(!pinfo_new)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return SNMP_MALLOC_FAIL;
	}

	memcpy(pinfo_new, pinfo, sizeof(U0_ITEM));

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return SNMP_MALLOC_FAIL;
	}

	if(snmp_U0_hash_get_ifindex_by_ip(pinfo->ipv4.s_addr))
	{
		zlog_err("%s[%d] : add repeated(%x|%x)\n", __func__, __LINE__, pinfo->ipv4.s_addr, pinfo->ifindex);
		return SNMP_OK;
	}
	
	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->ipv4.s_addr;
	pbucket->data = (void *)pinfo_new;

	zlog_debug(SNMP_DBG_U0, "%s[%d] : info(%x : %x)\n", __func__, __LINE__, pinfo->ipv4.s_addr, pinfo->ifindex);
	
	if(hios_hash_add(&snmp_U0_hash_table, pbucket) != 0)
	{
		XFREE(MTYPE_SNMPD_CONFIG, pinfo);
		XFREE(MTYPE_HASH_BACKET, pbucket);
		zlog_debug (SNMP_DBG_U0, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			snmp_U0_hash_table.num_entries);
		return SNMP_OVERSIZE;
	}
	return SNMP_OK;
}

uint32_t snmp_U0_hash_get_ifindex_by_ip(uint32_t s_addr)
{
	struct hash_bucket *cursor = NULL;
	U0_ITEM *pinfo = NULL;

	cursor = hios_hash_find(&snmp_U0_hash_table, (void *)s_addr);
	if(NULL == cursor)
		return 0;

	pinfo = (U0_ITEM *)cursor->data;
	if(s_addr == pinfo->ipv4.s_addr)
		return pinfo->ifindex;
	else
		return 0;
}


static int snmp_U0_hash_del(uint32_t s_addr)
{
	struct hash_bucket *cursor = NULL;
	U0_ITEM *pinfo = NULL;

	zlog_debug(SNMP_DBG_U0, "%s[%d] : ---- ipv4(%x)\n", __func__, __LINE__, s_addr);

	cursor = hios_hash_find(&snmp_U0_hash_table, (void *)s_addr);
	if(NULL == cursor)
		return SNMP_NOT_FOUND;

	pinfo = (U0_ITEM *)cursor->data;
		
	XFREE(MTYPE_SNMPD_CONFIG, pinfo);
	hios_hash_delete(&snmp_U0_hash_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);

	return SNMP_ERROR;
}


int snmp_handle_ospf_msg(struct ipc_mesg_n *pmsg)
{
	if(NULL == pmsg)
	{
		return 0;
	}
	
	struct ipc_msghdr_n *phdr = &pmsg->msghdr;
	struct u0_device_info *pU0_info = (struct u0_device_info *)pmsg->msg_data;
	
	U0_ITEM info;
	info.ipv4.s_addr = pU0_info->ne_ip.s_addr;
	info.ifindex 	 = pU0_info->ifindex;
	
	switch(phdr->msg_type)
	{
		case IPC_TYPE_EFM:

			if(IPC_OPCODE_ADD == phdr->opcode)
			{
				snmp_U0_hash_add(&info);
			}
			else if(IPC_OPCODE_DELETE == phdr->opcode)
			{
				snmp_U0_hash_del(pU0_info->ne_ip.s_addr);
			}
			
			break;

		default:
			break;
	}
}




