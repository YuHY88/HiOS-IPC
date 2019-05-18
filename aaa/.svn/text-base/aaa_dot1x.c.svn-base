/******************************************************************************
 * Filename: aaa_dot1x.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.9.14  lipf created
 *
******************************************************************************/

#include <string.h>
#include <time.h>
#include <assert.h>

#include <lib/memory.h>
#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/ifm_common.h>
#include <lib/md5.h>

#include "aaa_dot1x.h"
#include "aaa.h"
#include "aaa_user_manager.h"
#include "aaa_config.h"
#include "aaa_packet.h"
#include "aaa_radius_common.h"
#include "aaa_radius.h"
#include "aaa_radius_auth.h"
#include "aaa_radius_acct.h"
#include "aaa_msg_io.h"


extern struct thread_master *aaa_master;

/* dot1x哈希表 */
DOT1X_TABLE g_dot1x_table;
struct list *dot1x_port_cfg_ls = NULL;



/* dot1x 初始化 */
void aaa_dot1x_init (void)
{
	aaa_dot1x_switch_set (DISABLE);						//默认禁能 dot1x
	aaa_dot1x_type_based_set (BASED_PORT);				//默认基于port进行认证
	aaa_dot1x_login_mode_set (DOT1X_RADIUS_MODE);		//默认radius认证
	aaa_dot1x_auth_method_set (DOT1X_FINAL_METHOD);		//默认终结方式
	
	g_aaa_cfgs.dot1x.retry_times = DOT1X_DEF_RETRY_TIMES;
	g_aaa_cfgs.dot1x.retry_interval = DOT1X_DEF_RETRY_INTERVAL;
	
	//aaa_dot1x_pkt_register ();							//eth注册

	dot1x_port_cfg_ls = dot1x_port_list_create ();
}


/* 处理eap报文 */
void aaa_dot1x_rcv_pkt_handle (struct pkt_buffer *pkt)
{	
	struct ieee802_1x_hdr *dot1x_hdr = (struct ieee802_1x_hdr *)pkt->data;

	if (DISABLE == aaa_dot1x_switch_get ())
	{
		dot1x_eap_fail_send (dot1x_hdr->version, pkt->in_ifindex, pkt->cb.ethcb.smac);
		return;
	}
	
	switch (dot1x_hdr->type)
	{
		case IEEE802_1X_TYPE_EAP_PACKET:	//eap packet
			dot1x_eap_packet_handle (pkt);			
			break;
			
		case IEEE802_1X_TYPE_EAPOL_START:	//eapol start
			dot1x_eapol_start_handle (pkt);
			break;

		case IEEE802_1X_TYPE_EAPOL_LOGOFF:	//eapol logoff
			break;

		case IEEE802_1X_TYPE_EAPOL_KEY:
			break;

		case IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT:
			break;

		case IEEE802_1X_TYPE_SESSION_KEY:
			break;

		default:
			break;
	}
}


/* 处理 eap packet 报文 */
void dot1x_eap_packet_handle (struct pkt_buffer *pkt)
{
	struct eap_hdr *peap_hdr = (struct eap_hdr *)((char *)pkt->data + sizeof (struct ieee802_1x_hdr));
	uint8_t type = *((char *)pkt->data + sizeof (struct ieee802_1x_hdr) + sizeof (struct eap_hdr));
	switch (peap_hdr->code)
	{
		case EAP_CODE_REQUEST:
			if (EAP_TYPE_IDENTITY == type)		//identity request
			{
			}
			else if (EAP_TYPE_MD5_CHALLENGE == type)	//md5-challenge request
			{
			}				
			break;
			
		case EAP_CODE_RESPONSE:
			if (EAP_TYPE_IDENTITY == type)		//identity response
			{
				dot1x_eap_identity_response_handle (pkt);
			}
			else if (EAP_TYPE_MD5_CHALLENGE == type)	//md5-challenge response
			{
				dot1x_eap_md5_challenge_response_handle (pkt);
			}
			else if (EAP_TYPE_NAK == type)	//md5-challenge response
			{
				dot1x_eap_md5_challenge_response_handle (pkt);
			}				
			break;
			
		case EAP_CODE_SUCCESS:
			break;
			
		case EAP_CODE_FAILURE:
			break;
			
		default:
			break;
	}
}


/* 处理 eapol start 报文，存储认证信息 */
void dot1x_eapol_start_handle (struct pkt_buffer *pkt)
{
	uint32_t id = dot1x_id_create (pkt->in_ifindex, pkt->cb.ethcb.smac);
	DOT1X_ONLINE_INFO *pinfo_online = dot1x_user_online_get (id);

	struct ieee802_1x_hdr *hdr_eap = (struct ieee802_1x_hdr *)pkt->data;
	char eap_version = hdr_eap->version;	
	
	if (NULL != pinfo_online)	//基于端口认证，且该端口已经认证通过，则直接授权
	{
		dot1x_eap_success_send (eap_version, pkt->in_ifindex, pkt->cb.ethcb.smac);

		AAA_DOT1X_AUTHOR_MSG msg;
		memset (&msg, 0, sizeof (AAA_DOT1X_AUTHOR_MSG));
		msg.ifindex = pinfo_online->dot1x.ifindex;
		memcpy (msg.mac, pinfo_online->dot1x.mac, 6);
		msg.type_based = pinfo_online->type_based;
		msg.author_result = DOT1X_AUTHOR_ACCEPT;
		aaa_send_dot1x_author_response (&msg);
		return;
	}
		
	dot1x_eap_identity_request_send (eap_version, pkt->in_ifindex, pkt->cb.ethcb.smac);
}



/* 处理 eapol logoff 报文，删除已认证用户，记录日志 */
void dot1x_eapol_logoff_handle (struct pkt_buffer *pkt)
{
	uint32_t id = dot1x_id_create (pkt->in_ifindex, pkt->cb.ethcb.smac);
	
	DOT1X_ONLINE_INFO *pinfo = dot1x_user_online_get (id);
	if (NULL == pinfo)
		return;
	dot1x_user_online_del (id);
}

/* 处理 eapol key 报文 */
void dot1x_eapol_key_handle (struct pkt_buffer *pkt)
{
}

/* 处理 eapol encapsulated_asf_alert 报文 */
void dot1x_eapol_encapsulated_asf_alert_handle (struct pkt_buffer *pkt)
{
}

/* 处理 session key 报文 */
void dot1x_session_key_handle (struct pkt_buffer *pkt)
{
}


/* 处理 eapol identity response 报文 */
void dot1x_eap_identity_response_handle (struct pkt_buffer *pkt)
{
	if(pkt->data_len > sizeof (EAP_IDENTITY_RESPONSE_PKT))
	{
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] : wrong pkt length!\n", __FILE__, __LINE__);
		return;
	}
	EAP_IDENTITY_RESPONSE_PKT *pkt_eap = (EAP_IDENTITY_RESPONSE_PKT *)pkt->data;

	DOT1X_AUTHING_INFO *pinfo = (DOT1X_AUTHING_INFO *) XMALLOC (MTYPE_AAA, sizeof (DOT1X_AUTHING_INFO));
	if (NULL == pinfo)
	{
		zlog_err("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return;
	}

	memset (pinfo, 0, sizeof (DOT1X_AUTHING_INFO));
	memset (pinfo->dot1x.username, 0, USER_NAME_MAX + 1);
	memset (pinfo->dot1x.password_enctypted, 0, IEEE802_ATTACH_KEY_LEN);
	memset (pinfo->dot1x.mac, 0, 6);
	pinfo->pkt_radius.attribute_list = NULL;	
	
	pinfo->dot1x.ifindex = pkt->in_ifindex;
	memcpy (pinfo->dot1x.mac, pkt->cb.ethcb.smac, 6);
	pinfo->type_based = aaa_dot1x_type_based_get ();
	pinfo->dot1x.version = pkt_eap->hdr_dot1x.version;
	memcpy (pinfo->dot1x.username, pkt_eap->username, strlen (pkt_eap->username));
	pinfo->pkt_radius.id = dot1x_radius_id_create ();
	dot1x_user_authing_add (pinfo);

	if (DOT1X_RADIUS_MODE == aaa_dot1x_login_mode_get ())		//radius 认证
	{
		if (DOT1X_FINAL_METHOD == aaa_dot1x_auth_method_get ())
		{
			dot1x_eap_md5_challenge_request_send (pinfo, pkt->in_ifindex, (char *)pkt->cb.ethcb.smac);
		}
		else
		{
			//封装radius报文并发送
			dot1x_key_attach_create (pinfo->dot1x.key_attach);
			dot1x_send_relay_id_response_radius_access (pinfo, pkt);
		}
	}
	else	//local 认证
	{
		dot1x_eap_md5_challenge_request_send (pinfo, pkt->in_ifindex, (char *)pkt->cb.ethcb.smac);
	}
}


/* 处理 eapol identity response 报文 */
void dot1x_eap_md5_challenge_response_handle (struct pkt_buffer *pkt)
{	
	uint32_t id = 0;
	
	EAP_MD5_CHALLENGE_PKT *pkt_recv = (EAP_MD5_CHALLENGE_PKT *)pkt->data;
	
	/* 根据端口产生id，再根据id查询正在认证的用户信息 */
	id = dot1x_id_create (pkt->in_ifindex, pkt->cb.ethcb.smac);
	DOT1X_AUTHING_INFO *pinfo = dot1x_user_authing_get (id);

	if (NULL == pinfo)
		return;

	if (DOT1X_LOCAL_MODE == aaa_dot1x_login_mode_get ())
	{
		USER_CONFIG_INFO *pinfo_cfg = aaa_user_config_get_by_name (pinfo->dot1x.username);		
		if (NULL == pinfo_cfg)
		{
			dot1x_eap_fail_send (pkt_recv->hdr_dot1x.version, pkt->in_ifindex, pkt->cb.ethcb.smac);
		}
		else
		{
			if (0 == dot1x_md5_local_auth_check (1, pinfo->dot1x.key_attach, 
						pinfo_cfg->local_auth_user.password, (char *)pkt_recv->key_attach))
			{
				dot1x_eap_success_send (pkt_recv->hdr_dot1x.version, pkt->in_ifindex, pkt->cb.ethcb.smac);
				dot1x_user_online_add (dot1x_copy_authing_info_to_online (pinfo));

				AAA_DOT1X_AUTHOR_MSG msg;
				memset (&msg, 0, sizeof (AAA_DOT1X_AUTHOR_MSG));
				msg.ifindex = pinfo->dot1x.ifindex;
				memcpy (msg.mac, pinfo->dot1x.mac, 6);
				msg.type_based = pinfo->type_based;
				msg.author_result = DOT1X_AUTHOR_ACCEPT;
				aaa_send_dot1x_author_response (&msg);
			}
			else
			{
				dot1x_eap_fail_send (pkt_recv->hdr_dot1x.version, pkt->in_ifindex, pkt->cb.ethcb.smac);
			}
		}
		dot1x_user_authing_del (pinfo->id);
	}
	else
	{
		if (DOT1X_FINAL_METHOD == aaa_dot1x_auth_method_get ())	//终结模式
		{
			dot1x_send_final_radius_request (pinfo, (char *)pkt_recv->key_attach);
		}
		else
		{
			dot1x_send_relay_md5_response_radius_access (pinfo, pkt);
		}
	}		
}



/* 发送 eap identity request 报文 */
void dot1x_eap_identity_request_send (char eap_version, uint32_t ifindex, uint8_t *dmac)
{
	char 	buf[1024];
	size_t	len = 0;
	EAP_IDENTITY_REQUEST_PKT pkt_send;
	memset(buf, 0, sizeof (buf));
	memset(&pkt_send, 0, sizeof (EAP_IDENTITY_REQUEST_PKT));

	pkt_send.hdr_dot1x.version = eap_version;
	pkt_send.hdr_dot1x.type = IEEE802_1X_TYPE_EAP_PACKET;
	pkt_send.hdr_eap.code = EAP_CODE_REQUEST;
	pkt_send.hdr_eap.identifier = 1;
	pkt_send.type = EAP_TYPE_IDENTITY;
	
	pkt_send.hdr_eap.length = htons(sizeof (struct eap_hdr) + sizeof(pkt_send.type));
	pkt_send.hdr_dot1x.length = pkt_send.hdr_eap.length;	
	
	len = sizeof (EAP_IDENTITY_REQUEST_PKT);
	memcpy (buf, &pkt_send, sizeof (EAP_IDENTITY_REQUEST_PKT));

	aaa_eap_pkt_send (buf, len, ifindex, (char *)dmac);
}



/* dot1x 认证流程检测 */
int aaa_dot1x_auth_process_check (void *para)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	DOT1X_AUTHING_INFO *pinfo = NULL;

	for((node) = hios_hash_start(&g_dot1x_table.dot1x_authing_table, &cursor); node; (node) = hios_hash_next(&g_dot1x_table.dot1x_authing_table, &cursor))		
	{
		pinfo = (DOT1X_AUTHING_INFO *)(node->data);
		if (pinfo == NULL)
			return AAA_OK;

		if (AUTHEN_INVALID == pinfo->auth_result)
		{
			
		}
	}

	//thread_add_timer_msec (aaa_master, aaa_dot1x_auth_process_check, NULL, DOT1X_DEF_AUTHING_CHECK_INTERVAL);
	return AAA_OK;
}



/* dot1x attach key create */
void dot1x_key_attach_create (char *pkey)
{
	int randno;
	int ret;

	srand (time (0) + getppid () + getpid ());	// random enough :)
	for (ret = 0; ret < IEEE802_ATTACH_KEY_LEN;)
	{
		randno = rand ();
		memcpy ( pkey,  &randno, sizeof (int));
		pkey += sizeof (int);
		ret += sizeof (int);
	}
}


/* 发送 eap md5-challenge request 报文*/
void dot1x_eap_md5_challenge_request_send (DOT1X_AUTHING_INFO *pinfo, uint32_t ifindex, char *dmac)
{
	char 	buf[1024];
	size_t	len = 0;
	memset(buf, 0, sizeof (buf));
	
	EAP_MD5_CHALLENGE_PKT pkt_eap;
	memset(&pkt_eap, 0, sizeof (EAP_MD5_CHALLENGE_PKT));

	pkt_eap.hdr_dot1x.version = pinfo->dot1x.version;
	pkt_eap.hdr_dot1x.type = IEEE802_1X_TYPE_EAP_PACKET;
	pkt_eap.hdr_eap.code = EAP_CODE_REQUEST;
	pkt_eap.hdr_eap.identifier = 1;

	pkt_eap.hdr_eap.length = htons (sizeof (EAP_MD5_CHALLENGE_PKT) - sizeof (struct ieee802_1x_hdr));
	pkt_eap.hdr_dot1x.length = pkt_eap.hdr_eap.length;
	pkt_eap.type = EAP_TYPE_MD5_CHALLENGE;
	pkt_eap.key_len = IEEE802_ATTACH_KEY_LEN;
	len = sizeof (EAP_MD5_CHALLENGE_PKT);

	if (DOT1X_LOCAL_MODE == aaa_dot1x_login_mode_get ())
	{
		dot1x_key_attach_create ((char *)pkt_eap.key_attach);
		memcpy (pinfo->dot1x.key_attach, pkt_eap.key_attach, IEEE802_ATTACH_KEY_LEN);
	}
	else
	{
		if(DOT1X_FINAL_METHOD == aaa_dot1x_auth_method_get ())
		{
			dot1x_key_attach_create ((char *)pkt_eap.key_attach);
			memcpy (pinfo->dot1x.key_attach, pkt_eap.key_attach, IEEE802_ATTACH_KEY_LEN);
		}
		else
		{
		}		
	}		
	memcpy (buf, &pkt_eap, sizeof (EAP_MD5_CHALLENGE_PKT));
	aaa_eap_pkt_send (buf, len, ifindex, dmac);
}


/* 发送 eap md5-challenge from radius request 报文*/
void dot1x_eap_radius_md5_challenge_request_send (char *buf, size_t len, DOT1X_AUTHING_INFO *pinfo)
{
	char buf_send[1024];
	memset (buf_send, 0, sizeof (buf_send));
	
	buf_send[0] = pinfo->dot1x.version;
	buf_send[1] = IEEE802_1X_TYPE_EAP_PACKET;
	buf_send[2] = *(buf + 2);
	buf_send[3] = *(buf + 3);
	memcpy (buf_send + 4, buf, len);
	aaa_eap_pkt_send (buf_send, (len + 4), pinfo->dot1x.ifindex, pinfo->dot1x.mac);
}



/* dot1x eap auth success pkt send */
void dot1x_eap_success_send (uint8_t version, uint32_t ifindex, uint8_t *dmac)
{
	char 	buf[1024];
	size_t	len = 0;
	memset(buf, 0, sizeof (buf));

	EAP_AUTH_RESULT_PKT pkt_send;
	memset (&pkt_send, 0, sizeof (EAP_AUTH_RESULT_PKT));

	pkt_send.hdr_dot1x.version = version;
	pkt_send.hdr_dot1x.type = IEEE802_1X_TYPE_EAP_PACKET;
	pkt_send.hdr_eap.code = EAP_CODE_SUCCESS;
	pkt_send.hdr_eap.identifier = 1;
	pkt_send.hdr_eap.length = htons (sizeof (struct eap_hdr));
	pkt_send.hdr_dot1x.length = pkt_send.hdr_eap.length;

	len = sizeof (EAP_AUTH_RESULT_PKT);
	memcpy (buf, &pkt_send, len);
	aaa_eap_pkt_send (buf, len, ifindex, (char *)dmac);
}


/* dot1x eap auth fail pkt send */
void dot1x_eap_fail_send (uint8_t version, uint32_t ifindex, uint8_t *dmac)
{
	char 	buf[1024];
	size_t	len = 0;
	memset(buf, 0, sizeof (buf));

	EAP_AUTH_RESULT_PKT pkt_send;
	memset (&pkt_send, 0, sizeof (EAP_AUTH_RESULT_PKT));

	pkt_send.hdr_dot1x.version = version;
	pkt_send.hdr_dot1x.type = IEEE802_1X_TYPE_EAP_PACKET;
	pkt_send.hdr_eap.code = EAP_CODE_FAILURE;
	pkt_send.hdr_eap.identifier = 1;
	pkt_send.hdr_eap.length = htons (sizeof (struct eap_hdr));
	pkt_send.hdr_dot1x.length = pkt_send.hdr_eap.length;

	len = sizeof (EAP_AUTH_RESULT_PKT);
	memcpy (buf, &pkt_send, len);
	aaa_eap_pkt_send (buf, len, ifindex, (char *)dmac);
}


/* dot1x md5 challenge check，检测md5 challenge应答中的加密项，用于本地认证 */
int dot1x_md5_local_auth_check (char id, char *key, char *spassword, char *dstr)
{
	char buf_out[32], buf_in[32];
	memset (buf_out, 0, 32);
	memset (buf_in, 0, 32);

	buf_in[0] = id;
	memcpy (&buf_in[1], spassword, strlen (spassword));
	memcpy (&buf_in[1 + strlen (spassword)], key, IEEE802_ATTACH_KEY_LEN);
		
	radius_md5_calc (buf_out, buf_in, 1 + strlen (spassword) + IEEE802_ATTACH_KEY_LEN);
	return strncmp (buf_out, dstr, IEEE802_ATTACH_KEY_LEN);		
}


/* dot1x开关配置 */
void aaa_dot1x_switch_set (int status)
{
	g_aaa_cfgs.dot1x.switchh = status;
	aaa_send_dot1x_cfg_to_hal (status);
}

int aaa_dot1x_switch_get (void)
{
	return g_aaa_cfgs.dot1x.switchh;
}


/* dot1x基于 port/mac 模式配置 */
void aaa_dot1x_type_based_set (DOT1X_TYPE_BASED type_based)
{
	g_aaa_cfgs.dot1x.type_based = type_based;
}

DOT1X_TYPE_BASED aaa_dot1x_type_based_get (void)
{
	return g_aaa_cfgs.dot1x.type_based;
}


/* dot1x 登录模式配置：radius/local */
void aaa_dot1x_login_mode_set (DOT1X_LOGIN_MODE mode)
{
	g_aaa_cfgs.dot1x.login_mode = mode;
}

DOT1X_LOGIN_MODE aaa_dot1x_login_mode_get (void)
{
	return g_aaa_cfgs.dot1x.login_mode;
}


/* dot1x 认证方式配置：final/relay */
void aaa_dot1x_auth_method_set (DOT1X_AUTH_METHOD method)
{
	g_aaa_cfgs.dot1x.auth_method = method;
}

DOT1X_AUTH_METHOD aaa_dot1x_auth_method_get (void)
{
	return g_aaa_cfgs.dot1x.auth_method;
}



/*	dot1x id 生成
 *	最高两位表示类型（基于端口/mac），然后“或”上ifindex
 *
 */
uint32_t dot1x_id_create (uint32_t ifindex, uint8_t *mac)
{
	DOT1X_TYPE_BASED type = aaa_dot1x_type_based_get ();
	uint32_t id = 0;

	id = type;
	id <<= 30;
	if (BASED_PORT == type)				//高两位表示模式
	{
		id |= ifindex;
	}
	else
	{
		for (int i = 0; i < 6; i++)		//高两位表示模式，并将mac六个字节求和
		{
			id += (*(mac + i) * (i + 1));
		}
	}
	
	return id;
}



/* 在正在认证的dot1x hash表中添加会话 */
int dot1x_user_authing_add (DOT1X_AUTHING_INFO *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	pinfo->id = dot1x_id_create (pinfo->dot1x.ifindex, (uint8_t *)pinfo->dot1x.mac);
	time (&pinfo->time_login);	//记录开始登陆时间

	pbucket = (struct hash_bucket *) XMALLOC (MTYPE_HASH_BACKET, sizeof (struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}
	
	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->id;
	pbucket->data = pinfo;
	if(hios_hash_add(&g_dot1x_table.dot1x_authing_table, pbucket) != 0)
	{
		XFREE (MTYPE_AAA, pinfo);
		XFREE (MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_dot1x_table.dot1x_authing_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

DOT1X_AUTHING_INFO *dot1x_user_authing_get (uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	DOT1X_AUTHING_INFO *pinfo = NULL;

	cursor = hios_hash_find (&g_dot1x_table.dot1x_authing_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (DOT1X_AUTHING_INFO *)cursor->data;
	if(user_id == pinfo->id)
		return pinfo;
	else
		return NULL;
}


int dot1x_user_authing_del (uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	DOT1X_AUTHING_INFO *pinfo = NULL;

	cursor = hios_hash_find (&g_dot1x_table.dot1x_authing_table, (void *)user_id);
	if(NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (DOT1X_AUTHING_INFO *)cursor->data;
		
	XFREE(MTYPE_AAA, pinfo);
	hios_hash_delete(&g_dot1x_table.dot1x_authing_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);

	return AAA_ERROR;
}


/* 在已登录的dot1x hash表中添加会话 */
int dot1x_user_online_add (DOT1X_ONLINE_INFO *pinfo)
{
	if (NULL == pinfo)
		return AAA_ERROR;
	
	struct hash_bucket *pbucket = NULL;
	pbucket = (struct hash_bucket *) XMALLOC (MTYPE_HASH_BACKET, sizeof (struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return AAA_MALLOC_FAIL;
	}	
	
	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->id;
	pbucket->data = pinfo;
	if(hios_hash_add(&g_dot1x_table.dot1x_online_table, pbucket) != 0)
	{
		XFREE (MTYPE_AAA, pinfo);
		XFREE (MTYPE_HASH_BACKET, pbucket);
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			g_dot1x_table.dot1x_online_table.num_entries);
		return AAA_OVERSIZE;
	}
	return AAA_OK;
}

DOT1X_ONLINE_INFO *dot1x_user_online_get (uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	DOT1X_ONLINE_INFO *pinfo = NULL;

	cursor = hios_hash_find (&g_dot1x_table.dot1x_online_table, (void *)user_id);
	if(NULL == cursor)
		return NULL;

	pinfo = (DOT1X_ONLINE_INFO *)cursor->data;
	if(user_id == pinfo->id)
		return pinfo;
	else
		return NULL;
}


int dot1x_user_online_del (uint32_t user_id)
{
	struct hash_bucket *cursor = NULL;
	DOT1X_ONLINE_INFO *pinfo = NULL;

	cursor = hios_hash_find (&g_dot1x_table.dot1x_online_table, (void *)user_id);
	if(NULL == cursor)
		return AAA_NOT_FOUND;

	pinfo = (DOT1X_ONLINE_INFO *)cursor->data;
		
	XFREE(MTYPE_AAA, pinfo);
	hios_hash_delete (&g_dot1x_table.dot1x_online_table, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);

	return AAA_ERROR;
}


/* 将正在认证用户信息拷贝到在线用户信息数据结构中 */
DOT1X_ONLINE_INFO * dot1x_copy_authing_info_to_online (DOT1X_AUTHING_INFO *pinfo)
{
	DOT1X_ONLINE_INFO *pinfo_online = dot1x_user_online_get (pinfo->id);
	if (pinfo_online)
	{		
		return NULL;	
	}
	else
	{
		pinfo_online = (DOT1X_ONLINE_INFO *) XMALLOC (MTYPE_AAA, sizeof (DOT1X_ONLINE_INFO));
		if(NULL == pinfo_online)
		{
			zlog_err("%s[%d] XMALLOC error\n", __func__, __LINE__);
			return NULL;
		}

		memset (pinfo_online, 0, sizeof (DOT1X_ONLINE_INFO));
		
		pinfo_online->id = pinfo->id;
		pinfo_online->type_based = pinfo->type_based;
		pinfo_online->time_login = pinfo->time_login;
		pinfo_online->dot1x.ifindex = pinfo->dot1x.ifindex;
		memcpy (pinfo_online->dot1x.mac, pinfo->dot1x.mac, 6);
		memcpy (pinfo_online->dot1x.username, pinfo->dot1x.username, strlen (pinfo->dot1x.username));
		memcpy (pinfo_online->dot1x.password_enctypted, pinfo->dot1x.password_enctypted, strlen (pinfo->dot1x.password_enctypted));		
		return pinfo_online;
	}		
}


/* dot1x 产生radius id */
char dot1x_radius_id_create (void)
{
	static char id = 0;
	id++;
	if (0 == id)
		id ++;

	return id;
}


/* dot1x 计算 message-authenticator = MD5(type,id,length,authen,attributes) */
void dot1x_calc_message_authenticator (RADIUS_SEND_PKT *pkt_radius)
{
	char buf_in[1024];
	char buf_out[16];
	memset (buf_in, 0, 256);
	memset (buf_out, 0, 16);
	int len = 0;

	pkt_radius->length = ntohs (pkt_radius->length);
	memcpy (buf_in, pkt_radius, 20);
	len += 20;

	struct listnode     *node, *nnode;  
    RADIUS_ATTR_PAIR    *attr_find; 
	
    for (ALL_LIST_ELEMENTS(pkt_radius->attribute_list, node, nnode, attr_find))	
    {
    	memcpy (buf_in + len, attr_find, attr_find->length);
		len += attr_find->length;
    }

	*(buf_in + len) = PW_MESSAGE_AUTHENTICATOR;
	len += 1;
	*(buf_in + len) = 18;
	len += 1;
	len += 16;

	hmac_md5 ((uchar *)buf_in, len, (uchar *)radius_key_get (), strlen (radius_key_get ()), (uchar *)buf_out);
	radius_attr_pair_add (pkt_radius->attribute_list, PW_MESSAGE_AUTHENTICATOR,\
        16, buf_out, 0);
}


/* dot1x 终结方式发送radius报文 */
void dot1x_send_final_radius_request (DOT1X_AUTHING_INFO *pinfo, char *password)
{	
	pinfo->pkt_radius.code = PW_ACCESS_REQUEST;
	//pinfo->pkt_radius.id = dot1x_radius_id_create ();
	memcpy (pinfo->pkt_radius.authenticator, pinfo->dot1x.key_attach, RADIUS_VECTOR_LEN);

	pinfo->pkt_radius.attribute_list = radius_attr_list_create();	
	radius_attr_pair_add (pinfo->pkt_radius.attribute_list, PW_USER_NAME,\
        strlen (pinfo->dot1x.username), pinfo->dot1x.username, 0);

	radius_attr_pair_add (pinfo->pkt_radius.attribute_list, PW_CHAP_PASSWORD,\
        RADIUS_VECTOR_LEN, password, 1);

	pinfo->pkt_radius.length = RADIUS_HDR_LEN + (2 + strlen (pinfo->dot1x.username)) + (3 + RADIUS_VECTOR_LEN);

	radius_auth_request_send (&pinfo->pkt_radius);
}


/* dot1x 中继方式，封装发送 identity response 报文 */
void dot1x_send_relay_id_response_radius_access (DOT1X_AUTHING_INFO *pinfo, struct pkt_buffer *pkt)
{
	EAP_IDENTITY_RESPONSE_PKT *pkt_recv = (EAP_IDENTITY_RESPONSE_PKT *)pkt->data;

	RADIUS_SEND_PKT pkt_radius;
	memset (&pkt_radius, 0, sizeof (RADIUS_SEND_PKT));

	pkt_radius.code = PW_ACCESS_REQUEST;
	pkt_radius.id = pinfo->pkt_radius.id;
	memcpy (pkt_radius.authenticator, pinfo->dot1x.key_attach, RADIUS_VECTOR_LEN);
	
	pkt_radius.attribute_list = radius_attr_list_create ();
	radius_attr_pair_add (pkt_radius.attribute_list, PW_USER_NAME,\
        strlen (pinfo->dot1x.username), pinfo->dot1x.username, 0);
	radius_attr_pair_add (pkt_radius.attribute_list, PW_EAP_MESSAGE,\
        (sizeof (struct eap_hdr) + 1 + strlen (pkt_recv->username)), (char *)&pkt_recv->hdr_eap, 0);

	radius_auth_pkt_fill_length (&pkt_radius);
	pkt_radius.length += 18;
	dot1x_calc_message_authenticator (&pkt_radius);
	radius_auth_pkt_fill_length (&pkt_radius);
	
	radius_auth_request_send (&pkt_radius);
	radius_attr_list_destory (pkt_radius.attribute_list);
}


/* dot1x 中继方式，封装发送 md5-challenge response 报文 */
void dot1x_send_relay_md5_response_radius_access (DOT1X_AUTHING_INFO *pinfo, struct pkt_buffer *pkt)
{
	EAP_MD5_CHALLENGE_PKT *pkt_recv = (EAP_MD5_CHALLENGE_PKT *)pkt->data;
	char *buf = ((char *)pkt->data + 4);

	RADIUS_SEND_PKT pkt_radius;
	memset (&pkt_radius, 0, sizeof (RADIUS_SEND_PKT));

	pkt_radius.code = PW_ACCESS_REQUEST;
	pkt_radius.id = pinfo->pkt_radius.id;
	memcpy (pkt_radius.authenticator, pinfo->dot1x.key_attach, RADIUS_VECTOR_LEN);
	
	pkt_radius.attribute_list = radius_attr_list_create ();
	radius_attr_pair_add (pkt_radius.attribute_list, PW_USER_NAME,\
        strlen (pinfo->dot1x.username), pinfo->dot1x.username, 0);

	radius_attr_pair_add (pkt_radius.attribute_list, PW_STATE,\
        16, pinfo->state, 0);	

	size_t len_eap = ntohs(pkt_recv->hdr_eap.length);
	int i = 0;

	while (len_eap > 253)
	{
		radius_attr_pair_add (pkt_radius.attribute_list, PW_EAP_MESSAGE,\
	        253, (buf + 253 * i), 0);
		i++;

		if (len_eap > 253)
			len_eap -= 253;
		else
			break;
	}
	radius_attr_pair_add (pkt_radius.attribute_list, PW_EAP_MESSAGE, (char)len_eap, (buf + 253 * i), 0);

	radius_auth_pkt_fill_length (&pkt_radius);
	pkt_radius.length += 18;
	dot1x_calc_message_authenticator (&pkt_radius);
	radius_auth_pkt_fill_length (&pkt_radius);
	
	radius_auth_request_send (&pkt_radius);
	radius_attr_list_destory (pkt_radius.attribute_list);
}



/* 处理 dot1x 从radius的认证结果
 * result : 0(failed), 1(success)
 */
void aaa_dot1x_handle_radius_auth_result (DOT1X_AUTHING_INFO *pinfo, int result)
{
	if (result)		//success
	{
		dot1x_eap_success_send (pinfo->dot1x.version, pinfo->dot1x.ifindex, (uint8_t *)pinfo->dot1x.mac);
		dot1x_user_online_add (dot1x_copy_authing_info_to_online (pinfo));

		AAA_DOT1X_AUTHOR_MSG msg;
		memset (&msg, 0, sizeof (AAA_DOT1X_AUTHOR_MSG));
		msg.ifindex = pinfo->dot1x.ifindex;
		memcpy (msg.mac, pinfo->dot1x.mac, 6);
		msg.type_based = pinfo->type_based;
		msg.author_result = DOT1X_AUTHOR_ACCEPT;
		aaa_send_dot1x_author_response (&msg);
	}
	else			//failed
	{
		dot1x_eap_fail_send (pinfo->dot1x.version, pinfo->dot1x.ifindex, (uint8_t *)pinfo->dot1x.mac);
	}

	dot1x_user_authing_del (pinfo->id);
}


/* 处理 dot1x 从radius的挑战消息 */
void aaa_dot1x_handle_radius_access_challenge (DOT1X_AUTHING_INFO *pinfo, RC_RCV_PAYLOAD *pkt_recv)
{
	/* 接收校验仍然有问题，暂时忽略了 */
	int i = aaa_dot1x_check_radius_access_challenge (pkt_recv);
	zlog_debug(AAA_DBG_DOT1X, "%s[%d] : %s\n", __func__, __LINE__,
		(i == 0) ? "check right" : "check wrong");
	
	char buf[1024];
	memset (buf, 0, sizeof (buf));
	
	RADIUS_ATTR_PAIR radius_pair;
	RADIUS_ATTR_PAIR radius_state_pair;
	memset (&radius_pair, 0, sizeof (RADIUS_ATTR_PAIR));
	memset (&radius_state_pair, 0, sizeof (RADIUS_ATTR_PAIR));

	radius_find_attr_from_response_pkt (pkt_recv, PW_EAP_MESSAGE, 0, &radius_pair);
	radius_find_attr_from_response_pkt (pkt_recv, PW_STATE, 0, &radius_state_pair);
	memcpy (pinfo->state, radius_state_pair.value, 16);
	
	dot1x_eap_radius_md5_challenge_request_send (radius_pair.value, (radius_pair.length - 2), pinfo);

/* 校验正确之后应该使用以下的代码 */
#if 0
	if (i)
		return;
	else
	{
		radius_find_attr_from_response_pkt (pkt_recv, PW_EAP_MESSAGE, 0, &radius_pair);
		
		dot1x_eap_radius_md5_challenge_request_send (radius_pair.value, (radius_pair.length - 2), pinfo);
	}
#endif
}


/* 检测radius挑战消息的message-authenticate属性是否正确
 * result : 0(correct), 非0(error)
 */
int aaa_dot1x_check_radius_access_challenge (RC_RCV_PAYLOAD *pkt_recv)
{
	char buf_in[1024];
	char buf_out[16];
	char buf_msg_authenticate[16];
	memset (buf_in, 0, 1024);
	memset (buf_out, 0, 16);

	memcpy (buf_in, pkt_recv, (RADIUS_HDR_LEN + pkt_recv->used_length));

	char ret = 0;
	ret = buf_in[2];
	buf_in[2] = buf_in[3];
	buf_in[3] = ret;
	
	for (uint32_t i = RADIUS_HDR_LEN; i < pkt_recv->used_length; )
	{
		if (PW_MESSAGE_AUTHENTICATOR != *(buf_in + i))
		{
			i += *(buf_in + i + 1);
		}
		else
		{
			memcpy (buf_msg_authenticate, (buf_in + i + 2), 16);
			for (uint32_t j = (i + 2); j < (i + 18); j++)
				buf_in[j] = 0;

			hmac_md5 ((uchar *)buf_in, pkt_recv->used_length, (uchar *)radius_key_get (), strlen (radius_key_get ()), (uchar *)buf_out);
			//hmac_md5 (buf_in, pkt_recv->used_length, "testing123", 10, buf_out);

#if 0
			printf ("buf_in(%s[%d]) : ", radius_key_get (), strlen (radius_key_get ()));
			for (int k = 0; k < pkt_recv->used_length; k++)
				printf ("%x ", buf_in[k]);
			printf ("\n");
			
			printf ("buf_out : ");
			for (int k = 0; k < 16; k++)
				printf ("%x ", buf_out[k]);
			printf ("\n");

			printf ("buf_msg : ");
			for (int k = 0; k < 16; k++)
				printf ("%x ", buf_msg_authenticate[k]);
			printf ("\n");
#endif
			return strncmp (buf_msg_authenticate, buf_out, 16);
		}
	}

	return -1;
} 



/* 显示dot1x的配置信息 */
void aaa_dot1x_show_config (struct vty *vty)
{
	vty_out(vty, "-----------------------dot1x configuration--------------------%s", VTY_NEWLINE);	
	vty_out(vty, "%-32s : %s%s", "dot1x",
		(ENABLE == aaa_dot1x_switch_get ()) ? "enable" : "disable", VTY_NEWLINE);
	vty_out(vty, "%-32s : %s%s", "login mode",
		(DOT1X_LOCAL_MODE == aaa_dot1x_login_mode_get ()) ? "local" : "radius", VTY_NEWLINE);
	vty_out(vty, "%-32s : %s%s", "auth  method",
		(DOT1X_FINAL_METHOD == aaa_dot1x_auth_method_get ()) ? "final" : "relay", VTY_NEWLINE);	
	vty_out(vty, "--------------------------------------------------------------%s%s", VTY_NEWLINE, VTY_NEWLINE);

	aaa_dot1x_show_port_cfg_info (vty);
}



extern int aaa_get_local_time_str (time_t t, char *str);
extern int aaa_get_user_online_time (time_t t, char *str);

/* 显示dot1x的已授权信息 */
void aaa_dot1x_show_online_info (struct vty *vty)
{
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	DOT1X_ONLINE_INFO *pinfo_online = NULL;

	char port_name[64];
	char time_start[64];
	char time_online[64];
	memset (port_name, 0, 64);
	memset (time_start, 0, 64);
	memset (time_online, 0, 64);

	vty_out(vty, "--------------------dot1x online information------------------%s", VTY_NEWLINE);
	
	for((node) = hios_hash_start(&g_dot1x_table.dot1x_online_table, &cursor); node; (node) = hios_hash_next(&g_dot1x_table.dot1x_online_table, &cursor))		
	{
		pinfo_online = (DOT1X_ONLINE_INFO *)(node->data);
		if(pinfo_online == NULL)
			return ;

		vty_out(vty, "**************************************************************%s", VTY_NEWLINE);
		
		if (BASED_PORT == pinfo_online->type_based)
		{
			ifm_get_name_by_ifindex (pinfo_online->dot1x.ifindex, port_name);
			vty_out(vty, "%-32s : %s%s", "port", port_name, VTY_NEWLINE);
		}
		else
		{
			vty_out(vty, "%-32s : %x:%x:%x:%x:%x:%x%s", "source mac",
				pinfo_online->dot1x.mac[0],
				pinfo_online->dot1x.mac[1],
				pinfo_online->dot1x.mac[2],
				pinfo_online->dot1x.mac[3],
				pinfo_online->dot1x.mac[4],
				pinfo_online->dot1x.mac[5], VTY_NEWLINE);
		}
		
		vty_out(vty, "%-32s : %s%s", "type based", 
			(BASED_PORT == pinfo_online->type_based)?"port":"mac", VTY_NEWLINE);		

		aaa_get_local_time_str (pinfo_online->time_login, time_start);
		aaa_get_user_online_time (pinfo_online->time_login, time_online);
		
		vty_out(vty, "%-32s : %s%s", "login  time", time_start, VTY_NEWLINE);
		vty_out(vty, "%-32s : %s%s", "online time", time_online, VTY_NEWLINE);
		
		vty_out(vty, "**************************************************************%s", VTY_NEWLINE);
	}	
}



/* dot1x 配置保存 */
void aaa_write_dot1x_config (struct vty *vty)
{
	if (ENABLE == aaa_dot1x_switch_get ())
		vty_out(vty, " dot1x enable %s", VTY_NEWLINE);
	if (BASED_MAC == aaa_dot1x_type_based_get ())
		vty_out(vty, " dot1x port-method macbased %s", VTY_NEWLINE);
	if (DOT1X_LOCAL_MODE == aaa_dot1x_login_mode_get ())
		vty_out(vty, " dot1x authenticator-server local %s", VTY_NEWLINE);
	if (DOT1X_RELAY_METHOD == aaa_dot1x_auth_method_get ())
		vty_out(vty, " dot1x authenticator-method relay %s", VTY_NEWLINE);
}





/* * Function: dot1x_port_index_cmp
 * * Purpose: compare port index value
 * * Returns: val1 < val2, return -1
 * *   		  val1 = val2, return 0
 * *		  val1 > val2, return 1
 * */
int dot1x_port_index_cmp (uint32_t *val1, uint32_t *val2)
{	
    if(NULL == val1)
    {
        if(NULL == val2)
            return 0;
        else
            return -1;
    }
    else
    {
        if(NULL == val2)
            return 1;
        else
        {
        	if(*val1 > *val2)
				return 1;
			else if(*val1 == *val2)
				return 0;
			else
				return -1;
        }
    }   
}

/* create attr list */
struct list *dot1x_port_list_create (void)
{
    struct list *ls = list_new ();	
    ls->del = (void (*) (void *))dot1x_port_cfg_free;	
    ls->cmp = (int (*) (void *, void *))dot1x_port_index_cmp;
    return ls;
}

/* destory dot1x port list */
void dot1x_port_list_destory (struct list *ls)
{
    list_delete_all_node (ls);
    list_free (ls);
}

/* dot1x port cfg new */ 
DOT1X_PORT_CFG *dot1x_port_cfg_new (void)
{	
    return (DOT1X_PORT_CFG *)XCALLOC(MTYPE_AAA, sizeof(DOT1X_PORT_CFG));
}

void dot1x_port_cfg_add (struct list *ls, uint32_t ifindex, int state, DOT1X_TYPE_BASED	type_based)
{
    DOT1X_PORT_CFG *port_cfg = dot1x_port_cfg_new ();
	if(NULL == port_cfg)
	{
		zlog_err("%s[%d] -> %s: XMALLOC error!\n", __FILE__, __LINE__, __func__);
		return;
	}	
	
    port_cfg->ifindex = ifindex;
	port_cfg->state = state;
	port_cfg->type_based = type_based;
		
    listnode_add_sort(ls, port_cfg);
	aaa_send_dot1x_port_cfg_to_hal (ifindex, DISABLE, BASED_PORT);
}

DOT1X_PORT_CFG *dot1x_port_cfg_lookup (struct list *ls, uint32_t ifindex)
{
    struct listnode *node, *nnode;	
    DOT1X_PORT_CFG	*port_find;	
    for(ALL_LIST_ELEMENTS (ls, node, nnode, port_find))	
    {       
        if(port_find->ifindex == ifindex)
            return port_find;   
    }   
    return NULL;
}

void dot1x_port_cfg_free (DOT1X_PORT_CFG *port_cfg)
{
    XFREE(MTYPE_AAA, port_cfg);
}

int dot1x_port_cfg_del (struct list *ls, uint32_t ifindex)
{
    DOT1X_PORT_CFG *port_cfg;
    port_cfg = dot1x_port_cfg_lookup (ls, ifindex);	
    listnode_delete (ls, port_cfg);	
    ls->del (port_cfg);	

	aaa_send_dot1x_port_cfg_to_hal (ifindex, DISABLE, BASED_PORT);
	
    return AAA_OK;
}


/* 禁能所有端口的dot1x功能，并向hal发送禁能消息 */
void dot1x_port_cfg_del_all (struct list *ls)
{
	struct listnode *node, *nnode;
    DOT1X_PORT_CFG	*port_find;
	
    for (ALL_LIST_ELEMENTS (ls, node, nnode, port_find))	
    {
    	aaa_send_dot1x_port_cfg_to_hal (port_find->ifindex, DISABLE, BASED_PORT);
		dot1x_port_cfg_del (ls, port_find->ifindex);
    }
}


/* 显示已使能端口 */
void aaa_dot1x_show_port_cfg_info (struct vty *vty)
{
	struct listnode *node, *nnode;	
    DOT1X_PORT_CFG	*port_find;

	vty_out(vty, "------------------port dot1x config information---------------%s", VTY_NEWLINE);

	int cnt = 0;
	char port_name[64];
	
    for(ALL_LIST_ELEMENTS (dot1x_port_cfg_ls, node, nnode, port_find))	
    {
    	cnt++;
		memset (port_name, 0, sizeof (port_name));
		ifm_get_name_by_ifindex (port_find->ifindex, port_name);
		if (1 == cnt)
		{
			vty_out (vty, "**************************************************************%s", VTY_NEWLINE);
			vty_out (vty, "%22s : %s %s", "port of dot1x enable", port_name, VTY_NEWLINE);
		}
		else
			vty_out (vty, "%22s   %s %s", " ", port_name, VTY_NEWLINE);
    }

	if (cnt)
		vty_out (vty, "**************************************************************%s", VTY_NEWLINE);
}




