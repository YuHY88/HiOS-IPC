/**********************************************************
* file name: gpnDevCoreInfo.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-07-08
* function: 
*    define GPN_DEV_INFO comm API
* modify:
*
***********************************************************/
#ifndef _GPN_DEV_CORE_INFO_C_
#define _GPN_DEV_CORE_INFO_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "socketComm/gpnSpecialWarrper.h"
#include "socketComm/gpnGlobalPortIndexDef.h"

#include "devCoreInfo/gpnDevCoreInfo.h"
#include "devCoreInfo/gpnGlobalDevTypeDef.h"
#include "devCoreInfo/gpnGlobalDevDriver.h"

/*GPN_DEV_INFO devie info cfg file */
static char gDevBaseInfo[] = GPN_DEV_UP_ROM_DIR(gpnDevCoreInfo.conf);
static char gDevCommAddr[] = GPN_DEV_UP_ROM_DIR(gpnSocketCommSubDevIp.conf);
static char gDevFixIf[] = GPN_DEV_UP_ROM_DIR(gpnDevFixInterface.conf);
/*GPN_DEV_INFO run device info show in RAM status file */
static char gRamDevCoreInfo[] = GPN_DEV_UP_RAM_DIR(gpnDevCoreInfoStData.txt);

/*mgt info save in RAM */
static char gDevMgtInfo[] = GPN_DEV_UP_RAM_DIR(gpnDevMgtInfo.conf);


/*GPN_DEV_INFO fix interface info: portType to portStrTyp */
stGpnDevPortTypeNameTp ggpnDevPortTypeName[] =
{
	/* 0*/{IFM_ALL_TYPE,          "IFM_ALL_TYPE",            "ALL"},
	/* 1*/{IFM_FUN_ETH_TYPE,      "IFM_FUN_ETH_TYPE",        "ETH"},	
	/* 2*/{IFM_SOFT_TYPE,         "IFM_SOFT_TYPE",           "SOFT"},
	/* 3*/{IFM_STMN_TYPE,         "IFM_STMN_TYPE",           "STMN"},
	/* 4*/{IFM_VC4_TYPE,          "IFM_VC4_TYPE",            "VC4"},
	/* 5*/{IFM_VC3_TYPE,          "IFM_VC3_TYPE",            "VC3"},
	/* 6*/{IFM_VC12_TYPE,         "IFM_VC12_TYPE",           "VC12"},
	/* 7*/{IFM_VCG_TYPE,          "IFM_VCG_TYPE",            "VCG"},
	/* 8*/{IFM_VCG_LP_TYPE,       "IFM_VCG_LP_TYPE",         "VCG_LP"},
	/* 9*/{IFM_E1_TYPE,           "IFM_E1_TYPE",             "E1"},
	/*10*/{IFM_E2_TYPE,           "IFM_E2_TYPE",             "E2"},
	/*11*/{IFM_E3_TYPE,           "IFM_E3_TYPE",             "E3"},
	/*12*/{IFM_V35_TYPE,          "IFM_V35_TYPE",            "V35"},
	/*13*/{IFM_DSL_TYPE,          "IFM_DSL_TYPE",            "DSL"},
	/*14*/{IFM_ETH_TYPE,          "IFM_ETH_TYPE",            "ETH"},
	/*15*/{IFM_VS_TYPE,           "IFM_VS_TYPE",             "VS"},
	/*16*/{IFM_LSP_TYPE,          "IFM_LSP_TYPE",            "LSP"},
	/*17*/{IFM_PW_TYPE,           "IFM_PW_TYPE",             "PW"},
	/*18*/{IFM_MPLS_TYPE,         "IFM_MPLS_TYPE",           "MPLS"},
	/*19*/{IFM_VLAN_OAM_TYPE,     "IFM_VLAN_OAM_TYPE",       "VLAN_OAM"},
	/*20*/{IFM_POWN48_TYPE,       "IFM_POWN48_TYPE",         "POWN48"},
	/*21*/{IFM_EQU_TYPE,	      "IFM_EQU_TYPE",            "EQU"},
	/*22*/{IFM_ENV_TYPE,          "IFM_ENV_TYPE",            "ENV"},
	/*23*/{IFM_FAN_TYPE,          "IFM_FAN_TYPE",            "FAN"},
	/*24*/{IFM_PWE3E1_TYPE,       "IFM_PWE3E1_TYPE",         "PWE3E1"},
	/*25*/{IFM_CFM_MEP_TYPE,      "IFM_CFM_MEP_TYPE",        "CFM_MEP"},
	/*26*/{IFM_POW220_TYPE,	      "IFM_POW220_TYPE",         "POW220"},
	/*27*/{IFM_PWE3STMN_TYPE,     "IFM_PWE3STMN_TYPE",       "PWE3STMN"},
	/*28*/{IFM_ETH_CLC_IN_TYPE,   "IFM_ETH_CLC_IN_TYPE",     "ETH_CLK_IN"},
	/*29*/{IFM_ETH_CLC_OUT_TYPE,  "IFM_ETH_CLC_OUT_TYPE",    "ETH_CLK_OUT"},
	/*30*/{IFM_ETH_CLC_SUB_TYPE,  "IFM_ETH_CLC_SUB_TYPE",    "ETH_CLC_SUB"},
	/*31*/{IFM_ETH_CLC_LINE_TYPE, "IFM_ETH_CLC_LINE_TYPE",   "ETH_CLC_LINE"},
	/*32*/{IFM_ETH_SFP_TYPE,      "IFM_ETH_SFP_TYPE",        "ETH_SFP"},
	/*33*/{IFM_VPLSPW_TYPE,       "IFM_VPLSPW_TYPE",         "VPLSPW"},
	/*34*/{IFM_VUNI_TYPE,         "IFM_VUNI_TYPE",           "VUNI"},

	/*34*/{IFM_FUN_ETH_SUB_TYPE,  "IFM_FUN_ETH_TYPE",        "ETH SUB"},	//lipf add for ipran
	/*35*/{IFM_TUNNEL_TYPE,  	  "IFM_TUNNEL_TYPE",         "TUNNEL"},		//lipf add for ipran
	/*36*/{IFM_E1_SUB_TYPE,  	  "IFM_E1_SUB_TYPE",         "PDH SUB"},	//lipf add for ipran
	/*37*/{IFM_BFD_TYPE,  	  	  "IFM_BFD_TYPE",         	 "BFD"},		//lipf add for ipran
	/*38*/{IFM_TRUNK_TYPE,  	  "IFM_TRUNK_TYPE",          "TRUNK"},		//lipf add for ipran
	/*39*/{IFM_TRUNK_SUBIF_TYPE,  "IFM_TRUNK_SUBIF_TYPE",    "TRUNK SUB"},	//lipf add for ipran
	/*40*/{IFM_VS_2_TYPE,  	  	  "IFM_VS_2_TYPE",         	 "IPRAN VS"},	//lipf add for ipran
	/*41*/{IFM_LSP_2_TYPE,  	  "IFM_LSP_2_TYPE",          "IPRAN LSP"},	//lipf add for ipran
	/*42*/{IFM_PW_2_TYPE,  	  	  "IFM_PW_2_TYPE",         	 "IPRAN PW"},	//lipf add for ipran
	
	/*42+1*/{IFM_NULL_TYPE,       "IFM_NULL_TYPE",           "NULL"},
};

stGpnDevInfo *pgpnDevCoreInfo = NULL;

/*GPN_DEV_INFO device base info: devType to devStrTyp */
extern stGpnDevTypeTp gGpnDevTypeTemplat[];

UINT32 gpnDevInfoUPPathInit(void)
{
	/*used for gpn_dev_info function*/
	gpnUnifyPathCreat((char *)GPN_DEV_UP_ROM_NODE);
	gpnUnifyPathCreat((char *)GPN_DEV_UP_RAM_NODE);

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevInfoInit(void)
{
	/*creat gpn_dev_info path */
	gpnDevInfoUPPathInit();
	
	/*creat gpn deviec core info file */
	gpnDevInfoCfgFileInit();

	/*init gpn device core info from file */
	gpnDevInfoFile2DataSt();

	/*init gpn device core info from hardware */
	gpnDevInfoHardware2DataSt();

	gpnDevCoreInfoDateSt2Ram();
	/*debug 
	debugGpnDevCoreInfoDateStPrint();*/

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : OK! \n\r",\
		__FUNCTION__, getpid());
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevInfoCfgFileInit(void)
{	
	/*
		device core infomation have 3 part :
		1 base infomation, like device type ...;
		2 communication addr infomation, like ip, port, mac ...(type no relation)
		3 fix interface info(type about)

		not device core information, just use this method :
		4 mgt info : inband info
	*/

	/*
		if process check dirctory 'gpn_dev_info' not exit, 
		gpn_dev_info creat default fromat config file(no valid info), 
		if soft can not get valid info after some where, process will exit, 
		usr should modify to correct info, then re-startup process
	*/
	
	/*part 1 : device base infomation config file creat */
	if(access(gDevBaseInfo, F_OK | R_OK) == 0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s exist\n\r",\
			__FUNCTION__, getpid(), gDevBaseInfo);
	}
	else
	{
		/*file not exsit, so creat it*/
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s not exist, creat default fromat base info!\n\r",\
			__FUNCTION__, getpid(), gDevBaseInfo);

		/*here we should check device type */
		gpnDevBaseInfoInitCreat(gDevBaseInfo);
	}

	/*part 2 : device communication infomation config file creat */
	if(access(gDevCommAddr, F_OK | R_OK) ==0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s exist\n\r",\
			__FUNCTION__, getpid(), gDevCommAddr);
	}
	else
	{
		/*file not exsit, so creat it*/
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s not exist, creat default fromat base info!\n\r",\
			__FUNCTION__, getpid(), gDevCommAddr);

		gpnDevCommInfoInitCreat(gDevCommAddr);
	}

	/*part 3 : fix interface infomation config file creat */
	if(access(gDevFixIf, F_OK | R_OK) ==0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s exist\n\r",\
			__FUNCTION__, getpid(), gDevFixIf);
	}
	else
	{
		/*file not exsit, so creat it*/
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s not exist, creat default fromat base info!\n\r",\
			__FUNCTION__, getpid(), gDevFixIf);

		gpnDevFixIfInfoInitCreat(gDevFixIf);
	}

	/*part 4 : mgt infomation config file creat */
	if(access(gDevMgtInfo, F_OK | R_OK) ==0)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s exist\n\r",\
			__FUNCTION__, getpid(), gDevMgtInfo);
	}
	else
	{
		/*file not exsit, so creat it*/
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s not exist, creat default fromat base info!\n\r",\
			__FUNCTION__, getpid(), gDevMgtInfo);

		gpnDevMgtInfoInitCreat(gDevMgtInfo);
	}
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevBaseInfoInitCreat(char *pbaseInfoPath)
{
	UINT32 i;
	UINT32 devTpNum;
	FILE *fp;
	char *pbuff;
	char buff[GPN_DEV_INFO_FILE_LEN];
	/*char p_mac[GPN_DEV_MAC_LEN];*/

	if(pbaseInfoPath == NULL)
	{
		pbaseInfoPath = gDevBaseInfo;
	}
	
	fp = fopen(pbaseInfoPath, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), pbaseInfoPath);
		return GPN_DEV_GEN_ERR;
	}
	
	pbuff = buff;

	/*device string type dispaly */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n#********************************\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "# %s created by p-%d \n", pbaseInfoPath, getpid());
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "#devTypeStr define like this :\n#");
	pbuff = pbuff + strlen(pbuff);
	
	devTpNum = gpnGlobalDevTpNum();
	for(i=0;i<devTpNum;i++)
	{
		if(((i+1)%4) == 0)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "\n#");
			pbuff = pbuff + strlen(pbuff);
		}
		snprintf(pbuff, GPN_DEV_INFO_LINE, " %s ", gGpnDevTypeTemplat[i].devStrType);
		pbuff = pbuff + strlen(pbuff);
	}
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n");
	pbuff = pbuff + strlen(pbuff);
	
	if(pgpnDevCoreInfo != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use dataStruct info!\n\r",
			__FUNCTION__, getpid(), pbaseInfoPath);
		
		/*creat file */
		snprintf(pbuff, GPN_DEV_INFO_LINE, "# this file contains device base infomaion\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n",\
			pgpnDevCoreInfo->devBaseInfo.devType);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devStrType %s\n",\
			pgpnDevCoreInfo->devBaseInfo.devStrType);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "selfSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.selfSlot);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "NMaSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.NMaSlot);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "NMbSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.NMbSlot);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "SWaSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.SWaSlot);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "SWbSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.SWbSlot);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "maxSlot %d\n",\
			pgpnDevCoreInfo->devBaseInfo.maxSlot);
		pbuff = pbuff + strlen(pbuff);

		snprintf(pbuff, GPN_DEV_INFO_LINE, "protocol-mac %02x:%02x:%02x:%02x:%02x:%02x\n",\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[0],\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[1],\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[2],\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[3],\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[4],\
			(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[5]);
		pbuff = pbuff + strlen(pbuff);
	}
	else
	{
		/*creat file*/
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use default info!\n\r",
			__FUNCTION__, getpid(), pbaseInfoPath);
		
		snprintf(pbuff, GPN_DEV_INFO_LINE, "# this file contains device base infomaion\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n", GPN_DEV_TYPE_NULL);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devStrType %s\n", "GPN_DEV_NULL");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "selfSlot 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "NMaSlot 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "NMbSlot 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "SWaSlot 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "SWbSlot 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "maxSlot 0\n");
		pbuff = pbuff + strlen(pbuff);

		/*init protocol mac as eth0 mac */
		/*gpnDevDrvGetEth0Mac(p_mac);*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "protocol-mac 00:00:00:00:00:00\n");
		pbuff = pbuff + strlen(pbuff);
	}
	
	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevCommInfoInitCreat(char *pcommInfoPath)
{
	UINT32 i;
	FILE *fp;
	char *pbuff;
	char buff[GPN_DEV_INFO_FILE_LEN];

	if(pcommInfoPath == NULL)
	{
		pcommInfoPath = gDevCommAddr;
	}
	
	fp = fopen(pcommInfoPath, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), pcommInfoPath);
		return GPN_DEV_GEN_ERR;
	}
	
	pbuff = buff;

	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n# %s created by p-%d \n",\
		pcommInfoPath, getpid());
	pbuff = pbuff + strlen(pbuff);
	
	if(pgpnDevCoreInfo != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use dataStruct info!\n\r",
			__FUNCTION__, getpid(), pcommInfoPath);
		
		/*creat file*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n",\
			pgpnDevCoreInfo->devCommInfo.devType);
		pbuff = pbuff + strlen(pbuff);
		
		snprintf(pbuff, GPN_DEV_INFO_LINE, "# cfg line syntaxs :slot n ip a.b.c.d port x y z u v w\n");
		pbuff = pbuff + strlen(pbuff);
		
		for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "slot %-3d ip %d.%d.%d.%d port %d %d %d %d %d %d\n",\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum,\
				(pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4 >> 24) & 0x000000FF,\
				(pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4 >> 16) & 0x000000FF,\
				(pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4 >>  8) & 0x000000FF,\
				(pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4 >>  0) & 0x000000FF,\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT1],\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT2],\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT3],\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT4],\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT5],\
				pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[GPN_DEV_CI_PER_SLOT_UPORT6]);
			
			pbuff = pbuff + strlen(pbuff);
		}
	}
	else
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use default info!\n\r",
			__FUNCTION__, getpid(), pcommInfoPath);
		
		/*creat file*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n", GPN_DEV_TYPE_NULL);
		pbuff = pbuff + strlen(pbuff);
		
		snprintf(pbuff, GPN_DEV_INFO_LINE, "# cfg line syntaxs :slot n ip a.b.c.d port x y z u v w\n");
		pbuff = pbuff + strlen(pbuff);

		for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "slot %-3d ip 10.0.0.%d port %d %d %d %d %d %d\n",\
				i+1,\
				i+1,\
				GPN_DEV_CI_DEFPORT_START+0*GPN_DEV_CI_MAX_SLOT+i,\
				GPN_DEV_CI_DEFPORT_START+1*GPN_DEV_CI_MAX_SLOT+i,\
				GPN_DEV_CI_DEFPORT_START+2*GPN_DEV_CI_MAX_SLOT+i,\
				GPN_DEV_CI_DEFPORT_START+3*GPN_DEV_CI_MAX_SLOT+i,\
				GPN_DEV_CI_DEFPORT_START+4*GPN_DEV_CI_MAX_SLOT+i,\
				GPN_DEV_CI_DEFPORT_START+5*GPN_DEV_CI_MAX_SLOT+i);
			pbuff = pbuff + strlen(pbuff);
		}
	}

	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);
	
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevFixIfInfoInitCreat(char *pdevFixIf)
{
	UINT32 i;
	UINT32 j;
	UINT32 maxPortType;
	FILE *fp;
	char *pbuff;
	char buff[GPN_DEV_INFO_FILE_LEN];

	if(pdevFixIf == NULL)
	{
		pdevFixIf = gDevFixIf;
	}
	
	fp = fopen(pdevFixIf, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), pdevFixIf);
		return GPN_DEV_GEN_ERR;
	}
	
	pbuff = buff;
	
	/*creat file*/
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n# %s created by p-%d \n",\
		pdevFixIf, getpid());
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "# cfg line syntaxs : \"portTypeStr\" \"num\"\n");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "# portTypeStr define like this :\n#");
	pbuff = pbuff + strlen(pbuff);

	/*device core info support portType Num */
	maxPortType = sizeof(ggpnDevPortTypeName)/sizeof(ggpnDevPortTypeName[0]);
	for(i=0;i<maxPortType;i++)
	{
		if(((i+1)%4) == 0)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "\n#");
			pbuff = pbuff + strlen(pbuff);
		}
		snprintf(pbuff, GPN_DEV_INFO_LINE, " %s ", ggpnDevPortTypeName[i].portTypeStr);
		pbuff = pbuff + strlen(pbuff);
	}
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n");
	pbuff = pbuff + strlen(pbuff);

	if(pgpnDevCoreInfo != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use dataStruct info!\n\r",
			__FUNCTION__, getpid(), pdevFixIf);

		/*creat core file info */
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n",\
			pgpnDevCoreInfo->fixPortInfo.devType);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "maxPortType %d\n", maxPortType);
		pbuff = pbuff + strlen(pbuff);
		
		for(i=0;i<pgpnDevCoreInfo->fixPortInfo.portTpNum;i++)
		{
			/*portType to portTypeStr */
			for(j=0;j<maxPortType;j++)
			{
				if(ggpnDevPortTypeName[j].portType == pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType)
				{
					snprintf(pbuff, GPN_DEV_INFO_LINE, "%s %d\n", ggpnDevPortTypeName[j].portTypeStr,\
						pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portNum);
					pbuff = pbuff + strlen(pbuff);
					break;
				}
			}

			if(j >= maxPortType)
			{
				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : portType(%08x) not change to str!\n\r",\
					__FUNCTION__, getpid(), pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType);
				snprintf(pbuff, GPN_DEV_INFO_LINE, "#%s(%d) : portType(%08x) not change to str!\n\r",\
					__FUNCTION__, getpid(), pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType);
				pbuff = pbuff + strlen(pbuff);
				break;
			}
		}
	}
	else
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use default info!\n\r",
			__FUNCTION__, getpid(), pdevFixIf);

		/*creat file core info*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n", GPN_DEV_TYPE_NULL);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "maxPortType %d\n", maxPortType);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "IFM_FUN_ETH_TYPE 2\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "IFM_SOFT_TYPE    1\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "IFM_EQU_TYPE     1\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "IFM_POWN48_TYPE  2\n");
		pbuff = pbuff + strlen(pbuff);
	}

	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevMgtInfoInitCreat(char *pdevMgtInfo)
{
	UINT32 inbandIp;
	FILE *fp;
	char *pbuff;
	char buff[GPN_DEV_INFO_FILE_LEN];

	if(pdevMgtInfo == NULL)
	{
		pdevMgtInfo = gDevMgtInfo;
	}
	
	fp = fopen(pdevMgtInfo, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), pdevMgtInfo);
		return GPN_DEV_GEN_ERR;
	}
	
	pbuff = buff;
	
	/*creat file*/
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n# %s created by p-%d \n",\
		pdevMgtInfo, getpid());
	pbuff = pbuff + strlen(pbuff);

	if(pgpnDevCoreInfo != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use dataStruct info!\n\r",
			__FUNCTION__, getpid(), pdevMgtInfo);
		
		/*creat core file info */
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n",\
			pgpnDevCoreInfo->fixPortInfo.devType);
		pbuff = pbuff + strlen(pbuff);
		inbandIp = pgpnDevCoreInfo->devMgtInfo.inbandIp;
		/*printf("liuyf debug inbandIp %08x\n\r", inbandIp);*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandIp %d.%d.%d.%d\n",\
			(UINT8)((inbandIp >> 24) & 0x000000FF),\
			(UINT8)((inbandIp >> 16) & 0x000000FF),\
			(UINT8)((inbandIp >>  8) & 0x000000FF),\
			(UINT8)((inbandIp >>  0) & 0x000000FF));
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandVlan %d\n",\
			pgpnDevCoreInfo->devMgtInfo.inbandVlan);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "ib1QPri %d\n",\
			pgpnDevCoreInfo->devMgtInfo.ib1QPri);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "ibMac %02x:%02x:%02x:%02x:%02x:%02x\n",\
			pgpnDevCoreInfo->devMgtInfo.ibMac[0],\
			pgpnDevCoreInfo->devMgtInfo.ibMac[1],\
			pgpnDevCoreInfo->devMgtInfo.ibMac[2],\
			pgpnDevCoreInfo->devMgtInfo.ibMac[3],\
			pgpnDevCoreInfo->devMgtInfo.ibMac[4],\
			pgpnDevCoreInfo->devMgtInfo.ibMac[5]);
		pbuff = pbuff + strlen(pbuff);

	}
	else
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : creat \"%s\" use default info!\n\r",
			__FUNCTION__, getpid(), pdevMgtInfo);

		/*creat file core info*/
		snprintf(pbuff, GPN_DEV_INFO_LINE, "devType %d\n", GPN_DEV_TYPE_NULL);
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandIp 0.0.0.0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandVlan 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "ib1QPri 0\n");
		pbuff = pbuff + strlen(pbuff);
		snprintf(pbuff, GPN_DEV_INFO_LINE, "ibMac 00:00:00:00:00:00\n");
		pbuff = pbuff + strlen(pbuff);
	}

	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevBaseInfoInitReCreat(char *pbaseInfoPath)
{
	stGpnDevBaseInfo baseInfo;

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : get data from \"%s\"!\n\r",
		__FUNCTION__, getpid(), pbaseInfoPath);
	gpnDevBaseInfoFile2DataSt(&baseInfo);

	if(memcmp(&(pgpnDevCoreInfo->devBaseInfo), &baseInfo, sizeof(stGpnDevBaseInfo)) != 0)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : re-creat \"%s\"!\n\r",
			__FUNCTION__, getpid(), pbaseInfoPath);
		gpnDevBaseInfoInitCreat(pbaseInfoPath);
	}
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevCommInfoInitReCreat(char *pcommInfoPath)
{
	stGpnDevCommInfo devCommInfo;

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : get data from \"%s\"!\n\r",
		__FUNCTION__, getpid(), pcommInfoPath);
	gpnDevCommInfoFile2DataSt(&devCommInfo);

	if(memcmp(&(pgpnDevCoreInfo->devCommInfo), &devCommInfo, sizeof(stGpnDevCommInfo)) != 0)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : re-creat \"%s\"!\n\r",
			__FUNCTION__, getpid(), pcommInfoPath);
		gpnDevCommInfoInitCreat(pcommInfoPath);
	}
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevFixIfInfoInitReCreat(char *pdevFixIf)
{
	stDevFixIf fixPortInfo;
	UINT32 sysPortTpNum;
	UINT32 len;
	
	/*fix fixPortInfo struct */
	sysPortTpNum = sizeof(ggpnDevPortTypeName)/sizeof(ggpnDevPortTypeName[0]);
	fixPortInfo.sysPortTpNum = sysPortTpNum;
	fixPortInfo.maxPortTpNum = sysPortTpNum;
	
	len = sysPortTpNum * sizeof(stIfInfo);
	fixPortInfo.pfixPort = (stIfInfo *)malloc(len);
	if(fixPortInfo.pfixPort == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : malloc err!\n\r",
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR;
	}
	memset(fixPortInfo.pfixPort, 0, len);

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : get data from \"%s\"!\n\r",
		__FUNCTION__, getpid(), pdevFixIf);
	gpnDevFixIfInfoFile2DataSt(&fixPortInfo);
	
	if( (memcmp(&(pgpnDevCoreInfo->fixPortInfo), &fixPortInfo, (sizeof(stDevFixIf) - 4)) != 0) ||\
		(memcmp(pgpnDevCoreInfo->fixPortInfo.pfixPort, fixPortInfo.pfixPort, len) != 0) )
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : re-creat \"%s\"!\n\r",
			__FUNCTION__, getpid(), pdevFixIf);
		gpnDevFixIfInfoInitCreat(pdevFixIf);
	}

	free(fixPortInfo.pfixPort);
	
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevMgtInfoInitReCreat(char *pdevMgtInfo)
{
	stDevMgtInfo devMgtInfo;

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : get data from \"%s\"!\n\r",
		__FUNCTION__, getpid(), pdevMgtInfo);
	
	gpnDevMgtInfoFile2DataSt(&devMgtInfo);
	
	if(memcmp(&(pgpnDevCoreInfo->devMgtInfo), &devMgtInfo, sizeof(stDevMgtInfo)) != 0)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : re-creat \"%s\"!\n\r",
			__FUNCTION__, getpid(), pdevMgtInfo);
		gpnDevMgtInfoInitCreat(pdevMgtInfo);
	}
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevInfoFile2DataSt(void)
{
	/*malloc memrey space, then init memrey */
	gpnDevCoreInfoSpaceInit();

	/*
		device core infomation have 3 part :
		1 base infomation, like device type ...;
		2 communication addr infomation, like ip, port, mac ...
		3 fix interface info(type about)

		not device core information :
		4 mgt info : inband info
	*/
	
	if(pgpnDevCoreInfo != NULL)
	{
		/*part 1 : device base infomation fromt config file to data struct */
		gpnDevBaseInfoFile2DataSt(&(pgpnDevCoreInfo->devBaseInfo));
		
		/*debug print 
		debugGpnDevCoreInfoDateStPrint();*/
		
		/*part 2 : device communication infomation config file to data struct */
		gpnDevCommInfoFile2DataSt(&(pgpnDevCoreInfo->devCommInfo));

		/*debug print 
		debugGpnDevCoreInfoDateStPrint();*/

		/*part 3 : device fix port infomation config file to data struct */
		gpnDevFixIfInfoFile2DataSt(&(pgpnDevCoreInfo->fixPortInfo));

		/*debug print 
		debugGpnDevCoreInfoDateStPrint();*/
		
		/*part 4 :mgt infomation config file to data struct */
		gpnDevMgtInfoFile2DataSt(&(pgpnDevCoreInfo->devMgtInfo));

		/*debug print 
		debugGpnDevCoreInfoDateStPrint();*/
		
	}
	else
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL\n\r",\
			__FUNCTION__, getpid());
	}
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevCoreInfoSpaceInit(void)
{
	UINT32 i;
	UINT32 j;
	UINT32 sysPortTpNum;
	char ipStr[20];
	
	if(pgpnDevCoreInfo != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo err! already mollac!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	pgpnDevCoreInfo = (stGpnDevInfo *)malloc(sizeof(stGpnDevInfo));
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : malloc err!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}
	memset(pgpnDevCoreInfo, 0, sizeof(stGpnDevInfo));

	/*part 1 : core info init */
	/*init pgpnDevCoreInfo info : empyt info */
	pgpnDevCoreInfo->devBaseInfo.devType = GPN_DEV_TYPE_NULL;
	snprintf(pgpnDevCoreInfo->devBaseInfo.devStrType, GPN_DEV_MAX_TYPE_STR_LEN,\
		"%s", "GPN_DEV_NULL");
	
	pgpnDevCoreInfo->devBaseInfo.selfSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.NMaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.NMbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWaSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.SWbSlot = 0;
	pgpnDevCoreInfo->devBaseInfo.maxSlot = 0;

	/*init protocol mac as eth0 mac */
	gpnDevDrvGetEth0Mac(pgpnDevCoreInfo->devBaseInfo.protocol_mac);
	
	/*part 2 : comm info init */
	pgpnDevCoreInfo->devCommInfo.devType = GPN_DEV_TYPE_NULL;
	for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
	{
		pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum = i+1;

		snprintf(ipStr, 20, "10.0.0.%d", i+1);
		pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4= inet_addr(ipStr);
		for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
		{
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[j] = 
				GPN_DEV_CI_DEFPORT_START+j*GPN_DEV_CI_MAX_SLOT+i;
		}
	}

	/*part 3 : fix port init  */
	/*empty space */
	sysPortTpNum = sizeof(ggpnDevPortTypeName)/sizeof(ggpnDevPortTypeName[0]);
	pgpnDevCoreInfo->fixPortInfo.devType = GPN_DEV_TYPE_NULL;
	pgpnDevCoreInfo->fixPortInfo.sysPortTpNum = sysPortTpNum;
	pgpnDevCoreInfo->fixPortInfo.maxPortTpNum = sysPortTpNum;
	pgpnDevCoreInfo->fixPortInfo.portTpNum = 0;
	/*malloc MAX space , pfixPort == NULL*/
	pgpnDevCoreInfo->fixPortInfo.pfixPort = (stIfInfo *)malloc(sysPortTpNum *sizeof(stIfInfo));
	if(pgpnDevCoreInfo->fixPortInfo.pfixPort == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : malloc pfixPort err!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR;
	}
	memset(pgpnDevCoreInfo->fixPortInfo.pfixPort, 0, sysPortTpNum *sizeof(stIfInfo));

	/*part 4 : mgt info init  */
	pgpnDevCoreInfo->devMgtInfo.devType = GPN_DEV_TYPE_NULL;
	pgpnDevCoreInfo->devMgtInfo.inbandIp = 0;
	pgpnDevCoreInfo->devMgtInfo.inbandVlan = 0;
	pgpnDevCoreInfo->devMgtInfo.ib1QPri = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[0] = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[1] = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[2] = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[3] = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[4] = 0;
	pgpnDevCoreInfo->devMgtInfo.ibMac[5] = 0;
	
	/*test print 
	debugGpnDevCoreInfoDateStPrint();*/
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevBaseInfoFile2DataSt(stGpnDevBaseInfo *pgpnDevBaseInfo)
{
	FILE *fp;
	UINT32 i;
	UINT32 slot;
	UINT32 devType;
	UINT32 optRsl;
	UINT32 ddMac[GPN_DEV_MAC_LEN];
	char *cfgStr;
	char *info;
	char baseInfo[GPN_DEV_INFO_LINE];

	if(pgpnDevBaseInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevBaseInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	/*clear data struct to 0 */
	memset(pgpnDevBaseInfo, 0, sizeof(stGpnDevBaseInfo));
	
	if(access(gDevBaseInfo,F_OK | R_OK) < 0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : access %s err!\n\r",\
			__FUNCTION__, getpid(), gDevBaseInfo);
		
		return GPN_DEV_GEN_ERR;
	}

	fp = fopen(gDevBaseInfo, "rb");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gDevBaseInfo);
		return GPN_DEV_GEN_ERR;
	}

	/*read config from file result */
	optRsl = GPN_DEV_GEN_OK;
	
	while(gpnFgets(baseInfo, GPN_DEV_INFO_LINE, fp) != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) :  base info line : %s\n\r",\
			__FUNCTION__, getpid(), baseInfo);
		if( (baseInfo[0] == '#') ||\
			(baseInfo[0] == '\0') ||\
			(baseInfo[0] == '\n') ||\
			(baseInfo[0] == '\r') )
		{
			continue;
		}

		cfgStr = NULL;
		/*info type */
		for(i=0;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] != ' ') &&\
				(baseInfo[i] != '\t') )
			{
				cfgStr = &(baseInfo[i]);
				break;
			}
		}
		
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] == ' ') ||\
				(baseInfo[i] == '\t') )
			{
				baseInfo[i] = '\0';
				break;
			}
		}
		
		info = NULL;
		/*info data */
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] != ' ') &&\
				(baseInfo[i] != '\t') )
			{
				info = &(baseInfo[i]);
				break;
			}
		}
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] == ' ') ||\
				(baseInfo[i] == '\t') ||\
				(baseInfo[i] == '\r') ||\
				(baseInfo[i] == '\n') ||\
				(baseInfo[i] == '\0') )
			{
				baseInfo[i] = '\0';
				break;
			}
		}

		if( (info == NULL) ||
			(cfgStr == NULL) )
		{
			optRsl = GPN_DEV_GEN_ERR;
		}
		else if(strncmp (cfgStr, "devType", 10) == 0)
		{
			if(sscanf((const char *)info, "%d", &devType) == 1)
			{
				pgpnDevBaseInfo->devType = devType;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "devStrType", 10) == 0)
		{
			snprintf(pgpnDevBaseInfo->devStrType,\
				GPN_DEV_MAX_TYPE_STR_LEN, "%s", info);
		}
		else if(strncmp (cfgStr, "selfSlot", 8) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->selfSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "NMaSlot", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->NMaSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "NMbSlot", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->NMbSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "SWaSlot", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->SWaSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "SWbSlot", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->SWbSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "maxSlot", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &slot) == 1)
			{
				pgpnDevBaseInfo->maxSlot = slot;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "protocol-mac", 12) == 0)
		{
			if(sscanf((const char *)info, "%x:%x:%x:%x:%x:%x",
				&(ddMac[0]), &(ddMac[1]), &(ddMac[2]), &(ddMac[3]), &(ddMac[4]), &(ddMac[5])) == 6)
	        {
				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %02x:%02x:%02x:%02x:%02x:%02x!\n\r",
					__FUNCTION__, getpid(), ddMac[0],\
					ddMac[1], ddMac[2], ddMac[3],
					ddMac[4], ddMac[5]);
				
				pgpnDevBaseInfo->protocol_mac[0] = (UINT8)ddMac[0];
				pgpnDevBaseInfo->protocol_mac[1] = (UINT8)ddMac[1];
				pgpnDevBaseInfo->protocol_mac[2] = (UINT8)ddMac[2];
				pgpnDevBaseInfo->protocol_mac[3] = (UINT8)ddMac[3];
				pgpnDevBaseInfo->protocol_mac[4] = (UINT8)ddMac[4];
				pgpnDevBaseInfo->protocol_mac[5] = (UINT8)ddMac[5];
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else
		{
			optRsl = GPN_DEV_GEN_ERR;
		}

		if(optRsl == GPN_DEV_GEN_ERR)
		{
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : base info line err(%s)!\n\r",
				__FUNCTION__, getpid(), cfgStr);
			break;
		}
	}

	fclose(fp);

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s read over!\n\r",\
		__FUNCTION__, getpid(), gDevBaseInfo);

	return optRsl;
}

UINT32 gpnDevCommInfoFile2DataSt(stGpnDevCommInfo *pdevCommInfo)
{
	FILE *fp;
	UINT32 i;
	UINT32 j;
	char *cfgStr;
	char *argSlot;
	char *argIp;
	char *argPort[GPN_DEV_CI_PER_SLOT_UPORTS];
	char commInfo[GPN_DEV_INFO_LINE];
	UINT32 devType;
	UINT32 slot;
	UINT32 port[GPN_DEV_CI_PER_SLOT_UPORTS];

	if(pdevCommInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pdevCommInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}
	
	if(access(gDevCommAddr,F_OK | R_OK) < 0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : access %s err!\n\r",\
			__FUNCTION__, getpid(), gDevCommAddr);
		
		return GPN_DEV_GEN_ERR;
	}

	fp = fopen(gDevCommAddr, "rb");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gDevCommAddr);
		return GPN_DEV_GEN_ERR;
	}

	while(gpnFgets(commInfo, GPN_DEV_INFO_LINE, fp) != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : base info line : %s\n\r",\
			__FUNCTION__, getpid(), commInfo);
		if( (commInfo[0] == '#') ||\
			(commInfo[0] == '\0') ||\
			(commInfo[0] == '\n') ||\
			(commInfo[0] == '\r') )
		{
			continue;
		}

		/*find cfg line start */
		cfgStr = NULL;
		for(i=0;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (commInfo[i] != ' ') &&\
				(commInfo[i] != '\t') )
			{
				cfgStr = &(commInfo[i]);
				break;
			}
		}
		
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (commInfo[i] == ' ') ||\
				(commInfo[i] == '\t') )
			{
				commInfo[i] = '\0';
				break;
			}
		}

		/*find slot id, first string is "slot", so jump "slot..."*/
		argSlot = NULL;
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (commInfo[i] != ' ') &&\
				(commInfo[i] != '\t') )
			{
				argSlot = &(commInfo[i]);
				break;
			}
		}
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (commInfo[i] == ' ') ||\
				(commInfo[i] == '\t') )
			{
				commInfo[i] = '\0';
				break;
			}
		}
			
		argIp = NULL;
		if(strncmp(cfgStr, "devType", 7) != 0)
		{
			/*find slot ip, second string is "ip", so jump "...ip..."*/
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] != ' ') &&\
					(commInfo[i] != '\t') )
				{
					break;
				}
			}
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] == ' ') ||\
					(commInfo[i] == '\t') )
				{
					break;
				}
			}
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] != ' ') &&\
					(commInfo[i] != '\t') )
				{
					argIp = &(commInfo[i]);
					break;
				}
			}
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] == ' ') ||\
					(commInfo[i] == '\t') )
				{
					commInfo[i] = '\0';
					break;
				}
			}

			/*find slot UPD port, thrid string is "port", so jump "...port..." */
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] != ' ') &&\
					(commInfo[i] != '\t') )
				{
					break;
				}
			}
			for(i++;i<GPN_DEV_INFO_LINE;i++)
			{
				if( (commInfo[i] == ' ') ||\
					(commInfo[i] == '\t') )
				{
					break;
				}
			}
			for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
			{
				argPort[j] = NULL;
				for(i++;i<GPN_DEV_INFO_LINE;i++)
				{
					if( (commInfo[i] != ' ') &&\
						(commInfo[i] != '\t') )
					{
						argPort[j] = &(commInfo[i]);
						break;
					}
				}
				for(i++;i<GPN_DEV_INFO_LINE;i++)
				{
					/* may be cfg line over */
					if( (commInfo[i] == ' ') ||\
						(commInfo[i] == '\t') ||\
						(commInfo[i] == '\r') ||\
						(commInfo[i] == '\n') ||\
						(commInfo[i] == '\0') )
					{
						break;
					}
				}
			}
		}

		if(i<GPN_DEV_INFO_LINE)
		{
			if(strncmp (cfgStr, "slot", 4) == 0)
			{
				if(sscanf((const char *)argSlot, "%d", &slot) != 1)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config arg sscanf err, slot %s ->%d\n\r",\
						__FUNCTION__, getpid(), argSlot, slot);
					fclose(fp);

					return GPN_DEV_GEN_ERR;
				}
				
				for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
				{
					if(sscanf((const char *)(argPort[j]), "%d", &(port[j])) != 1)
					{
						GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config arg sscanf err, port%d %s->%d\n\r",\
							__FUNCTION__, getpid(), j, argPort[j], port[j]);
						fclose(fp);

						return GPN_DEV_GEN_ERR;
					}
				}
			}
			else if(strncmp (cfgStr, "devType", 7) == 0)
			{
				if(sscanf((const char *)argSlot, "%d", &devType) != 1)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config arg sscanf err, %s %s ->%d\n\r",\
						__FUNCTION__, getpid(), cfgStr, argSlot, devType);
					fclose(fp);

					return GPN_DEV_GEN_ERR;
				}

				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : get devType(%d).\n\r",\
					__FUNCTION__, getpid(), devType);
				pdevCommInfo->devType = devType;

				/*this cfg line paser end */
				continue;
			}
			else
			{
				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config line start(%s) err\n\r",\
					__FUNCTION__, getpid(), cfgStr);
				fclose(fp);
				
				return GPN_DEV_GEN_ERR;
			}
		}
		else
		{
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config format err\n\r",\
				__FUNCTION__, getpid());
			fclose(fp);
			
			return GPN_DEV_GEN_ERR;
		}
		
		if( (slot <= GPN_DEV_CI_MAX_SLOT) &&\
			(slot > 0) )
		{
			pdevCommInfo->slotCommInfo[slot-1].slotNum = slot;
			pdevCommInfo->slotCommInfo[slot-1].ipV4 = inet_addr(argIp);
			
			for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
			{
				if(port[j] <= 0x0000FFFF)
				{
					pdevCommInfo->slotCommInfo[slot-1].updPort[j] = (UINT16)port[j];
	
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : file cfg: udp_port(%d) - %d(0x%08x)\n\r",
						__FUNCTION__, getpid(), j, port[j], port[j]);
				}
				else
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config file \"port\" err\n\r",\
						__FUNCTION__, getpid());
					fclose(fp);
					
					return GPN_DEV_GEN_ERR;
				}
			}
		}
			

		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : file cfg:slot(%2d) ip(%s(%08x)) -port- %d\n\r",\
			__FUNCTION__, getpid(), slot, argIp, inet_addr(argIp), port[0]);
	}

	fclose(fp);

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s read over!\n\r",\
		__FUNCTION__, getpid(), gDevCommAddr);
	
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevFixIfInfoFile2DataSt(stDevFixIf *pfixPortInfo)
{
	FILE *fp;
	UINT32 i;
	//UINT32 optRsl;
	UINT32 devType;
	UINT32 filePortTpNum;
	char *cfgStr;
	char *pportNum;
	
	char fixIfStr[GPN_DEV_INFO_LINE];

	if(pfixPortInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pfixPortInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}
	
	if(access(gDevFixIf,F_OK | R_OK) < 0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : access %s err!\n\r",\
			__FUNCTION__, getpid(), gDevFixIf);
		
		return GPN_DEV_GEN_ERR;
	}

	fp = fopen(gDevFixIf, "rb");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gDevFixIf);
		return GPN_DEV_GEN_ERR;
	}
	
	/*read config from file result */
	//optRsl = GPN_DEV_GEN_OK;
	
	while(gpnFgets(fixIfStr, GPN_DEV_INFO_LINE, fp) != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) :  base info line : %s\n\r",\
			__FUNCTION__, getpid(), fixIfStr);
		if( (fixIfStr[0] == '#') ||\
			(fixIfStr[0] == '\0') ||\
			(fixIfStr[0] == '\n') ||\
			(fixIfStr[0] == '\r') )
		{
			continue;
		}

		cfgStr = NULL;
		/*info type */
		for(i=0;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (fixIfStr[i] != '\t') &&\
				(fixIfStr[i] != ' ') )
			{
				cfgStr = &(fixIfStr[i]);
				break;
			}
		}
		for(i=0;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (fixIfStr[i] == ' ') ||\
				(fixIfStr[i] == '\t') )
			{
				fixIfStr[i] = '\0';
				break;
			}
		}

		pportNum = NULL;
		/*info data */
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (fixIfStr[i] != ' ') &&\
				(fixIfStr[i] != '\t') )
			{
				pportNum = &(fixIfStr[i]);
				break;
			}
		}

		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (fixIfStr[i] == ' ') ||\
				(fixIfStr[i] == '\t') ||\
				(fixIfStr[i] == '\r') ||\
				(fixIfStr[i] == '\n') ||\
				(fixIfStr[i] == '\0') )
			{
				fixIfStr[i] = '\0';
				break;
			}
		}

		if( (cfgStr == NULL) ||\
			(pportNum == NULL) )
		{
			//optRsl = GPN_DEV_GEN_ERR;
		}
		else
		{
			if(strncmp (cfgStr, "devType", 7) == 0)
			{
				if(sscanf((const char *)pportNum, "%d", &devType) != 1)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config arg sscanf err, %s %s ->%d\n\r",\
						__FUNCTION__, getpid(), cfgStr, pportNum, devType);
					fclose(fp);

					return GPN_DEV_GEN_ERR;
				}

				pfixPortInfo->devType = devType;
			}
			else if(strncmp (cfgStr, "maxPortType", 11) == 0)
			{
				if(sscanf((const char *)pportNum, "%d", &filePortTpNum) != 1)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : config arg sscanf err, %s %s ->%d\n\r",\
						__FUNCTION__, getpid(), cfgStr, pportNum, filePortTpNum);
					fclose(fp);

					return GPN_DEV_GEN_ERR;
				}

				if(pfixPortInfo->maxPortTpNum < filePortTpNum)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : filePortTpNum(%d) more than maxPortTpNum(%d)\n\r",\
						__FUNCTION__, getpid(),\
						filePortTpNum,\
						pfixPortInfo->maxPortTpNum);
				}
				if(pfixPortInfo->sysPortTpNum != filePortTpNum)
				{
					GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : filePortTpNums(%d) missmatch maxPortTpNum(%d)\n\r",\
						__FUNCTION__, getpid(),\
						filePortTpNum,\
						pfixPortInfo->sysPortTpNum);
				}

				pfixPortInfo->sysPortTpNum = filePortTpNum;
				pfixPortInfo->portTpNum = 0;
				
			}
			else
			{
				gpnDevFixIfFileCmdLine2DataSt(pfixPortInfo, fixIfStr, pportNum);
			
				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s %s\n\r",\
					__FUNCTION__, getpid(), fixIfStr, pportNum);
			}
		}
	}

	fclose(fp);
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) :  read %s over!\n\r",\
			__FUNCTION__, getpid(), fixIfStr);

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevFixIfFileCmdLine2DataSt(stDevFixIf *pfixPortInfo,
	char *portTypeStr, char *pportNum)
{
	UINT32 i;
	UINT32 seat;
	UINT32 portNum;
	UINT32 portTypeNum;
	
	/*assert */
	if( (pfixPortInfo == NULL) ||\
		(portTypeStr == NULL) ||\
		(pportNum == NULL) )
	{
		return GPN_DEV_GEN_ERR;
	}

	if(pfixPortInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pfixPortInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}
	
	if(pfixPortInfo->pfixPort == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pfixPortInfo->pfixPort is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : register portType \"%s\" num \"%s\"\n\r",\
		__FUNCTION__, getpid(), portTypeStr, pportNum);

	/*if space is end */
	if(pfixPortInfo->portTpNum > pfixPortInfo->maxPortTpNum)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : when proc \"%s %s\", space end!\n\r",\
			__FUNCTION__, getpid(), portTypeStr, pportNum);
		return GPN_DEV_GEN_ERR;
	}

	/*lable */
	seat = pfixPortInfo->portTpNum;
	
	if(sscanf((const char *)pportNum, "%d", &portNum) == 1)
	{
		pfixPortInfo->pfixPort[seat].portNum = portNum;
	}
	else
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %s find port num %s err!\n\r",\
			__FUNCTION__, getpid(), portTypeStr, pportNum);
		pfixPortInfo->pfixPort[seat].portNum = 0;
	}

	/*device core info support portType Num */
	portTypeNum = sizeof(ggpnDevPortTypeName)/sizeof(ggpnDevPortTypeName[0]);
	for(i=0;i<portTypeNum;i++)
	{
		if( (strncmp(ggpnDevPortTypeName[i].portTypeStr, portTypeStr,\
			  strlen(portTypeStr)) == 0) &&\
			(strncmp(ggpnDevPortTypeName[i].portTypeStr, portTypeStr,\
			  strlen(ggpnDevPortTypeName[i].portTypeStr)) == 0) )
		{
			pfixPortInfo->pfixPort[seat].portType = ggpnDevPortTypeName[i].portType;
			/*add portType Num */
			pfixPortInfo->portTpNum++;
			break;
		}
	}

	if(i >= portTypeNum)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : \"%s\" can not find suit typeStr, err!\n\r",\
			__FUNCTION__, getpid(), portTypeStr);
	}
	
	return GPN_DEV_GEN_OK; 
}
UINT32 gpnDevMgtInfoFile2DataSt(stDevMgtInfo *pDevMgtInfo/* the space this porint porinted must be sizeof(stDevMgtInfo)*/)
{
	FILE *fp;
	UINT32 i;
	UINT32 devType;
	UINT32 in1QPri;
	UINT32 inbandVlan;
	UINT32 optRsl;
	UINT32 ddMac[GPN_DEV_MAC_LEN];
	char *cfgStr;
	char *info;
	char baseInfo[GPN_DEV_INFO_LINE];

	if(pDevMgtInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pDevMgtInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	/*clear data struct to 0 */
	memset(pDevMgtInfo, 0, sizeof(stDevMgtInfo));
	
	if(access(gDevMgtInfo,F_OK | R_OK) < 0)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : access %s err!\n\r",\
			__FUNCTION__, getpid(), gDevMgtInfo);
		
		return GPN_DEV_GEN_ERR;
	}

	fp = fopen(gDevMgtInfo, "rb");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gDevMgtInfo);
		return GPN_DEV_GEN_ERR;
	}

	/*read config from file result */
	optRsl = GPN_DEV_GEN_OK;
	
	while(gpnFgets(baseInfo, GPN_DEV_INFO_LINE, fp) != NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) :  base info line : %s\n\r",\
			__FUNCTION__, getpid(), baseInfo);
		if( (baseInfo[0] == '#') ||\
			(baseInfo[0] == '\0') ||\
			(baseInfo[0] == '\n') ||\
			(baseInfo[0] == '\r') )
		{
			continue;
		}

		cfgStr = NULL;
		/*info type */
		for(i=0;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] != ' ') &&\
				(baseInfo[i] != '\t') )
			{
				cfgStr = &(baseInfo[i]);
				break;
			}
		}
		
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] == ' ') ||\
				(baseInfo[i] == '\t') )
			{
				baseInfo[i] = '\0';
				break;
			}
		}
		
		info = NULL;
		/*info data */
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] != ' ') &&\
				(baseInfo[i] != '\t') )
			{
				info = &(baseInfo[i]);
				break;
			}
		}
		for(i++;i<GPN_DEV_INFO_LINE;i++)
		{
			if( (baseInfo[i] == ' ') ||\
				(baseInfo[i] == '\t') ||\
				(baseInfo[i] == '\r') ||\
				(baseInfo[i] == '\n') ||\
				(baseInfo[i] == '\0') )
			{
				baseInfo[i] = '\0';
				break;
			}
		}

		if( (info == NULL) ||
			(cfgStr == NULL) )
		{
			optRsl = GPN_DEV_GEN_ERR;
		}
		else if(strncmp (cfgStr, "devType", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &devType) == 1)
			{
				pDevMgtInfo->devType = devType;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "inbandIp", 8) == 0)
		{
			pDevMgtInfo->inbandIp = inet_addr(info);
		}
		else if(strncmp (cfgStr, "inbandVlan", 10) == 0)
		{
			if(sscanf((const char *)info, "%d", &inbandVlan) == 1)
			{
				pDevMgtInfo->inbandVlan = inbandVlan;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "ib1QPri", 7) == 0)
		{
			if(sscanf((const char *)info, "%d", &in1QPri) == 1)
			{
				pDevMgtInfo->ib1QPri = in1QPri;
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else if(strncmp (cfgStr, "ibMac", 5) == 0)
		{
			if(sscanf((const char *)info, "%x:%x:%x:%x:%x:%x",
				&(ddMac[0]), &(ddMac[1]), &(ddMac[2]), &(ddMac[3]), &(ddMac[4]), &(ddMac[5])) == 6)
	        {
				GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : %02x:%02x:%02x:%02x:%02x:%02x!\n\r",
					__FUNCTION__, getpid(), ddMac[0],\
					ddMac[1], ddMac[2], ddMac[3],
					ddMac[4], ddMac[5]);
				
				pDevMgtInfo->ibMac[0] = (UINT8)ddMac[0];
				pDevMgtInfo->ibMac[1] = (UINT8)ddMac[1];
				pDevMgtInfo->ibMac[2] = (UINT8)ddMac[2];
				pDevMgtInfo->ibMac[3] = (UINT8)ddMac[3];
				pDevMgtInfo->ibMac[4] = (UINT8)ddMac[4];
				pDevMgtInfo->ibMac[5] = (UINT8)ddMac[5];
			}
			else
			{
				optRsl = GPN_DEV_GEN_ERR;
			}
		}
		else
		{
			optRsl = GPN_DEV_GEN_ERR;
		}

		if(optRsl == GPN_DEV_GEN_ERR)
		{
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : base info line err(%s)!\n\r",
				__FUNCTION__, getpid(), cfgStr);
			break;
		}
	}

	fclose(fp);

	GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s read over!\n\r",\
		__FUNCTION__, getpid(), gDevMgtInfo);

	return optRsl;
}

UINT32 gpnDevInfoHardware2DataSt(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR;
	}

	/*
		device core infomation have 3 part :
		1 base infomation, like device type ...;
		2 communication addr infomation, like ip, port, mac ...
		3 fix interface info(type about)
	*/

	/*part 1 : device base infomation fromt config file to data struct */
	gpnDevBaseInfoHardware2DataSt();
	
	/*debug print 
	debugGpnDevCoreInfoDateStPrint();*/
	
	/*part 2 : device communication infomation config file to data struct */
	gpnDevCommInfoHardware2DataSt();

	/*debug print 
	debugGpnDevCoreInfoDateStPrint();*/

	/*part 3 : device fix port infomation config file to data struct */
	gpnDevFixIfInfoHardware2DataSt();

	/*debug print 
	debugGpnDevCoreInfoDateStPrint();*/

	/*part 4 : mgt infomation config file to data struct */
	gpnDevMgtInfoHardware2DataSt();

	/*debug print 
	debugGpnDevCoreInfoDateStPrint();*/

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevTypeInfoHardware2DataSt(void)
{
	UINT32 devType;
	
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	/*base CFLAGS macore defined device type, 
	   get device type from HW, use special driver */
#if defined(GPN_CFLAGS_DEV_HT201)
	devType = GPN_DEV_TYPE_HT201;
#elif defined(GPN_CFLAGS_DEV_HT201E)
	devType = GPN_DEV_TYPE_HT201E;
#elif defined(GPN_CFLAGS_DEV_H20RN2000)
	devType = GPN_DEV_TYPE_H20RN2000;
#elif defined(GPN_CFLAGS_DEV_HT157)
	devType = GPN_DEV_TYPE_HT157;
#elif defined(GPN_CFLAGS_DEV_HT158)
	devType = GPN_DEV_TYPE_HT158;
#elif defined(GPN_CFLAGS_DEV_VX)
	devType = GPN_DEV_TYPE_VX;
#else
	devType = GPN_DEV_TYPE_NULL;
#endif

	switch(devType)
	{
		case GPN_DEV_TYPE_HT201:
			printf ("%s[%d] : gpnAlarm start -> HT201 device\n", __func__, __LINE__);
			break;
		case GPN_DEV_TYPE_HT201E:
			printf ("%s[%d] : gpnAlarm start -> HT201E device\n", __func__, __LINE__);
			break;
		case GPN_DEV_TYPE_H20RN2000:
			printf ("%s[%d] : gpnAlarm start -> H20RN2000 device\n", __func__, __LINE__);
			break;
		case GPN_DEV_TYPE_HT157:
			printf ("%s[%d] : gpnAlarm start -> HT157 device\n", __func__, __LINE__);
			break;
		case GPN_DEV_TYPE_HT158:
			printf ("%s[%d] : gpnAlarm start -> HT158 device\n", __func__, __LINE__);
			break;
		case GPN_DEV_TYPE_VX:
			printf ("%s[%d] : gpnAlarm start -> VX device\n", __func__, __LINE__);
			break;
		default:
			break;
	}
	
	pgpnDevCoreInfo->devBaseInfo.devType = devType;
	if(gpnGlobalDevType2DevStrType(devType, pgpnDevCoreInfo->devBaseInfo.devStrType,\
		GPN_DEV_MAX_TYPE_STR_LEN) == GPN_DEV_GEN_ERR)
	{
		printf ("%s[%d] : devType(%d:%d) not support\n", __func__, __LINE__, devType, GPN_DEV_TYPE_HT201);
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : devType(%d) change to DevStrType err!\n\r",\
			__FUNCTION__, getpid(), devType);
		return GPN_DEV_GEN_ERR;
	}

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevBaseInfoHardware2DataSt(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	#if 0
	/*get devType from devStrType */
	if(gpnGlobalDevStrType2DevType(pgpnDevCoreInfo->devBaseInfo.devStrType,\
		&(pgpnDevCoreInfo->devBaseInfo.devType)) != GPN_DEV_GEN_OK)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : \"%s\" change to DevType err!\n\r",\
			__FUNCTION__, getpid(), pgpnDevCoreInfo->devBaseInfo.devStrType);

		/* modify for  when devStrType changed */
		/*return GPN_DEV_GEN_ERR; */
		pgpnDevCoreInfo->devBaseInfo.devType = GPN_DEV_TYPE_NULL;
	}

	if(pgpnDevCoreInfo->devBaseInfo.devType == GPN_DEV_TYPE_NULL)
	{
		/*sys not know devType from cfgFile, get from HW */
		gpnDevTypeInfoHardware2DataSt();
	}
	#endif
	/* soft should not use other type's bin */
	gpnDevTypeInfoHardware2DataSt();
	
	switch(pgpnDevCoreInfo->devBaseInfo.devType)
	{
		case GPN_DEV_TYPE_NULL:
			/*do nothing, control out of ablity*/
			fprintf(stderr, "%s(%d) : device type get err!\n\r",\
				__FUNCTION__, getpid());
			exit(0);
			break;

		case GPN_DEV_TYPE_HT201:
			gpnDevDrvHT201Driver();
			break;

		case GPN_DEV_TYPE_HT201E:
			gpnDevDrvHT201EDriver();
			break;

		case GPN_DEV_TYPE_H20RN2000:
			gpnDevDrvH20RN2000Driver();
			break;

		case GPN_DEV_TYPE_HT157:
			gpnDevDrvHT157Driver();
			break;

		case GPN_DEV_TYPE_HT158:
			gpnDevDrvHT158Driver();
			break;

		case GPN_DEV_TYPE_VX:
			gpnDevDrvVXDriver();
			break;

#if 0
		case GPN_DEV_TYPE_EDD0402B:
		case GPN_DEV_TYPE_EDD0402X:
		case GPN_DEV_TYPE_EDD0204:
			/*now in DEV_TYPE_GET process, should move to here */
            gpnDevDrvH18EDD0402BDriver();
			break;
			
		case GPN_DEV_TYPE_H20PN1660P:
			gpnDevDrvH20PN1660PDriver();
			break;
			
		case GPN_DEV_TYPE_H20PN1660C:
			gpnDevDrvH20PN1660CDriver();
			break;
			
		case GPN_DEV_TYPE_H20PN1610:
			gpnDevDrvH20PN1610Driver();
			break;
			
		case GPN_DEV_TYPE_EDD2402B:
			gpnDevDrvH18EDD2402BDriver();
			break;	
			
		case GPN_DEV_TYPE_V8ENM:
			gpnDevDrvH9MONM81Driver();
			break;
			
		case GPN_DEV_TYPE_V8ECPX10:
			gpnDevDrvH9MOCPX10Driver();
			break;
			
		case GPN_DEV_TYPE_H20CE1604C:
			gpnDevDrvH20CE1604CDriver();
			break;

		case GPN_DEV_TYPE_H20PN2000:
			gpnDevDrvH20PN2000Driver();
			break;
#endif

		default:
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : unknow device type \"%s(%d)\"\n\r",\
				__FUNCTION__, getpid(), pgpnDevCoreInfo->devBaseInfo.devStrType,\
				pgpnDevCoreInfo->devBaseInfo.devType);
			break;
	}
	
	/*re-creat device base info */
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : syn  \"%s\"\n\r",\
		__FUNCTION__, getpid(), gDevBaseInfo);
	gpnDevBaseInfoInitReCreat(gDevBaseInfo);

	/*debug print 
	debugGpnDevCoreInfoDateStPrint()*/;
	
	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevCommInfoHardware2DataSt(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	/*devcommInfo's devType is same with devBaseInfo's devType */
	pgpnDevCoreInfo->devCommInfo.devType = pgpnDevCoreInfo->devBaseInfo.devType;
	
	switch(pgpnDevCoreInfo->devCommInfo.devType)
	{
		case GPN_DEV_TYPE_NULL:
			/*do nothing, control out of ablity*/
			/*do nothing, control out of ablity*/
			fprintf(stderr, "%s(%d) : device type get err!\n\r",\
				__FUNCTION__, getpid());
			exit(0);
			break;

#if 0
		case GPN_DEV_TYPE_EDD0402B:
		case GPN_DEV_TYPE_EDD0402X:
		case GPN_DEV_TYPE_EDD0204:
		case GPN_DEV_TYPE_H20PN1660P:
		case GPN_DEV_TYPE_H20PN1660C:
		case GPN_DEV_TYPE_H20PN1610:
		case GPN_DEV_TYPE_H20CE1604C:
		case GPN_DEV_TYPE_H20PN2000:
		case GPN_DEV_TYPE_EDD2402B:	
		case GPN_DEV_TYPE_V8ENM:
		case GPN_DEV_TYPE_V8ECPX10:
#endif

		case GPN_DEV_TYPE_HT201:
		case GPN_DEV_TYPE_HT201E:
		case GPN_DEV_TYPE_H20RN2000:
		case GPN_DEV_TYPE_HT157:
		case GPN_DEV_TYPE_HT158:
		case GPN_DEV_TYPE_VX:
			/*should not special cfg comm info */
			break;

		default:
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : unknow device type \"%s(%d)\"\n\r",\
				__FUNCTION__, getpid(), pgpnDevCoreInfo->devBaseInfo.devStrType,\
				pgpnDevCoreInfo->devBaseInfo.devType);
			break;
	}

	/*re-creat device comm info */
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : syn  \"%s\"\n\r",\
		__FUNCTION__, getpid(), gDevCommAddr);
	gpnDevCommInfoInitReCreat(gDevCommAddr);

	/*debug print 
	debugGpnDevCoreInfoDateStPrint()*/;
	
	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevFixIfInfoHardware2DataSt(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR;
	}

	/*devcommInfo's devType is same with devBaseInfo's devType */
	pgpnDevCoreInfo->fixPortInfo.devType = pgpnDevCoreInfo->devBaseInfo.devType;

	switch(pgpnDevCoreInfo->fixPortInfo.devType)
	{
		case GPN_DEV_TYPE_NULL:
			/*do nothing, control out of ablity*/
			fprintf(stderr, "%s(%d) : device type get err!\n\r",\
				__FUNCTION__, getpid());
			exit(0);
			break;

		case GPN_DEV_TYPE_HT201:
			gpnDevDrvHT201GetFixPort();
			break;

		case GPN_DEV_TYPE_HT201E:
			gpnDevDrvHT201EGetFixPort();
			break;

		case GPN_DEV_TYPE_H20RN2000:
			gpnDevDrvH20RN2000GetFixPort();
			break;

		case GPN_DEV_TYPE_HT157:
			gpnDevDrvHT157GetFixPort();
			break;

		case GPN_DEV_TYPE_HT158:
			gpnDevDrvHT158GetFixPort();
			break;

		case GPN_DEV_TYPE_VX:
			gpnDevDrvVXGetFixPort();
			break;

#if 0
		case GPN_DEV_TYPE_EDD0402B:
		case GPN_DEV_TYPE_EDD0402X:
		case GPN_DEV_TYPE_EDD0204:
			gpnDevDrvH18EDD0402BGetFixPort();
			break;
			
		case GPN_DEV_TYPE_H20PN1660P:
			gpnDevDrvH20PN1660PGetFixPort();
			break;
			
		case GPN_DEV_TYPE_H20PN1660C:
			gpnDevDrvH20PN1660CGetFixPort();
			break;
			
		case GPN_DEV_TYPE_H20PN1610:
			gpnDevDrvH20PN1610GetFixPort();
			break;

		case GPN_DEV_TYPE_EDD2402B:
			gpnDevDrvH18EDD2402BGetFixPort();
			break;
			
		case GPN_DEV_TYPE_V8ENM:
			gpnDevDrvH9MONM81GetFixPort();
			break;
			
		case GPN_DEV_TYPE_V8ECPX10:
			gpnDevDrvH9MOCPX10GetFixPort();
			break;

		case GPN_DEV_TYPE_H20CE1604C:
			gpnDevDrvH20CE1604CGetFixPort();
			break;

		case GPN_DEV_TYPE_H20PN2000:
			gpnDevDrvH20PN2000GetFixPort();
			break;
#endif

		default:
			/*default templat */
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : use default fix portTemplat!\n\r",\
				__FUNCTION__, getpid());
			break;
	}

	/*re-creat device fix Interface info */
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : syn \"%s\"\n\r",\
		__FUNCTION__, getpid(), gDevFixIf);
	gpnDevFixIfInfoInitReCreat(gDevFixIf);

	/*debug print 
	debugGpnDevCoreInfoDateStPrint()*/;

	return GPN_DEV_GEN_OK;
}
UINT32 gpnDevMgtInfoHardware2DataSt(void)
{
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR;
	}

	/*devMgtInfo's devType is same with devBaseInfo's devType */
	pgpnDevCoreInfo->devMgtInfo.devType = pgpnDevCoreInfo->devBaseInfo.devType;
	switch(pgpnDevCoreInfo->devMgtInfo.devType)
	{
		case GPN_DEV_TYPE_NULL:
			/*do nothing, control out of ablity*/
			fprintf(stderr, "%s(%d) : device type get err!\n\r",\
				__FUNCTION__, getpid());
			exit(0);
			break;

		case GPN_DEV_TYPE_HT201:
			gpnDevDrvHT201GetMgtInfo();
			break;

		case GPN_DEV_TYPE_HT201E:
			gpnDevDrvHT201EGetMgtInfo();
			break;

		case GPN_DEV_TYPE_H20RN2000:
			gpnDevDrvH20RN2000GetMgtInfo();
			break;

		case GPN_DEV_TYPE_HT157:
			gpnDevDrvHT157GetMgtInfo();
			break;

		case GPN_DEV_TYPE_HT158:
			gpnDevDrvHT158GetMgtInfo();
			break;

		case GPN_DEV_TYPE_VX:
			gpnDevDrvVXGetMgtInfo();
			break;

		
#if 0
		case GPN_DEV_TYPE_EDD0402B:
		case GPN_DEV_TYPE_EDD0402X:
		case GPN_DEV_TYPE_EDD0204:
			break;
			
		case GPN_DEV_TYPE_H20PN1660P:
			break;
			
		case GPN_DEV_TYPE_H20PN1660C:
			break;
			
		case GPN_DEV_TYPE_H20PN1610:
			gpnDevDrvH20PN1610GetMgtInfo();
			break;
			
		case GPN_DEV_TYPE_EDD2402B:
			break;
			
		case GPN_DEV_TYPE_V8ENM:
			break;
			
		case GPN_DEV_TYPE_V8ECPX10:
			gpnDevDrvH9MOCPX10GetMgtInfo();
			break;

		case GPN_DEV_TYPE_H20CE1604C:
			gpnDevDrvH20CE1604CGetMgtInfo();
			break;

		case GPN_DEV_TYPE_H20PN2000:
			gpnDevDrvH20PN2000GetMgtInfo();
			break;
#endif

		default:
			/*default templat */
			GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : use default mgt info!\n\r",\
				__FUNCTION__, getpid());
			break;
	}

	/*re-creat device fix Interface info */
	GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : syn \"%s\"\n\r",\
		__FUNCTION__, getpid(), gDevMgtInfo);
	gpnDevMgtInfoInitReCreat(gDevMgtInfo);

	/*debug print 
	debugGpnDevCoreInfoDateStPrint()*/;

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevCoreInfoDateSt2Ram(void)
{
	FILE *fp;
	char *pbuff;
	UINT32 i;
	UINT32 j;
	char buff[GPN_DEV_INFO_FILE_LEN];
	

	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}
	
	if(access(gRamDevCoreInfo, F_OK) == 0)
	{
		/*init role  transfer inet socket ip & port from config file */
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CUP, "%s(%d) : %s exist\n\r",\
			__FUNCTION__, getpid(), gRamDevCoreInfo);
		return GPN_DEV_GEN_OK;
	}

	fp = fopen(gRamDevCoreInfo, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gRamDevCoreInfo);
		return GPN_DEV_GEN_ERR;
	}
	
	/*creat file*/
	pbuff = buff;

	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\r%s created by p-%d \n\r", gRamDevCoreInfo, getpid());
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO baseInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devBaseInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType : %s(%d)\n\rselfSlot: %d\nNMaSlot : %d\nNMbSlot : %d\nSWaSlot : %d\nSWbSlot : %d\nMaxSlot : %d\n",
		pgpnDevCoreInfo->devBaseInfo.devStrType,\
		pgpnDevCoreInfo->devBaseInfo.devType,\
		pgpnDevCoreInfo->devBaseInfo.selfSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMaSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMbSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWaSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWbSlot,\
		pgpnDevCoreInfo->devBaseInfo.maxSlot);
	pbuff = pbuff + strlen(pbuff);

	snprintf(pbuff, GPN_DEV_INFO_LINE, "protocol-mac : %02x:%02x:%02x:%02x:%02x:%02x\n",
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[0],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[1],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[2],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[3],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[4],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[5]);
	pbuff = pbuff + strlen(pbuff);
	
	/*comm info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO commInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devCommInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
	{
		snprintf(pbuff, GPN_DEV_INFO_LINE, "slotNum(%d) ip(%08x)\n",\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum,\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4);
		pbuff = pbuff + strlen(pbuff);
		
		for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "udpPort%d(%d) ",\
				j+1, pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[j]);
			pbuff = pbuff + strlen(pbuff);
		}
		
		snprintf(pbuff, GPN_DEV_INFO_LINE, " \n");
		pbuff = pbuff + strlen(pbuff);
	}

	/*fix port info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO fixIfInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->fixPortInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	for(i=0;i<pgpnDevCoreInfo->fixPortInfo.portTpNum;i++)
	{
		snprintf(pbuff, GPN_DEV_INFO_LINE, "protType(%08x) num(%d)\n\r",\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType,\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portNum);
		pbuff = pbuff + strlen(pbuff);
	}

	/*mgt info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO MgtInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devMgtInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandIp %d.%d.%d.%d\n\r",\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>24) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>16) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 8) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 0) & 0x000000FF));
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandVlan %d\n\r",\
		pgpnDevCoreInfo->devMgtInfo.inbandVlan);
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "ib1QPri %d\n\r",\
		pgpnDevCoreInfo->devMgtInfo.ib1QPri);
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "ibMac %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
		pgpnDevCoreInfo->devMgtInfo.ibMac[0],\
		pgpnDevCoreInfo->devMgtInfo.ibMac[1],\
		pgpnDevCoreInfo->devMgtInfo.ibMac[2],\
		pgpnDevCoreInfo->devMgtInfo.ibMac[3],\
		pgpnDevCoreInfo->devMgtInfo.ibMac[4],\
		pgpnDevCoreInfo->devMgtInfo.ibMac[5]);
	pbuff = pbuff + strlen(pbuff);

	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);

	return GPN_DEV_GEN_OK;
}

UINT32 gpnDevCoreInfoDateSt2RamRenew(void)
{
	FILE *fp;
	char *pbuff;
	UINT32 i;
	UINT32 j;
	char buff[GPN_DEV_INFO_FILE_LEN];
	

	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	fp = fopen(gRamDevCoreInfo, "w+");
	if(fp == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : fopen \"%s\" fail!\n\r",
			__FUNCTION__, getpid(), gRamDevCoreInfo);
		return GPN_DEV_GEN_ERR;
	}
	
	/*creat file*/
	pbuff = buff;

	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\r%s renewed by p-%d \n\r", gRamDevCoreInfo, getpid());
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO baseInfo struct print \n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devBaseInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType : %s(%d)\n\rselfSlot: %d\nNMaSlot : %d\nNMbSlot : %d\nSWaSlot : %d\nSWbSlot : %d\nMaxSlot : %d\n",
		pgpnDevCoreInfo->devBaseInfo.devStrType,\
		pgpnDevCoreInfo->devBaseInfo.devType,\
		pgpnDevCoreInfo->devBaseInfo.selfSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMaSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMbSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWaSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWbSlot,\
		pgpnDevCoreInfo->devBaseInfo.maxSlot);
	pbuff = pbuff + strlen(pbuff);


	snprintf(pbuff, GPN_DEV_INFO_LINE, "protocol-mac : %02x:%02x:%02x:%02x:%02x:%02x\n",
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[0],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[1],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[2],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[3],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[4],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[5]);
	pbuff = pbuff + strlen(pbuff);
	
	/*comm info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO commInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devCommInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
	{
		snprintf(pbuff, GPN_DEV_INFO_LINE, "slotNum(%d) ip(%08x)\n",\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum,\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4);
		pbuff = pbuff + strlen(pbuff);
		
		for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
		{
			snprintf(pbuff, GPN_DEV_INFO_LINE, "udpPort%d(%d) ",\
				j+1, pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[j]);
			pbuff = pbuff + strlen(pbuff);
		}
		
		snprintf(pbuff, GPN_DEV_INFO_LINE, " \n");
		pbuff = pbuff + strlen(pbuff);
	}

	/*fix port info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO fixIfInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->fixPortInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	for(i=0;i<pgpnDevCoreInfo->fixPortInfo.portTpNum;i++)
	{
		snprintf(pbuff, GPN_DEV_INFO_LINE, "protType(%08x) num(%d)\n\r",\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType,\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portNum);
		pbuff = pbuff + strlen(pbuff);
	}

	/*mgt info */
	snprintf(pbuff, GPN_DEV_INFO_LINE, "\n\rGPN_DEV_INFO MgtInfo struct print\n\r");
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devMgtInfo.devType);
	pbuff = pbuff + strlen(pbuff);
	
	snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandIp %d.%d.%d.%d\n\r",\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>24) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>16) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 8) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 0) & 0x000000FF));
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "inbandVlan %d\n\r",\
		pgpnDevCoreInfo->devMgtInfo.inbandVlan);
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "ib1QPri %d\n\r",\
		pgpnDevCoreInfo->devMgtInfo.ib1QPri);
	pbuff = pbuff + strlen(pbuff);
	snprintf(pbuff, GPN_DEV_INFO_LINE, "ibMac %02x:%02x:%02x:%02x:%02x:%02x\n\r",\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[0],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[1],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[2],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[3],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[4],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[5]);
	pbuff = pbuff + strlen(pbuff);

	fwrite(buff, strlen(buff), 1, fp);
	fflush(fp);
	fclose(fp);

	return GPN_DEV_GEN_OK;
}

char *gpnGlobalPortTypeSmpStrGet(UINT32 portType)
{
	UINT32 i;
	UINT32 portTypeNum;

	portTypeNum = sizeof(ggpnDevPortTypeName)/sizeof(ggpnDevPortTypeName[0]);

	for(i=0;i<portTypeNum;i++)
	{
		if(portType == ggpnDevPortTypeName[i].portType)
		{
			return (char *)(ggpnDevPortTypeName[i].smpStr);
		}
	}

	/* error type */
	return (char *)(ggpnDevPortTypeName[IFM_NULL_TYPE].smpStr);
}

UINT32 debugGpnDevCoreInfoDateStPrint(void)
{
	UINT32 i;
	UINT32 j;
	
	if(pgpnDevCoreInfo == NULL)
	{
		GPN_DEV_CI_PRINT(GPN_DEV_CI_CMP, "%s(%d) : pgpnDevCoreInfo is NULL!\n\r",\
			__FUNCTION__, getpid());
		return GPN_DEV_GEN_ERR; 
	}

	/*base info */
	fprintf(stdout, "\n\rGPN_DEV_INFO baseInfo struct print\n\r");
	fprintf(stdout, "devType : %s(%d)\n\rselfSlot: %d\n\rNMaSlot : %d\n\rNMbSlot : %d\n\rSWaSlot : %d\n\rSWbSlot : %d\n\rMaxSlot : %d\n\r",
		pgpnDevCoreInfo->devBaseInfo.devStrType,\
		pgpnDevCoreInfo->devBaseInfo.devType,\
		pgpnDevCoreInfo->devBaseInfo.selfSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMaSlot,\
		pgpnDevCoreInfo->devBaseInfo.NMbSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWaSlot,\
		pgpnDevCoreInfo->devBaseInfo.SWbSlot,\
		pgpnDevCoreInfo->devBaseInfo.maxSlot);

	fprintf(stdout, "protocol-mac : %02x:%02x:%02x:%02x:%02x:%02x\n",
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[0],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[1],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[2],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[3],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[4],\
		(UINT8)pgpnDevCoreInfo->devBaseInfo.protocol_mac[5]);

	/*comm info */
	fprintf(stdout, "\n\rGPN_DEV_INFO commInfo struct print\n\r");
	fprintf(stdout, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devCommInfo.devType);
	for(i=0;i<GPN_DEV_CI_MAX_SLOT;i++)
	{
		fprintf(stdout, "slotNum(%d) ip(%08x)\n\r",\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].slotNum,\
			pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].ipV4);
			
		for(j=0;j<GPN_DEV_CI_PER_SLOT_UPORTS;j++)
		{
			fprintf(stdout, "udpPort%d(%d) ",\
				j+1, pgpnDevCoreInfo->devCommInfo.slotCommInfo[i].updPort[j]);
		}
		fprintf(stdout, " \n\r");
	}

	/*fix port info */
	fprintf(stdout, "\n\rGPN_DEV_INFO fixIfInfo struct print\n\r");
	fprintf(stdout, "devType(%d)\n\r",\
		pgpnDevCoreInfo->fixPortInfo.devType);
	for(i=0;i<pgpnDevCoreInfo->fixPortInfo.portTpNum;i++)
	{
		fprintf(stdout, "protType(%08x) num(%d)\n\r",\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portType,\
			pgpnDevCoreInfo->fixPortInfo.pfixPort[i].portNum);
	}

	/*mgt info */
	fprintf(stdout, "\n\rGPN_DEV_INFO mgtInfo struct print\n\r");
	fprintf(stdout, "devType(%d)\n\r",\
		pgpnDevCoreInfo->devMgtInfo.devType);
	fprintf(stdout, "inbandIp :%d.%d.%d.%d\n\rinbandVlan: %d\n\rib1QPri : %d\n\r",
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 24) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >> 16) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>  8) & 0x000000FF),\
		(UINT8)((pgpnDevCoreInfo->devMgtInfo.inbandIp >>  0) & 0x000000FF),\
		pgpnDevCoreInfo->devMgtInfo.inbandVlan,\
		pgpnDevCoreInfo->devMgtInfo.ib1QPri);

	fprintf(stdout, "ibMac : %02x:%02x:%02x:%02x:%02x:%02x\n",
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[0],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[1],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[2],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[3],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[4],\
		(UINT8)pgpnDevCoreInfo->devMgtInfo.ibMac[5]);
	
	return GPN_DEV_GEN_OK;
}

INT32 gpnDevCoreFree(void)
{
	if(pgpnDevCoreInfo != NULL)
	{
		free(pgpnDevCoreInfo);
		pgpnDevCoreInfo	= NULL;
	}

	return GPN_DEV_GEN_ERR; 
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_DEV_CORE_INFO_C_ */

