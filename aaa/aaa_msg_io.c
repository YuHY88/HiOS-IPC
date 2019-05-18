/******************************************************************************
 * Filename: aaa_msg_io.c
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

//#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <lib/thread.h>
#include <netinet/in.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>

#include <lib/memtypes.h>
#include <lib/memshare.h>
#include <lib/msg_ipc_n.h>

#include "aaa_common.h"
#include "vty.h"
#include "aaa_packet.h"
#include "aaa_radius.h"
#include "aaa_radius_auth.h"
#include "aaa_tac_plus.h"
#include "aaa_snmp.h"
#include "aaa_msg_io.h"

extern struct thread_master *aaa_master;


/* receive common IPC messages */
int aaa_msg_rcv(void *para)
{
#if 0
    static struct ipc_mesg mesg;
	memset (&mesg, 0, sizeof (struct ipc_mesg));

    if (ipc_recv_common (&mesg, MODULE_ID_AAA) == -1)
    {
		return 0;
    }

	switch (mesg.msghdr.msg_type)
	{
		case IPC_TYPE_VTY:
			//aaa_handle_vty_msg (&mesg);
			break;

		case IPC_TYPE_SNMP:
			//aaa_handle_snmp_msg (&mesg);
			break;

		default:
			break;
	}
#endif

	return 0;
}


/* receive common IPC messages */
int aaa_msg_rcv_n(struct ipc_mesg_n *pmsg, int imlen)
{
    if(NULL == pmsg)
	{
		return 0;
	}

	int retva = 0;
    int revln = 0;

    //printf("aaa recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        /* æ¶ˆæ¯å¤„ç† */
        switch(pmsg->msghdr.msg_type)
        {
            case IPC_TYPE_VTY:
				aaa_handle_vty_msg (pmsg);
				break;

			case IPC_TYPE_SNMP:
				aaa_handle_snmp_msg (pmsg);
				break;

			case IPC_TYPE_PACKET:	//from ftm, udp/tcp
				aaa_handle_ftm_msg(pmsg);
				break;

			default:
                 printf("aaa_msg_rcv, receive unk message\r\n");
                 break;
        }
    }
    else
    {
         printf("aaa recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

	if(pmsg)
	{
    	mem_share_free(pmsg, MODULE_ID_AAA);
	}
    
    return retva;
}





/* å‘é?è®¤è¯åº”ç­”æ¶ˆæ? */
int aaa_send_login_response (const char *username, uint32_t user_id, uint32_t user_id_buf,
						LOGIN_RESULT result, int level)
{
	AAA_LOGIN_RESPONSE log_rep;
	memset(&log_rep, 0, sizeof(AAA_LOGIN_RESPONSE));
	memset(log_rep.username, 0, sizeof(log_rep.username));
	memset(log_rep.ttyname, 0, sizeof(log_rep.ttyname));

	log_rep.user_id = user_id;
	log_rep.user_id_buf = user_id_buf;
	log_rep.auth_result = result;
	log_rep.level = level;
	
	if(username != NULL)
		memcpy (log_rep.username, username, strlen(username));

	switch (result)
	{
		case LOGIN_FAILED:
			zlog_debug(AAA_DBG_COMMON, "send result to vty, user[name:%s][id_buf:%d] login failed\n", username, user_id_buf);
			break;

		case LOGIN_TIMEOUT:
			zlog_debug(AAA_DBG_COMMON, "send result to vty, user[name:%s][id_buf:%d] login timeout\n", username, user_id_buf);
			break;

		case LOGIN_REPEATED:
			zlog_debug(AAA_DBG_COMMON, "send result to vty, user[name:%s][id_buf:%d] login repeated\n", username, user_id_buf);
			break;

		case LOGIN_SUCCESS:
			zlog_debug(AAA_DBG_COMMON, "send result to vty, user[name:%s][id:%x][id_buf:%d][priv:%d] login success\n",\
				username, user_id, user_id_buf, log_rep.level);
			break;

		default:
			break;
	}

#if 0
	
	if(ipc_send_common(&log_rep, sizeof(log_rep), 1, MODULE_ID_VTY,
			MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0) == -1)
	{
		zlog_debug(0, "%s[%d] : aaa send login result to vty failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}

#endif


	/*AAA_VERIFY_RESPONSE *pmsg_recv = (AAA_VERIFY_RESPONSE *) ipc_send_common_wait_reply (&log_rep, sizeof (AAA_LOGIN_RESPONSE), 1, 
		 MODULE_ID_VTY, MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0, 0);*/

	struct ipc_mesg_n* pmsg = ipc_sync_send_n2(&log_rep, sizeof (AAA_LOGIN_RESPONSE), 1, 
		 MODULE_ID_VTY, MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0, 0, 1000);

	AAA_VERIFY_RESPONSE *pmsg_recv = NULL;

	if(NULL == pmsg)
	{
		return AAA_IPC_FAIL;
	}
	
    if (0 != pmsg->msghdr.data_num)
	{
		pmsg_recv = (AAA_VERIFY_RESPONSE *)pmsg->msg_data;
			
		if ((pmsg_recv->user_id != log_rep.user_id)
			|| (pmsg_recv->user_id_buf != log_rep.user_id_buf)
			|| (0 != strcmp (pmsg_recv->username, log_rep.username))
			|| (AAA_MSG_AUTH_RESPONSE != pmsg_recv->msg_type))
		{
			zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa send login response to vty failed, user[%s][%d][%x]",
				__func__, __LINE__, log_rep.username, log_rep.user_id_buf, log_rep.user_id);
			if(pmsg)
			{
				mem_share_free(pmsg, MODULE_ID_AAA);
			}
			return AAA_IPC_FAIL;
		}
		else
		{
			zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa send login response to vty success, user[%s][%d][%x]",
				__func__, __LINE__, log_rep.username, log_rep.user_id_buf, log_rep.user_id);
			if(pmsg)
			{
				mem_share_free(pmsg, MODULE_ID_AAA);
			}
			return AAA_OK;
		}
	}
	else
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa send login response to vty failed, user[%s][%d][%x]",
			__func__, __LINE__, log_rep.username, log_rep.user_id_buf, log_rep.user_id);
		if(pmsg)
		{
			mem_share_free(pmsg, MODULE_ID_AAA);
		}
		return AAA_IPC_FAIL;
	}
}


/* å‘é?ç”¨æˆ·æƒé™æå‡åº”ç­”æ¶ˆæ? */
int aaa_send_improve_level_response (const char *username, uint32_t user_id, 
									IMPROVE_RESULT result, int level)
{
	AAA_IMPROVE_LEVEL_RESPONSE msg;
	memset(&msg, 0, sizeof(AAA_IMPROVE_LEVEL_RESPONSE));

	if(username != NULL)
		memcpy(msg.username, username, strlen(username));
	msg.user_id = user_id;	
	msg.result = result;
	msg.level = level;

	/*if(ipc_send_common(&msg, sizeof(AAA_FORCE_EXIT), 1, MODULE_ID_VTY,
						MODULE_ID_AAA, IPC_TYPE_AAA,
						AAA_MSG_IMPROVE_LEVEL_RESPONSE, 0) == -1)
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : aaa send improve level response to vty failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}*/

	ipc_send_reply_n2((void *)&msg, sizeof(AAA_FORCE_EXIT), 1, MODULE_ID_VTY,
		MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_IMPROVE_LEVEL_RESPONSE, 0, 0, IPC_OPCODE_REPLY);

	return AAA_OK;
}



/* å‘é?å¼ºåˆ¶é??å‡ºæ¶ˆæ? */
int aaa_send_force_exit_request (const char *username, uint32_t user_id, FORCE_EXIT_REASON reason)
{
	AAA_FORCE_EXIT msg;
	memset(&msg, 0, sizeof(AAA_FORCE_EXIT));

	msg.user_id = user_id;
	msg.reason = reason;

	if(username != NULL)
		memcpy(msg.username, username, strlen(username));

#if 0		
	if (ipc_send_common(&msg, sizeof (AAA_FORCE_EXIT), 1, MODULE_ID_VTY,
						MODULE_ID_AAA, IPC_TYPE_AAA,
						AAA_MSG_FORCE_EXIT, 0) == -1)
	{
		zlog_debug(0, "%s[%d] : aaa send force exit to vty failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}
#endif
	/*AAA_VERIFY_RESPONSE *pmsg_recv = (AAA_VERIFY_RESPONSE *)ipc_send_common_wait_reply (&msg, sizeof (AAA_FORCE_EXIT), 1, 
		 MODULE_ID_VTY, MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0, 0);*/

	struct ipc_mesg_n* pmsg = ipc_sync_send_n2(&msg, sizeof (AAA_FORCE_EXIT), 1, 
		 MODULE_ID_VTY, MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0, 0, 1000);

	AAA_VERIFY_RESPONSE *pmsg_recv = NULL;

	if(NULL == pmsg)
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : vty exit user[%s][%x] error",
			__func__, __LINE__, msg.username, msg.user_id);
		
		return AAA_FAIL;
	}
	
    if (0 != pmsg->msghdr.data_num)
	{
		pmsg_recv = (AAA_VERIFY_RESPONSE *)pmsg->msg_data;
		
		if ((pmsg_recv->user_id != msg.user_id)
			|| (0 != strcmp (pmsg_recv->username, msg.username))
			|| (AAA_MSG_FORCE_EXIT != pmsg_recv->msg_type))
		{
			zlog_debug(AAA_DBG_COMMON, "%s[%d] : vty exit user[%s][%x] error",
				__func__, __LINE__, msg.username, msg.user_id);
			if(pmsg)
			{
				mem_share_free(pmsg, MODULE_ID_AAA);
			}
			return AAA_FAIL;
		}
		else
		{
			zlog_debug(AAA_DBG_COMMON, "%s[%d] : vty exit user[%s][%x] success",
				__func__, __LINE__, msg.username, msg.user_id);
			if(pmsg)
			{
				mem_share_free(pmsg, MODULE_ID_AAA);
			}
			return AAA_OK;
		}
	}
	else
	{
		zlog_debug(AAA_DBG_COMMON, "%s[%d] : vty exit user[%s][%x] success",
			__func__, __LINE__, msg.username, msg.user_id);
		if(pmsg)
		{
			mem_share_free(pmsg, MODULE_ID_AAA);
		}
		return AAA_OK;
	}
}




/* å‘halå‘é?? port/mac æŽˆæƒä¿¡æ¯ */
int aaa_send_dot1x_author_response (AAA_DOT1X_AUTHOR_MSG *msg)
{
	/*if (ipc_send_common(&msg, sizeof (AAA_DOT1X_AUTHOR_MSG), 1, MODULE_ID_HAL,
						MODULE_ID_AAA, IPC_TYPE_AAA,
						AAA_MSG_DOT1X_AUTHOR_RESPONSE, 0) == -1)
	{
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] : aaa send dot1x author response to hal failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}*/

	ipc_send_reply_n2((void *)&msg, sizeof (AAA_DOT1X_AUTHOR_MSG), 1, MODULE_ID_HAL,
		MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_DOT1X_AUTHOR_RESPONSE, 0, 0, IPC_OPCODE_REPLY);

	return AAA_OK;
}



/* å‘halå‘é?? eap å…¨å±€é…ç½®ä¿¡æ¯ */
int aaa_send_dot1x_cfg_to_hal (int state)
{
	AAA_DOT1X_CFG_MSG msg;
	msg.state = state;
	
	/*if (ipc_send_common(&msg, sizeof (AAA_DOT1X_CFG_MSG), 1, MODULE_ID_HAL,
						MODULE_ID_AAA, IPC_TYPE_AAA,
						AAA_MSG_DOT1X_CFG, 0) == -1)
	{
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] : aaa send dot1x port config information to hal failed\n", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}*/

	ipc_send_msg_n2(&msg, sizeof (AAA_DOT1X_CFG_MSG), 1, MODULE_ID_HAL,
		MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_DOT1X_CFG, 0, 0);
	
	return AAA_OK;
}


/* å‘halå‘é?? port eap é…ç½®ä¿¡æ¯ */
int aaa_send_dot1x_port_cfg_to_hal (uint32_t ifindex, int state, DOT1X_TYPE_BASED type_based)
{
	AAA_DOT1X_PORT_CFG_MSG msg;
	msg.state = state;
	
	/*if (ipc_send_common(&msg, sizeof (AAA_DOT1X_PORT_CFG_MSG), 1, MODULE_ID_HAL,
						MODULE_ID_AAA, IPC_TYPE_AAA,
						AAA_MSG_DOT1X_PORT_CFG, 0) == -1)
	{
		zlog_debug(AAA_DBG_DOT1X, "%s[%d] : aaa send dot1x port config information to hal failed", __FUNCTION__, __LINE__);
		return AAA_IPC_FAIL;
	}*/
	
	ipc_send_msg_n2(&msg, sizeof (AAA_DOT1X_PORT_CFG_MSG), 1, MODULE_ID_HAL,
		MODULE_ID_AAA, IPC_TYPE_AAA, AAA_MSG_DOT1X_PORT_CFG, 0, 0);
						
	zlog_debug(AAA_DBG_DOT1X, "%s[%d] : aaa send dot1x port config information to hal success", __FUNCTION__, __LINE__);
	return AAA_OK;
}




