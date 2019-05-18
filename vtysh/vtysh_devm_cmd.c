/**
 * \page cmds_ref_devm DEVM
 * - \subpage modify_log_devm
 * 
 */
 
/**
 * \page modify_log_devm Modify Log
 * \section devm-v007r004 HiOS-V007R004
 *  -# 
 * \section devm-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"

static struct cmd_node devm_node =
{
  DEVM_NODE,
  "%s(config-devm)# "
};

DEFUNSH (VTYSH_DEVM,
		devm_node_inter,
		devm_node_inter_cmd,
		"device",
		"inter devm node\n"
		)
{
	vty->node = DEVM_NODE;
	return CMD_SUCCESS;
}

DEFUNSH (VTYSH_DEVM,
	 vtysh_exit_devm,
	 vtysh_exit_devm_cmd,
	 "exit",
	 "Exit current mode and down to previous mode\n")
{
  return vtysh_exit (vty);
}

ALIAS (vtysh_exit_devm,
       vtysh_quit_devm_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")


DEFSH(VTYSH_DEVM,set_slotn_power_cmd_vtysh,
	"set slot <1-32> power (up|down)",
	"set devm para\n"
	"shelf equ slot num\n"
	"slot range from 1 to 32\n"
	"power cmd control slot power\n"
	"set slotn power up\n"
	"set slotn power down\n")


DEFSH(VTYSH_DEVM,set_temperature_limit_cmd_vtysh,
	"set temperature (upper|lower) limit <-50-100>",
	"set devm para\n"
	"the temperature limit of device\n"
	"the top of working temperature\n"
	"the bottom of wroking temperature\n"
	"limit\n"
	"the range in which we can set temperature,box equ work temperature <-10~55>, shelf equ work temperate <0~45>\n")	


DEFSH(VTYSH_DEVM,set_dynamic_voltage_cmd_vtysh,
	"set power <1-2> dynamic voltage (enable|disable)",
	"set devm para\n"
	"which power you want to set\n"
	"power range from 1 to 2\n"
	"read dynamic voltage from power,need power hardware support\n"
	"voltage from power\n"
	"open read dynamic voltage and voltage alarm\n"
	"dclose read dynamic voltage and voltage alarm\n")


DEFSH(VTYSH_DEVM,set_voltage_limit_cmd_vtysh,
	"set power <1-2> voltage (upper|lower) limit <1-242>",
	"set devm para\n"
	"which power you want to set"
	"power range from 1 to 2"
	"the voltage limit of device\n"
	"the top of working voltage\n"
	"the bottom of wroking voltage\n"
	"limit\n"
	"the range in which we can set voltage from 1 to 242\n")


DEFSH(VTYSH_DEVM,hal_cmd_show_slot_hash_cmd_vtysh,
	"hal-cmd show slot <1-32> hash",
	"show or set hal devm commond\n"
	"show devm para\n"
	"which slot\n"
	"slot range from 1 to 32\n"
	"slot plug status hash info\n")


DEFSH(VTYSH_DEVM,hal_cmd_show_global_variables_cmd_vtysh,
	"hal-cmd show global variables",
	"show or set hal devm commond\n"
	"show devm para\n"
	"global info\n"
	"variables info\n")
	

DEFSH(VTYSH_DEVM,hal_cmd_show_slot_cmd_vtysh,
	"hal-cmd show slot [<1-32>]",
	"show or set hal devm commond\n"
	"show devm para\n"
	"which slot\n"
	"slot range from 1 to 32\n")


DEFSH(VTYSH_DEVM,hal_cmd_set_hash_add_slot_cmd_vtysh,
	"hal-cmd set hash add slot <1-32>",
	"hal-cmd  show or set hal devm commond\n"
	"set  change hal hash\n"
	"hash slot plug status hash info hash\n"
	"insert add slot bucket to hash\n"
	"slot slot choice <1-32>\n"
	"<1-32> slot range <1-32>\n")


DEFSH(VTYSH_DEVM,hal_cmd_set_hash_del_slot_cmd_vtysh,
	"hal-cmd set hash delete slot <1-32>",
	"show or set hal devm commond\n"
	"change hal hash\n"
	"slot plug status hash info hash\n"
	"insert del slot bucket to hash\n"
	"which slot\n"
	"slot range from1 to 32\n")


DEFSH(VTYSH_DEVM,hal_cmd_set_slot_fpga_reconfig_cmd_vtysh,
	"hal-cmd set  slot <1-32> fpga-reconfig",
	"show or set hal devm commond\n"
	"set commond\n"
	"which slot to execute cmd\n"
	"slot range from 1 to 32\n"
	"fpga reconfig commond\n")

	
DEFSH(VTYSH_DEVM,devm_op_eeprom_info_cmd_vtysh,
	"devm (set|get|clear|erase) slot <0-32> eeprom (device|dev|hardware_version|oem|hver|vendor|slot_name|vnd|date|mac|1st_mac|hwcfg|macnum|ver|bom|sn|lic|cfginit|all) [WORD]",
	"show or set hal devm commond\n"
	"set commond\n"
	"set commond\n"
	"clear selected eeprom info str commond\n"
	"erase all the eeprom info commond\n"
	"box equ eeprom or shelf equ backplane eeprom\n"
	"slot 0 is box eeprom or shelf equ backplane eeprom, shelf card slot <1-32>\n"
	"eeprominfo\n"
	"dev_name info from eeprom\n"
	"oem_name info from eeprom\n"
	"hardware_version info from eeprom\n"
	"vendor info from eeprom\n"
	"slot_name info from eeprom\n"
	"mac info from eeprom\n"
	"ID info from eeprom\n"
	"mac_num info from eeprom\n"
	"all info from this eeprom\n"
	"the info user want to set")


DEFSH(VTYSH_DEVM,device_watchdog_interval_cmd_vtysh,
	"device watchdog interval <0-30>",
	"device command\n"
	"set watchdog config\n"
	"watchdog feed interval\n"
	"0 means no watchdog\n")

DEFSH(VTYSH_DEVM,device_heart_beat_interval_cmd_vtysh,
	"device heart-beat  interval <0-255>",
	"device command\n"
	"heart-beat\n"
	"interval\n"
	"para range from 0 to 255, 0 means no heart beat\n")

DEFSH(VTYSH_DEVM,show_slot_rtime_cmd_vtysh,
	"show slot run-time [<1-32>]",
	"show command\n"
	"which slot\n"
	"total run-time\n"
	"slot range from 1 to 32>\n")

DEFSH(VTYSH_DEVM,show_slot_port_cmd_vtysh,
	"show slot port [<1-32>]",
	"show command\n"
	"which slot\n"
	"which port\n"
	"slot range from 1 to 32>\n")


DEFSH(VTYSH_DEVM,show_slot_mac_cmd_vtysh,
	"show slot mac [<1-32>]",
	"show command\n"
	"which slot\n"
	"which mac\n"
	"slot range from 1 to 32>\n")

DEFSH(VTYSH_DEVM,show_slot_version_cmd_vtysh,
	"show slot version [<1-32>]",
	"show command\n"
	"which slot\n"
	"version of slot\n"
	"slot range from 1 to 32>\n")

DEFSH(VTYSH_DEVM,show_slot_type_cmd_vtysh,
	"show slot type [<1-32>]",
	"show command\n"
	"which slot\n"
	"type of slot\n"
	"slot range from 1 to 32>\n")

	
DEFSH(VTYSH_DEVM,reboot_slot_all_cmd_vtysh,
	"reboot slot all",
	"reboot command\n"
	"which slot\n"
	"all slot\n")


//DEFSH(VTYSH_DEVM,reset_master_cmd_vtysh,
//	"reset master",
//	"reset command\n"
//	"master\n")



DEFSH(VTYSH_DEVM,reboot_slot_cmd_vtysh,
	"reboot slot <1-32>",
	"reboot command\n"
	"which slot\n"
	"slot range from 1 to 32\n")


DEFSH(VTYSH_DEVM,setmx_m2s_cmd_vtysh,
	"setmx m2s",
	"shelf equ set its mainboard status\n"
	"switch slave to master\n")

DEFSH(VTYSH_DEVM,reset_slot_cmd_vtysh,
	"reset slot <1-32>",
	"reset card\n"
	"which slot to execute cmd\n"
	"slot range is  from 1 to 32\n")

DEFSH(VTYSH_DEVM,show_dev_slot_all_info_cmd_vtysh,
	"show slot all",
	"show  command\n"
	"which slot\n"
	"all slot\n")

DEFSH(VTYSH_DEVM,show_dev_slot_info_cmd_vtysh,
	"show slot <1-32>",
	"show  command\n"
	"which slot\n"
	"slot range is  from 1 to 32\n")

DEFSH(VTYSH_DEVM,show_dev_backplane_cmd_vtysh,
	"show device backplane",
	"show  command\n"
	"device info\n"
	"backplane info\n")

DEFSH(VTYSH_DEVM,show_dev_unit_cmd_vtysh,
	"show device unit",
	"show  command\n"
	"device info\n"
	"unit of the equipment\n")

DEFSH(VTYSH_DEVM,show_dev_power_cmd_vtysh,
	"show device power",
	"show  command\n"
	"device info\n"
	"power of the unit\n")

DEFSH (VTYSH_DEVM,show_dev_fan_cmd_vtysh,
	"show device fan",
	"show  command\n"
	"device info\n"
	"fan info of the unit\n")

DEFSH (VTYSH_DEVM,show_dev_temp_cmd_vtysh,
	"show device temperature {slot <1-32>}",
	"show  command\n"
	"device info\n"
	"temperature info\n"
	"which slot\n"
	"slot range from 1 to 32\n")
 

/* debug 日志发送到 syslog 使能状态设置 */
DEFSH (VTYSH_DEVM,devm_log_level_ctl_cmd_vtysh,"debug devm(enable | disable)",		
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")	
 
/* debug 日志发送到 syslog 使能状态显示 */
DEFSH (VTYSH_DEVM,devm_show_log_level_ctl_cmd_vtysh,"show debug devm",		
	SHOW_STR"Output log of debug level\n""Program name\n")	




void vtysh_init_devm_cmd ()
{
	install_node (&devm_node, NULL);
	
	vtysh_install_default (DEVM_NODE);	

	install_element_level (CONFIG_NODE, &devm_node_inter_cmd, VISIT_LEVE, CMD_SYNC);	
	install_element_level (DEVM_NODE, &vtysh_exit_devm_cmd, VISIT_LEVE, CMD_SYNC);

	install_element_level (DEVM_NODE, &set_slotn_power_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &set_temperature_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (DEVM_NODE, &set_dynamic_voltage_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);
	install_element_level (DEVM_NODE, &set_voltage_limit_cmd_vtysh, CONFIG_LEVE_5, CMD_SYNC);

	install_element_level (DEVM_NODE, &hal_cmd_show_slot_hash_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &hal_cmd_show_global_variables_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &hal_cmd_show_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &hal_cmd_set_hash_add_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &hal_cmd_set_hash_del_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &hal_cmd_set_slot_fpga_reconfig_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (SYSRUN_NODE, &devm_op_eeprom_info_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	
	install_element_level (DEVM_NODE, &device_watchdog_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);	
	install_element_level (DEVM_NODE, &device_heart_beat_interval_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);	
	
	install_element_level (DEVM_NODE, &show_slot_rtime_cmd_vtysh,MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_slot_port_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_slot_mac_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_slot_version_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_slot_type_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	install_element_level (DEVM_NODE, &reboot_slot_all_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
 	install_element_level (DEVM_NODE, &reboot_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
//	install_element_level (DEVM_NODE, &reset_master_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (DEVM_NODE, &setmx_m2s_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);	
	install_element_level (DEVM_NODE, &reset_slot_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL); 
	
	install_element_level (DEVM_NODE, &show_dev_slot_all_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_dev_slot_info_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (DEVM_NODE, &show_dev_backplane_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_dev_unit_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (DEVM_NODE, &show_dev_power_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);	
	install_element_level (DEVM_NODE, &show_dev_fan_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);
	install_element_level (DEVM_NODE, &show_dev_temp_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL) ;

	install_element_level (CONFIG_NODE, &devm_log_level_ctl_cmd_vtysh, CONFIG_LEVE_5, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &devm_show_log_level_ctl_cmd_vtysh, MONITOR_LEVE_2, CMD_LOCAL);

	return;
}


