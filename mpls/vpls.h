/**
 * @file      : vpls.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月5日 9:38:09
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_VSI_H
#define HIOS_VSI_H

#include <lib/mpls_common.h>

#define VSI_MAC_NUM_MAX 512


/* VSI 消息类型 */
enum VSI_INFO
{
    VSI_INFO_INVALID                  = 0,
    VSI_INFO_PW                       = 1,
    VSI_INFO_AC                       = 2,
    VSI_INFO_MAC_LEARN                = 3,
    VSI_INFO_VSI                      = 4,
    VSI_INFO_NAME                     = 5,
    VSI_INFO_LEARN_LIMIT              = 6,
    VSI_INFO_MAC_BLACKLIST            = 7,
    VSI_INFO_MAC_WHITELIST            = 8,
    VSI_INFO_MAC_BLACKLIST_ALL        = 9,
    VSI_INFO_MAC_WHITELIST_ALL        = 10,
    VSI_INFO_QOS_CAR                  = 11,
    VSI_INFO_ENCAP                    = 12,
    VSI_INFO_STORM_SUPPRESS_UC        = 13,
    VSI_INFO_STORM_SUPPRESS_BC        = 14,
    VSI_INFO_STORM_SUPPRESS_MC        = 15,
    VSI_INFO_STATIC_MAC               = 16,
    VSI_INFO_STATIC_MAC_ALL           = 17,
    VSI_INFO_VSI_STATIC_MAC           = 18,
    VSI_INFO_PW_BACKUP                = 19,
    VSI_INFO_INTERFACE_STATIC_MAC     = 20,
    VSI_INFO_INTERFACE_STATIC_MAC_ALL = 21,
    VSI_INFO_DYNAMIC_MAC_BULK_GET     = 22,
};


extern struct hash_table vsi_table; // vsi hash 表，存储 struct vsi_entry，以 vsi_id 作为 key

/* 对 vsi_entry 的基本处理 */
extern void vsi_table_init(uint32_t size);
extern void vsi_free(struct vsi_entry *pvsi);
extern struct vsi_entry *vsi_create(uint16_t vsi_id);
extern int vsi_add(struct vsi_entry *pvsi);
extern int vsi_delete(uint16_t vsi_id);
extern struct vsi_entry *vsi_lookup(uint16_t vsi_id);

/* 外部接口: 获取 vsi 数据 */
extern int vsi_get_bulk(uint32_t vsi_id, struct vsi_entry vsi_buff[]);
extern int vsi_get_vsiif_bulk(uint32_t vsi_id, uint32_t ifindex, uint32_t if_buff[]);

/* vsi 处理 pw 的操作 */
extern struct l2vc_entry *vsi_lookup_pw(uint16_t vsi_id, uint32_t pwindex);
extern int vsi_add_pw(struct vsi_entry *pvsi, struct l2vc_entry *pl2vc);
extern int vsi_delete_pw(struct vsi_entry *pvsi, struct l2vc_entry *pl2vc);
extern int vsi_delete_pw_all(struct vsi_entry *pvsi);

/* vsi 处理接口的操作 */
extern int vsi_add_if(struct vsi_entry *pvsi, uint32_t ifindex);
extern int vsi_delete_if(struct vsi_entry *pvsi, uint32_t ifindex);
extern int vsi_delete_if_all(struct vsi_entry *pvsi);

/* vsi 处理 MAC 黑/白名单的操作 */
extern int vsi_add_mac(struct list *plist, uint8_t *pmac);
extern int vsi_delete_mac(struct list *plist, uint8_t *pmac);
extern int vsi_delete_mac_all(struct list *plist);
extern uint8_t *vsi_lookup_mac(struct list *plist, uint8_t *pmac);

/* vsi 处理 pw 主备切换 */
extern int vsi_set_pw_status(struct l2vc_entry *pl2vc);


#endif


