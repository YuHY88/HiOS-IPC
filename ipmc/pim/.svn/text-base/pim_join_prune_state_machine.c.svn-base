/*
*   PIM join/prune状态机相关处理
*/

#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/pkt_buffer.h>
#include <lib/checksum.h>

#include "pim.h"
#include "pim_pkt.h"
#include "pim_join_prune.h"
#include "pim_mroute.h"
#include "../ipmc_main.h"


uint32_t pim_jp_down_prune_delay(struct pim_neighbor *pnbr)
{
	struct pim_neighbor_node *pnbr_node = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
	
	for (ALL_LIST_ELEMENTS(&pnbr->neighbor_list, pnode, pnextnode, pnbr_node))
	{
		if(!(pnbr_node->opt_flag & PIM_HELLO_LAN_DELAY_PRESENT))
		{
			return FALSE;
		}
	}
	
	return TRUE;
}

uint16_t pim_jp_down_sg_grp_node_holdtime_get(struct ipmc_if *pif, uint32_t grp_addr)
{
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;

	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), grp_addr);
	if(sg_nodes)
	{
		return sg_nodes->holdtime;
	}
	return 0;
}

/*sint32 pim_jp_down_grp_node_et_timer_expire(struct thread *thread)*/
sint32 pim_jp_down_grp_node_et_timer_expire(void *para)
{
	uint32_t event = 0;
	struct pim_down_jp_grp_node *grp_node = NULL;

	grp_node = (struct pim_down_jp_grp_node *)para;
	grp_node->et_timer = 0;
	event = PIM_JP_DOWN_EVENT_WC_ET_EX;
	
	pim_jp_down_wc_state_machine(grp_node, event);
	return ERRNO_SUCCESS;
}

/*sint32 pim_jp_down_grp_node_ppt_timer_expire(struct thread *thread)*/
sint32 pim_jp_down_grp_node_ppt_timer_expire(void *para)
{
	uint32_t event = 0;
	struct pim_down_jp_grp_node *grp_node = NULL;

	/*grp_node = (struct pim_down_jp_grp_node *)(thread->arg);*/
	grp_node = (struct pim_down_jp_grp_node *)para;
	grp_node->ppt_timer = 0;
	event = PIM_JP_DOWN_EVENT_WC_PPT_EX;
	
	pim_jp_down_wc_state_machine(grp_node, event);
	
	return ERRNO_SUCCESS;
}

void pim_jp_down_wc_sgrpt_change(struct pim_down_jp_grp_node *grp_node, uint32_t event)
{
	uint32_t cursor = 0;
	struct ipmc_if *pif = NULL;
    struct hash_bucket *pbucket = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node *sg_rpt_node = NULL;

	/*遍历相关的sg_node*/
	pif = grp_node->pif;
	/*根据grp_addr获取到sgrpt_nodes*/
	
	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), grp_node->grp_addr);
	if(sg_nodes == NULL)
	{
		return ;
	}
	HASH_BUCKET_LOOP(pbucket, cursor, sg_nodes->sg_table)
	{
		if(pbucket->data)
		{
			sg_rpt_node = (struct pim_down_jp_sg_node *)pbucket->data;
			if(sg_rpt_node->type & PIM_JP_SG_RPT_TYPE)
			{
				pim_jp_down_sg_rpt_state_machine(sg_rpt_node, event);
			}
		}
	}
}

void pim_jp_down_wc_ni(struct pim_down_jp_grp_node *grp_node, uint32_t event)
{
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_WC_J_RCV:
			grp_node->state = PIM_JP_DOWN_STATE_J;
			/*启动定时器et，设置为holdtime*/
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
			}
			/*grp_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_grp_node_et_timer_expire,(void *)grp_node, grp_node->holdtime);*/
			grp_node->et_timer = high_pre_timer_add("JPDownWCETtimer",LIB_TIMER_TYPE_NOLOOP,
										pim_jp_down_grp_node_et_timer_expire,(void *)grp_node,1000 * grp_node->holdtime);

			/*收到*G加枝，影响同一个G的(S,G,RPT)状态机*/
			pim_jp_down_wc_sgrpt_change(grp_node, event);

			/*收到*G加枝后，计算(*,G)表项的出接口*/
			pim_mrt_wc_oif_add(grp_node->pif, grp_node);
			break;
		case PIM_JP_DOWN_EVENT_WC_P_RCV:
			grp_node->state = PIM_JP_DOWN_STATE_J;
			
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
			}
			grp_node->et_timer = 0;
			
			if(grp_node->ppt_timer)
			{
				high_pre_timer_delete(grp_node->ppt_timer);
			}
			grp_node->ppt_timer = 0;
			break;
		case PIM_JP_DOWN_EVENT_WC_PPT_EX:
			break;
		case PIM_JP_DOWN_EVENT_WC_ET_EX:
			break;
		default:
			break;
	}
}

void pim_jp_down_wc_j(struct pim_down_jp_grp_node *grp_node, uint32_t event)
{
	struct ipmc_if *pif = NULL;
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_WC_J_RCV:
			grp_node->state = PIM_JP_DOWN_STATE_J;
			/* 启动定时器et，设置为max(holdtime, et_timer) ,暂时都设置为holdtime*/
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
			}
			/*grp_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_grp_node_et_timer_expire,(void *)grp_node, grp_node->holdtime);*/
			grp_node->et_timer = high_pre_timer_add("JPDownWCETtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_down_grp_node_et_timer_expire,(void *)grp_node,1000 * grp_node->holdtime);

			/*收到*G加枝，影响(S,G,RPT)状态机*/
			pim_jp_down_wc_sgrpt_change(grp_node, event);
			break;
		case PIM_JP_DOWN_EVENT_WC_P_RCV:
			grp_node->state = PIM_JP_DOWN_STATE_PP;
			pif = grp_node->pif;
			if(grp_node->ppt_timer)
			{
				high_pre_timer_delete(grp_node->ppt_timer);
			}
			/* set to the J/P_Override_Interval(I) if the router has more than one neighbor */
			if(pif->pim_nbr->nbr_num > 1 && pim_jp_down_prune_delay(pif->pim_nbr))
			{
				/*grp_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_grp_node_ppt_timer_expire, (void *)grp_node, 
					(pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override)/1000);*/
				grp_node->ppt_timer = high_pre_timer_add("JPDownWCPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_grp_node_ppt_timer_expire,(void *)grp_node,
								(pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override));
			}
			else
			{
				/*只有一个邻居的情况下设置ppt timer立即超时*/
				/*grp_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_grp_node_ppt_timer_expire, (void *)grp_node, 0);*/
				grp_node->ppt_timer = high_pre_timer_add("JPDownWCPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_grp_node_ppt_timer_expire,(void *)grp_node,
								0);
			}
			break;
		case PIM_JP_DOWN_EVENT_WC_PPT_EX:
			break;
		case PIM_JP_DOWN_EVENT_WC_ET_EX:
			grp_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*加枝超时，删除*G出接口*/
			pim_mrt_wc_oif_del(grp_node->pif, grp_node);
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			grp_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
				grp_node->et_timer = 0;
			}
			pim_mrt_wc_oif_del(grp_node->pif, grp_node->grp_addr);
			/*删除grp_node*/
			
		default:
			break;
	}
}

void pim_jp_down_wc_pp(struct pim_down_jp_grp_node *grp_node, uint32_t event)
{
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_WC_J_RCV:
			grp_node->state = PIM_JP_DOWN_STATE_J;
			/* 取消定时器ppt*/
			if(grp_node->ppt_timer)
			{
				high_pre_timer_delete(grp_node->ppt_timer);
			}
			grp_node->ppt_timer = 0;
			/* 启动定时器et，设置为max(holdtime, et_timer) ,暂时都设置为holdtime*/
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
			}
			/*grp_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_grp_node_et_timer_expire,(void *)grp_node, grp_node->holdtime);*/
			grp_node->et_timer = high_pre_timer_add("JPDownWCETtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_grp_node_et_timer_expire,(void *)grp_node,
								1000 * grp_node->holdtime);
			/*收到*G加枝，影响(S,G,RPT)状态机*/
			pim_jp_down_wc_sgrpt_change(grp_node, event);

			break;
		case PIM_JP_DOWN_EVENT_WC_P_RCV:
			break;
		case PIM_JP_DOWN_EVENT_WC_PPT_EX:
			grp_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*Send PruneEcho(S,G)*/
			
			pim_mrt_wc_oif_del(grp_node->pif, grp_node);
			break;
		case PIM_JP_DOWN_EVENT_WC_ET_EX:
			grp_node->state = PIM_JP_DOWN_STATE_NOINFO;
			pim_mrt_wc_oif_del(grp_node->pif, grp_node);
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			grp_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(grp_node->et_timer)
			{
				high_pre_timer_delete(grp_node->et_timer);
				grp_node->et_timer = 0;
			}
			if(grp_node->ppt_timer)
			{
				high_pre_timer_delete(grp_node->ppt_timer);
				grp_node->ppt_timer = 0;
			}
			pim_mrt_wc_oif_del(grp_node->pif, grp_node->grp_addr);
			/*删除grp_node */
			
		default:
			break;
	}
}
/*
+------------++-----------------------------------------+
|                     ||                         Event                                     |
|  ++-------------+--------------+-------------+---------+
|Prev State   ||Receive      | Receive       | Prune-      | Expiry Timer|
|                 ||Join(*,G)    | Prune(*,G)   | Pending     | Expires       |
|                 ||                |                   | Timer        |                  |
|                 ||                |                   | Expires      |                  |
+------------++-------------+--------------+-------------+
|                 ||-> J state   | -> NI state  | -             | -               |
|NoInfo (NI) ||start Expiry  |                   |                |                  |
|                 ||Timer         |                    |               |                   |
+------------++-------------+--------------+-------------+
|                 ||-> J state    | -> PP state  | -            | -> NI state |
|Join (J)       ||restart         | start Prune- |               |                   |
|                 ||Expiry Timer | Pending       |               |                   |
|                 ||                  | Timer          |               |                   |
+------------++-------------+--------------+-------------+-
|Prune-        ||-> J state   | -> PP state  | -> NI state | -> NI state |
|Pending (PP)||restart        |                   | Send Prune-|                   |
|                 ||Expiry Timer |                   | Echo(*,G)    |                  |
+------------++-------------+--------------+-------------+-
*/
void pim_jp_down_wc_state_machine(struct pim_down_jp_grp_node *grp_node, uint32_t event)
{
	if(!grp_node || !grp_node->pif)
	{
		return ;
	}
	switch(grp_node->state)
	{
		case PIM_JP_DOWN_STATE_NOINFO:
			pim_jp_down_wc_ni(grp_node, event);
			break;
		case PIM_JP_DOWN_STATE_J:
			pim_jp_down_wc_j(grp_node, event);
			break;
		case PIM_JP_DOWN_STATE_PP:
			pim_jp_down_wc_pp(grp_node, event);
			break;
		default:
			break;
	}
}

/*sint32 pim_jp_down_sg_node_et_timer_expire(struct thread *thread)*/
sint32 pim_jp_down_sg_node_et_timer_expire(void *para)
{
	uint32_t event = 0;
	struct pim_down_jp_sg_node *sg_node = NULL;

	sg_node = (struct pim_down_jp_sg_node *)para;
	sg_node->et_timer = 0;
	event = PIM_JP_DOWN_EVENT_SG_ET_EX;
	
	pim_jp_down_sg_state_machine(sg_node, event);
	return ERRNO_SUCCESS;
}

/*sint32 pim_jp_down_sg_node_ppt_timer_expire(struct thread *thread)*/
sint32 pim_jp_down_sg_node_ppt_timer_expire(void *para)
{
	uint32_t event = 0;
	struct pim_down_jp_sg_node *sg_node = NULL;

	sg_node = (struct pim_down_jp_sg_node *)para;
	sg_node->ppt_timer = 0;
	event = PIM_JP_DOWN_EVENT_SG_PPT_EX;
	
	pim_jp_down_sg_state_machine(sg_node, event);
	return ERRNO_SUCCESS;
}

void pim_jp_down_sg_ni(struct pim_down_jp_sg_node *sg_node, uint32_t event)
{
	uint16_t holdtime = 0;
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_J_RCV:
			sg_node->state = PIM_JP_DOWN_STATE_J;
			/* 启动et timer，设置为报文中的holdtime */
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sg_node->pif, sg_node->grp_addr);
			if(sg_node->et_timer)
			{
				high_pre_timer_delete(sg_node->et_timer);
			}
			/*sg_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sg_node_et_timer_expire,(void *)sg_node, holdtime);*/
			sg_node->et_timer = high_pre_timer_add("JPDownSGETtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sg_node_et_timer_expire,(void *)sg_node,
								1000 * holdtime);
			pim_mrt_sg_oif_add(sg_node->pif, sg_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_P_RCV:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_j(struct pim_down_jp_sg_node *sg_node, uint32_t event)
{
	uint16_t holdtime = 0;
	struct ipmc_if *pif = NULL;

	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_J_RCV:
			sg_node->state = PIM_JP_DOWN_STATE_J;
			/* 重启et timer，设置为报文中的holdtime */
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sg_node->pif, sg_node->grp_addr);
			if(sg_node->et_timer)
			{
				high_pre_timer_delete(sg_node->et_timer);
			}
			/*sg_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sg_node_et_timer_expire,(void *)sg_node, holdtime);*/
			sg_node->et_timer = high_pre_timer_add("JPDownSGETtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sg_node_et_timer_expire,(void *)sg_node,
								1000 * holdtime);
			break;
		case PIM_JP_DOWN_EVENT_SG_P_RCV:
			sg_node->state = PIM_JP_DOWN_STATE_PP;
			/* 启动ppt timer */
			pif = sg_node->pif;
			if(sg_node->ppt_timer)
			{
				high_pre_timer_delete(sg_node->ppt_timer);
			}
			/* set to the J/P_Override_Interval(I) if the router has more than one neighbor */
			if(pif->pim_nbr->nbr_num > 1 && pim_jp_down_prune_delay(pif->pim_nbr))
			{
				/*sg_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_sg_node_ppt_timer_expire, (void *)sg_node, 
					(pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override)/1000);*/
				sg_node->ppt_timer = high_pre_timer_add("JPDownSGPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sg_node_ppt_timer_expire,(void *)sg_node,
								pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override);
			}
			else
			{
				/*只有一个邻居的情况下设置ppt timer立即超时*/
				/*sg_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_sg_node_ppt_timer_expire, (void *)sg_node, 0);*/
				sg_node->ppt_timer = high_pre_timer_add("JPDownSGPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sg_node_ppt_timer_expire,(void *)sg_node,
								0);
			}
			break;
		case PIM_JP_DOWN_EVENT_SG_ET_EX:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/* 删除(S,G)出接口*/
			//pim_mrt_sg_oif_del(pif, sg_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_PPT_EX:
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(sg_node->et_timer)
			{
				high_pre_timer_delete(sg_node->et_timer);
			}
			sg_node->et_timer = 0;
			/* 删除(S,G)出接口*/
			//pim_mrt_sg_oif_del(pif, sg_node);
			/*删除sg_node*/
			
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_pp(struct pim_down_jp_sg_node *sg_node, uint32_t event)
{
	uint16_t holdtime = 0;
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_J_RCV:
			sg_node->state = PIM_JP_DOWN_STATE_J;
			if(sg_node->ppt_timer)
			{
				high_pre_timer_delete(sg_node->ppt_timer);
			}
			sg_node->ppt_timer = 0;
			/*设置et timer*/
			if(sg_node->et_timer)
			{
				high_pre_timer_delete(sg_node->et_timer);
			}
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sg_node->pif, sg_node->grp_addr);
			/*sg_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sg_node_et_timer_expire,(void *)sg_node, holdtime);*/
			sg_node->et_timer = high_pre_timer_add("JPDownSGETtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sg_node_et_timer_expire,(void *)sg_node,
								holdtime*1000);
			break;
		case PIM_JP_DOWN_EVENT_SG_P_RCV:
			break;
		case PIM_JP_DOWN_EVENT_SG_ET_EX:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			//pim_mrt_sg_oif_del(sg_node->pif, sg_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_PPT_EX:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*Send PruneEcho(S,G)*/
			
			pim_mrt_sg_oif_del(sg_node->pif, sg_node->src_addr, sg_node->grp_addr);
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(sg_node->et_timer)
			{
				high_pre_timer_delete(sg_node->et_timer);
			}
			sg_node->et_timer = 0;
			if(sg_node->ppt_timer)
			{
				high_pre_timer_delete(sg_node->ppt_timer);
			}
			sg_node->ppt_timer = 0;
			//pim_mrt_sg_oif_del(sg_node->pif, sg_node);
			/*删除sg_node*/
			
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_state_machine(struct pim_down_jp_sg_node *sg_node, uint32_t event)
{
	if(!sg_node || !sg_node->pif)
	{
		return ;
	}
	switch(sg_node->state)
	{
		case PIM_JP_DOWN_STATE_NOINFO:
			pim_jp_down_sg_ni(sg_node, event);
			break;
		case PIM_JP_DOWN_STATE_J:
			pim_jp_down_sg_j(sg_node, event);
			break;
		case PIM_JP_DOWN_STATE_PP:
			pim_jp_down_sg_pp(sg_node, event);
			break;
		default:
			break;
	}
}

sint32 pim_jp_down_sgrpt_node_et_timer_expire(struct thread *thread)
{
	uint32_t event = 0;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	sgrpt_node = (struct pim_down_jp_sg_node *)(thread->arg);
	sgrpt_node->et_timer = 0;
	event = PIM_JP_DOWN_EVENT_SG_RPT_ET_EX;
	
	pim_jp_down_sg_rpt_state_machine(sgrpt_node, event);
	return ERRNO_SUCCESS;
}

sint32 pim_jp_down_sgrpt_node_ppt_timer_expire(struct thread *thread)
{
	uint32_t event = 0;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	sgrpt_node = (struct pim_down_jp_sg_node *)(thread->arg);
	sgrpt_node->ppt_timer = 0;
	event = PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX;
	
	pim_jp_down_sg_rpt_state_machine(sgrpt_node, event);
	return ERRNO_SUCCESS;
}

void pim_jp_down_sg_rpt_ni(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	uint16_t holdtime = 0;
	struct ipmc_if *pif = NULL;

	if(!sgrpt_node || !(sgrpt_node->pif))
	{
		return ;
	}
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV:
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV:
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_PP;
			pif = sgrpt_node->pif;
			/*启动et timer，设置为报文中的holdtime*/
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(pif, sgrpt_node->grp_addr);
			if(sgrpt_node->et_timer)
			{
				/*thread_cancel(sgrpt_node->et_timer);*/
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			/*sgrpt_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_et_timer_expire, (void *)sgrpt_node, holdtime);*/
			sgrpt_node->et_timer = high_pre_timer_add("JPDownSGRptETtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sgrpt_node_et_timer_expire,(void *)sgrpt_node,
								holdtime*1000);
			if(sgrpt_node->ppt_timer)
			{
				/*thread_cancel(sgrpt_node->ppt_timer);*/
				high_pre_timer_delete(sgrpt_node->ppt_timer);
			}
			/*多个邻居，所有邻居都prune delay时就要延迟剪枝*/
			if(pif->pim_nbr->nbr_num > 1 && pim_jp_down_prune_delay(sgrpt_node->pif->pim_nbr))
			{
				/*sgrpt_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_ppt_timer_expire, (void *)sgrpt_node, 
					(pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override)/1000);*/
				sgrpt_node->ppt_timer = high_pre_timer_add("JPDownSGRptPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sgrpt_node_ppt_timer_expire,(void *)sgrpt_node,
								pim_instance_global->pim_conf.lan_delay + pim_instance_global->pim_conf.override);
			}
			else
			{
				/*只有一个邻居的情况下设置ppt timer立即超时*/
				/*sgrpt_node->ppt_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_ppt_timer_expire, (void *)sgrpt_node, 0);*/
				sgrpt_node->ppt_timer = high_pre_timer_add("JPDownSGRptPPTtimer",LIB_TIMER_TYPE_NOLOOP,
								pim_jp_down_sgrpt_node_ppt_timer_expire,(void *)sgrpt_node,
								0);
			}
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_ET_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG:
			
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_rpt_p(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	uint16_t holdtime = 0;

	switch(event)
	{
		case PIM_JP_DOWN_EVENT_WC_J_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_PT;
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*取消定时器et /ppt*/
			if(sgrpt_node->et_timer)
			{
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = 0;
			if(sgrpt_node->ppt_timer)
			{
				high_pre_timer_delete(sgrpt_node->ppt_timer);
			}
			sgrpt_node->ppt_timer = 0;
			
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_P;
			/*重启et timer，设置为报文中的holdtime*/
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sgrpt_node->pif, sgrpt_node->grp_addr);
			/*if(sgrpt_node->et_timer)
			{
				thread_cancel(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_et_timer_expire, (void *)sgrpt_node, holdtime);
			*/
			sgrpt_node->et_timer = high_pre_timer_add("JPDownSGRptETtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_down_sgrpt_node_et_timer_expire,(void *)sgrpt_node,
							1000* holdtime);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX:
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_ET_EX:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			
			/*del sg_rpt oif */
			//pim_mrt_sg_rpt_oif_del(pif, sgrpt_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG:
			
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(sgrpt_node->et_timer)
			{
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = 0;
			if(sgrpt_node->ppt_timer)
			{
				high_pre_timer_delete(sgrpt_node->ppt_timer);
			}
			sgrpt_node->ppt_timer = 0;
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_rpt_pp(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_WC_J_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_PPT;
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_PPT;
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*取消定时器et /ppt*/
			if(sgrpt_node->et_timer)
			{
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = 0;
			if(sgrpt_node->ppt_timer)
			{
				high_pre_timer_delete(sgrpt_node->ppt_timer);
			}
			sgrpt_node->ppt_timer = 0;
			
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX:
			sgrpt_node->state = PIM_JP_DOWN_STATE_P;
			
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_ET_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG:
			
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			if(sgrpt_node->et_timer)
			{
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = 0;
			if(sgrpt_node->ppt_timer)
			{
				high_pre_timer_delete(sgrpt_node->ppt_timer);
			}
			sgrpt_node->ppt_timer = 0;
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			/*删除sgrpt_node*/
			
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_rpt_pt(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	uint16_t holdtime = 0;
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_P;
			/*重启et timer，设置为报文中的holdtime*/
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sgrpt_node->pif, sgrpt_node->grp_addr);
			/*if(sgrpt_node->et_timer)
			{
				thread_cancel(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_et_timer_expire, (void *)sgrpt_node, holdtime);
			*/
			sgrpt_node->et_timer = high_pre_timer_add("JPDownSGRptETtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_down_sgrpt_node_et_timer_expire,(void *)sgrpt_node,
							1000 * holdtime);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_ET_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*取消定时器et timer*/
			if(sgrpt_node->et_timer)
			{
				high_pre_timer_delete(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = 0;
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_rpt_ppt(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	uint16_t holdtime = 0;
	
	switch(event)
	{
		case PIM_JP_DOWN_EVENT_SG_RPT_WCJ_RCV:
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV:
			sgrpt_node->state = PIM_JP_DOWN_STATE_PP;
			/*重启et timer，设置为报文中的holdtime*/
			holdtime = pim_jp_down_sg_grp_node_holdtime_get(sgrpt_node->pif, sgrpt_node->grp_addr);
			/*if(sgrpt_node->et_timer)
			{
				thread_cancel(sgrpt_node->et_timer);
			}
			sgrpt_node->et_timer = thread_add_timer(ipmc_master, pim_jp_down_sgrpt_node_et_timer_expire, (void *)sgrpt_node, holdtime);
			*/
			sgrpt_node->et_timer = high_pre_timer_add("JPDownSGRptETtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_down_sgrpt_node_et_timer_expire,(void *)sgrpt_node,
							1000 * holdtime);
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_PPT_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_ET_EX:
			
			break;
		case PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG:
			sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
			/*del sg_rpt oif */
			pim_mrt_sg_rpt_oif_del(sgrpt_node->pif, sgrpt_node);
			break;
		case PIM_JP_DOWN_EVENT_IFM_DEL:
			
			break;
		default:
			break;
	}
}

void pim_jp_down_sg_rpt_state_machine(struct pim_down_jp_sg_node *sgrpt_node, uint32_t event)
{
	if(!sgrpt_node || !sgrpt_node->pif)
	{
		return ;
	}
	switch(sgrpt_node->state)
	{
		case PIM_JP_DOWN_STATE_NOINFO:
			pim_jp_down_sg_rpt_ni(sgrpt_node, event);
			break;
		case PIM_JP_DOWN_STATE_P:
			pim_jp_down_sg_rpt_p(sgrpt_node, event);
			break;
		case PIM_JP_DOWN_STATE_PP:
			pim_jp_down_sg_rpt_pp(sgrpt_node, event);
			break;
		case PIM_JP_DOWN_STATE_PT:
			pim_jp_down_sg_rpt_pt(sgrpt_node, event);
			break;
		case PIM_JP_DOWN_STATE_PPT:
			pim_jp_down_sg_rpt_ppt(sgrpt_node, event);
			break;
		default:
			break;
	}
}

sint32 pim_jp_up_wc_jt_timer_expire(struct thread *thread)
{
	struct pim_up_jp_grp_node *pup_wc = NULL;
	struct pim_mrt_wc * wc_mrt = NULL;

	pup_wc = (struct pim_up_jp_grp_node *)(thread->arg);
	pup_wc->jt_timer = 0;
	/*走超时定时器状态机，状态机中发包*/
	/*根据grp查找(*G)*/
	wc_mrt = pup_wc->to_wc;
	pim_jp_up_wc_state_machine(wc_mrt, NULL, PIM_JP_UP_EVENT_JOIN_TIMER_EX);
	return 0;
}

sint32 pim_jp_up_sg_jt_timer_expire(struct thread *thread)
{
	struct pim_up_jp_sg_node *pup_sg = NULL;
	struct pim_mrt_sg * sg_mrt = NULL;

	pup_sg = (struct pim_up_jp_sg_node *)(thread->arg);
	pup_sg->jt_timer = 0;
	/*走超时定时器状态机，状态机中发包*/
	/*根据grp查找(SG)*/
	sg_mrt = (struct pim_mrt_sg *)pup_sg->to_sg;
	pim_jp_up_sg_state_machine(sg_mrt, NULL, PIM_JP_UP_EVENT_JOIN_TIMER_EX);
	return 0;
}

sint32 pim_jp_up_sg_ot_timer_expire(struct thread *thread)
{
	struct pim_up_jp_sg_node *pup_sg = NULL;
	struct pim_mrt_sg * sg_mrt = NULL;
	uint32_t event = 0;

	pup_sg = (struct pim_up_jp_sg_node *)(thread->arg);
	pup_sg->ot_timer = 0;
	/*走超时定时器状态机，状态机中发包*/
	event = PIM_JP_UP_EVENT_SGRPT_OTIMER_EX;
	/*根据grp查找(SGrpt)*/
	sg_mrt = pup_sg->to_sg;
	pim_jp_up_sg_rpt_state_machine(sg_mrt, event);
	return 0;
}

void pim_jp_up_wc_nj(struct pim_mrt_wc *wc_mrt, uint32_t event)
{
	struct pim_up_jp_grp_node *pup_wc = NULL;
	uint32_t ret = 0;
	
	switch(event)
	{
		case PIM_JP_UP_EVENT_WC_JOINDESIRED_T:
			/*J state, Send Join(*,G), Set Join Timer to t_periodic (default 60s)*/
			wc_mrt->jp_up_state = PIM_JP_UP_STATE_JOINED;
			ret = pim_jp_up_wc_jp_send( wc_mrt->upstream_ifindex,  wc_mrt->rpf_nbr, wc_mrt->grp_addr,wc_mrt->rp_addr, PIM_JP_UP_SEND_WC_JOIN);
			if(ret != NET_SUCCESS)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_wc_jp_send failed\n", wc_mrt->grp_addr);
			}
			/*添加周期性发送(*,G)加枝报文的节点*/
			pup_wc = pim_jp_up_send_wc_jp_upstream(wc_mrt, PIM_CREATE);
			if(pup_wc == NULL)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_send_wc_jp_upstream failed\n", wc_mrt->grp_addr);
				break;
			}
			/*SET TIMER 60S*/
			/*if(pup_wc->jt_timer)
			{
				thread_cancel(pup_wc->jt_timer);
			}
			pup_wc->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_wc_jt_timer_expire, (void *)pup_wc, PIM_JP_PERIODIC);
			*/
			pup_wc->jt_timer = high_pre_timer_add("JPDownWCJTtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_up_wc_jt_timer_expire,(void *)pup_wc,
							1000 * PIM_JP_PERIODIC);
			break;
		case PIM_JP_UP_EVENT_WC_JOINDESIRED_F:
			break;
		default:
			break;
	}
}

void pim_jp_up_wc_j(struct pim_mrt_wc *wc_mrt, struct pim_mrt_wc *old_wc_mrt, uint32_t event)
{
	struct pim_up_jp_grp_node *pup_wc = NULL;
	uint32_t ret = 0;
	
	switch(event)
	{
		case PIM_JP_UP_EVENT_WC_JOINDESIRED_T:
			break;
		case PIM_JP_UP_EVENT_WC_JOINDESIRED_F:
			/*NJ state, Send Prune(*,G), Cancel Join Timer*/
			wc_mrt->jp_up_state = PIM_JP_UP_STATE_NOTJOINED;
			ret = pim_jp_up_wc_jp_send(wc_mrt->upstream_ifindex,  wc_mrt->rpf_nbr, wc_mrt->grp_addr,wc_mrt->rp_addr, PIM_JP_UP_SEND_WC_PRUNE);
			if(ret != NET_SUCCESS)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_wc_jp_send failed\n", wc_mrt->grp_addr);
			}
			/*获取发送(*,G)加枝的节点，取消定时器*/
			ret = pim_jp_up_send_wc_jp_node_del(wc_mrt);
			if(ret != ERRNO_SUCCESS)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_send_wc_jp_node_del failed\n", wc_mrt->grp_addr);
			}
			break;
		case PIM_JP_UP_EVENT_JOIN_TIMER_EX:
			/* send (*,G) join, set jt_timer to t_periodic */
			ret = pim_jp_up_wc_jp_send(wc_mrt->upstream_ifindex,  wc_mrt->rpf_nbr, wc_mrt->grp_addr, wc_mrt->rp_addr, PIM_JP_UP_SEND_WC_JOIN);
			if(ret != NET_SUCCESS)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_wc_jp_send failed\n", wc_mrt->grp_addr);
			}
			/*周期性发送(*,G)加枝报文的节点*/
			pup_wc = pim_jp_up_send_wc_jp_upstream(wc_mrt, PIM_CREATE);
			if(pup_wc == NULL)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_send_wc_jp_upstream failed\n", wc_mrt->grp_addr);
				break;
			}
			/*if(pup_wc->jt_timer)
			{
				thread_cancel(pup_wc->jt_timer);
			}
			pup_wc->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_wc_jt_timer_expire, (void *)pup_wc, PIM_JP_PERIODIC);
			*/
			pup_wc->jt_timer = high_pre_timer_add("JPDownWCJTtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_up_wc_jt_timer_expire,(void *)pup_wc,
							1000 * PIM_JP_PERIODIC);
			break;
		case PIM_JP_UP_EVENT_RPF_CH_NHP:
			/*Send Join(*,G) to new next hop; Send Prune(*,G) to old next hop; Set Join Timer to t_periodic*/
			ret = pim_jp_up_wc_jp_send(wc_mrt->upstream_ifindex,  wc_mrt->rpf_nbr, wc_mrt->grp_addr, wc_mrt->rp_addr, PIM_JP_UP_SEND_WC_JOIN);
			if(ret != NET_SUCCESS)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_wc_jp_send failed\n", wc_mrt->grp_addr);
			}
			if(old_wc_mrt)
			{
				ret = pim_jp_up_wc_jp_send(old_wc_mrt->upstream_ifindex,  old_wc_mrt->rpf_nbr, old_wc_mrt->grp_addr, old_wc_mrt->rp_addr, PIM_JP_UP_SEND_WC_PRUNE);
				if(ret != NET_SUCCESS)
				{
					IPMC_LOG_ERROR("(*, %x) pim_jp_up_wc_jp_send failed\n", old_wc_mrt->grp_addr);
				}
			}
			/*周期性发送(*,G)加枝报文的节点*/
			pup_wc = pim_jp_up_send_wc_jp_upstream(wc_mrt, PIM_CREATE);
			if(pup_wc == NULL)
			{
				IPMC_LOG_ERROR("(*, %x) pim_jp_up_send_wc_jp_upstream failed\n", wc_mrt->grp_addr);
				break;
			}
			/*if(pup_wc->jt_timer)
			{
				thread_cancel(pup_wc->jt_timer);
			}
			pup_wc->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_wc_jt_timer_expire, (void *)pup_wc, PIM_JP_PERIODIC);
			*/
			pup_wc->jt_timer = high_pre_timer_add("JPDownWCJTtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_up_wc_jt_timer_expire,(void *)pup_wc,
							1000 * PIM_JP_PERIODIC);
			break;
		case PIM_JP_UP_EVENT_SEE_WC_JOIN:
			/*Increase jt timer to t_joinsuppress*/
			/*暂时不支持，正常发送join报文*/
			/*可以在上游收取报文的流程中处理*/
			break;
		case PIM_JP_UP_EVENT_SEE_WC_PRUNE:
			/*decrease jt timer to t_override, default 2.5s*/
			/*可以在上游收取报文的流程中处理*/
			break;
		case PIM_JP_UP_EVENT_RPF_CH_AST:
			/*decrease jt timer to t_override, default 2.5s*/
			/*暂时不支持*/
			break;
		case PIM_JP_UP_EVENT_RPF_CH_GENID:
			/*Decrease Join Timer to t_override*/
			break;
		default:
			break;
	}
}

void pim_jp_up_sg_nj(struct pim_mrt_sg *sg_mrt, uint32_t event)
{
	struct pim_up_jp_sg_node *up_sg = NULL;

	switch(event)
	{
		case PIM_JP_UP_EVENT_SG_JOINDESIRED_T:
			/*-> J state, Send Join(S,G); Set Join Timer to t_periodic*/
			sg_mrt->jp_up_state = PIM_JP_UP_STATE_JOINED;
			pim_jp_up_sg_jp_send( sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr,sg_mrt->grp_addr, sg_mrt->src_addr, PIM_JP_UP_SEND_SG_JOIN);
			up_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_TYPE, PIM_CREATE);
			if(up_sg)
			{
				up_sg->to_sg = sg_mrt;
				/*Set Join Timer to t_periodic*/
				/*if(up_sg->jt_timer)
				{
					thread_cancel(up_sg->jt_timer);
				}
				up_sg->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_jt_timer_expire, (void *)up_sg, PIM_JP_PERIODIC);
				*/
				up_sg->jt_timer = high_pre_timer_add("JPDownSGJTtimer",LIB_TIMER_TYPE_NOLOOP,
							pim_jp_up_sg_jt_timer_expire,(void *)up_sg,
							1000 * PIM_JP_PERIODIC);
			}
			break;
		case PIM_JP_UP_EVENT_SG_JOINDESIRED_F:
			break;
		default:
			break;
	}
}

void pim_jp_up_sg_j(struct pim_mrt_sg *sg_mrt, struct pim_mrt_sg * old_sg_mrt, uint32_t event)
{
	struct pim_up_jp_sg_node *pup_sg = NULL;
	
	switch(event)
	{
		case PIM_JP_UP_EVENT_SG_JOINDESIRED_T:
			break;
		case PIM_JP_UP_EVENT_SG_JOINDESIRED_F:
			/*-> NJ state ,Send Prune(S,G); Set SPTbit(S,G) to FALSE; Cancel Join Timer*/
			sg_mrt->jp_up_state = PIM_JP_UP_STATE_NOTJOINED;
			pim_jp_up_sg_jp_send(sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr, sg_mrt->grp_addr,sg_mrt->src_addr, PIM_JP_UP_SEND_SG_PRUNE);
			sg_mrt->flag &= ~PIM_MRT_SPT;
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_TYPE, PIM_LOOKUP);
			if(pup_sg)
			{
				if(pup_sg->jt_timer)
				{
					high_pre_timer_delete(pup_sg->jt_timer);
					pup_sg->jt_timer = 0;
				}
				if(pup_sg->ot_timer)
				{
					high_pre_timer_delete(pup_sg->ot_timer);
					pup_sg->ot_timer = 0;
				}
				/*删除这个节点*/
				pim_jp_up_send_sg_node_del(sg_mrt, pup_sg);
			}
			break;
		case PIM_JP_UP_EVENT_JOIN_TIMER_EX:
			/*发送报文*/
			pim_jp_up_sg_jp_send(sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr,sg_mrt->grp_addr,sg_mrt->src_addr, PIM_JP_UP_SEND_SG_JOIN);
			/*设置定时器*/
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_TYPE, PIM_CREATE);
			if(pup_sg)
			{
				/*if(pup_sg->jt_timer)
				{
					thread_cancel(pup_sg->jt_timer);
				}
				pup_sg->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_jt_timer_expire, (void *)pup_sg, PIM_JP_PERIODIC);
				*/
				pup_sg->jt_timer = high_pre_timer_add("JPDownSGJTtimer",LIB_TIMER_TYPE_NOLOOP,
						pim_jp_up_sg_jt_timer_expire,(void *)pup_sg,
						1000 * PIM_JP_PERIODIC);
			}
			break;
		case PIM_JP_UP_EVENT_RPF_CH_NHP:
			/* Send Join(S,G) to new next hop; Send Prune(S,G) to old next hop; Set Join Timer to t_periodic */
			/*发送报文*/
			pim_jp_up_sg_jp_send(sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr,sg_mrt->grp_addr,sg_mrt->src_addr, PIM_JP_UP_SEND_SG_JOIN);
			if(old_sg_mrt)
			{
				pim_jp_up_sg_jp_send(old_sg_mrt->upstream_ifindex, old_sg_mrt->rpf_nbr,old_sg_mrt->grp_addr,old_sg_mrt->src_addr, PIM_JP_UP_SEND_SG_PRUNE);
			}
			/*设置定时器*/
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_TYPE, PIM_CREATE);
			if(pup_sg)
			{
				/*if(pup_sg->jt_timer)
				{
					thread_cancel(pup_sg->jt_timer);
				}
				pup_sg->jt_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_jt_timer_expire, (void *)pup_sg, PIM_JP_PERIODIC);
				*/
				pup_sg->jt_timer = high_pre_timer_add("JPDownSGJTtimer",LIB_TIMER_TYPE_NOLOOP,
						pim_jp_up_sg_jt_timer_expire,(void *)pup_sg,
						1000 * PIM_JP_PERIODIC);
			}
			break;
		case PIM_JP_UP_EVENT_SEE_SG_JOIN:
			/* Increase Join Timer to t_joinsuppress */
			break;
		case PIM_JP_UP_EVENT_SEE_SG_PRUNE:
			/* Decrease Join Timer to t_override */
			break;
		case PIM_JP_UP_EVENT_SEE_SGRPT_PRUNE:
			/* Decrease Join Timer to t_override */
			break;
		case PIM_JP_UP_EVENT_SEE_WC_PRUNE:
			/* Decrease Join Timer to t_override */
			break;
		case PIM_JP_UP_EVENT_RPF_CH_AST:
			/* Decrease Join Timer to t_override */
			break;
		case PIM_JP_UP_EVENT_RPF_CH_GENID:
			/* Decrease Join Timer to t_override */
			break;
		default:
			break;
	}
}

void pim_jp_up_sg_rpt_nj(struct pim_mrt_sg *sg_mrt, uint32_t event)
{
	switch(event)
	{
		case PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T:
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_P;
			break;
		case PIM_JP_UP_EVENT_SGRPT_OLIST_NOTNULL:
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NP;
			break;
		default:
			break;
	}
}

void pim_jp_up_sg_rpt_p(struct pim_mrt_sg *sg_mrt, uint32_t event)
{
	switch(event)
	{
		case PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_F:
			/* ->np state, send join(s,g,rpt) */
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NP;
			pim_jp_up_sg_jp_send( sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr,sg_mrt->grp_addr,sg_mrt->src_addr, PIM_JP_UP_SEND_SG_RPT_JOIN);
			break;
		case PIM_JP_UP_EVENT_SGRPT_JOINDESIRED_F:
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NJ;
			break;
		case PIM_JP_UP_EVENT_RPF_CH_NHP:
			/*wc的rpf改变，sgrpt剪枝的上游改变*/
			break;
		default:
			break;
	}
}

void pim_jp_up_sg_rpt_np(struct pim_mrt_sg *sg_mrt, uint32_t event)
{
	struct pim_up_jp_sg_node *pup_sg = NULL;
	struct pim_mrt_wc *mrt_wc = NULL;
	
	switch(event)
	{
		case PIM_JP_UP_EVENT_SGRPT_PRUNEDESIRED_T:
			/* ->p state, send prune(s,g,rpt),cancel ot */
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_P;
			mrt_wc = sg_mrt->wc_mrt;
			if(!mrt_wc)
			{
				break;
			}
			pim_jp_up_sg_jp_send(mrt_wc->upstream_ifindex, mrt_wc->rpf_nbr, sg_mrt->grp_addr, sg_mrt->src_addr, PIM_JP_UP_SEND_SG_RPT_PRUNE);
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_LOOKUP);
			if(pup_sg)
			{
				if(pup_sg->ot_timer)
				{
					high_pre_timer_delete(pup_sg->ot_timer);
					pup_sg->ot_timer = 0;
				}
			}
			break;
		case PIM_JP_UP_EVENT_SGRPT_JOINDESIRED_F:
			/*->nj, cancel ot*/
			sg_mrt->jp_up_rptstate = PIM_JP_UP_STATE_RPT_NJ;
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_LOOKUP);
			if(pup_sg)
			{
				if(pup_sg->ot_timer)
				{
					high_pre_timer_delete(pup_sg->ot_timer);
					pup_sg->ot_timer = 0;
				}
			}
			break;
		case PIM_JP_UP_EVENT_SGRPT_OTIMER_EX:
			/*send join(s,g,rpt), leave ot unset*/
			pim_jp_up_sg_jp_send(sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr, sg_mrt->grp_addr, sg_mrt->src_addr, PIM_JP_UP_SEND_SG_RPT_JOIN);
			break;
		case PIM_JP_UP_EVENT_SEE_SGRPT_PRUNE:
			/*OT = min(OT, t_override), default t_overrid 2.5s */
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_CREATE);
			if(pup_sg)
			{
				/*if(pup_sg->ot_timer)
				{
					thread_cancel(pup_sg->ot_timer);
				}
				pup_sg->ot_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_ot_timer_expire, (void *)pup_sg, PIM_JP_OVERRIDE);
				*/
				pup_sg->ot_timer = high_pre_timer_add("JPDownSGOTtimer",LIB_TIMER_TYPE_NOLOOP,
						pim_jp_up_sg_ot_timer_expire,(void *)pup_sg,
						1000 * PIM_JP_OVERRIDE);
			}
			break;
		case PIM_JP_UP_EVENT_SEE_SGRPT_JOIN:
			/*Cancel OT*/
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_LOOKUP);
			if(pup_sg)
			{
				if(pup_sg->ot_timer)
				{
					high_pre_timer_delete(pup_sg->ot_timer);
					pup_sg->ot_timer = 0;
				}
			}
			break;
		case PIM_JP_UP_EVENT_SEE_SG_PRUNE:
			/*OT = min(OT, t_override)*/
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_CREATE);
			if(pup_sg)
			{
				/*if(pup_sg->ot_timer)
				{
					thread_cancel(pup_sg->ot_timer);
				}
				pup_sg->ot_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_ot_timer_expire, (void *)pup_sg, PIM_JP_OVERRIDE);
				*/
				pup_sg->ot_timer = high_pre_timer_add("JPDownSGOTtimer",LIB_TIMER_TYPE_NOLOOP,
						pim_jp_up_sg_ot_timer_expire,(void *)pup_sg,
						1000 * PIM_JP_OVERRIDE);
			}
			break;
		case PIM_JP_UP_EVENT_RPF_CH_TO_WCRPF:
			/*OT = min(OT, t_override)*/
			pup_sg = pim_jp_up_send_sg_jp_upstream(sg_mrt, PIM_JP_SG_RPT_TYPE, PIM_CREATE);
			if(pup_sg)
			{
				/*if(pup_sg->ot_timer)
				{
					thread_cancel(pup_sg->ot_timer);
				}
				pup_sg->ot_timer = thread_add_timer(ipmc_master, pim_jp_up_sg_ot_timer_expire, (void *)pup_sg, PIM_JP_OVERRIDE);
				*/
				pup_sg->ot_timer = high_pre_timer_add("JPDownSGOTtimer",LIB_TIMER_TYPE_NOLOOP,
						pim_jp_up_sg_ot_timer_expire,(void *)pup_sg,
						1000 * PIM_JP_OVERRIDE);
			}
			break;
		default:
			break;
	}
}

/*在(*,G)的RP改变时，up_nbr改变时需要用到old_wc_mrt*/
void pim_jp_up_wc_state_machine(struct pim_mrt_wc *wc_mrt, struct pim_mrt_wc *old_wc_mrt, uint32_t event)
{
	if(wc_mrt == NULL)
	{
		return ;
	}

	switch(wc_mrt->jp_up_state)
	{
		case PIM_JP_UP_STATE_NOTJOINED:
			pim_jp_up_wc_nj(wc_mrt,  event);
			break;
		case PIM_JP_UP_STATE_JOINED:
			pim_jp_up_wc_j(wc_mrt, old_wc_mrt, event);
			break;
		default:
			break;
	}
	return ;
		
}

void pim_jp_up_sg_state_machine(struct pim_mrt_sg *sg_mrt, struct pim_mrt_sg *old_sg_mrt, uint32_t event)
{
	if(sg_mrt == NULL)
	{
		return ;
	}

	switch(sg_mrt->jp_up_state)
	{
		case PIM_JP_UP_STATE_NOTJOINED:
			pim_jp_up_sg_nj(sg_mrt, event);
			break;
		case PIM_JP_UP_STATE_JOINED:
			pim_jp_up_sg_j(sg_mrt, old_sg_mrt, event);
			break;
		default:
			break;
	}
	return ;
}

void pim_jp_up_sg_rpt_state_machine(struct pim_mrt_sg *sg_mrt, uint32_t event)
{
	if(sg_mrt == NULL)
	{
		return ;
	}

	switch(sg_mrt->jp_up_rptstate)
	{
		case PIM_JP_UP_STATE_RPT_NJ:
			pim_jp_up_sg_rpt_nj(sg_mrt,  event);
			break;
		case PIM_JP_UP_STATE_RPT_P:
			pim_jp_up_sg_rpt_p(sg_mrt,  event);
			break;
		case PIM_JP_UP_STATE_RPT_NP:
			pim_jp_up_sg_rpt_np(sg_mrt,  event);
			break;
		default:
			break;
	}
	return ;
}


