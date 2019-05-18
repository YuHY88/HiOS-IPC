/**
 * \page cmds_ref_l2 L2
 * - \subpage modify_log_l2
 * - \subpage elps_session_cmd
 * - \subpage elps_protect_port_cmd_vtysh
 * - \subpage elps_data_vlan_cmd_vtysh
 * - \subpage elps_primary_vlan_cmd_vtysh
 * - \subpage elps_keep_alive_cmd_vtysh
 * - \subpage elps_hold_off_cmd_vtysh
 * - \subpage elps_backup_failback_cmd_vtysh
 * - \subpage elps_priority_cmd_vtysh
 * - \subpage elps_bind_cfm_session_cmd_vtysh
 * - \subpage elps_session_enable_cmd_vtysh
 * - \subpage elps_debug_packet_cmd_vtysh
 * - \subpage elps_admin_force_cmd_vtysh
 * - \subpage elps_admin_manual_cmd_vtysh
 * - \subpage elps_admin_lockout_cmd_vtysh
 * - \subpage show_elps_config_cmd_vtysh
 * - \subpage show_elps_config_cmd_vtysh
 * - \subpage show_elps_debug_cmd_vtysh
 * - \subpage erps_session_cmd
 * - \subpage erps_physical_ring_cmd_vtysh
 * - \subpage erps_rpl_role_cmd_vtysh
 * - \subpage erps_data_traffic_cmd_vtysh
 * - \subpage erps_raps_channel_cmd_vtysh
 * - \subpage erps_backup_failback_cmd_vtysh
 * - \subpage erps_hold_off_cmd_vtysh
 * - \subpage erps_keep_alive_cmd_vtysh
 * - \subpage erps_guard_timer_cmd_vtysh
 * - \subpage erps_ring_id_cmd_vtysh
 * - \subpage erps_level_cmd_vtysh
 * - \subpage erps_bind_cfm_session_cmd_vtysh
 * - \subpage erps_virtual_channel_cmd_vtysh
 * - \subpage erps_sub_ring_block_cmd_vtysh
 * - \subpage erps_session_enable_cmd_vtysh
 * - \subpage erps_admin_force_cmd_vtysh
 * - \subpage erps_admin_clear_cmd_vtysh
 * - \subpage erps_debug_packet_cmd_vtysh
 * - \subpage show_erps_config_cmd_vtysh
 * - \subpage show_erps_session_cmd_vtysh 
 * 
 */
 
/**
 * \page modify_log_l2 Modify Log
 * \section l2-v007r004 HiOS-V007R004
 *  -# 
 * \section l2-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include <lib/ifm_common.h>
#include "lib/command.h"
#include "vtysh.h"
#include <lib/devm_com.h>


static unsigned int vtysh_devtype=0;
static struct cmd_node vlan_node =
{
  VLAN_NODE,
  "%s(config-vlan)# "
};

static struct cmd_node mac_node =
{ 
MAC_NODE,  
"%s(config-mac)# ",  
1, 
};

static struct cmd_node cfm_md_node =
{
  CFM_MD_NODE,
  "%s(config-md)# ",
  1,
};

static struct cmd_node cfm_ma_node =
{
  CFM_MA_NODE,
  "%s(config-ma)# ",
  1,
};

static struct cmd_node cfm_session_node =
{
  CFM_SESSION_NODE,
  "%s(config-session)# ",
  1,
};

static struct cmd_node elps_session_node =
{
	ELPS_SESSION_NODE,
	"%s(config-elps-session)# ",
	1
};
    
static struct cmd_node erps_session_node =
{
    ERPS_SESSION_NODE,
    "%s(config-erps-session)# ",
    1
};
#if 0
/*create instance, enter stp view cmd*/
DEFUNSH (VTYSH_L2,
		stp_instance,
		stp_instance_cmd_vtysh,
		"stp instance <1-8>",
		"Multiple Spanning Tree Protocol (MSTP) mode\n"
		"Spanning tree instance\n"
		"Identifier of spanning tree instance, Please input an integer from 1 to 8\n")
{
	char 				*pprompt = NULL;
	unsigned int		instance_id;
	
	instance_id = atoi(argv[0]);

	/*inter instance view*/
	vty->node  = MSTP_NODE;
	pprompt = cmd_prompt ( vty->node );

	if(pprompt)
	{
		/* format the prompt */
		snprintf(pprompt, VTY_BUFSIZ, "%%s(config-mstp-instance %d)#", instance_id);
	}	

	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
        vtysh_exit_stp_instance,
        vtysh_exit_stp_instance_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

ALIAS (vtysh_exit_stp_instance,
       vtysh_quit_stp_instance_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")
#endif
static struct cmd_node trunk_gloable_node =
{
    TRUNK_GLOABLE_NODE,
    "%s(config-trunk-gloable)# ",
    1
};

DEFUNSH (VTYSH_L2,
		vlan_get,
		vlan_get_cmd,
		"vlan <1-4094>",
		"vlan node\n"
		"vlanid start <1-4094>\n"
		)
{
	char *pprompt = NULL;
	uint16_t vlanid = 0;

	vlanid = atoi(argv[0]);

	vty->node = VLAN_NODE;
	pprompt = vty->change_prompt;
    if ( pprompt )
    {
        /* format the prompt */
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-vlan %d)#",vlanid);
    }
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
	 vtysh_exit_vlan,
	 vtysh_exit_vlan_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_vlan,
       vtysh_quit_vlan_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")



DEFSH(VTYSH_L2,vlan_range_set_cmd_vtysh,
	"vlan-range <1-8> VLAN_STRING",
	"Vlan node\n"
	"Vlan_STRING segment,1:1-512;2:513-1024...etc\n"
	"VLAN_STRING,use string to set vlan(eg. F:vlan 1 to 4)\n")

DEFSH(VTYSH_L2,no_vlan_range_set_cmd_vtysh,
	"no vlan-range <1-8> VLAN_STRING",
	"No command\n"
	"Vlan node\n"
	"Vlan_STRING segment,1:1-512;2:513-1024...etc\n"
	"VLAN_STRING,use string to set vlan(eg. F:vlan 1 to 4)\n")

/*vlan 子模??*/
DEFSH(VTYSH_L2,no_vlan_get_range_cmd_vtysh,
	"no vlan <1-4094> {to <1-4094>}",
	"No command\n"
	"Vlan node\n"
	"Vlanid start <1-4094>\n"
	"To\n"
	"Vlanid end <1-4094>\n")

DEFSH(VTYSH_L2,vlan_get_range_cmd_vtysh,
	"vlan <1-4094> to <1-4094>",
	"Vlan node\n"
	"Vlanid start <1-4094>\n"
	"To\n"
	"Vlanid end <1-4094>\n")

DEFSH(VTYSH_L2,vlan_name_cmd_vtysh,
	"name NAME",
	"Set vlan name\n"
	"Vlan name and length of name is <1-31>\n")

DEFSH(VTYSH_L2,no_vlan_name_cmd_vtysh,
	"no name",
	"Default vlan name\n"
	"Name\n")

DEFSH(VTYSH_L2,vlan_mac_learn_cmd_vtysh,
	"mac learn (enable|disable)",
	"Mac\n"
	"Learn\n"
	"Enable\n"
	"Disable\n")
		
DEFSH(VTYSH_L2,vlan_mac_limit_cmd_vtysh,
	"mac limit <1-16384> action (discard | forward)",
	"Mac\n"
	"Limit the count of mac learn\n"
	"Limit value <1-16384> and the default value is 0.\n"
	"Action when limit value is exceeded\n"
	"Discard the packet\n"
	"Forward the packet\n")	
	
DEFSH(VTYSH_L2,no_vlan_mac_limit_cmd_vtysh,
	"no mac limit",
	"Clear config\n"
	"Mac\n"
	"Limit the count of mac learn\n")	

DEFSH(VTYSH_L2,no_vlan_storm_supress_cmd_vtysh, 
	"no storm suppress {unicast | broadcast | multicast}",
	"Clear config\n"
	"Broadcast storm\n"
	"Storm-control\n"
	"Unicast\n"
	"Broadcast\n"
	"Multicast\n")

DEFSH(VTYSH_L2,vlan_storm_supress_cmd_vtysh, 
	"storm suppress {unicast | broadcast | multicast}",
	"Broadcast storm\n"
	"Storm-control\n"
	"Unicast\n"
	"Broadcast\n"
	"Multicast\n")
		
DEFSH(VTYSH_L2,show_vlan_cmd_vtysh,
	"show vlan  [<1-4094>]",
	"Show command\n"
	"Vlan node\n"
	"Vlanid <1-4094>\n")

		
DEFSH(VTYSH_L2,show_vlan_summary_cmd_vtysh,
	"show vlan summary",
	"Show command\n"
	"Vlan node\n"
	"Vlan information summary\n")


/*l2 子模??*/
DEFSH(VTYSH_L2,show_switch_interface_cmd_vtysh,
	"show switch interface",
	"Show command \n"
	"Mode switch \n"
	"Interface info\n")

DEFSH(VTYSH_L2,l2_mode_switch_cmd_vtysh,
	"switch (hybrid | access | trunk )",
	"Choice mode\n"
	"Switch hybrid\n" 
	"Switch access\n" 
	"Switch trunk\n" )

DEFSH(VTYSH_L2,l2if_mac_limit_cmd_vtysh,
	"mac limit <1-16384> action (discard | forward)",
	"Mac\n"
	"Limit the count of mac learn\n"
	"Limit value <1-16384> and the default value is 0.\n"
	"Action when limit value is exceeded\n"
	"Discard the packet\n"
	"Forward the packet\n")
	
DEFSH(VTYSH_L2,no_l2if_mac_limit_cmd_vtysh,
	"no mac limit",
	"Clear config\n"
	"Mac\n"
	"Limit the count of mac learn\n")	 
	
DEFSH(VTYSH_L2,no_l2if_storm_supress_cmd_vtysh,
	"no storm-suppress {unicast | broadcast | multicast}",
	"Clear config\n"
	"Broadcast storm\n"
	"Storm-control\n"
	"Unicast\n"
	"Broadcast\n"
	"Multicast\n")	
	
DEFSH(VTYSH_L2,l2if_storm_supress_cmd_vtysh,
	"storm-suppress {unicast | broadcast | multicast}  rate <64-50000>",
	"Broadcast storm\n"
	"Storm-control\n"
	"Unicast\n"
	"Broadcast\n"
	"Multicast\n"
	"Rate \n" 
	"Rate values in kilobits (1000 bits) per second\n")	
		
DEFSH(VTYSH_L2,no_switch_access_vlan_cmd_vtysh,
	"no switch access-vlan",
	"No command\n"
	"Mode switch \n"
	"Access vlanid\n")

DEFSH(VTYSH_L2,switch_access_vlan_cmd_vtysh,
	"switch access-vlan <1-4094>",
	"Mode switch\n"
	"Access-vlan\n"
	"Vlanid<1-4094>\n")

DEFSH(VTYSH_L2,no_switch_vlan_add_cmd_vtysh,
	"no switch vlan  <1-4094> {to <1-4094>}",
	"No command\n"
	"Mode switch\n"
	"Switch vlan range\n"
	"Vanid start <1-4094>\n"
	"To\n"
	"Vlanid end <1-4094>\n")

DEFSH(VTYSH_L2,switch_vlan_add_cmd_vtysh,
	"switch vlan  <1-4094> {to <1-4094>}",
	"Mode switch\n"
	"Switch vlan\n"
	"Vlanid start <1-4094>\n"
	"To\n"
	"Vlanid end <1-4094>\n")
	

DEFSH(VTYSH_L2,switch_vlan_tag_add_cmd_vtysh,
	"switch vlan  <1-4094> {to <1-4094>} egress-tagged (enable | disable)",
	"Mode switch\n"
	"Switch vlan\n"
	"Vlanid start <1-4094>\n"
	"To\n"
	"Vlanid end <1-4094>\n"
	"Whether the egress message carries a vlan tag\n"
	"Enable\n"
	"Disable\n")



/*l2cp*/
	 
DEFSH (VTYSH_L2,
	  l2cp_mpls_func_cmd,
	  "l2cp (eoam|lacp|lldp|stp|dot1x) mpls (vsi|vc-id) <1-4294967295>",
	  "Configure Layer2 Protocol Tunnelling \n"
	  "Ethernet Operation, Administration and Maintenance \n"
	  "Link Aggregation (LACP) \n"
	  "Link Layer Discover Protocol \n"
	  "Spanning Tree Protocols \n"
	  "Port Authentication (802.1 X) \n"
	  "L2cp tunnel over MPLS \n"
	  "L2cp tunnel over VPLS \n"
	  "L2cp tunnel over VPWS \n"
	  "vc-id: 1-4294967295 vsi:1-1024"
	  )

DEFSH (VTYSH_L2,
     l2cp_tunnel_func_cmd_vtysh,
     "l2cp (eoam|lacp|lldp|stp|dot1x) tunnel {vlan <1-4094> cos <0-7>}",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
     /*"GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "Tunnel the protocol data unit \n"
     "Vlan \n"
     "Specify Vlan id, 1-4094 \n"
     "Cos \n"
     "Specify Cos id, 0-7"
     )
DEFSH (VTYSH_L2,
     l2cp_tunnel_func2_cmd_vtysh,
     "l2cp (eoam|lacp|lldp|stp|dot1x) tunnel dmac XX:XX:XX:XX:XX:XX {vlan <1-4094> cos <0-7>}",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
    /* "GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "Tunnel the protocol data unit \n"
     "Change l2cp Dest MAC to Group Dest Mac Address \n"
     "Specify Group Dest Mac Address:01:00:0c:cd:cd:d0,01:00:0c:cd:cd:d1,01:00:0c:cd:cd:d2 \n"
     "Vlan \n"
     "Specify Vlan id, 1-4094 \n"
     "Cos \n"
     "Specify Cos id, 0-7"
     )
     
DEFSH (VTYSH_L2,
     l2cp_discard_func_cmd_vtysh,
     "l2cp (eoam|lacp|lldp|stp|dot1x) discard",
     "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
     /*"GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "Discard the protocol data unit"
     )

DEFSH (VTYSH_L2,
     no_l2cp_tunnel_cmd_vtysh,
     "no l2cp (eoam|lacp|lldp|stp|dot1x|all)", 
	 "Disable Layer2 Protocol Function \n"
	 "Configure Layer2 Protocol Tunnelling \n"
     "Ethernet Operation, Administration and Maintenance \n"
     /*"GMRP Multicast Registration Protocol \n"
     "GVRP VLAN Registration Protocol \n"*/
     "Link Aggregation (LACP) \n"
	 "Link Layer Discover Protocol \n"
	 "Spanning Tree Protocols \n"
     "Port Authentication (802.1 X) \n"
     "delete all l2cp protocol \n"
     "Discard the protocol data unit \n"
     "Tunnel the protocol data unit ")

DEFSH(VTYSH_L2,
	l2cp_config_show_cmd_vtysh,
	"show l2cp",
	"Show running system information \n"
	"show l2cp config")
/*qinq 子模??*/
DEFSH(VTYSH_L2,no_dot1q_tunnel_svlan_cmd_vtysh,
	"no dot1q-tunnel",
	"No command\n"
	"Dot1q-tunnel rule\n" )

DEFSH(VTYSH_L2,dot1q_tunnel_svlan_cmd_vtysh,
	"dot1q-tunnel svlan <1-4094> cos <0-7>",
	"Dot1q-tunnel rule\n"
	"Svlan_new\n" 
	"Svlan_newid<1-4094>\n"
	"Set cos\n"
	"Cos <0-7>\n")

DEFSH(VTYSH_L2,no_vlan_mapping_qinq_cmd_vtysh,
	"no vlan-mapping qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>}",
	"No command \n"
	"Vlan-mapping\n"
	"Qinq rule\n"
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Cvlanid end <1-4094>\n")

DEFSH(VTYSH_L2,vlan_mapping_qinq_cvlans_delete_svlan_cmd_vtysh,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> {to <1-4094>} delete svlan",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid <1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Cvlanid end <1-4094>\n"
	"Delete command\n"
	"Svlan\n")

DEFSH(VTYSH_L2,vlan_mapping_qinq_scvlans_to_svlan_cmd_vtysh,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> to <1-4094> translate svlan  <1-4094> {cos <0-7>} cvlan * ",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid start <1-4094>\n"
	"To\n"
	"Cvlanid end <1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Clvan_new\n"
	"Cvlan_newid not change\n")

DEFSH(VTYSH_L2,vlan_mapping_qinq_scvlan_to_scvlan_cmd_1_vtysh,
	"vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> cos <0-7> cvlan <1-4094> ",
	"Vlan-mapping rule\n"
	"Qinq rule\n" 
	"Svlan\n"
	"Svlanid<1-4094>\n"
	"Cvlan\n"
	"Cvlanid<1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Cvlan_new\n"
	"Cvlan_newid<1-4094>\n")
 
 DEFSH(VTYSH_L2,vlan_mapping_qinq_scvlan_to_scvlan_cmd_2_vtysh,
	 "vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> {cos <0-7>} ",
	 "Vlan-mapping rule\n"
	 "Qinq rule\n" 
	 "Svlan\n"
	 "Svlanid<1-4094>\n"
	 "Cvlan\n"
	 "Cvlanid<1-4094>\n"
	 "Translate command\n"
	 "Svlan_new\n"
	 "Svlan_newid<1-4094>\n"
	 "Cos of svlan_new\n"
	 "Cos<0-7>\n")
 
 DEFSH(VTYSH_L2,vlan_mapping_qinq_scvlan_to_scvlan_cmd_3_vtysh,
	 "vlan-mapping qinq svlan <1-4094> cvlan <1-4094> translate svlan <1-4094> cvlan <1-4094>",
	 "Vlan-mapping rule\n"
	 "Qinq rule\n" 
	 "Svlan\n"
	 "Svlanid<1-4094>\n"
	 "Cvlan\n"
	 "Cvlanid<1-4094>\n"
	 "Translate command\n"
	 "Svlan_new\n"
	 "Svlan_newid<1-4094>\n"
	 "Cvlan_new\n"
	 "Cvlan_newid<1-4094>\n")

DEFSH(VTYSH_L2,no_vlan_mapping_svlan_cmd_vtysh,
	"no vlan-mapping dot1q <1-4094> {to <1-4094>} ",
	"No command\n"
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n")

DEFSH(VTYSH_L2,vlan_mapping_svlan_to_scvlan_cmd_vtysh,
	"vlan-mapping dot1q <1-4094> translate svlan <1-4094> {cos <0-7>} cvlan <1-4094> ",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"Translate command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n"
	"Cvlan_new\n"
	"Cvlan_newid <1-4094>\n")

DEFSH(VTYSH_L2,vlan_mapping_add_svlan_cmd_vtysh,
	"vlan-mapping dot1q <1-4094> {to <1-4094>} add svlan <1-4094> { cos <0-7> }",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n"
	"Add command\n"
	"Svlan_new\n"
	"Svlan_newid<1-4094>\n"
	"Cos of svlan_new\n"
	"Cos<0-7>\n")

DEFSH(VTYSH_L2,vlan_mapping_svlans_to_svlan_cmd_vtysh,
	"vlan-mapping dot1q <1-4094> {to <1-4094>} translate <1-4094>",
	"Vlan-mapping rule\n"
	"Dot1q rule\n"
	"Svlanid start <1-4094>\n"
	"To\n"
	"Svlanid end <1-4094>\n"
	"Translate command\n"
	"Svlan_newid <1-4094>\n")
	
/*loopd*/

DEFSH(VTYSH_L2,loopdetect_cmd_vtysh,
	"loop-detect {vlan <1-4094>|interval <1-60>|restore-time <3-60000>| action (block|shutdown)}",
	"Loop detect\n"
	"Vlan node\n"
	"Vlanid <1-4094>\n"
	"Interval time\n"
	"Time in seconds\n"
	"Restore-time\n"
	"Time in seconds\n"
	"Loop action\n"
	"Action block\n"
	"Action shutdown\n"
	)

DEFSH(VTYSH_L2,no_loopdetect_cmd_vtysh,
	"no loop-detect",
	"No command\n"
	"Loop-detect\n"
	)

	
DEFSH(VTYSH_L2,show_loopdetect_cmd_vtysh,
	"show loop-detect interface {ethernet USP}",
	"Show command\n"
	"Loop detect\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	)

DEFSH(VTYSH_L2,show_loopdetect_gigabit_ethernet_cmd_vtysh,
	"show loop-detect interface gigabitethernet USP",
	"Show command\n"
	"Loop detect\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
	)
	
DEFSH(VTYSH_L2,show_loopdetect_xgigabit_ethernet_cmd_vtysh,
	"show loop-detect interface xgigabitethernet USP",
	"Show command\n"
	"Loop detect\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
	)

DEFSH(VTYSH_L2,show_loopdetect_interface_cmd_vtysh,
	"show loop-detect",
	"Show command\n"
	"Loop detect\n"
	)

DEFSH(VTYSH_L2,loopdetect_pkt_debug_cmd_vtysh,
	"debug loop-detect packet",
	"Debug\n"
	"Loop detect\n"
	"Dump send and receive packet"
	)

DEFSH(VTYSH_L2,loopdetect_pkt_debug_disable_cmd_vtysh,
	"no debug loop-detect packet",
	"No command\n"
	"Debug\n"
	"Loop detect\n"
	"Not dump send and receive packet"
	)



/*******mstp module cmd************************************************/
#if 0
DEFSH (VTYSH_L2,
        stp_region_name_cmd_vtysh,
        "stp region-name NAME",
        MSTP_STR
        "Specify region name\n"
        "Region name, A maximum of 32 characters can be entered\n")

/*region name*/
DEFSH (VTYSH_L2,
        no_stp_region_name_cmd_vtysh,
        "no stp region-name",
        NO_STR
        MSTP_STR
        "Specify region name\n")

/*revision level*/
DEFSH (VTYSH_L2,
        stp_revision_level_cmd_vtysh,
        "stp revision-level <0-65535>",
        MSTP_STR
        "Specify revision level\n"
        "revision level, Please input an integer from 0 to 65535, default is 0\n")

DEFSH (VTYSH_L2,
        no_stp_revision_level_cmd_vtysh,
        "no stp revision-level",
        NO_STR
        MSTP_STR
        "Specify revision level\n")

#if 0
/*create instance, enter stp view cmd*/
DEFSH (VTYSH_L2,
        stp_instance_cmd_vtysh,
        "stp instance <1-8>",
        MSTP_STR
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")
#endif

/*delete stp instance cmd*/
DEFSH (VTYSH_L2,
        no_stp_instance_cmd_vtysh,
        "no stp instance <1-8>",
		NO_STR
		MSTP_STR
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")

/*stp vlan cmd*/
DEFSH(VTYSH_L2,
		stp_vlan_cmd_vtysh,
        "stp vlan <1-4094> {to <1-4094>}",
        MSTP_STR
        "vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n"
        "range of vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n")

/*no stp vlan cmd*/
DEFSH(VTYSH_L2,
		no_stp_vlan_cmd_vtysh,
        "no stp vlan <1-4094> {to <1-4094>}",
        NO_STR
        MSTP_STR
        "vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n"
        "range of vlan\n"
        "vlan range, Please input an integer from 1 to 4094\n")

/*stp priority cmd*/
DEFSH(VTYSH_L2,
		stp_priority_cmd_vtysh,
        "stp priority <0-61440>",
        MSTP_STR
        "Specify bridge priority\n"
        "Bridge priority, in steps of 4096, the default value is 32768. Please input an integer from 0 to 61440\n")
        
/*no stp priority cmd*/
DEFSH(VTYSH_L2,
		no_stp_priority_cmd_vtysh,
        "no stp priority",
        NO_STR
        MSTP_STR
        "Specify bridge priority\n")
        
/*stp root cmd*/
DEFSH(VTYSH_L2,
		stp_root_cmd_vtysh,
        "stp root (master | slave)",
        MSTP_STR
        "Specify root switch\n"
        "Primary root bridge\n"
        "Secondary root bridge\n")
        
/*no stp root cmd*/
DEFSH(VTYSH_L2,
		no_stp_root_cmd_vtysh,
        "no stp root",
        NO_STR
        MSTP_STR
        "Specify root switch\n")

/*stp hello interval cmd*/
DEFSH(VTYSH_L2,
		stp_hello_time_cmd_vtysh,
        "stp hello-time <1-60>",
        MSTP_STR
        "Specify hello time interval\n"
        "The default value is 2. Please input an integer from 1 to 60\n")

/*no stp hello interval cmd*/
DEFSH(VTYSH_L2,
		no_stp_hello_time_cmd_vtysh,
        "no stp hello-time",
        NO_STR
        MSTP_STR
        "Specify hello time interval\n")

/*stp forward delay cmd*/
DEFSH(VTYSH_L2,
		stp_forward_delay_cmd_vtysh,
        "stp forward-delay <4-30>",
        MSTP_STR
        "Specify forward delay\n"
        "The default value is 15s. Please input an integer from 4 to 30\n")

/*no stp forward delay cmd*/
DEFSH(VTYSH_L2,
		no_stp_forward_delay_cmd_vtysh,
        "no stp forward-delay",
        NO_STR
        MSTP_STR
        "Specify forward delay\n")
        
/*stp max-age cmd*/
DEFSH(VTYSH_L2,
		stp_max_age_cmd_vtysh,
        "stp max-age <6-40>",
        MSTP_STR
        "Specify max age\n"
        "The default value is 20s. Please input an integer from 6 to 40\n")
        
/*no stp max-age cmd*/
DEFSH(VTYSH_L2,
		no_stp_max_age_cmd_vtysh,
        "no stp max-age",
        NO_STR
        MSTP_STR
        "Specify max age\n")

/*stp max-hop cmd*/
DEFSH(VTYSH_L2,
		stp_max_hop_cmd_vtysh,
        "stp max-hop <1-40>",
        MSTP_STR
        "Specify max hops\n"
        "The default value is 20. Please input an integer from 1 to 40\n")
        
/*no stp max-hop cmd*/
DEFSH(VTYSH_L2,
		no_stp_max_hop_cmd_vtysh,
        "no stp max-hop",
        NO_STR
        MSTP_STR
        "Specify max hops\n")
        
/*stp time-out cmd*/
DEFSH(VTYSH_L2,
		stp_time_out_cmd_vtysh,
        "stp time-out <1-600>",
        MSTP_STR
        "Specify aged out time factor\n"
        "The default value is 6. Please input an integer from 1 to 600\n")
        
/*no stp time-out cmd*/
DEFSH(VTYSH_L2,
		no_stp_time_out_cmd_vtysh,
        "no stp time-out",
	    NO_STR
        MSTP_STR
        "Specify aged out time factor\n")
        
/*stp enable instance cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_enable_instance_cmd_vtysh,
		"stp enable instance <1-8>",
		MSTP_STR
		"Enable\n"
		"Spanning tree instance\n"
		"Please input an integer from 1 to 8\n")
		
/*no stp enable instance cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_enable_instance_cmd_vtysh,
		"no stp enable instance <1-8>",
	    NO_STR
		MSTP_STR
		"Enable\n"
		"Spanning tree instance\n")

/*no stp enable instance cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_priority_cmd_vtysh,
		"stp priority <0-240>",
		MSTP_STR
		"Specify port priority\n"
		"Port priority, in steps of 16\n")

/*no stp enable instance cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_priority_cmd_vtysh,
		"no stp priority",
	    NO_STR
		MSTP_STR
		"Specify port priority\n"
		"Port priority, in steps of 16\n")

/*stp port cost cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_cost_cmd_vtysh,
		"stp cost <1-200000000>",
		MSTP_STR
		"Specify port path cost\n"
		"Port path cost, Please input an integer from 1 to 200,000,000\n")

/*no stp port cost cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_cost_cmd_vtysh,
		"no stp cost",		
	    NO_STR
		MSTP_STR
		"Specify port path cost\n")

/*stp edge-port cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_edge_cmd_vtysh,
		"stp edge-port",
		MSTP_STR
		"Specify edge port\n")

/*no stp edge-port cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_edge_cmd_vtysh,
		"no stp edge-port",		
	    NO_STR
		MSTP_STR
		"Specify edge port\n")

/*stp filter-port cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_filter_port_cmd_vtysh,
		"stp filter-port",
		MSTP_STR
		"Enable port's BPDU filter function\n")

/*no stp filter-port cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_filter_port_cmd_vtysh,
		"no stp filter-port",
	    NO_STR
		MSTP_STR
		"Enable port's BPDU filter function\n")

/*stp p2p cmd*/
DEFSH(VTYSH_L2, 
		interface_stp_p2p_cmd_vtysh,
		"stp p2p",
		MSTP_STR
		"Specify point to point link\n")

/*no stp p2p cmd*/
DEFSH(VTYSH_L2, 
		no_interface_stp_p2p_cmd_vtysh,
		"no stp p2p",
	    NO_STR
		MSTP_STR
		"Specify point to point link\n")

/*show stp interface [USP]*/
DEFSH(VTYSH_L2,
		show_stp_interface_trunk_cmd_vtysh,
		"show stp interface { trunk TRUNK}",
        SHOW_STR
        MSTP_STR
        "Specify interface\n"
        "trunk interface type\n"
        "The port/subport of trunk, format: <1-128>\n")

DEFSH(VTYSH_L2,
		show_stp_interface_ethernet_cmd_vtysh,
		"show stp interface (ethernet |gigabitethernet |xgigabitethernet) USP",
        SHOW_STR
        MSTP_STR
        "Specify interface\n"
	"GigabitEthernet interface type\n"
	"10GigabitEthernet interface type\n"
        "The port/subport of the interface,  format: <0-7>/<0-31>/<1-255>\n")



/*show stp instance*/
DEFSH(VTYSH_L2,
		show_stp_instance_cmd_vtysh,
        "show stp instance [<1-8>]",
        SHOW_STR
        MSTP_STR        
        "Spanning tree instance\n"
        "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")


#endif

#if 0

DEFSH(VTYSH_L2,
		debug_stp_cmd_vtysh,
        "debug stp (all | one_run_sm | two_run_sm | three_run_sm | recv_debug | send_debug | link_change | normal_debug)",
        "Output log of debug level\n"
        MSTP_STR        
		"one run sm\n"
		"two run sm\n"
		"three run sm\n"
		"recv debug\n"
		"send debug\n"
		"link change\n"	
		"normal debug\n")


DEFSH(VTYSH_L2,
		no_debug_stp_cmd_vtysh,
        "no debug stp (all | one_run_sm | two_run_sm | three_run_sm | recv_debug | send_debug | link_change |normal_debug)",
		"Set terminal line parameters\n"
		"Output log of debug level\n"
        MSTP_STR
		"one run sm\n"
		"two run sm\n"
		"three run sm\n"
		"recv debug\n"
		"send debug\n"
		"link change\n"	
		"normal debug\n")
#endif


/******mstp cmd config**************************************/



/*trunk子模??*/
DEFSH( VTYSH_L2,
	trunk_backup_cmd_vtysh,
	"backup non-failback",
	"Backup mode\n"
	"Interface no failback\n"
	)
	
DEFSH( VTYSH_L2,
	trunk_backup_wtr_cmd_vtysh,
	"backup failback [wtr <0-3600>]",
	"Backup mode\n"
	"Interface failback\n"
	"Set wtr\n"
	"The default value is 30s. Please input an integer from 0 to 3600\n"
	)
	
DEFSH( VTYSH_L2,
    no_trunk_backup_wtr_cmd_vtysh,
    "no backup failback wtr",
    NO_STR
    "Backup mode\n"
    "Interface failback\n"
    "Set wtr\n"
    )
DEFSH(VTYSH_L2,
	  gloable_trunk_link_aggregation_cmd_vtysh,
	"link-aggregation global load-sharing mode (destination-ip|destination-mac|destination-port|ingress-port|source-ip|source-mac|source-port|)",
      "Gloable link-aggregation load-sharing mode\n"
      "Gloable link-aggregation\n"
      "Gloable link-aggregation load-sharing mode\n"
      "Load-sharing mode\n"
      "Destination ip\n"
      "Destination mac\n"
      "Destination port\n"
      "Ingress port\n"
      "Source ip\n"
      "Source mac\n"
      "Source port \n"
      "Source port and dest port\n"
	)
DEFSH(VTYSH_L2,
	  no_gloable_trunk_link_aggregation_cmd_vtysh,
	   "undo link-aggregation global load-sharing mode",
	      NO_STR
	      "Link-aggregation load-sharing mode\n"
	      "Link-aggregation load-sharing mode\n"
	      "Link-aggregation load-sharing mode\n"
	 )

DEFSH(VTYSH_L2,
      gloable_trunk_load_balance_cmd_vtysh,
      "gloable-load-balance (label|sip-dip|dip|sip|dmac|smac|sport-dmac|smac-dmac |sport-dport)",
      "Gloable-load-balance mode\n"
      "Label\n"
      "Source ip and dest ip\n"
      "Dest ip\n"
      "Source ip\n"
      "Dest mac\n"
      "Source mac\n"
      "Source port and dest mac\n"
      "Source mac and dest mac\n"
      "Source port and dest port\n"
     )
	
DEFSH(VTYSH_L2,
	no_gloable_trunk_load_balance_cmd_vtysh,
		  "no gloable-load-balance",
		  NO_STR
		  "Gloable-load-balance mode\n"
		 )

DEFSH( VTYSH_L2,trunk_load_balance_cmd_vtysh,
	"load-balance (label |sip-dip|dip|sip|dmac|smac|sport-dmac|smac-dmac |sport-dport)",
	"Load-balance mode\n"
	"Label\n"
	"Source ip and dest ip\n"
	"Dest ip\n"
	"Source ip\n"
	"Dest mac\n"
	"Source mac\n"
	"Source port and dest mac\n"
	"Source mac and dest mac\n"
	"Source port and dest port\n"
	)

DEFSH(VTYSH_L2,no_trunk_load_balance_cmd_vtysh,
	"no load-balance",
	NO_STR
	"Load-balance mode\n"
	)
 
DEFSH( VTYSH_L2|VTYSH_IFM,trunk_member_port_set_cmd_vtysh,
    "trunk <1-128> {passive|priority <0-65535>}",
    "Set trunk member\n"
    "The value of trunk id\n"
    "Passive mode\n"
    "Port priority\n"
    "The default value is 32768. Please input an integer from 0 to 65535\n"
    )

DEFSH( VTYSH_L2|VTYSH_IFM,no_trunk_member_port_set_cmd_vtysh,
    "no trunk <1-128> {passive|priority}",
    NO_STR
    "Set trunk member\n"
    "The value of trunk id\n"
    "Passive mode\n"
    "Port priority\n"
    )

DEFSH(VTYSH_L2,trunk_lacp_enable_cmd_vtysh,
	"lacp enable",
	"Link aggregation control protocol\n"
	"Enable lacp\n"
	)

DEFSH(VTYSH_L2,no_trunk_lacp_enable_cmd_vtysh,
	"no lacp enable",
	NO_STR
	"Link aggregation control protocol\n"
	"Enable lacp\n"
	)

DEFSH(VTYSH_L2,trunk_lacp_interval_cmd_vtysh,
	"lacp interval <1-30>",
	"Link aggregation control protocol\n"
	"Packet send interval\n"
	"The default value is 1s. Please input an integer from 1 to 30\n"
	)

DEFSH(VTYSH_L2,no_trunk_lacp_interval_cmd_vtysh,
	"no lacp interval",
	NO_STR
	"Link aggregation control protocol\n"
	"Packet send interval\n"
	)

DEFSH(VTYSH_L2,trunk_lacp_priority_cmd_vtysh,
	"lacp priority <0-65535>",
	"Link aggregation control protocol\n"
	"Set lacp system priority\n"
	"The default value is 32768. Please input an integer from 0 to 65535\n"
	)

DEFSH(VTYSH_L2,no_trunk_lacp_priority_cmd_vtysh,
	"no lacp priority",
	NO_STR
	"Link aggregation control protocol\n"
	"Set lacp system priority\n"
	)

DEFSH(VTYSH_L2,debug_trunk_lacp_packet_cmd_vtysh,
	"debug lacp packet {recv|send}",
    "Set lacp debug\n"
	"Link aggregation control protocol\n"
	"Recvice/send protocol packet\n"
	"Recvice protocol packet\n"
	"Send protocol packet\n"
	)
DEFSH( VTYSH_L2,no_debug_trunk_lacp_packet_cmd_vtysh,
    "no debug lacp packet {recv|send}",
    NO_STR
    "Set lacp debug\n"
    "Link aggregation control protocol\n"
    "Recvice/send protocol packet\n"
    "Recvice protocol packet\n"
    "Send protocol packet\n"
    )

DEFSH(VTYSH_L2,show_trunk_lacp_statistics_cmd_vtysh,
	"show lacp statistics",
	SHOW_STR
	"Link aggregation control protocol\n"
	"Lacp packet statistics\n"
	)
	
#if 0
/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_L2,l2_log_level_ctl_cmd_vtysh,	"debug l2(enable | disable)",		
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")	

/* debug 日志发送到 syslog 使能状态显???*/
DEFSH (VTYSH_L2,l2_show_log_level_ctl_cmd_vtysh,"show debug l2",		
	SHOW_STR"Output log of debug level\n""Program name\n")	
#endif
/************** static mac 子模块 **********************************/

DEFUNSH (VTYSH_L2 | VTYSH_MPLS,
	mac_mode,
        mac_mode_cmd_vtysh,
        "mac",
        "Mac command mode\n")
{
        vty->node = MAC_NODE;

        return CMD_SUCCESS;
}

DEFUNSH(VTYSH_L2,
	vtysh_exit_mac,
	vtysh_exit_mac_cmd_vtysh,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

DEFSH(VTYSH_L2,
      configure_ethernet_static_mac_cmd_vtysh,
      "mac static XX:XX:XX:XX:XX:XX interface ethernet USP vlan <1-4094> { blackhole}",
      "Mac\n"
      "Static mac \n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Interface \n"
      "Interface type ethernet\n"
      "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
      "Vlan \n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")

DEFSH(VTYSH_L2,
	  configure_gigabit_ethernet_static_mac_cmd_vtysh,
	  "mac static XX:XX:XX:XX:XX:XX interface gigabitethernet USP vlan <1-4094> { blackhole}",
	  "Mac\n"
	  "Static mac \n"
	  "Mac format XX:XX:XX:XX:XX:XX\n"
	  "Interface \n"
	  "Interface type gigabitethernet\n"
	  "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
	  "Vlan \n"
	  "Vlan value <1-4094>\n"
	  "Blackhole \n")

DEFSH(VTYSH_L2,
      configure_xgigabit_ethernet_static_mac_cmd_vtysh,
      "mac static XX:XX:XX:XX:XX:XX interface xgigabitethernet USP vlan <1-4094> { blackhole}",
      "Mac\n"
      "Static mac \n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Interface \n"
      "Interface type xgigabitethernet\n"
      "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
      "Vlan \n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")

DEFSH(VTYSH_L2,
      configure_trunk_static_mac_cmd_vtysh,
      "mac static XX:XX:XX:XX:XX:XX interface trunk <1-128> vlan <1-4094> { blackhole}",
      "Mac\n"
      "Static mac \n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Interface \n"
      "Interface type trunk\n"
      "Trunk <1-128>\n"
      "Vlan \n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")

DEFSH(VTYSH_L2 | VTYSH_MPLS,
      show_mac_static_all_cmd_vtysh,
      "show mac static",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n")

DEFSH(VTYSH_L2,
      show_mac_static_mac_cmd_vtysh,
      "show mac static mac XX:XX:XX:XX:XX:XX ",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Mac\n"
      "Mac format XX:XX:XX:XX:XX:XX\n")
      
DEFSH(VTYSH_L2,
      show_mac_static_ethernet_cmd_vtysh,
      "show mac static interface ethernet USP {vlan <1-4094> | blackhole}",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Interface\n"
      "Interface type ethernet\n"
      "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
      "Vlan \n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")

DEFSH(VTYSH_L2,
	  show_mac_static_gigabit_ethernet_cmd_vtysh,
	  "show mac static interface gigabitethernet USP {vlan <1-4094> | blackhole}",
	  "Show static mac information\n"
	  "Mac\n"
	  "Static mac \n"
	  "Interface\n"
	  "Interface type gigabitethernet\n"
	  "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
	  "Vlan \n"
	  "Vlan value <1-4094>\n"
	  "Blackhole \n")

DEFSH(VTYSH_L2,
	  show_mac_static_xgigabit_ethernet_cmd_vtysh,
	  "show mac static interface xgigabitethernet USP {vlan <1-4094> | blackhole}",
	  "Show static mac information\n"
	  "Mac\n"
	  "Static mac \n"
	  "Interface\n"
	  "Interface type xgigabitethernet\n"
	  "The port/subport of the interface, format: <1-7>/<1-31>/<1-255>\n"
	  "Vlan \n"
	  "Vlan value <1-4094>\n"
	  "Blackhole \n")


DEFSH(VTYSH_L2,
      show_mac_static_trunk_cmd_vtysh,
      "show mac static interface trunk <1-128> {vlan <1-4094> | blackhole}",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Interface\n"
      "Interface type trunk\n"
      "Trunk <1-128>\n"
      "Vlan\n"
      "Vlan value <1-4094>\n"
      "Blackhole \n")   

DEFSH(VTYSH_L2,
      show_mac_static_vlan_cmd_vtysh,
      "show mac static vlan <1-4094>",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Vlan\n"
      "Vlan value <1-4094>\n")
      
DEFSH(VTYSH_L2,
      show_mac_static_blackhole_cmd_vtysh,
      "show mac static blackhole",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n"
      "Blackhole mac\n")      

DEFSH(VTYSH_L2,
      undo_configure_static_mac_cmd_vtysh,
      "no mac XX:XX:XX:XX:XX:XX vlan <1-4094>",
      "Mac disable\n"
      "Mac\n"
      "Mac format XX:XX:XX:XX:XX:XX\n"
      "Vlan\n"
      "Vlan value <1-4094>\n")

DEFSH(VTYSH_L2,
      configure_mac_ageing_time_cmd_vtysh,
      "mac age-time <0-65535>",
      "Mac\n"
      "Dynamic mac ageing time \n"
      "Time value <0-65535> seconds and the default value is 300 seconds.\n")
      
DEFSH(VTYSH_L2,
      no_configure_mac_ageing_time_cmd_vtysh,
      "no mac age-time",
      "Disable mac ageing time\n"
      "Mac\n"
      "Dynamic mac ageing time \n")     

DEFSH(VTYSH_L2,
      show_mac_config_cmd_vtysh,
      "show mac config",
      "Show\n"
      "Mac\n"
      "Mac configuration \n")
      
DEFSH(VTYSH_L2,
      config_mac_move_cmd_vtysh,
      "mac move (enable |disable)",
      "Mac\n"
      "Config mac move\n"
      "Mac move enable \n"
      "Mac move disable  \n")    
      
DEFSH(VTYSH_L2,
      mac_mac_limit_cmd_vtysh,
      "mac limit <1-16384> action (discard | forward)",
      "Mac\n"
      "Limit the count of mac learn\n"
      "Limit value <1-16384> and the default value is 0.\n"
      "Action when limit value is exceeded\n"
      "Discard the packet\n"
      "Forward the packet\n") 
      
DEFSH(VTYSH_L2,
      no_mac_mac_limit_cmd_vtysh,
      "no mac limit",
      "Clear config\n"
      "Mac\n"
      "Limit the count of mac learn\n")    

//---------------cfm md-------------------------------------//
DEFUNSH(VTYSH_L2,
		md_view,
		md_view_cmd,
		"cfm md  <1-8>",
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")
{
	char *pprompt = NULL;
	uint8_t md_index;
	
	vty->node = CFM_MD_NODE;	

	md_index = atoi(argv[0]);
	vty->index = (void *)(unsigned int)md_index;
		
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-md-%d)# ", md_index);
	}
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
	vtysh_exit_md,
	vtysh_exit_md_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_md,
	vtysh_quit_md_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")


DEFSH(VTYSH_L2,
		no_md_view_cmd_vtysh,
		"no cfm md  <1-8>",
		NO_STR
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")

DEFSH(VTYSH_L2,
		md_name_cmd_vtysh,
		"name  NAME",
		"Md name\n"
		"Md name--max.31\n")


DEFSH(VTYSH_L2,
		md_level_cmd_vtysh,
		"level  <0-7>",
		"Maintenance domain level\n"
		"Md level ,<0-7>\n")

DEFSH(VTYSH_L2,
		no_md_level_cmd_vtysh,
		"no level",
		NO_STR		
		"Maintenance domain level\n")

//---------------cfm ma-------------------------------------//
DEFUNSH(VTYSH_L2,
		ma_view,
		ma_view_cmd,
		"ma  <1-1024>",
		"Maintenance association\n"
		"Ma index ,<1-1024>\n")
{
	char *pprompt = NULL;
	uint16_t ma_index;

	vty->node = CFM_MA_NODE;	

	ma_index = atoi(argv[0]);	
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-md-%d-ma-%d)# ", (unsigned int)vty->index,ma_index);
	}
	
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
	vtysh_exit_ma,
	vtysh_exit_ma_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	char *pprompt = NULL;
	
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-md-%d)# ", (unsigned int)vty->index);
	}

	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_ma,
	vtysh_quit_ma_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")

DEFSH(VTYSH_L2,
		no_ma_view_cmd_vtysh,
		"no ma  <1-1024>",
		NO_STR
		"Maintenance association\n"
		"Ma index ,<1-1024>\n")

DEFSH(VTYSH_L2 ,
		ma_name_cmd_vtysh,
		"name  NAME",
		"Ma name\n"
		"Ma name--max.31\n")

DEFSH(VTYSH_L2 ,
		no_ma_name_cmd_vtysh,
		"no name",
		NO_STR
		"Ma name\n")


DEFSH(VTYSH_L2,
		ma_service_vlan_cmd_vtysh,
		"service vlan <1-4094>",
		"Service\n"
		"vlan\n"		
		"Vlan index ,<1-4094>\n")
		
DEFSH(VTYSH_L2,
		no_ma_service_vlan_cmd_vtysh,
		"no service vlan",
		NO_STR
		"Service\n"
		"Vlan\n")

DEFSH(VTYSH_L2 ,
		ma_priority_cmd_vtysh,
		"priority  <0-7>",
		"Packet cos priority\n"
		"Priority index ,<0-7>\n")

DEFSH(VTYSH_L2 ,
		no_ma_priority_cmd_vtysh,
		"no priority",
		NO_STR
		"Packet cos priority\n")

DEFSH(VTYSH_L2 ,
		ma_mip_enable_cmd_vtysh,
		"mip enable",
		"Maintenance association Intermediate Point\n"
		"Enable\n")

DEFSH(VTYSH_L2 ,
		no_ma_mip_enable_cmd_vtysh,
		"no mip enable",
		NO_STR
		"Maintenance association Intermediate Point\n"
		"Enable\n")

DEFSH(VTYSH_L2 ,
		ma_1731_enable_cmd_vtysh,
		"protocol y.1731",
		"protocol\n"
		"y.1731\n")

DEFSH(VTYSH_L2 ,
		no_ma_1731_enable_cmd_vtysh,
		"no protocol y.1731",
		NO_STR
		"protocol\n"
		"y.1731\n")


		
//---------------cfm session----------------------------------//
DEFUNSH(VTYSH_L2,
		session_name,
		session_name_cmd,
		"cfm session  <1-1024>",
		"Connectivity fault management\n"
		"Session\n"
		"Session index ,<1-1024>\n")
{
	uint16_t sess_index;
	char *pprompt = NULL;

    VTY_GET_INTEGER_RANGE ( "session", sess_index, argv[0], 1, 1024);

	vty->node = CFM_SESSION_NODE;	
    pprompt = vty->change_prompt;
    if ( pprompt )
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-cfm-session-%d)# ", sess_index);
	}
	
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
	vtysh_exit_cfmsess,
	vtysh_exit_cfmsess_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}

ALIAS (vtysh_exit_cfmsess,
	vtysh_quit_cfmsess_cmd,
	"quit",
	"Exit current mode and down to previous mode\n")

DEFSH(VTYSH_L2,
		no_session_name_cmd_vtysh,
		"no cfm session  <1-1024>",
		NO_STR
		"Connectivity fault management\n"
		"Session\n"
		"Session index ,<1-1024>\n")

DEFSH(VTYSH_L2,
		session_service_ma_cmd_vtysh,
		"service ma <1-1024>",
		"Service\n"
		"Maintenance association\n"		
		"Ma index ,<1-1024>\n")

DEFSH(VTYSH_L2,
		no_session_service_ma_cmd_vtysh,
		"no service ma",
		NO_STR
		"Service\n"
		"Maintenance association\n")		
		
		
DEFSH(VTYSH_L2,
		session_direction_down_cmd_vtysh,
		"direction down",
		"Direction\n"
		"Down:outward\n")
			
DEFSH(VTYSH_L2,
		session_direction_up_cmd_vtysh,
		"direction up  {over_pw}",
		"Direction\n"
		"Up:inward \n" 
		"Over pseudo wire\n")


DEFSH(VTYSH_L2,
		no_session_direction_cmd_vtysh,
		"no direction",
		NO_STR
		"Direction\n")

DEFSH(VTYSH_L2,
		session_localmep_cmd_vtysh,
		"local-mep <1-8191>",
		"Local maintenance association end point\n"
		"Local-mep index\n")
		
DEFSH(VTYSH_L2,
		no_session_localmep_cmd_vtysh,
		"no local-mep",
		NO_STR
		"Local maintenance association end point\n")
		
DEFSH(VTYSH_L2,
		session_rmep_cmd_vtysh,
		"remote-mep <1-8191> {mac XX:XX:XX:XX:XX:XX}",
		"Remote maintenance association end point\n"
		"Remote-mep index\n"
		"Mac address\n"
		"Mac address\n")

DEFSH(VTYSH_L2,
		no_session_rmep_cmd_vtysh,
		"no remote-mep",
		NO_STR
		"remote maintenance association end point\n")

DEFSH(VTYSH_L2,
		session_ccm_en_cmd_vtysh,
		" cc enable (3ms | 10ms | 100ms | 1s | 10s | 60s | 600s)",
		"Continuity check message\n"
		"Enable\n"
		"Interval value 3ms\n"
		"Interval value 10ms\n"
		"Interval value 100ms\n"
		"Interval value 1s\n"
		"Interval value 10s\n"
		"Interval value 60s\n"
		"Interval value 600s\n")

DEFSH(VTYSH_L2,
		no_session_ccm_en_cmd_vtysh,
		"no cc enable",
		NO_STR
		"Continuity check message\n"
		"Enable\n")

DEFSH(VTYSH_L2,
        cfm_lb_enable_cmd_vtysh,
	    "lb enable {mac XX:XX:XX:XX:XX:XX|size <95-1500>|discard <0-7>}",
	    "Loopback.\n"
	    "Enable.\n"
		"Mac address\n"
		"Mac address\n"
		"Number of data bytes\n"
		"Bytes 95 to 1500,default 95\n"
		"Discard loopback packets when congestion conditions\n"
		"Discard priority,default 0\n")
		

DEFSH(VTYSH_L2,
    no_cfm_lb_enable_cmd_vtysh,
    "no lb enable",
	NO_STR    
    "Loopback.\n"
    "Enable.\n")


DEFSH(VTYSH_L2,
        cfm_lt_enable_cmd_vtysh,
   		"lt enable {mac XX:XX:XX:XX:XX:XX}",
    	"Link tracing.\n"
   	    "Enable.\n"
		"Mac address\n"
		"Mac address\n")

DEFSH(VTYSH_L2,
    no_cfm_lt_enable_cmd_vtysh,
    "no lt enable",
    NO_STR
    "Link tracing.\n"
    "Enable.\n")


DEFSH(VTYSH_L2,
        cfm_lm_enable_cmd_vtysh,
	    "lm enable {interval <5-3600> | mac XX:XX:XX:XX:XX:XX }",
	    "Loss Measurement\n"
	    "Enable\n"
	    "Interval of Loss Measurement Message\n"
	    "Range of interval:<5-3600>,unit:second\n"
	    "Mac address\n"
	    "Mac address\n")


DEFSH(VTYSH_L2,
        no_cfm_lm_enable_cmd_vtysh,
        "no lm enable",
        "Delete\n"
        "Loss Measurement\n"
        "Enable\n")

DEFSH(VTYSH_L2,
        cfm_dm_enable_cmd_vtysh,
	    "dm enable {interval <5-3600> | mac XX:XX:XX:XX:XX:XX}",
	    "Delay Measurement\n"
	    "Enable\n"
	    "Interval of Delay Measurement Message\n"
	    "Range of interval:<5-3600>,unit:second\n"
	    "Mac address\n"
	    "Mac address\n")


DEFSH(VTYSH_L2,
        no_cfm_dm_enable_cmd_vtysh,
        "no dm enable",
        "Delete\n"
        "Delay Measurement\n"
        "Enable\n")


DEFSH(VTYSH_L2,
		show_cfm_md_cmd_vtysh,
		"show cfm md [<1-8>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Maintenance domain\n"
		"Md index,<1-8>\n")

DEFSH(VTYSH_L2,
		show_cfm_ma_cmd_vtysh,
		"show cfm ma [<1-1024>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Maintenance association\n"
		"Ma index,<1-1024>\n")

DEFSH(VTYSH_L2,
		show_cfm_session_cmd_vtysh,
		"show cfm session [<1-1024>]",
		SHOW_STR
		"Connectivity fault management\n"
		"Session\n"
		"Session index,<1-1024>\n")


DEFSH(VTYSH_L2,
	show_cfm_lm_dm_result_cmd_vtysh,
	"show cfm session <1-1024> (sla|test)",
	SHOW_STR
	"Connectivity fault management\n"
	"Session\n"
	"Range of session index:<1-1024>\n"
	"Service Level Agreement\n"
    "Eth-test\n")	
		
/*DEFSH(VTYSH_L2,
		cfm_if_enable_session_cmd_vtysh,
		"cfm enable session <1-8191>",
		"Connectivity fault management\n"
		"enable\n"	
		"session\n"
		"session index ,<1-8191>\n")

DEFSH(VTYSH_L2,
		no_cfm_if_enable_session_cmd_vtysh,
		"no cfm enable session <1-8191>",
		NO_STR
		"Connectivity fault management\n"
		"enable\n"	
		"session\n"
		"session index ,<1-8191>\n")*/


DEFSH(VTYSH_L2,
		cfm_enable_interface_gigabit_ethernet_cmd_vtysh,
		"cfm enable  interface gigabitethernet USP",
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

DEFSH(VTYSH_L2,
		no_cfm_enable_interface_gigabit_ethernet_cmd_vtysh,
		"no cfm enable	interface gigabitethernet USP",
		NO_STR
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR)

DEFSH(VTYSH_L2,
		cfm_enable_interface_xgigabit_ethernet_cmd_vtysh,
		"cfm enable  interface xgigabitethernet USP",
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)

DEFSH(VTYSH_L2,
		no_cfm_enable_interface_xgigabit_ethernet_cmd_vtysh,
		"no cfm enable  interface xgigabitethernet USP",
		NO_STR
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR)



DEFSH(VTYSH_L2,
    cfm_ais_enable_cmd_vtysh,
    "ais enable interval (1s|60s) level <1-7>",
    "Alarm indication signal\n"
    "Enable\n"
    "Interval of AIS message\n"
    "interval: 1s\n"
    "interval: 60s\n"    
    "The md level\n"
    "The md level value: 1-7\n")

DEFSH(VTYSH_L2,
    no_cfm_ais_enable_cmd_vtysh,
    "no ais enable  ",
    NO_STR
    "Alarm indication signal\n"
    "Enable\n")

DEFSH(VTYSH_L2,
    cfm_tst_mode_in_service_cmd_vtysh,
    "test mode in-service",
    "Eth-test\n"
    "Mode of eth-test\n"
    "MEP is configured for in-service testing\n")

DEFSH(VTYSH_L2,
    cfm_tst_mode_out_service_cmd_vtysh,
    "test mode out-of-service lck-interval (1s|60s) lck-level <1-7>",
    "Eth-test\n"
    "Mode of eth-test\n"
    "MEP is configured for out-of-service testing\n"
    "Tx Interval of lck message, should be same as ais tx interval\n"
    "interval: 1s\n"
    "interval: 60s\n"    
    "Lck target md level\n"
    "The md level value: 1-7\n")

DEFSH(VTYSH_L2,
    cfm_test_enable_cmd_vtysh,
    "test enable {"
    "mac XX:XX:XX:XX:XX:XX"
    "|size <64-1400>|number <1-10>"
    "|interval <0-10>"
    "|pattern (zero_without_crc|zero_with_crc|prbs_without_crc|prbs_with_crc)|discard <0-7>}",
    "Eth-test\n"
    "Enable\n"
    "Mac address\n"
    "Mac address\n"
	"Number of data bytes\n"
	"Bytes 64 to 1400,default 64\n"
	"Number of packets\n"
	"Packet numbers 1 to 10,default 5\n"
	"Interval of test message\n"
	"Interval 0 to 10s, default 1s\n"
	"Pattern of the test TLV in the TST message\n"
	"All zero without CRC32(default)\n"
	"All zero with CRC32\n"
	"Pseudo random bit sequence without CRC32\n"
	"Pseudo random bit sequence with CRC32\n"
	"Discard test packets when congestion conditions\n"
	"Discard priority,default 0\n")

DEFSH(VTYSH_L2,
    no_cfm_test_enable_cmd_vtysh,
    "no test enable",
    NO_STR
    "Eth-test\n"
    "Enable\n")

/***************************cfm debug********************************/
DEFSH (VTYSH_L2,
	cfm_debug_cmd_vtysh,
	"debug cfm (enable|disable) (lb|lt|test|common|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"CFM debug enable\n"
	"CFM debug disable\n"
	"CFM debug type lb\n"
	"CFM debug type lt\n"
	"CFM debug type test\n"
	"CFM debug type common\n"
	"CFM debug type all\n")


DEFSH (VTYSH_L2,
	show_cfm_debug_cmd_vtysh,
	"show cfm debug",
	SHOW_STR
	"Cfm"
	"Debug status\n")
	
/****************************l2 debug*******************************/
DEFSH (VTYSH_L2,
	  l2_debug_cmd_vtysh,
	  "debug l2 (enable|disable) (elps|erps|lacp|lldp|mstp|trunk|mac|qinq|vlan|efm|common|all)",
	  "Debug information to moniter\n"
	  "config l2 debug information\n"
	  "l2 debug enable\n"
	  "l2 debug disable\n"
	  "l2 debug type elps\n"
	  "l2 debug type erps\n"
	  "l2 debug type lacp\n"
	  "l2 debug type lldp\n"
	  "l2 debug type mstp\n"
	  "l2 debug type trunk\n"
	  "l2 debug type mac static\n"
	  "l2 debug type qinq\n"
	  "l2 debug type vlan\n"
	  "l2 debug type efm\n"
	  "l2 debug type common\n"
	  "l2 debug type all\n")

DEFSH (VTYSH_L2,
	   show_l2_debug_cmd_vtysh,
	   "show l2 debug",
	   SHOW_STR
	   "l2"
	   "Debug status\n")


DEFSH (VTYSH_L2,
       show_l2_timerinfo_cmd_vtysh,
           "show l2 timer",
       SHOW_STR
            "L2  module information\n"
            "L2  module timer\n")

//---------------elps  session start  ------------------------------//

/**
* \page elps_session_cmd elps session
* - 功能说明 \n
*   config 视图下创???elps session
* - 命令格式 \n
*    elps session <1-128>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-128> |每一个session用一个数字来标识，取值范围是1~128|
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps session <1-128>
* - 使用举例 \n
*     Hios(config)#elps session 1   
*
*/

DEFUNSH(VTYSH_L2,
	elps_session,
	elps_session_cmd,
	"elps session <1-128>",
	"Elps management\n"
	"Session\n"
	"Range of sess_id:<1-128>\n")
{
    vty->node = ELPS_SESSION_NODE;
    return CMD_SUCCESS;
}

DEFUNSH (VTYSH_L2,
        vtysh_exit_elps_session,
        vtysh_exit_elps_session_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}


ALIAS (vtysh_exit_elps_session,
        vtysh_quit_elps_session_cmd,
        "quit",
        "Exit current mode and down to previous mode\n")

DEFSH(VTYSH_L2,
	no_elps_session_cmd_vtysh,
	"no elps session <1-128>",
	"Delete\n"
	"Elps management\n"
	"Session\n"
	"Range of sess_id:<1-128>\n")


/**
* \page elps_protect_port_cmd_vtysh elps protect-port
* - 功能说明 \n
*   session 视图下配置保护端???
* - 命令格式 \n
*   elps protect-port master ethernet USP backup ethernet USP
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |ethernet USP   |主端???|
*   |ethernet USP   |备端???|
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session的主、备端口
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps protect-port
* - 使用举例 \n
*     Hios(config-elps-session)# elps protect-port master ethernet 1/1/3 backup ethernet 1/1/4 
*
*/	
DEFSH(VTYSH_L2,
	elps_protect_port_cmd_vtysh,
	"elps protect-port master (ethernet|gigabitethernet|xgigabitethernet) USP backup (ethernet|gigabitethernet|xgigabitethernet) USP",
	"Elps management\n"
	"Protect port\n"
	"Master\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	"Backup\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH(VTYSH_L2,
	no_elps_protect_port_cmd_vtysh,
	"no elps protect-port",
	"Delete\n"
	"Elps management\n"
	"Protect port\n")


/**
* \page elps_data_vlan_cmd_vtysh elps vlan
* - 功能说明 \n
*   session 视图下配置保护的 vlan 范围
* - 命令格式 \n
*   elps vlan <1-4094> {to <1-4094> }
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-4094>   |起始VLAN ID，整数形式，取值范???-4094 |
*   |<1-4094>   |结束VLAN ID，整数形式，取值范???-4094 |
* - 缺省情况 \n
*   缺省情况下，起始vlan id 和结???vlan id 一???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session的保护vlan
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps vlan <1-4094> {to <1-4094> }
*     no elps vlan all
* - 使用举例 \n
*     Hios(config-elps-session)# elps vlan 4
*
*/

DEFSH(VTYSH_L2,
	elps_data_vlan_cmd_vtysh,
	"elps vlan <1-4094> {to <1-4094> }",
	"Elps management\n"
	"Elps data  vlan start\n"
	"Range of data vlan:<1-4094>\n"
	"Elps data  vlan end\n"
	"Range of data vlan:<1-4094>\n")

DEFSH(VTYSH_L2,
	no_elps_data_vlan_cmd_vtysh,
	"no elps vlan <1-4094> {to <1-4094> }",
	"Delete\n"
	"Elps management\n"
	"Elps data  vlan\n"
	"Elps data  vlan start\n"
	"Range of data vlan:<1-4094>\n"
	"Elps data  vlan end\n"
	"Range of data vlan:<1-4094>\n")

DEFSH(VTYSH_L2,
	no_elps_data_vlan_all_cmd_vtysh,
	"no elps vlan all",
	"Delete\n"
	"Elps management\n"
	"Elps data  vlan\n"
	"All \n")


/**
* \page elps_primary_vlan_cmd_vtysh elps primary-vlan
* - 功能说明 \n
*   session 视图下配置发送报文的协议 vlan
* - 命令格式 \n
*   elps primary-vlan <1-4094>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-4094>   |协议VLAN ID，整数形式，取值范???-4094 |
* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session协议vlan
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps primary-vlan
* - 使用举例 \n
*     Hios(config-elps-session)# elps primary-vlan 3
*
*/

DEFSH(VTYSH_L2,
	elps_primary_vlan_cmd_vtysh,
	"elps primary-vlan <1-4094>",
	"Elps management\n"
	"Elps primary vlan\n"
	"Range of primary vlan:<1-4094>\n")

DEFSH(VTYSH_L2,
	no_elps_primary_vlan_cmd_vtysh,
	"no elps primary-vlan",
	"Delete\n"
	"Elps management\n"
	"Elps primary vlan\n")


/**
* \page elps_keep_alive_cmd_vtysh elps keep-alive
* - 功能说明 \n
*   session视图下配置报文发送间???
* - 命令格式 \n
*   elps keep-alive <1-600>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-600>   |Aps报文发送间隔，单位为秒（s），默认???|
* - 缺省情况 \n
*   缺省情况下，aps报文发送间隔为5s???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置aps报文的发送间???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps keep-alive
* - 使用举例 \n
*     Hios(config-elps-session)# elps keep-alive 10
*
*/

DEFSH(VTYSH_L2,
	elps_keep_alive_cmd_vtysh,
	"elps keep-alive <1-600>",
	"Elps management\n"
	"Elps keep alive\n"
	"Range of keep alive:<1s-600s>\n")

DEFSH(VTYSH_L2,
	no_elps_keep_alive_cmd_vtysh,
	"no elps keep-alive",
	"Delete\n"
	"Elps management\n"
	"Elps keep alive\n")

/**
* \page elps_hold_off_cmd_vtysh elps keep-alive
* - 功能说明 \n
*   session视图下配???hold-off 时间
* - 命令格式 \n
*   elps hold-off <0-600>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<0-600>   |Elps session 的hold-off时间，单位为秒（s），默认???|
* - 缺省情况 \n
*   缺省情况下，elps session的hold-off时间???s???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session的hold-off时间
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps hold-off
* - 使用举例 \n
*     Hios(config-elps-session)# elps hold-off 0 
*
*/

DEFSH(VTYSH_L2,
	elps_hold_off_cmd_vtysh,
	"elps hold-off <0-600>",
	"Elps management\n"
	"Elps hold off\n"
	"Range of hold off:<0s-600s>\n")

DEFSH(VTYSH_L2,
	no_elps_hold_off_cmd_vtysh,
	"no elps hold-off ",
	"Delete\n"
	"Elps management\n"
	"Elps hold off\n")


/**
* \page elps_backup_failback_cmd_vtysh elps backup
* - 功能说明 \n
*   session 视图下配置切换模式和回切时间
* - 命令格式 \n
*   elps backup { non-failback | failback [ wtr <1-12> ] }
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |non-failback   |非返回模式|
*   |failback       |返回模式|
*   |<1-12>         |Elps session 的wtr时间，单位为分钟（min），默认???|
* - 缺省情况 \n
*   缺省情况下，elps session 的切换模式为failback时，wtr的时间为5min???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session切换模式和回切时间???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps backup
* - 使用举例 \n
*     Hios(config-elps-session)# elps backup failback wtr 1
*
*/

DEFSH(VTYSH_L2,
	elps_backup_create_cmd_vtysh,
	"elps backup non-failback ",
	"Elps management\n"
	"Protection restoration mode\n"
	"Non-revertive mode\n")
	
DEFSH(VTYSH_L2,
	elps_backup_failback_cmd_vtysh,
	"elps backup failback [ wtr <1-12> ]",
	"Elps management\n"
	"Protection restoration mode\n"
	"Revertive mode\n"
	"Wait to restore\n"
	"Range of wtr:<1min-12min>\n")


DEFSH(VTYSH_L2,
	no_elps_backup_create_cmd_vtysh,
	"no elps backup",
	"Delete\n"
	"Elps management\n"
	"Protection restoration mode\n")


/**
* \page elps_priority_cmd_vtysh elps priority
* - 功能说明 \n
*   session 视图下配???aps 报文的优先级???
* - 命令格式 \n
*   elps priority <0-7>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<0-7>   |Aps报文优先级，默认???。|
* - 缺省情况 \n
*   缺省情况下，elps session的aps 报文的优先级??????
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session的aps 报文的优先级???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps priority 
* - 使用举例 \n
*     Hios(config-elps-session)# elps priority 5  
*
*/

DEFSH(VTYSH_L2,
	elps_priority_cmd_vtysh,
	"elps priority <0-7>",
	"Elps management\n"
	"Elps packet priority\n"
	"Range of priority:<0-7>\n")

DEFSH(VTYSH_L2,
	no_elps_priority_cmd_vtysh,
	"no elps priority ",
	"Delete\n"
	"Elps management\n"
	"Elps packet priority\n")


/**
* \page elps_bind_cfm_session_cmd_vtysh elps bind cfm session
* - 功能说明 \n
*   session 视图下配???aps 报文的优先级???
* - 命令格式 \n
*   elps bind cfm session <1-1024>{master |backup}
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-8192>    |cfm session 的session ID|
*   |master      |CFM会话绑定在主端口上|
*   |backup      |CFM会话绑定在备端口上|
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置与elps session绑定的CFM会话的session ID
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no elps bind cfm session <1-1024>{master |backup}
* - 使用举例 \n
*     Hios(config-elps-session)# elps bind cfm session 1 master 
*
*/

DEFSH(VTYSH_L2,
	elps_bind_cfm_session_cmd_vtysh,
	"elps bind cfm session <1-1024>(master |backup)",
	"Elps management\n"
	"bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"Session index ,<1-1024>\n"
	"Master\n"
	"Backup\n")

DEFSH(VTYSH_L2,
	no_elps_bind_cfm_session_cmd_vtysh,
	"no elps bind cfm session <1-1024>(master |backup)",
	"Delete\n"
	"Elps management\n"
	"bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"Session index ,<1-1024>\n")


/**
* \page elps_session_enable_cmd_vtysh elps (enable | disable)
* - 功能说明 \n
*   session 视图下配置会话使???
* - 命令格式 \n
*   elps (enable | disable)
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |enable    |Elps session 使能|
*   |disable   |Elps session 无效|
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session 使能/无效
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*     Hios(config-elps-session)# elps enable 
*
*/

DEFSH(VTYSH_L2,
	elps_session_enable_cmd_vtysh,
	"elps (enable | disable)",
	"Elps management\n"
	"Enable\n"
	"Disable\n")

/**
* \page elps_debug_packet_cmd_vtysh debug debug elps packet
* - 功能说明 \n
*   config 视图下显???elps 报文的信???
* - 命令格式 \n
*   debug elps packet
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 elps 报文的信???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      no debug elps packet
* - 使用举例 \n
*     Hios(config)# debug elps packet
*
*/

DEFSH(VTYSH_L2,
	elps_debug_packet_cmd_vtysh,
	"debug elps packet",
	"DEBUG_STR"
	"Elps management\n"
	"Elps packet\n" )

DEFSH(VTYSH_L2,
	no_elps_debug_packet_cmd_vtysh,
	"no debug elps packet",
	"Delete\n"
	"DEBUG_STR"
	"Elps management\n"
	"Elps packet\n" )

/**
* \page elps_admin_force_cmd_vtysh  elps force-switch
* - 功能说明 \n
*   session 视图下配置强制切换到???
* - 命令格式 \n
*    elps force-switch
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session强制切换到备
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      no elps force-switch
* - 使用举例 \n
*     Hios(config-elps-session)# elps force-swich
*
*/

DEFSH(VTYSH_L2,
	elps_admin_force_cmd_vtysh,
	"elps force-switch",
	"Elps management\n"
	"Force switching\n" )

DEFSH(VTYSH_L2,
	no_elps_admin_force_cmd_vtysh,
	"no elps force-switch",
	"Delete\n"
	"Elps management\n"
	"Force switching\n" )


/**
* \page elps_admin_manual_cmd_vtysh  elps manual-switch
* - 功能说明 \n
*   session 视图下配置手工切换到???
* - 命令格式 \n
*    elps manual-switch
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session手工切换到备
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      no elps manual-switch
* - 使用举例 \n
*     Hios(config-elps-session)# elps manual-switch
*
*/

DEFSH(VTYSH_L2,
	elps_admin_manual_cmd_vtysh,
	"elps manual-switch",
	"Elps management\n"
	"Manual switching\n" )

DEFSH(VTYSH_L2,
	no_elps_admin_manual_cmd_vtysh,
	"no elps manual-switch",
	"Delete\n"
	"Elps management\n"
	"Manual switching\n" )


/**
* \page elps_admin_lockout_cmd_vtysh  elps lockout
* - 功能说明 \n
*   session 视图下配???lock
* - 命令格式 \n
*    elps lockout
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   session模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     配置elps session 的lock
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      no elps lockout
* - 使用举例 \n
*     Hios(config-elps-session)# elps lockout
*
*/

DEFSH(VTYSH_L2,
	elps_admin_lockout_cmd_vtysh,
	"elps lockout",
	"Elps management\n"
	"Lockout of protection\n" )

DEFSH(VTYSH_L2,
	no_elps_admin_lockout_cmd_vtysh,
	"no elps lockout",
	"Delete\n"
	"Elps management\n"
	"Lockout of protection\n" )


/**
* \page show_elps_config_cmd_vtysh  show elps config
* - 功能说明 \n
*    config 视图下显???elps的配???
* - 命令格式 \n
*    show elps config
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 elps的配???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      ???
* - 使用举例 \n
*     Hios (config)# show elps config
*     Num:1
*       session-id        : 1
*       master-port       : ethernet 1/1/2
*       backup-port       : ethernet 1/1/3
*       primary-vlan      : 3
*       protection-vlan   : 4-100
*       keep-alive        : 5
*       hold-off          : 0
*       backup-mode       : Failback
*       wtr               : 60
*       priority          : 6
*       session-status    : enable
*
*/


DEFSH(VTYSH_L2,
	show_elps_config_cmd_vtysh,
	"show elps config",
	"Display.\n"
	"Elps management\n"
	"Config\n")

/**
* \page show_elps_session_cmd_vtysh  show elps session
* - 功能说明 \n
*   config 视图下显???elps session的信???
* - 命令格式 \n
*    show elps session [<1-128>]
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-128> |每一个session用一个数字来标识，取值范围是1~128|
* - 缺省情况 \n
*   缺省情况下，显示所有配置的session信息 ???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 elps session的信???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*     Hios (config)# show elps session
*     Total            : 1
*     enable_num       : 1
*     Num:1
*     session-id    master-port       backup-port       current-state       
*     ------------------------------------------------------------------------
*     1             ethernet 1/1/2    ethernet 1/1/3    Signal Fail for Protection
*
*       primary-vlan      : 3
*       active-port       : Master-port
*       protection-vlan   : 4-100
*       keep-alive        : 5
*       hold-off          : 0
*       backup-mode       : Failback
*       wtr               : 60
*       priority          : 6
*       session-status    : enable
*
*/

DEFSH(VTYSH_L2,
	show_elps_session_cmd_vtysh,
	"show elps session [<1-128>]",
	"Display.\n"
	"Elps management\n"
	"Session\n"
	"Range of local_mep:<1-128>\n")


/**
* \page show_elps_debug_cmd_vtysh  show elps debug
* - 功能说明 \n
*    config 视图下显示APS报文收发包数量及debug开关状???
* - 命令格式 \n
*    show elps debug
* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   无???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   11
* - 使用指南 \n
*   - 应用场景 \n
*     显示 APS报文收发包数量及debug开关状???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*      ???
* - 使用举例 \n
*     Hios (config)# show elps debug
*     aps recv total 159
*     aps send total 159
*     aps error total 0
*     aps debug packet is disable
*
*/
DEFSH(VTYSH_L2,
	show_elps_debug_cmd_vtysh,
	"show elps debug",
	"Display.\n"
	"Elps management\n"
	"Debug\n")

//---------------elps  session end  ------------------------------//


//---------------erps  session start	------------------------------//

/**
* \page erps_session_cmd erps session

* - 功能说明 \n
*   config 视图下创???erps session

* - 命令格式 \n
*   erps session <1-255>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-255>   |每一个session用一个数字来标识，取值范围是1~255|

* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no erps session <1-255>

* - 使用举例 \n
*     Huahuan(config)#erps session 1 

*
*/

DEFUNSH(VTYSH_L2,
	erps_session,
	erps_session_cmd,
	"erps session <1-255>",
	"Erps management\n"
	"Session\n"
	"Range of sess_id:<1-255>\n")
{
		vty->node = ERPS_SESSION_NODE;
		return CMD_SUCCESS;
}
DEFUNSH (VTYSH_L2,
		vtysh_exit_erps_session,
		vtysh_exit_erps_session_cmd,
		"exit",
		"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}
ALIAS (vtysh_exit_erps_session,
		vtysh_quit_erps_session_cmd,
		"quit",
		"Exit current mode and down to previous mode\n")
		
DEFSH(VTYSH_L2,
	no_erps_session_cmd_vtysh,
	"no erps session <1-255>",
	"Delete\n"
	"Erps management\n"
	"Session\n"
	"Range of sess_id:<1-255>\n")






/**
* \page erps_physical_ring_cmd_vtysh erps physical-ring
* - 功能说明 \n
*   session 视图下配置保护端???
* - 命令格式 \n
*   physical-ring east-interface (ethernet|gigabitethernet|xgigabitethernet) USP west-interface (ethernet|gigabitethernet|xgigabitethernet) USP
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |USP   |东向端口The port???0-7>/<0-31>/<1-255>|
*   |USP   |西向端口The port???0-7>/<0-31>/<1-255>|
* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session的东、西端口
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no erps physical-ring
* - 使用举例 \n
*     Huahuan(config-erps-session)# physical-ring east-interface gigabitethernet 1/1/3 west-interface gigabitethernet 1/1/4 
*
*/	
DEFSH(VTYSH_L2,
	erps_physical_ring_cmd_vtysh,
	"physical-ring east-interface (ethernet|gigabitethernet|xgigabitethernet) USP west-interface (ethernet|gigabitethernet|xgigabitethernet) USP",
	"physical-ring\n"
	"east-interface\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	"west-interface \n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR)

DEFSH(VTYSH_L2,
	erps_physical_subring_cmd_vtysh,
	"physical-subring east-interface (ethernet|gigabitethernet|xgigabitethernet) USP",
	"physical-subring\n"
	"east-interface\n"
	CLI_INTERFACE_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	CLI_INTERFACE_ETHERNET_VHELP_STR
	)

DEFSH(VTYSH_L2,
	no_erps_physical_ring_cmd_vtysh,
	"no physical-ring",


	"Delete\n"
	"physical-ring\n")


DEFSH(VTYSH_L2,
	no_erps_physical_subring_cmd_vtysh,
	"no physical-subring",
	"Delete\n"
	"physical-subring\n")

/**
* \page erps_rpl_role_cmd_vtysh rpl role
* - 功能说明 \n
*   session 视图下配置设备节点角???
* - 命令格式 \n
*   rpl role {{ owner | neighbor }{ east-interface | west-interface} | non-owner }}
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |owner            |owner|
*   |neighbor         |neighbor|
*   |east-interface   |东向端口与RPL连接|
*   |west-interface   |西向端口与RPL连接|
*   |non-owner        |non-owner|
* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session的rpl角色
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no rpl role
* - 使用举例 \n
*     Huahuan(config-erps-session)# rpl role owner east-interface
*
*/		
DEFSH(VTYSH_L2,
	erps_rpl_role_cmd_vtysh,
	"rpl role (owner | neighbor) (east-interface | west-interface)",
	"ring protection link\n"
	"role\n"
	"owner \n"
	"neighbor \n"
	"rpl interface:east-interface \n"
    "rpl interface:west-interface \n")
	
DEFSH(VTYSH_L2,
	erps_rpl_role_nonowner_cmd_vtysh,
	"rpl role non-owner",
	"ring protection link\n"
	"role\n"
	"non-owner \n")
	
DEFSH(VTYSH_L2,
	no_erps_rpl_role_cmd_vtysh,
	"no rpl role",
	"Delete\n"
	"ring protection link\n"
	"role\n")

/**
* \page erps_data_traffic_cmd_vtysh erps physical-ring
* - 功能说明 \n
*   session 视图下配置配置业务VLAN

* - 命令格式 \n
*   data-traffic <1-4094> {to <1-4094> }

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-4094>   |起始VLAN ID，整数形式，取值范???-4094|
*   |<1-4094>   |结束VLAN ID，整数形式，取值范???-4094|

* - 缺省情况 \n
*   缺省情况下，起始vlan id 和结???vlan id 一???

* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session的保护vlan

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no data-traffic <1-4094> {to <1-4094> }
*     no data-traffic all

* - 使用举例 \n
*     Huahuan(config-erps-session)# data-traffic 3
*/		
DEFSH(VTYSH_L2,
	erps_data_traffic_cmd_vtysh,
	"data-traffic <1-4094> {to <1-4094> }",
	"data-traffic\n"
	"Range of data vlan:<1-4094>\n"
	"data-traffic end\n"
	"Range of data vlan:<1-4094>\n")




DEFSH(VTYSH_L2,
	no_erps_data_traffic_cmd_vtysh,
	"no data-traffic <1-4094> {to <1-4094> }",
	"Delete\n"
	"data-traffic\n"
	"Range of data vlan:<1-4094>\n"
	"data-traffic end\n"
	"Range of data vlan:<1-4094>\n")







DEFSH(VTYSH_L2,
	no_erps_data_traffic_all_cmd_vtysh,
	"no data-traffic all",


	"Delete\n"
	"data-traffic\n"
	"all \n")




/**
* \page erps_raps_channel_cmd_vtysh erps raps-channel
* - 功能说明 \n
*   session 视图下配置发送报文的协议 vlan

* - 命令格式 \n
*   raps-channel  <1-4094>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-4094>   |起始VLAN ID，整数形式，取值范???-4094|

* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session协议vlan
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no raps-channel

* - 使用举例 \n
*    Huahuan(config-erps-session)# raps-channel 3

*/			

DEFSH(VTYSH_L2,
	erps_raps_channel_cmd_vtysh,
	"raps-channel <1-4094>",
	"raps-channel\n"
	"Range of primary vlan:<1-4094>\n")






DEFSH(VTYSH_L2,
	no_erps_raps_channel_cmd_vtysh,
	"no raps-channel",


	"Delete\n"
	"raps-channel\n")




/**
* \page erps_backup_create_cmd_vtysh erps backup
* - 功能说明 \n
*   session 视图下配置切换模式和回切时间
* - 命令格式 \n
*   erps backup { non-failback | failback [ wtr <1-12> ] }
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |non-failback   |非返回模式|
*   |failback	    |返回模式|
*   |<1-12>         |Erps session 的wtr时间，单位为分钟（min），默认???min|
* - 缺省情况 \n
*   缺省情况下，erps session 的切换模式为failback时，wtr的时间为5min???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session切换模式和回切时间???
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*    no erps backup
* - 使用举例 \n
*    Huahuan(config-erps-session)# erps backup failback wtr 1

*/  

DEFSH(VTYSH_L2,
	erps_backup_create_cmd_vtysh,
	"backup non-failback ",



	"Protection restoration mode\n"
	"Non-revertive mode\n")


DEFSH(VTYSH_L2,
	erps_backup_failback_cmd_vtysh,
	"backup failback [ wtr <1-12> ]",



	"Protection restoration mode\n"
	"Revertive mode\n"
	"Wait to restore\n"
	"Range of wtr:<1-12>,step:1min\n")



DEFSH(VTYSH_L2,
	no_erps_backup_failback_cmd_vtysh,
	"no backup",


	"Delete\n"

	"Protection restoration mode\n")


/**
* \page erps_hold_off_cmd_vtysh erps hold-off
* - 功能说明 \n
*   session视图下配???hold-off 时间
* - 命令格式 \n
*   erps hold-off < 0-100>
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |< 0-100>   |Erps session 的hold-off时间，单位为100ms，默认???|
* - 缺省情况 \n
*   缺省情况下，erps session的hold-off时间???s???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session的hold-off时间
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*    no erps hold-off
* - 使用举例 \n
*    Huahuan(config-erps-session)# erps hold-off 0 
*/  
DEFSH(VTYSH_L2 ,
	erps_hold_off_cmd_vtysh,
	"hold-off <0-100>",
	"hold-off \n"
	"Range of hold off:<0-100>,step:100ms\n")

DEFSH(VTYSH_L2 ,
	no_erps_hold_off_cmd_vtysh,
	"no hold-off",
	"Delete\n"
	"hold-off \n")

/**
* \page erps_keep_alive_cmd_vtysh erps keep-alive

* - 功能说明 \n
*   session视图下配置报文发送间???

* - 命令格式 \n
*   erps keep-alive <1-600>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-600>   |r-aps报文发送间隔，单位为秒（s），默认???|

* - 缺省情况 \n
*   缺省情况下，r-aps报文发送间隔为5s???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置r-aps报文的发送间???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*    no erps keep-alive

* - 使用举例 \n
*    Huahuan(config-erps-session)# erps keep-alive 10
*/  



DEFSH(VTYSH_L2,
	erps_keep_alive_cmd_vtysh,
	"keep-alive <1-600>",
	"Erps keep alive\n"
	"Range of keep alive:<1s-600s>\n")






DEFSH(VTYSH_L2,
	no_erps_keep_alive_cmd_vtysh,
	"no keep-alive",


	"Delete\n"
	"Erps keep alivet\n")




/**
* \page erps_guard_timer_cmd_vtysh erps guard-time
* - 功能说明 \n
*   session 视图下配置guard-time时间???
* - 命令格式 \n
*   guard-timer <1-200>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-200>   |Erps session 的guard-time时间，单位为10ms，默认???00ms|



* - 缺省情况 \n
*   缺省情况下，erps session的guard-time时间???00ms???

* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session的guard-time时间???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*    no guard-timer

* - 使用举例 \n
*    Huahuan(config-erps-session)# guard-timer 50  

*/  

DEFSH(VTYSH_L2 ,
	erps_guard_timer_cmd_vtysh,
	"guard-timer <1-200>",
	"guard-timer \n"
	"Range of guard timer:<1-200>,step:10ms\n")











DEFSH(VTYSH_L2 ,
	no_erps_guard_timer_cmd_vtysh,
	"no guard-timer",



	"Delete\n"
	"guard-timer \n")








/**
* \page erps_ring_id_cmd_vtysh erps ring-id
* - 功能说明 \n
*   session视图下配置物理环ID

* - 命令格式 \n
*   ring-id <1-255>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-255>   |物理环ID，整数形式，取值范???~255|


* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置物理环ID

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config-erps-session)# ring-id 1 

*/  

DEFSH(VTYSH_L2 ,
	erps_ring_id_cmd_vtysh,
	"ring-id <1-255>",
	"ring id\n"
	"Range of ring id :<1-255>\n")







/**
* \page erps_level_cmd_vtysh erps level
* - 功能说明 \n
*   session视图下配置维护实体组的等???

* - 命令格式 \n
*   level <0-7>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<0-7>   |维护实体组等级，整数形式，取值范??? ~ 7|
* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置维护实体组的等级
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config-erps-session)# level 1 
*/ 

DEFSH(VTYSH_L2 ,
	erps_level_cmd_vtysh,
	"level <0-7>",
	"level\n"
	"Range of level:<0-7>\n")


/**
* \page erps_bind_cfm_session_cmd_vtysh erps bind cfm session
* - 功能说明 \n
*   session 视图下配???与ERPS绑定的CFM 会话的session ID
* - 命令格式 \n
*   bind cfm session <1-8192>{master |backup}
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-8192>        |cfm session 的session ID|
*   |east-interface  |CFM会话绑定在东向端口上|
*   |west-interface  |CFM会话绑定在西向端口上|
* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   erps session模式

* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置与erps session绑定的CFM会话的session ID

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no bind cfm session (east-interface | west-interface)

* - 使用举例 \n
*    Huahuan(config-erps-session)# bind cfm session 1 east-inteface 


*/ 

DEFSH(VTYSH_L2,
	erps_bind_cfm_session_cmd_vtysh,
	"bind cfm session <1-1024> (east-interface | west-interface)",
	"Bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"Session index ,<1-1024>\n"
	"East-interface\n"
	"West-interface\n")






DEFSH(VTYSH_L2,
	no_erps_bind_cfm_session_cmd_vtysh,
	"no bind cfm session (east-interface | west-interface)",


	"Delete\n"
	"Bind\n"
	"Connectivity fault management\n"
	"Session\n"
	"East-interface\n"
	"West-interface\n")





/**
* \page erps_virtual_channel_cmd_vtysh erps virtual-channel

* - 功能说明 \n
*   session 视图下绑定子环和主环并配置子环的虚拟通道

* - 命令格式 \n
*   virtual-channel <1-4094> attached-to-session <1-255>

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-4094>     |虚拟通道的协议VLAN|
*   |<1-255>      |主环的ID|
* - 缺省情况 \n
*   ???


* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置绑定子环和主环并配置子环的虚拟通道

*   - 前置条件 \n
*     需先配置主环和子环的ERPS保护实体并且需先配置子环block端口，否则命令失???

*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no virtual-channel

* - 使用举例 \n
*    Huahuan(config-erps-session)# virtual-channel 3 attached-to-session 1 


*/

DEFSH(VTYSH_L2,
	erps_virtual_channel_cmd_vtysh,
	"sub-ring attached-to-session <1-255>",
	"subring\n"
	"attached to master ring session id\n"
	"rang of session :<1-255>\n")




/**
* \page erps_sub_ring_block_cmd_vtysh erps sub-ring block

* - 功能说明 \n
*   session 视图下绑定子环和主环并配置子环的虚拟通道

* - 命令格式 \n
*   sub-ring block (east-interface | west-interface)

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |east-interface    |公共端口是东向端口|
*   |west-interface    |公共端口是西向端口|
* - 缺省情况 \n
*   ???


* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     配置互联节点上主环和子环的公共端???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no sub-ring

* - 使用举例 \n
*    Huahuan(config-erps-session)# sub-ring block west-interface 


*/


DEFSH(VTYSH_L2,
	no_erps_sub_ring_block_cmd_vtysh,
	"no sub-ring",


	"Delete\n"
	"Sub-ring\n")



/**
* \page erps_session_enable_cmd_vtysh erps (enable|disable)
* - 功能说明 \n
*   session 视图下配置会话使???
* - 命令格式 \n
*   erps (enable | disable)
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |enable    |Erps session 使能|
*   |disable   |Erps session 无效|
* - 缺省情况 \n
*   ???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session 使能/无效
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config-erps-session)# erps enable 
*/



DEFSH(VTYSH_L2,
	erps_session_enable_cmd_vtysh,
	"erps (enable | disable)",
	"Erps management\n"
	"Enable\n"
	"Disable\n")

/**
* \page erps_debug_packet_cmd_vtysh debug erps packet
* - 功能说明 \n
*   config 视图下显???erps 报文的信???

* - 命令格式 \n
*   debug erps packet

* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     显示 erps 报文的信???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     no debug erps packet

* - 使用举例 \n
*    Huahuan(config)# debug erps packet


*/

DEFSH(VTYSH_L2,
	erps_debug_packet_cmd_vtysh,
	"debug erps packet",
	"DEBUG_STR"
	"Erps management\n"
	"Erps packet\n" )





DEFSH(VTYSH_L2,
	no_erps_debug_packet_cmd_vtysh,
	"no debug erps packet",


	"Delete\n"
	"DEBUG_STR"
	"Erps management\n"
	"Erps packet\n" )
/**
* \page erps_admin_force_cmd_vtysh erps admin switch
* - 功能说明 \n
*   session 视图下配置手???强制倒换
* - 命令格式 \n
*   (force-switch|manual-switch) (east-interface | west-interface)
* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |force-switch   |强制倒换|
*   |manual-switch  |手动倒换|
*   |east-interface	|倒换block的端口是东向端口|
*   |west-interface	|倒换block的端口是西向端口|
 * - 缺省情况 \n
*   ???
* - 命令模式 \n
*   erps session模式
* - 用户等级 \n
*   5
* - 使用指南 \n
*   - 应用场景 \n
*     配置erps session 使能/无效
*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config-erps-session)# erps manual-switch east-interface
*/
DEFSH(VTYSH_L2,
	erps_admin_force_cmd_vtysh,
	"(force-switch|manual-switch) (east-interface | west-interface)",
    "Force switch\n"
    "Manual switch\n"
    "East interface will be block\n"
    "West interface will be block\n")



/**
* \page erps_admin_clear_cmd_vtysh erps clear
* - 功能说明 \n
*   session 视图下清除强制或手动block指定端口的配???

* - 命令格式 \n
*   clear

* - 参数说明 \n
*   ???
* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   erps session模式

* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     清除强制或手动block指定端口的配置???

*   - 前置条件 \n
*     需使用force-switch或manual-switch强制或手动block指定端口???

*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config-erps-session)# clear














*/


DEFSH(VTYSH_L2,
	erps_admin_clear_cmd_vtysh,
	"clear",
	"Clear\n")






/**
* \page show_erps_config_cmd_vtysh show erps config
* - 功能说明 \n
*   config 视图下显???erps的配???
* - 命令格式 \n
*   show erps config

* - 参数说明 \n
*   ???



* - 缺省情况 \n
*   ???

* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     显示 erps的配???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    Huahuan(config)# show erps config
*  Num:2
*       session-id        : 2
*       session-status    : enable
*       session-role      : owner
*       east_interface    : gigabitethernet 1/1/5
*       east_interface    : --
*       rpl_interface     : east interface
*       data-traffic      : 2, 4, 101 - 150 
*       raps-channel      : 2












*       backup-mode       : Failback
*       WTR               : 1 min
*       hold-off          : 10000ms
*       guard-timer       : 500ms
*       WTB               : 5500ms
*       keep-alive        : 5s

*       priority          : 6
*       ring-id           : 2
*       level             : 2
*       east cfm session  : --
*       west cfm session  : --
*       attach to session : 1
*       virtual vlan      : 3


*/

DEFSH(VTYSH_L2,
	show_erps_config_cmd_vtysh,
	"show erps config",


	"Display.\n"
	"Erps management\n"
	"Config\n")





/**
* \page show_erps_session_cmd_vtysh show erps session
* - 功能说明 \n
*   config 视图下显???erps session的信???
* - 命令格式 \n
*   show erps session [<1-255>] 

* - 参数说明 \n
*   |参数   |说明          |
*   |------|------------ |
*   |<1-255>    |每一个session用一个数字来标识，取值范围是1~255|

* - 缺省情况 \n
*   缺省情况下，显示所有配置的session信息 

* - 命令模式 \n
*   config模式
* - 用户等级 \n
*   5

* - 使用指南 \n
*   - 应用场景 \n
*     显示 erps session的信???

*   - 前置条件 \n
*     ???
*   - 后续任务 \n
*     ???
*   - 注意事项 \n
*     ???
*   - 相关命令 \n
*     ???
* - 使用举例 \n
*    
*   Huahuan(config)# show erps session
*        Total             : 1
*        enable_num       : 1
*     Num :1
*    id   role       east_interface         west_interface         current-state     
*    ----------------------------------------------------------------------------
*    2    owner      gigabitethernet 1/1/5  --                     Idle
*        session-status    : enable
*        rpl_interface     : east interface
*        block_interface   : east interface
*        data-traffic      : 2, 4, 101 - 150 
*        raps-channel      : 2
*        backup-mode       : Failback
*        WTR               : 1 min
*        hold-off          : 10000ms
*        guard-timer       : 500ms
*        WTB               : 5500ms
*        keep-alive        : 5s
*        priority          : 6
*        ring-id           : 2
*        level             : 2
*        east cfm session  : --
*        west cfm session  : --
*        attach to session : 1
*       virtual vlan      : 3





*/

DEFSH(VTYSH_L2,
	show_erps_session_cmd_vtysh,
	"show erps session [<1-255>]",


	"Display.\n"
	"Erlps management\n"
	"Session\n"
	"Range of session id:<1-255>\n")

//---------------erps  session end  ------------------------------//
      
DEFSH(VTYSH_L2,
		cfm_enable_interface_trunk_cmd_vtysh,
		"cfm enable  interface trunk TRUNK",
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)

DEFSH(VTYSH_L2,
		no_cfm_enable_interface_trunk_cmd_vtysh,
		"no cfm enable  interface trunk TRUNK",
		NO_STR
		"Connectivity fault management\n"
		"Enable\n"	
		CLI_INTERFACE_STR
		CLI_INTERFACE_TRUNK_STR
		CLI_INTERFACE_TRUNK_VHELP_STR)


DEFSH(VTYSH_L2,
	reserve_vlan_set_cmd_vtysh,
	"reserve-vlan <1-4094>",
	"Reserve vlan:some vlan in used by sys\n"
	"Reserve vlan value <1-4094>\n")

DEFSH(VTYSH_L2,
	no_reserve_vlan_set_cmd_vtysh,
	"no reserve-vlan",
	NO_STR
	"Reserve vlan:some vlan in used by sys\n")

DEFSH(VTYSH_L2,
	reserve_vlan_show_cmd_vtysh,
	"show interface reserve-vlan",
	SHOW_STR
	CLI_INTERFACE_STR
	"Reserve vlan:some vlan in used by sys\n")

/*************************port isolate cmd***************/
DEFSH(VTYSH_L2,
		port_isolate_cmd_vtysh,
		"port-isolation (group| vlan) ID {root}",
		"Port isolate config\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group id or vlan id: <1-4094>\n"
		"Port role\n")

DEFSH(VTYSH_L2,
		no_port_isolate_cmd_vtysh,
		"no port-isolation (group| vlan) ID",
		NO_STR
		"port isolate config\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group id or vlan id: <1-4094>\n")

DEFSH(VTYSH_L2,
		show_port_isolate_cmd_vtysh,
		"show port-isolation (group| vlan) [ID]",
		SHOW_STR
		"port isolation\n"
		"Port isolation group\n"
		"Port isolation vlan\n"
		"Port isolation group number or vlan id\n")

DEFSH(VTYSH_L2,
		show_port_isolate_interface_cmd_vtysh,
		"show port-isolation interface (ethernet| gigabitethernet| xgigabitethernet) USP",
		SHOW_STR
		"port isolation\n"
		"Port isolation interface\n"
		CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of interface, format:0-7>/<0-31>/<1-255>\n")

	
void
vtysh_init_l2_cmd ()
{
 	install_node (&vlan_node, NULL);
	vtysh_install_default (VLAN_NODE);
	install_element (VLAN_NODE, &vtysh_exit_vlan_cmd, CMD_SYNC);

	/*vlan*/	
	install_element_level (CONFIG_NODE, &vlan_range_set_cmd_vtysh,VISIT_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_vlan_range_set_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

	
	install_element_level (CONFIG_NODE, &vlan_get_cmd,VISIT_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_vlan_get_range_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &vlan_get_range_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (CONFIG_NODE, &show_vlan_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (CONFIG_NODE, &show_vlan_summary_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);	
	
		
	install_element_level (VLAN_NODE, &vlan_name_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLAN_NODE, &vlan_mac_learn_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLAN_NODE, &vlan_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (VLAN_NODE, &no_vlan_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (VLAN_NODE, &no_vlan_storm_supress_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLAN_NODE, &vlan_storm_supress_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (VLAN_NODE, &no_vlan_name_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	/*l2if  vlan */
	install_element_level (PHYSICAL_IF_NODE, &show_switch_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (PHYSICAL_IF_NODE, &no_switch_access_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &switch_access_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_switch_vlan_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &switch_vlan_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (PHYSICAL_IF_NODE, &switch_vlan_tag_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (PHYSICAL_IF_NODE, &l2_mode_switch_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &l2if_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (PHYSICAL_IF_NODE, &no_l2if_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &l2if_storm_supress_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (PHYSICAL_IF_NODE, &no_l2if_storm_supress_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    /*l2cp*/
	install_element_level (PHYSICAL_IF_NODE, &l2cp_mpls_func_cmd,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &l2cp_tunnel_func_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &no_l2cp_tunnel_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &l2cp_config_show_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (PHYSICAL_IF_NODE, &l2cp_tunnel_func2_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &l2cp_discard_func_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	/*trunk  vlan*/
	install_element_level (TRUNK_IF_NODE, &show_switch_interface_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (TRUNK_IF_NODE, &l2_mode_switch_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &no_switch_access_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (TRUNK_IF_NODE, &switch_access_vlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	
	install_element_level (TRUNK_IF_NODE, &no_switch_vlan_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &switch_vlan_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (TRUNK_IF_NODE, &switch_vlan_tag_add_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);	

	/* l2if  qinq */
	install_element_level (PHYSICAL_IF_NODE, &no_dot1q_tunnel_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &dot1q_tunnel_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_vlan_mapping_qinq_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_qinq_cvlans_delete_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlans_to_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_1_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_2_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_qinq_scvlan_to_scvlan_cmd_3_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_vlan_mapping_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_svlan_to_scvlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_add_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &vlan_mapping_svlans_to_svlan_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);

	/*loopd*/
	install_element_level (PHYSICAL_IF_NODE, &loopdetect_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_loopdetect_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &show_loopdetect_interface_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &loopdetect_pkt_debug_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &loopdetect_pkt_debug_disable_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_loopdetect_cmd_vtysh,CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_loopdetect_gigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_loopdetect_xgigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5, CMD_LOCAL);

/**********mstp module cmd start********************************************/
#if 0
	/*mstp whole config */
	install_node (&mstp_node, NULL);
	vtysh_install_default (MSTP_NODE);
	install_element_level (MSTP_NODE, &vtysh_exit_stp_instance_cmd, CONFIG_LEVE_5);	
	install_element_level (MSTP_NODE, &vtysh_quit_stp_instance_cmd, CONFIG_LEVE_5);	

	/*region config*/
	install_element_level (CONFIG_NODE, &stp_region_name_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (CONFIG_NODE, &no_stp_region_name_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (CONFIG_NODE, &stp_revision_level_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (CONFIG_NODE, &no_stp_revision_level_cmd_vtysh, CONFIG_LEVE_5);

	/*instance	mode create*/
	install_element_level (CONFIG_NODE, &stp_instance_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (CONFIG_NODE, &no_stp_instance_cmd_vtysh, CONFIG_LEVE_5);

	/*instance config*/
	install_element_level (MSTP_NODE, &stp_vlan_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_vlan_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (MSTP_NODE, &stp_priority_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_priority_cmd_vtysh, CONFIG_LEVE_5);	

	install_element_level (MSTP_NODE, &stp_root_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_root_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (MSTP_NODE, &stp_hello_time_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_hello_time_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (MSTP_NODE, &stp_forward_delay_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_forward_delay_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (MSTP_NODE, &stp_max_age_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_max_age_cmd_vtysh, CONFIG_LEVE_5);
	
	install_element_level (MSTP_NODE, &stp_max_hop_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_max_hop_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (MSTP_NODE, &stp_time_out_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (MSTP_NODE, &no_stp_time_out_cmd_vtysh, CONFIG_LEVE_5);

/*physical interface node cmd*/
	install_element_level (PHYSICAL_IF_NODE, &interface_stp_enable_instance_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_enable_instance_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (PHYSICAL_IF_NODE, &interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5); 

	install_element_level (PHYSICAL_IF_NODE, &interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5); 

	install_element_level (PHYSICAL_IF_NODE, &interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5); 

	install_element_level (PHYSICAL_IF_NODE, &interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5);		

	install_element_level (PHYSICAL_IF_NODE, &interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5);		

	/*trunk node cmd*/
	install_element_level (TRUNK_IF_NODE, &interface_stp_enable_instance_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_enable_instance_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (TRUNK_IF_NODE, &interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5);	

	install_element_level (TRUNK_IF_NODE, &interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5);	

	install_element_level (TRUNK_IF_NODE, &interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5);	

	install_element_level (TRUNK_IF_NODE, &interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5);

	install_element_level (TRUNK_IF_NODE, &interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level (TRUNK_IF_NODE, &no_interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5);

/*show cmd*/
	install_element_level (CONFIG_NODE, &show_stp_interface_trunk_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_stp_interface_ethernet_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (CONFIG_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2);	

	install_element_level (MSTP_NODE, &show_stp_interface_trunk_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (MSTP_NODE, &show_stp_interface_ethernet_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (MSTP_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2);

	install_element_level (PHYSICAL_IF_NODE, &show_stp_interface_trunk_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_stp_interface_ethernet_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (PHYSICAL_IF_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2);	

	install_element_level (TRUNK_IF_NODE, &show_stp_interface_trunk_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_stp_interface_ethernet_cmd_vtysh, MONITOR_LEVE_2);
	install_element_level (TRUNK_IF_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2); 
#endif
#if 0
	/*debug cmd*/
	install_element_level(TRUNK_IF_NODE, &debug_stp_cmd_vtysh, MANAGE_LEVE);	
	install_element_level (PHYSICAL_IF_NODE, &debug_stp_cmd_vtysh, MANAGE_LEVE); 	
	install_element_level (MSTP_NODE, &debug_stp_cmd_vtysh, MANAGE_LEVE);
	install_element_level (CONFIG_NODE, &debug_stp_cmd_vtysh, MANAGE_LEVE);			
	install_element_level (TRUNK_IF_NODE, &no_debug_stp_cmd_vtysh, MANAGE_LEVE); 
	install_element_level (PHYSICAL_IF_NODE, &no_debug_stp_cmd_vtysh, MANAGE_LEVE);		
	install_element_level (MSTP_NODE, &no_debug_stp_cmd_vtysh, MANAGE_LEVE);
	install_element_level (CONFIG_NODE, &no_debug_stp_cmd_vtysh, MANAGE_LEVE);	
#endif

/**********mstp module cmd end********************************************/
	
#if 0
	install_element_level(PHYSICAL_IF_NODE,&interface_tlv_enable_dot1_tlv_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level(PHYSICAL_IF_NODE,&no_interface_tlv_enable_dot1_tlv_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level(PHYSICAL_IF_NODE,&interface_tlv_enable_dot3_tlv_cmd_vtysh, CONFIG_LEVE_5);
	install_element_level(PHYSICAL_IF_NODE,&no_interface_tlv_enable_dot3_tlv_cmd_vtysh, CONFIG_LEVE_5);
#endif

    /*trunk子模??*/
    install_element_level ( TRUNK_IF_NODE, &trunk_backup_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &trunk_backup_wtr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &no_trunk_backup_wtr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &trunk_load_balance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &no_trunk_load_balance_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &trunk_lacp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &no_trunk_lacp_enable_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &trunk_lacp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &no_trunk_lacp_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &trunk_lacp_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &no_trunk_lacp_priority_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC );
    install_element_level ( TRUNK_IF_NODE, &show_trunk_lacp_statistics_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL );
    install_element_daemon_order_level( PHYSICAL_IF_NODE, &trunk_member_port_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_L2, VTYSH_IFM );
    install_element_daemon_order_level( PHYSICAL_IF_NODE, &no_trunk_member_port_set_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 2, VTYSH_L2, VTYSH_IFM );

	install_node (&trunk_gloable_node, NULL);  
 	vtysh_install_default (TRUNK_GLOABLE_NODE);
   install_element_level( CONFIG_NODE, &gloable_trunk_load_balance_cmd_vtysh, MANAGE_LEVE,CMD_LOCAL );
   install_element_level( CONFIG_NODE, &no_gloable_trunk_load_balance_cmd_vtysh, MANAGE_LEVE ,CMD_LOCAL);
   install_element_level(CONFIG_NODE,&gloable_trunk_link_aggregation_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL);
   install_element_level(CONFIG_NODE,&no_gloable_trunk_link_aggregation_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &debug_trunk_lacp_packet_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &no_debug_trunk_lacp_packet_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL );

	//install_element_level ( CONFIG_NODE, &l2_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL );
	//install_element_level ( CONFIG_NODE, &l2_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL );
	
	/*static mac */
    install_node (&mac_node, NULL);
    vtysh_install_default (MAC_NODE);
    install_element_level(CONFIG_NODE, &mac_mode_cmd_vtysh,VISIT_LEVE, CMD_SYNC);
    install_element_level(MAC_NODE, &vtysh_exit_mac_cmd_vtysh,VISIT_LEVE, CMD_SYNC);
    install_element_level(MAC_NODE, &configure_ethernet_static_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MAC_NODE, &configure_gigabit_ethernet_static_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(MAC_NODE, &configure_xgigabit_ethernet_static_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MAC_NODE, &configure_trunk_static_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);          
    install_element_level(MAC_NODE, &undo_configure_static_mac_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(MAC_NODE, &configure_mac_ageing_time_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);   
    install_element_level(MAC_NODE, &no_configure_mac_ageing_time_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);     
    install_element_level(MAC_NODE, &config_mac_move_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);  
    install_element_level(MAC_NODE, &mac_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);  
    install_element_level(MAC_NODE, &no_mac_mac_limit_cmd_vtysh,CONFIG_LEVE_5, CMD_SYNC);
	
    install_element_daemon_order_level(CONFIG_NODE, &show_mac_static_all_cmd_vtysh,
                                    MONITOR_LEVE_2, CMD_LOCAL, 2, VTYSH_L2, VTYSH_MPLS);
    install_element_level(CONFIG_NODE, &show_mac_static_mac_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL); 
    install_element_level(CONFIG_NODE, &show_mac_static_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_mac_static_gigabit_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_mac_static_xgigabit_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_mac_static_trunk_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);   
    install_element_level(CONFIG_NODE, &show_mac_static_vlan_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);  
    install_element_level(CONFIG_NODE, &show_mac_static_blackhole_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);       
    install_element_level(CONFIG_NODE, &show_mac_config_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);  

//---------------------------cfm------------------------------------//

	install_node (&cfm_md_node, NULL);
	vtysh_install_default (CFM_MD_NODE);
	install_element_level (CFM_MD_NODE, &vtysh_exit_md_cmd,VISIT_LEVE,CMD_SYNC);
	install_element_level (CONFIG_NODE, &md_view_cmd,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CONFIG_NODE, &no_md_view_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_node (&cfm_ma_node, NULL);
	vtysh_install_default (CFM_MA_NODE);
	install_element_level (CFM_MA_NODE, &vtysh_exit_ma_cmd,VISIT_LEVE,CMD_SYNC);
	install_element_level (CFM_MD_NODE, &ma_view_cmd,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CFM_MD_NODE, &no_ma_view_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_node (&cfm_session_node, NULL);
	vtysh_install_default (CFM_SESSION_NODE);
	install_element_level (CFM_SESSION_NODE, &vtysh_exit_cfmsess_cmd,VISIT_LEVE,CMD_SYNC);
	install_element_level (CONFIG_NODE, &session_name_cmd,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CONFIG_NODE, &no_session_name_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	
	install_element_level (CFM_MD_NODE, &md_name_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MD_NODE, &md_level_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MD_NODE, &no_md_level_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	
	install_element_level (CFM_MA_NODE, &ma_name_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CFM_MA_NODE, &no_ma_name_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);		
	install_element_level (CFM_MA_NODE, &ma_service_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &no_ma_service_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &ma_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &no_ma_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &ma_mip_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &no_ma_mip_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &ma_1731_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_MA_NODE, &no_ma_1731_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CFM_SESSION_NODE, &session_service_ma_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &no_session_service_ma_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CFM_SESSION_NODE, &session_direction_down_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);	
	install_element_level (CFM_SESSION_NODE, &session_direction_up_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &no_session_direction_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &session_localmep_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &no_session_localmep_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &session_rmep_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &no_session_rmep_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &session_ccm_en_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &no_session_ccm_en_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    
    /* cfm lb/lt */
    install_element_level (CFM_SESSION_NODE, &cfm_lb_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (CFM_SESSION_NODE, &no_cfm_lb_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
	
    install_element_level (CFM_SESSION_NODE, &cfm_lt_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (CFM_SESSION_NODE, &no_cfm_lt_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);

    /*cfm lm/dm */
    install_element_level (CFM_SESSION_NODE, &cfm_lm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (CFM_SESSION_NODE, &no_cfm_lm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (CFM_SESSION_NODE, &cfm_dm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    install_element_level (CFM_SESSION_NODE, &no_cfm_dm_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
    
	install_element_level (CONFIG_NODE, &show_cfm_md_cmd_vtysh,CONFIG_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_cfm_ma_cmd_vtysh,CONFIG_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_cfm_session_cmd_vtysh,CONFIG_LEVE_2,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_cfm_lm_dm_result_cmd_vtysh,CONFIG_LEVE_2,CMD_LOCAL);

	install_element_level (CFM_SESSION_NODE, &cfm_enable_interface_gigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CFM_SESSION_NODE, &no_cfm_enable_interface_gigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &cfm_enable_interface_xgigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CFM_SESSION_NODE, &no_cfm_enable_interface_xgigabit_ethernet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CFM_SESSION_NODE, &cfm_enable_interface_trunk_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CFM_SESSION_NODE, &no_cfm_enable_interface_trunk_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CFM_SESSION_NODE, &cfm_ais_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CFM_SESSION_NODE, &no_cfm_ais_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);

	install_element_level (CFM_SESSION_NODE, &cfm_tst_mode_in_service_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CFM_SESSION_NODE, &cfm_tst_mode_out_service_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CFM_SESSION_NODE, &cfm_test_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (CFM_SESSION_NODE, &no_cfm_test_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 

	install_element_level (CONFIG_NODE, &cfm_debug_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_cfm_debug_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &l2_debug_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_l2_debug_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_l2_timerinfo_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	
//---------------------------elps------------------------------------//

	install_node (&elps_session_node, NULL); 
	vtysh_install_default (ELPS_SESSION_NODE); 
	install_element_level (ELPS_SESSION_NODE, &vtysh_exit_elps_session_cmd,VISIT_LEVE,CMD_SYNC);
		
	install_element_level (CONFIG_NODE, &elps_session_cmd,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CONFIG_NODE, &no_elps_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (CONFIG_NODE, &elps_debug_packet_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &no_elps_debug_packet_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL); 
	
	//show commands
	install_element_level (CONFIG_NODE, &show_elps_config_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_elps_session_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL); 
	install_element_level (CONFIG_NODE, &show_elps_debug_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL); 
	
	install_element_level (ELPS_SESSION_NODE, &elps_protect_port_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);  
	
	install_element_level (ELPS_SESSION_NODE, &no_elps_protect_port_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_data_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_data_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &no_elps_data_vlan_all_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_primary_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_primary_vlan_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_keep_alive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_keep_alive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_hold_off_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_hold_off_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_backup_create_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &elps_backup_failback_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_backup_create_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_priority_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ELPS_SESSION_NODE, &elps_bind_cfm_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &no_elps_bind_cfm_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ELPS_SESSION_NODE, &elps_session_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 

	//admin commands
	install_element_level (ELPS_SESSION_NODE, &elps_admin_force_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (ELPS_SESSION_NODE, &no_elps_admin_force_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (ELPS_SESSION_NODE, &elps_admin_manual_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (ELPS_SESSION_NODE, &no_elps_admin_manual_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (ELPS_SESSION_NODE, &elps_admin_lockout_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
	install_element_level (ELPS_SESSION_NODE, &no_elps_admin_lockout_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 

    install_node (&erps_session_node, NULL); 
    vtysh_install_default (ERPS_SESSION_NODE); 
    install_element_level (ERPS_SESSION_NODE, &vtysh_exit_erps_session_cmd,VISIT_LEVE,CMD_SYNC);

    install_element_level (CONFIG_NODE, &erps_session_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (CONFIG_NODE, &no_erps_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (CONFIG_NODE, &erps_debug_packet_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL);
    install_element_level (CONFIG_NODE, &no_erps_debug_packet_cmd_vtysh,MANAGE_LEVE,CMD_LOCAL); 

    install_element_level (ERPS_SESSION_NODE, &erps_physical_ring_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
	install_element_level (ERPS_SESSION_NODE, &erps_physical_subring_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_physical_ring_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (ERPS_SESSION_NODE, &no_erps_physical_subring_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_rpl_role_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_rpl_role_nonowner_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_rpl_role_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_data_traffic_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_data_traffic_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);   
    install_element_level (ERPS_SESSION_NODE, &no_erps_data_traffic_all_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_raps_channel_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_raps_channel_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_backup_failback_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_backup_create_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_backup_failback_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_hold_off_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_hold_off_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_keep_alive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (ERPS_SESSION_NODE, &no_erps_keep_alive_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_guard_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_guard_timer_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_ring_id_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_level_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_bind_cfm_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_bind_cfm_session_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_virtual_channel_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &no_erps_sub_ring_block_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    install_element_level (ERPS_SESSION_NODE, &erps_session_enable_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC); 
    
    install_element_level (ERPS_SESSION_NODE, &erps_admin_force_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
    install_element_level (ERPS_SESSION_NODE, &erps_admin_clear_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL); 
    
    install_element_level (CONFIG_NODE, &show_erps_config_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL); 
    install_element_level (CONFIG_NODE, &show_erps_session_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL); 

	devm_comm_get_id(1, 0, 3, &vtysh_devtype );
	if( vtysh_devtype == ID_HT157 || vtysh_devtype == ID_HT158 )
	{
		install_element_level (PHYSICAL_IF_NODE, &reserve_vlan_set_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (PHYSICAL_IF_NODE, &no_reserve_vlan_set_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
		install_element_level (CONFIG_NODE, &reserve_vlan_show_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);	
	}


	/*port isolate command install*/
	install_element_level (PHYSICAL_IF_NODE, &port_isolate_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &no_port_isolate_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_port_isolate_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);	
	install_element_level (CONFIG_NODE, &show_port_isolate_interface_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);	
}


