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
#include <mpls/pw.h>
#include "ldp.h"
#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_label.h"
#include "ldp_pw.h"


struct list ldp_pw_conf_list; /* ldp pw conf node list */


int ldp_sess_pws_recorect_timer(struct thread *thread)
{
    struct ldp_sess     *psess       = NULL;
    struct hash_bucket  *pbucket     = NULL;
    int32_t              cursor      = 0;
    struct listnode     *pnode       = NULL;
    struct listnode     *pnodenext   = NULL;
    struct ldp_pwinfo   *ppw_lookup  = NULL;

    HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
    {
        psess = (struct ldp_sess *)pbucket->data;
        if((NULL!= psess) && (LDP_STATUS_UP == psess->status))
        {
            for(ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, ppw_lookup))
            {
                if((NULL != ppw_lookup) && (MPLS_MARTINI_PW_UP != ppw_lookup->state))
                {
                    if((NULL != ppw_lookup->local_pwindex) && (0 != ppw_lookup->peer_pwinfo.pwinfo.vcid)
                         && (0 != ppw_lookup->peer_pwinfo.inlabel))
                    {
                        l2vc_add_label(ppw_lookup);
                    }
                }
            }
        }
    }
    gldp.ppw_correct = thread_add_timer(mpls_master, ldp_sess_pws_recorect_timer, (void *)0, LDP_MARTINI_PW_RECORRECT_TIME);

    return ERRNO_SUCCESS;
}

/**
* @brief      <+添加一组PW配置到LDP PW配置节点内+>
* @param[in ] <+pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_pwconf *ldp_pw_conf_node_add(struct l2vc_entry *pl2vc)
{
    struct ldp_pwconf *ppwconf = NULL;
    if (NULL == pl2vc)
    {
        return NULL;
    }

    MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4), pl2vc->pwinfo.vcid);
    ppwconf = (struct ldp_pwconf *)XCALLOC(MTYPE_LDP_ENTRY, sizeof(struct ldp_pwconf));
    if (NULL == ppwconf)
    {
        return NULL;
    }

    memset(ppwconf, 0, sizeof(struct ldp_pwconf));
    ppwconf->peer_lsrid = pl2vc->peerip.addr.ipv4;

    /* add pl2vcnode to pwlist in the pwconf node for they use the same peer ip */
    listnode_add(&ppwconf->pw_list, pl2vc);

    /* add pwconfig node to ldp_pw_conf_list */
    listnode_add(&ldp_pw_conf_list, ppwconf);

    return ppwconf;
}

/**
* @brief      <+删除LDP PW配置节点+>
* @param[in ] <+ppwconf:LDP PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
/* please be sure the pw_list is NULL when the pw_conf_node is deleted */
int ldp_pw_conf_node_del(struct ldp_pwconf *ppwconf)
{
    MPLS_LDP_DEBUG(ppwconf->peer_lsrid, MPLS_LDP_DEBUG_PW, "peerip %s\n", ldp_ipv4_to_str(ppwconf->peer_lsrid));
    listnode_delete(&ldp_pw_conf_list, ppwconf);
    XFREE(MTYPE_LDP_ENTRY, ppwconf);
    return ERRNO_SUCCESS;
}

/**
* @brief      <+根据对端LSRID查找LDP PW配置节点+>
* @param[in ] <+peer_lsrid:对端LSRID+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_pwconf *ldp_pw_conf_node_lookup(uint32_t peer_lsrid)
{
    struct ldp_pwconf *ppwinfo   = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    struct list       *plist     = &ldp_pw_conf_list;

    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, ppwinfo))
    {
        if (ppwinfo->peer_lsrid == peer_lsrid)
        {
            MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_PW, "successful\n");
            return ppwinfo;
        }
    }
    MPLS_LDP_DEBUG(peer_lsrid, MPLS_LDP_DEBUG_PW, "failed\n");
    return NULL;
}

/**
* @brief      <+PW添加通知LDP模块+>
* @param[in ] <+pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int pw_join_ldp(struct l2vc_entry *pl2vc)
{
    struct ldp_sess     *psess      = NULL;
    struct ldp_pwinfo   *pldp_pw    = NULL;
    struct ldp_pwconf   *ppwconf    = NULL;
    uint32_t            local_lsrid = LDP_MPLS_LSRID;

    if (NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4), pl2vc->pwinfo.vcid);

    /* handle the ldp pw conf list first  */
    ppwconf = ldp_pw_conf_node_lookup(pl2vc->peerip.addr.ipv4);
    if (NULL == ppwconf)
    {
        ppwconf = ldp_pw_conf_node_add(pl2vc);
        if (NULL == ppwconf)
        {
            return ERRNO_MALLOC;
        }
    }
    else
    {
        MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4), pl2vc->pwinfo.vcid);
        listnode_add(&ppwconf->pw_list, pl2vc);
    }

    psess = ldp_session_lookup(local_lsrid, pl2vc->peerip.addr.ipv4);
    if (NULL == psess)
    {
        MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u failed for no ldp session available\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4), pl2vc->pwinfo.vcid);
        /* Return if find session failed , pw will be added when the session is created in the hello receive procedure */
        return ERRNO_SUCCESS;
    }

    pldp_pw = ldp_sess_pw_lookup(psess, pl2vc);
    if (NULL != pldp_pw)
    {
        pldp_pw->local_pwindex = pl2vc;
        if ((0 != pldp_pw->peer_pwinfo.pwinfo.vcid) && (0 != pldp_pw->peer_pwinfo.inlabel))
        {
            MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "advertise pw %s up vcid %u up outlabel %d.\n", pl2vc->name, pldp_pw->peer_pwinfo.pwinfo.vcid, pldp_pw->peer_pwinfo.outlabel);
            if (ERRNO_SUCCESS != l2vc_add_label(pldp_pw))
            {
                return ldp_send_pw_release(psess, &pldp_pw->peer_pwinfo);
            }
        }
    }
    else
    {
        pldp_pw = ldp_sess_add_pw(psess, pl2vc, NULL);
    }

    if (LDP_STATUS_UP == psess->status)
    {
        if (!pldp_pw->req_flag)
        {
            ldp_send_pw_request(psess, pldp_pw->local_pwindex);
        }
        ldp_send_pw_mapping(psess, pldp_pw->local_pwindex);
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+PW删除通知LDP模块+>
* @param[in ] <+pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int pw_leave_ldp(struct l2vc_entry *pl2vc)
{
    struct ldp_sess     *psess          = NULL;
    //struct ldp_adjance *premot_adjance = NULL;
    struct ldp_pwinfo   *pldp_pwinfo    = NULL;
    struct ldp_pwconf   *ppwconf        = NULL;

    MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4), pl2vc->pwinfo.vcid);

    /* handle the ldp pw conf list first  */
    ppwconf = ldp_pw_conf_node_lookup(pl2vc->peerip.addr.ipv4);
    if (NULL == ppwconf)
    {
        MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "find conf node failed from ldp_pw_conf_list by peerip %s\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4));
        return ERRNO_FAIL;
    }

    listnode_delete(&ppwconf->pw_list, pl2vc);
    if (list_isempty(&ppwconf->pw_list))
    {
        ldp_pw_conf_node_del(ppwconf);
    }

    psess = ldp_session_lookup(LDP_MPLS_LSRID, pl2vc->peerip.addr.ipv4);
    if (NULL == psess)
    {
        MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "find ldp session failed by peerip %s\n", ldp_ipv4_to_str(pl2vc->peerip.addr.ipv4));
        return ERRNO_NOT_FOUND;
    }
    pldp_pwinfo = ldp_sess_pw_lookup(psess, pl2vc);
    if (NULL != pldp_pwinfo)
    {
        pldp_pwinfo->req_flag = 0;

        if (pldp_pwinfo->local_pwindex)
        {
            /* Inform the peer to release the bingding */
            ldp_send_pw_withdraw(psess, pl2vc);
            /* */
            if (0 != pldp_pwinfo->peer_pwinfo.pwinfo.vcid)
            {
                ldp_send_pw_release(psess, &pldp_pwinfo->peer_pwinfo);
                ldp_sess_del_pw(psess, pldp_pwinfo, LDP_PW_CONF_PEER);
            }

            l2vc_delete_label(pldp_pwinfo->local_pwindex);
            ldp_sess_del_pw(psess, pldp_pwinfo, LDP_PW_CONF_LOCAL);
        }
        else
        {
            MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "find local ldp pw conf in the ldp session faield.\n");
        }
    }
    else
    {
        MPLS_LDP_DEBUG(pl2vc->peerip.addr.ipv4, MPLS_LDP_DEBUG_PW, "find ldp pw conf in the ldp session failed.\n");
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP会话加载对应的PW配置+>
* @param[in ] <+psess:LDP会话结构 ppwconf:LDP PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_add_pws(struct ldp_sess *psess, struct ldp_pwconf *ppwconf)
{
    struct listnode  *pnode         = NULL;
    struct listnode  *pnodenext     = NULL;
    struct l2vc_entry *pl2vc_lookup = NULL;

    if ((NULL == psess) || (NULL == ppwconf))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(ppwconf->peer_lsrid, MPLS_LDP_DEBUG_PW, "ldp session peer %s reload pws from ldp pw conf list.\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    for (ALL_LIST_ELEMENTS(&ppwconf->pw_list, pnode, pnodenext, pl2vc_lookup))
    {
        if (pl2vc_lookup)
        {
            ldp_sess_add_pw(psess, pl2vc_lookup, NULL);
        }
    }
    return ERRNO_SUCCESS;

}

/**
* @brief      <+LDP会话移除PWs配置+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_del_pws(struct ldp_sess *psess)
{
    struct listnode  *pnode         = NULL;
    struct listnode  *pnodenext     = NULL;
    struct ldp_pwinfo *pldp_pwinfo  = NULL;

    if (NULL == psess)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "ldp session peer %s clear pws from ldp pw conf list.\n", ldp_ipv4_to_str(psess->key.peer_lsrid));

    for (ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, pldp_pwinfo))
    {
        if (pldp_pwinfo)
        {
            /* 回话删除前必须先清除本地的和远端的回话结构，否则整个缓存结构不会释放，导致内存泄露 */
            if (pldp_pwinfo->local_pwindex)
            {
                ldp_sess_del_pw(psess, pldp_pwinfo, LDP_PW_CONF_LOCAL);
            }

            if(0 != pldp_pwinfo->peer_pwinfo.pwinfo.vcid)
            {
                ldp_sess_del_pw(psess, pldp_pwinfo, LDP_PW_CONF_PEER);
            }
        }
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+LDP会话加载一组PW配置+>
* @param[in ] <+psess:LDP会话结构 local_pw:LDP 本端PW配置结构 peer_pw:LDP 远端PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_pwinfo *ldp_sess_add_pw(struct ldp_sess *psess, struct l2vc_entry *local_pw, struct l2vc_entry *peer_pw)
{
    struct ldp_pwinfo *pldp_pwinfo = NULL;

    pldp_pwinfo = (struct ldp_pwinfo *)XCALLOC(MTYPE_LSP_ENTRY, sizeof(struct ldp_pwinfo));
    memset(pldp_pwinfo, 0, sizeof(struct ldp_pwinfo));

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, " pldp_pwinfo->local_pwindex %p\n", pldp_pwinfo->local_pwindex);
    /* lcoal ldp config record */
    if (NULL != local_pw)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, " by local vcid %u\n", local_pw->pwinfo.vcid);
        pldp_pwinfo->local_pwindex = local_pw;
    }
    /* peer ldp config record */
    if (NULL != peer_pw)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "by peer vcid %u\n", peer_pw->pwinfo.vcid);
        memcpy(&pldp_pwinfo->peer_pwinfo, peer_pw, sizeof(struct l2vc_entry));
    }

    listnode_add(&psess->pwlist, pldp_pwinfo);

    return pldp_pwinfo;
}

/**
* @brief      <+LDP会话删除一组PW配置+>
* @param[in ] <+psess:LDP会话结构 pldp_pwinfo:LDP会话内pw配置结构 flag:本远端pw配置标记+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_del_pw(struct ldp_sess *psess, struct ldp_pwinfo *pldp_pwinfo, uint32_t flag)
{
    if ((NULL == psess) || (NULL == pldp_pwinfo))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (LDP_PW_CONF_LOCAL == flag)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "ldp session peer %s del local pw vcid %u.\n",ldp_ipv4_to_str(psess->key.peer_lsrid), pldp_pwinfo->local_pwindex->pwinfo.vcid);
        pldp_pwinfo->local_pwindex = NULL;
    }

    if (LDP_PW_CONF_PEER == flag)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "ldp session peer %s del peer pw vcid %u.\n", ldp_ipv4_to_str(psess->key.peer_lsrid), pldp_pwinfo->peer_pwinfo.pwinfo.vcid);
        memset(&pldp_pwinfo->peer_pwinfo, 0, sizeof(struct l2vc_entry));
    }

    if ((NULL == pldp_pwinfo->local_pwindex) && (0 == pldp_pwinfo->peer_pwinfo.pwinfo.vcid))
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pw node delete\n");
        listnode_delete(&psess->pwlist, pldp_pwinfo);

        XFREE(MTYPE_LSP_ENTRY, pldp_pwinfo);
    }
    return ERRNO_SUCCESS;
}

/**
* @brief      <+LDP会话内PW配置查询+>
* @param[in ] <+psess:LDP会话结构 ppw:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
struct ldp_pwinfo *ldp_sess_pw_lookup(struct ldp_sess *psess, struct l2vc_entry *ppw)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_pwinfo *ppw_lookup = NULL;

    for (ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, ppw_lookup))
    {
        if (ppw_lookup)
        {
            if (NULL != ppw_lookup->local_pwindex)
            {
                if (ppw_lookup->local_pwindex->pwinfo.vcid == ppw->pwinfo.vcid)
                {
                    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "successful for local node %p\n", ppw_lookup->local_pwindex);
                    return ppw_lookup;
                }
            }

            if (ppw_lookup->peer_pwinfo.pwinfo.vcid == ppw->pwinfo.vcid)
            {
                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "successful for peer node %p\n", ppw_lookup->local_pwindex);
                return ppw_lookup;
            }
        }
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "failed\n");
    return NULL;

}

/**
* @brief      <+LDP会话OPERATIONAL，开始PWs协商+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_pws_negotiate_start(struct ldp_sess *psess)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_pwinfo *ppw_lookup = NULL;

    if (LDP_STATUS_UP != psess->status)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "for all pws in the session");
    for (ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, ppw_lookup))
    {
        if (ppw_lookup)
        {
            if (NULL != ppw_lookup->local_pwindex)
            {
                ldp_send_pw_mapping(psess, ppw_lookup->local_pwindex);
            }
        }
    }
    return ERRNO_SUCCESS;

}

/**
* @brief      <+LDP会话DOWN，开始PW拆除+>
* @param[in ] <+psess:LDP会话结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_sess_pws_negotiate_end(struct ldp_sess *psess)
{
    struct listnode  *pnode       = NULL;
    struct listnode  *pnodenext   = NULL;
    struct ldp_pwinfo *ppw_lookup = NULL;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "for all pws in the session");
    for (ALL_LIST_ELEMENTS(&psess->pwlist, pnode, pnodenext, ppw_lookup))
    {
        if (ppw_lookup)
        {
            if (NULL != ppw_lookup->local_pwindex)
            {
                ldp_send_pw_withdraw(psess, ppw_lookup->local_pwindex);
                if (0 != ppw_lookup->peer_pwinfo.pwinfo.vcid)
                {
                    ldp_send_pw_release(psess, &ppw_lookup->peer_pwinfo);
                    ldp_sess_del_pw(psess, ppw_lookup, LDP_PW_CONF_PEER);
                }

                MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "advertise pw vcid %u down\n", ppw_lookup->local_pwindex->pwinfo.vcid);
                l2vc_delete_label(ppw_lookup->local_pwindex);
            }
        }
    }
    return ERRNO_SUCCESS;

}

/**
* @brief      <+处理PW请求报文+>
* @param[in ] <+psess:LDP会话结构 pbasemsg:LDP基础消息头+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_pw_request(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg)
{
    struct l2vc_entry    peer_vc;
    struct ldpmsg_req   *ppw_req    = NULL;
    struct ldptlv_fec   *pfectlv    = NULL;
    struct ldp_pwinfo   *pldp_pwinfo = NULL;

    if ((NULL == psess) || (NULL == pbasemsg))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    if (LDP_MSG_LABEL_REQUSET != ntohs(pbasemsg->msg_type))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    ppw_req = (struct ldpmsg_req *)pbasemsg;
    pfectlv = &ppw_req->fec;

    memset(&peer_vc, 0, sizeof(struct l2vc_entry));
    peer_vc.pwinfo.vcid = ntohl(pfectlv->element.pwfec.vcid);
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "mpls ldp get pw request vcid %u.\n", peer_vc.pwinfo.vcid);
    pldp_pwinfo = ldp_sess_pw_lookup(psess, &peer_vc);
    if (NULL != pldp_pwinfo)
    {
        memcpy(&pldp_pwinfo->peer_pwinfo, &peer_vc, sizeof(struct l2vc_entry));
        if (pldp_pwinfo->req_flag)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "mpls ldp get pw request repeat.\n");
        }
    }
    else
    {
        pldp_pwinfo = ldp_sess_add_pw(psess, NULL, &peer_vc);
    }

    pldp_pwinfo->req_flag = 1;

    if (NULL !=  pldp_pwinfo->local_pwindex)
    {
        ldp_send_pw_mapping(psess, pldp_pwinfo->local_pwindex);
    }

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "receive pw vcid %u request finished\n", peer_vc.pwinfo.vcid);
    return ERRNO_SUCCESS;
}
/**
* @brief      <+处理PW协商报文+>
* @param[in ] <+psess:LDP会话结构 pbasemsg:LDP基础消息头+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_pw_maping(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg)
{
    struct ldpmsg_map   *plabelmap      = NULL;
    struct ldptlv_fec   *pfectlv        = NULL;
    struct ldptlv_label *plabeltlv      = NULL;
    struct ldp_pwinfo   *pldp_pwinfo    = NULL;
    struct l2vc_entry    peer_vc;
    uint32_t outlabel = 0;

    if ((NULL == psess) || (NULL == pbasemsg))
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    if (LDP_MSG_LABEL_MAPPING != ntohs(pbasemsg->msg_type))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plabelmap = (struct ldpmsg_map *)pbasemsg;
    pfectlv = &plabelmap->fec;
    plabeltlv = (struct ldptlv_label *)((char *)pfectlv + (ntohs(pfectlv->basetlv.tlv_len) + LDP_BASETLV_LEN));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "peerip %s vcid %u\n", ldp_ipv4_to_str(psess->key.peer_lsrid), ntohl(pfectlv->element.pwfec.vcid));

    outlabel |= ntohs(plabeltlv->label);
    outlabel |= (plabeltlv->label2) << 16;

    memset(&peer_vc, 0, sizeof(struct l2vc_entry));
    peer_vc.inlabel = outlabel;
    peer_vc.pwinfo.pwindex = ntohl(pfectlv->element.pwfec.vcid);
    peer_vc.pwinfo.vcid = ntohl(pfectlv->element.pwfec.vcid);
    pfectlv->element.pwfec.bitmap.u = ntohs(pfectlv->element.pwfec.bitmap.u);
    peer_vc.pwinfo.ctrlword_flag = pfectlv->element.pwfec.bitmap.pwbit.cbit;
    peer_vc.pwinfo.mtu = ntohs(pfectlv->element.pwfec.mtu.mtumax);

    if (pfectlv->element.pwfec.bitmap.pwbit.vctype == PW_VC_TYPE_ETH)
    {
        peer_vc.pwinfo.tag_flag = PW_VC_TYPE_ETH;
    }
    else if (pfectlv->element.pwfec.bitmap.pwbit.vctype == PW_VC_TYPE_TDM)
    {
        peer_vc.pwinfo.tag_flag = PW_VC_TYPE_TDM;
    }
    else if (pfectlv->element.pwfec.bitmap.pwbit.vctype == PW_VC_TYPE_VLAN)
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "mpls pw support vctype vlan.\n");

        peer_vc.pwinfo.tag_flag = PW_VC_TYPE_VLAN;
    }

    pldp_pwinfo = ldp_sess_pw_lookup(psess, &peer_vc);
    if (NULL != pldp_pwinfo)
    {
        memcpy(&pldp_pwinfo->peer_pwinfo, &peer_vc, sizeof(struct l2vc_entry));
        if (NULL != pldp_pwinfo->local_pwindex)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "advertise pw vcid %d UP %x.\n", pldp_pwinfo->local_pwindex->pwinfo.vcid, outlabel);
            if (ERRNO_SUCCESS != l2vc_add_label(pldp_pwinfo))
            {
                return ldp_send_pw_release(psess, &peer_vc);
            }
        }
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pldp_pwinfo->local_pwindex %p UP .\n", pldp_pwinfo->local_pwindex);
    }
    else
    {
        MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "mpls LDP get pw maping without receiving pw request packete.\n");
        ldp_sess_add_pw(psess, NULL, &peer_vc);
    }

    MPLS_LDP_PKT_PW_RECV(gldp);
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "recieve pw mapping finished\n");
    return ERRNO_SUCCESS;
}
/**
* @brief      <+处理PW撤回报文+>
* @param[in ] <+psess:LDP会话结构 pbasemsg:LDP基础消息头+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_pw_withdraw(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg)
{
    struct ldpmsg_withdraw  *pwithdraw  = NULL;
    struct ldptlv_fec       *pfectlv    = NULL;
    struct ldp_pwinfo       *pldp_pwinfo = NULL;
    struct l2vc_entry       peer_vc;

    if ((NULL == psess) || (NULL == pbasemsg))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    pwithdraw = (struct ldpmsg_withdraw *)pbasemsg;
    pfectlv = &pwithdraw->fec;

    memset(&peer_vc, 0, sizeof(struct l2vc_entry));
    peer_vc.pwinfo.vcid = ntohl(pfectlv->element.pwfec.vcid);

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "vcid %u\n", peer_vc.pwinfo.vcid);
    pldp_pwinfo = ldp_sess_pw_lookup(psess, &peer_vc);
    if (NULL != pldp_pwinfo)
    {
        if (NULL != pldp_pwinfo->local_pwindex)
        {
            MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "%p advertise pw vcid %u down.\n",pldp_pwinfo->local_pwindex,  ntohl(pfectlv->element.pwfec.vcid));
            l2vc_delete_label(pldp_pwinfo->local_pwindex);
        }

        if (0 != pldp_pwinfo->peer_pwinfo.pwinfo.vcid)
        {
            ldp_send_pw_release(psess, &pldp_pwinfo->peer_pwinfo);
            ldp_sess_del_pw(psess, pldp_pwinfo, LDP_PW_CONF_PEER);
        }

    }
    return ERRNO_SUCCESS;
}
/**
* @brief      <+处理PW映射释放报文+>
* @param[in ] <+psess:LDP会话结构 pbasemsg:LDP基础消息头+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_recv_pw_release(struct ldp_sess *psess, struct ldpmsg_base  *pbasemsg)
{
    struct ldpmsg_release   *prelease   = NULL;
    struct ldptlv_fec       *pfectlv    = NULL;

    if ((NULL == psess) || (NULL == pbasemsg))
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    prelease = (struct ldpmsg_release *)pbasemsg;
    pfectlv = &prelease->fec;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "vcid %u\n", ntohl(pfectlv->element.pwfec.vcid));
    return ERRNO_SUCCESS;
}

/**
* @brief      <+发送PW协商请求报文+>
* @param[in ] <+psess:LDP会话结构 pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_pw_request(struct ldp_sess *psess, struct l2vc_entry *pl2vc)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader     = NULL;
    struct ldpmsg_base  *pbasemsg    = NULL;
    struct ldptlv_fec   *pfec_temp   = NULL;
    char    *pend = NULL;
    uint32_t len = 0;
    uint16_t vctype = 0;

    if (NULL == psess || NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "peer lsrid %s vcid %d inlabel %d\n", ldp_ipv4_to_str(psess->key.peer_lsrid), pl2vc->pwinfo.vcid, pl2vc->inlabel);

    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);
    pfec_temp = (struct ldptlv_fec *)((char *)pend - (sizeof(struct ldptlv_fec) - sizeof(struct ldptlv_vccv)));
    pfec_temp->element.pwfec.type = LDP_TLV_PW_FEC;

    if((PW_ENCAP_ETHERNET_RAW == pl2vc->pwinfo.tag_flag) || (PW_ENCAP_VLAN_RAW == pl2vc->pwinfo.tag_flag))
    {
        vctype = PW_VC_TYPE_ETH;
    }
    else
    {
        vctype = PW_VC_TYPE_VLAN;
    }

    if(pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }

#if 0
    if (pl2vc->pwinfo.ac_type == AC_TYPE_ETH)
    {
        if (pl2vc->pwinfo.tag_flag)
        {
            vctype = PW_VC_TYPE_VLAN;
        }
        else
        {
            vctype = PW_VC_TYPE_ETH;
        }
    }
    else if (pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    else
    {
        /* 交换PW VCTYEP */
        vctype = PW_VC_TYPE_VLAN;
    }
#endif
    pfec_temp->element.pwfec.bitmap.pwbit.vctype = vctype;
    pfec_temp->element.pwfec.bitmap.pwbit.cbit   = pl2vc->pwinfo.ctrlword_flag;
    pfec_temp->element.pwfec.bitmap.u = htons(pfec_temp->element.pwfec.bitmap.u);
    pfec_temp->element.pwfec.groupid         = 0;
    pfec_temp->element.pwfec.vcid            = htonl(pl2vc->pwinfo.vcid);
    pfec_temp->element.pwfec.length          = 8;
    pfec_temp->element.pwfec.mtu.parmater_id = LDP_PW_PARA_ID;
    pfec_temp->element.pwfec.mtu.length      = LDP_PW_MTU_LEN;
    pfec_temp->element.pwfec.mtu.mtumax      = htons(pl2vc->pwinfo.mtu);
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_PWFEC_VAL_LEN);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_REQUSET, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    MPLS_LDP_PKT_PW_SEND(gldp);

    return ldp_send_tcp_pkt(psess, pheader, len);

}
/**
* @brief      <+发送PW协商报文+>
* @param[in ] <+psess:LDP会话结构 pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_pw_mapping(struct ldp_sess *psess, struct l2vc_entry *pl2vc)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader = NULL;
    struct ldpmsg_base  *pbasemsg = NULL;
    struct ldptlv_fec   *pfec_temp = NULL;
    struct notifytlv_pwstatus *pwstatus = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char *pend = NULL;
    uint32_t len = 0;
    uint16_t vctype = 0;

    if (NULL == psess || NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "peer lsrid %s vcid %d inlabel %d\n", ldp_ipv4_to_str(psess->key.peer_lsrid), pl2vc->pwinfo.vcid, pl2vc->inlabel);

    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);//caculate the end address of the packet
    pwstatus = (struct notifytlv_pwstatus *)(pend - sizeof(struct notifytlv_pwstatus));

    pwstatus->tlv_type = htons(PW_STATUS_TLV | 0x8000);
    pwstatus->tlv_len = htons(4);
    pwstatus->pwstatus = htonl(0x0);

    plabel_temp = (struct ldptlv_label *)((char *)pwstatus - sizeof(struct ldptlv_label));

    /* pw inlabel tlv */
    plabel_temp->label = pl2vc->inlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (pl2vc->inlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);
    /* pw fec tlv */
    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - sizeof(struct ldptlv_fec));
    pfec_temp->element.pwfec.type = LDP_TLV_PW_FEC;

    if((PW_ENCAP_ETHERNET_RAW == pl2vc->pwinfo.tag_flag) || (PW_ENCAP_VLAN_RAW == pl2vc->pwinfo.tag_flag))
    {
        vctype = PW_VC_TYPE_ETH;
    }
    else
    {
        vctype = PW_VC_TYPE_VLAN;
    }

    if(pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
#if 0
    if (pl2vc->pwinfo.ac_type == AC_TYPE_ETH)
    {
        if (pl2vc->pwinfo.tag_flag)
        {
            vctype = PW_VC_TYPE_VLAN;
        }
        else
        {
            vctype = PW_VC_TYPE_ETH;
        }
    }
    else if (pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    else
    {
        /* 交换PW VCTYEP */
        vctype = PW_VC_TYPE_VLAN;
    }
#endif
    pfec_temp->element.pwfec.bitmap.pwbit.vctype = vctype;
    pfec_temp->element.pwfec.bitmap.pwbit.cbit   = pl2vc->pwinfo.ctrlword_flag;
    pfec_temp->element.pwfec.bitmap.u = htons(pfec_temp->element.pwfec.bitmap.u);

    pfec_temp->element.pwfec.groupid = 0;
    pfec_temp->element.pwfec.vcid = htonl(pl2vc->pwinfo.vcid);
    pfec_temp->element.pwfec.length = 12;
    pfec_temp->element.pwfec.mtu.parmater_id = LDP_PW_PARA_ID;
    pfec_temp->element.pwfec.mtu.length = LDP_PW_MTU_LEN;
    pfec_temp->element.pwfec.mtu.mtumax = htons(pl2vc->pwinfo.mtu);
    /* add for vccv check */
    pfec_temp->element.pwfec.vccv.id = LDP_MPLS_VCCV_ID;
    pfec_temp->element.pwfec.vccv.length = LDP_MPLS_VCCV_LEN;
    if(pl2vc->pwinfo.ctrlword_flag)
    {
        pfec_temp->element.pwfec.vccv.cctype |= LDP_MPLS_VCCV_CONTROL_WORD_ENABLE;
    }

    if(pl2vc->bfd_id)
    {
        pfec_temp->element.pwfec.vccv.cvtype |= LDP_MPLS_VCCV_BFD_ENABLE;
    }

    if(LDP_MPLS_VCCV_LSP_PING_ENABLE)
    {
        pfec_temp->element.pwfec.vccv.cvtype |= LDP_MPLS_VCCV_LSP_PING_ENABLE;
    }


    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_PWFEC_VAL_LEN + sizeof(struct ldptlv_vccv));

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_MAPPING, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    MPLS_LDP_PKT_PW_SEND(gldp);

    return ldp_send_tcp_pkt(psess, pheader, len);
}
/**
* @brief      <+发送PW映射撤回报文+>
* @param[in ] <+psess:LDP会话结构 pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
/* pw withdraw msg will be sended with  the gernel label tlv in the packet */
int ldp_send_pw_withdraw(struct ldp_sess *psess, struct l2vc_entry *pl2vc)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader     = NULL;
    struct ldpmsg_base  *pbasemsg    = NULL;
    struct ldptlv_fec   *pfec_temp   = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char *pend = NULL;
    uint32_t len = 0;
    uint16_t vctype = 0;

    if (NULL == psess || NULL == pl2vc)
    {
        return ERRNO_FAIL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "peer lsrid %s vcid %d inlabel %d\n", ldp_ipv4_to_str(psess->key.peer_lsrid), pl2vc->pwinfo.vcid, pl2vc->inlabel);

    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);

    plabel_temp = (struct ldptlv_label *)(pend - sizeof(struct ldptlv_label));

    /* pw inlabel tlv */
    plabel_temp->label = pl2vc->inlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (pl2vc->inlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);

    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - (sizeof(struct ldptlv_fec) - sizeof(struct ldptlv_vccv)));
    pfec_temp->element.pwfec.type = LDP_TLV_PW_FEC;

    if((PW_ENCAP_ETHERNET_RAW == pl2vc->pwinfo.tag_flag) || (PW_ENCAP_VLAN_RAW == pl2vc->pwinfo.tag_flag))
    {
        vctype = PW_VC_TYPE_ETH;
    }
    else
    {
        vctype = PW_VC_TYPE_VLAN;
    }

    if(pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    
#if 0
    if (pl2vc->pwinfo.ac_type == AC_TYPE_ETH)
    {
        if (pl2vc->pwinfo.tag_flag)
        {
            vctype = PW_VC_TYPE_VLAN;
        }
        else
        {
            vctype = PW_VC_TYPE_ETH;
        }
    }
    else if (pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    else
    {
        /* 交换PW VCTYEP */
        vctype = PW_VC_TYPE_VLAN;
    }
#endif
    pfec_temp->element.pwfec.bitmap.pwbit.vctype = vctype;
    pfec_temp->element.pwfec.bitmap.pwbit.cbit   = pl2vc->pwinfo.ctrlword_flag;
    pfec_temp->element.pwfec.bitmap.u = htons(pfec_temp->element.pwfec.bitmap.u);

    pfec_temp->element.pwfec.length = 8;
    pfec_temp->element.pwfec.groupid = htonl(0);
    pfec_temp->element.pwfec.vcid = htonl(pl2vc->pwinfo.vcid);

    pfec_temp->element.pwfec.mtu.parmater_id = LDP_PW_PARA_ID;
    pfec_temp->element.pwfec.mtu.length = LDP_PW_MTU_LEN;
    pfec_temp->element.pwfec.mtu.mtumax = htons(pl2vc->pwinfo.mtu);
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "\n");
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_PWFEC_VAL_LEN);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_WITHRAW, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    return ldp_send_tcp_pkt(psess, pheader, len);
}

/**
* @brief      <+发送PW映射释放报文+>
* @param[in ] <+psess:LDP会话结构 pl2vc:PW配置结构+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int ldp_send_pw_release(struct ldp_sess *psess, struct l2vc_entry *pl2vc)
{
    struct ldp_pkt pkt;
    struct ldpmsg_pdu   *pheader    = NULL;
    struct ldpmsg_base  *pbasemsg   = NULL;
    struct ldptlv_fec   *pfec_temp  = NULL;
    struct ldptlv_label *plabel_temp = NULL;
    char    *pend   = NULL;
    uint32_t len    = 0;
    uint16_t vctype = 0;


    if (NULL == psess || NULL == pl2vc)
    {
        return ERRNO_PARAM_ILLEGAL;
    }
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "peer lsrid %s vcid %d inlabel %d\n", ldp_ipv4_to_str(psess->key.peer_lsrid), pl2vc->pwinfo.vcid, pl2vc->inlabel);

    memset(&pkt, 0, sizeof(struct ldp_pkt));
    pend = (char *)(&pkt) + sizeof(struct ldp_pkt);

    plabel_temp = (struct ldptlv_label *)(pend - sizeof(struct ldptlv_label));

    /* pw inlabel tlv */
    plabel_temp->label = pl2vc->inlabel;
    plabel_temp->label = htons(plabel_temp->label);
    plabel_temp->label2 = (pl2vc->inlabel && 0xf0000) >> 16;
    ldp_set_tlvbase(&(plabel_temp->basetlv), LDP_TLV_LABEL, LDP_TLV_LABEL_VAL_LEN);

    pfec_temp = (struct ldptlv_fec *)((char *)plabel_temp - (sizeof(struct ldptlv_fec) - sizeof(struct ldptlv_vccv)));
    /* pw ������� */
    pfec_temp->element.pwfec.type = LDP_TLV_PW_FEC;

    if((PW_ENCAP_ETHERNET_RAW == pl2vc->pwinfo.tag_flag) || (PW_ENCAP_VLAN_RAW == pl2vc->pwinfo.tag_flag))
    {
        vctype = PW_VC_TYPE_ETH;
    }
    else
    {
        vctype = PW_VC_TYPE_VLAN;
    }

    if(pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    
#if 0
    if (pl2vc->pwinfo.ac_type == AC_TYPE_ETH)
    {
        if (pl2vc->pwinfo.tag_flag)
        {
            vctype = PW_VC_TYPE_VLAN;
        }
        else
        {
            vctype = PW_VC_TYPE_ETH;
        }
    }
    else if (pl2vc->pwinfo.ac_type == AC_TYPE_TDM)
    {
        vctype = PW_VC_TYPE_TDM;
    }
    else
    {
        /* 交换PW VCTYEP */
        vctype = PW_VC_TYPE_VLAN;
    }
#endif
    pfec_temp->element.pwfec.bitmap.pwbit.vctype = vctype;
    pfec_temp->element.pwfec.bitmap.pwbit.cbit   = pl2vc->pwinfo.ctrlword_flag;
    pfec_temp->element.pwfec.bitmap.u = htons(pfec_temp->element.pwfec.bitmap.u);

    pfec_temp->element.pwfec.length = 8;
    pfec_temp->element.pwfec.groupid = 0;
    pfec_temp->element.pwfec.vcid = htonl(pl2vc->pwinfo.vcid);
    pfec_temp->element.pwfec.mtu.parmater_id = LDP_PW_PARA_ID;
    pfec_temp->element.pwfec.mtu.length = LDP_PW_MTU_LEN;
    pfec_temp->element.pwfec.mtu.mtumax = htons(pl2vc->pwinfo.mtu);
    ldp_set_tlvbase(&(pfec_temp->basetlv), LDP_TLV_FEC, LDP_TLV_PWFEC_VAL_LEN);

    pbasemsg = (struct ldpmsg_base *)((char *)pfec_temp - sizeof(struct ldpmsg_base));
    len = (pend - (char *)pbasemsg) - LDP_BASEMSG_LEN;
    ldp_set_msgbase(pbasemsg, LDP_MSG_LABEL_RELEASE, len);

    pheader = (struct ldpmsg_pdu *)((char *)pbasemsg - LDP_PDU_HEADER_LEN);
    len += (LDP_BASEMSG_LEN + LDP_IDENTIFY_LEN);
    ldp_set_header(pheader, psess->key.local_lsrid, len);
    len += LDP_HEADER_LEN;

    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pheader %p pkt: %s.\n", pheader, pkt_dump(pheader, len));
    MPLS_LDP_DEBUG(psess->key.peer_lsrid, MPLS_LDP_DEBUG_PW, "pkt: %s\n", pkt_dump(&pkt, sizeof(struct ldp_pkt)));

    return ldp_send_tcp_pkt(psess, pheader, len);
}

