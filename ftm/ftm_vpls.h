/**
 * @file      : ftm_vpls.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月9日 8:52:45
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_VSI_H
#define HIOS_FTM_VSI_H

#include <mpls/vpls_cmd.h>


/* VSI 数据结构 */
struct ftm_vsi
{
    struct vpls_info vpls;
    struct list *pwlist;    // listnode 存储 struct pw_info 结构指针
    struct list *aclist;    // listnode 存储 ac 侧的 ifindex
    struct list *blacklist; // listnode 存储 mac 地址黑名单
    struct list *whitelist; // listnode 存储 mac 地址白名单
};


extern struct hash_table ftm_vsi_table;


/* VSI function Prototypes. */
extern void ftm_vsi_table_init(unsigned int size);
extern struct ftm_vsi *ftm_vsi_create(uint16_t vsi_id);
extern int ftm_vsi_add(struct ftm_vsi *pftm_vsi);
extern int ftm_vsi_delete(uint16_t vsi_id);
extern struct ftm_vsi *ftm_vsi_lookup(uint16_t vsi_id);

/* vsi MAC 黑/白名单处理 */
extern int ftm_vsi_add_mac(uint16_t vsi_id, enum VSI_INFO info, uint8_t *pmac);
extern int ftm_vsi_delete_mac(uint16_t vsi_id, enum VSI_INFO info, uint8_t *pmac);
extern int ftm_vsi_delete_mac_all(uint16_t vsi_id, enum VSI_INFO info);
extern uint8_t *ftm_vsi_lookup_mac(struct list *plist, uint8_t *pmac);

/* PW function prototypes */
extern int ftm_vsi_add_pw(uint16_t vsi_id, uint32_t pwindex);
extern int ftm_vsi_delete_pw(uint16_t vsi_id, uint32_t pwindex);
extern int ftm_vsi_delete_pw_all(uint16_t vsi_id);
extern struct pw_info *ftm_vsi_lookup_pw(uint16_t vsi_id, uint32_t pwindex);

/* interface function prototypes */
extern int ftm_vsi_add_if(uint16_t vsi_id, uint32_t ifindex);
extern int ftm_vsi_delete_if(uint16_t vsi_id, uint32_t ifindex);
extern int ftm_vsi_delete_if_all(uint16_t vsi_id);
extern int ftm_vsi_mac_learn(uint16_t vsi_id, enum MAC_LEARN mac_learn_type);
extern int ftm_vsi_mac_learn_limit(uint16_t vsi_id, int limit_num);
extern int ftm_vsi_update_pw_tag(struct vpls_info* pvpls_info);
extern int ftm_vsi_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                            enum IPC_OPCODE opcode, uint32_t index);


#endif


