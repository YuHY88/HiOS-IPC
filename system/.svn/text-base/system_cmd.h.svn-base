/******************************************************************************
 * Filename: system_cmd.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for system module command head file .
 *
 * History:
 * 2016.9.13  cuiyudong created
 *
******************************************************************************/


#ifndef _SYSTEM_CMD_H_
#define _SYSTEM_CMD_H_

#define STR_MEM_TOTAL   "mem_total(kb)"
#define STR_MEM_FREE    "mem_free(kb)"
#define STR_PID             "pid"
#define STR_PPID            "ppid"
#define STR_STAT            "stat"

#define STR_PROCESS_NAME    "process"
#define STR_VM_SIZE         "vm_size"
#define STR_VSZ_SIZE        "vsz"
#define STR_VSZ_USAGE       "vsz%mem"

#define STR_RSS_SIZE        "rss_size(kb)"
#define STR_MEM_USAGE       "mem_usage"
#define STR_CPU_USAGE       "cpu"
#define STR_TOTAL           "total"

#define FIELDS_5 "%13llu%13llu%13llu%13llu%13llu"
#define FIELDS_3 (FIELDS_5 + 2*6)
#define FIELDS_2 (FIELDS_5 + 3*6)


/* 按位定义debug信息的类型 */
#define SYSTEM_DBG_COMMON		(1 << 0)	//common  debug
#define SYSTEM_DBG_EVENT		(1 << 1)		//event debug
#define SYSTEM_DBG_INFO		    (1 << 2)		//info debug
#define SYSTEM_DBG_PACKAGE	    (1 << 3)		//package  debug
#define SYSTEM_DBG_ERROR		(1 << 4)		//error   debug
#define SYSTEM_DBG_ALL	  (SYSTEM_DBG_COMMON |SYSTEM_DBG_EVENT |SYSTEM_DBG_INFO |SYSTEM_DBG_PACKAGE |SYSTEM_DBG_ERROR)	//所有debug



#define SIZE_TIME_BUF 8
#define NUM_TIME_SIZE 6
#define YEARS_1970_to_2000_SEC 0x386d4380
enum TIME_INDEX
{
    YEAR_INDEX = 0,
    MON_INDEX,
    DAY_INDEX,
    HOUR_INDEX,
    MIN_INDEX,
    SEC_INDEX = 5
};
void system_cmd_init(void);

#endif


