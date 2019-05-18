/*
*   define of ldp 
*/

#ifndef HIOS_LDP_LABEL_H
#define HIOS_LDP_LABEL_H
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_fec.h"
int ldp_recv_fec_request(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess);
int ldp_recv_fec_req_abort(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess);
int ldp_recv_fec_mapping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess);
int ldp_recv_fec_withdraw(struct ldpmsg_base *pbasemsg, struct ldp_sess *psess);
int ldp_recv_fec_release(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess);

int ldp_send_fec_request(struct ldp_sess *psess, struct inet_prefix *prefix);
int ldp_send_fec_req_abort(struct ldp_sess *psess, struct inet_prefix *prefix);
int ldp_send_fec_mapping(struct ldp_sess *psess, struct lsp_fec_t *pfec);
int ldp_send_fec_withdraw(struct ldp_sess *psess, struct lsp_fec_t *pfec);
int ldp_send_fec_release(struct ldp_sess *psess, struct ldp_fec_t *pfec, uint32_t outlabel);

int ldp_fec_maping_broadcast(struct lsp_fec_t *pfec);
int ldp_fec_withdraw_broadcast(struct lsp_fec_t *pfec);


#endif
