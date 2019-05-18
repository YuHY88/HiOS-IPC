#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _ZEBOS_SDM_NTP_CLIENT__H
#define _ZEBOS_SDM_NTP_CLIENT__H

#define YEARS_1970_to_2000_SEC 0x386d4380
#define JAN_1970            0x83aa7e80      /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT            (123)
#define TV_USEC_PER_SEC     1000000 /*1s*/
#define ADJ_THRESHOLD       100000 /*0.1s*/

#define NTP_VERSION         4 /* NTP 协议的默认版本 */
#define NTP_SYNC_INTERVAL   10 /* NTP 默认同步间隔 */
#define NTP_RTC_181_DIR     "/dev/rtc"
#define NTP_RTC_2000_DIR    "/dev/rtc0"
#define NTP_RTC_HT201_DIR   "/dev/rtc"
#define PATH_LOCALTIME      "/etc/localtime"
/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) ( 4294*(x) + ( (1981*(x))>>11 ) )

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via settimeofday) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ( (x) * 15.2587890625 )
#define MODE_ACTIVE 1      /* symmetric active mode */
#define MODE_PASSIVE 2     /* symmetric passive mode */
#define MODE_CLIENT 3      /* client mode */
#define MODE_SERVER 4      /* server mode */
#define MODE_BROADCAST 5   /* broadcast mode */
#define NTPCLIENTENABLE 1  /* ntp-client enable */
#define LI 0               /* leap indicator */
#define PREC -6            /* precision */

#define PKT_VERSION(li_vn_mode) ((uint32_t)(((li_vn_mode) >> 3) & 0x7))
#define PKT_MODE(li_vn_mode)    ((u_char)((li_vn_mode) & 0x7))

enum NTP_STATUS
{
    TsNoSync = 0,
    TsSynced
};

struct sNtpPacket
{
    uint8_t     ucLiVnMode;
    uint8_t     ucStratum;
    uint8_t     ucPoll;
    char        cPrecision;
    uint32_t    uiRootDelay;
    uint32_t    uiRootDispersion;
    char        cReferenceID[4];
    uint32_t    uiReftimestampInteger;
    uint32_t    uiReftimestampFractional;
    uint32_t    uiOritimestampInterger;
    uint32_t    uiOritimestampFractional;
    uint32_t    uiRecvtimestampInterger;
    uint32_t    uiRecvtimestampFractional;
    uint32_t    uiTrantimestampInterger;
    uint32_t    uiTrantimestampFractional;
} __attribute__((packed));

/******************************************************************************************
 LI 2(bit) | VN 3(bit) | Mode 3(bit) |Streatum 8(bit) | Poll 8(bit) | Precision 8(bit)    #NTPheader:合计32bit
 __________________________________________________________________________________________
                                    Root Delay 32(bit)
__________________________________________________________________________________________
                                  Root Dispersion 32(bit)
_________________________________________________________________________________________
                                Reference Identifier 32(bit)
______________________________________________________________________________________________
                                 Reference timestamp 64(bit)
___________________________________________________________________________________________
                                 Originate  Timestamp 64(bit)
___________________________________________________________________________________________
                                  Receive timestamp  64(bit)
___________________________________________________________________________________________
                                  Transmit  Timestamp 64(bit)
_____________________________________________________________________________________________
                                Key Identifier (optional) 32(bit)
_____________________________________________________________________________________________
                                Message digest(otptional) 128(ibt)
___________________________________________________________________________________________

********************************************************************************************/

extern void ntp_unicast_pkt_rcv();
extern void ntp_broadcast_pkt_rcv();
extern void ntp_sync_interval();
extern int ntp_client_enable(void *t);
extern void ntp_client_disable(void);
extern void ntp_pkt_register(void);
extern void ntp_pkt_unregister(void);
extern void ntp_init(void);
extern void ntp_broadcastpkt_register(void);
extern void ntp_broadcastpkt_unregister(void);
int ntp_msg_rcv_msg_n(struct ipc_mesg_n *pmesg, int imlen);


#endif
