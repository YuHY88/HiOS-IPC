/* 
*  multicast interface management 
*/

#ifndef IPMC_IF_H
#define IPMC_IF_H

#include <lib/types.h>
#include <lib/hash1.h>
#include <lib/linklist.h>
#include <lib/thread.h>

#include "ipmc.h"
#include "pim/pim_neighbor.h"
#include "pim/pim_join_prune.h"
#include "pim/pim.h"
#include "igmp/igmp.h"

/* 组播接口数据结构 */
struct ipmc_if
{
    uint32_t  ifindex;        /* multicast group id */
    uint8_t   index;       	  /* ipmc_if index in pim_instance */
	uint8_t   link_flag;      /* 0: linkup, 1: linkdown */	
	uint8_t   pim_mode;		  /* 0: disable, 1: pim sm, 2: pim dm */
	uint8_t   igmp_flag;      /* 0: disable, 1: igmp enable 2: igmp proxy enable, 3: igmp snooping enable */
	struct pim_config *pim_conf;       /* ifm store pim config */
	struct pim_neighbor *pim_nbr; /* pim interface and pim neighbor info */
	struct pim_down_jp_grp_node_s dw_wc_jp; /* ifm store all (*,G) j/p node*/
	struct pim_down_jp_sg_grp_node_s dw_sg_jp;	 /* ifm store all (S,G) j/p node*/
	struct igmp *igmp;        /* igmp info */
};

/*初始化，添加接口(ifindex, index)，删除接口(ifindex, index)*/
struct ipmc_if_t{
	uint32_t ipmc_if_num;
	uint32_t ipmc_if_array[IPMC_IF_MAX];
};

/* Hash of multicast interface table */
extern struct hash_table ipmc_if_table;
extern struct ipmc_if_t ipmc_if_s;

/* Prototypes of multicast interface hash */
extern void ipmc_if_table_init(unsigned int size);
extern struct ipmc_if * ipmc_if_add(uint32_t  ifindex);       /* 添加接口 */
extern int ipmc_if_delete(uint32_t  ifindex);   /* 删除接口 */
extern struct ipmc_if* ipmc_if_pim_enable(uint32_t ifindex, uint8_t pim_mode);
extern sint32 ipmc_if_pim_disable(uint32_t ifindex, uint8_t pim_mode);
extern struct ipmc_if *ipmc_if_lookup(uint32_t  ifindex); /* 查找接口 */
extern int ipmc_if_mode_change(uint32_t ifindex, int mode);    /* 处理接口模式改变事件 */
extern sint32 ipmc_if_up(uint32_t ifindex);
extern sint32 ipmc_if_down(uint32_t ifindex);
extern uint32_t ipmc_if_main_addr_get(uint32_t ifindex);
int ipmc_if_main_addr_mask_get(uint32_t ifindex);

#endif
