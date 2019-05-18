/**
 * @file      : labelm.c
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年3月8日 14:49:02
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#include <stdlib.h>
#include "labelm.h"
#include <lib/memory.h>
#include <lib/hash1.h>
#include <lib/errcode.h>
#include <lib/mpls_common.h>

void label_table_init(unsigned int size);
struct label_info *label_table_node_create(uint32_t label, uint32_t module_id);
int label_table_node_add(struct label_info *pinfo);
int label_tabel_node_delete(uint32_t label);
int label_table_get_bulk(struct label_info *pindex, uint32_t index_flag, struct label_info *pbuf);


struct index_bitmap label_pool[LABEL_POOL_MAX];

/* 增加相关实现，用于记录申请该标签的模块信息 */

struct hash_table label_table;

static unsigned int label_table_compute_hash(void *hash_key)
{   
    uint32_t hash_value = 0;

    hash_value = *((uint32_t *)hash_key);
    return hash_value;
}

static int label_table_compare(void *item, void *hash_key)
{
    if((NULL == item) || (NULL == hash_key))
    {
        return ERRNO_FAIL;
    }

    if(*((uint32_t *)((struct hash_bucket *)item)->hash_key) == *((uint32_t *)hash_key))
    {
        return ERRNO_SUCCESS;
    }

    return ERRNO_FAIL;
}

void label_table_init(unsigned int size)
{
    hios_hash_init(&label_table, size, label_table_compute_hash, label_table_compare);
}

struct label_info *label_table_node_create(uint32_t label, uint32_t module_id)
{
    struct label_info *pinfo = NULL;
    pinfo = (struct label_info *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct label_info));
    if (NULL == pinfo)
    {
        return NULL;
    }
    pinfo->label = label;
    pinfo->module_id = module_id;
    pinfo->state = LABEL_INUSED;
    
    if (ERRNO_SUCCESS != label_table_node_add(pinfo))
    {
        XFREE(MTYPE_HASH_BACKET, pinfo);
        return NULL;
    }

    return pinfo;
}

int label_table_node_add(struct label_info *pinfo)
{
    struct hash_bucket *pitem = NULL;
    if (label_table.hash_size == label_table.num_entries)
    {
        return ERRNO_OVERSIZE;
    }

    pitem = (struct hash_bucket *)XCALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == pitem)
    {
        return ERRNO_MALLOC;
    }
    if(NULL == pitem->data)
    {
        pitem->hash_key = &(pinfo->label);
        pitem->data = pinfo;

        hios_hash_add(&label_table, pitem);
    }
    else
    {

    }

    return ERRNO_SUCCESS;

}

int label_tabel_node_delete(uint32_t label)
{
    struct hash_bucket *pitem = NULL;
    struct label_info *pinfo = NULL;

    pitem = hios_hash_find(&label_table, &label);
    if ((NULL == pitem) || (NULL == pitem->data))
    {
        return ERRNO_FAIL;
    }
    pinfo = pitem->data;

    hios_hash_delete(&label_table, pitem);

    XFREE(MTYPE_HASH_BACKET, pinfo);
    XFREE(MTYPE_HASH_BACKET, pitem);

    return ERRNO_SUCCESS;
}

int label_table_get_bulk(struct label_info *pindex, uint32_t index_flag, struct label_info *pbuf)
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct label_info *plabel_info   = NULL;
    int msg_num  = IPC_MSG_LEN / sizeof(struct label_info);
    int cursor;
    int data_num = 0;

    if (0 == index_flag)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, label_table)
        {
            plabel_info = (struct label_info *)pbucket->data;
            if (plabel_info == NULL)
            {
                continue;
            }
            memcpy(&pbuf[data_num], plabel_info, sizeof(struct label_info));
            if (++data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else
    {
        pbucket = hios_hash_find(&label_table, &pindex->label);
        if (pbucket)
        {
            for (data_num = 0; data_num < msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&label_table, pbucket);
                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }
                plabel_info = (struct label_info *)pnext->data;
                memcpy(&pbuf[data_num], plabel_info, sizeof(struct label_info));
                pbucket = pnext;
            }
        }
        else
        {

        }
    }
    return data_num;

}

/**
 * @brief      : 标签池初始化
 * @param[in ] :
 * @param[out] :
 * @return     :
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:49:59
 * @note       : 初始化 32 个 label pool，每个 pool 32k label，总共 1M label
 */
void label_pool_init(void)
{
    unsigned int num    = 1024;
    unsigned int bitmap = num * sizeof(int);
    int          i;

    for(i = 0; i < LABEL_POOL_MAX; i++)
    {
        label_pool[i].index_num   = 0;
        label_pool[i].index_size  = 32 * num;
        label_pool[i].pbitmap     = malloc(bitmap);
        label_pool[i].bitmap_size = num;
        memset(label_pool[i].pbitmap, 255, bitmap);
    }
    label_table_init(LABEL_POOL_MAX * bitmap);
    return;
}


/**
 * @brief      : 标签池申请
 * @param[in ] : type - 标签类型
 * @param[out] :
 * @return     : 成功返回标签值，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:52:31
 * @note       :
 */
static int label_pool_alloc(unsigned char type)
{
    unsigned int i;
    unsigned int j;
    unsigned int num = 0;
    unsigned int *pbitmap = label_pool[type].pbitmap;

    if (label_pool[type].index_num == label_pool[type].index_size)
    {
        return 0;
    }

    for (i=0; i<label_pool[type].bitmap_size; i++)
    {
        /* 如果 bitmap 为 0，代表此 bitmap 的位全已被分配，继续遍历 */
        if (*pbitmap == 0)
        {
            pbitmap++;

            continue;
        }

        for (j=0; j<32; j++)
        {
            if ((*pbitmap & (0x80000000>>j)) == 0 )
            {
                continue;
            }

            *pbitmap &= ~(0x80000000>>j);   // bitmap 位置 0
            num = j+i*32;                   // 计算索引值

            if (num == 0)
            {
                continue;
            }

            if (num > label_pool[type].index_size)
            {
               return 0;
            }
            else
            {
                label_pool[type].index_num++;

                return num;
            }
        }
    }

    return 0;
}


/**
 * @brief      : 标签池释放
 * @param[in ] : type  - 标签类型
 * @param[in ] : index - 标签值
 * @param[out] :
 * @return     : 成功返回 1，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:55:37
 * @note       :
 */
static int label_pool_free(unsigned char type, unsigned int index)
{
    int n;
    int i;
    unsigned int *pbitmap = label_pool[type].pbitmap;

    if (index >= (label_pool[type].index_size*LABEL_POOL_MAX))
    {
        return 0;
    }

    index   = index % (label_pool[type].index_size);
    n       = index / (32);
    i       = index % (32);
    pbitmap = pbitmap + n;

    *pbitmap |= 0x80000000>>i ;      // 索引释放，标志位置 1
    label_pool[type].index_num--;

    return 1;
}


/**
 * @brief      : 标签池获取
 * @param[in ] : type  - 标签类型
 * @param[in ] : index - 标签值
 * @param[out] :
 * @return     : 成功返回 1，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 14:57:52
 * @note       :
 */
static int label_pool_get(unsigned char type, unsigned int index)
{
    int n, i;
    unsigned int *pbitmap = label_pool[type].pbitmap;

    if (index >= (label_pool[type].index_size*LABEL_POOL_MAX))
    {
        return 0;
    }

    index = index % (label_pool[type].index_size);
    n     = index / (32);    // 计算索引在 pool 内的偏移量
    i     = index % (32);    // 精确计算索引所占用的 bit 位

    pbitmap = pbitmap + n;

    if ((*pbitmap & (0x80000000>>i)) != 0)
    {
        *pbitmap &= ~(0x80000000>>i); // bitmap 位置 0
        label_pool[type].index_num++;

        return 1;
    }

    return 0;
}


/**
 * @brief      : 分配比指定标签大的标签
 * @param[in ] : label_min - 指定标签值
 * @param[out] :
 * @return     : 成功返回标签值，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 15:00:27
 * @note       :
 */
int label_min_alloc(uint32_t label_min)
{
    int i;
    int j;
    int label1 = label_min - 15;    // label 从 16 开始

    i = label1/(32 * 1024);    // 找到对应的 pool
    if (i >= LABEL_POOL_MAX)
    {
        return 0;
    }

    for (j=i; j<LABEL_POOL_MAX; j++)
    {
        if (label_pool[j].index_num < label_pool[j].index_size)
        {
            return label_pool_alloc(j) + 16;
        }
    }

    return 0;
}


/**
 * @brief      : 标签释放
 * @param[in ] : label - 标签值
 * @param[out] :
 * @return     : 成功返回 1，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 15:02:40
 * @note       :
 */
int label_free(uint32_t label)
{
    int i;
    int label1 = label - 15;    // label 从 1 开始

    i = label1/(32 * 1024);     // 找到对应的 pool
    if (i >= LABEL_POOL_MAX)
    {
        return 0;
    }
    
    label_tabel_node_delete(label);//clear the date in label hash table
    
    return label_pool_free(i, label1);
}


/**
 * @brief      : 标签申请
 * @param[in ] :
 * @param[out] :
 * @return     : 成功返回标签值，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 15:03:53
 * @note       :
 */
int label_alloc(uint32_t module_id)
{
    int i;
    int label = 0;

    for (i=0; i<LABEL_POOL_MAX; i++)
    {
        if (label_pool[i].index_num < label_pool[i].index_size)
        {
            
            label = label_pool_alloc(i) + 15;
            
            label_table_node_create(label, module_id);//recode label in the label hash table
            
            return label;
        }
    }

    return 0;
}


/**
 * @brief      : 标签获取
 * @param[in ] : label - 标签值
 * @param[out] :
 * @return     : 成功返回标签值，否则返回 0
 * @author     : ZhangFj
 * @date       : 2018年3月8日 15:05:11
 * @note       :
 */
int label_get(uint32_t label, uint32_t module_id)
{
    int i ;
    int label1 = label - 15;    // label 从 16 开始
    int ret = 0;

    i = label1/(32 * 1024);     // 找到对应的 pool
    if (i >= LABEL_POOL_MAX)
    {
        return ret;
    }

    ret = label_pool_get(i, label1);
    if(ret)
    {
        label_table_node_create(label, module_id);//recode label in the label hash table
    }
    return ret;
}


