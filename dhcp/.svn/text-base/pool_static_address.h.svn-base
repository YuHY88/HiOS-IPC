#ifndef _DHCP_STATIC_ADDRESS_H
#define _DHCP_STATIC_ADDRESS_H

#include "pool_address.h"
extern struct hash_table dhcp_static_addr_table;

void dhcp_static_addr_table_init(unsigned int uiSize);

struct dhcpOfferedAddr *  dhcp_static_addr_lookup(uint8_t *mac);
struct dhcpOfferedAddr *dhcp_static_addr_add(uint32_t ip, uint8_t *mac);

struct hash_bucket *dhcp_hash_find_key_next(struct hash_table *tab, void *hash_key);
int debug_dhcp_ipbind_get_bluk(struct dhcpOfferedAddr *lease);
void dhcp_static_addr_del_by_pool(struct ip_pool *pool_node);
void dhcp_static_addr_del_by_ifindex(uint32_t ifindex);
void dhcp_static_addr_del(uint32_t ip);

int dhcp_ipbind_get_bulk(void *pdata, int data_len, struct dhcpOfferedAddr lease[]);
extern int dhcp_com_ipbind_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr);

#endif

