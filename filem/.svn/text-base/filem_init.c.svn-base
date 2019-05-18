/* file operate 
   name filem_error.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#include "lib/log.h"
#include "lib/devm_com.h"
#include "lib/module_id.h"
#include "lib/msg_ipc_n.h"
#include "lib/hptimer.h"
#include "filem.h"
#include "filem_oper.h"
#include "filem_img.h"
#include "filem_tran.h"
#include "filem_slot.h"
#include "filem_sync.h"
#include "filem_tftp.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
//extern struct thread_master *master;

//int           gfiledebugsw  = 0;

int           gfilelocslot  = 1;

int           gfiledeviceid = 0;

int           gfilemtdkernl = 0;

int           gfileml3vpn   = 0;

char          gbfiledevname[FILEM_DEVNAME_MLEN] = FILEM_DEV_NAME;

filemtypes    gfiletypesman[FILEM_MTYPE_MAX] = {
                                               {0, 0, ""},
                                               {0, 0, FILEM_PATH_BIN},
                                               {0, 0, FILEM_PATH_CFG},
                                               {0, 0, FILEM_PATH_DAT},
                                               {0, 0, FILEM_PATH_VER},
                                               {0, 0, FILEM_PATH_TMP},
                                               }; 

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
static const char  *gfileerrlist[] =
{
    "No error",
    "Command is running",
    "Command is finished and OK",
    "File name error",
    "File path error",
    "File size error",
    "File already exist",
    "File no exist",
    "No enougth disk space",
    "File length too long",
    "Copy file fail",
    "Delete file fail",
    "Rename file fail",
    "Unknow file type",
    "The same file name",
    "Open file error",
    "Read file error",
    "Write file error",
    "Close file error",
    "Verify data error",
    "Slot error",
    "No operation",
    "Create socket fail",
    "Send data to socket fail",
    "Wait receive data fail",
    "Receive data timeout",
    "Receive data error",
    "Receive packet serno error",
    "Scocket connect fail", 
    "Send retry fail", 
    "Server ack error", 
    "Too many waste pkt", 
    "Download fail", 
    "Create thread fail",
    "MD5 check fail",
    "MD5 pad data error",
    "Slot HW no insert",
    "Board HW match error",
    "Update app fail",
    "Update kernel fail",
    "Update fpga fail",
    "Update boot fail",
    "No kernel area",
    "Slot error",
    "The disk is full",
    "Cancel the operation",
    "Resource is used or channel busy",
    "File transfer error",
    "File transfer timeout",
    "Conflict with dir name",
    "No memory",
    "Parameter error",
    "Operation conflict",
    "Kernel active error",
    "File type not match",
    "Request repeat",
    "No support this command",
    "IP address error",
    "No master CPU",
    "Authentication fail",
    "Other error"
};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_device_isht15x                              */
/*---------------------------------------------------------*/
int filem_device_ht15x(void)
{
    return(((gfiledeviceid == ID_HT157) || (gfiledeviceid == ID_HT158)) ? 1 : 0); 
}

/*---------------------------------------------------------*/
/* Name: filem_sys_setdebug                                */
/*---------------------------------------------------------*/
/*
void filem_sys_setdebug(int iState)
{
    gfiledebugsw = iState; 
}
*/

/*---------------------------------------------------------*/
/* Name: filem_error_get                                   */
/* Desc: 获取错误信息                                      */
/*---------------------------------------------------------*/
char* filem_sys_errget(int iErrNo)
{
    iErrNo = -iErrNo;
    
    if(iErrNo > 0) 
    {
         if(iErrNo >= FILEM_ERROR_MAXNO)
              return((char *)gfileerrlist[0]);   
         else return((char *)gfileerrlist[iErrNo]);
    }
    else return((char *)gfileerrlist[0]);    
    
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_name_totype                                 */
/* Retu: 0  1                                              */
/* Desc: 根据文件名判断是否为系统文件                      */
/*---------------------------------------------------------*/
int filem_name_totype(char *pLocName)
{
    char bDirName[128] = {0};
    
    filem_oper_cutpath(pLocName, bDirName);

    if(strcmp(bDirName, FILEM_PATH_BIN) == 0) return(FILEM_MTYPE_BIN);
    
    if(strcmp(bDirName, FILEM_PATH_VER) == 0) return(FILEM_MTYPE_VER);
    
    if(strcmp(bDirName, FILEM_PATH_CFG) == 0) return(FILEM_MTYPE_CFG);
    
    if(strcmp(bDirName, FILEM_PATH_DAT) == 0) return(FILEM_MTYPE_DAT);
    
    if(strcmp(bDirName, FILEM_PATH_TMP) == 0) return(FILEM_MTYPE_TMP);
    
    return(FILEM_MTYPE_NON);
}

/*---------------------------------------------------------*/
/* Name: filem_type_pathget                                */
/* Desc: 获取文件类型路径                                  */
/*---------------------------------------------------------*/
char* filem_type_pathget(int itype)
{
    return(gfiletypesman[itype].bPrePath);
}

/*---------------------------------------------------------*/
/* Name: filem_action_flagset                              */
/* Desc: 设置某类型当前操作标志                            */
/*---------------------------------------------------------*/

int filem_action_flagset(int itype, int flag)
{
   /*应该加数据互斥访问保护*/
    gfiletypesman[itype].iFlags |= flag;

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_action_flagclr                              */
/* Desc: 清除某类型当前操作标志                            */
/*---------------------------------------------------------*/

int filem_action_flagclr(int itype, int flag)
{
   /*应该加数据互斥访问保护*/
    gfiletypesman[itype].iFlags &= ~flag;

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_action_flagchk                              */
/* Desc: 获取某类型当前操作标志                            */
/*---------------------------------------------------------*/

int filem_action_flagchk(int itype, int flag)
{
    if((gfiletypesman[itype].iFlags & flag) == 0)
         return(0);
    else return(1);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_dir_init                                    */
/* Desc: 创建基本目录                                      */
/*---------------------------------------------------------*/
static int filem_dir_init(void)
{
    filem_oper_mkdir((char *)FILEM_PATH_BIN);
    filem_oper_mkdir((char *)FILEM_PATH_CFG);
    filem_oper_mkdir((char *)FILEM_PATH_DAT);
    filem_oper_mkdir((char *)FILEM_PATH_VER);
    filem_oper_mkdir((char *)FILEM_PATH_TMP);

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_l3vpn_set                             */
/* Desc: VPN设置                                            */
/*---------------------------------------------------------*/
int filem_tftp_l3vpn_set(int iVpnno)
{
    if((iVpnno >= 0) && (iVpnno <=127))
    {
         filemstatus  *pstainfo = NULL;
         
         gfileml3vpn = iVpnno;
         
         pstainfo = filem_slot_stainfoget(0);

         if(pstainfo) pstainfo->iL3Vpn = iVpnno;
         
         return(0);
    }
    else return(-1);
}

/*---------------------------------------------------------*/
/* Name: filem_tftp_l3vpn_get                             */
/* Desc: VPN设置                                            */
/*---------------------------------------------------------*/
int filem_tftp_l3vpn_get(void)
{
    return(gfileml3vpn);
}

/*---------------------------------------------------------*/
/* Name: filem_download_stat_set                           */
/* Desc: 下载状态设置                                      */
/*---------------------------------------------------------*/
void filem_download_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iDownload = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}

/*---------------------------------------------------------*/
/* Name: filem_upload_stat_set                             */
/* Desc: 上载状态设置                                      */
/*---------------------------------------------------------*/

void filem_upload_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpload = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}


/*---------------------------------------------------------*/
/* Name: filem_upgrade_time_get                            */
/* Desc: 读取最近一次升级的时间                            */
/*---------------------------------------------------------*/
static unsigned long filem_upgrade_time_get(void)
{
    int fd = 0;

    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    fd = filem_oper_open((char*)FILEM_FILE_UPGRADE_TIME, O_RDONLY, 0);

    if((fd > 0) && pstainfo)
    {
         filem_oper_read(fd, (char*)&pstainfo->iUpgradeTime, sizeof(unsigned long));
 
         filem_oper_close(fd);
 
         return(pstainfo->iUpgradeTime);
    }
    else return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_upgrade_time_set                            */
/* Desc: 设置最近一次升级的时间                            */
/*---------------------------------------------------------*/
void filem_upgrade_time_set(unsigned long timev)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        int fd = 0;
        
        pstainfo->iUpgradeTime = timev;
        
        fd = filem_oper_open((char *)FILEM_FILE_UPGRADE_TIME, 
                             O_WRONLY | O_TRUNC | O_CREAT | O_NOCTTY, 0644);

        if(fd > 0)
        {
             filem_oper_write(fd, (char *)&timev, sizeof(unsigned long));

             filem_oper_syncf(fd);
             
             filem_oper_close(fd);
        }

        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}

/*---------------------------------------------------------*/
/* Name: filem_upgrade_stat_set                            */
/* Desc: 升级状态设置                                      */
/*---------------------------------------------------------*/
void filem_upgrade_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgrade = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}


/*---------------------------------------------------------*/
/* Name: filem_upgfpga_stat_set                            */
/* Desc: 升级状态设置                                      */
/*---------------------------------------------------------*/
void filem_upgfpga_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgFpga = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}

/*---------------------------------------------------------*/
/* Name: filem_upgfpga_time_set                            */
/* Desc: 设置最近一次升级的时间                            */
/*---------------------------------------------------------*/
void filem_upgfpga_time_set(unsigned long timev)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgFpgaTime = timev;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}


/*---------------------------------------------------------*/
/* Name: filem_upgapp_stat_set                             */
/* Desc: 升级状态设置                                      */
/*---------------------------------------------------------*/
/*
void filem_upgapp_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgApp = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}
*/
/*---------------------------------------------------------*/
/* Name: filem_upgapp_time_set                             */
/* Desc: 设置最近一次升级的时间                            */
/*---------------------------------------------------------*/
/*
void filem_upgapp_time_set(unsigned long timev)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgAppTime = timev;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}
*/
/*---------------------------------------------------------*/
/* Name: filem_upgkernel_stat_set                          */
/* Desc: 升级状态设置                                      */
/*---------------------------------------------------------*/
void filem_upgkernel_stat_set(int iStat)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgKnl = -iStat;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}

/*---------------------------------------------------------*/
/* Name: filem_upgkernel_time_set                          */
/* Desc: 设置最近一次升级的时间                            */
/*---------------------------------------------------------*/
void filem_upgkernel_time_set(unsigned long timev)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
        pstainfo->iUpgKnlTime = timev;
        
        if(!filem_slot_ismaster()) filem_sync_statussend();
    }
}

/*---------------------------------------------------------*/
/* Name: filem_upgrade_is_busy                             */
/* Desc: 检测升级是否正在执行                              */
/*---------------------------------------------------------*/
int filem_upgrade_is_busy(void)
{
    filemstatus *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo) 
    {
         if((pstainfo->iUpgrade == FILEM_ERROR_RUNNING) ||
            (pstainfo->iUpgFpga == FILEM_ERROR_RUNNING) ||
            (pstainfo->iUpgKnl  == FILEM_ERROR_RUNNING))
              return(1);
         else return(0);
    }
    else return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_dev_nameget                                 */
/* 获取设备名称                                            */
/*---------------------------------------------------------*/
static void filem_device_name_get(void)
{   
    int           ret = 0;
 // int           iStrlen = 0;
 // int           iCount  = 0;
 // int           iCouTmp = 0;
    enum DEV_TYPE DevType = DEV_TYPE_INVALID;
    
 // char *pTmpstri = NULL;
    char  pDevName[32] = {0};
 // char  pDevTemp[32] = {0};
    
    struct devm_box BoxDev;
    
    if(devm_comm_get_dev_type(MODULE_ID_FILE, &DevType) != 0)
    {
        zlog_err("%s[%d]: filem get device type error, device type is %d\n",__FILE__,__LINE__, DevType);
    }

    switch(DevType)
    {
        case DEV_TYPE_BOX:  /* 盒式设备，例如 181 系列 */

             ret = devm_comm_get_box(MODULE_ID_FILE, &BoxDev);
             
             if(ret == 0)
             {
                 gfilemtdkernl = BoxDev.dev_subtype;
                 
                 strcpy(pDevName, (char*)BoxDev.dev_name);
             }
             break;

        case DEV_TYPE_SHELF:/* 单框设备，例如 2000 系列有背板的设备 */  
             
             ret = devm_comm_get_dev_name(1, 0, MODULE_ID_FILE, (unsigned char*)pDevName);
             break;

        case DEV_TYPE_STACK: /* 多框堆叠设备，暂时没有 */
             ret = 1;
             break;
             
        default: break;
    }
    
    printf("filem get device name from devm is: %s\r\n", pDevName);

    if(ret == 0)
    {
        if((strlen(pDevName) > 0) &&
           (strcmp(pDevName, gbfiledevname) != 0))
        {               
#if 0        
                                      /* 获取设备名称，如: H20RN181-4GX-4GE-4E1, 则只获取H20RN-181 */            
            pTmpstri = strchr(pDevName, '-');
            
            if(pTmpstri)
            {
                 pTmpstri++;
                 pTmpstri = strchr(pTmpstri, '-');
  
                 if(pTmpstri) //如果只有一个"-"，则直接拷贝
                 {
                     *pTmpstri = 0;
                      strcpy(pDevTemp, pDevName);
                 }
                 else strcpy(pDevTemp, pDevName);
            }
            else strcpy(pDevTemp, pDevName);
            
    
            memset(pDevName, 0, sizeof(pDevName));
                                  /* 去除设备名称中的分隔符 */
            iStrlen = (int)strlen(pDevTemp);   
                                  
            while(iCount < iStrlen)
            {
                if(((pDevTemp[iCount] <= 'Z') && (pDevTemp[iCount] >= 'A')) || 
                   ((pDevTemp[iCount] <= 'z') && (pDevTemp[iCount] >= 'a')) ||
                   ((pDevTemp[iCount] <= '9') && (pDevTemp[iCount] >= '0')))
                {
                   pDevName[iCouTmp] = pDevTemp[iCount]; iCouTmp++;
                }
                
                iCount++;
            }
#endif
            filem_oper_str2low(pDevName, FILEM_DEVNAME_MLEN, gbfiledevname);
        }
        zlog_debug(FILEM_DEBUG_OTHER, " filem_devname_get name=%s \n", gbfiledevname);
    }
}

/*---------------------------------------------------------*/
/* Name: filem_diskspace_get                               */
/* 获取磁盘剩余空间                                        */
/*---------------------------------------------------------*/

int filem_diskspace_get(void)
{
    static unsigned long iLastTime = 0;

    filemstatus  *pstainfo = NULL;

    unsigned long iCurrTime = time(NULL);

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo && ((iCurrTime - iLastTime) > 5))
    {
        iLastTime = iCurrTime;
        
        pstainfo->iFreeSpace = filem_oper_availdisk((char*)FILEM_PATH_BIN) + 
                               filem_oper_availdisk((char*)FILEM_PATH_DAT);
    }
    return(0);  
}


/*---------------------------------------------------------*/
/* Name: filem_sysversion_get                              */
/* 系统版本APP获取                                         */
/*---------------------------------------------------------*/
static void filem_sysversion_get(void)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo)
    {
        char bData[260] = {0};

        memset(bData, 0, 256);
        
        devm_comm_get_sys_version((uchar*)bData);
        
        strncpy(pstainfo->cSoftVer, bData, FILEM_VERS_MAXLN - 1);
    }
}


/*---------------------------------------------------------*/
/* Name: filem_knlversion_get                              */
/* FPGA版本获取                                            */
/*---------------------------------------------------------*/
static void filem_knlversion_get(void)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo)
    {
        filem_img_kernelver_get(pstainfo->cKernelVer);    
    }
}

/*---------------------------------------------------------*/
/* Name: filem_bootversion_get                             */
/* FPGA版本获取                                            */
/*---------------------------------------------------------*/
static void filem_bootversion_get(void)
{
    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo)
    {
        filem_img_bootver_get(pstainfo->cBootVer);    
    }
}

/*---------------------------------------------------------*/
/* Name: filem_fpgaversion_get                             */
/* FPGA版本获取                                            */
/*---------------------------------------------------------*/
static void filem_fpgaversion_get(void)
{
    short sFpgaVer = 0;

    filemstatus  *pstainfo = NULL;

    pstainfo = filem_slot_stainfoget(0);

    if(pstainfo)
    {
        devm_comm_get_fpga_ver(FILEM_UNIT, gfilelocslot, MODULE_ID_FILE, &sFpgaVer);

        if(sFpgaVer != 0)
             sprintf(pstainfo->cFpgaVer, "%x.%-2x", sFpgaVer/16, sFpgaVer%16);
        else sprintf(pstainfo->cFpgaVer, "-");
    }
}


/*---------------------------------------------------------*/
/* Name: filem_dev_init                                    */
/* 设备初始化                                              */
/*---------------------------------------------------------*/
static void filem_device_init(void)
{   
    struct devm sdev = {0};
    
    memset(&sdev, 0, sizeof(struct devm));
    
    devm_comm_get_mydevice_info(MODULE_ID_FILE, &sdev);

    gfilelocslot = (int)sdev.myslot;

    devm_comm_get_id(1, 0, MODULE_ID_FILE, (unsigned int*)&gfiledeviceid);

    filem_device_name_get();
    
    printf("Filem init gfilelocslot = %d, devid=0x%x\r\n", gfilelocslot, gfiledeviceid);
    
    zlog_debug(FILEM_DEBUG_OTHER, "Filem init gfilelocslot = %d, devid=0x%x\r\n", gfilelocslot, gfiledeviceid);
}

/*---------------------------------------------------------*/
/* Name: filem_dev_init                                    */
/* 设备初始化                                              */
/*---------------------------------------------------------*/
static void filem_statinf_init(void)
{   
    filem_upgrade_time_get();

    filem_diskspace_get();

    filem_sysversion_get();

    filem_fpgaversion_get();

    filem_knlversion_get();    

    filem_bootversion_get();
}

/*---------------------------------------------------------*/
/*filem_base_timer                                         */
/*基本定时器                                               */
/*---------------------------------------------------------*/

//static int filem_base_timer(struct thread *thread)
static int filem_base_timer(void *para)
{
   //printf(" filem base timer\r\n");
    
    filem_tran_timeout();

    filem_show_timeout();
    
    filem_sync_timeout();

    filem_sync_rttimeout();

    filem_tftp_session_timeout();
    
    filem_ftp_session_timeout();

  //thread_add_timer(filemmaster, filem_base_timer, NULL, FILEM_TIMER_BASE);  

    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_init                                        */
/* Desc: filem 系统初始化                                  */
/*---------------------------------------------------------*/
int filem_init(void)
{
    filem_dir_init();
    
    filem_device_init();
    
    filem_slot_init();
    
    filem_img_init();

    filem_statinf_init();
    
    filem_sync_init();
    
  /*filem_show_init();*/
  
    filem_tran_init();

    filem_cli_init();
        
 // thread_add_timer(filemmaster, filem_base_timer, NULL, 1);  

    high_pre_timer_add("FilemanTimer", LIB_TIMER_TYPE_LOOP, filem_base_timer, NULL, 1000);

    printf("Filem init Ok\r\n");
    
    return(0);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

