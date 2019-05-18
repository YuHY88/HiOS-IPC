
#ifndef HIOS_LDP_PW_H
#define HIOS_LDP_PW_H
#include "ldp_packet.h"

extern struct list ldp_pw_conf_list;    /* ldp pw conf node list */

struct ldp_pwconf
{
    uint32_t     peer_lsrid;
    struct list  pw_list;               /* Multiple PWS can be configed between two LSRs */
};
#define MPLS_MARTINI_PW_RECONNECT_NUM_MAX       3
#define MPLS_MARTINI_PW_UP                      0
#define MPSL_MARTINI_PW_DOWN                    1

struct ldp_pwinfo
{
    uint8_t             reconnect_num;
    uint8_t             state;         /* true for false */
    char                reason[128];
    
    uint8_t             req_flag;       /* request flag */
    struct l2vc_entry  *local_pwindex;  /* Local PW configuration */
    struct l2vc_entry   peer_pwinfo;    /* Peer PW configuration */
};

int ldp_sess_pws_recorect_timer(struct thread *thread);

int pw_join_ldp(struct l2vc_entry *pl2vc);
int pw_leave_ldp(struct l2vc_entry *pl2vc);

struct ldp_pwconf *ldp_pw_conf_node_add(struct l2vc_entry *pl2vc);
int ldp_pw_conf_node_del(struct ldp_pwconf *ppwconf);
struct ldp_pwconf *ldp_pw_conf_node_lookup(uint32_t peer_lsrid);
struct ldp_pwinfo *ldp_sess_pw_lookup(struct ldp_sess *psess, struct l2vc_entry *ppw);

struct ldp_pwinfo *ldp_sess_add_pw(struct ldp_sess *psess, struct l2vc_entry *local_pw, struct l2vc_entry *peer_pw);
int ldp_sess_del_pw(struct ldp_sess *psess, struct ldp_pwinfo *pldp_pwinfo, uint32_t flag);
int ldp_sess_pws_negotiate_start(struct ldp_sess *psess);
int ldp_sess_pws_negotiate_end(struct ldp_sess *psess);

int ldp_recv_pw_request(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg);
int ldp_recv_pw_maping(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg);
int ldp_recv_pw_withdraw(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg);
int ldp_recv_pw_release(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg);

int ldp_send_pw_request(struct ldp_sess *psess, struct l2vc_entry *pl2vc);
int ldp_send_pw_mapping(struct ldp_sess *psess, struct l2vc_entry *pl2vc);
int ldp_send_pw_withdraw(struct ldp_sess *psess, struct l2vc_entry *pl2vc);
int ldp_send_pw_release(struct ldp_sess *psess, struct l2vc_entry *pl2vc);

int ldp_sess_add_pws(struct ldp_sess *psess, struct ldp_pwconf *ppwconf);
int ldp_sess_del_pws(struct ldp_sess *psess);


#endif
