/**********************************************************
* file name: gpnDevCoreInfo.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-08
* function: 
*    define GPN_DEV_INFO comm API
* modify:
*
***********************************************************/
#ifndef _GPN_DEV_CORE_INFO_H_
#define _GPN_DEV_CORE_INFO_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnUnifyPathDef.h"
#include "lib/gpnSocket/socketComm/gpnCommList.h"
#include "lib/gpnSocket/socketComm/gpnDebugFuncApi.h"

#include "lib/gpnDevInfo/devCoreInfo/gpnGlobalDevTypeDef.h"

#define GPN_DEV_GEN_OK                      GPN_SOCK_SYS_OK
#define GPN_DEV_GEN_ERR                     GPN_SOCK_SYS_ERR

#define GPN_DEV_GEN_YES                     GPN_SOCK_SYS_YES
#define GPN_DEV_GEN_NO                      GPN_SOCK_SYS_NO


#define GPN_DEV_CI_PRINT(level, info...)    GPN_DEBUG_FUNC_API_PRINT((level), info)
#define GPN_DEV_CI_EGP                      GPN_DEBUG_FUNC_API_EGP
#define GPN_DEV_CI_SVP                      GPN_DEBUG_FUNC_API_SVP
#define GPN_DEV_CI_CMP                      GPN_DEBUG_FUNC_API_CMP
#define GPN_DEV_CI_CUP                      GPN_DEBUG_FUNC_API_CUP
#define GPN_DEV_CI_UD5                      GPN_DEBUG_FUNC_API_UD5
#define GPN_DEV_CI_UD4                      GPN_DEBUG_FUNC_API_UD4
#define GPN_DEV_CI_UD3                      GPN_DEBUG_FUNC_API_UD3
#define GPN_DEV_CI_UD2                      GPN_DEBUG_FUNC_API_UD2
#define GPN_DEV_CI_UD1                      GPN_DEBUG_FUNC_API_UD1
#define GPN_DEV_CI_UD0                      GPN_DEBUG_FUNC_API_UD0

#define GPN_DEV_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_dev_info/)
#define GPN_DEV_UP_ROM_DIR(name)			GPN_DEV_UP_ROM_NODE#name
  
#define GPN_DEV_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_dev_info/)
#define GPN_DEV_UP_RAM_DIR(name)			GPN_DEV_UP_RAM_NODE#name

#define GPN_DEV_INFO_LINE					256
#define GPN_DEV_INFO_FILE_LEN				4096

#define GPN_DEV_MAX_TYPE_STR_LEN			GPN_DEV_TYPE_MAX_TYPE_STR_LEN

#define GPN_DEV_MAC_LEN						6

typedef struct _stGpnDevBaseInfo_
{
	/*device type infomation, global define */
	UINT32 devType;
	
	/*deviec slot infomation, place local */
	UINT32 selfSlot;
	UINT32 NMaSlot;
	UINT32 NMbSlot;
	UINT32 SWaSlot;
	UINT32 SWbSlot;
	UINT32 maxSlot;
	
	char protocol_mac[GPN_DEV_MAC_LEN];
	UINT16 rsd;
	
	char devStrType[GPN_DEV_MAX_TYPE_STR_LEN];

	/*other ??? */
	
}stGpnDevBaseInfo;

#define GPN_DEV_CI_MAX_SLOT				20
#define GPN_DEV_CI_PER_SLOT_UPORT1		0
#define GPN_DEV_CI_PER_SLOT_UPORT2		1
#define GPN_DEV_CI_PER_SLOT_UPORT3		2
#define GPN_DEV_CI_PER_SLOT_UPORT4		3
#define GPN_DEV_CI_PER_SLOT_UPORT5		4
#define GPN_DEV_CI_PER_SLOT_UPORT6		5
#define GPN_DEV_CI_PER_SLOT_UPORTS		6

#define GPN_DEV_CI_DEFPORT_START		12001


typedef struct _stGpnSlotCommInfo_
{
	UINT32 slotNum;
	UINT32 ipV4;
	UINT16 updPort[GPN_DEV_CI_PER_SLOT_UPORTS];

	/*other ??? */
}stGpnSlotCommInfo;

typedef struct _stGpnDevCommInfo_
{
	UINT32 devType;

	stGpnSlotCommInfo slotCommInfo[GPN_DEV_CI_MAX_SLOT];
}stGpnDevCommInfo;

typedef struct _stIfInfo_
{
	UINT32 portType;
	UINT32 portNum;
}stIfInfo;

typedef struct _stDevFixIf_
{
	UINT32 devType;
	UINT32 portTpNum;
	UINT32 sysPortTpNum;
	UINT32 maxPortTpNum;
	
	stIfInfo *pfixPort;
}stDevFixIf;

typedef struct _stDevMgtInfo_
{
	UINT32 devType;
	UINT32 inbandIp;
	UINT32 inbandVlan;
	UINT32 ib1QPri;
	/* ... */
	UINT8 ibMac[GPN_DEV_MAC_LEN];
	UINT16 rvd;
}stDevMgtInfo;

typedef struct _stGpnDevCoreInfo_
{
	stGpnDevBaseInfo devBaseInfo;
	stGpnDevCommInfo devCommInfo;
	/*fix port info */
	stDevFixIf fixPortInfo;

	/* save this in RAM */
	stDevMgtInfo devMgtInfo;
}stGpnDevInfo;

typedef struct _stGpnDevPortTypeNameTp_
{
	UINT32 portType;
	const char *portTypeStr;
	const char *smpStr;
}stGpnDevPortTypeNameTp;

UINT32 gpnDevInfoUPPathInit(void);
UINT32 gpnDevInfoInit(void);
UINT32 gpnDevInfoCfgFileInit(void);
UINT32 gpnDevBaseInfoInitCreat(char *pbaseInfoPath);
UINT32 gpnDevCommInfoInitCreat(char *pcommInfoPath);
UINT32 gpnDevFixIfInfoInitCreat(char *pdevFixIf);
UINT32 gpnDevMgtInfoInitCreat(char *pdevFixIf);
UINT32 gpnDevBaseInfoInitReCreat(char *pbaseInfoPath);
UINT32 gpnDevCommInfoInitReCreat(char *pcommInfoPath);
UINT32 gpnDevFixIfInfoInitReCreat(char *pdevFixIf);
UINT32 gpnDevMgtInfoInitReCreat(char *pdevFixIf);
UINT32 gpnDevInfoFile2DataSt(void);
UINT32 gpnDevCoreInfoSpaceInit(void);
UINT32 gpnDevBaseInfoFile2DataSt(stGpnDevBaseInfo *pgpnDevBaseInfo);
UINT32 gpnDevCommInfoFile2DataSt(stGpnDevCommInfo *pdevCommInfo);
UINT32 gpnDevFixIfInfoFile2DataSt(stDevFixIf *pfixPortInfo);
UINT32 gpnDevFixIfFileCmdLine2DataSt(stDevFixIf *pfixPortInfo,
	char *portTypeStr, char *pportNum);
UINT32 gpnDevMgtInfoFile2DataSt(stDevMgtInfo *pfixPortInfo);
UINT32 gpnDevInfoHardware2DataSt(void);
UINT32 gpnDevBaseInfoHardware2DataSt(void);
UINT32 gpnDevTypeInfoHardware2DataSt(void);
UINT32 gpnDevCommInfoHardware2DataSt(void);
UINT32 gpnDevFixIfInfoHardware2DataSt(void);
UINT32 gpnDevMgtInfoHardware2DataSt(void);
UINT32 gpnDevCoreInfoDateSt2Ram(void);
UINT32 gpnDevCoreInfoDateSt2RamRenew(void);
char *gpnGlobalPortTypeSmpStrGet(UINT32 portType);
UINT32 debugGpnDevCoreInfoDateStPrint(void);
INT32 gpnDevCoreFree(void);

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_DEV_CORE_INFO_H_ */


