/*
*   管理 bfd 会话
*/
#ifndef HIOS_BFD_SESSION_H
#define HIOS_BFD_SESSION_H

#include <lib/types.h>
#include <lib/hash1.h>
#include "lib/mpls_common.h"
#include "bfd.h"

#define BFD_PKT_TMP_MAX_LEN 255


/* bfd 会话的基本数据结构 */
struct bfd_info
{
	 enum BFD_TYPE    type;        /* bfd 类型 */
	 enum BFD_STATUS state; 		/* 本端当前bfd 会话状态 */
	 enum BFD_DIAG   diag;			/*诊断信息*/
	 uint32_t tdm_pw_label;
	 uint16_t sess_id;				/*本地sess_id 号*/
	 uint32_t index;	           /* 检测对象的索引，for 接口是 ifindex，for pw 是 pwindex，for lsp 是 lspindex */
	 uint32_t local_id;            /* local discriminator */
	 uint32_t remote_id;		   /* remote discriminator*/	 
	 uint16_t remote_multiplier;   /* 检测次数*/
	 uint32_t recv_interval;	   /* 报文接收间隔 */
	 uint32_t send_interval;	   /* 报文发送间隔 */
	 uint32_t src_ip;
	 uint32_t dst_ip;
	 uint8_t  without_ip;          /*bfd for pw 报文封装 0:with IP/UDP  header; 1:without IP/UDP header*/
	 uint8_t  priority;			   /*bfd 报文优先级*/
	 uint8_t  offset;				/* 从IP头到bfd payload的偏移量，等于IP头加UDP头的长度 */
     uint8_t  pkt_len;
     uint8_t  pkt_template[BFD_PKT_TMP_MAX_LEN];
}__attribute__((packed));


/* bfd session 数据结构 */
struct bfd_sess
{
	enum BFD_TYPE    type;        /* bfd 类型 */
	enum BFD_STATUS  status;      /* 工作状态 */
	enum BFD_DIAG    diag;		   /*会话最后一个从up变成其他状态的原因*/
	enum BFD_EVENT   bfd_event;	/*记录收到会话事件*/
	enum BFD_EVENT   poll_event;	/*记录发送poll时事件*/
	uint8_t  without_ip;		/*bfd for pw 报文封装 0:with IP/UDP  header; 1:without IP/UDP header*/
	uint8_t	 ldp_flag;			/*bfd for ldp 联动会话标识*/
	uint32_t tdm_pw_label;
	uint16_t session_id_cfg;
	uint16_t session_id;
	uint32_t remote_id_cfg; 	  /* 配置的 remote discriminator，如果为 0 则动态学习 */
	uint32_t remote_id; 		  /* remote discriminator */
	uint32_t local_id_cfg;		  /* 配置的 local discriminator，如果为 0 则动态分配 */	
	uint32_t local_id;			  /* local discriminator */	
	uint32_t index; 			  /* 检测对象的索引，for 接口是 ifindex，for pw 是 pwindex，for lsp 是 lspindex */
	uint32_t dstip; 			  /* 对端的 IP 地址 */
	uint32_t srcip; 			  /* 源端的 IP 地址 */
	uint32_t remote_send_interval;
	uint32_t remote_recv_interval;
	uint16_t remote_multiplier;
	uint16_t remote_state;
	uint32_t cc_interval_cfg;	  /* 配置的本地 cc 间隔 */
	uint32_t cc_interval_send; 	  /* 协商结果: 报文发送间隔 */
	uint32_t cc_interval_recv; 	  /* 协商结果: 报文接收间隔 */
	uint16_t cc_multiplier_cfg;	  /* 配置的本地检测次数*/	
	uint16_t cc_multiplier; 	  /* 协商结果: 检测次数*/
	uint8_t  priority;			  /*报文发出的优先级*/
	uint8_t  poll;
	uint8_t  final;
	uint8_t  retry_num;           /* 重传次数 */
	uint8_t  retry_cc;			/*cc计数，用于判断超时*/
	uint16_t wtr;				   /* wtr 时间 */
	uint32_t init_interval;	    /* 协商阶段的报文发送和接收间隔，固定 1s */
	uchar 	 pwclass_name[NAME_STRING_LEN];
	struct bfd_template *ptem;	/*bfd 模板*/
	TIMERID retry_timer;	/*协商阶段报文重传定时器*/
	TIMERID cc_timer;	   /*检测状态*/
	TIMERID check_cc_timer;	 /*周期性发送 cc 报文定时器*/
	uint64_t recv_count;			/*统计会话接收报文数目*/
	uint64_t send_count;			/*统计会话发送报文数目*/
	uint64_t up_count;			    /*统计会话up次数*/
	uint64_t down_count;			/*统计会话down次数*/
}__attribute__((packed));

/*被用来分配存储bfd报文内部的local_id */
struct bfd_local_id 
{
	uint16_t sess_id;
	uint32_t local_id;
};

extern struct hash_table bfd_session_table;     /* bfd 会话 hash 表，用 local_id 作为 key */
extern struct hash_table bfd_local_id_table;
extern struct list *local_id_list;

extern struct hash_table bfd_template_table;
struct bfd_template   // bfd模板结构，存放不同模板
{
	uchar template_name[NAME_STRING_LEN]; /*模板名称*/
	uint8_t  auth_mode;					  /*认证方式*/
	uint16_t detect_mult;				  /*本端超时检测倍数*/
	uint32_t cc_tx_interval;			  /*cc发送周期*/
	uint32_t cc_rx_interval;
	uint32_t bfdindex;
};
/* 对 bfd 会话的操作 */
void bfd_session_table_init ( unsigned int size );
extern struct bfd_sess *bfd_session_create(uint16_t local_id);/* 创建一个 bfd 会话 */
extern int bfd_session_delete(uint16_t local_id); /* 删除一个 bfd 会话 */
extern struct bfd_sess *bfd_session_lookup(uint16_t local_id); /* 查找一个 bfd 会话 */
extern int bfd_session_ifname(uint16_t local_id, char *ifname); /* 查找一个 bfd 绑定的lsp/pw/... name */
extern void bfd_global_disable(void);
void bfd_session_disable(struct bfd_sess *psess);
int bfd_bind_for_lsp_pw(enum BFD_TYPE type, uint16_t local_id, uint32_t index, uint8_t ip_flag);
int bfd_unbind_for_lsp_pw(enum BFD_TYPE type, uint16_t local_id, uint32_t index);
int bfd_state_notify(enum BFD_STATUS state_now, struct bfd_sess *psess);
int bfd_local_id_create(struct bfd_sess *psess, struct list *list, uint32_t local_id);
struct bfd_sess *bfd_local_id_lookup(struct list *list, uint32_t local_id);
int bfd_local_id_delete(struct bfd_sess *psess, struct list *list);
int bfd_session_ifname(uint16_t sess_id, char *ifname);
int bfd_for_ldp_bind_unbind(enum BFD_SUBTYPE type, void *pdata);

void bfd_template_table_init(unsigned int size);
struct bfd_template *bfd_template_create(void);
struct bfd_template *bfd_template_lookup(uchar *pname);
int bfd_template_add(struct bfd_template *ptemplate);
int bfd_template_delete(uchar *pname);
int bfd_bind_for_lsp_pw_h3c(enum BFD_TYPE type, struct nni_info *nni);
int bfd_unbind_for_lsp_pw_h3c(uint16_t sess_id);
int bfd_update_sess_when_del_template(struct bfd_template *ptem);
int pw_class_change_bfd_template(struct pw_class *pclass);

#endif

