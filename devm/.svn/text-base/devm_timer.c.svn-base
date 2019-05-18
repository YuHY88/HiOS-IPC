#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <lib/command.h>
#include <lib/log.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/ifm_common.h>
#include <lib/linklist.h>
#include <lib/msg_ipc.h>
#include <lib/zassert.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>
#include <lib/devm_com.h>
#include <lib/alarm.h>
#include <lib/linklist.h>
//#include <lib/gpnSocket/socketComm/gpnAlmTypeDef.h>
//#include <lib/gpnSocket/socketComm/gpnSockCommRoleMan.h>
//#include <alarm/alarm_def.h>
#include <syslog/syslogd.h>
#include "devm.h"
#include "devm_timer.h"

/*device info struct*/
extern struct devm device;

/*execute the cmd in the list*/
struct list *execute_cmd_list = NULL;

/*mainboard status*/
extern enum HA_BOOTSTATE myboard_dev_start_status;
extern enum HA_BOOTSTATE otherboard_dev_start_status;
extern enum HA_ROLE ha_dev_ha_role;



void devm_init_cmd_timer(void)
{
    execute_cmd_list = list_new();
}


pthread_t devm_create_pthread(void (*pthread_func)(void *arg), void *arg)
{
    int ret = 0;
    pthread_attr_t t_pthread_attr;
    pthread_t pthread_id = 0;

    ret = pthread_attr_init(&t_pthread_attr);

    if (ret != 0)
    {
        zlog_err("%s[%d]:leave %s:error:fail to init pthread_attr_init,failed to create pthread ,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
        return 0;
    }
    else
    {
        ret = pthread_attr_setdetachstate(&t_pthread_attr, PTHREAD_CREATE_DETACHED);

        if (0 == ret)
        {
            ret = pthread_create(&pthread_id, &t_pthread_attr, (void *)pthread_func, arg);

            if (0 != ret)
            {
                zlog_err("%s[%d]:leave %s:error:fail to create pthread ,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
            }
            else
            {
                zlog_debug(DEVM_DBG_TIMER, "%s[%d]:leave %s:sucessfully create pthread, pthreadid = %ld\n", __FILE__, __LINE__, __func__, pthread_id);
            }
        }

        pthread_attr_destroy(&t_pthread_attr);

        if (0 != ret)
        {
            zlog_err("%s[%d]:leave %s:error:fail to destory t_pthread_attr,ret = %d,error = %s\n", __FILE__, __LINE__, __func__, ret, strerror(errno));
        }
    }

    return pthread_id;
}


void devm_pthread_timer_init_nsec(long minimum_timer_interval_nsec, void(*pthread_func)(union sigval v))
{
//  XXX int timer_create(clockid_t clockid, struct sigevent *evp, timer_t
//          *timerid);
//  clockid--值：CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
//      evp--存放环境值的地址,结构成员说明了定时器到期的通知方式和处理方式等
    // timerid--定时器标识符
    timer_t timerid;
    struct sigevent evp;
    memset(&evp, 0, sizeof(struct sigevent));
    //清零初始化

    evp.sigev_value.sival_int = 111;
    //也是标识定时器的，这和timerid有什么区别？回调函数可以获得
    evp.sigev_notify = SIGEV_THREAD;
    //线程通知的方式，派驻新线程
    evp.sigev_notify_function = pthread_func;   //线程函数地址

    if (timer_create(CLOCKID, &evp, &timerid) == -1)
    {
        perror("fail to timer_create");
        exit(-1);
    }

    //XXX   int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value,struct itimerspec *old_value);
    //timerid--定时器标识
    //flags--0表示相对时间，1表示绝对时间，通常使用相对时间
    //new_value--定时器的新初始值和间隔，如下面的it
    //old_value--取值通常为0，即第四个参数常为NULL,若不为NULL，则返回定时器的前一个值

    //第一次间隔it.it_value这么长,以后每次都是it.it_interval这么长,就是说it.it_value变0的时候会装载it.it_interval的值
    //it.it_interval可以理解为周期
    struct  itimerspec it;
    it.it_interval.tv_sec = minimum_timer_interval_nsec / 1000000; //间隔N second
    it.it_interval.tv_nsec = minimum_timer_interval_nsec % 1000000;
    it.it_value.tv_sec  = 1;
    it.it_value.tv_nsec = 0;

    if (timer_settime(timerid, 0, &it, NULL) == -1)
    {
        perror("fail to timer_settime");
        exit(-1);
    }

    return;
}


void devm_pthread_timer_init_sec(time_t minimum_timer_interval_sec, void(*pthread_func)(union sigval v))
{
    if (NULL != pthread_func)
    {
        devm_pthread_timer_init_nsec(minimum_timer_interval_sec * 1000000, pthread_func);
    }

    return;
}


void devm_pthread_timer_init_msec(time_t minimum_timer_interval_msec, void(*pthread_func)(union sigval v))
{
    if (NULL != pthread_func)
    {
        devm_pthread_timer_init_nsec(minimum_timer_interval_msec * 1000, pthread_func);
    }

    return;
}


void devm_cmd_pthread_timer_func(union sigval v)
{
    DEV_MYPRINTF("\n");

    if (NULL == execute_cmd_list)
    {
        DEV_MYPRINTF("check list\n");
        devm_list_check(execute_cmd_list);
        DEV_MYPRINTF("after list\n");
    }
    else
    {

    }

    return;
}


/*根据节点执行命令*/
int devm_timer_execute_pnode(struct listnode *pnode)
{
    /*execute it*/
    return 0;
}


int devm_list_check(struct list *p_list)
{
    struct listnode *pnode = NULL;
    struct listnode *pnode_next = NULL;
    struct listnode *p_execute_node = NULL;
    TIMER_S  *p_time = NULL;

    if (NULL == p_list)
    {
        return 1;
    }

    pnode = listhead(p_list);

    if (NULL == pnode)
    {
        DEV_MYPRINTF("list NULL\n");/*定时器链表中，没有到时间的节点*/
    }
    else
    {
        p_time = (TIMER_S *)listgetdata(pnode);
        p_time->iTickGap--;

        while (0 == p_time->iTickGap) /*循环遍历第一个节点是否到时间，需要执行*/
        {
            p_time = (TIMER_S *)listgetdata(pnode);
            pnode_next = pnode->next;

            if (NULL == p_time)
            {
                devm_timer_list_delete_node(p_list, pnode);
                devm_timer_free_pnode(pnode);
                continue;/*定时器节点异常，节点无时间信息*/
            }
            else
            {
                devm_timer_list_delete_node(p_list, pnode); /*删除节点，但不释放节点*/

                devm_timer_execute_pnode(p_execute_node);/*执行节点命令*/

                if (TIMER_LOOP == p_time->iType) /*如果node类型是LOOP,为循环执行节点，重新插入到list的合适位置*/
                {
                    devm_timer_list_add_node(p_list, pnode);
                }
                else
                {
                    devm_timer_free_pnode(pnode);
                }
            }

            pnode = pnode_next;

            if (NULL == pnode)
            {
                break;
            }
        }
    }

    return 0;
}


TIMER_S *devm_create_cmd_timer_struct(TIMER_S *t_timer_s)
{
    TIMER_S *p_timer_data = NULL;

    if (NULL != t_timer_s)
    {
        p_timer_data = (TIMER_S *)XMALLOC(MTYPE_DEVM_ENTRY, sizeof(TIMER_S));

        if (NULL == p_timer_data)
        {
            memcpy(p_timer_data, t_timer_s, sizeof(TIMER_S));
        }
        else
        {

        }
    }

    return p_timer_data;
}


struct listnode *devm_get_spec_timer_node_by_iTick(struct list *plist, int itick)
{
    struct listnode *p_aim_node = NULL;
    struct listnode *p_node = NULL;
    TIMER_S *p_data = NULL;

    if (NULL != plist)
    {
        /*遍历链表，寻找itick值刚好大于参数itick的节点*/
        for (ALL_LIST_ELEMENTS_RO(plist, p_node, p_data))
        {
            if (p_data->iTick <= itick)
            {
                continue;
            }
            else
            {
                p_aim_node = p_node;
            }
        }
    }

    return p_aim_node;
}


void devm_timer_list_add_node_next(struct list *list, struct listnode *current, struct listnode *node)
{
    if (current == NULL)
    {
        if (list->head)
        {
            list->head->prev = node;
        }
        else
        {
            list->tail = node;
        }

        node->next = list->head;
        node->prev = current;

        list->head = node;
    }
    else
    {
        if (current->next)
        {
            current->next->prev = node;
        }
        else
        {
            list->tail = node;
        }

        node->next = current->next;
        node->prev = current;

        current->next = node;
    }

    list->count++;

    return;
}



void devm_timer_list_add_node(struct list *list, struct listnode *pnode)
{
    if (NULL != list && NULL != pnode)
    {
        struct listnode *p_aim_node = NULL;

        TIMER_S *pdata = (TIMER_S *)pnode->data;

        p_aim_node = devm_get_spec_timer_node_by_iTick(list, pdata->iTick);

        pnode->prev = p_aim_node;
        devm_timer_list_add_node_next(list, p_aim_node, pnode);
    }

    return;
}


void devm_timer_list_delete_node(struct list *list, struct listnode *pnode)
{
    if (NULL != list && NULL != pnode)
    {
        if (pnode->prev)
        {
            pnode->prev->next = pnode->next;
        }
        else
        {
            list->head = pnode->next;
        }

        if (pnode->next)
        {
            pnode->next->prev = pnode->prev;
        }
        else
        {
            list->tail = pnode->prev;
        }

        list->count--;
    }

    return;
}


void devm_timer_free_pnode(struct listnode *pnode)
{
    if (NULL != pnode)
    {
        XFREE(MTYPE_DEVM_ENTRY, pnode->data);
        pnode->data = NULL;

        XFREE(MTYPE_DEVM_ENTRY, pnode);
        pnode = NULL;
    }

    return;
}

