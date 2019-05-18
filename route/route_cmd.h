/**
 * @file      : route_cmd.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:38:32
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_CMD_H
#define HIOS_ROUTE_CMD_H


extern void route_static_cmd_init(void);
extern int route_valid_ip_mask(int ip, int mask);
extern void route_set_router_id(void);
extern void routev6_set_router_id(void);
extern int routev6_check_ipv6_str(const char *src);


#endif

#define ROUTE_DEBUG_TYPE_ALL (1<<0)


