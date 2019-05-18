/*
*    索引分配模块
*
*/


#ifndef HIOS_INDEX_H
#define HIOS_INDEX_H


#define INDEX_TYPE_NUM  19     /*总共有 19 个 index type*/

#define LABEL_RESERVED_BASE     32*31*1024  /* 内部保留标签的起始值，固定用于 OAM 事件报文 */
#define LSP_RESERVED_BASE       10000       /* 内部保留标签的 LSP 索引范围 1-10000 */
#define OAM_RESERVED_BASE       512         /* 内部保留标签的 oam索引范围 1-512 */


/* 索引类型 */
enum INDEX_TYPE
{
	INDEX_TYPE_LSP = 0,     /* lsp 索引 */
	INDEX_TYPE_TCP_PORT,    /* TCP 端口号 */
	INDEX_TYPE_UDP_PORT,    /* UDP 端口号 */
	INDEX_TYPE_NHP,         /* NHP 和 ECMP 索引 */
	INDEX_TYPE_ARP,         /* ARP 和 ND 索引*/
	INDEX_TYPE_NDP,         /* ND 索引*/
	INDEX_TYPE_CAR,		    /* CAR 索引 */
	INDEX_TYPE_HQOS,		/* HQOS 索引 */
	INDEX_TYPE_OAM,         /* OAM 的软件索引 */	
	INDEX_TYPE_BFD_HW,      /* BFD 的硬件索引 */
	INDEX_TYPE_OAM_HW,      /* mpls OAM 的硬件索引 */
	INDEX_TYPE_LABEL,       /* 保留标签的索引 */	
	INDEX_TYPE_LABEL_LSP,   /* 保留标签的 LSP 索引 */
	INDEX_TYPE_IPMC, 
	INDEX_TYPE_H3C_ACL,		/*h3c 最小化申请acl group id*/
	INDEX_TYPE_VCAP,		/*vtss vcap 硬件索引*/
	INDEX_TYPE_ECE,
	INDEX_TYPE_POLICER,		/*vtss policer 索引*/
	INDEX_TYPE_LDP_FRR,
	INDEX_TYPE_DXC,
	INDEX_TYPE_MAX = INDEX_TYPE_NUM
};


/* 索引位图，每 bit 表示一个索引值 */
struct index_bitmap
{
	unsigned int *pbitmap;
    unsigned int index_size;	/* 可分配的索引总数*/
	unsigned int index_num; 	/* 已分配的索引数 */
	unsigned int bitmap_size;	/* 索引池拥有的 int 个数 */
	unsigned int cur_index_i;     /* 为了让申请索引的时候可以循环从数组里申请，加入该变量记录当前数组的位置*/
	unsigned int cur_index_j;
};


int index_register(unsigned char type, unsigned int index_size);/* register the index type and size */

int index_alloc(unsigned char type);/* alloc a index range in 1 - index_size */

int index_get(unsigned char type, unsigned int index);  /* 分配指定的索引 */

int index_free(unsigned char type, unsigned int index); /* 释放指定的索引 */


#endif

