/**
 * \page cmds_ref_ftm FTM
 * - \subpage modify_log_ftm
 *
 */

/**
 * \page modify_log_ftm Modify Log
 * \section ftm-v007r004 HiOS-V007R004
 *  -#
 * \section ftm-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

static struct cmd_node tcp_node =
{
	TCP_NODE,
	"%s(config-tcp)#"
};

DEFSH(VTYSH_FTM, show_ftm_fib_all_cmd,
        "show ftm fib",
        SHOW_STR
        "Ftm information\n"
        "Forwarding information base\n")

DEFSH(VTYSH_FTM, show_ftm_fib_cmd,
        "show ftm fib ip (A.B.C.D | A.B.C.D/<0-32>) {vpn <1-1024>}",
        SHOW_STR
        "Ftm information\n"
        "Forwarding information base\n"
        IP_STR
        "IP address\n"
        "IP address and Length of ip address mask\n"
        "Vpn information\n"
        "Vpn value\n")

DEFSH(VTYSH_FTM, show_ftm_fibv6_all_cmd,
    "show ftm ipv6 fib",
    SHOW_STR
    "Ftm information\n"
	"IPV6 configuration informaton\n"
    "Forwarding information base\n")

DEFSH(VTYSH_FTM, show_ftm_fibv6_cmd,
    "show ftm ipv6 fib (X:X:X:X:X:X:X:X | X:X:X:X:X:X:X:X/<0-128>) {vpn <1-1024>}",
    SHOW_STR
    "Ftm information\n"
	"IPV6 configuration information\n"
    "Forwarding information base\n"
    "IPV6 address\n"
    "IPV6 address and Length of ip address mask\n"
    "Vpn information\n"
    "Vpn value\n")

#if 0
DEFSH(VTYSH_FTM, show_ftm_nhp_cmd,
        "show ftm nhp index <1-8192>",
        SHOW_STR
        "Ftm information\n"
        "Nhp information\n"
        "Index\n"
        "<1-8192>\n")

DEFSH(VTYSH_FTM, show_ftm_ecmp_cmd,
        "show ftm ecmp index <1-8192>",
        SHOW_STR
        "Ftm information\n"
        "Ecmp information\n"
        "Index\n"
        "<1-8192>\n")
#endif

DEFSH(VTYSH_FTM, show_ftm_cmd_vtysh,
        "show ftm interface",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR )


DEFSH(VTYSH_FTM, show_ftm_ethernet_cmd_vtysh,
        "show ftm interface ethernet {USP}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_gigabit_ethernet_cmd_vtysh,
		"show ftm interface gigabitethernet {USP}",
		SHOW_STR
		"Show command\n"
		CLI_INTERFACE_STR
		CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_xgigabit_ethernet_cmd_vtysh,
        "show ftm interface xgigabitethernet {USP}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR )


DEFSH(VTYSH_FTM, show_ftm_tdm_cmd_vtysh,
        "show ftm interface tdm {USP}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_stm_cmd_vtysh,
        "show ftm interface stm {USP}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_vlanif_cmd_vtysh,
        "show ftm interface vlanif {<1-4094>}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_trunk_cmd_vtysh,
        "show ftm interface trunk {TRUNK}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_loopback_cmd_vtysh,
        "show ftm interface loopback {<0-128>}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR )


DEFSH(VTYSH_FTM, show_ftm_tunnel_cmd_vtysh,
        "show ftm interface tunnel {USP}",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )

DEFSH(VTYSH_FTM, show_ftm_clock_cmd_vtysh,
        "show ftm interface clock [<1-2>]",
        SHOW_STR
        "Show command\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )


DEFSH (VTYSH_FTM, debug_ftm_packet_cmd,
       "debug ftm packet (receive|send) (arp|ip|eth|udp|tcp|mpls|all)",
	   "Debug\n"
	   "Ftm\n"
	   "packet\n"
	   "receive\n"
	   "send\n"
	   "arp\n"
	   "ip\n"
	   "eth\n"
	   "udp\n"
	   "tcp\n"
	   "mpls\n"
	   "all\n")

DEFSH (VTYSH_FTM, no_debug_ftm_packet_cmd,
       "no debug ftm packet (receive|send) (arp|ip|eth|udp|tcp|mpls|all)",
       "Delete command\n"
       "Debug\n"
	   "Ftm\n"
	   "packet\n"
	   "receive\n"
	   "send\n"
	   "arp\n"
	   "ip\n"
	   "eth\n"
	   "udp\n"
	   "tcp\n"
	   "mpls\n"
	   "all\n")


DEFSH (VTYSH_FTM, show_ftm_packet_stat_cmd,
       "show ftm packet statistics",
	   "show\n"
	   "Ftm\n"
	   "packet\n"
	   "statistics\n")

DEFSH (VTYSH_FTM, clear_ftm_packet_stat_cmd,
       "clear ftm packet statistics",
	   "clear\n"
	   "Ftm\n"
	   "packet\n"
	   "statistics\n")

DEFSH (VTYSH_FTM, show_tcp_session_cmd,
       "show tcp session",
	   "show\n"
	   "tcp\n"
	   "session\n")

DEFSH (VTYSH_FTM, show_tcp_session_v6_cmd,
       "show tcp session ipv6",
	   "show\n"
	   "tcp\n"
	   "session\n"
	   "ipv6 session\n")

DEFSH (VTYSH_FTM, show_udp_session_cmd,
       "show udp session",
	   "show\n"
	   "udp\n"
	   "session\n")

DEFSH (VTYSH_FTM, show_udp_session_v6_cmd,
       "show udp session ipv6",
	   "show\n"
	   "udp\n"
	   "session\n"
	   "ipv6 session\n")


DEFSH (VTYSH_FTM, show_ipc_blockinfo_cmd,
       "show ipc <1-125> blockinfo",
	   "show\n"
	   "ipc\n"
	   "key value format<1-125>\n"
	   "blockinfo\n")

/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_FTM,ftm_log_level_ctl_cmd_vtysh,
        "debug ftm(enable | disable)",
        "Output log of debug level\n"
        "Program name\n"
        "Enable\n"
        "Disable\n")

/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_FTM,ftm_show_log_level_ctl_cmd_vtysh,
        "show debug ftm",
    	SHOW_STR
    	"Output log of debug level\n"
    	"Program name\n")

DEFSH (VTYSH_FTM, show_ftm_pw_index_cmd_vtysh,
    	"show ftm pw {index <1-4294967295>}",
    	SHOW_STR
    	"Forward soft management\n"
    	"Pseudo wire\n"
    	"Index for PW\n"
    	"<1-4294967295>\n")

DEFSH (VTYSH_FTM, show_ftm_ilm_cmd_vtysh,
    	"show ftm ilm {label <16-1015807>}",
        SHOW_STR
    	"Forward soft management\n"
    	"Incoming label map\n"
    	"Label for ILM\n"
    	"<16-1015807>\n")

DEFSH (VTYSH_FTM, show_ftm_nhlfe_cmd_vtysh,
    	"show ftm nhlfe {index <10001-15000>}",
    	SHOW_STR
    	"Forward soft management\n"
    	"Next hop label forward entry\n"
    	"Index for NHLFE\n"
    	"<10001-15000>\n")


DEFSH (VTYSH_FTM, show_ftm_mpls_tunnel_cmd_vtysh,
    	"show ftm tunnel [USP]",
    	SHOW_STR
    	"Forward soft management\n"
    	CLI_INTERFACE_TUNNEL_STR
    	CLI_INTERFACE_TUNNEL_VHELP_STR)

DEFSH (VTYSH_FTM, show_ftm_protocol_cmd_vtysh,
	"show ftm protocol (ip|eth|mpls)",
	SHOW_STR
	"Ftm \n"
	"Protocol\n"
	"Ip protocol info\n"
	"Eth protocol info\n"
	"Mpls protocol info\n")

DEFUNSH( VTYSH_FTM,
       tcp_mode,
       tcp_mode_cmd_vtysh,
       "tcp",
       "TCP mode\n")
{
	vty->node = TCP_NODE;
	return CMD_SUCCESS;
}

DEFUNSH ( VTYSH_FTM,
		 tcp_mode_exit,
		 tcp_mode_exit_cmd_vtysh,
		 "exit",
		 "Exit current mode and down to previous mode\n" )
{
   return vtysh_exit ( vty );
}


DEFSH(VTYSH_FTM,
	tcp_keepalive_control_switch_cmd_vtysh,
	"tcp keepalive (enable | disable)",
	"TCP\n"
	"Tcp keepalive\n"
	"enable\n"
	"disable\n")


DEFSH(VTYSH_FTM,
	tcp_keepalive_time_control_cmd_vtysh,
	"tcp keepalive-time <60-7200>",
	"TCP\n"
	"keepalive time\n"
	"<60-7200>more than 60s,less than 7200s(2h)\n")

DEFSH(VTYSH_FTM,
	show_tcp_config_cmd_vtysh,
	"show tcp config",
	"show\n"
	"TCP\n"
	"the config of TCP\n")

 DEFSH(VTYSH_FTM,
	ftm_pkt_debug_fun_cmd_vtysh,
	"debug ftm (pkt|ip|tcp|udp|l3|arp|ndp) (enable|disable)",
	"Debug information to moniter\n"
	"Programe name\n"
	"FTM packet info\n"
	"IP debug info\n"
	"TCP debug info\n"
	"UDP debug info\n"
	"L3 debug info\n"
	"Arp debug info\n"
	"Ndp debug info\n"
	"debug enable\n"
	"debug disable\n")
 
 DEFSH(VTYSH_FTM,
	 show_ftm_pkt_debug_fun_cmd_vtysh,
	 "show ftm debug",
	 SHOW_STR
	 "Ftm"
	 "Debug status\n")


void vtysh_init_ftm_cmd()
{
	install_node ( &tcp_node, NULL );
	vtysh_install_default(TCP_NODE);

	install_element_level(CONFIG_NODE, &tcp_mode_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level(TCP_NODE, &tcp_mode_exit_cmd_vtysh,VISIT_LEVE,CMD_SYNC);
	install_element_level(TCP_NODE, &tcp_keepalive_time_control_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level(TCP_NODE, &tcp_keepalive_control_switch_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level(CONFIG_NODE, &show_tcp_config_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);

    install_element_level(CONFIG_NODE, &show_ftm_fib_all_cmd,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ftm_fib_cmd,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ftm_fibv6_all_cmd,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_ftm_fibv6_cmd,MONITOR_LEVE_2, CMD_LOCAL);
    //install_element(CONFIG_NODE, &show_ftm_nhp_cmd, CMD_LOCAL);
    //install_element(CONFIG_NODE, &show_ftm_ecmp_cmd, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_gigabit_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_xgigabit_ethernet_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_tdm_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_stm_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_vlanif_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_trunk_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_loopback_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_tunnel_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_clock_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &debug_ftm_packet_cmd,MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &no_debug_ftm_packet_cmd,MANAGE_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ftm_packet_stat_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &clear_ftm_packet_stat_cmd,CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_tcp_session_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_tcp_session_v6_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_udp_session_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_udp_session_v6_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_ipc_blockinfo_cmd,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ftm_log_level_ctl_cmd_vtysh,MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ftm_show_log_level_ctl_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ftm_pw_index_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ftm_ilm_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ftm_nhlfe_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &show_ftm_mpls_tunnel_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ftm_protocol_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (CONFIG_NODE, &ftm_pkt_debug_fun_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ftm_pkt_debug_fun_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
}
