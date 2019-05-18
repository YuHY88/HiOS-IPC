/******************************************************************************
 * Filename: aaa_radius_acct.c
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
#include <lib/linklist.h>

#include "aaa_config.h"
#include "aaa_radius.h"
#include "aaa_radius_acct.h"
#include "radiusclient.h"
#include "lib/pkt_buffer.h"
#include "lib/memory.h"
#include "lib/memtypes.h"
#include "aaa_packet.h"


extern struct thread_master *aaa_master;

/* config radius id */
int radius_acct_pkt_fill_id(RADIUS_SEND_PKT *pkt)
{
	if(NULL == pkt)
		return AAA_ERROR;

	pkt->id += 1;
	if(0 == pkt->id)
		pkt->id = 1;
	
	return AAA_OK;
}

/* config radius accounting request length */
int radius_acct_pkt_fill_length(RADIUS_SEND_PKT *pkt)
{
	if(NULL == pkt)
		return AAA_ERROR;
    
    struct listnode     *node, *nnode;  
    RADIUS_ATTR_PAIR    *attr_find; 

	u_int16_t length = RADIUS_HDR_LEN;	
	
    for(ALL_LIST_ELEMENTS(pkt->attribute_list, node, nnode, attr_find))	
        length += attr_find->length;
	
	pkt->length = length;

	return AAA_OK;
}


/* config radius accounting request authenticator */
int radius_acct_pkt_fill_authenticator(RADIUS_SEND_PKT *pkt)
{
	char auth[RADIUS_VECTOR_LEN];
	char str_in[1024];
	uint16_t len_acct_pkt = 0;
	uint32_t len = 0;
	if(NULL == pkt)
		return AAA_ERROR;

	memset(auth, 0, sizeof(auth));
	memset(str_in, 0, sizeof(str_in));

	memcpy(str_in, &pkt->code, sizeof(pkt->code));
	len += sizeof(pkt->code);

	memcpy(str_in+len, &pkt->id, sizeof(pkt->id));
	len += sizeof(pkt->id);

	len_acct_pkt = htons(pkt->length);
	memcpy(str_in+len, &len_acct_pkt, sizeof(pkt->length));
	len += sizeof(pkt->length);	
	
	len += RADIUS_VECTOR_LEN;	/* 16 byte "0" */

	/* get all attributes */
	struct listnode     *node, *nnode;	
    RADIUS_ATTR_PAIR	*attr_find;	
    for(ALL_LIST_ELEMENTS(pkt->attribute_list, node, nnode, attr_find))	
    {       
        memcpy(str_in+len, attr_find, attr_find->length);
		len += attr_find->length;
    }

	memcpy(str_in+len, radius_key_get(), strlen(radius_key_get()));
	len += strlen(radius_key_get());

	radius_md5_calc(auth, str_in, len);	
	
	memcpy(pkt->authenticator, auth, RADIUS_VECTOR_LEN);	
	
	return AAA_OK;
}

/* produce account session id */
int radius_acct_session_id_get(char *session_id)
{
	static uint32_t id = 1;
	id++;
	if(0 == id)
		id = 1;
	uint32_t id_temp = htonl(id);
	memcpy(session_id, &id_temp, sizeof(uint32_t));
	return AAA_OK;
}

/* update account session id */
int radius_acct_session_id_update(RADIUS_SESSION_INFO* pinfo)
{
	RADIUS_ATTR_PAIR *pair;
	pair = radius_attr_pair_lookup(pinfo->acct_pkt.attribute_list, PW_ACCT_SESSION_ID);
	radius_acct_session_id_get(pair->value);
	return AAA_OK;
}

/* update accout id and authenticator */
int radius_update_acct_pkt(RADIUS_SEND_PKT* pkt)
{
	radius_acct_pkt_fill_id(pkt);
	radius_acct_pkt_fill_authenticator(pkt);
	return AAA_OK;
}


int radius_store_acct_info_into_session(USER_AUTHING_INFO* pinfo)
{
	char val[4];
	pinfo->session_info.radius_user.acct_pkt.code = PW_ACCOUNTING_REQUEST;
	radius_acct_pkt_fill_id(&pinfo->session_info.radius_user.acct_pkt);	
	
	pinfo->session_info.radius_user.acct_pkt.attribute_list = radius_attr_list_create();

	/* add username attr */
	radius_attr_pair_add(pinfo->session_info.radius_user.acct_pkt.attribute_list, PW_USER_NAME,\
        strlen(pinfo->user_info.username), pinfo->user_info.username, 0);

	/* add Acct-Status-Type : start */
	memset(val, 0, sizeof(val));
	val[3] = PW_STATUS_START;
	radius_attr_pair_add(pinfo->session_info.radius_user.acct_pkt.attribute_list, PW_ACCT_STATUS_TYPE, 4, val, 0);

	radius_acct_session_id_get(val);
	radius_attr_pair_add(pinfo->session_info.radius_user.acct_pkt.attribute_list, PW_ACCT_SESSION_ID, 4, val, 0);
				
	radius_acct_pkt_fill_length(&pinfo->session_info.radius_user.acct_pkt);
	radius_acct_pkt_fill_authenticator(&pinfo->session_info.radius_user.acct_pkt);	    //authenticator

	/* fill update interval */
	pinfo->session_info.radius_user.acct_update_inteval = radius_acct_update_interval_get();
	return AAA_OK;
}

/* set Acct-Status-Type */
int radius_acct_switch_type_set(struct list *ls, int status)
{
	RADIUS_ATTR_PAIR *pair;

	pair = radius_attr_pair_lookup(ls, PW_ACCT_STATUS_TYPE);
	*(pair->value + 3) = status;
	return AAA_OK;
}

/* get Acct-Status-Type */
int radius_acct_switch_type_get(RADIUS_SESSION_INFO *pinfo)
{
	RADIUS_ATTR_PAIR *pair;
	pair = radius_attr_pair_lookup(pinfo->acct_pkt.attribute_list, PW_ACCT_STATUS_TYPE);

	return *(pair->value + 3);
}

/* get accounting info and packet into accounting request */
int radius_acct_info_to_request_pkt(RADIUS_SEND_PKT *pkt, char *send_buf)
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
        memcpy(send_buf+length, attr_find, attr_find->length);
		length += attr_find->length;
    }
    
	return length;
}


//accounting request packet retry
int radius_acct_request_send(RADIUS_SEND_PKT *pkt, int status)
{
	char send_buf[RC_PKT_BUF_LEN];
    if((NULL == pkt) || (pkt->length > RC_PKT_BUF_LEN))
    {
        zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: radius account attribute is too long\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
    }      
	
	radius_acct_switch_type_set(pkt->attribute_list, status);
	if(PW_STATUS_START != status)
		radius_update_acct_pkt(pkt);
	
	memset(send_buf, '\0', RC_PKT_BUF_LEN);
	if(pkt->length == radius_acct_info_to_request_pkt(pkt, send_buf))
	{
		aaa_radius_pkt_send(send_buf, pkt->length, ACCT_PKT);
	}
	else
	{
		zlog_debug(AAA_DBG_RADIUS, "%s[%d] -> %s: radius account pkt length is wrong\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}

	return AAA_OK;
}

/* process rcv accounting response */
int radius_process_rcv_acct_pkt(RC_RCV_PAYLOAD *pradius_rcv_buf)
{
	int id = pradius_rcv_buf->id;	//to distinguish user

	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_AUTHING_INFO *pinfo_authing = NULL;
	USER_ONLINE_INFO *pinfo_online = NULL;
	
	for((node) = hios_hash_start(&g_user_table.user_authing_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_authing_table, &cursor))		
	{
		pinfo_authing = (USER_AUTHING_INFO *)(node->data);
		if(pinfo_authing == NULL)
		{
			return AAA_OK;
		}

		if(pinfo_authing->session_info.radius_user.acct_pkt.id == id)
		{
			zlog_debug(AAA_DBG_RADIUS, "%s[%d] : radius user[%s][%x] acct success\n", __func__, __LINE__,
				pinfo_authing->user_info.username, pinfo_authing->user_id);
			
			pinfo_authing->session_info.radius_user.auth_retry_times = 0;
			pinfo_authing->aaa_result = ACCT_SUCCESS;
		}
	}

	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		pinfo_online = (USER_ONLINE_INFO *)(node->data);
		if(pinfo_online == NULL)
		{
			return AAA_OK;
		}

		if(pinfo_online->session_info.radius_user.acct_pkt.id == id)
		{
			zlog_debug(AAA_DBG_RADIUS, "%s[%d] : radius user[%s][%x] acct success\n", __func__, __LINE__,
				pinfo_online->user_info.username, pinfo_online->user_id);
			
			pinfo_online->session_info.radius_user.auth_retry_times = 0;
			pinfo_online->session_info.radius_user.acct_failed_cnt = 0;
			pinfo_online->session_info.radius_user.acct_send_flag = FALSE;
		}
	}

	
	return AAA_OK;
}

//------------------------- end -----------------------------//


