/*
*  ndp_static.c  mange static ipv6 nd table
*
*/

#include <lib/memtypes.h>
#include <lib/memory.h>
#include <lib/errcode.h>
#include <lib/msg_ipc.h>
#include <lib/log.h>
#include <lib/types.h>
#include <lib/hash1.h>

#include "route_main.h"
#include "ndp_cmd.h"
#include "ndp_static.h"


/*静态nd hash 表*/
struct hash_table static_ndp_table;


/*nd hash key 计算*/
static uint32_t ndp_hash(void * ndp)
{
    struct ndp_key *pndp = ndp; 
    uint32_t key = 0;
    uint8_t  i = 0;

    for(i = 0; i< 16; i++)
    {
        key += pndp->ipv6_addr[i];
    }

    key += pndp->vpnid;
    key += pndp->ifindex;

    return key;
}

/*nd hash key 比较*/
static int ndp_hash_compare(void *item,void *ndp)
{
    struct ndp_key *pndp = ndp;
    struct ndp_key *pndp_cmp = NULL;
    
    pndp_cmp = ((struct hash_bucket *)item)->hash_key;

    if(!pndp || !pndp_cmp)
        return ERRNO_FAIL;

    if(IPV6_ADDR_SAME(pndp->ipv6_addr, pndp_cmp->ipv6_addr) && 
      (pndp->vpnid == pndp_cmp->vpnid) && (pndp->ifindex == pndp_cmp->ifindex))
    {
        return ERRNO_SUCCESS;
    }

	return ERRNO_FAIL;
}


/*hash 表初始化*/
void ndp_static_table_init(uint32_t size)
{
    hios_hash_init(&static_ndp_table, size, ndp_hash, ndp_hash_compare);
}


/* 添加静态 nd 表项  */
int ndp_static_add(struct ndp_neighbor *pnd_entry)
{
    struct hash_bucket  *item = NULL;
    struct ndp_neighbor *pndp = NULL;
    int ret = 0;
    
    if(!pnd_entry)
        return ERRNO_PARAM_ILLEGAL;
    
    ROUTE_NDP_ZLOG_DEBUG();
    
    if(gndp.num_static >= gndp.num_limit)
    {
        ROUTE_NDP_ZLOG_ERR("static ndp entry num over limit!\n");
        return ERRNO_OVERSIZE;
    }
    
    item = hios_hash_find(&static_ndp_table, &(pnd_entry->key));
    if(item)
    {
        pndp = (struct ndp_neighbor *)(item->data);
        memcpy(pndp, pnd_entry, sizeof(struct ndp_neighbor));
        ret = ipc_send_msg_n2(pndp, sizeof(struct ndp_neighbor), 1 , MODULE_ID_FTM,  MODULE_ID_ROUTE, 
                            IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_UPDATE, pndp->key.ifindex);
    }
    else
    {
        ROUTE_NDP_ZLOG_DEBUG();
        pndp = XCALLOC(MTYPE_NDP_STATIC_ENTRY, sizeof(struct ndp_neighbor));
        item = XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        memcpy(pndp, pnd_entry, sizeof(struct ndp_neighbor));

        item->data = pndp;
        item->hash_key = &(pndp->key);
        ret = hios_hash_add(&static_ndp_table, item);
        if(ret < 0)
        {	
        	XFREE(MTYPE_NDP_STATIC_ENTRY, pndp);
            XFREE(MTYPE_HASH_BACKET, item);
            return ERRNO_FAIL;
        }
        else
        {
            gndp.num_static ++;
            ret = ipc_send_msg_n2(pndp, sizeof(struct ndp_neighbor), 1 , MODULE_ID_FTM,  MODULE_ID_ROUTE, 
                                    IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_ADD,pndp->key.ifindex);
        }
    }

    if(ret < 0)
    {
        ROUTE_NDP_ZLOG_ERR("ipc_send_ftm error!\n");
        return ERRNO_IPC;
    }
    
    return ERRNO_SUCCESS;
}


/* 删除静态 nd 表项  */
int ndp_static_delete(struct ipv6_addr *pipaddr, uint32_t vpn, uint32_t ifindex)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket  *item = NULL;
    struct ndp_key hash_key;
    int ret = 0;

    if(!pipaddr || (vpn > 1024))
    {
        ROUTE_NDP_ZLOG_ERR("argument invalid!\n");
        return ERRNO_FAIL;
    }

    /*set hash key*/
    memset(&hash_key, 0, sizeof(struct ndp_key));
    memcpy(&hash_key.ipv6_addr, pipaddr, 16);
    hash_key.vpnid = vpn;
    hash_key.ifindex = ifindex;
    
    item = hios_hash_find(&static_ndp_table, &hash_key);
    if(item)
    {
        pndp = item->data;
        ret = ipc_send_msg_n2(pndp, sizeof(struct ndp_neighbor), 1, MODULE_ID_FTM,
               MODULE_ID_ROUTE, IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_DELETE, pndp->key.ifindex);
        if(ret < 0)
        {
            ROUTE_NDP_ZLOG_ERR("ipc_send_ftm error!\n");
            return ERRNO_FAIL;
        }

        gndp.num_static --;
        hios_hash_delete(&static_ndp_table, item);
        XFREE(MTYPE_NDP_STATIC_ENTRY, pndp);
        XFREE(MTYPE_HASH_BACKET, item);

        return ERRNO_SUCCESS;        
    }
    else
    {
        ROUTE_NDP_ZLOG_ERR();
        return ERRNO_FAIL;
    }
 
}

/*delete static ndp entry by ip or vpn or ip&vpn*/
void ndp_static_delete_by_ip_vpn(struct ipv6_addr *ip6addr, int vpn)
{
    struct hash_bucket  *bucket = NULL;
    struct hash_bucket  *bucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    int cursor = 0;
    int ret = 0;

    ROUTE_NDP_ZLOG_DEBUG("ip6addr:%p vpn:%d\n", ip6addr, vpn);
    
    HASH_BUCKET_LOOPW(bucket,cursor,static_ndp_table)
    {
        if(NULL == bucket->data)
    		continue;
       
        bucket_temp = bucket;
        bucket = bucket_temp->next;
		pndp = (struct ndp_neighbor *)bucket_temp->data; 
        
        /*if ipv6addr and vpn is exist,delete by ip and vpn*/
        if (ip6addr && (vpn >= 0))
        {
            if (IPV6_ADDR_SAME(ip6addr, pndp->key.ipv6_addr) 
                && ((uint32_t)vpn == pndp->key.vpnid))
            {
                goto delete;         
            }
        }
        else if (!ip6addr && vpn)
        {
            if ((uint32_t)vpn == pndp->key.vpnid)
            {
                goto delete;
            }
        }
        else if (ip6addr && (vpn < 0))
        {
            if (IPV6_ADDR_SAME(ip6addr, pndp->key.ipv6_addr))
            {
                goto delete;
            }
        }
        else
        {
            continue;        
        }

delete:
        ret = ipc_send_msg_n2(pndp, sizeof(struct ndp_neighbor), 1, MODULE_ID_FTM,  MODULE_ID_ROUTE,IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_ADD, pndp->key.ifindex);
        if(ret < 0)
        {
            ROUTE_NDP_ZLOG_ERR("ipc_send_ftm error!\n");          
            return;
        }
		
        hios_hash_delete(&static_ndp_table, bucket_temp);
        XFREE(MTYPE_NDP_STATIC_ENTRY, pndp);
        XFREE(MTYPE_HASH_BACKET, bucket_temp);
        
       	gndp.num_static--;

    }
}

/* 查找静态 nd 表项 */
struct ndp_neighbor *ndp_static_lookup(struct ipv6_addr *pipaddr, uint32_t vpn, uint32_t ifindex)
{
    struct hash_bucket  *item = NULL;
    struct ndp_key hash_key;

    if(!pipaddr || vpn > 1024)
    {
        ROUTE_NDP_ZLOG_ERR("argument invalid!\n");
        return NULL;
    }

    /*set hash key*/
    memset(&hash_key, 0, sizeof(struct ndp_key));
    memcpy(&hash_key.ipv6_addr, pipaddr, 16);
    hash_key.vpnid = vpn;
    hash_key.ifindex = ifindex;

    item = hios_hash_find(&static_ndp_table, &hash_key);
    if(!item)
    {
        return NULL;
    }
    
    return (struct ndp_neighbor *)(item->data);
}


/*接口l3if 添加重下静态ndp*/
void ndp_static_process_l3if_add(uint32_t ifindex)
{
    struct hash_bucket  *bucket = NULL;
    struct ndp_neighbor *pndp = NULL;
    int cursor = 0;
    int ret = 0;

    ROUTE_NDP_ZLOG_DEBUG("ifindex:0x%x\n", ifindex);
    
    HASH_BUCKET_LOOP(bucket,cursor,static_ndp_table)
    {
        if(bucket->data)
    		continue;
        
		pndp = (struct ndp_neighbor *)bucket->data; 

        if(ifindex != pndp->key.ifindex)
            continue;
        
        ret = ipc_send_msg_n2(pndp, sizeof(struct ndp_neighbor), 1, MODULE_ID_FTM,  MODULE_ID_ROUTE,
                              IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_ADD, pndp->key.ifindex);
        if(ret < 0)
        {
            ROUTE_NDP_ZLOG_ERR("ipc_send_ftm error!\n");          
            return;
        }
    }
}


/*处理三层接口删除事件*/
void ndp_static_process_l3if_del(uint32_t ifindex)
{
    struct hash_bucket  *bucket = NULL;
    struct hash_bucket  *bucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    struct ndp_neighbor *pndp_temp = NULL;
    int cursor = 0;
    int ret = 0;

    ROUTE_NDP_ZLOG_DEBUG("ifindex:0x%x\n",ifindex);

    HASH_BUCKET_LOOPW(bucket,cursor,static_ndp_table)
    {
        if(bucket->data)
            continue;

		pndp = (struct ndp_neighbor *)bucket->data; 
        pndp_temp = pndp;
        bucket_temp = bucket;
        bucket = bucket->next;
        
        if(ifindex == pndp->key.ifindex)
            continue;

        ret = ipc_send_msg_n2(pndp_temp, sizeof(struct ndp_neighbor), 1, MODULE_ID_FTM,  MODULE_ID_ROUTE,
                            IPC_TYPE_NDP, NDP_INFO_STATIC_ND, IPC_OPCODE_DELETE, pndp_temp->key.ifindex);
        if(ret < 0)
        {
            ROUTE_NDP_ZLOG_ERR("ipc_send_ftm error!\n");          
            return;
        }

		hios_hash_delete(&static_ndp_table, bucket_temp);
        XFREE(MTYPE_NDP_STATIC_ENTRY, pndp_temp);
        XFREE(MTYPE_HASH_BACKET, bucket_temp);
        
       	gndp.num_static--;
    }    
}


