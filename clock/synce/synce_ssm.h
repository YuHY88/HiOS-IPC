/******************************************************************************
 * Filename: synce_ssm.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for syncE protocal and ssm pkt header file.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/

#ifndef SYNCE_SSM_H_
#define SYNCE_SSM_H_

#define SSM_DNU     0x0f
#define SSM_PRC     0x02
#define SSM_SEC     0x0b
#define SSM_SSUA    0x04
#define SSM_SSUB    0x08
#define SSM_UNK     0x00

#define SSM_DNU_STR     "DNU"
#define SSM_PRC_STR     "PRC"
#define SSM_SEC_STR     "SEC"
#define SSM_SSUA_STR    "SSU-A"
#define SSM_SSUB_STR    "SSU-B"
#define SSM_UNK_STR     "UNK"

#define SSM_DEST_MAC_0 0x01
#define SSM_DEST_MAC_1 0x80
#define SSM_DEST_MAC_2 0xc2
#define SSM_DEST_MAC_3 0x00
#define SSM_DEST_MAC_4 0x00
#define SSM_DEST_MAC_5 0x02

#define SSM_ETH_TYPE    0x8809
#define SSM_ETH_SUBTYPE 0x0A
/*SSM组播地址*/
#define SSM_MAC  {0x01, 0x80, 0xC2, 0x00, 0x00, 0x02}
/*MAC地址长度*/
#define SSM_MAC_ADDR_LEN   6

#define SSM_PKT_MAX_LEN 1500

#define SYNCE_DEFAULT_MAC_LEN           6

#define SYNCE_SSM_EVENT_PKT_SEND_CNT    3

/* SSM 状态 */
enum SSM_STATUS
{
    SSM_STATUS_INVALID = 0,
    SSM_STATUS_TIMEOUT,      /* ssm 超时 */
    SSM_STATUS_NORMAL,
};
enum SSM_PKT_TYPE
{
    PKT_INFO = 0,
    PKT_ENEVT,
};
enum EVENT_SSM_PKT
{
    EVENT_RECV_INFO_PKT = 0,
    EVENT_RCV_EVENT_PKT,
    EVENT_UP,
    EVENT_DOWN,
    EVENT_TIMEOUT,
};


/* synce 在时钟源接口下的数据结构 */
struct synce_if
{
    enum SSM_STATUS status;
    uint8_t         recv_ql; /*收到的QL值*/
    uint8_t         priority;/* 接口优先级*/
    uint8_t         timeout_cnt; /*端口超时计数*/
    uint16_t        send_interval_cnt; /*端口发送报文间隔计数*/
    uint8_t         port_status; /*端口状态  1 : link  0 : down*/
    uint32_t        pkt_recv_cnt;     /* 接收的 ssm 报文数 */
    uint32_t        pkt_send_cnt;     /* 发送的 ssm 报文数 */
    uint32_t        pkt_err;      /* ssm 错包数 */
};

struct ssm_pkt
{
    uint8_t         subtype;            //SSM 报文子类型 0x0A
    uint8_t         itu_oui[3];         //ITU-OUI值  固定为 00-19-A7
    uint16_t        itu_subtype;        //ITU 子类型 固定值为 0x0001
    uint8_t         flag;               /* bit7 - bit4 : 版本号 固定值为 0x1
                                            bit3 : 0 代表information报文   1 代表 event 报文*/
    uint8_t         pad1[3];            //填充3字节
    uint8_t         tlv_type;           // SSM TLV类型 固定值为 0x01
    uint16_t        tlv_len;            //TLV长度  固定值 0x04
    uint8_t         ssm_code;           //high 4bits : unused  low 4bits : QL值
    uint8_t         pad2[32];           //填充32个字节数据，要求DATA部分最少36字节
} __attribute__((packed));

void synce_ssm_pkt_register(void);


int synce_ssm_port_up(struct synce_if *psynce_info, uint32_t ifindex);

int synce_ssm_port_down(struct synce_if *psynce_info, uint32_t ifindex);


/* 接收 ssm 报文 加到lldp_pkt_recv内*/
int synce_ssm_recv(struct pkt_buffer *ppkt);


int synce_ssm_send(struct synce_if *psynce_if, uint32_t ifindex, enum SSM_PKT_TYPE pkt_type); /* 发送 ssm 报文 */


int synce_ssm_fsm(enum EVENT_SSM_PKT en_type, struct synce_if *psynce_if, uint32_t ifindex);  /* ssm 状态机处理 */

int synce_ql_val_to_str(char *str, unsigned char ql);
int synce_sa_val_to_str(char *str, unsigned char sa);

void synce_protocol_run_stat_can(void);



#endif

