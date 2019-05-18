/******************************************************************************
 * Filename: aaa_radius_auth.c
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.4.25  lipf created
 *
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/md5.h>

#include "aaa_config.h"
#include "aaa_radius.h"
#include "aaa_radius_auth.h"
#include "aaa_radius_acct.h"
#include "radiusclient.h"
#include "aaa_packet.h"
#include "lib/pkt_buffer.h"
#include "lib/memory.h"
#include "lib/memtypes.h"
#include "aaa_dot1x.h"

extern struct thread_master *aaa_master;


//radius_pap_password_calc(), store calc result to password 
//success : return AAA_OK
//fail	  : return AAA_ERROR
int radius_pap_password_calc(char *pw_output, char *pw_input, RADIUS_SEND_PKT *auth_pkt)
{
	char  md5buf[RADIUS_SHARED_SECRET_LEN + RADIUS_HDR_LEN];		//md5 buf
	char  password[RADIUS_PASSWORD_LEN_MAX];	//password buf
	char  tempbuf[RADIUS_PASSWORD_LEN_MAX];
	char  length, padded_length, secretlen, i, pc, pswd_length;
	char *pauth, *buf;
	
	memset(md5buf, 0, RADIUS_SHARED_SECRET_LEN + RADIUS_HDR_LEN);
	
	length = strlen(pw_input);
	if (length > RADIUS_PASSWORD_LEN_MAX)			//the biggest length of the password:48
	{
		zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: Password is too long!\n",\
            __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}

	// Calculate the padded length 
	padded_length =
		(length + (RADIUS_VECTOR_LEN - 1)) & ~(RADIUS_VECTOR_LEN - 1);

	memset ( password, '\0', RADIUS_PASSWORD_LEN_MAX);				
	memcpy ( password, (char*)pw_input, length);

	secretlen = strlen(radius_key_get());		
	
	pauth =  auth_pkt->authenticator;
	buf = tempbuf;
	memset(buf, 0, RADIUS_PASSWORD_LEN_MAX);
	pswd_length = 0;	
	for (i = 0; i < padded_length; i += MD5_MAC_LEN)
	{
		// Calculate the MD5 digest
		strcpy ( md5buf, radius_key_get());
		memcpy ( md5buf + secretlen, pauth, MD5_MAC_LEN);	//secret + auth
		radius_md5_calc (buf, md5buf, secretlen + MD5_MAC_LEN);

		// Remeber the start of the digest 
		pauth = buf;

		// Xor the password into the MD5 digest 
		for (pc = i; pc < (i + MD5_MAC_LEN); pc++)
		{
			*buf++ ^= password[(uint8_t)pc];
		}
		pswd_length++;
	}
	memcpy(pw_output, tempbuf, 16*pswd_length);
	
	return AAA_OK;
}


/* chap id create */
uchar radius_chap_id_create (void)
{
	static uchar id = 0;
	id++;
	return id;
}

//radius_chap_password_calc(), store calc result to password 
//success : return AAA_OK
//fail	  : return AAA_ERROR
int radius_chap_password_calc(char *pw_output, char *pw_input, uchar chap_id, RADIUS_SEND_PKT *auth_pkt)
{
	char  md5buf[RADIUS_SHARED_SECRET_LEN + RADIUS_HDR_LEN];
	char  buf[RADIUS_PASSWORD_LEN_MAX + 1];
	
	memset(md5buf, 0, RADIUS_SHARED_SECRET_LEN + RADIUS_HDR_LEN);
	memset(buf, 0, RADIUS_PASSWORD_LEN_MAX);
		
	memcpy ( md5buf, &chap_id, sizeof (chap_id));
	memcpy ( md5buf + sizeof (chap_id), pw_input, strlen (pw_input));	
	memcpy ( md5buf + sizeof (chap_id) + strlen (pw_input), 
		auth_pkt->authenticator, RADIUS_VECTOR_LEN);
	
	radius_md5_calc (buf, md5buf, sizeof (chap_id) + strlen (pw_input) + RADIUS_VECTOR_LEN);
	memcpy (pw_output, buf, sizeof (buf));

	return AAA_OK;
}


//config radius id
int radius_auth_pkt_fill_id(USER_AUTHING_INFO *pinfo)
{
	if(NULL == pinfo)
		return AAA_ERROR;
	
	static char id = 1;		//id==0 will case fault when poweron
	int cnt = 0;
	
	/* make sure every id is different */
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo_get = NULL;
		
	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		pinfo_get = (USER_AUTHING_INFO *)(node->data);
		if(pinfo_get == NULL)
			return AAA_ERROR;

		if(pinfo_get->session_info.radius_user.auth_pkt.id == id)
		{
			id++;
			cnt++;
			if(cnt >=254)
			{
				zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: account id repeat!\n", __FILE__, __LINE__, __func__);
				return AAA_ERROR;
			}
		}
	}

	
	pinfo->session_info.radius_user.auth_pkt.id = id;
	id++;						//debug	
	if(0 == id)
		id = 1;

	return AAA_OK;
}

//config radius length
//length : the length of radius
int radius_auth_pkt_fill_length (RADIUS_SEND_PKT *pkt)
{
	if(NULL == pkt)
		return AAA_ERROR;
    
    struct listnode     *node, *nnode;  
    RADIUS_ATTR_PAIR    *attr_find; 

	u_int16_t length = RADIUS_HDR_LEN;	
	
    for(ALL_LIST_ELEMENTS(pkt->attribute_list, node, nnode, attr_find))	
    {
    	if(MODE_PAP == g_aaa_cfgs.radius.authen_mode)
    	{
			if(PW_CHAP_PASSWORD == attr_find->type)
			{
				continue;
			}
    	}
        else
        {
        	if(PW_USER_PASSWORD == attr_find->type)
			{
				continue;
			}
        }
		length += attr_find->length;
    } 
	
	pkt->length = length;

	return AAA_OK;
}



//config radius authenticator
int radius_auth_pkt_fill_authenticator(USER_AUTHING_INFO *pinfo)
{
	char auth[RADIUS_VECTOR_LEN];
	if(NULL == pinfo)
		return AAA_ERROR;

	radius_random_authenticator_get(auth);
	memcpy((char*)pinfo->session_info.radius_user.auth_pkt.authenticator, auth, RADIUS_VECTOR_LEN);	
	
	return AAA_OK;
}


/* fill auth head */
int radius_fill_auth_head(USER_AUTHING_INFO *pinfo)
{
    if(NULL == pinfo)
		return AAA_ERROR;
	
	pinfo->session_info.radius_user.auth_pkt.code = PW_ACCESS_REQUEST;
    radius_auth_pkt_fill_id(pinfo);
    radius_auth_pkt_fill_authenticator(pinfo);
    radius_auth_pkt_fill_length(&pinfo->session_info.radius_user.auth_pkt);
	
	return AAA_OK;
}


//radius_process_rcv_auth_pkt
int radius_process_rcv_auth_pkt (RC_RCV_PAYLOAD *pradius_rcv_buf)
{
	char md5buf[256];
	char tempbuf[MD5_MAC_LEN];
	char *pauth;
	RADIUS_ATTR_PAIR attr_level;

	/* for check hash */
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo = NULL;

	MD5_CTX context;

	if(NULL == pradius_rcv_buf)
	{
		zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: radius_rcv_buf is empty\n",\
            __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}
	memset(md5buf, 0, 256);
	memset(tempbuf, 0, MD5_MAC_LEN);


	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{	
		if (NULL == (pinfo = (USER_AUTHING_INFO *)(node->data)))
			return AAA_ERROR;
		
		if (pinfo->session_info.radius_user.auth_pkt.id == pradius_rcv_buf->id)
		{
			//radius_auth_timeout_check_stop(pinfo->user_info.username);		//clear timeout check
			pradius_rcv_buf->length = ntohs(pradius_rcv_buf->length);
			MD5Init (&context);
			MD5Update (&context, &pradius_rcv_buf->code, 
				RADIUS_CODE_LEN + RADIUS_ID_LEN + RADIUS_LENGTH_LEN);
			MD5Update (&context, pinfo->session_info.radius_user.auth_pkt.authenticator, MD5_MAC_LEN);
			MD5Update (&context, (pradius_rcv_buf->data),
			     pradius_rcv_buf->used_length-RADIUS_HDR_LEN);
			MD5Update (&context, radius_key_get(), strlen(radius_key_get()));
			MD5Final ((uint8_t *)tempbuf, &context);

			pauth = (char*)pradius_rcv_buf->auth;

			/* auth match */
			if(0 == memcmp(tempbuf, pauth, RADIUS_VECTOR_LEN))
			{
				switch (pradius_rcv_buf->code)
				{
					case PW_ACCESS_ACCEPT:		//accept
						memset(&attr_level, 0, sizeof(attr_level));
						if (AAA_OK == radius_find_attr_from_response_pkt(pradius_rcv_buf, PW_VENDOR_SPECIFIC, HH_EXEC_PRIVILEGE, &attr_level))
						{
							memcpy(&pinfo->user_info.level_author, attr_level.value, attr_level.length);
							pinfo->user_info.level_author = ntohl(pinfo->user_info.level_author);
						}
						else
						{
							pinfo->user_info.level_author = 0;
						}
						pinfo->aaa_result = AUTHEN_SUCCESS;
						zlog_debug(AAA_DBG_RADIUS, "%s[%d] : radius user[%s][%x][%d] authen success\n", __func__, __LINE__,
							pinfo->user_info.username, pinfo->user_id, pinfo->user_info.level_author);
						break;
					
					case PW_ACCESS_REJECT:		//reject
						pinfo->aaa_result = AUTHEN_FAILED;
						zlog_debug(AAA_DBG_RADIUS, "%s[%d] : radius user[%s][%x] authen failed\n", __func__, __LINE__,
							pinfo->user_info.username, pinfo->user_id);
						break;					

					default: 
						//response "Unknown RADIUS message code"
						break; 
				}
			}

			/* auth does not match */
			else
			{
				zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: respond-auth not match!\n",\
				    __FILE__, __LINE__, __func__);
				pinfo->aaa_result = AUTHEN_FAILED;
			}

			pinfo->session_info.radius_user.auth_retry_times = 0;
			return AAA_OK;
		}
	}

	DOT1X_AUTHING_INFO *pinfo_dot1x = NULL;
	for((node) = hios_hash_start(&g_dot1x_table.dot1x_authing_table, &cursor); node; (node) = hios_hash_next(&g_dot1x_table.dot1x_authing_table, &cursor))		
	{	
		if (NULL == (pinfo_dot1x = (DOT1X_AUTHING_INFO *)(node->data)))
			return AAA_ERROR;
		
		if (pinfo_dot1x->pkt_radius.id == pradius_rcv_buf->id)
		{
			switch (pradius_rcv_buf->code)
			{
				case PW_ACCESS_ACCEPT:
					aaa_dot1x_handle_radius_auth_result (pinfo_dot1x, 1);
					break;
				
				case PW_ACCESS_REJECT:
					aaa_dot1x_handle_radius_auth_result (pinfo_dot1x, 0);
					break;

				case PW_ACCESS_CHALLENGE:	//
					aaa_dot1x_handle_radius_access_challenge (pinfo_dot1x, pradius_rcv_buf);
				
				default: 
					//response "Unknown RADIUS message code"
					break;
			}
		}
	}
	
	return AAA_OK;
}


/*
 * Function: radius_create_attr_list
 *
 * Purpose: create an attribute-value pair list.
 *
 * Returns: success -> AAA_OK, failure -> AAA_ERROR.
 *
 */
int radius_auth_info_to_request_pkt(RADIUS_SEND_PKT *pkt, char *send_buf)
{
	RADIUS_SEND_PKT header;
	int length = RADIUS_HDR_LEN;
	memcpy(&header, pkt, sizeof(header));
	header.length = htons(header.length);
	memcpy(send_buf, &header, RADIUS_HDR_LEN);

    struct listnode     *node, *nnode;  
    RADIUS_ATTR_PAIR    *attr_find; 	
    for(ALL_LIST_ELEMENTS(pkt->attribute_list, node, nnode, attr_find))	
    {
    	if(MODE_PAP == g_aaa_cfgs.radius.authen_mode)
    	{
			if(PW_CHAP_PASSWORD == attr_find->type)
			{
				continue;
			}
    	}
        else
        {
        	if(PW_USER_PASSWORD == attr_find->type)
			{
				continue;
			}
        }

		memcpy(send_buf+length, attr_find, attr_find->length);
		length += attr_find->length;
    }
    
	return length;
}



/* store radius auth info to session */
int radius_store_auth_info_into_session(USER_AUTHING_INFO* pinfo)
{
    char pw_encrypted[RADIUS_PASSWORD_LEN_MAX + 1];	//password after being encrypted

	pinfo->session_info.radius_user.auth_pkt.code = PW_ACCESS_REQUEST;
	radius_auth_pkt_fill_id(pinfo);				    //id	
	radius_auth_pkt_fill_authenticator(pinfo);	    //authenticator
	
	pinfo->session_info.radius_user.auth_pkt.attribute_list = radius_attr_list_create();	
	radius_attr_pair_add(pinfo->session_info.radius_user.auth_pkt.attribute_list, PW_USER_NAME,\
        strlen(pinfo->user_info.username), pinfo->user_info.username, 0);
		
	/* encrypte and add password */
	memset(pw_encrypted, 0, MAX (AUTH_PASS_LEN, CHAP_VALUE_LENGTH));
	radius_pap_password_calc(pw_encrypted, pinfo->user_info.password_init, &pinfo->session_info.radius_user.auth_pkt);
	radius_attr_pair_add(pinfo->session_info.radius_user.auth_pkt.attribute_list, PW_USER_PASSWORD,\
        strlen(pw_encrypted), pw_encrypted, 0);

	/* 添加chap密码 */
	memset(pw_encrypted, 0, MAX (AUTH_PASS_LEN, CHAP_VALUE_LENGTH));
	uchar chap_id = radius_chap_id_create ();
	radius_chap_password_calc(pw_encrypted, pinfo->user_info.password_init,
		chap_id, &pinfo->session_info.radius_user.auth_pkt);
	radius_attr_pair_add(pinfo->session_info.radius_user.auth_pkt.attribute_list, PW_CHAP_PASSWORD,\
        strlen(pw_encrypted), pw_encrypted, chap_id);
	
	radius_auth_pkt_fill_length(&pinfo->session_info.radius_user.auth_pkt);	
	
	return 1;
}


int radius_auth_request_send (RADIUS_SEND_PKT *pkt)
{
	char send_buf[RC_PKT_BUF_LEN];
	memset(send_buf, 0, RC_PKT_BUF_LEN);
	if(NULL == pkt)
		return AAA_ERROR;

	if(pkt->length == radius_auth_info_to_request_pkt (pkt, send_buf))
		aaa_radius_pkt_send (send_buf, pkt->length, AUTH_PKT);
	else
	{
		zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: radius auth pkt length is wrong\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}

	return AAA_OK;
}

//------------------------- end -----------------------------//


