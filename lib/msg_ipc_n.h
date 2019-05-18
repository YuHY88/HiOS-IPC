/*
 *  define IPC message queue operation
 */


#ifndef HIOS_IPC_N_H_
#define HIOS_IPC_N_H_

#include <lib/types.h>
#include <lib/module_id.h>


#define IPC_MSG_REPLY_N    MODULE_ID_REPLY_A /* common IPC for reply */

#define IPC_RECV_TASK_MAX  16      /*单进程最多可以启动16个线程接收*/

#define IPC_HEADER_LEN_N   48      /* IPC 消息头长度 */

#define IPC_MSG_LEN_S      2000    /* 普通控制消息的最大长度 */

#define IPC_MSG_LEN_N      16320   /* 长消息最大长度,用于 16000 packet 的长度 */
                                   /* 通过单个消息传送的最大数据长度*/
#define IPC_MSG_MAXLEN    (IPC_HEADER_LEN_N +  IPC_MSG_LEN_N)  
                                   /* IPC 消息总长度 */

#define IPC_MSG_LEN        IPC_MSG_LEN_S    /* 用于控制消息的长度 */

#define IPC_MSG_LEN1       9960    /* 用于 9600 packet 的长度 */

/*====================================================================================================================*/
/* IPC 消息主类型 */
#if 1 
/*msg_ipc.h 删除后打开*/
enum IPC_TYPE
{
   IPC_TYPE_INVALID = 0,
   IPC_TYPE_LSP,    /* mpls lsp */
   IPC_TYPE_ILM,    /* mpls ilm */
   IPC_TYPE_NHLFE,  /* mpls nhlfe */
   IPC_TYPE_PW,     /* mpls pw */
   IPC_TYPE_VSI,    /* mpls vsi */
   IPC_TYPE_L3VPN,  /* mpls l3vpn */
   IPC_TYPE_MPLSOAM,/* mpls oam*/
   IPC_TYPE_ROUTE,  /* ip 路由 */
   IPC_TYPE_FIB,    /* ip fib */
   IPC_TYPE_NHP,    /* ip nhp */
   IPC_TYPE_L3IF,   /* ip l3if */
   IPC_TYPE_ARP,    /* arp 全局信息*/
   IPC_TYPE_ARPIF,  /* arp 接口信息 */
   IPC_TYPE_NDP,    /* ndp 全局信息*/
   IPC_TYPE_NDPIF,  /* ndp 接口信息*/
   IPC_TYPE_IFM,    /* 接口管理 */
   IPC_TYPE_FTM,    /* 软转发 */
   IPC_TYPE_VLAN,   /* vlan */
   IPC_TYPE_L2IF,   /* l2if */
   IPC_TYPE_MPLSIF, /* mpls if */
   IPC_TYPE_DEVM,   /* 设备管理*/
   IPC_TYPE_CES,    /* ces */
   IPC_TYPE_ACL,    /* qos acl */
   IPC_TYPE_CAR,    /* qos car */
   IPC_TYPE_QOS,    /* QOS 全局信息 */
   IPC_TYPE_HQOS,
   IPC_TYPE_QOSIF,  /* QOS 接口配置 */
   IPC_TYPE_CFM,    /* CFM 消息 */
   IPC_TYPE_PROTO,  /* 协议注册*/
   IPC_TYPE_PACKET, /* 转发报文 */
   IPC_TYPE_VTY,    /* VTY 消息 */
   IPC_TYPE_SYSLOG, /* syslog 消息 */
   IPC_TYPE_AAA,    /* AAA 消息 */
   IPC_TYPE_BFD,    /* BFD 消息 */
   IPC_TYPE_TRUNK,  /* trunk */
   IPC_TYPE_MAC,    /* mac 地址消息 */
   IPC_TYPE_SYNCE,  /* synce 消息 */
   IPC_TYPE_TUNNEL, /* tunnel 消息 */
   IPC_TYPE_ALARM,  /* 告警消息 */
   IPC_TYPE_FILE,	/* 文件管理消息 */
   IPC_TYPE_SNMP,   /* SNMP 消息 */
   IPC_TYPE_SNMP_TRAP,   /* SNMP TRAP */
   IPC_TYPE_SLA,	/* SLA 消息 */
   IPC_TYPE_OSPF,	/* OSPF 消息 */
   IPC_TYPE_RIP,	/* RIP 消息 */
   IPC_TYPE_ISIS,	/* ISIS 消息 */
   IPC_TYPE_OFP,    /* openflow 消息 */
   IPC_TYPE_ELPS,   /* ELPS 消息 */
   IPC_TYPE_ERPS,   /* ERPS ~{O{O"~} */
   IPC_TYPE_BGP,    /* BGP 消息 */   
   IPC_TYPE_HA = 51,     /* HA 消息 */  
   IPC_TYPE_MSTP,	/* MSTP 消息 */ 
   IPC_TYPE_SFP,    /* SFP消息 */
   IPC_TYPE_IPMC,   /*IPMC message*/
   IPC_TYPE_MPLSAPS,/* mpls aps*/
   IPC_TYPE_EFM,        /*efm message*/
   IPC_TYPE_SYSTEM, /* system message*/
   IPC_TYPE_SYNC_DATA,
   IPC_TYPE_LDP,    /* mpls ldp */
   IPC_TYPE_SDHMGT, /* sdhmgt message */
   IPC_TYPE_RMOND,/* RMON*/
   IPC_TYPE_L2CP, /*l2cp*/
   IPC_TYPE_STAT_DB, /* statis database */
   IPC_TYPE_RESERVE_VLAN, /*reseve*/
   IPC_TYPE_STM,    /* stm-n */
   IPC_TYPE_VCG,    /* vcg message */
   IPC_TYPE_MPLS,   /* MPLS  IPC */

   IPC_TYPE_STORM_CONTROL,   /* l2if storm control */
   IPC_TYPE_DOT1Q_TUNNEL,   /* l2if dot1q-tunnel */
   IPC_TYPE_OSPF_DCN,		/* OSPF DCN message */
   IPC_TYPE_TRIGER,         /* triger 驱动事件, 可用于模块给自己发消息 */  
   IPC_TYPE_MAX = 255
};


/* IPC 的操作码 */
enum IPC_OPCODE
{
	IPC_OPCODE_INVALID = 0,
	IPC_OPCODE_ADD,        /* add */
	IPC_OPCODE_DELETE,     /* delete */
	IPC_OPCODE_UPDATE,     /* update */
	IPC_OPCODE_GET,	       /* get */
	IPC_OPCODE_GET_BULK,   /* 批量 get */
	IPC_OPCODE_CLEAR,      /* 批量删除 */
	IPC_OPCODE_UP,	       /* up */
	IPC_OPCODE_DOWN,	   /* down */
	IPC_OPCODE_ENABLE,     /* enable */
	IPC_OPCODE_DISABLE,    /* disable */
	IPC_OPCODE_REGISTER,   /* 注册 */
	IPC_OPCODE_UNREGISTER, /* 取消注册 */
	IPC_OPCODE_EVENT,      /* 事件通知 */
	IPC_OPCODE_ACK,        /* 答复成功 */
	IPC_OPCODE_NACK,       /* 答复失败 */
	IPC_OPCODE_REPLY,      /* 应答 */
	IPC_OPCODE_READY,      /* 准备完毕 */
	IPC_OPCODE_FINISH,     /* 结束 */
	IPC_OPCODE_CONNECT,    /* 连接 */
	IPC_OPCODE_DISCONNECT, /* 断开连接 */
	IPC_OPCODE_CHANGE,          /*set efm undirection send pkt  remote loopback mac-swap*/
	IPC_OPCODE_LINK_MONITOR,/*set efm link monitor data in hal*/
	IPC_OPCODE_EFM,                    /*efm create and free struct in hal*/
	IPC_OPCODE_VPORT_ADD,
	IPC_OPCODE_REPORT,
	IPC_OPCODE_MAX = 255
};
#endif
/*used by BGP*/

typedef enum
{
    BGP_ENABLE_MSG = 1,
    BGP_DISABLE_MSG,
    BGP_SET_PASSWD_MSG,
    BGP_UNSET_PASSWD_MSG,
} bgp_msg_type_t;

struct peer_entry{
	uint32_t sip;
	uint32_t dip;
	char pass[256];
};



struct ipc_msghdr_n
{
    sint32         module_id;  /* module id of receiver */
    sint32         sender_id;  /* module id of sender */
	uint32_t       data_len;   /* IPC data length */
    uint32_t       msg_type;   /* IPC msg type */
    uint16_t       msg_subtype;/* subtype of msg, app can customize if needed */
    uint16_t       data_num;   /* number of data */
    uint8_t        opcode;     /* operation code of data */
    uint8_t        priority;   /* 消息的优先级 */
    uint8_t        unit;       /* unit 号，用于设备间的消息, 0 表示框内 */
    uint8_t        slot;       /* slot 号，用于板间的消息, 0 表示板内 */
    uint8_t        srcunit;    /* source unit */
    uint8_t        srcslot;    /* source slot */
    uint16_t       spare;
    uint32_t       sequence;   /* 消息的序号      sync use this field */
    uint32_t       msg_index;  /* maybe ifindex、lsp_index、pw_index and so on */

    uint32_t       offset;     /* 有效数据区偏移*/
    uint32_t       msgflag;    /* use for future */
    sint32         result;     /* for sync reply or BTB reply */
};


/* 控制消息 ipc 消息体 */
struct ipc_mesg_n
{
    struct ipc_msghdr_n msghdr;
	uint8_t             msg_data[4];  /*max len IPC_MSG_LEN_N*/
};

#define IPC_MSG_FLAG_SYNC  0x8000 /*同步请求消息，需要同步应答*/

/* IPC 承载消息体*/
struct ipc_quemsg_n
{
    sint32            recvmod;
    sint32            queueid;
  //sint32            msgtype;    
  /*主要用来识别定时器消息*/
  //sint32            msgpara;
    sint32            msgdlen;
    void             *msgbuff;
};

#define IPC_QUEMSG_TXTLEN (sizeof(struct ipc_quemsg_n) - 4)

struct ipc_thread_par
{
    int     iRevMod;
    int     iQueIdx;
    
    char    bTrdName[16];
};

/* ~{J}>]1(ND~} ipc ~{O{O"Le~} */
#if 0   //禁止使用该数据结构
#define IPC_MSG_LEN1      9960    /* 用于 9600 packet 的长度 */
struct ipc_pkt_n
{
    struct ipc_msghdr_n msghdr;
	uint8_t msg_data[IPC_MSG_LEN1];
};
#endif

/*====================================================================================================================*/

int ipc_init_n(void);

/* ***********************************************************
 * Function: ipc_get_qid_n
 * Para  : isRecv, 1 获取接收队列 0 发送队列
 * Return: QID
 * Description: 获取某模块的队列ID  
 *************************************************************/
int ipc_get_qid_n(int module, int isRecv);

/* ***********************************************************
 *  Function : ipc_send_msg_n0
 *  return   : 0 ok, -1 error
 *  description: send by QUEUE message, can use for BTB
 * ***********************************************************/
int ipc_send_msg_n0(struct ipc_quemsg_n *pmsg);

/* ***********************************************************
 *  Function : ipc_send_msg_n1
 *  return   : 0 ok, -1 error
 *  description: send by message body, can't use for BTB
 *              if return error, must free pmsg
 * ***********************************************************/
int ipc_send_msg_n1(struct ipc_mesg_n *pmsg, int msglen);

/* ***********************************************************
 *  Function : ipc_send_msg_n2
 *  return   : 0 ok, -1 error
 *  description: send by message body, can't use for BTB
 * ***********************************************************/
int ipc_send_msg_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, 
				    int  sender, uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex);


/* ***********************************************************
 *  Function : ipc_send_msg_ha_n1
 *  return   : 0 ok, -1 error
 *  description: send by message body, use for BTB
 *              if return error, must free pmsg
 * ***********************************************************/
int ipc_send_msg_ha_n1(struct ipc_mesg_n *pmsg, int msglen);

/* ***********************************************************
 *  Function : ipc_send_msg_ha_n2
 *  return   : 0 ok, -1 error
 *  description: send by message body, use for BTB
 *            slotidx for src slot and des slot 
 *            srcunit<<24|srcslot|<<16|desunit<<8|desslot
 * ***********************************************************/
int ipc_send_msg_ha_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, 
				       int  sender, uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex, uint32_t slotidx);

/* ***********************************************************
 *  Function : ipc_clear_que_n
 *  return   : 0 ok, -1 error
 *  description: clean message in queue
 * ***********************************************************/
int ipc_clear_que_n(int module, int msgtype);

/* ***********************************************************
 *  Function : ipc_recv_que_n
 *  return   : 0 ok, -1 error
 *  description: receive message 
 * ***********************************************************/

int ipc_recv_que_n(int module, struct ipc_quemsg_n *pmsg);

/* ***********************************************************
 *  Function   : ipc_recv_reply_n
 *  parameter  : module, receive module
 *  return     : 0 ok, -1 error  1秒超时返回一次
 *  description: receive reply message 
 * ***********************************************************/
int ipc_recv_reply_n(int module, struct ipc_mesg_n **rcvmsg, int *rcvlen, int iTmout);

/* ***********************************************************
 *  Function   : ipc_send_reply_n1
 *  return     : 0 ok, -1 error
 *  description: send reply message, can't used for BTB 
 * ***********************************************************/
int ipc_send_reply_n1(struct ipc_msghdr_n *rcvhdr, struct ipc_mesg_n *repmsg, int replen);

/* ***********************************************************
 *  Function   : ipc_send_reply_n2
 *  return     : 0 ok, -1 error
 *  description: send reply message, can't used for BTB 
 * ***********************************************************/
int ipc_send_reply_n2(void *pdata, uint32_t datlen, uint16_t datnum, int module, int  sender, 
					  uint32_t  msgtype, uint16_t subtype,  uint32_t sequence, uint32_t msgindex, uint8_t opcode);


/* ***********************************************************
 *  Function   : ipc_sync_send_n1
 *  return     : 0 ok, -1 send error, -2 recv error
 *  description: send mesg and wait reply,can't used for BTB 
 *              if return -1 must free sndmsg
 *              ir return 0 must free rcvmsg after used
 * ***********************************************************/
int ipc_sync_send_n1(struct ipc_mesg_n *sndmsg, int msglen, struct ipc_mesg_n **rcvmsg, int *rcvlen, int iTmOut);


/* ***********************************************************
 *  Function   : ipc_sync_send_n2
 *  return     : reply message 
 *  description: send mesg and wait reply,can't used for BTB 
 *              if return NULL fail
 *              ir return no NULL ok 
 * ***********************************************************/
struct ipc_mesg_n* ipc_sync_send_n2(void *pdata, uint32_t datlen,  uint16_t datnum, int module, int  sender,
				                    uint32_t msgtype, uint16_t subtype, uint8_t opcode, uint32_t msgindex, int iTmOut);

/*====================================================================================================================*/

typedef  int (*MsgProcCall)(struct ipc_mesg_n *pmsg, int imlen);

/*====================================================================================================================*/

/* ***********************************************************
 *  Function   : ipc_recv_init
 *  para       : pMaster APP process master thread 
 *  return     : >=0 fd  for main thread useing, -1 error
 *  description: ipc receive init for APP
 * ***********************************************************/
int ipc_recv_init(void *pMaster);


/* ***********************************************************
 *  Function   : ipc_recv_thread_start
 *  para       : iRecvMod  receive module (APP)
 *  para       : iPolicy   SCHED_OTHER SCHED_FIFO SCHED_RR
 *  para       : iPrio     receive thread priority
 *                       =-1 use main thread prority,
 *  para       : pMsgProc  message process function
 *  para       : iClrQue   whether clean message queue or not
 *  return     : 0 ok, -1 error
 *  description: ipc message receive thread  start
 * ***********************************************************/

int ipc_recv_thread_start(const char *pThreadName, int iRecvMod, int iPolicy, int iPrio, MsgProcCall pMsgProc, int iClrQue);


/*====================================================================================================================*/

#endif /* _MSG_IPC_H_ */

