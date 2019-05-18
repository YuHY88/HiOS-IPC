/*
* 
*/
#ifndef _TESTDATASTRUCTURE_H
#define _TESTDATASTRUCTURE_H

//#include "alarmPortMonitorCfgTable.h"
#include "socketComm/gpnSockMsgDef.h"

//tableµƒ∂¡–¥≤‚ ‘ π”√

#define GPN_TIMER_GEN_OK 				GPN_SOCK_SYS_OK
#define GPN_TIMER_GEN_ERR 				GPN_SOCK_SYS_ERR



#define GPN_ALM_MSG_PORT_MONITOR_GET				0xffff0000
#define GPN_ALM_MSG_PORT_MONITOR_GET_ACK			0xffff0001
#define GPN_ALM_MSG_PORT_MONITOR_GET_RSP			0xffff0002
#define GPN_ALM_MSG_PORT_MONITOR_GET_RSP_ACK		0xffff0003
#define GPN_ALM_MSG_PORT_MONITOR_GET_NEXT			0xffff0004
/*#define GPN_ALM_MSG_PORT_MONITOR_GET_NEXT_ACK		0xffff0005
#define GPN_ALM_MSG_PORT_MONITOR_GET_NEXT_RSP		0xffff0006
#define GPN_ALM_MSG_PORT_MONITOR_GET_NEXT_RSP_ACK	0xffff0007*/


#define GPN_ALM_MSG_CURR_DATA_DB_GET				0xfffe0000
#define GPN_ALM_MSG_CURR_DATA_DB_GET_RSP			0xfffe0000
#define GPN_ALM_MSG_CURR_DATA_DB_GET_NEXT			0xfffe0002





extern long testPortMonitorData[3][7];

/* function declarations */

//*my test Function*
unsigned int gpnAlmPortMonitorGet(unsigned int moniStaDBId, gpnSockMsg *ppgpnAlmMsgSpOut);

UINT32 gpnAlmPortMonitorEnSet(UINT32 index,UINT32 opt);
UINT32 gpnAlmSnmpApiPortMoniDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiPortMoniDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiAttribDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiAttribDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiCurrAlmDBGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiCurrAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiCurrAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiHistAlmDBGet(gpnSockMsg *pgpnStatMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnStatMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiHistAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiHistAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiEventDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiEventDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiDebugCurrAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiDebugCurrAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiDebugCurrAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiDebugHistAlmDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiDebugHistAlmDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiDebugHistAlmDBModify(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);

UINT32 gpnAlmSnmpApiDebugEventDBGet(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);
UINT32 gpnAlmSnmpApiDebugEventDBGetNext(gpnSockMsg *pgpnAlmMsgSpIn, UINT32 inLen, gpnSockMsg *pgpnAlmMsgSpOut, UINT32 outLen);



#endif /* _TESTDATASTRUCTURE_H */
