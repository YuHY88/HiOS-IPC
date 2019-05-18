/*
 * =====================================================================================
 *
 *       Filename:  ces_if.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/28/2017 09:09:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */

#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/types.h>
#include <lib/alarm.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include <ces/ces_if.h>
#include <ces/ces_main.h>
#include <ces/ces_msg.h>
#include <ces/ces_alarm.h>

/* 计算哈希key值 */
static unsigned int ces_if_hash(void *hash_key)
{
    if (NULL == hash_key)
    {
        return 0;
    }

    return (uint32_t) hash_key ;
}


/* 比较哈希key值 */
static int ces_if_compute(void *item, void *hash_key)
{
    struct hash_bucket *pbucket = item;

    if (NULL == item || NULL == hash_key)
    {
        return 1;
    }

    if (pbucket->hash_key == hash_key)
    {
        return 0;
    }

    return 1;
}


/* 初始化ces_if_table */
void ces_if_table_init(int size)
{
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    hios_hash_init(&ces_if_table, size, ces_if_hash, ces_if_compute);

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s' init ces_if_table successful.\n", __FILE__, __LINE__, __func__);
}

/* 添加接口CES*/
struct ces_if *ces_if_add(uint32_t ifindex)
{
    int ret = 0;
    struct hash_bucket *p_hash_bucket = NULL;
    struct ces_if *pif_new = NULL;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    p_hash_bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
    if (NULL == p_hash_bucket)
    {
        zlog_err("%s:%s:fail to malloc for the hash_node of the new ces_if!--%d\n", __FILE__, __func__, __LINE__);
        return NULL;
    }

    pif_new = (struct ces_if *)XMALLOC(MTYPE_IFM_ENTRY, sizeof(struct ces_if));
    if (NULL == pif_new)
    {
        zlog_err("%s[%d]: In function '%s' the memory is insufficient.\n", __FILE__, __LINE__, __func__);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    memset(p_hash_bucket, 0, sizeof(p_hash_bucket));    
    memset(pif_new, 0, sizeof(pif_new));

    //初始化ces信息
    ces_if_info_init(ifindex, pif_new);

    //ret = ipc_send_hal_wait_ack(&(pif_new->ces), sizeof(struct ces_entry), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_INVALID, IPC_OPCODE_ADD, ifindex);
    ret = ces_msg_send_hal_wait_ack(&(pif_new->ces), sizeof(struct ces_entry), 1, MODULE_ID_HAL,
                                    MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_INVALID, IPC_OPCODE_ADD, ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        XFREE(MTYPE_IFM_ENTRY, pif_new);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    memset(p_hash_bucket, 0, sizeof(struct hash_bucket));
    p_hash_bucket->hash_key = (void *)ifindex;
    p_hash_bucket->data = pif_new;

    ret = hios_hash_add(&ces_if_table, p_hash_bucket);
    if (ret != 0)
    {
        zlog_err("%s:%s:fail to add the hash_node of new ces_if to the ces_if_table!--the line of %d\n", __FILE__, __func__, __LINE__);
        XFREE(MTYPE_IFM_ENTRY, pif_new);
        XFREE(MTYPE_HASH_BACKET, p_hash_bucket);
        return NULL;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return pif_new;
}


/* 删除接口信息 */
int ces_if_delete(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;
    struct ces_if *pif = NULL;
    int ret = 0;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pitem = hios_hash_find(&ces_if_table, (void *)ifindex);

    if (NULL == pitem)
    {
        zlog_warn("%s[%d]: In function '%s' find ces_if info failed.\n", __FILE__, __LINE__, __func__);
        return 1;
    }

    pif = (struct ces_if *)pitem->data;

    if (pif != NULL)
    {
        //ret = ipc_send_hal_wait_ack(&(pif->ces), sizeof(struct ces_entry), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_INVALID, IPC_OPCODE_DELETE, pif->ifindex);
        ret = ces_msg_send_hal_wait_ack(&(pif->ces), sizeof(struct ces_entry), 1, MODULE_ID_HAL, MODULE_ID_CES,
                                        IPC_TYPE_CES, CES_INFO_INVALID, IPC_OPCODE_DELETE, pif->ifindex);
        if (ret != 0)
        {
            zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
            return 1;
        }
    }
	ces_if_alarm_clear(ifindex);

	/*删除接口，清除已经上报的告警信息*/
	/*ipran_alarm_port_unregister(IFM_E1_TYPE, IFM_SLOT_ID_GET(ifindex), 
								IFM_PORT_ID_GET(ifindex), 0, 0);
	ipran_alarm_port_unregister(IFM_PWE3E1_TYPE, IFM_SLOT_ID_GET(ifindex), 
								IFM_PORT_ID_GET(ifindex), 0, 0);*/

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_E1_TYPE;
	gPortInfo.iIfindex = ifindex;
	ipran_alarm_port_unregister(&gPortInfo);

	gPortInfo.iAlarmPort = IFM_PWE3E1_TYPE;
	ipran_alarm_port_unregister(&gPortInfo);
    if (IFM_IS_SUBPORT(ifindex))
    {
        ces_if_timeslot_clear(ifindex, pif->ces.timeslot);
    }

    hios_hash_delete(&ces_if_table, pitem);
    XFREE(MTYPE_IFM_ENTRY, pitem->data);
    XFREE(MTYPE_HASH_BACKET, pitem);

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s' delete from ces_if_table successful.\n", __FILE__, __LINE__, __func__);
    return 0;
}


/* 查找接口信息*/
struct ces_if *ces_if_lookup(uint32_t ifindex)
{
    struct hash_bucket *pitem = NULL;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pitem = hios_hash_find(&ces_if_table, (void *)ifindex);

    if (NULL == pitem)
    {
        return NULL;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s' ces_if lookup successful.\n", __FILE__, __LINE__, __func__);
    return (struct ces_if *)pitem->data;
}

/* 接口down事件 */
int ces_if_down(uint32_t ifindex)
{
    struct ces_if *pif;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        return 1;
    }

    pif->down_flag = IFNET_LINKDOWN;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return 0;
}


/* 接口up事件 */
int ces_if_up(uint32_t ifindex)
{
    struct ces_if *pif;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        return 1;
    }

    pif->down_flag = IFNET_LINKUP;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return 0;
}

//初始化ces结构体
void ces_if_info_init(uint32_t ifindex, struct ces_if *pif)
{
    struct ces_entry *pces;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if(NULL == pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
		return;
	}

    memset(pif, 0, sizeof(struct ces_if));
    pif->ifindex = ifindex;
    pif->down_flag = IFNET_LINKDOWN;

    pces = & (pif->ces);
    pces->encap     = CES_ENCAP_SATOP;
    pces->frame     = CES_FRAME_PCM31;
    pces->mode      = CES_MODE_INVALID;
    pces->clock     = CES_CLK_ACR;
    pces->loopback  = CES_LB_NOLOOPBACK;
    pces->rtp       = CES_DISABLE;
    pces->prbs      = CES_DISABLE;
    pces->jitbuffer = 4;

    if (IFM_IS_SUBPORT(ifindex))
    {
        pces->timeslot  = 0;
    }
    else
    {
        pces->timeslot = 0xfffffffe;
    }

    pces->recvclock = CES_RECEIVE_CLK_LINE;
    pces->label     = 0;
	pces->ces_alarm.los_ais_local = 1;
    memset(pces->j2, 'c', STM_OVERHEAD_LEN);
    
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
}

//ces哈希表是否已经创建，若没有创建则创建
struct ces_if *ces_if_create(uint32_t ifindex)
{
    struct ces_if *pif = NULL;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    //根据索引查找是否已经存在，若不存在进行创建
    pif = ces_if_lookup(ifindex);
    if (pif != NULL)
    {
        return pif;
    }

    pif = ces_if_add(ifindex);

    if (NULL == pif)
    {
        zlog_err("%s[%d] ces_if_add ifindex=0x%0x\n", __FUNCTION__, __LINE__, ifindex);
        return NULL;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return pif;
}

/*设置封装*/
int ces_set_encap(struct ces_if *pif, uint32_t encap)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->encap == encap)
    {
        return 0;
    }

    size = sizeof(pces->encap);

    //ret = ipc_send_hal_wait_ack(&encap, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_ENCAP, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&encap, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_ENCAP, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->encap = encap;

    return 0;
}

/*设置成帧*/
int ces_set_frame(struct ces_if *pif, uint32_t frame)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->frame == frame)
    {
        return 0;
    }

    size = sizeof(pces->frame);

    //ret = ipc_send_hal_wait_ack(&frame, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_FRAME, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&frame, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_FRAME, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->frame = frame;

    return 0;
}

/*设置模式*/
int ces_set_mode(struct ces_if *pif, uint32_t mode)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->mode == mode)
    {
        return 0;
    }

    size = sizeof(pces->mode);

    //ret = ipc_send_hal_wait_ack(&mode, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_MODE, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&mode, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_MODE, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->mode = mode;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_E1_TYPE;
	gPortInfo.iIfindex = pif->ifindex;
	ipran_alarm_port_register(&gPortInfo);

	gPortInfo.iAlarmPort = IFM_PWE3E1_TYPE;
	gPortInfo.iIfindex = pif->ifindex;
	ipran_alarm_port_register(&gPortInfo);

	/*ipran_alarm_port_register(IFM_E1_TYPE, IFM_SLOT_ID_GET(pif->ifindex), 
									IFM_PORT_ID_GET(pif->ifindex), 0, 0);
	ipran_alarm_port_register(IFM_PWE3E1_TYPE, IFM_SLOT_ID_GET(pif->ifindex), 
									IFM_PORT_ID_GET(pif->ifindex), 0, 0);*/
    return 0;
}

/*设置发送时钟*/
int ces_set_sendclock(struct ces_if *pif, uint32_t clock)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->clock == clock)
    {
        return 0;
    }

    size = sizeof(pces->clock);

    //ret = ipc_send_hal_wait_ack(&clock, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_SENDCLOCK, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&clock, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_SENDCLOCK, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->clock = clock;

    return 0;
}

/*设置接收时钟*/
int ces_set_recvclock(struct ces_if *pif, uint32_t clock)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->recvclock == clock)
    {
        return 0;
    }

    size = sizeof(pces->recvclock);

    //ret = ipc_send_hal_wait_ack(&clock, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_RECVCLOCK, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&clock, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_RECVCLOCK, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->recvclock = clock;

    return 0;
}

/*设置jitbuffer*/
int ces_set_jitbuffer(struct ces_if *pif, uint8_t jitbuffer)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->jitbuffer == jitbuffer)
    {
        return 0;
    }

    size = sizeof(pces->jitbuffer);

    //ret = ipc_send_hal_wait_ack(&jitbuffer, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_JITBUFFER, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&jitbuffer, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_JITBUFFER, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->jitbuffer = jitbuffer;

    return 0;
}

/*设置rtp*/
int ces_set_rtp(struct ces_if *pif, uint8_t rtp)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->rtp == rtp)
    {
        return 0;
    }

    size = sizeof(pces->rtp);

    //ret = ipc_send_hal_wait_ack(&rtp, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_RTP, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&rtp, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_RTP, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->rtp = rtp;

    return 0;
}

/*设置prbs*/
int ces_set_prbs(struct ces_if *pif, uint8_t prbs)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->prbs == prbs)
    {
        return 0;
    }

    size = sizeof(pces->prbs);

    //ret = ipc_send_hal_wait_ack(&prbs, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_PRBS, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&prbs, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_PRBS, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->prbs = prbs;

    return 0;
}

/*设置环回*/
int ces_set_loopback(struct ces_if *pif, uint32_t loopback)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->loopback == loopback)
    {
        return 0;
    }

    size = sizeof(pces->loopback);

    //ret = ipc_send_hal_wait_ack(&loopback, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&loopback, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_LOOPBACK, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->loopback = loopback;

    return 0;
}

/*设置子接口时隙*/
int ces_set_timeslot(struct ces_if *pif, uint32_t timeslot)
{
    struct ces_entry *pces = NULL;
    int ret = 0;
    int size = 0;

    if (!pif)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return 1;
    }

    pces = & (pif->ces);

    /*checkout if had config the same vaule*/
    if (pces->timeslot == timeslot)
    {
        return 0;
    }

    size = sizeof(pces->timeslot);

    //ret = ipc_send_hal_wait_ack(&timeslot, size, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_TIMESLOT, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(&timeslot, size, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_TIMESLOT, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    /*save info*/
    pces->timeslot = timeslot;

    return 0;
}

/*设置j2*/
int ces_set_j2(struct ces_if *pif, char *j2)
{
    int ret = 0;

    if (pif == NULL || j2 == NULL)
    {
        zlog_err("%s[%d] pif is null\n", __FUNCTION__, __LINE__);
        return -1;
    }

    /*checkout if had config the same vaule*/
    if(!strncmp(pif->ces.j2, j2 + 1, STM_OVERHEAD_LEN))
    {
        return 0;
    }

    //ret = ipc_send_hal_wait_ack(j2, STM_OVERHEAD_LEN+1, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_J2, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(j2, STM_OVERHEAD_LEN+1, 1, MODULE_ID_HAL, MODULE_ID_CES,
                                    IPC_TYPE_CES, CES_INFO_J2, IPC_OPCODE_UPDATE, pif->ifindex); 
	if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return -1;
    }

    /*save info, 只保存后15位字符*/
    memcpy(pif->ces.j2, j2 + 1, STM_OVERHEAD_LEN);
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.(j2:%s)\n", __FILE__, __LINE__, __func__, pif->ces.j2);

    return 0;
}

/* 设置ces信息*/
int ces_if_info_set(struct ces_if *pif, void *pinfo, enum CES_INFO type)
{
    int ret = 0;

    if ((NULL == pif) || (NULL == pinfo))
    {
        zlog_err("%s[%d] Failed to set, ces if NULL\n", __FUNCTION__, __LINE__);
        return 1;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    switch (type)
    {
        case CES_INFO_ENCAP:
            ret = ces_set_encap(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_FRAME:
            ret = ces_set_frame(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_MODE:
            ret = ces_set_mode(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_SENDCLOCK:
            ret = ces_set_sendclock(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_RECVCLOCK:
            ret = ces_set_recvclock(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_JITBUFFER:
            ret = ces_set_jitbuffer(pif, * (uint8_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_RTP:
            ret = ces_set_rtp(pif, * (uint8_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_PRBS:
            ret = ces_set_prbs(pif, * (uint8_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_LOOPBACK:
            ret = ces_set_loopback(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;

        case CES_INFO_TIMESLOT:
            ret = ces_set_timeslot(pif, * (uint32_t *) pinfo);

            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }

            break;
            
        case CES_INFO_J2:
            ret = ces_set_j2(pif, (char *)pinfo);
            if (ret != 0)
            {
                zlog_err("%s[%d] update ces info erro ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, pif->ifindex, type);
                return 1;
            }
            break;

        default:
            return 1;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.(ifindex:0x%x,down_flag:%u,encap:%u,fram:%u,mode:%u,clock:%u,jitbuffer:%u,rtp:%u,prbs:%u,lb:%u,timeslot:0x%x)\n",
               __FILE__, __LINE__, __func__, pif->ifindex, pif->down_flag, pif->ces.encap, pif->ces.frame, pif->ces.mode, pif->ces.clock,
               pif->ces.jitbuffer, pif->ces.rtp, pif->ces.prbs, pif->ces.loopback, pif->ces.timeslot);
    return 0;
}
void ces_if_alarm_clear(uint32_t ifindex)
{
	CesAlarmReport(CES_PCK_FULLLOS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_PCK_LOS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_PCK_FULLLOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);	
	CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 1);
	CesAlarmReport(CES_E1_LOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_E1_LOF, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_E1_LOMF, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
	CesAlarmReport(CES_E1_CRCERR, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);	
	CesAlarmReport(CES_E1_HDBE_ERR, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);

	return;
}

/**
* @brief      report all e1 port alarms of sc01q
* @param alarm_info: alarm infomation, include bitmap and alarm type
* @param slot_index: slot id
* @author     liubo
* @date       2018-12-05
* @note       "alarm_info->port_mask" use bit position to represent port num, 
              the bit value(0/1、00/01/10/11) indicates the alarm value
*/
int ces_if_sc01q_alarm_set(struct ces_sc01q_alarm_info *alarm_info, uint32_t slot_index)
{
    int ret = 0;
    int type = alarm_info->type;
    uint32_t alarm_value = 0;
    uint8_t byte_positon = 0;
    uint8_t bit_positon = 0;
    uint8_t slot = slot_index & 0xff;
    uint16_t port_mask[8] = {0};
    uint16_t ais_port_mask[16] = {0};
    struct ces_if *pif = NULL;
    struct ces_alarm *ces_alarm = NULL;
    struct ifm_usp if_usp;
    struct hash_bucket *pbucket = NULL;
    int cursor;

    //get data by alarm type. different length of usefull data
    if(type != CES_LOS_AIS_LOCAL)
    {
        memcpy(port_mask, alarm_info->port_mask, 16);
    }
    else
    {
        memcpy(ais_port_mask, alarm_info->port_mask, 32);
    }

    HASH_BUCKET_LOOP(pbucket, cursor, ces_if_table)
    {
        pif = pbucket->data;
        if(pif == NULL)
        {
            continue;
        }
        
        ces_alarm = &(pif->ces.ces_alarm);
        
        ret = ifm_get_usp_by_ifindex(pif->ifindex, &if_usp);
        if((ret != 0) || (if_usp.slot != slot))
        {
            continue;
        }
        
        switch (type)
        {
            case CES_PCK_FULLLOS:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->pck_fulllos != alarm_value)
                {
                    ces_alarm->pck_fulllos = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_PCK_FULLLOS, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_PCK_FULLLOS, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_PCK_LOS:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->pck_los != alarm_value)
                {
                    ces_alarm->pck_los = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_PCK_LOS, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_PCK_LOS, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_PCK_FULLLOS_REMOTE:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->pck_fulllos_r != alarm_value)
                {
                    ces_alarm->pck_fulllos_r = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_PCK_FULLLOS_REMOTE, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_PCK_FULLLOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_LOS_AIS_LOCAL:
                byte_positon = (if_usp.port - 1) >> 3;
                bit_positon = ((if_usp.port - 1) & 0x07)*2;
                alarm_value = (ais_port_mask[byte_positon] >> bit_positon) & 0x3;      //get alarm value from hal
                if (ces_alarm->los_ais_local != alarm_value)
                {
                    ces_alarm->los_ais_local = alarm_value;

                    if (alarm_value == 0)
                    {
    					/*1 los add, 0 ais clear*/
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 1);
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0);
                    }
                    else if (alarm_value == 3)
                    {
    					/*1 los clear, 0 ais add*/
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 1);
                    }
                    else
                    {
    					/*1 los clear, 0 ais clear*/
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 0);
    					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, pif->ifindex, 1);
                    }
                }
                break;
            case CES_E1_LOS_REMOTE:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->e1_los_r != alarm_value)
                {
                    ces_alarm->e1_los_r = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_E1_LOS_REMOTE, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_E1_LOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_E1_LOF:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->e1_lof != alarm_value)
                {
                    ces_alarm->e1_lof = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_E1_LOF, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_E1_LOF, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_E1_LOMF:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->e1_lomf != alarm_value)
                {
                    ces_alarm->e1_lomf = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_E1_LOMF, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_E1_LOMF, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_E1_CRCERR:
                alarm_value = CES_ALARM_MASK_GET(port_mask, if_usp.port);
                if (ces_alarm->e1_crcerr != alarm_value)
                {
                    ces_alarm->e1_crcerr = alarm_value;
                    if (alarm_value)
                    {
                        CesAlarmReport(CES_E1_CRCERR, GPN_SOCK_MSG_OPT_RISE, pif->ifindex, 0);
                    }
                    else
                    {
                        CesAlarmReport(CES_E1_CRCERR, GPN_SOCK_MSG_OPT_CLEAN,  pif->ifindex, 0);
                    }
                }
                break;
            case CES_E1_J2_ERR:
                break;
            default:
                break;
        }
    }

    
    return 0;   
}

/*ces告警信息设置*/
void ces_if_alarm_set(uint8_t value, uint32_t ifindex, enum CES_ALARM_TYPE type)
{
    struct ces_if *pif = NULL;
    struct ces_alarm *ces_alarm = NULL;

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        zlog_err("%s[%d] ces_lookup ifindex=0x%0x type=%d\n", __FUNCTION__, __LINE__, ifindex, type);
        return;
    }

    ces_alarm = &(pif->ces.ces_alarm);

    switch (type)
    {
        case CES_PCK_FULLLOS:
        {
            if (ces_alarm->pck_fulllos != value)
            {
                ces_alarm->pck_fulllos = value;

                if (value)
                {
					CesAlarmReport(CES_PCK_FULLLOS, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_PCK_FULLLOS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_PCK_LOS:
        {
            if (ces_alarm->pck_los != value)
            {
                ces_alarm->pck_los = value;

                if (value)
                {
					CesAlarmReport(CES_PCK_LOS, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_PCK_LOS, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_PCK_FULLLOS_REMOTE:
        {
            if (ces_alarm->pck_fulllos_r != value)
            {
                ces_alarm->pck_fulllos_r = value;

                if (value)
                {
					CesAlarmReport(CES_PCK_FULLLOS_REMOTE, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_PCK_FULLLOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_LOS_AIS_LOCAL:
        {
            if (ces_alarm->los_ais_local != value)
            {
                ces_alarm->los_ais_local = value;

                if (value == 0)
                {
					/*1 los add, 0 ais clear*/
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_RISE, ifindex, 1);
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
                else if (value == 3)
                {
					/*1 los clear, 0 ais add*/
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 1);
                }
                else
                {
					/*1 los clear, 0 ais clear*/
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
					CesAlarmReport(CES_LOS_AIS_LOCAL, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 1);
                }
            }
        }
        break;

        case CES_E1_LOS_REMOTE:
        {
            if (ces_alarm->e1_los_r != value)
            {
                ces_alarm->e1_los_r = value;

                if (value)
                {
					CesAlarmReport(CES_E1_LOS_REMOTE, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_E1_LOS_REMOTE, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_E1_LOF:
        {
            if (ces_alarm->e1_lof != value)
            {
                ces_alarm->e1_lof = value;

                if (value)
                {
					CesAlarmReport(CES_E1_LOF, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_E1_LOF, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_E1_LOMF:
        {
            if (ces_alarm->e1_lomf != value)
            {
                ces_alarm->e1_lomf = value;

                if (value)
                {
					CesAlarmReport(CES_E1_LOMF, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_E1_LOMF, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_E1_CRCERR:
        {
            if (ces_alarm->e1_crcerr != value)
            {
                ces_alarm->e1_crcerr = value;

                if (value)
                {
					CesAlarmReport(CES_E1_CRCERR, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_E1_CRCERR, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;

        case CES_E1_HDBE_ERR:
        {
            if (ces_alarm->e1_hdbe_err != value)

            {
                ces_alarm->e1_hdbe_err = value;

                if (value)
                {
					CesAlarmReport(CES_E1_HDBE_ERR, GPN_SOCK_MSG_OPT_RISE, ifindex, 0);
                }
                else
                {
					CesAlarmReport(CES_E1_HDBE_ERR, GPN_SOCK_MSG_OPT_CLEAN, ifindex, 0);
                }
            }
        }
        break;
        
        case CES_E1_J2_ERR:
            break;

        default:
            break;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.(ifindex:0x%x, type=%u, value=%u)\n", __FILE__, __LINE__, __func__, ifindex, type, value);

    return;
}

/* 查找传入 ifindex 之后对应的 n 个数据 */
int ces_if_bulk_get(uint32_t ifindex, struct ces_if cesif[])
{
    struct hash_bucket *pbucket = NULL;
    struct hash_bucket *pnext   = NULL;
    struct ces_if    *pif     = NULL;
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN / sizeof(struct ces_if);
    int cursor;
    int val = 0;

    if (0 == ifindex)   /* 初次获取 */
    {
        HASH_BUCKET_LOOP(pbucket, cursor, ces_if_table)
        {
            pif = (struct ces_if *)pbucket->data;

            if (NULL == pif)
            {
                continue;
            }

            memcpy(&cesif[data_num++], pif, sizeof(struct ces_if));

            if (data_num == msg_num)
            {
                return data_num;
            }
        }
    }
    else /* 找到 ifindex 后面第一个数据 */
    {
        pbucket = hios_hash_find(&ces_if_table, (void *)ifindex);

        if (NULL == pbucket)    /* 未找到 ifindex 对应的数据 */
        {
            /* 查找失败返回到当前哈希桶的链表头部 */
            val %= ces_if_table.compute_hash((void *)ifindex);

            if (NULL != ces_if_table.buckets[val])    /* 当前哈希桶链表不为空 */
            {
                pbucket = ces_if_table.buckets[val];
            }
            else                                        /* 当前哈希桶链表为空 */
            {
                for (++val; val < HASHTAB_SIZE; ++val)
                {
                    if (NULL != ces_if_table.buckets[val])
                    {
                        pbucket = ces_if_table.buckets[val];
                    }
                }
            }
        }

        if (NULL != pbucket)
        {
            for (data_num = 0; data_num < msg_num; data_num++)
            {
                pnext = hios_hash_next_cursor(&ces_if_table, pbucket);

                if ((NULL == pnext) || (NULL == pnext->data))
                {
                    break;
                }

                pif = pnext->data;
                memcpy(&cesif[data_num], pif, sizeof(struct ces_if));
                pbucket = pnext;
            }
        }
    }

    return data_num;
}

/*接口时隙是否可用*/
int ces_if_timeslot_valid(uint32_t ifindex, uint32_t timeslot)
{
    struct ces_if *ppif = NULL;
    struct ces_if *pif = NULL;
    int i = 0;

    ppif = ces_if_lookup(IFM_PARENT_IFINDEX_GET(ifindex));

    if (NULL == ppif)
    {
        return -1;
    }

    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        for (i = 1; i <= 31; i++)
        {
            if (timeslot & (1 << i))
            {
                if (!(ppif->ces.timeslot & (1 << i)))
                {
                    return -1;
                }
            }
        }
    }
    else
    {
        for (i = 1; i <= 31; i++)
        {
            if ((pif->ces.timeslot & (1 << i)) && (timeslot & (1 << i)))
            {
                continue;
            }

            if (timeslot & (1 << i))
            {
                if (!(ppif->ces.timeslot & (1 << i)))
                {
                    return -1;
                }
            }
        }

    }

    return 0;
}


/*子接口设置时隙，tdm物理口清除本端时隙*/
int ces_if_timeslot_set(uint32_t ifindex, uint32_t timeslot)
{
    struct ces_if *pif = NULL;
    struct ces_if *ppif = NULL;
    int i = 0;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        return -1;
    }

    ppif = ces_if_lookup(IFM_PARENT_IFINDEX_GET(ifindex));

    if (NULL == ppif)
    {
        return -1;
    }

    /*重新设置子端口时隙，先恢复至tdm物理端口中*/
    for (i = 1; i <= 31; i++)
    {
        if (pif->ces.timeslot & (1 << i))
        {
            ppif->ces.timeslot |= (1 << i);
        }
    }

    /*清除tdm物理口时隙*/
    for (i = 1; i <= 31; i++)
    {
        if (timeslot & (1 << i))
        {
            ppif->ces.timeslot &= ~(1 << i);
        }
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.(subifindex:0x%x(0x%x), ifindex:0x%x(0x%x))\n", __FILE__, __LINE__, __func__, ifindex, timeslot, IFM_PARENT_IFINDEX_GET(ifindex), ppif->ces.timeslot);

    return 0;
}

/*删除子接口时，恢复tdm物理口的时隙*/
int ces_if_timeslot_clear(uint32_t ifindex, uint32_t timeslot)
{
    struct ces_if *pif = NULL;
    int i = 0;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    /*获取tdm物理口的ces_if结构体*/
    pif = ces_if_lookup(IFM_PARENT_IFINDEX_GET(ifindex));

    if (NULL == pif)
    {
        return -1;
    }

    for (i = 1; i <= 31; i++)
    {
        if (timeslot & (1 << i))
        {
            pif->ces.timeslot |= (1 << i);
        }
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.(subifindex:0x%x(0x%x), ifindex:0x%x(0x%x))\n", __FILE__, __LINE__, __func__, ifindex, timeslot, IFM_PARENT_IFINDEX_GET(ifindex), pif->ces.timeslot);

    return 0;

}


/*获取ces相关计数*/
struct ces_count_info *ces_if_get_count(uint32_t ifindex)
{
    static struct ces_count_info count_info;

    struct ipc_mesg_n * pmsg = ipc_sync_send_n2(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_CES,
                               IPC_TYPE_CES, 0, IPC_OPCODE_GET, ifindex, 5000);

    memset(&count_info, 0, sizeof(struct ces_count_info));
    
    if(pmsg)
    {
        memcpy(&count_info, pmsg->msg_data, sizeof(struct ces_count_info));
        mem_share_free(pmsg, MODULE_ID_CES);

        return &count_info;
    }
    else
    {
        return NULL;
    }
    //return ipc_send_hal_wait_reply(NULL, 0, 1 , MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, 0, IPC_OPCODE_GET, ifindex);
}


/*处理板卡插入，接收隐藏再恢复时间*/
int ces_if_config_restore(uint32_t ifindex)
{
    struct ces_if *pif = NULL;
    int ret = 0;
    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        return 1;
    }

    //ret = ipc_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES, CES_INFO_CONFIGRESTORE, IPC_OPCODE_UPDATE, pif->ifindex);
    ret = ces_msg_send_hal_wait_ack(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_CES,
                                    CES_INFO_CONFIGRESTORE, IPC_OPCODE_UPDATE, pif->ifindex);
    if (ret != 0)
    {
        zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
        return 1;
    }

    zlog_debug(CES_DEBUG_TDM, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

    return 0;
}

