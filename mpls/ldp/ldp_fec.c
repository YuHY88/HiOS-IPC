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
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_fec.h"
#include "ldp_address.h"
#include "ldp_lsptrigger.h"

/**
* @brief      <+响应路由添加事件，更新LDP FEC+>
* @param[in ] <+prefix:地址前缀 nexthop:下一跳地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static int ldp_update_fec_for_rout_add(struct inet_prefix *prefix, struct inet_addr *nexthop)
{
    struct hash_bucket  *pbucket     = NULL;
    struct ldp_fec_t    *pldp_fec    = NULL;
    struct lsp_fec_t    *plsp_fec    = NULL;
    struct ldp_sess     *psess       = NULL;
    struct lsp_nhp_t    nhp;
    uint32_t outlabel = 0;
    uint32_t cursor;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "nexthop addr %s\n", ldp_ipv4_to_str(nexthop->addr.ipv4));
    /* Find the session by ldp interface addr, then lookup the ldp_fec in session */
    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if (NULL == ldp_peer_ifaddr_lookup(psess, nexthop))
        {
            continue;
        }
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, " find session peer lsrid %s\n", ldp_ipv4_to_str(psess->key.peer_lsrid));
        pldp_fec = ldp_fec_lookup(psess, prefix);
        if (pldp_fec)
        {
            goto nhp_update;
        }
        goto out;
    }
    goto out;
nhp_update:
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "nhp update\n");
    outlabel = pldp_fec->outlabel;
    if (0 == pldp_fec->nhp_cnt)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "0 nhp now flag update\n");
        ldp_fec_flag_update(psess, pldp_fec, LDP_SESS_FEC_ACTIVE);
    }

    plsp_fec = lsp_fec_lookup(prefix);
    if (NULL == plsp_fec)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "active fec %s find effect fec faield !\n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4));
        goto out;
    }

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "add nhp\n");
    /* add nhp for the route */
    memset(&nhp, 0, sizeof(struct lsp_nhp_t));
    nhp.outlabel = outlabel;
    nhp.type = LSP_TYPE_LDP;
    nhp.peer_lsrid = psess->key.peer_lsrid;
    nhp.nexthop.type = INET_FAMILY_IPV4;
    nhp.nexthop.addr.ipv4 = nexthop->addr.ipv4;

    if (NULL == lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp))
    {
        lsp_fec_add_nhp(plsp_fec, &nhp);
        pldp_fec->nhp_cnt++;
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "fetal error for get the same route the second time\n");
    }

out:
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    return ERRNO_SUCCESS;
}

/**
* @brief      <+响应路由删除事件，更新LDP FEC+>
* @param[in ] <+prefix:地址前缀 nexthop:下一跳地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
static int ldp_update_fec_for_rout_del(struct inet_prefix *prefix, struct inet_addr *nexthop)
{
    struct ldp_sess     *psess    = NULL;
    struct ldp_fec_t    *pldp_fec = NULL;
    struct lsp_fec_t    *plsp_fec = NULL;
    struct lsp_nhp_t    *pnhp     = NULL;
    struct lsp_nhp_t     nhp;
    uint32_t             ds_lsrid = 0;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    plsp_fec = lsp_fec_lookup(prefix);
    if (plsp_fec)
    {
        memset(&nhp, 0, sizeof(struct lsp_nhp_t));
        nhp.type = LSP_TYPE_LDP;
        nhp.nexthop.type = INET_FAMILY_IPV4;
        nhp.nexthop.addr.ipv4 = nexthop->addr.ipv4;

        pnhp = lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp);
        if (NULL != pnhp)
        {
            ds_lsrid = pnhp->peer_lsrid;
            psess = ldp_session_lookup(LDP_MPLS_LSRID, ds_lsrid);
            if (NULL == psess)
            {
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "route delete nhp failed for lookuping ldp session failed\n");
                return ERRNO_NOT_FOUND;
            }
            lsp_fec_delete_nhp(plsp_fec, pnhp);

            pldp_fec = ldp_fec_lookup(psess, prefix);
            if (pldp_fec)
            {
                pldp_fec->nhp_cnt--;
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec prefix %s nhp_cnt %d\n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4), pldp_fec->nhp_cnt);
                if (0 == pldp_fec->nhp_cnt)
                {
                    ldp_fec_flag_update(psess, pldp_fec, LDP_SESS_FEC_INACTIVE);
                }
            }
        }
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    return ERRNO_SUCCESS;
}

/**
* @brief      <+响应路由更新事件，完成本地发布FEC的生成及学习到的FEC的更新+>
* @param[in ] <+proute:路由结构 opcode:操作类型+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_respond_route_event(struct route_entry *proute, uint32_t opcode)
{
    struct lsp_fec_t *plsp_fec = NULL;

    if ((INET_FAMILY_IPV4 != proute->prefix.type) || (INET_FAMILY_IPV4 != proute->nhp[0].nexthop_connect.type))
    {
        return ERRNO_SUCCESS;
    }
    if (FALSE == inet_valid_network(proute->prefix.addr.ipv4))
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "route prefix %s nexthop num %d\n", ldp_ipv4_to_str(proute->prefix.addr.ipv4), proute->nhp_num);

    /* check if the route is a egress node */
    if (LDP_ROUTE_EGRESS_NODE_YES == is_ldp_route_egress_node(proute))
    {
        plsp_fec = lsp_fec_lookup(&proute->prefix);
        if (IPC_OPCODE_ADD == opcode)
        {
            if (NULL != plsp_fec)
            {
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "fatal error for lsp fec not delete for no active route.\n");
                lsp_fec_delete(plsp_fec);
            }
            plsp_fec = lsp_fec_create(&proute->prefix, proute->prefix.addr.ipv4, LDP_EGRESS_HOST);
            if(plsp_fec)
            {
                /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT send label maping */
                ldp_fec_maping_broadcast(plsp_fec);
            }
        }
        else if (IPC_OPCODE_DELETE == opcode)
        {
            if (NULL != plsp_fec)
            {
                /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT send label withdraw */
                ldp_fec_withdraw_broadcast(plsp_fec);
                lsp_fec_delete(plsp_fec);
            }
        }
    }
    else
    {
        /* if the route is not a egress node ,it will update the fec in the ldp session */
        if ((ROUTE_PROTO_STATIC == proute->nhp[0].protocol) || (ROUTE_PROTO_OSPF == proute->nhp[0].protocol))
        {
            if (opcode == IPC_OPCODE_ADD)
            {
                ldp_update_fec_for_rout_add(&proute->prefix, &proute->nhp[0].nexthop_connect);
            }
            else if (opcode == IPC_OPCODE_DELETE)
            {
                ldp_update_fec_for_rout_del(&proute->prefix, &proute->nhp[0].nexthop_connect);
            }
        }
    }
    return ERRNO_SUCCESS;
}

/**
* @brief      <+生成一个LDP FEC结构+>
* @param[in ] <+iptype:前缀地址类型 ipaddr:地址内容 prefixlen:地址掩码+>
* @param[out] <+none+>
* @return     <+返回struct ldp_fec_t *+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_fec_t *ldp_fec_create(uint32_t iptype, uint32_t ipaddr, uint32_t prefixlen)
{
    struct ldp_fec_t  *pldp_fec = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "new ldp fec %s/%d\n",ldp_ipv4_to_str(ipaddr), prefixlen);
    pldp_fec = (struct ldp_fec_t *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_fec_t));
    if (NULL == pldp_fec)
    {
        return NULL;
    }
    memset(pldp_fec, 0, sizeof(struct ldp_fec_t));
    pldp_fec->nhp_cnt = 0;
    pldp_fec->outlabel = 0;
    pldp_fec->prefix.type = iptype;
    pldp_fec->prefix.addr.ipv4 = ipaddr;
    pldp_fec->prefix.prefixlen = prefixlen;

    return pldp_fec;
}

/**
* @brief      <+将LDP FEC结构添加到会话FEC LIST+>
* @param[in ] <+psess:ldp会话 prefix:地址前缀 outlabel:出标签值+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_fec_t *ldp_fec_add(struct ldp_sess *psess, struct inet_prefix  *prefix, uint32_t outlabel)
{
    struct ldp_fec_t *pldp_fec = NULL;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "prefix %s/%d outlabel %u\n", ldp_ipv4_to_str(prefix->addr.ipv4), prefix->prefixlen, outlabel);

    pldp_fec = ldp_fec_create(INET_FAMILY_IPV4, prefix->addr.ipv4, prefix->prefixlen);
    if (NULL == pldp_fec)
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "ldp add fec failed for ldp_fec_create return NULL\n");
        goto out;
    }

    pldp_fec->outlabel = outlabel;
    listnode_add(&psess->feclist, pldp_fec);

out:
    return pldp_fec;

}

/**
* @brief      <+将一个LDP FEC从会话的FEC LIST中删除+>
* @param[in ] <+psess:ldp会话 pldp_fec:会话学习到fec结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fec_del(struct ldp_sess *psess, struct ldp_fec_t *pldp_fec)
{
    if ((NULL == pldp_fec) || (NULL == psess))
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "fec prefix %s\n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4));
    if (0 != pldp_fec->nhp_cnt)
    {
        ldp_fec_flag_update(psess, pldp_fec, LDP_SESS_FEC_INACTIVE);
    }

    listnode_delete(&psess->feclist, pldp_fec);
    XFREE(MTYPE_LDP_ENTRY, pldp_fec);
    return ERRNO_SUCCESS;
}

/**
* @brief      <+根据前缀从LDP会话FEC LIST中查找匹配的LDP FEC节点+>
* @param[in ] <+psess:ldp会话 prefix:地址前缀+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_fec_t *ldp_fec_lookup(struct ldp_sess *psess, struct inet_prefix *prefix)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_fec_t *pldp_fec    = NULL;
    if((prefix->prefixlen < 0) || (prefix->prefixlen > 32))
    {
        MPLS_LDP_ERROR("fatal error for paramer masklen error %d\n", prefix->prefixlen);
        return NULL;
    }
    for (ALL_LIST_ELEMENTS(&psess->feclist, pnode, pnodenext, pldp_fec))
    {
        if (pldp_fec)
        {
            if (1 == ipv4_is_same_subnet(pldp_fec->prefix.addr.ipv4, prefix->addr.ipv4, prefix->prefixlen))
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "by prefix %s successful\n", ldp_ipv4_to_str(prefix->addr.ipv4));
                return pldp_fec;
            }
        }
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "by prefix %s failed\n", ldp_ipv4_to_str(prefix->addr.ipv4));
    return NULL;
}

/**
* @brief      <+清除LDP 会话中FEC LIST+>
* @param[in ] <+psess:ldp会话+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fec_clear(struct ldp_sess *psess)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_fec_t *pldp_fec    = NULL;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "clear all ldp fecs in ldp session\n");
    for (ALL_LIST_ELEMENTS(&psess->feclist, pnode, pnodenext, pldp_fec))
    {
        if (pldp_fec)
        {
            ldp_fec_del(psess, pldp_fec);
        }
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+更新已经学习到LDP FEC的OUTLABLE值+>
* @param[in ] <+psess:ldp会话 pldp_fec:需要更新出标签的ldp fec结构 outlabel:新出标签+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fec_outlabel_update(struct ldp_sess *psess, struct ldp_fec_t *pldp_fec, uint32_t outlabel)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct lsp_fec_t *plsp_fec    = NULL;
    struct inet_addr *paddr       = NULL;
    struct lsp_nhp_t *pnhp        = NULL;
    struct lsp_nhp_t  nhp;

    if ((NULL == psess) || (NULL == pldp_fec))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "fec prefix %s  old label %d  new lable %d\n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4), pldp_fec->outlabel, outlabel);
    if (pldp_fec->outlabel == outlabel)
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    if (0 != pldp_fec->nhp_cnt)
    {
        plsp_fec = lsp_fec_lookup(&pldp_fec->prefix);
        if (plsp_fec)
        {
            for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode, pnodenext, paddr))
            {
                if (paddr)
                {
                    memset(&nhp, 0, sizeof(struct lsp_nhp_t));
                    nhp.outlabel = outlabel;
                    nhp.type = LSP_TYPE_LDP;
                    nhp.nexthop.type = INET_FAMILY_IPV4;
                    nhp.peer_lsrid = psess->key.peer_lsrid;
                    nhp.nexthop.addr.ipv4 = paddr->addr.ipv4;

                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "paddr->addr.ipv4 %s\n", ldp_ipv4_to_str(paddr->addr.ipv4));
                    pnhp = lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp);
                    if (pnhp)
                    {
                        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                        lsp_fec_delete_nhp(plsp_fec, pnhp);//del old lsp
                        lsp_fec_add_nhp(plsp_fec, &nhp);//add new lsp
                    }
                }
            }
        }
    }
    pldp_fec->outlabel = outlabel;
    return ERRNO_SUCCESS;
}

/**
* @brief      <+更新LDP FEC的生效状态，维护LSP FEC TREE+>
* @param[in ] <+psess:ldp会话 pldp_fec:需要更新状态的ldp fec结构 flag:活跃状态标记+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_fec_flag_update(struct ldp_sess *psess, struct ldp_fec_t * pldp_fec, uint16_t flag)
{
    struct listnode  *pnode1      = NULL;
    struct listnode  *pnodenext1  = NULL;
    struct lsp_fec_t *plsp_fec    = NULL;
    struct inet_addr *paddr       = NULL;
    struct lsp_nhp_t *pnhp        = NULL;
    struct lsp_nhp_t  nhp;

    if ((NULL == psess) || (NULL == pldp_fec))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (((0 == pldp_fec->nhp_cnt) && (LDP_SESS_FEC_INACTIVE == flag))
            || ((0 != pldp_fec->nhp_cnt) && (LDP_SESS_FEC_ACTIVE == flag)))
    {
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "fec prefix %s pldp_fec->nhp_cnt %u \n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4), pldp_fec->nhp_cnt);
    if (0 == pldp_fec->nhp_cnt)
    {
        /*inactive fec to active */
        plsp_fec = lsp_fec_lookup(&pldp_fec->prefix);
        if (NULL == plsp_fec)
        {
            plsp_fec = lsp_fec_create(&pldp_fec->prefix, psess->key.peer_lsrid, LDP_EGRESS_NONE);
            if (NULL == plsp_fec)
            {
                return ERRNO_MALLOC;
            }
        }
    }
    else
    {
        /* active fec to inactive */
        plsp_fec = lsp_fec_lookup(&pldp_fec->prefix);
        if (plsp_fec)
        {
            for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode1, pnodenext1, paddr))
            {
                if (paddr)
                {
                    memset(&nhp, 0, sizeof(struct lsp_nhp_t));
                    nhp.type = LSP_TYPE_LDP;
                    nhp.nexthop.type = INET_FAMILY_IPV4;
                    nhp.nexthop.addr.ipv4 = paddr->addr.ipv4;

                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "paddr->addr.ipv4 %s\n", ldp_ipv4_to_str(paddr->addr.ipv4));
                    pnhp = lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp);
                    if (pnhp)
                    {
                        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                        lsp_fec_delete_nhp(plsp_fec, pnhp);
                        if (0 == --pldp_fec->nhp_cnt)
                        {
                            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                        }
                    }
                }
            }

        }
    }

    return ERRNO_SUCCESS;
}

