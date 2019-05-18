/**********************************************************
* file name: sdhmgt_msg.h
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-15
* function:
*
* modify:
*
***********************************************************/

#ifndef _SDHMGT_MSG_H_
#define _SDHMGT_MSG_H_

/*
 * MX slave/master change
 * when slave -> master, this function update g_sdhmgt_entry
 */
void sdhmgt_MX_master_change(int new_status);

/*
 * send ipc message to devm, get mx master/slave
 */
void sdhmgt_get_MX_master_slave(void);

/*
 * send ipc message to filem, TX card(slotid) wait send config file to MX
 */
void sdhmgt_TX_requset_download_sdh_config(int slotid);

/*
 * send ipc message to filem, TX card(slotid) wait get config file from MX
 */
void sdhmgt_TX_requset_upload_sdh_config(int slotid);

/*
 * send ipc message to hsl(devm), TX card info update
 */
void sdhmgt_TX_card_update(void);
void sdhmgt_TX_card_eeprom_update(int t_slot, uint8_t *pdata, int data_len);

/*
 * send ipc message to hal(synce), get back trib clock info
 */
void sdhmgt_get_synce_clock(void);

/*
 * send ipc message to devm, get system base mac
 */
void sdhmgt_get_system_base_mac(void);

/*
 * send ipc message to devm, get device id, for check H9MOLMXE_VX
 */
void sdhmgt_get_system_device_id(void);

#endif

