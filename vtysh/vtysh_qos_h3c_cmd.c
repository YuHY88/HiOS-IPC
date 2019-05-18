#include <zebra.h>
#include <qos/acl_h3c_cli.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"


struct cmd_node acl_basic_node =
{
	ACL_BASIC_NODE,
	"%s(config-basic-acl)# ",
	1,
};

struct cmd_node acl_advanced_node =
{
	ACL_ADVANCED_NODE,
	"%s(config-advanced-acl)# ",
	1,
};

struct cmd_node acl_mac_node =
{
	ACL_MAC_NODE,
	"%s(config-mac-acl)# ",
	1,
};

struct cmd_node qos_classifier_node =
{
	QOS_CLASSIFIER_NODE,
	"%s(config-classifier)# ",
	1,
};

struct cmd_node qos_behavior_node =
{
	QOS_BEHAVIOR_NODE,
	"%s(config-behavior)# ",
	1,
};

struct cmd_node qos_policy1_node =
{
	QOS_POLICY1_NODE,
	"%s(config-policy)# ",
	1,
};

DEFUNSH (VTYSH_QOS,
	 vtysh_exit_basic_acl_num,
	 vtysh_exit_basic_acl_num_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}
	 
ALIAS (vtysh_exit_basic_acl_num,
	vtysh_quit_basic_acl_num_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")
	
DEFUNSH (VTYSH_QOS,
	 vtysh_exit_advanced_acl_num,
	 vtysh_exit_advanced_acl_num_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}
	
ALIAS (vtysh_exit_advanced_acl_num,
       vtysh_quit_advanced_acl_num_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	vtysh_exit_mac_acl_num,
	vtysh_exit_mac_acl_num_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}

ALIAS (vtysh_exit_mac_acl_num,
       vtysh_quit_mac_acl_num_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	vtysh_exit_qos_classifier,
	vtysh_exit_qos_classifier_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}
	
ALIAS (vtysh_exit_qos_classifier,
	   vtysh_quit_qos_classifier_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n")
	   
DEFUNSH (VTYSH_QOS,
	 vtysh_exit_qos_behavior,
	 vtysh_exit_qos_behavior_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_behavior,
	   vtysh_quit_qos_behavior_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n")

DEFUNSH (VTYSH_QOS,
	vtysh_exit_qos_policy1,
	vtysh_exit_qos_policy1_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit (vty);
}

ALIAS (vtysh_exit_qos_policy1,
	   vtysh_quit_qos_policy1_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n")

DEFUNSH(VTYSH_QOS,
	acl_h3c_num,
	acl_h3c_num_cmd,
	"acl {basic <2000-2999>|advanced <3000-3999>|mac <4000-4999>} ",
	"Acl rule\n"
	"Acl group for basic acl configure\n"
	"Acl basic id <2000-2999>\n"
	"Acl group for advanced acl configure\n"
	"Acl advanced id <3000-3999>\n"
	"Acl group for mac acl configure\n")
{
	int type = 0;
	uint32_t acl_num = 0;
	char *pprompt = NULL;
	char *prom_str = NULL;
	
	if(NULL != argv[0])
	{
		acl_num =(uint32_t) atoi(argv[0]);
		type = ACL_BASIC_NODE;
	}
	else if(NULL != argv[1])
	{
		acl_num =(uint32_t) atoi(argv[1]);
		type = ACL_ADVANCED_NODE;
	}
	else if(NULL != argv[2])
	{
		acl_num =(uint32_t) atoi(argv[2]);
		type = ACL_MAC_NODE;
	}
	
	vty->index = (void *)acl_num;
	vty->node = type;
	pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
		ACL_TYPE_PROMPT_GET(type, prom_str);
       // snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-%s-%d)#", prom_str, acl_num);
		
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-%s-acl)#", prom_str, acl_num);
    }
	return CMD_SUCCESS;
}

DEFSH (VTYSH_QOS,
	undo_acl_h3c_num_cmd_vtysh,
	"undo acl {basic <2000-2999>|advanced <3000-3999>|mac <4000-4999>} ",
	"Undo acl rule\n"
	"Acl rule\n"
	"Acl group for basic acl configure\n"
	"Acl basic id <2000-2999>\n"
	"Acl group for advanced acl configure\n"
	"Acl advanced id <3000-3999>\n"
	"Acl group for mac acl configure\n"
	"Acl mac id <4000-4999>\n")

DEFSH (VTYSH_QOS,
	acl_description_cmd_vtysh,
	"description TEXT",
	"ACL description\n"
	"ACL description (no more than 32 characters)\n")

DEFSH (VTYSH_QOS,
	undo_acl_description_cmd_vtysh,
	"undo description",
	NO_STR
	"Acl description\n")

DEFSH (VTYSH_QOS,
	acl_ip_h3c_rule_any_cmd_vtysh,
	"rule <1-65534> (deny|permit) source any",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl ip rule filter deny\n"
	"Acl ip rule filter permit\n"
	"Source ip\n"
	"Ip address:any\n")
	
DEFSH (VTYSH_QOS,
	acl_ip_h3c_rule_cmd_vtysh,
	"rule <1-65534> (deny|permit) source A.B.C.D A.B.C.D",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl ip rule filter deny\n"
	"Acl ip rule filter permit\n"
	"Source ip\n"
	"Source ip address A.B.C.D\n"
	"Source ip address wildcard A.B.C.D\n")

DEFSH (VTYSH_QOS,
	no_acl_ip_h3c_rule_cmd_vtysh,
	"undo rule <1-65534>",
	NO_STR
	"Acl ip rule\n"
	"Acl rule\n")

DEFSH (VTYSH_QOS,
	acl_ip_extend_h3c_rule_cmd_vtysh,
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

DEFSH (VTYSH_QOS,
	acl_ip_extend_3_h3c_rule_cmd_vtysh,
	"rule <1-65534> (deny|permit) (tcp|udp) {sport <1-65535>|dport <1-65535>|destionation A.B.C.D A.B.C.D|source A.B.C.D A.B.C.D|fragment}",
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
	
DEFSH (VTYSH_QOS,
	acl_ip_extend_2_h3c_rule_cmd_vtysh,
	"rule <1-65534> (deny|permit) tcp {sport <1-65535>|dport <1-65535>|destionation A.B.C.D A.B.C.D|source A.B.C.D A.B.C.D|fragment} tcp-flag {fin|syn|rst|psh|ack|urg}",
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
DEFSH (VTYSH_QOS,
	acl_mac_h3c_rule_cmd_vtysh,
	"rule <1-65534> (deny|permit) {dest-mac H-H-H H-H-H|source-mac H-H-H H-H-H|type HHHH HHHH|lsap HHHH HHHH|vlan <1-4095>|cos <0-7>|untag}",
	"Acl rule\n"
	"Acl rule number\n"
	"Acl mac rule filter deny\n"
	"Acl mac rule filter permit\n"
	"Destination mac \n"
	"Destination mac address:H-H-H\n"
	"Destination mac address mask:H-H-H\n"
	"Source mac\n"
	"Source mac address:H-H-H\n"
	"Source mac address mask:H-H-H\n"	
	"Ethernet type\n"
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

DEFSH (VTYSH_QOS,
	display_acl_info_cmd_vtysh,
	"display acl [<1-65534>]",
	"Display\n"
	"Acl rule\n"
	"Acl num\n")

/*  qos policy  */
	
DEFUNSH(VTYSH_QOS,qos_classifier_mode,
	qos_classifier_mode_cmd,
	"traffic classifier NAME",
	 "Qos traffic\n"
	 "Qos traffic classifier\n"
	 "Qos traffic classifier NAME\n")
{
	char *pprompt = NULL;
	uchar name[NAME_STRING_LEN] = {};
	
	if(argv[0] != NULL)
	{
		memcpy(name, argv[0], NAME_STRING_LEN);
	}
	vty->node = QOS_CLASSIFIER_NODE;

	pprompt = vty->change_prompt;
	if ( pprompt )
	{		 
		/* format the prompt */
		//snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-classifier-%s)#", name );
		
		snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-classifier)#");
	}
	
	return CMD_SUCCESS;
}

DEFUNSH(VTYSH_QOS,qos_behavior_mode,
	qos_bebavior_mode_cmd,
	"traffic behavior NAME",
	 "Qos traffic\n"
	 "Qos traffic behavior\n"
	 "Qos traffic behavior NAME\n")
{
	char *pprompt = NULL;
	uchar name[NAME_STRING_LEN] = {};

	if(argv[0] != NULL)
	{
		memcpy(name, argv[0], NAME_STRING_LEN);
	}
	
	vty->node = QOS_BEHAVIOR_NODE;

	pprompt = vty->change_prompt;
	if ( pprompt )
	{		 
		/* format the prompt */
		//snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-behavior-%s)#", name );
		
		snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-behavior)#");
	}
	
	return CMD_SUCCESS;
}

DEFUNSH(VTYSH_QOS,h3c_qos_policy_mode,
	h3c_qos_policy_mode_cmd,
	"qos policy NAME",
	 "Qos \n"
	 "Qos policy\n"
	 "Qos policy NAME\n")
{
	char *pprompt = NULL;
	uchar name[NAME_STRING_LEN] = {};
	
	if(argv[0] != NULL)
	{
		memcpy(name, argv[0], NAME_STRING_LEN);
	}
	
	vty->node = QOS_POLICY1_NODE;

	pprompt = vty->change_prompt;
	if ( pprompt )
	{		 
		/* format the prompt */
		//snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-policy-%s)#", name );
		
		snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-policy)#");
	}
	
	return CMD_SUCCESS;

}
	 
DEFSH (VTYSH_QOS,
	undo_h3c_qos_classifier_mode_cmd_vtysh,
	"undo traffic classifier NAME",
	"undo\n"
	"traffic\n"
	"classifier\n"
	"classifier name\n")
	
DEFSH (VTYSH_QOS,
	undo_h3c_qos_behavior_mode_cmd_vtysh,
	"undo traffic behavior NAME",
	"undo\n"
	"traffic"
	"behavior\n"
	"behavior name\n")
	
DEFSH (VTYSH_QOS,
	undo_h3c_qos_policy_mode_cmd_vtysh,
	"undo qos policy NAME",
	"undo\n"
	"Qos \n"
	"Qos policy\n"
	"Qos policy NAME\n")
	
DEFSH (VTYSH_QOS,
	qos_if_match_mac_cmd_vtysh,
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

DEFSH (VTYSH_QOS,
	qos_if1_match_ipex_cmd_vtysh,
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
	
DEFSH (VTYSH_QOS,
	undo_qos_if_match_mac_cmd_vtysh,
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
DEFSH (VTYSH_QOS,
	h3c_qos_policy_acl_car_cmd_vtysh,
	"car cir committed-information-rate cbs <0-10000000>",
	"Qos car\n"
	"Car cir\n"
	"Committed-information-rate\n"
	"Egress\n"
	"Cbs\n"
	"Committed information rate value <0-10000000> (Unit: Kbps)\n")
	
DEFSH (VTYSH_QOS,
	undo_h3c_qos_policy_acl_car_cmd_vtysh,
	"undo car cir committed-information-rate cbs <0-10000000>",
	"undo\n"
	"Qos car\n"
	"Car cir\n"
	"Committed-information-rate\n"
	"Egress\n"
	"Cbs\n"
	"Committed information rate value <0-10000000> (Unit: Kbps)\n")
DEFSH (VTYSH_QOS,
	qos_policy_h3c_acl_filter_cmd_vtysh,
	"filter (permit|deny)",
	"Acl action: filter\n"
	"Permit acl rule filter\n"
	"Deny acl rule filter\n")
	
DEFSH (VTYSH_QOS,
	undo_qos_policy_h3c_acl_filter_cmd_vtysh,
	"undo filter",
	"undo\n"
	"Acl action: filter\n")
	
DEFSH (VTYSH_QOS,
	qos_policy_h3c_classifier_behavior_cmd_vtysh,
	"classifier CLASSIFIER-NAME behavior BEHAVIOR-NAME",  
	"classifier\n"
	"classifier CLASSIFIER-NAME\n"
	"behavior\n"
	"behavior BEHAVIOR-NAME\n")
	
DEFSH (VTYSH_QOS,
	undo_qos_policy_h3c_classifier_behavior_cmd_vtysh,
	"undo classifier CLASSIFIER-NAME behavior BEHAVIOR-NAME",	
	"undo\n"
	"classifier\n"
	"classifier CLASSIFIER-NAME\n"
	"behavior\n"
	"behavior BEHAVIOR-NAME\n")
	
DEFSH (VTYSH_QOS,
	qos_apply_policy_interface_cmd_vtysh,
	"qos apply policy POLICY-NAME inbound",  
	"Qos\n"
	"Qos apply\n"
	"Qos policy\n"
	"Qos policy POLICY-NAME\n"
	"Qos policy inbound\n")
	
DEFSH (VTYSH_QOS,
	undo_qos_apply_policy_interface_cmd_vtysh,
	"undo qos apply policy POLICY-NAME inbound",  
	"undo\n"
	"Qos\n"
	"Qos apply\n"
	"Qos policy\n"
	"Qos policy POLICY-NAME\n"
	"Qos policy inbound\n")
	
DEFSH (VTYSH_QOS,
	qos_apply_policy_global_cmd_vtysh,
	"qos apply policy POLICY-NAME global inbound",
	"Qos\n"
	"Qos apply\n"
	"Qos policy\n"
	"Qos policy POLICY-NAME\n"
	"Qos policy global\n"
	"Qos policy inbound\n")
	
DEFSH (VTYSH_QOS,
	undo_qos_apply_policy_global_cmd_vtysh,
	"undo qos apply policy POLICY-NAME global inbound", 
	"undo\n"
	"Qos\n"
	"Qos apply\n"
	"Qos policy\n"
	"Qos policy POLICY-NAME\n"
	"Qos policy global\n"
	"Qos policy inbound\n")
	
DEFSH (VTYSH_QOS,
	display_qos_policy_acl_cmd_vtysh,
	"display acl [<1-65535>]",
	"display acl\n"
	"Acl rule\n"
	"Acl number <1-65535>\n")	
	
DEFSH (VTYSH_QOS,
	display_qos_mode_cmd_vtysh,
	"display traffic {classifier|bebavior|qos policy} user-defined [NAME] ",
	"display qos policy\n"
	"Qos traffic\n"
	"Qos traffic classifier\n"
	"Qos traffic bebavior\n"
	"Qos traffic qos policy\n"
	"Qos traffic qos policy\n"
	"user defined Qos traffic\n"
	"Qos traffic NAME\n")
DEFSH (VTYSH_QOS,
	display_qos_policy_mode_cmd_vtysh,
	"display qos policy user-defined [NAME] ",
	"display qos policy\n"
	"Qos traffic qos policy\n"
	"Qos traffic qos policy\n"
	"user defined Qos traffic\n"
	"Qos traffic NAME\n")
	
void vtysh_init_qos_h3c_cmd ()
{
	/*  h3c acl  */
	install_node (&acl_basic_node, NULL);
	install_node (&acl_advanced_node, NULL);
	install_node (&acl_mac_node, NULL);

	vtysh_install_default (ACL_BASIC_NODE);	
	vtysh_install_default (ACL_ADVANCED_NODE);	
	vtysh_install_default (ACL_MAC_NODE);
	
	install_element_level (CONFIG_NODE, &acl_h3c_num_cmd, VISIT_LEVE, CMD_SYNC);
	
	install_element_level (CONFIG_NODE, &undo_acl_h3c_num_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &display_acl_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	
	install_element_level (ACL_BASIC_NODE, &vtysh_exit_basic_acl_num_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &vtysh_exit_advanced_acl_num_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (ACL_MAC_NODE, &vtysh_exit_mac_acl_num_cmd, VISIT_LEVE, CMD_SYNC);
	
	install_element_level (ACL_BASIC_NODE, &acl_ip_h3c_rule_any_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (ACL_BASIC_NODE, &acl_ip_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &acl_ip_extend_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &acl_ip_extend_2_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &acl_ip_extend_3_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_MAC_NODE, &acl_mac_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (ACL_BASIC_NODE, &acl_description_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &acl_description_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_MAC_NODE, &acl_description_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (ACL_BASIC_NODE, &no_acl_ip_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_ADVANCED_NODE, &no_acl_ip_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (ACL_MAC_NODE, &no_acl_ip_h3c_rule_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	/*  qos policy  */
	
	install_node (&qos_classifier_node, NULL);
	install_node (&qos_behavior_node, NULL);
	install_node (&qos_policy1_node, NULL);
	vtysh_install_default (QOS_BEHAVIOR_NODE);
	vtysh_install_default (QOS_CLASSIFIER_NODE);
	vtysh_install_default (QOS_POLICY1_NODE);
	
	install_element_level (CONFIG_NODE, &qos_classifier_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_bebavior_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &h3c_qos_policy_mode_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (QOS_CLASSIFIER_NODE, &vtysh_exit_qos_classifier_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (QOS_BEHAVIOR_NODE, &vtysh_exit_qos_behavior_cmd, VISIT_LEVE, CMD_SYNC);
	install_element_level (QOS_POLICY1_NODE, &vtysh_exit_qos_policy1_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level (QOS_CLASSIFIER_NODE, &qos_if_match_mac_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (QOS_CLASSIFIER_NODE, &qos_if1_match_ipex_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (QOS_BEHAVIOR_NODE, &h3c_qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (QOS_BEHAVIOR_NODE, &qos_policy_h3c_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (QOS_POLICY1_NODE, &qos_policy_h3c_classifier_behavior_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &qos_apply_policy_interface_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &qos_apply_policy_interface_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &qos_apply_policy_global_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &display_qos_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &display_qos_policy_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &undo_h3c_qos_classifier_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_h3c_qos_behavior_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_h3c_qos_policy_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (QOS_CLASSIFIER_NODE, &undo_qos_if_match_mac_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level (QOS_BEHAVIOR_NODE, &undo_h3c_qos_policy_acl_car_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (QOS_BEHAVIOR_NODE, &undo_qos_policy_h3c_acl_filter_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (QOS_POLICY1_NODE, &undo_qos_policy_h3c_classifier_behavior_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (PHYSICAL_IF_NODE, &undo_qos_apply_policy_interface_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_SUBIF_NODE, &undo_qos_apply_policy_interface_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &undo_qos_apply_policy_global_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
}
