/*
*     define of device management
*
*/

#ifndef HIOS_DEVM_TIMER_H
#define HIOS_DEVM_TIMER_H

#include <signal.h>
#include <timer.h>
#include <lib/types.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <lib/msg_ipc.h>



/*devm timer realtime*/
#define CLOCKID CLOCK_REALTIME
#define DEVM_CMD_TIMER_INTERVAL_SEC 1
#define DEVM_CMD_TIMER_INTERVAL_MSEC 1

#define TIMER_LOOP 1
#define TIMER_NO_LOOP 0


typedef struct _TIMER_S
{
//    MSGQID ulMsgQId;
    int ulMsgQId;
    void (*pFunc)(void *);
    void *pArg;
    int iType;              /* TIMER_LOOP或TIMER_NO_LOOP */
    int iInterval;         /* 定时器时间长度 毫秒               */
    int iTick;              /* 定时器计时tick数                  */
    int iTickGap;           /* 和前一个定时器的时间差 tick数     */
    int ulModuleId;
} TIMER_S;


/*初始化定时器链表及初始化定时器*/
void devm_init_cmd_timer(void);


/*devm timer func*/
pthread_t devm_create_pthread(void (*pthread_func)(void *arg), void *arg);


/*初始化定时器， 指定时间间隔及执行函数*/
void devm_pthread_timer_init_nsec(long minimum_timer_interval_nsec, void(*pthread_func)(union sigval v));
void devm_pthread_timer_init_sec(time_t minimum_timer_interval_sec, void(*pthread_func)(union sigval v));
void devm_pthread_timer_init_msec(time_t minimum_timer_interval_msec, void(*pthread_func)(union sigval v));


/*定时器执行函数*/
void devm_cmd_pthread_timer_func(union sigval v);

/*在list中查找需要执行的节点*/
int devm_list_check(struct list *p_list);

/*执行节点命令*/
int devm_timer_execute_pnode(struct listnode *pnode);


/*创建pdata指向的空间，并赋值*/
TIMER_S *devm_create_cmd_timer_struct(TIMER_S *t_timer_s);
/*寻找节点插入位置*/
struct listnode *devm_get_spec_timer_node_by_iTick(struct list *plist, int itick);
/*在指定节点后添加新节点*/
void devm_timer_list_add_node_next(struct list *list, struct listnode *current, struct listnode *node);
/*将节点插入链表*/
void devm_timer_list_add_node(struct list *list, struct listnode *pnode);



/*清空节点*/
void devm_timer_free_pnode(struct listnode *pnode);
/*删除节点*/
void devm_timer_list_delete_node(struct list *list, struct listnode *pnode);


#endif




