/**
 * @file      : ftm_ilm.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:36:16
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_ILM_H
#define HIOS_FTM_ILM_H

#include <mpls/lspm.h>


/* ilm 数据结构 */
struct ilm_entry
{
    uint32_t lsp_index;
    uint32_t inlabel;
    uint32_t ac_index;        // ac ifindex for pw
    uint32_t pw_index;
    uint32_t group_index;     // protect-group for lsp, tunnelif for te-lsp
    uint32_t nhp_index;       // 出接口索引
    enum NHP_TYPE nhp_type;   // 下一跳的类型
    enum LSP_TYPE lsp_type;   // 本条 lsp 的类型
    enum LABEL_ACTION action; // label action
    uint16_t vpnid;           // for l3vpn and vpls
    uint8_t  pad[2];
    uint32_t car_cir;         // qos car cir
    uint32_t car_pir;         // qos car pir
    uint8_t  domain_id;       // qos domain，默认是 domain 0
    uint8_t  statis_enable;   // 1: 表示统计使能
	uint8_t  ttl;             // 默认值是 255
	uint8_t  exp;             // 默认值是 1
    void    *couter;          // 统计计数
};


/* Hash of ilm table */
extern struct hash_table ilm_table;


extern void ftm_ilm_table_init(unsigned int size);
extern int ftm_ilm_add(struct ilm_entry *pilm);
extern int ftm_ilm_update(struct ilm_entry *pilm, enum LSP_SUBTYPE subtype);
extern int ftm_ilm_update_config(struct ilm_entry *pilm, struct ilm_entry *pilm_new);
extern int ftm_ilm_delete(uint32_t label);
extern struct ilm_entry *ftm_ilm_lookup(uint32_t label);
extern int ftm_ilm_send_to_hal(struct ilm_entry *pilm, enum IPC_OPCODE opcode,
                                         enum LSP_SUBTYPE subtype);


#endif


