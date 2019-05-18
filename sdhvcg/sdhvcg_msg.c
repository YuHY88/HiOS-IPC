/*
*   vcg message receive and send
*
*/
#include <string.h>
#include <unistd.h>
#include "lib/msg_ipc_n.h"
#include <lib/zassert.h>
#include <lib/thread.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/memshare.h>
#include <ifm/ifm.h>
#include <sdhvcg/sdhvcg_if.h>
#include <sdhvcg/sdhvcg_main.h>
#include <sdhvcg/sdhvcg_msg.h>

/* receive IPC message */
int vcg_msg_rcv_msg(struct ipc_mesg_n *pmesg, int imlen)
{
    uint32_t ifindex;
    int ret = 0;
  
	int revln = 0;
	
    ifindex = pmesg->msghdr.msg_index;
    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
       if(IPC_TYPE_VCG == pmesg->msghdr.msg_type)
       {
            if (pmesg->msghdr.opcode == IPC_OPCODE_UPDATE)
            {
                if (pmesg->msghdr.msg_subtype == VCG_INFO_IF_ADD)
                {
                    vcg_if_create(pmesg->msg_data, ifindex);
                }
                else if (pmesg->msghdr.msg_subtype == VCG_INFO_IF_DELETE)
                {
                    vcg_if_delete(ifindex);
                }
                else if (pmesg->msghdr.msg_subtype == VCG_STATUS_IF_UPDATE)
                {
                    vcg_if_status_update(pmesg->msg_data, pmesg->msghdr.data_len, pmesg->msghdr.data_num);
                }
            }
            else if (pmesg->msghdr.opcode == IPC_OPCODE_GET_BULK)
            {
                if (pmesg->msghdr.msg_subtype == VCG_GET_CONFIG)
                {
                    vcg_get_config_bulk(pmesg->msg_data, &(pmesg->msghdr));
                }
            } 
	   }

    }

	mem_share_free(pmesg, MODULE_ID_VCG);
#if 0	
    while (token)
    {
        ret = ipc_recv_common(pmesg, MODULE_ID_VCG);

        if (ret == -1)
        {
            return ERRNO_FAIL;
        }

        token--;

        /* process the ipc message */
        phdr = &(pmesg->msghdr);
        pdata = pmesg->msg_data;
        ifindex = pmesg->msghdr.msg_index;

        if (phdr->msg_type == IPC_TYPE_VCG)
        {
            if (phdr->opcode == IPC_OPCODE_UPDATE)
            {
                if (phdr->msg_subtype == VCG_INFO_IF_ADD)
                {
                    vcg_if_create(pmesg->msg_data, ifindex);
                }
                else if (phdr->msg_subtype == VCG_INFO_IF_DELETE)
                {
                    vcg_if_delete(ifindex);
                }
                else if (phdr->msg_subtype == VCG_STATUS_IF_UPDATE)
                {
                    vcg_if_status_update(pmesg->msg_data, phdr->data_len, phdr->data_num);
                }
            }
            else if (phdr->opcode == IPC_OPCODE_GET_BULK)
            {
                if (phdr->msg_subtype == VCG_GET_CONFIG)
                {
                    ret = vcg_get_config_bulk(pmesg->msg_data, phdr->data_len, phdr->module_id, phdr->sender_id, phdr->msg_type, phdr->msg_subtype, phdr->msg_index);
                }
            }
        }
    }

#endif

    return ret;
}

/* deal message */
int vcg_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    time_t    curr;
	int retva = 0;
    //static struct ipc_mesg_n pmesg;

    vcg_msg_rcv_msg(pmsg, imlen);

    if (0 == g_vcg_config_finish)
    {
        time(&curr);

        if ((uint32_t)curr - g_vcg_config_timer > 300)
        {
            g_vcg_config_finish = 1;
        }
    }

    usleep(10000);
    //thread_add_event(vcg_master, vcg_msg_rcv, NULL, 0);
 
    return retva;
}


/* ***********************************************************
 *  Function : vcg_msg_send_hal_wait_ack
 *  return   : 0 ok, -1 error
 *  description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
 * ***********************************************************/
int vcg_msg_send_hal_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
                              enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret = 0;
    int rcvlen = 0;
    struct ipc_mesg_n *pSndMsg = NULL;
    struct ipc_mesg_n * pRcvMsg = NULL;

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_VCG);
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

        /*send info*/
        ret = ipc_sync_send_n1(pSndMsg, sizeof(struct ipc_mesg_n) + data_len, &pRcvMsg, &rcvlen, 5000);

        switch(ret)
        {
            /*send fail*/
            case -1:
            {
                mem_share_free(pSndMsg, MODULE_ID_VCG);
                return -1;
            }
            case -2:
            {
                /*recv fail*/
                return -1;

            }
            case 0:
            {
                /*recv success*/
                if(pRcvMsg != NULL)
                {
                    /*if noack return errcode*/
                    if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
                    {
                        memcpy(&ret, pRcvMsg->msg_data, sizeof(ret));

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

                    mem_share_free(pRcvMsg, MODULE_ID_VCG);
                    return ret;
                }
                else
                {
                    return -1;
                }
            }
            default:
                return -1;

        }

    }
    else
    {
        return -1;
    }
    return ret;
}


