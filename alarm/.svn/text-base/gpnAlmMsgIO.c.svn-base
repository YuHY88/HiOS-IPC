/******************************************************************************
 * Filename: gpnAlmMsgIO.c
 *	Copyright (c) 2016-2016 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.11.16  lipf created
 *
******************************************************************************/


#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>

#include "lib/module_id.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/alarm.h"
#include "lib/vty.h"

#include "gpnAlmMsgIO.h"
#include "gpnAlmSnmp.h"
#include "gpnAlmMsgProc.h"

extern struct thread_master *alarm_master;


static void alarm_handle_alarm_report_msg(struct ipc_mesg_n *pmsg)
{
	if(NULL == pmsg)
	{
		return;
	}
	
	gpnAlmSockMsgProc((gpnSockMsg *)pmsg->msg_data, pmsg->msghdr.data_len);
}



/* receive common IPC messages */
int alarm_msg_rcv(struct ipc_mesg_n *pmsg, int imlen)
{
    /*static struct ipc_mesg mesg;
	memset (&mesg, 0, sizeof (struct ipc_mesg));

    if (ipc_recv_common (&mesg, MODULE_ID_ALARM) == -1)
    {
		//zlog_err("%s[%d] recv msg err (%s)\n", __FUNCTION__, __LINE__, strerror(errno));
        goto out;
    }

	switch (mesg.msghdr.msg_type)
	{
		case IPC_TYPE_SNMP:
			alarm_handle_snmp_msg (&mesg);
			break;

		default:
			break;
	}

out:
    usleep (1000); //让出 CPU 10ms
    thread_add_event (alarm_master, alarm_msg_rcv, NULL, 0);

	return 0;*/

	if(NULL == pmsg)
	{
		return 0;
	}

	int retva = 0;
    int revln = 0;

    //printf("alarm recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        /* 消息处理 */
        switch(pmsg->msghdr.msg_type)
        {
            case IPC_TYPE_SNMP:
				alarm_handle_snmp_msg(pmsg);
				break;

			case IPC_TYPE_ALARM:
				alarm_handle_alarm_report_msg(pmsg);
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

    mem_share_free(pmsg, MODULE_ID_ALARM);

    
    return retva;
}


/* receive common IPC messages */
int alarm_socket_msg_rcv(void *para)
{
    ALARM_GPN_MSG_POLL;
    //usleep (10000); //让出 CPU 100ms
    //thread_add_event (alarm_master, alarm_socket_msg_rcv, NULL, 0);

	return 0;
}


