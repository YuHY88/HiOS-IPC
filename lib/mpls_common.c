/**
 * @file      : mpls_common.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 16:17:46
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <string.h>
#include "mpls_common.h"


/* add for mpls ipc common operation */

int mpls_ipc_msg_send(uint8_t *pdata, int data_len, int data_num, int module_src, int module_dst, 
                            int ipc_type, uint8_t opcode, uint16_t sub_opcode, int msg_index)
{
	int    ret = 0;

    struct ipc_mesg_n *pmesg = mem_share_malloc((sizeof(struct ipc_mesg_n) + data_len), module_src);
    if(pmesg == NULL) 
	{
		return ERRNO_MALLOC;
    }

    memset(pmesg, 0, (sizeof(struct ipc_msghdr_n) + data_len));
 
    pmesg->msghdr.module_id     = module_dst;
    pmesg->msghdr.sender_id     = module_src;
    pmesg->msghdr.msg_type      = ipc_type;
    pmesg->msghdr.opcode        = opcode;
    pmesg->msghdr.msg_subtype   = sub_opcode;
    pmesg->msghdr.msg_index     = msg_index;
    pmesg->msghdr.data_num      = data_num;

	if((pdata) && (data_len > 0))
    {
        pmesg->msghdr.data_len = data_len;
        
        memcpy(pmesg->msg_data, (uint8_t *)pdata, data_len);
    }
    
    ret = ipc_send_msg_n1(pmesg, (sizeof(struct ipc_msghdr_n) + data_len));

    if(-1 == ret)	
    {
		mem_share_free(pmesg, module_src);
        return ERRNO_IPC;
    }
    return ERRNO_SUCCESS;
}

int mpls_ipc_send_hal_wait_ack(void *pdata, int data_len, int data_num, int sender_id,
                     int msg_type, uint8_t opcode, uint8_t subtype, int msg_index)
{
    int    ret = 0;
    int    rcvlen = 0;

    struct ipc_mesg_n *pMsgSnd = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n)+data_len, MODULE_ID_MPLS);
    if(pMsgSnd == NULL) 
    {
        return -1;
    }

    memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));

    pMsgSnd->msghdr.data_len    = data_len;
    pMsgSnd->msghdr.module_id   = MODULE_ID_HAL;
    pMsgSnd->msghdr.sender_id   = sender_id;
    pMsgSnd->msghdr.msg_type    = msg_type;
    pMsgSnd->msghdr.msg_subtype = subtype;
    pMsgSnd->msghdr.msg_index   = msg_index;
    pMsgSnd->msghdr.data_num    = data_num;
    pMsgSnd->msghdr.opcode      = opcode;

    if (pdata)
    {
        memcpy(pMsgSnd->msg_data, pdata, data_len);
    }

    ret = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &rcvlen, 0);

    switch(ret)
    {
       case -1:
             mem_share_free(pMsgSnd, MODULE_ID_MPLS);
             return -1;

       case -2: 
            return -1;

       case  0:
             if(pMsgRcv == NULL) 
             {
                ret = -1;
             }
             else 
             {
                /*if noack return errcode*/
                if(IPC_OPCODE_NACK == pMsgRcv->msghdr.opcode)
                {
                    memcpy(&ret, pMsgRcv->msg_data, sizeof(ret));
                                   
                }
                /*if ack return 0*/
                else if(IPC_OPCODE_ACK == pMsgRcv->msghdr.opcode)
                {
                    ret = 0;
                }
                else
                {
                    ret = -1;
                }

                 mem_share_free(pMsgRcv, MODULE_ID_MPLS);
             }
             return ret;
             
        default: 
            return-1;
    }

    return ret;
}

/****************************************/

/**
 * @brief      : 获取标签
 * @param[in ] : vrf_id    - l3vpn 实例号
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回标签指针，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:18:14
 * @note       :
 */
uint32_t *mpls_com_get_label(uint16_t vrf_id, int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                        IPC_TYPE_L3VPN, L3VPN_SUBTYPE_LABEL,
                                        IPC_OPCODE_GET, vrf_id,0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1

        return (uint32_t *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 释放标签
 * @param[in ] : vrf_id    - l3vpn 实例号
 * @param[in ] : label     - 标签值
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 0，否则返回 -1
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:19:17
 * @note       :
 */
int mpls_com_label_free(uint16_t vrf_id, uint32_t label, int module_id)
{
    return ipc_send_msg_n2((uint8_t *)(&label), sizeof(uint32_t), 1, MODULE_ID_MPLS, module_id,
                            IPC_TYPE_L3VPN, L3VPN_SUBTYPE_LABEL,
                            IPC_OPCODE_DELETE, vrf_id);
}


/**
 * @brief      : 获取 l3vpn 实例
 * @param[in ] : vrf_id    - l3vpn 实例号
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 l3vpn 结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:21:35
 * @note       :
 */

struct l3vpn_entry *mpls_com_get_l3vpn_instance(uint16_t vrf_id, int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&vrf_id, sizeof(uint16_t), 1, MODULE_ID_MPLS, module_id,
                                        IPC_TYPE_L3VPN, L3VPN_SUBTYPE_INSTANCE,
                                        IPC_OPCODE_GET, 0,0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1
        return (struct l3vpn_entry *)pmesg->msg_data;
	}

    return NULL;

}

/**
 * @brief      : 获取 l3vpn 实例号
 * @param[in ] : vrf_id    - l3vpn 实例号
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 l3vpn实例号 ，失败返回0
 * @author     : ZhouDW
 * @date       : 2018年3月8日 16:21:35
 * @note       :
 */

uint16_t mpls_com_get_l3vpn_instance2(uint16_t vrf_id, int module_id)
{
	struct l3vpn_entry * l3_vpn = NULL;
	uint16_t vpn_temp = 0;
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&vrf_id, sizeof(uint16_t), 1, MODULE_ID_MPLS, module_id,
                                        IPC_TYPE_L3VPN, L3VPN_SUBTYPE_INSTANCE,
                                        IPC_OPCODE_GET, 0,0);
    if (NULL != pmesg)
	{
       	if(pmesg->msghdr.data_len != 0)
		{
			l3_vpn = (struct l3vpn_entry *)pmesg->msg_data;
			vpn_temp = l3_vpn->l3vpn.vrf_id;
		}
		mem_share_free(pmesg, module_id);
	}
    return vpn_temp;

}


/**
 * @brief      : l3vpn 事件注册
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 0，否则返回 -1
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:22:17
 * @note       :
 */
int mpls_com_l3vpn_event_register(int module_id)
{
    return ipc_send_msg_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id, IPC_TYPE_L3VPN,
                            0, IPC_OPCODE_REGISTER, 0);
}


/**
 * @brief      : 批量获取 l3vpn 实例
 * @param[in ] : vrf_id    - l3vpn 实例号
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到的 l3vpn 实例数量
 * @return     : 成功返回 l3vpn 数据组
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:23:07
 * @note       :
 */
struct l3vpn_entry *mpls_com_get_l3vpn_bulk(int vrf_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&vrf_id, 4, 1, MODULE_ID_MPLS,
                                            module_id, IPC_TYPE_L3VPN, L3VPN_SUBTYPE_INSTANCE,
                                            IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (struct l3vpn_entry *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 批量获取 l3vpn target
 * @param[in ] : pl3target - target 结构
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到的 target 个数
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:24:20
 * @note       :
 */
struct l3vpn_target *mpls_com_get_l3vpn_target_bulk(struct l3vpn_target *pl3target,
                                                                    int module_id, int *pdata_num)
{
    int target_len = 0;
    struct ipc_mesg_n *pmesg = NULL;

    if (NULL == pl3target)
    {
        return NULL;
    }

    if ('\0' != pl3target->target[0])
    {
        target_len = strlen((void *)pl3target->target)+1;
    }

    pmesg = ipc_sync_send_n2(pl3target->target, target_len, 1,
                            MODULE_ID_MPLS, module_id,
                            IPC_TYPE_L3VPN, pl3target->target_type,
                            IPC_OPCODE_GET_BULK, pl3target->vrf_id, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (struct l3vpn_target *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 获取 lsr-id
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 lsr-id 指针，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:25:52
 * @note       :
 */
uint32_t *mpls_com_get_lsr_id(int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                        IPC_TYPE_LSP, MPLS_GET_LSR_ID, IPC_OPCODE_GET, 0, 0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1
        return (uint32_t *)pmesg->msg_data;
	}

    return NULL;

}


/**
 * @brief      : 批量获取静态 lsp
 * @param[in ] : plspname  - lsp 名称
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到的静态 lsp 数量
 * @return     : 成功返回静态 lsp 结构组，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:26:38
 * @note       :
 */
struct static_lsp *mpls_com_get_slsp_bulk(uchar *plspname, int module_id, int *pdata_num)
{
    int name_len = 0;
    struct ipc_mesg_n *pmesg  = NULL;
    if (NULL != plspname)
    {
        if ('\0' != plspname[0])
        {
            name_len = strlen((void *)plspname)+1;
        }
    }

    pmesg = ipc_sync_send_n2(plspname, name_len, 1, MODULE_ID_MPLS,
                                module_id, IPC_TYPE_LSP, MPLS_GET_STATIC_LSP_BULK,
                                IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (struct static_lsp *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 批量获取 l2vc
 * @param[in ] : ppwname   - pw 名称
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到的 l2vc 数量
 * @return     : 成功返回 l2vc 结构组，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:27:38
 * @note       :
 */
struct l2vc_entry *mpls_com_get_l2vc_bulk(uchar *ppwname, int module_id, int *pdata_num)
{
    int name_len = 0;
    struct ipc_mesg_n *pmesg = NULL;

    if (NULL != ppwname)
    {
        if ('\0' != ppwname[0])
        {
            name_len = strlen((void *)ppwname)+1;
        }
    }

    pmesg = ipc_sync_send_n2(ppwname, name_len, 1, MODULE_ID_MPLS,
                                            module_id, IPC_TYPE_PW, MPLS_GET_L2VC_BULK,
                                            IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
	{
		if (NULL == pmesg->msg_data)
		{
			mem_share_free(pmesg, module_id);
		}
		else
		{
        	*pdata_num = pmesg->msghdr.data_num;

        	return (struct l2vc_entry *)pmesg->msg_data;
		}
	}

    return NULL;
}


/**
 * @brief      : 获取 pw 统计
 * @param[in ] : ppwname   - pw 名称
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 pw 统计信息，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:28:26
 * @note       :
 */
struct counter_t *mpls_com_get_l2vc_counter(uchar *ppwname, int module_id)
{
    int name_len = 0;
    struct ipc_mesg_n *pmesg = NULL;

    if (NULL != ppwname)
    {
        if ('\0' != ppwname[0])
        {
            name_len = strlen((void *)ppwname)+1;
        }
    }

    pmesg = ipc_sync_send_n2(ppwname, name_len, 1, MODULE_ID_MPLS, module_id,
                                      IPC_TYPE_PW, MPLS_GET_L2VC_COUNTER, IPC_OPCODE_GET, 0, 0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1
        return (struct counter_t *)pmesg->msg_data;
	}

    return NULL;

                             
}


/**
 * @brief      : 获取 tunnel 统计
 * @param[in ] : ifindex   - 接口索引
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 tunnel 统计信息，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:29:13
 * @note       :
 */
struct counter_t *mpls_com_get_tunnel_counter(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n *pmesg =  ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id, IPC_TYPE_TUNNEL,
                                      MPLS_GET_TUNNEL_COUNTER, IPC_OPCODE_GET, ifindex, 0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1
        return (struct counter_t *)pmesg->msg_data;
	}

    return NULL;

}


/**
 * @brief      : 批量获取 vsi
 * @param[in ] : vsi_id    - vsi id
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到 vsi 的数量
 * @return     : 成功返回 vsi 结构组，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:30:03
 * @note       :
 */
struct vsi_entry *mpls_com_get_vsi_bulk(uint32_t vsi_id, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&vsi_id, 4, 1, MODULE_ID_MPLS,
                                            module_id, IPC_TYPE_VSI, MPLS_GET_VSI_BULK,
                                            IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (struct vsi_entry *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 批量获取指定 vsi 中接口
 * @param[in ] : vsi_id    - vsi id
 * @param[in ] : ifindex   - 接口索引
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到接口的数量
 * @return     : 成功返回接口索引结构组，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:31:07
 * @note       :
 */
uint32_t *mpls_com_get_vsiif_bulk(uint32_t vsi_id, uint32_t ifindex, int module_id,
                                            int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&vsi_id, 4, 1, MODULE_ID_MPLS,
                                            module_id, IPC_TYPE_VSI, MPLS_GET_VSIIF_BULK,
                                            IPC_OPCODE_GET_BULK, ifindex, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (uint32_t *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 批量获取 tunnel
 * @param[in ] : ifindex   - 接口索引
 * @param[in ] : module_id - 模块名称
 * @param[out] : pdata_num - 获取到 tunnel 的数量
 * @return     : 成功返回 tunnel 结构组，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:32:40
 * @note       :
 */
struct tunnel_if *mpls_com_get_tunnel_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_TUNNEL, MPLS_GET_TUNNEL_BULK,
                                                IPC_OPCODE_GET_BULK, ifindex, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;

        return (struct tunnel_if *)pmesg->msg_data;
	}

    return NULL;
}


/**
 * @brief      : 获取 tunnel
 * @param[in ] : ifindex   - 接口索引
 * @param[in ] : module_id - 模块名称
 * @param[out] :
 * @return     : 成功返回 tunnel 接口结构，否则返回 NULL
 * @author     : ZhangFj
 * @date       : 2018年3月8日 16:33:34
 * @note       :
 */
struct tunnel_if *mpls_com_get_tunnel(uint32_t ifindex, int module_id)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS,
                                      module_id, IPC_TYPE_TUNNEL, MPLS_GET_TUNNEL,
                                      IPC_OPCODE_GET, ifindex, 0);
    if (NULL != pmesg)
	{
        //pmesg->msghdr.data_num == 1
        return (struct tunnel_if *)pmesg->msg_data;
	}

    return NULL;

}


struct hqos_pw *mpls_com_get_pw_hqos_bulk(uchar *ppwname, int module_id, int *pdata_num)
{
    int name_len = 0;
    struct ipc_mesg_n *pmesg = NULL;

    if (NULL != ppwname)
    {
        if ('\0' != ppwname[0])
        {
            name_len = strlen((void *)ppwname)+1;
        }
    }

    pmesg = ipc_sync_send_n2(ppwname, name_len, 1, MODULE_ID_MPLS,
                                            module_id, IPC_TYPE_PW, MPLS_GET_PW_HQOS_BULK,
                                            IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct hqos_pw *)pmesg->msg_data;
	}

    return NULL;
}


struct hqos_tunnel *mpls_com_get_tunnel_hqos_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_TUNNEL, MPLS_GET_TUNNEL_HQOS_BULK,
                                                IPC_OPCODE_GET_BULK, ifindex, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct hqos_tunnel *)pmesg->msg_data;
	}

    return NULL;
}

struct pw_class *mpls_com_get_pw_class_bulk(struct pw_class *pindex, uint32_t index_flag, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, sizeof(struct pw_class),
                                                          1, MODULE_ID_MPLS, module_id,
                                                          IPC_TYPE_PW, MPLS_GET_PW_CLASS_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct pw_class *)pmesg->msg_data;
    }

    return NULL;

}

struct serv_ins_mib *mpls_com_get_service_instance_bulk(struct serv_ins_mib *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, sizeof(struct serv_ins_mib),
                                                          1, MODULE_ID_MPLS, module_id,
                                                          IPC_TYPE_PW, MPLS_GET_SERV_INS_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct serv_ins_mib *)pmesg->msg_data;
    }

    return NULL;

}

struct xc_group_mib *mpls_com_get_xc_group_bulk(struct xc_group_mib *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, sizeof(struct xc_group_mib),
                                                          1, MODULE_ID_MPLS, module_id,
                                                          IPC_TYPE_PW, MPLS_GET_XC_GROUP_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct xc_group_mib *)pmesg->msg_data;
    }

    return NULL;

}

/* ldp add for mib */

struct ldp_ipprefix_mib *mpls_com_get_ldp_prefix_bulk(uint32_t prefix, uint32_t prelen, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(&prelen, sizeof(uint32_t), 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_PREFIX_BULK,
                                                IPC_OPCODE_GET_BULK, prefix, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_ipprefix_mib *)pmesg->msg_data;
	}

    return NULL;
}


struct ldp_config_global_mib *mpls_com_get_ldp_global_conf( uint32_t local_lsrid, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_CONF_GLOBAL,
                                                IPC_OPCODE_GET_BULK, local_lsrid, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_config_global_mib *)pmesg->msg_data;
	}

    return NULL;
}


struct ldp_remoteconf_mib *mpls_com_get_ldp_remotconf_bulk(uint32_t peer_lsrid, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_REMOT_BULK,
                                                IPC_OPCODE_GET_BULK, peer_lsrid, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_remoteconf_mib *)pmesg->msg_data;
	}

    return NULL;
}


struct ldp_localif_mib *mpls_com_get_ldp_localif_bulk(uint32_t ifindex, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_LOCALIF_BULK,
                                                IPC_OPCODE_GET_BULK, ifindex, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_localif_mib *)pmesg->msg_data;
	}

    return NULL;
}


struct ldp_session_mib *mpls_com_get_ldp_sess_bulk(uint32_t peer_lsrid, int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_SESS_BULK,
                                                IPC_OPCODE_GET_BULK, peer_lsrid, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_session_mib *)pmesg->msg_data;
	}

    return NULL;
}


struct ldp_lsp_mib *mpls_com_get_ldp_lsp_bulk(uint32_t lsp_index, int module_id, int *pdata_num)
{

	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, 0, 1, MODULE_ID_MPLS, module_id,
                                                IPC_TYPE_LDP, MPLS_GET_LDP_LSP_BULK,
                                                IPC_OPCODE_GET_BULK, lsp_index, 0);
    if (NULL != pmesg)
	{
        *pdata_num = pmesg->msghdr.data_num;
        return (struct ldp_lsp_mib *)pmesg->msg_data;
	}

    return NULL;
}

/* add for mpls service */


struct mpls_service *mpls_com_get_mpls_serivce_bulk(struct mpls_service *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, sizeof(struct mpls_service),
                                                          1, MODULE_ID_MPLS, module_id,
                                                          IPC_TYPE_PW, MPLS_GET_MPLS_SERV_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct mpls_service *)pmesg->msg_data;
    }

    return NULL;

}


struct mpls_mac *mpls_com_get_vpls_black_white_mac_bulk(struct mpls_mac *pindex, uint32_t data_len,
                                                int module_id, int *pdata_num)
{
    if(0 == pindex->vplsid)
    {
        data_len = 0;
    }
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, data_len, 1, MODULE_ID_MPLS, module_id,
                                                        IPC_TYPE_VSI, MPLS_GET_VSI_BLACKMAC_BULK,
                                                        IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct mpls_mac *)pmesg->msg_data;
    }

    return NULL;

}


struct l2vc_entry *mpls_com_get_vpls_pw_bulk(int vsi_id, struct l2vc_entry *pindex, uint32_t data_len,
                        int module_id, int *pdata_num)
{
    if(NULL == pindex)
    {
        data_len = 0;
    }
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, data_len, 1, MODULE_ID_MPLS, module_id,
                                                      IPC_TYPE_VSI, MPLS_GET_VSI_SLAVE_PW_BULK,
                                                      IPC_OPCODE_GET_BULK, vsi_id, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct l2vc_entry *)pmesg->msg_data;
    }

    return NULL;

}
/* 网管增加标签池占用情况查询功能 */
struct label_info *mpls_com_get_mpls_label_pool_bulk(struct label_info *pindex, uint32_t index_flag,
                                                int module_id, int *pdata_num)
{
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, sizeof(struct label_info),
                                                          1, MODULE_ID_MPLS, module_id,
                                                          IPC_TYPE_MPLS, MPLS_GET_LABEL_POOL_BULK,
                                                          IPC_OPCODE_GET_BULK, index_flag, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct label_info *)pmesg->msg_data;
    }

    return NULL;

}

/* add vpls static mac mib get func */
struct mpls_smac *mpls_com_get_vpls_static_mac_bulk(struct mpls_smac *pindex, uint32_t data_len,
                                                int module_id, int *pdata_num)
{
    if(0 == pindex->vplsid)
    {
        data_len = 0;
    }
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pindex, data_len, 1, MODULE_ID_MPLS, module_id,
                                                        IPC_TYPE_VSI, MPLS_GET_VSI_STATIC_MAC_BULK,
                                                        IPC_OPCODE_GET_BULK, 0, 0);
    if (NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (struct mpls_smac *)pmesg->msg_data;
    }

    return NULL;

}

