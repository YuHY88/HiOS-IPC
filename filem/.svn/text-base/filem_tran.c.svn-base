#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>

#include "lib/types.h"
#include "lib/memtypes.h"
#include "lib/memory.h"
#include <lib/thread.h>
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/log.h"
#include "lib/sys_ha.h"
#include "filem.h"
#include "filem_oper.h"
#include "filem_tran.h"
#include "filem_img.h"
#include "filem_slot.h"

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

#define  FILE_TRSBLK_SIZE    4096

#define  FILE_RETRY_COUNT    3

#define  FILE_RETRY_TIMEOUT  4

#define  FILE_PROMPT_COUNT   32
/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

typedef struct _ha_file_sess_
{
    int              iBusy;

    unsigned char    iSrcUnit;       /* 发送者unit 号 */
    
    unsigned char    iSrcSlot;       /* 发送者slot 号 */
    
    unsigned char    iDesUnit;       /* 接收者unit 号 */
    
    unsigned char    iDesSlot;       /* 接收者slot 槽位号 */

	

    unsigned char    iOpCode;

    unsigned char    iAsker;

    unsigned short   iSessSno;
    
    int              iResult;
    
    int              iProtout;

    int              iFileID;

    int              iFileLen;

    int              iCurrLen;       /*待发送数据长度      */
    
    int              iCurrPos;       /*当前发送或接收位置  */

    int              iBlksize;       /*每个数据包的大小    */

    int              iPercent;

    int              iTimeOut;

    int              iRetryCo;

    int              iMcState;       /*状态机*/

    int              iRetryMs;       /*可以重发的消息*/

    void            *pCbPara;

    FilemCall        pCbFun;
    
    struct ipc_mesg_n *bSndBuf;        /*发包             */
#if 0    
    struct ipc_mesg_n *bRcvBuf;        /*收包             */
#endif
    
    char             bLocName[FILEM_NAME_MAXLN]; 
                                     /*本地文件名          */
    char             bRemName[FILEM_NAME_MAXLN];
                                     /*远端文件名          */

    char             bResInfo[512];                            
}ha_file_sess;

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/
//extern struct thread_master *filemmaster;

unsigned short        gSesSerialno   = 0;

ha_file_sess          gFtSessionCtrl = {0};

struct list           gFtSendFileQue = {0,0,0,0,0};

                              //目前设计,收发只能使用一个SESSION 
                             
/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

static void  filem_tran_sessclr(ha_file_sess *pSess)
{
    if(pSess->iFileID >= 0) filem_oper_close(pSess->iFileID);
    
    pSess->iBusy     =  0;
    pSess->iDesUnit  =  0;
    pSess->iDesSlot  =  0;
    pSess->iFileLen  =  0;
    pSess->iPercent  =  0;
    pSess->iFileID   = -1;
    pSess->iOpCode   =  0;
    pSess->iCurrLen  =  0;
    pSess->iCurrPos  =  0;
    pSess->iSessSno  =  0;
    pSess->iRetryCo  =  0;
    pSess->iRetryMs  =  0;
    pSess->iResult   =  0;
    pSess->iTimeOut  =  0;
    pSess->iProtout  =  0;
    pSess->pCbFun    =  NULL;
    pSess->pCbPara   =  NULL;

    memset(pSess->bResInfo, 0, 512);
    memset(pSess->bLocName, 0, FILEM_NAME_MAXLN);
    memset(pSess->bRemName, 0, FILEM_NAME_MAXLN);
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/
static void filem_tran_cfgrecvnoti(ha_file_sess *pSession)
{
    if(strcmp(pSession->bLocName, HA_CONFIG_BATCHSYNC_FILE) == 0)
    {            
        if(pSession->iResult == 0)
        {
            printf("Config data batch trans ok!\r\n");

            zlog_err("Config data batch trans ok!\r\n");

            if(pSession->iOpCode == IPC_OPCODE_ADD)
            {
                ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_VTY, MODULE_ID_FILE, IPC_TYPE_HA, HA_SMSG_BATCH_END, 0, 0);
            }
        }
        else
        {
            printf("Config data batch trans fail! %s \r\n", filem_sys_errget(pSession->iResult));

            zlog_err("Config data batch trans fail! %s \r\n", filem_sys_errget(pSession->iResult));
        }
    }

    printf("File trans option=%d, name=%s, result=%d\r\n", (int)pSession->iOpCode, pSession->bLocName, pSession->iResult);

    zlog_notice("File trans option=%d, name=%s, result=%d\r\n", (int)pSession->iOpCode, pSession->bLocName, pSession->iResult);
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/
static void filem_tran_sndheadinit(ha_file_sess *pSession, struct ipc_msghdr_n *pMsgHdr)
{
    pMsgHdr->module_id   = MODULE_ID_FILE;
    pMsgHdr->sender_id   = MODULE_ID_FILE;
    pMsgHdr->msg_type    = IPC_TYPE_HA;
    pMsgHdr->opcode      = pSession->iOpCode;
    pMsgHdr->data_num    = 0;
    pMsgHdr->priority    = 0;
    pMsgHdr->unit        = pSession->iDesUnit;
    pMsgHdr->slot        = pSession->iDesSlot;
    pMsgHdr->srcunit     = pSession->iSrcUnit;
    pMsgHdr->srcslot     = pSession->iSrcSlot;

    pMsgHdr->sequence    = pSession->iSessSno;
    pMsgHdr->msgflag     = 0;
    pMsgHdr->result      = 0;
}


static int filem_tran_sndretrypkt(ha_file_sess *pSession)
{        
    if(pSession->iRetryCo > 0)
    {
         pSession->iTimeOut = FILE_RETRY_TIMEOUT;

         filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n) + pSession->iCurrLen);

         zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_retry  %d !\r\n", pSession->iCurrLen);

         return(0);
    }
    else return(-FILEM_ERROR_TRANSTOUT);
}

static void filem_tran_asksessend(ha_file_sess *pSession)
{
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_ask_sess_end result %d !\r\n", pSession->iResult);

    pSession->iBusy = 0;
    
    if(pSession->iFileID >= 0)
    {
        filem_oper_close(pSession->iFileID);

        pSession->iFileID = -1;

        if((pSession->iResult != 0) && 
           (pSession->iAsker) &&
           (pSession->iOpCode == IPC_OPCODE_GET)) 
        {
            filem_oper_remove(pSession->bLocName);
        }

        filem_oper_flush();
    }

    if(pSession->pCbFun) 
    {
        char bAction[160] = {0};

        if(pSession->iOpCode == IPC_OPCODE_GET) 
             sprintf(bAction, "Get file %s from slot%d", pSession->bLocName, pSession->iDesSlot);
        else sprintf(bAction, "Put file %s to slot%d", pSession->bLocName, pSession->iDesSlot);

        if(pSession->iResult == 0)
        {
             sprintf(pSession->bResInfo, "\033[1A""\033[K""Percent: 100%% \r\n");
             
             pSession->pCbFun(pSession->iResult, pSession->bResInfo, pSession->pCbPara, 0);

             sprintf(pSession->bResInfo, "%s successfully!\r\n", bAction);
        }
        else
        {
             sprintf(pSession->bResInfo, "%s fail, %s!\r\n", bAction, filem_sys_errget(pSession->iResult));
        }

        pSession->pCbFun(pSession->iResult, pSession->bResInfo, pSession->pCbPara, 1);
    }
}


static void filem_tran_aswsessend(ha_file_sess *pSession)
{
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_asw_sess_end result %d !\r\n", pSession->iResult);

    if(pSession->iFileID >= 0)
    {
        filem_oper_close(pSession->iFileID);

        pSession->iFileID = -1;

        if((pSession->iResult != 0) && 
           (!pSession->iAsker) && (pSession->iOpCode == IPC_OPCODE_ADD)) 
        {
            filem_oper_remove(pSession->bLocName);
        }

        filem_oper_flush();
    }

    filem_tran_sessclr(pSession);
}

static void filem_tran_promptshow(ha_file_sess *pSession)
{
    int iPercnt = 0;

    pSession->iProtout --;
    
    if((pSession->iProtout <= 0) && (pSession->iFileLen > 0))
    {
        pSession->iProtout = FILE_PROMPT_COUNT;
        
        iPercnt = filem_oper_percent(pSession->iCurrPos, pSession->iFileLen, 0);

        if(pSession->iPercent != iPercnt)
        {
            pSession->iPercent = iPercnt;
            
            sprintf(pSession->bResInfo, "\033[1A""\033[K""Percent:%4d%%\r\n", iPercnt);
    
            pSession->pCbFun(0, pSession->bResInfo, pSession->pCbPara, 0);
        }
    }          
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

static int filem_tran_sndreqpkt(ha_file_sess *pSession)
{
    struct ipc_msghdr_n *pMsgHdr = NULL;
    
    pMsgHdr = &pSession->bSndBuf->msghdr;

    filem_tran_sndheadinit(pSession, pMsgHdr);
    
    pMsgHdr->msg_subtype = HA_SMSG_FILE_REQ;

    pMsgHdr->msg_index   = pSession->iFileLen;
    
    pMsgHdr->data_len    = strlen(pSession->bRemName);

    strncpy((char*)pSession->bSndBuf->msg_data, pSession->bRemName, FILEM_NAME_MAXLN-1);

    pSession->iCurrLen   = (int)pMsgHdr->data_len;
    
    pSession->iMcState   = HA_SMSG_FILE_REQACK;

    pSession->iRetryCo   = FILE_RETRY_COUNT;
    
    pSession->iTimeOut   = FILE_RETRY_TIMEOUT;

    filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n) + pSession->iCurrLen);

    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_req file name %s len=%d!\r\n", pSession->bSndBuf->msg_data, pSession->iCurrLen);

    return(0);
}

/*------------------------------------------------------------------------------------------------------*/

/*该函数内部不能启动超时*/
static int filem_tran_sndreqackpkt(struct ipc_mesg_n *pRcvPkt,  
                                   int iDlen,int iResult, int iFlen)
{
    unsigned char iSrcUnit = 0;
    unsigned char iSrcSlot = 0;
    unsigned char iDesUnit = 0;
    unsigned char iDesSlot = 0;

    struct ipc_msghdr_n *pMsgHdr = NULL;

    pMsgHdr  = &pRcvPkt->msghdr;

    iSrcUnit = pMsgHdr->srcunit;
    iSrcSlot = pMsgHdr->srcslot;
    iDesUnit = pMsgHdr->unit;
    iDesSlot = pMsgHdr->slot;

    pMsgHdr->srcunit = iDesUnit;
    pMsgHdr->srcslot = iDesSlot;
    pMsgHdr->unit    = iSrcUnit;
    pMsgHdr->slot    = iSrcSlot;

    pMsgHdr->result  = iResult;

    if(iFlen >= 0) pMsgHdr->msg_index = iFlen;

    pMsgHdr->msg_subtype = HA_SMSG_FILE_REQACK;
    
    filem_slot_sendcpmsg(pRcvPkt, iDlen);

    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_reqack iResult %d !\r\n", iResult);

    return(0);
}

static int filem_tran_snddatpkt(ha_file_sess *pSession)
{
    char   *pDatBuf = NULL;
    struct ipc_msghdr_n *pMsgHdr =  NULL;
    
    pMsgHdr = &pSession->bSndBuf->msghdr;

    pDatBuf = (char*)pSession->bSndBuf->msg_data;

    filem_oper_seek(pSession->iFileID, pSession->iCurrPos, SEEK_SET);
    
    pSession->iCurrLen = filem_oper_read(pSession->iFileID, pDatBuf, pSession->iBlksize);

    if(pSession->iCurrLen < 0) return(-FILEM_ERROR_READ);

    filem_tran_sndheadinit(pSession, pMsgHdr);

    pMsgHdr->msg_subtype = HA_SMSG_FILE_DAT;
    pMsgHdr->msg_index   = pSession->iCurrPos;
    pMsgHdr->data_len    = pSession->iCurrLen;
        
    pSession->iMcState   = HA_SMSG_FILE_DATACK;
    pSession->iRetryCo   = FILE_RETRY_COUNT; /*数据发送侧控制重发*/
    pSession->iTimeOut   = FILE_RETRY_TIMEOUT;
    
    filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n) + pSession->iCurrLen);
    
  //zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_data data pos %d, len  %d !\r\n", pSession->iCurrPos, pSession->iCurrLen);

    return(0);
}

static int filem_tran_snddatackpkt(ha_file_sess *pSession)
{
    struct ipc_msghdr_n *pMsgHdr =  NULL;
    
    pMsgHdr = &pSession->bSndBuf->msghdr;
    
    filem_tran_sndheadinit(pSession, pMsgHdr);

    pMsgHdr->msg_subtype = HA_SMSG_FILE_DATACK;
    pMsgHdr->msg_index   = pSession->iCurrPos;
    pMsgHdr->data_len    = 0;
    pMsgHdr->result      = pSession->iResult;
        
    pSession->iCurrLen   = 0;
    pSession->iRetryCo   = 1; 
    pSession->iTimeOut   = FILE_RETRY_TIMEOUT * FILE_RETRY_COUNT;
    
    filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n));

  //zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_dataack result %d !\r\n", pSession->iResult);

    return(0);
}


static void filem_tran_sndendpkt(ha_file_sess *pSession)
{
    struct ipc_msghdr_n *pMsgHdr =  NULL;
    
    pMsgHdr = &pSession->bSndBuf->msghdr;
    
    filem_tran_sndheadinit(pSession, pMsgHdr);

    pMsgHdr->msg_subtype = HA_SMSG_FILE_END;
    pMsgHdr->msg_index   = pSession->iCurrPos;
    pMsgHdr->data_len    = 0;
    pMsgHdr->result      = pSession->iResult;
        
    pSession->iCurrLen   = 0;

    pSession->iMcState   = HA_SMSG_FILE_ENDACK;
    pSession->iRetryCo   = 2;//FILE_RETRY_COUNT;
    pSession->iTimeOut   = FILE_RETRY_TIMEOUT;
        
    filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n));
    
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_end result %d !\r\n", pSession->iResult);
}


static void filem_tran_sndendackpkt(ha_file_sess *pSession)
{
    struct ipc_msghdr_n *pMsgHdr =  NULL;
    
    pMsgHdr = &pSession->bSndBuf->msghdr;
    
    filem_tran_sndheadinit(pSession, pMsgHdr);

    pMsgHdr->msg_subtype = HA_SMSG_FILE_ENDACK;
    pMsgHdr->msg_index   = pSession->iCurrPos;
    pMsgHdr->data_len    = 0;
    pMsgHdr->result      = pSession->iResult;
        
    pSession->iCurrLen   = 0;
    pSession->iRetryCo   = 0; 
    pSession->iTimeOut   = 0; 
    pSession->iMcState   = 0;

    filem_slot_sendcpmsg(pSession->bSndBuf, sizeof(struct ipc_msghdr_n));
    
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_snd_endack result %d !\r\n", pSession->iResult);
}


/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

static void filem_tran_rcvreqpkt(ha_file_sess *pSession,
                                 struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvreq name %s  opcode %d!\r\n", pSession->bLocName, pSession->iOpCode);

    if(pSession->iOpCode == IPC_OPCODE_ADD)
    {
        pSession->iFileID = filem_oper_open(pSession->bLocName, O_WRONLY|O_CREAT|O_TRUNC, 0666);

        if(pSession->iFileID < 0) 
        {
            pSession->iResult = -FILEM_ERROR_OPEN;
        }
        
        pSession->iRetryMs = HA_SMSG_FILE_REQ;
        pSession->iRetryCo = 1;
        
        if(pSession->iResult == 0)
        {
             pSession->iMcState = HA_SMSG_FILE_DAT;
             pSession->iTimeOut = FILE_RETRY_TIMEOUT * FILE_RETRY_COUNT;
        }
        else
        {
             pSession->iMcState = HA_SMSG_FILE_END;
             pSession->iTimeOut = FILE_RETRY_TIMEOUT;
        }

        filem_tran_sndreqackpkt(pRcvPkt, iDlen, pSession->iResult, -1);
    }
    else //(pSession->iOpCode == IPC_OPCODE_GET)
    {
        pSession->iFileID = filem_oper_open(pSession->bLocName, O_RDONLY, 0666); 
        
        if(pSession->iFileID >= 0)
        {
             pSession->iFileLen = filem_oper_size_getf(pSession->iFileID);  
     
             if(pSession->iFileLen <= 0) 
             {
                 pSession->iResult = -FILEM_ERROR_LENGTH;
             }
        }
        else pSession->iResult = -FILEM_ERROR_NOEXIST;
        

        if(pSession->iResult == 0)
        {
            pSession->iRetryMs = HA_SMSG_FILE_REQ;
            
            pSession->iMcState = HA_SMSG_FILE_DATACK;
            
            filem_tran_sndreqackpkt(pRcvPkt, iDlen, pSession->iResult, pSession->iFileLen);
                                     //发送完ACK 马上发送DATA
            pSession->iCurrPos = 0;
                                     
            filem_tran_snddatpkt(pSession); 
        }
        else
        {
            pSession->iFileLen = 0;
            
            pSession->iRetryMs = HA_SMSG_FILE_REQ;

            pSession->iMcState = HA_SMSG_FILE_END;

            pSession->iRetryCo = 1;
            pSession->iTimeOut = FILE_RETRY_TIMEOUT;
            
            filem_tran_sndreqackpkt(pRcvPkt, iDlen, pSession->iResult, pSession->iFileLen);
        }
    }
}


static void filem_tran_rcvreqackpkt(ha_file_sess *pSession,
                                    struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;

    if(pSession->iResult == 0)                  /*本地操作结果优先*/
    {
        pSession->iResult = pMsgHdr->result;
    }                                      

    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvreqack result %d  opcode %d!\r\n", pSession->iResult, pSession->iOpCode);

    if(pSession->iOpCode == IPC_OPCODE_ADD)
    {
        if(pSession->iResult == 0)
        {
            pSession->iResult = filem_tran_snddatpkt(pSession);
        }
    }
    else
    {
        if(pSession->iResult == 0)
        {
            if(pSession->iFileID < 0)
            {
                 pSession->iFileID = filem_oper_open(pSession->bLocName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
            }
            
            if(pSession->iFileID < 0)
            {
			     pSession->iResult = -FILEM_ERROR_OPEN;
            }
            else 
            {
                 pSession->iMcState = HA_SMSG_FILE_DAT;     //wait data packet
 
                 pSession->iFileLen = pMsgHdr->msg_index;
                 
                 pSession->iRetryCo = 1;
                 pSession->iTimeOut = FILE_RETRY_TIMEOUT * FILE_RETRY_COUNT;
            }
        }
    }

    if(pSession->iResult != 0) filem_tran_sndendpkt(pSession);

    if((pSession->iResult == 0) && (pSession->pCbFun)) 
    {
        pSession->iProtout = FILE_PROMPT_COUNT;
        
        if(pSession->iFileLen > 0)
             sprintf(pSession->bResInfo, "Percent:   0%%  \r\n");
        else sprintf(pSession->bResInfo, "Percent: 100%%  \r\n");

        pSession->pCbFun(0, pSession->bResInfo, pSession->pCbPara, 0);
    }
}
/*------------------------------------------------------------------------------------------------------*/

static int filem_tran_rcvdatpkt(ha_file_sess *pSession,
                                struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr =  NULL;
    
    pMsgHdr = &pRcvPkt->msghdr;
    
  //zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvdata  pos =%d len %d!\r\n", pMsgHdr->msg_index, pMsgHdr->data_len);

    if((pMsgHdr->result == 0) && (pSession->iResult == 0))
    {
        if(pSession->iCurrPos == (int)pMsgHdr->msg_index)
        {
            pSession->iCurrLen = (int)pMsgHdr->data_len;
 
            if(pSession->iCurrLen > 0)    // &&(pSession->iCurrLen <= iDlen + IPC_HEADER_LEN))
            {
                filem_oper_write(pSession->iFileID, pRcvPkt->msg_data, pSession->iCurrLen);
  
                pSession->iCurrPos += pSession->iCurrLen;
            }
            else
            {
                if(pSession->iCurrLen < 0) pSession->iResult = -FILEM_ERROR_BTBTRANS;
            } 
        }
        else
        {
            zlog_debug(FILEM_DEBUG_TRANS, "Receive repeat data packet\r\n");
        }
    }
    else
    {
        if(pSession->iResult == 0) pSession->iResult = pMsgHdr->result;
    }
    
    filem_tran_snddatackpkt(pSession);
    /*
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvdata Pos = %d, iCurrlen= %d, iFileLen = %d\r\n",
                 pSession->iCurrPos, pSession->iCurrLen, pSession->iFileLen);
    */    
    if((pSession->iCurrPos < pSession->iFileLen) && (pSession->iResult == 0))
    {
         pSession->iMcState = HA_SMSG_FILE_DAT;
    }
    else 
    {
        if(pSession->iAsker)   // must get
        {
             pSession->iRetryMs = HA_SMSG_FILE_DAT;
             pSession->iMcState = HA_SMSG_FILE_ENDACK;

             filem_tran_sndendpkt(pSession);
        }
        else                   // must set
        {
             pSession->iRetryMs = HA_SMSG_FILE_DAT;
             pSession->iMcState = HA_SMSG_FILE_END;
        }
    }

    if((pSession->iAsker) && (pSession->pCbFun)) filem_tran_promptshow(pSession);

    return(0);
}


static void filem_tran_rcvdatackpkt(ha_file_sess *pSession,
                                    struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr =  &pRcvPkt->msghdr;
    /*
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvdataack  pos %d! sespos %d curlen %d result %d\r\n",
                pMsgHdr->msg_index, pSession->iCurrPos, pSession->iCurrLen, pMsgHdr->result);
    */
    if((pSession->iResult == 0) && (pMsgHdr->result == 0))
    {
        if((pSession->iCurrPos + pSession->iCurrLen) == (int)pMsgHdr->msg_index)
        {    
            pSession->iCurrPos = pMsgHdr->msg_index;

            if(pSession->iCurrPos >= pSession->iFileLen)
            {
                 if(pSession->iAsker)   //must put
                 {
                      pSession->iMcState = HA_SMSG_FILE_ENDACK;
                      
                      filem_tran_sndendpkt(pSession);
                 }
                 else                   //must get
                 {
                      pSession->iMcState = HA_SMSG_FILE_END;
                      pSession->iRetryCo = 1;
                      pSession->iTimeOut = FILE_RETRY_TIMEOUT * FILE_RETRY_COUNT;
                 }
            }
            else
            {
                 pSession->iRetryMs = 0;
                 
                 filem_tran_snddatpkt(pSession);
            }

            if((pSession->iAsker) && (pSession->pCbFun)) filem_tran_promptshow(pSession);
        }
      /*else 收到错误报文 不处理 等待超时重发上次发送的数据报文 */
    }
    else
    {
        if(pSession->iResult == 0) pSession->iResult = pMsgHdr->result;

        if(pSession->iAsker) filem_tran_sndendpkt(pSession);
    }
}

static void filem_tran_rcvendpkt(ha_file_sess *pSession,
                                 struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;

    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvend  result %d\r\n", pMsgHdr->result);

    if(pSession->iResult == 0) 
    {
        pSession->iResult = pMsgHdr->result;  
    }
    
    if((pSession->iResult == 0) &&
       (pSession->iFileID >= 0) && 
       ((int)pMsgHdr->msg_index != pSession->iFileLen))
    {
        pSession->iResult = -FILEM_ERROR_BTBTRANS;
    }
                                               /*传输被取消,不回ACK*/
    if(pMsgHdr->result != -FILEM_ERROR_CANCEL) filem_tran_sndendackpkt(pSession);

    if(!filem_slot_ismaster()) filem_tran_cfgrecvnoti(pSession);
    
    filem_tran_aswsessend(pSession);
}


static void filem_tran_rcvendackpkt(ha_file_sess *pSession,
                                    struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = NULL;

    pMsgHdr = &pRcvPkt->msghdr;
    
    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_rcvendack  result %d\r\n", pMsgHdr->result);
    
    if(pSession->iResult == 0) pSession->iResult = pMsgHdr->result;  
    
    filem_tran_asksessend(pSession);
}



/*------------------------------------------------------------------------------------------------------*/

static ha_file_sess* filem_tran_packetsesmatch(ha_file_sess *pSession,
                                               struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;
    /*
    if(pMsgHdr->opcode == IPC_OPCODE_DELETE)
        return(NULL);
    else
    */
    {
         //(pMsgHdr->unit    == pSession->iSrcUnit) &&
         //(pMsgHdr->srcunit == pSession->iDesUnit) &&
        if((pSession->iBusy) &&
           (pMsgHdr->slot    == pSession->iSrcSlot) &&
           (pMsgHdr->srcslot == pSession->iDesSlot) &&
           (pMsgHdr->sequence== pSession->iSessSno))
             return(pSession);
        else 
        {
           //other process ?
            return(NULL);
        }
    }
}

static ha_file_sess* filem_tran_requestpreproc(ha_file_sess *pSession,
                                               struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;

    if((pMsgHdr->data_len > 0) && 
       (pMsgHdr->data_len < FILEM_NAME_MAXLN))
    {
         char bTmpName[FILEM_NAME_MAXLN] = {0};

         memcpy(bTmpName, pRcvPkt->msg_data, pMsgHdr->data_len);
         
         bTmpName[pMsgHdr->data_len] = 0;

         if(pMsgHdr->opcode == IPC_OPCODE_DELETE)
         {
             if(filem_oper_remove(bTmpName) == 0)
             {
                 filem_tran_sndreqackpkt(pRcvPkt, iDlen, 0, -1);
             }
             else
             {
                 filem_tran_sndreqackpkt(pRcvPkt, iDlen, -FILEM_ERROR_NOEXIST, -1);
             }

             return(NULL); //删除文件，接收方不创建SESSION
         }
         else
         {
             if(pSession->iBusy)
             {
                  if(filem_tran_packetsesmatch(pSession, pRcvPkt, iDlen) != NULL)
                  {
                       if(pSession->iRetryMs == HA_SMSG_FILE_REQ)  //请求重发处理
                       {
                           filem_tran_sndreqackpkt(pRcvPkt, iDlen, pSession->iResult, pSession->iFileLen);
                                                                   //GET 请求重发第一个报文
                           if(pSession->iOpCode == IPC_OPCODE_GET)
                           {
                                pSession->iCurrPos = 0;
                                
                                filem_tran_snddatpkt(pSession);
                           }
                       }
                  }
                  else filem_tran_sndreqackpkt(pRcvPkt, iDlen, -FILEM_ERROR_CHANBUSY, -1);
                  
                  return(NULL);
             }
             else
             {
                  filem_tran_sessclr(pSession);
                  
                  pSession->iBusy    = 1;
                  pSession->iAsker   = 0;
                  pSession->iDesUnit = pMsgHdr->srcunit;
                  pSession->iDesSlot = pMsgHdr->srcslot;
                  pSession->iOpCode  = pMsgHdr->opcode;
                  pSession->iSessSno = pMsgHdr->sequence;
                  pSession->iFileLen = pMsgHdr->msg_index;

                  pSession->iMcState = HA_SMSG_FILE_REQ;
 
                  strcpy(pSession->bLocName, bTmpName);
 
                  return(pSession);
             }
         }
    }
    else 
    {
        filem_tran_sndreqackpkt(pRcvPkt, iDlen, -FILEM_ERROR_NAME, -1);
        return(NULL);
    }
}


/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/
/*add request*/
/*PUT 操作 请求端 接收状态机*/
static void  filem_tran_putaskrcvmc(ha_file_sess *pSession, 
                                    struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;

    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:

             zlog_debug(FILEM_DEBUG_TRANS, "Put sender no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:

             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_REQACK)
             {
                 filem_tran_rcvreqackpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                 zlog_debug(FILEM_DEBUG_TRANS, "Put asker receive not expecting packet\r\n");
             }
             break;

        case HA_SMSG_FILE_DAT:

             zlog_debug(FILEM_DEBUG_TRANS, "Put asker no this state!\r\n");
             break;
            
        case HA_SMSG_FILE_DATACK:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_DATACK)
             {
                 filem_tran_rcvdatackpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                 zlog_debug(FILEM_DEBUG_TRANS, "Put asker receive not expecting packet\r\n");
             }
             break;

        case HA_SMSG_FILE_END:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Put asker no this state!\r\n");
             break;

        case HA_SMSG_FILE_ENDACK:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_ENDACK)
             {
                 filem_tran_rcvendackpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                 zlog_debug(FILEM_DEBUG_TRANS, "Put asker receive not expecting packet\r\n");
             }
             break;
             
        default: break;
    }
}

static void filem_tran_putaswrcvmc(ha_file_sess *pSession, 
                                   struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;

    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_REQ)
             {
                 filem_tran_rcvreqpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                 zlog_debug(FILEM_DEBUG_TRANS, "Put answer receive not expecting packet\r\n");
             }
             break;
             
        case HA_SMSG_FILE_REQACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_DAT:
             
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_DAT)
             {
                  filem_tran_rcvdatpkt(pSession, pRcvPkt, iDlen);
             }
             else if(pMsgHdr->msg_subtype == HA_SMSG_FILE_END)
             {
                  filem_tran_rcvendpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Put answer receive not expecting packet\r\n");
             }
             break;
            
        case HA_SMSG_FILE_DATACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_END:
             
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_END)
             {
                  filem_tran_rcvendpkt(pSession, pRcvPkt, iDlen);
             }
             else if(pMsgHdr->msg_subtype == pSession->iRetryMs)
             {
                  filem_tran_snddatackpkt(pSession); 
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Put answer receive not expecting packet\r\n");
             }
             break;
            
        case HA_SMSG_FILE_ENDACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;
             
        default: break;
    }
}


static void filem_tran_getaskrevmc(ha_file_sess *pSession, 
                                   struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;
    
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:
    
             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:
                                                        //必须收到ACK
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_REQACK)
             {
                 filem_tran_rcvreqackpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                 zlog_debug(FILEM_DEBUG_TRANS, "Get asker receive not expecting packet\r\n");
             }
             break;

        case HA_SMSG_FILE_DAT:
    
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_DAT)
             {
                  filem_tran_rcvdatpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Get asker receive not expecting packet\r\n");
             }
             break;
             
        case HA_SMSG_FILE_DATACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;

             
        case HA_SMSG_FILE_END:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;

             
        case HA_SMSG_FILE_ENDACK:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_ENDACK)
             {
                  filem_tran_rcvendackpkt(pSession, pRcvPkt, iDlen);
             }
             else if(pMsgHdr->msg_subtype == pSession->iRetryMs)
             {
                  filem_tran_snddatackpkt(pSession); 
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Get asker receive not expecting packet\r\n");
             }
             
             break;
             
        default: break;
    }
}


static void filem_tran_getaswrevmc(ha_file_sess *pSession, 
                                   struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    struct ipc_msghdr_n *pMsgHdr = &pRcvPkt->msghdr;
    
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_REQ)
             {
                  filem_tran_rcvreqpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Get answer receive not expecting packet\r\n");
             }
             break;

             
        case HA_SMSG_FILE_REQACK:
                         
             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;

             
        case HA_SMSG_FILE_DAT:
             
             zlog_debug(FILEM_DEBUG_TRANS, "Get answer receive not expecting packet\r\n");
             break;
            
        case HA_SMSG_FILE_DATACK:
            
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_DATACK)
             {
                  filem_tran_rcvdatackpkt(pSession, pRcvPkt, iDlen);
             }
             else if(pMsgHdr->msg_subtype == HA_SMSG_FILE_END)
             {
                  filem_tran_rcvendpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Get answerreceive not expecting packet\r\n");
             }
             break;

    
        case HA_SMSG_FILE_END:
             
             if(pMsgHdr->msg_subtype == HA_SMSG_FILE_END)
             {
                  filem_tran_rcvendpkt(pSession, pRcvPkt, iDlen);
             }
             else
             {
                  zlog_debug(FILEM_DEBUG_TRANS, "Get answer receive not expecting packet\r\n");
             }
             break;
            
        case HA_SMSG_FILE_ENDACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;
             
        default: break;
    }
}


/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

void filem_tran_btbreceive(struct ipc_mesg_n *pRcvPkt, int iDlen)
{
    ha_file_sess *pMchSes = NULL;
    struct ipc_msghdr_n  *pMsgHdr = NULL;

    pMsgHdr = &pRcvPkt->msghdr;
        
    if((pMsgHdr->slot != gfilelocslot))
    {
        zlog_debug(FILEM_DEBUG_TRANS, "Receive packet not my slot\r\n");
        return;
    }
    
    if((pMsgHdr->opcode != IPC_OPCODE_ADD) &&
       (pMsgHdr->opcode != IPC_OPCODE_GET) &&
       (pMsgHdr->opcode != IPC_OPCODE_DELETE))
    {
        zlog_debug(FILEM_DEBUG_TRANS, "Receive packet with error opcode\r\n");
        return;
    }
                                          
    if(pMsgHdr->msg_subtype == HA_SMSG_FILE_REQ)
    {
        pMchSes = filem_tran_requestpreproc(&gFtSessionCtrl, pRcvPkt, iDlen);
    }
    else
    {
        pMchSes = filem_tran_packetsesmatch(&gFtSessionCtrl, pRcvPkt, iDlen);
    }
        
    if(pMchSes != NULL)
    {
        switch(pMsgHdr->opcode)
        {
            case IPC_OPCODE_ADD:
                 if(pMchSes->iAsker)
                      filem_tran_putaskrcvmc(pMchSes, pRcvPkt, iDlen);
                 else filem_tran_putaswrcvmc(pMchSes, pRcvPkt, iDlen);
                 break;
                 
            case IPC_OPCODE_GET:
                 if(pMchSes->iAsker)
                      filem_tran_getaskrevmc(pMchSes, pRcvPkt, iDlen);
                 else filem_tran_getaswrevmc(pMchSes, pRcvPkt, iDlen);
                 break;
                 
            case IPC_OPCODE_DELETE:
                 if((pMchSes->iAsker) && 
                    (pMsgHdr->opcode == IPC_OPCODE_DELETE) &&
                    (pMsgHdr->msg_subtype == HA_SMSG_FILE_REQACK))
                    
                 {
                     pMchSes->iBusy   = 0;
                     pMchSes->iResult = 0;
                     
                     if(pMchSes->pCbFun) 
                     {
                         if(pMsgHdr->result >= 0)
                              sprintf(pMchSes->bResInfo, "Delete file %s successfully!\r\n", pMchSes->bRemName);
                         else sprintf(pMchSes->bResInfo, "Delete file %s fail! %s\r\n", pMchSes->bRemName, filem_sys_errget(pMsgHdr->result));
                  
                         pMchSes->pCbFun(pMchSes->iResult, pMchSes->bResInfo,pMchSes->pCbPara, 1);
                     }
                 }
                 break;

            default: break;
        }
    }
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

static void filem_tran_putasktimeout(ha_file_sess *pSession)
{
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:

             zlog_debug(FILEM_DEBUG_TRANS, "Put sender no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:

             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_sndendpkt(pSession);

             break;
             
        case HA_SMSG_FILE_DAT:     
             
             zlog_debug(FILEM_DEBUG_TRANS, "Put sender no this state!\r\n");
             break;

        case HA_SMSG_FILE_DATACK:

             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_sndendpkt(pSession);

             break;

        case HA_SMSG_FILE_END:
             
             zlog_debug(FILEM_DEBUG_TRANS, "Put sender no this state!\r\n");
             break;

        case HA_SMSG_FILE_ENDACK:

             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_asksessend(pSession);

             break;
             
        default: break;    
    }
}

static void filem_tran_putaswtimeout(ha_file_sess *pSession)
{
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:

             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:
             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_DAT:     

             pSession->iResult = -FILEM_ERROR_TRANSTOUT;
             
             filem_tran_aswsessend(pSession);

             break;

        case HA_SMSG_FILE_DATACK:
            
             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_END:

           //pSession->iResult = -FILEM_ERROR_TRANSTOUT;
             
             filem_tran_aswsessend(pSession);

             break;

        case HA_SMSG_FILE_ENDACK:

             zlog_debug(FILEM_DEBUG_TRANS, "Put answer no this state!\r\n");
             break;
             
        default: break;    
    }
}



static void filem_tran_getasktimeout(ha_file_sess *pSession)
{
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:

             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:

             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_sndendpkt(pSession);

             break;
             
        case HA_SMSG_FILE_DAT:     
             
             pSession->iResult = -FILEM_ERROR_TRANSTOUT;

             filem_tran_asksessend(pSession);
              
             break;

        case HA_SMSG_FILE_DATACK:

             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;

        case HA_SMSG_FILE_END:
             
             zlog_debug(FILEM_DEBUG_TRANS, "Get asker no this state!\r\n");
             break;


        case HA_SMSG_FILE_ENDACK:

             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_asksessend(pSession);
           
             break;
             
        default: break;    
    }
}

static void filem_tran_getaswtimeout(ha_file_sess *pSession)
{
    switch(pSession->iMcState)
    {
        case HA_SMSG_FILE_REQ:

             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;
             
        case HA_SMSG_FILE_REQACK:

             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_DAT:     

             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;

        case HA_SMSG_FILE_DATACK:
            
             pSession->iResult = filem_tran_sndretrypkt(pSession);

             if(pSession->iResult != 0) filem_tran_aswsessend(pSession);;

             break;

        case HA_SMSG_FILE_END:

           //pSession->iResult = -FILEM_ERROR_TRANSTOUT;

             filem_tran_aswsessend(pSession);

             break;

        case HA_SMSG_FILE_ENDACK:

             zlog_debug(FILEM_DEBUG_TRANS, "Get answer no this state!\r\n");
             break;
             
        default: break;    
    }
}


void filem_tran_timeout(void)
{
    ha_file_sess *pSession = NULL;

    pSession = &gFtSessionCtrl;

    if(pSession->iBusy && (pSession->iTimeOut > 0))
    {
        pSession->iTimeOut --;

        if(pSession->iTimeOut == 0)
        {
                pSession->iRetryCo --;

                if(pSession->iOpCode == IPC_OPCODE_ADD)
                {
                    if(pSession->iAsker)
                         filem_tran_putasktimeout(pSession);
                    else filem_tran_putaswtimeout(pSession);
                }
                else
                {
                    if(pSession->iAsker)
                         filem_tran_getasktimeout(pSession);
                    else filem_tran_getaswtimeout(pSession);
                }
        }
    }
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

void filem_tran_forcestop(int iIsCb) 
{
    ha_file_sess *pSession = &gFtSessionCtrl;

    if(pSession->iBusy)
    {
        zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_forcestop fileid=%d name=%s!\r\n", 
                    pSession->iFileID, pSession->bLocName);

        pSession->iBusy = 0;

        pSession->iResult = -FILEM_ERROR_CANCEL;

        if(pSession->iAsker) filem_tran_sndendpkt(pSession);
        
        if((iIsCb) && (pSession->pCbFun))
        {
            sprintf(pSession->bResInfo, "transmit forcestop!\r\n");
            
            pSession->pCbFun(pSession->iResult, pSession->bResInfo, pSession->pCbPara, 1);
        }                                  /*强制结束,通知自动同步终止*/
        
        if(pSession->iFileID >= 0)
        {
            filem_oper_close(pSession->iFileID);
        }
        
        if((pSession->iAsker && (pSession->iOpCode == IPC_OPCODE_GET)) || 
          (!pSession->iAsker && (pSession->iOpCode != IPC_OPCODE_GET)))
        {
            filem_oper_remove(pSession->bLocName);
        }
        
        filem_tran_sessclr(pSession);

        filem_oper_flush();

        printf("file transmit forcestop \r\n");

		zlog_notice("file transmit forcestop because CPU switch over.\r\n");
    }
}
/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

int filem_tran_putfile(int iUnit, int iSlot, char *pLocFile, 
                       char *pRemFile, FilemCall pCbfun, void *pCbPar)
{
    ha_file_sess *pSession = NULL;

    pSession = &gFtSessionCtrl;

    if(pSession->iBusy) return(-FILEM_ERROR_CHANBUSY);

    filem_tran_sessclr(pSession);
    
    pSession->iBusy     = 1;
    pSession->iAsker    = 1;
    pSession->iOpCode   = IPC_OPCODE_ADD;
    pSession->iDesUnit  = iUnit;
    pSession->iDesSlot  = iSlot;
    pSession->iSessSno  = ++gSesSerialno;
    pSession->pCbFun    = pCbfun;
    pSession->pCbPara   = pCbPar;
            
    strncpy(pSession->bLocName, pLocFile, FILEM_NAME_MAXLN-1);

    if(!pRemFile)
         strncpy(pSession->bRemName, pLocFile, FILEM_NAME_MAXLN-1);
    else strncpy(pSession->bRemName, pRemFile, FILEM_NAME_MAXLN-1); 

    pSession->iFileID = filem_oper_open(pSession->bLocName, O_RDONLY, 0666);

    if(pSession->iFileID < 0)
    {
        filem_tran_sessclr(pSession);
    
        return(-FILEM_ERROR_NOEXIST);
    }
    else
    {
        pSession->iFileLen = filem_oper_size_getf(pSession->iFileID);

        if(pSession->iFileLen < 0) 
        {
            filem_tran_sessclr(pSession);
            return(-FILEM_ERROR_LENGTH);
        }
        else
        {
            zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_putfile file name %s len=%d!\r\n", pLocFile, pSession->iFileLen);

            filem_tran_sndreqpkt(pSession);

            return(0);
        }
    }
}

/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

int filem_tran_getfile(int iUnit, int iSlot, char *pLocFile, 
                       char *pRemFile, FilemCall pCbfun, void *pCbPar)
{
    ha_file_sess *pSession = NULL;

    pSession = &gFtSessionCtrl;

    if(pSession->iBusy) return(-FILEM_ERROR_CHANBUSY);

    filem_tran_sessclr(pSession);
    
    pSession->iBusy     = 1;
    pSession->iAsker    = 1;
    pSession->iOpCode   = IPC_OPCODE_GET;
    pSession->iDesUnit  = iUnit;
    pSession->iDesSlot  = iSlot;
    pSession->iSessSno  = ++gSesSerialno;
    pSession->pCbFun    = pCbfun;
    pSession->pCbPara   = pCbPar;
            
    strncpy(pSession->bRemName, pRemFile, FILEM_NAME_MAXLN-1);

    if(!pLocFile)
         strncpy(pSession->bLocName, pRemFile, FILEM_NAME_MAXLN-1);
    else strncpy(pSession->bLocName, pLocFile, FILEM_NAME_MAXLN-1); 

    zlog_debug(FILEM_DEBUG_TRANS, "filem_tran_getfile file name %s len=%d!\r\n", pRemFile, pSession->iFileLen);

    filem_tran_sndreqpkt(pSession);

    return(0);
}


int filem_tran_delfile(int iUnit, int iSlot, char *pLocFile, 
                       char *pRemFile, FilemCall pCbfun, void *pCbPar)
{
    ha_file_sess *pSession = NULL;

    pSession = &gFtSessionCtrl;

    if(pSession->iBusy) return(-FILEM_ERROR_CHANBUSY);

    filem_tran_sessclr(pSession);
    
    pSession->iBusy     = 1;
    pSession->iAsker    = 1;
    pSession->iOpCode   = IPC_OPCODE_DELETE;
    pSession->iDesUnit  = iUnit;
    pSession->iDesSlot  = iSlot;
    pSession->iSessSno  = ++gSesSerialno;
    pSession->pCbFun    = pCbfun;
    pSession->pCbPara   = pCbPar;
            
    strncpy(pSession->bRemName, pRemFile, FILEM_NAME_MAXLN-1);

    if(!pLocFile)
         strncpy(pSession->bLocName, pRemFile, FILEM_NAME_MAXLN-1);
    else strncpy(pSession->bLocName, pLocFile, FILEM_NAME_MAXLN-1); 

    return(filem_tran_sndreqpkt(pSession));
}

int filem_tran_tranisbusy(void)
{
    return(gFtSessionCtrl.iBusy);
}
/*
static  void filem_tran_batchcfgcall(int iResult, char *pResInfo, void *pPar, int iFlag)
{
    if(iResult == 0)
    {
         printf("Config data batch sync to slave ok! \r\n");

         zlog_notice("Config data batch sync to slave ok! \r\n");
    }
    else
    {
         printf("Config data batch sync to slave fail! %s \r\n", filem_sys_errget(iResult));

         zlog_err("Config data batch sync to slave fail! %s \r\n", filem_sys_errget(iResult));
    }
}
*/
/*------------------------------------------------------------------------------------------------------*/
/*
int filem_tran_batchcfgfile(char *pMsgdata)
{
    int    iret = 0;
    struct ha_batch_req sreq;

    memcpy(&sreq, pMsgdata, sizeof(struct ha_batch_req));
    
    printf("File trans config batch request, unit=%d, slot=%d,file=%s\r\n", sreq.unit, sreq.slot, sreq.bFile);
    
 // zlog_debug(FILEM_DEBUG_TRANS, "File trans config batch request, unit=%d, slot=%d,file=%s\r\n", sreq.unit,sreq.slot, sreq.bFile);

    filem_tran_forcestop(1);

    if(filem_oper_isexist(sreq.bFile))
    {
        iret = filem_tran_putfile(0, sreq.slot, sreq.bFile, NULL, filem_tran_batchcfgcall, NULL); 
    }
    else
    {
        iret = -FILEM_ERROR_NOEXIST;
        
        printf("Config data batch file no exist %s \r\n", sreq.bFile);

        zlog_err("Config data batch file no exist %s \r\n", sreq.bFile);
    }

    return(iret);
}
*/
/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/

int filem_tran_init(void)
{
    memset(&gFtSessionCtrl, 0, sizeof(gFtSessionCtrl));
        
    gFtSessionCtrl.iSrcSlot =  gfilelocslot;
    gFtSessionCtrl.iFileID  = -1;

    gFtSessionCtrl.iBlksize =  FILE_TRSBLK_SIZE;

    gFtSessionCtrl.bSndBuf = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(struct ipc_mesg_n) + IPC_MSG_LEN_N);
  //gFtSessionCtrl.bRcvBuf = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(struct ipc_mesg_n) + IPC_MSG_LEN_N);
    
    return(0);
}
/*------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------*/


