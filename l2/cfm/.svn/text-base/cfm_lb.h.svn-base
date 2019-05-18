/**
 * @file      : cfm_lb.h
 * @brief     : define of 802.1ag and Y.1731 lb
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:33:26
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */


#ifndef HIOS_CFM_LB_H
#define  HIOS_CFM_LB_H

#include <lib/types.h>
#include <lib/oam_common.h>
#include <sys/time.h>

#include "ftm/pkt_eth.h"

/* CFM LB 的报文结构 */
struct cfm_lb_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;   
	uint32_t              trans_id;
	//uint8_t               end_tlv;
	//uint8_t               rev[21];
	uint8_t 			   tlvs[1500];
}__attribute__((packed));

/* CFM LB 的数据结构 */
struct cfm_lb
{
	uint8_t        lbm_type;        /* LBM 报文的类型， 1:单播， 2:组播 */
	uint8_t    	   lbm_priority;    /* 报文的优先级 */	
	uint32_t       lbm_tx_count;    /* LBM 报文的发送数量， 默认5个 */ 
	uint32_t       lbm_tx_interval; /* LBM 报文的发送周期， 等于LBR的接收超时时间， 默认为2s */
	uint32_t       lbr_rx_timeout;  /* LBR 的接收超时时间， 默认为2s */ 
    uint32_t       lbm_len;         /* LBM 报文长度 */
    uint32_t       next_trans_id;

	struct cfm_lb_pdu  lbm_pdu;

	struct timeval lbm_tx_time;     /* LBM 的发送时间 */ 
	struct timeval lbr_rx_time;     /* LBR 的接收时间 */ 

	uint32_t       lbr_rx_count;    /* LBR 报文的接收数量 */ 
	uint32_t       delay_sum;       /* LB 的时延总数 */
	uint32_t       delay_min;       /* LB 的最小时延 */
	uint32_t       delay_max;       /* LB 的最大时延 */

	void           *vty;
};

int cfm_lb_start(struct cfm_sess *sess, void *vty, uchar *dmac,uint32_t size);
int cfm_lb_stop(struct cfm_sess *sess);
int cfm_rcv_lbm(struct cfm_sess *sess, struct pkt_buffer *pkt);
int cfm_rcv_lbr(struct cfm_sess *sess, struct pkt_buffer *pkt);

#endif
