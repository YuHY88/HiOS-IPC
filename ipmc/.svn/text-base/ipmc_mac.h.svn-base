/* 
*  multicast mac management 
*/
#ifndef IPMC_MAC_H
#define IPMC_MAC_H

#include <lib/types.h>
#include <lib/hash1.h>
#include "ipmc.h"


/* Hash of multicast mac table */
extern struct hash_table mc_mac_table;


/* multicast mac 表的操作函数 */
void mc_mac_table_init(unsigned int size); /* 组播 mac hash 表创建 */
int mc_mac_add(struct mcmac_t *pmac);         /* 添加一个组播 mac */
int mc_mac_delete(struct mcmac_t *pmac);     /* 删除一个组播 mac */
struct mcmac_t *mc_mac_lookup(struct mcmac_t *pmac); /* 查找一个组播 mac 表*/


#endif
