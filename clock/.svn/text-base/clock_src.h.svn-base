/*
*    管理时钟源端口
*/
#ifndef HIOS_CLOCK_SRC_H
#define HIOS_CLOCK_SRC_H


#include <lib/ifm_common.h>


#define CLOCK_SRC_NUM_MAX   8    /* 时钟源个数 */


/* 时钟源数据结构 */
struct clock_src_t
{
    uint32_t              ifindex;
    uchar                 mac[MAC_LEN];
    uchar                 pad[2];
    enum IFNET_LINKSTAT   down_flag;      /* 0: linkup, 1: linkdown */
    struct synce_if      *psynce;     /* syncE 的接口数据 */
};


/* 时钟源数组 */
struct clock_src_t *pclock_src[CLOCK_SRC_NUM_MAX];


/* clock_src 操作函数 */
void clock_src_init(void);  /* 时钟源数组初始化 */
struct clock_src_t *clock_src_create(uint32_t ifindex);  /* 新建一个时钟源 */
int clock_src_add(struct clock_src_t *psrc);              /* 添加到时钟源数组 */
int clock_src_delete(uint32_t ifindex);                  /* 从时钟源数组删除 */
struct clock_src_t *clock_src_lookup(uint32_t ifindex);  /* 查找一个时钟源 */
int clock_src_up(uint32_t ifindex);     /* 处理接口 up 事件 */
int clock_src_down(int32_t ifindex);   /* 处理接口 down 事件 */

#endif

