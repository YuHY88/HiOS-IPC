/**
 * @file      : cfm_lt.h
 * @brief     : define of 802.1ag and Y.1731 lt
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:33:52
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */


#ifndef HIOS_CFM_LT_H
#define  HIOS_CFM_LT_H

#include <lib/types.h>
#include <lib/oam_common.h>
#include <sys/time.h>

#include "ftm/pkt_eth.h"

enum cfm_ltm_relay_action
{
	CFM_LTM_RELAY_HIT      = 1,  /* LTM到达指定终点 */ 
	CFM_LTM_RELAY_FDB      = 2,  /* LTM根据FDB继续转发 */ 
	CFM_LTM_RELAY_MPDB     = 3,  /* LTM根据MIP database 继续转发 */
	CFM_LTM_RELAY_MAX      = 4,
};

enum
{
	Reply_Ingress_Tlv = 5,
	LTM_Egress_Identifier_Tlv = 7,		
	LTR_Egress_Identifier_Tlv = 8
};

struct cfm_ltm_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;
	uint32_t              trans_id;        /* 交易ID */
	uint8_t               ttl;
	uint8_t               origin_mac[6];   /* 发起LT测试的源MAC */
	uint8_t               target_mac[6];   /* LT测试的目的MAC */
	//uint8_t               end_tlv;
	//uint8_t               rev[12];
	uint8_t				  tlvs[200];	
}__attribute__((packed));

struct cfm_ltr_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;     
	uint32_t              trans_id;       /* 交易ID */
	uint8_t               ttl;
	uint8_t               action;         /* LTM的中继动作 */ 
	//uint8_t               end_tlv;
	//uint8_t               rev[23];
	uint8_t				  tlvs[200];
}__attribute__((packed));

struct cfm_lt
{
	uint8_t    	   priority;    /* 报文的优先级 */
	uint8_t        ttl;
	uint8_t        ltr_rx_timeout;       /* LT 测试超时时间， 默认2s */
	uint8_t        target_mac[6];        /* LT测试的目的MAC */
	uint32_t       next_trans_id;
	uint8_t        remain_len;
	
	struct timeval ltm_tx_time;          /* LTM 的发送时间 */ 
	struct timeval ltr_rx_time;          /* LTR 的接收时间 */ 

	struct cfm_ltm_pdu ltm_pdu;

	void           *vty;
};

int cfm_lt_start(struct cfm_sess *sess, void *vty, uchar *dmac);
int cfm_lt_stop(struct cfm_sess *sess);
int cfm_rcv_ltm(struct cfm_sess *sess, struct pkt_buffer *pkt);
int cfm_rcv_ltr(struct cfm_sess *sess, struct pkt_buffer *pkt);

#endif

