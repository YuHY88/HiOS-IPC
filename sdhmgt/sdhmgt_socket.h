/**********************************************************
* file name: sdhmgt_socket.h
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-15
* function:
*
* modify:
*
***********************************************************/

#ifndef _SDHMGT_SOCKET_H_
#define _SDHMGT_SOCKET_H_

#include <sys/socket.h>
#include <netinet/in.h>

#define MSDH_CMD_TX_2_MX_HELLO          0x1166
#define MSDH_CMD_MX_2_TX_HELLO          0x1167
#define MSDH_CMD_TX_DOWNLOAD            0x1701
#define MSDH_CMD_TX_UPLOAD              0x1702
#define MSDH_CMD_GET_EEPROM             0x1703
#define MSDH_CMD_SET_EEPROM             0x1704

enum SDHMGT_SNMP_TRAP_SUBTYPE_INFO
{
    /*sdhmgt snmp subtype*/
    SDHMGT_SUBTYPE_INVALID = 0,
    SDHMGT_SUBTYPE_REPORT_TRAP_INFO
};

/*
 * use this function send to TX card, deal from snmp receive msdh frame,
 * TX card check, then local deal or transmit to other sdh device
 * use UDP port1, default 3201
 */
extern int      sdhmgt_send_msdh_for_sdh_device(uint8_t *pframe, int frm_len);

/*
 * use this function recv from TX card, then send to snmp process
 * use UDP port1, default 3201
 */
extern void     sdhmgt_recv_msdh_from_sdh_device(int fd);

/*
 * for MX card send message to TX card, eg. hello message, config file up/download request message
 * only between MX and TX card use
 * use UDP port2, default 3202
 */
extern int      sdhmgt_send_msdh_for_sdh_card(uint8_t slot, uint8_t *pframe, int frm_len);
/*
 * from TX card receive message, eg. hello message, config file up/download request message
 * only between MX and TX card use
 * use UDP port2, default 3202
 */
extern void     sdhmgt_recv_msdh_from_sdh_card(int fd);

/*
 *  MX master card send hello message to TX master card
 */
extern void     sdhmgt_send_msdh_hello(void);

extern void     sdhmgt_send_msdh_eep_get(int slot);
extern void     sdhmgt_send_msdh_eep_set(int slot, uint8_t *peeprom, int eeprom_len);

/*
 * send msdh ack after deal msdh frame
 */
extern void     sdhmgt_send_msdh_ack(uint8_t slot, uint16_t cmd_type, uint8_t status);

/*
 * deal msdh frame
 */
extern int      sdhmgt_deal_msdh_frame(uint8_t *pframe, int len);

/*
 * select listen
 */
extern void     sdhmgt_socket_select_loop(void);

/*
 * create socket
 */
extern void     sdhmgt_socket_init(void);

#endif

