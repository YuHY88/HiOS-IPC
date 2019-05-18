/******************************************************************************
 * Filename: rmond.h
 *  Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.10.23  lidingcheng created
 *
******************************************************************************/

#ifndef _RMOND_H_
#define _RMOND_H_

#include <lib/hash1.h>
#include <lib/errcode.h>

#include <lib/command.h>
#include <lib/inet_ip.h>

#include "syslog.h"


#define RMOND_ERROR           -1
#define RMOND_OK              ERRNO_SUCCESS
#define RMOND_FAIL            ERRNO_FAIL
#define RMOND_NOT_FOUND       ERRNO_NOT_FOUND
#define RMOND_MALLOC_FAIL     ERRNO_MALLOC
#define RMOND_OVERSIZE        ERRNO_OVERSIZE
#define RMOND_IPC_FAIL        ERRNO_IPC
#define RMOND_PARAM_ERR       ERRNO_PARAM_ILLEGAL
#define RMOND_PORT_ALLOC_FAIL ERRNO_PORT_ALLOC
#define RMOND_PKT_SEND_FAIL   ERRNO_PKT_SEND
#define RMOND_PKT_RECV_FAIL   ERRNO_PKT_RECV

#endif /* _RMOND_H_ */

