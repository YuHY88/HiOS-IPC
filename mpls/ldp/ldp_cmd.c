/*
*   define of ldp ÃüÁîÐÐÊµÏÖ
*/
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/ifm_common.h>
#include <lib/mpls_common.h>
#include <mpls/lspm.h>
#include <mpls/labelm.h>

#include "mpls_main.h"
#include "mpls_if.h"
#include <mpls/mpls.h>
#include "mpls/lsp_fec.h"
#include <mpls/bfd/bfd.h>

#include "ldp.h"
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_adjance.h"
#include "ldp_pw.h"
#include "ldp_fec.h"
#include "ldp_fsm.h"
#include "ldp_lsptrigger.h"
extern int mpls_if_ldp_enable(struct mpls_if *pif);
extern int mpls_if_ldp_disable(struct mpls_if *pif);

extern void mpls_ldp_remot_config_write(struct vty *vty);
extern void ldp_if_hello_internal_update(uint32_t hello_internal);
extern void mpls_ldp_fec_lsp_show(struct vty *vty);

#define LDP_LSP_NHP_TYPE_ECMP       1
#define LDP_LSP_NHP_TYPE_FRR        2
#define LDP_LSP_NHP_TYPE_ONE        0

static void mpls_ldp_lsp_show(struct vty *vty, uint32_t lsp_index, uint32_t flag)
{
    struct lsp_entry  *plsp    = NULL;
    char fec_buf[INET_ADDRSTRLEN + 5]  = "";
    char destip[INET_ADDRSTRLEN]     = "";
    char nexthop[INET_ADDRSTRLEN]    = "";
    char label_buf[16]               = "";
    char flag_str[INET_ADDRSTRLEN]    = "";

    plsp = mpls_lsp_lookup(lsp_index);
    if ((plsp == NULL) || (LSP_TYPE_LDP != plsp->lsp_type))
    {
        return;
    }
    inet_ipv4tostr(plsp->nexthop.addr.ipv4, nexthop);
    /* 如果是ECMP 或者FRR的备不在显示地址前缀*/
    if(LDP_LSP_NHP_TYPE_ONE == flag)
    {
        inet_ipv4tostr(plsp->destip.addr.ipv4, destip);
        sprintf(fec_buf, "%s/%d", destip, plsp->destip.prefixlen);
        
        sprintf(flag_str, "%s", " ");
    }
    else if(LDP_LSP_NHP_TYPE_FRR == flag)
    {
        sprintf(fec_buf, "    ");
        sprintf(flag_str, "%s", "frr ");
    }
    else if(LDP_LSP_NHP_TYPE_ECMP == flag)
    {
        sprintf(fec_buf, "    ");
        sprintf(flag_str, "%s", "ecmp");
    }
    
    if (plsp->inlabel)
    {
        if (plsp->outlabel)
        {
            sprintf(label_buf, "%u/%u", plsp->inlabel, plsp->outlabel);
        }
        else
        {
            sprintf(label_buf, "%u/%s", plsp->inlabel, "NULL");
        }
    }
    else
    {
        if (plsp->outlabel)
        {
            sprintf(label_buf, "%s/%u", "NULL", plsp->outlabel);
        }
        else
        {
            sprintf(label_buf, "%s/%s", "NULL", "NULL");
        }
    }
    vty_out(vty, "%-20s%-16s", fec_buf, label_buf);
    vty_out(vty, "%-16s", plsp->direction == LSP_DIRECTION_INGRESS ? "ingress" : plsp->direction == LSP_DIRECTION_TRANSIT ? "transit" : "egress");
    vty_out(vty, "%-5s%-20s%s", flag_str, plsp->nexthop.addr.ipv4 == 0 ? "-" : nexthop, VTY_NEWLINE);


}


void mpls_ldp_fec_lsp_show(struct vty *vty)
{
    struct ptree_node   *ptreenode_lookup  = NULL;
    struct ptree_node   *ptreenode = NULL;
    struct listnode     *pnode      = NULL;
    struct listnode     *pnodenext  = NULL;
    struct lsp_fec_t    *pfec       = NULL;
    struct lsp_nhp_t    *pnhp       = NULL;
    struct lsp_nhp_t    *pnhp_master = NULL;
    struct lsp_nhp_t    *pnhp_backup = NULL;
    int i = 0;

    vty_out(vty, "%-20s%-16s%-16s%-5s%-20s %s", "Fec", "In/Out", "Direction","Flag", "Next-hop", VTY_NEWLINE);
    vty_out(vty, "------------------------------------------------------------%s", VTY_NEWLINE);

    for (ptreenode = ptree_top(lsp_fec_tree); (NULL != ptreenode);)
    {
        i = 0;
        ptreenode_lookup = ptreenode;
        ptreenode = ptree_next(ptreenode);
        if(NULL == ptreenode_lookup->info)
        {
            continue;
        }
        pfec = ptreenode_lookup->info;
        
        if(0 != pfec->egress_lsp)
        {
            mpls_ldp_lsp_show(vty, pfec->egress_lsp, LDP_LSP_NHP_TYPE_ONE);
        }
        for (ALL_LIST_ELEMENTS(&pfec->nhplist, pnode, pnodenext, pnhp))
        {
            if(NULL == pnhp)
            {
                continue;
            }
            /* 判断是ECMP 或者FRR */
            if(pfec->nhplist.count == NHP_ECMP_NUM)
            {
                if(pnhp->frr_index)
                {
                    if((pnhp->frr_master)&& (NULL != pnhp->pnhp_frr))
                    {
                        pnhp_master = pnhp;
                        pnhp_backup = pnhp->pnhp_frr;
                        mpls_ldp_lsp_show(vty, pnhp_master->ingress_lsp, LDP_LSP_NHP_TYPE_ONE);
                        mpls_ldp_lsp_show(vty, pnhp_backup->ingress_lsp, LDP_LSP_NHP_TYPE_FRR);
                        
                        mpls_ldp_lsp_show(vty, pnhp_master->transit_lsp, LDP_LSP_NHP_TYPE_ONE);
                        mpls_ldp_lsp_show(vty, pnhp_backup->transit_lsp, LDP_LSP_NHP_TYPE_FRR);
                        pnhp_master = NULL;
                        pnhp_backup = NULL;
                    }
                }
                else
                {
                    if(i == 0)
                    {
                        i++;
                        pnhp_master = pnhp;
                    }
                    else
                    {
                        pnhp_backup = pnhp;
                    }

                    if(pnhp_master && pnhp_backup)
                    {
                        mpls_ldp_lsp_show(vty, pnhp_master->ingress_lsp, LDP_LSP_NHP_TYPE_ONE);
                        mpls_ldp_lsp_show(vty, pnhp_backup->ingress_lsp, LDP_LSP_NHP_TYPE_ECMP);
                        
                        mpls_ldp_lsp_show(vty, pnhp_master->transit_lsp, LDP_LSP_NHP_TYPE_ONE);
                        mpls_ldp_lsp_show(vty, pnhp_backup->transit_lsp, LDP_LSP_NHP_TYPE_ECMP);
                        pnhp_master = NULL;
                        pnhp_backup = NULL;
                    }
                }
            }
            else
            {
                mpls_ldp_lsp_show(vty, pnhp->ingress_lsp, LDP_LSP_NHP_TYPE_ONE);
                mpls_ldp_lsp_show(vty, pnhp->transit_lsp, LDP_LSP_NHP_TYPE_ONE);
            }
        }
    }
}



void mpls_ldp_fec_tree_show(void)
{
    struct ptree_node  *ptreenode   = NULL;
    struct ptree_node  *ptreenode2   = NULL;
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct lsp_fec_t *pfec = NULL;
    struct lsp_nhp_t *pnhp = NULL;

    for (ptreenode2 = ptree_top(lsp_fec_tree); (NULL != ptreenode2);)
    {
        ptreenode = ptreenode2;
        ptreenode2 = ptree_next(ptreenode2);
        if(NULL == ptreenode->info)
        {
            continue;
        }
        pfec = ptreenode->info;
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_FSM, "FEC prefix: %s", ldp_ipv4_to_str(pfec->prefix.addr.ipv4));
        for (ALL_LIST_ELEMENTS(&pfec->nhplist, pnode, pnodenext, pnhp))
        {
            if (pnhp)
            {
                MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_FSM, "  outlabel : %d  D-stream : %s  next-hop : %s\n", pnhp->outlabel, ldp_ipv4_to_str(pnhp->peer_lsrid), ldp_ipv4_to_str2(pnhp->nexthop.addr.ipv4));
            }
        }
    }
}

DEFUN(ldp_enable,
      ldp_enable_cmd,
      "mpls ldp enable",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Enable interface local ldp function\n"
     )
{
    uint8_t pmode = 0;
    struct mpls_if  *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ifindex = %x.\n", ifindex);

    
    if ((ifm_get_mode(ifindex, MODULE_ID_MPLS, &pmode)) || (pmode != IFNET_MODE_L3))
    {
        vty_error_out(vty, "please select mode l3 pmode %d.%s", pmode, VTY_NEWLINE);
        return CMD_WARNING;
    }

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    pif = mpls_if_get(ifindex);
    if (pif == NULL)
    {
        return CMD_WARNING;
    }
    if (pif->ldp_conf.ldp_enable == ENABLE)
    {
        return CMD_SUCCESS;
    }
    if (ERRNO_SUCCESS != mpls_if_ldp_enable(pif))
    {
        return CMD_SUCCESS;
    }
    return CMD_SUCCESS;
}

DEFUN(no_ldp_enable,
      no_ldp_enable_cmd,
      "no mpls ldp enable",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Enable interface local ldp function\n"
     )
{
    struct mpls_if *pif = NULL;
    uint32_t ifindex = (uint32_t)vty->index;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ifindex = %x.\n", ifindex);
    pif = mpls_if_lookup(ifindex);
    if ((pif == NULL) || (pif->ldp_conf.ldp_enable == DISABLE))
    {
        return CMD_WARNING;
    }
    if (ERRNO_SUCCESS != mpls_if_ldp_disable(pif))
    {
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

#if 0
DEFUN(show_mpls_ldp_interface,
   show_mpls_ldp_interface_cmd,
   "show mpls ldp interface",
   "show\n"
   "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
   "Label Distribution Protocol(LDP)\n"
   "Interface that enabled local ldp function\n"
  )
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct mpls_if     *pif     = NULL;
    int cursor  = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
    {
        pif = (struct mpls_if *)pbucket->data;
        if ((pif == NULL) || (ENABLE != pif->ldp_conf.ldp_enable))
        {
            continue;
        }

    }


 return CMD_SUCCESS;
}
#endif

/**
* @brief      <++>
* @param[in ] <++>
* @param[out] <++>
* @return     <++>
* @author     wus
* @date       2018/3/1
* @note       <++>
*/
void mpls_ldp_remot_config_write(struct vty *vty)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct ldp_adjance *padjance   = NULL;
    struct list         *plist      = &ldp_remot_adjance_list;
    char dip[INET_ADDRSTRLEN]       = "";

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if (padjance->type == LDP_SESSION_REMOT)
        {
            inet_ipv4tostr(padjance->peer_lsrid, dip);
            vty_out(vty, " mpls ldp remote-peer %s%s", dip, VTY_NEWLINE);
        }
    }
}
DEFUN(mpls_ldp_remte_peer_enable,
      mpls_ldp_remote_peer_cmd,
      "mpls ldp remote-peer A.B.C.D",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify remote peer\n"
      "Peer lsrid format: A.B.C.D\n"
     )
{
    uint32_t peer_lsrid = inet_strtoipv4((char *)argv[0]);

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    if((inet_valid_network(peer_lsrid)) && (0 != peer_lsrid))
    {
        mpls_ldp_remot_enable(peer_lsrid);
    }
    else
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(no_mpls_ldp_remte_peer_enable,
      no_mpls_ldp_remote_peer_cmd,
      "no mpls ldp remote-peer A.B.C.D",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify remote peer\n"
      "Peer lsrid format: A.B.C.D\n"
     )
{
    uint32_t peer_lsrid = inet_strtoipv4((char *)argv[0]);

    if(inet_valid_network(peer_lsrid))
    {
        mpls_ldp_remot_disable(peer_lsrid);
    }
    else
    {
        vty_error_out(vty, "The specified IP address is invalid.%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(show_mpls_ldp_prefix,
      ldp_show_ip_prefix_cmd,
      "show mpls ldp ip-prefix",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Ip prefix, format: A.B.C.D/M information\n"
     )
{
    struct ldp_filter   *pfilter = NULL;
    struct hash_bucket  *pbucket = NULL;
    char buf[INET_ADDRSTRLEN + 5]  = "";
    char dip[INET_ADDRSTRLEN]    = "";
    int cursor = 0;
    int linenum = 1;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    vty_out(vty, "%-5s%-20s%s", "SN", "Prefix-list:", VTY_NEWLINE);
    vty_out(vty, "%s%s", "----------------------", VTY_NEWLINE);
    HASH_BUCKET_LOOP(pbucket, cursor, ldp_filter_tabel)
    {
        pfilter = (struct ldp_filter *)pbucket->data;
        if (pfilter)
        {
            inet_ipv4tostr(pfilter->prefix.addr.ipv4, dip);
            sprintf(buf, "%s/%d", dip, pfilter->prefix.prefixlen);
            vty_out(vty, "%-5d%-20s%s", linenum++, buf, VTY_NEWLINE);
        }
    }
    return CMD_SUCCESS;
}

DEFUN(mpls_ldp_prefix,
      mpls_ldp_prefix_cmd,
      "mpls ldp ip-prefix A.B.C.D/M",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Ip prefix that need to create lsp\n"
      "Ip prefix, format: A.B.C.D for IPv4\n"
     )
{
    struct inet_prefix prefix;
    struct prefix_ipv4 destip;
    struct ldp_filter *pfilter = NULL;
    uint32_t hipv4 = 0;

    if (0 == str2prefix_ipv4(argv[0], &destip))
    {
        vty_out(vty, "Warning: The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    hipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    hipv4 = ntohl(hipv4);
    if (0 == inet_valid_network(hipv4))
    {
        vty_out(vty, "Warning: The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    prefix.type      = destip.family;
    prefix.addr.ipv4 = hipv4;
    prefix.prefixlen = destip.prefixlen;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "prefix %s\n", ldp_ipv4_to_str(hipv4));
    pfilter = ldp_filter_lookup(&prefix);
    if (NULL == pfilter)
    {
        ldp_filter_create(&prefix);
    }
    return CMD_SUCCESS;
}


DEFUN(no_mpls_ldp_prefix,
      no_mpls_ldp_prefix_cmd,
      "no mpls ldp ip-prefix [A.B.C.D/M]",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Ip prefix that need to create lsp\n"
      "Ip prefix, format: A.B.C.D/M for IPv4\n"
     )
{
    struct inet_prefix prefix;
    struct ldp_filter *pfilter = NULL;
    struct prefix_ipv4  destip;
    uint32_t hipv4 = 0;
    if(NULL == argv[0])
    {
        ldp_filter_clear();
        return CMD_SUCCESS;
    }
    
    if (0 == str2prefix_ipv4(argv[0], &destip))
    {
        vty_out(vty, "Warning: The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    hipv4 = ipv4_network_addr(destip.prefix.s_addr, destip.prefixlen);
    hipv4 = ntohl(hipv4);
    if (0 == inet_valid_network(hipv4))
    {
        vty_out(vty, "Warning: The specified IP address is invalid.%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    prefix.type      = destip.family;
    prefix.addr.ipv4 = hipv4;
    prefix.prefixlen = destip.prefixlen;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "prefix %s\n", ldp_ipv4_to_str(hipv4));
    pfilter = ldp_filter_lookup(&prefix);
    if (pfilter)
    {
        ldp_filter_del(pfilter);
    }
    return CMD_SUCCESS;
}

#if 0
DEFUN(mpls_proxy_egress,
      mpls_proxy_egress_cmd,
      "mpls proxy-egress enable",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Proxy egress lsp\n"
      "Enable proxy egress lsp function\n"
     )
{
    struct inet_prefix prefix;
    gldp.proxy_egress = ENABLE;

    struct ldp_filter *pfilter = NULL;

    pfilter = ldp_filter_lookup(&prefix);
    if (pfilter)
    {
        ldp_filter_del(pfilter);
    }


    return CMD_SUCCESS;
}


DEFUN(no_mpls_proxy_egress,
      no_mpls_proxy_egress_cmd,
      "no mpls proxy-egress enable",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Proxy egress lsp\n"
      "Enable proxy egress lsp function\n"
     )
{
    struct inet_prefix prefix;
    gldp.proxy_egress = DISABLE;

    struct ldp_filter *pfilter = NULL;

    pfilter = ldp_filter_lookup(&prefix);
    if (pfilter)
    {
        ldp_filter_del(pfilter);
    }


    return CMD_SUCCESS;
}

#endif

DEFUN(ldp_bfd_enable,
      ldp_bfd_enable_cmd,
      "ldp bfd enable",
      "Label Distribution Protocol(LDP)\n"
      "BFD(Bidirectional Forwarding Detection) function\n"
      "Enable ability of ldp to create bfd session\n"
     )
{
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    gldp.bfd_enable = MPLS_LDP_BFD_ENABLE;
    ldp_sesss_bfd_unbind(BFD_SUBTYPE_BIND_LDP);
    return CMD_SUCCESS;
}

DEFUN(no_ldp_bfd_enable,
      no_ldp_bfd_enable_cmd,
      "no ldp bfd enable",
      "Delete\n"
      "Label Distribution Protocol(LDP)\n"
      "Disable ability of ldp to create bfd session\n"
     )
{
    gldp.bfd_enable = MPLS_LDP_BFD_DISBALE;//DEFAULT
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    ldp_sesss_bfd_unbind(BFD_SUBTYPE_UNBIND_LDP);
    return CMD_SUCCESS;
}


DEFUN(ldp_label_policy,
      ldp_label_policy_cmd,
      "ldp label-policy (all | ip-prefix | host | none)",
      "Label Distribution Protocol(LDP)\n"
      "Policy for label distribution\n"
      "Distribute label for all the route\n"
      "Distribute label for route match the ip-prefix\n"
      "Distribute label for the host route\n"
      "Never distribute label\n"
     )
{
    uint32_t policy_old = gldp.policy;

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "policy %s\n", argv[0]);
    if (argv[0][0] == 'a')
    {
        gldp.policy = LABEL_POLICY_ALL;
    }
    else if (argv[0][0] == 'i')
    {
        vty_out(vty, "Warning:the modification on ip-prefix list will not take effect on the LDP session that has been up, when the ip-prefix lsp trigger is configured !%s", VTY_NEWLINE);
        gldp.policy = LABEL_POLICY_PREFIX;
    }
    else if (argv[0][0] == 'h')
    {
        gldp.policy = LABEL_POLICY_HOST;
    }
    else if (argv[0][0] == 'n')
    {
        gldp.policy = LABEL_POLICY_NONE;
    }
    if (policy_old != gldp.policy)
    {
        ldp_session_clear();
    }

    return CMD_SUCCESS;
}

DEFUN(no_ldp_label_policy,
      no_ldp_label_policy_cmd,
      "no ldp label-policy",
      "Delete\n"
      "Label Distribution Protocol(LDP)\n"
      "Policy for label distribution\n"
     )
{
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    if (gldp.policy != LABEL_POLICY_HOST)
    {
        gldp.policy = LABEL_POLICY_HOST;
        ldp_session_clear();
    }

    return CMD_SUCCESS;
}

DEFUN(ldp_advertise_mode,
      ldp_advertise_mode_cmd,
      "mpls ldp advertise (dod | du)",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Label Advertisement Mode\n"
      "Downstream on Demand Mode\n"
      "Downstream Unsolicited Mode\n"
     )
{
    uint32_t advertise = LABEL_ADVERTISE_DU;

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    if (argv[0][1] == 'o')
    {
        advertise = LABEL_ADVERTISE_DOD;
    }

    if (gldp.advertise != advertise)
    {
        gldp.advertise = advertise;
        vty_out(vty, "All the ldp sssions is restarted for the ldp advertise mode be changed!%s", VTY_NEWLINE);
        ldp_session_clear();
    }

    return CMD_SUCCESS;
}


DEFUN(no_ldp_advertise_mode,
      no_ldp_advertise_mode_cmd,
      "no mpls ldp advertise",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Label Advertisement Mode\n"
     )
{
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    if (LABEL_ADVERTISE_DU != gldp.advertise)
    {
        gldp.advertise = LABEL_ADVERTISE_DU;
        vty_warning_out(vty, "all the ldp sssions will be restarted ,if the ldp advertise mode be changed.%s", VTY_NEWLINE);
        ldp_session_clear();
    }
    return CMD_SUCCESS;
}
void ldp_if_hello_internal_update(uint32_t hello_internal)
{
    /* list for each ldp interface update the hello internal */
    struct hash_bucket *pbucket     = NULL;
    struct mpls_if *pif = NULL;
    struct ifm_l3   pl3if;
    uint32_t cursor = 0;
    uint32_t interval = hello_internal;
    
    HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
    {
        pif = (struct mpls_if *)pbucket->data;
        if (NULL == pif)
        {
            continue;
        }
        if(ENABLE == pif->ldp_conf.ldp_enable)
        {
            /* é…ç½®æ”¹å° */
            if(interval < pif->ldp_conf.hello_send_effect)
            {
                pif->ldp_conf.hello_send_config = interval;
                pif->ldp_conf.hello_send_effect = interval;
                if(0 != pif->ldp_conf.phello_thread)
                {
                    MPLS_TIMER_DEL(pif->ldp_conf.phello_thread);
                    pif->ldp_conf.phello_thread = MPLS_TIMER_ADD(ldp_adjance_link_hello_timer, (void *)(pif->ifindex), pif->ldp_conf.hello_send_effect);
                    
                    if ((ifm_get_l3if(pif->ifindex, MODULE_ID_MPLS, &pl3if) == 0) && (pl3if.ipv4[0].addr != 0))
                    {
                        ldp_send_hello(pl3if.ipv4[0].addr, LDP_HELLO_ADDR, pif->ifindex, LDP_SESSION_LOCAL);
                    }
                }
            }
            else
            {
                /* é…ç½®æ”¹å¤§ */
                pif->ldp_conf.hello_send_config = interval;
                interval = ldp_adjance_hello_internal_recalculate(&pif->ldp_conf.neighbor_list);
                pif->ldp_conf.hello_send_effect = interval;
                
                if(0 != pif->ldp_conf.phello_thread)
                {
                    MPLS_TIMER_DEL(pif->ldp_conf.phello_thread);
                    pif->ldp_conf.phello_thread = MPLS_TIMER_ADD(ldp_adjance_link_hello_timer, (void *)(pif->ifindex), pif->ldp_conf.hello_send_effect);

                    if ((ifm_get_l3if(pif->ifindex, MODULE_ID_MPLS, &pl3if) == 0) && (pl3if.ipv4[0].addr != 0))
                    {
                        ldp_send_hello(pl3if.ipv4[0].addr, LDP_HELLO_ADDR, pif->ifindex, LDP_SESSION_LOCAL);
                    }
                }
                

            }
        }
    }
}
DEFUN(ldp_hello_interval,
      ldp_hello_interval_cmd,
      "mpls ldp hello <1-3600>",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify interval of Hello-send time(in seconds)\n"
      "INTEGER<1-3600> Interval of Hello-send timer(in seconds)\n"
     )
{
    uint16_t hello_interval = 0;

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");

    VTY_GET_INTEGER_RANGE("hello", hello_interval, argv[0], 1, 3600);
    if(hello_interval != gldp.hello_internal)
    {
        gldp.hello_internal = hello_interval;

        ldp_if_hello_internal_update(hello_interval);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ldp_hello_interval,
      no_ldp_hello_interval_cmd,
      "no mpls ldp hello",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify interval of Hello-send time(in seconds)\n"
     )
{
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");

    if(LDP_HELLO_INTERNAL_DEFAULT != gldp.hello_internal)
    {
        gldp.hello_internal = LDP_HELLO_INTERNAL_DEFAULT;
        
        ldp_if_hello_internal_update(LDP_HELLO_INTERNAL_DEFAULT);
    }
    return CMD_SUCCESS;
}


DEFUN(ldp_keepalive_interval,
      ldp_keepalive_interval_cmd,
      "mpls ldp keepalive <1-3600>",
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify interval of KeepAlive-send time(in seconds)\n"
      "INTEGER<1-3600> Interval of KeepAlive-send timer(in seconds)\n"
     )
{
    uint16_t keepalive = 0;

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");


    VTY_GET_INTEGER_RANGE("keepalive", keepalive, argv[0], 1, 3600);

    gldp.keepalive_internal = keepalive;

    return CMD_SUCCESS;
}


DEFUN(no_ldp_keepalive_interval,
      no_ldp_keepalive_interval_cmd,
      "no mpls ldp keepalive",
      "Delete\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Specify interval of KeepAlive-send time(in seconds)\n"
     )
{
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");

    gldp.keepalive_internal = LDP_KEEP_ALIVE_TIME_DEFAULT;//Ä¬ÈÏ300Ãë

    return CMD_SUCCESS;
}

DEFUN(ldp_peer_lsrid_debug,
      ldp_peer_lsrid_debug_cmd,
      "debug ldp peer A.B.C.D",
      DEBUG_STR
      "Label Distribution Protocol(LDP)\n"
      "Peer lsrid to be debuged\n"
      "Ip address, format: A.B.C.D\n"
     )
{

    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");

    gldp.debug_peerlsrid = inet_strtoipv4((char *)argv[0]);
    return CMD_SUCCESS;
}

DEFUN(no_ldp_peer_lsrid_debug,
      no_ldp_peer_lsrid_debug_cmd,
      "no debug ldp peer",
      DEBUG_STR
      "Label Distribution Protocol(LDP)\n"
      "Peer lsrid to be debuged\n"
     )
{
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");

    gldp.debug_peerlsrid = 0;
    return CMD_SUCCESS;
}

DEFUN(ldp_procedures_debug,
      ldp_procedures_debug_cmd,
      "debug ldp {discovery | fsm | notify | label | pw | main | all}",
      DEBUG_STR
      "Label Distribution Protocol(LDP)\n"
      "Discovery debugging function\n"
      "Fsm debugging function\n"
      "Notification debugging function\n"
      "label debugging function\n"
      "PW debugging function\n"
      "Main debugging function\n"
      "All debugging functions\n"
     )
{
    uint32_t flag = 0;
    int i = 0;
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    for (i = 0; i < argc; i++)
    {
        /* debug ldp discovery fsm ... */
        if (NULL != argv[i])
        {
            gldp.debug_ldpflag |= 1u << i;
            flag = 1;
        }
    }

    if (0 == flag)
    {
        /* debug ldp , default to debug all */
        gldp.debug_ldpflag = 0x3f;
    }
    else
    {
        if (NULL != argv[6])
        {
            /* debug ldp all */
            gldp.debug_ldpflag = 0x3f;
        }
    }
    return CMD_SUCCESS;
}



DEFUN(no_ldp_procedures_debug,
      no_ldp_procedures_debug_cmd,
      "no debug ldp {discovery | fsm | notify | label | pw | main | all}",
      "Delete\n"
      DEBUG_STR
      "Label Distribution Protocol(LDP)\n"
      "Discovery debugging function\n"
      "Fsm debugging function\n"
      "Notification debugging function\n"
      "label debugging function\n"
      "PW debugging function\n"
      "Main debugging function\n"
      "All debugging functions\n"
     )
{
    uint32_t flag = 0;
    int i = 0;
    MPLS_LSRID_CHECK(vty);
    MPLS_LDP_GLOBAL_ENABEL_CHECK(vty);
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    for (i = 0; i < argc; i++)
    {
        /* debug ldp discovery fsm ... */
        if (NULL != argv[i])
        {
            gldp.debug_ldpflag &= ~(1u << i);
            flag = 1;
        }
    }

    if (0 == flag)
    {
        /* no debug ldp */
        gldp.debug_ldpflag = 0;
    }
    else
    {
        /* no debug ldp all */
        if (NULL != argv[6])
        {
            /* no debug ldp all */
            gldp.debug_ldpflag = 0;
        }
    }

    return CMD_SUCCESS;
}


DEFUN(ldp_show_config,
      ldp_show_config_cmd,
      "show mpls ldp config",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Show configuration view information\n"
     )
{
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    vty_out(vty, "%-20s  %s%s", "MPLS ldp function:", gmpls.ldp_enable ? "enable":"disable", VTY_NEWLINE);
    vty_out(vty, "%-20s  %s%s", "Policy for label distribution:",
            gldp.policy == LABEL_POLICY_ALL ? "all" :
            (gldp.policy == LABEL_POLICY_HOST ? "host" :
             (gldp.policy == LABEL_POLICY_PREFIX ? "ip-prefix" : "none")), VTY_NEWLINE);

    vty_out(vty, "%-20s  %s%s", "Label Advertisement Mode:",
            gldp.advertise == LABEL_ADVERTISE_DU ? "du" : "dod", VTY_NEWLINE);

    vty_out(vty, "%-20s  %s%s", "Ldp label control mode:",
            gldp.control == LABEL_CONTOL_ORDERED ? "order" : "independent", VTY_NEWLINE);

    vty_out(vty, "%-20s  %d%s", "Ldp adjance Hello internal:", gldp.hello_internal, VTY_NEWLINE);
    vty_out(vty, "%-20s  %d%s", "Ldp Session Keepalive internal:", gldp.keepalive_internal, VTY_NEWLINE);
    vty_out(vty, "%-20s  %s%s", "Ldp Session BFD function:", gldp.bfd_enable ? "enable":"disable", VTY_NEWLINE);
    return CMD_SUCCESS;
}

/* show mpls ldp adjance [peer A.B.C.D] */
DEFUN(ldp_show_adjance,
      ldp_show_adjance_cmd,
      "show mpls ldp adjance [peer A.B.C.D]",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Adjance information\n"
      "Specify peer IP\n"
      "IP_ADDR<X.X.X.X>\n"
     )
{
    struct ldp_sess     *psess      = NULL;
    struct ldp_adjance *padjance   = NULL;
    struct hash_bucket  *pbucket    = NULL;
    struct listnode     *pnode     = NULL;
    struct listnode     *pnodenext = NULL;
    char source_ifaddr[INET_ADDRSTRLEN] = "";
    char peer_lsrid[INET_ADDRSTRLEN]    = "";
    int cursor;
    int i = 0;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    vty_out(vty, "%-3s%-16s%-16s%-10s%-10s%-32s %s", "SN", "SoureAddr", "PeerID", "RcvdHello", "Type", "CreateTime", VTY_NEWLINE);
    vty_out(vty, "-------------------------------------------------------------------------%s", VTY_NEWLINE);
    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if (psess)
        {
            if (NULL != argv[0])
            {
                if (psess->key.peer_lsrid != inet_strtoipv4((char *)argv[0]))
                {
                    continue;
                }
            }

            for (ALL_LIST_ELEMENTS(&psess->adjacency_list, pnode, pnodenext, padjance))
            {
                if (padjance)
                {
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "padjance->psess %p\n", padjance->psess);
                    inet_ipv4tostr(padjance->peer_ip, source_ifaddr);
                    inet_ipv4tostr(padjance->peer_lsrid, peer_lsrid);
                    vty_out(vty, "%-3d%-16s%-16s%-10u%-10s%-32s %s", ++i, source_ifaddr, peer_lsrid,
                            padjance->rcv_cnt, padjance->type == LDP_SESSION_LOCAL ? "L" : "R", ldp_time_to_str(padjance->creat_time), VTY_NEWLINE);
                }
            }
        }
    }
    return CMD_SUCCESS;
}

DEFUN(ldp_show_session,
      ldp_show_session_cmd,
      "show mpls ldp session [peer A.B.C.D]",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Session information\n"
      "Specify peer IP\n"
      "IP_ADDR<X.X.X.X>\n"
     )
{
    struct ldp_sess     *psess   = NULL;
    struct hash_bucket  *pbucket = NULL;
    char local_lsrid[INET_ADDRSTRLEN]   = "";
    char peer_lsrid[INET_ADDRSTRLEN]    = "";
    char status[LDP_STATUS_UP + 1][20]    = {{"non existent"}, {"initialized"}, {"opensent"}, {"openrec"}, {"operational"}};
    int cursor, i = 0;
    uint32_t state = LDP_STATUS_INIT;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    vty_out(vty, "%-3s%-15s%-15s%-10s%-10s%-32s%s", "SN", "PeerID", "Status", "Role", "Type", "CreateTime", VTY_NEWLINE);
    vty_out(vty, "-------------------------------------------------------------------------%s", VTY_NEWLINE);
    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if (psess)
        {
            if (NULL != argv[0])
            {
                if (psess->key.peer_lsrid != inet_strtoipv4((char *)argv[0]))
                {

                    continue;
                }
            }
            state = psess->status;
            if ((LDP_ROLE_PASSIVE == psess->role) && (LDP_STATUS_INIT == psess->status))
            {
                state = LDP_STATUS_NOEXIST;
            }
            inet_ipv4tostr(psess->key.local_lsrid, local_lsrid);
            inet_ipv4tostr(psess->key.peer_lsrid, peer_lsrid);
            vty_out(vty, "%-3d%-15s%-15s", ++i, peer_lsrid, status[state]);
            vty_out(vty, "%-10s", LDP_ROLE_ACTIVE == psess->role ? "active" : "passive");
            vty_out(vty, "%-10s", psess->type == LDP_SESSION_LOCAL ? "L" : (psess->type == LDP_SESSION_REMOT ? "R" : "C"));
            vty_out(vty, "%-32s%s", ldp_time_to_str(psess->creat_time), VTY_NEWLINE);

            struct listnode     *pnode     = NULL;
            struct listnode     *pnodenext = NULL;
            struct ldp_pwinfo   *ppw_info  = NULL;
            struct ldp_fec_t    *pldp_fec  = NULL;
            struct inet_addr    *paddr     = NULL;

            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LSR-ID local:%s    peer:%s", local_lsrid, peer_lsrid);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "state : %s  reason %s", status[psess->status], psess->reason);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "reconnect num: %d", psess->reconnect_num);
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN,  "-------------------------------------------------------------------------%s", VTY_NEWLINE);


            for (ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, ppw_info))
            {
                if (ppw_info)
                {
                    if (ppw_info->local_pwindex)
                    {
                        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "pw local vcid: %d label: %d mtu: %d actye: %d tagflag: %d ctrlword: %d",
                                       ppw_info->local_pwindex->pwinfo.vcid, ppw_info->local_pwindex->inlabel, ppw_info->local_pwindex->pwinfo.mtu,
                                       ppw_info->local_pwindex->pwinfo.ac_type, ppw_info->local_pwindex->pwinfo.tag_flag, ppw_info->local_pwindex->pwinfo.ctrlword_flag);
                    }
                    else
                    {
                        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "pw local vcid --");
                    }
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "pw peer  vcid: %d label: %d mtu: %d actye: %d tagflag: %d ctrlword: %d",
                                   ppw_info->peer_pwinfo.pwinfo.vcid, ppw_info->peer_pwinfo.inlabel, ppw_info->peer_pwinfo.pwinfo.mtu,
                                   ppw_info->peer_pwinfo.pwinfo.ac_type, ppw_info->peer_pwinfo.pwinfo.tag_flag, ppw_info->peer_pwinfo.pwinfo.ctrlword_flag);
                    if((NULL != ppw_info->local_pwindex) && (0 != ppw_info->peer_pwinfo.pwinfo.vcid))
                    {
                        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "flag %d reason %s\n", ppw_info->state,ppw_info->reason);
                    }
                }
            }
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN,  "---------------------------show fec -------------------------------%s", VTY_NEWLINE);
            for (ALL_LIST_ELEMENTS(&psess->feclist, pnode, pnodenext, pldp_fec))
            {
                if (pldp_fec)
                {
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "%s fec prefix %s  prefixlen %d", ((0 != pldp_fec->nhp_cnt) ? "++active" : "--inactive"), ldp_ipv4_to_str(pldp_fec->prefix.addr.ipv4), pldp_fec->prefix.prefixlen);
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "nhp cnt %d\n", pldp_fec->nhp_cnt);
                }
            }
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN,  "--------------------------show--addrs------------------------------%s", VTY_NEWLINE);
            for (ALL_LIST_ELEMENTS(&psess->peer_ifaddrs, pnode, pnodenext, paddr))
            {
                if (paddr)
                {
                    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "peer ldp nhp address %s ", ldp_ipv4_to_str(paddr->addr.ipv4));
                }
            }
        }
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "***********************************end **************************************");
    }
    mpls_ldp_fec_tree_show();
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "***********************************fec tree end **************************************");
    return CMD_SUCCESS;
}

DEFUN(ldp_show_lsp,
      ldp_show_lsp_cmd,
      "show mpls ldp lsp",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Information about the label switched path (LSP)\n"
     )
{
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    mpls_ldp_fec_lsp_show(vty);
    return CMD_SUCCESS;
}


DEFUN(ldp_show_statistics,
      ldp_show_statistics_cmd,
      "show mpls ldp statistics",
      "Show information\n"
      "Specify MPLS(Multiprotocol Label Switching) configuration information\n"
      "Label Distribution Protocol(LDP)\n"
      "Statistics about LDP session\n"
     )
{
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    vty_out(vty, "-------------------ldp statics information ------------%s", VTY_NEWLINE);
    vty_out(vty, "               Session cnt :%u%s", gldp.sess_cnt,     VTY_NEWLINE);
    vty_out(vty, "                Label Send :%u%s", gldp.label_send,   VTY_NEWLINE);
    vty_out(vty, "             Label Receive :%u%s", gldp.label_recv,   VTY_NEWLINE);
    vty_out(vty, "               Packet Send :%u%s", gldp.pkt_send,     VTY_NEWLINE);
    vty_out(vty, "            Packet Receive :%u%s", gldp.pkt_recv,     VTY_NEWLINE);
    vty_out(vty, "              Packet Error :%u%s", gldp.pkt_err,      VTY_NEWLINE);
    vty_out(vty, "            Packet Pw send :%u%s", gldp.pkt_pwsend,   VTY_NEWLINE);
    vty_out(vty, "            Packet Pw recv :%u%s", gldp.pkt_pwrecv,   VTY_NEWLINE);
    return CMD_SUCCESS;
}

void ldp_cmd_init(void)
{
    /*ldp config command*/
    install_element(PHYSICAL_IF_NODE, &ldp_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_IF_NODE, &no_ldp_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &ldp_enable_cmd, CMD_SYNC);
    install_element(PHYSICAL_SUBIF_NODE, &no_ldp_enable_cmd, CMD_SYNC);

    install_element(TRUNK_IF_NODE, &ldp_enable_cmd, CMD_SYNC);
    install_element(TRUNK_IF_NODE, &no_ldp_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &ldp_enable_cmd, CMD_SYNC);
    install_element(TRUNK_SUBIF_NODE, &no_ldp_enable_cmd, CMD_SYNC);


    install_element(MPLS_NODE, &mpls_ldp_prefix_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_mpls_ldp_prefix_cmd, CMD_SYNC);

    install_element(MPLS_NODE, &mpls_ldp_remote_peer_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_mpls_ldp_remote_peer_cmd, CMD_SYNC);

    install_element(MPLS_NODE, &ldp_bfd_enable_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_ldp_bfd_enable_cmd, CMD_SYNC);

    install_element(MPLS_NODE, &ldp_label_policy_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_ldp_label_policy_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &ldp_advertise_mode_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_ldp_advertise_mode_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &ldp_hello_interval_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_ldp_hello_interval_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &ldp_keepalive_interval_cmd, CMD_SYNC);
    install_element(MPLS_NODE, &no_ldp_keepalive_interval_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &ldp_peer_lsrid_debug_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &no_ldp_peer_lsrid_debug_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_procedures_debug_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &no_ldp_procedures_debug_cmd, CMD_LOCAL);

    /*ldp show command*/
    install_element(CONFIG_NODE, &ldp_show_ip_prefix_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_show_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_show_adjance_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_show_session_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_show_lsp_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ldp_show_statistics_cmd, CMD_LOCAL);

}



