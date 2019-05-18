/**
 * @file      : route_nhp.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 17:18:15
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_NHP_H
#define HIOS_ROUTE_NHP_H

#include <lib/route_com.h>
#include "route.h"


extern struct hash_table route_nhp_table;     // 用 nhp_index 做 key
extern struct hash_table route_nexthop_table; // 用 ifindex、nexthop、vpn 做 key
extern struct hash_table route_ecmp_table;    // 用所有的 nhp_index 做 key


/* 对 nhp_info 表的添加、删除、更新操作，根据索引查找 key */
extern void route_nhp_table_init(uint32_t size);
extern int route_nhp_add(struct nhp_info *pinfo);
extern int route_nhp_delete(uint32_t nhp_index);
extern struct nhp_info *route_nhp_lookup(uint32_t nhp_index);

/* 下一跳分离，对 nhp_entry 表的操作，根据 key 查找 nhp 索引 */
extern void route_nexthop_table_init(uint32_t size);
extern int route_nexthop_add(struct nhp_entry *pnhp);
extern void route_nexthop_update(struct nhp_entry *pnhp_old, struct nhp_entry *pnhp_new);
extern int route_nexthop_delete(struct nhp_entry *pnhp);
extern struct nhp_entry *route_nexthop_lookup(struct nhp_entry *pnhp);

/* 下一跳分离，对 ecmp_group 表的操作，根据 key 查找 ecmp 索引 */
extern void route_ecmp_table_init(unsigned int size);
extern struct ecmp_group *route_ecmp_create(struct ecmp_group *pecmp); /* 根据 nhp 生成 ecmp_group */
extern void route_ecmp_update(struct ecmp_group *pecmp_old, struct ecmp_group *pecmp_new);
extern int route_ecmp_add(struct ecmp_group *pecmp);
extern int route_ecmp_delete(struct ecmp_group *pecmp);
extern struct ecmp_group *route_ecmp_lookup(struct ecmp_group *pecmp);

extern int route_nhp_add_process(struct route_entry *proute);
extern int route_nhp_delete_process(struct route_entry *proute);


#endif


