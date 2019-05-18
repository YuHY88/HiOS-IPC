/**
 * @file      : l2vpn_cmd.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年4月25日 14:14:52
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_L2VPN_CMD_H
#define HIOS_L2VPN_CMD_H

#include "l2vpn_h3c.h"


extern void l2vpn_cmd_h3c_init(void);
extern int l2vpn_service_instance_config_write(struct vty *vty);
extern int l2vpn_pw_class_config_write(struct vty *vty);
extern int l2vpn_xc_group_config_write(struct vty *vty);
extern int l2vpn_connection_config_write(struct vty *vty, struct xc_group *pgroup);


#endif


