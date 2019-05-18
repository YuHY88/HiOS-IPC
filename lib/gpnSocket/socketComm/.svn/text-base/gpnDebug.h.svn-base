/**********************************************************
* file name: gpnDebug.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-12-09
* function: 
*    define module debug method and debuginfo proc
* modify:
*
***********************************************************/
#ifndef _GPN_DEBUG_H_
#define _GPN_DEBUG_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"
#include "lib/gpnSocket/socketComm/gpnCommList.h"


#define GPN_DEBUG_GEN_OK 				1
#define GPN_DEBUG_GEN_ERR 				2

#define GPN_DEBUG_ENABLE				1
#define GPN_DEBUG_DISABLE				2

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                            GPN_DEBUG_SMP_STELY global contrl                                                   */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define EN_GEN_SYS_DEBUG_PRINT
/*#undef EN_GEN_SYS_DEBUG_PRINT*/

#ifndef EN_GEN_SYS_DEBUG_PRINT

#define GEN_SYS_DEBUG_PRINT(level, info...) 

#else
extern UINT32 ggpnSysMulDegbuCtrl;

#define GEN_SYS_DEBUG_PRINT(level, info...)	\
if((1U<<level) & ggpnSysMulDegbuCtrl)\
{\
	printf(info);\
}\

#endif


#define GEN_SYS_DEBUG_EGP				9 /* emergency */
#define GEN_SYS_DEBUG_SVP				8 /* severity */
#define GEN_SYS_DEBUG_CMP				7 /* common */
#define GEN_SYS_DEBUG_CUP				6 /* cue */
#define GEN_SYS_DEBUG_UD5				5 /* use define 5 */
#define GEN_SYS_DEBUG_UD4				4 /* use define 4*/
#define GEN_SYS_DEBUG_UD3				3 /* use define 3*/
#define GEN_SYS_DEBUG_UD2				2 /* use define 2*/
#define GEN_SYS_DEBUG_UD1				1 /* use define 1*/
#define GEN_SYS_DEBUG_UD0				0 /* use define 0*/

UINT32 gpnSmpDebugDefaultInit(void);
UINT32 gpnSmpDebugUserDefInit(UINT32 debugCtrl);

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                            GPN_DEBUG_SUP_STELY global contrl                                                   */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*public token id define, start from 1,11~100.. reserved for private token*/
#define GEN_SYS_DEBUG_PUB_TRACE_TOKEN		1 /* xxx1 */
#define GEN_SYS_DEBUG_PUB_XXX2_TOKEN		2 /* xxx2 */
#define GEN_SYS_DEBUG_PUB_XXX3_TOKEN		3 /* xxx3 */
#define GEN_SYS_DEBUG_PUB_XXX4_TOKEN		4 /* xxx4 */
#define GEN_SYS_DEBUG_PUB_XXX5_TOKEN		5 /* xxx5 */
#define GEN_SYS_DEBUG_PUB_XXX6_TOKEN		6 /* xxx6 */
#define GEN_SYS_DEBUG_PUB_XXX7_TOKEN		7 /* xxx7 */
#define GEN_SYS_DEBUG_PUB_XXX8_TOKEN		8 /* xxx8 */
#define GEN_SYS_DEBUG_PUB_XXX9_TOKEN		9 /* xxx9 */

#ifndef GPN_NO_SUPER_DEBUGGING    /* make sure we're wanted */
/******************* Start sup-debug private macros ************************/
#define _GPN_DBG_IF_            gpnIsSupDebugEn()
#define __GPN_DBGMSGT(x)     	gpnSupDebugTokenName x,  gpnSupDebugMsg x
#define __GPN_DBGTRACE       	__GPN_DBGMSGT((GEN_SYS_DEBUG_PUB_TRACE_TOKEN,"%s(): %s, %d:\n",__FUNCTION__,\
                                 __FILE__,__LINE__))
#define __GPN_DBGMSGL(x)     	__GPN_DBGTRACE, gpnSupDebugMsg x
#define __GPN_DBGMSGTL(x)    	__GPN_DBGTRACE, gpnSupDebugTokenName x, gpnSupDebugMsg x

/************************* End   private macros ****************************/



/**************************Start public  macros ****************************/
#define GPN_DEBUGMSG(x)        do {if (_GPN_DBG_IF_) {gpnSupDebugMsg x;} }while(0)
#define GPN_DEBUGMSGT(x)       do {if (_GPN_DBG_IF_) {__GPN_DBGMSGT(x);} }while(0)
#define GPN_DEBUGTRACE         do {if (_GPN_DBG_IF_) {__GPN_DBGTRACE;} }while(0)
#define GPN_DEBUGMSGL(x)       do {if (_GPN_DBG_IF_) {__GPN_DBGMSGL(x);} }while(0)
#define GPN_DEBUGMSGTL(x)      do {if (_GPN_DBG_IF_) {__GPN_DBGMSGTL(x);} }while(0)

#else                           /* NETSNMP_NO_DEBUGGING := enable streamlining of the code */

#define GPN_DEBUGMSG(x)
#define GPN_DEBUGMSGT(x)
#define GPN_DEBUGTRACE
#define GPN_DEBUGMSGL(x)
#define GPN_DEBUGMSGTL(x)

#endif

/*super debug token option define*/
#define GEN_SYS_DEBUG_OPT_LOG			1 /* debug info to log*/
#define GEN_SYS_DEBUG_OPT_PRINT			2 /* print debug info*/
#define GEN_SYS_DEBUG_OPT_L_P			3 /* log and print debug info*/

/*GPN_DEBUG_MAX_SUP_TOKEN define*/
#define GPN_DEBUG_MAX_SUP_TOKEN			32

/*token name lenth define*/
#define GPN_DEBUG_TOKEN_NAME_BLEN		32
typedef struct _supDebugDescrTp_
{
	UINT32 tokenId;
	UINT32 enable;
	UINT32 opt;
    const char *tokenName;
}supDebugDescrTp;

typedef struct _supDebugDescr_
{
	UINT32 tokenId;
	UINT32 enable;
	UINT32 opt;
    char tokenName[GPN_DEBUG_TOKEN_NAME_BLEN];
}supDebugDescr;

typedef struct _supDebugDescrNode_
{
	NODE node;
	supDebugDescr *pdebugDes;
}supDebugDescrNode;

typedef struct _supDebugInfo_
{
	UINT32 debugEn;
	
	LIST enTokenQuen;
	UINT32 enTokenNum;
	LIST disTokenQuen;
	UINT32 disTokenNum;

	supDebugDescr *tokenTable;
	UINT32 tokenNum;
}supDebugInfo;

UINT32 gpnSupDebugInit(void);
UINT32 gpnDebugIsTokenEn(UINT32 tokenId);
supDebugDescr *gpnDebugEnTokenDescrGet(UINT32 tokenId);
UINT32 gpnSupDebugMsg(UINT32 tokenId, const char *format, ...);
UINT32 gpnSupDebugTokenName(UINT32 tokenId, const char *format, ...);
UINT32 gpnSupDebugDis(UINT32 tokenId);
UINT32 gpnSupDebugEn(UINT32 tokenId);
UINT32 gpnSupDebugTokenTableGetAll(UINT32 *tableAddr, UINT32 *lineNum);
UINT32 gpnSupDebugEnCfg(UINT32 opt);
UINT32 gpnIsSupDebugEn(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_DEBUG_H_*/

