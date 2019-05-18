#include <string.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/route_com.h>
#include <lib/index.h>
#include <lib/zassert.h>
#include "mpls_main.h"
#include "lsp_fec.h"
#include "labelm.h"
#include "mpls.h"
#include "lspm.h"
#include "pw.h"
#include "ldp/ldp_label.h"

extern int lsp_fec_delete_nhp_only(struct lsp_fec_t *plsp_fec, struct lsp_nhp_t *pnhp);

struct ptree *lsp_fec_tree = NULL;


/* nhp 按照优先级排序 */
static int lsp_fec_nhp_cmp(void *val1, void *val2)
{
    struct lsp_nhp_t *pnhp_new = NULL;
    struct lsp_nhp_t *pnhp_old = NULL;

    if ((NULL == val1) || (NULL == val2))
    {
        return 0;
    }

    pnhp_new = (struct lsp_nhp_t *)val1;
    pnhp_old = (struct lsp_nhp_t *)val2;

    if (pnhp_new->distance < pnhp_old->distance)
    {
        return -1;
    }
    else if (pnhp_new->distance == pnhp_old->distance)
    {
        if (pnhp_new->nexthop.addr.ipv4 < pnhp_old->nexthop.addr.ipv4)
        {
            return -1;
        }
    }

    return 0;
}


void lsp_fec_tree_init(unsigned int size)
{
    lsp_fec_tree = ptree_init(size);
}


void lsp_fec_tree_clear(void)
{
    ptree_free(lsp_fec_tree);
}

#if 0
/* 设置 lsp 对应的路由表信息 */
static int lsp_fec_set_route(struct route_entry *proute, struct lsp_fec_t *pfec,
                             struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_FAIL;

    if ((NULL == proute) || (NULL == pfec) || (NULL == pnhp) || (pnhp->ingress_lsp == 0))
    {
        return ret;
    }

    memcpy(&(proute->prefix), &(pfec->prefix), sizeof(struct inet_prefix));
    proute->vpn              = 0;
    proute->nhp_num          = 1;
    proute->nhp[0].vpn       = 0;
    proute->nhp[0].down_flag = LINK_UP;
    proute->nhp[0].active    = 0;
    proute->nhp[0].action    = NHP_ACTION_FORWARD;
    proute->nhp[0].nhp_type  = NHP_TYPE_LSP;
    proute->nhp[0].ifindex   = pnhp->ingress_lsp;
    proute->nhp[0].nexthop.addr.ipv4 = pnhp->nexthop.addr.ipv4;

    if (LSP_TYPE_STATIC == pnhp->type)
    {
        proute->nhp[0].protocol = ROUTE_PROTO_MPLSTP;
        proute->nhp[0].distance = ROUTE_METRIC_MPLSTP;
    }
    else if (LSP_TYPE_LDP == pnhp->type)
    {
        proute->nhp[0].protocol = ROUTE_PROTO_LDP;
        proute->nhp[0].distance = ROUTE_METRIC_LDP;
    }
    else if (LSP_TYPE_RSVPTE == pnhp->type)
    {
        proute->nhp[0].protocol = ROUTE_PROTO_RSVPTE;
        proute->nhp[0].distance = ROUTE_METRIC_RSVPTE;
    }

    return ERRNO_SUCCESS;
}
#endif


struct lsp_fec_t *
lsp_fec_create(struct inet_prefix *pprefix, uint32_t nexthop, uint32_t flag)
{
    struct lsp_fec_t  *pfec = NULL;
    pfec = (struct lsp_fec_t *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct lsp_fec_t));
    if (NULL == pfec)
    {
        MPLS_LOG_ERROR("malloc lsp fec failed !\n");
        return NULL;
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "prefix %s\n", ldp_ipv4_to_str(pprefix->addr.ipv4));
    memset(pfec, 0, sizeof(struct lsp_fec_t));
    pfec->nhplist.cmp = lsp_fec_nhp_cmp;

    pfec->prefix.type = pprefix->type;
    pfec->prefix.addr.ipv4 = pprefix->addr.ipv4;
    pfec->prefix.prefixlen = pprefix->prefixlen;
    pfec->egress_type = flag;

    pfec->inlabel = label_alloc(MODULE_ID_LDP); /* alloc inlabel */
    pfec->nexthop.type = INET_FAMILY_IPV4;
    pfec->nexthop.addr.ipv4 = nexthop;

    lsp_fec_add(pfec);
    return pfec;
}
/* mpls-tp、rsvp-te、ldp 分配入标签之后生成 fec 和 egress lsp */
struct lsp_fec_t *lsp_fec_add(struct lsp_fec_t *pfec)
{
    struct ptree_node *pnode    = NULL;
    struct lsp_entry *plspm = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if ((NULL == pfec) || (pfec->inlabel == 0))
    {
        return NULL;
    }

    pnode = ptree_node_get(lsp_fec_tree, (u_char *) & (pfec->prefix.addr.ipv4), pfec->prefix.prefixlen);
    if (NULL == pnode)
    {
        MPLS_LOG_ERROR("get fec tree node failed !\n");
        return NULL;
    }

    if (NULL == pnode->info)
    {
        pnode->info = (void *)pfec;

        if (LDP_EGRESS_NONE == pfec->egress_type) //非本地路由无需添加EGRESS LSP
        {
            MPLS_LOG_ERROR("learing fec no need to add egress lsp\n");
            return pfec;
        }
        /* 添加 egress lsp */
        plspm = mpls_lsp_new(pfec->inlabel, 0, LSP_TYPE_LDP);
        if (NULL == plspm)
        {
            return pfec;
        }
        pfec->egress_lsp = plspm->lsp_index;/* fec save the lsp_index */
        memcpy(&(plspm->destip), &(pfec->prefix), sizeof(struct inet_prefix));
        memcpy(&(plspm->nexthop), &(pfec->nexthop), sizeof(struct inet_addr));
        plspm->nhp_type = NHP_TYPE_CONNECT;
        plspm->nhp_index = 0;
        plspm->down_flag = LINK_UP;
        mpls_lsp_add(plspm);
    }
    else
    {
        MPLS_LOG_ERROR("add fec in lsp fec tree failed for get pnode info != NULL !\n");
    }

    return pfec;
}


/* mpls-tp、rsvp-te、ldp 删除入标签之后删除 fec 和 egress lsp */
int lsp_fec_delete(struct lsp_fec_t *pfec)
{
    struct ptree_node *pnode       = NULL;
    struct lsp_fec_t  *pfec_lookup = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "fec prefix %s\n", ldp_ipv4_to_str(pfec->prefix.addr.ipv4));

    if (NULL == pfec)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    pnode = ptree_node_lookup(lsp_fec_tree, (u_char *) & (pfec->prefix.addr.ipv4),
                              pfec->prefix.prefixlen);
    if ((pnode == NULL) || (NULL == pnode->info))
    {
        MPLS_LDP_ERROR("error lsp fec delete for find pnode info NULL\n");
        return ERRNO_SUCCESS;
    }

    pfec_lookup = (struct lsp_fec_t *)pnode->info;

    /* 先删除 nhplist */
    if (pfec_lookup->nhplist.count)
    {
        lsp_fec_clear_nhp(pfec_lookup);
    }

    /* 最后删 fec */
    if (pfec_lookup->egress_lsp)
    {
        mpls_lsp_delete(pfec_lookup->egress_lsp);
        mpls_lsp_free_index(pfec_lookup->egress_lsp);
    }
    if (pfec_lookup->inlabel)
    {
        label_free(pfec_lookup->inlabel);
    }
    pnode->info = NULL;
    pnode->lock = 0;
    ptree_node_delete(pnode);

    XFREE(MTYPE_LSP_ENTRY, pfec_lookup);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "finished\n");
    return ERRNO_SUCCESS;
}


/* 根据 destination 查找 fec */
struct lsp_fec_t *lsp_fec_lookup(struct inet_prefix *pprefix)
{
    struct ptree_node *pnode = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if (NULL == pprefix)
    {
        return NULL;
    }
    if((pprefix->prefixlen < 0) || (pprefix->prefixlen > 32))
    {
        MPLS_LDP_ERROR("prefixlen %d\n", pprefix->prefixlen);
        return NULL;
    }

    pnode = ptree_node_lookup(lsp_fec_tree, (u_char *) & (pprefix->addr.ipv4),
                              pprefix->prefixlen);
    if (NULL == pnode)
    {
        return NULL;
    }
    if (NULL == pnode->info)
    {
        return NULL;
    }

    return (struct lsp_fec_t *)(pnode->info);
}


/* 根据 destination 查找最长匹配的 fec */
struct lsp_fec_t *lsp_fec_match(struct inet_prefix *pprefix)
{
    struct ptree_node *pnode = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if (NULL == pprefix)
    {
        return NULL;
    }

    pnode = ptree_node_match1(lsp_fec_tree, (u_char *) & (pprefix->addr.ipv4),
                              pprefix->prefixlen);
    if (NULL == pnode)
    {
        return NULL;
    }

    return (struct lsp_fec_t *)(pnode->info);
}


/* 从fec nhplist 中查找 nhp */
struct lsp_nhp_t *lsp_fec_lookup_nhp(struct list *nhplist, struct lsp_nhp_t *pnhp)
{
    struct lsp_nhp_t *pnhp_lookup = NULL;
    struct listnode  *pnode       = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if ((NULL == nhplist) || (NULL == pnhp))
    {
        return NULL;
    }

    for (ALL_LIST_ELEMENTS_RO(nhplist, pnode, pnhp_lookup))
    {
        if ((pnhp_lookup->type == pnhp->type) && (pnhp_lookup->nexthop.type == pnhp->nexthop.type)
                && (pnhp_lookup->nexthop.addr.ipv4 == pnhp->nexthop.addr.ipv4))
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "find nhp successful\n");
            return pnhp_lookup;
        }
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "find nhp failed\n");
    return NULL;
}


/* 收到下一跳分配的出标签，向 fec nhplist 添加 nhp 和 lsp */
int lsp_fec_add_nhp(struct lsp_fec_t *plsp_fec, struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_SUCCESS;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    /* ldp send maping broadcast when the first nhp added */
    if(0 == plsp_fec->nhplist.count)
    {
        /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT */
        ldp_fec_maping_broadcast(plsp_fec);
    }

    ret = lsp_fec_add_frr_nhp(plsp_fec, pnhp);
    if(ERRNO_SUCCESS != ret)
    {
        lsp_fec_add_com_nhp(plsp_fec, pnhp);
    }

    return ERRNO_SUCCESS;
}


int lsp_fec_delete_nhp_only(struct lsp_fec_t *plsp_fec, struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_SUCCESS;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    ret = lsp_fec_del_frr_nhp(plsp_fec, pnhp);
    if(ERRNO_SUCCESS != ret)
    {
        lsp_fec_del_com_nhp(plsp_fec, pnhp);
    }

    if ((list_isempty(&plsp_fec->nhplist)) && (LDP_EGRESS_NONE == plsp_fec->egress_type))
    {
        /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT ldp send lable withdraw mesg to the upstream lsrs */
        ldp_fec_withdraw_broadcast(plsp_fec);
    }

    return ERRNO_SUCCESS;
}


/* 下一跳回收出标签时，从fec nhplist 中删除 nhp 和 lsp */
int lsp_fec_delete_nhp(struct lsp_fec_t *plsp_fec, struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_SUCCESS;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    ret = lsp_fec_del_frr_nhp(plsp_fec, pnhp);
    if(ERRNO_SUCCESS != ret)
    {
        lsp_fec_del_com_nhp(plsp_fec, pnhp);
    }

    if ((list_isempty(&plsp_fec->nhplist)) && (LDP_EGRESS_NONE == plsp_fec->egress_type))
    {
        /* MPLS_LDP_FEC_BROADCAST_MANAGEMENT ldp send lable withdraw mesg to the upstream lsrs */
        ldp_fec_withdraw_broadcast(plsp_fec);
        lsp_fec_delete(plsp_fec);
    }

    return ERRNO_SUCCESS;
}


/* 删除 nhplist */
int lsp_fec_clear_nhp(struct lsp_fec_t *pfec)
{
    struct lsp_nhp_t *pnhp_lookup = NULL;
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if (NULL == pfec)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    for (ALL_LIST_ELEMENTS(&pfec->nhplist, pnode, pnodenext, pnhp_lookup))
    {
        if (pnhp_lookup)
        {
            lsp_fec_delete_nhp_only(pfec, pnhp_lookup);

            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "nhp delete successful\n");
        }
    }
    return ERRNO_SUCCESS;
}




/* 收到下一跳分配的出标签，向 fec nhplist 添加 nhp 和 lsp */
int lsp_fec_add_com_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp)
{
    struct lsp_nhp_t *pnhp_new = NULL;
    struct lsp_entry *plspm = NULL;

    if ((NULL == pfec) || (NULL == pnhp) || (pnhp->outlabel == 0))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "ldp add nhp outlabel %d\n", pnhp->outlabel);
    /* 添加 nhp */
    pnhp_new = (struct lsp_nhp_t *)XCALLOC(MTYPE_NHP_ENTRY, sizeof(struct lsp_nhp_t));
    if (NULL == pnhp_new)
    {
        return ERRNO_MALLOC;
    }
    memcpy(pnhp_new, pnhp, sizeof(struct lsp_nhp_t));
    listnode_add_sort(&pfec->nhplist, pnhp_new);

    /* 添加 ingress lsp */
    plspm = mpls_lsp_new(0, pnhp->outlabel, LSP_TYPE_LDP);
    if (NULL == plspm)
    {
        return ERRNO_MALLOC;
    }
    pnhp_new->ingress_lsp = plspm->lsp_index;/* fec save the lsp_index */
    memcpy(&(plspm->destip), &(pfec->prefix), sizeof(struct inet_prefix));
    memcpy(&(plspm->nexthop), &(pnhp->nexthop), sizeof(struct inet_addr));
    plspm->nhp_type = NHP_TYPE_CONNECT;
    plspm->nhp_index = 0;
    plspm->down_flag = LINK_UP;
    mpls_lsp_add(plspm);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "lsp_index %d\n", plspm->lsp_index);

    /* 添加 transit lsp */
    plspm = mpls_lsp_new(pfec->inlabel, pnhp->outlabel, LSP_TYPE_LDP);
    if (NULL == plspm)
    {
        return ERRNO_MALLOC;
    }
    pnhp_new->transit_lsp = plspm->lsp_index;/* fec save the lsp_index */
    memcpy(&(plspm->destip), &(pfec->prefix), sizeof(struct inet_prefix));
    memcpy(&(plspm->nexthop), &(pnhp->nexthop), sizeof(struct inet_addr));
    plspm->nhp_type = NHP_TYPE_CONNECT;
    plspm->nhp_index = 0;
    plspm->down_flag = LINK_UP;
    mpls_lsp_add(plspm);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "lsp_index %d\n", plspm->lsp_index);

    /* 通知 pw 处理 */
    l2vc_process_tunnel_ldp_lsp_add();
    mpls_ldp_fec_tree_show();
    
    return ERRNO_SUCCESS;
}



/* 下一跳回收出标签时，从fec nhplist 中删除 nhp 和 lsp */
int lsp_fec_del_com_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp)
{
    struct lsp_nhp_t *pnhp_lookup = NULL;
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");

    if ((NULL == pfec) || (NULL == pnhp))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "outlabel %d\n", pnhp->outlabel);

    for (ALL_LIST_ELEMENTS(&pfec->nhplist, pnode, pnodenext, pnhp_lookup))
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "pnhp_lookup->type %d pnhp_lookup->nexthop %s", pnhp_lookup->type, ldp_ipv4_to_str(pnhp_lookup->nexthop.addr.ipv4));
        if ((pnhp_lookup->type == pnhp->type)
                && (memcmp(&(pnhp_lookup->nexthop), &(pnhp->nexthop), sizeof(struct inet_addr)) == 0))
        {
            list_delete_node(&pfec->nhplist, pnode);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, " ingress_lsp %d\n", pnhp_lookup->ingress_lsp);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, " transit_lsp %d\n", pnhp_lookup->transit_lsp);
            /* ingress lsp 通知路由更新 */
            if (pnhp_lookup->ingress_lsp)
            {
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
                /* 通知 pw 删除该隧道信息 */
                l2vc_process_tunnel_ldp_lsp_del(pnhp_lookup->ingress_lsp);

                /* 删除 ingress lsp  */
                mpls_lsp_delete(pnhp_lookup->ingress_lsp);
                mpls_lsp_free_index(pnhp_lookup->ingress_lsp);
            }

            if (pnhp_lookup->transit_lsp)
            {
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "\n");
                mpls_lsp_delete(pnhp_lookup->transit_lsp);
                mpls_lsp_free_index(pnhp_lookup->transit_lsp);
            }

            XFREE(MTYPE_NHP_ENTRY, pnhp_lookup);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "nhp delete successful\n");
            break;
        }
    }
    mpls_ldp_fec_tree_show();
    return ERRNO_SUCCESS;
}

/* added for ldp frr function */


int lsp_fec_send_frr_ftm(struct ldp_frr_t *ldp_frr, int opcode)
{
    int ret = ERRNO_SUCCESS;
    
    ret = MPLS_IPC_SENDTO_FTM(ldp_frr, sizeof(struct ldp_frr_t), 1, MODULE_ID_MPLS, IPC_TYPE_LDP, opcode, 0, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "frr send to ftm failed\n");
    }
    else
    {
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "frr send to ftm successsful\n");
    }
    return ret;
}

int lsp_fec_add_frr_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_SUCCESS;
    //struct ldp_frr_t ldp_frr = {0};
    int i , lsp_master= 0;
    struct route_entry * proute     = NULL;
    struct lsp_nhp_t *pnhp_new      = NULL;
    struct lsp_nhp_t *pnhp_old    = NULL;
    
    struct lsp_entry *plsp_transmit = NULL;
    struct lsp_entry *plsp_del  = NULL;
    struct lsp_entry *plsp_frr1 = NULL;
    struct lsp_entry *plsp_frr2 = NULL;
    struct lsp_entry *pfrr_master = NULL;

    struct lsp_nhp_t *pnhp_lookup = NULL;
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;

    if(0 == pfec->nhplist.count)
    {
        ret = ERRNO_FAIL;
    }
    else
    {
        proute = route_com_get_route(&pfec->prefix, 0 , MODULE_ID_MPLS);
        if(NULL == proute)
        {
            ret = ERRNO_FAIL;
        }
        else if(NHP_TYPE_FRR != proute->nhpinfo.nhp_type)
        {
            mem_share_free_bydata(proute, MODULE_ID_MPLS);
            ret = ERRNO_FAIL;
        }
        else
        {
            /* check if a ldp frr generate */
            for (i = 0; i < proute->nhp_num; i++)
            {
                if(proute->nhp[i].nexthop_connect.addr.ipv4 == pnhp->nexthop.addr.ipv4)
                {
                    if(0 == i)
                    {
                        lsp_master = 1;/* default the first route to be the master in the frr group */
                        pnhp->frr_master = 1;
                    }
                    pnhp_new = pnhp;
                    continue;
                }
                for (ALL_LIST_ELEMENTS(&pfec->nhplist, pnode, pnodenext, pnhp_lookup))
                {
                    if (pnhp_lookup)
                    {
                        if(proute->nhp[i].nexthop_connect.addr.ipv4 == pnhp_lookup->nexthop.addr.ipv4)
                        {

                            pnhp_old = pnhp_lookup;
                            if(0 == i)
                            {
                                lsp_master = 0;
                                pnhp_old->frr_master = 1;
                            }

                            continue;
                        }
                    }
                }
            }

            if((NULL == pnhp_new) || (NULL == pnhp_old))
            {
                ret = ERRNO_FAIL;
            }
            else
            {
                MPLS_LOG_DEBUG("ldp frr gernerate\n");
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "ldp frr gernerate\n");
                /* add new nhp to the fec nhplist */
                pnhp_new = NULL;
                pnhp_new = (struct lsp_nhp_t *)XCALLOC(MTYPE_NHP_ENTRY, sizeof(struct lsp_nhp_t));
                if (NULL == pnhp_new)
                {
                    ret = ERRNO_FAIL;
                    goto out;
                }
                memcpy(pnhp_new, pnhp, sizeof(struct lsp_nhp_t));
                listnode_add_sort(&pfec->nhplist, pnhp_new);

                /* add transit lsp for new nhp */
                plsp_transmit = mpls_lsp_new(pfec->inlabel, pnhp_new->outlabel, LSP_TYPE_LDP);
                if (NULL == plsp_transmit)
                {
                    ret = ERRNO_FAIL;
                    goto out;
                }

                /* frr masger ingress lsp */
                plsp_frr1 = mpls_lsp_new(0, pnhp_new->outlabel, LSP_TYPE_LDP);
                if(NULL == plsp_frr1)
                {
                    mpls_lsp_delete(plsp_transmit->lsp_index);
                    mpls_lsp_free_index(plsp_transmit->lsp_index);
                    ret = ERRNO_FAIL;
                    goto out;
                }

                /* frr backup ingress lsp */
                plsp_frr2 = mpls_lsp_new(0, pnhp_old->outlabel, LSP_TYPE_LDP);
                if((NULL == plsp_frr2)  || (NULL == plsp_frr1) || (NULL == plsp_transmit))
                {
                    mpls_lsp_delete(plsp_transmit->lsp_index);
                    mpls_lsp_free_index(plsp_transmit->lsp_index);
                    mpls_lsp_delete(plsp_frr1->lsp_index);
                    mpls_lsp_free_index(plsp_frr1->lsp_index);
                    ret = ERRNO_FAIL;
                    goto out;
                }

                pnhp_new->transit_lsp = plsp_transmit->lsp_index;/* fec save the lsp_index */
                memcpy(&(plsp_transmit->destip), &(pfec->prefix), sizeof(struct inet_prefix));
                memcpy(&(plsp_transmit->nexthop), &(pnhp_new->nexthop), sizeof(struct inet_addr));
                plsp_transmit->nhp_type = NHP_TYPE_CONNECT;
                plsp_transmit->nhp_index = 0;
                plsp_transmit->down_flag = LINK_UP;
                mpls_lsp_add(plsp_transmit);

                /* nhp to record ldp frr by each other */
                pnhp_new->pnhp_frr = pnhp_old;
                pnhp_old->pnhp_frr = pnhp_new;
                pnhp_new->frr_index = index_alloc(INDEX_TYPE_LDP_FRR) + 1000;/* 获取FRR INDEX*/
                pnhp_old->frr_index = pnhp_new->frr_index;
                
                /* ldp frr group : pnhp_added:pnhp_new   master: ? */
                plsp_del = mpls_lsp_lookup(pnhp_old->ingress_lsp);
                pnhp_old->ingress_lsp = 0;/* pw will not find the lsp that will be deleted */

                memcpy(&(plsp_frr1->destip), &(pfec->prefix), sizeof(struct inet_prefix));
                memcpy(&(plsp_frr1->nexthop), &(pnhp_new->nexthop), sizeof(struct inet_addr));
                plsp_frr1->nhp_type = NHP_TYPE_CONNECT;
                plsp_frr1->nhp_index = 0;
                plsp_frr1->down_flag = LINK_UP;

                memcpy(&(plsp_frr2->destip), &(pfec->prefix), sizeof(struct inet_prefix));
                memcpy(&(plsp_frr2->nexthop), &(pnhp_old->nexthop), sizeof(struct inet_addr));
                plsp_frr2->nhp_type = NHP_TYPE_CONNECT;
                plsp_frr2->nhp_index = 0;
                plsp_frr2->down_flag = LINK_UP;

                pfrr_master = (lsp_master) ? plsp_frr1 : plsp_frr2;

                /* add the frr master lsp to ensure the pw can select the right lsp */
                mpls_lsp_add((lsp_master) ? plsp_frr1 : plsp_frr2);

                /* add the frr backup lsp to ensure the pw can select the right lsp */
                mpls_lsp_add((lsp_master) ? plsp_frr2 : plsp_frr1);


                pnhp_new->ingress_lsp = plsp_frr1->lsp_index;
                pnhp_old->ingress_lsp = plsp_frr2->lsp_index;
                MPLS_LOG_DEBUG("ldp frr gernerate add pnhp_new->ingress_lsp %x\n", pnhp_new->ingress_lsp);
                MPLS_LOG_DEBUG("ldp frr gernerate add pnhp_old->ingress_lsp %x\n", pnhp_old->ingress_lsp);

                l2vc_process_tunnel_ldp_lsp_frr(plsp_del->lsp_index, pfrr_master->lsp_index, 0);

                /* send ldp frr to hal */
                //ldp_frr.group_index = pnhp_old->frr_index;
                //ldp_frr.master_lsp = (lsp_master) ? plsp_frr1->lsp_index : plsp_frr2->lsp_index;
                //ldp_frr.backup_lsp = (lsp_master) ? plsp_frr2->lsp_index : plsp_frr1->lsp_index;
                //lsp_fec_send_frr_ftm(&ldp_frr, IPC_OPCODE_ADD);

                /* pws used old lsp advertise hal to update lsp to ldp frr */
                
                //l2vc_process_tunnel_ldp_lsp_frr(plsp_del->lsp_index, pfrr_master->lsp_index, pnhp_new->frr_index);
                MPLS_LOG_DEBUG("ldp frr gernerate delete lspindex %x\n", plsp_del->lsp_index);
                /* lsp delete plsp_del here */
                mpls_lsp_delete(plsp_del->lsp_index);
                
            }
            mem_share_free_bydata(proute, MODULE_ID_MPLS);
        }
    }
    MPLS_LOG_DEBUG("ldp frr gernerate finished\n");
out:
    return ret;
}

int lsp_fec_del_frr_nhp(struct lsp_fec_t *pfec, struct lsp_nhp_t *pnhp)
{
    int ret = ERRNO_SUCCESS;
    uint32_t lsp_del1 = 0;
    uint32_t lsp_del2 = 0;
    uint32_t lsp_del3 = 0;
    struct lsp_entry *plsp_add = NULL;
    struct ldp_frr_t ldp_frr = {0};
    uint32_t lsp_used = 0;

    struct lsp_nhp_t *pnhp_del = lsp_fec_lookup_nhp(&pfec->nhplist, pnhp);
    struct lsp_nhp_t *pnhp_res = (struct lsp_nhp_t *)pnhp_del->pnhp_frr;

    if((NULL == pnhp_del) || (NULL == pnhp_del->pnhp_frr) || (0 == pnhp_del->frr_index))
    {
        ret = ERRNO_FAIL;
    }
    else
    {
        if(pnhp_del->frr_master)
        {
            lsp_used = pnhp_del->ingress_lsp;
        }
        else
        {
            lsp_used = pnhp_res->ingress_lsp;
        }
        
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_LABEL, "ldp frr disslove\n");
        /* 解除LDP FRR关系 */
        pnhp_del->pnhp_frr = NULL;
        pnhp_res->pnhp_frr = NULL;
        pnhp_res->frr_index = 0;
        
        /* to ensure deleted lsp will not be select by the pw */
        /* record the lsp index will be delete */
        lsp_del1 = pnhp_res->ingress_lsp;
        lsp_del2 = pnhp_del->ingress_lsp;
        lsp_del3 = pnhp_del->transit_lsp;
        /* clear the lsp index in nhp */
        pnhp_res->ingress_lsp = 0;
        pnhp_del->ingress_lsp = 0;
        pnhp_del->transit_lsp = 0;

        plsp_add = mpls_lsp_new(0, pnhp_res->outlabel, LSP_TYPE_LDP);
        if(NULL == plsp_add)
        {
            ret = ERRNO_FAIL;
            goto out;
        }
        memcpy(&(plsp_add->destip), &(pfec->prefix), sizeof(struct inet_prefix));
        memcpy(&(plsp_add->nexthop), &(pnhp_res->nexthop), sizeof(struct inet_addr));
        plsp_add->nhp_type = NHP_TYPE_CONNECT;
        plsp_add->nhp_index = 0;
        plsp_add->down_flag = LINK_UP;
        pnhp_res->ingress_lsp = plsp_add->lsp_index;
        mpls_lsp_add(plsp_add);

        /* pw advertise hal to update lsp to plsp_add */
        l2vc_process_tunnel_ldp_lsp_frr(lsp_used, plsp_add->lsp_index, 0);
        
        /* ldp frr advertise ftm to delete frr group all */
        ldp_frr.group_index = pnhp_del->frr_index;
        //lsp_fec_send_frr_ftm(&ldp_frr, IPC_OPCODE_DELETE);

        /* recover the lsp index in the nhp and delete nhp */
        pnhp_del->ingress_lsp = lsp_del2;
        pnhp_del->transit_lsp = lsp_del3;
        lsp_fec_delete_nhp(pfec, pnhp_del);
        
        mpls_lsp_delete(lsp_del1);
        mpls_lsp_free_index(lsp_del1);

        index_free(INDEX_TYPE_LDP_FRR, (ldp_frr.group_index - 1000));
    }
out:
    return ret;
}



