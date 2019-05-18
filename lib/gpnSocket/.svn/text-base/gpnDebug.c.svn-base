/**********************************************************
* file name: gpnDebug.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-12-09
* function: 
*    define module debug method and debuginfo proc
* modify:
*
***********************************************************/
#ifndef _GPN_DEBUG_C_
#define _GPN_DEBUG_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <stdarg.h>


#include "socketComm/gpnDebug.h"
#include "socketComm/gpnDebugUser.h"
#include "gpnLog/gpnLogFuncApi.h"


/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                            GPN_DEBUG_SMP_STELY global contrl                                                   */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
UINT32 ggpnSysMulDegbuCtrl = 0; /*多级别打印全局控制*/

UINT32 gpnSmpDebugDefaultInit(void)
{
	#if 1
	ggpnSysMulDegbuCtrl = 0;
	#else
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_EGP;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_SVP;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_CMP;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_CUP;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD5;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD4;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD3;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD2;
	ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD1;
	/*timer bingo default screen */
	/*ggpnSysMulDegbuCtrl |= 1U<<GEN_SYS_DEBUG_UD0;*/
	#endif
	return GPN_DEBUG_GEN_OK;
}
UINT32 gpnSmpDebugUserDefInit(UINT32 debugCtrl)
{
	ggpnSysMulDegbuCtrl = debugCtrl;
	return GPN_DEBUG_GEN_OK;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*                            GPN_DEBUG_SUP_STELY global contrl                                                   */
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static supDebugInfo *pgsupDebugInfo;

/*public token template*/
supDebugDescrTp gsupDebugPubDescrTp[] =
{
	{GEN_SYS_DEBUG_PUB_TRACE_TOKEN, GPN_DEBUG_DISABLE, GEN_SYS_DEBUG_OPT_LOG, "gpn-pub-debug-test"}
};

UINT32 gpnSupDebugInit(void)
{
	UINT32 i;
	UINT32 j;
	supDebugDescrTp *pDebugTokenTp;
	supDebugDescrNode *pDebugNode;
	supDebugDescr *pDebugDescr;
	
	i = sizeof(gsupDebugPubDescrTp)/sizeof(gsupDebugPubDescrTp[0])+\
		sizeof(gsupDebugPriDescrTp)/sizeof(gsupDebugPriDescrTp[0]);

	if(i > GPN_DEBUG_MAX_SUP_TOKEN)
	{
		i = GPN_DEBUG_MAX_SUP_TOKEN;
	}
	
	j = sizeof(supDebugInfo)+\
		i*sizeof(supDebugDescrNode)+\
		i*sizeof(supDebugDescr);
	
	pgsupDebugInfo = (supDebugInfo *)malloc(j);
	if(pgsupDebugInfo == NULL)
	{
		return GPN_DEBUG_GEN_ERR;
	}
	memset((UINT8 *)(pgsupDebugInfo),0,j);

	pDebugNode = (supDebugDescrNode *)((UINT8 *)pgsupDebugInfo + sizeof(supDebugInfo));
	pDebugDescr = (supDebugDescr *)((UINT8 *)pDebugNode + i*sizeof(supDebugDescrNode));

	pgsupDebugInfo->debugEn = GPN_DEBUG_ENABLE;
	listInit(&(pgsupDebugInfo->enTokenQuen));
	pgsupDebugInfo->enTokenNum = 0;
	listInit(&(pgsupDebugInfo->disTokenQuen));
	pgsupDebugInfo->disTokenNum = 0;
	pgsupDebugInfo->tokenTable = pDebugDescr;
	pgsupDebugInfo->tokenNum = i;
	
	/*public*/
	pDebugTokenTp = gsupDebugPubDescrTp;
	j = sizeof(gsupDebugPubDescrTp)/sizeof(gsupDebugPubDescrTp[0]);
	for(i=0;i<j;i++)
	{
		pDebugNode->pdebugDes = pDebugDescr;

		pDebugDescr->tokenId = pDebugTokenTp->tokenId;
		pDebugDescr->enable = pDebugTokenTp->enable;
		pDebugDescr->opt = pDebugTokenTp->opt;
		strncpy(pDebugDescr->tokenName, pDebugTokenTp->tokenName, GPN_DEBUG_TOKEN_NAME_BLEN);

		if(pDebugDescr->enable == GPN_DEBUG_ENABLE)
		{
			listAdd(&(pgsupDebugInfo->enTokenQuen), (NODE *)pDebugNode);
			pgsupDebugInfo->enTokenNum++;
		}
		else
		{
			listAdd(&(pgsupDebugInfo->disTokenQuen), (NODE *)pDebugNode);
			pgsupDebugInfo->disTokenNum++;
		}

		pDebugNode++;
		pDebugDescr++;
		pDebugTokenTp++;
	}
	/*private*/
	pDebugTokenTp = gsupDebugPriDescrTp;
	j = sizeof(gsupDebugPriDescrTp)/sizeof(gsupDebugPriDescrTp[0]);
	for(i=0;i<j;i++)
	{
		pDebugNode->pdebugDes = pDebugDescr;

		pDebugDescr->tokenId = pDebugTokenTp->tokenId;
		pDebugDescr->enable = pDebugTokenTp->enable;
		pDebugDescr->opt = pDebugTokenTp->opt;
		strncpy(pDebugDescr->tokenName, pDebugTokenTp->tokenName, GPN_DEBUG_TOKEN_NAME_BLEN);

		if(pDebugDescr->enable == GPN_DEBUG_ENABLE)
		{
			listAdd(&(pgsupDebugInfo->enTokenQuen), (NODE *)pDebugNode);
			pgsupDebugInfo->enTokenNum++;
		}
		else
		{
			listAdd(&(pgsupDebugInfo->disTokenQuen), (NODE *)pDebugNode);
			pgsupDebugInfo->disTokenNum++;
		}

		pDebugNode++;
		pDebugDescr++;
		pDebugTokenTp++;
	}
	return GPN_DEBUG_GEN_OK;
}

UINT32 gpnDebugIsTokenEn(UINT32 tokenId)
{
	UINT32 enTokenNum;
	supDebugDescrNode *pDebugNode;

	enTokenNum = 0;
	pDebugNode = (supDebugDescrNode *)listFirst(&(pgsupDebugInfo->enTokenQuen));
	while((pDebugNode != NULL)&&(enTokenNum < pgsupDebugInfo->enTokenNum))
	{
		if(pDebugNode->pdebugDes->tokenId == tokenId)
		{
			if(pDebugNode->pdebugDes->enable == GPN_DEBUG_ENABLE)
			{
				return GPN_DEBUG_GEN_OK;
			}
			else
			{
				return GPN_DEBUG_GEN_ERR;
			}
		}
	}
	
	return GPN_DEBUG_GEN_ERR;
}
supDebugDescr *gpnDebugEnTokenDescrGet(UINT32 tokenId)
{
	UINT32 enTokenNum;
	supDebugDescrNode *pDebugNode;

	enTokenNum = 0;
	pDebugNode = (supDebugDescrNode *)listFirst(&(pgsupDebugInfo->enTokenQuen));
	while((pDebugNode != NULL)&&(enTokenNum < pgsupDebugInfo->enTokenNum))
	{
		if(pDebugNode->pdebugDes->tokenId == tokenId)
		{
			return pDebugNode->pdebugDes;
		}
	}

	return NULL;
}
UINT32 gpnSupDebugMsg(UINT32 tokenId, const char *format, ...)
{
    va_list         debugargs;
	supDebugDescr *pDebugDescr;

    va_start(debugargs, format);

	pDebugDescr = gpnDebugEnTokenDescrGet(tokenId);
	if(pDebugDescr != NULL)
	{
		if(pDebugDescr->opt == GEN_SYS_DEBUG_OPT_LOG)
		{
			gpnVLog(GPN_LOG_L_DEBUG, format, debugargs);
		}
		else if(pDebugDescr->opt == GEN_SYS_DEBUG_OPT_PRINT)
		{
			//printf(format, ...);
		}
		else /*pDebugDescr->opt == GEN_SYS_DEBUG_OPT_L_P*/
		{
			gpnVLog(GPN_LOG_L_DEBUG, format, debugargs);
			//printf(format, ...);
		}
	}
	/*
    if(gpnDebugIsTokenEn(tokenId) == GPN_DEBUG_GEN_OK)
	{
        gpnVLog(GPN_LOG_L_DEBUG, format, debugargs);
    }*/
    va_end(debugargs);

	return GPN_DEBUG_GEN_OK;
}
UINT32 gpnSupDebugTokenName(UINT32 tokenId, const char *format, ...)
{
	va_list         debugargs;
	supDebugDescr *pDebugDescr;

	va_start(debugargs, format);
	pDebugDescr = gpnDebugEnTokenDescrGet(tokenId);
	if(pDebugDescr != NULL)
	{
   		gpnSupDebugMsg(pDebugDescr->tokenId, "%s: ", pDebugDescr->tokenName);
	}

	va_end(debugargs);
	
	return GPN_DEBUG_GEN_OK;
}
/*SNMP & CLI cfg option*/
UINT32 gpnSupDebugDis(UINT32 tokenId)
{
	UINT32 enTokenNum;
	supDebugDescrNode *pDebugNode;

	enTokenNum = 0;
	pDebugNode = (supDebugDescrNode *)listFirst(&(pgsupDebugInfo->enTokenQuen));
	while((pDebugNode != NULL)&&(enTokenNum < pgsupDebugInfo->enTokenNum))
	{
		if(pDebugNode->pdebugDes->tokenId == tokenId)
		{
			break;
		}
	}
	if((pDebugNode == NULL)||(enTokenNum >= pgsupDebugInfo->enTokenNum))
	{
		return GPN_DEBUG_GEN_OK;
	}

	listDelete(&(pgsupDebugInfo->enTokenQuen), (NODE *)pDebugNode);
	if(pgsupDebugInfo->enTokenNum > 0)
	{
		pgsupDebugInfo->enTokenNum--;
	}
	pDebugNode->pdebugDes->enable = GPN_DEBUG_DISABLE;
	listAdd(&(pgsupDebugInfo->disTokenQuen), (NODE *)pDebugNode);
	pgsupDebugInfo->disTokenNum++;

	return GPN_DEBUG_GEN_OK;
}
UINT32 gpnSupDebugEn(UINT32 tokenId)
{
	UINT32 disTokenNum;
	supDebugDescrNode *pDebugNode;

	disTokenNum = 0;
	pDebugNode = (supDebugDescrNode *)listFirst(&(pgsupDebugInfo->disTokenQuen));
	while((pDebugNode != NULL)&&(disTokenNum < pgsupDebugInfo->disTokenNum))
	{
		if(pDebugNode->pdebugDes->tokenId == tokenId)
		{
			break;
		}
	}
	if((pDebugNode == NULL)||(disTokenNum >= pgsupDebugInfo->disTokenNum))
	{
		return GPN_DEBUG_GEN_ERR;
	}

	listDelete(&(pgsupDebugInfo->disTokenQuen), (NODE *)pDebugNode);
	if(pgsupDebugInfo->disTokenNum > 0)
	{
		pgsupDebugInfo->disTokenNum--;
	}
	pDebugNode->pdebugDes->enable = GPN_DEBUG_DISABLE;
	listAdd(&(pgsupDebugInfo->enTokenQuen), (NODE *)pDebugNode);
	pgsupDebugInfo->enTokenNum++;

	return GPN_DEBUG_GEN_OK;
}

UINT32 gpnSupDebugTokenTableGetAll(UINT32 *tableAddr, UINT32 *lineNum)
{
	*tableAddr = (UINT32)pgsupDebugInfo->tokenTable;
	*lineNum = pgsupDebugInfo->tokenNum;

	return GPN_DEBUG_GEN_OK;
}

UINT32 gpnSupDebugEnCfg(UINT32 opt)
{
	if( (opt == GPN_DEBUG_ENABLE) ||
		(opt == GPN_DEBUG_DISABLE) )
	{
		pgsupDebugInfo->debugEn = opt;
		
		return GPN_DEBUG_GEN_OK;
	}
	else
	{
		return GPN_DEBUG_GEN_ERR;
	}
}

UINT32 gpnIsSupDebugEn(void)
{
	if(pgsupDebugInfo->debugEn == GPN_DEBUG_ENABLE)
	{
		return GPN_DEBUG_GEN_OK;
	}
	else
	{
		return GPN_DEBUG_GEN_ERR;
	}
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_DEBUG_C_*/

