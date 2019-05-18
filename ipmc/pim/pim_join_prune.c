/*
*   PIM join/prune处理
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
#include <ftm/pkt_ip.h>

#include "pim_join_prune.h"
#include "pim_pkt.h"
#include "pim_mroute.h"
#include "../ipmc_main.h"
#include "../ipmc_public.h"

/* grp addr/src addr as hash_key */
static uint32_t pim_jp_down_grp_hash(void *hash_key)
{	
    return ((unsigned int) (hash_key));
}

static int pim_jp_down_grp_compare(void *item, void *hash_key)
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
void pim_jp_down_grp_table_init(struct hash_table *grp_table, unsigned int size)
{
	hios_hash_init(grp_table, size, pim_jp_down_grp_hash, pim_jp_down_grp_compare);
}
 
static uint32_t pim_jp_down_grp_src_hash(void *hash_key)
{	
	struct pim_down_jp_sg_node *sg_node = (struct pim_down_jp_sg_node *)hash_key;
    return ((unsigned int)(sg_node->src_addr));
}

static int pim_jp_down_grp_src_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;
	struct pim_down_jp_sg_node *sg_data = NULL;
	struct pim_down_jp_sg_node *sg_node = (struct pim_down_jp_sg_node *)hash_key;

	if ((NULL == item) || (NULL == sg_node))
    {
        return ERRNO_FAIL;
    }
	sg_data = (struct pim_down_jp_sg_node *)pbucket->data;
	if(sg_data->grp_addr == sg_node->grp_addr && sg_data->src_addr == sg_node->src_addr && sg_data->type == sg_node->type)
	{
		return ERRNO_SUCCESS;
	}

    return ERRNO_FAIL;
}

void pim_jp_down_grp_src_table_init(struct hash_table *table, unsigned int size)
{
	hios_hash_init(table, size, pim_jp_down_grp_src_hash, pim_jp_down_grp_src_compare);
}

static uint32_t pim_jp_up_grp_src_hash(void *hash_key)
{	
	struct pim_up_jp_sg_node *sg_node = (struct pim_up_jp_sg_node *)hash_key;
    return ((unsigned int)(sg_node->src_addr));
}

static int pim_jp_up_grp_src_compare(void *item, void *hash_key)
{  
	struct hash_bucket *pbucket = item;
	struct pim_up_jp_sg_node *sg_data = NULL;
	struct pim_up_jp_sg_node *sg_node = (struct pim_up_jp_sg_node *)hash_key;

	if ((NULL == item) || (NULL == sg_node))
    {
        return ERRNO_FAIL;
    }

	sg_data = (struct pim_up_jp_sg_node *)pbucket->data;
	if(sg_data->grp_addr == sg_node->grp_addr && sg_data->src_addr == sg_node->src_addr && sg_data->type == sg_node->type)
	{
		return ERRNO_SUCCESS;
	}

    return ERRNO_FAIL;
}

void pim_jp_up_grp_src_table_init(struct hash_table *table, unsigned int size)
{
	hios_hash_init(table, size, pim_jp_up_grp_src_hash, pim_jp_up_grp_src_compare);
}

struct pim_down_jp_grp_node* pim_jp_down_grp_node_add(struct hash_table *grp_table, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_grp_node *grp_node = NULL;

	pitem = hios_hash_find(grp_table, (void *)grp_addr);
	if (NULL != pitem)
	{
		return (struct pim_down_jp_grp_node*)pitem->data;
	}

	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	IPMC_LOG_ERROR("grp %u malloc (struct hash_bucket) failed!\n", grp_addr);
        return NULL;
    }

	grp_node = (struct pim_down_jp_grp_node *)XCALLOC(MTYPE_IPMC_PIM_JP_GRP_ENTRY, sizeof(struct pim_down_jp_grp_node));
	if (NULL == grp_node)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("grp %u malloc(struct pim_down_jp_grp_node) failed!\n", grp_addr);
		return NULL;
	}
	memset(grp_node, 0, sizeof(struct pim_down_jp_grp_node));
	grp_node->grp_addr = grp_addr;

	pitem->hash_key = (void *)grp_addr;
	pitem->data = (void *)grp_node;

	hios_hash_add(grp_table, pitem);

	return grp_node;
}

struct pim_down_jp_grp_node* pim_jp_down_grp_node_lookup(struct hash_table *grp_table, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(grp_table, (void *)grp_addr);
    if (NULL == pitem)
    {
        return NULL;
    }
	
    return (struct pim_down_jp_grp_node *)pitem->data;
}

struct pim_down_jp_grp_node* pim_jp_down_grp_node_get(struct ipmc_if *pif, uint32_t grp_addr)
{
	struct pim_down_jp_grp_node *grp_node = NULL;

	grp_node = pim_jp_down_grp_node_lookup(&(pif->dw_wc_jp.grp_table), grp_addr);
	if(grp_node == NULL)
	{
		grp_node = pim_jp_down_grp_node_add(&(pif->dw_wc_jp.grp_table), grp_addr);
		if(grp_node == NULL)
		{
			return NULL;
		}
		grp_node->pif = pif;
		grp_node->et_timer = NULL;
		grp_node->ppt_timer = NULL;
	}
	return grp_node;
}

struct pim_down_jp_sg_nodes* pim_jp_down_sg_grp_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint16_t holdtime)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;

	pitem = hios_hash_find(&(pif->dw_sg_jp.sg_grp_table), (void *)grp_addr);
	if (NULL != pitem)
	{
		return (struct pim_down_jp_sg_nodes *)pitem->data;
	}
	/*创建pim_down_jp_sg_node_s*/
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	IPMC_LOG_ERROR("grp %u malloc (struct hash_bucket) failed!\n", grp_addr);
        return NULL;
    }
	sg_nodes = (struct pim_down_jp_sg_nodes *)XCALLOC(MTYPE_IPMC_PIM_JP_SG_GRP_ENTRY, sizeof(struct pim_down_jp_sg_nodes));
	if (NULL == sg_nodes)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("grp %u malloc(struct pim_down_jp_grp_node) failed!\n", grp_addr);
		return NULL;
	}
	sg_nodes->grp_addr = grp_addr;
	sg_nodes->sg_num = 0;
	sg_nodes->sg_rpt_num = 0;
	sg_nodes->holdtime = holdtime;

	/*初始化sg_table*/
	pim_jp_down_grp_src_table_init(&(sg_nodes->sg_table), PIM_SRC_MAX);

	pitem->hash_key = (void *)grp_addr;
	pitem->data = (void *)sg_nodes;

	hios_hash_add(&(pif->dw_sg_jp.sg_grp_table), pitem);

	return sg_nodes;
}

struct pim_down_jp_sg_nodes * pim_jp_down_sg_grp_node_lookup(struct hash_table *grp_table, uint32_t grp_addr)
{
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(grp_table, (void *)grp_addr);
    if (NULL == pitem)
    {
        return NULL;
    }
	
    return (struct pim_down_jp_sg_nodes *)pitem->data;
}

uint32_t pim_jp_down_sg_grp_node_del(struct hash_table *sg_grp_table, uint32_t grp_addr)
{
	struct pim_down_jp_sg_nodes *sg_grp = NULL;
	struct hash_bucket *pitem = NULL;
	
	pitem = hios_hash_find(sg_grp_table, (void *)grp_addr);
    if (NULL == pitem)
    {
        return NULL;
    }

	hios_hash_delete(sg_grp_table, pitem);
	sg_grp = (struct pim_down_jp_sg_nodes *)pitem->data;
	XFREE(MTYPE_IPMC_PIM_JP_SG_GRP_ENTRY, sg_grp);
    pitem->prev = NULL;
    pitem->next = NULL;
	pitem->data = NULL;
	XFREE(MTYPE_HASH_BACKET, pitem);
	
	return ERRNO_SUCCESS;
}

struct pim_down_jp_sg_node * pim_jp_down_sg_grpsg_node_add(struct ipmc_if *pif, struct hash_table *sg_table, uint32_t grp_addr, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_node *sg_node = NULL;
	struct pim_down_jp_sg_node sg = {0};

	sg.grp_addr = grp_addr;
	sg.src_addr = src_addr;
	sg.type = PIM_JP_SG_TYPE;
	pitem = hios_hash_find(sg_table, (void *)(&sg));
	if (NULL != pitem)
	{
		return (struct pim_down_jp_sg_node *)pitem->data;
	}
	/*创建pim_down_jp_sg_node_s*/
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	IPMC_LOG_ERROR("(%x, %x) XCALLOC(struct hash_bucket) failed!\n", src_addr, grp_addr);
        return NULL;
    }
	sg_node = (struct pim_down_jp_sg_node *)XCALLOC(MTYPE_IPMC_PIM_JP_SG_GRPSG_ENTRY, sizeof(struct pim_down_jp_sg_node));
	if (NULL == sg_node)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("(%x, %x) XCALLOC(struct pim_down_jp_sg_node) failed!\n", src_addr, grp_addr);
		return NULL;
	}
	sg_node->grp_addr = grp_addr;
	sg_node->src_addr = src_addr;
	sg_node->type = PIM_JP_SG_TYPE;
	sg_node->state = PIM_JP_DOWN_STATE_NOINFO;
	sg_node->ppt_timer = NULL;
	sg_node->et_timer = NULL;
	sg_node->pif = pif;

	pitem->hash_key = (void *)sg_node;
	pitem->data = (void *)sg_node;

	hios_hash_add(sg_table, pitem);

	return sg_node;
}

struct pim_down_jp_sg_node * pim_jp_down_sg_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime)
{
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node *sg_node = NULL;

	sg_nodes = pim_jp_down_sg_grp_node_add(pif, grp_addr, holdtime);
	if(sg_nodes == NULL)
	{
		return NULL;
	}
	sg_node = pim_jp_down_sg_grpsg_node_add(pif, &(sg_nodes->sg_table), grp_addr, src_addr);

	return sg_node;
}

uint32_t pim_jp_down_sg_node_del(struct pim_down_jp_sg_node * sg)
{
	struct ipmc_if *pif = NULL;
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;

	if ( NULL == sg )
	{
		return ERRNO_FAIL;
	}

	if(sg && sg->pif)
	{
		pif = sg->pif;
	}
	
	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), sg->grp_addr);
	if(sg_nodes == NULL )
	{
		IPMC_LOG_ERROR("pim_jp_down_sg_grp_node_lookup return NULL, failed\n");
		return ERRNO_FAIL;
	}
	
	pitem = hios_hash_find(&(sg_nodes->sg_table), (void *)sg);
	sg = (struct pim_down_jp_sg_node *)pitem->data;
	
	hios_hash_delete(&(sg_nodes->sg_table), pitem);
	sg_nodes->sg_num--;
	XFREE(MTYPE_IPMC_PIM_JP_SG_GRPSG_ENTRY, sg);
    pitem->prev = NULL;
    pitem->next = NULL;
	pitem->data = NULL;
	XFREE(MTYPE_HASH_BACKET, pitem);
	return ERRNO_SUCCESS;
}

struct pim_down_jp_sg_node * pim_jp_down_sg_node_lookup(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node sg = {0};

	
	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), grp_addr);
	if(sg_nodes == NULL)
	{
		return NULL;
	}
	
	sg.grp_addr = grp_addr;
	sg.src_addr = src_addr;
	sg.type = PIM_JP_SG_TYPE;
	pitem = hios_hash_find(&(sg_nodes->sg_table), (void *)(&sg));
    if (NULL == pitem)
    {
        return NULL;
    }
	
    return (struct pim_down_jp_sg_node *)(pitem->data);
}

struct pim_down_jp_sg_node * pim_jp_down_sg_node_get(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime)
{
	struct pim_down_jp_sg_node *sg_node = NULL;

	sg_node = pim_jp_down_sg_node_lookup(pif, grp_addr, src_addr);
	if(sg_node == NULL)
	{
		sg_node = pim_jp_down_sg_node_add(pif, grp_addr, src_addr, holdtime);
	}

	return sg_node;
}

struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_lookup(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node sg_node = {0};
	
	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), grp_addr);
	if(sg_nodes == NULL )
	{
		return NULL;
	}
	sg_node.grp_addr = grp_addr;
	sg_node.src_addr = src_addr;
	sg_node.type = PIM_JP_SG_RPT_TYPE;
	pitem = hios_hash_find(&(sg_nodes->sg_table), (void *)(&sg_node));
    if (NULL == pitem)
    {
        return NULL;
    }
	
    return (struct pim_down_jp_sg_node *)(pitem->data);
}

struct pim_down_jp_sg_node * pim_jp_down_sg_grpsgrpt_node_add(struct ipmc_if *pif, struct hash_table *sg_table, uint32_t grp_addr, uint32_t src_addr)
{
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;
	struct pim_down_jp_sg_node sg_node = {0};

	sg_node.grp_addr = grp_addr;
	sg_node.src_addr = src_addr;
	sg_node.type = PIM_JP_SG_RPT_TYPE;
	pitem = hios_hash_find(sg_table, (void *)&sg_node);
	if (NULL != pitem)
	{
		return (struct pim_down_jp_sg_node *)pitem->data;
	}
	/*创建pim_down_jp_sg_rpt_node*/
	pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
    	IPMC_LOG_ERROR("(%x, %x) XCALLOC(struct hash_bucket) failed!\n", src_addr, grp_addr);
        return NULL;
    }
	sgrpt_node = (struct pim_down_jp_sg_node *)XCALLOC(MTYPE_IPMC_PIM_JP_SG_GRPSGRPT_ENTRY, sizeof(struct pim_down_jp_sg_node));
	if (NULL == sgrpt_node)
	{
		XFREE(MTYPE_HASH_BACKET, pitem);
		IPMC_LOG_ERROR("(%x, %x) XCALLOC(struct pim_down_jp_sg_node) failed!\n", src_addr, grp_addr);
		return NULL;
	}
	sgrpt_node->grp_addr = grp_addr;
	sgrpt_node->src_addr = src_addr;
	sgrpt_node->type = PIM_JP_SG_RPT_TYPE;
	sgrpt_node->state = PIM_JP_DOWN_STATE_NOINFO;
	sgrpt_node->ppt_timer = NULL;
	sgrpt_node->et_timer = NULL;
	sgrpt_node->pif = pif;

	pitem->hash_key = (void *)sgrpt_node;
	pitem->data = (void *)sgrpt_node;

	hios_hash_add(sg_table, pitem);

	return sgrpt_node;
}

struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_add(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime)
{
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	sg_nodes = pim_jp_down_sg_grp_node_add(pif, grp_addr, holdtime);
	if(sg_nodes == NULL)
	{
		return NULL;
	}
	sgrpt_node = pim_jp_down_sg_grpsgrpt_node_add(pif, &(sg_nodes->sg_table), grp_addr, src_addr);

	return sgrpt_node;
}

uint32_t pim_jp_down_sgrpt_node_del(struct pim_down_jp_sg_node * sg_rpt)
{
	struct ipmc_if *pif = NULL;
	struct hash_bucket *pitem = NULL;
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;

	if ( NULL == sg_rpt )
	{
		return ERRNO_FAIL;
	}

	if(sg_rpt && sg_rpt->pif)
	{
		pif = sg_rpt->pif;
	}

	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), sg_rpt->grp_addr);
	if(sg_nodes == NULL )
	{
		IPMC_LOG_ERROR("pim_jp_down_sg_grp_node_lookup return NULL, failed\n");
		return ERRNO_FAIL;
	}
	
	pitem = hios_hash_find(&(sg_nodes->sg_table), (void *)sg_rpt);
	sg_rpt = (struct pim_down_jp_sg_node *)pitem->data;
	
	hios_hash_delete(&(sg_nodes->sg_table), pitem);
	sg_nodes->sg_rpt_num--;
	XFREE(MTYPE_IPMC_PIM_JP_SG_GRPSGRPT_ENTRY, sg_rpt);
    pitem->prev = NULL;
    pitem->next = NULL;
	pitem->data = NULL;
	XFREE(MTYPE_HASH_BACKET, pitem);
	return ERRNO_SUCCESS;
}

struct pim_down_jp_sg_node * pim_jp_down_sgrpt_node_get(struct ipmc_if *pif, uint32_t grp_addr, uint32_t src_addr, uint16_t holdtime)
{
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	sgrpt_node = pim_jp_down_sgrpt_node_lookup(pif, grp_addr, src_addr);
	if(sgrpt_node == NULL)
	{
		sgrpt_node = pim_jp_down_sgrpt_node_add(pif, grp_addr, src_addr, holdtime);
	}

	return sgrpt_node;
}

sint32 pim_jp_down_join_process(uint8_t **pim_para, uint16_t join_num, uint32_t grp_addr, struct ipmc_if *pif, uint32_t pim_para_len, uint16_t holdtime)
{
	uint8_t *jbuf = *pim_para;
	uint8_t flag = 0, mask_len = 0;
	uint32_t addr = 0, rp_addr = 0, event = 0;
	struct encode_ipv4_source *src_rp = NULL;
	struct pim_down_jp_grp_node *grp_node = NULL;
	struct pim_down_jp_sg_node *sg_node = NULL;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	while(join_num && (pim_para_len > 0))
	{
		join_num--;
		src_rp = (struct encode_ipv4_source *)jbuf;
		flag = src_rp->flag;
		mask_len = src_rp->mask_len;
		addr = ntohl(src_rp->src_addr);
		
		jbuf += sizeof(struct encode_ipv4_source);
		pim_para_len -= sizeof(struct encode_ipv4_source);

		if(mask_len != PIM_GRP_SRC_MASK_MAX)
		{
			continue;
		}
		/*处理(*,G)加枝, (S)WR bit*/
		if((flag & PIM_JP_R_BIT) && (flag & PIM_JP_W_BIT))
		{
			/*RP(G) == addr 才是有效的(*,G)加枝*/
			rp_addr = pim_rp_check(grp_addr);
			if( rp_addr == addr)
			{
				/*创建或者维护*g下游节点*/
				grp_node = pim_jp_down_grp_node_get(pif, grp_addr);
				if(grp_node)
				{
					grp_node->holdtime = holdtime;
					event = PIM_JP_DOWN_EVENT_WC_J_RCV;
					pim_jp_down_wc_state_machine(grp_node, event);
				}
				else
				{
					IPMC_LOG_ERROR(" pim_jp_down_grp_node_get(0x%x) return NULL, failed\n", grp_addr);
					continue;
				}
			}
			else
			{
				/*无效的(*G)加枝*/
				IPMC_LOG_ERROR("RP(0x%x) = 0x%x, RP in message is 0x%x\n", grp_addr, rp_addr, addr);
				continue;
			}
		}
		else if((flag & PIM_JP_S_BIT) && !(flag & (PIM_JP_W_BIT | PIM_JP_R_BIT)))
		{
			/*处理(S,G)加枝，只有S bit*/
			if(!inet_valid_ipv4(addr))
			{
				continue;
			}
			/*根据src_addr grp_addr 获取sg */
			sg_node = pim_jp_down_sg_node_get(pif, grp_addr, addr, holdtime);
			if(sg_node)
			{
				event = PIM_JP_DOWN_EVENT_SG_J_RCV;
				pim_jp_down_sg_state_machine(sg_node, event);
			}
			else
			{
				IPMC_LOG_ERROR("pim_jp_down_sg_node_get(0x%x, 0x%x) return NULL, failed\n", addr, grp_addr);
				continue;
			}
		}
		else if(!(flag & PIM_JP_W_BIT) && (flag & PIM_JP_R_BIT))
		{
			/*处理(S,G,RPT)加枝，(S)R bit*/
			if(!inet_valid_ipv4(addr))
			{
				continue;
			}
			/*根据src_addr grp_addr 获取sgrpt */
			sgrpt_node = pim_jp_down_sgrpt_node_get(pif, grp_addr, addr, holdtime);
			if(sgrpt_node)
			{
				event = PIM_JP_DOWN_EVENT_SG_RPT_SGRPTJ_RCV;
				pim_jp_down_sg_rpt_state_machine(sgrpt_node, event);
			}
			else
			{
				IPMC_LOG_ERROR("pim_jp_down_sgrpt_node_get(0x%x, 0x%x) return NULL, failed\n", addr, grp_addr);
				continue;
			}
		}
	}
	
	pim_para = &jbuf;
	return ERRNO_SUCCESS;
}

sint32 pim_jp_down_prune_process(uint8_t **pim_para, uint16_t prune_num, uint32_t grp_addr, struct ipmc_if *pif, uint32_t pim_para_len, uint16_t holdtime)
{
	uint8_t *pbuf = *pim_para;
	uint8_t flag = 0, mask_len = 0;
	uint32_t addr = 0, rp_addr = 0, event = 0;
	struct encode_ipv4_source *src_rp = NULL;
	struct pim_down_jp_grp_node *grp_node = NULL;
	//struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node *sg_node = NULL;
	struct pim_down_jp_sg_node *sgrpt_node = NULL;

	while(prune_num && (pim_para_len > 0))
	{
		prune_num--;
		src_rp = (struct encode_ipv4_source *)pbuf;
		flag = src_rp->flag;
		mask_len = src_rp->mask_len;
		addr = ntohl(src_rp->src_addr);
		
		pbuf += sizeof(struct encode_ipv4_source);
		pim_para_len -= sizeof(struct encode_ipv4_source);

		if(mask_len != PIM_GRP_SRC_MASK_MAX)
		{
			continue;
		}
		/*处理(*,G)加枝, (S)WR bit*/
		if((flag & PIM_JP_R_BIT) && (flag & PIM_JP_W_BIT))
		{
			/*RP(G) == addr 才是有效的(*,G)加枝*/
			rp_addr = pim_rp_check( grp_addr);
			if( rp_addr == addr)
			{
				/*创建或者维护*g下游节点*/
				grp_node = pim_jp_down_grp_node_get(pif, grp_addr);
				if(grp_node)
				{
					grp_node->holdtime = holdtime;
					event = PIM_JP_DOWN_EVENT_WC_P_RCV;
					pim_jp_down_wc_state_machine(grp_node, event);
				}
				else
				{
					IPMC_LOG_ERROR("pim_jp_down_grp_node_get(0x%x) return NULL, failed\n", grp_addr);
					continue;
				}
			}
			else
			{
				/*无效的(*G)加枝*/
				IPMC_LOG_ERROR("RP(0x%x) = 0x%x, RP in message is 0x%x\n", grp_addr, rp_addr, addr);
				continue;
			}
		}
		else if((flag & PIM_JP_S_BIT) && !(flag & (PIM_JP_W_BIT | PIM_JP_R_BIT)))
		{
			/*处理(S,G)加枝，只有S bit*/
			if(!inet_valid_ipv4(addr))
			{
				continue;
			}
			/*根据src_addr grp_addr 获取sg */
			sg_node = pim_jp_down_sg_node_get(pif, grp_addr, addr, holdtime);
			if(sg_node)
			{
				event = PIM_JP_DOWN_EVENT_SG_P_RCV;
				pim_jp_down_sg_state_machine(sg_node, event);
			}
			else
			{
				IPMC_LOG_ERROR("pim_jp_down_sg_node_get(0x%x, 0x%x) return NULL, failed\n", addr, grp_addr);
				continue;
			}
		}
		else if(!(flag & PIM_JP_W_BIT) && (flag & PIM_JP_R_BIT))
		{
			/*处理(S,G,RPT)加枝，(S)R bit*/
			if(!inet_valid_ipv4(addr))
			{
				continue;
			}
			/*根据src_addr grp_addr 获取sgrpt */
			sgrpt_node = pim_jp_down_sgrpt_node_get(pif, grp_addr, addr, holdtime);
			if(sgrpt_node)
			{
				event = PIM_JP_DOWN_EVENT_SG_RPT_SGRPTP_RCV;
				pim_jp_down_sg_rpt_state_machine(sgrpt_node, event);
			}
			else
			{
				IPMC_LOG_ERROR("pim_jp_down_sgrpt_node_get(0x%x, 0x%x) return NULL, failed\n", addr, grp_addr);
				continue;
			}
		}
	}
	
	pim_para = &pbuf;
	return ERRNO_SUCCESS;
}
 
sint32 pim_jp_down_end_of_message(uint32_t grp_addr, struct ipmc_if *pif)
{
	struct pim_down_jp_sg_nodes *sg_nodes = NULL;
	struct pim_down_jp_sg_node *sg_rpt = NULL;
	struct pim_down_jp_sg_node *sg = NULL;
	struct hash_bucket *bucket = NULL;
	uint32_t cursor = 0;
	
	sg_nodes = pim_jp_down_sg_grp_node_lookup(&(pif->dw_sg_jp.sg_grp_table), grp_addr);
	if(sg_nodes == NULL )
	{
		return ERRNO_SUCCESS;
	}
	if(sg_nodes->sg_table.num_entries)
	{
		HASH_BUCKET_LOOP(bucket, cursor, sg_nodes->sg_table)
		{
			sg_rpt = (struct pim_down_jp_sg_node *)bucket->data;
			if(sg_rpt->type == PIM_JP_SG_RPT_TYPE)
			{
				pim_jp_down_sg_rpt_state_machine(sg_rpt, PIM_JP_DOWN_EVENT_SG_RPT_END_OF_MSG);
				if(sg_rpt->state == PIM_JP_DOWN_STATE_NOINFO)
				{
					/*del sg_rpt node*/
					pim_jp_down_sgrpt_node_del(sg_rpt);
				}
			}
			else if(sg_rpt->type == PIM_JP_SG_TYPE)
			{
				
			}
		}
	}
	if(sg_nodes->sg_table.num_entries)
	{
		HASH_BUCKET_LOOP(bucket, cursor, sg_nodes->sg_table)
		{
			sg = (struct pim_down_jp_sg_node *)bucket->data;
			if(sg->state == PIM_JP_DOWN_STATE_NOINFO)
			{
				/*del sg node*/
				pim_jp_down_sg_node_del(sg);
			}
		}
	}
	return ERRNO_SUCCESS;
}

/*下游收到j/p报文*/
sint32 pim_jp_downstream_recv(struct ipmc_if *pif, struct pim_neighbor *pim_nbr, uint8_t *pim_para, uint32_t pim_para_len)
{
	uint8_t grp_num = 0, mask_len = 0;
	uint16_t holdtime = 0, join_num = 0, prune_num = 0;
	uint32_t grp_addr = 0, jp_len = 0;
	struct pim_join_prune_hdr jp_hdr = {};
	struct pim_join_prune_grp *jp_grp = NULL;
	//struct pim_down_jp_grp_node *grp_node = NULL;
	
	/* 00       01          00 d2 */
	/* pad     grp_num  holdtime */
	/*结构体encode_ipv4_ucast占8字节*/
	
	//jp_hdr = (struct pim_join_prune_hdr *)pim_para;
	//grp_num = jp_hdr->group_num;
	//holdtime = ntohs(jp_hdr->holdtime);
	ipmc_getc(&pim_para);
	grp_num = ipmc_getc(&pim_para);
	holdtime = ntohs(ipmc_getw(&pim_para));
	pim_para_len -= 4;
	//printf("grp_num:%d, holdtime:%d\n", grp_num, holdtime);
	
	while(grp_num && (pim_para_len > 0))
	{
		grp_num--;
		/*01 00 00 20 225.1.1.1*/
		jp_grp = (struct pim_join_prune_grp *)pim_para;
		grp_addr = ntohl(jp_grp->grp.grp_addr);
		mask_len = jp_grp->grp.mask_len;
		join_num = ntohs(jp_grp->join_num);
		prune_num = ntohs(jp_grp->prune_num);
		
		pim_para += sizeof(struct pim_join_prune_grp);
		pim_para_len -= sizeof(struct pim_join_prune_grp);
		
		/*没有(*,*,RP) 则mask_len == 32*/
		if(mask_len == PIM_GRP_SRC_MASK_MAX && ipv4_is_multicast(grp_addr))
		{
			/*处理加枝*/
			if(join_num)
			{
				pim_jp_down_join_process(&pim_para, join_num, grp_addr, pif, pim_para_len, holdtime);
			}
			/*处理剪枝*/
			if(prune_num)
			{
				pim_jp_down_prune_process(&pim_para, prune_num, grp_addr, pif, pim_para_len, holdtime);
			}
			/*grp end of msg*/
			pim_jp_down_end_of_message(grp_addr, pif);
		}
		else
		{
			jp_len = (join_num + prune_num ) * sizeof(struct encode_ipv4_source);
			pim_para += jp_len;
			pim_para_len -= jp_len;
		}
	}
	return ERRNO_SUCCESS;
}

sint32 pim_jp_upstream_recv(struct ipmc_if *pif, uint32_t upstream , uint8_t *pim_para, uint32_t pim_para_len)
{
	/*主要处理上游的剪枝信息，进行override*/
	/*对于加枝信息，不启动抑制机制，因此暂不处理加枝*/
	uint8_t grp_num = 0, mask_len = 0,  flag = 0;
	uint16_t holdtime = 0, join_num = 0, prune_num = 0;
	uint32_t grp_addr = 0, jp_len = 0, addr = 0;
	struct pim_join_prune_hdr *jp_hdr = NULL;
	struct pim_join_prune_grp *jp_grp = NULL;
	struct encode_ipv4_source *src = NULL;
	struct pim_mrt_wc *wc = NULL;
	struct pim_mrt_sg *sg = NULL;

	/*检查upstream是否为邻居*/
	if(pim_nbr_node_lookup(pif->pim_nbr, upstream) == NULL)
	{
		IPMC_LOG_NOTICE("upstream %x is not a neighbor\n", upstream);
		return ERRNO_NOT_FOUND;
	}
	
	/* 00       01          00 d2 */
	/* pad     grp_num  holdtime */
	jp_hdr = (struct pim_join_prune_hdr *)pim_para;
	grp_num = jp_hdr->group_num;
	holdtime = ntohs(jp_hdr->holdtime);
	pim_para += sizeof(struct pim_join_prune_hdr);
	pim_para_len -= sizeof(struct pim_join_prune_hdr);

	while(grp_num && (pim_para_len > 0))
	{
		/*01 00 00 20 225.1.1.1*/
		jp_grp = (struct pim_join_prune_grp *)pim_para;
		grp_addr = ntohl(jp_grp->grp.grp_addr);
		mask_len = ntohs(jp_grp->grp.mask_len);
		join_num = ntohs(jp_grp->join_num);
		prune_num = ntohs(jp_grp->prune_num);
		
		pim_para += sizeof(struct pim_join_prune_grp);
		pim_para_len -= sizeof(struct pim_join_prune_grp);
		
		/*没有(*,*,RP) 则mask_len == 32*/
		if(mask_len == PIM_GRP_SRC_MASK_MAX && ipv4_is_multicast(grp_addr))
		{
			/*处理上游加枝，暂时不作处理*/
			if(join_num)
			{
				jp_len = join_num * sizeof(struct encode_ipv4_source);
				pim_para += jp_len;
				pim_para_len -= jp_len;
			}
			/*处理上游剪枝*/
			while(prune_num-- && pim_para_len)
			{
				src = (struct encode_ipv4_source *)pim_para;
				pim_para += sizeof(struct encode_ipv4_source);
				pim_para_len -= sizeof(struct encode_ipv4_source);
				flag = src->flag;
				addr = ntohl(src->src_addr);
				mask_len = src->mask_len;
				if(mask_len != PIM_GRP_SRC_MASK_MAX)
				{
					continue;
				}
				/*处理(*,G)剪枝, (S)WR bit*/
				if((flag & PIM_JP_R_BIT) && (flag & PIM_JP_W_BIT))
				{
					/*剪枝中rp != addr，需要处理*/
					wc = pim_mrt_wc_get(grp_addr);
					if(wc && wc->jp_up_state == PIM_JP_UP_STATE_JOINED && wc->rpf_nbr == upstream)
					{
						/*发送一个(*,G)加枝override prune*/
						IPMC_LOG_DEBUG("recv (*,G) prune upstream, wc(*,%x) join send\n", grp_addr);
						pim_jp_up_wc_jp_send(wc->upstream_ifindex, wc->rpf_nbr, grp_addr, wc->rp_addr, PIM_JP_UP_SEND_WC_JOIN);
					}
				}
				else if((flag & PIM_JP_S_BIT) && !(flag & (PIM_JP_W_BIT | PIM_JP_R_BIT)))
				{
					/*处理(S,G)加枝，只有S bit*/
					sg = pim_mrt_sg_get( addr, grp_addr, PIM_LOOKUP);
					if(sg && sg->jp_up_state == PIM_JP_UP_STATE_JOINED && sg->rpf_nbr == upstream)
					{
						/*发送一个sg加枝*/
						IPMC_LOG_DEBUG("recv (S,G) prune upstream, sg(%x,%x)join send\n", addr, grp_addr);
						pim_jp_up_sg_jp_send(sg->upstream_ifindex, sg->rpf_nbr, grp_addr, addr, PIM_JP_UP_SEND_SG_JOIN);
					}
				}
				else if(!(flag & PIM_JP_W_BIT) && (flag & PIM_JP_R_BIT))
				{
					/*处理(S,G,RPT)加枝，(S)R bit*/
					sg = pim_mrt_sg_get( addr, grp_addr, PIM_LOOKUP);
					wc = pim_mrt_wc_get(grp_addr);
					if(sg == NULL || sg->jp_up_rptstate != PIM_JP_UP_STATE_RPT_P)
					{
						if(wc->jp_up_state == PIM_JP_UP_STATE_JOINED && wc->rpf_nbr == upstream)
						{
							/*防止接口被sgrpt减枝报文删除，发送*g加枝override*/
							IPMC_LOG_DEBUG("recv (S,G,RPT) prune upstream, wc(*,%x) join send\n");
							pim_jp_up_wc_jp_send(wc->upstream_ifindex, wc->rpf_nbr, grp_addr, wc->rp_addr, PIM_JP_UP_SEND_WC_JOIN);
						}
					}
				}
			}
		}
		else
		{
			jp_len = (join_num + prune_num ) * sizeof(struct encode_ipv4_source);
			pim_para += jp_len;
			pim_para_len -= jp_len;
		}
	}
	return ERRNO_SUCCESS;
}

sint32 pim_jp_recv(struct ipmc_if *pif, uint32_t sip, uint8_t *pim_para, uint32_t pim_para_len)
{
	sint32 upstream = 0;
	struct encode_ipv4_ucast *ucast = NULL;
	struct pim_neighbor_node *pim_nbr = NULL;

	/*检测sip是否为接口的邻居*/
	if((pim_nbr = pim_nbr_node_lookup(pif->pim_nbr, sip)) == NULL)
	{
		/*收到错误的jp报文*/
		return ERRNO_UNMATCH;
	}
	
	/* 01 00  addr_family encode_type */
	/* 01 01 01 01 unicast_address */
	//ucast = (struct encode_ipv4_ucast *)pim_para;
	ipmc_getc(&pim_para);
	ipmc_getc(&pim_para);
	upstream = ntohl(ipmc_getl(&pim_para));
	//pim_para += sizeof(struct encode_ipv4_ucast);
	pim_para_len -= 6;
	
	/*检测upstream是否为当前设备的地址,返回接口ifindex*/
	if(ip4_addr_local_main(upstream) != 0)
	{
		/*从下游收到j/p报文*/
		pim_jp_downstream_recv(pif, pif->pim_nbr, pim_para, pim_para_len);
	}
	else
	{
		/*从上游收到j/p报文*/
		pim_jp_upstream_recv(pif, upstream, pim_para, pim_para_len);
	}
	return ERRNO_SUCCESS;
}

/*目前只发送一个组，一个加枝/剪枝*/
uint32_t pim_jp_up_wc_jp_send(uint32_t ifindex, uint32_t upstream, uint32_t grp_addr, uint32_t rp_addr, uint8_t type)
{
	struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_para = NULL;
	struct pim_join_prune_hdr jp_hdr;
	struct pim_join_prune_grp jp_grp;
	struct encode_ipv4_source jp_src;
	
	union pkt_control pkt_ctrl;
	uint32_t len = 0, para_len = 0;;
	sint32 ret = ERRNO_FAIL;

	memset(&jp_hdr, 0, sizeof(struct pim_join_prune_hdr));
	memset(&jp_grp, 0, sizeof(struct pim_join_prune_grp));
	memset(&jp_src, 0, sizeof(struct encode_ipv4_source));
	/* encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));
	
	pim_para = (uint8_t *)ipmc_send_buf + sizeof(struct pim_hdr);
	
	/*put jp_hdr*/
	jp_hdr.upstream.addr_family = 1;
	jp_hdr.upstream.encode_type = 0;
	jp_hdr.upstream.unicast_addr = htonl(upstream);
	jp_hdr.reserved = 0;
	jp_hdr.group_num = 1;
	jp_hdr.holdtime = htons(PIM_JP_HOLDTIME);/*default210, jp_interval *3.5*/
	ipmc_setc(&pim_para, jp_hdr.upstream.addr_family);
	ipmc_setc(&pim_para, jp_hdr.upstream.encode_type);
	ipmc_setl(&pim_para, jp_hdr.upstream.unicast_addr);
	ipmc_setc(&pim_para, jp_hdr.reserved);
	ipmc_setc(&pim_para, jp_hdr.group_num);
	ipmc_setw(&pim_para, jp_hdr.holdtime);
	para_len += 10;
	//memcpy(pim_para, &jp_hdr, sizeof(struct pim_join_prune_hdr));
	//pim_para += sizeof(struct pim_join_prune_hdr);
	//para_len += sizeof(struct pim_join_prune_hdr);
		
	/*put jp_grp*/
	jp_grp.grp.addr_family = 1;
	jp_grp.grp.encode_type = 0;
	jp_grp.grp.reserved = 0;
	jp_grp.grp.mask_len = 32;
	jp_grp.grp.grp_addr = htonl(grp_addr);
	if(type == PIM_JP_UP_SEND_WC_JOIN)
	{
		jp_grp.join_num = 1;
		jp_grp.prune_num = 0;
	}
	else
	{
		jp_grp.join_num = 0;
		jp_grp.prune_num = 1;
	}
	memcpy(pim_para, &jp_grp, sizeof(struct pim_join_prune_grp));
	pim_para += sizeof(struct pim_join_prune_grp);
	para_len += sizeof(struct pim_join_prune_grp);
	/*srclist*/
	jp_src.addr_family = 1;
	jp_src.encode_type = 0;
	jp_src.flag = (PIM_JP_S_BIT | PIM_JP_W_BIT | PIM_JP_R_BIT);
	jp_src.mask_len = 32;
	jp_src.src_addr = htonl(rp_addr);
	memcpy(pim_para, &jp_src, sizeof(struct encode_ipv4_source));
	pim_para += sizeof(struct encode_ipv4_source);
	para_len += sizeof(struct encode_ipv4_source);
	
	/*pim para length*/
	len = (uint8_t *)pim_para - (uint8_t *)ipmc_send_buf;
	
	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_JOIN_PRUNE;
	pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
	
	/*ip hdr */
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	pkt_ctrl.ipcb.ifindex = ifindex;
	pkt_ctrl.ipcb.sip = ipmc_if_main_addr_get(ifindex);
	pkt_ctrl.ipcb.dip = ALLPIM_ROUTER_GROUP;
	pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IPMC;
	pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
	pkt_ctrl.ipcb.tos = 4;	/*hello msg not contain ip priority*/
	pkt_ctrl.ipcb.ttl = 1;
	pkt_ctrl.ipcb.is_changed = 1;

	ret = pkt_send(PKT_TYPE_IPMC, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		IPMC_LOG_ERROR("pkt_send fail!\n");
	}
	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);	
	return ret;
}


void pim_jp_up_send_wc_node_del(struct pim_neighbor_node *pnbr_node, uint32_t grp_addr)
{
	struct pim_up_jp_grp_node *pup_wc = NULL;
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&(pnbr_node->up_wc_jp), (void *)grp_addr);
	if(pitem == NULL)
	{
		IPMC_LOG_NOTICE("(*, %x) up_wc not find\n ", grp_addr);
		return ;
	}
	pup_wc = (struct pim_up_jp_grp_node *)pitem->data;

	/*释放pup_wc*/
	if(pup_wc->jt_timer)
	{
		high_pre_timer_delete(pup_wc->jt_timer);
		pup_wc->jt_timer = 0;
	}
	pup_wc->pnbr = NULL;
	
	hios_hash_delete(&(pnbr_node->up_wc_jp), pitem);
	XFREE(MTYPE_IPMC_PIM_JP_UP_GRP_ENTRY, pup_wc);
    pitem->prev = NULL;
    pitem->next = NULL;
	XFREE(MTYPE_HASH_BACKET, pitem);
	
	return ;
}

struct pim_up_jp_grp_node *pim_jp_up_send_wc_node_get(struct pim_neighbor_node *pnbr_node, struct pim_mrt_wc *wc_mrt, uint8_t create_flag)
{
	struct pim_up_jp_grp_node *pup_wc = NULL;
	struct hash_bucket *pitem = NULL;

	pitem = hios_hash_find(&(pnbr_node->up_wc_jp), (void *)wc_mrt->grp_addr);
    if(pitem)
    {
		pup_wc = (struct pim_up_jp_grp_node *)pitem->data;
		if(create_flag == 0)
		{
			return pup_wc;
		}
    }
	else
	{
		if(create_flag == 0)
		{
			return pup_wc;
		}
		/*创建grp_node*/
		pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
		if (NULL == pitem)
		{
			IPMC_LOG_ERROR("grp %u malloc (struct hash_bucket) failed!\n", wc_mrt->grp_addr);
			return NULL;
		}
		
		pup_wc = (struct pim_up_jp_grp_node *)XCALLOC(MTYPE_IPMC_PIM_JP_UP_GRP_ENTRY, sizeof(struct pim_up_jp_grp_node));
		if (NULL == pup_wc)
		{
			XFREE(MTYPE_HASH_BACKET, pitem);
			IPMC_LOG_ERROR("grp %u malloc(struct pim_up_jp_grp_node) failed!\n", wc_mrt->grp_addr);
			return NULL;
		}
		memset(pup_wc, 0, sizeof(struct pim_up_jp_grp_node));
		pup_wc->grp_addr = wc_mrt->grp_addr;
		pup_wc->to_wc = wc_mrt;
		pup_wc->pnbr = pnbr_node;
		pup_wc->jt_timer = NULL;
		
		pitem->hash_key = (void *)wc_mrt->grp_addr;
		pitem->data = (void *)pup_wc;
		
		hios_hash_add(&(pnbr_node->up_wc_jp), pitem);
	}
	
	pup_wc->used_rp = wc_mrt->rp_addr;
	pup_wc->used_rpf = wc_mrt->rpf_nbr;
	pup_wc->ifindex = wc_mrt->upstream_ifindex;
	if(wc_mrt->jp_up_state == PIM_JP_UP_STATE_JOINED)
	{
		pup_wc->state = PIM_JP_UP_SEND_WC_JOIN;
	}
	else
	{
		pup_wc->state = PIM_JP_UP_SEND_WC_PRUNE;
	}

	return pup_wc;
}

struct pim_up_jp_grp_node * pim_jp_up_send_wc_jp_upstream(struct pim_mrt_wc *wc_mrt, uint32_t create_flag)
{
	struct ipmc_if *pif = NULL;
	struct pim_neighbor_node *pnbr_node = NULL;
	struct pim_up_jp_grp_node *pup_wc = NULL;
	
	if(wc_mrt == NULL || wc_mrt->upstream_ifindex == 0 || wc_mrt->rpf_nbr == 0 || wc_mrt->rp_addr == 0)
	{
		IPMC_LOG_ERROR("wc_mrt NULL\n");
		return NULL;
	}
	else if(wc_mrt->upstream_ifindex == 0 || wc_mrt->rpf_nbr == 0 || wc_mrt->rp_addr == 0)
	{
		IPMC_LOG_ERROR("(*, %x) upstream_ifindex:%u, rpf_nbr:%u, rp:%u\n", wc_mrt->grp_addr,
			wc_mrt->upstream_ifindex, wc_mrt->rpf_nbr, wc_mrt->rp_addr);
		return NULL;
	}
	/*根据upstream_ifindex查找ipmc_if*/
	pif = ipmc_if_lookup(wc_mrt->upstream_ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex 0x%x not exist\n", wc_mrt->upstream_ifindex);
		return NULL;
	}
	/*根据rpf_nbr查找邻居节点*/
	pnbr_node = pim_nbr_node_lookup(pif->pim_nbr, wc_mrt->rpf_nbr);
	if(pnbr_node == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex %x, nbr_node %x not exist\n", pif->ifindex, wc_mrt->rpf_nbr);
		return NULL;
	}
	/*获取(*,G)上游节点*/
	pup_wc = pim_jp_up_send_wc_node_get(pnbr_node, wc_mrt, create_flag);
	if(pup_wc == NULL)
	{
		IPMC_LOG_ERROR("pim_jp_up_send_wc_node_get(%x, %x) return NULL\n", pnbr_node->nbr_addr, wc_mrt->grp_addr);
		return NULL;
	}
	return pup_wc;
	/*设置这个节点的发送周期为60s*/

}

uint32_t pim_jp_up_send_wc_jp_node_del(struct pim_mrt_wc *wc_mrt)
{
	struct ipmc_if *pif = NULL;
	struct pim_neighbor_node *pnbr_node = NULL;
	
	if(wc_mrt == NULL || wc_mrt->upstream_ifindex == 0 || wc_mrt->rpf_nbr == 0 || wc_mrt->rp_addr == 0)
	{
		IPMC_LOG_ERROR("wc_mrt NULL\n");
		return ERRNO_FAIL;
	}
	else if(wc_mrt->upstream_ifindex == 0 || wc_mrt->rpf_nbr == 0 || wc_mrt->rp_addr == 0)
	{
		IPMC_LOG_ERROR("(*, %x) upstream_ifindex:%u, rpf_nbr:%u, rp:%u\n", wc_mrt->grp_addr,
			wc_mrt->upstream_ifindex, wc_mrt->rpf_nbr, wc_mrt->rp_addr);
		return ERRNO_FAIL;
	}
	/*根据upstream_ifindex查找ipmc_if*/
	pif = ipmc_if_lookup(wc_mrt->upstream_ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex 0x%x not exist\n", wc_mrt->upstream_ifindex);
		return ERRNO_FAIL;
	}
	/*根据rpf_nbr查找邻居节点*/
	pnbr_node = pim_nbr_node_lookup(pif->pim_nbr, wc_mrt->rpf_nbr);
	if(pnbr_node == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex %x, nbr_node %x not exist\n", pif->ifindex, wc_mrt->rpf_nbr);
		return ERRNO_FAIL;
	}
	/*删除这个节点*/
	pim_jp_up_send_wc_node_del(pnbr_node, wc_mrt->grp_addr);
	
	return ERRNO_SUCCESS;
}

uint32_t pim_jp_up_sg_jp_send(uint32_t ifindex, uint32_t upstream, uint32_t grp_addr, uint32_t src_addr, uint8_t type)
{
	struct pim_hdr *pimhdr = NULL;
	uint8_t *pim_para = NULL;
	struct pim_join_prune_hdr jp_hdr;
	struct pim_join_prune_grp jp_grp;
	struct encode_ipv4_source jp_src;
	
	union pkt_control pkt_ctrl;
	uint32_t len = 0, para_len = 0;;
	sint32 ret = ERRNO_FAIL;
	
	memset(&jp_hdr, 0, sizeof(struct pim_join_prune_hdr));
	memset(&jp_grp, 0, sizeof(struct pim_join_prune_grp));
	memset(&jp_src, 0, sizeof(struct encode_ipv4_source));
	/* encap pim parameter*/
	pimhdr = (struct pim_hdr *)ipmc_send_buf;
	memset(pimhdr, 0, sizeof(struct pim_hdr));
	
	pim_para = (uint8_t *)ipmc_send_buf + sizeof(struct pim_hdr);
	
	/*put jp_hdr 报文中是10字节，但是结构体占12字节，upstream占8字节*/
	jp_hdr.upstream.addr_family = 1;
	jp_hdr.upstream.encode_type = 0;
	jp_hdr.upstream.unicast_addr = htonl(upstream);
	jp_hdr.reserved = 0;
	jp_hdr.group_num = 1;
	jp_hdr.holdtime = htons(PIM_JP_HOLDTIME);/*default210, jp_interval *3.5*/
	//memcpy(pim_para, &jp_hdr, sizeof(struct pim_join_prune_hdr));
	//pim_para += sizeof(struct pim_join_prune_hdr);
	//para_len += sizeof(struct pim_join_prune_hdr);

	ipmc_setc(&pim_para, jp_hdr.upstream.addr_family);
	ipmc_setc(&pim_para, jp_hdr.upstream.encode_type);
	ipmc_setl(&pim_para, jp_hdr.upstream.unicast_addr);
	ipmc_setc(&pim_para, jp_hdr.reserved);
	ipmc_setc(&pim_para, jp_hdr.group_num);
	ipmc_setw(&pim_para, jp_hdr.holdtime);
	para_len += 10;
	
	/*put jp_grp*/
	jp_grp.grp.addr_family = 1;
	jp_grp.grp.encode_type = 0;
	jp_grp.grp.reserved = 0;
	jp_grp.grp.mask_len = 32;
	jp_grp.grp.grp_addr = htonl(grp_addr);
	if(type == PIM_JP_UP_SEND_SG_JOIN || type == PIM_JP_UP_SEND_SG_RPT_JOIN)
	{
		jp_grp.join_num = 1;
		jp_grp.prune_num = 0;
	}
	else if(type == PIM_JP_UP_SEND_SG_PRUNE || type == PIM_JP_UP_SEND_SG_RPT_PRUNE)
	{
		jp_grp.join_num = 0;
		jp_grp.prune_num = 1;
	}
	memcpy(pim_para, &jp_grp, sizeof(struct pim_join_prune_grp));
	pim_para += sizeof(struct pim_join_prune_grp);
	para_len += sizeof(struct pim_join_prune_grp);
	/*srclist*/
	jp_src.addr_family = 1;
	jp_src.encode_type = 0;
	jp_src.flag |= (PIM_JP_S_BIT);
	if(type == PIM_JP_UP_SEND_SG_RPT_JOIN || type == PIM_JP_UP_SEND_SG_RPT_PRUNE)
	{
		jp_src.flag |= (PIM_JP_R_BIT);
	}
	jp_src.mask_len = 32;
	jp_src.src_addr = htonl(src_addr);
	memcpy(pim_para, &jp_src, sizeof(struct encode_ipv4_source));
	pim_para += sizeof(struct encode_ipv4_source);
	para_len += sizeof(struct encode_ipv4_source);
	
	/*pim para length*/
	len = (uint8_t *)pim_para - (uint8_t *)ipmc_send_buf;
	
	/*encap pim hdr*/
	pimhdr->pim_vers = PIM_VERSION;
	pimhdr->pim_types = PIM_JOIN_PRUNE;
	pimhdr->pim_cksum = in_checksum((uint16_t*)pimhdr, len);
	
	/*ip hdr */
	memset(&pkt_ctrl, 0, sizeof(union pkt_control));
	pkt_ctrl.ipcb.chsum_enable = ENABLE;
	pkt_ctrl.ipcb.protocol = IP_P_PIM;
	pkt_ctrl.ipcb.ifindex = ifindex;
	pkt_ctrl.ipcb.sip = ipmc_if_main_addr_get(ifindex);
	pkt_ctrl.ipcb.dip = ALLPIM_ROUTER_GROUP;
	pkt_ctrl.ipcb.pkt_type = PKT_TYPE_IPMC;
	pkt_ctrl.ipcb.if_type = PKT_INIF_TYPE_IF;
	pkt_ctrl.ipcb.tos = 4;	/*hello msg not contain ip priority*/
	pkt_ctrl.ipcb.ttl = 1;
	pkt_ctrl.ipcb.is_changed = 1;

	ret = pkt_send(PKT_TYPE_IPMC, &pkt_ctrl, (void *)ipmc_send_buf, len);
	if(ret)
	{
		IPMC_LOG_ERROR("pkt_send fail!\n");
	}
	memset(ipmc_send_buf, 0, IPMC_SEND_BUF_SIZE);
	return ret;
}

struct pim_up_jp_sg_node *pim_jp_up_send_sg_node_get(struct pim_neighbor_node *pnbr_node, struct pim_mrt_sg *sg_mrt, uint32_t type, uint8_t create_flag)
{
	struct pim_up_jp_sg_node *pup_sg = NULL;
	struct pim_up_jp_sg_node up_sg = {0};
	struct hash_bucket *pitem = NULL;

	up_sg.grp_addr = sg_mrt->grp_addr;
	up_sg.src_addr = sg_mrt->src_addr;
	up_sg.type = type;
	pitem = hios_hash_find(&(pnbr_node->up_sg_jp), (void *)(&up_sg));
    if(pitem)
    {
		pup_sg = (struct pim_up_jp_sg_node *)pitem->data;
		if(create_flag == 0)
		{
			return pup_sg;
		}
    }
	else
	{
		if(create_flag == 0)
		{
			return pup_sg;
		}
		/*创建grp_node*/
		pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
		if (NULL == pitem)
		{
			IPMC_LOG_ERROR("grp %u malloc (struct hash_bucket) failed!\n", sg_mrt->grp_addr);
			return NULL;
		}
		
		pup_sg = (struct pim_up_jp_sg_node *)XCALLOC(MTYPE_IPMC_PIM_JP_UP_SG_ENTRY, sizeof(struct pim_up_jp_sg_node));
		if (NULL == pup_sg)
		{
			XFREE(MTYPE_HASH_BACKET, pitem);
			IPMC_LOG_ERROR("(%x, %x)  malloc(struct pim_up_jp_sg_node) failed!\n", sg_mrt->src_addr, sg_mrt->grp_addr);
			return NULL;
		}
		memset(pup_sg, 0, sizeof(struct pim_up_jp_sg_node));
		pup_sg->grp_addr = sg_mrt->grp_addr;
		pup_sg->src_addr = sg_mrt->src_addr;
		pup_sg->type = type;/*PIM_JP_SG_TYPE / PIM_JP_SGRPT_TYPE*/
		pup_sg->to_sg = sg_mrt;
		
		
		pitem->hash_key = (void *)(pup_sg);
		pitem->data = (void *)pup_sg;
		
		hios_hash_add(&(pnbr_node->up_sg_jp), pitem);
	}
	
	pup_sg->used_rp = sg_mrt->rp_addr;
	pup_sg->used_rpf = sg_mrt->rpf_nbr;
	pup_sg->ifindex = sg_mrt->upstream_ifindex;
	if(pup_sg->type == PIM_JP_SG_TYPE)
	{
		pup_sg->state = sg_mrt->jp_up_state;
	}
	else if(pup_sg->type == PIM_JP_SG_RPT_TYPE)
	{
		pup_sg->state = sg_mrt->jp_up_rptstate;
	}

	return pup_sg;
}

uint32_t pim_jp_up_send_sg_node_del(struct pim_mrt_sg *sg_mrt, struct pim_up_jp_sg_node *pup_sg)
{
	struct hash_bucket *pitem = NULL;
	struct ipmc_if *pif = NULL;
	struct pim_neighbor_node *pnbr_node = NULL;
	
	/*根据upstream_ifindex查找ipmc_if*/
	printf("pim_jp_up_send_sg_node_del\n");
	pif = ipmc_if_lookup(sg_mrt->upstream_ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex 0x%x not exist\n", sg_mrt->upstream_ifindex);
		return ERRNO_FAIL;
	}
	/*根据rpf_nbr查找邻居节点*/
	pnbr_node = pim_nbr_node_lookup(pif->pim_nbr, sg_mrt->rpf_nbr);
	if(pnbr_node == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex %x, nbr_node %x not exist\n", pif->ifindex, sg_mrt->rpf_nbr);
		return ERRNO_FAIL;
	}

	pitem = hios_hash_find(&(pnbr_node->up_sg_jp), (void *)(&pup_sg));
    if(pitem)
    {
		pup_sg = (struct pim_up_jp_sg_node *)pitem->data;
		
		/*del from up_sg_jp*/
		hios_hash_delete(&(pnbr_node->up_sg_jp), pitem);
		
		/*del up_sg*/
		XFREE(MTYPE_IPMC_PIM_JP_UP_SG_ENTRY, pup_sg);
		pitem->prev = NULL;
		pitem->next = NULL;
		pitem->data = NULL;
		XFREE(MTYPE_HASH_BACKET, pitem);
    }
	else
	{
		IPMC_LOG_ERROR("Upstream ifindex %x, sg(%x, %x) not exist\n", pif->ifindex, pup_sg->src_addr, pup_sg->grp_addr);
		return ERRNO_FAIL;
	}

	return ERRNO_SUCCESS;
}

struct pim_up_jp_sg_node* pim_jp_up_send_sg_jp_upstream(struct pim_mrt_sg *sg_mrt, uint32_t type, uint32_t create_flag)
{
	struct ipmc_if *pif = NULL;
	struct pim_neighbor_node *pnbr_node = NULL;
	struct pim_up_jp_sg_node *pup_sg = NULL;
	
	if(sg_mrt == NULL)
	{
		IPMC_LOG_ERROR("sg_mrt NULL\n");
		return NULL;
	}
	else if(sg_mrt->upstream_ifindex == 0 || sg_mrt->rpf_nbr == 0 || sg_mrt->src_addr == 0)
	{
		IPMC_LOG_ERROR("(%x, %x) upstream_ifindex:%u, rpf_nbr:%u, rp:%u\n", sg_mrt->src_addr, sg_mrt->grp_addr,
			sg_mrt->upstream_ifindex, sg_mrt->rpf_nbr, sg_mrt->rp_addr);
		return NULL;
	}
	/*根据upstream_ifindex查找ipmc_if*/
	pif = ipmc_if_lookup(sg_mrt->upstream_ifindex);
	if(pif == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex 0x%x not exist\n", sg_mrt->upstream_ifindex);
		return NULL;
	}
	/*根据rpf_nbr查找邻居节点*/
	pnbr_node = pim_nbr_node_lookup(pif->pim_nbr, sg_mrt->rpf_nbr);
	if(pnbr_node == NULL)
	{
		IPMC_LOG_ERROR("ipmc_if ifindex %x, nbr_node %x not exist\n", pif->ifindex, sg_mrt->rpf_nbr);
		return NULL;
	}
	/*获取(S,G)上游节点*/
	pup_sg = pim_jp_up_send_sg_node_get(pnbr_node, sg_mrt, type, create_flag);
	if(pup_sg == NULL)
	{
		IPMC_LOG_ERROR("pim_jp_up_send_sg_node_get(%x, %x) return NULL\n", pnbr_node->nbr_addr, sg_mrt->grp_addr);
		return NULL;
	}
	return pup_sg;
}

