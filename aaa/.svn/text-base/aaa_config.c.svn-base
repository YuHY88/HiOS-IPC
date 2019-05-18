/******************************************************************************
 * Filename: aaa_config.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.11  lipf created
 *
******************************************************************************/

//#include <arpa/inet.h>
#include <string.h>

#include <lib/thread.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/aaa_common.h>

#include <lib/hptimer.h>

#include "aaa_config.h"
#include "aaa_radius.h"
#include "aaa_tac_plus.h"
#include "aaa_local_auth.h"
#include "aaa_none_auth.h"
#include "aaa_msg_handle.h"
#include "aaa_user_manager.h"
#include "aaa_dot1x.h"


/* 全局配置参数定义 */
AAA_CFG 	g_aaa_cfgs;		//aaa配置信息
ENABLE_INFO g_enable;		//enable配置信息

extern struct thread_master *aaa_master;

/* aaa config init */
void aaa_cfgs_init(void)
{
	memset(&g_aaa_cfgs, 0, sizeof(g_aaa_cfgs));

	aaa_set_auth_order (METHOD_LOCAL, METHOD_INVALID, METHOD_INVALID, METHOD_INVALID);
	aaa_console_admin_mode_set (DISABLE);
	aaa_online_author_failed_set (DISABLE);
	aaa_online_acct_failed_set (DISABLE);
	aaa_user_max_idle_time_set(AAA_DEF_IDLE_MAX * 60);
	aaa_login_max_num_set (AAA_ONLINE_NUM_MAX);
	aaa_user_def_priv_set (AAA_DEF_LOGIN_PRIVILEGE);
	aaa_alive_time_max_set (AAA_DEF_ALIVE_TIME_MAX);

	aaa_user_manager_init();
	aaa_radius_init();
	aaa_tac_plus_init();
	aaa_local_auth_init();
	aaa_none_auth_init();

	aaa_enable_init();		//enable口令初始化，赋密码初值

	aaa_dot1x_init();
	
	//thread_add_timer_msec(aaa_master, aaa_auth_process_check, NULL, 500);
	//thread_add_timer_msec(aaa_master, aaa_online_user_status_check, NULL, 1000);

	/* 添加定时器，单位ms */	
	high_pre_timer_add((char *)"AAA_Auth_Process_Timer", LIB_TIMER_TYPE_LOOP, aaa_auth_process_check, NULL, AAA_AUTH_PROCESS_CHECK_INTERVAL);
	high_pre_timer_add((char *)"AAA_Online_User_Status_Timer", LIB_TIMER_TYPE_LOOP, aaa_online_user_status_check, NULL, AAA_USER_ONLINE_CHECK_INTERVAL);
	high_pre_timer_add((char *)"AAA_Dot1x_Auth_Process_Timer", LIB_TIMER_TYPE_LOOP, aaa_dot1x_auth_process_check, NULL, DOT1X_DEF_AUTHING_CHECK_INTERVAL);
}


/************************ AAA公共配置项的set、get *************************/


/* 配置串口管理员模式 */
void aaa_console_admin_mode_set(int sw)
{
	g_aaa_cfgs.console_admin_mode = sw;
}

/* 获取串口管理员模式配置 */
int aaa_console_admin_mode_get(void)
{
	return g_aaa_cfgs.console_admin_mode;
}



/* 配置认证模式顺序 */
void aaa_set_auth_order(LOG_METHOD method_1, LOG_METHOD method_2,
					LOG_METHOD method_3, LOG_METHOD method_4)
{
	g_aaa_cfgs.auth_order = 0;
	g_aaa_cfgs.auth_order |= (method_1 << 12);
	g_aaa_cfgs.auth_order |= (method_2 << 8);
	g_aaa_cfgs.auth_order |= (method_3 << 4);
	g_aaa_cfgs.auth_order |= (method_4 << 0);
		
	/* 若配置tacacs，先验证授权/计费失败是否允许在线，看情况打开授权/计费功能 */
	if((METHOD_TACACS == method_1) || (METHOD_TACACS == method_2) ||
		(METHOD_TACACS == method_3) || (METHOD_TACACS == method_4))
	{
		if(DISABLE == g_aaa_cfgs.online_author_failed)
		{
			if(DISABLE == g_aaa_cfgs.tac_plus.author_switch)
				g_aaa_cfgs.tac_plus.author_switch = ENABLE;			
		}
		if(DISABLE == g_aaa_cfgs.online_acct_failed)
		{
			if(DISABLE == g_aaa_cfgs.tac_plus.acct_switch)
				g_aaa_cfgs.tac_plus.acct_switch = ENABLE;			
		}
	}

	/* 若配置radius，先验证计费失败是否允许在线，看情况打开计费功能 */
	if((METHOD_RADIUS == method_1) || (METHOD_RADIUS == method_2) ||
		(METHOD_RADIUS == method_3) || (METHOD_RADIUS == method_4))
	{
		if(DISABLE == g_aaa_cfgs.online_acct_failed)
		{
			if(DISABLE == g_aaa_cfgs.radius.acct_switch)
				g_aaa_cfgs.radius.acct_switch = ENABLE;			
		}
	}
}


/* 获取不同顺序的认证模式 */
LOG_METHOD aaa_get_auth_order(int order)
{
	return (((g_aaa_cfgs.auth_order & 0x0000ffff) >> (4 * (4 - order))) & 0x0f);
}


LOG_METHOD aaa_get_user_log_method(uint32_t user_id)
{
	uint32_t id = (user_id >> 24);

	switch(id)
	{
		case METHOD_RADIUS: 
			return METHOD_RADIUS;
		case METHOD_TACACS: 
			return METHOD_TACACS;
		case METHOD_LOCAL: 
			return METHOD_LOCAL;
		case METHOD_NONE: 
			return METHOD_NONE;
		default: 
			return METHOD_INVALID;
	}
}


void aaa_online_author_failed_set(int status)
{
	g_aaa_cfgs.online_author_failed = status;
}

int aaa_online_author_failed_get(void)
{
	return g_aaa_cfgs.online_author_failed;
}

void aaa_online_acct_failed_set(int status)
{
	g_aaa_cfgs.online_acct_failed = status;
}

int aaa_online_acct_failed_get(void)
{
	return g_aaa_cfgs.online_acct_failed;
}

void aaa_user_max_idle_time_set(time_t time)
{
	g_aaa_cfgs.idle_time_max = time;
}

time_t aaa_user_max_idle_time_get(void)
{
	return g_aaa_cfgs.idle_time_max;
}

void aaa_login_max_num_set(uint32_t num)
{
	g_aaa_cfgs.login_num_max = num;
}

uint32_t aaa_login_max_num_get(void)
{
	return g_aaa_cfgs.login_num_max;
}

void aaa_user_def_priv_set(int priv)
{
	g_aaa_cfgs.def_level = priv;
}

int aaa_user_def_priv_get(void)
{
	return g_aaa_cfgs.def_level;
}


void aaa_none_user_def_priv_set(int priv)
{
	g_aaa_cfgs.none.def_level = priv;
}

int aaa_none_user_def_priv_get(void)
{
	return g_aaa_cfgs.none.def_level;
}

/* 配置最大保活时间 */
void aaa_alive_time_max_set(int time)
{
	g_aaa_cfgs.alive_time_max = time;
}

int aaa_alive_time_max_get(void)
{
	return g_aaa_cfgs.alive_time_max;
}

/* enable口令初始化 */
void aaa_enable_init(void)
{
	memset(g_enable.username, 0, USER_NAME_MAX);
	strcpy(g_enable.username, ENABLE_NAME);
	
	for(int ret = 0; ret < 16; ret++)
	{
		memset(g_enable.password[ret], 0, USER_PASSWORD_MAX);
		strcpy(g_enable.password[ret], DEF_ENABLE_PASSWORD);
	}
}

/* enable口令密码设置 */
void aaa_enable_password_set(const char *password, int level)
{
	if((level >= 0) && (level <= 15))
	{
		memset(g_enable.password[level], 0, USER_PASSWORD_MAX);
		memcpy(g_enable.password[level], password, strlen(password));
	}
}

/* enable口令密码查询 */
char *aaa_enable_password_get(int level)
{
	return g_enable.password[level];
}


/* 检验enable口令密码是否匹配 */
/* 0	：匹配
 * 非0	：不匹配
 */
int aaa_enable_password_cmp(char *password, int level)
{
	if(strlen(password) == strlen(g_enable.password[level]))
		return memcmp(password, g_enable.password[level], strlen(password));
	else
		return -1;
}




/* 显示登录模式 */
void aaa_show_login_mode(struct vty *vty)
{
	char str[32];
	char mode[4][8];
	memset(str, 0, sizeof(str));

	for(int ret = 0; ret < 4; ret++)
	{
		memset(mode[ret], 0, sizeof(mode[ret]));

		if(METHOD_RADIUS == aaa_get_auth_order(ret+1))
			strcpy(mode[ret], "radius");
		else if(METHOD_TACACS == aaa_get_auth_order(ret+1))
			strcpy(mode[ret], "tacacs");
		else if(METHOD_LOCAL == aaa_get_auth_order(ret+1))
			strcpy(mode[ret], "local");
		else if(METHOD_NONE == aaa_get_auth_order(ret+1))
		{
			strcpy(mode[ret], "none");
			if(3 == ret)
				sprintf(str, "%s -> %s -> %s -> %s", mode[0], mode[1], mode[2], mode[3]);
		}
		else
		{
			if(1 == ret)
				sprintf(str, "%s", mode[0]);
			else if(2 == ret)
				sprintf(str, "%s -> %s", mode[0], mode[1]);
			else if(3 == ret)
				sprintf(str, "%s -> %s -> %s", mode[0], mode[1], mode[2]);			
			break;
		}
	}	
	vty_out(vty, "%-32s : %s%s", "login mode", str, VTY_NEWLINE);
}

/* 显示串口管理员模式 */
void aaa_show_console_admin_mode(struct vty *vty)
{
	vty_out(vty, "%-32s : %s%s", "console admin mode",
		aaa_console_admin_mode_get()?"enable":"disable", VTY_NEWLINE);
}


/* 显示授权失败策略 */
void aaa_show_author_failed_online(struct vty *vty)
{
	vty_out(vty, "%-32s : %s%s", "authorize failed online",
		aaa_online_author_failed_get()?"enable":"disable", VTY_NEWLINE);
}

/* 显示计费失败策略 */
void aaa_show_acct_failed_online(struct vty *vty)
{
	vty_out(vty, "%-32s : %s%s", "account failed online",
		aaa_online_acct_failed_get()?"enable":"disable", VTY_NEWLINE);
}

/* 显示用户最大允许空闲时间 */
void aaa_show_user_max_idle_time(struct vty *vty)
{
	vty_out(vty, "%-32s : %d minutes%s", "user max idle time",
		(int)aaa_user_max_idle_time_get()/60, VTY_NEWLINE);
}

/* 显示用户最大允许空闲登录数量 */
void aaa_show_user_max_login_num(struct vty *vty)
{
	vty_out(vty, "%-32s : %d%s", "user max login num",
		aaa_login_max_num_get(), VTY_NEWLINE);
}

/* 显示用户默认登录权限 */
void aaa_show_user_default_level(struct vty *vty)
{
	vty_out(vty, "%-32s : %d%s", "user default login level",
		aaa_user_def_priv_get(), VTY_NEWLINE);
}


/* 显示radius配置 */
void aaa_show_radius_config(struct vty *vty)
{
	struct sockaddr_in srv;
	char   ipv4_str[IPV4_STR_LEN_MAX] = {0};	
	uint32_t ipv4;
	
	/* show radius authentication server */
	memset(&srv, 0, sizeof(srv));
	radius_auth_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	if(inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
		vty_out(vty, "%-32s : %s %s", "radius authenticate server",
			ipv4_str, VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "radius authenticate server vpn",
		radius_auth_server_vpn_get(), VTY_NEWLINE);

	/* show radius account server */
	memset(&srv, 0, sizeof(srv));
	radius_acct_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	if((inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL))
		vty_out(vty, "%-32s : %s (%s) %s", "radius account server",\
			ipv4_str, radius_acct_server_switch_get()?"enable":"disable", VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "radius account server vpn",
		radius_acct_server_vpn_get(), VTY_NEWLINE);

	/* show radius shared-key */
	vty_out(vty, "%-32s : %s %s",  "radius authenticate mode", (MODE_PAP == radius_authen_mode_get ())?"pap":"chap", VTY_NEWLINE);
	vty_out(vty, "%-32s : %d minutes%s",  "radius account update interval", radius_acct_update_interval_get()/60, VTY_NEWLINE);
	vty_out(vty, "%-32s : %s %s",   "radius key", radius_key_get(), VTY_NEWLINE);	
	vty_out(vty, "%-32s : %d %s",   "radius retry times", radius_retry_times_get(), VTY_NEWLINE);
	vty_out(vty, "%-32s : %d ms%s", "radius retry interval", radius_retry_interval_get(), VTY_NEWLINE);
	vty_out(vty, "%-32s : %d ms%s", "radius response timeout", radius_response_timeout_get(), VTY_NEWLINE);
}

/* 显示tacacs配置 */
void aaa_show_tacacs_config(struct vty *vty)
{
	struct sockaddr_in srv;
	char   ipv4_str[IPV4_STR_LEN_MAX] = {0};	
	uint32_t ipv4;

	tac_plus_authen_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	if(inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
		vty_out(vty, "%-32s : %s %s", "tacacs authenticate server", ipv4_str, VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "tacacs authenticate server vpn",
		tac_plus_authen_server_vpn_get(), VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "tacacs authenticate mode",
		(tac_plus_authen_mode_get() == TAC_PLUS_AUTHEN_TYPE_CHAP)?"chap":"pap", VTY_NEWLINE);

	/* show tac_plus author server */
	tac_plus_author_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	if((inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL))
		vty_out(vty, "%-32s : %s (%s) %s", "tacacs authortize server",\
			ipv4_str, tac_plus_author_server_switch_get()?"enable":"disable", VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "tacacs authortize server vpn",
		tac_plus_author_server_vpn_get(), VTY_NEWLINE);

	/* show tac_plus authen server */
	tac_plus_acct_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	if((inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL))
		vty_out(vty, "%-32s : %s (%s) %s", "tacacs account server",\
			ipv4_str, tac_plus_acct_server_switch_get()?"enable":"disable", VTY_NEWLINE);

	vty_out(vty, "%-32s : %u %s", "tacacs account server vpn",
		tac_plus_acct_server_vpn_get(), VTY_NEWLINE);

	vty_out(vty, "%-32s : %d minutes%s", "tacacs account update interval",\
		(int)tac_plus_acct_update_interval_get()/60, VTY_NEWLINE);
	vty_out(vty, "%-32s : %s %s", "tacacs key",tac_plus_key_get(), VTY_NEWLINE);	
	vty_out(vty, "%-32s : %d ms%s", "tacacs response timeout",\
		(int)tac_plus_response_timeout_get(), VTY_NEWLINE);
}


/* show radius config infomation */
void aaa_show_config(struct vty *vty)
{
	vty_out (vty, "----------------------common configuration--------------------%s", VTY_NEWLINE);	
	aaa_show_login_mode (vty);
	aaa_show_console_admin_mode (vty);
	aaa_show_author_failed_online (vty);
	aaa_show_acct_failed_online (vty);
	aaa_show_user_max_idle_time (vty);
	aaa_show_user_max_login_num (vty);
	aaa_show_user_default_level (vty);
	aaa_show_none_user_config (vty);
	vty_out (vty, "%s----------------------radius configuration--------------------%s", VTY_NEWLINE, VTY_NEWLINE);
	aaa_show_radius_config (vty);
	vty_out (vty, "%s----------------------tacacs configuration--------------------%s", VTY_NEWLINE, VTY_NEWLINE);
	aaa_show_tacacs_config (vty);
	vty_out (vty, "%s--------------------------------------------------------------%s", VTY_NEWLINE, VTY_NEWLINE);
}


/* 显示enable密码 */
void aaa_show_enable_password(struct vty *vty, const char *level)
{
	char str[64];
	memset(str, 0, sizeof(str));
	sprintf(str, "%-6s %-32s", "level", "password");
	vty_out(vty, "----------------------enable password--------------------%s", VTY_NEWLINE);
	vty_out(vty, "%s%s", str, VTY_NEWLINE);
	
	if(NULL == level)
	{
		for(int ret = 0; ret < 16; ret++)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-6d %-32s", ret, aaa_enable_password_get(ret));
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
	}
	else
	{
		memset(str, 0, sizeof(str));
		sprintf(str, "%-6d %-32s", atoi(level), aaa_enable_password_get(atoi(level)));
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
	}
	vty_out(vty, "---------------------------------------------------------%s", VTY_NEWLINE);
}



/* common config的配置保存 */
void aaa_write_common_config(struct vty *vty)
{
	if (AAA_ONLINE_NUM_MAX != aaa_login_max_num_get ())
		vty_out(vty, " login-user max-number %d %s", aaa_login_max_num_get (), VTY_NEWLINE);

	if ((AAA_DEF_IDLE_MAX * 60) != aaa_user_max_idle_time_get ())
		vty_out(vty, " login-user idle-cut %d %s", (int)aaa_user_max_idle_time_get ()/60, VTY_NEWLINE);

	if (AAA_DEF_LOGIN_PRIVILEGE != aaa_user_def_priv_get ())
		vty_out(vty, " login-user default-level %d %s", aaa_user_def_priv_get (), VTY_NEWLINE);

	if (DISABLE != aaa_online_author_failed_get ())
		vty_out(vty, " authorize failed online %s %s", 
			(ENABLE == aaa_online_author_failed_get ())?"enable":"disable", VTY_NEWLINE);

	if (DISABLE != aaa_online_acct_failed_get ())
		vty_out(vty, " account failed online %s %s", 
			(ENABLE == aaa_online_acct_failed_get ())?"enable":"disable", VTY_NEWLINE);

	if (DISABLE != aaa_console_admin_mode_get())
		vty_out(vty, " console-admin-mode %s %s", 
			(ENABLE == aaa_console_admin_mode_get())?"enable":"disable", VTY_NEWLINE);

	char mode[4][8];
	LOG_METHOD method = METHOD_INVALID;
	int ret = 0;
	for(ret = 0; ret < 4; ret++)
	{
		memset(mode[ret], 0, sizeof(mode[ret]));
		method = aaa_get_auth_order(ret + 1);
		if(METHOD_RADIUS == method)
			strcpy(mode[ret], "radius");
		else if(METHOD_TACACS == method)
			strcpy(mode[ret], "tacacs");
		else if(METHOD_LOCAL == method)
			strcpy(mode[ret], "local");
		else if(METHOD_NONE == method)
		{
			strcpy(mode[ret], "none");
			break;
		}
		else if(METHOD_INVALID == method)
			break;
	}
	
	if(0 == ret)
		vty_out(vty, " login-mode %s %s", mode[0], VTY_NEWLINE);
	else if(1 == ret)
		vty_out(vty, " login-mode %s %s %s", mode[0], mode[1], VTY_NEWLINE);
	else if(2 == ret)
		vty_out(vty, " login-mode %s %s %s %s", mode[0], mode[1], mode[2], VTY_NEWLINE);
	else if(3 == ret)
		vty_out(vty, " login-mode %s %s %s %s %s", mode[0], mode[1], mode[2], mode[3], VTY_NEWLINE);
}

/* radius的配置保存 */
void aaa_write_radius_config(struct vty *vty)
{
	struct sockaddr_in srv;
	char   ipv4_str[IPV4_STR_LEN_MAX] = {0};
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;
	
	/* write radius auth server */
	memset(&srv, 0, sizeof(srv));
	radius_auth_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	vpn = radius_auth_server_vpn_get();
	
	if(srv.sin_addr.s_addr != RADIUS_DEF_SEVR_IPV4
		&& inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
	{
		if(vpn)
		{
			vty_out(vty, " radius authenticate server %s l3vpn %u %s", ipv4_str, vpn, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " radius authenticate server %s %s", ipv4_str, VTY_NEWLINE);
		}
	}

	/* write radius acct server */
	radius_acct_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	vpn = radius_acct_server_vpn_get();
	
	if(srv.sin_addr.s_addr != RADIUS_DEF_SEVR_IPV4
		&& inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
	{
		if(vpn)
		{
			vty_out(vty, " radius account server %s l3vpn %u %s", ipv4_str, vpn, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " radius account server %s %s", ipv4_str, VTY_NEWLINE);
		}
	}

	/* write radius auth server status */
	if (DISABLE != radius_acct_server_switch_get ())
		vty_out(vty, " radius account %s %s", radius_acct_server_switch_get ()?"enable":"disable", VTY_NEWLINE);

	/* write radius auth shared-key */
	if (MODE_PAP != radius_authen_mode_get ())
		vty_out(vty, " radius authenticate mode %s %s", (MODE_PAP == radius_authen_mode_get ())?"pap":"chap", VTY_NEWLINE);
	
	/* write radius auth shared-key */
	vty_out(vty, " radius key %s %s", radius_key_get (), VTY_NEWLINE);

	/* write radius acct interval */
	vty_out(vty, " radius account interval %d %s", radius_acct_update_interval_get ()/60, VTY_NEWLINE);
}

/* tacacs的配置保存 */
void aaa_write_tacacs_config(struct vty *vty)
{
	struct sockaddr_in srv;
	char   ipv4_str[IPV4_STR_LEN_MAX] = {0};
	char   *tac_key = NULL;
	uint32_t ipv4 = 0;
	uint16_t vpn = 0;

	memset(&srv, 0, sizeof(srv));
	tac_plus_authen_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	vpn = tac_plus_authen_server_vpn_get();
		
	if(srv.sin_addr.s_addr != TAC_PLUS_DEF_SERV_IPV4
		&& inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
	{
		if(vpn)
		{
			vty_out(vty, " tacacs authenticate server %s l3vpn %u %s", ipv4_str, vpn, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " tacacs authenticate server %s %s", ipv4_str, VTY_NEWLINE);
		}
	}

	vty_out(vty, " tacacs authenticate mode %s %s",
		(tac_plus_authen_mode_get() == TAC_PLUS_AUTHEN_TYPE_CHAP)?"chap":"pap", VTY_NEWLINE);

	memset(&srv, 0, sizeof(srv));
	tac_plus_author_server_get(&srv);
	ipv4 = srv.sin_addr.s_addr;
	vpn = tac_plus_author_server_vpn_get();
	
	if(srv.sin_addr.s_addr != TAC_PLUS_DEF_SERV_IPV4
		&& inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
	{
		if(vpn)
		{
			vty_out(vty, " tacacs authorize server %s l3vpn %u %s", ipv4_str, vpn, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " tacacs authorize server %s %s", ipv4_str, VTY_NEWLINE);
		}
	}

	vty_out(vty, " tacacs authorize %s %s", tac_plus_author_server_switch_get()?"enable":"disable", VTY_NEWLINE);

	memset(&srv, 0, sizeof(srv));
	tac_plus_acct_server_get(&srv);
	vpn = tac_plus_acct_server_vpn_get();
	
	ipv4 = srv.sin_addr.s_addr;
	if(srv.sin_addr.s_addr != TAC_PLUS_DEF_SERV_IPV4
		&& inet_ntop(AF_INET, &ipv4, ipv4_str, IPV4_STR_LEN_MAX) != NULL)
	{
		if(vpn)
		{
			vty_out(vty, " tacacs account server %s l3vpn %u %s", ipv4_str, vpn, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, " tacacs account server %s %s", ipv4_str, VTY_NEWLINE);
		}
	}

	vty_out(vty, " tacacs account %s %s", tac_plus_acct_server_switch_get()?"enable":"disable", VTY_NEWLINE);
	vty_out(vty, " tacacs account interval %d %s", (int)tac_plus_acct_update_interval_get()/60, VTY_NEWLINE);

	tac_key = tac_plus_key_get();
	if(tac_key != NULL && strcmp(tac_key, TAC_PLUS_DEF_SHARED_SECRET) != 0)
	{
		vty_out(vty, " tacacs key %s %s", tac_key, VTY_NEWLINE);
	}
}

/* local的配置保存 */
void aaa_write_local_config (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;
		
	for((node) = hios_hash_start(&g_user_table.user_config_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_config_table, &cursor))		
	{
		pinfo = (USER_CONFIG_INFO *)(node->data);
		if(pinfo == NULL)
			return ;

		if(0 == strcmp(pinfo->local_auth_user.username, SUPER_USER_NAME))
		{
			continue;
		}
		else if(0 == strcmp(pinfo->local_auth_user.username, ADMIN_USER_NAME))	//待修改
		{
			if(0 != strcmp(pinfo->local_auth_user.password, ADMIN_USER_PASSWORD))
			{
				vty_out(vty, " local-user %s password %s level %d %s", 
					ADMIN_USER_NAME,
					pinfo->local_auth_user.password_encrypted,
					pinfo->local_auth_user.level,
					VTY_NEWLINE);
			}

			if(ADMIN_USER_DEF_NUM != pinfo->local_auth_user.max_repeat_num)
			{
				vty_out(vty, " local-user %s max-repeat-number %d %s", 
					pinfo->local_auth_user.username,
					pinfo->local_auth_user.max_repeat_num, VTY_NEWLINE);
			}

			if(pinfo->local_auth_user.service & SERVICE_TYPE_SSH)
			{
				vty_out(vty, " local-user %s service-type ssh %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
			if(pinfo->local_auth_user.service & SERVICE_TYPE_TELNET)
			{
				vty_out(vty, " local-user %s service-type telnet %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
			if(pinfo->local_auth_user.service & SERVICE_TYPE_TERMINAL)
			{
				vty_out(vty, " local-user %s service-type terminal %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
		}
		else
		{
			vty_out(vty, " local-user %s password %s level %d %s", 
				pinfo->local_auth_user.username,
				pinfo->local_auth_user.password_encrypted,
				pinfo->local_auth_user.level,
				VTY_NEWLINE);

			if(AAA_DEF_LOCAL_MAX_REPEAT_NUM != pinfo->local_auth_user.max_repeat_num)
			{
				vty_out(vty, " local-user %s max-repeat-number %d %s", 
					pinfo->local_auth_user.username,
					pinfo->local_auth_user.max_repeat_num, VTY_NEWLINE);
			}

			if(pinfo->local_auth_user.service & SERVICE_TYPE_SSH)
			{
				vty_out(vty, " local-user %s service-type ssh %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
			if(pinfo->local_auth_user.service & SERVICE_TYPE_TELNET)
			{
				vty_out(vty, " local-user %s service-type telnet %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
			if(pinfo->local_auth_user.service & SERVICE_TYPE_TERMINAL)
			{
				vty_out(vty, " local-user %s service-type terminal %s", 
					pinfo->local_auth_user.username, VTY_NEWLINE);
			}
		}
	}
}


/* none的配置保存 */
void aaa_write_none_config (struct vty *vty)
{
	if (AAA_DEF_NONE_AUTH_PRIVILEGE != aaa_none_user_def_priv_get ())
		vty_out(vty, " none-user level %d %s", aaa_none_user_def_priv_get (), VTY_NEWLINE);
}



/* 密码加密算法 */
void aaa_password_encode (char *dpw, char *spw)
{
	for (uint32_t i = 0; i < strlen (spw); i++)
		*(dpw + i) = *(spw +i) - 10;
}

/* 密码解密算法 */
void aaa_password_decode (char *dpw, char *spw)
{
	for (uint32_t i = 0; i < strlen (spw); i++)
		*(dpw + i) = *(spw +i) + 10;
}




