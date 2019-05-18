
#ifndef IPRAN_SNMP_DATA_CACHE
#define IPRAN_SNMP_DATA_CACHE

#ifdef __cplusplus
extern          "C"
{
#endif

#include <time.h>
#include <lib/hash1.h>
#include "snmp_config_table.h"

#include "net-snmp-config.h"
#include "types.h"
#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"


#define SNMP_CACHE_TABLE_REFRESH_TIME       	(1000*1000)
#define SNMP_CACHE_TABLE_REFRESH_TIME_MAX   	(1000*1000)
#define SNMP_CACHE_TABLE_DATA_AGING_TIME_MAX	(1000*1000*10)	//10s

#define SNMP_CACHE_TABLE_NOTOIPC_TIME       1     // 1s  must < ipc timeout


#define EIGHT_BITS_EXCHANGE(T,V)                \
{                                               \
    (T) |= (uint8_t)(((V) & 0x80)>>7);          \
    (T) |= (uint8_t)(((V) & 0x40)>>5);          \
    (T) |= (uint8_t)(((V) & 0x20)>>3);          \
    (T) |= (uint8_t)(((V) & 0x10)>>1);          \
    (T) |= (uint8_t)(((V) & 0x08)<<1);          \
    (T) |= (uint8_t)(((V) & 0x04)<<3);          \
    (T) |= (uint8_t)(((V) & 0x02)<<5);          \
    (T) |= (uint8_t)(((V) & 0x01)<<7);          \
}

#define FOUR_BITS_EXCHANGE(T,V)                 \
{                                               \
    T |= (((V)&0xf0)>>4);                       \
    T |= (((V)&0x0f)<<4);                       \
}

enum ipran_snmp_action
{
    IPRAN_SNMP_GETNEXT = 0,
    IPRAN_SNMP_GET
};

enum ipran_snmp_result
{
    IPRAN_SNMP_CACHE_OK = 0,
    IPRAN_SNMP_CACHE_FAILED = -1
};

enum ipran_snmp_cache_lock
{
    SNMP_CACHE_UNLOCK = 0,
    SNMP_CACHE_LOCK
};

struct ipran_snmp_data_cache
{
    struct list     *data_list;
    int             node_size;

    struct timeval  timer_forget;

    /*by lipf add, 2018/9/17*/
    int             data_lock;

    /*by caojt add, 2018-05-31*/
    int             nullkey;
    /*by caojt add end*/

    int (*cb_get_data_from_ipc)(struct ipran_snmp_data_cache *, void *);
    void           *(*cb_data_lookup)(struct ipran_snmp_data_cache *, const int, const void *);

    /*by caojt add, 2018-05-31, new module use, replace cb_get_data_from_ipc*/
    int (*cb_data_get)(struct ipran_snmp_data_cache *, void *, int);
    int (*cb_data_cmp)(const void *, const void *);
    /*by caojt add end*/
};

extern int ipran_mib_tables_list_check(void *para);

extern struct ipran_snmp_data_cache *snmp_cache_init(int, void *, void *);
extern int                           snmp_cache_add(struct ipran_snmp_data_cache *, const void *, const int);
extern int                           snmp_cache_free(void *);
extern int                           snmp_cache_delete_all(struct ipran_snmp_data_cache *);
extern void                         *snmp_cache_get_data_by_index(struct ipran_snmp_data_cache *, int, const void *);

/*by caojt add, 2018-05-31, new module use*/
extern struct ipran_snmp_data_cache *snmp_cache_init2(int, void *, void *, void *);
extern void                         *snmp_cache_data_get(struct ipran_snmp_data_cache *, int, const void *);
extern int                           snmp_cache_valfilter_add(struct ipran_snmp_data_cache *, const void *, const int);
extern int                           snmp_cache_nullkey_set(struct ipran_snmp_data_cache *, int);

extern unsigned int                  snmp_req_msg_flag(int nullkey, int exact);
/*by caojt add end*/

#define MSDH_OVER_SNMP_BUFFER   1200
struct ipran_msdhoversnmp
{
    unsigned char   msdhtodevice[MSDH_OVER_SNMP_BUFFER];
    unsigned char   msdhtoPC[MSDH_OVER_SNMP_BUFFER];
};
extern struct ipran_msdhoversnmp    g_msdhoversnmp;


int snmp_scalar_info_get(void * pdata, int data_len, int module_id, int subtype);
int snmp_table_info_get_bulk(struct ipran_snmp_data_cache *cache, void *index, int index_len,
	int info_len, int module_id, int subtype);





enum {
	MIB_REGISTER_FAILED,		
	MIB_REGISTER_EXIST,
	MIB_REGISTER_SUCCESS
};


#define TABLE_OID_LEN					MAX_OID_LEN/8		//16位OID目前完全够用
#define TREE_ROOT_OID					1					//root节点OID
#define TREE_DEF_CHCHE_CHECK_DEPTH		7					//默认的内存检测深度

#define MIB_TREE_CACHE_FIFO_MAX         50



typedef struct mib_tree_node_data
{
	size_t table_index_len;				//表索引的长度	
	size_t table_data_len;				//表数据的长度
	
	struct list *table_list;			//表数据链表
	
	/* get bulk operate */
	int module_id;						//目的模块id	
	int msg_type;	
	int sub_type;	
	int operate;

	struct timeval data_product_time;	//表缓存数据的最后产生时间

    int data_lock;
	
} MIB_TREE_NODE_DATA;


typedef struct mib_tree_node
{
	oid oid[TABLE_OID_LEN];				//16位现在已完全够用
	
	size_t oid_len;						//使用的OID长度，也可以表示本节点位于的树深度	

	MIB_TREE_NODE_DATA data;

	int cache_null;						//TRUE:节点中无缓存;FALSE:节点中有缓存

	struct mib_tree_node *pParentNode;
	
	struct list *pSonNodesList;
	
} MIB_TREE_NODE;


typedef struct _mib_tree_cache_fifo
{
    MIB_TREE_NODE *queue[MIB_TREE_CACHE_FIFO_MAX];

    int head;
    int tail;

    size_t cnt;
    
} MIB_TREE_CACHE_FIFO;



typedef struct _mib_tree
{
	MIB_TREE_NODE *pRoot;

	size_t node_cnt;

	//结合深度，可提高内存管理的效率，避免轮询无效节点
	uint32_t depth;						//深度
	uint32_t depth_cache_start;			//有缓存数据的起始深度
	uint32_t depth_cache_end;			//有缓存数据的最大深度

    //标识是否有数据缓存
    int has_cache;

    MIB_TREE_CACHE_FIFO *pFifo;
	
} MIB_TREE;





extern void snmp_mib_tables_tree_init(void);
extern MIB_TREE_NODE *snmp_mib_tables_tree_node_new(void);
extern MIB_TREE_NODE *snmp_mib_tables_tree_node_lookup(oid *pOid, size_t oid_len);
extern int snmp_mib_tables_tree_node_del(oid *pOid, size_t oid_len);
extern void snmp_mib_tables_tree_check(MIB_TREE_NODE *pNode);
extern void snmp_mib_tables_tree_node_cache_check(MIB_TREE_NODE *pNode);

extern MIB_TREE_CACHE_FIFO *snmp_mib_tree_cache_fifo_new(void);
extern int snmp_mib_tree_cache_fifo_check(void *para);

extern int isFull(MIB_TREE_CACHE_FIFO *pFifo);
extern int isEmpty(MIB_TREE_CACHE_FIFO *pFifo);
extern int pushFifo(MIB_TREE_CACHE_FIFO *pFifo, MIB_TREE_NODE *pNode);
extern MIB_TREE_NODE *pullFifo(MIB_TREE_CACHE_FIFO *pFifo);
extern MIB_TREE_NODE *getFifoData(MIB_TREE_CACHE_FIFO *pFifo, int pos);


#if 1
int snmp_mib_register(MIB_TREE_NODE *pTableReg);
void snmp_mib_unregister(MIB_TREE_NODE *pTableReg);
//void *snmp_table_get_data_by_index(oid *pTableOid, int exact, void *pIndex, void *pIndexNext);
//void snmp_mib_init(void);
#endif



#ifdef __cplusplus
}
#endif
#endif

