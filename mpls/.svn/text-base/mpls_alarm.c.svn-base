/******************************************************************************
 * Filename: mpls_alarm.c
 * Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.12.20  zfj created
 *
******************************************************************************/

#include <string.h>
#include <lib/command.h>
#include <lib/thread.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/alarm.h>
#include "mpls_alarm.h"

extern struct thread_master *mpls_master;


int mpls_alarm_socket_msg_rcv(struct thread *t)
{
    gpnSockCommApiNoBlockProc();
    usleep(100000); //让出 CPU 100ms
    thread_add_event(mpls_master, mpls_alarm_socket_msg_rcv, NULL, 0);
    return 0;
}


static uint32_t gpnMplsCoreProc (gpnSockMsg *pgnNsmMsgSp, uint32_t len)
{
    return 1;
}


uint32_t mpls_gpn_function_init (void)
{
    uint32_t reVal;

    reVal = gpnSockCommApiSockCommStartup(GPN_SOCK_ROLE_MPLS, (usrMsgProc)gpnMplsCoreProc);
    if (reVal == 2)
    {
        return 2;
    }

    return 1;
}



