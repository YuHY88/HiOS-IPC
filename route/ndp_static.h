/*
*     static ipv6 nd table
*/


#ifndef HIOS_NDP_STATIC_H
#define HIOS_NDP_STATIC_H

#include <lib/inet_ip.h>
#include <lib/log.h>

#include "ndp.h"
#include "ndp_cmd.h"

extern struct hash_table static_ndp_table;

#define ROUTE_NDP_ZLOG_DEBUG(format,...)\
    zlog_debug(NDP_DEBUG_LOG, "%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define ROUTE_NDP_ZLOG_ERR(format,...)\
    zlog_err("%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);


/*添加接口主ip时初始化接口nd 控制信息*/
/*3层接口添加ip 时初始化ndp 控制信息*/
#define ROUTE_NDPIF_CFG_INIT(pif,if_index)\
        pif->arp.ifindex = if_index;\
        pif->arp.ndp_disable = NDP_ENABLE_DEF; \
        pif->arp.ndp_proxy = NDP_NO_PROXY_DEF; \
        pif->arp.neighbor_limit = ND_TOTAL_NUM;




/* 静态 nd 表项 hash 表初始化 */
void ndp_static_table_init(uint32_t size);

/* 添加静态 nd 表项  */
int ndp_static_add(struct ndp_neighbor *pnd_entry);

/* 删除静态 nd 表项  */
int ndp_static_delete(struct ipv6_addr *pipaddr, uint32_t vpn, uint32_t ifindex);

/* 查找静态 nd 表项 */
struct ndp_neighbor *ndp_static_lookup(struct ipv6_addr *pipaddr, uint32_t vpn, uint32_t ifindex);

/*delete static ndp entry by ip or vpn or ip&vpn*/
void ndp_static_delete_by_ip_vpn(struct ipv6_addr *ip6addr, int vpn);

/*处理三层接口添加事件*/
void ndp_static_process_l3if_add(uint32_t ifindex);

/*处理三册接口删除事件*/
void ndp_static_process_l3if_del(uint32_t ifindex);


#endif /*NDP_STATIC_H*/

