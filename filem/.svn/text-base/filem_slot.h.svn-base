/*file management head file
  name filem_slot.h
  edit suxq
  date 2016/05/23
  desc file data slot manage 
*/
#ifndef _FILEM_SLOT_H_

#define _FILEM_SLOT_H_

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#define FILEM_SLOT_COUN_MAX   32

#define FILEM_SLOT_NUM_ALL    32

enum
{
    FILEM_SLOT_INSERT = 0,

    FILEM_SLOT_EXSERT = 1,

    FILEM_SLOT_CONFIG = 2
};

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/* board slot info struct */
typedef struct _filemslot_
{
    int            unit;

    int            slot;                       /*槽位*/

    int            munit;                      /*表示主板槽位*/
    
    int            mslot;

    int            hwstate;                    /*硬件状态*/

    unsigned int   boardid;                    /*板卡 ID*/

    int            iscpu;                      /*是否本板具有文件管理能力*/

    int            issync;                     /*正在同步状态*/

    int            confsync;                   /*同步配置参数*/

    int            hastat;                     /*槽位的HA状态*/

    int            sysstat;                    /*槽位的系统启动状态*/ 

    int            syntimer;                   /*同步定时器 或上报定时器*/

    int            gstTimer;                   /*GET STAT 的定时器*/
    
    short          rcvtype;                    /* 细分类型      */

    short          rcvflags;         
    
    int            rcvoffset;                  /* 有效数据当前偏移*/

    int            rcvtotlen;                  /* 有效数据总长    */  
    
    unsigned int   rcvserno;                   /* 接收序号, 用于接收匹配*/  

    int            rcvtout;                    /* 报文接收超时*/

    char          *rcvbuff;

                                      
    unsigned int   sndserno;                   /* 发送序号*/  
    
    char           name[FILEM_DEVNAME_MLEN];   /*板卡名*/

    char           hwinfo[FILEM_DEVNAME_MLEN]; /*硬件信息, 保存FPGA描述*/
    
    struct list    synclist;                   /*当前同步文件队列*/

    struct list    filelist[FILEM_MTYPE_MAX];

    filemstatus    filestainf;                /*文件状态信息*/

    kernelarea     fileknltbl[FILEM_KRL_AREA_MAX];
                                              /*kernel IMG 信息*/

}filemslot;



/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
int filem_slot_init(void);

void filem_slot_clrlist(filemslot *pSlot);

int filem_slot_sendmsg(struct ipc_mesg_n *pMsg, int iLen);

int filem_slot_sendcpmsg(struct ipc_mesg_n *pMsg, int iLen);

int filem_slot_brdname(int iSlot, char *psname);

unsigned int filem_slot_brdid(int iSlot);

int filem_slot_fpgahwinf(int iSlot, char *phwinf);

filemslot* filem_slot_objectget(int iSlot);

filemstatus* filem_slot_stainfoget(int iSlot);

kernelarea* filem_slot_knltblget(int iSlot);

struct list* filem_slot_filelist(int iSlot, int iType);

int filem_slot_cpuhwcheck(int iSlot);

int filem_slot_isinsert(int iSlot);

void filem_slot_devmmsg(struct ipc_mesg_n *pMsg, int iLen);

int  filem_slot_info(char **bBufDat);

int  filem_slot_slaveslot(void);

void filem_slot_setsync(int iSlot, int iSync);

int filem_slot_cansync(int iDes);

int filem_slot_canrtsync(int iDes);

int filem_slot_ismaster(void);

int filem_slot_issubcpu(int iSlot);

int filem_slot_ismaincpu(int iSlot);

int filem_slot_isnosync(void);

int filem_slot_verinfo(char *bBufDat, int islot);

filemstatus* filem_slot_stanextget(int index, int exact);


#endif

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

