/**
 * @file      : lsp_static.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 14:03:08
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_STATIC_LSP_H
#define HIOS_STATIC_LSP_H

#include <lib/mpls_common.h>


extern struct hash_table static_lsp_table; // 静态 lsp hash 表，存储 struct static_lsp，以 lsp name 作为 key

/* 对 static_lsp_table 的基本操作 */
extern void static_lsp_table_init(unsigned int size);
extern void static_lsp_free(struct static_lsp *plsp);
extern struct static_lsp *static_lsp_create(void);
extern int static_lsp_add(struct static_lsp *plsp);
extern int static_lsp_delete(uchar *pname);
extern struct static_lsp *static_lsp_lookup(uchar *pname);

/* 外部接口: 获取静态 lsp 数据 */
extern int static_lsp_get_bulk(uchar *pname, int data_len, struct static_lsp slsp_buff[]);

/* 模块间接口: 获取静态 lsp 数据 */
extern uint32_t static_lsp_get_index(uchar *pname);
extern uchar *static_lsp_get_name(uint32_t index);

/* 静态 lsp 状态处理 */
extern void static_lsp_admin_up(struct static_lsp *plsp);
extern void static_lsp_admin_down(struct static_lsp *plsp);
extern void static_lsp_up(struct static_lsp *plsp);
extern void static_lsp_down(struct static_lsp *plsp);
extern void static_lsp_enable(struct static_lsp *plsp);
extern void static_lsp_disable(struct static_lsp *plsp);

/* 静态 lsp 响应接口事件 */
extern void static_lsp_process_ifup(uint32_t ifindex);
extern void static_lsp_process_ifdown(uint32_t ifindex);
extern void static_lsp_process_ifevent(uint32_t ifindex, int mode, enum OPCODE_E opcode);
extern void static_lsp_process_arp(uint32_t lsp_index, enum OPCODE_E opcode, uint32_t nhp_index);

extern struct static_lsp *static_lsp_select(uint32_t destip);


#endif


