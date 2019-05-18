/*
*       manage the static arp table
*
*/

#ifndef HIOS_STATIC_ARP_H
#define HIOS_STATIC_ARP_H

#include <lib/types.h>
#include <lib/hash1.h>

#include "arp.h"
#include "route_if.h"
#include "arp_cmd.h"


#define ARP_LOG_DEBUG(format, ...) \
    zlog_debug(ARP_DEBUG_LOG, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define ARP_LOG_ERROR(format, ...) \
    zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

extern struct hash_table static_arp_table;

/* 添加主 IP 时添加接口 ARP 信息 */
#define ROUTE_ARPIF_CFG_INIT(pif, if_index)\
    pif->arp.ifindex = if_index;\
    pif->arp.arp_disable = 1;\
    pif->arp.arp_num_max = ARP_NUM_MAX_DEF;\
    pif->arp.arp_proxy = 0;\
    pif->arp.arp_num = 0;

/* 添加主 IP 时添加接口 ARP 信息 */
#define ROUTE_ARPIF_CFG_CLR(pif)\
    pif->arp.arp_disable = 0;\
    pif->arp.arp_num_max = 0;\
    pif->arp.arp_proxy = 0;\
    pif->arp.arp_num = 0;

/* Prototypes. */
struct arp_entry *arp_static_lookup( uint32_t uiIpaddr, uint16_t uiVpn);
int  arp_static_add(struct arp_entry *pstArp,uint8_t restore_flag);
void arp_static_process_l3if_add(uint32_t ifindex);
void arp_static_process_l3if_del(uint32_t ifindex);
int  arp_static_delete(uint32_t uiIpaddr,  uint16_t usVpn);
void arp_static_table_init(unsigned int size);
int  arp_static_get_bulk(struct arp_key *key,struct arp_entry parp[]);
uint32_t arp_static_get_outif(struct arp_entry *sarp);
void arp_static_delete_all(void);
void arp_static_delete_by_interface(uint32_t ifindex);
void arp_static_delete_by_slot(uint8_t slot_num);
void arp_static_route_change_resolve_outif(const struct route_if *pif,uint8_t action);

#endif

