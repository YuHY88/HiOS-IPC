/**********************************************************
* file name: gpnSockInitdMsgDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-11
* function: 
*    define details about communication between gpnInitd modules and others
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_INITD_MSG_DEF_H_
#define _GPN_SOCK_INITD_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"
#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"

/***********************************/
/*define owner : GPN_SOCK_OWNER_INIT*/
/***********************************/

/*=====================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include normal comm msg used by gpn_init                                            */
#define GPN_INIT_MT_NORMAL						((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 1                                                                        */
/****************************************************************************************/
#define GPN_INIT_MST_ND							((1 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :   1                                                                         */
/****************************************************************************************/
#define GPN_INIT_SST_ND_REQ						((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_CFG_DOWN_RSP               ((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_STEP_OK_REPT				((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_CFG_DOWN_REQ               ((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 2                                                                        */
/****************************************************************************************/
#define GPN_INIT_MST_HIT_ME_REQ					((2 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :   1                                                                         */
/****************************************************************************************/
#define GPN_INIT_SST_RERUN_ME_REQ				((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_RERUN_HE_REQ				((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_REBOOT_SYS_REQ				((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_KILL_ME_REQ				((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_FAIL_NOT					((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 3                                                                        */
/****************************************************************************************/
#define GPN_INIT_MST_MON_ADD					((3 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :   1                                                                         */
/****************************************************************************************/
#define GPN_INIT_SST_MON_ADD_HW_CHECK			((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_ADD_PROCESS			((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_DEL_PROCESS			((3 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_ADD_STARUP_DELAY		((4 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_ADD_METHOD				((5 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_ADD_ND_OPT				((6 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_ADD_MNA				((7 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_MON_NEED_ASK				((8 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/
/****************************************************************************************/
/*define main type 1's main-sub type  (from 1 to 0xFE)  : 4                                                                        */
/****************************************************************************************/
#define GPN_INIT_MST_SNMPD_COMM_CHECK			((4 << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
/****************************************************************************************/
/*define main type 1's sub-sub type  (from 1 to 0xFE)  :   1                                                                         */
/****************************************************************************************/
#define GPN_INIT_SST_SNMPD_R_COMMITY_GET		((1 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
#define GPN_INIT_SST_NOTIFY_SNMPD_R_COMMITY		((2 << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)
/****************************************************************************************/


#define  GPN_INITD_PROCESS_ND_REQ				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_ND)|(GPN_INIT_SST_ND_REQ)) /*gpnInit send nd-msg to other processes in order to diagnosis them*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_PROCESS_ND_REQ
*/
#define  GPN_INITD_PROCESS_ND_REQ_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_PROCESS_ND_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/
#define  GPN_INITD_PROCESS_ND_RSP				((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_INITD_PROCESS_ND_REQ))   /*other modu send nd-rspons-msg to other modu in order to report diagnosis result*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
	iMsgType:	GPN_INITD_PROCESS_ND_RSP
	iMsgPara1: self-pid
	iMsgPara2: process statue(1:ok,2:fail)
	iMsgPara3: process fail resean (0:normal err;....)
*/
#define  GPN_INITD_PROCESS_ND_RSP_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_PROCESS_ND_RSP))   /*other modu send nd-rspons-msg to other modu in order to report diagnosis result*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
*/


#define GPN_INITD_MSG_CFG_DOWN_NOTIFY			((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_ND)|(GPN_INIT_SST_CFG_DOWN_RSP)) /*gpnInit send to others modu to notify 'config down', means system init ok  */
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	need ACK
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MSG_CFG_DOWN_NOTIFY
	iMsgPara1: who send config down
	iMsgPara2: 1-config down;2 not config down
	msgCellLen= 0
*/
#define GPN_INITD_MSG_CFG_DOWN_NOTIFY_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MSG_CFG_DOWN_NOTIFY))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
	iMsgType:	GPN_INITD_MSG_CFG_DOWN_NOTIFY_ACK
*/
	
#define GPN_INITD_MSG_CFG_DOWN_REQ				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_ND)|(GPN_INIT_SST_CFG_DOWN_REQ)) /*other module send to gpn_init modu to ask 'config down' state  */
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:    need ACK
	iDstId: 	GPN_INIT
	iSrcId: 	other mod
	iMsgType:	GPN_INITD_MSG_CFG_DOWN_REQ
	iMsgPara1: req role
	msgCellLen= 0
*/
#define GPN_INITD_MSG_CFG_DOWN_REQ_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MSG_CFG_DOWN_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MSG_CFG_DOWN_REQ_ACK
*/

#define GPN_INIT_MSG_PARA_SUS1					1
#define GPN_INIT_MSG_PARA_SUS2					2
#define GPN_INIT_MSG_PARA_SUS3					3
#define GPN_INIT_MSG_PARA_SUS4					4
#define GPN_INIT_MSG_PARA_SUS5					5
#define GPN_INIT_MSG_PARA_SUS6					6

#define  GPN_INITD_MSG_STEP_OK_REPT				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_ND)|(GPN_INIT_SST_STEP_OK_REPT)) /*others send to gpnInit startUp-stepx-ok-msg to notify gpnInit do next step */
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
	iMsgType:	GPN_INITD_MSG_STEP_OK_REPT
	iMsgPara1: step index(1 2 ...)
	iMsgPara2: step end statue(1:ok,2:fail)
*/
#define  GPN_INITD_MSG_STEP_OK_REPT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MSG_STEP_OK_REPT))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MSG_STEP_OK_REPT_ACK
*/


#define  GPN_INITD_RERUN_ME_REQ					((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_HIT_ME_REQ)|(GPN_INIT_SST_RERUN_ME_REQ)) /*request gpn_init to rerun self*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_RERUN_ME_REQ
	iMsgPara1: self-pid
	iMsgPara2: delay kill time(default 0)
	iMsgPara3: delay restartup time(default 0)
*/
#define  GPN_INITD_RERUN_ME_REQ_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_RERUN_ME_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_RERUN_HE_REQ					((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_HIT_ME_REQ)|(GPN_INIT_SST_RERUN_HE_REQ)) /* request gpn_init to rerun someone */
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_RERUN_HE_REQ
	iMsgPara1: self-pid
	iMsgPara2: delay kill time(default 0)
	iMsgPara3: delay restartup time(default 0)
	msgCellLen= strlen(str)
	payload:"$(OPT PROCESS'S NAME)"
	
*/
#define  GPN_INITD_RERUN_HE_REQ_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_RERUN_HE_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/


#define  GPN_INITD_REBOOT_SYS_REQ				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_HIT_ME_REQ)|(GPN_INIT_SST_REBOOT_SYS_REQ)) /*request gpn_init to reboot system*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_REBOOT_SYS_REQ
	iMsgPara1: self-pid
	iMsgPara2: delay time before reboot system(default 0)
*/
#define  GPN_INITD_REBOOT_SYS_REQ_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_REBOOT_SYS_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_KILL_ME_REQ					((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_HIT_ME_REQ)|(GPN_INIT_SST_KILL_ME_REQ)) /*request gpn_init to kill self*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_KILL_ME_REQ
	iMsgPara1: self-pid
	iMsgPara2: delay kill time(default 0)
*/
#define  GPN_INITD_KILL_ME_REQ_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_KILL_ME_REQ))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_PROCESS_FAIL_NOT				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_HIT_ME_REQ)|(GPN_INIT_SST_FAIL_NOT)) /*notify gpn_init : I am bad*/
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_PROCESS_FAIL_NOT
	iMsgPara1: self-pid
	iMsgPara2: delay time before process  this fatal fail(default 0)
	iMsgPara3: process fatal fail method(default method 0 is gpnInit-base)
*/
#define  GPN_INITD_PROCESS_FAIL_NOT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_PROCESS_FAIL_NOT))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_HW_CHECK				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_HW_CHECK))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_HW_CHECK
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload: Check(n): "methodx (indicate)	"
*/
#define  GPN_INITD_MON_ADD_HW_CHECK_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_HW_CHECK))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_PROCESS				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_PROCESS))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_PROCESS
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) arg ..."
*/
#define  GPN_INITD_MON_ADD_PROCESS_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_PROCESS))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_DEL_PROCESS				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_DEL_PROCESS))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_DEL_PROCESS
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) arg ..."
*/
#define  GPN_INITD_MON_DEL_PROCESS_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_DEL_PROCESS))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_STARUP_DELAY				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_STARUP_DELAY))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_STARUP_DELAY
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) xs"
*/
#define  GPN_INITD_MON_ADD_STARUP_DELAY_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_STARUP_DELAY))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_METHOD				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_METHOD))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_METHOD
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) reboot/runself/ignor/'process series'"
*/
#define  GPN_INITD_MON_ADD_METHOD_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_METHOD))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_ND_OPT				((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_ND_OPT))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_ND_OPT
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) enable/disable"
*/
#define  GPN_INITD_MON_ADD_ND_OPT_ACK			((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_ND_OPT))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_ADD_MNA					((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_ADD_MNA))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INIT
	iMsgType:	GPN_INITD_MON_ADD_MNA
	iMsgPara1: self-pid
	msgCellLen= strlen(str)
	payload:"$(FULL_PATH) enable/disable"
*/
#define  GPN_INITD_MON_ADD_MNA_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_ADD_MNA))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
*/

#define  GPN_INITD_MON_NEED_ASK					((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_MON_ADD)|(GPN_INIT_SST_MON_NEED_ASK))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu
	iMsgType:	GPN_INITD_MON_NEED_ASK
	msgCellLen= 0
*/
#define  GPN_INITD_MON_NEED_ASK_ACK				((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_MON_NEED_ASK))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	other modu
	iSrcId: 	GPN_INITu
*/

#define  GPN_INITD_SNMP_R_COMMITY_GET			((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_SNMPD_COMM_CHECK)|(GPN_INIT_SST_SNMPD_R_COMMITY_GET))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iSrcId: 	GPN_INIT
	iDstId: 	other modu(may be sdmd)
	iMsgType:	GPN_INITD_SNMP_R_COMMITY_GET
	msgCellLen= 0
*/
#define  GPN_INITD_SNMP_R_COMMITY_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_SNMP_R_COMMITY_GET))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iDstId: 	GPN_INIT
	iSrcId: 	other modu(may be sdmd)
*/

#define  GPN_INITD_NOTIFY_SNMPD_R_COMMITY		((GPN_SOCK_OWNER_INIT)|(GPN_INIT_MT_NORMAL)|(GPN_INIT_MST_SNMPD_COMM_CHECK)|(GPN_INIT_SST_NOTIFY_SNMPD_R_COMMITY))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iSrcId: 	other modu(may be sdmd)
	iDstId: 	GPN_INIT
	iMsgType:	GPN_INITD_NOTIFY_SNMPD_R_COMMITY
	iMsgPara1: commity lenth
	msgCellLen= strlen(commity)
	msgPaload = "commity"
*/
#define  GPN_INITD_NOTIFY_SNMPD_R_COMMITY_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_INITD_NOTIFY_SNMPD_R_COMMITY))
/*parameter indication:
	iIndex: 	iIndex++
	iMsgCtr:	
	iSrcId: 	GPN_INITu
	iDstId: 	other modu(may be sdmd)
*/


#define GPN_INITD_CMD_STR_MAX_LEN				1024
#define GPN_INITD_COMMITY_MAX_LEN				32

UINT32 gpnSockInitdMsgNDRequest(UINT32 MODU_ID);
UINT32 gpnSockInitdMsgNDResponse(pid_t pid, UINT32 runSta);
UINT32 gpnSockInitdMsgTxIsCfgDownAsk(void);
UINT32 gpnSockInitdMsgTxCfgDown2ValidModule(void);
UINT32 gpnSockInitdMsgTxCfgDown2OtherModule(UINT32 MODU_ID, UINT32 cfg_down);

UINT32 gpnSockInitdMsgSUStepXEndNotify(UINT32 stepIndex, UINT32 sta);

UINT32 gpnSockInitdMsgReRunMeRequest(pid_t pid, UINT32 killDelay, UINT32 upDelay);
UINT32 gpnSockInitdMsgReRunHeRequest(pid_t pid, char *procName, UINT32 killDelay, UINT32 upDelay);;
UINT32 gpnSockInitdMsgRebootRequest(pid_t pid, UINT32 delay);
UINT32 gpnSockInitdMsgKillRequest(pid_t pid, UINT32 delay);
UINT32 gpnSockInitdMsgFatalFail(pid_t pid, UINT32 delay, UINT32 method);

UINT32 gpnSockInitdMsgHwcMonAdd(pid_t pid, char *hwcStr);
UINT32 gpnSockInitdMsgPorcessMonAdd(pid_t pid, char *processStr);
UINT32 gpnSockInitdMsgPorcessMonDel(pid_t pid, char *processStr);
UINT32 gpnSockInitdMsgAdMonAdd(pid_t pid, char *starupDlyStr);
UINT32 gpnSockInitdMsgMethodMonAdd(pid_t pid, char *methodStr);
UINT32 gpnSockInitdMsgNdMonAdd(pid_t pid, char *ndOptStr);
UINT32 gpnSockInitdMsgMNAAdd(pid_t pid, char *MNAOptStr);
UINT32 gpnSockInitdMsgMonRegisterNotify(pid_t pid,
										char *processStr,
										char *starupDlyStr,
										char *methodStr,
										char *ndOptStr,
										char *MNAOptStr);
UINT32 gpnSockInitdMsgNeedMonAsk(UINT32 MODU_ID);
UINT32 gpnSockInitdMsgSnmpdCommityGet(UINT32 MODU_ID);
UINT32 gpnSockInitdMsgNotifySnmpdCommity(char *commity, UINT32 len);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_INITD_MSG_DEF_H_*/

