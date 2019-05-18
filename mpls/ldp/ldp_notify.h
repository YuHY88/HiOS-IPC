
#ifndef HIOS_LDP_NOTIFY_H
#define HIOS_LDP_NOTIFY_H

int ldp_send_notification(struct ldp_sess *psess, uint32_t err_code, uint32_t msgid);

int ldp_recv_notification(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb);

#endif
