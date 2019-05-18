/*====================================================================================== 
* file: alarm.h
* by  : lipf 
* note: define all alarm types and level and others
======================================================================================*/

#ifndef _ALARM_H_
#define _ALARM_H_

#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnAlmTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockAlmMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnSockIfmMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnSockTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockCommFuncApi.h>
#include <lib/gpnSocket/socketComm/gpnSockCommRoleMan.h>
#include <lib/gpnDevInfo/devCoreInfo/gpnGlobalDevTypeDef.h>
#include <lib/gpnDevInfo/devCoreInfo/gpnDevCoreInfo.h>

#include <lib/gpnSocket/socketComm/gpnStatTypeDef.h>
#include <lib/gpnSocket/socketComm/gpnSockStatMsgDef.h>


struct gpnPortInfo
{
	uint32_t iAlarmPort;
	uint32_t iIfindex;

	struct ifm_usp usp;
	
	uint32_t iMsgPara1;
	uint32_t iMsgPara2;
};


int ipranIndexToGpnIndex (struct gpnPortInfo *pgPortInfo, optObjOrient *pPortIndex);
int gpnIndexToIpranIndex (optObjOrient *pPortIndex, struct gpnPortInfo *pgPortInfo);

void ipran_alarm_port_register (struct gpnPortInfo *pgPortInfo);
void ipran_alarm_port_unregister (struct gpnPortInfo *pgPortInfo);
void ipran_alarm_report (struct gpnPortInfo *pgPortInfo, uint32_t alarmCode, uint32_t alarmState);
void ipran_alarm_event_report (struct gpnPortInfo *pgPortInfo, uint32_t subEvtType, uint32_t detial);

#endif



