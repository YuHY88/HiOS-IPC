/**
 * @file      : lspm.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月2日 15:18:10
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_LSPM_H
#define HIOS_LSPM_H

#include <lib/hash1.h>
#include <lib/mpls_common.h>


/* lsp update 时的 subtype */
enum LSP_SUBTYPE
{
    LSP_SUBTYPE_INVALID = 0,
    LSP_SUBTYPE_DOWN,       // lsp down
    LSP_SUBTYPE_UP,         // lsp up
    LSP_SUBTYPE_GROUP,      // lsp group_index
    LSP_SUBTYPE_QOS,        // qos mapping
    LSP_SUBTYPE_CAR,        // qos car
    LSP_SUBTYPE_HQOS,       // hqos
    LSP_SUBTYPE_COUNTER,    // statistics enable
    LSP_SUBTYPE_MPLSTP_OAM, // mplstp oam session
    LSP_SUBTYPE_INLABEL,    // bidirectional lsp inlabel
    LSP_SUBTYPE_TUNNEL,     // lsp bind tunnel
    LSP_SUBTYPE_LSR_ID,     // mpls lsr id
    LSP_SUBTYPE_LSRV6_ID,   // mpls lsrv6 id
};


extern struct hash_table lsp_table; // lspm hash 表，存储 struct lsp_entry，以 lsp_index 作为 key

/* 对 lsp_table 的基本操作 */
extern void mpls_lsp_table_init(unsigned int size);
extern void mpls_lsp_free(struct lsp_entry *plsp);
extern struct lsp_entry *mpls_lsp_create(void);
extern struct lsp_entry *mpls_lsp_new(uint32_t inlabel, uint32_t outlabel, enum LSP_TYPE type);
extern int mpls_lsp_add(struct lsp_entry *lsp);
extern int mpls_lsp_delete(uint32_t lsp_index);
extern struct lsp_entry *mpls_lsp_lookup(uint32_t lsp_index);
extern int mpls_lsp_update(struct lsp_entry *plsp, enum LSP_SUBTYPE subtype);

/* 处理 lsp 索引分配与释放 */
extern int mpls_lsp_alloc_index(void);
extern int mpls_lsp_free_index(uint32_t lsp_index);

/* 处理 lsp up/down 事件 */
extern void mpls_lsp_down(uint32_t lsp_index);
extern void mpls_lsp_up(uint32_t lsp_index);

/* 模块间接口: 通过 lsp 索引获取静态 lsp 名字 */
extern uchar *mpls_lsp_get_name(uint32_t lsp_index);

/* 初始化设备启动时默认配置 */
extern void mpls_lsp_init(void);


#endif


