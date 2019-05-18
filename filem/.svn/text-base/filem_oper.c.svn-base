/* file operate 
   name filem_oper.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include "compiler.h"
#include <mtd/mtd-user.h>

#include "lib/memory.h"
#include "lib/log.h"

#include "filem.h"
#include "filem_oper.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_oper_percent                                */
/* Desc: 百分比计算                                        */
/*---------------------------------------------------------*/
int filem_oper_percent(int iVa, int iVb, int iPrec)
{
    int iResu = 0;
    int iLeft = 0;
    
    iPrec += 2;

    iResu  = iVa/iVb;
    iLeft  = iVa%iVb;

    while(iPrec-- > 0)
    {
        iLeft *= 10;

        iResu  = iResu * 10 + iLeft / iVb;
        
        iLeft  = iLeft % iVb;
    }
        
    return(iResu);
}
/*---------------------------------------------------------*/
/* Name: filem_oper_str2low                                */
/* Desc: 字符串全部转换成小写                              */
/*---------------------------------------------------------*/
int filem_oper_str2low(char *src, int dessz, char *dest)
{
    int idx;
  
    if(!src || !dest) 
    {
        return(-1);
    }
    
    if(strlen(src) > (unsigned int)(dessz - 1)) 
    {
        return(-1);
    }
    
    dessz = strlen(src);
  
    for(idx = 0; idx <dessz; idx++) 
    {
        dest[idx] = tolower (src[idx]);
    }
    dest[idx] = '\0';
  
    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_namelist                               */
/* Retu: 成功文件数                                        */
/* Desc: 读取路径下文件名列表                              */
/*---------------------------------------------------------*/
int filem_oper_namelist(char *pPath, char *pfilter, 
                        int  iBufLn, char *pNamedat)
{
    int               iCounts = 0;
    int               iDatPos = 0;
    DIR              *pDirDir = NULL;
    struct dirent    *pDirEnt = NULL;       

    memset(pNamedat, 0, iBufLn);

    pDirDir = opendir(pPath);

    if(pDirDir == NULL) return(0);
  
    do
    {
        pDirEnt = readdir(pDirDir);
        
        if(pDirEnt != NULL)   
        {
            if(pDirEnt->d_type != DT_DIR) //不处理子目录
            {
                if((strcmp(pDirEnt->d_name, "." ) != 0) &&
                   (strcmp(pDirEnt->d_name, "..") != 0))
                {
                    if(strlen(pDirEnt->d_name) < FILEM_NAME_MAXLN)
                    {
                         if(pfilter == NULL)
                         {
                             iCounts ++;
                             
                             strcpy(pNamedat + iDatPos, pDirEnt->d_name);
     
                             iDatPos += strlen(pDirEnt->d_name) + 1;
                         }
                         else
                         {
                             if(strstr(pDirEnt->d_name, pfilter) != NULL)
                             {
                                iCounts ++;
                                
                                strcpy(pNamedat + iDatPos, pDirEnt->d_name);
        
                                iDatPos += strlen(pDirEnt->d_name) + 1;
                             }
                         }
                         
                         if((iDatPos + FILEM_NAME_MAXLN) > iBufLn) break;
                     }
                 }               
            }
        }
        
    }while(pDirEnt != NULL);

    closedir(pDirDir);

    return(iCounts);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_nameparse                              */
/* Retu: 成功返回字段数                                    */
/* Desc: 文件名分段解析                                    */
/*---------------------------------------------------------*/
int filem_oper_nameparse(char *pName, char **pfield, int iCou, char bsp)
{
    int   index  = 0;
    int   iItmLn = 0;
    char *pPosCr = pName;

    while(1)
    {        /*第一个分割符北忽略,连续分割符看作一个*/
        if((*pPosCr == '\0') || (*pPosCr == bsp))
        {
            if(*pPosCr == '\0') break;

           *pPosCr = '\0';
           
            iItmLn = 0; 
            
            if(index >= iCou) break;
        }
        else
        {
            if(iItmLn == 0)
            {
                pfield[index] = pPosCr; index++;
            }

            iItmLn ++;
        }
        
        pPosCr ++;        
    }

    return(index);
}

/*---------------------------------------------------------*/
/* Name: filem_oper_mkdir                                  */
/* Retu: 成功返回1 失败返回0                               */
/* Desc: 创建目录                                          */
/*---------------------------------------------------------*/
int filem_oper_mkdir(char *pPath)
{
    DIR *pDir = NULL;

    pDir = opendir(pPath);

    if(pDir != NULL)
    {
        closedir(pDir);

        return(0);
    }
    else
    {
        return(mkdir(pPath, 0777));
    }
}

/*---------------------------------------------------------*/
/* Name: filem_oper_IsExist                                */
/* Retu: 成功返回1 失败返回0                               */
/* Desc: 文件存在检查                                      */
/*---------------------------------------------------------*/
int filem_oper_isexist(char *pFile)
{
    struct stat sTmStat;
   
    if(stat(pFile, &sTmStat) == 0)
         return(1);
    else return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_oper_isdir                                  */
/* Retu: TRUE返回1 FALSE返回0                              */
/* Desc: 是否为文件目录检查                                */
/*---------------------------------------------------------*/

int filem_oper_isdir(char *pFile)
{
    struct stat sTmStat;
   
    if(stat(pFile, &sTmStat) == 0)
    {
        if(S_ISDIR(sTmStat.st_mode))
        {
          return(1);
        }
    }

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_oper_size_get                               */
/* Retu: 成功返回1 失败返回-1                              */
/* Desc: 获取文件大小                                      */
/*---------------------------------------------------------*/
int filem_oper_size_get(char *pFile)
{
  struct stat sTmStat;

  if(stat(pFile, &sTmStat) == 0)
       return((int)sTmStat.st_size);
  else return(-1);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_size_getf                               */
/* Retu: 成功返回1 失败返回-1                              */
/* Desc: 获取文件大小                                      */
/*---------------------------------------------------------*/
int filem_oper_size_getf(int iFd)
{
     struct stat sTmStat;
    
     if(fstat(iFd, &sTmStat) == 0)
          return((int)sTmStat.st_size);
     else return(-1);
}

/*---------------------------------------------------------*/
/* Name: filem_oper_sync                                   */
/* Retu: 成功返回0 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_syncf(int iFd)
{
     if(iFd < 0) 
          return(-1);
     else return(fsync(iFd));
}

/*---------------------------------------------------------*/
/* Name: filem_oper_flush                                  */
/* Retu: 成功返回0 失败返回-1                              */
/*---------------------------------------------------------*/
void filem_oper_flush(void)
{
    system("sync &");
}

/*---------------------------------------------------------*/
/* Name: filem_oper_seek                                   */
/* Retu: 成功返回0 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_seek(int iFd, int iOff, int iWhen)
{
     if(iFd < 0) 
          return(-1);
     else return(lseek(iFd, iOff, iWhen));
}


/*---------------------------------------------------------*/
/* Name: filem_oper_read                                   */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_read(int iFd, char *pBuff, int iLen)
{
     if((iFd < 0) || (pBuff== NULL)) 
          return(-1);
     else return(read(iFd, pBuff, iLen));
}

/*---------------------------------------------------------*/
/* Name: filem_oper_read                                   */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_write(int iFd, char *pBuff, int iLen)
{
     if((iFd < 0) || (pBuff== NULL)) 
          return(-1);
     else return(write(iFd, pBuff, iLen));
}


/*---------------------------------------------------------*/
/* Name: filem_oper_read                                   */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_close(int iFd)
{
     if(iFd < 0) 
          return(-1);
     else return(close(iFd));
}


/*---------------------------------------------------------*/
/* Name: filem_oper_erase                                  */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_erase(int iFd, int istart, int len)
{
    int          count;
    mtd_info_t   mtdinf;
    erase_info_t erase;

    if((len <= 0) || (istart < 0)) return(-1);

    memset(&mtdinf, 0, sizeof(mtd_info_t));

    memset(&erase, 0, sizeof(erase_info_t));

    if(ioctl(iFd, MEMGETINFO, &mtdinf) != 0) return(-1);

    if(mtdinf.erasesize == 0) return(-1);
  
    count = len / mtdinf.erasesize;

    if(count == 0) count = 1;

    printf("erase mtd start=%d, len=%d, erasesize=%d, coun=%d\n", istart, len, mtdinf.erasesize, count);
      
    while(count-- > 0)
    {
        erase.start  = istart;
        erase.length = mtdinf.erasesize;
        
        if(ioctl(iFd, MEMERASE, &erase) != 0) return(-1);

        istart +=  mtdinf.erasesize;
    }

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_sectorsize_get                         */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_sectorsize_get(int iFd)
{
    mtd_info_t mtdinf;

    memset(&mtdinf, 0, sizeof(mtd_info_t));

    if(ioctl(iFd, MEMGETINFO, &mtdinf) != 0) return(2048);
  
    if(mtdinf.erasesize < 2048) return(2048);
  
    return(mtdinf.erasesize);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_open                                   */
/* Retu: 成功返回n 失败返回-1                              */
/*---------------------------------------------------------*/
int filem_oper_open(char *pFile, int iFlag, int iMode)
{
  if(pFile == NULL) 
       return(-1);
  else
  {
      if(strlen(pFile) > 0)
           return(open(pFile, iFlag, iMode));
      else return(-1);
  }
}


/*---------------------------------------------------------*/
/* Name: filem_oper_namechk                                */
/* Retu: 成功返回0 失败返回-X                              */
/* Desc: 文件名有效检查                                    */
/*---------------------------------------------------------*/
int filem_oper_namechk(char *pFile)
{
    int iLen = 0;
    int iIdx = 0;
    
    if(pFile == NULL) return(-1);

    iLen = strlen(pFile);
    
    if((iLen <= 0) || (iLen >= FILEM_NAME_MAXLN)) return(-1);
  
    for(iIdx = 0; iIdx < iLen; iIdx++)
    {
        if(((pFile[iIdx] >= 'A') && (pFile[iIdx] <= 'Z')) ||
           ((pFile[iIdx] >= 'a') && (pFile[iIdx] <= 'z')) ||
           ((pFile[iIdx] >= '0') && (pFile[iIdx] <= '9')) ||
           ((pFile[iIdx] >= '#') && (pFile[iIdx] <= ')')) ||
           ((pFile[iIdx] >= '+') && (pFile[iIdx] <= '.')) ||
            (pFile[iIdx] == '=') || (pFile[iIdx] == '_')  || (pFile[iIdx] == '/'))
        {
             continue;
        }
        else 
        {
             return(-1);
        }
    }

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_cutpath                                */
/* Retu: 成功返回0 失败返回-X                              */
/* Desc: 截取路径部分                                      */
/*---------------------------------------------------------*/
int filem_oper_cutpath(char *pFile, char *pPath)
{
    if((pFile != NULL) && (pPath != NULL)) 
    {
         int  iCpLen = 0;

         iCpLen = strlen(pFile);

         if(iCpLen <= 0) 
         {
             pPath[0] = 0; return(-1);
         }

         while(--iCpLen >= 0)
         { 
             if((pFile[iCpLen] == '/') || (pFile[iCpLen] == '\\')) break;
         }

         if(iCpLen < 0) pPath[0] = 0;
         else
         {
             memcpy(pPath, pFile, iCpLen + 1);
             
             pPath[iCpLen + 1] = 0;
         }
         
         return(0);
    }
    else 
    {
         if(pPath)  pPath[0] = 0; 
         return(-1);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_oper_cutname                                */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 截取文件名部分                                    */
/*---------------------------------------------------------*/
int filem_oper_cutname(char *pFile, char *pSname)
{
    if((pFile != NULL) && (pSname != NULL))  
    {
         int  iCpLen = 0;

         iCpLen = strlen(pFile);

         if(iCpLen <= 0) 
         {
             pSname[0] = 0; 
             return(-1);
         }

         while(--iCpLen >= 0)
         { 
             if((pFile[iCpLen] == '/') || (pFile[iCpLen] == '\\')) 
             {
                 break;
             }
         }

         if(iCpLen < 0)
         {
              strcpy(pSname, pFile);
         }
         else 
         {
              strcpy(pSname, pFile + iCpLen + 1);
         }
     
         return(0);
    }
    else 
    {
        if(pSname) pSname[0] = 0; 
         return(-1);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_oper_pathchk                                */
/* Retu: 成功返回0 失败返回-X                              */
/* Desc: 路径名有效检查(检查全路径)                        */
/*---------------------------------------------------------*/
int filem_oper_pathchk(char *pPath)
{
    int  iLen = 0;
        
    iLen = strlen(pPath);
    
    if((iLen <= 0) || (iLen >= FILEM_NAME_MAXLN)) 
    {
        return(-1);
    }
  
    if((pPath[0] != '/') && (pPath[0] != '\\'))  
    {
        return(-1);
    }
  
    if((pPath[iLen - 1] != '/') && (pPath[iLen - 1] != '\\')) 
    {
        return(-1);
    }
  
    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_oper_remove                                 */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 删除文件                                          */
/*---------------------------------------------------------*/
int filem_oper_remove(char *pFile)
{
     if(pFile == NULL)
          return(-1);
     else return(remove(pFile));
}


/*---------------------------------------------------------*/
/* Name: filem_oper_rename                                 */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 文件改名                                          */
/*---------------------------------------------------------*/
int filem_oper_rename(char *pSFile, char *pDFile)
{
     if((pSFile == NULL) || (pDFile == NULL))
          return(-1);
     else return(rename(pSFile, pDFile));
}


/*---------------------------------------------------------*/
/* Name: filem_oper_copy                                   */
/* Retu: 成功返回0, 失败返回-x                             */
/* Desc: 复制文件                                          */
/*---------------------------------------------------------*/
int filem_oper_copy(char *pSFile, char *pDFile, FilemCall pProCall, void *pPara)
{
    int   iResult =  0;
    int   iRetval =  0;
    int   iReadFd = -1;
    int   iWritFd = -1;    
    int   iTskDly =  0;
    int   iReadLn =  0;
    int   iFileSz =  0;
    int   iCpyCou =  0;
    int   iPercnt =  0;
    char *pBuffer = NULL;
    char  pProInf[64]={0};

    pBuffer = XMALLOC(MTYPE_FILEM_ENTRY, 130*1024);

    if(pBuffer == NULL) return(-1);

    iReadFd = open(pSFile, O_RDONLY, 0);

    if(iReadFd < 0)
    {
        XFREE(MTYPE_FILEM_ENTRY, pBuffer);
        
        return(-1);  
    }
    
    iFileSz = filem_oper_size_getf(iReadFd);
    
    iWritFd = open(pDFile, O_WRONLY|O_TRUNC|O_CREAT, 0777);

    if(iWritFd < 0)
    {
        close(iReadFd);
        XFREE(MTYPE_FILEM_ENTRY, pBuffer);
        
        return(-1);  
    }
    
    if(pProCall) 
    {
        sprintf(pProInf, "Percent:   0%%  \r\n");
        pProCall(0, pProInf, pPara, 0);
    }

    while(1)
    {
        iTskDly ++;

        if((iTskDly > 16) && pProCall && (iFileSz > 0))
        {
            iPercnt = filem_oper_percent(iCpyCou, iFileSz, 0);
            
            sprintf(pProInf, "\033[1A""\033[K""Percent:%4d%%\r\n", iPercnt);

            pProCall(0, pProInf, pPara, 0);

            iTskDly = 0;
            
            usleep(1000); 
        }
        
        iReadLn = read(iReadFd, pBuffer, 128*1024);
        
        if(iReadLn <= 0)
        {
            iResult = ((iReadLn < 0) ? -1 : 0); 
            break;
        }
        
        iRetval = write(iWritFd, pBuffer, iReadLn);
        
        if(iRetval <= 0)
        {
            iResult = -1; break;
        }
    
        iCpyCou += iReadLn;
    }

    if(iResult == 0)
    {
        if(pProCall) 
        {
            if(iFileSz <= 0)
            {
                sprintf(pProInf, "\033[1A""\033[K""Percent: 100%% \r\n");
            }
            else 
            {
                iPercnt = filem_oper_percent(iCpyCou, iFileSz, 0);
                sprintf(pProInf, "\033[1A""\033[K""Percent: %3d%% \r\n", iPercnt);
            }
            pProCall(0, pProInf, pPara, 0);
        }
    }

    if(iWritFd >= 0)
    {
       fsync(iWritFd);
       
       close(iWritFd);
    }
    
    if(iReadFd >= 0) close(iReadFd);

    if(pBuffer != NULL) 
    {
        XFREE(MTYPE_FILEM_ENTRY, pBuffer);
    }

    return(iResult); 
}


/*---------------------------------------------------------*/
/* Name: filem_oper_availdisk                              */
/* Retu: 成功返回可用空间的大小，单位B, 失败返回 0         */
/* Desc: 文件解压                                          */
/*---------------------------------------------------------*/
unsigned int filem_oper_availdisk(char *pDPath)
{
    struct statfs diskInfo;
  //unsigned long long blocksize;
  //unsigned long long totalsize;
  //unsigned long long freeDisk;
    unsigned long long diskfree;
  
    if(pDPath == NULL)
    {
        return 0;
    }
    else
    {
        statfs(pDPath, &diskInfo); 
  
      //blocksize = diskInfo.f_bsize;                 //每个block里包含的字节数，byte
      //totalsize = blocksize * diskInfo.f_blocks;    //总的字节数，f_blocks为block的数目 
  
      //freeDisk = diskInfo.f_bfree * blocksize;      //剩余空间的大小，byte
        diskfree = diskInfo.f_bavail * diskInfo.f_bsize;   //可用空间大小，byte
  
        zlog_debug(FILEM_DEBUG_OTHER, "%s[%d]: filem get disk available = %llu B, %llu KB \n", 
                   __FILE__,__LINE__, diskfree, diskfree >> 10); 
  
        return((unsigned int)diskfree);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_oper_extract                                */
/* Retu: 成功返回0, 失败返回-x                             */
/* Desc: 文件解压                                          */
/*---------------------------------------------------------*/
int filem_oper_extract(char *pSFile, char *pDPath, 
                       FilemCall pProCall, void *pPara, int iSpchk)
{
    char bExec[256] = {0};

    if(iSpchk)
    {
        int iSize = 0;
        
        iSize = filem_oper_size_get(pSFile);
        
       /* tar.gz 格式的打包文件解压后大于源文件的1.33倍，此处确保有足够的解压空间 */
        iSize = (iSize + iSize/3);

        zlog_debug(FILEM_DEBUG_OTHER, "Extract file need disk space %d\n", iSize);

        if(iSize > (int)filem_oper_availdisk(pDPath))
        {
            return -FILEM_ERROR_DISKFULL;
        }
    }

    sprintf(bExec, "tar -xzf %s -C %s", pSFile, pDPath);

    zlog_debug(FILEM_DEBUG_OTHER, "Extract file %s\n", bExec);
            
    system(bExec);

    system("sync");

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_oper_clrdir                                 */
/* Retu: 成功返回0, 失败返回-x                             */
/* Desc: 清除目录下文件                                    */
/*---------------------------------------------------------*/
int filem_oper_clrdir(char *pDPath)
{
    char bExec[256] = {0};

    sprintf(bExec, "rm %s* -f", pDPath);

    system(bExec);

    system("sync");
    
    return(0);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

