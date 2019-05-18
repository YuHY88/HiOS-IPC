/**********************************************************
* file name: gpnSocketMan.c
* Copyright: 
	 Copyright 2013 huahuan.
* author: 
*    huahuan liuyf 2013-08-14
* function: 
*    define socket comm details
* modify:
*
***********************************************************/
#ifndef _GPN_SOCKET_MAN_C_
#define _GPN_SOCKET_MAN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "socketComm/gpnSocketMan.h"

UINT32 gpnSetSockFdTxRxFifoReCfg(int Fd)
{
	/*set socket fifo size 16k*/
	INT32 bufSize = 1024*16;
	
	if(setsockopt(Fd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(INT32)) < 0)
	{
		return GPN_SOCKET_GEN_ERR;
	}
	if(setsockopt(Fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(INT32)) < 0)
	{
		return GPN_SOCKET_GEN_ERR;
	}

	return GPN_SOCKET_GEN_OK;
}
UINT32 gpnSetSockFdTxRxFifoSpCfg(int Fd, INT32 rxFifo, INT32 txFifo)
{
	if(rxFifo > GPN_SOCKET_RX_MAX_FIFO)
	{
		rxFifo = GPN_SOCKET_RX_MAX_FIFO;
	}
	if(txFifo > GPN_SOCKET_TX_MAX_FIFO)
	{
		txFifo = GPN_SOCKET_TX_MAX_FIFO;
	}
	
	if(setsockopt(Fd, SOL_SOCKET, SO_RCVBUF, &rxFifo, sizeof(INT32)) < 0)
	{
		return GPN_SOCKET_GEN_ERR;
	}
	if(setsockopt(Fd, SOL_SOCKET, SO_SNDBUF, &txFifo, sizeof(INT32)) < 0)
	{
		return GPN_SOCKET_GEN_ERR;
	}

	return GPN_SOCKET_GEN_OK;
}

UINT32 gpnSetSockFdNonbock(int Fd)
{
	int flags;

	if ((flags = fcntl(Fd, F_GETFL, 0)) == -1)
	{
		perror("fcntl socket error");
		GPN_SOCKET_MAN_PRINT(GPN_SOCKET_MAN_AGP, "gpnSetSockFdNonbock:set Fd err %d\n\r", flags);
		
		return GPN_SOCKET_GEN_ERR;
	}
	else
	{
		fcntl(Fd, F_SETFL, flags|O_NONBLOCK);
	}
	
	return GPN_SOCKET_GEN_OK;
}
INT32 gpnUnixTcpCommServer(char *svName, UINT32 client_n)
{
	INT32 svSockFd;
	UINT32 reVal;
	struct sockaddr_un svAddr;

	/*delete old svName obj*/
	unlink(svName);
	svSockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(svSockFd == -1)
	{
		perror("socket");
	}
	else
	{
		svAddr.sun_family = AF_UNIX;
		strcpy(svAddr.sun_path, svName);
		bind(svSockFd, (struct sockaddr *)&svAddr, sizeof(svAddr));
		listen(svSockFd, client_n);

		/*before acept,set svAddr non-block*/
		reVal = gpnSetSockFdNonbock(svSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			/*fd must set to O_NONBLOCK*/
			close(svSockFd);
			unlink(svName);
			svSockFd = -1;
		}
	}
	return svSockFd;
}
INT32 gpnUnixTcpCommClient(void)
{
	INT32 cliSockFd;
	UINT32 reVal;

	cliSockFd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(cliSockFd == -1)
	{
		perror("socket");
	}
	else
	{
		/*before acept,set svAddr non-block*/
		reVal = gpnSetSockFdNonbock(cliSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			/*fd must set to O_NONBLOCK*/
			close(cliSockFd);
			cliSockFd = -1;
		}
	}
	return cliSockFd;
}
INT32 gpnUnixTcpClientConntct(int cliFd, char *svName)
{
	struct sockaddr_un svAddr;
	INT32 reVal;

	svAddr.sun_family = AF_UNIX;
	strcpy(svAddr.sun_path, svName);
	reVal = connect(cliFd, (struct sockaddr *)&svAddr, sizeof(svAddr));
	if(reVal == -1)
	{
		perror("local cli connect");
		return GPN_SOCKET_GEN_ERR;
	}
	
	return GPN_SOCKET_GEN_OK;
}
INT32 gpnUnixUdpCommServer(char *svName)
{
	INT32 svSockFd;
	UINT32 reVal;
	UINT32 addrLen;
	struct sockaddr_un svAddr;

	/*delete old svName obj*/
	unlink(svName);
	svSockFd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(svSockFd == -1)
	{
		perror("UU socket");
	}
	else
	{
		svAddr.sun_family = AF_UNIX;
		strcpy(svAddr.sun_path, svName);
		addrLen = sizeof(svAddr);
		addrLen = offsetof(struct sockaddr_un, sun_path) + strlen(svAddr.sun_path);
		bind(svSockFd, (struct sockaddr *)&svAddr, (socklen_t)addrLen);
		
		/*before acept,set svAddr non-block*/
		reVal = gpnSetSockFdNonbock(svSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			/*fd must set to O_NONBLOCK*/
			close(svSockFd);
			unlink(svName);
			svSockFd = -1;
		}

		/*rx tx fifo re-set*/
		/*some of socket fd's rx tx fifo should be specil cfg*/
		reVal = gpnSetSockFdTxRxFifoReCfg(svSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			close(svSockFd);
			unlink(svName);
			svSockFd = -1;
		}
	}
	return svSockFd;
}
INT32 gpnUnixUdpCommClient(char *cliName)
{
	INT32 svSockFd;

	svSockFd = gpnUnixUdpCommServer(cliName);
	return svSockFd;
}
INT32 gpnINETUdpCommServer(UINT32 ip, UINT32 port)
{
	INT32 svSockFd;
	UINT32 reVal;
	UINT32 addrLen;
	struct sockaddr_in svAddr;

	/*delete old svName obj*/
	
	svSockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(svSockFd == -1)
	{
		perror("IU socket");
	}
	else
	{
		svAddr.sin_family = AF_INET;
		svAddr.sin_addr.s_addr = ip;
		/*svAddr.sin_addr.s_addr = htonl(INADDR_ANY);*/
		svAddr.sin_port = port;
		addrLen = sizeof(svAddr);
		bind(svSockFd, (struct sockaddr *)&svAddr, (socklen_t)addrLen);

		/*before acept,set svAddr non-block*/
		reVal = gpnSetSockFdNonbock(svSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			/*fd must set to O_NONBLOCK*/
			close(svSockFd);
			/*unlink();*/
			svSockFd = -1;
		}

		/*rx tx fifo re-set*/
		reVal = gpnSetSockFdTxRxFifoReCfg(svSockFd);
		if(reVal == GPN_SOCKET_GEN_ERR)
		{
			close(svSockFd);
			/*unlink();*/
			svSockFd = -1;
		}
	}
	return svSockFd;
}
INT32 gpnINETUdpCommClient(UINT32 ip, UINT32 port)
{
	INT32 svSockFd;

	svSockFd = gpnINETUdpCommServer(ip, port);
	return svSockFd;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SOCKET_MAN_C_ */
