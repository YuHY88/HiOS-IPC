/**********************************************************
* file name: gpnFtFileTypeDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-06-17
* function: 
*    define GPN_FT file type comm API
* modify:
*
***********************************************************/
#ifndef _GPN_FT_FILE_TYPE_DEF_C_
#define _GPN_FT_FILE_TYPE_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ftFileType/gpnFtFileTypeDef.h"
#include "socketComm/gpnSpecialWarrper.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"


static char gftFileTypeCfg[] = GPN_FTFT_UP_ROM_DIR(gpnFtFile2Type.conf);
stFtDefGloInfo *pgpnFtFileRsData = NULL;

UINT32 gpnFTFTUPPathInit()
{
	/*used for gpn_ft function*/
	gpnUnifyPathCreat((char *)GPN_FTFT_UP_ROM_NODE);
	gpnUnifyPathCreat((char *)GPN_FTFT_UP_RAM_NODE);

	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFtFileTypeDefineInit()
{
	/*creat path */
	gpnFTFTUPPathInit();
	
	/*init pgpnFtFileRsData from file */
	gpnFTFileTypeRsDataInit();

	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeRsDataInit()
{
	UINT32 devType;

	if(access(gftFileTypeCfg,F_OK | R_OK)==0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s : %s exist, use cfg file info\n\r",\
			__FUNCTION__, gftFileTypeCfg);	
	}
	else
	{
		/*file not exsit, so creat it*/
		GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s : %s not exist, creat base device type\n\r",\
			__FUNCTION__, gftFileTypeCfg);

		/*get dev type */
		gpnDevApiGetDevType(&devType, NULL, 0);
		
		gpnFTFileTypeCreatBaseDevType(devType);
	}

	gpnFTFileTypeRsDataSpaceInit();
	gpnFTFileTypeGetRsData();
	/*debugGpnFtFileType2FilePathDatePrint();*/

	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeCreatBaseDevType(UINT32 devType)
{
	switch(devType)
	{
		case GPN_DEV_TYPE_V8ENM:
			gpnFTFileTypeCreatBaseH9MONM81();
			break;
		case GPN_DEV_TYPE_V8ECPX10:
			gpnFTFileTypeCreatBaseH9MOCPX10();
			break;

		case GPN_DEV_TYPE_H20PN2000:
			gpnFTFileTypeCreatBaseH20PN2000();

		default:
			GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: err devType %d !\n\r",
				__FUNCTION__, devType);
			break;
			
	}
	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeCreatBaseH9MONM81()
{
	FILE *fp;
	char *pbuff;
	char buff[GPN_FTFT_CFG_FILE_LEN];
	
	fp = fopen(gftFileTypeCfg, "w+");
	if(fp == NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: fopen \"%s\" fail!\n\r",
			__FUNCTION__, gftFileTypeCfg);
		
		return GPN_FTFT_GEN_ERR;
	}
	
	pbuff = buff;

	/*distribution */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##name:gpnFtFile2Type.conf \n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##match device type:V8e NM\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##GPN_FT defined for file(char) to file-type(u-int) + file-path(char)\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##add like this:\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE  FULL_PATH 0/1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE : gpn-ft define a u-int witch corresponding a real file\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##0/1:option arg,0 means nothing; 1 means if add slot about suffix\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##example:1	 /home/gpnInitMon.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n\r\n\r");
	/*config */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "1 /home/config/ofs/cfg.start.card 1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "2 /home/config/ofs/cfg.start.info.card 1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "3 /home/config/ofs/cfg.product.card 1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "4 /home/config/ofs/cfg.userdef.card 1\n");
	pbuff = pbuff + strlen(pbuff);
	/*file limit */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "######      can defined most 32 types    ##### \n");
	pbuff = pbuff + strlen(pbuff);
	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);

	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: creat \"%s\", size(%d)!\n\r",
		__FUNCTION__, gftFileTypeCfg, strlen(buff));
	
	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeCreatBaseH9MOCPX10()
{
	FILE *fp;
	char *pbuff;
	char buff[GPN_FTFT_CFG_FILE_LEN];
	
	fp = fopen(gftFileTypeCfg, "w+");
	if(fp == NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: fopen \"%s\" fail!\n\r",
			__FUNCTION__, gftFileTypeCfg);
		
		return GPN_FTFT_GEN_ERR;
	}
	
	pbuff = buff;

	/*distribution */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##name:gpnFtFile2Type.conf \n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##match device type:V8e CPX10\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##GPN_FT defined for file(char) to file-type(u-int) + file-path(char)\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##add like this:\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE  FULL_PATH 0/1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE : gpn-ft define a u-int witch corresponding a real file\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##0/1:option arg,0 means nothing; 1 means if add slot about suffix\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##example:1	 /home/gpnInitMon.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n\r\n\r");
	/*config */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "1 /home/imi_config/startup-config.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "2 /home/imi_config/startup-config.info 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "3 /home/imi_config/product-config.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "4 /home/imi_config/default-config.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	/*file limit */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "######      can defined most 32 types    ##### \n");
	pbuff = pbuff + strlen(pbuff);
	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);

	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: creat \"%s\", size(%d)!\n\r",
		__FUNCTION__, gftFileTypeCfg, strlen(buff));
	
	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeCreatBaseH20PN2000()
{
	FILE *fp;
	char *pbuff;
	char buff[GPN_FTFT_CFG_FILE_LEN];
	
	fp = fopen(gftFileTypeCfg, "w+");
	if(fp == NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: fopen \"%s\" fail!\n\r",
			__FUNCTION__, gftFileTypeCfg);
		
		return GPN_FTFT_GEN_ERR;
	}
	
	pbuff = buff;

	/*distribution */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##name:gpnFtFile2Type.conf \n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##match device type:H20PN2000\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##GPN_FT defined for file(char) to file-type(u-int) + file-path(char)\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##add like this:\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE  FULL_PATH 0/1\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##FILE_TYPE : gpn-ft define a u-int witch corresponding a real file\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##0/1:option arg,0 means nothing; 1 means if add slot about suffix\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##example:1	 /home/gpnInitMon.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "##################################################################\n\r\n\r");
	/*config */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "1 /home/imi_config/startup-config.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "2 /home/imi_config/product-config.conf 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "3 /home/config/gpn/em/em_cfg_db.xml 0\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "4 /home/config/gpn/em/em_cfg_db.xml 0\n");
	pbuff = pbuff + strlen(pbuff);
	/*file limit */
	snprintf(pbuff, GPN_FTFT_CFG_LINE, "######      can defined most 32 types    ##### \n");
	pbuff = pbuff + strlen(pbuff);
	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);

	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: creat \"%s\", size(%d)!\n\r",
		__FUNCTION__, gftFileTypeCfg, strlen(buff));
	
	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTFileTypeRsDataSpaceInit()
{
	UINT32 i;
	UINT32 j;
	UINT8 *buffPtr;

	i = sizeof(stFtDefGloInfo) +/*file type define global struct size*/\
		GPN_FTFT_FILT_TYPE_MAX_NODE*sizeof(stFtDefNode);
	pgpnFtFileRsData = (stFtDefGloInfo*)malloc(i);
	if(pgpnFtFileRsData == NULL)
	{
		zlog_err("%s:malloc err\n\r", __FUNCTION__);
		perror("malloc");
		return GPN_FTFT_GEN_ERR;
	}
	memset((UINT8 *)pgpnFtFileRsData, 0, i);

	listInit(&(pgpnFtFileRsData->actDefQue));
	pgpnFtFileRsData->actDefNum = 0;
	listInit(&(pgpnFtFileRsData->idleDefQue));
	pgpnFtFileRsData->idleDefNum = 0;


	buffPtr = (UINT8 *)pgpnFtFileRsData + sizeof(stFtDefGloInfo);
	for(j=0;j<GPN_FTFT_FILT_TYPE_MAX_NODE;j++)
	{
		listAdd(&(pgpnFtFileRsData->idleDefQue), (NODE *)buffPtr);
		pgpnFtFileRsData->idleDefNum++;

		buffPtr += sizeof(stFtDefNode);
	}

	/*debug*/
	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: gpn_ft's req space init OK!\n\r",\
		__FUNCTION__);
	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: maxNode: %d activeNode %d idleNode %d\n\r",\
		__FUNCTION__, GPN_FTFT_FILT_TYPE_MAX_NODE, pgpnFtFileRsData->actDefNum, pgpnFtFileRsData->idleDefNum);
	
	return GPN_FTFT_GEN_OK;
}

UINT32 gpnFTSysFileTypeRsRegister(UINT32 fileType, char *path, UINT32 arg)
{
	UINT32 sysFileType;
	UINT32 actDefNum;
	stFtDefNode *pftDefNode;

	/*assert */
	if(path == NULL)
	{
		return GPN_FTFT_GEN_ERR;
	}

	/*fileType to sysFileType */
	sysFileType = (fileType & GPN_FTFT_FILE_TYPE_MASK) | GPN_FTFT_SYS_FILE_CLASS;
	
	actDefNum = 0;
	pftDefNode = (stFtDefNode *)listFirst(&(pgpnFtFileRsData->actDefQue));
	while((pftDefNode != NULL)&&(actDefNum < pgpnFtFileRsData->actDefNum))
	{
		if(pftDefNode->fileType == sysFileType)
		{
			GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: sysFiletype(%08x) fileType(%08x) already register, err!\n\r",\
				__FUNCTION__, sysFileType, fileType);

			return GPN_FTFT_GEN_ERR;
		}
		
		actDefNum++;
		pftDefNode = (stFtDefNode *)listNext((NODE *)(pftDefNode));
	}
	
	/*get idle node*/
	if(pgpnFtFileRsData->idleDefNum > 0)
	{
		pftDefNode = (stFtDefNode *)listGet(&(pgpnFtFileRsData->idleDefQue));
		if(pftDefNode == NULL)
		{
			GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: get idleDefNode err!\n\r",\
				__FUNCTION__);
			return GPN_FTFT_GEN_ERR;
		}
		pgpnFtFileRsData->idleDefNum--;

		/*init reqNode in list */
		pftDefNode->fileType = sysFileType;
		pftDefNode->arg = arg;
		memcpy(pftDefNode->path, path, GPN_FTFT_MAX_PATH_LEN);

		/*add in active list */
		listAdd(&(pgpnFtFileRsData->actDefQue), (NODE *)pftDefNode);
		pgpnFtFileRsData->actDefNum++;

		return GPN_FTFT_GEN_OK;
	}
	else
	{
		GPN_FTFT_PRINT(GPN_FTFT_AGP, "%s : idleDefNum = 0\n\r",\
			__FUNCTION__);

		return GPN_FTFT_GEN_ERR;
	}
}

UINT32 gpnFTFileTypeGetRsData()
{
	UINT32 i;
	UINT32 ft;
	UINT32 arg;
	FILE *fp;
	char *uiType;
	char *chPaht;
	char *chArg;
	char typeDef[GPN_FTFT_CFG_LINE];
	
	fp = fopen(gftFileTypeCfg, "rb");
	if(fp == NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: fopen \"%s\" fail!\n\r",
			__FUNCTION__, gftFileTypeCfg);
		
		return GPN_FTFT_GEN_ERR;
	}
	
	while(gpnFgets(typeDef, GPN_FTFT_CFG_LINE, fp) != NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s:  cfg line : %s\n\r",\
			__FUNCTION__, typeDef);
		
		if( (typeDef[0] == '#') ||\
			(typeDef[0] == '\0') ||\
			(typeDef[0] == '\n') ||\
			(typeDef[0] == '\r') ||\
			(typeDef[0] == ' ') )
		{
			continue;
		}
		
		/*find U-INT fileType */
		uiType = typeDef;
		for(i=0;i<GPN_FTFT_CFG_LINE;i++)
		{
			if( (typeDef[i] == ' ') ||\
				(typeDef[i] == '\t') )
			{
				typeDef[i] = '\0';
				break;
			}
		}
		/*find file path */
		chPaht = NULL;
		for(i++;i<GPN_FTFT_CFG_LINE;i++)
		{
			if( (typeDef[i] != ' ') &&\
				(typeDef[i] != '\t') )
			{
				chPaht = &(typeDef[i]);
				break;
			}
		}
		for(i++;i<GPN_FTFT_CFG_LINE;i++)
		{
			if( (typeDef[i] == ' ') ||\
				(typeDef[i] == '\t') )
			{
				typeDef[i] = '\0';
				break;
			}
		}
		/*find opt arg*/
		chArg = NULL;
		for(i++;i<GPN_FTFT_CFG_LINE;i++)
		{
			if( (typeDef[i] != ' ') &&\
				(typeDef[i] != '\t') )
			{
				chArg = &(typeDef[i]);
				break;
			}
		}
		for(i++;i<GPN_FTFT_CFG_LINE;i++)
		{
			if( (typeDef[i] == ' ') ||\
				(typeDef[i] == '\t') ||\
				(typeDef[i] == '\n') ||\
				(typeDef[i] == '\r') ||\
				(typeDef[i] == '\0') )
			{
				typeDef[i] = '\0';
				break;
			}
		}
		
		if( (uiType == NULL) ||\
			(chPaht == NULL) ||\
			(chArg == NULL) )
		{
			GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: decode cfg line err!\n\r",
				__FUNCTION__);

			fclose(fp);
			
			return GPN_FTFT_GEN_ERR;
		}

		
		if(sscanf((const char *)uiType, "%d", &ft) != 1)
		{
			GPN_FTFT_PRINT(GPN_FTFT_AGP, "%s: uiType(%s), sscanf err!\n\r",\
				__FUNCTION__, uiType);

			fclose(fp);
			
			return GPN_FTFT_GEN_ERR;
		}
		if(sscanf((const char *)chArg, "%d", &arg) != 1)
		{
			GPN_FTFT_PRINT(GPN_FTFT_AGP, "%s: uiType(%s), sscanf err!\n\r",\
				__FUNCTION__, uiType);
			fclose(fp);
			
			return GPN_FTFT_GEN_ERR;
		}
		
		/*register */
		gpnFTSysFileTypeRsRegister(ft, chPaht, arg);
	}

	GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: config file read end!\n\r",\
		__FUNCTION__);
	fclose(fp);

	return GPN_FTFT_GEN_OK;
}

UINT32 debugGpnFtFileType2FilePathDatePrint()
{
	UINT32 actDefNum;
	stFtDefNode *pftDefNode;

	if(pgpnFtFileRsData == NULL)
	{
		GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: pgpnFtFileRsData is NULL, err!\n\r",\
			__FUNCTION__);
					
		return GPN_FTFT_GEN_ERR;
	}
	
	actDefNum = 0;
	pftDefNode = (stFtDefNode *)listFirst(&(pgpnFtFileRsData->actDefQue));
	while((pftDefNode != NULL)&&(actDefNum < pgpnFtFileRsData->actDefNum))
	{
		fprintf(stdout, "fileType(%08x) path(%64s) arg(%2d)\n\r",\
			pftDefNode->fileType, pftDefNode->path, pftDefNode->arg);
		
		actDefNum++;
		pftDefNode = (stFtDefNode *)listNext((NODE *)(pftDefNode));
	}
	
	return GPN_FTFT_GEN_OK;
}



/********************************************************************************/
/*                COMM     API                                                                                                         */
/********************************************************************************/

UINT32 gpnFtFtAPIFileType2SrcFilePath(UINT32 fileType, UINT32 dstSuffix, char *pPath, UINT32 len)
{
	UINT32 actDefNum;
	stFtDefNode *pftDefNode;

	/*assert */
	if( (pPath == NULL) ||\
		(len < GPN_FTFT_MAX_PATH_LEN) )
	{
		return GPN_FTFT_GEN_ERR;
	}
	if(pgpnFtFileRsData == NULL)
	{
		/*creat global file type date and creat file type define */
		gpnFtFileTypeDefineInit();
	}
	
	actDefNum = 0;
	pftDefNode = (stFtDefNode *)listFirst(&(pgpnFtFileRsData->actDefQue));
	while((pftDefNode != NULL)&&(actDefNum < pgpnFtFileRsData->actDefNum))
	{
		if(pftDefNode->fileType == fileType)
		{
			if(pftDefNode->arg == GPN_FTFT_FILE_SUFFIX_SLOT)
			{
				snprintf(pPath, GPN_FTFT_MAX_PATH_LEN, "%s.%d", pftDefNode->path, dstSuffix);
			}
			else
			{
				snprintf(pPath, GPN_FTFT_MAX_PATH_LEN, "%s", pftDefNode->path);
			}

			GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: get fileType(%08x)'s src path(%s)!\n\r",\
				__FUNCTION__, fileType, pPath);
			return GPN_FTFT_GEN_OK;
		}
		
		actDefNum++;
		pftDefNode = (stFtDefNode *)listNext((NODE *)(pftDefNode));
	}

	GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: get fileType(%08x)'s src path err!\n\r",\
		__FUNCTION__, fileType);
				
	return GPN_FTFT_GEN_ERR;
}
UINT32 gpnFtFtAPIFileType2TargetFilePath(UINT32 fileType, UINT32 srcSuffix, UINT32 taskSuffix, char *pPath, UINT32 len)
{
	UINT32 actDefNum;
	stFtDefNode *pftDefNode;

	/*assert */
	if( (pPath == NULL) ||\
		(len < GPN_FTFT_MAX_PATH_LEN) )
	{
		return GPN_FTFT_GEN_ERR;
	}
	if(pgpnFtFileRsData == NULL)
	{
		/*creat global file type date and creat file type define */
		gpnFtFileTypeDefineInit();
	}
	
	actDefNum = 0;
	pftDefNode = (stFtDefNode *)listFirst(&(pgpnFtFileRsData->actDefQue));
	while((pftDefNode != NULL)&&(actDefNum < pgpnFtFileRsData->actDefNum))
	{
		if(pftDefNode->fileType == fileType)
		{
			GPN_FTFT_PRINT(GPN_FTFT_CUP, "%s: get fileType(%08x)'s path(%s)!\n\r",\
				__FUNCTION__, fileType, pftDefNode->path);
			if(pftDefNode->arg == GPN_FTFT_FILE_SUFFIX_SLOT)
			{
				snprintf(pPath, GPN_FTFT_MAX_PATH_LEN, "%s.%d.%d",\
					pftDefNode->path, srcSuffix, taskSuffix);
			}
			else
			{
				snprintf(pPath, GPN_FTFT_MAX_PATH_LEN, "%s.%d",\
					pftDefNode->path, taskSuffix);
			}
			
			return GPN_FTFT_GEN_OK;
		}
		
		actDefNum++;
		pftDefNode = (stFtDefNode *)listNext((NODE *)(pftDefNode));
	}

	GPN_FTFT_PRINT(GPN_FTFT_CMP, "%s: get fileType(%08x)'s target path err!\n\r",\
		__FUNCTION__, fileType);
				
	return GPN_FTFT_GEN_ERR;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_FT_FILE_TYPE_DEF_C_ */

