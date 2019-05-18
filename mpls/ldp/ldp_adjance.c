/*
*   ���� ldp �Ự����ʵ��
*/

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
#include "ldp_notify.h"
#include "ldp_fsm.h"
#include "ldp_address.h"

struct list ldp_neighbor_list;

struct list ldp_remot_adjance_list;

struct list ldp_local_addr_list;

/**
* @brief      <+创建一个LDP邻接体结构+>
* @param[in ] <+local_lsrid:本地LSRID peer_lsrid:对端LSRID peer_ifaddr: 对端HELLO报文原地址 ifindex:本地接口索引 type:邻居类型+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1 
* @note       <+none+>
*/
struct ldp_adjance *ldp_adjance_create(uint32_t local_lsrid, uint32_t peer_lsrid, uint32_t peer_ifaddr, uint32_t ifindex, uint8_t type)
{
    struct list *plist            = NULL;
    struct mpls_if *pif           = NULL;
    struct ldp_adjance *padjance = NULL;
    
    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "create adjance peerlsrid %s type %s\n",ldp_ipv4_to_str( peer_lsrid), LDP_SESSION_LOCAL == type ? "local" : "remote");

    if(LDP_SESSION_LOCAL == type)
    {
        pif = mpls_if_lookup(ifindex);
        if(NULL == pif)
        {
            goto out;
        }
        plist = &pif->ldp_conf.neighbor_list;
    }
    else if(LDP_SESSION_REMOT == type)
    {
        plist = &ldp_remot_adjance_list;
    }
    else
    {
        goto out;
    }

    padjance = (struct ldp_adjance *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_adjance));
    if (NULL == padjance)
    {
        goto out;
    }
    
    memset(padjance, 0, sizeof(struct ldp_adjance));
    padjance->ifindex = ifindex;
    padjance->local_lsrid = local_lsrid;
    padjance->peer_lsrid = peer_lsrid;
    padjance->peer_ip = peer_ifaddr;
    padjance->type = type;
    padjance->neighbor_hold = LDP_NEIGH_EXPIRE_TIME;
    padjance->hello_send = gldp.hello_internal;
    time(&padjance->creat_time);
    if(LDP_SESSION_REMOT == type)
    {
        padjance->tcp_addr.local_addr = LDP_MPLS_LSRID;
        padjance->tcp_addr.peer_addr = peer_lsrid;
        padjance->tcp_addr.priority = 0;
    }
    ldp_adjance_add(padjance, plist);
out:
    return padjance;
}

/**
* @brief      <+将一个LDP邻接体结构加入邻接体链表+>
* @param[in ] <+padjance:邻居结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_adjance_add(struct ldp_adjance *padjance, struct list *plist)
{
    MPLS_LDP_DEBUG(padjance->peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "add adjance peerlsrid %s\n", ldp_ipv4_to_str(padjance->peer_lsrid));

    listnode_add(plist, padjance);

    return ERRNO_SUCCESS;
}

/**
* @brief      <+从LDP邻接体链表中删除一个LDP邻接体+>
* @param[in ] <+padjance:邻居结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_adjance_delete(struct ldp_adjance *padjance, struct list *plist)
{
    MPLS_LDP_DEBUG(padjance->peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "delete adjance peerlsrid %s\n", ldp_ipv4_to_str(padjance->peer_lsrid));
    if ((NULL == padjance) || (NULL == plist))
    {
        return ERRNO_FAIL;
    }
    if (padjance->phello_timer)
    {
        MPLS_TIMER_DEL(padjance->phello_timer);
        padjance->phello_timer = 0;
    }

    if (padjance->phold_timer)
    {
        MPLS_TIMER_DEL(padjance->phold_timer);
        padjance->phold_timer = 0;
    }

    listnode_delete(plist, padjance);
    XFREE(MTYPE_LDP_ENTRY, padjance);

    return ERRNO_SUCCESS;
}

/**
* @brief      <+根据接口索引、hello源地址、邻接体类型及对端LSRID查找一个邻接体+>
* @param[in ] <+peer_lsrid:对端LSRID peer_ifaddr: 对端HELLO报文原地址 local—ifindex:本地接口索引 type:邻居类型+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_adjance *ldp_adjance_lookup(uint32_t peer_lsrid, uint32_t local_ifindex, uint32_t peer_ifaddr, uint8_t type)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct ldp_adjance *padjance  = NULL;
    struct list         *plist      = NULL;
    struct mpls_if      *pif        = NULL;

    if(LDP_SESSION_LOCAL == type)
    {
        pif = mpls_if_lookup(local_ifindex);
        if(NULL == pif)
        {
            goto out;
        }
        if(pif->ldp_conf.neighbor_list.count)
        {
            plist = &pif->ldp_conf.neighbor_list;
        }
        else
        {
            goto out;
        }
    }
    else if(LDP_SESSION_REMOT == type)
    {
        plist = &ldp_remot_adjance_list;
    }
    else
    {
        goto out;
    }

    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, " peer_lsrid %s ifindex %x peer if addr %s.\n", ldp_ipv4_to_str(peer_lsrid), local_ifindex, ldp_ipv4_to_str2(peer_ifaddr));
    /* ldp neighbor use the peer-lsrid , local-ifindex and peer hello sip to find a neighbor */
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if (((padjance->peer_lsrid == peer_lsrid) && (padjance->ifindex == local_ifindex)
                && (padjance->type == type) && (padjance->peer_ip == peer_ifaddr))
                 || ((padjance->ifindex == local_ifindex) && (!peer_lsrid) 
                 && (!peer_ifaddr)))
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp %s neighbor founded .\n", (LDP_SESSION_REMOT == type) ? "remote":"local");
            return padjance;
        }
    }
out:
    return NULL;
}

/**
* @brief      <+定时发送LINK HELLO报文的定时器+>
* @param[in ] <+thread:伪线程结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_adjance_link_hello_timer(void *para)
{
    struct mpls_if  *pif = NULL;
    struct ifm_l3   pl3if;
    uint32_t ifindex = 0;
    uint16_t hello_send = gldp.hello_internal;

    ifindex = (uint32_t)para;
    pif = mpls_if_get(ifindex);
    if (pif == NULL)
    {
        goto out;
    }
    if (ENABLE != pif->ldp_conf.ldp_enable)
    {
        pif->ldp_conf.phello_thread = 0;
        goto out;
    }
    if ((ifm_get_l3if(ifindex, MODULE_ID_MPLS, &pl3if) < 0) || pl3if.ipv4[0].addr == 0)
    {
        goto reset_timer;
    }
    hello_send = pif->ldp_conf.hello_send_effect;
    
    ldp_send_hello(pl3if.ipv4[0].addr, LDP_HELLO_ADDR, ifindex, LDP_SESSION_LOCAL);
reset_timer:
    pif->ldp_conf.phello_thread = MPLS_TIMER_ADD(ldp_adjance_link_hello_timer, (void *)ifindex, hello_send);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ldp reset link hello timer after %d second expire\n", hello_send);
out:
    return ERRNO_SUCCESS;
}

/**
* @brief      <+定时发送远端HELLO报文的定时器+>
* @param[in ] <+thread:伪线程结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_adjance_target_hello_timer(void *para)
{
    struct ldp_adjance *padjance = NULL;
    uint32_t ifindex = 0;

    padjance = (struct ldp_adjance *)(para);
    if (NULL == padjance)
    {
        goto out;
    }
    if (gmpls.ldp_enable == DISABLE)
    {
        goto reset_timer;
    }

    ldp_send_hello(padjance->local_lsrid, padjance->peer_lsrid, ifindex, padjance->type);
reset_timer:
    padjance->phello_timer = MPLS_TIMER_ADD(ldp_adjance_target_hello_timer, (void *)padjance, padjance->hello_send);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ldp reset remote hello timer after %d second expire\n", padjance->hello_send);
out:
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP 邻接体关系HOLD定时器+>
* @param[in ] <+thread:伪线程结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+LDP neighbor hold timer will set when neighbor creat, reset when hello msg received+>
*/
int ldp_adjance_hold_timer(void *para)
{
    struct ldp_adjance *padjance = (struct ldp_adjance *)(para);
    struct mpls_if      *pif      = NULL;
    struct list         *plist    = NULL;
    if (NULL == padjance)
    {
        goto out;
    }
    if (padjance->phold_timer)
    {
        padjance->phold_timer = 0;
    }
    MPLS_LDP_DEBUG(padjance->peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "adjance hold timer expire peer lsrid %s\n", ldp_ipv4_to_str(padjance->peer_lsrid));
    padjance->rcv_cnt = 0;
    if (padjance->psess)
    {
        if(1 == padjance->psess->adjacency_list.count)
        {
            ldp_send_notification(padjance->psess, LDP_NOTIF_HOLD_TIMER_EXPIRED, 0);
        }
        ldp_adjance_leave_sess(padjance, padjance->psess, LDP_EVENT_NEIGHBOR_TIMEOUT);
    }
    /*remote ldp neighbor will not be delete */
    if (LDP_SESSION_LOCAL == padjance->type)
    {
        pif = mpls_if_lookup(padjance->ifindex);
        if(NULL == pif)
        {
            goto out;
        }
        if(pif->ldp_conf.neighbor_list.count)
        {
            plist = &pif->ldp_conf.neighbor_list;
        }
        else
        {
            goto out;
        }
        ldp_adjance_delete(padjance, plist);

        /* recaculate the ldp hello send internal */
        pif->ldp_conf.hello_send_effect = ldp_adjance_hello_internal_recalculate(plist);
    }
out:
    return ERRNO_SUCCESS;
}
/**
* @brief      <+A neighbor will join a LDP session+>
* @param[in ] <+padjance:LDP邻居结构 psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+A LDP session may have many neighbors+>
*/
int ldp_adjance_join_sess(struct ldp_adjance *padjance, struct ldp_sess *psess)
{
    int i = 0;
    struct ifm_l3 pl3if;
    uint8_t  adjacency_type = padjance->type;

    MPLS_LDP_DEBUG(padjance->peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "a adjance join a session peer lsrid %s psess %p\n", ldp_ipv4_to_str(padjance->peer_lsrid), psess);
    padjance->psess = psess;
    psess->type |= adjacency_type;

    /* Send address maping msg for the adjancy */
    if ((LDP_SESSION_LOCAL == adjacency_type) && (LDP_STATUS_UP == psess->status))
    {
        if (ifm_get_l3if(padjance->ifindex, MODULE_ID_MPLS, &pl3if) == 0)
        {
            for (i = 0; ((i < IFM_IP_NUM) && (pl3if.ipv4[i].addr != 0)); i++)
            {
                ldp_send_address_maping(padjance, pl3if.ipv4[i].addr);
            }
        }
    }
    /* update the transport address, if the adjance is the first one */
    if(0 == psess->adjacency_list.count)
    {
        memcpy(&psess->tcp_addr, &padjance->tcp_addr, sizeof(struct ldp_taddr));
        psess->role = (psess->tcp_addr.peer_addr < psess->tcp_addr.local_addr) ? LDP_ROLE_ACTIVE : LDP_ROLE_PASSIVE;
    }

    listnode_add(&psess->adjacency_list, padjance);

    return ERRNO_SUCCESS;
}
/**
* @brief      <+ A adjance leave a LDP session +>
* @param[in ] <+padjance:LDP邻居结构 psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+A LDP session may have many neighbors+>
*/
void ldp_adjance_leave_sess(struct ldp_adjance *padjance, struct ldp_sess *psess, uint32_t event)
{
    uint8_t  adjacency_type = padjance->type;
    uint32_t counter = psess->adjacency_list.count;
    struct ifm_l3   pl3if ;
    int i = 0;

    MPLS_LDP_DEBUG(padjance->peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "psess->type old %d peer lsrid %s psess %p\n", psess->type, ldp_ipv4_to_str(padjance->peer_lsrid), psess);
    counter --;
    if (LDP_SESSION_LOCAL == adjacency_type)
    {
        if (LDP_STATUS_UP == psess->status)
        {
            if (ifm_get_l3if(padjance->ifindex, MODULE_ID_MPLS, &pl3if) == 0)
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "ldp neighbor leave the session now delete the nhp.nexthop = %s.\n", ldp_ipv4_to_str(padjance->peer_ip));
                for (i = 0; i < IFM_IP_NUM; i++)
                {
                    if (pl3if.ipv4[i].addr)
                    {
                        ldp_send_address_withdraw(padjance, pl3if.ipv4[i].addr);
                    }
                }
            }
        }
        if ((0 == counter) || ((1 == counter) && (LDP_SESSION_REMOT & psess->type)))
        {
            psess->type &= (~adjacency_type);
        }
    }
    else if (LDP_SESSION_REMOT == adjacency_type)
    {
        psess->type &= (~adjacency_type);
    }

    listnode_delete(&psess->adjacency_list, padjance);
    padjance->psess = NULL;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_DISCOVER, "psess->type new %d", psess->type);

    if (0 == counter)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, (MPLS_LDP_DEBUG_DISCOVER | MPLS_LDP_DEBUG_FSM | MPLS_LDP_DEBUG_NOTIFY),
                       "ldp delete session status %d when the neighbor count is zero\n", psess->status);

        ldp_session_fsm(event, psess);
    }
}

int ldp_adjance_hello_internal_recalculate(struct list *plist)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct ldp_adjance  *padjance   = NULL;
    uint32_t hello_send = gldp.hello_internal;
    
    if(NULL == plist)
    {
        return hello_send;
    }
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if (hello_send > padjance->neighbor_hold /3)
        {
            hello_send = padjance->neighbor_hold /3;
        }
    }

    return hello_send;

}

