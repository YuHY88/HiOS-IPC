#ifndef HIOS_EFM_AGENT_H
#define HIOS_EFM_AGENT_H

#include "l2/efm/efm.h"
#include "l2/efm/efm_pkt.h"
/*efm agent 11000A 11000An 1101n info define refer to H0FL_P interior communication 20140117*/
#define EFM_MSDH_OUI_DATA_LEN 5
#define EFM_MSDH_FRM_HEAD_LEN 18
#define EFM_MSDH_FRM_HEAD 0x96     	/*protocol provide */
#define EFM_MSDH_VER 0x02                       /*protocol provide*/
#define EFM_MSDH_CMD_ID 0x0000              /*protocol provide*/
/*the device info of efm agent remote device refer to H0FL_P interior communication 20140117*/
#define EFM_REMOTE_DEVICE_ID 0x35		  /*protocol provide*/
#define EFM_REMOTE_11000A_TYPE 0xed                 /*protocol provide*/
#define EFM_REMOTE_11000An_TYPE 0xfa	   /*protocol provide*/
#define EFM_REMOTE_1101n_TYPE 0xeb		   /*protocol provide*/
/*HT123 user-defined*/
#define EFM_REMOTE_123_TYPE 0x03
/*(device info get) and (info get) and (info set) refer to H0FL_P interior communication 20140117*/
#define EFM_MSDH_DEVICE_GET_CMD 0x1101                //device 
#define EFM_MSDH_INFO_GET_CMD  0x4040	   /*protocol provide*/
#define EFM_MSDH_INFO_SET_CMD 0x1840	   /*protocol provide*/
#define EFM_MSDH_INFO_SET_GET_CMD 0x1040    /*protocol provide*/

#define EFM_AGENT_ALM_MIBNODE_LEN 19

#define CONFIG_WRITE_TXT  "/home/bin/agent.txt"
enum efm_agent_trap_type
{
	hpmcoltalm,
	hpmcethalm,
};

int 
efm_parse_remote_device_info(efm_agent_remote_base_info *  aefm_RbaseInfo,uint8_t * buf,uint16_t msdh_length);

int
efm_parse_msdh_device_info(efm_agent_info *aefm,uint8_t * buf,uint16_t msdh_length);

int 
efm_parse_remote_getinfo( efm_agent_remote_state_info * aefm_RstInfo,uint8_t * buf,uint8_t msdh_length);

int 
efm_parse_msdh_getinfo(efm_agent_info * aefm,uint8_t *  buf,uint16_t msdh_length);

int 
efm_parse_remote_setinfo( efm_agent_remote_set_info * aefm_RsetInfo,uint8_t * buf,uint8_t msdh_length);

int
efm_parse_msdh_setinfo(efm_agent_info * aefm,uint8_t *	buf,uint16_t msdh_length);

int 
efm_parse_remote_SetGetinfo( struct efm_if * pefm,uint8_t * buf,uint8_t msdh_length);

int 
efm_parse_msdh_SetGetinfo(struct efm_if * pefm,uint8_t * buf,uint16_t msdh_length);

int 
efm_agent_pdu_package(struct efm_if * pefm,uint8_t * buf);

int
efm_format_organization_specific_pdu (struct efm_if *pefm, register uint8_t *bufptr);

void 
efm_agent_disable(struct efm_if * pefm);

int 
efm_agent_pdu_tx (struct efm_if *pefm);

int 
efm_agent_pdu_timer_expiry(void * arg);

int 
efm_agent_trap_ThreeAlarm(efm_agent_info * aefm,uint16_t event_type,uint8_t trap_act,uint32_t ifindex);

void 
clear_efm_agent_data(efm_agent_info * aefm);

void 
efm_agent_save_remote_config_info(struct efm_if *pefm);

int 
efm_agent_trap(uint8_t data,enum efm_agent_trap_type trap_node, uint32_t ifindex);

int 
efm_trap_event_to_snmpd(uint8_t trap_data, enum efm_agent_trap_type trap_node,uint32_t ifindex);

int 
efm_agent_write_data(efm_agent_info * aefm);

int
efm_agent_u0_info_send(uint32_t ifindex,efm_agent_info *aefm,enum IPC_OPCODE opcode);

int
efm_agent_ipc_send_ospf(void * pdata,int data_len,enum IPC_OPCODE opcode);

void 
efm_agent_update_ip(struct u0_device_info * u0_device,uint16_t data_len);

void 
efm_agent_send_info_reply_dcn();

#endif


























