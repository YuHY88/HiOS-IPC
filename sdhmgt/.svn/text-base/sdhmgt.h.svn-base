/**********************************************************
* file name: sdhmgt.h
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan jianghongzhi 2018-04-17
* function:
*
* modify:
*
***********************************************************/

#ifndef _SDHMGT_H_
#define _SDHMGT_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>

#include <lib/log.h>

#include "msg_ipc.h"
#include "devm_com.h"
#include "msdh_common.h"
#include "clock/clock_if.h"

//#define SDHMGT_PRINT_EN

#ifdef SDHMGT_PRINT_EN
#define SDHMGT_DEBUG                printf
#else
#define SDHMGT_DEBUG                zlog_debug
#endif

#ifdef SDHMGT_PRINT_EN
#define SDHMGT_ERROR                printf
#else
#define SDHMGT_ERROR                zlog_err
#endif

#define SDHMGT_UPLOAD               0
#define SDHMGT_DOWNLOAD             1

#define SDHMGT_SLOT_ADD             1
#define SDHMGT_SLOT_DEL             2

#define MD5_SIZE                    16
#define MD5_STR_LEN                 (MD5_SIZE * 2)

//--------------------------------------------------------------------
#define SDHMGT_ETH8_IP              0x01000810  // MX local IP 1.0.8.16 for slot8 TX
#define SDHMGT_ETH9_IP              0x01000911  // MX local IP 1.0.9.17 for slot9 TX
#define SDHMGT_DST_IP_TX1           0x01000808  // slot 8 TX IP 1.0.8.8
#define SDHMGT_DST_IP_TX2           0x01000909  // slot 9 TX IP 1.0.9.9

#define SDHMGT_TX1_SLOT                6        // TX logic slot 8, physical slot 6
#define SDHMGT_TX2_SLOT                7        // TX logic slot 9, physical slot 7

#define SDHMGT_AGENT_SDH_CARD_NUM     12
#define SDHMGT_BACK_TRIB_CLOCK_NUM     4

#define SDHMGT_UPORT_1              3201
#define SDHMGT_UPORT_2              3202

//--------------------------------------------------------------------
#define SDHMGT_HELLO_TIMEOUT          10        // 10 sec
#define SDHMGT_IPC_LISTEN_MESG     10000        // 10000 usec, 10 msec
#define SDHMGT_IPC_LISTEN_SNMP      1000        // 1000 usec, 1 msec
#define SDHMGT_IIMING_SEND_TIMER    1000        // 1000 msec, 1 sec
#define SDHMGT_IIMING_QUERY_TIMER  60000        // 60000 msec, 60 sec

#define SDHMGT_PROCESS_TIMEOUT_THR    15        // 15 * SDHMGT_IIMING_QUERY_TIMER secs

enum SDHMGT_AGENT_TX_SOCKET
{
    SDHMGT_SOCKET_FOR_SLOT8_UPORT_1 = 0,        // listen SDHMGT_ETH8_IP, SDHMGT_UPORT_1
    SDHMGT_SOCKET_FOR_SLOT8_UPORT_2,            // listen SDHMGT_ETH8_IP, SDHMGT_UPORT_2
    SDHMGT_SOCKET_FOR_SLOT9_UPORT_1,            // listen SDHMGT_ETH9_IP, SDHMGT_UPORT_1
    SDHMGT_SOCKET_FOR_SLOT9_UPORT_2,            // listen SDHMGT_ETH9_IP, SDHMGT_UPORT_2
    SDHMGT_SOCKET_MAX
};

#define SDH_CONFIG_FILE     "/data/cfg/sdh_config.tar.gz"

struct sdhmgt_slot_info                     /*for sdh card*/
{
    uint8_t                 location;       /*0x00:ok, 0x80:no exist*/
    uint8_t                 soft_ver;       /*soft version*/
    uint8_t                 fpgaa_ver;      /*fpga a version*/
    uint8_t                 fpgab_ver;      /*fpga b version*/
    uint8_t                 id;             /*card type id*/
};

struct sdhmgt_entry
{
    int                     h9molmxe_vx;    /*check device_id*/

    enum DEVM_HA_E          ms_status;      /*local card MASTER or SLAVE*/
    uint8_t                 slot_main;
    uint8_t                 slot_local;

    uint8_t                 serial_type;    /*serial type, for sdh card<MSDH>*/
    uint8_t                 device_type;    /*device type, for sdh card<MSDH>*/

    /*sdh master slot index, for sdh card*/
    int                     sdh_master_slot; /* SDHMGT_TX1_SLOT/SDHMGT_TX2_SLOT/0 */
    int                     sdh_master_last; /* SDHMGT_TX1_SLOT/SDHMGT_TX2_SLOT/0 */

    /*sdh card info timeout timer, SDHMGT_HELLO_TIMEOUT */
    long                    sdh_hello_time;  /* record sysinfo.uptime */

    /* sdh card info */
    struct sdhmgt_slot_info slot_info[SDHMGT_AGENT_SDH_CARD_NUM];
    struct sdhmgt_slot_info slot_last[SDHMGT_AGENT_SDH_CARD_NUM];

    /*get eeprom flag and cnt, if 0 == cnt, stop get */
    int                     slot_eeprom_get_cnt[SDHMGT_AGENT_SDH_CARD_NUM];

    /* socket */
    int                     max_fd;
    int                     socket[SDHMGT_SOCKET_MAX];

    /*back trub clcok*/
    struct syncE_sdhmgt     trib_clock[SDHMGT_BACK_TRIB_CLOCK_NUM];
    int                     process_clock_timeout_cnt;

    /*agent sdh config file, view SDH_CONFIG_FILE*/
    uint8_t                 config_file;    /*SDH config flag, 0:error, 1:ok*/
    char                    md5_str[MD5_STR_LEN + 1];   /*SDH config file MD5*/
    int                     config_file_mtime;

    /*system base mac, form eeprom*/
    uchar                   sys_base_mac[6];

    /*tx nm ip*/
    uint32_t                nm_ip_addr;
};

// --------------------------------------------------------
extern struct sdhmgt_entry      g_sdhmgt_entry;
extern struct thread_master    *g_psdhmgt_master;
extern uint32_t                 g_sdhmgt_debug;


// --------------------------------------------------------
/*
 * check TX ID, get MX ID
 *   0, invalid
 */
extern uint32_t             sdhmgt_get_MXID_from_TXID(uint8_t id);


/*
 * check SDH_CONFIG_FILE, update sdhmgt_entry.config_file
 */
extern void                 sdhmgt_config_file_check(void);
extern void                 sdhmgt_config_file_recheck(void);


#endif

