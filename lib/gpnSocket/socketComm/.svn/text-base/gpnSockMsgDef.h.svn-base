/**********************************************************
* file name: gpnSockMsgDef.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-19
* function: 
*    define details about communication between modules
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_MSG_DEF_H_
#define _GPN_SOCK_MSG_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#include "lib/gpnSocket/socketComm/gpnSockCommModuDef.h"

#include "lib/gpnDevInfo/devCoreInfo/gpnDevStatus.h"

/*=============================================================*/
/*=============================================================*/
/*CMD msg head*/
typedef struct _gpnSockCmdMsg_
{
	UINT32     iIndex;							/*CMD编号*/
	UINT32 	   iMsgCtrl;						/*消息处理控制信息*/
	UINT32     iDstId;                         	/*目的模块id */
	UINT32     iSrcId;                         	/*源模块id */
	UINT32     iMsgType;                       	/*消息类型 */
    UINT32     iMsgPara1;                      	/*参数 1*/
    UINT32     iMsgPara2;                      	/*参数 2*/
    UINT32     iMsgPara3;                      	/*参数 3*/
    UINT32     iMsgPara4;                      	/*参数 4*/
    UINT32     iMsgPara5;                      	/*参数 5*/
    UINT32     iMsgPara6;                      	/*参数 6*/
	UINT32     iMsgPara7;                      	/*参数 7*/
    UINT32     iMsgPara8;                      	/*参数 8*/
    UINT32     iMsgPara9;                      	/*参数 9*/
	UINT32     iMsgParaA;                      	/*参数 A*/
	UINT32     iMsgParaB;                      	/*参数 B*/
	UINT32     iMsgParaC;                      	/*参数 C*/
	UINT32     iMsgParaD;                      	/*参数 D*/
	UINT32     iMsgParaE;                      	/*参数 C*/
	UINT32     iMsgParaF;                      	/*参数 D*/
    UINT32	   msgCellLen;                    	/*消息信元 */
}gpnSockMsg;

#define GPN_SOCK_MSG_SUFFIX_INDEX	0
#define GPN_SOCK_MSG_SUFFIX_CTRL	1
#define GPN_SOCK_MSG_SUFFIX_DST 	2
#define GPN_SOCK_MSG_SUFFIX_SRC 	3
#define GPN_SOCK_MSG_SUFFIX_CMD 	4
#define GPN_SOCK_MSG_SUFFIX_P1 		5
#define GPN_SOCK_MSG_SUFFIX_P2 		6
#define GPN_SOCK_MSG_SUFFIX_P3 		7
#define GPN_SOCK_MSG_SUFFIX_P4 		8
#define GPN_SOCK_MSG_SUFFIX_P5 		9
#define GPN_SOCK_MSG_SUFFIX_P6 		10
#define GPN_SOCK_MSG_SUFFIX_P7 		11
#define GPN_SOCK_MSG_SUFFIX_P8 		12
#define GPN_SOCK_MSG_SUFFIX_P9 		13
#define GPN_SOCK_MSG_SUFFIX_PA 		14
#define GPN_SOCK_MSG_SUFFIX_PB 		15
#define GPN_SOCK_MSG_SUFFIX_PC 		16
#define GPN_SOCK_MSG_SUFFIX_PD 		17
#define GPN_SOCK_MSG_SUFFIX_PE 		18
#define GPN_SOCK_MSG_SUFFIX_PF 		19
#define GPN_SOCK_MSG_SUFFIX_PL 		20
#define	GPN_SOCK_MSG_HEAD_LEN  		21

#define GPN_SOCK_MSG_HEAD_BLEN		(GPN_SOCK_MSG_HEAD_LEN*4)
#define GPN_SOCK_MSG_HEAD_PAL_BLEN	(0x0000000F*4)


/*msg paload*/
/*UINT8 payload[msgCellLen];*/

/*msg struct*/
/*|--------------------------------------------------------------------------------------------------------------------------|*/
/*|Index|Ctrl|DesId|SrcId|Type|Para1|Para2|Para3|Para4|Para5|Para6|Para7|Para8|Para9|ParaA|ParaB|ParaC|ParaD|ParaE|ParaF|PLen|    Payload   |*/
/*|--------------------------------------------------------------------------------------------------------------------------|*/
/*|            over head                 |                                                           message head text                                                                      |msg payload|*/
/*|--------------------------------------------------------------------------------------------------------------------------|*/

/*=============================================================================================*/
/*=============================================================================================*/
typedef struct _gpnSockAckMsg_
{
	UINT32     iIndex;                        	/*消息通信序号 */
	UINT32     iMsgCtrl;                       	/*消息处理控制信息 */
	UINT32     iDstId;                         	/*目的模块id */
	UINT32     iSrcId;                          /*源模块id */
	UINT32     iMsgType;                        /*消息类型 */
}gpnSockAckMsg;

#define GPN_SOCK_ACK_SUFFIX_INDEX		0
#define GPN_SOCK_ACK_SUFFIX_CTRL 		1
#define GPN_SOCK_ACK_SUFFIX_DST 		2
#define GPN_SOCK_ACK_SUFFIX_SRC 		3
#define GPN_SOCK_ACK_SUFFIX_CMD 		4
#define GPN_SOCK_ACK_LEN		 		5
#define GPN_SOCK_ACK_BLEN		 		GPN_SOCK_ACK_LEN*4

/*============================================================================*/
/*============================================================================*/
/*******************************************************************************************************/
/*|bit |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|21|00|*/
/*|                                          RSV                                           |               CPU ID                |X  |          MODU ID             |*/
/*******************************************************************************************************/
/*source & destination define*/
#define GPN_SOCK_DIREC_RSV_BITS			0xFFFF0000
#define GPN_SOCK_DIREC_RSV_MASK			0x0000FFFF
#define GPN_SOCK_DIREC_RSV_SHIFT		16

#define GPN_SOCK_CPUID_BITS 			0x0000FF00
#define GPN_SOCK_CPUID_MASK 			0x000000FF
#define GPN_SOCK_CPUID_SHIFT 			8

#define GPN_SOCK_MODUID_BITS 			0x0000007F
#define GPN_SOCK_MODUID_MASK 			0x0000007F
#define GPN_SOCK_MODUID_SHIFT 			0

#define GPN_SOCK_DIREC_BITS 			0x0000FF7F
#define GPN_SOCK_DIREC_MASK 			0x0000FF7F
#define GPN_SOCK_DIREC_SHIFT 			0

#define GPN_SOCK_MSG_MT_CREAT(index)	(((index) << GPN_SOCK_MSG_MAIN_TYPE_SHIFT) & GPN_SOCK_MSG_MAIN_TYPE_BIT)
#define GPN_SOCK_MSG_ST_CREAT(index)	(((index) << GPN_SOCK_MSG_SUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUB_TYPE_BIT)
#define GPN_SOCK_MSG_MST_CREAT(index)	(((index) << GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT) & GPN_SOCK_MSG_MAINSUB_TYPE_BIT)
#define GPN_SOCK_MSG_SST_CREAT(index)	(((index) << GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT) & GPN_SOCK_MSG_SUBSUB_TYPE_BIT)

/*msg direction creat*/
#define GPN_SOCK_CREAT_DIREC(cpuId, moduId)	((((cpuId)&GPN_SOCK_CPUID_MASK)<<GPN_SOCK_CPUID_SHIFT) | \
											 (((moduId)&GPN_SOCK_MODUID_MASK)<<GPN_SOCK_MODUID_SHIFT))
#define GPN_SOCK_DECOM_CPUID(direc)			(((direc)>>GPN_SOCK_CPUID_SHIFT)&GPN_SOCK_CPUID_MASK)
#define GPN_SOCK_DECOM_MODUID(direc)		(((direc)>>GPN_SOCK_MODUID_SHIFT)&GPN_SOCK_MODUID_MASK)	

/*============================================================================*/


/*******************************************************************************************************/
/*|bit |31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|09|08|07|06|05|04|03|02|21|00|*/
/*| msg ctrl   |          msg owern          |       msg main type        |                              msg sub type                                    |*/
/*|                |                                   |                                    |           msg main sub type    |        msg sub sub type         |*/
/*******************************************************************************************************/
/*gpn_socket comm messeg module : use 7 bits, 0~0x7F */
#define GPN_SOCK_MSG_M_PROTOCOL_SELF    (0)
#define GPN_SOCK_MSG_M_TRANSFER         (GPN_COMMM_TRANSFER+1)
#define GPN_SOCK_MSG_M_NMSYN            (GPN_COMMM_NM_SYN+1)
#define GPN_SOCK_MSG_M_SHELL            (GPN_COMMM_TERM_SHELL+1)
#define GPN_SOCK_MSG_M_ALM              (GPN_COMMM_ALM+1)
#define GPN_SOCK_MSG_M_IFM              (GPN_COMMM_IFM+1)
#define GPN_SOCK_MSG_M_SDM              (GPN_COMMM_SDM+1)
#define GPN_SOCK_MSG_M_NSM              (GPN_COMMM_NSM+1)
#define GPN_SOCK_MSG_M_ONMD             (GPN_COMMM_ONMD+1)
#define GPN_SOCK_MSG_M_LAG              (GPN_COMMM_LAG+1)
#define GPN_SOCK_MSG_M_PWE              (GPN_COMMM_PWE+1)
#define GPN_SOCK_MSG_M_INIT             (GPN_COMMM_INIT+1)
#define GPN_SOCK_MSG_M_STAT             (GPN_COMMM_STAT+1)
#define GPN_SOCK_MSG_M_CFGMGT           (GPN_COMMM_CFGMGT+1)
#define GPN_SOCK_MSG_M_SYSMGT           (GPN_COMMM_SYSMGT+1)
#define GPN_SOCK_MSG_M_FT               (GPN_COMMM_FT+1)
#define GPN_SOCK_MSG_M_OAMD             (GPN_COMMM_OAMD+1)
#define GPN_SOCK_MSG_M_SYNCED           (GPN_COMMM_SYNCED+1)
#define GPN_SOCK_MSG_M_COPROC           (GPN_COMMM_COPROC+1)
#define GPN_SOCK_MSG_M_XLOAD            (GPN_COMMM_XLOAD+1)
#define GPN_SOCK_MSG_M_IMI              (GPN_COMMM_IMI+1)

/***************** lipf add for ipran ******************************/
#define GPN_SOCK_MSG_M_MPLS             (GPN_COMMM_MPLS+1)
#define GPN_SOCK_MSG_M_CLOCK            (GPN_COMMM_CLOCK+1)
#define GPN_SOCK_MSG_M_SYSTEM           (GPN_COMMM_SYSTEM+1)
#define GPN_SOCK_MSG_M_DEVM             (GPN_COMMM_DEVM+1)
#define GPN_SOCK_MSG_M_HAL              (GPN_COMMM_HAL+1)
#define GPN_SOCK_MSG_M_L2               (GPN_COMMM_L2+1)
#define GPN_SOCK_MSG_M_CES              (GPN_COMMM_CES+1)
#define GPN_SOCK_MSG_M_VTYSH            (GPN_COMMM_VTYSH+1)
#define GPN_SOCK_MSG_M_FTM             	(GPN_COMMM_FTM+1)

/************************ end **************************************/

#if 0
#define GPN_SOCK_MSG_M_RESERVED04       (GPN_COMMM_RESERVED04+1)
#define GPN_SOCK_MSG_M_RESERVED05       (GPN_COMMM_RESERVED05+1)
#define GPN_SOCK_MSG_M_RESERVED06       (GPN_COMMM_RESERVED06+1)
#define GPN_SOCK_MSG_M_RESERVED07       (GPN_COMMM_RESERVED07+1)
#define GPN_SOCK_MSG_M_RESERVED08       (GPN_COMMM_RESERVED08+1)
#define GPN_SOCK_MSG_M_RESERVED09       (GPN_COMMM_RESERVED09+1)
#define GPN_SOCK_MSG_M_RESERVED10       (GPN_COMMM_RESERVED10+1)
#define GPN_SOCK_MSG_M_RESERVED11       (GPN_COMMM_RESERVED11+1)
#define GPN_SOCK_MSG_M_RESERVED12       (GPN_COMMM_RESERVED12+1)
#endif
#define GPN_SOCK_MSG_M_RESERVED13       (GPN_COMMM_RESERVED13+1)
#define GPN_SOCK_MSG_M_RESERVED14       (GPN_COMMM_RESERVED14+1)
#define GPN_SOCK_MSG_M_RESERVED15       (GPN_COMMM_RESERVED15+1)
#define GPN_SOCK_MSG_M_RESERVED16       (GPN_COMMM_RESERVED16+1)
#define GPN_SOCK_MSG_M_RESERVED17       (GPN_COMMM_RESERVED17+1)
#define GPN_SOCK_MSG_M_RESERVED18       (GPN_COMMM_RESERVED18+1)
#define GPN_SOCK_MSG_M_RESERVED19       (GPN_COMMM_RESERVED19+1)
#define GPN_SOCK_MSG_M_RESERVED20       (GPN_COMMM_RESERVED20+1)
#define GPN_SOCK_MSG_M_RESERVED21       (GPN_COMMM_RESERVED21+1)
#define GPN_SOCK_MSG_M_RESERVED22       (GPN_COMMM_RESERVED22+1)
#define GPN_SOCK_MSG_M_RESERVED23       (GPN_COMMM_RESERVED23+1)
#define GPN_SOCK_MSG_M_RESERVED24       (GPN_COMMM_RESERVED24+1)
#define GPN_SOCK_MSG_M_RESERVED25       (GPN_COMMM_RESERVED25+1)
#define GPN_SOCK_MSG_M_RESERVED26       (GPN_COMMM_RESERVED26+1)
#define GPN_SOCK_MSG_M_RESERVED27       (GPN_COMMM_RESERVED27+1)
#define GPN_SOCK_MSG_M_RESERVED28       (GPN_COMMM_RESERVED28+1)
#define GPN_SOCK_MSG_M_RESERVED29       (GPN_COMMM_RESERVED29+1)
#define GPN_SOCK_MSG_M_RESERVED30       (GPN_COMMM_RESERVED30+1)
#define GPN_SOCK_MSG_M_RESERVED31       (GPN_COMMM_RESERVED31+1)
#define GPN_SOCK_MSG_M_RESERVED32       (GPN_COMMM_RESERVED32+1)
#define GPN_SOCK_MSG_M_RESERVED33       (GPN_COMMM_RESERVED33+1)
#define GPN_SOCK_MSG_M_RESERVED34       (GPN_COMMM_RESERVED34+1)
#define GPN_SOCK_MSG_M_RESERVED35       (GPN_COMMM_RESERVED35+1)
#define GPN_SOCK_MSG_M_RESERVED36       (GPN_COMMM_RESERVED36+1)
#define GPN_SOCK_MSG_M_RESERVED37       (GPN_COMMM_RESERVED37+1)
#define GPN_SOCK_MSG_M_RESERVED38       (GPN_COMMM_RESERVED38+1)
#define GPN_SOCK_MSG_M_RESERVED39       (GPN_COMMM_RESERVED39+1)
#define GPN_SOCK_MSG_M_RESERVED40       (GPN_COMMM_RESERVED40+1)
#define GPN_SOCK_MSG_M_RESERVED41       (GPN_COMMM_RESERVED41+1)
#define GPN_SOCK_MSG_M_RESERVED42       (GPN_COMMM_RESERVED42+1)
#define GPN_SOCK_MSG_M_RESERVED43       (GPN_COMMM_RESERVED43+1)
#define GPN_SOCK_MSG_M_RESERVED44       (GPN_COMMM_RESERVED44+1)
#define GPN_SOCK_MSG_M_RESERVED45       (GPN_COMMM_RESERVED45+1)
#define GPN_SOCK_MSG_M_RESERVED46       (GPN_COMMM_RESERVED46+1)
#define GPN_SOCK_MSG_M_RESERVED47       (GPN_COMMM_RESERVED47+1)

#define GPN_SOCK_MSG_M_TIMER_SEV        (GPN_COMMM_TIMER_SEV+1)
/* gpn_socket real communication role end */

/* do not use for gpn_sock_msg define */
/*#define GPN_SOCK_MSG_M_MN_TO_SUB1T             XXXXXX */
/*#define GPN_SOCK_MSG_M_MN_TO_SUB1F             XXXXXX */
/*#define GPN_SOCK_MSG_SYN_TO_NMA                XXXXXX */
#define GPN_SOCK_MODU_MAX               0x7F

/*=============================================================*/
/*=============================================================*/

/*CPU ID define*/
/*see in commRole*/

/*other CPU ID define*/

#define GPN_SOCK_CPU_ID_MAX             0xFF

/*=============================================================*/
/*=============================================================*/
/*Msg Type struct define*/

/*msgsub type part1: is ACK or CMD, bit31*/
/*CMD & ACK*/
/*CMD:0x*********/
/*ACK:CMD|0x8000000*/
#define GPN_SOCK_MSG_ACK_BIT            0x80000000
#define GPN_SOCK_MSG_ACK_MASK           0x7FFFFFFF

/*msgsub type part2: is Request or Respons, bit30*/
/*request & respons*/
/*request:0x*********/
/*respons:request|0x4000000*/
#define GPN_SOCK_MSG_RESPONS_BIT        0x40000000
#define GPN_SOCK_MSG_RESPONS_MASK       0xBFFFFFFF

/*msg sub type part3 -- who define this msg(msg owner), bit29~bit23*/
#define GPN_SOCK_MSG_OWNER_BIT          0x3F800000
#define GPN_SOCK_MSG_OWNER_MASK         0xC07FFFFF

#define GPN_SOCK_MSG_OWNER_SHIFT        23

#define GPN_SOCK_OWNER_SELF             ((GPN_SOCK_MSG_M_PROTOCOL_SELF << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_TRANSFER         ((GPN_SOCK_MSG_M_TRANSFER << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_NMSYN            ((GPN_SOCK_MSG_M_NMSYN << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_SHELL            ((GPN_SOCK_MSG_M_SHELL << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_ALM              ((GPN_SOCK_MSG_M_ALM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_IFM              ((GPN_SOCK_MSG_M_IFM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_SDM              ((GPN_SOCK_MSG_M_SDM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_NSM              ((GPN_SOCK_MSG_M_NSM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_ONMD             ((GPN_SOCK_MSG_M_ONMD << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_LAG              ((GPN_SOCK_MSG_M_LAG << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_PWE              ((GPN_SOCK_MSG_M_PWE << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_INIT             ((GPN_SOCK_MSG_M_INIT << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_STAT             ((GPN_SOCK_MSG_M_STAT << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_CFGMGT           ((GPN_SOCK_MSG_M_CFGMGT << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_SYSMGT           ((GPN_SOCK_MSG_M_SYSMGT << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_FT               ((GPN_SOCK_MSG_M_FT << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_SYNCED           ((GPN_SOCK_MSG_M_SYNCED << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_COPROC           ((GPN_SOCK_MSG_M_COPROC << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_XLOAD            ((GPN_SOCK_MSG_M_XLOAD << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_IMI              ((GPN_SOCK_MSG_M_IMI << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)

/***************** lipf add for ipran ******************************/
#define GPN_SOCK_OWNER_MPLS             ((GPN_SOCK_MSG_M_MPLS << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_CLOCK            ((GPN_SOCK_MSG_M_CLOCK << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_SYSTEM           ((GPN_SOCK_MSG_M_SYSTEM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_DEVM             ((GPN_SOCK_MSG_M_DEVM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_HAL              ((GPN_SOCK_MSG_M_HAL << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_L2               ((GPN_SOCK_MSG_M_L2 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_CES              ((GPN_SOCK_MSG_M_CES << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_VTYSH            ((GPN_SOCK_MSG_M_VTYSH << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_FTM              ((GPN_SOCK_MSG_M_FTM << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)

/************************ end **************************************/

#if 0
#define GPN_SOCK_OWNER_RESERVED04       ((GPN_SOCK_MSG_M_RESERVED04 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED05       ((GPN_SOCK_MSG_M_RESERVED05 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED06       ((GPN_SOCK_MSG_M_RESERVED06 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED07       ((GPN_SOCK_MSG_M_RESERVED07 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED08       ((GPN_SOCK_MSG_M_RESERVED08 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED09       ((GPN_SOCK_MSG_M_RESERVED09 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED10       ((GPN_SOCK_MSG_M_RESERVED10 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED11       ((GPN_SOCK_MSG_M_RESERVED11 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED12       ((GPN_SOCK_MSG_M_RESERVED12 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#endif
#define GPN_SOCK_OWNER_RESERVED13       ((GPN_SOCK_MSG_M_RESERVED13 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED14       ((GPN_SOCK_MSG_M_RESERVED14 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED15       ((GPN_SOCK_MSG_M_RESERVED15 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED16       ((GPN_SOCK_MSG_M_RESERVED16 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED17       ((GPN_SOCK_MSG_M_RESERVED17 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED18       ((GPN_SOCK_MSG_M_RESERVED18 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED19       ((GPN_SOCK_MSG_M_RESERVED19 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED20       ((GPN_SOCK_MSG_M_RESERVED20 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED21       ((GPN_SOCK_MSG_M_RESERVED21 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED22       ((GPN_SOCK_MSG_M_RESERVED22 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED23       ((GPN_SOCK_MSG_M_RESERVED23 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED24       ((GPN_SOCK_MSG_M_RESERVED24 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED25       ((GPN_SOCK_MSG_M_RESERVED25 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED26       ((GPN_SOCK_MSG_M_RESERVED26 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED27       ((GPN_SOCK_MSG_M_RESERVED27 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED28       ((GPN_SOCK_MSG_M_RESERVED28 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED29       ((GPN_SOCK_MSG_M_RESERVED29 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED30       ((GPN_SOCK_MSG_M_RESERVED30 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED31       ((GPN_SOCK_MSG_M_RESERVED31 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED32       ((GPN_SOCK_MSG_M_RESERVED32 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED33       ((GPN_SOCK_MSG_M_RESERVED33 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED34       ((GPN_SOCK_MSG_M_RESERVED34 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED35       ((GPN_SOCK_MSG_M_RESERVED35 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED36       ((GPN_SOCK_MSG_M_RESERVED36 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED37       ((GPN_SOCK_MSG_M_RESERVED37 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED38       ((GPN_SOCK_MSG_M_RESERVED38 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED39       ((GPN_SOCK_MSG_M_RESERVED39 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED40       ((GPN_SOCK_MSG_M_RESERVED40 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED41       ((GPN_SOCK_MSG_M_RESERVED41 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED42       ((GPN_SOCK_MSG_M_RESERVED42 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED43       ((GPN_SOCK_MSG_M_RESERVED43 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED44       ((GPN_SOCK_MSG_M_RESERVED44 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED45       ((GPN_SOCK_MSG_M_RESERVED45 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED46       ((GPN_SOCK_MSG_M_RESERVED46 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)
#define GPN_SOCK_OWNER_RESERVED47       ((GPN_SOCK_MSG_M_RESERVED47 << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)

#define GPN_SOCK_OWNER_TIMER			((GPN_SOCK_MSG_M_TIMER_SEV << GPN_SOCK_MSG_OWNER_SHIFT) & GPN_SOCK_MSG_OWNER_BIT)


/*msg sub type part4 -- main msg type, bit22~bit16*/
#define GPN_SOCK_MSG_MAIN_TYPE_BIT		0x007F0000
#define GPN_SOCK_MSG_MAIN_TYPE_MASK		0xFF80FFFF

#define GPN_SOCK_MSG_MAIN_TYPE_SHIFT    16


/*msg sub type part5 -- sub msg type, bit15~bit10*/
#define GPN_SOCK_MSG_SUB_TYPE_BIT		0x0000FFFF
#define GPN_SOCK_MSG_SUB_TYPE_MASK		0xFFFF0000

#define GPN_SOCK_MSG_SUB_TYPE_SHIFT     0

/*msg sub type part5(1) -- sub(main) msg type, bit15~bit8*/
#define GPN_SOCK_MSG_MAINSUB_TYPE_BIT	0x0000FF00
#define GPN_SOCK_MSG_MAINSUB_TYPE_MASK	0xFFFF00FF

#define GPN_SOCK_MSG_MAINSUB_TYPE_SHIFT 8

/*msg sub type part5(2) -- sub(sub) msg type, bit7~bit10*/
#define GPN_SOCK_MSG_SUBSUB_TYPE_BIT	0x000000FF
#define GPN_SOCK_MSG_SUBSUB_TYPE_MASK	0xFFFFFF00

#define GPN_SOCK_MSG_SUBSUB_TYPE_SHIFT  0

/*=============================================================*/
/*=============================================================*/
/*respons return value define*/
#define  GPN_SOCK_MSG_RSPONSE_OK      	0x00000001   /*normal, ok */
#define  GPN_SOCK_MSG_RSPONSE_PARA_ERR  0x00000002   /*Request para err */
#define  GPN_SOCK_MSG_RSPONSE_NOT_SUP   0x00000003   /*Requset do not support */
#define  GPN_SOCK_MSG_RSPONSE_AGAIN     0x00000004   /*Request send twice */
#define  GPN_SOCK_MSG_RSPONSE_NO_DEF    0x00000005   /*Requset do not define */

/*enable & disable*/
#define GPN_SOCK_MSG_OPT_ENABLE			1
#define GPN_SOCK_MSG_OPT_DISABLE		2
/*creat & delete*/
#define GPN_SOCK_MSG_OPT_CREAT			1
#define GPN_SOCK_MSG_OPT_DELETE			2
/*rise & clean*/
#define GPN_SOCK_MSG_OPT_RISE			1
#define GPN_SOCK_MSG_OPT_CLEAN			2
/*device state*/
#define GPN_SOCK_MSG_DEV_STA_NULL 		GPN_DEV_STA_NULL
#define GPN_SOCK_MSG_DEV_STA_PULL		GPN_DEV_STA_PULL
#define GPN_SOCK_MSG_DEV_STA_INSERT		GPN_DEV_STA_INSERT
#define GPN_SOCK_MSG_DEV_STA_RUN		GPN_DEV_STA_RUN
#define GPN_SOCK_MSG_DEV_STA_ERR		GPN_DEV_STA_ERR
#define GPN_SOCK_MSG_DEV_STA_FAKE		GPN_DEV_STA_FAKE

/*invalide port index*/
#define GPN_GEN_MSG_INVALID_PORT		0xFFFFFFFF
#define GPN_GEN_MSG_INVALID_DEV			0xFFFFFFFF

/*invalide 32bit file*/
#define GPN_GEN_MSG_INVALID_32BIT_FILE	0xeeeeeeed

/*portIndex*/
/*详见专门定义*/


/*****************************************************************************************************************/
/*msg control part 									     												            */
/*****************************************************************************************************************/
/*is need resend or rereceive*/
/*CMD:0x********|0x80000000*/
#define GPN_SOCK_MSG_RE_TX_RX_BIT		0x80000000
#define GPN_SOCK_MSG_RE_TX_RX_MASK		0x7FFFFFFF
/*is need tx ACK*/
/*CMD:0x********|0x40000000*/
#define GPN_SOCK_MSG_ACK_NEED_BIT		0x40000000
#define GPN_SOCK_MSG_ACK_NEED_MASK		0xBFFFFFFF



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_MSG_DEF_H_*/

