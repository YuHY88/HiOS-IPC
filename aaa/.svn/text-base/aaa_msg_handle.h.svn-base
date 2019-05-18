/******************************************************************************
 * Filename: aaa_msg_handle.h
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

#ifndef _AAA_MSG_HANDLE_H_
#define _AAA_MSG_HANDLE_H_

#include <lib/aaa_common.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>
#include "aaa_user_manager.h"

int aaa_handle_login_request(struct ipc_mesg_n *pmsg_ipc);
int aaa_auth_process_check(void *para);
int aaa_online_user_status_check(void *para);

void aaa_handle_echo_request(struct ipc_mesg_n *pmsg_ipc);
void aaa_handle_improve_level_request(struct ipc_mesg_n *pmsg_ipc);
void aaa_handle_vty_msg(struct ipc_mesg_n *pmsg);
void aaa_handle_errno(struct ipc_mesg_n *pmsg_ipc);

void aaa_method_radius_process_check(USER_AUTHING_INFO *pinfo);
void aaa_method_tacacs_process_check(USER_AUTHING_INFO *pinfo);
void aaa_method_local_process_check(USER_AUTHING_INFO *pinfo);
void aaa_method_none_process_check(USER_AUTHING_INFO *pinfo);

void aaa_handle_auth_result(USER_AUTHING_INFO *pinfo);
int aaa_user_login_repeat(const char *username, LOG_METHOD login_method);

void aaa_send_verify_msg (struct ipc_mesg_n *pmsg_ipc, uint32_t user_id, uint32_t user_id_buf,
					char *username, AAA_MSG_TYPE msg_type);
void aaa_send_verify_msg_noack (struct ipc_mesg_n *pmsg_ipc);

int aaa_send_improve_level_response (const char *username, uint32_t user_id, 
									IMPROVE_RESULT result, int level);

int aaa_check_local_user_service(char *tty, int service_type);

int aaa_send_max_idle_time_msg(time_t time);


#endif /* _AAA_MSG_HANDLE_H_ */

