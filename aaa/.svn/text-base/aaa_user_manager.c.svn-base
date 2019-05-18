/******************************************************************************
 * Filename: aaa_user_manager.c
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
#include <lib/aaa_common.h>
#include <lib/hash1.h>
#include <lib/memory.h>
#include <lib/memtypes.h>

#include "aaa_user_manager.h"
#include "aaa_config.h"
#include "aaa_dot1x.h"



USER_TABLE g_user_table;



/* hash key 产生方法 */
unsigned int aaa_compute_hash(void *hash_key)
{
	if(NULL == hash_key)
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] -> %s : hash_key is NULL!\n",\
			__FILE__, __LINE__, __func__);
		return 0;
	}

    return (unsigned int)hash_key;
}


/* hash key 比较方法 */
int aaa_compare_hash(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = (struct hash_bucket *)item;

	if(NULL == pbucket || NULL == hash_key)
	{
		return AAA_ERROR;
	}

	if (pbucket->hash_key == hash_key)
    {
        return AAA_OK;
    }
    return AAA_ERROR;
}


/* 初始化两个用户表 */
void aaa_user_manager_init(void)
{
	hios_hash_init(&g_user_table.user_config_table, AAA_CONFIG_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
	hios_hash_init(&g_user_table.user_authing_table, AAA_ONLINE_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
	hios_hash_init(&g_user_table.user_online_table, AAA_ONLINE_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
	hios_hash_init(&g_user_table.user_log_table, AAA_ONLINE_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);

	/* dot1x 哈希表初始化 */
	hios_hash_init(&g_dot1x_table.dot1x_authing_table, DOT1X_AUTHING_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
	hios_hash_init(&g_dot1x_table.dot1x_online_table, DOT1X_ONLINE_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
	hios_hash_init(&g_dot1x_table.dot1x_log_table, DOT1X_LOG_NUM_MAX, aaa_compute_hash,
        aaa_compare_hash);
}


/* 在用户配置的hash表中添加会话 
 * 现在只有本地用户存储有用户配置
 */
int aaa_user_config_add(USER_CONFIG_INFO *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	pinfo->user_id = aaa_alloc_user_config_id(METHOD_LOCAL);
	
	if(0 == pinfo->user_id)
		return AAA_OVERSIZE;

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}	
	
	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->user_id;
	pbucket->data = pinfo;
	if(hios_hash_add(&g_user_table.user_config_table, pbucket) != 0)
	{
		XFREE(MTYPE_AAA, pinfo);
		XFREE(MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_COMMON, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_user_table.user_config_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

USER_CONFIG_INFO *aaa_user_config_get_by_id(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_config_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (USER_CONFIG_INFO *)cursor->data;
	if(user_id == pinfo->user_id)
		return pinfo;
	else
		return NULL;
}

USER_CONFIG_INFO *aaa_user_config_get_by_name(const char *name)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_config_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_config_table, &cursor))		
	{
		pinfo = (USER_CONFIG_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		if((strlen(name) == strlen(pinfo->local_auth_user.username)) &&
			(0 == strcmp(pinfo->local_auth_user.username, name)))
		{
			return pinfo;
		}
	}
	return NULL;
}


int aaa_user_config_del(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_config_table, (void *)user_id);
	if(NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (USER_CONFIG_INFO *)cursor->data;
		
	XFREE(MTYPE_AAA, pinfo);
	hios_hash_delete(&g_user_table.user_config_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);

	aaa_free_user_config_id(user_id);
	return AAA_ERROR;
}


/* 在正在登录用户的hash表中添加会话 */
int aaa_user_authing_add(USER_AUTHING_INFO *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	pinfo->user_id = aaa_alloc_user_authing_id();
	if(0 == pinfo->user_id)
		return AAA_OVERSIZE;

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}

	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->user_id;
	pbucket->data = pinfo;
	if(hios_hash_add(&g_user_table.user_authing_table, pbucket) != 0)
	{
		XFREE(MTYPE_AAA, pinfo);
		XFREE(MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_COMMON, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_user_table.user_authing_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

USER_AUTHING_INFO *aaa_user_authing_get_by_id(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_authing_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (USER_AUTHING_INFO *)cursor->data;
	if(user_id == pinfo->user_id)
		return pinfo;
	else
		return NULL;
}

USER_AUTHING_INFO *aaa_user_authing_get_by_name(const char *name)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		pinfo = (USER_AUTHING_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		if((strlen(name) == strlen(pinfo->user_info.username)) &&
			(0 == strncmp(pinfo->user_info.username, name, strlen(name))))
		{
			return pinfo;
		}
	}
	return NULL;
}


int aaa_user_authing_del(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_authing_table, (void *)user_id);
	if(NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (USER_AUTHING_INFO *)cursor->data;

	/*radius_attr_list_destory(pinfo->session_info.radius_user.auth_pkt.attribute_list);
    radius_attr_list_destory(pinfo->session_info.radius_user.acct_pkt.attribute_list);
	XFREE(MTYPE_AAA, pinfo->session_info.tacacs_user.authen_data);
	XFREE(MTYPE_AAA, pinfo->session_info.tacacs_user.author_data);
	XFREE(MTYPE_AAA, pinfo->session_info.tacacs_user.acct_data);*/
	
	XFREE(MTYPE_AAA, pinfo);
	hios_hash_delete(&g_user_table.user_authing_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);
	aaa_free_user_authing_id(user_id);
	
	return AAA_OK;
}

int aaa_user_authing_del_all(void)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		pinfo = (USER_AUTHING_INFO *)(node->data);
		if(pinfo == NULL)
			return AAA_OK;

		aaa_user_authing_del(pinfo->user_id);	
	}
	return AAA_OK;
}



/* 在radius的hash表中添加会话 */
int aaa_user_online_add(USER_ONLINE_INFO *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	/*pinfo->user_id = aaa_alloc_user_online_id (pinfo->log_method);
	if(0 == pinfo->user_id)
		return AAA_OVERSIZE;*/

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}

	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->user_id;
	pbucket->data = pinfo;
	if(hios_hash_add(&g_user_table.user_online_table, pbucket) != 0)
	{
		XFREE(MTYPE_AAA, pinfo);
		XFREE(MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_COMMON, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_user_table.user_online_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

USER_ONLINE_INFO *aaa_user_online_get_by_id(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_online_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (USER_ONLINE_INFO *)cursor->data;
	if(user_id == pinfo->user_id)
		return pinfo;
	else
		return NULL;
}

USER_ONLINE_INFO *aaa_user_online_get_by_name(const char *name)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		if((strlen(name) == strlen(pinfo->user_info.username)) &&
			(0 == strncmp(pinfo->user_info.username, name, strlen(name))))
		{
			return pinfo;
		}
	}
	return NULL;
}


int aaa_user_online_del(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	cursor = hios_hash_find (&g_user_table.user_online_table, (void *)user_id);
	if (NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (USER_ONLINE_INFO *)cursor->data;

	radius_attr_list_destory (pinfo->session_info.radius_user.auth_pkt.attribute_list);
    radius_attr_list_destory (pinfo->session_info.radius_user.acct_pkt.attribute_list);


	/* 这里的释放待解决 */
#if 0
	if (pinfo->session_info.tacacs_user.authen_data)
		XFREE (MTYPE_AAA, pinfo->session_info.tacacs_user.authen_data);
	if (pinfo->session_info.tacacs_user.author_data)
		XFREE (MTYPE_AAA, pinfo->session_info.tacacs_user.author_data);
	if (pinfo->session_info.tacacs_user.acct_data)
		XFREE (MTYPE_AAA, pinfo->session_info.tacacs_user.acct_data);
#endif

	XFREE (MTYPE_AAA, pinfo);
	hios_hash_delete (&g_user_table.user_online_table, cursor);
	XFREE (MTYPE_HASH_BACKET, cursor);
	aaa_free_user_online_id (user_id);
	return AAA_OK;
}

int aaa_user_online_del_all(void)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return AAA_OK;

		aaa_record_exit_login_time (&pinfo->user_info.time_exit);
		aaa_user_log_add (aaa_copy_online_to_log(pinfo));			//记录登录日志

		aaa_user_online_del (pinfo->user_id);	
	}
	return AAA_OK;
}



/* 在用户日志hash表中添加会话 */
int aaa_user_log_add(USER_LOG_INFO *pinfo)
{
	struct hash_bucket *pbucket = NULL;
	USER_LOG_INFO *pinfo_find = NULL;
	struct hash_bucket *node = NULL;
	void *cursor = NULL;

	/*********************** log记录到/data/dat/record_log.tet中 **************************/
	char strLog[256];
	char strLoginMode[64];
	char strLoginResult[64];
	char strLoginTime[64];
	char strOnlineTime[64];
	memset (strLog, 0, sizeof(strLog));
	memset (strLoginMode, 0, sizeof(strLoginMode));
	memset (strLoginResult, 0, sizeof(strLoginResult));
	memset (strLoginTime, 0, sizeof(strLoginTime));
	memset (strOnlineTime, 0, sizeof(strOnlineTime));

	switch(pinfo->log_result)
	{
		case LOGIN_FAILED: 
			strcpy (strLoginResult, "failed");
			break;
		case LOGIN_TIMEOUT:
			strcpy (strLoginResult, "timeout");
			break;
		case LOGIN_REPEATED:
			strcpy (strLoginResult, "repeated");
			break;
		case LOGIN_MAX:
			strcpy (strLoginResult, "reach max number");
			break;
		case LOGIN_SUCCESS:
			strcpy (strLoginResult, "success");
			break;
		default:
			strcpy (strLoginResult, "unknown");
			break;
	}
	
	aaa_get_local_time_str(pinfo->user_info.time_start, strLoginTime);

	if (LOGIN_SUCCESS == pinfo->log_result)
	{
		aaa_get_user_online_time(pinfo->user_info.time_start, strOnlineTime);
		switch (pinfo->log_method)
		{
			case METHOD_RADIUS:
				strcpy (strLoginMode, "radius");
				break;
			case METHOD_TACACS:
				strcpy (strLoginMode, "tacacs");
				break;
			case METHOD_LOCAL:
				strcpy (strLoginMode, "local");
				break;
			case METHOD_NONE:
				strcpy (strLoginMode, "none");
				break;
			default:
				strcpy (strLoginMode, "unknown");
				break;
		}
		sprintf (strLog, "user[%s][level:%d] login[%s] by[%s] from[%s] at[%s], online-time[%s]",
			pinfo->user_info.username,
			pinfo->user_info.level_author,
			strLoginResult,
			strLoginMode,
			pinfo->user_info.ttyname,
			strLoginTime,
			strOnlineTime);
	}
	else
	{
		sprintf (strLog, "user[%s] login[%s] from[%s] at[%s]",
			pinfo->user_info.username,
			strLoginResult,
			pinfo->user_info.ttyname,
			strLoginTime);
	}
	zlog_info ("aaa log : %s\n", strLog);
	
	if (0 == strcmp(SUPER_USER_NAME, pinfo->user_info.username)) //superadmin 不记录到hash中
	{
		return AAA_OK;
	}

	/********************************* end ***************************************/

	pinfo->user_id = aaa_alloc_user_log_id();
	
	if(0 == pinfo->user_id)		//表示id已循环分配一周，清空所有log，重新从1开始记录
	{
		for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
		{
			pinfo_find = (USER_LOG_INFO *)(node->data);
			if(pinfo_find == NULL)
				return AAA_OK;

			aaa_user_log_del (pinfo_find->user_id);
		}
		pinfo->user_id = aaa_alloc_user_log_id();	//在分配得到 id = 1
	}
	else if(pinfo->user_id > AAA_LOG_NUM_MAX)	//超过最大记录数，清空表中最早的log，再添加最新的log
	{
		pinfo_find = aaa_user_log_get_by_id (pinfo->user_id - AAA_LOG_NUM_MAX);
		
		if(NULL != pinfo_find)
		{
			aaa_user_log_del (pinfo_find->user_id);
		}
	}

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}

	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->user_id;
	pbucket->data = pinfo;
	if(hios_hash_add (&g_user_table.user_log_table, pbucket) != 0)
	{
		XFREE (MTYPE_AAA, pinfo);
		XFREE (MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_COMMON, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_user_table.user_log_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

USER_LOG_INFO *aaa_user_log_get_by_id(uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_log_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (USER_LOG_INFO *)cursor->data;
	if(user_id == pinfo->user_id)
		return pinfo;
	else
		return NULL;
}

USER_LOG_INFO *aaa_user_log_get_by_name(const char *name)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		if((strlen(name) == strlen(pinfo->user_info.username)) &&
			(0 == strncmp(pinfo->user_info.username, name, strlen(name))))
		{
			return pinfo;
		}
	}
	return NULL;
}

int aaa_user_log_del (uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;

	cursor = hios_hash_find(&g_user_table.user_log_table, (void *)user_id);
	if(NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (USER_LOG_INFO *)cursor->data;
	
	XFREE(MTYPE_AAA, pinfo);
	hios_hash_delete(&g_user_table.user_log_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);
	return AAA_ERROR;
}


int aaa_user_log_del_all(void)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return AAA_OK;
		
		aaa_user_log_del(pinfo->user_id);	
	}
	return AAA_OK;
}


/* 将正在认证用户的信息拷贝到历史用户信息中 */
USER_LOG_INFO *aaa_copy_authing_to_log(USER_AUTHING_INFO *pinfo, LOGIN_RESULT result)
{
	USER_LOG_INFO *pinfo_log = NULL;
	if(NULL == (pinfo_log = (USER_LOG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_LOG_INFO))))
		return NULL;

	memset(pinfo_log, 0, sizeof(USER_LOG_INFO));
	//pinfo_log->user_id = aaa_alloc_user_log_id();
	memcpy(&pinfo_log->user_info, &pinfo->user_info, sizeof(USER_INFO));
	
	pinfo_log->log_result = result;
	aaa_record_start_login_time(&pinfo_log->user_info.time_start);
	return pinfo_log;
}

/* 将在线用户的信息拷贝到历史用户信息中 */
USER_LOG_INFO *aaa_copy_online_to_log(USER_ONLINE_INFO *pinfo)
{
	USER_LOG_INFO *pinfo_log = NULL;
	if(NULL == (pinfo_log = (USER_LOG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_LOG_INFO))))
		return NULL;

	memset(pinfo_log, 0, sizeof(USER_LOG_INFO));
	//pinfo_log->user_id = aaa_alloc_user_log_id();
	memcpy(&pinfo_log->user_info, &pinfo->user_info, sizeof(USER_INFO));
	pinfo_log->log_method = pinfo->log_method;
	pinfo_log->log_result = LOGIN_SUCCESS;

	aaa_record_exit_login_time(&pinfo_log->user_info.time_exit);
	return pinfo_log;
}


/* 将在线用户的信息拷贝到历史用户信息中 */
USER_ONLINE_INFO *aaa_copy_authing_to_online(USER_AUTHING_INFO *pinfo)
{
	USER_ONLINE_INFO *pinfo_online = NULL;
	if(NULL == (pinfo_online = (USER_ONLINE_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_ONLINE_INFO))))
		return NULL;

	memset(pinfo_online, 0, sizeof(USER_LOG_INFO));

	if (0 == strcmp (pinfo->user_info.username, SUPER_USER_NAME))
	{
		pinfo_online->user_id = aaa_alloc_user_online_id (METHOD_LOCAL);
		pinfo_online->log_method = METHOD_LOCAL;
	}
	else
	{
		pinfo_online->user_id = aaa_alloc_user_online_id (aaa_get_auth_order(pinfo->auth_order));		
		pinfo_online->log_method = aaa_get_auth_order (pinfo->auth_order);
	}		
	pinfo_online->alive_time = aaa_alive_time_max_get ();
	memcpy (&pinfo_online->user_info, &pinfo->user_info, sizeof (USER_INFO));
	memcpy(&pinfo_online->session_info, &pinfo->session_info, sizeof (SESSION_INFO));

	if (METHOD_RADIUS == pinfo_online->log_method)
		pinfo_online->session_info.radius_user.acct_update_inteval = radius_acct_update_interval_get ();
	else if (METHOD_TACACS == pinfo_online->log_method)
		pinfo_online->session_info.tacacs_user.acct_update_time = tac_plus_acct_update_interval_get ();
	
	aaa_record_start_login_time (&pinfo_online->user_info.time_start);
	return pinfo_online;
}




/* 用户id缓存区 */
static uint32_t	user_online_id_buf[AAA_ONLINE_NUM_MAX/32] = {0};

/* 分配不重复的用户id，并分配最小的未使用的id */
/* 返回0		：达到最大记录数目，分配id失败 */
/* 返回范围	：1~AAA_ONLINE_NUM_MAX */
uint32_t aaa_alloc_user_online_id(LOG_METHOD method)
{
	uint32_t id = 0;
	int i,j;
	for(i = 0;i < AAA_ONLINE_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_online_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(0 == ((user_online_id_buf[i] >> j) & 0x01))
				{
					id = (32*i + j + 1);
					id |= (method << 24);
					user_online_id_buf[i] |= (0x00000001 << j);
					return id;
				}
			}
		}
	}
	return id;
}


int aaa_free_user_online_id(uint32_t user_id)
{
	int i = ((user_id & 0x00ffffff) - 1) / 32;
	int j = ((user_id & 0x00ffffff) - 1) % 32;
	if((user_online_id_buf[i] >> j) & 0x01)		//该id已分配，可释放
	{
		user_online_id_buf[i] ^= (0x000000001 << j);
		return AAA_OK;
	}
	else										//该id未分配，释放未分配的id
	{
		return AAA_FAIL;
	}
}


/* 获取在线用户总数 */
int aaa_get_user_online_num(void)
{
	int sum = 0;
	int i,j;
	for(i = 0;i < AAA_ONLINE_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_online_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(1 == ((user_online_id_buf[i] >> j) & 0x01))
				{
					sum++;
				}
			}
		}
		sum += 32;
	}
	return sum;
}



/* 用户id缓存区 */
static uint32_t	user_config_id_buf[AAA_CONFIG_NUM_MAX/32] = {0};

/* 分配不重复的用户id，并分配最小的未使用的id */
/* 返回0		：达到最大记录数目，分配id失败 */
/* 返回范围	：1~AAA_CONFIG_NUM_MAX */
uint32_t aaa_alloc_user_config_id(LOG_METHOD method)
{
	uint32_t id = 0;
	int i,j;
	for(i = 0;i < AAA_CONFIG_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_config_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(0 == ((user_config_id_buf[i] >> j) & 0x01))
				{
					id = (32*i + j + 1);
					id |= (method << 24);
					user_config_id_buf[i] |= (0x00000001 << j);
					return id;
				}
			}
		}
	}
	return id;
}

int aaa_free_user_config_id(uint32_t user_id)
{
	int i = ((user_id & 0x00ffffff) - 1) / 32;
	int j = ((user_id & 0x00ffffff) - 1) % 32;
	if((user_config_id_buf[i] >> j) & 0x01)		//该id已分配，可释放
	{
		user_config_id_buf[i] ^= (0x000000001 << j);
		return AAA_OK;
	}
	else										//该id未分配，释放未分配的id
	{
		return AAA_FAIL;
	}
}


/* 获取在线用户总数 */
int aaa_get_user_config_num(void)
{
	int sum = 0;
	int i,j;
	for(i = 0;i < AAA_CONFIG_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_config_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(1 == ((user_config_id_buf[i] >> j) & 0x01))
				{
					sum++;
				}
			}
		}
		sum += 32;
	}
	return sum;
}


/* 用户id缓存区 */
static uint32_t	user_log_id_buf = 0;

/* 分配不重复的用户id，并分配最小的未使用的id */
/* 返回0		：达到最大记录数目，分配id失败 */
/* 返回范围	：1~AAA_HISTORY_NUM_MAX */
uint32_t aaa_alloc_user_log_id(void)
{
	user_log_id_buf++;	
	return user_log_id_buf;
}


/* 获取记录的历史用户总数 */
int aaa_get_user_log_num(void)
{
	if (user_log_id_buf > AAA_LOG_NUM_MAX)
		return AAA_LOG_NUM_MAX;
	else
		return user_log_id_buf;
}



/* 用户id缓存区 */
static uint32_t	user_authing_id_buf[AAA_AUTHING_NUM_MAX/32] = {0};

/* 分配不重复的用户id，并分配最小的未使用的id */
/* 返回0		：达到最大记录数目，分配id失败 */
/* 返回范围	：1~AAA_AUTHING_NUM_MAX */
uint32_t aaa_alloc_user_authing_id(void)
{
	uint32_t id = 0;
	int i,j;
	for(i = 0;i < AAA_AUTHING_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_authing_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(0 == ((user_authing_id_buf[i] >> j) & 0x01))
				{
					id = (32*i + j + 1);
					user_authing_id_buf[i] |= (0x00000001 << j);
					return id;
				}
			}
		}
	}
	return id;
}

int aaa_free_user_authing_id(uint32_t user_id)
{
	int i = (user_id - 1) / 32;
	int j = (user_id - 1) % 32;
	if((user_authing_id_buf[i] >> j) & 0x01)		//该id已分配，可释放
	{
		user_authing_id_buf[i] ^= (0x000000001 << j);
		return AAA_OK;
	}
	else										//该id未分配，释放未分配的id
	{
		return AAA_FAIL;
	}
}


/* 获取正在登陆用户总数 */
int aaa_get_user_authing_num(void)
{
	int sum = 0;
	int i,j;
	for(i = 0;i < AAA_AUTHING_NUM_MAX/32;i++)
	{
		if(0xffffffff != user_authing_id_buf[i])
		{
			for(j = 0;j < 32;j++)
			{
				if(1 == ((user_authing_id_buf[i] >> j) & 0x01))
				{
					sum++;
				}
			}
		}
		sum += 32;
	}
	return sum;
}




/* record time of start login */
int aaa_record_start_login_time(time_t *t)
{
	time(t);
	return AAA_OK;
}

/* record time of exit login */
int aaa_record_exit_login_time(time_t *t)
{
	time(t);
	return AAA_OK;
}

/* 根据用户的登录时间t，获取在线用户的在线时长，并转换为字符串存放在*str中 */
int aaa_get_user_online_time(time_t t, char *str)
{	
	struct tm *gmt = NULL;
	time_t time_now, time_buf;
	time(&time_now);

	time_buf = time_now - t;

	gmt = gmtime(&time_buf);

	/* x day x:x:x */
    sprintf(str, "%d day %d:%d:%d", gmt->tm_mday-1, gmt->tm_hour,gmt->tm_min, gmt->tm_sec);
	return AAA_OK;
}

/* 根据用户的登录时间time_start、退出时间time_exit，获取用户的历史在线时长,
 * 并转换为字符串存放在*str中
 */
int aaa_get_user_log_online_time(time_t time_start, time_t time_exit, char *str)
{	
	struct tm *gmt = NULL;
	time_t time_buf;

	time_buf = time_exit - time_start;

	gmt = gmtime(&time_buf);

	/* x day x:x:x */
    sprintf(str, "%d day %d:%d:%d", gmt->tm_mday-1, gmt->tm_hour,gmt->tm_min, gmt->tm_sec);
	return AAA_OK;
}

/* 将时间t转换为格式：年/月/日 周 时/分/秒                 */
int aaa_get_local_time_str(time_t t, char *str)
{
	const char *wday[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	struct tm *p;
	p = localtime(&t);
	
	sprintf(str, "%d/%d/%d %s %d:%d:%d", 1900 + p->tm_year, 1+ p->tm_mon, p->tm_mday,\
		wday[p->tm_wday], p->tm_hour, p->tm_min, p->tm_sec);

	return AAA_OK;
}


/* 显示本地用户配置信息 */
void aaa_show_user_config(struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;
	
	vty_out(vty, "----------------------user config information----------------------%s", VTY_NEWLINE);
	char str[128];
	int id = 0;	
	
	for((node) = hios_hash_start(&g_user_table.user_config_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_config_table, &cursor))		
	{
		pinfo = (USER_CONFIG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->local_auth_user.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : %s", "name", pinfo->local_auth_user.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : %s", "password", pinfo->local_auth_user.password);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : %d", "level", pinfo->local_auth_user.level);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : %d", "max repeat num", pinfo->local_auth_user.max_repeat_num);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-15s : ", "service");
		aaa_h3c_get_local_user_service_type(pinfo, &str[18]);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}	
}



/* 显示所有在线用户信息 */
void aaa_show_user_online_all (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	char str[128];
	char str_login_mode[16];
	char str_login_time[32];
	char str_online_time[32];		
	int id = 0;
	
	vty_out(vty, "----------------------user online information----------------------%s", VTY_NEWLINE);	

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		id++;			
		memset(str_login_mode,  0, sizeof(str_login_mode));
		memset(str_login_time,  0, sizeof(str_login_time));
		memset(str_online_time, 0, sizeof(str_online_time));

		if(METHOD_RADIUS == pinfo->log_method)
			strcpy(str_login_mode, "radius");
		else if(METHOD_TACACS == pinfo->log_method)
			strcpy(str_login_mode, "tacacs");
		else if(METHOD_LOCAL == pinfo->log_method)
			strcpy(str_login_mode, "local");
		else if(METHOD_NONE == pinfo->log_method)
			strcpy(str_login_mode, "none");

		aaa_get_local_time_str(pinfo->user_info.time_start, str_login_time);
		aaa_get_user_online_time(pinfo->user_info.time_start, str_online_time);

		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "authorize level", pinfo->user_info.level_author);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "current   level", pinfo->user_info.level_current);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  mode", str_login_mode);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "online time", str_online_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}

/* 显示radius在线用户信息 */
void aaa_show_user_online_radius (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	char str[128];
	char str_login_mode[16];
	char str_login_time[32];
	char str_online_time[32];		
	int id = 0;	

	vty_out(vty, "-----------------online information of radius users----------------%s", VTY_NEWLINE);

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if(METHOD_RADIUS != pinfo->log_method)
			continue;

		id++;
		memset(str_login_mode,  0, sizeof(str_login_mode));
		memset(str_login_time,  0, sizeof(str_login_time));
		memset(str_online_time, 0, sizeof(str_online_time));

		if(METHOD_RADIUS == pinfo->log_method)
			strcpy(str_login_mode, "radius");
		else if(METHOD_TACACS == pinfo->log_method)
			strcpy(str_login_mode, "tacacs");
		else if(METHOD_LOCAL == pinfo->log_method)
			strcpy(str_login_mode, "local");
		else if(METHOD_NONE == pinfo->log_method)
			strcpy(str_login_mode, "none");

		aaa_get_local_time_str(pinfo->user_info.time_start, str_login_time);
		aaa_get_user_online_time(pinfo->user_info.time_start, str_online_time);

		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "authorize level", pinfo->user_info.level_author);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "current   level", pinfo->user_info.level_current);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  mode", str_login_mode);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "online time", str_online_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}

/* 显示tacacs在线用户信息 */
void aaa_show_user_online_tacacs (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	char str[128];
	char str_login_mode[16];
	char str_login_time[32];
	char str_online_time[32];		
	int id = 0;	

	vty_out(vty, "-----------------online information of tacacs users----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if(METHOD_TACACS != pinfo->log_method)
			continue;

		id++;
		memset(str_login_mode,  0, sizeof(str_login_mode));
		memset(str_login_time,  0, sizeof(str_login_time));
		memset(str_online_time, 0, sizeof(str_online_time));

		if(METHOD_RADIUS == pinfo->log_method)
			strcpy(str_login_mode, "radius");
		else if(METHOD_TACACS == pinfo->log_method)
			strcpy(str_login_mode, "tacacs");
		else if(METHOD_LOCAL == pinfo->log_method)
			strcpy(str_login_mode, "local");
		else if(METHOD_NONE == pinfo->log_method)
			strcpy(str_login_mode, "none");

		aaa_get_local_time_str(pinfo->user_info.time_start, str_login_time);
		aaa_get_user_online_time(pinfo->user_info.time_start, str_online_time);

		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "authorize level", pinfo->user_info.level_author);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "current   level", pinfo->user_info.level_current);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  mode", str_login_mode);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "online time", str_online_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}

/* 显示local在线用户信息 */
void aaa_show_user_online_local (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	char str[128];
	char str_login_mode[16];
	char str_login_time[32];
	char str_online_time[32];		
	int id = 0;	

	vty_out(vty, "-----------------online information of local users----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if(METHOD_LOCAL != pinfo->log_method)
			continue;

		id++;
		memset(str_login_mode,  0, sizeof(str_login_mode));
		memset(str_login_time,  0, sizeof(str_login_time));
		memset(str_online_time, 0, sizeof(str_online_time));

		if(METHOD_RADIUS == pinfo->log_method)
			strcpy(str_login_mode, "radius");
		else if(METHOD_TACACS == pinfo->log_method)
			strcpy(str_login_mode, "tacacs");
		else if(METHOD_LOCAL == pinfo->log_method)
			strcpy(str_login_mode, "local");
		else if(METHOD_NONE == pinfo->log_method)
			strcpy(str_login_mode, "none");

		aaa_get_local_time_str(pinfo->user_info.time_start, str_login_time);
		aaa_get_user_online_time(pinfo->user_info.time_start, str_online_time);

		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "authorize level", pinfo->user_info.level_author);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "current   level", pinfo->user_info.level_current);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  mode", str_login_mode);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "online time", str_online_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}

/* 显示none在线用户信息 */
void aaa_show_user_online_none (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	char str[128];
	char str_login_mode[16];
	char str_login_time[32];
	char str_online_time[32];		
	int id = 0;	
	
	vty_out(vty, "-----------------online information of none users----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if(METHOD_NONE != pinfo->log_method)
			continue;

		id++;
		memset(str_login_mode,  0, sizeof(str_login_mode));
		memset(str_login_time,  0, sizeof(str_login_time));
		memset(str_online_time, 0, sizeof(str_online_time));

		if(METHOD_RADIUS == pinfo->log_method)
			strcpy(str_login_mode, "radius");
		else if(METHOD_TACACS == pinfo->log_method)
			strcpy(str_login_mode, "tacacs");
		else if(METHOD_LOCAL == pinfo->log_method)
			strcpy(str_login_mode, "local");
		else if(METHOD_NONE == pinfo->log_method)
			strcpy(str_login_mode, "none");

		aaa_get_local_time_str(pinfo->user_info.time_start, str_login_time);
		aaa_get_user_online_time(pinfo->user_info.time_start, str_online_time);

		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "authorize level", pinfo->user_info.level_author);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "current   level", pinfo->user_info.level_current);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  mode", str_login_mode);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "online time", str_online_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}


/* 显示在线用户信息 */
void aaa_show_user_online(struct vty *vty, const char *str_mode)
{
	if(NULL == str_mode)
	{
		aaa_show_user_online_all (vty);
	}
	else
	{
		if (0 == strcmp (str_mode, "radius"))
			aaa_show_user_online_radius (vty);
		else if (0 == strcmp (str_mode, "tacacs"))
			aaa_show_user_online_tacacs (vty);
		if (0 == strcmp (str_mode, "local"))
			aaa_show_user_online_local (vty);
		if (0 == strcmp (str_mode, "none"))
			aaa_show_user_online_none (vty);
	}
}


/* 显示所有用户日志信息 */
void aaa_show_user_log_all (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;
	
	vty_out(vty, "------------------------user log information-----------------------%s", VTY_NEWLINE);
	char str[128];
	char str_login_time[32];
	char str_online_time[32];
	int id = 0;
	
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-18s : %d", "level", pinfo->user_info.level_author);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			if(METHOD_RADIUS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "radius");
			else if(METHOD_TACACS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "tacacs");
			else if(METHOD_LOCAL == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "local");
			else if(METHOD_NONE == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "none");
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}		

		memset(str, 0, sizeof(str));
		if(LOGIN_FAILED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "failed");
		else if(LOGIN_TIMEOUT == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "timeout");
		else if(LOGIN_REPEATED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "repeated");
		else if(LOGIN_SUCCESS == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "success");
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		memset(str_login_time, 0, sizeof(str_login_time));
		aaa_get_local_time_str (pinfo->user_info.time_start, str_login_time);
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			memset(str_online_time, 0, sizeof(str_online_time));
			aaa_get_user_log_online_time (pinfo->user_info.time_start, pinfo->user_info.time_exit, str_online_time);
			sprintf(str, "%-18s : %s", "online time", str_online_time);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}	

		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}


/* 显示radius用户日志信息 */
void aaa_show_user_log_radius (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;
		
	char str[128];
	char str_login_time[32];
	char str_online_time[32];
	int id = 0;

	vty_out(vty, "-------------------log information of radius users-----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if (METHOD_RADIUS != pinfo->log_method)
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-18s : %d", "level", pinfo->user_info.level_author);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			if(METHOD_RADIUS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "radius");
			else if(METHOD_TACACS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "tacacs");
			else if(METHOD_LOCAL == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "local");
			else if(METHOD_NONE == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "none");
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}		

		memset(str, 0, sizeof(str));
		if(LOGIN_FAILED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "failed");
		else if(LOGIN_TIMEOUT == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "timeout");
		else if(LOGIN_REPEATED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "repeated");
		else if(LOGIN_SUCCESS == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "success");
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		memset(str_login_time, 0, sizeof(str_login_time));
		aaa_get_local_time_str (pinfo->user_info.time_start, str_login_time);
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			memset(str_online_time, 0, sizeof(str_online_time));
			aaa_get_user_log_online_time (pinfo->user_info.time_start, pinfo->user_info.time_exit, str_online_time);
			sprintf(str, "%-18s : %s", "online time", str_online_time);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}	

		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}


/* 显示tacacs用户日志信息 */
void aaa_show_user_log_tacacs (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;	
	
	char str[128];
	char str_login_time[32];
	char str_online_time[32];
	int id = 0;
	
	vty_out(vty, "-------------------log information of tacacs users-----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if (METHOD_TACACS != pinfo->log_method)
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-18s : %d", "level", pinfo->user_info.level_author);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			if(METHOD_RADIUS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "radius");
			else if(METHOD_TACACS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "tacacs");
			else if(METHOD_LOCAL == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "local");
			else if(METHOD_NONE == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "none");
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}		

		memset(str, 0, sizeof(str));
		if(LOGIN_FAILED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "failed");
		else if(LOGIN_TIMEOUT == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "timeout");
		else if(LOGIN_REPEATED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "repeated");
		else if(LOGIN_SUCCESS == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "success");
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		memset(str_login_time, 0, sizeof(str_login_time));
		aaa_get_local_time_str (pinfo->user_info.time_start, str_login_time);
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			memset(str_online_time, 0, sizeof(str_online_time));
			aaa_get_user_log_online_time (pinfo->user_info.time_start, pinfo->user_info.time_exit, str_online_time);
			sprintf(str, "%-18s : %s", "online time", str_online_time);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}	

		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}


/* 显示local用户日志信息 */
void aaa_show_user_log_local (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;	
	
	char str[128];
	char str_login_time[32];
	char str_online_time[32];
	int id = 0;

	vty_out(vty, "-------------------log information of local users-----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if (METHOD_LOCAL != pinfo->log_method)
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-18s : %d", "level", pinfo->user_info.level_author);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			if(METHOD_RADIUS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "radius");
			else if(METHOD_TACACS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "tacacs");
			else if(METHOD_LOCAL == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "local");
			else if(METHOD_NONE == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "none");
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}		

		memset(str, 0, sizeof(str));
		if(LOGIN_FAILED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "failed");
		else if(LOGIN_TIMEOUT == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "timeout");
		else if(LOGIN_REPEATED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "repeated");
		else if(LOGIN_SUCCESS == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "success");
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		memset(str_login_time, 0, sizeof(str_login_time));
		aaa_get_local_time_str (pinfo->user_info.time_start, str_login_time);
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			memset(str_online_time, 0, sizeof(str_online_time));
			aaa_get_user_log_online_time (pinfo->user_info.time_start, pinfo->user_info.time_exit, str_online_time);
			sprintf(str, "%-18s : %s", "online time", str_online_time);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}	

		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}

/* 显示none用户日志信息 */
void aaa_show_user_log_none (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;	
	
	char str[128];
	char str_login_time[32];
	char str_online_time[32];
	int id = 0;

	vty_out(vty, "-------------------log information of none users-----------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{
		pinfo = (USER_LOG_INFO *)(node->data);
		if(pinfo == NULL)
			return;

		if(0 == strcmp(pinfo->user_info.username, SUPER_USER_NAME)) //不显示超级用户
			continue;

		if (METHOD_NONE != pinfo->log_method)
			continue;

		id++;
		if(1 == id)
			vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
		
		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %d", "id", id);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "name", pinfo->user_info.username);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			sprintf(str, "%-18s : %d", "level", pinfo->user_info.level_author);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			if(METHOD_RADIUS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "radius");
			else if(METHOD_TACACS == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "tacacs");
			else if(METHOD_LOCAL == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "local");
			else if(METHOD_NONE == pinfo->log_method)
				sprintf(str, "%-18s : %s", "login  mode", "none");
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}		

		memset(str, 0, sizeof(str));
		if(LOGIN_FAILED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "failed");
		else if(LOGIN_TIMEOUT == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "timeout");
		else if(LOGIN_REPEATED == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "repeated");
		else if(LOGIN_SUCCESS == pinfo->log_result)
			sprintf(str, "%-18s : %s", "login  result", "success");
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		sprintf(str, "%-18s : %s", "login  from", pinfo->user_info.ttyname);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);

		memset(str, 0, sizeof(str));
		memset(str_login_time, 0, sizeof(str_login_time));
		aaa_get_local_time_str (pinfo->user_info.time_start, str_login_time);
		sprintf(str, "%-18s : %s", "login  time", str_login_time);
		vty_out(vty, "%s%s", str, VTY_NEWLINE);
		
		if(LOGIN_SUCCESS == pinfo->log_result)
		{
			memset(str, 0, sizeof(str));
			memset(str_online_time, 0, sizeof(str_online_time));
			aaa_get_user_log_online_time (pinfo->user_info.time_start, pinfo->user_info.time_exit, str_online_time);
			sprintf(str, "%-18s : %s", "online time", str_online_time);
			vty_out(vty, "%s%s", str, VTY_NEWLINE);
		}	

		vty_out(vty, "*******************************************************************%s", VTY_NEWLINE);
	}
}



/* 显示用户日志信息 */
void aaa_show_user_log(struct vty *vty, const char *str_mode)
{
	if(NULL == str_mode)
	{
		aaa_show_user_log_all (vty);
	}
	else
	{
		if (0 == strcmp (str_mode, "radius"))
			aaa_show_user_log_radius (vty);
		else if (0 == strcmp (str_mode, "tacacs"))
			aaa_show_user_log_tacacs (vty);
		if (0 == strcmp (str_mode, "local"))
			aaa_show_user_log_local (vty);
		if (0 == strcmp (str_mode, "none"))
			aaa_show_user_log_none (vty);
	}
}





/*****************************************  华三命令行相关函数 ***************************************/

/**
 * @brief      : 检测本地用户名的合法性
 * @param[in ] : username  - 用户名
 * @param[out] :
 * @return     : 0:有效, 1:长度过长或过短, 2:存在非法字符或禁用的用户名
 * @author     : Lipf
 * @date       : 2018年5月25日
 * @note       :
 */

int aaa_h3c_username_is_valid(char *username)
{
	size_t len_username = strlen(username);
	if((len_username < USER_NAME_MIN) || (len_username > USER_NAME_MAX))
	{
		return 1;
	}

	if((0 == strcmp(username, "a"))  ||\
	   (0 == strcmp(username, "al")) ||\
	   (0 == strcmp(username, "all")))
	{
		return 2;
	}
	   
	char *pBuf = username;
	while('\0' != *pBuf)
	{
		if(('\\' == *pBuf) ||\
		   ('|' == *pBuf) ||\
		   ('/' == *pBuf) ||\
		   (':' == *pBuf) ||\
		   ('*' == *pBuf) ||\
		   ('?' == *pBuf) ||\
		   ('<' == *pBuf) ||\
		   ('>' == *pBuf) ||\
		   ('@' == *pBuf))
		{
			return 2;
		}
		pBuf++;
	}

	return 0;
}



/**
 * @brief      : 增加本地用户
 * @param[in ] : vty    		- vty
 * @param[in ] : username 		- 用户名
 * @param[in ] : manage_flag 	- manage标识
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月25日
 * @note       :
 */

int aaa_h3c_add_local_user(struct vty *vty, char *username, int manage_flag)
{	
	USER_CONFIG_INFO *pinfo = NULL;		
	char *pprompt = NULL;	
	
	int res_username_check = aaa_h3c_username_is_valid(username);
	if(0 == res_username_check)
	{
		pinfo = aaa_user_config_get_by_name(username);
		if(NULL == pinfo)
		{
			pinfo = (USER_CONFIG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_CONFIG_INFO));
			if(NULL == pinfo)
			{
				return -1;
			}
			memset (pinfo, 0, sizeof(USER_CONFIG_INFO));
			pinfo->local_auth_user.encrypted = FALSE;
			pinfo->local_auth_user.level = 14;
			pinfo->local_auth_user.max_repeat_num = 3;

			if(manage_flag)
			{
				pinfo->local_auth_user.service = (SERVICE_TYPE_SSH|SERVICE_TYPE_TELNET|SERVICE_TYPE_TERMINAL);
			}
			
			strcpy(pinfo->local_auth_user.username, username);
			aaa_user_config_add(pinfo);			
			
		    pprompt = vty->change_prompt;
		    if ( pprompt )
		    {
		        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-luser-manage-%s)# ", 
					pinfo->local_auth_user.username);
			}
			vty->index = (void *)pinfo; // to fix
			vty->node = AAA_LOCAL_USER_NODE;
		}
		else
		{
			pprompt = vty->change_prompt;
		    if ( pprompt )
		    {
		        snprintf ( pprompt, VTY_BUFSIZ, "%%s(config-luser-manage-%s)# ", 
					pinfo->local_auth_user.username);
			}
			vty->index = (void *)pinfo; // to fix
			vty->node = AAA_LOCAL_USER_NODE;

			if(manage_flag)
			{
				pinfo->local_auth_user.service = (SERVICE_TYPE_SSH|SERVICE_TYPE_TELNET|SERVICE_TYPE_TERMINAL);
			}
		}

		return 0;
	}
	else if(1 == res_username_check)
	{
		vty_error_out (vty, "Cmd username is too long or too short, size <%d-%d>.%s", 
			USER_NAME_MIN, USER_NAME_MAX, VTY_NEWLINE);
		return -1;
	}
	else if(2 == res_username_check)
	{
		vty_error_out (vty, "Cmd username has illegal charactor.%s", VTY_NEWLINE);
		return -1;
	}
	else
	{
		vty_error_out (vty, "Unknown error.%s", VTY_NEWLINE);
		return -1;
	}
}



/**
 * @brief      : 配置本地用户的密码
 * @param[in ] : vty    		- vty
 * @param[in ] : password 		- 用户密码
 * @param[in ] : pw_type	 	- 密码配置方式
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月29日
 * @note       :
 */

int aaa_h3c_modify_local_user_password(struct vty *vty, char *password, int flag)
{	
	USER_CONFIG_INFO *pinfo = (USER_CONFIG_INFO *)vty->index;
	if(NULL == pinfo)
	{
		return -1;
	}

	if((strlen(password) < 1) || (strlen(password) > USER_PASSWORD_MAX))
	{
		vty_error_out (vty, "Cmd password is too short or too long, size <1-%d>.%s", USER_PASSWORD_MAX, VTY_NEWLINE);
		return -1;
	}
	
	switch(flag)
	{
		case 1:		//cipher
			return -1;
			break;

		case 2:		//hash
			return -1;
			break;

		case 3:		//simple
			strcpy(pinfo->local_auth_user.password, password);
			aaa_password_encode(pinfo->local_auth_user.password_encrypted, pinfo->local_auth_user.password);
			break;

		default:
			break;
	}
	return 0;
}


/**
 * @brief      : 删除本地用户的密码
 * @param[in ] : vty    		- vty
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月29日
 * @note       :
 */

int aaa_h3c_del_local_user_password(struct vty *vty)
{
	USER_CONFIG_INFO *pinfo = (USER_CONFIG_INFO *)vty->index;
	if(NULL == pinfo)
	{
		return -1;
	}

	memset(pinfo->local_auth_user.password, 0, USER_PASSWORD_MAX);
	memset(pinfo->local_auth_user.password_encrypted, 0, USER_PASSWORD_MAX);

	return 0;
}


/**
 * @brief      : 配置本地用户支持的服务类型
 * @param[in ] : vty    	- vty
 * @param[in ] : type    	- 服务类型
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月29日
 * @note       :
 */

int aaa_h3c_modify_local_user_service_type(struct vty *vty, char *type)
{
	USER_CONFIG_INFO *pinfo = (USER_CONFIG_INFO *)vty->index;
	if(NULL == pinfo)
	{
		return -1;
	}
	
	if(0 == memcmp(type, "ssh", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_SSH;
	}
	else if(0 == memcmp(type, "telnet", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_TELNET;
	}
	else if(0 == memcmp(type, "terminal", 3))
	{
		pinfo->local_auth_user.service |= SERVICE_TYPE_TERMINAL;
	}
	else
	{
		return -1;
	}

	return 0;
}



/**
 * @brief      : 配置本地用户支持的服务类型
 * @param[in ] : vty    	- vty
 * @param[in ] : type    	- 服务类型
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月29日
 * @note       :
 */

int aaa_h3c_undo_local_user_service_type(struct vty *vty, char *type)
{
	USER_CONFIG_INFO *pinfo = (USER_CONFIG_INFO *)vty->index;
	if(NULL == pinfo)
	{
		return -1;
	}
	
	if(0 == memcmp(type, "ssh", 3))
	{
		if(pinfo->local_auth_user.service & SERVICE_TYPE_SSH)
		{
			pinfo->local_auth_user.service ^= SERVICE_TYPE_SSH;
		}
	}
	else if(0 == memcmp(type, "telnet", 3))
	{
		if(pinfo->local_auth_user.service & SERVICE_TYPE_TELNET)
		{
			pinfo->local_auth_user.service ^= SERVICE_TYPE_TELNET;
		}
	}
	else if(0 == memcmp(type, "terminal", 3))
	{
		if(pinfo->local_auth_user.service & SERVICE_TYPE_TERMINAL)
		{
			pinfo->local_auth_user.service ^= SERVICE_TYPE_TERMINAL;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}



/**
 * @brief      : 获取本地用户支持的服务类型
 * @param[in ] : pinfo    	- 用户信息指针
 * @param[out] :
 * @return     : 成功返回0，否则返回 -1
 * @author     : Lipf
 * @date       : 2018年5月29日
 * @note       :
 */

int aaa_h3c_get_local_user_service_type(USER_CONFIG_INFO *pinfo, char *strService)
{
	char *buf = strService;

	if(SERVICE_TYPE_SSH & pinfo->local_auth_user.service)
	{
		memcpy(buf, "ssh ", 4);
		buf += 4;
	}

	if(SERVICE_TYPE_TELNET & pinfo->local_auth_user.service)
	{
		memcpy(buf, "telnet ", 7);
		buf += 7;
	}

	if(SERVICE_TYPE_TERMINAL & pinfo->local_auth_user.service)
	{
		strcpy(buf, "terminal");
	}

	return 0;
}


