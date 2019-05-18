/*====================================================================================================================*/
/*------------------------------------------------------------------------*/
/* �� �� �� : hptimer.h                                                      */
/*  ��    �� : suxiqing                                                   */
/* �������� : 2006-08-05                                                  */
/* ��    Ȩ :                                                             */
/* ˵    �� : ���ļ������ṩVOS��Ķ�ʱ������ӿ�                         */
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

#define LIB_TIMER_MIN_INTERVAL  10    /*��С��ʱ���10MS*/

typedef unsigned long  TIMERID;

/*-------------------------------------------------------*/
/*Name: high_pre_timer_call                              */
/*retu: NONE                                             */
/*desc: ������ʱ��������                                        */
/*-------------------------------------------------------*/
void high_pre_timer_call(void);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_dotick                            */
/*retu: void                                             */
/*desc: ��ʱ��λ��ʱ                                           */
/*-------------------------------------------------------*/
void high_pre_timer_dotick(void);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_add                               */
/*Para: iType    -- 0 noloop 1 loop                      */
/*Para: pFunc    -- ��ʱִ�к���                               */
/*Para: pArg    -- ��������                                  */
/*Para: iMillSec   -- ��ʱ����ʱ�����                           */
/*retu: ����Timer ID��ʧ���򷵻� 0                               */
/*desc: ������ʱ��                                            */
/*-------------------------------------------------------*/

TIMERID high_pre_timer_add(const char *bName, int iType, int(*pFunc)(void *), void *pArg, int iMillSec);

/*-------------------------------------------------------*/
/*Name: VOS_TimerDelete                                  */
/*Para: ulTimerID  -- Timer Id                           */
/*retu: �ɹ�0��ʧ���򷵻�-1                                      */
/*desc: ɾ����ʱ��                                            */
/*-------------------------------------------------------*/
int high_pre_timer_delete(TIMERID lTimerId);


/*-------------------------------------------------------*/
/*Name: high_pre_timer_passtime                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- ��ʱ���Ѿ��߹���ʱ��                         */
/*retu: �ɹ�0��ʧ���򷵻�-1                                      */
/*desc: ��ö�ʱ���Ѿ��߹���ʱ��                                     */
/*-------------------------------------------------------*/
int high_pre_timer_passtime(TIMERID lTimerId, int *plRet);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_remain                          */
/*Para: ulTimerID  -- Timer Id                           */
/*Para: plRet      -- ��ʱ��ʣ��ʱ��               */
/*retu: �ɹ�0��ʧ���򷵻�-1                           */
/*desc: ��ö�ʱ��ʣ��ʱ��                         */
/*-------------------------------------------------------*/
int high_pre_timer_remain(TIMERID lTimerId, int *plRet);

/*-------------------------------------------------------*/
/*Name: high_pre_timer_init                              */
/*Para: iBasTimer  ���������ж�ʱ�������Լ��           ,��λ    MS    */
/*Para: iModId     ��Ϣ����ģ���ID                             */
/*Desc: ��ʼ�����̵Ķ�ʱ��                                        */
/*-------------------------------------------------------*/

int high_pre_timer_init(int iBasTimer, int iModId);

/*-------------------------------------------------------*/
/*Name: VOS_BaseTimerInit                                */
/*Para: iTimeval  ��׼��ʱ�� ��λms                             */
/*retu: NONE                                             */
/*desc: ������ʱ����ʼ��                                         */
/*-------------------------------------------------------*/
int high_pre_timer_start(void);

/*-------------------------------------------------------*/
/*Name: VOS_TimerInfoShow                                */
/*Para: iChanId    -- vty                                */
/*retu: �ɹ�VOS_OK��ʧ���򷵻�VOS_ERROR                          */
/*desc: ��ʾ��ʱ����Ϣ                                          */
/*-------------------------------------------------------*/
int high_pre_timer_info(char *pOutBuf, int pDatLen);


#ifdef  __cplusplus
}
#endif

/*====================================================================================================================*/
#endif
