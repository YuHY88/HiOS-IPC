/*
 *  define IPC message queue operation
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
#include <linux/sysctl.h>
#include "log.h"

#include "zassert.h"
#include "thread.h"
#include "memshare.h"
#include "msg_ipc_n.h"

/*====================================================================================================================*/
/*====================================================================================================================*/

static int         iIpcFirstRecvT  =  1;
static int         iIpcReadPipdFd  = -1;

static int         iIpcWritePipeFd = -1;    //所有线程使用一条管道

static MsgProcCall fIpcMsgCallBak[IPC_RECV_TASK_MAX] = {NULL};    
                                           //callback要区分
static struct thread_master *gPMasterThread = NULL;


/*====================================================================================================================*/

/* ***********************************************************
 *  Function   : ipc_recv_task_entry
 *  return     : 0 ok, -1 error
 *  description: ipc message receive thread  
 * ***********************************************************/

static void* ipc_recv_thread_entry(void *para)
{ 
     int                   iRecvMod = 0;
     int                   iCbkFIdx = 0;
     struct ipc_quemsg_n   sPipeMsg = {0,0,0,NULL};
     struct ipc_thread_par sTrdPara;
     static int            iEvtBrkC = 0;
     /*
     static int            iMsgrcvC = 0;
     */
     memcpy(&sTrdPara, para, sizeof(struct ipc_thread_par));

     iRecvMod = sTrdPara.iRevMod;

     iCbkFIdx = sTrdPara.iQueIdx;

     if((iRecvMod < 1) || (iRecvMod >= MODULE_ID_MAXNUM) ||
        (iCbkFIdx < 0) || (iCbkFIdx >= IPC_RECV_TASK_MAX) ) 
     {
         printf("%s, %d, fd error qid=%d, cbid=%d\r\n",  __FUNCTION__, __LINE__, iRecvMod, iCbkFIdx);
         return(NULL);
     }

     prctl(PR_SET_NAME, sTrdPara.bTrdName);   
#if 1
     if(iIpcFirstRecvT)
     {
         sigset_t  sNewSigSet;  
                                                      /* 第一个接收线程中,打开ALARM信号    */
         sigemptyset(&sNewSigSet);  
         sigaddset(&sNewSigSet, SIGALRM);  
         sigprocmask(SIG_UNBLOCK, &sNewSigSet, NULL);

         iIpcFirstRecvT = 0;
     }
#endif

  // printf("PR_SET_NAME = %s, RMOD=%d, cbid=%d\r\n",  sTrdPara.bTrdName, iRecvMod, iCbkFIdx);
     
     while(1)
     {
          if(ipc_recv_que_n(iRecvMod, &sPipeMsg) == 0)
          {
              /*   test code
              iMsgrcvC ++;

              if((sPipeMsg.recvmod != 22) || 
                ((sPipeMsg.queueid != 123456) && (sPipeMsg.queueid != 0)))
              {
                   printf(" filem recv msg ipc msg data error !!!!!!!2222\r\n");
              }

              if(iMsgrcvC > 10000)
              {
                  printf(" filem recv msg iMsgrcvC = %d\r\n", iMsgrcvC); 
                  iMsgrcvC = 0;
              }
              */ 
              
              sPipeMsg.queueid = iCbkFIdx;
                                                 
              if(write(iIpcWritePipeFd, &sPipeMsg, sizeof(struct ipc_quemsg_n)) > 0)
              {
                   memset(&sPipeMsg, 0,  sizeof(struct ipc_quemsg_n));
              }
              else
              {
                   usleep(10000);

                   if(sPipeMsg.msgbuff) mem_share_free(sPipeMsg.msgbuff, iRecvMod);
                   
                   printf("%s, %d, pipe write error, %d, errno=%d\n", __FUNCTION__, __LINE__, sizeof(struct ipc_quemsg_n), errno);
                  
                   zlog_err("%s, %d, pipe write error, %d\n", __FUNCTION__, __LINE__, sizeof(struct ipc_quemsg_n));
              }
          }
          else
          {
              if(errno != EINTR)
              {
                   usleep(10000);
                   
                 //printf("%s, %d, ipc receivee error!\n", __FUNCTION__, __LINE__);
                  
                   zlog_err("%s, %d, ipc receivee error!\n", __FUNCTION__, __LINE__);
              }
              else   //test code
              {
                   iEvtBrkC ++;
                   if(iEvtBrkC > 10000)
                   {
                       iEvtBrkC = 0;
                    // printf("%s, %d, ipc receivee error, signal interrupt! %d, %s\n", __FUNCTION__, __LINE__, getpid(), sTrdPara.bTrdName);
                   }
              }
          }
     }

     return(NULL);
}



/* ***********************************************************
 *  Function   : ipc_recv_thread_create
 *  return     : 0 ok, -1 error
 *  description: ipc message receive thread  create
 * ***********************************************************/

static int ipc_recv_thread_create(void* (*pfun)(void*), int iStackSz,
                                  int iPolicy, int iPrio, void *pPara)
{
    int                 iRet = 0;
    pthread_t           iTid = 0;
    pthread_attr_t      sThAttr;
    struct sched_param  sParam;

    if(iPrio < 0)
    {
        iPolicy = sched_getscheduler(getpid());

        if(iPolicy == SCHED_OTHER)
        {
            sParam.sched_priority = getpriority(PRIO_PROCESS, getpid());

           //sParam.sched_priority = iPrio;
        }
        else
        {
            sched_getparam(getpid(), &sParam);
           //sParam.sched_priority = iPrio;        
        }
    }
    else
    {
         sParam.sched_priority = iPrio;
    }
    
    pthread_attr_init(&sThAttr);

    pthread_attr_setscope(&sThAttr, PTHREAD_SCOPE_SYSTEM);

    pthread_attr_setstacksize(&sThAttr, iStackSz);

    pthread_attr_setdetachstate(&sThAttr, PTHREAD_CREATE_DETACHED);
    
    pthread_attr_setinheritsched(&sThAttr, PTHREAD_EXPLICIT_SCHED);
    
    pthread_attr_setschedpolicy(&sThAttr, iPolicy); //SCHED_RR
    
    pthread_attr_setschedparam(&sThAttr, &sParam);

    iRet = pthread_create(&iTid, &sThAttr, pfun, pPara);
    
    if(iRet != 0)
    {
         printf("%s, %d, thread create error, %d\n", __FUNCTION__, __LINE__, iRet);
         
         zlog_err("%s, %d, thread create error, %d\n", __FUNCTION__, __LINE__, iRet);
         
         return(-1);
    }
    else 
    {
       //pthread_setname_np(iTid, bName);
         return( 0);
    }
}


/* ***********************************************************
 *  Function   : ipc_recv_thread_start
 *  para       : iRecvMod  receive module (APP)
 *  para       : iPrio     receive thread priority(no used)
 *  para       : pMsgProc  message process function
 *  para       : iClrQue   whether clean message queue or not
 *  return     : 0 ok, -1 error
 *  description: ipc message receive thread  start
 * ***********************************************************/

int ipc_recv_thread_start(const char *bName, int iRecvMod, int iPolicy, int iPrio,  
                          MsgProcCall pMsgProc, int iClrQue)
{
     int           iIndex = 0;
     int           iQueid = 0;
   //unsigned long iParVa = 0;

     static struct ipc_thread_par sParVa = {0};
     
        
     if((pMsgProc == NULL) || (bName == NULL))
     {
         printf("%s, %d, message proc fun or thread name can't be null\n", __FUNCTION__, __LINE__);
         
         zlog_err("%s, %d, message proc fun or thread name can't be null\n", __FUNCTION__, __LINE__);
         return(-1);
     }
     /*
     if((iPrio < 0) || (iPrio > 99))
     {
         printf("%s, %d, thread priority=%d is error\n", __FUNCTION__, __LINE__, iPrio);
         
         zlog_err("%s, %d, thread priority=%d is error\n", __FUNCTION__, __LINE__, iPrio);
         return(-1);
     }
     */       
     if((iRecvMod < 1) || (iRecvMod >= MODULE_ID_MAXNUM))
     {
         printf("%s, %d, recv module id=%d is error\n", __FUNCTION__, __LINE__, iRecvMod);
         
         zlog_err("%s, %d, recv module id=%d is error\n", __FUNCTION__, __LINE__, iRecvMod);
         return(-1);
     }
    
     if((iQueid = ipc_get_qid_n(iRecvMod, 1)) < 0)
     {
         printf("%s, %d, recv queue id=%d is error\n", __FUNCTION__, __LINE__, iQueid);
         
         zlog_err("%s, %d, recv queue id=%d is error\n", __FUNCTION__, __LINE__, iQueid);
         return(-1);
     }

     if(iClrQue) ipc_clear_que_n(iRecvMod, 0);            /*清理队列中的垃圾消息*/
 
     for(iIndex = 1; iIndex < IPC_RECV_TASK_MAX; iIndex++)
     {
         if(fIpcMsgCallBak[iIndex] == NULL) break;
     }

     if(iIndex >= IPC_RECV_TASK_MAX)
     {
         printf("%s, %d, start too many receive thread =%d is error\n", __FUNCTION__, __LINE__, iIndex);

         zlog_err("%s, %d, start too many receive thread =%d is error\n", __FUNCTION__, __LINE__, iIndex);
         return(-1);
     }

     fIpcMsgCallBak[iIndex] = pMsgProc;

     memset(&sParVa, 0, sizeof(struct ipc_thread_par));
     
     sParVa.iRevMod  = iRecvMod;
     sParVa.iQueIdx  = iIndex;
     
     strncpy(sParVa.bTrdName, bName, 15);
          
     if(ipc_recv_thread_create(ipc_recv_thread_entry, 128 * 1024,  iPolicy, iPrio, &sParVa) == -1)
     {
         zlog_err("%s, %d, create thread error\n", __FUNCTION__, __LINE__);
         return(-1);
     }
     else
     {
         usleep(300000);
         return(0);
     }
}
/*====================================================================================================================*/

/* ***********************************************************
 *  Function   : ipc_recv_thread_read
 *  return     : 0 ok, -1 error
 *  description: ipc message read & process  
 * ***********************************************************/

static int ipc_recv_thread_read(struct thread *pThread)
{
    int                  iLen;
    struct ipc_quemsg_n  bMsg = {0};
    
    iLen = read(iIpcReadPipdFd, &bMsg, sizeof(struct ipc_quemsg_n));
    
    if(iLen <= 0)
    {
        zlog_err("%s, %d, read pipe error\n", __FUNCTION__, __LINE__);
    }
    else
    {
        if(bMsg.msgbuff != NULL) //high_pre_timer_dotick();  (bMsg.msgbuff == NULL)TIMER MSG
        {
            if((bMsg.queueid > 0) && 
               (bMsg.queueid < IPC_RECV_TASK_MAX))
            {
                if((fIpcMsgCallBak[bMsg.queueid] != NULL) &&
                   (bMsg.msgdlen >= IPC_HEADER_LEN_N))
                {
                     fIpcMsgCallBak[bMsg.queueid](bMsg.msgbuff, bMsg.msgdlen);
                }
                else
                {
                     mem_share_free(bMsg.msgbuff, bMsg.recvmod);
                }
            }
            else mem_share_free(bMsg.msgbuff, bMsg.recvmod);
        }
    }
       
    thread_add_read(gPMasterThread, ipc_recv_thread_read, NULL, iIpcReadPipdFd);

    return(iLen);
}


/*====================================================================================================================*/
/* ***********************************************************
 *  Function   : ipc_recv_init
 *  para       : pMaster APP process master thread 
 *  return     : >=0 fd  for main thread useing, -1 error
 *  description: ipc receive init for APP
 * ***********************************************************/
int ipc_recv_init(void *pMaster)
{
    int iRetVa    = -1;
    int iPipFd[2] = {-1, -1};

    memset(fIpcMsgCallBak, 0, sizeof(MsgProcCall) * IPC_RECV_TASK_MAX);

    if(pMaster == NULL) 
    {
        printf("%s, %d, init parameter error pmaster=NULL\n", __FUNCTION__, __LINE__);
        
        zlog_err("%s, %d, init parameter error pmaster=NULL\n", __FUNCTION__, __LINE__);
        return(-1);
    }

    gPMasterThread = (struct thread_master*)pMaster;
  
    iRetVa = pipe(iPipFd);
    
    if(iRetVa != 0)
    {
        printf("%s, %d, create pipe error\n", __FUNCTION__, __LINE__);
         
        zlog_err("%s, %d, create pipe error\n", __FUNCTION__, __LINE__);
        return(-1);
    }
    else
    {
        iIpcReadPipdFd  = iPipFd[0];
        
        iIpcWritePipeFd = iPipFd[1];

        thread_add_read(gPMasterThread, ipc_recv_thread_read, NULL, iIpcReadPipdFd);

      //printf("%s, %d, create pipe readfd=%d,writefd=%d\n", __FUNCTION__, __LINE__, iIpcReadPipdFd, iIpcWritePipeFd);

        return(iIpcReadPipdFd);
    }
}
/*====================================================================================================================*/

/*====================================================================================================================*/



