/***********************************************************************
*
* ftm_arp.c
*
* manage the arp table
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <lib/zassert.h>
#include <lib/inet_ip.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/thread.h>
#include <lib/ether.h>
#include <lib/index.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <route/arp_cmd.h>

#include "ftm_debug.h"
#include "ftm_lsp.h"
#include "ftm_ifm.h"
#include "ftm_nhp.h"
#include "ftm_fib.h"
#include "pkt_ip.h"
#include "pkt_eth.h"
#include "ftm_pkt.h"
#include "ftm_ifm.h"
#include "ftm_tunnel.h"
#include "ftm.h"
#include "pkt_arp.h"
#include "ftm_arp.h"


struct arp_global garp;
struct hash_table arp_table;
struct arp_to_static garp_to_static_list;


/*计算hash key*/
static unsigned int arp_Hash(void *parp)
{
	unsigned int uiHashValue;

	uiHashValue = ((struct arp_key *)parp)->ipaddr + ((struct arp_key *)parp)->vpnid;

	return uiHashValue;
}


/*比较hash key*/
static int arp_Compare(void *item, void *hash_key)
{
    struct arp_key *pstKey = NULL;
    struct arp_key *pstKeyCompare = NULL;

    if ((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    pstKey = (struct arp_key *)hash_key;
    pstKeyCompare = (struct arp_key *)(((struct hash_bucket *)item)->hash_key);
    if (NULL == pstKeyCompare)
    {
        return ERRNO_FAIL;
    }

    if ((pstKey->ipaddr == pstKeyCompare->ipaddr) &&
        (pstKey->vpnid == pstKeyCompare->vpnid))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}


/*hash 表初始化*/
void arp_table_init(unsigned int uiSize)
{
    hios_hash_init(&arp_table, uiSize, arp_Hash, arp_Compare);
	index_register(INDEX_TYPE_ARP, uiSize);
}


/*arp 添加函数*/
int  arp_add(struct arp_entry *pstArp, int ms_flag)
{
	struct ftm_ifm *pifm = NULL;
	struct hash_bucket *pstItem = NULL;

	if(NULL == pstArp)
	    return ERRNO_FAIL;

    FTM_ARP_ZLOG_DEBUG("garp.num_limit:%d garp.count:%d garp.num_static:%d\n",
                                    garp.num_limit,garp.count,garp.num_static);

	pstItem = hios_hash_find(&arp_table, (void *)(&pstArp->key));
	if (pstItem)
	{
		return arp_update(pstArp, pstItem->data);
	}
	else
	{
		/*添加静态arp */
		if(pstArp->status == ARP_STATUS_STATIC)
		{
            FTM_ARP_ZLOG_DEBUG("Add static arp!\n");

            if(GSTATIC_ARP_LT_GLIMIT && GARP_NUM_LT_GLIMIT)
            {
				garp.num_static++;
				arp_add_entry(pstArp, ms_flag);
			}
			else if(GSTATIC_ARP_LT_GLIMIT && GARP_NUM_EQ_GLIMIT)
			{
			    /*先删除一个动态arp 之后再添加*/
				arp_delete_one();
				garp.num_static++;
				arp_add_entry(pstArp, ms_flag);
			}
		}
		else
		{
    		FTM_ARP_ZLOG_DEBUG("ifindex:0x%x\n",pstArp->ifindex);

            pifm = ftm_ifm_lookup(pstArp->ifindex);
            if(!pifm || !(pifm->parp))
                return ERRNO_FAIL;

            FTM_ARP_ZLOG_DEBUG("parp->arp_num:%d parp->arp_num_max:%d\n",
                             pifm->parp->arp_num ,pifm->parp->arp_num_max);

            if (GARP_NUM_LT_GLIMIT &&
                GDYNAMIC_ARP_LT_GLIMIT &&
                PIF_DYNAMIC_ARP_LT_PLIMIT(pifm))
            {
                /*physical or physical subport slot dynamic arp count*/
                if (IFM_TYPE_IS_PHYSICAL(pstArp->ifindex) || 
                    (IFM_TYPE_IS_PHYSICAL(pstArp->ifindex) && (IFM_TYPE_IS_SUBPORT(pstArp->ifindex))))
                {
                    
     
                    if (!SLOT_DYNAMIC_ARP_LT_SLIMIT(pstArp->ifindex))
                    {
                        FTM_ARP_ZLOG_DEBUG("It is fail!\n");
                        return ERRNO_FAIL;
                    }
                    else
                    {
                        SLOT_DYNAMIC_ARP_CNT_INC((pstArp->ifindex));
                    }
                }
               
				garp.count++;
                pifm->parp->arp_num ++;
				arp_add_entry(pstArp, ms_flag);
			}
		}
	}

    return ERRNO_SUCCESS;
}


/*arp 添加到hash 并通知hal、nhp、lsp添加*/
void arp_add_entry(struct arp_entry *pstArp, int ms_flag)
{
    struct hash_bucket *pstItem  = NULL;
    struct arp_entry *pstArp_new = NULL;
    uint32_t num = 0;
    int      ret = 0;

    if(pstArp == NULL)
    {
        FTM_ARP_ZLOG_ERR("pstArp is NULL!\n");
        return;
    }

    FTM_ARP_ZLOG_DEBUG("ip:0x%x vpn:%d\n",pstArp->key.ipaddr,pstArp->key.vpnid);

    pstItem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET,sizeof (struct hash_bucket));
    pstArp_new = (struct arp_entry *)XCALLOC(MTYPE_ARP_ENTRY,sizeof (struct arp_entry));
    if(!pstItem || !pstArp_new)
    {
        FTM_ARP_ZLOG_ERR("Xcalloc failed!\n");
        return;
    }

    /*分配arp 存储索引*/
    num = index_alloc(INDEX_TYPE_ARP);
    if(!num)
    {
        FTM_ARP_ZLOG_ERR("Failed to alloc index for arp!\n");
        XFREE(MTYPE_ARP_ENTRY,pstArp_new );
        XFREE(MTYPE_HASH_BACKET,pstItem );
        return;
    }

    /*存储arp 表项到hash 表*/
    memcpy(pstArp_new, pstArp, sizeof(struct arp_entry));
    pstArp_new->arpindex = num + ARP_INDEX_BASE;
    pstItem->data = pstArp_new;
    pstItem->hash_key = &(pstArp_new->key);
    ret = hios_hash_add(&arp_table, pstItem);
    if(ret)
    {
        FTM_ARP_ZLOG_ERR("Add arp err! ipaddr:0x%x\n",pstArp_new->key.ipaddr);
        index_free(INDEX_TYPE_ARP,pstArp->arpindex - ARP_INDEX_BASE);
        XFREE(MTYPE_ARP_ENTRY, pstArp_new);
        XFREE(MTYPE_HASH_BACKET,pstItem);
        return;
    }

    /*添加半连接时不更新nhp,不通知lsp*/
    if(pstArp_new->status != ARP_STATUS_INCOMPLETE)
    {
        ftm_lsp_process_arp_event(pstArp_new, OPCODE_ADD);
        ftm_tunnel_process_arp_event(pstArp_new, OPCODE_ADD);
        ftm_nhp_update_arp(pstArp_new, 1);
    }

    /*for arp entry sync between master and slave*/
	if(ms_flag != ARP_SLAVE)
	{
    	FTM_ARP_DYNAMIC_BOARD_SYNC(pstArp_new, IPC_OPCODE_ADD);
	}
    /*通知hal 添加arp 表项*/
    ret = ftm_msg_send_to_hal(pstArp_new, sizeof(struct arp_entry), 1,
                                    IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_ADD, 0);
    if(-1 == ret)
    {
        FTM_ARP_ZLOG_ERR("Notify hal add arp failed!\n");
        return;
    }

}


/*判断ifindex、port、status是否更新有更新则返回0*/
int arp_update_info(struct arp_entry *parp_new, struct arp_entry *parp_old)
{
	int flag =0;

	if(parp_old->ifindex != parp_new->ifindex)
	{
		parp_old->ifindex = parp_new->ifindex;
		flag++;
	}

	if(memcmp(parp_old->mac,parp_new->mac,MAC_LEN))
	{
		memcpy(parp_old->mac,parp_new->mac,MAC_LEN);
		flag++;
	}

    if(parp_old->port != parp_new->port)
	{
		parp_old->port = parp_new->port;
		flag++;
	}

	if(parp_old->status !=  parp_new->status)
	{
		parp_old->status = parp_new->status;
		flag++;
	}
	
	if(parp_old->status_old !=  parp_new->status_old)
	{
		parp_old->status_old = parp_new->status_old;
		flag++;
	}

	if(flag != 0)
		return 0;
	else
		return 1;
}


/*arp 表项更新通知lsp、nhp、hal*/
int  arp_update(struct arp_entry *parp_new, struct arp_entry *parp_old)
{
    uint32_t old_ifindex = 0;
    uint8_t  update_flag = 1;
	int      ret = 0;
	struct ftm_ifm *pifm_old = NULL;
	struct ftm_ifm *pifm_new = NULL;
    
    if (!parp_old || !parp_new )
    {
        FTM_ARP_ZLOG_ERR();
        return ERRNO_FAIL;
    }

    /*静态arp 更新到静态arp*/
	if((parp_old->status == ARP_STATUS_STATIC)&&(parp_old->status_old == 0))
	{
        if(parp_new->status == ARP_STATUS_STATIC)
        {
            FTM_ARP_ZLOG_DEBUG("Static arp update to static arp!\n");

    		update_flag = arp_update_info(parp_new,parp_old);

            //重复下发静态arp时，通知hal 添加静态路由
            if(update_flag != 0)
            {
            	return ERRNO_SUCCESS;
            	#if 0
                ftm_lsp_process_arp_event(parp_old, OPCODE_UPDATE);
                ftm_tunnel_process_arp_event(parp_old, OPCODE_UPDATE);
                ret = ftm_msg_send_to_hal(parp_old, sizeof(struct arp_entry), 1,
                                          IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
				#endif
				}

        }
	}

    /*半连接、老化、全连接表项更新到全连接*/
	if((parp_old->status == ARP_STATUS_INCOMPLETE)
		||(parp_old->status == ARP_STATUS_COMPLETE)
		||(parp_old->status == ARP_STATUS_AGED)
		||(parp_old->status == ARP_STATUS_STATIC && parp_old->status_old != 0))
	{
		if(parp_new->status == ARP_STATUS_COMPLETE 
			|| (parp_new->status == ARP_STATUS_STATIC && parp_new->status_old == ARP_STATUS_COMPLETE))
        {
            FTM_ARP_ZLOG_DEBUG("Dynamic arp update!\n");

            if (parp_old->ifindex != parp_new->ifindex)
            {
                pifm_old = ftm_ifm_lookup(parp_old->ifindex);
                pifm_new = ftm_ifm_lookup(parp_new->ifindex);
                if (pifm_old && pifm_new && (pifm_old->parp) && (pifm_new->parp))
                {
                    if (PIF_DYNAMIC_ARP_LT_PLIMIT(pifm_new))
                    {
                        FTM_ARP_ZLOG_ERR("updae arp ifindex success! old_ifindex:0x%x new_ifindex:0x%x\n",parp_old->ifindex,parp_new->ifindex);
                        pifm_old->parp->arp_num --;
                        pifm_new->parp->arp_num ++;
                    }
                    else
                    {
                        FTM_ARP_ZLOG_ERR("updae arp ifindex fail! old_ifindex:0x%x new_ifindex:0x%x\n",parp_old->ifindex,parp_new->ifindex);
                        return ERRNO_FAIL;
                    }
                }
            }
            
            /*for arp entry sync between master and slave*/
            FTM_ARP_DYNAMIC_BOARD_SYNC(parp_old, IPC_OPCODE_DELETE);

    		update_flag = arp_update_info(parp_new,parp_old);

            parp_old->time = garp.age_time;
    		parp_old->count = 0;
            
            /*for arp entry sync between master and slave*/
            FTM_ARP_DYNAMIC_BOARD_SYNC(parp_old, IPC_OPCODE_ADD);

            if(update_flag != 0)//无更新则直接返回
                return ERRNO_FAIL;

        }
    }

    /*动态arp 表项更新到静态arp 表项*/
	if((parp_old->status == ARP_STATUS_INCOMPLETE)
		||(parp_old->status == ARP_STATUS_COMPLETE)
		||(parp_old->status == ARP_STATUS_AGED)
        ||(parp_old->status == ARP_STATUS_LLDP))
	{
		if(parp_new->status == ARP_STATUS_STATIC)
        {
            FTM_ARP_ZLOG_DEBUG("Dynamic arp update to static arp!\n");

            /*如果静态arp 数量达到静态最大规格则更新失败*/
            if(!GSTATIC_ARP_LT_GLIMIT)
            {
                FTM_ARP_ZLOG_DEBUG("Dynamic arp update to static fail!\n");
                return ERRNO_FAIL;
            }

    		update_flag = arp_update_info(parp_new,parp_old);

            if(update_flag != 0)//无更新则直接返回
                return ERRNO_FAIL;

    		FTM_ARP_ZLOG_DEBUG("old_ifindex:0x%x gcount:%d gnum_static:%d\n",
                                 parp_old->ifindex,garp.count,garp.num_static);

            /*更新全局arp 计数*/
			garp.count--;
			garp.num_static++;

            /*更新接口arp 计数*/
            old_ifindex = parp_old->ifindex;
            pifm_old = ftm_ifm_lookup(old_ifindex);
            if(pifm_old && (pifm_old->parp) && (pifm_old->parp->arp_num > 0))
                pifm_old->parp->arp_num--;

        }
	}


    //arp更新通知lsp、nhp、hal 更新
    if((update_flag == 0))
	{
		ftm_lsp_process_arp_event(parp_old, OPCODE_UPDATE);
        ftm_tunnel_process_arp_event(parp_old, OPCODE_UPDATE);
		ftm_nhp_update_arp(parp_old, 1);
		ret = ftm_msg_send_to_hal(parp_old, sizeof(struct arp_entry), 1,
                                  IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
	}

    if(ret == -1)
    {
        FTM_ARP_ZLOG_ERR("Notify hal update arp failed!\n");
        return ERRNO_FAIL;
    }

    return ERRNO_SUCCESS;
}


/*arp 表项删除*/
int  arp_delete(uint32_t uiIpaddr, uint16_t usVpn, int ms_flag)
{
	struct  hash_bucket *pstItem = NULL;
	struct  arp_entry *pstArp = NULL;
	struct  ftm_ifm *pifm = NULL;
    struct  arp_key stKey;

    FTM_ARP_ZLOG_DEBUG("ip:0x%x vpn:%d\n",uiIpaddr,usVpn);

    stKey.ipaddr = uiIpaddr;
	stKey.vpnid = usVpn;
	pstItem = hios_hash_find(&arp_table, (void *)&stKey);
	if(pstItem)
	{
		pstArp = pstItem->data;
		if(pstArp->status == ARP_STATUS_STATIC)
		{
			garp.num_static--;
			return arp_delete_entry(pstArp,pstItem, ms_flag);
		}
		else
		{
            /*更新全局arp 计数*/
            garp.count--;

            /*slot cnt decrease*/
            
            if (IFM_TYPE_IS_PHYSICAL(pstArp->ifindex) || 
                    (IFM_TYPE_IS_PHYSICAL(pstArp->ifindex) && (IFM_TYPE_IS_SUBPORT(pstArp->ifindex))))
            {
                SLOT_DYNAMIC_ARP_CNT_DEC((pstArp->ifindex));
            }

            pifm = ftm_ifm_lookup(pstArp->ifindex);
            if(pifm && (pifm->parp)&& (pifm->parp->arp_num > 0))
				pifm->parp->arp_num--;

            return arp_delete_entry(pstArp,pstItem, ms_flag);
		}
	}

    return ERRNO_SUCCESS;
}


/*从hash 表删除arp 并通知nhp、lsp、hal 删除*/
int arp_delete_entry(struct arp_entry *pstArp,struct hash_bucket *pstItem, int ms_flag)
{
    int ret = 0;

    if(pstArp == NULL || pstItem == NULL)
    {
        FTM_ARP_ZLOG_ERR("pstArp or pstItem is NULL!\n");
        return ERRNO_FAIL;
    }

    FTM_ARP_ZLOG_DEBUG("ip:0x%x vpn:%d\n",pstArp->key.ipaddr,pstArp->key.vpnid);

    ret = ftm_msg_send_to_hal(pstArp, sizeof(struct arp_entry), 1,
                             IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_DELETE, 0);
    if(-1 == ret)
    {
        FTM_ARP_ZLOG_ERR("Notify hal delete arp failed!\n");
        //return ERRNO_FAIL; /*if hal_arp delete failed,delete ftm_arp first. otherwise arp_cnt error! Add arp fail!*/
    }
    
    /*for arp entry sync between master and slave*/
	if(ms_flag != ARP_SLAVE)
	{
    	FTM_ARP_DYNAMIC_BOARD_SYNC(pstArp, IPC_OPCODE_DELETE);
	}

    hios_hash_delete(&arp_table, pstItem);

    ftm_nhp_update_arp(pstArp, 0);
    if(pstArp->status != ARP_STATUS_INCOMPLETE)
    {
        ftm_lsp_process_arp_event(pstArp, OPCODE_DELETE);//arp删除通知lsp删除
        ftm_tunnel_process_arp_event(pstArp, OPCODE_DELETE);
    }

    index_free(INDEX_TYPE_ARP,pstArp->arpindex-10000 );
    XFREE( MTYPE_ARP_ENTRY, pstArp );
    XFREE( MTYPE_HASH_BACKET, pstItem );

    return ERRNO_SUCCESS;
}


/*随机删除一条动态arp表项*/
int arp_delete_one(void)
{
	uint32_t cursor = 0;
    struct   arp_entry *parp = NULL;
    struct   hash_bucket *pbucket = NULL;

    FTM_ARP_ZLOG_DEBUG();

	HASH_BUCKET_LOOP(pbucket,cursor,arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			if(parp->status != ARP_STATUS_STATIC)
                return arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
		}
	}

    return ERRNO_SUCCESS;
}


/*老化所有的动态arp */
int  arp_delete_all(void)
{
	uint32_t cursor = 0;
	uint32_t num = 0;
	int      ret = 0;
    struct   arp_entry   *parp = NULL;
    struct   hash_bucket *pbucket = NULL;

    FTM_ARP_ZLOG_DEBUG();

	HASH_BUCKET_LOOPW(pbucket,cursor,arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;
			arp_delete( parp->key.ipaddr, parp->key.vpnid, ARP_MASTER);
			num++;
			#if 0
			if(parp->status == ARP_STATUS_COMPLETE)
			{
				parp->time = ARP_TIMER*3;
				parp->status = ARP_STATUS_AGED;
				num ++;

				//arp删除通知lsp删除 (置为age状态)
				ftm_lsp_process_arp_event(parp, OPCODE_UPDATE);
                ftm_tunnel_process_arp_event(parp, OPCODE_UPDATE);

				ret = ftm_msg_send_to_hal(parp, sizeof(struct arp_entry), 1,
                                       IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
			    if(ret == -1)
			    {
                    FTM_ARP_ZLOG_ERR("Notify hal update arp failed!\n");
					return ERRNO_FAIL;
			    }
			}
			#endif
		}
	}

    FTM_ARP_ZLOG_DEBUG("%d dynamic arp wait aged!\n",num);

    return num;
}


/*老化指定接口的动态arp*/
int  arp_delete_interface(uint32_t ifindex)
{
	uint32_t cursor = 0;
	uint32_t num = 0;
	int      ret = 0;
    struct arp_entry *parp = NULL;
    struct hash_bucket *pbucket = NULL;

    FTM_ARP_ZLOG_DEBUG("ifindex:0x%x\n",ifindex)

	HASH_BUCKET_LOOPW(pbucket,cursor,arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;
			if( parp->ifindex == ifindex)
			{
				arp_delete( parp->key.ipaddr, parp->key.vpnid, ARP_MASTER);
				num++;
			}

			#if 0
			if((parp->status == ARP_STATUS_COMPLETE)&&(parp->ifindex == ifindex))
			{
				parp->time = ARP_TIMER*3;
				parp->status = ARP_STATUS_AGED;
				num ++;

				//arp删除通知lsp删除 (置为age状态)
				ftm_lsp_process_arp_event(parp, OPCODE_UPDATE);
                ftm_tunnel_process_arp_event(parp, OPCODE_UPDATE);

				ret = ftm_msg_send_to_hal(parp, sizeof(struct arp_entry),1,
                                      IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
			    if(ret == -1)
			    {
                    FTM_ARP_ZLOG_ERR("Notify hal update arp failed!\n");
                    return ERRNO_FAIL;
			    }
			}
			#endif
		}
	}

	FTM_ARP_ZLOG_DEBUG("%d dynamic arp wait aged!\n",num);

	return num;
}


/*老化指定动态arp*/
int arp_delete_ip(uint32_t ipaddr)
{
	uint32_t cursor = 0;
	uint32_t num = 0;
	int      ret = 0;
	struct arp_entry *parp = NULL;
	struct hash_bucket *pbucket = NULL;

    FTM_ARP_ZLOG_DEBUG("ipaddr:0x%x\n",ipaddr);

	HASH_BUCKET_LOOPW( pbucket, cursor, arp_table)
	{
		if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;
			if(parp->key.ipaddr == ipaddr)
			{
				arp_delete( parp->key.ipaddr, parp->key.vpnid, ARP_MASTER);
				num++;
			}
			#if 0
			if((parp->status == ARP_STATUS_COMPLETE)&&(parp->key.ipaddr == ipaddr))
			{
				parp->time = ARP_TIMER*3;
				parp->status = ARP_STATUS_AGED;
				num ++;

				//arp删除通知lsp删除 (置为age状态)
				ftm_lsp_process_arp_event(parp, OPCODE_UPDATE);
                ftm_tunnel_process_arp_event(parp, OPCODE_UPDATE);

				ret = ftm_msg_send_to_hal(parp, sizeof(struct arp_entry), 1,
                                       IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
				if(ret == -1)
				{
                    FTM_ARP_ZLOG_ERR("Notify hal update arp failed!\n");
					return ERRNO_FAIL;
				}
			}
			#endif
		}
	}

    FTM_ARP_ZLOG_DEBUG("%d dynamic arp wait aged!\n",num);

	return num;

}


/*老化指定vpn 的动态arp*/
int  arp_delete_vpn(uint16_t vpnid)
{
	uint32_t cursor = 0;
	uint32_t num = 0;
	int      ret = 0;
    struct arp_entry *parp = NULL;
    struct hash_bucket *pbucket = NULL;

    FTM_ARP_ZLOG_DEBUG("vpn:%d\n",vpnid);

	HASH_BUCKET_LOOPW( pbucket, cursor, arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;
			if(parp->key.vpnid == vpnid)
			{
				arp_delete( parp->key.ipaddr, parp->key.vpnid, ARP_MASTER);
				num++;				
			}
			#if 0
			if((parp->status == ARP_STATUS_COMPLETE)&&(parp->key.vpnid == vpnid))
			{
				parp->time = ARP_TIMER*3;
				parp->status = ARP_STATUS_AGED;
				num ++;

				//arp删除通知lsp删除 (置为age状态)
				ftm_lsp_process_arp_event(parp, OPCODE_UPDATE);
                ftm_tunnel_process_arp_event(parp, OPCODE_UPDATE);

				ret = ftm_msg_send_to_hal(parp, sizeof(struct arp_entry), 1,
                                       IPC_TYPE_ARP, ARP_INFO_HAL_ENTRY, IPC_OPCODE_UPDATE, 0);
			    if(ret == -1)
			    {
                    FTM_ARP_ZLOG_ERR("Notify hal update arp failed!\n");
                    return ERRNO_FAIL;
			    }
			}
			#endif
		}
	}

    FTM_ARP_ZLOG_DEBUG("%d dynamic arp wait aged!\n",num);

	return num;
}


/*接口down 删除接口下动态arp */
int  arp_delete_if_linkdown(uint32_t ifindex)
{
	uint32_t cursor = 0;
	struct arp_entry *parp = NULL;
	struct arp_entry *parp_temp = NULL;
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pbucket_temp = NULL;
    struct ftm_ifm *pifm = NULL;

    FTM_ARP_ZLOG_DEBUG("ifindex:0x%x\n",ifindex);

    pifm = ftm_ifm_lookup(ifindex);
    if(pifm == NULL)
        return ERRNO_FAIL;

	HASH_BUCKET_LOOPW(pbucket,cursor,arp_table)
	{
		if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
            parp_temp = parp;
			pbucket_temp = pbucket;
			pbucket = pbucket->next;

			if((parp->status != ARP_STATUS_STATIC)&&(parp->ifindex == pifm->ifm.ifindex))
			{
                garp.count--;

                /*更新接口计数*/
                if (IFM_TYPE_IS_PHYSICAL(parp_temp->ifindex) || 
                    (IFM_TYPE_IS_PHYSICAL(parp_temp->ifindex) && (IFM_TYPE_IS_SUBPORT(parp_temp->ifindex))))
                {
                    SLOT_DYNAMIC_ARP_CNT_DEC((parp_temp->ifindex));
                }

                if((pifm->parp)&& (pifm->parp->arp_num > 0))
                    pifm->parp->arp_num--;

                arp_delete_entry(parp_temp,pbucket_temp, ARP_MASTER);
			}
		}
	}

	return ERRNO_SUCCESS;
}


/*去接口代理删除动态arp*/
void arp_delete_if_proxy_disable(struct ftm_ifm *pifm)
{
    uint32_t mip = 0;//主ip
    uint8_t  mip_prefix = 0;
    uint32_t sip = 0;//从ip
    uint32_t sip_prefix = 0;
    uint32_t cursor = 0;
	uint8_t  flag = 0;
	struct arp_entry *parp = NULL;
	struct arp_entry *parp_temp = NULL;
	struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pbucket_temp = NULL;

    FTM_ARP_ZLOG_DEBUG();

    if(pifm == NULL)
        return;

    /*获取接口主从ip 及掩码*/
    mip = pifm->pl3if->ipv4[0].addr;
    mip_prefix = pifm->pl3if->ipv4[0].prefixlen;
    sip = pifm->pl3if->ipv4[1].addr;
    sip_prefix = pifm->pl3if->ipv4[1].prefixlen;

    HASH_BUCKET_LOOPW(pbucket,cursor,arp_table)
    {
	    if(pbucket->data)
	    {
	        parp = (struct arp_entry *)pbucket->data;
			pbucket_temp = pbucket;
			pbucket = pbucket->next;
			parp_temp = pbucket_temp->data;

            //代理删除只删除动态arp
            if((parp->status == ARP_STATUS_STATIC) || (parp->status == ARP_STATUS_LLDP))
                continue;

            if(parp->ifindex != pifm->ifm.ifindex)
                continue;

            /*主ip 存在，从ip 不存在*/
            if(ARP_IF_IP_EXIST(mip) && !ARP_IF_IP_EXIST(sip))
            {
                flag = ipv4_is_same_subnet(mip,parp->key.ipaddr,mip_prefix);
            }
            else if(ARP_IF_IP_EXIST(mip) && ARP_IF_IP_EXIST(sip))
            {
                if(ipv4_is_same_subnet(mip,parp->key.ipaddr,mip_prefix)
                   || ipv4_is_same_subnet(sip,parp->key.ipaddr,sip_prefix))
                {
                    flag = 1;
                }
            }

            /*存在与当前arp 同网段的接口ip*/
            if(flag)
			{
                /*更新arp 计数*/
                garp.count--;
                if((pifm->parp)&& (pifm->parp->arp_num > 0))
                    pifm->parp->arp_num--;
		        
                //arp删除通知lsp 、nhp、hal删除
                if (IFM_TYPE_IS_PHYSICAL(parp_temp->ifindex) || 
                    (IFM_TYPE_IS_PHYSICAL(parp_temp->ifindex) && (IFM_TYPE_IS_SUBPORT(parp_temp->ifindex))))
                {
                    SLOT_DYNAMIC_ARP_CNT_DEC((parp_temp->ifindex));
                }

                arp_delete_entry(parp_temp,pbucket_temp, ARP_MASTER);
			}
		}
	}
}


void arp_delete_vlanif_port_down(uint32_t port)
{
    struct arp_entry *parp = NULL;
    struct arp_entry *parp_temp = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct ftm_ifm *pifm = NULL;
    struct ftm_ifm *port_ifm = NULL;
    uint32_t cursor = 0;
    
    /*if port is not physicat port,return. 
     *vlanif member port mustbe physical port or trunk*/
    if ((port <= 0) || !(IFM_TYPE_IS_PHYSICAL(port)))
    {
        FTM_ARP_ZLOG_ERR("port:0x%x\n", port);
        return;
    } 
    
    port_ifm = ftm_ifm_lookup(port);
    if (!port_ifm) 
        return;
    if (port_ifm->ifm.mode != IFNET_MODE_SWITCH)
        return;

    FTM_ARP_ZLOG_DEBUG("port:0x%x\n", port);
    
	HASH_BUCKET_LOOPW(pbucket,cursor,arp_table)
	{
		if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
            parp_temp = parp;
			pbucket_temp = pbucket;
			pbucket = pbucket->next;

			if((parp->status != ARP_STATUS_STATIC)&&(parp->port == port))
			{
                /*更新全局计数*/
                garp.count--;

               pifm = ftm_ifm_lookup(parp->ifindex);
               if(pifm == NULL) return;

                /*更新接口计数*/
                if((pifm->parp)&& (pifm->parp->arp_num > 0))
                    pifm->parp->arp_num--;
    			
                //arp删除通知lsp 、nhp、hal删除
                arp_delete_entry(parp_temp,pbucket_temp, ARP_MASTER);

			}
		}
	}
}
/*arp delete by reset slot*/
void ftm_arp_reset_slot(uint8_t slot_num)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct arp_entry *parp = NULL;
    uint32_t cursor = 0;

    if (slot_num > 8)
    {
        FTM_ARP_ZLOG_ERR();
        return;
    }

    FTM_ARP_ZLOG_DEBUG("slot_num:0x%x\n", slot_num);

    HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket->next;
        
        parp = (struct arp_entry *)(pbucket_temp->data);
        if (parp && (slot_num == IFM_SLOT_ID_GET(parp->ifindex)))
        {
            arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
        }
    }
}


void ftm_arp_slot_numlimit(uint32_t data)
{
    uint32_t slot_num_limit = 0;
    uint16_t slot = 0;
    uint16_t num_limit = 0;
    uint32_t cursor = 0;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct arp_entry *parp = NULL;

    slot_num_limit = data;
    slot = slot_num_limit >> 16;
    num_limit = slot_num_limit & 0xffff;/*get low 16bit*/

    FTM_ARP_ZLOG_DEBUG("slot_num_limit:0x%x slot:0x%x num_limit:0x%x\n", slot_num_limit, slot, num_limit);

    if (slot > SLOT_MAX)
    {
        FTM_ARP_ZLOG_ERR();
        return;
    }

    garp.slot_num_limit[slot][0] = num_limit; 

    /*if this slot's arp num gt than num_limit,then delete it*/
    if (num_limit < garp.slot_num_limit[slot][1])
    {
        HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
        {
            pbucket_temp = pbucket;
            pbucket = pbucket->next;

            parp = (struct arp_entry *)(pbucket_temp->data);
            if (parp && (slot == IFM_SLOT_ID_GET(parp->ifindex)))
            {
                arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
            }

            if (num_limit == garp.slot_num_limit[slot][1])
            {
                break;
            }
        }   
    }
}


/*查找指定的arp 表项*/
struct arp_entry * arp_lookup(uint32_t uiIpaddr, uint16_t usVpn)
{
    struct arp_key stKey;
    struct hash_bucket *pstItem = NULL;

    stKey.ipaddr = uiIpaddr;
    stKey.vpnid  = usVpn;

    pstItem = hios_hash_find(&arp_table, (void *)&stKey);

    if (NULL == pstItem)
    {
        return NULL;
    }

    return (struct arp_entry *)pstItem->data;
}


/*查找指定arp 没有则触发学习*/
struct arp_entry * arp_lookup_active(uint32_t ipaddr, uint16_t vpn)
{
	struct arp_entry *parp = NULL;

	parp = arp_lookup(ipaddr, vpn);
	if(!parp)
	{
		arp_miss(ipaddr, vpn);
		return NULL;
	}
	else if(parp->status == ARP_STATUS_INCOMPLETE)
    	return NULL;
	else
    	return parp;
}

void arp_fake_entry_time_flush(uint8_t time)
{   
    struct hash_bucket *pbucket = NULL;
    struct arp_entry *parp = NULL;
    uint32_t cursor = 0;

    FTM_ARP_ZLOG_DEBUG("fake_time:%d\n",time);
    garp.fake_expire_time = time;
    HASH_BUCKET_LOOP(pbucket, cursor, arp_table)
    {
        parp = pbucket->data;
        if(!parp || parp->status != ARP_STATUS_INCOMPLETE) continue;

        parp->time = time;
    }
}

static int arp_fake_entry_age()
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct arp_entry *parp = NULL;
    struct ftm_ifm *pif = NULL;
    uint8_t smac[6];
    uint32_t cursor = 0;
    uint32_t addr = 0;

	HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
	{
	    pbucket_temp = pbucket;
        pbucket = pbucket_temp->next;

        parp = pbucket_temp->data;
        if(parp && (parp->status == ARP_STATUS_INCOMPLETE) && !parp->gratuious)
        {
            if (parp->time <= 0)
            {
                arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
            }
            else
            {
				pif = ftm_ifm_lookup(parp->ifindex);
                if(pif && (pif->parp) && (pif->parp->arp_disable) && (parp->count < 2))
                {
                    FTM_IFM_GET_MAC(pif,smac);
                                
                	addr = ftm_ifm_get_l3if_ipv4(pif->pl3if, parp->key.ipaddr);
                    arp_send(parp, ARP_OPCODE_REQUEST, addr, smac);
                    parp->count ++;
                }
                
                parp->time --;
            }
        }
    }
	high_pre_timer_add("ArpHalfTimer", LIB_TIMER_TYPE_NOLOOP, arp_fake_entry_age, NULL, 1000);/* thread to  half join */

    return ERRNO_SUCCESS;
}
static int arp_age ()
{
	uint32_t cursor = 0;
    uint32_t addr = 0;
	uint8_t  smac[6];
    struct arp_entry *parp = NULL;
    struct hash_bucket *pbucket = NULL;
    struct ftm_ifm *pif = NULL;

	HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;

            /*静态arp 不老化*/
			if((parp->status == ARP_STATUS_STATIC && parp->status_old == 0)     || 
               parp->status == ARP_STATUS_INCOMPLETE ||
               parp->status == ARP_STATUS_LLDP)
			    continue;

			if(parp->time > garp.age_time)
			{
				parp->time = garp.age_time;
			}
			
			if(parp->status_old == 0)
			parp->time -= ARP_TIMER;
			
            /*剩余老化时间小于1/3 老化时间，发送请求次数小于3时老化探测*/
			if((parp->time <= garp.age_time/3)&&(parp->time >= ARP_TIMER)&& (parp->count < 3))
			{
                FTM_ARP_ZLOG_DEBUG("[Probe] ip:0x%x time:%d\n",parp->key.ipaddr,parp->time);

				pif = ftm_ifm_lookup(parp->ifindex);
                if(pif && (pif->parp) && (pif->parp->arp_disable))
                {
                    FTM_IFM_GET_MAC(pif,smac);

                    /*获取arp 报文源ip*/
                	addr = ftm_ifm_get_l3if_ipv4(pif->pl3if, parp->key.ipaddr);
                    arp_send(parp, ARP_OPCODE_REQUEST, addr, smac);
					
					if(parp->status_old == 0)
    					parp->count++;
                }
			}
			else if(parp->time < ARP_TIMER)
			{
				FTM_ARP_ZLOG_DEBUG("[Delete] parp->time:%d\n" ,parp->time);

                arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
			}
			else
			{
			    FTM_ARP_ZLOG_DEBUG("[Age] parp->time:%d\n" ,parp->time);

				//parp->time -= ARP_TIMER;
			}
		}
	}

	return ERRNO_SUCCESS;
}


/*初始化函数*/
void ftm_arp_init(void)
{
    uint8_t i = 0;

    /*init global arp contril struct*/
    memset(&garp, 0, sizeof(struct arp_global));
	garp.num_limit = ARP_TOTAL_NUM;
	garp.age_time = ARP_AGED_TIME;
	garp.timer = ARP_TIMER;
    garp.fake_expire_time = ARP_FAKE_TIME;
    for (i = 0; i < SLOT_MAX; i++)
        garp.slot_num_limit[i][0] = ARP_TOTAL_NUM;
    
	arp_table_init(ARP_TOTAL_NUM);
	pkt_arp_init();
	high_pre_timer_add("ArpAgeTimer", LIB_TIMER_TYPE_LOOP, arp_age, NULL, ARP_TIMER*1000);/* thread to age arp */
	arp_fake_entry_age();

	garp_to_static_list.status = ARP_TO_STATIC_DIS;
	garp_to_static_list.arp_to_static_list = list_new();
}
int ftm_arp_get_statis(struct ftm_arp_count *arp_statis)
{
	int cursor = 0;
    struct arp_entry *arp = NULL;
    struct hash_bucket *pbucket = NULL;

    HASH_BUCKET_LOOP ( pbucket, cursor, arp_table)
    {
        if(pbucket->data)
        {
         	arp = (struct arp_entry *)pbucket->data;

			switch(arp->status)
			{
				case ARP_STATUS_STATIC:
					arp_statis->arp_static_num++;
					break;
				case ARP_STATUS_COMPLETE:
					arp_statis->arp_complete_num++;
					break;
				case ARP_STATUS_INCOMPLETE:
					arp_statis->arp_incomplete_num++;
					break;
				case ARP_STATUS_AGED:
					arp_statis->arp_aged_num++;
					break;
                case ARP_STATUS_LLDP:
                    arp_statis->arp_rule_num++;
                    break;
				default:
		        	return 0;
		        }
			}

    	}
	arp_statis->arp_total_num = arp_statis->arp_static_num + arp_statis->arp_complete_num + 
								arp_statis->arp_incomplete_num + arp_statis->arp_aged_num + 
								arp_statis->arp_rule_num;
    return 0;
}

/*处理 arp 全局消息*/
int ftm_arp_msg(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode, sint32 sender_id)
{
    uint8_t  i = 0;
    uint8_t *data = NULL;
    struct arp_entry *pstArpItem = NULL;
	struct ftm_arp_count arp_statis;
	struct ftm_ifm *pifm = NULL;

    FTM_ARP_ZLOG_DEBUG();

	if(!pdata)
	{
		return ERRNO_FAIL;
	}
    data = (uint8_t *)pdata;
    for(i = 0; i < iDataNum; i++)
    {
        switch (ucSubtype)
    	{
        	case ARP_INFO_STATIC_ARP:
            case ARP_INFO_LLDP:
        	{
    	        pstArpItem = (struct arp_entry *)data;
				if(ucSubtype == ARP_INFO_LLDP)
				{
					pifm = ftm_ifm_lookup(pstArpItem->ifindex);
		            if(!pifm || !(pifm->parp))
		            {
						FTM_ARP_ZLOG_DEBUG(" ifindex=%x pifm or pifm->parp is NULL\n",pstArpItem->ifindex);
						return ERRNO_FAIL;
		            }
					if(!pifm->pl3if)
					{
						FTM_ARP_ZLOG_DEBUG(" if not is l3 mode \n");
						return ERRNO_FAIL;
		            }
					pstArpItem->key.vpnid = pifm->pl3if->vpn;
				}
    	        if(enOpcode == IPC_OPCODE_DELETE)
    	        {
    	            arp_delete(pstArpItem->key.ipaddr, pstArpItem->key.vpnid, ARP_MASTER);
    	        }
    	        else if(enOpcode == IPC_OPCODE_ADD)
    	        {
    	            arp_add(pstArpItem, ARP_MASTER);
    	        }
                data += sizeof(struct arp_entry);
                break;
        	}
    		case ARP_INFO_NUM_LIMIT:
    		{
        		ftm_arp_global_add(data, ARP_INFO_NUM_LIMIT);
                data += sizeof(struct arp_global);
                break;
    		}
    		case ARP_INFO_AGE_TIME:
    		{
        		ftm_arp_global_add(data, ARP_INFO_AGE_TIME);
                data += sizeof(struct arp_global);
                break;
    		}
            case ARP_INFO_FAKE_TIME:
            {
        		ftm_arp_global_add(data, ARP_INFO_FAKE_TIME);
                data += sizeof(struct arp_global);
                break;
            }
            case ARP_INFO_FIXED:
            {
        		ftm_arp_global_add(data, ARP_INFO_FIXED);
                data += sizeof(struct arp_global);
                break;
            }
            case ARP_INFO_RESET_SLOT:
            {
                ftm_arp_reset_slot(*(uint8_t *)data);
                break;
            }
            case ARP_INFO_SLOT_NUM_LIMIT:
            {
                ftm_arp_slot_numlimit(*(uint32_t *)data);
                break;
            }
			case ARP_INFO_CONF_STATIS:
			{
				memset(&arp_statis, 0, sizeof(struct ftm_arp_count));
				ftm_arp_get_statis (&arp_statis);
			  	ipc_send_reply_n2(&arp_statis, sizeof(struct ifm_arp), 1, sender_id,
                        MODULE_ID_FTM, IPC_TYPE_ARP, ARP_INFO_CONF_STATIS, 1, 0, IPC_OPCODE_REPLY);
				

				break;
            }	
            default:
            {
                FTM_ARP_ZLOG_DEBUG("Sub_type err!sub_type:%d\n",ucSubtype);
                break;
            }
        }
            
    }

    return ERRNO_SUCCESS;
}
int ftm_arp_devm_msg(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode)
{
	uint8_t i = 0;
    uint8_t *data = NULL;

    FTM_ARP_ZLOG_DEBUG("pdata:%p opcode:%d\n",pdata,enOpcode);

    if(pdata == NULL)
        return ERRNO_FAIL;
    data = (uint8_t *)pdata;
	for(i = 0;i < iDataNum;i++)
	{
        if(enOpcode == IPC_OPCODE_EVENT)
        {
			if((DEV_EVENT_SLOT_ADD == ucSubtype)||
			  (DEV_EVENT_SLOT_DELETE == ucSubtype)||
			  (DEV_EVENT_HA_BOOTSTATE_CHANGE == ucSubtype))
            {
                ftm_brard_status_change_process((struct devm_com *)pdata);
            }
		}


	}

	return ERRNO_SUCCESS;
}

#if 0
int ftm_arp_msg_mib_num(void *pdata, int iDataLen, int iDataNum, uint8_t ucSubtype, enum IPC_OPCODE enOpcode, uint16_t sender_id)
{
    uint8_t  i = 0;
    uint8_t *data = NULL;
	struct ftm_arp_count arp_statis;

    FTM_ARP_ZLOG_DEBUG();
    
    data = (uint8_t *)pdata;
    for(i = 0; i < iDataNum; i++)
    {
        switch (ucSubtype)
    	{
			case ARP_INFO_CONF_STATIS:
			{
				memset(&arp_statis, 0, sizeof(struct ftm_arp_count));
				ftm_arp_get_statis (&arp_statis);
			  	ipc_send_reply_n2(&arp_statis, sizeof(struct ifm_arp), 1, sender_id,
                        MODULE_ID_FTM, IPC_TYPE_ARP, ARP_INFO_CONF_STATIS, 1, 0);

				break;
            }
            default:
            {
                FTM_ARP_ZLOG_DEBUG("Sub_type err!sub_type:%d\n",ucSubtype);
                break;
            }
        }
            
    }

    return ERRNO_SUCCESS;
}

#endif
/* 下发 arp 全局配置 */
int ftm_arp_global_add(void *pdata, uint8_t subtype)
{
	uint32_t cursor = 0;
	struct hash_bucket *pbucket_temp = NULL;
	struct hash_bucket *pbucket = NULL;
    struct arp_global *garp_temp = NULL;
    struct arp_entry  *parp = NULL;
    struct arp_entry  *parp_temp = NULL;

    FTM_ARP_ZLOG_DEBUG("pdata:%p type:%d\n",pdata,subtype);

    garp_temp = (struct arp_global *)pdata;

    if(subtype == ARP_INFO_AGE_TIME)
	{
		garp.age_time = garp_temp->age_time;

        /*刷新arp 老化时间为配置值*/
        HASH_BUCKET_LOOP(pbucket, cursor, arp_table)
        {
            if(pbucket->data)
            {
                parp = pbucket->data;
                if(parp->status == ARP_STATUS_COMPLETE)
                    parp->time = garp.age_time;
            }
        }
	}

	if(subtype == ARP_INFO_NUM_LIMIT)
	{
		garp.num_limit = garp_temp->num_limit;

        if(GDYNAMIC_ARP_LT_GLIMIT || GDYNAMIC_ARP_EQ_GLIMT)
            return ERRNO_SUCCESS;

		HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
		{
            if(pbucket->data)
            {
                parp = (struct arp_entry *)(pbucket->data);
                parp_temp = parp;
                pbucket_temp = pbucket;
                pbucket = pbucket_temp->next;

                if(parp_temp->status != ARP_STATUS_STATIC)
                {
                    arp_delete(parp_temp->key.ipaddr,parp_temp->key.vpnid, ARP_MASTER);
                }

                if(GDYNAMIC_ARP_EQ_GLIMT || GDYNAMIC_ARP_LT_GLIMIT)
                    return ERRNO_SUCCESS;
            }
		}
	}

    if(subtype == ARP_INFO_FAKE_TIME)
    {
        garp.fake_expire_time = garp_temp->fake_expire_time;
        arp_fake_entry_time_flush(garp.fake_expire_time);
    }

    if(subtype == ARP_INFO_FIXED)
    {
        garp.fixed_flag = garp_temp->fixed_flag;
        FTM_ARP_ZLOG_ERR("fixed_flag:%d\n", garp.fixed_flag);
        //arp_entry_fixed(garp.fixed_flag);
    }
	return ERRNO_SUCCESS;
}


/*l3if 添加时添加接口arp数据结构并初始化*/
int ftm_arpif_add(struct ftm_ifm *pifm)
{
    FTM_ARP_ZLOG_DEBUG();

    if(NULL == pifm)
		return ERRNO_FAIL;

	FTM_ARP_ZLOG_DEBUG("pifm->ifm.ifindex:0x%x\n", pifm->ifm.ifindex);

	/* 保存3层arp信息 */
	if(pifm->parp == NULL)
	{
		pifm->parp = (struct ifm_arp *)XCALLOC(MTYPE_IFM_ENTRY,sizeof(struct ifm_arp));
		if(pifm->parp == NULL)
		{
            FTM_ARP_ZLOG_ERR("Xcalloc failed!\n");
            return ERRNO_MALLOC;
		}
	}


	return ERRNO_SUCCESS;
}


/*l3if 删除时删除接口arp数据接口，并删除接口下动态arp*/
int ftm_arpif_delete(struct ftm_ifm *pifm)
{
    FTM_ARP_ZLOG_DEBUG();

    if(NULL == pifm)
		return ERRNO_FAIL;

	FTM_ARP_ZLOG_DEBUG("pifm->ifm.ifindex:0x%x\n", pifm->ifm.ifindex);

    /*接口arp 信息删除，对应接口下的arp 表项删除*/
    if (pifm->parp == NULL)
	{
		return ERRNO_SUCCESS;
	}
    else
    {
    	if (!(pifm->pl3if))
		{
			XFREE(MTYPE_IFM_ENTRY, pifm->parp);	
			return ERRNO_SUCCESS;
		}

		if (!(ROUTE_IF_IPV6_EXIST(pifm->pl3if->ipv6[0].addr)))
        {
        	XFREE(MTYPE_IFM_ENTRY, pifm->parp);
		}
    }

	return ERRNO_SUCCESS;
}


/*arp接口控制信息更新*/
int ftm_arpif_update(void *pdata, uint32_t ifindex, uint8_t subtype)
{
	uint32_t cursor = 0;
	struct arp_entry *parp = NULL;
	struct hash_bucket *pbucket = NULL;
	struct ftm_ifm *pifm = NULL;
	struct ifm_arp *pif_arp = NULL;

    FTM_ARP_ZLOG_DEBUG("pdata:%p ifindex:0x%x type:%d\n",pdata,ifindex,subtype);

    if(pdata == NULL)
        return ERRNO_FAIL;

    pif_arp = (struct ifm_arp *)pdata;
	pifm = ftm_ifm_lookup(ifindex);
	if((pifm == NULL) || (pifm->parp == NULL))
	{
	    FTM_ARP_ZLOG_ERR("pifm or pifm->parp is NULL!\n");
		return ERRNO_FAIL;
	}

	switch(subtype)
    {
        case ARP_INFO_LEARN_LIMIT:
            pifm->parp->arp_disable = pif_arp->arp_disable;
            break;
        case ARP_INFO_AGENT:
            pifm->parp->arp_proxy = pif_arp->arp_proxy;
			if( pifm->parp->arp_proxy == 0)//禁止代理,接口下不同网段arp表项删除
			{
				arp_delete_if_proxy_disable(pifm);
			}
            break;
		case ARP_INFO_NUM_LIMIT:
            pifm->parp->arp_num_max = pif_arp->arp_num_max;

            if(PIF_DYNAMIC_ARP_LT_PLIMIT(pifm) || PIF_DYNAMIC_ARP_EQ_PLIMIT(pifm))
                return ERRNO_SUCCESS;

			HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
			{
				if(pbucket->data)
				{
					parp = (struct arp_entry *)pbucket->data;
					pbucket = pbucket->next;

					if((parp->status != ARP_STATUS_STATIC)&&(parp->ifindex == ifindex))
					{
                        arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
					}

		            if(PIF_DYNAMIC_ARP_LT_PLIMIT(pifm) || PIF_DYNAMIC_ARP_EQ_PLIMIT(pifm))
                        return ERRNO_SUCCESS;
				}
		    }
            break;
		default:
            return ERRNO_SUCCESS;
	}

    return ERRNO_SUCCESS;
}


/* 处理 arpif 消息 */
int ftm_arpif_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t index)
{
	uint8_t i = 0;
    uint8_t *data = NULL;

    FTM_ARP_ZLOG_DEBUG("pdata:%p opcode:%d\n",pdata,opcode);

    if(pdata == NULL)
        return ERRNO_FAIL;

    data = (uint8_t *)pdata;
	for(i = 0;i < data_num;i++)
	{
        if(opcode == IPC_OPCODE_UPDATE)
		{
			ftm_arpif_update(data, index, subtype);
		}

		data += sizeof(struct ifm_arp);
	}

	return ERRNO_SUCCESS;
}


/*接口ip 删除，删除接口下动态arp */
int ftm_arpif_addr_delete(struct ftm_ifm *pifm,uint8_t ip_flag)
{
    struct arp_entry *parp = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor = 0;

    FTM_ARP_ZLOG_DEBUG("pfim:%p flag:%d\n",pifm,ip_flag);

    if(pifm == NULL || ip_flag > IFM_IP_NUM)
		return ERRNO_FAIL;

    HASH_BUCKET_LOOPW(pbucket, cursor, arp_table)
	{
        if(pbucket->data)
		{
			parp = (struct arp_entry *)pbucket->data;
			pbucket = pbucket->next;

			if((parp->status != ARP_STATUS_STATIC)&&(parp->ifindex == pifm->ifm.ifindex))
			{
                /*接口主ip 删除，删除接口下所有arp 表项*/
                if(ip_flag == 0)
                {
                    arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
                }
                else
                {
                    /*与从ip 同网段的动态arp 删除*/
                    if(ipv4_is_same_subnet(pifm->pl3if->ipv4[ip_flag].addr,parp->key.ipaddr,
                                                      pifm->pl3if->ipv4[ip_flag].prefixlen))
                    {
                        arp_delete(parp->key.ipaddr,parp->key.vpnid, ARP_MASTER);
                    }
                }
			}
		}
	}

    /*接口主 ip 删除初始化arp 数据结构*/
    if (NULL != pifm->parp && 0 == ip_flag)
    {
		pifm->parp->arp_disable = 0;
		pifm->parp->arp_num = 0;
		pifm->parp->arp_num_max = 0;
		pifm->parp->arp_proxy = 0;
    }

    return ERRNO_SUCCESS;
}

/*recv msg from ha*/
int ftm_arp_rcv_ha(struct ipc_mesg_n *pmesg)
{
    struct ipc_msghdr_n *phdr = &(pmesg->msghdr);
    
    FTM_ARP_ZLOG_DEBUG();

    if (phdr == NULL)
    {
        FTM_ARP_ZLOG_ERR();
        return ERRNO_FAIL;
    }

    FTM_ARP_ZLOG_DEBUG("Recv msg from ha!opcode:%d\n", phdr->opcode);

    switch (phdr->opcode)
    {
        case IPC_OPCODE_ADD:
        case IPC_OPCODE_DELETE:
        case IPC_OPCODE_UPDATE:
            ftm_arp_dynamic_sync_restore( pmesg );
            break;
        case IPC_OPCODE_EVENT:
            ftm_arp_dynamic_board_sync();    
            break;
        default:
            FTM_ARP_ZLOG_ERR();
            zlog_err("In func %s ,line:%d\n", __func__, __LINE__);
            break;
    }

    return ERRNO_SUCCESS;
}


int ftm_arp_dynamic_board_sync(void)
{
    struct hash_bucket *pbucket = NULL;
    struct arp_entry *parp = NULL;
    uint32_t cursor = 0;
    uint32_t num_success = 0;
    uint32_t total_num = 0;
    int8_t ret = 0;

    FTM_ARP_ZLOG_DEBUG();

    HASH_BUCKET_LOOP(pbucket, cursor, arp_table)
    {
        parp = (struct arp_entry *)(pbucket->data); 
        if (parp && (parp->status == ARP_STATUS_COMPLETE))
        {
            //ret = ipc_send_ha(parp, sizeof(struct arp_entry), 1, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_ADD, 0);
			ret = ipc_send_msg_n2(parp, sizeof(struct arp_entry), 1, MODULE_ID_FTM, MODULE_ID_FTM, IPC_TYPE_HA, IPC_TYPE_ARP, IPC_OPCODE_ADD, 0);
			if (ret < 0)
            {
                FTM_ARP_ZLOG_ERR("ipc_send ha arp entry error!ip:0x%x vpn:%d\n",parp->key.ipaddr, parp->key.vpnid);
            }
            else
            {
                num_success ++;
            }

            total_num ++;
        }
    }

    FTM_ARP_ZLOG_DEBUG("total arp entry num %d ,sync success num:%d\n", total_num, num_success);
    return num_success;
}

void ftm_arp_dynamic_sync_restore(struct ipc_mesg_n *pmesg)
{
    int8_t ret = 0;
    struct arp_entry *parp = NULL;
    struct ipc_msghdr_n *phdr = NULL;

    FTM_ARP_ZLOG_DEBUG();

    if (pmesg == NULL || pmesg->msg_data == NULL) 
        return;
    phdr = (struct ipc_msghdr_n *)&(pmesg->msghdr);
    
    if (phdr == NULL || phdr->data_num == 0) 
        return;

    FTM_ARP_ZLOG_DEBUG();
   
    parp = (struct arp_entry *)(pmesg->msg_data);
    switch (phdr->opcode)
    {
        case IPC_OPCODE_ADD:
            ret = arp_add( parp , ARP_SLAVE);
            if(ERRNO_SUCCESS != ret)
            {
                FTM_ARP_ZLOG_ERR("ha_arp_add_err! ip:0x%x vpn:%d\n", parp->key.ipaddr, parp->key.vpnid);
            }
            break;
        case IPC_OPCODE_UPDATE:
            FTM_ARP_ZLOG_ERR("This is an error!\n");
            break;
        case IPC_OPCODE_DELETE:
            ret = arp_delete( parp->key.ipaddr, parp->key.vpnid, ARP_SLAVE);
            if(ERRNO_SUCCESS != ret)
            {
                FTM_ARP_ZLOG_ERR("ha_arp_delete_err! ip:0x%x vpn:%d\n", parp->key.ipaddr, parp->key.vpnid);
            }
            break;
        default:
                break;
    }

}


int8_t ftm_arp_cur_board_is_backup(void)
{
    struct devm_unit devm_msg; 
	int ret = 0;
	enum DEV_TYPE dev_type = DEV_TYPE_INVALID;
    
	ret = devm_comm_get_dev_type(MODULE_ID_FTM,&dev_type);
	if( 0 != ret )
	{
		zlog_err("In func %s, line:%d devm_comm_get_dev_type err,ret = %d\n", __func__, __LINE__,ret);
		return -1;
	}
	if( DEV_TYPE_BOX == dev_type )
	{
        return DEV_BOARD_INFO_GET_FAIL;
	}
	/*get slave slot num*/
    if (0 != devm_comm_get_unit(1, MODULE_ID_FTM, &devm_msg)) 
    {
        zlog_err("In func %s, line:%d\n", __func__, __LINE__);
        return DEV_BOARD_INFO_GET_FAIL;
    }
    
    /*if it is  slave board,return 1*/
    if (devm_msg.slot_slave_board == devm_msg.myslot)
    {
        return DEV_BOARD_IS_SLAVE;
    }
    else
    {
        return DEV_BOARD_IS_MASTER;
    }
}


