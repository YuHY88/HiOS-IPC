/******************************************************************************
 * Filename: rmond_config.h
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: rmond_config functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/

#ifndef __ZEBOS_RMON_CONFIG_H_
#define __ZEBOS_RMON_CONFIG_H_

typedef enum
{
    PAL_FALSE = 0,
    PAL_TRUE = 1
}   pal;

struct rmon_master
{
    struct route_table *etherStats_table;
    struct route_table *historyControl_table;
    struct route_table *alarm_table;
    struct route_table *event_table;
    struct route_table *interface_table;
}   rmon_master;

struct rmon_interface
{
    u_int32_t ifindex;

    struct rmon_if_stats if_init_cntrs;
    struct rmon_if_stats if_hist_cntrs;
};

#endif /* ifndef ZEBOS_STP_CONG_H_ */
