/*
 * =====================================================================================
 *
 *       Filename:  igmpv3.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/03/2017 11:02:03 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <string.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/memory.h>
#include <lib/checksum.h>
#include <lib/hptimer.h>

#include "igmp_pkt.h"
#include "ipmc_if.h"
#include "ipmc_main.h"
#include "igmp_time.h"
#include "igmpv3.h"

#define IGMP_BUFSIZE (1024)

//static void group_retransmit_timer_on(struct igmp_group *group);
static void group_retransmit_timer_start(struct igmp_group *pgrp);
static long igmp_group_timer_remain_msec(struct igmp_group *group);
static long igmp_source_timer_remain_msec(struct igmp_source *source);
static void group_query_send(struct igmp_group *group);
static void source_query_send_by_flag(struct igmp_group *group,
					   int num_sources_tosend);


void igmp_group_reset_gmi(struct igmp_group *group)
{
	long int group_membership_interval_msec;
	struct igmp *igmp = NULL;

	igmp = group->grp_igmp;

	 /*
	 RFC 3376: 8.4. Group Membership Interval

	 The Group Membership Interval is the amount of time that must pass
	 before a multicast router decides there are no more members of a
	 group or a particular source on a network.

	 This value MUST be ((the Robustness Variable) times (the Query
	 Interval)) plus (one Query Response Interval).

	 group_membership_interval_msec = querier_robustness_variable *
									  (1000 * querier_query_interval) +
									  100 * query_response_interval_dsec;
	 */
 	group_membership_interval_msec =
		 IGMP_GMI_MSEC(igmp->qrv, igmp->qqi, igmp->max_resp_dsec);


	zlog_debug(IGMP_DBG_RCV, "Resetting group %d timer to GMI=%ld.%03ld\n",
			group->grp_addr,
			group_membership_interval_msec / 1000,
			group_membership_interval_msec % 1000);

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and
	it represents the time for the *filter-mode* of the group to
	expire and switch to INCLUDE mode.
	*/
 	zassert(group->grp_filtermode);

 	//igmp_group_timer_on(group, group_membership_interval_msec);
 	igmp_group_timer_start(group, group_membership_interval_msec);
}

#if 0 
static int igmp_source_timer(struct thread *t)
{
	struct igmp_source *source = NULL;
	struct igmp_group *group = NULL;

	zassert(t);
	source = THREAD_ARG(t);
	zassert(source);

 	group = source->src_grp;
	if (NULL == group)
	{
		return ERRNO_FAIL;
	}

	IPMC_LOG_DEBUG("Source timer expired for group %d source %d\n",
		group->grp_addr, source->sip);

 	zassert(source->src_timer);
 	source->src_timer = NULL;

	 /*
	 RFC 3376: 6.3. IGMPv3 Source-Specific Forwarding Rules

	 Group
	 Filter-Mode	Source Timer Value	  Action
	 -----------	------------------	  ------
	 INCLUDE		TIMER == 0			  Suggest to stop forwarding
										  traffic from source and
										  remove source record.  If
										  there are no more source
										  records for the group, delete
										  group record.

	 EXCLUDE		TIMER == 0			  Suggest to not forward
										  traffic from source
										  (DO NOT remove record)

	 Source timer switched from (T > 0) to (T == 0): disable forwarding.
	 */

 	zassert(!source->src_timer);

	if (group->grp_filtermode)
	{
		 /* EXCLUDE mode */
		 igmp_source_forward_stop(source);
	}
	else 
	{
	 	/* INCLUDE mode */
		
		 /* igmp_source_delete() will stop forwarding source */
		 igmp_source_delete(source);
		/*
		If there are no more source records for the group, delete group
		record.
		*/
		if (!listcount(group->grp_src_list)) 
		{
			igmp_group_delete_empty_include(group);
		}
	}

	return ERRNO_SUCCESS;
}
 
static void source_timer_off(struct igmp_group *group,
				  struct igmp_source *source)
{
	if (NULL == source->src_timer)
	{
	 	return;
	}

	IPMC_LOG_DEBUG("Cancelling TIMER event for group %d source %d\n",
		group->grp_addr, source->sip);

	THREAD_OFF(source->src_timer);
	zassert(!source->src_timer);
}
 
static void igmp_source_timer_on(struct igmp_group *group,
			  struct igmp_source *source, long int interval_msec)
{
	source_timer_off(group, source);


	 IPMC_LOG_DEBUG ("Scheduling %ld.%03ld sec TIMER event for group %d source %d\n",
			interval_msec / 1000, interval_msec % 1000, group->grp_addr, source->sip);

	THREAD_TIMER_MSEC_ON(ipmc_master, source->src_timer,
			igmp_source_timer, source, interval_msec);
	zassert(source->src_timer);

	/*
	 RFC 3376: 6.3. IGMPv3 Source-Specific Forwarding Rules
	 
	 Source timer switched from (T == 0) to (T > 0): enable forwarding.
	*/
	igmp_source_forward_start(source);
}
#endif

static int igmp_source_timer_out(void *pada)
{
	struct igmp_source *psrc = NULL;
	struct igmp_group *pgrp = NULL;

	psrc = (struct igmp_source *)pada;
	if (NULL == psrc)
	{
		return ERRNO_FAIL;
	}
	
 	pgrp = psrc->src_grp;
	if (NULL == pgrp)
	{
		return ERRNO_FAIL;
	}

	zlog_debug(IGMP_DBG_RCV, "Source timer expired for group %d source %d\n",
		pgrp->grp_addr, psrc->sip);

 	//zassert(source->src_timer);
 	//source->src_timer = NULL;
	psrc->src_timer = 0;
	
	 /*
	 RFC 3376: 6.3. IGMPv3 Source-Specific Forwarding Rules

	 Group
	 Filter-Mode	Source Timer Value	  Action
	 -----------	------------------	  ------
	 INCLUDE		TIMER == 0			  Suggest to stop forwarding
										  traffic from source and
										  remove source record.  If
										  there are no more source
										  records for the group, delete
										  group record.

	 EXCLUDE		TIMER == 0			  Suggest to not forward
										  traffic from source
										  (DO NOT remove record)

	 Source timer switched from (T > 0) to (T == 0): disable forwarding.
	 */

 	zassert(!psrc->src_timer);

	if (pgrp->grp_filtermode)
	{
		 /* EXCLUDE mode */
		 igmp_source_forward_stop(psrc);
	}
	else 
	{
	 	/* INCLUDE mode */
		
		 /* igmp_source_delete() will stop forwarding source */
		 igmp_source_delete(psrc);
		/*
		If there are no more source records for the group, delete group
		record.
		*/
		if (!listcount(pgrp->grp_src_list)) 
		{
			igmp_group_delete_empty_include(pgrp);
		}
	}

	return ERRNO_SUCCESS;
}
 
static void igmp_source_timer_stop(struct igmp_source *psrc)
{
	if ((NULL == psrc) || (0 == psrc->src_timer))
	{
	 	return;
	}

	high_pre_timer_delete(psrc->src_timer);
	psrc->src_timer = 0;
}
 
static void igmp_source_timer_start(struct igmp_group *pgrp,
			  struct igmp_source *psrc, long int interval_msec)
{
	char timerName[20] = "SourceTimer";
	
	if ((NULL == pgrp) || (NULL == psrc))
	{
		return;
	}
	
	igmp_source_timer_stop(psrc);


	zlog_debug(IGMP_DBG_RCV, "Scheduling %ld.%03ld sec TIMER event for group %d source %d\n",
			interval_msec / 1000, interval_msec % 1000, pgrp->grp_addr, psrc->sip);

	psrc->src_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, 
			igmp_source_timer_out, (void *)psrc, interval_msec);

	/*
	 RFC 3376: 6.3. IGMPv3 Source-Specific Forwarding Rules
	 
	 Source timer switched from (T == 0) to (T > 0): enable forwarding.
	*/
	igmp_source_forward_start(psrc);
}

void igmp_source_reset_gmi(struct igmp *igmp,
				struct igmp_group *group,
				struct igmp_source *source)
{
	long group_membership_interval_msec;

	group_membership_interval_msec = IGMP_GMI_MSEC(igmp->qrv, igmp->qqi, igmp->max_resp_dsec);

 	zlog_debug(IGMP_DBG_RCV, "Resetting source %d timer to GMI=%ld.%03ld sec for group %d on %d\n",
		source->sip, group_membership_interval_msec / 1000, group_membership_interval_msec % 1000,
		group->grp_addr, igmp->pif->ifindex);

	igmp_source_timer_start(group, source, group_membership_interval_msec);
	
	//igmp_source_timer_on(group, source, group_membership_interval_msec);
}
 
static void source_mark_delete_flag(struct list *source_list)
{
	struct listnode	  *src_node;
	struct igmp_source *src;

	for (ALL_LIST_ELEMENTS_RO(source_list, src_node, src)) 
	{
 		IGMP_SOURCE_DO_DELETE(src->src_flags);
	}
}
 
static void source_mark_send_flag(struct list *source_list)
{
	struct listnode	  *src_node;
	struct igmp_source *src;

	for (ALL_LIST_ELEMENTS_RO(source_list, src_node, src)) 
	{
 		IGMP_SOURCE_DO_SEND(src->src_flags);
	}
}
 
static int source_mark_send_flag_by_timer(struct list *source_list)
{
	struct listnode	  *src_node;
	struct igmp_source *src;
	int	num_marked_sources = 0;

	for (ALL_LIST_ELEMENTS_RO(source_list, src_node, src)) 
	{
	 	/* Is source timer running? */
	 	if (src->src_timer) 
		{
	   		IGMP_SOURCE_DO_SEND(src->src_flags);
	   		++num_marked_sources;
	 	}
	 	else 
		{
	   		IGMP_SOURCE_DONT_SEND(src->src_flags);
	 	}
	}

	return num_marked_sources;
}
 
static void source_clear_send_flag(struct list *source_list)
{
	struct listnode	  *src_node;
	struct igmp_source *src;

	for (ALL_LIST_ELEMENTS_RO(source_list, src_node, src)) 
	{
		IGMP_SOURCE_DONT_SEND(src->src_flags);
	}
}
 
/*
Any source (*,G) is forwarded only if mode is EXCLUDE {empty}
*/
static void group_exclude_fwd_anysrc_ifempty(struct igmp_group *group)
{
	zassert(group->grp_filtermode);

	if (listcount(group->grp_src_list) < 1) 
	{
 		igmp_anysource_forward_start(group);
	}
}
 
void igmp_source_free(struct igmp_source *source)
{
	/* make sure there is no source timer running */
	zassert(!source->src_timer);

	XFREE(MTYPE_IPMC_IGMP_GROUP_SOURCE, source);
}
 
static void source_channel_oil_detach(struct igmp_source *source)
{
//   if (source->source_channel_oil) {
//	 pim_channel_oil_del(source->source_channel_oil);
//	 source->source_channel_oil = 0;
//   }
}
 
/*
igmp_source_delete:		 stop fowarding, and delete the source
igmp_source_forward_stop: stop fowarding, but keep the source
*/
void igmp_source_delete(struct igmp_source *source)
{
	struct igmp_group *group = NULL;

	group = source->src_grp;
	if (NULL == group)
	{
		return;
	}

	zlog_debug(IGMP_DBG_RCV, "Deleting IGMP source %d for group %d\n",
		source->sip, group->grp_addr);

 	//source_timer_off(group, source);
 	igmp_source_timer_stop(source);
 	igmp_source_forward_stop(source);

 	/* sanity check that forwarding has been disabled */
 	if (IGMP_SOURCE_TEST_FORWARDING(source->src_flags)) 
	{
		zlog_warn("%s: forwarding=ON(!) IGMP source %d for group %d",
		   __FUNCTION__, source->sip, group->grp_addr);
		/* warning only */
	}

 	source_channel_oil_detach(source);

	 /*
	 notice that listnode_delete() can't be moved
	 into igmp_source_free() because the later is
	 called by list_delete_all_node()
	 */
 	listnode_delete(group->grp_src_list, source);

 	igmp_source_free(source);

 	if (group->grp_filtermode) 
 	{
	 	group_exclude_fwd_anysrc_ifempty(group);
 	}
}
 
static void source_delete_by_flag(struct list *source_list)
{
	struct listnode	*src_node;
	struct listnode	*src_nextnode;
	struct igmp_source *src;

	for (ALL_LIST_ELEMENTS(source_list, src_node, src_nextnode, src))
	{
		if (IGMP_SOURCE_TEST_DELETE(src->src_flags))
		{
			igmp_source_delete(src);
		}
	}
}
 
void igmp_source_delete_expired(struct list *source_list)
{
	struct listnode	*src_node;
	struct listnode	*src_nextnode;
	struct igmp_source *src;

 	for (ALL_LIST_ELEMENTS(source_list, src_node, src_nextnode, src))
	{
	 	if (0 == src->src_timer)
		{
			igmp_source_delete(src);
		}
	}
}

struct igmp_source *igmp_find_source_by_addr(struct igmp_group *group, uint32_t sip)
{
	struct listnode	*src_node;
	struct igmp_source *src;

	for (ALL_LIST_ELEMENTS_RO(group->grp_src_list, src_node, src))
	{
	 	if (sip == src->sip)
	 	{
			return src;
	 	}
	}

	return NULL;
}

static struct igmp_source *source_new(struct igmp_group *group, uint32_t sip)
{
	struct igmp_source *src = NULL;

	zlog_debug(IGMP_DBG_RCV, "Creating new IGMP source %d for group %d\n", sip, group->grp_addr);
	
	src = XMALLOC(MTYPE_IPMC_IGMP_GROUP_SOURCE, sizeof(struct igmp_source));
	if (NULL == src) 
	{
		zlog_warn("%s %s: XMALLOC() failure", __FILE__, __FUNCTION__);
		
		return NULL; /* error, not found, could not create */
	}

	src->src_timer	= 0;
	src->src_grp	= group; /* back pointer */
	src->sip		= sip;
	src->src_create	= igmp_time_monotonic_sec();
	src->src_flags	= 0;
	src->sqrt 		= 0;

 	listnode_add(group->grp_src_list, src);

 	zassert(!src->src_timer); /* source timer == 0 */

 	/* Any source (*,G) is forwarded only if mode is EXCLUDE {empty} */
 	igmp_anysource_forward_stop(group);

 	return src;
}
 
static struct igmp_source *add_source_by_addr(struct igmp *igmp,
					   struct igmp_group *group, uint32_t sip)
{
	struct igmp_source *src = NULL;

	src = igmp_find_source_by_addr(group, sip);
	if (NULL != src) 
	{
	 	return src;
	}

	src = source_new(group, sip);
	if (NULL == src) 
	{
	 	return NULL;
	}

	return src;
}
 

static void allow(struct igmp *igmp, uint32_t from,
		   uint32_t grp_addr, int num_sources, uint32_t *psrc)
{
	struct igmp_source *source = NULL;
	struct igmp_group *group = NULL;
	uint32_t sip;
	int	i;

	/* non-existant group is created as INCLUDE {empty} */
	group = igmp_add_group_by_addr(igmp, grp_addr);
	zlog_debug(IGMP_DBG_RCV, "%s:%d grp_addr %d.%d.%d.%d\n", __FUNCTION__, __LINE__,
		(grp_addr>>24)&(0XFF), (grp_addr>>16)&(0XFF), (grp_addr>>8)&(0XFF), grp_addr&(0XFF));
	if (NULL == group) 
	{
	 	return;
	}

	/* scan received sources */
	for (i = 0; i < num_sources; ++i) 
	{
	 	sip = ntohl(psrc[i]);
		zlog_debug(IGMP_DBG_RCV, "%s:%d sip %d.%d.%d.%d\n", __FUNCTION__, __LINE__,
			(sip>>24)&(0XFF), (sip>>16)&(0XFF), (sip>>8)&(0XFF), sip&(0XFF));
	 	source = add_source_by_addr(igmp, group, sip);
		if (NULL == source) 
		{
		 	/*wether need to create new source*/
		 	continue;
		}

		/*
		RFC 3376: 6.4.1. Reception of Current-State Records

		When receiving IS_IN reports for groups in EXCLUDE mode is
		sources should be moved from set with (timers = 0) to set with
		(timers > 0).

		igmp_source_reset_gmi() below, resetting the source timers to
		GMI, accomplishes this.
		*/
	 	igmp_source_reset_gmi(igmp, group, source);
	} /* scan received sources */
}
 

void igmpv3_report_isin(struct igmp *igmp, uint32_t from,
			uint32_t grp_addr, int num_sources, uint32_t *psrc)
{
	zlog_debug(IGMP_DBG_RCV, "%s:%d from:%d grp_addr:%d num_sources:%d\n", __FUNCTION__, __LINE__,
		from, grp_addr, num_sources);

	allow(igmp, from, grp_addr, num_sources, psrc);
}

static void isex_excl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	uint32_t sip;
	int i;

	/* EXCLUDE mode */
	zassert(group->grp_filtermode);

	/* E.1: set deletion flag for known sources (X,Y) */
	source_mark_delete_flag(group->grp_src_list);

	/* scan received sources (A) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* E.2: lookup reported source from (A) in (X,Y) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (NULL != psrc) 
		{
			/* E.3: if found, clear deletion flag: (X*A) or (Y*A) */
			IGMP_SOURCE_DONT_DELETE(psrc->src_flags);
		}
		else 
		{
			/* E.4: if not found, create source with timer=GMI: (A-X-Y) */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}
			
			zassert(!psrc->src_timer); /* timer == 0 */
			igmp_source_reset_gmi(group->grp_igmp, group, psrc);
			zassert(psrc->src_timer); /* (A-X-Y) timer > 0 */
		}

	} /* scan received sources */

	/* E.5: delete all sources marked with deletion flag: (X-A) and (Y-A) */
	source_delete_by_flag(group->grp_src_list);
}

static void isex_incl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	uint32_t sip;
	int i;

	/* INCLUDE mode */
	zassert(!group->grp_filtermode);

	/* I.1: set deletion flag for known sources (A) */
	source_mark_delete_flag(group->grp_src_list);

	/* scan received sources (B) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* I.2: lookup reported source (B) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (psrc) 
		{
			/* I.3: if found, clear deletion flag (A*B) */
			IGMP_SOURCE_DONT_DELETE(psrc->src_flags);
		}
		else 
		{
			/* I.4: if not found, create source with timer=0 (B-A) */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}
			
			zassert(!psrc->src_timer); /* (B-A) timer=0 */
		}
	} /* scan received sources */

	/* I.5: delete all sources marked with deletion flag (A-B) */
	source_delete_by_flag(group->grp_src_list);

	group->grp_filtermode = 1; /* boolean=true */

	zassert(group->grp_filtermode);

	group_exclude_fwd_anysrc_ifempty(group);
}

void igmpv3_report_isex(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources)
{
	struct igmp_group *group = NULL;

	/* non-existant group is created as INCLUDE {empty} */
	group = igmp_add_group_by_addr(igmp, group_addr);
	if (NULL == group)
	{
		return;
	}

	if (group->grp_filtermode) 
	{
		/* EXCLUDE mode */
		isex_excl(group, num_sources, sources);
	}
	else 
	{
		/* INCLUDE mode */
		isex_incl(group, num_sources, sources);
		zassert(group->grp_filtermode);
	}

	zassert(group->grp_filtermode);

	igmp_group_reset_gmi(group);
}

static void toin_incl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	struct igmp *igmp = NULL;
	int num_sources_tosend;
	uint32_t sip;
	int i;

	igmp = group->grp_igmp;
	if (NULL == igmp)
	{
		return;
	}

	num_sources_tosend = listcount(group->grp_src_list);
	
	/* Set SEND flag for all known sources (A) */
	source_mark_send_flag(group->grp_src_list);

	/* Scan received sources (B) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* Lookup reported source (B) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (psrc) 
		{
			/* If found, clear SEND flag (A*B) */
			IGMP_SOURCE_DONT_SEND(psrc->src_flags);
			--num_sources_tosend;
		}
		else 
		{
			/* If not found, create new source */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}
		}

		/* (B)=GMI */
		igmp_source_reset_gmi(igmp, group, psrc);
	}

	/* Send sources marked with SEND flag: Q(G,A-B) */
	if (num_sources_tosend > 0) 
	{
		source_query_send_by_flag(group, num_sources_tosend);
	}
}

static void toin_excl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	struct igmp *igmp = NULL; 
	uint32_t sip;
	int num_sources_tosend;
	int i;

	igmp = group->grp_igmp;
	if (NULL == igmp)
	{
		return;
	}
	
	/* Set SEND flag for X (sources with timer > 0) */
	num_sources_tosend = source_mark_send_flag_by_timer(group->grp_src_list);

	/* Scan received sources (A) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* Lookup reported source (A) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (psrc) 
		{
			if (psrc->src_timer) 
			{
				/* If found and timer running, clear SEND flag (X*A) */
				IGMP_SOURCE_DONT_SEND(psrc->src_flags);
				--num_sources_tosend;
			}
		}
		else 
		{
			/* If not found, create new source */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}
		}

		/* (A)=GMI */
		igmp_source_reset_gmi(igmp, group, psrc);
	}

	/* Send sources marked with SEND flag: Q(G,X-A) */
	if (num_sources_tosend > 0) 
	{
		source_query_send_by_flag(group, num_sources_tosend);
	}

	/* Send Q(G) */
	group_query_send(group);
}

void igmpv3_report_toin(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources)
{
	struct igmp_group *group = NULL;

	/* non-existant group is created as INCLUDE {empty} */
	group = igmp_add_group_by_addr(igmp, group_addr);
	if (NULL == group) 
	{
		return;
	}

	if (group->grp_filtermode) 
	{
		/* EXCLUDE mode */
		toin_excl(group, num_sources, sources);
	}
	else 
	{
		/* INCLUDE mode */
		toin_incl(group, num_sources, sources);
	}
}

static void toex_incl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	int num_sources_tosend = 0;
	uint32_t sip;
	int i;

	zassert(!group->grp_filtermode);

	/* Set DELETE flag for all known sources (A) */
	source_mark_delete_flag(group->grp_src_list);

	/* Clear off SEND flag from all known sources (A) */
	source_clear_send_flag(group->grp_src_list);

	/* Scan received sources (B) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* Lookup reported source (B) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (psrc) 
		{
			/* If found, clear deletion flag: (A*B) */
			IGMP_SOURCE_DONT_DELETE(psrc->src_flags);
			/* and set SEND flag (A*B) */
			IGMP_SOURCE_DO_SEND(psrc->src_flags);
			++num_sources_tosend;
		}
		else 
		{
			/* If source not found, create source with timer=0: (B-A)=0 */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}
			zassert(!psrc->src_flags); /* (B-A) timer=0 */
		}
	} /* Scan received sources (B) */

	group->grp_filtermode = 1; /* boolean=true */

	/* Delete all sources marked with DELETE flag (A-B) */
	source_delete_by_flag(group->grp_src_list);

	/* Send sources marked with SEND flag: Q(G,A*B) */
	if (num_sources_tosend > 0) 
	{
		source_query_send_by_flag(group, num_sources_tosend);
	}

	zassert(group->grp_filtermode);

	group_exclude_fwd_anysrc_ifempty(group);
}

static void toex_excl(struct igmp_group *group,
			  int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
	int num_sources_tosend = 0;
	long group_timer_msec;
	uint32_t sip;
	int i;

	/* set DELETE flag for all known sources (X,Y) */
	source_mark_delete_flag(group->grp_src_list);

	/* clear off SEND flag from all known sources (X,Y) */
	source_clear_send_flag(group->grp_src_list);

	/* scan received sources (A) */
	for (i = 0; i < num_sources; ++i) 
	{
		sip = ntohl(sources[i]);

		/* lookup reported source (A) in known sources (X,Y) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (psrc) 
		{
			/* if found, clear off DELETE flag from reported source (A) */
			IGMP_SOURCE_DONT_DELETE(psrc->src_flags);
		}
		else 
		{
			/* if not found, create source with Group Timer: (A-X-Y)=Group Timer */
			psrc = source_new(group, sip);
			if (NULL == psrc) 
			{
				/* ugh, internal malloc failure, skip source */
				continue;
			}

			zassert(!psrc->src_timer); /* timer == 0 */
			group_timer_msec = igmp_group_timer_remain_msec(group);
			//igmp_source_timer_on(group, psrc, group_timer_msec);
			igmp_source_timer_start(group, psrc, group_timer_msec);
			zassert(psrc->src_timer); /* (A-X-Y) timer > 0 */

			/* make sure source is created with DELETE flag unset */
			zassert(!IGMP_SOURCE_TEST_DELETE(psrc->src_flags));
		}

		/* make sure reported source has DELETE flag unset */
		zassert(!IGMP_SOURCE_TEST_DELETE(psrc->src_flags));

		if (psrc->src_timer) 
		{
			/* if source timer>0 mark SEND flag: Q(G,A-Y) */
			IGMP_SOURCE_DO_SEND(psrc->src_flags);
			++num_sources_tosend;
		}
	} /* scan received sources (A) */

	/*
	delete all sources marked with DELETE flag:
	Delete (X-A)
	Delete (Y-A)
	*/
	source_delete_by_flag(group->grp_src_list);

	/* send sources marked with SEND flag: Q(G,A-Y) */
	if (num_sources_tosend > 0) 
	{
		source_query_send_by_flag(group, num_sources_tosend);
	}
}

void igmpv3_report_toex(struct igmp *igmp, uint32_t from,
			uint32_t group_addr, int num_sources, uint32_t *sources)
{
	struct igmp_group *group = NULL;

	zlog_debug(IGMP_DBG_RCV, "%s:%d from:%d group:%d num_src:%d\n", __FUNCTION__, __LINE__,
			from, group_addr, num_sources);

	/* non-existant group is created as INCLUDE {empty} */
	group = igmp_add_group_by_addr(igmp, group_addr);
	if (NULL == group) 
	{
		return;
	}

	if (group->grp_filtermode) 
	{
		/* EXCLUDE mode */
		toex_excl(group, num_sources, sources);
	}
	else 
	{
		/* INCLUDE mode */
		toex_incl(group, num_sources, sources);
	}
	
	zassert(group->grp_filtermode);

	/* Group Timer=GMI */
	igmp_group_reset_gmi(group);
}

void igmpv3_report_allow(struct igmp *igmp, uint32_t from,
			 uint32_t group_addr, int num_sources, uint32_t *sources)
{
	allow(igmp, from, group_addr, num_sources, sources);
}

/*
  RFC3376: 6.6.3.1. Building and Sending Group Specific Queries

  When transmitting a group specific query, if the group timer is
  larger than LMQT, the "Suppress Router-Side Processing" bit is set
  in the query message.
*/
static void group_retransmit_group(struct igmp_group *group)
{
	struct igmp *igmp;
	long				  lmqc; 	 /* Last Member Query Count */
	long				  lmqi_msec; /* Last Member Query Interval */
	long				  lmqt_msec; /* Last Member Query Time */
	int 				  s_flag;

	igmp = group->grp_igmp;

	lmqc	  = igmp->qrv;
	lmqi_msec = 100 * igmp->spec_max_resp_dsec;
	lmqt_msec = lmqc * lmqi_msec;

  /*
	RFC3376: 6.6.3.1. Building and Sending Group Specific Queries
	
	When transmitting a group specific query, if the group timer is
	larger than LMQT, the "Suppress Router-Side Processing" bit is set
	in the query message.
  */
	s_flag = igmp_group_timer_remain_msec(group) > lmqt_msec;

	zlog_debug(IGMP_DBG_SND, "retransmit_group_specific_query: group %d on %d: s_flag=%d count=%d\n",
		   group->grp_addr, igmp->pif->ifindex, s_flag, group->grp_sqrt_count);

  /*
	RFC3376: 4.1.12. IP Destination Addresses for Queries

	Group-Specific and Group-and-Source-Specific Queries are sent with
	an IP destination address equal to the multicast address of
	interest.
  */

	igmp_send_membership_query(igmp, group, NULL, 0 /* num_sources_tosend */, 
		group->grp_addr/* dst_addr */, group->grp_addr /* group_addr */, s_flag);
}


/*
RFC3376: 6.6.3.2. Building and Sending Group and Source Specific Queries

When building a group and source specific query for a group G, two
separate query messages are sent for the group.	The first one has
the "Suppress Router-Side Processing" bit set and contains all the
sources with retransmission state and timers greater than LMQT.	The
second has the "Suppress Router-Side Processing" bit clear and
contains all the sources with retransmission state and timers lower
or equal to LMQT.  If either of the two calculated messages does not
contain any sources, then its transmission is suppressed.
*/
static int group_retransmit_sources(struct igmp_group *group,
					 int send_with_sflag_set)
{
 	uint32_t query_buf1[IGMP_BUFSIZE]; /* 1 = with s_flag set */
 	uint32_t query_buf2[IGMP_BUFSIZE]; /* 2 = with s_flag clear */
 	struct listnode	*src_node = NULL;
	int	num_retransmit_srcs_left = 0;
 	struct igmp_source *src = NULL;
	uint32_t *src_addr1 = NULL;
 	uint32_t *src_addr2 = NULL;
	struct igmp	*igmp = NULL;
	int	num_srcs_tosend1 = 0;
 	int	num_srcs_tosend2 = 0;
	int	query_buf1_max_srcs;
 	int	query_buf2_max_srcs;
	long lmqc;	  /* Last Member Query Count */
 	long lmqi_msec; /* Last Member Query Interval */
 	long lmqt_msec; /* Last Member Query Time */
	
	query_buf1_max_srcs = (sizeof(query_buf1) - IGMP_V3_SOURCES_OFFSET) >> 2;
	query_buf2_max_srcs = (sizeof(query_buf2) - IGMP_V3_SOURCES_OFFSET) >> 2;
	 
 	src_addr1 = query_buf1;
 	src_addr2 = query_buf2;

 	igmp = group->grp_igmp;

 	lmqc	  = igmp->qrv;
 	lmqi_msec = 100 * igmp->spec_max_resp_dsec;
 	lmqt_msec = lmqc * lmqi_msec;

	/* Scan all group sources */
	for (ALL_LIST_ELEMENTS_RO(group->grp_src_list, src_node, src)) 
	{
		 /* Source has retransmission state? */
		 if (src->sqrt < 1)
		 {
			 continue;
		 }

		 if (--src->sqrt > 0) 
		 {
			 ++num_retransmit_srcs_left;
		 }

	 	/* Copy source address into appropriate query buffer */
	 	if (igmp_source_timer_remain_msec(src) > lmqt_msec) 
	 	{
		 	*src_addr1 = src->sip;
		 	++src_addr1;
			num_srcs_tosend1++;
	 	}
	 	else 
		{
		 	*src_addr2 = src->sip;
		 	++src_addr2;
			num_srcs_tosend2++;
		}
	}

	zlog_debug(IGMP_DBG_SND, "retransmit_grp&src_specific_query: group %d on %d: srcs_with_sflag=%d \
		srcs_wo_sflag=%d will_send_sflag=%d retransmit_src_left=%d\n",
		group->grp_addr, igmp->pif->ifindex, num_srcs_tosend1, num_srcs_tosend2,
		send_with_sflag_set, num_retransmit_srcs_left);

 	if (num_srcs_tosend1 > 0) 
	{
		 /*
		   Send group-and-source-specific query with s_flag set and all
		   sources with timers greater than LMQT.
		 */

	 	if (send_with_sflag_set) 
		{

		 	if (num_srcs_tosend1 > query_buf1_max_srcs) 
			{
			 	zlog_warn("%s: group %d on %d: s_flag=1 unable to fit %d sources into buf_size=%zu (max_sources=%d)",
					 __FUNCTION__, group->grp_addr, igmp->pif->ifindex,
					 num_srcs_tosend1, sizeof(query_buf1), query_buf1_max_srcs);
			}
		 	else 
			{
				 /*
				   RFC3376: 4.1.12. IP Destination Addresses for Queries
				   
				   Group-Specific and Group-and-Source-Specific Queries are sent with
				   an IP destination address equal to the multicast address of
				   interest.
				 */

				igmp_send_membership_query(igmp, group, query_buf1,
					num_srcs_tosend1, group->grp_addr, group->grp_addr, 1 /* s_flag */);
			}
		} /* send_with_sflag_set */
	}

 	if (num_srcs_tosend2 > 0) 
	{
		 /*
		   Send group-and-source-specific query with s_flag clear and all
		   sources with timers lower or equal to LMQT.
		 */
	 	if (num_srcs_tosend2 > query_buf2_max_srcs) 
		{
		 	zlog_warn("%s: group %d on %d: s_flag=0 unable to fit %d sources into buf_size=%zu (max_sources=%d)",
				 __FUNCTION__, group->grp_addr, igmp->pif->ifindex,
				 num_srcs_tosend2, sizeof(query_buf2), query_buf2_max_srcs);
		}
	 	else 
		{
			   /*
			 RFC3376: 4.1.12. IP Destination Addresses for Queries

			 Group-Specific and Group-and-Source-Specific Queries are sent with
			 an IP destination address equal to the multicast address of
			 interest.
			   */

		 	igmp_send_membership_query(igmp, group, query_buf2, 
		 		num_srcs_tosend2, group->grp_addr, group->grp_addr, 0 /* s_flag */);
		}
	}

 	return num_retransmit_srcs_left;
}
					 
#if 0 
static int igmp_group_retransmit(struct thread *t)
{
	struct igmp_group *group;
	int num_retransmit_sources_left;
	int send_with_sflag_set; /* boolean */

 	zassert(t);
 	group = THREAD_ARG(t);
 	zassert(group);

	IPMC_LOG_DEBUG("group_retransmit_timer: group %d on %d",
	 	group->grp_addr, group->grp_igmp->pif->ifindex);

 	/* Retransmit group-specific queries? (RFC3376: 6.6.3.1) */
 	if (group->grp_sqrt_count > 0) 
	{
		/* Retransmit group-specific queries (RFC3376: 6.6.3.1) */
		group_retransmit_group(group);
		--group->grp_sqrt_count;

		/*
		RFC3376: 6.6.3.2
		If a group specific query is scheduled to be transmitted at the
		same time as a group and source specific query for the same group,
		then transmission of the group and source specific message with the
		"Suppress Router-Side Processing" bit set may be suppressed.
		*/
		send_with_sflag_set = 0; /* boolean=false */
	}	 
 	else 
	 {
		 send_with_sflag_set = 1; /* boolean=true */
	 }

 	/* Retransmit group-and-source-specific queries (RFC3376: 6.6.3.2) */
 	num_retransmit_sources_left = group_retransmit_sources(group,
						  send_with_sflag_set);

 	group->grp_qrt_timer = 0;

	 /*
	 Keep group retransmit timer running if there is any retransmit
	 counter pending
	 */
	if ((num_retransmit_sources_left > 0) ||
		(group->grp_sqrt_count > 0)) 
	{
		group_retransmit_timer_on(group);
	}

 	return ERRNO_SUCCESS;
}
 
/*
group_retransmit_timer_on:
if group retransmit timer isn't running, starts it;
otherwise, do nothing
*/
static void group_retransmit_timer_on(struct igmp_group *group)
{
	struct igmp	*igmp = NULL;
	long lmqi_msec; /* Last Member Query Interval */

 	/* if group retransmit timer is running, do nothing */
 	if (group->grp_qrt_timer) 
	{
		return;
	}

 	igmp = group->grp_igmp;
 	if (NULL == igmp)
	{
		return;
	}

 	lmqi_msec = 100 * igmp->spec_max_resp_dsec;

	IPMC_LOG_DEBUG("Scheduling %ld.%03ld sec retransmit timer for group %d on %d",
		lmqi_msec / 1000, lmqi_msec % 1000, group->grp_addr, igmp->pif->ifindex);

 	THREAD_TIMER_MSEC_ON(ipmc_master, group->grp_qrt_timer,
			igmp_group_retransmit, group, lmqi_msec);
}
#endif

static int igmp_group_retransmit_timer_out(void *para)
{
	int num_retransmit_sources_left;
	struct igmp_group *pgrp = NULL;
	int send_with_sflag_set; /* boolean */

	pgrp = (struct igmp_group *)para;
 	if (NULL == pgrp)
	{
		return ERRNO_FAIL;
	}

	zlog_debug(IGMP_DBG_SND, "group_retransmit_timer: group %d on %d\n",
	 	pgrp->grp_addr, pgrp->grp_igmp->pif->ifindex);

 	/* Retransmit group-specific queries? (RFC3376: 6.6.3.1) */
 	if (pgrp->grp_sqrt_count > 0) 
	{
		/* Retransmit group-specific queries (RFC3376: 6.6.3.1) */
		group_retransmit_group(pgrp);
		--pgrp->grp_sqrt_count;

		/*
		RFC3376: 6.6.3.2
		If a group specific query is scheduled to be transmitted at the
		same time as a group and source specific query for the same group,
		then transmission of the group and source specific message with the
		"Suppress Router-Side Processing" bit set may be suppressed.
		*/
		send_with_sflag_set = 0; /* boolean=false */
	}	 
 	else 
	 {
		 send_with_sflag_set = 1; /* boolean=true */
	 }

 	/* Retransmit group-and-source-specific queries (RFC3376: 6.6.3.2) */
 	num_retransmit_sources_left = group_retransmit_sources(pgrp,
						  send_with_sflag_set);

 	pgrp->grp_qrt_timer = 0;

	 /*
	 Keep group retransmit timer running if there is any retransmit
	 counter pending
	 */
	if ((num_retransmit_sources_left > 0) ||
		(pgrp->grp_sqrt_count > 0)) 
	{
		group_retransmit_timer_start(pgrp);
	}
 
 	return ERRNO_SUCCESS;
}
 
/*
group_retransmit_timer_start:
if group retransmit timer isn't running, starts it;
otherwise, do nothing
*/
static void group_retransmit_timer_start(struct igmp_group *group)
{
	char *timerName = "GrpretransTimer";
	struct igmp	*igmp = NULL;
	long lmqi_msec; /* Last Member Query Interval */

 	/* if group retransmit timer is running, do nothing */
 	if (group->grp_qrt_timer) 
	{
		return;
	}

 	igmp = group->grp_igmp;
 	if (NULL == igmp)
	{
		return;
	}

 	lmqi_msec = 100 * igmp->spec_max_resp_dsec;

	zlog_debug(IGMP_DBG_SND, "Scheduling %ld.%03ld sec retransmit timer for group %d on %d\n",
		lmqi_msec / 1000, lmqi_msec % 1000, group->grp_addr, igmp->pif->ifindex);

	group->grp_qrt_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, 
			igmp_group_retransmit_timer_out, (void *)group, lmqi_msec);
}

void group_retransmit_timer_stop(struct igmp_group *pgrp)
{
 	/* if group retransmit timer is running, do nothing */
 	if ((NULL == pgrp) || (0 == pgrp->grp_qrt_timer))
	{
		return;
	}

 	high_pre_timer_delete( pgrp->grp_qrt_timer);
	pgrp->grp_qrt_timer = 0;
}


static long igmp_group_timer_remain_msec(struct igmp_group *group)
{
	//return ((group->grp_timer != 0) ? 1000 * thread_timer_remain_second(group->grp_timer) : 0);
	int retime;
	
	high_pre_timer_remain(group->grp_timer, &retime);

	return (long)retime;
}
 
static long igmp_source_timer_remain_msec(struct igmp_source *source)
{
	//return ((source->src_timer != 0)? 1000 * thread_timer_remain_second(source->src_timer) : 0);
	int retime;
	
	high_pre_timer_remain(source->src_timer, &retime);

	return (long)retime;
}
 
/*
RFC3376: 6.6.3.1. Building and Sending Group Specific Queries
*/
static void group_query_send(struct igmp_group *group)
{
	long lmqc;	/* Last Member Query Count */

 	lmqc = group->grp_igmp->qrv;

 	/* lower group timer to lmqt */
 	igmp_group_timer_lower_to_lmqt(group);

 	/* reset retransmission counter */
 	group->grp_sqrt_count = lmqc;

 	/* immediately send group specific query (decrease retransmit counter by 1)*/
 	group_retransmit_group(group);

 	/* make sure group retransmit timer is running */
	group_retransmit_timer_start(group);
 	//group_retransmit_timer_on(group);
}
 
/*
RFC3376: 6.6.3.2. Building and Sending Group and Source Specific Queries
*/
static void source_query_send_by_flag(struct igmp_group *group,
					   int num_sources_tosend)
{
	struct listnode	*src_node = NULL;
	struct igmp_source *src = NULL;
	struct igmp *igmp = NULL;
	long lmqc;	  /* Last Member Query Count */
	long lmqi_msec; /* Last Member Query Interval */
	long lmqt_msec; /* Last Member Query Time */

 	zassert(num_sources_tosend > 0);

 	igmp = group->grp_igmp;

 	lmqc	   = igmp->qrv;
 	lmqi_msec = 100 * igmp->spec_max_resp_dsec;
 	lmqt_msec = lmqc * lmqi_msec;

	/*
	 RFC3376: 6.6.3.2. Building and Sending Group and Source Specific Queries

	 (...) for each of the sources in X of group G, with source timer larger
	 than LMQT:
	 o Set number of retransmissions for each source to [Last Member
	 Query Count].
	 o Lower source timer to LMQT.
	*/
 	for (ALL_LIST_ELEMENTS_RO(group->grp_src_list, src_node, src)) 
	{
	 	if (IGMP_SOURCE_TEST_SEND(src->src_flags)) 
		{
			/* source "src" in X of group G */
			if (igmp_source_timer_remain_msec(src) > lmqt_msec) 
			{
			 	src->sqrt = lmqc;
			 	igmp_source_timer_lower_to_lmqt(src);
			}
		}
	}

 	/* send group-and-source specific queries */
 	group_retransmit_sources(group, 1 /* send_with_sflag_set=true */);

 	/* make sure group retransmit timer is running */
	group_retransmit_timer_start(group);
 	//group_retransmit_timer_on(group);
}
 
static void block_excl(struct igmp_group *group,
			int num_sources, uint32_t *sources)
{
	struct igmp_source *psrc = NULL;
 	int num_sources_tosend = 0;
	long group_timer_msec;
	uint32_t sip;
 	int i;

	/* 1. clear off SEND flag from all known sources (X,Y) */
	source_clear_send_flag(group->grp_src_list);

 	/* 2. scan received sources (A) */
 	for (i = 0; i < num_sources; ++i) 
	{
	 	sip = ntohl(sources[i]);

		/* lookup reported source (A) in known sources (X,Y) */
		psrc = igmp_find_source_by_addr(group, sip);
		if (!psrc) 
		{
			 /* 3: if not found, create source with Group Timer: (A-X-Y)=Group Timer */
			 psrc = source_new(group, sip);
			 if (!psrc) 
			 {
				 /* ugh, internal malloc failure, skip source */
				 continue;
			 }

			zassert(!psrc->src_timer); /* timer == 0 */
			group_timer_msec = igmp_group_timer_remain_msec(group);
			igmp_source_timer_start(group, psrc, group_timer_msec);
			//igmp_source_timer_on(group, psrc, group_timer_msec);
			zassert(psrc->src_timer); /* (A-X-Y) timer > 0 */
		}

		if (psrc->src_timer) 
		{
			 /* 4. if source timer>0 mark SEND flag: Q(G,A-Y) */
			 IGMP_SOURCE_DO_SEND(psrc->src_flags);
			 ++num_sources_tosend;
		}
	}

 	/* 5. send sources marked with SEND flag: Q(G,A-Y) */
 	if (num_sources_tosend > 0) 
 	{
		source_query_send_by_flag(group, num_sources_tosend);
 	}
}
 
static void block_incl(struct igmp_group *group,
				int num_sources, uint32_t *sources)
{
 	struct igmp_source *psrc = NULL;
	int num_sources_tosend = 0;
	uint32_t sip;
	int i;
 
	/* 1. clear off SEND flag from all known sources (B) */
	source_clear_send_flag(group->grp_src_list);
 
	/* 2. scan received sources (A) */
	for (i = 0; i < num_sources; ++i) 
	{
	 	sip = ntohl(sources[i]);

	 	/* lookup reported source (A) in known sources (B) */
	 	psrc = igmp_find_source_by_addr(group, sip);
	 	if (psrc) 
	 	{
			 /* 3. if found (A*B), mark SEND flag: Q(G,A*B) */
			 IGMP_SOURCE_DO_SEND(psrc->src_flags);
			 ++num_sources_tosend;
	 	}
	} 
  
	 /* 4. send sources marked with SEND flag: Q(G,A*B) */
	 if (num_sources_tosend > 0) 
	 {
		 source_query_send_by_flag(group, num_sources_tosend);
	 }
}
 
void igmpv3_report_block(struct igmp *igmp, uint32_t from,
			  uint32_t group_addr, int num_sources, uint32_t *sources)
{
	struct igmp_group *group = NULL;

	/* non-existant group is created as INCLUDE {empty} */
	group = igmp_add_group_by_addr(igmp, group_addr);
	if (NULL == group) 
	{
		return;
	}

	if (group->grp_filtermode) 
	{
		 /* EXCLUDE mode */
		 block_excl(group, num_sources, sources);
	}
	else 
	{
		 /* INCLUDE mode */
		 block_incl(group, num_sources, sources);
	}
}
 

void igmp_group_timer_lower_to_lmqt(struct igmp_group *group)
{
	struct igmp	*igmp = NULL;
	int	lmqi_dsec; /* Last Member Query Interval */
	int	lmqc;	   /* Last Member Query Count */
	int	lmqt_msec; /* Last Member Query Time */

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and
	it represents the time for the *filter-mode* of the group to
	expire and switch to INCLUDE mode.
	*/
	if (IGMP_MODE_IS_IN == group->grp_filtermode) 
	{
	  	return;
	}

  	igmp	  = group->grp_igmp;

  	lmqi_dsec = igmp->spec_max_resp_dsec;
  	lmqc	  = igmp->qrv;
  	lmqt_msec = IGMP_LMQT_MSEC(lmqi_dsec, lmqc); /* lmqt_msec = (100 * lmqi_dsec) * lmqc */

  	zassert(group->grp_filtermode);

  	//igmp_group_timer_on(group, lmqt_msec);
	igmp_group_timer_start(group, lmqt_msec);
}
  
void igmp_source_timer_lower_to_lmqt(struct igmp_source *source)
{
	struct igmp_group	*group = NULL;
	struct igmp			*igmp = NULL;
	struct ipmc_if		*pif = NULL;
	int	 lmqi_dsec; /* Last Member Query Interval */
	int	 lmqc;		/* Last Member Query Count */
	int	 lmqt_msec; /* Last Member Query Time */

	group   = source->src_grp;
	igmp    = group->grp_igmp;
	pif	    = igmp->pif;

	lmqi_dsec = igmp->spec_max_resp_dsec;
	lmqc 	  = igmp->qrv;
	lmqt_msec = IGMP_LMQT_MSEC(lmqi_dsec, lmqc); /* lmqt_msec = (100 * lmqi_dsec) * lmqc */

	zlog_debug(IGMP_DBG_SND, "group %d source %d on %d: LMQC=%d LMQI=%d dsec LMQT=%d msec\n",
				group->grp_addr, source->sip,pif->ifindex, lmqc, lmqi_dsec, lmqt_msec);

   	//igmp_source_timer_on(group, source, lmqt_msec);
   	igmp_source_timer_start(group, source, lmqt_msec);
}
   
void igmp_send_membership_query(struct igmp *igmp, struct igmp_group *group,
			uint32_t *sip, int num_srcs, uint32_t dip, uint32_t grpip, uint8_t s_flag)
{
	struct igmp_pkt_count *pcount = NULL;
	struct igmp_instance *pinst = NULL;
	struct igmpv3_query *query = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	char *pbuf = NULL;
	uint16_t	 checksum;
	uint8_t version;
	uint32_t src;
	char buf[12];
	int igmplen;
	int size;
	int vpn;
	int i;
	
	zassert(num_srcs >= 0);

	if (NULL == igmp)
	{
		return;
	}

	pif = igmp->pif;
	if (NULL == pif)
	{
		zlog_debug(IGMP_DBG_SND, "%s\n", "ipmc interface struct error");
		return;
	}

	pigmp = pif->igmp;
	if (NULL == pigmp)
	{
		return;
	}
	
	vpn = igmp_instance_find (pif->ifindex);
	if (vpn < 0)
	{
		zlog_debug(IGMP_DBG_SND, "vpn:%d not exist\n\r", vpn);
		return;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		zlog_debug(IGMP_DBG_SND, "%s\n", "vpn instance struct not exist");
		return;
	}
	
	version = pinst->version;
	if ((num_srcs != 0) && (version == IGMP_VERSION3))
	{
		pbuf = (char *)malloc(sizeof (struct igmpv3_query) + num_srcs*sizeof(uint32_t));
		if (NULL == pbuf)
		{
			return;
		}

		query = (struct igmpv3_query *)pbuf;
	}
	else
	{
		memset(buf, 0, 12);
		query = (struct igmpv3_query *)buf;
	}

	s_flag = IGMP_FORCE_BOOLEAN(s_flag);
	zassert((s_flag == 0) || (s_flag == 1));

	/*set the common fields*/
	query->type = IGMP_QUERY_MEMBER;
	query->group = htonl(grpip);
	query->chsum = 0;

	/*set version specific fields*/
	switch (version)
	{
		case IGMP_VERSION1:
			igmplen = 8;
			query->code = 0;
			break;
		case IGMP_VERSION2:
			igmplen = 8;
			query->code = igmp->max_resp_dsec;
			break;
		case IGMP_VERSION3:
			igmplen = 12;
			query->code 	= igmp_msg_encode16to8(igmp->max_resp_dsec);
			query->sflag 	= s_flag;
			query->qqi_code = igmp_msg_encode16to8(igmp->qqi);
			query->src_num  = htons(num_srcs);

			/*
			RFC 3376: 4.1.6. QRV (Querier's Robustness Variable)

			If non-zero, the QRV field contains the [Robustness Variable]
			value used by the querier, i.e., the sender of the Query.  If the
			querier's [Robustness Variable] exceeds 7, the maximum value of
			the QRV field, the QRV is set to zero.
			*/
			query->qrv = (igmp->qrv > 7) ? 0 : igmp->qrv;

			for (i = 0; i < num_srcs; i++)
			{
				query->sip[i] = htonl(sip[i]);
			}

			/*igmp query len*/
			igmplen = sizeof (struct igmpv3_query) + (num_srcs - 1)*sizeof(uint32_t);
			break;
		default:
			if (pbuf)
			{
				free(pbuf);
			}
			return;	
	}

	/*checksum*/
	query->chsum = in_checksum((uint16_t *)query, igmplen);
	
	/*igmp query len*/
	size = sizeof (struct igmpv3_query) + (num_srcs - 1)*sizeof(uint32_t);

	zlog_debug(IGMP_DBG_SND, "to %d on %d: group=%d sources=%d msg_size=%zd s_flag=%x QRV=%u QQI=%u QQIC=%02x checksum=%x\n",
		dip, pif->ifindex, grpip, num_srcs, size, s_flag, query->qrv, 
		query->qqi_code, query->qqi_code, checksum);

	/*send out the query*/
	src = ipmc_if_main_addr_get(pif->ifindex);
	igmp_ipmc_pkt_send(src, dip, pif->ifindex, (void *)query, size);

	if (igmp_pkt_dbg.pkt_snd)
	{
		zlog_debug (IGMP_DBG_SND, "%s\n", "IGMP Query Sent");
		igmp_packet_dump((void *)query, IGMP_QUERY_MEMBER, version);
	}
	
	if (pbuf)
	{
		free(pbuf);
	}

	/*
	s_flag sanity test: s_flag must be set for general queries

	RFC 3376: 6.6.1. Timer Updates

	When a router sends or receives a query with a clear Suppress
	Router-Side Processing flag, it must update its timers to reflect
	the correct timeout values for the group or sources being queried.

	General queries don't trigger timer update.
	*/
	if (!s_flag) 
	{
		/* general query? */
		if (0 == grpip) 
		{
			zlog_warn("%s: to %d on %d: group=%d sources=%d: s_flag is clear for general query!",
				__FUNCTION__, dip, pif->ifindex, grpip, num_srcs);
		}
	}

	pcount = &(pigmp->count);
	if (version == IGMP_VERSION3)
	{
		if (grpip)
		{
			if (num_srcs < 1)
			{
				pcount->gqry_v3_snd_pkts++;
			}
			else
			{
				pcount->gsqry_v3_snd_pkts++;
			}
		}
		else
		{
			pcount->qry_v3_snd_pkts++;
		}
	}
	else if (version == IGMP_VERSION1)
	{
		pcount->qry_v1_snd_pkts++;
	}
	else if (version == IGMP_VERSION2)
	{
		pcount->qry_v2_snd_pkts++;
	}
}
	

