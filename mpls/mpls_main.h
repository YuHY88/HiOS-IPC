/**
 * @file      : mpls_main.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月28日 9:13:39
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef MPLS_MAIN_H
#define MPLS_MAIN_H

#include <lib/thread.h>


#define MPLS_LOG_DEBUG(format, ...)\
    zlog_debug(MPLS_DBG_L2VC, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define MPLS_LOG_ERROR(format, ...)\
    zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define MPLS_CLI_INFO  "Prompt information display in vtysh_mpls_cmd.c \n"


extern struct thread_master *mpls_master;
extern int mpls_errno;


extern void mpls_init(void);
extern void mpls_die(void);
extern void mpls_ipc_init ( void );

#endif


