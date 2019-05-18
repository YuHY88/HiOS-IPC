/**
 * @file      : vpls_cmd.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 16:08:54
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/ether.h>
#include <lib/errcode.h>
#include "mpls_main.h"
#include "pw_cmd.h"
#include "mpls.h"
#include "vpls_cmd.h"


static struct list *config_list = NULL;  // listnode 存储 struct vsi_mac_entry 结构指针，解决配置恢复问题
int vsi_black_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac);
int vsi_white_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac);
int vsi_static_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac);


/* 初始化 vsi node */
static struct cmd_node vsi_node =
{
    VSI_NODE,
    "%s(mpls-vsi)#",
    1
};


/* 初始化 mac node */
static struct cmd_node mac_node =
{
    MAC_NODE,
    "%s(config-mac)# ",
    1,
};


/**
 * @brief      : vpls 配置管理
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:10:46
 * @note       :
 */
static void mpls_vsi_config_write_process(struct vty *vty, struct vsi_entry *pvsi)
{
    struct listnode *pnode = NULL;
    uint8_t         *pmac  = NULL;

    vty_out(vty, " vsi %d%s", pvsi->vpls.vsi_id, VTY_NEWLINE);

    if (pvsi->name[0] != '\0')
    {
        vty_out(vty, "  name %s%s", pvsi->name, VTY_NEWLINE);
    }

    if (pvsi->vpls.mac_learn == MAC_LEARN_ENABLE)
    {
        if (pvsi->vpls.learning_limit > 0 )
        {
            vty_out(vty, "  mac-learn limit %d%s", pvsi->vpls.learning_limit, VTY_NEWLINE);
        }
    }

    if (pvsi->vpls.mac_learn == MAC_LEARN_DISABLE)
    {
        vty_out(vty, "  mac-learn disable%s", VTY_NEWLINE);
    }

    if (pvsi->vpls.tag_flag == PW_ENCAP_ETHERNET_TAG)
    {
        vty_out(vty, "  encapsulate ethernet tag tpid %#x vlan %u%s",
                    pvsi->vpls.tpid, pvsi->vpls.vlan, VTY_NEWLINE);
    }
    else if (pvsi->vpls.tag_flag == PW_ENCAP_VLAN_RAW)
    {
        vty_out(vty, "  encapsulate vlan raw tpid %#x vlan %u%s",
                    pvsi->vpls.tpid, pvsi->vpls.vlan, VTY_NEWLINE);
    }
    else if (pvsi->vpls.tag_flag == PW_ENCAP_VLAN_TAG)
    {
        vty_out(vty, "  encapsulate vlan tag tpid %#x vlan %u%s",
                    pvsi->vpls.tpid, pvsi->vpls.vlan, VTY_NEWLINE);
    }

    if (0 != pvsi->vpls.uc_cbs)
    {
        vty_out(vty, "  storm-suppress unicast cir %u cbs %u%s",
                pvsi->vpls.uc_cir, pvsi->vpls.uc_cbs, VTY_NEWLINE);
    }

    if (0 != pvsi->vpls.bc_cbs)
    {
        vty_out(vty, "  storm-suppress broadcast cir %u cbs %u%s",
                pvsi->vpls.bc_cir, pvsi->vpls.bc_cbs, VTY_NEWLINE);
    }

    if (0 != pvsi->vpls.mc_cbs)
    {
        vty_out(vty, "  storm-suppress multicast cir %u cbs %u%s",
                pvsi->vpls.mc_cir, pvsi->vpls.mc_cbs, VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(pvsi->blacklist, pnode, pmac))
    {
        vty_out(vty, "  mac-blacklist %02x:%02x:%02x:%02x:%02x:%02x%s", pmac[0], pmac[1],
                pmac[2], pmac[3],pmac[4], pmac[5], VTY_NEWLINE);
    }

    for (ALL_LIST_ELEMENTS_RO(pvsi->whitelist, pnode, pmac))
    {
        vty_out(vty, "  mac-whitelist %02x:%02x:%02x:%02x:%02x:%02x%s", pmac[0], pmac[1],
                pmac[2], pmac[3],pmac[4], pmac[5], VTY_NEWLINE);
    }

    return;
}


/**
 * @brief      : vsi 配置下发
 * @param[in ] : vsi_id - vsi 实例
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回 ERRNO_IPC
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:12:08
 * @note       :
 */
static int vsi_id_download(uint16_t vsi_id, enum OPCODE_E opcode)
{
    enum IPC_OPCODE ipc_opcode = IPC_OPCODE_INVALID;
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == OPCODE_ADD)
    {
        ipc_opcode = IPC_OPCODE_ADD;
    }
    else if (opcode == OPCODE_DELETE)
    {
        ipc_opcode = IPC_OPCODE_DELETE;
    }

    ret = MPLS_IPC_SENDTO_HAL(&vsi_id, sizeof(uint16_t), 1,
                        MODULE_ID_MPLS, IPC_TYPE_VSI, ipc_opcode, VSI_INFO_VSI, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    ret = MPLS_IPC_SENDTO_FTM(&vsi_id, sizeof(uint16_t), 1, 
                        MODULE_ID_MPLS, IPC_TYPE_VSI, ipc_opcode, VSI_INFO_VSI, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发封装类型
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:13:19
 * @note       :
 */
static int vsi_encapsulate_download(struct vsi_entry *pvsi)
{
    int ret;

    ret = mpls_ipc_send_hal_wait_ack(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                                MODULE_ID_MPLS, IPC_TYPE_VSI,
                                IPC_OPCODE_UPDATE, VSI_INFO_ENCAP, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ret;
    }

    ret = MPLS_IPC_SENDTO_FTM(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                        MODULE_ID_MPLS, IPC_TYPE_VSI, IPC_OPCODE_UPDATE, VSI_INFO_ENCAP, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发 mac 学习功能
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:14:13
 * @note       :
 */
static int vsi_mac_learn_enable_download(struct vsi_entry *pvsi)
{
    int ret;

    ret = mpls_ipc_send_hal_wait_ack(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                                MODULE_ID_MPLS, IPC_TYPE_VSI,
                                IPC_OPCODE_UPDATE, VSI_INFO_MAC_LEARN, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ret;
    }

    ret = MPLS_IPC_SENDTO_FTM(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                        MODULE_ID_MPLS, IPC_TYPE_VSI, IPC_OPCODE_UPDATE, VSI_INFO_MAC_LEARN, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发 mac 学习上限
 * @param[in ] : pvsi - vsi 结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:15:40
 * @note       :
 */
static int vsi_mac_learn_limit_download(struct vsi_entry *pvsi)
{
    int ret;

    ret = mpls_ipc_send_hal_wait_ack(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                                MODULE_ID_MPLS, IPC_TYPE_VSI,
                                IPC_OPCODE_UPDATE, VSI_INFO_LEARN_LIMIT, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ret;
    }

    ret = MPLS_IPC_SENDTO_FTM(&(pvsi->vpls), sizeof(struct vpls_info), 1, 
                        MODULE_ID_MPLS, IPC_TYPE_VSI, IPC_OPCODE_UPDATE, VSI_INFO_LEARN_LIMIT, 0);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发 mac 黑白名单
 * @param[in ] : pvsi   - vsi 结构
 * @param[in ] : pmac   - mac 地址
 * @param[in ] : info   - vsi 下发信息类型
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:17:43
 * @note       :
 */
int vsi_white_black_mac_download(struct vsi_entry *pvsi, uint8_t *pmac, enum VSI_INFO info, enum OPCODE_E opcode)
{
    int msg_len = 0;
    int ret = ERRNO_SUCCESS;

    MPLS_LOG_DEBUG();

    if (NULL != pmac)
    {
        msg_len = MAC_LEN;
    }

    if (opcode == OPCODE_ADD)
    {
        ret = mpls_ipc_send_hal_wait_ack(pmac, msg_len, 1, MODULE_ID_MPLS,
                                    IPC_TYPE_VSI, IPC_OPCODE_ADD, info, pvsi->vpls.vsi_id);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(pmac, msg_len, 1, MODULE_ID_MPLS,
                            IPC_TYPE_VSI, IPC_OPCODE_ADD, info, pvsi->vpls.vsi_id);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(pmac, msg_len, 1, MODULE_ID_MPLS,
                            IPC_TYPE_VSI, IPC_OPCODE_DELETE, info, pvsi->vpls.vsi_id);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(pmac, msg_len, 1, MODULE_ID_MPLS,
                            IPC_TYPE_VSI, IPC_OPCODE_DELETE, info, pvsi->vpls.vsi_id);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发接口
 * @param[in ] : pvsi    - vsi 结构
 * @param[in ] : ifindex - 接口索引
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:22:23
 * @note       :
 */
int vsi_ac_interface_download(struct vsi_entry *pvsi, uint32_t ifindex, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == OPCODE_ADD)
    {
        ret = mpls_ipc_send_hal_wait_ack(&pvsi->vpls.vsi_id, sizeof(uint16_t), 1,
                                    MODULE_ID_MPLS, IPC_TYPE_VSI,
                                    IPC_OPCODE_ADD, VSI_INFO_AC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(&pvsi->vpls.vsi_id, sizeof(uint16_t), 1, 
                            MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_ADD, VSI_INFO_AC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(&pvsi->vpls.vsi_id, sizeof(uint16_t), 1,
                            MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_AC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(&pvsi->vpls.vsi_id, sizeof(uint16_t), 1, 
                            MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_AC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发 pw
 * @param[in ] : ppwinfo - pw 结构
 * @param[in ] : opcode  - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:25:54
 * @note       :
 */
int vsi_pwinfo_download(struct pw_info *ppwinfo, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == OPCODE_ADD)
    {
        ret = mpls_ipc_send_hal_wait_ack(ppwinfo, sizeof(struct pw_info), 1,
                                    MODULE_ID_MPLS, IPC_TYPE_VSI,
                                    IPC_OPCODE_ADD, VSI_INFO_PW, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }

        ret = MPLS_IPC_SENDTO_FTM(ppwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_VSI, IPC_OPCODE_ADD, VSI_INFO_PW, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(ppwinfo, sizeof(struct pw_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_PW, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }

        ret = MPLS_IPC_SENDTO_FTM(ppwinfo, sizeof(struct pw_info), 1, 
                            MODULE_ID_MPLS, IPC_TYPE_VSI, IPC_OPCODE_DELETE, VSI_INFO_PW, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to ftm\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 下发 vsi pw 主备切换消息
 * @param[in ] : ifindex - 主 pw 索引
 * @param[in ] : pinfo   - 更新消息携带的数据
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 10:50:01
 * @note       :
 */
int vsi_pw_status_download(uint32_t ifindex, void *pinfo)
{
    int ret;

    MPLS_LOG_DEBUG();

    ret = MPLS_IPC_SENDTO_HAL(pinfo, 4, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                        IPC_OPCODE_UPDATE, VSI_INFO_PW_BACKUP, ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    ret = MPLS_IPC_SENDTO_FTM(pinfo, 4, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                        IPC_OPCODE_UPDATE, VSI_INFO_PW_BACKUP, ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to ftm\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发 pw 名称
 * @param[in ] : pl2vc  - l2vc 结构
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:29:41
 * @note       :
 */
int vsi_pw_name_download(struct l2vc_entry *pl2vc, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == OPCODE_ADD)
    {
        ret = MPLS_IPC_SENDTO_HAL(pl2vc->name, NAME_STRING_LEN, 1, MODULE_ID_MPLS,
                            IPC_TYPE_VSI, IPC_OPCODE_ADD, VSI_INFO_NAME, pl2vc->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }
    else if (opcode == OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(pl2vc->name, NAME_STRING_LEN, 1, MODULE_ID_MPLS,
                            IPC_TYPE_VSI, IPC_OPCODE_DELETE, VSI_INFO_NAME, pl2vc->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发风暴抑制
 * @param[in ] : pvsi   - vsi 结构
 * @param[in ] : type   - vsi 下发类型
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:33:38
 * @note       :
 */
static int vsi_storm_suppress_download(struct vsi_entry *pvsi, enum VSI_INFO type,
                                    enum IPC_OPCODE opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (opcode == IPC_OPCODE_ADD)
    {
        ret = mpls_ipc_send_hal_wait_ack(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                                    MODULE_ID_MPLS, IPC_TYPE_VSI,
                                    IPC_OPCODE_ADD, type, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }
    }
    else if (opcode == IPC_OPCODE_DELETE)
    {
        ret = MPLS_IPC_SENDTO_HAL(&(pvsi->vpls), sizeof(struct vpls_info), 1,
                            MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, type, 0);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发静态 mac
 * @param[in ] : pl2vc  - l2vc 结构
 * @param[in ] : pmac   - mac 地址
 * @param[in ] : opcode - 操作码
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:36:03
 * @note       :
 */
static int vsi_pw_mac_download(struct l2vc_entry *pl2vc, uchar *pmac, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (OPCODE_ADD == opcode)
    {
        ret = mpls_ipc_send_hal_wait_ack(pmac, MAC_LEN, 1, MODULE_ID_MPLS,
                                    IPC_TYPE_VSI, IPC_OPCODE_ADD,
                                    VSI_INFO_STATIC_MAC, pl2vc->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pmac, MAC_LEN, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_STATIC_MAC, pl2vc->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }
    else if (OPCODE_DOWN == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(NULL, 0, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_STATIC_MAC_ALL, pl2vc->pwinfo.pwindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 10:42:01
 * @note       :
 */
int vsi_interface_mac_download(uint32_t ifindex, uchar *pmac, enum OPCODE_E opcode)
{
    int ret;

    MPLS_LOG_DEBUG();

    if (OPCODE_ADD == opcode)
    {
        ret = mpls_ipc_send_hal_wait_ack(pmac, MAC_LEN, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                                    IPC_OPCODE_ADD, VSI_INFO_INTERFACE_STATIC_MAC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ret;
        }
    }
    else if (OPCODE_DELETE == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(pmac, MAC_LEN, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_INTERFACE_STATIC_MAC, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }
    else if (OPCODE_DOWN == opcode)
    {
        ret = MPLS_IPC_SENDTO_HAL(NULL, 0, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                            IPC_OPCODE_DELETE, VSI_INFO_INTERFACE_STATIC_MAC_ALL, ifindex);
        if (ERRNO_SUCCESS != ret)
        {
            MPLS_LOG_ERROR("send to hal\n");

            return ERRNO_IPC;
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vsi 下发静态 mac 删除
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:37:42
 * @note       :
 */
static int vsi_static_mac_download(uint16_t vsi_id)
{
    int ret;

    ret = MPLS_IPC_SENDTO_HAL(NULL, 0, 1, MODULE_ID_MPLS, IPC_TYPE_VSI,
                        IPC_OPCODE_DELETE, VSI_INFO_VSI_STATIC_MAC, vsi_id);
    if (ERRNO_SUCCESS != ret)
    {
        MPLS_LOG_ERROR("send to hal\n");

        return ERRNO_IPC;
    }

    return ERRNO_SUCCESS;
}

int vsi_black_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac)
{
    struct listnode   *pnode   = NULL;
    uint8_t           *smac     = NULL;
    
    if ((NULL == pvsi) || (NULL == pmac))
    {
        return ERRNO_FAIL;
    }
    /* check if the mac is in the vpls black mac list by pw */
    if (NULL != pvsi->blacklist)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->blacklist, pnode, smac))
        {
            if (0 == memcmp(pmac, smac, MAC_LEN))
            {
                vty_error_out(vty, "Mac address already exist within the blacklist !%s", VTY_NEWLINE);

                return ERRNO_FAIL;
            }
        }
    }

    return ERRNO_SUCCESS;
}

int vsi_white_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac)
{
    struct listnode   *pnode   = NULL;
    uint8_t           *smac     = NULL;

    if ((NULL == pvsi) || (NULL == pmac))
    {
        return ERRNO_FAIL;
    }
    /* check if the mac is in the vpls white mac list by pw */
    if (NULL != pvsi->blacklist)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->whitelist, pnode, smac))
        {
            if (0 == memcmp(pmac, smac, MAC_LEN))
            {
                vty_error_out(vty, "Mac address already exist within the whitelist !%s", VTY_NEWLINE);

                return ERRNO_FAIL;
            }
        }
    }

    return ERRNO_SUCCESS;
}

/**
 * @brief      : vsi 检查 mac 地址冲突
 * @param[in ] : vty  - vty 全局结构
 * @param[in ] : pvsi - vsi 结构
 * @param[in ] : pmac - mac 地址
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:38:28
 * @note       : 相同 vsi 内不同 pw 不允许配置相同静态 mac
 * @note       : 静态 mac 不能与 mac 黑名单冲突
 */
int vsi_static_mac_conflict(struct vty *vty, struct vsi_entry *pvsi, uchar *pmac)
{
    struct l2vc_entry *pl2vc    = NULL;
    struct listnode   *pnode1   = NULL;
    struct listnode   *pnode2   = NULL;
    struct mpls_if    *pif      = NULL;
    uint32_t          *pifindex = NULL;
    uint8_t           *smac     = NULL;
    char ifname[IFNET_NAMESIZE] = "";

    if ((NULL == pvsi) || (NULL == pmac))
    {
        return ERRNO_FAIL;
    }

    /* check if the mac is in the vpls static mac list by pws */
    if (NULL != pvsi->pwlist)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode1, pl2vc))
        {
            if ((NULL == pl2vc) || (NULL == pl2vc->mac_list))
            {
                continue;
            }

            for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode2, smac))
            {
                if (0 == memcmp(smac, pmac, MAC_LEN))
                {
                    vty_error_out(vty, "Mac address already exist within the pw %s !%s", pl2vc->name, VTY_NEWLINE);

                    return ERRNO_FAIL;
                }
            }
        }
    }

    /* check if the mac is in the vpls static mac list by ac interfaces */
    if (NULL != pvsi->aclist)
    {
        for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode1, pifindex))
        {
            pif = mpls_if_get((uint32_t)pifindex);
            if (NULL == pif)
            {
                continue;
            }

            memset(ifname, 0, IFNET_NAMESIZE);
            ifm_get_name_by_ifindex((uint32_t)pifindex, ifname);

            for (ALL_LIST_ELEMENTS_RO(&pif->mac_list, pnode2, smac))
            {
                if (0 == memcmp(smac, pmac, MAC_LEN))
                {
                    vty_error_out(vty, "Mac address already exist within the ac %s !%s", ifname, VTY_NEWLINE);

                    return ERRNO_FAIL;
                }
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 创建 vsi 实例命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:41:54
 * @note       :
 */
DEFUN (mpls_vsi,
    mpls_vsi_cmd,
    "vsi <1-1024>",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi    = NULL;
    char             *pprompt = NULL;
    uint16_t vsi_id;
    int      ret;

    MPLS_LSRID_CHECK(vty);

    vsi_id = atoi(argv[0]);

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        pvsi = vsi_create(vsi_id);
        if (NULL == pvsi)
        {
            vty_error_out(vty, "Fail to create vsi of %u.%s", vsi_id, VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = vsi_add(pvsi);
        if (ERRNO_SUCCESS != ret)
        {
            vsi_free(pvsi);

            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_SUCCESS;
        }

        ret = vsi_id_download(vsi_id, OPCODE_ADD);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_SUCCESS;
        }
    }

    vty->index = (void *)pvsi;
    vty->node  = VSI_NODE;

    pprompt = vty->change_prompt;
    if (NULL != pprompt)
    {
        snprintf(pprompt, VTY_BUFSIZ, "%%s(mpls-vsi-%s)# ", argv[0]);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 vsi 实例命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:44:10
 * @note       :
 */
DEFUN (no_mpls_vsi,
    no_mpls_vsi_cmd,
    "no vsi <1-1024>",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi     = NULL;
    struct listnode   *pnode    = NULL;
    struct l2vc_entry *pl2vc    = NULL;
    struct mpls_if    *pif      = NULL;
    uint32_t          *pifindex = NULL;
    uint16_t vsi_id;
    int      ret;

    vsi_id = atoi(argv[0]);

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        return ERRNO_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
    {
        if (NULL != pl2vc->mac_list)
        {
            vty_error_out(vty, "Please clear the static MAC at first !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pifindex))
    {
        pif = mpls_if_get((uint32_t)pifindex);
        if (NULL == pif)
        {
            continue;
        }

        if (!list_isempty(&pif->mac_list))
        {
            vty_error_out(vty, "Please clear the static MAC at first !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    ret = vsi_id_download(vsi_id, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_SUCCESS;
    }

    vsi_delete_if_all(pvsi);
    vsi_delete_pw_all(pvsi);
    vsi_delete_mac_all(pvsi->blacklist);
    vsi_delete_mac_all(pvsi->whitelist);
    vsi_delete(vsi_id);

    return CMD_SUCCESS;
}


/**
 * @brief      : vsi 显示命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:46:13
 * @note       :
 */
DEFUN (show_vsi,
    show_vsi_cmd,
    "show vsi [<1-1024>]",
    MPLS_CLI_INFO)
{
    char ifname[IFNET_NAMESIZE]      = "";
    struct listnode    *pnode        = NULL;
    struct l2vc_entry  *pl2vc        = NULL;
    struct l2vc_entry  *pl2vc_backup = NULL;
    struct vsi_entry   *pvsi         = NULL;
    struct hash_bucket *pbucket      = NULL;
    uint32_t           *pifindex     = NULL;
    uint16_t vsi_id;
    uint32_t cursor;
    uint32_t num       = 0;
    uint32_t group_num = 1;

    if (NULL != argv[0])
    {
        vsi_id = atoi(argv[0]);

        pvsi = vsi_lookup(vsi_id);
        if (pvsi == NULL)
        {
            return CMD_WARNING;
        }

        vty_out(vty, "*************************************************%s", VTY_NEWLINE);
        vty_out(vty, "%-20s: %u%s", "vsi-id", vsi_id, VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "vsi-name", pvsi->name, VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "failback status", pvsi->failback==FAILBACK_ENABLE?
                    "failback":"non-failback", VTY_NEWLINE);
        vty_out(vty, "%-20s: %u%s", "failback wtr", pvsi->wtr, VTY_NEWLINE);
        vty_out(vty, "%-20s: %s%s", "mac-learn", pvsi->vpls.mac_learn == MAC_LEARN_ENABLE ?
                    "enable" : "disable", VTY_NEWLINE);

        if (pvsi->vpls.mac_learn == MAC_LEARN_ENABLE)
        {
            vty_out(vty, "%-20s: %d%s", "mac-learn limit", pvsi->vpls.learning_limit, VTY_NEWLINE);
        }

        if (pvsi->vpls.tag_flag == PW_ENCAP_ETHERNET_RAW)
        {
            vty_out(vty, "%-20s: %s%s", "encapsulate", "ethernet raw", VTY_NEWLINE);
        }
        else if (pvsi->vpls.tag_flag == PW_ENCAP_ETHERNET_TAG)
        {
            vty_out(vty, "%-20s: %s%s", "encapsulate", "ethernet tag", VTY_NEWLINE);
        }
        else if (pvsi->vpls.tag_flag == PW_ENCAP_VLAN_RAW)
        {
            vty_out(vty, "%-20s: %s%s", "encapsulate", "vlan raw", VTY_NEWLINE);
        }
        else if (pvsi->vpls.tag_flag == PW_ENCAP_VLAN_TAG)
        {
            vty_out(vty, "%-20s: %s%s", "encapsulate", "vlan tag", VTY_NEWLINE);
        }

        if (pvsi->vpls.tag_flag != PW_ENCAP_ETHERNET_RAW)
        {
            vty_out(vty, "%-20s: %#x%s", "tpid", pvsi->vpls.tpid, VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "vlan", pvsi->vpls.vlan, VTY_NEWLINE);
        }

        if (0 != pvsi->vpls.uc_cbs)
        {
            vty_out(vty, "%-20s: %s%s", "storm-suppress", "unicast", VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cir", pvsi->vpls.uc_cir, VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cbs", pvsi->vpls.uc_cbs, VTY_NEWLINE);
        }

        if (0 != pvsi->vpls.bc_cbs)
        {
            vty_out(vty, "%-20s: %s%s", "storm-suppress", "broadcast", VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cir", pvsi->vpls.bc_cir, VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cbs", pvsi->vpls.bc_cbs, VTY_NEWLINE);
        }

        if (0 != pvsi->vpls.mc_cbs)
        {
            vty_out(vty, "%-20s: %s%s", "storm-suppress", "multicast", VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cir", pvsi->vpls.mc_cir, VTY_NEWLINE);
            vty_out(vty, "%-20s: %u%s", "cbs", pvsi->vpls.mc_cbs, VTY_NEWLINE);
        }

        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "*******************pw of vsi %d******************%s", vsi_id, VTY_NEWLINE);
        vty_out(vty, "%s",VTY_NEWLINE);

        if (pvsi->pwlist != NULL)
        {
            for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
            {
                if (pl2vc == NULL)
                {
                    continue;
                }

                if (0 != pl2vc->pwinfo.backup_index)
                {
                    continue;
                }

                vty_out(vty, "%10s : %s  %s%s", "pw name", pl2vc->name,
                        pl2vc->pwinfo.vpls.upe==0?"spe":"upe", VTY_NEWLINE);
            }

            vty_out(vty, "%s", VTY_NEWLINE);

            for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
            {
                if (pl2vc == NULL)
                {
                    continue;
                }

                if ((0 == pl2vc->pwinfo.backup_index)
                    || (PW_TYPE_BACKUP == pl2vc->pwinfo.pw_type))
                {
                    continue;
                }

                pl2vc_backup = pl2vc->ppw_backup;

                vty_out(vty, "%6s% -3d : %s", "group", group_num++, VTY_NEWLINE);
                vty_out(vty, "%10s%12s : %s%s", "", "master-pw", pl2vc->name, VTY_NEWLINE);
                vty_out(vty, "%10s%12s : %s%s", "", "backup-pw", pl2vc_backup->name, VTY_NEWLINE);
                vty_out(vty, "%10s%12s : %s%s", "", "work-status", pl2vc->pw_backup==
                            BACKUP_STATUS_SLAVE?"backup":"master", VTY_NEWLINE);
            }
        }

        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "***************interface of vsi %d***************%s", vsi_id, VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);

        if (pvsi->aclist != NULL)
        {
            for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pifindex))
            {
                ifm_get_name_by_ifindex((uint32_t)pifindex, ifname);

                vty_out(vty,"%10s : %s%s", "interface", ifname, VTY_NEWLINE);
            }
        }

        vty_out(vty, "%s", VTY_NEWLINE);
        vty_out(vty, "***********************end***********************%s", VTY_NEWLINE);
        vty_out(vty, "%s", VTY_NEWLINE);
    }
    else
    {
        HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
        {
            pvsi = (struct vsi_entry *)pbucket->data;
            if (NULL == pvsi)
            {
                continue;
            }

            if (++num == 1)
            {
                vty_out(vty, "%-10s:     %d %s", "total", vsi_table.num_entries, VTY_NEWLINE);
                vty_out(vty, "%s", VTY_NEWLINE);
                vty_out(vty, "%-10s%-32s%-10s%-15s%s", "vsi-id", "vsi-name", "mac-learn","learning_limit", VTY_NEWLINE);
            }

            if( (pvsi->vpls.mac_learn == MAC_LEARN_ENABLE) && (pvsi->vpls.learning_limit != -1))
            {
                vty_out(vty, "%-10u%-32s%-10s%-10d%s", pvsi->vpls.vsi_id, pvsi->name,
                            pvsi->vpls.mac_learn==0?"disable":"enable",
                            pvsi->vpls.learning_limit, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-10u%-32s%-10s%s", pvsi->vpls.vsi_id, pvsi->name,
                            pvsi->vpls.mac_learn==0?"disable":"enable", VTY_NEWLINE);
            }
        }

        if (0 != num)
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : vsi 名称配置命令
 * @param[in ] : name NAME - vsi 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:48:37
 * @note       :
 */
DEFUN (mpls_vsi_name,
    mpls_vsi_name_cmd,
    "name NAME",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;

    memcpy(pvsi->name, argv[0], NAME_STRING_LEN);

    return CMD_SUCCESS;
}


/**
 * @brief      : pw 回切模式及回切时延配置命令
 * @param[in ] : non-failback - 不回切模式
 * @param[in ] : failback     - 回切模式
 * @param[in ] : wtr <0-3600> - 回切时延
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年4月12日 11:17:26
 * @note       :
 */
DEFUN (mpls_vsi_pw_failback,
    mpls_vsi_pw_failback_cmd,
    "pw backup {non-failback | failback | wtr <0-3600>}",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi  = (struct vsi_entry *)vty->index;
    struct listnode   *pnode = NULL;
    struct l2vc_entry *pl2vc = NULL;

    if (NULL != argv[0])
    {
        pvsi->failback = FAILBACK_DISABLE;
    }
    else if (NULL != argv[1])
    {
        pvsi->failback = FAILBACK_ENABLE;
        pvsi->wtr      = 30;

        if (NULL != argv[2])
        {
            pvsi->wtr = atoi(argv[2]);
        }

        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
        {
            if ((NULL == pl2vc) || (NULL == pl2vc->ppw_backup) || (0 == pl2vc->pwinfo.backup_index))
            {
                continue;
            }

            if (PW_TYPE_BACKUP == pl2vc->pwinfo.pw_type)
            {
                continue;
            }

            vsi_set_pw_status(pl2vc);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 学习开关配置命令
 * @param[in ] : enable  - 使能学习
 * @param[in ] : disable - 不使能学习
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:49:16
 * @note       :
 */
DEFUN (vsi_maclearn,
    vsi_maclearn_cmd,
    "mac-learn (enable | disable)",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int ret;

    if (argv[0][0] == 'e')
    {
        if (pvsi->vpls.mac_learn == MAC_LEARN_ENABLE)
        {
            vty_warning_out(vty, "Already enable mac-learn !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pvsi->vpls.mac_learn      = MAC_LEARN_ENABLE;
        pvsi->vpls.learning_limit = 0;		// 0:表示不对 mac 学习数量进行限制

        ret = vsi_mac_learn_enable_download(pvsi);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (argv[0][0] == 'd')
    {
        if (pvsi->vpls.mac_learn == MAC_LEARN_DISABLE)
        {
            vty_warning_out(vty, "Already disable mac-learn !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pvsi->vpls.mac_learn      = MAC_LEARN_DISABLE;
        pvsi->vpls.learning_limit = 0;

        ret = vsi_mac_learn_enable_download(pvsi);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 学习上限配置命令
 * @param[in ] : limit <1-65535> - 上限值域
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:51:50
 * @note       : ht201 上限为 32767，ht2200 上限为 65535
 */
DEFUN(vsi_mac_learn_limit,
    vsi_mac_learn_limit_cmd,
    "mac-learn limit <1-65535>",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int limit_num = 0;
    int ret;

    if (pvsi->vpls.mac_learn != MAC_LEARN_ENABLE)
    {
        vty_error_out(vty, "Please set 'mac-learn enable' firstly!%s",VTY_NEWLINE);

        return CMD_WARNING;
    }

    limit_num = atoi(argv[0]);
    if (0 >= limit_num)
    {
        vty_error_out(vty, "Please set valid number!%s",VTY_NEWLINE);

        return CMD_WARNING;
    }

    pvsi->vpls.learning_limit = limit_num;

    ret = vsi_mac_learn_limit_download(pvsi);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 学习上限删除命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:57:15
 * @note       :
 */
DEFUN(no_vsi_mac_learn_limit,
    no_vsi_mac_learn_limit_cmd,
    "no mac-learn limit",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int ret;

    if (pvsi->vpls.mac_learn == MAC_LEARN_ENABLE )
    {
        pvsi->vpls.learning_limit = 0;

        ret = vsi_mac_learn_limit_download(pvsi);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 黑名单配置命令
 * @param[in ] : MAC - mac 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 16:57:52
 * @note       :
 */
DEFUN(vsi_mac_blacklist,
    vsi_mac_blacklist_cmd,
    "mac-blacklist MAC",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    uint8_t mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: mac */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* 全0 、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pvsi->blacklist->count == VSI_MAC_LIST_MAX)
    {
        vty_error_out(vty, "The MAC address number for blacklist exceed limit !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ( (NULL != pvsi->whitelist)&& (pvsi->whitelist->count))
    {
        vty_error_out(vty, "The white mac list has been eanble !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_static_mac_conflict(vty, pvsi, mac);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    ret = vsi_white_mac_conflict(vty, pvsi, mac);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    if (NULL != vsi_lookup_mac(pvsi->blacklist, mac))
    {
        vty_error_out(vty, "The MAC address already exists in blacklist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_white_black_mac_download(pvsi, mac, VSI_INFO_MAC_BLACKLIST, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_add_mac(pvsi->blacklist, mac);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 黑名单删除命令
 * @param[in ] : MAC - mac 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:19:43
 * @note       :
 */
DEFUN(no_vsi_mac_blacklist,
    no_vsi_mac_blacklist_cmd,
    "no mac-blacklist MAC",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    uint8_t mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: mac */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全0无效 */
    if (0 == ether_is_zero_mac(mac))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL == vsi_lookup_mac(pvsi->blacklist, mac))
    {
        return CMD_SUCCESS;
    }

    ret = vsi_white_black_mac_download(pvsi, mac, VSI_INFO_MAC_BLACKLIST, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_mac(pvsi->blacklist, mac);

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除整个 vsi 内 mac 黑名单命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:21:45
 * @note       :
 */
DEFUN(no_vsi_mac_blacklist_all,
    no_vsi_mac_blacklist_all_cmd,
    "no mac-blacklist all",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int ret = ERRNO_SUCCESS;

    if (list_isempty(pvsi->blacklist))
    {
        return CMD_SUCCESS;
    }

    ret = vsi_white_black_mac_download(pvsi, NULL, VSI_INFO_MAC_BLACKLIST_ALL, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_mac_all(pvsi->blacklist);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 mac 黑名单命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:22:56
 * @note       :
 */
DEFUN(show_vsi_mac_blacklist,
    show_vsi_mac_blacklist_cmd,
    "show mac-blacklist",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi  = (struct vsi_entry *)vty->index;
    struct listnode  *pnode = NULL;
    uint8_t          *pmac  = NULL;

    if (0 == pvsi->blacklist->count)
    {
        return CMD_SUCCESS;
    }

    vty_out(vty, "----------vsi %u mac-blacklist----------%s", pvsi->vpls.vsi_id, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "Total : %u%s", pvsi->blacklist->count, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(pvsi->blacklist, pnode, pmac))
    {
        vty_out(vty, "%12s: %02x:%02x:%02x:%02x:%02x:%02x%s", "MAC address",
                pmac[0], pmac[1], pmac[2], pmac[3],pmac[4], pmac[5], VTY_NEWLINE);
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 白名单配置命令
 * @param[in ] : MAC - mac 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:23:46
 * @note       :
 */
DEFUN(vsi_mac_whitelist,
    vsi_mac_whitelist_cmd,
    "mac-whitelist MAC",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    uint8_t mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: mac */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* 全0 、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (pvsi->whitelist->count == VSI_MAC_LIST_MAX)
    {
        vty_error_out(vty, "The MAC address number for whitelist exceed limit !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if ( (NULL != pvsi->blacklist)&& (pvsi->blacklist->count))
    {
        vty_error_out(vty, "The black mac list has been eanble !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_white_mac_conflict(vty, pvsi, mac);
    if(ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "The MAC address already exists in whitelist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }
    ret = vsi_black_mac_conflict(vty, pvsi, mac);
    if (ERRNO_SUCCESS != ret)
    {
        return CMD_WARNING;
    }

    if (NULL != vsi_lookup_mac(pvsi->whitelist, mac))
    {
        vty_error_out(vty, "The MAC address already exists in whitelist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_white_black_mac_download(pvsi, mac, VSI_INFO_MAC_WHITELIST, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_add_mac(pvsi->whitelist, mac);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : mac 白名单删除命令
 * @param[in ] : MAC - mac 地址
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:25:01
 * @note       :
 */
DEFUN(no_vsi_mac_whitelist,
    no_vsi_mac_whitelist_cmd,
    "no mac-whitelist MAC",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    uint8_t mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: mac */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全 0 无效 */
    if (0 == ether_is_zero_mac(mac))
    {
        vty_error_out(vty, "The specified MAC address is invalid !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL == vsi_lookup_mac(pvsi->whitelist, mac))
    {
        return CMD_SUCCESS;
    }

    ret = vsi_white_black_mac_download(pvsi, mac, VSI_INFO_MAC_WHITELIST, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_mac(pvsi->whitelist, mac);

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除整个 vsi 内 mac 白名单命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:26:13
 * @note       :
 */
DEFUN(no_vsi_mac_whitelist_all,
    no_vsi_mac_whitelist_all_cmd,
    "no mac-whitelist all",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int ret = ERRNO_SUCCESS;

    if (list_isempty(pvsi->whitelist))
    {
        return CMD_SUCCESS;
    }

    ret = vsi_white_black_mac_download(pvsi, NULL, VSI_INFO_MAC_WHITELIST_ALL, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ERRNO_IPC), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_mac_all(pvsi->whitelist);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 mac 白名单命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:31:48
 * @note       :
 */
DEFUN(show_vsi_mac_whitelist,
    show_vsi_mac_whitelist_cmd,
    "show mac-whitelist",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi  = (struct vsi_entry *)vty->index;
    struct listnode  *pnode = NULL;
    uint8_t          *pmac  = NULL;

    if (0 == pvsi->whitelist->count)
    {
        return CMD_SUCCESS;
    }

    vty_out(vty, "----------vsi %u mac-whitelist----------%s", pvsi->vpls.vsi_id, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "Total : %u%s", pvsi->whitelist->count, VTY_NEWLINE);
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "----------------------------------------%s", VTY_NEWLINE);

    for (ALL_LIST_ELEMENTS_RO(pvsi->whitelist, pnode, pmac))
    {
        vty_out(vty, "%12s: %02x:%02x:%02x:%02x:%02x:%02x%s", "MAC address",
                    pmac[0], pmac[1], pmac[2], pmac[3],pmac[4], pmac[5], VTY_NEWLINE);
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口绑定 vsi 命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:32:56
 * @note       :
 */
DEFUN(mpls_if_bind_vsi,
    mpls_if_bind_vsi_cmd,
    "mpls vsi <1-1024>",
    MPLS_CLI_INFO)
{
    struct vsi_entry     *pvsi  = NULL;
    struct mpls_if       *pif   = NULL;
    struct listnode      *pnode = NULL;
    struct listnode      *pnext = NULL;
    struct vsi_mac_entry *pvmac = NULL;
    uchar                *pmac  = NULL;
    uint32_t ifindex;
    uint16_t vsi_id;
    int      ret;

    vsi_id  = atoi(argv[0]);
    ifindex = (uint32_t)vty->index;

    pvsi = vsi_lookup(vsi_id);
    if (pvsi == NULL)
    {
        vty_error_out(vty, "Specify the VSI does not exist !%s", VTY_NEWLINE);

        return CMD_SUCCESS;
    }

    ret = vsi_add_if(pvsi, ifindex);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    ret = vsi_ac_interface_download(pvsi, ifindex, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vsi_delete_if(pvsi, ifindex);

        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (vty->config_read_flag == 1)
    {
        for (ALL_LIST_ELEMENTS(config_list, pnode, pnext, pvmac))
        {
            if (NULL == pvmac)
            {
                continue;
            }

            if (pvmac->ifindex != ifindex)
            {
                continue;
            }

            list_delete_node(config_list, pnode);

            pif = mpls_if_get(ifindex);
            if (NULL == pif)
            {
                XFREE(MTYPE_VSI_ENTRY, pvmac);

                continue;
            }

            vsi_interface_mac_download(ifindex, pvmac->mac, OPCODE_ADD);

            pmac = (uchar *)XCALLOC(MTYPE_VSI_ENTRY, MAC_LEN);

            memcpy(pmac, pvmac->mac, MAC_LEN);
            listnode_add(&pif->mac_list, pmac);

            gmpls.vsi_pw_num++;

            XFREE(MTYPE_VSI_ENTRY, pvmac);
        }

        if ((NULL != config_list) && list_isempty(config_list))
        {
            list_free(config_list);
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 接口解绑 vsi 命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:37:23
 * @note       :
 */
DEFUN(no_mpls_if_bind_vsi,
    no_mpls_if_bind_vsi_cmd,
    "no mpls vsi <1-1024>",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = NULL;
    struct mpls_if   *pif  = NULL;
    uint32_t ifindex;
    uint16_t vsi_id;
    int      ret;

    vsi_id  = atoi(argv[0]);
    ifindex = (uint32_t)vty->index;

    pvsi = vsi_lookup(vsi_id);
    if (pvsi == NULL)
    {
        return CMD_SUCCESS;
    }

    pif = mpls_if_get(ifindex);
    if (NULL != pif)
    {
        if (!list_isempty(&pif->mac_list))
        {
            vty_error_out(vty, "Please clear the static MAC at first !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    ret = vsi_ac_interface_download(pvsi, ifindex, OPCODE_DELETE);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    vsi_delete_if(pvsi, ifindex);

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 vsi 封装命令
 * @param[in ] : ethernet - ethernet 接入
 * @param[in ] : vlan     - vlan 接入
 * @param[in ] : raw      - raw 方式
 * @param[in ] : tag      - tag 方式
 * @param[in ] : 0x8100   - tpid 取值
 * @param[in ] : 0x88a8   - tpid 取值
 * @param[in ] : 0x9100   - tpid 取值
 * @param[in ] : <0-4094> - vlan id
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:38:33
 * @note       :
 */
DEFUN(mpls_vsi_encapsulate,
    mpls_vsi_encapsulate_cmd,
    "encapsulate (ethernet | vlan) (raw | tag) {tpid (0x8100|0x88a8|0x9100) | vlan <0-4094>}",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int               ret  =  ERRNO_SUCCESS;

    if (VSI_ENCAP_ETHERNET_RAW != pvsi->vpls.tag_flag)
    {
        vty_error_out(vty, "Vsi already configured encapsulation !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[0]: encapsulate type */
    if (argv[0][0] == 'e')
    {
        if (argv[1][0] == 'r')
        {
            if (VSI_ENCAP_ETHERNET_RAW == pvsi->vpls.tag_flag)
            {
                return CMD_SUCCESS;
            }

            pvsi->vpls.tag_flag = VSI_ENCAP_ETHERNET_RAW;
            pvsi->vpls.tpid     = 0;
            pvsi->vpls.vlan     = 0;
        }
        else if (argv[1][0] == 't')
        {
            pvsi->vpls.tag_flag = VSI_ENCAP_ETHERNET_TAG;
        }
    }
    else if (argv[0][0] == 'v')
    {
        if (argv[1][0] == 'r')
        {
            pvsi->vpls.tag_flag = VSI_ENCAP_VLAN_RAW;
        }
        else if (argv[1][0] == 't')
        {
            pvsi->vpls.tag_flag = VSI_ENCAP_VLAN_TAG;
        }
    }

    if (NULL != argv[2])
    {
        pvsi->vpls.tpid = strtol(argv[2], NULL, 16);
    }

    if (NULL != argv[3])
    {
        pvsi->vpls.vlan = atoi(argv[3]);
    }

    ret = vsi_encapsulate_download(pvsi);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 vsi 封装命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:41:08
 * @note       :
 */
DEFUN(no_mpls_vsi_encapsulate,
    no_mpls_vsi_encapsulate_cmd,
    "no encapsulate",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    int ret  = ERRNO_SUCCESS;

    if (VSI_ENCAP_ETHERNET_RAW == pvsi->vpls.tag_flag)
    {
        return CMD_SUCCESS;
    }

    pvsi->vpls.tag_flag = VSI_ENCAP_ETHERNET_RAW;
    pvsi->vpls.tpid     = 0;
    pvsi->vpls.vlan     = 0;

    ret = vsi_encapsulate_download(pvsi);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : vsi 风暴抑制配置命令
 * @param[in ] : unicast          - 单播报文
 * @param[in ] : broadcast        - 广播报文
 * @param[in ] : multicast        - 多播报文
 * @param[in ] : cir <0-10000000> - 承诺信息速率
 * @param[in ] : cbs <16-1600>    - 承诺突发尺寸
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:42:11
 * @note       :
 */
DEFUN(mpls_vsi_storm_suppress,
    mpls_vsi_storm_suppress_cmd,
    "storm-suppress (unicast | broadcast | multicast) cir <0-10000000> cbs <16-1600>",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    enum VSI_INFO   type   = VSI_INFO_INVALID;
    enum IPC_OPCODE opcode = IPC_OPCODE_ADD;
    uint32_t cir = 0;
    uint32_t cbs = 0;
    int      ret =  0;

    cir = atoi(argv[1]);    // argv[1]:cir
    cbs = atoi(argv[2]);    // argv[2]:cbs

    if (argv[0][0] == 'u')
    {
        if (0 != pvsi->vpls.uc_cbs)
        {
            vty_error_out(vty, "unicast suppress already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pvsi->vpls.uc_cir = cir;
        pvsi->vpls.uc_cbs = cbs;
        type              = VSI_INFO_STORM_SUPPRESS_UC;
    }
    else if (argv[0][0] == 'm')
    {
        if (0 != pvsi->vpls.mc_cbs)
        {
            vty_error_out(vty, "multicast suppress already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pvsi->vpls.mc_cir = cir;
        pvsi->vpls.mc_cbs = cbs;
        type              = VSI_INFO_STORM_SUPPRESS_MC;
    }
    else if (argv[0][0] == 'b')
    {
        if (0 != pvsi->vpls.bc_cbs)
        {
            vty_error_out(vty, "broadcast suppress already configuration !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        pvsi->vpls.bc_cir = cir;
        pvsi->vpls.bc_cbs = cbs;
        type              = VSI_INFO_STORM_SUPPRESS_BC;
    }

    ret = vsi_storm_suppress_download(pvsi, type, opcode);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : vsi 风暴抑制删除命令
 * @param[in ] : unicast          - 单播报文
 * @param[in ] : broadcast        - 广播报文
 * @param[in ] : multicast        - 多播报文
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月2日 17:46:53
 * @note       :
 */
DEFUN(no_mpls_vsi_storm_suppress,
    no_mpls_vsi_storm_suppress_cmd,
    "no storm-suppress (unicast | broadcast | multicast)",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;
    enum VSI_INFO   type   = VSI_INFO_INVALID;
    enum IPC_OPCODE opcode = IPC_OPCODE_DELETE;
    int ret = 0;

    if (argv[0][0] == 'u')
    {
        if (0 == pvsi->vpls.uc_cbs)
        {
        	vty_error_out(vty, "no unicast suppress, please configure first!%s", VTY_NEWLINE);

            return CMD_SUCCESS;
        }

        pvsi->vpls.uc_cir = 0;
        pvsi->vpls.uc_cbs = 0;
        type              = VSI_INFO_STORM_SUPPRESS_UC;
    }
    else if (argv[0][0] == 'b')
    {
        if (0 == pvsi->vpls.bc_cbs)
        {
        	vty_error_out(vty, "no broadcast suppress, please configure first!%s", VTY_NEWLINE);

            return CMD_SUCCESS;
        }

        pvsi->vpls.bc_cir = 0;
        pvsi->vpls.bc_cbs = 0;
        type              = VSI_INFO_STORM_SUPPRESS_BC;
    }
    else if (argv[0][0] == 'm')
    {
        if (0 == pvsi->vpls.mc_cbs)
        {
        	vty_error_out(vty, "no multicast suppress,please configure first!%s", VTY_NEWLINE);

            return CMD_SUCCESS;
        }

        pvsi->vpls.mc_cir = 0;
        pvsi->vpls.mc_cbs = 0;
        type              = VSI_INFO_STORM_SUPPRESS_MC;
    }

    ret = vsi_storm_suppress_download(pvsi, type, opcode);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 进入 MAC 节点命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:13:44
 * @note       :
 */
DEFUN(mac_node_func,
    mac_node_cmd,
    "mac",
    MPLS_CLI_INFO)
{
    vty->node = MAC_NODE;

    return CMD_SUCCESS;
}


/**
 * @brief      : 配置 pw 静态 mac 命令
 * @param[in ] : XX:XX:XX:XX:XX:XX - MAC 地址
 * @param[in ] : pw NAME           - pw 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:16:02
 * @note       :
 */
DEFUN(vsi_pw_static_mac,
    vsi_pw_static_mac_cmd,
    "vpls mac static XX:XX:XX:XX:XX:XX pw NAME",
    MPLS_CLI_INFO)
{
    struct vsi_entry  *pvsi  = NULL;
    struct l2vc_entry *pl2vc = NULL;
    uchar             *pmac  = NULL;
    uchar mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: MAC address */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[1]: pw name */
    pl2vc = l2vc_lookup((uchar *)argv[1]);
    if (NULL == pl2vc)
    {
        vty_error_out(vty, "Specify the PW dose not exist !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (!PW_ALREADY_BIND_VSI(&pl2vc->pwinfo))
    {
        if (1 != vty->config_read_flag)
        {
            vty_error_out(vty, "Specify the PW does not support static MAC configuration !%s",
                            VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    pvsi = vsi_lookup(pl2vc->pwinfo.vpls.vsi_id);
    if (NULL != pvsi)
    {
        ret = vsi_static_mac_conflict(vty, pvsi, mac);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }

        ret = vsi_black_mac_conflict(vty, pvsi, mac);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }
    }

    ret = vsi_pw_mac_download(pl2vc, mac, OPCODE_ADD);
    if (ERRNO_SUCCESS != ret)
    {
        vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

        return CMD_WARNING;
    }

    pmac = (uchar *)XCALLOC(MTYPE_VSI_ENTRY, MAC_LEN);
    if (NULL == pmac)
    {
        vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL == pl2vc->mac_list)
    {
        pl2vc->mac_list = list_new();
    }

    memcpy(pmac, mac, MAC_LEN);
    listnode_add(pl2vc->mac_list, pmac);

    gmpls.vsi_pw_num++;

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 pw 静态 mac 命令
 * @param[in ] : XX:XX:XX:XX:XX:XX - MAC 地址
 * @param[in ] : pw NAME           - pw 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:18:35
 * @note       :
 */
DEFUN(no_vsi_pw_static_mac,
    no_vsi_pw_static_mac_cmd,
    "no vpls mac static XX:XX:XX:XX:XX:XX pw NAME",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc     = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    uchar             *pmac      = NULL;
    uchar mac[MAC_LEN] = "";

    /* argv[0]: MAC address */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    /* argv[1]: pw name */
    pl2vc = l2vc_lookup((uchar *)argv[1]);
    if ((NULL == pl2vc) || (NULL == pl2vc->mac_list))
    {
        return CMD_SUCCESS;
    }

    if (!PW_ALREADY_BIND_VSI(&pl2vc->pwinfo))
    {
        return CMD_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(pl2vc->mac_list, pnode, pnextnode, pmac))
    {
        if(NULL != pmac)
        {
            if (0 == memcmp(mac, pmac, MAC_LEN))
            {
                vsi_pw_mac_download(pl2vc, pmac, OPCODE_DELETE);

                XFREE(MTYPE_VSI_ENTRY, pmac);

                list_delete_node(pl2vc->mac_list, pnode);

                gmpls.vsi_pw_num--;
            }
        }
    }

    if (list_isempty(pl2vc->mac_list))
    {
        list_free(pl2vc->mac_list);

        pl2vc->mac_list = NULL;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除单条 pw 上所有的静态 mac 命令
 * @param[in ] : pw NAME - pw 名称
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:25:50
 * @note       :
 */
DEFUN(no_vsi_pw_static_mac_all,
    no_vsi_pw_static_mac_all_cmd,
    "no vpls mac static all pw NAME",
    MPLS_CLI_INFO)
{
    struct l2vc_entry *pl2vc     = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    uchar             *pmac      = NULL;

    /* argv[0]: pw name */
    pl2vc = l2vc_lookup((uchar *)argv[0]);
    if ((NULL == pl2vc) || (NULL == pl2vc->mac_list))
    {
        return CMD_SUCCESS;
    }

    if (!PW_ALREADY_BIND_VSI(&pl2vc->pwinfo))
    {
        return CMD_SUCCESS;
    }

    if (!list_isempty(pl2vc->mac_list))
    {
        vsi_pw_mac_download(pl2vc, NULL, OPCODE_DOWN);
    }

    for (ALL_LIST_ELEMENTS(pl2vc->mac_list, pnode, pnextnode, pmac))
    {
        XFREE(MTYPE_VSI_ENTRY, pmac);
        list_delete_node(pl2vc->mac_list, pnode);

        gmpls.vsi_pw_num--;
    }

    if (list_isempty(pl2vc->mac_list))
    {
        list_free(pl2vc->mac_list);

        pl2vc->mac_list = NULL;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 9:22:20
 * @note       :
 */
DEFUN(vsi_interface_static_mac,
    vsi_interface_static_mac_cmd,
    "vpls mac static XX:XX:XX:XX:XX:XX interface {ethernet USP | gigabitethernet USP "
    "| xgigabitethernet USP | trunk TRUNK}",
    MPLS_CLI_INFO)
{
    struct vsi_mac_entry *pvmac = NULL;
    struct mpls_if       *pif   = NULL;
    uchar                *pmac  = NULL;
    uint32_t ifindex   = 0;
    uchar mac[MAC_LEN] = "";
    int ret = ERRNO_SUCCESS;

    /* argv[0]: MAC address */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", argv[1]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[2]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[3])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[3]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[4])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", argv[4]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[4], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    /* 配置恢复过程静态 MAC 不下发，等待接口绑定 vsi 再下发 */
    if (vty->config_read_flag != 1)
    {
        pif = mpls_if_get(ifindex);
        if (NULL == pif)
        {
            vty_error_out(vty, "The specified interface dose not exist !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        if (0 == pif->vpn)
        {
            vty_error_out(vty, "The specified interface does not belong to VSI !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        ret = vsi_static_mac_conflict(vty, vsi_lookup(pif->vpn), mac);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }

        ret = vsi_black_mac_conflict(vty, vsi_lookup(pif->vpn), mac);
        if (ERRNO_SUCCESS != ret)
        {
            return CMD_WARNING;
        }

        ret = vsi_interface_mac_download(ifindex, mac, OPCODE_ADD);
        if (ERRNO_SUCCESS != ret)
        {
            vty_error_out(vty, "%s%s", errcode_get_string(ret), VTY_NEWLINE);

            return CMD_WARNING;
        }

        pmac = (uchar *)XCALLOC(MTYPE_VSI_ENTRY, MAC_LEN);
        if (NULL == pmac)
        {
            vty_error_out(vty, "Memory alloc failed !%s", VTY_NEWLINE);

            return CMD_WARNING;
        }

        memcpy(pmac, mac, MAC_LEN);
        listnode_add(&pif->mac_list, pmac);

        gmpls.vsi_pw_num++;
    }
    else
    {
        if (config_list == NULL)
        {
            config_list = list_new();
        }

        pvmac = (struct vsi_mac_entry *)XCALLOC(MTYPE_VSI_ENTRY, sizeof(struct vsi_mac_entry));
        if (NULL != pvmac)
        {
            pvmac->ifindex = ifindex;
            memcpy(pvmac->mac, mac, MAC_LEN);

            if (config_list != NULL)
            {
                listnode_add(config_list, pvmac);
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 9:47:08
 * @note       :
 */
DEFUN(no_vsi_interface_static_mac,
    no_vsi_interface_static_mac_cmd,
    "no vpls mac static XX:XX:XX:XX:XX:XX interface {ethernet USP | gigabitethernet USP "
    "| xgigabitethernet USP | trunk TRUNK}",
    MPLS_CLI_INFO)
{
    struct mpls_if    *pif       = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnextnode = NULL;
    uchar             *pmac      = NULL;
    uint32_t ifindex   = 0;
    uchar mac[MAC_LEN] = "";

    /* argv[0]: MAC address */
    if (ether_valid_mac((char *)argv[0]))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    ether_string_to_mac((char *)argv[0], mac);

    /* mac 地址全0、组播、广播 mac 无效 */
    if ((0 == ether_is_broadcast_mac(mac))
        || (0 == ether_is_muticast_mac(mac))
        || (0 == ether_is_zero_mac(mac)))
    {
        vty_error_out(vty, "The specified MAC address is invalid.%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    if (NULL != argv[1])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", argv[1]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[2]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[3])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[3]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[4])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", argv[4]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[4], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    pif = mpls_if_get(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (0 == pif->vpn)
    {
        return CMD_SUCCESS;
    }

    for (ALL_LIST_ELEMENTS(&pif->mac_list, pnode, pnextnode, pmac))
    {
        if(NULL != pmac)
        {
            if (0 == memcmp(mac, pmac, MAC_LEN))
            {
                vsi_interface_mac_download(ifindex, pmac, OPCODE_DELETE);

                XFREE(MTYPE_VSI_ENTRY, pmac);

                list_delete_node(&pif->mac_list, pnode);

                gmpls.vsi_pw_num--;
            }
        }
    }

    return CMD_SUCCESS;
}


/**
 * @brief      :
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年5月30日 9:58:48
 * @note       :
 */
DEFUN(no_vsi_interface_static_mac_all,
    no_vsi_interface_static_mac_all_cmd,
    "no vpls mac static all interface {ethernet USP | gigabitethernet USP "
    "| xgigabitethernet USP | trunk TRUNK}",
    MPLS_CLI_INFO)
{
    struct mpls_if  *pif       = NULL;
    struct listnode *pnode     = NULL;
    struct listnode *pnextnode = NULL;
    uchar           *pmac      = NULL;
    uint32_t ifindex = 0;

    if (NULL != argv[0])
    {
        ifindex = ifm_get_ifindex_by_name("ethernet", argv[0]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[0], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[1])
    {
        ifindex = ifm_get_ifindex_by_name("gigabitethernet", argv[1]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[1], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
	else if (NULL != argv[2])
    {
        ifindex = ifm_get_ifindex_by_name("xgigabitethernet", argv[2]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[2], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }
    else if (NULL != argv[3])
    {
        ifindex = ifm_get_ifindex_by_name("trunk", argv[3]);
        if (0 == ifindex)
        {
            vty_error_out(vty, "Interface number: %s is Invalid.%s", argv[3], VTY_NEWLINE);

            return CMD_WARNING;
        }
    }

    pif = mpls_if_get(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    if (0 == pif->vpn)
    {
        return CMD_SUCCESS;
    }

    if (!list_isempty(&pif->mac_list))
    {
        vsi_interface_mac_download(ifindex, NULL, OPCODE_DOWN);
    }

    for (ALL_LIST_ELEMENTS(&pif->mac_list, pnode, pnextnode, pmac))
    {
        XFREE(MTYPE_VSI_ENTRY, pmac);
        list_delete_node(&pif->mac_list, pnode);

        gmpls.vsi_pw_num--;
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 删除 vsi 内所有 pw 的静态 mac 命令
 * @param[in ] : vsi <1-1024> - vsi 实例号
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:27:46
 * @note       :
 */
DEFUN(no_vsi_static_mac_all,
    no_vsi_static_mac_all_cmd,
    "no vpls mac static all vsi <1-1024>",
    MPLS_CLI_INFO)
{
    struct mpls_if    *pif       = NULL;
    struct vsi_entry  *pvsi      = NULL;
    struct l2vc_entry *pl2vc     = NULL;
    struct listnode   *pnode     = NULL;
    struct listnode   *pnode_mac = NULL;
    struct listnode   *pnextnode = NULL;
    uchar             *pmac      = NULL;
    uint32_t          *pifindex  = NULL;
    uint16_t vsi_id;

    /* argv[0]: vsi id */
    vsi_id = atoi(argv[0]);

    pvsi = vsi_lookup(vsi_id);
    if (NULL == pvsi)
    {
        return CMD_SUCCESS;
    }

    if ((!list_isempty(pvsi->pwlist))
        || (!list_isempty(pvsi->aclist)))
    {
        vsi_static_mac_download(vsi_id);
    }

    for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
    {
        for (ALL_LIST_ELEMENTS(pl2vc->mac_list, pnode_mac, pnextnode, pmac))
        {
            XFREE(MTYPE_VSI_ENTRY, pmac);
            list_delete_node(pl2vc->mac_list, pnode_mac);

            gmpls.vsi_pw_num--;
        }

        if ((NULL != pl2vc->mac_list) && (list_isempty(pl2vc->mac_list)))
        {
            list_free(pl2vc->mac_list);

            pl2vc->mac_list = NULL;
        }
    }

    pnode     = NULL;
    pnode_mac = NULL;
    pmac      = NULL;

    for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pifindex))
    {
        pif = mpls_if_get((uint32_t)pifindex);
        if (NULL == pif)
        {
            continue;
        }

        for (ALL_LIST_ELEMENTS(&pif->mac_list, pnode_mac, pnextnode, pmac))
        {
            XFREE(MTYPE_VSI_ENTRY, pmac);
            list_delete_node(&pif->mac_list, pnode_mac);

            gmpls.vsi_pw_num--;
        }

    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示静态 mac 配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:28:48
 * @note       :
 */
DEFUN(show_mac_static_all,
    show_mac_static_all_cmd,
    "show mac static",
    MPLS_CLI_INFO)
{
    struct hash_bucket *pbucket   = NULL;
    struct vsi_entry   *pvsi      = NULL;
    struct listnode    *pnode     = NULL;
    struct listnode    *pnode_mac = NULL;
    struct l2vc_entry  *pl2vc     = NULL;
    struct mpls_if     *pif       = NULL;
    uchar              *pmac      = NULL;
    uint32_t           *pifindex  = NULL;
    uchar mac[20] = "";
    char ifname[IFNET_NAMESIZE]   = "";
    int cursor = 0;

    if (gmpls.vsi_pw_num != 0)
    {
        vty_out(vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );
    }

    HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
    {
        pvsi = (struct vsi_entry *)pbucket->data;
        if (NULL == pvsi)
        {
            continue;
        }

        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
        {
            for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode_mac, pmac))
            {
                sprintf((char *)mac, "%02x:%02x:%02x:%02x:%02x:%02x", pmac[0], pmac[1], pmac[2],
                            pmac[3], pmac[4], pmac[5]);

                vty_out(vty, "%-21s %-7s %-7u %-31s %-9s%s", mac, " ", pvsi->vpls.vsi_id,
                            pl2vc->name, "static", VTY_NEWLINE);
            }
        }

        pnode     = NULL;
        pnode_mac = NULL;
        pmac      = NULL;

        for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pifindex))
        {
            pif = mpls_if_get((uint32_t)pifindex);
            if (NULL == pif)
            {
                continue;
            }

            memset(ifname, 0, IFNET_NAMESIZE);
            ifm_get_name_by_ifindex((uint32_t)pifindex, ifname);

            for (ALL_LIST_ELEMENTS_RO(&pif->mac_list, pnode_mac, pmac))
            {
                sprintf((char *)mac, "%02x:%02x:%02x:%02x:%02x:%02x", pmac[0], pmac[1], pmac[2],
                            pmac[3], pmac[4], pmac[5]);

                vty_out(vty, "%-21s %-7s %-7u %-31s %-9s%s", mac, " ", pvsi->vpls.vsi_id,
                            ifname, "static", VTY_NEWLINE);
            }
        }

    }

    if (gmpls.vsi_pw_num != 0)
    {
        vty_out(vty, "-------------------------------------------------------------------------------%s", VTY_NEWLINE );
        vty_out(vty, "VPLS Total: %d%s", gmpls.vsi_pw_num, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 vsi 节点下配置命令
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:33:40
 * @note       :
 */
DEFUN(show_this_vsi,
    show_this_vsi_cmd,
    "show this",
    MPLS_CLI_INFO)
{
    struct vsi_entry *pvsi = (struct vsi_entry *)vty->index;

    vty_out(vty, "#%s", VTY_NEWLINE);
    vty_out(vty, " #%s", VTY_NEWLINE);

    mpls_vsi_config_write_process(vty, pvsi);

    vty_out(vty, "#%s", VTY_NEWLINE);

    return CMD_SUCCESS;
}


/**
 * @brief      : 显示 vsi 节点下配置
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:34:06
 * @note       :
 */
int mpls_vsi_config_show_this(struct vty *vty)
{
    struct hash_bucket *pbucket = NULL;
    struct vsi_entry   *pvsi    = NULL;
    int                 cursor;

    HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
    {
        pvsi = (struct vsi_entry *)(pbucket->data);
        if (NULL == pvsi)
        {
            continue;
        }

        mpls_vsi_config_write_process(vty, pvsi);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vpls 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:35:07
 * @note       :
 */
static int mpls_vsi_config_write(struct vty *vty)
{
    struct hash_bucket *pbucket = NULL;
    struct vsi_entry   *pvsi    = NULL;
    int                 cursor;

    if (ENABLE == gmpls.enable)
    {
        vty_out(vty, "mpls%s", VTY_NEWLINE);
    }

    HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
    {
        pvsi = (struct vsi_entry *)(pbucket->data);
        if (NULL == pvsi)
        {
            continue;
        }

        mpls_vsi_config_write_process(vty, pvsi);
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : 静态 mac 配置管理
 * @param[in ] : vty - vty 全局结构
 * @param[out] :
 * @return     : 成功返回 ERRNO_SUCCESS，否则返回错误码 ERRNO_XXX
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:35:48
 * @note       :
 */
static int mpls_vsi_mac_config_write(struct vty *vty)
{
    struct mpls_if     *pif       = NULL;
    struct vsi_entry   *pvsi      = NULL;
    struct l2vc_entry  *pl2vc     = NULL;
    struct hash_bucket *pbucket   = NULL;
    struct listnode    *pnode     = NULL;
    struct listnode    *pnode_mac = NULL;
    uchar              *pmac      = NULL;
    uint32_t           *pifindex  = NULL;
    char ifname[IFNET_NAMESIZE]   = "";
    int cursor = 0;

    vty_out(vty, "mac%s", VTY_NEWLINE);

#if 0
    HASH_BUCKET_LOOP(pbucket, cursor, l2vc_table)
    {
        pl2vc = (struct l2vc_entry *)pbucket->data;
        if (NULL == pl2vc)
        {
            continue;
        }

        for (ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode, pmac))
        {
            vty_out(vty, " mac static %02x:%02x:%02x:%02x:%02x:%02x pw %s%s",
                        pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5],
                        pl2vc->name, VTY_NEWLINE);
        }
    }
#endif

    HASH_BUCKET_LOOP(pbucket, cursor, vsi_table)
    {
        pvsi = (struct vsi_entry *)pbucket->data;
        if (NULL == pvsi)
        {
            continue;
        }

        for (ALL_LIST_ELEMENTS_RO(pvsi->pwlist, pnode, pl2vc))
        {
            for(ALL_LIST_ELEMENTS_RO(pl2vc->mac_list, pnode_mac, pmac))
            {
                vty_out(vty, " vpls mac static %02x:%02x:%02x:%02x:%02x:%02x pw %s%s",
                        pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5],
                        pl2vc->name, VTY_NEWLINE);
            }
        }

        pnode     = NULL;
        pnode_mac = NULL;
        pmac      = NULL;

        for (ALL_LIST_ELEMENTS_RO(pvsi->aclist, pnode, pifindex))
        {
            pif = mpls_if_get((uint32_t)pifindex);
            if (NULL == pif)
            {
                continue;
            }

            memset(ifname, 0, IFNET_NAMESIZE);
            ifm_get_name_by_ifindex((uint32_t)pifindex, ifname);

            for (ALL_LIST_ELEMENTS_RO(&pif->mac_list, pnode_mac, pmac))
            {
                vty_out(vty, " vpls mac static %02x:%02x:%02x:%02x:%02x:%02x interface %s%s",
                        pmac[0], pmac[1], pmac[2], pmac[3], pmac[4], pmac[5],
                        ifname, VTY_NEWLINE);
            }
        }
    }

    return ERRNO_SUCCESS;
}


/**
 * @brief      : vpls 命令行注册
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月5日 9:36:37
 * @note       :
 */
void mpls_vsi_cmd_init(void)
{
    install_node (&vsi_node, mpls_vsi_config_write);
    install_node (&mac_node, mpls_vsi_mac_config_write);
    install_default (VSI_NODE);
    install_default (MAC_NODE);

    install_element (CONFIG_NODE, &mac_node_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &mpls_vsi_cmd, CMD_SYNC);
    install_element (MPLS_NODE, &no_mpls_vsi_cmd, CMD_SYNC);

    install_element (VSI_NODE, &show_this_vsi_cmd, CMD_LOCAL);
    install_element (VSI_NODE, &mpls_vsi_name_cmd, CMD_SYNC);
    install_element (VSI_NODE, &mpls_vsi_pw_failback_cmd, CMD_SYNC);
    install_element (VSI_NODE, &vsi_maclearn_cmd, CMD_SYNC);
    install_element (VSI_NODE, &vsi_mac_learn_limit_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_vsi_mac_learn_limit_cmd, CMD_SYNC);
    install_element (VSI_NODE, &vsi_mac_blacklist_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_vsi_mac_blacklist_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_vsi_mac_blacklist_all_cmd, CMD_SYNC);
    install_element (VSI_NODE, &show_vsi_mac_blacklist_cmd, CMD_LOCAL);
    install_element (VSI_NODE, &vsi_mac_whitelist_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_vsi_mac_whitelist_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_vsi_mac_whitelist_all_cmd, CMD_SYNC);
    install_element (VSI_NODE, &show_vsi_mac_whitelist_cmd, CMD_LOCAL);
    install_element (VSI_NODE, &mpls_vsi_encapsulate_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_mpls_vsi_encapsulate_cmd, CMD_SYNC);
    install_element (VSI_NODE, &mpls_vsi_storm_suppress_cmd, CMD_SYNC);
    install_element (VSI_NODE, &no_mpls_vsi_storm_suppress_cmd, CMD_SYNC);

    install_element (MAC_NODE, &vsi_pw_static_mac_cmd, CMD_SYNC);
    install_element (MAC_NODE, &no_vsi_pw_static_mac_cmd, CMD_SYNC);
    install_element (MAC_NODE, &no_vsi_pw_static_mac_all_cmd, CMD_SYNC);
    install_element (MAC_NODE, &vsi_interface_static_mac_cmd, CMD_SYNC);
    install_element (MAC_NODE, &no_vsi_interface_static_mac_cmd, CMD_SYNC);
    install_element (MAC_NODE, &no_vsi_interface_static_mac_all_cmd, CMD_SYNC);
    install_element (MAC_NODE, &no_vsi_static_mac_all_cmd, CMD_SYNC);
    install_element (CONFIG_NODE, &show_mac_static_all_cmd, CMD_LOCAL);

    install_element (PHYSICAL_IF_NODE, &mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &mpls_if_bind_vsi_cmd, CMD_SYNC);

    install_element (PHYSICAL_IF_NODE, &no_mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (PHYSICAL_SUBIF_NODE, &no_mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (TRUNK_IF_NODE, &no_mpls_if_bind_vsi_cmd, CMD_SYNC);
    install_element (TRUNK_SUBIF_NODE, &no_mpls_if_bind_vsi_cmd, CMD_SYNC);

    install_element (MPLS_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (CONFIG_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (VSI_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (PHYSICAL_IF_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (PHYSICAL_SUBIF_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (TRUNK_IF_NODE, &show_vsi_cmd, CMD_LOCAL);
    install_element (TRUNK_SUBIF_NODE, &show_vsi_cmd, CMD_LOCAL);

    return;
}


