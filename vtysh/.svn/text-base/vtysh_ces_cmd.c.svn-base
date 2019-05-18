/**
 * \page cmds_ref_ces CES
 * - \subpage modify_log_ces
 *
 */

/**
 * \page modify_log_ces Modify Log
 * \section ces-v007r004 HiOS-V007R004
 *  -#
 * \section ces-v007r003 HiOS-V007R003
 *  -#
 */

#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"
struct cmd_node sdh_dxc_node =
{ 
    SDH_DXC_NODE,
    "%s(config-sdhdxc)# ",
    1,
};


//配置端口封装格式
DEFSH (VTYSH_CES,
	port_encap_cmd_vtysh,
	"encapsulate (satop | cesopsn)",
	"Interface encap format\n"
	"Satop format\n"
	"Cesopsn format\n"
	)

//恢复默认端口封装格式
DEFSH( VTYSH_CES,
	no_port_encap_cmd_vtysh,
	"no encapsulate",
	NO_STR
	"Interface encap format\n"
	)

//配置端口的成帧模式
DEFSH (VTYSH_CES|VTYSH_IFM|VTYSH_MPLS,
    port_mode_cmd_vtysh,
    "mode (unframed | framed | multiframed)",
    "Interface frame mode\n"
    "Unframed mode\n"
    "Framed mode\n"
    "Multiframed mode\n"
    )

//恢复默认端口的成帧模式
DEFSH( VTYSH_CES|VTYSH_IFM|VTYSH_MPLS,
	no_port_mode_cmd_vtysh,
	"no mode",
	NO_STR
	"Interface frame mode\n"
	)

//配置端口的帧格式
DEFSH (VTYSH_CES,
    frame_format_cmd_vtysh,
    "frame (pcm30|pcm31) {crc}",
    "Interface frame format\n"
    "Pcm30 format\n"
    "Pcm31 format\n"
    "Crc\n"
    )

/*恢复默认帧格式*/
DEFSH(VTYSH_CES,
	no_frame_format_cmd_vtysh,
	"no frame",
	NO_STR
	"Interface frame format\n"
	)

//配置接口或子接口的时隙
DEFSH (VTYSH_CES,
    port_time_slot_cmd_vtysh,
    "time-slot <1-31> {to <1-31>}",
    "Interface time slot\n"
    "The value of time slot\n"
    "The end time slot\n"
    "The end value of time slot\n"
    )

DEFUNSH (VTYSH_CES,
		sdh_dxc_mode_cmd,
        sdh_dxc_mode_cmd_vtysh,
        "sdhdxc",
        "sdh_dxc command mode\n")
{
        vty->node = SDH_DXC_NODE;

        return CMD_SUCCESS;
}

//恢复接口或子接口的默认时隙
DEFSH(VTYSH_CES,
    no_time_slot_cmd_vtysh,
    "no time-slot",
    NO_STR
    "Interface time slot\n"
)


//配置端口的 jitter-buffer
DEFSH (VTYSH_CES,
    port_jitter_buffer_cmd_vtysh,
    "jitter-buffer <4-255>",
    "Interface jitter buffer\n"
    "The value of jitter buffer\n"
    )

/*恢复默认端口jitter buffer*/
DEFSH(VTYSH_CES,
	no_port_jitter_buffer_cmd_vtysh,
	"no jitter-buffer",
	NO_STR
	"Interface jitter buffer\n"
	)

//配置端口 rtp 使能和禁止
DEFSH (VTYSH_CES,
    port_rtp_cmd_vtysh,
    "rtp (enable|disable)",
    "Interface rtp\n"
    "Enable rtp of the interface\n"
    "Disable rtp of the interface\n"
    )


//配置端口环回
DEFSH (VTYSH_CES,
    port_loopback_cmd_vtysh,
    "loopback (internal|external)",
    "Interface loopback\n"
    "Internal loopback\n"
    "External loopback\n"
    )

//配置接口不环回
DEFSH(VTYSH_CES,
	no_port_loopback_cmd_vtysh,
	"no loopback",
	NO_STR
	"Interface loopback\n"
	)

//配置端口发送时钟
DEFSH (VTYSH_CES,
    port_clock_cmd_vtysh,
	"clock-send (loopback|local|acr|dcr|phy)",
	"Interface send clock\n"
	"Loopback clock\n"
	"Local clock\n"
	"Acr clock\n"
	"Dcr clock\n"
	"Phy clock\n"
    )

//恢复默认发送时钟
DEFSH(VTYSH_CES,
	no_port_clock_cmd_vtysh,
	"no clock-send",
	NO_STR
	"Interface send clock\n"
	)

//配置端口的接收时钟
DEFSH(VTYSH_CES,
	port_recvclock_cmd_vtysh,
	"clock-receive (line|external-sync|local)",
	"Interface receive clock\n"
	"Line clock\n"
	"External synchronization line clock\n"
	"Local clock\n"
	)

//恢复默认接收时钟
DEFSH(VTYSH_CES,
	no_port_recvclock_cmd_vtysh,
	"no clock-receive",
	NO_STR
	"Interface receive clock\n"
	)

//配置端口 prbs
DEFSH (VTYSH_CES,
    port_prbs_cmd_vtysh,
	"prbs (enable|disable)",
    "Interface prbs\n"
    "Enable prbs of the interface\n"
    "Disable prbs of the interface\n"
    )
    
/*--------------------------------start stm-n cli-----------------------------------*/
DEFSH(VTYSH_CES,
    ces_oh_j2_cmd_vtysh,
    "oh j2 sdh J2-VALUE { padding-zero }",
    "Overhead\n"
    "J2\n"
    "Default frame format: SDH\n"
    "Lower-Order VC-N path trace byte j2: 1-15 characters\n"
    "Padding character, space or zero\n")
    
DEFSH(VTYSH_CES,
    ces_no_oh_j2_cmd_vtysh,
    "no oh j2",
    NO_STR
    "Overhead\n"
    "J2\n")

DEFSH(VTYSH_CES,
    stm_loopback_cmd_vtysh,
    "loopback (local | remote)",
    "stm interface loopback\n"
    "stm internal loopback\n"
    "stm external loopback\n")

DEFSH(VTYSH_CES,
    stm_no_loopback_cmd_vtysh,
    "no loopback",
    NO_STR
    "Interface loopback\n")

DEFSH(VTYSH_CES,
    stm_oh_j0_cmd_vtysh,
    "oh j0 sdh J0-VALUE { padding-zero }",
    "Overhead\n"
    "J0\n"
    "Default frame format: SDH\n"
    "Regeneration Section Trace Message j0: 1-15 characters\n"
    "Padding character, space or zero\n")

DEFSH(VTYSH_CES,
    stm_no_oh_j0_cmd_vtysh,
    "no oh j0",
    NO_STR
    "Overhead\n"
    "J0\n")

DEFSH(VTYSH_CES,
    stm_oh_j1_cmd_vtysh,
    "oh j1 sdh J1-VALUE { padding-zero }",
    "Overhead\n"
    "J1\n"
    "Default frame format: SDH\n"
    "Higher-Order VC-N path trace byte j1: 1-15 characters\n"
    "Padding character, space or zero\n")

DEFSH(VTYSH_CES,
    stm_no_oh_j1_cmd_vtysh,
    "no oh j1",
    NO_STR
    "Overhead\n"
    "J0\n")

DEFSH(VTYSH_CES,
    stm_oh_c2_cmd_vtysh,
    "oh c2 C2_VALUE",
    "Overhead\n"
    "Path signal label byte\n"
    "Value <00-FF>\n")

DEFSH(VTYSH_CES,
    stm_no_oh_c2_cmd_vtysh,
    "no oh c2",
    NO_STR
    "Overhead\n"
    "Path signal label byte\n")

DEFSH(VTYSH_CES,
    stm_map_to_tdm_cmd_vtysh,
    "vc4 <1-16> vc12 <1-63> tdm interface USP",
    "Higher-Order path VC-4 ID\n"
    "Value of VC-4: <1-16>\n"
    "Higher-Order path VC-12 ID\n"
    "Value of VC-12: <1-63>\n"
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_VHELP_STR)

DEFSH(VTYSH_CES,
    stm_no_map_to_tdm_cmd_vtysh,
    "no vc4 <1-16> vc12 <1-63> tdm interface USP",
    NO_STR
    "Higher-Order path VC-4 ID\n"
    "Value of VC-4: <1-16>\n"
    "Higher-Order path VC-12 ID\n"
    "Value of VC-12: <1-63>\n"
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_VHELP_STR)

DEFSH(VTYSH_CES,
    stm_msp_group_cmd_vtysh,
    "msp-group <1-32> master interface stm USP backup interface stm USP { mode (opo|oto) }",
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: master\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "interface role: backup\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "msp mode\n"
    "one plus one mode\n"
    "one to one mode\n")

DEFSH(VTYSH_CES,
    stm_no_msp_group_cmd_vtysh,
    "no msp-group <1-32>",
    NO_STR
    "msp gourp\n"
    "Value of msp gourp ID: <1-32>\n")

DEFSH(VTYSH_CES,
    stm_msp_group_wtr_cmd_vtysh,
    "msp-group <1-32> backup failback wtr <1-12>",
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: backup\n"
    "switch from back to master interface\n"
    "switch time\n"
    "time <1-12>, default value is 1 min\n")

DEFSH(VTYSH_CES,
    stm_no_msp_group_wtr_cmd_vtysh,
    "no msp-group <1-32> backup failback wtr",
    NO_STR
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: backup\n"
    "switch from back to master interface\n"
    "switch time\n")
    
DEFSH(VTYSH_CES,
    show_msp_status_cmd_vtysh,
    "show msp-group <1-32> status",
    SHOW_STR
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "msp status:work at master interface or backup interface\n")

DEFSH(VTYSH_CES,
    config_tdm_dxc1_bothway_cmd_vtysh,
	"vc12dxc interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface tdm USP num <1-63>",
    "vc12dxc\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_TDM_VHELP_STR
    "num\n"
    "tdm interface<1-63>\n"
    
    )	

DEFSH(VTYSH_CES ,
    config_tdm_dxc_bothway_cmd_vtysh,   
	"vc12dxc NAME interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface tdm USP num <1-63>",
    "vc12dxc\n"
    "name\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_TDM_VHELP_STR
    "num\n"
    "tdm interface<1-63>\n")   


DEFSH(VTYSH_CES ,
    config_stm_dxc_bothway_cmd_vtysh,
	"vc12dxc NAME interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface stm USP vc4 <1-128> vc12 <1-63> num <1-63>",
    "vc12dxc\n"
    "name\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "num\n"
    "stm interface<1-63>\n"
    
    )

DEFSH(VTYSH_CES ,
    config_stm_dxc1_bothway_cmd_vtysh,
	"vc12dxc interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface stm USP vc4 <1-128> vc12 <1-63> num <1-63>",
    "vc12dxc\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "num\n"
    "stm interface<1-63>\n"
    
    )

DEFSH (VTYSH_CES, config_stm_dxc_wtr_failback_cmd_vtysh,
        "vc12dxc NAME backup failback [wtr <1-12>]",
        "vc12dxc\n"
        "name\n"
        "Backup\n"
        "failback\n"
    	"wtr time\n"
    	"wtr time<1-12>\n")
#if 0
DEFSH (VTYSH_CES, config_stm_dxc_wtr_non_failback_cmd_vtysh,
        "vc12dxc NAME backup non-failback",
        "vc12dxc\n"
        "name\n"
        "Backup\n"
        "non-failback")
#endif       
DEFSH(VTYSH_CES ,
    no_stm_dxc_namestr_cmd_vtysh,
	"no vc12dxc NAME",
    "delete namestr\n"
    "vc12dxc\n"
    "name\n"
    )	

DEFSH(VTYSH_CES ,
    show_stm_dxc_namestr_cmd_vtysh,
	"show vc12dxc [NAME]",
    "show namestr\n"
    "vc12dxc\n"
    "name\n"
    )	

DEFUNSH (VTYSH_CES,
        dpc_exit_vtysh,
        dxc_exit_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFSH(VTYSH_CES ,
    stm_switch_image_cmd_vtysh,
    "switch emulation-image (e1 | stm-1) slot <1-32>",
    "switch\n"
    "Switch emulation image\n"
    "Image for E1 (Default)\n"
    "Image for STM-1\n"
    "slot number <1-32>")

/*--------------------------------end stm-n cli-----------------------------------*/
#if 0
/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_CES,ces_log_level_ctl_cmd_vtysh,	"debug ces(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")

/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_CES,ces_show_log_level_ctl_cmd_vtysh,	"show debug ces",
	SHOW_STR"Output log of debug level\n""Program name\n")
#endif
//for debug info print
DEFSH (VTYSH_CES,
       ces_conf_debug_cmd_vtysh,
	   "debug ces (enable|disable) (all|tdm|stm|other)",
	   "Debuv config\n"
	   "Ces config\n"
       "Ces debug enable\n"
	   "Ces debug disable\n"
	   "Ces debug type all\n"
	   "Ces debug type tdm\n"
	   "Ces debug type stm\n"
	   "Ces debug type other\n")

/* install all cmd */
void vtysh_init_ces_cmd(void)
{
	install_node(&sdh_dxc_node, NULL);
	vtysh_install_default (SDH_DXC_NODE);
	
    install_element_level (CONFIG_NODE, &sdh_dxc_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_encap_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_port_encap_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_daemon_order_level  ( TDM_IF_NODE, &port_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 3, VTYSH_CES, VTYSH_IFM, VTYSH_MPLS );
	install_element_daemon_order_level  ( TDM_IF_NODE, &no_port_mode_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC, 3, VTYSH_CES, VTYSH_IFM, VTYSH_MPLS );
    install_element_level (TDM_IF_NODE, &frame_format_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_frame_format_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_jitter_buffer_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_port_jitter_buffer_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_rtp_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_port_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_clock_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_port_clock_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_recvclock_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &no_port_recvclock_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &port_prbs_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level (TDM_SUBIF_NODE, &port_time_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_SUBIF_NODE, &no_time_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	//install_element_level (CONFIG_NODE, &ces_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	//install_element_level (CONFIG_NODE, &ces_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

    /*--------------------------------start stm-n cli-----------------------------------*/
    install_element_level (TDM_IF_NODE, &ces_oh_j2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (TDM_IF_NODE, &ces_no_oh_j2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    
    install_element_level (STM_IF_NODE, &stm_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_no_loopback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_oh_j0_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_no_oh_j0_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_oh_j1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_no_oh_j1_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_oh_c2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_no_oh_c2_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_map_to_tdm_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (STM_IF_NODE, &stm_no_map_to_tdm_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &stm_msp_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &stm_no_msp_group_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &stm_msp_group_wtr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &stm_no_msp_group_wtr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level (CONFIG_NODE, &show_msp_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(SDH_DXC_NODE, &config_tdm_dxc_bothway_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(SDH_DXC_NODE, &config_tdm_dxc1_bothway_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(SDH_DXC_NODE, &config_stm_dxc_bothway_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(SDH_DXC_NODE, &config_stm_dxc1_bothway_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	//install_element_level(SDH_DXC_NODE, &config_stm_dxc_wtr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(SDH_DXC_NODE, &config_stm_dxc_wtr_failback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	//install_element_level(SDH_DXC_NODE, &config_stm_dxc_wtr_non_failback_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	
	install_element_level(SDH_DXC_NODE, &no_stm_dxc_namestr_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(SDH_DXC_NODE, &dxc_exit_cmd, VISIT_LEVE, CMD_SYNC);
    install_element_level(SDH_DXC_NODE, &show_stm_dxc_namestr_cmd_vtysh, VISIT_LEVE, CMD_SYNC);
	/*--------------------------------end stm-n cli-----------------------------------*/
	
	install_element_level (CONFIG_NODE, &stm_switch_image_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ces_conf_debug_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
}


