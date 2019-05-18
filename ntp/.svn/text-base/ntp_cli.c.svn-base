#include "zebra.h"
#include "lib/vty.h"
#include "lib/log.h"
#include "lib/memory.h"

#include "lib/msg_ipc_n.h"

#include <math.h>
#include "ntp_ntpclient.h"
#ifndef _ZEBOS_NTP_H
#include "ntp.h"
#define _ZEBOS_NTP_H
#endif

#ifndef __ZEBOS_NTP_CLI_H__
#include "ntp_cli.h"
#define __ZEBOS_NTP_CLI_H__
#endif


#ifndef _ZEBRA_COMMAND_H
#include "command.h"
#define _ZEBRA_COMMAND_H
#endif

static struct cmd_node stNtpNode = { NTP_NODE,  "%s(config-ntp)#",  1 };
const char *ntpEnableStri[] = {"disable",  "enable"};
const char *ntpNtpserverState[] = { "Not synchronized", "synchronized"};
static int iModeFlag = 0;


static void ntp_netif_iplongtostr(char *pcIPStr, int iBufLen, const uint32_t uiIP)
{
    if (NULL != pcIPStr)
    {
        snprintf(pcIPStr, iBufLen, "%d.%d.%d.%d",
                 (int)((uiIP >> 24) & 0xff), (int)((uiIP >> 16) & 0xff), (int)((uiIP >> 8) & 0xff), (int)(uiIP & 0xff));
    }

    return;
}

int ntp_check_ip_valid(uint32_t uiIPAddr)
{
    uint8_t a1 = 0, a2 = 0, a3 = 0, a4 = 0;

    a1 = (uint8_t)((uiIPAddr & 0xff000000) >> 24);
    a2 = (uint8_t)((uiIPAddr & 0x00ff0000) >> 16);
    a3 = (uint8_t)((uiIPAddr & 0x0000ff00) >> 8);
    a4 = (uint8_t)((uiIPAddr & 0x000000ff) >> 0);

    if (0 == a1)
    {
        return 0;
    }
    else if ((0 == a4))
    {
        return 0;
    }
    else if (255 == a1 && 255 == a2 && 255 == a3 && 255 == a4)
    {
        return 0;
    }
    else if (a1 >= 224 && a1 <= 239)
    {
        return 0;
    }
    else if (a1 >= 240 && a1 <= 254)
    {
        return 0;
    }
    else if (127 == a1)
    {
        return 0;
    }

    return 1;
}

static uint32_t ntp_ipstr_to_int(const char *pcIPStr)
{
    int   lIndex  = 0;
    int   lStrLen = 0;

    char *pTpHead = NULL;
    char *pTpFind = NULL;
    char  bStrBuf[32];

    unsigned long uIpAddr[4];

    if (NULL == pcIPStr)
    {
        return (-1);
    }

    lStrLen  = strlen(pcIPStr);

    if ((lStrLen < 7) || (lStrLen > 15))
    {
        return (-1);
    }

    for (lIndex = 0; lIndex < lStrLen; lIndex++)
    {
        if (!((pcIPStr[lIndex] == '.') ||
                ((pcIPStr[lIndex] >= '0') && (pcIPStr[lIndex] <= '9'))))
        {
            return (-1);
        }
    }

    pTpFind = bStrBuf;

    strcpy(bStrBuf, pcIPStr);

    for (lIndex = 0; lIndex < 4; lIndex++)
    {
        pTpHead = pTpFind;

        if (lIndex < 3)
        {
            pTpFind = (char *)strchr(pTpHead, '.');

            if (!pTpFind)
            {
                return (-1);
            }

            *pTpFind = 0;
            pTpFind ++;
        }

        lStrLen = strlen(pTpHead);

        if ((lStrLen < 1) || (lStrLen > 3))
        {
            return (-1);
        }

        uIpAddr[lIndex] = atol(pTpHead);

        if (uIpAddr[lIndex] > 255)
        {
            return (-1);
        }
    }

    return ((uIpAddr[0] << 24) | (uIpAddr[1] << 16) | (uIpAddr[2] << 8) | uIpAddr[3]);
}
static void ntp_clean_pkt_cnt(void)
{
    ntp_global.uiRecvCSpacket = 0;
    ntp_global.uiRecvPeerPacket = 0;
    ntp_global.uiSendCSPacket = 0;
    ntp_global.uiSendPeerPacket = 0;
}
static int ntp_serverip_set(char *pcIPAddr)
{
    uint32_t uiIPAddr;

    uiIPAddr = ntp_ipstr_to_int(pcIPAddr);

    if (ntp_check_ip_valid(uiIPAddr) != 1)
    {
        return 0;
    }

    ntp_global.uiServerIP = uiIPAddr;

    return 1;
}
#if 0
/* 注册要接收的协议报文类型 */
static void ntp_unicastpkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = NTP_PORT;
    pkt_register(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    ntp_unicast_pkt_rcv();

    return;
}

/* 注册要接收的协议报文类型 */
static void ntp_broadcastpkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dip  = 0xffffffff;
    proto.ipreg.dport = NTP_PORT;
    pkt_register(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    ntp_broadcast_pkt_rcv();

    return;
}

static void ntp_unicastpkt_unregister()
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = NTP_PORT;
    pkt_unregister(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}

static void ntp_broadcastpkt_unregister()
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dip  = 0xffffffff;
    proto.ipreg.dport = NTP_PORT;
    pkt_unregister(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}
#endif
/********************************************** debug ***********************************************/

const struct message ntp_debug_name[] =
{
	{.key = NTP_DBG_COMMON,		.str = "common"},
	{.key = NTP_DBG_EVENT,		.str = "event"},
	{.key = NTP_DBG_INFO,		    .str = "info"},
	{.key = NTP_DBG_PACKAGE,		.str = "package"},
	{.key = NTP_DBG_ERROR,		.str = "error"},
	{.key = NTP_DBG_ALL,		    .str = "all"}
};
    
extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

DEFUN (ntp_debug_monitor,
	ntp_debug_monitor_cmd,
	"debug ntp (enable|disable) (common|event|info|package|error|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common messege\n"
	"Type name of event messege\n"
	"Type name of info messege\n"
	"Type name of package messege \n"
	"Type name of error messege \n"
	"Type name of all debug\n")
{
	unsigned int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(ntp_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], ntp_debug_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, ntp_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* 显示个debug信息的状态  */
DEFUN (show_ntp_debug_monitor,
	show_ntp_debug_monitor_cmd,
	"show debug ntp",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	unsigned int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(ntp_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", ntp_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/

DEFUN(show_ntp,
      show_ntpconfig_cmd,
      "show ntp config",
      "show command\n"
      "ntp command\n"
      "show ntp config information\n"
     )
{
    char bIpStr[32] = {0};

    vty_out(vty, "====================================%s", VTY_NEWLINE);
    vty_out(vty, "ntp client            %s%s", ntpEnableStri[ntp_global.uiClientEnable % 2], VTY_NEWLINE);
    ntp_netif_iplongtostr(bIpStr, sizeof(bIpStr), ntp_global.uiServerIP);

    vty_out(vty, "ntp mode             %s%s", (CLINETMODE == ntp_global.uiWorkMode) ? "client-server" : "p2p", VTY_NEWLINE);
    vty_out(vty, "ntp %s               %s %s", (CLINETMODE == ntp_global.uiWorkMode) ? "server" : "peer  ", bIpStr, VTY_NEWLINE);
    vty_out(vty, "ntp packet           %s %s", ntp_global.ucPacketFormat, VTY_NEWLINE);
    vty_out(vty, "ntp sync-interval    %d s%s", ntp_global.uiSyncInterval, VTY_NEWLINE);
    vty_out(vty, "ntp version          %d %s", ntp_global.uiVersion, VTY_NEWLINE);
    vty_out(vty, "ntp VPN ID of packet %d %s", ntp_global.uiPacketVpnID, VTY_NEWLINE);


    return CMD_SUCCESS;
}

DEFUN(show_ntpstatus,
      show_ntpstatus_cmd,
      "show ntp status",
      "show command\n"
      "ntp command\n"
      "show the status of ntp protocol\n"
     )
{
    int32_t timeOffset = 0;
    int32_t timeDelay = 0;

    vty_out(vty, "====================================%s", VTY_NEWLINE);

    vty_out(vty, "clock status       : %s%s", ntpNtpserverState[ntp_global.uiSyncState], VTY_NEWLINE);
    vty_out(vty, "clock stratum      : %d%s", ntp_global.uiStratum, VTY_NEWLINE);

    if (strcmp("broadcast", (char *)ntp_global.ucPacketFormat) == 0)
    {
        vty_out(vty, "offset             : %.2f%s%s", 0.00, " ms", VTY_NEWLINE);
        vty_out(vty, "delay              : %.2f%s%s", 0.00, " ms", VTY_NEWLINE);
    }
    else
    {
        timeOffset = (ntp_global.stTimeOffset.tv_sec * 1000000) + (ntp_global.stTimeOffset.tv_usec);
        vty_out(vty, "offset             : %d.%d%s%s", timeOffset / 1000, abs(timeOffset % 1000), " ms", VTY_NEWLINE);
        timeDelay = (ntp_global.stTimeDelay.tv_sec * 1000000) + (ntp_global.stTimeDelay.tv_usec);
        vty_out(vty, "delay              : %d.%d%s%s", timeDelay / 1000, abs(timeDelay % 1000), " ms", VTY_NEWLINE);
    }

    vty_out(vty, "reference clock ID : %s%s", ntp_global.ucReferID, VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(show_ntpstatistics,
      show_ntpstatistics_cmd,
      "show ntp statistics",
      "show command\n"
      "ntp command\n"
      "show ntp message statistics\n"
     )
{
    vty_out(vty, "                     %s         %s %s", "client-server", "peer", VTY_NEWLINE);
    //vty_out (vty, "%s", VTY_NEWLINE);
    vty_out(vty, "the number of sent:        %-10d       %-10d%s", ntp_global.uiSendCSPacket, ntp_global.uiSendPeerPacket, VTY_NEWLINE);
    vty_out(vty, "the number of recv:        %-10d       %-10d%s", ntp_global.uiRecvCSpacket, ntp_global.uiRecvPeerPacket, VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(set_ntp_fun,
      set_ntp_cmd,
      "ntp client (enable|disable)",
      "ntp command\n"
      "set ntp client enable or disable\n"
      "enable ntp\n"
      "disable ntp\n")
{
    unsigned int iEnable = 0;
    int iRet = 0;

    /* if (strcmp(argv[0], "enable") == 0)
     {
         iEnable = 1;
     }*/

    iEnable = (strncmp(argv[0], "enable", 1) == 0) ? 1 : 0;

    if (iEnable == ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%ntp already %s %s", ntpEnableStri[iEnable], VTY_NEWLINE);
        return CMD_SUCCESS;
    }
    else
    {
        ntp_clean_pkt_cnt();

        if (1 == iEnable)
        {
            /* 广播报文 */
            if (strcmp("broadcast", (char *)ntp_global.ucPacketFormat) == 0)
            {
                //ntp_sync_interval();
                if (MODE_CLIENT == ntp_global.uiWorkMode) //客户端模式
                {
                    ntp_global.uiClientEnable = 1;

                    ntp_client_enable(NULL);

                    return CMD_SUCCESS;
                }
                else
                {
                    vty_warning_out(vty, "%%when set p2p mode need set packet mode unicast firstly %s", VTY_NEWLINE);
                    return CMD_WARNING;
                }
            }

            /* 单播报文 */
            if (MODE_CLIENT == ntp_global.uiWorkMode)//客户端模式
            {
                if (0 == iModeFlag)
                {
                    iRet = 1;
                }
            }
            else
            {
                if (1 == iModeFlag)
                {
                    iRet = 1;
                }
            }

            if (1 != iRet)
            {
                vty_warning_out(vty, "%%Need to configure the server/peer address firstly %s", VTY_NEWLINE);
                return CMD_WARNING;
            }

            if (1 == ntp_check_ip_valid(ntp_global.uiServerIP))
            {
                ntp_global.uiClientEnable = 1;
                //ntp_sync_interval();

                ntp_client_enable(NULL);
            }
            else
            {
                vty_warning_out(vty, "%%The server/peer address is not correct %s", VTY_NEWLINE);
            }
        }
        else/* 去使能并且恢复到默认值 */
        {
            ntp_global.uiClientEnable = 0;
            /*if (strcmp("unicast", (char *)ntp_global.ucPacketFormat) == 0)
            {
                ntp_unicastpkt_unregister();
            }
            else
            {
                ntp_broadcastpkt_unregister();
            }*/

            ntp_client_disable();
        }
    }

    return CMD_SUCCESS;
}

DEFUN(set_ntpserver_fun,
      set_ntpserver_cmd,
      "ntp (server|peer) A.B.C.D [vpn-instance <1-128>]",
      "ntp command\n"
      "set ntp server ip address\n"
      "set ntp peer ip address\n"
      "input host ipv4 address\n"
      "L3 vpn instance, only for log server\n" 
      "L3 vpn tag\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (MODE_CLIENT == ntp_global.uiWorkMode)
    {
        if (strcmp("peer", argv[0]) == 0)
        {
            vty_warning_out(vty, "%%current mode is client-server mode can't set peer address%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        if (strcmp("server", argv[0]) == 0)
        {
            vty_warning_out(vty, "%%current mode is p2p mode can't set server address%s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if (ntp_serverip_set((char *)argv[1]) == 0)
    {
        vty_warning_out(vty, "%%invalid ip address %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else
    {
        if (strcmp(argv[0], "server") == 0)
        {
            iModeFlag = 0;
        }
        else
        {
            iModeFlag = 1;
        }
        if(argv[2] != NULL)
        {
            ntp_global.uiPacketVpnID = atoi(argv[2]);
        }
        else
        {
            ntp_global.uiPacketVpnID = 0;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_set_server_fun,
      no_set_ntpserver_cmd,
      "no ntp (server|peer)",
      NO_STR
      "ntp command\n"
      "unset ntp server ip address\n"
      "unset ntp peer ip address\n"
     )
{
    int iRet = 0;

    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (0 == iModeFlag)
    {
        if (strcmp("peer", argv[0]) == 0)
        {
            iRet = -1;
        }
    }
    else
    {
        if (strcmp("server", argv[0]) == 0)
        {
            iRet = -1;
        }
    }

    if (0 == iRet)
    {
        ntp_global.uiServerIP = 0;
        ntp_global.uiPacketVpnID = 0;
    }
    else
    {
        vty_warning_out(vty, "%%configure failed %s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(ntp_name,
      ntp_name_cmd,
      "ntp",
      NO_STR
      "enter the ntp view\n")
{
    vty->node = NTP_NODE;

    return CMD_SUCCESS;
}

DEFUN(ntp_mode,
      ntp_mode_cmd,
      "ntp mode (p2p|client-server)",
      "ntp command\n"
      "set ntp working mode\n"
      "peer mode\n"
      "client-server mode\n"
     )
{
    uint32_t uiMode = 0;

    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (strcmp(argv[0], "p2p") == 0)
    {
        uiMode = MODE_ACTIVE;
    }
    else
    {
        uiMode = MODE_CLIENT;
    }

    if (uiMode != ntp_global.uiWorkMode)
    {
        ntp_global.uiServerIP = 0;

        if (uiMode == MODE_ACTIVE)
        {
            ntp_global.uiWorkMode = MODE_ACTIVE;
        }
        else
        {
            ntp_global.uiWorkMode = MODE_CLIENT;
        }
    }

    return CMD_SUCCESS;
}

DEFUN(no_ntp_mode,
      no_ntp_mode_cmd,
      "no ntp mode",
      NO_STR
      "ntp command\n"
      "unset ntp working mode\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ntp_global.uiWorkMode = MODE_CLIENT;

    return CMD_SUCCESS;
}

DEFUN(ntp_packet,
      ntp_packet_cmd,
      "ntp packet (unicast|broadcast)",
      "ntp command\n"
      "set ntp message format\n"
      "unicast format message\n"
      "broadcast format message\n"
     )
{
    char buf[15] = "";

    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (argv[0] != NULL)
    {
        strncpy(buf, argv[0], sizeof(buf));
    }

    if ((strcmp(buf, "broadcast") == 0) && (ntp_global.uiWorkMode != CLINETMODE))
    {
        return vty_warning_out(vty, "%%ntp packet broadcast is work under client-server mode %s", VTY_NEWLINE);
    }

    if (strcmp(buf, (char *)ntp_global.ucPacketFormat) != 0)
    {
        strncpy((char *)ntp_global.ucPacketFormat, buf, sizeof(ntp_global.ucPacketFormat));
    }
    else
    {
        return vty_warning_out(vty, "%%ntp packet already is %s %s", ntp_global.ucPacketFormat, VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(no_ntp_packet,
      no_ntp_packet_cmd,
      "no ntp packet",
      NO_STR
      "ntp command\n"
      "unset format message\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    strncpy((char *)ntp_global.ucPacketFormat, "unicast", sizeof(ntp_global.ucPacketFormat));

    return CMD_SUCCESS;
}

DEFUN(ntp_syncinterval,
      ntp_syncinterval_cmd,
      "ntp sync-interval <1-1000>",
      "ntp command\n"
      "set ntp synchronization time interval\n"
      "the value of synce-interval(second)\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (atoi(argv[0]) < 1 || atoi(argv[0]) > 1000)
    {
        return CMD_WARNING;
    }

    ntp_global.uiSyncInterval = atoi(argv[0]);

    return CMD_SUCCESS;
}

DEFUN(no_ntp_syncinterval,
      no_ntp_syncinterval_cmd,
      "no ntp sync-interval",
      NO_STR
      "ntp command\n"
      "unset sync-interval\n")
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ntp_global.uiSyncInterval = 10;

    return CMD_SUCCESS;
}

DEFUN(ntp_version,
      ntp_version_cmd,
      "ntp version <1-4>",
      "ntp command\n"
      "set ntp version\n"
      "the value of version\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (atoi(argv[0]) < 1 || atoi(argv[0]) > 4)
    {
        return CMD_WARNING;
    }

    ntp_global.uiVersion = atoi(argv[0]);

    return CMD_SUCCESS;
}

DEFUN(no_ntp_version,
      no_ntp_version_cmd,
      "no ntp version",
      NO_STR
      "ntp command\n"
      "unset ntp version\n"
     )
{
    if (NTPDISABLE != ntp_global.uiClientEnable)
    {
        vty_warning_out(vty, "%%modify configuration in ntp disable state%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ntp_global.uiVersion = 4;

    return CMD_SUCCESS;
}

int ntp_config_write(struct vty *vty)
{
    char bTmpNtpserver[32];
    char ntpMode[15];

    vty_out(vty, "ntp%s", VTY_NEWLINE);

    if (NTP_CONFIG_DEFAULT_MODE != ntp_global.uiWorkMode)
    {
        strncpy(ntpMode, "p2p", sizeof(ntpMode));
        vty_out(vty, " ntp mode %s%s", ntpMode, VTY_NEWLINE);
    }

    if (ntp_global.uiSyncInterval != NTP_CONFIG_DEFAULT_INTERVAL)
    {
        vty_out(vty, " ntp sync-interval %d%s", ntp_global.uiSyncInterval, VTY_NEWLINE);
    }

    if (ntp_global.uiVersion != NTP_CONFIG_DEFAULT_VERSION)
    {
        vty_out(vty, " ntp version %d%s", ntp_global.uiVersion, VTY_NEWLINE);
    }

    if (strcmp((char *)ntp_global.ucPacketFormat, NTP_CONFIG_DEFAULT_PACK_TYPE) != 0)
    {
        vty_out(vty, " ntp packet %s%s", ntp_global.ucPacketFormat, VTY_NEWLINE);
    }

    if (ntp_check_ip_valid(ntp_global.uiServerIP) == 1)
    {
        ntp_netif_iplongtostr(bTmpNtpserver, sizeof(bTmpNtpserver), ntp_global.uiServerIP);

        if (0 == iModeFlag)
        {
            if(ntp_global.uiPacketVpnID == 0)
            {
                vty_out(vty, " ntp server %s%s", bTmpNtpserver, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " ntp server %s vpn-instance %d%s", bTmpNtpserver,ntp_global.uiPacketVpnID, VTY_NEWLINE);
            }
        }
        else
        {
            if(ntp_global.uiPacketVpnID == 0)
            {
                vty_out(vty, " ntp peer %s%s", bTmpNtpserver, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " ntp peer %s vpn-instance %d%s", bTmpNtpserver,ntp_global.uiPacketVpnID, VTY_NEWLINE);
            }
        }
    }

    if (ntp_global.uiClientEnable == NTPENABLE)
    {
        vty_out(vty, " ntp client %s%s", ntpEnableStri[ntp_global.uiClientEnable % 2], VTY_NEWLINE);
    }

    return 0;
}

void ntp_cmd_init(void)
{
    install_node(&stNtpNode, ntp_config_write);
    install_default(NTP_NODE);

    install_element(CONFIG_NODE, &ntp_name_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &show_ntpconfig_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ntpstatus_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ntpstatistics_cmd, CMD_LOCAL);

    install_element(NTP_NODE, &ntp_mode_cmd, CMD_SYNC);
    install_element(NTP_NODE, &no_ntp_mode_cmd, CMD_SYNC);
    install_element(NTP_NODE, &ntp_packet_cmd, CMD_SYNC);
    install_element(NTP_NODE, &no_ntp_packet_cmd, CMD_SYNC);
    install_element(NTP_NODE, &ntp_syncinterval_cmd, CMD_SYNC);
    install_element(NTP_NODE, &no_ntp_syncinterval_cmd, CMD_SYNC);
    install_element(NTP_NODE, &ntp_version_cmd, CMD_SYNC);
    install_element(NTP_NODE, &no_ntp_version_cmd, CMD_SYNC);
    install_element(NTP_NODE, &set_ntp_cmd, CMD_SYNC);
    install_element(NTP_NODE, &set_ntpserver_cmd, CMD_SYNC);
    install_element(NTP_NODE, &no_set_ntpserver_cmd, CMD_SYNC);
    install_element(NTP_NODE, &show_ntpconfig_cmd, CMD_LOCAL);
    install_element(NTP_NODE, &show_ntpstatus_cmd, CMD_LOCAL);
    install_element(NTP_NODE, &show_ntpstatistics_cmd, CMD_LOCAL);
	/* new debug */
	install_element (CONFIG_NODE, &ntp_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_ntp_debug_monitor_cmd, CMD_SYNC);

    return;
}

