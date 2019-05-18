/******************************************************************************
 * Filename: aaa_local_auth.h
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

#ifndef _AAA_LOCAL_AUTH_H_
#define _AAA_LOCAL_AUTH_H_

#include <lib/aaa_common.h>



typedef struct __local_auth_cfg
{	
	time_t	idle_time_max;		//用户最大空闲时间	
	int		login_num_max;		//最大登录用户数量
	int		def_level;			//用户登录后的默认权限
	
}LOCAL_AUTH_CFG;



typedef struct _local_user_cfg
{
	char	username[USER_NAME_MAX + 1];
	char	password[USER_PASSWORD_MAX + 1];
	char	password_encrypted[USER_PASSWORD_MAX + 1];
	int		encrypted;
	int		level;
	int		max_repeat_num;

	/* add for H3C, 2018/5/29 */
	int		service;
	
}LOCAL_AUTH_USER_CFG;


typedef struct _local_auth_session_info
{	
	
	
}LOCAL_AUTH_SESSION_INFO;


void aaa_local_auth_init(void);


#endif /* _AAA_LOCAL_AUTH_H_ */

