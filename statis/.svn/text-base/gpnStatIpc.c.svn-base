/*
 * =====================================================================================
 *
 *       Filename:  gpnStatIpc.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/14/2017 04:34:14 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef _GPN_STAT_IPC_C_
#define _GPN_STAT_IPC_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#define __USE_GNU
#include <sched.h>

#include "lib/thread.h"
#include "lib/module_id.h"
#include "lib/vty.h"
#include "lib/command.h"

#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"

#include "socketComm/gpnSockStatMsgDef.h"

#include "gpnStatIpc.h"
#include "gpnStatFuncApi.h"
#include "gpnStatSnmp.h"
#include "gpnStatCmd.h"

struct thread   pthread;


static void stat_handle_statis_msg(struct ipc_mesg_n *pmsg)
{
	if(NULL == pmsg)
	{
		return;
	}
	
	gpnStatCorePorc((gpnSockMsg *)pmsg->msg_data, pmsg->msghdr.data_len);
}


/* receive common IPC messages */
int stat_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
	if(NULL == pmsg)
	{
		return 0;
	}

	int retva = 0;
    int revln = 0;

    //printf("statis recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        /* 消息处理 */
        switch(pmsg->msghdr.msg_type)
        {
            case IPC_TYPE_SNMP:
				stat_handle_snmp_msg(pmsg);
				break;

			case IPC_TYPE_STAT_DB:
				stat_handle_statis_msg(pmsg);
				break;
            
            default:
                 printf("alarm_msg_rcv, receive unk message\r\n");
                 break;
        }
    }
    else
    {
         printf("alarm recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

	if(pmsg)
	{
    	mem_share_free(pmsg, MODULE_ID_STAT);
	}
    
    return retva;
}




#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_STAT_IPC_C_*/


