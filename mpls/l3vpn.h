/**
 * @file      : l3vpn.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 11:08:22
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_L3VPN_H
#define HIOS_L3VPN_H

#include <lib/mpls_common.h>


enum L3VPN_INFO_E
{
    L3VPN_INFO_INVALID = 0,
    L3VPN_INFO_TUNNEL,
};


extern struct hash_table l3vpn_table;
extern struct hash_table l3vpn_route_table;

extern void mpls_l3vpn_table_init(uint32_t size);
extern void mpls_l3vpn_free(struct l3vpn_entry *pl3vpn);
extern struct l3vpn_entry *mpls_l3vpn_create(uint16_t vrf_id);
extern int mpls_l3vpn_add(struct l3vpn_entry *pl3vpn);
extern int mpls_l3vpn_delete(uint16_t vrf_id);
extern struct l3vpn_entry *mpls_l3vpn_lookup(uint16_t vrf_id);
extern int mpls_l3vpn_delete_lsp(struct l3vpn_entry *pl3vpn);
extern uint32_t mpls_l3vpn_route_get_label(uint16_t vrf_id);
extern int mpls_l3vpn_route_free_label(struct l3vpn_nhp *pnhp);
extern struct l3vpn_nhp *mpls_l3vpn_route_nhplist_add(struct list *plist,
                                                                struct l3vpn_nhp *pnhp);
extern int mpls_l3vpn_route_nhplist_delete(struct list *plist, struct l3vpn_nhp *pnhp);
extern struct l3vpn_nhp *mpls_l3vpn_route_nhp_lookup(struct list *plist,
                                                                    struct l3vpn_nhp *pnhp);
extern struct l3vpn_route *mpls_l3vpn_route_tree_add(struct ptree *proute_tree,
                                                                struct l3vpn_route *pvroute);
extern int mpls_l3vpn_route_tree_delete(struct ptree *proute_tree,
                                                    struct inet_prefix *prefix);
extern struct l3vpn_route *mpls_l3vpn_route_tree_lookup(struct ptree *proute_tree,
                                                                    struct inet_prefix *prefix);
extern int mpls_l3vpn_route_node_add(struct l3vpn_route *pvroute);
extern int mpls_l3vpn_route_node_delete(struct l3vpn_route *pvroute);
extern void mpls_l3vpn_route_tree_free(struct ptree *proute_tree);
extern void mpls_l3vpn_process_route(struct l3vpn_entry *pl3vpn,
                        struct l3vpn_route *pvroute, struct l3vpn_nhp *pnhp, enum OPCODE_E opcode);
extern void mpls_l3vpn_process_tunnel_up(uint32_t ifindex);
extern void mpls_l3vpn_process_tunnel_down(uint32_t ifindex);
extern int mpls_l3vpn_get_instance_bulk(uint32_t vrf_id, struct l3vpn_entry l3vpn_buff[]);
extern int mpls_l3vpn_get_target_bulk(uint32_t vrf_id, uchar *ptarget,
                            int data_len, enum L3VPN_SUBTYPE subtype, struct l3vpn_target target[]);


#endif


