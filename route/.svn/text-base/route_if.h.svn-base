/**
 * @file      : route_if.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 8:51:28
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_IF_H
#define HIOS_ROUTE_IF_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/ifm_common.h>
#include "arp.h"
#include "ndp.h"

#define ROUTE_IF_VAL_RESET(val)    ((val)=0)
#define ROUTE_IF_IP_EXIST(ip)      ((ip)!=0)
#define ROUTE_IF_IPV6_EXIST(ip)    (((ip[0])!=0) || ((ip[1])!=0) || ((ip[2])!=0) || ((ip[3])!=0) \
                                    || ((ip[4])!=0) || ((ip[5])!=0) || ((ip[6])!=0) || ((ip[7])!=0) \
                                    || ((ip[8])!=0) || ((ip[9])!=0) || ((ip[10])!=0) || ((ip[11])!=0) \
                                    || ((ip[12])!=0) || ((ip[13])!=0) || ((ip[14])!=0) || ((ip[15])!=0))

/*arp 模块宏*/
#define ARP_DISABLE_DEF              1
#define ARP_PROXY_DEF                0
#define ARP_NUM_MAX_DEF              ARP_TOTAL_NUM


/* L3IF 的配置信息 */
enum L3IF_INFO
{
    L3IF_INFO_INVALID = 0,
    L3IF_INFO_VPN,
    L3IF_INFO_IPV4,
    L3IF_INFO_IPV6,
    L3IF_INFO_IPV6_LINK_LOCAL,
    L3IF_INFO_URPF,
    L3IF_INFO_MAX = 16
};


/* urpf 类型 */
enum URPF_TYPE
{
    URPF_TYPE_INVALID = 0,
    URPF_TYPE_LOOSE,
    URPF_TYPE_STRICT
};

/* 接口下的 arp 和 nd 配置和统计信息 */
struct ifm_arp
{
	uint32_t ifindex;
	int retry_cnt;  /* 重传计数 */

	/* ipv4 arp 的参数 */
	uint8_t  arp_disable;   /* 0: disable, 1: enable */
	uint8_t  arp_proxy;		/* 0: disable, 1: enable */
	uint16_t arp_num;
	uint32_t arp_num_max;

	/* ipv6 neighbor 的参数 */
	uint8_t  ndp_disable;
	uint8_t  ndp_proxy;
	uint32_t neighbor_limit;  /* 动态邻居数量限制 */
	uint32_t neighbor_count;  /* 动态邻居数量统计 */
};

/* arp的统计信息 */
struct ftm_arp_count
{	
	uint32_t arp_total_num;
	uint32_t arp_static_num;
	uint32_t arp_complete_num;
	uint32_t arp_incomplete_num;
	uint32_t arp_aged_num;
	uint32_t arp_rule_num;
};

/* L3 模式的接口数据结构 */
struct route_if
{
     uint32_t ifindex;
     uint8_t  down_flag;            // 0: linkup, 1: linkdown
     uint8_t  encap_flag;           // 子接口封装vlan配置， 0 : 无封装，1: 有封装
     uint8_t  shutdown_flag;        // 子接口 shutdown 配置，0 : no shutdown，1 : shutdown
     uint8_t  link_local_flag;      // 0: disable, 1: enable
     uint8_t  dhcp_save_flag;       // 0: disable, 1: enable
     uint8_t  pad[3];
     enum URPF_TYPE urpf;           // 接口 urpf 配置
     struct ifm_l3  intf;           // 三层接口信息
	 struct ifm_arp arp;            // 接口下的 arp 信息
     struct list unnumbered_list;   // 保存借用者的接口索引
};


extern struct hash_table route_if_table;


extern void route_if_cmd_init(void);
extern void route_if_table_init(int size);
extern int route_if_add(struct route_if *pif);
extern int route_if_delete(uint32_t ifindex);
extern struct route_if *route_if_lookup(uint32_t ifindex);
extern int route_if_get_bulk(uint32_t ifindex, struct ifm_l3 l3if[]);
extern int route_if_down(uint32_t ifindex);
extern int route_if_up(uint32_t ifindex);
extern int route_if_add_ip(struct route_if *pif);
extern int route_if_delete_ip(struct route_if *pif);
extern int route_if_add_v6_ip(struct route_if *pif);
extern int route_if_delete_v6_ip(struct route_if *pif);
extern int route_if_delete_config(struct route_if *pif);
extern int route_if_check_ip_conflict(const struct route_if *pif_new);
extern int route_if_check_v6_ip_conflict(const struct route_if *pif_new);
extern void route_if_process_l3vpn_delete(uint16_t vrf_id);
extern void route_if_process_subif_delete(uint32_t ifindex);
extern int route_if_get_intf_mode(struct vty *vty);
extern int arp_if_update(struct ifm_arp *parp_old,enum ARP_INFO type);
extern int ndp_if_update(struct ifm_arp *pndp, enum NDP_INFO type);
extern int route_if_add_slave_ip(struct route_if *pif);
extern int route_if_delete_slave_ip(struct route_if *pif);
#endif


