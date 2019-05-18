/**
 * @file      : vpls_cmd.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 16:07:49
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_VSI_CMD_H
#define HIOS_VSI_CMD_H

#include "vpls.h"

enum MAC_LEARN
{
    MAC_LEARN_DISABLE = 0,
    MAC_LEARN_ENABLE,
    MAC_LEARN_MAX = 8
};


/* ac 接口静态 MAC 配置恢复数据结构 */
struct vsi_mac_entry
{
    uint32_t ifindex;
    uchar    mac[MAC_LEN];
};


extern int vsi_pwinfo_download(struct pw_info *ppwinfo, enum OPCODE_E opcode);
extern int vsi_ac_interface_download(struct vsi_entry *pvsi, uint32_t ifindex, enum OPCODE_E opcode);
extern int vsi_pw_status_download(uint32_t ifindex, void *pinfo);
extern int vsi_pw_name_download(struct l2vc_entry *pl2vc, enum OPCODE_E opcode);
extern int vsi_white_black_mac_download(struct vsi_entry *pvsi, uint8_t *pmac, enum VSI_INFO info, enum OPCODE_E opcode);
extern int vsi_interface_mac_download(uint32_t ifindex, uchar *pmac, enum OPCODE_E opcode);
extern void mpls_vsi_cmd_init(void);
extern int mpls_vsi_config_show_this(struct vty *vty);


#endif


