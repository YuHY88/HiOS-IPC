/**
 * \page cmds_ref_aaa AAA
 * - \subpage modify_log_aaa
 * - \subpage aaa_mode_enable
 * - \subpage show_aaa_cmd
 * - \subpage show_login_user_config_cmd
 * - \subpage show_login_user_online_cmd
 */
 
/**
 * \page modify_log_aaa Modify Log
 * \section aaa-v007r004 HiOS-V007R004
 *  -# 
 * \section aaa-v007r003 HiOS-V007R003
 *  -# 
 */
#include <zebra.h>
#include "command.h"
#include "vtysh.h"
#include "ifm_common.h"

#include "aaa/aaa_user_manager.h"


static struct cmd_node aaa_node =
{
	AAA_NODE,
	"%s(config-aaa)#",
};


static struct cmd_node aaa_local_user_node =
{
	AAA_LOCAL_USER_NODE,
	"%s(config-luser-manage)#",
};


/**
 * \page aaa_mode_enable aaa
 * - 功能说明 \n
 *   进入aaa视图
 * - 命令格式 \n
 *   aaa
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令进入aaa视图
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFUNSH(VTYSH_AAA,
		aaa_mode_enable,
		aaa_mode_enable_cmd,
		"aaa",
		"aaa command node\n")
{
	vty->node = AAA_NODE;

	return CMD_SUCCESS;
}


DEFUNSH(VTYSH_AAA,
	vtysh_exit_aaa,
	vtysh_exit_aaa_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}


ALIAS (vtysh_exit_aaa,
	   vtysh_quit_aaa_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n")


	
/************************* common show cmd *************************/

/**
 * \page show_aaa_cmd show aaa config
 * - 功能说明 \n
 *   显示aaa配置信息
 * - 命令格式 \n
 *   show aaa config
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config 或 aaa 模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     在config视图或aaa视图下，使用命令查询aaa配置信息
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFSH(VTYSH_AAA,
		show_aaa_cmd,
		"show aaa config",
		SHOW_STR
		"show aaa cfg\n"
		"Config\n");

/**
 * \page show_login_user_config_cmd show login-user config
 * - 功能说明 \n
 *   显示aaa的用户配置信息;
 * - 命令格式 \n
 *   show login-user config
 * - 参数说明 \n
 *   无
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   aaa 模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     在aaa视图下，使用该命令查询aaa的用户配置信息
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *   Hios(config-aaa)#show login-user config \n
 *   ----------------------user config information---------------------- \n
 *   ******************************************************************* \n
 *   id             	: 1 \n
 *   name          	: admin \n
 *   password       	: huahuan \n
 *   level           	: 14 \n
 *   max repeat num  	: 1 \n
 *   ******************************************************************* \n
 *
 */
DEFSH(VTYSH_AAA,
		show_login_user_config_cmd,
		"show login-user config",
		SHOW_STR
		"login user\n"
		"user config information\n");

/**
 * \page show_login_user_online_cmd show login-user online {mode (radius|tacacs|local|none)}
 * - 功能说明 \n
 *   显示aaa的在线用户信息
 * - 命令格式 \n
 *   show login-user online {mode (radius|tacacs|local|none)}
 * - 参数说明 \n
 *   |参数  |说明          |
 *   |------|--------------|
 *   |radius|radius登陆模式|
 *   |tacacs|tacacs登陆模式|
 *   |local |本地登陆模式  |
 *   |none  |无认证登陆模式|
 *
 * - 缺省情况 \n
 *   无
 * - 命令模式 \n
 *   config 或 aaa 模式
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     在config视图或aaa视图下，使用命令查询aaa配置信息
 *   - 前置条件 \n
 *     无
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFSH(VTYSH_AAA,
		show_login_user_online_cmd,
		"show login-user online {mode (radius|tacacs|local|none)}",
		SHOW_STR
		"login user\n"
		"user online information\n"
		"login mode\n"
		"radius\n"
		"tacacs\n"
		"local\n"
		"none\n");

DEFSH(VTYSH_AAA,
		show_login_user_log_cmd,
		"show login-user log {mode (radius|tacacs|local|none)}",
		SHOW_STR
		"login user\n"
		"user log information\n"
		"login mode\n"
		"radius\n"
		"tacacs\n"
		"local\n"
		"none\n");


DEFSH(VTYSH_AAA,
		show_login_mode_config_cmd,
		"show login-mode config",
		SHOW_STR
		"login mode\n"
		"login mode config information\n");


DEFSH(VTYSH_AAA,
		aaa_enable_password_show_cmd,
		"show enable password [level <0-15>]",
		SHOW_STR
		"enable\n"
		"password\n");

/************************* common config cmd *************************/

DEFSH(VTYSH_AAA,
		aaa_login_max_num_set_cmd,
		"login-user max-number <1-32>",
		"login user\n"
		"max\n"
		"max number\n");

DEFSH(VTYSH_AAA,
		no_aaa_login_max_num_cmd,
		"no login-user max-number",
		"no\n"
		"login user\n"
		"max number\n");


DEFSH(VTYSH_AAA,
		aaa_login_user_idle_cut_time_set_cmd,
		"login-user idle-cut <0-30>",
		"login user\n"
		"idle-cut\n"
 		"0-30 minutes\n");

DEFSH(VTYSH_AAA,
		no_aaa_login_user_idle_cut_time_cmd,
		"no login-user idle-cut",
		"no\n"
		"login user\n"
		"idle-cut\n");


DEFSH(VTYSH_AAA,
		aaa_login_user_default_level_set_cmd,
		"login-user default-level <0-14>",
		"login user\n"
		"default-level\n"
		"0-14\n");

DEFSH(VTYSH_AAA,
		no_aaa_login_user_default_level_set_cmd,
		"no login-user default-level",
		"no\n"
		"login user\n"
		"default-level\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd0,
		"login-mode none",
		"login mode\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd1,
		"login-mode (tacacs|radius|local) [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd2,
		"login-mode radius (tacacs|local) [none]",
		"login mode\n"
		"radius protocol\n"
		"tacacs+ protocol\n"		
		"local authenticate\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd3,
		"login-mode tacacs (radius|local) [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd4,
		"login-mode local (radius|tacacs) [none]",
		"login mode\n"
		"local authenticate\n"
		"radius protocol\n"
		"tacacs+ protocol\n"		
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd5,
		"login-mode radius tacacs local [none]",
		"login mode\n"
		"radius protocol\n"
		"tacacs+ protocol\n"
		"local authenticate\n"				
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd6,
		"login-mode radius local tacacs [none]",
		"login mode\n"
		"radius protocol\n"
		"local authenticate\n"		
		"tacacs+ protocol\n"		
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd7,
		"login-mode tacacs radius local [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"				
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd8,
		"login-mode tacacs local radius [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"local authenticate\n"
		"radius protocol\n" 					
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd9,
		"login-mode local radius tacacs [none]",
		"login mode\n"		
		"local authenticate\n"
		"radius protocol\n"
		"tacacs+ protocol\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		aaa_login_mode_set_cmd10,
		"login-mode local tacacs radius [none]",
		"login mode\n"		
		"local authenticate\n"		
		"tacacs+ protocol\n"
		"radius protocol\n"
		"none authenticate\n");

DEFSH(VTYSH_AAA,
		no_aaa_login_mode_set_cmd,
		"no login-mode",
		"no\n"
		"aaa authenticate mode\n");


DEFSH(VTYSH_AAA,
		aaa_authorize_failed_handle_set_cmd,
		"authorize failed online (enable|disable)",
		"authorize\n"
		"authorize failed\n"
		"online\n"
		"enable\n"
		"disable\n");

DEFSH(VTYSH_AAA,
		aaa_account_failed_handle_set_cmd,
		"account failed online (enable|disable)",
		"account\n"
		"account failed\n"
		"online\n"
		"enable\n"
		"disable\n");


DEFSH(VTYSH_AAA,
		aaa_console_admin_mode_set_cmd,
		"console-admin-mode (enable|disable)",
		"console-admin-mode\n"
		"enable\n"
		"disable\n");


DEFSH(VTYSH_AAA,
		aaa_enable_password_set_cmd,
		"enable password WORD [level <0-15>]",
		"enable\n"
		"password\n"
		"Input password, length <1-63>\n"
		"level\n"
		"level range\n");


DEFSH(VTYSH_AAA,
		no_aaa_enable_password_set_cmd,
		"no enable password [level <0-15>]",
		"no\n"
		"enable\n"
		"password\n"
		"level\n"
		"level range\n");


/************************* radius cmd *************************/

DEFSH(VTYSH_AAA,
		radius_auth_server_set_cmd,
		"radius authenticate server A.B.C.D [l3vpn <1-128>]",
		"radius configuration commands\n"
		"authenticate\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n");

DEFSH(VTYSH_AAA,
		no_radius_auth_server_cmd,
		"no radius authenticate server",
		NO_STR
		"radius configuration command\n"
		"authenticate\n"
		"server\n");


DEFSH(VTYSH_AAA,
		radius_acct_server_set_cmd,
		"radius account server A.B.C.D [l3vpn <1-128>]",
		"radius configuration commands\n"
		"account\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n");

DEFSH(VTYSH_AAA,
		no_radius_acct_server_cmd,
		"no radius account server",
		NO_STR
		"radius configuration command\n"
		"account\n"
		"server\n");


DEFSH (VTYSH_AAA,
		radius_key_set_cmd,
		"radius key KEY",
		"radius auth key configure command\n"
		"radius auth key\n"
		"key value, size <1-64>\n");

DEFSH(VTYSH_AAA,
		no_radius_key_cmd,
		"no radius key",
		NO_STR
		"radius configuration command\n"
		"radius authentication key\n");

DEFSH(VTYSH_AAA,
		radius_account_switch_cmd,
		"radius account (enable|disable)",
		"radius mode\n"
		"account function\n"
		"enable account\n"
		"disable account\n");

DEFSH(VTYSH_AAA,
		radius_account_interval_cfg_cmd,
		"radius account interval <1-60>",
		"radius mode\n"
		"account function\n"
		"account interval\n"
		"1-60 minutes\n");


DEFSH(VTYSH_AAA,
		radius_authen_mode_set_cmd,
		"radius authenticate mode (pap|chap)",
		"Radius mode\n"
		"Authenticate\n" 
		"Mode\n"
		"Pap mode\n"
		"Chap mode\n");

DEFSH (VTYSH_AAA,
		no_radius_authen_mode_set_cmd,
		"no radius authenticate mode",
		"No\n"
		"Radius mode\n"
		"Authenticate\n" 
		"Mode\n");


/************************* tac_plus cmd *************************/

DEFSH(VTYSH_AAA,
		tac_plus_authen_server_set_cmd,
		"tacacs authenticate server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"authenticate\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n");

DEFSH(VTYSH_AAA,
		no_tac_plus_authen_server_cmd,
		"no tacacs authenticate server",
		NO_STR
		"tacacs configuration command\n"
		"authenticate\n"
		"server\n");

DEFSH(VTYSH_AAA,
		tac_plus_author_server_set_cmd,
		"tacacs authorize server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"authorize\n" 
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n");

DEFSH(VTYSH_AAA,
		no_tac_plus_author_server_cmd,
		"no tacacs authorize server",
		NO_STR
		"tacacs configuration command\n"
		"authorize\n" 
		"server\n");

DEFSH(VTYSH_AAA,
		tac_plus_acct_server_set_cmd,
		"tacacs account server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"account\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n");

DEFSH(VTYSH_AAA,
		no_tac_plus_acct_server_cmd,
		"no tacacs account server",
		NO_STR
		"tacacs configuration command\n"
		"account\n"
		"server\n");


DEFSH(VTYSH_AAA,
		tac_plus_key_set_cmd,
		"tacacs key KEY",
		"tacacs configuration command\n"
		"tacacs authentication key\n"
		"key value, size <1-64>\n");

DEFSH(VTYSH_AAA,
		no_tac_plus_key_cmd,
		"no tacacs key",
		NO_STR
		"tacacs configuration command\n"
		"tacacs authentication key\n");

DEFSH(VTYSH_AAA,
		tac_plus_authorize_switch_cmd,
		"tacacs authorize (enable|disable)",
		"tac_plus mode\n"
		"authorize function\n"
		"enable account\n"
		"disable account\n");

DEFSH(VTYSH_AAA,
		tac_plus_account_switch_cmd,
		"tacacs account (enable|disable)",
		"tac_plus mode\n"
		"account function\n"
		"enable account\n"
		"disable account\n");

DEFSH(VTYSH_AAA,
		tac_plus_account_interval_cfg_cmd,
		"tacacs account interval <1-60>",
		"tac_plus mode\n"
		"account function\n"
		"account interval\n"
		"1-60 minutes\n");


DEFSH(VTYSH_AAA,
		tacacs_authen_mode_set_cmd,
		"tacacs authenticate mode (pap|chap)",
		"Tacacs mode\n"
		"Authenticate\n" 
		"Mode\n"
		"Pap mode\n"
		"Chap mode\n");


DEFSH(VTYSH_AAA,
		no_tacacs_authen_mode_set_cmd,
		"no tacacs authenticate mode",
		"No\n"
		"Tacacs mode\n"
		"Authenticate\n" 
		"Mode\n");


/************************* local cmd *************************/

DEFSH(VTYSH_AAA,
		local_user_add_cmd,
		"local-user WORD password WORD level <0-14>",
		"Local user\n"
		"Username <1-55>\n"
		"Password\n"
		"Password <1-63>\n"
		"Level\n"
		"0-14 level\n");


DEFSH(VTYSH_AAA,
		local_user_delete_cmd,
		"no local-user WORD",
		"no\n"
		"local user\n"
		"user name\n");

DEFSH(VTYSH_AAA,
		local_user_change_password_cmd,
		"local-user WORD change-password old-password WORD new-password WORD",
		"local user\n"
		"Input user name\n"
		"change password\n"
		"old password\n"
		"Input old password <1-63>\n"
		"new password\n"
		"Input new password <1-63>\n");

DEFSH(VTYSH_AAA,
		local_user_max_repeat_num_config_cmd,
		"local-user WORD max-repeat-number <1-16>",
		"local user\n"
		"Input user name\n"
		"max repeat login number\n"
		"value\n");		

DEFSH(VTYSH_AAA,
		local_user_level_config_cmd,
		"local-user WORD level <0-14>",
		"local user\n"
		"Input user name\n"
		"Level\n"
		"Level value\n");

DEFSH(VTYSH_AAA,
		no_local_user_max_repeat_num_cmd,
		"no local-user WORD max-repeat-number",
		"no\n"
		"local user\n"
		"Input user name\n"
		"max repeat login number\n");

//add for h3c
DEFSH(VTYSH_AAA,
		local_user_service_config_cmd,
		"local-user WORD service-type (ssh|telnet|terminal)",
		"local user\n"
		"Input user name\n"
		"Service type\n"
		"Ssh type\n"
		"Telnet type\n"
		"Terminal type\n");



/************************* none cmd *************************/

DEFSH(VTYSH_AAA,
		none_user_level_config_cmd,
		"none-user level <0-14>",
		"none user\n"
		"level\n"
		"0-14 level");


DEFSH(VTYSH_AAA,
		no_none_user_level_config_cmd,
		"no none-user level",
		"no\n"
		"none user\n"
		"level\n");


/************************* dot1x cmd *************************/

DEFSH(VTYSH_AAA,
		dot1x_switch_set_cmd,
		"dot1x (enable|disable)",
		"Ieee802.1x authenticate\n"
		"Enable\n"
		"Disable\n");



DEFSH(VTYSH_AAA,
		dot1x_interface_switch_set_cmd,
		"dot1x (enable | disable)",
		"Ieee802.1x authenticate\n" 	
		"Enable\n"
		"Disable\n");


DEFSH(VTYSH_AAA,
		dot1x_interface_method_set_cmd,
		"dot1x port-method (portbased|macbased)",
		"Ieee802.1x authenticate\n"
		"Port method\n"
		"Port based\n"
		"Mac based\n");


DEFSH(VTYSH_AAA,
		dot1x_authenticator_server_set_cmd,
		"dot1x authenticator-server (radius|local)",
		"Ieee802.1x authenticate\n"
		"Authenticator server\n"
		"Radius method\n"
		"Local method\n");


DEFSH(VTYSH_AAA,
		dot1x_authenticator_method_set_cmd,
		"dot1x authenticator-method (final|relay)",
		"Ieee802.1x authenticate\n"
		"Authenticator method\n"
		"Final method\n"
		"Relay method\n");


DEFSH(VTYSH_AAA,
		dot1x_cfg_show_cmd,
		"show dot1x config",
		SHOW_STR
		"Ieee802.1x authenticate\n"
		"Config information\n");


DEFSH(VTYSH_AAA,
		dot1x_online_info_show_cmd,
		"show dot1x online",
		SHOW_STR
		"Ieee802.1x authenticate\n"
		"Online information\n");


/************************* debug cmd *************************/

DEFSH(VTYSH_AAA,
	   debug_aaa_packet_enable_cmd,
	   "debug aaa packet (receive|send) (radius|tacacs|all) enable",
	   "Debug control\n"
	   "aaa module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "radius protocol\n"
	   "tacacs protocol\n"
	   "all in aaa\n"
	   "enable\n");

DEFSH(VTYSH_AAA,
	   debug_aaa_packet_disable_cmd,
	   "debug aaa packet (receive|send) (radius|tacacs|all) disable",
	   "Debug control\n"
	   "aaa module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "radius protocol\n"
	   "tacacs protocol\n"
	   "all in aaa\n"
	   "disable\n");

DEFSH(VTYSH_AAA,
	   show_debug_aaa_packet_cmd,
	   "show aaa debug",
	   SHOW_STR    
	   "aaa module\n"
	   "Debug control\n");


DEFSH(VTYSH_AAA, aaa_log_level_ctl_cmd_vtysh, "debug aaa(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n");

DEFSH(VTYSH_AAA, aaa_show_log_level_ctl_cmd_vtysh, "show debug aaa",
	SHOW_STR"Output log of debug level\n""Program name\n");



/********************************************** H3C cmds ******************************************************/
DEFUNSH(VTYSH_AAA,
	  h3c_add_local_user,
	  h3c_add_local_user_cmd_vtysh,
	  "local-user WORD [class manage]",
	  "Local user config\n"
	  "Input user name, <1-55>\n"
	  "Type of local user\n"
	  "Type of device manage user\n")
{
	char *pprompt = vty->change_prompt;

	vty->node = AAA_LOCAL_USER_NODE;
	
    if(pprompt)
    {
        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-luser-manage-%s)# ", argv[0]);
	}

	return CMD_SUCCESS;
}


DEFUNSH(VTYSH_AAA,
	vtysh_exit_aaa_local_user,
	vtysh_exit_aaa_local_user_cmd,
	"exit",
	"Exit current mode and down to previous mode\n")
{
	return vtysh_exit(vty);
}


ALIAS (vtysh_exit_aaa_local_user,
	   vtysh_quit_aaa_local_user_cmd,
	   "quit",
	   "Exit current mode and down to previous mode\n");



DEFSH(VTYSH_AAA,
	h3c_modify_local_user_password_cmd,
	"password simple WORD",
	"Local user password\n"
	"Type of simple\n"
	"Input user password, <1-63>\n");

DEFSH(VTYSH_AAA,
	h3c_modify_local_user_password_cmd1,
	"password (cipher|hash|simple) WORD",
	"Local user password\n"
	"Type of cipher\n"
	"Type of hash\n"
	"Type of simple\n"
	"Input user password, <1-63>\n");

DEFSH(VTYSH_AAA,
	h3c_undo_local_user_password_cmd,
	"undo password",
	"Undo\n"
	"Local user password\n");

DEFSH(VTYSH_AAA,
	h3c_modify_local_user_service_type_cmd,
	"service-type (ssh|telnet|terminal)",	
	"Service type\n"
	"Ssh type\n"
	"Telnet type\n"
	"Terminal type\n");


DEFSH(VTYSH_AAA,
	h3c_undo_local_user_service_type_cmd,
	"undo service-type (ssh|telnet|terminal)",
	"Undo\n"
	"Service type\n"
	"Ssh type\n"
	"Telnet type\n"
	"Terminal type\n");


DEFSH (VTYSH_AAA,
	aaa_debug_monitor_cmd,
	"debug aaa (enable|disable) (common|radius|tacacs|local|none|dot1x|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common messege\n"
	"Type name of radius messege\n"
	"Type name of tacacs messege\n"
	"Type name of local log queue \n"
	"Type name of none log queue \n"
	"Type name of dot1x status\n"
	"Type name of all debug\n");


DEFSH (VTYSH_AAA,
	show_aaa_debug_monitor_cmd,
	"show aaa debug",
	SHOW_STR
	"Syslog"
	"Debug status\n");




void vtysh_init_aaa_cmd (void)
{
	install_node (&aaa_node, NULL);
	install_node (&aaa_local_user_node, NULL);
	vtysh_install_default (AAA_NODE);
	vtysh_install_default (AAA_LOCAL_USER_NODE);

	/* aaa enter cmd */
	install_element_level (CONFIG_NODE, &aaa_mode_enable_cmd, MANAGE_LEVE, CMD_SYNC);

	/* common show cmd */
	install_element_level (CONFIG_NODE, &show_aaa_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &vtysh_exit_aaa_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &vtysh_quit_aaa_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &show_aaa_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &show_login_user_config_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &show_login_user_online_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &show_login_user_log_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &show_login_mode_config_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &aaa_enable_password_show_cmd, MANAGE_LEVE, CMD_LOCAL);

	/* common config cmd */
	install_element_level (AAA_NODE, &aaa_login_max_num_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_aaa_login_max_num_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_user_idle_cut_time_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_aaa_login_user_idle_cut_time_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_user_default_level_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_aaa_login_user_default_level_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd0, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd1, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd2, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd3, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd4, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd5, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd6, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd7, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd8, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd9, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_login_mode_set_cmd10, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_aaa_login_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_authorize_failed_handle_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_account_failed_handle_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_console_admin_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &aaa_enable_password_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_aaa_enable_password_set_cmd, MANAGE_LEVE, CMD_SYNC);

	/* radius cmd */
	install_element_level (AAA_NODE, &radius_auth_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_radius_auth_server_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &radius_acct_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_radius_acct_server_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &radius_key_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_radius_key_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &radius_account_switch_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &radius_account_interval_cfg_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &radius_authen_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_radius_authen_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);

	/* tacacs cmd */
	install_element_level (AAA_NODE, &tac_plus_authen_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_tac_plus_authen_server_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_author_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_tac_plus_author_server_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_acct_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_tac_plus_acct_server_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_key_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_tac_plus_key_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_authorize_switch_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_account_switch_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tac_plus_account_interval_cfg_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &tacacs_authen_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_tacacs_authen_mode_set_cmd, MANAGE_LEVE, CMD_SYNC);

	/* local cmd */
	install_element_level (AAA_NODE, &local_user_add_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &local_user_delete_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &local_user_change_password_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &local_user_max_repeat_num_config_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &local_user_level_config_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_local_user_max_repeat_num_cmd, MANAGE_LEVE, CMD_SYNC);
	//add for h3c
	install_element_level (AAA_NODE, &local_user_service_config_cmd, MANAGE_LEVE, CMD_SYNC);

	/* none cmd */
	install_element_level (AAA_NODE, &none_user_level_config_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &no_none_user_level_config_cmd, MANAGE_LEVE, CMD_SYNC);

	/* dot1x cmd */
	install_element_level (AAA_NODE, &dot1x_switch_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (PHYSICAL_IF_NODE, &dot1x_interface_switch_set_cmd, MANAGE_LEVE, CMD_SYNC);
	//install_element_level (PHYSICAL_IF_NODE, &dot1x_interface_method_set_cmd, MANAGE_LEVE, CMD_SYNC); //基于port/mac的先不做，待扩展
	install_element_level (AAA_NODE, &dot1x_authenticator_server_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &dot1x_authenticator_method_set_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &dot1x_cfg_show_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_NODE, &dot1x_online_info_show_cmd, MANAGE_LEVE, CMD_LOCAL);
		
	/* debug cmd */
	install_element_level (AAA_NODE, &debug_aaa_packet_enable_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &debug_aaa_packet_disable_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (AAA_NODE, &show_debug_aaa_packet_cmd, MANAGE_LEVE, CMD_LOCAL);

	//install_element_level (CONFIG_NODE, &aaa_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_SYNC);
	//install_element_level (CONFIG_NODE, &aaa_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);


	/* H3C cmds */
	install_element_level (CONFIG_NODE, &h3c_add_local_user_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &vtysh_exit_aaa_local_user_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &vtysh_quit_aaa_local_user_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_password_cmd, MANAGE_LEVE, CMD_LOCAL);
	//install_element_level (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_password_cmd1, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &h3c_undo_local_user_password_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_service_type_cmd, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (AAA_LOCAL_USER_NODE, &h3c_undo_local_user_service_type_cmd, MANAGE_LEVE, CMD_LOCAL);

	/* new debug */
	install_element_level (CONFIG_NODE, &aaa_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
	install_element_level (CONFIG_NODE, &show_aaa_debug_monitor_cmd, MANAGE_LEVE, CMD_SYNC);
}



