
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "net-snmp-config.h"
#include <lib/types.h>
#include "types.h"

#include "net-snmp-includes.h"
#include "net-snmp-agent-includes.h"

#include "snmp_index_operater.h"

#include <lib/msg_ipc.h>
#include <lib/pkt_type.h>
#include <lib/module_id.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/pkt_buffer.h>
#include "lib/msg_ipc.h"
#include "lib/memshare.h"
#include "lib/msg_ipc_n.h"
#include <lib/ether.h>
#include <lib/ifm_common.h>
#include "filem/filem.h"
#include "mib_filem.h"
#include <lib/log.h>
#include <lib/devm_com.h>

//#include "lib/snmp_common.h"
#include "snmp_config_table.h"


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define FILEM_OIDPREFIX 1,3,6,1,4,1,9966,5,35,8,13

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

FindVarMethod filem_system_config_get;

FindVarMethod filem_filesys_filelist_get;

FindVarMethod filem_filesys_status_get;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

oid  filemSystemOid[] = { FILEM_OIDPREFIX};

oid  filemDevFileListTableOid[] = { FILEM_OIDPREFIX , 6};

oid  filemBoardVersionTableOid[] = { FILEM_OIDPREFIX , 7};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

struct variable1 filemFileSystemVariables[] =
{
    {filemFlieSysSatusUpgrade,   ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {1}},
    {filemFileSysSatusDownload,  ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {2}},
    {filemFileSysSatusUpload,    ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {3}},
    {filemFileSysL3VPNNumber,    ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {4}},
    {filemFileSysStaFpgaUpgrade, ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {5}},
    {filemFileSysStaKrnlUpgrade, ASN_INTEGER,    RONLY,    filem_filesys_status_get, 1, {10}},
} ;


struct variable2 filemDevFileListTableVariables[] =
{
    {filemFileSysDevFileIndex,      ASN_INTEGER,    RONLY,    filem_filesys_filelist_get, 2, {1, 1}},
    {filemFileSysDevFileName,       ASN_OCTET_STR,  RONLY,    filem_filesys_filelist_get, 2, {1, 2}},
    {filemFileSysDevFileTye,        ASN_INTEGER,    RONLY,    filem_filesys_filelist_get, 2, {1, 3}},
    {filemFileSysDevFileSize,       ASN_INTEGER,    RONLY,    filem_filesys_filelist_get, 2, {1, 4}},
    {filemFileSysDevFileCreatetime, ASN_UNSIGNED,   RONLY,    filem_filesys_filelist_get, 2, {1, 5}},
} ;

struct variable2 filemSystemConfigVariables[] =
{
    {filemSystemSlotNum,    ASN_INTEGER,    RONLY,    filem_system_config_get, 2, {1, 1}},
    {filemSystemSoftVer,    ASN_OCTET_STR,  RONLY,    filem_system_config_get, 2, {1, 2}},
    {filemSystemFpgaVer,    ASN_OCTET_STR,  RONLY,    filem_system_config_get, 2, {1, 3}},
    {filemSystemKernelVer,  ASN_OCTET_STR,  RONLY,    filem_system_config_get, 2, {1, 4}},
    {filemSystemUpgradeTime, ASN_UNSIGNED,  RONLY,    filem_system_config_get, 2, {1, 5}},
    {filemSystemFreeSpace,  ASN_INTEGER,    RONLY,    filem_system_config_get, 2, {1, 6}},
    {filemSystemHardwareVer,  ASN_OCTET_STR,RONLY,    filem_system_config_get, 2, {1, 7}},
} ;


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
void  init_mib_filem(void)
{
    REGISTER_MIB("ipran/filemFileSystemSta", filemFileSystemVariables, variable1, filemSystemOid);
    REGISTER_MIB("ipran/filemFileListTable", filemDevFileListTableVariables, variable2, filemDevFileListTableOid);
    REGISTER_MIB("ipran/filemFileSystemVer", filemSystemConfigVariables, variable2, filemBoardVersionTableOid);
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

int filem_snmp_sendwait(void *pData, int iDatLen, int iDatNum,
                        unsigned char iStype, unsigned char iOpCod, unsigned int iIndex)
{
    int    iRetv = 0;
    int    iRepL = 0;

    struct ipc_mesg_n *pMsgSnd = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgSnd = mem_share_malloc(sizeof(struct ipc_mesg_n), MODULE_ID_SNMPD);

    if(pMsgSnd == NULL) return(-1);

    memset(pMsgSnd, 0, sizeof(struct ipc_mesg_n));
 
    pMsgSnd->msghdr.data_len  = 0;
    pMsgSnd->msghdr.module_id = MODULE_ID_FILE;
    pMsgSnd->msghdr.sender_id = MODULE_ID_SNMPD;
    pMsgSnd->msghdr.msg_type  = IPC_TYPE_SNMP;
    pMsgSnd->msghdr.msg_subtype = iStype;
    pMsgSnd->msghdr.opcode      = iOpCod;
    pMsgSnd->msghdr.msg_index   = iIndex;
    pMsgSnd->msghdr.data_num    = iDatNum;

    iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_mesg_n), &pMsgRcv, &iRepL, 5000);

    switch(iRetv)
    {
       case -1:    
             mem_share_free(pMsgSnd, MODULE_ID_SNMPD);
             return(-1);

       case -2:    
            return(-1);

       case  0:
             if(pMsgRcv == NULL) iRetv = -1;
             else 
             {
                 if(pMsgRcv->msghdr.data_len >= (unsigned int)iDatLen)
                 {
                      iRetv = 0;
                      memcpy(pData, pMsgRcv->msg_data, iDatLen);
                 }
                 else iRetv = -1;

                 mem_share_free(pMsgRcv, MODULE_ID_SNMPD);
             }
             return(iRetv);
             
        default: return(-1);
    }
}

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
u_char *filem_system_config_get(struct variable *pVar,
                                oid *pName,  size_t *iOidLen,
                                int  iExact, size_t *iVarlen, WriteMethod **write_method)
{
    int    iRetVal = 0;
    char  *pErrInf = {0};
    unsigned int iIndex = 0;
    static filemstatus sFileRec;
    static struct devm_version version;
    int             iSize;
    unsigned long   iModTime;
    char            bName[FILEM_NAME_MAXLN];
    char            bType[16];
    static u_long    ulTime;

    memset(&sFileRec, 0, sizeof(filemstatus));
    memset(&version, 0, sizeof(struct devm_version));

    iRetVal = ipran_snmp_int_index_get(pVar, pName, iOidLen, &iIndex, iExact);

    if (iRetVal < 0)
    {
        return (NULL);
    }

    //iExact = iExact ? IPC_OPCODE_GET : IPC_OPCODE_GET_BULK;

    iRetVal = filem_snmp_sendwait(&sFileRec, sizeof(filemsnmp), 1,
                                  FILEM_SNMP_GET_SYSCONFIG, iExact, iIndex);

    if (iRetVal != 0)
    {
        return (NULL);
    }

    iRetVal = devm_comm_get_slot_version(1, sFileRec.iSlotNum, MODULE_ID_SNMPD, &version);

    if (iRetVal != 0)
    {
        return (NULL);
    }

    ipran_snmp_int_index_set(pVar, pName, iOidLen, sFileRec.iSlotNum);

    switch (pVar->magic)
    {
        case filemSystemSlotNum:
            *iVarlen = sizeof(unsigned int);
            return ((u_char *)&sFileRec.iSlotNum);

        case filemSystemSoftVer:
            *iVarlen = strlen(version.softver);
            return (u_char *)version.softver;

        case filemSystemFpgaVer:
            *iVarlen = strlen(version.fpgaver);
            return (u_char *)version.fpgaver;

        case filemSystemKernelVer:
            *iVarlen = strlen(sFileRec.cKernelVer);
            return (u_char *)sFileRec.cKernelVer;

        case filemSystemUpgradeTime:
            ulTime = sFileRec.iUpgradeTime;
            *iVarlen = sizeof(long);
            return ((u_char *)&ulTime);

        case filemSystemFreeSpace:
            *iVarlen = sizeof(int);
            return ((u_char *)&sFileRec.iFreeSpace);

        case filemSystemHardwareVer:
            *iVarlen = strlen(version.hwver);
            return (u_char *)version.hwver;

        default :
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}

u_char *filem_filesys_status_get(struct variable *pVar,
                                 oid *pName,  size_t *iOidLen,
                                 int  iExact, size_t *iVarlen, WriteMethod **write_method)
{
    int    iRetVal = 0;
    char  *pErrInf = {0};
    static filemstatus status;

    iRetVal = header_generic(pVar, pName, iOidLen,
                             iExact, iVarlen, write_method);

    if (iRetVal == MATCH_FAILED)
    {
        return (NULL);
    }

    iRetVal = filem_snmp_sendwait(&status, sizeof(filemstatus), 1,
                                  FILEM_SNMP_GET_STATUS, iExact, 0);

    if (iRetVal == 0)
    {
        switch (pVar->magic)
        {
            case filemFlieSysSatusUpgrade:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iUpgrade);

            case filemFileSysSatusDownload:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iDownload);

            case filemFileSysSatusUpload:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iUpload);
            
            case filemFileSysL3VPNNumber:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iL3Vpn);

            case filemFileSysStaFpgaUpgrade:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iUpgFpga);

            case filemFileSysStaKrnlUpgrade:
                *iVarlen = sizeof(int);
                return ((u_char *)&status.iUpgKnl);

            default :
                return (NULL);
        }
    }
    else
    {
        return (NULL);
    }
}


u_char *filem_filesys_filelist_get(struct variable *pVar,
                                   oid *pName,  size_t *iOidLen,
                                   int  iExact, size_t *iVarlen, WriteMethod **write_method)
{
    int    iRetVal = 0;
    char  *pErrInf = {0};
    unsigned int iIndex = 0;
    static filemsnmp sFileRec;

    int             iSize;
    unsigned long   iModTime;
    char            bName[FILEM_NAME_MAXLN];
    char            bType[16];
    static u_long    ulTime;

    memset(&sFileRec, 0, sizeof(filemsnmp));

    iRetVal = ipran_snmp_int_index_get(pVar, pName, iOidLen, &iIndex, iExact);

    if (iRetVal < 0)
    {
        return (NULL);
    }

    //iExact = iExact ? IPC_OPCODE_GET : IPC_OPCODE_GET_BULK;

    iRetVal = filem_snmp_sendwait(&sFileRec, sizeof(filemsnmp), 1,
                                  FILEM_SNMP_GET_FILELIST, iExact, iIndex);

    if (iRetVal != 0)
    {
        return (NULL);
    }

    ipran_snmp_int_index_set(pVar, pName, iOidLen, sFileRec.iIndex);

    switch (pVar->magic)
    {
        case filemFileSysDevFileIndex:
            *iVarlen = sizeof(unsigned int);
            return ((u_char *)&sFileRec.iIndex);

        case filemFileSysDevFileSize:
            *iVarlen = sizeof(unsigned int);
            return ((u_char *)&sFileRec.iSize);

        case filemFileSysDevFileTye:
            *iVarlen = sizeof(unsigned int);
            return ((u_char *)&sFileRec.bType);

        case filemFileSysDevFileCreatetime:
            ulTime = sFileRec.iModTime;
            *iVarlen = sizeof(long);
            return ((u_char *)&ulTime);

        case filemFileSysDevFileName:
            *iVarlen = strlen(sFileRec.bName);
            return (u_char *)sFileRec.bName;

        default :
            zlog_debug(SNMP_DBG_MIB_GET, "%s[%d] index default\n", __FUNCTION__, __LINE__);
            return (NULL);
    }
}


