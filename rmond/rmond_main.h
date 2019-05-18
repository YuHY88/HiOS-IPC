/******************************************************************************
 * Filename: rmond_main.h
 *  Copyright (c) 2016-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: main functions for rmond.
 *
 * History:
 *  *2017.10.23  lidingcheng created
 *
******************************************************************************/


#ifndef __RMON_MAIN_H__
#define __RMON_MAIN_H__

struct thread_master *get_rmond_thread_master();

int rmon_msg_rcv(struct ipc_mesg_n *pmsg, int imlen) ;

#endif
