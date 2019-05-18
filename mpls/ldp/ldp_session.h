/*
*LDP SEESION.H
*/
#ifndef HIOS_LDP_SESSION_H
#define HIOS_LDP_SESSION_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/mpls_common.h>
#include "ldp.h"
#include "ldp_adjance.h"

/* ldp status */
enum LDP_STATUS
{
    LDP_STATUS_NOEXIST = 0,   /* nonexist */
    LDP_STATUS_INIT,          /* initialize */
    LDP_STATUS_OPENSEND,      /* opensend */
    LDP_STATUS_OPENREC,       /* openrec */
    LDP_STATUS_UP             /* operational */
};

struct ldp_sess_key
{
    uint32_t peer_lsrid;      /* The mpls identify of peer */
    uint32_t local_lsrid;     /* The mpls identify of local */
};

struct ldp_backooff
{
    uint32_t       backoff_min;
    uint32_t       backoff_max;
    uint32_t       backoff_run;
    TIMERID        pbackoff_thread;
};
#define LDP_SESS_RECONNECT_NUM_INCREASE(v)      ((v)->reconnect_num ++)

#define LDP_SESS_REASON_MAX_LEN                 256

enum LDP_TCP_STATUS
{
    LDP_TCP_LINK_DOWN = 0,
    LDP_TCP_LINK_UP
};
    
/* ldp session */
struct ldp_sess
{
    struct ldp_sess_key     key;                /* Key of ldp session for hash */
    struct ldp_taddr        tcp_addr;           /* TCP session sip and dip */
    uint16_t                sport;              /* sport of the tcp link for the ldp session */
    uint16_t                dport;              /* dport of the tcp link for the ldp session */
    uint16_t                tcp_status;         /* tcp session status flag */

    enum LDP_ROLE           role;               /* active or passive for start ldp negotiation */
    uint8_t                 type;               /* ldp session type */
    enum LDP_STATUS         status;             /* ldp session status */

    struct list             adjacency_list;     /* A session will get at least one ldp adjance ,remote or local */
    struct list             pwlist;             /* A session can provide negotiation services for multiple PWs .*/
    struct list             feclist;            /**/
    struct list             peer_ifaddrs;       /**/

    uint32_t                kasnd_cnt;          /**/
    uint32_t                karcv_cnt;
    uint32_t                maxpdu;             /**/
    uint32_t                sess_hold;          /* ldp session hold time */

    TIMERID                 pkeepalive_timer;   /* Session keepalive timer */
    TIMERID                 phold_timer;        /* Session keepalive hold timer */

    uint16_t                bfd_sess;           /* BFD index */
    long                    creat_time;         /* Time of ldp session create */
    struct ldp_backooff     backoff_time;       /* */
    uint32_t                reconnect_num;
    char                    reason[LDP_SESS_REASON_MAX_LEN];             /* record reason of session down */
};



extern struct hash_table ldp_session_table;     /* ldp session hash table */

int ldp_sess_keepalive_timer(void *para);
int ldp_session_hold_timer(void *para);

void ldp_session_table_init(unsigned int size);
struct ldp_sess *ldp_session_create(uint32_t peer_ip, uint32_t local_lsrid);
int ldp_session_add(struct ldp_sess *psess);
int ldp_session_delete(struct ldp_sess *psess);
void ldp_session_down(struct ldp_sess *psess); 
void ldp_session_clear(void); 
void ldp_sess_label_maping_clear(struct ldp_sess *psess);
int ldp_sess_adjances_clear(struct ldp_sess     *psess, uint32_t event);

struct ldp_sess *ldp_session_lookup(uint32_t local_lsrid, uint32_t peer_lsrid); 
void ldp_session_neighbor_priority_zero(struct ldp_sess *psess);
struct ldp_adjance *ldp_session_select_neighbor(struct ldp_sess *psess);

int ldp_session_backoff_timer(void *para);
void ldp_sess_process_tcp_event(void *ptcp, uint8_t event);

void ldp_tcp_port_alloc(struct ldp_sess *psess);
void ldp_tcp_port_relese(struct ldp_sess *psess);

#endif

