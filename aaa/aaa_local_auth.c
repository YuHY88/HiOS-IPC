/******************************************************************************
 * Filename: aaa_local_auth.c
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

#include <string.h>

#include <lib/command.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/aaa_common.h>

#include "aaa_local_auth.h"
#include "aaa_config.h"
#include "aaa_user_manager.h"


void aaa_local_auth_init(void)
{
	/* 隐藏超级用户，不可修改 */
	USER_CONFIG_INFO *pinfo_1 = (USER_CONFIG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_CONFIG_INFO));
	if(NULL == pinfo_1)
		zlog_err("%s[%d] : malloc error!\n", __FILE__, __LINE__);
	else
	{
		memset(pinfo_1, 0, sizeof(USER_CONFIG_INFO));
		strcpy(pinfo_1->local_auth_user.username, SUPER_USER_NAME);
		strcpy(pinfo_1->local_auth_user.password, SUPER_USER_PASSWORD);
		pinfo_1->local_auth_user.level = 15;
		pinfo_1->local_auth_user.max_repeat_num = 1;
		pinfo_1->local_auth_user.service = (SERVICE_TYPE_SSH|SERVICE_TYPE_TELNET|SERVICE_TYPE_TERMINAL);
		aaa_user_config_add(pinfo_1);
	}

	/* 默认管理员用户，可修改 */
	USER_CONFIG_INFO *pinfo_2 = (USER_CONFIG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_CONFIG_INFO));
	if(NULL == pinfo_2)
		zlog_err("%s[%d] : malloc error!\n", __FILE__, __LINE__);
	else
	{
		memset(pinfo_2, 0, sizeof(USER_CONFIG_INFO));
		strcpy(pinfo_2->local_auth_user.username, ADMIN_USER_NAME);
		strcpy(pinfo_2->local_auth_user.password, ADMIN_USER_PASSWORD);
		pinfo_2->local_auth_user.level = ADMIN_USER_LEVEL;
		pinfo_2->local_auth_user.max_repeat_num = ADMIN_USER_DEF_NUM;
		pinfo_2->local_auth_user.service = (SERVICE_TYPE_SSH|SERVICE_TYPE_TELNET|SERVICE_TYPE_TERMINAL);
		aaa_user_config_add(pinfo_2);
	}	
}




