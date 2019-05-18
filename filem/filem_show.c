/* file list show 
   name filem_show.c
   edit suxq
   date 2016/05/23
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>

#include "lib/memory.h"
#include "lib/msg_ipc_n.h"
#include "lib/devm_com.h"
#include "lib/log.h"

#include "filem.h"
#include "filem_img.h"
#include "filem_tran.h"
#include "filem_slot.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*
void filem_show_print(unsigned char *data, int len)
{
    int loop;
    int line = 0;

    for(loop = 0; loop < len; loop++)
    {
        printf("%02x ", data[loop]);

        line ++;

        if(line == 16) 
        {
           line = 0;
           
           printf("\r\n");
        }
    }
}
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_show_listclr                                */
/* 清空列表                                                */
/*---------------------------------------------------------*/
void filem_show_listclr(struct list *plist)
{
    struct listnode  *pnode;
    struct listnode  *pnext;
  
    for(pnode = plist->head; pnode; pnode = pnext)
    {
        pnext = pnode->next;

        XFREE(MTYPE_FILEM_ENTRY, pnode);
    }

    plist->count = 0; 
    
    plist->head  = plist->tail= NULL;
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_show_statread                               */
/* Retu: 无                                                */
/* Desc: 读取文件状态                                      */
/*---------------------------------------------------------*/
static void filem_show_statread(char *pFile, fileitems *pData)
{
     struct stat sTmStat;
     
     if(stat(pFile, &sTmStat) == 0)
     {
          if((sTmStat.st_mode & S_IFLNK) == S_IFLNK)
               pData->iStype = FILEM_STYPE_FLNK;
          else if((sTmStat.st_mode & S_IFSOCK) == S_IFSOCK) 
               pData->iStype = FILEM_STYPE_FSOK;
          else if((sTmStat.st_mode & S_IFBLK) == S_IFBLK) 
               pData->iStype = FILEM_STYPE_FBLK;
          else if((sTmStat.st_mode & S_IFIFO) == S_IFIFO) 
               pData->iStype = FILEM_STYPE_FIFO;
          else if((sTmStat.st_mode & S_IFREG) == S_IFREG) 
               pData->iStype = FILEM_STYPE_FREG;
          else if((sTmStat.st_mode & S_IFCHR) == S_IFCHR) 
               pData->iStype = FILEM_STYPE_FCHR;
          else if((sTmStat.st_mode & S_IFDIR) == S_IFDIR) 
               pData->iStype = FILEM_STYPE_FDIR;
          else pData->iStype = FILEM_STYPE_FUNK;
 
          if(sTmStat.st_mode & S_IRUSR) pData->iPerm |= FILEM_PERM_IRUSR;
          if(sTmStat.st_mode & S_IWUSR) pData->iPerm |= FILEM_PERM_IWUSR;
          if(sTmStat.st_mode & S_IXUSR) pData->iPerm |= FILEM_PERM_IXUSR;
          
          if(sTmStat.st_mode & S_IRGRP) pData->iPerm |= FILEM_PERM_IRGRP;
          if(sTmStat.st_mode & S_IWGRP) pData->iPerm |= FILEM_PERM_IWGRP;
          if(sTmStat.st_mode & S_IXGRP) pData->iPerm |= FILEM_PERM_IXGRP;
          
          if(sTmStat.st_mode & S_IROTH) pData->iPerm |= FILEM_PERM_IROTH;
          if(sTmStat.st_mode & S_IWOTH) pData->iPerm |= FILEM_PERM_IWOTH;
          if(sTmStat.st_mode & S_IXOTH) pData->iPerm |= FILEM_PERM_IXOTH;
    
          pData->iGid     = (int)sTmStat.st_gid;
          pData->iUid     = (int)sTmStat.st_uid;
          pData->iSize    = (int)sTmStat.st_size;
          pData->iBlocks  = (int)sTmStat.st_blocks;
          pData->iBlkSize = (int)sTmStat.st_blksize;
 
          pData->iAccTime = (unsigned long)sTmStat.st_atime;
          pData->iModTime = (unsigned long)sTmStat.st_mtime;
          pData->iChgTime = (unsigned long)sTmStat.st_ctime;
     }
     else
     {
          zlog_debug(FILEM_DEBUG_SYNC, "Read file %s infor error\r\n", pFile);
     }
}


/*---------------------------------------------------------*/
/* Name: filem_show_dirlist                                */
/* Retu: 无                                                */
/* Desc: 读取目录下文件信息                                */
/*---------------------------------------------------------*/
int filem_show_dirlist(int iType, struct list *pList, char *pDir, int iGetst)
{
    int               lIndCou = 0;
    DIR              *pDirDir = NULL;
    struct dirent    *pDirEnt = NULL;       
    fileitems        *pFileDt = NULL;
    char bTpName[FILEM_NAME_MAXLN] = {0};
    
    filem_show_listclr(pList);

    pDirDir = opendir(pDir);

    if(pDirDir != NULL)
    {
         do
         {
             pDirEnt = readdir(pDirDir);
             
             if(pDirEnt != NULL)   
             {
                 if(pDirEnt->d_type != DT_DIR) //不处理子目录
                 {
                     if((strcmp(pDirEnt->d_name, "." ) != 0) &&
                        (strcmp(pDirEnt->d_name, "..") != 0) &&
                        (strcmp(pDirEnt->d_name, FILEM_FILE_CHANGE_LABLE) != 0))
                     {
                         pFileDt = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(fileitems));
 
                         if(pFileDt != NULL)
                         {
                             lIndCou++;
 
                             memset(pFileDt, 0, sizeof(fileitems));
 
                             pFileDt->iMtype = iType;
                             pFileDt->iIndex = lIndCou;
 
                             strncpy(pFileDt->bName, pDirEnt->d_name, FILEM_NAME_MAXLN-1);

                             if(iGetst)
                             {
                                 memset(bTpName, 0, FILEM_NAME_MAXLN);
                                 
                                 snprintf(bTpName, (FILEM_NAME_MAXLN - 1), "%s%s", pDir, pFileDt->bName);
                                 
                                 filem_show_statread(bTpName, pFileDt);
                             }
                             
                             list_add_bynode(pList, &pFileDt->sNode);
                         }
                     }
                 }
             }
             
         }while(pDirEnt != NULL);
 
         closedir(pDirDir);
 
         return(lIndCou);
    }
    else return(-FILEM_ERROR_PATH);
}


/*---------------------------------------------------------*/
/* Name: filem_show_refresh                                */
/* Retu: 无                                                */
/* Desc: 本槽刷新文件列表 频率过滤刷新                     */
/*---------------------------------------------------------*/
static int filem_show_refresh(int iType, struct list *pList, char *pInDir, int iTout)
{
    int iFrsh = 0;
    int iPLen = 0;
    
    switch(iType)
    {
        case FILEM_MTYPE_BIN:
        case FILEM_MTYPE_CFG:
        case FILEM_MTYPE_DAT:
        case FILEM_MTYPE_VER:

             if(gfiletypesman[iType].iTimeOut <= 0) iFrsh = 1;
             break;

        case FILEM_MTYPE_NON:

             if(pInDir != NULL) iPLen = strlen(pInDir);

             if(iPLen > 0)
             {
                 iFrsh = 1;

                 if(pInDir[iPLen-1] == '/')
                 {
                     sprintf(gfiletypesman[iType].bPrePath, "%s", pInDir);
                 }
                 else 
                 {
                     sprintf(gfiletypesman[iType].bPrePath, "%s/", pInDir);
                 }
             }
             break;
             
        default: break;
    }

    if(iFrsh)
    {
         gfiletypesman[iType].iTimeOut = iTout; 
         
         return(filem_show_dirlist(iType, pList, gfiletypesman[iType].bPrePath, 1));
    }
    else return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_show_timeout                                */
/* Retu: 无                                                */
/* Desc: 文件列表刷新超时                                  */
/*---------------------------------------------------------*/
void filem_show_timeout(void)
{
    int iType = FILEM_MTYPE_BIN;

    for(;iType < FILEM_MTYPE_TMP; iType++)
    {
        if(gfiletypesman[iType].iTimeOut > 0)
        {
           gfiletypesman[iType].iTimeOut --;
        }
    }
}


/*---------------------------------------------------------*/
/* Name: filem_show_init                                   */
/* Retu: 无                                                */
/* Desc: 文件列初始化                                      */
/*---------------------------------------------------------*/
/*
int filem_show_init(void)
{       
    return(0);
}
*/

/*---------------------------------------------------------*/
/* Name: filem_show_timetostr                              */
/* Retu: 无                                                */
/* Desc: 把日期转换成字符串                                */
/*---------------------------------------------------------*/
void filem_show_timetostr(time_t iSecond, char *bDat)
{
    struct tm sDate;

    localtime_r(&iSecond, &sDate);

    sprintf(bDat, "%d/%02d/%02d %02d:%02d:%02d",sDate.tm_year + 1900, 
                  sDate.tm_mon + 1, sDate.tm_mday, sDate.tm_hour, sDate.tm_min, sDate.tm_sec);
}


/*---------------------------------------------------------*/
/* Name: filem_show_filelist                               */
/* Retu: 无                                                */
/* Desc: 显示文件列表信息                                  */
/*---------------------------------------------------------*/
int filem_show_filelist(int iSlot, int iType, char *pPath, char **bBufDat)
{
    int              iAllSiz  =  0;
    int              iAllLen  = -1;
    char             bDatStr[64] = {0};
    char            *pFileInf = NULL;
    fileitems       *pFileItm = NULL;
    struct list     *pFileLst = NULL;
    struct listnode *pFileNod = NULL;
    const char      *bTypeStr[] = {"u", "f", "c", "d", "b", "-", "l", "s"};

    if((iType < FILEM_MTYPE_NON) || (iType >= FILEM_MTYPE_TMP)) 
    {
        return(-FILEM_ERROR_UNKTYPE);
    }
    
    pFileLst = filem_slot_filelist(iSlot, iType);
    
    if(pFileLst == NULL) return(-FILEM_ERROR_SLOTERROR);
                                             /*显示本槽要先刷新*/        
    if((iSlot == 0) || (iSlot == gfilelocslot)) 
    {
        iAllLen = filem_show_refresh(iType, pFileLst, pPath, 0); 

        if(iAllLen < 0) return(iAllLen);
    }

    pFileInf = XMALLOC(MTYPE_FILEM_ENTRY, 128 * (pFileLst->count + 200));

    if(pFileInf == NULL) return(-FILEM_ERROR_NOMEM);

    iAllLen = 0;
    
    if(pFileLst->count <= 0)
    {
        iAllLen += sprintf(pFileInf + iAllLen, "no files\r\n");

       *bBufDat = pFileInf;  
       
        return(iAllLen);
    }
    else        
    {
        iAllLen += sprintf(pFileInf + iAllLen, "No. Permission      Size DateTime            Name                          \r\n" \
                                               "--- ---------- --------- ------------------- ------------------------------\r\n");
    }
   
    pFileNod = pFileLst->head;
    
    while(pFileNod)
    {
        pFileItm = (fileitems*)pFileNod;
        
        iAllSiz += pFileItm->iSize;
        
        filem_show_timetostr(pFileItm->iModTime, bDatStr);
      
        iAllLen += sprintf(pFileInf + iAllLen, "%3d ", pFileItm->iIndex);
            
        iAllLen += sprintf(pFileInf + iAllLen, "%s", bTypeStr[pFileItm->iStype%8]);

        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IRUSR) ? "r" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IWUSR) ? "w" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IXUSR) ? "x" : "-"));


        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IRGRP) ? "r" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IWGRP) ? "w" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IXGRP) ? "x" : "-"));
        
 
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IROTH) ? "r" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IWOTH) ? "w" : "-"));
        iAllLen += sprintf(pFileInf + iAllLen, "%s", ((pFileItm->iPerm & FILEM_PERM_IXOTH) ? "x" : "-"));

        
        iAllLen += sprintf(pFileInf + iAllLen, " %9d %s %s\r\n", pFileItm->iSize, bDatStr, pFileItm->bName);

        pFileNod = pFileNod->next;
    }
           
    iAllLen += sprintf(pFileInf + iAllLen, "--- ---------- --------- ------------------- ------------------------------\r\n"
                                           "Total file %d, size = %d\r\n", pFileLst->count, iAllSiz);
   *bBufDat = pFileInf;  
   
    return(iAllLen);
}

/*---------------------------------------------------------*/
/* Name: filem_show_byindex                                */
/* Retu: 无                                                */
/* Desc: 只显示主板                                        */
/*---------------------------------------------------------*/
int filem_show_byindex(filemsnmp *pFileBuf, unsigned int index, int exact)
{
    unsigned int     iFileNum = 0;
    unsigned int     iType    = 0;
    struct list     *pFileCfg = NULL;
    struct list     *pFileDat = NULL;
    struct list     *pFileVer = NULL;
    struct listnode *pFileNod = NULL;
    fileitems       *pFileItm = NULL;


    pFileCfg = filem_slot_filelist(0, FILEM_MTYPE_CFG);
    pFileDat = filem_slot_filelist(0, FILEM_MTYPE_DAT);
    pFileVer = filem_slot_filelist(0, FILEM_MTYPE_VER);

    if(!pFileCfg || !pFileDat || !pFileVer)  return(-1);
    
    filem_show_refresh(FILEM_MTYPE_CFG, pFileCfg, NULL, 10);
    filem_show_refresh(FILEM_MTYPE_DAT, pFileDat, NULL, 10);
    filem_show_refresh(FILEM_MTYPE_VER, pFileVer, NULL, 10);

    if(!exact) index = (index <= 0) ? 1 : (index + 1);

    if((pFileCfg->count + pFileDat->count + pFileVer->count) < index)
    {
        return -1;
    }

    iFileNum = index;

    if(iFileNum <= pFileCfg->count)
    {
        iType = 1;
        
        pFileNod = pFileCfg->head;
    }
    else
    {
        iFileNum = iFileNum - pFileCfg->count;
        
        if(iFileNum <= pFileDat->count)
        {
            iType = 2;
            
            pFileNod = pFileDat->head;
        }
        else
        {
            iType = 3;
            
            iFileNum = iFileNum - pFileDat->count;
            
            pFileNod = pFileVer->head;
        }
    }

    while(pFileNod && (--iFileNum > 0)) pFileNod = pFileNod->next;

    if(pFileNod)
    {
         pFileItm = (fileitems*)pFileNod;
 
         pFileBuf->iIndex   = index;
         pFileBuf->bType    = iType;
         pFileBuf->iSize    = pFileItm->iSize;
         pFileBuf->iModTime = pFileItm->iModTime;
         
         strcpy(pFileBuf->bName, pFileItm->bName);

         return(0);
    }
    else return(-1);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

