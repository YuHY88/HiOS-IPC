#ifndef SERVERLOOP_H
#define SERVERLOOP_H

extern void server_loop2(Authctxt *authctxt, struct ssh_packet *ssh_packet);
extern void *vty_ssh_cmd(void *arg);
extern int netconf_pkt_send(struct login_session *session, unsigned char *data, int len);
extern int netconf_pkt_send_n(struct login_session *session, unsigned char *data, int len);
extern int netconf_msg_test_channel(Channel *c);
extern void *netconf_msg_handle(void *arg);
extern void session_thread_req(Channel *c, struct ssh_packet *ssh_packet);
extern int session_vty_req(struct ssh_packet *ssh_packet, Channel *c);


#endif

