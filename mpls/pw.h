/**
 * @file      : pw.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月1日 10:16:42
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_PW_H
#define HIOS_PW_H

#include "mpls_if.h"
#include "ldp/ldp_pw.h"

#define PW_FAILBACK_IS_VALID(pif) \
    (((pif)->ppw_master != NULL) && \
    ((pif)->ppw_slave != NULL))


enum PW_SUBTYPE
{
    PW_SUBTYPE_INVALID = 0,
    PW_SUBTYPE_DOWN,       // pw down
    PW_SUBTYPE_UP,         // pw up
    PW_SUBTYPE_HQOS,       // qos hqos
    PW_SUBTYPE_QOS,        // qos mapping
    PW_SUBTYPE_CAR,        // qos car
    PW_SUBTYPE_COUNTER,    // 统计
};


extern struct hash_table l2vc_table; // l2vc hash 表，存储 struct l2vc_entry，以 pw name 作为 key
extern struct hash_table pw_table;   // pw hash 表，存储 struct pw_info，以 pwindex 作为 key


/* 对 l2vc_table 的基本操作 */
extern void l2vc_table_init(uint32_t size);
extern void l2vc_free(struct l2vc_entry *pl2vc);
extern struct l2vc_entry *l2vc_create(void);
extern int l2vc_add(struct l2vc_entry *ppw);
extern int l2vc_delete(uchar *pname);
extern struct l2vc_entry *l2vc_lookup(uchar *pname);

/* 外部接口: 获取 pw 数据 */
extern struct counter_t *l2vc_get_counter(uchar *pname);
extern int l2vc_get_bulk(uchar *pname, int data_len, struct l2vc_entry l2vc_buff[]);

/* pw 选择隧道操作 */
extern uint32_t l2vc_select_tunnel_ldp_lsp(struct l2vc_entry *pl2vc);
extern void l2vc_select_tunnel_new(struct pw_info *ppwinfo);
extern struct tunnel_if *l2vc_select_tunnel_interface(struct l2vc_entry *pl2vc);
extern int l2vc_select_tunnel(struct l2vc_entry *pl2vc);
extern int l2vc_unbind_lsp_tunnel(struct l2vc_entry *pl2vc);


/* 模块间接口: 处理 tunnel up 事件 */
extern void l2vc_process_tunnel_interface_up(struct tunnel_if *pif);

/* 模块间接口: 处理静态 lsp up/down 事件 */
extern void l2vc_process_tunnel_static_lsp_up(struct static_lsp *plsp);
extern void l2vc_process_tunnel_static_lsp_down(struct static_lsp *plsp);

/* 模块间接口: 处理 ldp 隧道与动态 pw */
extern void l2vc_process_tunnel_ldp_lsp_add(void);
extern void l2vc_process_tunnel_ldp_lsp_del(uint32_t lsp_index);
extern int l2vc_add_martini(struct l2vc_entry *pl2vc);
extern int l2vc_delete_martini(struct l2vc_entry *pl2vc);
extern int l2vc_add_label(struct ldp_pwinfo *pldp_pwinfo);
extern int l2vc_delete_label(struct l2vc_entry *pl2vc);

/* pw 告警处理 */
extern void l2vc_alarm_process(enum OPCODE_E opcode, uint32_t pwindex);

/* 对 pw_table 的基本操作 */
extern void pw_table_init(uint32_t size);
extern int pw_add(struct pw_info *ppw);
extern int pw_delete(uint32_t pwindex);
extern struct pw_info *pw_lookup(uint32_t pwindex);
extern struct l2vc_entry *pw_get_l2vc(struct pw_info *pwinfo);

/* pw 对私网 lsp 的操作 */
extern int pw_get_lsp_index(struct pw_info *ppwinfo);
extern int pw_free_lsp_index(struct pw_info *ppwinfo);
extern int pw_add_lsp(struct l2vc_entry *pl2vc);
extern int pw_add_lsp_transit(struct l2vc_entry *pl2vc, struct l2vc_entry *pl2vc_switch);
extern int pw_delete_lsp(struct pw_info *ppwinfo);

/* 处理 pw up/down 状态 */
extern void pw_tunnel_down(struct pw_info *ppwinfo);
extern void pw_tunnel_up(struct pw_info *ppwinfo);
extern void pw_detection_protocal_down(struct pw_info *ppwinfo);
extern void pw_detection_protocal_up(struct pw_info *ppwinfo);
extern int pw_set_pw_status(struct mpls_if *pif);

/* 获取 PW 信息 */
extern uint32_t pw_get_index(uchar *pname);
extern uchar *pw_get_name(uint32_t pwindex);
extern int pw_get_hqos_bulk(uchar *pname, int data_len, struct hqos_pw *hqos_array);

extern void l2vc_process_tunnel_ldp_lsp_frr(uint32_t lsp_index_old, uint32_t lsp_index_new, uint32_t group_index);
#endif


