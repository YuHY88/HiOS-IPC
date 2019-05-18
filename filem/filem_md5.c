/* file process command 
   name filem_md5.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include "lib/types.h"
#include "lib/memory.h"
#include "lib/md5.h"

#include "filem.h"
#include "filem_md5.h"
#include "filem_oper.h"


/*---------------------------------------------------------*/
/* Name: filem_md5_pad_getbyfd                             */
/* Para: iSize MTD 传人正确长度, 文件必须传人0               */
/* Desc: 通过文件描述符获取md5                              */
/*---------------------------------------------------------*/
static int filem_md5_pad_getbyfd(int fd, filepadinf *pPad, int iSize)
{
    int iRdLn = 0;

    if(iSize >= FILEM_PAD_INF_SIZE)  
    {
        int *pFlen = NULL;

        int  iret = 0;
        
        iret = filem_oper_seek(fd, iSize-FILEM_PAD_INF_SIZE, SEEK_SET);

        
      //printf("3, iret=%d errno=%d\r\n", iret, errno);
     
        iRdLn = filem_oper_read(fd, (char *)pPad, FILEM_PAD_INF_SIZE);

        pFlen = (int*)pPad->len;              /*文件中包含原始长度不包括填充*/

       *pFlen = iSize;                        /*替换为填充以后的长度*/
    }

  //printf("2, fd=%d  size=%d, iRdLn=%d\r\n", fd, iSize, iRdLn);
     
    
    return((iRdLn == FILEM_PAD_INF_SIZE) ? 0 : -FILEM_ERROR_MD5PADERR);
}


/*---------------------------------------------------------*/
/* Name: filem_md5_pad_getbyname                            */
/* Para: iFlen MTD 传人正确长度, 文件必须传人0                          */
/* Desc: 通过文件名获取 md5                                */
/*---------------------------------------------------------*/
int filem_md5_pad_getbyname(char *pFile, filepadinf *pPad, int iFlen)
{
    int iFd   = 0;
    int iRetv = 0;

    if(pFile == NULL) 
    {
        return(-FILEM_ERROR_NAME);
    }
    
    iFd = filem_oper_open(pFile, O_RDONLY, 0);

    if(iFd >= 0) 
    {         
         if(iFlen == 0) 
         {
             iFlen = filem_oper_size_getf(iFd);
         }

         iRetv = filem_md5_pad_getbyfd(iFd, pPad, iFlen);
         
         filem_oper_close(iFd);
    }
    else 
    {
        iRetv = -FILEM_ERROR_OPEN;
    }
    
    return(iRetv);
}


/*---------------------------------------------------------*/
/* Name: filem_md5_calculate                               */
/* Desc: 计算文件的 md5                                    */
/*---------------------------------------------------------*/
int filem_md5_calculate(int iFd, int iBlkLen, char *bMd5, int iFileLn)
{
    int            iReadLn = 0;
    int            iChkLen = 0;
    int            iBufLen = 64 * 1024;
    md5_ctxt       sCtx;
    char          *bBuffer = NULL;
    unsigned char  bDesDat[16] = {0};
        
    bBuffer = XMALLOC(MTYPE_FILEM_ENTRY, iBufLen + 1024);

    if(bBuffer == NULL) return(-FILEM_ERROR_NOMEM);
    
    MD5Init(&sCtx);
    
    filem_oper_seek(iFd, 0, SEEK_SET);

    iChkLen = iBlkLen;
    
    while(iChkLen > 0)
    {
         if(iChkLen > iBufLen) 
              iReadLn = iBufLen;
         else iReadLn = iChkLen;
         
         if(iReadLn > 0)
         {
             iReadLn = filem_oper_read(iFd, bBuffer, iReadLn);

             if(iReadLn <= 0) break;

             MD5Update(&sCtx, bBuffer, iReadLn);
         }

         iChkLen -= iReadLn;
    }

    if(iFileLn >= 2 * iBlkLen)   /*文件尾部分*/
    {
        filem_oper_seek(iFd, -iBlkLen, SEEK_END);

        iChkLen = iBlkLen;
        
        while(iChkLen > 0)
        {
             if(iChkLen > iBufLen) 
                  iReadLn = iBufLen;
             else iReadLn = iChkLen;
             
             if(iReadLn > 0)
             {
                 iReadLn = filem_oper_read(iFd, bBuffer, iReadLn);

                 if(iReadLn <= 0) break;

                 MD5Update(&sCtx, bBuffer, iReadLn);
             }

             iChkLen -= iReadLn;
        }
    }

    MD5Final(bDesDat, &sCtx);

    XFREE(MTYPE_FILEM_ENTRY, bBuffer);

    for(iReadLn = 0; iReadLn < FILEM_PAD_MD5_MARK; iReadLn++)
    {
        sprintf((bMd5 + iReadLn*2),     "%x", ((bDesDat[iReadLn] >> 4) & 0x0f));
        sprintf((bMd5 + iReadLn*2 + 1), "%x", ((bDesDat[iReadLn] >> 0) & 0x0f));
    }
    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_md5_integritychk                            */
/* Desc: 文件完整性校验                                           */
/*---------------------------------------------------------*/
int filem_md5_integritychk(char *pFile)
{
    int  iFd = -1;

    iFd = filem_oper_open(pFile, O_RDONLY, 0);

    if(iFd < 0) return(-FILEM_ERROR_OPEN);
    
    if(filem_oper_size_getf(iFd) < FILEM_PAD_INF_SIZE)  
    {
        filem_oper_close(iFd);  
        return(-FILEM_ERROR_LENGTH);
    }
    else
    {
        filepadinf sPadDat;
        
        memset(&sPadDat, 0, sizeof(filepadinf));

        filem_oper_seek(iFd, -FILEM_PAD_INF_SIZE, SEEK_END);

        filem_oper_read(iFd, (char *)&sPadDat, FILEM_PAD_INF_SIZE);

        filem_oper_close(iFd); 

        if(strncmp(sPadDat.flags, FILEM_PAD_INF_FLAG_DATA, FILEM_PAD_INF_FLAG_LEN) == 0)      
             return(0);      
        else return(-FILEM_ERROR_MD5PADERR);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_md5_matchcheck                              */
/* Desc: md5 匹配校验                                      */
/*---------------------------------------------------------*/
int filem_md5_matchcheck(char *pFile, int iFLn)
{
    int        iFd  = -1;
    int        iRev =  0;
    filepadinf sPadDat;
    char       bMd5[64];

    iFd = filem_oper_open(pFile, O_RDONLY, 0);

//  printf("1, file = %s fd=%d  fln=%d\r\n", pFile, iFd, iFLn);
    
    if(iFd < 0) return(-FILEM_ERROR_OPEN);
    
    if(iFLn == 0) iFLn = filem_oper_size_getf(iFd);

    if(iFLn < FILEM_PAD_INF_SIZE) //小于128字节的不做MD5检查
    {
        filem_oper_close(iFd);  
        return(0);
    }

    if(filem_md5_pad_getbyfd(iFd, &sPadDat, iFLn) != 0)
    {
        filem_oper_close(iFd); 
        
        return(-FILEM_ERROR_MD5PADERR);
    }

    if(strncmp(sPadDat.flags, FILEM_PAD_INF_FLAG_DATA, FILEM_PAD_INF_FLAG_LEN) != 0)
    {
        filem_oper_close(iFd); 
        return(0); //无MD5信息的不检查
    }
    
    iRev = filem_md5_calculate(iFd, iFLn-FILEM_PAD_INF_SIZE, bMd5, iFLn-FILEM_PAD_INF_SIZE);

    filem_oper_close(iFd); 

    if(iRev == 0)
    {
        if(memcmp(sPadDat.md5, bMd5, FILEM_PAD_INF_MD5_LEN) != 0) 
        {
            iRev = -FILEM_ERROR_MD5CHKERR;
        }
    }
    
    return(iRev);
}


/*---------------------------------------------------------*/
/* Name: filem_md5_wholefile                               */
/* Desc: md5 一个完整的文件                                       */
/*---------------------------------------------------------*/
int filem_md5_wholefile(char *pFile, char *bOut, int iBsize)
{
    int  iFd  = -1;
    int  iRev =  0;
    int  iFLn =  0;

    iFd = filem_oper_open(pFile, O_RDONLY, 0);
    
    if(iFd < 0)
        return(-FILEM_ERROR_OPEN);
    else
    {
        iFLn = filem_oper_size_getf(iFd);

        iRev = filem_md5_calculate(iFd, iFLn, bOut, iFLn);

        filem_oper_close(iFd); 

        return(iRev);
    }
}



/*---------------------------------------------------------*/
/* Name: filem_md5_filepadinf                              */
/* Desc: md5 标签信息显示                                  */
/*---------------------------------------------------------*/
int filem_md5_filepadinf(char *pFile, char **pInfo)
{
    int   iRetv = 0;
    int   iALen = 0;
    int   iSize = 0;
    char *bTbuf = NULL;
    char  bDisp[128] = {0};
    char  bTime[6][8] = {{0}};
    char  bName[FILEM_NAME_MAXLN] = {0};

    filepadinf sPadInf = {{0}};
    
    if(pFile == NULL) return(-FILEM_ERROR_NAME);
    
    memset( bDisp, 0, 128);
    memset( bTime, 0, sizeof(bTime));
    memset(&sPadInf, 0, sizeof(filepadinf));

    sprintf(bName, "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);
    
    iRetv = filem_md5_pad_getbyname(bName, &sPadInf, 0);

    if(iRetv < 0) return(iRetv);
    
    bTbuf = XMALLOC(MTYPE_FILEM_ENTRY, 1024);

    if(bTbuf == NULL) 
    {
        return(-FILEM_ERROR_NOMEM);
    }
    
    if(strncmp(sPadInf.flags, FILEM_PAD_INF_FLAG_DATA, FILEM_PAD_INF_FLAG_LEN) != 0)
    {
        iALen += sprintf(bTbuf + iALen, "File %s no MD5 Info:\r\n", pFile);
    }
    else
    {
        iSize  = *((int*)sPadInf.len);
            
        iALen += sprintf(bTbuf + iALen, "File    : %s\r\n", pFile);

        iALen += sprintf(bTbuf + iALen, "Size    : %d\r\n", iSize);

        strncpy(bDisp, sPadInf.ver, FILEM_PAD_INF_VER_LEN);

        bDisp[FILEM_PAD_INF_VER_LEN] = 0;
        
        iALen += sprintf(bTbuf + iALen, "Version : %s\r\n", bDisp);

        strncpy(bTime[0], sPadInf.date,    4);
        strncpy(bTime[1], sPadInf.date+ 4, 2);
        strncpy(bTime[2], sPadInf.date+ 6, 2);
        strncpy(bTime[3], sPadInf.date+ 9, 2);
        strncpy(bTime[4], sPadInf.date+11, 2);
        strncpy(bTime[5], sPadInf.date+13, 2);

        iALen += sprintf(bTbuf + iALen, "Time    : %s/%s/%s %s:%s:%s\r\n", 
                         bTime[0], bTime[1], bTime[2], bTime[3], bTime[4], bTime[5]);

        strncpy(bDisp, sPadInf.md5, FILEM_PAD_INF_MD5_LEN);

        bDisp[FILEM_PAD_INF_MD5_LEN] = 0;

        iALen += sprintf(bTbuf + iALen, "MD5     : %s\r\n", bDisp);
    }
    
   *pInfo  = bTbuf;  

    return(iALen);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

