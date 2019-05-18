#ifndef HIOS_QOS_MAIN_H
#define HIOS_QOS_MAIN_H

#include "lib/log.h"

/* qos debug */
#define QOS_DBG_MSG    0x01
#define QOS_DBG_CFG    0X02
#define QOS_DBG_TRACE  0X04
#define QOS_DBG_SLA    0X08

/* QoS主线程 */
extern struct thread_master *qos_master;

/* qos log_err */
#define QOS_LOG_ERR(fmt, ...) \
    do { \
        	zlog_err ( "%s[%d] : "fmt, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while(0)

/* qos log_debug */
#define QOS_LOG_MSG(fmt, ...) \
    do { \
            zlog_debug ( QOS_DBG_MSG, "%s[%d] : "fmt, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while(0)

/* qos log_debug */
#define QOS_LOG_CFG(fmt, ...) \
    do { \
            zlog_debug ( QOS_DBG_CFG, "%s[%d] : "fmt, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while(0)

/* qos log_debug */
#define QOS_LOG_DBG(fmt, ...) \
    do { \
            zlog_debug ( QOS_DBG_TRACE, "%s[%d] : "fmt, __FILE__, __LINE__, ##__VA_ARGS__ ); \
    } while(0)

/* qos log_debug */
#define QOS_LOG_SLA(fmt, ...) \
	do { \
			zlog_debug ( QOS_DBG_SLA, "%s[%d] : "fmt, __FILE__, __LINE__, ##__VA_ARGS__ ); \
	} while(0)


#define QOS_ERR_BASE 						100
#define QOS_ERR_ATTACHED  					(QOS_ERR_BASE + 1)
#define QOS_ERR_CONFIGURED					(QOS_ERR_BASE + 2)
#define QOS_ERR_CAR_PROFILE_NOT_FOUND		(QOS_ERR_BASE + 3)
#define QOS_ERR_CAR_PROFILE_NOT_SHARED		(QOS_ERR_BASE + 4)
#define QOS_ERR_CAR_PROFILE_NOT_CONFIG		(QOS_ERR_BASE + 5)
#define QOS_ERR_CAR_NOT_APPLIED				(QOS_ERR_BASE + 6)
#define QOS_ERR_IF_NOT_FOUND				(QOS_ERR_BASE + 7)
#define QOS_ERR_ILL_PIR						(QOS_ERR_BASE + 8)
#define QOS_ERR_ILL_PBS						(QOS_ERR_BASE + 9)
#define QOS_ERR_GPACT						(QOS_ERR_BASE + 10)
#define QOS_ERR_YGPACT						(QOS_ERR_BASE + 11)
#define QOS_ERR_YRPACT						(QOS_ERR_BASE + 12)
#define QOS_ERR_RPACT						(QOS_ERR_BASE + 13)
#define QOS_ERR_ILL_DROPHIGH				(QOS_ERR_BASE + 14)
#define QOS_ERR_BINDED_TRUNK				(QOS_ERR_BASE + 15)
#define QOS_ERR_CONFIG_IF					(QOS_ERR_BASE + 16)
#define QOS_ERR_CONFLICT					(QOS_ERR_BASE + 17)
#define QOS_ERR_CAR_HQOS					(QOS_ERR_BASE + 18)
#define QOS_ERR_CONFIG_ETH_SUBIF			(QOS_ERR_BASE + 19)
#define QOS_ERR_NOT_SUPPORT	                (QOS_ERR_BASE + 20)



#endif


