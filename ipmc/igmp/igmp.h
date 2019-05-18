#ifndef IGMP_H
#define IGMP_H

#include <lib/types.h>
#include <lib/pkt_buffer.h>
#include <lib/linklist.h>
#include <lib/hptimer.h>

#include "igmp_pkt.h"

extern int igmp_debug;
extern struct list *igmp_inst;


#define IGMP_INS_MIN 1
#define	IGMP_INS_MAX 128

#define	IGMP_QUERY_MIN_INTERVAL		1
#define	IGMP_QUERY_MAX_INTERVAL		18000
#define	IGMP_QUERY_DEFAULT_INTERVAL	60

#define IGMP_QUARY_MIN_KEEPALIVE 	 60
#define IGMP_QUARY_MAX_KEEPALIVE 	 300
#define IGMP_QUARY_DEFAULT_KEEPALIVE 125

#define IGMP_ROBUST_MIN_COUNT		2
#define IGMP_ROBUST_MAX_COUNT		5
#define	IGMP_ROBUST_DEFAULT_COUNT	2

#define IGMP_LAST_MEMBER_QUERY_MIN_INTERVAL		1
#define IGMP_LAST_MEMBER_QUERY_MAX_INTERVAL		5
#define IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL	1

#define IGMP_RESPONSE_MIN_TIME		1
#define IGMP_RESPONSE_MAX_TIME		25
#define IGMP_RESPONSE_DEFAULT_TIME	10

#define IGMP_QUERY_MAX_RESP_TIME_DSEC		100
#define IGMP_QUERY_MAX_SPEC_RESP_TIME_DSEC	10

#define IGMP_FLAG_DISABLE		0
#define IGMP_FLAG_ENABLE		1
#define IGMP_FLAG_PROXY_ENABLE	2
#define IGMP_FLAG_SNOOP_ENABLE	3

#define IGMP_FLAG_CONFIG_VERSION				(1<<0)
#define IGMP_FLAG_CONFIG_REQ_ALERT				(1<<1)
#define IGMP_FLAG_CONFIG_SND_ALERT				(1<<2)
#define IGMP_FLAG_CONFIG_QUERY					(1<<3)
#define IGMP_FLAG_CONFIG_ROBUST_CNT				(1<<4)
#define IGMP_FLAG_CONFIG_MAX_RESP				(1<<5)
#define IGMP_FLAG_CONFIG_OTHER_QUERIER_ALIVE	(1<<6)
#define IGMP_FLAG_CONFIG_LASTMEM_QUERY			(1<<7)
#define IGMP_FLAG_CONFIG_TEST(flag, para)		((flag) & (para))
#define IGMP_FLAG_CONFIG_SET(flag, para)		((flag) |= (para))
#define IGMP_FLAG_CONFIG_CLEAR(flag, para)		((flag) &= ~(para))

#define IGMP_SOURCE_MASK_FORWARDING        (1 << 0)
#define IGMP_SOURCE_MASK_DELETE            (1 << 1)
#define IGMP_SOURCE_MASK_SEND              (1 << 2)
#define IGMP_SOURCE_TEST_FORWARDING(flags) ((flags) & IGMP_SOURCE_MASK_FORWARDING)
#define IGMP_SOURCE_TEST_DELETE(flags)     ((flags) & IGMP_SOURCE_MASK_DELETE)
#define IGMP_SOURCE_TEST_SEND(flags)       ((flags) & IGMP_SOURCE_MASK_SEND)
#define IGMP_SOURCE_DO_FORWARDING(flags)   ((flags) |= IGMP_SOURCE_MASK_FORWARDING)
#define IGMP_SOURCE_DO_DELETE(flags)       ((flags) |= IGMP_SOURCE_MASK_DELETE)
#define IGMP_SOURCE_DO_SEND(flags)         ((flags) |= IGMP_SOURCE_MASK_SEND)
#define IGMP_SOURCE_DONT_FORWARDING(flags) ((flags) &= ~IGMP_SOURCE_MASK_FORWARDING)
#define IGMP_SOURCE_DONT_DELETE(flags)     ((flags) &= ~IGMP_SOURCE_MASK_DELETE)
#define IGMP_SOURCE_DONT_SEND(flags)       ((flags) &= ~IGMP_SOURCE_MASK_SEND)


struct igmp_join 
{
  	uint32_t grpip;
  	uint32_t sip;
};

struct igmp_source 
{
	uint32_t			sip;
	//struct thread   	*src_timer;
	TIMERID				src_timer;
	struct igmp_group	*src_grp;    	/* back pointer */
	time_t				src_create;
	uint32_t            src_flags;

	/*
	RFC 3376: 6.6.3.2. Building and Sending Group and Source Specific Queries
	*/
	int					 sqrt;  /*source query retransmit count*/
};

struct igmp_group 
{
	struct igmp *grp_igmp;  /* back pointer    igmp*/
	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and it
	represents the time for the *filter-mode* of the group to expire and
	switch to INCLUDE mode.
	*/
	//struct thread   *grp_timer;
	TIMERID			  grp_timer;
	
	/* Shared between group-specific and
	 group-and-source-specific retransmissions */
	//struct thread    *grp_qrt_timer;
	TIMERID			  grp_qrt_timer;

	/* Counter exclusive for group-specific retransmissions
	 (not used by group-and-source-specific retransmissions,
	 since sources have their counters) */
	int               grp_sqrt_count;

	uint32_t          grp_addr;
	int               grp_filtermode;  		/*group filter mode: 0=INCLUDE, 1=EXCLUDE */
	struct list      *grp_src_list;        /* list of struct igmp_source */
	time_t            grp_creat;        	
	int64_t           last_v1_rpt_dsec;
	int64_t           last_v2_rpt_dsec;
};

/*IGMP packet statistics*/
struct igmp_pkt_count
{
	uint32_t qry_v1_rsv_pkts;
	uint32_t qry_v1_snd_pkts;
	uint32_t qry_v2_rsv_pkts;
	uint32_t qry_v2_snd_pkts;
	uint32_t qry_v3_rsv_pkts;
	uint32_t qry_v3_snd_pkts;
	uint32_t gqry_v3_rsv_pkts;
	uint32_t gqry_v3_snd_pkts;
	uint32_t gsqry_v3_rsv_pkts;
	uint32_t gsqry_v3_snd_pkts;
	uint32_t rpt_v1_pkts;
	uint32_t rpt_v2_pkts;
	uint32_t rpt_v3_pkts;
	uint32_t rpt_isin_pkts;
	uint32_t rpt_isexl_pkts;
	uint32_t rpt_toin_pkts;
	uint32_t rpt_toexl_pkts;
	uint32_t rpt_alw_pkts;
	uint32_t rpt_blk_pkts;
	uint32_t lev_v2_pkts;
	uint32_t invalid_pkts;
};

struct igmp
{
	struct ipmc_if  *pif;					/*back to ipmc interface*/
	//struct thread	*query_timer; 			/* timer: issue IGMP general queries */
	//struct thread	*other_querier_timer;   /* timer: other querier present */
	TIMERID			query_timer; 			/* timer: issue IGMP general queries */
	TIMERID			other_querier_timer;   /* timer: other querier present */
	struct list		*group_list; 			/* list of struct igmp_group */
	struct list     *join_list;				/*ipmc static ip list*/

	/*igmp parament*/
	uint8_t			ssm_map_enable;			/*1:enable 0:disable*/
	uint32_t		cfg_flag;				/*use for paraments config*/
	uint8_t 		version;   				/*igmp version*/
	uint8_t			fast_leave;
	uint8_t  		req_alert;				/*igmp require router alert*/
	uint8_t  		send_alert;				/*igmp send router alert*/
	uint8_t 		max_resp_dsec;			/*igmp max response time (1-25)*/
	uint16_t			keepalive;				/*igmp other querier keepalive time*/
	int				qqi;      				/* QQI  querier_query_interval*/
	int				qrv; 	 				/* QRV querier_robustness_variable*/
	int				start_qry_cnt;
	int 			spec_max_resp_dsec;

	/*igmp packet statistics*/
	struct igmp_pkt_count count;
};

struct igmp_ssm_map
{
	uint32_t grpip;
	uint32_t prefix;
  	uint32_t sip;
};

struct igmp_instance
{
	uint8_t 		id;         /*instance id*/
	
	struct list 	*mcif;		/*interface enable igmp*/
	//struct list     *grplist;	/*igmp_join:  grpip/makslen + sip*/
	struct list		*ssm_map;	/*igmp ssm mapping: grpip/masklen + sip*/
		
	uint8_t		version;			/*igmp version 1,2,3*/
	uint8_t 	keepalive_set;	/*keepalive set flag*/
	uint16_t		keepalive;		/*igmp querier keepalive time (60-300)*/
	uint16_t 	qqi;			/*igmp querier query interval (1-18000)*/	
	uint8_t 	robust_count; 	/*igmp robust count (2-5) */
	uint8_t 	fast_leave; 		/*1:fast-leave enable; 0:fast-leave disable*/
	uint8_t		last_mem_qqi;   /*last member query interl (1-5)*/
	uint8_t		max_resp;		/*max response time (1-25)*/
	uint8_t		send_alert;		/*send router alert (0:disable 1:enable)*/
	uint8_t		require_alert;	/*require router alert (0:disable 1:enable)*/
};

struct igmp_pkt_debug
{
	uint8_t  pkt_rsv;      /* packet receive */
	uint8_t  pkt_snd;      /* packet send */
};

struct igmp_instance *igmp_instance_create(uint8_t id);
struct igmp_instance *igmp_instance_lookup(uint8_t id);
int igmp_instance_delete(uint8_t id);
int igmp_instance_mcif_get(uint8_t id, uint32_t ifindex);
void igmp_instance_mcif_del(uint8_t id, uint32_t ifindex);
int igmp_instance_find(uint32_t ifindex);

struct igmp_ssm_map *igmp_instance_ssm_map_lookup(
		struct igmp_instance *pinst, struct igmp_ssm_map *pmap);
int igmp_instance_ssm_map_add(struct igmp_instance *pinst, 
		struct igmp_ssm_map *pmap);
void igmp_instance_ssm_map_del(struct igmp_instance *pinst, 
		struct igmp_ssm_map *pmap);

#if 0
struct igmp_join * igmp_instance_mip_lookup(uint8_t id, struct igmp_join *grp);
int igmp_instance_mip_add(uint8_t id, struct igmp_join *grp);
int igmp_instance_mip_delete(uint8_t id, struct igmp_join *grp);
#endif

int recv_igmp_query(struct igmpv3_query *query, uint8_t query_version, struct ip_control *pipcb);
int igmp_v3_report(struct igmpv3_report *report, struct ip_control *pipcb);
int igmp_v2_report(struct igmpv12_report *report, struct ip_control *pipcb);
int igmp_v1_report(struct igmpv12_report *report, struct ip_control *pipcb);
int igmp_v2_leave(struct igmpv12_report *leave, struct ip_control *pipcb);

#if 0
void igmp_other_querier_timer_on(struct igmp *igmp);
void igmp_other_querier_timer_off(struct igmp *igmp);
void igmp_general_query_on(struct igmp *igmp);
void igmp_general_query_off(struct igmp *igmp);
#endif

void igmp_other_querier_timer_start(struct igmp *pigmp);
void igmp_other_querier_timer_stop(struct igmp *pigmp);
int igmp_general_query_timer_start(void *para);
void igmp_general_query_timer_stop(struct igmp *pigmp);
void igmp_startup_mode_on(struct igmp *igmp);
void igmp_group_delete_empty_include(struct igmp_group *group);
//void igmp_group_timer_on(struct igmp_group *group, long interval_msec);
void igmp_group_timer_start(struct igmp_group *pgrp, long interval_msec);

struct igmp *igmp_new(struct ipmc_if *pif);
void igmp_delete(struct igmp *igmp);

struct igmp_group * igmp_add_group_by_addr(struct igmp *igmp, uint32_t grpip);
struct igmp_join * igmp_join_lookup(struct igmp *igmp, struct igmp_join *join);
int  igmp_join_add(struct igmp *igmp, struct igmp_join *join);
int igmp_join_delete(struct igmp *igmp, struct igmp_join *join);

void igmp_anysource_forward_start(struct igmp_group *group);
void igmp_anysource_forward_stop(struct igmp_group *group);
void igmp_source_forward_start(struct igmp_source *source);
void igmp_source_forward_stop(struct igmp_source *source);
void igmp_init(void);

#endif

