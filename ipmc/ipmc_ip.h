/* 
*  multicast ip management 
*/
#ifndef IPMC_IP_H
#define IPMC_IP_H


#include <lib/types.h>
#include <lib/hash1.h>
#include "ipmc.h"


/* Hash of multicast ip table */
extern struct hash_table mc_ip_table;


/* Prototypes of mc_mac hash */
void mc_ip_table_init(unsigned int size); /* 组播 ip hash 表创建 */
int mc_ip_add(struct mcip_t *pip);           /* 添加一个组播 IP */
int mc_ip_delete(struct mcip_t *pip);       /* 删除一个组播 ip */
struct mcip_t *mc_ip_lookup(struct mcip_t *pip); /* 查找一个组播 ip 表 */


#endif

