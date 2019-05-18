#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/types.h>
#include <lib/devm_com.h>
#include "lib/thread.h"
#include "lib/log.h"
#include <lib/hptimer.h>

#include <ftm/pkt_eth.h>
#include <unistd.h> 
#include "../l2_if.h"
#include "../trunk.h"
#include "lacp.h"
#include "l2_msg.h"

extern struct thread_master *l2_master;
extern unsigned char l2_mac[MAC_LEN];


/*打印收/发报文*/
static void lacp_pkt_dump(void *pkt_data, uint32_t ifindex, enum LACP_DUMP_PKT type)
{
    int ret = 0;
    char ifname[IFNET_NAMESIZE];
    char *p_pkt = NULL;

    if (NULL == pkt_data)
    {
        return;
    }

    /*打印发送报文*/
    switch (type)
	{
        case LACP_DUMP_RECEIVE:
        {
            if (lacp_recv_dbg)
            {
                ret = ifm_get_name_by_ifindex(ifindex, ifname);

                if (ret < 0)
                {
                    assert(0);
                }

                printf("%s receive lacp packet: \n", ifname);
                p_pkt = pkt_dump(pkt_data, sizeof(struct lacp_pkt));
                printf("%s", p_pkt);
                printf("\n");
            }
		}
		break;
        case LACP_DUMP_SEND:
        {
            if (lacp_send_dbg)
            {
                ret = ifm_get_name_by_ifindex(ifindex, ifname);

                if (ret < 0)
                {
                    assert(0);
                }

				 printf("%s send lacp packet: \n",ifname);
				 p_pkt = pkt_dump(pkt_data, sizeof(struct lacp_pkt));
				 printf("%s",p_pkt);
				 printf("\n");
			}
		}
		break;
		default:
			break;
	}

    return;
}

/*根据收包的报文跟新信息*/
static void lacp_record_pdu (struct trunk *trunk_entry, uint32_t ifindex)
{
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    struct trunk_port *tport = NULL;
    struct l2if *l2_trunk_if = NULL;
    struct lacp_pkt *pdu = NULL;
    unsigned int is_sync = LACP_FALSE;
    int is_maintaining = 0;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    l2_trunk_if = l2if_get(trunk_entry->ifindex);

    if (NULL == l2_trunk_if)
    {
        zlog_err("%s[%d]:leave %s:The trunk failed to get l2_if\n", __FILE__, __LINE__, __func__);
        return;
    }

    if ((pdu = tport->link.pdu) == NULL)
    {
        zlog_err("%s[%d]:leave %s:pdu NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /* 43.4.9 recordPDU paragraph 5 */
    is_maintaining = ((LACP_ACTIVITY_ACTIVE == LACP_GET_ACTIVITY(pdu->actor_state))
                      || ((LACP_ACTIVITY_ACTIVE == LACP_GET_ACTIVITY(tport->link.actor_state))
                          && (LACP_ACTIVITY_ACTIVE == LACP_GET_ACTIVITY(pdu->partner_state))));

    /* 43.4.9 recordPDU paragraph 1 */
    tport->link.partner_port = pdu->actor_port;
    tport->link.partner_ppri = pdu->actor_ppri;
    memcpy(tport->link.partner_sys, pdu->actor_sys, MAC_LEN);

    tport->link.partner_spri = pdu->actor_spri;
    tport->link.partner_key = pdu->actor_key;
    /* Record partner state (except SYNCHRONIZATION which is set or cleared below) */
    tport->link.partner_state = pdu->actor_state;

    LACP_CLR_DEFAULTED(tport->link.actor_state);

    /*两端都设置被动模式，不处理*/
    if ((LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.actor_state))
            && (LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.partner_state)))
    {
        return;
    }

    /* 43.4.9 recordPDU paragraph 2 */
    is_sync = ((tport->link.actor_port == pdu->partner_port)
               && (tport->priority == pdu->partner_ppri)
               && (memcmp(l2_trunk_if->mac, pdu->partner_sys, MAC_LEN) == 0)
               && (trunk_entry->priority == pdu->partner_spri)
               && (tport->link.actor_key == pdu->partner_key)
               && (LACP_GET_AGGREGATION(tport->link.actor_state) == LACP_GET_AGGREGATION(pdu->partner_state))
               && (LACP_SYNCHRONIZATION_IN_SYNC == LACP_GET_SYNCHRONIZATION(pdu->actor_state))
               && is_maintaining)
              /* 43.4.9 recordPDU paragraph 3 */
              || ((LACP_AGGREGATION_INDIVIDUAL == LACP_GET_AGGREGATION(pdu->actor_state))
                  && (LACP_SYNCHRONIZATION_IN_SYNC == LACP_GET_SYNCHRONIZATION(pdu->actor_state))
                  && is_maintaining);

    if (is_sync)
    {
        LACP_SET_SYNCHRONIZATION(tport->link.partner_state);
    }
    else
    {
        /* 43.4.9 recordPDU paragraph 4 */
        LACP_CLR_SYNCHRONIZATION(tport->link.partner_state);
    }

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return;
}

/* 更新默认值*/
static void lacp_record_default(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*将本端设备中对端信息设置为默认*/
    tport->link.partner_port = tport->link.partner_admin_port;
    tport->link.partner_ppri = tport->link.partner_admin_ppri;
    memcpy (tport->link.partner_sys, tport->link.partner_admin_sys, MAC_LEN);
    tport->link.partner_spri = tport->link.partner_admin_spri;
    tport->link.partner_key = tport->link.partner_admin_key;
    tport->link.partner_state = tport->link.partner_admin_state;

    LACP_SET_DEFAULTED (tport->link.actor_state);
	return;
}
/*更新默认选中状态*/
static void lacp_update_default_selected(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    if ((tport->link.partner_port != tport->link.partner_admin_port)
    || (tport->link.partner_ppri != tport->link.partner_admin_ppri)
    || (memcmp (tport->link.partner_sys, tport->link.partner_admin_sys, MAC_LEN) != 0)
    || (tport->link.partner_spri != tport->link.partner_admin_spri)
    || (tport->link.partner_key != tport->link.partner_admin_key)
    || (LACP_GET_AGGREGATION (tport->link.partner_state) != LACP_GET_AGGREGATION (tport->link.partner_admin_state)))
    {
        tport->link.selected = LACP_UNSELECTED; 
    }
	return;
}

/*更新选中状态*/
static void lacp_update_selected(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;
    struct lacp_pkt *pdu = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    if ((pdu = tport->link.pdu) == NULL)
    {
        zlog_err("%s[%d]:leave %s:pdu NULL\n",__FILE__,__LINE__,__func__);
        return;
    }

    if ((tport->link.partner_port != pdu->actor_port)
    || (tport->link.partner_ppri != pdu->actor_ppri)
    || (memcmp (tport->link.partner_sys, pdu->actor_sys, MAC_LEN) != 0)
    || (tport->link.partner_spri != pdu->actor_spri)
    || (tport->link.partner_key != pdu->actor_key)
    || (LACP_GET_AGGREGATION (tport->link.partner_state) != LACP_GET_AGGREGATION (pdu->actor_state)))
    {
        tport->link.selected = LACP_UNSELECTED; 
    }
	return;
}

/*若收到的对端报文与本端信息不同，则发送lacp报文进行快速更新*/
static int lacp_update_ntt(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;
    struct lacp_pkt *pdu = NULL;
    struct l2if *l2_trunk_if = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return LACP_FALSE;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return LACP_FALSE;
    }

    l2_trunk_if = l2if_get(trunk_entry->ifindex);

    if (NULL == l2_trunk_if)
    {
        zlog_err("%s[%d]:leave %s:The trunk failed to get l2_if\n", __FILE__, __LINE__, __func__);
        return LACP_FALSE;
    }

    if ((pdu = tport->link.pdu) == NULL)
    {
        zlog_err("%s[%d]:leave %s:pdu NULL\n",__FILE__,__LINE__,__func__);
        return LACP_FALSE;
    }

    if ((tport->link.actor_port != pdu->partner_port)
    || (tport->priority != pdu->partner_ppri)
    || (memcmp (l2_trunk_if->mac, pdu->partner_sys, MAC_LEN) != 0)
    || (trunk_entry->priority != pdu->partner_spri)
    || (tport->link.actor_key != pdu->partner_key)
    || (LACP_GET_ACTIVITY (tport->link.actor_state) != LACP_GET_ACTIVITY (pdu->partner_state))
    || (LACP_GET_TIMEOUT (tport->link.actor_state) != LACP_GET_TIMEOUT (pdu->partner_state))
    || (LACP_GET_SYNCHRONIZATION (tport->link.actor_state) != LACP_GET_SYNCHRONIZATION (pdu->partner_state))
    || (LACP_GET_AGGREGATION (tport->link.actor_state) != LACP_GET_AGGREGATION (pdu->partner_state))
    || (LACP_GET_COLLECTING (tport->link.actor_state) != LACP_GET_COLLECTING (pdu->partner_state))
    || (LACP_GET_DISTRIBUTING (tport->link.actor_state) != LACP_GET_DISTRIBUTING (pdu->partner_state)))
    {   
        return LACP_TRUE;
    }

    return LACP_FALSE;
}
/*阻塞端口*/
static void lacp_port_blocking(struct trunk *trunk_entry, uint32_t ifindex)
{
    int ret = 0;
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*设置端口阻塞*/
    LACP_CLR_COLLECTING (tport->link.actor_state);
    if(tport->link.port_state != IFNET_STP_BLOCK)
    {
        ret = ifm_set_block(tport->ifindex, MODULE_ID_L2, IFNET_STP_BLOCK);
        if (ret < 0)
        {
            zlog_err("%s[%d]:%s:lacp: set block error on\n",__FILE__,__LINE__,__func__);
            return;
        }
        tport->link.port_state = IFNET_STP_BLOCK;
    }
    LACP_CLR_DISTRIBUTING (tport->link.actor_state);
	
	return;
}
/*转发端口*/
static void lacp_port_forwarding(struct trunk *trunk_entry, uint32_t ifindex)
{
    int ret = 0;
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    LACP_SET_DISTRIBUTING(tport->link.actor_state);
	if(tport->link.port_state != IFNET_STP_FORWARD)
	{
		ret = ifm_set_block(tport->ifindex, MODULE_ID_L2, IFNET_STP_FORWARD);
		if (ret < 0)
		{
			zlog_err("%s[%d]:%s:lacp: set forward error on\n",__FILE__,__LINE__,__func__);
			return;
		}
		tport->link.port_state = IFNET_STP_FORWARD;
	}
	LACP_SET_COLLECTING (tport->link.actor_state);

	return;
}

/*端口linkdown处理*/
static void lacp_port_linkdown_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport->failback = 0;
    tport->link.selected = LACP_UNSELECTED;
    lacp_record_default(trunk_entry, ifindex);
    LACP_CLR_EXPIRED(tport->link.actor_state);

    LACP_CLR_SYNCHRONIZATION(tport->link.partner_state);

    LACP_CLR_SYNCHRONIZATION (tport->link.actor_state);
    
    /*设置接收标记为FALSE*/
    tport->link.rx_flag = LACP_FALSE;
    
	return;
}

/*端口linkup处理*/
static void lacp_port_linkup_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport->link.selected = LACP_UNSELECTED;
    lacp_record_default (trunk_entry, ifindex);
    LACP_CLR_SYNCHRONIZATION (tport->link.partner_state);
    LACP_SET_TIMEOUT (tport->link.partner_state);
    LACP_SET_EXPIRED (tport->link.actor_state);
    LACP_CLR_SYNCHRONIZATION (tport->link.actor_state);

    /*更新本地信息*/
    tport->link.tx_timer = LACP_FAST_PERIODIC_TIME;
    tport->link.rx_timer = (3*trunk_entry->lacp_interval);
    tport->link.rx_flag = LACP_FALSE;
	
	return;
}

/*超时处理*/
static void lacp_rx_expired_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    LACP_CLR_SYNCHRONIZATION(tport->link.partner_state);
    LACP_SET_TIMEOUT (tport->link.partner_state);
    LACP_SET_EXPIRED (tport->link.actor_state);
    
    /*超时后，再等待3s判断是否还超时*/
    tport->link.tx_timer = LACP_FAST_PERIODIC_TIME;
	tport->link.rx_timer = LACP_SHORT_TIMEOUT_TIME;
    return;
}

/*接收报文处理*/
static void lacp_rx_current_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    uint32_t ntt = LACP_FALSE;
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport->link.rx_timer = (3 * trunk_entry->lacp_interval);
    lacp_update_selected(trunk_entry, ifindex);
    ntt = lacp_update_ntt(trunk_entry, ifindex);
    lacp_record_pdu(trunk_entry, ifindex);
    LACP_CLR_EXPIRED(tport->link.actor_state);

    if (LACP_TRUE == ntt)
    {
        lacp_pkt_tx(trunk_entry, tport->ifindex);
    }

    /*设置接收标记为TRUE*/
    tport->link.rx_flag = LACP_TRUE;

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return;
}

/*接收超时设置为默认*/
static void lacp_rx_defaulted_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    lacp_update_default_selected(trunk_entry, ifindex);
    lacp_record_default (trunk_entry, ifindex);
    LACP_CLR_EXPIRED (tport->link.actor_state);

    /*设置端口阻塞*/
    LACP_CLR_SYNCHRONIZATION (tport->link.actor_state);

    /*设置接收标志为FALSE*/
    tport->link.rx_flag = LACP_FALSE;
	
	return;
}

/*成员口删除，将本端sync标志设置为未聚合完成，
发送至对端，对端设备将与其相连的端口不选中
*/
void lacp_port_delect_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*删除成员端口，清除同步标志并发送至对端*/
    if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
    {
        tport->link.selected = LACP_UNSELECTED;
        LACP_CLR_SYNCHRONIZATION(tport->link.actor_state);
        lacp_pkt_tx(trunk_entry, ifindex);
    }
    else
	{
		for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
		{
			tport_tmp->link.selected = LACP_UNSELECTED;
			LACP_CLR_SYNCHRONIZATION (tport_tmp->link.actor_state);
			lacp_pkt_tx(trunk_entry, tport_tmp->ifindex);
		}
	}

	return;
}

/*主备方式回切*/
/*static int lacp_backup_failback_timer(struct thread *thread)*/
 int lacp_backup_failback_timer(void *para)
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    /*trunk_entry = (struct trunk *)THREAD_ARG(thread);*/
    trunk_entry = (struct trunk*)para;
	
    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk entry null\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    //THREAD_TIMER_OFF(trunk_entry->fb_timer);

    /*发生回切时，若主端口linkdown,不处理*/
    tport = trunk_port_lookup(trunk_entry->trunkid, trunk_entry->master_if);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport entry null\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    tport->failback = 0;

    if (IFNET_LINKDOWN == tport->linkstatus)
    {
        return 0;
    }

    tport->link.selected = LACP_SELECTED;
    LACP_SET_SYNCHRONIZATION(tport->link.actor_state);
    lacp_pkt_tx(trunk_entry, tport->ifindex);

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.%u\n", __FILE__, __LINE__, __func__, trunk_entry->trunkid);

    return 0;
}

int lacp_backup_master_port_select(struct trunk *trunk_entry, struct trunk_port *tport)
{
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    uint32_t ifindex = 0;
    int ret = 0;

    if ((NULL == trunk_entry) || (NULL == tport))
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*主设备*/
    if ((trunk_entry->priority < tport->link.partner_spri) ||
            ((trunk_entry->priority == tport->link.partner_spri) &&
             (memcmp(l2_mac, tport->link.partner_sys, MAC_LEN) < 0)))
    {
        trunk_backup_port_select(trunk_entry);
    }
    else
    {
        if (TRUNK_BACK_MEMBER_MAX == trunk_entry->portlist.count)
        {
            ifindex = tport->ifindex;

            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
            {
                if (!tport_tmp->link.partner_port)
                {
                    return ERRNO_FAIL;
                }

                if ((tport_tmp->link.partner_ppri < tport->link.partner_ppri) ||
                        ((tport_tmp->link.partner_ppri == tport->link.partner_ppri) &&
                         (tport_tmp->link.partner_port < tport->link.partner_port)))
                {
                    ifindex = tport_tmp->ifindex;
                }
            }
        }

        if (trunk_entry->master_if != ifindex)
        {
            trunk_entry->master_if = ifindex;
            /*发送给hal*/
            //ret = ipc_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_MASTER_PORT_SET, IPC_OPCODE_UPDATE, 0);
			ret = l2_msg_send_hal_wait_ack(trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
					MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_MASTER_PORT_SET, IPC_OPCODE_UPDATE, 0);

            if (ret != 0)
            {
                zlog_err("%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret);
                return ERRNO_FAIL;
            }

        }
    }


    return ERRNO_SUCCESS;
}
/*
	1、负载分担方式
		不考虑设备id,运行key等，收到报文即选中，用户 保证
	2、主备方式
		1)主设备选择工作端口，并把sync标志发送至对端
		2)备设备根据主设备的sync标志设置本端选中状态，
		其他一些问题用户保证

*/
static void lacp_select_logic_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    struct trunk_port *tport = NULL;
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    struct l2if *l2_trunk_if = NULL;
    uint32_t ifindex_tmp = 0;
    uint8_t sync = 0;
    int flag = 0;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    l2_trunk_if = l2if_get(trunk_entry->ifindex);

    if (NULL == l2_trunk_if)
    {
        zlog_err("%s[%d]:leave %s:The trunk failed to get l2_if\n", __FILE__, __LINE__, __func__);
        return;
    }

    sync = LACP_GET_SYNCHRONIZATION(tport->link.actor_state);

    /*负载分担模式不考虑系统id和运行key,收到报文即选中*/
    if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
    {
        tport->link.selected = LACP_SELECTED;
        LACP_SET_SYNCHRONIZATION(tport->link.actor_state);
    }
    /*主备模式*/
    else
    {
        lacp_backup_master_port_select(trunk_entry, tport);

        if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)
        {
            tport->link.selected = LACP_UNSELECTED;
            LACP_CLR_SYNCHRONIZATION(tport->link.actor_state);
        }
        else
        {
            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
            {
                if (tport_tmp->link.sync_state)
                {
                    flag = 1;
                    ifindex_tmp = tport_tmp->ifindex;
                    break;
                }
            }

            if (flag)
            {
                if (TRUNK_SWITCH_FAILBACK == trunk_entry->failback)
                {
                    if ((trunk_entry->master_if != ifindex_tmp) &&
                            (trunk_entry->master_if == tport->ifindex) &&
                            (IFNET_LINKUP == tport->linkstatus) &&
                            (!LACP_GET_SYNCHRONIZATION(tport->link.actor_state)) &&
                            (!tport->failback))
                    {
                        tport->failback = 1;
				/*
                        THREAD_TIMER_OFF(trunk_entry->fb_timer);
                        THREAD_TIMER_ON(l2_master, trunk_entry->fb_timer, lacp_backup_failback_timer, trunk_entry, trunk_entry->wtr);
			     */
			    high_pre_timer_add("LacpFailbackTimer", LIB_TIMER_TYPE_NOLOOP, 
										lacp_backup_failback_timer, trunk_entry, 1000*trunk_entry->wtr);

			}
                }
                else
                {
                    if ((ifindex_tmp != trunk_entry->master_if) && (ifindex == trunk_entry->master_if))
                    {
                        if (LACP_GET_SYNCHRONIZATION((tport->link.pdu)->actor_state))
                        {
                            tport->link.selected = LACP_SELECTED;
                            LACP_SET_SYNCHRONIZATION(tport->link.actor_state);
                        }
                    }
                }
            }
            else
            {
                tport->link.selected = LACP_SELECTED;
                LACP_SET_SYNCHRONIZATION(tport->link.actor_state);
            }
        }
    }

    if (sync != LACP_GET_SYNCHRONIZATION(tport->link.actor_state))
    {
        lacp_pkt_tx(trunk_entry, ifindex);
    }

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.(ifindex:0x%x)\n", __FILE__, __LINE__, __func__, ifindex);

    return;
}

/*
主备方式，端口发生端口linkdown或者接收超时，选择另一个
成员端口作为工作端口，并通知对端设备
*/
void lacp_port_linkdown_timeout_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
    {
        if (TRUNK_BACK_MEMBER_MAX == trunk_entry->portlist.count)
        {
            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
            {
                if (tport_tmp->ifindex == ifindex)
                {
                    continue;
                }

                /*主设备选择另一个端口作为工作口*/
                if (LACP_TRUE == tport_tmp->link.rx_flag)
                {
                    tport_tmp->link.selected = LACP_SELECTED;
                    LACP_SET_SYNCHRONIZATION(tport_tmp->link.actor_state);
                    lacp_pkt_tx(trunk_entry, tport_tmp->ifindex);
                }
            }
		}
	}

	return;
}


/*成员端口状态处理*/
static int  lacp_port_state_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*端口状态为选中状态处理*/
    if ((LACP_SELECTED == tport->link.selected) && (LACP_SYNCHRONIZATION_IN_SYNC == LACP_GET_SYNCHRONIZATION(tport->link.partner_state)))
    {
        lacp_port_forwarding(trunk_entry, ifindex);
    }
    else
    {
        lacp_port_blocking(trunk_entry, ifindex);
    }

    lacp_pkt_tx(trunk_entry, ifindex);

    return ERRNO_SUCCESS;
}
/*工作端口选择处理*/
static void lacp_port_select_set(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*负载分担方式，将所有工作端口按从小到大
        顺序排序，发送至hal处理*/
    if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
    {
        trunk_ecmp_port_select(trunk_entry);
        lacp_pkt_tx(trunk_entry, ifindex);
    }
    /*主备方式选择其中的一个端口工作*/
    else
    {
        if (TRUNK_BACK_MEMBER_MAX == trunk_entry->portlist.count)
        {
            tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

            if (NULL == tport)
            {
                return;
            }

            if (tport->link.sync_state)
            {
                if (tport->ifindex == trunk_entry->master_if)
                {
                    trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);

                    for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
                    {
                        if (tport_tmp->ifindex != trunk_entry->master_if)
                        {
                            tport_tmp->link.selected = LACP_UNSELECTED;
                            LACP_CLR_SYNCHRONIZATION(tport_tmp->link.actor_state);

                            lacp_pkt_tx(trunk_entry, tport_tmp->ifindex);
                        }
                    }
                }
                else
                {
                    trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
                }
            }
            else
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
                {
                    if (tport_tmp->ifindex != tport->ifindex)
                    {
                        if (LACP_TRUE == tport_tmp->link.rx_flag)
                        {
                            tport_tmp->link.selected = LACP_SELECTED;
                            LACP_SET_SYNCHRONIZATION(tport_tmp->link.actor_state);

                            lacp_pkt_tx(trunk_entry, tport_tmp->ifindex);
                        }
                    }
                }
            }

        }
        else
        {
            trunk_entry->status = BACKUP_STATUS_INVALID;
        }
    }

    return;
}


/*lacp报文收包处理*/
int lacp_pkt_rx(struct l2if *l2_if, struct pkt_buffer *pkt)
{
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    struct trunk_port *tport = NULL;
    struct trunk *trunk_entry = NULL;
    struct l2if *pport = NULL;
    struct lacp_pkt *lacp_pkt = NULL;

    if ((NULL == l2_if) || (NULL == pkt))
    {
        zlog_err("%s[%d]:leave %s:l2if or pkt NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*获取接口的l2_if数据结构，是否为trunk成员*/
    if (!(pkt->in_port))
    {
        return ERRNO_FAIL;
    }

	if(NULL == pkt->data)
	{
        return ERRNO_FAIL;
    }

    pport = l2if_lookup(pkt->in_port);

    if ((NULL == pport) || (0 == pport->trunkid))
    {
        return ERRNO_FAIL;
    }

    /*获取strunk trunk结构体指针*/
    trunk_entry = trunk_lookup(pport->trunkid);

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*聚合组为使能lacp协议，不处理*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        return ERRNO_FAIL;
    }

    /*获取端口的结构体指针*/
    tport = trunk_port_lookup(trunk_entry->trunkid, pkt->in_port);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*收包与本端mac相同报文不处理*/
	lacp_pkt = (struct lacp_pkt *)pkt->data; 
	if(memcmp(l2_if->mac, lacp_pkt->actor_sys, sizeof(uint8_t)*MAC_LEN) == 0)
	{
        return ERRNO_FAIL;
	}

	/*打印收到的lacp协议报文*/
    lacp_pkt_dump(pkt->data, tport->ifindex, LACP_DUMP_RECEIVE);

	/*将网络字节序转换为主机字节序*/
	lacp_pkt->actor_spri = ntohs(lacp_pkt->actor_spri);
	lacp_pkt->actor_key = ntohs(lacp_pkt->actor_key);
	lacp_pkt->actor_ppri = ntohs(lacp_pkt->actor_ppri);
	lacp_pkt->actor_port = ntohs(lacp_pkt->actor_port);
	lacp_pkt->partner_spri = ntohs(lacp_pkt->partner_spri);
	lacp_pkt->partner_key = ntohs(lacp_pkt->partner_key);
	lacp_pkt->partner_ppri = ntohs(lacp_pkt->partner_ppri);
	lacp_pkt->partner_port = ntohs(lacp_pkt->partner_port);

    tport->link.pdu = lacp_pkt;
    lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_RX_PKT);
    tport->link.pdu = NULL;

    tport->link.pkt_rx++;

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return ERRNO_SUCCESS;
}


/*lacp报文发送，两端都设置被动模式不发送*/
void lacp_pkt_tx(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*端口linkdown或者两端都开启被动模式，不发送*/
    if ((IFNET_LINKDOWN == tport->linkstatus) ||
            ((LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.actor_state))
             && (LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.partner_state))))
    {
        return;
    }

    lacp_tx(trunk_entry, ifindex);

	return;
}

/*接收超时定时器*/
static void lacp_rx_timeout(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*端口状态linkdown不处理*/
    if (IFNET_LINKDOWN == tport->linkstatus)
    {
        return;
    }

    /*超时后，根据本端超时状态更新，默认为短超时*/
    if (tport->link.rx_timer > 1)
    {
        tport->link.rx_timer--;
    }
    else
    {
        tport->link.rx_timer = (3 * trunk_entry->lacp_interval);

        /*只在状态为LACP_RX_EXPIRED和LACP_CURRENT处理超时，
                其他状态不处理*/
        if ((LACP_RX_EXPIRED == tport->link.state) || (LACP_CURRENT == tport->link.state))
        {
            lacp_fsm(trunk_entry, tport->ifindex, LACP_EVENT_RX_TIMEOUT);
        }
    }

    return ;
}
/*封装lacp协议报文*/
static int lacp_pkt_encap(struct trunk *trunk_entry, uint32_t ifindex, struct lacp_pkt *pkt)
{
    struct trunk_port *tport = NULL;
    struct l2if *l2_trunk_if = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*获取trunk接口的l2_if结构体*/
    l2_trunk_if = l2if_get(trunk_entry->ifindex);

    if (NULL == l2_trunk_if)
    {
        zlog_err("%s[%d]:leave %s:The trunk failed to get l2_if\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*获取成员端口结构体*/
    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memset(pkt, 0, sizeof(struct lacp_pkt));
	pkt->subtype = LACP_SUBTYPE;
	pkt->version = LACP_VERSION;

	/* Actor information */
	pkt->actor_tlv_type = LACP_ACTOR_INFO_TLV;
	pkt->actor_infor_len = LACP_ACTOR_INFO_LENGTH;
	pkt->actor_spri = htons(trunk_entry->priority);
	memcpy(pkt->actor_sys, l2_trunk_if->mac, sizeof(uint8_t)*MAC_LEN);
	pkt->actor_key = htons(tport->link.actor_key);
	pkt->actor_ppri = htons(tport->priority);
	pkt->actor_port = htons(tport->link.actor_port);
	pkt->actor_state = tport->link.actor_state;

	/* Partner information */
	pkt->partner_tlv_type = LACP_PARTNER_INFO_TLV;
	pkt->partner_infor_len = LACP_PARTNER_INFO_LENGTH;
	pkt->partner_spri = htons(tport->link.partner_spri);
	memcpy(pkt->partner_sys, tport->link.partner_sys, sizeof(uint8_t)*MAC_LEN);
	pkt->partner_key = htons(tport->link.partner_key);
	pkt->partner_ppri = htons(tport->link.partner_ppri);
	pkt->partner_port = htons(tport->link.partner_port);
	pkt->partner_state = tport->link.partner_state;

	/* Collector information */
	pkt->col_tlv_type = LACP_COLLECTOR_INFO_TLV;
	pkt->col_infor_len = LACP_COLLECTOR_INFO_LENGTH;
	pkt->col_max_delay = LACP_COLLECTOR_MAX_DELAY;
	
	/* Terminator information */
	pkt->ter_tlv_type = LACP_TERMINATOR_TLV;
	pkt->ter_len = LACP_TERMINATOR_LENGTH;

    return ERRNO_SUCCESS;
}

/*报文发送*/
int lacp_tx(struct trunk *trunk_entry, uint32_t ifindex)
{
    int ret = 0;
    union pkt_control pktcontrol;
    uint8_t lacp_mac[MAC_LEN] = LACP_MAC;
    struct trunk_port *tport = NULL;
    struct lacp_pkt pkt;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*封装lacp报文结构体*/
	ret = lacp_pkt_encap(trunk_entry, ifindex, &pkt);
	if(ret != 0)
	{
		zlog_err("%s[%d]:leave %s:encap lacp pkt error\n",__FILE__,__LINE__,__func__);
		return ERRNO_FAIL;
	}

    /*封装报文头*/
	memset(&pktcontrol, 0, sizeof(union pkt_control));
    memcpy(pktcontrol.ethcb.dmac, lacp_mac, sizeof(uint8_t)*MAC_LEN);
	memcpy(pktcontrol.ethcb.smac, l2_mac, sizeof(uint8_t)*MAC_LEN);
    pktcontrol.ethcb.ethtype = LACP_TYPE;
    pktcontrol.ethcb.smac_valid = 1;
    pktcontrol.ethcb.is_changed = 1;
    pktcontrol.ethcb.ifindex = ifindex;
	pktcontrol.ethcb.cos = 6;

    /*发送lacp协议报文*/
    ret = pkt_send(PKT_TYPE_ETH, &pktcontrol, &pkt, sizeof(struct lacp_pkt));
    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:pkt_send error\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }
	
   	/*打印发送的lacp协议报文*/
    lacp_pkt_dump((void *)&pkt, ifindex, LACP_DUMP_SEND);
    tport->link.pkt_tx++;
	
    return ERRNO_SUCCESS;
}

/*周期发送报文，根据对端超时状态进行发送*/
static void lacp_periodic_tx(struct trunk *trunk_entry, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*端口linkdown或者两端都开启被动模式，不发送*/
    if ((IFNET_LINKDOWN == tport->linkstatus) ||
            ((LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.actor_state))
             && (LACP_ACTIVITY_PASSIVE == LACP_GET_ACTIVITY(tport->link.partner_state))))
    {
        return;
    }

    /*依据对端超时状态发送lacp协议报文*/
    if (tport->link.tx_timer > 1)
    {
        tport->link.tx_timer--;
    }
    else
    {
        if (LACP_RX_EXPIRED == tport->link.state)
        {
            tport->link.tx_timer = LACP_FAST_PERIODIC_TIME;
        }
        else
        {
            tport->link.tx_timer = trunk_entry->lacp_interval;
        }
        
        lacp_pkt_tx(trunk_entry, ifindex);
    }
    return ;
}

/*lacp定时器，1s一次*/
//int lacp_timer_start(struct thread *thread)
int lacp_timer_start(void *para)
{
    int i = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;

    for (i = 1; i <= IFM_TRUNK_MAX; i++)
    {
        trunk_entry = trunk_lookup(i);

        if (NULL == trunk_entry)
        {
            continue;
        }

        if (TRUNK_ENABLE == trunk_entry->lacp_enable)
        {
            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                lacp_periodic_tx(trunk_entry, tport->ifindex);
                lacp_rx_timeout(trunk_entry, tport->ifindex);
            }
        }
    }
    
	//thread_add_timer (l2_master, lacp_timer_start, NULL, 1);

    return ERRNO_SUCCESS;
}


/*lacp信息初始化*/
static void lacp_port_info_init(struct trunk *trunk_entry, uint32_t ifindex)
{
    int ret = 0;
    struct trunk_port *tport = NULL;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    memset(&(tport->link), 0, sizeof(struct lacp_link));
	/*设置发送间隔，接收超时(3倍发送间隔)*/
    tport->link.sync_state = 0;
    tport->link.rx_timer = (3*trunk_entry->lacp_interval);
    tport->link.tx_timer = trunk_entry->lacp_interval;
    tport->link.selected = LACP_UNSELECTED;
    tport->link.rx_flag = LACP_FALSE;
    tport->link.pdu = NULL;

    /* Actor information */
    tport->link.actor_port = IFM_PORT_ID_GET ( tport->ifindex );
    /*key值有trunkid、速率和双工模式组成，trunkid占高字节，
         速率和双工占低字节，低字节高四位为速率，低四位
         为双工模式，参考华为设置key值，速率:0011代表GE ,0010
         代表FE,0001 代表全双工(华为设备速率:0001 代表10M ,0010代表
         100M, 0011代表自动) */
    tport->link.actor_key = ((0xff00 & ((trunk_entry->trunkid) << 8)) | 0x31);

    if (LACP_TRUE == tport->passive)
    {
        LACP_CLR_ACTIVITY(tport->link.actor_state);
    }
    else
    {
        LACP_SET_ACTIVITY(tport->link.actor_state);
    }
    LACP_SET_AGGREGATION(tport->link.actor_state);     
    LACP_SET_DEFAULTED(tport->link.actor_state); 
	LACP_SET_TIMEOUT (tport->link.actor_state);
    LACP_CLR_COLLECTING (tport->link.actor_state);
    LACP_CLR_DISTRIBUTING (tport->link.actor_state);

    /* Partner information */
    LACP_SET_DEFAULTED (tport->link.partner_admin_state);
    LACP_SET_AGGREGATION (tport->link.partner_admin_state);
    LACP_SET_TIMEOUT (tport->link.partner_admin_state);
    LACP_SET_EXPIRED(tport->link.partner_admin_state);
    tport->link.partner_state = tport->link.partner_admin_state;

    /*statistics*/
    tport->link.pkt_tx = 0;
    tport->link.pkt_rx = 0;

	/*set block*/
    tport->link.port_state = IFNET_STP_BLOCK;
	ret = ifm_set_block(tport->ifindex, MODULE_ID_L2, IFNET_STP_BLOCK);
	if (ret < 0)
	{
		zlog_err("%s[%d]:%s:lacp: set block error on\n",__FILE__,__LINE__,__func__);
		return;
	}
	
    return ;
}


/*注册收包处理*/
void lacp_pkt_rx_register(void)
{
	union proto_reg proto;
	unsigned char lacp_mac[6] = {0x1, 0x80, 0xc2, 0x00, 0x00, 0x02};

	memset(&proto, 0, sizeof(union proto_reg));
	memcpy(proto.ethreg.dmac, lacp_mac, 6);
	proto.ethreg.dmac_valid = 1;
	proto.ethreg.ethtype = ETH_P_LACP;
	proto.ethreg.sub_ethtype = ETH_SUBTYPE_LACP;

	pkt_register(MODULE_ID_L2, PROTO_TYPE_ETH, &proto);	

    return ;
}


/*初始化lacp收发包开关*/
void lacp_pkt_debug_init(void)
{
	lacp_recv_dbg = 0;
	lacp_send_dbg = 0;

	return;
}


/*lacp事件处理状态机*/
void lacp_fsm(struct trunk *trunk_entry, uint32_t ifindex, enum LACP_EVENT event)
{
    LACP_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    struct trunk_port *tport = NULL;
    uint32_t sync_state_before = 0, sync_state = 0,recv_before_sync_state = 1;

    tport = trunk_port_lookup(trunk_entry->trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:tport NULL\n", __FILE__, __LINE__, __func__);
        return;
    }

    /*处理前sync状态*/
    sync_state_before = tport->link.sync_state;
    switch(event)
    {
        case LACP_EVENT_ADD_PORT:
        case LACP_EVENT_ENABLE_LACP:
        case LACP_EVENT_WORK_MODE:
        {
            lacp_port_info_init(trunk_entry, ifindex);

            if (IFNET_LINKUP == tport->linkstatus)
            {
                lacp_port_linkup_set(trunk_entry, ifindex);
                lacp_tx(trunk_entry, ifindex);

                tport->link.state = LACP_RX_EXPIRED;

            }
            else
            {
                tport->link.state = LACP_INVALID;
            }
			
       	 	break;
        }

        case LACP_EVENT_PORT_LINKUP:
        {
            lacp_port_linkup_set(trunk_entry, ifindex);
            lacp_tx(trunk_entry, ifindex);

            tport->link.state = LACP_RX_EXPIRED;
			
        	break;
        }

        case LACP_EVENT_PORT_LINKDOWN:
        {
            lacp_port_linkdown_set(trunk_entry, ifindex);
            lacp_port_blocking(trunk_entry, ifindex);

            tport->link.state = LACP_PORT_DISABLED;
			
        	break;
        }

        case LACP_EVENT_RX_TIMEOUT:
        {
            if (LACP_CURRENT == tport->link.state)
            {
                lacp_rx_expired_set(trunk_entry, ifindex);

                tport->link.state = LACP_RX_EXPIRED;
            }
            else if (LACP_RX_EXPIRED == tport->link.state)
            {
                lacp_rx_defaulted_set(trunk_entry, ifindex);
                lacp_port_blocking(trunk_entry, ifindex);
                lacp_port_linkdown_timeout_set(trunk_entry, ifindex);

                tport->link.state = LACP_DEFAULT;
            }
			
        	break;
        }

        case LACP_EVENT_RX_PKT:
        {
            /*保存收包原始状态信息，防止hal层快速收发包逻辑将端口block*/
	recv_before_sync_state = LACP_GET_SYNCHRONIZATION(tport->link.pdu->actor_state) & LACP_GET_SYNCHRONIZATION(tport->link.pdu->partner_state);
            if ((LACP_RX_EXPIRED == tport->link.state) || (LACP_CURRENT == tport->link.state) ||
                    (LACP_DEFAULT == tport->link.state))
            {
                lacp_rx_current_set(trunk_entry, ifindex);
                lacp_select_logic_set(trunk_entry, ifindex);

                tport->link.state = LACP_CURRENT;
            }
					
        	break;
        }

        default:
            break;

    }

    /*处理后的sync状态，如果不相同，则重新处理*/
    sync_state = LACP_GET_SYNCHRONIZATION(tport->link.actor_state) & LACP_GET_SYNCHRONIZATION(tport->link.partner_state);

    if (sync_state_before != sync_state || (sync_state_before && !recv_before_sync_state) )
    {
        tport->link.sync_state = sync_state;
        lacp_port_select_set(trunk_entry, ifindex);
        lacp_port_state_set(trunk_entry, ifindex);
        trunk_linkstatus_set(trunk_entry);
        trunk_speed_set(trunk_entry->trunkid);
        /*上报/清除告警*/
        trunk_member_alarm_report(trunk_entry->trunkid, ifindex);
    }

    LACP_LOG_DBG("%s[%d]: Leaving function '%s'.(state:%u)\n", __FILE__, __LINE__, __func__, tport->link.state);
    return;
}

