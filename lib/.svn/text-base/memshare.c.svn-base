/*====================================================================================================================*/
/*====================================================================================================================*/
/*-------------------------------------------------------------*/
/*  FileName : memshare.c                                      */
/*  Author   : suxiqing                                        */
/*  EditTime : 2018-1-10                                       */
/*  CopyRight:                                                 */
/*  共享内存,用于进程间传递报文和块数据                                         */
/*  每次申请的内存大小不能超过16k 超过16K的数据APP自行分片                           */
/*-------------------------------------------------------------*/
/*====================================================================================================================*/
/*====================================================================================================================*/
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/log.h"
#include "module_id.h"
#include "memshare.h"
#include "msg_ipc_n.h"
/*====================================================================================================================*/
/*====================================================================================================================*/
/*  ------------------------------------ */
/*  |cache table          | 1k        |  */
/*  -----------------------------------  */
/*  |128   blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |256   blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |512   blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |1024  blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |2048  blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |4096  blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |8192  blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  |16384 blocks table   | Estimate  |  */
/*  -----------------------------------  */
/*  -----------------------------------  */
/*  |data  area           | Estimate  |  */
/*  -----------------------------------  */

/*====================================================================================================================*/
#define MEM_SHARE_PRINTF(fmt,args...)   printf(fmt, ##args)

#define MEM_SHARE_ACCESS_PERM           0666    //0755

#define MEM_SHARE_MEMORY_KEY            41      /*共享内存KEY*/

#define MEM_SHARE_SEMAPHORE_KEY         51      /*共享内存sem*/

#define MEM_SHARE_INVALID_IDX           -1


#define MEM_SHARE_CACHETBL_SIZE         1024  

#define MEM_SHARE_TBLALIGN_BYTES        1024  

#define MEM_SHARE_BLOCK_ALIGN(len, align)  (((len) + (align - 1)) & ~(align - 1))     


#define MEM_SHARE_MONI_ENABLE           0      //使用情况监控

#define MEM_SHARE_MONI_TIMOUT           30     //30秒超时 


#define MEM_SHARE_MARGIN_ENABLE         1      //越界检查

#define MEM_SHARE_MARGIN_DATA          (char)0x5A

/*====================================================================================================================*/
#define MEM_SHARE_CHCHE_POOL_MAX        9             

#define MEM_SHARE_BLOCK_COUNT_64        8192    //0.5M

#define MEM_SHARE_BLOCK_COUNT_128       8192    //1M

#define MEM_SHARE_BLOCK_COUNT_256       8192    //2M

#define MEM_SHARE_BLOCK_COUNT_512       4096    //2M 

#define MEM_SHARE_BLOCK_COUNT_1K        4096    //4M 

#define MEM_SHARE_BLOCK_COUNT_2K        5120    //10M  

#define MEM_SHARE_BLOCK_COUNT_4K        2048    //8M

#define MEM_SHARE_BLOCK_COUNT_8K         512    //8M

#define MEM_SHARE_BLOCK_COUNT_16K       1024    //16m

                                        //47301632BYTES

#define MEM_SHARE_BLOCK_SIZE_MAX        16384

/*====================================================================================================================*/

typedef struct
{
    int              iMemAdd;

    int              iNext;

    int              iModule;

    int              iRealSz;          /*越界检查*/
#if MEM_SHARE_MONI_ENABLE
    unsigned long    iTOutTk;          /*记录超时间*/
#endif
}mem_share_blkctl;

typedef struct
{
    int              iBlockSize;
    
    int              iBlockCoun;

    int              iStartAddr;     

    int              iAreaLen;     

    int              iEndAddr;     


    int              iCtlTblLen;        /*块控制表偏移*/

    int              iCtlTblAdd;        /*块控制表偏移*/

    int              iFirstFree;

    int              iBlockUsed;        /*已使用块数       */

    int              iBlockFree;        /*空闲块数        */
    
    int              iUsePeakVa;        /*使用峰值        */   

    int              iRelResult;           
}mem_share_chches;

union semun
{
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO(Linux-specific) */
};

#if MEM_SHARE_MONI_ENABLE
static int   iShareMemToTick =  0; 
static int   iShareMsPerTick =  0;
#endif
static int   iShareMemSemSId = -1;
static int   iShareMemHandle = -1;
static char *pShareMemMapAdd = NULL;    /*各个进程映射地址*/
static char *pShareMemMapEnd = NULL;


static mem_share_chches sMemShaCaches[MEM_SHARE_CHCHE_POOL_MAX] =
{
    {64,    MEM_SHARE_BLOCK_COUNT_64,  0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {128,   MEM_SHARE_BLOCK_COUNT_128, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {256,   MEM_SHARE_BLOCK_COUNT_256, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {512,   MEM_SHARE_BLOCK_COUNT_512, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {1024,  MEM_SHARE_BLOCK_COUNT_1K,  0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {2048,  MEM_SHARE_BLOCK_COUNT_2K,  0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {4096,  MEM_SHARE_BLOCK_COUNT_4K,  0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {8192,  MEM_SHARE_BLOCK_COUNT_8K,  0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
    {16384, MEM_SHARE_BLOCK_COUNT_16K, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0},
};

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_semcreate                                  */
/*Desc: 信号量创建                                                */
/*-----------------------------------------------------------*/
static int mem_share_semcreate(int iKey)
{
    int iSemId = -1;
                                                                   /*create semaphore*/
    iSemId = semget(iKey, 1, IPC_CREAT | MEM_SHARE_ACCESS_PERM);

    if(iSemId == -1)  return(-1);
    else
    {
        union semun sSemuVal = {0};
        
        sSemuVal.val = 1;
    
        semctl(iSemId, 0, SETVAL, sSemuVal);

        return(iSemId);
    }
}

/*-----------------------------------------------------------*/
/*Name: mem_share_semget                                     */
/*Desc: 已创建信号量获取                                             */
/*-----------------------------------------------------------*/
static int mem_share_semget(int iKey)
{
    int iSemId = -1;
                                                                  /*create semaphore*/
    iSemId = semget(iKey, 1, MEM_SHARE_ACCESS_PERM);

    if(iSemId == -1) 
         return(-1);
    else return(iSemId);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_semup                                      */
/*Desc: 信号量删除                                                */
/*-----------------------------------------------------------*/
static int mem_share_semdelete(int sem_id)
{
    union semun sSemuVal = {0};

    sSemuVal.val = 1;

    if(semctl(sem_id, 0, IPC_RMID, sSemuVal) == -1)
         return(-1);
    else return( 0);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_semup                                      */
/*Desc: 信号释放                                                 */
/*-----------------------------------------------------------*/
static int mem_share_semup(int sem_id)
{
    struct sembuf sSemBuf;

    sSemBuf.sem_num = 0;
    sSemBuf.sem_flg = SEM_UNDO;
    sSemBuf.sem_op  = 1;
    
    if(semop(sem_id, &sSemBuf, 1) == -1)
         return(-1);
    else return( 0);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_semup                                      */
/*Desc: 信号获取                                                 */
/*-----------------------------------------------------------*/
static int mem_share_semdown(int sem_id)
{
    struct sembuf sSemBuf;

    sSemBuf.sem_num =  0;
    sSemBuf.sem_flg =  SEM_UNDO;
    sSemBuf.sem_op  = -1;
    
    if(semop(sem_id, &sSemBuf, 1) == -1)
         return(-1);
    else return( 0);
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_estimate                                   */
/*Desc: 创建前评估                                                */
/*-----------------------------------------------------------*/
static int mem_share_estimate(mem_share_chches *pCheTbl)
{
    int iBlkLoop = 0;
    int iMemAlen = MEM_SHARE_CACHETBL_SIZE;

    mem_share_chches *pChche = NULL;
    
    iMemAlen = MEM_SHARE_BLOCK_ALIGN(iMemAlen, MEM_SHARE_TBLALIGN_BYTES);

    for(iBlkLoop = 0; iBlkLoop < MEM_SHARE_CHCHE_POOL_MAX; iBlkLoop++)
    {
         pChche = &pCheTbl[iBlkLoop];
         
         pChche->iCtlTblAdd = iMemAlen;
         
         pChche->iCtlTblLen = pChche->iBlockCoun * sizeof(mem_share_blkctl);

         pChche->iCtlTblLen = MEM_SHARE_BLOCK_ALIGN(pChche->iCtlTblLen, MEM_SHARE_TBLALIGN_BYTES);

         iMemAlen += pChche->iCtlTblLen;
    }


    for(iBlkLoop = 0; iBlkLoop < MEM_SHARE_CHCHE_POOL_MAX; iBlkLoop++)
    {
         pChche = &pCheTbl[iBlkLoop];
         
         pChche->iStartAddr = iMemAlen;
         
         pChche->iAreaLen = pChche->iBlockCoun * pChche->iBlockSize;

         pChche->iAreaLen = MEM_SHARE_BLOCK_ALIGN(pChche->iAreaLen, MEM_SHARE_TBLALIGN_BYTES);

         pChche->iEndAddr = pChche->iStartAddr + pChche->iAreaLen;

         iMemAlen += pChche->iAreaLen;
    }

 // MEM_SHARE_PRINTF("%s[%d], share memory estimate area size = %d\r\n", __FILE__, __LINE__, iMemAlen);

    return(iMemAlen);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_blkinit                                    */
/*Desc: 初始化内存块                                               */
/*-----------------------------------------------------------*/
static void mem_share_blkinit(mem_share_chches *pChche, char *pStart)
{
    int iBlkLoop = 0;

    mem_share_blkctl *pBlkCtrl = NULL;

    pBlkCtrl = (mem_share_blkctl*)(pStart + pChche->iCtlTblAdd);
    
    for(iBlkLoop = 0; iBlkLoop < pChche->iBlockCoun; iBlkLoop++, pBlkCtrl++)
    {
         pBlkCtrl->iRealSz = 0;

         pBlkCtrl->iNext   = (iBlkLoop + 1);

         pBlkCtrl->iMemAdd = pChche->iStartAddr + iBlkLoop * pChche->iBlockSize;
    }

    pBlkCtrl --;
    
    pBlkCtrl->iNext = MEM_SHARE_INVALID_IDX;

    pChche->iFirstFree = 0;
    
    pChche->iBlockFree = pChche->iBlockCoun;
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_init                                       */
/*Desc: 平台共享内存初始化,创建  平台第一个进程调用                              */
/*-----------------------------------------------------------*/
int mem_share_init(void)
{
    int iBlkLoop = 0;
    int iMemSize = 0;

    system("sysctl -w kernel.shmmax=67108864");
        
#if MEM_SHARE_MONI_ENABLE
    iShareMsPerTick = 1000 / sysconf(_SC_CLK_TCK);
    iShareMemToTick = MEM_SHARE_MONI_TIMOUT * sysconf(_SC_CLK_TCK);
#endif
                                                                   /*create share semaphore first*/
    iShareMemSemSId = mem_share_semcreate(MEM_SHARE_SEMAPHORE_KEY);

    if(iShareMemSemSId == -1)
    {
       //zlog_err("%s[%d] create share semaphore error.\r\n", __FILE__, __LINE__);
         
         MEM_SHARE_PRINTF("%s[%d] create share semaphore error.\r\n", __FILE__, __LINE__);
    
         return(-1);
    }
                                                                   /*create share memory second*/
    iMemSize = mem_share_estimate(sMemShaCaches);

    iShareMemHandle = shmget(MEM_SHARE_MEMORY_KEY, (unsigned int)iMemSize, IPC_CREAT | MEM_SHARE_ACCESS_PERM);

    if(iShareMemHandle == -1)
    {
       //zlog_err("%s[%d] create share memory error.\r\n", __FILE__, __LINE__);
         
         MEM_SHARE_PRINTF("%s[%d] create share memory error.\r\n", __FILE__, __LINE__);

         return(-1);
    }

    pShareMemMapAdd = shmat(iShareMemHandle, NULL, 0);

    if(pShareMemMapAdd == NULL)
    {
       //zlog_err("%s[%d] create share memory size = %d, map address error.\r\n", __FILE__, __LINE__, iMemSize);
         
         MEM_SHARE_PRINTF("%s[%d] create share memory size = %d, map address error.\r\n", __FILE__, __LINE__, iMemSize);
    
         return(-1);
    }

    memset(pShareMemMapAdd, 0, iMemSize);     

    for(iBlkLoop = 0; iBlkLoop < MEM_SHARE_CHCHE_POOL_MAX; iBlkLoop++)             
    {
        mem_share_blkinit(&sMemShaCaches[iBlkLoop], pShareMemMapAdd);
    }                                                               /*直接写入共享内存*/

    pShareMemMapEnd = pShareMemMapAdd + iMemSize;

    mem_share_semdown(iShareMemSemSId);
    
    memcpy(pShareMemMapAdd, sMemShaCaches, sizeof(mem_share_chches) * MEM_SHARE_CHCHE_POOL_MAX);
                                                                   /*write cache table to share memory*/
    mem_share_semup(iShareMemSemSId);

    return(0);                                                                   
}

/*-----------------------------------------------------------*/
/*Name: mem_share_uninit                                     */
/*Desc: 平台共享内存初始化,创建  平台第一个进程调用                              */
/*-----------------------------------------------------------*/
int mem_share_uninit(void)
{
    if(shmctl(iShareMemHandle, IPC_RMID, 0) == -1)
    {
       //zlog_err("%s[%d] destory share memory error.\r\n", __FILE__, __LINE__);

         MEM_SHARE_PRINTF("%s[%d] destory share memory error.\r\n", __FILE__, __LINE__);
    }

    if(mem_share_semdelete(iShareMemSemSId) == -1)
    {
     // zlog_err("%s[%d] destory share semaphore error.\r\n", __FILE__, __LINE__);
        
        MEM_SHARE_PRINTF("%s[%d] destory share semaphore error.\r\n", __FILE__, __LINE__);
    }

    return(0);
}

/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_attach                                     */
/*Desc: 捆绑共享内存                                               */
/*-----------------------------------------------------------*/
int mem_share_attach(void)
{
    int iMemSize = 0;

#if MEM_SHARE_MONI_ENABLE
    iShareMsPerTick = 1000 / sysconf(_SC_CLK_TCK);
    iShareMemToTick = MEM_SHARE_MONI_TIMOUT * sysconf(_SC_CLK_TCK);
#endif
                                                                   /*create share semaphore first*/
    iShareMemSemSId = mem_share_semget(MEM_SHARE_SEMAPHORE_KEY);

    if(iShareMemSemSId == -1)
    {
       //zlog_err("%s[%d]  attach share semaphore error.\r\n", __FILE__, __LINE__);

         MEM_SHARE_PRINTF("%s[%d]  attach share semaphore error.\r\n", __FILE__, __LINE__);
    
         return(-1);
    }

    iMemSize = mem_share_estimate(sMemShaCaches);
                                                                   /*create share memory second*/
    iShareMemHandle = shmget(MEM_SHARE_MEMORY_KEY, (unsigned int)iMemSize,  MEM_SHARE_ACCESS_PERM);

    if(iShareMemHandle == -1)
    {
       //zlog_err("%s[%d]  attach share memory error.\r\n", __FILE__, __LINE__);

         MEM_SHARE_PRINTF("%s[%d] attach share memory error.\r\n", __FILE__, __LINE__);

         return(-1);
    }

    pShareMemMapAdd = shmat(iShareMemHandle, NULL, 0);

    if(pShareMemMapAdd == NULL)
    {
       //zlog_err("%s[%d] attach share memory size = %d, map address error.\r\n", __FILE__, __LINE__, iMemSize);

         MEM_SHARE_PRINTF("%s[%d] attach share memory size = %d, map address error.\r\n", __FILE__, __LINE__, iMemSize);
    
         return(-1);
    }
    
    pShareMemMapEnd = pShareMemMapAdd + iMemSize;

    return(0);                                                                   
}

/*-----------------------------------------------------------*/
/*Name: mem_share_detach                                     */
/*Desc: 平台共享内存初始化,创建  平台第一个进程调用                              */
/*-----------------------------------------------------------*/
int mem_share_detach(void)
{
    if(pShareMemMapAdd != NULL) 
    {
        if(shmdt(pShareMemMapAdd) == -1)
        {
          //zlog_err("%s[%d] detach share memory error.\r\n", __FILE__, __LINE__);

            MEM_SHARE_PRINTF("%s[%d] detach share memory error.\r\n", __FILE__, __LINE__);
        }
    }

    return(0);
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_addrcheck                                  */
/*Retu: int 1 0                                              */
/*Desc: 检查共享内存释放有效                                           */
/*-----------------------------------------------------------*/
int mem_share_addrcheck(char* pMemAdd)
{
    if((pMemAdd > pShareMemMapAdd) &&
       (pMemAdd < pShareMemMapEnd))
         return(1);
    else return(0);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_malloc                                     */
/*Retu: void*                                                */
/*Desc: 申请一块共享内存                                             */
/*-----------------------------------------------------------*/
void* mem_share_malloc(int iSize, int iModule)
{
    int               iCheIdx = 0;
    int               iAlSize = iSize;
    char             *pMemBuf = NULL;
    mem_share_chches *pCheTbl = NULL;
    mem_share_chches *pCheFnd = NULL;
    mem_share_chches *pCheSta = NULL;
    mem_share_blkctl *pObject = NULL;

#if MEM_SHARE_MARGIN_ENABLE
    iAlSize += 4;
#endif
    
    if((pShareMemMapAdd == NULL) || 
       (iModule <= 0) || (iModule >= MODULE_ID_MAXNUM) ||
       (iAlSize <= 0) || (iAlSize > MEM_SHARE_BLOCK_SIZE_MAX)) 
    {
       //zlog_err("%s[%d] malloc share mem error, size = %d too larg, or module = %d error\r\n", 
       //        __FILE__, __LINE__, iSize, iModule);
        
        MEM_SHARE_PRINTF("%s[%d] malloc share mem error, size = %d too large, or module = %d error\r\n", 
                         __FILE__, __LINE__, iSize, iModule);

        return((void*)pMemBuf);
    }

    mem_share_semdown(iShareMemSemSId);

    pCheTbl = (mem_share_chches*)pShareMemMapAdd;

    pCheSta = pCheTbl + MEM_SHARE_CHCHE_POOL_MAX;
    
    for(iCheIdx = 0; iCheIdx < MEM_SHARE_CHCHE_POOL_MAX; iCheIdx++, pCheTbl++)
    {
        if((iAlSize <= pCheTbl->iBlockSize) && (pCheTbl->iBlockFree > 0))
        {
             pCheFnd = pCheTbl; break;
        }
    }

    if(pCheFnd != NULL)
    {
         if(pCheFnd->iFirstFree >= 0)
         {
             pObject  = (mem_share_blkctl*)(pShareMemMapAdd + pCheFnd->iCtlTblAdd);

             pObject += pCheFnd->iFirstFree;
              

             pObject->iRealSz = iSize;
             pObject->iModule = iModule;

             pMemBuf = pShareMemMapAdd + pObject->iMemAdd;
             
             pCheFnd->iFirstFree = pObject->iNext;
             
             pCheFnd->iBlockUsed ++;
             pCheFnd->iBlockFree --;

             if(pCheFnd->iBlockUsed > pCheFnd->iUsePeakVa) pCheFnd->iUsePeakVa = pCheFnd->iBlockUsed;
             
             if((pCheFnd->iBlockFree <= 0) || (pCheFnd->iFirstFree < 0))
             {
                 pCheFnd->iBlockFree = 0;
                 pCheFnd->iFirstFree = MEM_SHARE_INVALID_IDX;
             } 
                           
             pObject->iNext =  MEM_SHARE_INVALID_IDX;
#if MEM_SHARE_MONI_ENABLE
             pObject->iTOutTk = times(NULL); //(unsigned long)iShareMemToTick + times(NULL);
#endif
             pCheSta->iBlockUsed ++;
             pCheSta->iBlockFree --;

             if(pCheSta->iBlockUsed > pCheSta->iUsePeakVa) pCheSta->iUsePeakVa = pCheSta->iBlockUsed;
 
#if MEM_SHARE_MARGIN_ENABLE
             pMemBuf[iSize + 0] = MEM_SHARE_MARGIN_DATA;  
             pMemBuf[iSize + 1] = MEM_SHARE_MARGIN_DATA;  
             pMemBuf[iSize + 2] = MEM_SHARE_MARGIN_DATA;  
             pMemBuf[iSize + 3] = MEM_SHARE_MARGIN_DATA; 
#endif
         }
         else
         {                                                    /*内存访问严重错误*/
             //zlog_err("%s[%d] malloc share mem error, free index = %d error, module = %d size = %d\r\n", 
             //         __FILE__, __LINE__, pCheFnd->iFirstFree, iModule, iSize);
 
              MEM_SHARE_PRINTF("%s[%d] malloc share mem error, free index = %d error, module = %d size = %d\r\n", 
                               __FILE__, __LINE__, pCheFnd->iFirstFree, iModule, iSize);
         }
    }
    else
    {
         //zlog_err("%s[%d] malloc share mem error, no any free memory block, module = %d, size=%d\r\n", 
         //         __FILE__, __LINE__, iModule, iSize);

         MEM_SHARE_PRINTF("%s[%d] malloc share mem error, no any free memory block, module = %d, size=%d\r\n", 
                          __FILE__, __LINE__, iModule, iSize);
    }

    mem_share_semup(iShareMemSemSId);
    
    return((void*)pMemBuf);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_free                                       */
/*Retu: void                                                 */
/*Desc: 释放一块共享内存                                             */
/*-----------------------------------------------------------*/
void mem_share_free(void *pBlock, int iModule)
{
    int               iCheIdx = 0;
    int               iOffset = 0;
    mem_share_chches *pCheTbl = NULL;
    mem_share_chches *pCheFnd = NULL;
    mem_share_chches *pCheSta = NULL;
    mem_share_blkctl *pObject = NULL;
    char             *pFreeBf = pBlock;
    
    if(!mem_share_addrcheck(pFreeBf))
    {
       //zlog_err("%s[%d] free share mem error, no share memory address = %p\r\n", __FILE__, __LINE__, pBlock);

        MEM_SHARE_PRINTF("%s[%d] free share mem error, no share memory address = %p\r\n", __FILE__, __LINE__, pBlock);
        return;
    }

    iOffset = (int)(pFreeBf - pShareMemMapAdd);

    mem_share_semdown(iShareMemSemSId);
    
    pCheTbl = (mem_share_chches*)pShareMemMapAdd;

    pCheSta = pCheTbl + MEM_SHARE_CHCHE_POOL_MAX;
    
    for(iCheIdx = 0; iCheIdx < MEM_SHARE_CHCHE_POOL_MAX; iCheIdx++, pCheTbl++)
    {
        if((iOffset >= pCheTbl->iStartAddr) && (iOffset < pCheTbl->iEndAddr))
        {
            pCheFnd = pCheTbl; break;
        }
    }

    if(pCheFnd != NULL)
    {
         iCheIdx  = iOffset - pCheFnd->iStartAddr;

         iCheIdx  = iCheIdx / pCheFnd->iBlockSize;

         pObject  = (mem_share_blkctl*)(pShareMemMapAdd + pCheFnd->iCtlTblAdd);

         pObject += iCheIdx;

         if(pObject->iMemAdd == iOffset)
         {
             if(pObject->iModule > 0)
             {
#if MEM_SHARE_MARGIN_ENABLE
                  if((pFreeBf[pObject->iRealSz + 0] != MEM_SHARE_MARGIN_DATA) ||  
                     (pFreeBf[pObject->iRealSz + 1] != MEM_SHARE_MARGIN_DATA) || 
                     (pFreeBf[pObject->iRealSz + 2] != MEM_SHARE_MARGIN_DATA) || 
                     (pFreeBf[pObject->iRealSz + 3] != MEM_SHARE_MARGIN_DATA))
                  {
                     //zlog_err("%s[%d] free share mem error %p!, margin overflow! size=%d, module:malloc=%d,free=%d!\r\n", 
                     //           __FILE__, __LINE__, pBlock, pObject->iRealSz, pObject->iModule, iModule);
                       struct ipc_msghdr_n *pmsghd = (struct ipc_msghdr_n *)pBlock;
                     
                       MEM_SHARE_PRINTF("%s[%d] free share mem error %p!, margin overflow! size=%d, module:malloc=%d,free=%d! msgtype=%u, subtype=%u\r\n", 
                                        __FILE__, __LINE__, pBlock, pObject->iRealSz, pObject->iModule, iModule, pmsghd->msg_type, pmsghd->msg_subtype);
                  }
#endif
                  pObject->iRealSz = 0;
                  
                  pObject->iModule = 0;
                  pObject->iNext   = pCheFnd->iFirstFree;
#if MEM_SHARE_MONI_ENABLE
                  pObject->iTOutTk = 0;
#endif
                  pCheFnd->iBlockUsed --;
                  pCheFnd->iBlockFree ++;
                  pCheFnd->iFirstFree = iCheIdx;
                  
                  pCheSta->iBlockUsed --;
                  pCheSta->iBlockFree ++;
             }
             else
             {
                  //zlog_err("%s[%d] free share mem error %p!, addr free repeat!\r\n", __FILE__, __LINE__, pBlock);
                
                  MEM_SHARE_PRINTF("%s[%d] free share mem error %p!, addr free repeat!\r\n", __FILE__, __LINE__, pBlock);
             }
         }
         else
         {                                                    /*内存访问严重错误*/
              //zlog_err("%s[%d] free share mem error %p!, addr match error!\r\n", __FILE__, __LINE__, pBlock);
              
              MEM_SHARE_PRINTF("%s[%d] free share mem error %p!, addr match error!\r\n", __FILE__, __LINE__, pBlock);
         }
    }
    else
    {
         //zlog_err("%s[%d] free share mem error, chche find error, block = %p\r\n", __FILE__, __LINE__, pBlock);

         MEM_SHARE_PRINTF("%s[%d] free share mem error, chche find error, block = %p\r\n", __FILE__, __LINE__, pBlock);
    }

    mem_share_semup(iShareMemSemSId);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_free_bydata                                */
/*Retu: void                                                 */
/*Desc: 通过消息的数据体 释放一块共享内存                                    */
/*-----------------------------------------------------------*/
void mem_share_free_bydata(void *pBlock, int iModule)
{
    if(pBlock != NULL)
    {
        mem_share_free((void*)((char*)pBlock - IPC_HEADER_LEN_N), iModule);
    }
}

/*-----------------------------------------------------------*/
/*Name: mem_share_addrmap                                    */
/*Retu: void*                                                */
/*Desc: 偏移映射为地址                                              */
/*-----------------------------------------------------------*/
void* mem_share_addrmap(void *pOffset)
{
     unsigned int iOffset = (unsigned int)pOffset;
     
     if(iOffset > 0)
          return((void*)(pShareMemMapAdd + iOffset));
     else return((void*)NULL);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_addrunmap                                  */
/*Retu: int                                                  */
/*Desc: 地址映射为偏移                                              */
/*-----------------------------------------------------------*/
void* mem_share_addrunmap(void *pBlkAddr)
{
     if(pBlkAddr != NULL) // && mem_share_addrcheck((char*)pBlkAddr))
     {
          return((void*)((char*)pBlkAddr - pShareMemMapAdd));
     }
     else return(NULL);
}


/*-----------------------------------------------------------*/
/*Name: mem_share_timeoutfree                                */
/*Retu: 返回释放内存块数                                             */
/*-----------------------------------------------------------*/
int mem_share_timeoutfree(unsigned long iTickOffset)
{
#if MEM_SHARE_MONI_ENABLE
    int               iCheId  = 0;
    int               iBlkId  = 0 ;
    int               iFreeC  = 0;   
    unsigned long     iTicks  = 0;
    mem_share_chches *pCheTbl = NULL;
    mem_share_chches *pCheSta = NULL;
    mem_share_blkctl *pBlkCtl = NULL;    
  
    pCheTbl = (mem_share_chches*)pShareMemMapAdd;

    pCheSta = pCheTbl + MEM_SHARE_CHCHE_POOL_MAX;
    
    mem_share_semdown(iShareMemSemSId);

    iTicks = times(NULL);
    
    for(iCheId = 0; iCheId < MEM_SHARE_CHCHE_POOL_MAX; iCheId++, pCheTbl++)
    {
         pBlkCtl = (mem_share_blkctl*)(pShareMemMapAdd + pCheTbl->iCtlTblAdd);

         for(iBlkId = 0; iBlkId < pCheTbl->iBlockCoun; iBlkId ++, pBlkCtl++)
         {
              if((pBlkCtl->iModule > 0) && (pBlkCtl->iModule < MODULE_ID_MAXNUM) &&
                 ((pBlkCtl->iTOutTk + iTickOffset) < iTicks))
              {
                   iFreeC++;
                   
                   pBlkCtl->iModule = 0;
                   pBlkCtl->iRealSz = 0;
                   pBlkCtl->iTOutTk = 0;
                   pBlkCtl->iNext   = pCheTbl->iFirstFree;

                   pCheTbl->iFirstFree = iBlkId;
                   pCheTbl->iBlockFree ++;
                   pCheTbl->iBlockUsed --;
                   
                   pCheSta->iBlockFree ++;              
                   pCheSta->iBlockUsed --;
                   pCheSta->iRelResult ++;
              }
         }
    }

    mem_share_semup(iShareMemSemSId);

    //zlog_err("%s[%d] force free share mem  for timeout, count = %d\r\n", __FILE__, __LINE__, iFreeC);
    
    MEM_SHARE_PRINTF("%s[%d] force free share mem  for timeout, count = %d\r\n", __FILE__, __LINE__, iFreeC);
    
    return(iFreeC);

#else
    return(0);
#endif
}
/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_share_cacheinfor                                 */
/*Para: iBufLen >= 1024                                      */
/*Retu: -1 error > 0 datalen                                 */
/*Desc: 显示共享内存的状态信息                                          */
/*-----------------------------------------------------------*/
int mem_share_cacheinfor(char *pBuffer, int iBufLen)
{
    int               iIndex  = 1;
    int               iAllLen =-1;
    int               iAllcou = 0;
    int               iAllFre = 0;
    mem_share_chches *pCheTbl = NULL;
    
    if((pBuffer != NULL) && (iBufLen >= 1024))
    {
         iAllLen = 0;

         pCheTbl = (mem_share_chches*)pShareMemMapAdd;
 
         iAllLen += sprintf(pBuffer + iAllLen, "BlkSz BlkCo  StartPos  AreaLen    EndPos CtlTblPos  CtlLen FreeCo FsFidx UsePeak\r\n");
         iAllLen += sprintf(pBuffer + iAllLen, "----- ----- --------- -------- --------- --------- ------- ------ ------ -------\r\n");
 
         mem_share_semdown(iShareMemSemSId);
         
         for(iIndex = 0; iIndex < MEM_SHARE_CHCHE_POOL_MAX; iIndex++, pCheTbl++)
         {
              iAllcou += pCheTbl->iBlockCoun;
              
              iAllFre += pCheTbl->iBlockFree;
              
              iAllLen += sprintf(pBuffer + iAllLen, "%5d %5d %9d %8d %9d %9d %7d %6d %6d %6d\r\n", 
                                 pCheTbl->iBlockSize, pCheTbl->iBlockCoun, pCheTbl->iStartAddr,
                                 pCheTbl->iAreaLen,   pCheTbl->iEndAddr,   pCheTbl->iCtlTblAdd, 
                                 pCheTbl->iCtlTblLen, pCheTbl->iBlockFree, pCheTbl->iFirstFree, pCheTbl->iUsePeakVa);
         }
 
         mem_share_semup(iShareMemSemSId);
         
         iAllLen += sprintf(pBuffer + iAllLen, "----- ----- --------- -------- --------- --------- ------- ------ ------ -------\r\n");

         iAllLen += sprintf(pBuffer + iAllLen, "ShareMemStart=%p\r\nAllBlockCount=%d, AllFreeCount=%d, AllPeakUsed=%d, AllForceFree=%d\r\n", 
                                                pShareMemMapAdd, iAllcou, iAllFre, pCheTbl->iUsePeakVa, pCheTbl->iRelResult);

    }
    
    return(iAllLen);
}

/*-----------------------------------------------------------*/
/*Name: mem_share_moduleinfor                                */
/*Para: iBufLen > 8192                                       */
/*Retu: -1 error > 0 datalen                                 */
/*Desc: 显示共享内存模块使用信息                                         */
/*-----------------------------------------------------------*/
int mem_share_moduleinfor(char *pBuffer, int iBufLen)
{
    int               iCheId  =  0;
    int               iBlkId  =  0;
    int               iAllLen = -1;   
    mem_share_chches *pCheTbl = NULL;
    mem_share_blkctl *pBlkCtl = NULL;    
    int               iSumCou[MODULE_ID_MAXNUM][MEM_SHARE_CHCHE_POOL_MAX+1] = {{0}};

    if((pBuffer != NULL) && (iBufLen >= MODULE_ID_MAXNUM * 128))
    {
         memset(iSumCou, 0, sizeof(int) * (MEM_SHARE_CHCHE_POOL_MAX +1) * MODULE_ID_MAXNUM);

         pCheTbl = (mem_share_chches*)pShareMemMapAdd;
 
         mem_share_semdown(iShareMemSemSId);
         
         for(iCheId = 0; iCheId < MEM_SHARE_CHCHE_POOL_MAX; iCheId++, pCheTbl++)
         {
              pBlkCtl = (mem_share_blkctl*)(pShareMemMapAdd + pCheTbl->iCtlTblAdd);

              for(iBlkId = 0; iBlkId < pCheTbl->iBlockCoun; iBlkId ++, pBlkCtl++)
              {
                   if((pBlkCtl->iModule > 0) && (pBlkCtl->iModule < MODULE_ID_MAXNUM))
                   {
                        iSumCou[0][iCheId] ++;
                        iSumCou[0][MEM_SHARE_CHCHE_POOL_MAX] ++;
                        iSumCou[pBlkCtl->iModule][iCheId] ++;
                        iSumCou[pBlkCtl->iModule][MEM_SHARE_CHCHE_POOL_MAX] ++;
                   }
              }
         }

         mem_share_semup(iShareMemSemSId);

         iAllLen = 0;
         
         iAllLen += sprintf(pBuffer + iAllLen, "Module  64blk 128Blk 256Blk 512Blk 1KBlks 2KBlks 4KBlks 8KBlks 16KBlk AllBlks\r\n");
         iAllLen += sprintf(pBuffer + iAllLen, "------ ------ ------ ------ ------ ------ ------ ------ ------ ------ -------\r\n");

         for(iCheId = 1; iCheId < MODULE_ID_MAXNUM; iCheId++)
         {
              iAllLen += sprintf(pBuffer + iAllLen, "%6d %6d %6d %6d %6d %6d %6d %6d %6d %6d %7d\r\n", iCheId, 
                                 iSumCou[iCheId][0], iSumCou[iCheId][1], iSumCou[iCheId][2],
                                 iSumCou[iCheId][3], iSumCou[iCheId][4], iSumCou[iCheId][5],
                                 iSumCou[iCheId][6], iSumCou[iCheId][7], iSumCou[iCheId][8], iSumCou[iCheId][9]);
         }
         iAllLen += sprintf(pBuffer + iAllLen, "------ ------ ------ ------ ------ ------ ------ ------ ------ ------- -------\r\n");

         iAllLen += sprintf(pBuffer + iAllLen, "Total  %6d %6d %6d %6d %6d %6d %6d %6d %6d %7d\r\n", 
                                               iSumCou[0][0], iSumCou[0][1], iSumCou[0][2], iSumCou[0][3], 
                                               iSumCou[0][4], iSumCou[0][5], iSumCou[0][6], iSumCou[0][7], iSumCou[0][8], iSumCou[0][9]);
   }
    
    return(iAllLen);
}


/*========================================================================================================================*/
/*========================================================================================================================*/

/*-----------------------------------------------------------*/
/*Name: mem_share_usedblkinfor                               */
/*Para: iBufLen > 128K                                       */
/*Retu: -1 error > 0 datalen                                 */
/*Desc: 显示共享内存已分配块信息                                         */
/*-----------------------------------------------------------*/
int mem_share_usedblkinfor(char *pBuffer, int iBufLen, int iPoolId, int iBlkcou)
{
    int                iBlkId  =  1;
    int                iAllLn  = -1;   
    int                iPrtBlk =  0;
    int                iTimes  =  0;
    unsigned long      iTicks  =  0;
    mem_share_chches  *pCheTbl = NULL;
    mem_share_blkctl  *pBlkCtl = NULL;    
    struct ipc_msghdr_n *pMsgHed = NULL;
    
    if((pBuffer != NULL) && (iBufLen >= 1024) && 
       (iPoolId >= 0) && (iPoolId < MEM_SHARE_CHCHE_POOL_MAX))
    {
         pCheTbl  = (mem_share_chches*)pShareMemMapAdd;

         pCheTbl += iPoolId;
         
         iAllLn   = 0;
         
         iAllLn  += sprintf(pBuffer + iAllLn, "Module   Size Time(ms)       addr RcvMod SndMod MTyp STyp OpC datlen\r\n");
         iAllLn  += sprintf(pBuffer + iAllLn, "------ ------ -------- ---------- ------ ------ ---- ---- --- ------\r\n");
         
         iTicks  = times(NULL);

         mem_share_semdown(iShareMemSemSId);
         
         pBlkCtl = (mem_share_blkctl*)(pShareMemMapAdd + pCheTbl->iCtlTblAdd);

         for(iBlkId = 0; iBlkId < pCheTbl->iBlockCoun; iBlkId ++, pBlkCtl++)
         {
             if((pBlkCtl->iModule > 0) && (pBlkCtl->iModule < MODULE_ID_MAXNUM))
             {
                  iPrtBlk ++;
                  pMsgHed = (struct ipc_msghdr_n*)(pShareMemMapAdd + pBlkCtl->iMemAdd);

#if MEM_SHARE_MONI_ENABLE
                  iTimes  = iShareMsPerTick * (int)(iTicks - pBlkCtl->iTOutTk);
#endif             
                  iAllLn += sprintf(pBuffer + iAllLn, "%4d   %6d %8d 0x%08x %6d %6d %4d %4d %3d %6d\r\n", pBlkCtl->iModule, 
                                                        pBlkCtl->iRealSz, iTimes, pMsgHed, pMsgHed->module_id, pMsgHed->sender_id, 
                                                        (int)pMsgHed->msg_type, (int)pMsgHed->msg_subtype, (int)pMsgHed->opcode,
                                                        (int)pMsgHed->data_len);
              }

              if(((iBufLen - iAllLn) < 256) || (iPrtBlk >= iBlkcou))
              {
                  iAllLn += sprintf(pBuffer + iAllLn, "----Not all memory block infor\r\n"); 
                  break;
              }
         }
 
         mem_share_semup(iShareMemSemSId);
   }
    
    return(iAllLn);
}

/*========================================================================================================================*/
/*========================================================================================================================*/

