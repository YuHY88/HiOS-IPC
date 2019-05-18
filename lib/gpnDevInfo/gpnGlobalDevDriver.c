/**********************************************************
* file name: gpnGlobalDevDriver.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-09
* function: 
*    define GPN_DEV_DRIVER method
* modify:
*
***********************************************************/
#ifndef _GPN_GLOBAL_DEV_DRIVER_C_
#define _GPN_GLOBAL_DEV_DRIVER_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
//#include <arpa/inet.h>
#include <errno.h>

#include "socketComm/gpnSpecialWarrper.h"


#include "socketComm/gpnGlobalPortIndexDef.h"

#include "devCoreInfo/gpnGlobalDevDriver.h"
#include "devCoreInfo/gpnDevCoreInfo.h"

extern stGpnDevInfo *pgpnDevCoreInfo;

UINT32 gpnDevDrvGetEth0Mac(char *macAddr)
{
	INT32 sockfd;
	struct ifreq p_mac;

	if(macAddr == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	memset(macAddr, 0, 6);
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("socket");
		return GPN_DEV_DRV_GEN_ERR;
	}

	memset(&p_mac, 0, sizeof(p_mac));
	strcpy(p_mac.ifr_name, "eth0");

	if((ioctl(sockfd, SIOCGIFHWADDR, &p_mac)) < 0)
	{
		close(sockfd);
		return GPN_DEV_DRV_GEN_ERR;
	}
	close(sockfd);
	
	memcpy(macAddr, p_mac.ifr_hwaddr.sa_data, 6);	
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvGetEthHHMac(char *macAddr)
{
	if(macAddr == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	memset(macAddr, 0, 6);
	
	gpnDevDrvGetEth0Mac(macAddr);
	
	/*eth0 to eth-hh*/
	macAddr[3] += 1;	
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvIndependentTypeDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	pgpnDevCoreInfo->devBaseInfo.selfSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 0;

	/*protocol mac can not get from HW*/
	/*gpnDevDrvGetEth0Mac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);*/
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvDefaultGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo fixPortTp[] =
		{
			{IFM_LSP_TYPE,		2},
			{IFM_FUN_ETH_TYPE,	2},
			{IFM_E1_TYPE,		2},
			{IFM_PWE3E1_TYPE,	2},	
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(fixPortTp)/sizeof(fixPortTp[0]);
	pstIfInfo = (stIfInfo *)fixPortTp;

	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}

	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1660PDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1660PGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H20PN1660PFixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			{IFM_ENV_TYPE,		1},
			{IFM_POWN48_TYPE,	2},
			{IFM_POW220_TYPE,	2},
			{IFM_FUN_ETH_TYPE,	64},	
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H20PN1660PFixPortTp)/sizeof(H20PN1660PFixPortTp[0]);
	pstIfInfo = (stIfInfo *)H20PN1660PFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1660CDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1660CGetFixPort(void)
{
	gpnDevDrvH20PN1660PGetFixPort();
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1610Driver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*1610 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN1610GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H20PN1610FixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			{IFM_ENV_TYPE,		1},
			{IFM_POWN48_TYPE,	2},
			{IFM_POW220_TYPE,	2},
			{IFM_EQU_TYPE,		2},
			{IFM_FAN_TYPE,		4},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H20PN1610FixPortTp)/sizeof(H20PN1610FixPortTp[0]);
	pstIfInfo = (stIfInfo *)H20PN1610FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20PN1610GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20CE1604CDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*1604C protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20CE1604CGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H20PN1610FixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	2},
			//{IFM_POW220_TYPE,	2},
			//{IFM_EQU_TYPE,		2},
			//{IFM_FAN_TYPE,		4},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H20PN1610FixPortTp)/sizeof(H20PN1610FixPortTp[0]);
	pstIfInfo = (stIfInfo *)H20PN1610FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20CE1604CGetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN2000Driver(void)
{
	UINT32 selfSlot;
	UINT8 mac[6];
	/**have big problem**/
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	selfSlot = 0;
	if(gpnDevDrvH20PN2000GetSelfSlotDriver(&selfSlot) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get selfSlot err\n\r",\
			__FUNCTION__, getpid());
	}

	if(gpnDevDrvH20PN2000GetPMacDriver(mac) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get protocol_mac err\n\r",\
			__FUNCTION__, getpid());
	}
	
	pgpnDevCoreInfo->devBaseInfo.selfSlot = selfSlot;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 1;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 2;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 20;

	/*2000 protocol mac is saved in blacboard eeprom, here is temp set */
	//memcpy(pgpnDevCoreInfo->devBaseInfo.protocol_mac, mac, 6);
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20PN2000GetSelfSlotDriver(UINT32 *pslot)
{
	INT32 fd;
	FILE *fp;
	UINT32 selfSlot;
	char line[GPN_DEV_INFO_LINE];
	s_2000_fpga argv;

	/* 2000 ram self slot mark file */
	char gDevSelfSlot[] = GPN_DEV_UP_RAM_DIR(gpnDevSelfSlot.txt);

	/*check if self slot ram mark file exit */
	if(access(gDevSelfSlot, F_OK | R_OK) == 0)
	{
		/* exit, read ram file for self slot */
		fp = fopen(gDevSelfSlot, "rb");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}

		if(gpnFgets(line, GPN_DEV_INFO_LINE, fp) != NULL)
		{
			if(sscanf((const char *)line, "%d", &selfSlot) == 1)
			{
				*pslot = selfSlot;
				fclose(fp);
				return GPN_DEV_DRV_GEN_OK;
			}
		}

		fclose(fp);

		return GPN_DEV_DRV_GEN_ERR;
	}
	else
	{
		/* not exit, read fpga driver for self slot */
		/* read fpga driver */
        fd = open("/dev/spidev0.0", O_RDWR);
		if(fd < 0)
    	{
        	return GPN_DEV_DRV_GEN_ERR;
    	}
		
		argv.cs = 0;
		argv.addr = 0x0002;
		argv.size = 4;
		read(fd, &argv, argv.size);
	
		selfSlot = ((UINT32)argv.buff[0] << 24) |\
				   ((UINT32)argv.buff[1] << 16) |\
				   ((UINT32)argv.buff[2] <<	8) |\
				   ((UINT32)argv.buff[3]);
		/* change to soft slot */
		selfSlot +=1;

		if( (selfSlot != 1) && (selfSlot != 2) )
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read self slot %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		*pslot = selfSlot;
		
		close(fd);

		/*creat self slot ram mark file */
		fp = fopen(gDevSelfSlot, "w+");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : creat %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		snprintf(line, GPN_DEV_INFO_LINE, "%d\n", selfSlot);

		fwrite(line, strlen(line), 1, fp);
		fflush(fp);
		fclose(fp);

		/*printf("%s(%d) : read self slot %d\n\r",\
			__FUNCTION__, getpid(), selfSlot);*/
	}
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20PN2000GetPMacDriver(UINT8 *pMac)
{
	/* will do like gpnDevDrvH20PN2000GetSelfSlotDriver */
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20PN2000GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H20PN2000FixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	2},
			//{IFM_POW220_TYPE,	2},
			//{IFM_EQU_TYPE,		2},
			//{IFM_FAN_TYPE,		4},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H20PN2000FixPortTp)/sizeof(H20PN2000FixPortTp[0]);
	pstIfInfo = (stIfInfo *)H20PN2000FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20PN2000GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH18EDD0402BDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH18EDD2402BDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH18EDD0402BGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H18EDD0402BFixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			{IFM_ENV_TYPE,		1},
			{IFM_POWN48_TYPE,	2},
			{IFM_POW220_TYPE,	2},
			{IFM_E1_TYPE,		8},
			{IFM_PWE3E1_TYPE,	8},
			{IFM_FUN_ETH_TYPE,	6},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H18EDD0402BFixPortTp)/sizeof(H18EDD0402BFixPortTp[0]);
	pstIfInfo = (stIfInfo *)H18EDD0402BFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH18EDD2402BGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H18EDD2402BFixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			{IFM_ENV_TYPE,		1},
			{IFM_EQU_TYPE,	    2},
			{IFM_FAN_TYPE,	    4},
			{IFM_POWN48_TYPE,	2},
			{IFM_POW220_TYPE,	2},
			{IFM_FUN_ETH_TYPE,	26},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H18EDD2402BFixPortTp)/sizeof(H18EDD2402BFixPortTp[0]);
	pstIfInfo = (stIfInfo *)H18EDD2402BFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH9MONM81Driver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	pgpnDevCoreInfo->devBaseInfo.selfSlot = 17;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 17;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 18;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 10;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 11;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 20;

	/*protocol mac can not get from HW*/
	/*gpnDevDrvGetEth0Mac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);*/
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH9MONM81GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo V8eNMFixPortTp[] =
		{
			{IFM_SOFT_TYPE,		1},
			{IFM_ENV_TYPE,		1},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(V8eNMFixPortTp)/sizeof(V8eNMFixPortTp[0]);
	pstIfInfo = (stIfInfo *)V8eNMFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH9MOCPX10Driver(void)
{
	UINT32 selfSlot;
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	selfSlot = 0;
	if(gpnDevDrvH9MOCPX10GetSelfSlotDriver(&selfSlot) == GPN_DEV_DRV_GEN_OK)
	{
		pgpnDevCoreInfo->devBaseInfo.selfSlot = selfSlot;
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get selfSlot err\n\r", __FUNCTION__, getpid());
	}
	
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 17;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 18;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 10;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 11;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 20;

	/*protocol mac can not get from HW*/
	/*gpnDevDrvGetEth0Mac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);*/
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH9MOCPX10GetSelfSlotDriver(UINT32 *pslot)
{
	INT32 fd;
	char cSlot;
	
	/*assert */
	if(pslot == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	fd = open("/dev/cat9554", O_RDWR);

	if(fd < 0)
	{
		perror("Open cat9554 err\n");

		return GPN_DEV_DRV_GEN_ERR;	
	}

	ioctl(fd, 1, &cSlot);

	cSlot &= 0x03;

	if(cSlot == 2)
	{
		cSlot = 11;
	}
	else
	{
		cSlot = 10;
	}

	close(fd);

	*pslot = cSlot;
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH9MOCPX10GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo VCPx10FixPortTp[] =
	{
		{IFM_SOFT_TYPE,		1},
		{IFM_ENV_TYPE,		1},
		{IFM_EQU_TYPE,		1},
	};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(VCPx10FixPortTp)/sizeof(VCPx10FixPortTp[0]);
	pstIfInfo = (stIfInfo *)VCPx10FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;
		
		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH9MOCPX10GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}





/* add for ipran, by lipf, 2018/4/9
 * HT201
 * HT201E
 * H20RN2000
 * HT157
 * HT158
 * VX
 */

UINT32 gpnDevDrvHT201Driver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*201 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT201GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo HT201FixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	1},
			//{IFM_POW220_TYPE,	1},
			//{IFM_EQU_TYPE,		1},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(HT201FixPortTp)/sizeof(HT201FixPortTp[0]);
	pstIfInfo = (stIfInfo *)HT201FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT201GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}



UINT32 gpnDevDrvHT201EDriver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*201 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT201EGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo HT201EFixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	1},
			//{IFM_POW220_TYPE,	1},
			//{IFM_EQU_TYPE,		1},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(HT201EFixPortTp)/sizeof(HT201EFixPortTp[0]);
	pstIfInfo = (stIfInfo *)HT201EFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT201EGetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}



UINT32 gpnDevDrvH20RN2000Driver(void)
{
#if 0
	UINT32 selfSlot;
	UINT8 mac[6];
	/**have big problem**/
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	selfSlot = 0;
	if(gpnDevDrvH20RN2000GetSelfSlotDriver(&selfSlot) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get selfSlot err\n\r",\
			__FUNCTION__, getpid());
	}

	if(gpnDevDrvH20RN2000GetPMacDriver(mac) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get protocol_mac err\n\r",\
			__FUNCTION__, getpid());
	}
	
	pgpnDevCoreInfo->devBaseInfo.selfSlot = selfSlot;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 1;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 2;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 20;

	/*2000 protocol mac is saved in blacboard eeprom, here is temp set */
	//memcpy(pgpnDevCoreInfo->devBaseInfo.protocol_mac, mac, 6);
#endif

	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*2000 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20RN2000GetSelfSlotDriver(UINT32 *pslot)
{
	INT32 fd;
	FILE *fp;
	UINT32 selfSlot;
	char line[GPN_DEV_INFO_LINE];
	s_2000_fpga argv;

	/* 2000 ram self slot mark file */
	char gDevSelfSlot[] = GPN_DEV_UP_RAM_DIR(gpnDevSelfSlot.txt);

	/*check if self slot ram mark file exit */
	if(access(gDevSelfSlot, F_OK | R_OK) == 0)
	{
		/* exit, read ram file for self slot */
		fp = fopen(gDevSelfSlot, "rb");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}

		if(gpnFgets(line, GPN_DEV_INFO_LINE, fp) != NULL)
		{
			if(sscanf((const char *)line, "%d", &selfSlot) == 1)
			{
				*pslot = selfSlot;
				fclose(fp);
				return GPN_DEV_DRV_GEN_OK;
			}
		}

		fclose(fp);

		return GPN_DEV_DRV_GEN_ERR;
	}
	else
	{
		/* not exit, read fpga driver for self slot */
		/* read fpga driver */
        fd = open("/dev/spidev0.0", O_RDWR);
		if(fd < 0)
    	{
        	return GPN_DEV_DRV_GEN_ERR;
    	}
		
		argv.cs = 0;
		argv.addr = 0x0002;
		argv.size = 4;
		read(fd, &argv, argv.size);
	
		selfSlot = ((UINT32)argv.buff[0] << 24) |\
				   ((UINT32)argv.buff[1] << 16) |\
				   ((UINT32)argv.buff[2] <<	8) |\
				   ((UINT32)argv.buff[3]);
		/* change to soft slot */
		selfSlot +=1;

		if( (selfSlot != 1) && (selfSlot != 2) )
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read self slot %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		*pslot = selfSlot;
		
		close(fd);

		/*creat self slot ram mark file */
		fp = fopen(gDevSelfSlot, "w+");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : creat %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		snprintf(line, GPN_DEV_INFO_LINE, "%d\n", selfSlot);

		fwrite(line, strlen(line), 1, fp);
		fflush(fp);
		fclose(fp);

		/*printf("%s(%d) : read self slot %d\n\r",\
			__FUNCTION__, getpid(), selfSlot);*/
	}
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20RN2000GetPMacDriver(UINT8 *pMac)
{
	/* will do like gpnDevDrvH20PN2000GetSelfSlotDriver */
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvH20RN2000GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo H20RN2000FixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	2},
			//{IFM_POW220_TYPE,	2},
			//{IFM_EQU_TYPE,		2},
			//{IFM_FAN_TYPE,		4},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(H20RN2000FixPortTp)/sizeof(H20RN2000FixPortTp[0]);
	pstIfInfo = (stIfInfo *)H20RN2000FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvH20RN2000GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}



UINT32 gpnDevDrvHT157Driver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*201 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT157GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo HT157FixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	1},
			//{IFM_POW220_TYPE,	1},
			//{IFM_EQU_TYPE,		1},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(HT157FixPortTp)/sizeof(HT157FixPortTp[0]);
	pstIfInfo = (stIfInfo *)HT157FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT157GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}



UINT32 gpnDevDrvHT158Driver(void)
{
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	gpnDevDrvIndependentTypeDriver();

	/*201 protocol mac is eth-hh mac */
	gpnDevDrvGetEthHHMac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT158GetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo HT158FixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	1},
			//{IFM_POW220_TYPE,	1},
			//{IFM_EQU_TYPE,		1},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(HT158FixPortTp)/sizeof(HT158FixPortTp[0]);
	pstIfInfo = (stIfInfo *)HT158FixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvHT158GetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}



UINT32 gpnDevDrvVXDriver(void)
{
	UINT32 selfSlot;
	UINT8 mac[6];
	/**have big problem**/
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	/*base info fix set */
	selfSlot = 0;
	if(gpnDevDrvVXGetSelfSlotDriver(&selfSlot) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get selfSlot err\n\r",\
			__FUNCTION__, getpid());
	}

	if(gpnDevDrvVXGetPMacDriver(mac) != GPN_DEV_DRV_GEN_OK)
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : get protocol_mac err\n\r",\
			__FUNCTION__, getpid());
	}
	
	pgpnDevCoreInfo->devBaseInfo.selfSlot = selfSlot;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 1;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 2;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 20;

	/*VX protocol mac is saved in blacboard eeprom, here is temp set */
	//memcpy(pgpnDevCoreInfo->devBaseInfo.protocol_mac, mac, 6);
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvVXGetSelfSlotDriver(UINT32 *pslot)
{
	INT32 fd;
	FILE *fp;
	UINT32 selfSlot;
	char line[GPN_DEV_INFO_LINE];
	s_2000_fpga argv;

	/* VX ram self slot mark file */
	char gDevSelfSlot[] = GPN_DEV_UP_RAM_DIR(gpnDevSelfSlot.txt);

	/*check if self slot ram mark file exit */
	if(access(gDevSelfSlot, F_OK | R_OK) == 0)
	{
		/* exit, read ram file for self slot */
		fp = fopen(gDevSelfSlot, "rb");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}

		if(gpnFgets(line, GPN_DEV_INFO_LINE, fp) != NULL)
		{
			if(sscanf((const char *)line, "%d", &selfSlot) == 1)
			{
				*pslot = selfSlot;
				fclose(fp);
				return GPN_DEV_DRV_GEN_OK;
			}
		}

		fclose(fp);

		return GPN_DEV_DRV_GEN_ERR;
	}
	else
	{
		/* not exit, read fpga driver for self slot */
		/* read fpga driver */
        fd = open("/dev/spidev0.0", O_RDWR);
		if(fd < 0)
    	{
        	return GPN_DEV_DRV_GEN_ERR;
    	}
		
		argv.cs = 0;
		argv.addr = 0x0002;
		argv.size = 4;
		read(fd, &argv, argv.size);
	
		selfSlot = ((UINT32)argv.buff[0] << 24) |\
				   ((UINT32)argv.buff[1] << 16) |\
				   ((UINT32)argv.buff[2] <<	8) |\
				   ((UINT32)argv.buff[3]);
		/* change to soft slot */
		selfSlot +=1;

		if( (selfSlot != 1) && (selfSlot != 2) )
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : read self slot %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		*pslot = selfSlot;
		
		close(fd);

		/*creat self slot ram mark file */
		fp = fopen(gDevSelfSlot, "w+");
		if(fp == NULL)
		{
			GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : creat %s err\n\r",\
				__FUNCTION__, getpid(), gDevSelfSlot);
			return GPN_DEV_DRV_GEN_ERR;
		}
		
		snprintf(line, GPN_DEV_INFO_LINE, "%d\n", selfSlot);

		fwrite(line, strlen(line), 1, fp);
		fflush(fp);
		fclose(fp);

		/*printf("%s(%d) : read self slot %d\n\r",\
			__FUNCTION__, getpid(), selfSlot);*/
	}
	
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvVXGetPMacDriver(UINT8 *pMac)
{
	/* will do like gpnDevDrvVXGetSelfSlotDriver */
	return GPN_DEV_DRV_GEN_OK;
}
UINT32 gpnDevDrvVXGetFixPort(void)
{
	UINT32 portTypeNum;
	stIfInfo *pstIfInfo;
	stIfInfo VXFixPortTp[] =
		{
			//{IFM_SOFT_TYPE,		1},
			//{IFM_ENV_TYPE,		1},
			//{IFM_POWN48_TYPE,	2},
			//{IFM_POW220_TYPE,	2},
			//{IFM_EQU_TYPE,		2},
			//{IFM_FAN_TYPE,		4},
		};
	
	/*assert */
	if(pgpnDevCoreInfo == NULL)
	{
		return GPN_DEV_DRV_GEN_ERR;
	}

	portTypeNum = sizeof(VXFixPortTp)/sizeof(VXFixPortTp[0]);
	pstIfInfo = (stIfInfo *)VXFixPortTp;

	/*bulid SUITABLE space */
	if(portTypeNum <= pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)
	{
		pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = pgpnDevCoreInfo->fixPortInfo.maxPortTpNum;
		pgpnDevCoreInfo->fixPortInfo.portTpNum = portTypeNum;

		memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0,\
			(pgpnDevCoreInfo->fixPortInfo.maxPortTpNum)*sizeof(stIfInfo));
		memcpy(pgpnDevCoreInfo->fixPortInfo.pfixPort, pstIfInfo, portTypeNum*sizeof(stIfInfo));
	}
	else
	{
		GPN_DEV_DRV_PRINT(GPN_DEV_DRV_AGP, "%s(%d) : portTpNum(%d) over than maxPortTpNum(%d)]\n\r",\
			__FUNCTION__, getpid(), portTypeNum, pgpnDevCoreInfo->fixPortInfo.maxPortTpNum);
		return GPN_DEV_DRV_GEN_ERR;
	}
	
	return GPN_DEV_DRV_GEN_OK;
}

UINT32 gpnDevDrvVXGetMgtInfo(void)
{
	return GPN_DEV_DRV_GEN_OK;
}





#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_GLOBAL_DEV_DRIVER_C_ */

