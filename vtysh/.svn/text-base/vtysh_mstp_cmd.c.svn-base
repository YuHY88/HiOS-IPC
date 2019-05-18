
/**
 * \page cmds_ref_stp STP
 * - \subpage stp_instance_cmd_vtysh
 * - \subpage stp_region_name_cmd_vtysh
 * - \subpage stp_revision_level_cmd_vtysh
 * - \subpage stp_mode_cmd_vtysh
 * - \subpage stp_priority_cmd_vtysh
 * - \subpage stp_root_cmd_vtysh
 * - \subpage stp_hello_time_cmd_vtysh
 * - \subpage stp_forward_delay_cmd_vtysh
 * - \subpage stp_max_age_cmd_vtysh
 * - \subpage stp_max_hop_cmd_vtysh
 * - \subpage stp_vlan_cmd_vtysh
 * - \subpage stp_msti_priority_cmd_vtysh
 * - \subpage interface_stp_enable_cmd_vtysh
 * - \subpage interface_stp_priority_cmd_vtysh
 * - \subpage interface_stp_cost_cmd_vtysh
 * - \subpage interface_stp_instance_priority_cmd_vtysh
 * - \subpage interface_stp_instance_cost_cmd_vtysh
 * - \subpage interface_stp_edge_cmd_vtysh
 * - \subpage interface_stp_filter_port_cmd_vtysh
 * - \subpage interface_stp_p2p_cmd_vtysh
 * - \subpage show_stp_interface_cmd_vtysh
 * - \subpage show_stp_instance_cmd_vtysh
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

static struct cmd_node mstp_node =
{
  MSTP_NODE,
  "%s(config-mstp-instance)# ",
};


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
		snprintf(pprompt, VTY_BUFSIZ, "%%s(config-mstp-instance %u)#", instance_id);
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


/*******mstp module cmd************************************************/
	   
/**
* \page stp_region_name_cmd_vtysh [no]stp region-name NAME
* - 功能说明 \n
*  Specify region name 
* - 命令格式 \n
*	stp region-name NAME
* - 参数说明 \n
*	|参数  |说明		  |
*	|------|--------------|
*	|NAME  |A maximum of 32 characters|
* - 缺省情况 \n
*  MAC addr
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
* config stp region name
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  no stp region-name
* - 参数说明 \n
*	无
* - 使用举例 \n
*	  无
*
*/
  
	   
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
	   
	   
/**
* \page stp_revision_level_cmd_vtysh [no]stp revision-level <0-65535>
* - 功能说明 \n
*	Specify revision level 
* - 命令格式 \n
*	stp revision-level <0-65535>
* - 参数说明 \n
*	|参数 	   |说明			|
*	|-----------|------------|
*	|<0-65535>	| Revision level |
* - 缺省情况 \n
* 0
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
* Specify revision level
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp revision-level
* - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/
	   
	   
	   
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
	   
/**
* \page stp_mode_cmd_vtysh [no]stp mode (stp|rstp|mstp)
* - 功能说明 \n
*	Specify state machine mode 
* - 命令格式 \n
*	stp mode (stp|rstp|mstp)
* - 参数说明 \n
*	|参数 			|说明  |
*	|--------------|-------|
*	|stp	| Spanning Tree Protocol |
*	|rstp	| Rapid Spanning Tree Protocol |
*	|mstp | Multiple Spanning Tree Protocol |
* - 缺省情况 \n
* mstp mode
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
* Specify state machine mode\n
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  no stp mode\n
 * - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/

/*mstp mode*/
DEFSH(VTYSH_L2, 
	stp_mode_cmd_vtysh,
	"stp mode (stp|rstp|mstp)",
	MSTP_STR
	"Specify state machine mode\n"
	"Spanning Tree Protocol (STP) mode\n"
	"Rapid Spanning Tree Protocol (RSTP) mode\n"
	"Multiple Spanning Tree Protocol (MSTP) mode\n")
	   
/*stp mode cmd*/
DEFSH(VTYSH_L2,
	no_stp_mode_cmd_vtysh,
	"no stp mode",
	NO_STR
	MSTP_STR
	"Specify state machine mode\n")

/**
* \page stp_priority_cmd_vtysh [no]stp priority <0-61440>
* - 功能说明 \n
*	 Specify bridge priority 
* - 命令格式 \n
*	stp priority <0-61440>
* - 参数说明 \n
*	|参数 		|说明 									 |
*	|-----------|-----------------------------|
*	|<0-61440>	 | range of priority, default is 32768 |
* - 缺省情况 \n
* 32768
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*  Specify bridge priority 
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  no stp priority
* - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/

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


/**
* \page stp_root_cmd_vtysh [no]stp root (master | slave)
* - 功能说明 \n
*	 Specify root switch 
* - 命令格式 \n
*	stp root (master | slave)
* - 参数说明 \n
*	|参数  |说明		  |
*	|------|--------------|
*	|master |Primary root bridge|
*	|slave)  |Secondary root bridge|
* - 缺省情况 \n
* 不配置
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	Specify root switch 
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  no stp root 
* - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/

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
	   
/**
* \page stp_hello_time_cmd_vtysh [no]stp hello-time <1-10>
* - 功能说明 \n
*	Specify hello time interval
* - 命令格式 \n
*	stp hello-time <1-10>
* - 参数说明 \n
*	|参数   |说明								  |
*	|--------|--------------------------|
*	|<1-10>  | range of hello time, default is 2|
* - 缺省情况 \n
* 2s
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	Specify hello time interval
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	  no stp hello-time
* - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/
	   
	   
	   
/*stp hello interval cmd*/
DEFSH(VTYSH_L2,
	stp_hello_time_cmd_vtysh,
	"stp hello-time <1-10>",
	MSTP_STR
	"Specify hello time interval\n"
	"The default value is 2. Please input an integer from 1 to 10\n")
	   
/*no stp hello interval cmd*/
DEFSH(VTYSH_L2,
	no_stp_hello_time_cmd_vtysh,
	"no stp hello-time",
	NO_STR
	MSTP_STR
	"Specify hello time interval\n")
	   
/**
* \page stp_forward_delay_cmd_vtysh [no] stp forward-delay <4-30>
* - 功能说明 \n
*	Specify forward delay
* - 命令格式 \n
*	stp forward-delay <4-30>
* - 参数说明 \n
*	|参数   |说明										 |
*	|--------|------------------------------|
*	|<4-30>  | range of forward delay, default is 15|
* - 缺省情况 \n
* 15s
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	Specify forward delay
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp forward-delay
* - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/



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

/**
* \page stp_max_age_cmd_vtysh [no]stp max-age <6-40>
* - 功能说明 \n
*	 Specify max age 
* - 命令格式 \n
*	stp max-age <6-40>
* - 参数说明 \n
*	|参数  |说明								|
*	|-------|--------------------------|
*	|<6-40>| range of max age, default is 20|
* - 缺省情况 \n
* 20
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	Specify forward delay
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp max-age
 * - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/




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

/**
* \page stp_max_hop_cmd_vtysh [no]stp max-hop <1-40>
* - 功能说明 \n
*	 Specify max hops
* - 命令格式 \n
*  stp max-hop <1-40>
* - 参数说明 \n
*	|参数   |说明								  |
*	|--------|-------------------------|
*	|<1-40>  | range of max hop, default is 20|
* - 缺省情况 \n
* 20
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	Specify forward delay
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp max-hop
 * - 参数说明 \n
*  无
* - 使用举例 \n
*	  无
*
*/



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

/*************************enter instance view & config **********************/

/**
* \page stp_instance_cmd_vtysh [no]stp instance <1-8>
* - 功能说明 \n
*	 inter instance view mode
* - 命令格式 \n
*  stp instance <1-8>
* - 参数说明 \n
*	|参数  |说明			   |
*	|------|------------------|
*	|<1-8>| range of instance id   |
* - 缺省情况 \n
* 
* - 命令模式 \n
*	config mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	inter instance view mode
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp instance <1-8>
 *	 |参数  |说明	   |
 *	 |------|--------------|
 *	 |<1-8>| range of instance|
* - 使用举例 \n
*	  无
*
*/



/*delete stp instance cmd*/
DEFSH (VTYSH_L2,
	   no_stp_instance_cmd_vtysh,
	   "no stp instance <1-8>",
	   NO_STR
	   MSTP_STR
	   "Spanning tree instance\n"
	   "Identifier of spanning tree instance, Please input an integer from 1 to 8\n")


/**
* \page stp_vlan_cmd_vtysh [no]stp vlan <1-4094> {to <1-4094>}
* - 功能说明 \n
*	 config instance vlan
* - 命令格式 \n
*  stp vlan <1-4094> {to <1-4094>}
* - 参数说明 \n
*	|参数 				|说明 		 |
*	|----------------------|--------------|
*	|<1-4094> {to <1-4094>}| config range of vlan|
* - 缺省情况 \n
* 没有vlan id
* - 命令模式 \n
*	instance view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config instance vlan
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp vlan <1-4094> {to <1-4094>}
 * - 参数说明 \n
 *	 |参数				 |说明	   |
 *	 |----------------------|--------------|
 *	 |<1-4094> {to <1-4094>}| config range of vlan|
* - 使用举例 \n
*	  无
*
*/
	   
	   
/*stp vlan cmd:
*1. each vlan must be belong to one msti,one msti can contain some vlan
*2. vlan config in mstp instance view
*/
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


/**
* \page stp_msti_priority_cmd_vtysh [no]stp priority <0-61440>
* - 功能说明 \n
*	 config instance priority
* - 命令格式 \n
*  stp priority <0-61440>
* - 参数说明 \n
*	|参数    |说明								   |
*	|----------|---------------------------------|
*	|<0-61440>| config value of priority, default is 32768|
* - 缺省情况 \n
* no stp priority
* - 命令模式 \n
*	instance view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config instance vlan
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp priority 
 * - 参数说明 \n
 * 无
* - 使用举例 \n
*	  无
*
*/

	   
	   
	   
/*stp priority cmd*/
DEFSH(VTYSH_L2,
	   stp_msti_priority_cmd_vtysh,
	   "stp priority <0-61440>",
	   MSTP_STR
	   "Specify msti priority\n"
	   "MSTI priority, in steps of 4096, the default value is 32768. Please input an integer from 0 to 61440\n")

/*no stp priority cmd*/
DEFSH(VTYSH_L2,
	   no_stp_msti_priority_cmd_vtysh,
	   "no stp priority",
	   NO_STR
	   MSTP_STR
	   "Specify bridge priority\n")

/***********enter interface view & config ****************************/


	   
	   
	   
/**
* \page interface_stp_enable_cmd_vtysh [no]stp enable		
* - 功能说明 \n
*	 Port config stp enable
* - 命令格式 \n
*	stp enable
* - 参数说明 \n
*	无
* - 缺省情况 \n
* stp disable
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config port stp enable
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp enable
* - 参数说明 \n
* 无
* - 使用举例 \n
*	  无
*
*/	   
/*port stp enable cmd*/
DEFSH(VTYSH_L2,
	interface_stp_enable_cmd_vtysh,
	"stp enable",
	MSTP_STR
	"Enable Spanning Tree Protocol (STP)\n")
	   
/*port no stp enable cmd*/
DEFSH(VTYSH_L2, 
	no_interface_stp_enable_cmd_vtysh,
	"no stp enable",
	NO_STR
	MSTP_STR
	"Enable Spanning Tree Protocol (STP)\n")
	   
	   
	   
/**
* \page interface_stp_priority_cmd_vtysh [no]stp priority <0-240>
* - 功能说明 \n
*	 Specify port priority
* - 命令格式 \n
*	stp priority <0-240>
* - 参数说明 \n
*	|参数    |说明										|
*	|----------|---------------------------------------|
*	|<0-240>	 | config port value of priority, default is 128|
* - 缺省情况 \n
* 128
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config interface  priority
*	- 前置条件 \n
*	 接口使能
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	 no stp priority
 * - 参数说明 \n
 * 无
* - 使用举例 \n
*	  无
*
*/
	   
	   
/*config interface priority cmd*/
DEFSH(VTYSH_L2, 
	interface_stp_priority_cmd_vtysh,
	"stp priority <0-240>",
	MSTP_STR
	"Specify port priority\n"
	"Port priority, in steps of 16\n")
	   
/*no stp priority cmd*/
DEFSH(VTYSH_L2, 
	no_interface_stp_priority_cmd_vtysh,
	"no stp priority",
	NO_STR
	MSTP_STR
	"Specify port priority\n"
	"Port priority, in steps of 16\n")
	   
/**
* \page interface_stp_cost_cmd_vtysh [no]stp cost <1-200000000>
* - 功能说明 \n
*	 Specify port path cost
* - 命令格式 \n
*	stp cost <1-200000000>
* - 参数说明 \n
*	|参数    |说明										|
*	|----------|---------------------------------------|
*	|<1-200000000>	| config value of port path cost|
* - 缺省情况 \n
* 20000
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config interface cost
*	- 前置条件 \n
*	接口使能 
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp cost
 * - 参数说明 \n
 * 无
* - 使用举例 \n
*	  无
*
*/



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

/**
* \page interface_stp_instance_priority_cmd_vtysh [no]stp instance <1-8> port priority <0-240>
* - 功能说明 \n
*	 Specify instance port path cost
* - 命令格式 \n
*	stp instance <1-8> port priority <0-240>
* - 参数说明 \n
*	|参数    |说明		|
*	|----------|-----------|
*	|<1-8>	| Specify instance id|
 *	 |<0-240>  | Specify port priority|
* - 缺省情况 \n
* 128
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config specify instance interface cost
*	- 前置条件 \n
*	 接口使能
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp instance <1-8> port priority
 * - 参数说明 \n
 *	 |<1-8>  | Specify instance id|
* - 使用举例 \n
*	  无
*
*/



/*config interface priority cmd*/
DEFSH(VTYSH_L2, 
	   interface_stp_instance_priority_cmd_vtysh,
	   "stp instance <1-8> port priority <0-240>",
	   MSTP_STR 	   
	   "Spanning tree instance\n"
	   "Identifier of spanning tree instance"
	   "Specify port parameter\n"
	   "Specify port priority\n"
	   "Port priority, in steps of 16, The default is 128.\n")

/*config interface priority cmd*/
DEFSH(VTYSH_L2, 
	   no_interface_stp_instance_priority_cmd_vtysh,
	   "no stp instance <1-8> port priority",
	   NO_STR
	   MSTP_STR
	   "Spanning tree instance\n"
	   "Identifier of spanning tree instance"
	   "Specify port parameter\n"
	   "Specify port priority\n"
	   "Port priority, in steps of 16, The default is 128.\n")

/**
* \page interface_stp_instance_cost_cmd_vtysh [no]stp instance <1-8> cost <1-200000000>
* - 功能说明 \n
*	 Specify port path cost
* - 命令格式 \n
*	stp instance <1-8> cost <1-200000000>
* - 参数说明 \n
*	|参数 			 |说明				|
*	|----------------|-----------------|
*	|<1-8>					| Specify instance id |
 *	 |<1-200000000>   | Specify port cost	 |
* - 缺省情况 \n
* 
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config specify instance cost
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp instance <1-8> port cost
 * - 参数说明 \n
 *	 |<1-8>  | Specify instance id|
* - 使用举例 \n
*	  无
*
*/



/*config interface priority cmd*/
DEFSH(VTYSH_L2, 
	   interface_stp_instance_cost_cmd_vtysh,
	   "stp instance <1-8> cost <1-200000000>",
	   MSTP_STR
	   "Spanning tree instance\n"
	   "Identifier of spanning tree instance"
	   "Specify port path cost\n"
	   "Port path cost, Please input an integer from 1 to 200,000,000\n")

/*config interface priority cmd*/
DEFSH(VTYSH_L2, 
	   no_interface_stp_instance_cost_cmd_vtysh,
	   "no stp instance <1-8> cost",
	   NO_STR
	   MSTP_STR
	   "Spanning tree instance\n"
	   "Identifier of spanning tree instance"
	   "Specify port path cost\n")


/**
* \page interface_stp_edge_cmd_vtysh [no]stp edge-port
* - 功能说明 \n
*	 Specify port edge port
* - 命令格式 \n
*	stp edge-port
* - 参数说明 \n
*  无
* - 缺省情况 \n
* 
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config interface edge port
*	- 前置条件 \n
*	 接口使能
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp edge-port
 * - 参数说明 \n
 *	 无
* - 使用举例 \n
*	  无
*
*/

	   
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
	   
/**
* \page interface_stp_filter_port_cmd_vtysh [no]stp filter-port
* - 功能说明 \n
*	 Specify port filter port
* - 命令格式 \n
*	stp filter-port
* - 参数说明 \n
* 无
* - 缺省情况 \n
* 
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config interface filter port
*	- 前置条件 \n
*	 接口使能
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp filter-port
 * - 参数说明 \n
 *	 无
* - 使用举例 \n
*	  无
*
*/
	   
	   
	   
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
	   
/**
* \page interface_stp_p2p_cmd_vtysh [no]stp p2p
* - 功能说明 \n
* Specify point to point link 
* - 命令格式 \n
*	stp p2p
* - 参数说明 \n
* 无
* - 缺省情况 \n
* 
* - 命令模式 \n
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	config interface  point to point link 
*	- 前置条件 \n
*  接口使能
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	no stp p2p
* - 参数说明 \n
*	无
* - 使用举例 \n
*	  无
*
*/
	   
	   
	   
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
	   
 /*********************show cmd****************************************/
	   
/**
* \page show_stp_instance_cmd_vtysh show stp {instance <0-8>}
* - 功能说明 \n
* show stp instance info 
* - 命令格式 \n
*	show stp {instance <0-8>}
* - 参数说明 \n
*	|参数 		   |说明				  |
*	|----------|-------------------------|
*	|instance <0-8> | Specify instance id|
* - 缺省情况 \n
* show all instance info
* - 命令模式 \n
*	  config mode
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	show instance info 
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	
* - 参数说明 \n
*	 无
* - 使用举例 \n
*Hios(config)# show stp 														   \n												   
*																								   \n								  
*-------[ Region config ]-------												  \n												   
*																								   \n								   
*Region   name					   : huahuan											\n												 
*Revision level 					   : 0												  \n												 
*CIST Vlan Map					   : 11 to 19, 31 to 99, 101 to 4094			\n														 
*																									   \n							   
*																									  \n							   
*-------[CIST Global Info][Mode MSTP]-------								  \n													   
*Bridge 							   : 32768.00:1d:80:01:fc:03					  \n													 
*CIST Root/ERPC 				   : 32768.00:1d:80:01:fc:03 / 0				  \n													 
*CIST RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0		   \n															 
*Bridge Priority					   : 32768											  \n												 
*Bridge Root Role				   : NULL													 \n 										 
*Hello Time 						   : 3s 												\n												 
*Foward Delay					   : 15s													   \n										 
*Message Max Age				   : 20s													   \n										 
*Message Max Hop Count			 : 20													   \n										   
*																										  \n						   
*-------[Port(gigabitethernet 1/1/5)] status[FORWARDING]------- 	   \n															   
*Port Protocol						   : Enabled											 \n 											 
*Port STP Mode					   : MSTP														\n										 
*Designated Bridge				   : 32768.00:1d:80:01:fc:03						   \n												 
*Port Role							   : Designated Port								   \n												 
*Port Priority						   : 128													\n											 
*Port Path Cost 					   : 20000												  \n											 
*Port Edge Port 					   : Disable											  \n											 
*Port Filter Port					   : Disable											  \n											 
*Port Mstp P2P					   : Disable													 \n 									 
*																										   \n						   
*TC   Send							   : 0														\n											 
*TCN  Send							   : 0													   \n											 
*TC   Recv							   : 0														\n											 
*TCN  Recv							   : 0														 \n 										 
*BPDU Send						   : 35032														\n										 
*				 Config: 0, RST: 0, MST: 35032												 \n 									   
*																											\n						   
*BPDU Received					   : 34719														 \n 									 
*				 Config: 0, RST: 0, MST: 34719												  \n									   
*																											\n						   
*																											\n						   
*-------[Port(gigabitethernet 1/1/6)] status[FORWARDING]------- 		  \n														   
*Port Protocol						   : Enabled											   \n											 
*Port STP Mode					   : MSTP														   \n									 
*Designated Bridge				   : 32768.00:1d:80:01:fc:03							\n												 
*Port Role							   : Designated Port								   \n												 
*Port Priority						   : 128												   \n											 
*Port Path Cost 					   : 20000														\n										 
*Port Edge Port 					   : Disable													 \n 									 
*Port Filter Port					   : Disable													 \n 									 
*Port Mstp P2P					   : Disable													 \n 									 
*																											  \n					   
*TC   Send							   : 0															   \n									 
*TCN  Send							   : 0															   \n									 
*TC   Recv							   : 0															   \n									 
*TCN  Recv							   : 0															   \n									 
*BPDU Send						   : 35031														\n										 
*				 Config: 1, RST: 0, MST: 35030													\n									   
*																											 *
 *																											  \n					   
*BPDU Received					   : 34728														\n										 
*				 Config: 0, RST: 0, MST: 34728													\n									   
*																												\n					   
*																											   \n					   
*----------[Instance 1 Global Info]----------										  \n											   
*Vlan Map							   : 1 to 10, 100											  \n										 
*Instance Bridge Priority		   : 32768													  \n										 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03						  \n												 
*MSTI RegRoot/IRPC				   : 4096.00:1d:80:01:fc:25 / 20000 			   \n													 
*																											 \n 					   
*Instance[1], enabled mstp port:																  \n								   
 *																												  \n				   
*----------[MSTI Port(gigabitethernet 1/1/5)] status[FORWARDING]---------- \n														   
*Port Role							   : Root Port														  \n								 
*Port Priority						   : 128															   \n								 
*Port Cost							   : 20000															  \n								 
*Designated Bridge				   : 4096.00:1d:80:01:fc:25 								  \n										 
*Port Times 						   : RemHops 20 												   \n									 
 *																													\n				   
*																													 \n 			   
*----------[MSTI Port(gigabitethernet 1/1/6)] status[DISCARDING]----------	 \n 													   
*Port Role							   : Alternate Port 												  \n								 
*Port Priority						   : 128																\n								 
*Port Cost							   : 20000															   \n								 
*Designated Bridge				   : 4096.00:1d:80:01:fc:25 								  \n										 
*Port Times 						   : RemHops 20 												   \n									 
*																													 \n 			   
*																													  \n			   
*----------[Instance 2 Global Info]----------												   \n									   
*Vlan Map							   : 20 to 30														   \n								 
*Instance Bridge Priority		   : 32768															  \n								 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03								 \n 										 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						   \n											 
*																												   \n				   
*Instance[2], enabled mstp port:																	  \n							   
*																												   \n				   
*----------[MSTI Port(gigabitethernet 1/1/5)] status[FORWARDING]---------- \n														   
*Port Role							   : Designated Port											  \n									 
*Port Priority						   : 128															  \n								 
*Port Cost							   : 20000															 \n 								 
*Designated Bridge				   : 32768.00:1d:80:01:fc:03								\n											 
*Port Times 						   : RemHops 20 												  \n									 
*																												  \n				  
*																												  *
*																												  \n				  
*----------[MSTI Port(gigabitethernet 1/1/6)] status[FORWARDING]----------\n														   
*Port Role							   : Designated Port											  \n									 
*Port Priority						   : 128															   \n								 
*Port Cost							   : 20000															 \n 								 
*Designated Bridge				   : 32768.00:1d:80:01:fc:03								\n											 
*Port Times 						   : RemHops 20 												\n										 
*																												  \n				   
*																												 \n 				   
*----------[Instance 3 Global Info]----------											  \n										   
*Vlan Map							   :																   \n								 
*Instance Bridge Priority		   : 32768														   \n									 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03							   \n											 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						  \n											 
*																												 \n 				   
*Instance[3], enabled mstp port:																	\n								   
*Info: No instance[3] port information is available.										   \n									   
*																												 \n 				   
*----------[Instance 4 Global Info]----------											 \n 										   
*Vlan Map							   :																	\n								 
*Instance Bridge Priority		   : 32768															\n									 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03							   \n											 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						  \n											 
*																												 \n 				   
*Instance[4], enabled mstp port:																	\n								   
*Info: No instance[4] port information is available.										  \n									   
*																											   \n					  
*----------[Instance 5 Global Info]----------											 \n 										   
*Vlan Map							   :																  \n								 
*Instance Bridge Priority		   : 32768														  \n									 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03							 \n 											 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						\n												 
*																												\n					   
*Instance[5], enabled mstp port:																	\n								   
*Info: No instance[5] port information is available.										   \n									   
*																												\n					   
*----------[Instance 6 Global Info]----------											 \n 										   
*Vlan Map							   :																   \n								 
*Instance Bridge Priority		   : 32768															\n									 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03							   \n											 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						  \n											 
*																												   \n				   
*Instance[6], enabled mstp port:																	   \n							   
*Info: No instance[6] port information is available.											  \n								   
*																												   \n				   
*----------[Instance 7 Global Info]----------											   \n										   
*Vlan Map							   :																	 \n 							 
*Instance Bridge Priority		   : 32768															 \n 								 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03								\n											 
*MSTI RegRoot/IRPC				   : 32768.00:1d:80:01:fc:03 / 0						  \n											 
 *																												 \n 				   
*Instance[7], enabled mstp port:																	\n								   
*Info: No instance[7] port information is available.										   \n									   
*																												 \n 				   
*----------[Instance 8 Global Info]----------											  \n										   
*Vlan Map							   :																   \n								 
*Instance Bridge Priority		   : 32768														   \n									 
*MSTI Bridge ID 				   : 32768.00:1d:80:01:fc:03							  \n											 
*MSTI RegRoot/IRPC		   : 32768.00:1d:80:01:fc:03 / 0						 \n 											 
*																												  \n				   
*Instance[8], enabled mstp port:																	\n								   
*Info: No instance[8] port information is available.		 \n
* \n
* Hios(config)# show stp instance 1 										   \n														
*																	   \n															
* -------[ Region config ]-------										   \n														
*																	   \n															
* Region   name 					: huahuan							   \n															
* Revision level					: 0 									   \n														
* CIST Vlan Map 					: 11 to 19, 31 to 99, 101 to 4094		   \n														
*																	   \n															
*																	   \n															
* -------[CIST Global Info][Mode MSTP]-------							   \n														
* Bridge						: 32768.00:1d:80:01:fc:03				   \n														
* CIST Root/ERPC				: 32768.00:1d:80:01:fc:03 / 0			   \n														
* CIST RegRoot/IRPC 			: 32768.00:1d:80:01:fc:03 / 0			   \n														
* Bridge Priority				: 32768 									   \n													
* Bridge Root Role				: NULL										   \n													
* Hello Time					: 3s									   \n														
* Foward Delay				: 15s									   \n														
* Message Max Age			: 20s									   \n														
* Message Max Hop Count 		: 20									   \n														
*																	   \n															
* -------[Instance 1 Global Info]-------								   \n														
* Vlan Map						: 1 to 10, 100							   \n														
* Instance Bridge Priority			: 32768 								   \n														
 *MSTI Bridge ID					: 32768.00:1d:80:01:fc:03			   \n															
* MSTI RegRoot/IRPC 				: 4096.00:1d:80:01:fc:25 / 20000		   \n														
*																	   \n															
*																	   \n															
* ----------[MSTI Port(gigabitethernet 1/1/5)] status[FORWARDING]----------\n															
* Port Role 						: Root Port 							   \n														
* Port Priority 						: 128								   \n															
* Port Cost 						: 20000 								   \n														
* Designated Bridge 				: 4096.00:1d:80:01:fc:25				   \n														
* Port Times						: RemHops 20						   \n															
*																	   \n															
*																	   \n															
* ----------[MSTI Port(gigabitethernet 1/1/6)] status[DISCARDING]----------\n															
* Port Role 						: Alternate Port						   \n														
* Port Priority 						: 128								   \n															
* Port Cost 						: 20000 								   \n														
* Designated Bridge 				: 4096.00:1d:80:01:fc:25				   \n														
* Port Times						: RemHops 20						   \n															
*																	   \n															
* Hios(config)# 														   \n				  
* \n
*
*
*/
  
/**
* \page show_stp_interface_cmd_vtysh show stp interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}
* - 功能说明 \n
* show stp interface info 
* - 命令格式 \n
*	show stp interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}
* - 参数说明 \n
*	|参数 					 |说明									   | 
*	|----------------|--------------------------------|
*	|ethernet USP				| format: <0-7>/<0-31>/<1-255>|
*	|gigabitethernet USP	 | format: <0-7>/<0-31>/<1-255>|
*	| xgigabitethernet USP	| format: <0-7>/<0-31>/<1-255>|
*	|trunk TRUNK			  |format: <1-128>							|
* - 缺省情况 \n
* 
* - 命令模式 \n
*	 config mode
*	interface view mode
* - 用户等级 \n
*	5
* - 使用指南 \n
*	- 应用场景 \n
*	show interface info 
*	- 前置条件 \n
*	  无
*	- 后续任务 \n
*	  无
*	- 注意事项 \n
*	  无
*	- 相关命令 \n
*	
* - 参数说明 \n
*	无
* - 使用举例 \n
*	 
* Hios(config)# show stp interface												\n														
*																							   \n									   
* -------[ Region config ]-------												\n														
*																							  \n									  
* Region   name 				   : huahuan										  \n												 
* Revision level					   : 0												\n										  
* CIST Vlan Map 				   : 11 to 19, 31 to 99, 101 to 4094		   \n													  
*																								   \n									
*																								   \n								  
* -------[CIST Global Info][Mode MSTP]-------							   \n													 
* Bridge							   : 32768.00:1d:80:01:fc:03			   \n												  
* CIST Root/ERPC				   : 32768.00:1d:80:01:fc:03 / 0				 \n 										   
* CIST RegRoot/IRPC 			   : 32768.00:1d:80:01:fc:03 / 0		   \n											   
* Bridge Priority					   : 32768											   \n								 
* Bridge Root Role				   : NULL													\n							   
* Hello Time						   : 3s 												\n							   
* Foward Delay					   : 15s													  \n							 
* Message Max Age				   : 20s													   \n								 
* Message Max Hop Count 		  : 20													   \n								  
* \n
* -------[Port(gigabitethernet 1/1/5)] status[FORWARDING]-------		  \n											  
* Port Protocol 					   : Enabled												 \n 						   
* Port STP Mode 				   : MSTP														 \n 						   
* Designated Bridge 			   : 32768.00:1d:80:01:fc:03							 \n 											 
* Port Role 						   : Designated Port									\n												 
* Port Priority 					   : 128													 \n 										 
* Port Path Cost					   : 20000												   \n											 
* Port Edge Port					   : Disable												\n											 
* Port Filter Port					   : Disable												\n											 
* Port Mstp P2P 				   : Disable													   \n									 
*																											  \n					   
* TC   Send 						   : 0														   \n										 
* TCN  Send 						   : 0														   \n										 
* TC   Recv 						   : 0														   \n										 
* TCN  Recv 						   : 0														   \n										 
* BPDU Send 					   : 53946														   \n									 
*				 Config: 0, RST: 0, MST: 53946													\n									   
*																											  \n					   
* BPDU Received 				   : 53580														   \n									 
*				 Config: 0, RST: 0, MST: 53580													 \n 								   
*																											 \n 					   
* -------[MSTI 1 Global Info]-------													   \n										   
* Instance Id						   : 1														   \n										 
* Vlan Map							   : 1 to 10, 100										   \n											 
* Priority							   : 32768														   \n									 
* MSTI Bridge ID				   : 32768.00:1d:80:01:fc:03							  \n											 
* MSTI RegRoot/IRPC 			   : 4096.00:1d:80:01:fc:25 / 20000 			\n														 
*																											  \n					   
* -------[MSTI(1) Port(gigabitethernet 1/1/5)] status[FORWARDING]------- \n 															
* Port Role 						   : Root Port												\n											 
* Port Priority 					   : 128													   \n										 
* Port Cost 						   : 20000													 \n 										 
* Designated Bridge 			   : 4096.00:1d:80:01:fc:25 							   \n											 
* Port Times						   : RemHops 20 										 \n 											 
*																											  \n					   
*																											  \n					   
* -------[MSTI 2 Global Info]-------														 \n 										
* Instance Id						   : 2															\n										 
* Vlan Map							   : 20 to 30												\n											 
* Priority							   : 32768														   \n									 
* MSTI Bridge ID				   : 32768.00:1d:80:01:fc:03							  \n											 
* MSTI RegRoot/IRPC 			   : 32768.00:1d:80:01:fc:03 / 0				   \n													 
*																											   \n					   
* -------[MSTI(2) Port(gigabitethernet 1/1/5)] status[FORWARDING]-------  \n															
* Port Role 						   : Designated Port									   \n											 
* Port Priority 					   : 128													   \n										 
* Port Cost 						   : 20000														   \n									 
* Designated Bridge 			   : 32768.00:1d:80:01:fc:03							   \n											 
* Port Times						   : RemHops 20 										 \n 											 
*																											  \n					   
*																											  \n					   
*																											   \n					   
* -------[Port(gigabitethernet 1/1/6)] status[FORWARDING]-------			  \n														
* Port Protocol 					   : Enabled												 \n 										 
* Port STP Mode 				   : MSTP														   \n									 
* Designated Bridge 			   : 32768.00:1d:80:01:fc:03							 \n 											 
* Port Role 						   : Designated Port									 \n 											 
* Port Priority 					   : 128													  \n										 
* Port Path Cost					   : 20000													 \n 										 
* Port Edge Port					   : Disable												  \n										 
* Port Filter Port					   : Disable												 \n 										 
* Port Mstp P2P 				   : Disable														\n									 
*																											  \n					   
* TC   Send 						   : 0															\n										 
* TCN  Send 						   : 0															\n										 
* TC   Recv 						   : 0															 \n 									 
* TCN  Recv 						   : 0															\n										 
* BPDU Send 					   : 53945															\n									 
*				 Config: 1, RST: 0, MST: 53944													 \n 								   
*																											  \n					   
* BPDU Received 				   : 53589															\n									 
*				 Config: 0, RST: 0, MST: 53589													 \n 								   
*																											   \n					   
* -------[MSTI 1 Global Info]-------														 \n 										
* Instance Id						   : 1															 \n 									 
* Vlan Map							   : 1 to 10, 100											\n											 
* Priority							   : 32768															 \n 								 
* MSTI Bridge ID				   : 32768.00:1d:80:01:fc:03								\n											 
* MSTI RegRoot/IRPC 			   : 4096.00:1d:80:01:fc:25 / 20000 			  \n													 
*																												\n					   
* -------[MSTI(1) Port(gigabitethernet 1/1/6)] status[DISCARDING]-------	 \n 														
* Port Role 						   : Alternate Port 										  \n										 
* Port Priority 					   : 128														 \n 									 
* Port Cost 						   : 20000														\n										 
* Designated Bridge 			   : 4096.00:1d:80:01:fc:25 								 \n 										 
* Port Times						   : RemHops 20 											\n											 
*																												 \n 				   
*																												 \n 				   
* -------[MSTI 2 Global Info]-------															\n										
* Instance Id						   : 2															   \n									 
* Vlan Map							   : 20 to 30													\n										 
* Priority							   : 32768															   \n								 
* MSTI Bridge ID				   : 32768.00:1d:80:01:fc:03								  \n										 
* MSTI RegRoot/IRPC 			   : 32768.00:1d:80:01:fc:03 / 0					   \n												 
*																												  \n				   
* -------[MSTI(2) Port(gigabitethernet 1/1/6)] status[FORWARDING]-------	 \n 														
* Port Role 						   : Designated Port										   \n										 
* Port Priority 					   : 128														   \n									 
* Port Cost 						   : 20000														 \n 									 
* Designated Bridge 			   : 32768.00:1d:80:01:fc:03								   \n										 
* Port Times						   : RemHops 20 											   \n										 
*																													\n				   
*																									  \n
*\n
*/
	   
	   
/*show stp interface [USP]*/
DEFSH (VTYSH_L2,
   show_stp_interface_cmd_vtysh,
   "show stp interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}",
   SHOW_STR
   MSTP_STR
   "Specify interface\n"
   "Ethernet interface type\n"
   "The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>\n"
   CLI_INTERFACE_GIGABIT_ETHERNET_STR
   "The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
   CLI_INTERFACE_XGIGABIT_ETHERNET_STR
   "The port/subport of 10gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
   "trunk interface type\n"
   "The port/subport of trunk, format: <1-128>\n")


/*show stp instance*/
DEFSH(VTYSH_L2,
	show_stp_instance_cmd_vtysh,
   "show stp {instance <0-8>}",
   SHOW_STR
   MSTP_STR
   "Spanning tree instance\n"
   "Identifier of spanning tree instance, Please input an integer from 0 to 8\n")	   

DEFSH(VTYSH_L2,
		debug_stp_running_info_cmd_vtysh,
        "debug mstp running-info interface {ethernet USP | gigabitethernet USP | xgigabitethernet USP | trunk TRUNK}",
        DEBUG_STR
        MSTP_STR
        "The current running info of mstp\n"
        "Specify interface\n"
        "Ethernet interface type\n"
        "The port/subport of ethernet, format: <0-7>/<0-31>/<1-255>\n"
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
		"The port/subport of gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
		CLI_INTERFACE_XGIGABIT_ETHERNET_STR
		"The port/subport of 10gigabit ethernet, format: <0-7>/<0-31>/<1-255>\n"
        "trunk interface type\n"
        "The port/subport of trunk, format: <1-128>\n")

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
void vtysh_init_mstp_cmd (void)
{
	   
   /*mstp whole config */
   install_node (&mstp_node, NULL);
   vtysh_install_default (MSTP_NODE);
   install_element_level (MSTP_NODE, &vtysh_exit_stp_instance_cmd, CONFIG_LEVE_5,CMD_SYNC); 
   install_element_level (MSTP_NODE, &vtysh_quit_stp_instance_cmd, CONFIG_LEVE_5,CMD_SYNC); 
	   
   /*region config*/
   install_element_level (CONFIG_NODE, &stp_region_name_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_region_name_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*revision level*/
   install_element_level (CONFIG_NODE, &stp_revision_level_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_revision_level_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp mode*/
   install_element_level (CONFIG_NODE, &stp_mode_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_mode_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp priority*/
   install_element_level (CONFIG_NODE, &stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp root*/
   install_element_level (CONFIG_NODE, &stp_root_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_root_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp hello-time*/
   install_element_level (CONFIG_NODE, &stp_hello_time_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_hello_time_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp forward-delay*/
   install_element_level (CONFIG_NODE, &stp_forward_delay_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_forward_delay_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp max-age*/
   install_element_level (CONFIG_NODE, &stp_max_age_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_max_age_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp max-age*/
   install_element_level (CONFIG_NODE, &stp_max_hop_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_max_hop_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
		   
   /*---------------------instance config cmd----------------------*/
  
   /*instance config*/
   install_element_level (CONFIG_NODE, &stp_instance_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (CONFIG_NODE, &no_stp_instance_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*vlan config*/
   install_element_level (MSTP_NODE, &stp_vlan_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (MSTP_NODE, &no_stp_vlan_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*vlan config*/
   install_element_level (MSTP_NODE, &stp_msti_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (MSTP_NODE, &no_stp_msti_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
	   
   /*---------------------interface config cmd----------------------*/
   /*physical interface node cmd*/
  
   /*stp enable*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp priority*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
   /*stp cost*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
   /*stp instance<1-8> priority*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_instance_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_instance_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   /*stp instance<1-8> cost*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_instance_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_instance_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   /*stp edge*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
   /*stp filter_port*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);	   
   /*stp p2p_port*/
   install_element_level (PHYSICAL_IF_NODE, &interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (PHYSICAL_IF_NODE, &no_interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);	   
	   
   /*trunk interface node cmd*/
   /*stp enable*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_enable_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   /*stp priority*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   /*stp cost*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   /*stp instance<1-8> priority*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_instance_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_instance_priority_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);    
   /*stp instance<1-8> cost*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_instance_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_instance_cost_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);    
   /*stp edge*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_edge_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC); 
   /*stp filter_port*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_filter_port_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);	   
   /*stp p2p_port*/
   install_element_level (TRUNK_IF_NODE, &interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);
   install_element_level (TRUNK_IF_NODE, &no_interface_stp_p2p_cmd_vtysh, CONFIG_LEVE_5,CMD_SYNC);  
	   
   /*-------------------------show cmd-------------------------------*/
   /*config mode*/
   install_element_level (CONFIG_NODE, &show_stp_interface_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);
   install_element_level (CONFIG_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);		   
  
   /*instance mode*/
   install_element_level (MSTP_NODE, &show_stp_interface_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);
   install_element_level (MSTP_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);
	   
   /*interface mode*/
   install_element_level (PHYSICAL_IF_NODE, &show_stp_interface_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);
   install_element_level (PHYSICAL_IF_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);	   
   install_element_level (TRUNK_IF_NODE, &show_stp_interface_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);
   install_element_level (TRUNK_IF_NODE, &show_stp_instance_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL); 	   
	   
	   
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

	install_element_level (CONFIG_NODE, &debug_stp_running_info_cmd_vtysh, MONITOR_LEVE_2,CMD_LOCAL);

	/**********mstp module cmd end********************************************/		   
}

 
