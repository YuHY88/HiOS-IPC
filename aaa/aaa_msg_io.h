/******************************************************************************
 * Filename: aaa_msg_io.h
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

#ifndef _AAA_MSG_IO_H_
#define _AAA_MSG_IO_H_

#include <lib/aaa_common.h>
#include <lib/msg_ipc_n.h>

int aaa_msg_rcv (void *para);
int aaa_msg_rcv_n(struct ipc_mesg_n *pmsg, int imlen);

int aaa_send_login_response (const char *username, uint32_t user_id,uint32_t user_id_buf,
	LOGIN_RESULT result, int level);
int aaa_send_improve_privilege_response (const char *username, uint32_t user_id, 
									IMPROVE_RESULT result, int privilege);
int aaa_send_force_exit_request (const char *username, uint32_t user_id, FORCE_EXIT_REASON reason);

int aaa_send_dot1x_author_response (AAA_DOT1X_AUTHOR_MSG *msg);
int aaa_send_dot1x_cfg_to_hal (int state);
int aaa_send_dot1x_port_cfg_to_hal (uint32_t ifindex, int state, DOT1X_TYPE_BASED type_based);


#endif /* _AAA_MSG_IO_H_ */

