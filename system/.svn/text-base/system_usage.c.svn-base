/******************************************************************************
 * Filename: system_usage.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system usage calculate .
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/

#include<stdio.h>
#include<string.h>
#include<dirent.h>
#include<sys/stat.h>
#include <unistd.h>
#include"lib/alarm.h"
#include"lib/msg_ipc.h"
#include"lib/log.h"

#include"system_usage.h"
#include"system_alarm.h"

jiffy_counts_t cpu_jif[CPU_NUM_MAX];
jiffy_counts_t cpu_prev_jif[CPU_NUM_MAX];
unsigned char system_alarm_gloal_flag = 0;  /* system告警位 bit0 = 1 : already report CPU_high_usage alarm
                                                            bit0 = 0 : no CPU_high_usage alarm
                                                            bit1 = 1 : already report MEM_high_usage alarm
                                                            bit1 = 0 : no MEM_high_usage alarm
                                                            bit2 = 1 : already report MEM_DRY alarm
                                                            bit2 = 0 : no MEM_DRY alarm */
proc_all_state proc_info_total;
proc_prev_data_t proc_prev_gloal; //保存进程的上一周期的cpu使用滴答。
extern sys_global_conf_t sys_conf;

/************************************************
 * Function: sysm_total_add
 * Input:    proc_data : 进程数据指针
 * Output:   NONE
 * Return:   0 : 数据添加成功  -1 : 数据添加失败
 * Description:  将获取到的进程信息添加到进程数据结构内.
 ************************************************/
static int sysm_total_add(procinfo *proc_data)
{
    int cnt;

    if (NULL == proc_data)
    {
        return -1;
    }

    cnt = proc_info_total.proc_cnt;
    memcpy(&proc_info_total.proc_data[cnt], proc_data, sizeof(procinfo));
    proc_info_total.proc_cnt++;
    return 0;
}

/************************************************
 * Function: get_proc_info
 * Input:    p : 进程文件中的stat文件的内容
 * Output:   pinfo : 进程的各个参数数据结构
 * Return:   0 : 数据添加成功  -1 : 数据添加失败
 * Description:  通过/proc/pid/stat文件，读取该进程的各个参数值
 ************************************************/
static int get_proc_info(char *p, procinfo *pinfo)
{
    char *s, *t;

    if (NULL == p || NULL == pinfo)
    {
        printf("%s : failed\n", __FUNCTION__);
        return -1;
    }

    /** pid **/
    sscanf(p, "%u", &(pinfo->pid));
    s = strchr(p, '(') + 1;
    t = strchr(p, ')');
    strncpy(pinfo->exName, s, t - s);
    pinfo->exName [t - s] = '\0';
    //              1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33
    sscanf((t + 2), "%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
           &(pinfo->state),        &(pinfo->ppid),     &(pinfo->pgrp),     &(pinfo->session),      &(pinfo->tty),
           &(pinfo->tpgid),        &(pinfo->flags),    &(pinfo->minflt),   &(pinfo->cminflt),      &(pinfo->majflt),
           &(pinfo->cmajflt),      &(pinfo->utime),    &(pinfo->stime),    &(pinfo->cutime),       &(pinfo->cstime),
           &(pinfo->counter),      &(pinfo->priority), &(pinfo->timeout),  &(pinfo->itrealvalue),  &(pinfo->starttime),
           &(pinfo->vsize),        &(pinfo->rss),      &(pinfo->rlim),     &(pinfo->startcode),    &(pinfo->endcode),
           &(pinfo->startstack),   &(pinfo->kstkesp),  &(pinfo->kstkeip),  &(pinfo->signal),       &(pinfo->blocked),
           &(pinfo->sigignore),    &(pinfo->sigcatch), &(pinfo->wchan));

    return 0;
}

/************************************************
 * Function: sysm_update_info_from_proc_path
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description:  通过/proc/pid/stat文件，读取该进程的各个参数值
 ************************************************/
int sysm_update_info_from_proc_path(void)
{
    DIR *dir;
    int pid = -1;
    struct dirent *ptr;
    FILE *fp;
    char filepath[64];
    char stat_str[STAT_STR_MAX], *s;
    procinfo proc_data;
    memset(&proc_data, 0, sizeof(procinfo));
    dir = opendir("/proc");

    if (NULL != dir)
    {
        while ((ptr = readdir(dir)) != NULL)
        {
            if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))
            {
                continue;
            }

            if (DT_DIR != ptr->d_type) //只读取普通目录
            {
                continue;
            }

            sprintf(filepath, "/proc/%s/stat", ptr->d_name);

            if ((fp = fopen(filepath, "r")) == NULL)
            {
                continue;
            }

            if ((s = fgets(stat_str, STAT_STR_MAX - 1, fp)) == NULL)
            {
                fclose(fp);
                continue;
            }

            if (get_proc_info(stat_str, &proc_data) != 0)
            {
                fclose(fp);
                continue;
            }

            sysm_total_add(&proc_data);
            fclose(fp);
        }

        closedir(dir);
    }

    return pid;

}
#if 0
/************************************************
 * Function: get_items
 * Input:    buffer : 数据区指针  ie : 第几项
 * Output:   none
 * Return:   非NULL : 数据遍历成功，并返回字符串指针  NULL : 数据遍历失败
 * Description:  获取以空格为间隔的第几个字符串
 ************************************************/
static char *get_items(char *buffer, int ie)
{
    char *p = NULL;

    if (NULL == buffer)
    {
        return NULL;
    }

    p = buffer;
    int len = strlen(buffer);
    int count = 0;//统计空格数

    if (1 == ie || ie < 1)
    {
        return p;
    }

    int i;

    for (i = 0; i < len; i++)
    {
        if (' ' == *p)
        {
            count++;

            if (count == ie - 1)
            {
                p++;
                break;
            }
        }

        p++;
    }

    return p;
}
#endif
/************************************************
 * Function: get_cpu_total_occupy
 * Input:    none
 * Output:   none
 * Return:   非0  : 数据遍历成功，并返回当前总的CPU时间滴答  0 : 数据获取失败
 * Description: 获取当前总CPU时间滴答，用于计算各个进程的CPU使用率
 ************************************************/
static unsigned long long get_cpu_total_occupy(void)
{
    FILE *fp;
    unsigned long long  user, nice, sys, idle;
    char buff[1024] = {0};
    char name[16];
    fp = fopen("/proc/stat", "r");

    if (NULL == fp)
    {
        printf("fopen /proc/stat failed!\n");
        return 0;
    }

    fgets(buff, sizeof(buff), fp);

    sscanf(buff, "%s %Lu %Lu %Lu %Lu", name, &user, &nice, &sys, &idle);


    fclose(fp);     //关闭文件fd
    return (user + nice + sys + idle);
}
#if 0
/************************************************
 * Function: get_proc_occupy
 * Input:    pid : 进程ID
 * Output:   none
 * Return:   非0  : 数据遍历成功，并返回当前总的CPU时间滴答  0 : 数据获取失败
 * Description: 获取某一进程当前使用的总的CPU时间滴答
 ************************************************/
static unsigned long long get_proc_occupy(const int pid)
{
    char file[64] = {0};
    unsigned long long  utime, stime, cutime, cstime;

    FILE *fd;
    char line_buff[1024] = {0};
    sprintf(file, "/proc/%d/stat", pid); //文件中第11行包含着

    fd = fopen(file, "r");  //以R读的方式打开文件再赋给指针fd

    if (NULL == fd)
    {
        return -1;
    }

    fgets(line_buff, sizeof(line_buff), fd);  //从fd文件中读取长度为buff的字符串再存到起始地址为buff这个空间里

    char *q = get_items(line_buff, PROC_TIME_ITEM); //取得从第14项开始的起始指针
    sscanf(q, "%Lu %Lu %Lu %Lu", &utime, &stime, &cutime, &cstime); //格式化第14,15,16,17项

    fclose(fd);     //关闭文件fd
    return (utime + stime + cutime + cstime);
}
#endif
/************************************************
 * Function: update_sys_mem_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 更新系统内存使用率
 ************************************************/
int update_sys_mem_usage(void)
{
    FILE *fp = NULL;
    char buf[TEMPBUFSIZE];

    fp = fopen("/proc/meminfo", "r");

    if (NULL == fp)
    {
        printf("%s : fopen meminfo failed!\n", __FUNCTION__);
        return -1;
    }

    memset(buf, 0, TEMPBUFSIZE);
    fgets(buf, TEMPBUFSIZE - 1, fp);
    sscanf(buf, "%*s %d", &sys_conf.mem_info.mem_total);

    memset(buf, 0, TEMPBUFSIZE);
    fgets(buf, TEMPBUFSIZE - 1, fp);
    sscanf(buf, "%*s %d", &sys_conf.mem_info.mem_free);
    sys_conf.mem_info.mem_usage =  \
                                   (int)(((long long int)((long long int)sys_conf.mem_info.mem_total - (long long int)sys_conf.mem_info.mem_free) * (long long int)10000) / (long long int)sys_conf.mem_info.mem_total);
    fclose(fp);

    if ((sys_conf.mem_info.mem_usage >= sys_conf.system_mem_alarm_threshold ) \
            && !(system_alarm_gloal_flag & SYSTEM_MEM_USAGE_HIGH_ALARM))
    {
        SystemAlarmReport(SYSTEM_MEM_USAGE_HIGH_ALARM, GPN_SOCK_MSG_OPT_RISE, 0, 0);
        system_alarm_gloal_flag |= SYSTEM_MEM_USAGE_HIGH_ALARM;
        zlog(NULL, LOG_ALERT, "Memory is too low:\ttotal %dKB  free %dKB usage: %d.%d%%\n" \
             , sys_conf.mem_info.mem_total, sys_conf.mem_info.mem_free,sys_conf.mem_info.mem_usage / 100 ,sys_conf.mem_info.mem_usage % 100);
    }
    else if ((sys_conf.mem_info.mem_usage < sys_conf.system_mem_alarm_threshold) \
             && (system_alarm_gloal_flag & SYSTEM_MEM_USAGE_HIGH_ALARM))
    {
        SystemAlarmReport(SYSTEM_MEM_USAGE_HIGH_ALARM, GPN_SOCK_MSG_OPT_CLEAN, 0, 0);
        system_alarm_gloal_flag &= ~SYSTEM_MEM_USAGE_HIGH_ALARM;
    }

    if ((sys_conf.mem_info.mem_usage >= SYSTEM_MEM_DRY_THRESHOLD) \
            && !(system_alarm_gloal_flag & SYSTEM_MEM_USAGE_DRY_ALARM))
    {
        SystemAlarmReport(SYSTEM_MEM_USAGE_DRY_ALARM, GPN_SOCK_MSG_OPT_RISE, 0, 0);
        system_alarm_gloal_flag |= SYSTEM_MEM_USAGE_DRY_ALARM;
        zlog(NULL, LOG_ALERT, "Memory is dried :\ttotal %dKB  free %dKB \n" \
             , sys_conf.mem_info.mem_total, sys_conf.mem_info.mem_free);
    }
    else if ((sys_conf.mem_info.mem_usage < SYSTEM_MEM_DRY_THRESHOLD) \
             && (system_alarm_gloal_flag & SYSTEM_MEM_USAGE_DRY_ALARM))
    {
        SystemAlarmReport(SYSTEM_MEM_USAGE_DRY_ALARM, GPN_SOCK_MSG_OPT_CLEAN, 0, 0);
        system_alarm_gloal_flag &= ~SYSTEM_MEM_USAGE_DRY_ALARM;
    }

    return 0;
}

/************************************************
 * Function: update_app_mem_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 更新各个进程内存使用率
 ************************************************/
static int update_app_mem_usage(void)
{
    int cnt;

    for (cnt = 0; cnt < proc_info_total.proc_cnt; cnt++)
    {
        proc_info_total.proc_data[cnt].mem_usage = \
                (long long int)((long long int)(proc_info_total.proc_data[cnt].vsize / 1024) * 10000) / (long long int)sys_conf.mem_info.mem_total;
    }

    return 0;
}

/************************************************
 * Function: sysm_read_cpu_jiffy
 * Input:    fp : 文件指针    p_jif : CPU时钟滴答数据结构指针
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 获取/proc/stat中存放的CPU时钟滴答，用于计算总的CPU使用率
 ************************************************/
static int sysm_read_cpu_jiffy(FILE *fp, jiffy_counts_t *p_jif)
{
    char line_buf[LINE_BUF_SIZE];

    static const char fmt[] = "%s %llu %llu %llu %llu %llu %llu %llu %llu";

    int ret;

    if (!fgets(line_buf, LINE_BUF_SIZE, fp) || line_buf[0] != 'c' /* not "cpu" */)
    {
        return 0;
    }

    ret = sscanf(line_buf, fmt, p_jif->name,
                 &p_jif->usr, &p_jif->nic, &p_jif->sys, &p_jif->idle,
                 &p_jif->iowait, &p_jif->irq, &p_jif->softirq,
                 &p_jif->steal);

    if (ret >= 4)
    {
        p_jif->total = p_jif->usr + p_jif->nic + p_jif->sys + p_jif->idle
                       + p_jif->iowait + p_jif->irq + p_jif->softirq + p_jif->steal;
        /* procps 2.x does not count iowait as busy time */
        p_jif->busy = p_jif->total - p_jif->idle - p_jif->iowait;
    }

    return ret;
}

/************************************************
 * Function: sysm_cal_cpu_jiffy
 * Input:    p_jif : CPU当前时钟滴答数据结构指针    p_prev_jif : CPU上个周期时钟滴答数据结构指针 p_cpu : cpu 数据指针
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 计算某个CPU的总的使用率
 ************************************************/
static int sysm_cal_cpu_jiffy(jiffy_counts_t *p_jif, jiffy_counts_t *p_prev_jif, cpu_info_t *p_cpu)
{
    if (NULL == p_jif || NULL == p_prev_jif || NULL == p_cpu)
    {
        return -1;
    }

    p_cpu->idle_usage = ((p_jif->idle - p_prev_jif->idle) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->sys_usage = ((p_jif->sys - p_prev_jif->sys) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->usr_usage = ((p_jif->usr - p_prev_jif->usr) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->nice_usage = ((p_jif->nic - p_prev_jif->nic) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->io_usage  = ((p_jif->iowait - p_prev_jif->iowait) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->irq_usage = ((p_jif->irq - p_prev_jif->irq) * 10000) / (p_jif->total - p_prev_jif->total);
    p_cpu->sirq_usage = ((p_jif->softirq - p_prev_jif->softirq) * 10000) / (p_jif->total - p_prev_jif->total);
    strcpy(p_cpu->name, p_jif->name);
    return 0;
}
static int cpu_update_first_flag = 0;
/************************************************
 * Function: update_sys_cpu_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 更新当前系统所有CPU的总的使用率
 ************************************************/
static int update_sys_cpu_usage(void)
{
    FILE *fp = NULL;
    fp = fopen("/proc/stat", "r");

    if (NULL == fp)
    {
        return -1;
    }

    if (!sys_conf.num_cpus)
    {
        while (1)
        {
            if (sysm_read_cpu_jiffy(fp, &cpu_jif[sys_conf.num_cpus]) <= 4)
            {
                break;
            }

            cpu_prev_jif[sys_conf.num_cpus] = cpu_jif[sys_conf.num_cpus];
            sys_conf.num_cpus++;
        }

        usleep(50000);
    }
    else
    {
        unsigned int i;

        for (i = 0; i < sys_conf.num_cpus; i++)
        {
            sysm_read_cpu_jiffy(fp, &cpu_jif[i]);
            sysm_cal_cpu_jiffy(&cpu_jif[i], &cpu_prev_jif[i], &sys_conf.cpu_info[i]);
            memcpy(&cpu_prev_jif[i], &cpu_jif[i], sizeof(jiffy_counts_t));
        }
    }

    if (cpu_update_first_flag == 0)
    {
        cpu_update_first_flag = 1;
        fclose(fp);
        return 0;
    }

    if (((10000 - sys_conf.cpu_info[0].idle_usage) >= sys_conf.system_cpu_alarm_threshold ) \
            && !(system_alarm_gloal_flag & SYSTEM_CPU_USAGE_HIGH_ALARM))
    {
        SystemAlarmReport(SYSTEM_CPU_USAGE_HIGH_ALARM, GPN_SOCK_MSG_OPT_RISE, 0, 0);
        system_alarm_gloal_flag |= SYSTEM_CPU_USAGE_HIGH_ALARM;
        zlog(NULL, LOG_ALERT, "CPU usage is too high :  %d.%d%%", (10000 - sys_conf.cpu_info[0].idle_usage) / 100, (10000 - sys_conf.cpu_info[0].idle_usage) % 100);
    }
    else if (((10000 - sys_conf.cpu_info[0].idle_usage) < sys_conf.system_cpu_alarm_threshold) \
             && (system_alarm_gloal_flag & SYSTEM_CPU_USAGE_HIGH_ALARM))
    {
        SystemAlarmReport(SYSTEM_CPU_USAGE_HIGH_ALARM, GPN_SOCK_MSG_OPT_CLEAN, 0, 0);
        system_alarm_gloal_flag &= ~SYSTEM_CPU_USAGE_HIGH_ALARM;
    }

    fclose(fp);
    return 0;
}

/************************************************
 * Function: sysm_sort_app_by_cpu
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 遍历所有的进程数据结构，根据进程使用率进行排序
 ************************************************/
static int sysm_sort_app_by_cpu(void)
{
    procinfo temp;
    int i, j;

    for (i = 0; i < proc_info_total.proc_cnt; i++)
        for (j = 0; j < proc_info_total.proc_cnt - i; j++)
        {
            if (proc_info_total.proc_data[j].cpu_usage < proc_info_total.proc_data[j + 1].cpu_usage)
            {
                memcpy(&temp, &proc_info_total.proc_data[j + 1], sizeof(procinfo));
                memcpy(&proc_info_total.proc_data[j + 1], &proc_info_total.proc_data[j], sizeof(procinfo));
                memcpy(&proc_info_total.proc_data[j], &temp, sizeof(procinfo));
            }
        }

    return 0;
}
static unsigned long long system_get_proc_prev_tm(int pid)
{
    int index;

    for (index = 0; index < proc_prev_gloal.proc_cnt; index++)
    {
        if (proc_prev_gloal.old_proc_buzy_tm[index].pid == pid)
        {
            return proc_prev_gloal.old_proc_buzy_tm[index].old_tm;
        }
    }

    return 0;
}
/************************************************
 * Function: update_app_cpu_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 遍历所有的进程数据结构，更新进程的cpu使用率，和内存占用情况，并根据进程使用率进行排序
 ************************************************/
static int update_app_cpu_usage(void)
{
    long long int cnt, cpu_usage_temp;
    unsigned long long cur_cpu_total_tm = 0;
    unsigned long long cur_proc_buzy_tm = 0;
    unsigned long long prev_proc_buzy_tm = 0;

    cur_cpu_total_tm = get_cpu_total_occupy();
    proc_prev_gloal.proc_cnt = proc_info_total.proc_cnt;

    for (cnt = 0; cnt < proc_info_total.proc_cnt; cnt++)
    {
        cur_proc_buzy_tm = proc_info_total.proc_data[cnt].utime + proc_info_total.proc_data[cnt].stime + \
                           proc_info_total.proc_data[cnt].cutime + proc_info_total.proc_data[cnt].cstime;

        if (cur_proc_buzy_tm == 0)
        {
            proc_info_total.proc_data[cnt].cpu_usage = 0;
            continue;
        }

        prev_proc_buzy_tm = system_get_proc_prev_tm(proc_info_total.proc_data[cnt].pid);
        cpu_usage_temp = ((cur_proc_buzy_tm - prev_proc_buzy_tm) * 10000) \
                         / (cur_cpu_total_tm - proc_prev_gloal.old_cpu_total_tm);

        if (cpu_usage_temp < 10000) // 增加计算得到的CPU使用率结果的检测，如果超过100%，则沿用上个周期计算所得。
        {
            proc_info_total.proc_data[cnt].cpu_usage = cpu_usage_temp;
        }

        proc_prev_gloal.old_proc_buzy_tm[cnt].old_tm = cur_proc_buzy_tm;
        proc_prev_gloal.old_proc_buzy_tm[cnt].pid = proc_info_total.proc_data[cnt].pid;

    }

    proc_prev_gloal.old_cpu_total_tm = cur_cpu_total_tm;
    sysm_sort_app_by_cpu();
    return 0;
}

/************************************************
 * Function: sysm_update_mem_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 更新系统和进程的内存使用
 ************************************************/
void sysm_update_mem_usage(void)
{
    update_sys_mem_usage();
    update_app_mem_usage();
}

/************************************************
 * Function: sysm_update_cpu_usage
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 更新系统和进程的cpu使用率
 ************************************************/
void sysm_update_cpu_usage(void)
{
    update_sys_cpu_usage();
    update_app_cpu_usage();
}

/************************************************
 * Function: sysm_init_proc_total_data
 * Input:    none
 * Output:   none
 * Return:   0 : 数据遍历成功  -1 : 数据遍历失败
 * Description: 初始化进程信息数据结构，用于每次获取数据前清除数据
 ************************************************/
void sysm_init_proc_total_data(void)
{
    //memset(&proc_info_total,0,sizeof(proc_all_state));
    proc_info_total.proc_cnt = 0;
}

