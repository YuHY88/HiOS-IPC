/******************************************************************************
 * Filename: system.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system module head file .
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/

#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define SYSTEM_UPDATE_INTERVAL 6000  //系统更新间隔 单位 : ms
#define SYSTEM_TIMEZONE_DAFAULT (8)

extern struct thread_master *system_master;
struct system_threshold
{
    int cpu_alarm_threshold;
    int memory_alarm_threshold;
};

enum SYSTEM_SNMP_SUBTYPE_INFO
{
    /*system snmp subtype*/
    IPC_TYPE_SNMP_SYSTEM_CPU_NUM = 1,
    IPC_TYPE_SNMP_SYSTEM_CPU_NUM_NEXT,
    IPC_TYPE_SNMP_SYSTEM_CPU_INFO,
    IPC_TYPE_SNMP_SYSTEM_THRESHOLD_INFO,
};
int system_msg_rcv(struct thread *t);

int system_update_status(void *thread);
int system_snmp_cpu_info_get(struct ipc_msghdr_n *phdr);
int system_snmp_threshold_info_get(struct ipc_msghdr_n *phdr);
int system_next_cpu_num_get(struct ipc_msghdr_n *phdr);
int system_timezone_set(int zone);
int system_timezone_get(void);
int system_msg_rcv_msg(struct ipc_mesg_n *pmesg,int imlen);


#endif
