/******************************************************************************
 * Filename: xxx_alarm.h
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

#ifndef _SYSTEM_GROUP_H_
#define _SYSTEM_GROUP_H_

#include <sys/types.h>
#include "asn1.h"

#define SYS_GRP_STRING_LEN    255

#define SYS_GRP_MAX_OID_LEN   24
#define IPRAN_VERS_DESC       ""
#define IPRAN_SYS_NAME        ""
#define IPRAN_SYS_CONTACT     ""
#define IPRAN_SYS_LOC         ""
#define DEFAULT_SYS_SERVICES  72

typedef struct system_group
{
    char            sysDesc[SYS_GRP_STRING_LEN + 1];
    char            sysContact[SYS_GRP_STRING_LEN + 1];
    char            sysName[SYS_GRP_STRING_LEN + 1];
    char            sysLocation[SYS_GRP_STRING_LEN + 1];
    oid             sysObjectID[SYS_GRP_MAX_OID_LEN];
    int             sysoid_len;
    int             sysServices;
};

extern struct system_group *sys_grp_value_get(void);
extern int sys_grp_contact_set(char *contact);
extern int sys_grp_sys_name_set(char *sys_name);
extern int sys_grp_location_set(char *location);

extern int init_sys_grp_value(void);
extern void sys_grp_sysobjectid_str_get(char *str_sysobjid);

#endif /* _SYSTEM_GROUP_H_ */



