/******************************************************************************
 * Filename: aaa_none_auth.h
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

#ifndef _AAA_NONE_AUTH_H_
#define _AAA_NONE_AUTH_H_

#include "lib/vty.h"


typedef struct __none_auth_cfg
{
	time_t	idle_time_max;		//用户最大空闲时间
	int		login_num_max;		//最大登录用户数量
	int		def_level;			//用户登录后的默认权限
	
}NONE_AUTH_CFG;


typedef struct _none_auth_session_info
{	
	
	
}NONE_AUTH_SESSION_INFO;

void aaa_none_auth_init (void);
void aaa_show_none_user_config (struct vty *vty);


#endif /* _AAA_NONE_AUTH_H_ */

