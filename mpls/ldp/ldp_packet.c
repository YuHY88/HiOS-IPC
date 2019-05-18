/*
*   ldp �շ�����״̬������
*/
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
#include "ldp_notify.h"
#include "ldp_fsm.h"
#include "ldp_pw.h"
#include "ldp_address.h"
#include "ldp_fec.h"
extern int pkt_close_tcp(int module_id, struct ip_proto *ptcp);

/***********************ldp packet send**************************************/

/**
* @brief      <+none+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void ldp_set_msgbase(struct ldpmsg_base *pbasemsg, uint32_t pktype, uint32_t len)
{
    static uint32_t message_id  = 0;
    pbasemsg->ubit = 0;
    pbasemsg->msg_type = htons(pktype);//hello msg
    pbasemsg->msgid = htonl(message_id++);
    pbasemsg->msg_len = htons(len);
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
void ldp_set_header(struct ldpmsg_pdu *pheader, uint32_t local_lsrid, uint32_t len)
{
    pheader->version = htons(LDP_VERSION);
    pheader->pdu_len = htons(len);
    pheader->lsrid = htonl(local_lsrid);
    pheader->label_space = htons(LDP_LABEL_GLOBAL);
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
void ldp_set_tlvbase(struct ldptlv_base *pbasetlv, uint32_t tlvtype, uint32_t len)
{
    pbasetlv->flag = 0;
    pbasetlv->tlv_type = htons(tlvtype);
    pbasetlv->tlv_len = htons(len);
}

/**
* @brief      <+LDP send a UDP packet+>
* @param[in ] <+local_ip:源地址 peer_ip:目的地址 ifindex:发包接口索引 data:载荷指针 data_len:载荷长度+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_udp_pkt(uint32_t local_ip, uint32_t peer_ip,
                     uint32_t ifindex, void *data, int data_len)
{
    int ret = ERRNO_FAIL;
    union pkt_control pctr;

    memset(&pctr, 0, sizeof(union pkt_control));
    pctr.ipcb.protocol = IP_P_UDP;
    pctr.ipcb.ifindex = ifindex;
    pctr.ipcb.sip = local_ip;
    pctr.ipcb.dip = peer_ip;
    pctr.ipcb.dport = UDP_PORT_LDP;
    pctr.ipcb.sport = UDP_PORT_LDP;
    pctr.ipcb.tos = 4;

    if (peer_ip == LDP_HELLO_ADDR) //local hello
        pctr.ipcb.ttl = 1;
    else                           //remote hello
        pctr.ipcb.ttl = 255;

    pctr.ipcb.is_changed = 1;

    ret = pkt_send(PKT_TYPE_UDP, &pctr, data, data_len);
    if (ret)
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "failed\n");
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ldp send packet %s.\n", pkt_dump(data, data_len));

    MPLS_LDP_PKT_SEND(gldp);
    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP send a TCP packet +>
* @param[in ] <+psess:LDP会话结构 data:载荷 data_len:载荷长度+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_tcp_pkt(struct ldp_sess *psess, void *data, int data_len)
{
    int ret = ERRNO_FAIL;
    union pkt_control pctr;

    memset(&pctr, 0, sizeof(union pkt_control));
    pctr.ipcb.protocol = IP_P_TCP;
    pctr.ipcb.sip = psess->tcp_addr.local_addr;
    pctr.ipcb.dip = psess->tcp_addr.peer_addr;
    pctr.ipcb.dport = psess->dport;
    pctr.ipcb.sport = psess->sport;
    pctr.ipcb.tos = 4;
    pctr.ipcb.ttl = 255;
    pctr.ipcb.is_changed = 1;

    ret = pkt_send(PKT_TYPE_TCP, &pctr, data, data_len);
    if (ret)
    {
        MPLS_LDP_ERROR("LDP SEND \n");
        return ERRNO_FAIL;
    }
    MPLS_LDP_PKT_SEND(gldp);

    return ERRNO_SUCCESS;
}

/**
* @brief      <+local ldp send udp muticast link hello packet ,  remote ldp send udp unicast hello packet+>
* @param[in ] <+local_ip:源IP peer_ip:目的IP ifindex:发包接口索引 ldptype:LDP类型+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_hello(uint32_t local_ip, uint32_t peer_ip, uint32_t ifindex, uint32_t ldptype)
{
    struct ldp_pkt pkt;
    struct ldpmsg_hello *phello = NULL;
    uint32_t len = 0;
    if (0 == LDP_MPLS_LSRID)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, " peer_ip %s.\n", ldp_ipv4_to_str(peer_ip));

    memset(&pkt, 0, sizeof(struct ldp_pkt));

    /* set the common hello parameters */
    phello = &(pkt.msg.hello);
    phello->chp.neigh_hold = htons(LDP_NEIGH_EXPIRE_TIME); /* ����Ϊ��λ */
    if (ldptype & LDP_SESSION_REMOT)
    {
        phello->chp.flag = htons(1 << 15);
    }

    phello->traddr.addr = htonl(LDP_MPLS_LSRID);

    ldp_set_tlvbase(&(phello->traddr.basetlv), LDP_TLV_TRADDR, LDP_TLV_TRADDR_VAL_LEN);

    /* caculate the length in the tlv is the tlv value len */
    ldp_set_tlvbase(&(phello->chp.basetlv), LDP_TLV_HELLO, LDP_TLV_HELLO_VAL_LEN);

    /* caculate the length in the base msg : tlv value len + tlv base len + msgid len */
    len += (LDP_BASETLV_LEN * 2 + LDP_TLV_TRADDR_VAL_LEN + LDP_TLV_HELLO_VAL_LEN + LDP_MSGID_LEN);
    ldp_set_msgbase(&(phello->basemsg), LDP_MSG_HELLO, len);

    /* caculate the length in the pdu header : excluding the Version and PDU Length fields */
    len += (LDP_IDENTIFY_LEN + LDP_BASEMSG_LEN);
    ldp_set_header(&(pkt.header), LDP_MPLS_LSRID, len);
    len += LDP_HEADER_LEN;
    return ldp_send_udp_pkt(local_ip, peer_ip, ifindex, &pkt, len);
}


/**
* @brief      <+LDP send a session init packet+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_init(struct ldp_sess *psess)
{
    struct ldp_pkt pkt;
    struct ldpmsg_init *pinit = NULL;
    uint32_t len = LDP_TLV_INIT_VAL_LEN;
    uint16_t adertise = (LABEL_ADVERTISE_DOD == gldp.advertise) ? 0x1u << 15 : 0x0;

    /* set Iint Tlv */
    pinit = &(pkt.msg.init);
    pinit->chp.version = htons(LDP_VERSION);
    pinit->chp.session_hold = htons(LDP_SESSION_EXPIRE_TIME);
    pinit->chp.flag = htons(adertise);
    pinit->chp.pdu_len = htons(LDP_MAX_PDU_LEN);
    pinit->chp.peer_lsrid = htonl(psess->key.peer_lsrid);
    pinit->chp.peer_labelspace = 0;

    ldp_set_tlvbase(&(pinit->chp.basetlv), LDP_TLV_INIT, len);

    len += (LDP_BASETLV_LEN + LDP_MSGID_LEN);
    ldp_set_msgbase(&(pinit->basemsg), LDP_MSG_INIT, len);

    len += (LDP_IDENTIFY_LEN + LDP_BASEMSG_LEN);
    ldp_set_header(&(pkt.header), psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_FSM, " peer_ip %s.\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    return ldp_send_tcp_pkt(psess, &pkt, len);
}

/**
* @brief      <+LDP send a session keepalive packet+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_keepalive(struct ldp_sess *psess)
{
    struct ldp_pkt pkt;
    uint32_t len = LDP_MSGID_LEN;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "peerlsrid %s packet\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    ldp_set_msgbase(&(pkt.msg.keepalive.basemsg), LDP_MSG_KEEPALIVE, len);

    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(&(pkt.header), psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_PKT_KA_SEND(psess);
    return ldp_send_tcp_pkt(psess, &pkt, len);
}


/***************************LDP PACKET RECEIVE*****************************************/


/**
* @brief      <+LDP receive a hello packet+>
* @param[in ] <+pbasemsg:LDP基础消息头 peer_lsrid:对端LSRID pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+Receipt of an LDP Hello identifies a "Hello adjacency" with a potential LDP peer reachable+>
*/
int ldp_recv_hello(struct ldpmsg_base  *pbasemsg, uint32_t peer_lsrid, struct ip_control  *pipcb)
{
    struct mpls_if  *pif = NULL;
    struct ifm_l3   pl3if;
    struct ldpmsg_hello *phello = NULL;
    struct hellotlv_chp *pchp = NULL;
    struct hellotlv_traddr *ptraddr = NULL;
    struct ldp_adjance *padjance = NULL;
    struct ldp_sess *psess = NULL;
    uint32_t type = LDP_SESSION_LOCAL;
    uint32_t ifindex = pipcb->ifindex;
    uint32_t local_lsrid = LDP_MPLS_LSRID;
    uint32_t sip = LDP_MPLS_LSRID;
    uint32_t dip = peer_lsrid;
    uint16_t hello_hold = 0;
    uint16_t hello_send = 0;
    struct ldp_pwconf *ppwconf = NULL;
    uint32_t len_tmp = 0;
    uint32_t use_flag = 0;

    phello = (struct ldpmsg_hello *)pbasemsg;

    uint16_t msg_len = ntohs(phello->basemsg.msg_len);
    len_tmp = msg_len - 4;/* 减去msgid长度 */

    if(len_tmp)
    {
        /* 如果消息长度大于 0， 说明存在TLV */
        if(LDP_TLV_HELLO == ntohs(phello->chp.basetlv.tlv_type))
        {
            pchp = &phello->chp;
        }
        else
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " return for common hello tlv mising.\n");
            return ERRNO_FAIL;
        }

        len_tmp = len_tmp - sizeof(struct hellotlv_chp);/* 减去通用参数TLV长度*/

        if(len_tmp)
        {
            /* 如果消息长度大于 0， 说明存在TLV */
            if(LDP_TLV_TRADDR == ntohs(phello->traddr.basetlv.tlv_type))
            {
                ptraddr = &phello->traddr;
            }
            else
            {
                MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " return for unknown tlv.\n");
                return ERRNO_FAIL;
            }
        }
        else
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " transport address tlv not exist.\n");
        }
    }
    

    /* type remote or local */
    if (ntohs(pchp->flag) >> 15)
    {
        if(LDP_MPLS_LSRID != pipcb->dip)
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp droping target hello for local lsrid changed.\n");
            return ERRNO_FAIL;
        }
        MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER,"recieve remote hello packet\n");
        type = LDP_SESSION_REMOT;
        ifindex = 0;
    }
    else
    {
        MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER,"recieve local hello packet\n");
        pif = mpls_if_lookup(ifindex);
        if (!pif || (pif->ldp_conf.ldp_enable == DISABLE))
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " return for ldp disabled on if %x.\n", ifindex);
            return ERRNO_FAIL;
        }

        if ((ifm_get_l3if(ifindex, MODULE_ID_MPLS, &pl3if) < 0) || (pl3if.ipv4[0].addr == 0))
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " ldp droping link hello packet for geting primary address on ifindex %d failed\n", ifindex);
            return ERRNO_NOT_FOUND;
        }
        sip = pl3if.ipv4[0].addr;
        dip = LDP_HELLO_ADDR;
    }

    padjance = ldp_adjance_lookup(peer_lsrid, ifindex, pipcb->sip, type);
    if (padjance == NULL)
    {
        if (LDP_SESSION_REMOT == type)
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp droping target hello for remote ldp disabled.\n");
            return ERRNO_NOT_FOUND;
        }
        /* a new ldp adjance */
        MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp receive hello but find neighbor on the ifindex %x failed.\n", ifindex);
        padjance = ldp_adjance_create(local_lsrid, peer_lsrid, pipcb->sip, ifindex, type);
        if (NULL == padjance)
        {
            return ERRNO_MALLOC;
        }
    }

    
    hello_hold = ntohs(pchp->neigh_hold);
    hello_send = hello_hold/3;
    /* check if the hello hold illegal */
    if (hello_hold != padjance->neighbor_hold)
    {
        padjance->neighbor_hold = hello_hold;
        /* compute the new hello send time */
        hello_send = (gldp.hello_internal < hello_send) ? gldp.hello_internal : hello_send;
        if (LDP_SESSION_REMOT == padjance->type)
        {
            padjance->hello_send = hello_send;
        }
        else
        {
            /* 更新接口hello间隔 */
            pif->ldp_conf.hello_send_effect = ldp_adjance_hello_internal_recalculate(&pif->ldp_conf.neighbor_list);
        }
    }

    if (padjance->phold_timer)
    {
        /* reset the hello hold timer */
        MPLS_TIMER_DEL(padjance->phold_timer);
        padjance->phold_timer = 0;
    }
    padjance->rcv_cnt++;
    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "Ldp reset the hello hold timer %d second.\n", padjance->neighbor_hold);
    padjance->phold_timer = MPLS_TIMER_ADD(ldp_adjance_hold_timer, (void *)padjance, padjance->neighbor_hold + 1);

    psess = ldp_session_lookup(local_lsrid, peer_lsrid);
    if (psess == NULL)
    {
        /* a new ldp session */
        psess = ldp_session_create(peer_lsrid, local_lsrid);
        if (NULL == psess)
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp droping hello for ldp session creating failed\n");
            return ERRNO_MALLOC;
        }
        /* check ldp_pw_conf_list ,if pw is config for the ldp session */
        ppwconf = ldp_pw_conf_node_lookup(peer_lsrid);
        if (ppwconf)
        {
            /* Add the pws in the ppwconf->pwlist to the ldp sess */
            ldp_sess_add_pws(psess, ppwconf);
        }
    }

    /* Update the transport address on the link when a hello packet is received */
    padjance->tcp_addr.local_addr = local_lsrid;
    if((NULL != padjance->psess) && (padjance->tcp_addr.peer_addr == padjance->psess->tcp_addr.peer_addr))
    {
        use_flag = 1;
    }
    /* if tcp tlv is null  the ip on the link will be used for tcp address */
    padjance->tcp_addr.peer_addr = (ptraddr ? ntohl(ptraddr->addr) : pipcb->sip);

    if(use_flag)
    {
        memcpy(&padjance->psess->tcp_addr, &padjance->tcp_addr, sizeof(struct ldp_taddr));
        psess->role = (psess->tcp_addr.peer_addr < psess->tcp_addr.local_addr) ? LDP_ROLE_ACTIVE : LDP_ROLE_PASSIVE;
    }
    /* if the adjance has not been join a session ,do it */
    if (NULL == padjance->psess)
    {
        ldp_adjance_join_sess(padjance, psess);
        if(psess->role == LDP_ROLE_ACTIVE)
        {
            ldp_send_hello(sip, dip, ifindex, type);
        }
    }

    if (psess->status == LDP_STATUS_NOEXIST)
    {
        ldp_session_fsm(LDP_EVENT_RECV_DISCOVER, psess);
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP receive session init packet+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_init(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct ldpmsg_init  *pinit = (struct ldpmsg_init *)pbasemsg;
    //uint32_t msgid = 0;
    uint16_t adertise = (LABEL_ADVERTISE_DOD == gldp.advertise) ? 0x1u << 15 : 0x0;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "recieve init msg\n");

    //msgid = ntohl(pbasemsg->msgid);

    if (LDP_ROLE_PASSIVE == psess->role)
    {
        psess->dport = pipcb->sport;//��¼�Զ�TCPԴ�˿���ΪĿ�Ķ˿�
        psess->sport = pipcb->dport;
        psess->tcp_addr.local_addr = LDP_MPLS_LSRID;
        psess->tcp_addr.peer_addr = pipcb->sip;
        
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "ldp passive learn sport %u dport %u peer ip %x\n", pipcb->sport, pipcb->dport, pipcb->sip);
    }

    psess->sess_hold = MIN(ntohs(pinit->chp.session_hold), LDP_SESSION_EXPIRE_TIME);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "receive init packet with session hold %d "
                   "local hold time %d now is %d after learned from the packet\n",
                   ntohs(pinit->chp.session_hold),  LDP_SESSION_EXPIRE_TIME, psess->sess_hold);

    if (0 == psess->adjacency_list.count)
    {
        ldp_send_notification(psess, LDP_NOTIF_SESSION_REJECTED_NO_HELLO, 0);
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_DISCOVER | MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY), "ldp reject init for no hello\n");
        ldp_session_fsm(LDP_EVENT_REC_PKT_UNTIMELY, psess);
        goto out;
    }

    if (adertise != ntohs(pinit->chp.flag))
    {
        ldp_send_notification(psess, LDP_NOTIF_SESSION_REJECTED_PARAMETERS_ADVERTISEMENT_MODE, 0);
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY),
                       "ldp reject init message for different advertise mode\n");
        goto out;
    }

    /* passive will keep noexist until a init packet is received */
    if ((LDP_STATUS_NOEXIST == psess->status) || (LDP_STATUS_OPENSEND == psess->status))
    {
        ldp_session_fsm(LDP_EVENT_RECV_INITAL, psess);
    }
    else
    {
        ldp_send_notification(psess, LDP_NOTIF_SHUTDOWN, 0);
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_DISCOVER | MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY), "ldp reject init for no hello\n");
        ldp_session_fsm(LDP_EVENT_REC_PKT_UNTIMELY, psess);
    }

out:
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "finished\n");
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP receive a session keepalive packet+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_keepalive(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_FSM, "message\n");
    if (psess)
    {
        if ((LDP_STATUS_OPENREC == psess->status) || (LDP_STATUS_UP == psess->status))
        {
            ldp_session_fsm(LDP_EVENT_RECV_KEEPALIVE, psess);
        }
        MPLS_LDP_PKT_KA_RECV(psess);
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP receive a request, PW request or FEC request+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_request(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct ldpmsg_req *preq     = NULL;
    struct ldptlv_fec *pfectlv  = NULL;

    preq = (struct ldpmsg_req *)pbasemsg;
    pfectlv = &preq->fec;

    if (LDP_TLV_PW_FEC == pfectlv->element.type)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "PW\n");
        ldp_recv_pw_request(psess, pbasemsg);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "FEC\n");
        ldp_recv_fec_request(pbasemsg, psess);
    }

    return ERRNO_FAIL;
}


/**
* @brief      <+none+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_req_abort(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    return ERRNO_FAIL;
}


/**
* @brief      <+LDP receive a maping packet, PW maping or FEC maping+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_mapping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct ldpmsg_map   *plabelmap = NULL;
    struct ldptlv_fec   *pfectlv   = NULL;

    plabelmap = (struct ldpmsg_map *)pbasemsg;
    pfectlv = &plabelmap->fec;

    if (LDP_TLV_PW_FEC == pfectlv->element.type)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "PW\n");
        return ldp_recv_pw_maping(psess, pbasemsg);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "FEC\n");
        return ldp_recv_fec_mapping(pbasemsg, psess);
    }
}


/**
* @brief      <+LDP receive a withdraw packet, PW withdraw or FEC withdraw+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_withdraw(struct ldpmsg_base *pbasemsg, struct ldp_sess *psess, struct ip_control *pipcb)
{
    struct ldpmsg_withdraw *pwithdraw = NULL;
    struct ldptlv_fec *pfectlv = NULL;

    pwithdraw = (struct ldpmsg_withdraw *)pbasemsg;
    pfectlv = &pwithdraw->fec;

    /* ADVERTISE_PW  DOWN */
    if (LDP_TLV_PW_FEC == pfectlv->element.type)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "PW\n");
        return ldp_recv_pw_withdraw(psess, pbasemsg);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "FEC\n");
        return ldp_recv_fec_withdraw(pbasemsg, psess);
    }
}

/**
* @brief      <+LDP receive a release packet, PW release or FEC release+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:LDP会话结构 pipcb:报文控制结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+A reply from the withdraw msg receiver means the label maping has been deleted+>
*/
int ldp_recv_release(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct ldpmsg_release *prelease = NULL;
    struct ldptlv_fec *pfectlv = NULL;

    prelease = (struct ldpmsg_release *)pbasemsg;
    pfectlv = &prelease->fec;

    //�ж�FEC ����
    if (LDP_TLV_PW_FEC == pfectlv->element.type)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "PW\n");
        return ldp_recv_pw_release(psess, pbasemsg);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "FEC\n");
        return ldp_recv_fec_release(pbasemsg, psess);
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP receive a udp packet , link hello or target hello +>
* @param[in ] <+ppkt:报文BUFFER结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_udp_pkt(struct pkt_buffer *ppkt)
{
    struct ldpmsg_pdu   *phdr       = NULL;
    struct ldpmsg_base  *pbasemsg   = NULL;
    struct ip_control   *pipcb      = NULL;
    uint32_t peer_lsrid = 0;

    phdr     = ppkt->data;
    pipcb = (struct ip_control *) & (ppkt->cb);
    pipcb->ifindex = ppkt->in_ifindex;
    peer_lsrid = ntohl(phdr->lsrid);

    if (pkt_pull(ppkt, LDP_PDU_HEADER_LEN))
    {
        MPLS_LDP_ERROR("pkt_pull failed\n");
        return ERRNO_FAIL;
    }

    pbasemsg = ppkt->data;

    if(LDP_MSG_HELLO == ntohs(pbasemsg->msg_type))
    {
        ldp_recv_hello(pbasemsg, peer_lsrid, pipcb);
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP recevie a TCP packet+>
* @param[in ] <+ppkt:报文BUFFER结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
    
    
int ldp_recv_tcp_pkt(struct pkt_buffer *ppkt)
{
    struct ldpmsg_pdu   *ppdu = NULL;
    struct ldpmsg_base  *pbasemsg = NULL;
    struct ip_control   *pipcb = NULL;  
    struct ldp_sess     *psess = NULL;
    uint32_t peer_lsrid;
    uint32_t pdu_len = 0;
    uint32_t len_tmp = 0;
    uint32_t msg_len = 0;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_FSM, "with packet : %s\n", pkt_dump(ppkt->data,ppkt->data_len));

    pipcb = (struct ip_control *)&(ppkt->cb);
    pipcb->ifindex = ppkt->in_ifindex;

    /* 如果报文长短不小于一个PDU+消息基础头长度则按照一个PDU处理 */
    while (ppkt->data_len >= (LDP_PDU_HEADER_LEN + LDP_BASEMSG_LEN))
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_FSM, "ppkt->data_len %d\n", ppkt->data_len);
        ppdu     = ppkt->data;
        peer_lsrid = ntohl(ppdu->lsrid);
        pdu_len = ntohs(ppdu->pdu_len) + LDP_HEADER_LEN;

        MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, "ppkt->data_len %d\n", ppkt->data_len);
        
        psess = ldp_session_lookup(LDP_MPLS_LSRID, peer_lsrid);
        if(NULL == psess)
        {
            gldp.pkt_err++;
            MPLS_LDP_ERROR("ldp droping packet for matching session failed\n");
            return ERRNO_FAIL;
        }
        /* Pbasemsg偏向该PDU的第一个基础消息头 */
        pbasemsg = (struct ldpmsg_base *)((char *)ppkt->data + LDP_PDU_HEADER_LEN);

        /* 如果PDU整体长度减去当前处理的MSG的整体长度后长度仍超过PDU头部长度+消息基础头长度则认为PDU集成了其他的消息 */
        for(len_tmp = pdu_len, msg_len = 0;(len_tmp -= msg_len) >= (LDP_PDU_HEADER_LEN + LDP_BASEMSG_LEN);
        pbasemsg = (struct ldpmsg_base *)((char *)pbasemsg + msg_len))
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, "pbasemsg %p, len_tmp %d\n", pbasemsg, len_tmp);

            /* 获取消息整体长度 */
            msg_len = ntohs(pbasemsg->msg_len) + LDP_BASEMSG_LEN;

            /* 处理当前消息 */
            switch(ntohs(pbasemsg->msg_type))
            {
                case LDP_MSG_INIT:
                    ldp_recv_init(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_KEEPALIVE:
                    ldp_recv_keepalive(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_ADDR:
                    ldp_recv_address_maping(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_ADDRWITH:
                    ldp_recv_address_withdraw(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_NOTIFY:
                    ldp_recv_notification(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_LABEL_MAPPING:
                    ldp_recv_mapping(pbasemsg, psess, pipcb);
                    break;          
                case LDP_MSG_LABEL_REQUSET:
                    ldp_recv_request(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_LABEL_ABORT:
                    ldp_recv_req_abort(pbasemsg, psess, pipcb);
                    break;              
                case LDP_MSG_LABEL_WITHRAW:
                    ldp_recv_withdraw(pbasemsg, psess, pipcb);
                    break;
                case LDP_MSG_LABEL_RELEASE:
                    ldp_recv_release(pbasemsg, psess, pipcb);
                    break;          
                default:
                    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_FSM, " now drop the packet for unknown msg type %x.\n", pbasemsg->msg_type);
                    break;
            }
        }

        /* 移除整个PDU */
        if(pkt_pull(ppkt, pdu_len)) 
        {
            MPLS_LDP_ERROR("ldp droping packet for packet len error %d\n", ppkt->data_len);
            return ERRNO_FAIL;
        }

    }
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP receive a packet, UDP or TCP+>
* @param[in ] <+ppkt:报文BUFFER结构 pkt_type:报文类型+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_pkt(struct pkt_buffer *ppkt, uint32_t pkt_type)
{
    MPLS_BOARD_MASTER(gmpls);
    
    MPLS_LDP_PKT_RECV(gldp);
    switch (pkt_type)
    {
        case LDP_PKT_TYPE_UDP:
            ldp_recv_udp_pkt(ppkt);
            break;

        case LDP_PKT_TYPE_TCP:
            ldp_recv_tcp_pkt(ppkt);
            break;

        default:
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
            break;
    }
    return ERRNO_SUCCESS;
}



