/******************************************************************************
 * Filename: ces_alarm.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.12.1  lipf created
 *
******************************************************************************/

#ifndef _CES_ALARM_H_
#define _CES_ALARM_H_

#include "lib/gpnSocket/socketComm/gpnSockCommRoleMan.h"
#include "lib/gpnSocket/socketComm/gpnSockCommFuncApi.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"
#include "lib/gpnSocket/socketComm/gpnAlmTypeDef.h"
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>


//int ces_alarm_socket_msg_rcv (struct thread *t);
int ces_alarm_socket_msg_rcv(void *pmsg);

uint32_t ces_gpn_function_init (void);
void CesAlarmReport(int iAlmType, int iReport, unsigned int ifindex, int iAlmPara);
void stm_alarm_report(int alarm_type, int report, uint32_t ifindex, uint16_t vc4_id, int alarm_param);


#endif /* _CES_ALARM_H_ */



