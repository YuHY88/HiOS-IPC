/******************************************************************************
 * Filename: aaa_tac_plus.h
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

#ifndef _AAA_TAC_PLUS_H_
#define _AAA_TAC_PLUS_H_

#include <time.h>
#include <lib/types.h>
#include <lib/aaa_common.h>

#include "aaa_user_manager.h"
#include "tacplus.h"
#include "aaa_msg_handle.h"
#include "aaa.h"
#include "aaa_tac_plus_common.h"



/* TAC_PLUS configuration parameters.  */
typedef struct _tac_plus_cfg_
{	
	char authen_server_name[MAX_HOSTNAME_LEN+1];  		//Authen Server address string.	
	char authen_server_bak_name[MAX_HOSTNAME_LEN+1];  	//Backup authen server address string.	
	char author_server_name[MAX_HOSTNAME_LEN+1];    	//Author Server address string.	
	char author_server_bak_name[MAX_HOSTNAME_LEN+1];	//Backup author server address string.	
	char acct_server_name[MAX_HOSTNAME_LEN+1];    		//Acct Server address string.	
	char acct_server_bak_name[MAX_HOSTNAME_LEN+1];  	//Backup acct server address string.	
	
	struct sockaddr_in authen_server;             //Authen Server address.	
	struct sockaddr_in authen_server_bak;         //Backup authen server address.
	struct sockaddr_in author_server;             //Author Server address.	
	struct sockaddr_in author_server_bak;         //Backup author server address.
	struct sockaddr_in acct_server;               //Acct Server address.
	struct sockaddr_in acct_server_bak;           //Backup acct server address.

	int 	 authen_mode;
	uint16_t authen_vpn;
	uint16_t author_vpn;
	uint16_t acct_vpn;
	
	char authen_key[TAC_PLUS_SHARED_SECRET_LEN+1];  //Authen shared secret key.
	char author_key[TAC_PLUS_SHARED_SECRET_LEN+1];  //Author shared secret key.
	char acct_key[TAC_PLUS_SHARED_SECRET_LEN+1];  	//Acct   shared secret key.
	
	uint32_t 	rsp_timeout;  				//server response timeout.
	uint32_t 	interval_for_acct_update;	//acct update packet interval

	int 		author_switch;			//认证功能开关
	int 		acct_switch;			//计费功能开关	
	
	//int			login_num_max;		//最大登录用户数量
	
}TACACS_CFG;
	


/* Only for testing and debug, must be 1 in the release version */
extern int tac_encryption;

void tac_plus_session_table_init(void);
void aaa_tac_plus_init(void);

USER_AUTHING_INFO *tac_plus_authing_user_get_by_session_id(int id, int mode);
USER_ONLINE_INFO *tac_plus_online_user_get_by_session_id(int id, int mode);

int aaa_tac_plus_authen_start(struct vty *vty, USER_AUTHING_INFO *pinfo, int login_mode);
int tac_acct_send_stop(TACACS_SESSION_INFO* precord);
int aaa_tac_plus_process_recv_pkt(void *pkt, uint32_t pkt_len);

int tac_plus_key_set(const char *key);
char *tac_plus_key_get(void);
int tac_plus_sess_id_get(void);


int tac_plus_session_new_src_port(uint16_t *src_port);
int tac_plus_session_src_port_set(TACACS_SESSION_INFO *precord, int mode);
time_t tac_plus_response_timeout_get(void);
u_int16_t tac_plus_cfg_authen_server_port_get(void);
u_int16_t tac_plus_cfg_bak_authen_server_port_get(void);
u_int16_t tac_plus_cfg_author_server_port_get(void);
u_int16_t tac_plus_cfg_bak_author_server_port_get(void);
u_int16_t tac_plus_cfg_acct_server_port_get(void);
u_int16_t tac_plus_cfg_bak_acct_server_port_get(void);

int tac_plus_acct_server_switch_set(int status);
int tac_plus_acct_server_switch_get(void);
int tac_plus_acct_update_interval_set(time_t val);
time_t tac_plus_acct_update_interval_get(void);

void tac_plus_session_show(struct vty *vty);

/* function declaration in tac_plus_crypt.c */
void _tac_crypt(char *buf, TAC_HDR *th, int length);
char *_tac_md5_pad(int len, TAC_HDR *hdr);

/* function declaration in tac_plus_header.c */
int _tac_req_header(char type, int cont_session, TAC_HDR *th);
char *_tac_check_header(TAC_HDR *th);

/* function declaration in tac_plus_attrib.c */
void tac_add_attrib(struct tac_attrib **attr, char *name, char *value);
void tac_add_attrib_pair(struct tac_attrib **attr, char *name, char sep, char *value);
void tac_free_attrib(struct tac_attrib **attr);

/* function declaration in tac_plus_authen.c */
int tac_record_authen_info(USER_AUTHING_INFO *pinfo, const char *rem_addr, char action, char authen_type);
int tac_authen_pkt_send(TACACS_SESSION_INFO *psess);
int tac_authen_rcv_process(TAC_HDR *th, void *body, struct areply *re);

/* function declaration in tac_plus_author.c */
int tac_record_author_info(USER_AUTHING_INFO *pinfo, const char *r_addr);
int tac_author_pkt_send(TACACS_SESSION_INFO *psess);
int tac_author_rcv_process(TAC_HDR *th, void *body, struct areply *re);

/* function declaration in tac_plus_acct.c */
int tac_record_acct_info(USER_AUTHING_INFO *pinfo, const char *r_addr);
int tac_acct_pkt_send(TACACS_SESSION_INFO *psess);
int tac_acct_rcv_process(TAC_HDR *th, void *body, struct areply *re);

int tac_plus_authen_server_set(struct sockaddr_in *pserver);
int tac_plus_authen_server_get(struct sockaddr_in *pserver);
int tac_plus_author_server_set(struct sockaddr_in *pserver);
int tac_plus_author_server_get(struct sockaddr_in *pserver);
int tac_plus_acct_server_set(struct sockaddr_in *pserver);
int tac_plus_acct_server_get(struct sockaddr_in *pserver);
int tac_plus_author_server_switch_set(int status);
int tac_plus_author_server_switch_get(void);

int tac_plus_authen_mode_set(int mode);
int tac_plus_authen_mode_get(void);

int tac_plus_authen_server_vpn_set(uint16_t vpn);
uint16_t tac_plus_authen_server_vpn_get(void);

int tac_plus_author_server_vpn_set(uint16_t vpn);
uint16_t tac_plus_author_server_vpn_get(void);

int tac_plus_acct_server_vpn_set(uint16_t vpn);
uint16_t tac_plus_acct_server_vpn_get(void);

int tac_acct_session_renew_src_port(int *src_port);
int tac_acct_pkt_send_body(TACACS_SESSION_INFO *precord);

/* user test and show*/
int tac_plus_show_login_test_result(struct thread *thread);
int tac_plus_login_test(struct vty *vty, const char *username, const char *password);


#endif  /* _AAA_TAC_PLUS_H_ */

