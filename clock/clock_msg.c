/*
*   clock message receive and send
*
*/
#include <unistd.h>
#include <lib/msg_ipc_n.h>
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include "lib/pkt_buffer.h"
#include <ftm/pkt_eth.h>
#include <ifm/ifm.h>
#include <clock/clock_main.h>
#include <clock/clock_msg.h>
#include <clock/clock_src.h>
#include <clock/clock_if.h>

#include "clock_cmd.h"
#include "synce/synce_ssm.h"
#include "synce/synce.h"




static int clock_msg_rcv_pkt_n(struct ipc_mesg_n *pmesg)
{
	struct pkt_buffer	*pkt = NULL;	
	struct eth_control  *ethcb = NULL;
	
	pkt = (struct pkt_buffer *)pmesg->msg_data;
	if(pkt == NULL)
	{
		return ERRNO_FAIL;
						
	}
	ethcb = (struct eth_control *)(&pkt->cb);


    /* 通过 ehtype 识别报文 */
    if (pkt->protocol == ETH_P_SYNCE) /*慢协议synce协议报文*/
    {
        if (ethcb->sub_ethtype == ETH_SUBTYPE_SYNCE)
        {
            synce_ssm_recv(pkt);
        }
    }
    else if (pkt->protocol == ETH_P_PTP) /*1588协议报文*/
    {

    }
    else
    {
        zlog_debug(CLOCK_DBG_ERROR,"%s[%d]: %s unknown protocol : 0x%x\n", __FILE__, __LINE__, __func__, pkt->protocol);
    }

    return ERRNO_SUCCESS;

}


/* 接收 IPC 消息 */
int clock_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen)
{
    //int msg_num = IPC_MSG_LEN / sizeof(struct ces_if);
    //int msg_num = 63;
    int ret = 0;
    int revln = 0;
    uint32_t ifindex = 0;
    struct ipc_msghdr_n *phdr = NULL;
    struct ifm_event *pevent = NULL;
    void *pdata = NULL;

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
    if(revln <= imlen)
    {
        /* 消息处理 */
        phdr = &(pmesg->msghdr);
        pdata = pmesg->msg_data;

        if (IPC_TYPE_IFM == pmesg->msghdr.msg_type)
        {
            if (IPC_OPCODE_EVENT == pmesg->msghdr.opcode)
            {
                pevent = (struct ifm_event *)pdata;
                ifindex = pevent->ifindex;

                if (IFNET_EVENT_DOWN == phdr->msg_subtype)
                {
                    zlog_debug(CLOCK_DBG_INFO,"%s[%d] recv event notify ifindex 0x%0x down\n", __FUNCTION__, __LINE__, ifindex);
                    clock_src_down(ifindex);
                }
                else if (IFNET_EVENT_UP == phdr->msg_subtype)
                {
                    zlog_debug(CLOCK_DBG_INFO,"%s[%d] recv event notify ifindex 0x%0x up\n", __FUNCTION__, __LINE__, ifindex);
                    clock_src_up(ifindex);
                }
            }
        }
        else if (IPC_TYPE_SYNCE == phdr->msg_type)
        {
            
            switch (phdr->msg_subtype)
            {
                case IPC_TYPE_SNMP_SYNCE_GLOBAL_INFO:
                    synce_snmp_global_cfg_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYNCE_CLOCK_IF_INFO:
                    synce_snmp_clk_if_info_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYNCE_CLOCK_IF_NEXT_IFINDEX:
                    synce_snmp_clk_if_next_ifindex_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_INFO:
                    synce_snmp_clk_src_info_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYNCE_CLOCK_SRC_TABLE_NEXT_IFINDEX:
                    synce_snmp_clk_src_next_ifindex_get(phdr);
                    break;

                default :
                    break;
            }
        }
        else if(phdr->msg_type == IPC_TYPE_PACKET)
        {
            clock_msg_rcv_pkt_n(pmesg);
        }
    }
    else
    {
         //FILEM_DEBUG("filem recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
         printf("%s[%d], datalen error, data_len=%d, msgrcv len = %d\n",
                        __FUNCTION__, __LINE__, revln, imlen);
    }

    mem_share_free(pmesg, MODULE_ID_CLOCK);

    return ret;
}




