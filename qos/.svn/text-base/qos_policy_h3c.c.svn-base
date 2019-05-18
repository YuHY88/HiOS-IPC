

#include <string.h>
#include <stdlib.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/log.h>
#include <lib/vty.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
//#include <lib/msg_ipc.h>
#include <lib/thread.h>
#include <lib/index.h>
#include <lib/ether.h>

#include "qos_policy.h"
#include "qos_main.h"
#include "qos_if.h"
#include "qos_car.h"
#include "acl.h"
#include "qos_mirror.h"
#include "acl_cli.h"
#include "qos_policy_h3c.h"




struct list qos_policy_conf_list;
struct list qos_policy_list;

void qos_policy_h3c_init(void)
{	
	index_register(INDEX_TYPE_H3C_ACL, QOS_H3C_ACL_INVALID_ENTRY);

	memset(&qos_policy_conf_list, 0, sizeof(struct list));

	memset(&qos_policy_list, 0, sizeof(struct list));
}

struct qos_policy_h3c *qos_policy_h3c_add_list(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type)
{
	struct qos_policy_h3c *qos_policy = NULL;
	
	qos_policy = (struct qos_policy_h3c *)XMALLOC(MTYPE_QOS_ENTRY, sizeof(struct qos_policy_h3c));
	if(NULL == qos_policy)
	{
		QOS_LOG_ERR("The memory is insufficient.\n");
		return NULL;
	}
	
	memset(qos_policy,0,sizeof(struct qos_policy_h3c));

	if(type == QOS_CLASSIFIER_NAME)
	{
		memcpy(qos_policy->classifier_name,policy.classifier_name,sizeof(uchar)* NAME_STRING_LEN );

		listnode_add (&qos_policy_conf_list,(void *)qos_policy);
	}
	else if(type == QOS_BEHAVIOR_NAME)
	{
		memcpy(qos_policy->behavior_name,policy.behavior_name,sizeof(uchar)* NAME_STRING_LEN );

		listnode_add (&qos_policy_conf_list,(void *)qos_policy);
	}
	else if(type == QOS_POLICY_NAME)
	{
		memcpy(qos_policy->policy_name,policy.policy_name,sizeof(uchar)* NAME_STRING_LEN );

		listnode_add (&qos_policy_list,(void *)qos_policy);
	}
	

	return qos_policy;
}


struct qos_policy_h3c *qos_policy_name_add(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type)
{	
	struct qos_policy_h3c *qos_policy = NULL;
	struct listnode *node = NULL;
	struct list *list = NULL;
	int flag = 0;
	
	if ((list_isempty (&qos_policy_conf_list))&&(type != QOS_POLICY_NAME))
	{	
		qos_policy = qos_policy_h3c_add_list(policy, type);
		return qos_policy;
	}
	else if ((list_isempty (&qos_policy_list))&&(type == QOS_POLICY_NAME))
	{
		qos_policy = qos_policy_h3c_add_list(policy, type);
		return qos_policy;
	}

	if ((!list_isempty (&qos_policy_conf_list))&&(type != QOS_POLICY_NAME))
	{	
		list = &qos_policy_conf_list;
	}
	else if ((!list_isempty (&qos_policy_list))&&(type == QOS_POLICY_NAME))
	{
		list = &qos_policy_list;
	}


	for (node = list->head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);

		if(type == QOS_CLASSIFIER_NAME)
		{
			if(!strncmp(policy.classifier_name, qos_policy->classifier_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				QOS_LOG_ERR("this policy existed.\n");
				return qos_policy;
			}
			
		}
		else if(type == QOS_BEHAVIOR_NAME)
		{
			if(!strncmp(policy.behavior_name, qos_policy->behavior_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				QOS_LOG_ERR("this policy existed.\n");
				return qos_policy;
			}
		}
		else if(type == QOS_POLICY_NAME)
		{
			if(!strncmp(policy.policy_name, qos_policy->policy_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				QOS_LOG_ERR("this policy existed.\n");
				return qos_policy;
			}
		}
	}

	if(flag == 0)
	{
		qos_policy = qos_policy_h3c_add_list(policy, type);
		return qos_policy;
	}
	
	return qos_policy;
}


struct qos_policy_h3c *qos_policy_name_lookup(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type)
{
	struct qos_policy_h3c *qos_policy = NULL;
	struct listnode *node = NULL;
	struct list *list = NULL;
	int flag = 0;
	
	if ((list_isempty (&qos_policy_conf_list))&&(type != QOS_POLICY_NAME))
	{	
		QOS_LOG_ERR("this policy not existed.\n");
		return NULL;
	}
	else if ((list_isempty (&qos_policy_list))&&(type == QOS_POLICY_NAME))
	{
		QOS_LOG_ERR("this policy not existed.\n");
		return NULL;
	}

	if ((!list_isempty (&qos_policy_conf_list))&&(type != QOS_POLICY_NAME))
	{	
		list = &qos_policy_conf_list;
	}
	else if ((!list_isempty (&qos_policy_list))&&(type == QOS_POLICY_NAME))
	{
		list = &qos_policy_list;
	}


	for (node = list->head ; node; node = node->next)
	{
		qos_policy = listgetdata(node);

		if(type == QOS_CLASSIFIER_NAME)
		{
			if(!strncmp(policy.classifier_name, qos_policy->classifier_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				return qos_policy;
			}	
		}
		else if(type == QOS_BEHAVIOR_NAME)
		{
			if(!strncmp(policy.behavior_name, qos_policy->behavior_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				return qos_policy;
			}
		}
		else if(type == QOS_POLICY_NAME)
		{
			if(!strncmp(policy.policy_name, qos_policy->policy_name, sizeof(uchar)* NAME_STRING_LEN))
			{
				flag = 1;
				return qos_policy;
			}
		}
	}

	if(flag == 0)
	{
		QOS_LOG_ERR("this policy not existed.\n");
		return NULL;
	}
	
	return qos_policy;
}

int qos_policy_h3c_delete(struct qos_policy_h3c policy, enum QOS_POLICY_NAME_TYPE type)
{
	struct qos_policy_h3c qos_policy;
	struct qos_policy_h3c *qos_policy_c = NULL;
	
	if((type == QOS_CLASSIFIER_NAME || type == QOS_BEHAVIOR_NAME) && list_isempty (&qos_policy_conf_list))
	{
		QOS_LOG_ERR("can not find this classifier or behavior policy.\n");
		return ERRNO_NOT_FOUND;
	}else if(type == QOS_POLICY_NAME && list_isempty (&qos_policy_list))
	{
		QOS_LOG_ERR("can not find this policy.\n");
		return ERRNO_NOT_FOUND;
	}


	memset(&qos_policy,0,sizeof(struct qos_policy_h3c));

	if(type == QOS_CLASSIFIER_NAME)
	{
		memcpy(qos_policy.classifier_name,policy.classifier_name,sizeof(uchar)* NAME_STRING_LEN );
		qos_policy_c = qos_policy_name_lookup(qos_policy, type);
		if(qos_policy_c == NULL)
		{
			QOS_LOG_ERR("Find classifier struct qos_policy_h3c failed!!\n");
			return CMD_WARNING;
		}
		if(qos_policy_c->classifier_mask == 0)
		{
			QOS_LOG_ERR("please set classifier configure!!\n");
			return CMD_WARNING;
		}
		listnode_delete (&qos_policy_conf_list,(void *)qos_policy_c);
	}
	else if(type == QOS_BEHAVIOR_NAME)
	{
		memcpy(qos_policy.behavior_name,policy.behavior_name,sizeof(uchar)* NAME_STRING_LEN );
		qos_policy_c = qos_policy_name_lookup(qos_policy, type);
		if(qos_policy_c == NULL)
		{
			QOS_LOG_ERR("Find classifier struct qos_policy_h3c failed!!\n");
			return CMD_WARNING;
		}
		if(qos_policy_c->classifier_mask == 0)
		{
			QOS_LOG_ERR("please set classifier configure!!\n");
			return CMD_WARNING;
		}

		listnode_delete (&qos_policy_conf_list,(void *)qos_policy_c);
	}
	else if(type == QOS_POLICY_NAME)
	{
		memcpy(qos_policy.policy_name,policy.policy_name,sizeof(uchar)* NAME_STRING_LEN );
		qos_policy_c = qos_policy_name_lookup(qos_policy, type);
		if(qos_policy_c == NULL)
		{
			QOS_LOG_ERR("Find classifier struct qos_policy_h3c failed!!\n");
			return CMD_WARNING;
		}
		if(qos_policy_c->classifier_mask == 0)
		{
			QOS_LOG_ERR("please set classifier configure!!\n");
			return CMD_WARNING;
		}

		listnode_delete (&qos_policy_list,(void *)qos_policy_c);
	}

	return CMD_SUCCESS;
}

struct cmd_node qos_classifier_node =
{
  QOS_CLASSIFIER_NODE,
  "%s(config-classifier)# ",
  0,
};

struct cmd_node qos_behavior_node =
{
  QOS_BEHAVIOR_NODE,
  "%s(config-behavior)# ",
  0,
};

struct cmd_node qos_policy1_node =
{
  QOS_POLICY1_NODE,
  "%s(config-policy)# ",
  0,
};

	
DEFUN (qos_classifier_mode,
	qos_classifier_mode_cmd,
	"traffic classifier NAME",
	 "Qos traffic\n"
	 "Qos traffic classifier\n"
	 "Qos traffic classifier NAME\n")
{
	enum QOS_POLICY_NAME_TYPE type = QOS_CLASSIFIER_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy = NULL;

	uint8_t ret = 0;
	
	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.classifier_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy = qos_policy_name_lookup(policy, type);
	if(ppolicy == NULL)
	{
		ppolicy = qos_policy_name_add(policy, type);	
		ppolicy->acl_num = index_alloc(INDEX_TYPE_H3C_ACL);
		if(ppolicy->classifier_mask == 1)
		{
			vty_error_out(vty, "if-match has exist!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	
	ppolicy->classifier_mask = 1;
	
	ret = acl_group_add(ppolicy->acl_num);
	if(ret)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty->node = QOS_CLASSIFIER_NODE;
	vty->index = (void *)ppolicy;
	return CMD_SUCCESS;
}

DEFUN (qos_behavior_mode,
	qos_bebavior_mode_cmd,
	"traffic behavior NAME",
	 "Qos traffic\n"
     "Qos traffic behavior\n"
     "Qos traffic behavior NAME\n")
{
	enum QOS_POLICY_NAME_TYPE type = QOS_BEHAVIOR_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy = NULL;

	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.behavior_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy = qos_policy_name_lookup(policy, type);
	if(ppolicy == NULL)
	{
		ppolicy = qos_policy_name_add(policy, type);	
		if(ppolicy->behavior_mask == 1)
		{
			vty_error_out(vty, "traffic behavior has exist!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	ppolicy->behavior_mask = 1;
	vty->node = QOS_BEHAVIOR_NODE;
	vty->index = (void *)ppolicy;
	return CMD_SUCCESS;
}


DEFUN (h3c_qos_policy_mode,
	h3c_qos_policy_mode_cmd,
	"qos policy NAME",
	 "Qos \n"
     "Qos policy\n"
     "Qos policy NAME\n")
{
	enum QOS_POLICY_NAME_TYPE type = QOS_POLICY_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy = NULL;

	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy = qos_policy_name_lookup(policy, type);
	if(ppolicy == NULL)
	{
		ppolicy = qos_policy_name_add(policy, type);	
		if(ppolicy->policy_mask == 1)
		{
			vty_error_out(vty, "qos policy is not exist!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	ppolicy->policy_mask = 1;
	vty->node = QOS_POLICY1_NODE;
	vty->index = (void *)ppolicy;
	return CMD_SUCCESS;

}

DEFUN (undo_h3c_qos_classifier_mode,
	undo_h3c_qos_classifier_mode_cmd,
	"undo traffic classifier NAME",
	"undo\n"
	"traffic\n"
	"classifier\n"
	"classifier name\n")
{
	uint8_t ret = 0;
	enum QOS_POLICY_NAME_TYPE type = QOS_CLASSIFIER_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy_c = NULL;

	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.classifier_name, argv[0], sizeof(uchar) * NAME_STRING_LEN );

	ppolicy_c = qos_policy_name_lookup(policy, QOS_CLASSIFIER_NAME );

	if(ppolicy_c == NULL)
	{
		vty_error_out(vty, "Find classifier struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(ppolicy_c->acl_num != 0)
	{
		ret = acl_group_delete(ppolicy_c->acl_num);
		if(ret)
		{
			vty_error_out(vty, "cannot find this acl group ,please check out%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		index_free(INDEX_TYPE_H3C_ACL, ppolicy_c->acl_num);
	}
	qos_policy_h3c_delete(policy, type);
	return CMD_SUCCESS;

}

DEFUN (undo_h3c_qos_behavior_mode,
	undo_h3c_qos_behavior_mode_cmd,
	"undo traffic behavior NAME",
	"undo\n"
	"traffic"
	"behavior\n"
	"behavior name\n")
{
	enum QOS_POLICY_NAME_TYPE type = QOS_BEHAVIOR_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy_b = NULL;

	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.behavior_name, argv[0], sizeof(uchar) * NAME_STRING_LEN );

	ppolicy_b = qos_policy_name_lookup(policy, QOS_BEHAVIOR_NAME );

	if(ppolicy_b == NULL)
	{
		vty_error_out(vty, "Find classifier struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	qos_policy_h3c_delete(policy, type);

	return CMD_SUCCESS;

}

DEFUN (undo_h3c_qos_policy_mode,
	undo_h3c_qos_policy_mode_cmd,
	"undo qos policy NAME",
	"undo\n"
	"Qos \n"
	"Qos policy\n"
	"Qos policy NAME\n")
{
	enum QOS_POLICY_NAME_TYPE type = QOS_POLICY_NAME;
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy_p = NULL;

	memset(&policy, 0, sizeof(struct qos_policy_h3c));

	memcpy(policy.policy_name, argv[0], sizeof(uchar) * NAME_STRING_LEN );

	ppolicy_p = qos_policy_name_lookup(policy, QOS_POLICY_NAME );

	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find classifier struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	qos_policy_h3c_delete(policy, type);

	return CMD_SUCCESS;

}
DEFUN  (qos_if_match_mac,
	qos_if_match_mac_cmd,
	"if-match {destination-mac H-H-H|source-mac H-H-H|service-vlan-id <1-4094>|service-dot1p <0-7>}",
	"if-match\n"
	"destination-mac\n"
	"destination-mac H-H-H\n"
	"source-mac \n"
	"source-mac H-H-H\n"
	"service-vlan-id\n"
	"service-vlan-id <1-4094>\n"
	"service-dot1p\n"
	"service-dot1p <0-7>\n")
{
	uint8_t ret = 0;
	struct mac_rule rule_tmp;
	struct qos_policy_h3c *ppolicy = NULL;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy->classifier_mask == 0)
	{
		vty_error_out(vty, "this classifier is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argc < 1)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&rule_tmp, 0, sizeof(struct mac_rule));
	rule_tmp.vlan = ACl_DEF_SVLANID;
	rule_tmp.cvlan = ACl_DEF_CVLANID;
	rule_tmp.cos = ACL_DEF_COS;
	rule_tmp.cvlan_cos = ACL_DEF_CLAN_COS;
	rule_tmp.offset_len = ACL_DEF_OFFSET;
	rule_tmp.dscp = ACL_DEF_DSCP;
	rule_tmp.tos = ACL_DEF_TOS;

	if(NULL != argv[0])
	{
		ether_string1_to_mac((char *)argv[0], (uchar *)rule_tmp.dmac);
		rule_tmp.dmac_mask = 1;
		
		rule_tmp.ruleid = H3C_DESTINATION_MAC;
	}
	if(NULL != argv[1])
	{
		ether_string1_to_mac((char *)argv[1], (uchar *)rule_tmp.smac);
		rule_tmp.smac_mask = 1;
		rule_tmp.ruleid = H3C_SOURCE_MAC;
	}
	if(NULL != argv[2])
	{
		rule_tmp.vlan = (uint32_t)atoi(argv[2]);
		rule_tmp.ruleid = H3C_SERVICE_VLAN_ID;
	}
	if(NULL != argv[3])
	{
		rule_tmp.cos = (uint32_t)atoi(argv[3]);
		rule_tmp.ruleid = H3C_SERVICE_DOT1P;
	}
	if((NULL == argv[0]) && (NULL == argv[1]) &&(NULL == argv[2]) &&(NULL == argv[3]))
	{
		vty_error_out(vty, "if-match is null %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = acl_rule_add((struct mac_rule* )&rule_tmp, ACL_TYPE_MAC, ppolicy->acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	return CMD_SUCCESS;
}


DEFUN  (uudo_qos_if_match_mac,
	undo_qos_if_match_mac_cmd,
	"undo if-match (destination-mac|source-mac|service-vlan-id|service-dot1p|control-plane|dscp|ip-precedence)",
	"undo\n"
	"if-match\n"
	"destination-mac\n"
	"destination-mac H-H-H\n"
	"source-mac \n"
	"source-mac H-H-H\n"
	"service-vlan-id\n"
	"service-vlan-id <1-4094>\n"
	"service-dot1p\n"
	"service-dot1p <0-7>\n")
{
	uint8_t ret = 0;
	struct qos_policy_h3c *ppolicy = NULL;
	enum H3C_RULE_ID rule_id = H3C_RULE_ID_NONE;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy->classifier_mask == 0)
	{
		vty_error_out(vty, "this classifier is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	if(!strcmp(argv[0], "destination-mac"))
	{
		rule_id = H3C_DESTINATION_MAC;
	}
	else if(!strcmp(argv[0], "source-mac"))
	{
		rule_id = H3C_SOURCE_MAC;
	}
	else if(!strcmp(argv[0], "service-vlan-id"))
	{
		rule_id = H3C_SERVICE_VLAN_ID;
	}
	else if(!strcmp(argv[0], "service-dot1p"))
	{
		rule_id = H3C_SERVICE_DOT1P;
	}
	else if(!strcmp(argv[0], "control-plane"))
	{
		rule_id = H3C_CONTRL_PLANE_PROCTOL;
	}
	else if(!strcmp(argv[0], "dscp"))
	{
		rule_id = H3C_DSCP;
	}
	else if(!strcmp(argv[0], "ip-precedence"))
	{		
		rule_id = H3C_IP_PRECEDENCE;
	}
	ret = acl_rule_delete(rule_id ,ppolicy->acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule delete fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	} 
   
	return CMD_SUCCESS;
}


DEFUN  (qos_if_match_ipex,
		qos_if1_match_ipex_cmd,
       "if-match {any|control-plane protocol <1-255>|dscp <0-63>|ip-precedence <0-7>}",
       "if-match\n"
       "if-match any\n"
       "control-plane\n"
       "proctol\n"     
       "proctol number\n"
       "dscp\n"
       "dscp id\n"
	   "ip-precedence\n"
	   "ip-precedence number\n")
{
	uint8_t ret = 0;
	struct ip_extend_rule rule_tmp;
	struct qos_policy_h3c *ppolicy = NULL;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy->classifier_mask == 0)
	{
		vty_error_out(vty, "this classifier is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argc < 1)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(&rule_tmp, 0, sizeof(struct ip_extend_rule));
	rule_tmp.dscp = ACL_DEF_DSCP;
	rule_tmp.tos = ACL_DEF_TOS;
	
	if((NULL != argv[0])&&(argc != 1))
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(NULL != argv[1])
	{
		rule_tmp.protocol = (uint32_t)atoi(argv[1]);
		rule_tmp.proto_mask = 1;
		rule_tmp.ruleid = H3C_CONTRL_PLANE_PROCTOL;
	}
	if(NULL != argv[2])
	{
		rule_tmp.dscp = (uint32_t)atoi(argv[2]);
		rule_tmp.ruleid = H3C_DSCP;
	}
	if(NULL != argv[3])
	{
		rule_tmp.tos = (uint32_t)atoi(argv[3]);
		rule_tmp.ruleid = H3C_IP_PRECEDENCE;
	}

	ret = acl_group_add(ppolicy->acl_num);
	if(ret)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = acl_rule_add((struct ip_extend_rule* )&rule_tmp, ACL_TYPE_IP_EXTEND, ppolicy->acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
		
	return CMD_SUCCESS;
}


DEFUN  (h3c_qos_policy_acl_car,
		h3c_qos_policy_acl_car_cmd,
       "car cir committed-information-rate cbs <0-10000000>",
       "Qos car\n"
       "Car cir\n"
       "Committed-information-rate\n"
       "Egress\n"
	   "Cbs\n"
	   "Committed information rate value <0-10000000> (Unit: Kbps)\n")
{
	struct car_t car;
    struct car_t *pcar = NULL;
	struct qos_policy_h3c *ppolicy = NULL;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy->behavior_mask == 0)
	{
		vty_error_out(vty, "traffic behavior is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

    memset(&car, 0, sizeof(struct car_t));
    car.car_id = index_alloc(INDEX_TYPE_CAR) + QOS_CAR_INDEX_OFFSET;
	
	car.green_act.color  = QOS_COLOR_GREEN;
    car.green_act.drop   = QOS_CAR_PACKETS_PASS;
    car.yellow_act.color = QOS_COLOR_YELLOW;
    car.yellow_act.drop  = QOS_CAR_PACKETS_PASS;
    car.red_act.color    = QOS_COLOR_RED;
    car.red_act.drop     = QOS_CAR_PACKETS_DROP;
    car.color_blind = QOS_CAR_COLOR_AWARE;
    car.shared      = QOS_CAR_PROFILE_SHARED;

    pcar = qos_car_create(&car);
    if(NULL == pcar)
    {
        vty_error_out(vty, "QoS car profile created failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
    }

	ppolicy->behavior_mask = 1;
	ppolicy->action.action.act_num = 1;
	ppolicy->action.action.car = 1;  
	ppolicy->action.action.car_id = car.car_id;
	ppolicy->action.type = POLICY_TYPE_CAR;

	
    return CMD_SUCCESS;
}

DEFUN  (undo_h3c_qos_policy_acl_car,
	   undo_h3c_qos_policy_acl_car_cmd,
	  "undo car cir committed-information-rate cbs <0-10000000>",
	  "undo\n"
	  "Qos car\n"
	  "Car cir\n"
	  "Committed-information-rate\n"
	  "Egress\n"
	  "Cbs\n"
	  "Committed information rate value <0-10000000> (Unit: Kbps)\n")
{
#if 0
   struct car_t car;
   struct car_t *pcar = NULL;
   struct qos_policy_h3c *ppolicy = NULL;

   ppolicy = (struct qos_policy_h3c *)vty->index;
   if(ppolicy == NULL)
   {
	   vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
	   return CMD_WARNING;
   }
   if(ppolicy->behavior_mask == 1)
   {
	   vty_error_out(vty, "action has exist!!%s", VTY_NEWLINE);
	   return CMD_WARNING;
   }

   memset(&car, 0, sizeof(struct car_t));
   car.car_id = index_alloc(INDEX_TYPE_CAR) + QOS_CAR_INDEX_OFFSET;
   
   car.green_act.color	= QOS_COLOR_GREEN;
   car.green_act.drop	= QOS_CAR_PACKETS_PASS;
   car.yellow_act.color = QOS_COLOR_YELLOW;
   car.yellow_act.drop	= QOS_CAR_PACKETS_PASS;
   car.red_act.color	= QOS_COLOR_RED;
   car.red_act.drop 	= QOS_CAR_PACKETS_DROP;
   car.color_blind = QOS_CAR_COLOR_AWARE;
   car.shared	   = QOS_CAR_PROFILE_SHARED;

   pcar = qos_car_create(&car);
   if(NULL == pcar)
   {
	   vty_error_out(vty, "QoS car profile created failed!!%s", VTY_NEWLINE);
	   return CMD_WARNING;
   }

   ppolicy->behavior_mask = 1;
   ppolicy->action.action.act_num = 1;
   ppolicy->action.action.car = 1;	
   ppolicy->action.action.car_id = car.car_id;
   ppolicy->action.type = POLICY_TYPE_CAR;

 #endif  
   return CMD_SUCCESS;
}


DEFUN  (qos_policy_h3c_acl_filter,
		qos_policy_h3c_acl_filter_cmd,
       "filter (permit|deny)",
	   "Acl action: filter\n"
	   "Permit acl rule filter\n"
	   "Deny acl rule filter\n")
{
	struct qos_policy_h3c *ppolicy = NULL;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy->behavior_mask == 0)
	{
		vty_error_out(vty, "traffic behavior is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ppolicy->behavior_mask = 1;
	ppolicy->action.action.act_num = 1;
	ppolicy->action.action.filter = 1; 
	ppolicy->action.type = POLICY_TYPE_FILTER;

	if(!strncmp(argv[0], "deny", 1))
	{
		ppolicy->action.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[0], "permit", 1))
	{
		ppolicy->action.action.filter_act = 0; 
	}

	return CMD_SUCCESS;
}

DEFUN  (undo_qos_policy_h3c_acl_filter,
	undo_qos_policy_h3c_acl_filter_cmd,
	"undo filter",
	"undo\n"
	"Acl action: filter\n")
{
	struct qos_policy_h3c *ppolicy = NULL;

	ppolicy = (struct qos_policy_h3c *)vty->index;
	if(ppolicy == NULL)
	{
	   vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
	   return CMD_WARNING;
	}
	if(ppolicy->behavior_mask == 0)
	{
	   vty_error_out(vty, "traffic behavior has exist!!%s", VTY_NEWLINE);
	   return CMD_WARNING;
	}

	ppolicy->action.action.filter = 0; 
	ppolicy->action.action.filter_act = 0; 
	return CMD_SUCCESS;
}
DEFUN  (qos_policy_h3c_classifier_behavior,
		qos_policy_h3c_classifier_behavior_cmd,
		"classifier CLASSIFIER-NAME behavior BEHAVIOR-NAME",  
		"classifier\n"
		"classifier CLASSIFIER-NAME\n"
		"behavior\n"
		"behavior BEHAVIOR-NAME\n")
{
	int rv = 0;
	struct qos_policy_h3c *ppolicy_p = NULL;
	struct qos_policy_h3c *ppolicy_c = NULL;
	struct qos_policy_h3c *ppolicy_b = NULL;
	struct qos_policy_h3c policy_c;
	struct qos_policy_h3c policy_b;

	ppolicy_p = (struct qos_policy_h3c *)vty->index;
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "qos policy is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	/*²éÕÒclassifierÓë behavior ÅäÖÃÊÇ·ñ´æÔÚ*/
	memset(&policy_c, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_c.classifier_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_c = qos_policy_name_lookup(policy_c, QOS_CLASSIFIER_NAME );
	if(ppolicy_c == NULL)
	{
		vty_error_out(vty, "Find classifier struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_c->classifier_mask == 0)
	{
		vty_error_out(vty, "please set classifier configure!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	memset(&policy_b, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_b.behavior_name, argv[1], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_b = qos_policy_name_lookup(policy_b, QOS_BEHAVIOR_NAME);
	if(ppolicy_b == NULL)
	{
		vty_error_out(vty, "Find behavior struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_b->behavior_mask == 0)
	{
		vty_error_out(vty, "please set behavior configure!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memcpy(ppolicy_p->classifier_name, policy_c.classifier_name, sizeof(uchar)* NAME_STRING_LEN );
	memcpy(ppolicy_p->behavior_name, policy_b.behavior_name, sizeof(uchar)* NAME_STRING_LEN );
	memcpy(&ppolicy_p->action, &ppolicy_b->action, sizeof(struct acl_action));
	ppolicy_p->classifier_mask = 1;
	ppolicy_p->behavior_mask = 1;
	ppolicy_p->policy_mask = 1;
	ppolicy_p->acl_num = ppolicy_c->acl_num;
	
	return rv;
}

DEFUN  (undo_qos_policy_h3c_classifier_behavior,
		undo_qos_policy_h3c_classifier_behavior_cmd,
		"undo classifier CLASSIFIER-NAME behavior BEHAVIOR-NAME",  
		"undo\n"
		"classifier\n"
		"classifier CLASSIFIER-NAME\n"
		"behavior\n"
		"behavior BEHAVIOR-NAME\n")
{
	int rv = 0;
	struct qos_policy_h3c *ppolicy_p = NULL;
	struct qos_policy_h3c *ppolicy_c = NULL;
	struct qos_policy_h3c *ppolicy_b = NULL;
	struct qos_policy_h3c policy_c;
	struct qos_policy_h3c policy_b;

	ppolicy_p = (struct qos_policy_h3c *)vty->index;
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "policy is not exist!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	/*²éÕÒclassifierÓë behavior ÅäÖÃÊÇ·ñ´æÔÚ*/
	memset(&policy_c, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_c.classifier_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_c = qos_policy_name_lookup(policy_c, QOS_CLASSIFIER_NAME );
	if(ppolicy_c == NULL)
	{
		vty_error_out(vty, "Find classifier struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_c->classifier_mask == 0)
	{
		vty_error_out(vty, "please set classifier configure!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	memset(&policy_b, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_b.behavior_name, argv[1], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_b = qos_policy_name_lookup(policy_b, QOS_BEHAVIOR_NAME);
	if(ppolicy_b == NULL)
	{
		vty_error_out(vty, "Find behavior struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_b->behavior_mask == 0)
	{
		vty_error_out(vty, "please set behavior configure!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	memset(ppolicy_p->classifier_name, 0, sizeof(uchar)* NAME_STRING_LEN );
	memset(ppolicy_p->behavior_name, 0, sizeof(uchar)* NAME_STRING_LEN );
	memset(&ppolicy_p->action, 0, sizeof(struct acl_action));
	ppolicy_p->classifier_mask = 0;
	ppolicy_p->behavior_mask = 0;
	ppolicy_p->policy_mask = 0;
	ppolicy_p->acl_num = 0;
	
	return rv;
}

DEFUN  (qos_apply_policy_interface,
		qos_apply_policy_interface_cmd,
		"qos apply policy POLICY-NAME inbound",  
		"Qos\n"
		"Qos apply\n"
		"Qos policy\n"
		"Qos policy POLICY-NAME\n"
		"Qos policy inbound\n")
{
	int ret = 0;
	int rv = 0;
	struct qos_policy_h3c policy_p;
	struct qos_policy_h3c *ppolicy_p = NULL; 
	uint32_t      ifindex;  
	
	ifindex = (uint32_t)vty->index;
	
	memset(&policy_p, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_p.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_p = qos_policy_name_lookup(policy_p, QOS_POLICY_NAME);
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "please set policy configure with binding classifier and behavior!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ppolicy_p->ifindex = ifindex;
	ppolicy_p->action.ifindex = ifindex;
	ppolicy_p->action.direct = QOS_DIR_INGRESS;

	ret = qos_policy_add_if(ppolicy_p->acl_num, ppolicy_p->action.type, QOS_DIR_INGRESS, ifindex, &ppolicy_p->action.action);
 
	rv = qos_policy_return_str(ret,vty);
	return rv;
}

DEFUN  (undo_qos_apply_policy_interface,
		undo_qos_apply_policy_interface_cmd,
		"undo qos apply policy POLICY-NAME inbound",  
		"undo\n"
		"Qos\n"
		"Qos apply\n"
		"Qos policy\n"
		"Qos policy POLICY-NAME\n"
		"Qos policy inbound\n")
{
	int ret = 0;
	int rv = 0;
	struct qos_policy_h3c policy_p;
	struct qos_policy_h3c *ppolicy_p = NULL; 
	uint32_t	  ifindex;	
	
	ifindex = (uint32_t)vty->index;
	
	memset(&policy_p, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_p.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_p = qos_policy_name_lookup(policy_p, QOS_POLICY_NAME);
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "please set policy configure with binding classifier and behavior!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	ret = qos_policy_delete_if(ppolicy_p->acl_num, ppolicy_p->action.type, QOS_DIR_INGRESS, ifindex);
 
	rv = qos_policy_return_str(ret,vty);
	return rv;
}

DEFUN  (qos_apply_policy_global,
		qos_apply_policy_global_cmd,
		"qos apply policy POLICY-NAME global inbound",  
		"Qos\n"
		"Qos apply\n"
		"Qos policy\n"
		"Qos policy POLICY-NAME\n"
		"Qos policy global\n"
		"Qos policy inbound\n")
{
	int ret = 0;
	int rv = 0;
	struct qos_policy_h3c policy_p;
	struct qos_policy_h3c *ppolicy_p = NULL; 
	
	memset(&policy_p, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_p.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_p = qos_policy_name_lookup(policy_p, QOS_POLICY_NAME);
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "please set policy configure with binding classifier and behavior!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	ppolicy_p->ifindex = 0;
	ppolicy_p->action.ifindex = 0;
	ppolicy_p->action.direct = QOS_DIR_INGRESS;

	ret = qos_policy_add(ppolicy_p->acl_num, ppolicy_p->action.type, QOS_DIR_INGRESS, 0, &ppolicy_p->action.action);
 
	rv = qos_policy_return_str(ret,vty);
	
	return rv;
}

DEFUN  (undo_qos_apply_policy_global,
		undo_qos_apply_policy_global_cmd,
		"undo qos apply policy POLICY-NAME global inbound",	
		"undo\n"
		"Qos\n"
		"Qos apply\n"
		"Qos policy\n"
		"Qos policy POLICY-NAME\n"
		"Qos policy global\n"
		"Qos policy inbound\n")
{
	int ret = 0;
	int rv = 0;
	struct qos_policy_h3c policy_p;
	struct qos_policy_h3c *ppolicy_p = NULL; 
	
	memset(&policy_p, 0, sizeof(struct qos_policy_h3c));
	memcpy(policy_p.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN );
	ppolicy_p = qos_policy_name_lookup(policy_p, QOS_POLICY_NAME);
	if(ppolicy_p == NULL)
	{
		vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(ppolicy_p->policy_mask == 0)
	{
		vty_error_out(vty, "please set policy configure with binding classifier and behavior!!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	ret = qos_policy_delete(ppolicy_p->acl_num, ppolicy_p->action.type, QOS_DIR_INGRESS, 0);
 
	rv = qos_policy_return_str(ret,vty);
	
	return rv;
}

/**
* @brief	  if_match_mac_show
* @param[in ] struct vty *vty :vty
* @param[in ] struct mac_rule *mac_tmp :mac ruleç»“æž„ä½“
* @param[out] æ— 
* @return	  æ— 
* @author	  Wumm
* @date 	  2018å¹´6æœˆ15æ—¥ 14:00:42
* @note 	  æ— 
*/
static void if_match_mac_show ( struct vty *vty, struct mac_rule *mac_tmp)
{
	vty_out(vty," if-match");
	
	if(mac_tmp->smac_mask == 1)
	{
		vty_out(vty," source-mac %02x%02x-%02x%02x-%02x%02x",
			mac_tmp->smac[0],mac_tmp->smac[1],mac_tmp->smac[2],mac_tmp->smac[3],mac_tmp->smac[4],mac_tmp->smac[5]);
	}
	if(mac_tmp->dmac_mask == 1)
	{
		vty_out(vty," destination-mac %02x%02x-%02x%02x-%02x%02x",
			mac_tmp->dmac[0],mac_tmp->dmac[1],mac_tmp->dmac[2],mac_tmp->dmac[3],mac_tmp->dmac[4],mac_tmp->dmac[5]);
	}
	if(mac_tmp->vlan != ACl_DEF_SVLANID)
	{
		vty_out(vty," service-vlan-id %d",mac_tmp->vlan);
	}
	if(mac_tmp->cos != ACL_DEF_COS)
	{
		vty_out(vty," service-dot1p %d",mac_tmp->cos);
	}

	vty_out(vty,"%s",VTY_NEWLINE);

}


/**
* @brief	  if_match_ipex_show
* @param[in ] struct vty *vty :vty
* @param[in ] struct ip_extend_rule *ip_extend_tmp :ipex ruleç»“æž„ä½“
* @param[out] æ— 
* @return	  æ— 
* @author	  Wumm
* @date 	  2018å¹´6æœˆ13æ—¥ 14:00:42
* @note 	  æ— 
*/
static void if_match_ipex_show ( struct vty *vty, struct ip_extend_rule *ip_extend_tmp)
{
	vty_out(vty," if-match");

	if(ip_extend_tmp->proto_mask == 0)
	{
		vty_out(vty," any");
	}
	else
	{
		if((ip_extend_tmp->protocol != 1)&&(ip_extend_tmp->protocol != 2)
			&&(ip_extend_tmp->protocol != 4)&&(ip_extend_tmp->protocol != 6)
			&&(ip_extend_tmp->protocol != 17))
		{
			vty_out(vty," control-plane protocol 0x%02x",ip_extend_tmp->protocol);
		}
		else
			vty_out(vty," control-plane protocol %s",proto_type_parse_str(ip_extend_tmp->protocol));
		if(ip_extend_tmp->dscp != ACL_DEF_DSCP)
		{
			vty_out(vty," dscp %d",ip_extend_tmp->dscp);
		}
		if(ip_extend_tmp->tos != ACL_DEF_TOS)
		{
			vty_out(vty," ip-precedence %d",ip_extend_tmp->tos);
		}
	}
	
	vty_out (vty, "%s", VTY_NEWLINE);	

}

static void h3c_qos_policy_show(struct vty *vty, uint32_t type)
{
	struct qos_policy_h3c *qos_policy = NULL;
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct listnode *node_r = NULL;
	struct listnode *node_p = NULL;
	struct rule_node rule_node;

	if(type == QOS_CLASSIFIER_NAME || type == QOS_BEHAVIOR_NAME)
	{
		if(NULL != qos_policy_conf_list.head)
		{
			for (node = qos_policy_conf_list.head ; node; node = node->next)
			{
				qos_policy = listgetdata(node);
				if(qos_policy != NULL)
				{
					if(type == QOS_CLASSIFIER_NAME)
					{
						if(qos_policy->classifier_mask == 1)
						{
							vty_out(vty, "Classifier:%s%s",qos_policy->classifier_name, VTY_NEWLINE);
							
							acl_node = acl_group_lookup(qos_policy->acl_num);
							if(NULL == acl_node)
							{
								vty_error_out(vty,"acl %d doesn't exist!%s",qos_policy->acl_num, VTY_NEWLINE);
								return ;
							}
							if(acl_node->rulelist.head != NULL)
						   	{
							   	for (node_r = acl_node->rulelist.head ; node_r; node_r = node_r->next)
							   	{
									memcpy(&rule_node, listgetdata (node_r), sizeof(rule_node));
									
									if(rule_node.type == ACL_TYPE_MAC)
									{
										if_match_mac_show ( vty, &rule_node.rule.mac_acl);
									}
									if(rule_node.type == ACL_TYPE_IP_EXTEND)
									{
										if_match_ipex_show ( vty, &rule_node.rule.ipex_acl);
									}
							   	}
	    					}
						}
					}
					if(type == QOS_BEHAVIOR_NAME)
					{ 
						if(qos_policy->behavior_mask == 1)
						{
							vty_out(vty, "Behavior:%s%s",qos_policy->behavior_name, VTY_NEWLINE);
							if((qos_policy->action.action.act_num == 1) 
								&& (qos_policy->action.action.filter == 1)
								&& (qos_policy->action.type == POLICY_TYPE_FILTER))
							{
								if(qos_policy->action.action.filter_act == 1)
								{
									vty_out(vty, "filter deny%s", VTY_NEWLINE);
								}else if(qos_policy->action.action.filter_act == 0)
								{
									vty_out(vty, "filter permit%s", VTY_NEWLINE);
								}
							}
						}
					}
				}
			}
		}
	}
	else if(type == QOS_POLICY_NAME)
	{
		if(NULL != qos_policy_list.head)
		{
			for (node_p = qos_policy_list.head ; node_p; node_p = node_p->next)
			{
				qos_policy = listgetdata(node_p);
				if(qos_policy != NULL)
				{
					if(qos_policy->policy_mask == 1)
					{
						vty_out(vty, "Classifier: %s%s",qos_policy->classifier_name, VTY_NEWLINE);
						vty_out(vty, "Behavior: %s%s",qos_policy->behavior_name, VTY_NEWLINE);
					}
				}
			}
		}
	}
}

static void h3c_qos_policy_show_by_name(struct vty *vty, uint32_t type, struct qos_policy_h3c *qos_policy)
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;

	if(type == QOS_CLASSIFIER_NAME || type == QOS_BEHAVIOR_NAME)
	{
		
		if(type == QOS_CLASSIFIER_NAME)
		{
			if(qos_policy->classifier_mask == 1)
			{
				vty_out(vty, "Classifier:%s%s",qos_policy->classifier_name, VTY_NEWLINE);
				
				acl_node = acl_group_lookup(qos_policy->acl_num);
				if(NULL == acl_node)
				{
					vty_error_out(vty,"acl %d doesn't exist!%s",qos_policy->acl_num, VTY_NEWLINE);
					return ;
				}
				if(acl_node->rulelist.head != NULL)
			   	{
				   	for (node = acl_node->rulelist.head ; node; node = node->next)
				   	{
						memcpy(&rule_node, listgetdata (node), sizeof(rule_node));
						
						if(rule_node.type == ACL_TYPE_MAC)
						{
							if_match_mac_show ( vty, &rule_node.rule.mac_acl);
						}
						if(rule_node.type == ACL_TYPE_IP_EXTEND)
						{
							if_match_ipex_show ( vty, &rule_node.rule.ipex_acl);
						}
				   	}
				}
			}
		}
		if(type == QOS_BEHAVIOR_NAME)
		{ 
			if(qos_policy->behavior_mask == 1)
			{
				vty_out(vty, "Behavior:%s%s",qos_policy->behavior_name, VTY_NEWLINE);
				if((qos_policy->action.action.act_num == 1) 
					&& (qos_policy->action.action.filter == 1)
					&& (qos_policy->action.type == POLICY_TYPE_FILTER))
				{
					if(qos_policy->action.action.filter_act == 1)
					{
						vty_out(vty, "filter deny%s", VTY_NEWLINE);
					}else if(qos_policy->action.action.filter_act == 0)
					{
						vty_out(vty, "filter permit%s", VTY_NEWLINE);
					}
				}
			}
		}
	}
	else if(type == QOS_POLICY_NAME)
	{
		if(qos_policy->policy_mask == 1)
		{
			vty_out(vty, "Classifier: %s%s",qos_policy->classifier_name, VTY_NEWLINE);
			vty_out(vty, "Behavior: %s%s",qos_policy->behavior_name, VTY_NEWLINE);
		}
	}
}
DEFUN  (display_qos_mode,
	display_qos_mode_cmd,
	"display traffic (classifier|bebavior) user-defined [NAME]",
	"display qos policy\n"
	"Qos traffic\n"
	"Qos traffic classifier\n"
	"Qos traffic bebavior\n"
	"Qos traffic qos policy\n"
	"Qos traffic qos policy\n"
	"user defined Qos traffic\n"
	"Qos traffic NAME\n")
{
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy = NULL;
	enum QOS_POLICY_NAME_TYPE type;

	if(argc < 2)
	{
		vty_error_out(vty, "Input data is error!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(!strncmp(argv[0], "c", 1))
	{
		type = QOS_CLASSIFIER_NAME;
	}
	else if(!strncmp(argv[0], "b", 1))
	{
		type = QOS_BEHAVIOR_NAME;
	}
	else
	{
		vty_error_out(vty, "Input data is error!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(argv[1] != NULL)
	{
		
		memset(&policy, 0, sizeof(struct qos_policy_h3c));
		if(type == QOS_CLASSIFIER_NAME)
		{
			memcpy(policy.classifier_name, argv[1], sizeof(uchar)* NAME_STRING_LEN);
		}else if(type == QOS_BEHAVIOR_NAME)
		{
			memcpy(policy.behavior_name, argv[1], sizeof(uchar)* NAME_STRING_LEN);
		}
		ppolicy = qos_policy_name_lookup(policy, type);
		if(ppolicy == NULL)
		{
			vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		h3c_qos_policy_show_by_name(vty, type, ppolicy);		
		return CMD_SUCCESS;
	}
	h3c_qos_policy_show(vty, type);
	return CMD_SUCCESS;
}

DEFUN  (display_qos_poliyc_mode,
	display_qos_policy_mode_cmd,
	"display qos policy user-defined [NAME]",
	"display qos policy\n"
	"Qos traffic qos policy\n"
	"Qos traffic qos policy\n"
	"user defined Qos traffic\n"
	"Qos traffic NAME\n")
{
	struct qos_policy_h3c policy;
	struct qos_policy_h3c *ppolicy = NULL;
	enum QOS_POLICY_NAME_TYPE type;

	type = QOS_POLICY_NAME;
	
	if(argv[0] != NULL)
	{
		
		memset(&policy, 0, sizeof(struct qos_policy_h3c));
		memcpy(policy.policy_name, argv[0], sizeof(uchar)* NAME_STRING_LEN);
		ppolicy = qos_policy_name_lookup(policy, type);
		if(ppolicy == NULL)
		{
			vty_error_out(vty, "Find qos policy struct qos_policy_h3c failed!!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		h3c_qos_policy_show_by_name(vty, type, ppolicy);		
		return CMD_SUCCESS;
	}
	h3c_qos_policy_show(vty, type);
	return CMD_SUCCESS;
}

int qos_policy_classifer_h3c_config_write (struct vty *vty)
{		
	struct qos_policy_h3c *qos_policy = NULL;
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct listnode *node_r = NULL;
	struct rule_node rule_node;
	
	if(NULL != qos_policy_conf_list.head)
	{
		for (node = qos_policy_conf_list.head ; node; node = node->next)
		{
			qos_policy = listgetdata(node);
			if(qos_policy != NULL)
			{
				if(qos_policy->classifier_mask == 1)
				{
					vty_out(vty, "traffic classifier %s%s",qos_policy->classifier_name, VTY_NEWLINE);
					
					acl_node = acl_group_lookup(qos_policy->acl_num);
					if(NULL == acl_node)
					{
						vty_error_out(vty,"acl %d doesn't exist!%s",qos_policy->acl_num, VTY_NEWLINE);
						return ERRNO_NOT_FOUND;
					}
					if(acl_node->rulelist.head != NULL)
					{
						for (node_r = acl_node->rulelist.head ; node_r; node_r = node_r->next)
						{
							memcpy(&rule_node, listgetdata (node_r), sizeof(rule_node));
							
							if(rule_node.type == ACL_TYPE_MAC)
							{
								if_match_mac_show ( vty, &rule_node.rule.mac_acl);
							}
							if(rule_node.type == ACL_TYPE_IP_EXTEND)
							{
								if_match_ipex_show ( vty, &rule_node.rule.ipex_acl);
							}
						}
					}
				}	
			}
			
		}
		vty_out ( vty, "!%s", VTY_NEWLINE );
	}
    return ERRNO_SUCCESS;
}


int qos_policy_behavior_h3c_config_write (struct vty *vty)
{		
	struct qos_policy_h3c *qos_policy = NULL;
	struct listnode *node = NULL;
	
	if(NULL != qos_policy_conf_list.head)
	{
		for (node = qos_policy_conf_list.head ; node; node = node->next)
		{
			qos_policy = listgetdata(node);
			if(qos_policy != NULL)
			{
				if(qos_policy->behavior_mask == 1)
				{
					vty_out(vty, "traffic behavior %s%s",qos_policy->behavior_name, VTY_NEWLINE);
					if((qos_policy->action.action.act_num == 1) 
						&& (qos_policy->action.action.filter == 1)
						&& (qos_policy->action.type == POLICY_TYPE_FILTER))
					{
						if(qos_policy->action.action.filter_act == 1)
						{
							vty_out(vty, " filter deny%s", VTY_NEWLINE);
						}else if(qos_policy->action.action.filter_act == 0)
						{
							vty_out(vty, " filter permit%s", VTY_NEWLINE);
						}
					}
				}
			}
			
		}
	}
		
	vty_out ( vty, "!%s", VTY_NEWLINE );
    return ERRNO_SUCCESS;
}

int qos_policy_h3c_config_write (struct vty *vty)
{		
	struct qos_policy_h3c *qos_policy = NULL;
	struct listnode *node_p = NULL;
	char ifname[IFNET_NAMESIZE];
	int ret = 0;
	
	if(NULL != qos_policy_list.head)
	{
		for (node_p = qos_policy_list.head ; node_p; node_p = node_p->next)
		{
			qos_policy = listgetdata(node_p);
			if(qos_policy != NULL)
			{
				if(qos_policy->policy_mask == 1)
				{
				
					vty_out(vty, "qos policy %s%s",qos_policy->policy_name, VTY_NEWLINE);
					vty_out(vty, " classifier %s behavior %s%s",qos_policy->classifier_name, qos_policy->behavior_name, VTY_NEWLINE);
					if(qos_policy->ifindex == 0 && qos_policy->action.direct == QOS_DIR_INGRESS)
					{
						vty_out(vty, "qos apply policy %s global inbound%s", qos_policy->policy_name, VTY_NEWLINE);
					}
					if(qos_policy->ifindex != 0 && qos_policy->action.direct == QOS_DIR_INGRESS)
					{
						ret = ifm_get_name_by_ifindex(qos_policy->ifindex, ifname);
						if(ret < 0)
						{
						    assert(0);
						}
						vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE );
						vty_out(vty, " qos apply policy %s inbound%s", qos_policy->policy_name, VTY_NEWLINE);
					}
				}
			}
		}
		
		vty_out ( vty, "!%s", VTY_NEWLINE );
	}
    return ERRNO_SUCCESS;
}
void qos_policy_h3c_cmd_init(void)
{
	install_node (&qos_classifier_node, NULL);
	install_default (QOS_CLASSIFIER_NODE);
	install_node (&qos_behavior_node, NULL);
	install_default (QOS_BEHAVIOR_NODE);
	install_node (&qos_policy1_node, NULL);
	install_default (QOS_POLICY1_NODE);
		
	install_element (CONFIG_NODE, &qos_classifier_mode_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &qos_bebavior_mode_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &h3c_qos_policy_mode_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &qos_apply_policy_global_cmd, CMD_SYNC);
//	install_element (CONFIG_NODE, &display_qos_policy_acl_cmds);

	install_element (QOS_CLASSIFIER_NODE, &qos_if_match_mac_cmd, CMD_SYNC);
	install_element (QOS_CLASSIFIER_NODE, &qos_if1_match_ipex_cmd, CMD_SYNC);
	
	install_element (QOS_BEHAVIOR_NODE, &h3c_qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (QOS_BEHAVIOR_NODE, &qos_policy_h3c_acl_filter_cmd, CMD_SYNC);

	install_element (QOS_POLICY1_NODE, &qos_policy_h3c_classifier_behavior_cmd, CMD_SYNC);
	
	install_element (PHYSICAL_IF_NODE, &qos_apply_policy_interface_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &qos_apply_policy_interface_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &undo_h3c_qos_classifier_mode_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &undo_h3c_qos_behavior_mode_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &undo_h3c_qos_policy_mode_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_qos_apply_policy_global_cmd, CMD_SYNC);

	install_element (QOS_CLASSIFIER_NODE, &undo_qos_if_match_mac_cmd, CMD_SYNC);

	install_element (QOS_BEHAVIOR_NODE, &undo_h3c_qos_policy_acl_car_cmd, CMD_SYNC);
	install_element (QOS_BEHAVIOR_NODE, &undo_qos_policy_h3c_acl_filter_cmd, CMD_SYNC);

	install_element (QOS_POLICY1_NODE, &undo_qos_policy_h3c_classifier_behavior_cmd, CMD_SYNC);

	install_element (PHYSICAL_IF_NODE, &undo_qos_apply_policy_interface_cmd, CMD_SYNC);
	install_element (PHYSICAL_SUBIF_NODE, &undo_qos_apply_policy_interface_cmd, CMD_SYNC);
	
	install_element (CONFIG_NODE, &display_qos_mode_cmd, CMD_LOCAL);
	
	install_element (CONFIG_NODE, &display_qos_policy_mode_cmd, CMD_LOCAL);
}


