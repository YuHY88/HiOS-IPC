/*
*    标签管理模块
*
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <lib/index.h>
#include <lib/memory.h>

#include "vcidm.h"


#define VCID_POOL_MAX   31   /* 第 32 个标签池留给保留标签 */


struct index_bitmap vcid_pool[VCID_POOL_MAX];


/* 初始化 32 个 vcid pool，每个 pool 32k vcid，总共 1M vcid */
void vcid_init(void)
{
    unsigned int num = 1024;	
    unsigned int bitmap = num * sizeof(int);
	int i;

    for (i=0; i<VCID_POOL_MAX; i++)
    {
		vcid_pool[i].index_num = 0;
		vcid_pool[i].index_size = 32 * num;			/* 每个pool索引最大数量 */
	 	vcid_pool[i].pbitmap = malloc(bitmap);		/* 每个pool申请bitmap个字节 */
		memset(vcid_pool[i].pbitmap, 255, bitmap);	/* 每个bit置1 */
		vcid_pool[i].bitmap_size = num;
    }
}


/* 从索引池中分配索引 */
static int vcid_pool_alloc(unsigned char type)
{
	int i,j;
	unsigned int num = 0;
	unsigned int *pbitmap = vcid_pool[type].pbitmap;

    if (vcid_pool[type].index_num == vcid_pool[type].index_size)
    {
		return 0;
    }

    for (i=0; i<vcid_pool[type].bitmap_size; i++)
	{
		if (*pbitmap == 0) /*如果bitmap为0，代表此bitmap的位全已被分配，继续遍历*/
		{
		    pbitmap++;
			continue;
		}

		for (j=0; j<32; j++)
		{
			if ((*pbitmap & (0x80000000>>j)) == 0)
			{
				continue;
			}

			*pbitmap &= ~(0x80000000>>j); /*bitmap位置0*/
			num = j+i*32;                 /*计算索引值*/

			if (num == 0)
			{
				continue;
			}

			if (num > vcid_pool[type].index_size)
			{
          		 return 0;
			}
            else
            {
				vcid_pool[type].index_num++;
				return num;
            }
		}
	}

	return 0;
}


/* 释放索引 */
static int vcid_pool_free(unsigned char type, unsigned int index)
{
	unsigned int *pbitmap = vcid_pool[type].pbitmap;
	int n, i;

    if (index >= (vcid_pool[type].index_size*VCID_POOL_MAX))
    {
		return 0;
    }

    index = index % (vcid_pool[type].index_size);

	n = index / (32);
	i = index % (32);

	pbitmap = pbitmap + n;
	*pbitmap |= 0x80000000>>i ;      /*索引释放，标志位置1*/
	vcid_pool[type].index_num--;

	return 1;
}


/* 获取索引 */
static int vcid_pool_get(unsigned char type, unsigned int index)
{
	unsigned int *pbitmap = vcid_pool[type].pbitmap;
	int n, i;

    if (index >= (vcid_pool[type].index_size*VCID_POOL_MAX))
    {
		return 0;
    }

    index = index%(vcid_pool[type].index_size);

	n = index / (32);	/* 计算索引在pool内的偏移量 */
	i = index % (32);	/* 精确计算索引所占用的bit位 */

	pbitmap = pbitmap + n;

	if ((*pbitmap & (0x80000000>>i)) != 0)
	{
		*pbitmap &= ~(0x80000000>>i); /*bitmap位置0*/
		vcid_pool[type].index_num++;

		return 1;
	}

	return 0;
}


/*  分配比 vcid_min 大的标签 */
int vcid_min_alloc(uint32_t vcid_min)
{
	int vcid1 = vcid_min - 1; /* vcid 从 1 开始 */
	int i, j;

	i = vcid1/(32 * 1024);    /* 找到对应的 pool */
	if (i >= VCID_POOL_MAX)
	{
		return 0;
	}

    for (j=i; j<VCID_POOL_MAX; j++)
    {
    	if (vcid_pool[j].index_num < vcid_pool[j].index_size)
    	{
			return vcid_pool_alloc(j) + 1;
    	}
    }

	return 0;
}


/* 释放指定的 vcid */
int vcid_free(uint32_t vcid)
{
	int vcid1 = vcid - 1;  /* vcid 从 1 开始 */
	int i;

	i = vcid1/(32 * 1024); /* 找到对应的 pool */
	if (i >= VCID_POOL_MAX)
	{
		return 0;
	}

	return vcid_pool_free(i, vcid1);
}


/*  随机分配标签 */
int vcid_alloc(void)
{
	int i;

    for (i=0; i<VCID_POOL_MAX; i++)
    {
    	if (vcid_pool[i].index_num < vcid_pool[i].index_size)
    	{
			return vcid_pool_alloc(i) + 1;
    	}
    }

	return 0;
}


/* 分配指定的 vcid */
int vcid_get(uint32_t vcid)
{
	int vcid1 = vcid - 1;/* vcid 从 1 开始 */
	int i;

	i = vcid1/(32 * 1024);/* 找到对应的 pool */
	if(i >= VCID_POOL_MAX)
	{
		return 0;
	}

	return vcid_pool_get(i, vcid1);
}


