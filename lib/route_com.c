/**
 * @file      : route_com.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 17:23:36
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/route_com.h>
#include <zebra.h>

/* add for route ipc common operation */

int route_ipc_msg_send(void *pdata, int data_len, int data_num, int module_dst, int module_src, 
                            int ipc_type, uint16_t sub_opcode, uint8_t opcode, int msg_index)
{
	int    ret = 0;

    struct ipc_mesg_n *pmesg = mem_share_malloc((sizeof(struct ipc_msghdr_n) + data_len), module_src);
    if(pmesg == NULL) 
	{
		return ERRNO_MALLOC;
    }

    memset(pmesg, 0, (sizeof(struct ipc_msghdr_n) + data_len));
 
    pmesg->msghdr.data_len      = data_len;
    pmesg->msghdr.module_id     = module_dst;
    pmesg->msghdr.sender_id     = module_src;
    pmesg->msghdr.msg_type      = ipc_type;
    pmesg->msghdr.opcode        = opcode;
    pmesg->msghdr.msg_subtype   = sub_opcode;
    pmesg->msghdr.msg_index     = msg_index;
    pmesg->msghdr.data_num      = data_num;

	if(pdata)
		memcpy(pmesg->msg_data, (uint8_t *)pdata, data_len);

    ret = ipc_send_msg_n1(pmesg, (sizeof(struct ipc_msghdr_n) + data_len));

    if(-1 == ret)	
    {
		mem_share_free(pmesg, module_src);
        return ERRNO_IPC;
    }
    return ERRNO_SUCCESS;
}

/****************************************/

/**
 * @brief      : 路由事件注册（IPv4）
 * @param[in ] : type      - 路由协议类型
 * @param[in ] : module_id - 注册者模块
 * @param[out] :
 * @return     : 成功返回 0 ，否则返回非 0
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:26:41
 * @note       :
 */
int route_event_register(enum ROUTE_PROTO type, int module_id)
{
    int family = INET_FAMILY_IPV4;

    return route_ipc_msg_send(&family, sizeof(int), 1, MODULE_ID_ROUTE,
                            module_id, IPC_TYPE_ROUTE, type, IPC_OPCODE_REGISTER, 0);
}


/**
 * @brief      : 路由事件取消注册（IPv4）
 * @param[in ] : type      - 取消注册的路由协议类型
 * @param[in ] : module_id - 取消注册者模块
 * @param[out] :
 * @return     : 成功返回 0 ，否则返回非 0
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:28:09
 * @note       :
 */
int route_event_unregister(enum ROUTE_PROTO type, int module_id)
{
    int family = INET_FAMILY_IPV4;

    return route_ipc_msg_send(&family, sizeof(int), 1, MODULE_ID_ROUTE,
                            module_id, IPC_TYPE_ROUTE, type, IPC_OPCODE_UNREGISTER, 0);
}


/**
 * @brief      : 路由事件注册（IPv6）
 * @param[in ] : type      - 路由协议类型
 * @param[in ] : module_id - 注册者模块
 * @param[out] :
 * @return     : 成功返回 0 ，否则返回非 0
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:28:58
 * @note       :
 */
int routev6_event_register(enum ROUTE_PROTO type, int module_id)
{
    int family = INET_FAMILY_IPV6;

    return route_ipc_msg_send(&family, sizeof(int), 1, MODULE_ID_ROUTE,
                            module_id, IPC_TYPE_ROUTE, type, IPC_OPCODE_REGISTER, 0);
}


/**
 * @brief      : 路由事件取消注册（IPv6）
 * @param[in ] : type      - 取消注册的路由协议类型
 * @param[in ] : module_id - 取消注册者模块
 * @param[out] :
 * @return     : 成功返回 0 ，否则返回非 0
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:29:13
 * @note       :
 */
int routev6_event_unregister(enum ROUTE_PROTO type, int module_id)
{
    int family = INET_FAMILY_IPV6;

    return route_ipc_msg_send(&family, sizeof(int), 1, MODULE_ID_ROUTE,
                            module_id, IPC_TYPE_ROUTE, type, IPC_OPCODE_UNREGISTER, 0);
}


/**
 * @brief      : 更新路由
 * @param[in ] : proute    - 活跃路由结构
 * @param[in ] : opcode    - 操作码
 * @param[in ] : module_id - 发送者模块
 * @param[out] :
 * @return     : 成功返回 0 ，否则返回非 0
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:29:28
 * @note       :
 */
int route_com_update(struct route_entry *proute, enum IPC_OPCODE opcode, int module_id)
{
    return route_ipc_msg_send(proute, sizeof(struct route_entry), 1, MODULE_ID_ROUTE, module_id,
						IPC_TYPE_ROUTE, 0, opcode, 0);
}


/**
 * @brief      : 获取 router-id （IPv4）
 * @param[in ] : module_id - 获取者模块
 * @param[out] :
 * @return     : 成功返回 router-id ，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:30:21
 * @note       :
 */
uint32_t *route_com_get_router_id(int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_ROUTE, module_id, IPC_TYPE_ROUTE,
                                        ROUTE_GET_ROUTER_ID, IPC_OPCODE_GET, 0, 0);
    if (NULL != pmesg)
    {
    	// pmesg->msghdr.data_num == 1
    	return (uint32_t *)pmesg->msg_data;
    }

    return NULL;

}


/**
 * @brief      : 获取 router-id （IPv6）
 * @param[in ] : module_id - 获取者模块
 * @param[out] :
 * @return     : 成功返回 router-id ，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:31:14
 * @note       :
 */
struct ipv6_addr *route_com_get_routerv6_id(int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_ROUTE, module_id, IPC_TYPE_ROUTE,
                                        ROUTE_GET_ROUTERV6_ID, IPC_OPCODE_GET, 0, 0);
    if (NULL != pmesg)
    {
    	// pmesg->msghdr.data_num == 1
    	return (struct ipv6_addr *)pmesg->msg_data;
    }
    return NULL;
}


/**
 * @brief      : 获取活跃路由
 * @param[in ] : pprefix   - 路由前缀
 * @param[in ] : vpn       - vpn 实例
 * @param[in ] : module_id - 获取者模块
 * @param[out] :
 * @return     : 成功返回活跃路由结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:31:36
 * @note       :
 */
struct route_entry * route_com_get_route(struct inet_prefix *pprefix, uint16_t vpn, int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pprefix, sizeof(struct inet_prefix), 1, MODULE_ID_ROUTE,
                                        module_id, IPC_TYPE_ROUTE, ROUTE_GET_ACTIVE_ROUTE,
                                        IPC_OPCODE_GET, vpn, 0);
    if (NULL != pmesg)
    {
    	// pmesg->msghdr.data_num == 1
    	return (struct route_entry *)pmesg->msg_data;
    }

    return NULL;
}


/**
 * @brief      : 批量获取静态路由
 * @param[in ] : proute    - 活跃路由结构
 * @param[in ] : module_id - 获取者模块
 * @param[out] : pdata_num - 获取到静态路由的数量
 * @return     : 成功返回静态路由结构指针（多个连续静态路由结构），否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:32:44
 * @note       :
 */
struct route_static *route_com_get_sroute_bulk(struct route_static *proute, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(proute, sizeof(struct route_static),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_ROUTE, ROUTE_GET_STATIC_ROUTE_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (struct route_static *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 获取 arp 全局配置
 * @param[in ] : module_id - 获取者模块
 * @param[out] :
 * @return     : 成功返回 arp 全局配置结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:35:36
 * @note       :
 */
struct arp_global *route_com_get_garp_conf(int module_id)
{
	struct ipc_mesg_n* pmesg = ipc_sync_send_n2(NULL,0, 1 , MODULE_ID_ROUTE, module_id,
                                IPC_TYPE_ARP, ARP_MIB_GLOBAL_CONF, IPC_OPCODE_GET, 0, 1);
	if (NULL != pmesg)
	{
		return (struct arp_global *)pmesg->msg_data;
	}
	return NULL;
}


/**
 * @brief      : 批量获取静态 arp
 * @param[in ] : ipaddr    - IP 地址
 * @param[in ] : vpn       - vpn 实例
 * @param[in ] : module_id - 获取者模块
 * @param[out] : pdata_num - 获取到静态 arp 的数量
 * @return     : 成功返回静态 arp 结构指针（多个连续静态 arp 结构），否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:36:15
 * @note       :
 */
struct arp_entry *route_com_get_sarp_bulk(uint32_t ipaddr, uint16_t vpn, int module_id, int *pdata_num)
{
    struct arp_key key;
	struct ipc_mesg_n *pmesg = NULL;

    key.ipaddr = ipaddr;
    key.vpnid  = vpn;

    pmesg = ipc_sync_send_n2(&key,sizeof(struct arp_key),1,MODULE_ID_ROUTE,module_id,IPC_TYPE_ARP,
                                        ARP_MIB_STATIC_ARP,IPC_OPCODE_GET_BULK,0,1);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct arp_entry *)(pmesg->msg_data);
    }

    return NULL;
}

struct ifm_arp *route_com_get_intf_arp_conf_bulk(int module_id, uint32_t ifindex, int *pdata_num)
{
       struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&ifindex,4, 1 , MODULE_ID_ROUTE, module_id,
                                              IPC_TYPE_ARP, ARP_MIB_INTF_ARP_CONF, IPC_OPCODE_GET_BULK, 0, 1);
       if (NULL != pmesg)
       {
              *pdata_num = pmesg->msghdr.data_num;
              return (struct ifm_arp *)pmesg->msg_data;
       }

       return NULL;
                                                                      
}

struct ftm_arp_count *route_com_get_arp_statis(int module_id, uint32_t ifindex, int *pdata_num)
{
	struct ipc_mesg_n* pmesg = ipc_sync_send_n2(NULL,0, 1 , MODULE_ID_FTM, module_id,
                                IPC_TYPE_ARP, ARP_INFO_CONF_STATIS, IPC_OPCODE_GET, 0, 1);
	if (NULL != pmesg)
	{
		return (struct ftm_arp_count *)pmesg->msg_data;
	}
	return NULL;

}                                                                      

int route_com_string_to_type(int afi, const char *s)
{
    if (s == NULL)
    	return -1;
	if (afi == AFI_IP)
	{

	    if (strncmp (s, "c", 1) == 0)
			return ROUTE_PROTO_CONNECT;
		else if (strncmp (s, "d", 1) == 0)//In order to adapt H3C:direct
			return ROUTE_PROTO_CONNECT;
	    else if (strncmp (s, "s", 1) == 0)
			return ROUTE_PROTO_STATIC;
	    else if (strncmp (s, "r", 1) == 0)
			return ROUTE_PROTO_RIP;
	    else if (strncmp (s, "o", 1) == 0)
			return ROUTE_PROTO_OSPF;
	    else if (strncmp (s, "is", 2) == 0)
			return ROUTE_PROTO_ISIS;
	    else if (strncmp (s, "ib", 2) == 0)
			return ROUTE_PROTO_IBGP;
		else if (strncmp (s, "eb", 2) == 0)
			return ROUTE_PROTO_EBGP;
	}

	if (afi == AFI_IP6)
	{
		if (strncmp (s, "c", 1) == 0)
			return ROUTE_PROTO_CONNECT;
		else if (strncmp (s, "d", 1) == 0)//In order to adapt H3C:direct
			return ROUTE_PROTO_CONNECT;
		else if (strncmp (s, "s", 1) == 0)
			return ROUTE_PROTO_STATIC;
		else if (strncmp (s, "r", 1) == 0)
			return ROUTE_PROTO_RIPNG;
		else if (strncmp (s, "o", 1) == 0)
			return ROUTE_PROTO_OSPF6;
		else if (strncmp (s, "is", 2) == 0)
			return ROUTE_PROTO_ISIS6;
		else if (strncmp (s, "ib", 2) == 0)
			return ROUTE_PROTO_IBGP6;
		else if (strncmp (s, "eb", 2) == 0)
			return ROUTE_PROTO_EBGP6;
	}


    return -1;
}


const char *route_com_type_to_string(unsigned int route_type)
{
    const char *str = NULL;

    switch ( route_type )
    {
        case ROUTE_PROTO_CONNECT:
            str = "connected";
            break;
        case ROUTE_PROTO_STATIC:
            str = "static";
            break;
        case ROUTE_PROTO_RIP:
            str = "rip";
            break;
		case ROUTE_PROTO_RIPNG:
            str = "ripng";
            break;
        case ROUTE_PROTO_OSPF:
            str = "ospf";
            break;
		case ROUTE_PROTO_OSPF6:
            str = "ospf6";
            break;
        case ROUTE_PROTO_ISIS:
		case ROUTE_PROTO_ISIS6:
            str = "isis";
            break;
        case ROUTE_PROTO_IBGP:
		case ROUTE_PROTO_IBGP6:
            str = "ibgp";
            break;
        case ROUTE_PROTO_EBGP:
		case ROUTE_PROTO_EBGP6:
            str = "ebgp";
            break;
        default :
            str = "Unkown route";
            break;
    }

	return str;
}


/* h3c route tree get for mib */


struct route_entry *route_com_get_route_active_bulk(struct route_entry *proute, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(proute, sizeof(struct route_entry),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_ROUTE, ROUTE_GET_ACTIVE_ROUTE,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (struct route_entry *)pmesg->msg_data;
	}

    return NULL;
}
struct route_count_mib *route_com_get_route_count_bulk(struct route_count_mib *pcount, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pcount, sizeof(struct route_count_mib),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_ROUTE, ROUTE_GET_RIB_COUNT,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct route_count_mib *)pmesg->msg_data;
    }

    return NULL;
}

struct route_rib_mib *route_com_get_route_rib_bulk(struct route_rib_mib *prib, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(prib, sizeof(struct route_rib_mib),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_ROUTE, ROUTE_GET_RIB_ROUTE,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct route_rib_mib *)pmesg->msg_data;
    }

    return NULL;
}

struct route_entry *route_com_get_route_active_brief_bulk(struct route_entry *proute, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(proute, sizeof(struct route_entry),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_ROUTE, ROUTE_GET_ACTIVE_ROUTE_BRIEF,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct route_entry *)pmesg->msg_data;
    }

    return NULL;
}

/**
 * @brief      : 获取 route 全局配置
 * @param[in ] : module_id - 获取者模块
 * @param[out] :
 * @return     : 成功返回 route 全局配置结构，否则返回 NULL
 * @author     : sunjl
 * @date       : 2018年08月01日 09:35:36
 * @note       :
 */
struct route_global *route_com_get_route_gconf(int module_id)
{
    struct ipc_mesg_n *pmesg =  ipc_sync_send_n2(NULL,0, 1 , MODULE_ID_ROUTE, module_id,
                                IPC_TYPE_ROUTE, ROUTE_GET_ROUTE_GCONF, IPC_OPCODE_GET, 0,0);
	
    if (NULL != pmesg)
    {
        return (struct route_global *)pmesg->msg_data;
    }

	return NULL;
}


