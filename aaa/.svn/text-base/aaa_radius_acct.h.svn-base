/******************************************************************************
 * Filename: aaa_radius_acct.h
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

#ifndef _AAA_RADIUS_ACCT_H_
#define _AAA_RADIUS_ACCT_H_

int radius_acct_pkt_fill_id(RADIUS_SEND_PKT *pkt);
int radius_acct_pkt_fill_length(RADIUS_SEND_PKT *pkt);
int radius_acct_pkt_fill_authenticator(RADIUS_SEND_PKT *pkt);
int radius_acct_session_id_get(char *session_id);
int radius_acct_session_id_update(RADIUS_SESSION_INFO* psess);

int radius_update_acct_pkt(RADIUS_SEND_PKT* pkt);

int radius_acct_switch_type_set(struct list *ls, int status);
int radius_acct_switch_type_get(RADIUS_SESSION_INFO *psess);
int radius_acct_info_to_request_pkt(RADIUS_SEND_PKT *pkt, char *send_buf);
int radius_acct_request_send(RADIUS_SEND_PKT *pkt, int status);

int radius_store_acct_info_into_session(USER_AUTHING_INFO* pinfo);
int radius_acct_status_type_set(struct list *ls, int status);
int radius_acct_status_type_get(RADIUS_SESSION_INFO *psess);

int radius_process_rcv_acct_pkt(RC_RCV_PAYLOAD *pradius_rcv_buf);


#endif  /* _AAA_RADIUS_ACCT_H_ */

