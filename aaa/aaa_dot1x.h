/******************************************************************************
 * Filename: dot1x.h
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

#ifndef DOT1X_H
#define DOT1X_H

#include <time.h>

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/aaa_common.h>
#include <lib/linklist.h>

#include "aaa.h"
#include "aaa_radius_common.h"


/* IEEE Std 802.1X-2001, 7.2 */


#define IEEE802_1X_TYPE				0x888e
#define IEEE802_ATTACH_KEY_LEN		16


struct ieee802_1x_hdr
{
	uint8_t version;
	uint8_t type;
	uint16_t length;
	/* followed by length octets of data */
}
__attribute__ ((packed));

#define EAPOL_VERSION 1

enum
{ 
	IEEE802_1X_TYPE_EAP_PACKET = 0,
	IEEE802_1X_TYPE_EAPOL_START = 1,
	IEEE802_1X_TYPE_EAPOL_LOGOFF = 2,
	IEEE802_1X_TYPE_EAPOL_KEY = 3,
	IEEE802_1X_TYPE_EAPOL_ENCAPSULATED_ASF_ALERT = 4,
	IEEE802_1X_TYPE_SESSION_KEY = 14
};

/* draft-congdon-radius-8021x-20.txt */

struct ieee802_1x_eapol_key
{
	uint8_t type;
	uint16_t key_length;
	uint8_t replay_counter[8];		/* does not repeat within the life of the keying
				 * material used to encrypt the Key field;
				 * 64-bit NTP timestamp MAY be used here */
	uint8_t key_iv[16];		/* cryptographically random number */
	uint8_t key_index;			/* key flag in the most significant bit:
				 * 0 = broadcast (default key),
				 * 1 = unicast (key mapping key); key index is in the
				 * 7 least significant bits */
	uint8_t key_signature[16];		/* HMAC-MD5 message integrity check computed with
				 * MS-MPPE-Send-Key as the key */

	/* followed by key: if packet body length = 44 + key length, then the
	* key field (of key_length bytes) contains the key in encrypted form;
	* if packet body length = 44, key field is absent and key_length
	* represents the number of least significant octets from
	* MS-MPPE-Send-Key attribute to be used as the keying material;
	* RC4 key used in encryption = Key-IV + MS-MPPE-Recv-Key */
}
__attribute__ ((packed));

enum
{ 
	EAPOL_KEY_TYPE_RC4 = 1
};


/* RFC 2284 - PPP Extensible Authentication Protocol (EAP) */

struct eap_hdr
{
	uint8_t code;
	uint8_t identifier;
	uint16_t length;			/* including code and identifier */
	/* followed by length-2 octets of data */
}
__attribute__ ((packed));

enum
{ 
	EAP_CODE_REQUEST = 1, 
	EAP_CODE_RESPONSE = 2, 
	EAP_CODE_SUCCESS = 3,
	EAP_CODE_FAILURE = 4
};

/* EAP Request and Response data begins with one octet Type. Success and
 * Failure do not have additional data. */

/* RFC 2284, 3.0 */
enum
{ 
	EAP_TYPE_IDENTITY = 1,
	EAP_TYPE_NOTIFICATION = 2,
	EAP_TYPE_NAK = 3 					/* Response only */ ,
	EAP_TYPE_MD5_CHALLENGE = 4,
	EAP_TYPE_ONE_TIME_PASSWORD = 5 		/* RFC 1938 */ ,
	EAP_TYPE_GENERIC_TOKEN_CARD = 6,
	EAP_TYPE_TLS = 13					/* RFC 2716 */
};



//liuxiaoping 6-17
#ifdef CONFIG_EAP_SIM
struct ieee802_1x_sess_key
{
	uint8_t  type;  /*0:session_key;1:session_key_ack*/
	uint16_t length; /*length of type_length+data*/
	//data
	uint8_t  mac[6];  
}
__attribute__ ((packed));

struct ieee802_1x_sub_key
{
	uint8_t	type;   /*1: sign_key; 2: crypt_key*/
	uint16_t length;  /*length of key*/
}
__attribute__ ((packed));

enum
{ 
	EAP_SESSION_KEY_REQ = 0,
	EAP_SESSION_KEY_ACK = 1
};

enum
{ 
	SESSION_SIGN_KEY = 1,
	SESSION_CRYPT_KEY = 2
};
#endif


typedef struct _eap_attribute
{
	uint8_t		type;
	uint16_t	length;
	uint8_t		*data;
}EAP_ATTR;


typedef struct _eap_packet
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	struct eap_hdr			hdr_eap;
	EAP_ATTR			   *hdr_attr;
	
}EAP_PACKET;


/* eapol start 报文 */
typedef struct _eapol_start_pkt
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	
}__attribute__ ((packed)) EAPOL_START_PKT;


/* eap identity request 报文 */
typedef struct _eap_identity_request_pkt
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	struct eap_hdr			hdr_eap;
	uint8_t 				type;
	
}__attribute__ ((packed)) EAP_IDENTITY_REQUEST_PKT;


/* eap identity response 报文 */
typedef struct _eap_identity_response_pkt
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	struct eap_hdr			hdr_eap;
	char					type;
	char 					username[64];
	
}__attribute__ ((packed)) EAP_IDENTITY_RESPONSE_PKT;


/* eap md5-challenge request/response 报文 */
typedef struct _eap_md5_challenge_pkt
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	struct eap_hdr			hdr_eap;
	uint8_t 				type;
	uint8_t					key_len;
	uint8_t					key_attach[IEEE802_ATTACH_KEY_LEN];
	
}__attribute__ ((packed)) EAP_MD5_CHALLENGE_PKT;


/* eap auth result 报文 */
typedef struct _eap_auth_result_pkt
{
	struct ieee802_1x_hdr 	hdr_dot1x;
	struct eap_hdr			hdr_eap;
	
}__attribute__ ((packed)) EAP_AUTH_RESULT_PKT;



typedef struct _dot1x_info
{
	uint32_t	ifindex;
	char		mac[6];
	//struct list	*vlan_hdr;
	char		username[USER_NAME_MAX + 1];
	char		password_enctypted[IEEE802_ATTACH_KEY_LEN];
	char		key_attach[IEEE802_ATTACH_KEY_LEN];
	char 		version;
	
}DOT1X_INFO;



/* 终结/中继方式 */
typedef enum
{
	DOT1X_FINAL_METHOD,
	DOT1X_RELAY_METHOD,

}DOT1X_AUTH_METHOD;


/* 从 本地/radius 进行认证 */
typedef enum
{
	DOT1X_LOCAL_MODE,
	DOT1X_RADIUS_MODE,
	
}DOT1X_LOGIN_MODE;



typedef struct _dot1x_authing_info
{
	uint32_t			id;
	DOT1X_INFO			dot1x;
	AAA_RESULT			auth_result;
	int					auth_finished;
	DOT1X_TYPE_BASED	type_based;

	int					retry_times;
	int					retry_interval;	
	time_t				time_login;

	RADIUS_SEND_PKT 	pkt_radius;
	char				state[16];
	
}DOT1X_AUTHING_INFO;


typedef struct _dot1x_online_info
{
	uint32_t			id;			
	DOT1X_INFO			dot1x;
	DOT1X_TYPE_BASED	type_based;	
	time_t				time_login;
	
}DOT1X_ONLINE_INFO;


typedef struct _dot1x_log_info
{
	uint32_t			id;
	DOT1X_INFO			dot1x;
	DOT1X_TYPE_BASED	type_based;
	time_t				time_login;
	time_t				time_logoff;
	
}DOT1X_LOG_INFO;


#define DOT1X_DEF_AUTHING_CHECK_INTERVAL	200									//正在认证检测时间间隔,ms
#define DOT1X_DEF_RETRY_TIMES				3									//认证报文重发次数
#define DOT1X_DEF_RETRY_INTERVAL			3*DOT1X_DEF_AUTHING_CHECK_INTERVAL	//认证报文重发间隔


typedef struct _dot1x_config
{
	int					switchh;		//dot1x 开关标识
	DOT1X_TYPE_BASED	type_based;		//基于何种方式认证
	DOT1X_AUTH_METHOD	auth_method;	//dot1x 认证方式：final、relay
	DOT1X_LOGIN_MODE	login_mode;		//dot1x 登录模式：local、radius

	int					retry_times;
	int					retry_interval;
}DOT1X_CFG;



#define DOT1X_AUTHING_NUM_MAX			64		//可同时进行认证的的最大数目
#define DOT1X_ONLINE_NUM_MAX			64		//在线的最大数目
#define DOT1X_LOG_NUM_MAX				64		//可记录的最大数目


/* dot1x hash table */
typedef struct _dot1x_table
{
	struct hash_table dot1x_authing_table;	//正在登录的用户表
	struct hash_table dot1x_online_table;	//登录的用户表
	struct hash_table dot1x_log_table;		//登录的用户表
	
}DOT1X_TABLE;




/* dot1x 端口链表，用于存储端口的dot1x功能配置信息 */
typedef struct _dot1x_port_cfg
{
	uint32_t 			ifindex;
	int					state;		//关：DISABLE; 开：ENABLE
	DOT1X_TYPE_BASED	type_based;

}DOT1X_PORT_CFG;




extern DOT1X_TABLE g_dot1x_table;			/* dot1x哈希表 */
extern struct list *dot1x_port_cfg_ls;

void aaa_dot1x_init (void);

void aaa_dot1x_rcv_pkt_handle (struct pkt_buffer *pkt);

void dot1x_eap_packet_handle(struct pkt_buffer *pkt);
void dot1x_eapol_start_handle(struct pkt_buffer *pkt);
void dot1x_eapol_logoff_handle(struct pkt_buffer *pkt);
void dot1x_eapol_key_handle(struct pkt_buffer *pkt);
void dot1x_eapol_encapsulated_asf_alert_handle(struct pkt_buffer *pkt);
void dot1x_session_key_handle(struct pkt_buffer *pkt);

void dot1x_eap_identity_response_handle (struct pkt_buffer *pkt);
void dot1x_eap_md5_challenge_response_handle (struct pkt_buffer *pkt);
void dot1x_eap_success_send (uint8_t version, uint32_t ifindex, uint8_t *dmac);
void dot1x_eap_fail_send (uint8_t version, uint32_t ifindex, uint8_t *dmac);


uint32_t dot1x_id_create (uint32_t ifindex, uint8_t *mac);
void dot1x_key_attach_create (char *pkey);
DOT1X_ONLINE_INFO * dot1x_copy_authing_info_to_online (DOT1X_AUTHING_INFO *pinfo);

void dot1x_eap_identity_request_send (char eap_version, uint32_t ifindex, uint8_t *dmac);
void dot1x_eap_md5_challenge_request_send (DOT1X_AUTHING_INFO *pinfo, uint32_t ifindex, char *dmac);
void dot1x_eap_radius_md5_challenge_request_send (char *buf, size_t len, DOT1X_AUTHING_INFO *pinfo);

int dot1x_md5_local_auth_check (char id, char *key, char *spassword, char *dstr);

char dot1x_radius_id_create (void);
void dot1x_calc_message_authenticator (RADIUS_SEND_PKT *pkt_radius);

void dot1x_send_final_radius_request (DOT1X_AUTHING_INFO *pinfo, char *password);
void dot1x_send_relay_id_response_radius_access (DOT1X_AUTHING_INFO *pinfo, struct pkt_buffer *pkt);
void dot1x_send_relay_md5_response_radius_access (DOT1X_AUTHING_INFO *pinfo, struct pkt_buffer *pkt);
void aaa_dot1x_handle_radius_auth_result (DOT1X_AUTHING_INFO *pinfo, int result);
void aaa_dot1x_handle_radius_access_challenge (DOT1X_AUTHING_INFO *pinfo, RC_RCV_PAYLOAD *pkt_recv);

int aaa_dot1x_check_radius_access_challenge (RC_RCV_PAYLOAD *pkt_recv);


int dot1x_user_authing_add (DOT1X_AUTHING_INFO *pinfo);
DOT1X_AUTHING_INFO *dot1x_user_authing_get (uint32_t user_id);
int dot1x_user_authing_del (uint32_t user_id);

int dot1x_user_online_add (DOT1X_ONLINE_INFO *pinfo);
DOT1X_ONLINE_INFO *dot1x_user_online_get (uint32_t user_id);
int dot1x_user_online_del (uint32_t user_id);


void aaa_dot1x_switch_set (int status);
int aaa_dot1x_switch_get (void);
void aaa_dot1x_type_based_set (DOT1X_TYPE_BASED type_based);
DOT1X_TYPE_BASED aaa_dot1x_type_based_get (void);
void aaa_dot1x_login_mode_set (DOT1X_LOGIN_MODE mode);
DOT1X_LOGIN_MODE aaa_dot1x_login_mode_get (void);
void aaa_dot1x_auth_method_set (DOT1X_AUTH_METHOD method);
DOT1X_AUTH_METHOD aaa_dot1x_auth_method_get (void);


void aaa_dot1x_show_config (struct vty *vty);
void aaa_dot1x_show_online_info (struct vty *vty);
void aaa_write_dot1x_config (struct vty *vty);


extern void radius_md5_calc (char* output, char* input, uint32_t inlen);


int dot1x_port_index_cmp (uint32_t *val1, uint32_t *val2);
struct list *dot1x_port_list_create (void);
void dot1x_port_list_destory (struct list *ls);
DOT1X_PORT_CFG *dot1x_port_cfg_new (void);
void dot1x_port_cfg_add (struct list *ls, uint32_t ifindex, int state, DOT1X_TYPE_BASED	type_based);
DOT1X_PORT_CFG *dot1x_port_cfg_lookup (struct list *ls, uint32_t ifindex);
void dot1x_port_cfg_free (DOT1X_PORT_CFG *port_cfg);
int dot1x_port_cfg_del (struct list *ls, uint32_t ifindex);
void dot1x_port_cfg_del_all (struct list *ls);

void aaa_dot1x_show_port_cfg_info (struct vty *vty);


int aaa_dot1x_auth_process_check (void *para);

#endif /* DOT1X_H */


