/*
 * =====================================================================================
 *
 *       Filename:  igmp_timer.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/05/2017 03:11:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <lib/zebra.h>
#include <lib/log.h>

#include "igmp_time.h"
 
static int gettime_monotonic(struct timeval *tv)
{
	int result;

	result = gettimeofday(tv, 0);
	if (result) 
	{
 		zlog_err("%s: gettimeofday() failure: errno=%d: %s",
	  		__FUNCTION__, errno, safe_strerror(errno));
	}

	return result;
}
 
/*
igmp_time_monotonic_sec():
number of seconds since some unspecified starting point
*/
int64_t igmp_time_monotonic_sec()
{
	struct timeval now_tv;
 
	if (gettime_monotonic(&now_tv)) 
	{
	 	zlog_err("%s: gettime_monotonic() failure: errno=%d: %s",
		  	__FUNCTION__, errno, safe_strerror(errno));
	 	return -1;
	}

	return now_tv.tv_sec;
}
 
/*
igmp_time_monotonic_dsec():
number of deciseconds since some unspecified starting point
*/
int64_t igmp_time_monotonic_dsec()
{
	struct timeval now_tv;
	int64_t now_dsec;

	if (gettime_monotonic(&now_tv)) 
	{
 		zlog_err("%s: gettime_monotonic() failure: errno=%d: %s",
	  		__FUNCTION__, errno, safe_strerror(errno));
 		return -1;
	}

	now_dsec = ((int64_t) now_tv.tv_sec) * 10 + ((int64_t) now_tv.tv_usec) / 100000;

	return now_dsec;
}

