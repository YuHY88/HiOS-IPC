/***********************************************************************
*
* pool_address.c
*
* manage the dhcp address
*
***********************************************************************/
#include <stdio.h>
#include <string.h>
#include <lib/zassert.h>
#include <lib/hash1.h>
#include <lib/inet_ip.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/thread.h>
#include <lib/ether.h>
#include <lib/index.h>
#include <lib/log.h>
#include <lib/ifm_common.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/errcode.h>

#include "pool.h"
#include "pool_static_address.h"

struct hash_table dhcp_static_addr_table;

static unsigned int compute(void *hash_key)
{
	uint8_t *mac;
	unsigned int key;

    if(NULL == hash_key)
    {
        return 0;
    }
	
	mac =  (uint8_t *)hash_key;
	key = mac[0]+mac[1]+mac[2]+mac[3]+mac[4]+mac[5];
	
    return key;
}

static int compare(void *item, void *hash_key)
{    
    struct hash_bucket *pbucket = item;

    if(NULL == pbucket || NULL == hash_key)
    {
        return 1;
    }

    if(!memcmp(pbucket->hash_key,hash_key,6))
    {
        return 0;
    }
    else
    { 
    	return 1; 
	}
}

void dhcp_static_addr_table_init(unsigned int uiSize)
{
    if(0 == uiSize)
    {
        zlog_err("size must great than or equal to zero!\n");
        return ;
    }   
    hios_hash_init(&dhcp_static_addr_table, uiSize, compute, compare);
    return ;
}

struct dhcpOfferedAddr *dhcp_static_addr_add(uint32_t ip, uint8_t *mac)
{
	struct hash_bucket *addr_item = NULL;
	struct dhcpOfferedAddr *addr_node = NULL;
	int ret;

	if(!ip || !mac)
	{
		return NULL;
	}

	//update
	/*if(addr_item)
	{
		addr_node = addr_item->data;
		memcpy(addr_node->chaddr,mac,6);
		return addr_node;
	}*/

	//new
	addr_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == addr_item)
	{
		return NULL;
	}

	addr_node = (struct dhcpOfferedAddr *)XMALLOC(MTYPE_DHCP_ENTRY, sizeof(struct dhcpOfferedAddr));
	if(NULL == addr_node)
	{
		free(addr_item);
		return NULL;
	}

	memset(addr_item, 0, sizeof(struct hash_bucket));
	memset(addr_node, 0, sizeof(struct dhcpOfferedAddr));

	addr_node->yiaddr = ip;
	memcpy(addr_node->chaddr,mac,6);

	addr_item->data = addr_node;
	addr_item->hash_key = addr_node->chaddr;


	ret = hios_hash_add(&dhcp_static_addr_table, addr_item);
	if(ret != 0)
	{
		XFREE(MTYPE_DHCP_ENTRY, addr_node);
		addr_node = NULL;
		XFREE(MTYPE_HASH_BACKET, addr_item);
		addr_item = NULL;
		return NULL;
	}

	return addr_item->data;		
}

void dhcp_static_addr_del(uint32_t ip)
{
	struct dhcpOfferedAddr *addr_node = NULL;
    struct hash_bucket *pbucket = NULL,*pbucket_next = NULL;
    int cursor;
	struct in_addr inaddr;

	if(!ip )
	{
		return;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	inaddr.s_addr = ip;
		
	HASH_BUCKET_LOOPW(pbucket, cursor, dhcp_static_addr_table)
	{
		addr_node = pbucket->data;
		pbucket_next = pbucket->next;	
		
		if(addr_node->yiaddr == ip)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "del static ip %s success in static address table",inet_ntoa(inaddr));		
		
			hios_hash_delete(&dhcp_static_addr_table, pbucket);
			XFREE(MTYPE_DHCP_ENTRY, addr_node);
			addr_node = NULL;
			XFREE(MTYPE_HASH_BACKET, pbucket);		
			pbucket = NULL;
			return;
		}
		
		pbucket   =	pbucket_next;	
	}
	
}

void dhcp_static_addr_del_by_ifindex(uint32_t ifindex)
{
	struct dhcpOfferedAddr *addr_node = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
	struct in_addr inaddr;


	if(!ifindex)
	{
		return;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
			
	HASH_BUCKET_LOOP(pbucket, cursor, dhcp_static_addr_table)
	{
		addr_node = pbucket->data;
		if(addr_node->ifindex == ifindex)
		{
			addr_node->state   = e_state_nouse;	
			addr_node->ifindex = 0;
			inaddr.s_addr		= addr_node->yiaddr;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "static addr %s change to nouse ok", inet_ntoa(inaddr));				
		}
	}
			
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "leave the function of '%s',--the line of %d",__func__,__LINE__);
	
	return ;	
}

void dhcp_static_addr_del_by_pool(struct ip_pool *pool_node)
{
	struct dhcpOfferedAddr *addr_node = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
	struct hash_bucket *pbucket_next = NULL;

	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
		
	HASH_BUCKET_LOOPW(pbucket, cursor, dhcp_static_addr_table)
	{
		addr_node = pbucket->data;
		pbucket_next =	pbucket->next;	
		if(addr_node->pool_index == pool_node->pool_index)
		{
			hios_hash_delete(&dhcp_static_addr_table, pbucket);
			XFREE(MTYPE_DHCP_ENTRY, addr_node);
			addr_node = NULL;
			XFREE(MTYPE_HASH_BACKET, pbucket);	
			pbucket = NULL;
		}
		pbucket = pbucket_next;
	}
			
	return ;
}

struct dhcpOfferedAddr *  dhcp_static_addr_lookup(uint8_t *mac)
{
    struct hash_bucket *pstItem = NULL;	

    pstItem = hios_hash_find(&dhcp_static_addr_table, mac);

    if (NULL == pstItem)
    {
        return NULL;
    }
    
    return (struct dhcpOfferedAddr *)pstItem->data;
}

int debug_dhcp_ipbind_get_bluk(struct dhcpOfferedAddr *lease)
{
	uint32_t tmp;
	uchar   *mac = NULL;
	struct in_addr inaddr;

	if(!lease)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "The ip pool is not exit!");
		return 1;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Pool-index: %d", lease->pool_index);
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "ifindex: %08X", lease->ifindex);
	tmp = lease->yiaddr; 
	mac = lease->chaddr;
	inaddr.s_addr = tmp;					
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "static-bind ip  %s  mac %02x:%02x:%02x:%02x:%02x:%02x ",inet_ntoa(inaddr),\
		mac[0] ,mac[1] ,mac[2] ,mac[3] ,mac[4] ,mac[5]);
    return 0;
}

//find key next node
struct hash_bucket *dhcp_hash_find_key_next(struct hash_table *tab, void *hash_key)
{
    unsigned int val = tab->compute_hash(hash_key) % HASHTAB_SIZE;
    struct hash_bucket *item = NULL;
    struct hash_bucket *b = NULL;
	int flag = DISABLE;/* 是否查找到匹配选项 */
	
    for((b = tab->buckets[val]); b; b = b->next) 
	{
		item = b;
		if(flag == ENABLE)
		{
			return item;
		}
		if(tab->compare(item, hash_key) == 0) 
		{
			flag = ENABLE;
		}
    }
    return NULL;
}

int dhcp_ipbind_get_bulk(void *pdata, int data_len, struct dhcpOfferedAddr lease[])
{
    struct dhcpOfferedAddr addr_node;
	struct dhcpOfferedAddr * bind_node = NULL;
    struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pstItem = NULL;	
	struct hash_bucket *node = NULL;
	void *tcursor = NULL;
	
    int cursor;	
	uint8_t *mac;
	uchar pool_index = 0;
	int data_num = 0;
	int ret = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct dhcpOfferedAddr);
	
	addr_node = *((struct dhcpOfferedAddr*)pdata);
	pool_index = addr_node.pool_index;

	mac = (uint8_t *)XCALLOC(MTYPE_IF, 16);
	if(NULL == mac) 
	{
		DHCP_LOG_ERROR("Error: There is no ram space\n");		  
	
		return 0;
	}
	memcpy(mac, addr_node.chaddr, 16);
	if (!dhcp_static_addr_table.num_entries)
	{
		return data_num;
	}
	if(pool_index == 0)
	{
		for(ret = pool_index + 1; ret < 256; ret++)
		{
			HASH_BUCKET_LOOP(pbucket,cursor,dhcp_static_addr_table)
			{
				bind_node = pbucket->data;
				if(bind_node && bind_node->pool_index == ret)
				{
					memcpy(&lease[data_num], bind_node, sizeof(struct dhcpOfferedAddr));
					debug_dhcp_ipbind_get_bluk(&lease[data_num]);
					data_num++;
					if (data_num == msg_num)
					{
						return data_num;
					}	
				}
			}
		}
	} /* end if pool_index == 0 */
	else 
	{
		//find key next node
		pstItem = dhcp_hash_find_key_next(&dhcp_static_addr_table, mac);

		if (NULL == pstItem)
		{
			return 0;
		}
		
		for (ret = pool_index + 1; ret < 256; ret++)
		{	
			//after check first pool, from next pool first node start check
			if (ret > pool_index + 1)
			{
				pstItem = hios_hash_start(&dhcp_static_addr_table, &tcursor);
			}

			//check first pool, from next mac check
			for((node) = pstItem; node; (node) = hios_hash_next(&dhcp_static_addr_table, &tcursor))		
			{
				bind_node = (struct dhcpOfferedAddr *)(node->data);
				if(bind_node == NULL)
				{
					return data_num;
				}

				if(bind_node && bind_node->pool_index == ret)
				{
					memcpy(&lease[data_num], bind_node, sizeof(struct dhcpOfferedAddr));
					debug_dhcp_ipbind_get_bluk(&lease[data_num]);
					data_num++;
					if(data_num == msg_num)
					{
						return data_num;
					}	
				}
			}
		}/* end for ret = pool_index + 1; ret < 256; ret++ */
	}
	return data_num;
}

int dhcp_com_ipbind_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int ret = 0;
    int msg_len = 0;

    msg_len = IPC_MSG_LEN/sizeof(struct dhcpOfferedAddr);
    struct dhcpOfferedAddr lease[msg_len];
    memset(lease, 0, msg_len*sizeof(struct dhcpOfferedAddr));
    ret = dhcp_ipbind_get_bulk(pdata, pmsghdr->data_len, lease);

    if (ret > 0)
    {
        ret = ipc_send_reply_n2(lease, ret*sizeof(struct dhcpOfferedAddr), ret, MODULE_ID_SNMPD, 
                                    MODULE_ID_DHCP,IPC_TYPE_L3IF, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_REPLY);
    }
    else
    {
        ret = ipc_send_reply_n2(NULL, 0, 0, MODULE_ID_SNMPD, MODULE_ID_DHCP, IPC_TYPE_L3IF,
                                pmsghdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);
    }
    return ret;  
}
