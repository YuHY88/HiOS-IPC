/******************************************************************************
 * Filename: aaa_msg_handle.c
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
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/aaa_common.h>

#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc_n.h>


#include "aaa_msg_handle.h"
#include "aaa_msg_io.h"
#include "aaa_config.h"
#include "aaa_user_manager.h"
#include "aaa_radius_auth.h"
#include "aaa_radius_acct.h"
#include "aaa_tac_plus.h"

extern struct thread_master *aaa_master;


/* 处理登录请求消息 */
int aaa_handle_login_request(struct ipc_mesg_n *pmsg_ipc)
{
	AAA_LOGIN_REQUEST *pmsg = (AAA_LOGIN_REQUEST *)pmsg_ipc->msg_data;
	USER_AUTHING_INFO *pinfo = NULL;
	
	if(NULL == pmsg)
		return AAA_ERROR;
	if(strlen(pmsg->username) < 1)
	{
		aaa_send_login_response (pmsg->username, 0, pmsg->user_id_buf, LOGIN_FAILED, 0);
		return AAA_ERROR;
	}

	
#if 0
	printf("tty  : %s\n", pmsg->ttyname);
	printf("ip   : %x\n", pmsg->ip);
	printf("port : %d\n", pmsg->port);
#endif
	
	if(LOGIN_REQUEST == pmsg->type)			//登录请求
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa recv user[%s][%s][%s][%x][%d] login-request from vty\n",
			__func__, __LINE__,	pmsg->username, pmsg->password, pmsg->ttyname, pmsg->ip, pmsg->port);
		
		aaa_send_verify_msg (pmsg_ipc, pmsg->user_id, pmsg->user_id_buf,
			pmsg->username, AAA_MSG_AUTH_REQUEST);
		
		if (g_user_table.user_online_table.num_entries >= aaa_login_max_num_get ())
		{
			USER_LOG_INFO *pinfo_log = (USER_LOG_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_LOG_INFO));
			if(NULL == pinfo_log)
		    {
		        zlog_err("%s[%d] -> %s: XMALLOC error!\n", __FILE__, __LINE__, __func__);
		        return AAA_MALLOC_FAIL;
		    }	
			memset(pinfo_log, 0, sizeof(USER_LOG_INFO));
			
			aaa_send_login_response (pmsg->username, 0, pmsg->user_id_buf, LOGIN_MAX, 0);

			pinfo_log->user_id = aaa_alloc_user_log_id ();
			pinfo_log->log_method = METHOD_INVALID;
			pinfo_log->log_result = LOGIN_MAX;
			memcpy (pinfo_log->user_info.username, pmsg->username, strlen (pmsg->username));
			memcpy (pinfo_log->user_info.ttyname, pmsg->ttyname, strlen (pmsg->ttyname));
			pinfo_log->user_info.ip = pmsg->ip;
			pinfo_log->user_info.port = pmsg->port;
			time (&pinfo_log->user_info.time_start);
			aaa_user_log_add (pinfo_log);
		}
		else
		{
			pinfo = (USER_AUTHING_INFO *)XMALLOC(MTYPE_AAA, sizeof(USER_AUTHING_INFO));
			if(NULL == pinfo)
		    {
		        zlog_err("%s[%d] -> %s: XMALLOC error!\n", __FILE__, __LINE__, __func__);
		        return AAA_MALLOC_FAIL;
		    }	
			memset(pinfo, 0, sizeof(USER_AUTHING_INFO));			
			
			/* 记录用户信息 */
			memset(pinfo->user_info.username, 0, sizeof(pinfo->user_info.username));
			memset(pinfo->user_info.password_init, 0, sizeof(pinfo->user_info.password_init));
			memset(pinfo->user_info.password, 0, sizeof(pinfo->user_info.password));
			memset(pinfo->user_info.ttyname, 0, sizeof(pinfo->user_info.ttyname));			
			memcpy(pinfo->user_info.username, pmsg->username, strlen(pmsg->username));
			memcpy(pinfo->user_info.password_init, pmsg->password, strlen(pmsg->password));
			memcpy(pinfo->user_info.password, pmsg->password, strlen(pmsg->password));
			memcpy(pinfo->user_info.ttyname, pmsg->ttyname, strlen(pmsg->ttyname));
			pinfo->auth_order = 1;
			pinfo->user_id_buf = pmsg->user_id_buf;
			pinfo->user_info.ip = pmsg->ip;
			pinfo->user_info.port = pmsg->port;
			pinfo->aaa_result = AUTHEN_INVALID;
			pinfo->auth_finished = FALSE;

			/* 填写radius认证信息 */
			radius_store_auth_info_into_session(pinfo);
			radius_store_acct_info_into_session(pinfo);	
			pinfo->session_info.radius_user.auth_retry_times = g_aaa_cfgs.radius.retry_times;
			pinfo->session_info.radius_user.auth_timeleft_retry = g_aaa_cfgs.radius.rsp_timeout;
			pinfo->session_info.radius_user.acct_retry_times = g_aaa_cfgs.radius.retry_times;
			pinfo->session_info.radius_user.acct_timeleft_retry = g_aaa_cfgs.radius.rsp_timeout;
			pinfo->session_info.radius_user.acct_update_inteval = radius_acct_update_interval_get ();
			pinfo->session_info.radius_user.auth_result = AUTHEN_INVALID;
			pinfo->session_info.radius_user.acct_result = AUTHEN_INVALID;
			pinfo->session_info.radius_user.acct_send_flag = FALSE;
			pinfo->session_info.radius_user.acct_failed_cnt = 0;
			pinfo->session_info.radius_user.acct_timeleft_response = RADIUS_DEF_ACCT_TIMEOUT;
			
			/* 填写tacacs+认证信息，若与ftm之间ipc有问题，释放端口、申请端口将会阻塞一段时间 */
			char *remote_addr = NULL;
			tac_record_authen_info(pinfo, remote_addr, TAC_PLUS_AUTHEN_LOGIN, tac_plus_authen_mode_get());
			tac_record_author_info(pinfo, remote_addr);
			tac_record_acct_info(pinfo, remote_addr);
			pinfo->session_info.tacacs_user.authen_timeleft = TAC_PLUS_RSP_TIMEOUT;
			pinfo->session_info.tacacs_user.author_timeleft = TAC_PLUS_RSP_TIMEOUT;
			pinfo->session_info.tacacs_user.acct_timeleft = TAC_PLUS_RSP_TIMEOUT;
			pinfo->session_info.tacacs_user.acct_update_time = tac_plus_acct_update_interval_get ();
			pinfo->session_info.tacacs_user.authen_result = AUTHEN_INVALID;
			pinfo->session_info.tacacs_user.author_result = AUTHEN_INVALID;
			pinfo->session_info.tacacs_user.acct_result = AUTHEN_INVALID;
			
			/* 填写local认证信息，现在不需要，以后可扩展 */

			/* 填写none认证信息，现在不需要，以后可扩展 */

			
			aaa_user_authing_add (pinfo);	//添加到正在认证用户hash表中
		}			
	}
	else if(LOGIN_EXIT == pmsg->type)	//登出请求
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa recv user[%s] login-exit from vty\n",
			__func__, __LINE__,	pmsg->username);
		
		USER_ONLINE_INFO *pinfo_online = aaa_user_online_get_by_id(pmsg->user_id);
		if(NULL != pinfo_online)	//找到在线用户
		{
			aaa_send_verify_msg (pmsg_ipc, pmsg->user_id, pmsg->user_id_buf,
				pmsg->username, AAA_MSG_AUTH_REQUEST);
			aaa_record_exit_login_time (&pinfo_online->user_info.time_exit);
			aaa_user_log_add (aaa_copy_online_to_log(pinfo_online));		//记录登录日志
			if(AAA_OK == aaa_user_online_del(pinfo_online->user_id))		//删除在线用户消息成功
				return AAA_OK;
		}
		else
		{
			aaa_send_verify_msg_noack (pmsg_ipc);
		}
	}

	return AAA_OK;
}


/* 处理保活消息 */
void aaa_handle_echo_request(struct ipc_mesg_n *pmsg_ipc)
{
	time_t max_idle_time = aaa_user_max_idle_time_get();
	
	AAA_ECHO_REQUEST *pmsg	= (AAA_ECHO_REQUEST *)pmsg_ipc->msg_data;
	USER_ONLINE_INFO *pinfo = aaa_user_online_get_by_id(pmsg->user_id);
	if(NULL == pinfo)
	{
		aaa_send_force_exit_request (pmsg->username, pmsg->user_id, EXIT_IDLE);
	}
	else
	{
		pinfo->alive_time = aaa_alive_time_max_get();
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : user[%s][%x], idle_time[%d], max_idle_time[%d]\n",
			__func__, __LINE__,
			pmsg->username, pmsg->user_id,
			(int)pmsg->idle_time, (int)max_idle_time);

		if (max_idle_time)
		{
			if (pmsg->idle_time >= max_idle_time)	//用户空闲时间已超过最大允许空闲时间
			{	
				if (AAA_FAIL == aaa_send_force_exit_request (pinfo->user_info.username, pinfo->user_id, EXIT_IDLE))
				{
					aaa_user_log_add (aaa_copy_online_to_log (pinfo));
					aaa_user_online_del (pinfo->user_id);
					zlog_err ("%s[%d] : vty force-exit user error\n", __func__, __LINE__);
				}
				else
				{
					aaa_user_log_add (aaa_copy_online_to_log (pinfo));
					aaa_user_online_del (pinfo->user_id);
				}
			}
		}
	}
}

/* 处理权限提升消息 */
void aaa_handle_improve_level_request(struct ipc_mesg_n *pmsg_ipc)
{
	AAA_IMPROVE_LEVEL *pmsg	= (AAA_IMPROVE_LEVEL *)pmsg_ipc->msg_data;
	USER_ONLINE_INFO *pinfo = aaa_user_online_get_by_id(pmsg->user_id);
	if(NULL != pinfo)	//找到在线用户
	{
		if((strlen(pinfo->user_info.username) == strlen(pmsg->username))\
			&& (0 == strcmp(pinfo->user_info.username, pmsg->username)))
		{


/* 暂时屏蔽enable的密码验证，直接提升到最大权限 */
#if 0
			if(0xff == pmsg->level)
			{
				if(0 == aaa_enable_password_cmp(pmsg->enable_password, pmsg->level))
				{
					pinfo->user_info.level_current = pinfo->user_info.level_author;
					aaa_send_improve_level_response(pmsg->username, pmsg->user_id,
												IMPROVE_SUCCESS, pinfo->user_info.level_author);
				}
				else
					aaa_send_improve_level_response(pmsg->username, pmsg->user_id, IMPROVE_FAILED, 0);
			}
			else if(pmsg->level <= pinfo->user_info.level_author)
			{
				if(0 == aaa_enable_password_cmp(pmsg->enable_password, pmsg->level))
				{
					pinfo->user_info.level_current = pmsg->level;
					aaa_send_improve_level_response(pmsg->username, pmsg->user_id, IMPROVE_SUCCESS, pmsg->level);
				}
				else
					aaa_send_improve_level_response(pmsg->username, pmsg->user_id, IMPROVE_FAILED, 0);
			}
			else
				aaa_send_improve_level_response(pmsg->username, pmsg->user_id, IMPROVE_OVERFLOW, 0);
#endif

			pinfo->user_info.level_current = pinfo->user_info.level_author;
			aaa_send_improve_level_response (pmsg->username, pmsg->user_id, IMPROVE_SUCCESS,
				pinfo->user_info.level_current);
		}
	}
	else
		aaa_send_improve_level_response(pmsg->username, pmsg->user_id, IMPROVE_FAILED, 0);
}



/* 处理错误码上报消息 */
void aaa_handle_errno(struct ipc_mesg_n *pmsg_ipc)
{
	AAA_ERR_REPORT_MSG *pErrMsg = (AAA_ERR_REPORT_MSG *)pmsg_ipc->msg_data;
	switch(pErrMsg->errNo)
	{
		case ERR_INVALID:
			break;
			
		case ERR_VTY_RESET:
			aaa_user_authing_del_all();
			aaa_user_online_del_all();
			break;

		default:
			zlog_err("%s[%d] : unsupport error NO..\n", __func__, __LINE__);
			break;
	}
}


/* 处理从vty接收的消息 */
void aaa_handle_vty_msg(struct ipc_mesg_n *pmsg)
{
	switch(pmsg->msghdr.msg_subtype)	//接收到的消息类型
	{
		case AAA_MSG_AUTH_REQUEST:
			aaa_handle_login_request(pmsg);
			break;

		case AAA_MSG_ECHO:
			aaa_handle_echo_request(pmsg);
			break;

		case AAA_MSG_IMPROVE_LEVEL:
			aaa_handle_improve_level_request(pmsg);
			break;

		case AAA_MSG_ERRNO_REPORT:
			aaa_handle_errno(pmsg);
			break;

		default:
			break;
	}
}


/* 认证流程处理 */
int aaa_auth_process_check(void *para)
{
	USER_AUTHING_INFO *pinfo = NULL;	
	struct hash_bucket *node = NULL;
	void *cursor = NULL;

	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		if(NULL == (pinfo = (USER_AUTHING_INFO *)(node->data)))
			return AAA_ERROR;

		/* 检测到invalid，说明认证顺序已到末尾，向vty发送认证失败，认证结束 */
		if (0 == strcmp (SUPER_USER_NAME, pinfo->user_info.username))
		{
			if (TRUE == aaa_user_login_repeat (pinfo->user_info.username, METHOD_LOCAL))
			{
				aaa_send_login_response(pinfo->user_info.username, 0, 
					pinfo->user_id_buf, LOGIN_REPEATED, 0);
				aaa_user_log_add (aaa_copy_authing_to_log (pinfo, LOGIN_REPEATED));
				aaa_user_authing_del (pinfo->user_id);
				goto out;
			}
			aaa_method_local_process_check(pinfo);
		}
		else
		{
			switch(aaa_get_auth_order (pinfo->auth_order))
			{
				case METHOD_INVALID:
					zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa user[%s] authen finished\n",
						__func__, __LINE__, pinfo->user_info.username);
					pinfo->aaa_result = AUTHEN_FAILED;
					pinfo->auth_finished = TRUE;
					break;

				case METHOD_RADIUS:
					zlog_debug(AAA_DBG_RADIUS, "%s[%d] : aaa user[%s] authen by radius\n",
						__func__, __LINE__, pinfo->user_info.username);
					if (TRUE == aaa_user_login_repeat (pinfo->user_info.username, METHOD_RADIUS))
					{
						aaa_send_login_response(pinfo->user_info.username, 0,
							pinfo->user_id_buf, LOGIN_REPEATED, 0);
						aaa_user_log_add (aaa_copy_authing_to_log (pinfo, LOGIN_REPEATED));
						aaa_user_authing_del (pinfo->user_id);
						goto out;
					}					
					aaa_method_radius_process_check(pinfo);
					break;

				case METHOD_TACACS:
					zlog_debug(AAA_DBG_TACACS, "%s[%d] : aaa user[%s] authen by tacacs\n",
						__func__, __LINE__, pinfo->user_info.username);
					if (TRUE == aaa_user_login_repeat (pinfo->user_info.username, METHOD_TACACS))
					{
						aaa_send_login_response(pinfo->user_info.username, 0,
							pinfo->user_id_buf, LOGIN_REPEATED, 0);
						aaa_user_log_add (aaa_copy_authing_to_log (pinfo, LOGIN_REPEATED));
						aaa_user_authing_del (pinfo->user_id);
						goto out;
					}
					aaa_method_tacacs_process_check(pinfo);
					break;

				case METHOD_LOCAL:
					zlog_debug(AAA_DBG_LOCAL, "%s[%d] : aaa user[%s] authen by local\n",
						__func__, __LINE__, pinfo->user_info.username);
					
					if (TRUE == aaa_user_login_repeat (pinfo->user_info.username, METHOD_LOCAL))
					{
						aaa_send_login_response(pinfo->user_info.username, 0,
							pinfo->user_id_buf, LOGIN_REPEATED, 0);
						aaa_user_log_add (aaa_copy_authing_to_log (pinfo, LOGIN_REPEATED));
						aaa_user_authing_del (pinfo->user_id);
						goto out;
					}
					aaa_method_local_process_check(pinfo);
					break;

				case METHOD_NONE:
					zlog_debug(AAA_DBG_NONE, "%s[%d] : aaa user[%s] authen by none\n",
						__func__, __LINE__, pinfo->user_info.username);
					if (TRUE == aaa_user_login_repeat (pinfo->user_info.username, METHOD_NONE))
					{
						aaa_send_login_response(pinfo->user_info.username, 0,
							pinfo->user_id_buf, LOGIN_REPEATED, 0);
						aaa_user_log_add (aaa_copy_authing_to_log (pinfo, LOGIN_REPEATED));
						aaa_user_authing_del (pinfo->user_id);
						goto out;
					}
					aaa_method_none_process_check(pinfo);
					break;

				default:break;
			}
		}
		aaa_handle_auth_result(pinfo);
	}

out:
	//thread_add_timer_msec(aaa_master, aaa_auth_process_check, NULL, AAA_AUTH_PROCESS_CHECK_INTERVAL);
	return AAA_OK;
}


/* 在线用户状态定时检测，包括计费报文的定时发送、保活状态的检测等 */
int aaa_online_user_status_check(void *para)
{
	USER_ONLINE_INFO *pinfo = NULL;	
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		if(NULL == (pinfo = (USER_ONLINE_INFO *)(node->data)))
			return AAA_ERROR;

		if(0 == pinfo->alive_time)		//超时未收到用户的保活消息
		{
			if (AAA_FAIL == aaa_send_force_exit_request (pinfo->user_info.username, pinfo->user_id,
												EXIT_ECHO_TIMEOUT))
			{
				aaa_user_log_add (aaa_copy_online_to_log(pinfo));
				aaa_user_online_del (pinfo->user_id);
				zlog_err ("%s[%d] : vty force-exit user error\n", __func__, __LINE__);
			}
			else
			{
				aaa_user_log_add (aaa_copy_online_to_log(pinfo));
				aaa_user_online_del (pinfo->user_id);
			}
			continue;
		}
		else
			pinfo->alive_time--;

		/* 计费失败禁止在线时，计费出现失败，强制用户下线 */
		
		/* 定时发送计费报文 */		
		if(METHOD_RADIUS == pinfo->log_method)
		{
			if(DISABLE == g_aaa_cfgs.radius.acct_switch)
				continue;

			/* 计费失败在线禁能且计费失败次数达到5次，强制用户下线 */
			if ((pinfo->session_info.radius_user.acct_failed_cnt >= 5)	
				&& (DISABLE == aaa_online_acct_failed_get ()))
			{
				if (AAA_FAIL == aaa_send_force_exit_request (pinfo->user_info.username,
													pinfo->user_id, EXIT_ACCT_FAIL))
				{
					aaa_user_log_add (aaa_copy_online_to_log(pinfo));
					aaa_user_online_del (pinfo->user_id);
					zlog_err ("%s[%d] : vty force-exit user error\n", __func__, __LINE__);
				}
				else
				{
					aaa_user_log_add (aaa_copy_online_to_log(pinfo));
					aaa_user_online_del (pinfo->user_id);
				}
			}
			else
			{
				if (FALSE == pinfo->session_info.radius_user.acct_send_flag)	//正常收到计费应答
				{
					if (1 == pinfo->session_info.radius_user.acct_timeleft_retry)
					{
						radius_acct_request_send (&pinfo->session_info.radius_user.acct_pkt, PW_STATUS_ALIVE);
						pinfo->session_info.radius_user.acct_send_flag = TRUE;
					}
					
					pinfo->session_info.radius_user.acct_timeleft_retry--;
					if (0 == pinfo->session_info.radius_user.acct_timeleft_retry)
						pinfo->session_info.radius_user.acct_timeleft_retry = radius_acct_update_interval_get ();
				}
				else
				{
					if (1 == pinfo->session_info.radius_user.acct_timeleft_response)
					{
						radius_acct_request_send (&pinfo->session_info.radius_user.acct_pkt, PW_STATUS_ALIVE);
					}

					pinfo->session_info.radius_user.acct_timeleft_response--;
					if (0 == pinfo->session_info.radius_user.acct_timeleft_response)
					{
						pinfo->session_info.radius_user.acct_timeleft_response = RADIUS_DEF_ACCT_TIMEOUT;
						pinfo->session_info.radius_user.acct_failed_cnt++;
						zlog_debug(AAA_DBG_RADIUS, "%s[%d] : radius account response timeout %d, user[%s][%x]\n",
							__func__, __LINE__, pinfo->session_info.radius_user.acct_failed_cnt,
							pinfo->user_info.username, pinfo->user_id);
					}
				}
			}				
		}
		else if(METHOD_TACACS == pinfo->log_method)
		{
			if(DISABLE == g_aaa_cfgs.tac_plus.acct_switch)
				continue;

			if ((TRUE == pinfo->session_info.tacacs_user.acct_failed_flag)	/* 计费失败且失败禁止在线 */
				&& (DISABLE == aaa_online_acct_failed_get ()))
			{
				if (AAA_FAIL == aaa_send_force_exit_request (pinfo->user_info.username, 
															pinfo->user_id, EXIT_ACCT_FAIL))
				{
					aaa_copy_online_to_log (pinfo);
					aaa_user_online_del (pinfo->user_id);
					zlog_err ("%s[%d] : vty force-exit user error\n", __func__, __LINE__);
				}
				else
				{
					aaa_user_log_add (aaa_copy_online_to_log(pinfo));
					aaa_user_online_del (pinfo->user_id);
				}
			}
			else
			{
				if (1 == pinfo->session_info.tacacs_user.acct_update_time)
				{
					tac_acct_pkt_send (&pinfo->session_info.tacacs_user);
					pinfo->session_info.tacacs_user.acct_timeleft_response = TAC_PLUS_DEF_ACCT_TIMEOUT;
					pinfo->session_info.tacacs_user.acct_send_flag = TRUE;
					zlog_debug(AAA_DBG_TACACS, "%s[%d] : tacacs account send\n", __func__, __LINE__);
				}
				
				pinfo->session_info.tacacs_user.acct_update_time--;
				if (0 == pinfo->session_info.tacacs_user.acct_update_time)
					pinfo->session_info.tacacs_user.acct_update_time = tac_plus_acct_update_interval_get();

				if (TRUE == pinfo->session_info.tacacs_user.acct_send_flag)
				{					
					pinfo->session_info.tacacs_user.acct_timeleft_response--;
					if (0 == pinfo->session_info.tacacs_user.acct_timeleft_response)
						pinfo->session_info.tacacs_user.acct_failed_flag = TRUE;
					
					zlog_debug(AAA_DBG_TACACS, "%s[%d] : tacacs account response timeout left %ds, user[%s][%x]\n",
						__func__, __LINE__,
						(int)pinfo->session_info.tacacs_user.acct_timeleft_response,
						pinfo->user_info.username, pinfo->user_id);
				}					
			}				
		}
	}

	//thread_add_timer_msec(aaa_master, aaa_online_user_status_check, NULL, AAA_USER_ONLINE_CHECK_INTERVAL);
	return AAA_OK;
}


/* radius认证流程处理 */
void aaa_method_radius_process_check(USER_AUTHING_INFO *pinfo)
{
	/* 因process-check 间隔时间较短，而重发radius报文间隔时间较长，用该计数器控制radius重发间隔 */
	static uint32_t auth_timer = 0;
	static uint32_t acct_timer = 0;
	
	switch(pinfo->aaa_result)
	{
		case AUTHEN_INVALID:
		case AUTHEN_NO_SERVER:
		case AUTHEN_NO_USER:
			if (0 == pinfo->session_info.radius_user.auth_retry_times)
			{
				pinfo->aaa_result = AUTHEN_NO_SERVER;	//超时了，找不到服务器
				pinfo->auth_order++;					//切换到下一顺序的认证模式
				if (pinfo->auth_order > 4)				//已经是最后的认证模式
				{
					pinfo->aaa_result = AUTHEN_FAILED;
					pinfo->auth_finished = TRUE;
				}
				auth_timer = 0;
			}
			else
			{
				auth_timer++;
				if (auth_timer >= radius_retry_interval_get ()/AAA_AUTH_PROCESS_CHECK_INTERVAL)
				{
					radius_auth_request_send (&pinfo->session_info.radius_user.auth_pkt);
					pinfo->session_info.radius_user.auth_retry_times--;
					auth_timer = 0;
				}
			}
			break;

		case AUTHEN_FAILED:
			pinfo->auth_finished = TRUE;
			auth_timer = 0;
			break;

		case AUTHEN_SUCCESS:
			auth_timer = 0;
			if(ENABLE == g_aaa_cfgs.radius.acct_switch)
			{
				if(0 == pinfo->session_info.radius_user.acct_retry_times)
				{
					pinfo->aaa_result = ACCT_NO_SERVER;
					pinfo->auth_finished = TRUE;
				}
				else
				{
					acct_timer++;
					if(acct_timer >= radius_retry_interval_get ()/AAA_AUTH_PROCESS_CHECK_INTERVAL)
					{
						radius_acct_request_send(&pinfo->session_info.radius_user.acct_pkt, PW_STATUS_START);
						pinfo->session_info.radius_user.acct_retry_times--;
						acct_timer = 0;
					}
				}					
			}
			else
			{
				/* 计费禁能，说明允许计费失败在线 */
				if(DISABLE == g_aaa_cfgs.online_acct_failed)	/* 检测到冲突，配置出现错误 */
				{	
					pinfo->aaa_result = AUTHEN_NO_SERVER;	//超时了，找不到服务器
					pinfo->auth_order++;					//切换到下一顺序的认证模式
					if(pinfo->auth_order > 4)				//已经是最后的认证模式
					{
						pinfo->aaa_result = AUTHEN_FAILED;
						pinfo->auth_finished = TRUE;
					}
				}
				else
				{
					pinfo->aaa_result = ACCT_FAILED;
					pinfo->auth_finished = TRUE;
				}
			}
			break;

		case AUTHOR_NO_SERVER:
		case AUTHOR_FAILED:
		case AUTHOR_SUCCESS:	
			break;

		case ACCT_NO_SERVER:
		case ACCT_FAILED:
		case ACCT_SUCCESS:
			acct_timer = 0;
			pinfo->auth_finished = TRUE;
			break;

		default:
			break;
	}
}


/* tacacs认证流程处理 */
void aaa_method_tacacs_process_check(USER_AUTHING_INFO *pinfo)
{
	static int authen_cnt = 0;
	static int author_cnt = 0;
	static int acct_cnt = 0;
	
	switch(pinfo->aaa_result)
	{
		case AUTHEN_INVALID:
		case AUTHEN_NO_SERVER:
		case AUTHEN_NO_USER:
			if(0 == authen_cnt)
			{
				tac_authen_pkt_send (&pinfo->session_info.tacacs_user);
				authen_cnt++;
			}				
			
			if(0 == pinfo->session_info.tacacs_user.authen_timeleft)
			{
				pinfo->aaa_result = AUTHEN_NO_SERVER;	//超时了，找不到服务器
				pinfo->auth_order++;					//切换到下一顺序的认证模式
				if(pinfo->auth_order > 4)				//已经是最后的认证模式
				{
					pinfo->aaa_result = AUTHEN_FAILED;
					pinfo->auth_finished = TRUE;
				}
				authen_cnt = 0;
			}
			else
			{
				pinfo->session_info.tacacs_user.authen_timeleft--;			
			}
			break;

		case AUTHEN_FAILED:
			pinfo->auth_finished = TRUE;
			break;

		case AUTHEN_SUCCESS:
			if(ENABLE == g_aaa_cfgs.tac_plus.author_switch)
			{
				if(0 == author_cnt)
				{
					tac_author_pkt_send(&pinfo->session_info.tacacs_user);
					author_cnt++;
				}
				
				if(0 == pinfo->session_info.tacacs_user.author_timeleft)
				{
					pinfo->aaa_result = AUTHOR_NO_SERVER;
					if(DISABLE == g_aaa_cfgs.online_author_failed)	//授权失败禁止上线
					{						
						pinfo->auth_finished = TRUE;
					}
				}
				else
				{
					pinfo->session_info.tacacs_user.author_timeleft--;			
				}
			}
			else
			{
				/* 计费禁能，说明允许计费失败在线 */
				if(DISABLE == g_aaa_cfgs.online_author_failed)	/* 检测到冲突，出现配置错误 */
				{
					pinfo->aaa_result = AUTHEN_NO_SERVER;	//定义为找不到服务器，防止无法登陆设备
					pinfo->auth_order++;					//切换到下一顺序的认证模式
					if(pinfo->auth_order > 4)				//已经是最后的认证模式
					{
						pinfo->aaa_result = AUTHEN_FAILED;
						pinfo->auth_finished = TRUE;
					}
				}
				else
				{
					if(ENABLE == g_aaa_cfgs.tac_plus.acct_switch)
					{
						if(0 == acct_cnt)
						{
							tac_acct_pkt_send(&pinfo->session_info.tacacs_user);
							acct_cnt++;
						}

						if(0 == pinfo->session_info.tacacs_user.acct_timeleft)
						{							
							pinfo->aaa_result = ACCT_NO_SERVER;
							pinfo->auth_finished = TRUE;
						}
						else
						{
							pinfo->session_info.tacacs_user.acct_timeleft--;			
						}
					}
					else
					{
						if(DISABLE == g_aaa_cfgs.online_acct_failed)	/* 检测到冲突，出现配置错误 */
						{
							pinfo->aaa_result = AUTHEN_NO_SERVER;	//定义为找不到服务器
							pinfo->auth_order++;					//切换到下一顺序的认证模式							
							if(pinfo->auth_order > 4)				//已经是最后的认证模式
							{
								pinfo->aaa_result = AUTHEN_FAILED;
								pinfo->auth_finished = TRUE;
							}
						}
						else
						{
							pinfo->auth_finished = TRUE;
						}					
					}
				}									
			}
			break;

		case AUTHOR_NO_SERVER:
		case AUTHOR_FAILED:
			if(ENABLE == g_aaa_cfgs.online_author_failed)	//允许授权失败在线
			{
				if(ENABLE == g_aaa_cfgs.tac_plus.acct_switch)	//继续计费
				{
					if(0 == acct_cnt)
					{
						tac_acct_pkt_send(&pinfo->session_info.tacacs_user);
						acct_cnt++;
					}
					
					if(0 == pinfo->session_info.tacacs_user.acct_timeleft)
					{							
						pinfo->aaa_result = ACCT_NO_SERVER;
						pinfo->auth_finished = TRUE;
					}
					else
					{
						pinfo->session_info.tacacs_user.acct_timeleft--;			
					}
				}
				else
				{
					if(DISABLE == g_aaa_cfgs.online_acct_failed)	/* 检测到冲突，出现配置错误 */
					{
						pinfo->aaa_result = AUTHEN_NO_SERVER;	//定义为找不到服务器
						pinfo->auth_order++;					//切换到下一顺序的认证模式
						if(pinfo->auth_order > 4)				//已经是最后的认证模式
						{
							pinfo->aaa_result = AUTHEN_FAILED;
							pinfo->auth_finished = TRUE;
						}
					}
					else
					{
						pinfo->aaa_result = ACCT_FAILED;
						pinfo->auth_finished = TRUE;
					}
				}
			}
			else
			{
				pinfo->auth_finished = TRUE;
			}
			break;

		
		case AUTHOR_SUCCESS:
			if(ENABLE == g_aaa_cfgs.tac_plus.acct_switch)	//继续计费
			{
				if(0 == acct_cnt)
				{
					tac_acct_pkt_send(&pinfo->session_info.tacacs_user);
					acct_cnt++;
				}

				if(0 == pinfo->session_info.tacacs_user.acct_timeleft)
				{
					pinfo->aaa_result = ACCT_NO_SERVER;
					pinfo->auth_finished = TRUE;
				}
				else
				{
					pinfo->session_info.tacacs_user.acct_timeleft--;			
				}
			}
			else
			{
				if(DISABLE == g_aaa_cfgs.online_acct_failed)	/* 检测到冲突，出现配置错误 */
				{
					pinfo->aaa_result = AUTHEN_NO_SERVER;	//定义为找不到服务器
					pinfo->auth_order++;					//切换到下一顺序的认证模式
					if(pinfo->auth_order > 4)				//已经是最后的认证模式
					{
						pinfo->aaa_result = AUTHEN_FAILED;
						pinfo->auth_finished = TRUE;
					}					
				}
				else
				{
					pinfo->aaa_result = ACCT_FAILED;
					pinfo->auth_finished = TRUE;
				}
			}
			break;
			
		case ACCT_NO_SERVER:
		case ACCT_FAILED:
		case ACCT_SUCCESS:
			pinfo->auth_finished = TRUE;
			break;

		default:
			break;
	}
	
	if(TRUE == pinfo->auth_finished)	//计数器是为了保证认证/授权/计费报文只发送一次，因为使用tcp，不能重复发
	{
		if(authen_cnt) authen_cnt = 0;
		if(author_cnt) author_cnt = 0;
		if(acct_cnt) acct_cnt = 0;
	}
}


/* local认证流程处理 */
void aaa_method_local_process_check(USER_AUTHING_INFO *pinfo)
{	
	if((AUTHEN_INVALID == pinfo->aaa_result) || 
		(AUTHEN_NO_SERVER == pinfo->aaa_result) || 
		(AUTHEN_NO_USER == pinfo->aaa_result))
	{
		USER_CONFIG_INFO *user_config = aaa_user_config_get_by_name (pinfo->user_info.username);
		
		if(NULL == user_config)
		{
			pinfo->aaa_result = AUTHEN_NO_USER;
			pinfo->auth_order++;					//切换到下一顺序的认证模式
			if(pinfo->auth_order > 4)				//已经是最后的认证模式
			{
				pinfo->aaa_result = AUTHEN_FAILED;
				pinfo->auth_finished = TRUE;
			}
		}
		else
		{
			zlog_debug(AAA_DBG_LOCAL, "%s[%d] : aaa find local-user[%s][%s] <---> login-user[%s][%s]\n",
				__func__, __LINE__,
				user_config->local_auth_user.username,
				user_config->local_auth_user.password,
				pinfo->user_info.username,
				pinfo->user_info.password_init);

			if(FALSE == aaa_check_local_user_service(pinfo->user_info.ttyname, user_config->local_auth_user.service))
			{
				pinfo->aaa_result = AUTHEN_FAILED;
				pinfo->auth_finished = TRUE;
			}

			if(strlen(user_config->local_auth_user.password))
			{
				if(0 == strcmp(user_config->local_auth_user.password, pinfo->user_info.password_init))
				{
					pinfo->user_info.level_author = user_config->local_auth_user.level;
					pinfo->aaa_result = AUTHEN_SUCCESS;
					pinfo->auth_finished = TRUE;
				}
				else
				{
					pinfo->aaa_result = AUTHEN_FAILED;
					pinfo->auth_finished = TRUE;
				}
			}
			else
			{
				pinfo->user_info.level_author = user_config->local_auth_user.level;
				pinfo->aaa_result = AUTHEN_SUCCESS;
				pinfo->auth_finished = TRUE;
			}
		}
	}								
}


/* none认证流程处理 */
void aaa_method_none_process_check(USER_AUTHING_INFO *pinfo)
{
	pinfo->user_info.level_author = aaa_none_user_def_priv_get();
	pinfo->aaa_result = AUTHEN_SUCCESS;
	pinfo->auth_finished = TRUE;
}


/* 确认本地用户服务类型是否支持 */
int aaa_check_local_user_service(char *tty, int service_type)
{
	if(((0 == memcmp("console", tty, 3)) && (service_type & SERVICE_TYPE_TERMINAL))
		|| ((0 == memcmp("telnet", tty, 3)) && (service_type & SERVICE_TYPE_TELNET)) 
		|| ((0 == memcmp("ssh", tty, 3)) && (service_type & SERVICE_TYPE_SSH)))
	{
		return TRUE;
	}

	return FALSE;
}


/* 处理认证结果 */
void aaa_handle_auth_result(USER_AUTHING_INFO *pinfo)
{
	USER_ONLINE_INFO *pinfo_online = NULL;
	
	if(TRUE == pinfo->auth_finished)	//认证已完成
	{
		switch(pinfo->aaa_result)
		{
			case AUTHEN_INVALID:
			case AUTHEN_NO_SERVER:
			case AUTHEN_NO_USER:
			case AUTHEN_FAILED:
				goto send_failed_msg;
				break;
				
			case AUTHOR_NO_SERVER:				
			case AUTHOR_FAILED:
				if(ENABLE == g_aaa_cfgs.online_author_failed)
					goto send_success_msg;
				else
					goto send_failed_msg;
				break;
			
			case ACCT_NO_SERVER:
			case ACCT_FAILED:
				if(ENABLE == g_aaa_cfgs.online_acct_failed)
					goto send_success_msg;
				else
					goto send_failed_msg;
				break;
				
			case AUTHEN_SUCCESS:
			case AUTHOR_SUCCESS:
			case ACCT_SUCCESS:
				goto send_success_msg;
				break;
				
			default:break;
		}

send_success_msg:
		pinfo_online = aaa_copy_authing_to_online (pinfo);

		if(ENABLE == aaa_console_admin_mode_get ())		//串口管理员模式使能
		{
			pinfo_online->user_info.level_author = pinfo->user_info.level_author;
			if(0 == strcmp (pinfo_online->user_info.ttyname, "console"))
			{
				if(pinfo_online->user_info.level_author < 14)
					pinfo_online->user_info.level_author = 14;
			}
		}
		else
		{
			pinfo_online->user_info.level_author = pinfo->user_info.level_author;
		}

		pinfo_online->user_info.level_current = (pinfo_online->user_info.level_author < aaa_user_def_priv_get())?
				pinfo_online->user_info.level_author:aaa_user_def_priv_get();

		/* 为方便其他人使用，暂时直接给用户授权权限，当vty做好enable之后，再去掉该行 */
		pinfo_online->user_info.level_current = pinfo_online->user_info.level_author;

		if (AAA_OK == aaa_send_login_response (pinfo_online->user_info.username, pinfo_online->user_id,
			pinfo->user_id_buf, LOGIN_SUCCESS, pinfo_online->user_info.level_current))
		{
			aaa_user_online_add (pinfo_online);
		}
		else
		{
			zlog_debug(AAA_DBG_COMMON, "%s[%d] : login-response verify msg err\n", __func__, __LINE__);
		}
		aaa_user_authing_del (pinfo->user_id);
		return;

send_failed_msg:
		aaa_send_login_response (pinfo->user_info.username, pinfo->user_id,
			pinfo->user_id_buf, LOGIN_FAILED, 0);
		aaa_user_log_add (aaa_copy_authing_to_log(pinfo, LOGIN_FAILED));
		aaa_user_authing_del (pinfo->user_id);
		return;
	}
}


/* 检查用户重复登录情况，local/none允许重复登录，radius/tacacs+不允许重复登录 
 * TRUE ：表示重复登录或已达到最大重复登录次数
 * FALSE：表示未重复登录
 */

int aaa_user_login_repeat(const char *username, LOG_METHOD login_method)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;	
	USER_CONFIG_INFO *pinfo_config = NULL;	
	int repeat_num = 0;

	if (METHOD_RADIUS == login_method)
	{
		for ((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
		{
			pinfo = (USER_ONLINE_INFO *)(node->data);
			if(pinfo == NULL)
				return FALSE;

			if (0 == strcmp(pinfo->user_info.username, username))
			{
				if (METHOD_RADIUS == pinfo->log_method)
					return TRUE;
			}
		}
	}
	else if (METHOD_TACACS == login_method)
	{
		for ((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
		{
			pinfo = (USER_ONLINE_INFO *)(node->data);
			if(pinfo == NULL)
				return FALSE;

			if (0 == strcmp(pinfo->user_info.username, username))
			{
				if (METHOD_TACACS == pinfo->log_method)
					return TRUE;
			}
		}
	}
	else if (METHOD_LOCAL == login_method)
	{
		if (NULL == (pinfo_config = aaa_user_config_get_by_name (username)))
			return FALSE;
		
		for ((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
		{
			pinfo = (USER_ONLINE_INFO *)(node->data);
			if(pinfo == NULL)
				return FALSE;

			if (0 == strcmp(pinfo->user_info.username, username))
			{
				if (METHOD_LOCAL == pinfo->log_method)
					repeat_num++;
			}
		}
		if ((repeat_num + 1) > pinfo_config->local_auth_user.max_repeat_num)
			return TRUE;
		else	
			return FALSE;
	}

	return FALSE;
}


/* 向vty发送 max_idle_time */
int aaa_send_max_idle_time_msg(time_t time)
{
	AAA_MAX_IDLE_MSG msg;
	memset(&msg, 0, sizeof(AAA_MAX_IDLE_MSG));

	msg.idle_time = time;

	if (ipc_send_msg_n2(&msg, sizeof (AAA_MAX_IDLE_MSG), 1, MODULE_ID_VTY,
		MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_MAX_IDLE_TIME, 0, 0) == -1)
	{
		zlog_debug ("%s[%d] : aaa send max-idle-time to vty failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}

	return AAA_OK;
}


/* 向vty发送应答确认消息 */
void aaa_send_verify_msg (struct ipc_mesg_n *pmsg_ipc, uint32_t user_id, 
					uint32_t user_id_buf, char *username, AAA_MSG_TYPE msg_type)
{
	AAA_VERIFY_RESPONSE msg;
	memset (&msg, 0, sizeof (AAA_VERIFY_RESPONSE));

	msg.user_id = user_id;
	msg.user_id_buf = user_id_buf;
	msg.msg_type = msg_type;
	if (NULL != username)
		memcpy (msg.username, username, strlen (username));

	zlog_debug(AAA_DBG_COMMON, "%s[%d] : send verify msg to vty, id[%x], username[%s], msg_type[%d]\n",
		__func__, __LINE__,	msg.user_id, msg.username, msg.msg_type);
	
	/*ipc_send_reply (&msg, sizeof (AAA_VERIFY_RESPONSE),
		pmsg_ipc->msghdr.sender_id, pmsg_ipc->msghdr.module_id,
		pmsg_ipc->msghdr.msg_type, pmsg_ipc->msghdr.msg_subtype, 0);*/

	ipc_send_reply_n2(&msg, sizeof (AAA_VERIFY_RESPONSE), 1,
		pmsg_ipc->msghdr.sender_id, pmsg_ipc->msghdr.module_id,
		pmsg_ipc->msghdr.msg_type, pmsg_ipc->msghdr.msg_subtype, 0, 0, IPC_OPCODE_REPLY);
}


void aaa_send_verify_msg_noack (struct ipc_mesg_n *pmsg_ipc)
{
	/*ipc_send_noack (0, pmsg_ipc->msghdr.sender_id, pmsg_ipc->msghdr.module_id,
		pmsg_ipc->msghdr.msg_type, pmsg_ipc->msghdr.msg_subtype, 0);*/

	ipc_send_reply_n2(NULL, 0, 0, pmsg_ipc->msghdr.sender_id, pmsg_ipc->msghdr.module_id,
		pmsg_ipc->msghdr.msg_type, pmsg_ipc->msghdr.msg_subtype, 0, 0, IPC_OPCODE_NACK);
}



