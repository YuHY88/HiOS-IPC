/*mange active ndp hash table*/

#include <lib/hash1.c>
#include <lib/route_com.h>
#include <lib/memtypes.h>
#include <lib/index.h>
#include <lib/errcode.h>
#include <lib/msg_ipc_n.h>
#include <lib/prefix.h>
#include <lib/inet_ip.h>
#include <lib/memory.h>
#include <lib/log.h>
#include <route/ndp_cmd.h>

#include "ftm.h"
#include "ftm_ifm.h"
#include "ftm_ndp.h"
#include "ftm_fib.h"
#include "pkt_ndp.h"
#include "ftm_nhp.h"

/*ndp 全?植???*/
struct ndp_global gndp = {ND_TOTAL_NUM, NDP_REACHABLE_TIME, NDP_STALE_TIME, NDP_INCOMPLETE_AGE_TIME, NDP_TIMER, 0, 0, 0, 0};

/*??态nd hash ??*/
struct hash_table ndp_table;


/*nd hash key ????*/
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

/*nd hash key ?冉?*/
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



/*hash ????始??*/
void ndp_table_init(uint32_t size)
{
    hios_hash_init(&ndp_table, size, ndp_hash, ndp_hash_compare);
    index_register(INDEX_TYPE_NDP, size);
}

//??hash 桶????nd ????
static void ndp_add_entry(struct ndp_neighbor *pnd_entry)
{    
    struct hash_bucket *item  = NULL;
    struct ndp_neighbor *pndp = NULL;
    uint32_t index = 0;
    int     ret = 0;

    FTM_NDP_ZLOG_DEBUG();
    if(!pnd_entry)
    {
        FTM_NDP_ZLOG_DEBUG("pndp_entry is NULL!\n");
        return;
    }
    
    item = XCALLOC(MTYPE_HASH_BACKET,sizeof (struct hash_bucket));
    pndp = XCALLOC(MTYPE_NDP_ENTRY,sizeof (struct ndp_neighbor));
    if(!item || !pndp)
    {
        FTM_NDP_ZLOG_ERR("Xcalloc failed!\n");
        return;
    }

    /*????ndp ?娲????*/
    index = index_alloc(INDEX_TYPE_NDP);
    if(!index)
    {
        FTM_NDP_ZLOG_ERR("Failed to alloc index for ndp!\n");
        XFREE(MTYPE_NDP_ENTRY,pndp );
        XFREE(MTYPE_HASH_BACKET,item );
        return;
    }

    /*?娲d ???畹絟ash ??*/
    memcpy(pndp, pnd_entry, sizeof(struct ndp_neighbor));
    pndp->nd_index = index + NDP_INDEX_BASE;
    item->data = pndp;
    item->hash_key = &(pndp->key);
    ret = hios_hash_add(&ndp_table, item);
    if(ret == -1)
    {
        FTM_NDP_ZLOG_ERR("Add ndp err!\n");
        index_free(INDEX_TYPE_NDP,pndp->nd_index - NDP_INDEX_BASE);
        XFREE(MTYPE_NDP_ENTRY, pndp);
        XFREE(MTYPE_HASH_BACKET,item);
        return;
    }

    /*???影?连??时??通知nhp ????*/
    if(pndp->status != NDP_STATUS_INCOMPLETE)
    {
        ftm_nhp_update_ndp(pndp, 1);
    }

    ret = ftm_msg_send_to_hal(pndp, sizeof(struct ndp_neighbor), 1, 
                                    IPC_TYPE_NDP, 0, IPC_OPCODE_ADD, 0);
    if(-1 == ret)
    {
        FTM_NDP_ZLOG_ERR("Notify hal add ndp failed!\n");
        return;
    }
}


//????nd????
int ndp_add(struct ndp_neighbor *pnd_entry)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket  *item = NULL;
    struct ftm_ifm *pifm = NULL;

    if(!pnd_entry)
    {
        FTM_NDP_ZLOG_ERR("Invalid argument!\n");
        return ERRNO_FAIL;
    }
    
    FTM_NDP_ZLOG_DEBUG();
    item = hios_hash_find(&ndp_table, &(pnd_entry->key));
    if(item)
    {
        pndp = item->data;
        ndp_update(pndp, pnd_entry);
    }
    else
    {
        FTM_NDP_ZLOG_DEBUG();
        if(pnd_entry->status == NDP_STATUS_STATIC)
        {
            if(GSTATIC_NDP_LT_GLIMIT && GNDP_NUM_LT_GLIMIT)
            {
                gndp.num_static ++;
                ndp_add_entry(pnd_entry);
            }
            /*??态nd 未?锏????值全??nd ?锏????值时??
                      *????删??一????态nd ??????*/
            else if(GSTATIC_NDP_LT_GLIMIT && GNDP_NUM_EQ_GLIMIT)
            {
                //ndp_delete_one_dynamic();
                //gndp.num_static ++;
                //return ndp_add_entry(pnd_entry);
            }
        }
        else
        {
            FTM_NDP_ZLOG_DEBUG();
            pifm = ftm_ifm_lookup(pnd_entry->key.ifindex);
            if(!pifm || !(pifm->parp))
            {
                FTM_NDP_ZLOG_ERR("Interface 0x%x pifm or pifm->parp not exit!\n",pnd_entry->key.ifindex);
                return ERRNO_FAIL;
            }
            if(PIF_DYNAMIC_NDP_LT_PLIMIT(pifm) && PIF_DYNAMIC_NDP_LT_PLIMIT(pifm)
                                  && GNDP_NUM_LT_GLIMIT && GDYNAMIC_NDP_LT_GLIMIT)
            { 
                pifm->parp->neighbor_count ++;
                gndp.count ++;
                ndp_add_entry(pnd_entry);
            }
        }     
    }
    
    return ERRNO_SUCCESS;
}



/*?卸?ifindex??port??status?欠??????懈????蚍祷?0*/
static int ndp_update_info(struct ndp_neighbor *pndp_new, struct ndp_neighbor *pndp_old)
{
	int flag =0;
    
	if(pndp_old->key.ifindex != pndp_new->key.ifindex)
	{
		pndp_old->key.ifindex = pndp_new->key.ifindex;
		flag++;
	}

	if(memcmp(pndp_old->mac,pndp_new->mac,MAC_LEN))
	{
		memcpy(pndp_old->mac,pndp_new->mac,MAC_LEN);
		flag++;
	}

    if(pndp_old->port != pndp_new->port)
	{
		pndp_old->port = pndp_new->port;
		flag++;
	}

	if(pndp_old->status !=  pndp_new->status)
	{
		pndp_old->status = pndp_new->status;
		flag++;
	}

    if(pndp_old->isrouter != pndp_new->isrouter)
    {
        pndp_old->isrouter = pndp_new->isrouter;
        flag++;
    }

	if(flag != 0)
		return 0;
	else
		return 1;
    
}


//????nd????
int ndp_update(struct ndp_neighbor *pnd_old, struct ndp_neighbor *pnd_new)
{    
    struct ftm_ifm *pifm = NULL;
    uint8_t  update_flag = 1;
    int ret = 0;

    FTM_NDP_ZLOG_DEBUG();
    
    /*??态???碌???态*/
    if(pnd_old->status == NDP_STATUS_STATIC)
    {
        if(pnd_new->status == NDP_STATUS_STATIC)
        {
            update_flag = ndp_update_info(pnd_new, pnd_old);

            //?馗??路???态ndp时??通知hal ???泳?态路??
            if(update_flag != 0)
            {
                 ret = ftm_msg_send_to_hal(pnd_old, sizeof(struct ndp_neighbor), 1, 
                                             IPC_TYPE_NDP, 0, IPC_OPCODE_UPDATE, 0);
                 if(-1 == ret)
                 {
                    FTM_NDP_ZLOG_ERR("Notify hal add arp failed!\n");
                    return ERRNO_FAIL;
                 }
            }
        }
    }

    /*??态???碌???态*/
    if(pnd_old->status != NDP_STATUS_STATIC)
    {
        if(pnd_new->status == NDP_STATUS_STATIC)
        {
            update_flag = ndp_update_info(pnd_new, pnd_old);

            /*?薷?????直?臃???*/
            if(update_flag)
                return ERRNO_SUCCESS;

            /*原?涌诩????约?*/
            pifm = ftm_ifm_lookup(pnd_old->key.ifindex);
            if(!pifm || !(pifm->parp) || (pifm->parp->neighbor_count <= 0))
            {
                FTM_NDP_ZLOG_ERR("Interface 0x%x pifm or pifm->parp not exit!\n", pnd_old->key.ifindex);
                return ERRNO_FAIL;
            }
            pifm->parp->neighbor_count --;

            /*全?旨???????*/
            gndp.count --;
            gndp.num_static ++;  

            /*?陆涌诩????约?*/
            pifm = ftm_ifm_lookup(pnd_new->key.ifindex);
            if(!pifm || !(pifm->parp))
            {
                FTM_NDP_ZLOG_ERR("Interface 0x%x pifm or pifm->parp not exit!", pnd_new->key.ifindex)
                return ERRNO_FAIL;
            }
            pifm->parp->neighbor_count ++;
            
        }
    }


    /*??态???碌???态*/
    if(pnd_old->status != NDP_STATUS_STATIC)
    {
        if(pnd_new->status == NDP_STATUS_REACHABLE)
        {
            update_flag = ndp_update_info(pnd_new, pnd_old);

            /*?薷?????直?臃???*/
            if(update_flag)
                return ERRNO_SUCCESS;

			FTM_NDP_ZLOG_DEBUG("Dynamic ndp update dynamic ndp!\n");
            pnd_old->time = gndp.reach_time;
            pnd_old->count = 0;
        }
    }

    if(update_flag == 0)
    {
         ftm_nhp_update_ndp(pnd_old, 1);
         ret = ftm_msg_send_to_hal(pnd_old, sizeof(struct ndp_neighbor), 1, 
                                     IPC_TYPE_NDP, 0, IPC_OPCODE_UPDATE, 0);
         if(-1 == ret)
         {
            FTM_NDP_ZLOG_ERR("Notify hal update ndp failed!\n");
            return ERRNO_FAIL;
         }
    }

    return ERRNO_SUCCESS;
}

/*删??nd ????*/
static void ndp_delete_entry(struct hash_bucket *item)
{
    struct ndp_neighbor *pndp = NULL;
    uint8_t ipv6_str[IPV6_ADDR_STRLEN];
    int ret = 0;

    if(!item || !(item->data))
    {
        FTM_NDP_ZLOG_DEBUG("Item or item->data is NULL!\n");
        return;
    }
    pndp = item->data;

    inet_ipv6tostr((struct ipv6_addr *)(pndp->key.ipv6_addr), (char *)ipv6_str, IPV6_ADDR_STRLEN);
    FTM_NDP_ZLOG_DEBUG("ipv6:%s vpn:%d\n", ipv6_str, pndp->key.vpnid);

    /*nd 删??通知nhp??hal????*/
    ftm_nhp_update_ndp(pndp, 0);
    ret = ftm_msg_send_to_hal(pndp, sizeof(struct ndp_neighbor), 1,
                             IPC_TYPE_NDP, 0, IPC_OPCODE_DELETE, 0);
    if(ret == -1)
    {
        FTM_NDP_ZLOG_ERR("Notify hal delete ndp failed!\n");
        return;
    }

    hios_hash_delete(&ndp_table, item);

    index_free(INDEX_TYPE_NDP,pndp->nd_index - NDP_INDEX_BASE);
    XFREE(MTYPE_NDP_ENTRY, pndp);
    XFREE(MTYPE_HASH_BACKET, item );
}


//删??nd????
int ndp_delete(struct ipv6_addr *paddr, uint16_t vpn, uint32_t ifindex)
{
	struct  hash_bucket *item = NULL;
    struct  ndp_neighbor *pndp = NULL;
	struct  ftm_ifm *pifm = NULL;
    struct  ndp_key key;
    uint8_t ipv6_str[IPV6_ADDR_STRLEN];

    inet_ipv6tostr(paddr, (char *)ipv6_str, IPV6_ADDR_STRLEN);
    FTM_NDP_ZLOG_DEBUG("ipv6:%s vpn:%d\n", ipv6_str, vpn);

    memset(&key, 0, sizeof(struct ndp_key));
    memcpy(key.ipv6_addr, paddr, sizeof(struct ipv6_addr));
    key.vpnid = vpn;
    key.ifindex = ifindex;
    
	item = hios_hash_find(&ndp_table, (void *)&key);
	if(item)
	{
		pndp = item->data;
		if(pndp->status == NDP_STATUS_STATIC)
		{
			gndp.num_static --;
			ndp_delete_entry(item);
		}
		else
		{
            /*????全??ndp ????*/
            gndp.count--;
            
            /*???陆涌?arp ????*/
            pifm = ftm_ifm_lookup(pndp->key.ifindex);
            if(pifm && (pifm->parp)&& (pifm->parp->neighbor_count> 0))
				pifm->parp->neighbor_count --;	

            ndp_delete_entry(item);
		}
	}

    return ERRNO_SUCCESS;
}


/*?匣?指???涌诘亩?态nd ????*/
int ndp_delete_interface(uint32_t ifindex)
{
	uint32_t cursor = 0;
	uint32_t num = 0;
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;

    FTM_NDP_ZLOG_DEBUG("ifindex:0x%x\n",ifindex)

	HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
	{
        if(pbucket->data)
		{
			pndp = pbucket->data;
			if((pndp->status != NDP_STATUS_STATIC)&&(pndp->key.ifindex == ifindex))
			{
				pndp->time = NDP_PROBE_AGE_TIME;
				pndp->status = NDP_STATUS_PROBE;
                pndp->count = 0;
				num ++;

				FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
                
                ndp_probe_fast(pndp);
				pndp->count ++;
			}
		}
	}
    
	FTM_NDP_ZLOG_DEBUG("%d dynamic ndp wait aged!\n",num);
    
	return num;

}


/*?匣?指??vpn ??nd ????*/
int ndp_delete_vpn(uint16_t vpnid)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;
	uint32_t num = 0;

    FTM_NDP_ZLOG_DEBUG("vpnid:%d\n", vpnid)

	HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
	{
        if(pbucket->data)
		{
			pndp = pbucket->data;
			if((pndp->status != NDP_STATUS_STATIC) && (pndp->key.vpnid == vpnid))
			{
				pndp->time = NDP_PROBE_AGE_TIME;
				pndp->status = NDP_STATUS_PROBE;
                pndp->count = 0;
				num ++;

				FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
                
                ndp_probe_fast(pndp);
				pndp->count ++;
			}
		}
	}
    
	FTM_NDP_ZLOG_DEBUG("%d dynamic ndp wait aged!\n",num);
    
	return num;
}


/*?匣????? ??nd ????*/
int ndp_delete_all(void)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;
	uint32_t cursor = 0;
	uint32_t num = 0;

    FTM_NDP_ZLOG_DEBUG();

	HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
	{
        if(pbucket->data)
		{
			pndp = pbucket->data;
			if(pndp->status != NDP_STATUS_STATIC)
			{
				pndp->time = NDP_PROBE_AGE_TIME;
				pndp->status = NDP_STATUS_PROBE;
                pndp->count = 0;
				num ++;

				FTM_NDP_STATUS_UPDATE_NOTIFY_HAL(pndp);
				
                ndp_probe_fast(pndp);
				pndp->count ++;
			}
		}
	}
    
	FTM_NDP_ZLOG_DEBUG("%d dynamic ndp wait aged!\n",num);
    
	return num;
}



/*????指??nd ????*/
struct ndp_neighbor *ndp_lookup(uint8_t paddr[16], uint16_t vpn, uint32_t ifindex)
{
    struct hash_bucket  *item = NULL;
    struct ndp_key key;

    if(!paddr)
    {
        FTM_NDP_ZLOG_ERR("argument invalid!\n");
        return NULL;
    }

    /*set hash key*/
    memset(&key, 0, sizeof(struct ndp_key));
    memcpy(key.ipv6_addr, paddr, 16);
    key.vpnid = vpn;
    key.ifindex = ifindex;

    item = hios_hash_find(&ndp_table, &key);
    if(!item)
    {
        FTM_NDP_ZLOG_ERR();
        return NULL;
    }
    
    return (struct ndp_neighbor *)(item->data);
}



//????ip??vpn???冶???
struct ndp_neighbor *ndp_lookup_active(struct ipv6_addr *paddr, uint16_t vpn, uint32_t ifindex)
{
    struct ndp_neighbor *pndp = NULL;

    pndp = ndp_lookup(paddr->ipv6, vpn, ifindex);
    if(!pndp)
    {
        ndp_miss(paddr,vpn,ifindex);
        return NULL;
    }
    else if(pndp->status == NDP_STATUS_INCOMPLETE 
              || pndp->status == NDP_STATUS_DELAY
              || pndp->status == NDP_STATUS_PROBE)
    {
        return NULL;
    }
	else
    {
        //stale状态??nd ?????????蟹??头???????时?谢???delay 状态
        if(pndp->status == NDP_STATUS_STALE)
        {
            ndp_fsm(pndp, pndp->key.ipv6_addr, NDP_EVENT_PKT_SEND, 0);
        }

    	return pndp;
    	
    }
}

void ndp_probe_fast(struct ndp_neighbor *ndp)
{
	struct ftm_ifm *pifm = NULL;
	struct ipv6_addr *pip6a = NULL;
	uint8_t smac[6] = {0};

	if (!ndp)
	{
		FTM_NDP_ZLOG_DEBUG("Fast probe err!\n");
		return;
	}
	
    pifm = ftm_ifm_lookup(ndp->key.ifindex);
    if(!pifm || !(pifm->pl3if)) return;
	
    pip6a = ftm_ifm_get_l3if_ipv6(pifm->pl3if, (struct ipv6_addr *)(ndp->key.ipv6_addr));
    if(!pip6a) pip6a = &(g_fib.routerv6_id);//?????也???同????ip??????route_id
    
    FTM_IFM_GET_MAC(pifm, smac);
    ndp_send(ndp, NDP_OPCODE_NSOLICIT, pip6a->ipv6, smac);
	
}

void ndp_delete_if_linkdown(uint32_t ifindex)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    struct ftm_ifm *pifm = NULL;
    uint32_t cursor = 0;

    FTM_NDP_ZLOG_DEBUG();
    
    HASH_BUCKET_LOOPW(pbucket,cursor,ndp_table)
    {
        if(!(pbucket->data))
            continue;

        pbucket_temp = pbucket;
        pbucket = pbucket->next;
        
        pndp = (struct ndp_neighbor *)pbucket_temp->data;
        if((pndp->key.ifindex == ifindex) && (pndp->status != NDP_STATUS_STATIC))
        {
            FTM_NDP_ZLOG_DEBUG();

			gndp.count --;
            
            pifm = ftm_ifm_lookup(ifindex);
            if(pifm && (pifm->parp))
            {
                pifm->parp->neighbor_count --;
            }

            ndp_delete_entry(pbucket_temp);
        }
    }

}

/*ndp incomplete entry age*/
int ndp_fake_entry_age()
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    struct ftm_ifm *pifm = NULL;
    uint32_t cursor = 0;

    HASH_BUCKET_LOOPW(pbucket, cursor, ndp_table)
    {
        pbucket_temp = pbucket;
        pbucket = pbucket_temp->next;

        pndp = pbucket_temp->data;
        if(pndp && (pndp->status == NDP_STATUS_INCOMPLETE))
        {
            if(pndp->time <= 0)
            {
                ndp_delete((struct ipv6_addr *)(pndp->key.ipv6_addr), pndp->key.vpnid, pndp->key.ifindex);
            }
            else
            {
                pifm = ftm_ifm_lookup(pndp->key.ifindex);
                if(pifm && (pifm->parp) && (pifm->parp->ndp_disable == NDP_ENABLE_DEF))
                {
                    ndp_probe_fast(pndp);
                }
            }
            
            pndp->time --;
        }
    }
    
	high_pre_timer_add("NdpHalfTimer", LIB_TIMER_TYPE_NOLOOP, ndp_fake_entry_age, NULL, 1000);/* thread to  half join */
	return 0;
}







/*ndp status change timer;REACHABLE---->STALE;  INCOMPLETE---->EMPTY  STALE--->DELAY  DELAY->PROBE*/
int ndp_status_timer()
{
    struct ndp_neighbor *ndp = NULL;
    struct hash_bucket  *pbucket = NULL;
    uint32_t cursor = 0;
    uint8_t ip6str[IPV6_ADDR_STRLEN];

    HASH_BUCKET_LOOPW(pbucket, cursor, ndp_table)
    {
        if(!(pbucket->data))
        {
            pbucket = pbucket->next;
            continue;
        }
        
        ndp = (struct ndp_neighbor *)(pbucket->data);
        pbucket = pbucket->next;
        if((ndp->status == NDP_STATUS_PERMANENT)
           ||(ndp->status == NDP_STATUS_STATIC)
           ||(ndp->status == NDP_STATUS_INCOMPLETE))
            continue;

        inet_ipv6tostr((struct ipv6_addr *)(ndp->key.ipv6_addr), (char *)ip6str, IPV6_ADDR_STRLEN);
        FTM_NDP_ZLOG_DEBUG("ip6:%s status:%d time:%d count:%d\n",ip6str, ndp->status, ndp->time, ndp->count);


        /*????REACHABLE ?????纱?时?涞?谢?为STALE 状态
                        ????INCOMPLETE ????????时?涞街??删??*/
        if(ndp->time < NDP_TIMER)
        {  
            FTM_NDP_ZLOG_DEBUG("status:%d\n", ndp->status);
            if(ndp->status == NDP_STATUS_REACHABLE)
            {
                //?纱???时?涞?????纱??猿?时?录?
                ndp_fsm(ndp, ndp->key.ipv6_addr, NDP_EVENT_REACHABLE_TIMEOUT, 0);
            }
			else if(ndp->status == NDP_STATUS_STALE)
			{
				//?潜?准协???娑?????????员???????????????效????占??????
				ndp_fsm(ndp, ndp->key.ipv6_addr, NDP_EVENT_STALE_TIMEOUT, 0);
			}
            else if(ndp->status == NDP_STATUS_DELAY)
            {
                /*delay状态??时?????纱???确?铣?时?录?*/
                ndp_fsm(ndp, ndp->key.ipv6_addr, NDP_EVENT_REACHCONFIRM_TIMEOUT, 0);
            }
            //else if((ndp->status == NDP_STATUS_PROBE) && (ndp->count >= 3))
            else if(ndp->status == NDP_STATUS_PROBE)
            {
                if (ndp->count >= 3)
                {
                    ndp_delete((struct ipv6_addr *)(ndp->key.ipv6_addr), ndp->key.vpnid, ndp->key.ifindex);
                }
                else
                {                
				    ndp_probe_fast(ndp);
                    ndp->count ++;
                    ndp->time = NDP_PROBE_AGE_TIME; 
                }
            }
        }

        
        NDP_ENTRY_TIME_DECREASE(ndp->time);
    }
    
	return ERRNO_SUCCESS;
}




//????ndp???撇???息
void ftm_ndp_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode)
{
    struct ndp_neighbor *pndp = NULL;
    struct ipv6_addr ipv6;
    uint8_t  i = 0;
    
    FTM_NDP_ZLOG_DEBUG("pdata:%p data_len:%d data_num:%d subtype:%d opcode:%d\n", 
                                        pdata, data_len, data_num, subtype, opcode);

    for(i = 0; i < data_num; i++)
    {
        if(subtype == NDP_INFO_STATIC_ND)
        {
            pndp = (struct ndp_neighbor *)pdata;
            if(opcode == IPC_OPCODE_ADD)
            {
                FTM_NDP_ZLOG_DEBUG("pndp:%p vpn:%d pndp->status:%d\n", pndp, pndp->key.vpnid, pndp->status);
                ndp_add(pndp);
            }
            else if(opcode == IPC_OPCODE_DELETE)
            {
                memset(&ipv6, 0, sizeof(struct ipv6_addr));
                memcpy(&ipv6, pndp->key.ipv6_addr, sizeof(struct ipv6_addr));
                ndp_delete(&ipv6, pndp->key.vpnid, pndp->key.ifindex);
            }
            else if(opcode == IPC_OPCODE_UPDATE)
            {
                ndp_add(pndp);
            }

            pdata = ((char *)pdata + sizeof(struct ndp_neighbor));
        }
        if((subtype == NDP_INFO_REACH_TIME) 
            || (subtype == NDP_INFO_STALE_TIME)
            || (subtype == NDP_INFO_NUM_LIMIT)
            || (subtype == NDP_INFO_FAKE_TIME))
        {
            ftm_ndp_global_add(pdata, subtype);
            pdata = (char *)pdata + sizeof(struct ndp_global);
        }
        
    }

}


//????ndp全????????息
int ftm_ndp_global_add(struct ndp_global *pdata, uint8_t subtype)
{
    struct ndp_global *g_ndp = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    struct ndp_neighbor *pndp_temp = NULL;
    struct ftm_ifm *pifm = NULL;
    uint32_t cursor = 0;

    FTM_NDP_ZLOG_DEBUG("pdata:%p type:%d\n", pdata, subtype);

    g_ndp = (struct ndp_global *)pdata;

    if(subtype == NDP_INFO_REACH_TIME)
	{
		gndp.reach_time= g_ndp->reach_time;

        /*刷??ndp ?纱?时??为????值*/
        HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
        {
            if(pbucket->data)
            {
                pndp = pbucket->data;
                if(pndp->status == NDP_STATUS_REACHABLE)
                    pndp->time = gndp.reach_time;
            }
        }
	}
    
    if(subtype == NDP_INFO_STALE_TIME)
	{
		gndp.stale_time= g_ndp->stale_time;

        /*刷??ndp stale 状态?却?时??为????值*/
        HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
        {
            if(pbucket->data)
            {
                pndp = pbucket->data;
                if(pndp->status == NDP_STATUS_STALE)
                    pndp->time = gndp.stale_time;
            }
        }
	}
   
    if(subtype == NDP_INFO_FAKE_TIME)
    {
		gndp.fake_time = g_ndp->fake_time;
        
        HASH_BUCKET_LOOP(pbucket, cursor, ndp_table)
        {
            if(!(pbucket->data))
            {
                pndp = pbucket->data;
                if(pndp->status == NDP_STATUS_INCOMPLETE)
                    pndp->time = gndp.fake_time;
            }
        }
    }


	if(subtype == NDP_INFO_NUM_LIMIT)
	{
		gndp.num_limit= g_ndp->num_limit;

        if(GDYNAMIC_NDP_LT_GLIMIT || GDYNAMIC_NDP_EQ_GLIMT)
            return ERRNO_SUCCESS;

        FTM_NDP_ZLOG_ERR("ftm.gndp.num_limit:%d temp.gndp.num_limit:%d\n",
                                        gndp.num_limit, g_ndp->num_limit);
        
		HASH_BUCKET_LOOPW(pbucket, cursor, ndp_table)
		{
            if(pbucket->data)
            {
                pndp = pbucket->data;
                pndp_temp = pndp;
                pbucket_temp = pbucket;
                pbucket = pbucket->next;

                if(pndp_temp->status != NDP_STATUS_STATIC)
                {
                    /*????全??ndp ????*/
                    gndp.count--;
                    
                    /*???陆涌?arp ????*/
                    pifm = ftm_ifm_lookup(pndp->key.ifindex);
                    if(pifm && (pifm->parp)&& (pifm->parp->neighbor_count> 0))
                        pifm->parp->neighbor_count --;  

                    ndp_delete_entry(pbucket_temp);
                }

                if(GDYNAMIC_NDP_EQ_GLIMT || GDYNAMIC_NDP_LT_GLIMIT)
                    return ERRNO_SUCCESS;
            }
		}
	}
    
	return ERRNO_SUCCESS;

}

//l3if ????时???咏涌诳?????息
int ftm_ndpif_add(struct ftm_ifm *pifm)
{
    FTM_NDP_ZLOG_DEBUG();

    if(NULL == pifm)
		return ERRNO_FAIL;
    
    FTM_NDP_ZLOG_DEBUG("pifm->ifindex:0x%x\n", pifm->ifm.ifindex);	

	if(pifm->parp == NULL)
	{
		pifm->parp = XCALLOC(MTYPE_IFM_ENTRY,sizeof(struct ifm_arp));
		if(pifm->parp == NULL)
		{
            FTM_NDP_ZLOG_DEBUG("Xcalloc failed!\n");
            return ERRNO_MALLOC;
		}
	}

	return ERRNO_SUCCESS;

}


//l3if 删??时删???涌诳?????息
int ftm_ndpif_delete(struct ftm_ifm *pifm)
{
    FTM_NDP_ZLOG_DEBUG();

    if(NULL == pifm)
		return ERRNO_FAIL;
	
	FTM_NDP_ZLOG_DEBUG("pifm->ifm.ifindex:0x%x\n", pifm->ifm.ifindex);

	/* 删??3???涌?ndp ??????息 */
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
		
    	if (!(ROUTE_IF_IP_EXIST(pifm->pl3if->ipv4[0].addr)))
		{
			XFREE(MTYPE_IFM_ENTRY, pifm->parp);	
		}	
    }

	return ERRNO_SUCCESS;

}


//???陆涌诳?????息
int ftm_ndpif_update(void *pdata, uint32_t ifindex, uint8_t subtype)
{
    struct ifm_arp *pif_ndp = NULL;
    struct ftm_ifm *pifm = NULL;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_temp = NULL;
    struct ndp_neighbor *pndp = NULL;
    struct ndp_neighbor *pndp_temp = NULL;
    int cursor = 0;

    if(!pdata)
        return ERRNO_FAIL;
    
    pif_ndp = (struct ifm_arp *)pdata;
    pifm = ftm_ifm_lookup(ifindex);
    if(!pifm)
    {
        FTM_NDP_ZLOG_ERR();
        return ERRNO_FAIL;
    }

    switch(subtype)
    {
        case NDP_INFO_LEARN_LIMIT:
            pifm->parp->ndp_disable = pif_ndp->ndp_disable;
            break;
        case NDP_INFO_AGENT:
            pifm->parp->ndp_proxy= pif_ndp->ndp_proxy;
            break;
        case NDP_INFO_NUM_LIMIT:
            pifm->parp->neighbor_limit = pif_ndp->neighbor_limit;

            if(PIF_DYNAMIC_NDP_LT_PLIMIT(pifm) || PIF_DYNAMIC_NDP_EQ_PLIMIT(pifm))
                return ERRNO_SUCCESS;

            HASH_BUCKET_LOOPW(pbucket, cursor, ndp_table)
            {
                if(pbucket->data)
                {
                    pndp = pbucket->data;
                    pndp_temp = pndp;
                    pbucket_temp = pbucket;
                    pbucket = pbucket->next;

                    if((pndp_temp->status != NDP_STATUS_STATIC) &&
                        (pndp_temp->key.ifindex == pif_ndp->ifindex))
                    {
                        /*????全??ndp ????*/
                        gndp.count--;
                        
                        /*???陆涌?arp ????*/
                        if(pifm && (pifm->parp)&& (pifm->parp->neighbor_count> 0))
            				pifm->parp->neighbor_count --;	
                        
                        ndp_delete_entry(pbucket_temp);
                    }
                }
            
	            if(PIF_DYNAMIC_NDP_EQ_PLIMIT(pifm))
                    return ERRNO_SUCCESS;
            }
            break;
        default:
            break;
    }

    return ERRNO_SUCCESS;
}

/*if interface addr delete,delete related ipv6 neighbors*/
void ftm_ndpif_addr_delete(struct ftm_ifm *pifm, uint8_t ip_flag)
{
    struct ndp_neighbor *pndp = NULL;
    struct hash_bucket *pbucket = NULL;
    uint32_t cursor = 0;

    if(pifm == NULL) return;
    FTM_NDP_ZLOG_DEBUG("pfim->ifm.ifindex:0x%x flag:%d\n",pifm->ifm.ifindex, ip_flag);

    HASH_BUCKET_LOOPW(pbucket, cursor, ndp_table)
	{
        if(pbucket->data)
		{
			pndp = (struct ndp_neighbor *)pbucket->data;
			pbucket = pbucket->next;

			if((pndp->status != NDP_STATUS_STATIC)&&(pndp->key.ifindex == pifm->ifm.ifindex))
			{
                /*if master ip delete,delete all the ndp entry except linklocal*/
                if(ip_flag == NDP_IPV6_ADDR_MASTER)
                {
                	FTM_NDP_ZLOG_DEBUG();
                    if(!ipv6_is_linklocal((struct ipv6_addr *)(pndp->key.ipv6_addr)))
                        ndp_delete((struct ipv6_addr *)(pndp->key.ipv6_addr), pndp->key.vpnid, pndp->key.ifindex);
                }
                else if (ip_flag == NDP_IPV6_ADDR_SLAVE)
                {
                	FTM_NDP_ZLOG_DEBUG();
                    if(ipv6_is_same_subnet((struct ipv6_addr *)(pifm->pl3if->ipv6[1].addr),
						(struct ipv6_addr *)(pndp->key.ipv6_addr), pifm->pl3if->ipv6[1].prefixlen))
                    {
                        ndp_delete((struct ipv6_addr *)(pndp->key.ipv6_addr), pndp->key.vpnid, pndp->key.ifindex);
                    }
                }
                else if(ip_flag == NDP_IPV6_ADDR_LINKLOCAL)
                {
                	FTM_NDP_ZLOG_DEBUG();
                    if(ipv6_is_linklocal((struct ipv6_addr *)(pndp->key.ipv6_addr)))
                        ndp_delete((struct ipv6_addr *)(pndp->key.ipv6_addr), pndp->key.vpnid, pndp->key.ifindex);
                        
                }
                else
                {
                    FTM_NDP_ZLOG_ERR("This must be error!ip_flag:%d\n",ip_flag);
                }
			}
		}
	}
    
}


int ftm_ndpif_msg(void *pdata, int data_len, int data_num, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t ifindex)
{
	uint8_t i = 0;
    uint8_t *data = NULL;

    FTM_NDP_ZLOG_DEBUG();

    if(pdata == NULL)  
        return ERRNO_FAIL;

    data = (uint8_t *)pdata;
	for(i = 0;i < data_num;i++)
	{
        if(opcode == IPC_OPCODE_UPDATE)
		{
			ftm_ndpif_update(data, ifindex, subtype);
		}

		data += sizeof(struct ifm_arp);
	}

	return ERRNO_SUCCESS;
}

/*??始??*/
void ftm_ndp_init(void)
{
    ndp_table_init(ND_TOTAL_NUM);
    index_register(INDEX_TYPE_NDP, ND_TOTAL_NUM);
    ndp_register();	
	high_pre_timer_add("NdpStatusTimer", LIB_TIMER_TYPE_LOOP, ndp_status_timer, NULL, NDP_TIMER*1000);/* thread to  half join */
    ndp_fake_entry_age();
}




