/* file process command 
   name filem_cmd.c
   edit suxq
   date 2016/05/26
*/
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#include <zebra.h>

#include "lib/types.h"
#include "lib/memory.h"
#include "lib/md5.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include "lib/log.h"
#include "lib/vty.h"
#include "lib/command.h"
#include "ftm/pkt_udp.h"
#include "lib/module_id.h"
#include "lib/devm_com.h"
#include "lib/sys_ha.h"
#include "lib/inet_ip.h"
#include "filem.h"
#include "filem_tftp.h"
#include "filem_tran.h"
#include "filem_cmd.h"
#include "filem_oper.h"
#include "filem_img.h"
#include "filem_slot.h"
#include "filem_sync.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
static void *filem_cmd_cmdprocess(void *pPara);

static int   filem_cmd_threadcreate(void * (*pfun)(void*), int iStackSz, int iPrio, void *pPara);

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_notifycall                              */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 操作结果通知主槽                                  */
/*---------------------------------------------------------*/
static void filem_cmd_notifycall(int iResult, char *pResInfo, void *pPar, int iFlag)
{
    if(pResInfo) printf("%s", pResInfo);

    //send command result to master 
	
	printf(" send command result to master !\r\n");
	
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_order_set                           */
/*---------------------------------------------------------*/
static void filem_cmd_order_set(filemoperate *pDest, filemoperate *pSrc) 
{    
    memset(pDest, 0, sizeof(filemoperate));

    pDest->iCmdType = htonl(pSrc->iCmdType);
    pDest->iProType = htonl(pSrc->iProType);
    pDest->iSlotno  = htonl(pSrc->iSlotno);
    pDest->iResPort = htonl(pSrc->iResPort);
    pDest->iFlag    = htonl(pSrc->iFlag);
    pDest->iDtype   = htonl(pSrc->iDtype);
     
    memcpy(pDest->bIpAddr,  pSrc->bIpAddr,  FILEM_ADDR_STRLN);
    memcpy(pDest->bUsrName, pSrc->bUsrName, FILEM_USER_STRLN);
    memcpy(pDest->bPassWrd, pSrc->bPassWrd, FILEM_USER_STRLN);

    memcpy(pDest->bSrcName, pSrc->bSrcName, FILEM_NAME_MAXLN);
    memcpy(pDest->bDesName, pSrc->bDesName, FILEM_NAME_MAXLN);
    memcpy(pDest->bMd5Data, pSrc->bMd5Data, FILEM_PAD_MD5_BUSZ + 8);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_order_get                           */
/*---------------------------------------------------------*/
static void filem_cmd_order_get(filemoperate *pDest, filemoperate *pSrc) 
{    
    memset(pDest, 0, sizeof(filemoperate));

    pDest->iCmdType = ntohl(pSrc->iCmdType);
    pDest->iProType = ntohl(pSrc->iProType);
    pDest->iSlotno  = ntohl(pSrc->iSlotno);
    pDest->iResPort = ntohl(pSrc->iResPort);
    pDest->iFlag    = ntohl(pSrc->iFlag);
    pDest->iDtype   = ntohl(pSrc->iDtype);
     
    memcpy(pDest->bIpAddr,  pSrc->bIpAddr,  FILEM_ADDR_STRLN);
    memcpy(pDest->bUsrName, pSrc->bUsrName, FILEM_USER_STRLN);
    memcpy(pDest->bPassWrd, pSrc->bPassWrd, FILEM_USER_STRLN);

    memcpy(pDest->bSrcName, pSrc->bSrcName, FILEM_NAME_MAXLN);
    memcpy(pDest->bDesName, pSrc->bDesName, FILEM_NAME_MAXLN);
    memcpy(pDest->bMd5Data, pSrc->bMd5Data, FILEM_PAD_MD5_BUSZ + 8);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_send_command                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 发送指令到其他槽位                                */
/*---------------------------------------------------------*/
static int filem_cmd_send_command(int iSlot, filemoperate *pOper)
{
    /* 向指定槽位发送命令 */
    filemslot *pSlot = NULL;

    pSlot = filem_slot_objectget(iSlot);

    if(pSlot && pSlot->hwstate && pSlot->iscpu)
    {
        char           bInfor[128] = {0};
        struct ipc_mesg_n *pSndMsg = NULL;

        pSndMsg = mem_share_malloc(sizeof(struct ipc_msghdr_n) + sizeof(filemoperate), MODULE_ID_FILE);

        if(pSndMsg)
        {
            memset(&pSndMsg->msghdr, 0, sizeof(struct ipc_msghdr_n));
            
            pSndMsg->msghdr.module_id   = MODULE_ID_FILE;
            pSndMsg->msghdr.sender_id   = MODULE_ID_FILE;
            pSndMsg->msghdr.msg_type    = IPC_TYPE_HA;
            pSndMsg->msghdr.msg_subtype = HA_SMSG_REALT_CMD;
            pSndMsg->msghdr.opcode      = 0;  //IPC_OPCODE_UPDATE
            pSndMsg->msghdr.unit        = pSlot->unit;
            pSndMsg->msghdr.slot        = pSlot->slot;
            pSndMsg->msghdr.srcunit     = pSlot->unit;
            pSndMsg->msghdr.srcslot     = gfilelocslot;
            pSndMsg->msghdr.data_len    = sizeof(filemoperate);
            pSndMsg->msghdr.sequence    = ++pSlot->sndserno;

            filem_cmd_order_set((filemoperate*)pSndMsg->msg_data, pOper);
           
		    sprintf(bInfor, "Send command to slot %d len=%d, OK\r\n", iSlot, sizeof(filemoperate));

            zlog_debug(FILEM_DEBUG_SYNC, "%s", bInfor);

            filem_slot_sendmsg(pSndMsg, sizeof(struct ipc_msghdr_n) + sizeof(filemoperate));
            
            if(pOper->pEndCall) pOper->pEndCall(0, bInfor, pOper->pCallPar, 0);
		}
    }

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_recv_command                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 接收来自主控的指令                                */
/*---------------------------------------------------------*/
int filem_cmd_recv_command(struct ipc_mesg_n *pMsgPkt, int iDlen)
{
    iDlen -= IPC_HEADER_LEN_N; //sizeof(struct ipc_msghdr_n);

    if(iDlen >= (int)sizeof(filemoperate))
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

		if(pCmdPar)
        {   
            filem_cmd_order_get(pCmdPar, (filemoperate*)pMsgPkt->msg_data);

            pCmdPar->pCallPar = NULL;
            
            pCmdPar->pEndCall = filem_cmd_notifycall;

            zlog_debug(FILEM_DEBUG_SYNC, "filem_cmd_recv_command recv iCmdType = %d", pCmdPar->iCmdType);

            switch(pCmdPar->iCmdType)
            {
                case FILEM_OPCMD_UPDATESYS:
                case FILEM_OPCMD_UPDATEPATCH:
                case FILEM_OPCMD_UPDATEFPGA:
                case FILEM_OPCMD_UPDATEAPP:
                case FILEM_OPCMD_UPDATEKNL:
                     {
                         char bInfo[64] = {0};
                         
                         if(filem_upgrade_is_busy())
                         {
                             sprintf(bInfo, "Upgrade operation conflict \r\n");
                             
                             zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
 
                             filem_cmd_notifycall(-FILEM_ERROR_CONFLICT, bInfo, NULL, 0);
                         }
                         else
                         {
                             if((pCmdPar->iFlag & FILEM_OPTFLG_BYSYS) != 0)
                             {
                                 if(filem_md5_integritychk(pCmdPar->bSrcName) != 0)
                                 {
                                     sprintf(bInfo, "Upgrade operation file no integrity!\r\n");
 
                                     zlog_debug(FILEM_DEBUG_UPGRADE,"%s", bInfo);
                                     
                                     filem_cmd_notifycall(-FILEM_ERROR_MD5PADERR, bInfo, NULL, 0);
                                 }
                                 else
                                 {
                                     filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
                                     pCmdPar = NULL;
                                 }
                             }
                             else
                             {
                                 filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
                                 pCmdPar = NULL;
                             }
                         }                         
                     }
                     break;
                     
                case FILEM_OPCMD_DOWNLOAD:
                case FILEM_OPCMD_UPLOAD:
                case FILEM_OPCMD_COPY:
                case FILEM_OPCMD_DELETE:
                case FILEM_OPCMD_RENAME:
                case FILEM_OPCMD_MD5CHK:
                case FILEM_OPCMD_UPDATEUBOOT:
                case FILEM_OPCMD_ACTIVEKNL:

                     filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
                     pCmdPar = NULL;
                     break;

                case FILEM_OPCMD_GETSTATE:
                   
                     if(!filem_slot_ismaster()) 
                     {
                         filemslot *pSlot = NULL;

                         pSlot = filem_slot_objectget(0);

                         if(pSlot != NULL) pSlot->mslot = pMsgPkt->msghdr.srcslot;

                         filem_sync_statussend();
                     }
                     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "Receive get state command, and send data.\r\n");
                     break;
                     
                case FILEM_OPCMD_DBGENABLE:
                default:break;
            }

            if(pCmdPar != NULL) XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
        }
    }

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_inputnamecheck                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 输入名称检查和输出                                */
/*---------------------------------------------------------*/
int filem_cmd_inputnamecheck(int iType, char *pInput, char *pOut)
{
    int   iRetV = 0;
    char  bPath[FILEM_NAME_MAXLN] = {0};
    char  bName[FILEM_NAME_MAXLN] = {0};
    
    if(filem_oper_namechk(pInput) != 0) return(-FILEM_ERROR_NAME);
    
    filem_oper_cutpath(pInput, bPath);
    filem_oper_cutname(pInput, bName);

    if(iType == FILEM_MTYPE_NON)
    {
        if(filem_oper_pathchk(bPath) == 0) 
        {
             if(strlen(bName) > 0)
                  strcpy(pOut, pInput);
             else iRetV = -FILEM_ERROR_NAME;
        }
        else iRetV = -FILEM_ERROR_PATH;
    }
    else
    {
        if((iType > FILEM_MTYPE_NON) && (iType < FILEM_MTYPE_MAX))
        {
             if(strlen(bPath) == 0)
                  sprintf(pOut, "%s%s", filem_type_pathget(iType), bName);
             else iRetV = -FILEM_ERROR_PATH;
        }
        else iRetV = -FILEM_ERROR_UNKTYPE;
    }

    if(iRetV < 0)  
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Input file %s error, %s!", pInput, filem_sys_errget(iRetV));
        
        return(iRetV);
    }
    return(iRetV);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_inputipv4check                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 输入名称检查和输出                                */
/*---------------------------------------------------------*/
int filem_cmd_inputipv4check(char *pAddr)
{
    unsigned int iIpAdd = 0;

    iIpAdd = inet_strtoipv4(pAddr);

    return(inet_valid_host(iIpAdd));
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_updsysrun                               */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 执行升级补丁                                      */
/*---------------------------------------------------------*/
static int filem_cmd_runupgredepatch(int iSlot, char *pSysName)
{
    int   iRetV = 0;
    int   iCount = 0;
    int   iCLoop = 0;
    char *pNameL = NULL;
    char *pNameP = NULL;
    char  bBPart[FILEM_NAME_MAXLN] = {0};
    char  bTName[FILEM_NAME_MAXLN] = {0};
    char  bFName[FILEM_NAME_MAXLN] = {0};
#if 0        
    char  bBName[FILEM_NAME_MAXLN] = {0};
    if(filem_slot_brdname(iSlot, bBName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");        
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif
    if(!filem_oper_isexist(pSysName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem patch file %s no exist \r\n", pSysName);
        
        return(-FILEM_ERROR_NOEXIST);
    }

    pNameL = XMALLOC(MTYPE_FILEM_ENTRY, 200 * FILEM_NAME_MAXLN);

    if(pNameL == NULL) return(-FILEM_ERROR_NOMEM);

    zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade slot=%d extract name=%s\r\n", iSlot, pSysName);

    iRetV = filem_oper_extract(pSysName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);

    if(iRetV != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade extract file %s fail, %s\n", pSysName,  filem_sys_errget(iRetV));

        XFREE(MTYPE_FILEM_ENTRY, pNameL);

        filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

        return(iRetV);
    }

    iCount = filem_oper_namelist(filem_type_pathget(FILEM_MTYPE_TMP), NULL, 200 * FILEM_NAME_MAXLN, pNameL);

  
    sprintf(bBPart, "%04x_patch", filem_slot_brdid(iSlot));

    iCLoop = 0;
    
    pNameP = pNameL;
    
    memset(bFName, 0, FILEM_NAME_MAXLN);
    memset(bTName, 0, FILEM_NAME_MAXLN);
    
    while(iCLoop++ < iCount)
    {
        strncpy(bTName, pNameP, FILEM_NAME_MAXLN - 1);

        pNameP += strlen(pNameP) + 1;

        if(strstr(bTName, bBPart) != NULL)
        {
            sprintf(bFName, "%s%s", filem_type_pathget(FILEM_MTYPE_TMP), bTName);
            
            break;
        }
    }

    if(strlen(bFName) == 0) 
    {
        iRetV = -FILEM_ERROR_NOEXIST;
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade slot=%d filename no found, icount=%d\r\n", iSlot, iCount);
    }
    else
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade slot=%d filename=%s\r\n", iSlot, bFName);
        
        iRetV = filem_oper_extract(bFName, filem_type_pathget(FILEM_MTYPE_BIN), NULL, NULL, 0);
    }
    
    XFREE(MTYPE_FILEM_ENTRY, pNameL);

    filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradefpgas                         */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据系统文件升级                                  */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradefpgas(int iSlot, char *psName, int extra)
{
    int   iCount = 0;
    int   iCLoop = 0;
    int   iItems = 0;
    int   iFpgaX = 0;
    int   iRetVa = -FILEM_ERROR_NOEXIST;
    
    char *pNameL = NULL;
    char *pNameP = NULL;
    char *bField[6] = {NULL};
    char  bHwInf[FILEM_PATH_MAXLN] = {0};
    char  bTName[FILEM_NAME_MAXLN] = {0};  /*目录下一个文件*/
    char  bFName[FILEM_NAME_MAXLN] = {0};  /*FPGA文件的一部分*/
    char  bDName[FILEM_NAME_MAXLN] = {0};  /*最后文件名*/

#if 0        
    char  bBName[FILEM_NAME_MAXLN] = {0};  
    if(filem_slot_brdname(iSlot, bBName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif
    if(filem_slot_fpgahwinf(iSlot, bHwInf) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade get slot fpga hw infor error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade get slot=%d fpga hw infor:%s\r\n", iSlot, bHwInf);

    pNameL = XMALLOC(MTYPE_FILEM_ENTRY, 200 * FILEM_NAME_MAXLN);

    if(pNameL == NULL) return(-FILEM_ERROR_NOMEM);
    
    if(extra)
    {       
        if(!filem_oper_isexist(psName))
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade file no exist %s\n", psName);

            XFREE(MTYPE_FILEM_ENTRY, pNameL);

            return(-FILEM_ERROR_NOEXIST);
        }
        else
        {
            iRetVa = filem_oper_extract(psName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);        

            if(iRetVa != 0)
            {
                zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade extract file %s fail, %s\n", psName,  filem_sys_errget(iRetVa));

                XFREE(MTYPE_FILEM_ENTRY, pNameL);

                filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

                return(iRetVa);
            }
        }
    }
    
    iCount = filem_oper_namelist(filem_type_pathget(FILEM_MTYPE_TMP), NULL, 200 * FILEM_NAME_MAXLN, pNameL);

    iItems = filem_oper_nameparse(bHwInf, bField, 5, ';');

    if(iItems > 1)   /*除第一个空串外, 过滤掉最后一个空串*/
    {
        if(strlen(bField[iItems - 1]) == 0) iItems --;
    }

    if(iItems <= 0) 
    {
        iItems = 1;
        bHwInf[0] = '\0';
        bField[0] = bHwInf;
    }
       
    zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade nameparse, iItems=%d field0=%s\r\n", iItems, bField[0]);

    while(iFpgaX < iItems)
    {
        if(strlen(bField[iFpgaX]) == 0)
             sprintf(bFName, "%04x_fpga", filem_slot_brdid(iSlot));
        else sprintf(bFName, "%04x_fpga-%s", filem_slot_brdid(iSlot), bField[iFpgaX]);
        

        iFpgaX ++;

        pNameP = pNameL;
        
        iCLoop = iRetVa = 0;

        memset(bDName, 0, FILEM_NAME_MAXLN);
        memset(bTName, 0, FILEM_NAME_MAXLN);
        
        while(iCLoop++ < iCount)
        {
            strncpy(bTName, pNameP, FILEM_NAME_MAXLN-1);

            pNameP += strlen(pNameP) + 1;

            if(strstr(bTName, bFName) != NULL)
            {
                sprintf(bDName, "%s%s", filem_type_pathget(FILEM_MTYPE_TMP), bTName);
                
                zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade slot=%d filename=%s index=%d\r\n", iSlot, bDName, iFpgaX);

                if(filem_oper_isexist(bDName))
                {
                     iRetVa = filem_fpga_upgrade(iSlot, iFpgaX, bDName);

                     if(iRetVa != 0)
                     {
                         zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade slot=%d fail, %s\n", iSlot, filem_sys_errget(iRetVa));
                     }
                }
                else 
                {
                    iRetVa = -FILEM_ERROR_NOEXIST;
                    
                    zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade fail, fpga  file %s no exist\n", bTName);
                }

                break;
            }
            
            if(iRetVa < 0) break;
        }

        if(iRetVa < 0) break;

        if(strlen(bDName) == 0)    /*没找到匹配文件*/
        {
            iRetVa = -FILEM_ERROR_NOEXIST;
            zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade fail, fpga  file %s no exist\n", bFName);
            break;
        }
    }
    
    XFREE(MTYPE_FILEM_ENTRY, pNameL);

    if(extra) filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetVa);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradefpgaf                         */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据具体文件升级                                  */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradefpgaf(int iSlot, char *pDName, int index)
{
    int  iRetVa = 0;
    
    if(!filem_oper_isexist(pDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade file no exist %s\n", pDName);

        return(-FILEM_ERROR_NOEXIST);
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade slot=%d, file=%s, index=%d\r\n", iSlot, pDName, index);
        
    iRetVa = filem_fpga_upgrade(iSlot, index, pDName);

    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade slot=%d fail, %s\n", iSlot, filem_sys_errget(iRetVa));
    }
    
    return(iRetVa);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradeknls                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据系统文件升级kernel                            */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradeknls(int iSlot, char *psName, int extra)
{
    int   iCount = 0;
    int   iCLoop = 0;
    int   iRetVa = -FILEM_ERROR_NOEXIST;
    
    char *pNameL = NULL;
    char *pNameP = NULL;

    char  bTName[FILEM_NAME_MAXLN] = {0};  /*目录下一个文件*/
    char  bFName[FILEM_NAME_MAXLN] = {0};  /*文件的一部分*/
    char  bDName[FILEM_NAME_MAXLN] = {0};  /*最后文件名*/
#if 0        
    char  bBName[FILEM_NAME_MAXLN] = {0};   
    if(filem_slot_brdname(iSlot, bBName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif    
    pNameL = XMALLOC(MTYPE_FILEM_ENTRY, 200 * FILEM_NAME_MAXLN);

    if(pNameL == NULL) return(-FILEM_ERROR_NOMEM);

    if(extra)
    {       
        if(!filem_oper_isexist(psName))
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade file no exist %s\n", psName);

            XFREE(MTYPE_FILEM_ENTRY, pNameL);

            return(-FILEM_ERROR_NOEXIST);
        }
        else
        {
            iRetVa  = filem_oper_extract(psName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);    

            if(iRetVa != 0)
            {
                zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade extract file %s fail, %s\n", psName,  filem_sys_errget(iRetVa));

                XFREE(MTYPE_FILEM_ENTRY, pNameL);

                filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

                return(iRetVa);
            }
        }
    }
    
    iCount = filem_oper_namelist(filem_type_pathget(FILEM_MTYPE_TMP), NULL, 200 * FILEM_NAME_MAXLN, pNameL);
  
    sprintf(bFName, "%04x_kernel", filem_slot_brdid(iSlot));
    
    pNameP = pNameL;

    memset(bDName, 0, FILEM_NAME_MAXLN);
    memset(bTName, 0, FILEM_NAME_MAXLN);

    while(iCLoop++ < iCount)
    {
        strncpy(bTName, pNameP, FILEM_NAME_MAXLN-1);

        pNameP += strlen(pNameP) + 1;

        if(strstr(bTName, bFName) != NULL)
        {
            sprintf(bDName, "%s%s", filem_type_pathget(FILEM_MTYPE_TMP), bTName);
            
            break;
        }
    }

    if(strlen(bDName) == 0)  
    {
        iRetVa = -FILEM_ERROR_NOEXIST;
        
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot=%d filename no found icount=%d\r\n", iSlot, iCount);
    }
    else
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot=%d filename=%s\r\n", iSlot, bTName);

        iRetVa = filem_img_kernel_upgrade(bDName);
        
        if(iRetVa == 0)
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade is complete!\r\n");
        }
        else
        {
            iRetVa = -FILEM_ERROR_UPDATEKNL;
            
            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade fail, %s\n", filem_sys_errget(iRetVa));
        }
    }

    
    XFREE(MTYPE_FILEM_ENTRY, pNameL);

    if(extra) filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetVa);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradeknlf                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据具体文件升级kernel                            */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradeknlf(int iSlot, char *pDName, int index)
{
    int iRetVa = 0;
    
    if(!filem_oper_isexist(pDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade file no exist %s\n", pDName);

        return(-FILEM_ERROR_NOEXIST);
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot=%d, file=%s\r\n", iSlot, pDName);
        
    iRetVa = filem_img_kernel_upgrade(pDName);

    filem_oper_flush();

    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot=%d fail, %s\n", iSlot, filem_sys_errget(iRetVa));
    }

    return(iRetVa);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradeapps                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据系统文件升级app                               */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradeapps(int iSlot, char *psName, int extra)
{
    int   iCount = 0;
    int   iCLoop = 0;
    int   iRetVa = -FILEM_ERROR_NOEXIST;
    
    char *pNameL = NULL;
    char *pNameP = NULL;

    char  bTName[FILEM_NAME_MAXLN] = {0};  /*目录下一个文件*/
    char  bFName[FILEM_NAME_MAXLN] = {0};  /*文件的一部分*/
    char  bDName[FILEM_NAME_MAXLN] = {0};  /*最后文件名*/
#if 0        
    char  bBName[FILEM_NAME_MAXLN] = {0};   
    if(filem_slot_brdname(iSlot, bBName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif    
    pNameL = XMALLOC(MTYPE_FILEM_ENTRY, 200 * FILEM_NAME_MAXLN);

    if(pNameL == NULL) return(-FILEM_ERROR_NOMEM);

    if(extra)
    {       
        if(!filem_oper_isexist(psName))
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade file no exist %s\n", psName);

            XFREE(MTYPE_FILEM_ENTRY, pNameL);

            return(-FILEM_ERROR_NOEXIST);
        }
        else
        {
            iRetVa  = filem_oper_extract(psName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);    

            if(iRetVa != 0)
            {
                zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade extract file %s fail, %s\n", psName,  filem_sys_errget(iRetVa));

                XFREE(MTYPE_FILEM_ENTRY, pNameL);

                filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

                return(iRetVa);
            }
        }
    }
    
    iCount = filem_oper_namelist(filem_type_pathget(FILEM_MTYPE_TMP), NULL, 200 * FILEM_NAME_MAXLN, pNameL);

    sprintf(bFName, "%04x_app", filem_slot_brdid(iSlot));
    
    pNameP = pNameL;

    memset(bDName, 0, FILEM_NAME_MAXLN);
    memset(bTName, 0, FILEM_NAME_MAXLN);

    while(iCLoop++ < iCount)
    {
        strncpy(bTName, pNameP, FILEM_NAME_MAXLN - 1);

        pNameP += strlen(pNameP) + 1;
        
        if(strstr(bTName, bFName) != NULL)
        {
            sprintf(bDName, "%s%s", filem_type_pathget(FILEM_MTYPE_TMP), bTName);
            
            break;
        }
    }

    if(strlen(bDName) == 0)  
    {
        iRetVa = -FILEM_ERROR_NOEXIST;
        
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade slot=%d filename no found icount=%d\r\n", iSlot, iCount);
    }
    else
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade slot=%d filename=%s\r\n", iSlot, bDName);

        iRetVa = filem_oper_extract(bDName, filem_type_pathget(FILEM_MTYPE_BIN), NULL, NULL, 0);

        if(iRetVa == 0)
        {
            sprintf(bFName, "%s%s", filem_type_pathget(FILEM_MTYPE_BIN), FILEM_FILE_INIT_CONFIG);

            sprintf(bTName, "%s%s", filem_type_pathget(FILEM_MTYPE_CFG), FILEM_FILE_INIT_CONFIG);

            filem_oper_copy(bFName, bTName, NULL, NULL);

            zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade is complete!\r\n");
        }
        else
        {
            iRetVa = -FILEM_ERROR_UPDATEKNL;
            
            zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade fail, name %s, %s\n", bDName, filem_sys_errget(iRetVa));
        }
    }

    
    XFREE(MTYPE_FILEM_ENTRY, pNameL);

    if(extra) filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetVa);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradeappf                          */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 根据具体文件升级app                               */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradeappf(int iSlot, char *pDName, int index)
{
    int   iRetVa = 0;
    char  bBName[FILEM_NAME_MAXLN] = {0};   
    char  bFName[FILEM_NAME_MAXLN] = {0};   
   
    if(!filem_oper_isexist(pDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade file no exist %s\n", pDName);

        return(-FILEM_ERROR_NOEXIST);
    }

    zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade slot=%d, file=%s\r\n", iSlot, pDName);
        
    iRetVa = filem_oper_extract(pDName, filem_type_pathget(FILEM_MTYPE_BIN), NULL, NULL, 0);
 
    if(iRetVa == 0)
    {
        sprintf(bBName, "%s%s", filem_type_pathget(FILEM_MTYPE_BIN), FILEM_FILE_INIT_CONFIG);
 
        sprintf(bFName, "%s%s", filem_type_pathget(FILEM_MTYPE_CFG), FILEM_FILE_INIT_CONFIG);
 
        filem_oper_copy(bBName, bFName, NULL, NULL);
 
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade is complete!\r\n");
    }
    else
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade fail, file %s, %s\n", pDName, filem_sys_errget(iRetVa));
    }

    filem_oper_flush();

    return(iRetVa);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_updsysrun                               */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 执行升级系统 本地CPU负责执行                      */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradesys(int iSlot, char *pSysName, int iFlag, FilemCall pEndCall, void *pPara)
{
    int   iRetV  = 0;

    char  bInfo[256] ={0};
    
    if(!filem_oper_isexist(pSysName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem sys file %s no exist \r\n", pSysName);
        
        return(-FILEM_ERROR_NOEXIST);
    }
        
    sprintf(bInfo, "%s", "Upgrading, Wait a few moments, Please!\r\n");
    
    if(pEndCall) pEndCall(iRetV, bInfo, pPara, 0);

    zlog_debug(FILEM_DEBUG_UPGRADE, "filem upgrade sys file name %s\r\n", pSysName);

    iRetV = filem_oper_extract(pSysName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);

    if(iRetV != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade extract file %s fail, %s\n", pSysName,  filem_sys_errget(iRetV));

        filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

        return(iRetV);
    }
   
    iRetV = filem_cmd_runupgradeapps(iSlot, pSysName, 0);
    
                                                     /*升级KERNEL*/
    if((iRetV == 0) && (iFlag & FILEM_OPTFLG_UPDKNL))
    {
        iRetV =  filem_cmd_runupgradeknls(iSlot, pSysName, 0);

        if(iRetV != 0)
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade fail, %s\n", filem_sys_errget(iRetV));

            iRetV = -FILEM_ERROR_UPDATEKNL;
        }
        else
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot %d  is complete!\r\n", iSlot);
        }
    }

    if((iRetV == 0) && (iFlag & FILEM_OPTFLG_UPDFPGA)) 
    {
        iRetV = filem_cmd_runupgradefpgas(iSlot, pSysName, 0);

        if(iRetV != 0)
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade fail, %s\n", filem_sys_errget(iRetV));

            iRetV = -FILEM_ERROR_UPDATEFPGA;
        }
        else
        {
            zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade slot %d  is complete!\r\n", iSlot);
        }
    }
    
    filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_runupgradeboot                           */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 执行升级boot                                      */
/*---------------------------------------------------------*/
static int filem_cmd_runupgradeboot(int islot, char *pFile)
{
    int  iRetV = 0;
    char bFName[FILEM_NAME_MAXLN] = {0};
    char bFpart[FILEM_NAME_MAXLN] = {0};
#if 0
    char bBName[FILEM_NAME_MAXLN] = {0};
    if(filem_slot_brdname(islot, bBName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif    
    sprintf(bFName, "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);
    
    filem_oper_extract(bFName, filem_type_pathget(FILEM_MTYPE_TMP), NULL, NULL, 1);

   //search file 

  //sprintf(bFpart, "%s_%s_uboot", gbfiledevname, bBName);
  
    sprintf(bFpart, "%04x_uboot", filem_slot_brdid(islot));
    sprintf(bFName, "%s%s", filem_type_pathget(FILEM_MTYPE_TMP), bFpart);

    if(!filem_oper_isexist(bFName))
    {
        filem_oper_cutname(bFName, bFpart);
        
        zlog_debug(FILEM_DEBUG_UPGRADE, "Boot upgrade fail, file %s no exist\n", bFpart);

      //zlog_err("Boot upgrade fail, file %s no exist\n", bFName);
        
        return(-FILEM_ERROR_NOEXIST);
    }
    else
    {
        iRetV = filem_img_boot_upgrade(bFName);

        zlog_debug(FILEM_DEBUG_UPGRADE, "Boot upgrade, %s, result = %d\n", bFName, iRetV);
    } 
    
    filem_oper_clrdir(filem_type_pathget(FILEM_MTYPE_TMP));

    return(iRetV);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_cmdprocess                              */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 创建线程执行命令                                  */
/*---------------------------------------------------------*/
static void *filem_cmd_cmdprocess(void *pPara)
{
    int           iRetVal = 0;
    char          bReInf[256] = {0};
    char          bSrcName[FILEM_NAME_MAXLN] = {0};
    char          bDesName[FILEM_NAME_MAXLN] = {0};
 
    filemoperate *pCmdPar = pPara;
    
    if(pCmdPar == NULL)
    {
       zlog_debug(FILEM_DEBUG_OTHER, "Create thread parameter error!\r\n");

       return(NULL);
    }
   
    filem_oper_cutname(pCmdPar->bSrcName, bSrcName);
    
    filem_oper_cutname(pCmdPar->bDesName, bDesName);

    usleep(300000);     /*等待调用者先退出*/
  
    switch(pCmdPar->iCmdType)
    {
        case FILEM_OPCMD_DOWNLOAD:

             if((pCmdPar->iFlag & FILEM_OPTFLG_FTP) == 0)
             {
                 iRetVal = filem_tftp_dnloadfile(pCmdPar->bIpAddr, 
                                                 pCmdPar->bDesName,
                                                 pCmdPar->bSrcName,
                                                 pCmdPar->iResPort,
                                                 pCmdPar->pEndCall,
                                                 pCmdPar->pCallPar);
             }
             else
             {
                 iRetVal = filem_ftp_downloadfile(pCmdPar->bIpAddr, 
                                                  pCmdPar->bUsrName,
                                                  pCmdPar->bPassWrd,
                                                  pCmdPar->bDesName,
                                                  pCmdPar->bSrcName,
                                                  pCmdPar->iResPort,
                                                  pCmdPar->pEndCall,
                                                  pCmdPar->pCallPar);
             }

             if(iRetVal == 0)
                  sprintf(bReInf, "Download file %s Successfully! \r\n", bSrcName);
             else sprintf(bReInf, "Download file %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));

             break;
             
        case FILEM_OPCMD_UPLOAD:
             if((pCmdPar->iFlag & FILEM_OPTFLG_FTP) == 0)
             {
                 iRetVal = filem_tftp_uploadfile(pCmdPar->bIpAddr, 
                                                 pCmdPar->bSrcName,
                                                 pCmdPar->bDesName,
                                                 pCmdPar->iResPort,
                                                 pCmdPar->pEndCall,
                                                 pCmdPar->pCallPar);
             }
             else
             {
                 iRetVal = filem_ftp_uploadfile(pCmdPar->bIpAddr, 
                                                 pCmdPar->bUsrName,
                                                 pCmdPar->bPassWrd,
                                                 pCmdPar->bSrcName,
                                                 pCmdPar->bDesName,
                                                 pCmdPar->iResPort,
                                                 pCmdPar->pEndCall,
                                                 pCmdPar->pCallPar);
             }
             
             if(iRetVal == 0)
                  sprintf(bReInf, "Upload file %s Successfully! \r\n", bSrcName);
             else sprintf(bReInf, "Upload file %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));

             break;

        case FILEM_OPCMD_COPY:

             iRetVal = 0;
             
             if(pCmdPar->iDtype == FILEM_MTYPE_VER) 
             {
                 if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
                      iRetVal = -FILEM_ERROR_CONFLICT;
                 else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_COPY);
             }

             if(iRetVal == 0)
             {
                 iRetVal = filem_oper_copy(pCmdPar->bSrcName, pCmdPar->bDesName, pCmdPar->pEndCall, pCmdPar->pCallPar);

                 if(pCmdPar->iDtype == FILEM_MTYPE_VER) 
                 {
                     filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_COPY);
                 }

                 filem_oper_flush();

                 if(iRetVal != 0) iRetVal = -FILEM_ERROR_COPY;
             }

             if(iRetVal == 0)
                  sprintf(bReInf, "Copy file %s to %s Successfully! \r\n", bSrcName, bDesName);
             else 
             {
                 filem_oper_remove(pCmdPar->bDesName);
                 
                 sprintf(bReInf, "Copy file %s to %s fail, %s! \r\n", bSrcName, bDesName, filem_sys_errget(iRetVal));
             }

             break;

        case FILEM_OPCMD_DELETE:

             if(filem_oper_remove(pCmdPar->bSrcName) != 0)
             {
                 iRetVal = -FILEM_ERROR_DELETE;
             }

             filem_oper_flush();

             if(iRetVal == 0)
                  sprintf(bReInf, "Delete file %s Successfully! \r\n", bSrcName);
             else sprintf(bReInf, "Delete file %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));

             break;
             
        case FILEM_OPCMD_RENAME:
             if(filem_oper_rename(pCmdPar->bSrcName, pCmdPar->bDesName) != 0)
             {
                 iRetVal = -FILEM_ERROR_RENAME;
             }
             
             if(iRetVal == 0)
                  sprintf(bReInf, "Rename file %s to %s Successfully! \r\n", bSrcName, bDesName);
             else sprintf(bReInf, "Rename file %s to %s fail, %s! \r\n", bSrcName, bDesName, filem_sys_errget(iRetVal));

             break;

        case FILEM_OPCMD_MD5CHK: 

             iRetVal = filem_md5_matchcheck(pCmdPar->bSrcName, 0);

             if(iRetVal == 0)
                  sprintf(bReInf, "Md5 check file %s Successfully! \r\n", bSrcName);
             else sprintf(bReInf, "Md5 check file %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));

             break;

        case FILEM_OPCMD_UPDATESYS:

             filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

             iRetVal = filem_cmd_runupgradesys(pCmdPar->iSlotno, pCmdPar->bSrcName, pCmdPar->iFlag, pCmdPar->pEndCall, pCmdPar->pCallPar);

             if(iRetVal == 0)
             {
                 filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);
                
                 sprintf(bReInf, "Sys upgrade name %s Successfully! \r\n", bSrcName);

                 filem_upgrade_time_set(time(NULL));
             }
             else 
             {
                 filem_upgrade_stat_set(iRetVal);
                
                 sprintf(bReInf, "Sys upgrade name %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));
             }
             
             break;
             
        case FILEM_OPCMD_UPDATEPATCH:

             filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

             iRetVal = filem_cmd_runupgredepatch(pCmdPar->iSlotno, pCmdPar->bSrcName);

             if(iRetVal == 0)
             {
                  filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);

                  filem_upgrade_time_set(time(NULL));

                  sprintf(bReInf, "Patch upgrade name %s Successfully! \r\n", bSrcName);
             }
             else 
             {
                  filem_upgrade_stat_set(iRetVal);

                  sprintf(bReInf, "Patch upgrade name %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));
             }

             break;

        case FILEM_OPCMD_UPDATEFPGA:

             filem_upgfpga_stat_set(-FILEM_ERROR_RUNNING);

             if((pCmdPar->iFlag & FILEM_OPTFLG_BYSYS) != 0)
             {
                 iRetVal = filem_cmd_runupgradefpgas(pCmdPar->iSlotno, pCmdPar->bSrcName, 1);
             }
             else
             {
                 iRetVal = filem_cmd_runupgradefpgaf(pCmdPar->iSlotno, pCmdPar->bSrcName, pCmdPar->iProType);
             }

             if(iRetVal == 0)
             {
                  filem_upgfpga_stat_set(-FILEM_ERROR_FINISHED);

                  filem_upgfpga_time_set(time(NULL));

                  sprintf(bReInf, "Fpga upgrade name %s Successfully! \r\n", bSrcName);
             }
             else
             {
                  filem_upgfpga_stat_set(iRetVal);

                  sprintf(bReInf, "Fpga upgrade name %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));
             }
             break;

        case FILEM_OPCMD_UPDATEKNL:

             filem_upgkernel_stat_set(-FILEM_ERROR_RUNNING);

             if((pCmdPar->iFlag & FILEM_OPTFLG_BYSYS) != 0)
             {
                 iRetVal = filem_cmd_runupgradeknls(pCmdPar->iSlotno, pCmdPar->bSrcName, 1);
             }
             else
             {
                 iRetVal = filem_cmd_runupgradeknlf(pCmdPar->iSlotno, pCmdPar->bSrcName, 0);
             }

             if(iRetVal == 0)
             {
                  filem_upgkernel_stat_set(-FILEM_ERROR_FINISHED);

                  filem_upgkernel_time_set(time(NULL));

                  sprintf(bReInf, "Kernel upgrade name %s Successfully! \r\n", bSrcName);
             }
             else
             {
                  filem_upgkernel_stat_set(iRetVal);

                  sprintf(bReInf, "Kernel upgrade name %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));
             }
             break;

        case FILEM_OPCMD_UPDATEAPP:

             filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

 
             if((pCmdPar->iFlag & FILEM_OPTFLG_BYSYS) != 0)
             {
                 iRetVal = filem_cmd_runupgradeapps(pCmdPar->iSlotno, pCmdPar->bSrcName, 1);
             }
             else
             {
                 iRetVal = filem_cmd_runupgradeappf(pCmdPar->iSlotno, pCmdPar->bSrcName, 0);
             }

             if(iRetVal == 0)
             {
                  filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);

                  filem_upgrade_time_set(time(NULL));

                  sprintf(bReInf, "App upgrade name %s Successfully! \r\n", bSrcName);
             }
             else
             {
                  filem_upgrade_stat_set(iRetVal);

                  sprintf(bReInf, "App upgrade name %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));
             }
             break;

        case FILEM_OPCMD_UPDATEUBOOT:

            iRetVal = filem_cmd_runupgradeboot(pCmdPar->iSlotno, pCmdPar->bSrcName);

             if(iRetVal == 0)
                  sprintf(bReInf, "Upgrade uboot ver %s Successfully! \r\n", bSrcName);
             else sprintf(bReInf, "Upgrade uboot ver %s fail, %s! \r\n", bSrcName, filem_sys_errget(iRetVal));

             break;

        case FILEM_OPCMD_ACTIVEKNL:

            iRetVal = filem_img_kernel_activesw();

             if(iRetVal == 0)
                  sprintf(bReInf, "Kernel active switch Successfully! \r\n");
             else sprintf(bReInf, "Kernel active siwtch fail, %s! \r\n", filem_sys_errget(iRetVal));

             break;

        case FILEM_OPCMD_GETSTATE:
             
        case FILEM_OPCMD_DBGENABLE:
             break;

        default:break;
    }

    if(iRetVal != 0)
    {
        zlog_err("%s\n", bReInf);
    }
    else 
    {
        zlog_info("%s\n", bReInf);
    }
    
    if(pCmdPar->pEndCall) pCmdPar->pEndCall(iRetVal, bReInf, pCmdPar->pCallPar, 1);

    XFREE(MTYPE_FILEM_ENTRY, pCmdPar);

    return(NULL);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_threadcreate                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 设置线程属性                                      */
/*---------------------------------------------------------*/
static int filem_cmd_threadcreate(void * (*pfun)(void*), int iStackSz, int iPrio, void *pPara)
{
    int                 iRet;
    pthread_t           iTid;
    pthread_attr_t      sThAttr;
    struct sched_param  sParam;

    sParam.sched_priority = 0;//iPrio;
    
    pthread_attr_init(&sThAttr);

    pthread_attr_setscope(&sThAttr, PTHREAD_SCOPE_SYSTEM);

    pthread_attr_setstacksize(&sThAttr, iStackSz);

    pthread_attr_setdetachstate(&sThAttr, PTHREAD_CREATE_DETACHED);
    
    pthread_attr_setinheritsched(&sThAttr, PTHREAD_EXPLICIT_SCHED);
    
    pthread_attr_setschedpolicy(&sThAttr, SCHED_OTHER);
    
    pthread_attr_setschedparam(&sThAttr, &sParam);

    iRet = pthread_create(&iTid, &sThAttr, pfun, pPara);
    
    if(iRet != 0)
    {
         zlog_debug(FILEM_DEBUG_OTHER, " ret= %d errno = %d\r\n", iRet, errno);
         return(-FILEM_ERROR_CREATETHREAD);
    }
    else return(0);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_remove                                  */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 删除文件                                          */
/*---------------------------------------------------------*/
int filem_cmd_remove(int iType, char *pFile, FilemCall pEndCall, void *pPara, int iASyn)
{
    int  iRetV = 0;
    char bInfo[256] = {0};
    char bName[FILEM_NAME_MAXLN] = {0};
    
    iRetV = filem_cmd_inputnamecheck(iType, pFile, bName);

    zlog_debug(FILEM_DEBUG_OTHER, "filem_cmd_remove file name =%s\r\n", bName);
   
    if(iRetV < 0) 
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Delete file %s fail, %s!\r\n", pFile, filem_sys_errget(iRetV));
        
        return(iRetV);
    }
    
    if(!filem_oper_isexist(bName)) return(-FILEM_ERROR_NOEXIST);
                      
    if(iASyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        if(pCmdPar)
        {
             memset(pCmdPar, 0, sizeof(filemoperate));
         
             pCmdPar->iCmdType = FILEM_OPCMD_DELETE;
         
             strcpy(pCmdPar->bSrcName, bName);
                  
             pCmdPar->pCallPar = pPara;
             
             pCmdPar->pEndCall = pEndCall;
 
             iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_NOMEM;
    }
    else
    {
        if(filem_oper_remove(bName) != 0) iRetV = -FILEM_ERROR_DELETE;
        
        if(iRetV == 0)
        {
            sprintf(bInfo, "Delete file %s Successfully!\r\n", pFile);

            zlog_debug(FILEM_DEBUG_OTHER, "%s", bInfo);
        }
        else 
        {
            sprintf(bInfo, "Delete file %s fail, %s!\r\n", pFile, filem_sys_errget(iRetV));
            
            zlog_debug(FILEM_DEBUG_OTHER, "%s", bInfo);
        }

        filem_oper_flush();

        if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
    }

    return(iRetV);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_cmd_asycopy                                 */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 异步复制文件                                      */
/*---------------------------------------------------------*/
int filem_cmd_copy(int   iStype, char *pSFile, int iDtype,
                   char *pDFile, FilemCall pEndCall, void *pPara, int iASyn)
{
    int   iRetVal = 0;
    char  bRInfo[256] = {0};

    char  bSName[FILEM_NAME_MAXLN] = {0};
    char  bDName[FILEM_NAME_MAXLN] = {0};
    
    iRetVal = filem_cmd_inputnamecheck(iStype, pSFile, bSName);
    
    if(iRetVal != 0) return(iRetVal);

    iRetVal = filem_cmd_inputnamecheck(iDtype, pDFile, bDName);
    
    if(iRetVal != 0) return(iRetVal);

    if(strcmp(bSName, bDName) == 0)
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Copy file %s fail, %s!\r\n", bSName, filem_sys_errget(iRetVal));
        return(-FILEM_ERROR_SAMENAME); 
    }
    
    if(!filem_oper_isexist(bSName))  
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Copy file %s fail, %s!\r\n", bSName, filem_sys_errget(iRetVal));
        
        return(-FILEM_ERROR_NOEXIST);
    }
    
    if(iASyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        if(pCmdPar)
        {
             memset(pCmdPar, 0, sizeof(filemoperate));
          
             pCmdPar->iCmdType = FILEM_OPCMD_COPY;
          
             strcpy(pCmdPar->bSrcName, bSName);
             
             strcpy(pCmdPar->bDesName, bDName);
                   
             pCmdPar->pCallPar = pPara;
              
             pCmdPar->pEndCall = pEndCall;

             pCmdPar->iDtype   = iDtype;
                  
             iRetVal = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
        }
        else iRetVal = -FILEM_ERROR_NOMEM;
    }
    else
    {
         iRetVal = 0;
         
         if(iDtype == FILEM_MTYPE_VER) 
         {
             if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
                  iRetVal = -FILEM_ERROR_CONFLICT;
             else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_COPY);
         }

         if(iRetVal == 0)
         {
             iRetVal = filem_oper_copy(bSName, bDName, pEndCall, pPara);

             if(iDtype == FILEM_MTYPE_VER) 
             {
                 filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_COPY);
             }

             if(iRetVal != 0)  iRetVal = -FILEM_ERROR_COPY;
         }
         
         if(iRetVal == 0)
         {
             sprintf(bRInfo, "Copy file %s to %s Successfully!\r\n", bSName, bDName);
             
             zlog_debug(FILEM_DEBUG_OTHER, "%s", bRInfo);
         }
         else
         {
             filem_oper_remove(bDName);
         
             sprintf(bRInfo, "Copy file %s to %s fail, %s!\r\n", bSName, bDName, filem_sys_errget(iRetVal));

             zlog_debug(FILEM_DEBUG_OTHER, "%s", bRInfo);
         }
         
         filem_oper_flush();

         if(pEndCall) pEndCall(iRetVal, bRInfo, pPara, 1);
    }
        
    return(iRetVal); 
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_rename                                  */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 文件改名                                          */
/*---------------------------------------------------------*/
int filem_cmd_rename(int iType, char *pSFile, char *pDFile,
                     FilemCall pEndCall, void *pPara, int iASyn)
{
    int  iRetV = 0;
    char bRInfo[256] = {0};
    char bSPath[FILEM_NAME_MAXLN] = {0};
    char bDPath[FILEM_NAME_MAXLN] = {0};
    
    char bSName[FILEM_NAME_MAXLN] = {0};
    char bDName[FILEM_NAME_MAXLN] = {0};

    iRetV = filem_cmd_inputnamecheck(iType, pSFile, bSName);

    zlog_debug(FILEM_DEBUG_OTHER, "Rename file src name = %s.\r\n", bSName);

    if(iRetV < 0) 
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Rename file %s fail, %s!\r\n", pSFile, filem_sys_errget(iRetV));
        
        return(iRetV);
    }

    iRetV = filem_cmd_inputnamecheck(iType, pDFile, bDName);

    zlog_debug(FILEM_DEBUG_OTHER, "filem_cmd_rename dst name = %s.\r\n", bDName);

    if(iRetV < 0) 
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Rename file %s fail, %s!\r\n", pSFile, filem_sys_errget(iRetV));
        
        return(iRetV);
    }
    
    if(iType == FILEM_MTYPE_NON)
    {
        filem_oper_cutpath(pSFile, bSPath);
        filem_oper_cutpath(pDFile, bDPath);

        if(strcmp(bDPath, bSPath) != 0)
        {
            zlog_debug(FILEM_DEBUG_OTHER, "Rename file %s fail, %s!\r\n", pSFile, filem_sys_errget(-FILEM_ERROR_PATH));
         
            return(-FILEM_ERROR_PATH);
        }
    }

    if(strcmp(bSName, bDName) == 0) return(-FILEM_ERROR_SAMENAME); 
    
    if(iRetV == 0) 
    {
       if(!filem_oper_isexist(bSName)) iRetV = -FILEM_ERROR_NOEXIST; 
    }
    
    if(iRetV == 0) 
    {
       if( filem_oper_isexist(bDName)) iRetV = -FILEM_ERROR_EXIST; 
    }
    
    if(iRetV < 0)  
    {
        zlog_debug(FILEM_DEBUG_OTHER, "Rename file %s fail, %s!\r\n", pSFile, filem_sys_errget(iRetV));
        
        return(iRetV);
    }

    if(iASyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        if(pCmdPar)
        {
             memset(pCmdPar, 0, sizeof(filemoperate));
          
             pCmdPar->iCmdType =  FILEM_OPCMD_RENAME;
          
             strcpy(pCmdPar->bSrcName, bSName);
             
             strcpy(pCmdPar->bDesName, bDName);
                   
             pCmdPar->pCallPar = pPara;
 
             pCmdPar->pEndCall = pEndCall;
          
             iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_NOMEM;
    }
    else
    {        
        if(filem_oper_rename(bSName, bDName) != 0) iRetV = -FILEM_ERROR_RENAME;
        
        if(iRetV == 0)
        {
            sprintf(bRInfo, "Rename file %s to %s Successfully!\r\n", pSFile, pDFile);

            zlog_debug(FILEM_DEBUG_OTHER, "%s", bRInfo);
        }
        else 
        {
            sprintf(bRInfo, "Rename file %s to %s fail, %s!\r\n", pSFile, pDFile, filem_sys_errget(iRetV));
            
            zlog_debug(FILEM_DEBUG_OTHER, "%s", bRInfo);
        }

        filem_oper_flush();
        
        if(pEndCall) pEndCall(iRetV, bRInfo, pPara, 1);
    }              
    
    return(iRetV);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_dnload                                  */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 下载                                              */
/*---------------------------------------------------------*/
int filem_cmd_tftpdnload(int iType, char *pIp, char *pRFile, char *pLFile, 
                         FilemCall pEndCall, void *pPara, int iAsyn)
{
    int  iRetVa = 0;

    char bLPath[FILEM_NAME_MAXLN] = {0};
    char bLName[FILEM_NAME_MAXLN] = {0};

/*
    if(filem_oper_isexist(pLFile) && filem_oper_isdir(pLFile))
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Download file fail, file type is direction\n");

        return -FILEM_ERROR_NAME;
    }
*/
    if((filem_oper_namechk(pRFile) != 0) ||
       (filem_oper_namechk(pLFile) != 0))
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Download file %s fail, filename error\r\n", pRFile);

        return(-FILEM_ERROR_NAME);
    }

    filem_oper_cutpath(pLFile, bLPath);
  
    filem_oper_cutname(pLFile, bLName);
    
    if(iType == FILEM_MTYPE_NON)
    {
        if(filem_oper_pathchk(bLPath) == 0) 
        {
             if(strlen(bLName) > 0)
                  strcpy(bLPath, pLFile);
             else iRetVa = -FILEM_ERROR_NAME;
        }
        else iRetVa = -FILEM_ERROR_PATH;
    }
    else //only support FILEM_MTYPE_VER
    {
        if(strlen(bLPath) == 0)
             sprintf(bLPath, "%s%s", filem_type_pathget(iType), bLName);
        else iRetVa = -FILEM_ERROR_PATH;
    }

    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Download file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));

        return(iRetVa);
    }

    if(iAsyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        if(pCmdPar)
        {
             memset(pCmdPar, 0, sizeof(filemoperate));
               
             pCmdPar->iCmdType =  FILEM_OPCMD_DOWNLOAD;
          
             strcpy(pCmdPar->bSrcName, pRFile);
             
             strcpy(pCmdPar->bDesName, bLPath);
             
             strcpy(pCmdPar->bIpAddr,  pIp);
                   
             pCmdPar->pCallPar = pPara;
              
             pCmdPar->pEndCall = pEndCall;
  
             pCmdPar->iResPort = UDP_PORT_TFTP;
 
             iRetVa = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
        }
        else iRetVa = -FILEM_ERROR_NOMEM;
    }
    else
    {
        iRetVa = filem_tftp_dnloadfile(pIp, bLPath, pRFile, UDP_PORT_TFTP, pEndCall, pPara);
       
        if(iRetVa < 0)
        {
          //filem_download_stat_set(iRetVa);
            
            zlog_debug(FILEM_DEBUG_TFTP, "Download file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));
        }
    }

    return(iRetVa);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_tftpupload                              */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 异步步下载                                        */
/*---------------------------------------------------------*/
int filem_cmd_tftpupload(int  iType,  char *pIp, char *pRFile, char *pLFile,
                         FilemCall pEndCall, void *pPara, int iAsyn)
{
    int  iRetVa = 0;
    char bLPath[FILEM_NAME_MAXLN] = {0};
    char bLName[FILEM_NAME_MAXLN] = {0};

    if(filem_oper_isdir(pLFile))
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Upload file fail, file type is direction\r\n");

        return(-FILEM_ERROR_NAME);
    }


    if((filem_oper_namechk(pRFile) != 0) ||
       (filem_oper_namechk(pLFile) != 0))
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Upload file %s fail, filename error\r\n", pRFile);

        return(-FILEM_ERROR_NAME);
    }
    
    filem_oper_cutpath(pLFile, bLPath);
  
    filem_oper_cutname(pLFile, bLName);

    if(iType == FILEM_MTYPE_NON)
    {
        if(filem_oper_pathchk(bLPath) == 0) 
        {
             if(strlen(bLName) > 0)
                  strcpy(bLPath, pLFile);
             else iRetVa = -FILEM_ERROR_NAME;
        }
        else iRetVa = -FILEM_ERROR_PATH;
    }
    else  
    {
        if((iType > FILEM_MTYPE_NON) && (iType < FILEM_MTYPE_TMP))
        {
             if(strlen(bLPath) == 0)
                  sprintf(bLPath, "%s%s", filem_type_pathget(iType), bLName);
             else iRetVa = -FILEM_ERROR_PATH;
        }
        else iRetVa = -FILEM_ERROR_UNKTYPE;
    }

    if(iRetVa == 0)
    {
        if(!filem_oper_isexist(bLPath))  iRetVa = -FILEM_ERROR_NOEXIST;
    }
    
    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_TFTP, "Upload file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));

        return(iRetVa);
    }
    
    if(iAsyn)
    {
    
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        memset(pCmdPar, 0, sizeof(filemoperate));
     
        pCmdPar->iCmdType =  FILEM_OPCMD_UPLOAD;
     
        strcpy(pCmdPar->bSrcName, bLPath);
        
        strcpy(pCmdPar->bDesName, pRFile);
        
        strcpy(pCmdPar->bIpAddr,  pIp);
              
        pCmdPar->pCallPar = pPara;
        
        pCmdPar->pEndCall = pEndCall;

        pCmdPar->iResPort = UDP_PORT_TFTP;

        iRetVa = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
    }
    else
    {
        iRetVa = filem_tftp_uploadfile(pIp, bLPath, pRFile, UDP_PORT_TFTP, pEndCall, pPara);
        
        if(iRetVa < 0)
        {
          //filem_upload_stat_set(iRetVa);
            
            zlog_debug(FILEM_DEBUG_TFTP, "Download file %s fail, %s\r\n", bLPath, filem_sys_errget(iRetVa));
        }
    }
    return(iRetVa);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_ftpdnload                              */
/* Retu: 3é1|·μ??0 ê§°ü·μ??-x                              */
/* Desc: ????                                              */
/*---------------------------------------------------------*/
int filem_cmd_ftpdnload(int  iType, char *pIp, char *pUser, char *pPwd, 
                        char *pRFile, char *pLFile, FilemCall pEndCall, void *pPara, int iAsyn)
{
    int  iRetVa = 0;

    char bLPath[FILEM_NAME_MAXLN] = {0};
    char bLName[FILEM_NAME_MAXLN] = {0};

    if((filem_oper_namechk(pRFile) != 0) ||
       (filem_oper_namechk(pLFile) != 0))
    {
        zlog_debug(FILEM_DEBUG_FTP, "Download file %s fail, filename error\r\n", pRFile);

        return(-FILEM_ERROR_NAME);
    }

    filem_oper_cutpath(pLFile, bLPath);
  
    filem_oper_cutname(pLFile, bLName);
    
    if(iType == FILEM_MTYPE_NON)
    {
        if(filem_oper_pathchk(bLPath) == 0) 
        {
             if(strlen(bLName) > 0)
                  strcpy(bLPath, pLFile);
             else iRetVa = -FILEM_ERROR_NAME;
        }
        else iRetVa = -FILEM_ERROR_PATH;
    }
    else //only support FILEM_MTYPE_VER
    {
        if(strlen(bLPath) == 0)
             sprintf(bLPath, "%s%s", filem_type_pathget(iType), bLName);
        else iRetVa = -FILEM_ERROR_PATH;
    }

    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_FTP, "Download file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));

        return(iRetVa);
    }

    if(iAsyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

        if(pCmdPar)
        {
             memset(pCmdPar, 0, sizeof(filemoperate));
               
             pCmdPar->iCmdType =  FILEM_OPCMD_DOWNLOAD;
          
             strcpy(pCmdPar->bSrcName, pRFile);
             
             strcpy(pCmdPar->bDesName, bLPath);
             
             strcpy(pCmdPar->bIpAddr,  pIp);

             strcpy(pCmdPar->bUsrName, pUser);

             strcpy(pCmdPar->bPassWrd, pPwd);
                   
             pCmdPar->pCallPar = pPara;
              
             pCmdPar->pEndCall = pEndCall;
 
             pCmdPar->iFlag   |= FILEM_OPTFLG_FTP;
 
             pCmdPar->iResPort = UDP_PORT_FTP_CTRL;
 
             iRetVa = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
        }
        else iRetVa = -FILEM_ERROR_NOMEM;
    }
    else
    {
        iRetVa = filem_ftp_downloadfile(pIp, pUser, pPwd, bLPath, pRFile, 0, pEndCall, pPara);

        if(iRetVa < 0)
        {
          //filem_download_stat_set(iRetVa);
            
            zlog_debug(FILEM_DEBUG_FTP, "Download file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));
        }
    }

    return(iRetVa);
}


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_ftpupload                              */
/* Retu: 3é1|·μ??0 ê§°ü·μ??-x                              */
/* Desc: òì2?2?????                                        */
/*---------------------------------------------------------*/
int filem_cmd_ftpupload(int  iType,  char *pIp, char *pUser, char *pPwd,
                        char *pRFile, char *pLFile, FilemCall pEndCall, void *pPara, int iAsyn)
{
    int  iRetVa = 0;
    char bLPath[FILEM_NAME_MAXLN] = {0};
    char bLName[FILEM_NAME_MAXLN] = {0};

    if(filem_oper_isdir(pLFile))
    {
        zlog_debug(FILEM_DEBUG_FTP, "Upload file fail, file type is direction\r\n");

        return(-FILEM_ERROR_NAME);
    }


    if((filem_oper_namechk(pRFile) != 0) ||
       (filem_oper_namechk(pLFile) != 0))
    {
        zlog_debug(FILEM_DEBUG_FTP, "Upload file %s fail, filename error\r\n", pRFile);

        return(-FILEM_ERROR_NAME);
    }
    
    filem_oper_cutpath(pLFile, bLPath);
  
    filem_oper_cutname(pLFile, bLName);

    if(iType == FILEM_MTYPE_NON)
    {
        if(filem_oper_pathchk(bLPath) == 0) 
        {
             if(strlen(bLName) > 0)
                  strcpy(bLPath, pLFile);
             else iRetVa = -FILEM_ERROR_NAME;
        }
        else iRetVa = -FILEM_ERROR_PATH;
    }
    else  
    {
        if((iType > FILEM_MTYPE_NON) && (iType < FILEM_MTYPE_TMP))
        {
             if(strlen(bLPath) == 0)
                  sprintf(bLPath, "%s%s", filem_type_pathget(iType), bLName);
             else iRetVa = -FILEM_ERROR_PATH;
        }
        else iRetVa = -FILEM_ERROR_UNKTYPE;
    }

    if(iRetVa == 0)
    {
        if(!filem_oper_isexist(bLPath))  iRetVa = -FILEM_ERROR_NOEXIST;
    }
    
    if(iRetVa != 0)
    {
        zlog_debug(FILEM_DEBUG_FTP, "Upload file %s fail, %s\r\n", pRFile, filem_sys_errget(iRetVa));

        return(iRetVa);
    }
    
    if(iAsyn)
    {
        filemoperate *pCmdPar = NULL;

        pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));
        
        memset(pCmdPar, 0, sizeof(filemoperate));
     
        pCmdPar->iCmdType =  FILEM_OPCMD_UPLOAD;
     
        strcpy(pCmdPar->bSrcName, bLPath);
        
        strcpy(pCmdPar->bDesName, pRFile);
        
        strcpy(pCmdPar->bIpAddr,  pIp);
        
        strcpy(pCmdPar->bUsrName, pUser);
        
        strcpy(pCmdPar->bPassWrd, pPwd);
              
        pCmdPar->pCallPar = pPara;
        
        pCmdPar->pEndCall = pEndCall;

        pCmdPar->iFlag   |= FILEM_OPTFLG_FTP;
        
        pCmdPar->iResPort = UDP_PORT_FTP_CTRL;

        iRetVa = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
    }
    else
    {
        iRetVa = filem_ftp_uploadfile(pIp, pUser, pPwd, bLPath, pRFile, 0, pEndCall, pPara);
        
        if(iRetVa < 0)
        {
          //filem_upload_stat_set(iRetVa);
            
            zlog_debug(FILEM_DEBUG_TFTP, "Download file %s fail, %s\r\n", bLPath, filem_sys_errget(iRetVa));
        }
    }
    return(iRetVa);
}

/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_matchsysname                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 检查输入文件是否为SYS文件, patch                  */
/*---------------------------------------------------------*/
static int filem_cmd_matchsysname(int iSlot, char *pFile, char *pTag, char *pMatch)
{
    char *bSchPos = NULL;
    char  bDevName[FILEM_NAME_MAXLN] = {0};

    strncpy(bDevName, pFile, FILEM_NAME_MAXLN-1);

    bSchPos = strchr(bDevName, '_');

    if(bSchPos == NULL) return(-FILEM_ERROR_FILEMATCH);

    *bSchPos = '\0';
    
    sprintf(pMatch, "%s_%s", bDevName, pTag);

    if(strncmp(pFile, pMatch, strlen(pMatch)) != 0)
    {
        zlog_debug(FILEM_DEBUG_OTHER, "File name match check error, filename=%s devname=%s match=%s!\r\n", pFile, bDevName, pMatch);

        return(-FILEM_ERROR_FILEMATCH);
    }

    //考虑检查打包附加信息部分
    
    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_matchfilename                           */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 检查输入文件是否为APP,KNL文件                     */
/*---------------------------------------------------------*/
static int filem_cmd_matchfilename(int iSlot, char *pFile, char *pTag, char *pMatch)
{
    char *bSchPos = NULL;
    char  bBName[FILEM_NAME_MAXLN] = {0};
#if 0
    if((filem_slot_brdname(iSlot, bBName) != 0) ||
       (strlen(bBName) == 0))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }

    sprintf(pMatch, "%s_%04x_%s", bBName, filem_slot_brdid(iSlot), pTag);

#endif
    strncpy(bBName, pFile, FILEM_NAME_MAXLN-1);

    bSchPos = strchr(bBName, '_');

    if(bSchPos == NULL) return(-FILEM_ERROR_HWMATCHERR);

    *bSchPos = '\0';

    sprintf(pMatch, "%s_%04x_%s", bBName, filem_slot_brdid(iSlot), pTag);

    if(strncmp(pFile, pMatch, strlen(pMatch)) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "File name match check error, board=%s, pFile=%s, bField=%s\r\n", bBName, pFile, pMatch);
        return(-FILEM_ERROR_HWMATCHERR);
    }
    
    return(0);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_matchnorname                            */
/* Retu: 成功返回>=0 失败返回-x                                    */
/* Desc: 检查输入文件是否为APP,KNL文件                                */
/*---------------------------------------------------------*/
static int filem_cmd_matchfpganame(int iSlot, char *pFile, char *pMatch, int *iFndId)
{
    int   iItems  = 0;
    char *bSchPos = NULL;
    char *bField[10] = {NULL};
    char  bBName[FILEM_NAME_MAXLN] = {0};
    char  bFName[FILEM_NAME_MAXLN] = {0};
    char  bHwInf[FILEM_NAME_MAXLN] = {0};

    memset(bField, 0, sizeof(char*) * 10);
#if 0
    if((filem_slot_brdname(iSlot, bBName) != 0) ||
       (strlen(bBName) == 0))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "filem get slot name error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }
#endif
    strncpy(bBName, pFile, FILEM_NAME_MAXLN-1);

    bSchPos = strchr(bBName, '_');

    if(bSchPos == NULL) return(-FILEM_ERROR_HWMATCHERR);

    *bSchPos = '\0';

    if(filem_slot_fpgahwinf(iSlot, bHwInf) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade get slot fpga hw infor error \n");
        return(-FILEM_ERROR_SLOTNOINS);
    }

    iItems = filem_oper_nameparse(bHwInf, bField, 5, ';');

    if(iItems <= 0) 
    {
        iItems = 1;
        bHwInf[0] = '\0';
        bField[0] = bHwInf;
    }
    else
    {
        if(iItems > 1)   /*除第一个空串外, 过滤掉最后一个空串*/
        {
            if(strlen(bField[iItems - 1]) == 0) iItems --;
        }
    }

    //搜索第几片FPGA 暂不支持

   *iFndId = 1; /*仅支持一片FPGA 序号为 1*/

    if(strlen(bField[0]) == 0)
         sprintf(bFName, "%s_%04x_%s", bBName, filem_slot_brdid(iSlot), pMatch);
    else sprintf(bFName, "%s_%04x_%s-%s", bBName, filem_slot_brdid(iSlot), pMatch, bField[0]);

    zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade hwinfor parse =%s, items=%d\r\n", bFName, iItems);

    if(strncmp(pFile, bFName, strlen(bFName)) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "File name match check error, board=%s, pFile=%s, bField=%s\r\n", bBName, pFile, bFName);
        return(-FILEM_ERROR_HWMATCHERR);
    }
    
    return(0);  
}


/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_upgradesys                              */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的系统升级                                */
/*---------------------------------------------------------*/
int filem_cmd_upgradesys(int  iSlot, char *pFile, FilemCall pEndCall, 
                         void *pPara, int  iFlag, int iASyn)
{
    int   iRetV = 0;
  //int   index = 0;
    char  bInfo[256] = {0};
    char  bName[FILEM_NAME_MAXLN] = {0};
    char  bMatc[FILEM_NAME_MAXLN] = {0};
    filemoperate *pCmdPar = NULL;
    
    if(filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);
    
    if(iSlot == 0) iSlot = gfilelocslot;

    if((iSlot <= 0) || (iSlot > FILEM_SLOT_COUN_MAX))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade %s fail, slot error, slot = %d\r\n", pFile, iSlot);

        return(-FILEM_ERROR_NOSLOT);
    }

    iRetV = filem_cmd_matchsysname(iSlot, pFile, (char*)"sys", bMatc);
    
    if(iRetV != 0) return(iRetV);

    memset(bName, 0, FILEM_NAME_MAXLN);
    
    snprintf(bName, FILEM_NAME_MAXLN-1, "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);
    
    zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade file name %s\r\n", bName);
    
    if(!filem_oper_isexist(bName))  
    {        
        zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade fail, file %s no exist\r\n", pFile);

        return(-FILEM_ERROR_NOEXIST);
    }

    if(filem_md5_integritychk(bName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade fail, file %s no integrity!\r\n", pFile);
        
        return(-FILEM_ERROR_MD5PADERR);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));
    
    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);
    
    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iFlag    = iFlag | FILEM_OPTFLG_BYSYS;
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;
    pCmdPar->iCmdType = FILEM_OPCMD_UPDATESYS;
    
    strcpy(pCmdPar->bSrcName, bName);
    strcpy(pCmdPar->bDesName, bName);
/*
    if(iSlot == FILEM_SLOT_NUM_ALL)
    {
        for(index = 1; index < FILEM_SLOT_COUN_MAX; index++)
        {
            if(index != gfilelocslot) 
            {
                pCmdPar->iSlotno = index;

                if(filem_slot_cpuhwcheck(index))
                {
                    filem_cmd_send_command(index, pCmdPar);
                }
            }
        }  
    }
    else
    {
        if(iSlot != gfilelocslot)
        {
            if(filem_slot_cpuhwcheck(iSlot))
            {
                filem_cmd_send_command(iSlot, pCmdPar);
            }
            else 
            {
                iRetV = -FILEM_ERROR_HWMATCHERR;
            }
        } 
    }
*/
    if(iSlot != gfilelocslot)
    {
        if(filem_slot_cpuhwcheck(iSlot))
        {
             if(filem_slot_issubcpu(iSlot)) 
             {
                 iRetV = filem_md5_wholefile(bName, pCmdPar->bMd5Data, FILEM_PAD_MD5_BUSZ) ;
             
                 zlog_debug(FILEM_DEBUG_UPGRADE, "Md5 file for sub card upgrade sys=%s\r\n", pCmdPar->bMd5Data);
             }
             
             if(iRetV == 0) filem_cmd_send_command(iSlot, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_HWMATCHERR;
    }
    else   //if((iSlot == gfilelocslot) || (iSlot == FILEM_SLOT_NUM_ALL))
    {
         iSlot = gfilelocslot;

         if(filem_upgrade_is_busy())
         {
             iRetV = -FILEM_ERROR_CONFLICT;
             
             printf("Sys upgrade operation conflict \r\n");

             zlog_debug(FILEM_DEBUG_UPGRADE, "Sys upgrade operation conflict \r\n");
         }
         else
         {
             filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

             if(iASyn)
             {  
                 pCmdPar->iSlotno = iSlot;
                 
                 iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);

                 if(iRetV == 0)
                 {
                     pCmdPar = NULL;    /*不释放内存*/
    			 	 iRetV   = -FILEM_ERROR_NOOP;
                 }
             }
             else
             {
             //  filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);
                 
                 iRetV = filem_cmd_runupgradesys(iSlot, bName, iFlag, pEndCall, pPara);
                 
                 if(iRetV == 0)
                 {
                     filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);
                     
                     filem_upgrade_time_set(time(NULL));

                     sprintf(bInfo, "Sys upgrade Slot %d name %s Successfully!\r\n", iSlot, bName);
                 
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
                 else
                 {
                     filem_upgrade_stat_set(iRetV);
                     
                     sprintf(bInfo, "Sys upgrade Slot %d name %s fail, %s!\r\n", iSlot, bName, filem_sys_errget(iRetV));
     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
             
                 if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
             }
         }
    }

    if(pCmdPar != NULL)
    {
        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    
    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_upgardepatch                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的PATCH升级                               */
/*---------------------------------------------------------*/
int filem_cmd_upgardepatch(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn)
{
    int  iRetV  = 0;
    char bInfo[256] = {0};
    char bName[FILEM_NAME_MAXLN] = {0};
    char bMatc[FILEM_NAME_MAXLN] = {0};
    filemoperate *pCmdPar = NULL;

    if(filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);

    if(iSlot == 0) iSlot = gfilelocslot;

    if((iSlot <= 0) || (iSlot > FILEM_SLOT_COUN_MAX))  
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade version %s fail, slot error\r\n", pFile);

        return(-FILEM_ERROR_NOSLOT);
    }

    iRetV = filem_cmd_matchsysname(iSlot, pFile, (char*)"patch", bMatc);
    
    if(iRetV != 0) return(iRetV);

    memset(bName, 0, FILEM_NAME_MAXLN); 
    
    snprintf(bName, FILEM_NAME_MAXLN-1, "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);

    if(!filem_oper_isexist(bName))  
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade fail, file %s no exist\r\n", pFile);

        return(-FILEM_ERROR_NOEXIST);
    }

    if(filem_md5_integritychk(bName) != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade fail, file %s no integrity!\r\n", pFile);
        
        return(-FILEM_ERROR_MD5PADERR);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));
    
    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);
    
    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iFlag    = FILEM_OPTFLG_UPDAPP;
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;

    pCmdPar->iCmdType = FILEM_OPCMD_UPDATEPATCH;

    strcpy(pCmdPar->bSrcName, bName);
    strcpy(pCmdPar->bDesName, bName);
    
    /*
    if(iSlot == FILEM_SLOT_NUM_ALL)
    {
        int index = 1;

        for(index = 1; index < FILEM_SLOT_COUN_MAX; index++)
        {
            if(index != gfilelocslot) 
            {
                pCmdPar->iSlotno = index;
                
                if(filem_slot_cpuhwcheck(index))
                {
                    filem_cmd_send_command(index, pCmdPar);
                }
            }
        }
    }
    else
    {
       if(iSlot != gfilelocslot) 
       {
            if(filem_slot_cpuhwcheck(iSlot))
            {
                filem_cmd_send_command(iSlot, pCmdPar);
            }
            else 
            {
                iRetV = -FILEM_ERROR_HWMATCHERR;
            }
       }
    }
    */

 
    if(iSlot != gfilelocslot) 
    {
        if(filem_slot_cpuhwcheck(iSlot))
        {
             if(filem_slot_issubcpu(iSlot)) 
             {
                 iRetV = filem_md5_wholefile(bName, pCmdPar->bMd5Data, FILEM_PAD_MD5_BUSZ) ;
             
                 zlog_debug(FILEM_DEBUG_UPGRADE, "Md5 file for sub card upgrade patch=%s\r\n", pCmdPar->bMd5Data);
             }
             
             if(iRetV == 0) filem_cmd_send_command(iSlot, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_HWMATCHERR;
    }
    else    //if((iSlot == gfilelocslot) || (iSlot == FILEM_SLOT_NUM_ALL))
    {
        if(filem_upgrade_is_busy())
        {
            iRetV = -FILEM_ERROR_CONFLICT;
            
            printf("Patch upgrade operation conflict \r\n");

            zlog_debug(FILEM_DEBUG_UPGRADE, "Patch upgrade operation conflict \r\n");
        }
        else
        {
            filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);
            if(iASyn)
            {       
                 pCmdPar->iSlotno = iSlot;
                 
                 iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);

                 if(iRetV == 0) 
                 {
                     pCmdPar = NULL;    /*不释放内存*/
    			 	 iRetV   = -FILEM_ERROR_NOOP;
                 }
            }
            else
            {
               //filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

                 iRetV = filem_cmd_runupgredepatch(iSlot, bName);
        
                 if(iRetV == 0)
                 {
                     filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);
                     
                     filem_upgrade_time_set(time(NULL));

                     sprintf(bInfo, "Patch upgrade file %s Successfully!\r\n", bName);
                 
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
                 else
                 {
                     filem_upgrade_stat_set(iRetV);

                     sprintf(bInfo, "Patch upgrade file %s fail, %s!\r\n", bName, filem_sys_errget(iRetV));
        
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
        
                 if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);           
            }
        }
    }
    
    if(pCmdPar != NULL)
    {
        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    
    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_upgradefpga                             */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的FPGA升级 不支持所有板卡同时升级         */
/*---------------------------------------------------------*/
int filem_cmd_upgradefpga(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlags)
{
    int  iRetV  = 0;
    int  iLocal = 0;
    int  iFndId = 0;
    char bInfo[256]={0};
    char bFpga[16] = "fpga";
    char bMatch[FILEM_NAME_MAXLN] = {0};  
    char bDName[FILEM_NAME_MAXLN] = {0};  
    filemoperate *pCmdPar = NULL;
    
    if((iFlags == FILEM_OPTFLG_BYSYS) && filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);

    if(iSlot == 0) iSlot = gfilelocslot;

    if(!filem_slot_isinsert(iSlot))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade %s for slot%d fail, slot error\r\n", pFile, iSlot);

        return(-FILEM_ERROR_SLOTNOINS);
    }    

    if(iFlags == FILEM_OPTFLG_BYSYS)
    {
        iRetV = filem_cmd_matchsysname(iSlot, pFile, (char*)"sys", bMatch);
    }
    else
    {                                              
        iRetV = filem_cmd_matchfpganame(iSlot, pFile, bFpga, &iFndId);
    }

    if(iRetV != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Fpga upgrade by file %s error, file type match fail!\r\n", pFile);

        return(iRetV);
    }

    memset(bDName, 0, FILEM_NAME_MAXLN);
    
    snprintf(bDName, FILEM_NAME_MAXLN -1 , "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);

    if(!filem_oper_isexist(bDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Upgrade fpga %s for slot%d fail, file no exist!\r\n", bDName, iSlot);

        return(-FILEM_ERROR_NOEXIST);
    }

    if((iFlags == FILEM_OPTFLG_BYSYS) && (filem_md5_integritychk(bDName) != 0))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Upgrade fpga %s for slot%d fail, file no integrity!\r\n", bDName, iSlot);
        
        return(-FILEM_ERROR_MD5PADERR);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);

    memset(pCmdPar, 0, sizeof(filemoperate));

    
    pCmdPar->iFlag    = FILEM_OPTFLG_UPDFPGA | iFlags;
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->iProType = iFndId;
    pCmdPar->pEndCall = pEndCall;
    pCmdPar->iCmdType = FILEM_OPCMD_UPDATEFPGA;
    
    strcpy(pCmdPar->bSrcName, bDName);
    strcpy(pCmdPar->bDesName, bDName);

    if(iSlot == gfilelocslot) 
         iLocal = 1;
    else
    {
         if(filem_slot_cpuhwcheck(iSlot))
              iLocal = 0;
         else iLocal = 1;    
    }   
 
    zlog_debug(FILEM_DEBUG_UPGRADE, "Upgrade fpga slot=%d, local=%d\r\n", iSlot, iLocal);
    
    if(!iLocal)
    {
        if(filem_slot_issubcpu(iSlot)) 
        {
            iRetV = filem_md5_wholefile(bDName, pCmdPar->bMd5Data, FILEM_PAD_MD5_BUSZ) ;

            zlog_debug(FILEM_DEBUG_UPGRADE, "Md5 file for sub card upgrade fpga=%s\r\n", pCmdPar->bMd5Data);
        }
    
        if(iRetV == 0) filem_cmd_send_command(iSlot, pCmdPar);

        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    else
    {
        if(filem_upgrade_is_busy())
        {
            iRetV = -FILEM_ERROR_CONFLICT;
            
            printf("Upgrade fpga operation conflict \r\n");

            zlog_debug(FILEM_DEBUG_UPGRADE, "Upgrade fpga operation conflict \r\n");

            XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
        }
        else
        {
            filem_upgfpga_stat_set(-FILEM_ERROR_RUNNING);
         
            if(iASyn)
            {   
                 iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
     
                 if(iRetV == 0) 
                     iRetV = -FILEM_ERROR_NOOP;
                 else 
                 {
                     XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
                 }
            }
            else
            {
                 XFREE(MTYPE_FILEM_ENTRY, pCmdPar);

              // filem_upgfpga_stat_set(-FILEM_ERROR_RUNNING);

                 if(iFlags == FILEM_OPTFLG_BYSYS)
                 {
                     iRetV = filem_cmd_runupgradefpgas(iSlot, bDName, 1);
                 }
                 else 
                 {
                     iRetV = filem_cmd_runupgradefpgaf(iSlot, bDName, iFndId);
                 }
                 
                 if(iRetV == 0)
                 {
                     filem_upgfpga_stat_set(-FILEM_ERROR_FINISHED);

                     filem_upgfpga_time_set(time(NULL));
                     
                     sprintf(bInfo, "Fpga upgrade name %s Successfully!\r\n", bDName);
                 
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
                 else
                 {
                     filem_upgfpga_stat_set(iRetV);
                     
                     sprintf(bInfo, "Fpga upgrade name %s fail, %s!\r\n", bDName, filem_sys_errget(iRetV));
     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
      
                 if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
            }
        }
    }
    
    return(iRetV);
}



/*---------------------------------------------------------*/
/* Name: filem_cmd_upgradekernel                           */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的kernle升级 不支持所有板卡同时升级       */
/*---------------------------------------------------------*/
int filem_cmd_upgradekernel(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlags)
{
    int  iRetV  = 0;
    char bInfo[256]={0};
    
    char bMatch[FILEM_NAME_MAXLN] = {0};
    char bDName[FILEM_NAME_MAXLN] = {0};
    
    filemoperate *pCmdPar = NULL;
    
    if(filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);

    if(iSlot == 0) iSlot = gfilelocslot;

    if(!filem_slot_cpuhwcheck(iSlot))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade %s for slot%d fail, slot error\r\n", pFile, iSlot);

        return(-FILEM_ERROR_HWMATCHERR);
    }

    if(iFlags == FILEM_OPTFLG_BYSYS)
    {
        iRetV = filem_cmd_matchsysname(iSlot, pFile, (char*)"sys", bMatch);
    }
    else
    {                            
        iRetV = filem_cmd_matchfilename(iSlot, pFile, (char*)"kernel", bMatch);
    }
   
    if(iRetV != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade by file %s error, file type match fail!\r\n", pFile);

        return(iRetV);
    }

    memset(bDName, 0, FILEM_NAME_MAXLN);

    snprintf(bDName, FILEM_NAME_MAXLN - 1,  "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);  /*直接是文件名*/

    if(!filem_oper_isexist(bDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade %s for slot%d fail, file no exist!\r\n", bDName, iSlot);

        return(-FILEM_ERROR_NOEXIST);
    }

    if((iFlags == FILEM_OPTFLG_BYSYS) && (filem_md5_integritychk(bDName) != 0))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade %s for slot%d fail, file no integrity!\r\n", bDName, iSlot);
        
        return(-FILEM_ERROR_MD5PADERR);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);

    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iFlag    = FILEM_OPTFLG_UPDKNL | iFlags;;
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;
    pCmdPar->iCmdType = FILEM_OPCMD_UPDATEKNL;
    
    strcpy(pCmdPar->bSrcName, bDName);
    
    strcpy(pCmdPar->bDesName, bDName);

    zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade slot=%d, filename=%s\r\n", iSlot, bDName);
    
    if(iSlot != gfilelocslot)
    {
    
        if(filem_slot_cpuhwcheck(iSlot)) 
        {
             if(filem_slot_issubcpu(iSlot)) 
             {
                 iRetV = filem_md5_wholefile(bDName, pCmdPar->bMd5Data, FILEM_PAD_MD5_BUSZ) ;
     
                 zlog_debug(FILEM_DEBUG_UPGRADE, "Md5 file for sub card upgrade knl=%s\r\n", pCmdPar->bMd5Data);
             }
         
             if(iRetV == 0) filem_cmd_send_command(iSlot, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_HWMATCHERR;
        
        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    else
    {
        if(filem_upgrade_is_busy())
        {
            iRetV = -FILEM_ERROR_CONFLICT;
            
            printf("Kernel upgrade operation conflict \r\n");

            zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel upgrade operation conflict \r\n");

            XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
        }
        else
        {
            filem_upgkernel_stat_set(-FILEM_ERROR_RUNNING);
            if(iASyn)
            {   
                 iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
     
                 if(iRetV == 0) 
                     iRetV = -FILEM_ERROR_NOOP;
                 else 
                 {
                     XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
                 }
            }
            else
            {
                 XFREE(MTYPE_FILEM_ENTRY, pCmdPar);

               //filem_upgkernel_stat_set(-FILEM_ERROR_RUNNING);

                 if(iFlags == FILEM_OPTFLG_BYSYS)
                 {
                     iRetV = filem_cmd_runupgradeknls(iSlot, bDName, 1);
                 }
                 else 
                 {
                     iRetV = filem_cmd_runupgradeknlf(iSlot, bDName, 0);
                 }
                 
                 if(iRetV == 0)
                 {
                     filem_upgkernel_stat_set(-FILEM_ERROR_FINISHED);

                     filem_upgkernel_time_set(time(NULL));
                     
                     sprintf(bInfo, "Kernel upgrade name %s Successfully!\r\n", bDName);
                 
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
                 else
                 {
                     filem_upgkernel_stat_set(iRetV);
                     
                     sprintf(bInfo, "Kernel upgrade name %s fail, %s!\r\n", bDName, filem_sys_errget(iRetV));
     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
      
                 if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
            }
        }
    }
    
    return(iRetV);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_changeappname                           */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc:15x类设备修改APP版本文件名          */
/*---------------------------------------------------------*/
static int filem_cmd_changeappname(char *pMatch, char *pFile)
{
    struct list   sVerList;
    int           iActSLen = 0;
    char         *bDesPath = NULL;
    fileitems    *pFileItm = NULL;
    char          bActStri[FILEM_NAME_MAXLN] = {0}; 
    char          bSrcFile[FILEM_NAME_MAXLN] = {0}; 
    char          bDesFile[FILEM_NAME_MAXLN] = {0}; 

    bDesPath = filem_type_pathget(FILEM_MTYPE_VER);
    
    iActSLen = sprintf(bActStri, "act_%s", pMatch);    

    memset(&sVerList, 0, sizeof(struct list));
    
    filem_show_dirlist(FILEM_MTYPE_VER, &sVerList, bDesPath, 0);

    pFileItm = (fileitems*)sVerList.head;

    while(pFileItm)
    {
        if(strncmp(pFileItm->bName, bActStri, iActSLen) == 0)
        {
            sprintf(bSrcFile, "%s%s", bDesPath, pFileItm->bName);
            
            sprintf(bDesFile, "%s%s", bDesPath, pFileItm->bName + 4);
            
            filem_oper_rename(bSrcFile, bDesFile);
        }

        pFileItm = (fileitems*)pFileItm->sNode.next;
    }

    filem_show_listclr(&sVerList);

    sprintf(bSrcFile, "%s%s", bDesPath, pFile);
    
    sprintf(bDesFile, "%sact_%s", bDesPath, pFile);
    
    filem_oper_rename(bSrcFile, bDesFile);

    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_upgradeappbin                           */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的APP升级 不支持所有板卡同时升级          */
/*---------------------------------------------------------*/
int filem_cmd_upgradeappbin(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlags)
{
    int  iRetV =  0;
    char bInfo[256]={0};
    char bMatch[FILEM_NAME_MAXLN] = {0}; 
    char bDName[FILEM_NAME_MAXLN] = {0};   
    filemoperate *pCmdPar = NULL;
    
    if((iFlags == FILEM_OPTFLG_BYSYS) && filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);

    if(iSlot == 0) iSlot = gfilelocslot;

    if(!filem_slot_cpuhwcheck(iSlot))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade %s for slot%d fail, slot error\r\n", pFile, iSlot);

        return(-FILEM_ERROR_HWMATCHERR);
    }

    if(iFlags == FILEM_OPTFLG_BYSYS)
    {
        iRetV = filem_cmd_matchsysname(iSlot, pFile, (char*)"sys", bMatch);
    }
    else
    {                            
        iRetV = filem_cmd_matchfilename(iSlot, pFile, (char*)"app", bMatch);
    }
   
    if(iRetV != 0)
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade by file %s error, file type match fail!\r\n", pFile);

        return(iRetV);
    }

    memset(bDName, 0, FILEM_NAME_MAXLN);
    
    snprintf(bDName, FILEM_NAME_MAXLN -1, "%s%s", filem_type_pathget(FILEM_MTYPE_VER), pFile);   

    if(!filem_oper_isexist(bDName))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade %s for slot%d fail, file no exist!\r\n", bDName, iSlot);

        return(-FILEM_ERROR_NOEXIST);
    }
    
    if((iFlags == FILEM_OPTFLG_BYSYS) && (filem_md5_integritychk(bDName) != 0))
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade %s for slot%d fail, file no integrity!\r\n", bDName, iSlot);
         
        return(-FILEM_ERROR_MD5PADERR);
    }  
    
    if(filem_device_ht15x())
    {
        iRetV = filem_cmd_changeappname(bMatch, pFile);

        if(iRetV == 0)
        {
             filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);

             filem_upgrade_time_set(time(NULL));
                     
             sprintf(bInfo, "App upgrade name %s Successfully!\r\n", bDName);
                 
             zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
         }
         else
         {
             filem_upgrade_stat_set(iRetV);
             
             sprintf(bInfo, "App upgrade name %s fail, %s!\r\n", bDName, filem_sys_errget(iRetV));
     
             zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
         }
      
         if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);

         return((pEndCall) ?  0: iRetV);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);

    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iFlag    = FILEM_OPTFLG_UPDAPP | iFlags;
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;
    pCmdPar->iCmdType = FILEM_OPCMD_UPDATEAPP;
    
    strcpy(pCmdPar->bSrcName, bDName);
    strcpy(pCmdPar->bDesName, bDName);    

    zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade slot=%d, filename=%s\r\n", iSlot, bDName);
    
    if(iSlot != gfilelocslot)
    {
        if(filem_slot_cpuhwcheck(iSlot)) 
        {
             if(filem_slot_issubcpu(iSlot)) 
             {
                 iRetV = filem_md5_wholefile(bDName, pCmdPar->bMd5Data, FILEM_PAD_MD5_BUSZ) ;
     
                 zlog_debug(FILEM_DEBUG_UPGRADE, "Md5 file for sub card upgrade app=%s\r\n", pCmdPar->bMd5Data);
             }
         
             if(iRetV == 0) filem_cmd_send_command(iSlot, pCmdPar);
        }
        else iRetV = -FILEM_ERROR_HWMATCHERR;
        
        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    else
    {
        if(filem_upgrade_is_busy())
        {
            iRetV = -FILEM_ERROR_CONFLICT;
            
            printf("App upgrade operation conflict \r\n");

            zlog_debug(FILEM_DEBUG_UPGRADE, "App upgrade operation conflict \r\n");

            XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
        }
        else
        {
            filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

            if(iASyn)
            {   
                 iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
     
                 if(iRetV == 0) 
                     iRetV = -FILEM_ERROR_NOOP;
                 else 
                 {
                     XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
                 }
            }
            else
            {
                 XFREE(MTYPE_FILEM_ENTRY, pCmdPar);

              // filem_upgrade_stat_set(-FILEM_ERROR_RUNNING);

                 if(iFlags == FILEM_OPTFLG_BYSYS)
                 {
                     iRetV = filem_cmd_runupgradeapps(iSlot, bDName, 1);
                 }
                 else 
                 {
                     iRetV = filem_cmd_runupgradeappf(iSlot, bDName, 0);
                 }
                 
                 if(iRetV == 0)
                 {
                     filem_upgrade_stat_set(-FILEM_ERROR_FINISHED);
                     
                     filem_upgrade_time_set(time(NULL));
                     
                     sprintf(bInfo, "App upgrade name %s Successfully!\r\n", bDName);
                 
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
                 else
                 {
                     filem_upgrade_stat_set(iRetV);
                     
                     sprintf(bInfo, "App upgrade name %s fail, %s!\r\n", bDName, filem_sys_errget(iRetV));
     
                     zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
                 }
      
                 if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
            }
        }
    }
    
    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_upgradeuboot                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的BOOT升级                                */
/*---------------------------------------------------------*/
int filem_cmd_upgradeuboot(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn)
{
/*该函数有问题,停止维护*/
    int  iRetV = 0;
    char bInfo[256]={0};
    filemslot    *pSlot= NULL;
    filemoperate *pCmdPar = NULL;


    if(iSlot == 0) iSlot = gfilelocslot;

    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX)) 
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Upload boot version %s fail, slot error\r\n", pFile);

        return(-FILEM_ERROR_NOSLOT);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);

    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iDtype   = FILEM_MTYPE_VER;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;
    
    pCmdPar->iCmdType = FILEM_OPCMD_UPDATEUBOOT;

    strcpy(pCmdPar->bSrcName, pFile);
    strcpy(pCmdPar->bDesName, pFile);
    
    pSlot= filem_slot_objectget(iSlot);

    if((iSlot != gfilelocslot) && pSlot->hwstate && pSlot->iscpu)
    {
        filem_cmd_send_command(iSlot, pCmdPar);
        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    else    
    {
        if(iASyn)
        {
             iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
			 
             if(iRetV == 0) 
			 	 iRetV = -FILEM_ERROR_NOOP;
             else 
             {
                 XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
             }
        }
        else
        {
             XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
 
             iRetV = filem_cmd_runupgradeboot(iSlot, pFile);
 
             if(iRetV == 0)
             {
                 sprintf(bInfo, "Boot upgrade ver %s Successfully!\r\n", pFile);
             
                 zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
             }
             else
             {
                 sprintf(bInfo, "Boot upgrade ver %s fail, %s!\r\n", pFile, filem_sys_errget(iRetV));
 
                 zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
             }
     
             if(pEndCall)  pEndCall(iRetV, bInfo, pPara, 1);
        }
    }

    return(iRetV);
}


/*---------------------------------------------------------*/
/* Name: filem_cmd_activekernel                            */
/* Retu: 成功返回0 失败返回-x                              */
/* Desc: 一个板卡的KNL倒换                                 */
/*---------------------------------------------------------*/
int filem_cmd_activekernel(int iSlot, int iIdx, FilemCall pEndCall, void *pPara, int iASyn)
{
    int  iRetV =  0;
    int  iLocal = 0;
    char bInfo[256]={0};

    filemslot    *pSlot= NULL;
    filemoperate *pCmdPar = NULL;
    
    if(filem_device_ht15x()) return(-FILEM_ERROR_NOSUPPORT);

    if(iSlot == 0) iSlot = gfilelocslot;

    if((iSlot <= 0) || (iSlot >= FILEM_SLOT_COUN_MAX))  
    {
        zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel active for slot%d fail, slot error\r\n", iSlot);

        return(-FILEM_ERROR_NOSLOT);
    }

    pCmdPar = XMALLOC(MTYPE_FILEM_ENTRY, sizeof(filemoperate));

    if(pCmdPar == NULL) return(-FILEM_ERROR_NOMEM);

    memset(pCmdPar, 0, sizeof(filemoperate));
    
    pCmdPar->iFlag    = 0;
    pCmdPar->iSlotno  = iSlot;
    pCmdPar->pCallPar = pPara;
    pCmdPar->pEndCall = pEndCall;
    pCmdPar->iCmdType = FILEM_OPCMD_ACTIVEKNL;
    
    pSlot = filem_slot_objectget(iSlot);

    if(iSlot == gfilelocslot) iLocal = 1;
    else    
    {
         iLocal = 0;

         if((!pSlot->hwstate) || (!pSlot->iscpu))
         {                        /*板卡不在位,或没有CPU不能升级*/
             XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
 
             return(-FILEM_ERROR_HWMATCHERR);
         }
    }   

    zlog_debug(FILEM_DEBUG_UPGRADE, "Kernel active slot=%d, local=%d\r\n", iSlot, iLocal);
    
    if(!iLocal)
    {
        filem_cmd_send_command(iSlot, pCmdPar);

        XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
    }
    else
    {
        if(iASyn)
        {   
             iRetV = filem_cmd_threadcreate(filem_cmd_cmdprocess, 131072, 5, pCmdPar);
     
             if(iRetV != 0) 
             {
                 XFREE(MTYPE_FILEM_ENTRY, pCmdPar);
             }
        }
        else
        {
             XFREE(MTYPE_FILEM_ENTRY, pCmdPar);

             iRetV = filem_img_kernel_activesw();

             if(iRetV == 0)
             {
                 sprintf(bInfo, "Kernel active switch Successfully!\r\n");
             
                 zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);

               //if(!filem_slot_ismaster()) filem_sync_statussend();
             }
             else
             {
                 sprintf(bInfo, "Kernel active switch fail! %s\r\n", filem_sys_errget(iRetV));
     
                 zlog_debug(FILEM_DEBUG_UPGRADE, "%s", bInfo);
             }
      
             if(pEndCall) pEndCall(iRetV, bInfo, pPara, 1);
        }
    }
    
    return(iRetV);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
/*---------------------------------------------------------*/
/* Name: filem_cmd_stateupdateask                          */
/* Retu: 成功返回0 失败返回-x                                      */
/* Desc: 请求子卡更新状态                                          */
/*---------------------------------------------------------*/
int filem_cmd_stateupdateask(int iSlot)
{
    filemoperate sCmdPar = {0};
    
    memset(&sCmdPar, 0, sizeof(filemoperate));
    
    sCmdPar.iFlag    = 0;
    sCmdPar.iSlotno  = iSlot;
    sCmdPar.iCmdType = FILEM_OPCMD_GETSTATE;
    
    if(filem_slot_issubcpu(iSlot)) 
    {
         filem_cmd_send_command(iSlot, &sCmdPar);

         zlog_debug(FILEM_DEBUG_UPGRADE, "set get state command to slot=%d\r\n", iSlot);
    }
    
    return(0);
}

/*---------------------------------------------------------*/
/* Name: filem_cmd_subcarddebugset                         */
/* Retu: 成功返回0 失败返回-x                                      */
/* Desc: 设置子卡的FILEMDEBUG                                   */
/*---------------------------------------------------------*/
int filem_cmd_subcarddebugset(int iSlot, int iEnable)
{
    filemoperate sCmdPar = {0};
    
    memset(&sCmdPar, 0, sizeof(filemoperate));
    
    sCmdPar.iFlag    = 0;
    sCmdPar.iSlotno  = iSlot;
    sCmdPar.iDtype   = iEnable;
    sCmdPar.iCmdType = FILEM_OPCMD_DBGENABLE;
    
    if(filem_slot_issubcpu(iSlot)) 
    {
         filem_cmd_send_command(iSlot, &sCmdPar);

         zlog_debug(FILEM_DEBUG_UPGRADE, "set debug enable command to slot=%d, state=%d\r\n", iSlot, iEnable);
         return(0);
    }
    else return(-FILEM_ERROR_HWMATCHERR);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

