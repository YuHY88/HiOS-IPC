/**********************************************************
* file name: gpnSignalSdkPain.c
* Copyright: 
	 Copyright 2015 huahuan.
* author: 
*    huahuan liuyf 2015-05-01
* function: 
*    define SDK pain signal process about used by GPN_SIGNAL
* modify:
*
***********************************************************/
#ifndef _GPN_SIGNAL_SDK_PAIN_C_
#define _GPN_SIGNAL_SDK_PAIN_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <sys/stat.h>

#include "gpnSignal/gpnSignalSdkPain.h"
#include "socketComm/gpnTimerService.h"

UINT32 gpnSignalFixSdkPainSigHandler(SIGHANDLER pdHandler)
{
	/*assert */
	if(pdHandler == NULL)
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s : para err!",\
			__FUNCTION__);
		return GPN_SIG_PROC_GEN_ERR;
	}

	/*gpnSignalFixSigWithHandler(GPN_SIG_TYPE_SDK_PAIN, pdHandler);*/
	
	gpnSignalFixSigWithHandlerMaskAll(GPN_SIG_TYPE_SDK_PAIN, pdHandler);

	return GPN_SIG_PROC_GEN_OK;
}

UINT32 gpnSignalSendSdkPainSig(void)
{
	/*gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_SDK_PAIN);*/
	gpnSignalSendSmpSig2AllPID(GPN_SIG_TYPE_SDK_PAIN);

	return GPN_SIG_PROC_GEN_OK;
}

void gpnSignalProcSdkPainSigInCPX10(INT32 sig)
{
	printf("gpnSignalProcSdkPainSigInCPX10###########################################\n\r");
	return;
}

#include <sys/socket.h>
#include <linux/netlink.h>
#include "socketComm/gpnSocketMan.h"
#include "gpnLog/gpnLogFuncApi.h"
#include "socketComm/gpnSockAlmMsgDef.h"

/* maximum payload size*/
#define MAX_PAYLOAD 1024
#define GPN_TEMP_NETLINK_GROUP	1

struct nlmsghdr *g_gpn_nlh = NULL;
struct sockaddr_nl g_gpn_nl_src_addr;
int g_gpn_nl_sock_fd;

void gpnNetlinkSdkPainProc(void)
{
	g_gpn_nl_sock_fd=socket(AF_NETLINK, SOCK_RAW, /*NETLINK_GENERIC*/25);
	if(g_gpn_nl_sock_fd == -1)
	{
		printf("gpnNetlinkSdkPainProc socket err ...\n\r");
	}
	printf("gpnNetlinkSdkPainProc socket %d ...\n\r", g_gpn_nl_sock_fd);
	
	/* set nublock*/
	gpnSetSockFdNonbock(g_gpn_nl_sock_fd);
	
	memset(&g_gpn_nl_src_addr, 0, sizeof(g_gpn_nl_src_addr));
	/* fix local/src addr info */
	g_gpn_nl_src_addr.nl_family = AF_NETLINK;
	g_gpn_nl_src_addr.nl_pid = getpid();
	/* interested in group 1<<2 */
	g_gpn_nl_src_addr.nl_groups = GPN_TEMP_NETLINK_GROUP;
	/* bind local/src addr with soc_fd */
	bind(g_gpn_nl_sock_fd, (struct sockaddr*)&g_gpn_nl_src_addr,
		sizeof(g_gpn_nl_src_addr));
	
	g_gpn_nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
	memset(g_gpn_nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

	/* Waiting for message from kernel */
	printf("Waiting for message from kernel ...\n\r");

	//netlink_rcv(g_gpn_nl_sock_fd, g_gpn_nlh);

	/*timer Read message from kernel */
	gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(1, second),\
		0, (timerOutFuncPtr)gpnNetlinkSdkPainMsgRecv,\
		(UINT32)g_gpn_nl_sock_fd, (UINT32)g_gpn_nlh);
	
	printf("start timer netlink_rcv ... sock_fd %d g_nlh %08x\n\r",\
		(UINT32)g_gpn_nl_sock_fd, (UINT32)g_gpn_nlh);
  
}  

void gpnNetlinkSdkPainMsgRecv(UINT32 p1, UINT32 p2)
{	
	struct sockaddr_nl dest_addr;
	struct msghdr msg;
	static struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	INT32 sock_fd;
	UINT32 len;
	
	sock_fd = (INT32)p1;
	nlh = (struct nlmsghdr *)p2;
	
	/* fix rcv buf for msg rcv */
	memset(&dest_addr, 0, sizeof(dest_addr));
	
	memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

	memset(&iov, 0, sizeof(iov));
	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

	memset(&msg, 0, sizeof(msg));
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	//printf("Received message ...\n\r");
	len = recvmsg(sock_fd, &msg, 0);
	if((INT32)len == -1)
	{
		//perror("recvmsg ");
	}
	else
	{
		printf("\n\rReceived message dest nl_pid(%d) nl_group(%d)\n\r",  
			dest_addr.nl_pid, dest_addr.nl_groups);
		printf("\n\rReceived message payload(%d) : len(%d) %s\n\r",  
			getpid(), len, (char *)NLMSG_DATA(nlh));

		/*if(strncmp((const char *)NLMSG_DATA(nlh), "reboot", 6) == 0)
		{*/
			printf("\n\rwrite config, and reboot ...\n\r");
			gpnNetlinkSdkPainHandle();
		/*}*/
	}

	/*close(sock_fd);*/
	/*free(nlh)*/
}

UINT32 gpnNetlinkSdkPainHandle(void)
{
	/* set to DA for event */
	gpnSockAlmMsgTxEmbStorOutEvent(0);

	/*timer Read message from kernel */
	gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(1, second),\
		0, (timerOutFuncPtr)gpnNetlinkSdkPainReboot, 0, 0);

	return GPN_SIG_PROC_GEN_OK;
}
UINT32 gpnNetlinkSdkPainReboot(void)
{
	FILE *fp;
	char sh[] = "/home/sdkPain.sh";
	char cmd[] =\
"#!/bin/sh \n"
"( \n"
"sleep 1 \n"
"echo \"hios\" \n"
"sleep 1 \n"
"echo \"enable\" \n"
"sleep 1 \n"
"echo \"write\" \n"
"sleep 20 \n"
"echo \"reload\") | telnet localhost 23 \n";

	gpnLog(GPN_LOG_L_CRIT, "%s: do \"system %s\" !\n\r",\
				__FUNCTION__, cmd);

	fp = fopen(sh, "w+");
	if(fp == NULL)
	{
		GPN_SIG_PROC_PRINT(GPN_SIG_PROC_EGP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), sh);
		return GPN_SIG_PROC_GEN_ERR;
	}

	fwrite(cmd, strlen(cmd), 1, fp);
	fflush(fp);
	fclose(fp);

	if(chmod(sh, 0x777) < 0)
	{
		gpnLog(GPN_LOG_L_CRIT, "--%s : chmod %s fail!--\n\r",
			__FUNCTION__, sh);
		
		return GPN_SIG_PROC_GEN_ERR;
	}
	
	if(system(sh) < 0)  
	{   
		perror("system failed...");
		gpnLog(GPN_LOG_L_CRIT, "%s: \"system %s\" fail!\n\r",\
			__FUNCTION__, cmd);
		return GPN_SIG_PROC_GEN_ERR;
	}

	return GPN_SIG_PROC_GEN_OK;
}
#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_SIGNAL_SDK_PAIN_C_ */
