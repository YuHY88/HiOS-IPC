#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node rmon_node =
{
	RMON_NODE,
	"%s(config-rmon)#",
};


DEFUNSH(VTYSH_RMON,
        rmon_mode_enable,
        rmon_mode_enable_cmd,
        "rmon",
        "rmon command node\n")
{
    vty->node = RMON_NODE;

    return CMD_SUCCESS;
}

DEFUNSH(VTYSH_RMON,
        vtysh_exit_rmon,
        vtysh_exit_rmon_cmd,
        "exit",
        "Exit current mode and down to previous mode\n")
{
    return vtysh_exit(vty);
}

DEFSH (VTYSH_RMON,
       rmon_if_collection_stats_cmd,
       "rmon collection (ethernet|gigabitethernet|xgigabitethernet) IFNAME statistics <1-100> {owner WORD}",
       "rmon\n"
       "Collection \n"
       "ethernet\n"
       "gigabitethernet\n"
       "xgigabitethernet\n"
       "interface name\n"
       "Ethernet Statistics\n"
       "Stat Index\n"
       "RMON Owner Identity\n"
       "rmon stat owner name max length 126\n");

DEFSH (VTYSH_RMON,     
     rmon_clear_if_counters_cmd,
     "rmon clear counters",
     "rmon\n"
     "Clear Counters\n"
     "counters\n") ;

DEFSH(VTYSH_RMON,
          no_rmon_if_collection_stats_cmd,
          "no rmon collection statistics <1-100> ",
          "no\n"
          "rmon\n"
          "collection \n"          
          "ethernet statistics\n"
          "statistics index\n") ;


DEFSH (VTYSH_RMON,
     rmon_if_collection_history_cmd,
//     "rmon collection interface IFNAME history <1-300> (buckets <1-50>|) (interval <1-3600>|) (owner WORD|)",
     "rmon collection (ethernet|gigabitethernet|xgigabitethernet) IFNAME history <1-300> buckets <1-50> interval <1-86400> {owner WORD} ",
     "rmon\n"
     "Collection \n"      
     "ethernet\n"
     "gigabitethernet\n"
     "xgigabitethernet\n"
     "Interface name\n"
     "history commands\n"
     "history Index\n"
     "buckets (default 50)\n"
     "buckets number\n"
     "polling Interval (default 1800)\n"
     "Seconds\n"
     "owner identity\n"
     "owner name max length 126\n") ;

DEFSH (VTYSH_RMON,
     no_rmon_if_collection_history_cmd,
     "no rmon collection history <1-300> ",
     "No\n"
     "Rmon\n"
     "Collection\n "
     "history commands\n"
     "history Index\n") ;


DEFSH (VTYSH_RMON,
        rmon_event_log_cmd ,
	"rmon event <1-1200> (log|trap|log-trap) description WORD {community WORD | owner WORD}",
	"rmon\n"
	"event command\n"
	"event Index\n"
	"log the event\n"
	"trap the event\n"
	"log and trap the event\n"
        "event description\n"
        "description string max length 126\n"
	"event community\n"
	"community string max length 126\n"	
	"owner identity\n"
	"owner name max length 126\n") ;

DEFSH (VTYSH_RMON,
     no_rmon_event_cmd,
     "no rmon event <1-1200> ",
     "No\n"
     "Rmon\n"
     "event commands\n"
     "event Index\n") ;


DEFSH (VTYSH_RMON,
      rmon_alarm_cmd,
      "rmon alarm <1-600> statistics <1-100> type  (etherStatsDropEvents | etherStatsOctets"
      " | etherStatsPkts | etherStatsBroadcastPkts | etherStatsMulticastPkts | etherStatsCRCAlignErrors"
      " | etherStatsUndersizePkts | etherStatsOversizePkts | etherStatsFragments | etherStatsJabbers"
      " | etherStatsCollisions | etherStatsPkts64Octets | etherStatsPkts65to127Octets | etherStatsPkts128to255Octets"
      " | etherStatsPkts256to511Octets | etherStatsPkts512to1023Octets | etherStatsPkts1024to1518Octets"
      " | etherStatsUnicastPkts | etherStatsErrorPkts | etherStatsLossPkts | etherStatsTxOctets | etherStatsTxPkts"
      " | etherStatsTxUnicastPkts | etherStatsTxBroadcastPkts | etherStatsTxMulticastPkts | etherStatsTxErrorPkts | etherStatsTxLossPkts)"
      " interval <1-86400> (delta | absolute) "
      " rising-threshold <1-2147483647> event <1-1200> falling-threshold <1-2147483647>"
      " event <1-1200> alarmstartup (rising|falling|risingAndFalling) {owner WORD}",
      "rmon\n"
      "alarm command \n"
      "alarm Index\n"
      "ethernet Statistics\n"
      "ethernet Statistics Index\n"
      "type\n"
      "Ethernet Statistics DropEvents\n"
      "Ethernet Statistics Receive Octets\n"
      "Ethernet Statistics Receive Packets\n"
      "Ethernet Statistics Receive Broadcast Packets\n"
      "Ethernet Statistics Receive Multicast Packets\n"
      "Ethernet Statistics CRC Align Errors\n"
      "Ethernet Statistics UnderSize Packets\n"
      "Ethernet Statistics OverSize Packets\n"
      "Ethernet Statistics Fragments\n"
      "Ethernet Statistics Jabbers\n"
      "Ethernet Statistics Collisions\n"
      "Ethernet Statistics Receive 64 Octets Packets\n"
      "Ethernet Statistics Receive 65 ~ 127 Octets Packets\n"
      "Ethernet Statistics Receive 128 ~ 255 Octets Packets\n"
      "Ethernet Statistics Receive 256 ~ 511 Octets Packets\n"
      "Ethernet Statistics Receive 512 ~ 1023 Octets Packets\n"
      "Ethernet Statistics Receive 1204 ~ 1518 Octets Packets\n"
      "Ethernet Statistics Receive Unicast Packets\n"
      "Ethernet Statistics Receive error Packets\n"
      "Ethernet Statistics In Discards Packets\n"
      "Ethernet Statistics Sent Octets\n"
      "Ethernet Statistics Sent Packets\n"
      "Ethernet Statistics Sent Unicast Packets\n"
      "Ethernet Statistics Sent Broadcast Packets\n"
      "Ethernet Statistics Sent Multicast Packets\n"
      "Ethernet Statistics Sent Error Packets\n"
      "Ethernet Statistics Out discards Packets\n"
      "alarm interval\n"
      "seconds\n"
      "delta\n"
      "absolute\n"
      "alarm rising threshold\n"
      "rising threshold value  <1 to 2147483647>\n"
      "event for rising alarm\n"
      "event index\n"
      "alarm falling threshold\n"
      "falling threshold value <1 to 2147483647>\n"
      "event for falling alarm\n"
      "event index\n"
      "alarm startup type\n"
      "rising\n"
      "falling\n"
      "rising and falling\n"
      "owner identity\n"
      "owner name max length 126\n");

DEFSH (VTYSH_RMON,
     no_rmon_alarm_cmd,
     "no rmon alarm <1-600> ",
     "No\n" 
     "Rmon\n"
     "alarm commands\n"
     "alarm Index\n") ;


DEFSH(VTYSH_RMON,
      show_rmon_spec_if_stats_cmd,
      "show rmon statistics <1-100>",
      "show\n"
      "rmon\n"
      "statistics\n"
      "statistics index\n") ;     

DEFSH (VTYSH_RMON,
	show_rmon_if_stats_cmd,
	"show rmon statistics",
	"show\n"
	"rmon\n"
	"statistics\n") ;


DEFSH (VTYSH_RMON,
	show_rmon_if_base_stats_cmd,
	"show rmon basestats",
	"Show\n"
	"Rmon\n"
	"basestats\n") ;


DEFSH (VTYSH_RMON,
	show_rmon_if_phy_stats_cmd,
	"show rmon phystats",
	"Show\n"
	"Rmon\n"
	"phystats\n") ;

DEFSH (VTYSH_RMON,
	show_rmon_history_ctrl_cmd,
	"show rmon history control",
	"Show\n"
	"Rmon\n"
	"history\n"
	"history control\n") ;

DEFSH (VTYSH_RMON,
	show_rmon_history_data_cmd,
	"show rmon history data  hist-control-table-index <1-300>",
	"Show\n"
	"Rmon\n"
	"history\n"
	"history data\n"
	"history collection control index\n"
	"history collection control index value\n") ;

DEFSH (VTYSH_RMON,
	show_rmon_event_cmd,
	"show rmon event",
	"Show\n"
	"Rmon\n"
	"event\n") ;

DEFSH (VTYSH_RMON,
	show_rmon_alarm_cmd,
	"show rmon alarm",
	"Show\n"
	"Rmon\n"
	"alarm\n") ;

DEFSH(VTYSH_RMON, rmon_log_level_ctl_cmd_vtysh, "debug rmon (enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n");

DEFSH(VTYSH_RMON, rmon_show_log_level_ctl_cmd_vtysh, "show debug rmon",
	SHOW_STR "Output log of debug level\n""Program name\n");


void vtysh_init_rmond_cmd() 
{
	install_node(&rmon_node, NULL);
	vtysh_install_default(RMON_NODE);
	
	install_element_level (CONFIG_NODE, &rmon_mode_enable_cmd, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (RMON_NODE, &vtysh_exit_rmon_cmd, CONFIG_LEVE_5,CMD_SYNC);		

	install_element_level (RMON_NODE, &rmon_clear_if_counters_cmd, CONFIG_LEVE_5,CMD_SYNC);		
	install_element_level (RMON_NODE, &rmon_if_collection_stats_cmd, CONFIG_LEVE_5,CMD_SYNC);	    
	install_element_level (RMON_NODE, &no_rmon_if_collection_stats_cmd, CONFIG_LEVE_5,CMD_SYNC);	
	
	install_element_level (RMON_NODE, &rmon_if_collection_history_cmd, CONFIG_LEVE_5,CMD_SYNC);
	install_element_level (RMON_NODE, &no_rmon_if_collection_history_cmd, CONFIG_LEVE_5,CMD_SYNC);	
		
	install_element_level (RMON_NODE, &rmon_event_log_cmd, CONFIG_LEVE_5,CMD_SYNC);		
    
	install_element_level (RMON_NODE, &no_rmon_event_cmd, CONFIG_LEVE_5,CMD_SYNC);	

	install_element_level (RMON_NODE, &rmon_alarm_cmd, CONFIG_LEVE_5,CMD_SYNC);		
	install_element_level (RMON_NODE, &no_rmon_alarm_cmd, CONFIG_LEVE_5,CMD_SYNC);	
	
	install_element_level (RMON_NODE, &show_rmon_if_stats_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (RMON_NODE, &show_rmon_spec_if_stats_cmd, CONFIG_LEVE_5, CMD_LOCAL);

    install_element_level (RMON_NODE, &show_rmon_if_phy_stats_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (RMON_NODE, &show_rmon_history_ctrl_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (RMON_NODE, &show_rmon_history_data_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (RMON_NODE, &show_rmon_event_cmd, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (RMON_NODE, &show_rmon_alarm_cmd, CONFIG_LEVE_5, CMD_LOCAL);	

	install_element_level (CONFIG_NODE, &rmon_log_level_ctl_cmd_vtysh, MANAGE_LEVE,CMD_SYNC); 
	install_element_level (CONFIG_NODE, &rmon_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE,CMD_SYNC);
}

