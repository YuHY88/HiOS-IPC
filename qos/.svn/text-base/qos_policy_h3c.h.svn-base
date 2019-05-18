
#ifndef HIOS_QOS_POLICY_H3C_H
#define HIOS_QOS_POLICY_H3C_H

#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/vty.h>
#include <lib/qos_common.h>
#include "qos_main.h"
#include "acl.h"

#define QOS_H3C_ACL_INVALID_ENTRY     	2000

extern struct list qos_policy_conf_list;/*建classifier和behavior单独链表*/
extern struct list qos_policy_list;		/*建policy 链表，最终用于下发*/


/* qos  name类型 */
enum QOS_POLICY_NAME_TYPE
{
	QOS_NAME_TYPE_NONE = 0,
	QOS_CLASSIFIER_NAME,
	QOS_BEHAVIOR_NAME,
	QOS_POLICY_NAME
};
	
/* rule id */
enum H3C_RULE_ID
{
	H3C_RULE_ID_NONE = 0,
	H3C_DESTINATION_MAC,
	H3C_SOURCE_MAC,
	H3C_SERVICE_VLAN_ID,
	H3C_SERVICE_DOT1P,
	H3C_CONTRL_PLANE_PROCTOL,
	H3C_DSCP,
	H3C_IP_PRECEDENCE,
};

struct qos_policy_h3c
{
	char classifier_name[NAME_STRING_LEN];
	char behavior_name[NAME_STRING_LEN];
	char policy_name[NAME_STRING_LEN];

	uint8_t  classifier_mask;/*配置流分类内容后置1*/
	uint8_t  behavior_mask;  /*配置流动作内容后置1*/
	uint8_t  policy_mask;	 /*流分类与流动作绑定后置1，下发标志*/
	uint32_t      acl_num;    
	uint32_t      ifindex;     /* policy 应用的接口 */
	
	//struct rule_node rule;
	
    struct list rulelist;
	struct qos_policy action;
};

void qos_policy_h3c_init(void);

struct qos_policy_h3c *qos_policy_h3c_add_list(struct qos_policy_h3c policy, 
enum QOS_POLICY_NAME_TYPE type);
struct qos_policy_h3c *qos_policy_name_add(struct qos_policy_h3c policy, enum 
QOS_POLICY_NAME_TYPE type);
struct qos_policy_h3c *qos_policy_name_lookup(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type);
int qos_policy_h3c_delete(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type);
void qos_policy_h3c_cmd_init(void);
int qos_policy_classifer_h3c_config_write (struct vty *vty);
int qos_policy_behavior_h3c_config_write (struct vty *vty);
int qos_policy_h3c_config_write (struct vty *vty);
void qos_policy_h3c_cmd_init(void);

#endif
