/******************************************************************************
 * Filename: aaa_tac_plus.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.9.22  luoyz created
 *
******************************************************************************/

#include <string.h>

#include <lib/pkt_buffer.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>

#include "aaa_config.h"
#include "aaa_tac_plus.h"
#include "aaa_packet.h"
#include "tacplus.h"


extern struct thread_master *aaa_master;


void aaa_tac_plus_init(void)
{
	g_aaa_cfgs.tac_plus.rsp_timeout = TAC_PLUS_RSP_TIMEOUT;
	
	g_aaa_cfgs.tac_plus.authen_server.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	g_aaa_cfgs.tac_plus.authen_server.sin_port = htons(TAC_PLUS_PORT);
	g_aaa_cfgs.tac_plus.author_server.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	g_aaa_cfgs.tac_plus.author_server.sin_port = htons(TAC_PLUS_PORT);
	g_aaa_cfgs.tac_plus.acct_server.sin_addr.s_addr = htonl(TAC_PLUS_DEF_SERV_IPV4);
	g_aaa_cfgs.tac_plus.acct_server.sin_port = htons(TAC_PLUS_PORT);

	g_aaa_cfgs.tac_plus.authen_mode = TAC_PLUS_AUTHEN_TYPE_CHAP;
	g_aaa_cfgs.tac_plus.author_switch = ENABLE;
	g_aaa_cfgs.tac_plus.acct_switch = ENABLE;

	g_aaa_cfgs.tac_plus.interval_for_acct_update = TAC_PLUS_DEF_UPDATE_INTERVAL;

	tac_plus_key_set(TAC_PLUS_DEF_SHARED_SECRET);
	aaa_tac_plus_pkt_register();
}

int tac_plus_authen_server_set(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;

	g_aaa_cfgs.tac_plus.authen_server.sin_addr.s_addr = pserver->sin_addr.s_addr;
	g_aaa_cfgs.tac_plus.authen_server.sin_port = pserver->sin_port;

	return AAA_OK;
}

int tac_plus_authen_server_get(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;

	pserver->sin_addr.s_addr = g_aaa_cfgs.tac_plus.authen_server.sin_addr.s_addr;
	pserver->sin_port = g_aaa_cfgs.tac_plus.authen_server.sin_port;

	return AAA_OK;
}

int tac_plus_authen_server_vpn_set(uint16_t vpn)
{
	g_aaa_cfgs.tac_plus.authen_vpn = vpn;
	return AAA_OK;
}

uint16_t tac_plus_authen_server_vpn_get(void)
{
	return g_aaa_cfgs.tac_plus.authen_vpn;
}


int tac_plus_authen_mode_set(int mode)
{
	g_aaa_cfgs.tac_plus.authen_mode = mode;
	return AAA_OK;
}

int tac_plus_authen_mode_get(void)
{
	return g_aaa_cfgs.tac_plus.authen_mode;
}



int tac_plus_author_server_set(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;

	g_aaa_cfgs.tac_plus.author_server.sin_addr.s_addr = pserver->sin_addr.s_addr;
	g_aaa_cfgs.tac_plus.author_server.sin_port = pserver->sin_port;

	return AAA_OK;
}

int tac_plus_author_server_get(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;

	pserver->sin_addr.s_addr = g_aaa_cfgs.tac_plus.author_server.sin_addr.s_addr;
	pserver->sin_port = g_aaa_cfgs.tac_plus.author_server.sin_port;

	return AAA_OK;
}

int tac_plus_author_server_vpn_set(uint16_t vpn)
{
	g_aaa_cfgs.tac_plus.author_vpn = vpn;
	return AAA_OK;
}

uint16_t tac_plus_author_server_vpn_get(void)
{
	return g_aaa_cfgs.tac_plus.author_vpn;
}


int tac_plus_acct_server_set(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;
	
	g_aaa_cfgs.tac_plus.acct_server.sin_addr.s_addr = pserver->sin_addr.s_addr;
	g_aaa_cfgs.tac_plus.acct_server.sin_port = pserver->sin_port;

	return AAA_OK;
}

int tac_plus_acct_server_get(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_FAIL;

	pserver->sin_addr.s_addr = g_aaa_cfgs.tac_plus.acct_server.sin_addr.s_addr;
	pserver->sin_port = g_aaa_cfgs.tac_plus.acct_server.sin_port;

	return AAA_OK;
}

int tac_plus_acct_server_vpn_set(uint16_t vpn)
{
	g_aaa_cfgs.tac_plus.acct_vpn = vpn;
	return AAA_OK;
}

uint16_t tac_plus_acct_server_vpn_get(void)
{
	return g_aaa_cfgs.tac_plus.acct_vpn;
}


/* tac_plus author server status set/get */
int tac_plus_author_server_switch_set(int status)
{
	g_aaa_cfgs.tac_plus.author_switch = status;
	return AAA_OK;
}

int tac_plus_author_server_switch_get(void)
{
	return g_aaa_cfgs.tac_plus.author_switch;
}

/* tac_plus author server status set/get */
int tac_plus_acct_server_switch_set(int status)
{
	g_aaa_cfgs.tac_plus.acct_switch = status;
	return AAA_OK;
}

int tac_plus_acct_server_switch_get(void)
{
	return g_aaa_cfgs.tac_plus.acct_switch;
}

/* tac_plus author server status set/get */
int tac_plus_acct_update_interval_set(time_t val)
{
	g_aaa_cfgs.tac_plus.interval_for_acct_update = val;
	return AAA_OK;
}

time_t tac_plus_acct_update_interval_get(void)
{
	return g_aaa_cfgs.tac_plus.interval_for_acct_update;
}

#if 0
/* tac_plus authen start */
int aaa_tac_plus_authen_start(USER_AUTHING_INFO *pinfo)
{
	char *tty = NULL;
	char *remote_addr = NULL;
	char action = TAC_PLUS_AUTHEN_LOGIN; /* default */
	char authen_type = TAC_PLUS_AUTHEN_TYPE_PAP; /* default */
	int ret = AAA_ERROR;

	if(strncmp(pinfo->user_info.ttyname, "/dev/", 5) == 0)
		tty = pinfo->user_info.ttyname + 5;

	
	if(NULL != pinfo_get)
	{
		if((strlen(pinfo->username) == strlen(pinfo_get->username))\
			&& (strlen(pinfo->password) == strlen(pinfo_get->password)))
		{
			if((0 == memcmp(pinfo->username, pinfo_get->username, strlen(pinfo_get->username)))\
				&& (0 == memcmp(pinfo->password, pinfo_get->password, strlen(pinfo_get->password))))
			{
				if(USER_LOGIN == login_mode)
				{
					aaa_auth_response_to_vty(pinfo_get->username, AUTH_SUCCESS, pinfo_get->privilege);
					tac_plus_record_start_login_time(pinfo_get);
				}
				else
				{
					if(DISABLE == tac_plus_author_server_switch_get())
						vty_out(vty, "authorize disable, ");
					else
					{
						switch(pinfo_get->author_result)
						{
							case AUTHOR_SUCCESS:
								vty_out(vty, "authorize success[priv:%d], ", pinfo_get->privilege);
								break;
							case AUTHOR_FAILED:
								vty_out(vty, "authorize failed, ");
								break;
							case AUTHOR_TIMEOUT:
								vty_out(vty, "authorize timeout, ");
								break;
							default:
								break;
						}
					}
					
					if(DISABLE == tac_plus_acct_server_switch_get())
						vty_out(vty, "account disable %s", VTY_NEWLINE);
					else
					{
						switch(pinfo_get->acct_result)
						{
							case ACCT_SUCCESS:
								vty_out(vty, "account success %s", VTY_NEWLINE);
								break;
							case ACCT_FAILED:
								vty_out(vty, "account failed %s", VTY_NEWLINE);
								break;
							case ACCT_TIMEOUT:
								vty_out(vty, "account timeout %s", VTY_NEWLINE);
								break;
							default:
								break;
						}
					}
				}
			}
		}				
		else
		{
			if(USER_LOGIN == login_mode)
				aaa_auth_response_to_vty(pinfo_get->username, AUTH_FAILED, 0);
			else
				vty_out(vty, "%suser %s authenticate failed %s",\
					VTY_NEWLINE, pinfo_get->username, VTY_NEWLINE);
		}
	}
	else
	{
		pinfo->login_mode = login_mode;
		memcpy(pinfo->username, pinfo->username, strlen(pinfo->username));
		memcpy(pinfo->password, pinfo->password, strlen(pinfo->password));

		tac_record_authen_info(pinfo, pinfo->username, pinfo->password, tty, remote_addr, action, authen_type);
		
		struct tac_attrib *author_attr = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));
		author_attr->attr = strdup("service=ppp");
		author_attr->attr_len = strlen("service=ppp");
		author_attr->next = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));
		author_attr->next->attr = strdup("protocol=ip");
		author_attr->next->attr_len = strlen("protocol=ip");
		author_attr->next->next = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));;
		author_attr->next->next->attr = strdup("priv_lvl*1");
		author_attr->next->next->attr_len = strlen("priv_lvl*1");
		author_attr->next->next->next = NULL;			
		tac_record_author_info(pinfo, pinfo->username, tty, remote_addr, author_attr);
		free(author_attr->attr);
		free(author_attr->next->attr);
		free(author_attr->next->next->attr);			
		XFREE(MTYPE_AAA, author_attr->next->next);
		XFREE(MTYPE_AAA, author_attr->next);
		XFREE(MTYPE_AAA, author_attr);
		
		tac_record_acct_info(pinfo, pinfo->username, tty, remote_addr, NULL);
		tac_plus_session_add(pinfo);						
		
	    ret = tac_authen_pkt_send(pinfo);
		if(ret != AAA_OK)
		{
			if(USER_LOGIN == login_mode)
				aaa_auth_response_to_vty(pinfo->username, AUTH_FAILED, 0);
		}
		return ret;
	}			
	
	return AAA_OK;
}
#endif

int aaa_tac_plus_process_recv_pkt(void *pkt, uint32_t pkt_len)
{
	TAC_HDR tac_head;
	struct areply arep;
	char *msg = NULL;

	if(NULL == pkt || pkt_len <= sizeof(TAC_HDR))
		return AAA_FAIL;

	aaa_pkt_dump((char *)pkt, pkt_len, AAA_PKT_RECV, AAA_TAC_PLUS, __FUNCTION__);
	memset(&arep, 0, sizeof(arep));
	/* get header */
	memcpy(&tac_head, pkt, sizeof(tac_head));

	/* check the reply fields in header */
	msg = _tac_check_header(&tac_head);
	if(msg != NULL)
	{
		zlog_debug(AAA_DBG_TACACS, "%s[%d] tacacs+ error packet header!", __FUNCTION__, __LINE__);
		return AAA_FAIL;
	}

	if(TAC_PLUS_AUTHEN == tac_head.type)
		tac_authen_rcv_process(&tac_head, (void *)((char *)pkt + sizeof(tac_head)), &arep);
	else if(TAC_PLUS_AUTHOR == tac_head.type)
		tac_author_rcv_process(&tac_head, (void *)((char *)pkt + sizeof(tac_head)), &arep);
	else if(TAC_PLUS_ACCT == tac_head.type)
		tac_acct_rcv_process(&tac_head, (void *)((char *)pkt + sizeof(tac_head)), &arep);
	else
	{
		zlog_debug(AAA_DBG_TACACS, "%s[%d] tacacs+ error packet type!", __FUNCTION__, __LINE__);
		return AAA_FAIL;
	}

	return AAA_OK;
}


int tac_plus_key_set(const char *key)
{
	if(NULL == key)
		return AAA_FAIL;

	bzero(g_aaa_cfgs.tac_plus.authen_key, TAC_PLUS_SHARED_SECRET_LEN);
	strncpy(g_aaa_cfgs.tac_plus.authen_key, key,
		TAC_PLUS_SHARED_SECRET_LEN);

	return AAA_OK;
}

char *tac_plus_key_get(void)
{
	return g_aaa_cfgs.tac_plus.authen_key;
}

/* for g_tac_session[id].id */
int tac_plus_sess_id_get(void)
{
	static int tac_sess_id = -1;

	tac_sess_id++;
	if(tac_sess_id >= TAC_PLUS_SESS_MAX)
		tac_sess_id = 0;

	return tac_sess_id;
}

/* 通过session_id获取正在认证的用户 */
USER_AUTHING_INFO *tac_plus_authing_user_get_by_session_id(int id, int mode)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		pinfo = (USER_AUTHING_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		switch(mode)
		{
			case TAC_PLUS_AUTHEN:
				if(pinfo->session_info.tacacs_user.authen_hdr.session_id == id)
					return pinfo;
				break;

			case TAC_PLUS_AUTHOR:
				if(pinfo->session_info.tacacs_user.author_hdr.session_id == id)
					return pinfo;
				break;

			case TAC_PLUS_ACCT:
				if(pinfo->session_info.tacacs_user.acct_hdr.session_id == id)
					return pinfo;
				break;
		}
	}
	return NULL;
}

/* 通过session_id获取已在线的用户 */
USER_ONLINE_INFO *tac_plus_online_user_get_by_session_id(int id, int mode)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo = (USER_ONLINE_INFO *)(node->data);
		if(pinfo == NULL)
			return NULL;

		switch(mode)
		{
			case TAC_PLUS_AUTHEN:
				if(pinfo->session_info.tacacs_user.authen_hdr.session_id == id)
					return pinfo;
				break;

			case TAC_PLUS_AUTHOR:
				if(pinfo->session_info.tacacs_user.author_hdr.session_id == id)
					return pinfo;
				break;

			case TAC_PLUS_ACCT:
				if(pinfo->session_info.tacacs_user.acct_hdr.session_id == id)
					return pinfo;
				break;
		}
	}
	return NULL;
}


/* free old src port, and get new src port */
int tac_plus_session_new_src_port(uint16_t *src_port)
{
	if(NULL == src_port) return AAA_FAIL;
	pkt_port_relese(MODULE_ID_AAA, PKT_TYPE_TCP, *src_port);
	*src_port = pkt_port_alloc(MODULE_ID_AAA, PKT_TYPE_TCP);
	return AAA_OK;
}

/**/
int tac_plus_session_src_port_set(TACACS_SESSION_INFO *psess, int mode)
{
	switch(mode)
	{
		case TAC_PLUS_AUTHEN:
			tac_plus_session_new_src_port(&psess->authen_port);
			break;
		case TAC_PLUS_AUTHOR:
			tac_plus_session_new_src_port(&psess->author_port);
			break;
		case TAC_PLUS_ACCT:
			tac_plus_session_new_src_port(&psess->acct_port);
			break;
		default:
			break;
	}
	return 1;
}


time_t tac_plus_response_timeout_get(void)
{
	return g_aaa_cfgs.tac_plus.rsp_timeout*AAA_AUTH_PROCESS_CHECK_INTERVAL;
}

u_int16_t tac_plus_cfg_authen_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.authen_server.sin_port;
}

u_int16_t tac_plus_cfg_bak_authen_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.authen_server_bak.sin_port;
}

u_int16_t tac_plus_cfg_author_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.author_server.sin_port;
}

u_int16_t tac_plus_cfg_bak_author_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.author_server_bak.sin_port;
}

u_int16_t tac_plus_cfg_acct_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.acct_server.sin_port;
}

u_int16_t tac_plus_cfg_bak_acct_server_port_get(void)
{
	return g_aaa_cfgs.tac_plus.acct_server_bak.sin_port;
}

#if 0
/* Function: tac_plus_session_show */
void tac_plus_session_show(struct vty *vty)
{
	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	tac_sess_recorder *pinfo = NULL;

	char str_start_time[64];
	char str_online_time[64];
	memset(str_start_time, 0, sizeof(str_start_time));
	memset(str_online_time, 0, sizeof(str_online_time));
	
	if(tac_session_table.num_entries > 0)
		vty_out(vty, "user                  start login time              online time %s", VTY_NEWLINE);	
	else
		return;	
	
	for((node) = hios_hash_start(&tac_session_table, (void **)&cursor); node; (node) = hios_hash_next(&tac_session_table, (void **)&cursor))		
	{
		pinfo = (tac_sess_recorder *)(node->data);
		if(pinfo == NULL)
		{
			return;
		}

		tac_plus_get_local_time_str(pinfo->time_start, str_start_time);
		tac_plus_get_user_online_time(pinfo, str_online_time);

		vty_out(vty, "%-20s %-30s %-30s %s", pinfo->username,\
			str_start_time, str_online_time, VTY_NEWLINE);
	}
}

/* show result of login test */
int tac_plus_show_login_test_result(struct thread *thread)
{
	struct vty *vty;
  	vty = THREAD_ARG (thread);

	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	tac_sess_recorder *pinfo = NULL;

	if(tac_session_table.num_entries == 0)
		return 0;
	for((node) = hios_hash_start(&tac_session_table, (void **)&cursor); node; (node) = hios_hash_next(&tac_session_table, (void **)&cursor))		
	{
		pinfo = (tac_sess_recorder *)(node->data);
		if(pinfo == NULL)
			return 0;
		
		if(TEST_LOGIN == pinfo->login_mode)
		{
			if(AUTH_SUCCESS == pinfo->authen_result)
			{
				vty_out(vty, "%suser %s authenticate success, ",\
						VTY_NEWLINE, pinfo->username);
				
				if((ENABLE == tac_plus_author_server_switch_get())\
					|| (ENABLE == tac_plus_acct_server_switch_get()))
				{
					if(DISABLE == tac_plus_author_server_switch_get())
					{
						vty_out(vty, "authorize disable, ");
					}						
					else
					{
						switch(pinfo->author_result)
						{
							case AUTHOR_SUCCESS:
								vty_out(vty, "authorize success[priv:%d], ", pinfo->privilege);
								break;
							case AUTHOR_FAILED:
								vty_out(vty, "authorize failed, ");
								break;
							case AUTHOR_TIMEOUT:
								vty_out(vty, "authorize timeout, ");
								break;
							default:
								thread_add_timer(aaa_master, tac_plus_show_login_test_result, vty, 1);
								return 1;
						}
					}
					
					if(DISABLE == tac_plus_acct_server_switch_get())
					{
						vty_out(vty, "account disable %s", VTY_NEWLINE);
						tac_plus_session_del(pinfo->username);
					}
					else
					{
						switch(pinfo->acct_result)
						{
							case ACCT_SUCCESS:
								vty_out(vty, "account success %s", VTY_NEWLINE);
								tac_plus_session_del(pinfo->username);
								break;
							case ACCT_FAILED:
								vty_out(vty, "account failed %s", VTY_NEWLINE);
								tac_plus_session_del(pinfo->username);
								break;
							case ACCT_TIMEOUT:
								vty_out(vty, "account timeout %s", VTY_NEWLINE);
								tac_plus_session_del(pinfo->username);
								break;
							default:
								thread_add_timer(aaa_master, tac_plus_show_login_test_result, vty, 1);
								return 1;
						}
					}					
				}					
				else
				{
					vty_out(vty, "authorize disable, account disable %s", VTY_NEWLINE);
					tac_plus_session_del(pinfo->username);
				}	
			}
			else if(AUTH_FAILED == pinfo->authen_result)
			{
				vty_out(vty, "%suser %s authenticate failed %s",\
					VTY_NEWLINE, pinfo->username, VTY_NEWLINE);
				tac_plus_session_del(pinfo->username);
			}					
			else if(AUTH_TIMEOUT == pinfo->authen_result)
			{
				vty_out(vty, "%suser %s authenticate timeout %s",\
					VTY_NEWLINE, pinfo->username, VTY_NEWLINE);
				tac_plus_session_del(pinfo->username);
			}
			else
				thread_add_timer(aaa_master, tac_plus_show_login_test_result, vty, 1);
		}
	}
	return 1;
}



/* test login */
int tac_plus_login_test(struct vty *vty, const char *username, const char *password)
{
	if(NULL == username)
	{
		zlog_err("%s[%d] -> %s: NULL pointer!\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}
	
	aaa_auth_request auth_msg;
	memset(&auth_msg, '\0', sizeof(auth_msg));
	memcpy(auth_msg.username, username, strlen(username));

	if(NULL != password)
		memcpy(auth_msg.password, password, strlen(password));
	
	auth_msg.msg_type = LOG_REQUEST;
	aaa_tac_plus_authen_start(vty, &auth_msg, TEST_LOGIN);
	
	thread_add_timer(aaa_master, tac_plus_show_login_test_result, vty, 3);

	return AAA_OK;
}
#endif



