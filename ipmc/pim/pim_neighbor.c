/*
*   PIM neighbor 管理，DR选举
*/

#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include <lib/checksum.h>
#include "hptimer.h"

#include <ftm/pkt_ip.h>

#include "../ipmc_main.h"
#include "../ipmc_public.h"
#include "pim_neighbor.h"
#include "../ipmc_if.h"
#include "pim.h"
#include "pim_pkt.h"
#include "pim_join_prune.h"
#include "pim_register.h"
#include "pim_mroute.h"


/* 收到 hello 报文触发建立邻居 */
struct pim_neighbor_node* pim_nbr_node_create(struct pim_neighbor *pnbr, struct pim_hello_opt *options)
{
	struct pim_neighbor_node *pnbr_node = NULL;
		
	pnbr_node = (struct pim_neighbor_node *)XCALLOC(MTYPE_IPMC_PIM_NBR_NODE_ENTRY, sizeof(struct pim_neighbor_node));
	if (NULL == pnbr_node)
	{
		return NULL;
	}
    memset(pnbr_node, 0, sizeof(struct pim_neighbor_node));
	
	pnbr_node->opt_flag = options->opt_flag;
	pnbr_node->gen_id = options->gen_id;
	if(pnbr_node->opt_flag & PIM_HELLO_DR_PRI_PRESENT)
	{
		pnbr_node->dr_priority = options->dr_priority;
	}
	if(pnbr_node->opt_flag & PIM_HELLO_LAN_DELAY_PRESENT)
	{
		pnbr_node->lan_delay = options->lan_delay;
		pnbr_node->override = options->override;
		pnbr_node->t_bit = options->lan_delay & 0x8000;
	}
	if(pnbr_node->opt_flag & PIM_HELLO_ADDRLIST_PRESENT)
	{
		pnbr_node->second_addr = options->second_addr;
	}
	if(pnbr_node->opt_flag & PIM_HELLO_HOLDTIME_PRESENT)
	{
		/*pnbr_node->nlt_timer = thread_add_timer(ipmc_master, pim_nbr_ntl_timer_expire, (void *)pnbr_node, options->holdtime);*/
		pnbr_node->nlt_timer = high_pre_timer_add("PimNbrNltTimer",LIB_TIMER_TYPE_NOLOOP,
					pim_nbr_ntl_timer_expire,(void *)pnbr_node,
					1000 * options->holdtime);
	}
	/*初始化up_wc_jp hash_table*/
	pim_jp_down_grp_table_init(&(pnbr_node->up_wc_jp), PIM_GRP_MAX);
	pim_jp_up_grp_src_table_init(&(pnbr_node->up_sg_jp), PIM_GRP_MAX);
	
	pim_nbr_node_add(pnbr, pnbr_node);

	return pnbr_node;
}

/* 添加到邻居链表 */
sint32 pim_nbr_node_add(struct pim_neighbor *pnbr, struct pim_neighbor_node *pnbr_node)
{
	if(NULL == pnbr_node)
	{
		return ERRNO_FAIL;
	}
	listnode_add (&(pnbr->neighbor_list), pnbr_node);
	pnbr->nbr_num++;

	return ERRNO_SUCCESS;
}

/* 从邻居链表删除 */
sint32 pim_nbr_node_delete(struct pim_neighbor *pnbr, struct pim_neighbor_node *pnbr_node)
{
	/* 清除邻居 */
	if(!pnbr || !pnbr_node || pnbr->nbr_num == 0)
	{
		return ERRNO_FAIL;
	}
	pnbr->nbr_num--;
	listnode_delete (&(pnbr->neighbor_list), pnbr_node);
	if(pnbr_node->nlt_timer)
	{
		high_pre_timer_delete(pnbr_node->nlt_timer);
		pnbr_node->nlt_timer = 0;
	}
	if(pnbr->cur_dr.dr_addr == pnbr_node->nbr_addr)
	{
		/*如果dr被删除，重新进行dr选举*/
		pim_dr_election((struct ipmc_if *)(pnbr_node->to_if));
	}
	if(pnbr_node->opt_flag & PIM_HELLO_LAN_DELAY_PRESENT)
	{
		/*重新计算delay,override*/
	}
	XFREE(MTYPE_IPMC_PIM_NBR_NODE_ENTRY, pnbr_node);
	
	return ERRNO_SUCCESS;
}


struct pim_neighbor_node* pim_nbr_node_lookup(struct pim_neighbor *pnbr, uint32_t nbr_addr)
{
	struct pim_neighbor_node *pnbr_node = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
	
	if(pnbr == NULL)
	{
		IPMC_LOG_ERROR("pnbr NULL");
		return NULL;
	}
	for (ALL_LIST_ELEMENTS(&(pnbr->neighbor_list), pnode, pnextnode, pnbr_node))
	{
		if(pnbr_node->nbr_addr == nbr_addr)
		{
			return pnbr_node;
		}
	}
	
	return NULL;
}

/*sint32 pim_nbr_ntl_timer_expire(struct thread *thread)*/
sint32 pim_nbr_ntl_timer_expire(void *para)
{
	struct pim_neighbor_node *pnbr_node = NULL;
	struct pim_neighbor *pnbr = NULL;
	struct ipmc_if *pif = NULL;
	sint32 ret = 0;
	

	/*删除邻居*/
	/*pnbr_node = (struct pim_neighbor_node *)THREAD_ARG(thread);*/
	pnbr_node = (struct pim_neighbor_node *)para;
	pif = (struct ipmc_if*)(pnbr_node->to_if);
	pnbr = pif->pim_nbr;
	pnbr_node->nlt_timer = 0;

	ret = pim_nbr_node_delete(pnbr, pnbr_node);
	if(ret != ERRNO_SUCCESS)
	{
		IPMC_LOG_DEBUG("ifindex %d delete nbr %d failed!\n", pif->ifindex, pnbr_node->nbr_addr);
	}
	
	return ret;
}

sint32 pim_nbr_option_update(struct ipmc_if *pif, struct pim_neighbor_node *pnbr_node, struct pim_hello_opt *hello_options)
{
	/*判断gen_id是否改变 */
	if(pnbr_node->gen_id != hello_options->gen_id)
	{
		/*说明邻居重新启动了*/
		pnbr_node->gen_id = hello_options->gen_id;
		
		/*立即发送hello报文，尽快建立邻居信息*/
		pim_hello_send(pif, IPMC_FALSE);
		/*发送join/prune,尽快恢复邻居的j/p状态*/
		
	}
	
	if((hello_options->opt_flag & PIM_HELLO_DR_PRI_PRESENT) && 
		(pnbr_node->opt_flag & PIM_HELLO_DR_PRI_PRESENT) && 
		(pnbr_node->dr_priority != hello_options->dr_priority))
	{
		/* dr priority change, dr election */
		pim_dr_election(pif);
	}
	
	if((hello_options->opt_flag & PIM_HELLO_LAN_DELAY_PRESENT) &&
		(pnbr_node->opt_flag & PIM_HELLO_LAN_DELAY_PRESENT))
	{
		if(hello_options->lan_delay != pnbr_node->lan_delay || 
			hello_options->override != pnbr_node->override)
		{
			/*01 f4 09 c4*/
			pnbr_node->t_bit = hello_options->lan_delay & 0x8000;
			/*当前接口delay ,override取所有邻居的最大值，不支持时使用默认值*/
			pnbr_node->lan_delay = hello_options->lan_delay;
			pnbr_node->override = hello_options->override;
			/*更新接口的delay,override*/
			
		}
	}
	if(hello_options->opt_flag & PIM_HELLO_HOLDTIME_PRESENT)
	{
		/*更新ntl_timer, holdtime == 0 则立即超时*/
		/*if(pnbr_node->nlt_timer)
		{
			thread_cancel(pnbr_node->nlt_timer);
			pnbr_node->nlt_timer = NULL;
		}
		pnbr_node->nlt_timer = thread_add_timer(ipmc_master, pim_nbr_ntl_timer_expire, 
			(void *)pnbr_node, hello_options->holdtime);*/
		pnbr_node->nlt_timer = high_pre_timer_add("PimNbrNltTimer",LIB_TIMER_TYPE_NOLOOP,
					pim_nbr_ntl_timer_expire,(void *)pnbr_node,
					1000 * hello_options->holdtime);
	}
	else
	{
		/*使用默认值105更新ntl_timer*/
		/*if(pnbr_node->nlt_timer)
		{
			thread_cancel(pnbr_node->nlt_timer);
			pnbr_node->nlt_timer = NULL;
		}*/
		/*pnbr_node->nlt_timer = thread_add_timer(ipmc_master, pim_nbr_ntl_timer_expire, 
			(void *)pnbr_node, PIM_HELLO_HOLDTIME_DEFAULT);*/

		pnbr_node->nlt_timer = high_pre_timer_add("PimNbrNltTimer",LIB_TIMER_TYPE_NOLOOP,
					pim_nbr_ntl_timer_expire,(void *)pnbr_node,
					1000 * PIM_HELLO_HOLDTIME_DEFAULT);
		
	}
	
	/*目前只保存一个sec_addr*/
	if(hello_options->opt_flag & PIM_HELLO_ADDRLIST_PRESENT)
	{
		pnbr_node->second_addr = hello_options->second_addr;
	}
	else
	{
		pnbr_node->second_addr = 0;
	}
	
	return ERRNO_SUCCESS;
}

sint32 pim_dr_is_better(struct pim_dr_state *dr, struct pim_neighbor_node *pnbr_node, uint16_t dr_pri_elec)
{
	if(dr_pri_elec)
	{
		return ((pnbr_node->dr_priority > dr->dr_priority) ||
			((pnbr_node->dr_priority == dr->dr_priority) && (pnbr_node->nbr_addr > dr->dr_addr)));
	}
	else
	{
		return (pnbr_node->nbr_addr > dr->dr_addr);
	}
}

sint32 pim_dr_election(struct ipmc_if *pif)
{
	struct pim_neighbor *pnbr = pif->pim_nbr;
	struct pim_neighbor_node *pnbr_node = NULL;
	struct listnode  *pnode  = NULL, *pnextnode = NULL;
	uint16_t dr_local = 0, dr_pri_elec = IPMC_TRUE, i = 0;
	struct hash_table tab = (pim_instance_global->pim_sg_table);
	struct hash_bucket *pbucket = NULL;
	struct pim_mrt_sg *sg = NULL;
	
	/*
		根据dr_pri、nbr_addr的顺序选举DR
		只有所有邻居都支持dr_pri时,根据dr_pri选举,否则根据nbr_addr
		端口启动pim时，默认为dr
		若dr改变，相关的rpf邻居改变
	*/
	/*判断当前接口是否为dr*/
	if(pnbr->cur_dr.dr_state & PIM_DR_LOCAL)
	{
		dr_local = IPMC_TRUE;
	}
	/*判断是否所有邻居都支持dr_pri*/		
	for (ALL_LIST_ELEMENTS(&pnbr->neighbor_list, pnode, pnextnode, pnbr_node))
	{
		if(!(pnbr_node->opt_flag & PIM_HELLO_DR_PRI_PRESENT))
		{
			dr_pri_elec = IPMC_FALSE;
			break;
		}
	}
	/*假设当前接口为dr*/
	pnbr->cur_dr.dr_addr = ipmc_if_main_addr_get(pif->ifindex);
	pnbr->cur_dr.dr_priority = pim_instance_global->pim_conf.dr_priority;
	pnbr->cur_dr.dr_state |= PIM_DR_LOCAL;
	for(ALL_LIST_ELEMENTS(&pnbr->neighbor_list, pnode, pnextnode, pnbr_node))
	{
		if(pim_dr_is_better(&(pnbr->cur_dr), pnbr_node, dr_pri_elec))
		{
			pnbr->cur_dr.dr_addr = pnbr_node->nbr_addr;
			pnbr->cur_dr.dr_priority = pnbr_node->dr_priority;
			pnbr->cur_dr.dr_state &= ~PIM_DR_LOCAL;
		}
	}
	if(dr_local && !(pif->pim_nbr->cur_dr.dr_state & PIM_DR_LOCAL))
	{
		/*当前接口从dr变成非dr*/
		/*在共享网段中，组播源注册状态需改变*/
		if(tab.num_entries)
		{
			for(i = 0; i < HASHTAB_SIZE; i++)
			{
				for(pbucket = tab.buckets[i]; pbucket; pbucket = pbucket->next)
				{
					if(pbucket->data != NULL)
					{
						sg = (struct pim_mrt_sg *)pbucket->data;
						if((sg->upstream_ifindex == pif->ifindex) && 
							(sg->reg_state == PIM_REG_STATE_J) &&
							(sg->flag & PIM_MRT_LOCAL))
						{
							pim_reg_state_machine(sg, PIM_REG_COULD_NOT_REG);
						}
					}
				}
			}
		}
	}
	return ERRNO_SUCCESS;
}

/*sint32 pim_hello_send_timer_expire(struct thread *thread)*/
sint32 pim_hello_send_timer_expire(void *para)
{
    struct ipmc_if *pif = NULL;
    
    /*pif = (struct ipmc_if *)THREAD_ARG(thread);*/
    pif = (struct ipmc_if *)para;
	pif->pim_nbr->hello_timer = 0;
    
    pim_hello_send(pif, IPMC_TRUE);

    return ERRNO_SUCCESS;
}

sint32 pim_hello_recv(struct ipmc_if *pif, uint32_t sip, uint8_t *pim_para, uint32_t pim_para_len)
{
	uint16_t opt_type = 0, opt_len = 0;
	struct pim_hello_opt options = {0};
	struct pim_hello_opt_type_len *popt_type_len = NULL;
	struct pim_neighbor *pnbr = pif->pim_nbr;
	struct pim_neighbor_node *pnbr_node = NULL;
	struct encode_ipv4_ucast *ucast = NULL;
	int ret = 0;

	if(pim_para_len == 0)
	{
		/*pim hello 报文不存在option，不更新定时器，不创建邻居*/
		IPMC_LOG_ERROR("ifindex %u recv pim hello message, option not exist! \n", pif->ifindex);
		return ERRNO_SUCCESS;
	}
	options.nbr_addr = sip;
	/*解析option,获取到holdtime, dr_priority, gen_id, LAN_delay, override*/
	while(pim_para_len > 0)
	{
		/*获取option type, option len*/
		popt_type_len = (struct pim_hello_opt_type_len *)pim_para;
		opt_type = ntohs(popt_type_len->opt_type);
		opt_len = ntohs(popt_type_len->opt_len);
		pim_para += (PIM_OPTION_TYPE_LEN + PIM_OPTION_LENGTH_LEN);
		pim_para_len -= (PIM_OPTION_TYPE_LEN + PIM_OPTION_LENGTH_LEN);
		switch(opt_type)
		{
			case PIM_HELLO_OPTION_HOLDTIME:
				/*00 69*/
				options.opt_flag |= PIM_HELLO_HOLDTIME_PRESENT;
				options.holdtime = ntohs(*((uint16_t *)pim_para));
				pim_para += opt_len;
				pim_para_len -= opt_len;
				break;
			case PIM_HELLO_OPTION_LAN_DELAY:
				/*01 f4 09 c4*/
				options.opt_flag |= PIM_HELLO_LAN_DELAY_PRESENT;
				options.lan_delay = ntohs(*((uint16_t *)pim_para));
				pim_para += 2;
				options.override = ntohs(*((uint16_t *)pim_para));
				pim_para += 2;
				pim_para_len -= opt_len;
				break;
			case PIM_HELLO_OPTION_DR_PRIORITY:
				/*00 00 00 01*/
				options.opt_flag |= PIM_HELLO_DR_PRI_PRESENT;
				options.dr_priority = ntohl(*((uint32_t *)pim_para));
				pim_para += opt_len;
				pim_para_len -= opt_len;
				break;
			case PIM_HELLO_OPTION_GEN_ID:
				/*73 a9 25 25*/
				/*gen id 肯定存在*/
				options.gen_id = ntohl(*((uint32_t *)pim_para));
				pim_para += opt_len;
				pim_para_len -= opt_len;
				break;
			case PIM_HELLO_OPTION_ADDR_LIST:
					if((opt_len / sizeof(struct encode_ipv4_ucast)) > 0 )
					{
						ucast = (struct encode_ipv4_ucast *)pim_para;
						/*暂时只保存一个从地址,其他的忽略*/
						/* 01 00  addr_family encode_type */
						/* 01 01 01 01 unicast_address */
						options.second_addr = ntohl(ucast->unicast_addr);
						options.second_addr_num = 1;
					}				
					pim_para += opt_len;
					pim_para_len -= opt_len;
					break;
			default:
				break;
		}
	}
	
	/*查找邻居是否已经存在，存在则更新*/
	pnbr_node = pim_nbr_node_lookup(pnbr, sip);
	if(pnbr_node != NULL)
	{
		pim_nbr_option_update(pif, pnbr_node, &options);
	}
	else
	{
		pnbr_node = pim_nbr_node_create(pnbr, &options);
		if(pnbr_node != NULL)
		{
			pnbr_node->nbr_addr = sip;
			pnbr_node->to_if = pif;
			/*发送hello报文*/
			ret = pim_hello_send(pif,IPMC_TRUE);
		}
	}
	return ret;
}

int pim_hello_send(struct ipmc_if *pif, uint32_t reset_timer)
{
    struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_para = NULL;
	union pkt_control pkt_ctrl;
	uint32_t len = 0, para_len = 0;;
	sint32 ret = ERRNO_FAIL;
	struct pim_hello_opt_holdtime holdtime_t = {0};
	struct pim_hello_opt_dr_pri dr_pri_t = {0};
	struct pim_hello_opt_gen_id gen_id_t = {0};
	struct pim_hello_opt_lan_delay lan_delay_t = {0};
	
	/* encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));
	
	pim_para = (uint8_t *)ipmc_send_buf + sizeof(struct pim_hdr);
	
	/*put holdtime*/
	holdtime_t.opt_type = htons(PIM_HELLO_OPTION_HOLDTIME);
	holdtime_t.opt_len = htons(PIM_OPTION_HOLDTIME_LEN);
	holdtime_t.holdtime = htons(pif->pim_conf->hello_itv * 3.5);
	memcpy(pim_para, &holdtime_t, sizeof(struct pim_hello_opt_holdtime));
	pim_para += sizeof(struct pim_hello_opt_holdtime);
	para_len += sizeof(struct pim_hello_opt_holdtime);
	
	/*put  DR priority*/
	dr_pri_t.opt_type = htons(PIM_HELLO_OPTION_DR_PRIORITY);
	dr_pri_t.opt_len = htons(PIM_OPTION_DR_PRIORITY_LEN);
	dr_pri_t.dr_priority = htonl(pif->pim_conf->dr_priority);
	memcpy(pim_para, &dr_pri_t, sizeof(struct pim_hello_opt_dr_pri));
	pim_para += sizeof(struct pim_hello_opt_dr_pri);
	para_len += sizeof(struct pim_hello_opt_dr_pri);

	/*put gen id*/
	gen_id_t.opt_type = htons(PIM_HELLO_OPTION_GEN_ID);
	gen_id_t.opt_len = htons(PIM_OPTION_GEN_ID_LEN);
	gen_id_t.gen_id = htonl(pif->pim_nbr->gen_id);
	memcpy(pim_para, &gen_id_t, sizeof(struct pim_hello_opt_gen_id));
	pim_para += sizeof(struct pim_hello_opt_gen_id);
	para_len += sizeof(struct pim_hello_opt_gen_id);
	
	/*put lan delay ,T not set*/
	lan_delay_t.opt_type = htons(PIM_HELLO_OPTION_LAN_DELAY);
	lan_delay_t.opt_len = htons(PIM_OPTION_LAN_DELAY_LEN);
	lan_delay_t.lan_delay = htons(pif->pim_conf->lan_delay);
	lan_delay_t.override = htons(pif->pim_conf->override);
	memcpy(pim_para, &lan_delay_t, sizeof(struct pim_hello_opt_lan_delay));
	pim_para += sizeof(struct pim_hello_opt_lan_delay);
	para_len += sizeof(struct pim_hello_opt_lan_delay);
	
	/*put sec_addr...*/

	/*pim para length*/
	len = (uint8_t *)pim_para - (uint8_t *)ipmc_send_buf;
	
	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_HELLO;
	pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
	
	/*ip hdr */
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	pkt_ctrl.ipcb.ifindex = pif->ifindex;
	pkt_ctrl.ipcb.sip = ipmc_if_main_addr_get(pif->ifindex);
	pkt_ctrl.ipcb.dip = ALLPIM_ROUTER_GROUP;
	pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IPMC;
	pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
	pkt_ctrl.ipcb.tos = 4;	/*hello msg not contain ip priority*/
	pkt_ctrl.ipcb.ttl = 1;
	pkt_ctrl.ipcb.is_changed = 1;

	ret = pkt_send(PKT_TYPE_IPMC, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		zlog_err("%s, %d pkt_send fail!\n",__FUNCTION__, __LINE__);
	}
	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
	#if 0
	if(reset_timer)
	{
		/*周期发送*/
		if(pif->pim_nbr->hello_timer)
		{
			thread_cancel(pif->pim_nbr->hello_timer);
			pif->pim_nbr->hello_timer = NULL;
		}
		/*pif->pim_nbr->hello_timer = thread_add_timer(ipmc_master, pim_hello_send_timer_expire, 
									(void *)pif, pif->pim_conf->hello_itv);*/
		pif->pim_nbr->hello_timer = high_pre_timer_add("PimHelloTimer",(reset_timer ? LIB_TIMER_TYPE_LOOP : LIB_TIMER_TYPE_NOLOOP),
					pim_hello_send_timer_expire,(void *)pif,
					pif->pim_conf->hello_itv);
	}
	#endif
	if(!(pif->pim_nbr->hello_timer))
	{
		pif->pim_nbr->hello_timer = high_pre_timer_add("PimHelloTimer",(reset_timer ? LIB_TIMER_TYPE_LOOP : LIB_TIMER_TYPE_NOLOOP),
					pim_hello_send_timer_expire,(void *)pif,
					1000 * pif->pim_conf->hello_itv);
	}
	else
	{
	}
	return ret;
}

void pim_nbr_stop(struct ipmc_if *pif)
{
	struct pim_neighbor *pnbr = NULL;
	struct pim_neighbor_node *pnbr_node = NULL;
	struct listnode  *pnode	 = NULL;
	struct listnode  *pnextnode = NULL;
	unsigned int val = 0;
	struct hash_table *grp_table = NULL, *sg_grp_table = NULL;
	struct hash_bucket *pbucket1 = NULL, *pbucket2 = NULL;
	struct pim_down_jp_grp_node *grp_node = NULL;
	struct pim_down_jp_sg_nodes *sg_grp = NULL;
	struct pim_down_jp_sg_node *sg_node = NULL;
	uint32_t grp_addr = 0, cursor = 0;

	/*立即老化*/
	/*将该接口下的所有pim信息删除*/
	/*pim nbr, 接口删除，主动将所有的邻居删除*/
	if(pif == NULL)
	{
		IPMC_LOG_NOTICE("pif == NULL\n");
		return ;
	}
	if(pif->dw_wc_jp.grp_num)
	{	
		grp_table = &(pif->dw_wc_jp.grp_table);
		for( val = 0; val < HASHTAB_SIZE; val++)
		{
			pbucket1 = grp_table->buckets[val];
			while(pbucket1)
			{
				grp_node = (struct pim_down_jp_grp_node *)pbucket1->data;
				grp_addr = grp_node->grp_addr;
				/*接口下的所有grp_node删除*/
				pim_jp_down_wc_state_machine(grp_node, PIM_JP_DOWN_EVENT_IFM_DEL);
				hios_hash_delete(grp_table, pbucket1);
				XFREE(MTYPE_IPMC_PIM_JP_GRP_ENTRY, grp_node);
				pbucket1->prev = NULL;
				pbucket1->next = NULL;
				XFREE(MTYPE_HASH_BACKET, pbucket1);
				pbucket1 = grp_table->buckets[val];
				
				/*sg/sgrpt删除*/
				sg_grp_table = &(pif->dw_sg_jp.sg_grp_table);
				sg_grp = pim_jp_down_sg_grp_node_lookup(sg_grp_table, grp_addr);
				if(sg_grp == NULL)
				{
					continue;
				}
				HASH_BUCKET_LOOP(pbucket2, cursor, sg_grp->sg_table)
				{
					if(pbucket2->data)
					{
						sg_node = (struct pim_down_jp_sg_node *)pbucket2->data;
						if(sg_node->type & PIM_JP_SG_RPT_TYPE)
						{
							pim_jp_down_sg_rpt_state_machine(sg_node, PIM_JP_DOWN_EVENT_IFM_DEL);
						}
						else if(sg_node->type & PIM_JP_SG_TYPE)
						{
							pim_jp_down_sg_state_machine(sg_node, PIM_JP_DOWN_EVENT_IFM_DEL);
						}
					}
				}
				/*删除sg_grp*/
				pim_jp_down_sg_grp_node_del(sg_grp_table, grp_addr);
			}
		}
		pif->dw_wc_jp.grp_num = 0;
	}
	
	if(pif->dw_sg_jp.grp_num)
	{
		grp_table = &(pif->dw_sg_jp.sg_grp_table);
		for( val = 0; val < HASHTAB_SIZE; val++)
		{
			pbucket1 = grp_table->buckets[val];
			while(pbucket1)
			{
				sg_grp = (struct pim_down_jp_sg_nodes *)pbucket1->data;
				grp_addr = sg_grp->grp_addr;
				HASH_BUCKET_LOOP(pbucket2, cursor, sg_grp->sg_table)
				{
					if(pbucket2->data)
					{
						sg_node = (struct pim_down_jp_sg_node *)pbucket2->data;
						if(sg_node->type & PIM_JP_SG_RPT_TYPE)
						{
							pim_jp_down_sg_rpt_state_machine(sg_node, PIM_JP_DOWN_EVENT_IFM_DEL);
						}
						else if(sg_node->type & PIM_JP_SG_TYPE)
						{
							pim_jp_down_sg_state_machine(sg_node, PIM_JP_DOWN_EVENT_IFM_DEL);
						}
					}
				}
				/*删除sg_grp*/
				pim_jp_down_sg_grp_node_del(sg_grp_table, grp_addr);
				pbucket1 = pbucket1->next;
			}
		}
	}
	pnbr = pif->pim_nbr;
	for(ALL_LIST_ELEMENTS(&(pnbr->neighbor_list), pnode, pnextnode, pnbr_node))
	{
		pim_nbr_node_delete(pnbr, pnbr_node);
	}
	
}

void pim_nbr_start(struct ipmc_if *pif)
{
	if(pif->pim_nbr == NULL)
	{
		IPMC_LOG_ERROR("pif->pim_nbr == NULL\n");
		return ;
	}
	pif->pim_nbr->cur_dr.dr_state |= PIM_DR_LOCAL;
	pif->pim_nbr->cur_dr.dr_priority = pif->pim_conf->dr_priority;
	pif->pim_nbr->cur_dr.dr_addr = ipmc_if_main_addr_get(pif->ifindex);
	pim_dr_election(pif);
	srandom(time(NULL));
	pif->pim_nbr->gen_id = (uint32_t)(random());
    pim_hello_send(pif, IPMC_FALSE);
    pim_hello_send(pif, IPMC_TRUE);
}

