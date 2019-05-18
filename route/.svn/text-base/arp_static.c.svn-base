/*
*       manage the static arp table
*
*/


#include <limits.h>
#include <string.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/ifm_common.h>

#include "route_main.h"
#include "route_if.h"
#include "arp_static.h"
#include "arp.h"

/*静态arp hash表*/
struct hash_table static_arp_table;


/*hash key计算*/
static unsigned int arp_Hash(void *parp)
{
	unsigned int uiHashValue;

	uiHashValue = ((struct arp_key *)parp)->ipaddr + ((struct arp_key *)parp)->vpnid;

	return uiHashValue;
}

/*hash key 比较*/
static int arp_Compare(void *item, void *hash_key)
{    
    struct arp_key *pstKey = NULL;
    struct arp_key *pstKeyCompare = NULL;

    /* 参数非空 */
    if((NULL == item) || (NULL == hash_key))
    {
        return -1;
    }

    pstKey = (struct arp_key *)hash_key;
    pstKeyCompare = (struct arp_key *)(((struct hash_bucket *)item)->hash_key);
    if(NULL == pstKeyCompare)
    {
        return -1;
    }

    if((pstKey->ipaddr == pstKeyCompare->ipaddr) &&
        (pstKey->vpnid == pstKeyCompare->vpnid))
    {
        return 0;
    }

    return -1;
}


/*hash 表初始化*/
void arp_static_table_init(unsigned int size)
{
    hios_hash_init(&static_arp_table, size, arp_Hash, arp_Compare);
}

/*静态arp 添加*/
int arp_static_add(struct arp_entry *pstArp,uint8_t restore_flag)
{
    struct hash_bucket *pstItem = NULL;
	struct arp_entry *pstArp_new = NULL;
    int iRet = 0;

    if(NULL == pstArp)
    {
        ARP_LOG_ERROR("pstArp is NULL!\n");
        return ARP_ENTRY_IS_NULL;
    }
	if(g_arp.num_static > ARP_STATIC_NUM 
		|| g_arp.num_static == ARP_STATIC_NUM)
	{
        ARP_LOG_ERROR("Add static arp error!\n");
        return ARP_OVER_STATIC_NUM;
	}

    pstItem = hios_hash_find(&static_arp_table, (void *)(&pstArp->key));
	if(pstItem)
	{
	    pstArp_new = pstItem->data;
		memcpy(pstArp_new, pstArp, sizeof(struct arp_entry));
        iRet = ipc_send_msg_n2(pstArp_new, sizeof(struct arp_entry), 1,MODULE_ID_FTM,MODULE_ID_ARP, 
											IPC_TYPE_ARP, 0, IPC_OPCODE_ADD, pstArp_new->ifindex);
        if(iRet)
        {
            ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
            return ERRNO_IPC;
        }
    }    
	else
    { 
        pstArp_new = (struct arp_entry *)XCALLOC(MTYPE_ARP_STATIC_ENTRY, sizeof(struct arp_entry));
        pstItem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        memcpy(pstArp_new, pstArp, sizeof(struct arp_entry));

        pstItem->data = pstArp_new;
        pstItem->hash_key = &(pstArp_new->key);
        iRet = hios_hash_add(&static_arp_table, pstItem);
        if(iRet)
        {	
        	XFREE(MTYPE_ARP_STATIC_ENTRY, pstArp_new);
            XFREE(MTYPE_HASH_BACKET, pstItem);
            return ARP_HASH_BUCKET_FULL;
        }
        else
        {	
        	g_arp.num_static++;

            /*restore configure*/
            if(restore_flag == 1)
            {
                return ERRNO_SUCCESS;
            }

            iRet = ipc_send_msg_n2(pstArp_new, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP, 
												IPC_TYPE_ARP, 0, IPC_OPCODE_ADD, pstArp_new->ifindex);
            if(iRet)
            {
                ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
                return ERRNO_IPC;
            }
        }
    }

    return ERRNO_SUCCESS;
}


/*接口l3if 添加重下静态arp*/
void arp_static_process_l3if_add(uint32_t ifindex)
{
    struct hash_bucket *bucket = NULL;
    struct arp_entry *parp = NULL;
    int cursor = 0;
    int ret = 0;

    ARP_LOG_DEBUG("ifindex:0x%x\n",ifindex);
    
    HASH_BUCKET_LOOP(bucket,cursor,static_arp_table)
    {
        if(bucket->data)
		{
			parp = (struct arp_entry *)bucket->data; 
            if(ifindex == parp->ifindex)
            {
                ret = ipc_send_msg_n2(parp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP, 
													IPC_TYPE_ARP, 0, IPC_OPCODE_ADD, parp->ifindex);
                if(ret)
                {
                    ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
                    return;
                }
            }
        }
    }
}


/*接口l3if 删除，删除接口下静态arp*/
void arp_static_process_l3if_del(uint32_t ifindex)
{    
    struct hash_bucket *bucket = NULL;
    struct arp_entry *parp = NULL;
    struct arp_entry *parp_temp = NULL;
    int cursor = 0;
    int ret = 0;

    ARP_LOG_DEBUG("ifindex:0x%x\n",ifindex);

    HASH_BUCKET_LOOPW(bucket,cursor,static_arp_table)
    {
        if(bucket->data)
		{
			parp = (struct arp_entry *)bucket->data; 
            parp_temp = parp;
            bucket = bucket->next;
            
            if(ifindex == parp->ifindex)
            {
                ret = ipc_send_msg_n2(parp_temp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP,
												IPC_TYPE_ARP, 0, IPC_OPCODE_DELETE, parp_temp->ifindex);
                if(ret)
                {
                    ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
                    return;
                }
				//arp_static_delete(parp_temp->key.ipaddr, parp_temp->key.vpnid);
            }
        }
    }
    
}


/*静态arp 删除*/
int arp_static_delete(uint32_t uiIpaddr, uint16_t usVpn)
{
    struct arp_key stKey;
    struct hash_bucket *pstItem = NULL;
	struct arp_entry *pstArp = NULL;
	int ret = 0;

    memset(&stKey,0,sizeof(struct arp_key));
    stKey.ipaddr = uiIpaddr;
    stKey.vpnid = usVpn;

    ARP_LOG_DEBUG("ip:0x%x vpn:%d\n",uiIpaddr,usVpn);
    
    pstItem = hios_hash_find(&static_arp_table, (void *)&stKey);
    if(NULL != pstItem)
    {   
    	pstArp = pstItem->data;
    	ret = ipc_send_msg_n2(pstArp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP,
										IPC_TYPE_ARP, 0, IPC_OPCODE_DELETE, pstArp->ifindex);
        if(ret)
        {
            ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
            return ERRNO_IPC;
        }

		hios_hash_delete(&static_arp_table, pstItem);
        XFREE(MTYPE_ARP_STATIC_ENTRY, pstArp );
        XFREE(MTYPE_HASH_BACKET, pstItem );
       	g_arp.num_static--;
        return 0;
    }
    else
    {
        return -1;
    }
     
     return 0;
}

void arp_static_delete_all(void)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct arp_entry *parp = NULL;
    uint32_t cursor = 0;
    int8_t   ret = 0;

    HASH_BUCKET_LOOPW(pbucket, cursor, static_arp_table)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket->next;

        if (!(pbucket_temp->data))
            continue;
        
    	parp = pbucket_temp->data;
    	ret = ipc_send_msg_n2(parp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP,IPC_TYPE_ARP, 0, IPC_OPCODE_DELETE, 0);
        if(ret)
        {
            ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");          
            return;
        }

		hios_hash_delete(&static_arp_table, pbucket_temp);
        XFREE(MTYPE_ARP_STATIC_ENTRY, parp);
        XFREE(MTYPE_HASH_BACKET, pbucket_temp);
       	g_arp.num_static--;
    }
}

/*静态arp 查找*/
struct arp_entry *arp_static_lookup(uint32_t uiIpaddr, uint16_t usVpn)
{
    struct arp_key stKey;
    struct hash_bucket *pstItem = NULL;

    memset(&stKey,0,sizeof(struct arp_key));
    stKey.ipaddr = uiIpaddr;
    stKey.vpnid = usVpn;

    pstItem = hios_hash_find(&static_arp_table, (void *)&stKey);
    if(NULL == pstItem)
    {
        return NULL;
    }
    
    return (struct arp_entry *)pstItem->data;
}


/*del static arp entry by interface*/
void arp_static_delete_by_interface(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct arp_entry *sarp = NULL;
    uint32_t cursor = 0;

    if (ifindex == 0)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, static_arp_table)
    {
       sarp = (struct arp_entry *)(pbucket->data);
       if (sarp && (sarp->ifindex == ifindex))
       {
            arp_static_delete(sarp->key.ipaddr, sarp->key.vpnid);
       }
    }
}

void arp_static_delete_by_slot(uint8_t slot_num)
{
    struct hash_bucket *pbucket = NULL;
    struct arp_entry *sarp = NULL;
    uint32_t cursor = 0;

    if (slot_num > 8)
    {
        return;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, static_arp_table)
    {
       sarp = (struct arp_entry *)(pbucket->data);
       if (sarp && (slot_num == IFM_SLOT_ID_GET(sarp->ifindex)))
       {
            arp_static_delete(sarp->key.ipaddr, sarp->key.vpnid);
       }
    }
}




/*mib 获取arp */
int arp_static_get_bulk(struct arp_key *key,struct arp_entry parp[])
{
    uint32_t num = 0;
    uint32_t msg_len = IPC_MSG_LEN/sizeof(struct arp_entry);
    struct arp_entry *parp_temp = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    uint32_t cursor = 0;
    uint32_t val = 0;

    ARP_LOG_DEBUG("ip:0x%x vpn:%d\n",key->ipaddr,key->vpnid);
  
    if(key->ipaddr == 0 && key->vpnid== 0)/*首次获取*/
    {
        HASH_BUCKET_LOOP(pbucket, cursor, static_arp_table)
        {
            parp_temp = (struct arp_entry *)(pbucket->data);
            if(parp_temp != NULL)
            {
                memcpy(&parp[num++],parp_temp,sizeof(struct arp_entry));

                ARP_LOG_DEBUG("num:%d\n",num);
                if(num == msg_len)
                {                 
                    return num;
                }                                    
            }
        }
    }
    else 
    {
        /*非首次获取则先获取到当前key后的第一个数据*/
        pbucket = hios_hash_find(&static_arp_table,key);
        if(NULL == pbucket)
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= static_arp_table.compute_hash(key);
            if(NULL != static_arp_table.buckets[val])
            {
                pbucket = static_arp_table.buckets[val];
            }
            else
            {
                for(++val; val < HASHTAB_SIZE; ++val)
                {
                    if(NULL != static_arp_table.buckets[val])
                    {
                        pbucket = static_arp_table.buckets[val];
                    }
                }
            }
        }
        
        if(NULL != pbucket)
        {
            for (num = 0; num < msg_len; num++)
            {
                pnext = hios_hash_next_cursor(&static_arp_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    ARP_LOG_DEBUG("pnext or pnext->data is NULL!\n");
                    break;
                }

                parp_temp = (struct arp_entry *)(pnext->data);

                if(parp_temp != NULL)
                    memcpy(&parp[num], parp_temp, sizeof(struct arp_entry));
                
                ARP_LOG_DEBUG("num:%d\n",num);
                pbucket = pnext;
            }
        }
    }

    return num;
    
}



/*for h3c command*/
uint32_t arp_static_get_outif(struct arp_entry *sarp)
{
    struct hash_bucket *pbucket = NULL;
    struct route_if *l3if = NULL;
    uint32_t cursor = 0;
    uint8_t i = 0;

    if (NULL == sarp)
    {
        ARP_LOG_ERROR("Illegal parameter.\n");
        return ERRNO_FAIL;
    }

    HASH_BUCKET_LOOP(pbucket, cursor, route_if_table)
    {
        l3if = (struct route_if *)(pbucket->data);
        if (NULL == l3if)
        {
            continue;
        }

        if (l3if->intf.vpn != sarp->key.vpnid)
        {
            continue;
        }

        /*if intf's status is down,resolve fail*/
        if ((1 == l3if->down_flag) || (1 == l3if->shutdown_flag))
        {
            continue;
        }

        /*get samesubnet outif ifindex*/
        for (i = 0; i < IFM_IP_NUM; i++)
        {
            if (l3if->intf.ipv4[i].addr == 0)
            {
                continue;
            }
            
            if (ipv4_is_same_subnet(l3if->intf.ipv4[i].addr, 
                sarp->key.ipaddr, l3if->intf.ipv4[i].prefixlen))
            {
                sarp->ifindex = l3if->intf.ifindex;
                return ERRNO_SUCCESS;
            }
        }
    }

    return ERRNO_FAIL;
}


void arp_static_route_change_resolve_outif(const struct route_if *pif,uint8_t action)
{
#if 0
    struct hash_bucket *pbucket = NULL;
    struct arp_entry *sarp = NULL;
    uint32_t ip = 0,ip_mask = 0;
    uint32_t cursor = 0;
    int8_t ret = 0;
   
    ARP_LOG_DEBUG();

    if (NULL == pif)
    {
        ARP_LOG_ERROR();
        return;
    }

    if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[0].addr))      // 主 IP 路由
    {
        ip = pif->intf.ipv4[0].addr;
        ip_mask = pif->intf.ipv4[0].prefixlen;
    }
    else if (ROUTE_IF_IP_EXIST(pif->intf.ipv4[1].addr)) // 从 IP 路由
    {
        ip = pif->intf.ipv4[1].addr;
        ip_mask = pif->intf.ipv4[1].prefixlen;
    }


    /*add intf's net route,resolve arp outif and flush to ftm*/
    HASH_BUCKET_LOOP(pbucket, cursor,static_arp_table)
    {
        sarp = (struct arp_entry *)(pbucket->data);
        if (ipv4_is_same_subnet(sarp->key.ipaddr, ip, ip_mask)
                        && (pif->intf.vpn == sarp->key.vpnid))
        {
            if (action == IPC_OPCODE_ADD)
            {
                sarp->ifindex = pif->ifindex;
            }
            else if (action == IPC_OPCODE_DELETE)
            {
                sarp->ifindex = 0;
            }
            
            ret = ipc_send_msg_n2(sarp, sizeof(struct arp_entry), 1, MODULE_ID_FTM,MODULE_ID_ARP, 
												       IPC_TYPE_ARP, 0, action, sarp->ifindex);
            if(ret)
            {
                ARP_LOG_ERROR("ipc_send_msg_n2 error!\n");
            }
        }
    }
#endif
}

int arp_static_intf_arp_conf(uint32_t ifindex, struct ifm_arp buff[])
{
 	struct ifm_arp *parp = NULL;
    struct route_if *pif = NULL;
	struct hash_bucket *pbucket = NULL;
	uint32_t msg_len = IPC_MSG_LEN / sizeof(struct ifm_arp);
	int cursor = 0;
	uint32_t num = 0;
	uint8_t flag = 0;
	
	HASH_BUCKET_LOOP( pbucket, cursor,route_if_table)
	{	
		if(pbucket->data)
		{	
			pif = pbucket->data;
            parp = (struct ifm_arp *)(&(pif->arp));

			if (!parp) continue;
			if (parp->ifindex == 0) continue;
			
			if (ifindex == 0)
			{				
				memcpy(&buff[num++], parp, sizeof(struct ifm_arp));
	
				 if(num == msg_len)
                {                 
                    return num;
                }   
				
			}
			else
			{
				if (parp->ifindex == ifindex)
				{
					flag = 1;
					continue;
									
				}
				if(flag==1)
				{
					memcpy(&buff[num++], parp, sizeof(struct ifm_arp));
					if(num == msg_len)
	                {                 
	                    return num;
	                } 
				}
				
			}
	    }
	}
	return num ;
}





