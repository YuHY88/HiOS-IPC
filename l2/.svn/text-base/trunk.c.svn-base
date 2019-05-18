#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/linklist.h>
#include <lib/errcode.h>
#include <lib/types.h>
#include <lib/alarm.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include <unistd.h> 
#include "l2/trunk.h"
#include "lacp/lacp.h"
#include "l2_if.h"
#include "l2_snmp.h"
#include <lib/hptimer.h>
#include "memshare.h"
#include "msg_ipc_n.h"
#include "mstp/mstp_init.h"
#include "l2_msg.h"

extern struct thread_master *l2_master;
struct trunk_config trunk_gloable_config;
/*******************************************************************************
 功能描述:trunk表初始化
 输入参数:无
 输出参数:无
 返回值域:无
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
void trunk_table_init ( void )
{
    int i = 0;
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    for(i = 0; i < IFM_TRUNK_MAX; i++)
    {
		trunk_table[i] = NULL;
    }
    
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n",__FILE__,__LINE__,__func__);
    return ;

}

/*******************************************************************************
 功能描述:通过trunkid进行查找是否存在对应的表
 输入参数:uint16_t trunkid 聚合组id
 输出参数:无
 返回值域:NULL或者trunk_entry
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
struct trunk *trunk_lookup( uint16_t trunkid )
{
    struct trunk *trunk_entry = NULL;
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return NULL;
    }

    /*判断trunk id对应的trunk结构体是否为NULL*/
    trunk_entry = trunk_table[trunkid - 1];
    if (NULL == trunk_entry)
    {
        return NULL;
    }
    
    return trunk_entry;
}

/*
* return:
* 	0 --> success
*  -1 --> error
* pada can be NULL, if so,the length should be 0.
* should delete this func.
*/
int trunk_comm_send_msg_wait_ack(uint16_t msg_subtype, uint8_t opcode,uint32_t length, void *pdata)
{
    int               iResult = 0;
    int               iRevLen = 0;
    int		  ret = 0;
    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;
 
    if (NULL == pdata)
    {    	
        TRUNK_LOG_DBG("%s[%d]: pada is NULL!!!\r\n", __FILE__, __LINE__);
        return -1;
    }

    pSndMsg =  ipc_msg_malloc(sizeof(struct ipc_mesg_n) + length, MODULE_ID_L2);
    
    if (NULL == pSndMsg)
    {
        TRUNK_LOG_DBG("%s[%d]:malloc share memory fail!!!\r\n", __FILE__, __LINE__);
        return -1;
    }     

    memset(pSndMsg, 0, sizeof(struct ipc_mesg_n));
    
    pSndMsg->msghdr.module_id = MODULE_ID_HAL;
    pSndMsg->msghdr.sender_id = MODULE_ID_L2;
    pSndMsg->msghdr.msg_type  = IPC_TYPE_TRUNK;
    pSndMsg->msghdr.msg_subtype = msg_subtype;
    pSndMsg->msghdr.data_num = 1;
    pSndMsg->msghdr.opcode = opcode;
    pSndMsg->msghdr.sequence  = 0;
	pSndMsg->msghdr.data_len = length;

	memcpy(pSndMsg->msg_data, pdata, length);

    iResult = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + length, &pRcvMsg, &iRevLen, 2000);

    if (iResult == -1)
    {
        ipc_msg_free(pSndMsg,MODULE_ID_L2);
		TRUNK_LOG_DBG("%s[%d]:send message error\n", __FILE__, __LINE__);
        return -1;
    }
    else if(iResult == -2)
    {
         TRUNK_LOG_DBG("%s[%d]:receive message error\n", __FILE__, __LINE__);
         return -1;
    }
    else  
    {
        if( pRcvMsg != NULL )
        {
        	ret = (int)pRcvMsg->msg_data;
             //memcpy(para_dev_box, pRcvMsg->msg_data, sizeof(struct devm_box));
             ipc_msg_free(pRcvMsg,MODULE_ID_L2);
             TRUNK_LOG_DBG("%s[%d]: sucessfully \n", __FILE__, __LINE__);
             return(ret);
        }
        else
			return(-1);
    }
}



/*******************************************************************************
 功能描述:通过trunkid进行创建trunk表
 输入参数:uint16_t trunkid 聚合组id
 输出参数:无
 返回值域:NULL或者trunk_entry
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
struct trunk *trunk_add( uint16_t trunkid )
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;

	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return NULL;
    }
    
    /*查找是否存在trunk id对应的trunk,若存在则返回其信息*/
    trunk_entry = trunk_lookup(trunkid);
    if(trunk_entry != NULL)
    {
        return trunk_entry;
    }

    /*申请trunk对应的内存，并初始化*/
    trunk_entry = (struct trunk *)XMALLOC(MTYPE_L2, sizeof(struct trunk));

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:error:fail to XMALLOC struct trunk\n", __FILE__, __LINE__, __func__);
        return NULL;
    }
	
    memset(trunk_entry, 0, sizeof(struct trunk));
	
    trunk_entry->ifindex        = IFM_TRUNK_IFINDEX_GET(trunkid);
    trunk_entry->trunkid        = trunkid;
    trunk_entry->speed          = 0;
    trunk_entry->work_mode      = TRUNK_MODE_ECMP;
    trunk_entry->failback       = TRUNK_SWITCH_NOFAILBACK;
    trunk_entry->wtr            = TRUNK_WTR_DEFAULT;
    trunk_entry->priority       = TRUNK_PRIORITY_DEFAULT;
    trunk_entry->lacp_enable    = TRUNK_DISABLE;
    trunk_entry->lacp_interval  = TRUNK_INTERVAL_DEFAULT;
    trunk_entry->ecmp_mode = trunk_gloable_config.gloable_balance_mode;
    trunk_entry->status          = BACKUP_STATUS_INVALID;
    trunk_entry->master_if      = 0;
    trunk_entry->upport_num     = 0;
    trunk_entry->down_flag      = IFNET_LINKDOWN;
    trunk_entry->fb_timer       = 0;
     trunk_entry->trunk_under_interface_flag = 0;
    memset(trunk_entry->member, 0, sizeof(uint32_t)*TRUNK_MEMBER_MAX);

    /*聚合组创建消息发送至hal，进行处理*/
	//ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_ADD, 0 );
	ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
					MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_ADD, 0 );

	if(ret != 0)
    {
        zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
        XFREE(MTYPE_L2, trunk_entry);
        return NULL;
    }

    trunk_table[trunkid - 1] = trunk_entry;  
    /*创建聚合组，无成员端口，上报trunk组不可用告警*/
	/*ipran_alarm_port_register(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(trunk_entry->ifindex),
							        IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0);

	ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(trunk_entry->ifindex), 
						IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);*/

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
	gPortInfo.iIfindex = trunk_entry->ifindex;
	ipran_alarm_port_register(&gPortInfo);

	ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);
    TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return trunk_entry;
}

/*******************************************************************************
 功能描述:删除trunkid对应的trunk表
 输入参数:uint16_t trunkid 聚合组id
 输出参数:无
 返回值域:ERRNO_SUCCESS 成功 ERRNO_FAIL失败
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_delete( uint16_t trunkid )
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct l2if *l2_if = NULL;
	
    
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }
    
    /*查找是否存在trunk id*/
    trunk_entry = trunk_lookup(trunkid);
    if (NULL == trunk_entry)
    {
        return ERRNO_SUCCESS;
    }

	/*trunk组中存在成员端口不删除*/
	if(trunk_entry->portlist.count != 0)
	{
		return ERRNO_SUCCESS;
	}
	
    /*将聚合组函数事件发送至hal进行处理*/
    //ret = ipc_send_hal_wait_ack( &trunkid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_DELETE, 0 );
	ret = l2_msg_send_hal_wait_ack( &trunkid, sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, 
							IPC_TYPE_TRUNK, TRUNK_INFO_TRUNK, IPC_OPCODE_DELETE, 0 );

	if(ret != 0)
    {
        zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
        return ERRNO_FAIL;
    }   

    /*删除trunk中存在的所有成员口*/
    for(ALL_LIST_ELEMENTS(&(trunk_entry->portlist), node, nnode, tport))
    {
        l2_if = l2if_get(tport->ifindex);
        if(l2_if != NULL)
        {
            l2_if->trunkid = 0;
        } 
        listnode_delete(&(trunk_entry->portlist), tport);
		XFREE(MTYPE_L2, tport);
    }  

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
	gPortInfo.iIfindex = trunk_entry->ifindex;
	ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
	ipran_alarm_port_unregister(&gPortInfo);	

    //THREAD_TIMER_OFF(trunk_entry->fb_timer);
    /*释放trunk申请的资源*/
    XFREE(MTYPE_L2, trunk_entry);
    trunk_table[trunkid - 1] = NULL;
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

    return ERRNO_SUCCESS;
}

int trunk_linkstatus_set(struct trunk *trunk_entry)
{
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    int flag = 0;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
	gPortInfo.iIfindex = trunk_entry->ifindex;
    TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        /*linkup设置*/
        if (IFNET_LINKDOWN == trunk_entry->down_flag)
        {
            if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
            {
                if ((TRUNK_BACK_MEMBER_MAX == trunk_entry->portlist.count) && (trunk_entry->upport_num != 0))
                {
                    trunk_entry->down_flag = IFNET_LINKUP;
                    /*发送至hal*/
	                    /*ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, 0 );
	                    if(ret != 0)
	                    {
	                        zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
	                        return ERRNO_FAIL;
	                    }*/
						
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
                }
            }
            else
            {
                if (trunk_entry->upport_num != 0)
                {
                    trunk_entry->down_flag = IFNET_LINKUP;
                    /*清除告警*/
					/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(trunk_entry->ifindex), 
							IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);*/

					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
                }
            }
        }
        /*linkdown设置*/
        else
        {
            if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
            {
                if ((trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX) ||
                        ((TRUNK_BACK_MEMBER_MAX == trunk_entry->portlist.count) && (0 == trunk_entry->upport_num)))
                {
                    trunk_entry->down_flag = IFNET_LINKDOWN;
                    /*发送至hal*/
                    /*ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, 0 );
                    if(ret != 0)
                    {
                        zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
                        return ERRNO_FAIL;
                    }*/
					/*上报告警*/
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);
                }
            }
            else
            {
                if (0 == trunk_entry->upport_num)
                {
                    trunk_entry->down_flag = IFNET_LINKDOWN;
                    /*发送至hal*/
                   /* ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, 0 );
                    if(ret != 0)
                    {
                        zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
							IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);*/

					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);
                }
            }
        }
    }
    else
    {
        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
        {
            if(tport_tmp->link.sync_state)
            {
                flag = 1;
                break;
            }
        } 
        
        if (IFNET_LINKDOWN == trunk_entry->down_flag)
        {
            if(flag)
            {
                trunk_entry->down_flag = IFNET_LINKUP;
                /*发送至hal*/
                /*ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_SHUTDOWN, IPC_OPCODE_UPDATE, 0 );
                if(ret != 0)
                {
                    zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
							IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);*/

				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_CLEAN);
            }
        }
        else
        {
            if (!flag)
            {
                trunk_entry->down_flag = IFNET_LINKDOWN;
                /*上报告警*/
				/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(trunk_entry->ifindex), 
							IFM_PORT_ID_GET(trunk_entry->ifindex), 0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);*/

				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_DOWN, GPN_SOCK_MSG_OPT_RISE);
            }
        }
    }
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

    return ERRNO_SUCCESS;
}
/*根据端口优先级与端口号选择主端口*/
int trunk_backup_port_select(struct trunk *trunk_entry)
{
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    uint16_t port_priority = 0;
    uint32_t ifindex = 0;
    
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)
    {
        trunk_entry->master_if = 0;
        trunk_entry->status = BACKUP_STATUS_INVALID;
    }
    else
    {
        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
        {
            port_priority = tport_tmp->priority;
            ifindex = tport_tmp->ifindex;
            break;
        }

        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
        {
            if((tport_tmp->priority < port_priority) || 
               ((tport_tmp->priority == port_priority) && (tport_tmp->ifindex < ifindex)))
            {
                ifindex = tport_tmp->ifindex;
            }
        }

        trunk_entry->master_if = ifindex;
    }

	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.(master_if:0x%x)\n", __FILE__, __LINE__, __func__,trunk_entry->master_if);

    return ERRNO_SUCCESS;
}

int trunk_ecmp_port_select(struct trunk *trunk_entry)
{
    struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    int num = 0,i = 0,j = 0;
    uint32_t ifindex_tmp = 0;
    int ret = 0;
    TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk_entry NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    memset(trunk_entry->member, 0, sizeof(uint32_t)*TRUNK_MEMBER_MAX);

    if (trunk_entry->lacp_enable != TRUNK_ENABLE)
    {
        /*获取工作端口*/
        for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
        {
            if (IFNET_LINKUP == tport_tmp->linkstatus)
            {
                trunk_entry->member[num] = tport_tmp->ifindex;
                num++;
            }
        }
    }
    else
    {
        /*获取工作端口*/
        for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
        {
            if(tport_tmp->link.sync_state)
            {
                trunk_entry->member[num] = tport_tmp->ifindex;
                num++;
            }
        }
    }

    /*将工作端口从小到大进行排序*/
    if(num)
    {
        for(i=0; i<num; i++)
        {
            for(j=i; j<num; j++)
            {
                if(trunk_entry->member[i] > trunk_entry->member[j])
                {
                    ifindex_tmp = trunk_entry->member[i];
                    trunk_entry->member[i] = trunk_entry->member[j];
                    trunk_entry->member[j] = ifindex_tmp;
                }
            }
        }

        /*用于调试*/
        for(i=0;i<num;i++)
        {
            TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.(ifindex:0x%x)\n", __FILE__, __LINE__, __func__,trunk_entry->member[i]);
        }
        
        /*将排好序的端口列表发送至hal*/
        //ret = ipc_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT_SELECT_SET, IPC_OPCODE_UPDATE, 0 );
			ret = l2_msg_send_hal_wait_ack( trunk_entry, sizeof(struct trunk), 1, MODULE_ID_HAL, 
							MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT_SELECT_SET, IPC_OPCODE_UPDATE, 0 );
        if(ret != 0)
        {
            zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
            return ERRNO_FAIL;
        }  
    }   
    
    TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return ERRNO_SUCCESS;
}

/*******************************************************************************
 功能描述:主备方式下，backup回切处理定时器
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
/*static int trunk_backup_failback_timer (struct thread *thread)*/
static int trunk_backup_failback_timer (void *para)
{
    struct trunk *trunk_entry = NULL;   
    struct l2if *l2_if = NULL;
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    /*trunk_entry = (struct trunk *)THREAD_ARG(thread);*/
	trunk_entry = (struct trunk *)para;

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:trunk entry null\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    //THREAD_TIMER_OFF(trunk_entry->fb_timer);

    /*先判断主端口是否仍然up*/
    l2_if = l2if_get(trunk_entry->master_if);
    if(l2_if != NULL)
    {
        if (IFNET_LINKUP == l2_if->down_flag)
        {
            trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);

            /*上报/清除告警*/
			trunk_member_alarm_report(trunk_entry->trunkid, trunk_entry->master_if);
        }
    } 

	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.%u\n", __FILE__, __LINE__, __func__,trunk_entry->trunkid);

    return ERRNO_SUCCESS;
}
/*******************************************************************************
 功能描述:trunk成员端口up处理
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_port_up( uint16_t trunkid, uint32_t ifindex )
{
    struct trunk *trunk_entry = NULL; 
    struct trunk_port *tport = NULL;
	struct l2if *t_l2if = NULL;

	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    /*聚合组id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }

    /*查找trunk id对应的trunk表*/
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup trunk\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*查找端口所在结构体*/
    tport = trunk_port_lookup(trunkid, ifindex);

    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup port\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*up数量加1*/
    trunk_entry->upport_num++;

    /*设置端口的link状态*/
    tport->linkstatus = IFNET_LINKUP;

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)
            {
                trunk_entry->status = BACKUP_STATUS_INVALID;

                /*上报/清除告警*/
                trunk_member_alarm_report(trunkid, ifindex);
            }
            else
            {
                if (1 == trunk_entry->upport_num)
                {
					if(trunk_entry->master_if == ifindex)
					{
						trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
					}
					else
					{
						trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
					}
					
					/*上报/清除告警*/
					trunk_member_alarm_report(trunkid, ifindex);
				}
				else
				{
					if(trunk_entry->master_if == ifindex)
                    {
                        if (TRUNK_SWITCH_FAILBACK == trunk_entry->failback)
                        {
                        /*
                            THREAD_TIMER_OFF(trunk_entry->fb_timer);
                            THREAD_TIMER_ON(l2_master, trunk_entry->fb_timer, trunk_backup_failback_timer, trunk_entry, trunk_entry->wtr);
				*/
			       high_pre_timer_add("LacpFailbackTimer", LIB_TIMER_TYPE_NOLOOP, 
										trunk_backup_failback_timer, (void*)trunk_entry, 1000*trunk_entry->wtr);
                        }
                    }
				}
			}
        }
        else
        {
            trunk_ecmp_port_select(trunk_entry);
			
			/*上报/清除告警*/
			trunk_member_alarm_report(trunkid, ifindex);
        }
    }
    else
    {
        lacp_fsm(trunk_entry, ifindex, LACP_EVENT_PORT_LINKUP);
    }

	/*trunk link状态设置*/
    trunk_linkstatus_set(trunk_entry);
    trunk_speed_set(trunk_entry->trunkid);
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

	/*modify by youcheng 2019/1/10 mstp support trunk*/
	t_l2if = l2if_lookup(trunk_entry->ifindex);
	if(t_l2if && t_l2if->mstp_port_info)
	{
		mstp_reinit();	
	}
	
    return ERRNO_SUCCESS;
}
/*******************************************************************************
 功能描述:trunk成员端口down处理
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_port_down( uint16_t trunkid, uint32_t ifindex )
{
    struct trunk *trunk_entry = NULL;  
    struct trunk_port *tport = NULL;
	struct l2if * t_l2if = NULL;

	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    /*聚合组id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }
    
    /*查找trunk id对应的trunk表*/
    trunk_entry = trunk_lookup( trunkid  );
    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup trunk\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }

    /*查找端口所在结构体*/
    tport = trunk_port_lookup(trunkid, ifindex);
    if (NULL == tport)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup port\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }
    /*up数量减1*/
    trunk_entry->upport_num--;

    /*设置端口的link状态*/
    tport->linkstatus = IFNET_LINKDOWN;

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            /*成员口小于2不处理*/
            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)
            {
				trunk_entry->status = BACKUP_STATUS_INVALID;
            }
			else
			{
				/*如果存在up端口，进行选择*/
                if (1 == trunk_entry->upport_num)
				{
					/*主端口发生down事件，选择备端口工作*/
					if(trunk_entry->master_if == ifindex)
					{
						trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
					}
					else
					{
						trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
					}
				}
				else
				{
					trunk_entry->status = BACKUP_STATUS_INVALID;
				}
			}
        }
        else
        {
            trunk_ecmp_port_select(trunk_entry);
        }
		
		/*上报/清除告警*/
		trunk_member_alarm_report(trunkid, ifindex);
    }
    else
    {
        lacp_fsm(trunk_entry, ifindex, LACP_EVENT_PORT_LINKDOWN);
    }

	/*trunk link状态设置*/
    trunk_linkstatus_set(trunk_entry);
    trunk_speed_set(trunk_entry->trunkid);
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

	/*modify by youcheng 2019/1/10 mstp support trunk*/
	t_l2if = l2if_lookup(trunk_entry->ifindex);
	if(t_l2if && t_l2if->mstp_port_info)
	{
		mstp_reinit();	
	}
    return ERRNO_SUCCESS;
}
/*******************************************************************************
 功能描述:向trunkid对应的trunk表中添加端口成员,并设置主或者备工作
                            1、有一个端口up时，选举up 的口工作
                            2、有两个口up/down，有主端口，选举主端口
                            3、有两个口up/down，无主端口，选举索引小的备端口工作
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_port_add( uint16_t trunkid, struct trunk_port *pport)
{
    int ret = 0;
    struct trunk *trunk_entry = NULL;    
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    struct l2if *l2_if = NULL;

	struct gpnPortInfo gPortInfo;
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    if (NULL == pport)
    {
        zlog_err("%s[%d]:leave %s:trunk_port NULL\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*判断trunk id是否有效*/
    if ((trunkid > IFM_TRUNK_MAX) || (trunkid < 1))
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n", __FILE__, __LINE__, __func__);
        return ERRNO_FAIL;
    }

    /*查找是否存在trunk id*/
    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup trunk\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }

    /*查找不到，将端口信息添加至链表中*/
    if ( listnode_lookup ( &(trunk_entry->portlist), pport ) == NULL )
    {
        /*发送至hal进行新端口的添加*/
        //ret = ipc_send_hal_wait_ack( &(trunkid), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_ADD, pport->ifindex );
		 ret = l2_msg_send_hal_wait_ack( &(trunkid), sizeof(uint16_t), 1, MODULE_ID_HAL,
		 			MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_ADD, pport->ifindex );
        if(ret != 0)
        {
        	/*HT201只支持4个成员口，hal返回超过最大限制*/
        	if(ret == ERRNO_OVERSIZE)
        	{
				return ERRNO_OVERSIZE;
			}
            zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
            return ERRNO_FAIL;
        }
    
        /*hal处理成功后，将结果保存至链表中*/
        listnode_add ( &(trunk_entry->portlist), pport );
    
		/* modify for ipran by lipf, 2018/4/25 */		
		memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
		gPortInfo.iAlarmPort = IFM_TRUNK_TYPE;
		gPortInfo.iIfindex = pport->ifindex;
		ipran_alarm_port_register(&gPortInfo);
    }

    /*将trunk成员信息保存至l2if中，并选举主/备工作*/
    l2_if = l2if_get(pport->ifindex);
    if(l2_if != NULL)
    {
        l2_if->trunkid = trunkid;
        if (IFNET_LINKUP == l2_if->down_flag)
        {
            trunk_entry->upport_num++;
            pport->linkstatus = IFNET_LINKUP;

        }
 
     }
     else
     {
        zlog_err ( "%s[%d] l2if_get fail\n", __FUNCTION__, __LINE__);
        return ERRNO_FAIL;
     }

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            /*选择主端口*/
            trunk_backup_port_select(trunk_entry);

            if (trunk_entry->portlist.count != TRUNK_BACK_MEMBER_MAX)


            {
                trunk_entry->status = BACKUP_STATUS_INVALID;
            }
            else
            {
                if (1 == trunk_entry->upport_num)
				{
					for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
					{
						if(tport->linkstatus != IFNET_LINKUP)
						{
							continue;
						}
				
						if(tport->ifindex == trunk_entry->master_if)
						{
							trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
						}
						else
						{
							trunk_backup_switch(trunk_entry, BACKUP_STATUS_SLAVE);
						}
					}
				}
                else if (TRUNK_BACK_MEMBER_MAX == trunk_entry->upport_num)
				{
					trunk_backup_switch(trunk_entry, BACKUP_STATUS_MASTER);
				}
				else
				{
					trunk_entry->status = BACKUP_STATUS_INVALID;
				}
			}
        }
        /*负载分担模式，根据up的端口排序发至hal*/
        else
        {
            trunk_ecmp_port_select(trunk_entry);
        }

		/*上报/清除告警*/
		trunk_member_alarm_report(trunkid, pport->ifindex);
    }
    else
    {
        if (TRUNK_MODE_BACKUP == trunk_entry->work_mode)
        {
            /*选择主端口*/
            trunk_backup_port_select(trunk_entry);
        }
    
        lacp_fsm(trunk_entry, pport->ifindex, LACP_EVENT_ADD_PORT);
		
		/*上报/清除告警*/
        trunk_member_alarm_report(trunk_entry->trunkid, pport->ifindex);
		if(pport->linkstatus == IFNET_LINKUP)
		{
			lacp_fsm(trunk_entry, pport->ifindex, LACP_EVENT_PORT_LINKUP);
		}
    }
	/*trunk link状态设置*/
    trunk_linkstatus_set(trunk_entry);
    trunk_speed_set(trunk_entry->trunkid);
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return ERRNO_SUCCESS;
}

/*******************************************************************************
 功能描述:向trunkid对应的trunk表中删除端口成员
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_port_delete( uint16_t trunkid, uint32_t ifindex )
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
	struct listnode *nnode = NULL;
    struct l2if *l2_if = NULL;
	struct l2if * t_l2if = NULL;
    int ret = 0;

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }
    
    /*查找是否存在trunk id*/
    trunk_entry = trunk_lookup( trunkid  );
    if (NULL == trunk_entry)
    {
        zlog_err("%s[%d]:leave %s:failed to lookup trunk\n",__FILE__,__LINE__,__func__);
        return ERRNO_FAIL;
    }

    /*删除成员口*/
    for(ALL_LIST_ELEMENTS(&(trunk_entry->portlist), node, nnode, tport))
    {
        if(tport->ifindex == ifindex)
        {
			/*端口删除前，清除本端sync,并发送至对端*/
            if (TRUNK_ENABLE == trunk_entry->lacp_enable)
			{
				lacp_port_delect_set(trunk_entry, ifindex);
			}
		
            //ret = ipc_send_hal_wait_ack( &(trunkid), sizeof(uint16_t), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_DELETE, tport->ifindex );
			ret = l2_msg_send_hal_wait_ack( &(trunkid), sizeof(uint16_t), 1, MODULE_ID_HAL,
		 			MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_PORT, IPC_OPCODE_DELETE, tport->ifindex );
            if(ret != 0)
            {
                zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
                return ERRNO_FAIL;
            }
			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = ifindex;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
			ipran_alarm_port_unregister(&gPortInfo);
            listnode_delete(&(trunk_entry->portlist), tport);
            XFREE(MTYPE_L2, tport);
            break;
        }
    } 
    
    /*将trunk成员信息保存至l2if中*/
    l2_if = l2if_get(ifindex);
    if(l2_if != NULL)
    {
        l2_if->trunkid = 0;
        if (IFNET_LINKUP == l2_if->down_flag)
        {
            trunk_entry->upport_num--;
        }
    	l2if_trunk_delete_port(trunkid, ifindex);
    }     
    else
    {
        zlog_err ( "%s[%d] l2if_get fail\n", __FUNCTION__, __LINE__);
        return ERRNO_FAIL;
    }

    /*负载分担，端口删除，重新发送工作口列表至hal*/
    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            trunk_ecmp_port_select(trunk_entry);
        }
		else
		{
			trunk_entry->master_if = 0;
			trunk_entry->status = BACKUP_STATUS_INVALID;
			/*上报/清除告警*/
			trunk_member_alarm_report(trunkid, ifindex);
		}
    }

	/*trunk link状态设置*/
    trunk_linkstatus_set(trunk_entry);
    trunk_speed_set(trunk_entry->trunkid);

	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

	/*modify by youcheng 2019/1/10 function:mstp support trunk*/
	
	t_l2if = l2if_lookup(trunk_entry->ifindex);
	if(t_l2if && t_l2if->mstp_port_info)
	{
		mstp_reinit();		

	}
    return ERRNO_SUCCESS;
}

/*******************************************************************************
 功能描述:向trunkid对应的trunk表中查找对应的端口
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
struct trunk_port * trunk_port_lookup(uint16_t trunkid, uint32_t ifindex )
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return NULL;
    }

    /*是否存在trunk id对应的trunk表*/
    trunk_entry = trunk_lookup( trunkid  );
    if (NULL == trunk_entry)
    {
        return NULL;
    }

    /*遍历成员链表是否存在*/
    for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
    {
        if(tport->ifindex == ifindex)
        {
            return tport;
        }
    } 
 
    return NULL;
}

/*******************************************************************************
 功能描述:初始化ifindex对应的结构体
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
struct trunk_port * trunk_port_init(uint16_t trunkid, uint32_t ifindex)
{
    struct trunk_port *tport = NULL;
    
    tport = (struct trunk_port *)XMALLOC(MTYPE_L2, sizeof(struct trunk_port));
    if (NULL == tport)
	{
        zlog_err("%s[%d]:leave %s:error:fail to XMALLOC struct trunk_port\n", __FILE__, __LINE__, __func__);
		return NULL;
	}
    memset(tport, 0, sizeof(struct trunk_port));
    tport->ifindex  = ifindex;
    tport->priority = TRUNK_PRIORITY_DEFAULT;
    tport->passive  = TRUNK_DISABLE;
    tport->linkstatus = IFNET_LINKDOWN;

    return tport;
}

/*******************************************************************************
 功能描述:主端口发生down事件，发生切换
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
int trunk_backup_switch( struct trunk *ptrunk, enum BACKUP_E status )
{
    int ret = 0;
    
	TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    if (NULL == ptrunk)
    {
        zlog_err ( "%s[%d] struct trunk null\n", __FUNCTION__, __LINE__);
        return ERRNO_FAIL;  
    }

    switch(status)
    {
        /*主工作*/
        case BACKUP_STATUS_MASTER:
        {
            ptrunk->status = BACKUP_STATUS_MASTER;
            //ret = ipc_send_hal_wait_ack( ptrunk, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_BACKUP_PORT_SELECT, IPC_OPCODE_UPDATE, 0 );
				ret = l2_msg_send_hal_wait_ack( ptrunk, sizeof(struct trunk), 1, MODULE_ID_HAL, 
							MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_BACKUP_PORT_SELECT, IPC_OPCODE_UPDATE, 0 );
            if(ret != 0)
            {
                zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
                return ERRNO_FAIL;
            } 
        }
        break;
        /*备工作*/
        case BACKUP_STATUS_SLAVE:
        {
            ptrunk->status = BACKUP_STATUS_SLAVE;
            //ret = ipc_send_hal_wait_ack( ptrunk, sizeof(struct trunk), 1, MODULE_ID_HAL, MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_BACKUP_PORT_SELECT, IPC_OPCODE_UPDATE, 0 );
				ret = l2_msg_send_hal_wait_ack( ptrunk, sizeof(struct trunk), 1, MODULE_ID_HAL, 
							MODULE_ID_L2, IPC_TYPE_TRUNK, TRUNK_INFO_BACKUP_PORT_SELECT, IPC_OPCODE_UPDATE, 0 );
            if(ret != 0)
            {
                zlog_err ( "%s[%d] ipc_send_hal_wait_ack errocode=%d\n", __FUNCTION__, __LINE__, ret );
                return ERRNO_FAIL;
            } 
        }
        break;
        default:
            break;
    }

    TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.(master_if:0x%x,status:%u)\n", __FILE__, __LINE__, __func__, ptrunk->master_if, ptrunk->status);
    return ERRNO_SUCCESS;
}
/*******************************************************************************
 功能描述:trunk成员端口上报/清除告警
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
void trunk_member_alarm_report(uint16_t trunkid, uint32_t ifindex)
{
	struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
	struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
    TRUNK_LOG_DBG("%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);
    
    /*判断trunk id是否有效*/
    if((trunkid > IFM_TRUNK_MAX) || (trunkid < 1) )
    {
        zlog_err("%s[%d]:leave %s:trunk id invalid\n",__FILE__,__LINE__,__func__);
        return;
    }
    
    /*查找是否存在trunk id*/
    trunk_entry = trunk_lookup(trunkid);
    if (NULL == trunk_entry)
    {
        return;
    }

    if (TRUNK_ENABLE == trunk_entry->lacp_enable)
	{
		tport = trunk_port_lookup(trunkid, ifindex);
        if (NULL == tport)
		{
			return;
		}
		
        if (tport->link.sync_state)
        {
            /*清除告警*/
			/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(ifindex),IFM_PORT_ID_GET(ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);*/

			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = ifindex;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
            TRUNK_LOG_DBG("%s[%d]: clear alarm(ifindex=0x%x).\n", __FILE__, __LINE__, ifindex);
        }
        else
        {
            /*上报告警*/
			/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(ifindex),IFM_PORT_ID_GET(ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);*/

			gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
			gPortInfo.iIfindex = ifindex;
			ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);
            TRUNK_LOG_DBG("%s[%d]: report alarm(ifindex=0x%x).\n", __FILE__, __LINE__, ifindex);
        }
	}
	else
    {
        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            tport = trunk_port_lookup(trunkid, ifindex);

            if (NULL == tport)
            {
                return;
            }
			
            if (IFNET_LINKUP == tport->linkstatus)
			{
                /*清除告警*/
				/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(ifindex),IFM_PORT_ID_GET(ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);*/

				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = ifindex;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
                TRUNK_LOG_DBG("%s[%d]: clear alarm(ifindex=0x%x).\n", __FILE__, __LINE__, ifindex);

			}
			else
            {
                /*上报告警*/
				/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(ifindex),IFM_PORT_ID_GET(ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);*/

				gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
				gPortInfo.iIfindex = ifindex;
				ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);
                TRUNK_LOG_DBG("%s[%d]: report alarm(ifindex=0x%x).\n", __FILE__, __LINE__, ifindex);
            }
		}
		else
		{
            if (BACKUP_STATUS_MASTER == trunk_entry->status)
			{
				for(ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
				{
					if(tport_tmp->ifindex == trunk_entry->master_if)
					{
                        /*清除告警*/
						/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(tport_tmp->ifindex),IFM_PORT_ID_GET(tport_tmp->ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);*/

						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = tport_tmp->ifindex;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
                        TRUNK_LOG_DBG("%s[%d]: clear alarm(ifindex=0x%x).\n", __FILE__, __LINE__, tport_tmp->ifindex);

					}
					else
                    {
                        /*上报告警*/
						/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(tport_tmp->ifindex),IFM_PORT_ID_GET(tport_tmp->ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);*/

						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = tport_tmp->ifindex;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);
                        TRUNK_LOG_DBG("%s[%d]: report alarm(ifindex=0x%x).\n", __FILE__, __LINE__, tport_tmp->ifindex);
                    }
				}
			}
            else if (BACKUP_STATUS_SLAVE == trunk_entry->status)
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
                {
                    if (tport_tmp->ifindex != trunk_entry->master_if)
                    {
                        /*清除告警*/
						/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(tport_tmp->ifindex),IFM_PORT_ID_GET(tport_tmp->ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);*/

						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = tport_tmp->ifindex;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_CLEAN);
                        TRUNK_LOG_DBG("%s[%d]: clear alarm(ifindex=0x%x).\n", __FILE__, __LINE__, tport_tmp->ifindex);

                    }
                    else
                    {
                        /*上报告警*/
						/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(tport_tmp->ifindex),IFM_PORT_ID_GET(tport_tmp->ifindex),
							0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);*/

						gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
						gPortInfo.iIfindex = tport_tmp->ifindex;
						ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);
                        TRUNK_LOG_DBG("%s[%d]: report alarm(ifindex=0x%x).\n", __FILE__, __LINE__, tport_tmp->ifindex);
                    }
				}
			}
			else
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
                {
                    /*上报告警*/
					/*ipran_alarm_report(IFM_TRUNK_TYPE, IFM_SLOT_ID_GET(tport_tmp->ifindex),IFM_PORT_ID_GET(tport_tmp->ifindex),
									0, 0, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);*/

					gPortInfo.iAlarmPort = IFM_FUN_ETH_TYPE;
					gPortInfo.iIfindex = tport_tmp->ifindex;
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_FUN_ETH_LAG_FAIL, GPN_SOCK_MSG_OPT_RISE);

                    TRUNK_LOG_DBG("%s[%d]: report alarm(ifindex=0x%x).\n", __FILE__, __LINE__, tport_tmp->ifindex);
                }
			}
		}
	}
	
	TRUNK_LOG_DBG("%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);

	return;
}

static int trunk_gloable_configuration_init(void)
{
	trunk_gloable_config.gloable_balance_mode = TRUNK_BALANCE_SMAC;
	return 0;
}
/*******************************************************************************
 功能描述:初始化
 输入参数:
 输出参数:
 返回值域:
--------------------------------------------------------------------------------
 修改作者:
 修改日期: 
*******************************************************************************/
void trunk_init()
{
    trunk_table_init();	
    trunk_cmd_init();
    /*注册收包处理*/
    lacp_pkt_rx_register();
	/*初始化调试开关*/
	lacp_pkt_debug_init();
	
	//thread_add_timer (l2_master, lacp_timer_start, NULL, 1);
    high_pre_timer_add("LacpTimer", LIB_TIMER_TYPE_LOOP, lacp_timer_start, NULL, 1000);
    trunk_gloable_configuration_init();
	return ;
}

/* 查找传入 trunkid 之后对应的 n 个数据 */
int trunk_info_bulk_get(uint16_t trunkid, struct trunk pif[])
{
    int data_num = 0;
    int msg_num  = IPC_MSG_LEN/sizeof(struct trunk);
	struct trunk *trunk_entry = NULL;
 	int i = 0;

    if (0 == trunkid)   /* 初次获取 */
    {
        for(i = 1; i <= IFM_TRUNK_MAX; i++)
	    {
	    
            trunk_entry = trunk_lookup(i);

            if (NULL == trunk_entry)
            {
                continue;
            }

            memcpy(&pif[data_num++], trunk_entry, sizeof(struct trunk));

            if (data_num == msg_num)
            {
                return data_num;
            }
	    }
    }
    else /* 找到 trunkid 后面第一个数据 */
    {
        for (i = trunkid + 1; i <= IFM_TRUNK_MAX; i++)
        {
            trunk_entry = trunk_lookup(i);

            if (NULL == trunk_entry)
            {
                continue;
            }

			memcpy(&pif[data_num++], trunk_entry, sizeof(struct trunk));
			if (data_num == msg_num)
			{
				return data_num;
			}
		}	
    }

    return data_num;
}


/*查找传入 trunk中所有成员端口 */
int trunk_port_info_bulk_get( uint16_t trunkid, struct trunk_port tport[])
{
    struct trunk *trunk_entry = NULL;
	struct trunk_port *tport_tmp = NULL;
    struct listnode *node_tmp = NULL;
    int data_num = 0;

    trunk_entry = trunk_lookup(trunkid);
    if (NULL == trunk_entry)
    {
        return data_num;
    }

	for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node_tmp, tport_tmp))
	{
		if (tport_tmp)
		{
			memcpy(&tport[data_num++], tport_tmp, sizeof(struct trunk_port));
		}
	}

    return data_num;
}

int trunk_speed_set(uint16_t trunkid)
{
    struct trunk *trunk_entry = NULL;
    struct trunk_port *tport = NULL;
    struct listnode *node = NULL;
    struct ifm_port ifm_port;
    uint32_t speed = 0;
	int ret = 0;

    trunk_entry = trunk_lookup(trunkid);

    if (NULL == trunk_entry)
    {
        return -1;
    }

    if (TRUNK_DISABLE == trunk_entry->lacp_enable)
    {
        /*负载分担时，所有up端口速率之和*/
        if (TRUNK_MODE_ECMP == trunk_entry->work_mode)
        {
            for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
            {
                if (IFNET_LINKUP == tport->linkstatus)
                {
                    ret = ifm_get_port_info(tport->ifindex, MODULE_ID_L2, &ifm_port);

                    if (ret)
                    {
                        continue;
                    }

                    if (IFNET_SPEED_GE == ifm_port.speed || IFNET_SPEED_INVALID  == ifm_port.speed)
                    {
                        speed += 1000;
                    }
                    else if (IFNET_SPEED_FE == ifm_port.speed)
                    {
                        speed += 100;

                    }
                    else if (IFNET_SPEED_10GE == ifm_port.speed)
                    {
                        speed += 10000;

                    }
                    else if (IFNET_SPEED_10M == ifm_port.speed)
                    {
                        speed += 10;
                    }
                }
            }
        }
        /*主备时，工作端口速率*/
        else
        {
            if (BACKUP_STATUS_MASTER == trunk_entry->status)
            {
                ret = ifm_get_port_info(trunk_entry->master_if, MODULE_ID_L2, &ifm_port);

                if (!ret)
                {
                    if (IFNET_SPEED_GE == ifm_port.speed || IFNET_SPEED_INVALID  == ifm_port.speed)
                    {
                        speed += 1000;
                    }
                    else if (IFNET_SPEED_FE == ifm_port.speed)
                    {
                        speed += 100;

                    }
                    else if (IFNET_SPEED_10GE == ifm_port.speed)
                    {
                        speed += 10000;

                    }
                    else if (IFNET_SPEED_10M == ifm_port.speed)
                    {
                        speed += 10;
                    }
                }
            }
            else if (BACKUP_STATUS_SLAVE == trunk_entry->status)
            {
                for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
                {
                    if (tport->ifindex != trunk_entry->master_if)
                    {
                        ret = ifm_get_port_info(tport->ifindex, MODULE_ID_L2, &ifm_port);

                        if (ret)
                        {
                            continue;
                        }

                        if (IFNET_SPEED_GE == ifm_port.speed || IFNET_SPEED_INVALID  == ifm_port.speed)
                        {
                            speed += 1000;
                        }
                        else if (IFNET_SPEED_FE == ifm_port.speed)
                        {
                            speed += 100;

                        }
                        else if (IFNET_SPEED_10GE == ifm_port.speed)
                        {
                            speed += 10000;

                        }
                        else if (IFNET_SPEED_10M == ifm_port.speed)
                        {
                            speed += 10;
                        }
                    }
                }
            }
        }
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO(&(trunk_entry->portlist), node, tport))
        {
            if (tport->link.sync_state)
            {
                ret = ifm_get_port_info(tport->ifindex, MODULE_ID_L2, &ifm_port);

                if (ret)
                {
                    continue;
                }

                if (IFNET_SPEED_GE == ifm_port.speed || IFNET_SPEED_INVALID  == ifm_port.speed)
                {
                    speed += 1000;
                }
                else if (IFNET_SPEED_FE == ifm_port.speed)
                {
                    speed += 100;

                }
                else if (IFNET_SPEED_10GE == ifm_port.speed)
                {
                    speed += 10000;

                }
                else if (IFNET_SPEED_10M == ifm_port.speed)
                {
                    speed += 10;
                }
            }
        }
    }

    if (speed != trunk_entry->speed)
    {
        trunk_entry->speed = speed;
        /*发送至ifm*/

    }

    return speed;
}
