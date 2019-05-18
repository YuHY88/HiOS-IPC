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

#include "pool_address.h"
#include "pool.h"
#include "dhcp_if.h"
#include "pool_static_address.h"
extern struct if_dhcp *pif_server;
struct hash_table dhcp_addr_table;
extern struct ip_pool *p_pool;

static unsigned int compute(void *hash_key)
{
    if(NULL == hash_key)
    {
        return 0;
    }
    return ((unsigned int)hash_key);
}

static int compare(void *item, void *hash_key)
{    
    struct hash_bucket *pbucket = item;

    if(NULL == pbucket || NULL == hash_key)
    {
        return 1;
    }

    if (pbucket->hash_key == hash_key)
    {
        return 0;
    }
    else
    { 
    	return 1; 
	}
}

void dhcp_addr_table_init(unsigned int uiSize)
{
    if(0 == uiSize)
    {
        zlog_err("size must great than or equal to zero!\n");
        return ;
    }   
    hios_hash_init(&dhcp_addr_table, uiSize, compute, compare);
    return ;
}

struct dhcpOfferedAddr *dhcp_addr_add(uint32_t ip, uint8_t *mac)
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
	addr_item->hash_key = (void *)addr_node->yiaddr;

	ret = hios_hash_add(&dhcp_addr_table, addr_item);
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

struct dhcpOfferedAddr *  dhcp_addr_add_by_node(struct dhcpOfferedAddr * addrnode)
{
	struct hash_bucket *addr_item = NULL;
	struct dhcpOfferedAddr *addr_node = NULL;
	int ret;
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

	if(!addrnode)
	{
		return NULL;
	}
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
	memcpy(addr_node,addrnode,sizeof(struct dhcpOfferedAddr));

	addr_item->data = addr_node;
	addr_item->hash_key = (void *)addr_node->yiaddr;

	ret = hios_hash_add(&dhcp_addr_table, addr_item);
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

void dhcp_addr_del(uint32_t ip, struct ip_pool *pool_node)
{
	struct hash_bucket *addr_item = NULL;
	struct dhcpOfferedAddr *addr_node = NULL;

	uint32_t start;
	uint16_t pos,m,n;
	uint8_t  *indexd;
	struct in_addr addr;

	if(!ip)
	{
		return;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

    addr_item = hios_hash_find(&dhcp_addr_table, (void *)ip);
	
	if(addr_item)
	{
		hios_hash_delete(&dhcp_addr_table, addr_item);

		addr_node = addr_item->data;
		
		XFREE(MTYPE_DHCP_ENTRY, addr_node);
		addr_node = NULL;
		XFREE(MTYPE_HASH_BACKET, addr_item);
		addr_item = NULL;
	}	

	if(pool_node)
	{
		indexd = pool_node->dhcp_pool.indexd;	
		
		if(indexd == NULL)
		{
			zlog_err("syn flag of addr,indexs or indexd is  NULL,return");
			return ;	
		}
		
		start =  ntohl(pool_node->dhcp_pool.start); 
		//end   =  ntohl(pool_node->dhcp_pool.end);

		pos = ntohl(ip)-start;
		m = pos/8;
		n = pos%8;

		indexd[m] |= 0x01<<n;
		
		addr.s_addr 	= ip;
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "syn flag of addr %s ,del",inet_ntoa(addr)); 		
	}
	
	return ;
}

void dhcp_addr_del_by_pool(struct ip_pool *pool_node)
{
	uint32_t addr, addr_net,start,end;
	uint16_t pos,m,n;
	uint8_t  *indexd;
	struct in_addr inaddr;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

	indexd	= pool_node->dhcp_pool.indexd; 	

	if(indexd == NULL)
	{
		zlog_err("indexs or indexd is  NULL");
		return ;	
	}

	start =  ntohl(pool_node->dhcp_pool.start); 
	end =  ntohl(pool_node->dhcp_pool.end);

	for(addr=start; addr <= end; addr++) 
	{
		pos = addr-start;
		m = pos/8;
		n = pos%8;
		
		if((indexd[m] & (0x01<<n)) == 0)//not allocated 
		{
			addr_net = htonl(addr);		
			dhcp_addr_del(addr_net,NULL);		
			inaddr.s_addr 	= addr_net;			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "dynamic addr %s ,del",inet_ntoa(inaddr)); 
		}	
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "leave the function of '%s',--the line of %d",__func__,__LINE__);

	return ;
}

void dhcp_addr_del_by_ifindex(uint32_t ifindex)
{
	struct dhcpOfferedAddr *addr_node = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    struct hash_bucket *pbucket_next = NULL;

	struct ip_pool *pool_node;
	uint16_t pos,m,n;
	struct in_addr addr;

	if(!ifindex)
	{
		return;
	}
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);

	HASH_BUCKET_LOOPW ( pbucket, cursor, dhcp_addr_table )
	{
		addr_node = pbucket->data;
		pbucket_next =	pbucket->next;	
		if(addr_node->ifindex== ifindex)
		{
			pool_node	 = dhcp_pool_lookup(addr_node->pool_index);
			pos = ntohl(addr_node->yiaddr)-ntohl(pool_node->dhcp_pool.start);
			m = pos/8;
			n = pos%8;					
			pool_node->dhcp_pool.indexd[m] |= 0x01<<n;

			
			addr.s_addr 	= addr_node->yiaddr;
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "del dynamic,  %s ok",inet_ntoa(addr));		
		
			hios_hash_delete(&dhcp_addr_table, pbucket);
			XFREE(MTYPE_DHCP_ENTRY, addr_node);
			addr_node = NULL;
			XFREE(MTYPE_HASH_BACKET, pbucket);
			pbucket = NULL;
		}
		
		pbucket	= pbucket_next;
	}

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "leave the function of '%s',--the line of %d",__func__,__LINE__);
	
	return ;
}

struct dhcpOfferedAddr *dhcp_addr_lookup(uint32_t ip)
{
    struct hash_bucket *pstItem = NULL;

	if(!ip)
	{
		return NULL;
	}
    pstItem = hios_hash_find(&dhcp_addr_table, (void *)ip);

    if (NULL == pstItem)
    {
        return NULL;
    }
    
    return (struct dhcpOfferedAddr *)pstItem->data;
}

struct dhcpOfferedAddr *dhcp_addr_lookup_by_mac(uint8_t *mac,struct ip_pool *pool_node)
{
	struct dhcpOfferedAddr *addr_node = NULL;
	uint8_t mac_zero[6];

	uint32_t addr, addr_net,start,end;
	uint16_t pos,m,n;
	uint8_t  *indexd;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'--the line of %d",__func__,__LINE__);
	memset(mac_zero,0,6);
	
	if(!mac || !memcmp(mac, mac_zero, 6))
	{
		return NULL;
	}
	indexd	= pool_node->dhcp_pool.indexd;	
	
	if(indexd == NULL)
	{
		zlog_err("indexd is  NULL");
		return NULL;	
	}
	
	start = ntohl(pool_node->dhcp_pool.start); 
	end = ntohl(pool_node->dhcp_pool.end);

	for(addr=start; addr <= end; addr++) 
	{
		pos = addr-start;
		m = pos/8;
		n = pos%8;	
		
		if((indexd[m] & (0x01<<n)) == 0)//not allocated 
		{
			addr_net = htonl(addr);		
			addr_node = dhcp_addr_lookup(addr_net);	
			if(addr_node && !memcmp(addr_node->chaddr,mac,6))
			{
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "find mac  ok , in dynamic table"); 	
				return addr_node;
			}
		}	
	}
	
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "leave the function of '%s',--the line of %d",__func__,__LINE__);
	
	return NULL;
}

/* true if a lease has expired */
int lease_expired(struct dhcpOfferedAddr *lease)
{
	return (lease->expires < (unsigned long) time(0));
}
#if 0
/* 向路由模块发送获取所有接口的ip地址的请求 */
struct ifm_l3 *dhcp_route_get_interface_ip_bulk(uint32_t ifindex,int module_id, int *pdata_num)
{
	struct ipc_mesg *pmesg = ipc_send_common_wait_reply1 ( NULL, 0, 1 , MODULE_ID_ROUTE, module_id,
                             IPC_TYPE_L3IF, 0, IPC_OPCODE_GET_BULK, ifindex );
    if(NULL != pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return ( struct ifm_l3 * ) pmesg->msg_data;
    }
    return NULL;
}
#else
struct ifm_l3 *dhcp_route_get_interface_ip_bulk(uint32_t ifindex,int module_id, int *pdata_num)
{
	
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(NULL, sizeof(struct ifm_l3),
                                                          1, MODULE_ID_ROUTE, module_id,
                                                          IPC_TYPE_L3IF, 0,
                                                          IPC_OPCODE_GET_BULK, 0, ifindex);
    if (NULL != pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (struct ifm_l3 *)pmesg->msg_data;
	}
	return NULL;
}
#endif
/* 检查接口地址是否在地址池范围内，或者检查接口地址是否和传入ip一致，返回冲突的数量 */
int check_interface_ip_address(uint32_t start, uint32_t end, uint32_t gateway, uint32_t ip)
{
	int ret = 0;
	int confilct_count = 0;
	int data_num = 0;
	struct ifm_l3 *l3 = NULL;
	
	l3 = dhcp_route_get_interface_ip_bulk(0, MODULE_ID_DHCP, &data_num);
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> ip pool start ip is 0x%x, end ip 0x%x, ip is 0x%x\n", __func__, start, end, ip);
	if (data_num == 0)
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> no route if table data return, read over\n", __func__);
	} 
	else 
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> success to get route if table data[num = %d] \n", __func__, data_num);
		for (ret = 0; ret < data_num; ret++)
		{
			if (start != 0 && end != 0)
			{
				if (l3->ipv4[0].addr >= start && l3->ipv4[0].addr <= end)
				{
					if (l3->ipv4[0].addr != gateway)
					{
						confilct_count++;
					}
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> there are confilct interface ip address is 0X%x between start 0x%x and end 0x%x, num is %d\n", __func__, l3->ipv4[0].addr, start, end, confilct_count);
				}
			} 
			else if (ip != 0)
			{
				if (l3->ipv4[0].addr == ip)
				{
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> there are confilct interface ip address is 0X%x, return 1\n",__func__, l3->ipv4[0].addr);
					return 1;
				}
			} 
			else
			{
				DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "in function '%s' -> start ip and end ip are null, ip is null \n", __func__);
			}
			l3++;
		}
	}
	return confilct_count;
}

/* find an assignable address, it check_expired is true, we check all the expired leases as well.
 * Maybe this should try expired leases by age... */
uint32_t dhcp_find_address(int check_expired,int check_if)
{
	uint32_t addr, ret,start,end, dns;
	
	struct dhcpOfferedAddr * offeredaddr = NULL;

	uint16_t pos,m,n;
	uint8_t  *indexs, *indexd;
	struct in_addr inaddr;
	struct in_addr netmask;
	uint8_t netmasklen;

	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s',check_expired: %d",__func__,check_expired);

	indexs	= p_pool->dhcp_pool.indexs; 
	indexd	= p_pool->dhcp_pool.indexd; 	
	
	if(indexs == NULL || indexd == NULL )
	{
		zlog_err("indexs or indexd is  NULL");
		return 0;	
	}

	start =  ntohl(p_pool->dhcp_pool.start); 
	end   =  ntohl(p_pool->dhcp_pool.end);
	netmask.s_addr = pif_server->mask;
	netmasklen = ip_masklen(netmask);
	dns = ntohl(p_pool->dhcp_pool.dns_server);
	
	for (addr=start; addr <= end; addr++) 
	{
		DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "Entering the function of '%s'",__func__);
		/* ie, 192.168.55.0 */
		//if (!(addr & 0xFF)) continue;

		/* ie, 192.168.55.255 */
		//if ((addr & 0xFF) == 0xFF) continue;

		ret = htonl(addr);

		inaddr.s_addr		= ret;

		//check interface ip
		if(dhcp_if_lookup_by_ip(ret,0))
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "interface  addr %s ,continue",inet_ntoa(inaddr));			
		
			continue;
		}

		//check gateway
		if(ret == p_pool->dhcp_pool.gateway)
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "gateway  addr %s ,continue",inet_ntoa(inaddr));			
			continue; 
		}

		if(check_interface_ip_address(0, 0, 0, ret))
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "There is a interface ip address is %s ,continue",inet_ntoa(inaddr));			
			continue; 
		}
		if(check_if && (!ipv4_is_same_subnet(ntohl(pif_server->ip_addr),ntohl(ret),netmasklen)))
		{
			//DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "addr %s not belong to server ip ,pif netmasklen:%d ,continue",inet_ntoa(inaddr),netmasklen);			
			continue; 
		}
		if(dns == addr)
		{
			//DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "addr %s not belong to server ip ,pif netmasklen:%d ,continue",inet_ntoa(inaddr),netmasklen);			
			continue; 
		}

		if((ret&(~pif_server->mask)) == 0 || ((ret&(~pif_server->mask))==(0xffffffff&(~pif_server->mask))))
		{
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "addr %s  belong to server ip ,pif netmasklen:%d ,but host ip is illegal ,continue",inet_ntoa(inaddr),netmasklen);			
			continue; 
		}

		pos = addr-start;
		m = pos/8;
		n = pos%8;

		
		//check static
		if((indexs[m] & (0x01<<n)) == 0)//static addr
		{			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "static addr %s ,continue",inet_ntoa(inaddr));			
			continue;
		}

		//check allocated dynamic addr
		if(indexd[m] & (0x01<<n))//not allocated 
		{
			indexd[m] &= ~(0x01<<n) ; //reverse  flag
			
			DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "dhcp_find_address() , find free addr,%s",inet_ntoa(inaddr));			
			return ret;
		}

		//check expired
		if(check_expired)
		{
			offeredaddr = dhcp_addr_lookup(ret);
			if(!offeredaddr)
			{
				zlog_err("dhcp_find_address() ,offeredaddr should not be NULL");
				continue;
			}
			else
			{
				if(lease_expired(offeredaddr))
				{
					DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "dhcp_find_address() , find expired addr,%s",inet_ntoa(inaddr));				
					return ret;
				}
			}

		}

	}

	return 0;
}

int debug_dhcp_dynamic_ipbind_get_bluk(struct dhcpOfferedAddr *lease)
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
	DHCP_LOG_DEBUG(DHCP_DEBUG_TYPE_SERVER, "dynamic-bind ip  %s  mac %02x:%02x:%02x:%02x:%02x:%02x ",inet_ntoa(inaddr),\
		mac[0] ,mac[1] ,mac[2] ,mac[3] ,mac[4] ,mac[5]);
    return 0;
}

int dhcp_dynamic_ipbind_get_bulk(void *pdata, int data_len, struct dhcpOfferedAddr lease[])
{
    struct dhcpOfferedAddr addr_node;
	struct dhcpOfferedAddr * bind_node = NULL;
    struct hash_bucket *pbucket = NULL;
	struct hash_bucket *pstItem = NULL;	
	struct hash_bucket *node = NULL;
	void *tcursor = NULL;
    int cursor;	
	uint32_t bind_ip = 0;
	uchar pool_index = 0;
	int data_num = 0;
	int ret = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct dhcpOfferedAddr);
	addr_node = *((struct dhcpOfferedAddr*)pdata);
	pool_index = addr_node.pool_index;
	bind_ip = addr_node.yiaddr;
	if(!dhcp_addr_table.num_entries){
		return data_num;
	}
	if(pool_index == 0)
	{
		for(ret = pool_index + 1; ret < 256; ret++)
		{
			HASH_BUCKET_LOOP(pbucket, cursor, dhcp_addr_table)
			{
				bind_node = pbucket->data;
				if(bind_node && bind_node->pool_index == ret)
				{
					memcpy(&lease[data_num], bind_node, sizeof(struct dhcpOfferedAddr));
					debug_dhcp_dynamic_ipbind_get_bluk(&lease[data_num]);
					data_num++;
					if(data_num == msg_num)
					{
						return data_num;
					}	
				}
			}
		}
	} 
	else 
	{		
		for (ret = pool_index + 1; ret < 256; ret++)
		{	
			//after check first pool, from next pool first node start check
			if (ret == pool_index + 1)
			{
				//find key next node
				pstItem = dhcp_hash_find_key_next(&dhcp_addr_table, (void *)bind_ip);
				if (NULL == pstItem)
				{
					continue;
				}
				//check first pool, from next ip check
				for((node) = pstItem; node; (node) = hios_hash_next(&dhcp_addr_table, &tcursor))		
				{
					bind_node = (struct dhcpOfferedAddr *)(node->data);
					if(bind_node == NULL)
					{
						return data_num;
					}
					if(bind_node && bind_node->pool_index == ret)
					{
						memcpy(&lease[data_num], bind_node, sizeof(struct dhcpOfferedAddr));
						debug_dhcp_dynamic_ipbind_get_bluk(&lease[data_num]);
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
				//after check first pool, from next pool first node start check			
				HASH_BUCKET_LOOP(pbucket, cursor, dhcp_addr_table)
				{
					bind_node = pbucket->data;
					if(bind_node == NULL)
					{
						return data_num;
					}
					if(bind_node && bind_node->pool_index == ret)
					{
						memcpy(&lease[data_num], bind_node, sizeof(struct dhcpOfferedAddr));
						debug_dhcp_dynamic_ipbind_get_bluk(&lease[data_num]);
						data_num++;
						if(data_num == msg_num)
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

int dhcp_com_dynamic_ipbind_get_bulk(void *pdata, struct ipc_msghdr_n *pmsghdr)
{
	int ret = 0;
    int msg_len = 0;

    msg_len = IPC_MSG_LEN/sizeof(struct dhcpOfferedAddr);
    struct dhcpOfferedAddr lease[msg_len];
    memset(lease, 0, msg_len*sizeof(struct dhcpOfferedAddr));
    ret = dhcp_dynamic_ipbind_get_bulk(pdata, pmsghdr->data_len, lease);

	if(ret > 0)
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

