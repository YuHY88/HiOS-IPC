/**********************************************************
* file name: gpnSockIfmMsgDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-28
* function: 
*    define details about communication between gpn_ifm modules and others
* modify:
*	huahuan wangyx 2014-08-04 (add debug msg to every module)	
*
***********************************************************/
#ifndef _GPN_SOCK_IFM_MSG_DEF_H_
#define _GPN_SOCK_IFM_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#include "lib/gpnSocket/socketComm/gpnSockMsgDef.h"

#include "lib/gpnSocket/socketComm/gpnSelectOpt.h"

#include "lib/gpnSocket/socketComm/gpnGlobalPortIndexDef.h"


/**********************************/
/*define owner : GPN_SOCK_OWNER_IFM*/
/**********************************/

/*=====================================================*/
/*define main type  (from 1 to 0x7E)  : 1                                                                        */
/*************************************************************************/
/*this main type include interface change notify msg(like register, unregister)                     */
#define GPN_IMF_MT_IFC_STATUS				((1 << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
/*************************************************************************/
/*define main type 1's sub type  (from 1 to 0xFFFE)  :                                                     */
/*************************************************************************/
#define GPN_IMF_ST_DEV_STATUS_GET			((1 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_DEV_STATUS_CHANG			((2 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_DEV_STATUS_NOTIFY		((3 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_PORT_STATUS_GET			((4 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_FIX_PORT_REG				((5 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_FIX_PORT_UNREG			((6 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_VARI_PORT_REG			((7 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_VARI_PORT_UNREG			((8 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
/*open or close some role module proccess debug*/
#define GPN_IMF_ST_ROLE_DEBUG_CTRL			((9 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_PEER_PORT_REPLACE		((10 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_IMF_ST_VIEW_PORT_REPLACE		((11 << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)

/*=====================================================*/

#define GPN_IFM_MSG_DEV_STATUS_GET			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_DEV_STATUS_GET))	/*get device status */
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_DEV_STATUS_GET
                   	iSrcId     = other modu
                   	iDstId     = IFM
                   	iMsgPara1  = devIndex
                   	msgCellLen = 0
*/

#define GPN_IFM_MSG_DEV_STATUS_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_DEV_STATUS_GET))	/*get device status ACK*/
/* msg parameters direction:
			iIndex     	= CMD_Index
			iMsgType   = GPN_IFM_MSG_FIX_PORT_REG_ACK
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
*/

#define GPN_IFM_MSG_DEV_STATUS_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_DEV_STATUS_GET))	/*get device status response*/
/* msg parameters direction:
			iIndex     	= tx_Index++
                   	iMsgType   = GPN_IFM_MSG_DEV_STATUS_GET_RSP
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
                   	iMsgPara2  = dev status(1 ???)
                   	msgCellLen= 0
*/

#define GPN_IFM_MSG_DEV_STATUS_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_DEV_STATUS_GET_RSP))	/*get device status response ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType  	= GPN_IFM_MSG_DEV_STATUS_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= IFM
*/

/*
see in devCoreInfo

#define GPN_DEV_STA_NULL					0
#define GPN_DEV_STA_PULL					1
#define GPN_DEV_STA_INSERT					2
#define GPN_DEV_STA_RUN					3
#define GPN_DEV_STA_MISMATCH                  		4
#define GPN_DEV_STA_ERR					5
#define GPN_DEV_STA_FAKE					6

*/
#define GPN_IFM_MSG_DEV_STATUS_CHANG		((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_DEV_STATUS_CHANG))	/*device status chang notify*/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_DEV_STATUS_CHANG
			iSrcId     = other modu(IFM)
			iDstId     = IFM(other modu)
			iMsgPara1  = devIndex
			iMsgPara2  = god devIndex
			iMsgPara3  = god portIndex;
			iMsgPara4  = god portIndex3;
			iMsgPara5  = god portIndex4;
			iMsgPara6  = god portIndex5;
			iMsgPara7  = god portIndex6;
			iMsgPara8  = new dev status(see in devCoreInfo)
			iMsgPara9  = notify pid
			msgCellLen = 0
*/

#define GPN_IFM_MSG_DEV_STATUS_CHANG_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_DEV_STATUS_CHANG))	/*device status chang notify ACK*/
/* msg parameters direction:
			iIndex     	= CMD_Index
			iMsgType   = GPN_IFM_MSG_DEV_STATUS_CHANG_ACK
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
*/


/*GPN_NOTIFY of GPN_IFM port system unique location define */
typedef struct _portUniqueIndex_
{
	UINT32 devIndex;
	UINT32 portIndex;
	UINT32 portIndex3;
	UINT32 portIndex4;
	UINT32 portIndex5;
	UINT32 portIndex6;
}portUniqueIndex;

#define GPN_IFM_MSG_PORT_STATUS_GET			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_PORT_STATUS_GET))	/*get prot status */
/* msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType   = GPN_IFM_MSG_DEV_STATUS_GET
                   	iSrcId     	= other modu
                   	iDstId     	= IFM
                   	iMsgPara1  = location index num(1=<i<=6)
                   	iMsgPara2  = portIndex1
                   	iMsgPara3  = portIndex2
                   	iMsgPara4  = portIndex3
                   	iMsgPara5  = portIndex4
                   	iMsgPara6  = portIndex5
                   	iMsgPara7  = portIndex6
                   	msgCellLen = 0
*/

#define GPN_IFM_MSG_PORT_STATUS_GET_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_DEV_STATUS_GET))	/*get prot status ACK*/
/* msg parameters direction:
			iIndex     	= CMD_Index
			iMsgType   = GPN_IFM_MSG_FIX_PORT_REG_ACK
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
*/

#define GPN_IFM_MSG_PORT_STATUS_GET_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_DEV_STATUS_GET))	/*get prot status response*/
/* msg parameters direction:
			iIndex     	= tx_Index++
                   	iMsgType   = GPN_IFM_MSG_DEV_STATUS_GET_RSP
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
                   	iMsgPara1  = location index num(1=<i<=6)
                   	iMsgPara2  = portIndex1
                   	iMsgPara3  = portIndex2
                   	iMsgPara4  = portIndex3
                   	iMsgPara5  = portIndex4
                   	iMsgPara6  = portIndex5
                   	iMsgPara7  = portIndex6
                   	iMsgPara8  = port status(1 ???)
                   	msgCellLen= 0
*/

#define GPN_IFM_MSG_PORT_STATUS_GET_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_PORT_STATUS_GET_RSP))	/*get prot status response ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType  	= GPN_IFM_MSG_PORT_STATUS_GET_RSP_ACK
			iSrcId	= other modu
			iDstId	= IFM
*/


/*GPN_NOTIFY of GPN_IFM Fix port notify format struct*/
typedef struct _stFixPorcReg_
{
	UINT32 portType;
	UINT32 portNum;
}stFixPorcReg;

#define GPN_IFM_MSG_FIX_PORT_REG			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_FIX_PORT_REG))	/*fix port register*/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_FIX_PORT_REG
                   	iSrcId     = IFM
                   	iDstId     = other modu
                   	iMsgPara1  = devIndex
                   	iMsgPara2  = notify pid
                   	msgCellLen = sizeof(Payload)
                   	Payload    = stFixPorcReg[x]
*/

#define GPN_IFM_MSG_FIX_PORT_REG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_FIX_PORT_REG))	/*fix port register ACK*/
/* msg parameters direction:
			iIndex     	= CMD_Index
			iMsgType   = GPN_IFM_MSG_FIX_PORT_REG_ACK
                   	iSrcId     	= other modu
                   	iDstId     	= IFM
*/

#define GPN_IFM_MSG_FIX_PORT_REG_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_FIX_PORT_REG))	/*fix port register response*/
/* msg parameters direction:
			iIndex     	= tx_Index++
                   	iMsgType   = GPN_IFM_MSG_FIX_PORT_REG_RSP
                   	iSrcId     	= other modu
                   	iDstId     	= IFM
                   	iMsgPara1  = respons return value
                   	msgCellLen= 0
*/

#define GPN_IFM_MSG_FIX_PORT_REG_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_FIX_PORT_REG_RSP))	/*fix port register response ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType  	= GPN_IFM_MSG_FIX_PORT_REG_RSP_ACK
			iSrcId	= IFM
			iDstId	= other modu
*/


#define GPN_IFM_MSG_FIX_PORT_UNREG			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_FIX_PORT_UNREG))	/*fix port unregister*/
/*  msg parameters direction: 
			iIndex     	= tx_Index++
			iMsgType  	= GPN_IFM_MSG_PORT_UNREG
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
                   	iMsgPara1  = devIndex
                   	iMsgPara2  = slot
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_FIX_PORT_UNREG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_FIX_PORT_UNREG))  /*fix port unregister ACK*/
/* msg parameters direction:
			iIndex	= CMD_Index
			iMsgType  	= GPN_IFM_MSG_PORT_UNREG_ACK
			iSrcId	= other modu
			iDstId	= IFM
*/

#define GPN_IFM_MSG_FIX_PORT_UNREG_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_FIX_PORT_UNREG))	/*fix port unregister response */
/* msg parameters direction:
			iIndex	   = tx_Index++
                   	iMsgType   = GPN_IFM_MSG_PORT_UNREG_RSP
                   	iSrcId     = other modu
                   	iDstId     = IFM
                   	iMsgPara1  = respons return value
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_FIX_PORT_UNREG_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_FIX_PORT_UNREG_RSP))   /*fix port unregister response ACK*/
/* msg parameters direction:
			iIndex	   = CMD_Index
			iMsgType      = GPN_IFM_MSG_PORT_UNREG_RSP_ACK
			iSrcId	   = IFM
			iDstId	   = other modu
*/

/*GPN_NOTIFY of GPN_IFM vari port notify format struct*/
typedef portUniqueIndex vpUniqueIndex;

#define GPN_IFM_MSG_VARI_PORT_REG			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_VARI_PORT_REG))	/*vari port register*/
/* msg parameters direction:
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_VARI_PORT_REG
                   	iSrcId     = IFM
                   	iDstId     = other modu
                   	iMsgPara1  = portIndex1
                   	iMsgPara2  = portIndex2
                   	iMsgPara3  = portIndex3
                   	iMsgPara4  = portIndex4
                   	iMsgPara5  = portIndex5
                   	iMsgPara6  = devIndex
                   	iMsgPara7  = register process pid 
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_VARI_PORT_REG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_VARI_PORT_REG))	/*vari port registerACK*/
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_VARI_PORT_REG_ACK
			iSrcId	  = other modu
			iDstId	  = IFM
*/

#define GPN_IFM_MSG_VARI_PORT_REG_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_VARI_PORT_REG))   	/*vari port register response*/
/* msg parameters direction:
			iIndex     	= tx_Index++
			iMsgType	= GPN_IFM_MSG_VARI_PORT_REG_RSP
                   	iSrcId     	= other modu
                   	iDstId     	= IFM
                   	iMsgPara1  = respons return value
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_VARI_PORT_REG_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_VARI_PORT_REG_RSP))	/*vari port register response ACK*/
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_VARI_PORT_REG_RSP_ACK
			iSrcId	  = other modu
			iDstId	  = IFM
*/

#define GPN_IFM_MSG_VARI_PORT_UNREG			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_VARI_PORT_UNREG))	/*vari port unregister*/
/*  msg parameters direction:
			iIndex     	= tx_Index++
			iMsgType   = GPN_IFM_MSG_VARI_PORT_UNREG
                   	iSrcId     	= IFM
                   	iDstId     	= other modu
                   	iMsgPara1  = portIndex1
                   	iMsgPara2  = portIndex2
                   	iMsgPara3  = portIndex3
                   	iMsgPara4  = portIndex4
                   	iMsgPara5  = portIndex5
                   	iMsgPara6  = devIndex
                   	iMsgPara7  = unregister process pid 
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_VARI_PORT_UNREG_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_VARI_PORT_UNREG))	/*vari port unregister ACK*/
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_VARI_PORT_UNREG_ACK
			iSrcId	  = other modu
			iDstId	  = IFM
*/

#define GPN_IFM_MSG_VARI_PORT_UNREG_RSP		((GPN_SOCK_MSG_RESPONS_BIT)|(GPN_IFM_MSG_VARI_PORT_UNREG))   	/*vari port unregister response*/
/* msg parameters direction:
			iIndex     	= tx_Index++
			iMsgType   = GPN_IFM_MSG_VARI_PORT_UNREG_RSP
                   	iSrcId     	= other modu
                   	iDstId     	= IFM
                   	iMsgPara1  = respons return value
                   	msgCellLen = 0
*/
#define GPN_IFM_MSG_VARI_PORT_UNREG_RSP_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_VARI_PORT_UNREG_RSP))	/*vari port unregister response ACK*/
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_VARI_PORT_UNREG_RSP_ACK
			iSrcId	  = IFM
			iDstId	  = other modu
*/

#define GPN_IFM_MSG_ROLE_DEBUG_CTRL			((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_ROLE_DEBUG_CTRL))	/*vari port unregister*/
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_ROLE_DEBUG_CTRL
                   	iSrcId     = other module
                   	iDstId     = SYSMGT(NMx)
                   	iMsgPara1  = debug level
                   	msgCellLen = 0
*/

#define GPN_IFM_MSG_ROLE_DEBUG_CTRL_ACK		((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_ROLE_DEBUG_CTRL))	/*vari port unregister ACK*/
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_ROLE_DEBUG_CTRL_ACK
			iSrcId	  = other modu
			iDstId	  = SYSMGT(NMx)
*/

#define GPN_IFM_MSG_PEER_PORT_REPLACE		((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_PEER_PORT_REPLACE))	/* peer port replace */
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_PEER_PORT_REPLACE
                   	iSrcId     = other module
                   	iDstId     = other module
                   	iMsgPara1  = old portIndex1
                   	iMsgPara2  = old portIndex2
                   	iMsgPara3  = old portIndex3
                   	iMsgPara4  = old portIndex4
                   	iMsgPara5  = old portIndex5
                   	iMsgPara6  = old devIndex
                   	iMsgPara7  = new portIndex1
                   	iMsgPara8  = new portIndex2
                   	iMsgPara9  = new portIndex3
                   	iMsgParaA  = new portIndex4
                   	iMsgParaB  = new portIndex5
                   	iMsgParaC  = new devIndex
                   	iMsgParaD  = auth pid
                   	msgCellLen = 0
*/

#define GPN_IFM_MSG_PEER_PORT_REPLACE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_PEER_PORT_REPLACE))	/* peer port replace ACK */
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_PEER_PORT_REPLACE_ACK
			iSrcId     = other module
                   	iDstId     = other module
*/

#define GPN_IFM_MSG_VIEW_PORT_REPLACE		((GPN_SOCK_OWNER_IFM)|(GPN_IMF_MT_IFC_STATUS)|(GPN_IMF_ST_VIEW_PORT_REPLACE))	/* view port replace */
/* msg parameters direction: 
			iIndex     = tx_Index++
			iMsgType   = GPN_IFM_MSG_VIEW_PORT_REPLACE
                   	iSrcId     = other module
                   	iDstId     = other module
                   	iMsgPara1  = old portIndex1
                   	iMsgPara2  = old portIndex2
                   	iMsgPara3  = old portIndex3
                   	iMsgPara4  = old portIndex4
                   	iMsgPara5  = old portIndex5
                   	iMsgPara6  = old devIndex
                   	iMsgPara7  = new portIndex1
                   	iMsgPara8  = new portIndex2
                   	iMsgPara9  = new portIndex3
                   	iMsgParaA  = new portIndex4
                   	iMsgParaB  = new portIndex5
                   	iMsgParaC  = new devIndex
                   	iMsgParaD  = auth pid
                   	msgCellLen = 0
*/

#define GPN_IFM_MSG_VIEW_PORT_REPLACE_ACK	((GPN_SOCK_MSG_ACK_BIT)|(GPN_IFM_MSG_VIEW_PORT_REPLACE))	/* view port replace ACK */
/* msg parameters direction:
			iIndex	  = CMD_Index
			iMsgType	  = GPN_IFM_MSG_VIEW_PORT_REPLACE_ACK
			iSrcId     = other module
                   	iDstId     = other module
*/

UINT32 gpnSockIfmMsgTxDevStateNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 devIndex, UINT32 sta);
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len);
UINT32 gpnSockIfmMsgTxVariPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 vPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, objLogicDesc *pPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify(stSockFdSet *pstFdSet, UINT32 commRole, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt);
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify(stSockFdSet *pstFdSet,  UINT32 commRole,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt);
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify(optObjOrient *pOldPeer, optObjOrient *pNewPeer, UINT32 MODU_ID);
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify(objLogicDesc *pOldView, objLogicDesc *pNewView, UINT32 MODU_ID);

/*define for x-modu notify index chang(creat or deletle) to IFM modu */
UINT32 gpnSockIfmMsgTxDevStateNotify2Ifm(UINT32 devIndex, UINT32 sta);
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Ifm(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len);
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Ifm(UINT32 vPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Ifm(objLogicDesc *pPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Ifm(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt);
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Ifm(stSockFdSet *pstFdSet,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt);
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Ifm(optObjOrient *pOldPeer, optObjOrient *pNewPeer);
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Ifm(objLogicDesc *pOldView, objLogicDesc *pNewView);
UINT32 gpnSockIfmMsgTxZebosL2NewIfNotify2Ifm(objLogicDesc *pl2If, objLogicDesc *ppeerIf);

/*define for notify status to ALM*/
UINT32 gpnSockIfmMsgTxDevStateNotify2Alm(UINT32 devIndex, UINT32 sta);
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Alm(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len);
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Alm(stSockFdSet *pstFdSet, UINT32 vPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Alm(objLogicDesc *pPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Alm(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt);
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Alm(stSockFdSet *pstFdSet,
	UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt);
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Alm(optObjOrient *pOldPeer, optObjOrient *pNewPeer);
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Alm(objLogicDesc *pOldView, objLogicDesc *pNewView);


/*define for notify status to STAT*/
UINT32 gpnSockIfmMsgTxDevStateNotify2Stat(UINT32 devIndex, UINT32 sta);
UINT32 gpnSockIfmMsgTxFixPortCapabilityNotify2Stat(UINT32 devIndex, stFixPorcReg *pfixPortSet, UINT32 len);
UINT32 gpnSockIfmMsgTxVariPortCreatNotify2Stat(stSockFdSet *pstFdSet, UINT32 vPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxVariMultPortCreatNotify2Stat(objLogicDesc *pPortIndex, UINT32 opt);
UINT32 gpnSockIfmMsgTxCfmMEPPortCreatNotify2Stat(stSockFdSet *pstFdSet, UINT32 ethPortId,
	UINT32 vlanId, UINT32 MDId, UINT32 MAId, UINT32 MEPId, UINT32 RemMEPId, UINT32 opt);
UINT32 gpnSockIfmMsgTxPTNPortCreatNotify2Stat(UINT32 vc/*pw*/, UINT32 vp/*lsp*/, UINT32 vs, UINT32 opt);
UINT32 gpnSockIfmMsgTxPeerPortReplaceNotify2Stat(optObjOrient *pOldPeer, optObjOrient *pNewPeer);
UINT32 gpnSockIfmMsgTxViewPortReplaceNotify2Stat(objLogicDesc *pOldView, objLogicDesc *pNewView);

/*define for every module debug ctrl */
UINT32 gpnSockIfmMsgTxRoleDebugCtrl2OtherModule(stSockFdSet *pstFdSet, UINT32 CPUId, UINT32 MODU_ID, UINT32 debugCtrl);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SOCK_IFM_MSG_DEF_H_*/

