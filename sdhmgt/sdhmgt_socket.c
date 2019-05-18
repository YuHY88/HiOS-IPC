/**********************************************************
* file name: sdhmgt_socket.c
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-16
* function:
*
* modify:
*
***********************************************************/
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/select.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <stdio.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/prctl.h>
#include <sys/sysinfo.h>

#include "pkt_type.h"
#include "module_id.h"
#include "ifm_common.h"
#include "devm_com.h"
#include "errcode.h"
#include "pkt_buffer.h"
#include "thread.h"

#include <ftm/pkt_udp.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_eth.h>
#include <ftm/pkt_ip.h>

#include "sdhmgt_msg.h"
#include "sdhmgt_snmp.h"
#include "msdh_common.h"
#include "sdhmgt.h"

#include "sdhmgt_socket.h"


#define SDHMGT_RECV_LEN         1200

uint8_t g_msdh_rx_data[SDHMGT_RECV_LEN];
uint8_t g_msdh_tx_data[SDHMGT_RECV_LEN];

//---------------------------------------------------------
uint16_t sdhmgt_packet_msdh_hello(uint8_t *pframe);


//---------------------------------------------------------
static void print_msdh_frame(uint8_t *pframe, int frm_len)
{
    int     i;
    char    tmp[64];
    char   *ptmp;

    if (NULL == pframe)
    {
        return;
    }

    SDHMGT_DEBUG(0, "msdh frame length : %d\n", frm_len);

    memset(tmp, 0, 64);
    ptmp = tmp;

    for (i = 0; i < frm_len; i++)
    {
        sprintf(ptmp, "%02X ", pframe[i]);
        ptmp = ptmp + 3;

        if (i % 16 == 15)
        {
            SDHMGT_DEBUG(0, "%s\n", tmp);

            memset(tmp, 0, 64);
            ptmp = tmp;
        }
    }

    SDHMGT_DEBUG(0, "%s\n", tmp);
}

int sdhmgt_send_msdh_for_sdh_device(uint8_t *pframe, int frm_len)
{
    struct sockaddr_in sockaddr;
    int ret = 0;
    int fd = 0;

    if (1 == g_sdhmgt_debug)
    {
        SDHMGT_DEBUG(0, "sdhmgt send msdh frame to UDP port 3201 \n");
        print_msdh_frame(pframe, frm_len);
    }

    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons(SDHMGT_UPORT_1);
    bzero(&sockaddr.sin_zero, 8);

    if (g_sdhmgt_entry.sdh_master_slot == SDHMGT_TX1_SLOT)
    {
        fd = g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_1];
        sockaddr.sin_addr.s_addr = htonl(SDHMGT_DST_IP_TX1);
    }
    else if (g_sdhmgt_entry.sdh_master_slot == SDHMGT_TX2_SLOT)
    {
        fd = g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_1];
        sockaddr.sin_addr.s_addr = htonl(SDHMGT_DST_IP_TX2);
    }
    else
    {
        return -1;
    }

    if (fd <= 0)
    {
        return -1;
    }

    ret = sendto(fd, pframe, frm_len, 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));

    if (ret < 0)
    {
//        SDHMGT_ERROR("send to socket %d err, errno:%d, str:%s\n", fd, errno, strerror(errno));
        return ret;
    }

    return ret;
}

/* listern udp port 3201 */
void sdhmgt_recv_msdh_from_sdh_device(int fd)
{
    struct sockaddr_in server_addr;
    int rcv_len = sizeof(struct sockaddr);
    int frm_len = 0;
    int ret = 0;

    frm_len = recvfrom(fd, g_msdh_rx_data, SDHMGT_RECV_LEN, 0, (struct sockaddr *)&server_addr, (socklen_t *)&rcv_len);

    if (frm_len > 0)
    {
        if (1 == g_sdhmgt_debug)
        {
            SDHMGT_DEBUG(0, "accept UDP port 3201, frame len:%d, %08x\n", frm_len, server_addr.sin_addr.s_addr);
            print_msdh_frame(g_msdh_rx_data, frm_len);
        }

        // msdh frame check
        ret = msdh_frm_isok((t_msdhinf *)g_msdh_rx_data, frm_len);

        if (1 != ret)
        {
            SDHMGT_ERROR("%s[%d]:leave %s:msdh_frm_isok error.\n", __FILE__, __LINE__, __FUNCTION__);
            return;
        }

        if (0x01 & g_msdh_rx_data[1])   // filter set msdh frame
        {
            return;
        }

        // send msdh to SNMPD
        sdhmgt_msg_to_snmp(g_msdh_rx_data, frm_len, SDHMGT_SUBTYPE_REPORT_TRAP_INFO);
    }
}

int sdhmgt_send_msdh_for_sdh_card(uint8_t slot, uint8_t *pframe, int frm_len)
{
    struct sockaddr_in sockaddr;
    int ret = 0;
    int fd = 0;

    if (1 == g_sdhmgt_debug)
    {
        SDHMGT_DEBUG(0, "sdhmgt send msdh frame to UDP port 3202 \n");
        print_msdh_frame(pframe, frm_len);
    }

    memset(&sockaddr, 0, sizeof(struct sockaddr_in));
    sockaddr.sin_family      = AF_INET;
    sockaddr.sin_port        = htons(SDHMGT_UPORT_2);
    bzero(&sockaddr.sin_zero, 8);

    if (slot == SDHMGT_TX1_SLOT)
    {
        fd = g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_2];
        sockaddr.sin_addr.s_addr = htonl(SDHMGT_DST_IP_TX1);
    }
    else if (slot == SDHMGT_TX2_SLOT)
    {
        fd = g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_2];
        sockaddr.sin_addr.s_addr = htonl(SDHMGT_DST_IP_TX2);
    }
    else
    {
        return -1;
    }

    if (fd <= 0)
    {
        return -1;
    }

    ret = sendto(fd, pframe, frm_len, 0, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr));

    if (ret < 0)
    {
//        SDHMGT_ERROR("send to socket %d err, errno:%d, str:%s\n", fd, errno, strerror(errno));
        return ret;
    }

    return ret;
}

/* listen udp port 3202 */
void sdhmgt_recv_msdh_from_sdh_card(int fd)
{
    struct sockaddr_in server_addr;
    int rcv_len = sizeof(struct sockaddr);
    int frm_len = 0;
    int ret = 0;

    frm_len = recvfrom(fd, g_msdh_rx_data, SDHMGT_RECV_LEN, 0, (struct sockaddr *)&server_addr, (socklen_t *)&rcv_len);

    if (frm_len > 0)
    {
        if (1 == g_sdhmgt_debug)
        {
            SDHMGT_DEBUG(0, "accept UDP port 3202, frame len:%d, %08x\n", frm_len, server_addr.sin_addr.s_addr);
            print_msdh_frame(g_msdh_rx_data, frm_len);
        }

        // msdh frame check
        ret = msdh_frm_isok((t_msdhinf *)g_msdh_rx_data, frm_len);

        if (1 != ret)
        {
            SDHMGT_ERROR("%s[%d]:leave %s:msdh_frm_isok error.\n", __FILE__, __LINE__, __FUNCTION__);
            return;
        }

        // deal msdh msg
        sdhmgt_deal_msdh_frame(g_msdh_rx_data, frm_len);
    }
}

uint16_t sdhmgt_packet_msdh_hello(uint8_t *pframe)
{
    int i;
    int timezone = 0;
    time_t t1, t2 ;
    struct tm *tm_local, *tm_utc;
    struct timeval tmval;
    uint16_t length = 0;

    pframe[length++] = (uint8_t)(MSDH_CMD_MX_2_TX_HELLO >> 8);
    pframe[length++] = (uint8_t)(MSDH_CMD_MX_2_TX_HELLO);
    pframe[length++] = 0x32;
    pframe[length++] = 0xFB;
    pframe[length++] = 0x00;

    pframe[length++] = 0x06;   // TLV NUM

    /* TLV : ntp_info */
    gettimeofday(&tmval, NULL);
    time(&t1);
    t2 = t1;
    tm_local = localtime(&t1);
    t1 = mktime(tm_local);
    tm_utc = gmtime(&t2);
    t2 = mktime(tm_utc);
    timezone = (t1 - t2) / 3600;

    pframe[length++] = 0x01;
    pframe[length++] = 0x09;
    pframe[length++] = (u_int8_t)(tmval.tv_sec >> 24);
    pframe[length++] = (u_int8_t)(tmval.tv_sec >> 16);
    pframe[length++] = (u_int8_t)(tmval.tv_sec >>  8);
    pframe[length++] = (u_int8_t)(tmval.tv_sec);
    pframe[length++] = (u_int8_t)(tmval.tv_usec >> 24);
    pframe[length++] = (u_int8_t)(tmval.tv_usec >> 16);
    pframe[length++] = (u_int8_t)(tmval.tv_usec >>  8);
    pframe[length++] = (u_int8_t)(tmval.tv_usec);
    pframe[length++] = timezone;

    /* TLV : config file md5 */
    pframe[length++] = 0x02;
    pframe[length++] = 0x21;
    pframe[length++] = (1 == g_sdhmgt_entry.config_file) ? 0 : 1;
    memcpy(pframe + length, g_sdhmgt_entry.md5_str, MD5_STR_LEN);
    length += MD5_STR_LEN;

    /* TLV : system base mac */
    pframe[length++] = 0x03;
    pframe[length++] = 0x06;
    memcpy(pframe + length, g_sdhmgt_entry.sys_base_mac, 6);
    length += 6;

    /* TLV : back trib clock */
    pframe[length++] = 0x04;
    pframe[length++] = 0x11;
    pframe[length++] = SDHMGT_BACK_TRIB_CLOCK_NUM;

    /*clock */
    for (i = 0; i < SDHMGT_BACK_TRIB_CLOCK_NUM; ++i)
    {
        pframe[length++] = g_sdhmgt_entry.trib_clock[i].trib_use;

        if (_SHR_PORT_TYPE_IS_E1(g_sdhmgt_entry.trib_clock[i].ifindex))
        {
            pframe[length++] = 3;
        }
        else if (_SHR_PORT_TYPE_IS_STM(g_sdhmgt_entry.trib_clock[i].ifindex))
        {
            pframe[length++] = 2;
        }
        else
        {
            pframe[length++] = 0;
        }

        pframe[length++] = _SHR_PORT_SLOT_ID_GET(g_sdhmgt_entry.trib_clock[i].ifindex);
        pframe[length++] = _SHR_PORT_ID_GET(g_sdhmgt_entry.trib_clock[i].ifindex);
    }

    /* TLV : MX master slot */
    pframe[length++] = 0x05;
    pframe[length++] = 0x01;
    pframe[length++] = g_sdhmgt_entry.slot_main;

    /* TLV : TX NM IP */
    pframe[length++] = 0x06;
    pframe[length++] = 0x04;
    pframe[length++] = (uint8_t)(g_sdhmgt_entry.nm_ip_addr >> 24);
    pframe[length++] = (uint8_t)(g_sdhmgt_entry.nm_ip_addr >> 16);
    pframe[length++] = (uint8_t)(g_sdhmgt_entry.nm_ip_addr >>  8);
    pframe[length++] = (uint8_t)(g_sdhmgt_entry.nm_ip_addr);

    return length;
}

void sdhmgt_send_msdh_hello(void)
{
    uint16_t     datalen;
    uint16_t     frm_len;

    memset(g_msdh_tx_data, 0x00, SDHMGT_RECV_LEN);

    datalen = sdhmgt_packet_msdh_hello(g_msdh_tx_data + MSDH_FRM_HEAD_LEN);

    if (0 != datalen)
    {
        frm_len = msdh_packet(0x00, SDHMGT_DST_IP_TX1, SDHMGT_ETH8_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX1_SLOT, g_msdh_tx_data, frm_len);

        frm_len = msdh_packet(0x00, SDHMGT_DST_IP_TX2, SDHMGT_ETH9_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX2_SLOT, g_msdh_tx_data, frm_len);
    }
}

void sdhmgt_send_msdh_eep_get(int slot)
{
    uint16_t    datalen = 0;
    uint16_t    frm_len;
    uint8_t    *pdata;

    if (slot < 2 || slot > 13)
    {
        return;
    }


    pdata  = g_msdh_tx_data + MSDH_FRM_HEAD_LEN;

    pdata[datalen++] = (uint8_t)(MSDH_CMD_GET_EEPROM >> 8);
    pdata[datalen++] = (uint8_t)(MSDH_CMD_GET_EEPROM);
    pdata[datalen++] = 0x32;
    pdata[datalen++] = 0xFB;
    pdata[datalen++] = slot - 2;

    if (SDHMGT_TX1_SLOT == g_sdhmgt_entry.sdh_master_slot)
    {
        frm_len = msdh_packet(0x01, SDHMGT_DST_IP_TX1, SDHMGT_ETH8_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX1_SLOT, g_msdh_tx_data, frm_len);
    }
    else if (SDHMGT_TX2_SLOT == g_sdhmgt_entry.sdh_master_slot)
    {
        frm_len = msdh_packet(0x01, SDHMGT_DST_IP_TX2, SDHMGT_ETH9_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX2_SLOT, g_msdh_tx_data, frm_len);
    }
}

void sdhmgt_send_msdh_eep_set(int slot, uint8_t *peeprom, int eeprom_len)
{
    uint16_t    datalen = 0;
    uint16_t    frm_len;
    uint8_t    *pdata;
    uint8_t     eeptmp[257];
    memcpy(eeptmp, peeprom, eeprom_len);

    if (NULL == peeprom || eeprom_len > 256)
    {
        return;
    }

    if (slot < 2 || slot > 13)
    {
        return;
    }

    pdata  = g_msdh_tx_data + MSDH_FRM_HEAD_LEN;

    pdata[datalen++] = (uint8_t)(MSDH_CMD_SET_EEPROM >> 8);
    pdata[datalen++] = (uint8_t)(MSDH_CMD_SET_EEPROM);
    pdata[datalen++] = 0x32;
    pdata[datalen++] = 0xFB;
    pdata[datalen++] = slot - 2;
    memcpy(pdata + datalen, peeprom, eeprom_len);
    datalen += eeprom_len;

    if (SDHMGT_TX1_SLOT == g_sdhmgt_entry.sdh_master_slot)
    {
        frm_len = msdh_packet(0x01, SDHMGT_DST_IP_TX1, SDHMGT_ETH8_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX1_SLOT, g_msdh_tx_data, frm_len);
    }
    else if (SDHMGT_TX2_SLOT == g_sdhmgt_entry.sdh_master_slot)
    {
        frm_len = msdh_packet(0x01, SDHMGT_DST_IP_TX2, SDHMGT_ETH9_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(SDHMGT_TX2_SLOT, g_msdh_tx_data, frm_len);
    }
}

void sdhmgt_send_msdh_ack(uint8_t slot, uint16_t cmd_type, uint8_t status)
{
    uint16_t     datalen;
    uint16_t     frm_len;
    uint8_t     *pdata;

    memset(g_msdh_tx_data, 0x00, SDHMGT_RECV_LEN);

    datalen = 0;
    pdata = g_msdh_tx_data + MSDH_FRM_HEAD_LEN;

    pdata[datalen++] = (uint8_t)(cmd_type >> 8);
    pdata[datalen++] = (uint8_t)(cmd_type);
    pdata[datalen++] = g_sdhmgt_entry.serial_type;
    pdata[datalen++] = g_sdhmgt_entry.device_type;
    pdata[datalen++] = status;

    if (slot == SDHMGT_TX1_SLOT)
    {
        frm_len = msdh_packet(0x00, SDHMGT_DST_IP_TX1, SDHMGT_ETH8_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(slot, g_msdh_tx_data, frm_len);
    }
    else if (slot == SDHMGT_TX2_SLOT)
    {
        frm_len = msdh_packet(0x00, SDHMGT_DST_IP_TX2, SDHMGT_ETH9_IP, datalen, NULL, (t_msdhinf *)g_msdh_tx_data);
        sdhmgt_send_msdh_for_sdh_card(slot, g_msdh_tx_data, frm_len);
    }
}

int sdhmgt_deal_msdh_frame(uint8_t *pframe, int len)
{
    uint8_t     *p = NULL;
    uint16_t    cmd_type = 0;
    uint8_t     send_slot_id = 0;
    uint8_t     main_slot_id = 0;
    uint8_t     status = 0;
    int         i = 0;
    int         card_num;
    struct sysinfo s_info;

    // Skip the msdh message header.
    p = pframe + MSDH_FRM_HEAD_LEN;

    // cmd_type
    cmd_type = (((uint16_t)p[0]) << 8) | p[1];
    p += 2;

    g_sdhmgt_entry.serial_type = *p++;
    g_sdhmgt_entry.device_type = *p++;

    switch (cmd_type)
    {
        case MSDH_CMD_TX_2_MX_HELLO:
            p += 2;
            send_slot_id = *p++;
            main_slot_id = *p++;

            if (send_slot_id != main_slot_id)
            {
                break;
            }

            g_sdhmgt_entry.sdh_master_slot = main_slot_id;

            sysinfo(&s_info);
            g_sdhmgt_entry.sdh_hello_time = s_info.uptime;

            p += 1;
            card_num    = *p++;

            if (card_num == 0 || card_num > SDHMGT_AGENT_SDH_CARD_NUM)
            {
                break;
            }

            memset(g_sdhmgt_entry.slot_info, 0, sizeof(g_sdhmgt_entry.slot_info));

            for (i = 0; i < card_num; ++i)
            {
                if (0x00 == p[0] && 0x00 != p[4])
                {
                    g_sdhmgt_entry.slot_info[i].location  = 1;
                    g_sdhmgt_entry.slot_info[i].soft_ver  = p[1];
                    g_sdhmgt_entry.slot_info[i].fpgaa_ver = p[2];
                    g_sdhmgt_entry.slot_info[i].fpgab_ver = p[3];
                    g_sdhmgt_entry.slot_info[i].id        = p[4];
                }

                p += 5;
            }

            sdhmgt_TX_card_update();
            break;

        case MSDH_CMD_TX_DOWNLOAD:
            send_slot_id = *p++;

            if (send_slot_id != g_sdhmgt_entry.sdh_master_slot)
            {
                sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_DOWNLOAD, 0x02);
            }
            else
            {
                if (1 == g_sdhmgt_entry.config_file)
                {
                    if (SDHMGT_TX1_SLOT == g_sdhmgt_entry.sdh_master_slot || SDHMGT_TX2_SLOT == g_sdhmgt_entry.sdh_master_slot)
                    {
                        sdhmgt_TX_requset_download_sdh_config(g_sdhmgt_entry.sdh_master_slot + 2);

                        sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_DOWNLOAD, 0x00);
                    }
                    else
                    {
                        sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_DOWNLOAD, 0x02);
                    }
                }
                else
                {
                    sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_DOWNLOAD, 0x01);
                }
            }

            break;

        case MSDH_CMD_TX_UPLOAD:
            send_slot_id = *p++;

            if (send_slot_id != g_sdhmgt_entry.sdh_master_slot)
            {
                sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_DOWNLOAD, 0x02);
            }
            else
            {
                if (SDHMGT_TX1_SLOT == g_sdhmgt_entry.sdh_master_slot || SDHMGT_TX2_SLOT == g_sdhmgt_entry.sdh_master_slot)
                {
                    sdhmgt_TX_requset_upload_sdh_config(g_sdhmgt_entry.sdh_master_slot + 2);
                    sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_UPLOAD, 0x00);
                }
                else
                {
                    sdhmgt_send_msdh_ack(send_slot_id, MSDH_CMD_TX_UPLOAD, 0x02);
                }
            }

            break;

        case MSDH_CMD_GET_EEPROM:
            status       = *p++;

            if (0 == status)
            {
                send_slot_id = *p++;

                if (send_slot_id < SDHMGT_AGENT_SDH_CARD_NUM)
                {
                    g_sdhmgt_entry.slot_eeprom_get_cnt[send_slot_id] = 0;

                    // update to hal
                    sdhmgt_TX_card_eeprom_update(send_slot_id, p, 256);
                }
            }

            break;

        default:
            break;
    }

    return 0;
}

/* socket listen */
void sdhmgt_socket_select_loop(void)
{
    int             i = 0;
    int             rc = 0;
    int             ret = 0;
    fd_set          readfds;
    uint32_t        diff;
    struct timeval  timeout;
    struct sysinfo  s_info;

    while (1)
    {
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;

        if (g_sdhmgt_entry.ms_status == DEVM_HA_MASTER)
        {
            rc = 0;
            FD_ZERO(&readfds);

            for (i = 0; i < SDHMGT_SOCKET_MAX; i++)
            {
                if (-1 != g_sdhmgt_entry.socket[i])
                {
                    rc = 1;
                    FD_SET(g_sdhmgt_entry.socket[i], &readfds);
                }
            }

            if (-1 == g_sdhmgt_entry.max_fd || 0 == rc)
            {
                usleep(10000);
            }
            else
            {
                ret = select(g_sdhmgt_entry.max_fd + 1, &readfds, NULL, 0, &timeout);

                if (ret < 0)
                {
//                    SDHMGT_ERROR("[%s:%s:%d]: select error\n", __FILE__, __FUNCTION__, __LINE__);
                }
                else
                {
                    if (FD_ISSET(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_1], &readfds))
                    {
                        sdhmgt_recv_msdh_from_sdh_device(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_1]);
                    }

                    if (FD_ISSET(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_2], &readfds))
                    {
                        sdhmgt_recv_msdh_from_sdh_card(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT8_UPORT_2]);
                    }

                    if (FD_ISSET(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_1], &readfds))
                    {
                        sdhmgt_recv_msdh_from_sdh_device(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_1]);
                    }

                    if (FD_ISSET(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_2], &readfds))
                    {
                        sdhmgt_recv_msdh_from_sdh_card(g_sdhmgt_entry.socket[SDHMGT_SOCKET_FOR_SLOT9_UPORT_2]);
                    }
                }
            }

            if (g_sdhmgt_entry.sdh_master_slot == SDHMGT_TX1_SLOT || g_sdhmgt_entry.sdh_master_slot == SDHMGT_TX2_SLOT)
            {
                sysinfo(&s_info);
                diff = s_info.uptime - g_sdhmgt_entry.sdh_hello_time;

                if (diff > SDHMGT_HELLO_TIMEOUT)
                {
                    memset(g_sdhmgt_entry.slot_info, 0, sizeof(g_sdhmgt_entry.slot_info));
                    sdhmgt_TX_card_update();

                    g_sdhmgt_entry.sdh_master_slot = 0;
                }
            }
        }
        else // slave
        {
            memset(g_sdhmgt_entry.slot_last, 0, sizeof(g_sdhmgt_entry.slot_info));
            usleep(10000);
        }
    }
}

void sdhmgt_socket_init(void)
{
    struct sockaddr_in client_addr;
    int i = 0;
    int ret = 0;
    int sock = 0;
    int flags = 0;
    uint32_t ip_addr[SDHMGT_SOCKET_MAX] = {SDHMGT_ETH8_IP, SDHMGT_ETH8_IP, SDHMGT_ETH9_IP, SDHMGT_ETH9_IP};
    uint16_t udpport[SDHMGT_SOCKET_MAX] = {SDHMGT_UPORT_1, SDHMGT_UPORT_2, SDHMGT_UPORT_1, SDHMGT_UPORT_2};

    SDHMGT_DEBUG(0, "%s[%d]:leave %s:enter function\n", __FILE__, __LINE__, __FUNCTION__);

    g_sdhmgt_entry.max_fd = -1;

    for (i = 0; i < SDHMGT_SOCKET_MAX; i++)
    {
        g_sdhmgt_entry.socket[i] = -1;
    }

    for (i = 0; i < SDHMGT_SOCKET_MAX; i++)
    {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sock < 0)
        {
            SDHMGT_ERROR("%s[%d]:leave %s:socket create error!\n", __FILE__, __LINE__, __FUNCTION__);
            perror("socket create error:\n");
            return;
        }

        flags = fcntl(sock, F_GETFL, 0);
        fcntl(sock, F_SETFL, flags | O_NONBLOCK);

        memset(&client_addr, 0, sizeof(struct sockaddr_in));
        client_addr.sin_family      = AF_INET;
        client_addr.sin_port        = htons(udpport[i]);
        client_addr.sin_addr.s_addr = htonl(ip_addr[i]);
        bzero(&client_addr.sin_zero, 8);

        ret = bind(sock, (struct sockaddr *)&client_addr, sizeof(client_addr));

        if (ret)
        {
            SDHMGT_ERROR("%s[%d]:leave %s:socket bind error!\n", __FILE__, __LINE__, __FUNCTION__);
            perror("socket bind error:\n");
        }

        g_sdhmgt_entry.socket[i] = sock;

        if (sock > g_sdhmgt_entry.max_fd)
        {
            g_sdhmgt_entry.max_fd = sock;
        }
    }
}


