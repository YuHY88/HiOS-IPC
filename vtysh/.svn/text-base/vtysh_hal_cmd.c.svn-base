/**
 * \page cmds_ref_hal HAL
 * - \subpage modify_log_hal
 *
 */

/**
 * \page modify_log_hal Modify Log
 * \section hal-v007r004 HiOS-V007R004
 *  -#
 * \section hal-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

DEFSH (VTYSH_HAL,
        hw_rw_cmd,
        "hw-rw <0-7> command CMD",
        "Hardware register read and write shell.\n"
        "Hardware chip unit.\n"
        "Hardware r/w command.\n"
        "Hardware r/w command string, param separeted by '/', (expample: ps/ge0).\n")


DEFSH ( VTYSH_HAL , show_hal_ifm_cmd_vtysh,
        "show hal ifm interface",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR )

DEFSH ( VTYSH_HAL, show_l2cp_interface_vtysh,
        "show l2cp ",
		"Show info\n"
		"l2cp information\n")

DEFSH ( VTYSH_HAL , show_hal_ifm_ethernet_cmd_vtysh,
        "show hal ifm interface ethernet {USP}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR )

DEFSH ( VTYSH_HAL, show_hal_ifm_tdm_cmd_vtysh,
        "show hal ifm interface tdm {USP}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TDM_STR
        CLI_INTERFACE_TDM_VHELP_STR )
/*
DEFSH ( VTYSH_HAL , show_hal_ifm_stm_cmd_vtysh,
        "show interface stm {USP}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_STM_STR
        CLI_INTERFACE_STM_VHELP_STR )
*/
DEFSH ( VTYSH_HAL , show_hal_ifm_vlanif_cmd_vtysh,
        "show hal ifm interface vlanif {<1-4094>}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR )

DEFSH ( VTYSH_HAL, show_hal_ifm_trunk_cmd_vtysh,
        "show hal ifm interface trunk {TRUNK}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TRUNK_STR
        CLI_INTERFACE_TRUNK_VHELP_STR )

DEFSH ( VTYSH_HAL , show_hal_ifm_loopback_cmd_vtysh,
        "show hal ifm interface loopback {<0-128>}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_LOOPBACK_STR
        CLI_INTERFACE_LOOPBACK_VHELP_STR )


DEFSH ( VTYSH_HAL , show_hal_ifm_tunnel_cmd_vtysh,
        "show hal ifm interface tunnel {USP}",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_TUNNEL_STR
        CLI_INTERFACE_TUNNEL_VHELP_STR )

DEFSH ( VTYSH_HAL , show_hal_ifm_clock_cmd_vtysh,
        "show hal ifm interface clock [<1-2>]",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
        CLI_INTERFACE_STR
        CLI_INTERFACE_CLOCK_STR
        CLI_INTERFACE_CLOCK_VHELP_STR )

DEFSH ( VTYSH_HAL,
        show_hal_ifm_gigabit_ethernet_cmd,
		"show hal ifm interface gigabitethernet {USP}",
		"show info\n"
		"\n"
		"\n"
		"Select an interface to configure\n"
		"GigabitEthernet interface type\n"
		"The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		)

DEFSH ( VTYSH_HAL,
        show_hal_ifm_xgigabit_ethernet_cmd,
		"show hal ifm interface xgigabitethernet {USP}",
		"show info\n"
		"\n"
		"\n"
		"Select an interface to configure\n"
		"10GigabitEthernet interface type\n"
		"The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		)

DEFSH ( VTYSH_HAL,
        show_hal_ifm_ethernetlist_cmd,
		"show hal ifm ethernet interface list" ,
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
		"Ethernet interface information\n"
		"Interface information\n"
		"Interface information list\n")

DEFSH ( VTYSH_HAL,
        show_hal_ifm_ifvlan_trunk_cmd,
		"show hal ifm ifvlan trunk TRUNK vlan <0-4095> cvlan <0-4095>",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
		"Ifvlan table"
		"Trunk interface\n"
		"The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
		"Vlan"
		"Vaule of vlan"
		"Cvlan"
		"Vaule of cvlan"
        )

DEFSH ( VTYSH_HAL,
        show_hal_ifm_ifvlan_ethernet_cmd,
		"show hal ifm ifvlan (ethernet|xgigabitethernet|gigabitethernet) USP vlan <0-4095> cvlan <0-4095>",
		"show info\n"
		"\n"
		"\n"
		"Ifvlan table"
		"Ethernet interface type\n"
		"GigabitEthernet interface type\n"
		"10GigabitEthernet interface type\n"
		"The port of the interface, format: <0-7>/<0-31>/<0-255>\n"
		"Vlan"
		"Vaule of vlan"
		"Cvlan"
		"Vaule of cvlan"
        )

DEFSH (VTYSH_HAL,
       show_hal_timerinfo_cmd_vty,
       "show hal timer",
       SHOW_STR
       "Hal module information\n"
       "Hal module timer\n")

DEFSH ( VTYSH_HAL,
		show_hal_mplsif_trunk_cmd,
		"show hal mplsif {trunk TRUNK}",
		"show info\n"
		"\n"
		"\n"
		"hal_ifm table"
		"Ethernet interface type\n"
		"The port of the interface, format: <0-7>/<0-31>/<0-255>\n"
		"Trunk interface\n"
		"The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
		)

DEFSH ( VTYSH_HAL,
		show_hal_mplsif_ethernet_cmd,
		"show hal mplsif ethernet USP",
		"show info\n"
		"\n"
		"\n"
		"hal_ifm table"
		"Ethernet interface type\n"
		"The port of the interface, format: <0-7>/<0-31>/<0-255>\n"
		"Trunk interface\n"
		"The port/subport of trunk, format: <1-128>[.<1-4095>]\n"
		)


DEFSH ( VTYSH_HAL,
		show_hal_mplsif_gigabit_ethernet_cmd,
		"show hal mplsif gigabitethernet USP",
		"show info\n"
		"\n"
		"\n"
		"hal_ifm table"
		"GigabitEthernet interface type\n"
		"The port of the interface, format: <0-7>/<0-31>/<0-255>\n"
		)

DEFSH ( VTYSH_HAL,
		show_hal_mplsif_xgigabit_ethernet_cmd,
		"show hal mplsif xgigabitethernet USP",
		"show info\n"
		"\n"
		"\n"
		"hal_ifm table"
		"10GigabitEthernet interface type\n"
		"The port of the interface, format: <0-7>/<0-31>/<0-255>\n"
		)

DEFSH ( VTYSH_HAL,
		show_hal_ifm_ifvlan_all_cmd,
		"show hal ifm ifvlan",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
		"Ifvlan table")

DEFSH ( VTYSH_HAL,
		debug_hal_ifm_log_cmd,
		"debug hal ifm (enable|disable)" ,
		"Debug\n"
		"Hal information\n"
		"Hal ifm information\n"
		"Enable\n"
		"Disable\n")


#if 0
DEFSH ( VTYSH_HAL,
        show_hal_nhp_cmd,
        "show hal nhp index NUM",
        SHOW_STR
        "Hal information\n"
        "Nhp information\n"
        "Nhp index\n"
        "<1-4096> or <10001-14096>\n"
      )

DEFSH ( VTYSH_HAL,
        show_hal_ecmp_cmd,
        "show hal ecmp index NUM",
        SHOW_STR
        "Hal information\n"
        "Ecmp information\n"
        "Index\n"
        "<1-8192>\n"
      )
#endif

DEFSH ( VTYSH_HAL,
	show_hal_all_arp_cmd,
	"show hal arp",
	"Show running arp information\n"
    "Hal\n"
    "Arp information\n" )


DEFSH(VTYSH_HAL,
       config_arp_anti_smac_cmd,
       "arp speed-limit source-mac [XX:XX:XX:XX:XX:XX] maximum <0-1024>",
       "Arp\n"
       "Arp packet speed-limit\n"
       "Ether header source mac\n"
       "Mac format\n"
       "The num of speed-limit allow\n"
       "The range of speed-limit\n"
       )

DEFSH(VTYSH_HAL,
      no_config_arp_anti_smac_cmd,
      "no arp speed-limit source-mac XX:XX:XX:XX:XX:XX",
      "Undo\n"
      "Arp\n"
      "Arp packet speed-limit\n"
      "Ether header source mac\n"
      "Mac format\n"
      "The num of speed-limit allow\n"
      "The range of speed-limit\n"
      )


DEFSH(VTYSH_HAL,
       config_arp_anti_sip_cmd,
       "arp speed-limit sip [A.B.C.D] maxinum <0-1024>",
       "Arp\n"
       "Arp packet speed-limit\n"
       "source ip in arp header\n"
       "Ip format\n"
       "The num of speed-limit allow\n"
       "The range of speed-limit\n"
)

DEFSH(VTYSH_HAL,
       no_config_arp_anti_sip_cmd,
       "no arp speed-limit sip A.B.C.D",
       "Undo\n"
       "Arp\n"
       "Arp packet speed-limit\n"
       "Source ip in arp header\n"
       "Ip format\n"
      )

DEFSH(VTYSH_HAL,
       config_arp_anti_vlan_cmd,
       "arp speed-limit vlan [<1-4095>] maxinum <0-1024>",
       "Arp\n"
       "Arp packet speed-limit\n"
       "Vlan\n"
       "Vlan range\n"
       "The num of speed-limit allow\n"
       "The range of speed-limit\n"
)

DEFSH(VTYSH_HAL,
       no_config_arp_anti_vlan_cmd,
       "no arp speed-limit vlan <1-4095>",
       "Undo\n"
       "Arp\n"
       "Arp packet speed-limit\n"
       "Vlan\n"
       "Vlan range\n"
      )

    
DEFSH(VTYSH_HAL,
    config_arp_anti_ifidx_eth_cmd,
    "arp speed-limit interface ethernet USP maxinum <0-1500>",
    "Arp\n"
    "Arp pkt speed limit\n"
    INTERFACE_STR
    CLI_INTERFACE_ETHERNET_STR
    CLI_INTERFACE_ETHERNET_VHELP_STR
    "The num of speed-limit allow\n"
    "The range of speed-limit\n"
    )

DEFSH(VTYSH_HAL,
    config_arp_anti_ifidx_geth_cmd,
    "arp speed-limit interface gigabitethernet USP maxinum <0-1500>",
    "Arp\n"
    "Arp pkt speed limit\n"
    INTERFACE_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    "The num of speed-limit allow\n"
    "The range of speed-limit\n"
    )
    
DEFSH(VTYSH_HAL,
    config_arp_anti_ifidx_xgeth_cmd,
    "arp speed-limit interface xgigabitethernet USP maxinum <0-1500>",
    "Arp\n"
    "Arp pkt speed limit\n"
    INTERFACE_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_STR
    CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    "The num of speed-limit allow\n"
    "The range of speed-limit\n"
    )


DEFSH( VTYSH_HAL,
    show_hal_arp_anti_acl_cmd,
    "show hal arp anti acl",
    SHOW_STR
    "Hal\n"
    "Arp\n"
    "Arp anti\n"
    "Arp anti acl\n"
    )

DEFSH( VTYSH_HAL,
      show_hal_ipv6_ndp_cmd,
      "show hal ipv6 neighbor",
      SHOW_STR
      "Hal\n"
      IPV6_STR
      "Ipv6 neighbor\n")


DEFSH ( VTYSH_HAL,
	show_hal_pbr_acl_cmd,
	"show hal pbr",
	"Show running pbr information\n"
	"Hal\n"
	"Pbr\n")


DEFSH ( VTYSH_HAL,
	show_hal_all_acl_cmd,
	"show hal acl",
	"Show running acl information\n"
	"Hal\n"
	"Acl information\n")

DEFSH (VTYSH_HAL,
        show_hal_qos_car_cmd,
        "show hal qos car-profile <1-2000>",
        SHOW_STR
        "hal\n"
        "Quality of Service\n"
        "QoS car-profile\n"
        "QoS car-profile ID\n" )

DEFSH ( VTYSH_HAL,
        show_hal_qos_mapping_domain_cmd,
        "show hal qos mapping (cos-domain|exp-domain|tos-domain|dscp-domain) <0-5>",
        SHOW_STR
        "hal\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Cos to queue mapping domain\n"
        "Exp to queue mapping domain\n"
        "Tos to queue mapping domain\n"
        "Dscp to queue mapping domain\n"
        "QoS mapping domain ID\n" )


DEFSH ( VTYSH_HAL,
        show_hal_qos_mapping_phb_cmd,
        "show hal qos mapping (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>",
        SHOW_STR
        "hal\n"
        "Quality of Service\n"
        "QoS mapping\n"
        "Queue to cos mapping\n"
        "Queue to exp mapping\n"
        "Queue to tos mapping\n"
        "Queue to dscp mapping\n"
        "QoS mapping phb ID\n" )



DEFSH ( VTYSH_HAL,
        debug_hal_bfd_table_cmd,
        "debug hal bfd table <0-1023>",
        "Debug\n"
        "Hal information\n"
        "bfd information\n"
        "table information\n"
        "<0-1023>\n")

DEFSH( VTYSH_HAL,
        debug_hal_bfd_global_cmd,
        "debug hal bfd global",
        "Debug\n"
        "Hal information\n"
        "bfd information\n"
        "global\n")

DEFSH( VTYSH_HAL,
        debug_hal_bfd_session_cmd,
		"debug hal bfd session <1-65535>",
       	"Debug\n"
		"Hal information\n"
		"bfd information\n"
		"session information\n"
		"local-id information\n"
		"<1-65535>\n")

DEFSH ( VTYSH_HAL,
        show_hal_ilm_cmd,
        "show hal ilm {label NUM}",
        "Show running system information\n"
        "Hal information\n"
        "Ilm information\n"
        "Label infomation\n"
        "Value <0x1-0xFFFFF>\n"/*"Value <0-4096>\n"*/
      )

DEFSH ( VTYSH_HAL,
		show_hal_nhlfe_cmd,
		"show hal nhlfe {index NUM}",
		"Show running system information\n"
		"Hal information\n"
		"Nhlfe information\n"
		"Index value\n"
		"Value <1-14096>\n"/*old value <1-4096>*/
	  )

DEFSH ( VTYSH_HAL,
        show_hal_fib_all_cmd,
        "show hal fib",
        SHOW_STR
        "Hal information\n"
        "Forwarding information base\n")

DEFSH ( VTYSH_HAL,
        show_hal_fib_cmd,
        "show hal fib ip (A.B.C.D | A.B.C.D/<0-32>) {vpn <1-1024>}",
        SHOW_STR
        "Hal information\n"
        "Forwarding information base\n"
        IP_STR
        "Ip address\n"
        "Ip address and Length of ip address mask\n"
        "Vpn information\n"
        "Vpn value\n"
      )

DEFSH(VTYSH_HAL, show_hal_fibv6_all_cmd,
    "show hal ipv6 fib",
    SHOW_STR
    "Hal information\n"
	"IPV6 configuration informaton\n"
    "Forwarding information base\n")

DEFSH(VTYSH_HAL, show_hal_fibv6_cmd,
    "show hal ipv6 fib (X:X:X:X:X:X:X:X | X:X:X:X:X:X:X:X/<0-128>) {vpn <1-1024>}",
    SHOW_STR
    "Hal information\n"
	"IPV6 configuration information\n"
    "Forwarding information base\n"
    "IPV6 address\n"
    "IPV6 address and Length of ip address mask\n"
    "Vpn information\n"
    "Vpn value\n")

DEFSH ( VTYSH_HAL,
		show_hal_pw_cmd,
		"show hal pw  {index NUM}  ",
		"Show running system information\n"
		"Hal information\n"
		"Pw information\n"
		"Index value\n"
		"Value <0x1-0xFFFFFFFF>\n"/*"old Value <1-4096>\n")*/
	  )


DEFSH ( VTYSH_HAL,
        show_hal_vsi_cmd,
        "show hal vsi {index NUM} ",
        "Show running system information\n"
		"Hal information\n"
		"VSI value\n"
		"Index value\n"
		"Value <1-1024>\n")/*"old Value <1-4096>\n")*/
DEFSH (  VTYSH_HAL,
                show_hal_tunnel_cmd,
                "show hal tunnel {if USP}",
                "Show running system information\n"
                "Hal information\n"
                "Tunnel information\n"
                "If \n"
                "Usp format <0-7>/<0-31>/<0-255> of the interface\n" )

DEFSH (VTYSH_HAL,
        hal_mac_show_all_cmd,
        "show mac",
        "Show command\n"
        "Show mac\n")

DEFSH (VTYSH_HAL,
        hal_mac_show_statics_cmd,
        "show mac statistics",
        "Show command\n"
        "Show mac\n"
        "Statistics mac\n")

DEFSH (VTYSH_HAL,
        hal_mac_show_eth_cmd,
        "show mac interface (ethernet | gigabitethernet | xgigabitethernet) USP {vlan <1-4094> | vsi <1-1024>}",
        "Show command\n"
        "Show mac\n"
        "Interface\n"
        "Ethernet interface type\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Vlan\n"
        "Vlan <1-4094>\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_show_trunk_cmd,
        "show mac interface trunk TRUNK {vlan <1-4094> | vsi <1-1024>}",
        "Show command\n"
        "Show mac\n"
        "Interface\n"
        "Trunk \n"
        CLI_INTERFACE_TRUNK_VHELP_STR
        //"Trunk <1-128> \n"
        "Vlan\n"
        "Vlan <1-4094>\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_show_vlan_cmd,
        "show mac vlan <1-4094>",
        "Show command\n"
        "Show mac\n"
        "Vlan\n"
        "Vlan <1-4094>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_show_vsi_cmd,
        "show mac vsi <1-1024>",
        "Show command\n"
        "Show mac\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_show_dynamic_cmd,
        "show mac dynamic",
        "Show command\n"
        "Show mac\n"
        "Dynamic\n")

DEFSH (VTYSH_HAL,
        hal_mac_clear_all_cmd,
        "clear mac",
        "Clear command\n"
        "Clear mac\n" )

DEFSH (VTYSH_HAL,
        hal_mac_clear_eth_cmd,
        "clear mac interface (ethernet | gigabitethernet | xgigabitethernet) USP {vlan <1-4094> | vsi <1-1024>}",
        "Clear command\n"
        "Clear mac\n"
        "Interface\n"
        "Ethernet interface type\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Vlan\n"
        "Vlan <1-4094>\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_clear_trunk_cmd,
        "clear mac interface trunk TRUNK {vlan <1-4094> | vsi <1-1024>}",
        "Clear command\n"
        "Clear mac\n"
        "Interface\n"
        "Ethernet interface type\n"
        CLI_INTERFACE_TRUNK_VHELP_STR
        //"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Vlan\n"
        "Vlan <1-4094>\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_clear_vlan_cmd,
        "clear mac vlan <1-4094>",
        "Clear command\n"
        "Clear mac\n"
        "Vlan\n"
        "Vlan <1-4094>\n" )

DEFSH (VTYSH_HAL,
        hal_mac_clear_vsi_cmd,
        "clear mac vsi <1-1024>",
        "Clear command\n"
        "Clear mac\n"
        "Vsi\n"
        "Vsi <1-1024>\n" )

DEFSH (VTYSH_HAL,
        hal_sfp_show_cmd,
        "show interface sfp",
        "Show command\n"
        CLI_INTERFACE_SFP_STR
        CLI_INTERFACE_SFP_VHELP_STR
       )

DEFSH(VTYSH_HAL,
	 hal_sfp_show_all_cmd,
	 "show interface sfp all",
	 "Show command\n"
	 "all information\n"
	 CLI_INTERFACE_SFP_STR
     CLI_INTERFACE_SFP_VHELP_STR
	 )

DEFSH(VTYSH_HAL,
    debug_hal_sla_session_cmd,
	"debug hal sla session <1-65535>",
	"debug command\n"
	"Hal information\n"
	SLA_STR
	"session\n"
	"session <1-65535>\n"
	)

DEFSH(VTYSH_HAL,
    debug_hal_sla_config_cmd,
	"debug hal sla config",
	"debug command\n"
	"Hal information\n"
	SLA_STR
	"config\n"
	)

DEFSH(VTYSH_HAL,
    debug_hal_packet_cmd,
	"debug hal packet (receive|send)",
	"Debug\n"
	"hal\n"
	"packet\n"
	"receive\n"
	"send\n"
	)

DEFSH(VTYSH_HAL,
    no_debug_hal_packet_cmd,
	"no debug hal packet (receive|send)",
	"Delete command\n"
	"Debug\n"
	"hal\n"
	"packet\n"
	"receive\n"
	"send\n"
	)

DEFSH ( VTYSH_HAL,
        show_hal_mplstp_oam_session_cmd,
        "show hal mplstp-oam session [<1-65535>]",
        SHOW_STR
        "hal\n"
        "mplstp oam\n"
        "session\n"
        "Range of oam-session:<1-65535>\n" )


DEFSH (VTYSH_HAL,
        debug_hal_mplstp_oam_session_lm_in_in_cmd,
        "debug hal mplstp-oam session <1-65535> near-end increase VALUE far-end increase VALUE",
        "Debug\n"
        "hal\n"
        "mplstp oam\n"
        "session\n"
        "Range of oam-session:<1-65535>\n"
        "Near end\n"
        "Increase departure of lm\n"
        "Value of departure\n"
        "Far end\n"
        "Increase departure of lm\n"
        "Value of departure\n")

DEFSH (VTYSH_HAL,
        debug_hal_mplstp_oam_session_lm_in_de_cmd,
        "debug hal mplstp-oam session <1-65535> near-end increase VALUE far-end decrease VALUE",
        "Debug\n"
        "hal\n"
        "mplstp oam\n"
        "session\n"
        "Range of oam-session:<1-65535>\n"
        "Near end\n"
        "increase departure of lm\n"
        "Value of departure\n"
        "Far end\n"
        "Decrease departure of lm\n"
        "Value of departure\n")

DEFSH (VTYSH_HAL,
        debug_hal_mplstp_oam_session_lm_de_in_cmd,
        "debug hal mplstp-oam session <1-65535> near-end decrease VALUE far-end increase VALUE",
        "Debug\n"
        "hal\n"
        "mplstp oam\n"
        "session\n"
        "Range of oam-session:<1-65535>\n"
        "Near end\n"
        "decrease departure of lm\n"
        "Value of departure\n"
        "Far end\n"
        "increase departure of lm\n"
        "Value of departure\n")

DEFSH (VTYSH_HAL,
        debug_hal_mplstp_oam_session_lm_de_de_cmd,
        "debug hal mplstp-oam session <1-65535> near-end decrease VALUE far-end decrease VALUE",
        "Debug\n"
        "hal\n"
        "mplstp oam\n"
        "session\n"
        "Range of oam-session:<1-65535>\n"
        "Near end\n"
        "decrease departure of lm\n"
        "Value of departure\n"
        "Far end\n"
        "Decrease departure of lm\n"
        "Value of departure\n")


DEFSH ( VTYSH_HAL,
        debug_hal_stg_cmd,
        "debug hal stg (forward|block|disable) interface (ethernet|gigabitethernet|xgigabitethernet) USP index <1-4095> vlanlist <0-4095> <0-4095> <0-4095> <0-4095> to <0-4095> <0-4095> to <0-4095>",
        "Debug\n"
        "hal\n"
        "stg\n"
        "forward\n"
        "block\n"
        "disable\n"
        "Interface\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "index \n"
        "index <1-4094>\n"
        "vlanlist \n"
        "vlan <1-4094> 0 meam no set\n"
        "vlan <1-4094> 0 meam no set\n"
        "vlan <1-4094> 0 meam no set\n"
        "vlan start <1-4094> 0 meam no set\n"
        "vlan range\n"
        "vlan end <1-4094> 0 meam no set\n"
        "vlan start <1-4094> 0 meam no set\n"
        "vlan range\n"
        "vlan end <1-4094> 0 meam no set\n" )



DEFSH ( VTYSH_HAL,
        debug_hal_stg2_cmd,
        "debug hal stg remove interface (ethernet |gigabitethernet|xgigabitethernet) USP index <1-4095>",
        "Debug\n"
        "hal\n"
        "stg\n"
        "remove\n"
        "Interface\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
        "index\n"
        "index <1-4094>\n" )


DEFSH ( VTYSH_HAL,
        debug_hal_stg_show_cmd,
        "debug hal stg show {<1-4095>}",
        "Debug\n"
        "hal\n"
        "stg\n"
        "show\n"
        "index <1-4095>\n" )


DEFSH (VTYSH_HAL,
        debug_hal_force_tx_cmd,
        "debug hal force-tx interface ethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga tx\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )

DEFSH (VTYSH_HAL,
        debug_hal_force_tx_gigabit_eth_cmd,
        "debug hal force-tx interface gigabitethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga tx\n"
        "Interface\n"
        "GigabitEthernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )

DEFSH (VTYSH_HAL,
        debug_hal_force_tx_xgigabit_eth_cmd,
        "debug hal force-tx interface xgigabitethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga tx\n"
        "Interface\n"
        "10GigabitEthernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )


DEFSH (VTYSH_HAL,
        debug_hal_force_enable_cmd,
        "debug hal force-enable interface ethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga port enable\n"
        "Interface\n"
        "Ethernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )

DEFSH (VTYSH_HAL,
        debug_hal_force_enable_gigabit_eth_cmd,
        "debug hal force-enable interface gigabitethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga port enable\n"
        "Interface\n"
        "GigabitEthernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )

DEFSH (VTYSH_HAL,
        debug_hal_force_enable_xgigabit_eth_cmd,
        "debug hal force-enable interface xgigabitethernet USP (enable|disable)",
        "Debug\n"
        "hal\n"
        "Force fpga port enable\n"
        "Interface\n"
        "10GigabitEthernet interface type\n"
        "The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
        "Enable\n"
        "Disable\n" )

DEFSH (VTYSH_HAL,
		debug_hal_optical_tx_cmd,
		"debug hal optical-tx interface ethernet USP (enable|disable)",
		"Debug\n"
		"hal\n"
		"Set optical tx\n"
		"Interface\n"
		"Ethernet interface type\n"
		"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
		"Enable\n"
		"Disable\n" )

DEFSH (VTYSH_HAL,
		debug_hal_optical_tx_gigabit_eth_cmd,
		"debug hal optical-tx interface gigabitethernet USP (enable|disable)",
		"Debug\n"
		"hal\n"
		"Set optical tx\n"
		"Interface\n"
		"GigabitEthernet interface type\n"
		"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
		"Enable\n"
		"Disable\n" )

DEFSH (VTYSH_HAL,
		debug_hal_optical_tx_xgigabit_eth_cmd,
		"debug hal optical-tx interface xgigabitethernet USP (enable|disable)",
		"Debug\n"
		"hal\n"
		"Set optical tx\n"
		"Interface\n"
		"10GigabitEthernet interface type\n"
		"The port of the interface,  format: <0-7>/<0-31>/<1-255>\n"
		"Enable\n"
		"Disable\n" )

DEFSH ( VTYSH_HAL,
        show_hal_hqos_wred_profile_cmd,
        "show hal qos wred-profile <1-10>",
        SHOW_STR
        "hal\n"
        "Quality of Service\n"
        "QoS wred-profile\n"
        "QoS wred-profile ID\n" )

DEFSH (VTYSH_HAL,
        debug_hal_bitmap_show_cmd,
        "debug hal bitmap show",
        "Debug\n"
        "hal\n"
        "bitmap\n"
        "show\n" )

DEFSH (VTYSH_HAL,
		show_hal_ifm_inter_cmd,
		"show hal ifm inter interface",
		"Show info\n"
		"Hal information\n"
		"Hal ifm information\n"
		"Inter Interface information\n"
		"Interface information\n")


DEFSH ( VTYSH_HAL,
        show_hal_hqos_queue_profile_cmd,
        "show hal qos queue-profile <1-100>",
        SHOW_STR
        "hal\n"
        "Quality of Service\n"
        "QoS queue-profile\n"
        "QoS queue-profile ID\n" )


DEFSH ( VTYSH_HAL,
        debug_hal_slot_gre_mode_set_cmd,
        "debug hal slot <1-8> gre mode (side |front)",
        "debug\n"
        "hal \n"
        "slot info\n"
        "slot num\n"
        "GRE\n"
        "Mode\n"
        "side\n"
        "front\n")

DEFSH( VTYSH_HAL,
       hal_cfm_alarm_enable_cmd,
       "hal cfm alarm (enable | disable)",
       "Hal information\n"
       "Cfm information\n"
       "Alarm information\n"
       "Enable\n"
       "Disable\n")

DEFSH( VTYSH_HAL,
       hsl_bfd_info_show_cmd,
       "show hsl bfd info session <0-65535>",
       "show\n"
       "hsl\n"
       "bfd\n"
       "bfd infomation\n"
       "session\n"
       "session id\n")


DEFSH( VTYSH_HAL,
       hal_cfm_rcv_ccm_delay_cmd,
       "hal cfm receive ccm delay <0-15000>",
       "Hal information\n"
       "Cfm information\n"
       "cfm receive ccm\n"
       "packet is CCM\n"
       "receive ccm after delay timer\n"
       "time(ms)\n")

DEFSH(VTYSH_HAL,
    	mpls_hqos_packet_len_cmd_vtysh,
    	"mpls packet-len <0-10000> len_change <0-100>",
    	"Multi-protocol label switch\n"
    	"Packet Length\n"
    	"Packet Length value <0-10000>\n"
    	"Length change\n"
    	"Length change value <0-100>\n")

#if 0
DEFSH(VTYSH_HAL,
    	debug_hal_mcgroup_create_cmd,
		"debug hal multicast group create NUM",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Create command\n"
		"Group id:1-4096\n")

DEFSH(VTYSH_HAL,
    	debug_hal_mcgroup_destroy_cmd,
		"debug hal multicast group destroy NUM",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Destroy command\n"
		"Group id:1-4096\n")

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_add_if_cmd,
		"debug hal multicast group NUM add interface {ethernet USP | tunnel USP | vlanif <1-4095> | trunk TRUNK}",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Add command\n"
		"Interface information\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Tunnel interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Vlanif interface type\n"
		"Vlan:1-4095\n"
		"Trunk interface type\n"
		"Trunk:<1-128>[.<1-4095>]\n")

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_add_gigabit_eth_if_cmd,
		"debug hal multicast group NUM add interface gigabitethernet USP",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Add command\n"
		"Interface information\n"
		"GigabitEthernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n")

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_add_xgigabit_eth_if_cmd,
		"debug hal multicast group NUM add interface xgigabitethernet USP",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Add command\n"
		"Interface information\n"
		"10GigabitEthernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		)

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_delete_if_cmd,
		"debug hal multicast group NUM delete interface {ethernet USP | tunnel USP | vlanif <1-4095> | trunk TRUNK}",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Delete command\n"
		"Interface information\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Tunnel interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Vlanif interface type\n"
		"Vlan:1-4095\n"
		"Trunk interface type\n"
		"Trunk:<1-128>[.<1-4095>]\n")

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_delete_gigabit_eth_if_cmd,
		"debug hal multicast group NUM delete interface gigabitethernet USP",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Delete command\n"
		"Interface information\n"
		"GigabitEthernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		)

DEFSH(VTYSH_HAL,
		debug_hal_mcgroup_delete_xgigabit_eth_if_cmd,
		"debug hal multicast group NUM delete interface xgigabitethernet USP",
		"Debug command\n"
		"Hal information\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Delete command\n"
		"Interface information\n"
		"10GigabitEthernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		)

DEFSH(VTYSH_HAL,
		debug_hal_mfib_add_cmd,
		"debug hal ipmc add sip A.B.C.D dip A.B.C.D {vpn <1-1024>} interface {ethernet USP | tunnel USP | vlanif <1-4095> | trunk TRUNK} multicast group NUM (check | nocheck)",
		"Debug command\n"
		"Hal information\n"
		"Ipmc module\n"
		"Add command\n"
		"Source ip\n"
		"SIP:A.B.C.D\n"
		"Destination ip\n"
		"DIP:A.B.C.D\n"
		"Vpn information\n"
		"VPN:<1-1024>\n"
		"Interface information\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Tunnel interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Vlanif interface type\n"
		"Vlan:1-4095\n"
		"Trunk interface type\n"
		"Trunk:<1-128>[.<1-4095>]\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Check source port\n"
		"Do not check source port\n")

DEFSH(VTYSH_HAL,
		debug_hal_mfib_gigabit_eth_add_cmd,
		"debug hal ipmc add sip A.B.C.D dip A.B.C.D {vpn <1-1024>} interface gigabitethernet USP multicast group NUM (check | nocheck)",
		"Debug command\n"
		"Hal information\n"
		"Ipmc module\n"
		"Add command\n"
		"Source ip\n"
		"SIP:A.B.C.D\n"
		"Destination ip\n"
		"DIP:A.B.C.D\n"
		"Vpn information\n"
		"VPN:<1-1024>\n"
		"Interface information\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Check source port\n"
		"Do not check source port\n")

DEFSH(VTYSH_HAL,
		debug_hal_mfib_xgigabit_eth_add_cmd,
		"debug hal ipmc add sip A.B.C.D dip A.B.C.D {vpn <1-1024>} interface xgigabitethernet USP multicast group NUM (check | nocheck)",
		"Debug command\n"
		"Hal information\n"
		"Ipmc module\n"
		"Add command\n"
		"Source ip\n"
		"SIP:A.B.C.D\n"
		"Destination ip\n"
		"DIP:A.B.C.D\n"
		"Vpn information\n"
		"VPN:<1-1024>\n"
		"Interface information\n"
		"Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
		"Multicast module\n"
		"Group information\n"
		"Group id:<1-4096>\n"
		"Check source port\n"
		"Do not check source port\n")


DEFSH(VTYSH_HAL,
		debug_hal_mfib_delete_cmd,
		"debug hal ipmc delete sip A.B.C.D dip A.B.C.D {vpn <1-1024>}",
		"Debug command\n"
		"Hal information\n"
		"Ipmc module\n"
		"Delete command\n"
		"Source ip\n"
		"SIP:A.B.C.D\n"
		"Destination ip\n"
		"DIP:A.B.C.D\n"
		"Vpn information\n"
		"VPN:<1-1024>")
#endif

DEFSH (VTYSH_HAL,
        hal_bfd_global_group_flag_cmd,
        "bfd global group <1-10> session <1-1024> to <1-1024>",
        "Enable bfd group command\n"
        "Bfd group\n"
        "Local id\n"
        "Local id <1-1024>\n" )

DEFSH (VTYSH_HAL,
		hal_bfd_no_global_group_flag_cmd,
		"no bfd global group <1-10> ",
		"Disable bfd group command\n"
		"Bfd group\n"
		"group\n"
		"group <1-10>\n" )

DEFSH (VTYSH_HAL,
		hal_bfd_global_priority_cmd,
		"bfd global priority <0-7> ",
		"bfd global priority command\n"
		"Bfd priority\n"
		"Bfd priority <0-7>\n" )

DEFSH ( VTYSH_HAL,
        hal_bfd_global_group_wtr_cmd,
        "bfd global group <1-10> wtr <1-100> ",
        "bfd group wtr timer command\n"
        "Bfd group\n"
		"Bfd group <1-10>\n"
        "wtr\n"
        "timer <1-100>\n" )
DEFSH ( VTYSH_HAL,
        bfd_acl_ces_pw_label_cmd,
        "(bfd | oam) acl ces pw-label <16-1015807>",
        "bfd\n"
        "acl\n"
        "ces\n"
        "pw-label\n"
        "pw-label id\n" )


DEFSH ( VTYSH_HAL,
        no_bfd_acl_ces_pw_label_cmd,
        "no (bfd | oam) acl ces pw-label <16-1015807>",
         "no\n"
        "bfd\n"
        "acl\n"
        "ces\n"
        "pw-label\n"
        "pw-label id\n" )

DEFSH ( VTYSH_HAL,
        hal_ifm_ifg_cmd_vtysh,
        "hal ifm interface ethernet USP ifg <0-50>",
        "hal\n"
        "Ifm\n"
        "Interface \n"
        "Ethernet interface type\n"
        "The port/subport of the interface, format: <0-7>/<0-31>/<0-255>[.<1-4095>]\n"
        "ifg\n"
        "ifg length value\n")

DEFSH ( VTYSH_HAL,
        add_hal_bfd_session_detect_num_cmd,
        "hal bfd session detect_num <1-10>",
        "add config\n"
        "Hal information\n"
        "bfd information\n"
        "session information\n"
        "local-id information\n"
        "<1-10>\n" )

DEFSH ( VTYSH_HAL,
        hal_bfd_detect_num_flag_cmd,
        "hal bfd detect_num (enable|disable)",
        "hal bfd command\n"
        "Hal information\n"
        "change bfd detect_num\n"
        "enable bfd detect_num\n"
        "disable bfd detect_num\n")

DEFSH ( VTYSH_HAL,
        hal_bfd_event_proc_time_cmd,
        "hal bfd event wait time <0-30>",
        "hal bfd command\n"
        "Hal information\n"
        "change bfd event wait time\n"
        "enable bfd event wait time\n"
        "<0-30>\n"
      )

DEFSH ( VTYSH_HAL,
        hal_bfd_fpga_detection_time_cmd,
        "hal bfd fpga detection time <1-10000> ",
        "bfd fpga set detection time\n"
        "detection time\n"
        "time\n"
        "time <1-10000>\n" )

DEFSH ( VTYSH_HAL,
        hal_bfd_fpga_delay_time_cmd,
        "hal bfd fpga delay time <1-30> ",
        "bfd fpga set delay time ms\n"
        "delay time ms\n"
        "time ms\n"
        "time <1-30> ms\n" )

DEFSH ( VTYSH_HAL,
        hal_bfd_fpga_enable_cmd,
        "hal bfd fpga detect (enable | disable) ",
        "bfd fpga set enable or disable\n"
        "enable or disable\n"
        "enable\n"
        "disable\n" )

DEFSH ( VTYSH_HAL,
        hal_qos_domain_hwid_cmd_vtysh,
        "debug qos-mapping-hw-id (cos-domain|tos-domain|exp-domain|dscp-domain) <0-5>",
        "debug\n"
        "qos-mapping_hw-id\n"
        "cos-domain\n"
        "tos-domain\n"
        "exp-domain\n"
        "dscp-domain\n"
        "domain-id value <0-5>\n")


DEFSH ( VTYSH_HAL,
        hal_qos_phb_hwid_cmd_vtysh,
        "debug qos-mapping-hw-id (cos-phb|exp-phb|tos-phb|dscp-phb) <0-5>",
        "debug\n"
        "qos-mapping_hw-id \n"
        "cos-phb\n"
        "tos-phb\n"
        "exp-phb\n"
        "dscp-phb\n"
        "phb-id value <0-5>\n")
DEFSH ( VTYSH_HAL,
		debug_hal_mspw_add_swap_cmd,
		"debug hal mspw add index NUM interface ethernet USP nhi NUM",
		"Debug command\n"
		"Hal information\n"
		"Mspw information\n"
		"Add command\n"
		"pw index\n"
		"<1-65535>\n"
		"interface information\n"
		"interface type ethernet\n"
		"Usp format <0-7>/<0-31>/<0-255> of the interface\n"
		"Nhi information\n"
		"<100000-116384>\n"
	  )


DEFSH ( VTYSH_HAL,
	  no_debug_hal_mspw_add_swap_cmd,
	  "no debug hal mspw add index NUM nhi NUM",
	  NO_STR
	  "Debug command\n"
	  "Hal information\n"
	  "Mspw information\n"
	  "Add command\n"
	  "pw index\n"
	  "<1-65535>\n"
	  "Nhi information\n"
	  "<100000-116384>\n"
	)

DEFSH ( VTYSH_HAL,
        acl_mspw_pw_label_cmd,
        "acl mspw pw-label <16-1015807>  <100000-116384>",
        "acl\n"
        "mspw\n"
        "pw-label\n"
        "pw-label id\n"
        "pw-swap  nhi\n")



DEFSH ( VTYSH_HAL,
        no_acl_mspw_pw_label_cmd,
        "no acl mspw pw-label <16-1015807>",
         "no\n"
        "acl\n"
        "mspw\n"
        "pw-label\n"
        "pw-label id\n" )

DEFSH ( VTYSH_HAL,
        hal_dev_led_test_cmd,
        "protest led (on|off)",
        "Protest command\n"
        "Led test\n"
        "Led on\n" 
        "Led off\n")

DEFSH ( VTYSH_HAL,
        hal_btb_buff_infor_cmd_vtysh,
        "show hal btb buffer",
        "Show command\n"
        "Show hal information\n"
        "Show hal board to board infor\n" 
        "Show hal btb mempool buffer infor\n")


/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_HAL,hal_log_level_ctl_cmd_vtysh,	"debug hal(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")

/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_HAL,hal_show_log_level_ctl_cmd_vtysh,	"show debug hal",
	SHOW_STR"Output log of debug level\n""Program name\n")

/* show sdhvcg lcas status */
DEFSH ( VTYSH_HAL,
        show_lcas_status_cmd_vtysh,
       "show lcas status",
		SHOW_STR
		"lcas\n" 
	    "lcas status\n")

/* show sdhvcg jx */
DEFSH ( VTYSH_HAL,
        show_oh_jx_cmd_vtysh,
        "show oh jx",
	    SHOW_STR
        "oh\n"
        "j1 and j2 state\n")

/* show sdhvcg soh */
DEFSH ( VTYSH_HAL,
        show_oh_status_cmd_vtysh,
        "show oh {c2|g1|f2|k3|f3|n1}",     
	    SHOW_STR
        "oh\n"
        "oh c2 status\n"
        "oh g1 status\n"
        "oh f2 status\n"
        "oh k3 status\n"
        "oh f3 status\n"
        "oh n1 status\n")

/* show sdhvcg bip/b3 cnt */
DEFSH ( VTYSH_HAL,
        b3_bip_statistics_cmd_vtysh,
       "show (b3|bip) statistics",
	   SHOW_STR
	   "b3 statistics\n"
	   "bip statistics\n"
	   "statistics\n")

/* clear sdhvcg bip/b3 cnt */
DEFSH ( VTYSH_HAL,
        clear_b3_bip_statistics_cmd_vtysh,
       "(b3|bip) statistics clear",
       "b3 statistics\n"
	   "bip statistics\n"
       "statistics\n"
       "clear statistics\n")

/* show sdhvcg channel cnt */
DEFSH ( VTYSH_HAL,
        vcg_statistics_cmd_vtysh,
        "show sdhvcg statistics",
	    SHOW_STR
       "sdhvcg\n"
       "statistics\n")

/* clear sdhvcg channel cnt */
DEFSH ( VTYSH_HAL,
        clear_vcg_statistics_cmd_vtysh,
        "sdhvcg statistics clear",
        "sdhvcg\n"
        "statistics\n"
        "clear statictics\n")

/* show sdhvcg lp status */
DEFSH ( VTYSH_HAL,
        show_lp_status_cmd_vtysh,
        "show lp status",
	    SHOW_STR
	    "lp\n"
        "lp status\n")

/* show sdhvcg current interface status */
DEFSH ( VTYSH_HAL,
        show_vcg_status_cmd_vtysh,
        "show sdhvcg status",
        SHOW_STR
        "sdhvcg\n"
        "sdhvcg status\n")

/* show sdhvcg all interface status */
DEFSH( VTYSH_HAL,
       show_vcg_interface_status_cmd_vtysh,
       "show interface vcg status {sdhvcg USP}",
        SHOW_STR
        CLI_INTERFACE_STR
        "vcg\n"
        "status info\n"
        CLI_INTERFACE_VCG_STR
        CLI_INTERFACE_VCG_VHELP_STR)


DEFSH( VTYSH_HAL,
       hal_sfp_set_rx_alarm_warn_cmd_vtysh,
	   "sfp rx power high_alarm <-40-8> low_alarm <-40-8> high_warn <-40-8> low_warn <-40-8>",
	   "Sfp rx power set\n"
	   "High alarm range\n"
	   "Low alarm range\n"
	   "High warn range\n"
	   "Low warn range\n" 
     )
     
DEFSH( VTYSH_HAL,
       hal_sfp_set_tx_alarm_warn_cmd_vtysh,
       "sfp tx power high_alarm <-40-8> low_alarm <-40-8> high_warn <-40-8> low_warn <-40-8>",
       "Sfp tx power set\n"
       "High alarm range\n"
       "Low alarm range\n"
       "High warn range\n"
       "Low warn range\n"
     )

DEFSH ( VTYSH_HAL,
        debug_hal_ip_pps_cmd,
        "ip pps <0-65535>" ,
        "ip\n"
        "pps\n"
        "<0-65535>\n")

DEFSH ( VTYSH_HAL,
        debug_hal_eth_pps_cmd,
        "eth pps <0-65535>" ,
        "eth\n"
        "pps\n"
        "<0-65535>\n")

DEFSH ( VTYSH_HAL,
        debug_hal_outbound_lr_cmd,
        "outbound limit-rate <0-10000>" ,
        "outbound\n"
        "limit-rate\n"
        "<0-10000> kbps\n")

DEFSH ( VTYSH_HAL,
       hal_devm_write_eeprom_cmd_vtysh,
       "write slot <0-32> eeprom sys WORD",
       "write file \n"
       "slot number \n"
       "eeprom \n" 
       "sys direction \n"
       "file name \n")

DEFSH ( VTYSH_HAL,
       erase_slot_user_epcs_cmd_vtysh,
       "erase slot <0-32> user",
       "erase epcs \n"
       "slot number \n"
       "user \n")
       
DEFSH ( VTYSH_HAL,
        debug_hal_stg_special_cmd_vtysh,
        "debug hal special stg (forward|block|disable) interface (ethernet|gigabitethernet|xgigabitethernet) USP",
        "Debug command\n"
        "Hal\n"
        "For HT158 vlan 4095\n"
        "Stg\n"
        "Forward\n"
        "Block\n"
        "Disable\n"
        "Interface\n"
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR)
        
DEFSH ( VTYSH_HAL,show_lp_alarm_cmd_vtysh,
		"show lp alarm USP",
		"show command\n"
		"low path\n"
		"low path alarm\n"
		"Usp format <0-7>/<0-31>/<0-255> of the interface\n")

DEFSH( VTYSH_HAL,
	show_interface_width_rate_cmd,
	"show width rate { interface ( gigabitethernet | xgigabitethernet ) USP} ",
	SHOW_STR
	"Width of interface\n"
	"Rate of width\n"
	CLI_INTERFACE_STR
	CLI_INTERFACE_GIGABIT_ETHERNET_STR
	CLI_INTERFACE_XGIGABIT_ETHERNET_STR
	"The port of the interface, format: <0-7>/<0-31>/<1-255>\n")

DEFSH (VTYSH_HAL,
	hal_debug_monitor_cmd,
	"debug hal (enable|disable) (acl|alarm|arp|bfd|cfm|oam|ces|devm|epcs|erp|fib|sys|gre|qos|clock|l2|l3|mpls|ndp|pkt|mac|ifm|all)",
	"Debug information to moniter\n"
	"Programe name hal\n"
	"Enable debug\n"
	"Disable debug\n"
	"Debug acl informations\n"
	"Debug alarm informations\n"
	"Debug arp informations\n"
	"Debug bfd informations\n"
	"Debug cfm informations\n"
	"Debug oam informations\n"
	"Debug ces informations\n"
	"Debug devm informations\n"
	"Debug epcs informations\n"
	"Debug erp informations\n"
	"Debug fib informations\n"
	"Debug system informations\n"
	"Debug gre informations\n"
	"Debug qos informations\n"
	"Debug clock informations\n"
	"Debug l2 informations\n"
	"Debug l3 informations\n"
	"Debug mpls informations\n"
	"Debug ndp informations\n"
	"Debug pkt informations\n"
	"Debug mac informations\n"
	"Debug ifm informations\n"
	"Debug all informations\n")


DEFSH (VTYSH_HAL,
	show_hal_debug_monitor_cmd,
	"show hal debug",
	SHOW_STR
	"Hal"
	"Debug status\n")

DEFSH ( VTYSH_HAL,
      show_hal_vlan_translate_cmd_vtysh,
      "show vlan translate-entry",
      "Show command\n"
      "vlan\n"
      "vlan translate or qinq entry\n")
      
DEFSH ( VTYSH_HAL,
      show_hal_mac_static_all_cmd_vtysh,
      "show hal mac static",
      "Show static mac information\n"
      "Mac\n"
      "Static mac \n")

void vtysh_init_hal_cmd()
{
	install_element (CONFIG_NODE, &hw_rw_cmd, CMD_LOCAL );
    //install_element ( CONFIG_NODE, &show_hal_nhp_cmd );
    //install_element ( CONFIG_NODE, &show_hal_ecmp_cmd );
    install_element_level ( CONFIG_NODE, &show_hal_ilm_cmd,MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_nhlfe_cmd,MONITOR_LEVE_2, CMD_LOCAL );
    install_element ( CONFIG_NODE, &show_hal_ifm_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_ethernet_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_tdm_cmd_vtysh, CMD_LOCAL);
	//install_element ( CONFIG_NODE, &show_hal_ifm_stm_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_vlanif_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_trunk_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_loopback_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_tunnel_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_clock_cmd_vtysh, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_gigabit_ethernet_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_xgigabit_ethernet_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_ethernetlist_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_ifvlan_trunk_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_ifvlan_ethernet_cmd, CMD_LOCAL);

    
	install_element(CONFIG_NODE, &show_hal_timerinfo_cmd_vty, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_hal_mplsif_trunk_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_hal_mplsif_ethernet_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_hal_mplsif_gigabit_ethernet_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_hal_mplsif_xgigabit_ethernet_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_hal_ifm_ifvlan_all_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &debug_hal_ifm_log_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, &show_l2cp_interface_vtysh, CMD_LOCAL);
    install_element_level ( CONFIG_NODE, &show_hal_fib_all_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_fib_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_fibv6_all_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_fibv6_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_pw_cmd,MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_vsi_cmd,MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_tunnel_cmd,MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &config_arp_anti_smac_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &no_config_arp_anti_smac_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &config_arp_anti_sip_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &no_config_arp_anti_sip_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &config_arp_anti_vlan_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &no_config_arp_anti_vlan_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &config_arp_anti_ifidx_eth_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &config_arp_anti_ifidx_geth_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &config_arp_anti_ifidx_xgeth_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level ( CONFIG_NODE, &show_hal_arp_anti_acl_cmd, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level ( CONFIG_NODE, &show_hal_all_arp_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &show_hal_ipv6_ndp_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &show_hal_pbr_acl_cmd, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level ( CONFIG_NODE, &show_hal_all_acl_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &show_hal_qos_car_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &show_hal_qos_mapping_domain_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &show_hal_qos_mapping_phb_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &show_hal_hqos_wred_profile_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &show_hal_hqos_queue_profile_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &debug_hal_bfd_table_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &debug_hal_bfd_global_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &debug_hal_bfd_session_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &show_interface_width_rate_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	

    install_element ( CONFIG_NODE, &show_hal_mplstp_oam_session_cmd, CMD_LOCAL);
    install_element ( CONFIG_NODE, &debug_hal_mplstp_oam_session_lm_in_in_cmd, CMD_LOCAL);
    install_element ( CONFIG_NODE, &debug_hal_mplstp_oam_session_lm_in_de_cmd, CMD_LOCAL);
    install_element ( CONFIG_NODE, &debug_hal_mplstp_oam_session_lm_de_in_cmd, CMD_LOCAL);
    install_element ( CONFIG_NODE, &debug_hal_mplstp_oam_session_lm_de_de_cmd, CMD_LOCAL);

	install_element_level ( CONFIG_NODE, &hal_mac_show_statics_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level ( CONFIG_NODE, &hal_mac_show_all_cmd, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level ( CONFIG_NODE, &hal_mac_show_eth_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &hal_mac_show_trunk_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &hal_mac_show_vlan_cmd, MONITOR_LEVE_2, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &hal_mac_show_vsi_cmd, MONITOR_LEVE_2, CMD_LOCAL );
	install_element_level ( CONFIG_NODE, &hal_mac_show_dynamic_cmd, MONITOR_LEVE_2, CMD_LOCAL);	//added by liubo 2018-3-15 for Q:53196
    install_element_level ( CONFIG_NODE, &hal_mac_clear_all_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level ( CONFIG_NODE, &hal_mac_clear_eth_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level ( CONFIG_NODE, &hal_mac_clear_trunk_cmd, CONFIG_LEVE_5, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &hal_mac_clear_vlan_cmd, CONFIG_LEVE_5, CMD_LOCAL );
    install_element_level ( CONFIG_NODE, &hal_mac_clear_vsi_cmd, CONFIG_LEVE_5, CMD_LOCAL);

	install_element_level ( CONFIG_NODE, &hal_sfp_show_all_cmd, MONITOR_LEVE_2,CMD_LOCAL );
    install_element_level ( PHYSICAL_IF_NODE, &hal_sfp_show_cmd,MONITOR_LEVE_2,CMD_LOCAL );
    install_element_level ( STM_IF_NODE, &hal_sfp_show_cmd,MONITOR_LEVE_2,CMD_LOCAL  );

    
	install_element (CONFIG_NODE, &hal_btb_buff_infor_cmd_vtysh, CMD_LOCAL);
        
	install_element (CONFIG_NODE, &hal_log_level_ctl_cmd_vtysh, CMD_LOCAL);
	install_element (CONFIG_NODE, &hal_show_log_level_ctl_cmd_vtysh, CMD_LOCAL);

	install_element_level ( CONFIG_NODE, &debug_hal_sla_session_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &debug_hal_sla_config_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level ( CONFIG_NODE, &debug_hal_packet_cmd, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level ( CONFIG_NODE, &no_debug_hal_packet_cmd, MONITOR_LEVE_2, CMD_LOCAL);

	install_element(CONFIG_NODE, &debug_hal_stg_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_stg2_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_stg_show_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_tx_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_tx_gigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_tx_xgigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_enable_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_enable_gigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_force_enable_xgigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_optical_tx_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_optical_tx_gigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_optical_tx_xgigabit_eth_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_hal_bitmap_show_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_hal_ifm_inter_cmd, CMD_LOCAL);

	//install_element_level(CONFIG_NODE, &debug_hal_slot_gre_mode_set_cmd, MANAGE_LEVE);
	install_element_level (CONFIG_NODE, &mpls_hqos_packet_len_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element ( CONFIG_NODE, & hal_cfm_alarm_enable_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, & hsl_bfd_info_show_cmd, CMD_LOCAL);
	install_element ( CONFIG_NODE, & hal_cfm_rcv_ccm_delay_cmd, CMD_LOCAL);

	install_element ( CONFIG_NODE, & hal_dev_led_test_cmd, CMD_LOCAL);
#if 0
	/*multicast group cmd*/
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_create_cmd, MANAGE_LEVE );
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_destroy_cmd, MANAGE_LEVE );
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_add_if_cmd, MANAGE_LEVE );
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_add_gigabit_eth_if_cmd, MANAGE_LEVE);
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_add_xgigabit_eth_if_cmd, MANAGE_LEVE);
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_delete_if_cmd, MANAGE_LEVE );
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_delete_gigabit_eth_if_cmd, MANAGE_LEVE);
	install_element_level ( CONFIG_NODE, &debug_hal_mcgroup_delete_xgigabit_eth_if_cmd, MANAGE_LEVE);

	/*ip multicast cmd*/
	install_element_level ( CONFIG_NODE, &debug_hal_mfib_add_cmd, MANAGE_LEVE );
	install_element_level ( CONFIG_NODE, &debug_hal_mfib_gigabit_eth_add_cmd, MANAGE_LEVE);
	install_element_level ( CONFIG_NODE, &debug_hal_mfib_xgigabit_eth_add_cmd, MANAGE_LEVE);
	install_element_level ( CONFIG_NODE, &debug_hal_mfib_delete_cmd, MANAGE_LEVE );
#endif

	install_element_level(CONFIG_NODE, &hal_bfd_global_group_flag_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &hal_bfd_no_global_group_flag_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &hal_ifm_ifg_cmd_vtysh, ROOT_LEVE, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &hal_bfd_global_group_wtr_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &hal_bfd_global_priority_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &bfd_acl_ces_pw_label_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &no_bfd_acl_ces_pw_label_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &add_hal_bfd_session_detect_num_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &hal_bfd_detect_num_flag_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &hal_bfd_event_proc_time_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &hal_qos_domain_hwid_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &hal_qos_phb_hwid_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &debug_hal_mspw_add_swap_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &no_debug_hal_mspw_add_swap_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &acl_mspw_pw_label_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &no_acl_mspw_pw_label_cmd, CONFIG_LEVE_5, CMD_SYNC);

    install_element_level(PHYSICAL_IF_NODE, &hal_sfp_set_tx_alarm_warn_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(PHYSICAL_IF_NODE, &hal_sfp_set_rx_alarm_warn_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);	 

	install_element_level(CONFIG_NODE, &hal_bfd_fpga_detection_time_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &hal_bfd_fpga_delay_time_cmd, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level(CONFIG_NODE, &hal_bfd_fpga_enable_cmd, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level(VCG_NODE, &show_lcas_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &show_oh_jx_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &show_oh_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &b3_bip_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &clear_b3_bip_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level(VCG_NODE, &vcg_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &clear_vcg_statistics_cmd_vtysh, MONITOR_LEVE_2, CMD_SYNC);
	install_element_level(VCG_NODE, &show_lp_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(VCG_NODE, &show_vcg_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_vcg_interface_status_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level(CONFIG_NODE, &debug_hal_ip_pps_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &debug_hal_eth_pps_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &debug_hal_outbound_lr_cmd, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(SYSRUN_NODE, &hal_devm_write_eeprom_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(SYSRUN_NODE, &erase_slot_user_epcs_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
 
    install_element_level(CONFIG_NODE, &debug_hal_stg_special_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
    install_element_level(CONFIG_NODE, &show_lp_alarm_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	
	install_element_level(CONFIG_NODE, &hal_debug_monitor_cmd, CONFIG_LEVE_5, CMD_LOCAL);
    install_element_level(CONFIG_NODE, &show_hal_debug_monitor_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_hal_vlan_translate_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level(CONFIG_NODE, &show_hal_mac_static_all_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
}

