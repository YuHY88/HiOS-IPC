/******************************************************************************
 * Filename: system.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: system模块处理查看cpu 和 内存的使用率 主模块
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/msg_ipc_n.h>
#include <lib/memshare.h>
#include <lib/errcode.h>
#include <lib/log.h>

#include "system.h"
#include "system_usage.h"
#include "system_alarm.h"
#include "system_cmd.h"

#define SYS_LOCALTIME_CFG "/etc/localtime"

extern sys_global_conf_t sys_conf;

/************************************************
 * Function: system_update_status
 * Input:    thread : 线程指针
 * Output:   NONE
 * Return:   0 : 数据更新成功  -1 : 数据更新失败
 * Description:  system模块定时执行程序，包括更新各个进程的cpu和内存使用等.
 ************************************************/
int system_update_status(void *thread)
{
    sysm_init_proc_total_data();

    sysm_update_info_from_proc_path();

    sysm_update_cpu_usage();

    sysm_update_mem_usage();
    return 0;
}

/* 接收 IPC 消息 */
int system_msg_rcv_msg(struct ipc_mesg_n *pmesg,int imlen)
{
    int ret = 0;
    int revln = 0;
    struct ipc_msghdr_n *phdr = NULL;
    
    revln = (int)pmesg->msghdr.data_len + IPC_HEADER_LEN_N; 
    if(revln <= imlen)
    {
        /* 娑堟伅澶勭悊 */
        phdr = &(pmesg->msghdr);

        if (phdr->msg_type == IPC_TYPE_SYSTEM)
        {
            switch (phdr->msg_subtype)
            {
                case IPC_TYPE_SNMP_SYSTEM_CPU_NUM_NEXT:
                    system_next_cpu_num_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYSTEM_CPU_INFO:
                    system_snmp_cpu_info_get(phdr);
                    break;

                case IPC_TYPE_SNMP_SYSTEM_THRESHOLD_INFO:
                    system_snmp_threshold_info_get(phdr);
                    break;

                default:
                    break;
            }
        }
        else if(phdr->msg_type == IPC_TYPE_STAT_DB)
        {
            gpnSystemCoreProc((gpnSockMsg *)pmesg->msg_data,pmesg->msghdr.data_len);
        }
    }
    else
    {
         //FILEM_DEBUG("filem recv msg: datalen error, data_len=%d, msgrcv len = %d\n", revln, imlen);
         printf("%s[%d], datalen error, data_len=%d, msgrcv len = %d\n",
                        __FUNCTION__, __LINE__, revln, imlen);
    }
    
    mem_share_free(pmesg, MODULE_ID_SYSTEM);

    return ret;

}

int system_next_cpu_num_get(struct ipc_msghdr_n *phdr)
{
    int     ret;
    uint32_t index_next = 0;


    if (phdr->msg_index == 0)
    {
        index_next = 1;
    }
    else
    {
        if (phdr->msg_index < (sys_conf.num_cpus - 1))
        {
            index_next = phdr->msg_index + 1;
        }
        else
        {
            index_next = 0;
        }
    }

    zlog_debug(SYSTEM_DBG_INFO,"[%s %d]: index_next: 0x%x \n", __FUNCTION__, __LINE__, index_next);
    ret = ipc_send_reply_n2(&index_next, sizeof(uint32_t),1,phdr->sender_id,MODULE_ID_SYSTEM,  \
                            IPC_TYPE_SYSTEM,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(SYSTEM_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}
struct system_threshold threshold_value;

int system_snmp_threshold_info_get(struct ipc_msghdr_n *phdr)
{
    int ret;

    threshold_value.cpu_alarm_threshold = sys_conf.system_cpu_alarm_threshold / 100;
    threshold_value.memory_alarm_threshold = sys_conf.system_mem_alarm_threshold / 100;

    ret = ipc_send_reply_n2(&threshold_value, sizeof(struct system_threshold),1,phdr->sender_id,MODULE_ID_SYSTEM,  \
                            IPC_TYPE_SYSTEM,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);

    if (ret < 0)
    {
        zlog_debug(SYSTEM_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return ret;
}

/*get system local global cfg*/
int system_snmp_cpu_info_get(struct ipc_msghdr_n *phdr)
{
    int     ret;

    if (phdr->msg_index > 0 && phdr->msg_index < sys_conf.num_cpus)
    {
        ret = ipc_send_reply_n2(&sys_conf.cpu_info[phdr->msg_index], sizeof(cpu_info_t),1,phdr->sender_id,MODULE_ID_SYSTEM,  \
                                IPC_TYPE_SYSTEM,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
    }
    else
    {
        ret = ipc_send_reply_n2(NULL, 0,1,phdr->sender_id,MODULE_ID_SYSTEM,  \
                                IPC_TYPE_SYSTEM,phdr->msg_subtype, phdr->sequence, phdr->msg_index, IPC_OPCODE_REPLY);
    }

    if (ret < 0)
    {
        zlog_debug(SYSTEM_DBG_ERROR,"[%s %d]ERROR: ipc_send_reply ret(%d)", __FUNCTION__, __LINE__, ret);

        return -1;
    }

    return 0;
}
int system_timezone_get(void)
{
    return sys_conf.sys_timezone;
}

int system_timezone_set(int zone)
{
    if (zone >= -12 && zone <= 12)
    {
        int len;
        FILE *locatimefile;
        char  tempbuf[128] = {0};
        struct timezone tz;

        sys_conf.sys_timezone  = zone;

        memset(tempbuf, 0, 128);

        locatimefile = fopen(SYS_LOCALTIME_CFG, "wb+");

        if (locatimefile != NULL)
        {
            sprintf(tempbuf, "%s", "TZif");
            //sprintf(tempbuf+4,"%s%+d","GMT",zone);
            tempbuf[23] = 1;
            tempbuf[27] = 1;
            tempbuf[39] = 1;

            len = sprintf(tempbuf + 50, "%s%+d", "GMT", zone);

            len++;

            tempbuf[40] = (len >> 24) & 0xFF;
            tempbuf[41] = (len >> 16) & 0xFF;
            tempbuf[42] = (len >> 8) & 0xFF;
            tempbuf[43] = (len >> 0) & 0xFF;

            zone *= 3600;

            tempbuf[44] = (zone >> 24) & 0xFF;
            tempbuf[45] = (zone >> 16) & 0xFF;
            tempbuf[46] = (zone >> 8) & 0xFF;
            tempbuf[47] = (zone >> 0) & 0xFF;

            fwrite(tempbuf, sizeof(char), len + 52, locatimefile);

            fclose(locatimefile);
            //set timezone to sys_tz
            memset(&tz, 0, sizeof(struct timezone));
            tz.tz_minuteswest = system_timezone_get() * 60;
            settimeofday(NULL, &tz);

            tzset();
        }
        else
        {
            zlog_err("can not open %s !", SYS_LOCALTIME_CFG);
            return (-1);
        }
    }
    else
    {
        return (-1);
    }

    return (0);
}





