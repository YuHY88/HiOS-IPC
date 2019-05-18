
#ifndef HIOS_LDP_FSM_H
#define HIOS_LDP_FSM_H

enum LDP_EVENT
{
    LDP_EVENT_INVALID = 0,       /**/
    LDP_EVENT_RECV_DISCOVER,     /* receive a hello packet */
    LDP_EVENT_TCP_FINISHED,      /* TCP link established event */
    LDP_EVENT_RECV_INITAL,       /* Initialized packet received event */
    LDP_EVENT_RECV_KEEPALIVE,    /* First keepalive packet received event */
    LDP_EVENT_KEEPALIVE_TIMEOUT, /* Session keepalive timeout event */
    LDP_EVENT_NEIGHBOR_TIMEOUT,  /* Adjance keepalive timeout event */
    LDP_EVENT_SHUTDOWN,          /* Shutdown event */
    LDP_EVENT_REC_PKT_UNTIMELY   /* Untimely packet received event */
};
void ldp_sesss_bfd_unbind(uint32_t type);
int ldp_fsm_goto_operational(struct ldp_sess *psess);
int ldp_fsm_goto_initialization(struct ldp_sess *psess);
int ldp_session_fsm(enum LDP_EVENT event, struct ldp_sess *psess);

#endif
