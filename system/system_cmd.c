/******************************************************************************
 * Filename: system_cmd.c
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system module command .
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <lib/command.h>
#include <lib/module_id.h>
#include <lib/log.h>
#include <lib/devm_com.h>
#include"lib/msg_ipc_n.h"
#include"lib/memory.h"

#include "system_usage.h"
#include "system_cmd.h"
#include "system.h"

extern sys_global_conf_t sys_conf;
extern proc_all_state proc_info_total;
static struct cmd_node stSystemNode = { SYSTEM_NODE,  "%s(config-system)#",  1 };

static int check_time_range(int time_value, int min, int max)
{
    if (time_value < min || time_value > max)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
static int str_to_time_index(struct tm *timeptr, int index, char *ptr)
{
    int ret = 0;

    switch (index)
    {
        case YEAR_INDEX:
            timeptr->tm_year = atoi(ptr) - 1900;

            if (check_time_range(timeptr->tm_year, 0, 3000))
            {
                ret = -1;
            }

            break;

        case MON_INDEX:
            timeptr->tm_mon = atoi(ptr) - 1;

            if (check_time_range(timeptr->tm_mon, 0, 11))
            {
                ret = -1;
            }

            break;

        case DAY_INDEX:
            timeptr->tm_mday = atoi(ptr);

            if (check_time_range(timeptr->tm_mday, 1, 31))
            {
                ret = -1;
            }

            break;

        case HOUR_INDEX:
            timeptr->tm_hour = atoi(ptr);

            if (check_time_range(timeptr->tm_hour, 0, 23))
            {
                ret = -1;
            }

            break;

        case MIN_INDEX:
            timeptr->tm_min = atoi(ptr);

            if (check_time_range(timeptr->tm_min, 0, 59))
            {
                ret = -1;
            }

            break;

        case SEC_INDEX:
            timeptr->tm_sec = atoi(ptr);

            if (check_time_range(timeptr->tm_sec, 0, 59))
            {
                ret = -1;
            }

            break;

        default:
            ret = -1;
            break;
    }

    return ret;
}
static int sysm_str_to_time(struct tm *timeptr, const char *pstr)
{
    int ret;
    char str_buf[TEMPBUFSIZE] = {0};
    char *token = NULL;
    int str_index = 0;

    if (NULL == pstr || NULL == timeptr)
    {
        zlog_debug(SYSTEM_DBG_ERROR,"%s : NULL point\n", __FUNCTION__);
        return -1;
    }

    strcpy(str_buf, pstr);
    token = strtok(str_buf, ":");

    while (token != NULL)
    {
        ret = str_to_time_index(timeptr, str_index, token);

        if (ret < 0)
        {
            break;
        }

        str_index++;
        token = strtok(NULL, ":");
    }

    if (str_index < 6)
    {
        return -1;
    }

    return ret;
}

/********************************************** debug ***********************************************/

const struct message system_debug_name[] =
{
	{.key = SYSTEM_DBG_COMMON,		.str = "common"},
	{.key = SYSTEM_DBG_EVENT,		.str = "event"},
	{.key = SYSTEM_DBG_INFO,		    .str = "info"},
	{.key = SYSTEM_DBG_PACKAGE,		.str = "package"},
	{.key = SYSTEM_DBG_ERROR,		.str = "error"},
	{.key = SYSTEM_DBG_ALL,		    .str = "all"}
};
    
extern void zlog_debug_set(struct vty *vty, unsigned int type, int enable);

DEFUN (system_debug_monitor,
	system_debug_monitor_cmd,
	"debug system (enable|disable) (common|event|info|package|error|all)",
	"Debug information to moniter\n"
	"Programe name\n"
	"Enable statue\n"
	"Disatble statue\n"
	"Type name of common messege\n"
	"Type name of event messege\n"
	"Type name of info messege\n"
	"Type name of package messege \n"
	"Type name of error messege \n"
	"Type name of all debug\n")
{
	unsigned int zlog_num;

	for(zlog_num = 0; zlog_num < array_size(system_debug_name); zlog_num++)
	{
		if(!strncmp(argv[1], system_debug_name[zlog_num].str, 3))
		{
			zlog_debug_set( vty, system_debug_name[zlog_num].key, !strncmp(argv[0], "enable", 3));

			return CMD_SUCCESS;
		}
	}

	vty_out (vty, "No debug typd find %s", VTY_NEWLINE);

	return CMD_SUCCESS;
}

/* 显示个debug信息的状态  */
DEFUN (show_system_debug_monitor,
	show_system_debug_monitor_cmd,
	"show debug system",
	SHOW_STR
	"Syslog"
	"Debug status\n")
{
	unsigned int type_num;

	vty_out(vty, "debug type         status %s", VTY_NEWLINE);

	for(type_num = 0; type_num < array_size(system_debug_name); ++type_num)
	{
		vty_out(vty, "%-15s    %-10s %s", system_debug_name[type_num].str, 
			!!(vty->monitor & (1 << type_num)) ? "enable" : "disable", VTY_NEWLINE);
	}

	return CMD_SUCCESS;
}


/*******************************************************************************************************/

DEFUN(system_name,
      system_name_cmd,
      "system",
      "enter the system view\n")
{
    vty->node = SYSTEM_NODE;

    return CMD_SUCCESS;
}

DEFUN(system_show_cpu_func,
      system_show_cpu_cmd,
      "show system cpu",
      "show\n"
      "system\n"
      "cpu\n"
     )
{
    procinfo *p = NULL;
    unsigned int i;
    int cnt;
    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);

    for (i = 1; i < sys_conf.num_cpus; i++)
        vty_out(vty, "%s : %d.%d%%usr  %d.%d%%nice  %d.%d%%sys  %d.%d%%idle  %d.%d%%io  %d.%d%%irq  %d.%d%%sirq%s"
                , sys_conf.cpu_info[i].name
                , (sys_conf.cpu_info[i].usr_usage / 100), (sys_conf.cpu_info[i].usr_usage % 100), (sys_conf.cpu_info[i].nice_usage / 100), (sys_conf.cpu_info[i].nice_usage % 100) \
                , (sys_conf.cpu_info[i].sys_usage / 100), (sys_conf.cpu_info[i].sys_usage % 100), (sys_conf.cpu_info[i].idle_usage / 100), (sys_conf.cpu_info[i].idle_usage % 100) \
                , (sys_conf.cpu_info[i].io_usage / 100), (sys_conf.cpu_info[i].io_usage % 100) \
                , (sys_conf.cpu_info[i].irq_usage / 100), (sys_conf.cpu_info[i].irq_usage % 100), (sys_conf.cpu_info[i].sirq_usage / 100), (sys_conf.cpu_info[i].sirq_usage % 100) \
                , VTY_NEWLINE);

    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);
    /*  pid   ppid   name    stat  VSZ   VSZ%MEM  %CPU*/
    vty_out(vty, "%5s %5s %16s %8s %9s %9s %9s(%%)%s"
            , STR_PID, STR_PPID, STR_PROCESS_NAME, STR_STAT, STR_VSZ_SIZE, STR_VSZ_USAGE, STR_CPU_USAGE, VTY_NEWLINE);

    for (cnt = 0; cnt < proc_info_total.proc_cnt; cnt++)
    {
        p = &proc_info_total.proc_data[cnt];

        if ((p->mem_usage == 0) || (strcmp(p->exName, "sh") == 0))
        {
            continue;
        }

        /*  pid   ppid   name stat  RSS   RSS%MEM   %CPU*/
        vty_out(vty, "%5d %5d %16s %8c %9d %7d.%02d %7d.%02d%s"
                , p->pid, p->ppid, p->exName, p->state
                , (p->vsize / 1024), (int)(p->mem_usage / 100), (int)(p->mem_usage % 100)
                , (int)(p->cpu_usage / 100), (int)(p->cpu_usage % 100), VTY_NEWLINE);
        usleep(50);
    }

    return CMD_SUCCESS;

}


DEFUN(system_display_cpu_func,
      system_display_cpu_cmd,
      "show cpu-usage",
      "show\n"
      "cpu usage statistics\n"
     )
{
    procinfo *p = NULL;
    unsigned int i;
    int cnt;
    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);

    for (i = 1; i < sys_conf.num_cpus; i++)
        vty_out(vty, "%s : %d.%d%%usr  %d.%d%%nice  %d.%d%%sys  %d.%d%%idle  %d.%d%%io  %d.%d%%irq  %d.%d%%sirq%s"
                , sys_conf.cpu_info[i].name
                , (sys_conf.cpu_info[i].usr_usage / 100), (sys_conf.cpu_info[i].usr_usage % 100), (sys_conf.cpu_info[i].nice_usage / 100), (sys_conf.cpu_info[i].nice_usage % 100) \
                , (sys_conf.cpu_info[i].sys_usage / 100), (sys_conf.cpu_info[i].sys_usage % 100), (sys_conf.cpu_info[i].idle_usage / 100), (sys_conf.cpu_info[i].idle_usage % 100) \
                , (sys_conf.cpu_info[i].io_usage / 100), (sys_conf.cpu_info[i].io_usage % 100) \
                , (sys_conf.cpu_info[i].irq_usage / 100), (sys_conf.cpu_info[i].irq_usage % 100), (sys_conf.cpu_info[i].sirq_usage / 100), (sys_conf.cpu_info[i].sirq_usage % 100) \
                , VTY_NEWLINE);

    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);
    /*  pid   ppid   name    stat  VSZ   VSZ%MEM  %CPU*/
    vty_out(vty, "%5s %5s %16s %8s %9s %9s %9s(%%)%s"
            , STR_PID, STR_PPID, STR_PROCESS_NAME, STR_STAT, STR_VSZ_SIZE, STR_VSZ_USAGE, STR_CPU_USAGE, VTY_NEWLINE);

    for (cnt = 0; cnt < proc_info_total.proc_cnt; cnt++)
    {
        p = &proc_info_total.proc_data[cnt];

        if ((p->mem_usage == 0) || (strcmp(p->exName, "sh") == 0))
        {
            continue;
        }

        /*  pid   ppid   name stat  RSS   RSS%MEM   %CPU*/
        vty_out(vty, "%5d %5d %16s %8c %9d %7d.%02d %7d.%02d%s"
                , p->pid, p->ppid, p->exName, p->state
                , (p->vsize / 1024), (int)(p->mem_usage / 100), (int)(p->mem_usage % 100)
                , (int)(p->cpu_usage / 100), (int)(p->cpu_usage % 100), VTY_NEWLINE);
        usleep(50);
    }

    return CMD_SUCCESS;

}


DEFUN(system_display_health_func,
      system_display_health_cmd,
      "show health",
      "show\n"
      "cpu usage and memory statistics\n"
     )
{
    unsigned int i;
    struct sysinfo info;
    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);

    for (i = 1; i < sys_conf.num_cpus; i++)
        vty_out(vty, "%s : %d.%d%%usr  %d.%d%%nice  %d.%d%%sys  %d.%d%%idle  %d.%d%%io  %d.%d%%irq  %d.%d%%sirq%s"
                , sys_conf.cpu_info[i].name
                , (sys_conf.cpu_info[i].usr_usage / 100), (sys_conf.cpu_info[i].usr_usage % 100), (sys_conf.cpu_info[i].nice_usage / 100), (sys_conf.cpu_info[i].nice_usage % 100) \
                , (sys_conf.cpu_info[i].sys_usage / 100), (sys_conf.cpu_info[i].sys_usage % 100), (sys_conf.cpu_info[i].idle_usage / 100), (sys_conf.cpu_info[i].idle_usage % 100) \
                , (sys_conf.cpu_info[i].io_usage / 100), (sys_conf.cpu_info[i].io_usage % 100) \
                , (sys_conf.cpu_info[i].irq_usage / 100), (sys_conf.cpu_info[i].irq_usage % 100), (sys_conf.cpu_info[i].sirq_usage / 100), (sys_conf.cpu_info[i].sirq_usage % 100) \
                , VTY_NEWLINE);

    vty_out(vty, "***************************************************************************%s",
            VTY_NEWLINE);
    sysinfo(&info);
    vty_out(vty, "         %13s%13s%13s%13s%13s%s",
            "total",
            "used",
            "free",
            "shared",
            "buffers", VTY_NEWLINE
           );
    vty_out(vty, "Mem(KB): ");
    vty_out(vty, FIELDS_5,
            (unsigned long long)(info.totalram / 1024),
            (unsigned long long)((info.totalram - info.freeram) / 1024),
            (unsigned long long)(info.freeram / 1024),
            (unsigned long long)(info.sharedram / 1024),
            (unsigned long long)(info.bufferram / 1024)
           );
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "-/+ buffers(KB):      ");
    vty_out(vty, FIELDS_2,
            (unsigned long long)((info.totalram - info.freeram - info.bufferram) / 1024),
            (unsigned long long)((info.freeram + info.bufferram) / 1024)
           );
    vty_out(vty, "%s", VTY_NEWLINE);
    vty_out(vty, "Swap(KB):");
    vty_out(vty, FIELDS_3,
            (unsigned long long)(info.totalswap / 1024),
            (unsigned long long)((info.totalswap - info.freeswap) / 1024),
            (unsigned long long)(info.freeswap / 1024)
           );
    vty_out(vty, "%s", VTY_NEWLINE);
    return CMD_SUCCESS;

}

DEFUN(system_show_time_2_func,
      system_show_time_2_cmd,
      "show system datetime",
      "show\n"
      "sysman\n"
      "time\n"
     )
{
    char szBuf[256] = {0};
    time_t timer = time(NULL);

    strftime(szBuf, sizeof(szBuf), "%Y-%m-%d %H:%M:%S", localtime(&timer));
    vty_out(vty, "%s GMT%+d%s", szBuf, sys_conf.sys_timezone, VTY_NEWLINE);

    return CMD_SUCCESS;
}

DEFUN(system_show_time_func,
      system_show_time_cmd,
      "show system datetime (local|utc)",
      "show\n"
      "sysman\n"
      "time\n"
     )
{
    char szBuf[256] = {0};
    time_t timer = time(NULL);

    if (!strncmp(argv[0], "utc", 3))
    {
        timer -= sys_conf.sys_timezone * 3600;
        strftime(szBuf, sizeof(szBuf), "%Y-%m-%d %H:%M:%S", localtime(&timer));
        vty_out(vty, "%s UTC%s", szBuf, VTY_NEWLINE);
    }
    else if (!strncmp(argv[0], "local", 3))
    {
        strftime(szBuf, sizeof(szBuf), "%Y-%m-%d %H:%M:%S", localtime(&timer));
        vty_out(vty, "%s GMT%+d%s", szBuf, sys_conf.sys_timezone, VTY_NEWLINE);
    }
    else
    {
        vty_warning_out(vty, "%% Please set right parameter%s", VTY_NEWLINE);
    }

    return CMD_SUCCESS;
}

DEFUN(system_set_time_2_func,
      system_set_time_2_cmd,
      "system datetime YYYY:MM:DD:HH:MM:SS",
      "set system\n"
      "system time\n"
      "year:mon:day:hour:min:sec\n"
     )
{
    struct tm tm_time;
    time_t tm_sec;
    struct timeval timeval_tm;
    int ret;

    if (NULL == argv[0])
    {

        vty_warning_out(vty, "%% argv err usage : system time year:mon:day:hour:min:sec%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = sysm_str_to_time(&tm_time, argv[0]);

    if (ret < 0)
    {
        vty_warning_out(vty, "%% usage : system time year:mon:day:hour:min:sec%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (tm_time.tm_year < 100)
    {
        vty_warning_out(vty, "%% usage : years value must be seted after 2000%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    timeval_tm.tv_sec = mktime(&tm_time);
    timeval_tm.tv_usec = 0;
    tm_sec = timeval_tm.tv_sec - YEARS_1970_to_2000_SEC - (sys_conf.sys_timezone * 3600);
    devm_comm_set_rtc(MODULE_ID_SYSTEM, 1, tm_sec);
    settimeofday(&timeval_tm, NULL);
    return CMD_SUCCESS;
}

DEFUN(system_set_time_func,
      system_set_time_cmd,
      "system datetime YYYY:MM:DD:HH:MM:SS (local|utc)",
      "set system\n"
      "system time\n"
      "year:mon:day:hour:min:sec\n"
     )
{
    struct tm tm_time;
    time_t tm_sec;
    struct timeval timeval_tm;
    int ret;

    if (NULL == argv[0])
    {

        vty_warning_out(vty, "%% argv err usage : system time year:mon:day:hour:min:sec%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ret = sysm_str_to_time(&tm_time, argv[0]);

    if (ret < 0)
    {
        vty_warning_out(vty, "%% usage : system time year:mon:day:hour:min:sec%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (tm_time.tm_year < 100)
    {
        vty_warning_out(vty, "%% usage : years value must be seted after 2000%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strncmp(argv[1], "local", 3))
    {
        timeval_tm.tv_sec = mktime(&tm_time);
        timeval_tm.tv_usec = 0;
        tm_sec = timeval_tm.tv_sec - YEARS_1970_to_2000_SEC - (sys_conf.sys_timezone * 3600);
        devm_comm_set_rtc(MODULE_ID_SYSTEM, 1, tm_sec);
    }
    else if (!strncmp(argv[1], "utc", 3))
    {
        timeval_tm.tv_sec = mktime(&tm_time) + (sys_conf.sys_timezone * 3600);
        timeval_tm.tv_usec = 0;
        tm_sec = timeval_tm.tv_sec - YEARS_1970_to_2000_SEC - (sys_conf.sys_timezone * 3600);
        devm_comm_set_rtc(MODULE_ID_SYSTEM, 1, tm_sec);
    }
    else
    {
        vty_warning_out(vty, "%% Please set right parameter%s", VTY_NEWLINE);
    }

    settimeofday(&timeval_tm, NULL);
    return CMD_SUCCESS;
}

DEFUN(system_set_default_time_func,
      system_set_default_time_cmd,
      "no system datetime",
      NO_STR
      "set system\n"
      "default time\n"
     )
{
    struct tm tm_time;
    time_t tm_sec;
    struct timeval timeval_tm;
    int ret;

    ret = sysm_str_to_time(&tm_time, "2000:01:01:00:00:00");

    if (ret < 0)
    {
        vty_warning_out(vty, "%% usage : no system time%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    timeval_tm.tv_sec = mktime(&tm_time);
    timeval_tm.tv_usec = 0;
    tm_sec = timeval_tm.tv_sec - YEARS_1970_to_2000_SEC;
    devm_comm_set_rtc(MODULE_ID_SYSTEM, 1, tm_sec);
    settimeofday(&timeval_tm, NULL);
    return CMD_SUCCESS;
}

DEFUN(system_show_threshold_func,
      system_show_threshold_cmd,
      "show system threshold",
      "show\n"
      "system statue\n"
      "system cpu and memory alarm threshold\n"
     )
{

    vty_out(vty, "the device system cpu alarm threshold     :  %d%%%s", (sys_conf.system_cpu_alarm_threshold / 100), VTY_NEWLINE);
    vty_out(vty, "the device system memory alarm threshold  :  %d%%%s", (sys_conf.system_mem_alarm_threshold / 100), VTY_NEWLINE);
    return CMD_SUCCESS;
}


DEFUN(system_set_threshold_func,
      system_set_threshold_cmd,
      "system (cpu | memory) threshold VALUE",
      "Set system\n"
      "Cpu set\n"
      "Memory set\n"
      "threshold set\n"
      "Input value 0-99\n"
     )
{
    int value = 0;
    VTY_GET_INTEGER_RANGE("VALUE", value, argv[1], SYSTEM_CPU_USAGE_ALARM_THRESHOLD_MIN, SYSTEM_MEM_USAGE_ALARM_THRESHOLD_MAX);

    if (argv[0][0] == 'c')
    {
        sys_conf.system_cpu_alarm_threshold = value * 100;
    }
    else if (argv[0][0] == 'm')
    {
        sys_conf.system_mem_alarm_threshold = value * 100;
    }
    else
    {
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(system_set_default_threshold_func,
      system_set_default_threshold_cmd,
      "no system (cpu | memory) threshold",
      NO_STR
      "set system\n"
      "Cpu set\n"
      "Memory set\n"
      "default time\n"
     )
{

    if (argv[0][0] == 'c')
    {
        sys_conf.system_cpu_alarm_threshold = SYSTEM_CPU_USAGE_ALARM_THRESHOLD_DEFALUT;
    }
    else if (argv[0][0] == 'm')
    {
        sys_conf.system_mem_alarm_threshold = SYSTEM_MEM_USAGE_ALARM_THRESHOLD_DEFALUT;
    }
    else
    {
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}
DEFUN(system_set_timezone_func,
      system_set_timezone_cmd,
      "system timezone <-12-12>",
      "Set system\n"
      "System timezone set\n"
      "Input value -12 - 12\n"
     )
{
    int value = 0;
    //VTY_GET_INTEGER_RANGE("VALUE", value, argv[0], -12, 12);
    value = atoi(argv[0]);
    if (system_timezone_set(value) < 0)
    {
        vty_warning_out(vty, "%% set system timezone failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(system_set_default_timezone_func,
      system_set_default_timezone_cmd,
      "no system timezone",
      NO_STR
      "set system\n"
      "system timezone set default\n"

     )
{
    int value = SYSTEM_TIMEZONE_DAFAULT;

    if (system_timezone_set(value) < 0)
    {
        vty_warning_out(vty, "%% set system timezone failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

static int system_config_write(struct vty *vty)
{
    vty_out(vty, "system%s", VTY_NEWLINE);

    if (SYSTEM_CPU_USAGE_ALARM_THRESHOLD_DEFALUT != sys_conf.system_cpu_alarm_threshold)
    {
        vty_out(vty, " system cpu threshold %d%s", sys_conf.system_cpu_alarm_threshold / 100, VTY_NEWLINE);
    }

    if (SYSTEM_MEM_USAGE_ALARM_THRESHOLD_DEFALUT != sys_conf.system_mem_alarm_threshold)
    {
        vty_out(vty, " system  memory threshold %d%s", sys_conf.system_mem_alarm_threshold / 100, VTY_NEWLINE);
    }

    if (sys_conf.sys_timezone != SYSTEM_TIMEZONE_DAFAULT)
    {
        vty_out(vty, " system  timezone %d%s", sys_conf.sys_timezone, VTY_NEWLINE);
    }

    vty_out(vty, "!%s", VTY_NEWLINE);
    return 0;
}

void system_cmd_init(void)
{
    install_node(&stSystemNode, system_config_write);
    install_default(SYSTEM_NODE);
    install_element(CONFIG_NODE, &system_name_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &system_show_cpu_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &system_display_cpu_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &system_display_health_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &system_show_time_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &system_show_time_2_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &system_show_threshold_cmd, CMD_LOCAL);

    install_element(SYSTEM_NODE, &system_show_cpu_cmd, CMD_LOCAL);
    install_element(SYSTEM_NODE, &system_display_cpu_cmd, CMD_LOCAL);
    install_element(SYSTEM_NODE, &system_display_health_cmd, CMD_LOCAL);
    install_element(SYSTEM_NODE, &system_show_time_cmd, CMD_LOCAL);
    install_element(SYSTEM_NODE, &system_show_time_2_cmd, CMD_LOCAL);
    install_element(SYSTEM_NODE, &system_show_threshold_cmd, CMD_LOCAL);

    install_element(SYSTEM_NODE, &system_set_threshold_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_default_threshold_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_timezone_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_default_timezone_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_time_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_time_2_cmd, CMD_SYNC);
    install_element(SYSTEM_NODE, &system_set_default_time_cmd, CMD_SYNC);
	/* new debug */
	install_element (CONFIG_NODE, &system_debug_monitor_cmd, CMD_SYNC);
	install_element (CONFIG_NODE, &show_system_debug_monitor_cmd, CMD_SYNC);

    return;
}






