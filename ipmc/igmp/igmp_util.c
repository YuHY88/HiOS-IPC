/*
 * =====================================================================================
 *
 *       Filename:  igmp_util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/03/2017 05:01:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <zebra.h>
	  
#include "log.h"
	  
#include "igmp_util.h"
	  
/*
RFC 3376: 4.1.7. QQIC (Querier's Query Interval Code)

If QQIC < 128,	QQI = QQIC
If QQIC >= 128, QQI = (mant | 0x10) << (exp + 3)

0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|1| exp | mant	|
+-+-+-+-+-+-+-+-+

Since exp=0..7 then (exp+3)=3..10, then QQI has
one of the following bit patterns:

exp=0: QQI = 0000.0000.1MMM.M000
exp=1: QQI = 0000.0001.MMMM.0000
...
exp=6: QQI = 001M.MMM0.0000.0000
exp=7: QQI = 01MM.MM00.0000.0000
--------- ---------
0x4  0x0  0x0  0x0
*/

uint8_t igmp_msg_encode16to8(uint16_t value)
{
	uint16_t mask = 0x4000;
	uint8_t  exp;
	uint16_t mant;
	uint8_t code;

	if (value < 128) 
	{
	  code = value;
	}
  	else 
	{
	  	for (exp = 7; exp > 0; --exp) 
		{
		  	if (mask & value)
			{
			  break;
			}

		  	mask >>= 1;
		}
	  
	  	mant = 0x000F & (value >> (exp + 3));
	  	code = ((uint8_t) 1 << 7) | ((uint8_t) exp << 4) | (uint8_t) mant;
	}

  return code;
}
	  
/*
RFC 3376: 4.1.7. QQIC (Querier's Query Interval Code)

If QQIC < 128,	QQI = QQIC
If QQIC >= 128, QQI = (mant | 0x10) << (exp + 3)

0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|1| exp | mant	|
+-+-+-+-+-+-+-+-+
*/

uint16_t igmp_msg_decode8to16(uint8_t code)
{
	uint16_t value;
	uint16_t mant;
	uint8_t  exp;

	if (code < 128) 
	{
	  value = code;
	}
  	else 
  	{
	  	mant = (code & 0x0F);
	  	exp  = (code & 0x70) >> 4;
	  	value = (mant | 0x10) << (exp + 3);
  	}

  	return value;
}
	  
void igmp_pkt_dump(const char *label, const uint8_t *buf, int size)
{
	char dump_buf[1000];
	int i = 0;
	int j = 0;

  	for (; i < size; ++i, j += 2) 
	{
	  	int left = sizeof(dump_buf) - j;
	  	if (left < 4) 
	  	{
		  	if (left > 1) 
			{
			  strcat(dump_buf + j, "!"); /* mark as truncated */
			}
		  	break;
	 	}
	  
	  	snprintf(dump_buf + j, left, "%02x", buf[i]);
	}
}


