/*
 * =====================================================================================
 *
 *       Filename:  igmp.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/28/2017 09:09:18 AM
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
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/ifm_common.h>
#include <lib/index.h>

#include "ipmc_if.h"
#include "ipmc_main.h"
#include "igmp_pkt.h"
#include "igmpv3.h"
#include "igmp_util.h"
#include "igmp_time.h"
#include "igmp_cmd.h"
#include "ipmc.h"
#include "igmp.h"

int igmp_debug = 1;

struct list *igmp_inst = NULL;

static struct igmp_group *find_group_by_addr(struct igmp *igmp, uint32_t grpip);
static void group_timer_stop(struct igmp_group *pgrp);
//static void group_timer_off(struct igmp_group *group);

static void igmp_instance_init(void)
{
	igmp_inst = list_new();
}

struct igmp_instance *igmp_instance_lookup(uint8_t id)
{
	struct igmp_instance *igmp = NULL;
	struct listnode *node = NULL;

	if (NULL == igmp_inst)
	{
		return NULL;
	}

	for (ALL_LIST_ELEMENTS_RO(igmp_inst, node, igmp))	
	{
	    if(igmp->id == id)
	    {
			return igmp;
	    }		
	}

	return NULL;
}

struct igmp_instance *igmp_instance_create(uint8_t id)
{
	struct igmp_instance *pinst = NULL;

	if (id > IGMP_INS_MAX)
	{
		return NULL;
	}
	
	pinst = XCALLOC (MTYPE_IPMC_IGMP_ENTRY, sizeof (struct igmp_instance));
	if (NULL == pinst)
	{
		return NULL;
	}

	pinst->id			 = id;
	
	pinst->mcif		 	 = list_new ();
	//pinst->grplist		 = list_new ();
	pinst->ssm_map		 = list_new();
	pinst->version 	 	 = IGMP_VERSION3;
	pinst->keepalive		 = IGMP_QUARY_DEFAULT_KEEPALIVE;
	pinst->qqi			 = IGMP_QUERY_DEFAULT_INTERVAL;
	pinst->robust_count  = IGMP_ROBUST_DEFAULT_COUNT;
	pinst->last_mem_qqi	 = IGMP_LAST_MEMBER_QUERY_DEFAULT_INTERVAL;
	pinst->max_resp		 = IGMP_RESPONSE_DEFAULT_TIME;
	pinst->fast_leave	 = 0;
	pinst->send_alert	 = 1;
	pinst->require_alert = 0;
	pinst->keepalive_set = 0;
	
	return pinst;
}

int igmp_instance_delete(uint8_t id)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_ssm_map *pmap = NULL;
	struct listnode *node = NULL;
	uint32_t *pifindex = NULL;
	
	if (id > IGMP_INS_MAX)
	{
		return ERRNO_PARAM_ILLEGAL;
	}
	
	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return ERRNO_NOT_FOUND;
	}
	
	/*delete all mcif*/
	if (NULL != pinst->mcif)
	{
		if (list_isempty(pinst->mcif))
		{	
			list_free(pinst->mcif);
			pinst->mcif = NULL;
		}	
		else
		{
			for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))
			{
				ipmc_if_delete(*pifindex);
				igmp_instance_mcif_del(id, *pifindex);

				if (NULL == pinst->mcif)
				{
					break;
				}
			}
		}
	}

	/*delete all ssm mapping*/
	if (NULL != pinst->ssm_map)
	{
		if (list_isempty(pinst->ssm_map))
		{	
			list_free(pinst->ssm_map);
			pinst->ssm_map = NULL;
		}	
		else
		{
			for (ALL_LIST_ELEMENTS_RO(pinst->ssm_map, node, pmap))
			{
				listnode_delete(pinst->ssm_map, pmap);
				XFREE(MTYPE_IPMC_IGMP_SSM_MAP, pmap);

				pmap = NULL;

				if (list_isempty(pinst->ssm_map))
				{	
					list_free(pinst->ssm_map);
					pinst->ssm_map = NULL;
					break;
				}		
			}
		}
	}
	
	/*delete igmp instance struct*/
	listnode_delete(igmp_inst, pinst);
	XFREE(MTYPE_IPMC_IGMP_ENTRY, pinst);
	
	return ERRNO_SUCCESS; 
}

int igmp_instance_mcif_lookup(uint8_t id, uint32_t ifindex)
{
	struct igmp_instance *pinst = NULL;
	struct listnode *node = NULL;
	uint32_t *pifindex;

	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return ERRNO_FAIL;
	}

	for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))	
	{
	    if(*pifindex == ifindex)
	    {
			return ERRNO_SUCCESS;
	    }		
	}

	return ERRNO_FAIL;
}

/*check and add interface to igmp instance*/
int igmp_instance_mcif_get(uint8_t id, uint32_t ifindex)
{
	struct igmp_instance *pinst = NULL;
	struct listnode *node = NULL;
	uint32_t *pifindex = NULL;
	int ret = 0;

	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return ERRNO_FAIL;
	}

	if (pinst->mcif == NULL)
	{
		pinst->mcif = list_new ();
	}
	
	ret = igmp_instance_mcif_lookup (id, ifindex);
	if (ret == ERRNO_SUCCESS)
	{
		return ERRNO_EXISTED;
	}

	pifindex = (uint32_t *)XMALLOC(MTYPE_IPMC_IF_ENTRY, sizeof(uint32_t));
	if(NULL == pifindex)
	{
		return ERRNO_FAIL;
	}
	
	*pifindex = ifindex;

	listnode_add (pinst->mcif, (void *)pifindex);	

	return ERRNO_SUCCESS;
}

/*delete mcif from igmp instance*/
void igmp_instance_mcif_del(uint8_t id, uint32_t ifindex)
{
	struct igmp_instance *pinst = NULL;
	struct listnode *node = NULL;
	struct listnode *nnode = NULL;
	uint32_t *pifindex = NULL;

	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return;
	}

	if (pinst->mcif == NULL)
	{
		return;
	}

	for ( ALL_LIST_ELEMENTS (pinst->mcif, node, nnode, pifindex))
	{
		if (*pifindex == ifindex)
		{	
			listnode_delete(pinst->mcif, pifindex);
			XFREE(MTYPE_IPMC_IF_ENTRY, pifindex);

			pifindex = NULL;

			if (list_isempty(pinst->mcif))
			{	
				list_free(pinst->mcif);
				pinst->mcif = NULL;
			}						
			break;
		}
	}
}	

/*find igmp instance by ifindex*/
int igmp_instance_find(uint32_t ifindex)
{
	struct igmp_instance *pinst = NULL;
	struct listnode	*node = NULL;
	uint32_t *pifindex = NULL;

	for (ALL_LIST_ELEMENTS_RO(igmp_inst, node, pinst))	
	{
	    for (ALL_LIST_ELEMENTS_RO(pinst->mcif, node, pifindex))
		{
		 	if (*pifindex == ifindex)
		 	{
		 		return pinst->id;
		 	}
		}		
	}

	return -1;
}

/*igmp ssm mapping info lookup*/
struct igmp_ssm_map *igmp_instance_ssm_map_lookup(
		struct igmp_instance *pinst, struct igmp_ssm_map *pmap)
{
	struct igmp_ssm_map *map = NULL;
	struct listnode *node = NULL;

	for (ALL_LIST_ELEMENTS_RO(pinst->ssm_map, node, map))	
	{
	    if ((pmap->grpip == map->grpip) 
			&& (pmap->prefix == map->prefix)
			&& (pmap->sip == map->sip))
	    {
			return map;
	    }		
	}

	return NULL;
}

/*add igmp ssm mapping information to igmp vpn instance*/
int igmp_instance_ssm_map_add(struct igmp_instance *pinst, 
		struct igmp_ssm_map *pmap)
{
	struct igmp_ssm_map *map = NULL;

	map = igmp_instance_ssm_map_lookup(pinst, pmap);
	if (NULL == map)
	{
		map = (struct igmp_ssm_map *)XMALLOC(MTYPE_IPMC_IGMP_SSM_MAP, sizeof(struct igmp_ssm_map));
		if (NULL == map)
		{
			return ERRNO_MALLOC;
		}

		memcpy(map, pmap, sizeof(struct igmp_ssm_map));
		
		listnode_add(pinst->ssm_map, map);
	}

	return ERRNO_SUCCESS;
}

/*delete igmp aam mapping information from igmp vpn instance*/
void igmp_instance_ssm_map_del(struct igmp_instance *pinst, 
		struct igmp_ssm_map *pmap)
{
	struct igmp_ssm_map *map = NULL;

	map = igmp_instance_ssm_map_lookup(pinst, pmap);
	if (NULL == map)
	{
		return;
	}

	listnode_delete(pinst->ssm_map, map);
	XFREE(MTYPE_IPMC_IGMP_SSM_MAP, map);
}

#if 0
/*find igmp group info from instance*/
struct igmp_join * igmp_instance_mip_lookup(uint8_t vpn, struct igmp_join *grp)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_join *pgrp = NULL;
	struct listnode	*node;

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return NULL;
	}

	for (ALL_LIST_ELEMENTS_RO(pinst->grplist, node, pgrp))
	{
	 	if (0 == memcmp(pgrp, grp, sizeof(struct igmp_join)))
	 	{
			return pgrp;
	 	}
	}

	return NULL;
}

/*add igmp group info to instance*/
int igmp_instance_mip_add(uint8_t id, struct igmp_join *grp)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_join *pgrp = NULL;

	pgrp = igmp_instance_mip_lookup(id, grp);
	if (NULL != pgrp)
	{
		return ERRNO_SUCCESS;
	}
	
	pgrp = (struct igmp_join *)XCALLOC(MTYPE_IPMC_IP_ENTRY, sizeof(struct igmp_join));
	if (NULL == pgrp)
	{
		return ERRNO_MALLOC;
	}

	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return ERRNO_FAIL;
	}

	if (pinst->grplist == NULL)
	{
		return ERRNO_FAIL;
	}
	
	memcpy(pgrp, grp, sizeof(struct igmp_join));

	listnode_add(pinst->grplist, (void *)pgrp);

	return ERRNO_SUCCESS;
}

/*delete igmp group info to instance*/
int igmp_instance_mip_delete(uint8_t id, struct igmp_join *grp)
{
	struct igmp_instance *pinst = NULL;
	struct igmp_join *pgrp = NULL;

	pgrp = igmp_instance_mip_lookup(id, grp);
	if (NULL == pgrp)
	{
		return ERRNO_SUCCESS;
	}

	pinst = igmp_instance_lookup(id);
	if (NULL == pinst)
	{
		return ERRNO_FAIL;
	}

	if (pinst->grplist == NULL)
	{
		return ERRNO_FAIL;
	}
	
	listnode_delete(pinst->grplist, (void *)grp);

	if (list_isempty(pinst->grplist))
	{	
		list_free(pinst->grplist);
		pinst->grplist = NULL;
	}

	return ERRNO_SUCCESS;
}
#endif

#if 0
static int igmp_other_querier_expire(struct thread *t)
{
	struct igmp *igmp;

	zassert(t);
	igmp = THREAD_ARG(t);
	zassert(igmp);

	zassert(igmp->other_querier_timer);
	zassert(!igmp->query_timer);

	IPMC_LOG_DEBUG();
	
  	igmp->other_querier_timer = NULL;

	/*
	We are the current querier, then
	re-start sending general queries.
	*/
  	igmp_general_query_on(igmp);

  	return ERRNO_SUCCESS;
}

void igmp_other_querier_timer_on(struct igmp *igmp)
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
  	long int oqqi_msec;
	int vpn;

  	zassert(igmp);

	pif = igmp->pif;
	if (NULL == pif)
	{
		return;
	}

	vpn = igmp_instance_find(pif->ifindex);
	if (vpn < 0)
	{
		return;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return;
	}

  	if (igmp->other_querier_timer) 
	{
	    /*
	      There is other querier present already,
	      then reset the other-querier-present timer.
	    */

		IPMC_LOG_DEBUG("Querier resetting TIMER event for Other-Querier-Present\n");

    	THREAD_OFF(igmp->other_querier_timer);
    	zassert(!igmp->other_querier_timer);
  	}
  	else 
	{
		/*
		  We are the current querier, then stop sending general queries:
		  igmp->t_igmp_query_timer = 0;
		*/
    	igmp_general_query_off(igmp);
		IPMC_LOG_DEBUG("%s:%d \n\r", __FUNCTION__, __LINE__);
  	}

	/*
	Since this socket is starting the other-querier-present timer,
	there should not be periodic query timer for this socket.
	*/
  	zassert(!igmp->query_timer);

	/*
	RFC 3376: 8.5. Other Querier Present Interval

	The Other Querier Present Interval is the length of time that must
	pass before a multicast router decides that there is no longer
	another multicast router which should be the querier.  This value
	MUST be ((the Robustness Variable) times (the Query Interval)) plus
	(one half of one Query Response Interval).

	other_querier_present_interval_msec = \
	  igmp->querier_robustness_variable * \
	  1000 * igmp->querier_query_interval + \
	  100 * (igmp->query_max_response_time_dsec >> 1);
	*/
	if (IGMP_FLAG_CONFIG_TEST(igmp->cfg_flag, IGMP_FLAG_CONFIG_OTHER_QUERIER_ALIVE))
	{
		oqqi_msec = 1000 * igmp->keepalive;
	}
	else
	{
		if (IGMP_FLAG_CONFIG_TEST(igmp->cfg_flag, IGMP_FLAG_CONFIG_ROBUST_CNT) ||
			IGMP_FLAG_CONFIG_TEST(igmp->cfg_flag, IGMP_FLAG_CONFIG_QUERY))
		{
			oqqi_msec = IGMP_OQPI_MSEC(igmp->qrv, igmp->qqi, igmp->max_resp_dsec);
			IPMC_LOG_DEBUG("%s :%d qrv:%d qqi:%d max_resp:%d\n\r",\
				__FUNCTION__, __LINE__, igmp->qrv, igmp->qqi, igmp->max_resp_dsec);
		}
		else
		{
			if (pinst->keepalive_set)
			{
				oqqi_msec = 1000 * pinst->keepalive;
			}
			else
			{
				oqqi_msec = IGMP_OQPI_MSEC(pinst->robust_count, pinst->qqi, pinst->max_resp);
			}
		}
	}

	IPMC_LOG_DEBUG("Querier scheduling %ld.%03ld sec TIMER event for Other-Querier-Present\n", 
			oqqi_msec / 1000, oqqi_msec % 1000);
		
  	THREAD_TIMER_MSEC_ON(ipmc_master, igmp->other_querier_timer,
		       igmp_other_querier_expire,
		       igmp, oqqi_msec);
}

void igmp_other_querier_timer_off(struct igmp *igmp)
{
  	zassert(igmp);

  	IPMC_LOG_DEBUG("igmp other querier timer off\n");
	
  	THREAD_OFF(igmp->other_querier_timer);
  	zassert(!igmp->other_querier_timer);
}
#endif

static int igmp_other_querier_timer_out(void *para)
{
	struct igmp *pigmp = NULL;

	pigmp = (struct igmp *)para;
	if (NULL == pigmp)
	{
		return ERRNO_FAIL;
	}

	pigmp->other_querier_timer = 0;
	
	/*
	We are the current querier, then
	re-start sending general queries.
	*/
	igmp_general_query_timer_start(pigmp);

	return ERRNO_SUCCESS;
}

void igmp_other_querier_timer_start(struct igmp *pigmp)
{
	char timerName[20] = "OtherqueryTimer";
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
  	long int oqqi_msec;
	int vpn;

  	if (NULL == pigmp)
	{
		return;
	}

	pif = pigmp->pif;
	if (NULL == pif)
	{
		return;
	}

	vpn = igmp_instance_find(pif->ifindex);
	if (vpn < 0)
	{
		return;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return;
	}

  	if (pigmp->other_querier_timer) 
	{
	    /*
	      There is other querier present already,
	      then reset the other-querier-present timer.
	    */

    	igmp_other_querier_timer_stop(pigmp);
  	}
  	else 
	{
		/*
		  We are the current querier, then stop sending general queries:
		  igmp->t_igmp_query_timer = 0;
		*/
    	igmp_general_query_timer_stop(pigmp);
  	}

	/*
	Since this socket is starting the other-querier-present timer,
	there should not be periodic query timer for this socket.
	*/
  	zassert(!pigmp->query_timer);

	/*
	RFC 3376: 8.5. Other Querier Present Interval

	The Other Querier Present Interval is the length of time that must
	pass before a multicast router decides that there is no longer
	another multicast router which should be the querier.  This value
	MUST be ((the Robustness Variable) times (the Query Interval)) plus
	(one half of one Query Response Interval).

	other_querier_present_interval_msec = \
	  igmp->querier_robustness_variable * \
	  1000 * igmp->querier_query_interval + \
	  100 * (igmp->query_max_response_time_dsec >> 1);
	*/
	if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_OTHER_QUERIER_ALIVE))
	{
		oqqi_msec = 1000 * pigmp->keepalive;
	}
	else
	{
		if (IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_ROBUST_CNT) ||
			IGMP_FLAG_CONFIG_TEST(pigmp->cfg_flag, IGMP_FLAG_CONFIG_QUERY))
		{
			oqqi_msec = IGMP_OQPI_MSEC(pigmp->qrv, pigmp->qqi, pigmp->max_resp_dsec);
			
			zlog_debug(IGMP_DBG_SND, "%s :%d qrv:%d qqi:%d max_resp:%d\n\r",\
				__FUNCTION__, __LINE__, pigmp->qrv, pigmp->qqi, pigmp->max_resp_dsec);
		}
		else
		{
			if (pinst->keepalive_set)
			{
				oqqi_msec = 1000 * pinst->keepalive;
			}
			else
			{
				oqqi_msec = IGMP_OQPI_MSEC(pinst->robust_count, pinst->qqi, pinst->max_resp);
			}
		}
	}

	zlog_debug(IGMP_DBG_OTHER, "Querier scheduling %ld.%03ld sec TIMER event for Other-Querier-Present\n", 
			oqqi_msec / 1000, oqqi_msec % 1000);
	
	pigmp->other_querier_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, 
				igmp_other_querier_timer_out, (void *)pigmp, oqqi_msec);
}

void igmp_other_querier_timer_stop(struct igmp *pigmp)
{
  	if (NULL == pigmp)
	{
		return;
	}

	if (pigmp->other_querier_timer)
	{
		high_pre_timer_delete(pigmp->other_querier_timer);
		pigmp->other_querier_timer = 0;
	}
}


int recv_igmp_query(struct igmpv3_query *query, uint8_t query_version, struct ip_control *pipcb)
{
	struct igmp_pkt_count *pcount = NULL;
	struct igmp_instance *pinst = NULL;
	struct igmp_source *psrc = NULL;
	struct igmp_group *pgrp = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *psip = NULL;
	uint8_t rsv_s_qrv = 0;
	int mip_prefix = 0;
	uint8_t s_flag = 0;
	uint32_t ifindex;
	uint32_t mip = 0;
	uint32_t sip = 0;  /*source ip*/
	uint16_t vpn = 0;
	uint16_t src_num;
	uint8_t qrv = 0;
	uint32_t grpip;
	uint8_t  qqic;
	uint16_t qqi;
	int i;
	
	ifindex = pipcb->ifindex;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		zlog_err("Cannot find mcif struct\n");
		return ERRNO_FAIL;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		zlog_err("IGMP function does not enable\n");
		return ERRNO_FAIL;
	}

	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		zlog_err("IGMP struct on interface:%d is error\n", ifindex);
		return ERRNO_FAIL;
	}

	//vpn = pipcb->vpn;
	vpn = igmp_instance_find(ifindex);
	if (vpn > IGMP_INS_MAX)
	{
		zlog_err("IGMP vpn is not exist\n");
		return ERRNO_FAIL;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		zlog_err("IGMP vpn:%d struct is not exist\n", vpn);
		return ERRNO_FAIL;
	}

	pcount = &(pigmp->count);
	sip = pipcb->sip;
	if (IGMP_FLAG_ENABLE == pmcif->igmp_flag)
	{
		mip = ipmc_if_main_addr_get(ifindex);
		if (mip == pipcb->sip)
		{
			zlog_err("IGMP cannot process pkt comes from itself\n");
			return ERRNO_SUCCESS;
		}

		/*
		RFC 3376: 6.6.2. Querier Election

		When a router receives a query with a lower IP address, it sets
		the Other-Querier-Present timer to Other Querier Present Interval
		and ceases to send queries on the network if it was the previously
		elected querier.
		*/

		mip_prefix = ipmc_if_main_addr_mask_get(ifindex);
		if (ipv4_is_same_subnet(mip, sip, mip_prefix) && (sip < mip))
		{
	    	//igmp_other_querier_timer_on(pigmp);
			igmp_other_querier_timer_start(pigmp);
			
			zlog_debug(IGMP_DBG_RCV, "%s:%d IGMP rcv pkt ip smaller, start other querier timer\n", __FUNCTION__, __LINE__);
	  	}

		if (query_version == 3) 
		{
			/*
			  RFC 3376: 4.1.6. QRV (Querier's Robustness Variable)

			  Routers adopt the QRV value from the most recently received Query
			  as their own [Robustness Variable] value, unless that most
			  recently received QRV was zero, in which case the receivers use
			  the default [Robustness Variable] value specified in section 8.1
			  or a statically configured value.
			*/
		    rsv_s_qrv = query->qrv;
		    qrv = 7 & rsv_s_qrv;
			pigmp->qrv = qrv ? qrv : pinst->robust_count;
	  	}

		/*
		RFC 3376: 4.1.7. QQIC (Querier's Query Interval Code)

		Multicast routers that are not the current querier adopt the QQI
		value from the most recently received Query as their own [Query
		Interval] value, unless that most recently received QQI was zero,
		in which case the receiving routers use the default.
		*/	
  		if (pigmp->other_querier_timer && query_version == 3) 
		{
			/* other querier present */
			qqic = query->qqi_code;
			qqi = igmp_msg_decode8to16(qqic);
			pigmp->qqi = qqi ? qqi : pinst->qqi;

			zlog_debug(IGMP_DBG_RCV, "Querier new query interval is QQI=%u sec (default:%d)\n",
				qqi, pinst->qqi);
	  	}

		/*
		RFC 3376: 6.6.1. Timer Updates

		When a router sends or receives a query with a clear Suppress
		Router-Side Processing flag, it must update its timers to reflect
		the correct timeout values for the group or sources being queried.

		General queries don't trigger timer update.
		*/
  		if (query_version == 3) 
		{
	    	s_flag = (1 << 3) & rsv_s_qrv;
	  	}
  		else 
		{
		    /* Neither V1 nor V2 have this field. Pimd should really go into
		     * a compatibility mode here and run as V2 (or V1) but it doesn't
		     * so for now, lets just set the flag to suppress these timer updates.
		     */
	    	s_flag = 1;
	  	}
  
  		if (!s_flag) 
		{
			grpip = ntohl(query->group);
	    	/* s_flag is clear */
	    	if (0 == grpip) 
			{
	      		/* this is a general query */

	      		/* log that general query should have the s_flag set */
	      		zlog_debug(IGMP_DBG_RCV, "General IGMP query v%d: Suppress Router-Side Processing flag is clear",
					query_version);

				pcount->qry_v3_rsv_pkts++;
	    	}
	    	else 
			{
	      		/* this is a non-general query: perform timer updates */
	      		pgrp = find_group_by_addr(pigmp, grpip);
	      		if (pgrp) 
				{
					/*
					  RFC 3376: 6.6.1. Timer Updates
					  Query Q(G,A): Source Timer for sources in A are lowered to LMQT
					  Query Q(G): Group Timer is lowered to LMQT
					*/
					src_num = ntohs(query->src_num);
					if (src_num < 1) 
					{
		  				/* Query Q(G): Group Timer is lowered to LMQT */
		  				igmp_group_timer_lower_to_lmqt(pgrp);

						pcount->gqry_v3_rsv_pkts++;
					}
					else 
					{
		  				/* Query Q(G,A): Source Timer for sources in A are lowered to LMQT */

		 		 		/* Scan sources in query and lower their timers to LMQT */
		  				psip = query->sip;
		  				for (i = 0; i < src_num; ++i) 
						{
							sip = ntohl(psip[i]);
						    psrc = igmp_find_source_by_addr(pgrp, sip);
						    if (psrc) 
							{
						      	igmp_source_timer_lower_to_lmqt(psrc);
						    }
		  				}

						pcount->gsqry_v3_rsv_pkts++;
					}
	      		}
	      		else 
				{
					pcount->invalid_pkts++;
					zlog_debug(IGMP_DBG_RCV, "IGMP query v%d on %d: could not find group %d for timer update\n",
						query_version, ifindex, ntohl(query->group));
					return ERRNO_FAIL; 
	      		}
	    	}
	  	} /* s_flag is clear: timer updates */

		if (query_version == 2)
		{
			pcount->qry_v2_rsv_pkts++;
		}
		else if (query_version == 1)
		{
			pcount->qry_v1_rsv_pkts++;
		}
	}
  
	return ERRNO_SUCCESS;
}

int igmp_v3_report(struct igmpv3_report *report, struct ip_control *pipcb)
{
	struct igmp_pkt_count *pcount = NULL;
	struct igmppv3_grec	*precd = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t *psrc = NULL;
    int rec_num_sources;
	int rec_auxdatalen;
	uint32_t rec_grp;
	uint32_t ifindex;
	int num_groups;
    int rec_type;
    int i;
	char *p;

	ifindex = pipcb->ifindex;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		zlog_err("Cannot find mcif struct\n");
		return ERRNO_FAIL;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		zlog_err("IGMP function does not enable\n");
		return ERRNO_FAIL;
	}

	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		zlog_err("IGMP struct on interface:%d is error\n", ifindex);
		return ERRNO_FAIL;
	}

  	num_groups = ntohs(report->grec_num);
  	if (num_groups < 1) 
	{
    	IPMC_LOG_ERROR("Recv IGMP report v3 from %d on %d: missing group records\n",
	      	pipcb->sip, pipcb->ifindex);
		
    	return ERRNO_FAIL;
  	}

  	precd = report->grec;
	if (NULL == precd)
	{
		IPMC_LOG_ERROR("Recv IGMP report v3 group record is error\n");
		
		return ERRNO_FAIL;
	}

	p = (char *)report->grec;
	/*for (i=0; i<32; i=i+4)
	{
		printf("data:%02x %02x %02x %02x\n", p[i], p[i+1], p[i+2], p[i+3]);
	}*/

	pcount = &(pigmp->count);
  	/* Scan groups */
  	for (i = 0; i < num_groups; i++) 
	{
		rec_type 		= precd->type;
    	rec_auxdatalen  = precd->aux_data;
    	rec_num_sources = ntohs(precd->src_num);
		rec_grp 		= ntohl(precd->group);

      	zlog_debug(IGMP_DBG_RCV, "Recv IGMP report v3 from %d on %d: record=%d type=%d auxdatalen=%d sources=%d group=%d\n",
		 		pipcb->sip, pipcb->ifindex, i, rec_type, rec_auxdatalen, rec_num_sources, rec_grp);

    	/* Scan sources */
		psrc = precd->sip;
    	
	    switch (rec_type) 
		{
	    	case IGMP_MODE_IS_IN:
	      		igmpv3_report_isin(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_isin_pkts++;
	      		break;
	    	case IGMP_MODE_IS_EXL:
	      		igmpv3_report_isex(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_isexl_pkts++;
	      		break;
	    	case IGMP_TO_IN_MODE:
				igmpv3_report_toin(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_toin_pkts++;
				break;
	    	case IGMP_TO_EXL_MODE:
	      		igmpv3_report_toex(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_toexl_pkts++;
	      		break;
	    	case IGMP_ALLOW_NEW_SRCS:
	      		igmpv3_report_allow(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_alw_pkts++;
	      		break;
	    	case IGMP_BLOCK_OLD_SRCS:
	      		igmpv3_report_block(pigmp, pipcb->sip, rec_grp, rec_num_sources, psrc);
				pcount->rpt_blk_pkts++;
	      		break;
	    	default:
				pcount->invalid_pkts++;
	      		zlog_warn("Recv IGMP report v3 from %d on %d: unknown record type: type=%d",
					pipcb->sip, pipcb->ifindex, rec_type);
				return ERRNO_FAIL;
	    }

		precd = (struct igmppv3_grec	 *)(psrc + rec_num_sources);
  	} /* for (group records) */

	pcount->rpt_v3_pkts++;
		
	return ERRNO_SUCCESS;
}

int igmp_v2_report(struct igmpv12_report *report, struct ip_control *pipcb)
{
	struct igmp_pkt_count *pcount = NULL;
	struct igmp_group *group = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t group_addr;
	uint32_t ifindex;

	zlog_debug(IGMP_DBG_RCV, "%s:%d  sip:%d\n\r", __FUNCTION__, __LINE__, pipcb->sip);

	ifindex = pipcb->ifindex;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		zlog_err("Cannot find mcif struct\n");
		return ERRNO_FAIL;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		zlog_err("IGMP function does not enable\n");
		return ERRNO_FAIL;
	}

	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		zlog_err("IGMP struct on interface:%d is error\n", ifindex);
		return ERRNO_FAIL;
	}

  	group_addr = report->group;

  	/* non-existant group is created as INCLUDE {empty} */
  	group = igmp_add_group_by_addr(pigmp, group_addr);
  	if (NULL == group) 
	{
    	return ERRNO_FAIL;
  	}

  	group->last_v2_rpt_dsec = igmp_time_monotonic_dsec();

	pcount = &(pigmp->count);
	pcount->rpt_v2_pkts++;

	return ERRNO_SUCCESS;
}

int igmp_v1_report(struct igmpv12_report *report, struct ip_control *pipcb)
{
	struct igmp_pkt_count *pcount = NULL;
	struct igmp_group *group = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t group_addr;
	uint32_t ifindex;

  	zlog_debug(IGMP_DBG_RCV, "%s:%d	sip:%d\n\r", __FUNCTION__, __LINE__, pipcb->sip);
  	
	ifindex = pipcb->ifindex;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		zlog_err("Cannot find mcif struct\n");
		return ERRNO_FAIL;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		zlog_err("IGMP function does not enable\n");
		return ERRNO_FAIL;
	}

	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		zlog_err("IGMP struct on interface:%d is error\n", ifindex);
		return ERRNO_FAIL;
	}

	group_addr = report->group;

  	/* non-existant group is created as INCLUDE {empty} */
  	group = igmp_add_group_by_addr(pigmp, group_addr);
  	if (NULL == group) 
	{
    	return ERRNO_FAIL;
  	}

  	group->last_v1_rpt_dsec = igmp_time_monotonic_dsec();

	pcount = &(pigmp->count);
	pcount->rpt_v1_pkts++;

	return ERRNO_SUCCESS;
}

int igmp_v2_leave(struct igmpv12_report *leave, struct ip_control *pipcb)
{
	struct igmp_pkt_count *pcount = NULL;
	struct ipmc_if *pmcif = NULL;
	struct igmp *pigmp = NULL;
	uint32_t ifindex;

	zlog_debug(IGMP_DBG_RCV, "%s\n", "receive v2 leave pkt");
		
	ifindex = pipcb->ifindex;
	pmcif = ipmc_if_lookup(ifindex);
	if (NULL == pmcif)
	{
		zlog_err("Cannot find mcif struct\n");
		return ERRNO_FAIL;
	}

	if (IGMP_FLAG_DISABLE == pmcif->igmp_flag)
	{
		zlog_err("IGMP function does not enable\n");
		return ERRNO_FAIL;
	}

	pigmp = pmcif->igmp;
	if (NULL == pigmp)
	{
		zlog_err("IGMP struct on interface:%d is error\n", ifindex);
		return ERRNO_FAIL;
	}

	pcount = &(pigmp->count);
	pcount->lev_v2_pkts++;
	
	return ERRNO_SUCCESS;
}

#if 0
static int igmp_general_query(struct thread *t);

void igmp_general_query_on(struct igmp *igmp)
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	int query_interval;
	int startup_mode;
	int vpn;

	zassert(igmp);

	pif = igmp->pif;
	if (NULL == pif)
	{
		return;
	}

	vpn = igmp_instance_find(pif->ifindex);
	if (vpn < 0)
	{
		return;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return;
	}
	
	/*
	Since this socket is starting as querier,
	there should not exist a timer for other-querier-present.
	*/
  	zassert(!igmp->other_querier_timer);

	/*
	RFC 3376: 8.6. Startup Query Interval

	The Startup Query Interval is the interval between General Queries
	sent by a Querier on startup.  Default: 1/4 the Query Interval.
	*/
  	startup_mode = igmp->start_qry_cnt > 0;
  	if (startup_mode) 
	{
    	--igmp->start_qry_cnt;
		THREAD_OFF(igmp->query_timer);
    	query_interval = IGMP_SQI(igmp->qqi);
  	}
  	else 
	{
		igmp_general_query_off(igmp);
    	query_interval = igmp->qqi;
  	}

	IPMC_LOG_DEBUG("Querier scheduling %d-second (%s) TIMER event for IGMP query\n",
			query_interval, startup_mode ? "startup" : "non-startup");
	
  	THREAD_TIMER_ON(ipmc_master, igmp->query_timer,
		  igmp_general_query, igmp, query_interval);
}

void igmp_general_query_off(struct igmp *igmp)
{
  	zassert(igmp);

	IPMC_LOG_DEBUG("IGMP querier cancelling query TIMER event\n");
	
  	THREAD_OFF(igmp->query_timer);
  	zassert(!igmp->query_timer);
}

/* Issue IGMP general query */
static int igmp_general_query(struct thread *t)
{
	struct igmp *igmp = NULL;
	uint32_t grpip;
	uint32_t dip;

  	zassert(t);

  	igmp = THREAD_ARG(t);

  	zassert(igmp);

	/*
	RFC3376: 4.1.12. IP Destination Addresses for Queries

	In IGMPv3, General Queries are sent with an IP destination address
	of 224.0.0.1, the all-systems multicast address.  Group-Specific
	and Group-and-Source-Specific Queries are sent with an IP
	destination address equal to the multicast address of interest.
	*/

  	dip = IGMP_GENERAL_QUERY_IPADDRESS;
  	grpip = 0;

	IPMC_LOG_DEBUG("Querier issuing IGMP general query\n");

  	igmp_send_membership_query(igmp, 0 /* igmp_group */,NULL, /*src ip*/
				 0 /* num_sources */, dip, grpip, igmp->send_alert/* s_flag: default set for general queries */);

  	igmp_general_query_on(igmp);

  	return ERRNO_SUCCESS;
}
#endif

static int  igmp_general_query_timer_out(void *para)
{
	struct igmp *pigmp = NULL;
	uint32_t grpip;
	uint32_t dip;

	pigmp = (struct igmp *)para;
	if (NULL == pigmp)
	{
		return ERRNO_FAIL;
	}

	/*
	RFC3376: 4.1.12. IP Destination Addresses for Queries

	In IGMPv3, General Queries are sent with an IP destination address
	of 224.0.0.1, the all-systems multicast address.  Group-Specific
	and Group-and-Source-Specific Queries are sent with an IP
	destination address equal to the multicast address of interest.
	*/

  	dip = IGMP_GENERAL_QUERY_IPADDRESS;
  	grpip = 0;

	zlog_debug(IGMP_DBG_SND, "%s\n", "Querier issuing IGMP general query");

  	igmp_send_membership_query(pigmp, 0 /* igmp_group */,NULL, /*src ip*/
				 0 /* num_sources */, dip, grpip, pigmp->send_alert/* s_flag: default set for general queries */);

  	return ERRNO_SUCCESS;
}

int igmp_general_query_timer_start(void *para)
{
	char timerName1[20] = "StartGenqueryTimer";
	char timerName2[20] = "GenqueryTimer";
	struct igmp *pigmp = NULL;
	uint32_t grpip;
	uint32_t dip;

	pigmp = (struct igmp *)para;
	if (NULL == pigmp)
	{
		return ERRNO_FAIL;
	}

	if (pigmp->start_qry_cnt)
	{
		/*
		RFC3376: 4.1.12. IP Destination Addresses for Queries

		In IGMPv3, General Queries are sent with an IP destination address
		of 224.0.0.1, the all-systems multicast address.  Group-Specific
		and Group-and-Source-Specific Queries are sent with an IP
		destination address equal to the multicast address of interest.
		*/

	  	dip = IGMP_GENERAL_QUERY_IPADDRESS;
	  	grpip = 0;

	  	igmp_send_membership_query(pigmp, 0 /* igmp_group */,NULL, /*src ip*/
					 0 /* num_sources */, dip, grpip, pigmp->send_alert/* s_flag: default set for general queries */);

		pigmp->start_qry_cnt--;

		/*
		RFC 3376: 8.6. Startup Query Interval

		The Startup Query Interval is the interval between General Queries
		sent by a Querier on startup.  Default: 1/4 the Query Interval.
		*/
		pigmp->query_timer = high_pre_timer_add(timerName1, LIB_TIMER_TYPE_NOLOOP, 
				igmp_general_query_timer_start, (void *)pigmp, 1000*IGMP_SQI(pigmp->qqi));
	}
	else
	{
		pigmp->query_timer = high_pre_timer_add(timerName2, LIB_TIMER_TYPE_LOOP, 
				igmp_general_query_timer_out, (void *)pigmp, pigmp->qqi);
	}

  	return ERRNO_SUCCESS;
}

void igmp_general_query_timer_stop(struct igmp *pigmp)
{
  	if (NULL == pigmp)
	{
		return;
	}

	if (pigmp->query_timer)
	{
		high_pre_timer_delete(pigmp->query_timer);
		pigmp->query_timer = 0;
	}
}

void igmp_startup_mode_on(struct igmp *igmp)
{
	struct igmp_instance *pinst = NULL;
	struct ipmc_if *pif = NULL;
	int vpn;
	
	pif = igmp->pif;
	if (NULL == pif)
	{
		return;
	}

	vpn = igmp_instance_find(pif->ifindex);
	if (vpn < 0)
	{
		return;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return;
	}
	/*
	RFC 3376: 8.7. Startup Query Count

	The Startup Query Count is the number of Queries sent out on
	startup, separated by the Startup Query Interval.  Default: the
	Robustness Variable.
	*/
	if (!IGMP_FLAG_CONFIG_TEST(igmp->cfg_flag, IGMP_FLAG_CONFIG_ROBUST_CNT))
	{
  		igmp->start_qry_cnt = pinst->robust_count;
	}

	/*
	Since we're (re)starting, reset QQI to default Query Interval
	*/
	if (!IGMP_FLAG_CONFIG_TEST(igmp->cfg_flag, IGMP_FLAG_CONFIG_QUERY))
	{
		igmp->qqi = pinst->qqi;
	}


	zlog_debug(IGMP_DBG_OTHER, "%s:%d start_qry_cnt:%d qqi:%d\n\r", \
		__FUNCTION__, __LINE__, igmp->start_qry_cnt, igmp->qqi);
}

static void igmp_group_free(struct igmp_group *group)
{
	zassert(!group->grp_qrt_timer);
	zassert(!group->grp_timer);
	zassert(group->grp_src_list);
	zassert(!listcount(group->grp_src_list));

	list_free(group->grp_src_list);

	XFREE(MTYPE_IPMC_GROUP, group);
}

static void igmp_group_delete(struct igmp_group *pgrp)
{
	struct listnode *src_node;
	struct listnode *src_nextnode;
	struct igmp_source *src;

	zlog_debug(IGMP_DBG_OTHER, "Deleting IGMP group %d from interface %d\n",
	       pgrp->grp_addr, pgrp->grp_igmp->pif->ifindex);

  	for (ALL_LIST_ELEMENTS(pgrp->grp_src_list, src_node, src_nextnode, src)) 
	{
    	igmp_source_delete(src);
  	}

	group_retransmit_timer_stop(pgrp);
	
  	//group_timer_off(group);
  	group_timer_stop(pgrp);
	pgrp->grp_timer = NULL;
	
  	listnode_delete(pgrp->grp_igmp->group_list, (void *)pgrp);
  	igmp_group_free(pgrp);
}

void igmp_group_delete_empty_include(struct igmp_group *group)
{
  	zassert(!group->grp_filtermode);
  	zassert(!listcount(group->grp_src_list));

  	igmp_group_delete(group);
}

struct igmp *igmp_new(struct ipmc_if *pif)
{
	struct igmp_instance *pinst = NULL;
	struct igmp *igmp = NULL;
	int vpn;

	vpn = igmp_instance_find(pif->ifindex);
	if (vpn < 0)
	{
		return NULL;
	}

	pinst = igmp_instance_lookup(vpn);
	if (NULL == pinst)
	{
		return NULL;
	}

  	igmp = XMALLOC(MTYPE_IPMC_IGMP_ENTRY, sizeof(struct igmp));
  	if (NULL == igmp) 
	{
    	zlog_warn("%s %s: XMALLOC() failure",	__FILE__, __FUNCTION__);
    	return NULL;
  	}

	igmp->group_list 	  = list_new ();
  	igmp->group_list->del = (void (*)(void *)) igmp_group_free;
	igmp->join_list 	  = list_new ();
	
	igmp->pif				   = pif;
	igmp->query_timer          = 0;
	igmp->other_querier_timer  = 0; /* no other querier present */
	igmp->ssm_map_enable	   = 0;
	igmp->cfg_flag			   = 0;
	igmp->version			   = pinst->version;
	igmp->req_alert			   = pinst->require_alert;
	igmp->send_alert		   = pinst->send_alert;
	igmp->max_resp_dsec        = pinst->max_resp;
	igmp->qrv 				   = pinst->robust_count;
	igmp->spec_max_resp_dsec   = pinst->last_mem_qqi;
	igmp->keepalive            = pinst->keepalive;
		
	return igmp;
}

static void igmp_free(struct igmp *igmp)
{
	zassert(!igmp->query_timer);
	zassert(!igmp->other_querier_timer);
	zassert(!listcount(igmp->group_list));

	XFREE(MTYPE_IPMC_IGMP_ENTRY, igmp);
}

void igmp_delete(struct igmp *igmp)
{
	struct listnode *nextnode = NULL;
	struct listnode *node = NULL;
	struct igmp_join *pjoin = NULL;
	struct igmp_group *grp = NULL;

	if (igmp->query_timer)
	{
		//igmp_general_query_off(igmp);
		igmp_general_query_timer_stop(igmp);
	}
	else
	{
		//igmp_other_querier_timer_off(igmp);
		igmp_other_querier_timer_stop(igmp);
	}
	
  	for (ALL_LIST_ELEMENTS(igmp->group_list, node, nextnode, grp))
	{
    	igmp_group_delete(grp);
  	}
	
	for (ALL_LIST_ELEMENTS(igmp->join_list, node, nextnode, pjoin))
	{
    	igmp_join_delete(igmp, pjoin);
  	}
	if(listcount(igmp->join_list) < 1)
	{
		list_delete(igmp->join_list);
		igmp->join_list = NULL;
	}
	igmp->pif = NULL;
	

  	igmp_free(igmp);
}



/*
  RFC 3376: 6.5. Switching Router Filter-Modes

  When a router's filter-mode for a group is EXCLUDE and the group
  timer expires, the router filter-mode for the group transitions to
  INCLUDE.

  A router uses source records with running source timers as its state
  for the switch to a filter-mode of INCLUDE.  If there are any source
  records with source timers greater than zero (i.e., requested to be
  forwarded), a router switches to filter-mode of INCLUDE using those
  source records.  Source records whose timers are zero (from the
  previous EXCLUDE mode) are deleted.
 */
 #if 0
static int igmp_group_timer(struct thread *t)
{
  	struct igmp_group *group = NULL;

  	zassert(t);
  	group = THREAD_ARG(t);
  	zassert(group);

	IPMC_LOG_DEBUG("Timer for group %d\n", group->grp_addr);

  	zassert(group->grp_filtermode);

  	group->grp_timer = NULL;
  	group->grp_filtermode = 0;

  	/* Any source (*,G) is forwarded only if mode is EXCLUDE {empty} */
  	igmp_anysource_forward_stop(group);

  	igmp_source_delete_expired(group->grp_src_list);

  	zassert(!group->grp_timer);
  	zassert(!group->grp_filtermode);

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	If there are no more source records for the group, delete group
	record.
	*/
  	if (listcount(group->grp_src_list) < 1)
	{
    	igmp_group_delete_empty_include(group);
  	}

  	return ERRNO_SUCCESS;
}

static void group_timer_off(struct igmp_group *group)
{
	if (NULL == group->grp_timer)
	{
    	return;
	}

	IPMC_LOG_DEBUG("Cancelling TIMER event for group %d\n",
			group->grp_addr);
    
  	THREAD_OFF(group->grp_timer);
  	zassert(!group->grp_timer);
}

void igmp_group_timer_on(struct igmp_group *group, long interval_msec)
{
  	group_timer_off(group);

	IPMC_LOG_DEBUG("Scheduling %ld.%03ld sec TIMER event for group %d\n",
			interval_msec / 1000, interval_msec % 1000, group->grp_addr);

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and
	it represents the time for the *filter-mode* of the group to
	expire and switch to INCLUDE mode.
	*/
  	zassert(group->grp_filtermode);

  	THREAD_TIMER_MSEC_ON(ipmc_master, group->grp_timer,
		       igmp_group_timer, group, interval_msec);
}
#endif

static int  igmp_group_timer_out(void *para)
{
	struct igmp_group *pgrp = NULL;

	pgrp = (struct igmp_group *)para;
  	if (NULL == pgrp)
	{
		return ERRNO_FAIL;
	}

	zlog_debug(IGMP_DBG_OTHER, "Timer for group %d\n", pgrp->grp_addr);

  	zassert(pgrp->grp_filtermode);

  	//group->grp_timer = NULL;
	pgrp->grp_timer = 0;
  	pgrp->grp_filtermode = 0;

  	/* Any source (*,G) is forwarded only if mode is EXCLUDE {empty} */
  	igmp_anysource_forward_stop(pgrp);

  	igmp_source_delete_expired(pgrp->grp_src_list);

  	zassert(!pgrp->grp_timer);
  	zassert(!pgrp->grp_filtermode);

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	If there are no more source records for the group, delete group
	record.
	*/
  	if (listcount(pgrp->grp_src_list) < 1)
	{
    	igmp_group_delete_empty_include(pgrp);
  	}

  	return ERRNO_SUCCESS;
}

static void group_timer_stop(struct igmp_group *pgrp)
{
	if (NULL == pgrp || 0 == pgrp->grp_timer)
	{
    	return;
	}

	zlog_debug(IGMP_DBG_OTHER, "Cancelling TIMER event for group %d\n", pgrp->grp_addr);

	if (pgrp->grp_timer)
	{
		high_pre_timer_delete(pgrp->grp_timer);
		pgrp->grp_timer = 0;
	}
}

void igmp_group_timer_start(struct igmp_group *pgrp, long interval_msec)
{
	char timerName[20] = "GroupTimer";
  	group_timer_stop(pgrp);

	zlog_debug(IGMP_DBG_OTHER, "Scheduling %ld.%03ld sec TIMER event for group %d\n",
			interval_msec / 1000, interval_msec % 1000, pgrp->grp_addr);

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and
	it represents the time for the *filter-mode* of the group to
	expire and switch to INCLUDE mode.
	*/
  	zassert(pgrp->grp_filtermode);

	pgrp->grp_timer = high_pre_timer_add(timerName, LIB_TIMER_TYPE_NOLOOP, 
					igmp_group_timer_out, (void *)pgrp, interval_msec);
}


static struct igmp_group *find_group_by_addr(struct igmp *igmp, uint32_t grpip)
{
	struct igmp_group *group = NULL;
	struct listnode *node = NULL;

  	for (ALL_LIST_ELEMENTS_RO(igmp->group_list, node, group))
  	{
	    if (grpip == group->grp_addr)
	    {
	     	return group;
	    }
  	}

  	return NULL;
}

struct igmp_group *igmp_add_group_by_addr(struct igmp *igmp, uint32_t grpip)
{
	struct igmp_group *group = NULL;

  	group = find_group_by_addr(igmp, grpip);
  	if (group) 
	{
    	return group;
  	}

	/*
	Non-existant group is created as INCLUDE {empty}:

	RFC 3376 - 5.1. Action on Change of Interface State

	If no interface state existed for that multicast address before
	the change (i.e., the change consisted of creating a new
	per-interface record), or if no state exists after the change
	(i.e., the change consisted of deleting a per-interface record),
	then the "non-existent" state is considered to have a filter mode
	of INCLUDE and an empty source list.
	*/

  	group = XMALLOC(MTYPE_IPMC_GROUP, sizeof(*group));
  	if (!group) 
	{
    	zlog_warn("%s %s: XMALLOC() failure",
	      	__FILE__, __FUNCTION__);
    	return NULL; /* error, not found, could not create */
  	}

  	group->grp_src_list = list_new();
  	if (!group->grp_src_list) 
	{
    	zlog_warn("%s %s: list_new() failure",
	      	__FILE__, __PRETTY_FUNCTION__);
    	XFREE(MTYPE_IPMC_GROUP, group); /* discard group */
    	return NULL; /* error, not found, could not initialize */
  	}
	
  	group->grp_src_list->del = (void (*)(void *)) igmp_source_free;

  	group->grp_timer		= 0;
  	group->grp_qrt_timer 	= 0;
  	group->grp_sqrt_count 	= 0;
  	group->grp_addr 		= grpip;
  	group->grp_igmp 		= igmp;
  	group->last_v1_rpt_dsec	= -1;
  	group->last_v2_rpt_dsec	= -1;
  	group->grp_creat  		= igmp_time_monotonic_sec();

  	/* initialize new group as INCLUDE {empty} */
  	group->grp_filtermode 	= 0; /* 0=INCLUDE, 1=EXCLUDE */

  	listnode_add(igmp->group_list, group);
	
    zlog_debug(IGMP_DBG_RCV, "Creating new IGMP group %d on interface %db group count:%d\n\r",
	       	group->grp_addr, group->grp_igmp->pif->ifindex, listcount(igmp->group_list));

	/*
	RFC 3376: 6.2.2. Definition of Group Timers

	The group timer is only used when a group is in EXCLUDE mode and
	it represents the time for the *filter-mode* of the group to
	expire and switch to INCLUDE mode.
	*/
  	zassert(!group->grp_filtermode); /* INCLUDE mode */
  	zassert(!group->grp_timer); /* group timer == 0 */

  	/* Any source (*,G) is forwarded only if mode is EXCLUDE {empty} */
  	igmp_anysource_forward_stop(group);

	/*igmp成员组的添加，触发pim加枝*/
	
  	return group;
}

static void igmp_join_free(struct igmp_join *ij)
{
	XFREE(MTYPE_IPMC_IP_ENTRY, ij);
}

struct igmp_join * igmp_join_lookup(struct igmp *igmp, struct igmp_join *join)
{
	struct igmp_join *pjoin = NULL;
	struct listnode *node = NULL;
	
	for (ALL_LIST_ELEMENTS_RO(igmp->join_list, node, pjoin))
	{
	 	if (0 == memcmp(pjoin, join, sizeof(struct igmp_join)))
	 	{
			return pjoin;
	 	}
	}

	return NULL;
}

int  igmp_join_add(struct igmp *igmp, struct igmp_join *join)
{
	struct igmp_join *pjoin = NULL;

	pjoin = igmp_join_lookup(igmp, join);
	if (NULL != pjoin)
	{
		return ERRNO_SUCCESS;
	}

	pjoin = (struct igmp_join *)XCALLOC(MTYPE_IPMC_IP_ENTRY, sizeof(struct igmp_join));
	if (NULL == pjoin)
	{
		return ERRNO_MALLOC;
	}
	
	memcpy(pjoin, join, sizeof(struct igmp_join));
	
	listnode_add(igmp->join_list, (void *)pjoin);

	return ERRNO_SUCCESS;
}

int igmp_join_delete(struct igmp *igmp, struct igmp_join *join)
{
	struct igmp_join *pjoin;
	
	pjoin = igmp_join_lookup(igmp, join);
	if (NULL == pjoin)
	{
		return ERRNO_SUCCESS;
	}
	
	listnode_delete(igmp->join_list, pjoin);

	igmp_join_free(pjoin);

	return ERRNO_SUCCESS;
}

static void igmp_source_group_table_set(int vpn, int ifindex, 
		uint32_t gip, uint32_t sip)
{
	struct mcgroup_t *grp = NULL;
	struct mcip_t *pip = NULL;
	struct mcip_t mcip;
	int group_id;
	int ret;

	mcip.dip = gip;
	mcip.sip = sip;
	mcip.vpnid = vpn;
	mcip.source_port_nocheck = 1;

	pip = mc_ip_lookup(&mcip);
	if (NULL == pip)
	{
		/*add mcip to mc_ip_table*/
		/*check mc_ip_table to lookup group id*/
		group_id = index_alloc(INDEX_TYPE_IPMC);
		if (group_id == 0)
		{
    		return;
		}

		ret = mc_group_create(group_id);
		if (ret)
		{
			index_free(INDEX_TYPE_IPMC, group_id);
			
    		return;
		}
		
		/*save group id*/
		mcip.group_id = group_id;

		/*add to mc ip table*/
		mc_ip_add(&mcip);

		pip = mc_ip_lookup(&mcip);
	}

	/*lookup group id struct wether in mc_ip_table*/
	grp = mc_group_lookup(pip->group_id);
	if (NULL == grp)
	{
    	return;
	}
	
	/*add mcip to mc_group_table */
	mc_group_add_if(grp, ifindex);
}

static void igmp_source_group_table_del(int vpn, int ifindex, 
		uint32_t gip, uint32_t sip)
{
	struct mcgroup_t *grp = NULL;
	struct mcip_t *pip = NULL;
	struct mcip_t mcip;
	int ret;
	
	mcip.dip = gip;
	mcip.sip = sip;
	mcip.vpnid = vpn;
	mcip.source_port_nocheck = 1;

	pip = mc_ip_lookup(&mcip);
	if (NULL == pip)
	{
		return;
	}
		
	/*lookup group id struct wether in mc_ip_table*/
	grp = mc_group_lookup(pip->group_id);
	if (NULL == grp)
	{
    	return;
	}
		
	/*delete mcip from mc_group_table */
	ret = mc_group_delete_if(grp, ifindex);
	if (ret)
	{
    	return;
	}

	/*check wether mcip used by other interface, if not delete it*/
	if (grp->iflist.count < 1)
	{
		mc_group_delete(pip->group_id);
	
		mcip.group_id = pip->group_id;
		mc_ip_delete(&mcip);
	}
}

/*(*, G) table set*/
void igmp_anysource_forward_start(struct igmp_group *group)
{
	struct igmp_join *pjoin = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	struct igmp_join join;
	int vpn;

	pigmp = group->grp_igmp;
	if (NULL == pigmp)
	{
		return;
	}

	pif = pigmp->pif;
	if (NULL == pif)
	{
		return;
	}

	if (1 != pif->pim_mode)
	{
		/*look up interface vpn instance*/
		vpn = igmp_instance_find(pif->ifindex);
		if (vpn < 0)
		{
	    	return;
		}

		/*static group&source*/
		join.grpip = group->grp_addr;
		join.sip   = 0;
		pjoin = igmp_join_lookup(pigmp, &join);
		if (NULL != pjoin)
		{
			return;
		}

		igmp_source_group_table_set(vpn, pif->ifindex, group->grp_addr, 0);
	}
}

void igmp_anysource_forward_stop(struct igmp_group *group)
{
	struct igmp_join *pjoin = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	struct igmp_join join;
	int vpn;

	pigmp = group->grp_igmp;
	if (NULL == pigmp)
	{
		return;
	}

	pif = pigmp->pif;
	if (NULL == pif)
	{
		return;
	}

	if (1 != pif->pim_mode)
	{
		/*look up interface vpn instance*/
		vpn = igmp_instance_find(pif->ifindex);
		if (vpn < 0)
		{
	    	return;
		}

		/*static group&source*/
		join.grpip = group->grp_addr;
		join.sip   = 0;
		pjoin = igmp_join_lookup(pigmp, &join);
		if (NULL != pjoin)
		{
			return;
		}

		igmp_source_group_table_del(vpn, pif->ifindex, group->grp_addr, 0);
	}
}

void igmp_source_forward_start(struct igmp_source *source)
{
	struct igmp_group *group = NULL;
	struct igmp_join *pjoin = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	struct igmp_join join;
	int vpn;

	group = source->src_grp;
	if (NULL == group)
	{
		return;
	}
	
	pigmp = group->grp_igmp;
	if (NULL == pigmp)
	{
		return;
	}

	pif = pigmp->pif;
	if (NULL == pif)
	{
		return;
	}

	if (1 != pif->pim_mode)
	{
		/*look up interface vpn instance*/
		vpn = igmp_instance_find(pif->ifindex);
		if (vpn < 0)
		{
	    	return;
		}

		/*static group&source*/
		join.grpip = group->grp_addr;
		join.sip   = source->sip;
		pjoin = igmp_join_lookup(pigmp, &join);
		if (NULL != pjoin)
		{
			return;
		}

		igmp_source_group_table_set(vpn, pif->ifindex, group->grp_addr, source->sip);
	}
}

void igmp_source_forward_stop(struct igmp_source *source)
{
	struct igmp_group *group = NULL;
	struct igmp_join *pjoin = NULL;
	struct ipmc_if *pif = NULL;
	struct igmp *pigmp = NULL;
	struct igmp_join join;
	int vpn;

	group = source->src_grp;
	if (NULL == group)
	{
		return;
	}

	pigmp = group->grp_igmp;
	if (NULL == pigmp)
	{
		return;
	}

	pif = pigmp->pif;
	if (NULL == pif)
	{
		return;
	}

	if (1 != pif->pim_mode)
	{
		/*look up interface vpn instance*/
		vpn = igmp_instance_find(pif->ifindex);
		if (vpn < 0)
		{
	    	return;
		}

		/*static group&source*/
		join.grpip = group->grp_addr;
		join.sip   = source->sip;
		pjoin = igmp_join_lookup(pigmp, &join);
		if (NULL != pjoin)
		{
			return;
		}

		igmp_source_group_table_del(vpn, pif->ifindex, group->grp_addr, source->sip);
	}
}

void igmp_init(void)
{
	igmp_pkt_register();
	igmp_instance_init();
	igmp_cmd_init();
}

