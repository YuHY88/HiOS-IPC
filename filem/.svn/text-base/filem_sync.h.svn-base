/*file management head file
  name filem_sync.h
  edit suxq
  date 2017/08/15
  desc file sync manage 
*/
#ifndef _FILEM_SYNC_H_

#define _FILEM_SYNC_H_

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

typedef struct _file_listhdr_
{
    short           iSType;         /* 细分类型        */

    short           iFlags;  
    
    short           iFragCou;       /* 总分片数 1-X    */
                                
    short           iFragCur;       /* 当前分片号1-X   */
                                    /* 相同序号为同一报文的分片*/
    int             iDatLen;        /* 当前报文数据长度 不含报文头*/ 
    
    int             iOffset;        /* 有效数据当前偏移*/

    int             iTotalLen;      /* 有效数据总长    */  
}file_listhdr;

typedef struct _file_syncitem_
{
    struct listnode  sNode;

    int              iUnit;

    int              iSlot;         /* 目标槽位*/

    int              iModule;
    
    int              iType;
    
    char             iActive;       /* 动作 ADD DEL UPDATE*/

    char             iIsRun;

    char             iRetry;

    char             iSynSlave;

    int              iTimeout;

    int              iResult;
    
    char             bLocName[FILEM_NAME_MAXLN];
    
    char             bRemName[FILEM_NAME_MAXLN];
}file_syncitem;

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/
                                    /*文件同步内部使用的操作标志*/

#define FILEM_CFGSYNC_COUNT   2     /*启动时配置文件同步次数*/

#define FILEM_RTTRANS_RETRY   2     /*实时传输重试次数*/

#define FILEM_RTTRANS_TOUT    90    /*90s无回调超时*/

#define FILEM_FRESH_TIMER     25    /*文件列表刷新时间 SLAVE 刷新上报时间*/

#define FILEM_MSYNC_DELAY     10    /*主CPU同步文件延迟*/

#define FILEM_STATEGET_TIMER  5     /*五秒要一次状态*/

#define FILEM_RCVPKT_TOUT     6     /*主CPU接收一类列表报文上报超时*/

#define FILEM_PERFRAG_FILES   ((IPC_MSG_LEN_N - 512) / sizeof(fileitems))

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_sync_forcestop                              */
/* Retu: 无                                                */
/* Desc: 文件同步强制结束                                  */
/*---------------------------------------------------------*/
void filem_sync_forcestop(int iHast);

/*---------------------------------------------------------*/
/* Name: filem_sync_timeout                                */
/* Retu: 无                                                */
/* Desc: 文件同步定时器                                    */
/*---------------------------------------------------------*/
void filem_sync_timeout(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_statussend                             */
/* 版本信息及操作状态上报  备用CPU执行                     */
/*---------------------------------------------------------*/

int filem_sync_statussend(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_init                                   */
/* 文件同步初始化                                          */
/*---------------------------------------------------------*/
int filem_sync_init(void);


/*---------------------------------------------------------*/
/* Name: filem_sync_rcvpkt                                 */
/* 文件列表接收  主CPU执行                                 */
/*---------------------------------------------------------*/
void filem_sync_packetrecv(struct ipc_mesg_n *pMsgPkt, int iDlen);

/*---------------------------------------------------------*/
/* Name: filem_sync_rtfilesync                             */
/* Retu: 无                                                 */
/* Desc: 文件实时同步请求处理                                        */
/*---------------------------------------------------------*/
int filem_sync_rtfilesync(int iModule, int iSlot, 
                          char iOpt, char iSynSla, char *pFile);

/*---------------------------------------------------------*/
/* Name: filem_sync_rttimeout                              */
/* Retu: 无                                                 */
/* Desc: 文件同步定时器                                           */
/*---------------------------------------------------------*/
void filem_sync_rttimeout(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_rttranrun                              */
/* 判断是否执行实时传输                                       */
/*---------------------------------------------------------*/
int filem_sync_rttranrun(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_batchcfgfile                           */
/* Retu: 无                                                 */
/* Desc: 配置数据批量同步文件                                        */
/*---------------------------------------------------------*/

int filem_sync_rtfilerequest(char *pData, char iOpt, char iSynSla);

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#endif
