/******************************************************************************
 * Filename: gpnAlmCmdShow.h
 *	Copyright (c) 2017-2017 Huahuan Electronics Co., LTD
 * All rights reserved
 *
 *
 * Functional description:
 *
 * History:
 * 2017.8.18  lipf created
 *
******************************************************************************/

#ifndef _GPN_ALM_CMD_SHOW_H_
#define _GPN_ALM_CMD_SHOW_H_

#include <lib/vty.h>


char *gpnAlmLevelStrGet (uint8_t almRank);
void gpnAlmCodeShow (struct vty *vty);
char *gpnAlmTypeNameStrGet (uint32_t subType);

char *gpnEvtLevelStrGet (uint8_t evtRank);
char *gpnEvtTypeNameStrGet (uint32_t subType);

void alarm_show_alarm_code (struct vty *vty);
void alarm_show_curr_alarm (struct vty *vty);
void alarm_show_hist_alarm (struct vty *vty);
void alarm_show_event (struct vty *vty);
void alarm_show_alarm_mainType (struct vty *vty);
void alarm_show_alarm_subType (struct vty *vty);
void alarm_show_register_info(struct vty *vty);


#endif /* _GPN_ALM_CMD_SHOW_H_ */

