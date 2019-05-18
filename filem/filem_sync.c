/* file process command 
   name filem_slot.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/memory.h"
#include "lib/devm_com.h"
#include "lib/module_id.h"
#include "lib/log.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/sys_ha.h"

#include "filem.h"
#include "filem_tran.h"
#include "filem_cmd.h"
#include "filem_oper.h"
#include "filem_img.h"
#include "filem_slot.h"
#include "filem_md5.h"
#include "filem_sync.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
struct list gFileRtTransQue;                     /*本板实时传输等待队列,只主控使用*/
                                                 /*用于处理其他进程的实时传输请求 */
struct list gFileLableLists[FILEM_MTYPE_MAX];    /*保存本板修改标签 即文件以前信息*/

static void filem_sync_rtsyncstart(file_syncitem *pFItem);

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*实时传输优先级最高,且忽略保护*/
/*槽位同步和CLI优先级次之,考虑保护*/

/*---------------------------------------------------------*/
/* Name: filem_sync_rttranrun                              */
/* 判断是否执行实时传输                                       */
/*---------------------------------------------------------*/
int filem_sync_rttranrun(void)
{
    return((gFileRtTransQue.count == 0) ? 0 : 1);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_lableupdate                            */
/* 文件指纹记录更新                                        */
/*---------------------------------------------------------*/
static void filem_sync_lableupdate(struct list *pRecLst, struct list *pCurLst)
{
    fileitems  *pSrc = NULL;
    fileitems  *pDes = NULL;

    filem_show_listclr(pRecLst);

    pSrc = (fileitems*)pCurLst->head;

    while(pSrc)
    {
         pDes = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(fileitems));

         if(pDes)
         {
             memcpy(pDes, pSrc, sizeof(fileitems));

             list_add_bynode(pRecLst, &pDes->sNode);
         }

         pSrc = (fileitems*)pSrc->sNode.next;
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_lablesave                              */
/* 文件指纹写入文件                                        */
/*---------------------------------------------------------*/
static void filem_sync_lablesave(struct list *pList, char *pPath)
{
    int  iFd =  -1;
    char bName[128] = {0};
    struct listnode *pNode = NULL;

    sprintf(bName, "%s%s", pPath, FILEM_FILE_CHANGE_LABLE);

    iFd =  filem_oper_open(bName, O_WRONLY|O_CREAT|O_TRUNC, 0);

    if(iFd >= 0)
    {
         pNode = pList->head;
 
         while(pNode)
         {
              filem_oper_write(iFd, (char*)pNode, sizeof(fileitems));
 
              pNode = pNode->next;
         }
         
         filem_oper_close(iFd);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_lableread                              */
/* 读取一类文件的变化指纹记录                              */
/*---------------------------------------------------------*/
static void filem_sync_lableread(int iType)
{
    int   iFd  = -1;
    int   iRet =  0;

    char *pPath = NULL;
    char  bName[128] = {0};

    fileitems *pFitem = NULL;

    pPath = filem_type_pathget(iType);
    
    sprintf(bName, "%s%s", pPath, FILEM_FILE_CHANGE_LABLE);

    iFd =  filem_oper_open(bName, O_RDONLY, 0);

    if(iFd >= 0)
    {
         for(;;)
         {
             pFitem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(fileitems));

             if(pFitem)
             {
                 iRet = filem_oper_read(iFd, (char*)pFitem, sizeof(fileitems));

                 if(iRet != sizeof(fileitems))
                 {
                     XFREE(MTYPE_FILEM_ENTRY, pFitem);
                     break;
                 }
                 else
                 {
                     list_add_bynode(&gFileLableLists[iType], &pFitem->sNode);
                 }
             }
         }

         filem_oper_close(iFd);
    }
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_sync_md5create                              */
/* 文件产生MD5指纹                                         */
/*---------------------------------------------------------*/
static void filem_sync_md5create(fileitems *pFitem, char *pPath)
{
    int   iFd    = -1;
    int   iCLen  = 0;
    int   iFLen  = 0;
    char  bFName[128] = {0};
    char  bMd5Dt[128] = {0};

    sprintf(bFName, "%s%s", pPath, pFitem->bName);

    iFd = filem_oper_open(bFName, O_RDONLY, 0);

    if(iFd >= 0)
    {
        iFLen = filem_oper_size_getf(iFd);

        if(iFLen < FILEM_MD5CHECK_LEN) 
             iCLen = iFLen;
        else iCLen = FILEM_MD5CHECK_LEN;
        
        if(filem_md5_calculate(iFd, iCLen, bMd5Dt, iFLen) == 0)
        {
            pFitem->iHmacLen = FILEM_PAD_MD5_BUSZ;   /*字符串表示*/

            memcpy(pFitem->bHmacData, bMd5Dt, FILEM_PAD_MD5_BUSZ);
        }

        filem_oper_close(iFd);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_filecheck                              */
/* 文件改变检查                                            */
/*---------------------------------------------------------*/
static int filem_sync_filecheck(struct list *pRecLst, fileitems *pFitem)
{
    int              iChgd = 1;
    fileitems       *pReco = NULL;
    struct listnode *pNode = NULL;
    
    pNode = pRecLst->head;
    while(pNode)
    {
        pReco = (fileitems *)pNode;

        if(strcmp(pFitem->bName, pReco->bName) == 0)
        {
            if((pFitem->iSize == pReco->iSize) &&
               (pFitem->iModTime == pReco->iModTime))
            {                                      /*本地根据文件名 大小 修改时间判断*/
                iChgd = 0; 
                     
                pFitem->iHmacLen = pReco->iHmacLen;
                     
                memcpy(pFitem->bHmacData, pReco->bHmacData, FILEM_PAD_MD5_BUSZ);
            }

            break;
        }

        pNode = pNode->next;
    }

    return(iChgd);
}


/*---------------------------------------------------------*/
/* Name: filem_sync_pathcheck                              */
/* 文件本地路径下是否改变                                  */
/*---------------------------------------------------------*/
static int filem_sync_pathcheck(int iType, int iSync, filemslot *pSlot) 
{
    int              iOneChg = 0;
    int              iDirChg = 0;
    char            *pDPath  = NULL;
    fileitems       *pFitem  = NULL;
    struct list     *pRecLst = NULL;
    struct list     *pCurLst = NULL;
    struct listnode *pCurNod = NULL;

    pDPath  = filem_type_pathget(iType);
    
    pCurLst = &pSlot->filelist[iType];
    
    filem_show_dirlist(iType, pCurLst, pDPath, 1);

    if(iSync)  /*不需要同步的目录,不需要计算MD5和修改指纹*/                       
    {
        pCurNod = pCurLst->head;

        pRecLst = &gFileLableLists[iType];
                                                    /*此处应该需要标识被删除文件???  */
        while(pCurNod)
        {
            pFitem  = (fileitems*)pCurNod;
            
            iOneChg = filem_sync_filecheck(pRecLst, pFitem);

            if(iOneChg)
            {
                iDirChg = iOneChg;

                pFitem->iHmacLen = 0;
                                                   /*不需要自动同步的目录，不需要计算MD5*/
                filem_sync_md5create(pFitem, pDPath);
            }
            
            pCurNod = pCurNod->next;
        }

        if(iDirChg)
        {
            filem_sync_lablesave(pCurLst, pDPath);
            
            filem_sync_lableupdate(pRecLst, pCurLst);
        }
    }
    return(iDirChg);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_fillhead                               */
/* 发送报文头填充                                          */
/*---------------------------------------------------------*/
static void filem_sync_fillhead(filemslot *pSlot, 
                               struct ipc_msghdr_n  *pMsgHdr) 
{
     pMsgHdr->module_id   = MODULE_ID_FILE;
     pMsgHdr->sender_id   = MODULE_ID_FILE;
     pMsgHdr->msg_type    = IPC_TYPE_HA;
     pMsgHdr->msg_subtype = HA_SMSG_DATA_REPORT;
     pMsgHdr->opcode      = 0;
     pMsgHdr->data_num    = 0;
     pMsgHdr->priority    = 0;
     pMsgHdr->unit        = pSlot->munit;
     pMsgHdr->slot        = pSlot->mslot;
     pMsgHdr->srcunit     = pSlot->unit;
     pMsgHdr->srcslot     = pSlot->slot;
     pMsgHdr->sequence    = pSlot->sndserno;
     pMsgHdr->result      = 0;
}

/*---------------------------------------------------------*/
/* Name: filem_sync_itemorderset                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_itemorderset(fileitems *pDest, fileitems *pSrcItm) 
{    
     memset(pDest, 0, sizeof(fileitems));
     
    pDest->iMtype  = htonl(pSrcItm->iMtype);
    pDest->iStype  = htonl(pSrcItm->iStype);
    pDest->iSlot   = htonl(pSrcItm->iSlot);
    pDest->iIndex  = htonl(pSrcItm->iIndex);
    pDest->iFlag   = htonl(pSrcItm->iFlag);
    pDest->iUid    = htonl(pSrcItm->iUid);
    pDest->iGid    = htonl(pSrcItm->iGid);
    pDest->iSize   = htonl(pSrcItm->iSize);
    pDest->iPerm   = htonl(pSrcItm->iPerm);
    pDest->iBlocks = htonl(pSrcItm->iBlocks);
    pDest->iModTime= htonl(pSrcItm->iModTime);
    pDest->iAccTime= htonl(pSrcItm->iAccTime);
    pDest->iChgTime= htonl(pSrcItm->iChgTime);
    pDest->iChgMd5 = htonl(pSrcItm->iChgMd5);
    pDest->iHmacLen= htonl(pSrcItm->iHmacLen);
    pDest->iChgTime= htonl(pSrcItm->iChgTime);

    memcpy(pDest->bHmacData, pSrcItm->bHmacData, 64);
    memcpy(pDest->bName, pSrcItm->bName, FILEM_NAME_MAXLN);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_itemorderget                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_itemorderget(fileitems *pDest, fileitems *pSrcItm) 
{    
    memset(pDest, 0, sizeof(fileitems));
     
    pDest->iMtype  = ntohl(pSrcItm->iMtype);
    pDest->iStype  = ntohl(pSrcItm->iStype);
    pDest->iSlot   = ntohl(pSrcItm->iSlot);
    pDest->iIndex  = ntohl(pSrcItm->iIndex);
    pDest->iFlag   = ntohl(pSrcItm->iFlag);
    pDest->iUid    = ntohl(pSrcItm->iUid);
    pDest->iGid    = ntohl(pSrcItm->iGid);
    pDest->iSize   = ntohl(pSrcItm->iSize);
    pDest->iPerm   = ntohl(pSrcItm->iPerm);
    pDest->iBlocks = ntohl(pSrcItm->iBlocks);
    pDest->iModTime= ntohl(pSrcItm->iModTime);
    pDest->iAccTime= ntohl(pSrcItm->iAccTime);
    pDest->iChgTime= ntohl(pSrcItm->iChgTime);
    pDest->iChgMd5 = ntohl(pSrcItm->iChgMd5);
    pDest->iHmacLen= ntohl(pSrcItm->iHmacLen);
    pDest->iChgTime= ntohl(pSrcItm->iChgTime);

    memcpy(pDest->bHmacData, pSrcItm->bHmacData, 64);
    memcpy(pDest->bName, pSrcItm->bName, FILEM_NAME_MAXLN);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_lsthdorderset                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_listhdorderset(file_listhdr *pDest, file_listhdr *pSrcItm) 
{    
    memset(pDest, 0, sizeof(file_listhdr));
     
    pDest->iSType    = htons(pSrcItm->iSType);
    pDest->iFlags    = htons(pSrcItm->iFlags);
    pDest->iFragCou  = htons(pSrcItm->iFragCou);
    pDest->iFragCur  = htons(pSrcItm->iFragCur);
    pDest->iDatLen   = htonl(pSrcItm->iDatLen);
    pDest->iOffset   = htonl(pSrcItm->iOffset);
    pDest->iTotalLen = htonl(pSrcItm->iTotalLen);
  
}

/*---------------------------------------------------------*/
/* Name: filem_sync_lsthdorderget                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_listhdorderget(file_listhdr *pDest, file_listhdr *pSrcItm) 
{    
    memset(pDest, 0, sizeof(file_listhdr));
     
    pDest->iSType    = ntohs(pSrcItm->iSType);
    pDest->iFlags    = ntohs(pSrcItm->iFlags);
    pDest->iFragCou  = ntohs(pSrcItm->iFragCou);
    pDest->iFragCur  = ntohs(pSrcItm->iFragCur);
    pDest->iDatLen   = ntohl(pSrcItm->iDatLen);
    pDest->iOffset   = ntohl(pSrcItm->iOffset);
    pDest->iTotalLen = ntohl(pSrcItm->iTotalLen);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_statusorderset                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_statusorderset(filemstatus *pDest, filemstatus *pSrcItm) 
{    
    memset(pDest, 0, sizeof(filemstatus));
     
    pDest->iSlotNum  = htonl(pSrcItm->iSlotNum);
    pDest->iDownload = htonl(pSrcItm->iDownload);
    pDest->iUpload   = htonl(pSrcItm->iUpload);
    pDest->iUpgrade  = htonl(pSrcItm->iUpgrade);
    pDest->iUpgradeTime = htonl(pSrcItm->iUpgradeTime);
    pDest->iUpgFpga     = htonl(pSrcItm->iUpgFpga);
    pDest->iUpgFpgaTime = htonl(pSrcItm->iUpgFpgaTime);
    pDest->iUpgKnl      = htonl(pSrcItm->iUpgKnl);
    pDest->iUpgKnlTime  = htonl(pSrcItm->iUpgKnlTime);
    pDest->iFreeSpace   = htonl(pSrcItm->iFreeSpace);
    pDest->iBootStat    = htonl(pSrcItm->iBootStat);
    pDest->iSlotFlags   = htonl(pSrcItm->iSlotFlags);
    pDest->iL3Vpn       = htonl(pSrcItm->iL3Vpn);

    memcpy(pDest->cSoftVer,   pSrcItm->cSoftVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cFpgaVer,   pSrcItm->cFpgaVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cKernelVer, pSrcItm->cKernelVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cBootVer,   pSrcItm->cBootVer, FILEM_VERS_MAXLN);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_statusorderget                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_statusorderget(filemstatus *pDest, filemstatus *pSrcItm) 
{    
    memset(pDest, 0, sizeof(filemstatus));
     
    pDest->iSlotNum  = ntohl(pSrcItm->iSlotNum);
    pDest->iDownload = ntohl(pSrcItm->iDownload);
    pDest->iUpload   = ntohl(pSrcItm->iUpload);
    pDest->iUpgrade  = ntohl(pSrcItm->iUpgrade);
    pDest->iUpgradeTime = ntohl(pSrcItm->iUpgradeTime);
    pDest->iUpgFpga     = ntohl(pSrcItm->iUpgFpga);
    pDest->iUpgFpgaTime = ntohl(pSrcItm->iUpgFpgaTime);
    pDest->iUpgKnl      = ntohl(pSrcItm->iUpgKnl);
    pDest->iUpgKnlTime  = ntohl(pSrcItm->iUpgKnlTime);
    pDest->iFreeSpace   = ntohl(pSrcItm->iFreeSpace);
    pDest->iBootStat    = ntohl(pSrcItm->iBootStat);
    pDest->iSlotFlags   = ntohl(pSrcItm->iSlotFlags);

    pDest->iL3Vpn       = ntohl(pSrcItm->iL3Vpn);

    memcpy(pDest->cSoftVer,   pSrcItm->cSoftVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cFpgaVer,   pSrcItm->cFpgaVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cKernelVer, pSrcItm->cKernelVer, FILEM_VERS_MAXLN);
    memcpy(pDest->cBootVer,   pSrcItm->cBootVer, FILEM_VERS_MAXLN);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_knlareorderset                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_knlareorderset(kernelarea *pDest, kernelarea *pSrcItm) 
{    
    memset(pDest, 0, sizeof(kernelarea));
     
    memcpy(pDest->isused, pSrcItm->isused, 4);
    memcpy(pDest->valid,  pSrcItm->valid, 4);
    memcpy(pDest->active, pSrcItm->active, 4);

    pDest->areasize = htonl(pSrcItm->areasize);
    pDest->datalen  = htonl(pSrcItm->datalen);

    memcpy(pDest->filevers, pSrcItm->filevers, FILEM_VERS_MAXLN);
    memcpy(pDest->md5data,  pSrcItm->md5data, FILEM_PAD_MD5_MARK);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_knlareorderget                           */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_knlareorderget(kernelarea *pDest, kernelarea *pSrcItm) 
{    
    memset(pDest, 0, sizeof(kernelarea));
     
    memcpy(pDest->isused, pSrcItm->isused, 4);
    memcpy(pDest->valid,  pSrcItm->valid, 4);
    memcpy(pDest->active, pSrcItm->active, 4);
    
    pDest->areasize = ntohl(pSrcItm->areasize);
    pDest->datalen  = ntohl(pSrcItm->datalen);
    
    memcpy(pDest->filevers, pSrcItm->filevers, FILEM_VERS_MAXLN);
    memcpy(pDest->md5data,  pSrcItm->md5data, FILEM_PAD_MD5_MARK);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_listsend                               */
/* 文件文件列表发送到主CPU                                 */
/*---------------------------------------------------------*/
static void filem_sync_listsend(int iType, int iChanged, filemslot *pSlot) 
{    
  //if((pSlot->forcerep > 0) || iChanged)
    {
        unsigned int   iOneLen = 0;
        unsigned int   iFrgcou = 0;
        unsigned int   iFrgcur = 0;
        unsigned int   iSumCou = 0;
        char          *pDatBuf = NULL;
        fileitems     *pFitem  = NULL;
        
        file_listhdr         sDatHdr;
        struct ipc_msghdr_n  sMsgHdr;
        struct list         *pCurLst = NULL;
        struct ipc_mesg_n   *pSndMsg =  NULL;

        pSlot->sndserno ++;
        
        filem_sync_fillhead(pSlot, &sMsgHdr);
        
        pCurLst = &pSlot->filelist[iType];

        iFrgcou = (pCurLst->count + FILEM_PERFRAG_FILES -1) / FILEM_PERFRAG_FILES;
           
        sDatHdr.iSType    = iType;
        sDatHdr.iDatLen   = 0;
        sDatHdr.iOffset   = 0;
        sDatHdr.iTotalLen = pCurLst->count * sizeof(fileitems);
        sDatHdr.iFragCou  = (iFrgcou == 0) ? 1 : iFrgcou;
        sDatHdr.iFragCur  = 1;
        sDatHdr.iFlags    = pSlot->sysstat;

        if(iFrgcou == 0)
        {
            sMsgHdr.data_len = sizeof(file_listhdr);

            pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + IPC_MSG_LEN_S, MODULE_ID_FILE);

            if(pSndMsg)
            {
                memcpy(&pSndMsg->msghdr, &sMsgHdr, sizeof(struct ipc_msghdr_n));

                filem_sync_listhdorderset((file_listhdr*)pSndMsg->msg_data, &sDatHdr);
                
                filem_slot_sendmsg(pSndMsg, sizeof(struct ipc_msghdr_n) + sizeof(file_listhdr));
            }
        }
        else
        {
            pFitem = (fileitems*)pCurLst->head;

            for(iFrgcur = 1; iFrgcur <= iFrgcou; iFrgcur++)
            {
                  pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + IPC_MSG_LEN_N, MODULE_ID_FILE);
                 
                  if(!pSndMsg) break;
                 
                  pDatBuf = (char*)pSndMsg->msg_data + sizeof(file_listhdr);
                              
                  sDatHdr.iOffset += iOneLen;
                  
                  iOneLen = iSumCou = 0;
     
                  while(pFitem)
                  {
                      filem_sync_itemorderset((fileitems*)(pDatBuf + iOneLen), pFitem);
     
                      iOneLen += sizeof(fileitems);
     
                      pFitem = (fileitems*)pFitem->sNode.next;
                      
                      if(++iSumCou >= FILEM_PERFRAG_FILES) break;
                  }
                  
                  sDatHdr.iFragCur = iFrgcur;
                  sDatHdr.iDatLen  = iOneLen;
     
                  sMsgHdr.data_len = iOneLen + sizeof(file_listhdr);
      
                  memcpy(&pSndMsg->msghdr, &sMsgHdr, sizeof(struct ipc_msghdr_n));
                  
                  filem_sync_listhdorderset((file_listhdr*)pSndMsg->msg_data, &sDatHdr);
                  
                  filem_slot_sendmsg(pSndMsg, sizeof(struct ipc_msghdr_n) + sizeof(file_listhdr) + iOneLen);
            }
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_statussend                             */
/* 版本信息及操作状态上报  备用CPU执行                     */
/*---------------------------------------------------------*/

int filem_sync_statussend(void)
{
     int                iAlen   = 0;
     filemslot         *pSlot   = NULL;
     file_listhdr       sDatHdr = {0};
     struct ipc_mesg_n *pSndMsg =  NULL;

     iAlen = sizeof(file_listhdr) + sizeof(filemstatus) + sizeof(kernelarea) * FILEM_KRL_AREA_MAX;;
     
     pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + iAlen, MODULE_ID_FILE);
     
     if(pSndMsg != NULL)
     {
         memset(&sDatHdr, 0, sizeof(file_listhdr));
         
         pSlot = filem_slot_objectget(0);

         pSlot->sndserno ++;
         filem_sync_fillhead(pSlot, &pSndMsg->msghdr);

         filem_diskspace_get();

         iAlen = sizeof(file_listhdr);


         filem_sync_statusorderset((filemstatus*)(pSndMsg->msg_data + iAlen), &pSlot->filestainf);
     
         iAlen += sizeof(filemstatus);


         filem_sync_knlareorderset((kernelarea*)(pSndMsg->msg_data + iAlen), &pSlot->fileknltbl[0]);

         iAlen += sizeof(kernelarea);


         filem_sync_knlareorderset((kernelarea*)(pSndMsg->msg_data + iAlen), &pSlot->fileknltbl[1]);

         iAlen += sizeof(kernelarea);

         
         pSndMsg->msghdr.data_len = iAlen;


         sDatHdr.iSType  = FILEM_MTYPE_MAX;
     
         sDatHdr.iDatLen = iAlen - sizeof(file_listhdr);
          
         filem_sync_listhdorderset((file_listhdr*)pSndMsg->msg_data, &sDatHdr);

         filem_slot_sendmsg(pSndMsg, sizeof(struct ipc_msghdr_n) + iAlen);
     }
     
     return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_filereport                             */
/* 文件目录上报  备用CPU执行                               */
/*---------------------------------------------------------*/
static void filem_sync_filereport(filemslot *pSlot) 
{
    if((pSlot->syntimer > 0) && !filem_tran_tranisbusy())
    {    
        if(--pSlot->syntimer == 0) 
        {
            pSlot->syntimer = FILEM_FRESH_TIMER;

            filem_sync_statussend();
 
            filem_sync_pathcheck(FILEM_MTYPE_BIN, 0, pSlot);
 
            filem_sync_listsend(FILEM_MTYPE_BIN,  1, pSlot);

            usleep(200000);
            
            filem_sync_pathcheck(FILEM_MTYPE_CFG, 0, pSlot);
 
            filem_sync_listsend(FILEM_MTYPE_CFG,  1, pSlot);
 
            usleep(200000);
 
            filem_sync_pathcheck(FILEM_MTYPE_DAT, 0, pSlot);
  
            filem_sync_listsend(FILEM_MTYPE_DAT,  1, pSlot);
 
            usleep(200000);

            filem_sync_pathcheck(FILEM_MTYPE_VER, 1, pSlot);
 
            filem_sync_listsend(FILEM_MTYPE_VER,  1, pSlot);
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_changecheck                            */
/* 文件修改检查 主用CPU执行                                */
/*---------------------------------------------------------*/
static void filem_sync_changecheck(filemslot *pSlot) 
{
    /*            
     filem_sync_pathcheck(FILEM_MTYPE_CFG, 0, pSlot);
 
     filem_sync_pathcheck(FILEM_MTYPE_DAT, 0, pSlot);
    */ 

     filem_sync_pathcheck(FILEM_MTYPE_VER, 1, pSlot);
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_sync_listqueadd                             */
/* 文件记录更新                                            */
/*---------------------------------------------------------*/
static void filem_sync_listqueadd(struct list *pList, char *pData, int iLen)
{
    int        iCln = 0;
    fileitems *pItem = NULL;
    
    while(iCln < iLen)
    {
        pItem = (fileitems *)XMALLOC(MTYPE_FILEM_ENTRY, sizeof(fileitems));
 
        if(pItem != NULL)
        {
            filem_sync_itemorderget(pItem, (fileitems*)(pData + iCln));

            list_add_bynode(pList, &pItem->sNode);
        }

        iCln += sizeof(fileitems);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_sync_listrecv                               */
/* 文件列表接收  主CPU执行                                 */
/*---------------------------------------------------------*/

static void filem_sync_listupdate(file_listhdr *pDatHdr, filemslot *pSlot, 
                                  unsigned int iSerno, char *pRevDat, int iDatLen) 
{
    struct list *pList   = NULL;

    pRevDat = pRevDat + sizeof(file_listhdr);

    if(pDatHdr->iOffset == 0)                  /*第一个报文*/
    {
        if(pDatHdr->iFragCou <= 1)             /*无分片*/
        {
             pList = filem_slot_filelist(pSlot->slot, pDatHdr->iSType);

             if(pList)                         /*正在同步的槽位,不做文件更新*/
             {
                filem_show_listclr(pList);

                filem_sync_listqueadd(pList, pRevDat, pDatHdr->iDatLen);

                if(filem_slot_ismaincpu(pSlot->slot)) 
                {
                    if((pDatHdr->iSType == FILEM_MTYPE_VER) && 
                       (pDatHdr->iFlags == HA_BOOTSTA_DATAOK) && (pSlot->syntimer <= 0))
                    {                             /*备板必须在加载完成后才启动同步*/
                         pSlot->syntimer = FILEM_MSYNC_DELAY; 

                         zlog_debug(FILEM_DEBUG_SYNC, "Receive file list, start sync timer\r\n"); 
                    }
                }
             }
        }
        else
        {
             if(pSlot->rcvbuff) XFREE(MTYPE_FILEM_ENTRY, pSlot->rcvbuff);

             pSlot->rcvbuff = XMALLOC(MTYPE_FILEM_ENTRY, pDatHdr->iTotalLen);

             if(pSlot->rcvbuff)               /*如果没有内存,直接丢弃接收数据*/
             {
                 pSlot->rcvtout   = FILEM_RCVPKT_TOUT;
                 pSlot->rcvserno  = iSerno;
                 pSlot->rcvtype   = pDatHdr->iSType;
                 pSlot->rcvoffset = pDatHdr->iDatLen;
                 pSlot->rcvtotlen = pDatHdr->iTotalLen;
                 
                 memcpy(pSlot->rcvbuff + pDatHdr->iOffset, pRevDat, pDatHdr->iDatLen);
             }
        }
    }
    else
    {
        if((pSlot->rcvbuff) && (pSlot->rcvserno  == iSerno) &&
           (pSlot->rcvtype   == pDatHdr->iSType)  &&
           (pSlot->rcvoffset == pDatHdr->iOffset) &&
           (pSlot->rcvtotlen >= pDatHdr->iOffset + pDatHdr->iDatLen))
        {
             memcpy(pSlot->rcvbuff + pDatHdr->iOffset, pRevDat, pDatHdr->iDatLen);

             pSlot->rcvoffset += pDatHdr->iDatLen;

             if(pSlot->rcvoffset >= pSlot->rcvtotlen)
             {
                  pList = filem_slot_filelist(pSlot->slot, pDatHdr->iSType);

                  if(pList)
                  {
                      filem_show_listclr(pList);

                      filem_sync_listqueadd(pList, pSlot->rcvbuff, pSlot->rcvtotlen);
                  }

                  XFREE(MTYPE_FILEM_ENTRY, pSlot->rcvbuff);

                  pSlot->rcvbuff = NULL;

                  if(filem_slot_ismaincpu(pSlot->slot))
                  {
                      if((pDatHdr->iSType == FILEM_MTYPE_VER) && 
                         (pDatHdr->iFlags == HA_BOOTSTA_DATAOK) && (pSlot->syntimer <= 0))
                      {
                          pSlot->syntimer = FILEM_MSYNC_DELAY;   

                          zlog_debug(FILEM_DEBUG_SYNC, "Receive file list, start sync timer\r\n"); 
                      }
                  }
             }
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_listrecv                               */
/* 文件列表接收  主CPU执行                                 */
/*---------------------------------------------------------*/
static void filem_sync_statusupdate(file_listhdr *pDatHdr,filemslot *pSlot, unsigned int iSerno,
                                    char *pRevDat, int iDatLen) 
{
    pRevDat = pRevDat + sizeof(file_listhdr);

    if((pDatHdr->iOffset == 0) && 
       (pDatHdr->iDatLen > (int) (sizeof(filemstatus) + sizeof(kernelarea))))
    {
        filem_sync_statusorderget(&pSlot->filestainf, (filemstatus*)pRevDat);

        pRevDat = pRevDat + sizeof(filemstatus);
        filem_sync_knlareorderget(&pSlot->fileknltbl[0], (kernelarea *)pRevDat); 

        pRevDat = pRevDat + sizeof(kernelarea);
        filem_sync_knlareorderget(&pSlot->fileknltbl[1], (kernelarea *)pRevDat); 
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_rcvpktproc                             */
/* 文件列表接收  主CPU执行                                 */
/*---------------------------------------------------------*/
void filem_sync_packetrecv(struct ipc_mesg_n *pMsgPkt, int iDlen)
{
    filemslot *pRemSlot = NULL;
    filemslot *pLocSlot = NULL;
    
    file_listhdr sDatHdr = {0};
    struct ipc_msghdr_n *pMsghdr = NULL;

    pMsghdr  = &pMsgPkt->msghdr;
    
    iDlen   -= sizeof(struct ipc_msghdr_n);
    
    pLocSlot = filem_slot_objectget(0);
    
    pRemSlot = filem_slot_objectget((int)pMsghdr->srcslot);

    if(pRemSlot && (pRemSlot != pLocSlot) && 
      (!pRemSlot->issync) && (pLocSlot->hastat == HA_ROLE_MASTER))
    {
         filem_sync_listhdorderget(&sDatHdr, (file_listhdr*)pMsgPkt->msg_data);

         switch(sDatHdr.iSType)
         {
             case FILEM_MTYPE_BIN:
             case FILEM_MTYPE_CFG:
             case FILEM_MTYPE_DAT:
             case FILEM_MTYPE_VER:
                  filem_sync_listupdate(&sDatHdr, pRemSlot, pMsghdr->sequence, (char*)pMsgPkt->msg_data, iDlen);
                  break;
             case FILEM_MTYPE_MAX:
                  filem_sync_statusupdate(&sDatHdr, pRemSlot, pMsghdr->sequence, (char*)pMsgPkt->msg_data, iDlen);
                  break;
             default: break;
         }
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_filenamefind                           */
/* 在目标板查找一个文件是否存在                            */
/*---------------------------------------------------------*/
static fileitems* filem_sync_filenamefind(char *bFname, struct list *pDlist) 
{
    fileitems  *pReco = NULL;
    
    pReco = (fileitems*)pDlist->head;
    
    while(pReco)
    {            
        if(strcmp(bFname, pReco->bName) == 0)
        {            
            pReco->iFlag &= ~FILEM_OPTFLG_DELETE;
            
            break;
        }

        pReco = (fileitems*)pReco->sNode.next;
    }

    return(pReco);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_difffilepull                           */
/* 找出一种类型文件中需要同步的文件                        */
/*---------------------------------------------------------*/
static void filem_sync_difffilepull(int itype, filemslot *pSSlot, filemslot *pDSlot) 
{
    struct list *pSlist = NULL;
    struct list *pDlist = NULL;
    
    fileitems       *pSitem = NULL;
    fileitems       *pDitem = NULL;
    file_syncitem   *pFItem = NULL; 

    pSlist = &pSSlot->filelist[itype];
    pDlist = &pDSlot->filelist[itype];

    pDitem = (fileitems*)pDlist->head;
    
    while(pDitem)
    {
        pDitem->iFlag |= FILEM_OPTFLG_DELETE;
       
        pDitem = (fileitems*)pDitem->sNode.next;
    }    
    
    pSitem = (fileitems*)pSlist->head;
    
    while(pSitem)
    {
        pDitem = filem_sync_filenamefind(pSitem->bName, pDlist);

        if(pDitem == NULL)
        {
            pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

            memset(pFItem, 0, sizeof(file_syncitem));
           
            pFItem->iSlot   = pDSlot->slot;

            pFItem->iActive = IPC_OPCODE_ADD;
        
            snprintf(pFItem->bLocName, 127, "%s%s", filem_type_pathget(itype), pSitem->bName);

            memcpy(pFItem->bRemName, pFItem->bLocName, FILEM_NAME_MAXLN);

            list_add_bynode(&pDSlot->synclist, &pFItem->sNode);
        }
        else
        {            
            if(memcmp(pSitem->bHmacData, pDitem->bHmacData, FILEM_PAD_MD5_BUSZ) != 0)
            {
                pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

                memset(pFItem, 0, sizeof(file_syncitem));

                pFItem->iSlot   = pDSlot->slot;

                pFItem->iActive = IPC_OPCODE_UPDATE;

                snprintf(pFItem->bLocName, 127, "%s%s", filem_type_pathget(itype), pSitem->bName);

                memcpy(pFItem->bRemName, pFItem->bLocName, FILEM_NAME_MAXLN);

                list_add_bynode(&pDSlot->synclist, &pFItem->sNode);
            }
        }

        pSitem = (fileitems*)pSitem->sNode.next;
    }

    pDitem = (fileitems*)pDlist->head;
    
    while(pDitem)
    {
        if((pDitem->iFlag & FILEM_OPTFLG_DELETE) != 0)
        {
            pDitem->iFlag &= ~FILEM_OPTFLG_DELETE;

            pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

            memset(pFItem, 0, sizeof(file_syncitem));
            
            pFItem->iSlot   = pDSlot->slot;

            pFItem->iActive = IPC_OPCODE_DELETE;

            snprintf(pFItem->bLocName, 127, "%s%s", filem_type_pathget(itype), pDitem->bName);

            memcpy(pFItem->bRemName, pFItem->bLocName, FILEM_NAME_MAXLN);

            list_add_bynode(&pDSlot->synclist, &pFItem->sNode);
        }
        
        pDitem = (fileitems*)pDitem->sNode.next;
    }    
}

/*---------------------------------------------------------*/
/* Name: filem_sync_transcallback                          */
/* 文件传输回调                                            */
/*---------------------------------------------------------*/
static void filem_sync_filetranscbk(int iResult, char *info, void *para, int iFlag) 
{
    if((para != NULL) && iFlag)    /*必须是传输完成后的回调*/
    {
        int            iRetva = 0;
        filemslot     *pSSlot = 0;
        filemslot     *pDSlot = para;
        file_syncitem *pFItem = NULL;

        pSSlot = filem_slot_objectget(0);
        
        if((iResult == 0) && (pSSlot->hastat == HA_ROLE_MASTER))
        {
            if(info != NULL)
            {
                printf("%s\r\n", info);

                zlog_notice("%s\r\n", info);
            }

            pFItem = (file_syncitem *)pDSlot->synclist.head;

            if(pFItem != NULL)
            {
                 list_delete_bynode(&pDSlot->synclist, &pFItem->sNode);
                 
                 if(pFItem->iActive == IPC_OPCODE_DELETE)
                 {
                     iRetva = filem_tran_delfile(0, pDSlot->slot, pFItem->bLocName, 
                                                pFItem->bRemName, filem_sync_filetranscbk, pDSlot);
                 }
                 else
                 {
                     iRetva = filem_tran_putfile(0, pDSlot->slot, pFItem->bLocName, 
                                                 pFItem->bRemName, filem_sync_filetranscbk, pDSlot);
                 }

                 if(iRetva != 0)
                 {
                     pSSlot->issync = 0;
                     pDSlot->issync = 0;

                     filem_show_listclr(&pDSlot->synclist);
                     
                     filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);

                     zlog_debug(FILEM_DEBUG_SYNC,"Sync file list to slot%d fail.%s.name=%s\r\n",
                                pDSlot->slot, filem_sys_errget(iRetva), pFItem->bLocName);

                     zlog_warn("Sync file list to slot%d fail.%s.name=%s\r\n",
                                pDSlot->slot, filem_sys_errget(iRetva), pFItem->bLocName);
                 }
                 
                 XFREE(MTYPE_FILEM_ENTRY, pFItem);
            }
            else
            {
                pSSlot->issync = 0;
                pDSlot->issync = 0;
                
                filem_show_listclr(&pDSlot->synclist);

                filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);

                zlog_debug(FILEM_DEBUG_SYNC,"Sync file list to slot%d successfully.\r\n", pDSlot->slot);
                
                zlog_notice("Sync file list to slot%d successfully.\r\n", pDSlot->slot);
            }
        }
        else
        {                                        /*发生倒换时,在传递下一个文件前终止*/
            pSSlot->issync = 0;
            pDSlot->issync = 0;

            filem_show_listclr(&pDSlot->synclist);

            filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);

            zlog_debug(FILEM_DEBUG_SYNC,"Sync file list to slot%d fail.\r\n", pDSlot->slot);
            
            zlog_warn("Sync file list to slot%d fail.\r\n", pDSlot->slot);
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_filesyncstart                          */
/* 文件同步  主CPU执行                                     */
/*---------------------------------------------------------*/
static void filem_sync_filesyncstart(filemslot *pDSlot) 
{
    filemslot     *pSSlot = NULL; 
    file_syncitem *pFItem = NULL; 
                                               /*执行同步前禁止下载和复制*/
    filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);

    pSSlot = filem_slot_objectget(0);
    
    pSSlot->issync = 1;

    filem_show_listclr(&pDSlot->synclist);
    
    if(pDSlot->confsync > 0)
    {
        char bCfgFile[FILEM_NAME_MAXLN] = {0};
        
        pDSlot->confsync --;                   /*同步至少2次*/

        snprintf(bCfgFile, FILEM_NAME_MAXLN-1, "%s%s", 
                 filem_type_pathget(FILEM_MTYPE_CFG), FILEM_FILE_RUN_CONFIG);

        if(filem_oper_isexist(bCfgFile))
        {
            pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

            memset(pFItem, 0, sizeof(file_syncitem));
     
            pFItem->iSlot   = pDSlot->slot;

            pFItem->iActive = IPC_OPCODE_UPDATE;
            
            strncpy(pFItem->bLocName, bCfgFile, FILEM_NAME_MAXLN-1);
            strncpy(pFItem->bRemName, bCfgFile, FILEM_NAME_MAXLN-1);

            list_add_bynode(&pDSlot->synclist, &pFItem->sNode);
        }

        snprintf(bCfgFile, FILEM_NAME_MAXLN-1, "%s%s", 
                 filem_type_pathget(FILEM_MTYPE_CFG), FILEM_FILE_SDH_CONFIG);

        if(filem_oper_isexist(bCfgFile))
        {
            pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

            memset(pFItem, 0, sizeof(file_syncitem));
     
            pFItem->iSlot   = pDSlot->slot;

            pFItem->iActive = IPC_OPCODE_UPDATE;
            
            strncpy(pFItem->bLocName, bCfgFile, FILEM_NAME_MAXLN-1);
            strncpy(pFItem->bRemName, bCfgFile, FILEM_NAME_MAXLN-1);

            list_add_bynode(&pDSlot->synclist, &pFItem->sNode);
        }
    }

    filem_sync_changecheck(pSSlot);
    
    filem_sync_difffilepull(FILEM_MTYPE_VER, pSSlot, pDSlot);

    if((pDSlot->synclist.count > 0) && !filem_tran_tranisbusy())
    {
        pDSlot->issync = 1;

        filem_sync_filetranscbk(0, NULL, pDSlot, 1);
    }
    else
    {
        pSSlot->issync = 0;
        
        filem_show_listclr(&pDSlot->synclist);

        filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_filesync                               */
/* 文件同步  主CPU执行                                     */
/*---------------------------------------------------------*/
static void filem_sync_filesync(filemslot *pSlot) 
{
                                      /*主CPU数据未加载完,不做文件同步*/
    int        iSlot = 0;
    filemslot *pDSlt = NULL;
    
    for(iSlot = 1; iSlot < FILEM_SLOT_COUN_MAX; iSlot++)
    {
        pDSlt = filem_slot_objectget(iSlot);

        if(pDSlt->rcvtout > 0)        /*备用CPU文件列表报文接收超时*/
        {                 
             if(--pDSlt->rcvtout == 0)
             {
                 if(pDSlt->rcvbuff)
                 {
                     XFREE(MTYPE_FILEM_ENTRY, pDSlt->rcvbuff);

                     pDSlt->rcvbuff   = NULL;
                     pDSlt->rcvserno  = pDSlt->rcvtype   = 0;
                     pDSlt->rcvoffset = pDSlt->rcvtotlen = 0;
                 }
             }
        }

        if((pDSlt != pSlot) && filem_slot_issubcpu(iSlot)) 
        {
             if(pDSlt->gstTimer > 0)
             {
                 if(--pDSlt->gstTimer == 0)
                 {
                      filem_cmd_stateupdateask(iSlot);
                      
                      pDSlt->gstTimer = FILEM_STATEGET_TIMER;
                 }
             }
        }

        if((pDSlt != pSlot) && filem_slot_ismaincpu(iSlot))
        {
            if(pDSlt->syntimer > 0)
            {                             
                 pDSlt->syntimer --;
                                      /*pDSlt->syntimer 收到备用CPU文件目录时启动*/
                                      /*一次只能同步一个槽位, 下一个周期同步下一个槽位*//*实时传输优先*/
                 if((pDSlt->syntimer == 0) && !filem_sync_rttranrun() &&
                     filem_slot_cansync(iSlot) && !filem_tran_tranisbusy())
                 {                     
                     filem_sync_filesyncstart(pDSlt);   /*检查同步文件 启动文件同步              文件同步过程可以北强制终止*/

                     zlog_debug(FILEM_DEBUG_SYNC,"Sync file list start, iSlot%d\r\n", iSlot); 
                 }
            }
        }
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_forcestop                              */
/* Retu: 无                                                 */
/* Desc: 槽位文件同步或CLI传输强制结束                                  */
/*---------------------------------------------------------*/
void filem_sync_forcestop(int iHast)
{
    int  iSlot = 0;
    filemslot *pSlot = NULL;
    
    filem_tran_forcestop(1);
    
	pSlot = filem_slot_objectget(0);

	if(pSlot != NULL) pSlot->issync = 0;

    filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_SYNC);
    
    if(iHast == HA_ROLE_MASTER)
    {
        for(iSlot = 1; iSlot < FILEM_SLOT_COUN_MAX; iSlot++)
        {
            pSlot = filem_slot_objectget(iSlot);

            if(pSlot != NULL)	
            {
                if(pSlot->issync)
                {
					pSlot->issync = 0;

                    filem_show_listclr(&pSlot->synclist);

                    zlog_debug(FILEM_DEBUG_SYNC,"Sync file list force stop. slot%d\r\n", iSlot);
					
                    zlog_notice("Sync file list force stop. slot%d\r\n", iSlot);
                }
            }
       }
   }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_timeout                                */
/* Retu: 无                                                */
/* Desc: 文件同步定时器                                    */
/*---------------------------------------------------------*/
void filem_sync_timeout(void)
{
    filemslot *pSlot = NULL;

    pSlot = filem_slot_objectget(0);
    
    switch(pSlot->hastat)
    {
        case HA_ROLE_SLAVE:

             if(pSlot->sysstat >= HA_BOOTSTA_END) filem_sync_filereport(pSlot);

             break;

        case HA_ROLE_MASTER:

             if(pSlot->sysstat == HA_BOOTSTA_DATAOK) filem_sync_filesync(pSlot);

             break;

        default: break;
    }
}


/*-------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_rtsynccall                             */
/* Retu: 无                                                 */
/* Desc: 文件实时传输结束回调                                        */
/*---------------------------------------------------------*/

static void filem_sync_rtsynccall(int iResult, char *pResInfo, void *pPar, int iFlag)
{
    int            iSNext = 0;
    file_syncitem *pFItem = pPar;

    if((pResInfo == NULL) || (pFItem == NULL))  return;

    if(iFlag == 0)
    {
         pFItem->iTimeout = FILEM_RTTRANS_TOUT; //进度回调 只需重置定时器
         return;
    }

    printf("Realtime trans file, retry=%d, result=%d\r\n", pFItem->iRetry, iResult);
    
    zlog_debug(FILEM_DEBUG_SYNC,"Realtime trans file, retry=%d, result=%d\r\n", pFItem->iRetry, iResult);

    pFItem->iResult = iResult;

    if(iResult != 0)                          /*传输失败*/
    {
         if(pFItem->iRetry <= 1)
              iSNext = 1;                     /*启动下一个请求*/
         else 
         {
              pFItem->iIsRun   = 0;  
              pFItem->iTimeout = 2;           /*2S后重试*/
         }
    }
    else                                      /*传输成功*/
    {
         iSNext = 1;                          /*启动下一个请求*/

         if((pFItem->iActive == IPC_OPCODE_GET) && pFItem->iSynSlave)
         {
             int            iSlvSlt = 0;
             file_syncitem *pNwItem = NULL;
             
             iSlvSlt = filem_slot_slaveslot();

             if(iSlvSlt > 0)
             {
                  pNwItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

                  if(pNwItem)                //启动备用板同步
                  {
                      memset(pNwItem, 0, sizeof(file_syncitem));
                      
                      pNwItem->iUnit    = 1;
                      pNwItem->iSlot    = iSlvSlt;
                      pNwItem->iActive  = IPC_OPCODE_ADD;
                      pNwItem->iRetry   = FILEM_RTTRANS_RETRY;
                      pNwItem->iTimeout = FILEM_RTTRANS_TOUT;
                      
                      strncpy(pNwItem->bLocName, pFItem->bLocName, FILEM_NAME_MAXLN-1);
                      strncpy(pNwItem->bRemName, pFItem->bLocName, FILEM_NAME_MAXLN-1);
                      
                      list_add_bynode(&gFileRtTransQue, &pNwItem->sNode);
                  }
             }
         }
    }
    
         
    if(iSNext)
    {
         //判断发起模块,发送结果通知
         zlog_notice("Realtime trans file, %s! retry=%d, result=%d\r\n", pResInfo, pFItem->iRetry, pFItem->iResult);
         
         list_delete_bynode(&gFileRtTransQue, &pFItem->sNode);

         XFREE(MTYPE_FILEM_ENTRY, pFItem);

         pFItem = (file_syncitem*)gFileRtTransQue.head;

         if(pFItem != NULL) filem_sync_rtsyncstart(pFItem);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_sync_rtsynccall                             */
/* Retu: 无                                                 */
/* Desc: 文件实时传输回调                                          */
/*---------------------------------------------------------*/

static void filem_sync_rtsyncstart(file_syncitem *pFItem)
{
    pFItem->iIsRun = 1;
    
    pFItem->iTimeout = FILEM_RTTRANS_TOUT;       /*90S超时*/
    
    switch(pFItem->iActive)
    {
        case IPC_OPCODE_DELETE:
    
             pFItem->iResult = filem_tran_delfile(0, pFItem->iSlot, pFItem->bLocName, pFItem->bRemName, filem_sync_rtsynccall, pFItem);
             break;
        
        case IPC_OPCODE_ADD:
    
             pFItem->iResult = filem_tran_putfile(0, pFItem->iSlot, pFItem->bLocName, pFItem->bRemName, filem_sync_rtsynccall, pFItem);
             break;               
    
        default: //IPC_OPCODE_GET:
    
             pFItem->iResult = filem_tran_getfile(0, pFItem->iSlot, pFItem->bLocName, pFItem->bRemName, filem_sync_rtsynccall, pFItem);
             break;
    }
    
    if(pFItem->iResult != 0) 
    {
        pFItem->iIsRun = 0;  pFItem->iTimeout = 2;  /*2S后重试*/
    }
    
    printf("Realtime sync file start, %s, result=%d, retry=%d\r\n", pFItem->bLocName, pFItem->iResult, pFItem->iRetry); 
    
    zlog_debug(FILEM_DEBUG_SYNC,"Realtime sync file start, %s, result=%d, retry=%d\r\n", pFItem->bLocName, pFItem->iResult, pFItem->iRetry); 
    
}

/*---------------------------------------------------------*/
/* Name: filem_sync_rtsyncfilter                           */
/* Retu: 1 重复 0 未重复                                        */
/* Desc: 文件实时传输重复请求过滤                                      */
/*---------------------------------------------------------*/

static int filem_sync_rtsyncfilter(char iOpt,
                                   int iSlot,  char *pFile)
{
    file_syncitem *pFItem = NULL;

    pFItem = (file_syncitem*)gFileRtTransQue.head;

    while(pFItem)
    {
        if((pFItem->iSlot == iSlot) && (pFItem->iActive == iOpt) && 
           (strcmp(pFile, pFItem->bLocName) == 0))
        {
            return(1);
        }
        pFItem = (file_syncitem*)pFItem->sNode.next;
    }

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_sync_rtfilesync                             */
/* Retu: 无                                                 */
/* Desc: 文件实时同步请求处理                                        */
/*---------------------------------------------------------*/
int filem_sync_rtfilesync(int iModule, int iSlot, 
                          char iOpt, char iSynSla, char *pFile) 
{
    if(filem_sync_rtsyncfilter(iOpt, iSlot, pFile))
    {
        printf("Realtime trans request repeat slot%d %s\r\n", iSlot, pFile);
        
        zlog_err("Realtime trans request repeat slot%d %s\r\n", iSlot, pFile);
        
        return(-FILEM_ERROR_REQREPEAT);
    }
    else
    {
        file_syncitem *pFItem = NULL;
                                          //过滤重复请求
        pFItem = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(file_syncitem));

        if(pFItem == NULL) return(-FILEM_ERROR_NOMEM);
        else
        {
            memset(pFItem, 0, sizeof(file_syncitem));

            pFItem->iUnit     = 1;
            pFItem->iSlot     = iSlot;
            pFItem->iActive   = iOpt;
            pFItem->iModule   = iModule;
            pFItem->iSynSlave = iSynSla;                 /*后续动作同步到备用CPU*/
            pFItem->iRetry    = FILEM_RTTRANS_RETRY;
            pFItem->iTimeout  = FILEM_RTTRANS_TOUT;      /*设置定时器避免逻辑错误,导致队列锁住*/
            
            strncpy(pFItem->bLocName, pFile, FILEM_NAME_MAXLN-1);
                                                         /*主CPU没有异名传输需求*/
            strncpy(pFItem->bRemName, pFile, FILEM_NAME_MAXLN-1);
            
            list_add_bynode(&gFileRtTransQue, &pFItem->sNode);

            if(gFileRtTransQue.count == 1)               /*当前没有实时传输任务执行, 则马上启动实时传输*/
            {
                 filem_sync_forcestop(HA_ROLE_MASTER);   /*立即停止当前正在执行的定期文件同步*/

                 filem_sync_rtsyncstart(pFItem);         
            }
          //else 如果当前有实时传输任务在执行 则在队列中等待

            zlog_debug(FILEM_DEBUG_SYNC,"Sync realtime sync file add to queue, %s, quecou=%d\r\n", pFile, gFileRtTransQue.count); 
            return(0);
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_sync_rttimeout                              */
/* Retu: 无                                                 */
/* Desc: 文件同步定时器                                           */
/*---------------------------------------------------------*/
void filem_sync_rttimeout(void)
{
    file_syncitem *pFItem = NULL;

    pFItem = (file_syncitem*)gFileRtTransQue.head;
    
    if(pFItem != NULL) 
    {
        if(--pFItem->iTimeout <= 0)
        {            
            if(--pFItem->iRetry > 0) 
            {
                if(pFItem->iIsRun) filem_tran_forcestop(0);  /*该情况应该不会发生*/
                    
                filem_sync_rtsyncstart(pFItem);
            }
            else
            {
                //判断发起模块,发送结果通知
            
                printf("Realtime trans file, %s retry fail, result=%d\r\n", pFItem->bLocName, pFItem->iResult);
                
                zlog_debug(FILEM_DEBUG_SYNC,"Realtime trans file, %s retry fail, result=%d\r\n", pFItem->bLocName, pFItem->iResult);

                zlog_notice("Realtime trans file, %s retry fail, result=%d\r\n", pFItem->bLocName, pFItem->iResult);
            
                list_delete_bynode(&gFileRtTransQue, &pFItem->sNode);
            
                XFREE(MTYPE_FILEM_ENTRY, pFItem);
            
                pFItem = (file_syncitem*)gFileRtTransQue.head;
            
                if(pFItem != NULL) filem_sync_rtsyncstart(pFItem);
            }
        }
    }
}

/*-------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_sync_rtfilerequest                          */
/* Retu: 无                                                 */
/* Desc: 实时传输请求                                            */
/*---------------------------------------------------------*/

int filem_sync_rtfilerequest(char *pData, char iOpt, char iSlvSyn)
{
    int    iret = 0;
    struct ha_batch_req sreq;

    memcpy(&sreq, pData, sizeof(struct ha_batch_req));
                                    
    if(iOpt == IPC_OPCODE_ADD)
    {
        if(filem_oper_isexist(sreq.bFile))
        {
            iret = filem_sync_rtfilesync(0, sreq.slot, iOpt, 0, sreq.bFile);
        }
        else
        {
            iret = -FILEM_ERROR_NOEXIST;
            
            printf("Realtime trans file no exist %s \r\n", sreq.bFile);

            zlog_err("Realtime trans file no exist %s \r\n", sreq.bFile);
        }
    }
    else
    {
        iret = filem_sync_rtfilesync(0, sreq.slot, iOpt, iSlvSyn, sreq.bFile);
    }
        
    printf("File realtime trans request, unit=%d, slot=%d,file=%s\r\n", sreq.unit, sreq.slot, sreq.bFile);

    return(iret);
}

/*-------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_sync_init                                   */
/* 文件同步初始化                                          */
/*---------------------------------------------------------*/
int filem_sync_init(void)
{
  /*filemslot *pSlot = NULL;

    pSlot = filem_slot_objectget(0);

    pSlot->hastat = HA_ROLE_MASTER;
    pSlot->sysstat = HA_BOOTSTA_DATAOK;
    pSlot->mslot   = 2;
 */   
    memset(&gFileRtTransQue, 0, sizeof(struct list));
  
    memset(gFileLableLists, 0, sizeof(struct list) * FILEM_MTYPE_MAX);   

    filem_sync_lableread(FILEM_MTYPE_BIN); 
    
    filem_sync_lableread(FILEM_MTYPE_CFG); 

    filem_sync_lableread(FILEM_MTYPE_DAT); 
                                              /*只做版本文件的自动同步*/
    filem_sync_lableread(FILEM_MTYPE_VER); 

    return(0);                                              
}


/*-------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------*/


