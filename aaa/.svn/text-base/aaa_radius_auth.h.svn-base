/******************************************************************************
 * Filename: aaa_radius_auth.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.4.25  lipf created
 *
******************************************************************************/

#ifndef _AAA_RADIUS_AUTH_H_
#define _AAA_RADIUS_AUTH_H_

#include "aaa_radius.h"

int radius_pap_password_calc(char *pw_output, char *pw_input, RADIUS_SEND_PKT *auth_pkt);
int radius_chap_password_calc(char *pw_output, char *pw_input, uchar chap_id, RADIUS_SEND_PKT *auth_pkt);
uchar radius_chap_id_create (void);

int radius_process_rcv_auth_pkt(RC_RCV_PAYLOAD *pradius_rcv_buf);

int radius_store_auth_info_into_session(USER_AUTHING_INFO* pinfo);
int radius_auth_info_to_request_pkt(RADIUS_SEND_PKT *pkt, char *send_buf);
int radius_auth_request_send(RADIUS_SEND_PKT *pkt);

int radius_auth_pkt_fill_id (USER_AUTHING_INFO *pinfo);
int radius_auth_pkt_fill_length (RADIUS_SEND_PKT *pkt);
int radius_auth_pkt_fill_authenticator (USER_AUTHING_INFO *psess);
int radius_fill_auth_head (USER_AUTHING_INFO *pinfo);



#endif  /* _AAA_RADIUS_AUTH_H_ */

