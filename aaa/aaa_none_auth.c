/******************************************************************************
 * Filename: aaa_none_auth.c
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

#include "aaa_none_auth.h"
#include "aaa_config.h"



void aaa_none_auth_init(void)
{
	aaa_none_user_def_priv_set (AAA_DEF_NONE_AUTH_PRIVILEGE);
}


/* 显示none用户的配置 */
void aaa_show_none_user_config (struct vty *vty)
{	
	vty_out(vty, "%-32s : %d%s", "none user level", aaa_none_user_def_priv_get (),
		VTY_NEWLINE);
}


