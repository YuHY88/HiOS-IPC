/**********************************************************
* file name: gpnSockCommFuncApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-29
* function: 
*    define gpn_socket comm apply in xxx module
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_COMM_FUNC_API_C_
#define _GPN_SOCK_COMM_FUNC_API_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*gpnSocket function include */
#include "socketComm/gpnSockInclude.h"

/*log function include*/
#include "socketComm/gpnSockCommFuncApi.h"
/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"
/*debug function include*/
#include "socketComm/gpnDebugFuncApi.h"

/*gpn timer mode include*/
#include "socketComm/gpnTimerService.h"

/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

UINT32 gpnSockCommApiSockCommStartup(UINT32 funcRole, usrMsgProc funUsrMsgProc)
{
	UINT32 reVal;
	
	/*assert */
	if(funUsrMsgProc == NULL )
	{
		GPN_SC_FUNC_API_PRINT(GEN_SYS_DEBUG_CMP, "%s : gpn_socket_comm funUsrMsgProc is NULL, err!\n\r",\
			__FUNCTION__);
		return GPN_SC_FUNC_API_GEN_ERR;
	}
	
	reVal = gpnSockCommInit(&(gSockCommData.gRoleData),\
		&(gSockCommData.gstFdSet), funcRole);
	if(reVal == GPN_SC_FUNC_API_GEN_ERR)
	{
		GPN_SC_FUNC_API_PRINT(GEN_SYS_DEBUG_CUP, "%s : gpn_socket_comm function err!\n\r",\
			__FUNCTION__);
		return GPN_SC_FUNC_API_GEN_ERR;
	}
	
	/*gpnLog(GPN_LOG_L_CRIT, "%s : init OK! funcRole is %08x\n\r",
		__FUNCTION__, funcRole);*/
		
	/*debug print communication socket init status*/
	/*debugGpnSockDisplayGlobalFdSet()*/

	gpnSockUsrMsgProcFunRegister(&(gSockCommData.gstFdSet), funUsrMsgProc);
	
	return GPN_SC_FUNC_API_GEN_OK;
}

UINT32 gpnSockCommApiNoBlockProc(void)
{
	UINT32 i;
	UINT32 reVal;
	struct timeval bTime;

	/*GPN_SC_FUNC_API_PRINT(GPN_SC_FUNC_API_CUP, "%s :  pid(%d), do gpn_socket_proc!\n\r",\
		__FUNCTION__, getpid());*/

	/*set wait time to 0, means no block */
    bTime.tv_sec = 0;
	bTime.tv_usec = 0;
	
	/*continue proc gpnSocket msg if they come*/
	for(i=0;i<GPN_SC_FUNC_API_ONCE_MAX_PROC;i++)
	{
		reVal = gpnSockCommSelectMainProc(&(gSockCommData.gstFdSet), &bTime);
		if(reVal != GPN_SC_FUNC_API_GEN_OK)
		{
			/*GPN_SC_FUNC_API_PRINT(GPN_SC_FUNC_API_CUP, "%s : pid(%d), done gpn_socket_proc!\n\r",\
				__FUNCTION__, getpid());*/
			break;
		}
	}

	return GPN_SC_FUNC_API_GEN_OK;
}

UINT32 gpnSockCommApiBlockProc(void)
{
	gpnSockCommSelectMainProc(&(gSockCommData.gstFdSet), NULL);
	
	return GPN_SC_FUNC_API_GEN_OK;
}

UINT32 gpnSockCommApiSelectTimerNoBlockProc(void)
{
	UINT32 i;
	UINT32 reVal;
	struct timeval bTime;

	/*GPN_SC_FUNC_API_PRINT(GPN_SC_FUNC_API_CUP, "%s :  pid(%d), do gpn_socket_proc!\n\r",\
		__FUNCTION__, getpid());*/
	
	/* check select timer bingo */
	gpnTimerSelectTimerNoBlockBingoCheck();
	
	/*set wait time to 0, means no block */
    bTime.tv_sec = 0;
	bTime.tv_usec = 0;
	
	/*continue proc gpnSocket msg if they come*/
	for(i=0;i<GPN_SC_FUNC_API_ONCE_MAX_PROC;i++)
	{
		reVal = gpnSockCommSelectMainProc(&(gSockCommData.gstFdSet), &bTime);
		if(reVal != GPN_SC_FUNC_API_GEN_OK)
		{
			/*GPN_SC_FUNC_API_PRINT(GPN_SC_FUNC_API_CUP, "%s : pid(%d), done gpn_socket_proc!\n\r",\
				__FUNCTION__, getpid());*/
			break;
		}
	}
	return GPN_SC_FUNC_API_GEN_OK;
}

UINT32 gpnSockCommApiSelectTimerBlockProc(void)
{
	static struct timeval bTime;

	/* check select timer bingo */
	gpnTimerSelectTimerBlockBingoCheck(&bTime);
	
	/*printf("%s : befor bTime(tv_sec %d tv_usec %d)\n\r",
					__FUNCTION__, bTime.tv_sec, bTime.tv_usec);*/
	gpnSockCommSelectMainProc(&(gSockCommData.gstFdSet), &bTime);
	/*printf("%s : after bTime(tv_sec %d tv_usec %d)\n\r",
					__FUNCTION__, bTime.tv_sec, bTime.tv_usec);*/
					
	return GPN_SC_FUNC_API_GEN_OK;
}


#ifdef    __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCK_COMM_FUNC_API_C_*/

