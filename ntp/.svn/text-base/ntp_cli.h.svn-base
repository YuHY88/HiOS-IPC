#ifndef __ZEBOS_NTP_CLI_H__
#define __ZEBOS_NTP_CLI_H__

#include "vty.h"

#define CLINETMODE 3
#define NTPDISABLE 0
#define NTPENABLE  1
#define NTP_SYS_CONF_DEF_NTP 0

#define NTP_CONFIG_DEFAULT_MODE     CLINETMODE
#define NTP_CONFIG_DEFAULT_ENALBE   NTPDISABLE
#define NTP_CONFIG_DEFAULT_INTERVAL 10
#define NTP_CONFIG_DEFAULT_VERSION  4
#define NTP_CONFIG_DEFAULT_PACK_TYPE    "unicast"

/* 按位定义debug信息的类型 */
#define NTP_DBG_COMMON		(1 << 0)	//common  debug
#define NTP_DBG_EVENT		    (1 << 1)		//event debug
#define NTP_DBG_INFO		    (1 << 2)		//info debug
#define NTP_DBG_PACKAGE	    (1 << 3)		//package  debug
#define NTP_DBG_ERROR		    (1 << 4)		//error   debug
#define NTP_DBG_ALL	        (NTP_DBG_COMMON |NTP_DBG_EVENT |NTP_DBG_INFO |NTP_DBG_PACKAGE |NTP_DBG_ERROR)	//所有debug


/* Install NTP related CLI.  */
void ntp_cmd_init(void);
int ntp_config_write(struct vty *vty);
extern int ntp_check_ip_valid(uint32_t uiIPAddr);

#endif

