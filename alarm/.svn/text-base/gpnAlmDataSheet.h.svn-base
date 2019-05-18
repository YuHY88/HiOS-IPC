/**********************************************************
* file name: gpnAlmDataBase.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-09-17
* function: 
*    define alarm data sheet struct and operation
* modify:
*
***********************************************************/
#ifndef _GPN_ALM_DATA_SHEET_H_
#define _GPN_ALM_DATA_SHEET_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include <lib/vty.h>
#include "socketComm/gpnCommList.h"

#include "type_def.h"

#define GPN_ALM_DATA_SHEET_OK 					GEN_SYS_OK
#define GPN_ALM_DATA_SHEET_ERR 					GEN_SYS_ERR

#define GPN_ALM_DATA_SHEET_YES 					GEN_SYS_OK
#define GPN_ALM_DATA_SHEET_NO 					GEN_SYS_ERR

#define GPN_ALM_CYC_DSHEET						1
#define GPN_ALM_LINE_DSHEET						2

#define GPN_ALM_NULL_DSHEET						0
#define GPN_ALM_CURR_ALM_DSHEET					1
#define GPN_ALM_HIST_ALM_DSHEET					2
#define GPN_ALM_EVENT_DSHEET					3
#define GPN_ALM_ALL_DSHEET						4
#define GPN_ALM_ILLEGAL_DSHEET					0xFFFFFFFF

#define GPN_ALM_DSHEET_INDEX_MAX				3


#define GPN_ALM_CURR_ALM_DB_SIZE				500/*500*/
#define GPN_ALM_HIST_ALM_DB_SIZE				300/*300*/
#define GPN_ALM_EVENT_DB_SIZE					100/*100*/

#define GPN_ALM_DS_HASH_KEY						21


/* alarm data sheet template data struct */
typedef struct _stDataSheetTemp_
{
	UINT32 dbType;
	UINT32 maxLine;
	UINT32 cycMode;
	UINT32 unitSize;
	const char *dbName;
}stDataSheetTemp;

typedef struct _stDataSheetNode_
{
	NODE node;
	
	UINT32 dbType;
	UINT32 maxLine;
	UINT32 cycMode;
	UINT32 unitSize;

	/* uniqu space use strndup */
	char *dbName;

	LIST usedhash[GPN_ALM_DS_HASH_KEY];
	LIST usedList;
	LIST idleList;
}stDataSheetNode;
typedef struct _stDStLineNode_
{
	NODE node;
	
	/* should be uniqu index */
	UINT32 index;

	void *lineAddr;
}stDStLineNode;

#define GPN_ALM_DS_ILLEGAL_INDEX				0xffffffff

typedef struct _stDStHashNode_
{
	NODE node;

	/* should be uniqu index */
	UINT32 index;
	
	stDStLineNode *lineNode;
}stDStHashNode;

#define GPN_ALM_DSHEET_SUFFIX_LEN				64
#define GPN_EVT_DSHEET_SUFFIX_LEN				GPN_ALM_DSHEET_SUFFIX_LEN

typedef struct _stCurrAlmDB_
{
	UINT32 index;
	
	UINT32 almType;
	
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;

	/* add for VPLS */
	UINT32 suffixLen;
	UINT8 almSuffix[GPN_ALM_DSHEET_SUFFIX_LEN];
}stCurrAlmDB;

typedef struct _stHistAlmDB_
{
	UINT32 index;
	
	UINT32 almType;
	
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	
	UINT32 level;
	UINT32 count;
	UINT32 firstTime;
	UINT32 thisTime;
	UINT32 disapTime;

	/* add for VPLS */
	UINT32 suffixLen;
	UINT8 almSuffix[GPN_ALM_DSHEET_SUFFIX_LEN];
}stHistAlmDB;

typedef struct _stEventAlmDB_
{
	UINT32 index;
	
	UINT32 eventType;
	
	UINT32 ifIndex;
	UINT32 ifIndex2;
	UINT32 ifIndex3;
	UINT32 ifIndex4;
	UINT32 ifIndex5;
	UINT32 devIndex;
	
	UINT32 level;
	UINT32 detial;

	/*add for embedded software CLI display */
	UINT32 thisTime;

	/* add for VPLS */
	UINT32 suffixLen;
	UINT8 evtSuffix[GPN_EVT_DSHEET_SUFFIX_LEN];
}stEventAlmDB;

typedef struct _stAlmDataSheetSpace_
{
	LIST almDSList;

}stAlmDataSheetSpace;

/* init */
UINT32 gpnAlmRAMDataSheetInit(void);
UINT32 gpnAlmXTypeDataSheetInit(stDataSheetTemp *pDsTemp);
UINT32 gpnAlmXTypeDataSheetLineInit(stDataSheetNode *pstDSheetNode);

/* dataSheet opt : add, delete, modify, get */
UINT32 gpnAlmDataSheetGetNode(UINT32 dbType, UINT32 index, stDStHashNode **ppstDStHashNode);
UINT32 gpnAlmDataSheetGetFirstNode(UINT32 dbType, stDStLineNode **ppstDStLineNode);
UINT32 gpnAlmDataSheetGetNextNode(UINT32 dbType, UINT32 index, stDStLineNode **ppstDStNextNode);
UINT32 gpnAlmDataSheetGetXDB(UINT32 dbType, stDataSheetNode **ppstDSheetNode);
UINT32 gpnAlmDataSheetGetLine(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len);
UINT32 gpnAlmDataSheetGetNextLine(UINT32 dbType, UINT32 index, void *nextLineBuff, UINT32 len);

UINT32 gpnAlmDataSheetAdd(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len);
UINT32 gpnAlmDataSheetDelete(UINT32 dbType, UINT32 index);
UINT32 gpnAlmDataSheetModify(UINT32 dbType, UINT32 index, void *lineBuff, UINT32 len);

UINT32 debugGPNAlmDataBaseTypeShow(void);
UINT32 debugGPNAlmRAMDBListPrint(UINT32 dbType);

UINT32 gpnAlmDataSheetCurrAlmViewWrite(struct vty *vty);
UINT32 gpnAlmDataSheetHistAlmViewWrite(struct vty *vty);
UINT32 gpnAlmDataSheetEventViewWrite(struct vty *vty);


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_ALM_DATA_SHEET_H_*/

