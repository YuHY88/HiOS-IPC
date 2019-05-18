/**
 * @file      : ftm_tunnel.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月9日 9:19:25
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_TUNNEL_H
#define HIOS_FTM_TUNNEL_H


extern struct hash_table ftm_gre_table; // GRE tunnel hash 表，用 sip 和 dip 做 key

extern void ftm_gre_table_init(int size);
extern int  ftm_gre_tunnel_add(struct tunnel_t *tunnel);
extern int  ftm_gre_tunnel_delete(struct tunnel_t *tunnel);
extern struct tunnel_t *ftm_gre_tunnel_lookup(struct inet_addr *psip, struct inet_addr *pdip);
extern int  ftm_gre_tunnel_update(struct tunnel_t *ptunnel_old, struct tunnel_t *ptunnel);
extern int ftm_tunnel_process_arp_event(struct arp_entry *parp, enum OPCODE_E opcode);

extern int ftm_tunnel_add(struct tunnel_t *tunnel);
extern int ftm_tunnel_delete(uint32_t ifindex);
extern struct tunnel_t *ftm_tunnel_lookup(uint32_t ifindex);
extern int ftm_tunnel_update(struct tunnel_t *tunnel);
extern int ftm_tunnel_msg(void *pdata, int data_len, int data_num, uint8_t subtype,
                                    enum IPC_OPCODE opcode, uint32_t ifindex);


#endif


