/* file process command 
   name filem_image.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

#include "lib/memory.h"
#include "lib/msg_ipc_n.h"
#include "lib/log.h"

#include "filem.h"
#include "filem_md5.h"
#include "filem_img.h"
#include "filem_oper.h"
#include "filem_tran.h"
#include "filem_slot.h"


/*-----------------------------------------------------------------------------------------*/

static kernelarea    gfilebooteainfo;

/*
static kernelarea    gfileknlareatbl[FILEM_KRL_AREA_MAX];
*/
static unsigned char gfileknlvalidflag[4]= {0xAA, 0x55, 0xCC, 0x77}; 

static unsigned char gfileknlactiveflag[4]= {0xCC, 0x77, 0xAA, 0x55}; 


static const char   *gfileknlareadev[FILEM_KRL_AREA_MAX]={FILEM_KRL_DEV_AREA0,FILEM_KRL_DEV_AREA1};   

static const char   *gfileknlareapath[FILEM_KRL_AREA_MAX]={FILEM_KRL_PATH_AREA0,FILEM_KRL_PATH_AREA1};        

static char          gfileknlarename[FILEM_NAME_MAXLN];
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/* 获取 kernel 管理表名称 */
/*---------------------------------------------------------*/
/* Name: filem_img_kerneltable_find                        */
/* Desc: 获取 kernel 管理表MTD名称                                */
/*---------------------------------------------------------*/

static void filem_img_kerneltable_find(void)
{
	int  iFile = -1;
	int  iDlen =  0;
	int  iLoop =  0;
	int  iCount = 0;
	int  iSize = sizeof(kernelarea) * FILEM_KRL_AREA_MAX;
	char bData[2048] = {0};
	char cMtdName[64] = {0};
	
	kernelarea *pArea = (kernelarea *)bData;

	                                        /* 遍历每个分区，找出 image 管理区所在的分区 */
	for(iCount = 0; iCount < 6; ++iCount)
	{
		sprintf(cMtdName, "/dev/mtd%d", iCount);

		iFile = filem_oper_open(cMtdName, O_RDONLY, 0);
			
		if(iFile >= 0)
		{
			filem_oper_seek(iFile, 0, SEEK_SET);

			iDlen = filem_oper_read(iFile, bData, 2048);

			filem_oper_close(iFile);

			if(iDlen < iSize)
			{
				continue ;
			}
			else
			{
				for(iLoop = 0; iLoop < FILEM_KRL_AREA_MAX; iLoop++)
				{
					if((memcmp(pArea[iLoop].isused, gfileknlvalidflag, 4) == 0) &&
					   (memcmp(pArea[iLoop].valid,  gfileknlvalidflag, 4) == 0))
					{
						strcpy(gfileknlarename, cMtdName);

                        zlog_debug(FILEM_DEBUG_OTHER, "image area name is: %s\n", gfileknlarename);

						printf("Kernel image table device: %s\r\n",  gfileknlarename);

                        break;
					}
				}

                if(iLoop < FILEM_KRL_AREA_MAX) break;
			}
		}
	}	
}


/*---------------------------------------------------------*/
/* Name: filem_img_knltblread                              */
/* Desc: IMAGE区域管理表 MD5 标签读取                       */
/*---------------------------------------------------------*/
static void filem_img_kernelpad_read(int index, int isize)
{
    int  retva = 0;
    char image[128] = {0};

    filepadinf  sPadInf;

    kernelarea *pKnlTbl = NULL;
    
    pKnlTbl = filem_slot_knltblget(0);

 // if(gfilemtdkernl)
    if(filem_device_ht15x())
    {                                                        //isize must > 0
        sprintf(image, "/dev/mtd%d", FILEM_KRL_DEV_MTDNUM ); //+ index);
    }
    else
    {                                                        //isize must == 0
        isize = 0;
        sprintf(image, "%s/%s", gfileknlareapath[index], FILEM_KRL_KNL_IMAGE);
    }

    retva = filem_md5_pad_getbyname(image, &sPadInf, isize);

    zlog_debug(FILEM_DEBUG_OTHER, "filem_img_kernelpad_read  %s, retva= %d\n", image, retva);

    if(retva == 0)
    {
        if(strncmp(sPadInf.flags, FILEM_PAD_INF_FLAG_DATA, FILEM_PAD_INF_FLAG_LEN) == 0)
        {
            pKnlTbl[index].datalen = *((int*)sPadInf.len);   /*包括PAD的长度*/
            
            memcpy(pKnlTbl[index].md5data, sPadInf.md5, FILEM_PAD_MD5_MARK);

            strncpy(pKnlTbl[index].filevers, sPadInf.ver, FILEM_PAD_INF_VER_LEN);
        }
    }
}


/*---------------------------------------------------------*/
/* Name: filem_img_kerneltable_read                        */
/* Desc: IMAGE区域管理表读取                                      */
/*---------------------------------------------------------*/
static int filem_img_kerneltable_read(void)
{
    int  iFile = -1;
    int  iDlen =  0;
    int  iLoop =  0;
    int  iSize = sizeof(kernelarea) * FILEM_KRL_AREA_MAX;

    char bData[2048] = {0};
    
    kernelarea *pArea = (kernelarea *)bData;
    kernelarea *pKnlTbl = NULL;

    pKnlTbl = filem_slot_knltblget(0);

    iFile = filem_oper_open(gfileknlarename, O_RDONLY, 0);

    zlog_debug(FILEM_DEBUG_OTHER, "open kernel table dev %d\n", iFile);
        
    if(iFile >= 0)
    {
        filem_oper_seek(iFile, 0, SEEK_SET);

        iDlen = filem_oper_read(iFile, bData, 2048);

        filem_oper_close(iFile);

        if(iDlen < iSize) memset(pKnlTbl, 0, iSize);
        else
        {                                      /*以网络字节序保存的*/
            pArea[0].areasize = ntohl(pArea[0].areasize);
            pArea[0].datalen  = ntohl(pArea[0].datalen);
            pArea[1].areasize = ntohl(pArea[1].areasize);
            pArea[1].datalen  = ntohl(pArea[1].datalen);

            memcpy(pKnlTbl, bData, iSize);

            for(iLoop = 0; iLoop < FILEM_KRL_AREA_MAX; iLoop++)
            {
                zlog_debug(FILEM_DEBUG_OTHER, " read kernel tabl %d\n", iLoop);
                
                if((memcmp(pKnlTbl[iLoop].isused, gfileknlvalidflag, 4) == 0) &&
                   (memcmp(pKnlTbl[iLoop].valid,  gfileknlvalidflag, 4) == 0))
                {
                    filem_img_kernelpad_read(iLoop, pKnlTbl[iLoop].datalen);                    
                }
                else
                {
                    pKnlTbl[iLoop].datalen  = 0;
                    
                    memset(pKnlTbl[iLoop].filevers, 0, FILEM_VERS_MAXLN);
                  
                    memset(pKnlTbl[iLoop].md5data, 0, FILEM_PAD_MD5_MARK);
                }
            }
        }
    }
    
    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_img_knltblwrite                             */
/* Desc: IMAGE区域管理表设置                                      */
/*---------------------------------------------------------*/
static int filem_img_kerneltable_write(void)
{
    int  iFile = -1;
    char bData[4096] = {0};
    
    kernelarea *pKtbl = NULL;
    kernelarea *pArea = NULL; 
    
    iFile = filem_oper_open(gfileknlarename, O_WRONLY, 0);

    if(iFile >= 0)
    {                                            /*以网络字节序保存*/
         filem_oper_read(iFile, bData, 4096);    /*目的保存一下BOOT分区信息*/
                                                 
       //if(filem_oper_erase(iFile, 0, 131072) != 0)
         if(filem_oper_erase(iFile, 0, 65536) != 0)
         {
             filem_oper_close(iFile);

             printf("Erase %s error", gfileknlarename);
             return(-1);
         }

         memset(bData, 0, 2048);                 /*只清除KERNEL分区信息*/

         pArea = (kernelarea*)bData;
         
         pKtbl = filem_slot_knltblget(0);

         memcpy(pArea, pKtbl, sizeof(kernelarea) * FILEM_KRL_AREA_MAX);
         
         pArea[0].areasize = htonl(pArea[0].areasize);
         pArea[0].datalen  = htonl(pArea[0].datalen);
         pArea[1].areasize = htonl(pArea[1].areasize);
         pArea[1].datalen  = htonl(pArea[1].datalen);

         filem_oper_seek(iFile, 0, SEEK_SET);
         
         filem_oper_write(iFile, bData, 4096);   /*写回完整的两个分区*/

         filem_oper_close(iFile);

         filem_oper_flush();

         return(0);
    }
    else 
    {
         printf("Open device %s error", gfileknlarename);
         return(-1);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_img_kernelidx_get                           */
/* Desc: 获取写入的区域ID                                   */
/*---------------------------------------------------------*/
static int filem_img_kernelidx_get(void)
{
     int  index = 0;
     int  noact = FILEM_KRL_AREA_MAX;
     int  isuse = FILEM_KRL_AREA_MAX;
     kernelarea *pKnlTbl = NULL;

     pKnlTbl = filem_slot_knltblget(0);

     for(index = 0; index < FILEM_KRL_AREA_MAX; index++)
     {
         if(memcmp(pKnlTbl[index].isused, gfileknlvalidflag, 4) == 0)
         {
             if(isuse == FILEM_KRL_AREA_MAX) isuse = index;            /*第一个可用区*/
             
             if(memcmp(pKnlTbl[index].valid, gfileknlvalidflag, 4) == 0)
             {
                  if(memcmp(pKnlTbl[index].active, gfileknlactiveflag, 4) != 0)
                  {
					                                                  /*第一个非激活区*/
					  if(noact == FILEM_KRL_AREA_MAX) noact = index;  
                  }
             }
             else break;                                              /*第一个非有效区*/
         }
     }

     if(index  < FILEM_KRL_AREA_MAX) return(index);

     if(noact != FILEM_KRL_AREA_MAX) return(noact);

     if(isuse != FILEM_KRL_AREA_MAX) return(isuse);

     return(FILEM_KRL_AREA_MAX);
}


/*---------------------------------------------------------*/
/* Name: filem_img_kerneldata_write                        */
/* Desc: 写入数据到内核                                     */
/*---------------------------------------------------------*/
static int filem_img_kerneldata_write(int index, char *pfile)
{
    char bCmdBuf[128] = {0};

    sprintf(bCmdBuf, "umount %s", gfileknlareapath[index]);

    zlog_debug(FILEM_DEBUG_UPGRADE, "filem_img_kerneldata_write, %s\n", bCmdBuf);
        
    system(bCmdBuf);
    
    sleep(1);
    
    sprintf(bCmdBuf, "%s /dev/%s -t", FILEM_KRL_WRT_FUNC, gfileknlareadev[index]);

    zlog_debug(FILEM_DEBUG_UPGRADE, "filem_img_kerneldata_write, %s\n", bCmdBuf);
    
    system(bCmdBuf);

    sleep(1);

    sprintf(bCmdBuf, "%s /dev/%s %s" , FILEM_KRL_WRT_FUNC, gfileknlareadev[index], pfile); 

    zlog_debug(FILEM_DEBUG_UPGRADE, "filem_img_kerneldata_write, %s\n", bCmdBuf);

    system(bCmdBuf);

  //filem_oper_flush();

    sleep(1);

    sprintf(bCmdBuf, "mount -t ubifs %s %s", gfileknlareadev[index], gfileknlareapath[index]);

    zlog_debug(FILEM_DEBUG_UPGRADE, "filem_img_kerneldata_write, %s\n", bCmdBuf);

    system(bCmdBuf);
    
    filem_oper_flush();

    sleep(1);

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_img_kernelmdev_write                        */
/* Desc: 写入数据到内核                                     */
/*---------------------------------------------------------*/

static int filem_img_kernelmdev_write(int iasize, char *pfile, int iflen, char *pmdev)
{
    int   iDevFd = -1;
    int   iRetva =  0;
    int   iSecSz =  0; 
    int   iDatFd = -1;
    int   iRetry =  0;
    int   iAllen =  0;
    int   iRetln =  0;
    int   iRdlen =  0;
    char *bDatBf =  NULL;
    
    iDatFd = filem_oper_open(pfile, O_RDONLY, 0);
    
    iDevFd = filem_oper_open(pmdev, O_RDWR, 0);

    if((iDevFd < 0) || (iDatFd < 0))
    {
        iRetva = -FILEM_ERROR_OPEN;
        goto kernelwriteerr;
    }
    
    iSecSz = filem_oper_sectorsize_get(iDevFd);
    
    bDatBf = XMALLOC(MTYPE_FILEM_ENTRY, iSecSz + 2048);
    
    if(bDatBf == NULL)  
    {
        iRetva = -FILEM_ERROR_NOMEM;
        goto kernelwriteerr;
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "Areasize =%d, filelen=%d, sectorlen=%d\n", iasize, iflen, iSecSz);

    while(1)
    {
        iAllen = iRetln = iRdlen = 0;

        if(filem_oper_erase(iDevFd, 0, iasize) != 0)
        {
            iRetva = -FILEM_ERROR_WRITE;  break;  /*删除失败立即终止*/
        }

        sleep(1);
       
        filem_oper_seek(iDatFd, 0, SEEK_SET);
        
        filem_oper_seek(iDevFd, 0, SEEK_SET);
        
        while(1)
        {
            if((iflen - iAllen) >= iSecSz)
                 iRdlen = iSecSz;
            else iRdlen = iflen - iAllen;
            
            iRetln = filem_oper_read(iDatFd, bDatBf, iRdlen);

            if(iRetln != iRdlen)
            {
                iRetva = -FILEM_ERROR_READ; 
                break;
            }
                
            iRetln = filem_oper_write(iDevFd, bDatBf, iSecSz);
            
            if(iRetln != iSecSz)
            {
                iRetva = -FILEM_ERROR_WRITE;  

                zlog_err("Mtd data write error! %d\n", iRetln);

                zlog_debug(FILEM_DEBUG_UPGRADE, "Mtd data write error! %d\n", iRetln);

                break;
            }

            iAllen += iRdlen;

            if(iAllen >= iflen) break;
        }
                

        if((iRetva == 0) || (++iRetry > 2)) break;   /*尝试两次*/

        iRetva = 0;
        
        zlog_err("Kernel data write retry! %d\n", iRetry);

        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel data write error and retry! %d\n", iRetry);
    }


kernelwriteerr:
    
    if(bDatBf) XFREE(MTYPE_FILEM_ENTRY, bDatBf);
    
    if(iDevFd >= 0) filem_oper_close(iDevFd);
    if(iDatFd >= 0) filem_oper_close(iDatFd);

    filem_oper_flush();

    return(iRetva);
}


/*---------------------------------------------------------*/
/* Name: filem_img_kernel_upgrade                          */
/* Desc: kernel升级                                        */
/*---------------------------------------------------------*/
int filem_img_kernel_upgrade(char *bFName)
{
     int  iRetv = 0;
     int  index = 0;
     int  iLoop = 0;
     int  iFlen = 0;
     kernelarea *pKnlTbl = NULL;
     char bImage[FILEM_NAME_MAXLN];

     zlog_debug(FILEM_DEBUG_UPGRADE, "upgrade kernel file %s \n", bFName); 

     if(!filem_oper_isexist(bFName)) return(-FILEM_ERROR_NOEXIST);

     index = filem_img_kernelidx_get();

     zlog_debug(FILEM_DEBUG_UPGRADE, "get kernel index %d \n", index);

     if(index >= FILEM_KRL_AREA_MAX)  return(-FILEM_ERROR_NOKNLAREA);

     pKnlTbl = filem_slot_knltblget(0);
     
     memset(pKnlTbl[index].valid, 0, 4);
     
     memset(pKnlTbl[index].active, 0, 4);

     pKnlTbl[index].datalen = 0;
        
     iRetv = filem_img_kerneltable_write();

     zlog_debug(FILEM_DEBUG_UPGRADE, "write kernel cfg table clear %d\n", iRetv);

     if(iRetv != 0)
     {
         zlog_err("Kernel boot cfg table write error!\n");

         return(-FILEM_ERROR_WRITE);
     }


   //if(!gfilemtdkernl)
     if(!filem_device_ht15x())
     {
         sprintf(bImage, "%s/%s", gfileknlareapath[index], FILEM_KRL_KNL_IMAGE);
         
         iRetv = filem_img_kerneldata_write(index, bFName);
     }
     else
     {
         iFlen = filem_oper_size_get(bFName);            /*MTD写 必须获取文件长度*/
         
         sprintf(bImage, "/dev/mtd%d", FILEM_KRL_DEV_MTDNUM); //+index;
         
         iRetv = filem_img_kernelmdev_write(pKnlTbl[index].areasize, bFName, iFlen, bImage);
         
     }

     zlog_debug(FILEM_DEBUG_UPGRADE, "write kernel image data %s, %d\n", bImage, iRetv);

     if(iRetv != 0)
     {
         zlog_err("Kernel image data update error!\n");

         return(-FILEM_ERROR_WRITE);
     }

     sleep(1);

     iRetv = filem_md5_matchcheck(bImage, iFlen);   

     zlog_debug(FILEM_DEBUG_UPGRADE, "kernel image data verify %s, %d\n", bImage, iRetv);

     if(iRetv != 0)
     {
         zlog_debug(FILEM_DEBUG_UPGRADE, "kernel image data verify fail!\n");

         return(-FILEM_ERROR_WRITE);
     }

     for(iLoop = 0; iLoop < FILEM_KRL_AREA_MAX; iLoop++)
     {
         memset(pKnlTbl[iLoop].active, 0, 4);
     }
     
     memcpy(pKnlTbl[index].valid, gfileknlvalidflag, 4);
     
     memcpy(pKnlTbl[index].active, gfileknlactiveflag, 4);
     
     filem_img_kernelpad_read(index, iFlen);

   //pKnlTbl[index].datalen = iFlen;
 
     iRetv = filem_img_kerneltable_write();
     
     zlog_debug(FILEM_DEBUG_UPGRADE, "write kernel table %d\n", iRetv);

     if(iRetv != 0)
     {
         zlog_err("Kernel boot cfg table write error!\n");

         return(-FILEM_ERROR_WRITE);
     }

     return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_img_kernel_activesw                         */
/* Desc: KERNEL倒换                                        */
/*---------------------------------------------------------*/
int filem_img_kernel_activesw(void)
{
     int  index =  0;
     int  valid =  0;
     int  spare = -1;

     kernelarea *pKnlTbl = NULL;

     pKnlTbl = filem_slot_knltblget(0);

     for(index = 0; index < FILEM_KRL_AREA_MAX; index++)
     {
         if(memcmp(pKnlTbl[index].isused, gfileknlvalidflag, 4) == 0)
         {          
             if(memcmp(pKnlTbl[index].valid, gfileknlvalidflag, 4) == 0)
             {
                 valid ++;
                 if(memcmp(pKnlTbl[index].active, gfileknlactiveflag, 4) != 0)
                 {
                     spare = index;
                 }
             }
         }
     }

     if((valid == 2) && (spare >= 0))
     {
          for(index = 0; index < FILEM_KRL_AREA_MAX; index++)
          {
              memset(pKnlTbl[index].active, 0, 4);
          }
 
          memcpy(pKnlTbl[spare].active, gfileknlactiveflag, 4);
 
          if(filem_img_kerneltable_write() == 0)
               return(0);
          else return(-FILEM_ERROR_ACTKERNEL);
     }
     else return(-FILEM_ERROR_NOKNLAREA);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_img_boottable_read                          */
/* Desc: 读取boot信息                                      */
/*---------------------------------------------------------*/
static int filem_img_boottable_read(void)
{
    int  iFile = -1;
    int  iDlen =  0;
    int  iCount = 0;
    char bData[2048] = {0};
	char cTemp = 0xff;
	
    iFile = filem_oper_open(gfileknlarename, O_RDONLY, 0);

    zlog_debug(FILEM_DEBUG_UPGRADE, "open boot table dev %d\n", iFile);
        
    if(iFile >= 0)
    {
        filem_oper_seek(iFile, 2048, SEEK_SET);

        iDlen = filem_oper_read(iFile, bData, 2048);

        filem_oper_close(iFile);

        if(iDlen < (int)sizeof(kernelarea))
            memset(&gfilebooteainfo, 0, sizeof(kernelarea));
        else
        {
			/*以网络字节序保存*/
            memcpy(&gfilebooteainfo, bData, sizeof(kernelarea));

            gfilebooteainfo.filevers[FILEM_VERS_MAXLN - 1] = 0;
			/* boot版本信息字符串添加结束符 */
			for(iCount = 0; iCount < FILEM_VERS_MAXLN - 1; ++iCount)
			{
				if(gfilebooteainfo.filevers[iCount] == cTemp)
				{
					gfilebooteainfo.filevers[iCount] = '\0';
					break;
				}
			}
            
            gfilebooteainfo.datalen  = ntohl(gfilebooteainfo.datalen);
        
            gfilebooteainfo.areasize = ntohl(gfilebooteainfo.areasize);
        }
    }
    
    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_img_boot_check                              */
/* Desc: boot信息校验                                       */
/*---------------------------------------------------------*/
static int filem_img_boot_check(int iFlen, int iSecSz, 
                                int ifd, int iDevFd, char *bBuff1)
{
    int   iRetva = 0;
    int   iAllen = 0;
    int   iRdlen = 0;
    int   iRetln = 0;
    char *bBuff2 = NULL;

    bBuff2 = XMALLOC(MTYPE_FILEM_ENTRY, iSecSz + 2048);

    if(bBuff2 == NULL) return(-FILEM_ERROR_NOMEM);

    while(iAllen < iFlen)
    {
        filem_oper_seek(iDevFd, iAllen, SEEK_SET);

        filem_oper_seek(ifd,  iAllen, SEEK_SET);

        if((iFlen - iAllen) >= iSecSz)
             iRdlen = iSecSz;
        else iRdlen = iFlen - iAllen;
        
        iRetln = filem_oper_read(ifd, bBuff1, iRdlen);

        if(iRetln != iRdlen)
        {
           iRetva = -FILEM_ERROR_READ;  break;
        }
        
        iRetln = filem_oper_read(iDevFd, bBuff2, iRdlen);

        if(iRetln != iRdlen)
        {
           iRetva = -FILEM_ERROR_READ;  break;
        }

        if(memcmp(bBuff1, bBuff2, iRdlen) != 0)
        {
           zlog_debug(FILEM_DEBUG_UPGRADE, "Uboot memcmp check error iRdlen=%d! off=%d\n", iRdlen, iAllen);

           iRetva = -FILEM_ERROR_READ;  break;
        }

        iAllen += iRdlen;
    }

    if(iRetva != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Uboot data verify error! \n");

        zlog_err("Uboot data verify error! \n");
    }

    if(bBuff2) XFREE(MTYPE_FILEM_ENTRY, bBuff2);

    return(iRetva);
}


/*---------------------------------------------------------*/
/* Name: filem_img_boot_check                              */
/* Desc: boot信息写入                                       */
/*---------------------------------------------------------*/
static int filem_img_boot_write(int ifd, int areasz, int iFlen)
{
    int   iRetva = 0;
    int   iRetln = 0;
    int   iRdlen = 0;
    int   iAllen = 0;
    int   iSecSz = 0;
    int   iRetry = 0;
    int   iDevFd = -1;
    char *bBuff1 = NULL;

    iDevFd = filem_oper_open((char *)FILEM_BOOT_DEVICE, O_RDWR, 0);

    if(iDevFd < 0)
    {
       iRetva = -FILEM_ERROR_OPEN;
       goto bootwriteerr;
    }

    iSecSz = filem_oper_sectorsize_get(iDevFd);
    
    bBuff1 = XMALLOC(MTYPE_FILEM_ENTRY, iSecSz + 2048);
    
    if(bBuff1 == NULL)  
    {
       iRetva = -FILEM_ERROR_NOMEM; 
       goto bootwriteerr;
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "Areasize =%d, filelen=%d, sectorlen=%d\n", areasz, iFlen, iSecSz);

    while(1)
    {
        iAllen = iRetln = iRdlen = 0;

        if(filem_oper_erase(iDevFd, 0, areasz) != 0)
        {
            iRetva = -FILEM_ERROR_WRITE;  
            break;
        }

        sleep(1);

        filem_oper_seek(ifd, 0, SEEK_SET);
        
        filem_oper_seek(iDevFd, 0, SEEK_SET);
       
        while(1)
        {
            if((iFlen - iAllen) >= iSecSz)
                 iRdlen = iSecSz;
            else iRdlen = iFlen - iAllen;
            
            iRetln = filem_oper_read(ifd, bBuff1, iRdlen);

            if(iRetln != iRdlen)
            {
                iRetva = -FILEM_ERROR_READ; 
                break;
            }
                
            iRetln = filem_oper_write(iDevFd, bBuff1, iSecSz);
            
            if(iRetln != iSecSz)
            {
                zlog_debug(FILEM_DEBUG_UPGRADE, "Mtd data write error! %d\n", iRetln);

                iRetva = -FILEM_ERROR_WRITE;  
                break;
            }

            iAllen += iRdlen;

            if(iAllen >= iFlen) break;
        }
                
        if(iRetva == 0) 
        {
           sleep(2);
           
           iRetva = filem_img_boot_check(iFlen, iSecSz, ifd, iDevFd, bBuff1);   
        }

        if((iRetva == 0) || (++iRetry > 2)) break;

        iRetva = 0;
        
        zlog_err("Uboot data write retry! %d\n", iRetry);

        zlog_debug(FILEM_DEBUG_UPGRADE, "Uboot data write error and retry! %d\n", iRetry);
    }
    
bootwriteerr:

    if(bBuff1) XFREE(MTYPE_FILEM_ENTRY,bBuff1);

    if(iDevFd >= 0) filem_oper_close(iDevFd);
    
    return(iRetva);
}

/*---------------------------------------------------------*/
/* Name: filem_img_boot_upgrade                            */
/* Desc: boot升级                                          */
/*---------------------------------------------------------*/
int filem_img_boot_upgrade(char *bFName)
{
     int  iRetv = 0;
     int  iFlen = 0;
     int  iFd   = -1;
     int  iAsiz = FILEM_BOOT_ASIZE;

     iFd = filem_oper_open(bFName, O_RDONLY, 0);

     if(iFd < 0) 
     {
         zlog_debug(FILEM_DEBUG_UPGRADE, "Open uboot file fail %s\n", bFName);
         
         return(-FILEM_ERROR_OPEN);
     }
     
     iFlen = filem_oper_size_getf(iFd);

     if(memcmp(gfilebooteainfo.valid,  gfileknlvalidflag, 4) == 0)
     {
        iAsiz = gfilebooteainfo.areasize;
     }

     if((iFlen <= 0) || (iFlen > iAsiz)) 
     {
         filem_oper_close(iFd);

         return(-FILEM_ERROR_LENGTH);
     }

     iRetv = filem_img_boot_write(iFd, iAsiz, iFlen);

     zlog_debug(FILEM_DEBUG_UPGRADE, "Update uboot result=%d, areasize=%d, filelen=%d\n", iRetv, iAsiz, iFlen);

     filem_oper_close(iFd);

     return(iRetv);
}

/*---------------------------------------------------------*/
/* Name: filem_img_kernel_ver_get                          */
/* Desc: 获取 激活区 的kernel 版本                          */
/*---------------------------------------------------------*/
int filem_img_kernelver_get(char *bBufDat)
{
    int   index   = 0;
    kernelarea *pKnlTbl = NULL;

    if(bBufDat == NULL) return(-1);

    pKnlTbl = filem_slot_knltblget(0);

    for(index = 0; index < FILEM_KRL_AREA_MAX; index++)
    {
        if(memcmp(pKnlTbl[index].isused, gfileknlvalidflag, 4) == 0)
        {          
            if(memcmp(pKnlTbl[index].valid, gfileknlvalidflag, 4)== 0)
            {
	            if(memcmp(pKnlTbl[index].active, gfileknlactiveflag, 4)== 0)
	            {
					strncpy(bBufDat, pKnlTbl[index].filevers, FILEM_VERS_MAXLN - 1);
                    
					zlog_debug(FILEM_DEBUG_OTHER, "kernel Version is %s\n", pKnlTbl[index].filevers);

					break;
				}
			}
        }
    }

    if(index >= FILEM_KRL_AREA_MAX) strcpy(bBufDat, "NULL");

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_img_bootver_get                             */
/* Desc: 获取 激活区 的kernel 版本                          */
/*---------------------------------------------------------*/
int filem_img_bootver_get(char *bBufDat)
{
   strncpy(bBufDat, gfilebooteainfo.filevers, FILEM_VERS_MAXLN - 1);
    
   return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_img_init                                    */
/* Desc: kernel、boot管理区信息初始化                       */
/*---------------------------------------------------------*/
int filem_img_init(void)
{
	memset(gfileknlarename, 0, sizeof(gfileknlarename));
	
    memset(&gfilebooteainfo, 0, sizeof(kernelarea));

	filem_img_kerneltable_find();

    filem_img_kerneltable_read();

    filem_img_boottable_read();

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_img_kernel_info_get                         */
/* Desc: kernel信息组包                                    */
/*---------------------------------------------------------*/
int filem_img_kernel_info_get(char *bBufDat, int iSlot)
{
	int   index   = 0;
	int   iAllLen = 0;
	int   iValIdx = 0;
	int   iActIdx = 0;

    filemslot  *pSlotCb = NULL;
    kernelarea *pKnlTbl = NULL;
	const char *bStaStr[2] = {"No", "Yes"};

    pSlotCb = filem_slot_objectget(iSlot);

    if(pSlotCb == NULL) return(-1);

    if(!pSlotCb->hwstate || !pSlotCb->iscpu) return(-1);
    
    pKnlTbl = pSlotCb->fileknltbl;

	iAllLen += sprintf(bBufDat + iAllLen, "No. Partition Valid Active      Size      Used Version    \r\n" \
										  "--- --------- ----- ------ --------- --------- -------------\r\n");

	for(index = 0; index < FILEM_KRL_AREA_MAX; index++)
	{
		if(memcmp(pKnlTbl[index].isused, gfileknlvalidflag, 4) == 0)
		{
			iValIdx = iActIdx = 0;
			
			if(memcmp(pKnlTbl[index].valid, gfileknlvalidflag, 4) == 0)	iValIdx = 1;
			
			if(memcmp(pKnlTbl[index].active, gfileknlactiveflag, 4) == 0)  iActIdx = 1;

			iAllLen += sprintf(bBufDat + iAllLen, "%3d %9s %5s %6s %9d %9d %s\r\n",
							   index +1, bStaStr[1] , bStaStr[iValIdx], bStaStr[iActIdx],
							   pKnlTbl[index].areasize, pKnlTbl[index].datalen, pKnlTbl[index].filevers);
		}
		else 
		{
			iAllLen += sprintf(bBufDat + iAllLen, "%3d %9s %5s %6s     *****     ***** *****\r\n",
							   index +1, bStaStr[0] , bStaStr[0], bStaStr[0]);
		}
	}

	return(iAllLen);
}



/*---------------------------------------------------------*/
/* Name: filem_img_kernel_info_get                         */
/* Desc: boot信息组包                                          */
/*---------------------------------------------------------*/
int filem_img_bootinfo_get(char *bBufDat)
{
    int   iAllLen = 0;
   
    if(memcmp(gfilebooteainfo.valid,  gfileknlvalidflag, 4) != 0)
    {
        iAllLen += sprintf(bBufDat + iAllLen, "Boot infor invalid\r\n");
    }
    else
    {
      //iAllLen += sprintf(bBufDat + iAllLen, "Boot area size  : %d\r\n", gfilebooteainfo.areasize);
        iAllLen += sprintf(bBufDat + iAllLen, "Boot data length: %d\r\n", gfilebooteainfo.datalen);
        iAllLen += sprintf(bBufDat + iAllLen, "Boot version    : %s\r\n", gfilebooteainfo.filevers);
    }

    return(iAllLen);
}



/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/



