/******************************************************************************
 * Filename: system_group.c
 *  Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2018.4.13  lidingcheng created
 *
******************************************************************************/

#include <string.h>
#include <lib/module_id.h>
#include <lib/devm_com.h>


#include "system_group.h"

struct system_group sys_group =
{
    IPRAN_VERS_DESC,
    IPRAN_SYS_CONTACT,
    IPRAN_SYS_NAME,
    IPRAN_SYS_LOC,
    {0},
    SYS_GRP_MAX_OID_LEN,
    DEFAULT_SYS_SERVICES
};

struct system_group *sys_grp_value_get(void)
{
    return &sys_group;
}

int sys_grp_contact_set(char *contact)
{
    int size;

    if (NULL == contact)
    {
        return -1;
    }

    size = sizeof(sys_group.sysContact);
    memset(sys_group.sysContact, 0, size);
    snprintf(sys_group.sysContact, size, "%s", contact);

    return 0 ;
}

int sys_grp_sys_name_set(char *sys_name)
{
    int size;

    if (NULL == sys_name)
    {
        return -1;
    }

    size = sizeof(sys_group.sysName);
    memset(sys_group.sysName, 0, size);
    snprintf(sys_group.sysName, size, "%s", sys_name);

    return 0;
}

int sys_grp_location_set(char *location)
{
    int size;

    if (NULL == location)
    {
        return -1;
    }

    size = sizeof(sys_group.sysLocation);
    memset(sys_group.sysLocation, 0, size);
    snprintf(sys_group.sysLocation, size, "%s", location);

    return 0;
}

void sys_grp_sysobjectid_str_get(char *str_sysobjid)
{
    char str_sysobjectid[128] = {""};
    int i = 0;

    init_sys_grp_value();

    for (i = 0; i < sys_group.sysoid_len; i++)
    {
        if (i == 0)
        {
            sprintf(str_sysobjectid, "%d" , sys_group.sysObjectID[i]) ;
        }
        else
        {
            sprintf(str_sysobjectid, "%s.%d" , str_sysobjectid , sys_group.sysObjectID[i]) ;
        }
    }

    sprintf(str_sysobjid , "%s" , str_sysobjectid);
}

int init_sys_grp_value(void)
{
    int     ret;

    sys_group.sysServices = DEFAULT_SYS_SERVICES;
    sys_group.sysoid_len  = 0;

    ret = devm_comm_get_sysObjectid(MODULE_ID_SNMPD, &sys_group.sysoid_len, sys_group.sysObjectID);

    if (0 != ret)
    {
        sys_group.sysoid_len = 2;       // default null_oid {0, 0}
        sys_group.sysObjectID[0] = 0;
        sys_group.sysObjectID[1] = 0;
    }

    devm_comm_get_dev_name(1, 0, MODULE_ID_DEVM, &sys_group.sysDesc);
}


