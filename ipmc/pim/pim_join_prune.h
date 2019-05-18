/*定义pim jp 相关的结构体*/

#include "../ipmc_if.h"
#include "pim_neighbor.h"
//#include "pim_mroute.h"

#ifndef PIM_JOIN_PRUNE_H
#define PIM_JOIN_PRUNE_H

#define PIM_GRP_MAX 1024
#define PIM_SRC_MAX 1024
#define PIM_GRP_SRC_MASK_MAX 32

/* j/p source flags  (5bit + SWR) */
#define PIM_JP_R_BIT 0x1
#define PIM_JP_W_BIT 0x2
#define PIM_JP_S_BIT 0x4

/*sg /sg rpt */
#define PIM_JP_SG_TYPE     0x1
#define PIM_JP_SG_RPT_TYPE 0x2
/* J/P_Override_Interval, t_override*/
//#define PIM_JP_OVERRIDE ((PIM_LAN_DELAY_DEFAULT + PIM_OVERRIDE_DEFAULT) / 1000)
/* J/P message periodic timer value, t_periodic 60s, override_default 2.5s*/
#define PIM_JP_PERIODIC 60
#define PIM_JP_OVERRIDE 2.5

/* J/P Hold time */
#define PIM_JP_HOLDTIME (3.5 * PIM_JP_PERIODIC)

/* (*,G), (S,G) downstream state (NI, J, PP)*/
#define PIM_JP_DOWN_STATE_BASE   0x0
#define PIM_JP_DOWN_STATE_NOINFO (PIM_JP_DOWN_STATE_BASE  )           /* NoInfo */
#define PIM_JP_DOWN_STATE_J      (PIM_JP_DOWN_STATE_BASE + 1)        /* Join */
#define PIM_JP_DOWN_STATE_PP     (PIM_JP_DOWN_STATE_BASE + 2)        /* Prune-Pending */
/*(S,G,RPT) downstream state (NI, PP, P, PTmp, PPTmp)*/
#define PIM_JP_DOWN_STATE_P     (PIM_JP_DOWN_STATE_BASE + 3)        /* Prune */
#define PIM_JP_DOWN_STATE_PT    (PIM_JP_DOWN_STATE_BASE + 4)        /* PruneTmp */
#define PIM_JP_DOWN_STATE_PPT   (PIM_JP_DOWN_STATE_BASE + 5)        /* Prune-Pending-Tmp */

#define PIM_JP_DOWN_EVENT_BASE 0x10
/* (*, G) downstream state machine event */
#define PIM_JP_DOWN_EVENT_WC_J_RCV  (PIM_JP_DOWN_EVENT_BASE + 1)
#define PIM_JP_DOWN_EVENT_WC_P_RCV  (PIM_JP_DOWN_EVENT_BASE + 2)
#define PIM_JP_DOWN_EVENT_WC_PPT_EX (PIM_JP_DOWN_EVENT_BASE + 3)
#define PIM_JP_DOWN_EVENT_WC_ET_EX  (PIM_JP_DOWN_EVENT_BASE + 4)
/*(S, G) downstream state machine event */
#define PIM_JP_DOWN_EVENT_SG_J_RCV  (PIM_JP_DOWN_EVENT_BASE + 5)
#define PIM_JP_DOWN_EVENT_SG_P_RCV  (PIM_JP_DOWN_EVENT_BASE + 6)
#define PIM_JP_DOWN_EVENT_SG_PPT_EX (PIM_JP_DOWN_EVENT_BASE + 7)
#define PIM_JP_DOWN_EVENT_SG_ET_EX  (PIM_JP_DOWN_EVENT_BASE + 8)
/*(S, G, RPT) downstream state machine event */
#define PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV    (PIM_JP_DOWN_EVENT_BASE + 9)
#define PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV (PIM_JP_DOWN_EVENT_BASE + 10)
#define PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV (PIM_JP_DOWN_EVENT_BASE + 11)
#define PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX     (PIM_JP_DOWN_EVENT_BASE + 12)
#define PIM_JP_DOWN_EVENT_SG_RPT_ET_EX      (PIM_JP_DOWN_EVENT_BASE + 13)
#define PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG (PIM_JP_DOWN_EVENT_BASE + 14)
/*ifm del, downstream node del*/
#define PIM_JP_DOWN_EVENT_IFM_DEL  (PIM_JP_DOWN_EVENT_BASE + 15)

#define PIM_JP_UP_STATE_BASE 0x20
/*  (*, G) /(S, G)  upstream state machine , Sending (*,G)/(S,G) Join/Prune Messages*/
#define PIM_JP_UP_STATE_NOTJOINED (PIM_JP_UP_STATE_BASE + 1)    /* Not Joined */
#define PIM_JP_UP_STATE_JOINED    (PIM_JP_UP_STATE_BASE + 2)    /* Joined */
/* (S,G,RPT) upstream state machine,  Sending (S,G,rpt) Join/Prune Messages */
#define PIM_JP_UP_STATE_RPT_NJ   (PIM_JP_UP_STATE_BASE + 3)     /* RPT Not joined(G) */
#define PIM_JP_UP_STATE_RPT_P    (PIM_JP_UP_STATE_BASE + 4)     /* Pruned (S, G, RPT) */
#define PIM_JP_UP_STATE_RPT_NP   (PIM_JP_UP_STATE_BASE + 5)     /* NotPruned (S, G, RPT) */

#define PIM_JP_UP_EVENT_BASE 0x30
/* (*,G) upstream state machine event */
#define PIM_JP_UP_EVENT_WC_JOINDESIRED_T    (PIM_JP_UP_EVENT_BASE + 1) 
#define PIM_JP_UP_EVENT_WC_JOINDESIRED_F    (PIM_JP_UP_EVENT_BASE + 2)
#define PIM_JP_UP_EVENT_SG_JOINDESIRED_T    (PIM_JP_UP_EVENT_BASE + 3) 
#define PIM_JP_UP_EVENT_SG_JOINDESIRED_F    (PIM_JP_UP_EVENT_BASE + 4)
#define PIM_JP_UP_EVENT_JOIN_TIMER_EX       (PIM_JP_UP_EVENT_BASE + 5)
#define PIM_JP_UP_EVENT_SEE_WC_JOIN      	(PIM_JP_UP_EVENT_BASE + 6)
#define PIM_JP_UP_EVENT_SEE_WC_PRUNE     	(PIM_JP_UP_EVENT_BASE + 7)
#define PIM_JP_UP_EVENT_SEE_SGRPT_JOIN  	(PIM_JP_UP_EVENT_BASE + 8)
#define PIM_JP_UP_EVENT_SEE_SGRPT_PRUNE  	(PIM_JP_UP_EVENT_BASE + 9)
#define PIM_JP_UP_EVENT_SEE_SG_JOIN     	(PIM_JP_UP_EVENT_BASE + 10)
#define PIM_JP_UP_EVENT_SEE_SG_PRUNE     	(PIM_JP_UP_EVENT_BASE + 11)
#define PIM_JP_UP_EVENT_RPF_CH_AST       	(PIM_JP_UP_EVENT_BASE + 12)
#define PIM_JP_UP_EVENT_RPF_CH_NHP       	(PIM_JP_UP_EVENT_BASE + 13)
#define PIM_JP_UP_EVENT_RPF_CH_GENID     	(PIM_JP_UP_EVENT_BASE + 14)
#define PIM_JP_UP_EVENT_RPF_CH_TO_WCRPF         (PIM_JP_UP_EVENT_BASE + 15)
#define PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T    (PIM_JP_UP_EVENT_BASE + 16) 
#define PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F    (PIM_JP_UP_EVENT_BASE + 17)
#define PIM_JP_UP_EVENT_SGRPT_JOINDESIRED_F     (PIM_JP_UP_EVENT_BASE + 18)
#define PIM_JP_UP_EVENT_SGRPT_OLIST_NOTNULL     (PIM_JP_UP_EVENT_BASE + 19)
#define PIM_JP_UP_EVENT_SGRPT_OTIMER_EX         (PIM_JP_UP_EVENT_BASE + 20)

/* upstream (*,G)/(S,G)/(S,G,RPT)  j/p message */
#define PIM_JP_UP_SEND_WC_JOIN 0x01
#define PIM_JP_UP_SEND_WC_PRUNE 0x02
#define PIM_JP_UP_SEND_SG_JOIN 0x03
#define PIM_JP_UP_SEND_SG_PRUNE 0x04
#define PIM_JP_UP_SEND_SG_RPT_JOIN 0x05
#define PIM_JP_UP_SEND_SG_RPT_PRUNE 0x06

/*取消定时器，重置定时器*/
#define PIM_JP_RESET_TIMER 1
#define PIM_JP_CANCEL_TIMER 0
#define PIM_CREATE 1
#define PIM_LOOKUP 0

/*组播组G的RP共享树路径上设备维护的状态机*/
/*每一个接口都维护下游(*,G)*/
struct pim_down_jp_grp_node_s
{
	uint32_t grp_num;
	struct hash_table grp_table;
};

struct pim_down_jp_grp_node
{
	uint16_t state;					/* (NI), (J), (PP) */
	uint16_t holdtime;				/* holdtime of received msg*/
	uint32_t grp_addr;				/* grp address */
	#if 0
	struct thread *ppt_timer;		/* Prune-Pending Timer (PPT) */
	struct thread *et_timer;		/* Join/Prune Expiry Timer (ET) */
	#endif
	TIMERID ppt_timer;
	TIMERID et_timer;
	struct ipmc_if *pif;			/* point to ipmc_if */
};

/*每一个设备都维护一个上游(*,G)*/
struct pim_up_jp_grp_node
{
	uint32_t state;					/* send join/prune msg*/
	uint32_t grp_addr;				/* (*,G)*/
	uint32_t used_rp;				/* Last RP Used */
	uint32_t used_rpf;				/* Last RPF Neighbor towards RP that was used */
	uint32_t ifindex;				/* Upstream ifindex*/
	TIMERID jt_timer;
	/*struct thread *jt_timer;	*/	/* Upstream Join/Prune Timer (JT) */
	struct pim_neighbor_node *pnbr; /* point to pnbr */
	struct pim_mrt_wc *to_wc;		/* point to wc */
};

/*组播源S和组播组G的source-specific Tree 或者shortest-path Tree路径上路由器维护的状态机*/
/*每一个接口都维护下游(S, G)*/
struct pim_down_jp_sg_grp_node_s
{
	uint32_t grp_num;
	uint32_t sg_num;
	uint32_t sg_rpt_num;
	struct hash_table sg_grp_table;
};

struct pim_down_jp_sg_nodes
{
	uint32_t sg_num;
	uint32_t sg_rpt_num;
	uint32_t grp_addr;
	uint16_t holdtime;				/*每一个组都有一个holdtime*/
	struct hash_table sg_table;		/*sg node*/
	//struct hash_table sg_rpt_table; /*sg rpt node*/
};

struct pim_down_jp_sg_node
{
	uint32_t state;					/* (NI), (J), (PP) /(NI), (P), (PP), (PTmp), (PPTmp) */
	uint32_t type;					/* (S,G) /(S,G,RPT) */
	uint32_t grp_addr;				/* grp address */
	uint32_t src_addr;				/* src address */
	#if 0
	struct thread *ppt_timer;		/* Prune-Pending Timer (PPT) */
	struct thread *et_timer;		/* Join/Prune Expiry Timer (ET) */
	#endif
	TIMERID ppt_timer;
	TIMERID et_timer;
	struct ipmc_if *pif;			/* point to ipmc_if */
};

/*每一个设备都维护一个上游(S,G)/(S,G,RPT) */
struct pim_up_jp_sg_node
{
	uint32_t state;					/* NotJoined(S,G), Joined(S,G) / RPTNotJoined(G), NotPruned(S,G,rpt), Pruned(S,G,rpt)*/
	uint32_t type;					/* sg/sg rpt */
	uint32_t grp_addr;				/* grp address */
	uint32_t src_addr;				/* src address */
	uint32_t used_rpf;				/* Last RPF Neighbor towards S that was used */
	uint32_t used_rp;				/* Last RP Used */
	uint32_t ifindex;				/* upstream ifindex */
	uint32_t sptbit;				/* indicates (S,G) state is active */
	#if 0
	struct thread *jt_timer;		/* Upstream(S,G) Join/Prune Timer (JT) */
	//struct thread *kat_timer;		/* (S,G) Keepalive Timer (KAT) */
	struct thread *ot_timer;		/* Prune-Pending Timer (PPT) */
	#endif
	TIMERID jt_timer;
	TIMERID ot_timer;
	struct pim_mrt_sg *to_sg;		/* point to sg_mrt */
};

#if 0
/* 与pim_down_jp_sg_node合并了 */
/* 组播组G的RP共享树维护组播源S的状态机 */
/* 每一个接口都维护下游(S,G,RPT)状态 */
struct pim_down_jp_sg_rpt_node
{
	uint32_t state;					/* (NI), (P), (PP), (PTmp), (PPTmp) */
	uint32_t grp_addr;				/* grp address */
	uint32_t src_addr;				/* src address */
	struct thread *ppt_timer;		/* Prune-Pending Timer (PPT) */
	struct thread *et_timer;		/* Join/Prune Expiry Timer (ET) */
	struct ipmc_if *pif;			/* point to ipmc_if */
};

/*每一个设备都维护上游(S,G,RPT)状态*/
struct pim_up_jp_sg_rpt_node
{
	uint32_t state;					/* RPTNotJoined(G), NotPruned(S,G,rpt), Pruned(S,G,rpt) */
	struct thread *ot_timer;		/* Prune-Pending Timer (PPT) */
};
#endif
void pim_jp_down_grp_table_init(struct hash_table *grp_table, unsigned int size);
void pim_jp_down_grp_src_table_init(struct hash_table *grp_table, unsigned int size);
void pim_jp_up_grp_src_table_init(struct hash_table *table, unsigned int size);
struct pim_down_jp_grp_node* pim_jp_down_grp_node_add(struct hash_table *grp_table, uint32_t grp_addr);
struct pim_down_jp_grp_node* pim_jp_down_grp_node_lookup(struct hash_table *grp_table, uint32_t grp_addr);
struct pim_down_jp_grp_node* pim_jp_down_grp_node_get(struct ipmc_if *pif, uint32_t grp_addr);
struct pim_down_jp_sg_nodes* pim_jp_down_sg_grp_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint16_t holdtime);
struct pim_down_jp_sg_nodes * pim_jp_down_sg_grp_node_lookup(struct hash_table *grp_table, uint32_t grp_addr);
uint32_t pim_jp_down_sg_grp_node_del(struct hash_table *sg_grp_table, uint32_t grp_addr);
struct pim_down_jp_sg_node * pim_jp_down_sg_grpsg_node_add(struct ipmc_if *pif, struct hash_table *sg_table, uint32_t grp_addr, uint32_t src_addr);
struct pim_down_jp_sg_node * pim_jp_down_sg_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime);
struct pim_down_jp_sg_node * pim_jp_down_sg_node_lookup(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr);
struct pim_down_jp_sg_node * pim_jp_down_sg_node_get(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime);
struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_lookup(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr);
struct pim_down_jp_sg_node * pim_jp_down_sg_grpsgrpt_node_add(struct ipmc_if *pif, struct hash_table *sg_table, uint32_t grp_addr, uint32_t src_addr);
struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime);
struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_get(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime);
uint32_t pim_jp_down_sgrpt_node_del(struct pim_down_jp_sg_node * sg_rpt);
uint32_t pim_jp_down_sg_node_del(struct pim_down_jp_sg_node * sg);
sint32 pim_jp_down_join_process(uint8_t **pim_para, uint16_t join_num, uint32_t grp_addr, struct ipmc_if *pif, uint32_t pim_para_len, uint16_t holdtime);
sint32 pim_jp_down_prune_process(uint8_t **pim_para, uint16_t prune_num, uint32_t grp_addr, struct ipmc_if *pif, uint32_t pim_para_len, uint16_t holdtime);
sint32 pim_jp_down_end_of_message(uint32_t grp_addr, struct ipmc_if *pif);
sint32 pim_jp_downstream_recv(struct ipmc_if *pif, struct pim_neighbor *pim_nbr, uint8_t *pim_para, uint32_t pim_para_len);
sint32 pim_jp_upstream_recv(struct ipmc_if *pif, uint32_t upstream , uint8_t *pim_para, uint32_t pim_para_len);
sint32 pim_jp_recv(struct ipmc_if *pif, uint32_t sip, uint8_t *pim_para, uint32_t pim_para_len);
uint32_t pim_jp_up_wc_jp_send(uint32_t ifindex, uint32_t upstream, uint32_t grp_addr, uint32_t rp_addr, uint8_t type);
void pim_jp_up_send_wc_node_del(struct pim_neighbor_node *pnbr_node, uint32_t grp_addr);
struct pim_up_jp_grp_node *pim_jp_up_send_wc_node_get(struct pim_neighbor_node *pnbr_node, struct pim_mrt_wc *wc_mrt, uint8_t create_flag);
struct pim_up_jp_grp_node * pim_jp_up_send_wc_jp_upstream(struct pim_mrt_wc *wc_mrt, uint32_t create_flag);
uint32_t pim_jp_up_send_wc_jp_node_del(struct pim_mrt_wc *wc_mrt);
uint32_t pim_jp_up_sg_jp_send(uint32_t ifindex, uint32_t upstream, uint32_t grp_addr, uint32_t src_addr, uint8_t type);
struct pim_up_jp_sg_node *pim_jp_up_send_sg_node_get(struct pim_neighbor_node *pnbr_node, struct pim_mrt_sg *sg_mrt, uint32_t type, uint8_t create_flag);
struct pim_up_jp_sg_node* pim_jp_up_send_sg_jp_upstream(struct pim_mrt_sg *sg_mrt, uint32_t type, uint32_t create_flag);



uint32_t pim_jp_down_prune_delay(struct pim_neighbor *pnbr);
uint16_t pim_jp_down_sg_grp_node_holdtime_get(struct ipmc_if *pif, uint32_t grp_addr);
void pim_jp_down_wc_sgrpt_change(struct pim_down_jp_grp_node *grp_node, uint32_t event);
void pim_jp_down_wc_state_machine(struct pim_down_jp_grp_node *grp_node, uint32_t event);
void pim_jp_down_sg_state_machine(struct pim_down_jp_sg_node *sg_node, uint32_t event);
void pim_jp_down_sg_rpt_state_machine(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event);



#endif
