/*
 *  define IPC message queue operation
 */

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <linux/sysctl.h>
#include "log.h"
#include "zassert.h"
#include "module_id.h"
#include "msg_ipc.h"
#include "msg_ipc_n.h"
#include "memshare.h"

/*====================================================================================================================*/

#define IPC_QUEUE_ACCESS   0666 //0755

#define IPC_QUEUE_OFFSET   1000

#define IPC_RECEIVE_RETRY  2000 
                           /*等待6秒*/

/*====================================================================================================================*/

typedef struct 
{
    int module;

    int mapmod;
    
    int queuse;             /*标志该模块是否有队列*/

    int msgmax;

    int openfd;

    int recvfd;

    int sendfd;
    
}ipc_queue_table;

/*====================================================================================================================*/


/*数组下标必须和模块号一致*/
ipc_queue_table ipc_queid_list[MODULE_ID_MAXNUM] =
{
    {0,                         0,                         0,    0, -1, -1, -1},      //   0 
    {MODULE_ID_IFM,             MODULE_ID_IFM,             1,  256, -1, -1, -1},      //   1 
    {MODULE_ID_DEVM,            MODULE_ID_DEVM,            1,  512, -1, -1, -1},      //   2 
    {MODULE_ID_VTY,             MODULE_ID_VTY,             1,  256, -1, -1, -1},      //   3 
    {MODULE_ID_MPLS,            MODULE_ID_MPLS,            1, 1024, -1, -1, -1},      //   4 
    {MODULE_ID_FTM,             MODULE_ID_FTM,             1, 1024, -1, -1, -1},      //   5 
    {MODULE_ID_HAL,             MODULE_ID_HAL,             1, 1024, -1, -1, -1},      //   6 
    {MODULE_ID_HSL,             MODULE_ID_HSL,             1, 1024, -1, -1, -1},      //   7 
    {MODULE_ID_QOS,             MODULE_ID_QOS,             1, 1024, -1, -1, -1},      //   8 
    {MODULE_ID_OSPF,            MODULE_ID_OSPF,            1, 1024, -1, -1, -1},      //   9 
    {MODULE_ID_ISIS,            MODULE_ID_ISIS,            1, 1024, -1, -1, -1},      //  10 

    {MODULE_ID_RIP,             MODULE_ID_RIP,             1,  512, -1, -1, -1},       //  11 
    {MODULE_ID_BGP,             MODULE_ID_BGP,             1,  512, -1, -1, -1},       //  12 
    {MODULE_ID_ROUTE,           MODULE_ID_ROUTE,           1,  256, -1, -1, -1},       //  13 
    {MODULE_ID_ARP,             MODULE_ID_FTM,             0,    0, -1, -1, -1},       //  14 
    {MODULE_ID_LDP,             MODULE_ID_MPLS,            0,    0, -1, -1, -1},       //  15 
    {MODULE_ID_RSVPTE,          MODULE_ID_RSVPTE,          1,  256, -1, -1, -1},       //  16 
    {MODULE_ID_VRRP,            MODULE_ID_VRRP,            1,  256, -1, -1, -1},       //  17 
    {MODULE_ID_DHCP,            MODULE_ID_DHCP,            1,  256, -1, -1, -1},       //  18 
    {MODULE_ID_LACP,            MODULE_ID_LACP,            1,  256, -1, -1, -1},       //  19 
    {MODULE_ID_L2,              MODULE_ID_L2,              1,  256, -1, -1, -1},       //  20 

    {MODULE_ID_SYSLOG,          MODULE_ID_SYSLOG,          1, 1024, -1, -1, -1},       //  21 
    {MODULE_ID_FILE,            MODULE_ID_FILE,            1,  256, -1, -1, -1},       //  22 
    {MODULE_ID_PING,            MODULE_ID_PING,            1,  256, -1, -1, -1},       //  23 
    {MODULE_ID_CES,             MODULE_ID_CES,             1,  256, -1, -1, -1},       //  24 
    {MODULE_ID_AAA,             MODULE_ID_AAA,             1,  256, -1, -1, -1},       //  25 
    {MODULE_ID_SYSTEM,          MODULE_ID_SYSTEM,          1,  128, -1, -1, -1},       //  26 
    {MODULE_ID_BFD,             MODULE_ID_BFD,             1,  512, -1, -1, -1},       //  27 
    {MODULE_ID_ALARM,           MODULE_ID_ALARM,           1,  512, -1, -1, -1},       //  28 
    {MODULE_ID_NTP,             MODULE_ID_NTP,             1,  128, -1, -1, -1},       //  29 
    {MODULE_ID_SNMPD,           MODULE_ID_SNMPD,           1,  256, -1, -1, -1},       //  30 

    {MODULE_ID_SDHMGT,          MODULE_ID_SDHMGT,          1,  256, -1, -1, -1},       //  31 
    {MODULE_ID_RESV1,           MODULE_ID_RESV1,           1,  256, -1, -1, -1},       //  32 
    {MODULE_ID_WEB,             MODULE_ID_WEB,             1,   64, -1, -1, -1},       //  33 
    {MODULE_ID_OPENFLOW,        MODULE_ID_OPENFLOW,        1,   64, -1, -1, -1},       //  34 
    {MODULE_ID_NETCONF,         MODULE_ID_NETCONF,         1,   64, -1, -1, -1},       //  35 
    {MODULE_ID_IPMC,            MODULE_ID_IPMC,            1,   64, -1, -1, -1},       //  36 
    {MODULE_ID_CLOCK,           MODULE_ID_CLOCK,           1,  256, -1, -1, -1},       //  37 
    {MODULE_ID_HA,              MODULE_ID_HA,              1,  256, -1, -1, -1},       //  38 
    {MODULE_ID_OSPF6,           MODULE_ID_OSPF6,           1,   64, -1, -1, -1},       //  39 
    {MODULE_ID_DHCPV6,          MODULE_ID_DHCPV6,          1,   64, -1, -1, -1},       //  40 

    {MODULE_ID_RMON,            MODULE_ID_RMON,            1,  256, -1, -1, -1},       //  41 

    {MODULE_ID_REPLY_A,         MODULE_ID_REPLY_A,         1,  128, -1, -1, -1},       //  42 
                                                                                      /*应答队列不允许映射*/                                 
    {MODULE_ID_STAT,            MODULE_ID_STAT,            1,  256, -1, -1, -1},       // 43

    {MODULE_ID_VCG,             MODULE_ID_VCG,             1,  256, -1, -1, -1},       //  44 
    {MODULE_ID_L3VPN,           MODULE_ID_MPLS,            0,    0, -1, -1, -1},       //  45 
    {MODULE_ID_SLSP,            MODULE_ID_MPLS,            0,    0, -1, -1, -1},       //  46 
    {MODULE_ID_L2VPN,           MODULE_ID_MPLS,            0,    0, -1, -1, -1},       //  47 
    {MODULE_ID_HALHA,           MODULE_ID_HALHA,           0,    0, -1, -1, -1},       //  48 
    {MODULE_ID_SNMPD_TRAP,      MODULE_ID_SNMPD_TRAP,      1,  256, -1, -1, -1},       //  49 
    {MODULE_ID_SNMPD_PACKET,    MODULE_ID_SNMPD_PACKET,    1,  256, -1, -1, -1},       //  50 
};                  

/*
#define MODULE_ID_VCG        44

#define MODULE_ID_L3VPN      45
#define MODULE_ID_SLSP       46
#define MODULE_ID_L2VPN      47

#define MODULE_ID_HALHA      48

#define MODULE_ID_SNMPD_TRAP 49

#define MODULE_ID_MAXNUM     50
 */  
/*====================================================================================================================*/
/* **********************************************************
 * Function: ipc_create
 * Return: > 0: message queue id
 *         < 0: error
 ***********************************************************/
static int ipc_create_n(int module, int queue_len)
{
    int  queid =  0;
    int  retva = -1;
    struct msqid_ds queds;

    queid = msgget(module + IPC_QUEUE_OFFSET, IPC_CREAT|IPC_QUEUE_ACCESS);

    if(queid < 0)
    {
        printf("Create ipc msg queue failed, module = %d\n", module);
        return(retva);
    }

    memset(&queds, 0, sizeof(struct msqid_ds));
                                              /* 获取队列参数 */
    retva = msgctl(queid, IPC_STAT, &queds);
                                              
    if(retva == -1)
    {
        printf("Get ipc msg queue parameter failed, module = %d\n", module);
        return(retva);
    }
                                              /* 设置队列参数 */
    queds.msg_qnum   = queue_len;             /* 队列长度 */
    queds.msg_qbytes = queue_len * (sizeof(struct ipc_quemsg_n) - sizeof(int)); 
                                              /* 队列的字节数 */
    retva = msgctl(queid, IPC_SET, &queds);

    if(retva == -1)
    {
        printf("Set ipc msg queue parameter failed, module = %d\n", module);
        return(retva);
    }
      
  //printf("Create message queue successful, module = %d, queid =%d \n", module, queid);
      
    return(queid);
}


/* **********************************************************
 * Function: ipc_close_n
 *    key: the message module id
 * Output:
 * Return:  0: ok
 *        < 0: error
 ************************************************************/
static int ipc_close_n(int module)
{
    int  queid = 0;

    queid = msgget(module + IPC_QUEUE_OFFSET, 0);

    if(queid < 0)
    {
        printf("ipc_close error, module = %d!\n", module);
        return(-1);
    }
    else
    {   
        if(msgctl(queid, IPC_RMID, NULL) != 0)
        {
            printf("IPC close failed. module = %d\n", module);
            return(-1);
        }
    }

    return(0);
}

/* **********************************************************
 * Function: ipc_connect
 * Input:
 *   key: the message module id
 * Return: >= 0: the message queue id
 *        -1: error
 ***********************************************************/
static int ipc_connect_n(int module)
{
    int  queid = -1;

    queid = msgget(module + IPC_QUEUE_OFFSET, 0);
    
    if(queid < 0)
    {
         printf("Connect message queue failed, module = %d \n", module);
         return(-1);
    }
    else return(queid);
}

/* ***********************************************************
 * Function: ipc_init
 * Return: 0 ok -1 error
 * Description: 创建系统公共的 IPC 由第一个启动进程调用  
 *************************************************************/

int ipc_init_n(void)
{
    int mid = -1;
    int qid = -1;
                                          /* max size of message */
    system("sysctl -w kernel.msgmax=100000");
                                          /* default max size of queue */
    system("sysctl -w kernel.msgmnb=5000000");

                                          /* ipc create message queue for every app module*/
    for(mid = 1; mid < MODULE_ID_MAXNUM; mid ++)
    {
         if(ipc_queid_list[mid].queuse)
         {
             qid = ipc_create_n(mid, ipc_queid_list[mid].msgmax);

             if(qid < 0)
             { 
              // assert(0);  
                 return(-1);     
             }              

             ipc_queid_list[mid].openfd = qid;
         }
    }

    return(0);
}

/*====================================================================================================================*/
/* ***********************************************************
 * Function: ipc_get_qid_n
 * Para  : isRecv, 1 获取接收队列 0 发送队列
 * Return: QID
 * Description: 获取某模块的队列ID  
 *************************************************************/
int ipc_get_qid_n(int module, int isRecv)
{
    int iQueid = -1;
    
    if(isRecv)
    {
        iQueid = ipc_queid_list[module].recvfd;
    
        if(iQueid < 0)  
        {
            if(ipc_queid_list[module].queuse)
            {
                iQueid = ipc_connect_n(module);
    
                ipc_queid_list[module].recvfd = iQueid;
            }
        }
    }
    else
    {
        iQueid = ipc_queid_list[module].sendfd;
    
        if(iQueid < 0)  
        {
            if(ipc_queid_list[module].queuse)
            {
                iQueid = ipc_connect_n(module);
    
                ipc_queid_list[module].sendfd = iQueid;
            }
        }
    }
    
    return(iQueid);
}

/*====================================================================================================================*/
/* ***********************************************************
 *  Function : ipc_send_id_n
 *  return   : >= 0 ok, -1 error
 * ***********************************************************/

static int ipc_send_id_n(int module)
{
    int queid = -1;
     
    if((module >= 1) && (module < MODULE_ID_MAXNUM))
    {
        module = ipc_queid_list[module].mapmod;   //目的模块映射
        
        queid  = ipc_queid_list[module].sendfd;
     
        if(queid < 0)  
        {
             if(ipc_queid_list[module].queuse)
             {
                 queid = ipc_connect_n(module);
      
                 ipc_queid_list[module].sendfd = queid;
             }
        }
    }

    return(queid);
}

/* ***********************************************************
 *  Function : ipc_send_msg_n0
 *  return   : 0 ok, -1 error
 *  description: send by QUEUE message, can use for BTB
 * ***********************************************************/
int ipc_send_msg_n0(struct ipc_quemsg_n *pmsg)
{
    int queid  = 0;
    
    if(pmsg == NULL) 
    {
        zlog_err("%s, %d, msg body is null or invalid\n", __FUNCTION__, __LINE__);
        return(-1);
    }
           
    queid = ipc_send_id_n(pmsg->recvmod);

    if(queid >= 0)
    {
         if(pmsg->msgbuff != NULL)
         {
             pmsg->msgbuff = mem_share_addrunmap(pmsg->msgbuff);
         }
         
         msgsnd(queid, pmsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT);
         
         return(0);
    }
    else
    {
         zlog_err("%s, %d, des module %d, queue id %d is error\n", __FUNCTION__, __LINE__, pmsg->recvmod, queid);
         return(-1);
    }
}

/* ***********************************************************
 *  Function : ipc_send_msg_n1
 *  Para     : msglen , must include (ipc_msghdr_n) head len
 *  return   : 0 ok, -1 error
 *  description: send by message body, can use for BTB
 * ***********************************************************/
int ipc_send_msg_n1(struct ipc_mesg_n *pmsg, int msglen)
{
    int module = 0;
    int queid  = 0;
    
    if((pmsg == NULL) || !mem_share_addrcheck((char*)pmsg))
    {
        zlog_err("%s, %d, msg body is null or invalid\n", __FUNCTION__, __LINE__);
        return(-1);
    }
      
    if((msglen < IPC_HEADER_LEN_N) || (msglen > IPC_MSG_MAXLEN))  
    {
        zlog_err("%s, %d, msg body len error, %d\n", __FUNCTION__, __LINE__, msglen);
        return(-1);
    }

    module = pmsg->msghdr.module_id;
 
    queid = ipc_send_id_n(module);

    if(queid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n sMsg = {0};

         sMsg.recvmod = module;
         sMsg.queueid = 0;
         sMsg.msgdlen = msglen;
         sMsg.msgbuff = mem_share_addrunmap(pmsg);
         
         while(retry > 0)
         {
             if(msgsnd(queid, &sMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;

             retry --;
                      
             usleep(10000);
         }
         
         return((retry > 0) ? 0 : -1);
    }
    else
    {
         zlog_err("%s, %d, des module %d, queue id %d is error\n", __FUNCTION__, __LINE__, module, queid);
         return(-1);
    }
}

/* ***********************************************************
 *  Function : ipc_send_msg_n2
 *  return   : 0 ok, -1 error
 *  description: send by message body, can use for BTB
 * ***********************************************************/
int ipc_send_msg_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, 
				    int  sender, uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex)
{
    int sendid = -1;

    sendid = ipc_send_id_n(module);
      
    if(sendid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n  sQMsg = {0};
         struct ipc_mesg_n   *pMesg = NULL;  

         if(!pdata) datlen = 0;
         
         pMesg = (struct ipc_mesg_n*)mem_share_malloc(datlen + IPC_HEADER_LEN_N,  sender);
         
         if(pMesg != NULL)
         {
             memset(pMesg, 0, IPC_HEADER_LEN_N);
      
             pMesg->msghdr.module_id   = module;
             pMesg->msghdr.sender_id   = sender;
             pMesg->msghdr.msg_type    = msgtype;
             pMesg->msghdr.msg_subtype = subtype;
             pMesg->msghdr.msg_index   = msgindex;
             pMesg->msghdr.opcode      = opcode;
             pMesg->msghdr.data_num    = datnum;
             pMesg->msghdr.data_len    = datlen;
    
             if(pdata && (datlen > 0)) memcpy(pMesg->msg_data, pdata, datlen);
               
             sQMsg.recvmod = module;
             sQMsg.queueid = 0;
             sQMsg.msgdlen = datlen + IPC_HEADER_LEN_N;
             sQMsg.msgbuff = mem_share_addrunmap(pMesg);
             
             while(retry > 0)
             {
                 if(msgsnd(sendid, &sQMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;
        
                 retry --;
                          
                 usleep(10000);
             }
             
             if(retry <= 0)  mem_share_free(pMesg, sender);

             return((retry > 0) ? 0 : -1);
         }
         else
         {
             zlog_err("%s, %d, malloc share memory error, len = %d\r\n", __FUNCTION__, __LINE__, datlen);
             return(-1);
         }
    }
    else
    {
         zlog_err("%s[%d]: ipc connect message queue (for send) failed! module=%d\r\n", __FUNCTION__, __LINE__, module);
         return(-1);
    }
}

/* ***********************************************************
 *  Function : ipc_send_msg_ha_n1
 *  return   : 0 ok, -1 error
 *  description: send by message body, can use for BTB
 * ***********************************************************/
int ipc_send_msg_ha_n1(struct ipc_mesg_n *pmsg, int msglen)
{
    int queid  = 0;
    
    if((pmsg == NULL) || !mem_share_addrcheck((char*)pmsg))
    {
        zlog_err("%s, %d, msg body is null or invalid\n", __FUNCTION__, __LINE__);
        return(-1);
    }
      
    if((msglen < IPC_HEADER_LEN_N) || (msglen > IPC_MSG_MAXLEN))  
    {
        zlog_err("%s, %d, msg body len error, %d\n", __FUNCTION__, __LINE__, msglen);
        return(-1);
    }

    queid = ipc_send_id_n(MODULE_ID_HA);

    if(queid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n sMsg = {0};

         sMsg.recvmod = MODULE_ID_HA;
         sMsg.queueid = 0;
         sMsg.msgdlen = msglen;
         sMsg.msgbuff = mem_share_addrunmap(pmsg);
         
         while(retry > 0)
         {
             if(msgsnd(queid, &sMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;

             retry --;
                      
             usleep(10000);
         }
         
         return((retry > 0) ? 0 : -1);
    }
    else
    {
         zlog_err("%s, %d, des module ha, queue id %d is error\n", __FUNCTION__, __LINE__, queid);
         return(-1);
    }
}

/* ***********************************************************
 *  Function : ipc_send_msg_ha_n2
 *  return   : 0 ok, -1 error
 *  description: send by message body, can use for BTB
 * ***********************************************************/
int ipc_send_msg_ha_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, int  sender, 
				      uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex, uint32_t slotidx)
{
    int sendid = -1;

    sendid = ipc_send_id_n(MODULE_ID_HA);
      
    if(sendid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n  sQMsg = {0};
         struct ipc_mesg_n   *pMesg = NULL;  

         if(!pdata) datlen = 0;
         
         pMesg = (struct ipc_mesg_n*)mem_share_malloc(datlen + IPC_HEADER_LEN_N,  sender);
         
         if(pMesg != NULL)
         {
             memset(pMesg, 0, IPC_HEADER_LEN_N);
      
             pMesg->msghdr.module_id   = module;
             pMesg->msghdr.sender_id   = sender;
             pMesg->msghdr.msg_type    = msgtype;
             pMesg->msghdr.msg_subtype = subtype;
             pMesg->msghdr.msg_index   = msgindex;
             pMesg->msghdr.opcode      = opcode;
             pMesg->msghdr.data_num    = datnum;
             pMesg->msghdr.data_len    = datlen;
             pMesg->msghdr.unit        = (slotidx>>8) & 0xff;
             pMesg->msghdr.slot        = (slotidx>>0) & 0xff;
             pMesg->msghdr.srcunit     = (slotidx>>24) & 0xff;
             pMesg->msghdr.srcslot     = (slotidx>>16) & 0xff;
    
             if(pdata && (datlen > 0)) memcpy(pMesg->msg_data, pdata, datlen);
               
             sQMsg.recvmod = MODULE_ID_HA;
             sQMsg.queueid = 0;
             sQMsg.msgdlen = datlen + IPC_HEADER_LEN_N;
             sQMsg.msgbuff = mem_share_addrunmap(pMesg);
             
             while(retry > 0)
             {
                 if(msgsnd(sendid, &sQMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;
        
                 retry --;
                          
                 usleep(10000);
             }
             
             if(retry <= 0)  mem_share_free(pMesg, sender);

             return((retry > 0) ? 0 : -1);
         }
         else
         {
             zlog_err("%s, %d, malloc share memory error, len = %d\r\n", __FUNCTION__, __LINE__, datlen);
             return(-1);
         }
    }
    else
    {
         zlog_err("%s[%d]: ipc connect message queue (for send) failed! module ha\r\n", __FUNCTION__, __LINE__);
         return(-1);
    }
}


/*====================================================================================================================*/

/* ***********************************************************
 *  Function : ipc_clear_que_n
 *  para     : module    clear quque 
 *  para     : msgtype   clear msgtype
 *  return   : 0 ok, -1 error
 *  description: clean message in queue
 * ***********************************************************/
int ipc_clear_que_n(int module, int msgtype)
{
    int queid = 0;

    if((module < 1) || (module >= MODULE_ID_MAXNUM))
    {
        zlog_err("%s, %d, des module id=%d is error\n", __FUNCTION__, __LINE__, module);

        return(-1);
    }
    
    queid = ipc_queid_list[module].recvfd;

    if(queid < 0)  
    {
        if(ipc_queid_list[module].queuse)
        {
            queid = ipc_connect_n(module);
 
            ipc_queid_list[module].recvfd = queid;
        }
    }

    if(queid >= 0)
    {
        struct ipc_quemsg_n smsg = {0,0,0,NULL};
        
        while(msgrcv(queid, &smsg, IPC_QUEMSG_TXTLEN, msgtype, IPC_NOWAIT) > 0)
        {
            if(smsg.msgbuff)
            {
                mem_share_free(mem_share_addrmap(smsg.msgbuff), module);
            }
        }

        return(0);
    }
    else
    {
        zlog_err("%s, %d, module %d, receiv queue id %d is error \r\n", __FUNCTION__, __LINE__, module, queid);

        return(-1);
    }
}

/* ***********************************************************
 *  Function : ipc_recv_que_n
 *  return   : 0 ok, -1 error
 *  description: receive message 
 * ***********************************************************/
int ipc_recv_que_n(int module, struct ipc_quemsg_n *pmsg)
{
    int queid = 0;

    if(pmsg == NULL)
    {
        zlog_err("%s, %d, msg body is null\n", __FUNCTION__, __LINE__);

        usleep(100000);
        
        return(-1);
    }
    
    if((module < 1) || (module >= MODULE_ID_MAXNUM))
    {
        zlog_err("%s, %d, des module id=%d is error\n", __FUNCTION__, __LINE__, module);

        usleep(100000);

        return(-1);
    }
    
    queid = ipc_queid_list[module].recvfd;

    if(queid < 0)  
    {
        if(ipc_queid_list[module].queuse)
        {
            queid = ipc_connect_n(module);
 
            ipc_queid_list[module].recvfd = queid;
        }
    }

    if(queid >= 0)
    {
        if(msgrcv(queid, pmsg, IPC_QUEMSG_TXTLEN, 0, 0) > 0)
        {
             if(pmsg->msgbuff != NULL) //&& (pmsg->msgdlen > 0))
             {
                  pmsg->msgbuff = mem_share_addrmap(pmsg->msgbuff);
             }
             return(0);
        }
        else return(-1);
    }
    else
    {
        zlog_err("%s, %d, module %d, receiv queue id %d is error \r\n", __FUNCTION__, __LINE__, module, queid);

        usleep(100000);

        return(-1);
    }
}

/*====================================================================================================================*/
/* ***********************************************************
 *  Function   : ipc_sync_send_n1
 *  para       : iTimeou wait timeout ms 
 *  return     : 0 ok, -1 send error, -2 recv error
 *  description: send mesg and wait reply,can't used for BTB 
 * ***********************************************************/
int ipc_sync_send_n1(struct ipc_mesg_n  *sndmsg, int  msglen, 
                     struct ipc_mesg_n **rcvmsg, int *rcvlen, int iTmOut)
{
    int module =  0;
    int recvid = -1;

    struct ipc_msghdr_n *rcvhdr = NULL;
    struct ipc_msghdr_n  sndhdr = {0, 0, 0};
    struct ipc_quemsg_n  rcvque = {0, 0, 0, NULL};

    if((rcvmsg == NULL) || (sndmsg == NULL) || (rcvlen == NULL)) return(-1); 

    iTmOut = (iTmOut < IPC_RECEIVE_RETRY) ? IPC_RECEIVE_RETRY : iTmOut;
    
   *rcvmsg = NULL;
   *rcvlen = 0;

    sndmsg->msghdr.msgflag |= IPC_MSG_FLAG_SYNC;

    memcpy(&sndhdr, &sndmsg->msghdr, sizeof(struct ipc_msghdr_n));
                                              /*必须记录发送信息*/
    module = sndhdr.sender_id;                /*应答接收*/

    recvid = ipc_queid_list[MODULE_ID_REPLY_A].recvfd;
    
    if(recvid < 0)  
    {
        if(ipc_queid_list[MODULE_ID_REPLY_A].queuse)
        {
            recvid = ipc_connect_n(MODULE_ID_REPLY_A);
    
            ipc_queid_list[MODULE_ID_REPLY_A].recvfd = recvid;
        }
    }
    
    if(recvid < 0)
    {
        zlog_err("%s[%d]: ipc connect reply module %d failed!\n", __FUNCTION__, __LINE__, module);
        return(-1);
    }

    while(msgrcv(recvid, &rcvque, IPC_QUEMSG_TXTLEN, module, IPC_NOWAIT) > 0)
    {
         rcvque.msgbuff = mem_share_addrmap(rcvque.msgbuff);

         mem_share_free(rcvque.msgbuff, module);

         memset(&rcvque, 0, sizeof(struct ipc_quemsg_n));
    }
    //printf("ipc_sync_send_n1 before send sndhdr.module_id=%d\r\n",sndhdr.module_id);
 
    if(ipc_send_msg_n1(sndmsg, msglen) != 0)
    {
        zlog_err("%s[%d]: message send from module %d to module %d fail!\n", 
                 __FILE__, __LINE__, module, sndhdr.module_id);
        return(-1);
    }

    iTmOut /= 10;   //10MS次数

    //printf("ipc_sync_send_n1 after send sndhdr.module_id=%d\r\n",sndhdr.module_id);
    
    while(iTmOut-- > 0)
    {
        if(msgrcv(recvid, &rcvque, IPC_QUEMSG_TXTLEN, module, IPC_NOWAIT) > 0)
        {
            //printf("ipc_sync_send_n1 rcvhdr=%p sndhdr.module_id=%d\r\n", rcvhdr, sndhdr.module_id);

            rcvhdr = (struct ipc_msghdr_n*)mem_share_addrmap(rcvque.msgbuff);
        
            //printf("ipc_sync_send_n1 receive  sndhdr.module_id=%d\r\n",sndhdr.module_id);

            if(rcvhdr != NULL)
            {
                if((rcvhdr->sender_id   == sndhdr.module_id) &&
                   (rcvhdr->msg_type    == sndhdr.msg_type)  &&
                   (rcvhdr->msg_subtype == sndhdr.msg_subtype) &&
              //   (rcvhdr->sequence    == sndhdr.sequence) &&
                  ((rcvhdr->opcode == IPC_OPCODE_REPLY) ||
                   (rcvhdr->opcode == IPC_OPCODE_ACK) || (rcvhdr->opcode == IPC_OPCODE_NACK)))
                {
                    *rcvlen = rcvque.msgdlen;
                    *rcvmsg = (struct ipc_mesg_n*)rcvhdr;
                     break;
                }
                else  
                {
                     printf("%s, receive msg not reply rcvhdr =%p\r\n",__FUNCTION__, rcvhdr);
                     
                     printf("moduleid=%d %d, type=%d %d, subtype=%d %d, opcode=%d\r\n", 
                            rcvhdr->sender_id, sndhdr.module_id, rcvhdr->msg_type, sndhdr.msg_type, rcvhdr->msg_subtype , sndhdr.msg_subtype, rcvhdr->opcode);

                     mem_share_free(rcvhdr, module);
                }
            }
        }

        usleep(10000); //让出 CPU 10ms
    }

    if(*rcvmsg != NULL) return(0);
    else
    {
        zlog_err("%s[%d] wait ipc reply timeout !\n", __FUNCTION__, __LINE__);
        return(-2);
    }
}

                     
/* ***********************************************************
 *  Function   : ipc_sync_send_n2
 *  return     : reply message 
 *  description: send mesg and wait reply,can't used for BTB 
 *              if return NULL fail
 *              ir return no NULL ok 
 * ***********************************************************/
struct ipc_mesg_n* ipc_sync_send_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, int  sender,
				                    uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex, int iTmOut) 
{
    int    iRetv = 0;
    int    iRepL = 0;

    struct ipc_mesg_n *pMsgSnd = NULL;
    struct ipc_mesg_n *pMsgRcv = NULL;

    pMsgSnd = mem_share_malloc(sizeof(struct ipc_msghdr_n) + datlen, sender);

    if(pMsgSnd == NULL) return(NULL);

    memset(pMsgSnd, 0, sizeof(struct ipc_msghdr_n));
 
    pMsgSnd->msghdr.data_len    = datlen;
    pMsgSnd->msghdr.module_id   = module;
    pMsgSnd->msghdr.sender_id   = sender;
    pMsgSnd->msghdr.msg_type    = msgtype;
    pMsgSnd->msghdr.msg_subtype = subtype;
    pMsgSnd->msghdr.opcode      = opcode;
    pMsgSnd->msghdr.msg_index   = msgindex;
    pMsgSnd->msghdr.data_num    = datnum;
    pMsgSnd->msghdr.msgflag     = IPC_MSG_FLAG_SYNC;

    if((pdata != NULL) && (datlen > 0)) memcpy(pMsgSnd->msg_data, pdata, datlen);

    iRetv = ipc_sync_send_n1(pMsgSnd, sizeof(struct ipc_msghdr_n) + datlen, &pMsgRcv, &iRepL, iTmOut);

    switch(iRetv)
    {
        case -1:   //发送失败
              mem_share_free(pMsgSnd, sender);
              return(NULL);
 
        case -2:   //发送成功, 接收失败
              return(NULL);
 
        case  0:
              return(pMsgRcv);
              
        default: return(NULL);
    }
}

/*====================================================================================================================*/
/* ***********************************************************
 *  Function   : ipc_recv_reply_n
 *  parameter  : module, receive module
 *  para       : iTmout  等待超时 MS
 *  return     : 0 ok, -1 error  1秒超时返回 
 *  description: receive reply message from reply queue
 * ***********************************************************/
int ipc_recv_reply_n(int module, struct ipc_mesg_n **rcvmsg, int *rcvlen, int iTmout)
{
    int queid = 0;
    struct ipc_quemsg_n sQueMsg = {0};
    
     if((rcvmsg == NULL) || (rcvlen == NULL))
     {
         zlog_err("%s, %d, msg receive body is null\n", __FUNCTION__, __LINE__);
             
         return(-1);
     }
          
     queid = ipc_queid_list[MODULE_ID_REPLY_A].recvfd;
    
     if(queid < 0)  
     {
         if(ipc_queid_list[MODULE_ID_REPLY_A].queuse)
         {
             queid = ipc_connect_n(MODULE_ID_REPLY_A);
    
             ipc_queid_list[MODULE_ID_REPLY_A].recvfd = queid;
         }
     }
    
     if(queid >= 0)
     {
         int retva = -1;

         iTmout /= 10;
         
         while(1)
         {
             if(msgrcv(queid, &sQueMsg, IPC_QUEMSG_TXTLEN, module, IPC_NOWAIT) > 0)
             {
                  retva  = 0;
                 *rcvlen = sQueMsg.msgdlen;
                 *rcvmsg = mem_share_addrmap(sQueMsg.msgbuff);    
                  break;
             }
             else
             {
                  if(--iTmout <= 0) break;
                  usleep(10000);
             }
         }
         return(retva);
     }
     else
     {
         zlog_err("%s, %d, module %d, receiv queue id %d is error \r\n", __FUNCTION__, __LINE__, module, queid);
        
         return(-1);
     }
}
                  
/* ***********************************************************
 *  Function   : ipc_send_reply_n1
 *  return     : 0 ok, -1 error
 *  description: send reply message, can't used for BTB 
 * ***********************************************************/
int ipc_send_reply_n1(struct ipc_msghdr_n *rcvhdr, struct ipc_mesg_n *repmsg, int replen)
{
    int rcvmod = 0;
    int sendid = -1;

    rcvmod = rcvhdr->sender_id;
    
    if((rcvmod < 1 ) || (rcvmod >= MODULE_ID_MAXNUM))
    {
        zlog_err("%s, %d, des module id is error %d\r\n", __FUNCTION__, __LINE__, rcvmod);
        return(-1);
    }
     
    if((repmsg == NULL) || !mem_share_addrcheck((char*)repmsg))
    {
        zlog_err("%s, %d, reply msg body is null or invalid\n", __FUNCTION__, __LINE__);
        return(-1);
    }
     
    if((replen < IPC_HEADER_LEN_N) || (replen > IPC_MSG_MAXLEN))  
    {
        zlog_err("%s, %d, reply msg data len error, %d\r\n", __FUNCTION__, __LINE__, replen);
        return(-1);
    }

    sendid = ipc_send_id_n(MODULE_ID_REPLY_A);
    
    if(sendid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n sMsg;

         repmsg->msghdr.module_id   = rcvmod;
         repmsg->msghdr.sender_id   = rcvhdr->module_id;
         repmsg->msghdr.msg_type    = rcvhdr->msg_type;
         repmsg->msghdr.msg_subtype = rcvhdr->msg_subtype;
       //repmsg->msghdr.msg_index   = rcvhdr->msg_index;
         repmsg->msghdr.sequence    = rcvhdr->sequence;

         
         sMsg.recvmod = rcvmod;
         sMsg.queueid = 0;
         sMsg.msgdlen = replen;
         sMsg.msgbuff = mem_share_addrunmap(repmsg);
         
         while(retry > 0)
         {
             if(msgsnd(sendid, &sMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;
    
             retry --;
                      
             usleep(10000);
         }
         
         return((retry > 0) ? 0 : -1);
    }
    else
    {
         zlog_err("%s[%d]: ipc connect reply queue (for send) failed!\n", __FUNCTION__, __LINE__);
         return(-1);
    }
}

/* ***********************************************************
 *  Function   : ipc_send_reply_n2
 *  return     : 0 ok, -1 error
 *  description: send reply message, can't used for BTB 
 * ***********************************************************/
int ipc_send_reply_n2(void *pdata, uint32_t datlen, uint16_t datnum, int module, int  sender, 
				 	  uint32_t  msgtype, uint16_t subtype,  uint32_t sequence, uint32_t msgindex, uint8_t opcode)
{
    int sendid = 0;
    
    sendid = ipc_send_id_n(MODULE_ID_REPLY_A);
      
    if(sendid >= 0)
    {
         int    retry = 3;
         struct ipc_quemsg_n  sQMsg = {0};
         struct ipc_mesg_n   *pMesg = NULL;  
         
         if(!pdata) datlen = 0;

         pMesg = (struct ipc_mesg_n*)mem_share_malloc(datlen + IPC_HEADER_LEN_N,  sender);
         
         if(pMesg != NULL)
         {
             memset(pMesg, 0, IPC_HEADER_LEN_N);
      
             pMesg->msghdr.module_id   = module;
             pMesg->msghdr.sender_id   = sender;
             pMesg->msghdr.msg_type    = msgtype;
             pMesg->msghdr.msg_subtype = subtype;
             pMesg->msghdr.msg_index   = msgindex;
             pMesg->msghdr.opcode      = opcode; //IPC_OPCODE_REPLY;
             pMesg->msghdr.sequence    = sequence;
             pMesg->msghdr.data_num    = datnum;
             pMesg->msghdr.data_len    = datlen;

             if((pdata) && (datlen > 0)) memcpy(pMesg->msg_data, pdata, datlen);
               
             sQMsg.recvmod = module;
             sQMsg.queueid = 0;
             sQMsg.msgdlen = datlen + IPC_HEADER_LEN_N;
             sQMsg.msgbuff = mem_share_addrunmap(pMesg);
             
             while(retry > 0)
             {
                 if(msgsnd(sendid, &sQMsg, IPC_QUEMSG_TXTLEN, IPC_NOWAIT) >= 0) break;
        
                 retry --;
                          
                 usleep(10000);
             }
             
             if(retry <= 0)  mem_share_free(pMesg, sender);

             return((retry > 0) ? 0 : -1);
             
         }
         else
         {
             zlog_err("%s, %d, malloc share memory error, len = %d\r\n", __FUNCTION__, __LINE__, datlen);
             return(-1);
         }
    }
    else
    {
         zlog_err("%s[%d]: ipc connect reply queue (for send) failed!\n", __FUNCTION__, __LINE__);
         return(-1);
    }
}

/*====================================================================================================================*/
/*====================================================================================================================*/



