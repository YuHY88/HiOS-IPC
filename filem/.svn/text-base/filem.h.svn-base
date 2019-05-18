/*file management head file
  name filem.h
  edit suxq
  date 2016/05/23
*/
#ifndef _FILEM_H_

#define _FILEM_H_

#include "lib/linklist.h"
//#include "filem_cmd.h"


typedef  void (*FilemCall)(int iResult, char *pResInfo, void *pPar, int iFlag);


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
//#define FILEM_PATH_SPCHR  "/"

#define FILEM_NULL        ""
#define FILEM_PATH_DEF    "/"
#define FILEM_PATH_BIN    "/home/bin/"
#define FILEM_PATH_CFG    "/data/cfg/"
#define FILEM_PATH_DAT    "/data/dat/"
#define FILEM_PATH_VER    "/data/ver/"
#define FILEM_PATH_TMP    "/data/ver/tmp/"
/*-----------------------------------------------------------------------------------------*/

#define FILEM_FILE_CHANGE_LABLE "lablelist.dat"

#define FILEM_FILE_UPGRADE_TIME "/data/dat/upgradetime.txt"

#define FILEM_FILE_RUN_CONFIG   "startup.conf"

#define FILEM_FILE_INIT_CONFIG  "config.init"

#define FILEM_FILE_SDH_CONFIG   "sdh_config.tar.gz"
/*-----------------------------------------------------------------------------------------*/


#define FILEM_DEV_NAME       "null"
#define FILEM_BRD_NAME       "main"


#define FILEM_UNIT           1
#define FILEM_BK_SLOT        0

#define FILEM_SLOT_COUN_MAX  32
#define FILEM_SLOT_NUM_ALL   32


#define FILEM_NEWLINE(pVty) ((((struct vty *)pVty)->type == VTY_TERM || ((struct vty *)pVty)->type == VTY_SHELL_SERV) ? "\r\n" : "\n")

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define FILEM_NAME_MAXLN     128

#define FILEM_PATH_MAXLN     64+1    /* 用户输入的字符限制不包含字符串结束符 '\0' */

#define FILEM_VERS_MAXLN     28      /* 固定值 */

#define FILEM_ADDR_STRLN     64

#define FILEM_USER_STRLN     64


#define FILEM_TIMER_BASE     1       /*基本定时器 1000MS*/ 

#define FILEM_DEVNAME_MLEN   64

#define FILEM_MD5CHECK_LEN   131072  /*为节省资源, 不做全文件MD5*/

//#define FILEM_DEBUG(fmt,args...)  if(gfiledebugsw) printf(fmt, ##args)

#define FILEM_DEBUG_TFTP     0x01
#define FILEM_DEBUG_UPGRADE  0x02
#define FILEM_DEBUG_SYNC     0x04
#define FILEM_DEBUG_TRANS    0x08
#define FILEM_DEBUG_FTP      0x10
#define FILEM_DEBUG_OTHER    0x20
#define FILEM_DEBUG_ALL      0x3F


/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

enum
{
    FILEM_ERROR_NONE         = 0,        /*仅提供给网管上报*/
    FILEM_ERROR_RUNNING      = 1,        /*仅提供给网管上报*/
    FILEM_ERROR_FINISHED     = 2,        /*仅提供给网管上报*/
    FILEM_ERROR_NAME         = 3,
    FILEM_ERROR_PATH         = 4,
    FILEM_ERROR_LENGTH       = 5,
    FILEM_ERROR_EXIST        = 6,
    FILEM_ERROR_NOEXIST      = 7,
    FILEM_ERROR_NOSPACE      = 8,
    FILEM_ERROR_TOOLONG      = 9,
    FILEM_ERROR_COPY         = 10,
    FILEM_ERROR_DELETE       = 11,
    FILEM_ERROR_RENAME       = 12,
    FILEM_ERROR_UNKTYPE      = 13,
    FILEM_ERROR_SAMENAME     = 14,
    FILEM_ERROR_OPEN         = 15,
    FILEM_ERROR_READ         = 16,
    FILEM_ERROR_WRITE        = 17,
    FILEM_ERROR_CLOSE        = 18,
    FILEM_ERROR_VERIFY       = 19,
    FILEM_ERROR_NOSLOT       = 20,
    FILEM_ERROR_NOOP         = 21,
    FILEM_ERROR_CREATSOCK    = 22,
    FILEM_ERROR_SENDDATAFAIL = 23,
    FILEM_ERROR_WAITRECVFAIL = 24,
    FILEM_ERROR_RECVTIMEOUT  = 25,
    FILEM_ERROR_RECVDATAFAIL = 26,
    FILEM_ERROR_PKTSERNOERROR= 27,
    FILEM_ERROR_CONNECTFAIL  = 28,
    FILEM_ERROR_SENDRETYFAIL = 29,
    FILEM_ERROR_PKTACKERROR  = 30,
    FILEM_ERROR_TOOMANYWASTE = 31,
    FILEM_ERROR_DLOADFAIL    = 32,
    FILEM_ERROR_CREATETHREAD = 33,
    FILEM_ERROR_MD5CHKERR    = 34,
    FILEM_ERROR_MD5PADERR    = 35,
    FILEM_ERROR_SLOTNOINS    = 36,
    FILEM_ERROR_HWMATCHERR   = 37,
    FILEM_ERROR_UPDATEAPP    = 38,
    FILEM_ERROR_UPDATEKNL    = 39,
    FILEM_ERROR_UPDATEFPGA   = 40,
    FILEM_ERROR_UPDATEBOOT   = 41,
    FILEM_ERROR_NOKNLAREA    = 42,
    FILEM_ERROR_SLOTERROR    = 43,
    FILEM_ERROR_DISKFULL     = 44,
    FILEM_ERROR_CANCEL       = 45,
    FILEM_ERROR_CHANBUSY     = 46,
    FILEM_ERROR_BTBTRANS     = 47,
    FILEM_ERROR_TRANSTOUT    = 48,
    FILEM_ERROR_DIRNAMECONF  = 49,
    FILEM_ERROR_NOMEM        = 50,
    FILEM_ERROR_PARAM        = 51,
    FILEM_ERROR_CONFLICT     = 52,
    FILEM_ERROR_ACTKERNEL    = 53,
    FILEM_ERROR_FILEMATCH    = 54,
    FILEM_ERROR_REQREPEAT    = 55,
    FILEM_ERROR_NOSUPPORT    = 56,
    FILEM_ERROR_IPADDRERR    = 57,
    FILEM_ERROR_NOMASTER     = 58,
    FILEM_ERROR_AUTHERROR    = 59,

    FILEM_ERROR_OTHERERROR   = 60,
     
    FILEM_ERROR_MAXNO        = 61,   
};
/*-----------------------------------------------------------------------------------------*/

                             /*文件存储类型(主)*/
enum FILEM_MTYPE
{
    FILEM_MTYPE_NON = 0,     /*其他文件*/
    FILEM_MTYPE_BIN = 1,     /*执行文件*/
    FILEM_MTYPE_CFG = 2,     /*配置文件等*/
    FILEM_MTYPE_DAT = 3,     /*配置文件等*/
    FILEM_MTYPE_VER = 4,     /*历史版本*/
#if 1    
    FILEM_MTYPE_TMP = 5,     /*临时文件*/
#endif
    FILEM_MTYPE_MAX = 6,   
};

                             /*文件存储类型(子)*/
                             /*文件类型及权限翻译*/
enum FILEM_STYPE
{
    FILEM_STYPE_FUNK = 0,    /* 未知类型  */
    FILEM_STYPE_FIFO = 1,    /* fifo 文件 */
    FILEM_STYPE_FCHR = 2,    /* fifo 文件 */
    FILEM_STYPE_FDIR = 3,    /* 目录      */
    FILEM_STYPE_FBLK = 4,    /* 块文件    */
    FILEM_STYPE_FREG = 5,    /* 正常文件  */
    FILEM_STYPE_FLNK = 6,    /* 连接文件  */
    FILEM_STYPE_FSOK = 7,    /* socket文件*/
};

#define FILEM_PERM_IRUSR     0x0100   /* read permission, owner */
#define FILEM_PERM_IWUSR     0x0080   /* write permission, owner */
#define FILEM_PERM_IXUSR     0x0040   /* execute/search permission, owner */
#define FILEM_PERM_IRWXU     0x01c0   /* read/write/execute permission, owner */

#define FILEM_PERM_IRGRP     0x0020   /* read permission, group */
#define FILEM_PERM_IWGRP     0x0010   /* write permission, group */
#define FILEM_PERM_IXGRP     0x0008   /* execute/search permission, group */
#define FILEM_PERM_IRWXG     0x0038   /* read/write/execute permission, group */

#define FILEM_PERM_IROTH     0x0004   /* read permission, other */
#define FILEM_PERM_IWOTH     0x0002   /* write permission, other */
#define FILEM_PERM_IXOTH     0x0001   /* execute/search permission, other */
#define FILEM_PERM_IRWXO     0x0007   /* read/write/execute permission, other */

/*-----------------------------------------------------------------------------------------*/

#define FILEM_OPTFLG_UPDAPP   0x0001
#define FILEM_OPTFLG_UPDKNL   0x0002
#define FILEM_OPTFLG_UPDFPGA  0x0004
#define FILEM_OPTFLG_DELETE   0x0008
#define FILEM_OPTFLG_FTP      0x0010

#define FILEM_OPTFLG_BYSYS    0x0100


#define FILEM_OPTACT_COPY     0x0001  /*操作标志位*/
#define FILEM_OPTACT_RENAME   0x0002
#define FILEM_OPTACT_DLOAD    0x0004
#define FILEM_OPTACT_SYNC     0x0008
#define FILEM_OPTACT_ALL      0x000F

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

typedef struct _fileitems_
{
    struct listnode  sNode;

    int              iMtype;
    
    int              iStype;
    
    int              iSlot;      
    
    int              iIndex;

    int              iFlag;

    int              iUid;

    int              iGid;

    int              iSize;

    int              iPerm;

    int              iBlocks;

    int              iBlkSize;
    
    unsigned long    iModTime;
    
    unsigned long    iAccTime;
    
    unsigned long    iChgTime;
 
    int              iChgMd5;

    int              iHmacLen;

    char             bHmacData[64];

    char             bName[FILEM_NAME_MAXLN];

//  char             bVers[FILEM_NAME_MAXLN];
}fileitems;


typedef struct _filemtypes_
{
     int            iTimeOut;

     int            iFlags;

     char           bPrePath[FILEM_NAME_MAXLN];
}filemtypes;


/* file 接收snmp消息子类型 */
enum
{
    FILEM_SNMP_GET_INVALID = 0, 
    FILEM_SNMP_GET_SYSCONFIG,
    FILEM_SNMP_GET_STATUS,  
    FILEM_SNMP_GET_FILELIST, 
    FILEM_SNMP_GET_MAX = 4,
};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
typedef struct _filemstatus_
{
    int            iSlotNum;
    
    int            iDownload;
    int            iUpload;

    int            iUpgrade;        /*sys*/
    unsigned long  iUpgradeTime;

    int            iUpgFpga;
    unsigned long  iUpgFpgaTime;

#if 0    
    int            iUpgApp;        /*not used*/
    unsigned long  iUpgAppTime;
#endif    
    int            iUpgKnl;
    unsigned long  iUpgKnlTime;

    unsigned int   iFreeSpace;      /*剩余空间，KB*/
    
    int            iBootStat;

    unsigned int   iSlotFlags;

    int            iL3Vpn;
    
    char           cSoftVer[FILEM_VERS_MAXLN];
    char           cFpgaVer[FILEM_VERS_MAXLN];
    char           cKernelVer[FILEM_VERS_MAXLN];
    char           cBootVer[FILEM_VERS_MAXLN];

}filemstatus;


#define FILEM_SLOT_FLAG_MAINCPU  0x010000


typedef struct _filemsnmp_
{
    int             iIndex;

    int             bType;

    int             iSize;

    unsigned long   iModTime;

    char            bName[FILEM_NAME_MAXLN];
    
}filemsnmp;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

extern int  gfilelocslot;

extern int  gfiledebugsw;

extern int  gfilemtdkernl;

extern char gbfiledevname[];

extern filemtypes  gfiletypesman[];


//extern filemstatus gsfileoptstat;
//extern filemsyscfg gsfilesysinfo;
/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
char* filem_sys_errget(int iErrNo);

int   filem_device_ht15x(void);

//void  filem_sys_setdebug(int iState);

void  filem_upgrade_time_set(unsigned long timev);

int   filem_diskspace_get(void);

char* filem_type_pathget(int type);

int   filem_name_totype(char *pLocName);

int   filem_action_flagset(int itype, int flag);

int   filem_action_flagclr(int itype, int flag);

int   filem_action_flagchk(int itype, int flag);

int   filem_tftp_l3vpn_set(int iVpnno);

int   filem_tftp_l3vpn_get(void);

void  filem_download_stat_set(int iStat);

void  filem_upload_stat_set(int iStat);

void  filem_upgrade_stat_set(int iStat);

void  filem_upgfpga_stat_set(int iStat);

void  filem_upgfpga_time_set(unsigned long timev);
/*
void  filem_upgapp_stat_set(int iStat);

void  filem_upgapp_time_set(unsigned long timev);
*/
void  filem_upgkernel_stat_set(int iStat);

void  filem_upgkernel_time_set(unsigned long timev);

int   filem_upgrade_is_busy(void);

int   filem_init(void);

int   filem_cli_init(void);

void  filem_show_listclr(struct list *plist);

int   filem_show_dirlist(int iType, struct list *pList, char *pDir, int iGetst);

void  filem_show_timeout(void);

void  filem_show_timetostr(time_t iSecond, char *bDat);

int   filem_show_filelist(int iSlot, int iType, char *pPath, char **bBufDat);

int   filem_show_byindex(filemsnmp *pFileBuf, unsigned int index, int exact);

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

