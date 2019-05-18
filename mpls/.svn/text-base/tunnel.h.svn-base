/**
 * @file      : tunnel.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月7日 16:21:02
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_TUNNEL_H
#define HIOS_TUNNEL_H

#include <lib/mpls_common.h>


#define TUNNEL_NUM_MAX   1024    // tunnel 表的规格

#define TUNNEL_FAILBACK_IS_VALID(pif) \
    (((pif)->p_mplstp->ingress_lsp != NULL) && \
    ((pif)->p_mplstp->backup_ingress_lsp != NULL))


/* tunnel ipc 的 subtype */
enum TUNNEL_INFO
{
    TUNNEL_INFO_INVALID = 0,
    TUNNEL_INFO_BACKUP,      // 更新保护信息, 包括 lsp\backup\down_flag 字段
    TUNNEL_INFO_IP,          // 更新 IP 信息
    TUNNEL_INFO_PROTOCOL,    // 更新协议信息
    TUNNEL_INFO_COUNTER,     // 统计信息
    TUNNEL_INFO_QOS,         // gre 协议更新 qos mapping
    TUNNEL_INFO_CAR,         // gre 协议更新 qos car
    TUNNEL_INFO_HQOS,        // gre 协议更新 hqos
    TUNNEL_INFO_MPLSTP_OAM,  // mplstp 更新 oam
    TUNNEL_INFO_OUTIF,       // 更新出接口
    TUNNEL_INFO_NEXTHOP,
    TUNNEL_INFO_MAX = 12
};

extern struct hash_table tunnel_table; // tunnel hash 表，存储 struct tunnel_if, 用 ifindex 作为 key


/* 对 tunnel_table 的基本操作 */
extern void tunnel_if_table_init ( int size );
extern struct tunnel_if *tunnel_if_create( uint32_t ifindex );
extern int tunnel_if_add(struct tunnel_if *pif);
extern int tunnel_if_delete(uint32_t ifindex);
extern struct tunnel_if *tunnel_if_lookup(uint32_t ifindex);
extern void tunnel_if_up(struct tunnel_if *pif);
extern void tunnel_if_down(struct tunnel_if *pif);

/* 外部接口: 获取 tunnel 数据 */
extern int tunnel_if_get_bulk(uint32_t ifindex, struct tunnel_if tunnel_buff[]);
extern int tunnel_if_get_hqos_bulk(uint32_t ifindex, struct hqos_tunnel hqos_array[]);
extern struct counter_t *tunnel_if_get_counter(uint32_t ifindex);

/* mplstp tunnel 的操作 */
extern int tunnel_mplstp_add_lsp(struct tunnel_if *pif, struct static_lsp *plsp, enum TUNNEL_STATUS backup_flag);/* tunnel 下配置静态 lsp */
extern int tunnel_mplstp_delete_lsp(struct tunnel_if *pif, struct static_lsp *plsp);/* tunnel 下删除静态 lsp */
extern int tunnel_mplstp_set_status(struct tunnel_if *pif);
extern int tunnel_mplstp_set_status_callback(void *para);
extern int tunnel_mplstp_process_status(struct tunnel_if *pif);
extern void tunnel_mplstp_process_lsp_status(struct static_lsp *plsp);
extern void tunnel_mplstp_process_lsp_delete(struct static_lsp *plsp);
extern void tunnel_mplstp_process_lsp_event(struct static_lsp *plsp, enum OPCODE_E opcode);
extern int tunnel_if_bind_mplstp_oam(struct tunnel_if *pif, uint16_t oam_id, enum TUNNEL_STATUS backup_flag);    /* tunnel 绑定 mplstp-oam */
extern int tunnel_if_unbind_mplstp_oam(struct tunnel_if *pif, enum TUNNEL_STATUS backup_flag); /* tunnel 解绑 mplstp-oam */

/* 模块间接口: 处理 pw 与 tunnel 绑定关系 */
extern int tunnel_if_bind_pw(struct tunnel_if *pif, struct l2vc_entry *pl2vc);    /* pw 与 tunnel 绑定，在 pw 添加时调用 */
extern int tunnel_if_unbind_pw(struct tunnel_if *pif, struct l2vc_entry *pl2vc);/* pw 与 tunnel 解绑定，在 pw 删除时调用 */

/* 处理 tunnel 与 pw car */
extern int tunnel_if_process_pw_car(struct tunnel_if *pif, uint32_t cir, enum OPCODE_E opcode, int direction);

/* 处理 tunnel 与 pw 统计 */
extern void tunnel_if_set_pw_statistics(struct tunnel_if *pif, int flag);

/* tunnel 告警处理 */
extern void tunnel_if_alarm_process(uint8_t down_flag, uint32_t ifindex);

/* 处理 gre tunnel 指定 outif 相应接口事件 */
extern void tunnel_if_process_ifevent(uint32_t ifindex, enum OPCODE_E opcode);


#endif


