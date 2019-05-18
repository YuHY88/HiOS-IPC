/**
 * @file      : route_main.h
 * @brief     :
 * @details   :
 * @author    : ZhangFj
 * @date      : 2018年2月26日 10:35:49
 * @version   :
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      :
 */

#ifndef HIOS_ROUTE_MAIN_H
#define HIOS_ROUTE_MAIN_H

#include <lib/linklist.h>
#include <lib/inet_ip.h>
#include <pthread.h>
#include <lib/hptimer.h>

#define ROUTE_LOG_DEBUG(format, ...) \
    zlog_debug(1, "%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define ROUTE_LOG_ERROR(format, ...) \
    zlog_err("%s[%d]: In function '%s' "format, __FILE__, __LINE__, __func__, ##__VA_ARGS__);

#define ROUTE_CLI_INFO  "Prompt information display in vtysh_route_cmd.c \n"


/* route 全局数据结构 */
struct route_global
{
	uint32_t         router_id;             // IPv4 router-id
	struct ipv6_addr routerv6_id;           // IPv6 router-id
	uint32_t         route_limit;           // 芯片注册的最大 ipv4 路由数量
	uint32_t         routev6_limit;         // 芯片注册的最大 ipv6 路由数量
    TIMERID route_timer;           // IPv4 静态路由迭代定时器
    TIMERID routev6_timer;         // IPv6 静态路由迭代定时器

    struct list      route_ipc_msg_list;
    struct list      route_msg_list;        // route 接收控制消息链表
    struct list      route_route_list;      // route 接收路由消息链表
    struct list      route_send_route_list; // route 发送路由(向 ftm)消息链表
    pthread_mutex_t  msg_lock;              // route 接收控制消息锁
    pthread_mutex_t  route_lock;            // route 接收路由消息锁
    pthread_mutex_t  route_send_lock;       // route 发送路由(向 ftm)消息锁

    /* 控制路由收敛标志，disable: 不重新迭代，enable: 重新迭代
     * 物理接口 up/down 时，子接口相应处理时关闭迭代标志，由物理口触发一次迭代 */
    int              conver_flag;           // IPv4 静态路由迭代标志
    uint8_t          route_ecmp_flag;       // IPv4 ecmp 使能标志
    uint8_t          route_frr_flag;        // IPv4 frr 使能标志
    uint8_t          routev6_ecmp_flag;     // IPv6 ecmp 使能标志
    uint8_t          routev6_frr_flag;      // IPv6 frr 使能标志
};

#define ROUTE_MSG_LOCK      pthread_mutex_lock(&g_route.msg_lock);
#define ROUTE_MSG_UNLOCK    pthread_mutex_unlock(&g_route.msg_lock);
#define ROUTE_ROUTE_LOCK    pthread_mutex_lock(&g_route.route_lock);
#define ROUTE_ROUTE_UNLOCK  pthread_mutex_unlock(&g_route.route_lock);
#define ROUTE_SEND_LOCK     pthread_mutex_lock(&g_route.route_send_lock);
#define ROUTE_SEND_UNLOCK   pthread_mutex_unlock(&g_route.route_send_lock);


/* arp 全局数据 */
extern struct arp_global g_arp;

/* ndp 全局数据*/
extern struct ndp_global gndp;

extern struct thread_master *route_master;

/* route 模块的全局数据 */
extern struct route_global g_route;

extern void route_init(void);
extern void route_die(void);
extern void arp_init(void);
extern void ndp_init(void);

#endif


