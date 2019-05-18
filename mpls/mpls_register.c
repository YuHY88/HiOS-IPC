/**
 * @file      : mpls_register.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 14:33:53
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/module_id.h>
#include <lib/zassert.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "mpls_register.h"
#include "mpls_main.h"


struct list *l3vpn_event_list = NULL;


/**
 * @brief      : l3vpn 消息注册
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRBNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:34:44
 * @note       :
 */
int l3vpn_register_msg(int module_id)
{
    if (l3vpn_event_list == NULL)
    {
        return ERRNO_FAIL;
    }

    if (listnode_lookup(l3vpn_event_list, (void *)module_id) == NULL)
    {
        listnode_add(l3vpn_event_list, (void *)module_id);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : l3vpn 事件链表初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:36:01
 * @note       :
 */
void l3vpn_event_list_init(void)
{
    l3vpn_event_list = list_new();
}


/**
 * @brief      : l3vpn 事件通知
 * @param[in ] : pl3vpn - l3vpn 结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:36:42
 * @note       :
 */
int l3vpn_event_notify(struct l3vpn_entry *pl3vpn, enum IPC_OPCODE opcode)
{
    struct listnode *pnode = NULL;
    int             *pdata = NULL;
    int module_id;

    for (ALL_LIST_ELEMENTS_RO(l3vpn_event_list, pnode, pdata))
    {
        module_id = (int)pdata;

        MPLS_LOG_DEBUG("");

        ipc_send_msg_n2(pl3vpn, sizeof(struct l3vpn_entry), 1, module_id,
                        MODULE_ID_MPLS, IPC_TYPE_L3VPN, 0, opcode, 0);
    }

    return ERRNO_SUCCESS;
}


