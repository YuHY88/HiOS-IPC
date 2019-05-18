/******************************************************************************
 * Filename: aaa_radius.c
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

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <assert.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/md5.h>
#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <lib/pkt_buffer.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>

#include "aaa_cmd.h"
#include "aaa_radius.h"
#include "aaa_packet.h"
#include "aaa_radius_auth.h"
#include "aaa_radius_acct.h"
#include "radiusclient.h"
#include "aaa_config.h"


extern struct thread_master *aaa_master;


void aaa_radius_init(void)
{
	/* default server */
	g_aaa_cfgs.radius.auth_server.sin_addr.s_addr = ntohl(RADIUS_DEF_SEVR_IPV4);
	g_aaa_cfgs.radius.auth_server.sin_port = ntohs(RADIUS_AUTH_UDP_PORT);
	g_aaa_cfgs.radius.acct_server.sin_addr.s_addr = ntohl(RADIUS_DEF_SEVR_IPV4);
	g_aaa_cfgs.radius.acct_server.sin_port = ntohs(RADIUS_ACCT_UDP_PORT);

	/* account server default enable */
	g_aaa_cfgs.radius.acct_switch = ENABLE;
	
	/* default shared-secret */
	radius_key_set(RADIUS_DEF_SECRET);

	/* default timeout & auth_interval & max-retry */
	g_aaa_cfgs.radius.rsp_timeout = RADIUS_DEF_TIMEOUT;	
	g_aaa_cfgs.radius.retry_interval = RADIUS_DEF_INTERVAL;	//default interval time
	g_aaa_cfgs.radius.retry_times = RADIUS_DEF_MAX_RETRY;	//default max-retry times
	g_aaa_cfgs.radius.interval_for_acct_update = RADIUS_DEF_UPDATE_INTERVAL;
	//g_aaa_cfgs.radius.login_num_max = AAA_ONLINE_NUM_MAX;	//最大登录用户数目	
	g_aaa_cfgs.radius.authen_mode = MODE_PAP;

	/* udp register */
	aaa_radius_pkt_register();

#if 0	
	/* used to check timeout */
	thread_add_timer(aaa_master, radius_auth_timeout_check, NULL, g_aaa_cfgs.radius.retry_interval);
	thread_add_timer(aaa_master, radius_acct_timeout_check, NULL, g_aaa_cfgs.radius.retry_interval);


	/* hash table init */
	radius_session_table_init();
#endif

}


int radius_auth_server_set(const struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_ERROR;

	g_aaa_cfgs.radius.auth_server.sin_addr.s_addr = pserver->sin_addr.s_addr;
	g_aaa_cfgs.radius.auth_server.sin_port = pserver->sin_port;

	return AAA_OK;
}

int radius_auth_server_get(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_ERROR;

	pserver->sin_addr.s_addr = g_aaa_cfgs.radius.auth_server.sin_addr.s_addr;
	pserver->sin_port = g_aaa_cfgs.radius.auth_server.sin_port;

	return AAA_OK;
}

int radius_auth_server_vpn_set(uint16_t vpn)
{
	g_aaa_cfgs.radius.auth_vpn = vpn;
	return AAA_OK;
}

uint16_t radius_auth_server_vpn_get(void)
{
	return g_aaa_cfgs.radius.auth_vpn;
}



int radius_acct_server_set(const struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_ERROR;

	g_aaa_cfgs.radius.acct_server.sin_addr.s_addr = pserver->sin_addr.s_addr;
	g_aaa_cfgs.radius.acct_server.sin_port = pserver->sin_port;

	return AAA_OK;
}

int radius_acct_server_get(struct sockaddr_in *pserver)
{
	if(NULL == pserver)
		return AAA_ERROR;

	pserver->sin_addr.s_addr = g_aaa_cfgs.radius.acct_server.sin_addr.s_addr;
	pserver->sin_port = g_aaa_cfgs.radius.acct_server.sin_port;

	return AAA_OK;
}

int radius_acct_server_vpn_set(uint16_t vpn)
{
	g_aaa_cfgs.radius.acct_vpn = vpn;
	return AAA_OK;
}

uint16_t radius_acct_server_vpn_get(void)
{
	return g_aaa_cfgs.radius.acct_vpn;
}


int radius_acct_server_switch_set(int status)
{	
	g_aaa_cfgs.radius.acct_switch = status;
	return AAA_OK;
}

int radius_acct_server_switch_get(void)
{
	return g_aaa_cfgs.radius.acct_switch;
}


//config radius_secret
int radius_key_set(const char *str)	
{
	if(NULL == str)
	{
		return AAA_ERROR;
	}
	
	memset(g_aaa_cfgs.radius.auth_key, 0, (RADIUS_SHARED_SECRET_LEN + 1));
	memcpy(g_aaa_cfgs.radius.auth_key, str, strlen(str));

	return AAA_OK;
}


/* 配置认证模式chap/pap */
void radius_authen_mode_set (RADIUS_AUTHEN_MODE mode)
{
	g_aaa_cfgs.radius.authen_mode = mode;
}

RADIUS_AUTHEN_MODE radius_authen_mode_get (void)
{
	return g_aaa_cfgs.radius.authen_mode;
}



//get radius_secret
char *radius_key_get(void)
{	
	return g_aaa_cfgs.radius.auth_key;
}

//set radius_timeout
int radius_response_timeout_set(uint32_t val)	
{
	g_aaa_cfgs.radius.rsp_timeout = val;
	return AAA_OK;
}

uint32_t radius_response_timeout_get(void)
{	
	return (g_aaa_cfgs.radius.rsp_timeout * g_aaa_cfgs.radius.retry_interval); 
}

//set radius interval time
int radius_retry_interval_set(uint32_t val)	
{
	g_aaa_cfgs.radius.retry_interval = val;
	return AAA_OK;
}

uint32_t radius_retry_interval_get(void)
{	
	return g_aaa_cfgs.radius.retry_interval;
}

//set radius auth retry times
int radius_retry_times_set(uint32_t val)	
{
	g_aaa_cfgs.radius.retry_times = val;
	return AAA_OK;
}

uint32_t radius_retry_times_get(void)
{	
	return g_aaa_cfgs.radius.retry_times;
}


//set radius acct update interval times
int radius_acct_update_interval_set(uint32_t val)	
{
	g_aaa_cfgs.radius.interval_for_acct_update = val;
	
	struct hash_bucket *node = NULL;
	void *cursor = NULL;
	USER_ONLINE_INFO *psess = NULL;
	
	if(g_user_table.user_online_table.num_entries == 0)
		return AAA_OK;
	
	for((node) = hios_hash_start(&g_user_table.user_online_table, &cursor); node; (node) = hios_hash_next(&g_user_table.user_online_table, &cursor))		
	{
		if(NULL == (psess = (USER_ONLINE_INFO *)(node->data)))
			return AAA_ERROR;
		psess->session_info.radius_user.acct_update_inteval = g_aaa_cfgs.radius.interval_for_acct_update;
	}
	return AAA_OK;
}

uint32_t radius_acct_update_interval_get(void)
{	
	return g_aaa_cfgs.radius.interval_for_acct_update;
}


/*
 * Function: radius_random_authenticator_get 
 *
 * Purpose: get random authenticator.
 *
 * Returns: none
 *
 */

void radius_random_authenticator_get (char *pauth)
{
	int randno;
	int ret;

	srand (time (0) + getppid () + getpid ());	// random enough :)
	for (ret = 0; ret < RADIUS_VECTOR_LEN;)
	{
		randno = rand ();
		memcpy ( pauth,  &randno, sizeof (int));
		pauth += sizeof (int);
		ret += sizeof (int);
	}
}


/* * Function: radius_attr_cmp
 * * Purpose: compare attr item value
 * * Returns: val1 < val2, return -1
 * *   		  val1 = val2, return 0
 * *		  val1 > val2, return 1
 * */
int radius_attr_cmp(const RADIUS_ATTR_PAIR* val1, const RADIUS_ATTR_PAIR* val2)
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
        	if(val1->type > val2->type)
				return 1;
			else if(val1->type == val2->type)
				return 0;
			else
				return -1;
        }
    }   
}

/* create attr list */
struct list *radius_attr_list_create(void)
{
    struct list *ls = list_new();	
    ls->del = (void (*) (void *))attr_pair_del;	
    ls->cmp = (int (*) (void *, void *))radius_attr_cmp;
    return ls;
}

/* destory attr list */
void radius_attr_list_destory(struct list *ls)
{
    list_delete_all_node(ls);
    list_free(ls);
}

/* radius attribute pair new */ 
RADIUS_ATTR_PAIR *radius_attr_pair_new(void)
{	
    return (RADIUS_ATTR_PAIR *)XCALLOC(MTYPE_AAA, sizeof(RADIUS_ATTR_PAIR));
}

void radius_attr_pair_add(struct list *ls, char type, char length, char *val, uchar chap_id)
{
    RADIUS_ATTR_PAIR *attr_add = radius_attr_pair_new();
	if(NULL == attr_add)
	{
		zlog_err ("%s[%d] -> %s: XMALLOC error!\n", __FILE__, __LINE__, __func__);
		return;
	}	
	
    attr_add->type = type;
	if(PW_CHAP_PASSWORD == type)
	{
		attr_add->length = length + 3;
		memcpy(attr_add->value, &chap_id, sizeof (chap_id));
		if (NULL != val)
			memcpy (attr_add->value + sizeof (chap_id), val, length);
	}
	else
	{
		attr_add->length = length + 2;
		if (NULL != val)
			memcpy (attr_add->value, val, length);
	}
		
    listnode_add_sort(ls, attr_add);
}

RADIUS_ATTR_PAIR *radius_attr_pair_lookup(struct list *ls, char type)
{
    struct listnode     *node, *nnode;	
    RADIUS_ATTR_PAIR	*attr_find;	
    for(ALL_LIST_ELEMENTS(ls, node, nnode, attr_find))	
    {       
        if(attr_find->type == type)
            return attr_find;   
    }   
    return NULL;
}

void attr_pair_del(RADIUS_ATTR_PAIR *attr)
{
    XFREE(MTYPE_AAA, attr);
}

int radius_attr_pair_del(struct list *ls, char type)
{
    RADIUS_ATTR_PAIR *attr_find;
    attr_find = radius_attr_pair_lookup(ls, type);	
    listnode_delete(ls, attr_find);	
    ls->del(attr_find);	
    return AAA_OK;
}

/* md5 */
void radius_md5_calc (char* output, char* input, uint32_t inlen)
{
	MD5_CTX         context;

	MD5Init (&context);
	MD5Update (&context, input, inlen);
	MD5Final ((uint8_t *)output, &context);
}


/* find attribute form auth/acct response packet
 * rev_pkt    : received response packet form service
 * type       : type of attribute to find
 * attr_found : attribute found
 */
int radius_find_attr_from_response_pkt(RC_RCV_PAYLOAD *rev_pkt, char type, char type_private, RADIUS_ATTR_PAIR *attr_found)
{
	uint32_t len = 0;
	uint32_t attr_len_sum = 0;	//sum length of all rcv attributes 

	char len_private = 0;
	char len_private_sum = 0;	//sum length of private attributes
			
	if(NULL == rev_pkt)
		return AAA_ERROR;
	
	char *rev_attr = rev_pkt->data;	
	attr_len_sum = (ntohs(rev_pkt->length) - RADIUS_HDR_LEN);
	if(attr_len_sum <= 2)
		return AAA_ERROR;
	
	while(len < attr_len_sum)
	{
		if(*rev_attr == type)
		{
			if(PW_VENDOR_SPECIFIC == type)
   			{
   				if(0 == type_private)
   				{
   					attr_found->type = *rev_attr;
   					attr_found->length = *(rev_attr + 1);
   					memcpy(attr_found->value, (rev_attr + 2), attr_found->length);
   					return AAA_OK;
   				}
   				else
   				{
   					len_private_sum = (*(rev_attr + 1) - 6);
   					rev_attr += 6;
   					while(len_private < len_private_sum)
   					{
   						if(*rev_attr == type_private)
   						{
   							attr_found->type = *rev_attr;
   							attr_found->length = *(rev_attr + 1);
   							memcpy(attr_found->value, (rev_attr + 2), attr_found->length);
   							return AAA_OK;
   						}
   						else
   						{
   							len_private += *(rev_attr + 1);
   							rev_attr += *(rev_attr + 1);   							
   						}
   					}
   					return AAA_ERROR;
   				}					
   			}
			else
			{
				attr_found->type = *rev_attr;
				attr_found->length = *(rev_attr + 1);
				memcpy(attr_found->value, (rev_attr + 2), (attr_found->length-2));
				return AAA_OK;
			}				
		}
		else
		{
			len += *(rev_attr + 1);
			rev_attr += *(rev_attr + 1);
		}
	}
	return AAA_ERROR;
}

/* handle radius auth msg */
int aaa_radius_auth_handle(struct vty *vty, USER_AUTHING_INFO *pinfo)
{
	
	return AAA_OK;
}

//check the time since sent msg the last time
//int timer
//handle the received payload
int aaa_radius_rcv_pkt_handle(struct pkt_buffer *pkt)
{
	RC_RCV_PAYLOAD radius_rcv_buf;
	int	   length_rcv_buf = 0;

	if(NULL == pkt)
	{
		return AAA_ERROR;
	}
	
	aaa_pkt_dump((char*)pkt->data, pkt->data_len, AAA_PKT_RECV, AAA_RADIUS, __func__);
	memset(&radius_rcv_buf, 0, sizeof(radius_rcv_buf));

	length_rcv_buf = pkt->data_len;
	if(length_rcv_buf < RADIUS_HDR_LEN)	//shorter than head of radius packet:20
	{
		return AAA_ERROR;
	}
	memcpy(&radius_rcv_buf, pkt->data, length_rcv_buf);	//copy rcv msg to radius_rcv_buf
	radius_rcv_buf.length = ntohs(radius_rcv_buf.length);
	radius_rcv_buf.used_length = length_rcv_buf;

	switch(radius_rcv_buf.code)	
	{
		case PW_ACCESS_ACCEPT:		//accept
		case PW_ACCESS_REJECT:		//reject
		case PW_ACCESS_CHALLENGE:
			radius_process_rcv_auth_pkt(&radius_rcv_buf);
			break;

        case PW_ACCOUNTING_RESPONSE:
			radius_process_rcv_acct_pkt(&radius_rcv_buf);
            break;

		default:
			//response "Unknown RADIUS message code"
			break;
	}

	return AAA_OK;
}


/* Function: aaa_radius_session_show */
void radius_session_show(struct vty *vty)
{
	
}


#if 0
/* show result of login test */
int radius_show_login_test_result(struct thread *thread)
{
	struct vty *vty;
  	vty = THREAD_ARG (thread);

	struct hash_bucket *node = NULL;
	struct hash_bucket *cursor = NULL;
	RADIUS_SESSION_INFO *psess = NULL;
	
	if(g_aaa_session.radius_session_table.num_entries == 0)
		return 0;
	
	for((node) = hios_hash_start(&g_aaa_session.radius_session_table, (void **)&cursor); node; (node) = hios_hash_next(&g_aaa_session.radius_session_table, (void **)&cursor))		
	{
		psess = (RADIUS_SESSION_INFO *)(node->data);
		if(psess == NULL)
			return 0;

		if(TEST_LOGIN == psess->login_mode)
		{
			switch(psess->result)
			{
				case AUTH_SUCCESS:
					if(ENABLE == radius_acct_server_switch_get())
						thread_add_timer(aaa_master, radius_show_login_test_result, vty, 1);
					else
					{
						vty_out(vty, "%suser %s authenticate/authorize success[priv:%d], account disable %s",\
							VTY_NEWLINE, psess->username, psess->privilege, VTY_NEWLINE);
						radius_session_del(psess->username);
					}						
					break;
				case AUTH_FAILED:
					vty_out(vty, "%suser %s authenticate/authorize failed %s", VTY_NEWLINE, psess->username, VTY_NEWLINE);
					radius_session_del(psess->username);
					break;
				case AUTH_TIMEOUT:
					vty_out(vty, "%suser %s authenticate/authorize timeout %s", VTY_NEWLINE, psess->username, VTY_NEWLINE);
					radius_session_del(psess->username);
					break;
				case ACCT_SUCCESS:
					vty_out(vty, "%suser %s authenticate/authorize success[priv:%d], account success %s",\
						VTY_NEWLINE, psess->username, psess->privilege, VTY_NEWLINE);
					radius_session_del(psess->username);
					break;
				case ACCT_FAILED:
					vty_out(vty, "%suser %s authenticate/authorize success[priv:%d], account failed %s",\
						VTY_NEWLINE, psess->username, psess->privilege, VTY_NEWLINE);
					radius_session_del(psess->username);
				case ACCT_TIMEOUT:
					vty_out(vty, "%suser %s authenticate/authorize success[priv:%d], account timeout %s",\
						VTY_NEWLINE, psess->username, psess->privilege, VTY_NEWLINE);
					radius_session_del(psess->username);
					break;
				default:
					thread_add_timer(aaa_master, radius_show_login_test_result, vty, 1);
					break;
			}					
		}
	}
	return 1;
}



/* test login */
int radius_login_test(struct vty *vty, const char *username, const char *password)
{
	if(NULL == username)
	{
		zlog_err("%s[%d] -> %s: NULL pointer!\n", __FILE__, __LINE__, __func__);
		return AAA_ERROR;
	}
	
	AAA_LOGIN_REQUEST auth_msg;
	memset(&auth_msg, 0, sizeof(auth_msg));
	memcpy(auth_msg.username, username, strlen(username));

	if(NULL != password)
		memcpy(auth_msg.password, password, strlen(password));
	
	auth_msg.type = LOGIN_REQUEST;
	aaa_radius_auth_handle(vty, &auth_msg);
	
	thread_add_timer(aaa_master, radius_show_login_test_result, vty, 3);

	return AAA_OK;
}
#endif



//------------------------- end -----------------------------//


