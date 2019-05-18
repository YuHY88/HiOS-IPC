/**
 * @file      : route_static.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:40:29
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_STATIC_ROUTE_H
#define HIOS_STATIC_ROUTE_H

#include <lib/route_com.h>

/* Hash of ipv4 静态路由 table */
extern struct hash_table static_route_table;

/* Hash of ipv6 静态路由 table */
extern struct hash_table static_routev6_table;


/* IPv4 静态路由表函数原型 */
extern void route_static_table_init(unsigned int size);
extern struct route_static *route_static_create(void);
extern int route_static_add(struct route_static *proute);
extern int route_static_delete(struct route_static *proute);
extern void route_static_free(struct route_static *proute);
extern struct route_static * route_static_lookup(struct route_static *proute);
extern int route_static_get_bulk(struct route_static *psroute, struct route_static sroute[], uint32_t index_flag);
extern void route_static_set_nhp(struct nhp_entry *pnhp, struct route_static *sroute);
extern int route_static_update_routing(void);
extern void route_static_process_routing(uint32_t ifindex);
extern void route_static_process_l3vpn_delete(uint16_t vrf_id);

/* IPv6 静态路由表函数原型 */
extern void routev6_static_set_nhp(struct nhp_entry *pnhp, struct route_static *sroute);
extern void routev6_static_table_init(unsigned int size);
extern struct route_static *routev6_static_create(void);
extern int routev6_static_add(struct route_static *proute);
extern int routev6_static_delete(struct route_static *proute);
extern void routev6_static_free(struct route_static *proute);
extern struct route_static *routev6_static_lookup(struct route_static *proute);
extern int routev6_static_get_bulk(struct route_static *psroute, struct route_static sroute[]);
extern int routev6_static_update_routing(void);


#endif


