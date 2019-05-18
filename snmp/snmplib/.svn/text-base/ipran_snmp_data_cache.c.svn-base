#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"
#include "ipran_snmp_data_cache.h"
#include "snmp_config_table.h"

#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/ifm_common.h>
#include <lib/pkt_buffer.h>
#include <lib/msg_ipc_n.h>
#include <lib/types.h>
#include <lib/linklist.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/inet_ip.h>
#include <lib/log.h>
#include <lib/hptimer.h>
//#include "lib/snmp_common.h"


//mib表链表，用于存放注册的mib信息，进行统一的内存管理
struct list g_mib_talbes_list = {.head = NULL, .tail = NULL, .count = 0, .cmp = NULL, .del = NULL};

struct ipran_msdhoversnmp   g_msdhoversnmp = {0};


/**
 * @brief      : mib表链表的定时内存管理
 * @param[in ] : 
 * @param[out] : 
 * @return     : 
 * @author     : Lipf
 * @date       : 2019年3月6日
 * @note       :
 */
int ipran_mib_tables_list_check(void *para)
{
	uint32_t diff = 0;
	
	struct listnode *node = NULL;

	struct ipran_snmp_data_cache *cache = NULL;

	struct timeval time_now = {.tv_sec = 0, .tv_usec = 0};
	
	if(g_mib_talbes_list.count)
	{
		for (node = listhead(&g_mib_talbes_list); node; node = listnextnode (node))
		{
			cache = (struct ipran_snmp_data_cache *)node->data;
			if(cache)
			{
				if(SNMP_CACHE_UNLOCK == cache->data_lock)
				{
					gettimeofday(&time_now, NULL);
					diff = (time_now.tv_sec - cache->timer_forget.tv_sec) * 1000 * 1000 + time_now.tv_usec - cache->timer_forget.tv_usec;

					if(diff > SNMP_CACHE_TABLE_DATA_AGING_TIME_MAX)	//10s释放缓存
					{
						snmp_cache_delete_all(cache);
					}
				}
			}
		}
	}
}


struct ipran_snmp_data_cache *snmp_cache_init(int node_size,
                                              void *cb_get_data_from_ipc,
                                              void *cb_data_lookup)
{
    if (0 == node_size || NULL == cb_get_data_from_ipc || NULL == cb_data_lookup)
    {
        return NULL;
    }

    struct ipran_snmp_data_cache *cache = XCALLOC(0, sizeof(struct ipran_snmp_data_cache));

    if (cache == NULL)
    {
        return NULL;
    }

    memset(cache, 0, sizeof(struct ipran_snmp_data_cache));
    cache->node_size = node_size;
    cache->cb_get_data_from_ipc = cb_get_data_from_ipc;
    cache->cb_data_get = NULL;
    cache->cb_data_lookup = cb_data_lookup;
    cache->data_list = list_new();
    cache->data_list->del = (void (*)(void *))snmp_cache_free;
    cache->data_list->cmp = NULL;
    gettimeofday(&cache->timer_forget, NULL);
    cache->nullkey = 0;

	//将注册mib表放入链表中
	listnode_add(&g_mib_talbes_list, (void *)cache);

    return cache;
}

int snmp_cache_add(struct ipran_snmp_data_cache *cache,
                   const void *node_value,
                   const int node_size)
{
    if (NULL == node_value || node_size != cache->node_size)
    {
        zlog_debug(SNMP_DBG_CACHE, "%s[%d]:'%s' cache->node_size not equals node_size\n", __FILE__, __LINE__, __func__);
        return (IPRAN_SNMP_CACHE_FAILED);
    }

    void *node_add = NULL;

    node_add = XCALLOC(0, cache->node_size);
    memcpy(node_add, node_value, cache->node_size);
    listnode_add(cache->data_list, node_add);

    return (IPRAN_SNMP_CACHE_OK);
}

int snmp_cache_free(void *node_value)
{
    XFREE(0, node_value);

    return (IPRAN_SNMP_CACHE_OK);
}

int snmp_cache_delete_all(struct ipran_snmp_data_cache *cache)
{
    list_delete_all_node(cache->data_list);
    cache->data_lock = SNMP_CACHE_UNLOCK;

    return (IPRAN_SNMP_CACHE_OK);
}

void *snmp_cache_get_data_by_index(struct ipran_snmp_data_cache *cache,
                                   const int exact,
                                   const void *index_value)
{
    void           *pmatch_data = NULL;
    void           *index_temp;
    struct timeval  time_now;
    u_int32_t       diff;
    int             ret = 0;
    int             tmp_nullkey;

    if (NULL == cache || NULL == cache->data_list
            || NULL == cache->cb_get_data_from_ipc || NULL == cache->cb_data_lookup)
    {
        return NULL;
    }

	gettimeofday(&time_now, NULL);
	diff = (time_now.tv_sec - cache->timer_forget.tv_sec) * 1000 * 1000 + time_now.tv_usec - cache->timer_forget.tv_usec;
	cache->timer_forget= time_now;

	if(diff > SNMP_CACHE_TABLE_REFRESH_TIME)
	{
		if (0 != cache->data_list->count)
		{
			snmp_cache_delete_all(cache);
		}
		
		cache->data_lock = SNMP_CACHE_UNLOCK;
	}
	

    if (0 == cache->data_list->count)
    {
        if (SNMP_CACHE_UNLOCK == cache->data_lock)
        {
            index_temp = XCALLOC(0, cache->node_size);
            memset(index_temp, 0, cache->node_size);

            tmp_nullkey = cache->nullkey;
            cache->nullkey = 1;
            ret = cache->cb_get_data_from_ipc(cache, index_temp);
            cache->nullkey = tmp_nullkey;
			gettimeofday(&cache->timer_forget, NULL);

            if (FALSE == ret)
            {
                cache->data_lock = SNMP_CACHE_LOCK;

                free(index_temp);
                return NULL;
            }

            free(index_temp);
        }
        else
        {
            return NULL;
        }
    }

    while (1)
    {
        pmatch_data = cache->cb_data_lookup(cache, exact, index_value);

        if (NULL != pmatch_data)
        {
            return pmatch_data;
        }
        else
        {
            if (NULL == cache->data_list->tail || NULL == cache->data_list->tail->data)
            {
                cache->data_lock = SNMP_CACHE_LOCK;
                return NULL;
            }

            if (SNMP_CACHE_UNLOCK == cache->data_lock)
            {
                ret = cache->cb_get_data_from_ipc(cache, cache->data_list->tail->data);
				gettimeofday(&cache->timer_forget, NULL);

                if (FALSE == ret)
                {
                    cache->data_lock = SNMP_CACHE_LOCK;

                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}

struct ipran_snmp_data_cache *snmp_cache_init2(int node_size,
                                               void *cb_data_get,
                                               void *cb_data_lookup,
                                               void *cb_data_cmp)
{
    if (0 == node_size || NULL == cb_data_get || NULL == cb_data_lookup)
    {
        return NULL;
    }

    struct ipran_snmp_data_cache *cache = XCALLOC(0, sizeof(struct ipran_snmp_data_cache));

    if (cache == NULL)
    {
        return NULL;
    }

    memset(cache, 0, sizeof(struct ipran_snmp_data_cache));
    cache->node_size = node_size;
    cache->cb_get_data_from_ipc = NULL;
    cache->cb_data_get = cb_data_get;
    cache->cb_data_lookup = cb_data_lookup;
    cache->data_list = list_new();
    cache->data_list->del = (void (*)(void *))snmp_cache_free;
    cache->data_list->cmp = (int (*)(void *, void *))cb_data_cmp;
    gettimeofday(&cache->timer_forget, NULL);
    cache->nullkey = 0;

    return cache;
}

int snmp_cache_valfilter_add(struct ipran_snmp_data_cache *cache,
                             const void *node_value,
                             const int node_size)
{
    int              rc;
    void            *node_add = NULL;
    struct listnode *n;

    if (NULL == node_value || node_size != cache->node_size)
    {
        return IPRAN_SNMP_CACHE_FAILED;
    }

    if (NULL != cache->data_list->cmp)
    {
        for (n = cache->data_list->head; n; n = n->next)
        {
            rc = (cache->data_list->cmp)(node_value, n->data);

            if (rc == 0)
            {
                return IPRAN_SNMP_CACHE_FAILED;
            }
        }
    }

    node_add = XCALLOC(0, cache->node_size);
    memcpy(node_add, node_value, cache->node_size);
    listnode_add(cache->data_list, node_add);

    return IPRAN_SNMP_CACHE_OK;
}


/*
 * replcae func snmp_cache_get_data_by_index, use func(cb_data_get), variable(null_key)
 */
void *snmp_cache_data_get(struct ipran_snmp_data_cache *cache,
                          const int exact,
                          const void *index_value)
{
    void           *pmatch_data = NULL;
    struct timeval  time_now;
    u_int32_t       diff;
    int             ret = 0;
    int             tmp_nullkey;

    if (NULL == cache || NULL == cache->data_list
            || NULL == cache->cb_data_get || NULL == cache->cb_data_lookup)
    {
        return NULL;
    }

    gettimeofday(&time_now, NULL);
    diff = (time_now.tv_sec - cache->timer_forget.tv_sec) * 1000 * 1000 + time_now.tv_usec - cache->timer_forget.tv_usec;
	cache->timer_forget = time_now;
	
    if (diff > SNMP_CACHE_TABLE_REFRESH_TIME)
    {
        if (0 != cache->data_list->count)
        {
            snmp_cache_delete_all(cache);
        }

        cache->data_lock = SNMP_CACHE_UNLOCK;
    }

    // frist lookup
    if (0 != cache->data_list->count)
    {
        pmatch_data = cache->cb_data_lookup(cache, exact, index_value);

        if (NULL != pmatch_data)
        {
            return pmatch_data;
        }

        snmp_cache_delete_all(cache);
    }

    if (SNMP_CACHE_LOCK == cache->data_lock)
    {
        return NULL;
    }

    ret = cache->cb_data_get(cache, index_value, exact);
	gettimeofday(&cache->timer_forget, NULL);

    if (FALSE == ret)
    {
        cache->data_lock = SNMP_CACHE_LOCK;

        return NULL;
    }

    // second lookup
    if (0 != cache->data_list->count)
    {
        pmatch_data = cache->cb_data_lookup(cache, exact, index_value);

        if (NULL != pmatch_data)
        {
            return pmatch_data;
        }

        snmp_cache_delete_all(cache);
    }

    return NULL;
}

int snmp_cache_nullkey_set(struct ipran_snmp_data_cache *cache, int nullkey)
{
    cache->nullkey = nullkey;

    return IPRAN_SNMP_CACHE_OK;
}

unsigned int snmp_req_msg_flag(int nullkey, int exact)
{
    if (1 == nullkey)
    {
        return 1;   // get all
    }
    else if (1 == exact)
    {
        return 2;   // get current, include current, from == current start
    }

    return 0;       // get current next, include current, from >= current start
}



/**
 * @brief      : mib标量获取接口函数
 * @param[in ] : pdata，用于存放获取的标量数据
 * @param[in ] : data_len，要获取的标量数据的长度
 * @param[in ] : module_id
 * @param[in ] : subtype
 * @param[out] : 
 * @return     : void *
 * @author     : Lipf
 * @date       : 2018/2/26
 * @note       :
 */

int snmp_scalar_info_get(void * pdata, int data_len, int module_id, int subtype)
{
	/*return ipc_send_common_wait_reply(NULL, 0, 1, module_id, MODULE_ID_SNMPD,
		IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0);*/

	if(NULL == pdata)
	{
		return FALSE;
	}

	struct ipc_mesg_n* pmsg = ipc_sync_send_n2(NULL, 0,  0, module_id, MODULE_ID_SNMPD,
		IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET, 0, 1000); 

	if(NULL == pmsg)
	{
		return FALSE;
	}

	if((data_len != pmsg->msghdr.data_len) || (pmsg->msghdr.opcode != IPC_OPCODE_REPLY))
	{
		mem_share_free(pmsg, module_id);
		return FALSE;
	}

	memcpy((uint8_t *)pdata, pmsg->msg_data, pmsg->msghdr.data_len);  

    mem_share_free(pmsg, module_id);
	return TRUE;
}




/**
 * @brief      : mib获取表接口函数
 * @param[in ] : cache，表缓存
 * @param[in ] : index，索引
 * @param[in ] : index_len，索引长度
 * @param[in ] : info_len，info长度
 * @param[in ] : module_id
 * @param[in ] : subtype
 * @param[out] : 
 * @return     : 
 * @author     : Lipf
 * @date       : 2018/2/26
 * @note       :
 */

int snmp_table_info_get_bulk(struct ipran_snmp_data_cache *cache, void *index, int index_len,
	int info_len, int module_id, int subtype)
{
    /*struct ipc_mesg *pmesg = ipc_send_common_wait_reply1(index, index_len, 1 , module_id, 
		MODULE_ID_SNMPD, IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET_BULK, 0);

    if (pmesg)
    {
        *pdata_num = pmesg->msghdr.data_num;
        return (void *)pmesg->msg_data;
    }*/

	int data_num = 0;
	int ret = 0;

	struct ipc_mesg_n* pmsg = ipc_sync_send_n2(index, index_len,  1, module_id, MODULE_ID_SNMPD,
		IPC_TYPE_SNMP, subtype, IPC_OPCODE_GET_BULK, 0, 1000); 

	if(NULL == pmsg)
	{
		return FALSE;
	}

	data_num = pmsg->msghdr.data_num;

	if (0 == data_num)
    {
		mem_share_free(pmsg, MODULE_ID_SNMPD);
        return FALSE;
    }
    else
    {
        for (ret = 0; ret < data_num; ret++)
        {
            snmp_cache_add(cache, (pmsg->msg_data + ret*info_len) , info_len);
        }

		mem_share_free(pmsg, MODULE_ID_SNMPD);        
    }
	
    return TRUE;
}




#if 0

/**********************************************************************************************/
/**********************************************************************************************/



/* */
struct snmp_mib_tables_hash *snmp_tables_hash = NULL;


/* hash key 产生方法 */
static unsigned int snmp_tables_hash_compute_hash(void *hash_key)
{
	if(NULL == hash_key)
	{
		zlog_debug (SNMP_DBG_CACHE, "%s[%d] -> %s : hash_key is NULL!\n",\
			__FILE__, __LINE__, __func__);
		return 0;
	}

	unsigned int key = 0;
	unsigned int key_temp[4] = {0, 0, 0, 0};
	int i = 0;
	oid *pOid = (oid *)hash_key;

	/* 第1个字节，由前6个OID相加，然后取低8位 */
	/* 第2个字节，由第7个OID各字节相加，然后取低8位 */
	/* 第3个字节，由第8-11个OID相加，然后取低8位 */
	/* 第4个字节，由第12-16个OID相加，然后取低8位 */
	
	for(i = 0; i < 6; i++)
	{
		key_temp[0] += (unsigned int)pOid[i];
	}

	for(i = 0; i < sizeof(oid); i++)
	{
		key_temp[1] += (pOid[6] >> (8 * (sizeof(oid) - i + 1)));
	}
	
	for(i = 7; i < 11; i++)
	{
		key_temp[2] += (unsigned int)pOid[i];
	}

	
	for(i = 11; i < 16; i++)
	{
		key_temp[3] += (unsigned int)pOid[i];
	}

	for(i = 0; i < 4; i++)
	{
		key_temp[i] &= 0x000000ff;
		key += (key_temp[i] << (8 * (3-i)));
	}

    return key;
}


/* hash key 比较方法 */
static int snmp_tables_hash_compare_hash(void *item, void *hash_key)
{
	struct hash_bucket *pbucket = (struct hash_bucket *)item;

	if(NULL == pbucket || NULL == hash_key)
	{
		return SNMP_ERROR;
	}

	if (*((unsigned int *)pbucket->hash_key) == *((unsigned int*)hash_key))
    {
        return SNMP_OK;
    }
    return SNMP_ERROR;
}

static void snmp_tables_hash_init(void)
{
	hios_hash_init(&snmp_tables_hash, 16, snmp_tables_hash_compute_hash, snmp_tables_hash_compare_hash);
	//snmp_tables_hash.talbes_hash_lock = SNMP_CACHE_UNLOCK;
}


struct snmp_mib_table_info *snmp_tables_hash_lookup(oid *pOid)
{
	struct hash_bucket *cursor = NULL;

	cursor = hios_hash_find(&snmp_tables_hash, (void *)pOid);
	if(NULL == cursor)
		return NULL;

	return (struct snmp_mib_table_info *)cursor->data;
}


/* 在U0的hash表中添加新的表项 */
static int snmp_tables_hash_add(struct snmp_mib_table_info *pinfo)
{
	struct hash_bucket *pbucket = NULL;

	struct snmp_mib_table_info *pinfo_new = (struct snmp_mib_table_info *)XMALLOC(MTYPE_SNMPD_CONFIG, sizeof(struct snmp_mib_table_info));
	if(!pinfo_new)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return SNMP_MALLOC_FAIL;
	}

	memcpy(pinfo_new, pinfo, sizeof(struct snmp_mib_table_info));

	pbucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == pbucket)
	{
		zlog_err ("%s[%d] XMALLOC error\n", __func__, __LINE__);
		return SNMP_MALLOC_FAIL;
	}

	if(snmp_tables_hash_lookup(pinfo->oid))
	{
		zlog_err("%s[%d] : add repeated(", __func__, __LINE__);

		int i = 0;
		for(i = 0; i < TABLE_OID_LEN; i++)
		{
			if(i == (TABLE_OID_LEN - 1))
			{
				printf("%d", pinfo->oid[i]);
			}
			else
			{
				printf("%d.", pinfo->oid[i]);;
			}
		}
		zlog_err(")\n");
		return SNMP_OK;
	}
	
	pbucket->next = NULL;
	pbucket->prev = NULL;
	pbucket->hashval = 0;	
	pbucket->hash_key = (void *)pinfo->oid;
	pbucket->data = (void *)pinfo_new;
	
	if(hios_hash_add(&snmp_tables_hash, pbucket) != 0)
	{
		XFREE(MTYPE_SNMPD_CONFIG, pinfo);
		XFREE(MTYPE_HASH_BACKET, pbucket);
		zlog_debug (SNMP_DBG_CACHE, "%s[%d] hash table has %d entries\n", __func__, __LINE__,
			snmp_tables_hash->talbes_hash.num_entries);
		return SNMP_OVERSIZE;
	}
	return SNMP_OK;
}




static void snmp_tables_hash_del(oid *pOid)
{
	struct hash_bucket *cursor = NULL;
	struct snmp_mib_table_info *pinfo = NULL;

	cursor = hios_hash_find(&snmp_tables_hash, (void *)pOid);
	if(NULL == cursor)
		return;

	pinfo = (struct snmp_mib_table_info *)cursor->data;
		
	XFREE(MTYPE_SNMPD_CONFIG, pinfo);
	hios_hash_delete(&snmp_tables_hash, cursor);
	XFREE(MTYPE_HASH_BACKET, cursor);
}



#if 0

static int snmp_tables_hash_node_add(struct snmp_mib_table_info *pTableNewReg)
{
	if(NULL != snmp_mib_node_lookup(pTableNewReg->oid))
		return MIB_REGISTER_EXIST;
	
	struct snmp_mib_table_info *pTableNewAdd = (struct snmp_mib_table_info *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct snmp_mib_table_info));
	if(NULL == pTableNewAdd)
	{
		zlog_err("%s[%d] -> %s : calloc error!\n", __FILE__, __LINE__, __func__);
		return MIB_REGISTER_FAILED;
	}
	memcpy(pTableNewAdd, pTableNewReg, sizeof(struct snmp_mib_table_info));
	listnode_add(snmp_tables_hash, pTableNewAdd);
	return MIB_REGISTER_SUCCESS;
}


static void snmp_tables_hash_node_free(struct snmp_mib_table_info *pTalbeInfo)
{
	XFREE (0, pTalbeInfo);
}

static void snmp_tables_hash_node_del(struct snmp_mib_table_info *pTalbeInfo)
{
	struct listnode  *node, *nnode;
	struct snmp_mib_table_info *pTableRegFound;

	int i = 0;
	
	for(ALL_LIST_ELEMENTS(snmp_tables_hash, node, nnode, pTableRegFound))
	{		
		for(i= 0; i < TABLE_OID_LEN; i++)
		{
			if(pTalbeInfo[i] != pTableRegFound->oid[i])
			{
				return;
			}
		}
		
		list_free(pTableRegFound->table_list);
		list_delete_node(snmp_tables_hash, node);
		snmp_tables_hash->del(pTableRegFound);		
	}	
}

static void snmp_tables_hash_node_del_all(void)
{
	list_delete_all_node(snmp_tables_hash);
}

static struct snmp_mib_table_info *snmp_tables_hash_node_lookup(oid *pTableOid)
{
	struct listnode  *node, *nnode;
	struct snmp_mib_table_info *pTableRegFound;

	int i = 0;

	if(NULL == pTableOid)
	{
		return NULL;		
	}
	
	for(ALL_LIST_ELEMENTS(snmp_tables_hash, node, nnode, pTableRegFound))
	{
		for(i= 0; i < TABLE_OID_LEN; i++)
		{
			if(pTableOid[i] != pTableRegFound->oid[i])
			{
				return NULL;
			}
		}

		return pTableRegFound;
	}
	
	return NULL;
}

#endif


/******ipc msg request******/
/* snmp mib 标量 查询 */

uint32_t *snmp_mib_scalar_get(int module_id, int ipc_type, int subtype)
{
    return ipc_send_common_wait_reply(NULL, 0, 1, module_id, MODULE_ID_SNMPD,
               ipc_type, subtype, 0, 0);
}



/* snmp mib 表 批量查询 */
static void *snmp_mib_table_get_bulk(void *pIindex, int oid, int table_id, int *pdata_num)
{
	struct snmp_mib_table_info *pTableReg = snmp_tables_hash_lookup(oid);
	
    struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pIindex, pTableReg->table_index_len, 1, 
		 pTableReg->module_id, MODULE_ID_SNMPD, pTableReg->msg_type, pTableReg->sub_type, pTableReg->operate, 0, 1000);
	
    if (NULL != pmesg)
	{
		*pdata_num = pmesg->msghdr.data_num;
		return (void *)pmesg->msg_data;
	}

    return NULL;
}


/* list operate */

static void snmp_table_data_node_add(struct snmp_mib_table_info *pTableReg, char *node)
{
	char *node_add = NULL;
	
	node_add = snmp_table_data_node_xcalloc(pTableReg->table_data_len);
	memcpy(node_add, node, pTableReg->table_data_len);

	pTableReg->data_lock = SNMP_CACHE_UNLOCK;
	pTableReg->table_list = NULL;
		
	listnode_add(pTableReg->table_list, node_add);
}


static char *snmp_table_data_node_lookup(int exact, void *pIndexInput, 
											struct snmp_mib_table_info *pTableReg)
{
	struct listnode  *node, *nnode;

	void *pTableFound;
	void *pIndex = (char *)XMALLOC(0, pTableReg->table_index_len);
	memset(pIndex, 0, pTableReg->table_index_len);
	
	for(ALL_LIST_ELEMENTS(pTableReg->table_list, node, nnode, pTableFound))
	{		
		if(0 == strncmp(pIndexInput, pIndex, pTableReg->table_index_len))	//全0，第一个节点
			return pTableReg->table_list->head->data;
		else
		{
			if(0 == strncmp(pIndexInput, node->data, pTableReg->table_index_len))
			{
				if(1 == exact)	//get
					return pTableFound;
				else
				{
					if(NULL == node->next)
						return NULL;
					else
						return node->next->data;
				}
			}
		}
	}
	XFREE (0, pIndex);
	return NULL;
}


static void snmp_table_node_del_all(struct snmp_mib_table_info *pTableReg)
{
	list_delete_all_node(pTableReg->table_list);
	pTableReg->data_lock = SNMP_CACHE_UNLOCK;
}

static void snmp_table_node_free(void *node)
{
	XFREE(0, node);
}

static int snmp_table_get_data_from_ipc(void *pIndex, struct snmp_mib_table_info *pTableReg)
{
	void *data_struct = NULL;
	int data_num = 0;
	int ret = 0;

	if((NULL == pIndex) || (NULL == pTableReg))
	{
		return FALSE;
	}
	
    /*struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pIndex, pTableReg->table_index_len, 1, 
		 pTableReg->module_id, MODULE_ID_SNMPD, pTableReg->msg_type, pTableReg->sub_type, pTableReg->operate, 0);
	
    if (NULL != pmesg)
	{
		data_num = pmesg->msghdr.data_num;
		return (void *)pmesg->msg_data;
	}*/
	
	struct ipc_mesg_n *pmesg = ipc_sync_send_n2(pIndex, pTableReg->table_index_len, 1, 
		 pTableReg->module_id, MODULE_ID_SNMPD, pTableReg->msg_type, pTableReg->sub_type, pTableReg->operate, 0, 1000);
	
    if(pmesg)
	{
		data_num = pmesg->msghdr.data_num;		
	
		if(0 == data_num)
		{
			zlog_debug(SNMP_DBG_CACHE, "%s[%d]: in function '%s' -> no table data return, read over\n",\
				__FILE__, __LINE__, __func__);
		}
		else
		{
			zlog_debug(SNMP_DBG_CACHE, "%s[%d]: in function '%s' -> success to get table data[num = %d]\n",\
				__FILE__, __LINE__, __func__, data_num);
			for(ret = 0; ret < data_num; ret++)	//store all data from ipc into list
			{
				snmp_table_data_node_add(pTableReg, data_struct);
				data_struct += pTableReg->table_data_len;			
			}
			
			gettimeofday(&pTableReg->data_product_time, NULL); //新数据老化时间
		}
		
		mem_share_free(pmesg, MODULE_ID_AAA);
	}
}

void *snmp_table_get_data_by_index(oid *pTableOid, int exact, void *pIndex, void *pIndexNext)
{
	int ret = 0;
	char *table_data = NULL;
	
	struct snmp_mib_table_info *pTableReg = snmp_tables_hash_lookup(pTableOid);
	if(NULL == pTableReg)
	{
		zlog_debug(SNMP_DBG_CACHE, "%s[%d]: in function '%s' -> no such registered table\n",\
			__FILE__, __LINE__, __func__);
		return NULL;
	}

	void           *pDataMatch = NULL;
    void           *pIndexTmp;
    struct timeval  time_now;
    u_int32_t       diff;
    int             tmp_nullkey;

    if ((NULL == pTableReg->table_list) || (0 == pTableReg->table_list->count))
    {
        return NULL;
    }

	gettimeofday(&time_now, NULL);
	diff = (time_now.tv_sec - pTableReg->data_product_time.tv_sec) * 1000 * 1000 +\
		(time_now.tv_usec - pTableReg->data_product_time.tv_usec);
	
	pTableReg->data_product_time = time_now;

	if(diff > SNMP_CACHE_TABLE_REFRESH_TIME)
	{
		if (0 != pTableReg->table_list->count)
		{
			//snmp_cache_delete_all(pTableReg);
			snmp_table_node_del_all(pTableReg);
		}
		
		pTableReg->data_lock = SNMP_CACHE_UNLOCK;
	}
	

    if (0 == pTableReg->table_list->count)
    {
        if (SNMP_CACHE_UNLOCK == pTableReg->data_lock)
        {
            pIndexTmp = XCALLOC(0, pTableReg->table_index_len);
            memset(pIndexTmp, 0, pTableReg->table_index_len);

            //tmp_nullkey = cache->nullkey;
            //cache->nullkey = 1;
            //ret = cache->cb_get_data_from_ipc(cache, index_temp);
            //cache->nullkey = tmp_nullkey;

			ret = snmp_table_get_data_from_ipc(pIndexTmp, pTableReg);
			gettimeofday(&pTableReg->data_product_time, NULL);

            if (FALSE == ret)
            {
                pTableReg->data_lock = SNMP_CACHE_LOCK;

                free(pIndexTmp);
                return NULL;
            }

            free(pIndexTmp);
        }
        else
        {
            return NULL;
        }
    }

    while (1)
    {
        pDataMatch = snmp_table_data_node_lookup(exact, pIndex, pTableReg);

        if (NULL != pDataMatch)
        {
            return (void *)pDataMatch;
        }
        else
        {
            if (NULL == pTableReg->table_list->tail || NULL == pTableReg->table_list->tail->data)
            {
                pTableReg->data_lock = SNMP_CACHE_LOCK;
                return NULL;
            }

            if (SNMP_CACHE_UNLOCK == pTableReg->data_lock)
            {
				ret = snmp_table_get_data_from_ipc(pTableReg->table_list->tail->data, pTableReg);
                //ret = cache->cb_get_data_from_ipc(pTableReg->table_list->tail->data, pTableReg);
				gettimeofday(&pTableReg->data_product_time, NULL);

                if (FALSE == ret)
                {
                    pTableReg->data_lock = SNMP_CACHE_LOCK;

                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
    }

    return NULL;
}

static int snmp_table_data_del(void *para)
{
	struct listnode  *node, *nnode;
	struct snmp_mib_table_info *pTableReg = NULL;

	struct timeval time_now = {.tv_sec = 0, .tv_usec = 0};

	if(0 == snmp_tables_hash->talbes_hash.hash_size)
	{
		return;
	}

	struct hash_bucket *hnode = NULL;
	void *cursor = NULL;

	for((node) = hios_hash_start(&snmp_tables_hash->talbes_hash, &cursor); hnode; (hnode) = hios_hash_next(&snmp_tables_hash->talbes_hash, &cursor))		
	{
		pTableReg = (struct snmp_mib_table_info *)(node->data);
		if(pTableReg == NULL)
		{
			return 1;
		}

		gettimeofday(&time_now, NULL);
		
		if(((time_now.tv_sec - pTableReg->data_product_time.tv_sec) * 1000 * 1000 +\
			(time_now.tv_usec - pTableReg->data_product_time.tv_usec)) > SNMP_CACHE_TABLE_DATA_AGING_TIME_MAX)
		{
			if(SNMP_CACHE_UNLOCK == pTableReg->data_lock)
			{
				snmp_table_node_del_all(pTableReg);
			}
		}
	}

	return 1;
}


void snmp_mib_init(void)
{
	snmp_tables_hash_init();
	
	//thread_add_timer(snmp_cli_master, snmp_table_data_del, NULL, 1);

	/* 定时释放mib数据缓存 */
	high_pre_timer_add("SnmpMibCacheProcTimer", LIB_TIMER_TYPE_LOOP, snmp_table_data_del, NULL, 5000);
}

int snmp_mib_register(struct snmp_mib_table_info *pTableReg)
{
	struct snmp_mib_table_info mib_reg_node;
	memset(&mib_reg_node, 0, sizeof(struct snmp_mib_table_info));
	memcpy(&mib_reg_node, pTableReg, sizeof(struct snmp_mib_table_info));

	mib_reg_node.table_list = list_new();
	mib_reg_node.table_list->del = (void (*) (void *))snmp_table_node_free;
	mib_reg_node.data_lock = SNMP_CACHE_UNLOCK;
	
	return snmp_tables_hash_add(&mib_reg_node);
}

void snmp_mib_unregister(struct snmp_mib_table_info *pTableReg)
{	
	snmp_tables_hash_del(pTableReg->oid);
}

#endif



/*************************************************************************************/
/************** 用树实现mib表的注册、解注册、数据处理、内存管理等 ********************/
/*************************************************************************************/


//mib表树，用于存放注册的mib信息，进行统一的内存管理
MIB_TREE g_mibTalbesTree = {
	.pRoot = NULL,
	.node_cnt = 0,
	.depth = 0,
	.depth_cache_start = 0,
	.depth_cache_end = 0,
	.has_cache = FALSE,
	.pFifo = NULL
};

void snmp_mib_tables_tree_init(void)
{
	g_mibTalbesTree.pRoot = snmp_mib_tables_tree_node_new();

	if(NULL == g_mibTalbesTree.pRoot)
	{
		fprintf(stdout, "mib tables tree init failed, exit\n");
		exit(0);
	}

	//root节点，OID = .1
	g_mibTalbesTree.pRoot->oid[0] = 1;
	g_mibTalbesTree.pRoot->oid_len = 1;

	g_mibTalbesTree.depth += 1;		//树深度加1
	g_mibTalbesTree.depth_cache_start = 7;
	g_mibTalbesTree.depth_cache_end = 0;

    g_mibTalbesTree.pFifo = snmp_mib_tree_cache_fifo_new();

    if(NULL == g_mibTalbesTree.pFifo)
    {
        fprintf(stdout, "Malloc mib tree cache fifo error!\n");
        exit(1);
    }
}


MIB_TREE_NODE *snmp_mib_tables_tree_node_new(void)
{
	MIB_TREE_NODE *pNode = (MIB_TREE_NODE *)XMALLOC(MTYPE_SNMPD_CONFIG, sizeof(MIB_TREE_NODE));

	if(!pNode)
	{
		return NULL;
	}

	memset(pNode, 0, sizeof(MIB_TREE_NODE));
	memset(&pNode->data, 0, sizeof(MIB_TREE_NODE_DATA));
	pNode->pParentNode = NULL;
	pNode->pSonNodesList = list_new();
	pNode->cache_null = TRUE;
	
	return pNode;
}


MIB_TREE_NODE *snmp_mib_tables_tree_node_lookup(oid *pOid, size_t oid_len)
{
	int depth = 0;
    int node_cnt = 0;

	/* 空树，或oid长度大于树的深度，返回NULL */
	if((NULL == g_mibTalbesTree.pRoot) || (0 == g_mibTalbesTree.node_cnt) || (oid_len > g_mibTalbesTree.depth))
	{
		return NULL;
	}

	MIB_TREE_NODE *pNode = g_mibTalbesTree.pRoot;

	MIB_TREE_NODE *pSonNode = NULL;
	
	struct listnode *pListNode = NULL;

	//i标识当前查询的树深度
	for(depth = 0; depth < (oid_len - 1); depth++)
	{
        //root节点不存在，或与root节点OID不匹配
		if((NULL == pNode) || (pOid[depth] != pNode->oid[depth]))
		{
			return NULL;
		}

		for (pListNode = listhead(pNode->pSonNodesList); pListNode; pListNode = listnextnode (pListNode), node_cnt++)
		{			
			pSonNode = (MIB_TREE_NODE *)pListNode->data;

			if(NULL == pSonNode)
			{
				return NULL;
			}

            //按照从小到大排序存放，若查询次数已超过该位OID，应停止
            if(node_cnt > pOid[depth+1])
            {
                return NULL;
            }
			
			if(pOid[depth+1] == pSonNode->oid[depth+1])
			{
				pNode = pSonNode;
				break;
			}
		}

		if(NULL == pListNode)
		{
			return NULL;
		}
	}

	return NULL;
}


int snmp_mib_tables_tree_node_add(MIB_TREE_NODE *pNode)
{
    struct listnode *pListNode = NULL;

    MIB_TREE_NODE *pCurrNode = NULL;
    
	if(NULL == pNode)
	{
		return FALSE;
	}

	if(TREE_ROOT_OID != pNode->oid[0])
	{
		fprintf(stdout, "Root node oid does not support, %d\n", pNode->oid[0]);
		return FALSE;
	}

	MIB_TREE_NODE *pParentNode = snmp_mib_tables_tree_node_lookup(pNode->oid, pNode->oid_len - 1);
	
	if(NULL == pParentNode)
	{
		pParentNode = snmp_mib_tables_tree_node_new();
		
		memcpy(pParentNode->oid, pNode->oid, pNode->oid_len - 1);
		pParentNode->oid_len = pNode->oid_len - 1;

		listnode_add(pParentNode->pSonNodesList, (void *)pNode);
		
		snmp_mib_tables_tree_node_add(pParentNode);
	}
	else
	{
		if(NULL == pParentNode->pSonNodesList)
		{
			pParentNode->pSonNodesList = list_new();            
		}

        if(pParentNode->pSonNodesList->count)
        {
            for (pListNode = listhead(pParentNode->pSonNodesList); pListNode; pListNode = listnextnode (pListNode))
    		{			
    			pCurrNode = (MIB_TREE_NODE *)pListNode->data;

    			if(NULL == pCurrNode)
    			{
    				return FALSE;
    			}
    			
    			if(pNode->oid[pNode->oid_len] < pCurrNode->oid[pNode->oid_len])
    			{
    				listnode_add_prev(pParentNode->pSonNodesList, pListNode, (void *)pNode);
                    
    				return TRUE;
    			}
                else if(pNode->oid[pNode->oid_len] < pCurrNode->oid[pNode->oid_len])
                {
                    fprintf(stderr, "Add table repeated! Please check OID!\n");
                    
                    return FALSE;
                }
    		}

            listnode_add(pParentNode->pSonNodesList, (void *)pNode);
        }
        else
        {
            listnode_add(pParentNode->pSonNodesList, (void *)pNode);
        }
	}

	
#if 0
	//根据oid
	MIB_TREE_NODE *pParentNode = g_mibTalbesTree.pRoot;
	MIB_TREE_NODE *pCurrNode = NULL;
	MIB_TREE_NODE *pNodeNew = NULL;

	int i = 0;

	//创建缺少的父节点
	for(i = 2; i < pNode->oid_len; i++)
	{
		pCurrNode = snmp_mib_tables_tree_node_lookup(pNode->oid, i);
		if(NULL == pCurrNode)
		{
			pNodeNew = snmp_mib_tables_tree_node_new();
			
			if(NULL == pNode)
			{
				return FALSE;
			}

			pNodeNew->pParentNode = pParentNode;
			memcpy(pNodeNew->oid, pNode->oid, i);

			listnode_add(pParentNode->pSonNodesList, (void *)pNodeNew);
		}

		pParentNode = pCurrNode;
	}

	//所有父节点添加完成，添加本节点
	listnode_add(pParentNode->pSonNodesList, (void *)pNode);
#endif

	//检查depth
	if(g_mibTalbesTree.depth_cache_end < pNode->oid_len)
	{
		g_mibTalbesTree.depth_cache_end = pNode->oid_len;
	}
	
	if(g_mibTalbesTree.depth_cache_start > pNode->oid_len)
	{
		g_mibTalbesTree.depth_cache_start = pNode->oid_len;
	}
	
	return TRUE;
}


//del 实际不会使用，暂不实现
int snmp_mib_tables_tree_node_del(oid *pOid, size_t oid_len)
{
	return TRUE;
}


//数定时检测
void snmp_mib_tables_tree_check(MIB_TREE_NODE *pNode)
{
	//MIB_TREE_NODE *pNode = g_mibTalbesTree.pRoot;

	MIB_TREE_NODE *pSonNode = NULL;
	
	struct listnode *pListNode = NULL;

    if((pNode->oid_len >= g_mibTalbesTree.depth_cache_end) || (pNode->oid_len >= g_mibTalbesTree.depth))
    {
        return;
    }

	for(pListNode = listhead(pNode->pSonNodesList); pListNode; pListNode = listnextnode(pListNode))
	{
        pSonNode = (MIB_TREE_NODE *)pListNode->data;
        
        if(NULL == pSonNode)
        {
            continue;
        }
        
		//节点深度大于或等于内存检测起始深度，检查内存
		if(pNode->oid_len >= g_mibTalbesTree.depth_cache_start)
		{
			snmp_mib_tables_tree_node_cache_check(pSonNode);
		}
        
        snmp_mib_tables_tree_check(pSonNode);
	}
}


//节点缓存管理
void snmp_mib_tables_tree_node_cache_check(MIB_TREE_NODE *pNode)
{
    if(NULL == pNode)
    {
        return;
    }

    uint32_t time_diff = 0;
    
    MIB_TREE_NODE_DATA *pData = &pNode->data;

    struct timeval time_now = {.tv_sec = 0, .tv_usec = 0};

    if(SNMP_CACHE_UNLOCK == pData->data_lock)   //处于解锁状态，才能检查缓存
    {
        gettimeofday(&time_now, NULL);

        time_diff = (time_now.tv_sec - pData->data_product_time.tv_sec) * 1000 * 1000 + \
            (time_now.tv_usec - pData->data_product_time.tv_usec);

        if(time_diff >= SNMP_CACHE_TABLE_DATA_AGING_TIME_MAX)
        {
            list_delete_all_node(pData->table_list);            
        }
    }    
}



//fifo init
MIB_TREE_CACHE_FIFO *snmp_mib_tree_cache_fifo_new(void)
{
    MIB_TREE_CACHE_FIFO *pFifo = (MIB_TREE_CACHE_FIFO *)XMALLOC(MTYPE_SNMPD_CONFIG, sizeof(MIB_TREE_CACHE_FIFO));

    if(pFifo)
    {
        pFifo->head = 0;
        pFifo->tail = 0;
        pFifo->cnt = 0;
    }

    return pFifo;
}


//mib tree 一个节点的内存管理
int snmp_mib_tree_cache_fifo_check(void *para)
{
    MIB_TREE_CACHE_FIFO *pFifo = g_mibTalbesTree.pFifo;

    MIB_TREE_NODE *pNode = NULL;

    int i = 0;

    for(i = 0; i < pFifo->cnt; i++)
    {
        pNode = getFifoData(pFifo, i);
        snmp_mib_tables_tree_node_cache_check(pNode);
    }
}


//fifo 满
int isFull(MIB_TREE_CACHE_FIFO *pFifo)
{
    if((0 == pFifo->head) && ((MIB_TREE_CACHE_FIFO_MAX - 1) == pFifo->tail))
    {
        return TRUE;
    }
    else if((pFifo->head - 1) == pFifo->tail)
    {
        return TRUE;
    }

    return FALSE;
}


//fifo 空
int isEmpty(MIB_TREE_CACHE_FIFO *pFifo)
{
    if(pFifo->head == pFifo->tail)
    {
        return TRUE;
    }

    return FALSE;
}


//压栈
int pushFifo(MIB_TREE_CACHE_FIFO *pFifo, MIB_TREE_NODE *pNode)
{
    //fifo满了，是pull第一个呢？还是push失败？
    if(TRUE == isFull(pFifo))
    {
        return FALSE;
    }

    if((pFifo->tail + 1) >= MIB_TREE_CACHE_FIFO_MAX)
    {
        pFifo->tail = 0;
    }

    pFifo->queue[pFifo->tail] = pNode;
    pFifo->cnt += 1;

    return TRUE;
}


//出栈
MIB_TREE_NODE *pullFifo(MIB_TREE_CACHE_FIFO *pFifo)
{
    MIB_TREE_NODE *pNode = pFifo->queue[pFifo->head];

    pFifo->queue[pFifo->head] = NULL;
    
    if((pFifo->head + 1) >= MIB_TREE_CACHE_FIFO_MAX)
    {
        pFifo->head = 0;
    }
    
    pFifo->cnt -= 1;

    return pNode;
}


//获取指定节点，pos是偏移地址
MIB_TREE_NODE *getFifoData(MIB_TREE_CACHE_FIFO *pFifo, int pos)
{
    return pFifo->queue[pFifo->head + pos];
}


//mib表注册
int snmp_mib_table_register(MIB_TREE_NODE *pTableReg)
{
	if(NULL == pTableReg)
	{
		return FALSE;
	}

	MIB_TREE_NODE *pNode = snmp_mib_tables_tree_node_new();

	memcpy(pNode->oid, pTableReg->oid, pTableReg->oid_len);
	pNode->oid_len = pTableReg->oid_len;

	memcpy(&pNode->data, &pTableReg->data, sizeof(MIB_TREE_NODE_DATA));
	
	return snmp_mib_tables_tree_node_add(pNode);
}


//mib表解注册，暂不会使用
void snmp_mib_table_unregister(MIB_TREE_NODE *pTableReg)
{	
	snmp_mib_tables_tree_node_del(pTableReg->oid, pTableReg->oid_len);
}


void *snmp_mib_table_get_data_from_ipc(MIB_TREE_NODE_DATA *pData, void *pIndex)
{
	;
}


void *snmp_mib_table_cache_find(struct list *list, void *pIndex, size_t index_len)
{
	;
}


void *snmp_mib_table_get_data_by_index(oid *pOid, size_t oid_len, void *pIndex)
{
	if(pOid && (oid_len > 0))
	{
		MIB_TREE_NODE *pNode = snmp_mib_tables_tree_node_lookup(pOid, oid_len);

		if(NULL == pNode)
		{
			printf("%s[%d] : not found table registered\n", __func__, __LINE__);
			return NULL;
		}

		return snmp_mib_table_cache_find(pNode->data.table_list, pIndex, pNode->data.table_index_len);
	}

	return NULL;
}



