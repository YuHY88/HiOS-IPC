/**********************************************************
* file name: gpnGlobalDevTypeDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-09
* function: 
*    define GPN_DEV_INFO Global type
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_DEV_TYPE_DEF_H_
#define _GPN_GLOBAL_DEV_TYPE_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#define GPN_DEV_TYPE_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_DEV_TYPE_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_DEV_TYPE_GEN_YES					GPN_SOCK_SYS_YES
#define GPN_DEV_TYPE_GEN_NO 					GPN_SOCK_SYS_NO

#define GPN_DEV_TYPE_PRINT(level, info...) 		GEN_SOCK_SYS_PRINT((level), info)
#define GPN_DEV_TYPE_AGP 						GEN_SOCK_SYS_AGP
#define GPN_DEV_TYPE_SVP 						GEN_SOCK_SYS_SVP
#define GPN_DEV_TYPE_CMP 						GEN_SOCK_SYS_CMP
#define GPN_DEV_TYPE_CUP 						GEN_SOCK_SYS_CUP

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++           SLOT TYPE DEFINE                 ++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GPN_SOCK_DEV_SLOT_ALL 	0xFFFFFFFF
#define GPN_SOCK_DEV_SLOT_NULL	0xeeeeeeed

#define GPN_SOCK_DEV_SLOT_00	0
#define GPN_SOCK_DEV_SLOT_01	1
#define GPN_SOCK_DEV_SLOT_02	2
#define GPN_SOCK_DEV_SLOT_03	3
#define GPN_SOCK_DEV_SLOT_04	4
#define GPN_SOCK_DEV_SLOT_05	5
#define GPN_SOCK_DEV_SLOT_06	6
#define GPN_SOCK_DEV_SLOT_07	7
#define GPN_SOCK_DEV_SLOT_08	8
#define GPN_SOCK_DEV_SLOT_09	9
#define GPN_SOCK_DEV_SLOT_10	10
#define GPN_SOCK_DEV_SLOT_11	11
#define GPN_SOCK_DEV_SLOT_12	12
#define GPN_SOCK_DEV_SLOT_13	13
#define GPN_SOCK_DEV_SLOT_14	14
#define GPN_SOCK_DEV_SLOT_15	15
#define GPN_SOCK_DEV_SLOT_16	16
#define GPN_SOCK_DEV_SLOT_17	17
#define GPN_SOCK_DEV_SLOT_18	18
#define GPN_SOCK_DEV_SLOT_19	19
#define GPN_SOCK_DEV_SLOT_20	20

#define GPN_SOCK_DEV_NMA_SLOT  	GPN_SOCK_DEV_SLOT_17
#define GPN_SOCK_DEV_NMB_SLOT  	GPN_SOCK_DEV_SLOT_20
#define GPN_SOCK_DEV_MAX_SLOT  	GPN_SOCK_DEV_SLOT_20

#define GPN_SOCK_DEV_NM_NUM		2
#define GPN_SOCK_DEV_NMA_INDEX  1
#define GPN_SOCK_DEV_NMB_INDEX  2


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++           DEVICE TYPE DEFINE                 ++++++++++++++++++*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define GPN_DEV_TYPE_NULL                           0/*0*/		

#define GPN_DEV_TYPE_HT201							1
#define GPN_DEV_TYPE_HT201E							2
#define GPN_DEV_TYPE_H20RN2000						3
#define GPN_DEV_TYPE_HT157							4
#define GPN_DEV_TYPE_HT158							5
#define GPN_DEV_TYPE_VX								6


#define GPN_DEV_TYPE_ALL                            (GPN_DEV_TYPE_VX + 1)

#define GPN_DEV_TYPE_MAX                            (0xFFF)/*just (0xFFE+0x1 4095)*/

#define GPN_EQU_TYPE_MAX                            GPN_DEV_TYPE_VX


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++           DEVICE TYPE DEFINE END             ++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GPN_DEV_TYPE_MAX_TYPE_STR_LEN			64

typedef struct _stGpnDevTypeTp_
{
	/*device type infomation, global define */
	UINT32 devType;
	const char *devStrType;
}stGpnDevTypeTp;

UINT32 gpnGlobalDevTpNum(void);
UINT32 gpnGlobalDevStrType2DevType(char *devStrType, UINT32 *devType);
UINT32 gpnGlobalDevType2DevStrType(UINT32 devType, char *devStrType, UINT32 lenth);
char *gpnGlobalDevStrGet(UINT32 devType);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_GLOBAL_DEV_TYPE_DEF_H_ */


