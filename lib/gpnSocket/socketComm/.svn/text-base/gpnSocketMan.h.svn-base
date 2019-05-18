/**********************************************************
* file name: gpnSocketMan.h
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-13
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_SOCKET_MAN_H_
#define _GPN_SOCKET_MAN_H_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
//#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>

#include "lib/gpnSocket/socketComm/gpnSockTypeDef.h"

#define GPN_SOCKET_GEN_OK 						GPN_SOCK_SYS_OK
#define GPN_SOCKET_GEN_ERR 						GPN_SOCK_SYS_ERR

#define GPN_SOCKET_MAN_PRINT(level, info...) 	GEN_SOCK_SYS_PRINT((level), info)
#define GPN_SOCKET_MAN_AGP 						GEN_SOCK_SYS_AGP
#define GPN_SOCKET_MAN_SVP 						GEN_SOCK_SYS_SVP
#define GPN_SOCKET_MAN_CMP 						GEN_SOCK_SYS_CMP
#define GPN_SOCKET_MAN_CUP 						GEN_SOCK_SYS_CUP

#define GPN_SOCKET_RX_MAX_FIFO					(1024*64)
#define GPN_SOCKET_TX_MAX_FIFO					(1024*64)

UINT32 gpnSetSockFdTxRxFifoReCfg(int Fd);
UINT32 gpnSetSockFdTxRxFifoSpCfg(int Fd, INT32 rxFifo, INT32 txFifo);
UINT32 gpnSetSockFdNonbock(int Fd);
INT32 gpnUnixTcpCommServer(char *svName, UINT32 client_n);
INT32 gpnUnixTcpCommClient(void);
INT32 gpnUnixTcpClientConntct(int cliFd, char *svName);
INT32 gpnUnixUdpCommServer(char *svName);
INT32 gpnUnixUdpCommClient(char *cliName);
INT32 gpnINETUdpCommServer(UINT32 ip, UINT32 port);
INT32 gpnINETUdpCommClient(UINT32 ip, UINT32 port);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*_GPN_SOCKET_MAN_H_*/

