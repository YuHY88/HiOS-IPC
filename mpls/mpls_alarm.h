/******************************************************************************
 * Filename: mpls_alarm.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.12.1  zfj created
 *
******************************************************************************/

#ifndef _MPLS_ALARM_H_
#define _MPLS_ALARM_H_

#include "lib/gpnSocket/socketComm/gpnSockCommRoleMan.h"
#include "lib/gpnSocket/socketComm/gpnSockCommFuncApi.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"
#include "lib/gpnSocket/socketComm/gpnAlmTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockIfmMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSockAlmMsgDef.h"
#include "lib/alarm.h"


int mpls_alarm_socket_msg_rcv (struct thread *t);
uint32_t mpls_gpn_function_init (void);


#endif /* _MPLS_ALARM_H_ */



