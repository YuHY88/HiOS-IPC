/******************************************************************************
 * Filename: xxx_alarm.c
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

#include <string.h>

#include <lib/command.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/types.h>
#include <lib/alarm.h>
#include "ces_if.h"
#include "ces_alarm.h"


extern struct thread_master *ces_master;

/**
* @brief      report stm alarms
* @param alarm_type: alarm type
* @param report: action--rise or clean
* @param ifindex: port ifindex
* @param vc4_id: vc4_id of ifindex
* @param alarm_param: extra alarm_param
* @author     liubo
* @date       2018-12-05
* @note       for hp alarm, vc4_id==1 for now. other alarms like rs and ms, vc4_id==0
*/
void stm_alarm_report(int alarm_type, int report, uint32_t ifindex, uint16_t vc4_id, int alarm_param)
{
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_STMN_TYPE;
	gPortInfo.iIfindex = ifindex;

    switch(alarm_type)
    {
        case STM_ALARM_OPT_LOS:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_SPI_LOS, report);
            break;
        case STM_ALARM_OPT_LOF:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_RS_LOF, report);
            break;
        case STM_ALARM_REMOTE_OFF:
            break;
        case STM_ALARM_TU_LOM:
            break;
        case STM_ALARM_AU_LOP:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_AU_LOP, report);
            break;
        case STM_ALARM_AU_AIS:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_AU_AIS, report);
            break;
        case STM_ALARM_RS_TIM:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_RS_TIM, report);
            break;
        case STM_ALARM_RS_EXC:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_RS_EXC, report);
            break;
        case STM_ALARM_RS_DEG:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_RS_DEG, report);
            break;
        case STM_ALARM_MS_AIS:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MS_AIS, report);
            break;
        case STM_ALARM_MS_RDI:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MS_RDI, report);
            break;
        case STM_ALARM_MS_REI:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MS_REI, report);
            break;
        case STM_ALARM_MS_EXC:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MS_EXC, report);
            break;
        case STM_ALARM_MS_DEG:
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_MS_DEG, report);
            break;
        case STM_ALARM_HP_TIM:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_TIM, report);
            break;
        case STM_ALARM_HP_SLM:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_PLM, report);
            break;
        case STM_ALARM_HP_UNQ:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_UNEQ, report);
            break;
        case STM_ALARM_HP_REI:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_REI, report);
            break;
        case STM_ALARM_HP_RDI:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_RDI, report);
            break;
        case STM_ALARM_HP_EXC:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_EXC, report);
            break;
        case STM_ALARM_HP_DEG:
            gPortInfo.iAlarmPort = IFM_VC4_TYPE;
            gPortInfo.iMsgPara1 = vc4_id;
            ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_HP_DEG, report);
            break;
            break;        
        default: 
            break;
    }

	return;
}

void CesAlarmReport(int iAlmType, int iReport, unsigned int ifindex, int iAlmPara)
{
    /*unsigned int iPortNum = 0;
    unsigned int iSlotNum = 0;
    iPortNum = IFM_PORT_ID_GET(ifindex);
    iSlotNum = IFM_SLOT_ID_GET(ifindex);*/

	/* modify for ipran by lipf, 2018/4/25 */
	struct gpnPortInfo gPortInfo;
	memset(&gPortInfo, 0, sizeof(struct gpnPortInfo));
	gPortInfo.iAlarmPort = IFM_PWE3E1_TYPE;
	gPortInfo.iIfindex = ifindex;

    switch(iAlmType)
    {
        case CES_PCK_FULLLOS:
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PWE3_AP_LOS, iReport);
             break;

        case CES_PCK_LOS:
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PWE3_P_LOS, iReport);
             break;

        case CES_PCK_FULLLOS_REMOTE:   
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PWE3_R_APLOS, iReport);
             break;
		case CES_LOS_AIS_LOCAL:
			 {
			 	gPortInfo.iAlarmPort = IFM_E1_TYPE;
				if(iAlmPara)
				{
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PPI_LOS, iReport);
				}
				else
				{
					ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PDH_AIS, iReport);
				}
			 }
			 break;
		case CES_E1_LOS_REMOTE:
			 gPortInfo.iAlarmPort = IFM_PWE3E1_TYPE;
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PWE3_R_PNULL, iReport);
			 break;
		case CES_E1_LOF:
			 gPortInfo.iAlarmPort = IFM_E1_TYPE;
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PDH_LOF, iReport);
  			 break;
		case CES_E1_LOMF:
			 gPortInfo.iAlarmPort = IFM_E1_TYPE;
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PDH_LOM, iReport);
			 break;
		case CES_E1_CRCERR:
			 gPortInfo.iAlarmPort = IFM_E1_TYPE;
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PDH_CRC, iReport);
			 break;
		case CES_E1_HDBE_ERR:
			 gPortInfo.iAlarmPort = IFM_E1_TYPE;
			 ipran_alarm_report(&gPortInfo, GPN_ALM_TYPE_PDH_EXC, iReport);
			 break;
        default: break;
    }

	return;
}


//int ces_alarm_socket_msg_rcv(struct thread *t)
int ces_alarm_socket_msg_rcv(void *pmsg)
{
    gpnSockCommApiNoBlockProc();
    //usleep (100000); //ÈÃ³ö CPU 100ms
    //thread_add_event (ces_master, ces_alarm_socket_msg_rcv, NULL, 0);
    
	return 0;
}

static uint32_t gpnCesCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
	return 1;
}


uint32_t ces_gpn_function_init (void)
{
	uint32_t reVal;
	
	reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_CES, (usrMsgProc)gpnCesCoreProc);
	if(reVal == 2)
	{
		return 2;
	}

	return 1;
}





