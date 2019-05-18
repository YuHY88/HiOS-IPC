/* 
*  multicast group management 
*/
#ifndef IPMC_GROUP_H
#define IPMC_GROUP_H


#include <lib/types.h>
#include <lib/hash1.h>
#include "ipmc.h"


/* Hash of multicast group table */
extern struct hash_table mc_group_table;


/* Prototypes of mcgroup hash */
void mc_group_table_init(unsigned int size);
int mc_group_create(uint16_t  group_id);  /* 创建 group 数据结构 */
int mc_group_delete(uint16_t  group_id);  /* 删除 group 数据结构 */
struct mcgroup_t *mc_group_lookup(uint16_t  group_id); /* 查找 group 数据结构 */

int mc_group_add_if(struct mcgroup_t *pgroup, uint32_t ifindex);    /* group 添加一个接口 */
int mc_group_delete_if(struct mcgroup_t *pgroup, uint32_t ifindex);/* group 删除一个接口 */
int mc_group_lookup_if(struct mcgroup_t *pgroup, uint32_t ifindex);/*group查询一个接口*/


#endif
