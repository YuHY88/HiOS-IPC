/******************************************************************************
 * Filename: aaa.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.11  lipf created
 *
******************************************************************************/

#ifndef _AAA_H_
#define _AAA_H_

//#include <netinet/in.h>
#include <lib/aaa_common.h>
#include <lib/hash1.h>
#include <lib/errcode.h>

#include <lib/command.h>
#include <lib/inet_ip.h>

#include "syslog.h"


#define AAA_ERROR           -1
#define AAA_OK              ERRNO_SUCCESS
#define AAA_FAIL            ERRNO_FAIL
#define AAA_NOT_FOUND       ERRNO_NOT_FOUND
#define AAA_MALLOC_FAIL     ERRNO_MALLOC
#define AAA_OVERSIZE        ERRNO_OVERSIZE
#define AAA_IPC_FAIL        ERRNO_IPC
#define AAA_PARAM_ERR       ERRNO_PARAM_ILLEGAL
#define AAA_PORT_ALLOC_FAIL ERRNO_PORT_ALLOC
#define AAA_PKT_SEND_FAIL   ERRNO_PKT_SEND
#define AAA_PKT_RECV_FAIL   ERRNO_PKT_RECV

#define AAA_YES     1
#define AAA_NO      2


#define AAA_DEF_LOGIN_PRIVILEGE			1		//登录之后默认权限
#define AAA_DEF_LOCAL_MAX_REPEAT_NUM	3		//默认的本地用户最大重复登录次数
#define AAA_DEF_NONE_AUTH_PRIVILEGE		1		//不认证用户的默认权限
#define AAA_DEF_ALIVE_TIME_MAX			ECHO_INTERVAL_TIME*12		//最大保活时间，单位s

#define AAA_DEF_IDLE_MAX				5		//默认用户最大空闲时间，分钟
#define AAA_ONLINE_NUM_MAX				32		//在线用户的最大数目
#define AAA_CONFIG_NUM_MAX				32		//可配置用户的最大数目
#define AAA_AUTHING_NUM_MAX				32		//可记录的用户登录记录的最大数目
#define AAA_LOG_NUM_MAX					32		//可记录的用户登录记录的最大数目
#define AAA_CIPHER_NUM_MAX				16		//口令的最大数目


#ifndef IPV4_STR_LEN_MAX
#define IPV4_STR_LEN_MAX       			16
#endif


#define AAA_AUTH_PROCESS_CHECK_INTERVAL		200		//认证流程检测时间间隔，ms
#define AAA_USER_ONLINE_CHECK_INTERVAL		1000	//用户在线状态检测时间间隔，ms

/* 隐藏的超级用户 */
#define SUPER_USER_NAME				"nmsuser"
#define SUPER_USER_PASSWORD			"Admin123"
#define	SUPER_USER_LEVEL			15

/* 默认配置的用户 */
#define	ADMIN_USER_NAME				"admin"
#define	ADMIN_USER_PASSWORD			"Admin123"
#define	ADMIN_USER_LEVEL			14
//#define ADMIN_USER_DEF_NUM		1
#define ADMIN_USER_DEF_NUM			16				//安微现场要求

/* enable默认密码 */
//#define	DEF_ENABLE_NAME			"enable"
#define DEF_ENABLE_PASSWORD			"root"


/* 按位定义debug信息的类型 */
#define AAA_DBG_COMMON		(1 << 0)		//login  debug
#define AAA_DBG_RADIUS		(1 << 1)		//radius debug
#define AAA_DBG_TACACS		(1 << 2)		//tacacs debug
#define AAA_DBG_LOCAL		(1 << 3)		//local  debug
#define AAA_DBG_NONE		(1 << 4)		//none   debug
#define AAA_DBG_DOT1X		(1 << 5)		//dot1x  debug
#define AAA_DBG_ALL			(AAA_DBG_COMMON |\
							 AAA_DBG_RADIUS |\
							 AAA_DBG_TACACS |\
							 AAA_DBG_LOCAL |\
							 AAA_DBG_NONE |\
							 AAA_DBG_DOT1X)	//所有debug



/* 认证模式 */
typedef  enum	
{
	METHOD_INVALID,	
	METHOD_RADIUS,
	METHOD_TACACS,	
	METHOD_LOCAL,
	METHOD_NONE

}LOG_METHOD;


typedef enum
{
	AUTHEN_INVALID = 0,
		
	AUTHEN_NO_SERVER,
	AUTHEN_NO_USER,
	AUTHEN_FAILED,
	AUTHEN_SUCCESS,

	AUTHOR_NO_SERVER,
	AUTHOR_FAILED,
	AUTHOR_SUCCESS,

	ACCT_NO_SERVER,
	ACCT_FAILED,
	ACCT_SUCCESS
	
}AAA_RESULT;


#define AAA_SHARED_KEY_LEN		  		64
#define RADIUS_SHARED_SECRET_LEN  		AAA_SHARED_KEY_LEN
#define TAC_PLUS_SHARED_SECRET_LEN      AAA_SHARED_KEY_LEN


/* Max hostname length. */
#define MAX_HOSTNAME_LEN          64


typedef enum 
{
	MODE_PAP,
	MODE_CHAP,
}RADIUS_AUTHEN_MODE;


typedef struct
{
	char			username[USER_NAME_MAX + 1];
	char			password[16][USER_PASSWORD_MAX + 1];
	
}ENABLE_INFO;



#endif /* _AAA_H_ */

