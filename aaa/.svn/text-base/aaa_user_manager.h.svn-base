/******************************************************************************
 * Filename: aaa_user_manager.h
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

#ifndef _AAA_USER_MANAGER_H_
#define _AAA_USER_MANAGER_H_


#include "aaa_radius_common.h"
#include "aaa_tac_plus_common.h"
#include "aaa_local_auth.h"


typedef struct
{
	RADIUS_SESSION_INFO			radius_user;
	TACACS_SESSION_INFO			tacacs_user;
	//LOCAL_AUTH_SESSION_INFO		local_auth_user;
	//NONE_AUTH_SESSION_INFO		none_auth_user;

}SESSION_INFO;


typedef struct
{
	char			username[USER_NAME_MAX + 1];
	char			password_init[USER_PASSWORD_MAX + 1];	//add by lipf, 2019/4/25
	char			password[USER_PASSWORD_MAX + 1];		//password for tacacs
	char			ttyname[LOGIN_FROM_LEN_MAX + 1];
	uint32_t		ip;
	uint16_t		port;
	int 			level_author;
	int				level_current;

	time_t			time_start;			//time of start login
	time_t			time_exit;			//time of exit login, for record log
	
}USER_INFO;

	
typedef struct
{
	uint32_t		user_id;
	uint32_t		user_id_buf;
	int				auth_order;
	int				auth_finished;
	AAA_RESULT		aaa_result;
	USER_INFO 		user_info;
	SESSION_INFO	session_info;
	
}USER_AUTHING_INFO;


typedef struct
{
	uint32_t		user_id;
	int				alive_time;
	USER_INFO		user_info;
	LOG_METHOD		log_method;
	SESSION_INFO	session_info;
	
}USER_ONLINE_INFO;


typedef struct
{
	uint32_t		user_id;
	LOG_METHOD		log_method;
	LOGIN_RESULT	log_result;
	USER_INFO		user_info;	
	
}USER_LOG_INFO;


typedef struct
{
	uint32_t				user_id;
	LOCAL_AUTH_USER_CFG		local_auth_user;
	
}USER_CONFIG_INFO;



typedef struct _user_table
{
	struct hash_table user_config_table;	//在线的用户表
	struct hash_table user_authing_table;	//正在登录的用户表
	struct hash_table user_online_table;	//登录的用户表
	struct hash_table user_log_table;		//登录的用户表
	
}USER_TABLE;



extern USER_TABLE g_user_table;


void aaa_user_manager_init(void);

unsigned int aaa_compute_hash(void *hash_key);
int aaa_compare_hash(void *item, void *hash_key);


USER_LOG_INFO 		*aaa_copy_authing_to_log(USER_AUTHING_INFO *pinfo, LOGIN_RESULT result);
USER_LOG_INFO 		*aaa_copy_online_to_log(USER_ONLINE_INFO *pinfo);
USER_ONLINE_INFO 	*aaa_copy_authing_to_online(USER_AUTHING_INFO *pinfo);

int aaa_user_config_add(USER_CONFIG_INFO *pinfo);
USER_CONFIG_INFO *aaa_user_config_get_by_id(uint32_t user_id);
USER_CONFIG_INFO *aaa_user_config_get_by_name(const char *name);
int aaa_user_config_del(uint32_t user_id);

int aaa_user_authing_add(USER_AUTHING_INFO *pinfo);
USER_AUTHING_INFO *aaa_user_authing_get_by_id(uint32_t user_id);
USER_AUTHING_INFO *aaa_user_authing_get_by_name(const char *name);
int aaa_user_authing_del(uint32_t user_id);
int aaa_user_authing_del_all(void);

int aaa_user_online_add(USER_ONLINE_INFO *pinfo);
USER_ONLINE_INFO *aaa_user_online_get_by_id(uint32_t user_id);
USER_ONLINE_INFO *aaa_user_online_get_by_name(const char *name);
int aaa_user_online_del(uint32_t user_id);
int aaa_user_online_del_all(void);

int aaa_user_log_add(USER_LOG_INFO *pinfo);
USER_LOG_INFO *aaa_user_log_get_by_id(uint32_t user_id);
USER_LOG_INFO *aaa_user_log_get_by_name(const char *name);
int aaa_user_log_del(uint32_t user_id);
int aaa_user_log_del_all(void);

uint32_t aaa_alloc_user_config_id(LOG_METHOD method);
int aaa_free_user_config_id(uint32_t user_id);
int aaa_get_user_config_num(void);

uint32_t aaa_alloc_user_authing_id(void);
int aaa_free_user_authing_id(uint32_t user_id);
int aaa_get_user_authing_num(void);

uint32_t aaa_alloc_user_online_id(LOG_METHOD method);
int aaa_free_user_online_id(uint32_t user_id);
int aaa_get_user_online_num(void);

uint32_t aaa_alloc_user_log_id(void);
int aaa_get_user_log_num(void);

int aaa_record_start_login_time(time_t *t);
int aaa_record_exit_login_time(time_t *t);
int aaa_get_user_online_time(time_t t, char *str);
int aaa_get_user_log_online_time(time_t time_start, time_t time_exit, char *str);
int aaa_get_local_time_str(time_t t, char *str);

void aaa_show_user_config(struct vty *vty);
void aaa_show_user_online(struct vty *vty, const char *str_mode);
void aaa_show_user_log(struct vty *vty, const char *str_mode);

void aaa_show_user_online_all (struct vty *vty);
void aaa_show_user_online_radius (struct vty *vty);
void aaa_show_user_online_tacacs (struct vty *vty);
void aaa_show_user_online_local (struct vty *vty);
void aaa_show_user_online_none (struct vty *vty);

void aaa_show_user_log_all (struct vty *vty);
void aaa_show_user_log_radius (struct vty *vty);
void aaa_show_user_log_tacacs (struct vty *vty);
void aaa_show_user_log_local (struct vty *vty);
void aaa_show_user_log_none (struct vty *vty);


/*****************************************  华三命令行相关函数 ***************************************/

int aaa_h3c_username_is_valid(char *username);
int aaa_h3c_add_local_user(struct vty *vty, char *username, int manage_flag);
int aaa_h3c_modify_local_user_password(struct vty *vty, char *password, int flag);
int aaa_h3c_del_local_user_password(struct vty *vty);
int aaa_h3c_modify_local_user_service_type(struct vty *vty, char *type);
int aaa_h3c_undo_local_user_service_type(struct vty *vty, char *type);
int aaa_h3c_get_local_user_service_type(USER_CONFIG_INFO *pinfo, char *strService);


#endif /* _AAA_USER_MANAGER_H_ */

