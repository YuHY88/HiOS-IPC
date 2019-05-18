/**
 * @file      : route_rib.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:41:23
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_RIB_H
#define HIOS_ROUTE_RIB_H


#include <lib/types.h>
#include <lib/inet_ip.h>
#include <lib/ptree.h>
#include <lib/linklist.h>
#include <lib/route_com.h>

/* 原始路由表结构 */
struct rib_entry
{
    struct inet_prefix prefix;
    uint16_t           vpn;
    struct list        nhplist; // nexthop 链表，存放 nhp_entry，按照 metric 从小到大的顺序排列
}__attribute__((packed));


/* 原始路由表中路由数量统计 */
struct rib_global
{
    /* ---总数统计--- */
    uint32_t num_max;           // 路由总数
    uint32_t num_entries;       // 实际存储路由总数

    /* ---协议统计--- */
    uint32_t num_static;        // 静态路由总数
    uint32_t num_static_active; // 静态路由激活的总数
    uint32_t num_direct;        // 直连路由总数
    uint32_t num_direct_active; // 直连路由激活的总数
    uint32_t num_ospf;          // ospf 协议路由总数
    uint32_t num_ospf_active;   // ospf 协议路由激活的总数
    uint32_t num_isis;          // isis 协议路由总数
    uint32_t num_isis_active;   // isis 协议路由激活的总数
    uint32_t num_rip;           // rip 协议路由总数
    uint32_t num_rip_active;    // rip 协议路由激活的总数
    uint32_t num_ibgp;          // ibgp 协议路由总数
    uint32_t num_ibgp_active;   // ibgp 协议路由激活的总数
    uint32_t num_ebgp;          // ebgp 协议路由总数
    uint32_t num_ebgp_active;   // ebgp 协议路由激活的总数
    uint32_t num_ldp;           // ldp 协议路由总数
    uint32_t num_ldp_active;    // ldp 协议路由激活的总数
    uint32_t num_mplstp;        // mplstp 协议路由总数
    uint32_t num_mplstp_active; // mplstp 协议路由激活的总数
    uint32_t num_rsvpte;        // rsvpte 协议路由总数
    uint32_t num_rsvpte_active; // rsvpte 协议路由激活的总数
};


extern struct rib_global rib_number;
extern struct rib_global ribv6_number;
extern struct ptree *rib_tree[L3VPN_SIZE+1];
extern struct ptree *ribv6_tree[L3VPN_SIZE+1];


#define ROUTE_NUMBER_ACTIVE \
    (rib_number.num_direct_active + rib_number.num_static_active +\
    rib_number.num_ospf_active + rib_number.num_isis_active +\
    rib_number.num_rip_active + rib_number.num_ibgp_active +\
    rib_number.num_ebgp_active + rib_number.num_ldp_active +\
    rib_number.num_mplstp_active + rib_number.num_rsvpte_active)

#define ROUTEV6_NUMBER_ACTIVE \
    (ribv6_number.num_direct_active + ribv6_number.num_static_active +\
    ribv6_number.num_ospf_active + ribv6_number.num_isis_active +\
    ribv6_number.num_rip_active + ribv6_number.num_ibgp_active +\
    ribv6_number.num_ebgp_active + ribv6_number.num_ldp_active +\
    ribv6_number.num_mplstp_active + ribv6_number.num_rsvpte_active)



/* IPv4 rib 函数原型 */
extern int rib_add(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern int rib_update(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern int rib_delete(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern struct rib_entry *rib_lookup(struct inet_prefix *prefix, uint16_t vpn);
extern struct nhp_entry *rib_lookup_nhp(struct list *nhplist, struct nhp_entry *pnhp);
extern int rib_tree_init(void);
extern void rib_tree_delete(uint16_t vpn);
extern void rib_tree_clear(void);
extern void rib_count_number_active(const struct nhp_entry *pnhp, uint32_t opcode);

/* IPv6 rib 函数原型 */
extern int ribv6_add(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern int ribv6_update(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern int ribv6_delete(struct inet_prefix *prefix, uint16_t vpn, struct nhp_entry *pnhp);
extern struct rib_entry *ribv6_lookup(struct inet_prefix *prefix, uint16_t vpn);
extern struct nhp_entry *ribv6_lookup_nhp(struct list *nhplist, struct nhp_entry *pnhp);
extern int ribv6_tree_init(void);
extern void ribv6_tree_delete(uint16_t vpn);
extern void ribv6_tree_clear(void);
extern void ribv6_count_number_active(const struct nhp_entry *pnhp, uint32_t opcode);

extern int route_rib_get_bulk_for_mib(struct route_rib_mib *pindex, struct route_rib_mib *pdata, uint32_t index_flag);
extern uint32_t route_rib_get_count_for_mib(struct route_count_mib *pdata, uint32_t index_flag);
#endif


