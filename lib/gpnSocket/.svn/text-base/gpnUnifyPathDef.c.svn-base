/**********************************************************
* file name: gpnUnifyPathDef.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-05-19
* function: 
*    define gpn unify file path method and opration
* modify:
*
***********************************************************/
#ifndef _GPN_UNIFY_PATH_DEF_C_
#define _GPN_UNIFY_PATH_DEF_C_
#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "gpnLog/gpnLogFuncApi.h"

#include "socketComm/gpnUnifyPathDef.h"

UINT32 gpnUnifyPathTestDemo(void)
{
	/*check and creat path */
	gpnUnifyPathCreat((char *)GPN_UP_ROM_NODE);
	gpnUnifyPathCreat((char *)GPN_UP_RAM_NODE);
	
	/*ROM node test */
	gpnUnifyPathRomNodeTest();
	/*RAM node test */
	gpnUnifyPathRamNodeTest();
	
	return GPN_UNIFYP_GEN_OK;
}

UINT32 gpnUnifyPathRomNodeTest(void)
{
	FILE *fp;
	char *pCmd;
	char cmd[GPN_UNIFYP_DEMO_FILE_LEN];/*1k*/
	char *romUnifyPathReadMe;
	
	romUnifyPathReadMe = (char *)(GPN_UNIFY_PATH_ROM_NODE(romUnifyPathReadMe));

	fp = fopen(romUnifyPathReadMe,"w+");
	if(fp == NULL)
	{
		gpnLog(GPN_LOG_L_CRIT, "--%s: creat %s fail!--\n\r",
			__FUNCTION__, romUnifyPathReadMe);
		GPN_UNIFYP_PRINT(GPN_UNIFYP_AGP, "--%s: creat %s fail!--\n\r",
			__FUNCTION__, romUnifyPathReadMe);
		
		return GPN_UNIFYP_GEN_ERR;
	}
	
	pCmd = cmd;
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** we do this for GPN_UNIFY_PATH(GPN_UNIFY_PATH_ROM_NODE) function test \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** we show you how to use this function at the same time \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "## %s \n", GPN_UNIFY_PATH_ROM_NODE_TOKEN);
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** rom_unify_path_default means Makefile not define ROM unify path \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** so default ROM path is \"%s\" \n", GPN_UNIFY_PATH_ROM_NODE());
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** rom_unify_path_config means Makefile define ROM unify path \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** config ROM path is \"%s\" \n",GPN_UNIFY_PATH_ROM_NODE());

	fwrite(cmd, strlen(cmd), 1, fp);
	fflush(fp);
	fclose(fp);

	return GPN_UNIFYP_GEN_OK;
}
UINT32 gpnUnifyPathRamNodeTest(void)
{
	FILE *fp;
	char *pCmd;
	char cmd[GPN_UNIFYP_DEMO_FILE_LEN];/*1k*/
	char *ramUnifyPathReadMe;

	ramUnifyPathReadMe = (char *)(GPN_UNIFY_PATH_RAM_NODE(ramUnifyPathReadMe));
	
	fp = fopen(ramUnifyPathReadMe,"w+");
	if(fp == NULL)
	{
		gpnLog(GPN_LOG_L_CRIT, "--%s: creat %s fail!--\n\r",
			__FUNCTION__, ramUnifyPathReadMe);
		GPN_UNIFYP_PRINT(GPN_UNIFYP_AGP, "--%s: creat %s fail!--\n\r",
			__FUNCTION__, ramUnifyPathReadMe);
		
		return GPN_UNIFYP_GEN_ERR;
	}
	
	pCmd = cmd;
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** we do this for GPN_UNIFY_PATH(GPN_UNIFY_PATH_RAM_NODE) function test \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** we show you how to use this function at the same time \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "## %s \n", GPN_UNIFY_PATH_RAM_NODE_TOKEN);
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** ram_unify_path_default means Makefile not define RAM unify path \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** so default RAM path is \"%s\" \n", GPN_UNIFY_PATH_RAM_NODE());
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** ram_unify_path_config means Makefile define ROM unify path \n");
	pCmd = pCmd + strlen(pCmd);
	snprintf(pCmd, GPN_UNIFYP_CMD_LEN, "** config RAM path is \"%s\" \n",GPN_UNIFY_PATH_RAM_NODE());

	fwrite(cmd, strlen(cmd), 1, fp);
	fflush(fp);
	fclose(fp);

	return GPN_UNIFYP_GEN_OK;
}
UINT32 gpnUnifyPathCreat(char *path)
{
	UINT32 pathLen;
	UINT32 pathDeep;
	char *p;
	char pathName[GPN_UNIFYP_MAX_PATHN_LEN+1];/* +1 for string's end '\0' */

	
	/*assert */
	if(path == NULL)
	{
		GPN_UNIFYP_PRINT(GPN_UNIFYP_AGP, "%s(%d) : assert err!\n\r",\
			__FUNCTION__, getpid());
		gpnLog(GPN_LOG_L_CRIT, "%s(%d) : assert err!\n\r",\
			__FUNCTION__, getpid());
		
		return GPN_UNIFYP_GEN_ERR;
	}

	GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s(%d) : will creat path %s!\n\r",\
		__FUNCTION__, getpid(), path);
	
	/*paht lenth check */
	pathLen = strlen(path);
	if(pathLen > GPN_UNIFYP_MAX_PATHN_LEN)
	{
		GPN_UNIFYP_PRINT(GPN_UNIFYP_AGP, "%s(%d) : input path lenth(%d) too big!\n\r",\
			__FUNCTION__, getpid(), pathLen);
		gpnLog(GPN_LOG_L_CRIT, "%s(%d) : input path lenth(%d) too big!\n\r",\
			__FUNCTION__, getpid(), pathLen);
		
		return GPN_UNIFYP_GEN_ERR;
	}

	GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s : %s pathLen %d\n\r",\
		__FUNCTION__, path, pathLen);

	/*if path exist check */
	if(access(path, F_OK) == -1)
	{
		/*set path deep is 0, cmp with GPN_UNIFYP_MAX_PATH_DEEP */
		pathDeep = 0;

		/*find every '/', then check if path exist, if not exist, creat it */
		p = pathName;
		while(pathDeep < GPN_UNIFYP_MAX_PATH_DEEP)
		{
			pathDeep++;

			memset(pathName, 0, GPN_UNIFYP_MAX_PATHN_LEN);
			strncpy(pathName, path, pathLen);
			p = strchr(p, '/');
			if(p != NULL)
			{
				/*cut part of path */
				++p;
				*p = '\0';
				++p;
			}
			
			GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s : %s pathLen %d\n\r",\
				__FUNCTION__, pathName, strlen(pathName));
			
			/*check if part of path exist, if not, creat it */
			if(access(pathName, F_OK) == -1)
			{
				if(mkdir(pathName, 0777) == -1)
				{
					/*mkdir err*/
					GPN_UNIFYP_PRINT(GPN_UNIFYP_AGP, "%s(%d) : mkdir %s err\n\r",\
						__FUNCTION__, getpid(), pathName);
					gpnLog(GPN_LOG_L_CRIT, "%s(%d) : mkdir %s err\n\r",\
						__FUNCTION__, getpid(), pathName);
				}
				else
				{
					GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s(%d) : mkdir %s good\n\r",\
						__FUNCTION__, getpid(), pathName);
				}
			}
			else
			{
				GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s(%d) : dir %s exist\n\r",\
					__FUNCTION__, getpid(), pathName);
			}
			
			if(p == NULL)
			{
				/*check over */
				GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s(%d) : p = NULL when creat part of path(%s)\n\r",\
					__FUNCTION__, getpid(), pathName);
				break;
			}
		}
	}
	else
	{
		GPN_UNIFYP_PRINT(GPN_UNIFYP_CUP, "%s(%d) : dir %s exist\n\r",\
			__FUNCTION__, getpid(), path);
	}
	
	return GPN_UNIFYP_GEN_OK;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /* _GPN_UNIFY_PATH_DEF_C_ */

