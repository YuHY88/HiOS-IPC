/*
   file management for huahuan IPRAN platform
   edit by suxq
   date 2016-05-23
*/
#include "../config.h"
#include <zebra.h>
#include <lib/version.h>
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include <lib/module_id.h>
#include <lib/devm_com.h>
#include <lib/sys_ha.h>
#include "lib/hptimer.h"
#include "lib/pkt_buffer.h"
#include <sched.h>
#include "getopt.h"
#include "thread.h"
#include "log.h"
#include "command.h"
#include "vty.h"
#include "memory.h"
#include "if.h"
#include "filter.h"
#include "prefix.h"
#include "plist.h"
#include "privs.h"
#include "sigevent.h"
#include "pid_file.h"
#include "vrf.h"

#include "filem.h"
#include "filem_tran.h"
#include "filem_cmd.h"
#include "filem_img.h"
#include "filem_slot.h"
#include "filem_sync.h"
#include "filem_tftp.h"
#define FILEM_DEFAULT_CONFIG  "filem.conf.sample"

/*-----------------------------------------------------------------------------------------*/

zebra_capabilities_t _caps_p [] =
{
    ZCAP_NET_RAW,
    ZCAP_BIND
};

struct zebra_privs_t filmd_privs =
{
#if defined(QUAGGA_USER)
    .user = QUAGGA_USER,
#endif
#if defined QUAGGA_GROUP
    .group = QUAGGA_GROUP,
#endif
#ifdef VTY_GROUP
    .vty_group = VTY_GROUP,
#endif
    .caps_p = _caps_p,
    .cap_num_p = 2,
    .cap_num_i = 0
};

struct option longopts[] = 
{
    { "daemon",      no_argument,       NULL, 'd'},
    { "config_file", required_argument, NULL, 'f'},
    { "pid_file",    required_argument, NULL, 'i'},
    { "socket",      required_argument, NULL, 'z'},
    { "vty_addr",    required_argument, NULL, 'A'},
    { "vty_port",    required_argument, NULL, 'P'},
    { "user",        required_argument, NULL, 'u'},
    { "group",       required_argument, NULL, 'g'},
    { "version",     no_argument,       NULL, 'v'},
    { "help",        no_argument,       NULL, 'h'},
    { 0 }
};

/* Configuration file and directory. */
//char *config_default = SYSCONFDIR FILEM_DEFAULT_CONFIG;

/* program name. */
char *progname;

/* is daemon? */
int   daemon_mode = 0;

/* Master of threads. */
struct thread_master *filemmaster;

/* Process ID saved for use by init system */
const char *pid_file = PATH_FILEM_PID;


/*-----------------------------------------------------------------------------------------*/

/* Help information display. */
static void usage(char *progname, int status)
{
    if(status != 0)
        fprintf (stderr, "Try `%s --help' for more information.\n", progname);
    else
    {    
        printf("Usage : %s [OPTION...]\n\n\
Daemon which manages FILEM version 3.\n\n\
-d, --daemon       Runs in daemon mode\n\
-f, --config_file  Set configuration file name\n\
-i, --pid_file     Set process identifier file name\n\
-z, --socket       Set path of zebra socket\n\
-A, --vty_addr     Set vty's bind address\n\
-P, --vty_port     Set vty's port number\n\
-u, --user         User to run as\n\
-g, --group        Group to run as\n\
-v, --version      Print program version\n\
-h, --help         Display this help and exit\n\n\
Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
    }
    exit(status);
}

/*-----------------------------------------------------------------------------------------*/

static void __attribute__ ((noreturn)) filem_exit(int status)
{
  //vrf_terminate();
    vty_terminate();

    if(filemmaster) 
    {
        thread_master_free (filemmaster);
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

    filem_exit(0);
}

/* SIGUSR1 handler. */
/*
static void sigusr1(void)
{
    zlog_info("SIGUSR1 received");

    zlog_rotate(NULL);
}
*/
struct quagga_signal_t filem_signals[] =
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

static void filem_msg_snmp_reply(struct ipc_msghdr_n *pReqHd,
                                 void *pData, int iDatLen, unsigned int iIndex)
{
    struct ipc_mesg_n *pMsgRep = NULL;

    if(pData == NULL) iDatLen = 0;

    pMsgRep = mem_share_malloc(sizeof(struct ipc_msghdr_n) + iDatLen, MODULE_ID_FILE);

    if(pMsgRep == NULL) return;   //如果申请不到内存,对端只能等待超时

    memset(pMsgRep, 0, sizeof(struct ipc_msghdr_n));

    pMsgRep->msghdr.data_len    = iDatLen;
    pMsgRep->msghdr.opcode      = IPC_OPCODE_REPLY;
    pMsgRep->msghdr.msg_index   = iIndex;
    pMsgRep->msghdr.data_num    = 1;
  
    if(pData != NULL) memcpy(pMsgRep->msg_data, pData, iDatLen);
    
    if(ipc_send_reply_n1(pReqHd, pMsgRep, sizeof(struct ipc_msghdr_n) + iDatLen) == -1)
    {
        mem_share_free(pMsgRep, MODULE_ID_FILE);
    }
}


static void filem_msg_snmp_request(struct ipc_msghdr_n *pMsgHd)
{
    int ret = 0;
    filemsnmp    fileinfo;
    filemstatus *pSysCfg = NULL;
    
    if((pMsgHd->sender_id != MODULE_ID_SNMPD) ||
       (pMsgHd->module_id != MODULE_ID_FILE)) 
    {
       return;
    }

  //zlog_debug(FILEM_DEBUG_OTHER, "filem_msg_snmp_request: get msg from snmp, subtype = %d \n", pMsgHd->msg_subtype);
       
    switch(pMsgHd->msg_subtype)
    {
        case FILEM_SNMP_GET_SYSCONFIG:

             pSysCfg = filem_slot_stanextget(pMsgHd->msg_index, pMsgHd->opcode);

             if(pSysCfg == NULL)
             {
                  filem_msg_snmp_reply(pMsgHd, NULL, 0, 0);
                    
                  zlog_err("%s[%d]: filem_msg_recv: get verinfo by index error,index=%d, code=%d\r\n", 
                            __FILE__,__LINE__, (int)pMsgHd->msg_index, (int)pMsgHd->opcode);  
             }
             else
             {
                  filem_diskspace_get();
                 
                  filem_msg_snmp_reply(pMsgHd, pSysCfg, sizeof(filemstatus), pMsgHd->msg_index);
             }
             
             break;
             
        case FILEM_SNMP_GET_STATUS:

             pSysCfg = filem_slot_stainfoget(0);

             filem_msg_snmp_reply(pMsgHd, pSysCfg, sizeof(filemstatus), 0);

             break;
             
        case FILEM_SNMP_GET_FILELIST:  
            
            ret = filem_show_byindex(&fileinfo, pMsgHd->msg_index, pMsgHd->opcode);

            if(ret < 0)
            {
                filem_msg_snmp_reply(pMsgHd, NULL, 0, 0);
                zlog_err("%s[%d]: filem_msg_recv: get file by index error \n", __FILE__,__LINE__);  
            }
            else
            {
                filem_msg_snmp_reply(pMsgHd, (void *)&fileinfo, sizeof(filemsnmp), pMsgHd->msg_index);
            }
            break;
            
        default:  break;
    }
}


static void filem_msg_hatypemsg(struct ipc_mesg_n *pMsg, int revln)
{
    switch(pMsg->msghdr.msg_subtype)
    {
        case HA_SMSG_BATCH_START:
            // printf("filem_msg_hatypemsg, batch_cconfig req\r\n");
             if(filem_slot_ismaster()) 
             {
                 filem_sync_rtfilerequest((char*)pMsg->msg_data, IPC_OPCODE_ADD, 0); /*配置数据批备请求 来自VTY*/
             }

             break;

        case HA_SMSG_TXFILE_REQ:
            //MODULE_ID_SGHMGT
             if((filem_slot_ismaster()) && 
               ((pMsg->msghdr.opcode == IPC_OPCODE_ADD) || 
                (pMsg->msghdr.opcode == IPC_OPCODE_DELETE) ||
                (pMsg->msghdr.opcode == IPC_OPCODE_GET)))
             {
                 filem_sync_rtfilerequest((char*)pMsg->msg_data, pMsg->msghdr.opcode, 1); /*配置数据批备请求 来自VTY*/
             }

             break;

        case HA_SMSG_REALT_CMD:
               
             filem_cmd_recv_command(pMsg, revln);

             break;

        case HA_SMSG_FILE_REQ:
        case HA_SMSG_FILE_REQACK:
        case HA_SMSG_FILE_DAT:
        case HA_SMSG_FILE_DATACK:
        case HA_SMSG_FILE_END:
        case HA_SMSG_FILE_ENDACK:
               
             filem_tran_btbreceive(pMsg, revln);

             break;

        case HA_SMSG_DATA_REPORT:
                         
             filem_sync_packetrecv(pMsg, revln);

             break;
              
        default:break;
    }
}


#if 0
/* 消息处理 */
static int filem_msg_recv()
{
    int        retva = 0;
    int        revln = 0;
    
    static int ipc_filem_id = -1;
    
    static struct ipc_pkt mesg;
    

    if(ipc_filem_id < 0)
    {
        ipc_filem_id = ipc_connect(IPC_MSG_FILEM);

        if(ipc_filem_id < 0) goto out;
    }

    retva = ipc_recv_pkt(ipc_filem_id, &mesg, MODULE_ID_FILE);   

    if(retva <= 0)
    {
        usleep(5000);
        goto out;
    }
    /*
    zlog_debug(FILEM_DEBUG_OTHER, "filem recv msg: msg_type=%d, opcode=%d msg_subtype=%d datlen=%d\r\n",
                 mesg.msghdr.msg_type, mesg.msghdr.opcode, mesg.msghdr.msg_subtype, retva);      
    */
    revln = (int)mesg.msghdr.data_len + IPC_HEADER_LEN; 

    if(revln > (retva + 4))
    {
        zlog_debug(FILEM_DEBUG_OTHER, "filem recv msg: datalen error, msghdr.data_len=%d, msgrcv return len = %d\n", revln, retva + 4);

        printf("filem recv msg: datalen error, msghdr.data_len=%d, msgrcv return len = %d\n", revln, retva + 4);

        goto out;
    }


    /* 消息处理 */
    switch (mesg.msghdr.msg_type)
    {
        case IPC_TYPE_SNMP:

             filem_msg_snmp_request(&mesg.msghdr);
             break;

        case IPC_TYPE_DEVM:
            
             filem_slot_devmmsg(&mesg, revln);
             break;
             
        case IPC_TYPE_HA:

             filem_msg_hatypemsg(&mesg, revln);
             
             break;
         default:
             zlog_debug(FILEM_DEBUG_OTHER, "filem_msg_recv, receive unk message\r\n");
             break;
    }
    
out:
  //usleep(5000);
    thread_add_event(filemmaster, filem_msg_recv, NULL, 0);
    
    return retva;
}
#endif
static int filem_msg_recv(struct ipc_mesg_n *pmsg, int imlen)
{
    int retva = 0;
    int revln = 0;

    zlog_debug(FILEM_DEBUG_OTHER, "filem recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        /* 消息处理 */
        switch(pmsg->msghdr.msg_type)
        {
            case IPC_TYPE_SNMP:

                 filem_msg_snmp_request(&pmsg->msghdr);
                 break;

            case IPC_TYPE_DEVM:
                
                 filem_slot_devmmsg(pmsg, revln);
                 break;
                 
            case IPC_TYPE_HA:

                 filem_msg_hatypemsg(pmsg, revln);
                 
                 break;
            
            case IPC_TYPE_PACKET:
                 if(filem_ftp_isruning())
                 {
                     filem_ftp_recv_ipcpacket(pmsg, revln);
                 }
                 else
                 {
                     filem_tftp_ipcpkt_receive(pmsg, revln);
                 }
                    
                 break;
            case IPC_TYPE_TRIGER:
            
                 if(filem_ftp_isruning()) filem_ftp_send_notify();
                 break;
                 
            case IPC_TYPE_FTM:
            
                 zlog_debug(FILEM_DEBUG_OTHER, "connect noti subtype=%d, code=%d\r\n", pmsg->msghdr.msg_subtype, (int)pmsg->msghdr.opcode);
                 
                 if(pmsg->msghdr.msg_subtype == PKT_SUBTYPE_TCP_CONNECT)
                 {
                     filem_ftp_connect_notimsg((char*)pmsg->msg_data, (int)pmsg->msghdr.data_len, (int)pmsg->msghdr.opcode);
                 }
                 break;

            default:
                 zlog_debug(FILEM_DEBUG_OTHER, "filem_msg_recv, receive unk message\r\n");
                 break;
        }
    }
    else
    {
         zlog_debug(FILEM_DEBUG_OTHER, "filem recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

    mem_share_free(pmsg, MODULE_ID_FILE);

    
    return retva;
}


/* IPC 初始化 */
static void filem_ipc_init(void)
{   
  //ret = ipc_connect_common();

  //thread_add_event(filemmaster, filem_msg_recv, NULL, 0);
   
    devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_FILE, MODULE_ID_FILE);
    devm_event_register(DEV_EVENT_SLOT_ADD, MODULE_ID_FILE, MODULE_ID_FILE);
    devm_event_register(DEV_EVENT_SLOT_DELETE, MODULE_ID_FILE, MODULE_ID_FILE);

    return ;
}


/* 主函数 */
int main(int argc, char *argv[], char *envp[])
{
    char  *ptr;
    int    opt;
    char  *vty_addr = NULL;
  //int    vty_port = 0;
    char  *config_file = NULL;
    struct thread thread;
    //struct sched_param param;
    cpu_set_t mask; 
    
  /* 初始化 umask */
    umask(0027);

  /* Preserve name of myself. */
    progname = ((ptr = strrchr (argv[0], '/')) ? ++ptr : argv[0]);
    
    CPU_ZERO(&mask);        
    CPU_SET(1, &mask);   /* 绑定cpu1*/ 
    sched_setaffinity(0, sizeof(mask), &mask);

    /* 设置进程调度优先级 */
    /*param.sched_priority = 30;
    if (sched_setscheduler(0, SCHED_RR, &param)) 
    {   
        perror("\n  priority set: ");
    }*/
    /* 参数解析 */
    while(1) 
    {
        opt = getopt_long(argc, argv, "df:i:z:hp:A:P:u:g:vC", longopts, 0);
    
        if(opt == EOF) break;

        switch(opt) 
        {
            case 0:
                 break;
                 
            case 'd':
                 daemon_mode = 1;
                 break;
                 
            case 'f':
                 config_file = optarg;
                 break;
                 
            case 'A':
                 vty_addr = optarg;
                 break;
                 
            case 'i':
                 pid_file = optarg;
                 break;

            case 'z':
                 //zclient_serv_path_set (optarg);
                 break;

            case 'P':
#if 0
                 if(strcmp(optarg, "0") == 0)
                 {
                    vty_port = 0;
                 }
                 else
                 {
                    vty_port = atoi (optarg);
                    if(vty_port <= 0 || vty_port > 0xffff)
                    {
                        vty_port = FILEM_VTY_PORT;
                    }
                 }
#endif
                 break;

            case 'u':
                 filmd_privs.user = optarg;
                 break;
                 
            case 'g':
                 filmd_privs.group = optarg;
                 break;
                 
            case 'v':
                 print_version(progname);
                 exit(0);
                 break;
                 
            case 'h':
                 usage(progname, 0);
                 break;
                 
            default:
                 usage(progname, 1);
                 break;
        }
    }

    /* 是否为root用户 */
    /*
    if(geteuid() != 0)
    {
        errno = EPERM;
        perror(progname);
        exit(1);
    }
    */
    /* 创建主线程 */
    filemmaster = thread_master_create();

    /* 初始化 syslog */
    zlog_default = openzlog(progname, ZLOG_FILEM,
                           LOG_CONS|LOG_NDELAY|LOG_PID,
                           LOG_DAEMON);
    /* 初始化信号 */
    signal_init(filemmaster, array_size(filem_signals), filem_signals);


    /* 公用数据初始化 */
    cmd_init(1);
    vty_init(filemmaster);
    memory_init();

  //mem_share_init();
    if(mem_share_attach() == -1)
    {
       printf(" share memory init fail\r\n");
       exit(0);
    }
    /*
    if(ipc_init_n() == -1)
    {
       printf(" ipc queue init fail\r\n");
       exit(0);
    }
    */
    high_pre_timer_init(1000, MODULE_ID_FILE);

    /* 创建守护进程 */
    if(daemon_mode) daemon (0, 0);  

    /* 创建 pid 文件 */
    pid_output(pid_file);
    
    vty_serv_sock(FILEM_VTYSH_PATH);
                                                                                                                                                                                                                                                                        
    /* filem 初始化 */
    filem_init();

    filem_ipc_init();

    /* 创建 vty socket */
    /* vty_serv_sock(FILEM_VTYSH_PATH);*/

    if(ipc_recv_init(filemmaster) == -1)
    {
        printf(" ipc receive init fail\r\n");
        exit(0);
    }

    if(ipc_recv_thread_start("FileIpcRev", MODULE_ID_FILE, SCHED_OTHER, -1, filem_msg_recv, 0) == -1)
    {
        printf(" ipc receive thread start fail\r\n");
        exit(0);
    }


    if(high_pre_timer_start() == -1)
    {
       printf(" timer start fail\r\n");
       exit(0);
    }
    
    /* 执行主线程 */
    while(thread_fetch (filemmaster, &thread)) thread_call (&thread);

    /* 进程退出 */
    zlog_warn("Thread failed");

    filem_exit(0);
    return(0);
}


