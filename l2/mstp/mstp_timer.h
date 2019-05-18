/*
*  Copyright (C) 2016~2017  Beijing Huahuan Electronics Co., Ltd 
*
*  liufy@huahuan.com 
*
*  file name: mstp_timer.h
*
*  date: 2017.3
*
*  modify:
*
*/


#ifndef _MSTP_TIMER_H_
#define _MSTP_TIMER_H_
#include "lib/hptimer.h"


#define MSTP_TIMER_ON_LOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("mstp_timer",LIB_TIMER_TYPE_LOOP, (F), (S), (V)*1000); \
			} while (0)
		
#define MSTP_TIMER_ON_UNLOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("mstp_timer",LIB_TIMER_TYPE_NOLOOP, (F), (S), (V)*1000); \
			   } while (0)
			   
#define MSTP_TIMER_MSEC_ON_LOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("mstp_timer",LIB_TIMER_TYPE_LOOP, (F), (S), (V)); \
			} while (0)
						
#define MSTP_TIMER_MESC_ON_UNLOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("mstp_timer",LIB_TIMER_TYPE_NOLOOP, (F), (S), (V)); \
			} while (0)

#define MSTP_TIMER_OFF(T) \
			do { \
				if (T) \
				{ \
				 high_pre_timer_delete (T); \
				(T) = 0; \
				} \
			} while (0)


int mstp_timer_start(void * num);







#endif

