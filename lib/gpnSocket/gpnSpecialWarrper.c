/**********************************************************
* file name: gpnSpecialWarrper.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-04-01
* function: 
*    define spcial function's gpn-format warrper
* modify:
*
***********************************************************/
#ifndef _GPN_SPCIAL_WARRPER_C_
#define _GPN_SPCIAL_WARRPER_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "stdio.h"
#include "errno.h"
#include "signal.h"

#include "gpnLog/gpnLogFuncApi.h"
#include "socketComm/gpnSpecialWarrper.h"

char *gpnFgets(char *s, INT32 size, FILE *stream)
{
	char *reVal;
	UINT32 againCounter;
	sigset_t newset;
	sigset_t oldset;
	/*sigset_t pedset;*/

	sigemptyset(&newset);
	sigaddset(&newset, 0);			// block signal 0
	sigaddset(&newset, SIGUSR1);	// block signal SIGUSR1
	sigaddset(&newset, SIGUSR2);	// block signal SIGUSR2
	sigprocmask(SIG_BLOCK, &newset, &oldset);

	againCounter = 0;

	gpnFgetsAgain:
		reVal = fgets(s, size, stream);
		if(reVal == NULL)
		{
			againCounter++;
			
			if(errno == EINTR)
			{
				/*print & log & goto again*/
				if(againCounter < GPN_FFETS_MAX_AGAIN)
				{
					/*temp printf & log*/
					/**/fprintf(stderr, "%s(%d) : read stream %08x interrupte system call(%dth again)\n\r",\
						__FUNCTION__, getpid(), (UINT32)stream, againCounter);

					/*gpnLog(GPN_LOG_L_CRIT, "%s : read stream %08x interrupte system call(%dth again)\n\r",\
						__FUNCTION__, (UINT32)stream, againCounter);*/

					goto gpnFgetsAgain;
				}
				else
				{
					/*always printf*/
					fprintf(stderr, "%s(%d) : read stream %08x interrupte system call(%dth again)\n\r",\
						__FUNCTION__, getpid(), (UINT32)stream, againCounter);

					gpnLog(GPN_LOG_L_EMERG, "%s : read stream %08x interrupte system call(%dth again)\n\r",\
						__FUNCTION__, (UINT32)stream, againCounter);
				}
			}
		}

	sigprocmask(SIG_SETMASK, &oldset, NULL);

	return reVal;
}

INT32 gpnKeyFileVersionGet(const char *pathname, gpnVerTag *pVerTag)
{
	FILE *fp;
	UINT32 reVal;
	UINT32 i;
	UINT32 v;
	UINT32 c;
	UINT32 r;
	UINT32 b;
	UINT32 line;
	char *devStr;
	char *vStr;
	char *cStr;
	char *rStr;
	char *bStr;
	char baseInfo[GPN_KEY_FILE_LINE_LEN];
	
	if(access(pathname, F_OK | R_OK) < 0)
	{
		gpnLog(GPN_LOG_L_EMERG, "%s : when check \"%s\"'s version, not exit or can't read\n\r",\
			__FUNCTION__, pathname);
		
		return GPN_SW_ERR;
	}

	fp = fopen(pathname, "rb");
	if(fp == NULL)
	{
		gpnLog(GPN_LOG_L_EMERG, "%s : fopen \"%s\" fail\n\r",\
			__FUNCTION__, pathname);
		return GPN_SW_ERR;
	}

	reVal = GPN_SW_ERR;
	/*read key fine vesion info from first 5 lines */
	line = 5;
	while(gpnFgets(baseInfo, GPN_KEY_FILE_LINE_LEN, fp) != NULL)
	{
		if(strncmp(baseInfo, GPN_KEY_FILE_VERSION_HEAD, strlen(GPN_KEY_FILE_VERSION_HEAD)) == 0)
		{
			/* analysis version info string */
			devStr = NULL;
			vStr = NULL;
			cStr = NULL;
			rStr = NULL;
			bStr = NULL;
			/* find v string */
			for(i=strlen(GPN_KEY_FILE_VERSION_HEAD);i<GPN_KEY_FILE_VERSION_LEN;i++)
			{
				if(baseInfo[i] == 'D')
				{
					baseInfo[i] = '\0';
					break;
				}
			}
			i++;
			devStr = &(baseInfo[i]);
			
			for(i++;i<GPN_KEY_FILE_VERSION_LEN;i++)
			{
				if(baseInfo[i] == 'V')
				{
					baseInfo[i] = '\0';
					break;
				}
			}
			i++;
			vStr = &(baseInfo[i]);
			
			for(i++;i<GPN_KEY_FILE_VERSION_LEN;i++)
			{
				if(baseInfo[i] == 'C')
				{
					baseInfo[i] = '\0';
					break;
				}
			}
			i++;
			cStr = &(baseInfo[i]);
			
			for(i++;i<GPN_KEY_FILE_VERSION_LEN;i++)
			{
				if(baseInfo[i] == 'R')
				{
					baseInfo[i] = '\0';
					break;
				}
			}
			i++;
			rStr = &(baseInfo[i]);

			for(i++;i<GPN_KEY_FILE_VERSION_LEN;i++)
			{
				if(baseInfo[i] == 'B')
				{
					baseInfo[i] = '\0';
					break;
				}
			}
			i++;
			bStr = &(baseInfo[i]);

			for(i++;i<GPN_KEY_FILE_VERSION_LEN;i++)
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

			if( (sscanf((const char *)vStr, "%d", &v) != 1) ||\
				(sscanf((const char *)cStr, "%d", &c) != 1) ||\
				(sscanf((const char *)rStr, "%d", &r) != 1) ||\
				(sscanf((const char *)bStr, "%d", &b) != 1) )
			{
				/* break while */
				break;
			}
			
			GPN_KEY_FILE_VERSION_CREAT(pVerTag, devStr, v, c, r, b);
			/*printf("%s : path %s devStr \"%s\"(%d) version %08x \n\r",\
				__FUNCTION__, pathname, pVerTag->devStr,\
				strlen(pVerTag->devStr), pVerTag->version);*/
			
			/* get right verTag, return OK, and break while */
			reVal = GPN_SW_OK;
			break;
		}
		
		/* check file line */
		line--;
		if(line == 0)
		{
			break;
		}
	}
	
	pclose(fp);
	return reVal;
}

INT32 gpnKeyFileVersionCheck(const char *pathname, gpnVerTag *pExpVerTag)
{
	gpnVerTag verTag;
	
	if(gpnKeyFileVersionGet(pathname, &verTag) != GPN_SW_OK)
	{
		gpnLog(GPN_LOG_L_CRIT, "%s : get \"%s\" is version err!\n\r",\
			__FUNCTION__, pathname);
		return GPN_SW_ERR;
	}
	
	if( (memcmp(verTag.devStr, pExpVerTag->devStr, strlen(verTag.devStr)) == 0) &&\
		((verTag.version == 0) ||\
		(verTag.version == pExpVerTag->version)) )
	{
		gpnLog(GPN_LOG_L_EMERG, "%s : \"%s\"'s version %s%08x, same with expVer %s%08x\n\r",\
			__FUNCTION__, pathname,\
			verTag.devStr,\
			verTag.version,\
			pExpVerTag->devStr,\
			pExpVerTag->version);
		return GPN_SW_OK;
	}

	return GPN_SW_ERR;
}

UINT32 gpnFindPsKeyWorkOrder(char *keyWord, UINT32 *order)
{
	UINT32 i;
	UINT32 j;
	UINT32 bMark;
	FILE *fp = NULL;
	char *p;
	char cmd[GPN_PS_KEY_MAX_CMD_LEN] ={0};
	char status[GPN_PS_KEY_MAX_CMD_LEN] = {0};
	
	/* assert */
	if( (keyWord == NULL) ||\
		(order == NULL) )
	{
		return GPN_SW_ERR;
	}
	
	/*grep -i, --ignore-case ignore case distinctions*/
	snprintf(cmd, GPN_PS_KEY_MAX_CMD_LEN, "ps | grep -i \"%s\"", keyWord);
	if(NULL == (fp = popen(cmd,"r")))   
	{ 
		perror("popen failed...");
		gpnLog(GPN_LOG_L_CRIT, "%s : %s popen failed!\n\r", __FUNCTION__, cmd);

		return GPN_SW_ERR;
	}

	/* just care first line */
	if( NULL == gpnFgets(status, GPN_PS_KEY_MAX_CMD_LEN, fp) )   
	{   
 		perror("fgets error...");
		pclose(fp); 
		gpnLog(GPN_LOG_L_CRIT, "%s : %s fgets error!\n\r", __FUNCTION__, cmd);

		return GPN_SW_ERR;
	}
	pclose(fp);

	/* except '\n' */
	p = strrchr(keyWord, '\n');
	if(p != NULL)
	{
		*p = '\0';
	}
	p = strrchr(status, '\n');
	if(p != NULL)
	{
		*p = '\0';
	}
	
	/* find key word order */
	bMark = GPN_SW_YES;
	for(i=0,j=1; i<GPN_PS_KEY_MAX_CMD_LEN; i++)
	{
		if( (bMark == GPN_SW_NO) &&\
			(status[i] == ' ') )
		{
			j++;
			bMark = GPN_SW_YES;
		}
		else if((bMark == GPN_SW_YES) &&\
				(status[i] != ' ') )
		{
			bMark = GPN_SW_NO;
			if( (strncasecmp(&(status[i]), keyWord, strlen(keyWord)) == 0) &&\
				(strncasecmp(&(status[i]), keyWord, strlen(&(status[i]))) == 0) )
			{
				*order = j;
				return GPN_SW_OK;
			}
		}
	}

	return GPN_SW_ERR;
}

UINT32 gpnProcessUniqueRunCheck(INT8 *progname)
{
	UINT32 count;
	UINT32 cmdOrder;
	FILE* fp=NULL;
	char cmd[GPN_UCHECK_MAX_CMD_LEN];
	char name[GPN_UCHECK_MAX_NAME_LEN];
	char command[] = "command";
	char *psName;
	char *p;

	/*assert*/
	if(progname == NULL)
	{
		return GPN_SW_ERR;
	}

	memset(cmd, 0, GPN_UCHECK_MAX_CMD_LEN);
	memset(name, 0, GPN_UCHECK_MAX_NAME_LEN);
	
	/*some processes like gpn_init start by initd, so '-' will be add ahead*/
	if(progname[0] == '-')
	{
		progname = ((p = strrchr(progname, '-')) ? ++p : progname);
	}
	progname = ((p = strrchr(progname, '/')) ? ++p : progname);
	p = strrchr(progname, '\n');
	if(p != NULL)
	{
		*p = '\0';
	}
	/*printf("input porgname %s\n\r", progname);*/
	
	if(gpnFindPsKeyWorkOrder(command, &cmdOrder) != GPN_SW_OK)
	{
		gpnLog(GPN_LOG_L_EMERG, "%s : get command order err!\n\r",
			__FUNCTION__);
		goto __check_err__;
	}
	gpnLog(GPN_LOG_L_EMERG, "%s : get command order err! %d\n\r",
			__FUNCTION__, cmdOrder);
	snprintf(cmd,GPN_UCHECK_MAX_CMD_LEN, "ps | grep \"%s\" | awk '{print $%d}'", progname, cmdOrder);
	
	if(NULL == (fp=popen(cmd, "r")))
	{   
		perror("popen failed...");   
		goto __check_err__;  
	}

	count = 0;
	while(gpnFgets(name, GPN_UCHECK_MAX_NAME_LEN, fp) != NULL)
	{
		/*some processes like gpn_init start by initd, so '-' will be add ahead*/
		if(psName[0] == '-')
		{
			psName = ((p = strrchr(psName, '-')) ? ++p : psName);
		}
		psName = ((p = strrchr(name, '/')) ? ++p : name);
		p = strrchr(psName, '\n');
		if(p != NULL)
		{
			*p = '\0';
		}
		/*printf("get ps porgname %s\n\r", psName);*/
		
		if( (strncmp(psName, progname, strlen(progname)) == 0) &&\
			(strncmp(psName, progname, strlen(psName)) == 0) )
		{
			count++;
		}
	}
	pclose(fp);

	if(count > 1)
	{
		/*process still running, so stop this starup*/
		fprintf(stderr, "%s already running, so stop this starup\n", progname);
		exit(0);
		return GPN_SW_ERR;
	}
	else
	{
		return GPN_SW_OK;
	}

	__check_err__:
		fprintf(stderr, "%s unique check err, exit !\n", progname);
		exit(0);
		return GPN_SW_ERR;
}

#ifdef    __cplusplus
}
#endif /* __cplusplus */

#endif /*_GPN_SPCIAL_WARRPER_C_*/

