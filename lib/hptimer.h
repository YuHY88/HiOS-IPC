/*====================================================================================================================*/
/*------------------------------------------------------------------------*/
/* 文 件 名 : hptimer.h                                                      */
/*  作    者 : suxiqing                                                   */
/* 创作日期 : 2006-08-05                                                  */
/* 产    权 :                                                             */
/* 说    明 : 此文件对外提供VOS层的定时器管理接口                         */
/*------------------------------------------------------------------------*/
/*====================================================================================================================*/
#ifndef __VOS_TIMER_H__
#define __VOS_TIMER_H__

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#define LIB_TIMER_TYPE_NOLOOP   0 
#define LIB_TIMER_TYPE_LOOP     1  

#define LIB_TIMER_MIN_INTERVAL  10    /*最小定时间隔10MS*/

typedef unsigned long  TIMERID;

/*-------------------------------------------------------*/
/*Name: high_pre_timer_call                              */
/*retu: NONE                                             */
/*desc: 基本定时器处理函数                                        */
/*-------------------------------------------------------*/
void high_pre_timer_call(void);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_dotick                            */
/*retu: void                                             */
/*desc: 计时单位到时                                           */
/*-------------------------------------------------------*/
void high_pre_timer_dotick(void);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_add                               */
/*Para: iType    -- 0 noloop 1 loop                      */
/*Para: pFunc    -- 超时执行函数                               */
/*Para: pArg    -- 函数参数                                  */
/*Para: iMillSec   -- 定时器的时间毫秒                           */
/*retu: 返回Timer ID，失败则返回 0                               */
/*desc: 创建定时器                                            */
/*-------------------------------------------------------*/

TIMERID high_pre_timer_add(const char *bName, int iType, int(*pFunc)(void *), void *pArg, int iMillSec);

/*-------------------------------------------------------*/
/*Name: VOS_TimerDelete                                  */
/*Para: ulTimerID  -- Timer Id                           */
/*retu: 成功0，失败则返回-1                                      */
/*desc: 删除定时器                                            */
/*-------------------------------------------------------*/
int high_pre_timer_delete(TIMERID lTimerId);


/*-------------------------------------------------------*/
/*Name: high_pre_timer_passtime                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- 定时器已经走过的时间                         */
/*retu: 成功0，失败则返回-1                                      */
/*desc: 获得定时器已经走过的时间                                     */
/*-------------------------------------------------------*/
int high_pre_timer_passtime(TIMERID lTimerId, int *plRet);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_remain                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- 定时器剩余时间               */
/*retu: 成功0，失败则返回-1                           */
/*desc: 获得定时器剩余时间                         */
/*-------------------------------------------------------*/
int high_pre_timer_remain(TIMERID lTimerId, int *plRet);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_init                              */
/*Para: iBasTimer  进程内所有定时器的最大公约数           ,单位    MS    */
/*Para: iModId     消息处理模块的ID                             */
/*Desc: 初始化进程的定时器                                        */
/*-------------------------------------------------------*/

int high_pre_timer_init(int iBasTimer, int iModId);

/*-------------------------------------------------------*/
/*Name: VOS_BaseTimerInit                                */
/*Para: iTimeval  基准定时器 单位ms                             */
/*retu: NONE                                             */
/*desc: 基本定时器初始化                                         */
/*-------------------------------------------------------*/
int high_pre_timer_start(void);

/*-------------------------------------------------------*/
/*Name: VOS_TimerInfoShow                                */
/*Para: iChanId    -- vty                                */
/*retu: 成功VOS_OK，失败则返回VOS_ERROR                          */
/*desc: 显示定时器信息                                          */
/*-------------------------------------------------------*/
int high_pre_timer_info(char *pOutBuf, int pDatLen);


#ifdef  __cplusplus
}
#endif

/*====================================================================================================================*/
#endif
