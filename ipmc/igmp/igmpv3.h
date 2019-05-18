#ifndef IGMPV3_H
#define IGMPV3_H

/* GMI: Group Membership Interval */
#define IGMP_GMI_MSEC(qrv,qqi,qri_dsec) ((qrv) * (1000 * (qqi)) + 100 * (qri_dsec))

/* OQPI: Other Querier Present Interval */
#if 0
#define IGMP_OQPI_MSEC(qrv,qqi,qri_dsec) ((qrv) * (1000 * qqi) + 100 * ((qri_dsec) >> 1))
#else
#define IGMP_OQPI_MSEC(qrv,qqi,qri_dsec) ((qrv) * (1000 * qqi) + 1000 * ((qri_dsec) >> 1))
#endif

/* SQI: Startup Query Interval */
#define IGMP_SQI(qi) (((qi) < 4) ? 1 : ((qi) >> 2))

/* LMQT: Last Member Query Time */
#define IGMP_LMQT_MSEC(lmqi_dsec, lmqc) ((lmqc) * (100 * (lmqi_dsec)))

/* OHPI: Older Host Present Interval */
#define IGMP_OHPI_DSEC(qrv,qqi,qri_dsec) ((qrv) * (10 * (qqi)) + (qri_dsec))

#define IGMP_FORCE_BOOLEAN(expr) 	((expr) != 0)

#define IGMP_MODE_IS_IN		1
#define IGMP_MODE_IS_EXL	2
#define IGMP_TO_IN_MODE 	3
#define IGMP_TO_EXL_MODE 	4
#define IGMP_ALLOW_NEW_SRCS	5
#define IGMP_BLOCK_OLD_SRCS 6

#define IGMP_V3_SOURCES_OFFSET 12

void group_retransmit_timer_stop(struct igmp_group *pgrp);

void igmp_group_reset_gmi(struct igmp_group *group);

void igmp_source_reset_gmi(struct igmp *igmp, 
			struct igmp_group *group, struct igmp_source *source);

void igmp_source_free(struct igmp_source *source);

void igmp_source_delete(struct igmp_source *source);

void igmp_source_delete_expired(struct list *source_list);

struct igmp_source * igmp_find_source_by_addr(struct igmp_group *group, uint32_t sip);

void igmpv3_report_isin(struct igmp *igmp, uint32_t from,
			uint32_t grp_addr, int num_sources, uint32_t *psrc);

void igmpv3_report_isex(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources);

void igmpv3_report_toin(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources);

void igmpv3_report_toex(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources);

void igmpv3_report_allow(struct igmp *igmp, uint32_t from,
			 uint32_t group_addr, int num_sources, uint32_t *sources);

void igmpv3_report_block(struct igmp *igmp, uint32_t from,
			  uint32_t group_addr, int num_sources, uint32_t *sources);

void igmp_group_timer_lower_to_lmqt(struct igmp_group *group);

void igmp_source_timer_lower_to_lmqt(struct igmp_source *source);

void igmp_send_membership_query(struct igmp *igmp, struct igmp_group *group,
			uint32_t *sip, int num_srcs, uint32_t dip, uint32_t grpip, uint8_t s_flag);

#endif

