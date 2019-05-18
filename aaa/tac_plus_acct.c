/* acct_s.c - Send account request to the server.
 * acct_r.c - Reads account reply from the server.
 * 
 * Copyright (C) 2010, Pawel Krawczyk <pawel.krawczyk@hush.com> and
 * Jeroen Nijhof <jeroen@jeroennijhof.nl>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program - see the file COPYING.
 *
 * See `CHANGES' file for revision history.
 *
 * modified by lipf @ 2017.5.12
 */

#include <string.h>
#include "lib/memory.h"
#include "lib/memtypes.h"
#include "lib/aaa_common.h"

#include "aaa_tac_plus.h"
#include "aaa_config.h"
#include "aaa_user_manager.h"	
#include "xalloc.h"
#include "tacplus.h"
#include "aaa_packet.h"


int tac_acct_session_renew_src_port(int *src_port)
{
	if(NULL == src_port) return AAA_FAIL;

	int temp = *src_port;
	typedef struct _port_list{
		int val;
		struct _port_list *next;
	}port_list;

	port_list *port_hdr = (port_list *)XMALLOC(MTYPE_AAA, sizeof(port_list));
	if(NULL == port_hdr)
	{
		zlog_err("%s[%d] -> %s : malloc error\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}
	port_hdr->val = 0;
	port_hdr->next = NULL;

	port_list *port = port_hdr;
	while(1)
	{		
		port->val = pkt_port_alloc(MODULE_ID_AAA, PKT_TYPE_TCP);
		if(-1 == port->val)
		{
			zlog_err("%s[%d] get port failed!", __FUNCTION__, __LINE__);
			return AAA_PORT_ALLOC_FAIL;
		}

		if(port->val > temp)
		{
			*src_port = port->val;
			port = port_hdr;
			while(port->next)
			{
				pkt_port_relese(MODULE_ID_AAA, PKT_TYPE_TCP, port->val);
				XFREE(MTYPE_AAA, port);
				port = port_hdr;
			}
			return AAA_OK;
		}
		else
		{
			port = port->next;
			if(NULL == (port = (port_list *)XMALLOC(MTYPE_AAA, sizeof(port_list))))
			{
				zlog_err("%s[%d] -> %s : malloc error\n", __FILE__, __LINE__, __func__);
				return AAA_ERROR;
			}
			port->val = 0;
			port->next = NULL;
		}
	}
}

/* Send account request to the server, along with attributes
   specified in attribute list prepared with tac_add_attrib.
 *
 * return value:
 *      AAA_OK 	  : success
 *   	AAA_ERROR : failed
 */

int tac_record_acct_info(USER_AUTHING_INFO *pinfo, const char *r_addr)
{
	char user_len, port_len, r_addr_len;
	int i = 0;          /* attributes count */
	struct tac_attrib *a;
	int data_len = 0;
	int len = 0;

	/* 需要添加属性时，使能这段代码，并增加free，参考author */
	struct tac_attrib *author_attr = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));
	/*author_attr->attr = strdup("service=ppp");
	author_attr->attr_len = strlen("service=ppp");
	author_attr->next = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));
	author_attr->next->attr = strdup("protocol=ip");
	author_attr->next->attr_len = strlen("protocol=ip");
	author_attr->next->next = (struct tac_attrib *)XCALLOC(MTYPE_AAA, sizeof(struct tac_attrib));;
	author_attr->next->next->attr = strdup("priv_lvl*1");
	author_attr->next->next->attr_len = strlen("priv_lvl*1");
	author_attr->next->next->next = NULL;
	*/

	
	_tac_req_header(TAC_PLUS_ACCT, 0, &pinfo->session_info.tacacs_user.acct_hdr);

	/* fill authen src port */
	tac_plus_session_src_port_set(&pinfo->session_info.tacacs_user, TAC_PLUS_ACCT);

	/* set header options */
    pinfo->session_info.tacacs_user.acct_hdr.version = TAC_PLUS_VER_0;
    pinfo->session_info.tacacs_user.acct_hdr.encryption = tac_encryption ? TAC_PLUS_ENCRYPTED_FLAG : TAC_PLUS_UNENCRYPTED_FLAG;

    zlog_debug(0, "%s: user '%s', tty '%s', rem_addr '%s', encrypt: %s\n", \
        __FUNCTION__, pinfo->user_info.username, \
        pinfo->user_info.ttyname, r_addr, tac_encryption ? "yes" : "no");
    
	user_len = pinfo->user_info.username != NULL ? (char)strlen(pinfo->user_info.username) : 0;
    port_len = pinfo->user_info.ttyname != NULL ? (char)strlen(pinfo->user_info.ttyname) : 0;
    r_addr_len = r_addr != NULL ? (char)strlen(r_addr) : 0;

	pinfo->session_info.tacacs_user.acct_pkt.flags = TAC_PLUS_ACCT_FLAG_START;
    pinfo->session_info.tacacs_user.acct_pkt.authen_method = TAC_PLUS_AUTHEN_METH_TACACSPLUS;
    pinfo->session_info.tacacs_user.acct_pkt.priv_lvl = TAC_PLUS_PRIV_LVL_USER;//tac_priv_lvl;
    pinfo->session_info.tacacs_user.acct_pkt.authen_type = TAC_PLUS_AUTHEN_TYPE_PAP;
    pinfo->session_info.tacacs_user.acct_pkt.authen_service = TAC_PLUS_AUTHEN_SVC_NONE;
    pinfo->session_info.tacacs_user.acct_pkt.user_len = user_len;
    pinfo->session_info.tacacs_user.acct_pkt.port_len = port_len;
    pinfo->session_info.tacacs_user.acct_pkt.r_addr_len = r_addr_len;

	/* calc length of all attributes */
	a = author_attr;
	data_len = user_len + port_len + r_addr_len;
	while(a)
	{
		data_len += sizeof(a->attr_len);
		data_len += a->attr_len;
		a = a->next;
		i++;
	}
	pinfo->session_info.tacacs_user.acct_pkt.arg_cnt = i;

	if (data_len > 1024)
	{
		zlog_err ("%s[%d] : tacacs acct pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}
	pinfo->session_info.tacacs_user.acct_data_size = data_len;
	memset (pinfo->session_info.tacacs_user.acct_data, 0, 1024);

	/* record all arg len */
	a = author_attr;
	len = 0;
	while(a)
	{
		memcpy(pinfo->session_info.tacacs_user.acct_data + len, &a->attr_len, sizeof(a->attr_len));
		len += sizeof(a->attr_len);
		a = a->next;
	}

	/* record all arg val */
	if(NULL != pinfo->user_info.username)
	{
		memcpy(pinfo->session_info.tacacs_user.acct_data + len, pinfo->user_info.username, strlen(pinfo->user_info.username));
		len += strlen(pinfo->user_info.username);
	}
	if(NULL != pinfo->user_info.ttyname)
	{
		memcpy(pinfo->session_info.tacacs_user.acct_data + len, pinfo->user_info.ttyname, strlen(pinfo->user_info.ttyname));
		len += strlen(pinfo->user_info.ttyname);
	}
	if(NULL != r_addr)
	{
		memcpy(pinfo->session_info.tacacs_user.acct_data + len, r_addr, strlen(r_addr));
		len += strlen(r_addr);
	}	
	
	a = author_attr;
	while(a)
	{
		memcpy(pinfo->session_info.tacacs_user.acct_data + len, a->attr, a->attr_len);
		len += a->attr_len;
		a = a->next;
	}

	pinfo->session_info.tacacs_user.acct_hdr.datalength = htonl(TAC_ACCT_REQ_FIXED_FIELDS_SIZE + data_len);

	/* free */
	/*free(author_attr->attr);
	free(author_attr->next->attr);
	free(author_attr->next->next->attr);			
	XFREE(MTYPE_AAA, author_attr->next->next);
	XFREE(MTYPE_AAA, author_attr->next);*/
	XFREE(MTYPE_AAA, author_attr);
	
	return AAA_OK;
}

int tac_acct_pkt_send_body(TACACS_SESSION_INFO *psess)
{
	int ret;
	uint32_t data_len = 0;
	char pkt[TAC_PLUS_SEND_PKT_LEN_MAX];
	memset(pkt, 0, TAC_PLUS_SEND_PKT_LEN_MAX);
	
	data_len = psess->acct_pkt.user_len + psess->acct_pkt.port_len + psess->acct_pkt.r_addr_len;
	for(ret = 0;ret < psess->acct_pkt.arg_cnt;ret++)
	{
		data_len += psess->acct_data[ret];
		data_len += sizeof(psess->acct_data[ret]);
	}		

	if((data_len+TAC_ACCT_REQ_FIXED_FIELDS_SIZE) != (uint32_t)ntohl(psess->acct_hdr.datalength))
	{
		zlog_err("%s: acct_data len error!\n", __FUNCTION__);
		return AAA_ERROR;
	}
		 
	memcpy(pkt, &psess->acct_pkt, sizeof(psess->acct_pkt));
	memcpy(pkt+sizeof(psess->acct_pkt), psess->acct_data, data_len);
    
    /* encrypt packet body  */
    _tac_crypt(pkt, &psess->acct_hdr, data_len+TAC_ACCT_REQ_FIXED_FIELDS_SIZE);

	/* send the authentication START packet body */
	ret = aaa_tac_plus_pkt_send(TAC_PLUS_ACCT, pkt, data_len+TAC_ACCT_REQ_FIXED_FIELDS_SIZE,
			psess->acct_port);
	aaa_pkt_dump(pkt, data_len+TAC_ACCT_REQ_FIXED_FIELDS_SIZE, AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	if(ret != AAA_OK)
	{
		zlog_err("%s: send body failed!\n", __FUNCTION__);
		return ret;
	}

    return AAA_OK;
}


int tac_acct_pkt_send(TACACS_SESSION_INFO *psess)
{
	int ret;
	//tac_acct_session_new_src_port(&pinfo->acct_port);
	tac_plus_session_src_port_set(psess, TAC_PLUS_ACCT);

	if (psess->acct_data_size > 1024)
	{
		zlog_err ("%s[%d] : tacacs acct pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}

	ret = aaa_tac_plus_pkt_send(TAC_PLUS_ACCT, (char *)&psess->acct_hdr, sizeof(psess->acct_hdr),
			psess->acct_port);
	aaa_pkt_dump((char *)&psess->acct_hdr, sizeof(psess->acct_hdr), AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	if(ret != AAA_OK)
	{
		zlog_err("%s: send header failed!\n", __FUNCTION__);
		return ret;
	}

	tac_acct_pkt_send_body(psess);
    return AAA_OK;
}

/* This function returns structure containing
    1. status (granted/denied)
    2. message for the user
    3. list of attributes returned by server
   The attributes should be applied to service acctization
   is requested for.
 *
 * return value:
 *   <  0 : error status code, see LIBTAC_STATUS_...
 *         LIBTAC_STATUS_READ_TIMEOUT
 *         LIBTAC_STATUS_SHORT_HDR
 *         LIBTAC_STATUS_SHORT_BODY
 *         LIBTAC_STATUS_PROTOCOL_ERR
 *   >= 0 : server response, see TAC_PLUS_ACCT_STATUS_...
 */
int tac_acct_rcv_process(TAC_HDR *th, void *body, struct areply *re)
{
    struct acct_reply *tb = NULL;
    size_t len_from_header, len_from_body;
    char *msg = NULL;

	if((NULL == th) || (NULL == body) || (NULL == re))
		return AAA_ERROR;
	re->attr = NULL; /* unused */
	re->msg = NULL;
	bzero(re, sizeof(struct areply));
	
	USER_AUTHING_INFO *pinfo_authing = NULL;
	USER_ONLINE_INFO *pinfo_online = NULL;
	
	pinfo_authing = tac_plus_authing_user_get_by_session_id(th->session_id, TAC_PLUS_ACCT);
	if(NULL == pinfo_authing)
	{
		pinfo_online = tac_plus_online_user_get_by_session_id(th->session_id, TAC_PLUS_ACCT);
		if(NULL == pinfo_online)
			return AAA_ERROR;
		else
		{
			len_from_header = ntohl(th->datalength);
			if (len_from_header > TAC_PLUS_MAX_PACKET_SIZE)
			{        
				zlog_err("%s: length declared in the packet %zu exceeds max packet size %d\n",\
					__FUNCTION__, len_from_header, TAC_PLUS_MAX_PACKET_SIZE);
				re->status = LIBTAC_STATUS_PROTOCOL_ERR;				
				return re->status;
			}	
		        
		    tb=(struct acct_reply *) xcalloc(1, len_from_header);
			/* get reply packet body */	
			memcpy(tb, body, len_from_header);

		    /* decrypt the body */
		    _tac_crypt( (char *)tb, th, len_from_header);

		    /* Convert network byte order to host byte order */
		    tb->msg_len  = ntohs(tb->msg_len);
		    tb->data_len = ntohs(tb->data_len);

		    /* check the length fields */
		    len_from_body = TAC_ACCT_REPLY_FIXED_FIELDS_SIZE + 
		    	tb->msg_len + tb->data_len;

		    if(len_from_header != len_from_body) {
		        zlog_err("%s: inconsistent reply body, incorrect key?\n", __FUNCTION__);
		        re->status = LIBTAC_STATUS_PROTOCOL_ERR;
		        free(tb);
		        return re->status;
		    }

		    /* save status and clean up */
		    if(tb->msg_len) {
		        msg=(char *) xcalloc(1, tb->msg_len+1);
		        bcopy( tb+TAC_ACCT_REPLY_FIXED_FIELDS_SIZE, msg, tb->msg_len);
		        msg[(int)tb->msg_len] = '\0';
		        re->msg = msg;      /* Freed by caller */
		    }

			zlog_debug(0, "%s[%d] : tacacs user[%s][%x] acct success\n", __func__, __LINE__,
				pinfo_online->user_info.username, pinfo_online->user_id);
			
			pinfo_online->session_info.tacacs_user.acct_timeleft = TAC_PLUS_RSP_TIMEOUT;
			pinfo_online->session_info.tacacs_user.acct_update_time = tac_plus_acct_update_interval_get();
			pinfo_online->session_info.tacacs_user.acct_send_flag = FALSE;
			
		    /* server logged our request successfully */
		    if (tb->status == TAC_PLUS_ACCT_STATUS_SUCCESS) {
				if(TAC_PLUS_ACCT_FLAG_START == pinfo_online->session_info.tacacs_user.acct_pkt.flags)
					pinfo_online->session_info.tacacs_user.acct_pkt.flags = TAC_PLUS_ACCT_FLAG_WATCHDOG;
				
		        zlog_debug(0, "%s[%d]->%s : account success\n", __FILE__, __LINE__, __func__);
		        if (!re->msg) re->msg = xstrdup("account success");
		        re->status = tb->status;
				
				//pinfo->acct_hdr.seq_no += 2;
				(*(pinfo_online->session_info.tacacs_user.acct_data + strlen(pinfo_online->session_info.tacacs_user.acct_data) - 1))++;		
				
		        free(tb);
		        return re->status;
		    }

		    zlog_err("%s[%d]->%s : accounting failed, server reply status=%d\n",\
		        __FILE__, __LINE__, __func__, tb->status);
		    switch(tb->status) {
		        case TAC_PLUS_ACCT_STATUS_FOLLOW:
		            re->status = tb->status;
		            if (!re->msg) re->msg=xstrdup("account failed");
					zlog_err("%s[%d]->%s : account failed\n", __FILE__, __LINE__, __func__);
					
		            break;

		        case TAC_PLUS_ACCT_STATUS_ERROR:
		        default:
		            re->status = tb->status;
		            if (!re->msg) re->msg=xstrdup("account err");
					zlog_err("%s[%d]->%s : account failed\n", __FILE__, __LINE__, __func__);
					
		            break;
		    }
		}
	}
	else
	{
		len_from_header = ntohl(th->datalength);
		if (len_from_header > TAC_PLUS_MAX_PACKET_SIZE)
		{        
			zlog_err("%s: length declared in the packet %zu exceeds max packet size %d\n",\
				__FUNCTION__, len_from_header, TAC_PLUS_MAX_PACKET_SIZE);
			re->status = LIBTAC_STATUS_PROTOCOL_ERR;
			pinfo_authing->aaa_result = ACCT_FAILED;
			return re->status;
		}	
	        
	    tb=(struct acct_reply *) xcalloc(1, len_from_header);
		/* get reply packet body */	
		memcpy(tb, body, len_from_header);

	    /* decrypt the body */
	    _tac_crypt( (char *)tb, th, len_from_header);

	    /* Convert network byte order to host byte order */
	    tb->msg_len  = ntohs(tb->msg_len);
	    tb->data_len = ntohs(tb->data_len);

	    /* check the length fields */
	    len_from_body = TAC_ACCT_REPLY_FIXED_FIELDS_SIZE + 
	    	tb->msg_len + tb->data_len;

	    if(len_from_header != len_from_body) {
	        zlog_err("%s: inconsistent reply body, incorrect key?\n", __FUNCTION__);
	        re->status = LIBTAC_STATUS_PROTOCOL_ERR;
			pinfo_authing->aaa_result = ACCT_FAILED;
	        free(tb);
	        return re->status;
	    }

	    /* save status and clean up */
	    if(tb->msg_len) {
	        msg=(char *) xcalloc(1, tb->msg_len+1);
	        bcopy( tb+TAC_ACCT_REPLY_FIXED_FIELDS_SIZE, msg, tb->msg_len);
	        msg[(int)tb->msg_len] = '\0';
	        re->msg = msg;      /* Freed by caller */
	    }

		zlog_debug(0, "%s[%d] : tacacs user[%s][%x] acct success\n", __func__, __LINE__,
				pinfo_authing->user_info.username, pinfo_authing->user_id);
		
		pinfo_authing->session_info.tacacs_user.acct_timeleft = TAC_PLUS_RSP_TIMEOUT;
		pinfo_authing->session_info.tacacs_user.acct_update_time = tac_plus_acct_update_interval_get();
	    /* server logged our request successfully */
	    if (tb->status == TAC_PLUS_ACCT_STATUS_SUCCESS) {
						
	        zlog_debug(0, "%s[%d]->%s : account success\n", __FILE__, __LINE__, __func__);
	        if (!re->msg) re->msg = xstrdup("account success");
	        re->status = tb->status;
			
			pinfo_authing->aaa_result = ACCT_SUCCESS;
			//pinfo->acct_hdr.seq_no += 2;
			(*(pinfo_authing->session_info.tacacs_user.acct_data + strlen(pinfo_authing->session_info.tacacs_user.acct_data) - 1))++;		
			
	        free(tb);
	        return re->status;
	    }

	    zlog_err("%s[%d]->%s : accounting failed, server reply status=%d\n",\
	        __FILE__, __LINE__, __func__, tb->status);
	    switch(tb->status) {
	        case TAC_PLUS_ACCT_STATUS_FOLLOW:
	            re->status = tb->status;
	            if (!re->msg) re->msg=xstrdup("account failed");
				zlog_err("%s[%d]->%s : account failed\n", __FILE__, __LINE__, __func__);
				pinfo_authing->aaa_result = ACCT_FAILED;
	            break;

	        case TAC_PLUS_ACCT_STATUS_ERROR:
	        default:
	            re->status = tb->status;
	            if (!re->msg) re->msg=xstrdup("account err");
				zlog_err("%s[%d]->%s : account failed\n", __FILE__, __LINE__, __func__);
				pinfo_authing->aaa_result = ACCT_FAILED;
	            break;
	    }
	}	

    free(tb);
    return re->status;
}

#if 0
void tac_acct_timeout_check_start(tac_sess_recorder *pinfo)
{
	pinfo->acct_timeleft = tac_plus_response_timeout_get();
}

int tac_acct_timeout_check(struct thread *t)
{	
	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	tac_sess_recorder *pinfo = NULL;
	
	if(tac_session_table.num_entries == 0)
		goto out;
	
	for((node) = hios_hash_start(&tac_session_table, (void **)&cursor); node; (node) = hios_hash_next(&tac_session_table, (void **)&cursor))		
	{
		if(NULL == (pinfo = (tac_sess_recorder *)(node->data)))
			return 0;

		if(pinfo->acct_timeleft < 0)
			pinfo->acct_timeleft = 0;
		else if(pinfo->acct_timeleft > 0)
		{
			pinfo->acct_timeleft--;
			if(0 == pinfo->acct_timeleft)
			{
				zlog_debug(0, "%s : user %s account timeout\n", __func__, pinfo->username);
				pinfo->acct_result = ACCT_TIMEOUT;
			}
		}

		/* send update acct packte */
		if(ENABLE == tac_plus_acct_server_switch_get())
		{
			if(1 == pinfo->acct_update_inteval)
			{
				tac_acct_pkt_send(pinfo);
			}
			
			if(0 == pinfo->acct_update_inteval)
				pinfo->acct_update_inteval = tac_plus_acct_update_interval_get();
			else
				pinfo->acct_update_inteval--;
		}
	}

out:
	thread_add_timer(aaa_master, tac_acct_timeout_check, NULL, 1);
	return 1;
}

void tac_acct_timeout_check_stop(tac_sess_recorder *pinfo)
{
	pinfo->acct_timeleft = 0;
}
#endif

//accounting stop packet send
int tac_acct_send_stop(TACACS_SESSION_INFO *psess)
{
	psess->acct_pkt.flags = TAC_PLUS_ACCT_FLAG_STOP;
	tac_acct_pkt_send(psess);
	//tac_plus_session_del(pinfo->username);

	return AAA_OK;
}



