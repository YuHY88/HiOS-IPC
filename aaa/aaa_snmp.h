/******************************************************************************
 * Filename: aaa_snmp.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.5.22  lipf created
 *
******************************************************************************/

#ifndef _AAA_SNMP_H_
#define _AAA_SNMP_H_


#include "lib/msg_ipc_n.h"
#include "lib/pkt_buffer.h"
#include "lib/types.h"
#include "lib/aaa_common.h"


#include "radiusclient.h"
#include "aaa_config.h"


/* aaa subtype */
enum AAA_CFG_SUBTYPE
{
	/********** scaler subtype ***********/
	SNMP_AAA_COMMON_CFG,
		
	SNMP_AAA_RADIUS_CFG,

	SNMP_AAA_TAC_CFG,

	/* enable口令密码 */
	SNMP_AAA_ENABLE_CFG,

	/********** table subtype ***********/
	SNMP_USER_CONFIG_TABLE,
	SNMP_USER_ONLINE_TABLE,
	SNMP_USER_LOG_TABLE,
};

struct aaa_snmp_radius_cfg
{
	int auth_server_ip;
	int auth_server_l3vpn;
	int acct_server_ip;
	int acct_server_l3vpn;
	int acct_switch;
	int acct_update_interval;
	char key[RADIUS_SHARED_SECRET_LEN+1];
	int retry_times;
	int retry_interval;
	int rsp_timeout;
	int auth_mode;
};


struct aaa_snmp_tacacs_cfg
{
	int authen_server_ip;
	int authen_server_l3vpn;
	int author_server_ip;
	int author_server_l3vpn;
	int author_switch;
	int acct_server_ip;
	int acct_server_l3vpn;
	int acct_switch;
	int acct_update_interval;
	char key[TAC_PLUS_SHARED_SECRET_LEN+1];
	int rsp_timeout;
};


struct aaa_snmp_common_cfg
{
	int login_user_idle_cut;
	int login_user_max_num;
	int login_user_def_level;
	int author_failed_online;
	int acct_failed_online;
	int console_admin_mode;
	int login_mode_first_pri;
	int login_mode_second_pri;
	int login_mode_third_pri;
	int login_mode_forth_pri;
	int none_user_level;
};


/**************************** 已配置用户数据结构定义 **********************************/

typedef struct aaa_snmp_user_config_index
{
	uint32_t user_id;
	
}SNMP_USER_CONFIG_INDEX;

typedef struct aaa_snmp_user_config_data
{
	char		username[USER_NAME_MAX + 1];
	char		password[USER_PASSWORD_MAX + 1];
	int			level;
	int			max_repeat_num;
	
}SNMP_USER_CONFIG_DATA;


typedef struct aaa_snmp_user_config_info
{
	SNMP_USER_CONFIG_INDEX 	index;	
	SNMP_USER_CONFIG_DATA 	data;
	
}SNMP_USER_CONFIG_INFO;

typedef struct aaa_snmp_user_config_list
{
	SNMP_USER_CONFIG_INFO 				data;
	struct aaa_snmp_user_config_list 	*next;
	
}SNMP_USER_CONFIG_LIST;


/**************************** 在线用户数据结构定义 **********************************/

typedef struct aaa_snmp_user_online_index
{
	uint32_t user_id;
	
}SNMP_USER_ONLINE_INDEX;

typedef struct aaa_snmp_user_online_data
{
	char		username[USER_NAME_MAX + 1];
	int			level_author;
	int			level_current;
	LOG_METHOD	login_method;
	char		login_from[LOGIN_FROM_LEN_MAX + 1];
	time_t 		time_login;
	time_t 		time_online;
	
}SNMP_USER_ONLINE_DATA;


typedef struct aaa_snmp_user_online_info
{
	SNMP_USER_ONLINE_INDEX 	index;	
	SNMP_USER_ONLINE_DATA 	data;
	
}SNMP_USER_ONLINE_INFO;

typedef struct aaa_snmp_user_online_list
{
	SNMP_USER_ONLINE_INFO 				data;
	struct aaa_snmp_user_online_list 	*next;
	
}SNMP_USER_ONLINE_LIST;


/**************************** 用户log数据结构定义 **********************************/

typedef struct aaa_snmp_user_log_index
{
	uint32_t user_id;
	
}SNMP_USER_LOG_INDEX;

typedef struct aaa_snmp_user_log_data
{
	char			username[USER_NAME_MAX + 1];
	int				level;
	LOG_METHOD		login_method;
	LOGIN_RESULT	login_result;
	char			login_from[LOGIN_FROM_LEN_MAX + 1];
	time_t			time_start;			//time of start login
	time_t			time_exit;			//time of exit login, for record log
	
}SNMP_USER_LOG_DATA;


typedef struct aaa_snmp_user_log_info
{
	SNMP_USER_LOG_INDEX 	index;	
	SNMP_USER_LOG_DATA 		data;
	
}SNMP_USER_LOG_INFO;

typedef struct aaa_snmp_user_log_list
{
	SNMP_USER_LOG_INFO 				data;
	struct aaa_snmp_user_log_list 	*next;
	
}SNMP_USER_LOG_LIST;


/***********************************************************************************/



void aaa_handle_snmp_msg (struct ipc_mesg_n *pmsg);
int aaa_snmp_config_info_get (struct ipc_msghdr_n *phdr);
void aaa_snmp_table_info_get (struct ipc_mesg_n *pmsg);
int aaa_snmp_user_config_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_CONFIG_INDEX *index);
int aaa_snmp_user_online_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_ONLINE_INDEX *index);
int aaa_snmp_user_log_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_LOG_INDEX *index);


#endif  /* _AAA_SNMP_H_ */

