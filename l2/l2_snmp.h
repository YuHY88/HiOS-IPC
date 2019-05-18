/*
*  Copyright (C) 2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: l2_snmp.h
*
*  date: 2017.1
*
*  modify:
*
*/

#ifndef _L2_SNMP_H_
#define _L2_SNMP_H_

enum L2_SNMP_SUBTYPE_INFO
{
	/*lldp snmp subtype*/
	IPC_TYPE_SNMP_LLDP_GLOBAL_CFG = 1,
	IPC_TYPE_SNMP_LLDP_PORT,			/*port msg*/
	IPC_TYPE_SNMP_LLDP_PORT_CONFIG,
	IPC_TYPE_SNMP_LLDP_STATS_TXRX_PORT,
	IPC_TYPE_SNMP_LLDP_LOC_PORT,
	IPC_TYPE_SNMP_LLDP_LOC_PORT_PRIV,
	IPC_TYPE_SNMP_LLDP_PORT_MSAP,		/*port msap msg*/
	IPC_TYPE_SNMP_LLDP_PORT_MSAP_MAN_ADDR,		/*port msap man addr msg*/

	IPC_TYPE_ALARM_LLDP_INSERT_TRAP,
	IPC_TYPE_ALARM_LLDP_DELETE_TRAP,
	IPC_TYPE_ALARM_LLDP_AGEOUT_TRAP,

	IPC_TYPE_ALARM_EFMAGENT_TRAP,
	/*loopdetect snmp subtype*/
	IPC_TYPE_SNMP_LOOPDETECT_INFO,
        IPC_TYPE_SNMP_EFM_INFO,
        IPC_TYPE_SNMP_EFM_AGENT_INFO,
	IPC_TYPE_SNMP_MSTP_INFO,
	/*l2 module other subtype*/

};




#endif

