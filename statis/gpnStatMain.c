/**********************************************************
* file name: gpnStatferMain.c
* Copyright: 
	 Copyright 2014 huahuan.
* author: 
*    huahuan liuyf 2014-03-11
* function: 
*    
* modify:
*
***********************************************************/
#ifndef _GPN_STAT_MAIN_C_
#define _GPN_STAT_MAIN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <zebra.h>
#include <sys/sysinfo.h>
#include <sched.h>

#include "version.h"
#include "getopt.h"
#include "thread.h"
#include "command.h"
#include "memory.h"
#include "keychain.h"
#include "vty.h"
#include "syslog.h"
#include "log.h"
#include "lib/hash1.h"
#include "lib/pid_file.h"

#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"
#include "lib/sigevent.h"

#include "gpnStatMain.h"
#include "gpnStatDebug.h"

#include "socketComm/gpnSockInclude.h"

/*log function include*/
#include "gpnLog/gpnLogFuncApi.h"

/*SUP SMP debug mode include*/
#include "socketComm/gpnDebugFuncApi.h"
#include "socketComm/gpnSockIfmMsgDef.h"
#include "socketComm/gpnSockStatMsgDef.h"
#include "socketComm/gpnSpecialWarrper.h"
#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

/*stat process include*/
#include "gpnStatProcMain.h"
#include "gpnStatFuncApi.h"
#include "gpnStatDataStruct.h"
#include "gpnStatUnifyPath.h"
#include "gpnStatMain.h"
#include "gpnStatCmd.h"

/*subAgent function include*/
//#include "net-snmp/net-snmp-config.h"
//#include "net-snmp/net-snmp-includes.h"
//#include "net-snmp/agent/net-snmp-agent-includes.h"
//#include "subAgent/snmp_subagent.h"
//#include "subAgent/statMgmtMib.h"

/*ipran lib include*/
#include "gpnStatIpc.h"
#include "gpnStatSnmp.h"


/* lipf add */
#define STATIS_DEFAULT_CONFIG   "statis.conf"
#define PATH_STATIS_PID "/var/run/statis.pid"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_STATIS_PID;  /* defined in config.h */

struct thread_master *stat_master = NULL;


/* gpn-stat version discription                                                                    */
/* V1R0C0B2 :                                                                                          */
/* 1 : get fix port list from gpnDevFixPortInfo.conf                                       */
/* 2 : chang log path to gpn_log default path                                               */
/* 3 : use gpn_sock comm api                                                                    */
UINT8 gGpnStatVersion = 5;
UINT8 gGpnStatRelease = 0;
/* here specially for database change */
UINT8 gGpnStatChange = 0;
UINT8 gGpnStatBuild = 3;

/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

UINT32 gpnStatSelfNotifyDevStatus(void)
{
	UINT32 sta;
	UINT32 devIndex;

	/*get device index */
	gpnDevApiGetDevIndex(&devIndex);
	
	/*get status, should use which method to get device status */
	sta = GPN_SOCK_MSG_DEV_STA_INSERT;

	/*notify device status to gpn_stat(self) */
	gpnSockIfmMsgTxDevStateNotify2Stat(devIndex, sta);
	
	return GPN_STAT_GEN_OK;
}
UINT32 gpnStatSelfNotifyFixPort(void)
{
	UINT32 devIndex;
	UINT32 portTpNum;
	UINT32 spaceSize;
	stStatPortObjCL *pportObjCL;

	/*get fix port */
	gpnDevApiGetDevFixPortInfo(&portTpNum, NULL, 0);
	
	spaceSize = portTpNum * sizeof(stStatPortObjCL);
	pportObjCL = (stStatPortObjCL *)malloc(spaceSize);
	if(pportObjCL == NULL)
	{
		return GPN_STAT_GEN_ERR;
	}

	gpnDevApiGetDevFixPortInfo(NULL, (stIfInfo *)pportObjCL, spaceSize);

	devIndex = 1;
	//devIndex = DeviceIndex_Create(1, 0);
	gpnSockIfmMsgTxFixPortCapabilityNotify2Stat(devIndex, pportObjCL, spaceSize);

	/*free malloc*/
	if(pportObjCL == NULL)
	{
		free(pportObjCL);
	}
	
	return GPN_STAT_GEN_OK;
}
UINT32 gpnStatLogInit(void)
{
	UINT32 reVal;
	
	/*INT8 *stderr = (INT8 *)("stderr");*/
	/*INT8 *stdout = (INT8 *)("stdout");*/
	/*INT8 *applog = (INT8 *)("applog");*/
	/*INT8 *callback = (INT8 *)("callback");*/
	/*INT8 *Lfile = (INT8 *)(GPN_DEFAULTL_LOG_ID);*/
	/*INT8 *Bfile = (INT8 *)(GPN_DEFAULTB_LOG_ID);*/
	/*INT8 *syslog = (INT8 *)("syslog");*/

	/* enable gpn_stat default log use gpn_log mode */
	/* ???? */
	
	INT8 *Lfile = (INT8 *)GPN_STAT_UP_RAM_DIR(gpn_stat.log);
	reVal = gpnLogFuncApiLogEnable(Lfile, GPN_LOGHANDLER_LFILE, GPN_LOG_L_EMERG, GPN_LOG_L_DEBUG);
	if(reVal == GPN_STAT_GEN_ERR)
	{
		zlog_err("%s : enable %s log err\n\r",
			__FUNCTION__, Lfile);
		return GPN_STAT_GEN_ERR;
	}
	/*printf("%s : enable %s log ok\n\r",
				__FUNCTION__, Lfile);*/

	Lfile = (INT8 *)GPN_STAT_UP_RAM_DIR(gpn_stat_tmp.log);
	/*reVal = gpnLogFuncApiLogEnable(Lfile, GPN_LOGHANDLER_LFILE, GPN_LOG_L_NOTICE, GPN_LOG_L_NOTICE);
	if(reVal == GPN_STAT_GEN_ERR)
	{
		printf("%s : enable %s log err\n\r",
			__FUNCTION__, Lfile);
		return GPN_STAT_GEN_ERR;
	}*/
	
	return GPN_STAT_GEN_OK;
}



INT32 gpnStatStarUp(void)
{
	UINT32 reVal;

	/*debugGpnMemSmpMgtCheck();*/
	
	//printf("step 1 : use default debug function init\n\r");
	/* step 1 : use default debug function init */
	gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_USER_MODE, 0x00000000/*0x00000000*//*0xFFFFFFFE*/);
	//gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_USER_MODE, 0xFFFFFFFE/*0x00000000*//*0xFFFFFFFE*/);

	//sleep(10);

	//printf("step 2 : gpn_up gpn_stat path init\n\r");
	/*step 2 : gpn_up gpn_stat path init*/
	gpnStatUPPathInit();
	
	//printf("step 3: use default log function init\n\r");	
	/*step 3 : use default log function init */
	reVal = gpnStatLogInit();
	if(reVal == GPN_STAT_GEN_ERR)
	{
		return GPN_STAT_GEN_ERR;
	}
	
	//sleep(10);

	//printf("step 4 : use gpn_sock_comm startup api to instance fd and use msg proc callbace register\n\r");
	/* step 4 : use gpn_sock_comm startup api to instance fd and use msg proc callbace register */
#if 0
	reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_STAT, (usrMsgProc)gpnStatCorePorc);
	if(reVal == GPN_STAT_GEN_ERR)
	{
		return GPN_STAT_GEN_ERR;
	}
#endif

	//sleep(10);
	
	//printf("step 5: process functions init\n\r");
	/* step 5 : process functions init */
	gpnStatProcInit();

	//sleep(10);

	//printf("step 6 : device, fix port ... alarmMan self notify\n\r");*/
	/* step 6 : device, fix port ... alarmMan self notify*/
	//---------------gpnStatSelfNotifyDevStatus();
	//---------------gpnStatSelfNotifyFixPort();
	
	//sleep(10);

	//printf("step 7 :subagent functions init\n\r");
	/* step 7 : subagent functions init */
//	snmp_subagent_init();
	gpnStatTrapDataMethodRegist((STATTRAPFUN)send_hist_data_trap);

	/* test */
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(1, second), 0,\
		gpnStatForNsmFollowTest, 0, 0);*/

	/* test */
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(8, second), 0,\
		gpnStatForEvnThredTpTest, 0, 0);*/

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(10, second), 0,\
		gpnStatForNsmMsgTest, 0, 0);*/

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(5, second), 0,\
		gpnStatForFixTableTest, 0, 0);*/

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(6, second), 0,\
		gpnStatForAlarmThredTpTest, 0, 0);*/
		
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(10, second), 0,\
		gpnStatForSdmMsgTest, 0, 0);*/

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(20, second), 0,\
		gpnStatForOamMsgTest, 0, 0);*/

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(30, second), 0,\
		gpnStatForOnmdMsgTest, 0, 0);*/

#if 0

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(9, second), 0,\
		gpnStatForSubReportTpTest, 0, 0);*/

	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(5, second), 0,\
		gpnStatForCalculateTest, 0, 0);
#endif
#if 0
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(5, second), 0,\
		gpnStatForEventThredTpTest, 0, 0);
		
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(6, second), 0,\
		gpnStatForSubFiltTpTest, 0, 0);

	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(7, second), 0,\
		gpnStatForTaskTest, 0, 0);
	
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(8, second), 0,\
		gpnStatForPortMonTest, 0, 0);
	
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(15, second), 0,\
		gpnStatForXPtnLspDataGetRsp, 0, 0);
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(16, second), 0,\
		gpnStatForXPtnLspDataGetRsp, 0, 0);
	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(17, second), 0,\
		gpnStatForXPtnLspDataGetRsp, 0, 0);

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(20, second), 0,\
		gpnStatForCurrDataGet, 0, 0);*/
	
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(60, second), 0,\
		gpnStatForHistDataGet, 0, 0);*/

	/**/gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(61, second), 0,\
		gpnStatForXPtnLspDataGetRsp, 0, 0);

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(65, second), 0,\
		gpnStatForCurrDataGet, 0, 0);*/
	
	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(85, second), 0,\
		gpnStatForHistDataGet, 0, 0);*/

	/*get lspStat from nsm*/
	gpnTimerAddUserOpt(GPN_TIMR_TYPE_ONCE, GPN_TIME_TO_TICKS(9, second), 0,\
		gpnStatMsgForPtnEthPStatGet, 0, 0);

	//UINT32 X = 100;
#endif
	//sleep(10);
	//printf("entry while\n\r");
#if 0
	while(1)
	{
		/* big sleep for cpu use-rata */
		usleep(30000);
		
		/*gpn_sock process, select opt (no block)*/
		/*gpnSockCommApiNoBlockProc();*/
		gpnSockCommApiSelectTimerNoBlockProc();

		/*net-snmp subagent process, select opt (no block)*/
		#if 0
		reVal = 10;
		while(reVal--)
		{
			snmp_subagent_tick(0);
			usleep(100);
		}
		#endif
	}
#endif
	#if 0
	gpnSockMsg gpnStatMsgSpIn;
	gpnSockMsg *pgpnStatMsgSpOut;
	objLogicDesc *plocalPIndex;
	stStatMsgMonObjCfg *pstatMonObjCfg;

	memset(&gpnStatMsgSpIn, 0, sizeof(gpnSockMsg));
	
	pgpnStatMsgSpOut = (gpnSockMsg *)malloc(sizeof(gpnSockMsg)+sizeof(objLogicDesc)+sizeof(stStatMsgMonObjCfg));
	pgpnStatMsgSpOut->msgCellLen = sizeof(objLogicDesc)+sizeof(stStatMsgMonObjCfg);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_GET_NEXT;

	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);
	
	pstatMonObjCfg = (stStatMsgMonObjCfg *)(&(pgpnStatMsgSpOut->msgCellLen)+1);
	plocalPIndex = (objLogicDesc *)(pstatMonObjCfg+1);
	printf("dev(%08x) port(%08x|%08x|%08x|%08x|%08x) find next Node dev(%08x) port(%08x|%08x|%08x|%08x|%08x)\n\r",\
		pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara1, pgpnStatMsgSpOut->iMsgPara3,\
		pgpnStatMsgSpOut->iMsgPara4, pgpnStatMsgSpOut->iMsgPara5, pgpnStatMsgSpOut->iMsgPara6,\
		plocalPIndex->devIndex, plocalPIndex->portIndex, plocalPIndex->portIndex3,\
		plocalPIndex->portIndex4, plocalPIndex->portIndex5, plocalPIndex->portIndex6);

	printf("gEn %d currEn %d\n\r",\
		pstatMonObjCfg->statMoniEn, pstatMonObjCfg->currStatMoniEn);
	printf("longEn %d task %d evnThId %d subFiltThId %d almThId %d rept %d\n\r",\
		pstatMonObjCfg->longCycStatMoniEn, pstatMonObjCfg->longCycBelongTask, pstatMonObjCfg->longCycEvnThredTpId,\
		pstatMonObjCfg->longCycSubFiltTpId, pstatMonObjCfg->longCycAlmThredTpId, pstatMonObjCfg->longCycHistAutoRept);
	printf("shortEn %d task %d evnThId %d subFiltThId %d almThId %d rept %d\n\r",\
		pstatMonObjCfg->shortCycStatMoniEn, pstatMonObjCfg->shortCycBelongTask, pstatMonObjCfg->shortCycEvnThredTpId,\
		pstatMonObjCfg->shortCycSubFiltTpId, pstatMonObjCfg->shortCycAlmThredTpId, pstatMonObjCfg->shortCycHistAutoRept);
	printf("udEn %d task %d evnThId %d subFiltThId %d almThId %d rept %d\n\r",\
		pstatMonObjCfg->udCycStatMoniEn, pstatMonObjCfg->udCycBelongTask, pstatMonObjCfg->udCycEvnThredTpId,\
		pstatMonObjCfg->udCycSubFiltTpId, pstatMonObjCfg->udCycAlmThredTpId, pstatMonObjCfg->udCycHistAutoRept,\
		pstatMonObjCfg->udCycSecs);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_MON_ADD;	
	gpnStatMsgSpIn.iMsgPara1 = plocalPIndex->devIndex;
	gpnStatMsgSpIn.iMsgPara2 = plocalPIndex->portIndex;
	gpnStatMsgSpIn.iMsgPara3 = plocalPIndex->portIndex3;
	gpnStatMsgSpIn.iMsgPara4 = plocalPIndex->portIndex4;
	gpnStatMsgSpIn.iMsgPara5 = plocalPIndex->portIndex5;
	gpnStatMsgSpIn.iMsgPara6 = plocalPIndex->portIndex6;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_BASE_MON_OPT;
	gpnStatMsgSpIn.iMsgPara7 = GPN_STAT_GEN_ENABLE;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);


	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgEvnThTp);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 0;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 1;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_EVN_THRED_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 2;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	pgpnStatMsgSpOut->msgCellLen = sizeof(stStatMsgSubFiltTp);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 0;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 1;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);

	gpnStatMsgSpIn.iMsgType = GPN_STAT_MSG_SUBFILT_TEMP_GET_NEXT;
	gpnStatMsgSpIn.iMsgPara1 = 2;
	reVal = gpnStatAgentXIoCtrl(&gpnStatMsgSpIn, sizeof(gpnSockMsg), pgpnStatMsgSpOut, 0);
	printf("reVal %d\n\r", reVal);
	#endif
	
	return GPN_STAT_GEN_OK;
}

UINT32 gpnStatHelp(INT32 status, INT8 *progname)
{
	if (status != 0)
	{
		fprintf (stderr, "Try `%s --help' for more information.\n", progname);
	}
	else
	{    
		printf ("Usage : %s [OPTION...]\n\
Daemon which manages gpnTransfer \n\n\
-d, --daemon       Runs in daemon mode\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n",
		progname);
	}

	exit (status);
	
	return GPN_STAT_GEN_OK;
}

UINT32 gpnStatDaemonize(UINT32 nochdir, UINT32 noclose)
{
	pid_t pid;
	//UINT32 ret;

	pid = fork();

	/* In case of fork is error. */
	if(pid < 0)
	{
		perror ("fork:");
		return GPN_STAT_GEN_ERR;
	}

	/* In case of this is parent process. */
	if(pid != 0)
	{
		exit(0);
	}

	/* Become session leader and get pid. */
	pid = setsid();

	if(pid < -1)
	{
		perror("setsid:");
		return GPN_STAT_GEN_ERR;;
	}

	/* Change directory to root. */
	if(!nochdir)
	{
		//ret = chdir("/");
		chdir("/");
	}

	/* File descriptor close. */
	if(!noclose)
	{
		INT32 fd;

		fd = open("/dev/null", O_RDWR, 0);
		if(fd != -1)
		{
			dup2(fd, STDIN_FILENO);
			//dup2 (fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			if(fd > 2)
			{
				close(fd);
			}
		}
	}

	return GPN_STAT_GEN_OK;
}


/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif


#if 0
static RETSIGTYPE *
signal_set ( int signo, void ( *func ) ( int ) )
{
    int ret;
    struct sigaction sig;
    struct sigaction osig;

    sig.sa_handler = func;
    sigemptyset ( &sig.sa_mask );
    sig.sa_flags = 0;
#ifdef SA_RESTART
    sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

    ret = sigaction ( signo, &sig, &osig );

    if ( ret < 0 )
    { return ( SIG_ERR ); }
    else
    { return ( osig.sa_handler ); }
}


void statis_die(void)
{
    if(NULL != zlog_default)
    {
        closezlog(zlog_default);
    }
    exit ( 0 );
}

#endif

/*---------------------------------- 修改共享内存时修改, lipf, 2018/8/23 --------------------*/
static void __attribute__ ((noreturn)) stat_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(stat_master) 
    {
        thread_master_free (stat_master);
    }

    if(zlog_default)  
    {
        closezlog(zlog_default);
    }
    exit(status);
}


/* SIGHUP handler. */
static void sighup(void)
{
    zlog_info("SIGHUP received");
}

/* SIGINT handler. */
static void sigint(void)
{
    zlog_notice("Terminating on signal SIGINT");
    exit(0);
}

/* SIGTERM handler. */
static void sigterm(void)
{
    zlog_notice("Terminating on signal SIGTERM");

    stat_exit(0);
}



struct quagga_signal_t statis_signals[] =
{
    {
        .signal  = SIGHUP,
        .handler = &sighup,
        .caught  = 0,
    },
    {
        .signal  = SIGINT,
        .handler = &sigint,
        .caught  = 0,
    },
    {
        .signal  = SIGTERM,
        .handler = &sigterm,
        .caught  = 0,
    },
    {
        .signal  = SIGALRM,
        .handler = &high_pre_timer_dotick,
        .caught  = 0,
    },
};


/*-------------------------------------- end ------------------------------------*/ 


#if 0
/* SIGHUP handler. */
static void sighup ( int sig )
{
    zlog_notice ( "SIGHUP received" );

    /* Reload config file */
    //vty_read_config ( config_file, config_default );

    /* Create VTY's socket */
    vty_serv_sock ( STATIS_VTYSH_PATH );

    /* Try to return to normal operation. */
}

/* SIGINT handler. */
static void sigint ( int sig )
{
    zlog_notice ( "SIGINT received\n" );

    statis_die();

    exit ( 1 );
}

/* SIGUSR1 handler. */
static void sigusr1 ( int sig )
{
    zlog_notice ( "SIGUSR1 received\n" );

    return;
}

/* SIGUSR2 handler. */
static void sigusr2 ( int sig )
{
    zlog_notice ( "SIGUSR2 received\n" );

    return;
}


/* Initialization of signal handles. */
static void _signal_init ()
{
    signal_set ( SIGHUP, sighup );
    signal_set ( SIGINT, sigint );
    signal_set ( SIGTERM, sigint );
    signal_set ( SIGPIPE, SIG_IGN );
    signal_set ( SIGUSR1, sigusr1 );
    signal_set ( SIGUSR2, sigusr2 );
}

#endif


static void gpnStatInit(void)
{
	//ipc_connect_common();	
	//_signal_init ();

	mem_share_attach();	/* 非第一个启动进程需调用attach */

	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(1000, MODULE_ID_STAT);

	/* 消息接收初始化 */
	if(ipc_recv_init(stat_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"StatisIpcRev", MODULE_ID_STAT, SCHED_OTHER, -1, stat_msg_rcv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }

	/* 启动基准定时器 */
    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }

	/* 添加定时器回调，ms */	
	high_pre_timer_add((char *)"GpnStatCoreProcTimer", LIB_TIMER_TYPE_LOOP, gpnStatCoreProc, NULL, 1000);

	cmd_init ( 1 );
	/* Create VTY's socket */
	vty_init(stat_master);
    vty_serv_sock(STATIS_VTYSH_PATH);

	statis_cmd_init();
}

INT32 main(INT32 argc, INT8 **argv)
{
	UINT32 daemon_mode;
	INT32 ch;
	INT8 *progname;
	INT8 *p;
	struct thread thread;

	/*default starup NOT in daemon mode*/
	daemon_mode = GPN_STAT_GEN_NO;
	
	/* Set umask before anything for security */
	umask (0027);

	/* Get program name. */
  	progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);  

	struct sched_param param;
	
	cpu_set_t mask; 
		
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1*/ 
	sched_setaffinity(0, sizeof(mask), &mask);

	param.sched_priority = 30;
#ifndef HAVE_KERNEL_3_0		
	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif
	/* First of all we need logging init. */
	zlog_default = openzlog(progname, ZLOG_STATIS,
		LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
	
	while((ch=getopt(argc, argv, "vhd-")) != -1)
	{
		switch(ch)
		{
			case 'v':
				fprintf (stderr, "%s: V%xR%xC%xB%x\n",\
					progname, gGpnStatVersion, gGpnStatRelease, gGpnStatChange, gGpnStatBuild);
				exit(0);

			case 'h':
				gpnStatHelp(0, progname);
				exit(0);
				
			case 'd':
				daemon_mode = GPN_STAT_GEN_YES;
				break;

			case '-':
				break;

			default:
				gpnStatHelp(1, progname);
				exit(0);
		}
	}

	
	/* 创建主线程 */
	stat_master = thread_master_create ();

	/* 初始化信号 */
	signal_init(stat_master, array_size(statis_signals), statis_signals);

	/*process unique check*/
	gpnProcessUniqueRunCheck(progname);

	if(daemon_mode == GPN_STAT_GEN_YES)
	{
		gpnStatDaemonize(1, 1);
	}

	memory_init();
	
	gpnStatInit();

	/*if or not in daemon */
	gpnStatStarUp();

	pid_output ( pid_file ); /* Pid file create. */	  	

	 /* Execute each thread. */
    while (thread_fetch(stat_master, &thread))
    {
    	thread_call ( &thread ); 
	}

    stat_exit(0);
	
	return GPN_STAT_GEN_OK;
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_STAT_MAIN_C_ */

