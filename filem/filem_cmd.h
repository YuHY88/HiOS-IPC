/*file management head file
  name filem_cmd.h
  edit suxq
  date 2016/05/23
*/
#ifndef _FILEM_CMD_H_

#define _FILEM_CMD_H_

#include "filem_md5.h"

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
enum    /* file operate */
{
    FILEM_OPCMD_NOOP        = 0, 
    FILEM_OPCMD_DOWNLOAD    = 1, 
    FILEM_OPCMD_UPLOAD      = 2, 
    
    FILEM_OPCMD_COPY        = 3, 
    FILEM_OPCMD_DELETE      = 4, 
    FILEM_OPCMD_RENAME      = 5, 
    FILEM_OPCMD_MD5CHK      = 6, 
    FILEM_OPCMD_UPDATESYS   = 7, 
    FILEM_OPCMD_UPDATEPATCH = 8, 
    FILEM_OPCMD_UPDATEFPGA  = 9, 
    FILEM_OPCMD_UPDATEAPP   = 10, 
    FILEM_OPCMD_UPDATEKNL   = 11, 
    FILEM_OPCMD_UPDATEUBOOT = 12, 
    FILEM_OPCMD_ACTIVEKNL   = 13, 
    FILEM_OPCMD_GETSTATE    = 14,     
    FILEM_OPCMD_DBGENABLE   = 15,    
};


typedef struct _filemoperate_
{
     int            iCmdType;

     int            iProType;
     
     int            iSlotno;

     int            iResPort;

     int            iFlag;

     int            iDtype;

     char           bIpAddr[FILEM_ADDR_STRLN];
     
     char           bUsrName[FILEM_USER_STRLN];
     char           bPassWrd[FILEM_USER_STRLN];

     char           bSrcName[FILEM_NAME_MAXLN];
     char           bDesName[FILEM_NAME_MAXLN];
     
     char           bMd5Data[FILEM_PAD_MD5_BUSZ+8];

     void          *pCallPar;

     FilemCall      pEndCall;
}filemoperate;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
int filem_cmd_inputnamecheck(int iType, char *pInput, char *pOut);

int filem_cmd_inputipv4check(char *pAddr);

int filem_cmd_recv_command(struct ipc_mesg_n *pMsgPkt, int iDlen);

int filem_cmd_remove(int iType, char *pFile, FilemCall pEndCall, void *pPara, int iASyn);

int filem_cmd_copy(int iStype, char *pSFile, int iDtype, 
                   char *pDFile, FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_rename(int iType, char *pSFile, char *pDFile,
                     FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_tftpdnload(int iType, char *pIp, char *pRFile, char *pLFile, 
                         FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_tftpupload(int  iType,  char *pIp, char *pRFile, 
                         char *pLFile, FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_ftpdnload(int  iType, char *pIp, char *pUser, char *pPwd, 
                        char *pRFile, char *pLFile, FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_ftpupload(int  iType,  char *pIp, char *pUser, char *pPwd,
                        char *pRFile, char *pLFile, FilemCall pEndCall, void *pPara, int iAsyn);

int filem_cmd_upgradesys(int   iSlot, char *pFile, FilemCall pEndCall, void *pPara, int   iFlag, int iASyn);

int filem_cmd_upgardepatch(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn);

int filem_cmd_upgradefpga(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlag);

int filem_cmd_upgradekernel(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlag);

int filem_cmd_upgradeuboot(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn);

int filem_cmd_upgradeappbin(int iSlot, char *pFile, FilemCall pEndCall, void *pPara, int iASyn, int iFlag);

int filem_cmd_activekernel(int iSlot, int iIdx, FilemCall pEndCall, void *pPara, int iASyn);

int filem_cmd_stateupdateask(int iSlot);

int filem_cmd_subcarddebugset(int iSlot, int iEnable);


#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

