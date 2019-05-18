/**
 * @file      : cfm.c
 * @brief     : define of 802.1ag and Y.1731 md and ma
 * @details   : 
 * @author    : huoqq
 * @date      : 2018年3月23日 14:32:48
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : 
 */

#include <string.h>
#include <stdlib.h>
#include <lib/zassert.h>
#include <lib/hash1.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/thread.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include "lib/msg_ipc.h"
#include "lib/log.h"
#include "lib/pkt_buffer.h"
#include "lib/pkt_type.h"
#include "lib/devm_com.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"


#include "l2/l2_if.h"

#include "cfm_session.h"
#include "cfm.h"
#include "cfm_lb.h"
#include "cfm_lt.h"
#include "cfm_test.h"
#include "cfm_cli.h"

#include "../vlan.h"
#include "../aps/elps_pkt.h"
#include "raps/erps_pkt.h"
unsigned int         cfm_device_id;                //2200:0x05F0

struct cfm_md 		*cfm_md_table[CFM_MD_NUM_MAX]; /* MD 数组 */
struct hash_table 	 cfm_ma_table;                 /* CFM ma hash 表，以 ma 索引为 hash key */

/**
 * @brief      : md 数组初始化
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void 	cfm_md_table_init(void)
{
	memset(cfm_md_table,0,CFM_MD_NUM_MAX*sizeof(struct cfm_md *));
}

/**
 * @brief      : 创建md 
 * @param[in ] : md_index, md 索引
 * @param[out] : 
 * @return     : 成功返回新创建的md指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_md *cfm_md_create(uint8_t md_index) 
{
	struct cfm_md * md_item;
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s', md_index:%d",__FILE__,__func__,md_index);

	if(md_index<1 || md_index>CFM_MD_NUM_MAX)
		return NULL;
		
	md_item = (struct cfm_md *)XMALLOC(MTYPE_CFM_MD, sizeof(struct cfm_md));
	if(NULL == md_item)
	{
		return NULL;
	}
	memset(md_item,0,sizeof(struct cfm_md));

	md_item->md_index = md_index;
	md_item->level = 0;
		
	cfm_md_table[md_index-1] = md_item;
	
	zlog_debug(CFM_DBG_COMMN, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return md_item;

}

/**
 * @brief      : 删除md 
 * @param[in ] : md_index, md 索引
 * @param[out] : 
 * @return     : 0
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_md_delete(uint8_t md_index)
{
	struct cfm_md * pmd;
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s', md_index:%d",__FILE__,__func__,md_index);

	pmd = cfm_md_table[md_index-1];
	
	cfm_md_clear_ma(pmd);
	
	XFREE(MTYPE_CFM_MD, pmd);

	cfm_md_table[md_index-1] = NULL;
	
	zlog_debug(CFM_DBG_COMMN, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return 0;

}

/**
 * @brief      : 查找md 
 * @param[in ] : md_index, md 索引
 * @param[out] : 
 * @return     : 成功返回md指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_md *cfm_md_lookup(uint8_t md_index)
{
	if(md_index>=1 && md_index<=CFM_MD_NUM_MAX)
		return cfm_md_table[md_index-1];

	return NULL;
}

/**
 * @brief      : 查找md 
 * @param[in ] : level, md level
 * @param[out] : 
 * @return     : 成功返回md指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_md *cfm_md_lookup_by_level(uint8_t level) 
{
	uint8_t i;
		
	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL && cfm_md_table[i]->level == level)
		{				
			return cfm_md_table[i];			
		}
	}
	return NULL;
}

/**
 * @brief      : 将ma添加到md中的ma链表
 * @param[in ] : pmd, md指针 
 * @param[in ] : pma, ma指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_md_add_ma(struct cfm_md *pmd, struct cfm_ma *pma)
{
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if (NULL==pmd || NULL == pma)	//预防操作空指针
	{
		return ERRNO_FAIL;
	}
	


	listnode_add(&pmd->ma_list,  pma);
	pma->md_index = pmd->md_index;
	
	cfm_ma_add(pma);//add ma in malist first

	zlog_debug(CFM_DBG_COMMN, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 将ma从md中的ma链表删除
 * @param[in ] : pmd, md指针 
 * @param[in ] : pma, ma指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_md_delete_ma(struct cfm_md *pmd, struct cfm_ma *pma)
{
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	
	if (NULL==pmd || NULL == pma)					
	{
		return ERRNO_FAIL;
	}

#if 0
	ipc_send_hal( pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
#endif

	if (pma->name && pma->vlan)
	{
		cfm_send_hal(pma, sizeof(struct cfm_ma), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_CFM, OAM_SUBTYPE_MA, IPC_OPCODE_DELETE, pma->ma_index);
	}
	
	cfm_ma_delete(pma->ma_index);//del ma in malist first

	
	listnode_delete(&pmd->ma_list,pma);
	
	//XFREE(MTYPE_IF,pma);		
	
	zlog_debug(CFM_DBG_COMMN, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 将md中的ma链表清空
 * @param[in ] : pmd, md指针 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void 	cfm_md_clear_ma(struct cfm_md *pmd)
{
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	struct cfm_ma *pma = NULL;
	struct listnode *node;

	
	if (NULL == pmd)					
	{
		return ;
	}
	
    for (node = pmd->ma_list.head ; node; node = node->next)
    {
		pma=listgetdata(node);

		if(pma)
		{
			cfm_ma_delete(pma->ma_index);//del ma in malist first
		
			//XFREE(MTYPE_CFM_MA,pma);				
		}
	}
	
	list_delete_all_node(&pmd->ma_list);
		
	zlog_debug(CFM_DBG_COMMN, "%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return ;

}



//------------------------------------ma-----------------------------------------

/**
 * @brief      : ma哈希表key值计算
 * @param[in ] : hash_key, ma key值
 * @param[out] : 
 * @return     : 哈希key
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static unsigned int compute_ma(void *hash_key)
{
    if ( NULL == hash_key )
    {
        return 0;
    }
    return ( ( unsigned int ) hash_key );
}

/**
 * @brief      : ma哈希表key值比较
 * @param[in ] : item, 哈希节点 
 * @param[in ] : hash_key, ma key值
 * @param[out] : 
 * @return     : 比较相等返回0，不相等返回1
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int compare_ma(void *item, void *hash_key)
{    
    struct hash_bucket *pbucket = item;

    if ( NULL == pbucket || NULL == hash_key )
    {
        return 1;
    }

    if ( pbucket->hash_key == hash_key )
    {
        return 0;
    }
    else
    { 
    	return 1; 
	}
}

/**
 * @brief      : ma哈希表初始化
 * @param[in ] : uiSize, 哈希节点最大个数 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void 	cfm_ma_table_init(unsigned int uiSize)
{
    if (0 == uiSize)
    {
        zlog_err("size must great than or equal to zero!\n");
        return ;
    }   
    hios_hash_init(&cfm_ma_table, uiSize, compute_ma, compare_ma);
    return ;

}

/**
 * @brief      : 创建一个 Ma 
 * @param[in ] : ma_index, ma索引 
 * @param[out] : 
 * @return     : 成功返回新创建的ma指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_ma *cfm_ma_create(uint16_t ma_index)
{
	struct cfm_ma * pma;
	
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	pma = (struct cfm_ma *)XMALLOC(MTYPE_CFM_MA, sizeof(struct cfm_ma));
	if(NULL == pma)
	{
		return NULL;
	}
	memset(pma,0,sizeof(struct cfm_ma));
	
	pma->ma_index = ma_index;
	pma->priority = 5;

	
	return pma;
}

/**
 * @brief      : 添加 Ma 到 hash 表
 * @param[in ] : pma, ma指针 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_ma_add(struct cfm_ma *pma)
{
	struct hash_bucket *ma_item = NULL;
	int ret;

	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	
	//new
	ma_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == ma_item)
	{
		return 1;
	}
	
	memset(ma_item, 0, sizeof(struct hash_bucket));
	
	ma_item->data = pma;
	ma_item->hash_key = (void *)(uint32_t)pma->ma_index;
	
	ret = hios_hash_add(&cfm_ma_table, ma_item);
	if(ret != 0)
	{
		XFREE(MTYPE_HASH_BACKET, ma_item);
		return ERRNO_FAIL;
	}
	
	return ERRNO_SUCCESS;

}

/**
 * @brief      : 删除一个 Ma
 * @param[in ] : ma_index, ma索引 
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_ma_delete(uint16_t ma_index)
{
	struct hash_bucket 		*ma_item = NULL;
	struct cfm_ma			*ma_node = NULL;

	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s', ma_index:%d",__FILE__,__func__,ma_index);


	ma_item = hios_hash_find(&cfm_ma_table, (void *)(uint32_t)ma_index);


	if(ma_item)
	{
		hios_hash_delete(&cfm_ma_table, ma_item);

		ma_node = ma_item->data;


		cfm_ma_clear_sess(ma_node); //del session first

		
		XFREE(MTYPE_CFM_MA, ma_node);
		XFREE(MTYPE_HASH_BACKET, ma_item);
		
	}

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 查找一个 Ma
 * @param[in ] : ma_index, ma索引 
 * @param[out] : 
 * @return     : 成功返回查找到的ma指针，失败返回NULL
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

struct cfm_ma *cfm_ma_lookup(uint16_t ma_index) 
{
	struct hash_bucket 		*ma_item = NULL;
	struct cfm_ma			*ma_node = NULL;

	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if(ma_index<1 || ma_index>CFM_MA_NUM_MAX)
		return NULL;

	
	ma_item = hios_hash_find(&cfm_ma_table, (void *)(uint32_t)ma_index);


	if(ma_item)
	{

		ma_node = ma_item->data;

		return ma_node;		
	}	

	return NULL;
}

//--------------------------ma add session------------------------------------------------

/**
 * @brief      : 将一个session添加到ma的session链表
 * @param[in ] : pma, ma指针 
 * @param[in ] : psess, session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_ma_add_sess(struct cfm_ma *pma, struct cfm_sess *psess)
{
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	if (NULL==pma || NULL == psess)	//预防操作空指针
	{
		return ERRNO_FAIL;
	}

	listnode_add(&pma->sess_list,  psess);	

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 将一个session从ma的session链表删除
 * @param[in ] : pma, ma指针 
 * @param[in ] : psess, session指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

int 	cfm_ma_delete_sess(struct cfm_ma *pma, struct cfm_sess *psess)
{
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);

	
	if (NULL == psess)					
	{
		zlog_debug(CFM_DBG_COMMN,"%s:leave the function of '%s' psess is NULL",__FILE__,__func__);
	
		return ERRNO_FAIL;
	}

	//cfm_session_delete(psess->local_mep);//del sess  in sesslist first

	if(NULL == pma)
	{
		zlog_debug(CFM_DBG_COMMN,"%s:leave the function of '%s',pma is NULL",__FILE__,__func__);
	
		return ERRNO_FAIL;
	}
	
	listnode_delete(&pma->sess_list,psess);

	
	//XFREE(MTYPE_IF,pma);		
	
	zlog_debug(CFM_DBG_COMMN,"%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return ERRNO_SUCCESS;

}

/**
 * @brief      : 将ma的session链表清空
 * @param[in ] : pma, ma指针 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void 	cfm_ma_clear_sess(struct cfm_ma *pma)  /* 删除 ma 下的所有 session */
{
	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'--the line of %d",__FILE__,__func__,__LINE__);
	struct cfm_sess *psess = NULL;
	struct listnode *node;

	
	if (NULL == pma)					
	{
		return ;
	}
	
    for (node = pma->sess_list.head ; node; node = node->next)
    {
		psess=listgetdata(node);

		if(psess)
		{
			cfm_session_delete_new(psess->sess_id);//del ma in malist first		
		}
	}
	
	list_delete_all_node(&pma->sess_list);
		
	zlog_debug(CFM_DBG_COMMN,"%s:leave the function of '%s',--the line of %d",__FILE__,__func__,__LINE__);

	return ;

}

/**
 * @brief      : 接收报文类型注册
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static void cfm_pkt_register()
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

	proto.ethreg.dmac_valid = 0;	
	proto.ethreg.ethtype = 0x8902;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);	

	return;
}

/**
 * @brief      : cfm模块初始化
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void cfm_init()
{
	cfm_md_table_init();
	cfm_ma_table_init(CFM_MA_NUM_MAX);
	cfm_session_table_init(CFM_MEP_NUM_MAX);

	cfm_pkt_register();

	cfm_cmd_init();

	if(devm_comm_get_id(1,0,MODULE_ID_L2,&cfm_device_id))
	{
		printf("cfm get dev id error!\n");
		cfm_device_id = ID_HT2200;
	}
	
	printf("cfm get dev id :0X%02X\n",cfm_device_id);

	make_crc32_table();
}

/**
 * @brief      : 根据level+vlan查找ma，用于报文接收
 * @param[in ] : level，md的level
 * @param[in ] : vlan， ma的vlan
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

/*static struct cfm_ma *cfm_find_ma(uint8_t level, uint16_t vlan)
{
	uint8_t i;
	struct listnode *p_listnode = NULL;
	struct cfm_ma *pma = NULL;
	
	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL && cfm_md_table[i]->level == level)
		{				
			for(ALL_LIST_ELEMENTS_RO(&cfm_md_table[i]->ma_list,p_listnode,pma))
			{
				if(pma->vlan == vlan)
				{
					return pma;
				}
			}
		}
	}
	
	return NULL;
}*/

/**
 * @brief      : mip处理lbm报文
 * @param[in ] : pma，ma的指针
 * @param[in ] : pkt， 报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_mip_rev_lbm(struct cfm_ma *pma,struct pkt_buffer *pkt)
{
	uint8_t			 pmac[6] = {0};

	struct raw_control rawcb;
	struct cfm_lb_pdu  lb_pdu;
	int ret = 0;
	char name[50];
	uint8_t flag1=0,flag2=0;

	struct listnode *p_listnode = NULL;
	void *data = NULL;
	uint8_t p_mode = 0;	

	uint8_t if_mac[6];
	uint8_t if_mode;
	
	uint8_t dmac_multi[6]={0x01,0x80,0xc2,0x00,0x00,0x30};
	struct cfm_md *pmd = NULL;

    int type;

	uint32_t parent_if;

	char name1[50];
	char name2[50];

	zlog_debug(CFM_DBG_COMMN,"%s:Entering the function of '%s'",__FILE__,__func__);

	if(pma == NULL || pkt == NULL)
		return ERRNO_FAIL;

	pmd = cfm_md_lookup(pma->md_index);
	if(!pmd)		
		return ERRNO_FAIL;
	
	dmac_multi[5] += pmd->level;
	
	memcpy(pmac, pkt->cb.ethcb.dmac, 6);
    
	zlog_debug(CFM_DBG_COMMN,"cfm_mip_rev_lbm(),dmac is %02X:%02X:%02X:%02X:%02X:%02X\n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);

	if(ifm_get_mac(pkt->in_ifindex, MODULE_ID_L2, pmac) == 0)	
	{
		if(pkt->cb.ethcb.dmac[0]&0x01)
		{
			if(!memcmp(pkt->cb.ethcb.dmac,dmac_multi,6))
			{
				flag1 = 1;
				flag2 = 1;
			}
			else
			{				
				zlog_debug(CFM_DBG_COMMN, "cfm_mip_rev_lbm(),dmac is multicast but not map\n");
				return ERRNO_FAIL;
			}
		}
		else
		{
			if(!memcmp(pkt->cb.ethcb.dmac,pmac,6))
			{
				flag1 = 1;
				flag2 = 0;
			}
			else
			{
				flag1 = 0;
				flag2 = 1;
			}
		}
		
	}
	else
	{	
		zlog_err("%s: %s,get the interface mac timeout",__FILE__,__func__);		
		return ERRNO_FAIL;
	}

	memcpy(if_mac,pmac,6);	
	zlog_debug(CFM_DBG_COMMN, "get interface mac :%02X:%02X:%02X:%02X:%02X:%02X\n",if_mac[0],if_mac[1],if_mac[2],if_mac[3],if_mac[4],if_mac[5]);

	
	if(ifm_get_mode(pkt->in_ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
		return ERRNO_FAIL;
	}	
    
	if_mode = p_mode;


	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_COMMN, "receive cfm lbm from interface:%s, vpn:%d \n",name,pkt->vpn);

	parent_if = IFM_PARENT_IFINDEX_GET(pkt->in_ifindex);
	ifm_get_name_by_ifindex(parent_if,name);
	zlog_debug(CFM_DBG_LB, "receive cfm lbm from parent interface:%s\n",name);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_COMMN, "physical port is:%s\n",name);

	memset (&lb_pdu, 0, sizeof(struct cfm_lb_pdu));
	memset (&rawcb, 0, sizeof(struct raw_control));

	memcpy (&lb_pdu.cfm_header, pkt->data,pkt->data_len);

	lb_pdu.ethhdr.h_tpid      = htons(0x8100);
	lb_pdu.ethhdr.h_vlan_tci  = htons((pma->priority << 13) | pma->vlan);
	lb_pdu.ethhdr.h_ethtype   = htons(0x8902);


	rawcb.ethtype    = 0x8902;
	rawcb.priority   = pma->priority;
	rawcb.ttl        = 255;

	type = IFM_TYPE_ID_GET ( pkt->in_ifindex );

	zlog_debug(CFM_DBG_LB,"receive cfm lbm action: response(%s), forward(%s) \n", 
		flag1 ? "yes" : "no", flag2 ? "yes" : "no");
	
	if(flag1)//response
	{	
		memcpy(&lb_pdu.ethhdr.h_dest, pkt->cb.ethcb.smac, 6);
		memcpy(&lb_pdu.ethhdr.h_source, if_mac, 6);		
		lb_pdu.cfm_header.opcode = CFM_LOOPBACK_REPLY_OPCODE;
		
		if (pkt->in_ifindex != pkt->in_port && type!=IFNET_TYPE_TRUNK)
		{
			/*bug:56330  modify 2018.9.20*/
			if (parent_if == pkt->in_port)
			{
				rawcb.in_ifindex = 0;		
				rawcb.out_ifindex = pkt->in_ifindex;
			}
			else
			{
				/* Only when for pw, up mep is exist */
				rawcb.out_ifindex = 0;			
				rawcb.in_ifindex = pkt->in_ifindex;
			}
		}
		else 
		{
			rawcb.in_ifindex = 0;		
			rawcb.out_ifindex = pkt->in_ifindex;
		}

		ifm_get_name_by_ifindex(rawcb.in_ifindex,name1);
		ifm_get_name_by_ifindex(rawcb.out_ifindex,name2);

		zlog_debug(CFM_DBG_LB,"cfm lbm response:in_ifindex:%s out_ifindex:%s,lbm from interface type:%d mode:%d\n", 
					 name1, name2, type, if_mode);
		
		ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lb_pdu, pkt->data_len+sizeof(struct vlan_ethhdr));		
		
	}

	if(flag2)//forward
	{	
		memcpy(&lb_pdu.ethhdr.h_dest, pkt->cb.ethcb.dmac, 6);
		memcpy(&lb_pdu.ethhdr.h_source, pkt->cb.ethcb.smac, 6);
		lb_pdu.cfm_header.opcode = CFM_LOOPBACK_MESSAGE_OPCODE;
		
		if (pkt->in_ifindex != pkt->in_port && type!=IFNET_TYPE_TRUNK)//pkt from nni
		{
			/*bug:56330  modify 2018.9.20*/
			if (parent_if == pkt->in_port)
			{
				rawcb.out_ifindex = 0;			
				rawcb.in_ifindex = pkt->in_ifindex;
			}
			else
			{
				//for pw
				rawcb.in_ifindex = 0;			
				rawcb.out_ifindex = pkt->in_ifindex;
			}
		}
		else//pkt from uni or normal mode
		{
			if(if_mode != IFNET_MODE_L2)
			{
				//for switch
				for(ALL_LIST_ELEMENTS_RO(&(vlan_table[pma->vlan]->portlist),p_listnode,data))
				{
					if(((uint32_t)data) != pkt->in_ifindex)
					{	
						rawcb.in_ifindex = 0;					
						rawcb.out_ifindex = (uint32_t)data;
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lb_pdu, pkt->data_len+sizeof(struct vlan_ethhdr));		
					}
				}
				return ERRNO_SUCCESS;
			}
			else
			{
				//for pw, up mep is exist */	
				rawcb.out_ifindex = 0;				
				rawcb.in_ifindex = pkt->in_ifindex;
			}
		}

		ifm_get_name_by_ifindex(rawcb.in_ifindex,name1);
		ifm_get_name_by_ifindex(rawcb.out_ifindex,name2);

		zlog_debug(CFM_DBG_LB, "cfm lbm forward:in_ifindex:%s out_ifindex:%s,lbm from interface type:%d mode:%d\n", 
					name1, name2, type, if_mode);
		
		ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &lb_pdu, pkt->data_len+sizeof(struct vlan_ethhdr));		
		
	}
	

	return ret;

	
}

/**
 * @brief      : mip处理ltm报文
 * @param[in ] : pma，ma的指针
 * @param[in ] : pkt， 报文指针
 * @param[out] : 
 * @return     : 错误码
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_mip_rev_ltm(struct cfm_ma *pma,struct pkt_buffer *pkt)
{
	uint8_t			   pmac[6] = {0};

	struct raw_control rawcb;
	struct cfm_ltm_pdu  ltm_pdu;
	struct cfm_ltr_pdu  ltr_pdu;
	
	int ret = 0;
	char name[50];
	uint8_t flag=0;

	struct listnode *p_listnode = NULL;
	void *data = NULL;
	unsigned char *p = ltr_pdu.tlvs;
	unsigned char len = 0;
	uint8_t p_mode = 0;	

	uint8_t if_mac[6];
	uint8_t if_mode;

    int type;

	uint32_t parent_if;

	if(pma == NULL || pkt == NULL)
		return ERRNO_FAIL;
	
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'",__FILE__,__func__);

	memset(&ltm_pdu,0,sizeof(struct cfm_ltm_pdu));
	memset(&ltr_pdu,0,sizeof(struct cfm_ltr_pdu));
	memset (&rawcb, 0, sizeof(struct raw_control));
	
	//memcpy (&ltm_pdu.cfm_header, pkt->data, sizeof(struct cfm_ltm_pdu)-sizeof(struct vlan_ethhdr));
	memcpy (&ltm_pdu.cfm_header, pkt->data, pkt->data_len);
	memcpy(pmac, ltm_pdu.target_mac, 6);

	zlog_debug(CFM_DBG_COMMN, "cfm_mip_rev_ltm(),target_mac is %02X:%02X:%02X:%02X:%02X:%02X\n",pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	
	
	if(ifm_get_mac(pkt->in_ifindex, MODULE_ID_L2, pmac) == 0)
	{
		if(!memcmp(ltm_pdu.target_mac,pmac,6))
			flag = 1;
	}
	else
	{	
		zlog_err("%s: %s,get the interface mac timeout",__FILE__,__func__);		
		return ERRNO_FAIL;
	}

	memcpy(if_mac,pmac,6);	
	zlog_debug(CFM_DBG_COMMN, "get interface mac :%02X:%02X:%02X:%02X:%02X:%02X\n",if_mac[0],if_mac[1],if_mac[2],if_mac[3],if_mac[4],if_mac[5]);


	
	if(ifm_get_mode(pkt->in_ifindex,MODULE_ID_L2, &p_mode) != 0)
	{
		zlog_err("%s: %s,get the interface mode timeout",__FILE__,__func__);		
		return ERRNO_FAIL;
	}	

	if_mode = p_mode;

	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'",__FILE__,__func__);

	ifm_get_name_by_ifindex(pkt->in_port,name);
	zlog_debug(CFM_DBG_COMMN, "physical port is:%s\n",name);

	ifm_get_name_by_ifindex(pkt->in_ifindex,name);
	zlog_debug(CFM_DBG_COMMN, "receive cfm lbm from interface:%s, vpn:%d \n",name,pkt->vpn);

	parent_if = IFM_PARENT_IFINDEX_GET(pkt->in_ifindex);
	ifm_get_name_by_ifindex(parent_if,name);
	zlog_debug(CFM_DBG_COMMN, "receive cfm ltm from parent interface:%s\n",name);

	
	/* copy cfm_header & trans_id & ttl */
	/*memcpy (&ltr_pdu.cfm_header, pkt->data, 
						sizeof(struct cfm_ltr_pdu) - sizeof(struct vlan_ethhdr));*/

	memcpy (&ltr_pdu.cfm_header, pkt->data, pkt->data_len);

	ltr_pdu.cfm_header.first_tlv_offset = 6;
	ltr_pdu.cfm_header.opcode           = CFM_LINKTRACE_REPLY_OPCODE;

	if(!flag)
		ltr_pdu.cfm_header.flag 			= 0xc0;//foward
	else
		ltr_pdu.cfm_header.flag 			= 0x80;//not foward
	
	
	if(flag)
	{
		ltr_pdu.action                      = CFM_LTM_RELAY_HIT;
	}
	else
	{
		ltr_pdu.action                      = CFM_LTM_RELAY_FDB;
	}

	ltr_pdu.ttl -=1;//ttl change
		
	//memcpy(&ltr_pdu.ethhdr.h_dest, pkt->cb.ethcb.smac, 6);
	memcpy(ltr_pdu.ethhdr.h_dest,   ltm_pdu.origin_mac, 6);
	memcpy(ltr_pdu.ethhdr.h_source, if_mac, 6);

	ltr_pdu.ethhdr.h_tpid      = htons(0x8100);
	ltr_pdu.ethhdr.h_vlan_tci  = htons((pma->priority << 13) | pma->vlan);
	ltr_pdu.ethhdr.h_ethtype   = htons(0x8902);

	rawcb.ethtype    = 0x8902;
	rawcb.priority   = pma->priority;
	rawcb.ttl        = 64;

	//tlvs
	*p++ = LTR_Egress_Identifier_Tlv;
	
	*p++ = 0;
	*p++ = 16;	
	
	*p++ = 0;
	*p++ = 0;
	memcpy(p,pkt->cb.ethcb.smac,6); //last egress
	p += 6;
	*p++ = 0;
	*p++ = 0;
	if(flag)
	{
		memset(p,0,6); //next egress
	}
	else
	{
		memcpy(p,if_mac,6);
	}
	p += 6;

	//tlvs
	*p++ = Reply_Ingress_Tlv;
	
	*p++ = 0;	
	*p++ = 9+strlen(name);
	
	*p++ = 1;
	memcpy(p,if_mac,6);
	p += 6;
	*p++ = strlen(name);//sublen
	*p++ = 5; //subtype
	strcpy((char *)p,name);
	p += strlen(name);
	
	//tlvs
	*p = 0;

	len = ltr_pdu.tlvs+199-p;//remain count
	
	type = IFM_TYPE_ID_GET ( pkt->in_ifindex );

	//send ltr
	if (pkt->in_ifindex != pkt->in_port && type!=IFNET_TYPE_TRUNK)
	{
		/*bug:56330  modify 2018.9.20*/
		if (parent_if == pkt->in_port)
		{
			rawcb.in_ifindex = 0;		
			rawcb.out_ifindex = pkt->in_ifindex;
		}
		else
		{
			/* Only when for pw, up mep is exist */
			rawcb.out_ifindex = 0;		
			rawcb.in_ifindex = pkt->in_ifindex;
		}
		
	}
	else 
	{
		rawcb.in_ifindex = 0;	
		rawcb.out_ifindex = pkt->in_ifindex;
	}

	ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ltr_pdu, sizeof(struct cfm_ltr_pdu)-len); 		

	//forward ltm
	if(!flag)
	{

		memcpy(&ltm_pdu.ethhdr.h_dest, pkt->cb.ethcb.dmac, 6);
		memcpy(&ltm_pdu.ethhdr.h_source, if_mac, 6);
		
		ltm_pdu.ethhdr.h_tpid	   = htons(0x8100);
		ltm_pdu.ethhdr.h_vlan_tci  = htons((pma->priority << 13) | pma->vlan);
		ltm_pdu.ethhdr.h_ethtype   = htons(0x8902);
		
		ltm_pdu.ttl -=1;//ttl change

		if(ltm_pdu.ttl == 0)
		{
			return ERRNO_FAIL;
		}
		
		if (pkt->in_ifindex != pkt->in_port && type!=IFNET_TYPE_TRUNK)//pkt from nni
		{
			if (parent_if == pkt->in_port)
			{
				rawcb.in_ifindex  = pkt->in_ifindex;			
				rawcb.out_ifindex = 0;
			}
			else
			{
				//for pw
				rawcb.in_ifindex  = 0;			
				rawcb.out_ifindex = pkt->in_ifindex;
			}

			zlog_debug(CFM_DBG_LT,"cfm ltm response:in_ifindex:%d out_ifindex:%d,lbm from interface type:%d mode:%d\n", 
					 rawcb.in_ifindex, rawcb.out_ifindex, type, if_mode);
			ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ltm_pdu, pkt->data_len+sizeof(struct vlan_ethhdr)); 		
			
		}
		else //pkt from uni or normal mode
		{
			if(if_mode != IFNET_MODE_L2)
			{
				//for switch
				for(ALL_LIST_ELEMENTS_RO(&(vlan_table[pma->vlan]->portlist),p_listnode,data))
				{
					if(((uint32_t)data) != pkt->in_ifindex)
					{	
						rawcb.in_ifindex = 0;					
						rawcb.out_ifindex = (uint32_t)data;
						ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ltm_pdu, pkt->data_len+sizeof(struct vlan_ethhdr));		
					}
				}
			}
			else
			{
				rawcb.out_ifindex = 0;			
				rawcb.in_ifindex = pkt->in_ifindex;
				ret = pkt_send(PKT_TYPE_RAW, (union pkt_control *)&rawcb, &ltm_pdu, pkt->data_len+sizeof(struct vlan_ethhdr));				
			}
		}
	}	

	return ret;

	
}

/**
 * @brief      : cfm报文接收
 * @param[in ] : pkt， 报文指针
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void cfm_pkt_rcv(struct pkt_buffer *pkt)
{
	struct cfm_sess *sess = NULL;
	struct cfm_pdu_header cfm_header;
	struct cfm_ma *pma = NULL;
	struct listnode *p_listnode = NULL;
	struct listnode *p_listnode1 = NULL;
	uint16_t 		vlan;
	uint8_t 		level;
	unsigned char	 i;
	
	zlog_debug(CFM_DBG_COMMN, "%s:Entering the function of '%s'",__FILE__,__func__);

	/* when pw cfm, pif is L3 */
	//if ((NULL == pif) || (NULL == pkt))
	if (NULL == pkt)
	{
		return;
	}

	memset (&cfm_header, 0, sizeof(struct cfm_pdu_header));

	memcpy (&cfm_header, pkt->data, sizeof(struct cfm_pdu_header));

	
	if(cfm_header.opcode == CFM_APS_OPCODE)
	{		
		aps_recv(pkt);
		zlog_debug(CFM_DBG_COMMN, "[%s] Receive a aps pdu.\r\n", __func__);
		return;
	}
	else if(cfm_header.opcode == CFM_AIS_MESSAGE_OPCODE)
	{
		zlog_debug(CFM_DBG_COMMN, "[%s] Receive a ais pdu.\n\r", __func__);
		cfm_rcv_ais(pkt);
		return;
	}
	else if(cfm_header.opcode == CFM_RAPS_OPCODE)
	{
		raps_recv(pkt);
		zlog_debug(CFM_DBG_COMMN, "[%s] Receive a aps pdu.\r\n", __func__);
		return;
	}
	else if(cfm_header.opcode == CFM_LCK_MESSAGE_OPCODE)
	{
		zlog_debug(CFM_DBG_COMMN, "[%s] Receive a lck pdu.\n\r", __func__);
		cfm_rcv_lck(pkt);
		return;			
	}		

	/* 根据报文查找对应的 cfm ma */	
	//pma = cfm_find_ma(cfm_header.level, pkt->cb.ethcb.svlan);

	level = cfm_header.level;
	vlan  = pkt->cb.ethcb.svlan;
	zlog_debug(CFM_DBG_COMMN, "[%s],Receive a cfm packet, level:%d,vlan:%d \n", __func__,level,vlan);
	
	for(i=0 ; i<CFM_MD_NUM_MAX ; i++)
	{
		if(cfm_md_table[i] != NULL && cfm_md_table[i]->level == level)
		{
		
			p_listnode = cfm_md_table[i]->ma_list.head;
			while(p_listnode)
			{
				pma = listgetdata(p_listnode);

				zlog_debug(CFM_DBG_COMMN, "[%s] loop ma_index:%d\n", __func__,pma->ma_index);
				if(pma->vlan == vlan)
				{
					zlog_debug(CFM_DBG_COMMN,"[%s] match ma_index:%d\n", __func__,pma->ma_index);
					
					if(!pma->mip_enable && !pma->sess_list.count)
					{
						zlog_debug(CFM_DBG_COMMN, "[%s] continue for mip disable  and session count is zero\n", __func__);
						
						p_listnode = p_listnode->next;			
						continue; 		
					}

					if(pma->mip_enable)
					{
						if(cfm_header.opcode == CFM_LOOPBACK_MESSAGE_OPCODE)
						{
							cfm_mip_rev_lbm(pma,pkt);
						}
						else if(cfm_header.opcode == CFM_LINKTRACE_MESSAGE_OPCODE)
						{
							cfm_mip_rev_ltm(pma,pkt);
						}
						
						p_listnode = p_listnode->next;			
						continue;
					}

					for(ALL_LIST_ELEMENTS_RO(&pma->sess_list,p_listnode1,sess))
					{
						zlog_debug(CFM_DBG_COMMN, "[%s] match sess_id:%d\n", __func__,sess->sess_id);
							
						switch (cfm_header.opcode)
						{
							case CFM_CONTINUITY_CHECK_OPCODE:
								/* CCM processed by HW */
								break;
							case CFM_LOOPBACK_REPLY_OPCODE:
								zlog_debug(CFM_DBG_LB, "[%s] Receive a lbr pdu.\n\r", __func__);
								cfm_rcv_lbr(sess, pkt);
								break;
							case CFM_LOOPBACK_MESSAGE_OPCODE:
								zlog_debug(CFM_DBG_LB, "[%s] Receive a lbm pdu.\n\r", __func__);
								cfm_rcv_lbm(sess, pkt);
								break;
							case CFM_LINKTRACE_REPLY_OPCODE:
								zlog_debug(CFM_DBG_LT, "[%s] Receive a ltr pdu.\n\r", __func__);
								cfm_rcv_ltr(sess, pkt);
								break;
							case CFM_LINKTRACE_MESSAGE_OPCODE:
								zlog_debug(CFM_DBG_LT, "[%s] Receive a ltm pdu.\n\r", __func__);
								cfm_rcv_ltm(sess, pkt);
								break;
							case CFM_TEST_MESSAGE_OPCODE:
								zlog_debug(CFM_DBG_TEST, "[%s] Receive a test pdu.\n\r", __func__);
								cfm_rcv_test(sess,pkt);
								break;
								
							default: 
								break;
						}				
						
						
					}						


				}					

				p_listnode = p_listnode->next;			
			}			
			
			
			zlog_debug(CFM_DBG_COMMN, "[%s] ,level:%d match ok,return\n\r", __func__,level);
			return; 
		}
	}

	zlog_debug(CFM_DBG_COMMN, "[%s],Receive a cfm packet but drop, level:%d not match \n", __func__,level);

	return;
}


/**
 * @brief      : snmp查询md
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : md_buff，输出md数据块
 * @return     : md数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_md_get_bulk(void *pdata, int data_len, struct cfm_md md_buff[])
{
    uchar md_id  = *((uchar *)pdata);
	uchar index;
    
    int msg_num  = IPC_MSG_LEN/sizeof(struct cfm_md);
    int data_num = 0;

    
    if(md_id == 0)/* 1st get */
    {

		for(index = 0;index<=(CFM_MD_NUM_MAX-1);index++)
		{
			if(cfm_md_table[index])
			{
				memcpy(&md_buff[data_num++], cfm_md_table[index], sizeof(struct cfm_md));
				if (data_num == msg_num)
				{
					return data_num;
				}
			}
		}

    }
    else
    {
		for(index = 0;index<=(CFM_MD_NUM_MAX-1);index++)
		{
			if(((index+1)>md_id) && cfm_md_table[index])
			{
				memcpy(&md_buff[data_num++], cfm_md_table[index], sizeof(struct cfm_md));
				if (data_num == msg_num)
				{
					return data_num;
				}
			}
		}
    }
    
    return data_num;
    
}

/**
 * @brief      : snmp查询ma
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : ma_buff，输出ma数据块
 * @return     : ma数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_ma_get_bulk(void *pdata, int data_len, struct cfm_ma ma_buff[])
{
    struct cfm_ma_key ma_key;
    u_int8_t  md_id;          
    u_int16_t ma_id;          
    
    int msg_num  = IPC_MSG_LEN/sizeof(struct cfm_ma);
    int data_num = 0;
	
	struct cfm_md *pmd=NULL;
	struct cfm_ma *pma=NULL ;   
	uchar index;
	struct listnode *node = NULL;

	ma_key = *((struct cfm_ma_key*)pdata);
	md_id  = ma_key.md_id;
	ma_id  = ma_key.ma_id;
 
	zlog_info("%s[%d]: '%s' md_id [%d] ma_id [%d]\n",__FILE__, __LINE__, __func__ , 
			   md_id , ma_id);   


    if (cfm_ma_table.num_entries == 0)
    {  
    	zlog_err("%s[%d]: '%s' ,there is no ma in ma table \n",__FILE__, __LINE__, __func__ );   
        return 0;
    }
	
	if(md_id>CFM_MD_NUM_MAX || ma_id > CFM_MA_NUM_MAX)
	{
    	zlog_err("%s[%d]: '%s' ,id is invalid \n",__FILE__, __LINE__, __func__); 
		return 0;
	}
	
    if(md_id == 0) //first get
    {
		for(index = 0;index<=(CFM_MD_NUM_MAX-1);index++)
		{
			pmd = cfm_md_table[index];
			
			if(pmd)
			{			
				for (ALL_LIST_ELEMENTS_RO (&pmd->ma_list, node,  pma))
				{
					memcpy(&ma_buff[data_num++], pma, sizeof(struct cfm_ma));
					if (data_num == msg_num)
					{
						return data_num;
					}
				}
			}
		}        
    }
	else
	{
		
		index = md_id-1;

		if(ma_id == 0)
		{
			for(index=index ;index<=(CFM_MD_NUM_MAX-1);index++)
			{
				pmd   = cfm_md_table[index];

				if(pmd)
				{
					for (ALL_LIST_ELEMENTS_RO (&pmd->ma_list, node,	pma))//from head
					{
						memcpy(&ma_buff[data_num++], pma, sizeof(struct cfm_ma));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}				
			}
		}
		else
		{		
			
			pma = cfm_ma_lookup(ma_id);
			
			if(!pma)
			{
				return 	data_num;
			}

			for(index=index ;index<=(CFM_MD_NUM_MAX-1);index++)
			{
				pmd   = cfm_md_table[index];
			
				if(pmd)
				{
					if(index == (md_id-1))//first loop
					{
						node = listnode_lookup(&pmd->ma_list, pma); 	
						if(!node)
							return data_num;
						
						node = listnextnode(node);//from next node
					}
					else
					{
						node = pmd->ma_list.head;//from head
					}
					
					for (node=node; node!=NULL; node=listnextnode(node))
					{
						pma = listgetdata(node);
						memcpy(&ma_buff[data_num++], pma, sizeof(struct cfm_ma));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}
			
			}						
			
		}
		
	}
    
    return data_num;

}

/**
 * @brief      : snmp查询session
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : sess_buff，输出session数据块
 * @return     : session数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_session_get_bulk(void *pdata, int data_len, struct cfm_sess sess_buff[])
{
    struct cfm_session_key sess_key;
    u_int16_t ma_id;          
    u_int16_t sess_id;   
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct cfm_sess);
    int data_num = 0;
	struct cfm_ma *pma=NULL;	
	struct cfm_sess *psess=NULL ;   
	u_int16_t index;
	struct listnode *node = NULL;

	struct hash_bucket *pbucket=NULL;

	sess_key = *((struct cfm_session_key*)pdata);
	ma_id  = sess_key.ma_id;
	sess_id  = sess_key.sess_id;
 
	zlog_debug(CFM_DBG_COMMN, "%s[%d]: '%s' ma_id [%d] sess_id [%d]\n",__FILE__, __LINE__, __func__ , 
			   ma_id , sess_id);   


    if (cfm_sess_table.num_entries == 0)
    {  
    	zlog_err("%s[%d]: '%s' ,there is no session in session table \n",__FILE__, __LINE__, __func__ );   
        return 0;
    }
	
	if(ma_id > CFM_MA_NUM_MAX || sess_id > 8191)
	{
    	zlog_err("%s[%d]: '%s' ,id is invalid \n",__FILE__, __LINE__, __func__); 
		return 0;
	}
	
    if(ma_id == 0) //first get
    {
		for(index = 0;index< CFM_MA_NUM_MAX;index++)
		{
			pbucket = cfm_ma_table.buckets[index];
			
			if(pbucket)
			{			
				pma = pbucket->data;
				
				for (ALL_LIST_ELEMENTS_RO (&pma->sess_list, node,  psess))
				{
					if(psess->local_mep == 0)
						continue;
				
					memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
					if (data_num == msg_num)
					{
						return data_num;
					}
				}
			}
		}        
    }
	else
	{
		
		index = ma_id;

		if(sess_id == 0)
		{
			for(index=index ;index<CFM_MA_NUM_MAX;index++)
			{
				pbucket = cfm_ma_table.buckets[index];

				if(pbucket)
				{				
					pma = pbucket->data;
					
					for (ALL_LIST_ELEMENTS_RO (&pma->sess_list, node,  psess))
					{
						if(psess->local_mep == 0)
							continue;
					
						memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}				
			}
		}
		else
		{		
			
			psess = cfm_session_lookup(sess_id|(ma_id<<16));
			
			if(!psess)
			{
				return 	data_num;
			}

			for(index=index ;index<CFM_MA_NUM_MAX;index++)
			{
			
				pbucket = cfm_ma_table.buckets[index];
			
				if(pbucket)
				{
					pma = pbucket->data;
					
					if(index == ma_id)//first loop
					{
						node = listnode_lookup(&pma->sess_list, psess); 	
						if(!node)
							return data_num;
						
						node = listnextnode(node);//from next node
					}
					else
					{
						node = pma->sess_list.head;//from head
					}
					
					for (node=node; node!=NULL; node=listnextnode(node))
					{
						psess = listgetdata(node);
						
						if(psess->local_mep == 0)
							continue;
						
						memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}
			
			}						
			
		}
		
	}
    
    return data_num;

}

/**
 * @brief      : snmp查询sla
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : sla_buff，输出sla数据块
 * @return     : sla数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_sla_get_bulk(void *pdata, int data_len, struct sla_measure sla_buff[])
{
    struct cfm_session_key sess_key;
    u_int16_t ma_id;          
    u_int16_t sess_id;   
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct sla_measure);
    int data_num = 0;
	struct cfm_ma *pma=NULL;	
	struct cfm_sess *psess=NULL ;   
	u_int16_t index;
	struct listnode *node = NULL;

	struct hash_bucket *pbucket=NULL;

	sess_key = *((struct cfm_session_key*)pdata);
	ma_id  = sess_key.ma_id;
	sess_id  = sess_key.sess_id;
 
	zlog_debug(CFM_DBG_COMMN, "%s[%d]: '%s' ma_id [%d] sess_id [%d]\n",__FILE__, __LINE__, __func__ , 
			   ma_id , sess_id);   


    if (cfm_sess_table.num_entries == 0)
    {  
    	zlog_err("%s[%d]: '%s' ,there is no session in session table \n",__FILE__, __LINE__, __func__ );   
        return 0;
    }
	
	if(ma_id > CFM_MA_NUM_MAX || sess_id > 8191)
	{
    	zlog_err("%s[%d]: '%s' ,id is invalid \n",__FILE__, __LINE__, __func__); 
		return 0;
	}
	
    if(ma_id == 0) //first get
    {
		for(index = 0;index<CFM_MA_NUM_MAX;index++)
		{
			pbucket = cfm_ma_table.buckets[index];
			
			if(pbucket)
			{			
				pma = pbucket->data;
				
				for (ALL_LIST_ELEMENTS_RO (&pma->sess_list, node,  psess))
				{
					if(psess->psla)
					{
						memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct cfm_sess));
						if (data_num == msg_num)
						{
							return data_num;
						}
					}
				}
			}
		}        
    }
	else
	{
		
		index = ma_id;

		if(sess_id == 0)
		{
			for(index=index ;index<CFM_MA_NUM_MAX;index++)
			{
				pbucket = cfm_ma_table.buckets[index];

				if(pbucket)
				{				
					pma = pbucket->data;
					
					for (ALL_LIST_ELEMENTS_RO (&pma->sess_list, node,  psess))
					{
						if(psess->psla)
						{
							memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct cfm_sess));
							if (data_num == msg_num)
							{
								return data_num;
							}
						}
					}
				}				
			}
		}
		else
		{		
			
			psess = cfm_session_lookup(sess_id|(ma_id<<16));
			
			if(!psess)
			{
				return 	data_num;
			}

			for(index=index ;index<CFM_MA_NUM_MAX;index++)
			{
			
				pbucket = cfm_ma_table.buckets[index];
			
				if(pbucket)
				{
					pma = pbucket->data;
					
					if(index == ma_id)//first loop
					{
						node = listnode_lookup(&pma->sess_list, psess); 	
						if(!node)
							return data_num;
						
						node = listnextnode(node);//from next node
					}
					else
					{
						node = pma->sess_list.head;//from head
					}
					
					for (node=node; node!=NULL; node=listnextnode(node))
					{
						psess = listgetdata(node);

						if(psess->psla)
						{
							memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct cfm_sess));
							if (data_num == msg_num)
							{
								return data_num;
							}
						}
					}
				}
			
			}						
			
		}
		
	}
    
    return data_num;

}
/**
 * @brief      : snmp查询session
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : sess_buff，输出session数据块
 * @return     : session数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_session_get_bulk_new(void *pdata, int data_len, struct cfm_sess sess_buff[])
{
    struct cfm_session_key sess_key;
    u_int16_t sess_id;   
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct cfm_sess);
    int data_num = 0;
	struct cfm_sess *psess=NULL ;   
	u_int16_t index;
	u_int8_t add_flag = 0;

	struct hash_bucket *pbucket=NULL;

	sess_key = *((struct cfm_session_key*)pdata);
	sess_id  = sess_key.sess_id;
 
	zlog_debug(CFM_DBG_COMMN, "%s[%d]: '%s' sess_id [%d]\n",__FILE__, __LINE__, __func__ ,  sess_id);   


    if (cfm_sess_table_new.num_entries == 0)
    {  
    	zlog_err("%s[%d]: '%s' ,there is no session in session table \n",__FILE__, __LINE__, __func__ );   
        return 0;
    }
	
	if(sess_id > 1024)
	{
    	zlog_err("%s[%d]: '%s' ,sess_id %d is invalid \n",__FILE__, __LINE__, __func__,sess_id); 
		return 0;
	}

	if(sess_id < 1023)  //0-1022
	{	
		add_flag = 1;
		index = sess_id+1;
		
		for(index= sess_id+1; index < CFM_MEP_NUM_MAX; index++)
		{
			pbucket = cfm_sess_table_new.buckets[index];
			
			if(pbucket)
			{			
				psess = pbucket->data;
				
				memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
				if (data_num == msg_num)
				{
					return data_num;
				}
			}
		}		 
		
	}
	else if(sess_id == 1023)//1023
	{
		pbucket = cfm_sess_table_new.buckets[0];
		if(pbucket)
		{
			psess = pbucket->data;
			
			memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
			
		}
	}

	if(add_flag)
	{
		pbucket = cfm_sess_table_new.buckets[0];
		if(pbucket)
		{
			psess = pbucket->data;
			
			memcpy(&sess_buff[data_num++], psess, sizeof(struct cfm_sess));
			
		}
	}
	
    
    return data_num;

}

/**
 * @brief      : snmp查询sla
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : data_len， 来自snmp数据长度
 * @param[out] : sla_buff，输出sla数据块
 * @return     : sla数据块个数
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

static int cfm_sla_get_bulk_new(void *pdata, int data_len, struct sla_measure sla_buff[])
{
    struct cfm_session_key sess_key;
    u_int16_t sess_id;   
	
    int msg_num  = IPC_MSG_LEN/sizeof(struct sla_measure);
    int data_num = 0;
	struct cfm_sess *psess=NULL ;   
	u_int16_t index;

	struct hash_bucket *pbucket=NULL;
    u_int8_t add_flag = 0;   

	sess_key = *((struct cfm_session_key*)pdata);
	sess_id  = sess_key.sess_id;
 
	zlog_debug(CFM_DBG_COMMN, "%s[%d]: '%s' , sess_id [%d]\n",__FILE__, __LINE__, __func__ , sess_id); 

	if (cfm_sess_table_new.num_entries == 0)
	{  
	   zlog_err("%s[%d]: '%s' ,there is no session in session table \n",__FILE__, __LINE__, __func__ );   
	   return 0;
	}

	if(sess_id > 1024)
	{
	   zlog_err("%s[%d]: '%s' ,sess_id %d is invalid \n",__FILE__, __LINE__, __func__,sess_id); 
	   return 0;
	}


	if(sess_id < 1023)	//0-1022
	{	
		add_flag = 1;
		index = sess_id+1;
		
		for(index=sess_id+1; index < CFM_MEP_NUM_MAX; index++)
		{
			pbucket = cfm_sess_table_new.buckets[index];
			
			if(pbucket)
			{			
				psess = pbucket->data;
				
				if(psess->psla)
				{
					memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct sla_measure));
					if (data_num == msg_num)
					{
						return data_num;
					}
				}
			}
		}		 
		
	}
	else if(sess_id == 1023)//1023
	{
		pbucket = cfm_sess_table_new.buckets[0];
		if(pbucket)
		{
			psess = pbucket->data;
			
			if(psess->psla)
			{
				memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct sla_measure));
			}
			
		}
	}
	
	if(add_flag)
	{
		pbucket = cfm_sess_table_new.buckets[0];
		if(pbucket)
		{
			psess = pbucket->data;
			
			if(psess->psla)
			{
				memcpy(&sla_buff[data_num++], psess->psla, sizeof(struct sla_measure));
			}
			
		}
	}

    
    return data_num;

}

/**
 * @brief      : snmp查询分发接口
 * @param[in ] : pdata， 来自snmp数据指针
 * @param[in ] : pmsghdr， ipc message header pointer
 * @param[out] : 
 * @return     : 
 * @author     : huoqq
 * @date       : 2018年3月23日 15:07:19
 * @note       : 
 */

void cfm_snmp_manage(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
    int ret = 0;
    int msg_len = 0;

    if(pmsghdr->msg_subtype ==  CFM_SNMP_MD_GET)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct cfm_md);
        struct cfm_md md_buff[msg_len];
        memset(md_buff, 0, msg_len*sizeof(struct cfm_md));
        ret = cfm_md_get_bulk(pdata, pmsghdr->data_len, md_buff);
#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(md_buff, ret*sizeof(struct cfm_md), ret, pmsghdr->sender_id, 
                                        MODULE_ID_L2,IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
        {
            ret = ipc_send_reply_n2(md_buff, ret*sizeof(struct cfm_md), ret, pmsghdr->sender_id, 
                     MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
        else
        {
			ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
        }
    }
    else if(pmsghdr->msg_subtype == CFM_SNMP_MA_GET)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct cfm_ma);
        struct cfm_ma ma_buff[msg_len];
        memset(ma_buff, 0, msg_len*sizeof(struct cfm_ma));

        ret = cfm_ma_get_bulk(pdata, pmsghdr->data_len, ma_buff);

#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(ma_buff, ret*sizeof(struct cfm_ma), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
		{
            ret = ipc_send_reply_n2(ma_buff, ret*sizeof(struct cfm_ma), ret, pmsghdr->sender_id,
                      MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
        else
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
        }
       
    }
    else if(pmsghdr->msg_subtype == CFM_SNMP_SESSION_GET)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct cfm_sess);
        struct cfm_sess sess_buff[msg_len];
        memset(sess_buff, 0, msg_len*sizeof(struct cfm_sess));

        ret = cfm_session_get_bulk(pdata, pmsghdr->data_len, sess_buff);
#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(sess_buff, ret*sizeof(struct cfm_sess), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
        {
            ret = ipc_send_reply_n2(sess_buff, ret*sizeof(struct cfm_sess), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->sequence, pmsghdr->msg_index,IPC_OPCODE_REPLY);
        }
        else
        {
			ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
        }
    }
    else if(pmsghdr->msg_subtype == CFM_SNMP_SLA_GET)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct sla_measure);
        struct sla_measure sla_buff[msg_len];
        memset(sla_buff, 0, msg_len*sizeof(struct sla_measure));

        ret = cfm_sla_get_bulk(pdata, pmsghdr->data_len, sla_buff);
#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(sla_buff, ret*sizeof(struct sla_measure), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
        {
            ret = ipc_send_reply_n2(sla_buff, ret*sizeof(struct sla_measure), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
        else
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
        }
    }
    else if(pmsghdr->msg_subtype == CFM_SNMP_SESSION_GET_NEW)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct cfm_sess);
        struct cfm_sess sess_buff[msg_len];
        memset(sess_buff, 0, msg_len*sizeof(struct cfm_sess));

        ret = cfm_session_get_bulk_new(pdata, pmsghdr->data_len, sess_buff);
#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(sess_buff, ret*sizeof(struct cfm_sess), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
		{
			ret = ipc_send_reply_n2(sess_buff, ret*sizeof(struct cfm_sess), ret, pmsghdr->sender_id,
									MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);
		}
		else
		{
			ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
		}

    }
    else if(pmsghdr->msg_subtype == CFM_SNMP_SLA_GET_NEW)
    {
        msg_len = IPC_MSG_LEN/sizeof(struct sla_measure);
        struct sla_measure sla_buff[msg_len];
        memset(sla_buff, 0, msg_len*sizeof(struct sla_measure));

        ret = cfm_sla_get_bulk_new(pdata, pmsghdr->data_len, sla_buff);
#if 0
        if (ret > 0)
        {
            ret = ipc_send_reply_bulk(sla_buff, ret*sizeof(struct sla_measure), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
        else
        {
            ret = ipc_send_noack(ERRNO_NOT_FOUND, pmsghdr->sender_id, MODULE_ID_L2, IPC_TYPE_CFM,
                                    pmsghdr->msg_subtype, pmsghdr->msg_index);
        }
#endif

		if (ret > 0)
        {
            ret = ipc_send_reply_n2(sla_buff, ret*sizeof(struct sla_measure), ret, pmsghdr->sender_id,
                                    MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype, pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_REPLY);
        }
        else
        {
            ret = ipc_send_reply_n2(NULL, 0, 0, pmsghdr->sender_id, 
				MODULE_ID_L2, IPC_TYPE_CFM, pmsghdr->msg_subtype,  pmsghdr->sequence, pmsghdr->msg_index, IPC_OPCODE_NACK);
        }
    }
        
    return ;

}

int cfm_ipc_send_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	int    ret = 0;
    int    rcvlen = 0;

    struct ipc_mesg_n *pMsgSnd = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n)+data_len, MODULE_ID_L2);
    if(pMsgSnd == NULL) 
	{
		return -1;
    }

    memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));

	pMsgSnd->msghdr.data_len    = data_len;
	pMsgSnd->msghdr.module_id   = module_id;
	pMsgSnd->msghdr.sender_id   = sender_id;
	pMsgSnd->msghdr.msg_type    = msg_type;
	pMsgSnd->msghdr.msg_subtype = subtype;
	pMsgSnd->msghdr.msg_index   = msg_index;
	pMsgSnd->msghdr.data_num    = data_num;
	pMsgSnd->msghdr.opcode      = opcode;

	if (pdata)
	{
		memcpy(pMsgSnd->msg_data, pdata, data_len);
	}

    ret = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &rcvlen, 0);

    switch(ret)
    {
       case -1:
             mem_share_free(pMsgSnd, MODULE_ID_L2);
             return -1;

       case -2: 
            return -1;

       case  0:
             if(pMsgRcv == NULL) 
			 {
			 	ret = -1;
             }
             else 
             {
             	/*if noack return errcode*/
				if(IPC_OPCODE_NACK == pMsgRcv->msghdr.opcode)
				{
					memcpy(&ret, pMsgRcv->msg_data, sizeof(ret));
							       
				}
				/*if ack return 0*/
				else if(IPC_OPCODE_ACK == pMsgRcv->msghdr.opcode)
				{
					ret = 0;
				}
				else
				{
					ret = -1;
				}

                 mem_share_free(pMsgRcv, MODULE_ID_L2);
             }
             return ret;
             
        default: 
			return-1;
    }

	return ret;
}

int cfm_msg_send_noack (uint32_t errcode, struct ipc_msghdr_n *rcvhdr,uint32_t msg_index)
{
	struct ipc_mesg_n * repmesg = NULL;
	uint32_t msg_len = 0;
	
	msg_len = sizeof(struct ipc_msghdr_n)+sizeof(uint32_t);
	
	repmesg = ipc_msg_malloc(msg_len, MODULE_ID_L2);
	if(NULL == repmesg)	
	{
		return ERRNO_FAIL;
			
	}

	memset(repmesg,0,msg_len);
	
	memcpy(repmesg->msg_data,&errcode,4);
	
	repmesg->msghdr.data_len = 4;   /*sizeof(errcode)*/
	repmesg->msghdr.data_num = 1;                 
	repmesg->msghdr.msg_index = msg_index;
	repmesg->msghdr.opcode = IPC_OPCODE_NACK;
	
	if(ipc_send_reply_n1(rcvhdr, repmesg, msg_len))
	{
		mem_share_free(repmesg, MODULE_ID_L2);	
		return ERRNO_FAIL;
	}

	return ERRNO_SUCCESS;
		
}

int cfm_send_hal(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
            enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret =0;
    struct ipc_mesg_n *pMsgSnd =  NULL;

	if(NULL== pdata)
	{
		return -1;
	}

    pMsgSnd = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_L2);
	if (NULL == pMsgSnd)
	{
		return -1;
	}

    pMsgSnd->msghdr.data_len    = data_len;
    pMsgSnd->msghdr.module_id   = module_id;
    pMsgSnd->msghdr.sender_id   = sender_id;
    pMsgSnd->msghdr.msg_type    = msg_type;
    pMsgSnd->msghdr.msg_subtype = msg_subtype;
    pMsgSnd->msghdr.msg_index   = msg_index;
    pMsgSnd->msghdr.data_num    = data_num;
    pMsgSnd->msghdr.opcode      = opcode;

    if(pdata)
    {
        memcpy(pMsgSnd->msg_data, pdata, data_len);
    }

    ret = ipc_send_msg_n1(pMsgSnd, sizeof(struct ipc_msghdr_n) + data_len);
    if(ret != 0) 
    {
        mem_share_free(pMsgSnd, MODULE_ID_L2);
    }
     
    return 0;
}

