/**
 * @file      : route.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:49:30
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_H
#define HIOS_ROUTE_H

#include <lib/route_com.h>
#include "route_rib.h"

#define ROUTE_CONVER_NUM       3
#define ROUTE_STATUS_INACTIVE  0
#define ROUTE_STATUS_ACTIVE    1
#define NHP_NUM_ONE            1
#define NHP_NUM_TWO            2


enum ROUTE_SUBTYPE
{
    ROUTE_SUBTYPE_IPV4 = 0,
    ROUTE_SUBTYPE_IPV6,
    ROUTE_SUBTYPE_ROUTER_ID,       // IPV4 router_id
    ROUTE_SUBTYPE_ROUTERV6_ID,     // IPV6 router_id
    ROUTE_SUBTYPE_ROUTE_LIMIT,     // IPV4 route number limit
    ROUTE_SUBTYPE_ROUTEV6_LIMIT,   // IPV6 route number limit
};


extern struct ptree *route_tree[L3VPN_SIZE+1];
extern struct ptree *routev6_tree[L3VPN_SIZE+1];


/* IPv4 路由表函数原型 */
extern int route_add(struct rib_entry *prib);
extern int route_update(struct rib_entry *prib);
extern int route_delete(struct inet_prefix *prefix, uint16_t vpn);
extern struct route_entry *route_lookup(struct inet_prefix *prefix, uint16_t vpn);
extern struct route_entry *route_match(struct inet_prefix *prefix, uint16_t vpn);
extern int route_tree_init(void);
extern void route_tree_delete(uint16_t vpn);
extern void route_tree_clear(void);
extern int route_select(struct route_entry *proute, struct rib_entry *prib);

/* IPv6 路由表函数原型 */
extern int routev6_add(struct rib_entry *prib);
extern int routev6_update(struct rib_entry *prib);
extern int routev6_delete(struct inet_prefix *prefix, uint16_t vpn);
extern struct route_entry *routev6_lookup(struct inet_prefix *prefix, uint16_t vpn);/* exactly match */
extern struct route_entry *routev6_match(struct inet_prefix *prefix, uint16_t vpn); /* mask longest match */
extern int routev6_tree_init(void);
extern void routev6_tree_delete(uint16_t vpn);
extern void routev6_tree_clear(void);
extern int routev6_select(struct route_entry *proute, struct rib_entry *prib);

extern int route_active_get_bulk_for_mib(struct route_entry *pindex, struct route_entry *padata, uint32_t index_flag);
extern int route_tree_mib_init(void);
#endif


