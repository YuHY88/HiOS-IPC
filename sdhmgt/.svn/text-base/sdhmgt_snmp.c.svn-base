/**********************************************************
* file name: sdhmgt_snmp.c
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-15
* function:
*
* modify:
*
***********************************************************/

#include <string.h>
#include <stdio.h>

#include <lib/msg_ipc_n.h>

#include "module_id.h"
#include "msg_ipc.h"
#include "errcode.h"
#include "types.h"
#include "msdh_common.h"
#include "sdhmgt_socket.h"
#include "sdhmgt_snmp.h"
#include "sdhmgt.h"

void sdhmgt_msg_to_snmp(uint8_t *pdata, int data_len, uint8_t subtype)
{
    if (NULL == pdata)
    {
        return;
    }

    //ipc_send_msdh(pdata, data_len, 1, MODULE_ID_SNMPD, MODULE_ID_SDHMGT, IPC_TYPE_SDHMGT, subtype, 0);

	ipc_send_msg_n2(pdata, data_len, 1, MODULE_ID_SNMPD_TRAP, MODULE_ID_SDHMGT, IPC_TYPE_SDHMGT, subtype, 0, 0);
}

int sdhmgt_msg_rcv_snmp(struct ipc_mesg_n *pmesg)
{
    struct ipc_msghdr_n *phdr = NULL;
    uint8_t *pframe = NULL;
    int ret;

    if (NULL == pmesg)
    {
        return ERRNO_FAIL;
    }

    phdr = &(pmesg->msghdr);
    pframe = pmesg->msg_data;

    if (g_sdhmgt_entry.ms_status != DEVM_HA_MASTER)
    {
        return ERRNO_FAIL;
    }
	
    ret = msdh_frm_isok((t_msdhinf *)pframe, phdr->data_len);

    if (1 == ret)
    {
        sdhmgt_send_msdh_for_sdh_device(pframe, phdr->data_len);
    }

    return ERRNO_SUCCESS;
}

