/**********************************************************
* file name: sdhmgt_msg.c
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
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <pthread.h>
#include <assert.h>

#include "thread.h"

#include "errcode.h"
#include "msg_ipc.h"
#include "linklist.h"
#include "devm_com.h"
#include "clock/clock_if.h"

#include "sdhmgt_main.h"
#include "sdhmgt_socket.h"
#include "sdhmgt_snmp.h"
#include "sdhmgt.h"

#include "sdhmgt_msg.h"

void sdhmgt_MX_master_change(int new_status)
{
    if (new_status == DEVM_HA_MASTER)
    {
        sleep(3);      // FIXME

        sdhmgt_get_system_base_mac();
        sdhmgt_get_synce_clock();
    }
}

void sdhmgt_get_MX_master_slave(void)
{
    struct devm_unit para_devm_unit;
    int unit = 1;
    int ret = 0;

    ret = devm_comm_get_unit(unit, MODULE_ID_SDHMGT, &para_devm_unit);

    if (0 != ret)
    {
        g_sdhmgt_entry.ms_status  = DEVM_HA_INVALID;
        g_sdhmgt_entry.slot_main  = 0;
        g_sdhmgt_entry.slot_local = 0;
    }
    else
    {
        if (para_devm_unit.slot_main_board == para_devm_unit.myslot)
        {
            para_devm_unit.slot_ha = DEVM_HA_MASTER;
        }
        else
        {
            para_devm_unit.slot_ha = DEVM_HA_SLAVE;
        }

        if (para_devm_unit.slot_ha != g_sdhmgt_entry.ms_status)
        {
            SDHMGT_DEBUG(0, "sdhmgt mx master change main_slot(%d) myslot(%d) ha_status(%d)\n", para_devm_unit.slot_main_board, para_devm_unit.myslot, para_devm_unit.slot_ha);

            sdhmgt_MX_master_change(para_devm_unit.slot_ha);
        }

        g_sdhmgt_entry.ms_status  = para_devm_unit.slot_ha;
        g_sdhmgt_entry.slot_main  = para_devm_unit.slot_main_board;
        g_sdhmgt_entry.slot_local = para_devm_unit.myslot;
    }
}

void sdhmgt_TX_requset_download_sdh_config(int slotid)
{
    struct ha_batch_req sdh_req;

    memset(&sdh_req, 0, sizeof(sdh_req));
    strcpy(sdh_req.bFile, SDH_CONFIG_FILE);
    sdh_req.slot = slotid;  // TX slot
    sdh_req.unit = 1;       // current fixed 1

    //ipc_send_filem(&sdh_req, sizeof(sdh_req), 1, MODULE_ID_FILE, MODULE_ID_SDHMGT, IPC_TYPE_HA, HA_SMSG_TXFILE_REQ, IPC_OPCODE_ADD);

	ipc_send_msg_n2(&sdh_req, sizeof(sdh_req), 1, MODULE_ID_FILE, MODULE_ID_SDHMGT,
			IPC_TYPE_HA, HA_SMSG_TXFILE_REQ, IPC_OPCODE_ADD, 0);
}

void sdhmgt_TX_requset_upload_sdh_config(int slotid)
{
    struct ha_batch_req sdh_req;

    memset(&sdh_req, 0, sizeof(sdh_req));
    strcpy(sdh_req.bFile, SDH_CONFIG_FILE);
    sdh_req.slot = slotid;  // TX slot
    sdh_req.unit = 1;       // current fixed 1

    //ipc_send_filem(&sdh_req, sizeof(sdh_req), 1, MODULE_ID_FILE, MODULE_ID_SDHMGT, IPC_TYPE_HA, HA_SMSG_TXFILE_REQ, IPC_OPCODE_GET);

	ipc_send_msg_n2(&sdh_req, sizeof(sdh_req), 1, MODULE_ID_FILE, MODULE_ID_SDHMGT,
			IPC_TYPE_HA, HA_SMSG_TXFILE_REQ, IPC_OPCODE_GET, 0);
}

void sdhmgt_TX_card_update(void)
{
    struct sdhmgt_slot_info        *pcard_mnt;
    struct sdhmgt_slot_info        *pcard_lst;
    struct devm_sdhmgt_card_mnt     slot[SDHMGT_AGENT_SDH_CARD_NUM];
    struct devm_sdhmgt_sdh_master   sdh_master;

    int         i, num, length;
    uint32_t    card_id;

    pcard_mnt = g_sdhmgt_entry.slot_info;
    pcard_lst = g_sdhmgt_entry.slot_last;

    num       = 0;
    memset(slot, 0, sizeof(slot));

    for (i = 0; i < SDHMGT_AGENT_SDH_CARD_NUM; ++i)
    {
        if (pcard_mnt[i].location != pcard_lst[i].location)
        {
            if (1 == pcard_mnt[i].location)
            {
                slot[num].opcode = 1;
                g_sdhmgt_entry.slot_eeprom_get_cnt[i] = 0x20;
            }
            else
            {
                slot[num].opcode = 2;
                g_sdhmgt_entry.slot_eeprom_get_cnt[i] = 0;
            }

            slot[num].slot          = i + 2;
            slot[num].soft_version  = pcard_mnt[i].soft_ver;
            slot[num].fpgaa_version = pcard_mnt[i].fpgaa_ver;
            slot[num].fpgab_version = pcard_mnt[i].fpgab_ver;

            card_id = sdhmgt_get_MXID_from_TXID(pcard_mnt[i].id);
            slot[num].id[0]         = 0xFE;
            slot[num].id[1]         = (uint8_t)(card_id >> 8);
            slot[num].id[2]         = (uint8_t)(card_id);
            slot[num].id[3]         = 0x00;

            num++;
        }

        pcard_lst[i] = pcard_mnt[i];
    }

    if (0 != num)
    {
        length = num * sizeof(struct devm_sdhmgt_card_mnt);

        /*ipc_send_hal(&slot, length, num, MODULE_ID_HAL, MODULE_ID_SDHMGT\
                     , IPC_TYPE_DEVM, DEVM_INFO_SDHMGT, IPC_OPCODE_UPDATE, 0);*/

		ipc_send_msg_n2(&slot, length, num, MODULE_ID_HAL, MODULE_ID_SDHMGT,
			IPC_TYPE_DEVM, DEVM_INFO_SDHMGT, IPC_OPCODE_UPDATE, 0);
    }

    // update sdh master info to hal
    if (g_sdhmgt_entry.sdh_master_last != g_sdhmgt_entry.sdh_master_slot)
    {
        g_sdhmgt_entry.sdh_master_last = g_sdhmgt_entry.sdh_master_slot;

        memset(&sdh_master, 0, sizeof(struct devm_sdhmgt_sdh_master));

        if (SDHMGT_TX1_SLOT == g_sdhmgt_entry.sdh_master_slot || SDHMGT_TX2_SLOT == g_sdhmgt_entry.sdh_master_slot)
        {
            sdh_master.slot = g_sdhmgt_entry.sdh_master_slot + 2;
        }
        else
        {
            sdh_master.slot = 0;
        }

        /*ipc_send_hal(&sdh_master, sizeof(struct devm_sdhmgt_sdh_master), 1, MODULE_ID_HAL, MODULE_ID_SDHMGT\
            , IPC_TYPE_DEVM, DEVM_INFO_SDHMGT_SDH_MASTER, IPC_OPCODE_UPDATE, 0);*/

		ipc_send_msg_n2(&sdh_master, sizeof(struct devm_sdhmgt_sdh_master), 1, MODULE_ID_HAL, MODULE_ID_SDHMGT,
			IPC_TYPE_DEVM, DEVM_INFO_SDHMGT_SDH_MASTER, IPC_OPCODE_UPDATE, 0);
    }

    for (i = 0; i < SDHMGT_AGENT_SDH_CARD_NUM; ++i)
    {
        if (0 != g_sdhmgt_entry.slot_eeprom_get_cnt[i])
        {
            if (0x00 == (g_sdhmgt_entry.slot_eeprom_get_cnt[i] & 0x07))
            {
                sdhmgt_send_msdh_eep_get(i + 2);
            }

            g_sdhmgt_entry.slot_eeprom_get_cnt[i]--;
        }
    }
}

void sdhmgt_TX_card_eeprom_update(int t_slot, uint8_t *pdata, int data_len)
{
    uint8_t     eepdata[257];

    if (NULL == pdata || data_len > 256)
    {
        return;
    }

    eepdata[0] = t_slot + 2;
    memcpy(eepdata + 1, pdata, 256);

    //ipc_send_hal(eepdata, 257, 1, MODULE_ID_HAL, MODULE_ID_SDHMGT, IPC_TYPE_DEVM, DEVM_INFO_EEPROM_INFO, IPC_OPCODE_UPDATE, 0);

	ipc_send_msg_n2(eepdata, 257, 1, MODULE_ID_HAL, MODULE_ID_SDHMGT,
			IPC_TYPE_DEVM, DEVM_INFO_EEPROM_INFO, IPC_OPCODE_UPDATE, 0);
}

void sdhmgt_get_synce_clock(void)
{
    struct ipc_mesg_n     *pmsg = NULL;
    struct syncE_sdhmgt *psynce = NULL;
    int i, num, index;

    memset(g_sdhmgt_entry.trib_clock, 0x00, sizeof(g_sdhmgt_entry.trib_clock));

    /*pmsg = ipc_send_hal_wait_reply1(NULL, 0, 1, MODULE_ID_HAL, MODULE_ID_SDHMGT,
        IPC_TYPE_SYNCE, CLOCKIF_INFO_SDHMGT_BK_GET, IPC_OPCODE_GET, 0);*/
	
	pmsg = ipc_sync_send_n2(NULL, 0, 0, MODULE_ID_HAL, MODULE_ID_SDHMGT, IPC_TYPE_SYNCE, 
		CLOCKIF_INFO_SDHMGT_BK_GET, IPC_OPCODE_GET, 0, 1000);

    if (NULL == pmsg)
    {
        SDHMGT_ERROR("%s[%d]:leave %s:query fail.\n", __FILE__, __LINE__, __FUNCTION__);
        g_sdhmgt_entry.process_clock_timeout_cnt++;

        if (g_sdhmgt_entry.process_clock_timeout_cnt >= SDHMGT_PROCESS_TIMEOUT_THR)
        {
            g_sdhmgt_entry.process_clock_timeout_cnt = SDHMGT_PROCESS_TIMEOUT_THR;
        }

        return;
    }

    g_sdhmgt_entry.process_clock_timeout_cnt = 0;

    num = pmsg->msghdr.data_num;
    psynce = (struct syncE_sdhmgt *)pmsg->msg_data;

    for (i = 0 ; i < num && i < SDHMGT_BACK_TRIB_CLOCK_NUM; ++i)
    {
        index = psynce->trib_index;

        if (index != 0 && index <= SDHMGT_BACK_TRIB_CLOCK_NUM)
        {
            memcpy(&g_sdhmgt_entry.trib_clock[index - 1], psynce, sizeof(struct syncE_sdhmgt));
        }

        psynce++;
    }

	mem_share_free(pmsg, MODULE_ID_SDHMGT);
}

void sdhmgt_get_system_base_mac(void)
{
    uint8_t mac[6];
    uint8_t unit = 1;
    uint8_t slot = 0;
    int ret = 0;

    memset(mac, 0, 6);

    ret = devm_comm_get_mac(unit, slot, MODULE_ID_SDHMGT, mac);

    if (ret)
    {
        SDHMGT_ERROR("%s[%d]:leave %s: error.\n", __FILE__, __LINE__, __func__);
        memset(g_sdhmgt_entry.sys_base_mac, 0x00, 6);
    }
    else
    {
        SDHMGT_DEBUG(0, "%s[%d]:leave %s: mac: %02x:%02x:%02x:%02x:%02x:%02x.\n",
                     __FILE__, __LINE__, __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        memcpy(g_sdhmgt_entry.sys_base_mac, mac, 6);
    }
}

void sdhmgt_get_system_device_id(void)
{
    int ret = 0;
    uint8_t unit = 1;
    uint8_t slot = 0;
    uint32_t dev_id;

    ret = devm_comm_get_id(unit, slot, MODULE_ID_SDHMGT, &dev_id);

    if (ret)
    {
        SDHMGT_ERROR("%s[%d]:leave %s: error.\n", __FILE__, __LINE__, __func__);
    }
    else
    {
        SDHMGT_DEBUG(0, "%s[%d]:leave %s: device_id: %d.\n", __FILE__, __LINE__, __func__, dev_id);
    }

    g_sdhmgt_entry.h9molmxe_vx = (ID_H9MOLMXE_VX == dev_id) ? 1 : 0;
}


