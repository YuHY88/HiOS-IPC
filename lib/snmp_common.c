/******************************************************************************
 * Filename: snmp_common.c
 *	Copyright (c) 2018-2018 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: snmp common functions for other programs.
 *
 * History:
 * 2018.6.6  lipf created
 *
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "syslog.h"

#include "snmp_common.h"
#include "memtypes.h"
#include "memshare.h"
#include "msg_ipc.h"
#include "msg_ipc_n.h"



/**
 * @brief      : 增加trap消息的消息头
 * @param[in ] : buf    - 用于存储trap信息的buf
 * @param[in ] : phdr 	- trap消息头
 * @param[out] : 
 * @return     : 成功返回1，否则返回0
 * @author     : Lipf
 * @date       : 2018年6月6日
 * @note       :
 */

int snmp_add_trap_msg_hdr(uchar *buf, struct snmp_trap_hdr *phdr)
{
	if((NULL == buf) || (NULL == phdr) || (NULL == phdr->trap_oid))
	{
		zlog_err("%s[%d] : snmp_add_trap_msg_hdr err\n", __func__, __LINE__);
		return 0;
	}
	
	phdr->len_msg = SNMP_TRAP_HDR_LEN + (phdr->len_trap_oid * sizeof(oid));
	phdr->cnt_data = 0;

	memcpy(buf, phdr, SNMP_TRAP_HDR_LEN);
	memcpy(buf + SNMP_TRAP_HDR_LEN, phdr->trap_oid, (phdr->len_trap_oid * sizeof(oid)));

	return 1;
}


/**
 * @brief      : 增加trap消息的一项数据
 * @param[in ] : buf    - 用于存储trap信息的buf
 * @param[in ] : pdata 	- trap的一项数据
 * @param[out] : 
 * @return     : 成功返回1，否则返回0
 * @author     : Lipf
 * @date       : 2018年6月6日
 * @note       :
 */

int snmp_add_trap_msg_data(uchar *buf, struct snmp_trap_data *pdata)
{
	if((NULL == buf) || (NULL == pdata->data_oid) || (NULL == pdata->data))
	{
		zlog_err("%s[%d] : snmp_add_trap_msg_data err\n", __func__, __LINE__);
		return 0;
	}	

	struct snmp_trap_hdr *phdr = (struct snmp_trap_hdr *)buf;
	
	memcpy(buf + phdr->len_msg, pdata, SNMP_TRAP_DATA_LEN);
	memcpy(buf + phdr->len_msg + SNMP_TRAP_DATA_LEN, pdata->data_oid, (pdata->len_data_oid * sizeof(oid)));
	memcpy(buf + phdr->len_msg + SNMP_TRAP_DATA_LEN + (pdata->len_data_oid * sizeof(oid)),
		pdata->data, pdata->len_data);
	
	phdr->cnt_data += 1;
	phdr->len_msg += (SNMP_TRAP_DATA_LEN + (pdata->len_data_oid * sizeof(oid)) + pdata->len_data);

	if(phdr->len_msg > IPC_MSG_LEN)
	{
		zlog_err("%s[%d] : data msg is too long, bigger than %d\n", __func__, __LINE__, IPC_MSG_LEN);
		return 0;
	}

	return 1;
}


/**
 * @brief      : 向snmp发送trap消息
 * @param[in ] : buf    	- 用于存储trap信息的buf
 * @param[in ] : sender_id 	- 发送trap消息进程的module_id
 * @param[out] : 
 * @return     : 成功返回1，否则返回0
 * @author     : Lipf
 * @date       : 2018年6月6日
 * @note       :
 */

int snmp_send_trap_msg(uchar *buf, int sender_id, uint8_t subtype, enum IPC_OPCODE opcode)
{
	/*struct snmp_trap_hdr *phdr = (struct snmp_trap_hdr *)buf;
	
	ipc_send_common((void *)buf, phdr->len_msg, 1, MODULE_ID_SNMPD, sender_id, IPC_TYPE_SNMP_TRAP, subtype, opcode);*/

	int  iRetVal = 0;
    int  iDatalen = 0;
    
    struct ipc_mesg_n *pSndMsg = NULL;

	struct snmp_trap_hdr *phdr = (struct snmp_trap_hdr *)buf;
	iDatalen = phdr->len_msg;

    pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + iDatalen, sender_id);
	if(pSndMsg == NULL) 
	{
		printf("%s[%d] : mem share malloc error\n", __func__, __LINE__);
		return 0;
    }
	
	memset(pSndMsg, 0, (sizeof(struct ipc_msghdr_n) + iDatalen));

    pSndMsg->msghdr.module_id   = MODULE_ID_SNMPD_TRAP;
    pSndMsg->msghdr.sender_id   = sender_id;
    pSndMsg->msghdr.msg_type    = IPC_TYPE_SNMP_TRAP;
    pSndMsg->msghdr.msg_subtype = subtype;
    pSndMsg->msghdr.opcode      = opcode;
    pSndMsg->msghdr.data_num    = 1;
    pSndMsg->msghdr.data_len    = (unsigned int)iDatalen;

	memcpy(pSndMsg->msg_data, buf, iDatalen);

    iRetVal = ipc_send_msg_n1(pSndMsg, (sizeof(struct ipc_msghdr_n) + iDatalen));

	/* 发送失败，需要本端释放共享内存 */
    if(-1 == iRetVal)	
    {
		mem_share_free(pSndMsg, sender_id);
    }

	return 1;
}


/**
 * @brief      : 向snmp发送应答数据，应答get、get-next
 * @param[in ] : phdr_rcv   - 接收到的消息头
 * @param[in ] : pdata 		- 待发送数据的指针
 * @param[in ] : data_len 	- 待发送数据的总长度
 * @param[in ] : data_num 	- 待发送数据的个数
 * @param[out] : 
 * @return     : 
 * @author     : Lipf
 * @date       : 2018年6月6日
 * @note       :
 */
void ipc_ack_to_snmp(struct ipc_msghdr_n *phdr_rcv, void *pdata, int data_len, int data_num)
{
	struct ipc_mesg_n *pMsgRep = NULL;

	if(NULL == pdata)
	{
		zlog_err("%s[%d] : error no data\n", __func__, __LINE__);
		return;
	}
	
	pMsgRep = mem_share_malloc(sizeof(struct ipc_msghdr_n) + data_len, MODULE_ID_SNMPD);
	if(pMsgRep == NULL)   //如果申请不到内存,对端只能等待超时
	{
		return;
	}

	memset(pMsgRep, 0, sizeof(struct ipc_msghdr_n) + data_len);
	
	pMsgRep->msghdr.data_len    = data_len;
    pMsgRep->msghdr.opcode      = IPC_OPCODE_REPLY;	//必填
    pMsgRep->msghdr.data_num    = data_num;
  
    memcpy(pMsgRep->msg_data, pdata, data_len);
    
    if(ipc_send_reply_n1(phdr_rcv, pMsgRep, sizeof(struct ipc_msghdr_n) + data_len) == -1)
    {
        mem_share_free(pMsgRep, MODULE_ID_SNMPD);
    }
}


/**
 * @brief      : 查询接收后，向snmp发送应答，告知snmp查询结束，以data_num=0来区分
 * @param[in ] : phdr_rcv   - 接收到的消息头
 * @param[in ] : pdata 		- 待发送数据的指针
 * @param[in ] : data_len 	- 待发送数据的总长度
 * @param[in ] : data_num 	- 待发送数据的个数
 * @param[out] : 
 * @return     : 
 * @author     : Lipf
 * @date       : 2018年6月6日
 * @note       :
 */
void ipc_noack_to_snmp(struct ipc_msghdr_n *phdr_rcv)
{
	struct ipc_mesg_n *pMsgRep = NULL;
	
	pMsgRep = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_SNMPD);
	if(pMsgRep == NULL)   //如果申请不到内存,对端只能等待超时
	{
		return;
	}

	memset(pMsgRep, 0, sizeof(struct ipc_mesg_n));
	
	pMsgRep->msghdr.opcode = IPC_OPCODE_REPLY;
	if(ipc_send_reply_n1(phdr_rcv, pMsgRep, sizeof(struct ipc_mesg_n)) == -1)
    {
        mem_share_free(pMsgRep, MODULE_ID_SNMPD);
    }
}



