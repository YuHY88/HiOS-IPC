/* Copyright (C) 2012-2013  HUAHUAN, Inc. All Rights Reserved. */

#ifndef _ZEBOS_NTP_H
#define _ZEBOS_NTP_H

#include <netinet/in.h>

#define IPC_TYPE_SNMP_NTP_GLOBAL_CFG  1

typedef struct ntp_info
{
    uint32_t        uiWorkMode;         /* 工作模式 */
    uint32_t        uiSyncInterval;     /* 同步间隔 */
    uint32_t        uiVersion;          /* 版本 */
    uint32_t        uiSendCSPacket;     /* 发包 */
    uint32_t        uiRecvCSpacket;     /* 收包 */
    uint32_t        uiSendPeerPacket;   /* 发送对等体报文 */
    uint32_t        uiRecvPeerPacket;   /* 接收对等体报文 */
    uint32_t        uiSyncFlag;         /* NTP 同步成功标记 */
    uint32_t        uiClientEnable ;    /* 客户端使能 */
    uint32_t        uiServerIP;         /* 服务器 IP */
    uint32_t        uiSyncState;        /* 同步状态 */
    uint32_t        uiStratum;          /* 层数 */
    struct timeval  stTimeOffset;       /* 时钟偏移 */
    struct timeval  stTimeDelay;        /* 时钟延时 */
    struct timeval  stBroadcastTime;    /* 广播报文时间间隔 */
    u_char          ucPacketFormat[16]; /* 报文类型，广播/单播 */
    u_char          ucReferID[16];      /* 参考时钟源 ID */
    uint16_t        uiPacketVpnID;

} ntp_globals;

extern ntp_globals ntp_global;

#endif /* _ZEBOS_SDM_H */
