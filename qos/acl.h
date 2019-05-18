
#ifndef HIOS_ACL_H
#define HIOS_ACL_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <lib/qos_common.h>





#define ACL_GROUP_NUM                256
#define ACL_RULE_NUM                 256

#define ACl_DEF_SVLANID              4096
#define ACl_DEF_CVLANID              4096
#define ACL_DEF_COS                  8
#define ACL_DEF_CLAN_COS             8
#define ACL_DEF_DSCP                 64
#define ACL_DEF_TOS                  8
#define ACL_DEF_OFFSET               0
#define ACL_OFFSET_LEN               16    /* 用户自定义字段的字节数 */

#define ACl_TCP_FLAG_FIN              1<<0
#define ACl_TCP_FLAG_SYN              1<<1
#define ACl_TCP_FLAG_RST              1<<2
#define ACl_TCP_FLAG_PSH              1<<3
#define ACl_TCP_FLAG_ACK              1<<4
#define ACl_TCP_FLAG_URG              1<<5



/* ACL 组数据结构 */
struct acl_group
{
	uchar       name[NAME_STRING_LEN];
	uint32_t    acl_num;
    struct list rulelist;
};


struct hash_table acl_group_table;/* acl hash 表，用索引做 key */


#define ACL_TAB_MSIZE 1024


/* ACL group 的操作函数 */
void acl_group_table_init(unsigned int size);
int acl_group_add(uint32_t acl_num);
int acl_group_delete(uint32_t acl_num);
struct acl_group *acl_group_lookup(uint32_t acl_num);


/* ACL rule 的操作函数 */
int acl_rule_size_get(uint8_t rule_type);
int acl_rule_add(void *prule, uint8_t rule_type, uint32_t acl_num);
int acl_rule_delete(uint16_t ruleid, uint32_t acl_num);
struct rule_node *acl_rule_lookup(uint16_t ruleid, uint32_t acl_num);
int acl_rule_get_bulk ( uint32_t acl_num, uint16_t ruleid, struct rule_node *prule);







#endif
















