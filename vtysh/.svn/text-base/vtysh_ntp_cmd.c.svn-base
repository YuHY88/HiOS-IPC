/**
 * \page cmds_ref_ntp NTP
 * - \subpage modify_log_ntp
 * 
 */
 
/**
 * \page modify_log_ntp Modify Log
 * \section ntp-v007r004 HiOS-V007R004
 *  -# 
 * \section ntp-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node stNtpNode = { NTP_NODE,  "%s(config-ntp)#",  1 };



DEFSH (VTYSH_NTP,ntp_log_level_ctl_cmd_vtysh,
    "debug ntp (enable|disable) (common|event|info|package|error|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common messege\n"
	"Type name of event messege\n"
	"Type name of info messege\n"
	"Type name of package messege \n"
	"Type name of error messege \n"
	"Type name of all debug\n")
 

DEFSH (VTYSH_NTP,ntp_show_log_level_ctl_cmd_vtysh,"show debug ntp",		
	SHOW_STR"Output log of debug level\n""Program name\n")	


DEFUNSH (VTYSH_NTP,
         vtysh_exit_ntp,
         vtysh_exit_ntp_cmd,
         "exit",
         "Exit current mode and down to previous mode\n"
         )
{
    return vtysh_exit(vty);
}

DEFUNSH (VTYSH_NTP,
         ntp_name,
         ntp_name_cmd,
         "ntp",
         "Enter the ntp view\n")
{
    vty->node = NTP_NODE;

    return CMD_SUCCESS;
}

DEFSH (VTYSH_NTP,
       show_ntp_cmd_vtysh,
       "show ntp config",
   	   "show command\n"
   	   "ntp command\n"
	   "show ntp cofig information\n"
       )
       
DEFSH (VTYSH_NTP,
       show_ntpstatus_cmd_vtysh,
       "show ntp status",
       "show command\n"
   	   "ntp command\n"
	   "show the status of ntp protocol\n"
       )

DEFSH (VTYSH_NTP,
       show_ntpstatistics_cmd_vtysh,
       "show ntp statistics",
       "show command\n"
   	   "ntp command\n"
	   "show ntp message statistics\n"
       )
       
DEFSH (VTYSH_NTP,
	   set_ntp_cmd_vtysh,
	   "ntp client (enable|disable)",
       "ntp command\n"
       "set ntp client enable or disable\n" 
       "enable ntp\n" 
       "disable ntp\n"
	   )
	  
DEFSH (VTYSH_NTP,
   	   set_ntpserver_cmd_vtysh,
   	   "ntp (server|peer) A.B.C.D [vpn-instance <1-128>]",
       "ntp command\n"
       "set ntp server ip address\n"
       "set ntp peer ip address\n"
       "input host ipv4 address\n"
       "L3 vpn instance, only for log server\n" 
       "L3 vpn tag\n"
   	  )

DEFSH (VTYSH_NTP,
       no_set_ntpserver_cmd_vtysh,
       "no ntp (server|peer)",
       NO_STR
       "ntp command\n"
       "unset ntp server ip address\n"
       "unset ntp peer ip address\n"
      )

DEFSH (VTYSH_NTP,
       ntp_mode_cmd_vtysh,
       "ntp mode (p2p|client-server)",
       "ntp command\n"
       "set ntp working mode\n"
       "peer mode\n"
       "client-server mode\n"
       )

DEFSH (VTYSH_NTP,
       no_ntp_mode_cmd_vtysh,
       "no ntp mode",
       NO_STR
       "ntp command\n"
       "unset ntp working mode\n"
       )

DEFSH (VTYSH_NTP,
       ntp_packet_cmd_vtysh,
       "ntp packet (unicast|broadcast)",
       "ntp command\n"
       "set ntp message format\n"
       "unicast format message\n"
       "broadcast format message\n"
       )

DEFSH (VTYSH_NTP,
       no_ntp_packet_cmd_vtysh,
       "no ntp packet",
       NO_STR
       "ntp command\n"
       "unset ntp message format\n"
       )

DEFSH (VTYSH_NTP,
       ntp_syncinterval_cmd_vtysh,
       "ntp sync-interval <1-1000>",
       "ntp command\n"
       "set ntp synchronization time interval\n"
       "the value of synce-interval(second)\n"
       )

DEFSH (VTYSH_NTP,
       no_ntp_syncinterval_cmd_vtysh,
       "no ntp sync-interval",
       NO_STR
       "ntp command\n"
       "unset ntp synchronization time interval\n"
       )
       
DEFSH (VTYSH_NTP,
       ntp_version_cmd_vtysh,
       "ntp version <1-4>",
       "ntp command\n"
       "set ntp version\n"
       "the value of version\n"
       )

DEFSH (VTYSH_NTP,
       no_ntp_version_cmd_vtysh,
       "no ntp version",
       NO_STR
       "ntp command\n"
       "unset ntp version\n"
      )

void
vtysh_init_ntp_cmd (void)
{
    install_node (&stNtpNode, NULL);
    vtysh_install_default (NTP_NODE);
    install_element_level (NTP_NODE, &vtysh_exit_ntp_cmd,VISIT_LEVE,CMD_SYNC);
    
    install_element_level (CONFIG_NODE, &ntp_name_cmd,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (CONFIG_NODE, &show_ntp_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ntpstatus_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &show_ntpstatistics_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	
    install_element_level (NTP_NODE, &ntp_mode_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &no_ntp_mode_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &ntp_packet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &no_ntp_packet_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &ntp_syncinterval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &no_ntp_syncinterval_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &ntp_version_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &no_ntp_version_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
    install_element_level (NTP_NODE, &set_ntp_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (NTP_NODE, &set_ntpserver_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (NTP_NODE, &no_set_ntpserver_cmd_vtysh,CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (NTP_NODE, &show_ntp_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (NTP_NODE, &show_ntpstatus_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	install_element_level (NTP_NODE, &show_ntpstatistics_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);

	install_element_level (CONFIG_NODE, &ntp_log_level_ctl_cmd_vtysh,CONFIG_LEVE_5,CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ntp_show_log_level_ctl_cmd_vtysh,MONITOR_LEVE_2,CMD_LOCAL);
	return;
}

