/*====================================================================================================================*/
/*-------------------------------------------------------------*/
/*  FileName : mempool.c                                       */
/*  Author   : suxiqing                                        */
/*  EditTime : 2011-10-25                                      */
/*  CopyRight:                                                 */
/*  该内存池可管理频繁使用等尺寸内存块                                          */
/*  如果使用数据快较多,尽量使用大桶,               尽量使用较少的桶数                  */
/*  建议(小于20个)     , 桶太多释放慢                                     */
/*-------------------------------------------------------------*/
/*====================================================================================================================*/
/*====================================================================================================================*/

#include <stdio.h>
#include <string.h>
#include <semaphore.h>


#include "lib/memory.h"

#include "lib/linklist.h"

#include "lib/log.h"

#include "lib/mempool.h"

/*====================================================================================================================*/
/*====================================================================================================================*/
#define MEM_POOL_PRINTF(fmt,args...) printf(fmt, ##args)

#define MEM_POOL_INVALID_BLKIDX   -1

#define MEM_POOL_ALIGN_BYTES       8

#define MEM_POOL_BLOCK_ALIGN(len)  (((len) + (MEM_POOL_ALIGN_BYTES - 1)) & ~(MEM_POOL_ALIGN_BYTES - 1)) 

typedef struct
{
    int             iUseFlg;
 
    int             iNext;

    char           *pObjMem;
}mem_pool_object;

typedef struct
{
    struct listnode sNode;

    char           *pStartAddr;       /*桶的原始申请地址*/
    
    char           *pEndAddr;

    int             iFreeResu;       /*最近释放结果*/

    int             iBlksCount;      /*桶中总的块数*/
    
    int             iFreeCount;      /*桶中空闲的块数*/
    
    int             iFirstFree;      /*第一个空闲快    */

    mem_pool_object sObjTable[1];    /*桶中所有快控制表*/
    
}mem_pool_bucket;

typedef struct
{
    int             iBktFreeEna;     /*空闲桶释放开关*/
                                     /*通常不要对空闲桶做释放处理,因为效率太低*/
    int             iBucketMax;      /*池子中最大允许桶数*/
                                     
    int             iBucketUsed;     /*当前已使用的桶数*/
    
    int             iBucketRSize;    /*每个桶的大小*/
#if 0    
    int             iBucketASize;    /*每个桶的申请内存大小*/
#endif    
    int             iBucketBlks;     /*每个桶中的块数*/
    

    int             iBlockRSize;     /*内存块的原始创建尺寸*/
    
    int             iBlockASize;     /*内存块的对齐后尺寸*/

    int             iBlockAllCo;     /*池子中已分配的总的块数*/

    int             iBlockFreeC;     /*池子中未使用的总块数*/

    sem_t           sMemSemVal;
    
    struct list     sBktFreeLst;     /*有空闲对象的桶列表           */
    
    struct list     sBktBusyLst;     /*没有空闲对象的桶列表*/
    
}mem_pool_ctrl;

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_pool_create                                      */
/*Desc: 创建专用内存池                                              */
/*-----------------------------------------------------------*/

mempool_id mem_pool_create(int iBucketCo, int iBucketSz, int iBlockSz, int iFreeEna)
{
    int   iBlkNSize = iBlockSz;

    mem_pool_ctrl *pMemPool = NULL;
  
    if(iBlkNSize < (int)sizeof(struct listnode))
    {
        iBlkNSize = (int)sizeof(struct listnode);
    }
    
    iBlkNSize = MEM_POOL_BLOCK_ALIGN(iBlkNSize);

    if((iBlkNSize < iBucketSz) && 
       (iBlockSz  >= MEM_POOL_BLOCK_SIZE_MIN ) &&
       (iBucketCo >= MEM_POOL_BUCKET_COUN_MIN) &&
       (iBucketSz >= MEM_POOL_BUCKET_SIZE_MIN))
    {
        pMemPool = XMALLOC(MTYPE_TMP, sizeof(mem_pool_ctrl));

        if(pMemPool != NULL) 
        {
            memset(pMemPool, 0, sizeof(mem_pool_ctrl));

            pMemPool->iBktFreeEna  = iFreeEna;
            
            pMemPool->iBucketMax   = iBucketCo;
           
            pMemPool->iBucketRSize = MEM_POOL_BLOCK_ALIGN(iBucketSz);

            pMemPool->iBlockRSize  = iBlockSz;

            pMemPool->iBlockASize  = iBlkNSize;

            pMemPool->iBucketBlks  = pMemPool->iBucketRSize / iBlkNSize;
            
            if(sem_init(&pMemPool->sMemSemVal, 0, 1) != 0)
            {
                 XFREE(MTYPE_TMP, pMemPool);
                 pMemPool = NULL;
                 
                 MEM_POOL_PRINTF("mem_pool_create sem init error! \r\n");
            }
        }
    }
    else
    {
         zlog_err("%s[%d] parameters error iBucketCo=%d, iBucketSz=%d, iBlockSz=%d\r\n", __FILE__, __LINE__, iBucketCo,iBucketSz,iBlockSz);
    
         MEM_POOL_PRINTF("%s[%d] parameters error iBucketCo=%d, iBucketSz=%d, iBlockSz=%d\r\n", __FILE__, __LINE__, iBucketCo,iBucketSz,iBlockSz);
    }

    return((mempool_id)pMemPool);
}

/*-----------------------------------------------------------*/
/*Name: mem_pool_destroy                                     */
/*Retu: void                                                 */
/*Desc: 销毁专用内存池                                              */
/*-----------------------------------------------------------*/
void mem_pool_destroy(mempool_id lMempid)
{
     mem_pool_ctrl *pMemPool = (mem_pool_ctrl*)lMempid;

     if(pMemPool != NULL)  
     {
         mem_pool_bucket *pBucket = NULL;
         mem_pool_bucket *pNexNod = NULL;
         
         sem_wait(&pMemPool->sMemSemVal);

         pBucket = (mem_pool_bucket*)pMemPool->sBktBusyLst.head;

         while(pBucket)
         {
              pNexNod = (mem_pool_bucket*)pBucket->sNode.next;
              
              if(pBucket->pStartAddr) XFREE(MTYPE_TMP, pBucket->pStartAddr);

              XFREE(MTYPE_TMP, pBucket);

              pBucket = pNexNod;
         }

         pBucket = (mem_pool_bucket*)pMemPool->sBktFreeLst.head;

         while(pBucket)
         {
              pNexNod = (mem_pool_bucket*)pBucket->sNode.next;
              
              if(pBucket->pStartAddr) XFREE(MTYPE_TMP, pBucket->pStartAddr);

              XFREE(MTYPE_TMP, pBucket);

              pBucket = pNexNod;
         }

         sem_post(&pMemPool->sMemSemVal);
         
         XFREE(MTYPE_TMP, pMemPool);
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_pool_expand                                      */
/*Retu: 0 , -1                                               */
/*Desc: 内存池生长                                                */
/*-----------------------------------------------------------*/
static int mem_pool_expand(mem_pool_ctrl *pMemPool)
{
    int              iCount = 0;
    int              pCtlLen = 0;
    char            *pNewBff = NULL;
    mem_pool_bucket *pBucket = NULL;        

    pCtlLen = sizeof(mem_pool_bucket) + sizeof(mem_pool_object) * pMemPool->iBucketBlks;
    
    pBucket = (mem_pool_bucket*)XMALLOC(MTYPE_TMP, pCtlLen);

    pNewBff = (char*)XMALLOC(MTYPE_TMP, pMemPool->iBucketRSize);
    
    if(pNewBff && pBucket)
    {
         memset(pBucket, 0, pCtlLen);

      // memset(pNewBff, 0, pMemPool->iBucketRSize);
         
         pBucket->pStartAddr = pNewBff;
     //  pBucket->pStartObj  = pNewBff;
         pBucket->pEndAddr   = pNewBff + pMemPool->iBucketRSize;
     
         pBucket->iFirstFree = 0;
         pBucket->iBlksCount = pMemPool->iBucketBlks;
         pBucket->iFreeCount = pMemPool->iBucketBlks;
         
         for(iCount = 0; iCount < pBucket->iBlksCount; iCount ++)
         {
               pBucket->sObjTable[iCount].iNext   = iCount + 1;     
         
               pBucket->sObjTable[iCount].pObjMem = pBucket->pStartAddr + iCount * pMemPool->iBlockASize;
         }

         iCount = pBucket->iBlksCount - 1;
         
         pBucket->sObjTable[iCount].iNext = MEM_POOL_INVALID_BLKIDX;

         pMemPool->iBucketUsed ++;
         pMemPool->iBlockAllCo += pMemPool->iBucketBlks;
         pMemPool->iBlockFreeC += pMemPool->iBucketBlks;

         list_add_bynode(&pMemPool->sBktFreeLst, &pBucket->sNode);

         return(0);
    }
    else 
    {
         zlog_err("%s[%d] malloc memory error pNewBff=%p, pBucket=%p, pCtlLen=%d\r\n",
                  __FILE__, __LINE__, pNewBff,pBucket,pCtlLen);

         MEM_POOL_PRINTF("%s[%d] malloc memory error pNewBff=%p, pBucket=%p, pCtlLen=%d\r\n", 
                        __FILE__, __LINE__, pNewBff,pBucket,pCtlLen);
         
         return(-1);
    }
}


/*-----------------------------------------------------------*/
/*Name: mem_pool_shrink                                      */
/*Retu: 0 , -1                                               */
/*Desc: 内存池收缩                                                */
/*-----------------------------------------------------------*/
static void mem_pool_shrink(mem_pool_ctrl *pMemPool, mem_pool_bucket *pBucket)
{
     list_delete_bynode(&pMemPool->sBktFreeLst, &pBucket->sNode);

     pMemPool->iBucketUsed --;

     pMemPool->iBlockAllCo -= pMemPool->iBucketBlks;

     pMemPool->iBlockFreeC -= pMemPool->iBucketBlks;

     
     XFREE(MTYPE_TMP, pBucket->pStartAddr);
     
     XFREE(MTYPE_TMP, pBucket);
}

/*-----------------------------------------------------------*/
/*Name: mem_pool_listmove                                    */
/*Retu: 0 , -1                                               */
/*Desc: 桶改变队列                                                */
/*-----------------------------------------------------------*/

static void mem_pool_move(struct list *pSrcLst, 
                          struct list *pDstLst, 
                          struct listnode *pNode)
{
    list_delete_bynode(pSrcLst, pNode);
    
    list_add_bynode(pDstLst, pNode);
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*-----------------------------------------------------------*/
/*Name: mem_pool_malloc                                      */
/*Retu: void*                                                */
/*Desc: 从指定池中申请一个块                                           */
/*-----------------------------------------------------------*/
void* mem_pool_malloc(mempool_id lMempid)
{
    char            *pCurPkt = NULL;
    mem_pool_bucket *pBucket = NULL;
    mem_pool_bucket *pNexBkt = NULL;
    mem_pool_object *pObject = NULL;   
    mem_pool_ctrl   *pMemPool = (mem_pool_ctrl*)lMempid;

    if(pMemPool == NULL) return(pCurPkt);

    sem_wait(&pMemPool->sMemSemVal);

    if((pMemPool->iBlockFreeC <= 0) &&
       (pMemPool->iBucketUsed < pMemPool->iBucketMax))
    {
        mem_pool_expand(pMemPool);
    }

    pBucket = (mem_pool_bucket*)pMemPool->sBktFreeLst.head;
    
    while(pBucket)
    {
        pNexBkt = (mem_pool_bucket*)pBucket->sNode.next;
        
        if((pBucket->iFreeCount > 0) && (pBucket->iFirstFree >= 0))
        {
            pObject = &pBucket->sObjTable[pBucket->iFirstFree];
            
            pCurPkt = pObject->pObjMem;

            pBucket->iFreeCount --;

            pBucket->iFirstFree = pObject->iNext;

            pObject->iUseFlg = 1;
            
            pObject->iNext = MEM_POOL_INVALID_BLKIDX;
            
            if((pBucket->iFreeCount <= 0) || (pBucket->iFirstFree < 0))
            {
                pBucket->iFreeCount = 0;
                pBucket->iFirstFree = MEM_POOL_INVALID_BLKIDX;

                mem_pool_move(&pMemPool->sBktFreeLst, &pMemPool->sBktBusyLst, &pBucket->sNode);
            }
            break;
        }
        else
        {                                                 //进到这里,肯定出现访问错误了,
            pBucket->iFreeCount = 0;
            pBucket->iFirstFree = MEM_POOL_INVALID_BLKIDX;
            
            mem_pool_move(&pMemPool->sBktFreeLst, &pMemPool->sBktBusyLst, &pBucket->sNode);
        }

        pBucket = pNexBkt;
    }

    if(pCurPkt != NULL)
    {
        pMemPool->iBlockFreeC --;
    }
    else
    {
        zlog_err("%s[%d] malloc memblock error, pool is full! mallocblk=%d, freeblk=%d\r\n", 
                  __FILE__, __LINE__, pMemPool->iBlockAllCo, pMemPool->iBlockFreeC);

        MEM_POOL_PRINTF("%s[%d] malloc memblock error, pool is full! mallocblk=%d, freeblk=%d\r\n", 
                       __FILE__, __LINE__, pMemPool->iBlockAllCo, pMemPool->iBlockFreeC);
    }

    sem_post(&pMemPool->sMemSemVal);
    
    return(pCurPkt);
}


/*-----------------------------------------------------------*/
/*Name: mem_pool_match                                       */
/*Retu: void                                                 */
/*Desc: 匹配桶并释放                                               */
/*-----------------------------------------------------------*/
static mem_pool_bucket* mem_pool_match(struct list *pList, char *pBlock, int iBlkSz)
{
    int              lOffset = 0;
    mem_pool_bucket *pBucket = NULL;
    mem_pool_bucket *pBktFnd = NULL;
    mem_pool_object *pObject = NULL; 

    pBucket = (mem_pool_bucket*)pList->head;
     
     while(pBucket)                                      /*如果桶数太多,会影响一点效率*/
     {         
         if((pBlock >= pBucket->pStartAddr) && (pBlock < pBucket->pEndAddr))
         {
             pBktFnd = pBucket;
             
             lOffset = (int)(pBlock - pBucket->pStartAddr);

             lOffset = lOffset / iBlkSz;
             
             pObject = &pBucket->sObjTable[lOffset];

             if(pBlock == pObject->pObjMem)
             {
                  if(pObject->iUseFlg)
                  {
                      pObject->iUseFlg = 0;
                      pObject->iNext   = pBucket->iFirstFree;

                      pBucket->iFreeCount ++;
                      pBucket->iFreeResu  = 1;
                      pBucket->iFirstFree = lOffset;
                  }
                  else
                  {
                      zlog_err("%s[%d] release repeat %p!\r\n", __FILE__, __LINE__, pBlock);
                     
                      MEM_POOL_PRINTF("%s[%d] release repeat %p!\r\n", __FILE__, __LINE__, pBlock);
                  }
             }
             else
             {
                  zlog_err("%s[%d] free address error %p!\r\n", __FILE__, __LINE__, pBlock);
                 
                  MEM_POOL_PRINTF("%s[%d] free address error %p!\r\n", __FILE__, __LINE__, pBlock);
             }

             break;
         }
    
         pBucket = (mem_pool_bucket*)pBucket->sNode.next;
     }

     return(pBktFnd);
}


/*-----------------------------------------------------------*/
/*Name: mem_pool_free                                        */
/*Retu: void                                                 */
/*Desc: 释放一块                                                 */
/*-----------------------------------------------------------*/
void* mem_pool_free(mempool_id lMempid, void *pBlock)
{
    mem_pool_bucket *pBucket = NULL;
    
    mem_pool_ctrl *pMemPool = (mem_pool_ctrl*)lMempid;

    if((pMemPool != NULL) && (pBlock != NULL))
    {        
         sem_wait(&pMemPool->sMemSemVal);
         
         pBucket = mem_pool_match(&pMemPool->sBktBusyLst, (char*)pBlock, pMemPool->iBlockASize);
  
         if(pBucket)
         {
              if(pBucket->iFreeResu)
              {
                  mem_pool_move(&pMemPool->sBktBusyLst, &pMemPool->sBktFreeLst, &pBucket->sNode);
              }
         }
         else
         {
              pBucket = mem_pool_match(&pMemPool->sBktFreeLst, (char*)pBlock, pMemPool->iBlockASize);
         }
         
         if(pBucket)
         {
              if(pBucket->iFreeResu)
              {
                   pBucket->iFreeResu = 0;
                   
                   pMemPool->iBlockFreeC ++;

                   if((pMemPool->iBktFreeEna) && (pBucket->iBlksCount == pBucket->iFreeCount))
                   {
                       mem_pool_shrink(pMemPool, pBucket);
                   }
              }
         }
         else
         {
              //zlog_err("%s[%d] free address error %p!, not mempool addr!\r\n", __FILE__, __LINE__, pBlock);
            
              //MEM_POOL_PRINTF("%s[%d] free address error %p!, not mempool addr!\r\n", __FILE__, __LINE__, pBlock);
         }
             
         sem_post(&pMemPool->sMemSemVal);
    }
    
    return(pBucket);
}

/*====================================================================================================================*/
/*====================================================================================================================*/

/*-----------------------------------------------------------*/
/*Name: mem_pool_infor                                       */
/*Para: iBufLen > 1024                                       */
/*Retu: -1 error >=0 datalen                                 */
/*Desc: 显示内存池的状态信息                                           */
/*-----------------------------------------------------------*/
int mem_pool_infor(mempool_id lMempid, char *pBuffer, int iBufLen)
{
    int              iAllLen = 0;
    int              iIndex   = 1;
    mem_pool_bucket *pBucket  = NULL;
    mem_pool_ctrl   *pMemPool = (mem_pool_ctrl*)lMempid;

    if((pMemPool == NULL) || (pBuffer == NULL)) return(-1);

    sem_wait(&pMemPool->sMemSemVal);

    iAllLen = pMemPool->sBktBusyLst.count + pMemPool->sBktFreeLst.count;

    iAllLen = (iAllLen + 12) * 80;
    
    if(iAllLen > iBufLen)
    {
        iAllLen = sprintf(pBuffer, "Buffer too small %d\r\n", iBufLen);
    }
    else
    {
        iAllLen = 0;

        iAllLen += sprintf(pBuffer + iAllLen, "Memory pool id = %p\r\n", pMemPool);
        iAllLen += sprintf(pBuffer + iAllLen, "----------------- ---------------------------\r\n");
        
        iAllLen += sprintf(pBuffer + iAllLen, "Bucket Max      : %d\r\n", pMemPool->iBucketMax);
        iAllLen += sprintf(pBuffer + iAllLen, "Bucket Used     : %d\r\n", pMemPool->iBucketUsed);
        iAllLen += sprintf(pBuffer + iAllLen, "Bucket Size     : %d\r\n", pMemPool->iBucketRSize);
        iAllLen += sprintf(pBuffer + iAllLen, "Block Per Bucket: %d\r\n", pMemPool->iBucketBlks);
        iAllLen += sprintf(pBuffer + iAllLen, "Free Bucket Free: %d\r\n", pMemPool->iBktFreeEna);

        iAllLen += sprintf(pBuffer + iAllLen, "Block Init Size : %d\r\n", pMemPool->iBlockRSize);
        iAllLen += sprintf(pBuffer + iAllLen, "Block Align Size: %d\r\n", pMemPool->iBlockASize);

        iAllLen += sprintf(pBuffer + iAllLen, "Malloc Block Cou: %d\r\n", pMemPool->iBlockAllCo);
        iAllLen += sprintf(pBuffer + iAllLen, "Free Block Count: %d\r\n", pMemPool->iBlockFreeC);

        iAllLen += sprintf(pBuffer + iAllLen, "Bucket List: \r\n");
            

        iAllLen += sprintf(pBuffer + iAllLen, "Index StartAddr  EndAddr    BlockCou FreeBlock FirstFree FreeResult\r\n" );
        iAllLen += sprintf(pBuffer + iAllLen, "----- ---------- ---------- -------- --------- --------- ----------\r\n" );

        pBucket = (mem_pool_bucket*)pMemPool->sBktFreeLst.head;
           
        while(pBucket)                                      
        {  
             iAllLen += sprintf(pBuffer + iAllLen, " %4d %p %p %8d %9d %9d %8d\r\n", iIndex,pBucket->pStartAddr, 
                                pBucket->pEndAddr, pBucket->iBlksCount, pBucket->iFreeCount, pBucket->iFirstFree, pBucket->iFreeResu);
        
             iIndex ++;
             
             pBucket = (mem_pool_bucket*)pBucket->sNode.next;
        }

        pBucket = (mem_pool_bucket*)pMemPool->sBktBusyLst.head;
              
        while(pBucket)                                      
        {  
             iAllLen += sprintf(pBuffer + iAllLen, "*%4d %p %p %8d %9d %9d %8d\r\n", iIndex,pBucket->pStartAddr, 
                                pBucket->pEndAddr, pBucket->iBlksCount, pBucket->iFreeCount, pBucket->iFirstFree, pBucket->iFreeResu);
        
             iIndex ++;
             
             pBucket = (mem_pool_bucket*)pBucket->sNode.next;
        }
    }

    sem_post(&pMemPool->sMemSemVal);

    return(iAllLen);
}



/*====================================================================================================================*/
/*====================================================================================================================*/


