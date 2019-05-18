/**
 * @file      : cfm_test.h
 * @brief     : define of Y.1731 test,lck
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:34:42
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */


#ifndef HIOS_CFM_TEST_H
#define  HIOS_CFM_TEST_H

#include <lib/types.h>
#include <lib/oam_common.h>
#include <sys/time.h>

#include "lib/hptimer.h"


#include "ftm/pkt_eth.h"

struct cfm_lck_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;   
	uint8_t               end_tlv;
	uint8_t               rev[25];
}__attribute__((packed));

struct cfm_lck
{
	//void				*plck_timer; 
	TIMERID				plck_timer;
	struct cfm_lck_pdu  lck_pdu;

};


enum cfm_tst_pattern
{
	zero_without_crc32,
	zero_with_crc32,
	prbs_without_crc32,		
	prbs_with_crc32
};

/* CFM TEST 的报文结构 */
struct cfm_test_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;   
	uint32_t              seq;
	uint8_t 			  tlvs[1500];
}__attribute__((packed));

/* CFM TEST 的数据结构 */
struct cfm_test
{
	//void					*ptest_timer;
	TIMERID					ptest_timer;
	uint8_t 				tst_dmac[6];     /* test 报文 目的mac ，单播或一类组播*/
    uint32_t        		tst_len;         /* test 报文长度, 95 to 1500, default 95 */
	uint32_t        		tst_tx_count;    /* test 报文的发送数量， 1 to 10, default 5 */ 
	uint32_t        		tst_tx_interval; /* test 报文的发送周期， 0 to 10s,  default 1s */
	enum cfm_tst_pattern 	tst_pattern;
	uint16_t    		    remain_len;
	
	struct cfm_test_pdu     test_pdu;
};

int cfm_test_start(struct cfm_sess *psess);
int cfm_test_stop(struct cfm_sess *psess);
int cfm_rcv_test(struct cfm_sess *psess, struct pkt_buffer *pkt);

int cfm_rcv_lck(struct pkt_buffer *pkt);

int make_crc32_table(void);  
int cfm_send_lck_timer_out(void *para);

#endif

