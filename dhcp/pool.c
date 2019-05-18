#include <string.h>
#include <stdlib.h>
#include <lib/hash1.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/thread.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/msg_ipc.h>
#include <lib/hash1.h>
#include "pool.h"
#include "dhcp_if.h"
#include "files.h"
#include "pool_address.h"
#include "pool_static_address.h"

struct hash_table ip_pool_table;

/**
 * @param   void *hash_key
 * @return  hash_value, 0 means failure 
 */
static unsigned int compute(void *hash_key)
{
	return (uint32_t)hash_key;
}


/**
 * @param  		void *item
 * @param		void *hash_key
 * @return		0, 1 means failure 
 */
static int compare(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = (struct hash_bucket *)item;

	assert(NULL != item);

	if (pbucket->hash_key == hash_key)
	{
		return ERRNO_SUCCESS;
	}

	return ERRNO_FAIL;
}

/**
 * @param  unsigned int size   POOL_TAB_MSIZE
 */
void dhcp_pool_table_init(unsigned int size)
{
	hios_hash_init(&ip_pool_table, size, compute, compare);
}


/**
 * @param		uchar *pool_name
 * @return		pool_item->hash_key,NULL means failure 
 */
struct ip_pool * dhcp_pool_add(uchar pool_index)
{
	struct hash_bucket *pool_item = NULL;
	struct ip_pool *pool_node = NULL;
	int ret;

	pool_item = hios_hash_find(&ip_pool_table, (void *)(int)pool_index);
	if(NULL != pool_item)
	{
		return pool_item->data;
	}

	pool_item = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pool_item)
	{
		return NULL;
	}

	pool_node = (struct ip_pool *)XMALLOC(MTYPE_POOL_ENTRY, sizeof(struct ip_pool));
	if(NULL == pool_node)
	{
		free(pool_item);
		pool_item = NULL;		
		return NULL;
	}

	memset(pool_item, 0, sizeof(struct hash_bucket));
	memset(pool_node, 0, sizeof(struct ip_pool));

	pool_node->pool_index 			 =  pool_index;
	pool_node->dhcp_pool.leasetime   =  24*60*60 ;  //   default val  1day

	pool_item->data = pool_node;
	pool_item->hash_key =(void *)(int)pool_node->pool_index;

	ret = hios_hash_add(&ip_pool_table, pool_item);
	if(ret != 0)
	{
		XFREE(MTYPE_POOL_ENTRY, pool_node);
		pool_node = NULL;
		XFREE(MTYPE_HASH_BACKET, pool_item);
		pool_item = NULL;
		return NULL;
	}

	return pool_item->data;
}

/**
 * @param 		uchar *pool_name
 * @return		0, 1 means failure 
 */
int dhcp_pool_delete(uchar pool_index)
{
	struct hash_bucket *pool_item = NULL;	
	struct ip_pool *pool_node = NULL;
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	

	pool_item = hios_hash_find(&ip_pool_table, (void *)(int)pool_index);
	if(pool_item == NULL)
	{
		return 1;
	}
	
	pool_node = pool_item->data;

	dhcp_addr_del_by_pool(pool_node);	
	dhcp_static_addr_del_by_pool(pool_node);

	if(pool_node ->dhcp_pool.indexd)
	{
		free(pool_node ->dhcp_pool.indexd);
		pool_node ->dhcp_pool.indexd = NULL;
	}
	
	if(pool_node ->dhcp_pool.indexs)
	{
		free(pool_node ->dhcp_pool.indexs);
		pool_node ->dhcp_pool.indexs = NULL;
	}
	
	
	hios_hash_delete(&ip_pool_table, pool_item);

	XFREE(MTYPE_POOL_ENTRY,pool_node);	
	pool_item->data = NULL; 
	pool_node = NULL;
	XFREE(MTYPE_HASH_BACKET, pool_item);
	pool_item = NULL;

	dhcp_write_leases(NULL);		

	return 0;
}

/**
 * @param		uchar *pool_name
 * @return		struct ip_pool * ,NULL means failure
 */
struct ip_pool *dhcp_pool_lookup(uchar pool_index)
{

	struct hash_bucket *pool_item = NULL;

	pool_item = hios_hash_find(&ip_pool_table, (void *)(int)pool_index);
	if(pool_item == NULL)
	{
		return NULL;
	}

	return ((struct ip_pool *)(pool_item->data));
}

int debug_dhcp_ippool_get_bulk(struct ip_pool *pool_node)
{
	uint32_t tmp;
	struct in_addr inaddr;
	if(!pool_node)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "The ip pool is not exit!");
		return 1;
	}

  	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Pool-index: %d", pool_node->pool_index);
	
	tmp = pool_node->dhcp_pool.gateway;
	inaddr.s_addr		= tmp;	
	if(tmp)
	{
  		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Gateway: %s/%d",inet_ntoa(inaddr),pool_node->dhcp_pool.mask_len);
	}
	else
	{
  		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Gateway: %s", "--");
	}

	tmp = pool_node->dhcp_pool.start;
	inaddr.s_addr		= tmp;	
  	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Start-address: %s", tmp?inet_ntoa(inaddr):"--");
	
	tmp = pool_node->dhcp_pool.end;
	inaddr.s_addr		= tmp;		
  	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "End-address: %s", tmp?inet_ntoa(inaddr):"--");

	tmp = pool_node->dhcp_pool.leasetime;
	if(tmp)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Lease: %d minutes", tmp/60);			
	}
	else
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Lease: %s", "Unlimited");	
	}	
	tmp = pool_node->dhcp_pool.dns_server;	
	inaddr.s_addr		= tmp;	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "DNS-server: %s", tmp?inet_ntoa(inaddr):"--");
	
	//total num
	tmp = pool_node->dhcp_pool.total;
  	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Total-num: %d", tmp);

	return 0;
}

int dhcp_ippool_get_bulk(void *pdata, int data_len, struct ip_pool ip_pool[])
{
	int data_num = 0;
	struct ip_pool *pool_node = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;	
	int flag = DISABLE;/* 是否查找到传入的pool_index */
	uchar pool_index = *((uchar *)pdata);
	int msg_num  = IPC_MSG_LEN/sizeof(struct ip_pool);
	if(pool_index == 0)
	{
		HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
		{
			pool_node = pbucket->data;
	    	if(pool_node)
	    	{
				memcpy(&ip_pool[data_num], pool_node, sizeof(struct ip_pool));

				debug_dhcp_ippool_get_bulk(&ip_pool[data_num]);
				data_num++;
				if(data_num == msg_num)
				{
					return data_num;
				}	  
    		}
		}		
	} 
	else 
	{
		HASH_BUCKET_LOOP(pbucket,cursor,ip_pool_table)
    	{
			pool_node = pbucket->data;
			if(pool_node) 
			{
				if(flag == DISABLE)
				{
					if(pool_node->pool_index == pool_index)
					{
						flag = ENABLE;
					}
					continue;
				}
				else
				{
					memcpy(&ip_pool[data_num], pool_node, sizeof(struct ip_pool));
					debug_dhcp_ippool_get_bulk(&ip_pool[data_num]);
					data_num++;
					if(data_num == msg_num)
					{
						flag = DISABLE;
						return data_num;
					}	
				}
			}
		}
		flag = DISABLE;
	}
	return data_num;
}

int dhcp_com_ippool_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
    int ret = 0;
    int msg_len = 0;

    msg_len = IPC_MSG_LEN/sizeof(struct ip_pool);
    struct ip_pool ip_pool[msg_len];
    memset(ip_pool, 0, msg_len*sizeof(struct ip_pool));
    ret = dhcp_ippool_get_bulk(pdata, pmsghdr->data_len, ip_pool);
    if(ret > 0)
    {
        ret = ipc_send_reply_n2(ip_pool, ret*sizeof(struct ip_pool), ret, MODULE_ID_SNMPD, 
                                    MODULE_ID_DHCP,IPC_TYPE_L3IF, pmsghdr->msg_subtype, 0, pmsghdr->msg_index, IPC_OPCODE_REPLY);
    }
    else
    {
        ret = ipc_send_reply_n2(NULL, 0, 0, MODULE_ID_SNMPD, MODULE_ID_DHCP, IPC_TYPE_L3IF,
                                pmsghdr->msg_subtype, 0, 0, IPC_OPCODE_NACK);
    }
    return ret;  
}
