/**
 * @file      : cfm.h
 * @brief     : define of 802.1ag and Y.1731 md and ma
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:33:14
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */


#ifndef HIOS_CFM_H
#define HIOS_CFM_H


#include <lib/types.h>
#include <lib/hash1.h>

/*cfm debug info*/
//#define CFM_DBG_CC		    0X01
#define CFM_DBG_LB			0x01
#define CFM_DBG_LT			0x02
#define CFM_DBG_TEST		0x04
#define CFM_DBG_COMMN       0x08
#define CFM_DBG_ALL      	0x1F

/*snmp get information*/
#define CFM_SNMP_MD_GET             0
#define CFM_SNMP_MA_GET             1
#define CFM_SNMP_SESSION_GET        2
#define CFM_SNMP_SLA_GET            3
#define CFM_SNMP_SESSION_GET_NEW    4
#define CFM_SNMP_SLA_GET_NEW        5

struct cfm_ma_key
{
    u_int8_t  md_id;          /* cfm md id <1-8> */
    u_int16_t ma_id;          /* cfm ma id <1-512> */
};

struct cfm_session_key
{
    u_int16_t ma_id;          /* cfm ma id <1-512> */
    u_int16_t sess_id;        /* cfm session id <1-1024> */
};


#define CFM_MD_NUM_MAX   8
#define CFM_MA_NUM_MAX   1024
#define CFM_MEP_NUM_MAX  1024

#define CFM_UCAST_FRAME  1
#define CFM_MCAST_FRAME  2

/*enum CFM_INFO
{
	CFM_INFO_INVALID = 0,	
	CFM_INFO_MD,
	CFM_INFO_SESS,
	CFM_INFO_SESS_UP,
	CFM_INFO_SESS_DOWN,	
	CFM_INFO_MA,
	
	CFM_INFO_MAX = 5
};*/


/* MD 数据结构 */
struct cfm_md
{
	char      	 name[NAME_STRING_LEN];
	uint8_t      md_index; /* md 索引: 1 - 8 */
	uint8_t      level;
	uint16_t     elps_bind;
	struct list  ma_list;  /* 存放 ma 指针 */
};


/* MA 数据结构 */
struct cfm_ma
{
	char        name[NAME_STRING_LEN];	
	uint16_t    vlan;
	uint16_t    ma_index;    /* ma 索引: 1 - 512 */
	uint8_t     md_index;    /* 所属的 md */
	uint8_t     priority;    /* 报文的优先级 */
	uint8_t     mip_enable;
	uint8_t     y1731_en;
	uint16_t    elps_bind;
	uint8_t     rev[2];
	struct list sess_list;   /* 存放 mep session 指针 */
};

void 			cfm_init(void);



/* MD 数组的操作函数 */
void 			cfm_md_table_init(void);
struct cfm_md  *cfm_md_create(uint8_t md_index);      /* 创建一个 MD */
int 			cfm_md_delete(uint8_t md_index);      /* 删除一个 MD */
struct cfm_md  *cfm_md_lookup(uint8_t md_index);      /* 查找一个 MD */
struct cfm_md  *cfm_md_lookup_by_level(uint8_t level); /* 查找一个 MD */
int 			cfm_md_add_ma(struct cfm_md *pmd, struct cfm_ma *pma);   /* 将 ma 添加到 ma_list */
int 			cfm_md_delete_ma(struct cfm_md *pmd, struct cfm_ma *pma); /* 将 ma 从 ma_list 删除 */
void 			cfm_md_clear_ma(struct cfm_md *pmd);  /* 删除 md 下的所有 Ma */


/* MA hash 表的操作函数 */
void 			cfm_ma_table_init(unsigned int uiSize);
struct cfm_ma  *cfm_ma_create(uint16_t ma_index);     /* 创建一个 Ma */
int 			cfm_ma_add(struct cfm_ma *pma);       /* 添加 Ma 到 hash 表 */
int 			cfm_ma_delete(uint16_t ma_index);     /* 删除一个 Ma */
struct cfm_ma  *cfm_ma_lookup(uint16_t ma_index); /* 查找一个 Ma */
int 			cfm_ma_add_sess(struct cfm_ma *pma, struct cfm_sess *psess);
int 			cfm_ma_delete_sess(struct cfm_ma *pma, struct cfm_sess *psess);
void			cfm_ma_clear_sess(struct cfm_ma *pma);  /* 删除 ma 下的所有 session */


void 			cfm_pkt_rcv(struct pkt_buffer *pkt);
void 			cfm_snmp_manage(void *pdata, struct ipc_msghdr_n *pmsghdr);

int cfm_ipc_send_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int cfm_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index);
int cfm_send_hal(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
            enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


extern struct cfm_md 		*cfm_md_table[CFM_MD_NUM_MAX]; /* MD 数组 */
extern struct hash_table 	 cfm_ma_table;                 /* CFM ma hash 表，以 ma 索引为 hash key */

#endif
