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
#include <mpls/bfd/bfd.h>
#include <mpls/bfd/bfd_session.h>
#include "ldp.h"
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_fsm.h"
#include "ldp_address.h"
#include "ldp_pw.h"

extern int bfd_for_ldp_bind_unbind(enum BFD_SUBTYPE type, void *pdata);

/**
* @brief      <+LDP会话开启或关闭BFD探测+>
* @param[in ] <+type：BFD使能或去使能标记+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+仅命令行调用+>
*/
void ldp_sesss_bfd_unbind(uint32_t type)
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
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_MAIN, "sssion peeer lsrid %s \n", ldp_ipv4_to_str(psess->key.peer_lsrid));
            if (LDP_STATUS_UP == psess->status)
            {
                bfd_for_ldp_bind_unbind(type, psess);
            }
        }
    }
}

/**
* @brief      <+LDP 会话状态迁移到OPERATIONAL+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fsm_goto_operational(struct ldp_sess *psess)
{
    struct ptree_node   *pnode  = NULL;
    struct lsp_fec_t    *pfec   = NULL;

    if (NULL == psess)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    ldp_session_neighbor_priority_zero(psess);

    /* cancel the backoff timer, if pending */
    psess->backoff_time.backoff_run = psess->backoff_time.backoff_min;
    if(psess->backoff_time.pbackoff_thread)
    {
        MPLS_TIMER_DEL(psess->backoff_time.pbackoff_thread);
        psess->backoff_time.pbackoff_thread = 0;
    }
    /* ldp session hold timer fist set here */
    if (0 == psess->phold_timer)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                       "LDP_EVENT_TCP_LINK__FINISHED, now set the session hold timer %d second.\n", psess->sess_hold);
        psess->phold_timer = MPLS_TIMER_ADD(ldp_session_hold_timer, (void *)psess, psess->sess_hold);
    }

    
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "peer lsrid %s start negotiate\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    /* add bfd for ldp session */
    if (MPLS_LDP_BFD_ENABLE == gldp.bfd_enable)
    {
        bfd_for_ldp_bind_unbind(BFD_SUBTYPE_BIND_LDP, psess);
    }

    if (0 == psess->pkeepalive_timer)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp peer lsrid %s keepalive timer starting.\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
        psess->pkeepalive_timer = MPLS_TIMER_ADD(ldp_sess_keepalive_timer, (void *)psess, psess->sess_hold / 3);
    }

    ldp_send_addresses_maping(psess);

    /* ADVERTISE_PW send pw inlable */
    ldp_sess_pws_negotiate_start(psess);

    if (gldp.policy != LABEL_POLICY_NONE)
    {
        /* local ldp send the label maping to the peer */
        PTREE_LOOP(lsp_fec_tree, pfec, pnode)
        {
            /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT */
            ldp_send_fec_mapping(psess, pfec);
        }
    }
    snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session goto operational\n" );
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP 会话状态迁移到NON-EXIST+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fsm_goto_initialization(struct ldp_sess *psess)
{
    if (NULL == psess)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "peer lsrid %s finish the negotiate\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

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

    /* ADVERTISE_PW  DOWN */
    if (LDP_STATUS_UP == psess->status)
    {
        if ((MPLS_LDP_BFD_ENABLE == gldp.bfd_enable) && (psess->bfd_sess))
        {
            bfd_for_ldp_bind_unbind(BFD_SUBTYPE_UNBIND_LDP, psess);
        }

        ldp_sess_pws_negotiate_end(psess);

        ldp_sess_label_maping_clear(psess);

        ldp_peer_ifaddrs_clear(psess);
    }
    
    psess->status = LDP_STATUS_NOEXIST;

    /* The active start the backoff timer to reconect regularly */
    if((LDP_ROLE_ACTIVE == psess->role) && (psess->adjacency_list.count))
    {
        if(0 == psess->backoff_time.pbackoff_thread)
        {
            psess->backoff_time.pbackoff_thread = MPLS_TIMER_ADD(ldp_session_backoff_timer, (void *)psess, psess->backoff_time.backoff_run);
        }
    }

    ldp_tcp_port_relese(psess);
    
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP 会话状态机+>
* @param[in ] <+event:状态机事件 psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_session_fsm(enum LDP_EVENT event, struct ldp_sess *psess)
{
    if (NULL == psess)
    {
        return ERRNO_FAIL;
    }

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                   "local_lsrid: %s peer_lsrid: %s\n", ldp_ipv4_to_str(psess->key.local_lsrid), ldp_ipv4_to_str2(psess->key.peer_lsrid));
    switch (event)
    {
    case LDP_EVENT_RECV_DISCOVER:
    {
        /* if a discover packet is received ,it means the ldp session has a adjance at least ,now open a tcp session */
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp recv LDP_EVENT_RECV_DISCOVER\n");

        if ((LDP_STATUS_NOEXIST == psess->status))
        {
            /* nonexistent->init: if sess active , send the init and become opensend. if session passive, waiting for the init msg */
            if (LDP_ROLE_ACTIVE == psess->role)
            {
                if(0 == psess->backoff_time.pbackoff_thread)
                {
                    ldp_tcp_port_alloc(psess);//only the active session can alloc the tcp port 
                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "Active ldp reset ldp_session_backoff_timer %d second.\n", psess->backoff_time.backoff_run);
                    psess->backoff_time.pbackoff_thread = MPLS_TIMER_ADD(ldp_session_backoff_timer, (void *)psess, psess->backoff_time.backoff_run);
                }
            }
            else
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "Passive ldp waiting for the init packet\n");
            }
        }
        break;
    }

    case LDP_EVENT_TCP_FINISHED:
    {
        if (LDP_ROLE_ACTIVE == psess->role)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "Active ldp send init message and become LDP_STATUS_OPENSEND.\n");
            ldp_send_init(psess);
            psess->status = LDP_STATUS_OPENSEND;
        }
        else
        {

        }
        
        break;
    }
    case LDP_EVENT_RECV_INITAL:
    {
        if (LDP_ROLE_ACTIVE == psess->role)
        {
            if (LDP_STATUS_OPENSEND == psess->status)
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                               "Active ldp receive init, now send keepalive message and become LDP_STATUS_OPENREC.\n");
                ldp_send_keepalive(psess);
                psess->status = LDP_STATUS_OPENREC;
            }
        }
        else
        {
            /* if passive of ldp received a init packet ,it means tcp session established */
            psess->tcp_status = LDP_TCP_LINK_UP;
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_UP\n");
            psess->status = LDP_STATUS_INIT;
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                              "Passive ldp receive init, now send init and keepalive message and become LDP_STATUS_OPENREC.\n");
           ldp_send_init(psess);
           /* passive send init packet and becomes to openrec status */
           psess->status = LDP_STATUS_OPENREC;
           ldp_send_keepalive(psess);

        }
        break;
    }
    case LDP_EVENT_RECV_KEEPALIVE:
    {
        if (LDP_STATUS_OPENREC == psess->status)
        {
            ldp_send_keepalive(psess);
            psess->status = LDP_STATUS_UP;

            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                           "ldp  LDP_STATUS_OPENREC receive keepalive now become LDP_STATUS_UP.\n");

            ldp_fsm_goto_operational(psess);
        }
        else if (LDP_STATUS_UP == psess->status)
        {
            if (psess->phold_timer)
            {
                MPLS_TIMER_DEL(psess->phold_timer);
                psess->phold_timer = 0;
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                               "LDP_STATUS_UP receive ldp keepalive, now reset the keepalive hold timer %d second.\n", psess->sess_hold);
            }
            else
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM,
                               "LDP_STATUS_UP receive ldp keepalive, checking hold timer NULL, now set the keepalive hold timer %d second.\n", psess->sess_hold);
            }
            psess->phold_timer = MPLS_TIMER_ADD(ldp_session_hold_timer, (void *)psess, psess->sess_hold);
        }
        break;
    }

    case LDP_EVENT_NEIGHBOR_TIMEOUT:
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp fsm LDP_EVENT_NEIGHBOR__TIMEOUT event\n");
        snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for adjance timeout\n" );
        ldp_fsm_goto_initialization(psess);
        ldp_session_delete(psess);
        break;
    }

    case LDP_EVENT_KEEPALIVE_TIMEOUT:
    {
        if((LDP_ROLE_ACTIVE == psess->role) && (psess->status != LDP_STATUS_UP))
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp fsm recevie inactive LDP_EVENT_KEEPALIVE__TIMEOUT event\n");
        }
        else
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp fsm receive active LDP_EVENT_KEEPALIVE__TIMEOUT event\n");
            snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for keepalive timeout\n" );
            ldp_fsm_goto_initialization(psess);
        }
        break;
    }

    case LDP_EVENT_SHUTDOWN:
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp fsm LDP_EVENT__SHUTDOWN event\n");
        snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for shutdown msg\n" );
        ldp_fsm_goto_initialization(psess);
        ldp_session_delete(psess);
        break;
    }

    case LDP_EVENT_REC_PKT_UNTIMELY:
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp fsm LDP_EVENT_REC_PKT_UNTIMELY_ event\n");
        snprintf(psess->reason, LDP_SESS_REASON_MAX_LEN, " ldp session down for receiving untimely message\n" );
        ldp_fsm_goto_initialization(psess);
        break;
    }

    default:
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "unknown ldp event here\n");
        break;
    }
    }

    return ERRNO_SUCCESS;
}

