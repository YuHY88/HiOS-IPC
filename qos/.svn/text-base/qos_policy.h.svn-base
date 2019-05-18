
#ifndef HIOS_QOS_POLICY_H
#define HIOS_QOS_POLICY_H

#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/vty.h>
#include "qos_main.h"
#include "acl.h"


extern struct list qos_policy_global;/* 全局模式下的 qos policy */


void qos_policy_global_init(void);
void qos_policy_cmd_init(void);
int qos_policy_config_write (struct vty *vty);



int qos_policy_add_list(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex, struct acl_action *paction);
int qos_policy_add(uint32_t acl_num, enum POLICY_TYPE type, enum QOS_DIR direct,uint32_t ifindex, struct acl_action *paction);
int qos_policy_delete(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct,uint32_t ifindex);
struct qos_policy *qos_policy_lookup(uint32_t acl_num, enum POLICY_TYPE type, enum QOS_DIR direct,uint32_t ifindex);


/* 接口配置 policy 的操作 */
int qos_policy_add_if(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex, struct acl_action *paction);
int qos_policy_delete_if(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex);
struct qos_policy *qos_policy_lookup_if(uint32_t acl_num, enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t ifindex);
int qos_policy_get_bulk (uint32_t ifindex,enum POLICY_TYPE type,enum QOS_DIR direct, uint32_t acl_num,struct qos_policy *ppolicy );
int qos_policy_return_str (int ret,struct vty *vty);


#endif
















