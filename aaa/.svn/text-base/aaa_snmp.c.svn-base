/******************************************************************************
 * Filename: aaa_snmp.c
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

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/memory.h>

#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc_n.h>
#include <lib/snmp_common.h>

#include "aaa_config.h"
#include "radiusclient.h"
#include "aaa_radius.h"
#include "aaa_tac_plus.h"
#include "aaa_snmp.h"


/*local temp variable*/
#define	IPC_BUF_LEN  16000

//static int   	int_value = 0;
//static uint32_t ip_value = 0;

static uchar ipc_buf[IPC_BUF_LEN];



/* aaa config info get */
int aaa_snmp_config_info_get (struct ipc_msghdr_n *phdr)
{
	struct sockaddr_in server;

	if(SNMP_AAA_COMMON_CFG == phdr->msg_subtype)
	{
		struct aaa_snmp_common_cfg common_cfg;
		memset(&common_cfg, 0, sizeof(struct aaa_snmp_common_cfg));

		common_cfg.login_user_idle_cut = aaa_user_max_idle_time_get()/60;
		common_cfg.login_user_max_num = aaa_login_max_num_get();
		common_cfg.login_user_def_level = aaa_user_def_priv_get();

		if(ENABLE == aaa_online_author_failed_get())
		{
			common_cfg.author_failed_online = 1;
		}
		else
		{
			common_cfg.author_failed_online = 2;
		}

		if(ENABLE == aaa_online_acct_failed_get())
		{
			common_cfg.acct_failed_online = 1;
		}
		else
		{
			common_cfg.acct_failed_online = 2;
		}

		if(ENABLE == aaa_console_admin_mode_get())
		{
			common_cfg.console_admin_mode = 1;
		}
		else
		{
			common_cfg.console_admin_mode = 2;
		}
		
		common_cfg.login_mode_first_pri = aaa_get_auth_order(1);
		common_cfg.login_mode_second_pri = aaa_get_auth_order(2);
		common_cfg.login_mode_third_pri = aaa_get_auth_order(3);
		common_cfg.login_mode_forth_pri = aaa_get_auth_order(4);
		common_cfg.none_user_level = aaa_none_user_def_priv_get();

		/*if(ipc_send_reply(&common_cfg, sizeof(common_cfg), phdr->sender_id,
			phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmp failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/
		
		ipc_ack_to_snmp(phdr, (void *)&common_cfg, sizeof(common_cfg), 1);
	}
	else if(SNMP_AAA_RADIUS_CFG == phdr->msg_subtype)
	{		
		struct aaa_snmp_radius_cfg radius_cfg;
		memset(&radius_cfg, 0, sizeof(struct aaa_snmp_radius_cfg));
		
		radius_auth_server_get(&server);
		radius_cfg.auth_server_ip = ntohl (server.sin_addr.s_addr);
		radius_cfg.auth_server_l3vpn = (int)radius_auth_server_vpn_get();
		
		radius_acct_server_get(&server);
		radius_cfg.acct_server_ip = ntohl (server.sin_addr.s_addr);
		radius_cfg.acct_server_l3vpn = (int)radius_acct_server_vpn_get();
		radius_cfg.acct_switch = radius_acct_server_switch_get();
		radius_cfg.acct_update_interval = radius_acct_update_interval_get();
		
		strcpy(radius_cfg.key, radius_key_get());
		
		radius_cfg.retry_times = radius_retry_times_get();
		radius_cfg.retry_interval = radius_retry_interval_get();
		radius_cfg.rsp_timeout = radius_response_timeout_get();
		
		switch(radius_authen_mode_get())
		{
			case MODE_PAP:
				radius_cfg.auth_mode = 1;
				break;
			case MODE_CHAP:
				radius_cfg.auth_mode = 2;
				break;
			default:
				break;
		}

		/*if(ipc_send_reply(&radius_cfg, sizeof(radius_cfg), phdr->sender_id,
			phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmp failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_ack_to_snmp(phdr, (void *)&radius_cfg, sizeof(radius_cfg), 1);
	}
	else if(SNMP_AAA_TAC_CFG == phdr->msg_subtype)
	{
		struct aaa_snmp_tacacs_cfg tac_cfg;
		memset(&tac_cfg, 0, sizeof(struct aaa_snmp_tacacs_cfg));

		tac_plus_authen_server_get(&server);
		tac_cfg.authen_server_ip = ntohl (server.sin_addr.s_addr);
		tac_cfg.authen_server_l3vpn = (int)tac_plus_authen_server_vpn_get();

		tac_plus_author_server_get(&server);
		tac_cfg.author_server_ip = ntohl (server.sin_addr.s_addr);
		tac_cfg.author_server_l3vpn = (int)tac_plus_author_server_vpn_get();

		if(ENABLE == tac_plus_author_server_switch_get())
		{
			tac_cfg.author_switch = 1;
		}
		else
		{
			tac_cfg.author_switch = 2;
		}

		tac_plus_acct_server_get(&server);
		tac_cfg.acct_server_ip = ntohl (server.sin_addr.s_addr);
		tac_cfg.acct_server_l3vpn = (int)tac_plus_acct_server_vpn_get();

		if(ENABLE == tac_plus_acct_server_switch_get())
		{
			tac_cfg.acct_switch = 1;
		}
		else
		{
			tac_cfg.acct_switch = 2;
		}
		
		tac_cfg.acct_update_interval = (int)tac_plus_acct_update_interval_get();
		strcpy(tac_cfg.key, tac_plus_key_get());
		tac_cfg.rsp_timeout = (int)tac_plus_response_timeout_get();
		
		/*if(ipc_send_reply(&tac_cfg, sizeof(tac_cfg), phdr->sender_id,
			phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmp failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/
		
		ipc_ack_to_snmp(phdr, (void *)&tac_cfg, sizeof(tac_cfg), 1);
	}
	else if(SNMP_AAA_ENABLE_CFG == phdr->msg_subtype)
	{
		/*if(ipc_send_reply(&g_enable, sizeof(ENABLE_INFO), phdr->sender_id,
			phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmp failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/
		
		ipc_ack_to_snmp(phdr, (void *)&g_enable, sizeof(ENABLE_INFO), 1);
	}

	return AAA_OK;
}



/* 获取 table 数据 */
void aaa_snmp_table_info_get (struct ipc_mesg_n *pmsg)
{
	switch (pmsg->msghdr.msg_subtype)
	{
		case SNMP_USER_CONFIG_TABLE:
			aaa_snmp_user_config_get_bulk (&pmsg->msghdr, (SNMP_USER_CONFIG_INDEX *)pmsg->msg_data);
			break;

		case SNMP_USER_ONLINE_TABLE:
			aaa_snmp_user_online_get_bulk (&pmsg->msghdr, (SNMP_USER_ONLINE_INDEX *)pmsg->msg_data);
			break;

		case SNMP_USER_LOG_TABLE:
			aaa_snmp_user_log_get_bulk (&pmsg->msghdr, (SNMP_USER_LOG_INDEX *)pmsg->msg_data);
			break;

		default:
			break;
	}
}



/* 已配置用户表应答 */
int aaa_snmp_user_config_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_CONFIG_INDEX *index)
{
	//uchar ipc_buf[IPC_MSG_LEN];
	uchar *pbuf = ipc_buf;
	memset (ipc_buf, 0, IPC_MSG_LEN);
	
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_CONFIG_INFO *pinfo = NULL;
	
	int cnt_info = 0; //to remember number of sessions
	int cnt_info_max = IPC_MSG_LEN/sizeof (SNMP_USER_CONFIG_INFO);

	/* to cache user config info */
	SNMP_USER_CONFIG_INFO user_info;


	/* flag to remember find session result, 0:not found; 1:find; 2:add next of result */
	int flag_user_found = 0;

	/* get radius session */
	for((node) = hios_hash_start(&g_user_table.user_config_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_config_table, &cursor))		
	{	
		if(NULL == (pinfo = (USER_CONFIG_INFO *)(node->data)))
			return AAA_OK;		

		if (0 == strcmp (pinfo->local_auth_user.username, SUPER_USER_NAME))
			continue;
		
		if(0 != index->user_id)
		{
			if(0 == flag_user_found)	//not find
			{
				if(index->user_id == pinfo->user_id)
				{
					flag_user_found = 1;
					continue;
				}	
			}
		}
		else
		{
			flag_user_found = 1;
		}

		if(flag_user_found)
		{
			cnt_info++;
			user_info.index.user_id = pinfo->user_id;
			strcpy(user_info.data.username, pinfo->local_auth_user.username);
			strcpy(user_info.data.password, pinfo->local_auth_user.password);
			user_info.data.level = pinfo->local_auth_user.level;
			user_info.data.max_repeat_num = pinfo->local_auth_user.max_repeat_num;

			memcpy(pbuf, &user_info, sizeof(SNMP_USER_CONFIG_INFO));
			pbuf += sizeof(SNMP_USER_CONFIG_INFO);
		}

		if(cnt_info >= cnt_info_max)
			break;
	}

	if(cnt_info)
	{
		/*if(ipc_send_reply_bulk(ipc_buf, cnt_info*sizeof(SNMP_USER_CONFIG_INFO),
			cnt_info, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, cnt_info*sizeof(SNMP_USER_CONFIG_INFO), cnt_info);
	}
	else
	{
		/*if(ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_noack_to_snmp(phdr);
	}

	return AAA_OK;
}



/* 在线用户表应答 */
int aaa_snmp_user_online_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_ONLINE_INDEX *index)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	time_t time_now;

	int cnt_info = 0; 	//记录以找到的在线用户数目
	int cnt_info_max = IPC_MSG_LEN / sizeof (SNMP_USER_ONLINE_INFO);

	/* pkt to store sessin info and send to snmpd */
	//uchar 	ipc_buf[IPC_MSG_LEN];
	size_t 	ipc_buf_len = 0;
	memset (ipc_buf, 0, IPC_MSG_LEN);

	/* flag to remember find session result, 0:not found; 1:find; 2:add next of result */
	int flag_user_found = 0;

	SNMP_USER_ONLINE_INFO info_found;

	/* get radius session */
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{	
		if(NULL == (pinfo = (USER_ONLINE_INFO *)(node->data)))
			return AAA_ERROR;		
				
		if(0 != index->user_id)
		{
			if(0 == flag_user_found)	//not find
			{
 				if(index->user_id == pinfo->user_id)
				{
					flag_user_found = 1;
					continue;
				}	
			}
		}
		else
		{
			flag_user_found = 1;
		}

		if(flag_user_found)
		{
			memset (&info_found, 0, sizeof (SNMP_USER_ONLINE_INFO));
			info_found.index.user_id = pinfo->user_id;
			memcpy (info_found.data.username, pinfo->user_info.username,
				strlen (pinfo->user_info.username));
			info_found.data.level_author = pinfo->user_info.level_author;
			info_found.data.level_current = pinfo->user_info.level_current;
			info_found.data.login_method = pinfo->log_method;
			memcpy (info_found.data.login_from, pinfo->user_info.ttyname,
				strlen (pinfo->user_info.ttyname));
			info_found.data.time_login = pinfo->user_info.time_start;
			
			time(&time_now);
			info_found.data.time_online = time_now - pinfo->user_info.time_start;

			memcpy(ipc_buf + ipc_buf_len, &info_found, sizeof (SNMP_USER_ONLINE_INFO));
			ipc_buf_len += sizeof (SNMP_USER_ONLINE_INFO);

			cnt_info++;
		}

		if(cnt_info >= cnt_info_max)
			break;
	}

	if (cnt_info)
	{
		/*if(ipc_send_reply_bulk (ipc_buf, ipc_buf_len,
			cnt_info, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/		

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, ipc_buf_len, cnt_info);
	}
	else
	{
		/*if (ipc_send_noack (0, phdr->sender_id, phdr->module_id, phdr->msg_type,
						phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_noack_to_snmp(phdr);
	}
	return AAA_OK;
}


/* 历史用户表应答 */
int aaa_snmp_user_log_get_bulk (struct ipc_msghdr_n *phdr, SNMP_USER_LOG_INDEX *index)
{
	//uchar ipc_buf[IPC_MSG_LEN];
	uchar *pbuf = ipc_buf;
	memset (ipc_buf, 0, IPC_MSG_LEN);
	
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_LOG_INFO *pinfo = NULL;

	int cnt_info = 0; //to remember number of sessions
	int cnt_info_max = IPC_MSG_LEN/sizeof(SNMP_USER_LOG_INFO);

	/* to cache log info */
	SNMP_USER_LOG_INFO log_info;

	/* flag to remember find session result, 0:not found; 1:find; 2:add next of result */
	int flag_user_found = 0;

	/* get radius session */
	for((node) = hios_hash_start(&g_user_table.user_log_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_log_table, &cursor))		
	{	
		if(NULL == (pinfo = (USER_LOG_INFO *)(node->data)))
		{
			break;		
		}
				
		if(0 != index->user_id)
		{
			if(0 == flag_user_found)	//not find
			{
				if(index->user_id == pinfo->user_id)
				{
					flag_user_found = 1;
					continue;
				}	
			}
		}
		else
		{
			flag_user_found = 1;
		}

		if(flag_user_found)
		{
			cnt_info++;
			log_info.index.user_id = pinfo->user_id;
			strcpy(log_info.data.username, pinfo->user_info.username);
			log_info.data.level = pinfo->user_info.level_author;
			log_info.data.login_method = pinfo->log_method;
			log_info.data.login_result = pinfo->log_result;
			strcpy(log_info.data.login_from, pinfo->user_info.ttyname);
			log_info.data.time_start = pinfo->user_info.time_start;
			log_info.data.time_exit = pinfo->user_info.time_exit;

			memcpy(pbuf, &log_info, sizeof(SNMP_USER_LOG_INFO));
			pbuf += sizeof(SNMP_USER_LOG_INFO);
		}

		if(cnt_info >= cnt_info_max)
			break;
	}

	if(cnt_info)
	{
		/*if(ipc_send_reply_bulk(ipc_buf, cnt_info*sizeof(SNMP_USER_LOG_INFO),
			cnt_info, phdr->sender_id, phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_ack_to_snmp(phdr, (void *)ipc_buf, cnt_info*sizeof(SNMP_USER_LOG_INFO), cnt_info);
	}
	else
	{
		/*if(ipc_send_noack(0, phdr->sender_id,
					phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index) < 0)
		{
			zlog_err("%s[%d] : aaa send config info to snmpd failed\n", __FUNCTION__, __LINE__);
			return AAA_IPC_FAIL;
		}*/

		ipc_noack_to_snmp(phdr);
	}

	return AAA_OK;
}



void aaa_handle_snmp_msg (struct ipc_mesg_n *pmsg)
{
	switch (pmsg->msghdr.opcode)
	{
		case IPC_OPCODE_GET:
			aaa_snmp_config_info_get (&pmsg->msghdr);
			break;

		case IPC_OPCODE_GET_BULK:
			aaa_snmp_table_info_get (pmsg);
			break;

		default:
			break;
	}
}



//------------------------- end -----------------------------//


