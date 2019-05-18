#ifndef _GPN_ALM_MAN_C_
#define _GPN_ALM_MAN_C_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*base include*/
//#define INCLUDE_SUBAGENT_MODULE
/*log function include*/

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
#include "lib/getopt.h"
#include "lib/msg_ipc_n.h"
#include "lib/memshare.h"
#include "lib/hptimer.h"
#include "lib/sigevent.h"

#include "gpnLog/gpnLogFuncApi.h"

#include "devCoreInfo/gpnDevCoreInfoFunApi.h"

#include "socketComm/gpnSpecialWarrper.h"
#include "socketComm/gpnSockInclude.h"
#include "socketComm/gpnSockCommFuncApi.h"

/*process function include*/
#include "gpnAlmMain.h"
#include "gpnAlmTypeStruct.h"
#include "gpnAlmScan.h"
#include "gpnAlmApi.h"
#include "gpnAlmUnifyPath.h"
#include "gpnAlmTimeProc.h"
#include "gpnAlmCfg.h"
#include "gpnAlmDebug.h"
#include "gpnAlmMsgProc.h"
#include "gpnAlmScan.h"
#include "gpnAlmCfg.h"
#include "gpnAlmPowDown.h"
#include "gpnAlmCmd.h"
#include "gpnAlmMasterSlaveSync.h"
#include "gpnAlmMsgProc.h"

/*dataSheet method function include*/
/*subAgent function include*/
#include "gpnAlmTrapNotify.h"


#include "gpnSignal/gpnSignalPowDown.h"

/*temp add sdk-pain-check in gpn_alarm */
#include "gpnSignal/gpnSignalSdkPain.h"


#include "gpnAlmMsgIO.h"
#include "gpnAlmTrap.h"

#ifdef INCLUDE_SUBAGENT_MODULE
#include <signal.h>
//#include "snmp_subagent.h"
//#include "snmp_almMgmtMib_handler.h"

#endif


//leo add---------------------------begin

#define ALARM_DEFAULT_CONFIG   "alarm.conf"
#define PATH_ALARM_PID "/var/run/alarm.pid"

/* Process ID saved for use by init system */
char *pid_file = (char *)PATH_ALARM_PID;  /* defined in config.h */


#define GPN_ALM_GEN_NULL 			GEN_SYS_NULL
#define GPN_ALM_GEN_YES 			GEN_SYS_YES
#define GPN_ALM_GEN_NO 				GEN_SYS_NO

#define GPN_ALM_UP_ROM_NODE					GPN_UNIFY_PATH_ROM_NODE(config/gpn/gpn_alarm/)
#define GPN_ALM_UP_ROM_DIR(name)			GPN_ALM_UP_ROM_NODE#name 

#define GPN_ALM_UP_RAM_NODE					GPN_UNIFY_PATH_RAM_NODE(gpn/gpn_alarm/)
#define GPN_ALM_UP_RAM_DIR(name)			GPN_ALM_UP_RAM_NODE#name



int  agent_check_and_process(int block);


//leo add---------------------------end


/* gpn-alarm version discription                                                                 */
/* V1R0C0B4 :                                                                                          */
/* 1 : get fix port list from gpnDevFixPortInfo.conf                                       */
/* 2 : chang log path to gpn_log default path                                               */
/* 3 : use gpn_sock comm api                                                                    */
unsigned char gGpnAlmVersion = 2;
unsigned char gGpnAlmRelease = 0;
/* here specially for database change */
unsigned char gGpnAlmChange = 0;
unsigned char gGpnAlmBuild = 0;


struct thread_master *alarm_master = NULL;


/*socket communication global data:define in socket communication module*/
extern sockCommData gSockCommData;

#ifdef INCLUDE_SUBAGENT_MODULE
int subagent_running = 1;

void stop_subagent(int a)
{
	subagent_running = 0;
}
#endif


/*---------------------------------- 修改共享内存时修改, lipf, 2018/8/23 --------------------*/
static void __attribute__ ((noreturn)) alarm_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(alarm_master) 
    {
        thread_master_free (alarm_master);
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

    alarm_exit(0);
}



struct quagga_signal_t alarm_signals[] =
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



/*device, fix port ... alarmMan self notify*/
UINT32 gpnAlmSelfNotifyDevStatus(void)
{
	unsigned int sta;
	unsigned int devIndex;

	/*get device index */
	gpnDevApiGetDevIndex(&devIndex);
	//devIndex = DeviceIndex_Create( 1, 0 );
	devIndex = DeviceIndex_Create( 1 );
	
	/*get status, should use which method to get device status */
	sta = GPN_SOCK_MSG_DEV_STA_INSERT;

	/*notify device status to gpn_stat(self) */
	gpnSockIfmMsgTxDevStateNotify2Alm(devIndex, sta);
	
	return GPN_ALM_GEN_OK;
}

UINT32 gpnAlarmSelfNotifyFixPort(void)
{
	UINT32 devIndex;
	UINT32 portTpNum;
	UINT32 spaceSize;
	stAlmPortObjCL *pportObjCL;

	/*get fix port */
	gpnDevApiGetDevFixPortInfo(&portTpNum, NULL, 0);
	
	spaceSize = portTpNum * sizeof(stAlmPortObjCL);
	pportObjCL = (stAlmPortObjCL *)malloc(spaceSize);
	if(pportObjCL == NULL)
	{
		return GPN_ALM_GEN_ERR;
	}

	gpnDevApiGetDevFixPortInfo(NULL, (stIfInfo *)pportObjCL, spaceSize);

	/*get device index */
	gpnDevApiGetDevIndex(&devIndex);
	//devIndex = DeviceIndex_Create( 1, 0 );
	devIndex = DeviceIndex_Create( 1 );
	
	gpnSockIfmMsgTxFixPortCapabilityNotify2Alm(devIndex, pportObjCL, spaceSize);

	/*free malloc*/
	if(pportObjCL != NULL)
	{
		free(pportObjCL);
	}
	
	return GPN_ALM_GEN_OK;
}
static UINT32 gpnAlmMainInit()
{
	gpnAlmDataStInit();
	
	/* alm default config */
	gpnAlmCfgInitCfg();

	/* alm mxml config reload */
	/* ??? todo */
	
	return GPN_ALM_GEN_OK;
}
UINT32 gpnAlmLogInit(void)
{
	unsigned int reVal;
	
	/*INT8 *stderr = (INT8 *)("stderr");*/
	/*INT8 *stdout = (INT8 *)("stdout");*/
	/*INT8 *applog = (INT8 *)("applog");*/
	/*INT8 *callback = (INT8 *)("callback");*/
	/*INT8 *Lfile = (INT8 *)(GPN_DEFAULTL_LOG_ID);*/
	/*INT8 *Bfile = (INT8 *)(GPN_DEFAULTB_LOG_ID);*/
	/*INT8 *syslog = (INT8 *)("syslog");*/

	/* enable gpn_alarm default log use gpn_log mode */
	/* ???? */
	
	INT8 *Lfile = GPN_ALM_UP_RAM_DIR(gpn_alarm.log);
	reVal = gpnLogFuncApiLogEnable(Lfile, GPN_LOGHANDLER_LFILE, GPN_LOG_L_EMERG, GPN_LOG_L_WARNING);
	//reVal = gpnLogFuncApiLogEnable(Lfile, GPN_LOGHANDLER_LFILE, GPN_LOG_L_EMERG, GPN_LOG_L_DEBUG);
	if(reVal == GPN_ALM_GEN_ERR)
	{
		zlog_err("%s : enable %s log err\n\r",
			__FUNCTION__, Lfile);
		return GPN_ALM_GEN_ERR;
	}
	/*printf("%s : enable %s log ok\n\r",
				__FUNCTION__, Lfile);*/

	Lfile = GPN_ALM_UP_RAM_DIR(gpn_alarm_tmp.log);
	/*reVal = gpnLogFuncApiLogEnable(Lfile, GPN_LOGHANDLER_LFILE, GPN_LOG_L_NOTICE, GPN_LOG_L_NOTICE);
	if(reVal == GPN_ALM_GEN_ERR)
	{
		printf("%s : enable %s log err\n\r",
			__FUNCTION__, Lfile);
		return GPN_ALM_GEN_ERR;
	}*/
	
	return GPN_ALM_GEN_OK;
}


int gpnAlmStarUp()
{
	unsigned int reVal;
	//static unsigned int leo_Count = 0;

	/*debugGpnMemSmpMgtCheck();*/
	
	/*step 1:log function init*/
	gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_USER_MODE, 0x00000000);
	//gpnDebugApiSmpStelyInit(GPN_SMP_DEBUG_USER_MODE, 0xFFFFFFFF); //open all GPN_DEBUG
	/*step 2:gpn_up gpn_alarm path init*/
	gpnAlmUPPathInit();
	
	/*step 3:log function init*/
	gpnAlmLogInit();

	/*step 4:socket fd role instance init*/
#if 0
	reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_ALM, (usrMsgProc)gpnAlmSockMsgProc);
	if(reVal == GPN_ALM_GEN_ERR)
	{
		return GPN_ALM_GEN_ERR;
	}
	gpnLog(GPN_LOG_L_CRIT, "GPN_SOCKET init OK! funcRole is %08x\n\r", GPN_SOCK_ROLE_ALM);
#endif


	/*print communication socket init status
	debugGpnSockDisplayGlobalFdSet();*/

	/*step 5:process functions init*/
	/*alm struct init*/
	gpnAlmMainInit();
	
	/*stip 6:Agentx  SubAgent init:trap method init, should modify */
	//gpnAlmTrapMsgQuenInit();

	//debugGpnAlmTable();
	//return GPN_ALM_GEN_OK;

	/*step 7:device, fix port ... alarmMan self notify*/
	//gpnAlmSelfNotifyDevStatus();
	//gpnAlarmSelfNotifyFixPort();

	/*step 8:after all init, startup 1s timer */
#if 0
	gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(1, second), 0,\
		gpnAlm1SecondTimerPorc, 1/*1 second*/, 0);

	gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(2, second), 0,\
		(timerOutFuncPtr)alarmNotification, 0, 0);
#endif

	/*step 9:net-snmp subagent start */
#ifdef INCLUDE_SUBAGENT_MODULE
	signal(SIGTERM, stop_subagent);
	signal(SIGINT, stop_subagent);
	snmp_subAgent_init();
#endif

	/*step 10:net-snmp subagent start */
	gpnSignalFixPowDownSigHandler((SIGHANDLER)(gpnAlmPDRiseSIGHandler));

	/*step 11:ebable sdk-pain-check pain msg recv */
	//gpnNetlinkSdkPainProc();

	/*step 12:add handler for calendar time change process */
	gpnTimerAddCalendarChangeCallBack((calChgCallback)(gpnAlmCalendarTimeChangeProc), 0, 0);

	/*gpnTimerAddUserOpt(GPN_TIMR_TYPE_CYC, GPN_TIME_TO_TICKS(60, second), 0,\
		gpnSockCalendarTimeChgTest, 0, 0);*/

#ifdef GPN_ALM_INSERT_DEBUG
	/*gpnAlmNotifyStateDebug();
	gpnAlmPeerPortReplaceNotifyDebug();
	gpnAlmViewPortReplaceNotifyDebug();*/
#endif

#if 0
	while(subagent_running)
	{
		/* big sleep for cpu use-rata */
		usleep(30000);
		
		/*gpn_sock process, select opt (no block)*/
		/*gpnSockCommApiNoBlockProc();*/
		gpnSockCommApiSelectTimerNoBlockProc();

		/*net-snmp subagent process, select opt (no block)*/
		reVal = 10;
		while(reVal--)
		{
			snmp_subagent_tick(0);
			usleep(100);
		}
	}
#endif
	
#ifdef INCLUDE_SUBAGENT_MODULE
	snmp_subagent_shutdown();
#endif	

	return GPN_ALM_GEN_OK;
}

unsigned int gpnAlmHelp(int status, char *progname)
{
	if (status != 0)
	{
		fprintf(stderr, "Try `%s --help' for more information.\n", progname);
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
	return GPN_ALM_GEN_OK;
}

unsigned int gpnAlmDaemonize(unsigned int nochdir, unsigned int noclose)
{
	pid_t pid;
	//unsigned int ret;

	pid = fork();

	/* In case of fork is error. */
	if(pid < 0)
	{
		perror ("fork");
		return GPN_ALM_GEN_ERR;
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
		perror("setsid");
		return GPN_ALM_GEN_ERR;;
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
		int fd;

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

	return GPN_ALM_GEN_OK;
}

static void alarm_init (void)
{	
	/* 共享内存、IPC      系统初始化，第一个启动进程调用，2018/8/21 */	
    if(mem_share_init() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    
    if(ipc_init_n() == -1)
    {
       printf(" ipc queue init fail\r\n");
       exit(0);
    }

	//mem_share_attach();	/* 非第一个启动进程需调用attach */

	/* 定时器初始化，时间间隔单位ms */
    high_pre_timer_init(1000, MODULE_ID_ALARM);

	/* 消息接收初始化 */
	if(ipc_recv_init(alarm_master) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

	/* 启动接收线程 */
    if(ipc_recv_thread_start((char *)"AlarmIpcRev", MODULE_ID_ALARM, SCHED_OTHER, -1, alarm_msg_rcv, 0) == -1)
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
	high_pre_timer_add((char *)"GpnAlm1sProcTimer",    LIB_TIMER_TYPE_LOOP, gpnAlm1SecondTimerPorc, NULL, 1000);
	//high_pre_timer_add((char *)"GpnAlmSocketRcvTimer", LIB_TIMER_TYPE_LOOP, alarm_socket_msg_rcv,   NULL, 50);
	
	//ipc_init();              /* 初始化系统的 IPC */
	//ipc_connect_common();
    //alarm_thread_init();
    alarm_cmd_init();
	alarm_MS_sync_init();
}


int main(int argc, char **argv)
{
	unsigned int daemon_mode;
	int ch;
	char *progname;
	char *p;
	struct thread thread;

	/*default starup NOT in daemon mode*/
	daemon_mode = GPN_ALM_GEN_NO;
	
	/* Set umask before anything for security */
	umask (0027);

	/* Get program name. */
  	progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);


	/************************* lipf add ****************************/
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
	zlog_default = openzlog(progname, ZLOG_ALARM,
		LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
	
	/***********************************************************/
		
	
	while((ch=getopt(argc, argv, "vhd-")) != -1)
	{
		switch(ch)
		{
			case 'v':
				fprintf (stdout, "%s: V%02dR%02dC%02dB%02d\n",\
					progname, gGpnAlmVersion, gGpnAlmRelease, gGpnAlmChange, gGpnAlmBuild);
				exit(0);
				
			case 'h':
				gpnAlmHelp(0, progname);
				exit(0);
				
			case 'd':
				daemon_mode = GPN_ALM_GEN_YES;
				break;

			case '-':
				break;

			default:
				gpnAlmHelp(1, progname);
				exit(0);
		}
	}

	/* Prepare master thread. */
    alarm_master = thread_master_create ();

	/* 初始化信号 */
	signal_init(alarm_master, array_size(alarm_signals), alarm_signals);

	//_signal_init ();
	cmd_init ( 1 );
    vty_init ( alarm_master );
	memory_init();
	
	/*process unique check*/
	gpnProcessUniqueRunCheck(progname);

	if(daemon_mode == GPN_ALM_GEN_YES)
	{
		gpnAlmDaemonize(1, 1);
	}
	
	/*if or not in daemon "while(1)"*/
	gpnAlmStarUp();

	pid_output ( pid_file ); /* Pid file create. */

	vty_serv_sock(ALARM_VTYSH_PATH);

	alarm_init();
	
	/* Execute each thread. */
    while ( thread_fetch ( alarm_master, &thread ) )
    { thread_call ( &thread ); }

	alarm_exit(0);
	//exit(0);
	return GPN_ALM_GEN_OK;
}



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_ALM_MAN_C_ */

