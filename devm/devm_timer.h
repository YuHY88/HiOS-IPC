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
    int iType;              /* TIMER_LOOP��TIMER_NO_LOOP */
    int iInterval;         /* ��ʱ��ʱ�䳤�� ����               */
    int iTick;              /* ��ʱ����ʱtick��                  */
    int iTickGap;           /* ��ǰһ����ʱ����ʱ��� tick��     */
    int ulModuleId;
} TIMER_S;


/*��ʼ����ʱ��������ʼ����ʱ��*/
void devm_init_cmd_timer(void);


/*devm timer func*/
pthread_t devm_create_pthread(void (*pthread_func)(void *arg), void *arg);


/*��ʼ����ʱ���� ָ��ʱ������ִ�к���*/
void devm_pthread_timer_init_nsec(long minimum_timer_interval_nsec, void(*pthread_func)(union sigval v));
void devm_pthread_timer_init_sec(time_t minimum_timer_interval_sec, void(*pthread_func)(union sigval v));
void devm_pthread_timer_init_msec(time_t minimum_timer_interval_msec, void(*pthread_func)(union sigval v));


/*��ʱ��ִ�к���*/
void devm_cmd_pthread_timer_func(union sigval v);

/*��list�в�����Ҫִ�еĽڵ�*/
int devm_list_check(struct list *p_list);

/*ִ�нڵ�����*/
int devm_timer_execute_pnode(struct listnode *pnode);


/*����pdataָ��Ŀռ䣬����ֵ*/
TIMER_S *devm_create_cmd_timer_struct(TIMER_S *t_timer_s);
/*Ѱ�ҽڵ����λ��*/
struct listnode *devm_get_spec_timer_node_by_iTick(struct list *plist, int itick);
/*��ָ���ڵ������½ڵ�*/
void devm_timer_list_add_node_next(struct list *list, struct listnode *current, struct listnode *node);
/*���ڵ��������*/
void devm_timer_list_add_node(struct list *list, struct listnode *pnode);



/*��սڵ�*/
void devm_timer_free_pnode(struct listnode *pnode);
/*ɾ���ڵ�*/
void devm_timer_list_delete_node(struct list *list, struct listnode *pnode);


#endif




