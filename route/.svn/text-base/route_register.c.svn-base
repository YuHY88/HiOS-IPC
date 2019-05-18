/**
 * @file      : route_register.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月27日 17:12:46
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <string.h>
#include <lib/zassert.h>

#include <lib/log.h>
#include <lib/memory.h>

#include <lib/errcode.h>
#include "route_register.h"
#include "route_main.h"


static struct list *l3if_event_list[IFNET_EVENT_MAX];
static struct list *route_event_list[ROUTE_PROTO_MAX];
static struct list *routev6_event_list[ROUTE_PROTO_MAX];


int l3if_unregister_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;
	struct listnode *pnode = NULL;
	struct listnode *pnode_next = NULL;
	struct l3if_register *if_register = NULL;

    if (subtype > IFNET_EVENT_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = l3if_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_FAIL;
    }

	for (ALL_LIST_ELEMENTS(plist, pnode, pnode_next, if_register))
	{
		if(if_register->module_id == module_id)
		{
			XFREE(0,if_register);
			listnode_delete(plist, if_register);
            return ERRNO_SUCCESS;
		}
	}

    return ERRNO_SUCCESS;
}

/**
 * @brief      : 三层接口信息变化注册
 * @param[in ] : module_id - 注册者模块名
 * @param[in ] : subtype   - 注册的事件类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:51:12
 * @note       :
 */
int l3if_register_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;
	struct listnode *pnode = NULL;
	struct l3if_register *if_register = NULL;

    if (subtype > IFNET_EVENT_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = l3if_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_FAIL;
    }

	for (ALL_LIST_ELEMENTS_RO(plist, pnode, if_register))
	{
		if(if_register->module_id == module_id)
		{
			if(if_register->ifindex != 0)
			{
				if_register->ifindex = 0;
			}
			return ERRNO_SUCCESS;
		}
	}

	if_register = (struct l3if_register *)XCALLOC(0, sizeof(struct l3if_register) );
	
	if(if_register == NULL)
	{
		ROUTE_LOG_DEBUG("malloc error!\n");
		return ERRNO_FAIL;
	}

	if_register->module_id = module_id;
	if_register->ifindex = 0;
	listnode_add(plist, if_register);

    return ERRNO_SUCCESS;
}


int l3if_register_msg_new(int module_id, uint8_t subtype ,u_int32 ifindex)
{
    struct list *plist = NULL;
	struct listnode *pnode = NULL;
	struct l3if_register *if_register = NULL;

    if (subtype > IFNET_EVENT_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = l3if_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_FAIL;
    }

	for (ALL_LIST_ELEMENTS_RO(plist, pnode, if_register))
	{
		if(if_register->module_id == module_id)
		{
			if(if_register->ifindex != ifindex)
			{
				if_register->ifindex = ifindex;
			}
			return ERRNO_SUCCESS;
		}
	}

	if_register = (struct l3if_register *)XCALLOC(0, sizeof(struct l3if_register) );
	if(if_register == NULL)
	{
		ROUTE_LOG_DEBUG("malloc error!\n");
		return ERRNO_FAIL;
	}

	if_register->module_id = module_id;
	if_register->ifindex = ifindex;
	listnode_add(plist, if_register);

    return ERRNO_SUCCESS;
}

/**
 * @brief      : 三层接口信息注册链表初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:53:48
 * @note       :
 */
void l3if_event_list_init(void)
{
    int i;

    for (i=0; i<IFNET_EVENT_MAX; ++i)
    {
        l3if_event_list[i] = list_new();
    }

    return;
}


/**
 * @brief      : 三层接口信息变化通知
 * @param[in ] : type   - 变化的信息类型
 * @param[in ] : pevent - 接口变化信息通知结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:54:49
 * @note       :
 */
int l3if_event_notify(enum IFNET_EVENT type, struct ifm_event *pevent)
{
    struct list     *plist = NULL;
    struct listnode *pnode = NULL;
    int             *pdata = NULL;
	struct l3if_register *l3_register = NULL;

    if (type > IFNET_EVENT_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = l3if_event_list[type];
    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
    {
    	if(pdata == NULL)
			continue;
		
        l3_register = (struct l3if_register *) pdata;
		
        ROUTE_LOG_DEBUG("module_id=%d type=%d ifindex=%x\n", l3_register->module_id, type, pevent->ifindex);
		
		if(l3_register->ifindex == pevent->ifindex || l3_register->ifindex == 0)
        {
        	ipc_send_msg_n2(pevent, sizeof(struct ifm_event), 1, l3_register->module_id,
                        MODULE_ID_IFM, IPC_TYPE_IFM, type, IPC_OPCODE_EVENT,0);
		}
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由变化注册（IPv4）
 * @param[in ] : module_id - 注册者模块名
 * @param[in ] : subtype   - 注册的事件类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:57:33
 * @note       :
 */
int route_register_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;

    if (subtype > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = route_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_FAIL;
    }

    if (listnode_lookup(plist, (void *)module_id) == NULL)
    {
        listnode_add(plist, (void *)module_id);

        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/**
 * @brief      : 路由变化取消注册（IPv4）
 * @param[in ] : module_id - 取消注册者模块名
 * @param[in ] : subtype   - 取消注册的事件类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:58:10
 * @note       :
 */
int route_unregister_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;

    if (subtype > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = route_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_SUCCESS;
    }

    listnode_delete(plist, (void *)module_id);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由消息注册链表初始化（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 16:58:50
 * @note       :
 */
void route_event_list_init(void)
{
    int i;

    for (i=0; i<ROUTE_PROTO_MAX; ++i)
    {
        route_event_list[i] = list_new();
    }

    return;
}


/**
 * @brief      : 路由消息通知（IPv4）
 * @param[in ] : type   - 路由协议类型
 * @param[in ] : proute - 活跃路由结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:01:31
 * @note       :
 */
int route_event_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                        enum IPC_OPCODE opcode)
{
    struct list     *plist = NULL;
    struct listnode *pnode = NULL;
    int             *pdata = NULL;
    static int notify_num = 0;
    int        module_id;
    int        ret;

    ROUTE_LOG_DEBUG("type=%d\n", type);

    if (type > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = route_event_list[type];
    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
    {
        module_id = (int)pdata;

        /* mpls 模块关注 frr 路由，单独通知 */
        if (module_id == MODULE_ID_MPLS)
        {
            continue;
        }

        ROUTE_LOG_DEBUG("module_id=%d type=%d opcode=%d\n", module_id, type, opcode);

        /* 发送 100 条让出 cpu 10ms */
        if (++notify_num == 100)
        {
            notify_num = 0;
            usleep(10000);
        }

        ret = ipc_send_msg_n2(proute, sizeof(struct route_entry), 1, module_id,
                            MODULE_ID_ROUTE, IPC_TYPE_ROUTE, type, opcode, 0);
        if (ret != ERRNO_SUCCESS)
        {
            ROUTE_LOG_ERROR("IPC send route\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由消息通知（IPv4），携带 frr 路由
 * @param[in ] : type   - 路由协议类型
 * @param[in ] : proute - 活跃路由结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 14:49:14
 * @note       :
 */
int route_event_frr_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                            enum IPC_OPCODE opcode)
{
    struct list     *plist = NULL;
    struct listnode *pnode = NULL;
    int             *pdata = NULL;
    static int notify_num = 0;
    int        module_id;
    int        ret;

    ROUTE_LOG_DEBUG("type=%d\n", type);

    if (type > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = route_event_list[type];
    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
    {
        module_id = (int)pdata;

        /* mpls 关注 frr 路由，其他模块不需要通知 frr */
        if (module_id != MODULE_ID_MPLS)
        {
            continue;
        }

        ROUTE_LOG_DEBUG("module_id=%d type=%d opcode=%d\n", module_id, type, opcode);

        /* 发送 100 条让出 cpu 10ms */
        if (++notify_num == 100)
        {
            notify_num = 0;
            usleep(10000);
        }

        ret = ipc_send_msg_n2(proute, sizeof(struct route_entry), 1, module_id,
                            MODULE_ID_ROUTE, IPC_TYPE_ROUTE, type, opcode, 0);
        if (ret != ERRNO_SUCCESS)
        {
            ROUTE_LOG_ERROR("IPC send route\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 注册者初次注册，将路由表内数据通知一次（IPv4）
 * @param[in ] : module_id - 注册者模块
 * @param[in ] : subtype   - 注册的路由协议类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:03:26
 * @note       :
 */
int route_register_notify(int module_id, uint8_t subtype)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    int msg_num = IPC_MSG_LEN/sizeof(struct route_entry);
    struct route_entry route[msg_num];
    int data_num = 0;
    int vpn = 0;
    int i;

    memset(route, 0, msg_num*sizeof(struct route_entry));

    ROUTE_LOG_DEBUG();

    /* 轮询 active 路由表，找出对应协议类型路由通知 */
    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        PTREE_LOOP(route_tree[vpn], proute, pnode)
        {
            for (i=0; i<proute->nhp_num; i++)
            {
                memcpy(&route[data_num], proute, sizeof(struct route_entry));
                route[data_num].nhp_num = 1;

                if (proute->nhp[i].protocol == subtype)
                {
                    route[data_num++].nhp[0] = proute->nhp[i];

                    /* 将多条路由组包，一起发送 */
                    if (data_num == msg_num)
                    {
                        route_event_register_notify(module_id, subtype, route, data_num);
                        memset(route, 0, msg_num*sizeof(struct route_entry));

                        data_num = 0;
                    }
                }

                /* frr 路由第二条不发送 */
                if ((proute->nhpinfo.nhp_type == NHP_TYPE_FRR) && (module_id != MODULE_ID_MPLS))
                {
                    break;
                }
            }
        }

        /* 发送最后剩余路由 */
        if ((data_num>0) && (data_num<msg_num))
        {
            route_event_register_notify(module_id, subtype, route, data_num);
            memset(route, 0, msg_num*sizeof(struct route_entry));

            data_num = 0;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由变化注册（IPv6）
 * @param[in ] : module_id - 注册者模块名
 * @param[in ] : subtype   - 注册的事件类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:05:36
 * @note       :
 */
int routev6_register_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;

    if (subtype > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = routev6_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_FAIL;
    }

    if (listnode_lookup(plist, (void *)module_id) == NULL)
    {
        listnode_add(plist, (void *)module_id);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由变化取消注册（IPv6）
 * @param[in ] : module_id - 取消注册者模块名
 * @param[in ] : subtype   - 取消注册的事件类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:06:48
 * @note       :
 */
int routev6_unregister_msg(int module_id, uint8_t subtype)
{
    struct list *plist = NULL;

    if (subtype > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = routev6_event_list[subtype];
    if (plist == NULL)
    {
        return ERRNO_SUCCESS;
    }

    listnode_delete(plist, (void *)module_id);

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 路由消息注册链表初始化（IPv4）
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:07:26
 * @note       :
 */
void routev6_event_list_init(void)
{
    int i;

    for (i=0; i<ROUTE_PROTO_MAX; ++i)
    {
        routev6_event_list[i] = list_new();
    }

    return;
}


/**
 * @brief      : 路由消息通知（IPv6）
 * @param[in ] : type   - 路由协议类型
 * @param[in ] : proute - 活跃路由结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:07:46
 * @note       :
 */
int routev6_event_notify(enum ROUTE_PROTO type, struct route_entry *proute,
                            enum IPC_OPCODE opcode)
{
    struct list     *plist = NULL;
    struct listnode *pnode = NULL;
    int             *pdata = NULL;
    static int notify_num = 0;
    int        module_id;
    int        ret;

    ROUTE_LOG_DEBUG("type=%d\n", type);

    if (type > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    plist = routev6_event_list[type];
    for (ALL_LIST_ELEMENTS_RO(plist, pnode, pdata))
    {
        module_id = (int)pdata;

        ROUTE_LOG_DEBUG("module_id=%d type=%d opcode=%d\n", module_id, type, opcode);

        /* 发送 100 条让出 cpu 10ms */
        if (++notify_num == 100)
        {
            notify_num = 0;

            usleep(10000);
        }

        ret = ipc_send_msg_n2(proute, sizeof(struct route_entry), 1, module_id,
                            MODULE_ID_ROUTE, IPC_TYPE_ROUTE, type, opcode, 0);
        if (ret != ERRNO_SUCCESS)
        {
            ROUTE_LOG_ERROR("IPC send route\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 注册者初次注册，将路由表内数据通知一次（IPv6）
 * @param[in ] : module_id - 注册者模块
 * @param[in ] : subtype   - 注册的路由协议类型
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:10:00
 * @note       :
 */
int routev6_register_notify(int module_id, uint8_t subtype)
{
    struct ptree_node  *pnode  = NULL;
    struct route_entry *proute = NULL;
    int msg_num = IPC_MSG_LEN/sizeof(struct route_entry);
    struct route_entry route[msg_num];
    int data_num = 0;
    int vpn = 0;
    int i;

    memset(route, 0, msg_num*sizeof(struct route_entry));

    ROUTE_LOG_DEBUG();

    /* 轮询 active 路由表，找出对应协议类型路由通知 */
    for (vpn=0; vpn<=L3VPN_SIZE; vpn++)
    {
        PTREE_LOOP(routev6_tree[vpn], proute, pnode)
        {
            for (i=0; i<proute->nhp_num; i++)
            {
                memcpy(&route[data_num], proute, sizeof(struct route_entry));
                route[data_num].nhp_num = 1;

                if (proute->nhp[i].protocol == subtype)
                {
                    route[data_num++].nhp[0] = proute->nhp[i];

                    /* 将多条路由组包，一起发送 */
                    if (data_num == msg_num)
                    {
                        route_event_register_notify(module_id, subtype, route, data_num);
                        memset(route, 0, msg_num*sizeof(struct route_entry));

                        data_num = 0;
                    }
                }

                /* frr 路由第二条不发送 */
                if (proute->nhpinfo.nhp_type == NHP_TYPE_FRR)
                {
                    break;
                }
            }
        }

        /* 发送最后剩余路由 */
        if ((data_num>0) && (data_num<msg_num))
        {
            route_event_register_notify(module_id, subtype, route, data_num);
            memset(route, 0, msg_num*sizeof(struct route_entry));

            data_num = 0;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 注册者初次注册时通知路由中活跃路由
 * @param[in ] : module_id - 注册者模块
 * @param[in ] : type      - 注册的协议类型
 * @param[in ] : proute    - 活跃路由结构
 * @param[in ] : data_num  - 通知的路由数量
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年2月27日 17:11:02
 * @note       :
 */
int route_event_register_notify(int module_id, enum ROUTE_PROTO type,
                                            struct route_entry *proute, int data_num)
{
    static int notify_num = 0;
    int ret;

    ROUTE_LOG_DEBUG("module_id=%d type=%d\n", module_id, type);

    if (type > ROUTE_PROTO_MAX)
    {
        return ERRNO_PARAM_ILLEGAL;
    }

    /* 发送 100 条让出 cpu 10ms */
    if (++notify_num == 100)
    {
        notify_num = 0;

        usleep(10000);
    }

    ret = ipc_send_msg_n2(proute, data_num*sizeof(struct route_entry), data_num,
                            module_id, MODULE_ID_ROUTE, IPC_TYPE_ROUTE, type, IPC_OPCODE_ADD, 0);
    if (ret != ERRNO_SUCCESS)
    {
        ROUTE_LOG_ERROR("IPC send route\n");

        return ERRNO_CLI;
    }

    return ERRNO_SUCCESS;
}


