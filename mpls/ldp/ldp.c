/*
*   define of ldp.c ʵ��
*/
#include <string.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/errcode.h>
#include <lib/prefix.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/route_com.h>
#include <lib/ifm_common.h>
#include <lib/log.h>
#include <ftm/pkt_tcp.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_ip.h>
#include <mpls/lspm.h>
#include "mpls.h"
#include "mpls_main.h"
#include "mpls_if.h"


#include "ldp_session.h"
#include "ldp_packet.h"
#include "ldp_lsptrigger.h"
#include "ldp_fec.h"
#include "lsp_fec.h"
#include "ldp_label.h"
#include "ldp_notify.h"
#include "ldp_fsm.h"
#include "ldp.h"
#include "lib/mpls_common.h"

struct ldp_global gldp;
int mpls_if_ldp_enable(struct mpls_if *pif);
int mpls_if_ldp_disable(struct mpls_if *pif);

int mpls_delete_local_ldp(void);
int mpls_delete_remot_ldp(void);

/**
* @brief      <+初始化LDP模块需要的表项+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+mpls模块初始化时调用+>
*/
void ldp_init(void)
{
    ldp_cmd_init();
    ldp_session_table_init(LDP_SESSION_NUM);
    ldp_filter_table_init(LDP_PREFIX_FILTER_NUM_MAX);

    memset(&gldp, 0, sizeof(struct ldp_global));
    gldp.policy = LABEL_POLICY_HOST;
    gldp.advertise = LABEL_ADVERTISE_DU;
    gldp.control = LABEL_CONTOL_ORDERED;
    gldp.hold = LABEL_HOLD_LIBERAL;
    gldp.keepalive_internal = LDP_KEEP_ALIVE_TIME_DEFAULT;
    gldp.hello_internal = LDP_HELLO_INTERNAL_DEFAULT;
    gldp.proxy_egress = DISABLE;//default enable
    gldp.bfd_enable = MPLS_LDP_BFD_DISBALE;
}

/**
* @brief      <+LDP模块收包注册+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+开启LDP全局开关时调用+>
*/
/* ldp udp and tcp packet receive register */
void ldp_pkt_register(void)
{
    union proto_reg proto;
    /* link hello */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_LDP;
    proto.ipreg.dip = LDP_HELLO_ADDR;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);
    /* target hello */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_LDP;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

    /* ldp active receive tcp packet */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_LDP;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

    /* ldp passive receive tcp packet */
    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_LDP;
    pkt_register(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);
}

/**
* @brief      <+ldp packet receive unregister+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+关闭LDP全局开关且会话全部关闭时调用+>
*/
void ldp_pkt_unregister(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_LDP;
    proto.ipreg.dip = LDP_HELLO_ADDR;
    pkt_unregister(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_LDP;
    pkt_unregister(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport = TCP_PORT_LDP;
    pkt_unregister(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.sport = TCP_PORT_LDP;
    pkt_unregister(MODULE_ID_MPLS, PROTO_TYPE_IPV4, &proto);
}

void ldp_tcp_port_alloc(struct ldp_sess *psess)
{
    struct ldp_adjance *padjance = NULL;
    struct ip_proto tcp;

    if(LDP_ROLE_ACTIVE == psess->role)
    {
        /* select the tcp sip and dip */
        padjance = ldp_session_select_neighbor(psess);
        if(padjance)
        {
            memcpy(&psess->tcp_addr, &padjance->tcp_addr, sizeof(struct ldp_taddr));
        }

        /* get the tcp port as sport of tcp session */
        psess->sport = pkt_port_alloc(MODULE_ID_MPLS, PKT_TYPE_TCP);
        psess->dport = TCP_PORT_LDP;
        psess->tcp_status = LDP_TCP_LINK_DOWN;
        MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_DOWN\n");

        /* open the tcp session and wait the tcp established msg from tcp module */
        tcp.dip   = psess->tcp_addr.peer_addr;
        tcp.sip   = psess->tcp_addr.local_addr;
        tcp.dport = psess->dport;
        tcp.sport = psess->sport;
        tcp.protocol = IP_P_TCP;

        pkt_open_tcp(MODULE_ID_MPLS, &tcp);
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "sport %d  dport %d\n", psess->sport, psess->dport);
}

/**
* @brief      <+关闭LDP会话TCP连接+>
* @param[in ] <+psess:LDP会话指针+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+关闭LDP会话时调用+>
*/
/* ldp close tcp link */
void ldp_tcp_port_relese(struct ldp_sess *psess)
{
    struct ip_proto tcp;

    tcp.dip   = psess->key.peer_lsrid;
    tcp.sip   = psess->key.local_lsrid;
    tcp.dport = psess->dport;
    tcp.sport = psess->sport;
    tcp.protocol = IP_P_TCP;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "sport %d  dport %d\n", psess->sport, psess->dport);
    pkt_close_tcp(MODULE_ID_MPLS, &tcp);
    psess->tcp_status = LDP_TCP_LINK_DOWN;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "LDP_TCP_LINK_DOWN\n");
    /* release the tcp port by dynamic */
    if(LDP_ROLE_ACTIVE == psess->role)
    {
        pkt_port_relese(MODULE_ID_MPLS, PKT_TYPE_TCP, psess->sport);
    }

}
/**
* @brief      <+LDP模块路由事件注册：直连路由、静态路由及OSPF路由+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
void mpls_ldp_route_event_register(void)
{
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "\n");
    route_event_register(ROUTE_PROTO_CONNECT, MODULE_ID_MPLS);
    route_event_register(ROUTE_PROTO_STATIC, MODULE_ID_MPLS);
    route_event_register(ROUTE_PROTO_OSPF, MODULE_ID_MPLS);
}
/**
* @brief      <+LDP模块路由事件取消注册+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+LDP模块将回收掉本机发布的FEC，学习到的FEC不处理+>
*/
void mpls_ldp_route_event_unregister(void)
{
    struct ptree_node   *ptreenode  = NULL;
    struct ptree_node   *ptreenode2 = NULL;
    struct lsp_fec_t    *plsp_fec   = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "and withdraw all the egress fecs in the fec tree\n");
    route_event_unregister(ROUTE_PROTO_CONNECT, MODULE_ID_MPLS);
    route_event_unregister(ROUTE_PROTO_STATIC, MODULE_ID_MPLS);
    route_event_unregister(ROUTE_PROTO_OSPF, MODULE_ID_MPLS);

    /* Clear the eggress lsp the ldp added by the route added for the next route add Egress lsp to successful */
    for (ptreenode2 = ptree_top(lsp_fec_tree); ptreenode2 != NULL;)
    {
        ptreenode = ptreenode2;
        ptreenode2 = ptree_next(ptreenode2);
        if(NULL == ptreenode->info)
        {
            continue;
        }
        plsp_fec = ptreenode->info;
        lsp_fec_delete(plsp_fec);
    }
}
/**
* @brief      <+清除本地LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int mpls_delete_local_ldp(void)
{
    struct hash_bucket *pbucket = NULL;
    struct mpls_if     *pif     = NULL;
    int                 cursor;
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "clear all local ldp config\n");

    gldp.advertise = LABEL_ADVERTISE_DU;
    gldp.hello_internal = LDP_HELLO_INTERNAL_DEFAULT;
    gldp.keepalive_internal = LDP_KEEP_ALIVE_TIME_DEFAULT;
    gldp.policy = LABEL_POLICY_HOST;
    gldp.bfd_enable = MPLS_LDP_BFD_DISBALE;
    
    /* list for each mplsif to delete local ldp config */
    HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
    {
        pif = (struct mpls_if *)pbucket->data;
        if (pif == NULL)
        {
            continue;
        }
        if (ENABLE == pif->ldp_conf.ldp_enable)
        {
            mpls_if_delete_local_ldp(pif);
        }
    }
    return ERRNO_SUCCESS;
}
/**
* @brief      <+清除远端LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+LDP远端配置暂时保存在邻居链表内+>
*/
int mpls_delete_remot_ldp(void)
{
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct ldp_adjance *padjance   = NULL;
    struct list         *plist      = &ldp_remot_adjance_list;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_MAIN, "clear all remote ldp config\n");
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if (padjance->type == LDP_SESSION_REMOT)
        {
            if (NULL != padjance->psess)
            {
                if(1 == padjance->psess->adjacency_list.count)
                {
                    ldp_send_notification(padjance->psess, LDP_NOTIF_SHUTDOWN, 0);
                }
                ldp_adjance_leave_sess(padjance, padjance->psess, LDP_EVENT_SHUTDOWN);
            }
            ldp_adjance_delete(padjance, plist);
        }
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+单接口开启本地LDP发现功能+>
* @param[in ] <+pif:mpls_if接口指针+>
* @param[out] <+none+>
* @return     <++none>
* @author     wus
* @date       2018/3/1
* @note       <+LDP可以通过单个接口发现多个潜在的LDP对等体+>
*/
int mpls_if_ldp_enable(struct mpls_if *pif)
{
    uint32_t internal = 1;
    uint32_t ifindex = pif->ifindex;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "link ldp discovery for a interface\n");
    pif->ldp_conf.ldp_enable = ENABLE;
    pif->ldp_conf.hello_send_config = gldp.hello_internal;
    pif->ldp_conf.hello_send_effect = gldp.hello_internal;
    pif->ldp_conf.phello_thread = MPLS_TIMER_ADD(ldp_adjance_link_hello_timer, (void *)ifindex, internal);

    return ERRNO_SUCCESS;
}
/**
* @brief      <+关闭一个接口的LDP邻居发现功能+>
* @param[in ] <+pif:mpls_if接口指针+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+关闭接口邻居发现会直接找到邻居并清除+>
*/
int mpls_if_ldp_disable(struct mpls_if *pif)
{
    struct ldp_adjance *padjance   = NULL;
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct list         *plist      = &pif->ldp_conf.neighbor_list;
    MPLS_LOG_DEBUG();
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "link ldp discovery for a interface\n");
    pif->ldp_conf.ldp_enable = DISABLE;
    if (pif->ldp_conf.phello_thread)
    {
        MPLS_TIMER_DEL(pif->ldp_conf.phello_thread);
        pif->ldp_conf.phello_thread = 0;
    }

    /* list for each adjance on this interface and delete it */
    for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
    {
        if ((padjance->type == LDP_SESSION_LOCAL) && (padjance->ifindex == pif->ifindex))
        {
            MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "ldp delete neighbor peer lsrid %s link ip %s.\n", ldp_ipv4_to_str(padjance->peer_lsrid), ldp_ipv4_to_str(padjance->peer_ip));
            if (padjance->psess)
            {
                if(1 == padjance->psess->adjacency_list.count)
                {
                    ldp_send_notification(padjance->psess, LDP_NOTIF_SHUTDOWN, 0);
                }

                ldp_adjance_leave_sess(padjance, padjance->psess, LDP_EVENT_SHUTDOWN);
            }
            ldp_adjance_delete(padjance, plist);
        }
    }
    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "\n");
    return ERRNO_SUCCESS;
}
/**
* @brief      <+开启远端邻居发现功能+>
* @param[in ] <+peerlsrid:要建立远端对等体的设备的LSRID+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+配置保存在邻居链表+>
*/
int mpls_ldp_remot_enable(uint32_t peerlsrid)
{
    struct ldp_adjance *padjance = NULL;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "peer lsrid %s.\n", ldp_ipv4_to_str(peerlsrid));
    padjance = ldp_adjance_lookup(peerlsrid, 0, peerlsrid, LDP_SESSION_REMOT);
    if (NULL == padjance)
    {
        padjance = ldp_adjance_create(LDP_MPLS_LSRID, peerlsrid, peerlsrid, 0, LDP_SESSION_REMOT);
        if (NULL != padjance)
        {
            padjance->phello_timer = MPLS_TIMER_ADD(ldp_adjance_target_hello_timer, (void *)padjance, 1);
        }
    }

    return ERRNO_SUCCESS;
}
/**
* @brief      <+关闭远端邻居发现功能+>
* @param[in ] <+peerlsrid:要建立远端对等体的设备的LSRID+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
int mpls_ldp_remot_disable(uint32_t peerlsrid)
{
    struct ldp_adjance *padjance = NULL;
    struct list         *plist    = &ldp_remot_adjance_list;

    MPLS_LDP_DEBUG(0, MPLS_LDP_DEBUG_DISCOVER, "peer lsrid %s.\n", ldp_ipv4_to_str(peerlsrid));
    padjance = ldp_adjance_lookup(peerlsrid, 0, peerlsrid, LDP_SESSION_REMOT);
    if (NULL != padjance)
    {
        if (padjance->psess)
        {
            if(1 == padjance->psess->adjacency_list.count)
            {
                ldp_send_notification(padjance->psess, LDP_NOTIF_SHUTDOWN, 0);
            }

            ldp_adjance_leave_sess(padjance, padjance->psess, LDP_EVENT_SHUTDOWN);
        }
        ldp_adjance_delete(padjance, plist);
    }

    return ERRNO_SUCCESS;
}

/**
* @brief      <+系统时间转换成字符串+>
* @param[in ] <+sys_time:系统时间+>
* @param[out] <+none+>
* @return     <+none+>
* @author     wus
* @date       2018/3/1
* @note       <+none+>
*/
char *ldp_time_to_str(time_t sys_time)
{
    static char buffer[32];
    struct tm  *user_time;
    user_time = localtime(&sys_time);

    snprintf(buffer, LDP_SESS_CREAT_TIME_LEN, "%d-%02d-%02d %02d:%02d:%02d",
             user_time->tm_year + 1900, user_time->tm_mon + 1, user_time->tm_mday,
             user_time->tm_hour, user_time->tm_min, user_time->tm_sec);
    return buffer;
}

char *ldp_ipv4_to_str(uint32_t addr)
{
    static char ldp_ip[INET_ADDRSTRLEN] = "";
    
    return inet_ipv4tostr(addr, (char *)(&ldp_ip));
}

char *ldp_ipv4_to_str2(uint32_t addr)
{
    static char ldp_ip2[INET_ADDRSTRLEN] = "";
    
    return inet_ipv4tostr(addr, (char *)(&ldp_ip2));
}

/* ldp add for mib */


/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_ldp_get_prefix(uint32_t prefix, uchar *prelen, int data_len, char *buf)
{
    int data_num = 0;

    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct ldp_filter  *pfilter = NULL;
    struct ldp_ipprefix_mib *pfilter_mib = (struct ldp_ipprefix_mib *)buf;

    int msg_num  = IPC_MSG_LEN / sizeof(struct ldp_ipprefix_mib);
    int cursor;

    if (0 == prefix)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, ldp_filter_tabel)
        {
            pfilter = (struct ldp_filter *)pbucket->data;
            if (pfilter == NULL)
            {
                continue;
            }

            memcpy(&pfilter_mib[data_num], pfilter, sizeof(struct ldp_ipprefix_mib));

            if (++data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket =  hios_hash_find(&ldp_filter_tabel, &prefix);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&ldp_filter_tabel, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                pfilter = (struct ldp_filter *)pnext->data;
                memcpy(&pfilter_mib[data_num], pfilter, sizeof(struct ldp_ipprefix_mib));
                pbucket = pnext;
            }
        }
        else
        {

        }
    }

    return data_num;
}

/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_ldp_get_global_conf(uint32_t index, int data_len, char *buf)
{
    uint32_t data_num = 0;
    struct ldp_config_global_mib *pbuf = (struct ldp_config_global_mib *)buf;

    if (0 == data_len)
    {
        pbuf->gldp_enable = gmpls.ldp_enable;
        pbuf->advertise = gldp.advertise;
        pbuf->policy = gldp.policy;
        pbuf->control = gldp.control;
        pbuf->hold = gldp.hold;
        pbuf->hello_internal = gldp.hello_internal;
        pbuf->keepalive_internal = gldp.keepalive_internal;
        data_num ++;
    }
    return data_num;
}
/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_ldp_get_localif(uint32_t index, int data_len, char *buf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct mpls_if     *pif     = NULL;
    struct ldp_localif_mib *pldp_if = (struct ldp_localif_mib *)buf;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct ldp_localif_mib);
    int cursor;

    if (0 == index)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, mpls_if_table)
        {
            pif = (struct mpls_if *)pbucket->data;
            if ((pif == NULL) || (ENABLE != pif->ldp_conf.ldp_enable))
            {
                continue;
            }
            pldp_if[data_num].ifindex = pif->ifindex;
            pldp_if[data_num].ldp_enable = pif->ldp_conf.ldp_enable;

            if (++data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket =  hios_hash_find(&mpls_if_table, &index);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num;)
            {
                pnext = hios_hash_next_cursor(&mpls_if_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                pif = (struct mpls_if *)pnext->data;
                pbucket = pnext;
                if ((pif == NULL) || (ENABLE != pif->ldp_conf.ldp_enable))
                {
                    continue;
                }
                pldp_if[data_num].ifindex = pif->ifindex;
                pldp_if[data_num].ldp_enable = pif->ldp_conf.ldp_enable;
                data_num++;
            }
        }
        else
        {

        }
    }
    return data_num;

}
/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_get_remot_conf(uint32_t index, int data_len, char *buf)
{
    struct list         *plist      = &ldp_remot_adjance_list;
    struct listnode     *pnode      = NULL;
    struct listnode     *pnextnode  = NULL;
    struct ldp_adjance *padjance   = NULL;
    struct ldp_remoteconf_mib *premotconf_mib = (struct ldp_remoteconf_mib *)buf;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct ldp_remoteconf_mib);
    uint32_t repeat_flag = 0;

    if (0 == index)
    {
        for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
        {
            if (padjance->type == LDP_SESSION_REMOT)
            {
                premotconf_mib[data_num].remote_peer = padjance->peer_lsrid;
                if (++data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS(plist, pnode, pnextnode, padjance))
        {
            if (padjance->type == LDP_SESSION_REMOT)
            {
                if (0 == repeat_flag)
                {
                    if (padjance->peer_lsrid == index)
                    {
                        repeat_flag = 1;
                    }
                    continue;
                }
                premotconf_mib[data_num].remote_peer = padjance->peer_lsrid;
                if (++data_num == msg_num)
                {
                    return data_num;
                }
            }
        }
    }
    return data_num;
}
/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_ldp_get_sess(uint32_t index, int data_len, char *buf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct ldp_sess    *psess   = NULL;
    struct ldp_session_mib *psess_mib = (struct ldp_session_mib *)buf;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct ldp_session_mib);
    struct ldp_sess_key key;
    int cursor;

    if (0 == index)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, ldp_session_table)
        {
            psess = (struct ldp_sess *)pbucket->data;
            if (psess == NULL)
            {
                continue;
            }
            psess_mib[data_num].local_lsrid = psess->key.local_lsrid;
            psess_mib[data_num].peer_lsrid = psess->key.peer_lsrid;
            psess_mib[data_num].role = psess->role;
            psess_mib[data_num].status = (LDP_STATUS_UP == psess->status) ? 1 : 2; //{ operational ( 1 ) , initialized ( 2 ) }
            psess_mib[data_num].type = psess->type;
            psess_mib[data_num].creat_time = psess->creat_time;
            if (++data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        key.local_lsrid = gmpls.lsr_id;
        key.peer_lsrid = index;
        pbucket = hios_hash_find(&ldp_session_table, &key);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&ldp_session_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                psess = (struct ldp_sess *)pnext->data;
                psess_mib[data_num].local_lsrid = psess->key.local_lsrid;
                psess_mib[data_num].peer_lsrid  = psess->key.peer_lsrid;
                psess_mib[data_num].role = psess->role;
                psess_mib[data_num].status = psess->status;
                psess_mib[data_num].type = psess->type;
                psess_mib[data_num].creat_time = psess->creat_time;
                pbucket = pnext;
            }
        }
        else
        {

        }
    }
    return data_num;
}
/**
* @brief      <+MIB获取LDP配置+>
* @param[in ] <+none+>
* @param[out] <+none+>
* @return     <+data_num:数据条数+>
* @author     wus
* @date       2018/3/1
* @note       <+mib+>
*/
int mpls_ldp_get_lsp(uint32_t lspindex, int data_len, char *buf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct lsp_entry   *plsp    = NULL;
    struct ldp_lsp_mib *plsp_mib = (struct ldp_lsp_mib *)buf;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct ldp_lsp_mib);
    int cursor;

    if (0 == lspindex)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, lsp_table)
        {
            plsp = (struct lsp_entry *)pbucket->data;
            if ((plsp == NULL) || (LSP_TYPE_LDP != plsp->lsp_type))
            {
                continue;
            }
            plsp_mib[data_num].direction = plsp->direction;
            plsp_mib[data_num].destip = plsp->destip;
            plsp_mib[data_num].nexthop = plsp->nexthop;
            plsp_mib[data_num].inlabel = plsp->inlabel;
            plsp_mib[data_num].outlabel = plsp->outlabel;
            plsp_mib[data_num].lspindex = plsp->lsp_index;
            if (++data_num == msg_num)
            {
                MPLS_LOG_DEBUG("data_num %d\n", data_num);
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&lsp_table, &lspindex);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num;)
            {
                pnext = hios_hash_next_cursor(&lsp_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                plsp = (struct lsp_entry *)pnext->data;
                pbucket = pnext;
                if ((plsp == NULL) || (LSP_TYPE_LDP != plsp->lsp_type))
                {
                    continue;
                }
                plsp_mib[data_num].direction = plsp->direction;
                plsp_mib[data_num].destip = plsp->destip;
                plsp_mib[data_num].nexthop = plsp->nexthop;
                plsp_mib[data_num].inlabel = plsp->inlabel;
                plsp_mib[data_num].outlabel = plsp->outlabel;
                plsp_mib[data_num].lspindex = plsp->lsp_index;
                data_num++;
            }
        }
        else
        {

        }
    }
    MPLS_LOG_DEBUG("data_num %d\n", data_num);
    return data_num;
}


