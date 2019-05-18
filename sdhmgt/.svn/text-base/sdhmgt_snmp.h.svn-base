/**********************************************************
* file name: sdhmgt_snmp.h
* Copyright:
     Copyright 2018 huahuan.
* author:
*    huahuan chengquan 2018-01-15
* function:
*
* modify:
*
***********************************************************/

#ifndef __SDHMGT_SNMP_H__
#define __SDHMGT_SNMP_H__


#include <lib/msg_ipc_n.h>


/* send trap or msdh ack to snmpd */
extern void     sdhmgt_msg_to_snmp(uint8_t *data, int data_len, uint8_t subtype);

/* receive msdh message from snmpd */
extern int      sdhmgt_msg_rcv_snmp(struct ipc_mesg_n *pmesg);

#endif

