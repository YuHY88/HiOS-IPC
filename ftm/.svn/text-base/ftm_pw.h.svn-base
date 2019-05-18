/**
 * @file      : ftm_pw.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:43:43
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_PW_H
#define HIOS_FTM_PW_H

#include <mpls/pw.h>


/* l2 模式的接口数据结构 */
struct ftm_ifm_l2
{
    uint16_t       vsi;          // 接口绑定的 vsi
    uint16_t       pad;
    uint32_t       master_index; // 主 pw 索引
    uint32_t       backup_index; // 备 pw 索引
    enum BACKUP_E  pw_backup;    // 0: 主工作, 1: 备工作
};


/* Hash of pw table */
extern struct hash_table ftm_pw_table;


/* Prototypes of pw index hash */
extern void ftm_pw_table_init(uint32_t size);
extern struct pw_info *ftm_pw_create(void);
extern int ftm_pw_add(struct pw_info *ppw);
extern int ftm_pw_upadte_status(uint32_t pwindex, enum PW_SUBTYPE subtype);
extern int ftm_pw_backup_status_update(uint32_t ifindex, enum BACKUP_E pw_backup);
extern int ftm_pw_update(struct pw_info *ppw_old, struct pw_info *ppw_new);
extern int ftm_pw_delete(uint32_t pwindex);
extern struct pw_info *ftm_pw_lookup(uint32_t pwindex);
extern int ftm_pw_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                            enum IPC_OPCODE opcode);
extern int ftm_pw_bind_if(uint32_t ifindex, uint32_t mpls_index, enum MPLSIF_INFO subtype);
extern int ftm_pw_unbind_if(uint32_t ifindex, uint32_t mpls_index, enum MPLSIF_INFO subtype);
extern int ftm_mplsif_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                                enum IPC_OPCODE opcode, uint32_t index);


#endif


