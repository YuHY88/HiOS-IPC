/*
*   ldp label ?��1?��?o����y
*/
#include <string.h>

#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/zassert.h>
#include <lib/prefix.h>
#include <lib/memtypes.h>
#include <lib/memory.h>

#include <mpls/lsp_fec.h>
#include <mpls/lspm.h>
#include <mpls/labelm.h>
#include <mpls/mpls.h>
#include <mpls/mpls_if.h>
#include "ldp.h"
#include "ldp_session.h"
#include "ldp_label.h"
#include "ldp_pw.h"
#include "ldp_address.h"
/**
* @brief      <+处理LDP FEC REQUEST报文+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:处理消息的会话+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_fec_request(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess)
{

    return ERRNO_SUCCESS;
}

/**
* @brief      <++>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:处理消息的会话+>
* @param[out] <++>
* @return     <++>
* @author     wus
* @date       2018/3/1
* @note       <++>
*/
int ldp_recv_fec_req_abort(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess)
{
    return ERRNO_FAIL;
}

/**
* @brief      <+处理LDP标签映射消息+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:处理消息的会话+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_fec_mapping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess)
{
    struct ldpmsg_map   *plabelmap      = NULL;
    struct ldptlv_fec   *pfectlv        = NULL;
    struct ldptlv_label *plabeltlv      = NULL;
    struct ldp_fec_t    *pldp_fec       = NULL;
    struct lsp_fec_t    *plsp_fec       = NULL;
    struct route_entry  *proute         = NULL;
    struct inet_prefix  prefix;
    struct lsp_nhp_t    nhp;
    uint32_t outlabel   = 0;
    uint32_t i = 0, j = 0;
    uint32_t nhp_num    = 0;
    uint32_t nexthop[NHP_ECMP_NUM] = {0};

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "learning a label mapping\n");

    plabelmap = (struct ldpmsg_map *)pbasemsg;
    pfectlv = &plabelmap->fec;
    plabeltlv = (struct ldptlv_label *)((char *)pfectlv + (ntohs(pfectlv->basetlv.tlv_len) + LDP_BASETLV_LEN));

    outlabel |= ntohs(plabeltlv->label);
    outlabel |= (plabeltlv->label2) << 16;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "outlabel %x pkt: %s\n", outlabel, pkt_dump(plabelmap, sizeof(struct ldpmsg_map)));
    memset(&prefix, 0, sizeof(struct inet_prefix));
    prefix.type = INET_FAMILY_IPV4;
    prefix.prefixlen = pfectlv->element.fec.prelen;
    prefix.addr.ipv4 = ntohl(pfectlv->element.fec.addr.ipv4);
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "received fec.addr = %s\n", ldp_ipv4_to_str(prefix.addr.ipv4));


    if((prefix.prefixlen < 0) || (prefix.prefixlen > 32))
    {
        MPLS_LDP_ERROR("pfectlv->element.fec.prelen %d\n", pfectlv->element.fec.prelen);
    }

    /* */
    plsp_fec = lsp_fec_lookup(&prefix);
    if (plsp_fec)
    {
        if (LDP_EGRESS_HOST == plsp_fec->egress_type)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "ldp receive local host prefix maping ,now record it and goto out\n");
            ldp_fec_add(psess, &prefix, outlabel);
            goto out;
        }
    }

    pldp_fec = ldp_fec_lookup(psess, &prefix);
    if (NULL == pldp_fec)
    {
        /* if find null a inactive fec will be added */
        pldp_fec = ldp_fec_add(psess, &prefix, outlabel);
    }

    /* caculate the nhp_num by the route and the psess->peer_ifaddrlist */
    proute = route_com_get_route(&prefix, 0, MODULE_ID_MPLS);
    if (proute)
    {
        for (i = 0; i < proute->nhp_num; i++)
        {
            if ((proute->nhpinfo.nhp_type != NHP_TYPE_IP)
                    && (proute->nhpinfo.nhp_type != NHP_TYPE_ECMP)
                    && (proute->nhpinfo.nhp_type != NHP_TYPE_FRR))
            {
                continue;
            }

            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "route nexthop %s\n", ldp_ipv4_to_str(proute->nhp[i].nexthop_connect.addr.ipv4));
            if (NULL != ldp_peer_ifaddr_lookup(psess, &proute->nhp[i].nexthop_connect))
            {
                nexthop[nhp_num] = proute->nhp[i].nexthop_connect.addr.ipv4;
                nhp_num++;
            }
        }
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "proute %p nhp num %d\n", proute, nhp_num);
        mem_share_free_bydata(proute, MODULE_ID_MPLS);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "proute %p nhp num %d\n", proute, nhp_num);
        goto out;
    }

    if (NULL != plsp_fec)
    {
        /*该NHP已经添加过的话则可能需要更新该NHP的出标签*/
        if (nhp_num)
        {
            if (0 != pldp_fec->nhp_cnt)
            {
                if (pldp_fec->outlabel != outlabel)
                {
                    ldp_fec_outlabel_update(psess, pldp_fec, outlabel);
                }
                goto out;
            }
            goto add_nhp;
        }
    }
    else
    {
        if (nhp_num)
        {
            plsp_fec = lsp_fec_create(&prefix, psess->key.peer_lsrid, LDP_EGRESS_NONE);
            if (NULL == plsp_fec)
            {
                goto out;
            }
        }
    }

add_nhp:
    if (nhp_num)
    {
        for (j = 0; j < nhp_num; j++)
        {
            memset(&nhp, 0, sizeof(struct lsp_nhp_t));
            nhp.nexthop.type = INET_FAMILY_IPV4;
            nhp.nexthop.addr.ipv4 = nexthop[j];
            nhp.type = LSP_TYPE_LDP;
            nhp.peer_lsrid = psess->key.peer_lsrid;
            nhp.outlabel = outlabel;

            /* add nhps to the lsp fec nhplist */
            if (NULL == lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp))
            {
                lsp_fec_add_nhp(plsp_fec, &nhp);
                if (pldp_fec)
                {
                    pldp_fec->nhp_cnt++;
                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                }
            }
            else
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "new fec add error for repeat\n");
            }
        }

    }

    MPLS_LDP_LABEL_CNT_RECV(gldp);
out:
    return ERRNO_SUCCESS;
}

/**
* @brief      <+处理LDP标签映射撤回消息+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:处理消息的会话+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_fec_withdraw(struct ldpmsg_base *pbasemsg, struct ldp_sess *psess)
{
    struct ldpmsg_withdraw *pwithdraw = NULL;
    struct ldptlv_fec  *pfectlv     = NULL;
    struct ldptlv_label *plabeltlv      = NULL;
    struct ldp_fec_t   *pldp_fec    = NULL;
    struct inet_prefix  prefix;
    uint32_t outlabel = 0;

    pwithdraw = (struct ldpmsg_withdraw *)pbasemsg;
    pfectlv = &pwithdraw->fec;
    plabeltlv = (struct ldptlv_label *)((char *)pfectlv + (ntohs(pfectlv->basetlv.tlv_len) + LDP_BASETLV_LEN));

    outlabel |= ntohs(plabeltlv->label);
    outlabel |= (plabeltlv->label2) << 16;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "remove label mapping\n");
    memset(&prefix, 0, sizeof(struct inet_prefix));
    prefix.type = INET_FAMILY_IPV4;
    prefix.prefixlen = pfectlv->element.fec.prelen;
    prefix.addr.ipv4 = ntohl(pfectlv->element.fec.addr.ipv4);

    if((prefix.prefixlen < 0) || (prefix.prefixlen > 32))
    {
        MPLS_LDP_ERROR("pfectlv->element.fec.prelen %d\n", pfectlv->element.fec.prelen);
    }

    
    MPLS_LDP_ERROR("prefixlen %x, ipaddr %x\n", prefix.prefixlen, prefix.addr.ipv4);
    pldp_fec = ldp_fec_lookup(psess, &prefix);
    if (pldp_fec)
    {
        MPLS_LDP_ERROR("fec len %x, fec addr %x\n", pldp_fec->prefix.prefixlen, pldp_fec->prefix.addr.ipv4);
        ldp_send_fec_release(psess, pldp_fec, outlabel);
        ldp_fec_del(psess, pldp_fec);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "active fec is not founded.\n");
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+处理LDP标签释放消息+>
* @param[in ] <+pbasemsg:LDP基础消息头 psess:处理消息的会话+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+ A reply from the withdraw msg receiver means the label maping has been deleted +>
*/
int ldp_recv_fec_release(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess)
{
    struct ldpmsg_release *prelease = NULL;
    struct ldptlv_fec *pfectlv = NULL;
    struct ldp_fec_t *pldp_fec = NULL;
    struct inet_prefix prefix;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "relase a label mapping\n");

    prelease = (struct ldpmsg_release *)pbasemsg;
    pfectlv = &prelease->fec;
    memset(&prefix, 0, sizeof(struct inet_prefix));
    prefix.type = INET_FAMILY_IPV4;
    prefix.prefixlen = pfectlv->element.fec.prelen;
    prefix.addr.ipv4 = ntohl(pfectlv->element.fec.addr.ipv4);

    if((prefix.prefixlen < 0) || (prefix.prefixlen > 32))
    {
        MPLS_LDP_ERROR("pfectlv->element.fec.prelen %d\n", pfectlv->element.fec.prelen);
    }

    pldp_fec = ldp_fec_lookup(psess, &prefix);
    if (!pldp_fec)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "The label has been released when the route is deleted !");
        return ERRNO_SUCCESS;
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "The label has been deleted failed when the route is deleted !");
    }
    return ERRNO_FAIL;
}

/**
* @brief      <+LDP发送一个标签请求消息+>
* @param[in ] <+psess:ldp会话 prefix:fec 前缀+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+DOD标签发布模式使用，上游设备主动向下游请求特定FEC的标签发布+>
*/
int ldp_send_fec_request(struct ldp_sess *psess, struct inet_prefix *prefix)
{
    return ERRNO_SUCCESS;
}
/**
* @brief      <+发布一个标签请求停止+>
* @param[in ] <+psess:ldp会话 prefix:fec 前缀+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+DOD标签发布模式使用，表示撤回之前一个未得到回应的标签请求消息+>
*/
int ldp_send_fec_req_abort(struct ldp_sess *psess, struct inet_prefix *prefix)
{
    struct ldp_pkt pkt;
    uint32_t len = 0;

    return ldp_send_tcp_pkt(psess, &pkt, len);
}

/**
* @brief      <+LDP发布一个标签映射消息给上游设备+>
* @param[in ] <+psess:ldp会话 pfec:lsp fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+An LSR sends a Label Release message to an LDP peer 
                to signal the peer that the LSR no longer needs specific FEC-label 
                mappings previously requested of and/or advertised by the peer.+>
*/
int ldp_send_fec_mapping(struct ldp_sess *psess, struct lsp_fec_t *pfec)
{
    struct ldp_pkt       pkt;
    struct ldpmsg_pdu   *pheader     = NULL;
    struct ldpmsg_base  *pbasemsg    = NULL;
    struct ldptlv_fec   *pfec_temp   = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char    *pend   = NULL;
    uint32_t len    = 0;

    if ((NULL == psess) || (NULL == pfec))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "send to peer %s\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);

    plabel_temp = (struct ldptlv_label *)(pend - sizeof(struct ldptlv_label));
    plabel_temp->label = pfec->inlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (pfec->inlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);

    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - sizeof(struct fec_element) - LDP_BASETLV_LEN);
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pfec_temp %p plabel_temp %p.\n", pfec_temp, plabel_temp);
    /* gernel fec tlv */
    pfec_temp->element.fec.type = 0x02;
    pfec_temp->element.fec.family = htons(INET_FAMILY_IPV4);
    pfec_temp->element.fec.prelen = pfec->prefix.prefixlen;//len of the addr len
    pfec_temp->element.fec.addr.ipv4 = htonl(pfec->prefix.addr.ipv4);
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_FEC_VAL_LEN);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pfec_temp %p.\n", pfec_temp);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_MAPPING, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    MPLS_LDP_LABEL_CNT_SEND(gldp);

    return ldp_send_tcp_pkt(psess, pheader, len);
}

/**
* @brief      <+LDP发布一个标签撤回消息+>
* @param[in ] <+psess:ldp会话 pfec:lsp fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+通知上游设备撤回一组标签绑定信息+>
*/
int ldp_send_fec_withdraw(struct ldp_sess *psess, struct lsp_fec_t *pfec)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader    = NULL;
    struct ldpmsg_base  *pbasemsg   = NULL;
    struct ldptlv_fec   *pfec_temp  = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char    *pend   = NULL;
    uint32_t len    = 0;

    if ((NULL == psess) || (NULL == pfec))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "ldp send fec withdraw msg to peer %s\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
    plabel_temp = (struct ldptlv_label *)(pend - sizeof(struct ldptlv_label));
    plabel_temp->label = pfec->inlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (pfec->inlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);
    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - sizeof(struct fec_element) - LDP_BASETLV_LEN);

    /* gernel fec tlv */
    pfec_temp->element.fec.type = 0x02;
    pfec_temp->element.fec.family = htons(INET_FAMILY_IPV4);
    pfec_temp->element.fec.prelen = pfec->prefix.prefixlen;
    pfec_temp->element.fec.addr.ipv4 = htonl(pfec->prefix.addr.ipv4);
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_FEC_VAL_LEN);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_WITHRAW, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    return ldp_send_tcp_pkt(psess, pheader, len);
}

/**
* @brief      <+发送一个标签释放消息+>
* @param[in ] <+psess:ldp会话 pfec:lsp fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+通知下游设备标签绑定信息已回收+>
*/
int ldp_send_fec_release(struct ldp_sess *psess, struct ldp_fec_t *pfec, uint32_t outlabel)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader    = NULL;
    struct ldpmsg_base  *pbasemsg   = NULL;
    struct ldptlv_fec   *pfec_temp  = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char *pend = NULL;
    uint32_t len = 0;

    if ((NULL == psess) || (NULL == pfec))
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "send a label release\n");
    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);

    plabel_temp = (struct ldptlv_label *)(pend - sizeof(struct ldptlv_label));
    plabel_temp->label = outlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (outlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);
    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - sizeof(struct fec_element) - LDP_BASETLV_LEN);

    /* gernel fec tlv */
    pfec_temp->element.fec.type = 0x02;
    pfec_temp->element.fec.family = htons(INET_FAMILY_IPV4);
    pfec_temp->element.fec.prelen = pfec->prefix.prefixlen;
    pfec_temp->element.fec.addr.ipv4 = htonl(pfec->prefix.addr.ipv4);
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_FEC_VAL_LEN);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_RELEASE, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    return ldp_send_tcp_pkt(psess, pheader, len);
}
/**
* @brief      <+send label maping to all the ldp session when a new fec founded +>
* @param[in ] <+psess:ldp会话 pfec:lsp fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+ MPLS_LDP_FEC_BROADCAST__MANAGEMENT +>
*/
int ldp_fec_maping_broadcast(struct lsp_fec_t *pfec)
{
    struct ldp_sess     *psess   = NULL;
    struct hash_bucket  *pbucket = NULL;
    int cursor;

    if ((NULL == pfec) || (LABEL_ADVERTISE_DOD == gldp.advertise))
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "%s\n", ldp_ipv4_to_str(pfec->prefix.addr.ipv4));
    
    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if (psess)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "status %d psess->type %d", psess->status, psess->type);
            if (LDP_STATUS_UP == psess->status)
            {
                ldp_send_fec_mapping(psess, pfec);
            }
        }
    }
    return ERRNO_SUCCESS;
}
/**
* @brief      <+send ldp label withdraw msg to all the ldp session +>
* @param[in ] <+psess:ldp会话 pfec:lsp fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+ MPLS_LDP_FEC_BROADCAST__MANAGEMENT +>
*/
int ldp_fec_withdraw_broadcast(struct lsp_fec_t *pfec)
{
    struct hash_bucket  *pbucket = NULL;
    struct ldp_sess     *psess   = NULL;
    int cursor;

    if ((NULL == pfec) || (LABEL_ADVERTISE_DOD == gldp.advertise))
    {
        return ERRNO_FAIL;
    }

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "%s\n", ldp_ipv4_to_str(pfec->prefix.addr.ipv4));

    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if (psess)
        {
            if ((LDP_STATUS_UP == psess->status) && (LDP_SESSION_LOCAL & psess->type))
            {
                ldp_send_fec_withdraw(psess, pfec);
            }
        }
    }
    return ERRNO_SUCCESS;
}


