/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_cli.h
*
*  date: 2016.12.23
*
*  modify:
*
*/

#ifndef _MSTP_CLI_H_
#define _MSTP_CLI_H_

int mstp_config_write(struct vty *vty);
void mstp_vty_init (void);

#endif

