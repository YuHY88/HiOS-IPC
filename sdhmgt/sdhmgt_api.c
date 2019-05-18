/**********************************************************
* file name: sdhmgt_api.c
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan jianghongzhi 2018-05-9
* function:
*
* modify:
*
***********************************************************/

#include <string.h>
#include <stdio.h>

#include <lib/msg_ipc_n.h>
#include <lib/thread.h>
#include <lib/errcode.h>
#include <lib/memory.h>
#include <lib/ifm_common.h>
#include <lib/hptimer.h>

#include "sdhmgt.h"
#include "sdhmgt_socket.h"
#include "sdhmgt_snmp.h"
#include "sdhmgt_msg.h"

#include "sdhmgt_api.h"

int sdhmgt_timing_send_msdh_hello(void *para);

int sdhmgt_timing_send_msdh_hello(void *para)
{
    if (1 != g_sdhmgt_entry.h9molmxe_vx)
    {
        return 0;
    }

    if (g_sdhmgt_entry.ms_status == DEVM_HA_MASTER)
    {
        sdhmgt_config_file_recheck();

        sdhmgt_send_msdh_hello();
    }

    return 0;
}



#if 0
int sdhmgt_rcv_ipc_snmp(struct thread *t)
{
    static struct ipc_mesg mesg;

    if (1 != g_sdhmgt_entry.h9molmxe_vx)
    {
        return -1;
    }

    memset(&mesg, 0, sizeof(struct ipc_mesg));

    if (-1 == ipc_recv_msdh(&mesg, MODULE_ID_SDHMGT))
    {
        usleep(SDHMGT_IPC_LISTEN_SNMP);
        thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_snmp, NULL, 0);

        return -1;
    }

    if (1 == g_sdhmgt_debug)
    {
        SDHMGT_DEBUG(0, "sdhmgt receive snmp msdh ipc message opcode=%d msg_type=%d sender_id=%d msg_subtype=%d\n",
                     mesg.msghdr.opcode, mesg.msghdr.msg_type, mesg.msghdr.sender_id, mesg.msghdr.msg_subtype);
    }

    if (mesg.msghdr.msg_type == IPC_TYPE_SDHMGT)
    {
        if (mesg.msghdr.opcode == IPC_OPCODE_GET)
        {
            sdhmgt_msg_rcv_snmp(&mesg);
        }
    }

    usleep(SDHMGT_IPC_LISTEN_SNMP);
    thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_snmp, NULL, 0);

    return ERRNO_SUCCESS;
}


int sdhmgt_rcv_ipc_mesg(struct thread *t)
{
    static struct ipc_mesg mesg;
    struct devm_com *pDevCom = NULL;
    struct syncE_sdhmgt *psynce = NULL;
    int i, num, index;
    int ha_status;

    memset(&mesg, 0, sizeof(struct ipc_mesg));

    if (-1 == ipc_recv_common(&mesg, MODULE_ID_SDHMGT))
    {
        usleep(SDHMGT_IPC_LISTEN_MESG);
        thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_mesg, NULL, 0);

        return -1;
    }

    if (1 == g_sdhmgt_debug)
    {
        SDHMGT_DEBUG(0, "sdhmgt receive process ipc message, opcode=%d msg_type=%d sender_id=%d msg_subtype=%d phdr->msg_index=0x%0x\n",
                     mesg.msghdr.opcode, mesg.msghdr.msg_type, mesg.msghdr.sender_id, mesg.msghdr.msg_subtype, mesg.msghdr.msg_index);
    }

    if (mesg.msghdr.msg_type == IPC_TYPE_DEVM)
    {
        if (mesg.msghdr.opcode == IPC_OPCODE_EVENT)
        {
            if (mesg.msghdr.msg_subtype == DEV_EVENT_HA_BOOTSTATE_CHANGE)
            {
                pDevCom = (struct devm_com *)(mesg.msg_data);

                if (g_sdhmgt_entry.slot_local == pDevCom->slot)
                {
                    if (HA_ROLE_MASTER == pDevCom->ha_role)
                    {
                        ha_status = DEVM_HA_MASTER;
                    }
                    else if (HA_ROLE_SLAVE == pDevCom->ha_role)
                    {
                        ha_status = DEVM_HA_SLAVE;
                    }
                    else
                    {
                        ha_status = DEVM_HA_INVALID;
                    }

                    SDHMGT_DEBUG(0, "sdhmgt mx master change %d\n", ha_status);

                    sdhmgt_MX_master_change(ha_status);
                    g_sdhmgt_entry.ms_status = ha_status;
                }
            }
        }
        else if (mesg.msghdr.opcode == IPC_OPCODE_UPDATE)
        {
            if (mesg.msghdr.msg_subtype == DEVM_INFO_EEPROM_INFO)
            {
                sdhmgt_send_msdh_eep_set(mesg.msg_data[0], mesg.msg_data + 1, 256);
                sdhmgt_send_msdh_eep_get(mesg.msg_data[0]);
            }
        }
    }
    else if (mesg.msghdr.msg_type == IPC_TYPE_SYNCE)
    {
        if (mesg.msghdr.msg_subtype == CLOCKIF_INFO_SDHMGT_BK_UPDATE)
        {
            g_sdhmgt_entry.process_clock_timeout_cnt = 0;

            num = mesg.msghdr.data_num;
            psynce = (struct syncE_sdhmgt *)mesg.msg_data;

            for (i = 0 ; i < num && i < SDHMGT_BACK_TRIB_CLOCK_NUM; ++i)
            {
                index = psynce->trib_index;

                if (index != 0 && index <= SDHMGT_BACK_TRIB_CLOCK_NUM)
                {
                    memcpy(&g_sdhmgt_entry.trib_clock[index - 1], psynce, sizeof(struct syncE_sdhmgt));
                }

                psynce++;
            }
        }
    }

    usleep(SDHMGT_IPC_LISTEN_MESG);
    thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_mesg, NULL, 0);

    return ERRNO_SUCCESS;
}
#endif


int sdhmgt_handle_ipc_msg(struct ipc_mesg_n *pmsg)
{
	struct devm_com *pDevCom = NULL;
    struct syncE_sdhmgt *psynce = NULL;
    int i, num, index;
    int ha_status;
	
	switch(pmsg->msghdr.msg_subtype)
	{
		case IPC_TYPE_SNMP:
			if (1 != g_sdhmgt_entry.h9molmxe_vx)
		    {
		        return -1;
		    }

		    if (pmsg->msghdr.opcode == IPC_OPCODE_GET)
		    {
		        sdhmgt_msg_rcv_snmp(pmsg);
		    }
			break;

		case IPC_TYPE_DEVM:
	        if (pmsg->msghdr.opcode == IPC_OPCODE_EVENT)
	        {
	            if (pmsg->msghdr.msg_subtype == DEV_EVENT_HA_BOOTSTATE_CHANGE)
	            {
	                pDevCom = (struct devm_com *)(pmsg->msg_data);

	                if (g_sdhmgt_entry.slot_local == pDevCom->slot)
	                {
	                    if (HA_ROLE_MASTER == pDevCom->ha_role)
	                    {
	                        ha_status = DEVM_HA_MASTER;
	                    }
	                    else if (HA_ROLE_SLAVE == pDevCom->ha_role)
	                    {
	                        ha_status = DEVM_HA_SLAVE;
	                    }
	                    else
	                    {
	                        ha_status = DEVM_HA_INVALID;
	                    }

	                    SDHMGT_DEBUG(0, "sdhmgt mx master change %d\n", ha_status);

	                    sdhmgt_MX_master_change(ha_status);
	                    g_sdhmgt_entry.ms_status = ha_status;
	                }
	            }
	        }
	        else if (pmsg->msghdr.opcode == IPC_OPCODE_UPDATE)
	        {
	            if (pmsg->msghdr.msg_subtype == DEVM_INFO_EEPROM_INFO)
	            {
	                sdhmgt_send_msdh_eep_set(pmsg->msg_data[0], pmsg->msg_data + 1, 256);
	                sdhmgt_send_msdh_eep_get(pmsg->msg_data[0]);
	            }
	        }
			break;

		case IPC_TYPE_SYNCE:
	        if (pmsg->msghdr.msg_subtype == CLOCKIF_INFO_SDHMGT_BK_UPDATE)
	        {
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
	        }			
			break;

		default:
             printf("sdhmgt_handle_ipc_msg, receive unknown message\r\n");
             break;
	}

    return ERRNO_SUCCESS;
}


/* receive common IPC messages */
int sdhmgt_msg_rcv_n(struct ipc_mesg_n *pmsg, int imlen)
{
    if(NULL == pmsg)
	{
		return 0;
	}

	int retva = 0;
    int revln = 0;

    //printf("sdhmgt recv msg: pmsg=%p, imlen=%d\r\n", pmsg, imlen);

    revln = (int)pmsg->msghdr.data_len + IPC_HEADER_LEN_N; 

    if(revln <= imlen)
    {
        sdhmgt_handle_ipc_msg(pmsg);
    }
    else
    {
         printf("aaa recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
    }

	if(pmsg)
	{
    	mem_share_free(pmsg, MODULE_ID_SDHMGT);
	}
    
    return retva;
}




int sdhmgt_timing_query(void *para)
{
    if (1 != g_sdhmgt_entry.h9molmxe_vx)
    {
        return 0;
    }

    sdhmgt_get_MX_master_slave();

    if (g_sdhmgt_entry.process_clock_timeout_cnt < SDHMGT_PROCESS_TIMEOUT_THR)
    {
        sdhmgt_get_synce_clock();
    }

    //thread_add_timer_msec(g_psdhmgt_master, sdhmgt_timing_query, NULL, SDHMGT_IIMING_QUERY_TIMER);
    return 0;
}

void sdhmgt_ipc_init(void)
{
    /*Receiving ipc messages and processing*/
    //thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_mesg, NULL, 0);
    //thread_add_event(g_psdhmgt_master, sdhmgt_rcv_ipc_snmp, NULL, 0);

    /*Sign up for change notification*/
    devm_event_register(DEV_EVENT_HA_BOOTSTATE_CHANGE, MODULE_ID_SDHMGT, 0);

    /*Regularly query and send hello periodically*/
    //thread_add_timer_msec(g_psdhmgt_master, sdhmgt_timing_send_msdh_hello, NULL, SDHMGT_IIMING_SEND_TIMER);
    //thread_add_timer_msec(g_psdhmgt_master, sdhmgt_timing_query, NULL, SDHMGT_IIMING_QUERY_TIMER);

	/* 添加定时器，单位ms */	
	high_pre_timer_add((char *)"SdhMgt_send_msdh_hello_Timer", LIB_TIMER_TYPE_LOOP, sdhmgt_timing_send_msdh_hello, NULL, SDHMGT_IIMING_SEND_TIMER);
	high_pre_timer_add((char *)"SdhMgt_query_Timer", LIB_TIMER_TYPE_LOOP, sdhmgt_timing_query, NULL, SDHMGT_IIMING_QUERY_TIMER);
}

void sdhmgt_info_init(void)
{
    sdhmgt_get_MX_master_slave();
    sdhmgt_get_system_base_mac();
    sdhmgt_get_synce_clock();
    sdhmgt_config_file_check();
    sdhmgt_get_nm_port_ip();

    sdhmgt_socket_init();
}

void sdhmgt_get_nm_port_ip(void)
{
    unsigned char   mac[6];

    memcpy(mac, g_sdhmgt_entry.sys_base_mac, 6);

    g_sdhmgt_entry.nm_ip_addr = 0x7F000000 | (((uint32_t)mac[3]) << 16) | (((uint32_t)mac[4]) << 8) | mac[5];
}


