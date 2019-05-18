/**
 * @file      : cfm_lb.h
 * @brief     : define of 802.1ag and Y.1731 lb
 * @details   : 
 * @author    : huoqq
 * @date      : 2018��3��23�� 14:33:26
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

/* CFM LB �ı��Ľṹ */
struct cfm_lb_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;   
	uint32_t              trans_id;
	//uint8_t               end_tlv;
	//uint8_t               rev[21];
	uint8_t 			   tlvs[1500];
}__attribute__((packed));

/* CFM LB �����ݽṹ */
struct cfm_lb
{
	uint8_t        lbm_type;        /* LBM ���ĵ����ͣ� 1:������ 2:�鲥 */
	uint8_t    	   lbm_priority;    /* ���ĵ����ȼ� */	
	uint32_t       lbm_tx_count;    /* LBM ���ĵķ��������� Ĭ��5�� */ 
	uint32_t       lbm_tx_interval; /* LBM ���ĵķ������ڣ� ����LBR�Ľ��ճ�ʱʱ�䣬 Ĭ��Ϊ2s */
	uint32_t       lbr_rx_timeout;  /* LBR �Ľ��ճ�ʱʱ�䣬 Ĭ��Ϊ2s */ 
    uint32_t       lbm_len;         /* LBM ���ĳ��� */
    uint32_t       next_trans_id;

	struct cfm_lb_pdu  lbm_pdu;

	struct timeval lbm_tx_time;     /* LBM �ķ���ʱ�� */ 
	struct timeval lbr_rx_time;     /* LBR �Ľ���ʱ�� */ 

	uint32_t       lbr_rx_count;    /* LBR ���ĵĽ������� */ 
	uint32_t       delay_sum;       /* LB ��ʱ������ */
	uint32_t       delay_min;       /* LB ����Сʱ�� */
	uint32_t       delay_max;       /* LB �����ʱ�� */

	void           *vty;
};

int cfm_lb_start(struct cfm_sess *sess, void *vty, uchar *dmac,uint32_t size);
int cfm_lb_stop(struct cfm_sess *sess);
int cfm_rcv_lbm(struct cfm_sess *sess, struct pkt_buffer *pkt);
int cfm_rcv_lbr(struct cfm_sess *sess, struct pkt_buffer *pkt);

#endif
