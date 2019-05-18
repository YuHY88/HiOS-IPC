#include <lib/log.h>
#include <lib/errcode.h>

#include "../ipmc_if.h"
#include "../ipmc_public.h"
#include "pim_mroute.h"


struct pim_mrt_sg* pim_ssm_sg_add(uint32_t ifindex, uint32_t src_addr, uint32_t grp_addr)
{
	uint32_t index = 0;
	struct ipmc_if *pif = NULL;
	struct pim_mrt_sg* sg = NULL;

	/*根据ifindex查找接口*/
	IPMC_LOG_NOTICE("ifindex:%x, (%x, %x)\n", ifindex, src_addr, grp_addr);
	pif = ipmc_if_lookup(ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("please add ifindex 0x%x to pim instance\n", ifindex);
		return NULL;
	}
	index = pif->index;
	sg = pim_mrt_sg_get(src_addr, grp_addr, IPMC_TRUE);
	if(sg == NULL)
	{
		IPMC_LOG_ERROR("pim_mrt_sg_get(%x, %x, %x, %d) return NULL, failed\n", ifindex, src_addr, grp_addr, IPMC_TRUE);
		return NULL;
	}

	if(!IF_ISSET(index, &(sg->oif.pim_in_oif)))
	{
		/*添加出接口，并且计算出接口*/
		IF_SET(index, &(sg->oif.pim_in_oif));
		sg->flag |= PIM_MRT_SSM;
		sg->flag |= PIM_MRT_SPT;
		sg->flag |= PIM_MRT_CONNECTED;	/*直连接收者*/
		
		/*重新计算出接口*/
		pim_mrt_sg_oif_cal(sg);
		
		if(JoinDesiredSG(sg))
		{
			/*上游状态机*/
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_T);
		}

		/*表项下发*/
		if(sg->flag & PIM_MRT_CACHE)
		{
			pim_mrt_sg_cache_add(sg);
		}
	}
	return sg;
}

uint32_t pim_ssm_sg_del(uint32_t ifindex, uint32_t src_addr, uint32_t grp_addr)
{
	uint32_t index = 0;
	struct ipmc_if *pif = NULL;
	struct pim_mrt_sg* sg = NULL;

	/*根据ifindex查找接口*/
	printf("ifindex:%x, (%x, %x)\n", ifindex, src_addr, grp_addr);
	pif = ipmc_if_lookup(ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("please add ifindex 0x%x to pim instance\n", ifindex);
		return ERRNO_FAIL;
	}
	index = pif->index;
	sg = pim_mrt_sg_get(src_addr, grp_addr, IPMC_FALSE);
	if(sg == NULL)
	{
		IPMC_LOG_ERROR("pim_mrt_sg_get(%x, %x, %x, %d) return NULL, failed\n", ifindex, src_addr, grp_addr, IPMC_FALSE);
		return ERRNO_FAIL;
	}
	/*出接口被删除，可能存在其他的出接口*/
	
	if(IF_ISSET(index, &(sg->oif.pim_in_oif)))
	{
		/*删除出接口，并且计算出接口*/
		IF_CLR(index, &(sg->oif.pim_in_oif));
		
		if(IF_ISZERO(&(sg->oif.pim_in_oif)))
		{
			sg->flag &= ~PIM_MRT_SSM;/*如果没有其他的pim_in_oif，删除ssm标志位*/
			sg->flag &= ~PIM_MRT_CONNECTED;	/*如果没与接收者直连，删除该标志位*/
		}
		/*重新计算出接口*/
		pim_mrt_sg_oif_cal(sg);
		
		if(!JoinDesiredSG(sg))
		{
			/*上游状态机*/
			pim_jp_up_sg_state_machine(sg, NULL, PIM_JP_UP_EVENT_SG_JOINDESIRED_F);
		}

		/*表项下发*/
		if((sg->flag & PIM_MRT_CACHE))
		{
			pim_mrt_sg_cache_add(sg);
		}
	}		

	return ERRNO_SUCCESS;
}

