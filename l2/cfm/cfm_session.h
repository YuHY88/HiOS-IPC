/**
 * @file      : cfm_session.h
 * @brief     : define of 802.1ag and Y.1731 mep session
 * @details   : 
 * @author    : huoqq
 * @date      : 2018��3��23�� 14:34:17
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */


#ifndef HIOS_CFM_SESSION_H
#define  HIOS_CFM_SESSION_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/oam_common.h>
#include "ftm/pkt_eth.h"

#include "lib/hptimer.h"

#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnStatTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockStatMsgDef.h>

struct cfm_ais_pdu
{
	struct vlan_ethhdr    ethhdr;
	struct cfm_pdu_header cfm_header;   
	uint8_t               end_tlv;
	uint8_t               rev[25];
}__attribute__((packed));

struct cfm_ais
{
    //uint8_t         interval; 
    //uint8_t         level; 
	//void				*pais_timer; 
	TIMERID				pais_timer;
	struct cfm_ais_pdu   ais_pdu;

};


/* mep session ���ݽṹ */
struct cfm_sess
{
	uint16_t        sess_id;   /* session ����: 1-1024 */
	enum MEP_DIRECT direct;
	uint8_t 		over_pw;
	enum OAM_STATUS state;
	struct oam_alarm_t alarm;       /* cfm �澯*/	
	uint16_t        local_mep;   /* local mep ����: 1-65535 */
	uint16_t        remote_mep;  /* remote mep ����:1-65535 */	
	uint32_t        cc_interval; /* cc ���ķ��ͼ�� */	
	uint8_t         cc_enable;	 /* 1: enable, 0:disable */	
	uint8_t         lb_enable; 	 /* 1: lb ʹ�� */	
	uint8_t         lt_enable; 	 /* 1: lt ʹ�� */
	uint8_t         md_index;    /* ������ md */
	uint16_t        ma_index;    /* ������ ma */
	uint16_t        vlan;        /* ma �� vlan */	
	uint32_t        ifindex;     /* ���ձ��ĵĽӿ� */
	uint32_t        ifindex_del;     /* ɾ���ӿ�ʱ���ݸýӿڣ����ڻָ� */
	uint8_t         level;	     /* md level */
	uint8_t		    ais_enable;  /* ais ʹ�� */
	uint8_t		    ais_rcv_interval;  /* ais ���ձ�־ */	
    uint8_t         ais_send_interval; 
    uint8_t         ais_send_level; //bits
	uint8_t		    tst_enable;  /* test ʹ�� */
	uint8_t		    tst_mode;    /* 0:in-service, 1:out-of-service */	
    uint32_t       	tst_next_trans_id;
    uint32_t       	tst_rxnum;
    uint32_t       	tst_errnum;		
    uint32_t       	tst_lastid;	
	uint8_t		    lck_rcv_interval;  /* lck ���ձ�־ */    
    uint8_t         lck_send_interval;	
    uint8_t         lck_send_level; //bits
	uint8_t         perfm_enable;   /* ���ܼ��ʹ�ܱ�־ */    
	uint8_t         lm_enable;   /* lm ʹ�� */	 
	uint8_t         dm_enable;   /* dm ʹ�� */
    uint16_t        lm_interval; /* lm ���Լ�� */
    uint16_t        dm_interval; /* dm ���Լ�� */
	uchar           lm_dmac[6];     /* lm dmac */
	uchar           dm_dmac[6];     /* dm dmac */	
	uint16_t        down_cnt;	 /* �Ự down �Ĵ��� */
	uint8_t         dmac_valid;   /* remote mep mac valid flag */	 
	uchar           dmac[6];     /* remote mep mac */
	uchar           smac[6];     /* local mep mac */
	uint16_t		elps_bind;
	uchar 			reconfig_flag;
	
	struct oam_alarm_t *palarm;	 /* оƬ�ϱ��� oam �澯 */
	struct sla_measure *psla; 	 /* lm/dm ���ܲ��ԵĽ����ֻ�������µĲ��Խ�� */	
#if 0
	void	*plb_timer; 		 /* lb/lt �ȴ�Ӧ��� timer thread */
	void	*pais_timer; 		 //ais receive timer
	void	*plck_timer; 		 //lck receive timer
#endif

	TIMERID	plb_timer; 		 /* lb/lt �ȴ�Ӧ��� timer thread */
	TIMERID	pais_timer; 		 //ais receive timer
	TIMERID	plck_timer; 		 //lck receive timer

	struct cfm_lb      *lb;
	struct cfm_lt      *lt;
	struct cfm_ais     *pais;
	struct cfm_lck     *plck;	
	struct cfm_test    *ptest;
};


/* mep session �Ĳ������� */
void 				 cfm_session_table_init(unsigned int uiSize);
struct cfm_sess 	*cfm_session_create(uint16_t sess_id);/* ����һ�� Mep session */
struct sla_measure  *cfm_session_create_sla(struct cfm_sess *psess);
int 				 cfm_session_add(struct cfm_sess *psess); /* session ��ӵ� hash �� */
int 				 cfm_session_delete(uint32_t key); /* ɾ��һ�� Mep session */
struct cfm_sess		*cfm_session_lookup(uint32_t key); /* ����һ�� Mep session */

int 	cfm_session_add_new(struct cfm_sess *psess);
int 	cfm_session_delete_new(uint16_t sess_id);

void 	cfm_send_session_to_hal(struct cfm_sess * psess,int add_flag);

int 	cfm_session_up(uint32_t key);
int 	cfm_session_down(uint32_t key);
int 	cfm_session_alarm(uint32_t key, uint16_t opcode, enum OAM_ALARM alarm_type);


int 	cfm_session_lm_enable(struct cfm_sess *psess);
int 	cfm_session_lm_disable(struct cfm_sess *psess);

int 	cfm_session_dm_enable(struct cfm_sess *psess);
int 	cfm_session_dm_disable(struct cfm_sess *psess);

int 	cfm_session_set_dm(uint32_t key, struct sla_measure *sla);
int 	cfm_session_set_lm(uint32_t key, struct sla_measure *sla);


int 	cfm_session_alarm_report(struct cfm_sess *psess, int opcode, unsigned int almcode);
int 	cfm_session_clear_alarm(struct cfm_sess	 *psess);

int 	cfm_ais_start(struct cfm_sess *psess);
int 	cfm_ais_stop(struct cfm_sess *psess);
int 	cfm_rcv_ais(struct pkt_buffer *pkt);

void  	config_finish_func(void * a);

struct cfm_sess		*cfm_session_lookup_new(uint16_t sess_id);

struct cfm_sess		*cfm_session_bind_elps(uint16_t sess_id); 
struct cfm_sess		*cfm_session_unbind_elps(uint16_t sess_id);

void 				 cfm_if_delete(uint32_t ifindex);
void 				 cfm_if_add(uint32_t ifindex);
void 				 cfm_session_reconfig(uint32_t ifindex);

extern struct hash_table cfm_sess_table_new;     /* CFM session hash ���� session id Ϊ hash key */
extern struct hash_table cfm_sess_table;     /* CFM session hash ���� local_mep Ϊ hash key */

int  cfm_session_perfm_enable(gpnSockMsg *pgnNsmMsgSp);
int  cfm_session_perfm_disable(gpnSockMsg *pgnNsmMsgSp);
int  cfm_session_perfm_get(gpnSockMsg *pgnNsmMsgSp);
#endif

