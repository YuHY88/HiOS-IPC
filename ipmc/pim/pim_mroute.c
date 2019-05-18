/*
*   PIM mrt处理
*/

#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
//#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
//#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/checksum.h>
#include <lib/route_com.h>
#include <lib/ifm_common.h>

#include "pim_mroute.h"
#include "pim_register.h"
#include "../ipmc_public.h"
#include "../ipmc.h"

/* grp addr/src addr as hash_key */
static uint32_t pim_mrt_grp_src_hash(void *hash_key)
{	
    return ((unsigned int) (hash_key));
}

static int pim_mrt_grp_src_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;

	if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

	if (pbucket->hash_key == hash_key)
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


void pim_mrt_sg_grp_table_init(struct hash_table *table, uint32_t size)
{
	hios_hash_init(table, size, pim_mrt_grp_src_hash, pim_mrt_grp_src_compare);
}

struct pim_mrt_sg_grp *pim_mrt_sg_grp_lookup(struct hash_table *table, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(table, (void *)grp_addr);
	if(pitem)
	{
		return (struct pim_mrt_sg_grp *)pitem->data;
	}
	return NULL;
}

struct pim_mrt_sg_grp *pim_mrt_sg_grp_add(struct hash_table *table, uint32_t src_addr, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == pitem)
	{
		IPMC_LOG_ERROR("XCALLOC(struct hash_bucket) failed!\n");
		return NULL;
	}
	
	sg_grp = (struct pim_mrt_sg_grp *)XCALLOC(MTYPE_IPMC_PIM_SG_ENTRY, sizeof(struct pim_mrt_sg_grp));
	if (NULL == sg_grp)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("XCALLOC(struct pim_mrt_sg_grp) failed!\n");
		return NULL;
	}
	memset(sg_grp, 0, sizeof(struct pim_mrt_sg_grp));
	sg_grp->grp_addr = grp_addr;
	pim_mrt_sg_grp_table_init(&(sg_grp->sg_table), PIM_MRT_SG_MAX);
	
	pitem->hash_key = (void *)sg_grp->grp_addr;
	pitem->data = (void *)sg_grp;
	hios_hash_add(table, pitem);

	
	return (struct pim_mrt_sg_grp *)pitem->data;
}

struct pim_mrt_sg *pim_mrt_sg_add(struct hash_table *table, uint32_t src_addr, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_mrt_sg *sg = NULL;

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == pitem)
	{
		IPMC_LOG_ERROR("XCALLOC(struct hash_bucket) failed!\n");
		return NULL;
	}
	
	sg = (struct pim_mrt_sg *)XCALLOC(MTYPE_IPMC_PIM_SG_ENTRY, sizeof(struct pim_mrt_sg));
	if (NULL == sg)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("XCALLOC(struct pim_mrt_sg_add) failed!\n");
		return NULL;
	}
	memset(sg, 0, sizeof(struct pim_mrt_sg));
	sg->flag |= PIM_MRT_SG;
	sg->flag |= PIM_MRT_UNUSED;
	sg->grp_addr = grp_addr;
	sg->src_addr = src_addr;
	sg->reg_state = PIM_REG_STATE_NI;
	/*sg->keepalive_timer = thread_add_timer(ipmc_master, 
								pim_mrt_sg_keepalive_timer_expire,
								(void *)sg, 
								Keepalive_Period_default);*/
	sg->keepalive_timer = high_pre_timer_add("JPDownSGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
						pim_mrt_sg_keepalive_timer_expire,(void *)sg,
						1000 * Keepalive_Period_default);
	
	pitem->hash_key = (void *)sg->src_addr;
	pitem->data = (void *)sg;
	hios_hash_add(table, pitem);

	return (struct pim_mrt_sg *)pitem->data;
}

struct pim_mrt_sg * pim_mrt_sg_get(uint32_t src_addr, uint32_t grp_addr, uint32_t create_flags)
{
	struct pim_instance *ppim = NULL;
	struct hash_bucket *pitem = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_upstream *upstream = NULL;
	
	ppim = pim_instance_global;
	if(ppim == NULL)
	{
		IPMC_LOG_ERROR(" pim_instance_global == NULL\n");
		return NULL;
	}
	if(!ipv4_is_multicast(grp_addr))
	{
		IPMC_LOG_ERROR(" grp_addr %x is not a multicast address\n", grp_addr);
		return NULL;
	}
	wc = pim_mrt_wc_lookup(ppim, grp_addr);
	sg_grp = pim_mrt_sg_grp_lookup(&(ppim->pim_sg_table), grp_addr);
	if(sg_grp)
	{
		/*根据src查找指定sg*/
		pitem = hios_hash_find(&(sg_grp->sg_table), (void *)src_addr);
		if(pitem)
		{
			sg = (struct pim_mrt_sg *)pitem->data;
			return sg;
		}
		else
		{
			if(create_flags)
			{
				sg = pim_mrt_sg_add(&(sg_grp->sg_table), src_addr, grp_addr);
				if(sg == NULL)
				{
					IPMC_LOG_ERROR("pim_mrt_sg_add(0x%x, 0x%x) return NULL, failed\n", src_addr, grp_addr);
					return NULL;
				}
				sg->jp_up_state = PIM_JP_UP_STATE_NOTJOINED;
				sg->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NP;
				sg->wc_mrt = wc;
			}
			else
			{
				IPMC_LOG_ERROR("SG(0x%x, 0x%x) return NULL, failed\n", src_addr, grp_addr);
				return NULL;
			}
		}
	}
	else 
	{
		if(create_flags)
		{
			sg_grp = pim_mrt_sg_grp_add(&(ppim->pim_sg_table), src_addr, grp_addr);
			if(sg_grp == NULL)
			{
				IPMC_LOG_ERROR("pim_mrt_sg_grp_add(0x%x, 0x%x) return NULL, failed\n", src_addr, grp_addr);
				return NULL;
			}
			else
			{
				/*创建sg*/
				sg = pim_mrt_sg_add(&(sg_grp->sg_table), src_addr, grp_addr);
				if(sg == NULL)
				{
					IPMC_LOG_ERROR("pim_mrt_sg_add(0x%x, 0x%x) return NULL failed\n", src_addr, grp_addr);
					return NULL;
				}
				sg->jp_up_state = PIM_JP_UP_STATE_NOTJOINED;
				sg->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NP;
				sg->wc_mrt = wc;
				sg->rp_addr = pim_rp_check(grp_addr);
				sg_grp->sg_num++;
			}
		}
		else
		{
			IPMC_LOG_ERROR("SG(0x%x, 0x%x) return NULL, failed\n", src_addr, grp_addr);
			return NULL;
		}
	}
	
	if(create_flags)
	{
		/*获取去往源的上游信息，上游信息状态决定了sg状态*/
		upstream = pim_mrt_upstream_get(src_addr, PIM_MRT_UPSTREAM_TO_SRC);
		if(upstream && upstream->flag == TRUE)
		{
			/*upstream 有效*/
			sg->flag &= ~PIM_MRT_UNUSED;
			if(upstream->ifindex && !upstream->rpf_nbr)
			{
				sg->flag |= PIM_MRT_LOCAL;
			}
			sg->rpf_nbr = upstream->rpf_nbr;
			sg->upstream_ifindex = upstream->ifindex;
			sg->upstream_index = upstream->index;
		}
		pim_mrt_sg_oif_cal(sg);
	}
	return sg;
}

uint32_t ip4_addr_local_main(uint32_t addr)
{
	struct ifm_l3 *l3if = NULL;
	int i, data_num = 0;
	uint32_t is_local = 0;

	l3if = pim_getbulk_if_ip(MODULE_ID_IPMC, &data_num);
	if (!data_num)
	{
		return PIM_FALSE;
	}
	
	for (i = 0; i < data_num; i++)
	{
		if ((l3if->ipv4[0].addr == addr) || 
			(l3if->ipv4[1].addr == addr))
		{
			is_local = l3if->ifindex;
			break;
		}
		l3if++;
	}
	
	return is_local;
}

struct pim_mrt_upstream *pim_mrt_upstream_lookup(struct hash_table *table, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(table, (void *)src_addr);
	if(pitem)
	{
		return (struct pim_mrt_upstream *)pitem->data;
	}
	return NULL;
}

struct pim_mrt_upstream *pim_mrt_upstream_add(struct hash_table *table, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_mrt_upstream *upstream = NULL;

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == pitem)
	{
		IPMC_LOG_ERROR("XCALLOC(struct hash_bucket) failed!\n");
		return NULL;
	}
	
	upstream = (struct pim_mrt_upstream *)XCALLOC(MTYPE_IPMC_PIM_SAME_SRC_ENTRY, sizeof(struct pim_mrt_upstream));
	if (NULL == upstream)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("XCALLOC(struct pim_mrt_upstream) failed!\n");
		return NULL;
	}
	memset(upstream, 0, sizeof(struct pim_mrt_upstream));
	upstream->toward = src_addr;
	//upstream->flag |= PIM_MRT_UNUSED;
	
	pitem->hash_key = (void *)upstream->toward;
	pitem->data = (void *)upstream;
	hios_hash_add(table, pitem);

	return (struct pim_mrt_upstream *)pitem->data;
}

struct pim_mrt_upstream * pim_mrt_upstream_get( uint32_t src_addr, uint32_t flag)
{
	uint8_t i = 0;
	uint32_t ifindex = 0, nexthop = 0;
	struct pim_instance *ppim = NULL;
	struct pim_mrt_upstream * upstream = NULL;
	struct inet_prefix prefix = {0};
	struct route_entry *proute = NULL;
	struct nhp_entry *pnhp = NULL;
	struct ipmc_if *pif = NULL;

	ppim = pim_instance_global;
	if(ppim == NULL)
	{
		return NULL;
	}
	upstream = pim_mrt_upstream_lookup(&(ppim->same_src_table), src_addr);
	if(upstream == NULL)
	{
		upstream = pim_mrt_upstream_add(&(ppim->same_src_table), src_addr);
		if(upstream == NULL)
		{
			IPMC_LOG_ERROR("pim_mrt_upstream_add(0x%x) return NULL, failed!\n", src_addr);
			return NULL;
		}
		/*获取upstream*/
		/*判断src_addr是否为本地地址*/
		if(ip4_addr_local_main(src_addr))
		{
			/*组播源为本地，暂时不会出现这种情况*/
			/*当前设备就是rp*/
			//upstream->flag &= ~PIM_MRT_UNUSED;
			//upstream->flag |= PIM_MRT_LOCAL;
			upstream->flag = TRUE;
			upstream->rpf_nbr = 0;
			upstream->ifindex = 0;
			upstream->index = 0;
			upstream->metric = 0;
			return upstream;
		}
		else
		{
			/*获取去往源的路由*/
			prefix.addr.ipv4 = src_addr;
			prefix.prefixlen = 32;
			prefix.type = INET_FAMILY_IPV4;
			proute =  (struct route_entry *)route_com_get_route(&prefix, 0, MODULE_ID_IPMC);
			if(proute == NULL)
			{
				IPMC_LOG_ERROR("route_com_get_route(0x%x) return NULL, failed!\n", src_addr);
				return upstream;
			}
			if(proute->nhp_num == 0)
			{
				IPMC_LOG_ERROR("proute->nhp_num == 0 ,no route to src 0x%x!\n", src_addr);
				return upstream;
			}
			for(i = 0; i < proute->nhp_num; i++)
			{
				pnhp = &(proute->nhp[i]);
				if((pnhp->action != NHP_ACTION_DROP) && (pnhp->nhp_type != NHP_TYPE_INVALID))
				{
					ifindex = pnhp->ifindex;
					/*根据ifindex查找邻居*/
					pif = ipmc_if_lookup(ifindex);
					if(pif)
					{
						/*与源直连的接口也必须开启pim，否则查找不到上游*/
						nexthop = pnhp->nexthop.addr.ipv4;
						if(nexthop == 0)
						{
							/*静态配置的路由，同网段时下一跳是0.0.0.0*/
							if((pnhp->action == NHP_ACTION_FORWARD) && (pnhp->nhp_type == NHP_TYPE_CONNECT))
							{
								nexthop = src_addr;
							}
							else
							{
								break;
							}
						}
						if(nexthop == src_addr)
						{
							/*直连源，源不是pim邻居*/
							//upstream->flag &= ~PIM_MRT_UNUSED;
							//upstream->flag |= PIM_MRT_CONNECTED;
							if(flag == PIM_MRT_UPSTREAM_TO_SRC)
							{
								if(!pim_nbr_node_lookup(pif->pim_nbr, nexthop))
								{
									/*src不是邻居*/
									if(upstream->flag == FALSE)
									{
										upstream->flag = TRUE;
										upstream->rpf_nbr = 0;
										upstream->ifindex = ifindex;
										upstream->index = pif->index;
										upstream->metric = pnhp->distance;
									}
									else
									{
										if(pnhp->distance < upstream->metric)
										{
											upstream->flag = TRUE;
											upstream->rpf_nbr = 0;
											upstream->ifindex = ifindex;
											upstream->index = pif->index;
											upstream->metric = pnhp->distance;
										}
									}
								}
								else
								{
									/*src 是邻居*/
									if(upstream->flag == FALSE)
									{
										upstream->flag = TRUE;
										upstream->rpf_nbr = nexthop;
										upstream->ifindex = ifindex;
										upstream->index = pif->index;
										upstream->metric = pnhp->distance;
									}
									else
									{
										if(pnhp->distance < upstream->metric)
										{
											upstream->flag = TRUE;
											upstream->rpf_nbr = nexthop;
											upstream->ifindex = ifindex;
											upstream->index = pif->index;
											upstream->metric = pnhp->distance;
										}
									}
								}
							}
							else if(flag == PIM_MRT_UPSTREAM_TO_RP)
							{
								if(!pim_nbr_node_lookup(pif->pim_nbr, nexthop))
								{
									continue;
								}
								if(upstream->flag == FALSE)
								{
									upstream->flag = TRUE;
									upstream->rpf_nbr = nexthop;
									upstream->ifindex = ifindex;
									upstream->index = pif->index;
									upstream->metric = pnhp->distance;
								}
								else
								{
									if(pnhp->distance < upstream->metric)
									{
										upstream->flag = TRUE;
										upstream->rpf_nbr = nexthop;
										upstream->ifindex = ifindex;
										upstream->index = pif->index;
										upstream->metric = pnhp->distance;
									}
								}
							}
						}
						else
						{
							/*不与源直连，经过交换机与源连接，同一个网段*/
							/*不与src直连时，必须是pim邻居*/
							if(pim_nbr_node_lookup(pif->pim_nbr, nexthop))
							{
								if(upstream->flag == FALSE)
								{
									//upstream->flag &= ~PIM_MRT_UNUSED;
									upstream->flag = TRUE;
									upstream->rpf_nbr = nexthop;
									upstream->ifindex = ifindex;
									upstream->index = pif->index;
									upstream->metric = pnhp->distance;
								}
								else if(pnhp->distance < upstream->metric)
								{
									//upstream->flag &= ~PIM_MRT_UNUSED;
									upstream->flag = TRUE;
									upstream->rpf_nbr = nexthop;
									upstream->ifindex = ifindex;
									upstream->index = pif->index;
									upstream->metric = pnhp->distance;
								}
							}
							
						}
					}
					else
					{
						/*出接口不是pim接口*/
						printf("nexthop is not ipmc if\n");
					}
				}
			}			
		}
	}
	return upstream;
}

uint32_t pim_mrt_sg_oif_cal(struct pim_mrt_sg *sg)
{
	uint8_t i = 0;
	struct pim_mrt_wc *wc = sg->wc_mrt;
	
	/*clear res_oif*/
	IF_SETZERO(&(sg->oif.res_oif));
	sg->oif.oif_num = 0;

	if(wc)
	{
		for(i = 0; i < IFBIT_SIZE; i++)
		{
			/*+ (*g) join*/
			IF_PLUS( &(sg->oif.res_oif), &(wc->oif.join_oif), i);
			
			/* - (S,G,rpt) prune*/
			IF_MINUS( &(sg->oif.res_oif), &(sg->oif.prune_oif), i);
			
			/*+ pim_include(*,G) */
			IF_PLUS( &(sg->oif.res_oif), &(wc->oif.pim_in_oif), i);

			/* - pim_exclude(S,G)*/
			IF_MINUS( &(sg->oif.res_oif), &(sg->oif.pim_ex_oif), i);

			/* - lost_assert(*,G)*/
			IF_MINUS( &(sg->oif.res_oif), &(wc->oif.lost_assert_oif), i);

			/*+ lost_assert(S,G,rpt) */

			if(sg->flag & PIM_MRT_SPT)
			{
				/* + joins(S,G)*/
				IF_PLUS( &(sg->oif.res_oif), &(wc->oif.join_oif), i);
				
				/* + pim_include(S,G)*/
				IF_PLUS( &(sg->oif.res_oif), &(wc->oif.pim_in_oif), i);
				
				/* - lost_assert(S,G)*/
				IF_MINUS( &(sg->oif.res_oif), &(sg->oif.lost_assert_oif), i);
			}
		}
	}
	else
	{
		if(sg->flag & PIM_MRT_SPT)
		{
			for(i = 0; i < IFBIT_SIZE; i++)
			{
				/* + joins(S,G)*/
				IF_PLUS( &(sg->oif.res_oif), &(sg->oif.join_oif), i);
				
				/* + pim_include(S,G)*/
				IF_PLUS( &(sg->oif.res_oif), &(sg->oif.pim_in_oif), i);
				
				/* - lost_assert(S,G)*/
				IF_MINUS( &(sg->oif.res_oif), &(sg->oif.lost_assert_oif), i);
			}
		}
	}
	IF_CLR(sg->upstream_index, &(sg->oif.res_oif));
	/*计算出接口数目*/
	for(i = 0; i < IPMC_IF_MAX; i++)
	{
		if(IF_ISSET(i, &(sg->oif.res_oif)))
		{
			sg->oif.oif_num++;
		}
	}
	
	return sg->oif.oif_num;
}

uint32_t pim_mrt_sg_oif_add(struct ipmc_if *pif, struct pim_down_jp_sg_node *sg_node)
{
	struct pim_mrt_sg* sg = NULL;
	uint32_t index = pif->index;
	/*接口下收到了sg加枝,已经添加了节点，节点老化，接口从表项中删除*/
	
	/*创建对应的sg表项*/
	sg = pim_mrt_sg_get(sg_node->src_addr, sg_node->grp_addr, IPMC_TRUE);
	if(sg == NULL)
	{
		IPMC_LOG_ERROR("pim_mrt_sg_get(%x, %x, %x, %d) return NULL, failed\n", pif->ifindex, sg_node->src_addr, sg_node->grp_addr, IPMC_TRUE);
		return NULL;
	}

	if(!IF_ISSET(index, &(sg->oif.join_oif)))
	{
		IF_SET(index, &(sg->oif.join_oif));
		sg->flag |= PIM_MRT_SM;
		sg->flag |= PIM_MRT_SG;
		sg->flag |= PIM_MRT_SPT;
		/*重新计算出接口*/
		pim_mrt_sg_oif_cal(sg);
		/*update upstream (S,G) state*/
		if(JoinDesiredSG(sg))
		{
			/*上游状态机*/
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
		}
		/*update upstream (S,G,RPT)  state*/
		if(PruneDesiredSGRPT(sg))
		{
			/*上游状态机*/
			pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
		}
		/*表项下发*/
		if(!(sg->flag & PIM_MRT_CACHE))
		{
			pim_mrt_sg_cache_add(sg);
		}
		else if(sg->flag & PIM_MRT_SPT)
		{
			pim_mrt_sg_cache_add(sg);
		}
	}
}

uint32_t pim_mrt_sg_oif_del(struct ipmc_if *pif, uint32_t src_addr, uint32_t grp_addr)
{
	struct pim_mrt_sg *sg = NULL;
	uint32_t index = 0;

	if(pif == NULL || src_addr == 0 || grp_addr == 0)
	{
		IPMC_LOG_ERROR("sg(%x, %x) ,pif == NULL || src_addr == 0 || grp_addr == 0\n", src_addr, grp_addr);
		return ERRNO_FAIL;
	}
	index = pif->index;
	/*查找mrt_sg*/
	sg = pim_mrt_sg_get(src_addr, grp_addr, IPMC_FALSE);
	if(sg == NULL)
	{
		IPMC_LOG_NOTICE("sg(%x,%x) is NULL\n", src_addr, grp_addr);
		return ERRNO_NOT_FOUND;
	}
	if(IF_ISSET(index, &(sg->oif.join_oif)))
	{
		IF_CLR(index, &(sg->oif.join_oif));
		pim_mrt_sg_oif_cal(sg);
		
		/*sg oif change, immediate_olist(S,G), inherited_olist(S,G)变化*/
		/*immediate_olist(S,G) 影响JoinDesired(S,G)*/
		/*inherited_olist(S,G)影响JoinDesired(S,G), assert, register state*/
		/*JoinDesired(S,G)影响 AssertTrackingDesired(S,G,I),Update_SPTbit(S,G,iif),sg upstream state*/

		/*SPT影响 PruneDesired(S,G,rpt),CouldAssert(S,G,I), AssertTrackingDesired(S,G,I),*/
		/*add Prune(S,G,rpt) to compound message,  lost_assert(S,G,rpt,I), send Assert(S,G) on iif, send Assert(*,G) on iif */
		Update_SPTbit(sg, sg->upstream_ifindex);
		if(!PruneDesiredSGRPT(sg))
		{
			pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
		}
		
		if(!JoinDesiredSG(sg))
		{
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
		}
		/* if spt_bit set, use (S,G) state and (*,G)state */
		/* if spt_bit not set, only use (*,G)state*/
		/*更新硬件表项*/
		if(sg->flag & PIM_MRT_CACHE)
		{
			pim_mrt_sg_cache_add(sg);
		}
	}
}

/*
  immediate_olist(*,G) =
	  joins(*,G) (+) pim_include(*,G) (-) lost_assert(*,G)

  immediate_olist(S,G) =
	  joins(S,G) (+) pim_include(S,G) (-) lost_assert(S,G)

 
inherited_olist(S,G,rpt) =
           (  joins(*,G) (-) prunes(S,G,rpt) )
       (+) ( pim_include(*,G) (-) pim_exclude(S,G))
       (-) ( lost_assert(*,G) (+) lost_assert(S,G,rpt) )

inherited_olist(S,G) =
   inherited_olist(S,G,rpt) (+)
   joins(S,G) (+) pim_include(S,G) (-) lost_assert(S,G)
 
*/
uint32_t immediate_olist_wc(struct pim_mrt_wc *wc)
{
	uint32_t i = 0;
	
	for( i = 0; i < IFBIT_SIZE; i++)
	{
		if((wc->oif.join_oif.if_bits[i]) | (wc->oif.pim_in_oif.if_bits[i]))
		{
			return IPMC_TRUE;
		}
	}
	return IPMC_FALSE;
}

uint32_t immediate_olist_sg(struct pim_mrt_sg *sg)
{
	uint32_t i = 0;
	
	for( i = 0; i < IFBIT_SIZE; i++)
	{
		if((sg->oif.join_oif.if_bits[i]) | (sg->oif.pim_in_oif.if_bits[i]))
		{
			return IPMC_TRUE;
		}
	}
	return IPMC_FALSE;
}

uint32_t inherited_olist_sgrpt(struct pim_mrt_sg *sg)
{
	uint32_t i = 0;
	struct pim_mrt_wc *wc = sg->wc_mrt;

	if(wc)
	{
		for( i = 0; i < IFBIT_SIZE; i++)
		{
			if((wc->oif.join_oif.if_bits[i] & (~(sg->oif.prune_oif.if_bits[i]))) | 
				(wc->oif.pim_in_oif.if_bits[i] & (~(sg->oif.pim_ex_oif.if_bits[i]))))
			{
				return IPMC_TRUE;
			}
		}
	}
	return IPMC_FALSE;
}

uint32_t inherited_olist_sg(struct pim_mrt_sg *sg)
{
	if(inherited_olist_sgrpt(sg) || immediate_olist_sg(sg))
	{
		return IPMC_TRUE;
	}
	return IPMC_FALSE;
}
#if 0
uint32_t pim_join_desired_sg(struct pim_mrt_sg *sg)
{
	/*
	bool JoinDesired(S,G) 
	{
		   return( immediate_olist(S,G) != NULL
				   OR 
				( KeepaliveTimer(S,G) is running AND inherited_olist(S,G) != NULL ) )
	}
	*/
	if(sg->flag & PIM_MRT_UNUSED)
	{
		return IPMC_FALSE;
	}
	return (immediate_olist_sg(sg) || (sg->keepalive_timer && inherited_olist_sg(sg)));
}
#endif

uint32_t pim_prune_desired_sgrpt(struct pim_mrt_sg *sg)
{
	/*
	bool PruneDesired(S,G,rpt) {
			  return ( RPTJoinDesired(G) AND
					   ( inherited_olist(S,G,rpt) == NULL
						 OR (SPTbit(S,G)==TRUE
							 AND (RPF'(*,G) != RPF'(S,G)) )))
		 }
	bool RPTJoinDesired(G) {
		  return (JoinDesired(*,G) )
		}
	bool JoinDesired(*,G) {
		   if (immediate_olist(*,G) != NULL OR
			   (JoinDesired(*,*,RP(G)) AND
				AssertWinner(*, G, RPF_interface(RP(G))) != NULL))
			   return TRUE
		   else
			   return FALSE
		}

	*/
	struct pim_mrt_wc *wc = sg->wc_mrt;
	if(!wc)
	{
		return IPMC_FALSE;
	}
	return (JoinDesiredWC(wc) && 
			(inherited_olist_sgrpt(sg) == IPMC_FALSE || 
				(!(sg->flag & PIM_MRT_SPT) && wc->upstream_ifindex != sg->upstream_ifindex)));
}

uint32_t pim_mrt_sg_cache_add(struct pim_mrt_sg *sg)
{
    struct mc_cache_t cache = {0};
	uint32_t ret = ERRNO_FAIL;

	sg->flag |= PIM_MRT_CACHE;
	/*向hal下发表项*/
	cache.sip = sg->src_addr;
	cache.dip = sg->grp_addr;
	cache.iif = sg->upstream_index;
	cache.vpnid = 0;
	/*表项无效时，下发空出接口，组播数据被丢弃*/
	if(sg->flag & PIM_MRT_UNUSED || (!(sg->flag & PIM_MRT_SPT) && (sg->wc_mrt == NULL || (sg->wc_mrt->flag & PIM_MRT_UNUSED))))
	{
		ret = ipmc_msg_send_to_hal((void *)(&cache), sizeof(struct mc_cache_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
							 IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, 0);
		/*ret = ipc_send_hal((void *)(&cache), sizeof(struct mc_cache_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
							 IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, 0);*/
		if(ret) 
		{
			IPMC_LOG_ERROR("Failed to send add mc group to hal.\n");
			return ERRNO_IPC;
		}
	}
	else
	{
		IF_COPY(&(cache.oif), &(sg->oif.res_oif), sizeof(struct if_set));
		ret = ipmc_msg_send_to_hal((void *)(&cache), sizeof(struct mc_cache_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
							 IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, 1);
		/*ret = ipc_send_hal((void *)(&cache), sizeof(struct mc_cache_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
							 IPMC_SUBTYPE_MFIB, IPC_OPCODE_ADD, 1);*/
		if(ret) 
		{
			IPMC_LOG_ERROR("Failed to send add mc group to hal.\n");
			return ERRNO_IPC;
		}
	}
	return ERRNO_SUCCESS;
}

uint32_t pim_rp_check(uint32_t grp_addr)
{
	
	if(pim_instance_global == NULL )
	{
		IPMC_LOG_ERROR("pim_instance_global NULL\n");
		return 0;
	}
	else if(pim_instance_global->rp_addr)
	{
		return (pim_instance_global->rp_addr);
	}
	return 0;
}

struct pim_mrt_wc* pim_mrt_wc_lookup(struct pim_instance *ppim, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;

	if(ppim == NULL)
	{
		IPMC_LOG_ERROR("pim_instance_global == NULL\n");
		return NULL;
	}
	/*根据grp_addr查找wc是否存在*/
	pitem = hios_hash_find(&(ppim->pim_wc_table), (void *)grp_addr);
	if(pitem)
	{
		return (struct pim_mrt_wc *)pitem->data;
	}
	return NULL;
}

struct pim_mrt_wc* pim_mrt_wc_add(struct pim_instance *ppim, uint32_t grp_addr)
{
	struct pim_mrt_wc* wc = NULL;
	struct hash_bucket *pitem = NULL;
	
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if (NULL == pitem)
	{
		IPMC_LOG_ERROR("XCALLOC(struct hash_bucket) failed!\n");
		return NULL;
	}
	
	wc = (struct pim_mrt_wc *)XCALLOC(MTYPE_IPMC_PIM_WC_ENTRY, sizeof(struct pim_mrt_wc));
	if (NULL == wc)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("XCALLOC(struct pim_mrt_wc) failed!\n");
		return NULL;
	}
	memset(wc, 0, sizeof(struct pim_mrt_wc));
	wc->flag |= PIM_MRT_WC;
	wc->flag |= PIM_MRT_UNUSED;
	wc->grp_addr = grp_addr;
	wc->jp_up_state = PIM_JP_UP_STATE_NOTJOINED;
	
	pitem->hash_key = (void *)wc->grp_addr;
	pitem->data = (void *)wc;
	hios_hash_add(&(ppim->pim_wc_table), pitem);

	return (struct pim_mrt_wc *)pitem->data;
}

struct pim_mrt_wc* pim_mrt_wc_get( uint32_t grp_addr)
{
	struct pim_instance *ppim = NULL;
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_upstream *upstream = NULL;

	ppim = pim_instance_global;
	if(ppim == NULL)
	{
		IPMC_LOG_ERROR(" pim_instance_global == NULL\n");
		return NULL;
	}
	if(!ipv4_is_multicast(grp_addr))
	{
		IPMC_LOG_ERROR(" grp_addr %x is not a multicast address\n", grp_addr);
		return NULL;
	}
	wc = pim_mrt_wc_lookup(ppim, grp_addr);
	if(wc)
	{
		return wc;
	}
	wc = pim_mrt_wc_add(ppim, grp_addr);
	if(wc == NULL)
	{
		IPMC_LOG_ERROR(" pim_mrt_wc_add return NULL, failed\n");
		return NULL;
	}
	wc->rp_addr = pim_rp_check(grp_addr);
	/*获取wc表项去往rp的上游信息*/
	upstream = pim_mrt_upstream_get( wc->rp_addr, PIM_MRT_UPSTREAM_TO_RP);
	if(upstream && upstream->flag == TRUE)
	{
		/*upstream 有效*/
		wc->flag &= ~PIM_MRT_UNUSED;
		if(upstream->ifindex == 0)
		{
			wc->flag |= PIM_MRT_LOCAL;
		}
		wc->rpf_nbr = upstream->rpf_nbr;
		wc->upstream_ifindex = upstream->ifindex;
		wc->upstream_index = upstream->index;
	}
	return wc;
}

void pim_mrt_wc_del(struct pim_mrt_wc *wc)
{
	struct pim_mrt_sg *sg = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct hash_table *sg_table = NULL;
	uint32_t val = 0;
	struct hash_bucket *bucket =NULL;

	/*表项删除，将所有的加枝删除*/
	pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_F);
	/*相关sg表项的wc为空*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp)
	{
		sg_table = &(sg_grp->sg_table);
		
		for(val = 0; val < HASHTAB_SIZE; val++)
		{
			for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
			{
				sg = (struct pim_mrt_sg *)bucket->data;
				if(sg)
				{
					sg->wc_mrt = NULL;
				}
			}
		}
	}
}

uint32_t pim_mrt_wc_oif_cal(struct pim_mrt_wc *wc)
{
	uint8_t i = 0;

	if(wc == NULL)
	{
		IPMC_LOG_ERROR("wc == NULL\n");
		return ERRNO_FAIL;
	}
	/*clear res_oif*/
	IF_SETZERO(&(wc->oif.res_oif));
	wc->oif.oif_num = 0;

	if(wc)
	{
		for(i = 0; i < IFBIT_SIZE; i++)
		{
			/*+ (*g) join*/
			IF_PLUS( &(wc->oif.res_oif), &(wc->oif.join_oif), i);
						
			/*+ pim_include(*,G) */
			IF_PLUS( &(wc->oif.res_oif), &(wc->oif.pim_in_oif), i);

			/* - lost_assert(*,G)*/
			IF_MINUS( &(wc->oif.res_oif), &(wc->oif.lost_assert_oif), i);
		}
	}
	/*当入口有效时才将入口剔除，否则默认0会误删出接口*/
	if(wc->upstream_ifindex)
	{
		IF_CLR(wc->upstream_index, &(wc->oif.res_oif));
	}
	
	/*计算出接口数目*/
	for(i = 0; i < IPMC_IF_MAX; i++)
	{
		if(IF_ISSET(i, &(wc->oif.res_oif)))
		{
			wc->oif.oif_num++;
		}
	}
	wc->oif_num = wc->oif.oif_num;
	return wc->oif.oif_num;
}

uint32_t pim_mrt_wc_oif_add(struct ipmc_if *pif, struct pim_down_jp_grp_node *grp_node)
{
	struct pim_mrt_wc *wc_mrt = NULL;

	if(pif == NULL)
	{
		IPMC_LOG_ERROR("pif == NULL, failed\n");
		return ERRNO_FAIL;
	}
	/* (*g)表项添加出接口*/
	wc_mrt = pim_mrt_wc_get( grp_node->grp_addr);
	if(wc_mrt == NULL)
	{
		IPMC_LOG_ERROR("pim_mrt_wc_get return NULL, failed\n");
		return ERRNO_FAIL;
	}
	if(!(IF_ISSET(pif->index, &(wc_mrt->oif.join_oif))))
	{
		/*添加端口*/
		IF_SET(pif->index, &(wc_mrt->oif.join_oif));
		pim_mrt_wc_oif_cal(wc_mrt);
		/*判断是否需要向上游发送加枝*/
		if(JoinDesiredWC(wc_mrt))
		{
			pim_jp_up_wc_state_machine(wc_mrt, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_T);
		}
		
	}
	
	return ERRNO_SUCCESS;
}

uint32_t pim_mrt_wc_oif_del(struct ipmc_if *pif, uint32_t grp_addr)
{
	uint32_t index = 0, val = 0;
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;

	if(pif == NULL || grp_addr == 0)
	{
		IPMC_LOG_NOTICE("pif == NULL || grp_node == NULL || grp_addr == 0\n");
		return ERRNO_FAIL;
	}
	/*获取wc表项*/
	wc = pim_mrt_wc_get(grp_addr);
	if(wc == NULL)
	{
		IPMC_LOG_DEBUG("(*, %x) == NULL\n", grp_addr);
		return ERRNO_SUCCESS;
	}
	index = pif->index;
	if(IF_ISSET(index, &(wc->oif.join_oif)))
	{
		IF_CLR(index, &(wc->oif.join_oif));

		/*重新计算出接口，(*,G)出接口删除会影响sg*/
		pim_mrt_wc_oif_cal(wc);
		/*判断是否需要向上游发送加枝*/
		if(!JoinDesiredWC(wc))
		{
			pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_F);
		}
		
		/*获取同一个grp下的sg，重新计算sg*/
		if(pim_instance_global == NULL)
		{
			IPMC_LOG_ERROR("pim_instance_global NULL\n");
			return ERRNO_FAIL;
		}
		if(pim_instance_global->pim_sg_table.num_entries == 0)
		{
			IPMC_LOG_NOTICE("pim_instance_global->pim_sg_table.num_entries == 0\n");
			return ERRNO_SUCCESS;
		}
		sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), grp_addr);
		if(sg_grp == NULL)
		{
			IPMC_LOG_NOTICE("(S, G) grp %x == NULL\n", grp_addr);
			return ERRNO_SUCCESS;
		}
		sg_table = &(sg_grp->sg_table);
		
		for(val = 0; val < HASHTAB_SIZE; val++)
		{
			for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
			{
				sg = (struct pim_mrt_sg *)bucket->data;
				if(sg)
				{
					pim_mrt_sg_oif_cal(sg);
					if(!RPTJoinDesired(sg))
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_JOINDESIRED_F);
					}
					else if(PruneDesiredSGRPT(sg))
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
					}
					else
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
					}
					
					if(!JoinDesiredSG(sg))
					{
						pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
					}

					/*若下发硬件，更新表项信息*/
					if(sg->flag & PIM_MRT_CACHE)
					{
						pim_mrt_sg_cache_add(sg);
					}
				}
				
			}
		}
	}	
	if(wc->oif_num == 0)
	{
		/* (*,G)没有出接口*/
	}
	
}

uint32_t pim_mrt_sg_rpt_oif_add(struct ipmc_if* pif, struct pim_down_jp_sg_node *sgrpt_node)
{
	/*sg rpt down state is p,  del sg's oif*/
	struct pim_mrt_sg *sg = NULL;
	sg = pim_mrt_sg_get(sgrpt_node->src_addr, sgrpt_node->grp_addr, PIM_LOOKUP);
	if(sg == NULL)
	{
		return 0;
	}
	/*sg表项的sgrpt收到剪枝，sg出接口重新计算，JoinDesired重新计算*/
	if(IF_ISSET(pif->index, &(sg->oif.prune_oif)))
	{
		IF_CLR(pif->index, &(sg->oif.prune_oif));
		pim_mrt_sg_oif_cal(sg);
		if(JoinDesiredSG(sg))
		{
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
		}
		/*更新表项*/
		if(sg->flag & PIM_MRT_CACHE)
		{
			pim_mrt_sg_cache_add(sg);
		}
	}
	return 0;
}

uint32_t pim_mrt_sg_rpt_oif_del(struct ipmc_if* pif, struct pim_down_jp_sg_node *sgrpt_node)
{
	/*sg rpt down state is p,  del sg's oif*/
	struct pim_mrt_sg *sg = NULL;
	sg = pim_mrt_sg_get(sgrpt_node->src_addr, sgrpt_node->grp_addr, PIM_LOOKUP);
	if(sg == NULL)
	{
		return 0;
	}
	/*sg表项的sgrpt收到剪枝，sg出接口重新计算，JoinDesired重新计算*/
	if(!IF_ISSET(pif->index, &(sg->oif.prune_oif)))
	{
		IF_SET(pif->index, &(sg->oif.prune_oif));
		pim_mrt_sg_oif_cal(sg);
		if(!JoinDesiredSG(sg))
		{
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
		}
		/*更新表项*/
		if(sg->flag & PIM_MRT_CACHE)
		{
			pim_mrt_sg_cache_add(sg);
		}
	}
	return 0;
}

/*sint32 pim_mrt_sg_keepalive_timer_expire(struct thread *thread)*/
sint32 pim_mrt_sg_keepalive_timer_expire(void *para)
{
	struct pim_mrt_sg * mrt_sg = NULL;

	if (!para)
		return ERRNO_SUCCESS;
	
	/*mrt_sg = (struct pim_mrt_sg *)THREAD_ARG (thread);*/
	mrt_sg = (struct pim_mrt_sg*)para;
	/*processing complexity*/
	mrt_sg->keepalive_timer = 0;
#if 0
1. 
On receipt of data from S to G on interface iif:
if( DirectlyConnected(S) == TRUE AND iif == RPF_interface(S) ) 
	set KeepaliveTimer(S,G) to Keepalive_Period
		
2.
void CheckSwitchToSpt(S,G) {
	 if ( ( pim_include(*,G) (-) pim_exclude(S,G)
			(+) pim_include(S,G) != NULL )
		  AND SwitchToSptDesired(S,G) ) {
	 # Note: Restarting the KAT will result in the SPT switch
			set KeepaliveTimer(S,G) to Keepalive_Period
	 }
   }
3.
bool CouldRegister(S,G) {
		return ( I_am_DR( RPF_interface(S) ) AND
				 KeepaliveTimer(S,G) is running AND
				 DirectlyConnected(S) == TRUE )
	 }
4.
When an RP receives a Register message,
if ( SPTbit(S,G) OR SwitchToSptDesired(S,G) ) {
	 if ( sentRegisterStop == TRUE ) {
		  set KeepaliveTimer(S,G) to RP_Keepalive_Period;
	 } else {
		  set KeepaliveTimer(S,G) to Keepalive_Period;
	 }
}
This may cause the upstream (S,G) state
machine to trigger a join if the inherited_olist(S,G) is not NULL.
at the RP, KeepaliveTimer(S,G) should be restarted to ( 3 *
Register_Suppression_Time + Register_Probe_Time ).

5.
bool JoinDesired(S,G) {
	 return( immediate_olist(S,G) != NULL
			 OR ( KeepaliveTimer(S,G) is running
				  AND inherited_olist(S,G) != NULL ) )
}
#endif
	
	
	return ERRNO_SUCCESS;
}

struct ifm_l3 *pim_getbulk_if_ip(int module_id, int *pdata_num)
{
	/*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_ROUTE, module_id,
                             IPC_TYPE_L3IF, 0, IPC_OPCODE_GET_BULK, 0);*/
    int ret = 0;
	struct ipc_mesg_n *pmesg = NULL;
	
	ret = ipmc_msg_send_hal_wait_reply((void *)pmesg,0,1,MODULE_ID_ROUTE,module_id,
                             						IPC_TYPE_L3IF, 0, IPC_OPCODE_GET_BULK, 0);
    if ( NULL != pmesg )
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct ifm_l3 * ) pmesg->msg_data;
    }
	
    return NULL;
}

/*struct ifm_l3 *pim_get_if_ip(int module_id, uint32_t ifindex)
{
	struct ifm_l3 *l3if = ipc_send_common_wait_reply( NULL, 0, 1 , MODULE_ID_ROUTE, module_id,
                             IPC_TYPE_L3IF, 0, IPC_OPCODE_GET, ifindex);
	
    if ( NULL != l3if )
    {
        return ( struct ifm_l3 * ) l3if;
    }
	
    return NULL;
}*/

void pim_mrt_sg_keepalive_timer_set(struct pim_mrt_sg * mrt_sg, uint8_t set_flag)
{
	if (mrt_sg->keepalive_timer)
	{
		high_pre_timer_delete(mrt_sg->keepalive_timer);
		mrt_sg->keepalive_timer = 0;
	}

	if (set_flag)
	{
		/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
										pim_mrt_sg_keepalive_timer_expire,
										(void *)mrt_sg, 
										Keepalive_Period_default);*/
		mrt_sg->keepalive_timer = high_pre_timer_add("JPDownSGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
						pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
						1000 * Keepalive_Period_default);
	}
	
	return;
}

/*从无效变成有效*/
void pim_mrt_wc_to_used(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	uint32_t val = 0;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;
	
	IPMC_LOG_NOTICE("(*,%x) rp_addr:%x, flag:%x to used\n", 
		wc->grp_addr, wc->rp_addr, wc->flag);
	wc->flag &= ~PIM_MRT_UNUSED;
	wc->rp_addr = new_upstream->toward;
	wc->rpf_nbr = new_upstream->rpf_nbr;
	wc->upstream_ifindex = new_upstream->ifindex;
	wc->upstream_index = new_upstream->index;
	if(new_upstream->ifindex == 0 && wc->rpf_nbr == 0)
	{
		wc->flag |= PIM_MRT_LOCAL;
	}
	else
	{
		wc->flag &= ~PIM_MRT_LOCAL;
	}
	/*计算出接口*/
	pim_mrt_wc_oif_cal(wc);
	if(JoinDesiredWC(wc))
	{
		pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_T);
	}
	/*遍历相关的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp == NULL)
	{
		IPMC_LOG_NOTICE("(*,%x) has no (S, G)\n", wc->grp_addr);
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*rp改变*/
				sg->rp_addr = wc->rp_addr;
				if(PruneDesiredSGRPT(sg))
				{
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
				}
				
				if(CouldRegisterSG(sg, sg->upstream_ifindex))	
				{
					/*could reg*/
					pim_reg_state_machine(sg, PIM_REG_COULD_REG);
				}
				/*重新计算出接口*/
				pim_mrt_sg_oif_cal(sg);
				/*若下发硬件，更新表项信息*/
				if(sg->flag & PIM_MRT_CACHE)
				{
					IPMC_LOG_NOTICE("(%x, %x) unused change to used\n", sg->src_addr, sg->grp_addr);
					/*状态机切换到used*/
					
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
}

/*从有效变成无效*/
void pim_mrt_wc_to_unused(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	uint32_t val = 0;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;

	IPMC_LOG_NOTICE("(*,%x) rp_addr:%x, flag:%x to unused\n", 
		wc->grp_addr, wc->rp_addr, wc->flag);
	pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_F);
	
	/*遍历相关的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp == NULL)
	{
		IPMC_LOG_NOTICE("(*,%x) has no (S, G)\n", wc->grp_addr);
		goto out;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*(S,G,RPT) 状态机改变*/
				pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_JOINDESIRED_F);
				/*RP改变*/
				sg->rp_addr = wc->rp_addr;
				
				/*注册状态改变，源端注册false，rp端注册false*/
				if(sg->flag & PIM_MRT_REG)
				{
					if(sg->flag & PIM_MRT_LOCAL)
					{
						/*源端注册状态change to false*/
						pim_reg_state_machine(sg, PIM_REG_COULD_NOT_REG);
					}
					else
					{
						/*rp端注册状态change to false*/
						sg->flag &= ~PIM_MRT_REG;
					}
				}

				/*重新计算出接口*/
				pim_mrt_sg_oif_cal(sg);
				/*若下发硬件，更新表项信息*/
				if(sg->flag & PIM_MRT_CACHE)
				{
					IPMC_LOG_NOTICE("(%x, %x) unused change to used\n", sg->src_addr, sg->grp_addr);
					/*状态机切换到used*/
					
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
out:
	wc->flag |= PIM_MRT_UNUSED;
	wc->flag &= ~PIM_MRT_LOCAL;
	if(new_upstream)
	{
		wc->rp_addr = new_upstream->toward;
		wc->rpf_nbr = new_upstream->rpf_nbr;
		wc->upstream_ifindex = new_upstream->ifindex;
		wc->upstream_index = new_upstream->index;
	}
	else
	{
		wc->rp_addr = 0;
		wc->rpf_nbr = 0;
		wc->upstream_ifindex = 0;
		wc->upstream_index = 0;
	}
	return 	;
}

/*上游接口改变*/
void pim_mrt_wc_iif_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	struct pim_mrt_wc old_wc = {0};
	uint32_t val = 0;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;
	
	IPMC_LOG_NOTICE("(*,%x) flag:%x, rp:%x, rpf_ifindex:%x, rpf_nbr:%x\n",
		wc->grp_addr, wc->flag, wc->rp_addr, wc->upstream_ifindex, wc->rpf_nbr);
	memcpy(&old_wc, wc, sizeof(struct pim_mrt_wc));
	wc->rp_addr = new_upstream->toward;
	wc->upstream_ifindex = new_upstream->ifindex;
	wc->rpf_nbr = new_upstream->rpf_nbr;
	wc->upstream_index = new_upstream->index;
	if(wc->upstream_ifindex == 0 && wc->rpf_nbr == 0)
	{
		/*rp在本地*/
		wc->flag |= PIM_MRT_LOCAL;
	}
	else
	{
		wc->flag &= ~PIM_MRT_LOCAL;
	}
	pim_mrt_wc_oif_cal(wc);
	/*查看是否需要加枝*/
	if(JoinDesiredWC(wc))
	{
		if(wc->jp_up_state == PIM_JP_UP_STATE_JOINED)
		{
			/*向原来的上游发送剪枝，向新的上游发送加枝*/
			pim_jp_up_wc_state_machine(wc, &old_wc, PIM_JP_UP_EVENT_RPF_CH_NHP);
		}
		else
		{
			/*向新的上游发送加枝*/
			pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_T);
		}
	}
	else
	{
		/*rp在本地时不需要发送加枝*/
		pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_F);
	}
	
	/*遍历相关的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp == NULL)
	{
		IPMC_LOG_NOTICE("(*,%x) has no (S, G)\n", wc->grp_addr);
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*SGRPT 剪枝状态*/
				if(PruneDesiredSGRPT(sg))
				{
					/*sgrpt剪枝的rpf邻居change*/
					if(sg->jp_up_rptstate == PIM_JP_UP_STATE_RPT_P)
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_RPF_CH_NHP);
					}
					else
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
					}
				}
				else
				{
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
				}
				if(wc->rp_addr != sg->rp_addr)
				{
					sg->rp_addr = wc->rp_addr;
					/*sg 的rp改变，源端和rp端都需要改变*/
					if(sg->flag & PIM_MRT_LOCAL)
					{
						if(sg->flag & PIM_MRT_REG)
						{
							/*rp 变成本地*/
							if(wc->flag & PIM_MRT_LOCAL)
							{
								/*不需要注册*/
								pim_reg_state_machine(sg, PIM_REG_COULD_NOT_REG);
							}
							else
							{
								/*只是rp地址改变，注册状态不变*/
								pim_reg_state_machine(sg, PIM_REG_RP_CHANGE);
							}
						}
					}
					else if(sg->flag & PIM_MRT_REG)
					{
						/*本地非rp*/
						if(!(wc->flag & PIM_MRT_LOCAL))
						{
							IPMC_LOG_NOTICE("(*,%x) rp:%x, non-local\n", wc->grp_addr, wc->rp_addr);
							sg->flag &= ~PIM_MRT_REG;
						}
					}
				}
				if(sg->flag & PIM_MRT_CACHE)
				{
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
}

/*上游邻居改变，上游接口不变化*/
void pim_mrt_wc_rpf_nbr_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	struct pim_mrt_wc old_wc = {0};
	uint32_t val = 0;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;
	
	IPMC_LOG_NOTICE("(*,%x) flag:%x, rp:%x, rpf_ifindex:%x, rpf_nbr:%x\n",
		wc->grp_addr, wc->flag, wc->rp_addr, wc->upstream_ifindex, wc->rpf_nbr);
	memcpy(&old_wc, wc, sizeof(struct pim_mrt_wc));
	wc->rp_addr = new_upstream->toward;
	wc->upstream_ifindex = new_upstream->ifindex;
	wc->rpf_nbr = new_upstream->rpf_nbr;
	wc->upstream_index = new_upstream->index;
	if(wc->upstream_ifindex == 0 && wc->rpf_nbr == 0)
	{
		/*rp在本地*/
		wc->flag |= PIM_MRT_LOCAL;
	}
	else
	{
		wc->flag &= ~PIM_MRT_LOCAL;
	}
	pim_mrt_wc_oif_cal(wc);
	/*查看是否需要加枝*/
	if(JoinDesiredWC(wc))
	{
		/*向原来的上游发送剪枝，向新的上游发送加枝*/
		pim_jp_up_wc_state_machine(wc, &old_wc, PIM_JP_UP_EVENT_RPF_CH_NHP);
	}
	
	/*遍历相关的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp == NULL)
	{
		IPMC_LOG_NOTICE("(*,%x) has no (S, G)\n", wc->grp_addr);
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*SGRPT 剪枝状态*/
				if(PruneDesiredSGRPT(sg))
				{
					/*sgrpt剪枝的rpf邻居change*/
					if(sg->jp_up_rptstate == PIM_JP_UP_STATE_RPT_P)
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_RPF_CH_NHP);
					}
					else
					{
						pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
					}
				}
				else
				{
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
				}
				if(wc->rp_addr != sg->rp_addr)
				{
					sg->rp_addr = wc->rp_addr;
					/*sg 的rp改变，源端和rp端都需要改变*/
					if(sg->flag & PIM_MRT_LOCAL)
					{
						if(sg->flag & PIM_MRT_REG)
						{
							/*rp 变成本地*/
							if(wc->flag & PIM_MRT_LOCAL)
							{
								/*不需要注册*/
								pim_reg_state_machine(sg, PIM_REG_COULD_NOT_REG);
							}
							else
							{
								/*只是rp地址改变，注册状态不变*/
								pim_reg_state_machine(sg, PIM_REG_RP_CHANGE);
							}
						}
					}
					else if(sg->flag & PIM_MRT_REG)
					{
						/*本地非rp*/
						if(!(wc->flag & PIM_MRT_LOCAL))
						{
							IPMC_LOG_NOTICE("(*,%x) rp:%x, non-local\n", wc->grp_addr, wc->rp_addr);
							sg->flag &= ~PIM_MRT_REG;
						}
					}
				}
				if(sg->flag & PIM_MRT_CACHE)
				{
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
}


/*上游接口，上游邻居都没改变，只有rp地址改变*/
void pim_mrt_wc_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	struct pim_mrt_wc old_wc = {0};
	uint32_t val = 0;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct hash_bucket *bucket = NULL;
	struct hash_table *sg_table = NULL;
	
	IPMC_LOG_NOTICE("(*,%x) flag:%x, rp:%x, rpf_ifindex:%x, rpf_nbr:%x\n",
		wc->grp_addr, wc->flag, wc->rp_addr, wc->upstream_ifindex, wc->rpf_nbr);
	memcpy(&old_wc, wc, sizeof(struct pim_mrt_wc));
	wc->rp_addr = new_upstream->toward;
	if(wc->upstream_ifindex == 0 && wc->rpf_nbr == 0)
	{
		/*rp在本地*/
		wc->flag |= PIM_MRT_LOCAL;
	}
	else
	{
		wc->flag &= ~PIM_MRT_LOCAL;
	}
	pim_mrt_wc_oif_cal(wc);
	/*查看是否需要加枝*/
	if(JoinDesiredWC(wc))
	{
		/*向原来的上游发送剪枝，向新的上游发送加枝*/
		pim_jp_up_wc_state_machine(wc, &old_wc, PIM_JP_UP_EVENT_RPF_CH_NHP);
	}
	
	/*遍历相关的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), wc->grp_addr);
	if(sg_grp == NULL)
	{
		IPMC_LOG_NOTICE("(*,%x) has no (S, G)\n", wc->grp_addr);
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*SGRPT 剪枝状态*/
				if(PruneDesiredSGRPT(sg) && sg->jp_up_rptstate == PIM_JP_UP_STATE_RPT_P)
				{
					/*sgrpt剪枝的rpf邻居change*/
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_RPF_CH_NHP);
				}
				if(wc->rp_addr != sg->rp_addr)
				{
					sg->rp_addr = wc->rp_addr;
					/*sg 的rp改变，源端和rp端都需要改变*/
					if(sg->flag & PIM_MRT_LOCAL)
					{
						if(sg->flag & PIM_MRT_REG)
						{
							/*rp 变成本地*/
							if(wc->flag & PIM_MRT_LOCAL)
							{
								/*不需要注册*/
								pim_reg_state_machine(sg, PIM_REG_COULD_NOT_REG);
							}
							else
							{
								/*只是rp地址改变，注册状态不变*/
								pim_reg_state_machine(sg, PIM_REG_RP_CHANGE);
							}
						}
					}
					else if(sg->flag & PIM_MRT_REG)
					{
						/*本地非rp*/
						if(!(wc->flag & PIM_MRT_LOCAL))
						{
							IPMC_LOG_NOTICE("(*,%x) rp:%x, non-local\n", wc->grp_addr, wc->rp_addr);
							sg->flag &= ~PIM_MRT_REG;
						}
					}
				}
				if(sg->flag & PIM_MRT_CACHE)
				{
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
}


void pim_mrt_wc_rp_change(struct pim_mrt_wc *wc, struct pim_mrt_upstream *new_upstream)
{
	if(wc->flag & PIM_MRT_UNUSED)
	{
		if(new_upstream && new_upstream->flag == TRUE)
		{
			/*从无效变有效*/
			pim_mrt_wc_to_used(wc, new_upstream);
		}
		else
		{
			/*从无效变无效*/
			wc->rp_addr = pim_instance_global->rp_addr;
			return ;
		}
	}
	else
	{
		if(new_upstream && new_upstream->flag == TRUE)
		{
			/*从有效变有效*/
			if(wc->upstream_ifindex != new_upstream->ifindex)
			{
				/*上游接口改变*/
				pim_mrt_wc_iif_change(wc, new_upstream);
			}
			else if(wc->rpf_nbr != new_upstream->rpf_nbr)
			{
				/*上游邻居改变*/
				pim_mrt_wc_rpf_nbr_change(wc, new_upstream);
			}
			else
			{
				pim_mrt_wc_change(wc, new_upstream);
			}
		}
		else
		{
			/*从有效变无效*/
			pim_mrt_wc_to_unused(wc, new_upstream);
		}
	}
}

void pim_mrt_rp_change(struct pim_instance *ppim)
{
	/*rp从无到有，或者rp改变*/
	/*遍历所有的(*,G)*/
	struct hash_table *wc_table = &(pim_instance_global->pim_wc_table);
	struct hash_bucket *pbucket = NULL;
	struct pim_mrt_wc *wc = NULL, *new_wc = NULL;
	struct pim_mrt_upstream *new_upstream = NULL, *old_upstream = NULL;
	uint32_t i = 0, new_rp = 0, cnt = 0;

	new_rp = ppim->rp_addr;
	if(new_rp == 0)
	{
		/*rp被删除*/
		IPMC_LOG_NOTICE("rp has been deleted\n");
	}
	else
	{
		IPMC_LOG_NOTICE("rp change to %x\n", new_rp);
		new_upstream = pim_mrt_upstream_get(new_rp, PIM_MRT_UPSTREAM_TO_RP);
	}
	for(i = 0; i < HASHTAB_SIZE; i++)
	{
		pbucket = wc_table->buckets[i];
		while(pbucket)
		{
			wc = (struct pim_mrt_wc *)pbucket->data;
			if(wc && wc->rp_addr != pim_rp_check(wc->grp_addr))
			{
				pim_mrt_wc_rp_change(wc, new_upstream);
			}
			if(++cnt >= wc_table->num_entries)
			{
				return ;
			}
			pbucket = pbucket->next;
		}
	}
	/*删除旧rp*/
	return;
}

void pim_mrt_swt_state_machine(struct pim_mrt_sg *mrt_sg, uint32_t event)
{
	switch(event)
	{
		case PIM_MRT_SWT_RCV_DATA:
			/* 与接收者直连 */
			if(mrt_sg && mrt_sg->wc_mrt && (mrt_sg->wc_mrt->flag & PIM_MRT_CONNECTED))	
			{
				/*rpt向spt切换*/
				mrt_sg->flag |= PIM_MRT_SWT;
				if(JoinDesiredSG(mrt_sg))
				{
					pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
					Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
					if(mrt_sg->flag & PIM_MRT_SPT)
					{
						pim_mrt_sg_oif_cal(mrt_sg);
						if(mrt_sg->flag & PIM_MRT_CACHE)
						{
							pim_mrt_sg_cache_add(mrt_sg);
						}
					}
				}
				/* 更新incoming,切换上流状态机 */
				if(PruneDesiredSGRPT(mrt_sg))
				{
					/* 发送sg rpt剪枝,启动sg rpt状态机s */
					pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);				
				}
			}
			break;
		case PIM_MRT_SWT_RCV_IGMP:
			/*igmp存在，则与接收者直连，可以进行切换*/
			if(!(mrt_sg->flag & PIM_MRT_SWT))
			{
				/*切换到spt，设置swt标志位*/
				if(SwitchToSptDesired(mrt_sg))
				{
					mrt_sg->flag |= PIM_MRT_SWT;
					if(JoinDesiredSG(mrt_sg))
					{
						pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
						Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
						if(mrt_sg->flag & PIM_MRT_SPT)
						{
							pim_mrt_sg_oif_cal(mrt_sg);
						}
					}
					/* 更新incoming,切换上流状态机 */
					if(PruneDesiredSGRPT(mrt_sg))
					{
						/* 发送sg rpt剪枝,启动sg rpt状态机s */
						pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);				
					}
					pim_mrt_sg_oif_cal(mrt_sg);
					if(mrt_sg->flag & PIM_MRT_CACHE)
					{
						pim_mrt_sg_cache_add(mrt_sg);
					}
				}
			}
			break;
		case PIM_MRT_SWT_NOPKT:
			/*在进行spt切换时，没有收到数据报文*/
			if(mrt_sg->flag & PIM_MRT_SWT)
			{
				mrt_sg->flag &= ~PIM_MRT_SWT;
				/*不与源直连且不与接收者直连且不是rp*/
				if(IF_ISZERO(&mrt_sg->oif.join_oif) && IF_ISZERO(&mrt_sg->oif.pim_in_oif)
					 && !(mrt_sg->flag & PIM_MRT_REG) && !(mrt_sg->flag & PIM_MRT_LOCAL))
			 	{
			 		Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
			 		pim_mrt_sg_oif_cal(mrt_sg);
                    if(!JoinDesiredSG(mrt_sg))
                    {
			 		    pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
                    }                    
					if(!PruneDesiredSGRPT(mrt_sg))
					{
						pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);	
					}
					if(mrt_sg->flag & PIM_MRT_CACHE)
					{
						pim_mrt_sg_cache_add(mrt_sg);
					}
				}
			}
			break;
		case PIM_MRT_SWT_RCV_LEAVE:
			/*收到igmp离组报文，非接收者直连*/
			if(mrt_sg->flag & PIM_MRT_SWT)
			{
				if(!SwitchToSptDesired(mrt_sg))
				{
					mrt_sg->flag &= ~PIM_MRT_SWT;
					if(IF_ISZERO(&mrt_sg->oif.join_oif) && IF_ISZERO(&mrt_sg->oif.pim_in_oif)
					 && !(mrt_sg->flag & PIM_MRT_REG) && !(mrt_sg->flag & PIM_MRT_LOCAL))
					{
						Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
			 		    pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
						pim_mrt_sg_oif_cal(mrt_sg);
						if(!PruneDesiredSGRPT(mrt_sg))
						{
							pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F); 
						}
						if(mrt_sg->flag & PIM_MRT_CACHE)
						{
							pim_mrt_sg_cache_add(mrt_sg);
						}
					}
				}
			}
			break;
		case PIM_MRT_SWT_UNUSED:
			if(mrt_sg->flag & PIM_MRT_SWT)
			{
				mrt_sg->flag &= ~PIM_MRT_SWT;
			}
			break;
		case PIM_MRT_SWT_USED:
			if(SwitchToSptDesired(mrt_sg))
			{
				mrt_sg->flag |= PIM_MRT_SWT;
				if(JoinDesiredSG(mrt_sg))
				{
					pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
					Update_SPTbit(mrt_sg, mrt_sg->upstream_ifindex);
					pim_mrt_sg_oif_cal(mrt_sg);
					if(mrt_sg->flag & PIM_MRT_CACHE)
					{
						pim_mrt_sg_cache_add(mrt_sg);
					}
				}
				/* 更新incoming,切换上流状态机 */
				if(PruneDesiredSGRPT(mrt_sg))
				{
					/* 发送sg rpt剪枝,启动sg rpt状态机s */
					pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T); 
				}
			}
			break;
		default:
			break;
	}
}

/*igmp有新的组成员加入*/
void pim_mrt_igmp_mem_add(struct ipmc_if *pif, uint32_t grp)
{
	/*某接口下游igmp组成员*/
	/*如果设备开启了pim协议，就可以创建(*,G)表项*/
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct hash_table *sg_table = NULL;
	uint32_t val = 0;
	struct hash_bucket *bucket =NULL;

	if(pif == NULL)
	{
		IPMC_LOG_ERROR("pif == NULL\n");
	}
	wc = pim_mrt_wc_get(grp);
	if(wc == NULL)
	{
		IPMC_LOG_DEBUG("pim_mrt_wc_get(%x) return NULL\n", grp);
		return ;
	}
	/*与接收者直连*/
	wc->flag |= PIM_MRT_CONNECTED;
	/*在wc表项中添加出接口*/
	if(IF_ISSET(pif->index, &(wc->oif.join_oif)))
	{
		return ;
	}
	IF_SET(pif->index, &(wc->oif.join_oif));
	/*新增加成员出接口，重新计算出接口信息*/
	pim_mrt_wc_oif_cal(wc);
	if(JoinDesiredWC(wc))
	{
		pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_T);
	}
	/*查找该组下的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), grp);
	if(sg_grp == NULL)
	{
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*重新计算出接口*/
				pim_mrt_sg_oif_cal(sg);
				if(JoinDesiredSG(sg))
				{
					pim_jp_up_sg_state_machine(sg, NULL,PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
				}
				if(PruneDesiredSGRPT(sg))
				{
					pim_jp_up_sg_rpt_state_machine(sg,PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
				}
				else
				{
					pim_jp_up_sg_rpt_state_machine(sg,PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
				}
				/*重新下硬件表项*/
				if(sg->flag & PIM_MRT_CACHE)
				{
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
}

/*igmp有新的组成员加入*/
void pim_mrt_igmp_mem_del(struct ipmc_if *pif, uint32_t grp)
{
	/*某接口下游igmp组成员*/
	/*如果设备开启了pim协议，就可以创建(*,G)表项*/
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_sg *sg = NULL;
	struct pim_mrt_sg_grp *sg_grp = NULL;
	struct hash_table *sg_table = NULL;
	uint32_t val = 0;
	struct hash_bucket *bucket =NULL;
	
	wc = pim_mrt_wc_get(grp);
	if(wc == NULL)
	{
		IPMC_LOG_DEBUG("pim_mrt_wc_get(%x) return NULL\n", grp);
		return ;
	}
	/*在wc表项中添加出接口*/
	if(!IF_ISSET(pif->index, &(wc->oif.join_oif)))
	{
		return ;
	}
	IF_CLR(pif->index, &(wc->oif.join_oif));
	/*新增加成员出接口，重新计算出接口信息*/
	pim_mrt_wc_oif_cal(wc);
	if(!JoinDesiredWC(wc))
	{
		pim_jp_up_wc_state_machine(wc, NULL, PIM_JP_UP_EVENT_WC_JOINDESIRED_F);
	}
	/*查找该组下的sg*/
	sg_grp = pim_mrt_sg_grp_lookup(&(pim_instance_global->pim_sg_table), grp);
	if(sg_grp == NULL)
	{
		return ;
	}
	sg_table = &(sg_grp->sg_table);
	
	for(val = 0; val < HASHTAB_SIZE; val++)
	{
		for((bucket = sg_table->buckets[val]); bucket; bucket = bucket->next)
		{
			sg = (struct pim_mrt_sg *)bucket->data;
			if(sg)
			{
				/*重新计算出接口*/
				pim_mrt_sg_oif_cal(sg);
				if(JoinDesiredSG(sg))
				{
					pim_jp_up_sg_state_machine(sg, NULL,PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
				}
				if(PruneDesiredSGRPT(sg))
				{
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
				}
				else
				{
					pim_jp_up_sg_rpt_state_machine(sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F);
				}
				/*重新下硬件表项*/
				if(sg->flag & PIM_MRT_CACHE)
				{
					pim_mrt_sg_cache_add(sg);
				}
			}
		}
	}
	
	/*(*G)表项无效时被删除*/
	if(wc->oif_num == 0)
	{
		pim_mrt_wc_del(wc);
	}
}

uint32_t I_Am_Assert_Loser(struct pim_mrt_sg * mrt_sg, uint32_t iif)
{
	/*
	I_Am_Assert_Loser(S, G, I) is true if the Assert state machine (in
	  Section 4.6.1) for (S,G) on Interface I is in "I am Assert Loser"
	  state.
	
	  I_Am_Assert_Loser(*, G, I) is true if the Assert state machine (in
	  Section 4.6.2) for (*,G) on Interface I is in "I am Assert Loser"
	  state.
	*/
	return IPMC_FALSE;
}

/*DirectlyConnectedS == PIM_MRT_LOCAL*/
uint32_t DirectlyConnectedS(uint32_t src, uint32_t iif)
{
	struct ifm_l3 l3if;
	int ret = 0;

	ret = ifm_get_l3if(iif, MODULE_ID_IPMC, &l3if);
	if (ipv4_is_same_subnet(src, l3if.ipv4[0].addr, l3if.ipv4[0].prefixlen) ||
		ipv4_is_same_subnet(src, l3if.ipv4[1].addr, l3if.ipv4[1].prefixlen))
	{
		return IPMC_TRUE;
	}
	return IPMC_FALSE;
}
/*更新spt标志位的时机:
1. JoinDesiredSG
2. inherited_olist_sgrpt变化
3. rpf_nbr 变化
4. upstream_ifindex变化
5. PIM_MRT_LOCAL变化
*/
void Update_SPTbit(struct pim_mrt_sg * mrt_sg, uint32_t iif)
{
	struct pim_mrt_wc * mrt_wc = mrt_sg->wc_mrt;
	
	if( iif == mrt_sg->upstream_ifindex && JoinDesiredSG(mrt_sg) == IPMC_TRUE &&
		   ((mrt_sg->flag & PIM_MRT_LOCAL) ||
			(mrt_sg->upstream_ifindex != mrt_wc->upstream_ifindex) || 
			(inherited_olist_sgrpt(mrt_sg) == IPMC_FALSE) ||
			(mrt_sg->rpf_nbr == mrt_wc->rpf_nbr && mrt_sg->rpf_nbr != 0) ||
			(I_Am_Assert_Loser(mrt_sg,iif))
			)
	  )
	{
		mrt_sg->flag |= PIM_MRT_SPT;
	}
	else
	{
		mrt_sg->flag &= ~PIM_MRT_SPT;
	}
}

uint32_t pim_local_reciever(struct pim_mrt_sg * sg)
{
	uint32_t i = 0;
	struct pim_mrt_wc * wc = sg->wc_mrt;

	if(wc)
	{
		for( i = 0; i < IFBIT_SIZE; i++)
		{
			if((wc->oif.join_oif.if_bits[i] & (~(sg->oif.prune_oif.if_bits[i]))) | 
				(sg->oif.pim_in_oif.if_bits[i]))
			{
				return IPMC_TRUE;
			}
		}
	}
	else
	{
		for( i = 0; i < IFBIT_SIZE; i++)
		{
			if(sg->oif.pim_in_oif.if_bits[i])
			{
				return IPMC_TRUE;
			}
		}
	}
	return IPMC_FALSE;
}

uint32_t SwitchToSptDesired(struct pim_mrt_sg * sg)
{
	/*
	SwitchToSptDesired(S,G) is a policy function that is implementation
	   defined.  An "infinite threshold" policy can be implemented by making
	   SwitchToSptDesired(S,G) return false all the time.  A "switch on
	   first packet" policy can be implemented by making
	   SwitchToSptDesired(S,G) return true once a single packet has been
	   received for the source and group.
	*/
	return IPMC_TRUE;
}

uint32_t CheckSwitchToSpt(struct pim_mrt_sg * mrt_sg) 
{
	if(pim_local_reciever(mrt_sg) && SwitchToSptDesired(mrt_sg)) 
	{
		if(mrt_sg->keepalive_timer)
		{
			high_pre_timer_delete(mrt_sg->keepalive_timer);
		}
		/*mrt_sg->keepalive_timer = thread_add_timer(ipmc_master, 
										pim_mrt_sg_keepalive_timer_expire,
										(void *)mrt_sg, 
										Keepalive_Period_default);*/
		mrt_sg->keepalive_timer = high_pre_timer_add("JPDownSGKeepaliveTimer",LIB_TIMER_TYPE_NOLOOP,
						pim_mrt_sg_keepalive_timer_expire,(void *)mrt_sg,
						1000 * Keepalive_Period_default);
		/*表示正在进行切换*/
		mrt_sg->flag |= PIM_MRT_SWT;
		/*rpt切换到spt，需要发送sg加枝，sgrpt剪枝*/
		if(JoinDesiredSG(mrt_sg))
		{
			pim_jp_up_sg_state_machine(mrt_sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
		}
		if(pim_prune_desired_sgrpt(mrt_sg))
		{
			pim_jp_up_sg_rpt_state_machine(mrt_sg, PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T);
		}
		if(!(mrt_sg->flag & PIM_MRT_SPT))
		{
			mrt_sg->flag |= PIM_MRT_SPT;
			/*第一次切换成spt，计算出接口，下发芯片表项*/
			pim_mrt_sg_oif_cal(mrt_sg);
			if(!(mrt_sg->flag & PIM_MRT_CACHE))
			{
				/*表项下发芯片*/
				pim_mrt_sg_cache_add(mrt_sg);
			}
		}
		return IPMC_TRUE;
	}
	else
	{
		/*不满足spt切换时*/
		if(mrt_sg->flag & PIM_MRT_SWT)
		{
			mrt_sg->flag &= ~PIM_MRT_SWT;
			
		}
	}
	return IPMC_FALSE;
}

uint32_t RPTJoinDesired(struct pim_mrt_sg * mrt_sg)
{
	if(mrt_sg == NULL || mrt_sg->wc_mrt == NULL)
	{
		IPMC_LOG_ERROR("mrt_sg == NULL || mrt_sg->wc_mrt == NULL\n");
		return IPMC_FALSE;
	}
	return JoinDesiredWC(mrt_sg->wc_mrt);
}

/*
bool PruneDesired(S,G,rpt) {
          return ( RPTJoinDesired(G) AND
                   ( inherited_olist(S,G,rpt) == NULL
                     OR (SPTbit(S,G)==TRUE
                         AND (RPF'(*,G) != RPF'(S,G)) )))
     }
*/
uint32_t PruneDesiredSGRPT(struct pim_mrt_sg * mrt_sg)
{
	struct pim_mrt_wc *mrt_wc = mrt_sg->wc_mrt;
	uint32_t wc_sg_rpf_diff = IPMC_FALSE;
	
	if(mrt_wc && mrt_wc->rpf_nbr != mrt_sg->rpf_nbr)
	{
		wc_sg_rpf_diff = IPMC_TRUE;
	}
	if(RPTJoinDesired(mrt_sg) && 
		(!inherited_olist_sgrpt(mrt_sg) || (mrt_sg->flag & PIM_MRT_SPT && wc_sg_rpf_diff)))
	{
		return IPMC_TRUE;
	}
	return IPMC_FALSE;
}

uint32_t JoinDesiredWC(struct pim_mrt_wc * wc)
{
/*
bool JoinDesired(*,G) {
        if (immediate_olist(*,G) != NULL OR
            (JoinDesired(*,*,RP(G)) AND
             AssertWinner(*, G, RPF_interface(RP(G))) != NULL))
            return TRUE
        else
            return FALSE
     }
*/	
	if(wc && !(wc->flag & PIM_MRT_UNUSED) && immediate_olist_wc(wc) && !(wc->flag & PIM_MRT_PRUNED))
	{
		return IPMC_TRUE;
	}
	return IPMC_FALSE;
}

/*
bool JoinDesired(S,G) {
         return( immediate_olist(S,G) != NULL
                 OR ( KeepaliveTimer(S,G) is running
                      AND inherited_olist(S,G) != NULL ) )
     }
这个会影响spt标志位，上游状态机
*/
uint32_t JoinDesiredSG(struct pim_mrt_sg * mrt_sg)
{
	if(mrt_sg)
	{
		if(mrt_sg->flag & PIM_MRT_UNUSED)
		{
			return IPMC_FALSE;
		}
		else if(immediate_olist_sg(mrt_sg) || (mrt_sg->keepalive_timer && inherited_olist_sg(mrt_sg)))
		{
			return IPMC_TRUE;
		}
	}
	return IPMC_FALSE;
}
