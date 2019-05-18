/******************************************************************************
 * Filename: synce_cmd.h
 * Copyright (c) 2016 - 2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description: functions for syncE command header file.
 *
 * History:
 * 2016.10.13  cuiyudong created
 *
******************************************************************************/

#ifndef SYNCE_CMD_H_
#define SYNCE_CMD_H_

#include"clock_if.h"

#define IF_NAME_LEN 32

#define CLOCK_STATE_DISPLAY(x) \
    ((x == CLOCK_STAT_LOCK) ? "locked" : "unlocked")

#define CLOCK_IFM_STATE_DISPLAY(x) \
    ((x == 1) ? "up" : "down")

#define CLOCK_VALID_STATE_DISPLAY(x) \
        ((x == VALID_STAT_VALID) ? "valid" : "invalid")

#define SSM_ENALBE_DISPLAY(x) \
        ((x == SYNCE_SSM_ENABLE) ? "Enabled" : ((x == SYNCE_SSM_DISABLE) ? "Disable" : "invalid"))

#define CLOCK_ENALBE_DISPLAY(x) ((x) ? "Enable" : "Disable")
#define CLOCK_MODE_AUTO     "auto"
#define CLOCK_MODE_MANUAL   "manual"
#define CLOCK_MODE_FORCE    "force"
#define SYNCE_RUN_STATE_AUTO_STR        "AUTO"
#define SYNCE_RUN_STATE_FREERUN_STR     "FREE-RUN"
#define SYNCE_RUN_STATE_HOLDOVER_STR    "HOLDOVER"
#define SYNCE_RUN_STATE_LOCKED_STR      "LOCKED"
#define SYNCE_RUN_STATE_LOSELOCK_STR    "LOST-LOCK"


void synce_cmd_init(void);
void l2if_synce_config_write(struct vty *vty, struct synce_if *psynce);

int synce_node_config_write(struct vty *vty);

#endif

