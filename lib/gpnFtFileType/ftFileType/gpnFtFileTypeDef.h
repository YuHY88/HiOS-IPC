/**********************************************************
* file name: gpnFtFileTypeDef.h
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-06-17
* function: 
*    define GPN_FT file type comm API
* modify:
*
***********************************************************/
#ifndef _GPN_FT_FILE_TYPE_DEF_H_
#define _GPN_FT_FILE_TYPE_DEF_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSockTypeDef.h"
#include "socketComm/gpnUnifyPathDef.h"
#include "socketComm/gpnCommList.h"


#define GPN_FTFT_GEN_OK 					GPN_SOCK_SYS_OK
#define GPN_FTFT_GEN_ERR 					GPN_SOCK_SYS_ERR

#define GPN_FTFT_PRINT(level, info...) 		GEN_SOCK_SYS_PRINT((level), info)
#define GPN_FTFT_AGP 						GEN_SOCK_SYS_AGP
#define GPN_FTFT_SVP 						GEN_SOCK_SYS_SVP
#define GPN_FTFT_CMP 						GEN_SOCK_SYS_CMP
#define GPN_FTFT_CUP 						GEN_SOCK_SYS_CUP
 

#define GPN_FTFT_UP_ROM_NODE				GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_ft/)
#define GPN_FTFT_UP_ROM_DIR(name)			GPN_FTFT_UP_ROM_NODE#name
  
#define GPN_FTFT_UP_RAM_NODE				GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_ft/)
#define GPN_FTFT_UP_RAM_DIR(name)			GPN_FTFT_UP_RAM_NODE#name

#define GPN_FTFT_CFG_LINE					256
#define GPN_FTFT_CFG_FILE_LEN				2048

#define GPN_FTFT_MAX_PATH_LEN				256
#define GPN_FTFT_FILT_TYPE_MAX_NODE			32

#define GPN_FTFT_FILE_SUFFIX_NONE			0
#define GPN_FTFT_FILE_SUFFIX_SLOT			1

/*fileType define :
	bit31~24 : file type's type
	bit23~0  : file type
*/
#define GPN_FTFT_FILE_CLASS_MASK			0xFFFF0000
#define GPN_FTFT_FILE_TYPE_MASK				0x0000FFFF

#define GPN_FTFT_SYS_FILE_CLASS				0x00000000
#define GPN_FTFT_USR_FILE_CLASS				0x00010000

#define GPN_FTFT_USR1_TYPE					((GPN_FTFT_USR_FILE_CLASS)|0x1)

typedef struct _stFtDefGloInfo_
{
	LIST actDefQue;
	UINT32 actDefNum;
	LIST idleDefQue;
	UINT32 idleDefNum;
	
}stFtDefGloInfo;

typedef struct _stFtDefNode_
{
	NODE node;
	
	UINT32 arg;
	
	UINT32 fileType;
	char path[GPN_FTFT_MAX_PATH_LEN];
	
}stFtDefNode;

UINT32 gpnFTFTUPPathInit();
UINT32 gpnFtFileTypeDefineInit();
UINT32 gpnFTFileTypeRsDataInit();
UINT32 gpnFTFileTypeCreatBaseDevType(UINT32 devType);
UINT32 gpnFTFileTypeCreatBaseH9MONM81();
UINT32 gpnFTFileTypeCreatBaseH9MOCPX10();
UINT32 gpnFTFileTypeCreatBaseH20PN2000();
UINT32 gpnFTFileTypeRsDataSpaceInit();
UINT32 gpnFTSysFileTypeRsRegister(UINT32 fileType, char *path, UINT32 arg);
UINT32 gpnFTFileTypeGetRsData();
UINT32 debugGpnFtFileType2FilePathDatePrint();


/********************************************************************************/
/*                COMM     API                                                                                                         */
/********************************************************************************/
UINT32 gpnFtFtAPIFileType2SrcFilePath(UINT32 fileType,
	UINT32 dstSuffix, char *pPath, UINT32 len);
UINT32 gpnFtFtAPIFileType2TargetFilePath(UINT32 fileType,
	UINT32 srcSuffix, UINT32 taskSuffix, char *pPath, UINT32 len);






#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_FT_FILE_TYPE_DEF_H_ */


