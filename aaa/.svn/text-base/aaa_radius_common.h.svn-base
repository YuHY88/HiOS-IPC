/******************************************************************************
 * Filename: aaa_radius_common.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.21  lipf created
 *
******************************************************************************/

#ifndef _AAA_RADIUS_COMMON_H_
#define _AAA_RADIUS_COMMON_H_

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


#define RADIUS_DEF_SEVR_IPV4		0			//default radius server ip
#define	RADIUS_DEF_SECRET			"secret"	//default secret 
#define RADIUS_DEF_TIMEOUT			1			//default timeout n*1(s)
#define RADIUS_DEF_INTERVAL			1000		//default interval time
#define RADIUS_DEF_MAX_RETRY		3			//default max-retry times
#define RADIUS_DEF_UPDATE_INTERVAL 	600			//default update interval 600s
#define RADIUS_DEF_ACCT_TIMEOUT		5			//在线计费时，计费报文接收的超时时间5s


//udp use
#define RC_PKT_BUF_LEN		1024
#define RC_DEF_UDP_TTL		64

//hash size
#define RADIUS_SESS_MAX		256


#define MD5_MAC_LEN				16

#define RADIUS_AUTH_UDP_PORT  	1812
#define RADIUS_ACCT_UDP_PORT  	1813

#define RADIUS_CODE_LEN			1
#define RADIUS_ID_LEN			1
#define RADIUS_LENGTH_LEN		2
#define RADIUS_VECTOR_LEN		16
#define RADIUS_HDR_LEN			20

#define RADIUS_USERNAME_LEN_MAX	128		//big than 63
#define RADIUS_PASSWORD_LEN_MAX	128		//max 128

#define RADIUS_RCV_MSG_LEN_MAX	1024	//debug


/* for radius test */
#define	USER_LOGIN		0
#define TEST_LOGIN		1

#define REMOTE_AUTHEN	0
#define LOCOAL_AUTHEN	1

//lpf debug

typedef struct _rc_rcv_payload
{
	char          	code;
	char          	id;
	uint16_t      	length;
	char          	auth[RADIUS_VECTOR_LEN];
	char			data[RADIUS_RCV_MSG_LEN_MAX-RADIUS_HDR_LEN];

	uint32_t		used_length;
}__attribute__((aligned(1))) RC_RCV_PAYLOAD;


/********************* new struct **************************/

//宏也需要改，参考auth_radius.h，统一放到radiusclient中

typedef struct radius_attr_pair
{
	char                	type;
	char              		length;
	char               		value[AUTH_STRING_LEN + 1];
}__attribute__((aligned(1))) RADIUS_ATTR_PAIR;


typedef struct _radius_send_pkt
{
	char          	code;
	char          	id;
	u_int16         length;
	char          	authenticator[RADIUS_VECTOR_LEN];
	struct list	    *attribute_list;	
}__attribute__((aligned(1))) RADIUS_SEND_PKT;



typedef struct _radius_session_info
{
	char				auth_retry_times;		//retry times
	char				acct_retry_times;		//retry times
	uint32_t			auth_timeleft_retry;	//再次发送认证报文的剩余时间
	uint32_t			acct_timeleft_retry;	//再次发送计费报文的剩余时间
	uint32_t			acct_update_inteval;	//acct update packet send interval	
	uint32_t			acct_timeleft_response;	//计费报文发送后等待应答的剩余时间
	uint32_t			acct_send_flag;			//计费报文已发送但尚未收到应答标志
	uint32_t			acct_failed_cnt;		//计费失败次数

	RADIUS_SEND_PKT		auth_pkt;
    RADIUS_SEND_PKT     acct_pkt;

	AAA_RESULT			auth_result;
	AAA_RESULT			acct_result;
	
}RADIUS_SESSION_INFO;

/************************ end ******************************/ 


#endif  /* _AAA_RADIUS_COMMON_H_ */

