/*
*   管理 mpls-tp oam 会话
*/
#ifndef HIOS_MPLSTP_OAM_H
#define HIOS_MPLSTP_OAM_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/hptimer.h>
#include <lib/oam_common.h>
#include "mpls/mpls_if.h"


#define MPLSTP_OAM_NUM  65535


/* mpls oam 全局数据结构 */
struct oam_global
{
	uint32_t        pkt_recv;	    /* 接收报文统计 */
	uint32_t        pkt_send;	    /* 发送报文统计 */
	uint32_t        pkt_err;	    /* 错误报文统计 */	
	uint16_t        sess_total;     /* session 总数*/
	uint16_t        sess_enable;	/* enable 的 session 数 */
	uint16_t        sess_up;        /* up 的 session 数 */
	uint16_t        sess_down;      /* down 的 session 数 */
	uint16_t        global_chtype;  /* oam global chtype */
	uint8_t         g_chtype_enable;/* oam global chtype enable */
};


/* mpls oam 会话的基本数据结构 */
struct oam_info
{
     uchar megname[NAME_STRING_LEN]; /* 所属meg的meg-name */   
	 enum OAM_TYPE      type;        /* 类型 */
	 enum OAM_STATUS    state; 	     /* 会话状态 */	 
	 enum MEP_DIRECT    direct;	     /* 只对 PW 有效 */
	 enum MP_TYPE       mp_type;     /* mep or mip */
	 uint32_t           index;	     /* 检测对象的索引，for 接口是 ifindex，for pw 是 pwindex，for lsp 是 lspindex */
	 uint16_t           session_id;  /* 会话session_id */
	 uint16_t			mp_id;		 /* mep-id or mip-id*/
	 uint16_t		    local_mep;   /* local mep 索引:1-8191 */
	 uint16_t		    remote_mep;  /* remote mep 索引:1-8191 */	 
	 uint32_t		    cc_interval; /* cc 报文发送间隔, 单位 ms, 3.3ms - 10s */
	 uint8_t		    cc_enable;   /* cc 使能 */
	 uint8_t            lm_enable;   /* lm 使能 */	 
	 uint8_t            dm_enable;   /* dm 使能 */
	 uint8_t            dm_exp;      /* dm报文优先级*/
	 uint16_t           dm_tlv_len;      /* tlv size of dmm */
     uint16_t           lm_interval; /* lm 测试间隔 */
	 uint16_t           lm_frequency;/* lm 测试次数 */	 
     uint16_t           dm_interval; /* dm 测试间隔 */
	 uint16_t           dm_frequency;/* dm 测试次数 */
	 uint8_t            priority;    /* 报文的优先级，默认是 5 */
	 uint8_t            level;       /* mep level，固定 7 */
	 uchar              mac[MAC_LEN];/* section oam 的对端 mac，默认是全 F */	 
	 uint16_t           vlan;        /* section oam 的 vlan, 默认是 4096 */
     uint16_t           channel_type;/* oam channel -type,默认0x8902 */
	 uint16_t			oam_hsid;
	 uint32_t			ccm_stat;
	 uint8_t			perfm_enable;
};


/* mpls-tp oam session 数据结构 */
struct oam_session
{
	struct oam_meg    *meg;         /* 指向所属meg*/
	struct oam_info    info;        /* 会话的基本信息 */
	struct oam_alarm_t alarm;       /* oam 告警*/
	struct sla_measure sla;         /* lm/dm 性能测试的结果，只保留最新的测试结果 */
    uint8_t  alarm_rpt;             /* 是否向告警模块上报告警,1为使能上报*/
	uint8_t  lb_enable;             /* 1: lb 使能 */	
	uint8_t  lt_enable;             /* 1: lt 使能 */
    uint8_t  ais_enable;            /* ais使能标志位*/
	uint8_t  lck_enable;            /* lck使能标志位*/
	uint8_t  csf_enable;			/*csf使能标志位*/
	uint8_t  lb_exp;                /* lb报文优先级*/
	uint16_t		   fwd_rmep;	/* mip绑定的前向rmep */
    uint16_t		   rev_rmep;	/* mip绑定的反向rmep */
    struct static_lsp *fwd_lsp;	    /* mip绑定的前向transit lsp */
    struct static_lsp *rev_lsp;	    /* mip绑定的反向transit lsp */
    struct l2vc_entry *sw_pw;		/* mip绑定的sw-pw */
	TIMERID  lb_timer;	    /* lb应答定时器*/
	TIMERID  lt_timer;	    /* lt应答定时器*/
    TIMERID  ais_send;       /* ais报文发送定时器*/
    TIMERID  ais_rcv;        /* ais报文接收定时器*/
    struct oam_ais *pais;
	TIMERID  lck_send;		/* lck报文发送定时器*/
	TIMERID  lck_rcv;		/* lck报文接收定时器*/
	struct oam_lck *plck;
    struct list    *client_list;    /* 当前server layer 的clinet layer,存入client layer的mip*/
	struct oam_csf *pcsf;
	//struct thread  *csf_send;
	//struct thread  *csf_rcv;
	TIMERID  csf_send;
	TIMERID  csf_rcv;
	uint32_t type;                  /* 当前回话是基于lb/lt*/
	uint16_t ttl_curr;
	uint8_t  ttl;
	uint16_t packet_size;
	uint32_t node_id;
	uint16_t data_tlv_len;
	uint8_t data_tlv_flag;
	uint8_t req_tlv_flag;
    uint16_t down_cnt;              /* 会话 down 的次数 */
	uint32_t seqNum;
	void    *plb_timer;             /* lb/lt 等待应答的 timer thread */
	struct timeval sendSec;		    /* 发送时间戳 */
	void *pvty;
};

struct snmp_oam
{
	struct oam_info    info;
	uint16_t 	fwd_rmep;
    uint16_t 	rev_rmep;
	uint8_t 	ais_enable;
	uint8_t 	ais_level;
    uint16_t 	ais_interval;
	uint8_t 	alarm_rpt;
	uint8_t  	lck_enable;   
	uint8_t  	lck_level;
	uint16_t 	lck_interval;
	uint8_t     csf_enable;
	uint8_t     csf_level;
	uint16_t	csf_interval;
	uchar 		pw_name[NAME_STRING_LEN];
	uchar 		fwd_lsp_name[NAME_STRING_LEN];
	uchar 		rsv_lsp_name[NAME_STRING_LEN];
};

struct oam_meg
{
	uchar name[NAME_STRING_LEN];    /* meg 名称 */
	uint8_t level;
	struct list *session_list;
};


extern struct oam_global gmplsoam;              /* mpls oam 全局数据结构 */
extern struct hash_table mplsoam_session_table; /* oam 会话 hash 表，用 session_id 作为 key */
extern struct hash_table mplsoam_meg_table;     /* oam meg表项hash表，meg-name作为key*/

/* 对 mpls-tp oam 会话的操作 */
void mplsoam_session_table_init ( unsigned int size );
struct oam_session *mplsoam_session_create(uint16_t session_id);/* 创建一个会话 */
int mplsoam_session_add(struct oam_session *psess); /* 添加到 hash 表 */
int mplsoam_session_delete(uint16_t session_id);     /* 删除一个会话 */
struct oam_session *mplsoam_session_lookup(uint16_t session_id); /* 查找一个会话 */

void mplsoam_session_up(uint16_t session_id);    /* 会话 up */
void mplsoam_session_down(uint16_t session_id); /* 会话 down */

int mplsoam_session_enable(uint16_t session_id, uint32_t index, enum OAM_TYPE type); /* 会话 enable */
int mplsoam_session_disable(uint16_t session_id); /* 会话 disable */

int mplsoam_session_ais_enable(struct oam_session *psess, uint8_t level, uint16_t interval);
int mplsoam_session_ais_disable(struct oam_session *psess);

int mplsoam_session_lck_enable(struct oam_session *psess, uint8_t level, uint16_t interval);
int mplsoam_session_lck_disable(struct oam_session *psess);

int mpls_if_update_tpoam_csf(struct mpls_if *pif);
int mplsoam_session_csf_enable(struct oam_session *psess, uint8_t level, uint16_t interval);
int mplsoam_session_csf_disable(struct oam_session *psess);

int mplsoam_session_lm_enable(struct oam_session *psess);
int mplsoam_session_lm_disable(uint16_t session_id);
int mplsoam_session_dm_enable(struct oam_session *psess);
int mplsoam_session_dm_disable(uint16_t session_id);
int mplsoam_session_set_dm(uint16_t session_id, struct sla_measure *psla);/* 设置 dm 测试结果*/
int mplsoam_session_set_lm(uint16_t session_id, struct sla_measure *psla);/* 设置 lm 测试结果*/

void mplsoam_session_alarm_rpt_enable(struct oam_session *psess);
void mplsoam_session_alarm_rpt_disable(struct oam_session *psess);
void mplsoam_session_alarm_process(uint16_t session_id, uint16_t state, enum OAM_ALARM alarm_type);/* process alarm*/
void mplsoam_session_down_clear_alarm(struct oam_session *psess);

int mplsoam_session_config_complete(struct oam_session *psess); /* 检查配置是否完整 */

int mplsoam_session_ifname(uint16_t session_id, char *ifname);
int mpls_pw_bind_oam_mip(uint16_t session_id, struct l2vc_entry *pl2vc);
int mpls_pw_unbind_oam_mip(uint16_t session_id, struct l2vc_entry *pl2vc);
int mplsoam_session_unbind_service(uint16_t session_id, void *service);


struct oam_meg *mplsoam_meg_create(uchar *name);
int mplsoam_meg_add(struct oam_meg *meg);
int mplsoam_meg_delete(uchar *name);
struct oam_meg *mplsoam_meg_lookup(uchar *name) ;




extern int ifm_get_name_by_ifindex ( uint32_t ifindex, char *name );
extern uchar *pw_get_name(uint32_t pwindex);
#endif
