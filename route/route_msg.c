/**
 * @file      : route_msg.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 16:21:56
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#define _GNU_SOURCE
#include <sched.h>
#include <sys/prctl.h>
#include <sys/msg.h>
#include <lib/prefix.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>
#include "route_main.h"
#include "route_if.h"
#include "route_register.h"
#include "route_static.h"
#include "route_msg.h"
#include "route_cmd.h"
#include "arp_static.h"


/**
 * @brief      : route 模块向 ftm 发送路由消息线程回调函数
 * @param[in ] : arg - 线程参数
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:26:16
 * @note       : 发送的消息先存入本地链表，然后根据队列状态决定是否发送消息
 */
void route_msg_send_thread_callback(void *arg)
{
    struct list             *plist  = NULL;
    struct listnode         *pnode  = NULL;
    struct ipc_mesg_n       *prmesg = NULL;
    struct ipc_msghdr_n     *phdr   = NULL;
    char buffer[IPC_MSG_LEN_N];
    struct ipc_mesg_n       *pmesg_temp = NULL;;
    struct msqid_ds qbuf;
    uint32_t max_num   = 2000;
    uint32_t free_byte = 50000;
    int      count     = 10;
    int ret;
    struct sched_param param;
    cpu_set_t mask;

    prctl(PR_SET_NAME, "send_route");

    /* 绑定 cpu1，并设定线程优先级为 50 */
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    param.sched_priority = 50;
    if (sched_setscheduler(0, SCHED_RR, &param))
    {
        perror("\n  priority set: ");
    }

    while(1)
    {
        count = 10;

        while (count-- > 0)
        {
            /*if ((qbuf.msg_qnum < max_num) && (qbuf.msg_qbytes - qbuf.msg_cbytes > free_byte)) */
            {
                plist = &g_route.route_send_route_list;

                ROUTE_SEND_LOCK
                if (list_isempty(plist))
                {
                    ROUTE_SEND_UNLOCK
                    continue;
                }

                memset(buffer, 0, IPC_MSG_LEN_N);
                pnode = listhead(plist);
                if ((NULL == pnode) || (NULL == pnode->data))
                {
                    ROUTE_SEND_UNLOCK;
                    continue;
                }

                prmesg = pnode->data;
                memcpy(buffer, prmesg, (prmesg->msghdr.data_len + IPC_HEADER_LEN_N));
                
                list_delete_node(plist, pnode);
                XFREE(MTYPE_ROUTE_THREAD, prmesg);
                ROUTE_SEND_UNLOCK;

                pmesg_temp = (struct ipc_mesg_n *)buffer;
                phdr = &pmesg_temp->msghdr;

                ROUTE_IPC_SENDTO_FTM(pmesg_temp->msg_data, phdr->data_len, 1, phdr->module_id, phdr->sender_id,
                            phdr->msg_type, phdr->msg_subtype, phdr->opcode, phdr->msg_index);

            }/*
            else
            {
                break;
            }*/
        }

        usleep(1000);
    }

    return;
}


/**
 * @brief      : route 模块发送路由消息至 ftm
 * @param[in ] : pdata     - 发送的数据指针
 * @param[in ] : data_len  - 发送的数据长度
 * @param[in ] : data_num  - 发送的消息数目
 * @param[in ] : module_id - 发送的目的模块
 * @param[in ] : sender_id - 发送者模块
 * @param[in ] : msg_type  - 发送的消息类型
 * @param[in ] : subtype   - 发送的消息子类型
 * @param[in ] : opcode    - 发送的消息操作码
 * @param[in ] : msg_index - 附加索引信息
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:27:37
 * @note       : 将消息存入本地链表，然后再使用线程发送
 */
void route_add_msg_to_send_list(void *pdata, int data_len, int data_num, int module_id,
                        int sender_id, enum IPC_TYPE msg_type, uint8_t subtype,
                        enum IPC_OPCODE opcode, uint32_t msg_index)
{
    struct ipc_mesg_n       *prmesg = NULL;
    struct ipc_msghdr_n     *phdr   = NULL;

    prmesg = (struct ipc_mesg_n *)XCALLOC(MTYPE_ROUTE_THREAD, (data_len + IPC_HEADER_LEN_N));
    if (NULL == prmesg)
    {
        ROUTE_LOG_ERROR("route msg malloc failed\n");

        return;
    }

    if ((pdata != NULL) && (data_len != 0))
    {
        memcpy(prmesg->msg_data, pdata, data_len);
    }

    phdr              = &prmesg->msghdr;
    phdr->data_len    = data_len;
	phdr->module_id   = module_id;
	phdr->msg_type    = msg_type;
	phdr->msg_subtype = subtype;
	phdr->msg_index   = msg_index;
	phdr->data_num    = data_num;
	phdr->opcode      = opcode;
	phdr->sender_id   = sender_id;

    ROUTE_SEND_LOCK
    listnode_add(&g_route.route_send_route_list, prmesg);
    ROUTE_SEND_UNLOCK

    return;
}



/**
 * @brief      : route 模块处理接收的路由消息
 * @param[in ] : phdr - 消息头
 * @param[in ] : data - 接收的数据指针
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:32:38
 * @note       :
 */
static void route_msg_process_route(struct ipc_msghdr_n *phdr, void *data)
{
    struct route_entry *proute = NULL;
    int family = INET_FAMILY_INVALID;
    int i;
    int j;

    for (i=0; i<phdr->data_num; i++)
    {
        if (phdr->opcode == IPC_OPCODE_REGISTER)
        {
            family = *(int *)data;

            if (family == INET_FAMILY_IPV4)
            {
                if (ERRNO_SUCCESS == route_register_msg(phdr->sender_id, phdr->msg_subtype))
                {
                    route_register_notify(phdr->sender_id, phdr->msg_subtype);
                }
            }
            else if (family == INET_FAMILY_IPV6)
            {
                routev6_register_msg(phdr->sender_id, phdr->msg_subtype);
                routev6_register_notify(phdr->sender_id, phdr->msg_subtype);
            }

            /* 注册消息只处理单条 */
            break;
        }
        else if (phdr->opcode == IPC_OPCODE_UNREGISTER)
        {
            family = *(int *)data;

            if (family == INET_FAMILY_IPV4)
            {
                route_unregister_msg(phdr->sender_id, phdr->msg_subtype);
            }
            else if (family == INET_FAMILY_IPV6)
            {
                routev6_unregister_msg(phdr->sender_id, phdr->msg_subtype);
            }

            /* 取消注册消息只处理单条 */
            break;
        }
        else if (phdr->opcode == IPC_OPCODE_ADD)
        {
            proute = (struct route_entry *)data;
            proute += i;

            if (proute->nhp[0].protocol == ROUTE_PROTO_STATIC)
            {
                struct route_static sroute;
                memset(&sroute, 0, sizeof(struct route_static));

                sroute.prefix           = proute->prefix;
                sroute.prefix.addr.ipv4 = ipv4_network_addr(ntohl(sroute.prefix.addr.ipv4), sroute.prefix.prefixlen);
                sroute.prefix.addr.ipv4 = ntohl(sroute.prefix.addr.ipv4);
                sroute.nexthop          = proute->nhp[0].nexthop;
                sroute.nhp_type         = NHP_TYPE_IP;
                sroute.action           = NHP_ACTION_FORWARD;
                sroute.distance         = ROUTE_METRIC_STATIC;
                sroute.down_flag        = LINK_DOWN;

                route_static_add(&sroute);
            }
            else
            {
                if (proute->prefix.type == INET_FAMILY_IPV6)
                {
                    for (j=0; j<proute->nhp_num; j++)
                    {
                        ribv6_add(&proute->prefix, proute->vpn, &proute->nhp[j]);
                    }
                }
                else
                {
                    for (j=0; j<proute->nhp_num; j++)
                    {
                        rib_add(&proute->prefix, proute->vpn, &proute->nhp[j]);
                    }
                }
            }
        }
        else if (phdr->opcode == IPC_OPCODE_DELETE)
        {
            proute = (struct route_entry *)data;
            proute += i;

            if (proute->nhp[0].protocol == ROUTE_PROTO_STATIC)
            {
                struct route_static sroute;
                memset(&sroute, 0, sizeof(struct route_static));

                sroute.prefix           = proute->prefix;
                sroute.prefix.addr.ipv4 = ipv4_network_addr(ntohl(sroute.prefix.addr.ipv4), sroute.prefix.prefixlen);
                sroute.prefix.addr.ipv4 = ntohl(sroute.prefix.addr.ipv4);
                sroute.nexthop          = proute->nhp[0].nexthop;
                sroute.nhp_type         = NHP_TYPE_IP;
                sroute.action           = NHP_ACTION_FORWARD;
                sroute.distance         = ROUTE_METRIC_STATIC;
                sroute.down_flag        = LINK_DOWN;

                route_static_delete(&sroute);
            }
            else
            {
                if (proute->prefix.type == INET_FAMILY_IPV6)
                {
                    for (j=0; j<proute->nhp_num; j++)
                    {
                        ribv6_delete(&proute->prefix, proute->vpn, &proute->nhp[j]);
                    }
                }
                else
                {
                    for (j=0; j<proute->nhp_num; j++)
                    {
                        rib_delete(&proute->prefix, proute->vpn, &proute->nhp[j]);
                    }
                }
            }
        }
        else if (phdr->opcode == IPC_OPCODE_UPDATE)
        {
            proute = (struct route_entry *)data;
            proute += i;

            if (proute->prefix.type == INET_FAMILY_IPV6)
            {
                for (j=0; j<proute->nhp_num; j++)
                {
                    ribv6_update(&proute->prefix, proute->vpn, &proute->nhp[j]);
                }
            }
            else
            {
                for (j=0; j<proute->nhp_num; j++)
                {
                    rib_update(&proute->prefix, proute->vpn, &proute->nhp[j]);
                }
            }
        }
    }

    return;
}

static int send_routerid_to_bgp(void *pdata, struct ipc_msghdr_n *phdr)
{

	route_set_router_id();
	if(g_route.router_id != 0 )
	{
		return ipc_send_reply_n2(&g_route.router_id, 4, 1, MODULE_ID_BGP, MODULE_ID_ROUTE,
							IPC_TYPE_BGP, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
	}
	
	ROUTE_LOG_ERROR("router-id send to bgp error\n");
	return -1;
}


/**
 * @brief      : route 模块处理接收的公共消息
 * @param[in ] : pdata - 接收的数据指针
 * @param[in ] : phdr  - 消息头
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:36:07
 * @note       :
 */
static int route_msg_rcv_ifevent_ifm(void *pdata, struct ipc_msghdr_n *phdr)
{
    struct ifm_event *pevent = NULL;
    struct route_if lif;
    uint32_t ifindex;

    memset(&lif, 0, sizeof(struct route_if));

    if (phdr->opcode == IPC_OPCODE_REGISTER)
    {
    	if(phdr->msg_index <= 7)
        {
        	l3if_register_msg(phdr->sender_id, phdr->msg_subtype);
    	}
		else
		{
			l3if_register_msg_new(phdr->sender_id, phdr->msg_subtype,phdr->msg_index);
		}
    }
	
    if (phdr->opcode == IPC_OPCODE_UNREGISTER)
    {
        l3if_unregister_msg(phdr->sender_id, phdr->msg_subtype);
    }
	
    else if (phdr->opcode == IPC_OPCODE_EVENT)
    {
        pevent      = (struct ifm_event *)pdata;
        ifindex     = pevent->ifindex;
        lif.ifindex = ifindex;

        if (phdr->msg_subtype == IFNET_EVENT_DOWN)
        {
            ROUTE_LOG_DEBUG("recv event notify ifindex 0x%0x down\n", ifindex);

            /* 忽略子接口 down 事件 */
            if ((IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex))
                || (IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex)))
            {
                return ERRNO_SUCCESS;
            }

            route_if_down(ifindex);
        }
        else if (phdr->msg_subtype == IFNET_EVENT_UP)
        {
            ROUTE_LOG_DEBUG("recv event notify ifindex 0x%0x up\n", ifindex);

            /* 忽略子接口 up 事件 */
            if ((IFM_TYPE_IS_METHERNET(ifindex) && IFM_IS_SUBPORT(ifindex))
                || (IFM_TYPE_IS_TRUNK(ifindex) && IFM_IS_SUBPORT(ifindex)))
            {
                return ERRNO_SUCCESS;
            }

            route_if_up(ifindex);
        }
        else if (phdr->msg_subtype == IFNET_EVENT_IF_DELETE)
        {
            ROUTE_LOG_DEBUG("recv event notify ifindex 0x%0x delete\n", ifindex);

            arp_static_process_l3if_del(ifindex);//l3if 删除，删除接口下创建的静态arp

            route_if_delete_config(&lif);
        }
        else if (phdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
        {
            ROUTE_LOG_DEBUG("recv event notify ifindex 0x%0x change mode %d\n", ifindex, pevent->mode);

            if (pevent->mode != IFNET_MODE_L3)
            {
                if ((IFM_TYPE_IS_METHERNET(ifindex) && !IFM_IS_SUBPORT(ifindex))
                    || (IFM_TYPE_IS_TRUNK(ifindex) && !IFM_IS_SUBPORT(ifindex)))
                {
                    route_if_process_subif_delete(ifindex);
                }

                arp_static_process_l3if_del(ifindex);
                route_if_delete_config(&lif);
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 处理三层接口消息
 * @param[in ] : pdata - 接收数据指针
 * @param[in ] : phdr  - 消息头
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:41:52
 * @note       :
 */
static int route_msg_rcv_ifevent_l3if(void *pdata, struct ipc_msghdr_n *phdr)
{
    struct route_if *pif = NULL;
    struct route_if  lif;
    int msg_num = IPC_MSG_LEN/sizeof(struct ifm_l3);
    struct ifm_l3  l3if[msg_num];
    struct ifm_l3 *pl3if = NULL;
    uint32_t ifindex;
    int ret = 0;
    int errcode = ERRNO_NOT_FOUND;

    memset(&lif, 0, sizeof(struct route_if));
    memset(l3if, 0, msg_num*sizeof(struct ifm_l3));

    ifindex = phdr->msg_index;
    if (phdr->opcode == IPC_OPCODE_GET)
    {
        pif = route_if_lookup(ifindex);
        if (NULL == pif)
        {
            ret = ipc_send_reply_n2(&errcode, 4, 1, phdr->sender_id, MODULE_ID_ROUTE,
                                IPC_TYPE_L3IF, phdr->msg_subtype, 1, ifindex, IPC_OPCODE_NACK);
        }
        else
        {
            ret = ipc_send_reply_n2(&(pif->intf), sizeof(struct ifm_l3), 1, phdr->sender_id,
                                MODULE_ID_ROUTE, IPC_TYPE_L3IF, phdr->msg_subtype, 1, ifindex, IPC_OPCODE_REPLY);
        }
    }
    else if (phdr->opcode == IPC_OPCODE_GET_BULK)
    {
        ret = route_if_get_bulk(ifindex, l3if);
        if (ret > 0)
        {
            ret = ipc_send_reply_n2(l3if, ret*sizeof(struct ifm_l3), ret, phdr->sender_id,
                                    MODULE_ID_ROUTE, IPC_TYPE_L3IF, phdr->msg_subtype, 1, ifindex, IPC_OPCODE_REPLY);
        }
        else
        {
            ret = ipc_send_reply_n2(&errcode, 4, 1, phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_L3IF,
                                    phdr->msg_subtype, 1, ifindex, IPC_OPCODE_NACK);
        }
    }
    else if (phdr->opcode == IPC_OPCODE_EVENT)
    {
        pl3if = (struct ifm_l3 *)pdata;
        lif.ifindex = ifindex;

        if (phdr->msg_subtype == IFNET_EVENT_IP_ADD)
        {
            if (pl3if != NULL)
            {
                memcpy(&lif.intf, pl3if, sizeof(struct ifm_l3));

                if (ROUTE_IF_IP_EXIST(lif.intf.ipv4[0].addr))
                {
                    ret = route_if_check_ip_conflict(&lif);
                    if ((ret == ERRNO_EXISTED_IP)
                        || (ret == ERRNO_CONFLICT_IP)
                        || (ret == ERRNO_SUBNET_SIMILAR))
                    {
                        ret = ipc_send_reply_n2(&ret, 4, 1, phdr->sender_id, MODULE_ID_ROUTE,
                                        IPC_TYPE_L3IF, phdr->msg_subtype, 1, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        //ret = ipc_send_ack(phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_L3IF, phdr->msg_subtype, ifindex);

                        route_if_add_ip(&lif);
                    }
                }
                else if (ROUTE_IF_IPV6_EXIST(lif.intf.ipv6[0].addr))
                {
                    ret = route_if_check_v6_ip_conflict(&lif);
                    if ((ret == ERRNO_EXISTED_IP)
                        || (ret == ERRNO_CONFLICT_IP)
                        || (ret == ERRNO_SUBNET_SIMILAR))
                    {
                        ret = ipc_send_reply_n2(&ret, 4, 1, phdr->sender_id, MODULE_ID_ROUTE,
                                        IPC_TYPE_L3IF, phdr->msg_subtype, 1, ifindex, IPC_OPCODE_REPLY);
                    }
                    else
                    {
                        //ret = ipc_send_ack(phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_L3IF, phdr->msg_subtype, ifindex);

                        route_if_add_v6_ip(&lif);
                    }
                }
                else
                {
                    //ret = ipc_send_ack(phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_L3IF, phdr->msg_subtype, ifindex);

                    route_if_add_ip(&lif);
                }
            }
        }
        else if (phdr->msg_subtype == IFNET_EVENT_IP_DELETE)
        {
            pif = route_if_lookup(ifindex);
            if (NULL != pif)
            {
            	if(phdr->sender_id == MODULE_ID_DHCPV6)
                    route_if_delete_v6_ip(pif);
                else
                    route_if_delete_ip(pif);

                if ((pl3if != NULL) && (phdr->sender_id == MODULE_ID_DHCP || phdr->sender_id == MODULE_ID_DHCPV6 ))
                {
                    /* 接收 dhcp 删除 ip 消息，保留标志 */
                    if (phdr->sender_id == MODULE_ID_DHCP)
                    {
                        pif->intf.ipv4_flag = pl3if->ipv4_flag;
                    }

                    if (phdr->sender_id == MODULE_ID_DHCPV6)
                    {
                        pif->intf.ipv6_flag = pl3if->ipv6_flag;
                    }
                }
            }
        }
    }

    return ret;
}


/**
 * @brief      : 处理路由获取消息
 * @param[in ] : pdata - 接收数据指针
 * @param[in ] : phdr  - 消息头
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:43:31
 * @note       :
 */
static int route_msg_rcv_ifevent_route(void *pdata, struct ipc_msghdr_n *phdr)
{
    struct route_entry *proute = NULL;
    struct inet_prefix *pprefix = NULL;
    int msg_num = IPC_MSG_LEN/sizeof(struct route_static);
    struct route_static *psroute = NULL;
    struct route_static sroute[msg_num];
    uint16_t vpn = phdr->msg_index;
    int ret = 0;
    int errcode = ERRNO_NOT_FOUND;
    memset(sroute, 0, msg_num*sizeof(struct route_static));

    char buf[IPC_MSG_LEN];
    memset(buf, 0, IPC_MSG_LEN);

    if (phdr->opcode == IPC_OPCODE_GET)
    {
        if (ROUTE_GET_ROUTER_ID == phdr->msg_subtype)
        {
            route_set_router_id();
            ret = ipc_send_reply_n2(&g_route.router_id, IP_ADDR_LEN, 1, phdr->sender_id, MODULE_ID_ROUTE,
                                IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
        }
        else if (ROUTE_GET_ROUTERV6_ID == phdr->msg_subtype)
        {
            routev6_set_router_id();
            ret = ipc_send_reply_n2(&g_route.routerv6_id.ipv6, IPV6_ADDR_LEN, 1,  phdr->sender_id,
                                MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
        }
        else if (ROUTE_GET_ACTIVE_ROUTE == phdr->msg_subtype)
        {
            pprefix = (struct inet_prefix *)pdata;
            pprefix->prefixlen = 32;

            proute = route_match(pprefix, vpn);
            if (NULL != proute)
            {
                ret = ipc_send_reply_n2(proute, sizeof(struct route_entry), 1, phdr->sender_id,
                                    MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                errcode = 1;
                ret = ipc_send_reply_n2(&errcode, 4, 1, phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_ROUTE,
                                phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_NACK);
            }
        }
    }
    else if (phdr->opcode == IPC_OPCODE_GET_BULK)
    {
        if(ROUTE_GET_STATIC_ROUTE_BULK == phdr->msg_subtype)
        {
            psroute = (struct route_static *)pdata;

            ret = route_static_get_bulk(psroute, sroute, phdr->msg_index);
            if (ret > 0)
            {
                ret = ipc_send_reply_n2(sroute, ret*sizeof(struct route_static), ret, phdr->sender_id,
                                        MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                ipc_noack_to_snmp(phdr);
            }
        }
        else if(ROUTE_GET_RIB_ROUTE == phdr->msg_subtype)
        {
            /* mib get rib route tree */
            ret = route_rib_get_bulk_for_mib((struct route_rib_mib *)pdata, (struct route_rib_mib *)buf, phdr->msg_index);
            if (ret > 0)
            {
                ret = ipc_send_reply_n2(buf, ret*sizeof(struct route_rib_mib), ret, phdr->sender_id,
                                        MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                ipc_noack_to_snmp(phdr);
            }
        }
        else if(ROUTE_GET_ACTIVE_ROUTE == phdr->msg_subtype)
        {
            /* mib get active route tree */
            ret = route_active_get_bulk_for_mib((struct route_entry *)pdata, (struct route_entry *)buf, phdr->msg_index);
            if (ret > 0)
            {
                ret = ipc_send_reply_n2(buf, ret*sizeof(struct route_entry), ret, phdr->sender_id,
                                        MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1, phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                ipc_noack_to_snmp(phdr);
            }

        }
        else if(ROUTE_GET_RIB_COUNT == phdr->msg_subtype)
        {
            /* mib get route count */
            ret = route_rib_get_count_for_mib((struct route_count_mib *)buf, phdr->msg_index);
            if (ret > 0)
            {
                ret = ipc_send_reply_n2(buf, ret*sizeof(struct route_count_mib), ret, phdr->sender_id,
                                        MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1,phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                ipc_noack_to_snmp(phdr);
            }

        }
        else if(ROUTE_GET_ACTIVE_ROUTE_BRIEF == phdr->msg_subtype)
        {
            /* add for mib rfc 1213 */
            ret = route_active_get_bulk_for_mib_brief((struct route_entry *)pdata, (struct route_entry *)buf, phdr->msg_index);
            if (ret > 0)
            {
                ret = ipc_send_reply_n2(buf, ret*sizeof(struct route_entry), ret, phdr->sender_id,
                                        MODULE_ID_ROUTE, IPC_TYPE_ROUTE, phdr->msg_subtype, 1,phdr->msg_index, IPC_OPCODE_REPLY);
            }
            else
            {
                ipc_noack_to_snmp(phdr);
            }

        }
    }
	else
	{
		route_msg_process_route(phdr,pdata);
	}
	
    return ret;
}


/**
 * @brief      : 处理 l3vpn 实例删除消息
 * @param[in ] : pdata - 接收数据指针
 * @param[in ] : phdr  - 消息头
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:45:08
 * @note       :
 */
static int route_msg_rcv_l3vpn(void *pdata, struct ipc_msghdr_n *phdr)
{
    struct l3vpn_entry *pl3vpn = NULL;

    pl3vpn = (struct l3vpn_entry *)pdata;

    if (phdr->opcode == IPC_OPCODE_DELETE)
    {
        route_if_process_l3vpn_delete(pl3vpn->l3vpn.vrf_id);
        route_static_process_l3vpn_delete(pl3vpn->l3vpn.vrf_id);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 处理 arp 消息
 * @param[in ] : pdata - 接收数据指针
 * @param[in ] : phdr  - 消息头
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:46:02
 * @note       :
 */
static int route_msg_rcv_arp(void *pdata, struct ipc_msghdr_n *phdr)
{
    uint32_t msg_len = IPC_MSG_LEN/sizeof(struct arp_entry);
    struct arp_entry parp[msg_len];
    struct arp_key   *key = NULL;
    uint32_t num = 0;

	uint32_t cfg_len = IPC_MSG_LEN / sizeof(struct ifm_arp);
	struct ifm_arp intf_arp_cfg[cfg_len];
	struct ifm_arp *intf_arp = NULL;
	uint32_t ifindex;
    ROUTE_LOG_DEBUG("phdr->msg_subtype:%d phdr->opcode:%d\n", phdr->msg_subtype, phdr->opcode);

    if(phdr->msg_subtype == ARP_MIB_GLOBAL_CONF)
    {
        if(phdr->opcode == IPC_OPCODE_GET)
        {
            ipc_send_reply_n2(&g_arp, sizeof(struct arp_global), 1, phdr->sender_id,
                            MODULE_ID_ROUTE, IPC_TYPE_ARP, ARP_MIB_GLOBAL_CONF, 1, 0, IPC_OPCODE_REPLY);
        }
    }
    else if(phdr->msg_subtype == ARP_MIB_STATIC_ARP)
    {
        if (phdr->opcode == IPC_OPCODE_GET_BULK)
        {
            memset(parp, 0, msg_len*sizeof(struct arp_entry));
            key = (struct arp_key *)pdata;
            num = arp_static_get_bulk(key, parp);

            if(num > 0 || num == 0)
            {
                ipc_send_reply_n2(parp, num*sizeof(struct arp_entry), num,
                                    phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_ARP,
                                    ARP_MIB_STATIC_ARP, 1, 0, IPC_OPCODE_REPLY);
            }
        }
    }
	else if (phdr->msg_subtype == ARP_MIB_INTF_ARP_CONF)
	{
		if(phdr->opcode == IPC_OPCODE_GET_BULK)
		{
            memset(intf_arp_cfg, 0, msg_len*sizeof(struct ifm_arp));
			ifindex = *((uint32_t *)pdata);
            num = arp_static_intf_arp_conf(ifindex, intf_arp_cfg);

            if(num > 0 || num == 0)
            {
                ipc_send_reply_n2(intf_arp_cfg, num*sizeof(struct ifm_arp), num,
                                    phdr->sender_id, MODULE_ID_ROUTE, IPC_TYPE_ARP,
                                    ARP_MIB_INTF_ARP_CONF, 1, 0, IPC_OPCODE_REPLY);
            }
        }
    }

    return ERRNO_SUCCESS;
}




/**
 * @brief      : 从本地链表读取公共消息执行
 * @param[in ] : pmesg - IPC 消息结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:46:23
 * @note       :
 */
int route_ipc_msg_callback(struct ipc_mesg_n *pmesg, int imlen)
{
    struct list     *plist = &g_route.route_ipc_msg_list;
    struct listnode *pnode = NULL;
    struct listnode *pnext = NULL;
    struct ipc_mesg_n *pdata = NULL;
    struct ipc_mesg_n *pcache = NULL;
    int repeat_flag = DISABLE;

    uint32_t msg_len = pmesg->msghdr.data_len + IPC_HEADER_LEN_N;

    if(msg_len <= imlen)
    {
        ROUTE_MSG_LOCK
        for (ALL_LIST_ELEMENTS(plist, pnode, pnext, pdata))
        {
            if (0 == memcmp(pdata, pmesg, (pdata->msghdr.data_len + IPC_HEADER_LEN_N)))
            {
                listnode_move_to_tail(plist, pnode);
        
                repeat_flag = ENABLE;
        
                break;
            }
        }
        
        if (DISABLE == repeat_flag)
        {
            pcache = (struct ipc_mesg_n *)XCALLOC(MTYPE_ROUTE_THREAD, msg_len);
            if (NULL == pcache)
            {
                ROUTE_LOG_ERROR("The system run out of memory !\n");
        
                ROUTE_MSG_UNLOCK
                return ERRNO_MALLOC;
            }
        
            memcpy(pcache, pmesg, msg_len);
            listnode_add(plist, pcache);
        }
        
        repeat_flag = DISABLE;
        ROUTE_MSG_UNLOCK
    }
    else
    {
        ROUTE_LOG_ERROR("The system run out of memory !\n");
    }

    mem_share_free(pmesg, MODULE_ID_ROUTE);
    
    return ERRNO_SUCCESS;
}






/**
 * @brief      : 从本地链表读取公共消息执行
 * @param[in ] : pmesg - IPC 消息结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:46:23
 * @note       :
 */
int route_ipc_msg_process(struct ipc_mesg_n *pmesg, int imlen)
{
    struct ipc_msghdr_n *phdr           = NULL;
   // struct listnode     *pnode          = NULL;
  //  struct ipc_mesg_n   *pmesg_lookup   = NULL;
  //struct list         *plist          = &g_route.route_ipc_msg_list;
    void                *pdata          = NULL;
    int ret;
   // int token = 1;

    /* æ¯æ¬¡æŽ¥æ”¶ 1 ä¸ªæ¶ˆæ? */
   // while(token)
   // {
    //    token--;
#if 0
        ROUTE_MSG_LOCK
        if (list_isempty(plist))
        {
            ROUTE_MSG_UNLOCK
            continue;
        }

        pnode = listhead(plist);
        if ((NULL == pnode) || (NULL == pnode->data))
        {
            ROUTE_MSG_UNLOCK
            continue;
        }

        pmesg_lookup = pnode->data;
        memcpy(pmesg, pmesg_lookup, (pmesg_lookup->msghdr.data_len + IPC_HEADER_LEN_N));
        list_delete_node(plist, pnode);
        XFREE(MTYPE_ROUTE_THREAD, pmesg_lookup);
        ROUTE_MSG_UNLOCK
#endif
        phdr  = &(pmesg->msghdr);
        pdata = pmesg->msg_data;

        ROUTE_LOG_DEBUG("opcode = %d, msg_type = %d subtype = %d\n", phdr->opcode, phdr->msg_type, phdr->msg_subtype);

        if (phdr->msg_type == IPC_TYPE_IFM)
        {
            ret = route_msg_rcv_ifevent_ifm(pdata, phdr);
        }
        else if (phdr->msg_type == IPC_TYPE_L3IF)
        {
            ret = route_msg_rcv_ifevent_l3if(pdata, phdr);
        }
        else if (phdr->msg_type == IPC_TYPE_ROUTE)
        {
            ret = route_msg_rcv_ifevent_route(pdata, phdr);
        }
        else if (phdr->msg_type == IPC_TYPE_ARP)
        {
            ret = route_msg_rcv_arp(pdata, phdr);
        }
        else if (phdr->msg_type == IPC_TYPE_L3VPN)
        {
            ret = route_msg_rcv_l3vpn(pdata, phdr);
        }
		else if(phdr->msg_type == IPC_TYPE_BGP)
        {
			ret = send_routerid_to_bgp(pdata,phdr);
		}
		else
		{
			zlog_err("route: unknown message get!");
		}
   // }
    mem_share_free(pmesg, MODULE_ID_ROUTE);

    return ret;
}


/**
 * @brief      : route 模块消息接收
 * @param[in ] : thread - 线程指针
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:47:29
 * @note       :
 */
int route_msg_rcv(struct thread *thread)
{
    char buffer[IPC_MSG_MAXLEN];
    
    route_ipc_msg_process((struct ipc_mesg_n *)buffer, 0);
    
    //thread_add_event(route_master, route_msg_rcv, NULL, 0);

    return ERRNO_SUCCESS;
}


