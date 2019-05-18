/*
 * =====================================================================================
 *
 *       Filename:  ipmc_if.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/12/2017 05:46:22 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <string.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <lib/ifm_common.h>

#include "route/route_main.h"
#include "route/route_if.h"
#include "ipmc.h"
#include "ipmc_main.h"
#include "ipmc_if.h"
#include "pim/pim_join_prune.h"

/* Hash of multicast interface table */

struct hash_table ipmc_if_table;
struct ipmc_if_t ipmc_if_s;
extern struct route_if *route_if_lookup(uint32_t ifindex);

static uint32_t ipmc_if_hash(void *hash_key)
{	
    return ((unsigned int) hash_key);
}

static int ipmc_if_compare(void *item, void *hash_key)
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

/* init multicast  interface table*/
void ipmc_if_table_init(unsigned int size)
{
	hios_hash_init(&ipmc_if_table, size, ipmc_if_hash, ipmc_if_compare);
	memset(&ipmc_if_s, 0, sizeof(struct ipmc_if_t));
}

/*add an interface to multicast  interface table*/
struct ipmc_if* ipmc_if_add(uint32_t ifindex)
{
	struct hash_bucket *pitem = NULL;
	struct ipmc_if *pmcif = NULL;
	struct mcif_t mcif;
	uint32_t ret = ERRNO_FAIL, index = 0;

	pitem = hios_hash_find(&ipmc_if_table, (void *)ifindex);
	if (NULL != pitem)
	{
		return (struct ipmc_if*)pitem->data;
	}
	
	if(ipmc_if_s.ipmc_if_num >= IPMC_IF_MAX)
	{
		IPMC_LOG_ERROR("IPMC if number exist MAX %u, \n", IPMC_IF_MAX);
		return NULL;
	}
	for(index = 0; index < IPMC_IF_MAX; index++)
	{
		if(ipmc_if_s.ipmc_if_array[index] == 0)
		{
			/*下发到hal,  add, ifindex,  index*/
			mcif.ifindex = ifindex;
			mcif.index = index;

			ret = ipmc_msg_send_to_hal((void*)(&mcif),sizeof(struct mcif_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
								 IPMC_SUBTYPE_IF, IPC_OPCODE_ADD, ipmc_if_s.ipmc_if_num);
			/*ret = ipc_send_hal((void *)(&mcif), sizeof(struct mcif_t), 1, MODULE_ID_HAL, MODULE_ID_IPMC, IPC_TYPE_IPMC,
								 IPMC_SUBTYPE_IF, IPC_OPCODE_ADD, ipmc_if_s.ipmc_if_num);*/
			if(ret)
			{
				IPMC_LOG_ERROR("Failed to send add ipmc_if to hal.\n");
				return NULL;
			}
			ipmc_if_s.ipmc_if_num++;
			ipmc_if_s.ipmc_if_array[index] = ifindex;
			IPMC_LOG_DEBUG("IPMC ifindex:%x, alloc index:%u", ifindex, index);
			break;
		}
	}
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	IPMC_LOG_ERROR("ifindex %u XCALLOC(struct hash_bucket) failed!\n", ifindex);
        return NULL;
    }

	pmcif = (struct ipmc_if *)XCALLOC(MTYPE_IPMC_IF_ENTRY, sizeof(struct ipmc_if));
	if (NULL == pmcif)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
    	IPMC_LOG_ERROR("ifindex %u XCALLOC(struct ipmc_if) failed!\n", ifindex);
        return NULL;
	}
	memset(pmcif, 0, sizeof(struct ipmc_if));
	
	/*对hash_table 进行初始化*/
	pim_jp_down_grp_table_init(&(pmcif->dw_wc_jp.grp_table), PIM_GRP_MAX);
	pim_jp_down_grp_table_init(&(pmcif->dw_sg_jp.sg_grp_table), PIM_GRP_MAX);

	pmcif->ifindex = ifindex;
	pmcif->index = index;
	pitem->hash_key = (void *)pmcif->ifindex;
	pitem->data = (void *)pmcif;

	hios_hash_add(&ipmc_if_table, pitem);
	
	return (struct ipmc_if *)pitem->data;
}

sint32 ipmc_if_delete(uint32_t ifindex)
{
	struct hash_bucket *pbucket = NULL;
	struct ipmc_if *pif = NULL;
	uint32_t index = 0;
	
	pbucket = hios_hash_find(&ipmc_if_table, (void *)ifindex);
    if (NULL == pbucket)
    {
        return ERRNO_NOT_FOUND;
    }
	
    hios_hash_delete(&ipmc_if_table, pbucket);
	pif = (struct ipmc_if *)pbucket->data;
	ifindex = pif->ifindex;
	index = pif->index;
	if(ipmc_if_s.ipmc_if_array[index] == ifindex)
	{
		ipmc_if_s.ipmc_if_array[index] = 0;
		ipmc_if_s.ipmc_if_num--;
	}
	XFREE(MTYPE_IPMC_IF_ENTRY, pif);
    XFREE(MTYPE_HASH_BACKET, pbucket);

    return ERRNO_SUCCESS;
}

/*add an interface to multicast  interface table*/
struct ipmc_if* ipmc_if_pim_enable(uint32_t ifindex, uint8_t pim_mode)
{
	struct ipmc_if *mcif = NULL;
	struct pim_neighbor *pnbr = NULL;
	struct pim_config *pim_conf = NULL;

	if(pim_mode != PIM_SM)
	{
		IPMC_LOG_NOTICE("Current pim-dm not support, only pim-sm support!\n");
		return NULL;
	}
	mcif = ipmc_if_lookup(ifindex);
	if(mcif == NULL)
	{
		mcif = ipmc_if_add(ifindex);
		if(mcif == NULL)
		{
			IPMC_LOG_NOTICE("ipmc_if_add(%x) return NULL\n", ifindex);
			return NULL;
		}
	}
	else
	{
		if(mcif->pim_mode == PIM_SM)
		{
			IPMC_LOG_NOTICE("ifindex %u in pim sm mode already!\n", ifindex);
			return mcif;
		}
	}
	/*初始化pim*/
	pnbr = (struct pim_neighbor *)XCALLOC(MTYPE_IPMC_PIM_NBR_ENTRY, sizeof(struct pim_neighbor));
	if (NULL == pnbr)
	{
		IPMC_LOG_ERROR("ifindex %u XCALLOC(struct pim_neighbor) failed!\n", ifindex);
		return NULL;
	}
	memset(pnbr, 0, sizeof(struct pim_neighbor));
	pim_conf = (struct pim_config *)XCALLOC(MTYPE_IPMC_PIM_CONF_ENTRY, sizeof(struct pim_config));
	if (NULL == pim_conf)
	{
		XFREE(MTYPE_IPMC_PIM_NBR_ENTRY, pnbr);
		IPMC_LOG_ERROR("ifindex %u XCALLOC(struct pim_neighbor) failed!\n", ifindex);
		return NULL;
	}
	memset(pim_conf, 0, sizeof(struct pim_config));
	mcif->pim_nbr = pnbr;
	mcif->pim_conf = pim_conf;
	mcif->pim_conf->dr_priority = PIM_DR_PRIORITY_DEFAULT;
	mcif->pim_conf->hello_itv = PIM_HELLO_PERIOD_DEFAULT;
	mcif->pim_conf->lan_delay = PIM_LAN_DELAY_DEFAULT;
	mcif->pim_conf->override = PIM_OVERRIDE_DEFAULT;
	mcif->pim_mode = pim_mode;
	pim_nbr_start(mcif);
	
	return mcif;
}

/*add an interface to multicast  interface table*/
sint32 ipmc_if_pim_disable(uint32_t ifindex, uint8_t pim_mode)
{
	struct ipmc_if *mcif = NULL;
	sint32 ret = ERRNO_SUCCESS;
	
	if(pim_mode != PIM_SM)
	{
		IPMC_LOG_NOTICE("Current pim-dm not support, only pim-sm support!\n");
		return ERRNO_UNMATCH;
	}
	mcif = ipmc_if_lookup(ifindex);
	if(mcif == NULL)
	{
		IPMC_LOG_NOTICE("ipmc_if (%x) == NULL\n", ifindex);
		return ERRNO_NOT_FOUND;
	}
	else
	{
		if(mcif->pim_mode != PIM_SM)
		{
			IPMC_LOG_ERROR("ifindex %u not in pim sm mode!\n", ifindex);
			return ERRNO_UNMATCH;
		}
	}
	/*接口禁用pim sm*/
	pim_nbr_stop(mcif);

	mcif->pim_mode = 0;	
	XFREE(MTYPE_IPMC_PIM_NBR_ENTRY, mcif->pim_nbr);
	XFREE(MTYPE_IPMC_PIM_CONF_ENTRY, mcif->pim_conf);

	/*查看是否使能igmp*/
	if(mcif->igmp_flag == 0)
	{
		ret = ipmc_if_delete(mcif->ifindex);
	}
	return ret;
}

/*lookup interface from multicast  interface table*/
struct ipmc_if *ipmc_if_lookup(uint32_t ifindex)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&ipmc_if_table, (void *)ifindex);
    if (NULL == pitem)
    {
        return NULL;
    }
	
    return (struct ipmc_if *)pitem->data;
}

/*process interface mode change events*/
sint32 ipmc_if_mode_change(uint32_t ifindex, int mode)
{
	struct ipmc_if *mcif = NULL;

	mcif = ipmc_if_lookup(ifindex);
	if (NULL == mcif)
	{
		return ERRNO_FAIL;
	}

	if (IFNET_MODE_L3 != mode)
	{
		ipmc_if_delete(ifindex);
	}

	return ERRNO_SUCCESS;
}

/* process interface up event*/
sint32 ipmc_if_up(uint32_t ifindex)
{
    struct ipmc_if *pif = NULL;

    pif = ipmc_if_lookup(ifindex);
    if(NULL == pif)
    {
        return ERRNO_NOT_FOUND;
    }

    pif->link_flag = LINK_UP;


    return ERRNO_SUCCESS;
}

/* process interface up event*/
sint32 ipmc_if_down(uint32_t ifindex)
{
    struct ipmc_if *pif = NULL;

    pif = ipmc_if_lookup(ifindex);
    if(NULL == pif)
    {
        return ERRNO_NOT_FOUND;
    }

    pif->link_flag = LINK_DOWN;


    return ERRNO_SUCCESS;
}

uint32_t ipmc_if_main_addr_get(uint32_t ifindex)
{
	/*根据接口索引获取接口的主地址*/
	struct ifm_l3 pl3if;

	if(ifm_get_l3if(ifindex, MODULE_ID_IPMC, &pl3if) == 0)
	{
		return pl3if.ipv4[0].addr;
	}
	
	return 0;
}

int ipmc_if_main_addr_mask_get(uint32_t ifindex)
{
	/*根据接口索引获取接口的主地址*/
	struct ifm_l3 pl3if;
	
	if(ifm_get_l3if(ifindex, MODULE_ID_IPMC, &pl3if) == 0)
	{
		return pl3if.ipv4[0].prefixlen;
	}
	
	return 0;
}

