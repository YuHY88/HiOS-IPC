/*------------------------------------------------------------------------*/
/* 文 件 名 : tftpcli.c                                                   */
/* 作    者 : suxiqing                                                    */
/* 创作日期 : 2007-08-05                                                  */
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
#include "filem.h"
#include "filem_tftp.h"
#include "filem_md5.h"
#include "filem_oper.h"

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

#define FILEM_TFTP_TRYTIMES         3       /* 尝试次数 */
#define FILEM_TFTP_TIMEOUT          5       /* 超时时间 */
#define FILEM_TFTP_DATASIZE         4096    /* 数据长度，字节 */
#define FILEM_TFTP_DEFASIZE         512     /* 数据长度，字节 */

/* TFTP 操作码 */
#define FILEM_TFTP_CODE_RRQ         1       /* 读请求 */
#define FILEM_TFTP_CODE_WRQ         2       /* 写请求 */
#define FILEM_TFTP_CODE_DATAS       3       /* 数据包 */
#define FILEM_TFTP_CODE_ACKS        4       /* 应答包 */
#define FILEM_TFTP_CODE_ERRORS      5       /* 错误 */
#define FILEM_TFTP_CODE_OACK        6       /* 选项应答包 */

/* 下载类型 */ 
#define FILEM_TFTP_NOACT            0       /* 无操作 */
#define FILEM_TFTP_DNLOAD           1       /* 上传文件 */
#define FILEM_TFTP_UPLOAD           2       /* 下载文件 */


/* TFTP传输报文结构 */
typedef struct _FILEM_TFTP_HDR_
{
    unsigned short iOpCode;
    union
    {
        char sStuff[FILEM_TFTP_DATASIZE];
        struct
        {
            unsigned short  iBlock;
            char            bDatas[FILEM_TFTP_DATASIZE];
        }sData;
        struct
        {
            unsigned short  iCode;
            char            bMsgs[FILEM_TFTP_DATASIZE];
        }sError;
    }sBody;
}FILEM_TFTP_HDR;

/* TFTP传输实例控制块 */
typedef struct _FILEM_TFTP_CTRL_
{
    int              iTrsType;                     /*1 下载 2 上传 */
    int              iFileType;
    
    int              iFileID;                      /*文件句柄*/            
    int              iFileLen;                     /*下载或上载文件长度  */  

    int              iBlksize;                     /*每个数据包的大小    */
    
    unsigned short   iCurrBlock;                   /*当前块ID            */  
    unsigned short   iServerPort;                  /*服务器端口          */
    
    unsigned short   iL3Vpn;                       /*0-127         */

    int              iCurrPos;                     /*当前发送或接收位置  */

    int              iSendLen;                     /*待发送数据长度      */
    int              iRecvLen;                     /*待发送数据长度      */

    int              iLstPercent;                  /*最后一个报文标志    */      
    
    int              iIsLastPkt;                   /*最后一个报文标志    */       

    int              iTryCount;                    /*尝试次数*/
    int              iTimerOut;                    /*超时定时器*/
    int              iOptResult;                   /*操作结果  */
    int              iPrompDly;                  
    
    void            *pProParam;
    FilemCall        pProcCall;
    struct inet_addr ServerIP;

    
    char             bOptInfo[256];                /*操作结果信息         */
    char             bLocName[FILEM_NAME_MAXLN];   /*本地文件名          */
    char             bRemName[FILEM_NAME_MAXLN];   /*远端文件名          */
    
    FILEM_TFTP_HDR  *bSendPackt;                   /*发包 因重发要缓存               */
    FILEM_TFTP_HDR  *bRecvPackt;                   /*收包   直接用接收内存            */
}FILEM_TFTP_CTRL;

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
//extern struct thread_master *filemmaster;

//static struct thread *filem_tftp_thread_event = NULL; 

/* FTFP 模式 */
const char *gTftpModString[]  = {"netascii", "octet", "mail"};

/* FTFP 选项名称 */
const char *gTftpOptString[]  = {"blksize", "tsize", "timeout"};

FILEM_TFTP_CTRL sTftpSessCtl  = {FILEM_TFTP_NOACT, FILEM_MTYPE_NON, 0};

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_tftp_pkt_register                           */
/* Desc: 注册要接收的协议报文类型                                      */
/*---------------------------------------------------------*/
static void filem_tftp_pkt_register(struct inet_addr *iIpAdd)
{
    union proto_reg proto;
    
    memset(&proto, 0, sizeof(union proto_reg));
    
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport    = UDP_PORT_TFTP;
 // proto.ipreg.dip      = iIpAdd->addr.ipv4;
    
    if(iIpAdd->type == INET_FAMILY_IPV4)
    {
         pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else if(iIpAdd->type == INET_FAMILY_IPV6)
    {
        ;
    }
    return;
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_pkt_unregister                         */
/* Desc: 注消要接收的协议报文类型                                      */
/*---------------------------------------------------------*/
static void filem_tftp_pkt_unregister(struct inet_addr *iIpAdd)
{
    union proto_reg proto;
    
    memset(&proto, 0, sizeof(union proto_reg));

    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport    = UDP_PORT_TFTP;
  //proto.ipreg.dip      = iIpAdd->addr.ipv4;
    
    if(iIpAdd->type == INET_FAMILY_IPV4)
    {   
         pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else if(iIpAdd->type == INET_FAMILY_IPV6)
    {
        ;
    }
    return;
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_ipcque_clear                           */
/* Desc: 清空 IPC 残余的垃圾信息                                    */
/*---------------------------------------------------------*/ 
/*
static void filem_tftp_ipcque_clear(void)
{
    struct pkt_buffer *pkt = NULL;
    
    do
    {
       pkt = pkt_rcv(MODULE_ID_FILE);
    }while(pkt != NULL);
}
*/
/*---------------------------------------------------------*/
/* Name: filem_tftp_percent_out                            */
/* Desc: 进度提示                                              */
/*---------------------------------------------------------*/ 

static void filem_tftp_percent_out(FILEM_TFTP_CTRL *pTftpCtl, int iForce)
{
    int  iPercent  =  0;
    char bProInf[128] = {0};

    pTftpCtl->iPrompDly = 0;
    
    if((pTftpCtl->iFileLen > 0) && pTftpCtl->pProcCall)
    {
        iPercent = (int)((float)pTftpCtl->iCurrPos / pTftpCtl->iFileLen * 100);

        if((pTftpCtl->iLstPercent != iPercent)  || iForce)
        {
            sprintf(bProInf, "\033[1A""\033[K""Percent:    %3d%% \r\n", iPercent);                  
    
            pTftpCtl->pProcCall(0, bProInf, pTftpCtl->pProParam, 0);   
        }
    }
}
/*---------------------------------------------------------*/
/* Name: filem_tftp_packet_send                            */
/* Desc: 报文数据发送                                            */
/*---------------------------------------------------------*/ 
static int filem_tftp_packet_send(FILEM_TFTP_CTRL *pTftpCtl)
{
    union pkt_control pktcontrol;
    
    memset(&pktcontrol, 0, sizeof(union pkt_control));
    
  //pktcontrol.ipcb.dip = ntohl(iServerIP);
    pktcontrol.ipcb.dip      = pTftpCtl->ServerIP.addr.ipv4;
    pktcontrol.ipcb.dport    = pTftpCtl->iServerPort;
    pktcontrol.ipcb.sport    = UDP_PORT_TFTP;
    pktcontrol.ipcb.protocol = IP_P_UDP;
    pktcontrol.ipcb.vpn      = pTftpCtl->iL3Vpn;
    pktcontrol.ipcb.ttl      = IP_TTL_DEFAULT;
    pktcontrol.ipcb.is_changed = 1;
    pktcontrol.ipcb.tos = 3;
    pktcontrol.ipcb.chsum_enable= 1;

    return(pkt_send(PKT_TYPE_UDP, &pktcontrol, (char*)pTftpCtl->bSendPackt, pTftpCtl->iSendLen));
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_session_end                            */
/* Desc: 回话结束                                              */
/*---------------------------------------------------------*/
static void filem_tftp_session_end(FILEM_TFTP_CTRL *pTftpCtl, int iOptResult)
{
    filem_tftp_pkt_unregister(&pTftpCtl->ServerIP);
    /*
    if(filem_tftp_thread_event != NULL)
    {
        thread_cancel(filem_tftp_thread_event);
        
        filem_tftp_thread_event = NULL;
    }
    */

    if(pTftpCtl->iFileID >= 0) filem_oper_close(pTftpCtl->iFileID);

    pTftpCtl->iOptResult = iOptResult;

    if(pTftpCtl->iOptResult == 0)
    { 
         if(pTftpCtl->iTrsType == FILEM_TFTP_UPLOAD)  
         {
              filem_upload_stat_set(-FILEM_ERROR_FINISHED);

              snprintf(pTftpCtl->bOptInfo, 120, "Tftp upload file %s Successfully\r\n", pTftpCtl->bLocName);
         }
         else 
         {
              filem_download_stat_set(-FILEM_ERROR_FINISHED);
              
              snprintf(pTftpCtl->bOptInfo, 120, "Tftp download file %s Successfully\r\n", pTftpCtl->bRemName);
         }
    }
    else
    { 
         if(pTftpCtl->iTrsType == FILEM_TFTP_UPLOAD)  
         {
              filem_upload_stat_set(pTftpCtl->iOptResult);
              
              snprintf(pTftpCtl->bOptInfo, 120, "Tftp upload file %s fail, %s\r\n", pTftpCtl->bLocName,  filem_sys_errget(pTftpCtl->iOptResult)); 
         }
         else 
         {
              filem_download_stat_set(pTftpCtl->iOptResult);

              filem_oper_remove(pTftpCtl->bLocName);
              
              snprintf(pTftpCtl->bOptInfo, 120, "Tftp download file %s fail,%s\r\n", pTftpCtl->bRemName, filem_sys_errget(pTftpCtl->iOptResult));
         }
             
         zlog_err("%s[%d]: %s", __FILE__, __LINE__, pTftpCtl->bOptInfo);
     }

    
     if(pTftpCtl->pProcCall)
     {
          filem_tftp_percent_out(pTftpCtl, 1);
          
          pTftpCtl->pProcCall(pTftpCtl->iOptResult, pTftpCtl->bOptInfo, pTftpCtl->pProParam, 1);
     }
     
     if(pTftpCtl->iFileType == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
 
     if(pTftpCtl->bSendPackt) XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bSendPackt);
     
   //if(pTftpCtl->bRecvPackt) XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bRecvPackt);

     memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

     filem_oper_flush();
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_session_timeout                        */
/* Desc: 回话超时计数                                            */
/*---------------------------------------------------------*/
void filem_tftp_session_timeout(void)
{
     FILEM_TFTP_CTRL *pTftpCtl = &sTftpSessCtl;      /*该操作不多余, 为支持多SESSION留接口*/

     if((pTftpCtl->iTrsType != FILEM_TFTP_NOACT) && (pTftpCtl->iTimerOut > 0))
     {
          pTftpCtl->iTimerOut--;

          if(pTftpCtl->iTimerOut == 0)
          {
              if(pTftpCtl->iTryCount > 0)
              {
                   pTftpCtl->iTryCount --;
                   pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;
                   
                   filem_tftp_packet_send(pTftpCtl);  //重发
              }
              else filem_tftp_session_end(pTftpCtl, -FILEM_ERROR_RECVTIMEOUT);
          }
     }
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_tftp_option_parse                           */
/* Para: pTftpCtl TFTP传输实例控制块                             */
/* Desc: 解析可选参数                                            */
/*---------------------------------------------------------*/
static void filem_tftp_option_parse(FILEM_TFTP_CTRL *pTftpCtl)
{
    int   iBlkSz = FILEM_TFTP_DEFASIZE;
    int   iIndex = 0;
    int   iFileL = 0;
    int   iFcoun = 0;
    int   iDatLn = 0;
    char *pDatBuf= NULL;
    char *pField[8] = {NULL};
    
    memset(pField, 0, sizeof(char*) * 8);

    iDatLn  = pTftpCtl->iRecvLen - 2;
    
    pDatBuf = pTftpCtl->bRecvPackt->sBody.sStuff;
        
    for(iIndex= 0; iIndex < iDatLn; iIndex++)
    {
        if(pDatBuf[iIndex] == '\0') pDatBuf[iIndex] = ' ';
    }

    pDatBuf[iDatLn] = 0;
    
    iFcoun = filem_oper_nameparse(pDatBuf, pField, 6, ' ');

    zlog_debug(FILEM_DEBUG_TFTP,"filem_oper_nameparse fieldcou =%d, datalen=%d\r\n", iFcoun, iDatLn);

    if(iFcoun >= 2)
    {
        for(iIndex= 0; iIndex < iFcoun; iIndex++)
        {
            zlog_debug(FILEM_DEBUG_TFTP,"filem_tftp_option_parse 1 pField[%d]=%s\r\n", iIndex, pField[iIndex]);
            
            if(strcmp(pField[iIndex], gTftpOptString[0]) == 0)
            {
                iIndex++;

                if(pField[iIndex] != NULL) iBlkSz = strtol(pField[iIndex], NULL, 10);

                zlog_debug(FILEM_DEBUG_TFTP,"filem_tftp_option_parse 2 pField[%d]=%s, %d\r\n", iIndex, pField[iIndex], iBlkSz);
            }
            else
            {
                if(strcmp(pField[iIndex], gTftpOptString[1]) == 0)
                {
                    iIndex++;

                    if(pField[iIndex] != NULL) iFileL = strtol(pField[iIndex], NULL, 10);
                    
                    zlog_debug(FILEM_DEBUG_TFTP,"filem_tftp_option_parse 3 pField[%d]=%s, %d\r\n", iIndex, pField[iIndex], iFileL);
                }
            }
        }
    }

    pTftpCtl->iBlksize = iBlkSz; pTftpCtl->iFileLen = iFileL;
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_receive_error                          */
/* Desc: 回话超时计数                                            */
/*---------------------------------------------------------*/
static int filem_tftp_receive_error(FILEM_TFTP_CTRL *pTftpCtl)
{
    unsigned short iCode = 0;

    iCode = ntohs(pTftpCtl->bRecvPackt->sBody.sError.iCode);
    
  //if(ntohs(pTftpCtl->bRecvPackt->sBody.sError.iCode) <= 7)
    {
    pTftpCtl->bRecvPackt->sBody.sStuff[pTftpCtl->iRecvLen -2] = 0;
    
    zlog_debug(FILEM_DEBUG_TFTP,"Tftp receiv error: error code = %d, error msg: %s \n", iCode, pTftpCtl->bRecvPackt->sBody.sError.bMsgs);
    }

    filem_tftp_session_end(pTftpCtl, -FILEM_ERROR_PKTACKERROR);
 
    return(-FILEM_ERROR_PKTACKERROR);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_upload_packet_set                      */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/*       pPktBuf  报文的数据指针                                  */
/* Desc: 向上载数据报文中填充数据                                      */
/*---------------------------------------------------------*/
static int filem_tftp_upload_dataset(FILEM_TFTP_CTRL *pTftpCtl)
{
    int iSndLen =0;

    if(pTftpCtl->iFileID >= 0) 
    {
      //iSndLen = pTftpCtl->iFileLen - pTftpCtl->iCurrPos;
        
      //if(iSndLen > pTftpCtl->iBlksize) 
      
      //iSndLen = pTftpCtl->iBlksize;
                         
        iSndLen = filem_oper_read(pTftpCtl->iFileID, pTftpCtl->bSendPackt->sBody.sData.bDatas, pTftpCtl->iBlksize);
        
        zlog_debug(FILEM_DEBUG_TFTP,"Upload file read len=%d, blksize=%u!\r\n", iSndLen, pTftpCtl->iBlksize);
        
        if(iSndLen < 0) return(-FILEM_ERROR_READ);
        else
        {
            pTftpCtl->iCurrPos += iSndLen;
            pTftpCtl->iSendLen  = iSndLen + 4;
            
          //if(pTftpCtl->iCurrPos >= pTftpCtl->iFileLen) pTftpCtl->iIsLastPkt = 1;           
            if(iSndLen < pTftpCtl->iBlksize) pTftpCtl->iIsLastPkt = 1;           
            return(0);
        }
    }
    else 
    {
        zlog_debug(FILEM_DEBUG_TFTP,"Upload file read fail, fd=%d len=%d!\r\n", pTftpCtl->iFileID, iSndLen);
        return(-FILEM_ERROR_READ);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_upload_request                         */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* retu: 失败返回错误代码                                          */
/* Desc: 上载传送请求执行函数                                        */
/*---------------------------------------------------------*/
static int filem_tftp_upload_request(FILEM_TFTP_CTRL *pTftpCtl)
{
    int iReturn = 0;
    int iSndLen = 0;

    pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_WRQ); 

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", pTftpCtl->bRemName);
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", gTftpModString[1]);
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", gTftpOptString[0]);
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%d", FILEM_TFTP_DATASIZE);
    iSndLen += 1;
     
    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s",  gTftpOptString[1]);/*tsize */
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%u",  pTftpCtl->iFileLen); 
    iSndLen += 3; /*1+2 why*/

    pTftpCtl->iSendLen = iSndLen;

  //filem_tftp_ipcque_clear();
                                                                         /* 发送请求报文，若失败则尝试三次 */
    iReturn = filem_tftp_packet_send(pTftpCtl);

    pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;                          //定时器启动
    pTftpCtl->iTryCount = FILEM_TFTP_TRYTIMES;
        
    zlog_debug(FILEM_DEBUG_TFTP,"Send Upload request len = %d, retlen = %d!\r\n", iSndLen, iReturn);

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_upload_data                            */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* retu: 失败返回错误代码                                          */
/* Desc: 上传发送下一片数据                                         */
/*---------------------------------------------------------*/
static int filem_tftp_upload_data(FILEM_TFTP_CTRL *pTftpCtl)
{
    int  iReturn = 0;
   
    zlog_debug(FILEM_DEBUG_TFTP,"Tftp upload data block = %d\n", pTftpCtl->iCurrBlock);
      
    pTftpCtl->iCurrBlock++;
    
    pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_DATAS);
    
    pTftpCtl->bSendPackt->sBody.sData.iBlock = htons(pTftpCtl->iCurrBlock);
    
    iReturn = filem_tftp_upload_dataset(pTftpCtl);

    if(iReturn == 0)
    {
        filem_tftp_packet_send(pTftpCtl);

        pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;                          //启动重发
        
        pTftpCtl->iTryCount = FILEM_TFTP_TRYTIMES;
    
        if(++pTftpCtl->iPrompDly > 50) filem_tftp_percent_out(pTftpCtl, 0);
    }
    else
    {
        zlog_debug(FILEM_DEBUG_TFTP,"Tftp Upload read file data error %d, block=%d, pos=%d\r\n", iReturn, pTftpCtl->iCurrBlock, pTftpCtl->iCurrPos);
    }
            
    return(iReturn);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_upload_oack                            */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* retu: 失败返回错误代码                                          */
/* Desc: 处理上传请求的选项应答                                       */
/*---------------------------------------------------------*/
static int filem_tftp_upload_oack(FILEM_TFTP_CTRL *pTftpCtl)
{
    int  iReturn = 0;

    if(pTftpCtl->iRecvLen > 2)
    {
        filem_tftp_option_parse(pTftpCtl);
    }

    zlog_debug(FILEM_DEBUG_TFTP,"Tftp upload recv oack: blksize = %d, tsize = %d\n",
                 pTftpCtl->iBlksize, pTftpCtl->iFileLen);
  
    pTftpCtl->iFileLen = filem_oper_size_getf(pTftpCtl->iFileID);
    
    iReturn = filem_tftp_upload_data(pTftpCtl);
       
    return(iReturn);
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_upload_process                         */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* retu: 0, 失败返回错误代码                                       */
/* Desc: 上传收到报文的处理                                         */
/*---------------------------------------------------------*/
static int filem_tftp_upload_process(FILEM_TFTP_CTRL *pTftpCtl)
{
    int              iResult = 0;
    unsigned short   iPktCod = 0;
    unsigned short   iBlkSer = 0;
    FILEM_TFTP_HDR  *pRcvPkt = pTftpCtl->bRecvPackt;
   
    iPktCod = ntohs(pRcvPkt->iOpCode);
    
    switch(iPktCod)
    {
        case FILEM_TFTP_CODE_DATAS:
             break;
             
        case FILEM_TFTP_CODE_ACKS:     
 
             iBlkSer = ntohs(pRcvPkt->sBody.sData.iBlock);
             
             if((pTftpCtl->iRecvLen >= 4) && (pTftpCtl->iCurrBlock == iBlkSer))
             {
                  if(pTftpCtl->iIsLastPkt) 
                  {
                       iResult = -FILEM_ERROR_FINISHED;
                       
                       filem_tftp_session_end(pTftpCtl, 0);
                  }
                  else 
                  {
                       iResult = filem_tftp_upload_data(pTftpCtl);  
  
                       if(iResult != 0) filem_tftp_session_end(pTftpCtl, iResult);
                  }
             }
           //else do nothing wait timeout & resend data & drop packet
             break;
 
        case FILEM_TFTP_CODE_OACK:     
            
             if((pTftpCtl->iCurrBlock == 0) && (pTftpCtl->iCurrPos == 0))
             {
                 iResult = filem_tftp_upload_oack(pTftpCtl);   //必须数据未开始传
 
                 if(iResult != 0) filem_tftp_session_end(pTftpCtl, iResult);
             }
             break;
 
        case FILEM_TFTP_CODE_ERRORS:                           /* 错误 */
            
             iResult = filem_tftp_receive_error(pTftpCtl);
             break;
        
        default: break;                                        /*其他报文直接丢弃*/
    }
    
    return(iResult);
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_tftp_dnload_datwrite                        */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* Desc: 向下载数据缓存中复制数据                                      */
/*---------------------------------------------------------*/
static int filem_tftp_dnload_datwrite(FILEM_TFTP_CTRL *pTftpCtl)
{
    int iDatLen = 0;
    
    iDatLen = pTftpCtl->iRecvLen - 4;
    
    if(iDatLen < pTftpCtl->iBlksize) pTftpCtl->iIsLastPkt = 1;
    
    if(pTftpCtl->iFileID >= 0)
    {
        if(iDatLen > 0)
        {
             if(filem_oper_write(pTftpCtl->iFileID, pTftpCtl->bRecvPackt->sBody.sData.bDatas, iDatLen) == iDatLen)
             {
                  pTftpCtl->iCurrPos += iDatLen;
                  return(0);
             }
             else return(-FILEM_ERROR_WRITE);
        }
        else return(0);
    }
    else return(-FILEM_ERROR_OPEN);    
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_dnload_request                         */
/* Desc: 下载传送请求执行函数                                        */
/*---------------------------------------------------------*/
/**/
static int filem_tftp_dnload_request(FILEM_TFTP_CTRL *pTftpCtl)
{
    int   iReturn = 0;
    int   iSndLen = 0;
    
    pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_RRQ);  
    
    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", pTftpCtl->bRemName);
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", gTftpModString[1]);
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s", gTftpOptString[0]);/* blksize */
    iSndLen += 1;
    
    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%d", FILEM_TFTP_DATASIZE);
    iSndLen += 1;
    
    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%s",  gTftpOptString[1]);/*tsize */
    iSndLen += 1;

    iSndLen += sprintf(pTftpCtl->bSendPackt->sBody.sStuff + iSndLen, "%u",  0); 
    iSndLen += 3; /*1+2 why*/

    pTftpCtl->iSendLen = iSndLen;

  //filem_tftp_ipcque_clear();

    iReturn = filem_tftp_packet_send(pTftpCtl);
        
    zlog_debug(FILEM_DEBUG_TFTP,"Send Dnload request len = %d, retlen = %d!\r\n", iSndLen, iReturn);
        
    pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;                          //定时器启动
    
    pTftpCtl->iTryCount = FILEM_TFTP_TRYTIMES;

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_dnload_oack                            */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* Desc: 下载传送请求执行函数                                        */
/*---------------------------------------------------------*/
static int filem_tftp_dnload_oack(FILEM_TFTP_CTRL *pTftpCtl)
{
    char  cPath[FILEM_NAME_MAXLN] = {0};

    if(pTftpCtl->iRecvLen > 2)
    {
        filem_tftp_option_parse(pTftpCtl);
    }
    
    zlog_debug(FILEM_DEBUG_TFTP,"Tftp dnload recv oack: blksize = %d, tsize = %d\n",
                 pTftpCtl->iBlksize, pTftpCtl->iFileLen);

    filem_oper_cutpath(pTftpCtl->bLocName, cPath);
    
    if(filem_oper_availdisk(cPath) < (unsigned int)pTftpCtl->iFileLen)
    {
        return -FILEM_ERROR_DISKFULL;
    }
    else
    {
        pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_ACKS);
        pTftpCtl->bSendPackt->sBody.sData.bDatas[0] = 0;
        pTftpCtl->bSendPackt->sBody.sData.iBlock = htons(pTftpCtl->iCurrBlock);

        pTftpCtl->iSendLen = 4;

        filem_tftp_packet_send(pTftpCtl);

        pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;                          //启动重发
        
        pTftpCtl->iTryCount = FILEM_TFTP_TRYTIMES;

        zlog_debug(FILEM_DEBUG_TFTP,"Tftp download send ack(for oack), CurrBlock = %d\r\n", pTftpCtl->iCurrBlock);

        return(0);
    }
}


/*---------------------------------------------------------*/
/* Name: flem_tftp_dnload_data                             */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* Desc: 下载数据执行函数                                          */
/*---------------------------------------------------------*/
static int filem_tftp_dnload_data(FILEM_TFTP_CTRL *pTftpCtl)
{
    int  iReturn = 0;

    pTftpCtl->iCurrBlock += 1;

    iReturn = filem_tftp_dnload_datwrite(pTftpCtl);

    if(iReturn == 0)
    {
        pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_ACKS);
        pTftpCtl->bSendPackt->sBody.sData.bDatas[0] = 0;
        pTftpCtl->bSendPackt->sBody.sData.iBlock = htons(pTftpCtl->iCurrBlock);

        pTftpCtl->iSendLen = 4;

        if(++pTftpCtl->iPrompDly > 50) filem_tftp_percent_out(pTftpCtl, 0);
    }
    else
    {
        int iStrlen = 0;
        pTftpCtl->bSendPackt->iOpCode = htons(FILEM_TFTP_CODE_ERRORS);
        pTftpCtl->bSendPackt->sBody.sError.iCode = 1;
        
        iStrlen = sprintf(pTftpCtl->bSendPackt->sBody.sError.bMsgs, "%s", filem_sys_errget(iReturn));

        pTftpCtl->iSendLen = iStrlen + 4;
    }

    filem_tftp_packet_send(pTftpCtl);

    pTftpCtl->iTimerOut = FILEM_TFTP_TIMEOUT;                          //启动重发
    
    pTftpCtl->iTryCount = FILEM_TFTP_TRYTIMES;

    zlog_debug(FILEM_DEBUG_TFTP,"Tftp download recv data, CurrBlock = %d\r\n", pTftpCtl->iCurrBlock);

    return(iReturn);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_dnload_check                           */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* Desc: 下载完成后文件检查                                         */
/*---------------------------------------------------------*/

static int filem_tftp_dnload_check(FILEM_TFTP_CTRL *pTftpCtl)
{
     int iResult = 0;

     if(pTftpCtl->iFileID >= 0) 
     {
         filem_oper_close(pTftpCtl->iFileID);
         pTftpCtl->iFileID = -1;
     }
     
     iResult = filem_md5_matchcheck(pTftpCtl->bLocName, 0);
                        
     if(iResult == 0) 
     {
          int  iNameLn = 0;
          char cFileName[FILEM_NAME_MAXLN] = {0};
          
          iNameLn = strlen(pTftpCtl->bLocName) - 4;
          
          if(strcmp(pTftpCtl->bLocName + iNameLn, ".tmp") == 0)
          {
              strncpy(cFileName, pTftpCtl->bLocName,  iNameLn);
              
              filem_oper_remove(cFileName);
         
              filem_oper_rename(pTftpCtl->bLocName, cFileName);
          }
     }          
     else filem_oper_remove(pTftpCtl->bLocName);

     return(iResult);
}


/*---------------------------------------------------------*/
/* Name: filem_tftp_dnload_process                         */
/* Para: pTftpCtl TFTP传输实例控制块                              */
/* retu: 0, 失败返回错误代码                                       */
/* Desc: 下载收到报文的处理                                         */
/*---------------------------------------------------------*/
static int filem_tftp_dnload_process(FILEM_TFTP_CTRL *pTftpCtl)
{
    int              iResult = 0;
    unsigned short   iPktCod = 0;
    unsigned short   iBlkSer = 0;
    FILEM_TFTP_HDR  *pRcvPkt = pTftpCtl->bRecvPackt;
   
    iPktCod = ntohs(pRcvPkt->iOpCode);
    
    switch(iPktCod)
    {
        case FILEM_TFTP_CODE_DATAS:
            
             iBlkSer = ntohs(pRcvPkt->sBody.sData.iBlock);
             
             if((pTftpCtl->iRecvLen >= 4) && ((pTftpCtl->iCurrBlock + 1) == iBlkSer))
             {
                  iResult = filem_tftp_dnload_data(pTftpCtl);

                  if(iResult != 0)
                  {
                       filem_tftp_session_end(pTftpCtl, iResult);
                  }
                  else 
                  {
                      if(pTftpCtl->iIsLastPkt) 
                      {
                           iResult = filem_tftp_dnload_check(pTftpCtl);
                      
                           if(iResult == 0)
                           {
                                iResult = -FILEM_ERROR_FINISHED;
                                filem_tftp_session_end(pTftpCtl, 0);
                           }
                           else filem_tftp_session_end(pTftpCtl, iResult);
                      }
                  }
             }
             break;
       
        case FILEM_TFTP_CODE_ACKS:     
             break;

        case FILEM_TFTP_CODE_OACK:     
            
             if((pTftpCtl->iCurrBlock == 0) && (pTftpCtl->iCurrPos == 0))
             {
                 iResult = filem_tftp_dnload_oack(pTftpCtl);   //必须数据未开始传
 
                 if(iResult != 0) filem_tftp_session_end(pTftpCtl, iResult);
             }
             break;
 
        case FILEM_TFTP_CODE_ERRORS:                           /* 错误 */
            
             iResult = filem_tftp_receive_error(pTftpCtl);
             break;
        
        default: break;                                        /*其他报文直接丢弃*/
    }
    
    return(iResult);
}

#if 0
/*---------------------------------------------------------*/
/* Name: filem_tftp_ipcpkt_recv                            */
/* Desc: TFTPIPC报文接收                                       */
/*---------------------------------------------------------*/
static int filem_tftp_ipcpkt_recv(struct thread *pthread)
{
    FILEM_TFTP_CTRL *pTftpCtl = &sTftpSessCtl;

  //filem_tftp_thread_event = NULL;
    
    if(pTftpCtl->iTrsType != FILEM_TFTP_NOACT) 
    {
        int                iResult = 0;
        struct pkt_buffer *pPktBuf = NULL;
        
        pPktBuf = pkt_rcv(MODULE_ID_FILE);

        if(pPktBuf) 
        {
              pTftpCtl->iRecvLen = (int)pPktBuf->data_len;
            
              memcpy(pTftpCtl->bRecvPackt, pPktBuf->data, pPktBuf->data_len);

              pTftpCtl->iServerPort = pPktBuf->cb.ipcb.sport;

              zlog_debug(FILEM_DEBUG_TFTP,"Tftp ipc receive data len=%d\r\n %s\r\n", pTftpCtl->iRecvLen, pkt_dump(pTftpCtl->bRecvPackt, 32));

              if(pTftpCtl->iRecvLen > 2)
              {
                  if(pTftpCtl->iTrsType == FILEM_TFTP_DNLOAD)
                       iResult = filem_tftp_dnload_process(pTftpCtl);
                  else iResult = filem_tftp_upload_process(pTftpCtl);
              }
        }

        if(iResult == 0)
        {
            usleep(1000);//延时 1ms ，释放CPU

           //filem_tftp_thread_event = 
            thread_add_event(filemmaster, filem_tftp_ipcpkt_recv, NULL, 0);
        }
    }

    return(0);
}
#endif
/*---------------------------------------------------------*/
/* Name: filem_tftp_ipcpkt_recv                            */
/* Desc: TFTPIPC报文接收                                       */
/*---------------------------------------------------------*/
int filem_tftp_ipcpkt_receive(struct ipc_mesg_n *pMsg, int revln)
{
    FILEM_TFTP_CTRL *pTftpCtl = &sTftpSessCtl;

    
    if(pTftpCtl->iTrsType != FILEM_TFTP_NOACT) 
    {
        int                iResult = 0;
        
        struct pkt_buffer *pPktBuf = (struct pkt_buffer *)pMsg->msg_data;
        
        pTftpCtl->iRecvLen = (int)pPktBuf->data_len;
            
      //memcpy(pTftpCtl->bRecvPackt, pMsg->msg_data + pPktBuf->data_offset, pPktBuf->data_len);

        pTftpCtl->bRecvPackt = (FILEM_TFTP_HDR*)(pMsg->msg_data + pPktBuf->data_offset);
            
        pTftpCtl->iServerPort = pPktBuf->cb.ipcb.sport;

       //zlog_debug(FILEM_DEBUG_TFTP,"Tftp ipc receive data len=%d\r\n %s\r\n", pTftpCtl->iRecvLen, pkt_dump(pTftpCtl->bRecvPackt, 32));

        if(pTftpCtl->iRecvLen > 2)
        {
            if(pTftpCtl->iTrsType == FILEM_TFTP_DNLOAD)
                 iResult = filem_tftp_dnload_process(pTftpCtl);
            else iResult = filem_tftp_upload_process(pTftpCtl);
        }

        pTftpCtl->bRecvPackt = NULL;
    }

    return(0);
}


/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_tftp_uploadfile                             */
/* Para: pIpAddr    SERVERIP                               */
/* Para: pLocName   本地文件名(包含路径)                   */
/* Para: pRemName   远端文件名(包含路径)                   */
/* Para: pTmChan    调用该函数的CLI通道                    */
/*                  如果非CLI调用,则必须传递NULL           */
/* para: pSndData   如果上传数据在数据缓存中该值不能为NULL */
/* para: iDataLen   指示pDatBuff长度                       */
/*                  传递数据缓存时pLocName可以为null       */
/* Retu: 成功返回FILEM_OK, 失败返回错误代码                */
/* Desc: 上载一个文件                                      */
/*---------------------------------------------------------*/
int filem_tftp_uploadfile(char *pIpAddr,   char *pLocName, char *pRemName, 
                          int iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int              iReturn  = 0;
    int              iFileTyp = 0;
    char             bProInfo[128] = {0};
    FILEM_TFTP_CTRL *pTftpCtl = &sTftpSessCtl;

    if(pTftpCtl->iTrsType != FILEM_TFTP_NOACT) 
    {
        if(pTftpCtl->iTrsType == FILEM_TFTP_DNLOAD)
        {
            snprintf(bProInfo, 120, "Tftp download is running, please retry after a moment.\r\n"); 
        }
        else //if(pTftpCtl->iTrsType == FILEM_TFTP_UPLOAD)
        {
            snprintf(bProInfo, 120, "Tftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal)   
             pProcCal(0, bProInfo, pProcPar, 0); 
        else printf("%s\r\n", bProInfo);
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pRemName == NULL) pRemName = pLocName;
    
    if((pLocName == NULL) || (strlen(pLocName) >= 120)) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
   
    memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));
    
    pTftpCtl->iTrsType      =  FILEM_TFTP_UPLOAD;
    pTftpCtl->iFileID       = -1;
    pTftpCtl->iBlksize      =  FILEM_TFTP_DEFASIZE; 
    pTftpCtl->iFileType     =  iFileTyp;
    pTftpCtl->iPrompDly     =  50;
    pTftpCtl->iLstPercent   =  0;
    pTftpCtl->iServerPort   =  iSvrPort;
    pTftpCtl->pProcCall     =  pProcCal;
    pTftpCtl->pProParam     =  pProcPar;
    pTftpCtl->ServerIP.type = (strchr(pIpAddr,':') == NULL) ? INET_FAMILY_IPV4: INET_FAMILY_IPV6;
    pTftpCtl->ServerIP.addr.ipv4 = inet_strtoipv4(pIpAddr);
    
    pTftpCtl->iL3Vpn        = (unsigned short)filem_tftp_l3vpn_get();
    
    strncpy(pTftpCtl->bLocName, pLocName, 120);
    strncpy(pTftpCtl->bRemName, pRemName, 120);

    pTftpCtl->bSendPackt = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(FILEM_TFTP_HDR));
  //pTftpCtl->bRecvPackt = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(FILEM_TFTP_HDR));

    if(pTftpCtl->bSendPackt == NULL) //|| (pTftpCtl->bRecvPackt == NULL))
    {
        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
        
        return(-FILEM_ERROR_NOMEM);
    }

    memset(pTftpCtl->bSendPackt, 0, sizeof(FILEM_TFTP_HDR));
 // memset(pTftpCtl->bRecvPackt, 0, sizeof(FILEM_TFTP_HDR));

    pTftpCtl->iFileID = filem_oper_open(pTftpCtl->bLocName, O_RDONLY, 0666);
    
    if(pTftpCtl->iFileID < 0)
    {
        XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bSendPackt);

      //XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bRecvPackt);     

        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

        return(-FILEM_ERROR_OPEN);
    }
    
    pTftpCtl->iFileLen = filem_oper_size_getf(pTftpCtl->iFileID);
                                                        /* 注册收包的模块和协议类型 */
    filem_tftp_pkt_register(&pTftpCtl->ServerIP);
    
    iReturn = filem_tftp_upload_request(pTftpCtl);
    
    if(iReturn == 0)
    {
        filem_upload_stat_set(-FILEM_ERROR_RUNNING);
     
      //zlog_debug("%s[%d]: filem tftp upload send request success", __FILE__, __LINE__);
 
      //filem_tftp_thread_event = 
      //thread_add_event(filemmaster, filem_tftp_ipcpkt_recv, NULL, 0);
 
        return(0);
    }
    else
    {
        filem_tftp_pkt_unregister(&pTftpCtl->ServerIP);
        
        XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bSendPackt);
      //XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bRecvPackt);   

        if(pTftpCtl->iFileID >= 0) filem_oper_close(pTftpCtl->iFileID);
        
        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

        return(-FILEM_ERROR_DLOADFAIL);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_dnloadfile                             */
/* Para: pIpAddr    SERVERIP                               */
/* Para: pLocName   本地文件名(包含路径)                   */
/* Para: pRemName   远端文件名(包含路径)                   */
/* Para: pTmChan    调用该函数的CLI通道                    */
/*                  如果非CLI调用,则必须传递NULL           */
/* Para: pRetData   如果不为null, 下载数据将放在数据缓存中 */
/*                  并通过该指针返回, 调用者负责释放       */
/*                  下载到缓存时本地文件名可以为NULL       */
/* Para: iRetLen    返回数据长度, 必须和pRetData 配套使用  */
/* Retu: 成功返回FILEM_OK, 失败返回错误代码                */
/* Desc: 下载一个文件                                      */
/*---------------------------------------------------------*/
int filem_tftp_dnloadfile(char *pIpAddr,  char  *pLocName, char *pRemName,
                          int   iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int              iReturn  = 0;
    int              iFileTyp = 0;
    char             bProInfo[128] = {0};
    FILEM_TFTP_CTRL *pTftpCtl = &sTftpSessCtl;
    
    if(pTftpCtl->iTrsType != FILEM_TFTP_NOACT) 
    {
        if(pTftpCtl->iTrsType == FILEM_TFTP_DNLOAD)
        {
            snprintf(bProInfo, 120, "Tftp download is running, please retry after a moment.\r\n"); 
        }
        else //if(pTftpCtl->iTrsType == FILEM_TFTP_UPLOAD)
        {
            snprintf(bProInfo, 120, "Tftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal)   
             pProcCal(0, bProInfo, pProcPar, 0); 
        else printf("%s\r\n", bProInfo);
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pLocName == NULL) pLocName = pRemName;
        
    if(strlen(pLocName) >= 120) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
    
    memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));
    
    pTftpCtl->iTrsType      =  FILEM_TFTP_DNLOAD;
    pTftpCtl->iFileType     =  iFileTyp;
    pTftpCtl->iFileID       = -1;
    pTftpCtl->iBlksize      =  FILEM_TFTP_DEFASIZE;  
    pTftpCtl->iPrompDly     =  50;
    pTftpCtl->iLstPercent   =  0;
    pTftpCtl->ServerIP.type =  (strchr(pIpAddr,':') == NULL) ? INET_FAMILY_IPV4: INET_FAMILY_IPV6;
    pTftpCtl->iServerPort   =  iSvrPort;
    pTftpCtl->pProcCall     =  pProcCal;
    pTftpCtl->pProParam     =  pProcPar;
    pTftpCtl->ServerIP.addr.ipv4 = inet_strtoipv4(pIpAddr);
    
    pTftpCtl->iL3Vpn        = (unsigned short)filem_tftp_l3vpn_get();
    
    strncpy(pTftpCtl->bLocName, pLocName, FILEM_NAME_MAXLN-8);

    strcat(pTftpCtl->bLocName, ".tmp");
    
    strncpy(pTftpCtl->bRemName, pRemName, FILEM_NAME_MAXLN-8);

    pTftpCtl->bSendPackt = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(FILEM_TFTP_HDR));
  //pTftpCtl->bRecvPackt = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(FILEM_TFTP_HDR));
    
    if(pTftpCtl->bSendPackt == NULL) //|| (pTftpCtl->bRecvPackt == NULL))
    {
        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
        
        return(-FILEM_ERROR_NOMEM);
    }
    
    memset(pTftpCtl->bSendPackt, 0, sizeof(FILEM_TFTP_HDR));
  //memset(pTftpCtl->bRecvPackt, 0, sizeof(FILEM_TFTP_HDR));
    
    pTftpCtl->iFileID = filem_oper_open(pTftpCtl->bLocName, O_WRONLY | O_TRUNC | O_CREAT, 0777);
    
    if(pTftpCtl->iFileID < 0)
    {
        XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bSendPackt);

      //XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bRecvPackt);

        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

        return(-FILEM_ERROR_OPEN);
    }
    
    filem_tftp_pkt_register(&pTftpCtl->ServerIP);
    
    iReturn = filem_tftp_dnload_request(pTftpCtl);   /* 发送请求 */
    
    if(iReturn == 0)
    {
        filem_download_stat_set(-FILEM_ERROR_RUNNING);
    
      //zlog_debug("%s[%d]: filem tftp download send request success", __FILE__, __LINE__);

      //filem_tftp_thread_event = 
      //thread_add_event(filemmaster, filem_tftp_ipcpkt_recv, NULL, 0);
    
        return(0);
    }
    else
    {
        filem_tftp_pkt_unregister(&pTftpCtl->ServerIP);
        
        XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bSendPackt);
      //XFREE(MTYPE_FILEM_ENTRY, pTftpCtl->bRecvPackt);

        if(pTftpCtl->iFileID >= 0) filem_oper_close(pTftpCtl->iFileID);

        memset(pTftpCtl, 0, sizeof(FILEM_TFTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

        return(iReturn);
    }
}

/*--------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------------*/

