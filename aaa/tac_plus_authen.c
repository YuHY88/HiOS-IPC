/* authen_s.c - Send authentication request to the server.
 * authen_r.c - Read authentication reply from server.
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
 * modified by luoyz @ 2016.9.27
 */

//#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include "lib/memory.h"
#include "lib/memtypes.h"
#include "lib/aaa_common.h"

#include "aaa_config.h"
#include "aaa_user_manager.h"
#include "aaa_tac_plus.h"
#include "xalloc.h"
#include "tacplus.h"
#include "aaa_packet.h"



#if defined(HAVE_OPENSSL_MD5_H) && defined(HAVE_LIBCRYPTO)
# include <openssl/md5.h>
#else
#include "md5.h"
#endif

extern struct thread_master *aaa_master;


int tac_record_authen_info(USER_AUTHING_INFO *pinfo, const char *rem_addr, char action, char authen_type)
{
	uint32_t  user_len, port_len, token_len, rem_addr_len, body_len, data_len;
	uint32_t  chal_len, mdp_len;
	//int  pkt_len = 0;
	char const *chal = "1234123412341234";
	char digest[MD5_DIGEST_LENGTH];
	char *token = NULL;
	char *mdp = NULL;
	MD5_CTX mdcontext;
	//char pkt[TAC_PLUS_SEND_PKT_LEN_MAX];
	int ret;
	char *pass = pinfo->user_info.password;

	/* record authen header */
	ret = _tac_req_header(TAC_PLUS_AUTHEN, 0, &pinfo->session_info.tacacs_user.authen_hdr);	//fill session_id,src_port,type
	if(ret != AAA_OK)
	{
		//XFREE(MTYPE_AAA, pinfo);
		return ret;
	}

	if (authen_type != TAC_PLUS_AUTHEN_TYPE_ASCII)	//fill version
    {
        pinfo->session_info.tacacs_user.authen_hdr.version = TAC_PLUS_VER_1;
    }

	pinfo->session_info.tacacs_user.authen_hdr.encryption = 
		tac_encryption ? TAC_PLUS_ENCRYPTED_FLAG : TAC_PLUS_UNENCRYPTED_FLAG;	//fill flag

	/* fill authen src port */
	tac_plus_session_src_port_set(&pinfo->session_info.tacacs_user, TAC_PLUS_AUTHEN);
		
	/* if CHAP , md5 */
	if (TAC_PLUS_AUTHEN_TYPE_CHAP == authen_type && pass != NULL)	
	{
        chal_len = strlen(chal);
        mdp_len = sizeof(char) + strlen(pass) + chal_len;
        mdp = (char *)xcalloc(1, mdp_len);
        mdp[0] = 5;
        memcpy(&mdp[1], pass, strlen(pass));
        memcpy(mdp + strlen(pass) + 1, chal, chal_len);
#if defined(HAVE_OPENSSL_MD5_H) && defined(HAVE_LIBCRYPTO)
		MD5_Init(&mdcontext);
		MD5_Update(&mdcontext, mdp, mdp_len);
		MD5_Final((char *) digest, &mdcontext);
#else
		MD5Init(&mdcontext);
		MD5Update(&mdcontext, mdp, mdp_len);
		MD5Final((uint8_t *) digest, &mdcontext);
#endif
        free(mdp);
        token = (char*)xcalloc(1, sizeof(char) + 1 + chal_len + MD5_DIGEST_LENGTH);
        token[0] = 5;
        memcpy(&token[1], chal, chal_len);
        memcpy(token + chal_len + 1, digest, MD5_DIGEST_LENGTH);
    }
	else	//PAP mode, doesnot need md5
	{
		token = xstrdup(pinfo->user_info.password);
    }

    /* get size of submitted data */
    user_len = pinfo->user_info.username != NULL ? strlen(pinfo->user_info.username) : 0;
    port_len = pinfo->user_info.ttyname != NULL ? strlen(pinfo->user_info.ttyname) : 0;
    rem_addr_len = rem_addr != NULL ? strlen(rem_addr) : 0;
    token_len = token != NULL ? strlen(token) : 0;

	/* fill the body of message */
	pinfo->session_info.tacacs_user.authen_pkt.action = action;
	pinfo->session_info.tacacs_user.authen_pkt.priv_lvl = TAC_PLUS_PRIV_LVL_MIN; /* can be ignored */
    pinfo->session_info.tacacs_user.authen_pkt.authen_type = authen_type;
	if(TAC_PLUS_AUTHEN_TYPE_PAP == pinfo->session_info.tacacs_user.authen_pkt.authen_type)
	{
		if(NULL == pinfo->user_info.username || NULL == pinfo->user_info.password)
		{
			zlog_err("%s: required username and password!\n",	__FUNCTION__);
			return AAA_FAIL;
		}
	}
	
	pinfo->session_info.tacacs_user.authen_pkt.service = TAC_PLUS_AUTHEN_SVC_NONE;
    pinfo->session_info.tacacs_user.authen_pkt.user_len = user_len;
    pinfo->session_info.tacacs_user.authen_pkt.port_len = port_len;
    pinfo->session_info.tacacs_user.authen_pkt.rem_addr_len = rem_addr_len;  /* may be e.g Caller-ID in future */
    pinfo->session_info.tacacs_user.authen_pkt.data_len = token_len;
	data_len = user_len + port_len + rem_addr_len + token_len;
	/* fill body length in header */
    body_len = sizeof(pinfo->session_info.tacacs_user.authen_pkt) + user_len + port_len + rem_addr_len + token_len;
    pinfo->session_info.tacacs_user.authen_hdr.datalength = htonl(body_len);	

	/* build the packet */
	if((sizeof(pinfo->session_info.tacacs_user.authen_hdr) + body_len) > TAC_PLUS_SEND_PKT_LEN_MAX)
	{
		zlog_err("%s: packet too long, limit: %d\n", __FUNCTION__, TAC_PLUS_SEND_PKT_LEN_MAX);
		return AAA_FAIL;
	}

	if (data_len > 1024)
	{
		zlog_err ("%s[%d] : tacacs authen pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}
	pinfo->session_info.tacacs_user.authen_data_size = data_len;
	memset(pinfo->session_info.tacacs_user.authen_data, 0, 1024);
	data_len = 0;
	
	//memset(pkt, 0, sizeof(pkt));
	
	if(pinfo->user_info.username != NULL)
	{
		//bcopy(user, pkt + pkt_len, user_len);  /* user */
		memcpy(pinfo->session_info.tacacs_user.authen_data, pinfo->user_info.username, user_len);
		data_len += user_len;
	}
	if(pinfo->user_info.ttyname != NULL)
	{
		memcpy(pinfo->session_info.tacacs_user.authen_data + data_len, pinfo->user_info.ttyname, port_len);
		data_len += port_len;
	}
	if(rem_addr != NULL)
	{
		memcpy(pinfo->session_info.tacacs_user.authen_data + data_len, rem_addr, rem_addr_len);
		data_len += rem_addr_len;
	}
	if(token != NULL)
	{
		memcpy(pinfo->session_info.tacacs_user.authen_data + data_len, token, token_len);
		data_len += token_len;
		memset(pinfo->user_info.password, 0, USER_PASSWORD_MAX);
		memcpy(pinfo->user_info.password, token, token_len);
	}

	/* check pkt_len */
    if ((data_len + sizeof(pinfo->session_info.tacacs_user.authen_pkt)) != body_len)
	{
        zlog_err("%s: body_len %d, pkt_len %d error\n", __FUNCTION__, body_len,\
			(data_len + sizeof(pinfo->session_info.tacacs_user.authen_pkt)));
        free(token);
        return AAA_FAIL;
    }	
	
	free(token);
	return AAA_OK;
}



int tac_authen_pkt_send(TACACS_SESSION_INFO *psess)
{	
	int  ret = 0;
	char pkt[TAC_PLUS_SEND_PKT_LEN_MAX];
	int data_len = 0;
	int body_len = 0;	

	tac_plus_session_src_port_set(psess, TAC_PLUS_AUTHEN);

	if (psess->authen_data_size > 1024)
	{
		zlog_err ("%s[%d] : tacacs authen pkt is too long\n", __func__, __LINE__);
		return AAA_ERROR;
	}
	
	/* send the TACACS+ packet header */
	aaa_tac_plus_pkt_send(TAC_PLUS_AUTHEN, (char *)&psess->authen_hdr, sizeof(psess->authen_hdr),
		psess->authen_port);
	aaa_pkt_dump((char *)&psess->authen_hdr, sizeof(psess->authen_hdr), AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	
	/* send the authentication START packet body */
	memset(pkt, 0, sizeof(pkt));
	memcpy(pkt, &psess->authen_pkt, sizeof(psess->authen_pkt));
	data_len = (psess->authen_pkt.user_len+psess->authen_pkt.port_len+psess->authen_pkt.rem_addr_len+psess->authen_pkt.data_len);

	memcpy(pkt + sizeof(psess->authen_pkt), psess->authen_data, data_len);
	body_len = data_len + sizeof(psess->authen_pkt);
	_tac_crypt(pkt, &psess->authen_hdr, body_len);
	
 	ret = aaa_tac_plus_pkt_send(TAC_PLUS_AUTHEN, pkt, body_len, psess->authen_port);
	aaa_pkt_dump(pkt, body_len, AAA_PKT_SEND, AAA_TAC_PLUS, __FUNCTION__);
	
	if(ret != AAA_OK)
	{
		zlog_err("%s: send body failed!\n", __FUNCTION__);
		//XFREE(MTYPE_AAA, psess);
		return ret;
	}
	
	return AAA_OK;
}

/* reads packet from TACACS+ server; returns:
 *  TAC_PLUS_AUTHEN_STATUS_PASS if the authentication succeded
 *  an other integer if failed. Check tacplus.h for all possible values
 *
 * return value:
 *   <  0 : error status code, see LIBTAC_STATUS_...
 *         LIBTAC_STATUS_READ_TIMEOUT
 *         LIBTAC_STATUS_SHORT_HDR
 *         LIBTAC_STATUS_SHORT_BODY
 *         LIBTAC_STATUS_PROTOCOL_ERR
 *   >= 0 : server response, see TAC_PLUS_AUTHEN_STATUS_...
 */
int tac_authen_rcv_process(TAC_HDR *th, void *body, struct areply *re)
{
	struct authen_reply *tb = NULL;
	size_t len_from_header, len_from_body;
	char *msg = NULL;
	//char *username = NULL;	

	if(NULL == th || NULL == body || NULL == re)
		return AAA_ERROR; /* should not happen */

	USER_AUTHING_INFO *pinfo = tac_plus_authing_user_get_by_session_id(th->session_id, TAC_PLUS_AUTHEN);
	
	if(NULL == pinfo)
	{
		zlog_debug(AAA_DBG_TACACS, "%s[%d] : cannot find authing user\n", __func__, __LINE__);
		return AAA_ERROR;
	}		
	
	len_from_header = ntohl(th->datalength);
	if (len_from_header > TAC_PLUS_MAX_PACKET_SIZE)
	{
		zlog_err("%s: length declared in the packet %zu exceeds max packet size %d\n",
			__FUNCTION__, len_from_header, TAC_PLUS_MAX_PACKET_SIZE);
		re->status = LIBTAC_STATUS_PROTOCOL_ERR;
		pinfo->aaa_result = AUTHEN_FAILED;
		return re->status;
	}
	tb = (struct authen_reply *)xcalloc(1, len_from_header);

	/* get reply packet body */
	memcpy(tb, body, len_from_header);

	/* decrypt the body */
	_tac_crypt((char *) tb, th, len_from_header);

	/* Convert network byte order to host byte order */
	tb->msg_len = ntohs(tb->msg_len);
	tb->data_len = ntohs(tb->data_len);

	//pkt_dump(tb, len_from_header);
	/* check the length fields */
	len_from_body = sizeof(tb->status) + sizeof(tb->flags) + sizeof(tb->msg_len)
			+ sizeof(tb->data_len) + tb->msg_len + tb->data_len;

	if (len_from_header != len_from_body)
	{
		zlog_err("%s: inconsistent reply body, incorrect key?\n", __FUNCTION__);
		re->msg = xstrdup(PROTOCOL_ERR_MSG);
		re->status = LIBTAC_STATUS_PROTOCOL_ERR;
		free(tb);
		pinfo->aaa_result = AUTHEN_FAILED;
		return re->status;
	}

	/* response to vtysh */
	if(TAC_PLUS_AUTHEN_STATUS_PASS == tb->status)	//authen success
	{
		pinfo->aaa_result = AUTHEN_SUCCESS;
		zlog_debug(AAA_DBG_TACACS, "%s[%d]->%s : authen success\n", __FILE__, __LINE__, __func__);		
	}
	else
	{
		pinfo->aaa_result = AUTHEN_FAILED;
		zlog_debug(AAA_DBG_TACACS, "%s[%d]->%s : authen failed\n", __FILE__, __LINE__, __func__);
	}	
	
	/* save status and clean up */
	re->status = tb->status;

	if (tb->msg_len > 0)
	{
		msg = (char *)xcalloc(tb->msg_len + 1, sizeof(char));
		memcpy(msg, (char*) tb + sizeof(struct authen_reply), tb->msg_len);
		re->msg = msg;
	}

	switch(re->status)
	{
		case TAC_PLUS_AUTHEN_STATUS_PASS:
			/* server authenticated username and password successfully */
			zlog_debug(AAA_DBG_TACACS, "%s: authentication ok\n", __FUNCTION__);
			break;

		case TAC_PLUS_AUTHEN_STATUS_GETPASS:
			/* server ask for continue packet with password */
			zlog_debug(AAA_DBG_TACACS, "%s: continue packet with password needed\n", __FUNCTION__);
			break;

		case TAC_PLUS_AUTHEN_STATUS_GETDATA:
			/* server wants to prompt user for more data */
			re->flags = tb->flags;
			zlog_debug(AAA_DBG_TACACS, "%s: continue packet with data request: msg=%.*s\n",
				__FUNCTION__, tb->msg_len, (char*)tb + sizeof(struct authen_reply));
			break;

		default:
			zlog_debug(AAA_DBG_TACACS, "%s: authentication failed, server reply status=%d\n",
					__FUNCTION__, re->status);
			break;
	}

	free(tb);
	return re->status;
} /* tac_authen_rcv_process */


#if 0

void tac_authen_timeout_check_start(tac_sess_recorder *pinfo)
{
	pinfo->authen_timeleft = tac_plus_response_timeout_get();
}

int tac_authen_timeout_check(struct thread *t)
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

		if(pinfo->authen_timeleft < 0)
			pinfo->authen_timeleft = 0;
		else if(pinfo->authen_timeleft > 0)
		{
			pinfo->authen_timeleft--;
			if(0 == pinfo->authen_timeleft)
			{
				zlog_debug(AAA_DBG_TACACS, "%s : user %s authenticate timeout\n", __func__, pinfo->username);
				pinfo->authen_result = AUTH_TIMEOUT;
				if(USER_LOGIN == pinfo->login_mode)
				{
					aaa_auth_response_to_vty(pinfo ? pinfo->username : NULL, AUTH_TIMEOUT, 0);
					tac_plus_session_del(pinfo->username);
				}
			}
		}
	}

out:
	thread_add_timer(aaa_master, tac_authen_timeout_check, NULL, 1);
	return 1;
}

void tac_authen_timeout_check_stop(tac_sess_recorder *pinfo)
{
	pinfo->authen_timeleft = 0;
}

#endif

