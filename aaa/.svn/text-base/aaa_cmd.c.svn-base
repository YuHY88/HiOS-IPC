/******************************************************************************
 * Filename: aaa_cmd.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.9.14  luoyz created
 *
******************************************************************************/

//#include <arpa/inet.h>
#include <string.h>

#include <lib/vty.h>
#include <lib/command.h>
#include <lib/inet_ip.h>
#include <lib/aaa_common.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/log.h>

#include "aaa.h"
#include "aaa_config.h"
#include "aaa_user_manager.h"
#include "aaa_cmd.h"
#include "aaa_radius.h"
#include "aaa_tac_plus.h"
#include "aaa_packet.h"
#include "aaa_dot1x.h"


extern aaa_pkt_debug aaa_pkt_dbg;
extern int syslog_debug;

#if 0
static int aaa_check_illegal_char(const char* p, unsigned int length)
{
	while(length--)
	{
		if(((*p >= '0') && (*p <= '9')) ||\
		   ((*p >= 'a') && (*p <= 'z')) ||\
		   ((*p >= 'A') && (*p <= 'Z')))
		{
			p++;
		}
		else
		{
			//printf("illegal char\n");
			return AAA_ERROR;
		}
	}
	return AAA_OK;
}
#endif


/**************************** aaa node ****************************/


DEFUN (aaa_mode_enable,
		aaa_mode_enable_cmd,
		"aaa",
		"aaa command node\n")
{
	vty->node = AAA_NODE;

	return CMD_SUCCESS;
}

/************************* common show cmd *************************/

DEFUN(show_aaa_fun,
		show_aaa_cmd,
		"show aaa config",
		SHOW_STR
		"show aaa cfg\n"
		"Config\n")
{
	aaa_show_config(vty);
	return CMD_SUCCESS;
}


DEFUN(show_login_user_config_fun,
		show_login_user_config_cmd,
		"show login-user config",
		SHOW_STR
		"login user\n"
		"user config information\n")
{
	aaa_show_user_config(vty);
	return CMD_SUCCESS;
}

DEFUN(show_login_user_online_fun,
		show_login_user_online_cmd,
		"show login-user online {mode (radius|tacacs|local|none)}",
		SHOW_STR
		"login user\n"
		"user online information\n"
		"login mode\n"
		"radius\n"
		"tacacs\n"
		"local\n"
		"none\n")
{
	aaa_show_user_online(vty, argv[0]);
	return CMD_SUCCESS;
}

DEFUN(show_login_user_log_fun,
		show_login_user_log_cmd,
		"show login-user log {mode (radius|tacacs|local|none)}",
		SHOW_STR
		"login user\n"
		"user log information\n"
		"login mode\n"
		"radius\n"
		"tacacs\n"
		"local\n"
		"none\n")
{
	aaa_show_user_log(vty, argv[0]);
	return CMD_SUCCESS;
}

DEFUN(show_login_mode_config_fun,
		show_login_mode_config_cmd,
		"show login-mode config",
		SHOW_STR
		"login mode\n"
		"login mode config information\n")
{
	aaa_show_login_mode(vty);
	return CMD_SUCCESS;
}


DEFUN(aaa_enable_password_show_fun,
		aaa_enable_password_show_cmd,
		"show enable password [level <0-15>]",
		SHOW_STR
		"enable\n"
		"password\n")
{
	aaa_show_enable_password(vty, argv[0]);
	return CMD_SUCCESS;
}

/************************* common config cmd *************************/

DEFUN(aaa_login_max_num_set_fun,
		aaa_login_max_num_set_cmd,
		"login-user max-number <1-32>",
		"login user\n"
		"max\n"
		"max number\n")
{
	if(NULL == argv[0])
	{
		return CMD_WARNING;
	}

	aaa_login_max_num_set((int)atoi(argv[0]));
	return CMD_SUCCESS;
}

DEFUN(no_aaa_login_max_num_fun,
		no_aaa_login_max_num_cmd,
		"no login-user max-number",
		"no\n"
 		"login user\n"
		"max number\n")
{
	aaa_login_max_num_set(AAA_ONLINE_NUM_MAX);
	return CMD_SUCCESS;
}


DEFUN(aaa_login_user_idle_cut_time_set_fun,
		aaa_login_user_idle_cut_time_set_cmd,
		"login-user idle-cut <0-30>",
		"login user\n"
		"idle-cut\n"
		"0-30 minutes\n")
{
	if(NULL == argv[0])
		return CMD_WARNING;

	aaa_user_max_idle_time_set((time_t)atoi(argv[0]) * 60);

	aaa_send_max_idle_time_msg((time_t)atoi(argv[0]) * 60);
	
	return CMD_SUCCESS;
}

DEFUN(no_aaa_login_user_idle_cut_time_fun,
		no_aaa_login_user_idle_cut_time_cmd,
		"no login-user idle-cut",
		"no\n"
		"login user\n"
		"idle-cut\n")
{
	aaa_user_max_idle_time_set(AAA_DEF_IDLE_MAX * 60);

	aaa_send_max_idle_time_msg(AAA_DEF_IDLE_MAX * 60);
	
	return CMD_SUCCESS;
}


DEFUN(aaa_login_user_default_level_set_fun,
		aaa_login_user_default_level_set_cmd,
		"login-user default-level <0-14>",
		"login user\n"
		"default-level\n"
		"0-14\n")
{
	if(NULL == argv[0])
		return CMD_WARNING;

	aaa_user_def_priv_set((int)atoi(argv[0]));
	return CMD_SUCCESS;
}

DEFUN(no_aaa_login_user_default_level_set_fun,
		no_aaa_login_user_default_level_set_cmd,
		"no login-user default-level",
		"no\n"
		"login user\n"
		"default-level\n")
{
	aaa_user_def_priv_set(AAA_DEF_LOGIN_PRIVILEGE);
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun0,
		aaa_login_mode_set_cmd0,
		"login-mode none",
		"login mode\n"
		"none authenticate\n")
{
	aaa_set_auth_order(METHOD_NONE, METHOD_INVALID, METHOD_INVALID, METHOD_INVALID);
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun1,
		aaa_login_mode_set_cmd1,
		"login-mode (tacacs|radius|local) [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"
		"none authenticate\n")
{
	LOG_METHOD mode = METHOD_INVALID;
	if(0 == memcmp("tacacs", argv[0], 3))
		mode = METHOD_TACACS;
	else if(0 == memcmp("radius", argv[0], 3))
		mode = METHOD_RADIUS;
	else if(0 == memcmp("local", argv[0], 3))
		mode = METHOD_LOCAL;

	if(NULL == argv[1])
		aaa_set_auth_order(mode, METHOD_INVALID, METHOD_INVALID, METHOD_INVALID);
	else
		aaa_set_auth_order(mode, METHOD_NONE, METHOD_INVALID, METHOD_INVALID);
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun2,
		aaa_login_mode_set_cmd2,
		"login-mode radius (tacacs|local) [none]",
		"login mode\n"
		"radius protocol\n"
		"tacacs+ protocol\n"		
		"local authenticate\n"
		"none authenticate\n")
{
	LOG_METHOD mode = METHOD_INVALID;
	if(0 == memcmp("tacacs", argv[0], 3))
		mode = METHOD_TACACS;
	else if(0 == memcmp("local", argv[0], 3))
		mode = METHOD_LOCAL;

	if(NULL == argv[1])
		aaa_set_auth_order(METHOD_RADIUS, mode, METHOD_INVALID, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_RADIUS, mode, METHOD_NONE, METHOD_INVALID);
	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun3,
		aaa_login_mode_set_cmd3,
		"login-mode tacacs (radius|local) [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"
		"none authenticate\n")
{
	LOG_METHOD mode = METHOD_INVALID;
	if(0 == memcmp("radius", argv[0], 3))
		mode = METHOD_RADIUS;
	else if(0 == memcmp("local", argv[0], 3))
		mode = METHOD_LOCAL;

	if(NULL == argv[1])
		aaa_set_auth_order(METHOD_TACACS, mode, METHOD_INVALID, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_TACACS, mode, METHOD_NONE, METHOD_INVALID);
	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun4,
		aaa_login_mode_set_cmd4,
		"login-mode local (radius|tacacs) [none]",
		"login mode\n"
		"local authenticate\n"
		"radius protocol\n"
		"tacacs+ protocol\n"		
		"none authenticate\n")
{
	LOG_METHOD mode = METHOD_INVALID;
	if(0 == memcmp("radius", argv[0], 3))
		mode = METHOD_RADIUS;
	else if(0 == memcmp("tacacs", argv[0], 3))
		mode = METHOD_TACACS;

	if(NULL == argv[1])
		aaa_set_auth_order(METHOD_LOCAL, mode, METHOD_INVALID, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_LOCAL, mode, METHOD_NONE, METHOD_INVALID);
	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun5,
		aaa_login_mode_set_cmd5,
		"login-mode radius tacacs local [none]",
		"login mode\n"
		"radius protocol\n"
		"tacacs+ protocol\n"
		"local authenticate\n"				
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_RADIUS, METHOD_TACACS, METHOD_LOCAL, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_RADIUS, METHOD_TACACS, METHOD_LOCAL, METHOD_NONE);	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun6,
		aaa_login_mode_set_cmd6,
		"login-mode radius local tacacs [none]",
		"login mode\n"
		"radius protocol\n"
		"local authenticate\n"		
		"tacacs+ protocol\n"		
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_RADIUS, METHOD_LOCAL, METHOD_TACACS, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_RADIUS, METHOD_LOCAL, METHOD_TACACS, METHOD_NONE);	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun7,
		aaa_login_mode_set_cmd7,
		"login-mode tacacs radius local [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"radius protocol\n"
		"local authenticate\n"				
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_TACACS, METHOD_RADIUS, METHOD_LOCAL, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_TACACS, METHOD_RADIUS, METHOD_LOCAL, METHOD_NONE);	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun8,
		aaa_login_mode_set_cmd8,
		"login-mode tacacs local radius [none]",
		"login mode\n"
		"tacacs+ protocol\n"
		"local authenticate\n"
		"radius protocol\n"						
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_TACACS, METHOD_LOCAL, METHOD_RADIUS, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_TACACS, METHOD_LOCAL, METHOD_RADIUS, METHOD_NONE);	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun9,
		aaa_login_mode_set_cmd9,
		"login-mode local radius tacacs [none]",
		"login mode\n"		
		"local authenticate\n"
		"radius protocol\n"
		"tacacs+ protocol\n"
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_LOCAL, METHOD_RADIUS, METHOD_TACACS, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_LOCAL, METHOD_RADIUS, METHOD_TACACS, METHOD_NONE);	
	return CMD_SUCCESS;
}

DEFUN(aaa_login_mode_set_fun10,
		aaa_login_mode_set_cmd10,
		"login-mode local tacacs radius [none]",
		"login mode\n"		
		"local authenticate\n"		
		"tacacs+ protocol\n"
		"radius protocol\n"
		"none authenticate\n")
{
	if(NULL == argv[0])
		aaa_set_auth_order(METHOD_LOCAL, METHOD_TACACS, METHOD_RADIUS, METHOD_INVALID);
	else
		aaa_set_auth_order(METHOD_LOCAL, METHOD_TACACS, METHOD_RADIUS, METHOD_NONE);	
	return CMD_SUCCESS;
}		

DEFUN(no_aaa_login_mode_set_fun,
		no_aaa_login_mode_set_cmd,
		"no login-mode",
		"no\n"
		"aaa authenticate mode\n")
{
	aaa_set_auth_order(METHOD_RADIUS, METHOD_LOCAL, METHOD_INVALID, METHOD_INVALID);
	return CMD_SUCCESS;
}


DEFUN(aaa_authorize_failed_handle_set_fun,
		aaa_authorize_failed_handle_set_cmd,
		"authorize failed online (enable|disable)",
		"authorize\n"
		"authorize failed\n"
		"online\n"
		"enable\n"
		"disable\n")
{
	if(NULL == argv[0])
		return CMD_WARNING;

	if(0 == memcmp("enable", argv[0], 1))
		aaa_online_author_failed_set(ENABLE);
	else if(0 == memcmp("disable", argv[0], 1))
	{
		if(DISABLE == tac_plus_author_server_switch_get())
		{
			vty_error_out (vty, "Tacacs authorize must be enabled first!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		aaa_online_author_failed_set(DISABLE);		
	}
		
	return CMD_SUCCESS;
}

DEFUN(aaa_account_failed_handle_set_fun,
		aaa_account_failed_handle_set_cmd,
		"account failed online (enable|disable)",
		"account\n"
		"account failed\n"
		"online\n"
		"enable\n"
		"disable\n")
{
	if(NULL == argv[0])
		return CMD_WARNING;

	if(0 == memcmp("enable", argv[0], 1))
		aaa_online_acct_failed_set(ENABLE);
	else if(0 == memcmp("disable", argv[0], 1))
	{		
		if(DISABLE == tac_plus_acct_server_switch_get())
		{
			vty_error_out (vty, "Tacacs account must be enabled first!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		if(DISABLE == radius_acct_server_switch_get())
		{
			vty_error_out (vty, "Radius account must be enabled first!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		aaa_online_acct_failed_set(DISABLE);
	}
		
	return CMD_SUCCESS;
}


DEFUN(aaa_console_admin_mode_set_fun,
		aaa_console_admin_mode_set_cmd,
		"console-admin-mode (enable|disable)",
		"console-admin-mode\n"
		"enable\n"
		"disable\n")
{
	if(NULL == argv[0])
		return CMD_WARNING;

	if(0 == memcmp("enable", argv[0], 3))
		aaa_console_admin_mode_set(ENABLE);
	else if(0 == memcmp("disable", argv[0], 3))
		aaa_console_admin_mode_set(DISABLE);
	return CMD_SUCCESS;
}


DEFUN(aaa_enable_password_set_fun,
		aaa_enable_password_set_cmd,
		"enable password WORD [level <0-15>]",
		"enable\n"
		"password\n"
		"Input password, length <1-63>\n"
		"level\n"
		"level range\n")
{
	if((strlen(argv[0]) > USER_PASSWORD_MAX) || (strlen(argv[0]) < USER_PASSWORD_MIN))
	{
		vty_error_out (vty, "The length of password is too long or too short, size <%d-%d>!%s", 
			USER_PASSWORD_MIN, USER_PASSWORD_MAX, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(NULL == argv[1])
	{
		for(int ret = 0; ret < 16; ret++)
			aaa_enable_password_set(argv[0], ret);		
	}
	else
	{
		aaa_enable_password_set(argv[0], atoi(argv[1]));
	}

	return CMD_SUCCESS;
}


DEFUN(no_aaa_enable_password_set_fun,
		no_aaa_enable_password_set_cmd,
		"no enable password [level <0-15>]",
		"no\n"
		"enable\n"
		"password\n"
		"level\n"
		"level range\n")
{
	if(NULL == argv[0])
		aaa_enable_init();
	else
		aaa_enable_password_set(DEF_ENABLE_PASSWORD, atoi(argv[0]));

	return CMD_SUCCESS;
}


/************************* radius cmd *************************/

DEFUN(radius_auth_server_set_fun,
		radius_auth_server_set_cmd,
		"radius authenticate server A.B.C.D [l3vpn <1-128>]",
		"radius configuration commands\n"
		"authenticate\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n")
{
	struct sockaddr_in srv;
	uint32_t ipv4 = 0;	
	uint16_t vpn = 0;

	radius_auth_server_get(&srv);
	if(inet_pton(AF_INET, argv[0], &ipv4) != 1)
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	//ipv4 = ntohl(ipv4);

	if(FALSE == inet_valid_ipv4(ntohl(ipv4)))
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	srv.sin_addr.s_addr = ipv4;
	if(radius_auth_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Config radius authentication server failed!%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	if(argv[1])
	{
		vpn = (uint16_t)atoi(argv[1]);		
	}
	else
	{
		vpn = 0;
	}
	radius_auth_server_vpn_set(vpn);

	return CMD_SUCCESS;
}

DEFUN(no_radius_auth_server_fun,
		no_radius_auth_server_cmd,
		"no radius authenticate server",
		NO_STR
		"radius configuration command\n"
		"authenticate\n"
		"server\n")
{
	struct sockaddr_in srv;
	srv.sin_addr.s_addr = htonl(RADIUS_DEF_SEVR_IPV4);
	srv.sin_port = htons(RADIUS_AUTH_UDP_PORT);
	if(radius_auth_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Reset default server failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(radius_acct_server_set_fun,
		radius_acct_server_set_cmd,
		"radius account server A.B.C.D [l3vpn <1-128>]",
		"radius configuration commands\n"
		"account\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n")
{
	struct sockaddr_in srv;
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;

	radius_acct_server_get(&srv);
	if(inet_pton(AF_INET, argv[0], &ipv4) != 1)
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	//ipv4 = ntohl(ipv4);

	if(FALSE == inet_valid_ipv4(ntohl(ipv4)))
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	srv.sin_addr.s_addr = ipv4;
	if(radius_acct_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Config radius account server failed!%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	if(argv[1])
	{
		vpn = (uint16_t)atoi(argv[1]);		
	}
	else
	{
		vpn = 0;
	}
	radius_acct_server_vpn_set(vpn);

	return CMD_SUCCESS;
}

DEFUN(no_radius_acct_server_fun,
		no_radius_acct_server_cmd,
		"no radius account server",
		NO_STR
		"radius configuration command\n"
		"account\n"
		"server\n")
{
	struct sockaddr_in srv;
	srv.sin_addr.s_addr = htonl(RADIUS_DEF_SEVR_IPV4);
	srv.sin_port = htons(RADIUS_ACCT_UDP_PORT);
	if(radius_acct_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Reset default server failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN (radius_key_set_fun,
		radius_key_set_cmd,
		"radius key KEY",
		"radius auth key configure command\n"
		"radius auth key\n"
		"key value, size <1-64>\n")
{
	if(strlen(argv[0]) > RADIUS_SHARED_SECRET_LEN)
	{
		vty_error_out (vty, "Cmd key is too long to match. Key size <1-64>.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(AAA_OK != radius_key_set(argv[0]))
	{
		vty_error_out (vty, "Set radius key failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(no_radius_key_fun,
		no_radius_key_cmd,
		"no radius key",
		NO_STR
		"radius configuration command\n"
		"radius authentication key\n")
{
	char secret_def[32];
	memset(secret_def, 0, sizeof(secret_def));
	memcpy(secret_def, RADIUS_DEF_SECRET, sizeof(RADIUS_DEF_SECRET));
	if(radius_key_set(secret_def) != AAA_OK)
	{
		vty_error_out (vty, "Reset default key failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(radius_account_switch_fun,
		radius_account_switch_cmd,
		"radius account (enable|disable)",
		"radius mode\n"
		"account function\n"
		"enable account\n"
		"disable account\n")
{
	if(0 == memcmp("enable", argv[0], 2))
		radius_acct_server_switch_set(ENABLE);
	else
	{
		if(DISABLE == aaa_online_acct_failed_get ())
		{			
			vty_error_out (vty, "Radius mode has been configured and account failed online is disable!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		radius_acct_server_switch_set(DISABLE);
	}		
	
	return CMD_SUCCESS;
}

DEFUN(radius_account_interval_cfg_fun,
		radius_account_interval_cfg_cmd,
		"radius account interval <1-60>",
		"radius mode\n"
		"account function\n"
		"account interval\n"
		"1-60 minutes\n")
{	
	radius_acct_update_interval_set((u_int32_t)atoi(argv[0]) * 60);
	return CMD_SUCCESS;
}

		
DEFUN (radius_authen_mode_set_fun,
		radius_authen_mode_set_cmd,
		"radius authenticate mode (pap|chap)",
		"Radius mode\n"
		"Authenticate\n" 
		"Mode\n"
		"Pap mode\n"
		"Chap mode\n")
{
	if(0 == memcmp("pap", argv[0], 2))
		radius_authen_mode_set (MODE_PAP);
	else
		radius_authen_mode_set (MODE_CHAP);

	return CMD_SUCCESS;
}

DEFUN (no_radius_authen_mode_set_fun,
		no_radius_authen_mode_set_cmd,
		"no radius authenticate mode",
		"No\n"
		"Radius mode\n"
		"Authenticate\n" 
		"Mode\n")
{
	radius_authen_mode_set (MODE_PAP);
	return CMD_SUCCESS;
}



/************************* tac_plus cmd *************************/

DEFUN(tac_plus_authen_server_set_fun,
		tac_plus_authen_server_set_cmd,
		"tacacs authenticate server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"authenticate\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n")
{
	struct sockaddr_in srv;
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;

	tac_plus_authen_server_get(&srv);
	if(inet_pton(AF_INET, argv[0], &ipv4) != 1)
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	//ipv4 = ntohl(ipv4);

	if(FALSE == inet_valid_ipv4(ntohl(ipv4)))
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	srv.sin_addr.s_addr = ipv4;
	if(tac_plus_authen_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Config tacacs authentication server failed!%s", VTY_NEWLINE);
        return CMD_WARNING;
	}

	if(argv[1])
	{
		vpn = (uint16_t)atoi(argv[1]);		
	}
	else
	{
		vpn = 0;
	}
	tac_plus_authen_server_vpn_set(vpn);

	return CMD_SUCCESS;
}

DEFUN(no_tac_plus_auth_server_fun,
		no_tac_plus_authen_server_cmd,
		"no tacacs authenticate server",
		NO_STR
		"tacacs configuration command\n"
		"authenticate\n"
		"server\n")
{
	struct sockaddr_in srv;
	srv.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	srv.sin_port = htons(TAC_PLUS_PORT);
	if(tac_plus_authen_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Reset default server failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(tac_plus_author_server_set_fun,
		tac_plus_author_server_set_cmd,
		"tacacs authorize server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"authorize\n" 
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n")
{
	struct sockaddr_in srv;
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;

	tac_plus_author_server_get(&srv);
	if(inet_pton(AF_INET, argv[0], &ipv4) != 1)
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	//ipv4 = ntohl(ipv4);

	if(FALSE == inet_valid_ipv4(ntohl(ipv4)))
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	srv.sin_addr.s_addr = ipv4;
	if(tac_plus_author_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Config tacacs authentication server failed!%s", VTY_NEWLINE);		
        return CMD_WARNING;
	}

	if(argv[1])
	{
		vpn = (uint16_t)atoi(argv[1]);		
	}
	else
	{
		vpn = 0;
	}
	tac_plus_author_server_vpn_set(vpn);
	

	return CMD_SUCCESS;
}

DEFUN(no_tac_plus_author_server_fun,
		no_tac_plus_author_server_cmd,
		"no tacacs authorize server",
		NO_STR
		"tacacs configuration command\n"
		"authorize\n" 
		"server\n")
{
	struct sockaddr_in srv;
	srv.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	srv.sin_port = htons(TAC_PLUS_PORT);
	if(tac_plus_author_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Reset default server failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(tac_plus_acct_server_set_fun,
		tac_plus_acct_server_set_cmd,
		"tacacs account server A.B.C.D [l3vpn <1-128>]",
		"tacacs configuration commands\n"
		"account\n"
		"server\n"
		"IPv4 dotted-decimal format A.B.C.D\n"
		"L3VPN\n"
		"L3VPN id <1-128>\n")
{
	struct sockaddr_in srv;
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;

	tac_plus_acct_server_get(&srv);
	if(inet_pton(AF_INET, argv[0], &ipv4) != 1)
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	//ipv4 = ntohl(ipv4);

	if(FALSE == inet_valid_ipv4(ntohl(ipv4)))
	{
		vty_error_out (vty, "The input ip is invaild!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	srv.sin_addr.s_addr = ipv4;
	if(tac_plus_acct_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Config tacacs authentication server failed!%s",
			VTY_NEWLINE);
        return CMD_WARNING;
	}

	if(argv[1])
	{
		vpn = (uint16_t)atoi(argv[1]);		
	}
	else
	{
		vpn = 0;
	}
	tac_plus_acct_server_vpn_set(vpn);

	return CMD_SUCCESS;
}

DEFUN(no_tac_plus_acct_server_fun,
		no_tac_plus_acct_server_cmd,
		"no tacacs account server",
		NO_STR
		"tacacs configuration command\n"
		"account\n"
		"server\n")
{
	struct sockaddr_in srv;
	srv.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	srv.sin_port = htons(TAC_PLUS_PORT);
	if(tac_plus_acct_server_set(&srv) != AAA_OK)
	{
		vty_error_out (vty, "Reset default server failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}


DEFUN(tac_plus_key_set_fun,
		tac_plus_key_set_cmd,
		"tacacs key KEY",
		"tacacs configuration command\n"
		"tacacs authentication key\n"
		"key value, size <1-64>\n")
{
	if(strlen(argv[0]) > TAC_PLUS_SHARED_SECRET_LEN)
	{
		vty_error_out (vty, "Cmd key is too long to match. Key size <1-64>.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	if(tac_plus_key_set(argv[0]) != AAA_OK)
	{
		vty_error_out (vty, "Set tacacs secret failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(no_tac_plus_key_fun,
		no_tac_plus_key_cmd,
		"no tacacs key",
		NO_STR
		"tacacs configuration command\n"
		"tacacs authentication key\n")
{	
	char secret_def[32];
	memset(secret_def, 0, sizeof(secret_def));
	memcpy(secret_def, TAC_PLUS_DEF_SHARED_SECRET, sizeof(TAC_PLUS_DEF_SHARED_SECRET));
	if(tac_plus_key_set(secret_def) != AAA_OK)
	{
		vty_error_out (vty, "Reset default key failed!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(tac_plus_authorize_switch_fun,
		tac_plus_authorize_switch_cmd,
		"tacacs authorize (enable|disable)",
		"tac_plus mode\n"
		"authorize function\n"
		"enable account\n"
		"disable account\n")
{
	if(0 == memcmp("enable", argv[0], 2))
		tac_plus_author_server_switch_set(ENABLE);
	else
	{
		if(DISABLE == aaa_online_author_failed_get ())
		{	
			vty_error_out (vty, "Tacacs mode has been configured and authorize failed online is disable!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		tac_plus_author_server_switch_set(DISABLE);
	}		
	
	return CMD_SUCCESS;
}


DEFUN (tacacs_authen_mode_set_fun,
		tacacs_authen_mode_set_cmd,
		"tacacs authenticate mode (pap|chap)",
		"Tacacs mode\n"
		"Authenticate\n" 
		"Mode\n"
		"Pap mode\n"
		"Chap mode\n")
{
	if(0 == memcmp("pap", argv[0], 2))
		tac_plus_authen_mode_set(TAC_PLUS_AUTHEN_TYPE_PAP);
	else
		tac_plus_authen_mode_set(TAC_PLUS_AUTHEN_TYPE_CHAP);

	return CMD_SUCCESS;
}

DEFUN (no_tacacs_authen_mode_set_fun,
		no_tacacs_authen_mode_set_cmd,
		"no tacacs authenticate mode",
		"No\n"
		"Tacacs mode\n"
		"Authenticate\n" 
		"Mode\n")
{
	tac_plus_authen_mode_set(TAC_PLUS_AUTHEN_TYPE_CHAP);
	return CMD_SUCCESS;
}

		

DEFUN(tac_plus_account_switch_fun,
		tac_plus_account_switch_cmd,
		"tacacs account (enable|disable)",
		"tac_plus mode\n"
		"account function\n"
		"enable account\n"
		"disable account\n")
{
	if(0 == memcmp("enable", argv[0], 2))
		tac_plus_acct_server_switch_set(ENABLE);
	else
	{
		if(DISABLE == aaa_online_acct_failed_get ())
		{			
			vty_error_out (vty, "Tacacs mode has been configured and account failed online is disable!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		tac_plus_acct_server_switch_set(DISABLE);
	}	
	return CMD_SUCCESS;
}

DEFUN(tac_plus_account_interval_cfg_fun,
		tac_plus_account_interval_cfg_cmd,
		"tacacs account interval <1-60>",
		"tac_plus mode\n"
		"account function\n"
		"account interval\n"
		"1-60 minutes\n")
{
	tac_plus_acct_update_interval_set(60 * (u_int32_t)atoi(argv[0]));
	return CMD_SUCCESS;
}



/************************* local cmd *************************/

DEFUN(local_user_add_fun,
		local_user_add_cmd,
		"local-user WORD password WORD level <0-14>",
		"Local user\n"
		"Username <1-55>\n"
		"Password\n"
		"Password <1-63>\n"
		"Level\n"
		"0-14 level\n")
{	
	if((0 == strcmp(SUPER_USER_NAME, argv[0])) ||
	   ((0 == strcmp(ADMIN_USER_NAME, argv[0])) && (vty->config_read_flag == 0)))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name(argv[0]);
	
	if(NULL == pinfo)
	{
		pinfo = (USER_CONFIG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_CONFIG_INFO));
		if(NULL == pinfo)
			return CMD_WARNING;

		memset (pinfo, 0, sizeof(USER_CONFIG_INFO));
		memset (pinfo->local_auth_user.username, 0, USER_NAME_MAX + 1);
		memset (pinfo->local_auth_user.password, 0, USER_PASSWORD_MAX + 1);
		memset (pinfo->local_auth_user.password_encrypted, 0, USER_PASSWORD_MAX + 1);
		pinfo->local_auth_user.encrypted = FALSE;

		pinfo->local_auth_user.service |= (SERVICE_TYPE_SSH|SERVICE_TYPE_TELNET|SERVICE_TYPE_TERMINAL);

		if((strlen(argv[0]) <= USER_NAME_MAX) && (strlen(argv[0]) >= USER_NAME_MIN))
		{
			memcpy(pinfo->local_auth_user.username, argv[0], strlen(argv[0]));
		}
		else
		{
			vty_error_out (vty, "Cmd username is too short or too long, size <%d-%d>.%s", 
				USER_NAME_MIN, USER_NAME_MAX, VTY_NEWLINE);
			return CMD_WARNING;
		}
		
		if((strlen(argv[1]) < USER_PASSWORD_MIN) || (strlen(argv[1]) > USER_PASSWORD_MAX))
		{
			vty_error_out (vty, "Cmd password is too short or too long, size <%d-%d>.%s", 
				USER_PASSWORD_MIN, USER_PASSWORD_MAX, VTY_NEWLINE);
			return CMD_WARNING;
		}
		else
		{
			if (vty->config_read_flag == 1)
			{
				memcpy (pinfo->local_auth_user.password_encrypted, argv[1], strlen(argv[1]));
				aaa_password_decode (pinfo->local_auth_user.password, pinfo->local_auth_user.password_encrypted);
			}
			else
			{
				memcpy(pinfo->local_auth_user.password, argv[1], strlen(argv[1]));
				aaa_password_encode (pinfo->local_auth_user.password_encrypted, pinfo->local_auth_user.password);
			}				
		}
		
		pinfo->local_auth_user.level = (int)atoi(argv[2]);
		pinfo->local_auth_user.max_repeat_num = AAA_DEF_LOCAL_MAX_REPEAT_NUM;
		aaa_user_config_add (pinfo);
		return CMD_SUCCESS;
	}
	else
	{
		if(0 == strcmp(pinfo->local_auth_user.username, ADMIN_USER_NAME))
		{
			memset(pinfo->local_auth_user.password, 0, USER_PASSWORD_MAX + 1);
			
			memcpy (pinfo->local_auth_user.password_encrypted, argv[1], strlen(argv[1]));
			aaa_password_decode (pinfo->local_auth_user.password, pinfo->local_auth_user.password_encrypted);

			pinfo->local_auth_user.level = (int)atoi(argv[2]);
			return CMD_SUCCESS;
		}
		else
		{
			vty_error_out (vty, "User %s already exists.%s", argv[0], VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
}


DEFUN(local_user_delete_fun,
		local_user_delete_cmd,
		"no local-user WORD",
		"no\n"
		"local user\n"
		"user name\n")
{
	if((strlen(SUPER_USER_NAME) == strlen(argv[0])) && (0 == strcmp(SUPER_USER_NAME, argv[0])))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	USER_CONFIG_INFO *pinfo = NULL;
	if((NULL != argv[0]) && (strlen(argv[0]) <= USER_NAME_MAX))
	{
		if((strlen(argv[0]) == strlen(ADMIN_USER_NAME))\
			&& (0 == strncmp(ADMIN_USER_NAME, argv[0], strlen(ADMIN_USER_NAME))))
		{
			vty_error_out (vty, "User %s can not be deleted!%s", argv[0], VTY_NEWLINE);
			return CMD_WARNING;
		}
		pinfo = aaa_user_config_get_by_name(argv[0]);
		if(NULL != pinfo)
		{
			aaa_user_config_del(pinfo->user_id);
			return CMD_SUCCESS;
		}		
		else
		{
			vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
			return CMD_WARNING;
		}
	}		
	else if(strlen(argv[0]) > USER_NAME_MAX)
	{
		vty_error_out (vty, "Cmd username is too short or too long, size <%d-%d>.%s", 
			USER_NAME_MIN, USER_NAME_MAX, VTY_NEWLINE);
		return CMD_WARNING;
	}
	return CMD_WARNING;
}

DEFUN(local_user_change_password_fun,
		local_user_change_password_cmd,
		"local-user WORD change-password old-password WORD new-password WORD",
		"local user\n"
		"Input user name\n"
		"change password\n"
		"old password\n"
		"Input old password <1-63>\n"
		"new password\n"
		"Input new password <1-63>\n")
{
	if((strlen(SUPER_USER_NAME) == strlen(argv[0])) && (0 == strcmp(SUPER_USER_NAME, argv[0])))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(strlen(argv[0]) > USER_NAME_MAX)
	{
		vty_error_out (vty, "Cmd username is too short or too long, size <%d-%d>.%s", 
			USER_NAME_MIN, USER_NAME_MAX, VTY_NEWLINE);
		return CMD_WARNING;
	}

	if ((strlen(argv[1]) == strlen(argv[2])) && (0 == strcmp (argv[1], argv[2])))
	{
		vty_error_out (vty, "Cmd old-password is same with new-password.%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name(argv[0]);
	if(NULL == pinfo)
	{
		vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if((strlen(argv[1]) < USER_PASSWORD_MIN) || (strlen(argv[1]) > USER_PASSWORD_MAX))
	{
		/* admin用户的密码为初始化直接配置，不受长度限制，所以该处增加对admin默认密码的判断 */
		if ((0 != strcmp(ADMIN_USER_NAME, argv[0])) || (0 != strcmp(ADMIN_USER_PASSWORD, argv[1])))
		{
			vty_error_out (vty, "Cmd password is too short or too long, size <%d-%d>.%s", 
				USER_PASSWORD_MIN, USER_PASSWORD_MAX, VTY_NEWLINE);
			return CMD_WARNING;
		}
	}
	if((strlen(argv[2]) < USER_PASSWORD_MIN) || (strlen(argv[2]) > USER_PASSWORD_MAX))
	{
		vty_error_out (vty, "Cmd password is too short or too long, size <%d-%d>.%s", 
			USER_PASSWORD_MIN, USER_PASSWORD_MAX, VTY_NEWLINE);
		return CMD_WARNING;
	}
	

	if((strlen(pinfo->local_auth_user.password) == strlen(argv[1]))\
		&& (0 == strcmp(pinfo->local_auth_user.password, argv[1])))		//旧密码匹配
	{
		memset(pinfo->local_auth_user.password, 0, USER_PASSWORD_MAX);
		memset(pinfo->local_auth_user.password_encrypted, 0, USER_PASSWORD_MAX);
		strcpy(pinfo->local_auth_user.password, argv[2]);
		aaa_password_encode (pinfo->local_auth_user.password_encrypted, pinfo->local_auth_user.password);
		return CMD_SUCCESS;
	}
	else
	{
		vty_error_out (vty, "The old password is wrong!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
}


DEFUN(local_user_level_config_fun,
		local_user_level_config_cmd,
		"local-user WORD level <0-14>",
		"local user\n"
		"Input user name\n"
		"Level\n"
		"Level value\n")
{
	if (0 == strcmp(SUPER_USER_NAME, argv[0]))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (0 == strcmp(ADMIN_USER_NAME, argv[0]))
	{
		vty_error_out (vty, "Level of %s is forbidden to config!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name(argv[0]);
	if(NULL == pinfo)
	{
		vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	pinfo->local_auth_user.level = (int)atoi(argv[1]);
	return CMD_SUCCESS;
}


DEFUN(local_user_max_repeat_num_config_fun,
		local_user_max_repeat_num_config_cmd,
		"local-user WORD max-repeat-number <1-16>",
		"local user\n"
		"Input user name\n"
		"max repeat login number\n"
		"value\n")
{
	if (0 == strcmp(SUPER_USER_NAME, argv[0]))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name(argv[0]);
	if(NULL == pinfo)
	{
		vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	pinfo->local_auth_user.max_repeat_num = (int)atoi(argv[1]);
	return CMD_SUCCESS;
}


DEFUN(no_local_user_max_repeat_num_fun,
		no_local_user_max_repeat_num_cmd,
		"no local-user WORD max-repeat-number",
		"no\n"
		"local user\n"
		"Input user name\n"
		"max repeat login number\n")
{	
	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name (argv[0]);
	if(NULL == pinfo)
	{
		vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (0 == memcmp(argv[0], SUPER_USER_NAME, strlen (argv[0])))
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
	else if (0 == memcmp(argv[0], ADMIN_USER_NAME, strlen (argv[0])))
		pinfo->local_auth_user.max_repeat_num = ADMIN_USER_DEF_NUM;
	else
		pinfo->local_auth_user.max_repeat_num = AAA_DEF_LOCAL_MAX_REPEAT_NUM;
	
	return CMD_SUCCESS;
}


//add for h3c
DEFUN(local_user_service_config_fun,
		local_user_service_config_cmd,
		"local-user WORD service-type (ssh|telnet|terminal)",
		"local user\n"
		"Input user name\n"
		"Service type\n"
		"Ssh type\n"
		"Telnet type\n"
		"Terminal type\n")
{
	if (0 == strcmp(SUPER_USER_NAME, argv[0]))
	{
		vty_error_out (vty, "String %s is used by system and forbidden to use.%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if (0 == strcmp(ADMIN_USER_NAME, argv[0]))
	{
		vty_error_out (vty, "Level of %s is forbidden to config!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	USER_CONFIG_INFO *pinfo = aaa_user_config_get_by_name(argv[0]);
	if(NULL == pinfo)
	{
		vty_error_out (vty, "User %s does not exist!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	if(0 == memcmp(argv[1], "ssh", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_SSH;
	}
	else if(0 == memcmp(argv[1], "telnet", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_TELNET;
	}
	else if(0 == memcmp(argv[1], "terminal", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_TERMINAL;
	}
	
	return CMD_SUCCESS;
}



/************************* none cmd *************************/

DEFUN(none_user_level_config_fun,
		none_user_level_config_cmd,
		"none-user level <0-14>",
		"none user\n"
		"level\n"
		"0-14 level")
{
	aaa_none_user_def_priv_set((int)atoi(argv[0]));
	return CMD_SUCCESS;
}


DEFUN(no_none_user_level_config_fun,
		no_none_user_level_config_cmd,
		"no none-user level",
		"no\n"
		"none user\n"
		"level\n")
{
	aaa_none_user_def_priv_set(AAA_DEF_NONE_AUTH_PRIVILEGE);
	return CMD_SUCCESS;
}


/************************* dot1x cmd *************************/

DEFUN(dot1x_switch_set_fun,
		dot1x_switch_set_cmd,
		"dot1x (enable|disable)",
		"Ieee802.1x authenticate\n"
		"Enable\n"
		"Disable\n")
{
	if (0 == memcmp ("enable", argv[0], 3))
		aaa_dot1x_switch_set (ENABLE);
	else
		aaa_dot1x_switch_set (DISABLE);
	
	return CMD_SUCCESS;
}


DEFUN(dot1x_interface_switch_set_fun,
		dot1x_interface_switch_set_cmd,
		"dot1x (enable | disable)",
		"Ieee802.1x authenticate\n" 	
		"Enable\n"
		"Disable\n")
{
	uint32_t ifindex = (uint32_t)vty->index;

	if (0 == memcmp("enable", argv[0], 1))
		dot1x_port_cfg_add (dot1x_port_cfg_ls, ifindex, ENABLE, BASED_PORT);
	else
		dot1x_port_cfg_add (dot1x_port_cfg_ls, ifindex, DISABLE, BASED_PORT);

	return CMD_SUCCESS;
}


DEFUN(dot1x_interface_method_set_fun,
		dot1x_interface_method_set_cmd,
		"dot1x port-method (portbased|macbased)",
		"Ieee802.1x authenticate\n"
		"Port method\n"
		"Port based\n"
		"Mac based\n")
{
	uint32_t ifindex = (uint32_t)vty->index;

	DOT1X_PORT_CFG *port_cfg = dot1x_port_cfg_lookup (dot1x_port_cfg_ls, ifindex);
	if (NULL == port_cfg)
	{
		vty_error_out (vty, "EAP of port must be enable first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if (0 == memcmp("portbased", argv[0], 3))
		port_cfg->type_based = BASED_PORT;
	else
		port_cfg->type_based = BASED_MAC;
	
	return CMD_SUCCESS;
}


DEFUN(dot1x_authenticator_server_set_fun,
		dot1x_authenticator_server_set_cmd,
		"dot1x authenticator-server (radius|local)",
		"Ieee802.1x authenticate\n"
		"Authenticator server\n"
		"Radius method\n"
		"Local method\n")
{
	if (0 == memcmp("radius", argv[0], 3))
		aaa_dot1x_login_mode_set (DOT1X_RADIUS_MODE);
	else
		aaa_dot1x_login_mode_set (DOT1X_LOCAL_MODE);
	
	return CMD_SUCCESS;
}


DEFUN(dot1x_authenticator_method_set_fun,
		dot1x_authenticator_method_set_cmd,
		"dot1x authenticator-method (final|relay)",
		"Ieee802.1x authenticate\n"
		"Authenticator method\n"
		"Final method\n"
		"Relay method\n")
{
	if (0 == memcmp("final", argv[0], 3))
	{
		if (MODE_PAP == radius_authen_mode_get ())
		{
			vty_error_out (vty, "Radius authenticator mode must be set to be PAP first!%s", VTY_NEWLINE);
			return CMD_WARNING;
		}
		aaa_dot1x_auth_method_set (DOT1X_FINAL_METHOD);
	}
	else
		aaa_dot1x_auth_method_set (DOT1X_RELAY_METHOD);
	
	return CMD_SUCCESS;
}


DEFUN(dot1x_cfg_show_fun,
		dot1x_cfg_show_cmd,
		"show dot1x config",
		SHOW_STR
		"Ieee802.1x authenticate\n"
		"Config information\n")
{
	aaa_dot1x_show_config (vty);	
	return CMD_SUCCESS;
}


DEFUN(dot1x_online_info_show_fun,
		dot1x_online_info_show_cmd,
		"show dot1x online",
		SHOW_STR
		"Ieee802.1x authenticate\n"
		"Online information\n")
{
	aaa_dot1x_show_online_info (vty);	
	return CMD_SUCCESS;
}


/************************* debug cmd *************************/

DEFUN(debug_aaa_packet_enable_fun,
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
	   "enable\n")
{
	if(0 == syslog_debug)
	{
		vty_error_out (vty, "AAA debug switch is off, please turn on first!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	
	if(strncmp(argv[0], "receive", 7) == 0)
	{
		if(strncmp(argv[1], "radius", 6) == 0)
			aaa_pkt_dbg.radius_recv = 1;
		else if(strncmp(argv[1], "tacacs", 5) == 0)
			aaa_pkt_dbg.tac_plus_recv = 1;
		else if(strncmp(argv[1], "all", 3) == 0)
		{
			aaa_pkt_dbg.radius_recv = 1;
			aaa_pkt_dbg.tac_plus_recv = 1;
		}
	}
	else if(strncmp(argv[0], "send", 4) == 0)
	{
		if(strncmp(argv[1], "radius", 6) == 0)
			aaa_pkt_dbg.radius_send = 1;
		else if(strncmp(argv[1], "tacacs", 5) == 0)
			aaa_pkt_dbg.tac_plus_send = 1;
		else if(strncmp(argv[1], "all", 3) == 0)
		{
			aaa_pkt_dbg.radius_send = 1;
			aaa_pkt_dbg.tac_plus_send = 1;
		}
		else
		{
			return CMD_WARNING;
		}
	}
	vty_out(vty, "Enable debug %s %s packet!%s", argv[0], argv[1], VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(debug_aaa_packet_disable_fun,
       debug_aaa_packet_disable_cmd,
       "debug packet (receive|send) (radius|tacacs|all) disable",
	   "Debug control\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "radius protocol\n"
	   "tacacs protocol\n"
	   "all in aaa\n"
	   "disable\n")
{
	if(strncmp(argv[0], "receive", 7) == 0)
	{
		if (strncmp(argv[1], "radius", 6) == 0)
			aaa_pkt_dbg.radius_recv = 0;
		else if (strncmp(argv[1], "tacacs", 5) == 0)
			aaa_pkt_dbg.tac_plus_recv = 0;
		else if (strncmp(argv[1], "all", 3) == 0)
		{
			aaa_pkt_dbg.radius_recv = 0;
			aaa_pkt_dbg.tac_plus_recv = 0;
		}
		else
		{
			return CMD_WARNING;
		}
	}
	else if(strncmp(argv[0], "send", 4) == 0)
	{
		if(strncmp(argv[1], "radius", 6) == 0)
			aaa_pkt_dbg.radius_send = 0;
		else if(strncmp(argv[1], "tacacs", 5) == 0)
			aaa_pkt_dbg.tac_plus_send = 0;
		else if(strncmp(argv[1], "all", 3) == 0)
		{
			aaa_pkt_dbg.radius_send = 0;
			aaa_pkt_dbg.tac_plus_send = 0;
		}
	}
	
	vty_out(vty, "Disable debug %s %s packet!%s", argv[0], argv[1], VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(show_debug_aaa_packet_fun,
       show_debug_aaa_packet_cmd,
       "show debug",
       SHOW_STR
	   "Debug control\n")
{
	/*if(0 == syslog_debug)
	{
		aaa_pkt_dbg.radius_send = 0;
		aaa_pkt_dbg.radius_recv = 0;
		aaa_pkt_dbg.tac_plus_send = 0;
		aaa_pkt_dbg.tac_plus_recv = 0;
	}*/

	vty_out(vty, "AAA: %s", VTY_NEWLINE);
	
	if(aaa_pkt_dbg.radius_send)
		vty_out(vty, "  Radius send    packet state: enable %s", VTY_NEWLINE);
	if(aaa_pkt_dbg.radius_recv)
		vty_out(vty, "  Radius receive packet state: enable %s", VTY_NEWLINE);
	if(aaa_pkt_dbg.tac_plus_send)
		vty_out(vty, "  Tacacs send    packet state: enable %s", VTY_NEWLINE);
	if(aaa_pkt_dbg.tac_plus_recv)
		vty_out(vty, "  Tacacs receive packet state: enable %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}




/********************************************* 华三命令行 **********************************************/

DEFUN(h3c_add_local_user_fun,
	  h3c_add_local_user_cmd,
	  "local-user WORD [class manage]",
	  "Local user config\n"
	  "Input user name, <1-55>\n"
	  "Type of local user\n"
	  "Type of device manage user\n")
{
	if(argv[1])
	{
		if(0 == aaa_h3c_add_local_user(vty, (char *)argv[0], 1))	//配置了manage
		{
			return CMD_SUCCESS;
		}
	}
	else
	{
		if(0 == aaa_h3c_add_local_user(vty, (char *)argv[0], 0))	//未配置manage
		{
			return CMD_SUCCESS;
		}
	}

	return CMD_WARNING;
}


DEFUN(h3c_modify_local_user_password_fun,
	h3c_modify_local_user_password_cmd,
	"password simple WORD",
	"Local user password\n"
	"Type of simple\n"
	"Input user password, <1-63>\n")
{
	if(0 == aaa_h3c_modify_local_user_password(vty, (char *)argv[0], 3))
	{
		return CMD_SUCCESS;
	}
	return CMD_WARNING;
}


DEFUN(h3c_modify_local_user_password_fun1,
	h3c_modify_local_user_password_cmd1,
	"password (cipher|hash|simple) WORD",
	"Local user password\n"
	"Type of cipher\n"
	"Type of hash\n"
	"Type of simple\n"
	"Input user password, <1-63>\n")
{
	if(strncmp(argv[0], "cipher", 1) == 0)
	{
		if(0 == aaa_h3c_modify_local_user_password(vty, (char *)argv[1], 1))
		{
			return CMD_SUCCESS;
		}
	}
	else if(strncmp(argv[0], "hash", 1) == 0)
	{
		if(0 == aaa_h3c_modify_local_user_password(vty, (char *)argv[1], 2))
		{
			return CMD_SUCCESS;
		}
	}
	else if(strncmp(argv[0], "simple", 1) == 0)
	{
		if(0 == aaa_h3c_modify_local_user_password(vty, (char *)argv[1], 3))
		{
			return CMD_SUCCESS;
		}
	}

	return CMD_WARNING;
}


DEFUN(h3c_undo_local_user_password_fun,
	h3c_undo_local_user_password_cmd,
	"undo password",
	"Undo\n"
	"Local user password\n")
{
	aaa_h3c_del_local_user_password(vty);

	return CMD_SUCCESS;
}


DEFUN(h3c_modify_local_user_service_type_fun,
	h3c_modify_local_user_service_type_cmd,
	"service-type (ssh|telnet|terminal)",
	"Service type\n"
	"Ssh type\n"
	"Telnet type\n"
	"Terminal type\n")
{		
	aaa_h3c_modify_local_user_service_type(vty, (char *)argv[0]);

	return CMD_SUCCESS;
}


DEFUN(h3c_undo_local_user_service_type_fun,
	h3c_undo_local_user_service_type_cmd,
	"undo service-type (ssh|telnet|terminal)",
	"undo\n"
	"Service type\n"
	"Ssh type\n"
	"Telnet type\n"
	"Terminal type\n")
{
	aaa_h3c_undo_local_user_service_type(vty, (char *)argv[0]);

	return CMD_SUCCESS;
}

/********************************************** debug ***********************************************/

const struct message aaa_debug_name[] =
{
	{.key = AAA_DBG_COMMON,		.str = "common"},
	{.key = AAA_DBG_RADIUS,		.str = "radius"},
	{.key = AAA_DBG_TACACS,		.str = "tacacs"},
	{.key = AAA_DBG_LOCAL,		.str = "local"},
	{.key = AAA_DBG_NONE,		.str = "none"},
	{.key = AAA_DBG_DOT1X,		.str = "dot1x"},
	{.key = AAA_DBG_ALL,		.str = "all"}
};


DEFUN (aaa_debug_monitor,
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
	"Type name of all debug\n")
{
	unsigned int typeid = 0;
	int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(aaa_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], aaa_debug_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, aaa_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* 显示个debug信息的状态  */
DEFUN (show_aaa_debug_monitor,
	show_aaa_debug_monitor_cmd,
	"show aaa debug",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(aaa_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", aaa_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/



static struct cmd_node aaa_node =
{
  AAA_NODE,
  "%s(config-aaa)# ",
  1
};


static struct cmd_node aaa_local_user_node =
{
  AAA_LOCAL_USER_NODE,
  "%s(config-luser-manage)# ",
  1
};



static int aaa_config_write(struct vty *vty)
{
	vty_out (vty, "aaa%s", VTY_NEWLINE);
	aaa_write_common_config (vty);	
	aaa_write_radius_config (vty);
	aaa_write_tacacs_config (vty);
	aaa_write_local_config (vty);
	aaa_write_none_config (vty);
	aaa_write_dot1x_config (vty);
	return 0;
}

void aaa_cmd_init(void)
{
	install_node (&aaa_node, aaa_config_write);
	install_node (&aaa_local_user_node, NULL);
	install_default (AAA_NODE);
	install_default (AAA_LOCAL_USER_NODE);

	install_node (&physical_if_node, NULL);

	/* aaa enter cmd */
	install_element (CONFIG_NODE, &aaa_mode_enable_cmd, CMD_SYNC);

	/* common show cmd */
	install_element (CONFIG_NODE, &show_aaa_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &show_aaa_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &show_login_user_config_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &show_login_user_online_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &show_login_user_log_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &show_login_mode_config_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &aaa_enable_password_show_cmd, CMD_LOCAL);

	/* common config cmd */
	install_element (AAA_NODE, &aaa_login_max_num_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_aaa_login_max_num_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_user_idle_cut_time_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_aaa_login_user_idle_cut_time_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_user_default_level_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_aaa_login_user_default_level_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd0, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd1, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd2, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd3, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd4, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd5, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd6, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd7, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd8, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd9, CMD_SYNC);
	install_element (AAA_NODE, &aaa_login_mode_set_cmd10, CMD_SYNC);
	install_element (AAA_NODE, &no_aaa_login_mode_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_authorize_failed_handle_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_account_failed_handle_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_console_admin_mode_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &aaa_enable_password_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_aaa_enable_password_set_cmd, CMD_SYNC);

	/* radius cmd */
	install_element (AAA_NODE, &radius_auth_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_radius_auth_server_cmd, CMD_SYNC);
	install_element (AAA_NODE, &radius_acct_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_radius_acct_server_cmd, CMD_SYNC);
	install_element (AAA_NODE, &radius_key_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_radius_key_cmd, CMD_SYNC);
	install_element (AAA_NODE, &radius_account_switch_cmd, CMD_SYNC);
	install_element (AAA_NODE, &radius_account_interval_cfg_cmd, CMD_SYNC);
	install_element (AAA_NODE, &radius_authen_mode_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_radius_authen_mode_set_cmd, CMD_SYNC);

	/* tacacs cmd */
	install_element (AAA_NODE, &tac_plus_authen_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_tac_plus_authen_server_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_author_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_tac_plus_author_server_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_acct_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_tac_plus_acct_server_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_key_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_tac_plus_key_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_authorize_switch_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_account_switch_cmd, CMD_SYNC);
	install_element (AAA_NODE, &tac_plus_account_interval_cfg_cmd, CMD_SYNC);
	//add by lipf, 2019/4/25
	install_element (AAA_NODE, &tacacs_authen_mode_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_tacacs_authen_mode_set_cmd, CMD_SYNC);

	/* local cmd */
	install_element (AAA_NODE, &local_user_add_cmd, CMD_SYNC);
	install_element (AAA_NODE, &local_user_delete_cmd, CMD_SYNC);
	install_element (AAA_NODE, &local_user_change_password_cmd, CMD_SYNC);
	install_element (AAA_NODE, &local_user_max_repeat_num_config_cmd, CMD_SYNC);
	install_element (AAA_NODE, &local_user_level_config_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_local_user_max_repeat_num_cmd, CMD_SYNC);
	//add for h3c
	install_element (AAA_NODE, &local_user_service_config_cmd, CMD_SYNC);

	/* none cmd */
	install_element (AAA_NODE, &none_user_level_config_cmd, CMD_SYNC);
	install_element (AAA_NODE, &no_none_user_level_config_cmd, CMD_SYNC);

	/* dot1x cmd */
	install_element (AAA_NODE, &dot1x_switch_set_cmd, CMD_SYNC);
	install_element (PHYSICAL_IF_NODE, &dot1x_interface_switch_set_cmd, CMD_SYNC);
	//install_element (PHYSICAL_IF_NODE, &dot1x_interface_method_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &dot1x_authenticator_server_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &dot1x_authenticator_method_set_cmd, CMD_SYNC);
	install_element (AAA_NODE, &dot1x_cfg_show_cmd, CMD_LOCAL);
	install_element (AAA_NODE, &dot1x_online_info_show_cmd, CMD_LOCAL);
		
	/* debug cmd */
	install_element (AAA_NODE, &debug_aaa_packet_enable_cmd, CMD_SYNC);
	install_element (AAA_NODE, &debug_aaa_packet_disable_cmd, CMD_SYNC);
	install_element (AAA_NODE, &show_debug_aaa_packet_cmd, CMD_LOCAL);
	
	/* H3C cmds */
	install_element (CONFIG_NODE, &h3c_add_local_user_cmd, CMD_LOCAL);
	install_element (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_password_cmd, CMD_LOCAL);
	//install_element (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_password_cmd1, CMD_LOCAL);
	install_element (AAA_LOCAL_USER_NODE, &h3c_undo_local_user_password_cmd, CMD_LOCAL);
	install_element (AAA_LOCAL_USER_NODE, &h3c_modify_local_user_service_type_cmd, CMD_LOCAL);
	install_element (AAA_LOCAL_USER_NODE, &h3c_undo_local_user_service_type_cmd, CMD_LOCAL);


	/* new debug */
	install_element (CONFIG_NODE, &aaa_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_aaa_debug_monitor_cmd, CMD_SYNC);
}


