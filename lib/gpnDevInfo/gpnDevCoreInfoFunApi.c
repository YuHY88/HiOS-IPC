/**********************************************************
* file name: gpnDevCoreInfoFunApi.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-09-02
* function: 
*    define GPN_DEV_INFO comm API
* modify:
*
***********************************************************/
#ifndef _GPN_DEV_CORE_INFO_FUN_API_C_
#define _GPN_DEV_CORE_INFO_FUN_API_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


#include "devCoreInfo/gpnDevCoreInfoFunApi.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

extern stGpnDevInfo *pgpnDevCoreInfo;

UINT32 gpnDevApiGetDevIndex(UINT32 *pdevIndex)
{
	UINT32 devType;
	
	/*get device type */
	gpnDevApiGetDevType(&devType, NULL, 0);
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : get device type(%d) use Api!\n\r",\
		__FUNCTION__, getpid(), devType);

	/*use level, slot, devictType to creat device index */
	//*pdevIndex = DeviceIndex_Create(level, slot);
	*pdevIndex = DeviceIndex_Create(1);		//modify by lipf, 2018/4/23	
	
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : get devIndex(%08x) use Api!\n\r",\
		__FUNCTION__, getpid(), *pdevIndex);
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiGetDevType(UINT32 *devType, char *devStrType, UINT32 strLen)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(devType != NULL)
	{
		*devType = pgpnDevCoreInfo->devBaseInfo.devType;
	}
	
	if(devStrType != NULL)
	{
		if(strLen >= GPN_DEV_MAX_TYPE_STR_LEN)
		{
			/*clear buffer first */
			memset(devStrType, 0, strLen);
			snprintf(devStrType, strLen, "%s", pgpnDevCoreInfo->devBaseInfo.devStrType);
		}
		else
		{
			return GPN_DEV_GEN_ERR;
		}
	}
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiGetSelfSoltNum(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	return pgpnDevCoreInfo->devBaseInfo.selfSlot;
}

UINT32 gpnDevApiGetNMxSoltNum(UINT32 *pNMaSlot, UINT32 *pNMbSlot)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(pNMaSlot != NULL)
	{
		*pNMaSlot = pgpnDevCoreInfo->devBaseInfo.NMaSlot;
	}
	
	if(pNMbSlot != NULL)
	{
		*pNMbSlot = pgpnDevCoreInfo->devBaseInfo.NMbSlot;
	}
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiGetProtocolMac(char *mac, UINT32 len)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if( (mac != NULL) &&\
		(len >= GPN_DEV_MAC_LEN) )
	{
		memcpy(mac, pgpnDevCoreInfo->devBaseInfo.protocol_mac, GPN_DEV_MAC_LEN);
		printf("gpnDevApiGetProtocolMac : %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[0],\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[1],\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[2],\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[3],\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[4],\
			pgpnDevCoreInfo->devBaseInfo.protocol_mac[5]);
	}
	return GPN_DEV_GEN_OK;
}
			
UINT32 gpnDevApiGetProtocolMacFromFile(char *mac, UINT32 len)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if( (mac != NULL) &&\
		(len >= GPN_DEV_MAC_LEN) )
	{
		gpnDevBaseInfoFile2DataSt(&(pgpnDevCoreInfo->devBaseInfo));
		memcpy(mac, pgpnDevCoreInfo->devBaseInfo.protocol_mac, GPN_DEV_MAC_LEN);
	}

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiSaveProtocolMac(char *mac)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(mac != NULL)
	{
		memcpy(pgpnDevCoreInfo->devBaseInfo.protocol_mac, mac, GPN_DEV_MAC_LEN);

		/*modify gpn_dev_info file, re-write ROM and RAM file */
		gpnDevBaseInfoInitReCreat(NULL);
		gpnDevCoreInfoDateSt2RamRenew();
	}

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiIsNMaSlot(void)
{
	UINT32 selfSlot;
	UINT32 NMaSlot;

	selfSlot = 0;
	NMaSlot = 0;

	selfSlot = gpnDevApiGetSelfSoltNum();
	gpnDevApiGetNMxSoltNum(&NMaSlot, NULL);

	if(selfSlot == NMaSlot)
	{
		return GPN_DEV_GEN_OK;
	}
	else
	{
		return GPN_DEV_GEN_ERR;
	}
}

UINT32 gpnDevApiIsNMbSlot(void)
{
	UINT32 selfSlot;
	UINT32 NMbSlot;

	selfSlot = 0;
	NMbSlot = 0;

	selfSlot = gpnDevApiGetSelfSoltNum();
	gpnDevApiGetNMxSoltNum(NULL, &NMbSlot);

	if(selfSlot == NMbSlot)
	{
		return GPN_DEV_GEN_OK;
	}
	else
	{
		return GPN_DEV_GEN_ERR;
	}
}

UINT32 gpnDevApiIsNMxSlot(void)
{
	UINT32 selfSlot;
	UINT32 NMaSlot;
	UINT32 NMbSlot;

	selfSlot = 0;
	NMaSlot = 0;
	NMbSlot = 0;

	selfSlot = gpnDevApiGetSelfSoltNum();
	gpnDevApiGetNMxSoltNum(&NMaSlot, &NMbSlot);

	if( (selfSlot == NMaSlot) ||\
		(selfSlot == NMbSlot) )
	{
		return GPN_DEV_GEN_OK;
	}
	else
	{
		return GPN_DEV_GEN_ERR;
	}
}

UINT32 gpnDevApiIsSubSlot(void)
{
	UINT32 selfSlot;
	UINT32 NMaSlot;
	UINT32 NMbSlot;

	selfSlot = 0;
	NMaSlot = 0;
	NMbSlot = 0;

	selfSlot = gpnDevApiGetSelfSoltNum();
	gpnDevApiGetNMxSoltNum(&NMaSlot, &NMbSlot);

	if( (selfSlot != NMaSlot) &&\
		(selfSlot != NMbSlot) )
	{
		return GPN_DEV_GEN_OK;
	}
	else
	{
		return GPN_DEV_GEN_ERR;
	}
}

UINT32 gpnDevApiGetSlotIp(UINT32 slot, UINT32 *ip, UINT16 *pUPort, UINT32 protNum)
{
	UINT32 i;
	UINT32 j;
	
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(protNum > GPN_DEV_CI_PER_SLOT_UPORTS)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d)getpid() : ask portNum(%d) too much, max is (%d), err!\n\r",\
			__FUNCTION__, getpid(), protNum, GPN_DEV_CI_PER_SLOT_UPORTS);
		
		return GPN_DEV_GEN_ERR;
	}
	
	for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
	{
		if(pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum == slot)
		{
			if(ip != NULL)
			{
				*ip = pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4;
			}

			for(j=0;j<protNum;j++)
			{
				*(pUPort+j) = pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[j];
			}

			return GPN_DEV_GEN_OK;
		}
	}

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : slot(%d) don't in GPN_DEV_INFO!\n\r",\
		__FUNCTION__, getpid(), slot);
	return GPN_DEV_GEN_ERR;
}

UINT32 gpnDevApiGetDevFixPortInfo(UINT32 *pportTpNum, stIfInfo *pstIfInfo, UINT32 pstIfInfoSpSize)
{
	UINT32 infoSize;
	
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(pportTpNum != NULL)
	{
		*pportTpNum = pgpnDevCoreInfo->fixPortInfo.portTpNum;
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : portTpNum %d!\n\r",\
			__FUNCTION__, getpid(), *pportTpNum);
	}

	infoSize = pgpnDevCoreInfo->fixPortInfo.portTpNum * sizeof(stIfInfo);
	if(pstIfInfo != NULL)
	{
		if(pstIfInfoSpSize < infoSize)
		{
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : user space too small(%d < %d)!\n\r",\
				__FUNCTION__, getpid(), pstIfInfoSpSize, infoSize);

			return GPN_DEV_GEN_ERR;
		}

		memcpy(pstIfInfo, pgpnDevCoreInfo->fixPortInfo.pfixPort, infoSize);
	}
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevApiGetMgtInbandIp(UINT32 *inbandIp)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	if(inbandIp != NULL)
	{
		*inbandIp = pgpnDevCoreInfo->devMgtInfo.inbandIp;
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : inbandIp %d.%d.%d.%d!\n\r",\
			__FUNCTION__, getpid(),\
			(UINT8)((*inbandIp >> 24) & 0x000000FF),\
			(UINT8)((*inbandIp >> 16) & 0x000000FF),\
			(UINT8)((*inbandIp >>  8) & 0x000000FF),\
			(UINT8)((*inbandIp >>  0) & 0x000000FF));
	}
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevApiSaveMgtInbandIp(UINT32 inbandIp)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	pgpnDevCoreInfo->devMgtInfo.inbandIp = inbandIp;

	/*modify gpn_dev_info file, re-write ROM and RAM file */
	gpnDevMgtInfoInitReCreat(NULL);
	gpnDevCoreInfoDateSt2RamRenew();

	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevApiSaveMgtL2Info(UINT32 inbandVlan, UINT32 ib1QPri, UINT8 *ibMac)
{
	if(pgpnDevCoreInfo == NULL)
	{
		gpnDevInfoInit();
	}

	pgpnDevCoreInfo->devMgtInfo.inbandVlan = inbandVlan;
	pgpnDevCoreInfo->devMgtInfo.ib1QPri = ib1QPri;
	if(ibMac != NULL)
	{
		memcpy(pgpnDevCoreInfo->devMgtInfo.ibMac, ibMac, GPN_DEV_MAC_LEN);
	}

	/*modify gpn_dev_info file, re-write ROM and RAM file */
	gpnDevMgtInfoInitReCreat(NULL);
	gpnDevCoreInfoDateSt2RamRenew();

	return GPN_DEV_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_DEV_CORE_INFO_C_ */

