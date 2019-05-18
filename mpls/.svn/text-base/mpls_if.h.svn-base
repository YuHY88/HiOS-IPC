/**
 * @file      : mpls_if.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 8:48:08
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_MPLS_IF_H
#define HIOS_MPLS_IF_H

#include <lib/hash1.h>
#include <lib/mpls_common.h>
#include <mpls/ldp/ldp_adjance.h>

/* mpls 接口信息，用作 IPC 的 subtype */
enum MPLSIF_INFO
{
    MPLSIF_INFO_INVALID = 0,
    MPLSIF_INFO_VPN,        // vsi
    MPLSIF_INFO_MASTER_PW,  // 主 pw
    MPLSIF_INFO_SLAVE_PW,   // 备 pw
    MPLSIF_INFO_PW_BACKUP,  // pw 的主备工作状态
    MPLSIF_INFO_AC_STATUS   // PW 绑定的 AC 侧状态
};


enum MPLS_IF_CES_MODE
{
    MPLS_IF_CES_MODE_INVALID = 0,
    MPLS_IF_CES_MODE_UNFRAMED,      // unframe
    MPLS_IF_CES_MODE_FRAMED,        // frame
    MPLS_IF_CES_MODE_MULTIFRAMED    // multiple frame
};


/* mpls 模块保存的接口配置 */
struct mpls_if
{
    uint32_t           ifindex;
    uint8_t            mode;        // 接口模式
    uint8_t            down_flag;   // 0: up, 1: down
    uint16_t           vpn;         // VSI ID
    uint32_t           ipv4;        // 接口的 IPV4 地址
    uint16_t           bfd_sessid;  // 保存使能的 bfd session id
    uint16_t           mplsoam_id;  // mplstp oam session id
    uint8_t            ospf_flag;   // 接口下路由协议使能标志
    uint8_t            isis_flag;
    uint8_t            rip_flag;
    uint8_t            bgp_flag;
    uint8_t            tdm_mode;    // 0:invalid 1:unfreme 2:frame 3:multiple frame

    uint16_t           aps_sessid;  // 保存使能的 aps session id
    enum BACKUP_E      pw_backup;   // 1: 主 pw 工作, 2: 备 pw 工作
    struct list       *list;        // 保存 bfd 会话 struct bfd_ip_map
    struct list        mac_list;    // 加入 vsi 接口下配置静态 MAC
    struct l2vc_entry *ppw_master;
    struct l2vc_entry *ppw_slave;
    struct ldp_if      ldp_conf;        // ldp config
};


extern struct hash_table mpls_if_table; // mpls 模块的接口表，用于保存接口下的配置信息


extern void mpls_if_table_init(int size);
extern void mpls_if_cmd_init(void);
extern struct mpls_if * mpls_if_create(uint32_t ifindex);
extern struct mpls_if * mpls_if_get(uint32_t ifindex);
extern int mpls_if_add(struct mpls_if *pif);
extern struct mpls_if *mpls_if_lookup(uint32_t ifindex);
extern void mpls_if_delete(uint32_t ifindex);
extern void mpls_if_down(uint32_t ifindex, int mode);
extern void mpls_if_up(uint32_t ifindex, int mode);
extern void mpls_if_mode_change(uint32_t ifindex, int mode);

extern int mpls_if_delete_interface_bfd(struct mpls_if *pif);
extern int mpls_if_delete_local_ldp(struct mpls_if *pif);
extern int mpls_if_delete_pw(struct mpls_if *pif);
extern int mpls_if_delete_vpls(struct mpls_if *pif);

extern int mpls_if_bind_pw(struct mpls_if *pif, struct l2vc_entry *pl2vc);
extern int mpls_if_unbind_pw(struct mpls_if *pif, struct l2vc_entry *pl2vc);

extern void mpls_if_down_for_pw(struct mpls_if *pif);
extern void mpls_if_up_for_pw(struct mpls_if *pif);

extern int mpls_if_bind_bfd_sess(uint32_t ifindex, uint16_t sess_id);
extern int mpls_if_unbind_bfd_sess(uint32_t ifindex);

extern int mpls_if_bind_mplsoam_sess(struct mpls_if *pif, uint16_t local_id);
extern int mpls_if_unbind_mplsoam_sess(struct mpls_if *pif);


#endif


