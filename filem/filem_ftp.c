/*------------------------------------------------------------------------*/
/* 文 件 名 : filem_ftp.c                                                   */
/* 作    者 : suxiqing                                                    */
/* 创作日期 : 2018-12-25                                                  */
/* 产    权 :                                                             */
/* 说    明 : 通过TFTP协议下载或上载文件的实现                            */
/*------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h> 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/timer.h>
#include <lib/log.h>
#include <lib/inet_ip.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include "ftm/pkt_ip.h"
#include "ftm/pkt_udp.h"
#include "ftm/pkt_tcp.h"
#include "filem.h"
#include "filem_tftp.h"
#include "filem_md5.h"
#include "filem_oper.h"

/*====================================================================================================================*/

#define FILEM_FTP_SVR_LISPORT      21
#define FILEM_FTP_CLI_LISPORT      4020    /*4020 - 4120*/
#define FILEM_FTP_CLI_CONPORT      5020    /*5020 - 5120*/


#define FILEM_FTP_NOACT            0       /* 无操作 */
#define FILEM_FTP_DNLOAD           1       /* 下载文件 */
#define FILEM_FTP_UPLOAD           2       /* 上传文件 */

#define FILEM_FTP_ERRBUF_LEN       256     // > 128
#define FILEM_FTP_CMDBUF_LEN       1024
#define FILEM_FTP_DATBUF_LEN       1024
#define FILEM_FTP_PACKET_LEN       1024


#define FILEM_FTP_CMD_TIMEOUT      8
#define FILEM_FTP_DAT_TIMEOUT      600
#define FILEM_FTP_END_TIMEOUT      5

#define FILEM_FTP_RTY_REPEAT       3
#define FILEM_FTP_RTY_TIMEOUT      2

//#define FILEM_FTP_FILE_MAXSIZE     0x2000000

#define FILEM_FTP_RCV_UNKWDATA     0
#define FILEM_FTP_RCV_CTRLDATA     1
#define FILEM_FTP_RCV_FILEDATA     2

/*====================================================================================================================*/

/*命令状态机*/
enum
{   
    FILEM_FTP_CMD_ABOR = 1,
    FILEM_FTP_CMD_ACCT,
    FILEM_FTP_CMD_ALLO,           //no support
    FILEM_FTP_CMD_APPE,           //no support
    FILEM_FTP_CMD_CDUP,           //no support
    FILEM_FTP_CMD_CWD,  
    FILEM_FTP_CMD_DELE,
    FILEM_FTP_CMD_HELP,
    FILEM_FTP_CMD_LIST,
    FILEM_FTP_CMD_MODE,
    FILEM_FTP_CMD_MKD,            //no support  
    FILEM_FTP_CMD_NLST,           //no support
    FILEM_FTP_CMD_NOOP,  
    FILEM_FTP_CMD_PASS,   
    FILEM_FTP_CMD_PASV,  
    FILEM_FTP_CMD_PORT,  
    FILEM_FTP_CMD_PWD,  
    FILEM_FTP_CMD_QUIT,  
    FILEM_FTP_CMD_REIN,            //no support
    FILEM_FTP_CMD_REST,            //no support
    FILEM_FTP_CMD_RETR,  
    FILEM_FTP_CMD_RMD,  
    FILEM_FTP_CMD_RNFR,            //no support
    FILEM_FTP_CMD_RNTO,            //no support
    FILEM_FTP_CMD_SITE,            //no support
    FILEM_FTP_CMD_SIZE,
    FILEM_FTP_CMD_SMNT,            //no support
    FILEM_FTP_CMD_STAT,   
    FILEM_FTP_CMD_STOR,   
    FILEM_FTP_CMD_STOU,   
    FILEM_FTP_CMD_STRU,   
    FILEM_FTP_CMD_SYST,   
    FILEM_FTP_CMD_TYPE,   
    FILEM_FTP_CMD_USER,   
    FILEM_FTP_CMD_CONN,
    FILEM_FTP_CMD_DATTRS,          //WAIT DATA CONNECT
    FILEM_FTP_CMD_DATEND,
    FILEM_FTP_CMD_SESEND,          //END SESSION
};  
/*
    ABOR    中断数据连接程序
    ACCT <account>  系统特权帐号
    ALLO <bytes>    为服务器上的文件存储器分配字节
    APPE <filename>     添加文件到服务器同名文件
    CDUP <dir path>     改变服务器上的父目录
    CWD <dir path>  改变服务器上的工作目录
    DELE <filename>     删除服务器上的指定文件
    HELP <command>  返回指定命令信息
    LIST <name>     如果是文件名列出文件信息，如果是目录则列出文件列表
    MODE <mode>     传输模式（S=流模式，B=块模式，C=压缩模式）
    MKD <directory>     在服务器上建立指定目录
    NLST <directory>    列出指定目录内容
    NOOP    无动作，除了来自服务器上的承认
    PASS <password>     系统登录密码
    PASV    请求服务器等待数据连接
    PORT <address>  IP 地址和两字节的端口 ID
    PWD     显示当前工作目录
    QUIT    从 FTP 服务器上退出登录
    REIN    重新初始化登录状态连接
    REST <offset>   由特定偏移量重启文件传递
    RETR <filename>     从服务器上找回（复制）文件
    RMD <directory>     在服务器上删除指定目录
    RNFR <old path>     对旧路径重命名
    RNTO <new path>     对新路径重命名
    SITE <params>   由服务器提供的站点特殊参数
    SMNT <pathname>     挂载指定文件结构
    STAT <directory>    在当前程序或目录上返回信息
    STOR <filename>     储存（复制）文件到服务器上
    STOU <filename>     储存文件到服务器名称上
    STRU <type>     数据结构（F=文件，R=记录，P=页面）
    SYST    返回服务器使用的操作系统
    TYPE <data type>    数据类型（A=ASCII，E=EBCDIC，I=binary）
    USER <username>     系统登录的用户名

    110 	新文件指示器上的重启标记
    120 	服务器准备就绪的时间（分钟数）
    125 	打开数据连接，开始传输
    150 	打开连接
    200 	成功
    202 	命令没有执行
    211 	系统状态回复
    212 	目录状态回复
    213 	文件状态回复
    214 	帮助信息回复
    215 	系统类型回复
    220 	服务就绪
    221 	退出网络
    225 	打开数据连接
    226 	结束数据连接
    227 	进入被动模式（IP 地址、ID 端口）
    230 	登录因特网
    250 	文件行为完成
    257 	路径名建立
    331 	要求密码
    332 	要求帐号
    350 	文件行为暂停
    421 	服务关闭
    425 	无法打开数据连接
    426 	结束连接
    450 	文件不可用
    451 	遇到本地错误
    452 	磁盘空间不足
    500 	无效命令
    501 	错误参数
    502 	命令没有执行
    503 	错误指令序列
    504 	无效命令参数
    530 	未登录网络
    532 	存储文件需要帐号
    550 	文件不可用
    551 	不知道的页类型
    552 	超过存储分配
    553 	文件名不允许
*/

/*====================================================================================================================*/
/*====================================================================================================================*/
/*===================================================*/
/*FTP传输实例控制块                                  */
/*===================================================*/
typedef struct _FILEM_FTP_ADDR_
{
    int              iIsReg;                  /*是否已注册*/
    int              iIpver;

    unsigned short   iSpare;
    unsigned short   iPort;
    
    unsigned int     iIpv4;
    unsigned char    bIpv6[16];
    
   char              bAddStr[FILEM_NAME_MAXLN]; /*存放地址字符串*/

}FILEM_FTP_ADDR;

typedef struct _FILEM_FTP_SOCK_
{
    int              iIsReg;                  /*是否已注册*/
    int              iIsCon;                  /*是否已连接*/
    int              iIpver;
    
    unsigned short   iSpare1;                 /*记录远端地址*/                 
    unsigned short   iDport;    
    unsigned int     iDipv4;
    unsigned char    bDipv6[16];
                                              /*记录本地地址*/   
    unsigned short   iSpare2;                 
    unsigned short   iSport;
    unsigned int     iSipv4;
    unsigned char    bSipv6[16];
}FILEM_FTP_SOCK;



typedef struct _FILEM_FTP_CTRL_
{
    int              iTrsType;                /*1 下载 2 上传 */
    
    int              iFileType;
    
    int              iFileID;                 /*文件句柄*/            
    int              iFileLen;                /*下载或上载文件长度  */  
    int              iCurrPos;                /*当前发送或接收位置  */


    int              iReqState;               /*请求状态机*/
    int              iReqTimeout;

  //int              iRetryTout;
    int              iRetryCoun;
    
    
    unsigned short   iL3Vpn;                  /*0-127         */
    
    FILEM_FTP_ADDR   sSvrAddr;                /*远端侦听地址*/

    FILEM_FTP_ADDR   sLocAddr;                /*本地侦听地址*/
   
    FILEM_FTP_SOCK   sCtrSock;                /*控制通道连接*/

    FILEM_FTP_SOCK   sDatSock;                /*数据通道连接*/

    int              iErrorCode;

    int              iEarlyCode;              /*提前收到结束代码*/
    
    int              iLastPcnt;
    void            *pProParam;
    FilemCall        pProcCall;
    

    int              iRecCmdLen;              /*控制链接收缓存      */
    int              iSndCmdLen;              /*控制链缓存      */ 
    int              iRecDatLen;              /*数据链接收缓存      */ 
    int              iSndDatLen;
    
    
    char             bUserName[FILEM_NAME_MAXLN];
    
    char             bPassWord[FILEM_NAME_MAXLN];
    
    char             bRemFname[FILEM_NAME_MAXLN]; 
                                           /*远端文件名 含路径   */
    char             bLocFname[FILEM_NAME_MAXLN]; 

    char             bLocTempN[FILEM_NAME_MAXLN];
                                           /*本地文件名 含路径   */
                                           /*保存错误信息        */ 
    char             bSvrErrBuf[FILEM_FTP_ERRBUF_LEN]; 
                                           
    char             bErrInfBuf[FILEM_FTP_ERRBUF_LEN];
                                           
    char             bOneCmdBuf[FILEM_FTP_CMDBUF_LEN];
                                           
    char             bRecCmdBuf[FILEM_FTP_CMDBUF_LEN];  
    
    char             bSndCmdBuf[FILEM_FTP_CMDBUF_LEN]; 
                                            /*文件数据收发缓存*/
    char             bRSDataBuf[FILEM_FTP_DATBUF_LEN];             
}FILEM_FTP_CTRL;

/*====================================================================================================================*/

//extern struct thread_master *filemmaster;

static unsigned short iFtpCtrPortIdx = 0;   /*循环使用0-99端口*/

FILEM_FTP_CTRL sFtpSessCtl = {0} ;

/*====================================================================================================================*/
/*====================================================================================================================*/
/*common fun*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_out_percent*/
/*Para: pftpctl */
/*Dest: output run percent infor */
/*---------------------------------------------------------*/

static void filem_ftp_out_percent(FILEM_FTP_CTRL *pftpCtl, int iForce)
{
    int  iPercnt = 0;
    char bProInf[128] = {0};
    
    if((pftpCtl->iFileLen > 0) && pftpCtl->pProcCall)
    {        
        iPercnt = (int)((float)pftpCtl->iCurrPos / pftpCtl->iFileLen * 100);
        
        if((pftpCtl->iLastPcnt != iPercnt) || iForce)
        {
            pftpCtl->iLastPcnt = iPercnt;
    
            sprintf(bProInf, "\033[1A""\033[K""Percent:    %3d%% \r\n", iPercnt);                  
    
            pftpCtl->pProcCall(0, bProInf, pftpCtl->pProParam, 0);      
        }
    }
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_is_decnumber*/
/*Para: pStr */
/*Dest: check a string is dec number or not  */
/*---------------------------------------------------------*/

static int filem_ftp_is_decnumber(char *pStr)
{
    int ulLen = 0;
    int ulIdx = 0;
    
    if(pStr == NULL) return(0);

    ulLen = strlen(pStr);

    if(ulLen == 0) return(0);
                                         /*检查是否全部为数字*/
    for(; ulIdx < ulLen; ulIdx++)
    {
        if((pStr[ulIdx] < '0') || (pStr[ulIdx] > '9')) break;
    }

    return((ulIdx < ulLen) ? 0 : 1);
}




/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_register_loccnt*/
/*Para: sIpAdd */
/*Dest: register connect sock to ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_register_loccnt(FILEM_FTP_SOCK *pSocket)
{
    int   retval = -1;

    if(!pSocket->iIsReg)
    {
        union proto_reg proto;
        
        pSocket->iIsReg = 1;

        memset(&proto, 0, sizeof(union proto_reg));
        
        proto.ipreg.protocol = IP_P_TCP;
        proto.ipreg.dport    = pSocket->iSport;   //des = loc
        
        if(pSocket->iIpver == INET_FAMILY_IPV4)
        {
             retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
        }
        else  
        {
             retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
        }
    }
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_unregister_loccnt*/
/*Para: sIpAdd */
/*Dest: unregister listen sock from ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_unregister_loccnt(FILEM_FTP_SOCK *pSocket)
{
    int retval = -1;

    if(pSocket->iIsReg)
    {
        union proto_reg proto;

        pSocket->iIsReg = 0;

        memset(&proto, 0, sizeof(union proto_reg));
        
        proto.ipreg.protocol = IP_P_TCP;
        proto.ipreg.dport    = pSocket->iSport;     //des == loc
        
        if(pSocket->iIpver == INET_FAMILY_IPV4)
        {
             pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
        }
        else  
        {        
             pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
        }
    }
    
    return(retval);
}


/*---------------------------------------------------------*/
/*Name: filem_ftp_register_loclis*/
/*Para: sIpAdd */
/*Dest: register listen sock to ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_register_loclis(FILEM_FTP_ADDR *sIpAdd)
{
    int   retval = 0;
    union proto_reg proto;

    sIpAdd->iIsReg = 1;
    
    memset(&proto, 0, sizeof(union proto_reg));
    
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport    = sIpAdd->iPort;   //des == loc
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4)
    {
         proto.ipreg.dip = sIpAdd->iIpv4;
         
         retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else  
    {
         memcpy(&proto.ipreg.dipv6, sIpAdd->bIpv6, 16);
        
         retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_unregister_loclis*/
/*Para: sIpAdd */
/*Dest: unregister listen sock from ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_unregister_loclis(FILEM_FTP_ADDR *sIpAdd)
{
    int   retval = 0;
    union proto_reg proto;

    sIpAdd->iIsReg = 0;
    
    memset(&proto, 0, sizeof(union proto_reg));
    
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport    = sIpAdd->iPort;
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4)
    {
         proto.ipreg.dip = sIpAdd->iIpv4;
         
         pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else  
    {
         memcpy(&proto.ipreg.dipv6, sIpAdd->bIpv6, 16);
        
         pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_connect_socket*/
/*Para: sIpAdd */
/*Dest: connect ro remote */
/*---------------------------------------------------------*/
static int filem_ftp_connect_socket(FILEM_FTP_ADDR *sIpAdd, unsigned short sPort)
{
    int   retval = 0;
	struct ip_proto proto;
        
    memset(&proto, 0, sizeof(struct ip_proto));
    
    proto.protocol = IP_P_TCP;
    proto.dport    = sIpAdd->iPort;
    proto.sport    = sPort;
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4) 
    {
         proto.type = PROTO_TYPE_IPV4;
        
         proto.dip  = sIpAdd->iIpv4;            //des = remote
         
         retval = pkt_open_tcp(MODULE_ID_FILE, &proto);
         
         zlog_debug(FILEM_DEBUG_FTP, " connect dip=%x, dport=%d, sport=%d,sip=%x, ret=%d\r\n", proto.dip, proto.dport, proto.sport,proto.sip, retval);
    }
    else //if(sIpAdd->iIpver == INET_FAMILY_IPV6)
    {
         proto.type = PROTO_TYPE_IPV6;

         memcpy(&proto.dipv6, sIpAdd->bIpv6, 16);
        
         retval = pkt_open_tcp(MODULE_ID_FILE, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_close_socket*/
/*Para: sIpAdd */
/*Dest: close a socket */
/*---------------------------------------------------------*/
static int filem_ftp_close_socket(FILEM_FTP_SOCK *pSock)
{
    int   retval = -1;
	struct ip_proto proto;

    if(pSock->iIsCon)
    {
        zlog_debug(FILEM_DEBUG_FTP, "Local close socket sport=%d\r\n", pSock->iSport);
        
        pSock->iIsCon = 0;
        
        memset(&proto, 0, sizeof(struct ip_proto));
  
        proto.protocol = IP_P_TCP;
        proto.dport    = pSock->iDport;
        proto.sport    = pSock->iSport;  
    
        if(pSock->iIpver == INET_FAMILY_IPV4)
        {
             proto.dip = pSock->iDipv4;
             proto.sip = pSock->iSipv4;   //des = remote
             
             retval = pkt_close_tcp(MODULE_ID_FILE, &proto);
        }
        else //if(sIpAdd->iIpv4 == INET_FAMILY_IPV6)
        {
             memcpy(&proto.dipv6, pSock->bDipv6, 16);
             
             memcpy(&proto.sipv6, pSock->bSipv6, 16);
             
             retval = pkt_close_tcp(MODULE_ID_FILE, &proto);
        }
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_accept_socket*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/

static int filem_ftp_accept_socket(FILEM_FTP_CTRL *pFtpCtl, 
                                   struct tcp_session *pSess, int iOptCod)
{
    int             iType = 0;
    FILEM_FTP_SOCK *pSock = NULL;

    zlog_debug(FILEM_DEBUG_FTP, "Ftp Accept socket, dip=%x dport=%d sip=%x sport=%d\r\n",
                 pSess->dip, pSess->dport, pSess->sip, pSess->sport);
                                                    //des=loc
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT)
    {
        if(pSess->dport == pFtpCtl->sLocAddr.iPort)
        {
             pSock = &pFtpCtl->sDatSock;
             iType = FILEM_FTP_RCV_FILEDATA;
        }
        
        if(pSess->dport == pFtpCtl->sCtrSock.iSport) 
        {
             pSock = &pFtpCtl->sCtrSock;
             iType = FILEM_FTP_RCV_CTRLDATA;
        }
     }

    if(pSock != NULL)
    {
        if(iOptCod == IPC_OPCODE_CONNECT)
        {
            zlog_debug(FILEM_DEBUG_FTP, "socket connect notify\r\n");

            if(!pSock->iIsCon)
            {
                pSock->iIsCon = 1;

                pSock->iIpver = INET_FAMILY_IPV4;
                
              //if(iType != FILEM_FTP_RCV_CTRLDATA)
                {
                pSock->iSport = pSess->dport;
                }
                pSock->iSipv4 = pSess->dip;

                pSock->iDport = pSess->sport;
                pSock->iDipv4 = pSess->sip;

                if(pSock->iIpver == INET_FAMILY_IPV6)
                {
                     memcpy(pSock->bSipv6, pSess->dipv6.ipv6, 16);
                     
                     memcpy(pSock->bDipv6, pSess->sipv6.ipv6, 16);
                }
            }
        }
        else
        {
            if(pSock->iIsCon)  
            {
               pSock->iIsCon = 0;
               
               if((iType == FILEM_FTP_RCV_FILEDATA) &&
                  (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
               {
                   pFtpCtl->iReqState  = FILEM_FTP_CMD_DATEND;
                   pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;

                   if((pFtpCtl->iTrsType == FILEM_FTP_UPLOAD) && 
                      (pFtpCtl->iCurrPos < pFtpCtl->iFileLen) && (pFtpCtl->iErrorCode == 0))
                   {
                        pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                   }
               }
            }
            
            zlog_debug(FILEM_DEBUG_FTP, "socket disconnect \r\n");
        }
    }
    return(iType);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_match_socket*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/

static int filem_ftp_match_socket(FILEM_FTP_CTRL *pFtpCtl, struct ip_control *iIpCon)
{
    //ipv4
    { // IPV4 
    if(pFtpCtl->sCtrSock.iIsCon)  
    {
        if((pFtpCtl->sCtrSock.iSipv4 == iIpCon->dip) &&
           (pFtpCtl->sCtrSock.iSport == iIpCon->dport))
        {
            return(FILEM_FTP_RCV_CTRLDATA);          //des = LOC
        }
    }

    if(pFtpCtl->sDatSock.iIsCon)  
    {
        if((pFtpCtl->sDatSock.iSipv4 == iIpCon->dip) &&
           (pFtpCtl->sDatSock.iSport == iIpCon->dport))
        {
             return(FILEM_FTP_RCV_FILEDATA);          
        }
    }
    }
    //{} IPV6
    return(FILEM_FTP_RCV_UNKWDATA);
}


/*---------------------------------------------------------*/
/*Name: filem_ftp_session_end*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/
static void filem_ftp_session_end(FILEM_FTP_CTRL *pftpCtl)
{
    if(pftpCtl->iFileID >= 0) filem_oper_close(pftpCtl->iFileID);

    if(pftpCtl->sDatSock.iIsCon) filem_ftp_close_socket(&pftpCtl->sDatSock);

    if(pftpCtl->sCtrSock.iIsCon) filem_ftp_close_socket(&pftpCtl->sCtrSock);

    if(pftpCtl->sLocAddr.iIsReg) filem_ftp_unregister_loclis(&pftpCtl->sLocAddr);

    if(pftpCtl->iTrsType == FILEM_FTP_DNLOAD) 
    {
        if(filem_oper_isexist(pftpCtl->bLocTempN)) filem_oper_remove(pftpCtl->bLocTempN);
    }
    
    if(pftpCtl->iErrorCode == 0)
    { 
         filem_ftp_out_percent(pftpCtl, 1);

         if(pftpCtl->iTrsType == FILEM_FTP_UPLOAD)  
         {
              filem_upload_stat_set(-FILEM_ERROR_FINISHED);
    
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp upload file %s Successfully\r\n", pftpCtl->bLocFname);
         }
         else 
         {
              filem_download_stat_set(-FILEM_ERROR_FINISHED);
              
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp download file %s Successfully\r\n", pftpCtl->bRemFname);
         }
    }
    else
    { 
         if(pftpCtl->iTrsType == FILEM_FTP_UPLOAD)  
         {
              filem_upload_stat_set(pftpCtl->iErrorCode);
              
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp upload file %s fail, %s, %s, datlen=%d\r\n",
                       pftpCtl->bLocFname,  filem_sys_errget(pftpCtl->iErrorCode), pftpCtl->bSvrErrBuf, pftpCtl->iCurrPos); 
         }
         else 
         {
              filem_download_stat_set(pftpCtl->iErrorCode);
                      
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp download file %s fail, %s, %s, datlen=%d\r\n",
                       pftpCtl->bRemFname, filem_sys_errget(pftpCtl->iErrorCode), pftpCtl->bSvrErrBuf, pftpCtl->iCurrPos); 
         }
    }

    zlog_err("%s[%d]: %s", __FILE__, __LINE__, pftpCtl->bErrInfBuf);

    if(pftpCtl->pProcCall)
    {
        pftpCtl->pProcCall(pftpCtl->iErrorCode, pftpCtl->bErrInfBuf, pftpCtl->pProParam, 1);           
    }
    
    if(pftpCtl->iFileType == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

    filem_ftp_unregister_loccnt(&pftpCtl->sCtrSock);

    memset(pftpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    filem_oper_flush();
}


/*---------------------------------------------------------*/
/*Name:filem_ftp_start_session                             */
/*Para:ftp_ctrl                                            */
/*Desc:发起连接开始会话                                            */
/*---------------------------------------------------------*/

static int filem_ftp_start_session(FILEM_FTP_CTRL *pFtpCtrl)
{
    pFtpCtrl->iReqState   = FILEM_FTP_CMD_CONN;
    pFtpCtrl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
        
    return(filem_ftp_connect_socket(&pFtpCtrl->sSvrAddr, pFtpCtrl->sCtrSock.iSport));
}

/*---------------------------------------------------------*/
/*Name:filem_ftp_clear_session                             */
/*Para:pFtpCtrl                                            */
/*Desc:清理会话队列                                            */
/*---------------------------------------------------------*/
/*
static void filem_ftp_clear_session(FILEM_FTP_CTRL *pFtpCtrl)
{
    struct pkt_buffer  *pPktBuf = NULL;
    
    while(1)
    {
         if((pPktBuf = pkt_rcv(MODULE_ID_FILE)) == NULL) break;
    }
}
*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_send_ctrlpkt                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送命令报文                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_ctrlpkt(FILEM_FTP_CTRL *pFtpCtl)
{
    union pkt_control pktcont;
    
    memset(&pktcont, 0, sizeof(union pkt_control));
    
    pktcont.ipcb.dip        = pFtpCtl->sCtrSock.iDipv4;
    pktcont.ipcb.dport      = pFtpCtl->sCtrSock.iDport;
    pktcont.ipcb.sip        = pFtpCtl->sCtrSock.iSipv4;
    pktcont.ipcb.sport      = pFtpCtl->sCtrSock.iSport;
    pktcont.ipcb.protocol   = IP_P_TCP;
    pktcont.ipcb.vpn        = pFtpCtl->iL3Vpn;
    pktcont.ipcb.ttl        = IP_TTL_DEFAULT;
    pktcont.ipcb.is_changed = 1;
    pktcont.ipcb.tos        = 3;              //des = remote
    pktcont.ipcb.chsum_enable= 1;

    return(pkt_send(PKT_TYPE_TCP, &pktcont, pFtpCtl->bSndCmdBuf, pFtpCtl->iSndCmdLen));
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_datapkt                          */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送数据报文                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_datapkt(FILEM_FTP_CTRL *pFtpCtl)
{
    union pkt_control pktcont;
    
    memset(&pktcont, 0, sizeof(union pkt_control));
    
    pktcont.ipcb.dip        = pFtpCtl->sDatSock.iDipv4;
    pktcont.ipcb.dport      = pFtpCtl->sDatSock.iDport;
    pktcont.ipcb.sip        = pFtpCtl->sDatSock.iSipv4;
    pktcont.ipcb.sport      = pFtpCtl->sDatSock.iSport;
    pktcont.ipcb.protocol   = IP_P_TCP;
    pktcont.ipcb.vpn        = pFtpCtl->iL3Vpn;
    pktcont.ipcb.ttl        = IP_TTL_DEFAULT;          //des = remote
    pktcont.ipcb.is_changed = 1;
    pktcont.ipcb.tos        = 3;
    pktcont.ipcb.chsum_enable= 1;

    return(pkt_send(PKT_TYPE_TCP, &pktcont, pFtpCtl->bRSDataBuf, pFtpCtl->iSndDatLen));
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_username                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送用户命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_username(FILEM_FTP_CTRL *pFtpCtl)
{
                                                /* send user name*/
    int iResult;
                                                
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "USER %s\r\n", pFtpCtl->bUserName); 

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
    
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_userpass                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送密码命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_userpass(FILEM_FTP_CTRL *pFtpCtl)
{
                                                /* send user name*/
    int iResult;
                                                
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "PASS %s\r\n", pFtpCtl->bPassWord); 

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
    
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_portcmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送端口命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_portcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int            iResult;
    unsigned int   iLocIp;
    unsigned short iPort;
    
    iLocIp = pFtpCtl->sLocAddr.iIpv4;
    iPort  = pFtpCtl->sLocAddr.iPort;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "PORT %u,%u,%u,%u,%u,%u\r\n", 
                                 ((iLocIp >> 24) & 0xff), ((iLocIp >> 16) & 0xff), 
                                 ((iLocIp >>  8) & 0xff), ((iLocIp >>  0) & 0xff),
                                 ((iPort  >>  8) & 0xff), ((iPort   >> 0) & 0xff));

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);

     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_typecmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送类型命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_typecmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*发送文件传送类型*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "TYPE I\r\n");

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_sizecmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送文件长度命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_sizecmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*发送文件传送类型*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "SIZE %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_quitcmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送终止会话命令                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_quitcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*发送文件传送类型*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "QUIT \r\n");

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}



/*---------------------------------------------------------*/
/* Name: filem_ftp_send_retrcmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送读取文件命令                                          */
/*---------------------------------------------------------*/
   
static int filem_ftp_send_retrcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int   iResult;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "RETR %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_storcmd                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: 发送写入文件命令                                          */
/*---------------------------------------------------------*/

static int filem_ftp_send_storcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int   iResult;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "STOR %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_check_dloadlen                         */
/* Para: pFtpCtl : FTP传送实例控制块                       */
/* Retu: 0  Ok  !=0 ERRORcode                             */
/*---------------------------------------------------------*/

static int filem_ftp_check_dnloadlen(FILEM_FTP_CTRL *pFtpCtl, char *pLenstr)
{
     char  cPath[FILEM_NAME_MAXLN] = {0};

     if(filem_ftp_is_decnumber(pLenstr))
     {
          pFtpCtl->iFileLen = atoi(pLenstr);
    
          if(pFtpCtl->iFileLen < 0) 
          {
              pFtpCtl->iErrorCode = -FILEM_ERROR_LENGTH;
          }
     }
    
     if(pFtpCtl->iErrorCode == 0)
     {
         filem_oper_cutpath(pFtpCtl->bLocFname, cPath);
         
         if(filem_oper_availdisk(cPath) < (unsigned int)pFtpCtl->iFileLen)
         {
              pFtpCtl->iErrorCode = -FILEM_ERROR_DISKFULL;
         }
     }

     return(pFtpCtl->iErrorCode);
}


/*---------------------------------------------------------*/
/* Name: filem_ftp_send_uploadfile                         */
/* Para: pFtpCtl    事例控制块                             */
/* Retu: 发送上传文件                                    */
/*---------------------------------------------------------*/
static int filem_ftp_send_uploadfile(FILEM_FTP_CTRL *pFtpCtl)
{
    int iRetry = 0;
    int iDelay = 0;

    filem_oper_seek(pFtpCtl->iFileID, pFtpCtl->iCurrPos, SEEK_SET);
    
    while((pFtpCtl->sDatSock.iIsCon) &&
          (pFtpCtl->iCurrPos < pFtpCtl->iFileLen))   
    {
        pFtpCtl->iSndDatLen = FILEM_FTP_DATBUF_LEN;
        
        pFtpCtl->iSndDatLen = filem_oper_read(pFtpCtl->iFileID, pFtpCtl->bRSDataBuf, pFtpCtl->iSndDatLen);

        if(pFtpCtl->iSndDatLen < 0) 
        {
             pFtpCtl->iErrorCode = -FILEM_ERROR_READ;
             break;
        }
 
        if(filem_ftp_send_datapkt(pFtpCtl) != 0)
        {
            iRetry = 1; break;
        }

        pFtpCtl->iRetryCoun = 0;
             
        pFtpCtl->iCurrPos += pFtpCtl->iSndDatLen;

        if(++iDelay > 10) break;
    }
    
    filem_ftp_out_percent(pFtpCtl, 0);

    return(iRetry);
   
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_check_dnloadfile                        */
/* Desc: 检查下载文件                                            */
/*---------------------------------------------------------*/
static int filem_ftp_check_dnloadfile(FILEM_FTP_CTRL *pFtpCtl)
{
     int iResult = 0;
     
     iResult = filem_md5_matchcheck(pFtpCtl->bLocTempN, 0);
                        
     if(iResult == 0) 
     {
          if(filem_oper_isexist(pFtpCtl->bLocFname))
          {
              filem_oper_remove(pFtpCtl->bLocFname);
              usleep(100000);
          }
          
          filem_oper_rename(pFtpCtl->bLocTempN, pFtpCtl->bLocFname);
     }          
     else
     {
          pFtpCtl->iErrorCode  = -FILEM_ERROR_MD5CHKERR;
          
          filem_oper_remove(pFtpCtl->bLocTempN);
     }

     return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_dnloadfile                         */
/* Desc: 数据通道接收数据处理                                        */
/*---------------------------------------------------------*/

static void filem_ftp_recv_dnloadfile(FILEM_FTP_CTRL *pFtpCtl, char *pDatBuf, int iDatlen)
{
    if((pFtpCtl->iTrsType  == FILEM_FTP_DNLOAD) &&
       (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
    {
         int iTmpLen = 0;

         if(iDatlen > 0)
         {
            iTmpLen = filem_oper_write(pFtpCtl->iFileID, pDatBuf, iDatlen);
         }
         
         if(iTmpLen != iDatlen)  
         {
              pFtpCtl->iErrorCode  = -FILEM_ERROR_WRITE;

              pFtpCtl->iReqState   =  FILEM_FTP_CMD_DATEND;
              
              pFtpCtl->iReqTimeout =  FILEM_FTP_CMD_TIMEOUT;

              filem_ftp_close_socket(&pFtpCtl->sDatSock);  /*关闭通道终止传输*/
#if 0
              filem_ftp_session_end(pFtpCtl);     /*出错了不走流程 立即关闭*/
#endif
         }
         else
         {
             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
             
             pFtpCtl->iCurrPos += iDatlen;

             filem_ftp_out_percent(pFtpCtl, 0);

             if(pFtpCtl->iCurrPos >= pFtpCtl->iFileLen)
             {
                 filem_oper_close(pFtpCtl->iFileID);

                 pFtpCtl->iFileID = -1;
                 
                 filem_ftp_close_socket(&pFtpCtl->sDatSock);

                 filem_ftp_check_dnloadfile(pFtpCtl);   /*下载完成了, 要走流程*/

                 if((pFtpCtl->iEarlyCode == 226) || (pFtpCtl->iEarlyCode == 250)) 
                 {                                       /*提前收到传输成功信号*/
                     filem_ftp_send_quitcmd(pFtpCtl);
                     pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                     pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                 }
                 else
                 {
                     pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                     pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                 }
             }
         }
    }
}                          

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_fill_svrackerror*/
/*Para: pftpctl */
/*Dest: output run percent infor */
/*---------------------------------------------------------*/

static void filem_ftp_fill_svrackerror(FILEM_FTP_CTRL *pftpCtl, int iCode, 
                                       char *pErrStr, int iErrcode)
{
    pftpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
    pftpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
    
    if(pftpCtl->iErrorCode == 0) pftpCtl->iErrorCode = iErrcode;

    if(!pErrStr)
         snprintf(pftpCtl->bSvrErrBuf, (FILEM_FTP_ERRBUF_LEN -1), "%d\r\n", iCode);                  
    else snprintf(pftpCtl->bSvrErrBuf, (FILEM_FTP_ERRBUF_LEN -1), "%d %s\r\n", iCode, pErrStr);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_process_respon*/
/*Para: pftpctl */
/*Dest: process control channel reply */
/*---------------------------------------------------------*/

static void filem_ftp_process_respon(FILEM_FTP_CTRL *pFtpCtl, int icode, char *pText)
{
     switch(pFtpCtl->iReqState)
     {
         case FILEM_FTP_CMD_CONN:

              if((icode == 200) || (icode == 220))
              {
                   if(filem_ftp_send_username(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_USER;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user name command\r\n");
                   }
                   else
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                       
                       pFtpCtl->iErrorCode  = -FILEM_ERROR_CONNECTFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user name command fail\r\n");
                   }
              }
              else
              {
                  filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_CONNECTFAIL);

                  zlog_debug(FILEM_DEBUG_FTP, "FTP Connect reply fail code = %d\r\n", icode);
              }

              break;

         case FILEM_FTP_CMD_USER:

              if((icode == 331) || (icode == 332))
              {
                   if(filem_ftp_send_userpass(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_PASS;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user pass command\r\n");
                   }
                   else
                   {
              
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user pass command fail\r\n");
                   }
              }
              else
              {              
                  filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_AUTHERROR);
                  
                  zlog_debug(FILEM_DEBUG_FTP, "FTP User command reply fail code = %d\r\n", icode);
              }
              
              break;

         case FILEM_FTP_CMD_PASS:

              if((icode == 200) || (icode == 220) || (icode == 230))
              {
                   if(filem_ftp_send_typecmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_TYPE;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send file type command\r\n");
                   }
                   else
                   {
                  
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                       
                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send file type command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_AUTHERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Password command reply fail code = %d\r\n", icode);
              }

              break;

         case FILEM_FTP_CMD_TYPE:

              if((icode == 200) || (icode == 220))
              {
                   if(filem_ftp_send_portcmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_PORT;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send port command\r\n");
                   }
                   else
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send port command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Type command reply fail code = %d\r\n", icode);
              }
        
              break;

         case FILEM_FTP_CMD_PORT:

              if((icode == 200) || (icode == 220))
              {
                   if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
                   {
                        if(filem_ftp_send_sizecmd(pFtpCtl) == 0)
                        {
                            pFtpCtl->iReqState   = FILEM_FTP_CMD_SIZE;
                            pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send get file size command\r\n");
                        }
                        else
                        {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                            
                            pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                            
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send get file size command fail\r\n");
                        }
                   }
                   else
                   {
                        if(filem_ftp_send_storcmd(pFtpCtl) == 0)
                        {
                            pFtpCtl->iReqState   = FILEM_FTP_CMD_STOR;
                            pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send file STOR command\r\n");
                        }
                        else
                        {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                            pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                            
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send file STOR command fail\r\n");
                        }
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Port command reply fail code = %d\r\n", icode);
              }            

              break;
              
         case FILEM_FTP_CMD_SIZE:
            
              if((icode == 213) || (icode == 200))
              {
                   if(filem_ftp_check_dnloadlen(pFtpCtl, pText) == 0)
                   {
                       pFtpCtl->iFileID = filem_oper_open(pFtpCtl->bLocTempN, O_WRONLY | O_TRUNC | O_CREAT, 0777);

                       if(pFtpCtl->iFileID >= 0)
                       {
                            if(filem_ftp_send_retrcmd(pFtpCtl) == 0)
                            {
                                pFtpCtl->iReqState  = FILEM_FTP_CMD_RETR;
                            
                                pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                            
                                zlog_debug(FILEM_DEBUG_FTP, "FTP Send file RETR command\r\n");
                            }
                            else
                            {
                                pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                                
                                pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                                
                                pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
 
                                zlog_debug(FILEM_DEBUG_FTP, "FTP Send file RETR command fail\r\n");
                            }
                       }
                       else
                       {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                            
                            pFtpCtl->iErrorCode  = -FILEM_ERROR_OPEN;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Open file =%s fail\r\n", pFtpCtl->bLocTempN);
                       }
                   }
                   else
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                       
                       pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Get file , file len check error %d\r\n", pFtpCtl->iFileLen);
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);

                   zlog_debug(FILEM_DEBUG_FTP, "FTP Size command reply fail code = %d\r\n", icode);
              }
              break;

         case FILEM_FTP_CMD_RETR:

              if((icode == 125) || (icode == 150))
              {
                   pFtpCtl->iReqState   = FILEM_FTP_CMD_DATTRS;
                   pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File option command reply code %d\r\n", icode);
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File option command reply fail code = %d\r\n", icode);
              }            
              
              break;
              
         case FILEM_FTP_CMD_STOR:

              if((icode == 125) || (icode == 150))
              {
                   pFtpCtl->iFileID = filem_oper_open(pFtpCtl->bLocFname, O_RDONLY, 0);

                   if(pFtpCtl->iFileID < 0)
                   {            
                       filem_ftp_close_socket(&pFtpCtl->sDatSock);
                       
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
               
                       pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Upload open file %s fail\r\n", pFtpCtl->bLocFname);
                   }
                   else
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_DATTRS;

                       pFtpCtl->iReqTimeout = 0; //FILEM_FTP_RTY_TIMEOUT * 2;
                   
                       pFtpCtl->iFileLen = filem_oper_size_getf(pFtpCtl->iFileID);

                       zlog_debug(FILEM_DEBUG_FTP, "FTP Upload file state=%d, timeout=%d, datalen=%d\r\n", pFtpCtl->iReqState, pFtpCtl->iReqTimeout, pFtpCtl->iFileLen);

                       ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
               
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File stor command reply fail code = %d\r\n", icode);
              }            
              
              break;

         case FILEM_FTP_CMD_DATTRS:
              /*wait send data over*/

              if((icode == 226) || (icode == 250))
              {
                   pFtpCtl->iEarlyCode = icode;
              }
              zlog_debug(FILEM_DEBUG_FTP, "FTP receive data trans end, in trans data state code = %d\r\n", icode);
              break;

         case FILEM_FTP_CMD_DATEND:

              if((icode == 226) || (icode == 250))
              {
                   if(filem_ftp_send_quitcmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command \r\n");
                   }
                   else
                   {
                       if(pFtpCtl->iErrorCode == 0)
                       {
                           pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                       }
                       
                       filem_ftp_session_end(pFtpCtl);
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
               
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Data transfer fail code = %d\r\n", icode);
              }            
              break;     

         case FILEM_FTP_CMD_SESEND:
            
              /*do nothing,  wait timeout and quit session*/
              break;

         case FILEM_FTP_CMD_QUIT:

              if((icode == 221) || (icode == 220) || (icode == 250))
              {
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Quit reply ok\r\n");
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Quit reply fail code = %d\r\n", icode);
              }            

              filem_ftp_session_end(pFtpCtl);
              
              break;              
        
         default: 
         
              zlog_debug(FILEM_DEBUG_FTP, "FTP Ftp client no care command or reply state=%d, code = %d\r\n",  pFtpCtl->iReqState, icode);
              break;
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_get_cmdline*/
/*Para: pFtpCtl */
/*Dest: get one line command  */
/*---------------------------------------------------------*/
static int filem_ftp_get_cmdline(FILEM_FTP_CTRL *pFtpCtl)
{
    int    iLeftOf =  0;
    int    iLeftEn =  0;
    int    iLinCou =  0;   
    int    iLineMx = FILEM_FTP_CMDBUF_LEN - 8;
    int    iLeftLn = pFtpCtl->iRecCmdLen;
    char  *pDatPos = pFtpCtl->bRecCmdBuf;

    pFtpCtl->bOneCmdBuf[0] = '\0';
    
    while(iLeftLn > 0)
    {
        if((*pDatPos == '\r') || (*pDatPos == '\n') || (*pDatPos == ' '))
        {
             pDatPos++; iLeftLn--; iLeftOf++; iLeftEn++;
        }
        else break;
    }

    if(iLeftLn <= 0)
    {
        pFtpCtl->iRecCmdLen = 0; return(0);
    }
    else
    {
        while(iLeftLn > 0) 
        {
            if((*pDatPos == '\r') || (*pDatPos == '\n')) break;
             
            iLeftEn ++;  pDatPos ++; iLeftLn--;
        }

        iLinCou = iLeftEn - iLeftOf;

        if(iLinCou > 0) 
        {
            if(iLinCou > iLineMx) iLinCou = iLineMx;
            
            memcpy(pFtpCtl->bOneCmdBuf, pFtpCtl->bRecCmdBuf + iLeftOf, iLinCou);

            pFtpCtl->bOneCmdBuf[iLinCou] = '\0';
        }
        
        while(iLeftLn > 0)
        {
            if((*pDatPos == '\r') || (*pDatPos == '\n') || (*pDatPos == ' '))
            {
                 pDatPos++; iLeftLn--; iLeftEn++;
            }
            else break;
        }

        if(iLeftLn > 0) memmove(pFtpCtl->bRecCmdBuf, pFtpCtl->bRecCmdBuf+iLeftEn, iLeftLn);
        
        pFtpCtl->iRecCmdLen = iLeftLn;

        return(iLinCou);
    }
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_parse_replymsg*/
/*Para: pFtpCtl */
/*Para: pCmdTxt return reply code's explain text */
/*Dest: pase one line reply message  */
/*---------------------------------------------------------*/
static int filem_ftp_parse_replymsg(FILEM_FTP_CTRL *pFtpCtl, char **pCmdTxt)
{
     int   iCode = -1;
     int   iCoun =  0;
     char *pText = NULL;

     iCoun = filem_ftp_get_cmdline(pFtpCtl);

     zlog_debug(FILEM_DEBUG_FTP, "FTP Reveive reply line = %s\r\n", pFtpCtl->bOneCmdBuf);

     if(iCoun > 0)
     {
         pText = pFtpCtl->bOneCmdBuf;
         
         while(iCoun > 0)
         {
             if(*pText == ' ') 
             {
                *pText = '\0'; pText++;
                 break;
             }
             
             iCoun --; pText++;
         }

         *pCmdTxt = pText;
         
         if(filem_ftp_is_decnumber(pFtpCtl->bOneCmdBuf)) 
         {
             iCode = atoi(pFtpCtl->bOneCmdBuf);
         }
         
         zlog_debug(FILEM_DEBUG_FTP, "FTP Parse reveive reply code = %d, text = %s\r\n", iCode, pText);
     }

     return(iCode);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ctrldata                           */
/* Desc: 接收控制通道数据处理                                        */
/*---------------------------------------------------------*/

static void filem_ftp_recv_ctrlpacket(FILEM_FTP_CTRL *pFtpCtl, char *pDatBuf, int iDatlen)
{
    int   iTmpLen = 0;
    int   iRetCod = 0;
    char *pPadTxt = NULL;
    
    while(iDatlen > 0)
    {
         iTmpLen = FILEM_FTP_CMDBUF_LEN - pFtpCtl->iRecCmdLen;

         if(iTmpLen > iDatlen) iTmpLen = iDatlen;

         memcpy(pFtpCtl->bRecCmdBuf + pFtpCtl->iRecCmdLen, pDatBuf, iTmpLen);

         iDatlen -= iTmpLen; pDatBuf += iTmpLen;
         
         pFtpCtl->iRecCmdLen += iTmpLen;

         iRetCod = filem_ftp_parse_replymsg(pFtpCtl, &pPadTxt);

         if(iRetCod >= 0)
         {
              filem_ftp_process_respon(pFtpCtl, iRetCod, pPadTxt);
         }
         else
         {
              zlog_debug(FILEM_DEBUG_FTP, "FTP Control data receive, cmd line error or no right reply code\r\n");
         }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ctrldata                           */
/* Desc: 接收控制通道数据处理                                        */
/*---------------------------------------------------------*/

void filem_ftp_send_notify(void)
{
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;

    if((pFtpCtl->iTrsType == FILEM_FTP_UPLOAD) &&
       (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
    {
        int iRetry = 0;
        
        iRetry = filem_ftp_send_uploadfile(pFtpCtl);

        if((pFtpCtl->iCurrPos >= pFtpCtl->iFileLen) || (pFtpCtl->iErrorCode != 0))
        {
             zlog_debug(FILEM_DEBUG_FTP, "FTP Upload file end errcode=%d, filelen=%d\r\n", pFtpCtl->iErrorCode, pFtpCtl->iFileLen);
            
             filem_oper_close(pFtpCtl->iFileID);
                                 
             pFtpCtl->iFileID = -1;

             pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
             
             usleep(200000);
             
             filem_ftp_close_socket(&pFtpCtl->sDatSock);
        }
        else
        {
             if(iRetry) pFtpCtl->iReqTimeout = FILEM_FTP_RTY_TIMEOUT;
             else
             {
                 usleep(10000);

                 ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
             }
        }
    }

}


/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ipcpacket                          */
/* Desc: 接收SOCKET数据 事件轮询驱动                                 */
/*---------------------------------------------------------*/
void filem_ftp_recv_ipcpacket(struct ipc_mesg_n *pMsg, int revln)
{
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;/*find session*/
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        int                iDatTyp = 0;

        struct pkt_buffer *pPktBuf = (struct pkt_buffer *)pMsg->msg_data;
        
        pPktBuf->data = pMsg->msg_data + pPktBuf->data_offset;

        iDatTyp = filem_ftp_match_socket(pFtpCtl, &pPktBuf->cb.ipcb);

        if(pPktBuf->data_len == 0)
        {
            zlog_debug(FILEM_DEBUG_FTP, "FTP receive packet, data len = 0\r\n");
        }
        switch(iDatTyp)
        {
            case FILEM_FTP_RCV_CTRLDATA:
                 filem_ftp_recv_ctrlpacket(pFtpCtl, pPktBuf->data, (int)pPktBuf->data_len);
                 break;
            
            case FILEM_FTP_RCV_FILEDATA:
                 filem_ftp_recv_dnloadfile(pFtpCtl, pPktBuf->data, (int)pPktBuf->data_len);
                 break;
            
            default:
                 zlog_debug(FILEM_DEBUG_FTP, "FTP receive packet, not ctrl, not data, no match socket\r\n");
                 break;
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_connect_notimsg                         */
/* Para: bSesDat struct tcp_session                        */
/* Desc: 收到FTM的TCP连接成功通知消息处理                               */
/*---------------------------------------------------------*/
void filem_ftp_connect_notimsg(char *bSesDat, int iDatLen, int iOptCod)
{
     if(iDatLen >= (int)sizeof(struct tcp_session))
     {
          int iSockType = 0;
          
          FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl; // find ftp control is right
          
          iSockType = filem_ftp_accept_socket(pFtpCtl, (struct tcp_session*)bSesDat, iOptCod);

          zlog_debug(FILEM_DEBUG_FTP, "FTP ftp accept connect type=%d, opcode=%d\r\n", iSockType, iOptCod);

          /*control channel connect ok*/
          
          if((iSockType == FILEM_FTP_RCV_CTRLDATA) && (iOptCod == IPC_OPCODE_CONNECT))
          {
              pFtpCtl->iReqState   = FILEM_FTP_CMD_CONN;
              pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;

                                                                  /*控制通道连接成功后, 必须正确填写本地侦听地址*/
              pFtpCtl->sLocAddr.iIsReg = 0;
              pFtpCtl->sLocAddr.iIpver = pFtpCtl->sCtrSock.iIpver;
                                                                  /*端口号已经设置好*/
            //pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT;  
              pFtpCtl->sLocAddr.iIpv4  = pFtpCtl->sCtrSock.iSipv4;

              if(pFtpCtl->sLocAddr.iIpver == INET_FAMILY_IPV6)
              {
                  memcpy(pFtpCtl->sLocAddr.bIpv6, pFtpCtl->sCtrSock.bSipv6, 16);
              }            

              filem_ftp_register_loclis(&pFtpCtl->sLocAddr);           
#if 0
              thread_add_event(filemmaster, filem_ftp_recv_ipcpacket, NULL, 0);   /*会话关闭时会自动删除*/
#endif
          }
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_tftp_session_timeout                        */
/* Desc: 会话超时处理                                            */
/*---------------------------------------------------------*/
void filem_ftp_session_timeout(void)
{
     FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;      /*该操作不多余, 为支持多SESSION留接口*/

     if((pFtpCtl->iTrsType != FILEM_FTP_NOACT) && (pFtpCtl->iReqTimeout > 0))
     {
          if(--pFtpCtl->iReqTimeout == 0)
          {         
               zlog_debug(FILEM_DEBUG_FTP, "FTP Timeout state=%d\r\n", pFtpCtl->iReqState);

               switch(pFtpCtl->iReqState)
               {
                    case FILEM_FTP_CMD_CONN:
                         
                         if(pFtpCtl->iErrorCode == 0)
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_CONNECTFAIL;
                         }
                         filem_ftp_session_end(pFtpCtl);

                         break;

                    case FILEM_FTP_CMD_USER:
                    case FILEM_FTP_CMD_PASS:

                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_AUTHERROR;
                         }
                         filem_ftp_session_end(pFtpCtl);
                         
                         break;
                    
                    case FILEM_FTP_CMD_TYPE:
                    case FILEM_FTP_CMD_PORT:
                    case FILEM_FTP_CMD_SIZE:
                    case FILEM_FTP_CMD_RETR:
                    case FILEM_FTP_CMD_STOR:

                         pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                         pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;

                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         break;
                    
                    case FILEM_FTP_CMD_DATTRS:

                         if(pFtpCtl->iTrsType == FILEM_FTP_UPLOAD)
                         {
                              pFtpCtl->iRetryCoun ++;

                              if(pFtpCtl->iRetryCoun > FILEM_FTP_RTY_REPEAT)
                              {
                                  filem_ftp_close_socket(&pFtpCtl->sDatSock);
                                  
                                  pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                                  pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                                  
                                  pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                              }
                              else
                              {
                                  ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
                              }
                         }
                         else
                         {
                              filem_ftp_close_socket(&pFtpCtl->sDatSock);
                              
                              pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                              pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
 
                              if(pFtpCtl->iErrorCode == 0) 
                              {
                                  pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                              }
                         }
                         break;          

                    case FILEM_FTP_CMD_DATEND:
                             
                         if(pFtpCtl->iFileID >= 0)
                         {
                             filem_oper_close(pFtpCtl->iFileID);
                         
                             pFtpCtl->iFileID = -1;
                         }
                          
                         pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                         pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                                                  
                         if(pFtpCtl->iErrorCode == 0)
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         break;                                  

                    case FILEM_FTP_CMD_SESEND:    /*用于异常终止*/
                         
                         if(filem_ftp_send_quitcmd(pFtpCtl) == 0)
                         {
                             pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                           
                             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                           
                             zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command \r\n");
                         }
                         else
                         {
                             if(pFtpCtl->iErrorCode == 0) 
                             {
                                 pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                             }
                             filem_ftp_session_end(pFtpCtl);
                         }

                         break;
                    
                    case FILEM_FTP_CMD_QUIT:
                         
                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         
                         filem_ftp_session_end(pFtpCtl);
                         
                         break;
                                              
                    default: break;
               }
          }
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_downloadfile                             */
/* Para: pSvrAddr   SERVERIP                               */
/* Para: pUser      登录用户名                             */
/* Para: pPaswd     登录用户密码                           */
/* Para: pLocName   文件名(含路径)                         */
/* Para: pRemName  文件名(含路径)                          */
/* Para: iSvrPort  svr端口号                          */
/* Retu: 成功返回0, 失败返回错误代码                  */
/*---------------------------------------------------------*/
int filem_ftp_uploadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                         char  *pLocName, char *pRemName, 
                          int   iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int             iFileTyp = 0;
    int             iIpver   = INET_FAMILY_IPV4;
    int             iResult  = 0;
    
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp download is running, please retry after a moment.\r\n"); 
        }
        else 
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal) 
        {
            pProcCal(0, pFtpCtl->bErrInfBuf, pProcPar, 0); 
        }
        else
        {
            zlog_err("%s[%d]: %s", __FILE__, __LINE__, pFtpCtl->bErrInfBuf);
        }
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pRemName == NULL) pRemName = pLocName;
        
    if(strlen(pLocName) >= (FILEM_NAME_MAXLN - 8)) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }

    if(strchr(pSvrAddr,':') != NULL) iIpver = INET_FAMILY_IPV6;
            
    memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    pFtpCtl->iTrsType  =  FILEM_FTP_UPLOAD;
    pFtpCtl->iFileType =  iFileTyp;
    pFtpCtl->iFileID   = -1;
    pFtpCtl->pProcCall =  pProcCal;
    pFtpCtl->pProParam =  pProcPar;

    pFtpCtl->iL3Vpn    = (unsigned short)filem_tftp_l3vpn_get();

    pFtpCtl->sSvrAddr.iIsReg = 0;
    pFtpCtl->sSvrAddr.iIpver = iIpver;
    pFtpCtl->sSvrAddr.iPort  = (iSvrPort == 0) ? FILEM_FTP_SVR_LISPORT : iSvrPort;;
    pFtpCtl->sSvrAddr.iIpv4  = 0;
    
    pFtpCtl->sCtrSock.iSport = FILEM_FTP_CLI_CONPORT + iFtpCtrPortIdx % 100;
    pFtpCtl->sCtrSock.iIpver = iIpver;
    
    pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT + iFtpCtrPortIdx % 100;

    if(iIpver == INET_FAMILY_IPV4)
          pFtpCtl->sSvrAddr.iIpv4 = inet_strtoipv4(pSvrAddr);
    else  inet_strtoipv6(pSvrAddr, (struct ipv6_addr *)pFtpCtl->sSvrAddr.bIpv6);
    
    strncpy(pFtpCtl->sSvrAddr.bAddStr, pSvrAddr, FILEM_NAME_MAXLN -8);

    strncpy(pFtpCtl->bUserName, pUser,    FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bPassWord, pPaswd,   FILEM_NAME_MAXLN - 8);
     
    strncpy(pFtpCtl->bRemFname, pRemName, FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bLocFname, pLocName, FILEM_NAME_MAXLN - 8);
   
  //filem_ftp_clear_session(pFtpCtl);

    filem_ftp_register_loccnt(&pFtpCtl->sCtrSock);
    
    iResult = filem_ftp_start_session(pFtpCtl);

    if(iResult != 0)
    {
        iResult = -FILEM_ERROR_CONNECTFAIL;

        filem_ftp_unregister_loccnt(&pFtpCtl->sCtrSock);
        
        memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
    
    zlog_debug(FILEM_DEBUG_FTP, "FTP Connect to server %s and wait. locname= %s remname = %s.\r\n", pSvrAddr, pLocName, pRemName);

  //after start session , wait ftm connect notify
    
    iFtpCtrPortIdx ++;
  
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_downloadfile                             */
/* Para: pSvrAddr   SERVERIP                               */
/* Para: pUser      登录用户名                             */
/* Para: pPaswd     登录用户密码                           */
/* Para: pLocName   文件名(含路径)                         */
/* Para: pRemName  文件名(含路径)                          */
/* Para: iSvrPort  svr端口号                          */
/* Retu: 成功返回0, 失败返回错误代码                  */
/*---------------------------------------------------------*/
int filem_ftp_downloadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                           char  *pLocName, char *pRemName, 
                           int   iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int             iFileTyp = 0;
    int             iIpver   = INET_FAMILY_IPV4;
    int             iResult  = 0;
    
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp download is running, please retry after a moment.\r\n"); 
        }
        else 
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal) 
        {
            pProcCal(0, pFtpCtl->bErrInfBuf, pProcPar, 0); 
        }
        else
        {
            zlog_err("%s[%d]: %s", __FILE__, __LINE__, pFtpCtl->bErrInfBuf);
        }
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pLocName == NULL) pLocName = pRemName;
        
    if(strlen(pLocName) >= (FILEM_NAME_MAXLN - 8)) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }

    if(strchr(pSvrAddr,':') != NULL) iIpver = INET_FAMILY_IPV6;
            
    memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    pFtpCtl->iTrsType  =  FILEM_FTP_DNLOAD;
    pFtpCtl->iFileType =  iFileTyp;
    pFtpCtl->iFileID   = -1;
    pFtpCtl->pProcCall =  pProcCal;
    pFtpCtl->pProParam =  pProcPar;

    pFtpCtl->iL3Vpn    = (unsigned short)filem_tftp_l3vpn_get();

    pFtpCtl->sSvrAddr.iIsReg = 0;
    pFtpCtl->sSvrAddr.iIpver = iIpver;
    pFtpCtl->sSvrAddr.iPort  = (iSvrPort == 0) ? FILEM_FTP_SVR_LISPORT : iSvrPort;;
    pFtpCtl->sSvrAddr.iIpv4  = 0;
    
    pFtpCtl->sCtrSock.iSport = FILEM_FTP_CLI_CONPORT + iFtpCtrPortIdx % 100;
    pFtpCtl->sCtrSock.iIpver = iIpver;

    pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT + iFtpCtrPortIdx % 100;
      
    if(iIpver == INET_FAMILY_IPV4)
          pFtpCtl->sSvrAddr.iIpv4 = inet_strtoipv4(pSvrAddr);
    else  inet_strtoipv6(pSvrAddr, (struct ipv6_addr *)pFtpCtl->sSvrAddr.bIpv6);
    
    strncpy(pFtpCtl->sSvrAddr.bAddStr, pSvrAddr, FILEM_NAME_MAXLN -8);

    strncpy(pFtpCtl->bUserName, pUser,    FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bPassWord, pPaswd,   FILEM_NAME_MAXLN - 8);
     
    strncpy(pFtpCtl->bRemFname, pRemName, FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bLocFname, pLocName, FILEM_NAME_MAXLN - 8);

    sprintf(pFtpCtl->bLocTempN, "%s.tmp", pFtpCtl->bLocFname);
   
  //filem_ftp_clear_session(pFtpCtl);
    
    filem_ftp_register_loccnt(&pFtpCtl->sCtrSock);
    
    iResult = filem_ftp_start_session(pFtpCtl);

    if(iResult != 0)
    {
        iResult = -FILEM_ERROR_CONNECTFAIL;
        
        filem_ftp_unregister_loccnt(&pFtpCtl->sCtrSock);

        memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
    
    zlog_debug(FILEM_DEBUG_FTP, "FTP Connect to server %s and wait. locname= %s remname = %s.\r\n", pSvrAddr, pLocName, pRemName);

  //after start session , wait ftm connect notify
    
    iFtpCtrPortIdx ++;
    
    return(iResult);
}

int filem_ftp_isruning(void)
{
    return(sFtpSessCtl.iTrsType != FILEM_FTP_NOACT) ;
}
/*====================================================================================================================*/
/*=====================================================================================================================*/
