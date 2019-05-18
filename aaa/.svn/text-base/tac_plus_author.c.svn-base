/* author_s.c - Send authorization request to the server.
 * author_r.c - Reads authorization reply from the server.
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
 * modified by lipf @ 2017.5.9
 */

#include <string.h>

#include "aaa_tac_plus.h"
#include "xalloc.h"
#include "tacplus.h"
#include "aaa_packet.h"
#include "lib/memory.h"
#include "lib/memtypes.h"


extern struct thread_master *aaa_master;

/* Send authorization request to the server, along with attributes
   specified in attribute list prepared with tac_add_attrib.
 *
 * return value:
 *      0 : success
 *   <  0 : error status code, see LIBTAC_STATUS_...
 *         LIBTAC_STATUS_WRITE_ERR
 *         LIBTAC_STATUS_WRITE_TIMEOUT (pending impl)
 *         LIBTAC_STATUS_ASSEMBLY_ERR  (pending impl)
 */

int tac_record_author_info(USER_AUTHING_INFO *pinfo, const char *r_addr)
{
	char user_len, port_len, r_addr_len;
	int i = 0;          /* attributes count */
	struct tac_attrib *a;
	int data_len = 0;
	int len = 0;

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
	
	_tac_req_header(TAC_PLUS_AUTHOR, 0, &pinfo->session_info.tacacs_user.author_hdr);
	/* fill authen src port */
	tac_plus_session_src_port_set(&pinfo->session_info.tacacs_user, TAC_PLUS_AUTHOR);
	
	/* set header options */
    pinfo->session_info.tacacs_user.author_hdr.version = TAC_PLUS_VER_0;
    pinfo->session_info.tacacs_user.author_hdr.encryption = tac_encryption ? TAC_PLUS_ENCRYPTED_FLAG : TAC_PLUS_UNENCRYPTED_FLAG;

    zlog_debug(AAA_DBG_TACACS, "%s: user '%s', tty '%s', rem_addr '%s', encrypt: %s\n", \
        __FUNCTION__, pinfo->user_info.username, \
        pinfo->user_info.ttyname, r_addr, tac_encryption ? "yes" : "no");
    
	user_len = pinfo->user_info.username != NULL ? (char)strlen(pinfo->user_info.username) : 0;
    port_len = pinfo->user_info.ttyname != NULL ? (char)strlen(pinfo->user_info.ttyname) : 0;
    r_addr_len = r_addr != NULL ? (char)strlen(r_addr) : 0;
	
    pinfo->session_info.tacacs_user.author_pkt.authen_method = TAC_PLUS_AUTHEN_METH_TACACSPLUS;
    pinfo->session_info.tacacs_user.author_pkt.priv_lvl = TAC_PLUS_PRIV_LVL_USER;//tac_priv_lvl;
    pinfo->session_info.tacacs_user.author_pkt.authen_type = TAC_PLUS_AUTHEN_TYPE_PAP;
    pinfo->session_info.tacacs_user.author_pkt.service = TAC_PLUS_AUTHEN_SVC_NONE;
    pinfo->session_info.tacacs_user.author_pkt.user_len = user_len;
    pinfo->session_info.tacacs_user.author_pkt.port_len = port_len;
    pinfo->session_info.tacacs_user.author_pkt.r_addr_len = r_addr_len;

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
	pinfo->session_info.tacacs_user.author_pkt.arg_cnt = i;

	if (data_len > 1024)
	{
		zlog_err ("%s[%d] : tacacs author pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}
	pinfo->session_info.tacacs_user.author_data_size = data_len;
	memset (pinfo->session_info.tacacs_user.author_data, 0, 1024);

	/* record all arg len */
	a = author_attr;
	len = 0;
	while(a)
	{
		memcpy(pinfo->session_info.tacacs_user.author_data + len, &a->attr_len, sizeof(a->attr_len));
		len += sizeof(a->attr_len);
		a = a->next;
	}

	/* record all arg val */
	if(NULL != pinfo->user_info.username)
	{
		memcpy(pinfo->session_info.tacacs_user.author_data + len, 
			pinfo->user_info.username, strlen(pinfo->user_info.username));
		len += strlen(pinfo->user_info.username);
	}
	if(NULL != pinfo->user_info.ttyname)
	{
		memcpy(pinfo->session_info.tacacs_user.author_data + len, 
			pinfo->user_info.ttyname, strlen(pinfo->user_info.ttyname));
		len += strlen(pinfo->user_info.ttyname);
	}
	if(NULL != r_addr)
	{
		memcpy(pinfo->session_info.tacacs_user.author_data + len, r_addr, strlen(r_addr));
		len += strlen(r_addr);
	}	
	
	a = author_attr;
	while(a)
	{
		memcpy (pinfo->session_info.tacacs_user.author_data + len, a->attr, a->attr_len);
		len += a->attr_len;		
		a = a->next;
	}

	pinfo->session_info.tacacs_user.author_hdr.datalength = htonl(TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE + data_len);

	/*a = author_attr;
	while (a)
	{
		XFREE (MTYPE_AAA, a->attr);
		a = a->next;
	}

	a = author_attr;
	struct tac_attrib *a_temp1 = NULL;
	struct tac_attrib *a_temp2 = NULL;
	while (a)
	{
		a_temp1 = a;
		while (a_temp1)
		{
			a_temp2 = a_temp1;
			a_temp1 = a_temp1->next;
		}
		if (a_temp2 == a)
			a = NULL;
		
		XFREE (MTYPE_AAA, a_temp2);
	}
	author_attr = NULL;*/

	if (author_attr->attr)
		free (author_attr->attr);
	if (author_attr->next->attr)
		free (author_attr->next->attr);
	if (author_attr->next->next->attr)
		free (author_attr->next->next->attr);
	if (author_attr->next->next)
		XFREE(MTYPE_AAA, author_attr->next->next);
	if (author_attr->next)
		XFREE(MTYPE_AAA, author_attr->next);
	if (author_attr)
		XFREE(MTYPE_AAA, author_attr);
	return AAA_OK;
}

int tac_author_pkt_send(TACACS_SESSION_INFO *psess)
{
	int ret;
	uint32_t data_len = 0;
	char pkt[TAC_PLUS_SEND_PKT_LEN_MAX];
	memset(pkt, 0, TAC_PLUS_SEND_PKT_LEN_MAX);

	tac_plus_session_src_port_set(psess, TAC_PLUS_AUTHOR);

	if (psess->author_data_size > 1024)
	{
		zlog_err ("%s[%d] : tacacs author pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}
	
	ret = aaa_tac_plus_pkt_send(TAC_PLUS_AUTHOR, (char *)(&psess->author_hdr), sizeof(psess->author_hdr),
							psess->author_port);
	aaa_pkt_dump((char *)&psess->author_hdr, sizeof(psess->author_hdr), AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	if(ret != AAA_OK)
	{
		zlog_err("%s: send header failed!\n", __FUNCTION__);
		return ret;
	}

	data_len = psess->author_pkt.user_len + psess->author_pkt.port_len + psess->author_pkt.r_addr_len;
	for(ret = 0;ret < psess->author_pkt.arg_cnt;ret++)
	{
		data_len += sizeof(char);
		data_len += psess->author_data[ret];
	}		

	if((data_len+TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE) != (uint32_t)ntohl(psess->author_hdr.datalength))
	{
		zlog_err("%s: author_data len error!\n", __FUNCTION__);
		return AAA_ERROR;
	}
		 
	memcpy(pkt, &psess->author_pkt, sizeof(psess->author_pkt));
	memcpy(pkt+sizeof(psess->author_pkt), psess->author_data, data_len);
    
    /* encrypt packet body  */
    _tac_crypt(pkt, &psess->author_hdr, data_len+TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE);

	/* send the authorize START packet body */
	ret = aaa_tac_plus_pkt_send(TAC_PLUS_AUTHOR, pkt, data_len+TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE,
							psess->author_port);
	aaa_pkt_dump(pkt, data_len+TAC_AUTHOR_REQ_FIXED_FIELDS_SIZE, AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	if(ret != AAA_OK)
	{
		zlog_err("%s: send body failed!\n", __FUNCTION__);
		return ret;
	}
	
    return AAA_OK;
}


/* This function returns structure containing
    1. status (granted/denied)
    2. message for the user
    3. list of attributes returned by server
   The attributes should be applied to service authorization
   is requested for.
 *
 * return value:
 *   <  0 : error status code, see LIBTAC_STATUS_...
 *         LIBTAC_STATUS_READ_TIMEOUT
 *         LIBTAC_STATUS_SHORT_HDR
 *         LIBTAC_STATUS_SHORT_BODY
 *         LIBTAC_STATUS_PROTOCOL_ERR
 *   >= 0 : server response, see TAC_PLUS_AUTHOR_STATUS_...
 */
int tac_author_rcv_process(TAC_HDR *th, void *body, struct areply *re)
{
	struct author_reply *tb = NULL;
	size_t len_from_header, len_from_body;
	size_t packet_read;
	char *pktp = NULL;
	unsigned int r = 0;
	int level = 15;

	if(NULL == th || NULL == body || NULL == re)
		return AAA_ERROR; /* should not happen */

	re->msg = NULL;
	USER_AUTHING_INFO *pinfo = tac_plus_authing_user_get_by_session_id(th->session_id, TAC_PLUS_AUTHOR);
	if(NULL == pinfo)
	{
		zlog_debug(AAA_DBG_TACACS, "%s[%d] -> %s : pinfo = %p\n", __FILE__, __LINE__, __func__, pinfo);
		return AAA_ERROR;
	}
		
    bzero(re, sizeof(struct areply));

    len_from_header = ntohl(th->datalength);
    if (len_from_header > TAC_PLUS_MAX_PACKET_SIZE)
	{
        zlog_err("%s: length declared in the packet %zu exceeds max packet size %d\n",\
            __FUNCTION__, len_from_header, TAC_PLUS_MAX_PACKET_SIZE);
        re->status = LIBTAC_STATUS_PROTOCOL_ERR;

		pinfo->user_info.level_author = 0;
		pinfo->aaa_result = AUTHOR_FAILED;
		
		return re->status;
    }
    tb = (struct author_reply *) xcalloc(1, len_from_header);
	/* get reply packet body */
	memcpy(tb, body, len_from_header);
	packet_read = len_from_header;

    /* decrypt the body */
    _tac_crypt((char *) tb, th, len_from_header);

    /* Convert network byte order to host byte order */
    tb->msg_len  = ntohs(tb->msg_len);
    tb->data_len = ntohs(tb->data_len);

    /* check consistency of the reply body
     * len_from_header = declared in header
     * len_from_body = value computed from body fields
     */
    len_from_body = TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE +
        tb->msg_len + tb->data_len;

    pktp = (char *) tb + TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE;

    /* cycle through the arguments supplied in the packet */
    for (r = 0; r < tb->arg_cnt && r < TAC_PLUS_MAX_ARGCOUNT; r++) {
        if ((len_from_body > packet_read) || ((size_t)((char *)pktp - (char *)tb) > packet_read)) {
            zlog_err("%s: arguments supplied in packet seem to exceed its size\n", __FUNCTION__);
            re->msg = xstrdup(PROTOCOL_ERR_MSG);
            re->status = LIBTAC_STATUS_PROTOCOL_ERR;

			pinfo->user_info.level_author = 0;
			pinfo->aaa_result = AUTHOR_FAILED;
			
            free(tb);
            return re->status;
        }
        len_from_body += sizeof(char); /* add arg length field's size*/
        len_from_body += *pktp; /* add arg length itself */
        pktp++;
    }

    if(len_from_header != len_from_body) {
        zlog_err("%s: inconsistent reply body, incorrect key?\n", __FUNCTION__);
        re->msg = xstrdup(PROTOCOL_ERR_MSG);
        re->status = LIBTAC_STATUS_PROTOCOL_ERR;

		pinfo->user_info.level_author = 0;
		pinfo->aaa_result = AUTHOR_FAILED;
		
        free(tb);
        return re->status;
    }

    /* packet seems to be consistent, prepare return messages */
    /* server message for user */
    if(tb->msg_len) {
        char *msg = (char *) xcalloc(1, tb->msg_len + 1);
        bcopy((char *) tb + TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE
            + (tb->arg_cnt)*sizeof(char),
            msg, tb->msg_len);
        msg[(int) tb->msg_len] = '\0';
        re->msg = msg;      /* freed by caller */
    }

    /* server message to syslog */
    if(tb->data_len) {
        char *smsg=(char *) xcalloc(1, tb->data_len+1);
        bcopy((char *) tb + TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE
            + (tb->arg_cnt)*sizeof(char)
            + tb->msg_len, smsg,
            tb->data_len);
        smsg[(int) tb->data_len] = '\0';
        zlog_err("%s: reply message: %s\n", __FUNCTION__,smsg);
        free(smsg);
    }

    zlog_debug(AAA_DBG_TACACS, "%s: authorization reply status=%d\n", __FUNCTION__, tb->status);

    /* prepare status */
    switch(tb->status) {
        /* success conditions */
        /* XXX support optional vs mandatory arguments */
        case TAC_PLUS_AUTHOR_STATUS_PASS_REPL:
            tac_free_attrib(&re->attr);

        case TAC_PLUS_AUTHOR_STATUS_PASS_ADD:		//author success
            {
                char *argp;

                if(!re->msg) re->msg = xstrdup(AUTHOR_OK_MSG);
                    re->status = tb->status;

                /* add attributes received to attribute list returned to
                   the client */
                pktp = (char *) tb + TAC_AUTHOR_REPLY_FIXED_FIELDS_SIZE;
                argp = pktp + (tb->arg_cnt * sizeof(char)) + tb->msg_len +
                    tb->data_len;
                zlog_debug(AAA_DBG_TACACS, "Args cnt %d\n", tb->arg_cnt);
                /* argp points to current argument string
                   pktp points to current argument length */
				
			for ( r = 0; r < tb->arg_cnt && r < TAC_PLUS_MAX_ARGCOUNT; r++)
			{
                char buff[256];
                char *sep;                
				char *value;
                char sepchar = '=';

                bcopy(argp, buff, (int)*pktp);	//copy arg1 to buff
                buff[(int)*pktp] = '\0';		//add "\0" to end
                sep = strchr(buff, '=');		//search "="
                
                if(sep == NULL) 				//if "=" not find , search "*"
				{
                    sep = strchr(buff, '*');
                }
                if(sep == NULL) 				//if "*" not find yet, arg error
				{
                    zlog_debug(AAA_DBG_TACACS, "AUTHOR_STATUS_PASS_ADD/REPL: av pair does not contain a separator: %s\n", buff);
                    /* now buff points to attribute name, make value ""
                       treat as "name=" */
                    //value = "";
                    value = NULL;
                }
				else
				{
                    sepchar = *sep;
                    *sep = '\0';
                    value = ++sep;
                    /* now buff points to attribute name,
                       value to the attribute value */
                }
                zlog_debug(AAA_DBG_TACACS, "Adding buf/value pair (%s,%s)\n", buff, value);
                tac_add_attrib_pair(&re->attr, buff, sepchar, value);
                argp += *pktp;
                pktp++;

				if(0 == strcmp(buff, "priv-lvl"))
					level = atoi(value);
            }
        }

		pinfo->user_info.level_author = level;
		pinfo->aaa_result = AUTHOR_SUCCESS;
				
        free(tb);
        return re->status;		
        break;
		
        /* authorization failure conditions */
        /* failing to follow is allowed by RFC, page 23  */
        case TAC_PLUS_AUTHOR_STATUS_FOLLOW:
        case TAC_PLUS_AUTHOR_STATUS_FAIL:
            if(!re->msg) re->msg = xstrdup(AUTHOR_FAIL_MSG);
            re->status = TAC_PLUS_AUTHOR_STATUS_FAIL;

			pinfo->user_info.level_author = 0;
			pinfo->aaa_result = AUTHOR_FAILED;
            break;
        /* error conditions */
        case TAC_PLUS_AUTHOR_STATUS_ERROR:
        default:
            if(!re->msg) re->msg = xstrdup(AUTHOR_ERR_MSG);
            re->status = TAC_PLUS_AUTHOR_STATUS_ERROR;

			pinfo->user_info.level_author = 0;
			pinfo->aaa_result = AUTHOR_FAILED;
			break;
    }

    free(tb);
    return re->status;	
}

#if 0

void tac_author_timeout_check_start(tac_sess_recorder *pinfo)
{
	pinfo->author_timeleft = tac_plus_response_timeout_get();
}

int tac_author_timeout_check(struct thread *t)
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

		if(pinfo->author_timeleft < 0)
			pinfo->author_timeleft = 0;
		else if(pinfo->author_timeleft > 0)
		{
			pinfo->author_timeleft--;
			if(0 == pinfo->author_timeleft)
			{
				zlog_debug(AAA_DBG_TACACS, "%s : user %s authorize timeout\n", __func__, pinfo->username);
				pinfo->author_result = AUTHOR_TIMEOUT;
				if(USER_LOGIN == pinfo->login_mode)
				{
					aaa_auth_response_to_vty(pinfo->username, AUTH_SUCCESS, 0);
					tac_plus_record_start_login_time(pinfo);
				}
				if(ENABLE == tac_plus_acct_server_switch_get())
					tac_acct_pkt_send(pinfo);
			}
		}
	}

out:
	thread_add_timer(aaa_master, tac_author_timeout_check, NULL, 1);
	return 1;
}

void tac_author_timeout_check_stop(tac_sess_recorder *pinfo)
{
	pinfo->author_timeleft = 0;
}

#endif

