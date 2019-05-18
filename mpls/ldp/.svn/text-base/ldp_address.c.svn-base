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
#include "ldp_address.h"
#include "ldp_fec.h"
#include "ldp_lsptrigger.h"

/**
* @brief      <+响应接口地址变动事件，通知LDP会话对端更新地址列表+>
* @param[in ] <+ifindex:事件接口索引 pevent:事件结构指针 flag:事件类型 +>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+只处理IPv4事件+>
*/
int ldp_respond_ifaddr_event(uint32_t ifindex, struct ifm_event *pevent, uint32_t flag)
{
    struct mpls_if      *pif        = NULL;
    struct ldp_adjance *padjance   = NULL;
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct list         *plist      = NULL;
    uint32_t ipaddr = 0;

    pif = mpls_if_lookup(ifindex);
    if(NULL == pif)
    {
        return ERRNO_SUCCESS;
    }

    if(0 == pif->ldp_conf.neighbor_list.count)
    {
        return ERRNO_SUCCESS;
    }

    if (INET_FAMILY_IPV4 != pevent->ipaddr.type)
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
        return ERRNO_SUCCESS;
    }

    plist = &pif->ldp_conf.neighbor_list;
    ipaddr = pevent->ipaddr.addr.ipv4;
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if ((padjance->type == LDP_SESSION_LOCAL) && (padjance->ifindex == ifindex))
        {
            switch (flag)
            {
                case IFNET_EVENT_IP_ADD:
                    ldp_send_address_maping(padjance, ipaddr);
                    break;

                case IFNET_EVENT_IP_DELETE:
                    ldp_send_address_withdraw(padjance, ipaddr);
                    break;

                default:
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
                    break;
            }
        }
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
    return ERRNO_SUCCESS;
}

/**
* @brief      <+发送会话所有邻居接口地址给对端，通知地址添加+>
* @param[in ] <+psess:LDP 会话结构指针+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+LDP会话OPERATIONAL时调用+>
*/
int ldp_send_addresses_maping(struct ldp_sess *psess)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnodenext  = NULL;
    struct ldp_adjance *padjance  = NULL;
    struct ifm_l3    pl3if;
    uint32_t i = 0;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "to peer lsrid %s .\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    for (ALL_LIST_ELEMENTS(&psess->adjacency_list, pnode, pnodenext, padjance))
    {
        if (NULL != padjance)
        {
            if (ifm_get_l3if(padjance->ifindex, MODULE_ID_MPLS, &pl3if) < 0)
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "failed for find l3if NULL\n");
                continue;
            }
            for (i = 0; ((i < IFM_IP_NUM) && (pl3if.ipv4[i].addr != 0)); i++)
            {
                ldp_send_address_maping(padjance, pl3if.ipv4[i].addr);
            }
        }
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+发送邻居接口地址给会话对端，通知地址添加+>
* @param[in ] <+pneighbor：LDP邻居指针，ipaddr:IPv4地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_address_maping(struct ldp_adjance *padjance, uint32_t ipaddr)
{
    struct ldp_pkt pkt;
    struct ldp_sess         *psess = NULL;
    struct ldpmsg_addrmap   *paddr = NULL;
    uint32_t len = LDP_TLV_ADDR_MAP_LEN;

    if ((NULL == padjance->psess) || (0 == ipaddr))
    {
        MPLS_LDP_ERROR();
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(padjance->psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "add the ldp nhphop %s\n", ldp_ipv4_to_str(ipaddr));
    psess = padjance->psess;
    paddr = &(pkt.msg.addr);
    paddr->familiy = htons(INET_FAMILY_IPV4);
    paddr->ipv4 = htonl(ipaddr);

    ldp_set_tlvbase(&(paddr->basetlv), LDP_TLV_ADDR, len);

    len += (LDP_BASETLV_LEN + LDP_MSGID_LEN);
    ldp_set_msgbase(&(paddr->basemsg), LDP_MSG_ADDR, len);

    len += (LDP_IDENTIFY_LEN + LDP_BASEMSG_LEN);
    ldp_set_header(&(pkt.header), psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    return ldp_send_tcp_pkt(psess, &pkt, len);
}

/**
* @brief      <+发送邻居接口地址给会话对端，通知地址撤回+>
* @param[in ] <+pneighbor：LDP邻居指针，ipaddr:IPv4地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_address_withdraw(struct ldp_adjance *padjance, uint32_t ipaddr)
{
    struct ldp_pkt pkt;
    struct ldp_sess             *psess = NULL;
    struct ldpmsg_addrwithdraw  *paddr = NULL;
    uint32_t len = LDP_TLV_ADDR_MAP_LEN;

    if (NULL == padjance->psess)
    {
        MPLS_LDP_ERROR();
        return ERRNO_SUCCESS;
    }
    MPLS_LDP_DEBUG(padjance->psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "cancel the ldp nhphop %s\n", ldp_ipv4_to_str(ipaddr));
    psess = padjance->psess;
    paddr = &(pkt.msg.addrelease);
    paddr->familiy = htons(INET_FAMILY_IPV4);
    paddr->ipv4 = htonl(ipaddr);

    ldp_set_tlvbase(&(paddr->basetlv), LDP_TLV_ADDR, len);

    len += (LDP_BASETLV_LEN + LDP_MSGID_LEN);
    ldp_set_msgbase(&(paddr->basemsg), LDP_MSG_ADDRWITH, len);

    len += (LDP_IDENTIFY_LEN + LDP_BASEMSG_LEN);
    ldp_set_header(&(pkt.header), psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    return ldp_send_tcp_pkt(psess, &pkt, len);
}


/**
* @brief      <+解析地址消息，更新对应会话中地址列表+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_address_maping(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct inet_addr    addr;
    struct ldptlv_base  *pbasetlv = (struct ldptlv_base  *)((char *)pbasemsg + sizeof(struct ldpmsg_base));
    uint16_t            *pfamiliy = (uint16_t *)((char *)pbasetlv + sizeof(struct ldptlv_base));
    uint32_t            *pipv4 = (uint32_t *)((char *)pfamiliy + sizeof(uint16_t));
    uint32_t            num = ntohs(pbasetlv->tlv_len) / sizeof(uint32_t);//caculate the addr num in the packet
    uint32_t            i = 0;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "iptype %s ", (INET_FAMILY_IPV4 == ntohs(*pfamiliy)) ? "ipv4":"ipv6");

    if (ntohs(*pfamiliy) != INET_FAMILY_IPV4)
    {
        goto out;
    }

    for (i = 0; i < num; i++)
    {
        memset(&addr, 0, sizeof(struct inet_addr));
        addr.type = INET_FAMILY_IPV4;
        addr.addr.ipv4 = ntohl(pipv4[i]);
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "receive maping ldp nhp %s\n", ldp_ipv4_to_str(addr.addr.ipv4));
        if (ERRNO_SUCCESS != ldp_peer_ifaddr_add(psess, &addr))
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "receive maping ldp nhp %s failed\n", ldp_ipv4_to_str(addr.addr.ipv4));
        }

        ldp_addr_add_nhp(psess, &addr);
    }

out:
    return ERRNO_SUCCESS;
}

/**
* @brief      <+解析地址消息，更新对应会话中地址列表+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_address_withdraw(struct ldpmsg_base  *pbasemsg, struct ldp_sess *psess, struct ip_control  *pipcb)
{
    struct inet_addr    addr;
    struct ldptlv_base  *pbasetlv = (struct ldptlv_base  *)((char *)pbasemsg + sizeof(struct ldpmsg_base));
    uint16_t            *pfamiliy = (uint16_t *)((char *)pbasetlv + sizeof(struct ldptlv_base));
    uint32_t            *pipv4 = (uint32_t *)((char *)pfamiliy + sizeof(uint16_t));
    uint32_t            num = ntohs(pbasetlv->tlv_len) / sizeof(uint32_t);//caculate the addr num in the packet
    uint32_t            i = 0;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if (ntohs(*pfamiliy) != INET_FAMILY_IPV4)
    {
        goto out;
    }

    for (i = 0; i < num; i++)
    {
        memset(&addr, 0, sizeof(struct inet_addr));
        addr.type = INET_FAMILY_IPV4;
        addr.addr.ipv4 = ntohl(pipv4[i]);
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "withdraw ldp nhp %s", ldp_ipv4_to_str(addr.addr.ipv4));
        if (ERRNO_SUCCESS != ldp_peer_ifaddr_del(psess, &addr))
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "withdraw nhp addr %s failed", ldp_ipv4_to_str(addr.addr.ipv4));
        }
        ldp_addr_del_nhp(psess, &addr);
    }

out:
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP 会话添加一个对端发送过来的链路地址+>
* @param[in ] <+psess:LDP会话指针 paddr:需要添加的地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+本地维护的对端的LDP接口地址信息+>
*/
int ldp_peer_ifaddr_add(struct ldp_sess *psess, struct inet_addr *paddr)
{
    struct inet_addr *paddr_new = NULL;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, " nhp %s\n", ldp_ipv4_to_str(paddr->addr.ipv4));

    if (NULL == ldp_peer_ifaddr_lookup(psess, paddr))
    {
        paddr_new = (struct inet_addr *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct inet_addr));
        if (NULL == paddr_new)
        {
            return ERRNO_MALLOC;
        }

        memcpy(paddr_new, paddr, sizeof(struct inet_addr));

        listnode_add(&psess->peer_ifaddrs, paddr_new);
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP 会话删除一个对端发送过来的链路地址+>
* @param[in ] <+psess:LDP会话指针 paddr:需要添加的地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+本地维护的对端的LDP接口地址信息+>
*/
int ldp_peer_ifaddr_del(struct ldp_sess *psess, struct inet_addr *paddr)
{
    struct listnode  *pnode         = NULL;
    struct listnode  *pnextnode     = NULL;
    struct inet_addr *paddr_lookup  = NULL;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, " nhp %s\n", ldp_ipv4_to_str(paddr->addr.ipv4));

    for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode, pnextnode, paddr_lookup))
    {
        if ((paddr->type == paddr_lookup->type) && (paddr->addr.ipv4 == paddr_lookup->addr.ipv4))
        {
            listnode_delete(&psess->peer_ifaddrs, paddr_lookup);
            XFREE(MTYPE_LDP_ENTRY, paddr_lookup);
            return ERRNO_SUCCESS;
        }
    }

    return ERRNO_FAIL;
}

/**
* @brief      <+LDP会话添加一个链路地址后更新LDP FEC下一跳+>
* @param[in ] <+psess:LDP会话指针 paddr:需要添加的地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author       wus
* @date         2018/3/1
* @note       <+none+>
*/
int ldp_addr_add_nhp(struct ldp_sess *psess, struct inet_addr *paddr)
{
    struct listnode     *pnode          = NULL;
    struct listnode     *pnodenext      = NULL;
    struct route_entry  *proute         = NULL;
    struct ldp_fec_t    *pldp_fec       = NULL;
    struct lsp_fec_t    *plsp_fec       = NULL;
    struct lsp_nhp_t nhp;
    uint32_t i = 0;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "for nhp %s added\n", ldp_ipv4_to_str(paddr->addr.ipv4));
    for (ALL_LIST_ELEMENTS(&psess->feclist, pnode, pnodenext, pldp_fec))
    {
        if (pldp_fec)
        {
            proute = route_com_get_route(&pldp_fec->prefix, 0, MODULE_ID_MPLS);
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
                    /* if the prefix in the proute is a egress node in the ldp, continue check the next fec prefix */
                    if (LDP_ROUTE_EGRESS_NODE_YES == is_ldp_route_egress_node(proute))
                    {
                        continue;
                    }
                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "fec prefix %s route nexthop[%d] %s\n",
                                   ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4), i, ldp_ipv4_to_str2(proute->nhp[i].nexthop_connect.addr.ipv4));

                    if (proute->nhp[i].nexthop_connect.addr.ipv4 == paddr->addr.ipv4)
                    {
                        /* if fec nhp_cnt = 0, it is a inactived */
                        if (0 == pldp_fec->nhp_cnt)
                        {
                            ldp_fec_flag_update(psess, pldp_fec, LDP_SESS_FEC_ACTIVE);
                        }
                        /* add nhp */
                        plsp_fec = lsp_fec_lookup(&pldp_fec->prefix);
                        if (plsp_fec)
                        {
                            memset(&nhp, 0, sizeof(struct lsp_nhp_t));
                            nhp.peer_lsrid = psess->key.peer_lsrid;
                            nhp.outlabel = pldp_fec->outlabel;
                            nhp.nexthop.type = INET_FAMILY_IPV4;
                            nhp.nexthop.addr.ipv4 = paddr->addr.ipv4;
                            nhp.type = LSP_TYPE_LDP;

                            /* update the nhp of the lsp fec */
                            if (NULL == lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp))
                            {
                                lsp_fec_add_nhp(plsp_fec, &nhp);
                                pldp_fec->nhp_cnt++;
                                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pfec_active->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                            }
                            else
                            {
                                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "error for try to add the same nhp the second time.\n");
                            }
                        }
                        else
                        {
                            MPLS_LDP_ERROR( "active ldp fec find lsp_fec %s\n", ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4));
                            /* ldp debug fetal error */
                        }
                    }
                }
                mem_share_free_bydata(proute, MODULE_ID_MPLS);
            }
        }
    }
    return ERRNO_SUCCESS;
}


/**
* @brief      <+LDP会话删除一个链路地址后更新LDP FEC下一跳+>
* @param[in ] <+psess:LDP会话指针 paddr:需要添加的地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/

int ldp_addr_del_nhp(struct ldp_sess *psess, struct inet_addr *paddr)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_fec_t *pldp_fec    = NULL;
    struct lsp_fec_t *plsp_fec    = NULL;
    struct lsp_nhp_t *pnhp        = NULL;
    struct lsp_nhp_t  nhp;

    memset(&nhp, 0, sizeof(struct lsp_nhp_t));
    nhp.nexthop.type = INET_FAMILY_IPV4;
    nhp.nexthop.addr.ipv4 = paddr->addr.ipv4;
    nhp.type = LSP_TYPE_LDP;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "for nhp %s deleted\n", ldp_ipv4_to_str(paddr->addr.ipv4));
    for (ALL_LIST_ELEMENTS(&psess->feclist, pnode, pnodenext, pldp_fec))
    {
        if (pldp_fec)
        {
            if (0 != pldp_fec->nhp_cnt)
            {
                plsp_fec = lsp_fec_lookup(&pldp_fec->prefix);
                if (plsp_fec)
                {
                    pnhp = lsp_fec_lookup_nhp(&plsp_fec->nhplist, &nhp);
                    if (pnhp)
                    {
                        lsp_fec_delete_nhp(plsp_fec, pnhp);
                        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                        if (0 == --pldp_fec->nhp_cnt)
                        {
                            ldp_fec_flag_update(psess, pldp_fec, LDP_SESS_FEC_INACTIVE);
                        }
                        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "pldp_fec->nhp_cnt %d\n", pldp_fec->nhp_cnt);
                    }
                }
            }
        }
    }
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP会话清除本地缓存的到对端的链路地址+>
* @param[in ] <+psess:LDP会话指针+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+LDP会话DOWN时调用+>
*/
int ldp_peer_ifaddrs_clear(struct ldp_sess *psess)
{
    struct listnode *pnode          = NULL;
    struct listnode *pnextnode      = NULL;
    struct inet_addr *paddr_lookup  = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode, pnextnode, paddr_lookup))
    {
        if (paddr_lookup)
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
            listnode_delete(&psess->peer_ifaddrs, paddr_lookup);
            XFREE(MTYPE_LDP_ENTRY, paddr_lookup);
        }
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+查询本回话缓存的对端的地址列表+>
* @param[in ] <+psess:LDP会话指针 paddr:地址+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct inet_addr *ldp_peer_ifaddr_lookup(struct ldp_sess *psess, struct inet_addr *paddr)
{
    struct listnode *pnode = NULL;
    struct listnode *pnextnode = NULL;
    struct inet_addr *paddr_lookup = NULL;

    for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode, pnextnode, paddr_lookup))
    {
        if (NULL != paddr_lookup)
        {
            if ((paddr->type == paddr_lookup->type) && (paddr->addr.ipv4 == paddr_lookup->addr.ipv4))
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "lookup addr %s successful ", ldp_ipv4_to_str(paddr->addr.ipv4));
                return paddr_lookup;
            }
        }
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_LABEL, "lookup addr %s failed", ldp_ipv4_to_str(paddr->addr.ipv4));
    return NULL;
}

