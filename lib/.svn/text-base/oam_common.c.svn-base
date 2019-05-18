#include <string.h>
#include "oam_common.h"
#include <lib/msg_ipc.h>


/* ***********************************************************
 *	Function : mpls_msg_send_hal_wait_ack
 *	return	 : 0 ok, 1 error
 *	description: send set msg to hal and wait ack, used to replace "ipc_send_hal_wait_ack"
 * ***********************************************************/
/*send info confirm ack or noack*/
int msg_sender_to_other_wait_ack(void *pdata, uint32_t data_len, uint16_t data_num, int module_id, int sender_id,
		enum IPC_TYPE msg_type, uint16_t msg_subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	struct ipc_mesg_n *pSndMsg = NULL;
	struct ipc_mesg_n * pRcvMsg = NULL;
	int rcvlen = 0;
	int ret = 0;
	
	pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, sender_id);
	if(pSndMsg != NULL)
	{
		pSndMsg->msghdr.data_len	= data_len;
		pSndMsg->msghdr.module_id	= module_id;
		pSndMsg->msghdr.sender_id	= sender_id;
		pSndMsg->msghdr.msg_type	= msg_type;
		pSndMsg->msghdr.msg_subtype = msg_subtype;
		pSndMsg->msghdr.msg_index	= msg_index;
		pSndMsg->msghdr.data_num	= data_num;
		pSndMsg->msghdr.opcode		= opcode;
			
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
				ipc_msg_free(pSndMsg,sender_id);
				return -1;
			case -2:
				/*recv fail*/
				return -1;
			case 0:
				/*recv success*/
				if(pRcvMsg != NULL)
				{	
					/*if noack return errcode*/
					if(IPC_OPCODE_NACK == pRcvMsg->msghdr.opcode)
					{
						memcpy(&ret,pRcvMsg->msg_data,sizeof(ret));
									   
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
					ipc_msg_free(pRcvMsg,sender_id);
				}
				else
				{
					return -1;
				}
				
				break;
			default:
				return -1;
		}
		
	}
	else
	{
		return -1;
	}

	return(ret);
}	   

