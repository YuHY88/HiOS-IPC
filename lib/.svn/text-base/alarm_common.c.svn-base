#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <linux/sysctl.h>
#include <lib/module_id.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>

#include <lib/ifm_common.h>
#include "alarm.h"
#include "log.h"

#include <lib/gpnSocket/socketComm/gpnSockMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h>
#include <lib/gpnSocket/socketComm/gpnSockAlmMsgDef.h>
#include <lib/gpnSocket/socketComm/gpnSockIfmMsgDef.h>


int ipranIndexToGpnIndex(struct gpnPortInfo *pgPortInfo, optObjOrient *pPortIndex)
{
	uint32_t unit 		= 0;
	uint32_t slot 		= 0;
	uint32_t port 		= 0;
	uint32_t subPort 	= 0;
	
	if((NULL == pgPortInfo) || (NULL == pPortIndex))
	{
		return -1;
	}

	unit = IFM_UNIT_ID_GET(pgPortInfo->iIfindex);
	slot = IFM_SLOT_ID_GET(pgPortInfo->iIfindex);
	port = IFM_PORT_ID_GET(pgPortInfo->iIfindex);
	subPort = IFM_SUBPORT_ID_GET(pgPortInfo->iIfindex);
	
	pPortIndex->devIndex = DeviceIndex_Create(unit);
	
	switch(pgPortInfo->iAlarmPort)
	{
		case IFM_FUN_ETH_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port);
			break;

		case IFM_TUNNEL_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slot, subPort);
			break;

		case IFM_FUN_ETH_SUB_TYPE:
		case IFM_VUNI_TYPE:
			pPortIndex->portIndex = FUN_ETH_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port);
			pPortIndex->portIndex3 = subPort;
			break;		
		
		case IFM_SOFT_TYPE:
		case IFM_PWE3E1_TYPE:
		case IFM_POWN48_TYPE:
		case IFM_STMN_TYPE:
		case IFM_PWE3STMN_TYPE:
		case IFM_EQU_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port);
			break;

		case IFM_VC4_TYPE:
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port);
			pPortIndex->portIndex3 = pgPortInfo->iMsgPara1;
			break;

		case IFM_TRUNK_TYPE:
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, 0, port);
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			pPortIndex->portIndex = SMP_PortIndex_Create(pgPortInfo->iAlarmPort, 0, port);
			pPortIndex->portIndex3 = subPort;
			break;

		case IFM_E1_TYPE:
			pPortIndex->portIndex = PDH_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port, pgPortInfo->iMsgPara1);
			break;

		case IFM_E1_SUB_TYPE:
			pPortIndex->portIndex = PDH_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port, pgPortInfo->iMsgPara1);
			pPortIndex->portIndex3 = subPort;
			break;

		case IFM_BFD_TYPE:
			pPortIndex->portIndex = LAB_PortIndex_Create(pgPortInfo->iAlarmPort, slot, pgPortInfo->iMsgPara1);
			break;

		case IFM_VS_2_TYPE:
		case IFM_LSP_2_TYPE:
		case IFM_PW_2_TYPE:
		case IFM_VPLSPW_TYPE:
			pPortIndex->portIndex = LAB_PortIndex_Create(pgPortInfo->iAlarmPort, 0, 0);
			pPortIndex->portIndex3 = pgPortInfo->iMsgPara1;
			break;

		case IFM_VLAN_OAM_TYPE:
			pPortIndex->portIndex = VLAN_PortIndex_Create(pgPortInfo->iAlarmPort, slot, port, subPort);
			break;

		case IFM_CFM_MEP_TYPE:
			pPortIndex->portIndex = CFM_MEP_PortIndex_Create(pgPortInfo->iAlarmPort, 0, pgPortInfo->iMsgPara1);
			break;
			
		default:
			return -1;
	}

	zlog_debug(0, "%s[%d] : gpnIndex(%x|%x|%x|%x|%x|%x)\n", __func__, __LINE__,
		pPortIndex->devIndex, pPortIndex->portIndex, pPortIndex->portIndex3,
		pPortIndex->portIndex4, pPortIndex->portIndex5, pPortIndex->portIndex6);
	return 0;
}


int gpnIndexToIpranIndex(optObjOrient *pPortIndex, struct gpnPortInfo *pgPortInfo)
{
	if((NULL == pgPortInfo) || (NULL == pPortIndex))
	{
		return -1;
	}
	memset(pgPortInfo, 0, sizeof(struct gpnPortInfo));
	
	pgPortInfo->usp.unit = DeviceIndex_GetUnit(pPortIndex->devIndex);
	pgPortInfo->iAlarmPort = PortIndex_GetType(pPortIndex->portIndex);
		
	switch(pgPortInfo->iAlarmPort)
	{
		case IFM_FUN_ETH_TYPE:
			pgPortInfo->usp.slot = IFM_FUN_ETH_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_FUN_ETH_PORTID_DECOM(pPortIndex->portIndex);
			break;

		case IFM_TUNNEL_TYPE:
			pgPortInfo->usp.slot = IFM_FUN_ETH_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.sub_port = IFM_FUN_ETH_PORTID_DECOM(pPortIndex->portIndex);
			break;

		case IFM_FUN_ETH_SUB_TYPE:
		case IFM_VUNI_TYPE:
			pgPortInfo->usp.slot = IFM_FUN_ETH_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_FUN_ETH_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.sub_port = pPortIndex->portIndex3;
			break;

		case IFM_SOFT_TYPE:
		case IFM_PWE3E1_TYPE:
		case IFM_POWN48_TYPE:
		case IFM_STMN_TYPE:
		case IFM_PWE3STMN_TYPE:
		case IFM_EQU_TYPE:
		case IFM_ENV_TYPE:
		case IFM_FAN_TYPE:
		case IFM_POW220_TYPE:
		case IFM_ETH_CLC_IN_TYPE:
		case IFM_ETH_CLC_OUT_TYPE:
		case IFM_ETH_CLC_SUB_TYPE:
		case IFM_ETH_CLC_LINE_TYPE:
		case IFM_ETH_SFP_TYPE:
			pgPortInfo->usp.slot = IFM_SMP_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			break;

		case IFM_VC4_TYPE:
			pgPortInfo->usp.slot = IFM_SMP_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->iMsgPara1 = pPortIndex->portIndex3;
			break;

		case IFM_TRUNK_TYPE:
			pgPortInfo->usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			break;

		case IFM_TRUNK_SUBIF_TYPE:
			pgPortInfo->usp.port = IFM_SMP_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.sub_port = pPortIndex->portIndex3;
			break;

		case IFM_E1_TYPE:
			pgPortInfo->usp.slot = IFM_PDH_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_PDH_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->iMsgPara2 = IFM_PDH_LP_DECOM(pPortIndex->portIndex);
			break;

		case IFM_E1_SUB_TYPE:
			pgPortInfo->usp.slot = IFM_PDH_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_PDH_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->iMsgPara1 = pPortIndex->portIndex3;
			pgPortInfo->iMsgPara2 = IFM_PDH_LP_DECOM(pPortIndex->portIndex);
			break;

		case IFM_BFD_TYPE:			
			pgPortInfo->usp.slot = IFM_LAB_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->iMsgPara1 = IFM_LAB_LAB_DECOM(pPortIndex->portIndex);
			break;

		case IFM_VS_2_TYPE:
		case IFM_LSP_2_TYPE:
		case IFM_PW_2_TYPE:
		case IFM_VPLSPW_TYPE:
			pgPortInfo->iMsgPara1 = pPortIndex->portIndex3;
			break;

		case IFM_VLAN_OAM_TYPE:
			pgPortInfo->usp.slot = IFM_VLAN_SLOT_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.port = IFM_VLAN_PORTID_DECOM(pPortIndex->portIndex);
			pgPortInfo->usp.sub_port = IFM_VLAN_VLAN_DECOM(pPortIndex->portIndex);
			break;

		case IFM_CFM_MEP_TYPE:
			pgPortInfo->iMsgPara1 = IFM_CFM_MEP_MEPID_DECOM(pPortIndex->portIndex);
			break;
			
		default:
			return -1;
	}
	
	return 0;
}




/* func     : 告警注册函数
 * alarmPort: 告警端口类型
 * slotNo   : 槽位号
 * portNo   : 端口号
 * para1    : 参数1，如eth子接口号
 * para2    : 参数2
 */
void ipran_alarm_port_register (struct gpnPortInfo *pgPortInfo)
{
	zlog_debug(0, "%s[%d] : alarmPort(%x), ifindex(%x), para(%d:%d)\n",
		__func__, __LINE__, pgPortInfo->iAlarmPort, pgPortInfo->iIfindex,
		pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2);
		
	optObjOrient portIndex;

	portIndex.devIndex   = GPN_GEN_MSG_INVALID_DEV;
	portIndex.portIndex  = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	
	ipranIndexToGpnIndex(pgPortInfo, &portIndex);

	gpnSockMsg gSockMsg;
	memset(&gSockMsg, 0, sizeof(gpnSockMsg));
	gSockMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_REG;
	gSockMsg.iMsgPara1 = portIndex.portIndex;
	gSockMsg.iMsgPara2 = portIndex.portIndex3;
	gSockMsg.iMsgPara3 = portIndex.portIndex4;
	gSockMsg.iMsgPara4 = portIndex.portIndex5;
	gSockMsg.iMsgPara5 = portIndex.portIndex6;
	gSockMsg.iMsgPara6 = portIndex.devIndex;

	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_ALARM, MODULE_ID_ALARM, 
		IPC_TYPE_ALARM, 0, 0, 1);
	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_STAT, MODULE_ID_STAT, 
		IPC_TYPE_STAT_DB, 0, 0, 1);
}


/* func 	: 告警解注册函数
 * alarmPort: 告警端口类型
 * slotNo	: 槽位号
 * portNo	: 端口号
 * para1	: 参数1，如eth子接口号
 * para2	: 参数2
 */
void ipran_alarm_port_unregister (struct gpnPortInfo *pgPortInfo)
{
	zlog_debug(0, "%s[%d] : alarmPort(%x), ifindex(%x), para(%d:%d)\n",
		__func__, __LINE__, pgPortInfo->iAlarmPort, pgPortInfo->iIfindex,
		pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2);

	optObjOrient portIndex;

	portIndex.devIndex   = GPN_GEN_MSG_INVALID_DEV;
	portIndex.portIndex  = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;
	
	ipranIndexToGpnIndex(pgPortInfo, &portIndex);

	gpnSockMsg gSockMsg;
	memset(&gSockMsg, 0, sizeof(gpnSockMsg));
	gSockMsg.iMsgType = GPN_IFM_MSG_VARI_PORT_UNREG;
	gSockMsg.iMsgPara1 = portIndex.portIndex;
	gSockMsg.iMsgPara2 = portIndex.portIndex3;
	gSockMsg.iMsgPara3 = portIndex.portIndex4;
	gSockMsg.iMsgPara4 = portIndex.portIndex5;
	gSockMsg.iMsgPara5 = portIndex.portIndex6;
	gSockMsg.iMsgPara6 = portIndex.devIndex;

	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_ALARM, MODULE_ID_ALARM, 
		IPC_TYPE_ALARM, 0, 0, 1);
	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_STAT, MODULE_ID_STAT, 
		IPC_TYPE_STAT_DB, 0, 0, 1);
}


/* func 	 : 告警上报函数
 * alarmPort : 告警端口类型
 * slotNo	 : 槽位号
 * portNo	 : 端口号
 * para1	 : 参数1，如eth子接口号
 * para2	 : 参数2
 * alarmCode : 告警编码
 * alarmState: 告警状态, 产生：GPN_SOCK_MSG_OPT_RISE; 消除：GPN_SOCK_MSG_OPT_CLEAN
 */

void ipran_alarm_report (struct gpnPortInfo *pgPortInfo, uint32_t alarmCode, uint32_t alarmState)
{
	zlog_debug(0, "%s[%d] : alarmPort(%x), ifindex(%x), para(%d:%d), alarmCode(0x%08x), state(%s)\n",
		__func__, __LINE__, pgPortInfo->iAlarmPort, pgPortInfo->iIfindex,
		pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2, alarmCode, 
		(GPN_SOCK_MSG_OPT_RISE == alarmState)?"rise":"clear");
	
	optObjOrient portIndex;
	portIndex.devIndex   = GPN_GEN_MSG_INVALID_DEV;
	portIndex.portIndex  = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;

	ipranIndexToGpnIndex(pgPortInfo, &portIndex);

	gpnSockMsg gSockMsg;
	memset(&gSockMsg, 0, sizeof(gpnSockMsg));
	gSockMsg.iMsgType = GPN_ALM_MSG_ALM_REPT_TYPE_BASE;
	gSockMsg.iMsgPara1 = portIndex.portIndex;
	gSockMsg.iMsgPara2 = portIndex.portIndex3;
	gSockMsg.iMsgPara3 = portIndex.portIndex4;
	gSockMsg.iMsgPara4 = portIndex.portIndex5;
	gSockMsg.iMsgPara5 = portIndex.portIndex6;
	gSockMsg.iMsgPara6 = portIndex.devIndex;
	gSockMsg.iMsgPara7 = alarmCode;
	gSockMsg.iMsgPara8 = alarmState;

	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_ALARM, MODULE_ID_ALARM, 
		IPC_TYPE_ALARM, 0, 0, 1);
}


void ipran_alarm_event_report (struct gpnPortInfo *pgPortInfo, uint32_t subEvtType, uint32_t detial)
{
	zlog_debug(0, "%s[%d] : alarmPort(%x), ifindex(%x), para(%d:%d), subEvtType(0x%08x)\n",
		__func__, __LINE__, pgPortInfo->iAlarmPort, pgPortInfo->iIfindex,
		pgPortInfo->iMsgPara1, pgPortInfo->iMsgPara2, subEvtType);
	
	optObjOrient portIndex;
	memset (&portIndex, 0, sizeof(vpUniqueIndex));

	portIndex.devIndex   = GPN_GEN_MSG_INVALID_DEV;
	portIndex.portIndex  = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex3 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex4 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex5 = GPN_GEN_MSG_INVALID_PORT;
	portIndex.portIndex6 = GPN_GEN_MSG_INVALID_PORT;

	ipranIndexToGpnIndex(pgPortInfo, &portIndex);
	
	gpnSockMsg gSockMsg;
	memset(&gSockMsg, 0, sizeof(gpnSockMsg));
	gSockMsg.iMsgType = GPN_ALM_MSG_EVT_REPT_TYPE_BASE;
	gSockMsg.iMsgPara1 = portIndex.portIndex;
	gSockMsg.iMsgPara2 = portIndex.portIndex3;
	gSockMsg.iMsgPara3 = portIndex.portIndex4;
	gSockMsg.iMsgPara4 = portIndex.portIndex5;
	gSockMsg.iMsgPara5 = portIndex.portIndex6;
	gSockMsg.iMsgPara6 = portIndex.devIndex;
	gSockMsg.iMsgPara7 = subEvtType;
	gSockMsg.iMsgPara8 = detial;

	ipc_send_msg_n2((void *)&gSockMsg, sizeof(gpnSockMsg), 1, MODULE_ID_ALARM, MODULE_ID_ALARM, 
		IPC_TYPE_ALARM, 0, 0, 1);
}

					





