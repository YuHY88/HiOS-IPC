/******************************************************************************
 * Filename: system_usage.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system module usage calcule head file  .
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/

#ifndef _SYS_USAGE_H_
#define _SYS_USAGE_H_

#define TEMPBUFSIZE         128
#define PROC_MAX            128
#define STRLEN_MAX          64
#define STAT_STR_MAX        1024
#define PROC_TIME_ITEM      14
#define CPU_NUM_MAX         8
#define LINE_BUF_SIZE       256
#define SYSTEM_CPU_USAGE_ALARM_THRESHOLD    9000  //上报cpu使用越限告警阈值
#define SYSTEM_CPU_USAGE_NO_ALARM_THRESHOLD 8000  //上报cpu使用越限告警消除阈值

#define SYSTEM_MEM_USAGE_ALARM_THRESHOLD    9000  //上报mem使用越限告警阈值
#define SYSTEM_MEM_USAGE_NO_ALARM_THRESHOLD 8000  //上报mem使用越限告警消除阈值
#define SYSTEM_MEM_USAGE_HIGH_ALARM             0x01
#define SYSTEM_CPU_USAGE_HIGH_ALARM             0x02
#define SYSTEM_MEM_USAGE_DRY_ALARM              0x04
#define SYSTEM_CPU_USAGE_ALARM_THRESHOLD_DEFALUT    9800  //上报cpu使用越限告警阈值
#define SYSTEM_MEM_USAGE_ALARM_THRESHOLD_DEFALUT    9800  //上报mem使用越限告警阈值
#define SYSTEM_CPU_USAGE_ALARM_THRESHOLD_MIN    0  //上报cpu使用越限告警阈值
#define SYSTEM_MEM_USAGE_ALARM_THRESHOLD_MAX    99  //上报mem使用越限告警阈值
#define SYSTEM_MEM_DRY_THRESHOLD                9800

typedef struct mem_infomation
{
    int mem_total;
    int mem_free;
    int mem_usage;
} mem_info_t;

typedef struct cpu_information
{
    char name[8];
    int total_usage;
    int usr_usage;
    int sys_usage;
    int nice_usage;
    int idle_usage;
    int io_usage;
    int irq_usage;
    int sirq_usage;
} cpu_info_t;

typedef struct jiffy_counts_t
{
    /* Linux 2.4.x has only first four */
    char name[8];
    unsigned long long usr, nic, sys, idle;
    unsigned long long iowait, irq, softirq, steal;
    unsigned long long total;
    unsigned long long busy;
} jiffy_counts_t;

typedef struct statstruct_proc
{
    int           pid;                      /** The process id. **/
    char          exName [STRLEN_MAX]; /** The filename of the executable **/
    char          state; /** 1 **/          /** R is running, S is sleeping,
               D is sleeping in an uninterruptible wait,
               Z is zombie, T is traced or stopped **/
    unsigned  int  euid,                      /** effective user id **/
              egid;                      /** effective group id */
    int           ppid;                     /** The pid of the parent. **/
    int           pgrp;                     /** The pgrp of the process. **/
    int           session;                  /** The session id of the process. **/
    int           tty;                      /** The tty the process uses **/
    int           tpgid;                    /** (too long) **/
    unsigned int    flags;                    /** The flags of the process. **/
    unsigned int    minflt;                   /** The number of minor faults **/
    unsigned int    cminflt;                  /** The number of minor faults with childs **/
    unsigned int    majflt;                   /** The number of major faults **/
    unsigned int  cmajflt;                  /** The number of major faults with childs **/
    int           utime;                    /** user mode jiffies **/
    int           stime;                    /** kernel mode jiffies **/
    int     cutime;                   /** user mode jiffies with childs **/
    int           cstime;                   /** kernel mode jiffies with childs **/
    int           counter;                  /** process's next timeslice **/
    int           priority;                 /** the standard nice value, plus fifteen **/
    unsigned int  timeout;                  /** The time in jiffies of the next timeout **/
    unsigned int  itrealvalue;              /** The time before the next SIGALRM is sent to the process **/
    int           starttime; /** 20 **/     /** Time the process started after system boot **/
    unsigned int  vsize;                    /** Virtual memory size **/
    unsigned int  rss;                      /** Resident Set Size **/
    unsigned int  rlim;                     /** Current limit in bytes on the rss **/
    unsigned int  startcode;                /** The address above which program text can run **/
    unsigned int    endcode;                  /** The address below which program text can run **/
    unsigned int  startstack;               /** The address of the start of the stack **/
    unsigned int  kstkesp;                  /** The current value of ESP **/
    unsigned int  kstkeip;                 /** The current value of EIP **/
    int     signal;                   /** The bitmap of pending signals **/
    int           blocked; /** 30 **/       /** The bitmap of blocked signals **/
    int           sigignore;                /** The bitmap of ignored signals **/
    int           sigcatch;                 /** The bitmap of catched signals **/
    unsigned int  wchan;  /** 33 **/        /** (too long) **/
    int     sched,        /** scheduler **/
            sched_priority;       /** scheduler priority **/

    long long int mem_usage; /*物理内存占用率，对100取整为整数部分，对100取余为小数部分*/
    long long int cpu_usage; /*CPU使用率，对100取整为整数部分，对100取余为小数部分*/
} procinfo;
typedef struct pro_total_data
{
    int proc_cnt;
    procinfo proc_data[PROC_MAX];

} proc_all_state;
struct proc_prev_signal_data
{
    int pid;
    unsigned long long old_tm;
};
typedef struct proc_prev_data
{
    int proc_cnt;
    unsigned long long  old_cpu_total_tm;
    struct proc_prev_signal_data  old_proc_buzy_tm[PROC_MAX];
} proc_prev_data_t;

typedef struct sys_global_conf_data
{
    unsigned int num_cpus;
    int system_cpu_alarm_threshold;
    int system_mem_alarm_threshold;
    mem_info_t mem_info;
    cpu_info_t cpu_info[CPU_NUM_MAX];
    int sys_timezone;
} sys_global_conf_t;
int sysm_update_info_from_proc_path(void);

void sysm_init_proc_total_data(void);
void sysm_update_mem_usage(void);
void sysm_update_cpu_usage(void);
int update_sys_mem_usage(void);


#endif


