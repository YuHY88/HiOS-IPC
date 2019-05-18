/******************************************************************************
 * Filename: aaa_config.h
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

#ifndef _AAA_CONFIG_H_
#define _AAA_CONFIG_H_

//#include <netinet/in.h>
#include <lib/aaa_common.h>
#include <lib/hash1.h>
#include <lib/errcode.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/inet_ip.h>

#include "aaa_radius.h"
#include "aaa_tac_plus.h"
#include "aaa_local_auth.h"
#include "aaa_none_auth.h"
#include "aaa_dot1x.h"



/*	认证顺序定义：auth_order, 32bits
 *	低16位存储认证顺序，高16位保留
 *	
 *  15              12   11           8   7            4  7              0
 * +-------------------+----------------+----------------+----------------+
 * |	第一认证方式		   | 第二认证方式			|	第三认证方式		 |	第四认证方式		  |
 * +-------------------+----------------+----------------+----------------+
 */

typedef struct _aaa_cfg
{	
	uint32_t		auth_order;				//认证顺序
	int				console_admin_mode;		//串口管理员模式开关			
	int				online_author_failed;	//授权失败是否在线
	int				online_acct_failed;		//计费失败是否在线

	time_t			idle_time_max;		//用户最大空闲时间
	int				login_num_max;		//最大登录用户数量
	int				def_level;			//用户登录后的默认权限
	int				alive_time_max;		//最大保活时间
	
	RADIUS_CFG    	radius;
	TACACS_CFG  	tac_plus;
	LOCAL_AUTH_CFG	local;
	NONE_AUTH_CFG	none;
	DOT1X_CFG		dot1x;
	
}AAA_CFG;



extern AAA_CFG 	g_aaa_cfgs;
extern ENABLE_INFO g_enable;

void aaa_cfgs_init (void);

void aaa_enable_init (void);
void aaa_enable_password_set (const char *password, int level);
char *aaa_enable_password_get (int level);
int aaa_enable_password_cmp (char *password, int level);

void aaa_console_admin_mode_set (int sw);
int aaa_console_admin_mode_get (void);

void aaa_set_auth_order (LOG_METHOD method_1, LOG_METHOD method_2,
					LOG_METHOD method_3, LOG_METHOD method_4);
LOG_METHOD aaa_get_auth_order (int order);
LOG_METHOD aaa_get_user_log_method (uint32_t user_id);

void aaa_online_author_failed_set (int status);
int aaa_online_author_failed_get (void);
void aaa_online_acct_failed_set (int status);
int aaa_online_acct_failed_get (void);
void aaa_user_max_idle_time_set (time_t time);
time_t aaa_user_max_idle_time_get (void);
void aaa_login_max_num_set (uint32_t num);
uint32_t aaa_login_max_num_get (void);
void aaa_user_def_priv_set (int priv);
int aaa_user_def_priv_get (void);
void aaa_none_user_def_priv_set (int priv);
int aaa_none_user_def_priv_get (void);
void aaa_alive_time_max_set (int time);
int aaa_alive_time_max_get (void);


void aaa_show_login_mode (struct vty *vty);
void aaa_show_console_admin_mode (struct vty *vty);
void aaa_show_author_failed_online (struct vty *vty);
void aaa_show_acct_failed_online (struct vty *vty);
void aaa_show_user_max_idle_time (struct vty *vty);
void aaa_show_user_max_login_num (struct vty *vty);
void aaa_show_user_default_level (struct vty *vty);
void aaa_show_radius_config (struct vty *vty);
void aaa_show_tacacs_config (struct vty *vty);
void aaa_show_config (struct vty *vty);
void aaa_show_enable_password (struct vty *vty, const char *level);


void aaa_write_common_config (struct vty *vty);
void aaa_write_radius_config (struct vty *vty);
void aaa_write_tacacs_config (struct vty *vty);
void aaa_write_local_config (struct vty *vty);
void aaa_write_none_config (struct vty *vty);

void aaa_password_encode (char *dpw, char *spw);
void aaa_password_decode (char *dpw, char *spw);


#endif /* _AAA_CONFIG_H_ */

