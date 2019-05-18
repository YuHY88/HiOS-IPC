/*
 *  define IPC message queue operation
 */


#ifndef HIOS_IPC_H_
#define HIOS_IPC_H_

#include <lib/types.h>
#include <lib/module_id.h>

#if 0
/* IPC queue for forwarding packet */
#define IPC_PACKET_SEND_VTY      100   /* ipc for VTY telnet send packet to ftm */
#define IPC_PACKET_FTM_VTY       101   /* ipc for VTY telnet receive packet from ftm */
#define IPC_PACKET_SEND      102   /* ipc for app send packet to ftm */
#define IPC_PACKET_FTM       103   /* ipc for app receive packet from ftm */
#define IPC_PACKET_HAL       104   /* ipc for ftm send packet to hal */
#define IPC_PACKET_NETCONF   105   /* ipc for netconf receive pkt */

#define IPC_PACKET_HSL0   110   /* ipc for receive priority = 0 packet */
#define IPC_PACKET_HSL1   111   /* ipc for receive priority = 1 packet */
#define IPC_PACKET_HSL2   112   /* ipc for receive priority = 2 packet */
#define IPC_PACKET_HSL3   113   /* ipc for receive priority = 3 packet */
#define IPC_PACKET_HSL4   114   /* ipc for receive priority = 4 packet */
#define IPC_PACKET_HSL5   115   /* ipc for receive priority = 5 packet */
#define IPC_PACKET_HSL6   116   /* ipc for receive priority = 6 packet */
#define IPC_PACKET_HSL7   117   /* ipc for receive priority = 7 packet */
#define IPC_PACKET_HSL8   118   /* ipc for receive priority = 8 packet */
#define IPC_PACKET_HSL9   119   /* ipc for receive priority = 9 packet */
#define IPC_PACKET_HSL10  120   /* ipc for receive priority = 10 packet */
#define IPC_PACKET_HSL11  121   /* ipc for receive priority = 11 packet */
#define IPC_PACKET_HSL12  122   /* ipc for receive priority = 12 packet */
#define IPC_PACKET_HSL13  123   /* ipc for receive priority = 13 packet */
#define IPC_PACKET_HSL14  124   /* ipc for receive priority = 14 packet */
#define IPC_PACKET_HSL15  125   /* ipc for receive priority = 15 packet */


/* IPC queue for control message */
#define IPC_MSG_COMMON     1   /* common IPC for every one */
#define IPC_MSG_REPLY      2   /* common IPC for reply */
#define IPC_MSG_HAL        3   /* hal IPC to send and receive hal message */
#define IPC_MSG_IFM        4   /* ifm IPC only to receive ack and reply */
#define IPC_MSG_ROUTE      5   /* route IPC to send and receive route message */
#define IPC_MSG_SYSLOG     6   /* syslog IPC to send and receive syslog message */
#define IPC_MSG_ALARM      7   /* alarm IPC to send and receive alarm message */
#define IPC_MSG_HA         8   /* HA IPC to send and receive HA message */
#define IPC_MSG_FTM        9   /* ftm IPC to send and receive ftm message */
#define IPC_MSG_NETCONF   10   /* for netconf receive pkt message*/

#define IPC_MSG_FILEM     11   /* file manage IPC to receive filem message*/ 

#define IPC_MSG_MSDH      12   /* for snmpd and sdhmgt send and receive MSDH message */

#define IPC_MSG_HALHA     13   /* use for hal send messge to other slot*/

#define IPC_MSG_HALACK    14   /* use for hal send messge ACK to req ASKER*/



#define IPC_HEADER_LEN    24      /* IPC 消息头长度 */
#define IPC_QUEUE_LEN     1000    /* IPC 队列长度 */
#define IPC_MSG_LEN       2000    /* 用于控制消息的长度 */
#define IPC_MSG_LEN1      9960    /* 用于 9600 packet 的长度 */

#define ACCESS_PERMISSION 0755
#define IPC_RSEND_COUNT 10
#define IPC_RSEND_INTERVAL(count)  (10000 + (count) * 20000)
#define IPC_RRECIVE_COUNT 30
#define IPC_RRECIVE_COUNT_BLOCK 70
#define IPC_RRECIVE_INTERVAL(count)  (10000 + (count) * 20000)


/* IPC 类型 */
enum IPC_TYPE
{
   IPC_TYPE_INVALID = 0,
   IPC_TYPE_LSP,    /* mpls lsp, 1 */
   IPC_TYPE_ILM,    /* mpls ilm */
   IPC_TYPE_NHLFE,  /* mpls nhlfe */
   IPC_TYPE_PW,     /* mpls pw */
   IPC_TYPE_VSI,    /* mpls vsi */
   IPC_TYPE_L3VPN,  /* mpls l3vpn */
   IPC_TYPE_MPLSOAM,/* mpls oam*/
   IPC_TYPE_ROUTE,  /* ip 路由 */
   IPC_TYPE_FIB,    /* ip fib */
   IPC_TYPE_NHP,    /* ip nhp */

   IPC_TYPE_L3IF,   /* ip l3if, 11 */
   IPC_TYPE_ARP,    /* arp 全局信息*/
   IPC_TYPE_ARPIF,  /* arp 接口信息 */
   IPC_TYPE_NDP,    /* ndp 全局信息*/
   IPC_TYPE_NDPIF,  /* ndp 接口信息*/
   IPC_TYPE_IFM,    /* 接口管理 */
   IPC_TYPE_FTM,    /* 软转发 */
   IPC_TYPE_VLAN,   /* vlan */
   IPC_TYPE_L2IF,   /* l2if */
   IPC_TYPE_MPLSIF, /* mpls if */

   IPC_TYPE_DEVM,   /* 设备管理, 21*/
   IPC_TYPE_CES,    /* ces */
   IPC_TYPE_ACL,    /* qos acl */
   IPC_TYPE_CAR,    /* qos car */
   IPC_TYPE_QOS,    /* QOS 全局信息 */
   IPC_TYPE_HQOS,
   IPC_TYPE_QOSIF,  /* QOS 接口配置 */
   IPC_TYPE_CFM,    /* CFM 消息 */
   IPC_TYPE_PROTO,  /* 协议注册*/
   IPC_TYPE_PACKET, /* 转发报文 */

   IPC_TYPE_VTY,    /* VTY 消息 , 31*/
   IPC_TYPE_SYSLOG, /* syslog 消息 */
   IPC_TYPE_AAA,    /* AAA 消息 */
   IPC_TYPE_BFD,    /* BFD 消息 */
   IPC_TYPE_TRUNK,  /* trunk */
   IPC_TYPE_MAC,    /* mac 地址消息 */
   IPC_TYPE_SYNCE,  /* synce 消息 */
   IPC_TYPE_TUNNEL, /* tunnel 消息 */
   IPC_TYPE_ALARM,  /* 告警消息 */
   IPC_TYPE_FILE,	/* 文件管理消息 */

   IPC_TYPE_SNMP,   /* SNMP 消息 , 41*/
   IPC_TYPE_SNMP_TRAP,   /* SNMP TRAP */
   IPC_TYPE_SLA,	/* SLA 消息 */
   IPC_TYPE_OSPF,	/* OSPF 消息 */
   IPC_TYPE_RIP,	/* RIP 消息 */
   IPC_TYPE_ISIS,	/* ISIS 消息 */
   IPC_TYPE_OFP,    /* openflow 消息 */
   IPC_TYPE_ELPS,   /* ELPS 消息 */
   IPC_TYPE_ERPS,   /* ERPS ~{O{O"~} */
   IPC_TYPE_BGP,    /* BGP 消息 */   

   IPC_TYPE_HA,     /* HA 消息 , 51 */  
   IPC_TYPE_MSTP,	/* MSTP 消息 */ 
   IPC_TYPE_SFP,    /* SFP消息 */
   IPC_TYPE_IPMC,  /*IPMC message*/
   IPC_TYPE_MPLSAPS,/* mpls aps*/
   IPC_TYPE_EFM,        /*efm message*/
   IPC_TYPE_SYSTEM, /* system message*/
   IPC_TYPE_SYNC_DATA,
   IPC_TYPE_LDP,    /* mpls ldp */
   IPC_TYPE_SDHMGT, /* sdhmgt message */

   IPC_TYPE_RMOND,  /* RMON, 61 */
   IPC_TYPE_L2CP,   /* l2cp */
   IPC_TYPE_RESERVE_VLAN, /*reseve*/
   IPC_TYPE_STM,    /* stm-n */
   IPC_TYPE_VCG,    /* vcg message */
   IPC_TYPE_MPLS,   /* MPLS  IPC */

   IPC_TYPE_STORM_CONTROL,   /* l2if storm control */
   IPC_TYPE_DOT1Q_TUNNEL,   /* l2if dot1q-tunnel */
   IPC_TYPE_OSPF_DCN,		/* OSPF DCN message */
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


/* ipc 消息头 */
struct ipc_msghdr
{
	sint32     module_id;  /* module id of receiver */
	uint16_t   sender_id;  /* module id of sender */
	uint16_t   data_len;   /* IPC data length */
	enum IPC_TYPE  msg_type;	 /* IPC msg type */
	uint8_t    msg_subtype;      /* subtype of msg, app can customize if needed */
	uint8_t    opcode;     /* operation code of data */
	uint8_t    data_num;   /* number of data */
	uint8_t    priority;   /* 消息的优先级 */
	uint8_t    unit;       /* unit 号，用于设备间的消息, 0 表示框内 */
	uint8_t    slot;       /* slot 号，用于板间的消息, 0 表示板内 */
	uint16_t   sequence;   /* 消息的序号 */
    
	uint32_t   msg_index;  /* maybe ifindex、lsp_index、pw_index and so on */


};


/* 控制消息 ipc 消息体 */
struct ipc_mesg
{
    struct ipc_msghdr msghdr;
	uint8_t msg_data[IPC_MSG_LEN];
};


/* 数据报文 ipc 消息体 */
struct ipc_pkt
{
    struct ipc_msghdr msghdr;
	uint8_t msg_data[IPC_MSG_LEN1];
};

/*ha msg send will use this struct*/
struct ha_ipc_mesg
{
    unsigned char target_slot;
    unsigned char target_queue_id;
    unsigned char pad[2];
    struct ipc_pkt ipc_mesg_data;
};

extern int ipc_common_id;   /* common ipc queue for every one */
extern int ipc_reply_id;    /* common ipc queue for reply */
extern int ipc_route_id;    /* ipc queue for route */
extern int ipc_ftm_id;      /* ipc queue for send message to ftm */
extern int ipc_hal_id;      /* ipc queue for hal */
extern int ipc_syslog_id;   /* ipc queue for syslog */
extern int ipc_ha_id;       /* ipc queue for send message to ha */

extern int ipc_pktrx_vty_id;    /* common ipc for app receive packet */
extern int ipc_pkttx_vty_id;    /* common ipc for app send packet */

extern int ipc_pktrx_id;    /* common ipc for app receive packet */
extern int ipc_pkttx_id;    /* common ipc for app send packet */


int ipc_create(int key, int queue_len, int msg_size);
int ipc_close(int id);
int ipc_connect(int key);
int ipc_send(int id, struct ipc_msghdr *msghdr, void *data);
int ipc_send_block(int id, struct ipc_msghdr *msghdr, void *data);
int ipc_recv(int id, struct ipc_mesg *pmsg, int module_id);
int ipc_recv_block(int id, struct ipc_mesg *pmsg, int module_id);
int ipc_recv_pkt(int id, struct ipc_pkt *pmsg, int module_id);


/* 公共 IPC 的函数原型*/
void ipc_init(void);
int ipc_connect_common(void);/* connect to common ipc */
int ipc_recv_common(struct ipc_mesg *pmsg, int module_id);/* receive msg from common ipc */
int ipc_send_common(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);/* send msg to common ipc */

/* 从公共 IPC 发送消息，支持在消息头中发送 index */
int ipc_send_common1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* send msg and return reply data */
void * ipc_send_common_wait_reply(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* send msg and return reply msg */
struct ipc_mesg * ipc_send_common_wait_reply1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);


/* send message and returen errcode */
int ipc_send_common_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* ipc for msdh */
int ipc_recv_msdh(struct ipc_mesg *pmsg, int module_id);
int ipc_send_msdh(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);

/* 发送和接收路由消息 */
int ipc_recv_route(struct ipc_mesg *pmsg, int module_id);
int ipc_send_route(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);
int ipc_send_ha(void *pdata, int data_len, int data_num, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);
int ipc_send_ha_1(struct ipc_pkt *pmsg, int dlen);


/* ~{7"KM~} ha ~{O{O"~} */
int ipc_send_partnerslot_hal_byha(void *pdata, int data_len, int data_num, int sender_id,
                                  enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ipc_send_partnerslot_common_byha(void *pdata, int data_len, int data_num, int sender_id,
                                     enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ipc_send_partnerslot_byha(void *pdata, int data_len, int data_num, int sender_id,
                              enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char queuqe_id);

int ipc_send_slot_byha(void *pdata, int data_len, int data_num, int sender_id,
                       enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id);

struct ipc_pkt *ipc_send_slot_byha_wait_reply1(void *pdata, int data_len, int data_num, int sender_id,
        enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id);

void *ipc_send_slot_byha_wait_reply(void *pdata, int data_len, int data_num, int sender_id,
                                    enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id);

int ipc_send_slot_byha_wait_ack(void *pdata, int data_len, int data_num, int sender_id,
                                enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id);

int ipc_send_slot_byha_1(struct ipc_pkt *pmsg, int dlen, unsigned char target_slot, unsigned char queuqe_id) ;

int ipc_send_partnerslot_byha_1(struct ipc_pkt *pmsg, int dlen, unsigned char queuqe_id);

int ipc_send_reply_bulk_toslot_byha(void *pdata, int data_len, int data_num, int sender_id,
                                    enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot);

int ipc_send_reply_toslot_byha(void *pdata, int data_len, int sender_id,
                               enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot);

int ipc_send_ack_toslot_byha(int sender_id, enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot);

int ipc_send_noack_toslot_byha(uint32_t errcode, int sender_id,
                               enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot);



/* 发送 syslog 消息 */
int ipc_send_syslog(void *pdata, int data_len, int sender_id, uint8_t subtype, enum IPC_OPCODE opcode);

/* 发送 ftm 消息 */
int ipc_send_ftm(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* 发送 hal 消息 */
int ipc_send_hal(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

int ipc_send_hal_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* send msg and return reply data */
void * ipc_send_hal_wait_reply(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* send msg and return reply msg */
struct ipc_mesg * ipc_send_hal_wait_reply1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index);

/* 支持发送多个应答 */
int ipc_send_reply_bulk (void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index);

/* 发送应答消息 */
int ipc_send_reply (void *pdata, int data_len, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index);

/* 发送确认成功的消息 */
int ipc_send_ack (int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index);

/* 发送确认失败的消息，需要发送错误码 */
int ipc_send_noack (uint32_t errcode, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index);

/* add by suxq 2017/09/19*/
int ipc_send_filemext(struct ipc_pkt *pmsg, int dlen);

int ipc_send_filem(void *pdata, int data_len, int data_num, int module, int sender,
                   enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode);

int ipc_rawsend(int id, void *data, int datlen, int wait);

int ipc_rawrecv(int id, void *data, int buflen, int module_id, int wait);

#endif
#endif /* _MSG_IPC_H_ */
