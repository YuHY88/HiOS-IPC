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
    short           iSType;         /* ϸ������        */

    short           iFlags;  
    
    short           iFragCou;       /* �ܷ�Ƭ�� 1-X    */
                                
    short           iFragCur;       /* ��ǰ��Ƭ��1-X   */
                                    /* ��ͬ���Ϊͬһ���ĵķ�Ƭ*/
    int             iDatLen;        /* ��ǰ�������ݳ��� ��������ͷ*/ 
    
    int             iOffset;        /* ��Ч���ݵ�ǰƫ��*/

    int             iTotalLen;      /* ��Ч�����ܳ�    */  
}file_listhdr;

typedef struct _file_syncitem_
{
    struct listnode  sNode;

    int              iUnit;

    int              iSlot;         /* Ŀ���λ*/

    int              iModule;
    
    int              iType;
    
    char             iActive;       /* ���� ADD DEL UPDATE*/

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
                                    /*�ļ�ͬ���ڲ�ʹ�õĲ�����־*/

#define FILEM_CFGSYNC_COUNT   2     /*����ʱ�����ļ�ͬ������*/

#define FILEM_RTTRANS_RETRY   2     /*ʵʱ�������Դ���*/

#define FILEM_RTTRANS_TOUT    90    /*90s�޻ص���ʱ*/

#define FILEM_FRESH_TIMER     25    /*�ļ��б�ˢ��ʱ�� SLAVE ˢ���ϱ�ʱ��*/

#define FILEM_MSYNC_DELAY     10    /*��CPUͬ���ļ��ӳ�*/

#define FILEM_STATEGET_TIMER  5     /*����Ҫһ��״̬*/

#define FILEM_RCVPKT_TOUT     6     /*��CPU����һ���б����ϱ���ʱ*/

#define FILEM_PERFRAG_FILES   ((IPC_MSG_LEN_N - 512) / sizeof(fileitems))

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

/*---------------------------------------------------------*/
/* Name: filem_sync_forcestop                              */
/* Retu: ��                                                */
/* Desc: �ļ�ͬ��ǿ�ƽ���                                  */
/*---------------------------------------------------------*/
void filem_sync_forcestop(int iHast);

/*---------------------------------------------------------*/
/* Name: filem_sync_timeout                                */
/* Retu: ��                                                */
/* Desc: �ļ�ͬ����ʱ��                                    */
/*---------------------------------------------------------*/
void filem_sync_timeout(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_statussend                             */
/* �汾��Ϣ������״̬�ϱ�  ����CPUִ��                     */
/*---------------------------------------------------------*/

int filem_sync_statussend(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_init                                   */
/* �ļ�ͬ����ʼ��                                          */
/*---------------------------------------------------------*/
int filem_sync_init(void);


/*---------------------------------------------------------*/
/* Name: filem_sync_rcvpkt                                 */
/* �ļ��б����  ��CPUִ��                                 */
/*---------------------------------------------------------*/
void filem_sync_packetrecv(struct ipc_mesg_n *pMsgPkt, int iDlen);

/*---------------------------------------------------------*/
/* Name: filem_sync_rtfilesync                             */
/* Retu: ��                                                 */
/* Desc: �ļ�ʵʱͬ��������                                        */
/*---------------------------------------------------------*/
int filem_sync_rtfilesync(int iModule, int iSlot, 
                          char iOpt, char iSynSla, char *pFile);

/*---------------------------------------------------------*/
/* Name: filem_sync_rttimeout                              */
/* Retu: ��                                                 */
/* Desc: �ļ�ͬ����ʱ��                                           */
/*---------------------------------------------------------*/
void filem_sync_rttimeout(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_rttranrun                              */
/* �ж��Ƿ�ִ��ʵʱ����                                       */
/*---------------------------------------------------------*/
int filem_sync_rttranrun(void);

/*---------------------------------------------------------*/
/* Name: filem_sync_batchcfgfile                           */
/* Retu: ��                                                 */
/* Desc: ������������ͬ���ļ�                                        */
/*---------------------------------------------------------*/

int filem_sync_rtfilerequest(char *pData, char iOpt, char iSynSla);

/*-----------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------*/

#endif
