/*
*   PIM 初始化相关
*/

#include <string.h>
#include <unistd.h>
#include <lib/memory.h>
#include <lib/vty.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/zassert.h>
#include <lib/command.h>

#include "pim.h"
#include "../ipmc_main.h"
#include "../ipmc_if.h"

struct pim_instance *pim_instance_global;

#if 0
struct pim_instance *pim_instance_array[PIM_INSTANCE_MAX];

struct pim_instance * pim_instance_create(uint8_t pim_id)
{
	struct pim_instance *pinstance = NULL;

	if(pim_instance_array[pim_id] != NULL )
	{
		return pim_instance_array[pim_id];
	}

	pinstance = (struct pim_instance *)XCALLOC(MTYPE_IPMC_PIM_INSTANCE_ENTRY, sizeof(struct pim_instance));
	if (NULL == pinstance)
	{
		IPMC_LOG_ERROR("XCALLOC(struct pim_instance) failed\n");
		return NULL;
	}
    memset(pinstance, 0, sizeof(struct pim_instance));
	/*初始化哈希表*/
	pim_mrt_sg_grp_table_init(&(pinstance->pim_wc_table), PIM_MRT_WC_MAX);
	pim_mrt_sg_grp_table_init(&(pinstance->pim_sg_table), PIM_MRT_SG_MAX);
	pim_mrt_sg_grp_table_init(&(pinstance->same_src_table),PIM_MRT_SG_MAX);
	
	pinstance->pim_id = pim_id;
	
	return pinstance;
}

sint32 pim_instance_delete(uint8_t pim_id)
{
	uint16_t index = 0;
	uint32_t ifindex = 0;
	struct ipmc_if *pif = NULL;
	struct pim_instance *pinstance = pim_instance_array[pim_id];

	if(pinstance == NULL)
	{
		return ERRNO_NOT_FOUND;
	}
	/*删除一个pim实例，将其下的接口去使能*/
	if(pinstance->pim_if_num)
	{
		/*接口去使能pim功能*/
		for(index = 0; index < PIM_IF_MAX && pinstance->pim_if_num; index++)
		{
			//ifindex = pinstance->pim_if_array[index];
			ifindex = ipmc_if_t.ipmc_if_array[index];
			pif = ipmc_if_lookup(ifindex);
			if(pif && pif->ifindex == ifindex && pif->pim_id == pim_id)
			{
				pim_nbr_stop(pif);
				pim_instance_ipmc_if_delete(pif);
			}
		}
	}
	
	XFREE(MTYPE_IPMC_PIM_INSTANCE_ENTRY, pinstance);
	
	pim_instance_array[pim_id] = NULL;
	
	return ERRNO_SUCCESS;
}


sint32 pim_instance_ipmc_if_add(struct ipmc_if *pif, struct pim_instance *ppim)
{
	uint32_t index = 0;
	
	if(!pif || !ppim)
	{
		return ERRNO_NOT_FOUND;
	}
	if(ppim->pim_if_num >= PIM_IF_MAX)
	{
		return ERRNO_OVERSIZE;
	}
	/*查看pim实例是否已经存在pif*/
	index = pif->index;
	if(index >= PIM_IF_MAX)
	{
		IPMC_LOG_NOTICE("index:%u\n", index);
		pif->index = 0;
	}
	if(ppim->pim_if_array[index] == pif->ifindex)
	{
		IPMC_LOG_NOTICE("index:%u, pim_if_array[index] = %x\n", index, pif->ifindex);
		return ERRNO_SUCCESS;
	}
	for(index = 0; index < PIM_IF_MAX; index++)
	{
		if(ppim->pim_if_array[index] == 0)
		{
			ppim->pim_if_num++;
			ppim->pim_if_array[index] = pif->ifindex;
			pif->index = index;
			IPMC_LOG_NOTICE("ifindex:%x, alloc index:%u", pif->ifindex, index);
			/*接口启动*/
			//pim_nbr_start(pif);
			break;
		}
	}
	return ERRNO_SUCCESS;
}


sint32 pim_instance_ipmc_if_delete(struct ipmc_if *pif)
{
	uint8_t index = 0;
	uint32_t ifindex = 0; 
	struct pim_instance *pinstance = NULL;

	if(!pif)
	{
		return ERRNO_NOT_FOUND;
	}
	ifindex = pif->ifindex;
	pinstance = pim_instance_global;
	if(pinstance && pinstance->pim_if_num)
	{
		for(index = 0; index < PIM_IF_MAX; index++)
		{
			if(pinstance->pim_if_array[index] == ifindex)
			{
				pinstance->pim_if_array[index] = 0;
				pinstance->pim_if_num--;

				/*删除ipmc_if*/
				return ipmc_if_delete(ifindex);
			}
		}
	}

	return ERRNO_NOT_FOUND;
	
}
#endif

void pim_instance_init()
{
	uint16_t index = 0;
	
	/*pim instance array初始化*/
	#if 0
	for(index = 0; index < PIM_INSTANCE_MAX; index++)
	{
		pim_instance_array[index] = NULL;
	}
	#endif
	pim_instance_global = (struct pim_instance *)XCALLOC(MTYPE_IPMC_PIM_INSTANCE_ENTRY, sizeof(struct pim_instance));
	if (NULL == pim_instance_global)
	{
		IPMC_LOG_ERROR("XCALLOC(struct pim_instance)failed, pim_instance_global == NULL\n");
		return ;
	}
    memset(pim_instance_global, 0, sizeof(struct pim_instance));
	/*初始化ssm哈希表*/
	pim_mrt_sg_grp_table_init(&(pim_instance_global->pim_wc_table), PIM_MRT_WC_MAX);
	pim_mrt_sg_grp_table_init(&(pim_instance_global->pim_sg_table), PIM_MRT_SG_MAX);
	pim_mrt_sg_grp_table_init(&(pim_instance_global->same_src_table),PIM_MRT_SG_MAX);
	
	/*使用默认值*/
	pim_instance_global->pim_conf.dr_priority = PIM_DR_PRIORITY_DEFAULT;
	pim_instance_global->pim_conf.hello_itv = PIM_HELLO_PERIOD_DEFAULT;
	pim_instance_global->pim_conf.lan_delay = PIM_LAN_DELAY_DEFAULT;
	pim_instance_global->pim_conf.override = PIM_OVERRIDE_DEFAULT;
	return ;
}
