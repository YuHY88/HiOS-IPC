/*====================================================================================================================*/
/*------------------------------------------------------------------------*/
/* �� �� �� : hptimer.c                                                      */
/*  ��    �� : suxiqing                                                     */
/* �������� : 2006-08-05                                                      */
/* ��    Ȩ :                                                               */
/*-------------------------------------------------------------------------*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*====================================================================================================================*/

#ifdef  __cplusplus
extern "C"{
#endif


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
/*====================================================================================================================*/
/*====================================================================================================================*/

typedef struct _high_pre_timer_ 
{
    struct _high_pre_timer_     *pNext;
    struct _high_pre_timer_     *pPrev;

    void                        *pArg;
    int                        (*pFunc)(void*);
    
    int                          iLoop;             /* ѭ����ʱ����־             */
    int                          iDelete;           /* ɾ����־             */
    int                          iInterval;         /* ��ʱ��ʱ�䳤�� ����          */
    int                          iTick;             /* ��ʱ����ʱtick��          */
    int                          iTickGap;          /* ��ǰһ����ʱ����ʱ��� tick��     */

    char                         bName[32];

}high_pre_timer;

typedef struct _high_pre_timer_tbl_
{
    high_pre_timer              *pTHead;
    int                          iCount;
    int                          iBasInt;           /*��׼��ʱ��*/
    int                          iModule;
    sem_t                        lSemVal;             
}high_pre_timer_lst;


/*====================================================================================================================*/
/*====================================================================================================================*/

static timer_t              gBaseTimerID   =  0;

static high_pre_timer_lst   gTimerListHead = {0}; /*������ʱ����                */

static high_pre_timer_lst   gTempTimerList = {0}; /*ժ���ĳ�ʱ��ʱ����ɵĵ�����*/
 
static struct ipc_quemsg_n  gBTimerMsgData = {0}; /*��׼��ʱ����Ϣ      */

/*-------------------------------------------------------*/
/*Name: high_pre_timer_subadd                            */
/*Para: pTimer --- ��ʱ��                                   */
/*retu: NONE                                             */
/*desc: ��gTimerListHead�������Ӷ�ʱ��                           */
/*-------------------------------------------------------*/
static void  high_pre_timer_subadd(high_pre_timer *pTimer)
{
    high_pre_timer *pNode = NULL;
    high_pre_timer *pTemp = NULL;
    int             iTick = 0;

    sem_wait(&gTimerListHead.lSemVal);
                                                /* �ҵ���һ����Ҫ����Ķ�ʱ������ʱ�Ķ�ʱ�� */
    pNode = gTimerListHead.pTHead;

    while((NULL != pNode) &&
         ((iTick += pNode->iTickGap) < pTimer->iTick))
    {
        pTemp = pNode;
        pNode = pNode->pNext;
    }

    if(NULL != pNode)                          /* ����ҵ��� */
    {
         pTimer->iTickGap = pTimer->iTick + pNode->iTickGap - iTick;
                                               /* ������һ������lTickGap */
         pNode->iTickGap  = iTick - pTimer->iTick;
         pTimer->pNext  = pNode;
         pTimer->pPrev  = pNode->pPrev;
 
         if(pNode->pPrev)
              pNode->pPrev->pNext = pTimer;
         else gTimerListHead.pTHead = pTimer;
 
         pNode->pPrev = pTimer;
    }
    else
    {
         if(NULL == gTimerListHead.pTHead)     /* ����Ϊ�� */
         {
             gTimerListHead.pTHead = pTimer;
             
             pTimer->iTickGap = pTimer->iTick;
             pTimer->pNext = pTimer->pPrev = NULL;
         }
         else                                  /* ������û�б�Ҫ����Ķ�ʱ������Ķ�ʱ�� */
         {
             pTimer->iTickGap = pTimer->iTick - iTick;
 
             if(pTemp != NULL) pTemp->pNext = pTimer;
 
             pTimer->pPrev = pTemp;
             pTimer->pNext = NULL;
         }
    }
    
    gTimerListHead.iCount++;
    
    sem_post(&gTimerListHead.lSemVal);
}


/*-------------------------------------------------------*/
/*Name: high_pre_timer_timeout                           */
/*retu: NONE                                             */
/*desc: ��ʱ����ʱִ�к���                                        */
/*-------------------------------------------------------*/
static void high_pre_timer_timeout(void)
{
    high_pre_timer *pTimer = NULL;

    while(1)
    {
        sem_wait(&gTimerListHead.lSemVal);      /*only use timer list sem */ 

        pTimer = gTempTimerList.pTHead;

        if(pTimer != NULL)
        {
             if(NULL != pTimer->pNext)
             {            
                 pTimer->pNext->pPrev = NULL;
             }

             gTempTimerList.iCount --;
             gTempTimerList.pTHead = pTimer->pNext;
        }
        
        sem_post(&gTimerListHead.lSemVal);

        if(pTimer == NULL) break;
        
        pTimer->pNext = pTimer->pPrev = NULL;
        
        if(pTimer->pFunc) (pTimer->pFunc)(pTimer->pArg);
        
        if(pTimer->iLoop  && !pTimer->iDelete)
             high_pre_timer_subadd(pTimer);
        else free(pTimer);

      //gTempTimerList.pTHead = NULL; gTempTimerList.iCount = 0; 
    }
}


/*-------------------------------------------------------*/
/*Name: high_pre_timer_dotick                            */
/*retu: void                                             */
/*desc: ��ʱ��λ��ʱ                                           */
/*-------------------------------------------------------*/
void high_pre_timer_dotick(void)
{
    int              lCount = 0;
    high_pre_timer  *pNode = NULL;

  //printf(" do timer tick\r\n");
    
    sem_wait(&gTimerListHead.lSemVal);

    pNode = gTimerListHead.pTHead;

    if(NULL != pNode)  pNode->iTickGap --;

    while((NULL != pNode) && (pNode->iTickGap <= 0))
    {
        lCount ++;
        pNode = pNode->pNext;
    }
                                             /* �����г�ʱ�Ķ�ʱ����gTimerListHeadժ����gTempTimerList��*/
    if(lCount > 0)
    {
        gTempTimerList.pTHead = gTimerListHead.pTHead;

        if(NULL != pNode)
        {
            gTimerListHead.pTHead = pNode;
            pNode->pPrev->pNext = NULL;
            pNode->pPrev = NULL;
        }
        else                                /* ���ȫ����ʱ����ʱ */
        {
            gTimerListHead.pTHead = NULL;
        }

        gTimerListHead.iCount -= lCount;
        
        gTempTimerList.iCount  = lCount;
    }

    sem_post(&gTimerListHead.lSemVal);
    
    if(lCount > 0) high_pre_timer_timeout();               
}
   
/*====================================================================================================================*/
/*====================================================================================================================*/
/*-------------------------------------------------------*/
/*Name: high_pre_timer_add                               */
/*Para: iType    -- 0 noloop 1 loop                      */
/*Para: pFunc    -- ��ʱִ�к���                               */
/*Para: pArg    -- ��������                                  */
/*Para: iMillSec   -- ��ʱ����ʱ�����                           */
/*retu: ����Timer ID��ʧ���򷵻� 0                               */
/*desc: ������ʱ��                                            */
/*-------------------------------------------------------*/

TIMERID high_pre_timer_add(const char *bName, int iType, int(*pFunc)(void *), void *pArg, int iMillSec)
{
    high_pre_timer *pTimer = NULL;
                                                 /* ��ʱ��ʱ�䲻������1��tick, ��ʱ֪ͨ�ֶα������ */
    if((iMillSec < gTimerListHead.iBasInt ) || 
       (pFunc == NULL))
    {
        return((TIMERID)NULL);
    }

    pTimer = (high_pre_timer*)malloc(sizeof(high_pre_timer));
       
    if(NULL != pTimer)
    {
        memset(pTimer, 0, sizeof(high_pre_timer));
        
        pTimer->iTick    = iMillSec / gTimerListHead.iBasInt;
        pTimer->iInterval= iMillSec;
        pTimer->pFunc    = pFunc;
        pTimer->pArg     = pArg;
        pTimer->iTickGap = 0;
        pTimer->iDelete  = 0;
        pTimer->iLoop    = (iType == LIB_TIMER_TYPE_NOLOOP) ? 0 : 1;
        pTimer->pNext    = pTimer->pPrev = NULL;

        if(bName) strncpy(pTimer->bName, bName, 31);

        high_pre_timer_subadd(pTimer);
    }

    return((TIMERID)pTimer);
}

/*-------------------------------------------------------*/
/*Name: VOS_TimerDelete                                  */
/*Para: ulTimerID  -- Timer Id                           */
/*retu: �ɹ�0��ʧ���򷵻�-1                                      */
/*desc: ɾ����ʱ��                                            */
/*-------------------------------------------------------*/
int high_pre_timer_delete(TIMERID lTimerId)
{
    high_pre_timer *pTimer = NULL;
    high_pre_timer *pNode  = NULL;

    pTimer = (high_pre_timer*)lTimerId;

    if(NULL == pTimer) return(-1);
  
    sem_wait(&gTimerListHead.lSemVal);

    pNode = gTimerListHead.pTHead;

    while(pNode && (pNode != pTimer)) pNode = pNode->pNext;

    if(NULL != pNode)                 /* ��������ָ���Ķ�ʱ�� */
    {
        if(NULL != pTimer->pNext)
        {
            pTimer->pNext->iTickGap += pTimer->iTickGap;
            pTimer->pNext->pPrev = pTimer->pPrev;
        }

        if(NULL != pTimer->pPrev)
             pTimer->pPrev->pNext  = pTimer->pNext;
        else gTimerListHead.pTHead = pTimer->pNext;

        gTimerListHead.iCount--;
    }
#if 1                                /*ɾ����ʱ�����еĶ�ʱ��*/
    else
    {
        pNode = gTempTimerList.pTHead;

        while(pNode && (pNode != pTimer)) pNode = pNode->pNext;
        
        if(NULL == pNode)           /*û�ҵ�*/         
        {
             pTimer->iDelete = 1;
        }
        else                       /* ��������ָ���Ķ�ʱ�� */
        {
             if(NULL != pTimer->pNext)
             {
                 pTimer->pNext->pPrev = pTimer->pPrev;
             }
        
             if(NULL != pTimer->pPrev)
                  pTimer->pPrev->pNext  = pTimer->pNext;
             else gTempTimerList.pTHead = pTimer->pNext;
        
             gTempTimerList.iCount--;
        }
    }
#endif
    if(pNode) free(pTimer);
    
    sem_post(&gTimerListHead.lSemVal);

    return(pNode ? 0 : -1);
}

/*-------------------------------------------------------*/
/*Name: high_pre_timer_passtime                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- ��ʱ���Ѿ��߹���ʱ��               */
/*retu: �ɹ�0��ʧ���򷵻�-1                           */
/*desc: ��ö�ʱ���Ѿ��߹���ʱ��                         */
/*-------------------------------------------------------*/
int high_pre_timer_passtime(TIMERID lTimerId, int *plRet)
{
    int             lResult = -1;
    int             lTick   = 0;
    high_pre_timer *pTimer  = NULL;
    high_pre_timer *pNode   = NULL;

    pTimer = (high_pre_timer*)lTimerId;

    if((NULL == pTimer) || (NULL == plRet)) return(-1);
 
    *plRet = 0;
    
    sem_wait(&gTimerListHead.lSemVal);

    pNode = gTimerListHead.pTHead;

    while(pNode && (pNode != pTimer))
    {
        lTick += pNode->iTickGap;
        pNode  = pNode->pNext;
    }

    if(NULL != pNode)  
    {
        lResult = 0;
        
        lTick += pNode->iTickGap;
        
       *plRet = (pNode->iTick - lTick) * gTimerListHead.iBasInt;
    }

    sem_post(&gTimerListHead.lSemVal);

    return(lResult);
}

/*-------------------------------------------------------*/
/*Name: high_pre_timer_remain                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- ��ʱ��ʣ��ʱ��               */
/*retu: �ɹ�0��ʧ���򷵻�-1                           */
/*desc: ��ö�ʱ��ʣ��ʱ��                         */
/*-------------------------------------------------------*/
int high_pre_timer_remain(TIMERID lTimerId, int *plRet)
{
    int             lResult = -1;
    int             lTick   = 0;
    high_pre_timer *pTimer  = NULL;
    high_pre_timer *pNode   = NULL;

    pTimer = (high_pre_timer*)lTimerId;

    if((NULL == pTimer) || (NULL == plRet)) return(-1);
 
    *plRet = 0;
    
    sem_wait(&gTimerListHead.lSemVal);

    pNode = gTimerListHead.pTHead;

    while(pNode && (pNode != pTimer))
    {
        lTick += pNode->iTickGap;
        pNode  = pNode->pNext;
    }

    if(NULL != pNode)  
    {
        lResult = 0;
        
        lTick += pNode->iTickGap;
        
       *plRet  = lTick * gTimerListHead.iBasInt;
    }

    sem_post(&gTimerListHead.lSemVal);

    return(lResult);
}

/*====================================================================================================================*/
/*====================================================================================================================*/

/*-------------------------------------------------------*/
/*Name: high_pre_timer_call                              */
/*retu: NONE                                             */
/*desc: ������ʱ��������                               */
/*-------------------------------------------------------*/
#if 1
void high_pre_timer_call(void)
{
     ipc_send_msg_n0(&gBTimerMsgData);

   //printf("timer signal =%d cou=%d mycount=%d\r\n", iSigNo,  timercou++, myselectcount);
}
#endif

/*-------------------------------------------------------*/
/*Name: VOS_BaseTimerInit                                */
/*retu: NONE                                             */
/*desc: ������ʱ����ʼ��                                 */
/*-------------------------------------------------------*/
int high_pre_timer_start(void)
{     
	int              iResult = 0;    
#if 0   
    sigset_t         sNewSigSet;  
    struct sigaction sNewSa, sOldSa;  
    
    memset(&sNewSa, 0, sizeof(sNewSa));  
                                                 /* �̴߳�ALARM�ź�    */
    sigemptyset(&sNewSigSet);  
    sigaddset(&sNewSigSet, SIGALRM);  
    sigprocmask(SIG_UNBLOCK, &sNewSigSet, NULL);

                                                 /* ע��ALARM�¼������� */
    sNewSa.sa_sigaction = high_pre_timer_call; 
    sNewSa.sa_flags     = SA_RESTART;//SA_SIGINFO | SA_RESTART;  

    sigfillset(&sNewSa.sa_mask);      
  //sigaddset(&sNewSa.sa_mask, SIGALRM);  
    sigaction(SIGALRM, &sNewSa, &sOldSa);
    
 #endif   

    iResult = timer_create(CLOCK_MONOTONIC, NULL, &gBaseTimerID);/*create timer; CLOCK_REALTIME is real time of the system*/

    if(0 == iResult) 
    {
         int               iTimeval = 0;
         struct itimerspec sTimerVal={{0, 0},{0,0}};

         iTimeval = gTimerListHead.iBasInt;

         sTimerVal.it_interval.tv_sec  = iTimeval / 1000;

         sTimerVal.it_interval.tv_nsec = (iTimeval % 1000) * 1000000;

         sTimerVal.it_value.tv_sec     = iTimeval / 1000;;

         sTimerVal.it_value.tv_nsec    = (iTimeval % 1000) * 1000000;
         
         iResult = timer_settime(gBaseTimerID, 0, &sTimerVal, NULL);
    }
    return((iResult == 0) ? 0 : -1);
}

/*-------------------------------------------------------*/
/*Name: high_pre_timer_init                              */
/*Para: iBasTimer  ���������ж�ʱ�������Լ��           ,��λ    MS    */
/*Para: iModId     ��Ϣ����ģ���ID                             */
/*Desc: ��ʼ�����̵Ķ�ʱ��                                        */
/*-------------------------------------------------------*/

int high_pre_timer_init(int iBasTimer, int iModId)
{
    memset(&gTimerListHead, 0, sizeof(high_pre_timer_lst));

    memset(&gTempTimerList, 0, sizeof(high_pre_timer_lst));

    memset(&gBTimerMsgData, 0, sizeof(struct ipc_quemsg_n));
    
    
    gBTimerMsgData.recvmod = iModId;

    sem_init(&gTimerListHead.lSemVal, 0, 1);
    sem_init(&gTempTimerList.lSemVal, 0, 1);
    
    gTimerListHead.iModule = iModId;
    
    gTimerListHead.iBasInt = (iBasTimer > 0) ? iBasTimer : LIB_TIMER_MIN_INTERVAL; 

    return(0);
}

/*====================================================================================================================*/
/*��ʱ��CLI API�ӿ�*/
/*-------------------------------------------------------*/
/*Name: high_pre_timer_info                              */
/*retu: �ɹ�>1��ʧ���򷵻�-1                                     */
/*desc: ��ʾ��ʱ����Ϣ                                          */
/*-------------------------------------------------------*/
int high_pre_timer_info(char *pOutBuf, int pDatLen)
{
    high_pre_timer   *pNode   = NULL;
    int               ulLoop  = 0;
    int               ulTicks = 0;
    int               lPntLen = 0;
    char              bTypeBf[2][8] = {"noloop", "loop"};

    if((pOutBuf == NULL) || (pDatLen < 250)) return(-1);

    if(gTimerListHead.iCount <= 0)
    {
        return(sprintf(pOutBuf, "Have no timer in sytem!\r\n"));
    }

    if((gTimerListHead.iCount + 2) * 90 > pDatLen)  
    {
        return(sprintf(pOutBuf, "Buffer to small!\r\n"));
    }
    
    lPntLen += sprintf(pOutBuf + lPntLen, "No   Type   CallFunc   TimeLen  LeftTime TimerName\r\n");
    lPntLen += sprintf(pOutBuf + lPntLen, "---- ------ ---------- -------- -------- ----------------\r\n");

    sem_wait(&gTimerListHead.lSemVal);

    pNode = gTimerListHead.pTHead;

    while(pNode)
    {
        ulLoop  ++;
        ulTicks += pNode->iTickGap;
        
        lPntLen += sprintf(pOutBuf + lPntLen, "%4d %-6s 0x%08x %8d %8d %-16s\r\n",
                               ulLoop, bTypeBf[pNode->iLoop%2], (unsigned long)pNode->pFunc, pNode->iInterval, 
                               ulTicks * gTimerListHead.iBasInt, pNode->bName);
    
        pNode  = pNode->pNext;
    }

    sem_post(&gTimerListHead.lSemVal);
 	
    return(lPntLen);
}

/*====================================================================================================================*/
/*====================================================================================================================*/

#ifdef  __cplusplus
}
#endif
/*====================================================================================================================*/
/*====================================================================================================================*/

