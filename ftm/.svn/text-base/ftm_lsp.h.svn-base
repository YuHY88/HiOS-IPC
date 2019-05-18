/**
 * @file      : ftm_lsp.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:02:04
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_LSP_H
#define HIOS_FTM_LSP_H

#include <lib/types.h>
#include <lib/mpls_common.h>

/* traverse lsp list time 3s */
#define TRAVERSE_LSP_LIST_TIME  3


/* Hash of lsp table，只保存公网 lsp，用于 arp 更新 */
extern struct hash_table ftm_lsp_table;
extern struct mpls_global g_mpls;

extern void ftm_mpls_init(void);
extern void ftm_lsp_table_init(unsigned int size);
extern int ftm_lsp_add(struct lsp_entry *plsp);
extern int ftm_lsp_update_config(struct lsp_entry *plsp_old, struct lsp_entry *plsp_new);
extern int ftm_lsp_delete(uint32_t lsp_index);
extern struct lsp_entry *ftm_lsp_lookup(uint32_t lsp_index);
extern void ftm_lsp_process_arp_event(struct arp_entry *parp, enum OPCODE_E opcode);
extern int ftm_lsp_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                            enum IPC_OPCODE opcode);
extern int ftm_tpoam_msg(struct ipc_msghdr_n *phdr,void *pdata, int data_len, uint8_t msg_subtype,
                                enum IPC_OPCODE opcode, uint32_t msg_index);
extern int ftm_msg_rcv_get_bulk_lsp(struct ipc_msghdr_n *phdr,uint32_t subtype, uint32_t ifindex);


#endif


