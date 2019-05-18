
#ifndef HIOS_LDP_NEIGHBOR_H
#define HIOS_LDP_NEIGHBOR_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/hptimer.h>
#include "ldp.h"


struct list ldp_remot_adjance_list;

struct list ldp_local_addr_list;

struct ldp_if
{
    uint16_t         ldp_enable;
    
    uint16_t         hello_send_config;
    uint16_t         hello_send_effect;
    
    TIMERID          phello_thread;
    struct list      neighbor_list;
};
struct ldp_taddr
{
    uint32_t local_addr;
    uint32_t peer_addr;
    uint32_t priority;
};

struct ldp_adjance
{
    uint8_t          type;          /* Remote or local */
    uint32_t         ifindex;       /* Local interface index */
    uint32_t         local_lsrid;   /* Local mpls identify*/
    uint32_t         peer_lsrid;    /* Peer mpls identify */
    uint32_t         peer_ip;       /* The sip of the hello packete from the peer */
    uint16_t         hello_send;    /* The hello send time used by target hello */
    uint16_t         neighbor_hold; /* The keepalvie time of the adjance */
    uint32_t         rcv_cnt;       /* hello pkt received counter */
    long             creat_time;      /* Adjance create time */
    TIMERID          phello_timer;  /* hello send timer */
    TIMERID          phold_timer;   /* hello hold timer */

    struct ldp_taddr tcp_addr;
    struct ldp_sess *psess;         /* The ldp session of the adjance */
};

int ldp_adjance_link_hello_timer(void *para);
int ldp_adjance_target_hello_timer(void *para);
int ldp_adjance_hold_timer(void *para);

struct ldp_adjance *ldp_adjance_create(uint32_t local_lsrid, uint32_t peer_lsrid, uint32_t peer_ifaddr, uint32_t ifindex, uint8_t type);
int ldp_adjance_add(struct ldp_adjance *padjance, struct list *plist);
int ldp_adjance_delete(struct ldp_adjance *padjance, struct list *plist);
struct ldp_adjance *ldp_adjance_lookup(uint32_t peer_lsrid, uint32_t local_ifindex, uint32_t peer_ifaddr, uint8_t type);

int ldp_adjance_join_sess(struct ldp_adjance *padjance, struct ldp_sess *psess);
void ldp_adjance_leave_sess(struct ldp_adjance *padjance, struct ldp_sess *psess, uint32_t event);

int ldp_adjance_hello_internal_recalculate(struct list *plist);

#endif
