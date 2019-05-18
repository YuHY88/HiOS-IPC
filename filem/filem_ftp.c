/*------------------------------------------------------------------------*/
/* �� �� �� : filem_ftp.c                                                   */
/* ��    �� : suxiqing                                                    */
/* �������� : 2018-12-25                                                  */
/* ��    Ȩ :                                                             */
/* ˵    �� : ͨ��TFTPЭ�����ػ������ļ���ʵ��                            */
/*------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h> 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <lib/thread.h>
#include <lib/msg_ipc_n.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/vty.h>
#include <lib/command.h>
#include <lib/timer.h>
#include <lib/log.h>
#include <lib/inet_ip.h>
#include <lib/memtypes.h>
#include <lib/memory.h>
#include "ftm/pkt_ip.h"
#include "ftm/pkt_udp.h"
#include "ftm/pkt_tcp.h"
#include "filem.h"
#include "filem_tftp.h"
#include "filem_md5.h"
#include "filem_oper.h"

/*====================================================================================================================*/

#define FILEM_FTP_SVR_LISPORT      21
#define FILEM_FTP_CLI_LISPORT      4020    /*4020 - 4120*/
#define FILEM_FTP_CLI_CONPORT      5020    /*5020 - 5120*/


#define FILEM_FTP_NOACT            0       /* �޲��� */
#define FILEM_FTP_DNLOAD           1       /* �����ļ� */
#define FILEM_FTP_UPLOAD           2       /* �ϴ��ļ� */

#define FILEM_FTP_ERRBUF_LEN       256     // > 128
#define FILEM_FTP_CMDBUF_LEN       1024
#define FILEM_FTP_DATBUF_LEN       1024
#define FILEM_FTP_PACKET_LEN       1024


#define FILEM_FTP_CMD_TIMEOUT      8
#define FILEM_FTP_DAT_TIMEOUT      600
#define FILEM_FTP_END_TIMEOUT      5

#define FILEM_FTP_RTY_REPEAT       3
#define FILEM_FTP_RTY_TIMEOUT      2

//#define FILEM_FTP_FILE_MAXSIZE     0x2000000

#define FILEM_FTP_RCV_UNKWDATA     0
#define FILEM_FTP_RCV_CTRLDATA     1
#define FILEM_FTP_RCV_FILEDATA     2

/*====================================================================================================================*/

/*����״̬��*/
enum
{   
    FILEM_FTP_CMD_ABOR = 1,
    FILEM_FTP_CMD_ACCT,
    FILEM_FTP_CMD_ALLO,           //no support
    FILEM_FTP_CMD_APPE,           //no support
    FILEM_FTP_CMD_CDUP,           //no support
    FILEM_FTP_CMD_CWD,  
    FILEM_FTP_CMD_DELE,
    FILEM_FTP_CMD_HELP,
    FILEM_FTP_CMD_LIST,
    FILEM_FTP_CMD_MODE,
    FILEM_FTP_CMD_MKD,            //no support  
    FILEM_FTP_CMD_NLST,           //no support
    FILEM_FTP_CMD_NOOP,  
    FILEM_FTP_CMD_PASS,   
    FILEM_FTP_CMD_PASV,  
    FILEM_FTP_CMD_PORT,  
    FILEM_FTP_CMD_PWD,  
    FILEM_FTP_CMD_QUIT,  
    FILEM_FTP_CMD_REIN,            //no support
    FILEM_FTP_CMD_REST,            //no support
    FILEM_FTP_CMD_RETR,  
    FILEM_FTP_CMD_RMD,  
    FILEM_FTP_CMD_RNFR,            //no support
    FILEM_FTP_CMD_RNTO,            //no support
    FILEM_FTP_CMD_SITE,            //no support
    FILEM_FTP_CMD_SIZE,
    FILEM_FTP_CMD_SMNT,            //no support
    FILEM_FTP_CMD_STAT,   
    FILEM_FTP_CMD_STOR,   
    FILEM_FTP_CMD_STOU,   
    FILEM_FTP_CMD_STRU,   
    FILEM_FTP_CMD_SYST,   
    FILEM_FTP_CMD_TYPE,   
    FILEM_FTP_CMD_USER,   
    FILEM_FTP_CMD_CONN,
    FILEM_FTP_CMD_DATTRS,          //WAIT DATA CONNECT
    FILEM_FTP_CMD_DATEND,
    FILEM_FTP_CMD_SESEND,          //END SESSION
};  
/*
    ABOR    �ж��������ӳ���
    ACCT <account>  ϵͳ��Ȩ�ʺ�
    ALLO <bytes>    Ϊ�������ϵ��ļ��洢�������ֽ�
    APPE <filename>     ����ļ���������ͬ���ļ�
    CDUP <dir path>     �ı�������ϵĸ�Ŀ¼
    CWD <dir path>  �ı�������ϵĹ���Ŀ¼
    DELE <filename>     ɾ���������ϵ�ָ���ļ�
    HELP <command>  ����ָ��������Ϣ
    LIST <name>     ������ļ����г��ļ���Ϣ�������Ŀ¼���г��ļ��б�
    MODE <mode>     ����ģʽ��S=��ģʽ��B=��ģʽ��C=ѹ��ģʽ��
    MKD <directory>     �ڷ������Ͻ���ָ��Ŀ¼
    NLST <directory>    �г�ָ��Ŀ¼����
    NOOP    �޶������������Է������ϵĳ���
    PASS <password>     ϵͳ��¼����
    PASV    ����������ȴ���������
    PORT <address>  IP ��ַ�����ֽڵĶ˿� ID
    PWD     ��ʾ��ǰ����Ŀ¼
    QUIT    �� FTP ���������˳���¼
    REIN    ���³�ʼ����¼״̬����
    REST <offset>   ���ض�ƫ���������ļ�����
    RETR <filename>     �ӷ��������һأ����ƣ��ļ�
    RMD <directory>     �ڷ�������ɾ��ָ��Ŀ¼
    RNFR <old path>     �Ծ�·��������
    RNTO <new path>     ����·��������
    SITE <params>   �ɷ������ṩ��վ���������
    SMNT <pathname>     ����ָ���ļ��ṹ
    STAT <directory>    �ڵ�ǰ�����Ŀ¼�Ϸ�����Ϣ
    STOR <filename>     ���棨���ƣ��ļ�����������
    STOU <filename>     �����ļ���������������
    STRU <type>     ���ݽṹ��F=�ļ���R=��¼��P=ҳ�棩
    SYST    ���ط�����ʹ�õĲ���ϵͳ
    TYPE <data type>    �������ͣ�A=ASCII��E=EBCDIC��I=binary��
    USER <username>     ϵͳ��¼���û���

    110 	���ļ�ָʾ���ϵ��������
    120 	������׼��������ʱ�䣨��������
    125 	���������ӣ���ʼ����
    150 	������
    200 	�ɹ�
    202 	����û��ִ��
    211 	ϵͳ״̬�ظ�
    212 	Ŀ¼״̬�ظ�
    213 	�ļ�״̬�ظ�
    214 	������Ϣ�ظ�
    215 	ϵͳ���ͻظ�
    220 	�������
    221 	�˳�����
    225 	����������
    226 	������������
    227 	���뱻��ģʽ��IP ��ַ��ID �˿ڣ�
    230 	��¼������
    250 	�ļ���Ϊ���
    257 	·��������
    331 	Ҫ������
    332 	Ҫ���ʺ�
    350 	�ļ���Ϊ��ͣ
    421 	����ر�
    425 	�޷�����������
    426 	��������
    450 	�ļ�������
    451 	�������ش���
    452 	���̿ռ䲻��
    500 	��Ч����
    501 	�������
    502 	����û��ִ��
    503 	����ָ������
    504 	��Ч�������
    530 	δ��¼����
    532 	�洢�ļ���Ҫ�ʺ�
    550 	�ļ�������
    551 	��֪����ҳ����
    552 	�����洢����
    553 	�ļ���������
*/

/*====================================================================================================================*/
/*====================================================================================================================*/
/*===================================================*/
/*FTP����ʵ�����ƿ�                                  */
/*===================================================*/
typedef struct _FILEM_FTP_ADDR_
{
    int              iIsReg;                  /*�Ƿ���ע��*/
    int              iIpver;

    unsigned short   iSpare;
    unsigned short   iPort;
    
    unsigned int     iIpv4;
    unsigned char    bIpv6[16];
    
   char              bAddStr[FILEM_NAME_MAXLN]; /*��ŵ�ַ�ַ���*/

}FILEM_FTP_ADDR;

typedef struct _FILEM_FTP_SOCK_
{
    int              iIsReg;                  /*�Ƿ���ע��*/
    int              iIsCon;                  /*�Ƿ�������*/
    int              iIpver;
    
    unsigned short   iSpare1;                 /*��¼Զ�˵�ַ*/                 
    unsigned short   iDport;    
    unsigned int     iDipv4;
    unsigned char    bDipv6[16];
                                              /*��¼���ص�ַ*/   
    unsigned short   iSpare2;                 
    unsigned short   iSport;
    unsigned int     iSipv4;
    unsigned char    bSipv6[16];
}FILEM_FTP_SOCK;



typedef struct _FILEM_FTP_CTRL_
{
    int              iTrsType;                /*1 ���� 2 �ϴ� */
    
    int              iFileType;
    
    int              iFileID;                 /*�ļ����*/            
    int              iFileLen;                /*���ػ������ļ�����  */  
    int              iCurrPos;                /*��ǰ���ͻ����λ��  */


    int              iReqState;               /*����״̬��*/
    int              iReqTimeout;

  //int              iRetryTout;
    int              iRetryCoun;
    
    
    unsigned short   iL3Vpn;                  /*0-127         */
    
    FILEM_FTP_ADDR   sSvrAddr;                /*Զ��������ַ*/

    FILEM_FTP_ADDR   sLocAddr;                /*����������ַ*/
   
    FILEM_FTP_SOCK   sCtrSock;                /*����ͨ������*/

    FILEM_FTP_SOCK   sDatSock;                /*����ͨ������*/

    int              iErrorCode;

    int              iEarlyCode;              /*��ǰ�յ���������*/
    
    int              iLastPcnt;
    void            *pProParam;
    FilemCall        pProcCall;
    

    int              iRecCmdLen;              /*���������ջ���      */
    int              iSndCmdLen;              /*����������      */ 
    int              iRecDatLen;              /*���������ջ���      */ 
    int              iSndDatLen;
    
    
    char             bUserName[FILEM_NAME_MAXLN];
    
    char             bPassWord[FILEM_NAME_MAXLN];
    
    char             bRemFname[FILEM_NAME_MAXLN]; 
                                           /*Զ���ļ��� ��·��   */
    char             bLocFname[FILEM_NAME_MAXLN]; 

    char             bLocTempN[FILEM_NAME_MAXLN];
                                           /*�����ļ��� ��·��   */
                                           /*���������Ϣ        */ 
    char             bSvrErrBuf[FILEM_FTP_ERRBUF_LEN]; 
                                           
    char             bErrInfBuf[FILEM_FTP_ERRBUF_LEN];
                                           
    char             bOneCmdBuf[FILEM_FTP_CMDBUF_LEN];
                                           
    char             bRecCmdBuf[FILEM_FTP_CMDBUF_LEN];  
    
    char             bSndCmdBuf[FILEM_FTP_CMDBUF_LEN]; 
                                            /*�ļ������շ�����*/
    char             bRSDataBuf[FILEM_FTP_DATBUF_LEN];             
}FILEM_FTP_CTRL;

/*====================================================================================================================*/

//extern struct thread_master *filemmaster;

static unsigned short iFtpCtrPortIdx = 0;   /*ѭ��ʹ��0-99�˿�*/

FILEM_FTP_CTRL sFtpSessCtl = {0} ;

/*====================================================================================================================*/
/*====================================================================================================================*/
/*common fun*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_out_percent*/
/*Para: pftpctl */
/*Dest: output run percent infor */
/*---------------------------------------------------------*/

static void filem_ftp_out_percent(FILEM_FTP_CTRL *pftpCtl, int iForce)
{
    int  iPercnt = 0;
    char bProInf[128] = {0};
    
    if((pftpCtl->iFileLen > 0) && pftpCtl->pProcCall)
    {        
        iPercnt = (int)((float)pftpCtl->iCurrPos / pftpCtl->iFileLen * 100);
        
        if((pftpCtl->iLastPcnt != iPercnt) || iForce)
        {
            pftpCtl->iLastPcnt = iPercnt;
    
            sprintf(bProInf, "\033[1A""\033[K""Percent:    %3d%% \r\n", iPercnt);                  
    
            pftpCtl->pProcCall(0, bProInf, pftpCtl->pProParam, 0);      
        }
    }
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_is_decnumber*/
/*Para: pStr */
/*Dest: check a string is dec number or not  */
/*---------------------------------------------------------*/

static int filem_ftp_is_decnumber(char *pStr)
{
    int ulLen = 0;
    int ulIdx = 0;
    
    if(pStr == NULL) return(0);

    ulLen = strlen(pStr);

    if(ulLen == 0) return(0);
                                         /*����Ƿ�ȫ��Ϊ����*/
    for(; ulIdx < ulLen; ulIdx++)
    {
        if((pStr[ulIdx] < '0') || (pStr[ulIdx] > '9')) break;
    }

    return((ulIdx < ulLen) ? 0 : 1);
}




/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_register_loccnt*/
/*Para: sIpAdd */
/*Dest: register connect sock to ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_register_loccnt(FILEM_FTP_SOCK *pSocket)
{
    int   retval = -1;

    if(!pSocket->iIsReg)
    {
        union proto_reg proto;
        
        pSocket->iIsReg = 1;

        memset(&proto, 0, sizeof(union proto_reg));
        
        proto.ipreg.protocol = IP_P_TCP;
        proto.ipreg.dport    = pSocket->iSport;   //des = loc
        
        if(pSocket->iIpver == INET_FAMILY_IPV4)
        {
             retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
        }
        else  
        {
             retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
        }
    }
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_unregister_loccnt*/
/*Para: sIpAdd */
/*Dest: unregister listen sock from ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_unregister_loccnt(FILEM_FTP_SOCK *pSocket)
{
    int retval = -1;

    if(pSocket->iIsReg)
    {
        union proto_reg proto;

        pSocket->iIsReg = 0;

        memset(&proto, 0, sizeof(union proto_reg));
        
        proto.ipreg.protocol = IP_P_TCP;
        proto.ipreg.dport    = pSocket->iSport;     //des == loc
        
        if(pSocket->iIpver == INET_FAMILY_IPV4)
        {
             pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
        }
        else  
        {        
             pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
        }
    }
    
    return(retval);
}


/*---------------------------------------------------------*/
/*Name: filem_ftp_register_loclis*/
/*Para: sIpAdd */
/*Dest: register listen sock to ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_register_loclis(FILEM_FTP_ADDR *sIpAdd)
{
    int   retval = 0;
    union proto_reg proto;

    sIpAdd->iIsReg = 1;
    
    memset(&proto, 0, sizeof(union proto_reg));
    
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport    = sIpAdd->iPort;   //des == loc
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4)
    {
         proto.ipreg.dip = sIpAdd->iIpv4;
         
         retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else  
    {
         memcpy(&proto.ipreg.dipv6, sIpAdd->bIpv6, 16);
        
         retval = pkt_register(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_unregister_loclis*/
/*Para: sIpAdd */
/*Dest: unregister listen sock from ftm  */
/*---------------------------------------------------------*/

static int filem_ftp_unregister_loclis(FILEM_FTP_ADDR *sIpAdd)
{
    int   retval = 0;
    union proto_reg proto;

    sIpAdd->iIsReg = 0;
    
    memset(&proto, 0, sizeof(union proto_reg));
    
    proto.ipreg.protocol = IP_P_TCP;
    proto.ipreg.dport    = sIpAdd->iPort;
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4)
    {
         proto.ipreg.dip = sIpAdd->iIpv4;
         
         pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV4, &proto);
    }
    else  
    {
         memcpy(&proto.ipreg.dipv6, sIpAdd->bIpv6, 16);
        
         pkt_unregister(MODULE_ID_FILE, PROTO_TYPE_IPV6, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_connect_socket*/
/*Para: sIpAdd */
/*Dest: connect ro remote */
/*---------------------------------------------------------*/
static int filem_ftp_connect_socket(FILEM_FTP_ADDR *sIpAdd, unsigned short sPort)
{
    int   retval = 0;
	struct ip_proto proto;
        
    memset(&proto, 0, sizeof(struct ip_proto));
    
    proto.protocol = IP_P_TCP;
    proto.dport    = sIpAdd->iPort;
    proto.sport    = sPort;
    
    if(sIpAdd->iIpver == INET_FAMILY_IPV4) 
    {
         proto.type = PROTO_TYPE_IPV4;
        
         proto.dip  = sIpAdd->iIpv4;            //des = remote
         
         retval = pkt_open_tcp(MODULE_ID_FILE, &proto);
         
         zlog_debug(FILEM_DEBUG_FTP, " connect dip=%x, dport=%d, sport=%d,sip=%x, ret=%d\r\n", proto.dip, proto.dport, proto.sport,proto.sip, retval);
    }
    else //if(sIpAdd->iIpver == INET_FAMILY_IPV6)
    {
         proto.type = PROTO_TYPE_IPV6;

         memcpy(&proto.dipv6, sIpAdd->bIpv6, 16);
        
         retval = pkt_open_tcp(MODULE_ID_FILE, &proto);
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_close_socket*/
/*Para: sIpAdd */
/*Dest: close a socket */
/*---------------------------------------------------------*/
static int filem_ftp_close_socket(FILEM_FTP_SOCK *pSock)
{
    int   retval = -1;
	struct ip_proto proto;

    if(pSock->iIsCon)
    {
        zlog_debug(FILEM_DEBUG_FTP, "Local close socket sport=%d\r\n", pSock->iSport);
        
        pSock->iIsCon = 0;
        
        memset(&proto, 0, sizeof(struct ip_proto));
  
        proto.protocol = IP_P_TCP;
        proto.dport    = pSock->iDport;
        proto.sport    = pSock->iSport;  
    
        if(pSock->iIpver == INET_FAMILY_IPV4)
        {
             proto.dip = pSock->iDipv4;
             proto.sip = pSock->iSipv4;   //des = remote
             
             retval = pkt_close_tcp(MODULE_ID_FILE, &proto);
        }
        else //if(sIpAdd->iIpv4 == INET_FAMILY_IPV6)
        {
             memcpy(&proto.dipv6, pSock->bDipv6, 16);
             
             memcpy(&proto.sipv6, pSock->bSipv6, 16);
             
             retval = pkt_close_tcp(MODULE_ID_FILE, &proto);
        }
    }
    
    return(retval);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_accept_socket*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/

static int filem_ftp_accept_socket(FILEM_FTP_CTRL *pFtpCtl, 
                                   struct tcp_session *pSess, int iOptCod)
{
    int             iType = 0;
    FILEM_FTP_SOCK *pSock = NULL;

    zlog_debug(FILEM_DEBUG_FTP, "Ftp Accept socket, dip=%x dport=%d sip=%x sport=%d\r\n",
                 pSess->dip, pSess->dport, pSess->sip, pSess->sport);
                                                    //des=loc
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT)
    {
        if(pSess->dport == pFtpCtl->sLocAddr.iPort)
        {
             pSock = &pFtpCtl->sDatSock;
             iType = FILEM_FTP_RCV_FILEDATA;
        }
        
        if(pSess->dport == pFtpCtl->sCtrSock.iSport) 
        {
             pSock = &pFtpCtl->sCtrSock;
             iType = FILEM_FTP_RCV_CTRLDATA;
        }
     }

    if(pSock != NULL)
    {
        if(iOptCod == IPC_OPCODE_CONNECT)
        {
            zlog_debug(FILEM_DEBUG_FTP, "socket connect notify\r\n");

            if(!pSock->iIsCon)
            {
                pSock->iIsCon = 1;

                pSock->iIpver = INET_FAMILY_IPV4;
                
              //if(iType != FILEM_FTP_RCV_CTRLDATA)
                {
                pSock->iSport = pSess->dport;
                }
                pSock->iSipv4 = pSess->dip;

                pSock->iDport = pSess->sport;
                pSock->iDipv4 = pSess->sip;

                if(pSock->iIpver == INET_FAMILY_IPV6)
                {
                     memcpy(pSock->bSipv6, pSess->dipv6.ipv6, 16);
                     
                     memcpy(pSock->bDipv6, pSess->sipv6.ipv6, 16);
                }
            }
        }
        else
        {
            if(pSock->iIsCon)  
            {
               pSock->iIsCon = 0;
               
               if((iType == FILEM_FTP_RCV_FILEDATA) &&
                  (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
               {
                   pFtpCtl->iReqState  = FILEM_FTP_CMD_DATEND;
                   pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;

                   if((pFtpCtl->iTrsType == FILEM_FTP_UPLOAD) && 
                      (pFtpCtl->iCurrPos < pFtpCtl->iFileLen) && (pFtpCtl->iErrorCode == 0))
                   {
                        pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                   }
               }
            }
            
            zlog_debug(FILEM_DEBUG_FTP, "socket disconnect \r\n");
        }
    }
    return(iType);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_match_socket*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/

static int filem_ftp_match_socket(FILEM_FTP_CTRL *pFtpCtl, struct ip_control *iIpCon)
{
    //ipv4
    { // IPV4 
    if(pFtpCtl->sCtrSock.iIsCon)  
    {
        if((pFtpCtl->sCtrSock.iSipv4 == iIpCon->dip) &&
           (pFtpCtl->sCtrSock.iSport == iIpCon->dport))
        {
            return(FILEM_FTP_RCV_CTRLDATA);          //des = LOC
        }
    }

    if(pFtpCtl->sDatSock.iIsCon)  
    {
        if((pFtpCtl->sDatSock.iSipv4 == iIpCon->dip) &&
           (pFtpCtl->sDatSock.iSport == iIpCon->dport))
        {
             return(FILEM_FTP_RCV_FILEDATA);          
        }
    }
    }
    //{} IPV6
    return(FILEM_FTP_RCV_UNKWDATA);
}


/*---------------------------------------------------------*/
/*Name: filem_ftp_session_end*/
/*Para: sIpAdd */
/*Dest: accept a socket, comefrom remote or FTM local noti*/
/*---------------------------------------------------------*/
static void filem_ftp_session_end(FILEM_FTP_CTRL *pftpCtl)
{
    if(pftpCtl->iFileID >= 0) filem_oper_close(pftpCtl->iFileID);

    if(pftpCtl->sDatSock.iIsCon) filem_ftp_close_socket(&pftpCtl->sDatSock);

    if(pftpCtl->sCtrSock.iIsCon) filem_ftp_close_socket(&pftpCtl->sCtrSock);

    if(pftpCtl->sLocAddr.iIsReg) filem_ftp_unregister_loclis(&pftpCtl->sLocAddr);

    if(pftpCtl->iTrsType == FILEM_FTP_DNLOAD) 
    {
        if(filem_oper_isexist(pftpCtl->bLocTempN)) filem_oper_remove(pftpCtl->bLocTempN);
    }
    
    if(pftpCtl->iErrorCode == 0)
    { 
         filem_ftp_out_percent(pftpCtl, 1);

         if(pftpCtl->iTrsType == FILEM_FTP_UPLOAD)  
         {
              filem_upload_stat_set(-FILEM_ERROR_FINISHED);
    
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp upload file %s Successfully\r\n", pftpCtl->bLocFname);
         }
         else 
         {
              filem_download_stat_set(-FILEM_ERROR_FINISHED);
              
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp download file %s Successfully\r\n", pftpCtl->bRemFname);
         }
    }
    else
    { 
         if(pftpCtl->iTrsType == FILEM_FTP_UPLOAD)  
         {
              filem_upload_stat_set(pftpCtl->iErrorCode);
              
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp upload file %s fail, %s, %s, datlen=%d\r\n",
                       pftpCtl->bLocFname,  filem_sys_errget(pftpCtl->iErrorCode), pftpCtl->bSvrErrBuf, pftpCtl->iCurrPos); 
         }
         else 
         {
              filem_download_stat_set(pftpCtl->iErrorCode);
                      
              snprintf(pftpCtl->bErrInfBuf, (FILEM_FTP_ERRBUF_LEN - 1), "Ftp download file %s fail, %s, %s, datlen=%d\r\n",
                       pftpCtl->bRemFname, filem_sys_errget(pftpCtl->iErrorCode), pftpCtl->bSvrErrBuf, pftpCtl->iCurrPos); 
         }
    }

    zlog_err("%s[%d]: %s", __FILE__, __LINE__, pftpCtl->bErrInfBuf);

    if(pftpCtl->pProcCall)
    {
        pftpCtl->pProcCall(pftpCtl->iErrorCode, pftpCtl->bErrInfBuf, pftpCtl->pProParam, 1);           
    }
    
    if(pftpCtl->iFileType == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);

    filem_ftp_unregister_loccnt(&pftpCtl->sCtrSock);

    memset(pftpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    filem_oper_flush();
}


/*---------------------------------------------------------*/
/*Name:filem_ftp_start_session                             */
/*Para:ftp_ctrl                                            */
/*Desc:�������ӿ�ʼ�Ự                                            */
/*---------------------------------------------------------*/

static int filem_ftp_start_session(FILEM_FTP_CTRL *pFtpCtrl)
{
    pFtpCtrl->iReqState   = FILEM_FTP_CMD_CONN;
    pFtpCtrl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
        
    return(filem_ftp_connect_socket(&pFtpCtrl->sSvrAddr, pFtpCtrl->sCtrSock.iSport));
}

/*---------------------------------------------------------*/
/*Name:filem_ftp_clear_session                             */
/*Para:pFtpCtrl                                            */
/*Desc:����Ự����                                            */
/*---------------------------------------------------------*/
/*
static void filem_ftp_clear_session(FILEM_FTP_CTRL *pFtpCtrl)
{
    struct pkt_buffer  *pPktBuf = NULL;
    
    while(1)
    {
         if((pPktBuf = pkt_rcv(MODULE_ID_FILE)) == NULL) break;
    }
}
*/
/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_send_ctrlpkt                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ���������                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_ctrlpkt(FILEM_FTP_CTRL *pFtpCtl)
{
    union pkt_control pktcont;
    
    memset(&pktcont, 0, sizeof(union pkt_control));
    
    pktcont.ipcb.dip        = pFtpCtl->sCtrSock.iDipv4;
    pktcont.ipcb.dport      = pFtpCtl->sCtrSock.iDport;
    pktcont.ipcb.sip        = pFtpCtl->sCtrSock.iSipv4;
    pktcont.ipcb.sport      = pFtpCtl->sCtrSock.iSport;
    pktcont.ipcb.protocol   = IP_P_TCP;
    pktcont.ipcb.vpn        = pFtpCtl->iL3Vpn;
    pktcont.ipcb.ttl        = IP_TTL_DEFAULT;
    pktcont.ipcb.is_changed = 1;
    pktcont.ipcb.tos        = 3;              //des = remote
    pktcont.ipcb.chsum_enable= 1;

    return(pkt_send(PKT_TYPE_TCP, &pktcont, pFtpCtl->bSndCmdBuf, pFtpCtl->iSndCmdLen));
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_datapkt                          */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: �������ݱ���                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_datapkt(FILEM_FTP_CTRL *pFtpCtl)
{
    union pkt_control pktcont;
    
    memset(&pktcont, 0, sizeof(union pkt_control));
    
    pktcont.ipcb.dip        = pFtpCtl->sDatSock.iDipv4;
    pktcont.ipcb.dport      = pFtpCtl->sDatSock.iDport;
    pktcont.ipcb.sip        = pFtpCtl->sDatSock.iSipv4;
    pktcont.ipcb.sport      = pFtpCtl->sDatSock.iSport;
    pktcont.ipcb.protocol   = IP_P_TCP;
    pktcont.ipcb.vpn        = pFtpCtl->iL3Vpn;
    pktcont.ipcb.ttl        = IP_TTL_DEFAULT;          //des = remote
    pktcont.ipcb.is_changed = 1;
    pktcont.ipcb.tos        = 3;
    pktcont.ipcb.chsum_enable= 1;

    return(pkt_send(PKT_TYPE_TCP, &pktcont, pFtpCtl->bRSDataBuf, pFtpCtl->iSndDatLen));
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_username                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: �����û�����                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_username(FILEM_FTP_CTRL *pFtpCtl)
{
                                                /* send user name*/
    int iResult;
                                                
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "USER %s\r\n", pFtpCtl->bUserName); 

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
    
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_userpass                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ������������                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_userpass(FILEM_FTP_CTRL *pFtpCtl)
{
                                                /* send user name*/
    int iResult;
                                                
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "PASS %s\r\n", pFtpCtl->bPassWord); 

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
    
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_portcmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ���Ͷ˿�����                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_portcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int            iResult;
    unsigned int   iLocIp;
    unsigned short iPort;
    
    iLocIp = pFtpCtl->sLocAddr.iIpv4;
    iPort  = pFtpCtl->sLocAddr.iPort;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "PORT %u,%u,%u,%u,%u,%u\r\n", 
                                 ((iLocIp >> 24) & 0xff), ((iLocIp >> 16) & 0xff), 
                                 ((iLocIp >>  8) & 0xff), ((iLocIp >>  0) & 0xff),
                                 ((iPort  >>  8) & 0xff), ((iPort   >> 0) & 0xff));

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);

     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_typecmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ������������                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_typecmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*�����ļ���������*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "TYPE I\r\n");

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_sizecmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: �����ļ���������                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_sizecmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*�����ļ���������*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "SIZE %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_quitcmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ������ֹ�Ự����                                          */
/*---------------------------------------------------------*/
static int filem_ftp_send_quitcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int iResult;
                                              /*�����ļ���������*/
    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "QUIT \r\n");

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}



/*---------------------------------------------------------*/
/* Name: filem_ftp_send_retrcmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ���Ͷ�ȡ�ļ�����                                          */
/*---------------------------------------------------------*/
   
static int filem_ftp_send_retrcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int   iResult;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "RETR %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_send_storcmd                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  -1 ERROR                             */
/* Desc: ����д���ļ�����                                          */
/*---------------------------------------------------------*/

static int filem_ftp_send_storcmd(FILEM_FTP_CTRL *pFtpCtl)
{
    int   iResult;

    pFtpCtl->iSndCmdLen = sprintf(pFtpCtl->bSndCmdBuf, "STOR %s\r\n", pFtpCtl->bRemFname);

    iResult = filem_ftp_send_ctrlpkt(pFtpCtl);
     
    return(iResult);
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_check_dloadlen                         */
/* Para: pFtpCtl : FTP����ʵ�����ƿ�                       */
/* Retu: 0  Ok  !=0 ERRORcode                             */
/*---------------------------------------------------------*/

static int filem_ftp_check_dnloadlen(FILEM_FTP_CTRL *pFtpCtl, char *pLenstr)
{
     char  cPath[FILEM_NAME_MAXLN] = {0};

     if(filem_ftp_is_decnumber(pLenstr))
     {
          pFtpCtl->iFileLen = atoi(pLenstr);
    
          if(pFtpCtl->iFileLen < 0) 
          {
              pFtpCtl->iErrorCode = -FILEM_ERROR_LENGTH;
          }
     }
    
     if(pFtpCtl->iErrorCode == 0)
     {
         filem_oper_cutpath(pFtpCtl->bLocFname, cPath);
         
         if(filem_oper_availdisk(cPath) < (unsigned int)pFtpCtl->iFileLen)
         {
              pFtpCtl->iErrorCode = -FILEM_ERROR_DISKFULL;
         }
     }

     return(pFtpCtl->iErrorCode);
}


/*---------------------------------------------------------*/
/* Name: filem_ftp_send_uploadfile                         */
/* Para: pFtpCtl    �������ƿ�                             */
/* Retu: �����ϴ��ļ�                                    */
/*---------------------------------------------------------*/
static int filem_ftp_send_uploadfile(FILEM_FTP_CTRL *pFtpCtl)
{
    int iRetry = 0;
    int iDelay = 0;

    filem_oper_seek(pFtpCtl->iFileID, pFtpCtl->iCurrPos, SEEK_SET);
    
    while((pFtpCtl->sDatSock.iIsCon) &&
          (pFtpCtl->iCurrPos < pFtpCtl->iFileLen))   
    {
        pFtpCtl->iSndDatLen = FILEM_FTP_DATBUF_LEN;
        
        pFtpCtl->iSndDatLen = filem_oper_read(pFtpCtl->iFileID, pFtpCtl->bRSDataBuf, pFtpCtl->iSndDatLen);

        if(pFtpCtl->iSndDatLen < 0) 
        {
             pFtpCtl->iErrorCode = -FILEM_ERROR_READ;
             break;
        }
 
        if(filem_ftp_send_datapkt(pFtpCtl) != 0)
        {
            iRetry = 1; break;
        }

        pFtpCtl->iRetryCoun = 0;
             
        pFtpCtl->iCurrPos += pFtpCtl->iSndDatLen;

        if(++iDelay > 10) break;
    }
    
    filem_ftp_out_percent(pFtpCtl, 0);

    return(iRetry);
   
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_check_dnloadfile                        */
/* Desc: ��������ļ�                                            */
/*---------------------------------------------------------*/
static int filem_ftp_check_dnloadfile(FILEM_FTP_CTRL *pFtpCtl)
{
     int iResult = 0;
     
     iResult = filem_md5_matchcheck(pFtpCtl->bLocTempN, 0);
                        
     if(iResult == 0) 
     {
          if(filem_oper_isexist(pFtpCtl->bLocFname))
          {
              filem_oper_remove(pFtpCtl->bLocFname);
              usleep(100000);
          }
          
          filem_oper_rename(pFtpCtl->bLocTempN, pFtpCtl->bLocFname);
     }          
     else
     {
          pFtpCtl->iErrorCode  = -FILEM_ERROR_MD5CHKERR;
          
          filem_oper_remove(pFtpCtl->bLocTempN);
     }

     return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_dnloadfile                         */
/* Desc: ����ͨ���������ݴ���                                        */
/*---------------------------------------------------------*/

static void filem_ftp_recv_dnloadfile(FILEM_FTP_CTRL *pFtpCtl, char *pDatBuf, int iDatlen)
{
    if((pFtpCtl->iTrsType  == FILEM_FTP_DNLOAD) &&
       (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
    {
         int iTmpLen = 0;

         if(iDatlen > 0)
         {
            iTmpLen = filem_oper_write(pFtpCtl->iFileID, pDatBuf, iDatlen);
         }
         
         if(iTmpLen != iDatlen)  
         {
              pFtpCtl->iErrorCode  = -FILEM_ERROR_WRITE;

              pFtpCtl->iReqState   =  FILEM_FTP_CMD_DATEND;
              
              pFtpCtl->iReqTimeout =  FILEM_FTP_CMD_TIMEOUT;

              filem_ftp_close_socket(&pFtpCtl->sDatSock);  /*�ر�ͨ����ֹ����*/
#if 0
              filem_ftp_session_end(pFtpCtl);     /*�����˲������� �����ر�*/
#endif
         }
         else
         {
             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
             
             pFtpCtl->iCurrPos += iDatlen;

             filem_ftp_out_percent(pFtpCtl, 0);

             if(pFtpCtl->iCurrPos >= pFtpCtl->iFileLen)
             {
                 filem_oper_close(pFtpCtl->iFileID);

                 pFtpCtl->iFileID = -1;
                 
                 filem_ftp_close_socket(&pFtpCtl->sDatSock);

                 filem_ftp_check_dnloadfile(pFtpCtl);   /*���������, Ҫ������*/

                 if((pFtpCtl->iEarlyCode == 226) || (pFtpCtl->iEarlyCode == 250)) 
                 {                                       /*��ǰ�յ�����ɹ��ź�*/
                     filem_ftp_send_quitcmd(pFtpCtl);
                     pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                     pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                 }
                 else
                 {
                     pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                     pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                 }
             }
         }
    }
}                          

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_fill_svrackerror*/
/*Para: pftpctl */
/*Dest: output run percent infor */
/*---------------------------------------------------------*/

static void filem_ftp_fill_svrackerror(FILEM_FTP_CTRL *pftpCtl, int iCode, 
                                       char *pErrStr, int iErrcode)
{
    pftpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
    pftpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
    
    if(pftpCtl->iErrorCode == 0) pftpCtl->iErrorCode = iErrcode;

    if(!pErrStr)
         snprintf(pftpCtl->bSvrErrBuf, (FILEM_FTP_ERRBUF_LEN -1), "%d\r\n", iCode);                  
    else snprintf(pftpCtl->bSvrErrBuf, (FILEM_FTP_ERRBUF_LEN -1), "%d %s\r\n", iCode, pErrStr);
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_process_respon*/
/*Para: pftpctl */
/*Dest: process control channel reply */
/*---------------------------------------------------------*/

static void filem_ftp_process_respon(FILEM_FTP_CTRL *pFtpCtl, int icode, char *pText)
{
     switch(pFtpCtl->iReqState)
     {
         case FILEM_FTP_CMD_CONN:

              if((icode == 200) || (icode == 220))
              {
                   if(filem_ftp_send_username(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_USER;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user name command\r\n");
                   }
                   else
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                       
                       pFtpCtl->iErrorCode  = -FILEM_ERROR_CONNECTFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user name command fail\r\n");
                   }
              }
              else
              {
                  filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_CONNECTFAIL);

                  zlog_debug(FILEM_DEBUG_FTP, "FTP Connect reply fail code = %d\r\n", icode);
              }

              break;

         case FILEM_FTP_CMD_USER:

              if((icode == 331) || (icode == 332))
              {
                   if(filem_ftp_send_userpass(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_PASS;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user pass command\r\n");
                   }
                   else
                   {
              
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send user pass command fail\r\n");
                   }
              }
              else
              {              
                  filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_AUTHERROR);
                  
                  zlog_debug(FILEM_DEBUG_FTP, "FTP User command reply fail code = %d\r\n", icode);
              }
              
              break;

         case FILEM_FTP_CMD_PASS:

              if((icode == 200) || (icode == 220) || (icode == 230))
              {
                   if(filem_ftp_send_typecmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_TYPE;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send file type command\r\n");
                   }
                   else
                   {
                  
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                       
                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send file type command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_AUTHERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Password command reply fail code = %d\r\n", icode);
              }

              break;

         case FILEM_FTP_CMD_TYPE:

              if((icode == 200) || (icode == 220))
              {
                   if(filem_ftp_send_portcmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_PORT;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send port command\r\n");
                   }
                   else
                   {
                       pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                       pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                       pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send port command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Type command reply fail code = %d\r\n", icode);
              }
        
              break;

         case FILEM_FTP_CMD_PORT:

              if((icode == 200) || (icode == 220))
              {
                   if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
                   {
                        if(filem_ftp_send_sizecmd(pFtpCtl) == 0)
                        {
                            pFtpCtl->iReqState   = FILEM_FTP_CMD_SIZE;
                            pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send get file size command\r\n");
                        }
                        else
                        {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                            
                            pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                            
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send get file size command fail\r\n");
                        }
                   }
                   else
                   {
                        if(filem_ftp_send_storcmd(pFtpCtl) == 0)
                        {
                            pFtpCtl->iReqState   = FILEM_FTP_CMD_STOR;
                            pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send file STOR command\r\n");
                        }
                        else
                        {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;

                            pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
                            
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Send file STOR command fail\r\n");
                        }
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Port command reply fail code = %d\r\n", icode);
              }            

              break;
              
         case FILEM_FTP_CMD_SIZE:
            
              if((icode == 213) || (icode == 200))
              {
                   if(filem_ftp_check_dnloadlen(pFtpCtl, pText) == 0)
                   {
                       pFtpCtl->iFileID = filem_oper_open(pFtpCtl->bLocTempN, O_WRONLY | O_TRUNC | O_CREAT, 0777);

                       if(pFtpCtl->iFileID >= 0)
                       {
                            if(filem_ftp_send_retrcmd(pFtpCtl) == 0)
                            {
                                pFtpCtl->iReqState  = FILEM_FTP_CMD_RETR;
                            
                                pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                            
                                zlog_debug(FILEM_DEBUG_FTP, "FTP Send file RETR command\r\n");
                            }
                            else
                            {
                                pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                                
                                pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                                
                                pFtpCtl->iErrorCode  = -FILEM_ERROR_SENDDATAFAIL;
 
                                zlog_debug(FILEM_DEBUG_FTP, "FTP Send file RETR command fail\r\n");
                            }
                       }
                       else
                       {
                            pFtpCtl->iReqState   =  FILEM_FTP_CMD_SESEND;
                            
                            pFtpCtl->iReqTimeout =  FILEM_FTP_END_TIMEOUT;
                            
                            pFtpCtl->iErrorCode  = -FILEM_ERROR_OPEN;
                        
                            zlog_debug(FILEM_DEBUG_FTP, "FTP Open file =%s fail\r\n", pFtpCtl->bLocTempN);
                       }
                   }
                   else
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                       
                       pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Get file , file len check error %d\r\n", pFtpCtl->iFileLen);
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);

                   zlog_debug(FILEM_DEBUG_FTP, "FTP Size command reply fail code = %d\r\n", icode);
              }
              break;

         case FILEM_FTP_CMD_RETR:

              if((icode == 125) || (icode == 150))
              {
                   pFtpCtl->iReqState   = FILEM_FTP_CMD_DATTRS;
                   pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File option command reply code %d\r\n", icode);
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File option command reply fail code = %d\r\n", icode);
              }            
              
              break;
              
         case FILEM_FTP_CMD_STOR:

              if((icode == 125) || (icode == 150))
              {
                   pFtpCtl->iFileID = filem_oper_open(pFtpCtl->bLocFname, O_RDONLY, 0);

                   if(pFtpCtl->iFileID < 0)
                   {            
                       filem_ftp_close_socket(&pFtpCtl->sDatSock);
                       
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
               
                       pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Upload open file %s fail\r\n", pFtpCtl->bLocFname);
                   }
                   else
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_DATTRS;

                       pFtpCtl->iReqTimeout = 0; //FILEM_FTP_RTY_TIMEOUT * 2;
                   
                       pFtpCtl->iFileLen = filem_oper_size_getf(pFtpCtl->iFileID);

                       zlog_debug(FILEM_DEBUG_FTP, "FTP Upload file state=%d, timeout=%d, datalen=%d\r\n", pFtpCtl->iReqState, pFtpCtl->iReqTimeout, pFtpCtl->iFileLen);

                       ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
               
                   zlog_debug(FILEM_DEBUG_FTP, "FTP File stor command reply fail code = %d\r\n", icode);
              }            
              
              break;

         case FILEM_FTP_CMD_DATTRS:
              /*wait send data over*/

              if((icode == 226) || (icode == 250))
              {
                   pFtpCtl->iEarlyCode = icode;
              }
              zlog_debug(FILEM_DEBUG_FTP, "FTP receive data trans end, in trans data state code = %d\r\n", icode);
              break;

         case FILEM_FTP_CMD_DATEND:

              if((icode == 226) || (icode == 250))
              {
                   if(filem_ftp_send_quitcmd(pFtpCtl) == 0)
                   {
                       pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                       pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                   
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command \r\n");
                   }
                   else
                   {
                       if(pFtpCtl->iErrorCode == 0)
                       {
                           pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                       }
                       
                       filem_ftp_session_end(pFtpCtl);
                       
                       zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command fail\r\n");
                   }
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
               
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Data transfer fail code = %d\r\n", icode);
              }            
              break;     

         case FILEM_FTP_CMD_SESEND:
            
              /*do nothing,  wait timeout and quit session*/
              break;

         case FILEM_FTP_CMD_QUIT:

              if((icode == 221) || (icode == 220) || (icode == 250))
              {
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Quit reply ok\r\n");
              }
              else
              {
                   filem_ftp_fill_svrackerror(pFtpCtl, icode, pText, -FILEM_ERROR_PKTACKERROR);
                   
                   zlog_debug(FILEM_DEBUG_FTP, "FTP Quit reply fail code = %d\r\n", icode);
              }            

              filem_ftp_session_end(pFtpCtl);
              
              break;              
        
         default: 
         
              zlog_debug(FILEM_DEBUG_FTP, "FTP Ftp client no care command or reply state=%d, code = %d\r\n",  pFtpCtl->iReqState, icode);
              break;
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/*Name: filem_ftp_get_cmdline*/
/*Para: pFtpCtl */
/*Dest: get one line command  */
/*---------------------------------------------------------*/
static int filem_ftp_get_cmdline(FILEM_FTP_CTRL *pFtpCtl)
{
    int    iLeftOf =  0;
    int    iLeftEn =  0;
    int    iLinCou =  0;   
    int    iLineMx = FILEM_FTP_CMDBUF_LEN - 8;
    int    iLeftLn = pFtpCtl->iRecCmdLen;
    char  *pDatPos = pFtpCtl->bRecCmdBuf;

    pFtpCtl->bOneCmdBuf[0] = '\0';
    
    while(iLeftLn > 0)
    {
        if((*pDatPos == '\r') || (*pDatPos == '\n') || (*pDatPos == ' '))
        {
             pDatPos++; iLeftLn--; iLeftOf++; iLeftEn++;
        }
        else break;
    }

    if(iLeftLn <= 0)
    {
        pFtpCtl->iRecCmdLen = 0; return(0);
    }
    else
    {
        while(iLeftLn > 0) 
        {
            if((*pDatPos == '\r') || (*pDatPos == '\n')) break;
             
            iLeftEn ++;  pDatPos ++; iLeftLn--;
        }

        iLinCou = iLeftEn - iLeftOf;

        if(iLinCou > 0) 
        {
            if(iLinCou > iLineMx) iLinCou = iLineMx;
            
            memcpy(pFtpCtl->bOneCmdBuf, pFtpCtl->bRecCmdBuf + iLeftOf, iLinCou);

            pFtpCtl->bOneCmdBuf[iLinCou] = '\0';
        }
        
        while(iLeftLn > 0)
        {
            if((*pDatPos == '\r') || (*pDatPos == '\n') || (*pDatPos == ' '))
            {
                 pDatPos++; iLeftLn--; iLeftEn++;
            }
            else break;
        }

        if(iLeftLn > 0) memmove(pFtpCtl->bRecCmdBuf, pFtpCtl->bRecCmdBuf+iLeftEn, iLeftLn);
        
        pFtpCtl->iRecCmdLen = iLeftLn;

        return(iLinCou);
    }
}

/*---------------------------------------------------------*/
/*Name: filem_ftp_parse_replymsg*/
/*Para: pFtpCtl */
/*Para: pCmdTxt return reply code's explain text */
/*Dest: pase one line reply message  */
/*---------------------------------------------------------*/
static int filem_ftp_parse_replymsg(FILEM_FTP_CTRL *pFtpCtl, char **pCmdTxt)
{
     int   iCode = -1;
     int   iCoun =  0;
     char *pText = NULL;

     iCoun = filem_ftp_get_cmdline(pFtpCtl);

     zlog_debug(FILEM_DEBUG_FTP, "FTP Reveive reply line = %s\r\n", pFtpCtl->bOneCmdBuf);

     if(iCoun > 0)
     {
         pText = pFtpCtl->bOneCmdBuf;
         
         while(iCoun > 0)
         {
             if(*pText == ' ') 
             {
                *pText = '\0'; pText++;
                 break;
             }
             
             iCoun --; pText++;
         }

         *pCmdTxt = pText;
         
         if(filem_ftp_is_decnumber(pFtpCtl->bOneCmdBuf)) 
         {
             iCode = atoi(pFtpCtl->bOneCmdBuf);
         }
         
         zlog_debug(FILEM_DEBUG_FTP, "FTP Parse reveive reply code = %d, text = %s\r\n", iCode, pText);
     }

     return(iCode);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ctrldata                           */
/* Desc: ���տ���ͨ�����ݴ���                                        */
/*---------------------------------------------------------*/

static void filem_ftp_recv_ctrlpacket(FILEM_FTP_CTRL *pFtpCtl, char *pDatBuf, int iDatlen)
{
    int   iTmpLen = 0;
    int   iRetCod = 0;
    char *pPadTxt = NULL;
    
    while(iDatlen > 0)
    {
         iTmpLen = FILEM_FTP_CMDBUF_LEN - pFtpCtl->iRecCmdLen;

         if(iTmpLen > iDatlen) iTmpLen = iDatlen;

         memcpy(pFtpCtl->bRecCmdBuf + pFtpCtl->iRecCmdLen, pDatBuf, iTmpLen);

         iDatlen -= iTmpLen; pDatBuf += iTmpLen;
         
         pFtpCtl->iRecCmdLen += iTmpLen;

         iRetCod = filem_ftp_parse_replymsg(pFtpCtl, &pPadTxt);

         if(iRetCod >= 0)
         {
              filem_ftp_process_respon(pFtpCtl, iRetCod, pPadTxt);
         }
         else
         {
              zlog_debug(FILEM_DEBUG_FTP, "FTP Control data receive, cmd line error or no right reply code\r\n");
         }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ctrldata                           */
/* Desc: ���տ���ͨ�����ݴ���                                        */
/*---------------------------------------------------------*/

void filem_ftp_send_notify(void)
{
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;

    if((pFtpCtl->iTrsType == FILEM_FTP_UPLOAD) &&
       (pFtpCtl->iReqState == FILEM_FTP_CMD_DATTRS))
    {
        int iRetry = 0;
        
        iRetry = filem_ftp_send_uploadfile(pFtpCtl);

        if((pFtpCtl->iCurrPos >= pFtpCtl->iFileLen) || (pFtpCtl->iErrorCode != 0))
        {
             zlog_debug(FILEM_DEBUG_FTP, "FTP Upload file end errcode=%d, filelen=%d\r\n", pFtpCtl->iErrorCode, pFtpCtl->iFileLen);
            
             filem_oper_close(pFtpCtl->iFileID);
                                 
             pFtpCtl->iFileID = -1;

             pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
             
             usleep(200000);
             
             filem_ftp_close_socket(&pFtpCtl->sDatSock);
        }
        else
        {
             if(iRetry) pFtpCtl->iReqTimeout = FILEM_FTP_RTY_TIMEOUT;
             else
             {
                 usleep(10000);

                 ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
             }
        }
    }

}


/*---------------------------------------------------------*/
/* Name: filem_ftp_recv_ipcpacket                          */
/* Desc: ����SOCKET���� �¼���ѯ����                                 */
/*---------------------------------------------------------*/
void filem_ftp_recv_ipcpacket(struct ipc_mesg_n *pMsg, int revln)
{
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;/*find session*/
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        int                iDatTyp = 0;

        struct pkt_buffer *pPktBuf = (struct pkt_buffer *)pMsg->msg_data;
        
        pPktBuf->data = pMsg->msg_data + pPktBuf->data_offset;

        iDatTyp = filem_ftp_match_socket(pFtpCtl, &pPktBuf->cb.ipcb);

        if(pPktBuf->data_len == 0)
        {
            zlog_debug(FILEM_DEBUG_FTP, "FTP receive packet, data len = 0\r\n");
        }
        switch(iDatTyp)
        {
            case FILEM_FTP_RCV_CTRLDATA:
                 filem_ftp_recv_ctrlpacket(pFtpCtl, pPktBuf->data, (int)pPktBuf->data_len);
                 break;
            
            case FILEM_FTP_RCV_FILEDATA:
                 filem_ftp_recv_dnloadfile(pFtpCtl, pPktBuf->data, (int)pPktBuf->data_len);
                 break;
            
            default:
                 zlog_debug(FILEM_DEBUG_FTP, "FTP receive packet, not ctrl, not data, no match socket\r\n");
                 break;
        }
    }
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_connect_notimsg                         */
/* Para: bSesDat struct tcp_session                        */
/* Desc: �յ�FTM��TCP���ӳɹ�֪ͨ��Ϣ����                               */
/*---------------------------------------------------------*/
void filem_ftp_connect_notimsg(char *bSesDat, int iDatLen, int iOptCod)
{
     if(iDatLen >= (int)sizeof(struct tcp_session))
     {
          int iSockType = 0;
          
          FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl; // find ftp control is right
          
          iSockType = filem_ftp_accept_socket(pFtpCtl, (struct tcp_session*)bSesDat, iOptCod);

          zlog_debug(FILEM_DEBUG_FTP, "FTP ftp accept connect type=%d, opcode=%d\r\n", iSockType, iOptCod);

          /*control channel connect ok*/
          
          if((iSockType == FILEM_FTP_RCV_CTRLDATA) && (iOptCod == IPC_OPCODE_CONNECT))
          {
              pFtpCtl->iReqState   = FILEM_FTP_CMD_CONN;
              pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;

                                                                  /*����ͨ�����ӳɹ���, ������ȷ��д����������ַ*/
              pFtpCtl->sLocAddr.iIsReg = 0;
              pFtpCtl->sLocAddr.iIpver = pFtpCtl->sCtrSock.iIpver;
                                                                  /*�˿ں��Ѿ����ú�*/
            //pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT;  
              pFtpCtl->sLocAddr.iIpv4  = pFtpCtl->sCtrSock.iSipv4;

              if(pFtpCtl->sLocAddr.iIpver == INET_FAMILY_IPV6)
              {
                  memcpy(pFtpCtl->sLocAddr.bIpv6, pFtpCtl->sCtrSock.bSipv6, 16);
              }            

              filem_ftp_register_loclis(&pFtpCtl->sLocAddr);           
#if 0
              thread_add_event(filemmaster, filem_ftp_recv_ipcpacket, NULL, 0);   /*�Ự�ر�ʱ���Զ�ɾ��*/
#endif
          }
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_tftp_session_timeout                        */
/* Desc: �Ự��ʱ����                                            */
/*---------------------------------------------------------*/
void filem_ftp_session_timeout(void)
{
     FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;      /*�ò���������, Ϊ֧�ֶ�SESSION���ӿ�*/

     if((pFtpCtl->iTrsType != FILEM_FTP_NOACT) && (pFtpCtl->iReqTimeout > 0))
     {
          if(--pFtpCtl->iReqTimeout == 0)
          {         
               zlog_debug(FILEM_DEBUG_FTP, "FTP Timeout state=%d\r\n", pFtpCtl->iReqState);

               switch(pFtpCtl->iReqState)
               {
                    case FILEM_FTP_CMD_CONN:
                         
                         if(pFtpCtl->iErrorCode == 0)
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_CONNECTFAIL;
                         }
                         filem_ftp_session_end(pFtpCtl);

                         break;

                    case FILEM_FTP_CMD_USER:
                    case FILEM_FTP_CMD_PASS:

                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_AUTHERROR;
                         }
                         filem_ftp_session_end(pFtpCtl);
                         
                         break;
                    
                    case FILEM_FTP_CMD_TYPE:
                    case FILEM_FTP_CMD_PORT:
                    case FILEM_FTP_CMD_SIZE:
                    case FILEM_FTP_CMD_RETR:
                    case FILEM_FTP_CMD_STOR:

                         pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                         pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;

                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         break;
                    
                    case FILEM_FTP_CMD_DATTRS:

                         if(pFtpCtl->iTrsType == FILEM_FTP_UPLOAD)
                         {
                              pFtpCtl->iRetryCoun ++;

                              if(pFtpCtl->iRetryCoun > FILEM_FTP_RTY_REPEAT)
                              {
                                  filem_ftp_close_socket(&pFtpCtl->sDatSock);
                                  
                                  pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                                  pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                                  
                                  pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                              }
                              else
                              {
                                  ipc_send_msg_n2(NULL, 0, 0, MODULE_ID_FILE,  MODULE_ID_FILE, IPC_TYPE_TRIGER, 0, IPC_OPCODE_EVENT, 0);
                              }
                         }
                         else
                         {
                              filem_ftp_close_socket(&pFtpCtl->sDatSock);
                              
                              pFtpCtl->iReqState   = FILEM_FTP_CMD_DATEND;
                              pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
 
                              if(pFtpCtl->iErrorCode == 0) 
                              {
                                  pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                              }
                         }
                         break;          

                    case FILEM_FTP_CMD_DATEND:
                             
                         if(pFtpCtl->iFileID >= 0)
                         {
                             filem_oper_close(pFtpCtl->iFileID);
                         
                             pFtpCtl->iFileID = -1;
                         }
                          
                         pFtpCtl->iReqState   = FILEM_FTP_CMD_SESEND;
                         pFtpCtl->iReqTimeout = FILEM_FTP_END_TIMEOUT;
                                                  
                         if(pFtpCtl->iErrorCode == 0)
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         break;                                  

                    case FILEM_FTP_CMD_SESEND:    /*�����쳣��ֹ*/
                         
                         if(filem_ftp_send_quitcmd(pFtpCtl) == 0)
                         {
                             pFtpCtl->iReqState   = FILEM_FTP_CMD_QUIT;
                           
                             pFtpCtl->iReqTimeout = FILEM_FTP_CMD_TIMEOUT;
                           
                             zlog_debug(FILEM_DEBUG_FTP, "FTP Send quit command \r\n");
                         }
                         else
                         {
                             if(pFtpCtl->iErrorCode == 0) 
                             {
                                 pFtpCtl->iErrorCode = -FILEM_ERROR_SENDDATAFAIL;
                             }
                             filem_ftp_session_end(pFtpCtl);
                         }

                         break;
                    
                    case FILEM_FTP_CMD_QUIT:
                         
                         if(pFtpCtl->iErrorCode == 0) 
                         {
                             pFtpCtl->iErrorCode = -FILEM_ERROR_RECVTIMEOUT;
                         }
                         
                         filem_ftp_session_end(pFtpCtl);
                         
                         break;
                                              
                    default: break;
               }
          }
     }
}

/*====================================================================================================================*/
/*====================================================================================================================*/
/*---------------------------------------------------------*/
/* Name: filem_ftp_downloadfile                             */
/* Para: pSvrAddr   SERVERIP                               */
/* Para: pUser      ��¼�û���                             */
/* Para: pPaswd     ��¼�û�����                           */
/* Para: pLocName   �ļ���(��·��)                         */
/* Para: pRemName  �ļ���(��·��)                          */
/* Para: iSvrPort  svr�˿ں�                          */
/* Retu: �ɹ�����0, ʧ�ܷ��ش������                  */
/*---------------------------------------------------------*/
int filem_ftp_uploadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                         char  *pLocName, char *pRemName, 
                          int   iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int             iFileTyp = 0;
    int             iIpver   = INET_FAMILY_IPV4;
    int             iResult  = 0;
    
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp download is running, please retry after a moment.\r\n"); 
        }
        else 
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal) 
        {
            pProcCal(0, pFtpCtl->bErrInfBuf, pProcPar, 0); 
        }
        else
        {
            zlog_err("%s[%d]: %s", __FILE__, __LINE__, pFtpCtl->bErrInfBuf);
        }
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pRemName == NULL) pRemName = pLocName;
        
    if(strlen(pLocName) >= (FILEM_NAME_MAXLN - 8)) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }

    if(strchr(pSvrAddr,':') != NULL) iIpver = INET_FAMILY_IPV6;
            
    memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    pFtpCtl->iTrsType  =  FILEM_FTP_UPLOAD;
    pFtpCtl->iFileType =  iFileTyp;
    pFtpCtl->iFileID   = -1;
    pFtpCtl->pProcCall =  pProcCal;
    pFtpCtl->pProParam =  pProcPar;

    pFtpCtl->iL3Vpn    = (unsigned short)filem_tftp_l3vpn_get();

    pFtpCtl->sSvrAddr.iIsReg = 0;
    pFtpCtl->sSvrAddr.iIpver = iIpver;
    pFtpCtl->sSvrAddr.iPort  = (iSvrPort == 0) ? FILEM_FTP_SVR_LISPORT : iSvrPort;;
    pFtpCtl->sSvrAddr.iIpv4  = 0;
    
    pFtpCtl->sCtrSock.iSport = FILEM_FTP_CLI_CONPORT + iFtpCtrPortIdx % 100;
    pFtpCtl->sCtrSock.iIpver = iIpver;
    
    pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT + iFtpCtrPortIdx % 100;

    if(iIpver == INET_FAMILY_IPV4)
          pFtpCtl->sSvrAddr.iIpv4 = inet_strtoipv4(pSvrAddr);
    else  inet_strtoipv6(pSvrAddr, (struct ipv6_addr *)pFtpCtl->sSvrAddr.bIpv6);
    
    strncpy(pFtpCtl->sSvrAddr.bAddStr, pSvrAddr, FILEM_NAME_MAXLN -8);

    strncpy(pFtpCtl->bUserName, pUser,    FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bPassWord, pPaswd,   FILEM_NAME_MAXLN - 8);
     
    strncpy(pFtpCtl->bRemFname, pRemName, FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bLocFname, pLocName, FILEM_NAME_MAXLN - 8);
   
  //filem_ftp_clear_session(pFtpCtl);

    filem_ftp_register_loccnt(&pFtpCtl->sCtrSock);
    
    iResult = filem_ftp_start_session(pFtpCtl);

    if(iResult != 0)
    {
        iResult = -FILEM_ERROR_CONNECTFAIL;

        filem_ftp_unregister_loccnt(&pFtpCtl->sCtrSock);
        
        memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
    
    zlog_debug(FILEM_DEBUG_FTP, "FTP Connect to server %s and wait. locname= %s remname = %s.\r\n", pSvrAddr, pLocName, pRemName);

  //after start session , wait ftm connect notify
    
    iFtpCtrPortIdx ++;
  
    return(iResult);
}

/*---------------------------------------------------------*/
/* Name: filem_ftp_downloadfile                             */
/* Para: pSvrAddr   SERVERIP                               */
/* Para: pUser      ��¼�û���                             */
/* Para: pPaswd     ��¼�û�����                           */
/* Para: pLocName   �ļ���(��·��)                         */
/* Para: pRemName  �ļ���(��·��)                          */
/* Para: iSvrPort  svr�˿ں�                          */
/* Retu: �ɹ�����0, ʧ�ܷ��ش������                  */
/*---------------------------------------------------------*/
int filem_ftp_downloadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                           char  *pLocName, char *pRemName, 
                           int   iSvrPort, FilemCall pProcCal, void *pProcPar)
{
    int             iFileTyp = 0;
    int             iIpver   = INET_FAMILY_IPV4;
    int             iResult  = 0;
    
    FILEM_FTP_CTRL *pFtpCtl = &sFtpSessCtl;
    
    if(pFtpCtl->iTrsType != FILEM_FTP_NOACT) 
    {
        if(pFtpCtl->iTrsType == FILEM_FTP_DNLOAD)
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp download is running, please retry after a moment.\r\n"); 
        }
        else 
        {
            snprintf(pFtpCtl->bErrInfBuf, 120, "Ftp upload is running, please retry after a moment.\r\n"); 
        }

        if(pProcCal) 
        {
            pProcCal(0, pFtpCtl->bErrInfBuf, pProcPar, 0); 
        }
        else
        {
            zlog_err("%s[%d]: %s", __FILE__, __LINE__, pFtpCtl->bErrInfBuf);
        }
        
        return(-FILEM_ERROR_CHANBUSY);
    }

    if(pLocName == NULL) pLocName = pRemName;
        
    if(strlen(pLocName) >= (FILEM_NAME_MAXLN - 8)) return(-FILEM_ERROR_NAME); 

    iFileTyp = filem_name_totype(pLocName);
    
    if(iFileTyp == FILEM_MTYPE_VER)   
    {
        if(filem_action_flagchk(FILEM_MTYPE_VER, FILEM_OPTACT_ALL))
             return(-FILEM_ERROR_CONFLICT);
        else filem_action_flagset(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }

    if(strchr(pSvrAddr,':') != NULL) iIpver = INET_FAMILY_IPV6;
            
    memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));
    
    pFtpCtl->iTrsType  =  FILEM_FTP_DNLOAD;
    pFtpCtl->iFileType =  iFileTyp;
    pFtpCtl->iFileID   = -1;
    pFtpCtl->pProcCall =  pProcCal;
    pFtpCtl->pProParam =  pProcPar;

    pFtpCtl->iL3Vpn    = (unsigned short)filem_tftp_l3vpn_get();

    pFtpCtl->sSvrAddr.iIsReg = 0;
    pFtpCtl->sSvrAddr.iIpver = iIpver;
    pFtpCtl->sSvrAddr.iPort  = (iSvrPort == 0) ? FILEM_FTP_SVR_LISPORT : iSvrPort;;
    pFtpCtl->sSvrAddr.iIpv4  = 0;
    
    pFtpCtl->sCtrSock.iSport = FILEM_FTP_CLI_CONPORT + iFtpCtrPortIdx % 100;
    pFtpCtl->sCtrSock.iIpver = iIpver;

    pFtpCtl->sLocAddr.iPort  = FILEM_FTP_CLI_LISPORT + iFtpCtrPortIdx % 100;
      
    if(iIpver == INET_FAMILY_IPV4)
          pFtpCtl->sSvrAddr.iIpv4 = inet_strtoipv4(pSvrAddr);
    else  inet_strtoipv6(pSvrAddr, (struct ipv6_addr *)pFtpCtl->sSvrAddr.bIpv6);
    
    strncpy(pFtpCtl->sSvrAddr.bAddStr, pSvrAddr, FILEM_NAME_MAXLN -8);

    strncpy(pFtpCtl->bUserName, pUser,    FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bPassWord, pPaswd,   FILEM_NAME_MAXLN - 8);
     
    strncpy(pFtpCtl->bRemFname, pRemName, FILEM_NAME_MAXLN - 8);

    strncpy(pFtpCtl->bLocFname, pLocName, FILEM_NAME_MAXLN - 8);

    sprintf(pFtpCtl->bLocTempN, "%s.tmp", pFtpCtl->bLocFname);
   
  //filem_ftp_clear_session(pFtpCtl);
    
    filem_ftp_register_loccnt(&pFtpCtl->sCtrSock);
    
    iResult = filem_ftp_start_session(pFtpCtl);

    if(iResult != 0)
    {
        iResult = -FILEM_ERROR_CONNECTFAIL;
        
        filem_ftp_unregister_loccnt(&pFtpCtl->sCtrSock);

        memset(pFtpCtl, 0, sizeof(FILEM_FTP_CTRL));

        if(iFileTyp == FILEM_MTYPE_VER) filem_action_flagclr(FILEM_MTYPE_VER, FILEM_OPTACT_DLOAD);
    }
    
    zlog_debug(FILEM_DEBUG_FTP, "FTP Connect to server %s and wait. locname= %s remname = %s.\r\n", pSvrAddr, pLocName, pRemName);

  //after start session , wait ftm connect notify
    
    iFtpCtrPortIdx ++;
    
    return(iResult);
}

int filem_ftp_isruning(void)
{
    return(sFtpSessCtl.iTrsType != FILEM_FTP_NOACT) ;
}
/*====================================================================================================================*/
/*=====================================================================================================================*/
