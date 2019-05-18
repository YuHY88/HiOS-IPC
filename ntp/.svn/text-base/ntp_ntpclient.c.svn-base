#include "thread.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <lib/ether.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include "lib/log.h"

#include "lib/msg_ipc_n.h"
#include "lib/module_id.h"
#include "lib/devm_com.h"
#include "lib/errcode.h"
#include <lib/hptimer.h>



#include "lib/inet_ip.h"
#include "ftm/pkt_ip.h"
#include <ftm/pkt_udp.h>

#include "ntp.h"
#include "ntp_ntpclient.h"
#include "ntp_cli.h"

#define NTP_DEFAUT_STRATUM 16
#define NTP_SYNC_SUCCESS   0
#define NTP_SYNC_FAIL      3

extern struct thread_master *ntp_master;
TIMERID ntptimer = 0;
struct thread *synctimethread = NULL;

struct timeval server_send_time;
struct timeval client_response_time;
struct timeval last_server_time;
struct timeval client_send_time;
struct timeval passive_send_time;
struct timeval timeori;
struct timeval timerecv;
struct timeval recv_broadcast_time;
struct timeval broadcast_time;
struct timeval last_bdcast_time;
static uint32_t uiIPAddr = 0;
static uint32_t uiCount = 0;
static int iActiveFlag = 0;
static int iClientFlag = 0;
static int iBroadFlag = 0;
static int iRtcSetFlag = 0;
//static uint32_t ntp_global.uiStratum = NTP_DEFAUT_STRATUM;
ntp_globals    ntp_global;


static void ntp_sig_alarm()
{
    uiIPAddr = 0;
    uiCount = 0;
    alarm(0);

    return;
}

static void ntp_broad_alarm()
{
    iBroadFlag = 0;
    uiIPAddr = 0;
    alarm(0);

    return;
}

#if 0
static int sysm_rtc_get_time(int *piYear, int *piMonth,  int *piDay,
                             int *piHour, int *piMinute, int *piSecond)
{
    int fd = -1;
    int ret = 0;
    struct rtc_time  sRtcTime;

    fd = open("/dev/rtc", O_RDONLY);

    if (fd < 0)
    {
        fd = open("/dev/rtc0", O_RDONLY);

        if (fd < 0)
        {
            zlog_debug("open /dev/rtc  error and device rtc clock sync failed\n");
            close(fd);
            return (-1);
        }
    }

    ret = ioctl(fd, RTC_RD_TIME, &sRtcTime);

    if (ret < 0)
    {
        zlog_debug("ioctl error\n");
        close(fd);
        return (-1);
    }

    if (piSecond)
    {
        *piSecond = sRtcTime.tm_sec;
    }

    if (piMinute)
    {
        *piMinute = sRtcTime.tm_min;
    }

    if (piHour)
    {
        *piHour   = sRtcTime.tm_hour;
    }

    if (piDay)
    {
        *piDay    = sRtcTime.tm_mday;
    }

    if (piMonth)
    {
        *piMonth  = sRtcTime.tm_mon;
    }

    if (piYear)
    {
        *piYear   = sRtcTime.tm_year;
    }

    close(fd);
    return (0);

}

static int sysm_rtc_get_systime(struct timeval *pstSetTime)
{
    struct tm sTmTime;
    //struct tm *tblock;
    //struct tm *time;

    int           iValues[6] = {0};
    sysm_rtc_get_time(&iValues[0], &iValues[1],  &iValues[2], &iValues[3], &iValues[4], &iValues[5]);

    memset(&sTmTime, 0, sizeof(struct tm));
    sTmTime.tm_year = iValues[0] - 1900;
    sTmTime.tm_mon  = iValues[1] - 1;
    sTmTime.tm_mday = iValues[2];
    sTmTime.tm_hour = iValues[3];
    sTmTime.tm_min  = iValues[4];
    sTmTime.tm_sec  = iValues[5];

    pstSetTime->tv_sec  = mktime(&sTmTime);
    pstSetTime->tv_usec = 0;

    return 0;
}


static int sysm_rtc_set_time(int  iYear, int  iMonth,  int  iDay,
                             int  iHour, int  iMinute, int  iSecond)
{
    int fd = -1;
    int ret = 0;

    fd = open("/dev/rtc", O_WRONLY);

    if (fd < 0)
    {
        fd = open("/dev/rtc0", O_WRONLY);

        if (fd < 0)
        {
            zlog_debug("open /dev/rtc  error and device rtc clock sync failed\n");
            close(fd);
            return (-1);
        }
    }

    struct rtc_time  sRtcTime;

    sRtcTime.tm_sec   = iSecond;

    sRtcTime.tm_min   = iMinute;

    sRtcTime.tm_hour  = iHour;

    sRtcTime.tm_mday  = iDay;

    sRtcTime.tm_mon   = iMonth;

    sRtcTime.tm_year  = iYear;

    sRtcTime.tm_wday  = 0;

    sRtcTime.tm_yday  = 0;

    sRtcTime.tm_isdst = 0;

    ret = ioctl(fd, RTC_SET_TIME, &sRtcTime);

    if (ret < 0)
    {
        zlog_debug("/dev/rtc ioctl error\n");
        close(fd);
        return (-1);
    }

    close(fd);
    return (0);

}
#endif
static int ntp_get_timezone(int *zone)
{
    FILE *fp;
    char buf[128];
    char *s = NULL;
    char data[32];
    fp = fopen(PATH_LOCALTIME, "rb");

    if (fp == NULL)
    {
        zlog_debug(NTP_DBG_ERROR,"%s, %d open file failed!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    fgets(buf, sizeof(buf), fp);
    s = strstr(buf + 50, "GMT");

    if (s == NULL)
    {
        zlog_debug(NTP_DBG_ERROR,"%s, %d error s == NULL !\n", __FUNCTION__, __LINE__);
        fclose(fp);
        return -1;
    }

    memcpy(data, s + 3, 3);
    *zone = atoi(data);
    fclose(fp);
    return 0;
}
static int ntp_localtime_modify(struct timeval *new_time)
{
    time_t tm_sec;
    int    timezone = 0;
    tm_sec = new_time->tv_sec - YEARS_1970_to_2000_SEC;

    if (iRtcSetFlag < 3) /* 时间同步后 写三次RTC芯片*/
    {
        ntp_get_timezone(&timezone);
        tm_sec -= (time_t)(timezone * 3600);
        devm_comm_set_rtc(MODULE_ID_SYSTEM, 1, tm_sec);
        iRtcSetFlag++;
    }

    settimeofday(new_time, NULL);
    return 0;
}

static void ntp_time_resolve(struct sNtpPacket *pstPacket,
                             struct timeval *server_send_time,
                             struct timeval *server_recv_time)
{
    server_send_time->tv_sec = ntohl(pstPacket->uiTrantimestampInterger) - JAN_1970;
    server_send_time->tv_usec = USEC(ntohl(pstPacket->uiTrantimestampFractional));

    server_recv_time->tv_sec = ntohl(pstPacket->uiRecvtimestampInterger) - JAN_1970;
    server_recv_time->tv_usec = USEC(ntohl(pstPacket->uiRecvtimestampFractional));

    return;
}

static void ntp_get_data(struct sNtpPacket *pstPacket)
{
    struct timeval server_recv_time;
    struct timeval client_revc_time;
    struct timeval time_tmp1, time_tmp2;

    ntp_time_resolve(pstPacket, &server_send_time, &server_recv_time);
    gettimeofday(&client_revc_time, NULL);

    time_tmp1.tv_sec = server_recv_time.tv_sec - client_send_time.tv_sec;
    time_tmp1.tv_usec = server_recv_time.tv_usec - client_send_time.tv_usec;
    time_tmp2.tv_sec = server_send_time.tv_sec - client_revc_time.tv_sec;
    time_tmp2.tv_usec = server_send_time.tv_usec - client_revc_time.tv_usec;

    /* time offset*/
    ntp_global.stTimeOffset.tv_sec = (time_tmp1.tv_sec >> 1) + (time_tmp2.tv_sec >> 1);
    ntp_global.stTimeOffset.tv_usec = (time_tmp1.tv_usec >> 1) + (time_tmp2.tv_usec >> 1);

    if (ntp_global.stTimeOffset.tv_usec >= TV_USEC_PER_SEC)
    {
        ntp_global.stTimeOffset.tv_usec -= TV_USEC_PER_SEC;
        ntp_global.stTimeOffset.tv_sec++;
    }

    /* time delay */
    time_tmp1.tv_sec = client_revc_time.tv_sec - client_send_time.tv_sec;
    time_tmp1.tv_usec = client_revc_time.tv_usec - client_send_time.tv_usec;
    time_tmp2.tv_sec = server_send_time.tv_sec - server_recv_time.tv_sec;
    time_tmp2.tv_usec = server_send_time.tv_usec - server_recv_time.tv_usec;

    ntp_global.stTimeDelay.tv_sec = time_tmp1.tv_sec - time_tmp2.tv_sec;
    ntp_global.stTimeDelay.tv_usec = time_tmp1.tv_usec - time_tmp2.tv_usec;

    if (ntp_global.stTimeDelay.tv_usec < 0)
    {
        ntp_global.stTimeDelay.tv_usec += TV_USEC_PER_SEC;
        ntp_global.stTimeDelay.tv_sec--;
    }

    return;
}

static void ntp_time_record(struct sNtpPacket *pstPacket)
{
    if (MODE_CLIENT == ntp_global.uiWorkMode)
    {
        iClientFlag = 1;
    }
    else
    {
        iActiveFlag = 1;
    }

    gettimeofday(&timerecv, NULL);

    timeori.tv_sec = ntohl(pstPacket->uiTrantimestampInterger);
    timeori.tv_usec = ntohl(pstPacket->uiTrantimestampFractional);
}

static void ntp_time_offset(struct sNtpPacket *pstPacket)
{
    struct timeval server_recv_time;
    struct timeval client_revc_time;
    struct timeval time_tmp1, time_tmp2;

    ntp_time_resolve(pstPacket, &client_response_time, &server_recv_time);
    gettimeofday(&client_revc_time, NULL);

    time_tmp1.tv_sec = server_recv_time.tv_sec - passive_send_time.tv_sec;
    time_tmp1.tv_usec = server_recv_time.tv_usec - passive_send_time.tv_usec;
    time_tmp2.tv_sec = client_response_time.tv_sec - client_revc_time.tv_sec;
    time_tmp2.tv_usec = client_response_time.tv_usec - client_revc_time.tv_usec;

    ntp_global.stTimeOffset.tv_sec = (time_tmp1.tv_sec >> 1) + (time_tmp2.tv_sec >> 1);
    ntp_global.stTimeOffset.tv_usec = (time_tmp1.tv_usec >> 1) + (time_tmp2.tv_usec >> 1);

    if (ntp_global.stTimeOffset.tv_usec >= TV_USEC_PER_SEC)
    {
        ntp_global.stTimeOffset.tv_usec -= TV_USEC_PER_SEC;
        ntp_global.stTimeOffset.tv_sec++;
    }

    /* time delay */
    time_tmp1.tv_sec = client_revc_time.tv_sec - client_send_time.tv_sec;
    time_tmp1.tv_usec = client_revc_time.tv_usec - client_send_time.tv_usec;

    time_tmp2.tv_sec = server_send_time.tv_sec - server_recv_time.tv_sec;
    time_tmp2.tv_usec = server_send_time.tv_usec - server_recv_time.tv_usec;

    ntp_global.stTimeDelay.tv_sec = time_tmp1.tv_sec - time_tmp2.tv_sec;
    ntp_global.stTimeDelay.tv_usec = time_tmp1.tv_usec - time_tmp2.tv_usec;

    if (ntp_global.stTimeDelay.tv_usec < 0)
    {
        ntp_global.stTimeDelay.tv_usec += TV_USEC_PER_SEC;
        ntp_global.stTimeDelay.tv_sec--;
    }

    return;
}

static void ntp_send_response_packet(struct sNtpPacket *pstPacket, uint32_t uiDestIP, uint16_t usDestPort)
{
    int iRet;
    union  pkt_control pktcontrol;
    struct timeval now;
    struct sNtpPacket stNtpPacket;

    memset(&pktcontrol, 0, sizeof(pktcontrol));
    memset(&stNtpPacket, 0, sizeof(struct sNtpPacket));
    gettimeofday(&now, NULL);

    stNtpPacket.uiRecvtimestampInterger = htonl(now.tv_sec + JAN_1970);
    stNtpPacket.uiRecvtimestampFractional = htonl(now.tv_usec);
    stNtpPacket.uiOritimestampInterger =  htonl(pstPacket->uiTrantimestampInterger);
    stNtpPacket.uiOritimestampFractional = htonl(pstPacket->uiTrantimestampFractional);

    stNtpPacket.ucLiVnMode = (LI << 6) | (PKT_VERSION(pstPacket->ucLiVnMode) << 3) | (MODE_PASSIVE);
    stNtpPacket.ucStratum = ntp_global.uiStratum;
    stNtpPacket.ucPoll = pstPacket->ucPoll;
    stNtpPacket.uiRootDelay = 0;
    stNtpPacket.cPrecision = PREC & 0xff;

    gettimeofday(&now, NULL);

    passive_send_time.tv_sec = now.tv_sec;
    passive_send_time.tv_usec = now.tv_usec;
    stNtpPacket.uiTrantimestampInterger = htonl(now.tv_sec + JAN_1970);
    stNtpPacket.uiTrantimestampFractional = htonl(now.tv_usec);

    pktcontrol.ipcb.dip = uiDestIP;
    pktcontrol.ipcb.sport = NTP_PORT;
    pktcontrol.ipcb.dport = usDestPort;
    pktcontrol.ipcb.ttl = 64;
    pktcontrol.ipcb.vpn = 0;
    pktcontrol.ipcb.tos = 5;
    pktcontrol.ipcb.chsum_enable = 1;

    iRet = pkt_send(PKT_TYPE_UDP, &pktcontrol, &stNtpPacket, sizeof(struct sNtpPacket));

    if (iRet == NET_FAILED)
    {
        zlog_debug(NTP_DBG_ERROR,"%s, %d send response ntp message failed!\n", __FUNCTION__, __LINE__);
    }
    else
    {
        ntp_global.uiSendPeerPacket++;
    }

    return;
}
#if 0
static void ntp_time_delay(struct sNtpPacket *pstPacket)
{
    struct timeval client_revc_time;
    struct timeval server_send_time;
    struct timeval server_recv_time;
    struct timeval time_tmp1, time_tmp2;

    ntp_time_resolve(pstPacket, &server_send_time, &server_recv_time);

    gettimeofday(&client_revc_time, NULL);

    /*time trans delay = (T4-T1) - (T3-T2)*/
    time_tmp1.tv_sec = client_revc_time.tv_sec - client_send_time.tv_sec;
    time_tmp1.tv_usec = client_revc_time.tv_usec - client_send_time.tv_usec;

    time_tmp2.tv_sec = server_send_time.tv_sec - server_recv_time.tv_sec;
    time_tmp2.tv_usec = server_send_time.tv_usec - server_recv_time.tv_usec;


    return;
}
#endif
static void ntp_server_message(struct sNtpPacket *pstPacket, uint32_t uiSrcIP)
{
    uint32_t uisrcIP_net = 0;

    if ((ntp_global.uiVersion != PKT_VERSION(pstPacket->ucLiVnMode)) ||
            (pstPacket->ucStratum > ntp_global.uiStratum))
    {
        return;
    }

    uisrcIP_net = htonl(uiSrcIP);

    if (ntp_global.uiServerIP == uiSrcIP)
    {
        ntp_global.uiStratum = pstPacket->ucStratum + 1;
        ntp_time_record(pstPacket);
        inet_ntop(AF_INET, &uisrcIP_net, (char *)ntp_global.ucReferID, sizeof(ntp_global.ucReferID));
        ntp_get_data(pstPacket);

        ntp_global.uiSyncFlag = NTP_SYNC_SUCCESS;
    }

    return;
}

static void ntp_broadcast_message(struct sNtpPacket *pstPacket, uint32_t uiSrcIP, uint16_t usSrcPort)
{
    double time;
    uint32_t uiSrcIP_net = 0;

    time = pow(2, pstPacket->ucPoll);

    if (iBroadFlag == 0)
    {
        uiIPAddr = uiSrcIP;
        signal(SIGALRM, ntp_broad_alarm);
        alarm((uint32_t)time + 5);
    }

    uiSrcIP_net = htonl(uiSrcIP);

    if (uiIPAddr == uiSrcIP)
    {
        alarm((uint32_t)time + 5);
        iBroadFlag = 1;

        inet_ntop(AF_INET, &uiSrcIP_net, (char *)ntp_global.ucReferID, sizeof(ntp_global.ucReferID));
        ntp_global.stBroadcastTime.tv_sec = ntohl(pstPacket->uiTrantimestampInterger) - JAN_1970;
        ntp_global.stBroadcastTime.tv_usec = ntohl(pstPacket->uiTrantimestampFractional);

        ntp_global.uiStratum = pstPacket->ucStratum + 1;
        gettimeofday(&recv_broadcast_time, NULL);

        ntp_global.uiSyncFlag = NTP_SYNC_SUCCESS;
    }

    return;
}

static void ntp_active_message(struct sNtpPacket *pstPacket, uint32_t uiSrcIP, uint16_t usSrcPort)
{
    double time;
    //struct timeval newtime;
    //struct timeval now;
    //struct timeval diff;
    //long int diffMs = 0;
    //int iRet = 0;
    uint32_t uiSrcIP_net = 0;
    time = pow(2, pstPacket->ucPoll);
    ntp_send_response_packet(pstPacket, uiSrcIP, usSrcPort);

    if (uiCount == 0)
    {
        uiIPAddr = uiSrcIP;
        signal(SIGALRM, ntp_sig_alarm);
        alarm((uint32_t)time + 5);
    }

    uiSrcIP_net = htonl(uiSrcIP);

    if (uiIPAddr == uiSrcIP)
    {
        alarm((uint32_t)time + 5);
        uiCount = uiCount + 1;

        if (uiCount >= 2)
        {
            if ((pstPacket->ucStratum < ntp_global.uiStratum) && ((pstPacket->ucLiVnMode & 0xc0) != 0xc0))
            {
                ntp_global.uiStratum = pstPacket->ucStratum + 1;
                inet_ntop(AF_INET, &uiSrcIP_net, (char *)ntp_global.ucReferID, sizeof(ntp_global.ucReferID));
                ntp_time_offset(pstPacket);
            }
        }
    }

    return;
}

static void ntp_passive_message(struct sNtpPacket *pstPacket, uint32_t uiSrcIP)
{
    uint32_t uiSrcIP_net = 0;

    if ((ntp_global.uiServerIP == uiSrcIP) &&
            (ntp_global.uiVersion == PKT_VERSION(pstPacket->ucLiVnMode)))
    {
        ntp_time_record(pstPacket);
        uiSrcIP_net = htonl(uiSrcIP);

        if ((pstPacket->ucStratum < ntp_global.uiStratum) && ((pstPacket->ucLiVnMode & 0xc0) != 0xc0))
        {
            ntp_global.uiStratum = pstPacket->ucStratum + 1;
            inet_ntop(AF_INET, &uiSrcIP_net, (char *)ntp_global.ucReferID, sizeof(ntp_global.ucReferID));
            ntp_get_data(pstPacket);

            ntp_global.uiSyncFlag = NTP_SYNC_SUCCESS;
        }
    }

    return;
}
#if 0
void ntp_unicast_pkt_rcv()
{
    struct pkt_buffer *pkt = NULL;
    struct sNtpPacket *pstPacket;

    pkt = pkt_rcv(MODULE_ID_NTP);

    if (NULL == pkt)
    {
        goto out;
    }
    else
    {
        pstPacket = (struct sNtpPacket *)pkt->data;

        if (MODE_CLIENT == ntp_global.uiWorkMode)
        {
            if (((pstPacket->ucLiVnMode & 0xc0) != 0xc0) &&
                    (PKT_MODE(pstPacket->ucLiVnMode) == MODE_SERVER) &&
                    ((pstPacket->ucStratum >= 1) && (pstPacket->ucStratum <= 15)))
            {
                ntp_global.uiRecvCSpacket++;

                ntp_server_message(pstPacket, pkt->cb.ipcb.sip);
            }
            else if (PKT_MODE(pstPacket->ucLiVnMode) == MODE_ACTIVE)
            {
                ntp_global.uiRecvPeerPacket++;

                ntp_active_message(pstPacket, pkt->cb.ipcb.sip, pkt->cb.ipcb.sport);
            }
        }
        else
        {
            if (PKT_MODE(pstPacket->ucLiVnMode) == MODE_PASSIVE)
            {
                ntp_global.uiRecvPeerPacket++;

                ntp_passive_message(pstPacket, pkt->cb.ipcb.sip);
            }
        }
    }

out:
    usleep(50000);
    thread_add_event(ntp_master, ntp_unicast_pkt_rcv, NULL, 0);
    return;
}

void ntp_broadcast_pkt_rcv()
{
    struct pkt_buffer *pkt = NULL;
    struct sNtpPacket *pstPacket;

    pkt = pkt_rcv(MODULE_ID_NTP);

    if (NULL == pkt)
    {
        goto out;
    }
    else
    {
        pstPacket = (struct sNtpPacket *)pkt->data;

        if (((pstPacket->ucLiVnMode & 0xc0) != 0xc0) &&
                (PKT_MODE(pstPacket->ucLiVnMode) == MODE_BROADCAST) &&
                ((pstPacket->ucStratum >= 1) && (pstPacket->ucStratum <= 15)))
        {
            ntp_global.uiRecvCSpacket++;

            if ((ntp_global.uiVersion == PKT_VERSION(pstPacket->ucLiVnMode)) &&
                    (pstPacket->ucStratum < ntp_global.uiStratum))
            {
                ntp_broadcast_message(pstPacket, pkt->cb.ipcb.sip, pkt->cb.ipcb.sport);
            }
        }
    }

out:
    usleep(50000);
    thread_add_event(ntp_master, ntp_broadcast_pkt_rcv, NULL, 0);
    return;
}
#endif
static int ntp_pkt_send()
{
    int iRet;
    unsigned int uiRefIp = 0;
    unsigned int uiRefIp_net = 0;
    struct sNtpPacket stPacket;
    struct timeval now;
    union  pkt_control pktcontrol;

    memset(&pktcontrol, 0, sizeof(pktcontrol));
    memset(&stPacket, 0, sizeof(stPacket));

    stPacket.ucLiVnMode = (LI << 6) | (ntp_global.uiVersion << 3) | (ntp_global.uiWorkMode);
    stPacket.ucStratum = ntp_global.uiStratum;
    stPacket.ucPoll = (4);
    stPacket.cPrecision = PREC & 0xff;

    if (strncmp((char *)ntp_global.ucReferID, "none", 3) == 0)
    {
        stPacket.cReferenceID[0] = 0;
        stPacket.cReferenceID[1] = 0;
        stPacket.cReferenceID[2] = 0;
        stPacket.cReferenceID[3] = 0;
    }
    else
    {
        inet_pton(AF_INET, (char *)ntp_global.ucReferID, &uiRefIp);
        uiRefIp_net = htonl(uiRefIp);
        stPacket.cReferenceID[0] = uiRefIp_net >> 24;
        stPacket.cReferenceID[1] = uiRefIp_net >> 16;
        stPacket.cReferenceID[2] = uiRefIp_net >> 8;
        stPacket.cReferenceID[3] = uiRefIp_net;
    }

    if (MODE_CLIENT == ntp_global.uiWorkMode)
    {
        stPacket.uiRootDelay = htonl(1 << 16);
        stPacket.uiRootDispersion = htonl(1 << 16);

        gettimeofday(&now, NULL);
        client_send_time.tv_sec = now.tv_sec;
        client_send_time.tv_usec = now.tv_usec;

        if (1 == iClientFlag)//收到服务器响应的报文
        {
            stPacket.uiRecvtimestampInterger = htonl(timerecv.tv_sec + JAN_1970);
            stPacket.uiRecvtimestampFractional = htonl(timerecv.tv_usec);
            stPacket.uiOritimestampInterger = htonl(timeori.tv_sec);
            stPacket.uiOritimestampFractional = htonl(timeori.tv_usec);
        }

        stPacket.uiTrantimestampInterger = htonl(now.tv_sec + JAN_1970);
        stPacket.uiTrantimestampFractional = htonl(now.tv_usec);
    }
    else
    {
        stPacket.uiRootDispersion = htonl(0x0560);
        gettimeofday(&now, NULL);

        stPacket.uiReftimestampInteger = htonl(now.tv_sec + JAN_1970 - 2);
        stPacket.uiReftimestampFractional = htonl(now.tv_usec);

        if (1 == iActiveFlag)//收到被动对等体回应的报文
        {
            stPacket.uiRecvtimestampInterger = htonl(timerecv.tv_sec + JAN_1970);
            stPacket.uiRecvtimestampFractional = htonl(timerecv.tv_usec);
            stPacket.uiOritimestampInterger = htonl(timeori.tv_sec);
            stPacket.uiOritimestampFractional = htonl(timeori.tv_usec);
        }

        gettimeofday(&now, NULL);
        client_send_time.tv_sec = now.tv_sec;
        client_send_time.tv_usec = now.tv_usec;

        stPacket.uiTrantimestampInterger = htonl(now.tv_sec + JAN_1970);
        stPacket.uiTrantimestampFractional = htonl(now.tv_usec);
    }

    pktcontrol.ipcb.dip = ntp_global.uiServerIP;
    pktcontrol.ipcb.dport = NTP_PORT;
    pktcontrol.ipcb.ttl = 64;
    pktcontrol.ipcb.vpn = ntp_global.uiPacketVpnID;        
    pktcontrol.ipcb.tos = 5;
    pktcontrol.ipcb.chsum_enable = 1;

    iRet = pkt_send(PKT_TYPE_UDP, &pktcontrol, &stPacket, sizeof(struct sNtpPacket));

    return iRet;
}

static void ntp_set_time()
{
    struct timeval stTime;
    struct timeval diff;

    if (strcmp((char *)"broadcast", (char *)ntp_global.ucPacketFormat) == 0)
    {
        if ((ntp_global.stBroadcastTime.tv_sec != 0) || (ntp_global.stBroadcastTime.tv_usec != 0))
        {
            if ((last_bdcast_time.tv_sec != ntp_global.stBroadcastTime.tv_sec) &&
                    (last_bdcast_time.tv_usec != ntp_global.stBroadcastTime.tv_usec))
            {
                last_bdcast_time.tv_sec = ntp_global.stBroadcastTime.tv_sec;
                last_bdcast_time.tv_usec = ntp_global.stBroadcastTime.tv_usec;

                //ntp_global.ntp_global.uiStratum = ntp_global.uiStratum;
                gettimeofday(&stTime, NULL);
                diff.tv_sec = stTime.tv_sec - recv_broadcast_time.tv_sec + ntp_global.stBroadcastTime.tv_sec;
                diff.tv_usec = stTime.tv_usec - recv_broadcast_time.tv_usec + ntp_global.stBroadcastTime.tv_usec;

                if (0 == ntp_localtime_modify(&diff))
                {
                    ntp_global.uiSyncState = TsSynced;
                }
                else
                {
                    ntp_global.uiSyncState = TsNoSync;
                }

            }
        }
    }
    else
    {
        if ((ntp_global.stTimeOffset.tv_sec != 0) || (ntp_global.stTimeOffset.tv_usec != 0))
        {
            if ((last_server_time.tv_sec != server_send_time.tv_sec) &&
                    (last_server_time.tv_usec != server_send_time.tv_usec))
            {
                last_server_time.tv_sec = server_send_time.tv_sec;
                last_server_time.tv_usec = server_send_time.tv_usec;
                //ntp_global.ntp_global.uiStratum = ntp_global.uiStratum;

                gettimeofday(&stTime, NULL);
                stTime.tv_sec = stTime.tv_sec + ntp_global.stTimeOffset.tv_sec;
                stTime.tv_usec = stTime.tv_usec + ntp_global.stTimeOffset.tv_usec;

                if (0 == ntp_localtime_modify(&stTime))
                {
                    ntp_global.uiSyncState = TsSynced;
                }
                else
                {
                    ntp_global.uiSyncState = TsNoSync;
                }

                //iShowFlag = 1;
            }
        }
    }

    return;
}

#if 0
void ntp_sync_interval()
{
    ntp_set_time();
    synctimethread = thread_add_timer(ntp_master, ntp_sync_interval, 0, ntp_global.uiSyncInterval);

    return;
}
#endif

static int ntp_pkt_rcv_n(struct ipc_mesg_n *pmesg)
{
    struct sNtpPacket *pstPacket;
    struct pkt_buffer   *pkt = NULL;    
    
    pkt = (struct pkt_buffer *)pmesg->msg_data;
    if(pkt == NULL)
    {
        return ERRNO_FAIL;
                        
    }

    if (NULL == pkt || ntp_global.uiClientEnable == 0)
    {
        goto out;
    }
    else
    {
        pstPacket = (struct sNtpPacket *)pkt->data;

        /* 接收NTP数据包的层和版本正确 */
        if (((pstPacket->ucStratum >= 1) && (pstPacket->ucStratum <= NTP_DEFAUT_STRATUM))   &&
                (PKT_VERSION(pstPacket->ucLiVnMode) == ntp_global.uiVersion))
        {
            switch (PKT_MODE(pstPacket->ucLiVnMode))
            {
                case MODE_ACTIVE :      /* 接收主动对等体报文 */

                    /* NTP已经同步后，且发送的报文的设备层数高于本设备 */
                    if ((ntp_global.uiStratum != NTP_DEFAUT_STRATUM) && (pstPacket->ucStratum < ntp_global.uiStratum))
                    {
                        goto out;
                    }

                    ntp_global.uiRecvPeerPacket++;
                    ntp_active_message(pstPacket, pkt->cb.ipcb.sip, pkt->cb.ipcb.sport);
                    break;

                case MODE_PASSIVE :     /* 接收被动对等体报文 */
                    ntp_global.uiRecvPeerPacket++;
                    ntp_passive_message(pstPacket, pkt->cb.ipcb.sip);
                    break;

                case MODE_CLIENT :      /* 接收客户端报文，设备暂不支持作为服务器 */
                    ;
                    break;

                case MODE_SERVER :      /* 接收服务器报文 */
                    ntp_global.uiRecvCSpacket++;
                    ntp_server_message(pstPacket, pkt->cb.ipcb.sip);
                    break;

                case MODE_BROADCAST :   /* 接收广播报文 */

                    /* 设备工作在广播客户端模式 */
                    if ((MODE_CLIENT == ntp_global.uiWorkMode) &&
                            (strcmp((char *)"broadcast", (char *)ntp_global.ucPacketFormat) == 0))
                    {
                        unsigned int dst_ip = 0;
                        inet_pton(AF_INET, "255.255.255.255", &dst_ip);

                        /* 目的 IP 是广播 IP，源端口是 NTP 默认的端口 */
                        if ((pkt->cb.ipcb.dip == dst_ip) && (pkt->cb.ipcb.sport == NTP_PORT))
                        {
                            ntp_global.uiRecvCSpacket++;
                            ntp_broadcast_message(pstPacket, pkt->cb.ipcb.sip, pkt->cb.ipcb.sport);
                        }
                        else
                        {
                            zlog_debug(NTP_DBG_ERROR,"ntp recv broadcast pkt.dip 0x%x src prot : 0x%x dst_ip:0x%x ,locah srcport:0x%x\n"
                                       , pkt->cb.ipcb.dip, htons(pkt->cb.ipcb.sport), dst_ip, ntohs(NTP_PORT));
                        }
                    }
                    else
                    {
                        zlog_err("ntp recv broadcast but drop it ! src prot : %x\n", htons(pkt->cb.ipcb.sport));
                    }

                    break;
            }

            ntp_set_time();
        }
    }

out:

    /* 丢包超过3个则认为同步失败 */
    if (ntp_global.uiSyncFlag > NTP_SYNC_FAIL)
    {
        //ntp_global.uiStratum = NTP_DEFAUT_STRATUM;

        ntp_global.uiSyncFlag = NTP_SYNC_FAIL;
        ntp_global.uiSyncState = TsNoSync;
        ntp_global.uiStratum = NTP_DEFAUT_STRATUM;
        ntp_global.stTimeOffset.tv_sec = 0;
        ntp_global.stTimeOffset.tv_usec = 0;
        ntp_global.stTimeDelay.tv_sec = 0;
        ntp_global.stTimeDelay.tv_usec = 0;
        memcpy((void *)ntp_global.ucReferID, (const void *)"none", sizeof(ntp_global.ucReferID));
    }
    return 0;
}

/* NTP 使能 */
int ntp_client_enable(void *t)
{
    int iRet = 0;

    if (strcmp((char *)"broadcast", (char *)ntp_global.ucPacketFormat) == 0)
    {
        goto out;
    }

    iRet = ntp_pkt_send();

    if (iRet == NET_FAILED)
    {
        zlog_err("%s, %d send ntp message failed!\n", __FUNCTION__, __LINE__);
    }
    else
    {
        if (MODE_CLIENT == ntp_global.uiWorkMode)
        {
            ntp_global.uiSendCSPacket++;
        }
        else
        {
            ntp_global.uiSendPeerPacket++;
        }
    }

out:
    ntp_global.uiSyncFlag++;
    ntptimer = high_pre_timer_add("NtpEnableTimer", LIB_TIMER_TYPE_NOLOOP, ntp_client_enable, NULL, ntp_global.uiSyncInterval * 1000);

    return iRet;
}

/* NTP 去使能 */
void ntp_client_disable(void)
{
    iActiveFlag = 0;
    iClientFlag = 0;
    iRtcSetFlag = 0;
    ntp_global.uiSyncState = TsNoSync;
    ntp_global.stTimeOffset.tv_sec = 0;
    ntp_global.stTimeOffset.tv_usec = 0;
    ntp_global.stTimeDelay.tv_sec = 0;
    ntp_global.stTimeDelay.tv_usec = 0;
    memcpy((void *)ntp_global.ucReferID, (const void *)"none", sizeof(ntp_global.ucReferID));

    ntp_global.uiStratum = NTP_DEFAUT_STRATUM;

    if (ntptimer != 0)
    {
        high_pre_timer_delete(ntptimer);
        ntptimer = 0;
    }

    return ;
}


/* 注册要接收的协议报文类型 */
void ntp_pkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_NTP;
    pkt_register(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}

/* 注册要接收的协议报文类型 */
void ntp_broadcastpkt_register(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dip  = 0xffffffff;
    proto.ipreg.dport = NTP_PORT;
    pkt_register(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}


/* 注销 NTP 收包 */
void ntp_pkt_unregister(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dport = UDP_PORT_NTP;
    pkt_unregister(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}
/* 注销 NTP 收包 */
void ntp_broadcastpkt_unregister(void)
{
    union proto_reg proto;

    memset(&proto, 0, sizeof(union proto_reg));
    proto.ipreg.protocol = IP_P_UDP;
    proto.ipreg.dip  = 0xffffffff;
    proto.ipreg.dport = UDP_PORT_NTP;
    pkt_unregister(MODULE_ID_NTP, PROTO_TYPE_IPV4, &proto);

    return;
}

int ntp_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen)
{
    //int msg_num = IPC_MSG_LEN / sizeof(struct ces_if);
    //int msg_num = 63;
    int ret = 0;
    int revln = 0;
    //uint32_t ifindex = 0;
    struct ipc_msghdr_n *phdr = NULL;
    //struct ifm_event *pevent = NULL;

    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
    if(revln <= imlen)
    {
        /* æ¶ˆæ¯å¤„ç† */
        phdr = &(pmesg->msghdr);
        if ((phdr->msg_type  == IPC_TYPE_SNMP) &&
                (phdr->opcode == IPC_OPCODE_GET))
        {
             //ret = ipc_send_reply(&ntp_global, sizeof(ntp_globals), phdr->sender_id, MODULE_ID_NTP,
             //                    IPC_TYPE_SNMP, phdr->msg_subtype, phdr->msg_index);
            
             ret = ipc_send_reply_n2(&ntp_global, sizeof(ntp_globals),1,phdr->sender_id,MODULE_ID_NTP,  \
                                         IPC_TYPE_SNMP,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
        }
        else if(phdr->msg_type == IPC_TYPE_PACKET)
        {
            ntp_pkt_rcv_n(pmesg);
        }        
    }
    else
    {
         //FILEM_DEBUG("filem recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
         printf("%s[%d], datalen error, data_len=%d, msgrcv len = %d\n",
                        __FUNCTION__, __LINE__, revln, imlen);
    }

    mem_share_free(pmesg, MODULE_ID_NTP);

    return ret;
}

void ntp_init(void)
{
    memset(&ntp_global, 0, sizeof(ntp_global));

    ntp_global.uiWorkMode = MODE_CLIENT;
    ntp_global.uiVersion = NTP_VERSION;
    ntp_global.uiSyncInterval = NTP_SYNC_INTERVAL;
    ntp_global.uiStratum = NTP_DEFAUT_STRATUM;
    strncpy((char *)ntp_global.ucPacketFormat, (char *)"unicast", sizeof(ntp_global.ucPacketFormat));
    strncpy((char *)ntp_global.ucReferID, (char *)"none", sizeof(ntp_global.ucReferID));

    ntp_broadcastpkt_register();
    ntp_pkt_register();
    ntp_cmd_init();


    return;
}


