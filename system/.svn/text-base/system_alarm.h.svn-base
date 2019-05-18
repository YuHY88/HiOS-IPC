/******************************************************************************
 * Filename: xxx_alarm.h
 *  Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.12.1  lipf created
 *
******************************************************************************/

#ifndef _SYSTEM_ALARM_H_
#define _SYSTEM_ALARM_H_

#include "lib/gpnSocket/socketComm/gpnSockCommRoleMan.h"
#include "lib/gpnSocket/socketComm/gpnSockCommFuncApi.h"
#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"
#include "lib/gpnSocket/socketComm/gpnAlmTypeDef.h"
#include "lib/thread.h"

#define SYSTEM_DAFUALUT_SLOT 0
#define SYSTEM_DAFUALUT_PORT 0



void SystemAlarmReport(int iAlmType, int iReport, unsigned int ifindex, int iAlmPara);

uint32_t system_gpn_function_init(void);
int32_t gpnSystemCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len);


#endif /* _SYSTEM_ALARM_H_ */



