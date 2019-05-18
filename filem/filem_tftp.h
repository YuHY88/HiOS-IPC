#ifndef _FILEM_TFTP_H_

#define _FILEM_TFTP_H_


/* ·þÎñÆ÷¶Ë¿Ú */
//#define FILEM_TFTP_SERVERPORT       69

void filem_tftp_session_timeout(void);

int  filem_tftp_uploadfile(char *pIpAddr,  char *pLocName, char *pRemName, 
                          int iSvrPort, FilemCall pProcCall, void *pProcPar);

int  filem_tftp_dnloadfile(char *pIpAddr,  char  *pLocName, char *pRemName, 
                          int iSvrPort, FilemCall pProcCall, void *pProcPar);

int filem_tftp_ipcpkt_receive(struct ipc_mesg_n *pMsg, int revln);

void filem_ftp_session_timeout(void);

int  filem_ftp_uploadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                          char  *pLocName, char *pRemName, 
                          int   iSvrPort, FilemCall pProcCal, void *pProcPar);


int  filem_ftp_downloadfile(char  *pSvrAddr, char *pUser,  char *pPaswd,    
                            char  *pLocName, char *pRemName, 
                            int   iSvrPort, FilemCall pProcCal, void *pProcPar);

void filem_ftp_connect_notimsg(char *bSesDat, int iDatLen, int iOptCod);

int filem_ftp_isruning(void);

void filem_ftp_recv_ipcpacket(struct ipc_mesg_n *pMsg, int revln);

void filem_ftp_send_notify(void);

#endif


