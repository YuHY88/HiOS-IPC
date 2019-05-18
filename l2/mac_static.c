/*
*       manage the static mac table
*
*/

//#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lib/msg_ipc.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include "mac_static.h"
#include <lib/devm_com.h>
#include "l2_msg.h"

/************************************************
 *
 * Function: mac_hash
 * Input:
 *      parp :hash key
 * Output:
        NONE
 * Return:
 *      NONE
 * Description:
 *      Accepts a pointer to a datum to be hashed
 *      and returns an unsigned integer
 ************************************************/
static unsigned int mac_hash(void *hash_key)
{
    unsigned int val = 0, i = 0;
    unsigned char ch;

    for (i = 0; i < MAC_LEN; i++)
    {
        ch = ((struct mac_key *)hash_key)->mac[i];
        val += (unsigned int)ch;
    }

    val += ((struct mac_key *)hash_key)->vlanid;
    return val;
}
/************************************************
 *
 * Function: mac_compare
 * Input:
 *      item    :the item in the hash table
        hash_key:hash key
 * Output:
        NONE
 * Return:
 *      -1  Failed to compare the mac hash table
         0  Successfully compare the mac hash table
 * Description:
 *      comparison function.
 ************************************************/
static int mac_compare(void *item, void *hash_key)
{
    struct mac_key *pstKey = NULL;
    struct mac_key *pstKeyCompare = NULL;

    /* 参数非空 */
    if ((NULL == item) || (NULL == hash_key))
    {
        return -1;
    }

    pstKey = (struct mac_key *)hash_key;
    pstKeyCompare = (struct mac_key *)(((struct hash_bucket *)item)->hash_key);

    if (NULL == pstKeyCompare)
    {
        return -1;
    }

    if ((memcmp(pstKey->mac, pstKeyCompare->mac, MAC_LEN) == 0) &&
        (pstKey->vlanid == pstKeyCompare->vlanid))
    {
        return 0;
    }

    return -1;
}


/************************************************
 * Function: mac_static_table_init
 * Input:
 *      size
 * Output:
        NONE
 * Return:
 *      NONE
 * Description:
 *      Initialization of static mac hash table.
 ************************************************/
void mac_static_table_init(unsigned int size)
{
    hios_hash_init(&mac_static_table, size, mac_hash, mac_compare);
}

/************************************************
 * Function: mac_static_add
 * Input:
 *      pmac :the item of in the hash table
 * Output:
        NONE
 * Return:
 *      -1 failed
         0 successed
 * Description:
 *      Add static mac hash item.
 ************************************************/
int mac_static_add(struct mac_entry *pmac)
{
    struct hash_bucket *pstItem = NULL;
    int iRet = 0;
    int ret = 0;
	extern unsigned int devtype;
	
    if (NULL == pmac)
    {
        return MAC_ERROR_NULL;
    }
  
    pstItem = hios_hash_find(&mac_static_table, (void *)(&pmac->key));

    if (pstItem)
    {
        MAC_LOG_DBG("%s[%d]:%s:This mac with vlan is already in static_mac_table !\n",__FILE__,__LINE__, __FUNCTION__);
        return MAC_ERROR_EXISTED;            
    }
    else
    {
     
        if(MAC_STATIC_NUM==mac_static_table.num_entries)
        {
              MAC_LOG_DBG("%s[%d]:%s:static mac add error, the num of static mac is already equal to the limit num ! \n",__FILE__,__LINE__, __FUNCTION__);
              return MAC_ERROR_OVERSIZE;  
        }	
	
        pstItem = (struct hash_bucket *) XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        if ( NULL == pstItem )
        {
            zlog_err("%s[%d]:%s:malloc pstItem failed!\n",__FILE__,__LINE__, __FUNCTION__);
            return -1;
        }
        
        memset(pstItem, 0, sizeof(struct hash_bucket));
        pstItem->data = pmac;
        pstItem->hash_key = &(pmac->key);
        //ret = ipc_send_hal_wait_ack(pmac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_ADD, 0);
        ret = l2_msg_send_hal_wait_ack(pmac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, MODULE_ID_L2, 
                                                    IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_ADD, 0);

		/*To solve the problem of bug55476, arp learns to remove static macarp after arp after aging*/
		if(devtype == ID_HT157 || devtype == ID_HT158)
			if(ret == 2)
				return MAC_ERROR_EXISTED;

		if (ret)
        {
              MAC_LOG_DBG("%s[%d]:%s:static mac add error,hal return ack  fail !\n",__FILE__,__LINE__, __FUNCTION__);
              return MAC_ERROR_HAL_FAIL;
        }
        else
        {
            iRet = hios_hash_add(&mac_static_table, pstItem);
            if (iRet)
            {
                XFREE(MTYPE_MAC_STATIC, pstItem->data);
                XFREE(MTYPE_HASH_BACKET, pstItem);
                return MAC_ERROR_HASH;
            }
        }
    }

    return MAC_ERROR_NO;
}

/************************************************
 * Function: mac_static_local_add
 * Input:
 *      pmac :the item of in the hash table
 * Output:
        NONE
 * Return:
 *      -1 failed
         0 successed
 * Description:
 *      Add static mac hash item,but not send to hal.
 ************************************************/
int mac_static_local_add(struct mac_entry *pmac)
{
    struct hash_bucket *pstItem = NULL;
    int iRet = 0;

    if (NULL == pmac)
    {
        return MAC_ERROR_NULL;
    }
   
    pstItem = hios_hash_find(&mac_static_table, (void *)(&pmac->key));
    if (pstItem)
    {
        MAC_LOG_DBG("%s[%d]:%s:This mac with vlan is already in static_mac_table !\n",__FILE__,__LINE__, __FUNCTION__);
        return MAC_ERROR_EXISTED;            
    }
    else
    {
        if(MAC_STATIC_NUM==mac_static_table.num_entries)
        {
              MAC_LOG_DBG("%s[%d]:%s:static mac add error, the num of static mac is already equal to the limit num ! \n",__FILE__,__LINE__, __FUNCTION__);
              return MAC_ERROR_OVERSIZE;  
        }	
	
        pstItem = (struct hash_bucket *) XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
        pstItem->data = pmac;
        pstItem->hash_key = &(pmac->key);
     
        iRet = hios_hash_add(&mac_static_table, pstItem);
        if (iRet)
        {
            XFREE(MTYPE_MAC_STATIC, pstItem->data);
            XFREE(MTYPE_HASH_BACKET, pstItem);
            return MAC_ERROR_HASH;
        }
    }
    return MAC_ERROR_NO;
}


/************************************************
 *
 * Function: mac_static_delete
 * Input:
 *      pmac    : Specifying the mac of item
        vlanid: Specifying the vlanid of item
 * Output:
        NONE
 * Return:
 *      0 successed
 * Description:
 *      Delete a mac .
 ************************************************/
int mac_static_delete(uchar *pmac, uint16_t vlanid)
{
    struct mac_key stKey;
    struct hash_bucket *pstItem = NULL;
    struct mac_entry *pstMac= NULL;
    int ret = 0;

    memcpy(stKey.mac, pmac, MAC_LEN);
    stKey.vlanid = vlanid;

    pstItem = hios_hash_find(&mac_static_table, (void *) & stKey);

    if (NULL != pstItem)
    {
        pstMac = (struct mac_entry *)pstItem->data;
        
        //ret = ipc_send_hal_wait_ack(pstMac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_DELETE, 0);      	
        ret = l2_msg_send_hal_wait_ack(pstMac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, 
                                MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_DELETE, 0);      	
        if (ret)
        {
            MAC_LOG_DBG("%s[%d]:%s:static mac delete,hal return ack  fail !\n",__FILE__,__LINE__, __FUNCTION__);
        //    return -1;
        }
        hios_hash_delete(&mac_static_table, pstItem);
        XFREE(MTYPE_MAC_STATIC, pstMac);
        XFREE(MTYPE_HASH_BACKET, pstItem);

        return 0;
    }
    else
    {
        pstMac = (struct mac_entry *) XMALLOC(MTYPE_MAC_STATIC, sizeof(struct mac_entry));
        if(NULL==pstMac)
        {
            zlog_err("%s[%d]:%s:static mac_entry  malloc  fail !\n",__FILE__,__LINE__, __FUNCTION__);
            return MAC_ERROR_MALLOC;
        }
        memset(pstMac, 0, sizeof(struct mac_entry));
        pstMac->status = MAC_STATUS_DYNAMIC;
        pstMac->key = stKey;
        //ret = ipc_send_hal_wait_ack(pstMac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_DELETE, 0);
        ret = l2_msg_send_hal_wait_ack(pstMac, sizeof(struct mac_entry), 1, MODULE_ID_HAL, 
							MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_DELETE, 0);
		XFREE(MTYPE_MAC_STATIC, pstMac);

        if (ret)
        {
            MAC_LOG_DBG("%s[%d]:%s:dynamic mac delete,hal return ack  fail !\n",__FILE__,__LINE__, __FUNCTION__);
            return MAC_ERROR_NOT_EXISTED;
        }
        return 0;
    }
}


/************************************************
 *
 * Function: mac_static_lookup
 * Input:
 *      pmac  : Specifying the mac of item
        vlanid: Specifying the vlanid of item
 * Output:
        NONE
 * Return:
 *      NULL  :no such item
        other : A pointer to the item in the hash table
 * Description:
 *      Lookup static mac hash item.
 ************************************************/
struct mac_entry *mac_static_lookup(uchar *pmac, uint16_t vlanid)
{
    struct mac_key stKey;
    struct hash_bucket *pstItem = NULL;

    memcpy(stKey.mac, pmac, MAC_LEN);
    stKey.vlanid = vlanid;

    pstItem = hios_hash_find(&mac_static_table, (void *)&stKey);

    if (NULL == pstItem)
    {
        return NULL;
    }

    return (struct mac_entry *)pstItem->data;

}

/************************************************
 * Function: mac_static_lookup_by_ifindex
 * Input:
 *      ifindex : The interface index
 * Output:
        NONE
 * Return:
 *      NULL  :no such item
        other : A pointer to the item in the mac_static_table
 * Description:
 *      Lookup static mac table item.
 ************************************************/
struct mac_entry * mac_static_lookup_by_ifindex(uint32_t ifindex)
{
    int cursor = 0;   
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_entry = NULL;
    
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
            pmac_entry = (struct mac_entry *)pbucket->data;

            if (pmac_entry->outif == ifindex)
            {
            	return pmac_entry;
            }     
    }
    return NULL;
}

/************************************************
 * Function: mac_static_delete_by_ifindex
 * Input:
 *      ifindex : The interface index
 * Output:
        NONE
 * Return:
 *      0   successed
         -1 failed
 * Description:
 *      According to the interface index to delete a static MAC.
 ************************************************/
int mac_static_delete_by_ifindex(uint32_t ifindex)
{
    struct mac_entry *pmac_entry = NULL;
    int ret=0;

    pmac_entry=mac_static_lookup_by_ifindex(ifindex);
    while(NULL!=pmac_entry)
    {
            ret=mac_static_delete(pmac_entry->key.mac,pmac_entry->key.vlanid);
            if (0 != ret)
            {
                MAC_LOG_DBG("%s[%d] mac_static_delete error!\n", __FUNCTION__, __LINE__);
                return -1;
            }
            pmac_entry=mac_static_lookup_by_ifindex(ifindex);
    }
    return 0;
}

/************************************************
 * Function: mac_static_delete_all_by_ifindex
 * Input:
 *      ifindex : The interface index
 * Output:
        NONE
 * Return:
 *      0   successed
         -1 failed
 * Description:
 *      According to the interface index to delete all static MAC of this interface, send one ipc msg only.
 ************************************************/
int mac_static_delete_all_by_ifindex(uint32_t ifindex)
{
    int ret = 0;
    int cursor;
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pbucket_next = NULL;
    struct mac_entry *pmac_entry= NULL;
    
    HASH_BUCKET_LOOPW(pbucket, cursor, mac_static_table)
    {
        pmac_entry = (struct mac_entry *)pbucket->data;
        pbucket_next =	pbucket->next;	
        if(pmac_entry->outif == ifindex)
        {
            hios_hash_delete(&mac_static_table, pbucket);
            XFREE(MTYPE_MAC_STATIC, pmac_entry);
            XFREE(MTYPE_HASH_BACKET, pbucket);	
        }
        pbucket = pbucket_next;
    }
    
    ret=ipc_send_msg_n2(&ifindex, sizeof(ifindex), 1, MODULE_ID_HAL, MODULE_ID_L2, 
                    IPC_TYPE_MAC,(uint8_t)MAC_INFO_STATIC_DEL_ALL, IPC_OPCODE_DELETE, 0);
    if (ret)
    {
        MAC_LOG_DBG("%s[%d] ipc_send_hal, return ack fail, err=%d!\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }

    return 0;
}

/************************************************
 * Function: mac_static_lookup_next_by_mac_entry
 * Input:
 *      ifindex : A pointer to the item in the mac_static_table
 * Output:
        NONE
 * Return:
 *      NULL  :no such item
        other : A pointer to the item in the mac_static_table
 * Description:
 *      Lookup next item by a static mac table item.
 ************************************************/
struct mac_entry * mac_static_lookup_next_by_mac_entry(struct mac_entry * pmac)
{
    int cursor = 0;   
    int flag=0;
    struct hash_bucket *pbucket = NULL;
    struct mac_entry *pmac_find = NULL;
    
    HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
    {
            pmac_find = (struct mac_entry *)pbucket->data;
            if(flag==0)
            {
	            if ((pmac_find->outif == pmac->outif)&&(pmac_find->key.vlanid==pmac->key.vlanid)\
					&&((memcmp(pmac_find->key.mac, pmac->key.mac, MAC_LEN) == 0)))
	            {
	            		flag=1;
	            		continue;
	            }   
            }
            else
            {
			if (pmac_find->outif == pmac->outif)
	            {
	            		return pmac_find;
	            } 
            }
    }
    return NULL;
}

/************************************************
 * Function: mac_static_add_by_ifindex
 * Input:
 *      ifindex : The interface index
 * Output:
        NONE
 * Return:
 *      0   successed
         -1 failed
 * Description:
 *      According to the interface index to add a static MAC.
 ************************************************/
int mac_static_add_by_ifindex(uint32_t ifindex)
{
    struct mac_entry *pmac_entry = NULL;
    int ret=0;

    pmac_entry=mac_static_lookup_by_ifindex(ifindex);
    while(NULL!=pmac_entry)
    {
            //ret = ipc_send_hal_wait_ack(pmac_entry, sizeof(struct mac_entry), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_ADD, 0);
            ret = l2_msg_send_hal_wait_ack(pmac_entry, sizeof(struct mac_entry), 1, MODULE_ID_HAL, 
                                    MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC, IPC_OPCODE_ADD, 0);
            if (ret)
            {
                MAC_LOG_DBG("%s[%d]:%s:static mac add error,hal return ack  fail !\n",__FILE__,__LINE__, __FUNCTION__);
                return -1;
            }
            pmac_entry=mac_static_lookup_next_by_mac_entry(pmac_entry);
    }
    return 0;
}

/************************************************
 * Function: mac_ageing_time_set
 * Input:
 *      time :the time of mac ageing
 * Output:
        NONE
 * Return:
 *      -1 failed
         0 successed
 * Description:
 *      Set the time of mac ageing.
 ************************************************/
int mac_ageing_time_set(struct mac_configuration *config)
{
    int ret = 0;	
	MAC_LOG_DBG("before ipc_send_hal time:%d\n",config->age_time);
    //ret = ipc_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_AGE_TIME_SET, IPC_OPCODE_UPDATE, 0);
    ret = l2_msg_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, 
                            MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_AGE_TIME_SET, IPC_OPCODE_UPDATE, 0);
    if (ret)
    {
        MAC_LOG_DBG("%s[%d]:%s:mac_ageing_time_set error,hal return ack  fail!\n",__FILE__,__LINE__, __FUNCTION__);
        return -1;
    }
    return 0;
}


/************************************************
 * Function: mac_move_set
 * Input:
 *      mac_move : the status of mac move enable or disable
 * Output:
        NONE
 * Return:
 *      -1 failed
         0 successed
 * Description:
 *      Set the status of mac move.
 ************************************************/
int mac_move_set(struct mac_configuration *config)
{
    int ret=0;
	MAC_LOG_DBG("before ipc_send_hal mac move :%d\n",config->mac_move);
    //ret = ipc_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC_MOVE, IPC_OPCODE_UPDATE, 0);
    ret = l2_msg_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, 
                            MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC_MOVE, IPC_OPCODE_UPDATE, 0);
    if (ret)
    {
        MAC_LOG_DBG("%s[%d]:%s:mac_move_set error,hal return ack  fail!\n",__FILE__,__LINE__, __FUNCTION__);
        return -1;
    }
    return 0;
}

/************************************************
 * Function: mac_limit_set
 * Input:
 *      mac_move : the status of mac limit number
 * Output:
        NONE
 * Return:
 *      -1 failed
         0 successed
 * Description:
 *      Set the status of mac limit.
 ************************************************/
int mac_limit_set(struct mac_configuration *config)
{
    int ret=0;
	MAC_LOG_DBG("before ipc_send_hal limit:%d-action:%d\n",config->limit_num,config->limit_action);
    //ret = ipc_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, 0);
    ret = l2_msg_send_hal_wait_ack(config, sizeof(struct mac_configuration), 1, MODULE_ID_HAL, 
                            MODULE_ID_L2, IPC_TYPE_MAC, (uint8_t)MAC_INFO_MAC_LIMIT, IPC_OPCODE_UPDATE, 0);
    if (ret)
    {
        MAC_LOG_DBG("%s[%d]:%s:mac_limit_set error,hal return ack  fail!\n",__FILE__,__LINE__, __FUNCTION__);
        return -1;
    }
    return 0;
}

/************************************************
 * Function: mac_config_init
 * Input:
 *      mac_config:mac configuration  
 * Output:
        NONE
 * Return:
 *      NONE
 * Description:
 *      Initialization of mac configuration.
 ************************************************/
void mac_config_init(void)
{	
	struct mac_configuration temp_config;
	
	mac_config.age_time=(uint32_t)MAC_AGE_TIME;
	mac_config.mac_move=(uint8_t)ENABLE;
	mac_config.limit_num=0;
	
	memset(&temp_config,0,sizeof(struct mac_configuration));	
	memcpy(&temp_config,&mac_config,sizeof(struct mac_configuration));
	
	mac_ageing_time_set(&temp_config);
	mac_move_set(&temp_config);
	mac_limit_set(&temp_config);
}


/*mib 查询接口*/
int mac_static_reply_mac_config(struct ipc_msghdr_n *phdr)
{
	int ret=0;
	
	//ret = ipc_send_reply (&mac_config, sizeof (struct mac_configuration), phdr->sender_id, phdr->module_id,phdr->msg_type, phdr->msg_subtype, phdr->msg_index );
    ret = ipc_send_reply_n2(&mac_config, sizeof (struct mac_configuration), 1, phdr->sender_id,
                                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
	if(ret<0)
	{
		MAC_LOG_DBG("%s[%d]:leave %s:error:ipc_send_reply ! \n",__FILE__,__LINE__,__func__);
		return -1;
	}
	return 0;
}

int mac_static_get_bulk( struct mac_key *key_entry,struct mac_entry entry_buff[])
{
	struct hash_bucket *pbucket = NULL;
	struct mac_entry *pmac_entry = NULL;
	int cursor = 0;   
	int data_num = 0;
	int msg_num  = IPC_MSG_LEN/sizeof(struct mac_entry);
	int flag=0;
	
	/*The first query*/
	if(0==key_entry->vlanid)
	{
		HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
		{
			pmac_entry = (struct mac_entry *)pbucket->data;
		
			memcpy(&entry_buff[data_num++],pmac_entry,sizeof(struct mac_entry));
			if (data_num == msg_num)
			{
			 	return data_num;
			}
		}
		return data_num;
	}
	else
	{
		HASH_BUCKET_LOOP( pbucket, cursor, mac_static_table )
		{
			pmac_entry = (struct mac_entry *)pbucket->data;
			if(0==flag)
			{
				if ((memcmp(pmac_entry->key.mac, key_entry->mac, MAC_LEN) == 0)\
						&&(pmac_entry->key.vlanid == key_entry->vlanid))
				{
					flag=1;
				}
				continue;
			}
			else
			{
				memcpy(&entry_buff[data_num++],pmac_entry,sizeof(struct mac_entry));
				if (data_num == msg_num)
				{
					flag=0;
				 	return data_num;
				}
			}
		}
		flag=0;
		return data_num;
	}
}

int mac_static_reply_mac_info_bulk(void *pdata,struct ipc_msghdr_n *phdr)
{
	struct mac_key *key_entry=(struct mac_key *)pdata;
	int msg_num  = IPC_MSG_LEN/sizeof(struct mac_entry);
	struct mac_entry entry_buff[IPC_MSG_LEN/sizeof(struct mac_entry)];
	int ret;

	memset(entry_buff, 0, msg_num*sizeof(struct mac_entry));
	
	ret=mac_static_get_bulk(key_entry,entry_buff);
	if(ret>0)
	{
		//ret = ipc_send_reply_bulk(entry_buff, ret*sizeof(struct mac_entry), ret, phdr->sender_id,
	    //                    phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
        ret = ipc_send_reply_n2(entry_buff, ret*sizeof(struct mac_entry), ret, phdr->sender_id,
                            phdr->module_id, phdr->msg_type, phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
	}
	else
	{
		//ret = ipc_send_noack(ERRNO_NOT_FOUND, phdr->sender_id, phdr->module_id, phdr->msg_type,
	    //                    phdr->msg_subtype, phdr->msg_index);
     //   ret = l2_msg_send_noack(ERRNO_NOT_FOUND, phdr, phdr->msg_index);
		ret = ipc_send_reply_n2(NULL, 0, 0, phdr->sender_id,
	                        phdr->module_id, phdr->msg_type, phdr->msg_subtype, 0,0, IPC_OPCODE_NACK);
	}
	return 0;
}

