/******************************************************************************
 * Filename: clock_cmd.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: command functions for clock interface header file.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/


#ifndef HIOS_CLOCK_CMD_H
#define HIOS_CLOCK_CMD_H

#define CLOCK_DEF_MODE      CLOCKIF_MODE_2MHZ
#define CLOCK_DEF_SIGNAL    CLOCKIF_SIGNAL_INOUT
#define CLOCK_DEF_CLK_QL    0


/* 按位定义debug信息的类型 */
#define CLOCK_DBG_COMMON		(1 << 0)	//common  debug
#define CLOCK_DBG_EVENT		    (1 << 1)		//event debug
#define CLOCK_DBG_INFO		    (1 << 2)		//info debug
#define CLOCK_DBG_PACKAGE	    (1 << 3)		//package  debug
#define CLOCK_DBG_ERROR		    (1 << 4)		//error   debug
#define CLOCK_DBG_ALL	        (CLOCK_DBG_COMMON |CLOCK_DBG_EVENT |CLOCK_DBG_INFO |CLOCK_DBG_PACKAGE |CLOCK_DBG_ERROR)	//所有debug




#define CLOCK_MODE_DISPLAY(x) \
    ((x == CLOCKIF_MODE_2MBIT) ? "2Mbits" : ((x == CLOCKIF_MODE_2MHZ) ? "2MHZ" : "invalid"))

#define CLOCK_SIGNAL_DISPLAY(x) \
        ((x == CLOCKIF_SIGNAL_IN) ? "IN" : ((x == CLOCKIF_SIGNAL_OUT) ? "OUT" : ((x == CLOCKIF_SIGNAL_INOUT) ? "IN-OUT" : "invalid")))

void clock_cmd_init(void);


#endif
