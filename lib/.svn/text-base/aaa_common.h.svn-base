/******************************************************************************
 * Filename: aaa_common.h
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2016.9.13  luoyz created
 *
******************************************************************************/

#ifndef _AAA_COMMON_H_
#define _AAA_COMMON_H_

//#include "vty.h"
#include <time.h>

#include "types.h"


#define SERVICE_TYPE_SSH		1<<0
#define SERVICE_TYPE_TELNET		1<<1
#define SERVICE_TYPE_TERMINAL	1<<2


//#define USER_NAME_MAX 		31
//#define USER_PASSWORD_MAX 	31

#define USER_NAME_MAX 		55			//匹配华三
#define USER_NAME_MIN 		1			//匹配华三
#define USER_PASSWORD_MAX 	63			//匹配华三
#define USER_PASSWORD_MIN 	1			//匹配华三

#define TTY_NAME_LEN_MAX  	63
#define LOGIN_FROM_LEN_MAX	31


#define	ENABLE_NAME			"enable"	//提升用户权限口令

#define ECHO_INTERVAL_TIME	5			//保活时间间隔5s


typedef enum					/* 消息类型 */
{
	AAA_MSG_INVALID = 0,				//
	AAA_MSG_AUTH_REQUEST,				//认证请求
	AAA_MSG_AUTH_RESPONSE,				//认证请求应答
	AAA_MSG_MAX_IDLE_TIME,				//最大空闲时间
	AAA_MSG_ECHO,						//保活
	AAA_MSG_IMPROVE_LEVEL,				//提升权限
	AAA_MSG_IMPROVE_LEVEL_RESPONSE,		//提升权限应答
	AAA_MSG_FORCE_EXIT,					//强制退出

	AAA_MSG_DOT1X_AUTHOR_RESPONSE,		//dot1x的认证应答
	AAA_MSG_DOT1X_CFG,
	AAA_MSG_DOT1X_PORT_CFG,

	AAA_MSG_ERRNO_REPORT,				//异常报告消息
	
}AAA_MSG_TYPE;



typedef enum		/* 登录结果，aaa向vty发送 */
{
	LOGIN_FAILED,		//失败
	LOGIN_TIMEOUT,		//超时
	LOGIN_REPEATED,		//重复
	LOGIN_MAX,			//达到用户最大登录总次数
	LOGIN_SUCCESS		//成功
	
}LOGIN_RESULT;


typedef enum			/* 登录类型 */
{
	LOGIN_REQUEST,		//登录请求
	LOGIN_EXIT			//退出请求
	
}LOGIN_TYPE;



typedef struct _aaa_log_request		/* 登录请求 */
{
	uint32_t 	user_id;
	uint32_t 	user_id_buf;
	char 		username[USER_NAME_MAX + 1];
	char 		password[USER_PASSWORD_MAX + 1];
	char 		ttyname[TTY_NAME_LEN_MAX + 1];	//可用于区分telnet/ssh/console
	uint32_t	ip;								//用户登录ip
	uint16_t	port;							//用户登录port
	LOGIN_TYPE	type;
	
}AAA_LOGIN_REQUEST;


typedef struct _aaa_log_response			/* 登录请求应答 */
{
	uint32_t		user_id;
	uint32_t		user_id_buf;
	char 			username[USER_NAME_MAX + 1];
	char 			ttyname[TTY_NAME_LEN_MAX + 1];
	LOGIN_RESULT  	auth_result;
	int				level;
	
}AAA_LOGIN_RESPONSE;




typedef struct _aaa_echo_request		/* 保活 */
{
	uint32_t 		user_id;
	char 			username[USER_NAME_MAX + 1];
	time_t			idle_time;
	
}AAA_ECHO_REQUEST;


typedef struct _aaa_max_idle_msg		/* 用户最大空闲时间 */
{
	time_t			idle_time;
	
}AAA_MAX_IDLE_MSG;



typedef enum		/* 权限提升结果，aaa向vty发送 */
{
	IMPROVE_FAILED,		//密码错误
	IMPROVE_OVERFLOW,	//超过用户最大权限
	IMPROVE_SUCCESS		//成功
	
}IMPROVE_RESULT;

typedef struct _aaa_improve_priv		/* 提升权限 */
{
	uint32_t 	user_id;
	char 		username[USER_NAME_MAX + 1];
	char 		enable_password[USER_PASSWORD_MAX + 1];	//口令密码
	int 		level;									//目标权限
	
}AAA_IMPROVE_LEVEL;

typedef struct _aaa_improve_priv_response		/* 提升权限应答 */
{
	uint32_t 		user_id;
	char 			username[USER_NAME_MAX + 1];
	IMPROVE_RESULT	result;
	int 			level;					//获得的权限
	
}AAA_IMPROVE_LEVEL_RESPONSE;


typedef enum
{
	EXIT_IDLE,			//由于空闲超时退出
	EXIT_ECHO_TIMEOUT,	//超时未收到用户的保活消息
	EXIT_ACCT_FAIL,		//在线情况下出现计费失败

}FORCE_EXIT_REASON;

typedef struct _aaa_force_exit			/* 强制退出 */
{
	uint32_t 			user_id;
	char				username[USER_NAME_MAX + 1];
	FORCE_EXIT_REASON	reason;
}AAA_FORCE_EXIT;



typedef struct _aaa_verify_response
{
	uint32_t 		user_id;
	uint32_t 		user_id_buf;
	char			username[USER_NAME_MAX + 1];
	AAA_MSG_TYPE	msg_type;
	
}AAA_VERIFY_RESPONSE;



/********************************** for dot1x *************************************/

/* 基于 端口、mac 认证 */
typedef enum
{
	BASED_PORT,
	BASED_MAC,
	//BASED_VLAN,
	
}DOT1X_TYPE_BASED;

typedef enum
{
	DOT1X_AUTHOR_ACCEPT,
	DOT1X_AUTHOR_REJECT,
	
}DOT1X_AUTHOR_RESULT;


typedef struct _aaa_dot1x_author_msg
{
	uint32_t 			ifindex;
	char				mac[6];
	DOT1X_TYPE_BASED	type_based;
	DOT1X_AUTHOR_RESULT	author_result;
	
}AAA_DOT1X_AUTHOR_MSG;


typedef struct _aaa_dot1x_cfg_msg
{
	int			state;				//ENABLE,DISABLE
	
}AAA_DOT1X_CFG_MSG;


typedef struct _aaa_dot1x_port_cfg_msg
{
	uint32_t 			ifindex;
	int					state;		//ENABLE,DISABLE
	DOT1X_TYPE_BASED 	type_based;	//现在只有给予port，mac以后扩展

}AAA_DOT1X_PORT_CFG_MSG;


/********************************** for error NO. **********************************/
typedef enum		/* 错误码，aaa与vty之间使用 */
{
	ERR_INVALID = 0,
	ERR_VTY_RESET,			//vty重启
	
}AAA_ERRNO;
	

typedef struct _vty_error_report_msg
{
	AAA_ERRNO 	errNo;		//错误码

}AAA_ERR_REPORT_MSG;


/************************************ end *****************************************/



#endif /* _AAA_COMMON_H_ */

