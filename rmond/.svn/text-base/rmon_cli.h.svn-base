/******************************************************************************
 * Filename: rmond_cli.h
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: rmond_cli functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/
#ifndef __RMON_CLI_H__
#define __RMON_CLI_H__

enum RMON_MTYPE
{
    RMON_MTYPE_NON = 0,   
    RMON_MTYPE_HAL = 1,   
    RMON_MTYPE_SNMP = 2,
    RMON_MTYPE_MAX = 6,   
};

int     rmon_config_write(struct vty *vty);
void    rmon_cli_init();

#endif
