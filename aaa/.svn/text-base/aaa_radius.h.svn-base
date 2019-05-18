/******************************************************************************
 * Filename: aaa_radius.h
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

#ifndef _AAA_RADIUS_H_
#define _AAA_RADIUS_H_

#include <time.h>

#include <lib/vty.h>
#include <lib/hash1.h>
#include <lib/pkt_buffer.h>

#include <lib/types.h>
#include <lib/log.h>
#include <lib/linklist.h>

#include "radiusclient.h"
#include "lib/msg_ipc_n.h"
#include "lib/aaa_common.h"

#include "aaa.h"
#include "aaa_radius_common.h"
#include "aaa_user_manager.h"



/* RADIUS configuration parameters.  */
typedef struct _radius_cfg_
{	
	char auth_server_name[MAX_HOSTNAME_LEN+1];    	//Authen/author Server address string. 	
	char auth_server_bak_name[MAX_HOSTNAME_LEN+1];  //Backup authen/author server address string.
	char acct_server_name[MAX_HOSTNAME_LEN+1];  	//Acct Server address string.
	char acct_server_bak_name[MAX_HOSTNAME_LEN+1];  //Backup acct server address string.
	
	struct sockaddr_in auth_server;               	//Authen/author Server address.		
	struct sockaddr_in auth_server_bak;           	//Backup authen/author server address.		
	struct sockaddr_in acct_server;             	//Acct Server address.		
	struct sockaddr_in acct_server_bak;             //Backup acct server address.

	uint16_t auth_vpn;
	uint16_t acct_vpn;
	
	char 		auth_key[RADIUS_SHARED_SECRET_LEN+1];  //Authen/author shared secret key.
	char 		acct_key[RADIUS_SHARED_SECRET_LEN+1];  //Acct shared secret key.
	
	uint32_t 	rsp_timeout;              	//server response timeout.	
	uint32_t 	retry_interval;  		   	//interval time for retransmition.	
  	uint32_t 	retry_times;         	   	//retransmit times.

    uint32_t 	interval_for_acct_update; 	//acct update packet interval
	int 		acct_switch;				//account function status

	RADIUS_AUTHEN_MODE	authen_mode;
	//int		login_num_max;			//最大登录用户数量
	
}RADIUS_CFG;




void aaa_radius_init(void);

int radius_auth_server_set(const struct sockaddr_in *pserver);
int radius_auth_server_get(struct sockaddr_in *pserver);
int radius_auth_server_vpn_set(uint16_t vpn);
uint16_t radius_auth_server_vpn_get(void);
int radius_acct_server_vpn_set(uint16_t vpn);
uint16_t radius_acct_server_vpn_get(void);

int radius_acct_server_set(const struct sockaddr_in *pserver);
int radius_acct_server_get(struct sockaddr_in *pserver);
int radius_acct_server_switch_set(int status);
int radius_acct_server_switch_get(void);
int radius_key_set(const char *str);
char *radius_key_get(void);
void radius_authen_mode_set (RADIUS_AUTHEN_MODE mode);
RADIUS_AUTHEN_MODE radius_authen_mode_get (void);
int radius_response_timeout_set(uint32_t val);
uint32_t radius_response_timeout_get(void);
int radius_retry_interval_set(uint32_t val);
uint32_t radius_retry_interval_get(void);
int radius_retry_times_set(uint32_t val);
uint32_t radius_retry_times_get(void);
int radius_acct_update_interval_set(uint32_t val);
uint32_t radius_acct_update_interval_get(void);

void radius_session_table_init(void);
int radius_session_add(RADIUS_SESSION_INFO *psess);
RADIUS_SESSION_INFO *radius_session_get(const char *username);
int radius_session_del(const char *username);

int radius_session_id_create(void);

int radius_pkt_fill_code(RADIUS_SEND_PKT *radius_send_pkt);
int radius_pkt_fill_id(RADIUS_SEND_PKT *radius_send_pkt);
int radius_pkt_fill_length(RADIUS_SEND_PKT *radius_send_pkt);
int radius_pkt_fill_authenticator(RADIUS_SEND_PKT *radius_send_pkt);
void radius_md5_calc(char* output, char* input, uint32_t inlen);
void radius_random_authenticator_get(char *pauth);

void radius_random_authenticator_get (char *pauth);

int aaa_radius_rcv_pkt_handle(struct pkt_buffer *pkt);

int radius_attr_cmp(const RADIUS_ATTR_PAIR* val1, const RADIUS_ATTR_PAIR* val2);
struct list *radius_attr_list_create(void);
void radius_attr_list_destory(struct list *ls);
RADIUS_ATTR_PAIR *radius_attr_pair_new(void);
void radius_attr_pair_add(struct list *ls, char type, char length, char *val, uchar chap_id);
RADIUS_ATTR_PAIR *radius_attr_pair_lookup(struct list *ls, char type);
int radius_attr_pair_del(struct list *ls, char type);
void attr_pair_del(RADIUS_ATTR_PAIR *attr);

int radius_find_attr_from_response_pkt(RC_RCV_PAYLOAD *rev_pkt, char type, char type_private, RADIUS_ATTR_PAIR *attr_found);

int aaa_radius_auth_handle(struct vty *vty,  USER_AUTHING_INFO *pinfo);

void radius_session_show(struct vty *vty);
int radius_show_login_test_result(struct thread *thread);

int radius_login_test(struct vty *vty, const char *username, const char *password);

uint32_t radius_alloc_user_id(void);
int radius_get_online_user_sum(void);


#endif  /* _AAA_RADIUS_H_ */

