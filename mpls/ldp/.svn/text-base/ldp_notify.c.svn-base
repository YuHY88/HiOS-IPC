#include <string.h>

#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/errcode.h>
#include <ftm/pkt_tcp.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_ip.h>
#include <mpls/lsp_fec.h>
#include <mpls/mpls_if.h>
#include <mpls/mpls.h>
#include <mpls_main.h>

#include "ldp.h"
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_fsm.h"
#include "ldp_notify.h"


/**
* @brief      <+å‘é€LDPé€šçŸ¥æ¶ˆæ¯+>
* @param[in ] <+psess:LDPä¼šè¯ç»“æž„ err_code:é”™è¯¯ç  msgid:æ¶ˆæ¯ID+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_notification(struct ldp_sess *psess, uint32_t err_code, uint32_t msgid)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu *pheader = NULL;
    struct ldpmsg_notify *pnotify = NULL;
    struct notifytlv_status *pstatus = NULL;
    struct notifytlv_pwstatus *ppwstatus = NULL;
    char *pend = NULL;
    uint32_t error = 0;
    uint32_t forward = 0;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, " err_code %d.\n", err_code);
    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);
    ppwstatus = (struct notifytlv_pwstatus *)pend;

    /* Ìî³äldp status tlv */
    pstatus = (struct notifytlv_status *)((char *)ppwstatus - sizeof(struct notifytlv_status));
    if (err_code == LDP_NOTIF_SUCCESS
            || err_code == LDP_NOTIF_UNKNOWN_MESG
            || err_code == LDP_NOTIF_UNKNOWN_TVL
            || err_code == LDP_NOTIF_LOOP_DETECTED
            || err_code == LDP_NOTIF_UNKNOWN_FEC
            || err_code == LDP_NOTIF_NO_ROUTE
            || err_code == LDP_NOTIF_NO_LABEL_RESOURCES_AVAILABLE
            || err_code == LDP_NOTIF_LABEL_RESOURCES_AVAILABLE
            || err_code == LDP_NOTIF_LABEL_ABORT
            || err_code == LDP_NOTIF_MISSING_MSG_PARAMS
            || err_code == LDP_NOTIF_UNSUPORTED_ADDR_FAMILY
            || err_code == PW_STATUS)
    {
        error = 0;
    }
    else
    {
        error = 0x1u << 31;
    }

    /* check to set the F bit */
    if (err_code == LDP_NOTIF_LOOP_DETECTED
            || err_code == LDP_NOTIF_UNKNOWN_FEC
            || err_code == LDP_NOTIF_NO_ROUTE)
    {
        forward = 0x1u << 30;
    }

    /* ÉèÖÃ notify Tlv value */
    pstatus->flag |= error;
    pstatus->flag |= forward;
    pstatus->flag |= err_code;
    pstatus->flag = htonl(pstatus->flag);
    pstatus->msgid = htonl(msgid);  //ÊÕµ½±¨ÎÄµÄ msgid
    pstatus->msgtype = htons(0);    //Ä¬ÈÏÖµÎª0

    ldp_set_tlvbase(&(pstatus->basetlv), LDP_TLV_STATUS, (sizeof(struct notifytlv_status) - sizeof(struct ldptlv_base)));
    pnotify = (struct ldpmsg_notify *)((char *)pstatus - sizeof(struct ldpmsg_base));

    ldp_set_msgbase(&(pnotify->basemsg), LDP_MSG_NOTIFY, (pend - (char *)pstatus) + LDP_MSGID_LEN);

    pheader = (struct ldpmsg_pdu *)((char *)pnotify - LDP_PDU_HEADER_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, (pend - (char *)pnotify) + LDP_IDENTIFY_LEN);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    return ldp_send_tcp_pkt(psess, pheader, pend - (char *)pheader);
}

/**
* @brief      <+å¤„ç†LDPé€šçŸ¥æ¶ˆæ¯+>
* @param[in ] <+pbasemsg:LDPåŸºç¡€æ¶ˆæ¯å¤´ psess:LDPä¼šè¯ç»“æž„ pipcb:æŠ¥æ–‡æŽ§åˆ¶ç»“æž„+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_notification(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct ldpmsg_notify *pnotify = NULL;
    pnotify = (struct ldpmsg_notify  *)pbasemsg;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "with packet: %s\n", pkt_dump(pnotify, sizeof(struct ldpmsg_notify)));

    if (psess)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "pnotify->flag & 0x3ffff = %x\n", pnotify->status.flag & 0x3ffff);
        /* ¸ù¾ÝÍ¨ÖªÏûÏ¢µÄÄÚÈÝ×ö¶ÔÓ¦´¦Àí */
        switch (ntohl(pnotify->status.flag) & 0x3ffff)
        {
        case LDP_NOTIF_BAD_LDP_ID:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_BAD_LDP_ID.\n");
            break;
        case LDP_NOTIF_BAD_PROTO_VERSION:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_BAD_PROTO_VERSION.\n");
            break;
        case LDP_NOTIF_BAD_PDU_LEN:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_BAD_PDU_LEN.\n");
            break;
        case LDP_NOTIF_UNKNOWN_MESG:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_UNKNOWN_MESG.\n");
            break;
        case LDP_NOTIF_BAD_MESG_LEN:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_BAD_MESG_LEN.\n");
            break;
        case LDP_NOTIF_UNKNOWN_TVL:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_UNKNOWN_TVL.\n");
            break;
        case LDP_NOTIF_BAD_TLV_LEN:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_BAD_TLV_LEN.\n");
            break;
        case LDP_NOTIF_MALFORMED_TLV:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_MALFORMED_TLV.\n");
            break;
        case LDP_NOTIF_HOLD_TIMER_EXPIRED:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_NOTIFY | MPLS_LDP_DEBUG_FSM), "LDP_NOTIF_HOLD_TIMER_EXPIRED.\n");

            ldp_send_notification(psess, LDP_NOTIF_HOLD_TIMER_EXPIRED, 0);

            ldp_session_fsm(LDP_EVENT_KEEPALIVE_TIMEOUT, psess);
            
            break;
        case LDP_NOTIF_SHUTDOWN:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_NOTIFY | MPLS_LDP_DEBUG_FSM), "LDP_NOTIF_SHUTDOWN_.\n");
            /* clear the ldp adjances in the psess */
        
            ldp_send_notification(psess, LDP_NOTIF_SHUTDOWN, 0);

            ldp_sess_adjances_clear(psess, LDP_EVENT_SHUTDOWN);//the session will delete after the adjance is clear
            break;
        case LDP_NOTIF_LOOP_DETECTED:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_LOOP_DETECTED.\n");
            break;
        case LDP_NOTIF_UNKNOWN_FEC:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_UNKNOWN_FEC.\n");
            break;
        case LDP_NOTIF_NO_ROUTE:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_NO_ROUTE.\n");
            break;
        case LDP_NOTIF_NO_LABEL_RESOURCES_AVAILABLE:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_NO_LABEL_RESOURCES_AVAILABLE.\n");
            break;
        case LDP_NOTIF_LABEL_RESOURCES_AVAILABLE:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_LABEL_RESOURCES_AVAILABLE.\n");
            break;
        case LDP_NOTIF_SESSION_REJECTED_NO_HELLO:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_NOTIFY | MPLS_LDP_DEBUG_FSM), "LDP_NOTIF_SESSION_REJECTED_NO_HELLO.\n");
            ldp_session_fsm(LDP_EVENT_REC_PKT_UNTIMELY, psess);
            break;
        case LDP_NOTIF_SESSION_REJECTED_PARAMETERS_ADVERTISEMENT_MODE:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_SESSION_REJECTED_PARAMETERS_ADVERTISEMENT_MODE.\n");
            break;
        case LDP_NOTIF_SESSION_REJECTED_PARAMETERS_MAX_PDU_LEN:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_SESSION_REJECTED_PARAMETERS_MAX_PDU_LEN.\n");
            break;
        case LDP_NOTIF_SESSION_REJECTED_PARAMETERS_LABEL_RANGE:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_SESSION_REJECTED_PARAMETERS_LABEL_RANGE.\n");
            break;
        case LDP_NOTIF_KEEPALIVE_TIMER_EXPIRED:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_NOTIFY | MPLS_LDP_DEBUG_FSM), "LDP_NOTIF_KEEPALIVE_TIMER_EXPIRED.\n");
            ldp_session_fsm(LDP_EVENT_KEEPALIVE_TIMEOUT, psess);
            break;
        case LDP_NOTIF_LABEL_ABORT:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_LABEL_ABORT.\n");
            break;
        case LDP_NOTIF_MISSING_MSG_PARAMS:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_MISSING_MSG_PARAMS.\n");
            break;
        case LDP_NOTIF_UNSUPORTED_ADDR_FAMILY:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_UNSUPORTED_ADDR_FAMILY.\n");
            break;
        case LDP_NOTIF_SESSION_REJECTED_BAD_KEEPALIVE_TIME:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_SESSION_REJECTED_BAD_KEEPALIVE_TIME.\n");
            break;
        case LDP_NOTIF_INTERNAL_ERROR:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "LDP_NOTIF_INTERNAL_ERROR.\n");
            break;

        default:
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_NOTIFY, "unknown error code\n");
            break;
        }
    }
    return ERRNO_SUCCESS;
}

