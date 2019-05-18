/******************************************************************************
 * Filename: aaa_tac_plus_common.h
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

#ifndef _AAA_TAC_PLUS_COMMON_H_
#define _AAA_TAC_PLUS_COMMON_H_

#include <time.h>
#include <lib/types.h>
#include <lib/aaa_common.h>

#include "tacplus.h"
#include "aaa.h"


#define DEBUGTAC


/* libtac.h */
struct tac_attrib {
    char *attr;
    char attr_len;
    struct tac_attrib *next;
};

struct areply {
    struct tac_attrib *attr;
    char *msg;
    int status : 8;
    int flags : 8;
    int seq_no : 8;
};

#ifndef TAC_PLUS_MAX_PACKET_SIZE
#define TAC_PLUS_MAX_PACKET_SIZE 128000 /* bytes */
#endif

#ifndef TAC_PLUS_MAX_ARGCOUNT
#define TAC_PLUS_MAX_ARGCOUNT 100 /* maximum number of arguments passed in packet */
#endif

#ifndef TAC_PLUS_PORT
#define	TAC_PLUS_PORT 49  /* default port */
#endif

#define TAC_PLUS_DEF_SERV_IPV4          0x00000000
#define TAC_PLUS_DEF_SHARED_SECRET      "secret"
#define TAC_PLUS_SEND_PKT_LEN_MAX       4095


#define TAC_PLUS_RSP_TIMEOUT  			6    	//n*500ms
#define TAC_PLUS_DEF_UPDATE_INTERVAL 	600		//default update interval 600s

#define TAC_PLUS_DEF_ACCT_TIMEOUT		30		//在线检测计费失败的超时时间5s


/* Internal status codes 
 *   all negative, tacplus status codes are >= 0
 */
#define LIBTAC_STATUS_ASSEMBLY_ERR  -1
#define LIBTAC_STATUS_PROTOCOL_ERR  -2
#define LIBTAC_STATUS_READ_TIMEOUT  -3
#define LIBTAC_STATUS_WRITE_TIMEOUT -4
#define LIBTAC_STATUS_WRITE_ERR     -5
#define LIBTAC_STATUS_SHORT_HDR     -6
#define LIBTAC_STATUS_SHORT_BODY    -7
#define LIBTAC_STATUS_CONN_TIMEOUT  -8
#define LIBTAC_STATUS_CONN_ERR      -9

#define PROTOCOL_ERR_MSG         "(Protocol error)"
#define AUTHEN_SYSERR_MSG        "(Authentication system error)"
#define AUTHOR_OK_MSG            "(Service granted)"
#define AUTHOR_FAIL_MSG          "(Service not allowed)"
#define AUTHOR_ERR_MSG           "(Service not allowed. Server error)"
#define AUTHOR_SYSERR_MSG        "(Authorization system error)"
#define ACCT_OK_MSG              "(Accounted ok)"
#define ACCT_FAIL_MSG            "(Accounting failed)"
#define ACCT_ERR_MSG             "(Accounting failed. Server error)"
#define ACCT_SYSERR_MSG          "(Accounting system error)"

/* TACACS+ session recorder */
#define TAC_PLUS_SESS_MAX        1024

typedef struct _tacacs_session_info_
{	
	time_t  		authen_timeleft;
	time_t  		author_timeleft;
	time_t  		acct_timeleft;
	time_t			acct_update_time;
	time_t			acct_timeleft_response;
	time_t			acct_failed_flag;
	int				acct_send_flag;			//已发送计费报文但尚未收到应答标志

	/* authen/author/acct info */
	uint16_t			authen_port;
	uint16_t			author_port;
	uint16_t			acct_port;
	
	TAC_HDR				authen_hdr;
	TAC_HDR				author_hdr;
	TAC_HDR				acct_hdr;
	struct authen_start authen_pkt;
	struct author		author_pkt;
	struct acct			acct_pkt;
	char				authen_data[1024];
	size_t				authen_data_size;
	char				author_data[1024];
	size_t				author_data_size;
	char				acct_data[1024];
	size_t				acct_data_size;

	AAA_RESULT			authen_result;		//authen result
	AAA_RESULT			author_result;		//author result
	AAA_RESULT			acct_result;		//acct	 result
	
}TACACS_SESSION_INFO;


#endif  /* _AAA_TAC_PLUS_COMMON_H_ */

