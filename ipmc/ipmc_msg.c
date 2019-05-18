/*
*   mpls message receive and send
*
*/
#include <string.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/log.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/route_com.h>
#include <lib/pkt_type.h>
#include <lib/oam_common.h>
#include <ftm/pkt_udp.h>
#include <ftm/pkt_tcp.h>
#include <ftm/pkt_mpls.h>
#include <ftm/pkt_ip.h>

#include "ipmc_main.h"
#include "ipmc_if.h"
#include "pim/pim_pkt.h"
#include "pim/pim_register.h"
#include "igmp/igmp_pkt.h"
#include "memshare.h"

/* ***********************************************************
*  Function : ipmc_msg_send_hal_wait_reply
*  return   : 0 ok, -1 error
*  description: send get msg to hal and wait reply data, used to replace "ipc_send_hal_wait_reply"
* ***********************************************************/
int ipmc_msg_send_hal_wait_reply(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int    iRetv = 0;
    int    iRepL = 0;

    struct ipc_mesg_n *pMsgSnd = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;
    pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_IPMC);
    if(pMsgSnd == NULL) 
    {
        return -1;
    }
    memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));
    pMsgSnd->msghdr.data_len    = data_len;
    pMsgSnd->msghdr.data_num    = data_num;
    pMsgSnd->msghdr.module_id   = module_id;
    pMsgSnd->msghdr.sender_id   = sender_id;
    pMsgSnd->msghdr.msg_type    = msg_type;
    pMsgSnd->msghdr.msg_subtype = msg_subtype;
    pMsgSnd->msghdr.opcode      = opcode;
    pMsgSnd->msghdr.msg_index   = msg_index;
    iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);
    switch(iRetv)
    {
        case -1:   //send msg fail
            mem_share_free(pMsgSnd, MODULE_ID_IPMC);
            return -1;

        case -2:   //send msg success, receive msg fail
            return -1;

        case  0:
            if(pMsgRcv == NULL) 
            {
                iRetv = -1;
            }
            else 
            {
                if(pMsgRcv->msghdr.data_len >= (unsigned int)data_len)
                {
                    iRetv = 0;
                    memcpy(pdata, pMsgRcv->msg_data, data_len);
                }
                else 
                {
                    iRetv = -1;
                }

                mem_share_free(pMsgRcv, MODULE_ID_IPMC);
            }
            return iRetv;
             
        default: 
            return -1;
    }
}
/* ***********************************************************
*  Function : ipmc_msg_send_hal_wait_ack
*  return   : 0 ok, -1 error
*  description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
* ***********************************************************/
int ipmc_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret =0;
	int    iRepL = 0;
    struct ipc_mesg_n *pSndMsg =  NULL;
    struct ipc_mesg_n *pRcvMsg = NULL;

if(NULL== pdata)
{
	data_len = 0;
}

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_IPMC);

    if(pSndMsg != NULL)
    {
        pSndMsg->msghdr.data_len    = data_len;
        pSndMsg->msghdr.module_id   = module_id;
        pSndMsg->msghdr.sender_id   = sender_id;
        pSndMsg->msghdr.msg_type    = msg_type;
        pSndMsg->msghdr.msg_subtype = msg_subtype;
        pSndMsg->msghdr.msg_index   = msg_index;
        pSndMsg->msghdr.data_num    = data_num;
        pSndMsg->msghdr.opcode      = opcode;

        if(pdata)
        {
            memcpy(pSndMsg->msg_data, pdata, data_len);
        }

    
	ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len, &pRcvMsg, &iRepL, 2000);

    if (ret == -1)
    {
        ipc_msg_free(pSndMsg,MODULE_ID_IPMC);
	 zlog_debug("%s[%d]:send message error\n", __FILE__, __LINE__);
        return ret;
    }
    else if(ret == -2)
    {
         zlog_debug("%s[%d]:receive message error\n", __FILE__, __LINE__);
         return ret;
    }
    else  
    {
       if(pRcvMsg != NULL)
	{	
		/*if noack return errcode*/
		if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
		{
			memcpy(ret,pRcvMsg->msg_data,sizeof(ret));
					       
		}
		/*if ack return 0*/
		else if(IPC_OPCODE_ACK == pRcvMsg->msghdr.opcode)
		{
			ret = 0;
		}
		else
		{
			ret = -1;
		}
		//ret = (int)pRcvMsg->msg_data;
		ipc_msg_free(pRcvMsg,MODULE_ID_L2);
	}
	else
	{
		return -1;
	}
     
    }
    }
     
    return ret;
}

/* ***********************************************************
*  Function : ipmc_msg_send_to_hal
*  return   : 0 ok, -1 error
*  description: send set msg to hal and wait ack, used to replace "ipc_send_hal"
* ***********************************************************/
int ipmc_msg_send_to_hal(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                                               enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret =0;
    struct ipc_mesg_n *pSndMsg =  NULL;

if(NULL== pdata)
{
	//data_len = 0;
	return -1;
}

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_IPMC);

    if(pSndMsg != NULL)
    {
        pSndMsg->msghdr.data_len    = data_len;
        pSndMsg->msghdr.module_id   = module_id;
        pSndMsg->msghdr.sender_id   = sender_id;
        pSndMsg->msghdr.msg_type    = msg_type;
        pSndMsg->msghdr.msg_subtype = msg_subtype;
        pSndMsg->msghdr.msg_index   = msg_index;
        pSndMsg->msghdr.data_num    = data_num;
        pSndMsg->msghdr.opcode      = opcode;

        if(pdata)
        {
            memcpy(pSndMsg->msg_data, pdata, data_len);
        }

        ret = ipc_send_msg_n1(pSndMsg, sizeof(struct ipc_msghdr_n) + data_len);
        if(ret != 0) 
        {
            mem_share_free(pSndMsg, MODULE_ID_IPMC);
        }
    }
     
    return(0);
}
/* 从路由协议模块接收路由消息*/
static int ipmc_msg_rcv_route(struct ipc_mesg_n *pmesg)
{
    struct ipc_msghdr_n  *phdr   = NULL;
    struct route_entry *proute = NULL;
    int i;
#if 0
    if (ipc_route_id < 0)
    {
        ipc_route_id = ipc_connect(IPC_MSG_ROUTE);
    }

    if (ipc_route_id < 0)
    {
        IPMC_LOG_ERROR("ipc connect failed!\n");
        return ERRNO_IPC;
    }

    if (-1 == ipc_recv(ipc_route_id, pmesg, MODULE_ID_IPMC))
    {
        return ERRNO_FAIL;
    }
#endif
    phdr = &(pmesg->msghdr);

    IPMC_LOG_DEBUG("sender_id:%d, msg_type:%d, opcode:%d, subtype:%d.\n",
                    phdr->sender_id, phdr->msg_type, phdr->opcode, phdr->msg_subtype);
	proute = (struct route_entry *)pmesg->msg_data;

    for (i=0; i<phdr->data_num; i++)
    {
        if (32 != proute->prefix.prefixlen)
        {
            proute += 1;
            continue;
        }

        if (phdr->opcode == IPC_OPCODE_ADD)
        {
            //lsp_fec_add_route(proute);
        }
        else if (phdr->opcode == IPC_OPCODE_DELETE)
        {
            //lsp_fec_delete_route(proute);
        }

        proute += 1;
    }

    return ERRNO_SUCCESS;
}


/* 从 ifm 接收接口事件 */

static void ipmc_msg_rcv_ifevent(struct ipc_msghdr_n *pmsghdr, void *pdata)
{
    struct ifm_event *pevent = NULL;
    uint32_t ifindex = 0;

	if (pmsghdr->opcode == IPC_OPCODE_EVENT)
	{
		pevent = (struct ifm_event *)pdata;
		ifindex = pevent->ifindex;
		if (pmsghdr->msg_subtype == IFNET_EVENT_DOWN)
		{
			IPMC_LOG_DEBUG("recv event notify ifindex 0x%0x down\n", ifindex);
            ipmc_if_down(ifindex);
		}
		else if (pmsghdr->msg_subtype == IFNET_EVENT_UP)
		{
			IPMC_LOG_DEBUG("recv event notify ifindex 0x%0x up\n", ifindex);
            ipmc_if_up(ifindex);
		}
		else if (pmsghdr->msg_subtype == IFNET_EVENT_IF_DELETE)
		{
			IPMC_LOG_DEBUG("recv event notify ifindex 0x%0x delete\n", ifindex);
            ipmc_if_delete(ifindex);
		}
		else if (pmsghdr->msg_subtype == IFNET_EVENT_MODE_CHANGE)
		{
			IPMC_LOG_DEBUG("recv event notify ifindex 0x%0x change mode %d\n", ifindex, pevent->mode);
            ipmc_if_mode_change(ifindex, pevent->mode);
		}
	}

	return;
}


/* 处理 IPC 消息 */
//static int ipmc_msg_rcv_msg(struct ipc_mesg *pmesg)
static int ipmc_msg_rcv_msg(struct ipc_mesg_n *pmsg, int imlen)
{
    void *pdata = NULL;

    /* process the ipc message */
    pdata = pmsg->msg_data;

    IPMC_LOG_DEBUG( "msg_type = %d, opcode = %d, subtype = %d\n",pmsg->msghdr.msg_type,
                   pmsg->msghdr.opcode,pmsg->msghdr.msg_subtype );

    if (pmsg->msghdr.msg_type == IPC_TYPE_IFM) /* 接口管理消息 */
	{
		ipmc_msg_rcv_ifevent(&(pmsg->msghdr), pdata);
	}
	else if (pmsg->msghdr.msg_type == IPC_TYPE_ROUTE) /* route消息 */
	{
		ipmc_msg_rcv_route(pmsg);
	}

    return 0;
}


/* 接收报文 */
static int  ipmc_msg_rcv_pkt(struct ipc_mesg_n *pmsg)
{
    struct pkt_buffer *ppkt = NULL;
    struct ip_control *ipcb;
	
	ppkt = (struct pkt_buffer *)pmsg->msg_data;
	if(ppkt == NULL)
		return ERRNO_FAIL;

	zlog_notice("%s[%d]: ipmc_msg_rcv_pkt ppkt->cb_type=%d",__FILE__, __LINE__,ppkt->cb_type);
    /* 根据控制信息类型解析报文 */
	switch (ppkt->cb_type)
	{
		case PKT_TYPE_IP:
		{	
			ipcb = (struct ip_control *)&(ppkt->cb);
			if(ipcb->protocol == IP_P_PIM)
			{
				/*接收到的是pimhdr + pim payload*/
				zlog_notice("%s[%d]: pim_recv\n", __FILE__, __LINE__);
				pim_pkt_recv(ppkt);
			}
			/*单播报文*/

			if(ipcb->protocol == IP_P_IGMP)
			{
				zlog_notice("%s[%d]: igmp pkt resc\n", __FILE__, __LINE__);
				igmp_pkt_recv(ppkt);
			}

			/*pim data msg recv,在ip_rcv_local中发送时用ip接收*/
			if (ipcb->protocol == IP_P_UDP)
			{
				zlog_notice("%s[%d]: pim data pkt recv\n", __FILE__, __LINE__);
				pim_data_pkt_recv(ppkt);
			}
			break;
        }
		/*pim data msg recv from udp , udp payload*/
		case PKT_TYPE_UDP:
			pim_data_pkt_recv(ppkt);
			break;
		case PKT_TYPE_IPMC:
		{
			/*组播报文*/
				
			break;
		}
        default:
        	break;
    }
	IPMC_LOG_DEBUG("%s[%d]: leaving function '%s'.\n", __FILE__, __LINE__, __func__);
    return ERRNO_SUCCESS;
}

int ipmc_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    int revln = 0;

    IPMC_LOG_DEBUG("ipmc recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

	if ( NULL == pmsg || 0 == imlen )
	{
		IPMC_LOG_DEBUG("ipmc recv msg: pmsg is NULL.\r\n");
		return ERRNO_FAIL;
	}

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 
	
	if ( revln <= imlen)
	{
		switch ( pmsg->msghdr.msg_type )
		{
			case IPC_TYPE_PACKET:
				ipmc_msg_rcv_pkt(pmsg);
				break;

			case IPC_TYPE_IFM:
				ipmc_msg_rcv_ifevent(&(pmsg->msghdr), (void*)pmsg->msg_data);
				break;

			case IPC_TYPE_ROUTE:
				ipmc_msg_rcv_route(pmsg);
				break;
			
			default:
				IPMC_LOG_DEBUG("ipmc_msg_rcv, receive unk message\r\n");
				break;
		}
		
	}
	else
	{
		IPMC_LOG_DEBUG("ipmc recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
	}

	mem_share_free(pmsg, MODULE_ID_IPMC);

    return ERRNO_SUCCESS;
}


