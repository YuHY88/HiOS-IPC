#ifndef HIOS_LACP_H
#define HIOS_LACP_H
#include "../l2_if.h"

#define LACP_FALSE 0
#define LACP_TRUE  1
#define LACP_MAC  {0x01, 0x80, 0xC2, 0x00, 0x00, 0x02}	/*LACP�鲥��ַ*/
#define LACP_LOG_DBG(fmt, ...) \
    do { \
        zlog_debug ( L2_DBG_LACP, fmt, ##__VA_ARGS__ ); \
    } while(0)

/*******************Э�����***********************/
#define LACP_TYPE                          		0x8809		/*Э������*/
#define LACP_SUBTYPE                      		0x01		/*Э�������� */
#define LACP_VERSION                        	0x01		/*LACPЭ��İ汾��*/
#define LACP_ACTOR_INFO_TLV          			0x01		/*Actor TLV����*/
#define LACP_ACTOR_INFO_LENGTH       			0x14		/*Actor����ϢTLV���� */
#define LACP_PARTNER_INFO_TLV        			0x02		/*Partner TLV���� */
#define LACP_PARTNER_INFO_LENGTH     			0x14		/*Partner����ϢTLV���� */
#define LACP_COLLECTOR_INFO_TLV      			0x03		/*�˿�Collector TLV����*/
#define LACP_COLLECTOR_INFO_LENGTH   			0x10		/*�˿�Collector��ϢTLV���� */
#define LACP_COLLECTOR_MAX_DELAY            	0x00		/*�˿�Collector���ʱ��*/
#define LACP_TERMINATOR_TLV                 	0x00		/*�˿�Terminator TLV����*/
#define LACP_TERMINATOR_LENGTH              	0x00		/*�˿�Terminator ����*/

#define LACP_GET_ACTIVITY(p)          		((p>>0) & 1)	/*��ȡ����/������ʽ*/
#define LACP_GET_TIMEOUT(p)           		((p>>1) & 1)	/*��ȡ��/�̳�ʱ*/
#define LACP_GET_AGGREGATION(p)          	((p>>2) & 1)	/*��ȡ��·�Ƿ�ɾۺ�*/
#define LACP_GET_SYNCHRONIZATION(p)      	((p>>3) & 1)	/*��ȡ�˿ھۺ��Ƿ����*/
#define LACP_GET_COLLECTING(p)           	((p>>4) & 1)	/*��ȡ��ǰ��·�հ��Ƿ�enable*/
#define LACP_GET_DISTRIBUTING(p)         	((p>>5) & 1)	/*��ȡ��ǰ��·�����Ƿ�enable*/
#define LACP_GET_DEFAULTED(p)             	((p>>6) & 1)	/*��ȡActorʹ�õ�Partner��Ϣ���Թ���Ա����/���յ�LACPDU*/
#define LACP_GET_EXPIRED(p)               	((p>>7) & 1)	/*��ȡActor RX״̬�����ڳ�ʱ/δ��ʱ״̬*/

#define LACP_ACTIVITY_PASSIVE             0				/*������ʽ*/
#define LACP_ACTIVITY_ACTIVE              1				/*������ʽ*/

#define LACP_TIMEOUT_LONG                 	0				/*����ʱ*/
#define LACP_TIMEOUT_SHORT                	1				/*�̳�ʱ*/
#define LACP_AGGREGATION_INDIVIDUAL      	0				/*������·�����ɾۺ�*/
#define LACP_AGGREGATION_AGGREGATABLE    	1				/*��·�ɾۺ�*/
#define LACP_SYNCHRONIZATION_OUT_OF_SYNC 	0				/*�˿ھۺ�δ���*/
#define LACP_SYNCHRONIZATION_IN_SYNC     	1				/*�˿ھۺ����*/
#define LACP_COLLECTING_DISABLED         	0				/*��ǰ��·�հ�disable*/
#define LACP_COLLECTING_ENABLED          	1				/*��ǰ��·�հ�enable*/
#define LACP_DISTRIBUTING_DISABLED       	0				/*��ǰ��·����disable*/
#define LACP_DISTRIBUTING_ENABLED        	1				/*��ǰ��·����enable*/
#define LACP_NOT_DEFAULTED                	0				/*Actorʹ�õ�Partner��Ϣ���Խ��յ�LACPDU*/
#define LACP_DEFAULTED                    	1				/*Actorʹ�õ�Partner��Ϣ���Թ���Ա���õ�Ĭ��ֵ*/
#define LACP_NOT_EXPIRED                  	0				/*Actor RX״̬�������ڳ�ʱ״̬*/
#define LACP_EXPIRED                      	1				/*Actor RX״̬�����ڳ�ʱ״̬*/

#define LACP_SET_ACTIVITY(p)          		(p |= (1<<0))	/*����Ϊ������ʽ*/
#define LACP_SET_TIMEOUT(p)           		(p |= (1<<1))	/*����Ϊ�̳�ʱ*/
#define LACP_SET_AGGREGATION(p)       		(p |= (1<<2))	/*����Ϊ��·�ǿɾۺϵ�*/
#define LACP_SET_SYNCHRONIZATION(p)   	(p |= (1<<3))	/*����Ϊ��·�ۺ����*/
#define LACP_SET_COLLECTING(p)         	(p |= (1<<4))	/*����Ϊ��ǰ��·�հ�enable*/
#define LACP_SET_DISTRIBUTING(p)          (p |= (1<<5))	/*����Ϊ��ǰ��·����enable*/
#define LACP_SET_DEFAULTED(p)             	(p |= (1<<6))	/*����ΪActorʹ�õ�Partner��Ϣ���Թ���Ա���õ�Ĭ��ֵ*/
#define LACP_SET_EXPIRED(p)               	(p |= (1<<7))	/*����ΪActor RX״̬�����ڳ�ʱ״̬*/

#define LACP_CLR_ACTIVITY(p)          		(p &= ~(1<<0))	/*����Ϊ������ʽ*/
#define LACP_CLR_TIMEOUT(p)           		(p &= ~(1<<1))	/*����Ϊ����ʱ*/
#define LACP_CLR_AGGREGATION(p)           	(p &= ~(1<<2))	/*����Ϊ��·Ϊ������·�����ɾۺ�*/
#define LACP_CLR_SYNCHRONIZATION(p)      	(p &= ~(1<<3))	/*����Ϊ��·�ۺ�δ���*/
#define LACP_CLR_COLLECTING(p)            	(p &= ~(1<<4))	/*����Ϊ��ǰ��·�հ�disable*/
#define LACP_CLR_DISTRIBUTING(p)         	(p &= ~(1<<5))	/*����Ϊ��ǰ��·����disable*/
#define LACP_CLR_DEFAULTED(p)             	(p &= ~(1<<6))	/*����ΪActorʹ�õ�Partner��Ϣ���Խ��յ�LACPDU*/
#define LACP_CLR_EXPIRED(p)               	(p &= ~(1<<7))	/*����ΪActor RX״̬�������ڳ�ʱ״̬*/
	
#define  LACP_FAST_PERIODIC_TIME   		1				/*�췢�ͼ��*/
#define  LACP_SHORT_TIMEOUT_TIME   		3				/*�̳�ʱʱ��*/

/*��ӡ��������*/
enum LACP_DUMP_PKT
{
	LACP_DUMP_RECEIVE = 0,
	LACP_DUMP_SEND
};

/* lacp bpdu�ṹ��*/
struct lacp_pkt
{
    uint8_t subtype;               							/*LACPЭ��������Ϊ0x01*/
    uint8_t version;		  						        /*LACPЭ��İ汾��Ϊ0x01*/

    /* Actor information */
	uint8_t actor_tlv_type;	    						/*ֵΪ0x01*/
	uint8_t actor_infor_len; 								/*Actor����ϢTLV���ȣ��̶�Ϊ0x14*/
    uint16_t  actor_spri;									/*Actor����Ա���õ�ϵͳ�Ż���*/
    uint8_t   actor_sys[MAC_LEN];							/*Actor��ϵͳMAC��ַ*/
    uint16_t  actor_key;									/*Actor������key*/
    uint16_t  actor_ppri;									/*Actor�Ķ˿����ȼ�*/
    uint16_t  actor_port;									/*Actor�Ķ˿ں�*/
    uint8_t   actor_state;									/*Actor�Ķ˿�״̬*/
	uint8_t res_3_1[3];										/*����*/
	
    /* Partner information */
	uint8_t partner_tlv_type;     							/*ֵΪ0x02*/
	uint8_t partner_infor_len;								/*Partner����ϢTLV���ȣ��̶�Ϊ0x14*/
    uint16_t  partner_spri;								/*Partner����Ա���õ�ϵͳ�Ż���*/
    uint8_t   partner_sys[MAC_LEN];						/*Partner��ϵͳMAC��ַ*/
    uint16_t  partner_key;									/*Partner������key*/
    uint16_t  partner_ppri;								/*Partner�Ķ˿����ȼ�*/
    uint16_t  partner_port;								/*Partner�Ķ˿ں�*/
	uint8_t   partner_state;								/*Actor��Ϊ��Partner״̬*/
	uint8_t res_3_2[3];										/*����*/
	
    /* Collector information */
	uint8_t col_tlv_type;	 								/*ֵΪ0x03*/
	uint8_t col_infor_len; 								/*�˿�Collector��ϢTLV����,�̶�Ϊ0x10*/
    uint16_t  col_max_delay;								/*���Ͷ˵�֡�ӾۺϽ����㵽MAC�ͻ��˵ķ����ӳ�*/
	uint8_t res_12[12];										/*����*/

	/* Terminator information */
	uint8_t ter_tlv_type;	      							/*ֵΪ0x00*/
	uint8_t ter_len;	          							/*ʹ�ù̶�ֵ0x00*/
	uint8_t res_50[50];	          							/*����*/
};

uint32_t	lacp_recv_dbg;									/*lacp���ձ��Ŀ���*/
uint32_t	lacp_send_dbg;									/*lacp���ͱ��Ŀ���*/

void lacp_pkt_debug_init(void);					/*lacp���ĵ��Կ��س�ʼ��*/
void lacp_pkt_rx_register(void);					/*lacp�հ�ע��*/
//int lacp_timer_start(struct thread *thread);		/*lacp��ʱ������*/	
int lacp_timer_start(void *para);
int lacp_tx (struct trunk *trunk_entry, uint32_t ifindex);	/*lacpЭ�鱨�ķ�װ*/
void lacp_fsm(struct trunk *trunk_entry, uint32_t ifindex, enum LACP_EVENT event);	/*lacp״̬��*/
void lacp_pkt_tx(struct trunk *trunk_entry, uint32_t ifindex);/*lacp���ķ���*/
int lacp_pkt_rx (struct l2if *l2_if, struct pkt_buffer *pkt); /*lacp���Ľ���*/
void lacp_port_delect_set(struct trunk *trunk_entry, uint32_t ifindex);
void lacp_port_linkdown_timeout_set(struct trunk *trunk_entry, uint32_t ifindex);
int lacp_backup_master_port_select(struct trunk *trunk_entry, struct trunk_port *tport);
int lacp_backup_failback_timer(void *para);

#endif
