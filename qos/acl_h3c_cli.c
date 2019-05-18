#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lib/types.h>
#include <lib/command.h>
#include <lib/linklist.h>
#include <lib/prefix.h>
#include <lib/errcode.h>
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>

#include "acl.h"
#include "qos_policy.h"
#include "qos_if.h"
#include "acl_cli.h"
#include "acl_h3c_cli.h"

struct cmd_node acl_basic_node =
{
  ACL_BASIC_NODE,
  "%s(config-basic-acl)# ",
  0,
};

struct cmd_node acl_advanced_node =
{
  ACL_ADVANCED_NODE,
  "%s(config-advanced-acl)# ",
  0,
};

struct cmd_node acl_mac_node =
{
  ACL_MAC_NODE,
  "%s(config-mac-acl)# ",
  0,
};


DEFUN(acl_h3c_num,
		acl_h3c_num_cmd,
		"acl {basic <2000-2999>|advanced <3000-3999>|mac <4000-4999>} ",
		"Acl rule\n"
		"Acl group for basic acl configure\n"
		"Acl basic id <2000-2999>\n"
		"Acl group for advanced acl configure\n"
		"Acl advanced id <3000-3999>\n"
		"Acl group for mac acl configure\n"
		"Acl mac id <4000-4999>\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;

	if(NULL != argv[0])
	{
		acl_num =(uint32_t) atoi(argv[0]);

		vty->index = (void *)acl_num;
		vty->node = ACL_BASIC_NODE;
	}
	else if(NULL != argv[1])
	{
		acl_num =(uint32_t) atoi(argv[1]);

		vty->index = (void *)acl_num;
		vty->node = ACL_ADVANCED_NODE;
	}
	else if(NULL != argv[2])
	{
		acl_num =(uint32_t) atoi(argv[2]);
		
		vty->index = (void *)acl_num;
		vty->node = ACL_MAC_NODE;
	}

	ret = acl_group_add(acl_num);
	if(ret)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}

DEFUN(undo_acl_h3c_num,
		undo_acl_h3c_num_cmd,
		"undo acl {basic <2000-2999>|advanced <3000-3999>|mac <4000-4999>} ",
		"Undo acl rule\n"
		"Acl rule\n"
		"Acl group for basic acl configure\n"
		"Acl basic id <2000-2999>\n"
		"Acl group for advanced acl configure\n"
		"Acl advanced id <3000-3999>\n"
		"Acl group for mac acl configure\n"
		"Acl mac id <4000-4999>\n")
{
	uint8_t ret = 0;
	uint32_t acl_num = 0;

	if(argc != 3)
	{
		vty_error_out(vty, "please check out %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL != argv[0])
	{
		acl_num = (uint32_t)atoi(argv[0]);
	}
	else if(NULL != argv[1])
	{
		acl_num = (uint32_t)atoi(argv[1]);
	}
	else if(NULL != argv[2])
	{
		acl_num = (uint32_t)atoi(argv[2]);
	}

	ret = acl_group_delete(acl_num);
	if(ret)
	{
		vty_error_out(vty, "cannot find this acl group ,please check out%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	return CMD_SUCCESS;
}


DEFUN(acl_description,
		acl_description_cmd,
		"description TEXT",
		"ACL description\n"
		"ACL description (no more than 32 characters)\n")
{ 
	struct acl_group *acl_node = NULL;
	
	acl_node = acl_group_lookup((uint32_t)(vty->index));
	if(acl_node == NULL)
	{
		vty_error_out(vty, "can not find this acl_group%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memcpy(&acl_node->name,argv[0], sizeof(uchar)* NAME_STRING_LEN );
	
	return CMD_SUCCESS;
}


DEFUN(undo_acl_description,
		undo_acl_description_cmd,
		"undo description",
		NO_STR
		"Acl description\n")
{
	struct acl_group *acl_node = NULL;
	
	acl_node = acl_group_lookup( ( uint32_t )vty->index);
	if(acl_node == NULL)
	{
		vty_error_out(vty, "can not find this acl_group%s", VTY_NEWLINE);
		return CMD_WARNING;
	} 
	
	if (acl_node->name[0] != '\0' )
	{
		memset(acl_node->name, 0, sizeof(uchar)* NAME_STRING_LEN );
	}
	else
	{
		vty_error_out(vty, "this acl group has no name%s",VTY_NEWLINE);
		return CMD_WARNING;
	}
			
	return CMD_SUCCESS;
}


DEFUN(acl_ip_h3c_rule_any,
	acl_ip_h3c_rule_any_cmd,
	"rule <1-65534> (deny|permit) source any",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl ip rule filter deny\n"
	"Acl ip rule filter permit\n"
	"Source ip\n"
	"Ip address:any\n")
{
	int rv = 0;
	int ret = 0;
	uint32_t acl_num = 0;
	struct ip_rule rule_tmp;
	struct qos_policy qos_policy;
	struct acl_group * acl_node = NULL;
	
	acl_num = ( uint32_t )vty->index;	
	memset(&rule_tmp, 0, sizeof(struct ip_rule));
	
	rule_tmp.ruleid = atoi(argv[0]);

	
	rule_tmp.ip = 0;
	rule_tmp.ip_masklen = 0;

	
	ret = acl_rule_add((struct ip_rule *) &rule_tmp, ACL_TYPE_IP, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&qos_policy.action,0,sizeof(struct acl_action));	
	qos_policy.action.filter = 1;  
	qos_policy.action.act_num = 1;
	qos_policy.acl_num = acl_num;
	
	if(!strncmp(argv[1], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[1], "permit", 1))
	{
		qos_policy.action.filter_act = 0; 
	} 

	ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER,QOS_DIR_INGRESS, 0, &qos_policy.action);

	rv = qos_policy_return_str(ret,vty);

	return rv;
}

/* 反掩码转掩码数字 */
uint32_t wildcard_to_mask(uint32_t wildcard)
{
	
	uint32_t mask_all = 0xffffffff;
	uint32_t mask;
	uint32_t n = 0;

	mask = mask_all - wildcard;
	for(n = 0; n < 32; n++)
	{
		if(!(mask << n))
		{
			return n;
		}
	}
	return 0;
}
/* 掩码数字转反掩码 */
uint32_t mask_to_wildcard(uint32_t n)
{	
	uint32_t mask_all = 0xffffffff;
	uint32_t mask;

	mask = mask_all >> n;
	return mask;
}

DEFUN(acl_ip_h3c_rule,
	acl_ip_h3c_rule_cmd,
	"rule <1-65534> (deny|permit) source A.B.C.D A.B.C.D",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl ip rule filter deny\n"
	"Acl ip rule filter permit\n"
	"Source ip\n"
	"Source ip address A.B.C.D\n"
	"Source ip address wildcard A.B.C.D\n")
{
	int rv = 0;
	int ret = 0;
	uint32_t acl_num = 0;
	struct ip_rule rule_tmp;
	struct qos_policy qos_policy;
	struct acl_group * acl_node = NULL;
	
	acl_num = ( uint32_t )vty->index;	
	memset(&rule_tmp, 0, sizeof(struct ip_rule));
	
	rule_tmp.ruleid = atoi(argv[0]);
	
	if((NULL != argv[2])&&(NULL != argv[3]))
	{
		rule_tmp.ip_mask = 1;
		rule_tmp.ip_flag = 0;

		rule_tmp.ip = inet_strtoipv4((char *)argv[2]);
		rule_tmp.ip_masklen = wildcard_to_mask(inet_strtoipv4((char *)argv[3]));	
	}
		
	ret = acl_rule_add((struct ip_rule *) &rule_tmp, ACL_TYPE_IP, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	
	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&qos_policy.action,0,sizeof(struct acl_action)); 
	qos_policy.action.filter = 1;  
	qos_policy.action.act_num = 1;
	qos_policy.acl_num = acl_num;
	
	if(!strncmp(argv[1], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[1], "permit", 1))
	{
		qos_policy.action.filter_act = 0; 
	} 

	ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER,QOS_DIR_INGRESS, 0, &qos_policy.action);

	rv = qos_policy_return_str(ret,vty);

	return rv;
}

DEFUN(no_acl_ip_h3c_rule,
	no_acl_ip_h3c_rule_cmd,
	"undo rule <1-65534>",
	NO_STR
	"Acl ip rule\n"
	"Acl rule\n")
{
	int ret =0;
	int rv =0;
	uint16_t ruleid = 0;
	uint32_t acl_num = 0;
	struct acl_group * acl_node = NULL;
	struct qos_policy qos_policy;
	
	acl_num =  ( uint32_t )vty->index;
	
	if(NULL != argv[0])
	{
		ruleid =(uint16_t)atoi(argv[0]);
	}

	ret = acl_rule_delete(ruleid ,acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule delete fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	} 
	
	memset(&qos_policy,0,sizeof(struct qos_policy));
	qos_policy.direct = QOS_DIR_INGRESS;
	qos_policy.acl_num = acl_num;	
	
	acl_node = acl_group_lookup(qos_policy.acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	ret = qos_policy_delete(qos_policy.acl_num, POLICY_TYPE_FILTER,QOS_DIR_INGRESS, 0);
				
	rv = qos_policy_return_str(ret,vty);

	return rv;
}
	
/* 不支持：fragment */
DEFUN(acl_ip_extend_h3c_rule,
	acl_ip_extend_h3c_rule_cmd,
	"rule <1-65534> (deny|permit) HH {destionation A.B.C.D A.B.C.D|source A.B.C.D A.B.C.D|fragment}",
	"Acl rule\n"
	"Acl rule number <1-65534>\n"
	"Acl ip-extended rule filter deny\n"
	"Acl ip-extended rule filter permit\n"
	"Acl ip-extended protocal  <0x00-0xff>\n"
	"Acl ip-extended destination ip:A.B.C.D\n"
	"Acl ip-extended destination ip_mask:A.B.C.D\n"
	"Acl ip-extended source ip:A.B.C.D\n"
	"Acl ip-extended source ip_mask:A.B.C.D\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl <1-255>\n"
	"Acl ip-extended fragment\n")
{
	int rv = 0;
	int ret = 0;
	uint32_t acl_num = 0;
	struct ip_extend_rule ip_extend_tmp;
	struct qos_policy qos_policy;
	struct acl_group * acl_node = NULL;
	
	acl_num =  ( uint32_t )vty->index;
	memset(&ip_extend_tmp, 0, sizeof(struct ip_extend_rule));
	ip_extend_tmp.dscp = ACL_DEF_DSCP;
	ip_extend_tmp.tos = ACL_DEF_TOS;

	ip_extend_tmp.ruleid = atoi(argv[0]);

	if(NULL != argv[2])
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"protocol",&ip_extend_tmp.protocol, (char *)argv[2],2,2, ACL_IPEX_PROTOCOL_MAX );
		ip_extend_tmp.proto_mask = 1;
	}
	
	if(NULL != argv[3])
	{
		ip_extend_tmp.dip = ntohl(inet_strtoipv4((char *)argv[3]));
		ip_extend_tmp.dip_masklen = wildcard_to_mask(inet_strtoipv4((char *)argv[4]));	;	
		ip_extend_tmp.dip_mask = 1;
	}
	if(NULL != argv[5])
	{
		ip_extend_tmp.sip = ntohl(inet_strtoipv4((char *)argv[5]));
		ip_extend_tmp.sip_masklen = wildcard_to_mask(inet_strtoipv4((char *)argv[6]));	
		ip_extend_tmp.sip_mask = 1;
	}
	
	if(NULL != argv[7])
	{
		ip_extend_tmp.fragment_mask = 1;
	}
	
	ret = acl_rule_add((struct ip_extend_rule* )&ip_extend_tmp, ACL_TYPE_IP_EXTEND, acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&qos_policy.action,0,sizeof(struct acl_action));	
	qos_policy.action.filter = 1;  
	qos_policy.action.act_num = 1;
	qos_policy.acl_num = acl_num;
	
	if(!strncmp(argv[1], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[1], "permit", 1))
	{
		qos_policy.action.filter_act = 0; 
	} 

	ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0, &qos_policy.action);

	rv = qos_policy_return_str(ret,vty);

	return rv;
}

/* 不支持：fragment */
DEFUN(acl_ip_extend_2_h3c_rule,
	acl_ip_extend_2_h3c_rule_cmd,
	"rule <1-65534> (deny|permit) (tcp|udp) {sport <1-65535>|dport <1-65535>|destionation A.B.C.D A.B.C.D|source A.B.C.D A.B.C.D|fragment|tcp-flag |fin|syn|rst|psh|ack|urg}",
	"Acl rule\n"
	"Acl rule number <1-65534>\n"
	"Acl ip-extended rule filter deny\n"
	"Acl ip-extended rule filter permit\n"
	"Acl ip-extended protocal tcp\n"
	"Acl ip-extended protocal udp\n"
	"Acl ip-extended sport <1-65535>\n"
	"Acl ip-extended dport <1-65535>\n"
	"Acl ip-extended destination ip:A.B.C.D\n"
	"Acl ip-extended destination ip_mask:A.B.C.D\n"
	"Acl ip-extended source ip:A.B.C.D\n"
	"Acl ip-extended source ip_mask:A.B.C.D\n"
	"Acl ip-extended ttl\n"
	"Acl ip-extended ttl <1-255>\n"
	"Acl ip-extended fragment\n"
	"Acl ip-extended tcp type\n"
	"Acl ip-extended tcp type: fin \n"
	"Acl ip-extended tcp type: syn \n"
	"Acl ip-extended tcp type: rst \n"
	"Acl ip-extended tcp type: psh \n"
	"Acl ip-extended tcp type: ack \n"
	"Acl ip-extended tcp type: urg \n")
{
	int rv = 0;
	int ret = 0;
	uint32_t acl_num = 0;
	struct ip_extend_rule ip_extend_tmp;
	struct qos_policy qos_policy;
	struct acl_group * acl_node = NULL;
	
	acl_num =  ( uint32_t )vty->index;
	memset(&ip_extend_tmp, 0, sizeof(struct ip_extend_rule));

	ip_extend_tmp.ruleid = atoi(argv[0]);
	
	ip_extend_tmp.dscp = ACL_DEF_DSCP;
	ip_extend_tmp.tos = ACL_DEF_TOS;

	/*protocol: 6-----tcp;17------udp*/
	if(!strncmp(argv[2], "tcp", 1))
	{
		ip_extend_tmp.protocol = 6;
		ip_extend_tmp.proto_mask = 1;
	}
	else if(!strncmp(argv[2], "udp", 1))
	{
		ip_extend_tmp.protocol = 17;
		ip_extend_tmp.proto_mask = 1;
	}
	if(NULL != argv[3])
	{
		ip_extend_tmp.sport = atoi(argv[3]);
		ip_extend_tmp.sport_mask = 1;
	}
	if(NULL != argv[4])
	{
		ip_extend_tmp.dport = atoi(argv[4]);
		ip_extend_tmp.dport_mask = 1;
	}
	
	if(NULL != argv[5])
	{
		ip_extend_tmp.dip = inet_strtoipv4((char *)argv[5]);
		ip_extend_tmp.dip_masklen = wildcard_to_mask(inet_strtoipv4((char *)argv[6]));	
		ip_extend_tmp.dip_mask = 1;
	}
	if(NULL != argv[7])
	{
		ip_extend_tmp.sip = inet_strtoipv4((char *)argv[7]);
		ip_extend_tmp.sip_masklen = wildcard_to_mask(inet_strtoipv4((char *)argv[8]));	
		ip_extend_tmp.sip_mask = 1;
	}
	
	if(NULL != argv[9])
	{
		ip_extend_tmp.fragment_mask = 1;
	}

	if(ip_extend_tmp.protocol == 6)
	{
		if(NULL != argv[10])
		{
			if(NULL != argv[11])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_FIN;
			}
			if(NULL != argv[12])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_SYN;
			}
			if(NULL != argv[13])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_RST;
			}
			if(NULL != argv[14])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_PSH;
			}
			if(NULL != argv[15])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_ACK;
			}
			if(NULL != argv[16])
			{
				ip_extend_tmp.tcp_type += ACl_TCP_FLAG_URG;
			}
		}
	}
	else
	{
		if((NULL != argv[10])||(NULL != argv[11])||(NULL != argv[12])
			||(NULL != argv[13])||(NULL != argv[14])||(NULL != argv[15])||(NULL != argv[16]))
		{
			vty_error_out(vty, "tcp-type flags fin/syn/rst/psh/ack/urg only set when protocal is tcp.%s", VTY_NEWLINE);	
			return CMD_WARNING;
		}	
	}
	
	ret = acl_rule_add((struct ip_extend_rule* )&ip_extend_tmp, ACL_TYPE_IP_EXTEND, acl_num);
	if(ret) 
	{
		vty_error_out(vty, "acl rule add fail %s", VTY_NEWLINE);
		return CMD_WARNING;
	}


	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&qos_policy.action,0,sizeof(struct acl_action));	
	qos_policy.action.filter = 1;  
	qos_policy.action.act_num = 1;
	qos_policy.acl_num = acl_num;
	
	if(!strncmp(argv[1], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[1], "permit", 1))
	{
		qos_policy.action.filter_act = 0; 
	} 

	ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0, &qos_policy.action);

	rv = qos_policy_return_str(ret,vty);

	return rv;
}


/* 不支持：lsap untag */
DEFUN(acl_mac_h3c_rule,
	acl_mac_h3c_rule_cmd,
	"rule <1-65534> (deny|permit) {dest-mac H-H-H H-H-H|source-mac H-H-H H-H-H|type HHHH HHHH|lsap HHHH HHHH|vlan <1-4095>|cos <0-7>|untag}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl mac rule filter deny\n"
	"Acl mac rule filter permit\n"
	"Destination mac\n"
	"Destination mac address:H-H-H\n"
	"Destination mac address mask:H-H-H\n"
	"Source mac\n"
	"Source mac address:H-H-H\n"
	"Source mac address mask:H-H-H\n"
	"Ethernet type"
	"Ethernet type <0x0600-0xffff>\n"
	"Ethernet type mask <0x0000-0xffff>\n"
	"LSAP\n"
	"LSAP <0x0000-0xffff>\n"
	"LSAP mask <0x0000-0xffff>\n"	
	"Vlan id\n"
	"Vlan id <1-4095>\n"
	"Cos id\n"
	"Cos id <0-7>\n"
	"Untag \n")
{
	int rv = 0;
	int ret = 0;
	uint32_t acl_num = 0;
	struct mac_rule rule_tmp;
	struct qos_policy qos_policy;
	struct acl_group * acl_node = NULL;

	if(argc < 3)
	{
		vty_error_out(vty, "please check out argc%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	acl_num =  ( uint32_t )vty->index;
	memset(&rule_tmp, 0, sizeof(struct mac_rule));
	rule_tmp.vlan = ACl_DEF_SVLANID;
	rule_tmp.cvlan = ACl_DEF_CVLANID;
	rule_tmp.cos = ACL_DEF_COS;
	rule_tmp.cvlan_cos = ACL_DEF_CLAN_COS;
	rule_tmp.offset_len = ACL_DEF_OFFSET;
	rule_tmp.dscp = ACL_DEF_DSCP;
	rule_tmp.tos = ACL_DEF_TOS;

	rule_tmp.ruleid = atoi(argv[0]);
	
	if((NULL != argv[2])&&(NULL != argv[3]))
	{	
		ether_string1_to_mac((char *)argv[2], (uchar *)rule_tmp.dmac);
		//ether_string1_to_mac((char *)argv[3], (uchar *)rule_tmp.dmac_wildcard);
		rule_tmp.dmac_mask = 1;
	}
	if((NULL != argv[4])&&(NULL != argv[5]))
	{		
		ether_string1_to_mac((char *)argv[4], (uchar *)rule_tmp.smac);
		//ether_string1_to_mac((char *)argv[5], (uchar *)rule_tmp.smac_wildcard);
		rule_tmp.smac_mask = 1;
	}

	if((NULL != argv[6])&&(NULL != argv[7]))
	{
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"ethtype",&rule_tmp.ethtype, (char *)argv[6],4,4, ACL_MAC_ETHTYPE_MAX );
		//ACL_GET_INTEGER_RANGE_HEX ( (char *)"ethtype_mask",&rule_tmp.ethtype_wildcard, (char *)argv[7],4,4, ACL_MAC_ETHTYPE_MAX);
		rule_tmp.ethtype_mask = 1;
	}

	if((NULL != argv[8])&&(NULL != argv[9]))
	{
	#if 0
		rule_tmp.offset_mask = 1;
		rule_tmp.offset_len = atoi(argv[9]);
		max_len = (4 - rule_tmp.offset_len)*2;
		ACL_GET_INTEGER_RANGE_HEX ( (char *)"offset_info",&rule_tmp.offset_info , (char *)argv[8],max_len,0, ACL_MAC_OFFSET_MAX );	
	#endif
	}

	if(NULL != argv[10])
	{
		rule_tmp.vlan = atoi(argv[10]);
	}
	if(NULL != argv[11])
	{
		rule_tmp.cos = atoi(argv[11]);
	}
	if(NULL != argv[12])
	{
		rule_tmp.untag_mask = atoi(argv[12]);
	}
	
	ret = acl_rule_add((struct mac_rule* )&rule_tmp, ACL_TYPE_MAC, acl_num);
	if(ret)
	{
		vty_error_out(vty, "acl rule add error %s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	acl_node = acl_group_lookup(acl_num);
	if(NULL == acl_node)
	{
		vty_error_out(vty, "can not find this acl_group,please config acl_group first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	memset(&qos_policy.action,0,sizeof(struct acl_action));	
	qos_policy.action.filter = 1;  
	qos_policy.action.act_num = 1;	
	qos_policy.acl_num = acl_num;
	
	if(!strncmp(argv[1], "deny", 1))
	{
		qos_policy.action.filter_act = 1;  // 0: permit, 1: deny 
	}
	else if(!strncmp(argv[1], "permit", 1))
	{
		qos_policy.action.filter_act = 0; 
	} 

	ret = qos_policy_add(qos_policy.acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0, &qos_policy.action);

	rv = qos_policy_return_str(ret,vty);
	
	return rv;
}

/**
* @brief	  h3c_acl_rule_ip_show
* @param[in ] struct vty *vty :vty
* @param[in ] struct ip_rule *rule_tmp :ip rule结构体
* @param[in ] uint32_t acl_num
* @param[out] 无
* @return	  无
* @author	  Wumm
* @date 	  2018年6月13日 14:00:42
* @note 	  无
*/
static void h3c_acl_rule_ip_show ( struct vty *vty, struct ip_rule *rule_tmp, uint32_t acl_num)
{
	struct prefix_ipv4 ipv4;
	char ipv4_addr[16];
	struct qos_policy *qos_policy = NULL;
	uint32_t ip_m = 0;
	char ip_mask[16];
	
	qos_policy = qos_policy_lookup(acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0);	
	if(qos_policy == NULL)
	{
		QOS_LOG_ERR("The rule is not exit!\n");
		return ;
	}
	
	vty_out(vty," rule %d",rule_tmp->ruleid );

	vty_out(vty, " %s", qos_policy->action.filter_act ? "deny" : "permit");
	if(rule_tmp->ip_mask == 1)
	{
		if(rule_tmp->ip == 0)
		{
			vty_out(vty, " %s any",rule_tmp->ip_flag ? "destionation" : "source");
		}
		else
		{
			ipv4.prefix.s_addr = htonl(rule_tmp->ip);
			inet_ntop ( AF_INET, ( void * ) &ipv4.prefix, ipv4_addr, 100 );
			ip_m = mask_to_wildcard(rule_tmp->ip_masklen);
			inet_ntop ( AF_INET, ( void * ) &ip_m, ip_mask, 100 );
			vty_out(vty, " %s %s %s",rule_tmp->ip_flag ? "destionation" : "source", ipv4_addr, ip_mask);
		}
	}
	
	vty_out(vty,"%s",VTY_NEWLINE);
}


/**
* @brief	  h3c_acl_rule_ipex_show
* @param[in ] struct vty *vty :vty
* @param[in ] struct ip_extend_rule *ip_extend_tmp :ipex rule结构体
* @param[in ] uint32_t acl_num
* @param[out] 无
* @return	  无
* @author	  Wumm
* @date 	  2018年6月13日 14:00:42
* @note 	  无
*/
static void h3c_acl_rule_ipex_show ( struct vty *vty, struct ip_extend_rule *ip_extend_tmp, uint32_t acl_num)
{
	struct prefix_ipv4 dip;
	struct prefix_ipv4 sip;
	char dip_addr[16];
	char sip_addr[16];
	char dip_mask[16];
	char sip_mask[16];
	uint32_t dip_m = 0;
	uint32_t sip_m = 0;
	
	struct qos_policy *qos_policy = NULL;
	
	qos_policy = qos_policy_lookup(acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0);	
	if(qos_policy == NULL)
	{
		QOS_LOG_ERR("The rule is not exit!\n");
		return ;
	}
	vty_out(vty," rule %d ",ip_extend_tmp->ruleid );
	
	vty_out(vty, " %s", qos_policy->action.filter_act ? "deny" : "permit");
	if(ip_extend_tmp->proto_mask == 0)
	{
		vty_out(vty," protocol any");
	}
	else
	{
		if((ip_extend_tmp->protocol != 1)&&(ip_extend_tmp->protocol != 2)
			&&(ip_extend_tmp->protocol != 4)&&(ip_extend_tmp->protocol != 6)
			&&(ip_extend_tmp->protocol != 17))
		{
			vty_out(vty," protocol 0x%02x",ip_extend_tmp->protocol);
		}
		else
			vty_out(vty," protocol %s",proto_type_parse_str(ip_extend_tmp->protocol));

		if((ip_extend_tmp->protocol == 6)||(ip_extend_tmp->protocol == 17))
		{
			if(ip_extend_tmp->sport_mask == 1)
				vty_out(vty," sport %d",ip_extend_tmp->sport);
			if(ip_extend_tmp->dport_mask == 1)
				vty_out(vty," dport %d",ip_extend_tmp->dport);
		}
	}
	if(ip_extend_tmp->sip_mask == 1)
	{
		sip.prefix.s_addr = htonl(ip_extend_tmp->sip);
		inet_ntop ( AF_INET, ( void * ) &sip.prefix, sip_addr, 100 );	
		sip_m = mask_to_wildcard(ip_extend_tmp->sip_masklen);
		inet_ntop ( AF_INET, ( void * ) &sip_m, sip_mask, 100 );
		vty_out(vty," source %s %s",sip_addr, sip_mask);
	}
	if(ip_extend_tmp->dip_mask == 1)
	{
		dip.prefix.s_addr = htonl(ip_extend_tmp->dip);
		inet_ntop ( AF_INET, ( void * ) &dip.prefix, dip_addr, 100 );
		dip_m = mask_to_wildcard(ip_extend_tmp->dip_masklen);
		inet_ntop ( AF_INET, ( void * ) &dip_m, dip_mask, 100 );
		vty_out(vty," destionation %s %s", dip_addr, dip_mask);
	}
	if((ip_extend_tmp->proto_mask == 1)&&(ip_extend_tmp->protocol == 6)&&(ip_extend_tmp->tcp_type != 0))
	{
		vty_out(vty," tcp-flag");
		tcp_type_parse_str(ip_extend_tmp->tcp_type,vty);
	}
	vty_out (vty, "%s", VTY_NEWLINE);	

}

/**
* @brief	  h3c_acl_rule_mac_show
* @param[in ] struct vty *vty :vty
* @param[in ] struct mac_rule *mac_tmp :mac rule结构体
* @param[in ] uint32_t acl_num 
* @param[out] 无
* @return	  无
* @author	  Wumm
* @date 	  2018年6月13日 14:00:42
* @note 	  无
*/
static void h3c_acl_rule_mac_show ( struct vty *vty, struct mac_rule *mac_tmp, uint32_t acl_num)
{	
	struct qos_policy *qos_policy = NULL;
	
	qos_policy = qos_policy_lookup(acl_num, POLICY_TYPE_FILTER, QOS_DIR_INGRESS, 0);	
	if(qos_policy == NULL)
	{
		QOS_LOG_ERR("The rule is not exit!\n");
		return ;
	}

	vty_out(vty," rule %d",mac_tmp->ruleid );
	
	vty_out(vty, " %s", qos_policy->action.filter_act ? "deny" : "permit");
	if(mac_tmp->ethtype_mask == 1)
	{
		if(mac_tmp->ethtype == 0)
		{
			vty_out(vty," type any");
		}
		else
		{
			vty_out(vty," type 0x%04x fffff",mac_tmp->ethtype );
		}
	}
	if(mac_tmp->smac_mask == 1)
	{
		vty_out(vty," source-mac %02x%02x-%02x%02x-%02x%02x ffff-fff-ffff",
			mac_tmp->smac[0],mac_tmp->smac[1],mac_tmp->smac[2],mac_tmp->smac[3],mac_tmp->smac[4],mac_tmp->smac[5]);
	}
	if(mac_tmp->dmac_mask == 1)
	{
		vty_out(vty," dest-mac %02x%02x-%02x%02x-%02x%02x ffff-fff-ffff",
			mac_tmp->dmac[0],mac_tmp->dmac[1],mac_tmp->dmac[2],mac_tmp->dmac[3],mac_tmp->dmac[4],mac_tmp->dmac[5]);
	}
	if(mac_tmp->untag_mask == 1)
	{
		vty_out(vty," %s","untag");
	}
	if(mac_tmp->vlan != ACl_DEF_SVLANID)
	{
		vty_out(vty," vlan %d",mac_tmp->vlan);
	}
	if(mac_tmp->cos != ACL_DEF_COS)
	{
		vty_out(vty," cos %d",mac_tmp->cos);
	}

	vty_out(vty,"%s",VTY_NEWLINE);

}

DEFUN(display_acl_info,
	display_acl_info_cmd,
	"display acl [<1-65534>]",
	"Display\n"
	"Acl rule\n"
	"Acl num\n")
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;

	if(NULL == argv[0])
	{
		//acl_config_write ( vty );
		return CMD_SUCCESS;
	}

	acl_node = acl_group_lookup(atoi(argv[0]));
	if(NULL == acl_node)
	{
		vty_error_out(vty,"acl %d doesn't exist!%s",atoi(argv[0]),VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty_out (vty, "=======================================================================%s", VTY_NEWLINE);
	if(acl_node->acl_num >=2000 && acl_node->acl_num <= 2999)
	{
		vty_out(vty,"acl basic %d %s",acl_node->acl_num,VTY_NEWLINE);
	}
	else if(acl_node->acl_num >=3000 && acl_node->acl_num <= 3999)
	{
		vty_out(vty,"acl advanced %d %s",acl_node->acl_num,VTY_NEWLINE);
	}
	else if(acl_node->acl_num >=4000 && acl_node->acl_num <= 4999)
	{
		vty_out(vty,"acl mac %d %s",acl_node->acl_num,VTY_NEWLINE);
	}
	
	if(acl_node->rulelist.head != NULL)
   	{
	   	for (node = acl_node->rulelist.head ; node; node = node->next)
	   	{
			memcpy(&rule_node, listgetdata (node), sizeof(rule_node));
			
			if(rule_node.type == ACL_TYPE_MAC)
			{
				h3c_acl_rule_mac_show ( vty, &rule_node.rule.mac_acl, rule_node.acl_num);
			}
			if(rule_node.type == ACL_TYPE_IP)
			{
				h3c_acl_rule_ip_show ( vty, &rule_node.rule.ip_acl, rule_node.acl_num);
			}
			if(rule_node.type == ACL_TYPE_IP_EXTEND)
			{
				h3c_acl_rule_ipex_show ( vty, &rule_node.rule.ipex_acl, rule_node.acl_num);
			}
	   	}
    }
	
	vty_out (vty, "=======================================================================%s", VTY_NEWLINE);
	return CMD_SUCCESS;
}

/**
* @brief	  acl_h3c_config_write acl 配置保存
* @param[in ] struct vty *vty :vty
* @param[out] 无
* @return	  ERRNO_SUCCESS
* @author	  Wumm
* @date 	  2018年6月13日 14:00:42
* @note 	  无
*/
#if 0
static int acl_h3c_config_write ( struct vty *vty )
{
	struct acl_group *acl_node = NULL;
	struct listnode *node = NULL;
	struct rule_node rule_node;
	struct hash_bucket *pbucket = NULL;
	int cursor;

	HASH_BUCKET_LOOP ( pbucket, cursor, acl_group_table)
	{
		if(NULL != pbucket->data)
		{
			acl_node = pbucket->data;
		
			if(acl_node->acl_num >=2000 && acl_node->acl_num <= 2999)
			{
				vty_out(vty,"acl basic %d %s",acl_node->acl_num,VTY_NEWLINE);
			}
			else if(acl_node->acl_num >=3000 && acl_node->acl_num <= 3999)
			{
				vty_out(vty,"acl advanced %d %s",acl_node->acl_num,VTY_NEWLINE);
			}
			else if(acl_node->acl_num >=4000 && acl_node->acl_num <= 4999)
			{
				vty_out(vty,"acl mac %d %s",acl_node->acl_num,VTY_NEWLINE);
			}
				
			if ( acl_node->name[0] != '\0' )
			{
				vty_out(vty," description %s%s",acl_node->name,VTY_NEWLINE);
			}
			if(acl_node->rulelist.head != NULL)
			{
				for (node = acl_node->rulelist.head ; node; node = node->next)
				{
					memcpy(&rule_node, listgetdata (node), sizeof(rule_node));
		
					if((rule_node.type == ACL_TYPE_MAC))
					{
						h3c_acl_rule_mac_show ( vty, &rule_node.rule.mac_acl, rule_node.acl_num);
					}
					if(rule_node.type == ACL_TYPE_IP)
					{
						h3c_acl_rule_ip_show ( vty, &rule_node.rule.ip_acl, rule_node.acl_num);
					}
					if(rule_node.type == ACL_TYPE_IP_EXTEND)
					{
						h3c_acl_rule_ipex_show ( vty, &rule_node.rule.ipex_acl, rule_node.acl_num);
					}
				}
			}	
		}  
	}
	return ERRNO_SUCCESS;
}
#endif
void acl_h3c_cli_init(void)
{
	install_node (&acl_basic_node, NULL);
	install_node (&acl_advanced_node, NULL);
	install_node (&acl_mac_node, NULL);
		
	install_default (ACL_BASIC_NODE);	
	install_default (ACL_ADVANCED_NODE);	
	install_default (ACL_MAC_NODE);


	install_element (CONFIG_NODE, &acl_h3c_num_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &undo_acl_h3c_num_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &display_acl_info_cmd, CMD_LOCAL);
	
	install_element (ACL_BASIC_NODE, &acl_ip_h3c_rule_cmd, CMD_SYNC);
	
	install_element (ACL_BASIC_NODE, &acl_ip_h3c_rule_any_cmd, CMD_SYNC);
	install_element (ACL_ADVANCED_NODE, &acl_ip_extend_h3c_rule_cmd, CMD_SYNC);
	install_element (ACL_ADVANCED_NODE, &acl_ip_extend_2_h3c_rule_cmd, CMD_SYNC);
	install_element (ACL_MAC_NODE, &acl_mac_h3c_rule_cmd, CMD_SYNC);

	install_element (ACL_BASIC_NODE, &acl_description_cmd, CMD_SYNC);
	install_element (ACL_ADVANCED_NODE, &acl_description_cmd, CMD_SYNC);
	install_element (ACL_MAC_NODE, &acl_description_cmd, CMD_SYNC);
	
	install_element (ACL_BASIC_NODE, &no_acl_ip_h3c_rule_cmd, CMD_SYNC);
	install_element (ACL_ADVANCED_NODE, &no_acl_ip_h3c_rule_cmd, CMD_SYNC);
	install_element (ACL_MAC_NODE, &no_acl_ip_h3c_rule_cmd, CMD_SYNC);

}
