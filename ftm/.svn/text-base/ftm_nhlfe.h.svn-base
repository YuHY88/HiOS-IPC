/**
 * @file      : ftm_nhlfe.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 17:24:51
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_FTM_NHLFE_H
#define HIOS_FTM_NHLFE_H

#include <lib/mpls_common.h>
#include <mpls/lspm.h>


/* nhlfe 数据结构 */
struct nhlfe_entry
{
    uint32_t lsp_index;
    uint32_t outlabel;
    uint32_t inlabel;         // 双向 lsp 的入标签
    uint32_t pw_index;
    uint32_t nhp_index;       // if nhp_type = NHP_TYPE_CONNECT, this is ifindex
    uint32_t group_index;     // protect-group for lsp, tunnelif for te-lsp
    uint32_t nexthop;         // if nexthop==0 then dmac is valid
    enum LSP_TYPE lsp_type;   // 0: invalid, 1: vpws, 2: pwe3, 3: vpls, 4: l3vpn, 5: static-lsp 6: ldp-lsp 7: te-lsp
    enum NHP_TYPE nhp_type;   // 0: invalid, 1: ip route, 2: tocpu, 3: connect, 4: lsp 5: frr 6: ecmp
    enum LABEL_ACTION action;
	uchar    dmac[6];         // 默认值是全 0
	uint8_t  ttl;             // 默认值是 255
	uint8_t  exp;             // 默认值是 1
	uint16_t vlan;            // for arp vlan
	uint16_t cvlan;           // for arp cvlan
    uint16_t mplsoam_id;      // mplstp oam session id
    uint16_t mplsoam_sec_vid; // mplstp oam section vlan id
	uint16_t pad;
    uint16_t hqos_id;         // hqos queue id
    uint32_t car_cir;         // qos car cir
    uint32_t car_pir;         // qos car pir
    uint16_t vpnid;
    uint8_t  phb_id;          // qos phb，默认是 phb 0
    uint8_t  phb_enable;      // 0 表示 exp copy，1 表示 phb 有效
	uint8_t  flag;            // 1: arp is valid
	uint8_t  down_flag;       // 1: down, 0 : up
    uint8_t  statis_enable;   // 1: 表示统计使能
    uint8_t  ttl_copy;        // 1: copy 上一级的 ttl，默认是 0
	void    *couter;
};

struct nhlfe_frr
{
	uint32_t index;
	uint32_t master_lsp_index;
	uint32_t backup_lsp_index;
};

/* Hash of nhlfe table */
extern struct hash_table nhlfe_table;


extern void ftm_nhlfe_table_init(unsigned int size);
extern int ftm_nhlfe_add(struct nhlfe_entry *pnhlfe);
extern int ftm_nhlfe_update_config(struct nhlfe_entry *pnhlfe,
                                                struct nhlfe_entry *pnhlfe_new);
extern int ftm_nhlfe_update(struct nhlfe_entry *pnhlfe, enum LSP_SUBTYPE subtype);
extern int ftm_nhlfe_update_arp(struct nhlfe_entry *pnhlfe, struct arp_entry *parp);
extern int ftm_nhlfe_delete(uint32_t lsp_index);
extern struct nhlfe_entry *ftm_nhlfe_lookup(uint32_t lsp_index);
extern int ftm_nhlfe_send_to_hal(struct nhlfe_entry *pnhlfe, enum IPC_OPCODE opcode,
                                            enum LSP_SUBTYPE subtype);
extern int ftm_nhlfe_send_to_mpls(struct nhlfe_entry *pnhlfe, enum IPC_OPCODE opcode,
                                            enum LSP_SUBTYPE subtype);


#endif


