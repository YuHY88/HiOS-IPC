/* file process command 
   name filem_slot.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "lib/memory.h"
#include "lib/devm_com.h"
#include "lib/module_id.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/log.h"
#include "lib/linklist.h"
#include "lib/sys_ha.h"

#include "filem.h"
#include "filem_tran.h"
#include "filem_cmd.h"
#include "filem_img.h"
#include "filem_slot.h"
#include "filem_oper.h"
#include "filem_sync.h"
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

static filemslot gfileslottable[FILEM_SLOT_COUN_MAX+1];

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_clrlist                                */
/* Desc: 清除非本槽的同步队列和文件列表                    */
/*---------------------------------------------------------*/
void filem_slot_clrlist(filemslot *pSlot)
{
    filem_show_listclr(&pSlot->synclist);
    filem_show_listclr(&pSlot->filelist[FILEM_MTYPE_NON]);
    filem_show_listclr(&pSlot->filelist[FILEM_MTYPE_BIN]);
    filem_show_listclr(&pSlot->filelist[FILEM_MTYPE_CFG]);
    filem_show_listclr(&pSlot->filelist[FILEM_MTYPE_DAT]);
    filem_show_listclr(&pSlot->filelist[FILEM_MTYPE_VER]);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_config                                 */
/* Desc: 设备管理的槽位配置数据                            */
/*---------------------------------------------------------*/
static void filem_slot_config(int cmd, filemslot *pSlot)
{
    if((pSlot->slot > 0) &&
       (pSlot->slot < FILEM_SLOT_COUN_MAX))
    {
        filemslot *pObjSlt = &gfileslottable[pSlot->slot];
        
        switch(cmd)
        {
            case FILEM_SLOT_EXSERT:

               //if(pObjSlt->slot != gfilelocslot)
                 {
                     filem_slot_clrlist(pObjSlt);

                     memset(pObjSlt, 0, sizeof(filemslot));
                 }
 
                 break;
                 
            case FILEM_SLOT_INSERT:
                
                 pObjSlt->slot     = pSlot->slot;
                 pObjSlt->hwstate  = 1;
                 pObjSlt->boardid  = pSlot->boardid;
                 pObjSlt->iscpu    = pSlot->iscpu;
                 pObjSlt->confsync = FILEM_CFGSYNC_COUNT;
                 pObjSlt->issync   = 0;
                 pObjSlt->hastat   = 0;
                 pObjSlt->sysstat  = 0;
                 pObjSlt->syntimer = 0;
                 pObjSlt->gstTimer = pSlot->iscpu ? FILEM_STATEGET_TIMER : 0;
                 
                 pObjSlt->filestainf.iSlotFlags = 0;
                 pObjSlt->filestainf.iSlotNum   = pSlot->slot;
                 
                 if(pObjSlt->slot == gfilelocslot)
                 {
                     pObjSlt->filestainf.iSlotFlags |= FILEM_SLOT_FLAG_MAINCPU;
                 }
                 
                //if(pObjSlt->slot == gfilelocslot)
                 {
                     pObjSlt->filestainf.iL3Vpn = filem_tftp_l3vpn_get(); /* only use for local slot*/
                 }

                 filem_oper_str2low(pSlot->name, FILEM_DEVNAME_MLEN, pObjSlt->name);
                 
                 filem_oper_str2low(pSlot->hwinfo, FILEM_DEVNAME_MLEN, pObjSlt->hwinfo);

                 break;
                 
            case FILEM_SLOT_CONFIG:

                 pObjSlt->boardid  = pSlot->boardid;
                 pObjSlt->iscpu    = pSlot->iscpu;

                 filem_oper_str2low(pObjSlt->name, FILEM_DEVNAME_MLEN, pSlot->name);

                 filem_oper_str2low(pSlot->hwinfo, FILEM_DEVNAME_MLEN, pObjSlt->hwinfo);

                 break;
            default: 
                break;
        }
    }
}


/*---------------------------------------------------------*/
/* Name: filem_slot_brdname                                */
/* Retu: 0  1                                              */
/* Desc: 获取板卡名                                        */
/*---------------------------------------------------------*/
int filem_slot_brdname(int iSlot, char *psname)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if((gfileslottable[iSlot].hwstate) &&
       (gfileslottable[iSlot].slot == iSlot))
    {
         strcpy(psname, gfileslottable[iSlot].name);
 
         return(0);
    }
    else return(-1);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_brdid                                  */
/* Retu: 0  1                                              */
/* Desc: 获取板id                                             */
/*---------------------------------------------------------*/
unsigned int filem_slot_brdid(int iSlot)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if((gfileslottable[iSlot].hwstate) &&
       (gfileslottable[iSlot].slot == iSlot))
    {
         return(gfileslottable[iSlot].boardid);
    }
    else return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_fpgahwinf                              */
/* Retu: 0  1                                              */
/* Desc: 获取FPGA硬件信息                                  */
/*---------------------------------------------------------*/
int filem_slot_fpgahwinf(int iSlot, char *phwinf)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if((gfileslottable[iSlot].hwstate) &&
       (gfileslottable[iSlot].slot == iSlot))
    {
         strcpy(phwinf, gfileslottable[iSlot].hwinfo);
 
         return(0);
    }
    else return(-1);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_sendmsg                                */
/* Retu: 0  1                                              */
/* Desc: 向一个CPU 槽位发送文件管理HA消息                  */
/*---------------------------------------------------------*/

int filem_slot_sendmsg(struct ipc_mesg_n *pMsg, int iLen)
{
    int iresult =0;
    
    iresult = ipc_send_msg_ha_n1(pMsg,  iLen);

    if(iresult != 0) mem_share_free(pMsg, MODULE_ID_FILE);

    return(iresult);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_sendmsg                                */
/* Retu: 0  1                                              */
/* Desc: 向一个CPU 槽位发送文件管理HA消息                  */
/*---------------------------------------------------------*/

int filem_slot_sendcpmsg(struct ipc_mesg_n *pMsg, int iLen)
{
    int                iresult = -1;
    struct ipc_mesg_n *pSndMsg = NULL;
           
    pSndMsg = mem_share_malloc(iLen, MODULE_ID_FILE);
    
    if(pSndMsg != NULL)
    {
        memcpy(pSndMsg, pMsg, iLen);
           
        iresult = ipc_send_msg_ha_n1(pSndMsg,  iLen);
           
        if(iresult != 0) mem_share_free(pSndMsg, MODULE_ID_FILE);
    }
   
    return(iresult);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_filelist                               */
/* Retu: 0  1                                              */
/* Desc: 获取某槽位某类文件列表                            */
/*---------------------------------------------------------*/
struct list* filem_slot_filelist(int iSlot, int iType)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if(iSlot < FILEM_SLOT_COUN_MAX)
    {
         if((gfileslottable[iSlot].hwstate) &&
            (gfileslottable[iSlot].iscpu))
              return(&gfileslottable[iSlot].filelist[iType]);
         else return(NULL);
    }
    else return(NULL);

}

/*---------------------------------------------------------*/
/* Name: filem_slot_objectget                              */
/* Retu: 0  1                                              */
/* Desc: 获取本槽位数据结构                                */
/*---------------------------------------------------------*/
filemslot* filem_slot_objectget(int iSlot)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if(iSlot < FILEM_SLOT_COUN_MAX)
    {
         return(&gfileslottable[iSlot]);
    }
    else return(NULL);   

}

/*---------------------------------------------------------*/
/* Name: filem_slot_stainfoget                             */
/* Retu: 0  1                                              */
/* Desc: 获取本槽位文件操作状态数据                        */
/*---------------------------------------------------------*/
filemstatus* filem_slot_stainfoget(int iSlot)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if(iSlot < FILEM_SLOT_COUN_MAX)
    {
         return(&gfileslottable[iSlot].filestainf);
    }
    else return(NULL);   
}

/*---------------------------------------------------------*/
/* Name: filem_slot_knltblget                              */
/* Retu: 0  1                                              */
/* Desc: 获取本槽位kernel状态数据                          */
/*---------------------------------------------------------*/
kernelarea* filem_slot_knltblget(int iSlot)
{
    if(iSlot == 0) iSlot = gfilelocslot;

    if(iSlot < FILEM_SLOT_COUN_MAX)
    {
         return(gfileslottable[iSlot].fileknltbl);
    }
    else return(NULL);   
}

/*---------------------------------------------------------*/
/* Name: filem_slot_stanextget                             */
/* Retu: null or obj                                       */
/* Desc: 获取下一槽位文件操作状态数据                      */
/*---------------------------------------------------------*/
filemstatus* filem_slot_stanextget(int index, int exact)
{
    filemstatus *pStaObj = NULL;

    if(exact)
    {
        if((index > 0) && (index < FILEM_SLOT_COUN_MAX)) 
        {
            if(gfileslottable[index].hwstate)             //&& (gfileslottable[index].iscpu))
            {
                pStaObj = &gfileslottable[index].filestainf;
            }
        }
    }
    else
    {
        index = (index <= 0) ? 1 : (index + 1);
        
        for(; index < FILEM_SLOT_COUN_MAX; index++)
        {
            if(gfileslottable[index].hwstate)             //&& (gfileslottable[index].iscpu))
            {
                pStaObj = &gfileslottable[index].filestainf;
                break;
            }
        }
    }

    return(pStaObj);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_cpuhwcheck                             */
/* Retu: 0  1                                              */
/* Desc: 槽位硬件检查 是否在位 是否为处理器                */
/*---------------------------------------------------------*/
int filem_slot_cpuhwcheck(int iSlot)
{
    if(iSlot == gfilelocslot) return(1);

    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX)) return(0);

    if(gfileslottable[iSlot].hwstate && gfileslottable[iSlot].iscpu) return(1);

    return(0);   
}

/*---------------------------------------------------------*/
/* Name: filem_slot_canrtsync                              */
/* Retu: 0  1                                              */
/* Desc: 检查是否可以执行实时同步                                  */
/*---------------------------------------------------------*/
int filem_slot_canrtsync(int iDes)
{
    int iSrc = gfilelocslot;
    
    if(gfileslottable[iDes].hwstate && 
       gfileslottable[iDes].iscpu &&
      (gfileslottable[iSrc].hastat == HA_ROLE_MASTER))
    {
         return(1);
    }
    else return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_slot_cansync                                */
/* Retu: 0  1                                              */
/* Desc: 检查是否可以同步                                  */
/*---------------------------------------------------------*/
int filem_slot_cansync(int iDes)
{
    int iSrc = gfilelocslot;
    
    if(gfileslottable[iDes].hwstate && 
       gfileslottable[iDes].iscpu &&
      !gfileslottable[iSrc].issync &&
      (gfileslottable[iSrc].hastat == HA_ROLE_MASTER))
    {
         return(!filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_COPY | FILEM_OPTACT_DLOAD));
    }
    else return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_slot_ismaster                               */
/* Retu: 0  1                                              */
/* Desc: 检查是否为主板                                    */
/*---------------------------------------------------------*/
int filem_slot_ismaster(void)
{   
    return(gfileslottable[gfilelocslot].hastat == HA_ROLE_MASTER);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_isinsert                               */
/* Retu: 0  1                                              */
/* Desc: 检查板卡是否在位                                  */
/*---------------------------------------------------------*/
int filem_slot_isinsert(int iSlot)
{
    if(iSlot == gfilelocslot) return(1);

    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX)) return(0);

    if(gfileslottable[iSlot].hwstate) return(1);

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_issubcpu                               */
/* Retu: 0  1                                              */
/* Desc: 检查板卡是否为子CPU                                       */
/*---------------------------------------------------------*/
int filem_slot_ismaincpu(int iSlot)
{
    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX)) return(0);

    if(!gfileslottable[iSlot].hwstate) return(0);

    if(!gfileslottable[iSlot].iscpu) return(0);

    if((gfileslottable[iSlot].filestainf.iSlotFlags & FILEM_SLOT_FLAG_MAINCPU) == 0) return(0);

    return(1);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_issubcpu                               */
/* Retu: 0  1                                              */
/* Desc: 检查板卡是否为子CPU                                       */
/*---------------------------------------------------------*/
int filem_slot_issubcpu(int iSlot)
{
    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX)) return(0);

    if(!gfileslottable[iSlot].hwstate) return(0);

    if(!gfileslottable[iSlot].iscpu) return(0);

    if((gfileslottable[iSlot].filestainf.iSlotFlags & FILEM_SLOT_FLAG_MAINCPU) != 0) return(0);

    return(1);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_setsync                                */
/* Retu: 0  1                                              */
/* Desc: 设置同步标志                                      */
/*---------------------------------------------------------*/
void filem_slot_setsync(int iSlot, int iSync)
{
    if(iSlot == 0) iSlot = gfilelocslot;
    
    gfileslottable[iSlot].issync = iSync;
}

/*---------------------------------------------------------*/
/* Name: filem_slot_isnosync                               */
/* Retu: 0  1                                              */
/* Desc: 检查是否可以同步                                  */
/*---------------------------------------------------------*/
int filem_slot_isnosync(void)
{
    int iSrc = gfilelocslot;
    
    if(!gfileslottable[iSrc].issync && 
       (gfileslottable[iSrc].hastat == HA_ROLE_MASTER))
    {
         return(1);
    }
    else return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_slot_slaveslot                              */
/* Retu: 0  1                                              */
/* Desc: 获取SLAVE槽位                                  */
/*---------------------------------------------------------*/

int filem_slot_slaveslot(void)
{
    int index = 1;
    
    for(; index < FILEM_SLOT_COUN_MAX; index++)
    {
        if((gfileslottable[index].hwstate) &&
           (gfileslottable[index].iscpu) && 
           (gfileslottable[index].hastat == HA_ROLE_SLAVE))
        {
            break;
        }
    }

    return((index < FILEM_SLOT_COUN_MAX) ? index : 0);
}

/*---------------------------------------------------------*/
/* Name: filem_slot_oneconfig                              */
/* 配置槽位信息  处理槽位拔插事件                          */
/*---------------------------------------------------------*/
static void filem_slot_oneconfig(void *pSlot)
{
    int       icmd = 0;
    filemslot sSlotInf;
    
    struct devm_slot *pDevSlt =(struct devm_slot*)pSlot;

    memset(&sSlotInf, 0, sizeof(filemslot));
    
    sSlotInf.slot     =  pDevSlt->slot;
    sSlotInf.hwstate  = (pDevSlt->status == DEV_STATUS_WORK);
    sSlotInf.boardid  =  pDevSlt->id;
    sSlotInf.iscpu    = (pDevSlt->hw.cpu_type != 0);
    
    icmd = sSlotInf.hwstate ? FILEM_SLOT_INSERT : FILEM_SLOT_EXSERT;
    
    strncpy(sSlotInf.name, (char*)pDevSlt->name, FILEM_DEVNAME_MLEN -1);  

    strncpy(sSlotInf.hwinfo, (char*)pDevSlt->hw.hw_ver, FILEM_DEVNAME_MLEN -1);
    
    filem_slot_config(icmd, &sSlotInf);

    printf("filem_slot_oneconfig slot=%d, name =%s, state=%d, cpu=%d\n",pDevSlt->slot, pDevSlt->name, sSlotInf.hwstate, sSlotInf.iscpu); 
}


/*---------------------------------------------------------*/
/* Name: filem_slot_allget                                 */
/* 获取所有槽位板卡信息 启动时调用                         */
/*---------------------------------------------------------*/
static void filem_slot_allget(void)
{
    int              ret   = 0;
    int              islot = 1;
    struct devm_slot pDevSlt;
    struct devm_box  pDevBox;
    enum DEV_TYPE DevType = DEV_TYPE_INVALID;
    
    printf(" filem_slot_get status \n");
    
    if(devm_comm_get_dev_type(MODULE_ID_FILE, &DevType) != 0)
    {
        zlog_err("%s[%d]: filem get device type error, device type is %d\n",__FILE__,__LINE__, DevType);        
    }
    
    switch(DevType)
    {
        case DEV_TYPE_BOX: /* 盒式设备，默认槽位号为 1，板卡名称为 "main"，板卡 ID 为0 */   
            
             gfileslottable[1].slot    =  1;
             gfileslottable[1].hwstate =  1;
             gfileslottable[1].iscpu   =  1;
             gfileslottable[1].issync  =  1;
	         gfileslottable[1].hastat  =  HA_ROLE_MASTER;
	         gfileslottable[1].sysstat =  HA_BOOTSTA_DATAOK;
            /*
             strcpy(gfileslottable[1].name, FILEM_BRD_NAME);
            */
             strcpy(gfileslottable[1].name, gbfiledevname);
            
             memset(&pDevBox, 0, sizeof(struct devm_box));
             
             ret = devm_comm_get_box(MODULE_ID_FILE, &pDevBox);

             printf("filem devm_comm_get_box, ret=%d, hwver=%s\r\n", ret, pDevBox.hw.hw_ver);
             
             if(ret == 0)
             {        
                 gfileslottable[1].boardid = pDevBox.id;
                 
                 filem_oper_str2low((char*)pDevBox.hw.hw_ver, FILEM_DEVNAME_MLEN, gfileslottable[1].hwinfo);
             }
             break;
            
        case DEV_TYPE_SHELF:/* 单框设备，例如 2000 系列有背板的设备 */          
    
             for(islot = 1; islot < FILEM_SLOT_COUN_MAX; islot++)
             {
                 ret = devm_comm_get_slot(1, islot, MODULE_ID_FILE, &pDevSlt);

                 if(ret == 0) filem_slot_oneconfig(&pDevSlt);
             }
             break;
            
        case DEV_TYPE_STACK:/* 多框堆叠设备，暂时没有 */
             break;

        default: break;
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_confsyncreset                          */
/* 重新设置配置数据文件同步                         */
/*---------------------------------------------------------*/
static void filem_slot_confsyncreset(void)
{
     int index = 0;
     
     for(index = 1; index < FILEM_SLOT_COUN_MAX; index++)
     {
         if((index != gfilelocslot) &&
            gfileslottable[index].hwstate && 
            gfileslottable[index].iscpu) 
         {
             gfileslottable[index].confsync = FILEM_CFGSYNC_COUNT;
         }
     }
}
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_hastatset                              */
/* 设置本槽位的HA状态,启动文件同步                         */
/*---------------------------------------------------------*/
static void filem_slot_hastatset(struct devm_com *pdevm)
{
    filemslot *pslot  = NULL;
    int        iRole  = 0;
	int        iOldR  = 0;
    int        iStat  = 0;
    int        iOldS  = 0;
    int        iMslot = 0;
    
	pslot = filem_slot_objectget(0);

	iOldR = pslot->hastat;
    iOldS = pslot->sysstat;    //记录原状态

    iRole = pdevm->ha_role;
    iStat = pdevm->ha_status; //HA_BOOTSTA_DATAOK; 

    if(iRole == HA_ROLE_MASTER) 
         iMslot = (int)pdevm->slot;
    else iMslot = (int)pdevm->main_slot;
        
	pslot->munit = 0;
    pslot->mslot = iMslot;
	
	if((iOldR == iRole) && (iOldS == iStat)) return;
	
  //printf("filem_slot_hastatset slot=%d, role=%d, bootstat=%d, mslot=%d\r\n", pslot->slot, iRole, pdevm->ha_status, iMslot);
    
    switch(iRole)
    {
        case HA_ROLE_INIT:

             pslot->hastat   = iRole;
             pslot->sysstat  = iStat;
             pslot->syntimer = 0;          /*停止上报和接收*/

             pslot->filestainf.iBootStat = iStat;

             if(iOldR != iRole)
             {
                 filem_sync_forcestop(iOldR);

                 filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_ALL);
             }

             break;
                
        case HA_ROLE_SLAVE:

             pslot->hastat  = iRole;
             pslot->sysstat = iStat;
             
             pslot->filestainf.iBootStat = iStat;
             
             if(iStat >= HA_BOOTSTA_END)
             {
                 if(pslot->syntimer <= 0)
                 {                     
                     pslot->syntimer = FILEM_FRESH_TIMER;
                 }                        /*启动备用自动刷新上报*/
             }
             else
             {
                 if(pslot->syntimer != 0) pslot->syntimer = 0;
                                         /*关闭备用自动刷新上报*/                 
             }

             if(iOldR != iRole)
             {
                 filem_sync_forcestop(iOldR);
                                         /*由执行指令控制禁入*/
                 filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_ALL);
             }

             break;


        case HA_ROLE_MASTER:

			 pslot->hastat   = iRole;
	         pslot->sysstat  = iStat;
	         pslot->syntimer = 0;

             pslot->filestainf.iBootStat = iStat;
	                                     /*MASTER不会向其他板卡主动发信息*/
	         if(iOldR != iRole) 
             {
                 filem_sync_forcestop(iOldR);
                 
                 filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_ALL);

                 filem_slot_confsyncreset();
             }

             break;
                                         
        default: break;
    }
}

/*---------------------------------------------------------*/
/* Name: filem_slot_hastatmodi                             */
/* 设置本槽位的HA状态,启动文件同步                           */
/*---------------------------------------------------------*/
static void filem_slot_hastatmodi(struct devm_com *pdevm)
{
    filemslot *pslot  = NULL;
    
	pslot = filem_slot_objectget((int)pdevm->slot);

    if(pslot != NULL)  pslot->hastat = pdevm->ha_role;
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_devmmsg                                */
/* Retu: 0  1                                              */
/* Desc: 槽位信息初始化                                    */
/*---------------------------------------------------------*/
void filem_slot_devmmsg(struct ipc_mesg_n *pMsg, int iLen)
{
	struct devm_slot     sDevSlt;
    struct devm_com     *pDevCom = NULL;
	struct ipc_msghdr_n *pMsghdr = NULL;

    pMsghdr = &pMsg->msghdr;

    switch(pMsghdr->msg_subtype)
    {
        case DEV_EVENT_HA_BOOTSTATE_CHANGE:

             pDevCom = (struct devm_com *)pMsg->msg_data;
             
             if(pDevCom->slot == gfilelocslot) 
             {
                  filem_slot_hastatset(pDevCom);
             }
             else
             {
                  if(pDevCom->slot != 0) filem_slot_hastatmodi(pDevCom);
             }

             printf("filem_slot_devmmsg msgslot=%d, locslot=%d, harole=%d\r\n", pDevCom->slot, gfilelocslot, pDevCom->ha_role);
             
             break;
             
        case DEV_EVENT_SLOT_ADD:
            
              memset(&sDevSlt, 0, sizeof(struct devm_slot));
            
              pDevCom = (struct devm_com*)pMsg->msg_data;
              
              if(devm_comm_get_slot(1, pDevCom->slot, MODULE_ID_FILE, &sDevSlt) == 0)
              {
                  sDevSlt.slot = pDevCom->slot;
                 
                  filem_slot_oneconfig(&sDevSlt);
                 
                  filem_slot_hastatmodi(pDevCom);
              }
                       
              break;

        
        case DEV_EVENT_SLOT_DELETE:
            
		     memset(&sDevSlt, 0, sizeof(struct devm_slot));

             pDevCom = (struct devm_com*)pMsg->msg_data;
             
           //ret = devm_comm_get_slot(1, pDevCom->slot, MODULE_ID_FILE, &sDevSlt);

             pDevCom->ha_role = 0;
                
           //sDevSlt.status = 0;
                        
             sDevSlt.slot = pDevCom->slot;
             
             filem_slot_oneconfig(&sDevSlt);

             filem_slot_hastatmodi(pDevCom);
            
             break;
             
        default:
             break;
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_init                                   */
/* Retu: 0  1                                              */
/* Desc: 槽位信息初始化                                    */
/*---------------------------------------------------------*/
int filem_slot_init(void)
{
    memset(gfileslottable, 0, sizeof(filemslot)*(FILEM_SLOT_COUN_MAX + 1));

    filem_slot_allget();
    
    gfileslottable[gfilelocslot].filestainf.iSlotNum   = gfilelocslot;
    gfileslottable[gfilelocslot].filestainf.iSlotFlags = FILEM_SLOT_FLAG_MAINCPU;
    
    return(0);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_slot_info                                   */
/* Retu: 0  1                                              */
/* Desc: 槽位信息显示                                      */
/*---------------------------------------------------------*/
int filem_slot_info(char **bBufDat)
{
    int        index   = 0;
    int        iAllLen = 0;
    char      *bBrdInf = NULL;
    filemslot *pSlot   = NULL;
    
    bBrdInf = XMALLOC(MTYPE_FILEM_ENTRY, FILEM_SLOT_COUN_MAX * 160);

    if(bBrdInf == NULL) return(-1);
    
    iAllLen += sprintf(bBrdInf + iAllLen, "Slot Hw    BrdID cpu syn timer Name                     \r\n" \
                                          "---- -- -------- --- --- ----- --------------------------\r\n");

    for(index = 1; index < FILEM_SLOT_COUN_MAX; index++)
    {
        pSlot = &gfileslottable[index];
        
        if(pSlot->slot != 0)
        {
            iAllLen += sprintf(bBrdInf + iAllLen, "%4d %2d %8x %3d %3d %5d %s\r\n",
                               pSlot->slot,  pSlot->hwstate, pSlot->boardid,
                               pSlot->iscpu, pSlot->issync,  pSlot->syntimer, pSlot->name);
        }
    }

    iAllLen += sprintf(bBrdInf + iAllLen, "---- -- -------- --- --- ----- --------------------------\r\n");
    iAllLen += sprintf(bBrdInf + iAllLen, "device name=%s\r\n", gbfiledevname);
                                          

   *bBufDat = bBrdInf;

    return(iAllLen);
}


/*---------------------------------------------------------*/
/* Name: filem_slot_verinfo                                */
/* Retu: 0  1                                              */
/* Desc: 槽位信息显示                                      */
/*---------------------------------------------------------*/
int filem_slot_verinfo(char *bBrdInf, int islot)
{
    int         iAllLen = 0;
    filemslot  *pSlot   = NULL;
    char        bTimeDt[64] = {0};
    const char *bBootSt[ 4] = {"init", "init", "bootend", "dataok"};
    const char *bHastat[ 4] = {"init", "init", "master", "slave"};

    pSlot = filem_slot_objectget(islot);

    if(pSlot == NULL)  return(-1);
        
    if(pSlot->hwstate && pSlot->iscpu) 
    {
         iAllLen += sprintf(bBrdInf + iAllLen, "Slot%d\r\n", pSlot->slot);
         
         iAllLen += sprintf(bBrdInf + iAllLen, "------------------ --------------------------\r\n");

         iAllLen += sprintf(bBrdInf + iAllLen, "HaRole             : %s\r\n", bHastat[pSlot->hastat % 4]); 
         
         iAllLen += sprintf(bBrdInf + iAllLen, "UploadStatus       : %s\r\n", filem_sys_errget(-pSlot->filestainf.iUpload)); 
 
         iAllLen += sprintf(bBrdInf + iAllLen, "DownloadStatus     : %s\r\n", filem_sys_errget(-pSlot->filestainf.iDownload)); 
 
         iAllLen += sprintf(bBrdInf + iAllLen, "AppUpgradeStatus   : %s\r\n", filem_sys_errget(-pSlot->filestainf.iUpgrade)); 
 
         filem_show_timetostr(pSlot->filestainf.iUpgradeTime, bTimeDt);
             
         iAllLen += sprintf(bBrdInf + iAllLen, "AppUpgradeTime     : %s\r\n", bTimeDt); 

    //   iAllLen += sprintf(bBrdInf + iAllLen, "AppUpgradeStatus   : %s\r\n", filem_sys_errget(-pSlot->filestainf.iUpgFpga)); 
         iAllLen += sprintf(bBrdInf + iAllLen, "FpgaUpgradeStatus  : %s\r\n", filem_sys_errget(-pSlot->filestainf.iUpgFpga)); 

         iAllLen += sprintf(bBrdInf + iAllLen, "KernelUpgradeStatus: %s\r\n", filem_sys_errget(-pSlot->filestainf.iUpgKnl)); 
 
         filem_show_timetostr(pSlot->filestainf.iUpgFpgaTime, bTimeDt);
             
     //  iAllLen += sprintf(bBrdInf + iAllLen, "FpgaUpgradeTime    : %s\r\n", bTimeDt); 

         iAllLen += sprintf(bBrdInf + iAllLen, "Ftp/Tftp L3vpn     : %d\r\n", pSlot->filestainf.iL3Vpn); 
     
         iAllLen += sprintf(bBrdInf + iAllLen, "DiskfreeSpace      : %u\r\n", pSlot->filestainf.iFreeSpace); 

         iAllLen += sprintf(bBrdInf + iAllLen, "Boot status        : %s\r\n", bBootSt[pSlot->filestainf.iBootStat % 4]); 

         iAllLen += sprintf(bBrdInf + iAllLen, "Appversion         : %s\r\n", pSlot->filestainf.cSoftVer); 
 
         iAllLen += sprintf(bBrdInf + iAllLen, "FPGAversion        : %s\r\n", pSlot->filestainf.cFpgaVer); 
 
         iAllLen += sprintf(bBrdInf + iAllLen, "Kernelversion      : %s\r\n", pSlot->filestainf.cKernelVer); 

         iAllLen += sprintf(bBrdInf + iAllLen, "Bootversion        : %s\r\n", pSlot->filestainf.cBootVer); 
         
         return(iAllLen);
    }
    else return(-1);                                     
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/


