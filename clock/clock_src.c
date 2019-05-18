#include <unistd.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include "lib/pkt_buffer.h"

#include "clock_cmd.h"
#include "clock_src.h"
#include "synce/synce_ssm.h"



/* 时钟源数组初始化 */
void clock_src_init(void)
{
    int i = 0;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        pclock_src[i] = NULL;
    }

    return;
}

/* 新建一个时钟源 */
struct clock_src_t *clock_src_create(uint32_t ifindex)
{
    struct clock_src_t *psrc = NULL;
    struct ifm_info ifm;

    /* 获取接口信息 */
    ifm_get_all_info(ifindex, MODULE_ID_CLOCK,&ifm);


    /* 分配内存 */
    psrc = (struct clock_src_t *)XCALLOC(MTYPE_CLOCK_ENTRY, sizeof(struct clock_src_t));

    if (psrc == NULL)
    {
        zlog_debug(CLOCK_DBG_ERROR,"%-15s[Func:%s]:Fail to malloc for new clock_src.--Line:%d", __FILE__, __func__, __LINE__);
        return NULL;
    }

    psrc->ifindex = ifindex;
    psrc->down_flag = ifm.status;
    memcpy(psrc->mac, ifm.mac, MAC_LEN);

    return psrc;
}

/* 添加到时钟源数组 */
int clock_src_add(struct clock_src_t *psrc)
{
    int i = 0;

    if (psrc == NULL)
    {
        return ERRNO_FAIL;
    }

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        if (pclock_src[i] == NULL)
        {
            pclock_src[i] = psrc;
            return ERRNO_SUCCESS;
        }
    }

    return ERRNO_OVERSIZE;
}

/* 从时钟源数组删除 */
int clock_src_delete(uint32_t ifindex)
{
    int i = 0;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        if (pclock_src[i] == NULL)
        {
            continue;
        }

        if (pclock_src[i]->ifindex == ifindex)
        {
            XFREE(MTYPE_CLOCK_ENTRY, pclock_src[i]);
            pclock_src[i] = NULL;
            return ERRNO_SUCCESS;
        }
    }

    return ERRNO_SUCCESS;
}

/* 查找一个时钟源 */
struct clock_src_t *clock_src_lookup(uint32_t ifindex)
{
    int i = 0;

    for (i = 0; i < CLOCK_SRC_NUM_MAX; i++)
    {
        if (pclock_src[i] == NULL)
        {
            continue;
        }

        if (pclock_src[i]->ifindex == ifindex)
        {
            return pclock_src[i];
        }
    }

    return NULL;
}

/* 处理接口 up 事件 */
int clock_src_up(uint32_t ifindex)
{
    struct clock_src_t *psrc = NULL;

    psrc = clock_src_lookup(ifindex);

    if (psrc == NULL)
    {
        return ERRNO_NOT_FOUND;
    }

    psrc->down_flag = IFNET_LINKUP;

    if (psrc->psynce != NULL)
    {
        synce_ssm_port_up(psrc->psynce, ifindex);
    }

    return ERRNO_SUCCESS;
}

/* 处理接口 down 事件 */
int clock_src_down(int32_t ifindex)
{
    struct clock_src_t *psrc = NULL;

    psrc = clock_src_lookup(ifindex);

    if (psrc == NULL)
    {
        return ERRNO_NOT_FOUND;
    }

    psrc->down_flag = IFNET_LINKDOWN;

    if (psrc->psynce != NULL)
    {
        synce_ssm_port_down(psrc->psynce, ifindex);
    }

    return ERRNO_SUCCESS;

}
