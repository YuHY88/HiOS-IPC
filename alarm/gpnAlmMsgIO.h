/******************************************************************************
 * Filename: gpnAlmMsgIO.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.11  lipf created
 *
******************************************************************************/

#ifndef _ALARM_MSG_IO_H_
#define _ALARM_MSG_IO_H_

#include "lib/msg_ipc_n.h"

#include "lib/gpnSocket/socketComm/gpnSockCommRoleMan.h"
#include "lib/gpnSocket/socketComm/gpnSockCommFuncApi.h"


#define ALARM_GPN_MSG_POLL gpnSockCommApiNoBlockProc()


int alarm_msg_rcv(struct ipc_mesg_n *pmsg, int imlen);
int alarm_socket_msg_rcv(void *para);



#endif /* _ALARM_MSG_IO_H_ */

