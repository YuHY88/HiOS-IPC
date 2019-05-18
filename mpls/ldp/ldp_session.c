
#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <mpls/mpls.h>
#include "mpls_main.h"
#include <mpls/mpls_if.h>
#include <mpls/lsp_fec.h>
#include <ftm/pkt_tcp.h>
#include <mpls/lspm.h>
#include <mpls/labelm.h>
#include <lib/ifm_common.h>

#include "ldp.h"
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_fsm.h"
#include "ldp_notify.h"
#include "ldp_fec.h"
#include "ldp_pw.h"

struct hash_table ldp_session_table;

/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static unsigned int ldp_session_compute_hash(void *hash_key)
{
    unsigned int hash_value;
    hash_value = ((struct ldp_sess_key *)hash_key)->local_lsrid
                 + ((struct ldp_sess_key *)hash_key)->peer_lsrid;

    return hash_value;
}

/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static int ldp_session_compare(void *item, void *hash_key)
{
    struct ldp_sess_key *pkey = NULL;
    struct ldp_sess_key *pkey_old = NULL;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pkey = (struct ldp_sess_key *)hash_key;
    pkey_old = (struct ldp_sess_key *)(((struct hash_bucket *)item)->hash_key);
    if (NULL == pkey_old)
    {
        return ERRNO_FAIL;
    }

    if ((pkey->peer_lsrid == pkey_old->peer_lsrid) && (pkey->local_lsrid == pkey_old->local_lsrid))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}
/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/

void ldp_session_table_init(unsigned int size)
{
    hios_hash_init(&ldp_session_table, size, ldp_session_compute_hash, ldp_session_compare);
}

/**
* @brief      <+创建一个LDP SESSION 节点+>
* @param[in ] <+peer_lsrid:对端LSRID local_lsrid:本端LSRID+>
* @param[out] <+none+>
* @return     <+LDP会话结构+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_sess *ldp_session_create(uint32_t peer_lsrid, uint32_t local_lsrid)
{
    struct ldp_sess *psess = NULL;
    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, "create session peer lsrid = %s localip %s.\n", ldp_ipv4_to_str(peer_lsrid), ldp_ipv4_to_str2(local_lsrid));

    psess = (struct ldp_sess *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_sess));
    if (NULL == psess)
    {
        return NULL;
    }
    memset(psess, 0, sizeof(struct ldp_sess));
    psess->key.local_lsrid = local_lsrid;
    psess->key.peer_lsrid = peer_lsrid;
    psess->status = LDP_STATUS_NOEXIST;
    psess->sess_hold = LDP_SESSION_EXPIRE_TIME;
    psess->backoff_time.backoff_max = LDP_SESSION_BACKOFF_TIME_MAX;
    psess->backoff_time.backoff_min = LDP_SESSION_BACKOFF_TIME_MIN;
    psess->backoff_time.backoff_run = LDP_SESSION_BACKOFF_TIME_MIN;
    snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for tcp down\n" );
    time(&psess->creat_time);

    if (ERRNO_SUCCESS != ldp_session_add(psess))
    {
        XFREE(MTYPE_LDP_ENTRY, psess);
        return NULL;
    }

    return psess;
}
/**
* @brief      <+将一个LDP 会话节点加入会话HASH表+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_session_add(struct ldp_sess *psess)
{
    struct hash_bucket *pitem = NULL;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer lsrid = %s.\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    if (ldp_session_table.hash_size == ldp_session_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }
    if(NULL == pitem->data)
    {
        pitem->hash_key = &(psess->key);
        pitem->data = psess;

        hios_hash_add(&ldp_session_table, pitem);
        MPLS_LDP_SESS_CNT_INCREASE(gldp);
    }
    else
    {
        MPLS_LOG_ERROR("add ldp session to hash failed for get hashbacket data != NULL\n");
    }

    return ERRNO_SUCCESS;

}

/**
* @brief      <+删除一个LDP会话节点+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_session_delete(struct ldp_sess *psess)
{
    struct hash_bucket *pitem = NULL;
    struct ldp_sess_key key = {0};

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "sesson peerlsrid %s\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    key.local_lsrid = psess->key.local_lsrid;
    key.peer_lsrid = psess->key.peer_lsrid;

    pitem = hios_hash_find(&ldp_session_table, &key);
    if ((NULL == pitem) || (NULL == pitem->data))
    {
        MPLS_LDP_ERROR("error delete ldp session for find pitem data NULL\n");
        return ERRNO_FAIL;
    }
    psess = pitem->data;

    /* cancel the pending session backoff timer */
    if(psess->backoff_time.pbackoff_thread)
    {
        MPLS_TIMER_DEL(psess->backoff_time.pbackoff_thread);
        psess->backoff_time.pbackoff_thread = 0;
    }
    if (psess->pkeepalive_timer)
    {
        MPLS_TIMER_DEL(psess->pkeepalive_timer);
        psess->pkeepalive_timer = 0;
    }
    if (psess->phold_timer)
    {
        MPLS_TIMER_DEL(psess->phold_timer);
        psess->phold_timer = 0;
    }
    ldp_sess_del_pws(psess);
    

    psess->status = LDP_STATUS_NOEXIST;

    hios_hash_delete(&ldp_session_table, pitem);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer_lsrid %s local_ip %s successful.\n", ldp_ipv4_to_str(psess->key.peer_lsrid), ldp_ipv4_to_str2(psess->key.local_lsrid));
    
    XFREE(MTYPE_LDP_ENTRY, psess);
    XFREE(MTYPE_HASH_BACKET, pitem);

    MPLS_LDP_SESS_CNT_DECREASE(gldp);
    return ERRNO_SUCCESS;
}

/**
* @brief      <+清除所有LDP会话+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void ldp_session_clear(void)
{
    struct ldp_sess     *psess   = NULL;
    struct hash_bucket  *pbucket = NULL;
    struct hash_bucket  *pbucket_temp = NULL;
    int cursor = 0;

    HASH_BUCKET_LOOPW(pbucket, cursor, ldp_session_table)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket->next;
        psess = (struct ldp_sess *)pbucket_temp->data;
        if (psess)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "sssion peeer lsrid %s \n", ldp_ipv4_to_str(psess->key.peer_lsrid));
            if (LDP_STATUS_NOEXIST != psess->status)
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY), "ldp reject init message for session clean\n");
                ldp_send_notification(psess, LDP_NOTIF_SHUTDOWN, 0);

                ldp_sess_adjances_clear(psess, LDP_EVENT_SHUTDOWN);
            }
        }
    }
}

int ldp_sess_adjances_clear(struct ldp_sess     *psess, uint32_t event)
{
    struct listnode  *pnode         = NULL;
    struct listnode  *pnextnode     = NULL;
    struct ldp_adjance *padjance_lookup  = NULL;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "\n");

    for (ALL_LIST_ELEMENTS(&psess->adjacency_list, pnode, pnextnode, padjance_lookup))
    {
        if (padjance_lookup)
        {
            ldp_adjance_leave_sess(padjance_lookup, psess, event);
        }
    }
    return ERRNO_SUCCESS;
}

/**
* @brief      <+撤回发布的标签映射,清除学习到的FEC+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+只在LDP会话DOWN时调用+>
*/
void ldp_sess_label_maping_clear(struct ldp_sess *psess)
{
    struct ptree_node   *ptreenode  = NULL;
    struct ptree_node   *ptreenode2 = NULL;
    struct lsp_fec_t    *plsp_fec   = NULL;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "session peer lsrid %s now clear label mapings\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    /* Send withdraw msg to the upstream lsr for the egress lsp */
    if (psess->type & LDP_SESSION_LOCAL)
    {
        for (ptreenode2 = ptree_top(lsp_fec_tree); (NULL != ptreenode2);)
        {
            ptreenode = ptreenode2;
            ptreenode2 = ptree_next(ptreenode2);
            if(NULL == ptreenode->info)
            {
                continue;
            }
            plsp_fec = ptreenode->info;
            /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT */
            ldp_send_fec_withdraw(psess, plsp_fec);
        }
    }
    /* Clear the nhps that the lsr add to the ldp fecs */
    ldp_fec_clear(psess);
}

/**
* @brief      <+根据本对端LSRID查找一组会话+>
* @param[in ] <+local_lsrid:本端LSRID peer_lsrid:远端LSRID+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_sess *ldp_session_lookup(uint32_t local_lsrid, uint32_t peer_lsrid)
{
    struct hash_bucket *pitem = NULL;
    struct ldp_sess_key key;

    key.peer_lsrid = peer_lsrid;
    key.local_lsrid = local_lsrid;
    pitem = hios_hash_find(&ldp_session_table, &key);
    if (NULL == pitem)
    {
        MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, "with peer_lsrid %s local_ip %s failed.\n", ldp_ipv4_to_str(peer_lsrid), ldp_ipv4_to_str2(local_lsrid));
        return NULL;
    }
    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, "with peer_lsrid %s local_ip %s successful.\n", ldp_ipv4_to_str(peer_lsrid), ldp_ipv4_to_str2(local_lsrid));
    return (struct ldp_sess *)pitem->data;
}
/**
* @brief      <+LDP会话保活定时器，周期发送keepalive报文+>
* @param[in ] <+thread:伪线程结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_keepalive_timer(void *para)
{
    struct ldp_sess *psess = (struct ldp_sess *)(para);
    if (psess == NULL)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                   "session peer lsrid %s send keepalive and reset hold timer internal %d\n", ldp_ipv4_to_str(psess->key.peer_lsrid), psess->sess_hold / 3);
    ldp_send_keepalive(psess);
    psess->pkeepalive_timer = MPLS_TIMER_ADD(ldp_sess_keepalive_timer, (void *)psess, (psess->sess_hold / 3));

    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP会话HOLD定时器，超时则会话DOWN+>
* @param[in ] <+thread:伪线程结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+Session hold timer will set when tcp ok, reset when keepalive msg received +>
*/
int ldp_session_hold_timer(void *para)
{
    struct ldp_sess *psess = (struct ldp_sess *)(para);
    if (psess == NULL)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer lsrid %s hold timer expire\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    /* Disable_session and del the lsp added when the session up */
    psess->phold_timer = 0;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY), "ldp send keepalive time out\n");
    ldp_send_notification(psess, LDP_NOTIF_KEEPALIVE_TIMER_EXPIRED, 0);

    ldp_session_fsm(LDP_EVENT_KEEPALIVE_TIMEOUT, psess);
    
    return ERRNO_SUCCESS;
}


struct ldp_adjance *ldp_session_select_neighbor(struct ldp_sess *psess)
{
    struct ldp_adjance  *padjance   = NULL;
    struct ldp_adjance  *padjance_lookup   = NULL;
    struct listnode     *pnode     = NULL;
    struct listnode     *pnodenext = NULL;
    
    for (ALL_LIST_ELEMENTS(&psess->adjacency_list, pnode, pnodenext, padjance_lookup))
    {
        if (padjance_lookup)
        {
            if(NULL == padjance)
            {
                padjance = padjance_lookup;
                continue;
            }
            if(padjance->tcp_addr.priority > padjance_lookup->tcp_addr.priority)
            {
                padjance = padjance_lookup;
            }
        }
    }
    
    if(padjance)
    {
        padjance->tcp_addr.priority ++;
    }
    
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer lsrid %s \n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    return padjance;
}


void ldp_session_neighbor_priority_zero(struct ldp_sess *psess)
{
    struct ldp_adjance  *padjance   = NULL;
    struct listnode     *pnode     = NULL;
    struct listnode     *pnodenext = NULL;
    
    for (ALL_LIST_ELEMENTS(&psess->adjacency_list, pnode, pnodenext, padjance))
    {
        if (padjance)
        {
            padjance->tcp_addr.priority = 0;
        }
    }
    
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer lsrid %s \n", ldp_ipv4_to_str(psess->key.peer_lsrid));
}

int ldp_session_backoff_timer(void *para)
{
    struct ldp_sess *psess = (struct ldp_sess *)(para);
    if (psess == NULL)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "session peer lsrid %s backoff timer %d second expire\n", ldp_ipv4_to_str(psess->key.peer_lsrid), psess->backoff_time.backoff_run);

    if((LDP_TCP_LINK_UP != psess->tcp_status) ||(LDP_STATUS_UP != psess->status))
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_DOWN\n");
        LDP_SESS_RECONNECT_NUM_INCREASE(psess);

        ldp_tcp_port_relese(psess);
        
        psess->status  = LDP_STATUS_NOEXIST;

        ldp_tcp_port_alloc(psess);
        
        psess->backoff_time.backoff_run = (psess->backoff_time.backoff_run * 2 > psess->backoff_time.backoff_max ? psess->backoff_time.backoff_max : psess->backoff_time.backoff_run * 2);
        /* ldp session backoff timer reset */
        psess->backoff_time.pbackoff_thread = MPLS_TIMER_ADD(ldp_session_backoff_timer, (void *)psess, psess->backoff_time.backoff_run);

    }
    else
    {
        psess->backoff_time.backoff_run = psess->backoff_time.backoff_min;
    }
    return ERRNO_SUCCESS;
}

void ldp_sess_process_tcp_event(void *ptcp, uint8_t event)
{
    struct ldp_sess     *psess   = NULL;
    struct hash_bucket  *pbucket = NULL;
    struct hash_bucket  *pbucket_temp = NULL;
    struct tcp_session *ptcp_sess = (struct tcp_session *)ptcp;
    int cursor = 0;
    MPLS_LDP_ERROR("event %s peer ip %s sport %d dport %d\n", 
        (IPC_OPCODE_CONNECT == event) ? "IPC_OPCODE_CONNECT":"IPC_OPCODE_DISCONNECT", ldp_ipv4_to_str(ptcp_sess->dip), ptcp_sess->dport, ptcp_sess->sport);

    HASH_BUCKET_LOOPW(pbucket, cursor, ldp_session_table)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket->next;
        psess = (struct ldp_sess *)pbucket_temp->data;
        if (psess)
        {
            if(IPC_OPCODE_DISCONNECT == event)
            {
                if((ptcp_sess->sport == psess->dport) && (ptcp_sess->dport == psess->sport))
                {
                    if ((LDP_STATUS_UP == psess->status) && (LDP_TCP_LINK_UP == psess->tcp_status))
                    {
                        MPLS_LDP_ERROR("sssion peer lsrid %s sport %d dport %d \n", ldp_ipv4_to_str(psess->key.peer_lsrid), psess->sport, psess->dport);
                        ldp_fsm_goto_initialization(psess);
                        snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for tcp down\n" );
                        psess->tcp_status = LDP_TCP_LINK_DOWN;
                        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_DOWN\n");
                    }
                    else
                    {
                        psess->tcp_status = LDP_TCP_LINK_DOWN;
                        psess->status  = LDP_STATUS_NOEXIST;
                    }
                }
            }
            else if((IPC_OPCODE_CONNECT == event) &&(LDP_TCP_LINK_DOWN == psess->tcp_status))
            {
                if(LDP_STATUS_NOEXIST == psess->status)
                {
                    if((ptcp_sess->sport == psess->dport) && (ptcp_sess->dport == psess->sport))
                    {
                        MPLS_LDP_ERROR("sssion peer lsrid %s sport %d dport %d \n", ldp_ipv4_to_str(psess->key.peer_lsrid), psess->sport, psess->dport);
                        snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for tcp established\n" );
                        psess->tcp_status = LDP_TCP_LINK_UP;
                        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_UP\n");
                        psess->status = LDP_STATUS_INIT;
                        ldp_session_fsm(LDP_EVENT_TCP_FINISHED, psess);
                    }
                }
            }
            else 
            {
                MPLS_LDP_ERROR("ldp reveive unknown tcp session message\n");
            }
        }
    }
}


