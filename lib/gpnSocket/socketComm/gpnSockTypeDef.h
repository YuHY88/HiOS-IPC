/**********************************************************
* file name: gpnSockTypeDef.h
* Copyright: 
	 Copyright 2012 huahuan.
* author: 
*    huahuan liuyf 2013-06-24
* function: 
*    system unity define
* modify:
*
***********************************************************/
#ifndef _GPN_SOCK_TYPE_DEF_H_
#define _GPN_SOCK_TYPE_DEF_H_

#ifdef    __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include <stdio.h>

#ifndef UINT8
#define UINT8 	unsigned char
#endif

#ifndef UINT16
#define UINT16	unsigned short
#endif

#ifndef UINT32
#define UINT32	unsigned int
#endif

#ifndef UINT64
#define UINT64  unsigned long long
#endif

#ifndef INT8
#define INT8 	char
#endif

#ifndef INT16
#define INT16	short
#endif

#ifndef INT32
#define INT32	int
#endif

#ifndef INT64
#define INT64   long long
#endif

#ifndef FLOAT
#define FLOAT   float
#endif

#ifndef PRAGMA_ATTR_PACKED
#define PRAGMA_ATTR_PACKED __attribute__ ((packed))
#endif

#define GPN_SOCK_SYS_OK 					1
#define GPN_SOCK_SYS_ERR					2

#define GPN_SOCK_SYS_YES 					1
#define GPN_SOCK_SYS_NO						2

#define GPN_SOCK_SYS_ENABLE 				1
#define GPN_SOCK_SYS_DISABLE				2

#define GPN_SOCK_SYS_ACTIVE 				1
#define GPN_SOCK_SYS_DEACTIVE 				2

#define GPN_SOCK_SYS_NULL					0

#define GPN_SOCK_08_NULL					0xed
#define GPN_SOCK_16_NULL					0xeeed
#define GPN_SOCK_32_NULL					0xeeeeeeed
/*#define GPN_SOCK_64_NULL					0xeeeeeeeeeeeeeeed*/

#define GPN_SOCK_08_FFFF					0xFF
#define GPN_SOCK_16_FFFF					0xFFFF
#define GPN_SOCK_32_FFFF					0xFFFFFFFF
/*#define GPN_SOCK_64_FFFF					0xFFFFFFFFFFFFFFFF*/

#define GEN_SOCK_SYS_AGP					1
#define GEN_SOCK_SYS_SVP					2
#define GEN_SOCK_SYS_CMP					3
#define GEN_SOCK_SYS_CUP					4

#if 1
#define GEN_SOCK_SYS_PRINT(level, info...)
#else
#define GEN_SOCK_SYS_PRINT(level, info...)	printf(info)
#endif

typedef struct _sockAliveMan_
{
	int fd;
	UINT32 toleratMis;
	UINT32 misCnt;
	UINT32 txFreq;
	UINT32 rxFreq;
}sockAliveMan;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _GPN_SOCK_TYPE_DEF_H_ */

