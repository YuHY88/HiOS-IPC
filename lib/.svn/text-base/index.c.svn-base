/*
*    索引分配模块
*
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "index.h"


/* 初始化 19 个索引池 */
struct index_bitmap index_pool[INDEX_TYPE_NUM] =
{
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},  
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},  
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},
  { NULL, 0 , 0, 0, 0, 0},  
};


/* 注册索引类型 */
int index_register(unsigned char type, unsigned int index_size)
{
    unsigned int num = 0;
	
    if(type > INDEX_TYPE_NUM) return 1;

	if(type != INDEX_TYPE_TCP_PORT)
	{
		index_size += 1;
	}

    num = index_size/32;
	if(index_size%32)
		num = num+1;
    num = num*sizeof(int);
	index_pool[type].index_num = 0;
	index_pool[type].cur_index_i = 0;
	index_pool[type].cur_index_j = 0;
	index_pool[type].index_size = index_size;
 	index_pool[type].pbitmap = malloc(num);	
	memset(index_pool[type].pbitmap, 255, num); /*init every bit = 1*/
	index_pool[type].bitmap_size = num/sizeof(int);
	index_get(type, 0);

	return 0;	
}


/*
 *索引分配函数,分配 1 - index_size 范围的索引
 *函数:index_alloc()	
 *输入参数:type -----表项类型
 *返回值:> 0 ------ 索引值
 *       = 0 ------ 索引分配失败
 */
int index_alloc(unsigned char type)
{
	unsigned int flag = 0;
	unsigned int i = 0;
	unsigned int j = 0;
	unsigned int num = 0;
	unsigned int *pbitmap = index_pool[type].pbitmap;

    if(index_pool[type].index_num == index_pool[type].index_size+1)	
		return 0;

re_alloc:
	if(type == INDEX_TYPE_TCP_PORT)
	{
		i = index_pool[type].cur_index_i;
		j = index_pool[type].cur_index_j;
		pbitmap = index_pool[type].pbitmap + i;
		
	}
	
    for( ; i<index_pool[type].bitmap_size; i++ )
	{
		if( *pbitmap == 0 ) /*如果bitmap为0，代表此bitmap的位全已被分配，继续遍历*/
		{
		    pbitmap++;
			continue;
		}
		for( ; j<32; j++ )
		{
			if((*pbitmap & (0x80000000>>j)) == 0 )
				continue;

			num = i*32+j;                 

			if(num == 0)
				continue;

			if(num > index_pool[type].index_size)
          	{	
				if(flag == 1)
					return 0;
          		goto loop;
			}
            else
            {
				index_pool[type].index_num++;
				*pbitmap &= ~(0x80000000>>j); 
				
				if(type == INDEX_TYPE_TCP_PORT)
				{
					if(j+1 >= 32)
					{
						j = 0;
						i += 1;
						if(i > index_pool[type].bitmap_size)
						{
							i = 0;
						}
						index_pool[type].cur_index_i = i;
						index_pool[type].cur_index_j = j;
					}
					else
					{
						index_pool[type].cur_index_i = i;
						index_pool[type].cur_index_j = j+1;
					}
				}
				
				return num;
            }
		}
	}
loop:
	if(type == INDEX_TYPE_TCP_PORT)
	{
		index_pool[type].cur_index_i = 0;
		index_pool[type].cur_index_j = 0;
		flag = 1;
		goto re_alloc;
	}

	return 0;
}



/*
 *索引释放函数
 *函数:index_free()	
 *输入参数:type -----表项类型
 *         index -----索引
 *输出参数:无
 *返回值:0 ------索引释放成功
 *       1 ------索引释放失败
 */
int index_free(unsigned char type, unsigned int index)
{
	int n, i;
	unsigned int *pbitmap = index_pool[type].pbitmap;

    if(index > index_pool[type].index_size || index == 0 )
	{
		return 1;
	}
	n = index / 32;
	i = index % 32;
	pbitmap += n;
	if((*pbitmap & (0x80000000>>i)) == 0 )
	{
		*pbitmap |= 0x80000000>>i ;    
		index_pool[type].index_num--;
	}
	else
	{
		return 1;
	}
	return 0;
}


/* 分配指定的索引 */
int index_get(unsigned char type, unsigned int index)
{
	int n, i;
	unsigned int *pbitmap = index_pool[type].pbitmap;

    if(index >= index_pool[type].index_size)
		return 1;

	n = index / 32;
	i = index % 32;
	pbitmap = pbitmap + n;

	if((*pbitmap & (0x80000000>>i)) != 0)
	{
		*pbitmap &= ~(0x80000000>>i); /*bitmap位置0*/
		index_pool[type].index_num++;
		return 0;
	}

	return 1;
}



